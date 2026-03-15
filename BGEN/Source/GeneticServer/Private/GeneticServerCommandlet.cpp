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

DEFINE_LOG_CATEGORY_STATIC(LogGeneticServer, Log, All);

UGeneticServerCommandlet::UGeneticServerCommandlet()
{
    // These flags tell the engine NOT to load worlds or standard game features
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
    
    // Default population size. You can expose this to a config file later.
    PopulationSize = 10;
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

void UGeneticServerCommandlet::GenerateNextEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Generating Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test"); // Your base tree

    for (int32 i = 0; i < PopulationSize; i++)
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
                    // 70% Crossover Chance
                    if (UGeneticSelectionLibrary::IsValidResult(ParentB) && FMath::FRand() < 0.7f) 
                    {
                        UCustomBehaviourTree* WrapperB = NewObject<UCustomBehaviourTree>();
                        if(WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
                        {
                             FString Log;
                             ChildWrapper = WrapperA->PerformCrossover(WrapperB, Log);
                        }
                    }
                    
                    // Fallback to clone if crossover skipped/failed
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
            if(!ChildWrapper->LoadBehaviorTree(SeedPath))
            {
                UE_LOG(LogGeneticServer, Error, TEXT("Failed to load Seed: %s"), *SeedPath);
                continue;
            }
        }

        // C. MUTATION
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
            // If Gen 0, mutate aggressively (1.0f) to build out the trees. Otherwise standard 40% chance.
            float MutRate = (CurrentGeneration == 0) ? 1.0f : 0.40f;
            int32 MutateTimes = (CurrentGeneration == 0) ? 5 : 1; 

            for(int k=0; k < MutateTimes; k++)
            {
                UGeneticMutationLibrary::MutateTree(ChildWrapper, MutRate);
            }

            // D. SAVE TO DISK & QUEUE JOB
            FString TreeHash = ChildWrapper->GetTreeHash();
            
            // Save using Generation and Hash
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            
            // SaveAsNewAsset writes the .uasset to the disk
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

            if (!FinalAssetPath.IsEmpty())
            {
                JobQueue.Add(FinalAssetPath);
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Job %d: %s"), i, *FinalAssetPath);
            }
        }
    }

    // Cleanup for next epoch
    CurrentEpochResults.Empty();
    CurrentGeneration++;
}

int32 UGeneticServerCommandlet::Main(const FString& Params)
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" Starting Headless Genetic Server Commandlet... "));
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    // 1. Initialize HTTP Server
    FModuleManager::LoadModuleChecked<FHttpServerModule>("HttpServer");
    auto& HttpServerModule = FHttpServerModule::Get();
    
    uint32 Port = 8080;
    TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(Port);

    if (!HttpRouter.IsValid())
    {
        UE_LOG(LogGeneticServer, Error, TEXT("Failed to initialize HTTP Router on port %d"), Port);
        return 1;
    }

    // 2. BIND ROUTES
    
    // --- ENDPOINT 1 - WORKER REQUESTS JOB ---
    HttpRouter->BindRoute(FHttpPath(TEXT("/api/request_job")), EHttpServerRequestVerbs::VERB_GET, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString AssignedAssetPath = GetNextJob();
            
            TSharedPtr<FJsonObject> JsonResponse = MakeShareable(new FJsonObject());
            if (!AssignedAssetPath.IsEmpty())
            {
                JsonResponse->SetStringField(TEXT("status"), TEXT("success"));
                JsonResponse->SetStringField(TEXT("asset_path"), AssignedAssetPath);
                UE_LOG(LogGeneticServer, Log, TEXT("Assigned job to worker: %s"), *AssignedAssetPath);
            }
            else
            {
                // Queue empty, tell worker to wait/standby
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
                
                // Record Result
                FSimulationResult Res;
                Res.BehaviorTreePath = AssetPath;
                Res.Fitness = FitnessValue;
                Res.GenerationNumber = CurrentGeneration - 1; // Since we incremented at the end of generation

                CurrentEpochResults.Add(Res);
                AllTimeResults.Add(Res);

                UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Worker finished %s | Fitness: %.2f | Progress: %d/%d"), 
                    *AssetPath, FitnessValue, CurrentEpochResults.Num(), PopulationSize);

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
    while (!IsEngineExitRequested())
    {
        // Sleep for 10ms to prevent 100% CPU usage
        FPlatformProcess::Sleep(0.01f); 

        double CurrentTime = FPlatformTime::Seconds();
        float DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;

        // Tick core engine systems required for async HTTP thread callbacks
        FTSTicker::GetCoreTicker().Tick(DeltaTime);
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
    }

    // 4. CLEANUP ON SHUTDOWN (Ctrl+C)
    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}