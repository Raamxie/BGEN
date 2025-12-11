#include "CustomBehaviourTree.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"

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
	DebugLogTree();

	return true;
}

void UCustomBehaviourTree::InitFromTreeInstance(UBehaviorTree* SourceTree)
{
	if (!SourceTree)
	{
		UE_LOG(LogTemp, Error, TEXT("InitFromTreeInstance: SourceTree is null!"));
		return;
	}

	// Create a new unique name for this generation's variant
	FString NewName = FString::Printf(TEXT("%s_MutableCopy"), *SourceTree->GetName());

	// DEEP COPY the asset into this object (Transient)
	BehaviorTreeAsset = DuplicateObject<UBehaviorTree>(SourceTree, this, *NewName);

	// Detach from any old graph to prevent editor crashes if we open it
	BehaviorTreeAsset->BTGraph = nullptr;

	UE_LOG(LogTemp, Log, TEXT("Cloned Tree: %s -> %s"), *SourceTree->GetName(), *BehaviorTreeAsset->GetName());
}

UBTCompositeNode* UCustomBehaviourTree::GetRootComposite()
{
    if (!BehaviorTreeAsset)
    {
        return nullptr;
    }
    return BehaviorTreeAsset->RootNode;
}

bool UCustomBehaviourTree::SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting)
{
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: No BehaviorTree loaded to save."));
        return false;
    }
    if (DestinationPackagePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DestinationPackagePath is empty."));
        return false;
    }

    // 1. Prepare Package Name
    FString PackageName = DestinationPackagePath;
    FString AssetName = FPackageName::GetShortName(PackageName);

    // 2. Create the Package
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: Failed to create package %s"), *PackageName);
        return false;
    }
    Package->FullyLoad();

    // 3. Duplicate the Object
    // We duplicate the runtime asset into the new package.
    UBehaviorTree* NewBT = DuplicateObject<UBehaviorTree>(BehaviorTreeAsset, Package, *AssetName);
    if (!NewBT)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DuplicateObject failed."));
        return false;
    }

    // 4. CRITICAL: Sanitize the New Asset
    // We must ensure the new BT doesn't point to the old Editor Graph (if it existed),
    // otherwise opening it will crash or show the old tree.
    // Your 'BehaviourTreeGraphModule' will rebuild this later.
    NewBT->BTGraph = nullptr; 
    
    // Set Flags required for an Editor Asset
    NewBT->SetFlags(RF_Public | RF_Standalone | RF_Transactional);

    // 5. Notify Asset Registry
    FAssetRegistryModule::AssetCreated(NewBT);
    Package->MarkPackageDirty();

    // 6. Save the Package (Fixed API Call)
    FString Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.Error = GError;
    SaveArgs.SaveFlags = SAVE_NoError; // Optional: Enforce no errors
    
    // UPackage::SavePackage is the correct static function in UE5
    bool bSaved = UPackage::SavePackage(Package, NewBT, *Filename, SaveArgs);

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: Failed to save package to %s"), *Filename);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("SaveAsNewAsset: Success! Saved to %s"), *Filename);
    return true;
}

void UCustomBehaviourTree::DebugMutation()
{
	UE_LOG(LogTemp, Log, TEXT("DebugMutation"));
}



/// MUTATION

TArray<UClass*> UCustomBehaviourTree::GetAvailableTaskClasses(const FString& Path)
{
	TArray<UClass*> ResultClasses;

	// 1. Get the Asset Registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	// 2. Define Filter
	FARFilter Filter;
	Filter.PackagePaths.Add(*Path);
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName()); // We are looking for Blueprints

	// 3. Search
	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	for (const FAssetData& Asset : AssetList)
	{
		// Load the Blueprint Asset
		UObject* LoadedAsset = Asset.GetAsset();
		if (UBlueprint* BP = Cast<UBlueprint>(LoadedAsset))
		{
			// We need the Generated Class (the actual class the game executes)
			if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(UBTTaskNode::StaticClass()))
			{
				ResultClasses.Add(BP->GeneratedClass);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d valid Task classes in %s"), ResultClasses.Num(), *Path);
	return ResultClasses;
}

void UCustomBehaviourTree::CollectNodes(UBTNode* Node, TArray<UBTCompositeNode*>& OutComposites, TArray<UBTTaskNode*>& OutTasks)
{
	if (!Node) return;

	if (UBTCompositeNode* Comp = Cast<UBTCompositeNode>(Node))
	{
		OutComposites.Add(Comp);
		for (FBTCompositeChild& Child : Comp->Children)
		{
			// Child can be a task or another composite
			if (Child.ChildComposite)
			{
				CollectNodes(Child.ChildComposite, OutComposites, OutTasks);
			}
			else if (Child.ChildTask)
			{
				OutTasks.Add(Child.ChildTask);
			}
		}
	}
}

bool UCustomBehaviourTree::MutateTree_Dynamic(const FString& SearchPath)
{
	if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode)
	{
		UE_LOG(LogTemp, Error, TEXT("MutateTree_Dynamic: Tree is invalid or empty."));
		return false;
	}

	// 1. Fetch Candidates (Genes)
	TArray<UClass*> Candidates = GetAvailableTaskClasses(SearchPath);
	if (Candidates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MutateTree_Dynamic: No task classes found in %s"), *SearchPath);
		return false;
	}

	// 2. Analyze current Phenotype (The Tree)
	TArray<UBTCompositeNode*> Composites;
	TArray<UBTTaskNode*> Tasks;
	CollectNodes(BehaviorTreeAsset->RootNode, Composites, Tasks);

	// 3. Choose Mutation Strategy
	bool bDoSwap = FMath::RandBool();

	// Fallback: If we can't swap (no tasks), force insertion. If we can't insert (no composites), fail.
	if (Tasks.Num() == 0) bDoSwap = false;
	if (Composites.Num() == 0) return false;

	UClass* ChosenClass = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];

	if (bDoSwap)
	{
		// --- STRATEGY: SWAP ---
		// Pick a victim task to replace
		UBTTaskNode* Victim = Tasks[FMath::RandRange(0, Tasks.Num() - 1)];
		UBTCompositeNode* Parent = Victim->GetParentNode(); // Valid because we collected it via traversal

		if (Parent)
		{
			// Create new node
			UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTreeAsset, ChosenClass, NAME_None, RF_Public | RF_Standalone);
			NewTask->InitializeFromAsset(*BehaviorTreeAsset); // Vital for initializing properties

			// Find victim index in parent and swap
			for (int32 i = 0; i < Parent->Children.Num(); i++)
			{
				if (Parent->Children[i].ChildTask == Victim)
				{
					Parent->Children[i].ChildTask = NewTask;
					UE_LOG(LogTemp, Log, TEXT("MutateTree: Replaced %s with %s"), *Victim->GetName(), *NewTask->GetName());
					break;
				}
			}
		}
	}
	else
	{
		// --- STRATEGY: INSERT ---
		// Pick a parent composite
		UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];
		
		// Create new node
		UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTreeAsset, ChosenClass, NAME_None, RF_Public | RF_Standalone);
		NewTask->InitializeFromAsset(*BehaviorTreeAsset);

		// Add as new child
		FBTCompositeChild NewChild;
		NewChild.ChildTask = NewTask;
		Parent->Children.Add(NewChild);

		UE_LOG(LogTemp, Log, TEXT("MutateTree: Inserted %s into %s"), *NewTask->GetName(), *Parent->GetName());
	}

	return true;
}



