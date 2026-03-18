#include "SimulationEventManager.h"
#include "GeneticGenerationModule.h" // For your custom log category
#include "GeneticGeneration/Public/GeneticGenerationModule.h"

void USimulationEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogGeneticGeneration, Log, TEXT("SimulationEventManager Subsystem Initialized."));
}

void USimulationEventManager::Deinitialize()
{
	OnSimulationEvent.Clear();
	Super::Deinitialize();
}

void USimulationEventManager::ReportNoFoodLeft()
{
	ReportEvent(ESimulationEvent::NoFoodLeft);
}

void USimulationEventManager::ReportNoWaterLeft()
{
	ReportEvent(ESimulationEvent::NoWaterLeft);
}

void USimulationEventManager::ReportSettlementComplete()
{
	ReportEvent(ESimulationEvent::SettlementComplete);
}

void USimulationEventManager::ReportPlayerDeath()
{
	ReportEvent(ESimulationEvent::PlayerDied);
}

void USimulationEventManager::ReportEvent(ESimulationEvent EventType)
{
	FString WorldName = GetWorld() ? GetWorld()->GetName() : TEXT("Unknown");
	UE_LOG(LogGeneticGeneration, Warning, TEXT("EVENT DIAGNOSTIC: ReportEvent called (%d) inside World: %s"), static_cast<int32>(EventType), *WorldName);

	if (OnSimulationEvent.IsBound())
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("EVENT DIAGNOSTIC: Delegate IS BOUND! Broadcasting now..."));
		OnSimulationEvent.Broadcast(EventType);
	}
	else
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("EVENT DIAGNOSTIC: Delegate IS NOT BOUND! No one is listening."));
	}
}