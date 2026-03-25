#include "GeneticServerCommandlet.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Containers/Ticker.h"
#include "Async/TaskGraphInterfaces.h"

// Includes required for the Genetic logic
#include "CustomBehaviourTree.h"
#include "GeneticSelectionLibrary.h"
#include "GeneticMutationLibrary.h"
#include "BehaviorTree/BehaviorTree.h"

DEFINE_LOG_CATEGORY_STATIC(LogGeneticServer, Log, All);

UGeneticServerCommandlet::UGeneticServerCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
    
    PopulationSize = 6;
    CurrentGeneration = 0;
}

FString UGeneticServerCommandlet::GetNextJob()
{
    if (JobQueue.Num() > 0)
    {
        FString Job = JobQueue[0];
        JobQueue.RemoveAt(0);
        return Job;
    }
    return TEXT(""); // Queue is empty
}

void UGeneticServerCommandlet::CheckForTimeouts()
{
    double CurrentTime = FPlatformTime::Seconds();
    TArray<int32> TimedOutJobIDs;

    // 1. Identify jobs that have exceeded the timeout limit
    for (const auto& Pair : ActiveJobs)
    {
        if (CurrentTime - Pair.Value.DispatchTime > JobTimeoutSeconds)
        {
            TimedOutJobIDs.Add(Pair.Key);
        }
    }

    // 2. Requeue them and remove from Active tracking
    for (int32 ID : TimedOutJobIDs)
    {
        FDispatchedJob JobToRequeue = ActiveJobs[ID];
        ActiveJobs.Remove(ID);

        JobQueue.Add(JobToRequeue.AssetPath);
        UE_LOG(LogGeneticServer, Error, TEXT("TIMEOUT: Job %d timed out after %.1f seconds! Re-adding %s to JobQueue."), 
            ID, JobTimeoutSeconds, *JobToRequeue.AssetPath);
    }
}

void UGeneticServerCommandlet::GenerateNextEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Generating Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test"); // Your base tree

    // 1. LOAD THE TEMPLATE ONCE OUTSIDE THE LOOP
    UCustomBehaviourTree* BaseSeedTemplate = NewObject<UCustomBehaviourTree>();
    if (!BaseSeedTemplate->LoadBehaviorTree(SeedPath))
    {
        UE_LOG(LogGeneticServer, Error, TEXT("CRITICAL: Failed to load Base Seed Tree at %s"), *SeedPath);
        return; 
    }

    // Cache the original hash so we know what a "pure" unmutated tree looks like
    FString BaseHash = BaseSeedTemplate->GetTreeHash();

    int32 JobsSuccessfullyQueued = 0;
    ActiveJobs.Empty(); 

    while (JobsSuccessfullyQueued < PopulationSize)
    {
        UCustomBehaviourTree* ChildWrapper = nullptr;

        // A. EVOLUTION (Gen > 0)
        if (CurrentGeneration > 0 && AllTimeResults.Num() > 0)
        {
            FSimulationResult ParentA = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);
            FSimulationResult ParentB = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);

            if (UGeneticSelectionLibrary::IsValidResult(ParentA))
            {
                UCustomBehaviourTree* WrapperA = NewObject<UCustomBehaviourTree>();
                if (WrapperA->LoadBehaviorTree(ParentA.BehaviorTreePath))
                {
                    if (UGeneticSelectionLibrary::IsValidResult(ParentB) && FMath::FRand() < 0.7f) 
                    {
                        UCustomBehaviourTree* WrapperB = NewObject<UCustomBehaviourTree>();
                        if(WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
                        {
                             FString Log;
                             ChildWrapper = WrapperA->PerformCrossover(WrapperB, Log);
                        }
                    }
                    
                    if (!ChildWrapper)
                    {
                        ChildWrapper = NewObject<UCustomBehaviourTree>();
                        ChildWrapper->InitFromTreeInstance(WrapperA->GetBTAsset());
                    }
                }
            }
        }

        // B. SEED FALLBACK (Gen 0 or Selection Failed)
        if (!ChildWrapper)
        {
            ChildWrapper = NewObject<UCustomBehaviourTree>();
            
            UBehaviorTree* SourceBT = BaseSeedTemplate->GetBTAsset();
            UBehaviorTree* DeepClone = DuplicateObject<UBehaviorTree>(SourceBT, GetTransientPackage());
            ChildWrapper->InitFromTreeInstance(DeepClone);
        }

        // C. MUTATION
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
            // === NEW LOGIC: GUARANTEED GEN 0 MUTATION ===
            if (CurrentGeneration == 0)
            {
                int32 Failsafe = 0;
                
                // Keep trying to mutate until the hash changes from the BaseHash
                // (Using a failsafe of 10 loops so the server doesn't freeze if the tree is mathematically un-mutable)
                while (ChildWrapper->GetTreeHash() == BaseHash && Failsafe < 10)
                {
                    // Pass 1.0 to guarantee a 100% chance of mutation attempt
                    UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                    Failsafe++;
                }

                if (Failsafe >= 10)
                {
                    UE_LOG(LogGeneticServer, Warning, TEXT("Failed to structurally mutate Gen 0 seed after 10 attempts! Moving on."));
                }
            }
            else
            {
                // Standard 40% chance for Generation 1 and beyond
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 0.4f);
            }
            
            // D. SAVE TO DISK & QUEUE JOB
            FString TreeHash = ChildWrapper->GetTreeHash();
            
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Index%d_%s"), CurrentGeneration, JobsSuccessfullyQueued, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

            if (!FinalAssetPath.IsEmpty())
            {
                JobQueue.Add(FinalAssetPath);
                UE_LOG(LogGeneticServer, Warning, TEXT("Queued Job %d: %s"), JobsSuccessfullyQueued, *FinalAssetPath);
                JobsSuccessfullyQueued++;
            }
            else
            {
                UE_LOG(LogGeneticServer, Error, TEXT("Failed to save generated tree! Retrying..."));
            }
        }
    }

    CurrentEpochResults.Empty();
    CurrentGeneration++;
}

