#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BP_SQLiteNode.generated.h"

class FSQLiteDatabase;  // Форвард-декларация

USTRUCT(BlueprintType)
struct FQueryResultRow
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite")
    TMap<FString, FString> Values;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FDatabaseConnectResult, bool, Success);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FQueryResult, const FString&, Result, const TArray<FQueryResultRow>&, Rows, int32, AffectedRows);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BP_SQLITEPLUGIN_API UBP_SQLiteNode : public UActorComponent
{
    GENERATED_BODY()

public:    
    UBP_SQLiteNode();
    virtual ~UBP_SQLiteNode();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "SQLite")
    void ConnectToDatabase(const FString& DatabasePath, const FDatabaseConnectResult& Callback);
    
    UFUNCTION(BlueprintCallable, Category = "SQLite")
    void ExecuteQuery(const FString& Query, 
                     const TMap<FString, FString>& Parameters,
                     const FQueryResult& Callback,
                     bool IsInsertOrUpdate);

    UFUNCTION(BlueprintCallable, Category = "SQLite")
    void CloseDatabaseConnection();

protected:
    FSQLiteDatabase* Database;
    FCriticalSection DatabaseLock;
};