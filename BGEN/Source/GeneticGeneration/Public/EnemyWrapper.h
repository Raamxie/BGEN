// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"

// Forward declarations prevent "unknown type" errors and circular dependencies
class AAIController;
class ACharacter;
class UBlackboardComponent;
class UBehaviorTreeComponent;

/**
 * Wrapper class to hold references to an individual entity in the population.
 * * NOTE: Since this is a raw C++ class (not a UCLASS/USTRUCT), it does not participate 
 * automatically in Garbage Collection. Ensure the UObjects pointed to here (like the Character) 
 * are kept alive by the World or other UPROPERTY references.
 */
class GENETICGENERATION_API EnemyWrapper
{
public:
	EnemyWrapper();
	~EnemyWrapper();

private:
	UBehaviorTree* BehaviorTree = nullptr;
	AAIController* AIController = nullptr;
	ACharacter* Character = nullptr;
	UBlackboardComponent* BlackboardComponent = nullptr;
	UBehaviorTreeComponent* BehaviorTreeComp = nullptr;

public:
	void SetBehaviorTree(UBehaviorTree* InBehaviorTree)
	{
		BehaviorTree = InBehaviorTree;
	}

	void SetAIController(AAIController* InAIController)
	{
		AIController = InAIController;
	}

	void SetPawn(ACharacter* InCharacter)
	{
		Character = InCharacter;
	}

	void SetBlackboardComponent(UBlackboardComponent* InBlackboardComponent)
	{
		BlackboardComponent = InBlackboardComponent;
	}

	void SetBehaviorTreeComp(UBehaviorTreeComponent* InBehaviorTreeComp)
	{
		BehaviorTreeComp = InBehaviorTreeComp;
	}

	float CalculateFitness();
};