#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Modules/ModuleManager.h"

class BEHAVIOURTREEGRAPH_API FBehaviourTreeGraphModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Build the Behavior Tree editor graph for a BehaviorTree asset at the given package path. */
	void BuildGraphForBehaviorTree(const FString& AssetPackagePath);
	static inline FBehaviourTreeGraphModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBehaviourTreeGraphModule>("BehaviourTreeGraph");
	}

private:
	void Internal_BuildGraph(UBehaviorTree* BTAsset);
};
