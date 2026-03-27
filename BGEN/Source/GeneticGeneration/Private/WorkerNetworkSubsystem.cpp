#include "WorkerNetworkSubsystem.h"
#include "GeneticGenerationModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

void UWorkerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentJobAssetPath = TEXT("");
	CurrentJobID = -1;
}

void UWorkerNetworkSubsystem::RequestJobFromMaster()
{
	if (!CurrentJobAssetPath.IsEmpty()) return; 

	if (bIsPolling) return; 
	bIsPolling = true;

	UE_LOG(LogTemp, Display, TEXT("WORKER: Sending polling request to Master Server..."));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://127.0.0.1:8080/api/request_job"));
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &UWorkerNetworkSubsystem::OnJobRequestComplete);
	Request->ProcessRequest();
}

void UWorkerNetworkSubsystem::OnJobRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	bIsPolling = false; 
	bool bGotJob = false;

	if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FString Status = JsonObject->GetStringField(TEXT("status"));
			if (Status == TEXT("success"))
			{
				CurrentJobAssetPath = JsonObject->GetStringField(TEXT("asset_path"));
				// Extract the Job ID
				CurrentJobID = JsonObject->GetIntegerField(TEXT("job_id")); 
				
				UE_LOG(LogTemp, Warning, TEXT("WORKER: Job %d Received! Loading Main Map..."), CurrentJobID);

				GetGameInstance()->GetTimerManager().ClearTimer(PollingTimerHandle);
				FGeneticGenerationModule& GenModule = FModuleManager::GetModuleChecked<FGeneticGenerationModule>("GeneticGeneration");
				GenModule.LoadSimulationMap();
				bGotJob = true; 
			}
		}
	}

	if (!bGotJob)
	{
		GetGameInstance()->GetTimerManager().SetTimer(PollingTimerHandle, this, &UWorkerNetworkSubsystem::RequestJobFromMaster, 2.0f, false);
	}
}

void UWorkerNetworkSubsystem::SubmitFitness(const FString& AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, const FString& TreeString)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://127.0.0.1:8080/api/submit"));
	Request->SetVerb("POST");
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

	FString Payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(Payload);
	Request->OnProcessRequestComplete().BindUObject(this, &UWorkerNetworkSubsystem::OnSubmitComplete);
	Request->ProcessRequest();
}

void UWorkerNetworkSubsystem::OnSubmitComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	// 1. Clear variables NOW, because the job is truly done
	CurrentJobAssetPath = TEXT("");
	CurrentJobID = -1;

	if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("WORKER: Server acknowledged results! Rebooting map..."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WORKER: Server unreachable or failed! Rebooting map anyway to avoid hanging..."));
	}

	// 2. Tell the Genetic Module to load the EmptyWaiting map. 
	// This cleanly wipes the memory and drops the Worker into the polling loop for the next job.
	FGeneticGenerationModule& GenModule = FModuleManager::GetModuleChecked<FGeneticGenerationModule>("GeneticGeneration");
	GenModule.LoadEmptyWaitingMap();
}