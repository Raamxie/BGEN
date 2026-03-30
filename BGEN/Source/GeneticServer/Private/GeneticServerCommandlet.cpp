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
	
    PopulationSize = 10;
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
        		// [FIXED] Queue the initial generation K times for multi-trial evaluation
        		for (int32 k = 0; k < TrialsPerGenome; k++)
        		{
        			JobQueue.Add(FinalAssetPath);
        		}
                
        		UE_LOG(LogGeneticServer, Log, TEXT("Queued Initial Job %d (%d trials): %s"), i, TrialsPerGenome, *FinalAssetPath);
        	}
        }
    }

	CurrentEpochResults.Empty();
	CurrentGeneration++;
}

void UGeneticServerCommandlet::ProcessCompletedEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Processing Completed Epoch %d"), CurrentGeneration);
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    if (SurvivingPopulation.Num() == 0)
    {
        // Gen 0: Just evaluate the initial random population, no parents to compete against.
        TArray<TArray<FSimulationResult>> Fronts = UGeneticSelectionLibrary::FastNonDominatedSort(CurrentEpochResults, 5);
        for (const TArray<FSimulationResult>& Front : Fronts) SurvivingPopulation.Append(Front);
    }
    else
    {
        // Gen > 0: The (μ+λ) ES. Combine surviving parents (μ) + evaluated offspring (λ)
        TArray<FSimulationResult> CombinedPool;
        CombinedPool.Append(SurvivingPopulation);
        CombinedPool.Append(CurrentEpochResults);

        TArray<TArray<FSimulationResult>> Fronts = UGeneticSelectionLibrary::FastNonDominatedSort(CombinedPool, 5);
        SurvivingPopulation.Empty();

        for (TArray<FSimulationResult>& Front : Fronts)
        {
            if (SurvivingPopulation.Num() + Front.Num() <= PopulationSize)
            {
                // Entire front fits
                SurvivingPopulation.Append(Front);
            }
            else
            {
                // Cutoff front: Use Priority Tie-Breaking to fill the exact remaining slots
                int32 NeededCount = PopulationSize - SurvivingPopulation.Num();
                // Start tie-breaking by dropping the 5th objective (passing 4 active objectives)
                TArray<FSimulationResult> Selected = UGeneticSelectionLibrary::PriorityTieBreakingSelection(Front, NeededCount, 4); 
                SurvivingPopulation.Append(Selected);
                break; // We have exactly 'PopulationSize' individuals now
            }
        }
    }

    // --- CSV LOGGING ---
    FString CSVData;
    for (const FSimulationResult& Res : SurvivingPopulation)
    {
    	CSVData += FString::Printf(TEXT("%d,%s,%d,%f,%f,%f,%f,%d,%f,%d,\"%s\"\n"),
            CurrentGeneration,
            *Res.TreeHash,
            Res.FrontIndex,
            Res.DamageDealt,
            Res.DamageTaken,
            Res.DistanceTravelled,
            Res.TreeUtilization,
            FMath::RoundToInt(Res.TreeSize),
            Res.Fitness, 
            Res.TrialsCompleted,
            *Res.TreeString
        );
    }
    FFileHelper::SaveStringToFile(CSVData, *CSVFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);

    // --- PREPARE NEXT OFFSPRING BATCH ---
    CurrentEpochResults.Empty();
    CurrentGeneration++;

    // Generate the λ offspring pool for the next evaluation phase
    GenerateSubsequentEpoch();
}

void UGeneticServerCommandlet::GenerateSubsequentEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Spawning Offspring for Epoch %d"), CurrentGeneration);

    for (int32 i = 0; i < PopulationSize; i++)
    {
        UCustomBehaviourTree* ChildWrapper = nullptr;
        
        // Select from the strictly culled μ SurvivingPopulation
        FSimulationResult ParentA = UGeneticSelectionLibrary::TournamentSelection(SurvivingPopulation, 3);
        FSimulationResult ParentB = UGeneticSelectionLibrary::TournamentSelection(SurvivingPopulation, 3);

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
        
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
            UGeneticMutationLibrary::MutateTree(ChildWrapper, MutationChance);

            FString TreeHash = ChildWrapper->GetTreeHash();
            int32 RetryCount = 0;
            
            // Note: In Multi-Objective, duplicate checking should span the SurvivingPopulation hashes as well
            while (EvaluatedHashes.Contains(TreeHash) && RetryCount < 10)
            {
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                TreeHash = ChildWrapper->GetTreeHash();
                RetryCount++;
            }
        	
            EvaluatedHashes.Add(TreeHash);
            
            // Enqueue exactly 'TrialsPerGenome' times for multi-trial evaluation
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

        	ChildWrapper->DebugLogTree(LogGeneticServer);
            if (!FinalAssetPath.IsEmpty())
            {
                for (int32 k = 0; k < TrialsPerGenome; k++)
                {
                    JobQueue.Add(FinalAssetPath);
                }
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Evolved Job %d (%d trials): %s"), i, TrialsPerGenome, *FinalAssetPath);
            }
        }
    }
}

