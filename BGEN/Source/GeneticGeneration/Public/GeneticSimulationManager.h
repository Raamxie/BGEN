#pragma once

#include "CoreMinimal.h"
#include "CustomBehaviourTree.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "GeneticSimulationManager.generated.h"

// Forward declarations
class UBehaviorTree;
class UWorld;

DECLARE_MULTICAST_DELEGATE(FOnEpochComplete);

USTRUCT()
struct FSimulationResult
{
	GENERATED_BODY()

	UPROPERTY()
	FString BehaviorTreePath;

	UPROPERTY()
	float Fitness = 0.0f;

	UPROPERTY()
	int32 GenerationNumber = 0;
};

UCLASS()
class GENETICGENERATION_API UGeneticSimulationManager : public UObject
{
	GENERATED_BODY()

public:
	UGeneticSimulationManager();

	// --- Lifecycle API ---

	/** Basic setup: Sets the World Context and Engine flags */
	void Init(UWorld* InWorld);

	/** * The Main Entry Point.
	 * Decides which tree to load, spawns agents, and starts the loop.
	 */
	void StartEpoch();

	// --- Communication API ---
	
	FOnEpochComplete OnEpochComplete;
	void FinishEpoch();

	// --- Simulation API ---

	void PreparePlayer();
	void SpawnEnemies(int32 AmountToSpawn, FString GenomePath);
	void Simulate();

	// --- Helpers ---

	void SetPause(bool bPauseState);
	bool IsPaused() const;

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
	
	// History of all previous runs
	UPROPERTY()
	TArray<FSimulationResult> AllTimeResults;

	// --- Internal Logic ---

	FString SelectTreeToEvolve(); // Helper to pick the parent

	bool DoesPlayerExist() const;
	UClass* GetClassFromPath(const FString& Path);
	UObject* LoadObjectFromPath(const FString& Path);

	UFUNCTION()
	void TimerCallback();
	
	UFUNCTION()
	void PlayerDiedListener();
	
	void StopSimulation();

	UPROPERTY()
	TMap<APawn*, UCustomBehaviourTree*> ActiveAgents;
};