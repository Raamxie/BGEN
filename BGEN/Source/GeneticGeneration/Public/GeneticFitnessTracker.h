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
	
	// Change the signature to accept the target
	void BeginTracking(AActor* InTargetPlayer);
	// --- Configuration (Designer Tweaks these) ---
	
	// Points awarded per unit of damage dealt to the Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float DamageDealtWeight = 1.0f;

	// Points awarded per second survived
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float SurvivalTimeWeight = 0.5f;

	// Penalty for taking damage (keep positive, we subtract it)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float DamageTakenPenalty = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float EfficiencyWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float SurvivalWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Fitness")
	float PlayerKillBonus = 0.0f;

	bool bDamagedPlayer = false;
	// --- Runtime API ---

	// Starts the timer and binds events
	void BeginTracking();

	// Returns the final calculated fitness
	float CalculateFitness();

	// Callable by Designer's BT Tasks (e.g., "Successfully Hid behind cover" -> Reward 50)
	UFUNCTION(BlueprintCallable, Category = "Genetic Fitness")
	void AddCustomReward(float Amount);

protected:

	void BeginPlay() override;

	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnPlayerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnPlayerDied();

private:
	float AccumulatedReward = 0.0f;
	double StartTime = 0.0f;
	bool bTrackingActive = false;
	bool bPlayerWasKilled = false;

	// Cache the player reference to detect when we hurt them
	UPROPERTY()
	AActor* TargetPlayer = nullptr;

		
};
