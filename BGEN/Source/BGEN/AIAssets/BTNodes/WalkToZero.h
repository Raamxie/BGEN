// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WalkToZero.generated.h"

/**
 * 
 */
UCLASS()
class BGEN_API UWalkToZero : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UWalkToZero();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
