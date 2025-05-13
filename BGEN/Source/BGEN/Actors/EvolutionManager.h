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
	void CalculateFitness() const;
	UPROPERTY()
	TArray<AActor*> SpawnedActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnActors();

public:
	AEvolutionManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 100;
	UPROPERTY(EditAnywhere)
	int32 DistanceFromCenter = 500;
	UPROPERTY(EditAnywhere)
	int32 AcceptanceRange = 20;
	UPROPERTY(EditAnywhere)
	UStaticMesh* PawnMesh;
	UPROPERTY(EditAnywhere)
	float SimulationSpeedMultiplier = 4.0f;
	int GetAcceptanceRange() const;
	void FinishedTask(int ID, int StepsTaken);
};
