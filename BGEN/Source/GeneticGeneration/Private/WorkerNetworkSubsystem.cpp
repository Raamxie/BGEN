#include "WorkerNetworkSubsystem.h"
#include "GeneticGenerationModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/GameplayStatics.h"

void UWorkerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentJobAssetPath = TEXT("");
	CurrentJobID = -1;
	bIsPolling = false;
	bIsSimulating = false;
}

void UWorkerNetworkSubsystem::RequestJobFromMaster()
{
	// Don't poll if we already have a job, are currently polling, or are actively simulating
	if (!CurrentJobAssetPath.IsEmpty()) return; 
	if (bIsPolling) return; 
	if (bIsSimulating) return;

	bIsPolling = true;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(MasterServerURL + TEXT("/api/request_job"));
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
			
			if (Status == TEXT("quit"))
			{
				UE_LOG(LogTemp, Error, TEXT("WORKER: Received KILL SIGNAL from Server. Shutting down..."));
				FGenericPlatformMisc::RequestExit(false);
				return;
			}
			
			if (Status == TEXT("success"))
			{
				bIsSimulating = true;
				CurrentJobAssetPath = JsonObject->GetStringField(TEXT("asset_path"));
				CurrentJobID = JsonObject->GetIntegerField(TEXT("job_id")); 
				
				if (GetGameInstance())
				{
					GetGameInstance()->GetTimerManager().ClearTimer(PollingTimerHandle);
				}

				// Trigger map load logic securely
				OnJobReceived.Broadcast(CurrentJobAssetPath);
				bGotJob = true; 
			}
		}
	}
	else 
	{
		FString ErrMsg = Response.IsValid() ? FString::Printf(TEXT("HTTP Code %d"), Response->GetResponseCode()) : TEXT("Connection Failed / Timeout");
		UE_LOG(LogTemp, Warning, TEXT("WORKER NETWORK: Polling failed (%s). Retrying in 2s..."), *ErrMsg);
	}

	if (!bGotJob && GetGameInstance())
	{
		GetGameInstance()->GetTimerManager().SetTimer(PollingTimerHandle, this, &UWorkerNetworkSubsystem::RequestJobFromMaster, 2.0f, false);
	}
}

void UWorkerNetworkSubsystem::SubmitFitness(const FString& AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, const FString& TreeString)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(MasterServerURL + TEXT("/api/submit"));
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

	UE_LOG(LogTemp, Log, TEXT("WORKER: Sending Submission Payload for Job %d..."), JobID);

	Request->OnProcessRequestComplete().BindLambda([=, this](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool bConnectedSuccessfully)
	{
		if (!bConnectedSuccessfully || !pResponse.IsValid() || pResponse->GetResponseCode() != 200)
		{
			FString ErrMsg = pResponse.IsValid() ? FString::Printf(TEXT("HTTP Code %d"), pResponse->GetResponseCode()) : TEXT("Connection Failed / Timeout");
			UE_LOG(LogTemp, Error, TEXT("WORKER FATAL: Submission dropped! (%s). JobID: %d. Retrying in 3 seconds..."), *ErrMsg, JobID);
			
			// THE FIX: Unpause the game clock so the 3-second timer can actually tick!
			if (GetWorld() && UGameplayStatics::IsGamePaused(GetWorld()))
			{
				UGameplayStatics::SetGamePaused(GetWorld(), false);
			}

			if (GetGameInstance())
			{
				FTimerDelegate RetryDel = FTimerDelegate::CreateLambda([=, this]() {
					SubmitFitness(AssetPath, JobID, Fitness, Distance, DamageTaken, DamageDealt, Reward, TimeAlive, TreeSize, Utilization, bPlayerKilled, TreeString);
				});
				// Use the persistent handle from the header
				GetGameInstance()->GetTimerManager().SetTimer(RetryTimerHandle, RetryDel, 3.0f, false);
			}
			return; 
		}

		bIsSimulating = false;
		CurrentJobAssetPath = TEXT("");
		CurrentJobID = -1;

		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(pResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, ResponseJson))
		{
			if (ResponseJson->GetStringField(TEXT("status")) == TEXT("quit"))
			{
				UE_LOG(LogTemp, Error, TEXT("WORKER: Server rejected submission and issued KILL SIGNAL. Shutting down..."));
				FGenericPlatformMisc::RequestExit(false);
				return;
			}
		}

		FGeneticGenerationModule& GenModule = FModuleManager::GetModuleChecked<FGeneticGenerationModule>("GeneticGeneration");
		GenModule.LoadEmptyWaitingMap();
	});

	Request->ProcessRequest();
}