// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EvolutionManager.generated.h"

UCLASS()
class BGEN_API AEvolutionManager : public AActor
{
	GENERATED_BODY()
	TMap<int, int> FinishedActors;
	void CalculateFitness();
	void SpawnActors();
	void DestroyActors();
	void NextStep();
	UPROPERTY()
	TArray<AActor*> SpawnedActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	AEvolutionManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Category: Simulation
	UPROPERTY(EditAnywhere, Category = "Simulation")
	int32 SpawnCount = 100;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	float SimulationSpeedMultiplier = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	float TickSpeed = 0.01f;

	// Category: Placement
	UPROPERTY(EditAnywhere, Category = "Placement")
	int32 DistanceFromCenter = 500;

	UPROPERTY(EditAnywhere, Category = "Placement")
	int32 AcceptanceRange = 20;

	// Category: Visual
	UPROPERTY(EditAnywhere, Category = "Visual")
	UStaticMesh* PawnMesh;
	int GetAcceptanceRange() const;
	void FinishedTask(int ID, int StepsTaken);
};
