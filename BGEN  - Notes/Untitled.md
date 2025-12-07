```cpp
void AGeneticGenerationModule::SpawnEnemies(int32 Count)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    // ---------------------------------------------------
    // 1. HARD-LOAD BLUEPRINT CLASS
    // ---------------------------------------------------
    FString EnemyPath = TEXT("Blueprint'/Game/AI/Enemies/BP_EnemyCharacter.BP_EnemyCharacter_C'");
    UClass* EnemyClass = LoadObject<UClass>(nullptr, *EnemyPath);

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Enemy Blueprint class: %s"), *EnemyPath);
        return;
    }

    // ---------------------------------------------------
    // 2. HARD-LOAD BEHAVIOR TREE ASSET
    // ---------------------------------------------------
    FString BTPath = TEXT("BehaviorTree'/Game/AI/BT/BT_Enemy.BT_Enemy'");
    UBehaviorTree* BTAsset = LoadObject<UBehaviorTree>(nullptr, *BTPath);

    if (!BTAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Behavior Tree: %s"), *BTPath);
        return;
    }

    // ---------------------------------------------------
    // 3. HARD-LOAD BLACKBOARD ASSET
    // ---------------------------------------------------
    FString BlackboardPath = TEXT("BlackboardData'/Game/AI/BB/BB_Enemy.BB_Enemy'");
    UBlackboardData* BlackboardAsset = LoadObject<UBlackboardData>(nullptr, *BlackboardPath);

    if (!BlackboardAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Blackboard: %s"), *BlackboardPath);
        return;
    }

    // ---------------------------------------------------
    // 4. Spawn Enemies
    // ---------------------------------------------------
    for (int32 i = 0; i < Count; i++)
    {
        FVector SpawnLocation = FVector(0, i * 200, 200);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        APawn* Enemy = World->SpawnActor<APawn>(
            EnemyClass,
            SpawnLocation,
            SpawnRotation
        );

        if (!Enemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy %d"), i);
            continue;
        }

	        // ---------------------------------------------------
	        // 5. Create an AI controller manually
	        // ---------------------------------------------------
	        AAIController* AI = World->SpawnActor<AAIController>(
	            AAIController::StaticClass(),
	            SpawnLocation,
	            SpawnRotation
	        );
	
	        if (!AI)
	        {
	            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AIController for enemy %d"), i);
	            continue;
	        }
	
	        AI->Possess(Enemy);
	
	        // ---------------------------------------------------
	        // 6. Create Blackboard & BehaviorTree components
	        // ---------------------------------------------------
	        UBlackboardComponent* BlackboardComp = NewObject<UBlackboardComponent>(AI);
	        BlackboardComp->RegisterComponent();
	        BlackboardComp->InitializeBlackboard(*BlackboardAsset);
	
	        UBehaviorTreeComponent* BTComp = NewObject<UBehaviorTreeComponent>(AI);
	        BTComp->RegisterComponent();
	
	        // ---------------------------------------------------
	        // 7. Create Custom Behaviour Tree Wrapper
	        // ---------------------------------------------------
	        UCustomBehaviourTree* CustomBT = NewObject<UCustomBehaviourTree>(AI);
	        CustomBT->BehaviorTreeAsset = BTAsset; // expose as UPROPERTY() in your class
	
	        // Initialize any internals in your custom class
	        CustomBT->InitializeTree(BlackboardComp, BTComp);
	
	        // ---------------------------------------------------
	        // 8. Launch the Behavior Tree
	        // ---------------------------------------------------
	        BTComp->StartTree(*BTAsset);
	
	        UE_LOG(LogTemp, Display, TEXT("Spawned AI %d and started behavior tree."), i);
    }
}

```