#include "WorkerNetworkSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"

void UWorkerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentJobAssetPath = TEXT("");
}

void UWorkerNetworkSubsystem::RequestJobFromMaster()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://127.0.0.1:8080/api/request_job"));
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &UWorkerNetworkSubsystem::OnJobRequestComplete);
	Request->ProcessRequest();
}

void UWorkerNetworkSubsystem::OnJobRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
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
				OnJobReceived.Broadcast(CurrentJobAssetPath);
			}
			else
			{
				// Standby: Master has no jobs. Wait 1 second and ask again.
				FTimerHandle RetryTimer;
				GetWorld()->GetTimerManager().SetTimer(RetryTimer, this, &UWorkerNetworkSubsystem::RequestJobFromMaster, 1.0f, false);
			}
		}
	}
}

void UWorkerNetworkSubsystem::SubmitFitness(const FString& AssetPath, float Fitness)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://127.0.0.1:8080/api/submit"));
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString Payload = FString::Printf(TEXT("{\"asset_path\":\"%s\", \"fitness\":%f}"), *AssetPath, Fitness);
	Request->SetContentAsString(Payload);
	Request->OnProcessRequestComplete().BindUObject(this, &UWorkerNetworkSubsystem::OnSubmitComplete);
	Request->ProcessRequest();
}

void UWorkerNetworkSubsystem::OnSubmitComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	// Result submitted. We are done!
	CurrentJobAssetPath = TEXT("");
}