#include "CustomBehaviourTree.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"

bool UCustomBehaviourTree::LoadBehaviorTree(const FString& AssetPath)
{
    if (AssetPath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("LoadBehaviorTree: AssetPath is empty"));
        return false;
    }

    // Build object path
    FString ObjectPath = AssetPath;
    if (!AssetPath.Contains(TEXT(".")))
    {
        FString BaseName = FPaths::GetCleanFilename(AssetPath);
        ObjectPath = FString::Printf(TEXT("%s.%s"), *AssetPath, *BaseName);
    }

    UObject* Loaded = StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *ObjectPath);
    if (!Loaded)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadBehaviorTree: Failed to load object at path %s"), *ObjectPath);
        return false;
    }

    BehaviorTreeAsset = Cast<UBehaviorTree>(Loaded);
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadBehaviorTree: Loaded object is not UBehaviorTree: %s"), *ObjectPath);
        return false;
    }
	UPackage* Pkg = BehaviorTreeAsset->GetOutermost();
	if (Pkg)
	{
		Pkg->FullyLoad();        // Force all exports to instantiate
	}

	BehaviorTreeAsset->ConditionalPostLoad();

    UE_LOG(LogTemp, Log, TEXT("LoadBehaviorTree: Successfully loaded BT: %s"), *ObjectPath);
    return true;
}

UBTCompositeNode* UCustomBehaviourTree::GetRootComposite()
{
    if (!BehaviorTreeAsset)
    {
        return nullptr;
    }
    return BehaviorTreeAsset->RootNode;
}

bool UCustomBehaviourTree::MutateTree_AddSequenceWithAttack()
{
    UBTCompositeNode* Root = GetRootComposite();
    if (!Root)
    {
        UE_LOG(LogTemp, Error, TEXT("MutateTree: Root composite is null"));
        return false;
    }

    // 1) Remove first child
    if (Root->Children.Num() > 0)
    {
        Root->Children.RemoveAt(0);
        UE_LOG(LogTemp, Log, TEXT("MutateTree: Removed first child of root"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MutateTree: No child to remove"));
    }

    // 2) Insert a Sequence as first child
    UObject* Outer = BehaviorTreeAsset;
    UBTComposite_Sequence* SequenceNode = NewObject<UBTComposite_Sequence>(Outer, UBTComposite_Sequence::StaticClass(), NAME_None, RF_Public | RF_Standalone);
    if (!SequenceNode)
    {
        UE_LOG(LogTemp, Error, TEXT("MutateTree: Failed to create sequence node"));
        return false;
    }
    SequenceNode->SetFlags(RF_Public | RF_Standalone);

    FBTCompositeChild SeqChild;
    SeqChild.ChildComposite = SequenceNode;
    Root->Children.Insert(SeqChild, 0);
    UE_LOG(LogTemp, Log, TEXT("MutateTree: Inserted sequence as first child"));

    // 3) Add a task child of type Attack
	// If your BTTask_Attack is a Blueprint task in /Game/AI/Tasks/BTTask_Attack.BTTask_Attack_C
	UClass* AttackTaskClass = LoadObject<UClass>(
		nullptr,
		TEXT("/Game/BehaviourTrees/Tasks/BTTask_Attack.BTTask_Attack_C")
	);

    if (!AttackTaskClass)
    {
        AttackTaskClass = UBTTaskNode::StaticClass();
        UE_LOG(LogTemp, Warning, TEXT("MutateTree: Could not find BTTask_Attack, using UBTTaskNode fallback"));
    }

    UBTTaskNode* TaskNode = NewObject<UBTTaskNode>(Outer, AttackTaskClass, NAME_None, RF_Public | RF_Standalone);
    if (!TaskNode)
    {
        UE_LOG(LogTemp, Error, TEXT("MutateTree: Failed to create task node"));
        return false;
    }
    TaskNode->SetFlags(RF_Public | RF_Standalone);
    TaskNode->NodeName = TEXT("Attack");

    FBTCompositeChild TaskChild;
    TaskChild.ChildTask = TaskNode;
    SequenceNode->Children.Add(TaskChild);

    UE_LOG(LogTemp, Log, TEXT("MutateTree: Added Attack task under the sequence"));

    return true;
}

bool UCustomBehaviourTree::SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting)
{
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: No BehaviorTree loaded"));
        return false;
    }
    if (DestinationPackagePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DestinationPackagePath is empty"));
        return false;
    }

    FString PackageName = DestinationPackagePath;
    FString AssetName = FPackageName::GetShortName(PackageName);
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: Failed to create package %s"), *PackageName);
        return false;
    }

    EObjectFlags Flags = RF_Public | RF_Standalone;

	UBehaviorTree* NewBT = DuplicateObject<UBehaviorTree>(BehaviorTreeAsset, Package);
	if (!NewBT)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DuplicateObject failed"));
		return false;
	}

	NewBT->Rename(*AssetName, Package);
	NewBT->SetFlags(RF_Public | RF_Standalone);

	Package->MarkPackageDirty();
	Package->FullyLoad();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().AssetCreated(NewBT);

	FString Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;

	FSavePackageResultStruct result = UPackage::Save(Package, NewBT, *Filename, SaveArgs);
	if (!result.IsSuccessful())
	{
		UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: Failed to save package %s"), *Filename);
		return false;
	}
	return true;
}
