// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "EditorAssetLibrary.h"
#include "UObject/SavePackage.h"
#include "GeneratedPawn.generated.h"


UCLASS()
class GENETICGENERATION_API AGeneratedPawn : public APawn
{
	GENERATED_BODY()

public:
	AGeneratedPawn();
	void SetID(int IDin);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GenerateBehaviorTree();
	UBehaviorTree* BehaviourTree;
	int ID;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	int GetID() { return ID; }

};
