#include "GeneticFitnessTracker.h"

#include "GeneticGenerationModule.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UGeneticFitnessTracker::UGeneticFitnessTracker()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UGeneticFitnessTracker::BeginPlay()
{
	Super::BeginPlay();
	// We wait for explicit BeginTracking from the Manager to avoid recording setup time
}

// GeneticGeneration/Private/GeneticFitnessTracker.cpp

void UGeneticFitnessTracker::BeginTracking(AActor* InTargetPlayer)
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("Begin Tracking initiated."));

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 1. Bind Owner Damage
	Owner->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage); // Safety remove
	Owner->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);

	// 2. Use the passed Target Player (Dependency Injection)
	TargetPlayer = InTargetPlayer; // Store in the member variable defined in .h

	APlayerUnleashedBase* PlayerUnleashed = Cast<APlayerUnleashedBase>(TargetPlayer);

	if (PlayerUnleashed)
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("Tracker: Bound to Player %s"), *PlayerUnleashed->GetName());

		// Bind Death
		PlayerUnleashed->OnPlayerEvent.RemoveDynamic(this, &UGeneticFitnessTracker::OnPlayerDied);
		PlayerUnleashed->OnPlayerEvent.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerDied);

		// Bind Damage
		PlayerUnleashed->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
		PlayerUnleashed->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
	}
	else
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("Tracker: TargetPlayer is NULL or Invalid Cast!"));
	}

	// 3. Reset State
	StartTime = GetWorld()->GetTimeSeconds();
	bTrackingActive = true;
	bPlayerWasKilled = false;
	AccumulatedReward = 0.0f;
}

void UGeneticFitnessTracker::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;
	AccumulatedReward -= (Damage * DamageTakenPenalty);
}

void UGeneticFitnessTracker::OnPlayerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;

	if (DamageCauser == GetOwner() || InstigatedBy == GetOwner()->GetInstigatorController())
	{
		AccumulatedReward += (Damage * DamageDealtWeight);
		bDamagedPlayer = true;
	}
	
}

void UGeneticFitnessTracker::OnPlayerDied()
{
	if (!bTrackingActive) return;

	// Mark success so CalculateFitness knows we won
	bPlayerWasKilled = true;
    
	// Optional: Log it
	UE_LOG(LogTemp, Display, TEXT("Tracker: Enemy %s witnessed Player Death!"), *GetOwner()->GetName());
}

void UGeneticFitnessTracker::AddCustomReward(float Amount)
{
	if (bTrackingActive)
	{
		AccumulatedReward += Amount;
	}
}

float UGeneticFitnessTracker::CalculateFitness()
{
	if (!bTrackingActive && AccumulatedReward == 0.0f) return 0.0f;

	if (!bDamagedPlayer)
	{
		return -50.0f;
	}

	double CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeAlive = (float)(CurrentTime - StartTime);

	// BASE SCORE: Damage Dealt + Time Alive
	float FinalScore = AccumulatedReward + (TimeAlive * SurvivalTimeWeight);

	// VICTORY BONUS
	if (bPlayerWasKilled)
	{
		// 1. Flat Bonus for the kill
		FinalScore += PlayerKillBonus;

		// 2. Efficiency Bonus: The faster we killed, the better
		// (Assuming 30.0f is max simulation time)
		float TimeRemaining = FMath::Max(0.0f, 30.0f - TimeAlive);
		FinalScore += (TimeRemaining * 50.0f); // 50 pts per second saved
	}

	UE_LOG(LogTemp, Display, TEXT("Enemy Fitness: %f"), FinalScore);
	return FMath::Max(0.0f, FinalScore);
}