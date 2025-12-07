#include "GeneticSimulationManager.h"
#include "GeneticGenerationModule.h" // For LogGeneticGeneration
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "PlayerUnleashedBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardData.h"

UGeneticSimulationManager::UGeneticSimulationManager()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: UGeneticSimulationManager constructed (Address: %p)"), this);
}

void UGeneticSimulationManager::Init(UWorld* InWorld)
{
    TargetWorld = InWorld;
    if (TargetWorld)
    {
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Manager Initialized. Target World: %s"), *TargetWorld->GetName());
    }
    else
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: Manager Init FAILED. TargetWorld is NULL!"));
    }
	if (GEngine)
	{
		GEngine->bSmoothFrameRate = false;
		GEngine->FixedFrameRate = 0.0f; // Uncapped
	}
}

void UGeneticSimulationManager::OnLevelReload(UWorld* NewWorld)
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("PERSISTENCE: Manager surviving into Generation %d"), GenerationCount);
	UKismetSystemLibrary::QuitEditor();

	// 1. Update the World Context
	TargetWorld = NewWorld;

	// 2. Safety: Clear any old timers from the previous world
	// (Although the world destruction usually handles this, it's safe to reset the handle)
	TimerHandle.Invalidate();

	// 3. Reset Transient State
	EnemySpawnPositions.Empty(); 
	// Clear any lists of actors from the old world, as those pointers are now invalid garbage!
    
	// 4. Restart the Logic
	SetPause(true);
	PreparePlayer();
	SpawnEnemies(1); // Pass gen count or difficulty based on 'GenerationCount'
	Simulate();
}

UWorld* UGeneticSimulationManager::GetWorld() const
{
    return TargetWorld;
}

void UGeneticSimulationManager::PreparePlayer()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: PreparePlayer called."));

    if (!TargetWorld) return;

    APlayerUnleashedBase* PlayerUnleashed = nullptr;

    // 1. Try to find existing first
    AActor* ExistingActor = UGameplayStatics::GetPlayerCharacter(TargetWorld, 0);
    PlayerUnleashed = Cast<APlayerUnleashedBase>(ExistingActor);

    // 2. If not found, spawn it
    if (!PlayerUnleashed)	
    {
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Player not found or wrong class. Spawning..."));

        UClass* PlayerPawnClass = GetClassFromPath("/Game/Actors/PlayerUnleashed/PlayerUnleashed.PlayerUnleashed_C");
        if (!PlayerPawnClass) return;

        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(TargetWorld, APlayerStart::StaticClass());
        FVector SpawnLoc = PlayerStart ? PlayerStart->GetActorLocation() : FVector(0, 0, 200);
        FRotator SpawnRot = PlayerStart ? PlayerStart->GetActorRotation() : FRotator::ZeroRotator;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // SPAWN
        APawn* SpawnedPawn = TargetWorld->SpawnActor<APawn>(PlayerPawnClass, SpawnLoc, SpawnRot, Params);
        
        // CAST IMMEDIATELY
        PlayerUnleashed = Cast<APlayerUnleashedBase>(SpawnedPawn);

        if (SpawnedPawn && TargetWorld->GetFirstPlayerController())
        {
            TargetWorld->GetFirstPlayerController()->Possess(SpawnedPawn);
        }
    }

    // 3. Bind Event
    if (PlayerUnleashed)
    {
        PlayerUnleashed->OnPlayerEvent.RemoveAll(this);
        PlayerUnleashed->OnPlayerEvent.AddDynamic(this, &UGeneticSimulationManager::PlayerDiedListener);
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: SUCCESS - Bound to Player Death Delegate."));
    }
    else
    {
        // If this prints, the Reparenting (Step 1) failed.
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: FAILURE - Spawned actor is NOT a APlayerUnleashedBase. Did you reparent the Blueprint?"));
    }
}

