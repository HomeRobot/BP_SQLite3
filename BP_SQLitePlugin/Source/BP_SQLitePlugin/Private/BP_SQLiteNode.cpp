#include "BP_SQLiteNode.h"
#include "SQLiteDatabase.h"
#include "Async/Async.h"

UBP_SQLiteNode::UBP_SQLiteNode()
   : Super()
{
   PrimaryComponentTick.bCanEverTick = false;
   Database = new FSQLiteDatabase();
}

UBP_SQLiteNode::~UBP_SQLiteNode()
{
   CloseDatabaseConnection();
   if(Database)
   {
       delete Database;
       Database = nullptr;
   }
}

void UBP_SQLiteNode::BeginPlay()
{
   Super::BeginPlay();
}

void UBP_SQLiteNode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay(EndPlayReason);
   CloseDatabaseConnection();
}

void UBP_SQLiteNode::ConnectToDatabase(const FString& DatabasePath, const FDatabaseConnectResult& Callback)
{
   AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, DatabasePath, Callback]()
   {
       FScopeLock Lock(&DatabaseLock);
       bool Success = false;
       
       if (Database != nullptr)
       {
           Success = Database->Open(*DatabasePath, ESQLiteDatabaseOpenMode::ReadWriteCreate);
           if (!Success)
           {
               UE_LOG(LogTemp, Error, TEXT("Failed to open database at path: %s"), *DatabasePath);
           }
       }
       
       AsyncTask(ENamedThreads::GameThread, [Callback, Success]()
       {
           Callback.ExecuteIfBound(Success);
       });
   });
}

void UBP_SQLiteNode::ExecuteQuery(
   const FString& Query,
   const TMap<FString, FString>& Parameters,
   const FQueryResult& Callback,
   bool IsInsertOrUpdate)
{
   AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Query, Parameters, Callback, IsInsertOrUpdate]()
   {
       FScopeLock Lock(&DatabaseLock);
       FString Result;
       int32 AffectedRows = 0;
       TArray<FQueryResultRow> Rows;

       if (Database == nullptr || !Database->IsValid())
       {
           AsyncTask(ENamedThreads::GameThread, [Callback]()
           {
               Callback.ExecuteIfBound(TEXT("Database not connected."), TArray<FQueryResultRow>(), 0);
           });
           return;
       }

       // Создаем prepared statement
       FSQLitePreparedStatement Statement = Database->PrepareStatement(*Query);
       if (!Statement.IsValid())
       {
           AsyncTask(ENamedThreads::GameThread, [Callback]()
           {
               Callback.ExecuteIfBound(TEXT("Failed to prepare statement."), TArray<FQueryResultRow>(), 0);
           });
           return;
       }

       // Привязка параметров
       for (const auto& Param : Parameters)
       {
           if (!Statement.SetBindingValueByName(*Param.Key, *Param.Value))
           {
               FString ErrorMsg = FString::Printf(TEXT("Failed to bind parameter: %s"), *Param.Key);
               AsyncTask(ENamedThreads::GameThread, [Callback, ErrorMsg]()
               {
                   Callback.ExecuteIfBound(ErrorMsg, TArray<FQueryResultRow>(), 0);
               });
               return;
           }
       }

       if (IsInsertOrUpdate)
       {
           if (!Statement.Execute())
           {
               Result = TEXT("Failed to execute insert/update query.");
           }
           else
           {
               AffectedRows = Database->GetLastInsertRowId();
               Result = FString::Printf(TEXT("%d rows affected."), AffectedRows);
           }

           AsyncTask(ENamedThreads::GameThread, [Callback, Result, Rows, AffectedRows]()
           {
               Callback.ExecuteIfBound(Result, Rows, AffectedRows);
           });
       }
       else
       {
           Statement.Execute([&Rows](const FSQLitePreparedStatement& Stmt) 
           {
               FQueryResultRow Row;
               for (const FString& Column : Stmt.GetColumnNames())
               {
                   FString Value;
                   if (Stmt.GetColumnValueByName(*Column, Value))
                   {
                       Row.Values.Add(Column, Value);
                   }
               }
               Rows.Add(Row);
               return ESQLitePreparedStatementExecuteRowResult::Continue;
           });

           Result = FString::Printf(TEXT("Query executed successfully. Retrieved %d rows."), Rows.Num());

           AsyncTask(ENamedThreads::GameThread, [Callback, Result, Rows, AffectedRows]()
           {
               Callback.ExecuteIfBound(Result, Rows, AffectedRows);
           });
       }
   });
}

void UBP_SQLiteNode::CloseDatabaseConnection()
{
   FScopeLock Lock(&DatabaseLock);
   if (Database != nullptr)
   {
       Database->Close();
   }
}