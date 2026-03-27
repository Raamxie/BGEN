#include "GeneticMutationLibrary.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTask_RunBehaviorRuntime.h"
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
	if (Tasks.Num() < 2)
	{
		if (Composites.Num() == 0) return AddNewComposite(Wrapper);
		return AddNewTask(Wrapper);
	}

	// 3. Standard Chance Check
	if (FMath::FRand() > MutationRate) return TEXT("No Mutation");
	
	// 4. Select Strategy (Rebalanced to include Deletion)
	float StrategyRoll = FMath::FRand();

	if (StrategyRoll < 0.25f) return AddNewTask(Wrapper);       // 25% Grow (Task OR Subtree)
	if (StrategyRoll < 0.40f) return AddNewComposite(Wrapper);  // 15% Branch
	if (StrategyRoll < 0.65f) return SwapTask(Wrapper);         // 25% Mutate Behavior (Task OR Subtree)
	if (StrategyRoll < 0.85f) return ShuffleChildren(Wrapper);  // 20% Reorder
    
	return DeleteNode(Wrapper);                                 // 15% Prune / Shrink Tree
}

FString UGeneticMutationLibrary::AddNewTask(UCustomBehaviourTree* Wrapper)
{
    // 1. Fetch both pools using the updated path
    TArray<UClass*> TaskClasses = Wrapper->GetAvailableTaskClasses("/Game/BehaviourTrees/EnemyTasks");
    
    // Note: Assuming subtrees also moved, adjust if they are still at "/Game/BehaviourTrees/TaskTrees"
    TArray<UBehaviorTree*> Subtrees = Wrapper->GetAvailableTaskTrees("/Game/BehaviourTrees/EnemyTasks/TaskTrees");
    
    int32 TotalChoices = TaskClasses.Num() + Subtrees.Num();
    if (TotalChoices == 0) return TEXT("Failed: No Tasks or Subtrees Found");

    TArray<UBTCompositeNode*> Composites;
    TArray<UBTTaskNode*> Tasks;
    Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

    if (Composites.Num() == 0) return TEXT("Failed: No Composites");

    // 2. Select Parent
    UBTCompositeNode* Parent = Composites[FMath::RandRange(0, Composites.Num() - 1)];

    // 3. Roll against the combined pool
    int32 Choice = FMath::RandRange(0, TotalChoices - 1);
    
    UBTTaskNode* NewTask = nullptr;
    FString TaskName;

    // 4. Instantiate the correct type
    if (Choice < TaskClasses.Num())
    {
        // It's a standard Task Blueprint
        UClass* NewClass = TaskClasses[Choice];
        NewTask = NewObject<UBTTaskNode>(Wrapper->GetBTAsset(), NewClass, NAME_None, RF_Public | RF_Transactional);
        TaskName = Wrapper->GetCleanNodeName(NewTask);
    }
    else
    {
        // It's a Subtree (Shift index down by TaskClasses.Num())
        UBehaviorTree* SelectedSubtree = Subtrees[Choice - TaskClasses.Num()];
        UBTTask_RunBehaviorRuntime* SubtreeTask = NewObject<UBTTask_RunBehaviorRuntime>(Wrapper->GetBTAsset(), UBTTask_RunBehaviorRuntime::StaticClass(), NAME_None, RF_Public | RF_Transactional);
        SubtreeTask->SetBehaviorAsset(SelectedSubtree);
        
        NewTask = SubtreeTask;
        TaskName = SelectedSubtree->GetName();
    }

    // 5. Initialize and Link
    NewTask->InitializeFromAsset(*Wrapper->GetBTAsset());

    FBTCompositeChild NewChild;
    NewChild.ChildTask = NewTask;
    Parent->Children.Add(NewChild);

    return FString::Printf(TEXT("Added Task/Subtree [%s] to [%s]"), *TaskName, *Wrapper->GetCleanNodeName(Parent));
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

    // 1. Fetch both pools using the updated path
    TArray<UClass*> TaskClasses = Wrapper->GetAvailableTaskClasses("/Game/BehaviourTrees/EnemyTasks");
    
    // Note: Assuming subtrees also moved, adjust if they are still at "/Game/BehaviourTrees/TaskTrees"
    TArray<UBehaviorTree*> Subtrees = Wrapper->GetAvailableTaskTrees("/Game/BehaviourTrees/EnemyTasks/TaskTrees");
    
    int32 TotalChoices = TaskClasses.Num() + Subtrees.Num();
    if (TotalChoices == 0) return TEXT("Failed: No Classes or Subtrees");

    // 2. Roll against the combined pool
    int32 Choice = FMath::RandRange(0, TotalChoices - 1);
    
    UBTTaskNode* NewTask = nullptr;
    FString TaskName;

    // 3. Instantiate the replacement
    if (Choice < TaskClasses.Num())
    {
        UClass* NewClass = TaskClasses[Choice];
        NewTask = NewObject<UBTTaskNode>(Wrapper->GetBTAsset(), NewClass, NAME_None, RF_Public | RF_Transactional);
        TaskName = Wrapper->GetCleanNodeName(NewTask);
    }
    else
    {
        UBehaviorTree* SelectedSubtree = Subtrees[Choice - TaskClasses.Num()];
        UBTTask_RunBehaviorRuntime* SubtreeTask = NewObject<UBTTask_RunBehaviorRuntime>(Wrapper->GetBTAsset(), UBTTask_RunBehaviorRuntime::StaticClass(), NAME_None, RF_Public | RF_Transactional);
        SubtreeTask->SetBehaviorAsset(SelectedSubtree);
        
        NewTask = SubtreeTask;
        TaskName = SelectedSubtree->GetName();
    }

    NewTask->InitializeFromAsset(*Wrapper->GetBTAsset());

    // 4. Perform the Swap
    for (int32 i = 0; i < Parent->Children.Num(); i++)
    {
        if (Parent->Children[i].ChildTask == Victim)
        {
            Parent->Children[i].ChildTask = NewTask;
            return FString::Printf(TEXT("Swapped [%s] with [%s]"), *Wrapper->GetCleanNodeName(Victim), *TaskName);
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

FString UGeneticMutationLibrary::DeleteNode(UCustomBehaviourTree* Wrapper)
{
	TArray<UBTCompositeNode*> Composites;
	TArray<UBTTaskNode*> Tasks;
	Wrapper->CollectNodes(Wrapper->GetBTAsset()->RootNode, Composites, Tasks);

	// FIX: Never delete the last remaining task
	if (Tasks.Num() <= 1) return TEXT("Skipped Delete: Tree too small");

	UBTTaskNode* Victim = Tasks[FMath::RandRange(0, Tasks.Num() - 1)];
	UBTCompositeNode* Parent = Victim->GetParentNode();

	if (!Parent) return TEXT("Failed: Orphan Task");

	// FIX: Never leave a Sequence or Selector completely empty
	if (Parent->Children.Num() <= 1) 
	{
		return TEXT("Skipped Delete: Parent would be left empty");
	}

	FString TaskName = Wrapper->GetCleanNodeName(Victim);

	for (int32 i = 0; i < Parent->Children.Num(); i++)
	{
		if (Parent->Children[i].ChildTask == Victim)
		{
			Parent->Children.RemoveAt(i);
			return FString::Printf(TEXT("Deleted Node [%s] from [%s]"), *TaskName, *Wrapper->GetCleanNodeName(Parent));
		}
	}

	return TEXT("Failed: Could not find node in parent's children array");
}