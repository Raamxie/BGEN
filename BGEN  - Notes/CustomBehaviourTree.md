```cpp
#include "CustomBehaviourTree.h"  
#include "AIController.h"  
#include "GeneticGenerationModule.h"  
#include "Engine/AssetManager.h"  
#include "Engine/StreamableManager.h"  
#include "BehaviorTree/BlackboardData.h"  
#include "Misc/PackageName.h"  
#include "UObject/SavePackage.h"  
#include "BehaviorTree/BTTaskNode.h"  
#include "BehaviorTree/BTDecorator.h"  
#include "BehaviorTree/BlackboardComponent.h"  
#include "BehaviorTree/Composites/BTComposite_Selector.h"  
  
UCustomBehaviourTree::UCustomBehaviourTree()  
{  
    Epoch = 0;  
    ID = 0;  
    Tag = TEXT("None");  
    BehaviorTree = nullptr;  
    ActiveComponent = nullptr;  
}  
  
void UCustomBehaviourTree::LoadTree(const FString& AssetLocation)  
{  
    if (AssetLocation.IsEmpty())  
    {        UE_LOG(LogGeneticGeneration, Error, TEXT("LoadTree failed — AssetLocation is empty."));  
        return;  
    }  
    BehaviorTreePath = FSoftObjectPath(AssetLocation);  
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();  
  
    UE_LOG(LogGeneticGeneration, Log, TEXT("Requesting async load for BehaviorTree: %s"), *AssetLocation);  
    StreamableHandle = Streamable.RequestAsyncLoad(  
        BehaviorTreePath,  
        FStreamableDelegate::CreateUObject(this, &UCustomBehaviourTree::OnBehaviorTreeLoaded)  
    );}  
  
void UCustomBehaviourTree::OnBehaviorTreeLoaded()  
{  
    UE_LOG(LogGeneticGeneration, Log, TEXT("Async load completed for CustomBehaviourTree"));  
  
    UObject* LoadedObj = nullptr;  
    if (BehaviorTreePath.IsValid())  
    {        LoadedObj = BehaviorTreePath.ResolveObject();  
    }  
    if (!LoadedObj && StreamableHandle.IsValid())  
    {        LoadedObj = StreamableHandle->GetLoadedAsset();  
    }  
    BehaviorTree = Cast<UBehaviorTree>(LoadedObj);  
  
    if (BehaviorTree)  
    {        UE_LOG(LogGeneticGeneration, Log, TEXT("Successfully loaded BT asset: %s"), *BehaviorTree->GetName());  
  
        // ✅ BehaviorTree is ready — you can mutate or run it now        DuplicateTree();  
    }    else  
    {  
        UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to cast loaded object to UBehaviorTree"));  
    }}  
  
void UCustomBehaviourTree::SaveTree()  
{  
    if (!BehaviorTree)  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("No BehaviorTree to save."));  
        return;  
    }  
    UPackage* Package = BehaviorTree->GetPackage();  
    if (!Package)  
    {        UE_LOG(LogGeneticGeneration, Error, TEXT("BehaviorTree has no valid package."));  
        return;  
    }  
    BehaviorTree->MarkPackageDirty();  
  
    const FString PackageFilename = FPackageName::LongPackageNameToFilename(  
        PackageFilename, FPackageName::GetAssetPackageExtension());  
  
    FSavePackageArgs SaveArgs;  
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;  
    SaveArgs.Error = GWarn;  
    SaveArgs.SaveFlags = SAVE_None;  
  
    if (UPackage::SavePackage(Package, BehaviorTree, *PackageFilename, SaveArgs))  
    {        UE_LOG(LogGeneticGeneration, Log, TEXT("Behavior Tree saved successfully: %s"), *PackageFilename);  
  
        if (LoadObject<UBehaviorTree>(nullptr, *Package->GetName()))  
        {            UE_LOG(LogGeneticGeneration, Log, TEXT("Verification successful: %s"), *Package->GetName());  
        }        else  
        {  
            UE_LOG(LogGeneticGeneration, Error, TEXT("Verification failed: %s"), *Package->GetName());  
        }    }    else  
    {  
        UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to save BehaviorTree: %s"), *PackageFilename);  
    }}  
  
void UCustomBehaviourTree::MutateCurrentTree()  
{  
    if (!BehaviorTree || !BehaviorTree->RootNode)  
    {        UE_LOG(LogGeneticGeneration, Error, TEXT("Cannot mutate: BehaviorTree or RootNode invalid."));  
        return;  
    }  
    if (TaskClasses.Num() == 0)  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("No task classes provided for mutation."));  
        return;  
    }  
    UBTCompositeNode* Root = BehaviorTree->RootNode;  
    int MutationType = FMath::RandRange(0, 2);  
  
    switch (MutationType)  
    {    case 0: // Replace existing child  
        if (Root->Children.Num() > 0)  
        {            int Index = FMath::RandRange(0, Root->Children.Num() - 1);  
            FBTCompositeChild& Child = Root->Children[Index];  
            int TaskIndex = FMath::RandRange(0, TaskClasses.Num() - 1);  
            UClass* TaskClass = TaskClasses[TaskIndex];  
  
            if (TaskClass)  
            {                UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTree, TaskClass, *FString::Printf(TEXT("MutatedTask_%d"), Index));  
                Child.ChildTask = NewTask;  
                UE_LOG(LogGeneticGeneration, Log, TEXT("Replaced child %d with task %s"), Index, *TaskClass->GetName());  
            }        }        break;  
  
    case 1: // Add new child  
        {  
            int TaskIndex = FMath::RandRange(0, TaskClasses.Num() - 1);  
            UClass* TaskClass = TaskClasses[TaskIndex];  
            UBTTaskNode* NewTask = NewObject<UBTTaskNode>(BehaviorTree, TaskClass, *FString::Printf(TEXT("NewTask_%d"), Root->Children.Num()));  
  
            FBTCompositeChild NewChild;  
            NewChild.ChildTask = NewTask;  
  
            if (DecoratorClasses.Num() > 0 && FMath::RandBool())  
            {                int DecIndex = FMath::RandRange(0, DecoratorClasses.Num() - 1);  
                UClass* DecClass = DecoratorClasses[DecIndex];  
                if (DecClass)  
                {                    UBTDecorator* NewDec = NewObject<UBTDecorator>(BehaviorTree, DecClass, *FString::Printf(TEXT("MutatedDecorator_%d"), DecIndex));  
                    NewChild.Decorators.Add(NewDec);  
                    UE_LOG(LogGeneticGeneration, Log, TEXT("Added decorator %s"), *DecClass->GetName());  
                }            }  
            Root->Children.Add(NewChild);  
            UE_LOG(LogGeneticGeneration, Log, TEXT("Added new task %s as child"), *TaskClass->GetName());  
        }        break;  
  
    case 2: // Remove random child  
        if (Root->Children.Num() > 1)  
        {            int RemoveIndex = FMath::RandRange(0, Root->Children.Num() - 1);  
            Root->Children.RemoveAt(RemoveIndex);  
            UE_LOG(LogGeneticGeneration, Log, TEXT("Removed child %d"), RemoveIndex);  
        }        break;  
  
    default:  
        break;  
    }  
    BehaviorTree->MarkPackageDirty();  
}  
  
  
  
/** Start the behavior tree on a provided AIController */  
void UCustomBehaviourTree::StartTree(AAIController* Controller)  
{  
    if (!Controller)  
    {        UE_LOG(LogGeneticGeneration, Error, TEXT("StartTree failed — Controller is null."));  
        return;  
    }  
    if (!BehaviorTree)  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("No BehaviorTree loaded yet for CustomBehaviourTree %d"), ID);  
        return;  
    }  
    // Create a runtime blackboard if not already  
    CreateRuntimeBlackboard();  
  
    // Initialize controller blackboard  
    if (BlackboardData)  
    {        BlackboardComponent = NewObject<UBlackboardComponent>(Controller, UBlackboardComponent::StaticClass(), TEXT("RuntimeBlackboardComp"));  
        BlackboardComponent->InitializeBlackboard(*BlackboardData);  
        Controller->BrainComponent = ActiveComponent; // your existing BTComponent  
        Controller->UseBlackboard(BlackboardData, BlackboardComponent);  
    }  
    bool bStarted = Controller->RunBehaviorTree(BehaviorTree);  
  
    if (bStarted)  
    {        UE_LOG(LogGeneticGeneration, Log, TEXT("BehaviorTree started on controller %s"), *Controller->GetName());  
        ActiveComponent = Cast<UBehaviorTreeComponent>(Controller->BrainComponent);  
    }    else  
    {  
        UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to start BehaviorTree on controller %s"), *Controller->GetName());  
    }}  
  
/** Stop the currently running BehaviorTree */  
void UCustomBehaviourTree::StopTree()  
{  
    if (!ActiveComponent)  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("No active BT component to stop for CustomBehaviourTree %d"), ID);  
        return;  
    }  
    if (ActiveComponent->IsRunning())  
    {        ActiveComponent->StopTree(EBTStopMode::Safe);  
        UE_LOG(LogGeneticGeneration, Log, TEXT("BehaviorTree stopped for CustomBehaviourTree %d"), ID);  
    }    else  
    {  
        UE_LOG(LogGeneticGeneration, Warning, TEXT("BehaviorTree already stopped for CustomBehaviourTree %d"), ID);  
    }  
    ActiveComponent = nullptr;  
}  
  
void UCustomBehaviourTree::LoadBlackboard(const FString& AssetPath)  
{  
    if (AssetPath.IsEmpty())  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("Blackboard path empty"));  
        return;  
    }  
    UBlackboardData* LoadedBB = LoadObject<UBlackboardData>(nullptr, *AssetPath);  
    if (LoadedBB)  
    {        BlackboardData = LoadedBB;  
        UE_LOG(LogGeneticGeneration, Log, TEXT("Loaded existing blackboard: %s"), *AssetPath);  
    }    else  
    {  
        UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to load blackboard: %s"), *AssetPath);  
    }}  
  
  
void UCustomBehaviourTree::CreateRuntimeBlackboard()  
{  
    if (BlackboardData) return;  
  
    BlackboardData = NewObject<UBlackboardData>(this, UBlackboardData::StaticClass(), TEXT("RuntimeBlackboard"));  
    BlackboardData->AddToRoot();  
    // AddBlackboardKey(FName("MyTarget"), UBlackboardKeyType_Object::StaticClass());  
  
    UE_LOG(LogGeneticGeneration, Log, TEXT("Runtime blackboard created with %d keys"), BlackboardData->Keys.Num());  
}  
  
void UCustomBehaviourTree::AddBlackboardKey(FName KeyName, UClass* KeyTypeClass)  
{  
    if (!BlackboardData || !KeyTypeClass) return;  
  
    FBlackboardEntry Entry;  
    Entry.EntryName = KeyName;  
    Entry.KeyType = NewObject<UBlackboardKeyType>(BlackboardData, KeyTypeClass);  
    BlackboardData->Keys.Add(Entry);  
  
    UE_LOG(LogGeneticGeneration, Log, TEXT("Added blackboard key %s"), *KeyName.ToString());  
}
```