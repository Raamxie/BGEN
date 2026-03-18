// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUnleashedBase.h"
#include "GeneticFitnessTracker.generated.h"

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

	// --- Configuration: General ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|General")
	float BaseFitnessScore = 500.0f; // Gives a buffer so early generations don't hit 0

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
	float PacifistPenalty = 600.0f;

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

	// NEW: Bloat Control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	int32 MaximumTreeNodes = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness|Structure")
	float BigTreePenalty = 900.0f;

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

	int32 GetTreeSize() const;

	float AccumulatedReward = 0.0f;
	float AccumulatedDamageTaken = 0.0f;
	float AccumulatedDistance = 0.0f;

	double StartTime = 0.0f;
	bool bTrackingActive = false;
	bool bPlayerWasKilled = false;
	bool bDamagedPlayer = false;

	FVector LastRecordedLocation;
	FTimerHandle MovementTimerHandle;

	UPROPERTY()
	AActor* TargetPlayer = nullptr;
};