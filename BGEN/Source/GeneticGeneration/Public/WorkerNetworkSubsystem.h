#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "WorkerNetworkSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobReceived, FString, AssetPath);

UCLASS()
class GENETICGENERATION_API UWorkerNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Genetic Worker")
	void RequestJobFromMaster();

	// UPDATED: Now takes all raw metrics to send to the server
	void SubmitFitness(const FString& AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, const FString& TreeString);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Genetic Worker")
	FString CurrentJobAssetPath;

	// NEW: Tracks the Job ID assigned by the Master
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Genetic Worker")
	int32 CurrentJobID = -1;

	UPROPERTY(BlueprintAssignable, Category = "Genetic Worker")
	FOnJobReceived OnJobReceived;

private:
	void OnJobRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnSubmitComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	bool bIsPolling = false;
	FTimerHandle PollingTimerHandle;
};