void UGeneticSimulationManager::SpawnEnemies(int32 AmountToSpawn)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: SpawnEnemies called. Requesting %d enemies."), AmountToSpawn);

    if (!TargetWorld) return;

    UClass* EnemyClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/EnemyUnleashed.EnemyUnleashed_C");
    UClass* ControllerClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/AIController_EnemyUnleashed.AIController_EnemyUnleashed_C");
    UBlackboardData* BBAsset = Cast<UBlackboardData>(LoadObjectFromPath("/Game/Actors/EnemyUnleashed/BB_EnemyUnleashed.BB_EnemyUnleashed"));

    if (!EnemyClass || !ControllerClass || !BBAsset)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: Critical Asset missing. Aborting Spawn."));
        return;
    }

    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        FVector Loc = (EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * -60.0f, 25.0f, 210.0f);
        
        // Spawn Pawn
        ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, FRotator::ZeroRotator);
        if (!Enemy) 
        {
            UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Failed to spawn Enemy Pawn %d"), i);
            continue;
        }

        // Spawn Controller
        ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, FRotator::ZeroRotator);
        if (AI)
        {
            AI->Possess(Enemy);
            
            // Generate Tree
            UCustomBehaviourTree* GenHelper = NewObject<UCustomBehaviourTree>(this);
            GenHelper->LoadBehaviorTree("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed");
            
            // bool bMutated = GenHelper->MutateTree_AddSequenceWithAttack();
            // if(bMutated)
            // {
            //      UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: Tree mutated for Enemy %d"), i);
            // }

            UBehaviorTree* FinalTree = GenHelper->GetBTAsset();

            if (FinalTree)
            {
                AI->AssignTree(FinalTree, BBAsset);
                AI->RunAssignedTree();
                UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Enemy %d Spawned and Tree Running."), i);
            }
        }
    }
}

void UGeneticSimulationManager::SetSpawnPositions(TArray<FVector> positions)
{
}

void UGeneticSimulationManager::Simulate()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulate() called. Starting Timer."));

    if (!TargetWorld) return;

	UGameplayStatics::SetGlobalTimeDilation(TargetWorld, 1.0f);
    // Timeout (e.g., 30 seconds)
	float TimeoutGameSeconds = 30.0f;
	TargetWorld->GetTimerManager().SetTimer(
		TimerHandle, 
		this, 
		&UGeneticSimulationManager::TimerCallback, 
		TimeoutGameSeconds, 
		false
	);
	
    SetPause(false);
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulation active. Timeout set for %.2fs."), TimeoutGameSeconds);
}

void UGeneticSimulationManager::TimerCallback()
{
    UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: TIMEOUT! Simulation took too long."));
	TriggerRestart();
}

void UGeneticSimulationManager::PlayerDiedListener()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: EVENT: Player Died! Stopping simulation."));
    TriggerRestart();
}

void UGeneticSimulationManager::StopSimulation()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Stopping Simulation..."));

    // Clear timer
    if (TargetWorld)
    {
        TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
    }

    SetPause(true);
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Game Paused. Simulation Complete."));
}

void UGeneticSimulationManager::TriggerRestart()
{
	if (!TargetWorld) return;

	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Restarting Level..."));

	// Ensure the timer is cleared so it doesn't fire while the map is unloading
	if (TargetWorld->GetTimerManager().TimerExists(TimerHandle))
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	}

	// Get current map name (e.g., "Main")
	FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(TargetWorld);

	// Reload the map
	UGameplayStatics::OpenLevel(TargetWorld, FName(*CurrentMapName));
}

void UGeneticSimulationManager::SetPause(bool bPauseState)
{
    if (TargetWorld)
    {
        UGameplayStatics::SetGamePaused(TargetWorld, bPauseState);
        UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: World Paused state set to: %s"), bPauseState ? TEXT("TRUE") : TEXT("FALSE"));
    }
}

bool UGeneticSimulationManager::DoesPlayerExist() const
{
    return UGameplayStatics::GetPlayerCharacter(TargetWorld, 0) != nullptr;
}

UClass* UGeneticSimulationManager::GetClassFromPath(const FString& Path)
{
    UClass* Cls = StaticLoadClass(UObject::StaticClass(), nullptr, *Path);
    if (!Cls) UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Failed to load class: %s"), *Path);
    return Cls;
}

UObject* UGeneticSimulationManager::LoadObjectFromPath(const FString& Path)
{
    UObject* Obj = StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
    if (!Obj) UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Failed to load object: %s"), *Path);
    return Obj;
}

bool UGeneticSimulationManager::IsPaused() const
{
    return UGameplayStatics::IsGamePaused(TargetWorld);
}