// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkToZero.h"

#include "Kismet/KismetMathLibrary.h"
#include "../LostPawnAIController.h"


UWalkToZero::UWalkToZero()
{
	NodeName = "Move Toward Origin if Closer";
}

EBTNodeResult::Type UWalkToZero::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (!controller)
		return EBTNodeResult::Failed;

	APawn* pawn = controller->GetPawn();
	if (!pawn)
		return EBTNodeResult::Failed;

	FVector currentLocation = pawn->GetActorLocation();
	FVector origin = FVector::ZeroVector;

	float currentDist = FVector::Dist(currentLocation, origin);

	// Generate random unit direction in X/Y plane
	FVector randomDir = UKismetMathLibrary::RandomUnitVector();
	randomDir.Z = 0.f;
	FVector newLocation = currentLocation + randomDir * 300.f; // adjust step size as needed

	float newDist = FVector::Dist(newLocation, origin);

	if (newDist < currentDist)
	{
		pawn->SetActorLocation(newLocation, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}