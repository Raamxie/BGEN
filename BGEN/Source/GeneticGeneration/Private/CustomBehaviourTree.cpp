#include "CustomBehaviourTree.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AIController.h"
#include "GeneticGenerationModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "BehaviorTreeEditorModule.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "Modules/ModuleManager.h"
#include "Misc/Guid.h"
#include "UObject/UnrealType.h" // FProperty & friends
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "Serialization/Archive.h"
#include "Misc/Paths.h"
#include "Misc/FeedbackContext.h"
#include "BehaviorTreeFactory.h"



UCustomBehaviourTree::UCustomBehaviourTree()
{
    BehaviorTree = nullptr;
    BlackboardData = nullptr;
    ActiveComponent = nullptr;
}

UBehaviorTree* UCustomBehaviourTree::LoadBehaviorTree(const FString& AssetPath)
{
    UObject* Obj = StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *AssetPath);
    return Cast<UBehaviorTree>(Obj);
}

void UCustomBehaviourTree::CreateRuntimeBlackboard()
{
    if (!BlackboardData)
    {
        // Create runtime BlackboardData owned by this UObject
        BlackboardData = NewObject<UBlackboardData>(this, UBlackboardData::StaticClass(), TEXT("RuntimeBlackboard"));
        if (!BlackboardData)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create runtime BlackboardData"));
            return;
        }
    }

    // Clear previous keys if any
    BlackboardData->Keys.Empty();

    // Add keys required by the user
    AddBlackboardKey(TEXT("SightRange"), UBlackboardKeyType_Int::StaticClass());
    AddBlackboardKey(TEXT("Player"), UBlackboardKeyType_Object::StaticClass());
    AddBlackboardKey(TEXT("AttackRange"), UBlackboardKeyType_Int::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("Created runtime blackboard with %d keys"), BlackboardData->Keys.Num());
}

void UCustomBehaviourTree::AddBlackboardKey(FName KeyName, UClass* KeyTypeClass)
{
    if (!BlackboardData || !KeyTypeClass) return;

    FBlackboardEntry Entry;
    Entry.EntryName = KeyName;
    // Create a KeyType instance owned by the BlackboardData
    Entry.KeyType = NewObject<UBlackboardKeyType>(BlackboardData, KeyTypeClass);
    BlackboardData->Keys.Add(Entry);

    UE_LOG(LogTemp, Log, TEXT("Added blackboard key %s"), *KeyName.ToString());
}

void UCustomBehaviourTree::StartTree(AAIController* Controller)
{
    if (!Controller)
    {
        UE_LOG(LogTemp, Error, TEXT("StartTree failed — Controller is null."));
        return;
    }

    if (!BehaviorTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("No BehaviorTree available to start."));
        return;
    }

    // Ensure controller has a BlackboardComponent (we expect the controller to have created one as a subobject)
    UBlackboardComponent* BBC = Controller->FindComponentByClass<UBlackboardComponent>();
    if (!BBC)
    {
        // As a fallback, create one and register it (rare if controller constructor created one)
        BBC = NewObject<UBlackboardComponent>(Controller, TEXT("RuntimeCreatedBlackboardComp"));
        if (BBC)
        {
            BBC->RegisterComponent();
        }
    }

    if (!BBC)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get or create a BlackboardComponent on controller %s"), *Controller->GetName());
        return;
    }

    // Initialize the controller's blackboard with our runtime blackboard data
    if (BlackboardData)
    {
        BBC->InitializeBlackboard(*BlackboardData);
        BBC->SetValueAsInt(TEXT("SightRange"), 5000);
        BBC->SetValueAsInt(TEXT("AttackRange"), 1000);
    }

    // Use the controller's RunBehaviorTree which will make its internal brain component run the provided tree
    const bool bStarted = Controller->RunBehaviorTree(BehaviorTree);
    if (bStarted)
    {
        // Cache the running component pointer
        ActiveComponent = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
        UE_LOG(LogTemp, Log, TEXT("Started runtime BehaviorTree on controller %s"), *Controller->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Controller failed to start runtime BehaviorTree %s"), *Controller->GetName());
    }
}

// This function is correct as-is
UBTTaskNode* UCustomBehaviourTree::LoadAndInstanceTask(const FString& Path, UObject* Outer)
{
	if (!Outer)
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("LoadAndInstanceTask requires a valid Outer."));
		return nullptr;
	}

	// Load BlueprintGeneratedClass or native class
	UClass* TaskClass = StaticLoadClass(UBTTaskNode::StaticClass(), nullptr, *Path);

	if (!TaskClass)
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to load class at path: %s"), *Path);
		return nullptr;
	}

	// Create instance
	UBTTaskNode* Node = NewObject<UBTTaskNode>(Outer, TaskClass);

	if (!Node)
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to instantiate task: %s"), *Path);
	}

	return Node;
}




void UCustomBehaviourTree::StopTree()
{
    if (!ActiveComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("No active BT component to stop"));
        return;
    }

    if (ActiveComponent->IsRunning())
    {
        ActiveComponent->StopTree(EBTStopMode::Safe);
        UE_LOG(LogTemp, Log, TEXT("Stopped runtime BehaviorTree"));
    }

    ActiveComponent = nullptr;
}