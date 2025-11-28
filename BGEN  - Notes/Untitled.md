```cpp
if (!World)  
{  
    UE_LOG(LogGeneticGeneration, Error, TEXT("No world context found!"));  
    return;  
}  
  
UE_LOG(LogGeneticGeneration, Log, TEXT("Simulation starting in world: %s"), *World->GetName());  
  
// --- 1) Choose a spawn location ------------------------------------------------------  
  
FVector SpawnLocation(0.f, 0.f, 100.f);  
FRotator SpawnRotation = FRotator::ZeroRotator;  
  
// Optional: Move spawn location if things overlap  
if (AActor* Player = World->GetFirstPlayerController() ? World->GetFirstPlayerController()->GetPawn() : nullptr)  
{  
    SpawnLocation = Player->GetActorLocation() + FVector(300.f, 0.f, 0.f);  
}  
  
// --- 2) Prepare spawn parameters -----------------------------------------------------  
  
FActorSpawnParameters SpawnParams;  
SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;  
SpawnParams.Owner = nullptr;  
SpawnParams.Instigator = nullptr;  
SpawnParams.bDeferConstruction = false;  
  
// --- 3) Spawn the character ----------------------------------------------------------  
  
UClass* CharacterClass = AGeneratedCharacter::StaticClass();  
if (!CharacterClass)  
{  
    UE_LOG(LogGeneticGeneration, Error, TEXT("AGeneratedCharacter class missing!"));  
    return;  
}  
  
AGeneratedCharacter* Enemy = World->SpawnActor<AGeneratedCharacter>(  
    CharacterClass,    SpawnLocation,    SpawnRotation,    SpawnParams);  
  
if (!Enemy)  
{  
    UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to spawn AGeneratedCharacter!"));  
    return;  
}  
  
UE_LOG(LogGeneticGeneration, Log, TEXT("Spawned GeneratedCharacter %s at %s"),  
       *Enemy->GetName(), *SpawnLocation.ToString());  
  
// --- 4) Wait until AIController is assigned -----------------------------------------  
  
World->GetTimerManager().SetTimerForNextTick([World, Enemy]()  
{  
    if (!Enemy)  
    {        UE_LOG(LogGeneticGeneration, Error, TEXT("Enemy disappeared before control assignment"));  
        return;  
    }  
    ACustomAIController* Controller = Cast<ACustomAIController>(Enemy->GetController());  
    if (!Controller)  
    {        UE_LOG(LogGeneticGeneration, Warning, TEXT("Enemy has no controller yet. Trying again..."));  
  
        // Try again next tick  
        World->GetTimerManager().SetTimerForNextTick([World, Enemy]()  
        {  
            ACustomAIController* C2 = Cast<ACustomAIController>(Enemy->GetController());  
            if (!C2)  
            {                UE_LOG(LogGeneticGeneration, Error, TEXT("Controller still missing after delay."));  
                return;  
            }  
            UE_LOG(LogGeneticGeneration, Log, TEXT("AI Controller acquired on retry: %s"), *C2->GetName());  
        });        return;  
    }  
    UE_LOG(LogGeneticGeneration, Log, TEXT("AIController assigned: %s"), *Controller->GetName());  
  
	    Controller->RuntimeBehaviourWrapper->Test_ModifyTreeAndSave(TEXT("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed"), TEXT("/Game/Generated/Test"));  
});  
  
// --- 5) Simulation is now running ----------------------------------------------------  
  
UE_LOG(LogGeneticGeneration, Log, TEXT("Simulation started successfully!"));
```