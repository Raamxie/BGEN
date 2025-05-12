// Fill out your copyright notice in the Description page of Project Settings.


#include "EvolutionManager.h"

#include "Kismet/KismetMathLibrary.h"
#include "LostPawn.h"

// Sets default values
AEvolutionManager::AEvolutionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEvolutionManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnActors();
	
}

// Called every frame
void AEvolutionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEvolutionManager::SpawnActors()
{
	const FVector Center = FVector(0, 0,0);
	for (int ID = 0; ID < SpawnCount; ++ID)
	{
		FVector2D Rand2D = FMath::RandPointInCircle(1.0f);

		// Convert to 3D and normalize (to ensure it’s exactly unit-length)
		FVector RandDir2D = FVector(Rand2D, 0.0f).GetSafeNormal();
		
		FVector SpawnLocation = Center + RandDir2D * DistanceFromCenter;
		SpawnLocation.Z = 0.f;
		FRotator SpawnRotation = FRotator::ZeroRotator;
		

		ALostPawn* SpawnedPawn = GetWorld()->SpawnActor<ALostPawn>(ALostPawn::StaticClass(), SpawnLocation, SpawnRotation);
		SpawnedPawn->SetID(ID);
		SpawnedPawn->SetMesh(PawnMesh);
	}
}


