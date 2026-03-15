#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SimulationEventManager.generated.h"

// Enum to define all possible events that can happen in the simulation
UENUM(BlueprintType)
enum class ESimulationEvent : uint8
{
	None,
	NoFoodLeft,
	NoWaterLeft,
	SettlementComplete,
	PlayerDied,
	Timeout
};

// Standard C++ multicast delegate for your Genetic Manager to bind to
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationEventTriggered, ESimulationEvent /*EventType*/);

UCLASS()
class GENETICGENERATION_API USimulationEventManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// -- UWorldSubsystem Overrides --
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// -- Blueprint Callable Endpoints --
	// Your Blueprints will call these when conditions are met
	UFUNCTION(BlueprintCallable, Category = "Genetic Simulation|Events")
	void ReportNoFoodLeft();

	UFUNCTION(BlueprintCallable, Category = "Genetic Simulation|Events")
	void ReportNoWaterLeft();

	UFUNCTION(BlueprintCallable, Category = "Genetic Simulation|Events")
	void ReportSettlementComplete();

	UFUNCTION(BlueprintCallable, Category = "Genetic Simulation|Events")
	void ReportPlayerDeath();

	// Core reporting function
	UFUNCTION(BlueprintCallable, Category = "Genetic Simulation|Events")
	void ReportEvent(ESimulationEvent EventType);

	// The delegate C++ systems will listen to
	FOnSimulationEventTriggered OnSimulationEvent;
};