// DEBUG VISUALIZATION

void UCustomBehaviourTree::DebugLogTree()
{
	if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugLogTree: No Tree/Root found."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("================================================="));
	UE_LOG(LogTemp, Warning, TEXT(" TREE STRUCTURE: %s"), *BehaviorTreeAsset->GetName());
	UE_LOG(LogTemp, Warning, TEXT(" Legend: [S]=Service, {D}=Decorator, (Node)"));
	UE_LOG(LogTemp, Warning, TEXT("================================================="));

	// Start recursion with empty prefix
	PrintPrettyNode(BehaviorTreeAsset->RootNode, "", true);

	UE_LOG(LogTemp, Warning, TEXT("================================================="));
}

void UCustomBehaviourTree::PrintPrettyNode(UBTNode* Node, FString Prefix, bool bIsLast)
{
    if (!Node) return;

    // 1. Determine the connector for THIS node
    // If last child: "L--", else "|--"
    FString Connector = bIsLast ? TEXT("L-- ") : TEXT("|-- ");
    
    // 2. Print the Node itself
    UE_LOG(LogTemp, Display, TEXT("%s%s%s"), *Prefix, *Connector, *GetCleanNodeName(Node));

    // 3. Prepare the prefix for the CHILDREN of this node
    // If we are the last child, our children shouldn't see our vertical bar.
    FString ChildPrefix = Prefix + (bIsLast ? TEXT("    ") : TEXT("|   "));

    UBTCompositeNode* Composite = Cast<UBTCompositeNode>(Node);
    if (Composite)
    {
        // --- PRINT SERVICES ---
        // Services belong to the composite itself. We print them "hanging" off the composite.
        for (UBTService* Service : Composite->Services)
        {
            if (Service)
            {
                // Print slightly indented with a specific marker
                UE_LOG(LogTemp, Log, TEXT("%s . [S] %s"), *ChildPrefix, *GetCleanNodeName(Service));
            }
        }

        // --- PRINT CHILDREN ---
        for (int32 i = 0; i < Composite->Children.Num(); i++)
        {
            FBTCompositeChild& ChildLink = Composite->Children[i];
            bool bIsLastChild = (i == Composite->Children.Num() - 1);

            // Print Decorators (Logic Gates) for this specific branch
            // We print them *before* recursing into the child node
            for (UBTDecorator* Deco : ChildLink.Decorators)
            {
                if (Deco)
                {
                    // If it's the last child, the decorator sits above the "L--", else above "|--"
                    // We use a specific symbol to show it's a gate.
                    FString DecoPrefix = ChildPrefix + (bIsLastChild ? TEXT("    ") : TEXT("|   ")); 
                    
                    // Note: Visually connecting decorators to the specific branch line is tricky in ASCII.
                    // A simple approach is just listing them above the child recursion.
                    UE_LOG(LogTemp, Log, TEXT("%s : {D} %s"), *ChildPrefix, *GetCleanNodeName(Deco));
                }
            }

            // Recurse
            UBTNode* ChildNode = ChildLink.ChildComposite ? (UBTNode*)ChildLink.ChildComposite : (UBTNode*)ChildLink.ChildTask;
            PrintPrettyNode(ChildNode, ChildPrefix, bIsLastChild);
        }
    }
}

FString UCustomBehaviourTree::GetCleanNodeName(UBTNode* Node)
{
	if (!Node) return TEXT("NULL");

	FString Name = Node->GetClass()->GetName();
    
	// Remove standard prefixes/suffixes for readability
	Name.RemoveFromStart("BTComposite_");
	Name.RemoveFromStart("BTTask_");
	Name.RemoveFromStart("BTDecorator_");
	Name.RemoveFromStart("BTService_");
	Name.RemoveFromEnd("_C"); // If using Blueprints

	// Append the specific instance name if it's different/set
	if (!Node->NodeName.IsEmpty() && !Name.Equals(Node->NodeName))
	{
		Name += FString::Printf(TEXT(" (\"%s\")"), *Node->NodeName);
	}
    
	return Name;
}


