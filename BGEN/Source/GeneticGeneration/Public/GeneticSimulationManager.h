#pragma once

#include "CoreMinimal.h"
#include "CustomBehaviourTree.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h" // For FTimerHandle
#include "GeneticSimulationManager.generated.h"

// Define a multicast delegate to notify the module
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEpochComplete);

// Forward declarations
class UBehaviorTree;
class UWorld;

USTRUCT()
struct FSimulationResult
{	
	GENERATED_BODY()

	UPROPERTY()
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY()
	float Fitness = 0.0f;
};

/**
 * Manages the genetic algorithm simulation loop.
 * Persistent across level reloads.
 */
UCLASS()
class GENETICGENERATION_API UGeneticSimulationManager : public UObject
{
	GENERATED_BODY()

public:
	UGeneticSimulationManager();

	// --- Lifecycle API ---

	/** Initial setup when the Manager is first created */
	void Init(UWorld* InWorld);

	/** Called by the Module when the level reloads to hand over the new World pointer */
	void OnLevelReload(UWorld* NewWorld);

	// --- Simulation API ---

	void PreparePlayer();
	void SpawnEnemies(int32 AmountToSpawn);
	void SetSpawnPositions(TArray<FVector> positions);
	
	/** Begins the Warmup Phase, followed automatically by the Simulation */
	void Simulate();

	// --- Helpers ---

	void SetPause(bool bPauseState);
	bool IsPaused() const;

	virtual UWorld* GetWorld() const override;

	// --- Persistent Data ---
	
	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	int32 GenerationCount = 1;

	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	float BestFitnessAllTime = 0.0f;

	// Bind to this in the Module to know when to reload the map
	UPROPERTY()
	FOnEpochComplete OnEpochComplete;

	// Call this when your simulation logic determines the epoch is over
	UFUNCTION()
	void FinishEpoch();

protected:
	// --- Transient Data ---

	UPROPERTY()
	UWorld* TargetWorld;

	UPROPERTY()
	TArray<FVector> EnemySpawnPositions;

	// Timer for the actual simulation limit (e.g. 30s)
	FTimerHandle TimerHandle;

	// Timer for the physics settling phase (e.g. 2s)
	FTimerHandle WarmupTimerHandle;

	UPROPERTY()
	TMap<APawn*, UCustomBehaviourTree*> ActiveAgents;

	UPROPERTY()
	FString LastGenerationBestTreePath;

	UPROPERTY()
	UCustomBehaviourTree* BestGenTreeWrapper;

	// --- Internal Logic ---

	bool DoesPlayerExist() const;
	UClass* GetClassFromPath(const FString& Path);
	UObject* LoadObjectFromPath(const FString& Path);

	UFUNCTION()
	void TimerCallback();
	
	UFUNCTION()
	void PlayerDiedListener();
	
	/** Reloads the current level to reset the simulation environment */
	void TriggerRestart();
	
	/** Stops timers and calculates fitness before restart */
	void StopSimulation();

	/** Called when physics have settled; starts AI logic and fitness tracking */
	void OnWarmupFinished();
};