#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CustomBehaviourTree.generated.h"

/**
 * Runtime helper to load, mutate and save a Behavior Tree.
 */
UCLASS()
class GENETICGENERATION_API UCustomBehaviourTree : public UObject
{
	GENERATED_BODY()

public:
	/// Load an existing BehaviorTree (e.g. "/Game/AI/BT_Original")
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	bool LoadBehaviorTree(const FString& AssetPath);

	/// Apply a fixed mutation:
	/// 1. Remove first child of root
	/// 2. Insert Sequence as first composite child
	/// 3. Add BTTask_Attack (or fallback) under that sequence.
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	bool MutateTree_AddSequenceWithAttack();

	/// Save current tree into a new package: DestinationPackagePath is like "/Game/Generated/MyEvolvedBT"
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	bool SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting = false);

	/// Get the in-memory UBehaviorTree instance
	UBehaviorTree* GetBTAsset() const { return BehaviorTreeAsset; }

private:
	UBehaviorTree* BehaviorTreeAsset = nullptr;

	UBTCompositeNode* GetRootComposite();
};