void UGeneticServerCommandlet::CheckForTimeouts()
{
	double CurrentTime = FPlatformTime::Seconds();
	TArray<int32> TimedOutJobIDs;

	// 1. Find all jobs that have exceeded the timeout limit
	for (const auto& Kvp : ActiveJobs)
	{
		if ((CurrentTime - Kvp.Value.DispatchTime) > JobTimeoutSeconds)
		{
			TimedOutJobIDs.Add(Kvp.Key);
		}
	}

	// 2. Process timeouts (Remove from Active, push back to Queue)
	for (int32 JobID : TimedOutJobIDs)
	{
		FDispatchedJob TimedOutJob = ActiveJobs[JobID];
		ActiveJobs.Remove(JobID);
        
		// Push it back to the end of the queue for another worker to pick up
		JobQueue.Add(TimedOutJob.AssetPath);
        
		UE_LOG(LogGeneticServer, Error, TEXT("MASTER: Job %d (%s) TIMED OUT after %f seconds! Re-adding to queue."), 
			JobID, *TimedOutJob.AssetPath, JobTimeoutSeconds);
	}
}

int32 UGeneticServerCommandlet::Main(const FString& Params)
{
	UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
	UE_LOG(LogGeneticServer, Warning, TEXT(" Starting Headless Genetic Server Commandlet... "));
	UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
	
	// Define the leading row with all your column names
	FString CSVHeader = TEXT("Generation,TreeHash,FrontIndex,DamageDealt,DamageTaken,DistanceTravelled,TreeUtilization,TreeSize,Fitness,TrialsCompleted,TreeString\n");
    
	// SaveStringToFile without the 'Append' flag creates a new file and writes the text
	if (FFileHelper::SaveStringToFile(CSVHeader, *CSVFilePath))
	{
		UE_LOG(LogGeneticServer, Log, TEXT("Created new CSV with headers at %s"), *CSVFilePath);
	}
	else
	{
		UE_LOG(LogGeneticServer, Error, TEXT("Failed to create CSV at %s"), *CSVFilePath);
	}
	
	

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
					// 1. Grab the current ID and increment it for the next job
					int32 AssignedJobID = NextJobID++;

					JsonResponse->SetStringField(TEXT("status"), TEXT("success"));
					JsonResponse->SetStringField(TEXT("asset_path"), AssignedAssetPath);
					JsonResponse->SetNumberField(TEXT("job_id"), AssignedJobID);

					// --- NEW: TRACK THE DISPATCHED JOB ---
					FDispatchedJob NewJob;
					NewJob.AssetPath = AssignedAssetPath;
					NewJob.DispatchTime = FPlatformTime::Seconds();
					ActiveJobs.Add(AssignedJobID, NewJob);
					// -------------------------------------

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
// --- ENDPOINT 2 - WORKER SUBMITS FITNESS ---
    HttpRouter->BindRoute(FHttpPath(TEXT("/api/submit")), EHttpServerRequestVerbs::VERB_POST, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString BodyString(Request.Body.Num(), UTF8_TO_TCHAR((const char*)Request.Body.GetData()));
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyString);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
            	// 1. Parse JSON
				FString AssetPath = JsonObject->GetStringField(TEXT("asset_path"));
				int32 JobID = JsonObject->GetIntegerField(TEXT("job_id"));
                
				// --- NEW: REMOVE FROM ACTIVE TRACKING ---
				if (ActiveJobs.Contains(JobID))
				{
					ActiveJobs.Remove(JobID);
				}
				// ----------------------------------------
                
				// Parse the 5 target objectives
				float DamageDealt = JsonObject->GetNumberField(TEXT("damage_dealt"));
                float DamageTaken = JsonObject->GetNumberField(TEXT("damage_taken"));
                float Distance = JsonObject->GetNumberField(TEXT("distance"));
                float Utilization = JsonObject->GetNumberField(TEXT("utilization"));
                int32 TreeSize = JsonObject->GetIntegerField(TEXT("tree_size"));
                
                float RawFitness = JsonObject->GetNumberField(TEXT("fitness")); // For legacy logging
                FString RawTreeString = JsonObject->GetStringField(TEXT("tree_string"));

                int32 CompletedGeneration = CurrentGeneration - 1;

                // 2. Extract TreeHash from AssetPath (Format: /Game/.../G0_Tree_<Hash>)
                FString TreeHash;
                int32 HashIndex;
                if (AssetPath.FindLastChar('_', HashIndex))
                {
                    TreeHash = AssetPath.RightChop(AssetPath.Len() - HashIndex - 1);
                }
                else
                {
                    TreeHash = AssetPath; // Fallback
                }

                // 3. Aggregate Data for this specific Genome Hash
                FSimulationResult& AggregatedResult = PendingEvaluations.FindOrAdd(TreeHash);
                
                if (AggregatedResult.TrialsCompleted == 0)
                {
                    AggregatedResult.BehaviorTreePath = AssetPath;
                    AggregatedResult.TreeHash = TreeHash;
                    AggregatedResult.GenerationNumber = CompletedGeneration;
                	AggregatedResult.TreeString = RawTreeString;
                }

                AggregatedResult.DamageDealt += DamageDealt;
                AggregatedResult.DamageTaken += DamageTaken;
                AggregatedResult.DistanceTravelled += Distance;
                AggregatedResult.TreeUtilization += Utilization;
                AggregatedResult.TreeSize += TreeSize;
                AggregatedResult.Fitness += RawFitness;
                AggregatedResult.TrialsCompleted++;

                UE_LOG(LogGeneticServer, Log, TEXT("MASTER: Received trial %d/%d for Job %d (%s)"), 
                    AggregatedResult.TrialsCompleted, TrialsPerGenome, JobID, *TreeHash);

                // 4. Check if K Trials are completed
                if (AggregatedResult.TrialsCompleted >= TrialsPerGenome)
                {
                    // Average the objectives
                    AggregatedResult.DamageDealt /= TrialsPerGenome;
                    AggregatedResult.DamageTaken /= TrialsPerGenome;
                    AggregatedResult.DistanceTravelled /= TrialsPerGenome;
                    AggregatedResult.TreeUtilization /= TrialsPerGenome;
                    AggregatedResult.TreeSize /= TrialsPerGenome;
                    AggregatedResult.Fitness /= TrialsPerGenome;

                    // Push to the Epoch tracking array
                    CurrentEpochResults.Add(AggregatedResult);
                    
                    UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Genome %s fully evaluated. Progress: %d/%d"), 
                        *TreeHash, CurrentEpochResults.Num(), PopulationSize);

                    // Remove from pending
                    PendingEvaluations.Remove(TreeHash);

                    // CHECK EPOCH COMPLETION
                    if (CurrentEpochResults.Num() >= PopulationSize)
                    {
                    	ProcessCompletedEpoch();
                    }
                }
            }

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"success\"}")), TEXT("application/json"));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    HttpServerModule.StartAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server listening on port %d"), Port);

    // --- BOOTSTRAP GENERATION 0 ---
	GenerateInitialEpoch();

    // 3. INFINITE LOOP TO KEEP PROCESS ALIVE
	// 3. INFINITE LOOP TO KEEP PROCESS ALIVE
	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		// Sleep for 10ms to prevent 100% CPU usage
		FPlatformProcess::Sleep(0.01f); 

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;

		// --- NEW: TICK THE TIMEOUT MANAGER ---
		CheckForTimeouts();
		// -------------------------------------

		// Tick core engine systems required for async HTTP thread callbacks
		FTSTicker::GetCoreTicker().Tick(DeltaTime);
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
	}

    // 4. CLEANUP ON SHUTDOWN (Ctrl+C)
    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}