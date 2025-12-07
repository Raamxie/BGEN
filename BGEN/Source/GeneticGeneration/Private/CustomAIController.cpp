#include "CustomAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CustomBehaviourTree.h"

ACustomAIController::ACustomAIController()
{
    // Create the standard components as subobjects so UE owns them
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

    // Ensure AAIController knows about them (AAIController expects a BrainComponent)
    // We don't assign BrainComponent directly here; RunBehaviorTree will set up the BrainComponent using the UBehaviorTreeComponent subobject
    // But keeping BehaviorComp available as a subobject is important.
}

void ACustomAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // If user assigned a static tree earlier, optionally run it:
    if (AssignedTree)
    {
        RunAssignedTree();
    }
}

void ACustomAIController::AssignTree(UBehaviorTree* Tree, UBlackboardData* BBData)
{
    AssignedTree = Tree;

    if (BBData && BlackboardComp)
    {
        BlackboardComp->InitializeBlackboard(*BBData);
    }
}

void ACustomAIController::RunAssignedTree()
{
    if (!AssignedTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("No tree assigned"));
        return;
    }

    RunBehaviorTree(AssignedTree);
}


void ACustomAIController::BeginPlay()
{
	Super::BeginPlay();
	DamageDealt = 0.0f;
	SpawnTime = GetWorld()->GetTimeSeconds();

}

void ACustomAIController::RecordDamageDealt(float Amount)
{
	DamageDealt += Amount;
}

float ACustomAIController::GetTimeAlive() const
{
	if (GetWorld())
	{
		return GetWorld()->GetTimeSeconds() - SpawnTime;
	}
	return 0.0f;
}
