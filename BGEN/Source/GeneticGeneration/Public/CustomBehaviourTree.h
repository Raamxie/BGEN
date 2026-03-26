#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CustomBehaviourTree.generated.h"

/**
 * Runtime helper to load, mutate and save a Behavior Tree.
 */


USTRUCT()
struct FEvolutionHistory
{
	GENERATED_BODY()

	FString ParentA_Path;
	FString ParentB_Path;
	FString SelectionMethod;
	
	FString CrossoverLog; // Details of the splice
	FString MutationLog;  // Details of the mutation
	
	// Snapshots of the parents (optional, can be large)
	FString ParentA_Structure;
	FString ParentB_Structure;
};


UCLASS()
class GENETICGENERATION_API UCustomBehaviourTree : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	bool LoadBehaviorTree(const FString& AssetPath);

	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	FString SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting = false);

	// --- EVOLUTION API ---
	
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	void InitFromTreeInstance(UBehaviorTree* SourceTree);
	

	// Returns the ASCII representation of the tree (reused for logging parents)
	FString GetTreeAsString();

	// Store the history here
	UPROPERTY()
	FEvolutionHistory EvolutionData;

	// Accessors
	UBehaviorTree* GetBTAsset() const { return BehaviorTreeAsset; }
	FString GetCleanNodeName(UBTNode* Node);

	// --- MUTATION HELPERS (Public for Library) ---
	
	TArray<UClass*> GetAvailableTaskClasses(const FString& Path);
	TArray<UClass*> GetAvailableDecoratorClasses(const FString& Path);
	TArray<UBehaviorTree*> GetAvailableTaskTrees(const FString& Path);
	
	// Helper to collect nodes for mutation library
	void CollectNodes(UBTNode* Node, TArray<UBTCompositeNode*>& OutComposites, TArray<UBTTaskNode*>& OutTasks);

	// Helper for Crossover
	struct FNodeHandle
	{
		UBTCompositeNode* Parent;
		int32 ChildIndex;
	};
	void GetAllCompositeSlots(UBTNode* Node, TArray<FNodeHandle>& OutSlots);
	void GetAllSubtrees(UBTNode* Node, TArray<UBTNode*>& OutNodes);

	UCustomBehaviourTree* PerformCrossover(UCustomBehaviourTree* DonorTreeWrapper, FString& OutLog);
	void DebugLogTree(FLogCategoryBase& Category);

	void PrintPrettyNode(UBTNode* Node, FString Prefix, bool bIsLast, FLogCategoryBase& Category);
	static UBTNode* DuplicateNodeRecursive(UBTNode* SourceNode, UBehaviorTree* TargetAsset);

	
	FString GetTreeStructureForHash();
	
	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	FString GetTreeHash();

private:
	UBehaviorTree* BehaviorTreeAsset = nullptr;
	
	// Internal helper for recursion
	void PrintNodeRecursive(auto& Builder, UBTNode* Node, FString Prefix, bool bIsLast);
};