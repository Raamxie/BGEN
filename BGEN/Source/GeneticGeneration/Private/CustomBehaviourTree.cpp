#include "CustomBehaviourTree.h"
#include "Misc/PackageName.h"
#include "Misc/SecureHash.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Decorators/BTDecorator_BlueprintBase.h"

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

void UCustomBehaviourTree::InitFromTreeInstance(UBehaviorTree* SourceTree)
{
	if (!SourceTree) return;

	// 1. Create a fresh Asset container
	BehaviorTreeAsset = NewObject<UBehaviorTree>(GetOuter(), NAME_None, RF_Public | RF_Transactional);
    
	// Copy basic settings (Blackboard reference, etc.)
	BehaviorTreeAsset->BlackboardAsset = SourceTree->BlackboardAsset;

	// 2. DEEP COPY THE HIERARCHY
	if (SourceTree->RootNode)
	{
		BehaviorTreeAsset->RootNode = Cast<UBTCompositeNode>(DuplicateNodeRecursive(SourceTree->RootNode, BehaviorTreeAsset));
	}
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

UCustomBehaviourTree* UCustomBehaviourTree::PerformCrossover(UCustomBehaviourTree* DonorTreeWrapper, FLogCategoryBase& Category)
{
    if (!DonorTreeWrapper || !BehaviorTreeAsset) return nullptr;

    // 1. Create New Wrapper (Standard)
    UCustomBehaviourTree* ChildWrapper = NewObject<UCustomBehaviourTree>(GetOuter());
    
    // 2. Init as Deep Copy of Self (Parent A)
    ChildWrapper->InitFromTreeInstance(this->BehaviorTreeAsset);

    // 3. Find Slots
    TArray<FNodeHandle> ChildSlots;
    ChildWrapper->GetAllCompositeSlots(ChildWrapper->GetBTAsset()->RootNode, ChildSlots);

    TArray<UBTNode*> DonorSubtrees;
    ChildWrapper->GetAllSubtrees(DonorTreeWrapper->GetBTAsset()->RootNode, DonorSubtrees);

    // 4. Perform Swap
    if (ChildSlots.Num() > 0 && DonorSubtrees.Num() > 0)
    {
        FNodeHandle TargetSlot = ChildSlots[FMath::RandRange(0, ChildSlots.Num() - 1)];
        UBTNode* DonorNode = DonorSubtrees[FMath::RandRange(0, DonorSubtrees.Num() - 1)];

        // *** FIX: USE RECURSIVE DUPLICATION ***
        UBTNode* ClonedGene = DuplicateNodeRecursive(DonorNode, ChildWrapper->GetBTAsset());
        
        // Link it up
        if (TargetSlot.Parent->Children.IsValidIndex(TargetSlot.ChildIndex))
        {
            FBTCompositeChild& Link = TargetSlot.Parent->Children[TargetSlot.ChildIndex];
            
            FString OldNodeName = Link.ChildTask ? GetCleanNodeName(Link.ChildTask) : GetCleanNodeName(Link.ChildComposite);
            
            if (ClonedGene->IsA(UBTTaskNode::StaticClass()))
            {
                Link.ChildTask = Cast<UBTTaskNode>(ClonedGene);
                Link.ChildComposite = nullptr;
            }
            else if (ClonedGene->IsA(UBTCompositeNode::StaticClass()))
            {
                Link.ChildComposite = Cast<UBTCompositeNode>(ClonedGene);
                Link.ChildTask = nullptr;
            }

            // Directly log to the provided category
            FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Log, 
                TEXT("Replaced Node [%s] in Parent A with Donor Subtree [%s] from Parent B at Parent Node [%s]"), 
                *OldNodeName, *GetCleanNodeName(ClonedGene), *GetCleanNodeName(TargetSlot.Parent));
                
            return ChildWrapper;
        }
    }
    
    FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT("Crossover Failed (No valid slots)"));
    return ChildWrapper;
}

