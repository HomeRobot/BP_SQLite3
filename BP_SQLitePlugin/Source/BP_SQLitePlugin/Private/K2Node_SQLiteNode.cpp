#include "K2Node_SQLiteNode.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Kismet/KismetStringLibrary.h"  // Добавили правильное включение

#define LOCTEXT_NAMESPACE "K2Node_SQLiteNode"

void UK2Node_SQLiteNode::AllocateDefaultPins()
{
    // Создаем исполняющие пины
    FEdGraphPinType ExecPinType;
    ExecPinType.PinCategory = UEdGraphSchema_K2::PC_Exec;
    
    CreatePin(EGPD_Input, ExecPinType, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, ExecPinType, UEdGraphSchema_K2::PN_Then);
    CreatePin(EGPD_Output, ExecPinType, TEXT("OnError"));

    // Входные параметры
    FEdGraphPinType StringPinType;
    StringPinType.PinCategory = UEdGraphSchema_K2::PC_String;
    CreatePin(EGPD_Input, StringPinType, TEXT("Query"));

    // Для Map
    FEdGraphPinType MapPinType;
    MapPinType.PinCategory = UEdGraphSchema_K2::PC_String;
    MapPinType.ContainerType = EPinContainerType::Map;
    CreatePin(EGPD_Input, MapPinType, TEXT("Parameters"));

    // Boolean
    FEdGraphPinType BoolPinType;
    BoolPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
    CreatePin(EGPD_Input, BoolPinType, TEXT("IsInsertOrUpdate"));

    // Для массива FQueryResultRow
    FEdGraphPinType RowsPinType;
    RowsPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
    RowsPinType.PinSubCategoryObject = FQueryResultRow::StaticStruct();
    RowsPinType.ContainerType = EPinContainerType::Array;
    CreatePin(EGPD_Output, RowsPinType, TEXT("Rows"));

    // Int
    FEdGraphPinType IntPinType;
    IntPinType.PinCategory = UEdGraphSchema_K2::PC_Int;
    CreatePin(EGPD_Output, IntPinType, TEXT("AffectedRows"));
}

FText UK2Node_SQLiteNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("SQLiteNode_Title", "Execute SQLite Query");
}

FText UK2Node_SQLiteNode::GetTooltipText() const
{
    return LOCTEXT("SQLiteNode_Tooltip", "Executes an SQLite query with parameters and returns results");
}

void UK2Node_SQLiteNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);
        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FSlateIcon UK2Node_SQLiteNode::GetIconAndTint(FLinearColor& OutColor) const
{
    static FSlateIcon Icon("EditorStyle", "GraphEditor.Database_16x");
    OutColor = FLinearColor(0.7f, 0.7f, 0.7f);
    return Icon;
}

UEdGraphPin* UK2Node_SQLiteNode::GetExecPin() const
{
    UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Execute);
    check(Pin == nullptr || Pin->Direction == EGPD_Input);
    return Pin;
}

void UK2Node_SQLiteNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    UEdGraphPin* ExecPin = GetExecPin();
    UEdGraphPin* ThenPin = GetThenPin();
    UEdGraphPin* ErrorPin = FindPin(TEXT("OnError"));
    
    if (ExecPin && ThenPin && ErrorPin)
    {
        // Создаем ноду вызова функции ExecuteQuery
        UK2Node_CallFunction* ExecuteQueryNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
        ExecuteQueryNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UBP_SQLiteNode, ExecuteQuery), UBP_SQLiteNode::StaticClass());
        ExecuteQueryNode->AllocateDefaultPins();

        // Соединяем пины
        CompilerContext.MovePinLinksToIntermediate(*ExecPin, *ExecuteQueryNode->GetExecPin());

        // Копируем параметры
        for (UEdGraphPin* Pin : Pins)
        {
            if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
            {
                UEdGraphPin* DestPin = ExecuteQueryNode->FindPin(Pin->PinName);
                if (DestPin)
                {
                    CompilerContext.MovePinLinksToIntermediate(*Pin, *DestPin);
                }
            }
        }

        // Помечаем ноду как удаленную
        BreakAllNodeLinks();
    }
}

UEdGraphPin* UK2Node_SQLiteNode::GetThenPin() const
{
    UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
    check(Pin == nullptr || Pin->Direction == EGPD_Output);
    return Pin;
}


FNodeHandlingFunctor* UK2Node_SQLiteNode::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
    return new FNodeHandlingFunctor(CompilerContext);
}

#undef LOCTEXT_NAMESPACE