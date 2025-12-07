#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h" // For FTimerHandle
#include "GeneticSimulationManager.generated.h"

// Forward declarations
class UBehaviorTree;
class UWorld;

/**
 * Manages the genetic algorithm simulation loop.
 * This object remains persistent across level reloads to store generation data.
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
	void Simulate();

	// --- Helpers ---

	void SetPause(bool bPauseState);
	bool IsPaused() const;

	// Essential override: Allows this UObject to access the World (for Timers, Spawning)
	virtual UWorld* GetWorld() const override;

	// --- Persistent Data (Survives Level Reloads) ---
	
	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	int32 GenerationCount = 1;

	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	float BestFitnessAllTime = 0.0f;

protected:
	// --- Transient Data (Reset every run) ---

	UPROPERTY()
	UWorld* TargetWorld;

	UPROPERTY()
	TArray<FVector> EnemySpawnPositions;

	FTimerHandle TimerHandle;

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
};