TArray<UClass*> UCustomBehaviourTree::GetAvailableDecoratorClasses(const FString& Path)
{
	TArray<UClass*> ResultClasses;

	// 1. Get the Asset Registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// 2. Define Filter
	FARFilter Filter;
	Filter.PackagePaths.Add(*Path);
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());

	// 3. Search
	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	for (const FAssetData& Asset : AssetList)
	{
		UObject* LoadedAsset = Asset.GetAsset();
		if (UBlueprint* BP = Cast<UBlueprint>(LoadedAsset))
		{
			// We verify if the Generated Class inherits from BTDecorator_BlueprintBase
			if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(UBTDecorator_BlueprintBase::StaticClass()))
			{
				ResultClasses.Add(BP->GeneratedClass);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d valid Decorator classes in %s"), ResultClasses.Num(), *Path);
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

// DEBUG VISUALIZATION

void UCustomBehaviourTree::DebugLogTree(FLogCategoryBase& Category)
{
	if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode)
	{
		FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Error, TEXT("DebugLogTree: No Tree/Root found."));
		return;
	}

	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT("================================================="));
	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT(" SERVER VIEW: TREE STRUCTURE: %s"), *BehaviorTreeAsset->GetName());
	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT(" Legend: [S]=Service, {D}=Decorator, (Node)"));
	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT("================================================="));

	// Start recursion
	PrintPrettyNode(BehaviorTreeAsset->RootNode, "", true, Category);

	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Warning, TEXT("================================================="));
}

