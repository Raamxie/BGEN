#include "GeneticServerCommandlet.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Containers/Ticker.h"
#include "Async/TaskGraphInterfaces.h"
#include "Misc/ScopeLock.h"

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
	FScopeLock Lock(&JobStateMutex);
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

    FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test"); 
	
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
				FScopeLock Lock(&JobStateMutex);
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
        TArray<TArray<FSimulationResult>> Fronts = UGeneticSelectionLibrary::FastNonDominatedSort(CurrentEpochResults, 5);
        for (const TArray<FSimulationResult>& Front : Fronts) SurvivingPopulation.Append(Front);
    }
    else
    {
        TArray<FSimulationResult> CombinedPool;
        CombinedPool.Append(SurvivingPopulation);
        CombinedPool.Append(CurrentEpochResults);

        TArray<TArray<FSimulationResult>> Fronts = UGeneticSelectionLibrary::FastNonDominatedSort(CombinedPool, 5);
        SurvivingPopulation.Empty();

        for (TArray<FSimulationResult>& Front : Fronts)
        {
            if (SurvivingPopulation.Num() + Front.Num() <= PopulationSize)
            {
                SurvivingPopulation.Append(Front);
            }
            else
            {
                int32 NeededCount = PopulationSize - SurvivingPopulation.Num();
                TArray<FSimulationResult> Selected = UGeneticSelectionLibrary::PriorityTieBreakingSelection(Front, NeededCount, 4); 
                SurvivingPopulation.Append(Selected);
                break; 
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

    GenerateSubsequentEpoch();
}

void UGeneticServerCommandlet::GenerateSubsequentEpoch()
{
    UE_LOG(LogGeneticServer, Warning, TEXT(" MASTER: Spawning Offspring for Epoch %d"), CurrentGeneration);

    for (int32 i = 0; i < PopulationSize; i++)
    {
        UCustomBehaviourTree* ChildWrapper = nullptr;
        
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
            
            while (EvaluatedHashes.Contains(TreeHash) && RetryCount < 10)
            {
                UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                TreeHash = ChildWrapper->GetTreeHash();
                RetryCount++;
            }
        	
            EvaluatedHashes.Add(TreeHash);
            
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/G%d_Tree_%s"), CurrentGeneration, *TreeHash);
            FString FinalAssetPath = ChildWrapper->SaveAsNewAsset(SaveName, true);

        	ChildWrapper->DebugLogTree(LogGeneticServer);
            if (!FinalAssetPath.IsEmpty())
            {
				FScopeLock Lock(&JobStateMutex);
                for (int32 k = 0; k < TrialsPerGenome; k++)
                {
                    JobQueue.Add(FinalAssetPath);
                }
                UE_LOG(LogGeneticServer, Log, TEXT("Queued Evolved Job %d (%d trials): %s"), i, TrialsPerGenome, *FinalAssetPath);
            }
        }
    }
}

void UGeneticServerCommandlet::ProcessSubmissions()
{
	FWorkerSubmission Sub;
	while (SubmissionQueue.Dequeue(Sub))
	{
		bool bIsValidJob = false;
		
		{
			// Thread Safe Removal check
			FScopeLock Lock(&JobStateMutex);
			if (ActiveJobs.Contains(Sub.JobID))
			{
				ActiveJobs.Remove(Sub.JobID);
				bIsValidJob = true;
			}
		}

		if (!bIsValidJob)
		{
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Ignored late/ghost submission for JobID %d. It was likely reassigned."), Sub.JobID);
			continue;
		}

		// [FIXED] Proper Math to Extract the pure TreeHash from the AssetPath 
		FString TreeHash;
		int32 HashIndex;
		if (Sub.AssetPath.FindLastChar('_', HashIndex))
		{
			// RightChop takes the number of chars to cut off from the LEFT side. 
			// We cut everything up to and including the '_'
			TreeHash = Sub.AssetPath.RightChop(HashIndex + 1);
		}
		else
		{
			TreeHash = Sub.AssetPath; 
		}

		// 3. Aggregate Data safely on the Game Thread
		FSimulationResult& AggregatedResult = PendingEvaluations.FindOrAdd(TreeHash);
                
		if (AggregatedResult.TrialsCompleted == 0)
		{
			AggregatedResult.BehaviorTreePath = Sub.AssetPath;
			AggregatedResult.TreeHash = TreeHash;
			AggregatedResult.GenerationNumber = CurrentGeneration - 1;
			AggregatedResult.TreeString = Sub.TreeString;
		}

		AggregatedResult.DamageDealt += Sub.DamageDealt;
		AggregatedResult.DamageTaken += Sub.DamageTaken;
		AggregatedResult.DistanceTravelled += Sub.Distance;
		AggregatedResult.TreeUtilization += Sub.Utilization;
		AggregatedResult.TreeSize += Sub.TreeSize;
		AggregatedResult.Fitness += Sub.Fitness;
		AggregatedResult.TrialsCompleted++;

		UE_LOG(LogGeneticServer, Log, TEXT("MASTER: Received trial %d/%d for Job %d (%s)"), 
			AggregatedResult.TrialsCompleted, TrialsPerGenome, Sub.JobID, *TreeHash);

		// 4. Check if K Trials are completed
		if (AggregatedResult.TrialsCompleted >= TrialsPerGenome)
		{
			AggregatedResult.DamageDealt /= TrialsPerGenome;
			AggregatedResult.DamageTaken /= TrialsPerGenome;
			AggregatedResult.DistanceTravelled /= TrialsPerGenome;
			AggregatedResult.TreeUtilization /= TrialsPerGenome;
			AggregatedResult.TreeSize /= TrialsPerGenome;
			AggregatedResult.Fitness /= TrialsPerGenome;

			CurrentEpochResults.Add(AggregatedResult);
                    
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Genome %s fully evaluated. Progress: %d/%d"), 
				*TreeHash, CurrentEpochResults.Num(), PopulationSize);

			PendingEvaluations.Remove(TreeHash);

			if (CurrentEpochResults.Num() >= PopulationSize)
			{
				ProcessCompletedEpoch();
			}
		}
	}
}

