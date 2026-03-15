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
	UE_LOG(LogGeneticGeneration, Warning, TEXT("Simulation Event Triggered: %d"), static_cast<int32>(EventType));

	if (OnSimulationEvent.IsBound())
	{
		OnSimulationEvent.Broadcast(EventType);
	}
}