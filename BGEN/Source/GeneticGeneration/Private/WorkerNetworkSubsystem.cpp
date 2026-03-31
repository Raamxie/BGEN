#include "WorkerNetworkSubsystem.h"
#include "Json.h"
#include "Kismet/GameplayStatics.h"

void UWorkerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bIsSimulating = false;
}

void UWorkerNetworkSubsystem::RequestJobFromMaster()
{
	// HARD LOCK: Do absolutely nothing if we are in the middle of a simulation
	if (bIsSimulating)
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("WORKER: Sending polling request to Master Server..."));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(MasterServerURL + TEXT("/api/request_job"));
	Request->SetVerb(TEXT("GET"));
	
	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid() && bIsSimulating == false)
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				FString Status = JsonObject->GetStringField(TEXT("status"));
				if (Status == TEXT("success"))
				{
					// LOCK THE WORKER: We have a job, stop polling.
					bIsSimulating = true;
					
					CurrentJobAssetPath = JsonObject->GetStringField(TEXT("asset_path"));
					CurrentJobID = JsonObject->GetIntegerField(TEXT("job_id"));

					// 3. BROADCAST DELEGATE INSTEAD OF FETCHING MANAGER
					OnJobReceived.Broadcast(CurrentJobAssetPath);
				}
				else if (Status == TEXT("standby"))
				{
					// Only schedule another poll if we are legitimately idle
					if (!bIsSimulating && GetWorld())
					{
						GetWorld()->GetTimerManager().SetTimer(PollingTimerHandle, this, &UWorkerNetworkSubsystem::RequestJobFromMaster, 3.0f, false);
					}
				}
			}
		}
		else
		{
			// Retry on network failure only if idle
			if (!bIsSimulating && GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(PollingTimerHandle, this, &UWorkerNetworkSubsystem::RequestJobFromMaster, 3.0f, false);
			}
		}
	});

	Request->ProcessRequest();
}

void UWorkerNetworkSubsystem::SubmitFitness(FString AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, FString TreeString)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(MasterServerURL + TEXT("/api/submit"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("asset_path"), AssetPath);
	JsonObject->SetNumberField(TEXT("job_id"), JobID);
	JsonObject->SetNumberField(TEXT("fitness"), Fitness);
	JsonObject->SetNumberField(TEXT("distance"), Distance);
	JsonObject->SetNumberField(TEXT("damage_taken"), DamageTaken);
	JsonObject->SetNumberField(TEXT("damage_dealt"), DamageDealt);
	JsonObject->SetNumberField(TEXT("reward"), Reward);
	JsonObject->SetNumberField(TEXT("time_alive"), TimeAlive);
	JsonObject->SetNumberField(TEXT("tree_size"), TreeSize);
	JsonObject->SetNumberField(TEXT("utilization"), Utilization);
	JsonObject->SetBoolField(TEXT("player_killed"), bPlayerKilled);
	JsonObject->SetStringField(TEXT("tree_string"), TreeString);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(OutputString);

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		// UNLOCK THE WORKER: Job is successfully posted, we can poll again
		bIsSimulating = false;
		
		if (GetWorld())
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("EmptyWaiting"));
		}
	});

	Request->ProcessRequest();
}