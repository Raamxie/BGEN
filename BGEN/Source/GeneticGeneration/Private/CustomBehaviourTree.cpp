#include "CustomBehaviourTree.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
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

FString UCustomBehaviourTree::SaveAsNewAsset(const FString& DestinationPackagePath, bool bOverwriteExisting)
{
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: No BehaviorTree loaded to save."));
        return FString();
    }
    if (DestinationPackagePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DestinationPackagePath is empty."));
        return FString();
    }

    // 1. Prepare Package Name
    FString PackageName = DestinationPackagePath;
    FString AssetName = FPackageName::GetShortName(PackageName);

    // 2. Create the Package
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: Failed to create package %s"), *PackageName);
        return FString();
    }
    Package->FullyLoad();

    // 3. Duplicate the Object
    // We duplicate the runtime asset into the new package.
    UBehaviorTree* NewBT = DuplicateObject<UBehaviorTree>(BehaviorTreeAsset, Package, *AssetName);
    if (!NewBT)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveAsNewAsset: DuplicateObject failed."));
        return FString();
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

	if (bSaved)
	{
		UE_LOG(LogTemp, Log, TEXT("Saved to %s"), *Filename);

		return PackageName + "." + AssetName; 
	}
	return FString();
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
    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("MutateTree_Dynamic: No Asset Loaded."));
        return false;
    }

    // --- STEP 1: BOOTSTRAP ROOT (If completely null) ---
    if (!BehaviorTreeAsset->RootNode)
    {
        // Create a default Root Selector if the asset has absolutely nothing
        UBTComposite_Selector* NewRoot = NewObject<UBTComposite_Selector>(BehaviorTreeAsset, TEXT("RootSelector"), RF_Public | RF_Standalone);
        NewRoot->InitializeFromAsset(*BehaviorTreeAsset);
        BehaviorTreeAsset->RootNode = NewRoot;
        UE_LOG(LogTemp, Warning, TEXT("MutateTree: Tree was null. Created Root Selector."));
    }

    // --- STEP 2: FETCH CANDIDATES ---
    TArray<UClass*> TaskClasses = GetAvailableTaskClasses(SearchPath);
    if (TaskClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("MutateTree: No task classes found in %s"), *SearchPath);
        return false;
    }

    // --- STEP 3: ANALYZE PHENOTYPE ---
    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    CollectNodes(BehaviorTreeAsset->RootNode, Composites, Tasks);

    // Safety check
    if (Composites.Num() == 0) return false;

    // --- STEP 4: CHOOSE STRATEGY ---
    // 0 = Swap Existing Task
    // 1 = Insert New Task (Flat)
    // 2 = Insert New Branch (Deep: Composite + Task)
    
    int32 Strategy = 0;

    // LOGIC UPDATE: Handle the "Empty Root" scenario
    if (Tasks.Num() == 0)
    {
        // If we have no tasks, we CANNOT swap (0).
        // To satisfy your request: "if there is no composite... add a composite and then insert action",
        // we FORCE Strategy 2. This prevents a flat 'Root -> Task' structure.
        Strategy = 2; 
        UE_LOG(LogTemp, Log, TEXT("MutateTree: Tree is empty. Forcing Branch Insertion."));
    }
    else
    {
        // Standard evolution: Randomly pick a strategy
        Strategy = FMath::RandRange(0, 2);
        
        // Fallback: If we picked Swap (0) but somehow have no tasks (redundant check but safe), switch to Insert (1)
        if (Strategy == 0 && Tasks.Num() == 0) Strategy = 1;
    }

    // Pick a random task class for the strategies that need one
    UClass* ChosenTaskClass = TaskClasses[FMath::RandRange(0, TaskClasses.Num() - 1)];

    // --- EXECUTE STRATEGY ---

    if (Strategy == 0) // SWAP
    {
        UBTTaskNode* Victim = Tasks[FMath::RandRange(0, Tasks.Num() - 1)];
        UBTCompositeNode* Parent = Victim->GetParentNode();

        if (Parent)
        {
            UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTreeAsset, ChosenTaskClass, NAME_None, RF_Public | RF_Standalone);
            NewTask->InitializeFromAsset(*BehaviorTreeAsset);

            // Find and swap
            for (int32 i = 0; i < Parent->Children.Num(); i++)
            {
                if (Parent->Children[i].ChildTask == Victim)
                {
                    Parent->Children[i].ChildTask = NewTask;
                    UE_LOG(LogTemp, Log, TEXT("Mutate: SWAPPED %s -> %s"), *Victim->GetName(), *NewTask->GetName());
                    break;
                }
            }
        }
    }
    else if (Strategy == 1) // INSERT TASK (Flat)
    {
        UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];

        UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTreeAsset, ChosenTaskClass, NAME_None, RF_Public | RF_Standalone);
        NewTask->InitializeFromAsset(*BehaviorTreeAsset);

        FBTCompositeChild NewChild;
        NewChild.ChildTask = NewTask;
        Parent->Children.Add(NewChild);

        UE_LOG(LogTemp, Log, TEXT("Mutate: INSERTED Task %s into %s"), *NewTask->GetName(), *Parent->GetName());
    }
    else if (Strategy == 2) // INSERT NEW BRANCH (Deep)
    {
        UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];

        // 1. Create New Composite
        UBTCompositeNode* NewComposite = nullptr;
        
        // Optional: If you want to strictly enforce "Add Selector" behavior, you can bias this.
        bool bIsSequence = FMath::RandBool(); 
        if (bIsSequence)
            NewComposite = NewObject<UBTComposite_Sequence>(BehaviorTreeAsset, NAME_None, RF_Public | RF_Standalone);
        else
            NewComposite = NewObject<UBTComposite_Selector>(BehaviorTreeAsset, NAME_None, RF_Public | RF_Standalone);
        
        NewComposite->InitializeFromAsset(*BehaviorTreeAsset);

        // 2. Create Inner Task (So the new branch isn't empty)
        UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTreeAsset, ChosenTaskClass, NAME_None, RF_Public | RF_Standalone);
        NewTask->InitializeFromAsset(*BehaviorTreeAsset);

        // 3. Link: Task -> NewComposite
        FBTCompositeChild TaskChild;
        TaskChild.ChildTask = NewTask;
        NewComposite->Children.Add(TaskChild);

        // 4. Link: NewComposite -> Parent
        FBTCompositeChild CompositeChild;
        CompositeChild.ChildComposite = NewComposite;
        Parent->Children.Add(CompositeChild);

        UE_LOG(LogTemp, Log, TEXT("Mutate: BRANCHED %s under %s (containing %s)"), 
            *NewComposite->GetName(), *Parent->GetName(), *NewTask->GetName());
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


