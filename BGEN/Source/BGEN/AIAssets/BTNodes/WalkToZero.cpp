// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkToZero.h"

#include "Kismet/KismetMathLibrary.h"
#include "../LostPawnAIController.h"
#include "BGEN/Actors/EvolutionManager.h"
#include "BGEN/Actors/LostPawn.h"


UWalkToZero::UWalkToZero()
{
	NodeName = "Move Toward Origin if Closer";
}

EBTNodeResult::Type UWalkToZero::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
		return EBTNodeResult::Failed;

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
		return EBTNodeResult::Failed;
	
	const FVector CurrentLocation = Pawn->GetActorLocation();
	const FVector Origin = FVector::ZeroVector;

	const float CurrentDist = FVector::Dist(CurrentLocation, Origin);

	if (ALostPawn* LostPawn = Cast<ALostPawn>(Pawn))
	{
		LostPawn->AddStep();
		AEvolutionManager* Manager = LostPawn->GetEvolutionManager();
		if (CurrentDist < Manager->GetAcceptanceRange())
		{
			Manager->FinishedTask(LostPawn->GetID(), LostPawn->GetStepsTaken());
			
			OwnerComp.StopTree();
		}
		
	}
	
	FVector2D Rand2D = FMath::RandPointInCircle(1.0f);

	// Convert to 3D and normalize (to ensure it’s exactly unit-length)
	FVector RandomDir2D = FVector(Rand2D, 0.0f).GetSafeNormal();
	const float StepSize = UKismetMathLibrary::RandomFloatInRange(0.5f, 300.f);
	
	const FVector NewLocation = CurrentLocation + RandomDir2D * StepSize; // adjust step size as needed

	const float NewDist = FVector::Dist(NewLocation, Origin);

	if (NewDist < CurrentDist)
	{
		Pawn->SetActorLocation(NewLocation, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}