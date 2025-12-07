#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Modules/ModuleManager.h"
#include "BehaviorTree/BTNode.h"
#include "EdGraph/EdGraphNode.h"

// Forward declarations
class UBehaviorTreeGraph;
class UBehaviorTreeGraphNode;

class BEHAVIOURTREEGRAPH_API FBehaviourTreeGraphModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FBehaviourTreeGraphModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBehaviourTreeGraphModule>("BehaviourTreeGraph");
	}

	/** * Main Entry point: 
	 * 1. Loads the Asset
	 * 2. Wipes existing Graph (if any)
	 * 3. Rebuilds Graph from Runtime RootNode
	 * 4. Saves Package
	 */
	void BuildGraphForBehaviorTree(const FString& AssetPackagePath);

private:
	void Internal_BuildGraph(UBehaviorTree* BTAsset);

	/** Recursive function to create Graph Nodes from Runtime Nodes */
	UBehaviorTreeGraphNode* CreateGraphNodeForRuntimeNode(
		UBehaviorTreeGraph* Graph, 
		UBTNode* RuntimeNode, 
		int32 Depth, 
		int32& CurrentXCounter
	);

	/** Connects Parent Graph Node to Child Graph Node via Pins */
	void CreateConnection(UBehaviorTreeGraphNode* ParentNode, UBehaviorTreeGraphNode* ChildNode);

	/** Adds Decorators and Services to the Graph Node */
	void RebuildSubNodes(UBehaviorTreeGraphNode* HostGraphNode, UBTCompositeNode* RuntimeComposite);
};