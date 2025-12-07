#include "GeneticFitnessTracker.h"
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

void UGeneticFitnessTracker::BeginTracking()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 1. Bind to Owner Damage (Penalty)
	Owner->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);

	// 2. Bind to Player Damage (Reward)
	// Note: In a real tool, you might iterate all "Enemy" tags, but here we assume one Player.
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	if (TargetPlayer)
	{
		TargetPlayer->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
	}

	StartTime = FPlatformTime::Seconds();
	bTrackingActive = true;
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

	// Check if WE caused the damage
	if (DamageCauser == GetOwner() || InstigatedBy == GetOwner()->GetInstigatorController())
	{
		AccumulatedReward += (Damage * DamageDealtWeight);
	}
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
	if (!bTrackingActive) return AccumulatedReward;

	double CurrentTime = FPlatformTime::Seconds();
	float TimeAlive = (float)(CurrentTime - StartTime);

	float FinalScore = AccumulatedReward + (TimeAlive * SurvivalTimeWeight);
	
	return FMath::Max(0.0f, FinalScore); // Never return negative fitness
}