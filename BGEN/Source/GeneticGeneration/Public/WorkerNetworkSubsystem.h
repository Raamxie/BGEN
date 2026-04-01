#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "WorkerNetworkSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobReceivedDelegate, FString, JobPath);

UCLASS()
class GENETICGENERATION_API UWorkerNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void RequestJobFromMaster();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SubmitFitness(const FString& AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, const FString& TreeString);

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	FString CurrentJobAssetPath;

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	int32 CurrentJobID;

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	bool bIsSimulating = false; 

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	bool bIsPolling = false; 

	UPROPERTY(BlueprintAssignable, Category = "Network")
	FOnJobReceivedDelegate OnJobReceived;

private:
	FString MasterServerURL = TEXT("http://127.0.0.1:8080");
	FTimerHandle PollingTimerHandle;
	FTimerHandle RetryTimerHandle; // THE FIX: Persistent handle for the retry loop

	void OnJobRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};