void UCustomBehaviourTree::PrintPrettyNode(UBTNode* Node, FString Prefix, bool bIsLast, FLogCategoryBase& Category)
{
	if (!Node) return;

	FString Connector = bIsLast ? TEXT("L-- ") : TEXT("|-- ");
	FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Display, TEXT("%s%s%s"), *Prefix, *Connector, *GetCleanNodeName(Node));

	FString ChildPrefix = Prefix + (bIsLast ? TEXT("    ") : TEXT("|   "));

	UBTCompositeNode* Composite = Cast<UBTCompositeNode>(Node);
	if (Composite)
	{
		// Print Services
		for (UBTService* Service : Composite->Services)
		{
			if (Service)
			{
				FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Log, TEXT("%s . [S] %s"), *ChildPrefix, *GetCleanNodeName(Service));
			}
		}

		// Print Children and their Decorators
		for (int32 i = 0; i < Composite->Children.Num(); i++)
		{
			FBTCompositeChild& ChildLink = Composite->Children[i];
			bool bIsLastChild = (i == Composite->Children.Num() - 1);

			for (UBTDecorator* Deco : ChildLink.Decorators)
			{
				if (Deco)
				{
					FMsg::Logf(nullptr, 0, Category.GetCategoryName(), ELogVerbosity::Log, TEXT("%s : {D} %s"), *ChildPrefix, *GetCleanNodeName(Deco));
				}
			}

			UBTNode* ChildNode = ChildLink.ChildComposite ? (UBTNode*)ChildLink.ChildComposite : (UBTNode*)ChildLink.ChildTask;
			PrintPrettyNode(ChildNode, ChildPrefix, bIsLastChild, Category);
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

void UCustomBehaviourTree::GetAllCompositeSlots(UBTNode* Node, TArray<FNodeHandle>& OutSlots)
{
    if (UBTCompositeNode* Comp = Cast<UBTCompositeNode>(Node))
    {
        for (int32 i = 0; i < Comp->Children.Num(); i++)
        {
            // Add this specific slot (Parent + Index)
            OutSlots.Add({ Comp, i });

            // Recurse
            UBTNode* Child = Comp->Children[i].ChildComposite ? (UBTNode*)Comp->Children[i].ChildComposite : (UBTNode*)Comp->Children[i].ChildTask;
            GetAllCompositeSlots(Child, OutSlots);
        }
    }
}

void UCustomBehaviourTree::GetAllSubtrees(UBTNode* Node, TArray<UBTNode*>& OutNodes)
{
    if (!Node) return;
    
    // We can steal this node
    OutNodes.Add(Node);

    if (UBTCompositeNode* Comp = Cast<UBTCompositeNode>(Node))
    {
        for (auto& ChildLink : Comp->Children)
        {
            UBTNode* Child = ChildLink.ChildComposite ? (UBTNode*)ChildLink.ChildComposite : (UBTNode*)ChildLink.ChildTask;
            GetAllSubtrees(Child, OutNodes);
        }
    }
}

FString UCustomBehaviourTree::GetTreeAsString()
{
	if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode) return TEXT("INVALID TREE");

	TStringBuilder<4096> SB;
	// Use a lambda to recurse
	auto RecursivePrint = [&](auto&& Self, UBTNode* Node, FString Prefix, bool bIsLast) -> void
	{
		if (!Node) return;
		FString Connector = bIsLast ? TEXT("L-- ") : TEXT("|-- ");
		SB.Appendf(TEXT("%s%s%s\n"), *Prefix, *Connector, *GetCleanNodeName(Node));

		FString ChildPrefix = Prefix + (bIsLast ? TEXT("    ") : TEXT("|   "));

		if (UBTCompositeNode* Comp = Cast<UBTCompositeNode>(Node))
		{
			for (UBTService* Service : Comp->Services)
			{
				if(Service) SB.Appendf(TEXT("%s . [S] %s\n"), *ChildPrefix, *GetCleanNodeName(Service));
			}
			for (int32 i = 0; i < Comp->Children.Num(); i++)
			{
				FBTCompositeChild& Link = Comp->Children[i];
				bool bLastChild = (i == Comp->Children.Num() - 1);
				for (UBTDecorator* Deco : Link.Decorators)
				{
					if(Deco) SB.Appendf(TEXT("%s : {D} %s\n"), *ChildPrefix, *GetCleanNodeName(Deco));
				}
				UBTNode* Child = Link.ChildComposite ? (UBTNode*)Link.ChildComposite : (UBTNode*)Link.ChildTask;
				Self(Self, Child, ChildPrefix, bLastChild);
			}
		}
	};

	RecursivePrint(RecursivePrint, BehaviorTreeAsset->RootNode, TEXT(""), true);
	return SB.ToString();
}


UBTNode* UCustomBehaviourTree::DuplicateNodeRecursive(UBTNode* SourceNode, UBehaviorTree* TargetAsset)
{
    if (!SourceNode || !TargetAsset) return nullptr;

    // 1. Duplicate the Node itself (Shallow copy of properties)
    UBTNode* NewNode = DuplicateObject<UBTNode>(SourceNode, TargetAsset);
    NewNode->InitializeFromAsset(*TargetAsset); // Important: Bind to new tree

    // 2. If it is a Composite, we must Deep Copy its children references
    if (UBTCompositeNode* SourceComp = Cast<UBTCompositeNode>(SourceNode))
    {
        UBTCompositeNode* NewComp = Cast<UBTCompositeNode>(NewNode);
        
        // The DuplicateObject call copied the "Children" array struct, 
        // effectively copying the POINTERS to the OLD nodes. We must overwrite them.
        
        for (int32 i = 0; i < NewComp->Children.Num(); i++)
        {
            FBTCompositeChild& NewLink = NewComp->Children[i];
            
            // Get the OLD child (currently pointed to)
            UBTNode* OldChild = NewLink.ChildComposite ? (UBTNode*)NewLink.ChildComposite : (UBTNode*)NewLink.ChildTask;
            
            if (OldChild)
            {
                // RECURSIVE CALL: Create a fresh copy of the child
                UBTNode* NewChild = DuplicateNodeRecursive(OldChild, TargetAsset);
                
                // REASSIGN the link to the NEW child
                if (UBTCompositeNode* C = Cast<UBTCompositeNode>(NewChild))
                {
                    NewLink.ChildComposite = C;
                    NewLink.ChildTask = nullptr;
                }
                else if (UBTTaskNode* T = Cast<UBTTaskNode>(NewChild))
                {
                    NewLink.ChildTask = T;
                    NewLink.ChildComposite = nullptr;
                }
            }

            // 3. DUPLICATE DECORATORS (Attached to the link)
            TArray<UBTDecorator*> NewDecos;
            for (UBTDecorator* OldDeco : NewLink.Decorators)
            {
                if (OldDeco)
                {
                     UBTDecorator* NewDeco = DuplicateObject<UBTDecorator>(OldDeco, TargetAsset);
                     NewDeco->InitializeFromAsset(*TargetAsset);
                     NewDecos.Add(NewDeco);
                }
            }
            NewLink.Decorators = NewDecos;
        }
        
        // 4. DUPLICATE SERVICES (Attached to the Composite)
        TArray<UBTService*> NewServices;
        for (UBTService* OldService : SourceComp->Services)
        {
             if (OldService)
             {
                 UBTService* NewS = DuplicateObject<UBTService>(OldService, TargetAsset);
                 NewS->InitializeFromAsset(*TargetAsset);
                 NewServices.Add(NewS);
             }
        }
        NewComp->Services = NewServices;
    }
    
    return NewNode;
}


