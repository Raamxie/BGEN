// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CustomAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UCustomBehaviourTree;

/**
 * Custom AI Controller that supports runtime-generated behavior trees
 */
UCLASS()
class GENETICGENERATION_API ACustomAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACustomAIController();

	virtual void OnPossess(APawn* InPawn) override;

	/** Assign an existing behavior tree + blackboard to the controller (does not auto-run) */
	void AssignTree(UBehaviorTree* Tree, UBlackboardData* BBData);

	/** Run the assigned tree (if any) */
	void RunAssignedTree();

	/** A runtime helper object that builds/starts behavior trees */
	UPROPERTY()
	UCustomBehaviourTree* RuntimeBehaviourWrapper;

protected:
	/** Subobjects created in constructor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorComp;



	/** Stored tree assigned via AssignTree */
	UPROPERTY()
	UBehaviorTree* AssignedTree;
};
