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

	// 1. Existing Damage Binding...
	Owner->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);

	// 2. Find Player and Bind to Death Event
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	APlayerUnleashedBase* PlayerUnleashed = Cast<APlayerUnleashedBase>(PlayerActor);

	if (PlayerUnleashed)
	{
		// Subscribe to the event you showed me
		PlayerUnleashed->OnPlayerEvent.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerDied);
        
		// Also bind to damage for partial points
		PlayerUnleashed->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
	}

	StartTime = GetWorld()->GetTimeSeconds();
	bTrackingActive = true;
	bPlayerWasKilled = false; // Reset state
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

	return FMath::Max(0.0f, FinalScore);
}