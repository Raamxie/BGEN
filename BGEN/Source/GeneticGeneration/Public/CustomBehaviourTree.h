#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomBehaviourTree.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UBlackboardComponent;
class AAIController;
class UBehaviorTreeComponent;
class UBTTaskNode;

/**
 * Lightweight helper to generate and run runtime Behavior Trees + Blackboard
 */
UCLASS()
class GENETICGENERATION_API UCustomBehaviourTree : public UObject
{
	GENERATED_BODY()

public:
	UCustomBehaviourTree();

	/** Load a BehaviorTree asset from path (keeps asset pointer) */
	UBehaviorTree* LoadBehaviorTree(const FString& AssetPath);

	/** Generate the simple runtime tree described by the user
	 *  Structure: Selector -> Sequence -> [CheckArea, MoveTo, Attack]
	 */
	UBehaviorTree* GenerateSimpleRuntimeTree();

	/** Start the runtime tree on the provided controller (initializes blackboard etc) */
	void StartTree(AAIController* Controller);

	/** Stop the currently running tree (if any) */
	void StopTree();
	void Test_ModifyTreeAndSave(const FString& SourceBTPath, const FString& SavePackagePath);

	/** Create runtime blackboard and add required keys */
	void CreateRuntimeBlackboard();

	/** Add a blackboard key (helper) */
	void AddBlackboardKey(FName KeyName, UClass* KeyTypeClass);

	/** Duplicate/load a task and instance it under the BehaviorTree */
	UBTTaskNode* LoadAndInstanceTask(const FString& Path, UObject* Outer);

	void Test_ModifyTreeAndSave_WithEditorGraph(const FString& SourceBTPath, const FString& SavePackagePath);

protected:
	/** Runtime objects owned by this helper */
	UPROPERTY()
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	UBlackboardData* BlackboardData;

	/** If the BT is running, store the active BT component pointer for stop/cleanup */
	UPROPERTY()
	UBehaviorTreeComponent* ActiveComponent;
};
