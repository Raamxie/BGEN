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
	InitialMutationCount = 3;
    CurrentGeneration = 0;
	
	CrossoverChance = 0.7f;
	MutationChance = 0.5f;
	
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
    	return;
    }
	GenerateSubsequentEpoch();
}

void UGeneticServerCommandlet::GenerateInitialEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Generating Initial Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test"); // Your base tree
	
    UCustomBehaviourTree* SeedLoader = NewObject<UCustomBehaviourTree>(this);
    if (!SeedLoader->LoadBehaviorTree(SeedPath))
    {
        UE_LOG(LogGeneticServer, Error, TEXT("Failed to load Seed: %s"), *SeedPath);
        return;
    }

    for (int32 i = 0; i < PopulationSize; i++)
    {
        UCustomBehaviourTree* ChildWrapper = NewObject<UCustomBehaviourTree>(this);
    	
        ChildWrapper->InitFromTreeInstance(SeedLoader->GetBTAsset());

        if (ChildWrapper->GetBTAsset())
        {
            for (int k = 0; k < InitialMutationCount; k++)
            {
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
            }
        	
            FString TreeHash = ChildWrapper->GetTreeHash();
        	
            int32 RetryCount = 0;
            int32 MaxRetries = 10;
            

            while (EvaluatedHashes.Contains(TreeHash) && RetryCount < MaxRetries)
            {
                UE_LOG(LogGeneticServer, Warning, TEXT("Duplicate Hash Found: %s. Forcing mutation..."), *TreeHash);
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                TreeHash = ChildWrapper->GetTreeHash();
                RetryCount++;
            }
        	
            EvaluatedHashes.Add(TreeHash);
            
            ChildWrapper->DebugLogTree(LogGeneticServer);
            
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

    for (int32 i = 0; i < PopulationSize; i++)
    {
        UCustomBehaviourTree* ChildWrapper = nullptr;
        
        FSimulationResult ParentA = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);
        FSimulationResult ParentB = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);

        if (UGeneticSelectionLibrary::IsValidResult(ParentA))
        {
            UCustomBehaviourTree* WrapperA = NewObject<UCustomBehaviourTree>(this);
            if (WrapperA->LoadBehaviorTree(ParentA.BehaviorTreePath))
            {
            	if (UGeneticSelectionLibrary::IsValidResult(ParentB) && FMath::FRand() < CrossoverChance) 
            	{
            		UCustomBehaviourTree* WrapperB = NewObject<UCustomBehaviourTree>(this);
            		if (WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
            		{
            			ChildWrapper = WrapperA->PerformCrossover(WrapperB, LogGeneticServer);
            		}
            	}
            	
                if (!ChildWrapper)
                {
                    ChildWrapper = NewObject<UCustomBehaviourTree>(this);
                    ChildWrapper->InitFromTreeInstance(WrapperA->GetBTAsset());
                }
            }
        }
    	UE_LOG(LogGeneticServer, Warning, TEXT("Selection Done"));
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
        	UE_LOG(LogGeneticServer, Warning, TEXT("Got into Mutation"));
            UGeneticMutationLibrary::MutateTree(ChildWrapper, MutationChance);

            FString TreeHash = ChildWrapper->GetTreeHash();
        	
            int32 RetryCount = 0;
            int32 MaxRetries = 10;
        	
            while (EvaluatedHashes.Contains(TreeHash) && RetryCount < MaxRetries)
            {
                UE_LOG(LogGeneticServer, Warning, TEXT("Duplicate Hash Found: %s. Forcing mutation..."), *TreeHash);
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                TreeHash = ChildWrapper->GetTreeHash();
                RetryCount++;
            }
        	
            EvaluatedHashes.Add(TreeHash);
        	
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

            if (!FinalAssetPath.IsEmpty())
            {
                JobQueue.Add(FinalAssetPath);
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Evolved Job %d: %s"), i, *FinalAssetPath);
            }
        }
    	UE_LOG(LogGeneticServer, Warning, TEXT("Mutation Done"));
    	ChildWrapper->DebugLogTree(LogGeneticServer);
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

	FString CSVFilePath = FPaths::ProjectSavedDir() / TEXT("GeneticResults.csv");
	FString CSVHeader = TEXT("Generation,JobID,BehaviorTreePath,Fitness,Distance,DamageTaken,DamageDealt,Reward,TimeAlive,TreeSize,Utilization,PlayerKilled,TreeString\n");

    // 2. BIND ROUTES
    
    // --- ENDPOINT 1 - WORKER REQUESTS JOB ---
	HttpRouter->BindRoute(FHttpPath(TEXT("/api/request_job")), EHttpServerRequestVerbs::VERB_GET, 
			FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
			{
				FString AssignedAssetPath = GetNextJob();
            
				TSharedPtr<FJsonObject> JsonResponse = MakeShareable(new FJsonObject());
				if (!AssignedAssetPath.IsEmpty())
				{
					// 1. Grab the current ID and increment it for the next job
					int32 AssignedJobID = NextJobID++;

					JsonResponse->SetStringField(TEXT("status"), TEXT("success"));
					JsonResponse->SetStringField(TEXT("asset_path"), AssignedAssetPath);
                
					// 2. Send the Job ID to the worker
					JsonResponse->SetNumberField(TEXT("job_id"), AssignedJobID);

					UE_LOG(LogGeneticServer, Log, TEXT("Assigned Job %d to worker: %s"), AssignedJobID, *AssignedAssetPath);
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
        FHttpRequestHandler::CreateLambda([this, CSVFilePath](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString BodyString(Request.Body.Num(), UTF8_TO_TCHAR((const char*)Request.Body.GetData()));
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyString);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                // 1. Parse JSON
                FString AssetPath = JsonObject->GetStringField(TEXT("asset_path"));
                int32 JobID = JsonObject->GetIntegerField(TEXT("job_id"));
                float FitnessValue = JsonObject->GetNumberField(TEXT("fitness"));
                float Distance = JsonObject->GetNumberField(TEXT("distance"));
                float DamageTaken = JsonObject->GetNumberField(TEXT("damage_taken"));
                float DamageDealt = JsonObject->GetNumberField(TEXT("damage_dealt"));
                float Reward = JsonObject->GetNumberField(TEXT("reward"));
                float TimeAlive = JsonObject->GetNumberField(TEXT("time_alive"));
                int32 TreeSize = JsonObject->GetIntegerField(TEXT("tree_size"));
                float Utilization = JsonObject->GetNumberField(TEXT("utilization"));
                bool bPlayerKilled = JsonObject->GetBoolField(TEXT("player_killed"));
                FString RawTreeString = JsonObject->GetStringField(TEXT("tree_string"));

                int32 CompletedGeneration = CurrentGeneration - 1;

                // 2. SANITIZE FOR MULTI-LINE CSV CELL
                // First, escape any existing double-quotes by turning them into double-double-quotes ("")
                FString EscapedTreeString = RawTreeString.Replace(TEXT("\""), TEXT("\"\""));
                // We no longer replace \n or commas, so the tree's original layout is preserved!

                // 3. Format CSV Row
                // CRITICAL: Notice the \"%s\" at the very end. This wraps the EscapedTreeString in quotes.
                FString CSVRow = FString::Printf(TEXT("%d,%d,%s,%f,%f,%f,%f,%f,%f,%d,%f,%s,\"%s\"\n"), 
                    CompletedGeneration,
                    JobID,
                    *AssetPath, 
                    FitnessValue,
                    Distance,
                    DamageTaken,
                    DamageDealt,
                    Reward,
                    TimeAlive,
                    TreeSize,
                    Utilization,
                    bPlayerKilled ? TEXT("True") : TEXT("False"),
                    *EscapedTreeString);

                // 4. Append Directly to File
                FFileHelper::SaveStringToFile(CSVRow, *CSVFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
                
                // 5. Keep only the minimal required struct in memory for Genetic Algorithm Operations
                FSimulationResult Res;
                Res.BehaviorTreePath = AssetPath;
                Res.Fitness = FitnessValue;
                Res.GenerationNumber = CompletedGeneration; 

                CurrentEpochResults.Add(Res);
                AllTimeResults.Add(Res);

                UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Worker finished Job %d (%s) | Fitness: %.2f | Progress: %d/%d"), 
                    JobID, *AssetPath, FitnessValue, CurrentEpochResults.Num(), PopulationSize);

                // 6. CHECK EPOCH COMPLETION
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