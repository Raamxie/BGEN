// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratedPawn.h"

#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "GeometryCollection/GeometryCollectionParticlesData.h"

// Sets default values
AGeneratedPawn::AGeneratedPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGeneratedPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGeneratedPawn::SetID(int IDin)
{
	ID = IDin;
	GenerateBehaviorTree();
}

void AGeneratedPawn::GenerateBehaviorTree()
{
    const FString PackageName = FString::Printf(TEXT("/Game/Generated/GBT_%d"), ID);
    const FString PackageFilename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
    const FName AssetName = *FString::Printf(TEXT("GBT_%d"), ID);

    // 1. Create a new package
    UPackage* Package = CreatePackage(*PackageName);
    Package->SetFlags(RF_Public | RF_Standalone);
    Package->FullyLoad();

    // 2. Create the BehaviorTree *in that package directly*
    BehaviourTree = NewObject<UBehaviorTree>(Package, AssetName, RF_Public | RF_Standalone);
    BehaviourTree->MarkPackageDirty();

    // 3. Add it to the Asset Registry (only once)
    FAssetRegistryModule::AssetCreated(BehaviourTree);

    // 4. Create the root node and children
    UBTComposite_Selector* RootNode = NewObject<UBTComposite_Selector>(BehaviourTree, FName("RootSelector"));
    BehaviourTree->RootNode = RootNode;

    UBTTask_MoveTo* MoveTask = NewObject<UBTTask_MoveTo>(BehaviourTree, FName("MoveTask"));
    FBTCompositeChild Child;
    Child.ChildTask = MoveTask;
    RootNode->Children.Add(Child);

    // 5. Save the package (save the package, not just the object)
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.Error = GWarn;
    SaveArgs.SaveFlags = SAVE_None;

    if (UPackage::SavePackage(Package, nullptr, *PackageFilename, SaveArgs))
    {
        UE_LOG(LogTemp, Log, TEXT("Behavior Tree saved successfully: %s"), *PackageFilename);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save Behavior Tree: %s"), *PackageFilename);
    }

    // 6. Reload to verify
    UBehaviorTree* LoadedBT = LoadObject<UBehaviorTree>(nullptr, *PackageName);
    if (LoadedBT)
    {
    	UE_LOG(LogTemp, Log, TEXT("Verification successful: %s"), *PackageName);

    }
    else
    {
    	UE_LOG(LogTemp, Error, TEXT("Verification failed: %s"), *PackageName);

    }
}







// Called every frame
void AGeneratedPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGeneratedPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