void UGeneticServerCommandlet::CheckForTimeouts()
{
	double CurrentTime = FPlatformTime::Seconds();
	TArray<int32> TimedOutJobIDs;

	// Safely evaluate timeouts
	{
		FScopeLock Lock(&JobStateMutex);
		for (const auto& Kvp : ActiveJobs)
		{
			if ((CurrentTime - Kvp.Value.DispatchTime) > JobTimeoutSeconds)
			{
				TimedOutJobIDs.Add(Kvp.Key);
			}
		}

		// Process timeouts
		for (int32 JobID : TimedOutJobIDs)
		{
			FDispatchedJob TimedOutJob = ActiveJobs[JobID];
			ActiveJobs.Remove(JobID);
        
			JobQueue.Add(TimedOutJob.AssetPath);
        
			UE_LOG(LogGeneticServer, Error, TEXT("MASTER: Job %d (%s) TIMED OUT after %.1f seconds! Re-adding to queue."), 
				JobID, *TimedOutJob.AssetPath, JobTimeoutSeconds);
		}
	}
}

int32 UGeneticServerCommandlet::Main(const FString& Params)
{
	UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
	UE_LOG(LogGeneticServer, Warning, TEXT(" Starting Headless Genetic Server Commandlet... "));
	UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
	
	FString CSVHeader = TEXT("Generation,TreeHash,FrontIndex,DamageDealt,DamageTaken,DistanceTravelled,TreeUtilization,TreeSize,Fitness,TrialsCompleted,TreeString\n");
    
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
					int32 AssignedJobID = -1;
					{
						FScopeLock Lock(&JobStateMutex);
						AssignedJobID = NextJobID++;

						FDispatchedJob NewJob;
						NewJob.AssetPath = AssignedAssetPath;
						NewJob.DispatchTime = FPlatformTime::Seconds();
						ActiveJobs.Add(AssignedJobID, NewJob);
					}

					JsonResponse->SetStringField(TEXT("status"), TEXT("success"));
					JsonResponse->SetStringField(TEXT("asset_path"), AssignedAssetPath);
					JsonResponse->SetNumberField(TEXT("job_id"), AssignedJobID);

					UE_LOG(LogGeneticServer, Log, TEXT("Assigned Job %d to worker: %s"), AssignedJobID, *AssignedAssetPath);
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
				// Push the raw parsed data to the MPSC Queue
				FWorkerSubmission Sub;
				Sub.AssetPath = JsonObject->GetStringField(TEXT("asset_path"));
				Sub.JobID = JsonObject->GetIntegerField(TEXT("job_id"));
				Sub.DamageDealt = JsonObject->GetNumberField(TEXT("damage_dealt"));
				Sub.DamageTaken = JsonObject->GetNumberField(TEXT("damage_taken"));
				Sub.Distance = JsonObject->GetNumberField(TEXT("distance"));
				Sub.Utilization = JsonObject->GetNumberField(TEXT("utilization"));
				Sub.TreeSize = JsonObject->GetIntegerField(TEXT("tree_size"));
				Sub.Fitness = JsonObject->GetNumberField(TEXT("fitness")); 
				Sub.TreeString = JsonObject->GetStringField(TEXT("tree_string"));

				SubmissionQueue.Enqueue(Sub);
            }

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"success\"}")), TEXT("application/json"));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    HttpServerModule.StartAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server listening on port %d"), Port);

    // --- BOOTSTRAP GENERATION 0 ---
	GenerateInitialEpoch();

	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.01f); 

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;

		// 1. Evaluate Late/Stuck Jobs
		CheckForTimeouts();

		// 2. Consume Worker submissions safely on the Game Thread
		ProcessSubmissions();

		// 3. Tick core engine systems required for async HTTP thread callbacks
		FTSTicker::GetCoreTicker().Tick(DeltaTime);
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
	}

    // 4. CLEANUP ON SHUTDOWN (Ctrl+C)
    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}