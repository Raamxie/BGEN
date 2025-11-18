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

    // Create the runtime wrapper with this controller as outer to keep it alive while possessed.
    RuntimeBehaviourWrapper = NewObject<UCustomBehaviourTree>(this);

    if (RuntimeBehaviourWrapper)
    {
        UBehaviorTree* Generated = RuntimeBehaviourWrapper->GenerateSimpleRuntimeTree();
        if (Generated)
        {
            // Start the runtime tree on this controller (this will initialize the controller's blackboard component and run the tree)
            RuntimeBehaviourWrapper->StartTree(this);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Runtime behaviour wrapper generated no BehaviorTree"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UCustomBehaviourTree instance"));
    }

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
