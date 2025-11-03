#include "GeneticGenerationModule.h"
#include "Engine/Engine.h" 
#include "Engine/World.h"
#include "Engine/GameEngine.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "HAL/PlatformMisc.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Containers/Ticker.h"
#include "Misc/OutputDeviceFile.h"
#include "GeneticGeneration/Public/GeneratedPawn.h"

DEFINE_LOG_CATEGORY(LogGeneticGeneration);

void FGeneticGenerationModule::StartupModule()
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("GeneticRun")))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration module skipped (no -GeneticRun flag)."));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));    

    static FOutputDeviceFile GeneticLogFile(*FPaths::Combine(FPaths::ProjectLogDir(), TEXT("GeneticGeneration.log")), true);

    // Add it as a global output device
    if (!GLog->IsRedirectingTo(&GeneticLogFile))
    {
        GLog->AddOutputDevice(&GeneticLogFile);

        UE_LOG(LogGeneticGeneration, Log, TEXT("Custom log file initialized at %s"), GeneticLogFile.GetFilename());

    }
    
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));

    FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FGeneticGenerationModule::OnWorldInitialized);
}

void FGeneticGenerationModule::ShutdownModule()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule shutting down."));
}

IMPLEMENT_MODULE(FGeneticGenerationModule, GeneticGeneration)


void FGeneticGenerationModule::OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("OnWorldInitialized called for world: %s"), *world->GetName());

    if (!world)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("OnWorldInitialized called with null world."));
        return;
    }
    
    if (world->GetName() != TEXT("Main") && world->GetName() != TEXT("Benchmark"))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("Skipping non-main world: %s"), *world->GetName());
        return;
    }

    // Simulate delayed execution manually (since no loop runs)
    UE_LOG(LogGeneticGeneration, Log, TEXT("World %s initialized, running simulation now."), *world->GetName());

    if (world->GetName() == TEXT("Benchmark"))
    {
        RunBenchmark(world);
    }
    else
    {
        RunSimulation(world);
    }
}



void FGeneticGenerationModule::RunSimulation(UWorld* world)
{
    if (!world)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("No world context found!"));
        FPlatformMisc::RequestExit(false);
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("World found: %s"), *world->GetName());

    // === Spawn an actor before ticking ===
    FActorSpawnParameters spawnParams;
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    FTransform spawnTransform = FTransform::Identity;
    AGeneratedPawn* spawnedPawn = world->SpawnActor<AGeneratedPawn>(
        AGeneratedPawn::StaticClass(), spawnTransform, spawnParams);
    
    if (spawnedPawn)
    {
        spawnedPawn->SetID(69);
        UE_LOG(LogGeneticGeneration, Log, TEXT("Spawned pawn %s with ID %d"),
            *spawnedPawn->GetName(), spawnedPawn->GetID());
    }
    // else
    // {
    //     UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to spawn AGeneratedPawn"));
    // }

    // === Simulation parameters ===
    const int32 maxTicks = 300;       // ~5 seconds at 60 FPS
    const float deltaSeconds = 1.0f / 60.0f;

    // === Timing measurement ===
    double startTime = FPlatformTime::Seconds();
    double lastTime = startTime;
    double totalSimulated = 0.0;
    int32 frameCount = 0;

    for (int32 i = 0; i < maxTicks; ++i)
    {
        double frameStart = FPlatformTime::Seconds();
        double deltaReal = frameStart - lastTime;
        lastTime = frameStart;

        // Optional: make Unreal's subsystems see the correct DeltaTime
        FApp::SetDeltaTime(deltaSeconds);
        FApp::SetCurrentTime(FApp::GetCurrentTime() + deltaSeconds);

        world->Tick(ELevelTick::LEVELTICK_All, deltaSeconds);

        // FIX: add *simulation* delta, not real delta
        totalSimulated += deltaSeconds;
        frameCount++;
    }

    double endTime = FPlatformTime::Seconds();
    double elapsedWall = endTime - startTime;
    double avgFPS = frameCount / elapsedWall;
    double speedup = totalSimulated / elapsedWall;

    UE_LOG(LogGeneticGeneration, Display,
        TEXT("Simulated %.2fs in %.2fs (%.2f FPS, %.2fx faster than real time)"),
        totalSimulated, elapsedWall, avgFPS, speedup);

    UE_LOG(LogGeneticGeneration, Log, TEXT("Simulation complete."));
    FPlatformMisc::RequestExit(false);
}

void FGeneticGenerationModule::RunBenchmark(UWorld* world)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("Benchmark Started!\n\n\n"));
}

