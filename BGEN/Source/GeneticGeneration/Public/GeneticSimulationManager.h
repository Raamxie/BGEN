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
	FString TreeHash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 GenerationNumber = 0;

	// Multi-Trial Tracking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TrialsCompleted = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString TreeString;

	// NSGA-II State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 FrontIndex = -1;

	// --- 5 OBJECTIVES ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DamageDealt = 0.0f;       // Maximize

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DamageTaken = 0.0f;       // Minimize (Will be inverted for sorting later)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DistanceTravelled = 0.0f; // Maximize

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TreeUtilization = 0.0f;   // Maximize

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TreeSize = 0.0f;          // Minimize (Will be inverted for sorting later)

	// Kept for backwards compatibility / scalar logging
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Fitness = 0.0f;
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

	void SpawnEnemies(int32 AmountToSpawn, FString GenomePath);
	void Simulate();

	void SetPause(bool bPauseState);
	bool IsPaused() const;

	UFUNCTION()
	void StartEpochWithJob(FString AssignedAssetPath);
	
	virtual UWorld* GetWorld() const override;

	UPROPERTY(VisibleAnywhere, Category = "Genetic Data")
	int32 GenerationCount = 1;

	UFUNCTION()
	void TransitionToMainMap(FString JobPath);

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