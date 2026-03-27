// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUnleashedBase.h"
#include "AIController.h"
#include "GeneticFitnessTracker.generated.h"

// Forward declaration to prevent circular dependency
class ACustomAIController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENETICGENERATION_API UGeneticFitnessTracker : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGeneticFitnessTracker();

	void BeginTracking(AActor* InTargetPlayer);
	float CalculateFitness();

	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	void AddCustomReward(float Amount);

	// --- Exposing Stats for Server Logs ---
	int32 GetTreeSize() const;
	float GetAccumulatedDistance() const { return AccumulatedDistance; }
	float GetAccumulatedDamageTaken() const { return AccumulatedDamageTaken; }
	float GetAccumulatedReward() const { return AccumulatedReward; }
	float GetAccumulatedDamageDealt() const { return AccumulatedDamageDealt; }
	float GetTimeAlive() const;

	// --- NEW: Tree Utilization Tracking ---

	/** Adds a node identifier to the set of executed tasks */
	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	void RecordNodeExecution(const FString& NodeIdentifier);

	/** Static helper to easily report execution from inside a Blueprint BTTask */
	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness", meta = (DisplayName = "Report Task Executed"))
	static void ReportTaskExecuted(AAIController* AIController, const FString& TaskName);

	/** Returns the ratio of executed unique tasks against the total size of the tree */
	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	float GetTreeUtilizationPercentage() const;

	/** Returns the exact count of unique tasks that ran */
	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	int32 GetExecutedTasksCount() const { return ExecutedTasks.Num(); }

	// --- Configuration: General ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|General")
	float BaseFitnessScore = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|General")
	float SurvivalTimeWeight = 0.5f;

	// --- Configuration: Combat ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Combat")
	float DamageDealtWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Combat")
	float SuccessfulAttackBonus = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Combat")
	float DamageTakenPenalty = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Combat")
	float PlayerKillBonus = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Combat")
	float PacifistPenalty = 100.0f;

	// --- Configuration: Movement ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Movement")
	float DistanceMovedWeight = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Movement")
	float MinimumExpectedDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Movement")
	float IdlePenalty = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Movement")
	float ProximityBonusWeight = 0.1f;

	// --- Configuration: Structure ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	int32 MinimumTreeNodes = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	float SmallTreePenalty = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	int32 MaximumTreeNodes = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	float BigTreePenalty = 200.0f;

	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	bool GetPlayerWasKilled() const { return bPlayerWasKilled; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnPlayerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnPlayerDied();

private:
	UFUNCTION()
	void RecordMovementRoutine();

	float AccumulatedReward = 0.0f;
	float AccumulatedDamageTaken = 0.0f;
	float AccumulatedDamageDealt = 0.0f;
	float AccumulatedDistance = 0.0f;

	double StartTime = 0.0f;
	bool bTrackingActive = false;
	bool bPlayerWasKilled = false;
	bool bDamagedPlayer = false;

	FVector LastRecordedLocation;
	FTimerHandle MovementTimerHandle;

	UPROPERTY()
	AActor* TargetPlayer = nullptr;

	// NEW: Cache the AI Controller so we can read the tree even if unpossessed
	UPROPERTY()
	ACustomAIController* CachedAIController = nullptr;

	// Tracks the unique names of tasks that have been executed
	UPROPERTY()
	TSet<FString> ExecutedTasks;
};