FString UCustomBehaviourTree::GetTreeStructureForHash()
{
	if (!BehaviorTreeAsset || !BehaviorTreeAsset->RootNode) return TEXT("INVALID_TREE");

	TStringBuilder<2048> SB;

	// Lambda for recursive flat traversal
	auto RecursiveBuild = [&](auto&& Self, UBTNode* Node) -> void
	{
		if (!Node) return;
		
		// Append the base node name
		SB.Append(GetCleanNodeName(Node));
		SB.Append(TEXT("|")); // Use a simple delimiter

		if (UBTCompositeNode* Comp = Cast<UBTCompositeNode>(Node))
		{
			// Append Services
			for (UBTService* Service : Comp->Services)
			{
				if (Service) SB.Appendf(TEXT("Srv:%s|"), *GetCleanNodeName(Service));
			}

			// Append Children & Decorators
			for (int32 i = 0; i < Comp->Children.Num(); i++)
			{
				FBTCompositeChild& Link = Comp->Children[i];
				
				for (UBTDecorator* Deco : Link.Decorators)
				{
					if (Deco) SB.Appendf(TEXT("Dec:%s|"), *GetCleanNodeName(Deco));
				}

				UBTNode* Child = Link.ChildComposite ? (UBTNode*)Link.ChildComposite : (UBTNode*)Link.ChildTask;
				Self(Self, Child); // Recurse
			}
		}
	};

	RecursiveBuild(RecursiveBuild, BehaviorTreeAsset->RootNode);
	return SB.ToString();
}

FString UCustomBehaviourTree::GetTreeHash()
{
	// 1. Get the pure structural representation
	FString TreeStructure = GetTreeStructureForHash();
	
	// 2. Hash it
	return FMD5::HashAnsiString(*TreeStructure);
}


TArray<UBehaviorTree*> UCustomBehaviourTree::GetAvailableTaskTrees(const FString& Path)
{
	TArray<UBehaviorTree*> ResultTrees;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;
	Filter.PackagePaths.Add(*Path);
	Filter.bRecursivePaths = true;
	// Look specifically for UBehaviorTree assets instead of Blueprints
	Filter.ClassPaths.Add(UBehaviorTree::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	for (const FAssetData& Asset : AssetList)
	{
		if (UBehaviorTree* BT = Cast<UBehaviorTree>(Asset.GetAsset()))
		{
			ResultTrees.Add(BT);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d valid Subtrees in %s"), ResultTrees.Num(), *Path);
	return ResultTrees;
}