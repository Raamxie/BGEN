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

		FString TreeHash;
		int32 HashIndex;
		if (Sub.AssetPath.FindLastChar('_', HashIndex))
		{
			TreeHash = Sub.AssetPath.RightChop(HashIndex + 1);
		}
		else
		{
			TreeHash = Sub.AssetPath; 
		}

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

		// --- NEW LOGIC: Print Active and Queued Jobs ---
		FString WaitingJobsStr;
		FString QueuedJobsStr;
		int32 QueuedCount = 0;
		{
			FScopeLock Lock(&JobStateMutex);
			
			// 1. Active Jobs (Assigned to a Worker)
			for (const auto& Kvp : ActiveJobs)
			{
				WaitingJobsStr += FString::Printf(TEXT("%d, "), Kvp.Key);
			}
			
			// 2. Queued Jobs (Waiting for Assignment)
			QueuedCount = JobQueue.Num();
			for (const FString& Path : JobQueue)
			{
				FString Hash;
				int32 Idx;
				if (Path.FindLastChar('_', Idx)) Hash = Path.RightChop(Idx + 1);
				else Hash = Path;
				
				// Use just the first 6 characters of the hash to keep the log readable without spam
				QueuedJobsStr += Hash.Left(6) + TEXT(", ");
			}
		}
		
		if (!WaitingJobsStr.IsEmpty())
		{
			WaitingJobsStr.RemoveFromEnd(TEXT(", "));
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Active Job IDs currently running: [%s]"), *WaitingJobsStr);
		}
		else
		{
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Active Job IDs currently running: [None]"));
		}

		if (!QueuedJobsStr.IsEmpty())
		{
			QueuedJobsStr.RemoveFromEnd(TEXT(", "));
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Unassigned Jobs in Queue (%d): [%s]"), QueuedCount, *QueuedJobsStr);
		}
		else
		{
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Unassigned Jobs in Queue: [None - Queue Empty]"));
		}
		// ---------------------------------------------------------------------------------

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
	// Don't evaluate timeouts if we are already tearing down the cluster
	if (bIsShuttingDown) return; 

	double CurrentTime = FPlatformTime::Seconds();
	bool bTimeoutOccurred = false;
	int32 OffendingJobID = -1;

	{
		FScopeLock Lock(&JobStateMutex);
		for (const auto& Kvp : ActiveJobs)
		{
			if ((CurrentTime - Kvp.Value.DispatchTime) > JobTimeoutSeconds)
			{
				bTimeoutOccurred = true;
				OffendingJobID = Kvp.Key;
				break; // One timeout is enough to kill the whole cluster
			}
		}
	}

	if (bTimeoutOccurred)
	{
		UE_LOG(LogGeneticServer, Error, TEXT("MASTER: FATAL TIMEOUT on Job %d! Initiating Cluster Kill Switch."), OffendingJobID);
		bIsShuttingDown = true;
		ShutdownStartTime = CurrentTime;
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
			TSharedPtr<FJsonObject> JsonResponse = MakeShareable(new FJsonObject());

			if (bIsShuttingDown)
			{
				JsonResponse->SetStringField(TEXT("status"), TEXT("quit"));
			}
			else 
			{
				FString AssignedAssetPath = GetNextJob();
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
           	// If the cluster is dying, reject the submission and tell them to quit.
           	if (bIsShuttingDown)
           	{
   				TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"quit\"}")), TEXT("application/json"));
   				OnComplete(MoveTemp(Response));
   				return true;
           	}
   
               // [FIXED] Safely null-terminate the byte array before converting to FString to prevent garbage memory reading
               TArray<uint8> BodyBytes = Request.Body;
               BodyBytes.Add('\0');
               FString BodyString = UTF8_TO_TCHAR((const char*)BodyBytes.GetData());
   
               TSharedPtr<FJsonObject> JsonObject;
               TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyString);
   
               if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
               {
   				int32 JobID = JsonObject->GetIntegerField(TEXT("job_id"));
               	bool bIsLateGhost = false;
               	{
               		FScopeLock Lock(&JobStateMutex);
               		if (!ActiveJobs.Contains(JobID)) bIsLateGhost = true;
               	}
   
               	// If a worker took too long, the job is invalid. Tell THIS specific worker to quit.
               	if (bIsLateGhost)
               	{
               		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"quit\"}")), TEXT("application/json"));
               		OnComplete(MoveTemp(Response));
               		return true;
               	}
               	
   				FWorkerSubmission Sub;
   				Sub.AssetPath = JsonObject->GetStringField(TEXT("asset_path"));
   				Sub.JobID = JobID;
   				Sub.DamageDealt = JsonObject->GetNumberField(TEXT("damage_dealt"));
   				Sub.DamageTaken = JsonObject->GetNumberField(TEXT("damage_taken"));
   				Sub.Distance = JsonObject->GetNumberField(TEXT("distance"));
   				Sub.Utilization = JsonObject->GetNumberField(TEXT("utilization"));
   				Sub.TreeSize = JsonObject->GetIntegerField(TEXT("tree_size"));
   				Sub.Fitness = JsonObject->GetNumberField(TEXT("fitness")); 
   				Sub.TreeString = JsonObject->GetStringField(TEXT("tree_string"));
   
   				SubmissionQueue.Enqueue(Sub);
               }
               else 
               {
                   // [FIXED] Alert the user immediately if the JSON was corrupted or invalid
                   UE_LOG(LogGeneticServer, Error, TEXT("MASTER FATAL: Failed to parse JSON from Worker! Payload: %s"), *BodyString);
               }
   
               TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"success\"}")), TEXT("application/json"));
               OnComplete(MoveTemp(Response));
               return true;
           }));

	// --- ENDPOINT 3 - ADMIN MANUAL SHUTDOWN ---
	// Pinging http://127.0.0.1:8080/api/admin/shutdown cleanly kills the cluster without abruptly crashing the Master
	HttpRouter->BindRoute(FHttpPath(TEXT("/api/admin/shutdown")), EHttpServerRequestVerbs::VERB_GET, 
		FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: Admin triggered manual cluster shutdown."));
			bIsShuttingDown = true;
			ShutdownStartTime = FPlatformTime::Seconds();

			TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"shutting_down\"}")), TEXT("application/json"));
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

		if (bIsShuttingDown)
		{
			// Keep the server alive for exactly 10 seconds to allow all workers to poll the "quit" command
			if (CurrentTime - ShutdownStartTime > 10.0f)
			{
				UE_LOG(LogGeneticServer, Warning, TEXT("MASTER: 10-Second Grace Period over. Exiting Engine."));
				break; 
			}
		}
		else
		{
			// Only check for timeouts and process data if we are running normally
			CheckForTimeouts();
			ProcessSubmissions();
		}

		FTSTicker::GetCoreTicker().Tick(DeltaTime);
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
	}

    // 4. CLEANUP ON SHUTDOWN
    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}