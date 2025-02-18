#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "BP_SQLiteNode.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "K2Node_SQLiteNode.generated.h"

UCLASS()
class UK2Node_SQLiteNode : public UK2Node
{
    GENERATED_BODY()

public:
    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
    virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    virtual FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const override;

protected:
    UEdGraphPin* GetThenPin() const;
    UEdGraphPin* GetExecPin() const;
};