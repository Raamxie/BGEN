// Fill out your copyright notice in the Description page of Project Settings.


#include "LostPawn.h"
#include "../AIAssets/LostPawnAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "../AIAssets/BTNodes/WalkToZero.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"

// Sets default values
ALostPawn::ALostPawn()
{
	// 1) Create and register a SceneComponent as our root (it carries the actor transform)
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// 2) Add a StaticMesh so you can see/move it
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SceneRoot);
	// (Optionally set a default mesh in C++ or in the Blueprint subclass)

	// AI auto-possession
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ALostPawnAIController::StaticClass();
	

}

// Called when the game starts or when spawned
void ALostPawn::BeginPlay()
{
	Super::BeginPlay();
	GenerateBehaviourTree();
	
	
	
}

// Called every frame
void ALostPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALostPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALostPawn::GenerateBehaviourTree() const
{
	AAIController* AaiController = Cast<AAIController>(GetController());
	if (!AaiController)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AI controller available for %d"), ID);
		return;
	}

	// Create blackboard asset dynamically
	// UBlackboardData* bbAsset = NewObject<UBlackboardData>(GetTransientPackage(), NAME_None, RF_Transient);
	// {
	// 	FBlackboardEntry entry;
	// 	entry.EntryName = "TargetActor";
	// 	entry.KeyType = NewObject<UBlackboardKeyType_Object>(bbAsset);
	// 	bbAsset->Keys.Add(entry);
	// }

	// Create behavior tree asset dynamically
	UBehaviorTree* BTAsset = NewObject<UBehaviorTree>(GetTransientPackage(), NAME_None, RF_Transient);

	// Build root selector
	auto RootSelector = NewObject<UBTComposite_Selector>(BTAsset);
	BTAsset->RootNode = RootSelector;
	

	auto Sequence = NewObject<UBTComposite_Sequence>(BTAsset);
	{
		FBTCompositeChild SeqChild;
		SeqChild.ChildComposite = Sequence;
		RootSelector->Children.Add(SeqChild);
	}
	


	UBTTask_Wait* WaitTask = NewObject<UBTTask_Wait>(BTAsset);
	WaitTask->WaitTime = 2.0f;           // pause for 2 seconds

	FBTCompositeChild WaitChild;
	WaitChild.ChildTask = WaitTask;
	Sequence->Children.Add(WaitChild);

	// MoveTo task node
	UWalkToZero* WalkTask = NewObject<UWalkToZero>(BTAsset);
	FBTCompositeChild WalkChild;
	WalkChild.ChildTask = WalkTask;
	Sequence->Children.Add(WalkChild);
	

	AaiController->RunBehaviorTree(BTAsset);
}

void ALostPawn::SetID(const int NewID)
{
	ID = NewID;
	UE_LOG(LogTemp, Display, TEXT("Lost Pawn %d has spawned"), ID);
}

void ALostPawn::SetMesh(UStaticMesh* Mesh) const
{
	MeshComp->SetStaticMesh(Mesh);
}

void ALostPawn::AddStep()
{
	StepsTaken++;
}

void ALostPawn::SetEvolutionManager(AEvolutionManager* NewEvolutionManager)
{
	EvolutionManager = NewEvolutionManager;
}

AEvolutionManager* ALostPawn::GetEvolutionManager() const
{
	return EvolutionManager;
}

int ALostPawn::GetID() const
{
	return ID;
}

int ALostPawn::GetStepsTaken() const
{
	return StepsTaken;
}




