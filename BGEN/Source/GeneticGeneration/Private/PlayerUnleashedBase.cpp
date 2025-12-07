// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnleashedBase.h"

#include "CustomAIController.h"
#include "GeneticGenerationModule.h"

// Sets default values
APlayerUnleashedBase::APlayerUnleashedBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerUnleashedBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerUnleashedBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerUnleashedBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerUnleashedBase::LogDamageReceived(float DamageAmount)
{
	UE_LOG(LogGeneticGeneration, Warning, TEXT("VERIFICATION: Player took %f damage!"), DamageAmount);
    
	// Optional: Check death logic here in C++ if you want
	// if (Health <= 0) { OnPlayerEvent.Broadcast(); }
}

void APlayerUnleashedBase::BroadcastPlayerDeath()
{
	UE_LOG(LogGeneticGeneration, Warning, TEXT("VERIFICATION: BroadcastPlayerDeath called from Blueprint!"));
	if (OnPlayerEvent.IsBound())
	{
		OnPlayerEvent.Broadcast();
	}
	else
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: No listeners bound to OnPlayerEvent!"));
	}
}