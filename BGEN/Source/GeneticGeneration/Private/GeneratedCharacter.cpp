#include "GeneratedCharacter.h"
#include "CustomAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TimerManager.h"
#include "Engine/World.h"

AGeneratedCharacter::AGeneratedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ACustomAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGeneratedCharacter::BeginPlay()
{
	Super::BeginPlay();

	ACustomAIController* C = Cast<ACustomAIController>(GetController());
	if (!C)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller not ready yet, delaying init..."));
		// schedule a tick-late attempt
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AGeneratedCharacter::InitializeDelayed);
		return;
	}

	InitializeAI(C);
}

void AGeneratedCharacter::InitializeDelayed()
{
	ACustomAIController* C = Cast<ACustomAIController>(GetController());
	if (C)
	{
		InitializeAI(C);
	}
}

void AGeneratedCharacter::InitializeAI(ACustomAIController* ControllerIn)
{
	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("No AIController!"));
		return;
	}

	// Example: Optionally load and assign a pre-authored BT from disk instead of runtime one
	// UBehaviorTree* BT = LoadObject<UBehaviorTree>(nullptr, TEXT("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed"));
	// if (BT && BT->BlackboardAsset)
	// {
	//     Controller->AssignTree(BT, BT->BlackboardAsset);
	//     Controller->RunAssignedTree();
	// }

	// In our workflow we use runtime BT created in OnPossess() of controller, so nothing to do here.
}

void AGeneratedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