void UCustomBehaviourTree::AppendTreeToLogFile(const FString& FileName, int32 Generation, float Fitness)
{
    if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode) return;

    // 1. Setup the String Builder
    TStringBuilder<4096> LogBuilder;
    
    LogBuilder.Appendf(TEXT("\n=================================================\n"));
    LogBuilder.Appendf(TEXT(" GENERATION: %d | FITNESS: %.2f | ASSET: %s\n"), Generation, Fitness, *BehaviorTreeAsset->GetName());
    LogBuilder.Appendf(TEXT("=================================================\n"));

    // 2. Define Recursive Lambda
    // We use 'auto&& self' to allow the lambda to call itself recursively
    auto WriteNodeRecursive = [&](auto&& self, UBTNode* Node, FString Prefix, bool bIsLast) -> void
    {
        if (!Node) return;

        // A. Print Current Node
        FString Connector = bIsLast ? TEXT("L-- ") : TEXT("|-- ");
        LogBuilder.Appendf(TEXT("%s%s%s\n"), *Prefix, *Connector, *GetCleanNodeName(Node));

        // B. Prepare Prefix for Children
        FString ChildPrefix = Prefix + (bIsLast ? TEXT("    ") : TEXT("|   "));

        // C. Handle Composite Children (Services, Decorators, Sub-nodes)
        if (UBTCompositeNode* Composite = Cast<UBTCompositeNode>(Node))
        {
            // Services
            for (UBTService* Service : Composite->Services)
            {
                if (Service)
                {
                    LogBuilder.Appendf(TEXT("%s . [S] %s\n"), *ChildPrefix, *GetCleanNodeName(Service));
                }
            }

            // Children
            for (int32 i = 0; i < Composite->Children.Num(); i++)
            {
                FBTCompositeChild& ChildLink = Composite->Children[i];
                bool bIsLastChild = (i == Composite->Children.Num() - 1);

                // Decorators (Logic Gates)
                for (UBTDecorator* Deco : ChildLink.Decorators)
                {
                    if (Deco)
                    {
                        LogBuilder.Appendf(TEXT("%s : {D} %s\n"), *ChildPrefix, *GetCleanNodeName(Deco));
                    }
                }

                // Recurse down
                UBTNode* ChildNode = ChildLink.ChildComposite ? (UBTNode*)ChildLink.ChildComposite : (UBTNode*)ChildLink.ChildTask;
                self(self, ChildNode, ChildPrefix, bIsLastChild);
            }
        }
    };

    // 3. Execute Lambda starting at Root
    WriteNodeRecursive(WriteNodeRecursive, BehaviorTreeAsset->RootNode, TEXT(""), true);

    LogBuilder.Append(TEXT("\n"));

    // 4. Write to File
    FString FullPath = FPaths::ProjectLogDir() / FileName;
    
    // Create the file if it doesn't exist, otherwise append
    FFileHelper::SaveStringToFile(
        LogBuilder.ToString(),
        *FullPath,
        FFileHelper::EEncodingOptions::AutoDetect,
        &IFileManager::Get(),
        EFileWrite::FILEWRITE_Append
    );
    
    UE_LOG(LogTemp, Log, TEXT("Logged Tree to %s"), *FullPath);
}

