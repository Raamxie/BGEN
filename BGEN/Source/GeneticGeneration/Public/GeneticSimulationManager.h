#pragma once

#include "CoreMinimal.h"
#include "CustomBehaviourTree.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "SimulationEventManager.h"
#include "GeneticSimulationManager.generated.h"


// Forward declarations
class UBehaviorTree;
class UWorld;

DECLARE_MULTICAST_DELEGATE(FOnEpochComplete);

USTRUCT(BlueprintType)
struct FSimulationResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString BehaviorTreePath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Fitness = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 GenerationNumber = 0;
};

UCLASS()
class GENETICGENERATION_API UGeneticSimulationManager : public UObject
{
	GENERATED_BODY()

public:
	UGeneticSimulationManager();

	void Init(UWorld* InWorld);
	void StartEpoch();

	FOnEpochComplete OnEpochComplete;
	void FinishEpoch();

	void PreparePlayer();
	void SpawnEnemies(int32 AmountToSpawn, FString GenomePath);
	void Simulate();

	void SetPause(bool bPauseState);
	bool IsPaused() const;

	UFUNCTION()
	void StartEpochWithJob(FString AssignedAssetPath);
	
	virtual UWorld* GetWorld() const override;

	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	int32 GenerationCount = 1;

protected:
	UPROPERTY()
	UWorld* TargetWorld;

	UPROPERTY()
	class APlayerUnleashedBase* ActivePlayer = nullptr;

	UPROPERTY()
	TArray<FVector> EnemySpawnPositions;

	FTimerHandle TimerHandle;
	
	// Local History
	UPROPERTY()
	TArray<FSimulationResult> AllTimeResults;

	// *** MULTI-INSTANCE SUPPORT ***
	FString InstanceID; 
	
	// Pool of assets found from other instances
	UPROPERTY()
	TArray<FSimulationResult> ForeignPool;

	// --- Internal Logic ---

	void TimerCallback();

	UFUNCTION()
	void PlayerDiedListener();
	
	FString SelectTreeToEvolve(); 

	bool DoesPlayerExist() const;
	UClass* GetClassFromPath(const FString& Path);
	UObject* LoadObjectFromPath(const FString& Path);
	
	void HandleSimulationEvent(ESimulationEvent EventType);
	
	void StopSimulation();



	UPROPERTY()
	TMap<APawn*, UCustomBehaviourTree*> ActiveAgents;
};