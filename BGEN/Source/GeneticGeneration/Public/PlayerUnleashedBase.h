// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerUnleashedBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEvent);
UCLASS()
class GENETICGENERATION_API APlayerUnleashedBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerUnleashedBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnPlayerEvent OnPlayerEvent;

	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	void LogDamageReceived(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "GeneticGeneration")
	void BroadcastPlayerDeath();
};
