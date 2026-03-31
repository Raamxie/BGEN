#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "WorkerNetworkSubsystem.generated.h"

// 1. DECLARE THE DELEGATE
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
	void SubmitFitness(FString AssetPath, int32 JobID, float Fitness, float Distance, float DamageTaken, float DamageDealt, float Reward, float TimeAlive, int32 TreeSize, float Utilization, bool bPlayerKilled, FString TreeString);

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	FString CurrentJobAssetPath;

	UPROPERTY(BlueprintReadOnly, Category = "Network")
	int32 CurrentJobID;

	// Track if we are actively simulating so we don't accidentally ask for another job
	UPROPERTY(BlueprintReadOnly, Category = "Network")
	bool bIsSimulating = false; 

	// 2. EXPOSE THE DELEGATE
	UPROPERTY(BlueprintAssignable, Category = "Network")
	FOnJobReceivedDelegate OnJobReceived;

private:
	FString MasterServerURL = TEXT("http://127.0.0.1:8080");
	FTimerHandle PollingTimerHandle;
};