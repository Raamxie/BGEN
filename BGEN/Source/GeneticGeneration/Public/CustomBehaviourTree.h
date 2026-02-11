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

	FString GetCleanNodeName(UBTNode* Node);

	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	bool MutateTree_Dynamic(const FString& SearchPath = "/Game/BehaviourTrees/Tasks");

	/// Save current tree into a new package: DestinationPackagePath is like "/Game/Generated/MyEvolvedBT"
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	FString SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting = false);
	void DebugMutation();
	void DebugLogTree();
	
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	void InitFromTreeInstance(UBehaviorTree* SourceTree);

	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	void AppendTreeToLogFile(const FString& FileName, int32 Generation, float Fitness);

	/// Get the in-memory UBehaviorTree instance
	UBehaviorTree* GetBTAsset() const { return BehaviorTreeAsset; }

private:
	UBehaviorTree* BehaviorTreeAsset = nullptr;

	UBTCompositeNode* GetRootComposite();

	
	void PrintPrettyNode(UBTNode* Node, FString Prefix, bool bIsLast);
	void PrintNodeRecursive(class UBTNode* Node, int32 Depth);

	// --- HELPERS ---
	/** Scans the Asset Registry for Blueprint classes derived from BTTaskNode */
	TArray<UClass*> GetAvailableTaskClasses(const FString& Path);

	/** Recursively collects all composites (branches) and tasks (leaves) */
	void CollectNodes(UBTNode* Node, TArray<UBTCompositeNode*>& OutComposites, TArray<UBTTaskNode*>& OutTasks);
};
