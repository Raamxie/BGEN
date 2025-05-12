// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LostPawn.generated.h"

UCLASS()
class BGEN_API ALostPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALostPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GenerateBehaviourTree();

	/** The root transform component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneRoot;

	/** A visible mesh so you can see your pawn move */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;
	

private:
	int ID;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void SetID(const int NewID);
	void SetMesh(UStaticMesh* Mesh);
	


};
