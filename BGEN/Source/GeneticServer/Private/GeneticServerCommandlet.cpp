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
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = false;
	
    PopulationSize = 2;
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
    if (CurrentGeneration == 0)
    {
        GenerateInitialEpoch();
    }
	GenerateSubsequentEpoch();
}

void UGeneticServerCommandlet::GenerateInitialEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Generating Initial Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test"); // Your base tree
	
    UCustomBehaviourTree* SeedLoader = NewObject<UCustomBehaviourTree>();
    if (!SeedLoader->LoadBehaviorTree(SeedPath))
    {
        UE_LOG(LogGeneticServer, Error, TEXT("Failed to load Seed: %s"), *SeedPath);
        return;
    }

    for (int32 i = 0; i < PopulationSize; i++)
    {

        UCustomBehaviourTree* ChildWrapper = NewObject<UCustomBehaviourTree>();
    	
        ChildWrapper->InitFromTreeInstance(SeedLoader->GetBTAsset());

        if (ChildWrapper->GetBTAsset())
        {
            for (int k = 0; k < 3; k++)
            {
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                ChildWrapper->DebugLogTree(LogGeneticServer);
            }
        	
            FString TreeHash = ChildWrapper->GetTreeHash();
            
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

            if (!FinalAssetPath.IsEmpty())
            {
                JobQueue.Add(FinalAssetPath);
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Initial Job %d: %s"), i, *FinalAssetPath);
            }
        }
    }

	CurrentEpochResults.Empty();
	CurrentGeneration++;
}

void UGeneticServerCommandlet::GenerateSubsequentEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Generating Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test");

    for (int32 i = 0; i < PopulationSize; i++)
    {
        UCustomBehaviourTree* ChildWrapper = nullptr;
        
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
                    if (WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
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

        // --- NEW SEED FALLBACK ---
        // If selection completely failed (e.g., Parent A was invalid) or the asset was missing, 
        // fallback to the Seed to ensure we don't lose an individual in this epoch.
        if (!ChildWrapper)
        {
            UCustomBehaviourTree* SeedLoader = NewObject<UCustomBehaviourTree>();
            if (SeedLoader->LoadBehaviorTree(SeedPath))
            {
                ChildWrapper = NewObject<UCustomBehaviourTree>();
                ChildWrapper->InitFromTreeInstance(SeedLoader->GetBTAsset());
            }
            else
            {
                UE_LOG(LogGeneticServer, Error, TEXT("CRITICAL: Failed to load fallback Seed: %s"), *SeedPath);
                continue; // Critical failure, skip this individual
            }
        }

        // C. MUTATION
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
            // Standard 40% chance to mutate once for subsequent generations
            UGeneticMutationLibrary::MutateTree(ChildWrapper, 0.40f);

            // D. SAVE TO DISK & QUEUE JOB
            FString TreeHash = ChildWrapper->GetTreeHash();
            
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);
            
            ChildWrapper->DebugLogTree(LogGeneticServer);
            UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

            if (!FinalAssetPath.IsEmpty())
            {
                JobQueue.Add(FinalAssetPath);
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Evolved Job %d: %s"), i, *FinalAssetPath);
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