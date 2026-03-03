#include "GeneticFitnessTracker.h"
#include "GeneticGenerationModule.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "Engine/World.h"
#include "TimerManager.h"

UGeneticFitnessTracker::UGeneticFitnessTracker()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UGeneticFitnessTracker::BeginPlay()
{
	Super::BeginPlay();
}

void UGeneticFitnessTracker::BeginTracking(AActor* InTargetPlayer)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	Owner->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);
	Owner->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);

	TargetPlayer = InTargetPlayer;

	APlayerUnleashedBase* PlayerUnleashed = Cast<APlayerUnleashedBase>(TargetPlayer);
	if (PlayerUnleashed)
	{
		PlayerUnleashed->OnPlayerEvent.RemoveDynamic(this, &UGeneticFitnessTracker::OnPlayerDied);
		PlayerUnleashed->OnPlayerEvent.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerDied);

		PlayerUnleashed->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
		PlayerUnleashed->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
	}

	LastRecordedLocation = Owner->GetActorLocation();
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &UGeneticFitnessTracker::RecordMovementRoutine, 0.5f, true);

	StartTime = GetWorld()->GetTimeSeconds();
	bTrackingActive = true;
	bPlayerWasKilled = false;
	bDamagedPlayer = false;
	AccumulatedReward = 0.0f;
	AccumulatedDamageTaken = 0.0f;
	AccumulatedDistance = 0.0f;
}

void UGeneticFitnessTracker::RecordMovementRoutine()
{
	if (!bTrackingActive) return;

	AActor* Owner = GetOwner();
	if (Owner)
	{
		FVector CurrentLocation = Owner->GetActorLocation();
		AccumulatedDistance += FVector::Dist2D(LastRecordedLocation, CurrentLocation);
		LastRecordedLocation = CurrentLocation;
	}
}
void UGeneticFitnessTracker::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;
	AccumulatedDamageTaken += Damage;
}

void UGeneticFitnessTracker::OnPlayerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;

	if (DamageCauser == GetOwner() || InstigatedBy == GetOwner()->GetInstigatorController())
	{
		AccumulatedReward += (Damage * DamageDealtWeight) + SuccessfulAttackBonus;
		bDamagedPlayer = true;
	}
}

void UGeneticFitnessTracker::OnPlayerDied()
{
	if (!bTrackingActive) return;
	bPlayerWasKilled = true;
}

void UGeneticFitnessTracker::AddCustomReward(float Amount)
{
	if (bTrackingActive) AccumulatedReward += Amount;
}

int32 UGeneticFitnessTracker::GetTreeSize() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn)
	{
		if (ACustomAIController* AI = Cast<ACustomAIController>(OwnerPawn->GetController()))
		{
			if (AI->RuntimeBehaviourWrapper && AI->RuntimeBehaviourWrapper->GetBTAsset())
			{
				TArray<UBTCompositeNode*> Composites;
				TArray<UBTTaskNode*> Tasks;
				AI->RuntimeBehaviourWrapper->CollectNodes(AI->RuntimeBehaviourWrapper->GetBTAsset()->RootNode, Composites, Tasks);
				return Composites.Num() + Tasks.Num();
			}
		}
	}
	return 0; // Will trigger penalty if wrapper is missing or tree is empty
}

float UGeneticFitnessTracker::CalculateFitness()
{
	if (!bTrackingActive && AccumulatedReward == 0.0f) return 1.0f;

	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);

	double CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeAlive = (float)(CurrentTime - StartTime);

	// 1. BASE REWARDS (Include BaseFitnessScore buffer)
	float FinalScore = BaseFitnessScore + AccumulatedReward;
	FinalScore += (TimeAlive * SurvivalTimeWeight);
	FinalScore += (AccumulatedDistance * DistanceMovedWeight);

	if (TargetPlayer && GetOwner())
	{
		float FinalDistanceToPlayer = FVector::Dist(TargetPlayer->GetActorLocation(), GetOwner()->GetActorLocation());
		float ProximityScore = FMath::Max(0.0f, 2000.0f - FinalDistanceToPlayer); 
		FinalScore += (ProximityScore * ProximityBonusWeight);
	}

	// 2. PENALTIES
	FinalScore -= (AccumulatedDamageTaken * DamageTakenPenalty);

	if (!bDamagedPlayer) FinalScore -= PacifistPenalty;
	if (AccumulatedDistance < MinimumExpectedDistance) FinalScore -= IdlePenalty;

	int32 TreeSize = GetTreeSize();
	if (TreeSize < MinimumTreeNodes) FinalScore -= SmallTreePenalty;

	// 3. VICTORY CONDITIONS
	if (bPlayerWasKilled)
	{
		FinalScore += PlayerKillBonus;
		float TimeRemaining = FMath::Max(0.0f, 30.0f - TimeAlive);
		FinalScore += (TimeRemaining * 50.0f); 
	}

	UE_LOG(LogGeneticGeneration, Display, TEXT("Enemy Fitness: %f (Dist: %.1f | TreeSize: %d)"), FinalScore, AccumulatedDistance, TreeSize);
	
	// Ensure we NEVER return 0 or negative so the Manager doesn't throw the score out
	return FMath::Max(1.0f, FinalScore);
}