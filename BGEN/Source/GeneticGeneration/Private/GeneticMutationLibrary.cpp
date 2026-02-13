#include "GeneticMutationLibrary.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"

FString UGeneticMutationLibrary::MutateTree(UCustomBehaviourTree* Wrapper, float MutationRate)
{
    if (!Wrapper || !Wrapper->GetBTAsset()) return TEXT("Error: Invalid Wrapper");

    // 1. Analyze Structure
    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    // 2. FORCE GROWTH (Bootstrap)
    // If tree is too small (< 2 tasks), we MUST grow it, regardless of mutation rate.
    // This prevents "Empty Tree" loops.
    if (Tasks.Num() < 2)
    {
        // If broken (no composite), add one
        if (Composites.Num() == 0) return AddNewComposite(Wrapper);
        
        // Otherwise add a task (Standard Random Task)
        return AddNewTask(Wrapper);
    }

    // 3. Standard Chance Check
    if (FMath::FRand() > MutationRate) return TEXT("No Mutation");

    // 4. Select Strategy
    float StrategyRoll = FMath::FRand();

    if (StrategyRoll < 0.3f) return AddNewTask(Wrapper);       // 30% Grow
    if (StrategyRoll < 0.5f) return AddNewComposite(Wrapper);  // 20% Branch
    if (StrategyRoll < 0.8f) return SwapTask(Wrapper);         // 30% Mutate Behavior
    
    return ShuffleChildren(Wrapper);                           // 20% Reorder
}

FString UGeneticMutationLibrary::AddNewTask(UCustomBehaviourTree* Wrapper)
{
    // 1. Get ALL available tasks (Move, Attack, Wait, etc.)
    // Ensure your Task Blueprints are actually inside this folder path!
    TArray<UClass*> TaskClasses = Wrapper->GetAvailableTaskClasses("/Game/BehaviourTrees");
    
    if (TaskClasses.Num() == 0) return TEXT("Failed: No Task Classes Found (Check Search Path!)");

    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    if (Composites.Num() == 0) return TEXT("Failed: No Composites");

    // 2. Randomly select a Parent Composite
    UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];

    // 3. Randomly select ANY task class (Equal probability)
    // This allows MoveTo, Wait, FindTarget to appear naturally
    UClass* NewClass = TaskClasses[FMath::RandRange(0, TaskClasses.Num() - 1)];

    // 4. Create and Add
    UBTTaskNode* NewTask = NewObject<UBTTaskNode>(Wrapper->GetBTAsset(), NewClass, NAME_None, RF_Public | RF_Transactional);
    NewTask->InitializeFromAsset(*Wrapper->GetBTAsset());

    FBTCompositeChild NewChild;
    NewChild.ChildTask = NewTask;
    Parent->Children.Add(NewChild);

    return FString::Printf(TEXT("Added Task [%s] to [%s]"), *Wrapper->GetCleanNodeName(NewTask), *Wrapper->GetCleanNodeName(Parent));
}

FString UGeneticMutationLibrary::AddNewComposite(UCustomBehaviourTree* Wrapper)
{
    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    if (Composites.Num() == 0) return TEXT("Failed: No Composites");

    UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];
    
    bool bIsSelector = FMath::RandBool();
    UClass* CompClass = bIsSelector ? UBTComposite_Selector::StaticClass() : UBTComposite_Sequence::StaticClass();
    
    UBTCompositeNode* NewComp = NewObject<UBTCompositeNode>(Wrapper->GetBTAsset(), CompClass, NAME_None, RF_Public | RF_Transactional);
    NewComp->InitializeFromAsset(*Wrapper->GetBTAsset());

    FBTCompositeChild NewChild;
    NewChild.ChildComposite = NewComp;
    Parent->Children.Add(NewChild);

    // Bootstrap the new branch with a task so it isn't empty
    AddNewTask(Wrapper); 

    return FString::Printf(TEXT("Added Branch [%s] to [%s]"), *Wrapper->GetCleanNodeName(NewComp), *Wrapper->GetCleanNodeName(Parent));
}

FString UGeneticMutationLibrary::SwapTask(UCustomBehaviourTree* Wrapper)
{
    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    if (Tasks.Num() == 0) return TEXT("Failed: No Tasks");

    UBTTaskNode* Victim = Tasks[FMath::RandRange(0, Tasks.Num() - 1)];
    UBTCompositeNode* Parent = Victim->GetParentNode();
    if (!Parent) return TEXT("Failed: Orphan Task");

    // Pure Random Swap
    TArray<UClass*> TaskClasses = Wrapper->GetAvailableTaskClasses("/Game/BehaviourTrees");
    if (TaskClasses.Num() == 0) return TEXT("Failed: No Classes");

    UClass* NewClass = TaskClasses[FMath::RandRange(0, TaskClasses.Num() - 1)];
    
    UBTTaskNode* NewTask = NewObject<UBTTaskNode>(Wrapper->GetBTAsset(), NewClass, NAME_None, RF_Public | RF_Transactional);
    NewTask->InitializeFromAsset(*Wrapper->GetBTAsset());

    for (int32 i = 0; i < Parent->Children.Num(); i++)
    {
        if (Parent->Children[i].ChildTask == Victim)
        {
            Parent->Children[i].ChildTask = NewTask;
            return FString::Printf(TEXT("Swapped [%s] with [%s]"), *Wrapper->GetCleanNodeName(Victim), *Wrapper->GetCleanNodeName(NewTask));
        }
    }
    return TEXT("Failed: Swap Logic Error");
}

FString UGeneticMutationLibrary::ShuffleChildren(UCustomBehaviourTree* Wrapper)
{
    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    if (Composites.Num() == 0) return TEXT("Failed: No Composites");
    
    UBTCompositeNode* Target = Composites[FMath::RandRange(0, Composites.Num() - 1)];
    if(Target->Children.Num() < 2) return TEXT("Skipped Shuffle (Single Child)");

    for (int32 i = 0; i < Target->Children.Num(); ++i)
    {
        int32 SwapIdx = FMath::RandRange(0, Target->Children.Num() - 1);
        Target->Children.Swap(i, SwapIdx);
    }
    return FString::Printf(TEXT("Shuffled children of [%s]"), *Wrapper->GetCleanNodeName(Target));
}