int32 UGeneticServerCommandlet::Main(const FString& Params)
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" Starting Headless Genetic Server Commandlet... "));
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FModuleManager::LoadModuleChecked<FHttpServerModule>("HttpServer");
    auto& HttpServerModule = FHttpServerModule::Get();
    
    uint32 Port = 8080;
    TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(Port);

    if (!HttpRouter.IsValid())
    {
        UE_LOG(LogGeneticServer, Error, TEXT("Failed to initialize HTTP Router on port %d"), Port);
        return 1;
    }

    // --- ENDPOINT 1 - WORKER REQUESTS JOB ---
    HttpRouter->BindRoute(FHttpPath(TEXT("/api/request_job")), EHttpServerRequestVerbs::VERB_GET, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString AssignedAssetPath = GetNextJob();
            
            TSharedPtr<FJsonObject> JsonResponse = MakeShareable(new FJsonObject());
            if (!AssignedAssetPath.IsEmpty())
            {
                int32 AssignedJobID = NextJobID++;
                
                JsonResponse->SetStringField(TEXT("status"), TEXT("success"));
                JsonResponse->SetStringField(TEXT("asset_path"), AssignedAssetPath);
                JsonResponse->SetNumberField(TEXT("job_id"), AssignedJobID);
                
                // TRACK THE JOB DISPATCH TIME
                FDispatchedJob NewJob;
                NewJob.AssetPath = AssignedAssetPath;
                NewJob.DispatchTime = FPlatformTime::Seconds();
                ActiveJobs.Add(AssignedJobID, NewJob);

                UE_LOG(LogGeneticServer, Log, TEXT("Assigned job %d to worker: %s"), AssignedJobID, *AssignedAssetPath);
            }
            else
            {
                JsonResponse->SetStringField(TEXT("status"), TEXT("standby"));
            }

            FString ResponseString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
            FJsonSerializer::Serialize(JsonResponse.ToSharedRef(), Writer);

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ResponseString, TEXT("application/json"));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    // --- ENDPOINT 2 - WORKER SUBMITS FITNESS ---
    HttpRouter->BindRoute(FHttpPath(TEXT("/api/submit")), EHttpServerRequestVerbs::VERB_POST, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString BodyString(Request.Body.Num(), UTF8_TO_TCHAR((const char*)Request.Body.GetData()));
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyString);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                FString AssetPath = JsonObject->GetStringField(TEXT("asset_path"));
                float FitnessValue = JsonObject->GetNumberField(TEXT("fitness"));
                int32 CurrentGenForLog = CurrentGeneration - 1; 
                int32 JobID = JsonObject->GetIntegerField(TEXT("job_id"));

                // UN-TRACK THE JOB (Worker successfully finished it)
                ActiveJobs.Remove(JobID);

                FSimulationResult Res;
                Res.BehaviorTreePath = AssetPath;
                Res.Fitness = FitnessValue;
                Res.GenerationNumber = CurrentGenForLog; 

                CurrentEpochResults.Add(Res);
                AllTimeResults.Add(Res);

                // Extended Metrics for the CSV
                float Dist = JsonObject->GetNumberField(TEXT("distance"));
                float DmgTaken = JsonObject->GetNumberField(TEXT("damage_taken"));
            	float DmgDealt = JsonObject->GetNumberField(TEXT("damage_dealt"));
                float Rew = JsonObject->GetNumberField(TEXT("reward"));
                float TimeAlive = JsonObject->GetNumberField(TEXT("time_alive"));
                int32 TreeSize = JsonObject->GetIntegerField(TEXT("tree_size"));
                float Util = JsonObject->GetNumberField(TEXT("utilization"));
                bool bKilled = JsonObject->GetBoolField(TEXT("player_killed"));
                FString TreeStr = JsonObject->GetStringField(TEXT("tree_string"));

                UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Worker finished Job %d | Fitness: %.2f | Progress: %d/%d"), 
                    JobID, FitnessValue, CurrentEpochResults.Num(), PopulationSize);

                // Write to CSV
                FString CSVPath = FPaths::ProjectLogDir() / TEXT("GeneticMasterResults.csv");
                bool bFileExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*CSVPath);
                FString CSVLine;

                if (!bFileExists)
                {
                    CSVLine += TEXT("JobID,Generation,Fitness,Distance,DamageTaken,DamageDealt,Reward,TimeAlive,TreeSize,Utilization,PlayerKilled,AssetPath,TreeStructure\n");
                }

                FString EscapedTree = TreeStr.Replace(TEXT("\""), TEXT("\"\""));

                CSVLine += FString::Printf(TEXT("%d,%d,%f,%f,%f,%f,%f,%f,%d,%f,%s,%s,\"%s\"\n"),
                    JobID, CurrentGenForLog, FitnessValue, Dist, DmgTaken,DmgDealt, Rew, TimeAlive, TreeSize, Util, 
                    bKilled ? TEXT("TRUE") : TEXT("FALSE"), *AssetPath, *EscapedTree);

                FFileHelper::SaveStringToFile(CSVLine, *CSVPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

                // CHECK EPOCH COMPLETION
                if (CurrentEpochResults.Num() >= PopulationSize)
                {
                    GenerateNextEpoch(); 
                }
            }

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"success\"}")), TEXT("application/json"));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    HttpServerModule.StartAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server listening on port %d"), Port);

    // --- BOOTSTRAP GENERATION 0 ---
    GenerateNextEpoch();

    // 3. INFINITE LOOP TO KEEP PROCESS ALIVE
    double LastTime = FPlatformTime::Seconds();
    double LastTimeoutCheckTime = FPlatformTime::Seconds();

    while (!IsEngineExitRequested())
    {
        // Sleep for 10ms to prevent 100% CPU usage
        FPlatformProcess::Sleep(0.01f); 

        double CurrentTime = FPlatformTime::Seconds();
        float DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;

        // Check for timed out jobs every 5 seconds
        if (CurrentTime - LastTimeoutCheckTime > 5.0)
        {
            CheckForTimeouts();
            LastTimeoutCheckTime = CurrentTime;
        }

        // Tick core engine systems required for async HTTP thread callbacks
        FTSTicker::GetCoreTicker().Tick(DeltaTime);
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
    }

    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}