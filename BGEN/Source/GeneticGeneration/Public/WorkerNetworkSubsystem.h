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

	// Call this from the Bootstrap map
	UFUNCTION(BlueprintCallable, Category = "Genetic Worker")
	void RequestJobFromMaster();

	// The Manager uses this to submit results
	void SubmitFitness(const FString& AssetPath, float Fitness);

	// The current job we are supposed to be simulating
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Genetic Worker")
	FString CurrentJobAssetPath;

	UPROPERTY(BlueprintAssignable, Category = "Genetic Worker")
	FOnJobReceived OnJobReceived;

private:
	void OnJobRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnSubmitComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};