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
    const FString PackageName = TEXT("/Game/Generated/GBT_") + FString::FromInt(ID);
    const FString PackageFilename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

    // 1. Create the package
    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();

    // 2. Create the BehaviorTree asset
    BehaviourTree = NewObject<UBehaviorTree>(Package, UBehaviorTree::StaticClass(), FName("GBT_69"), RF_Public | RF_Standalone);
    FAssetRegistryModule::AssetCreated(BehaviourTree);


    // 3. Notify the AssetRegistry that the asset exists
    FAssetRegistryModule::AssetCreated(BehaviourTree);

    // 4. Now create nodes inside the BT
    UBTComposite_Selector* RootNode = NewObject<UBTComposite_Selector>(BehaviourTree, FName("RootSelector"));
    BehaviourTree->RootNode = RootNode;


    UBTTask_MoveTo* MoveTask = NewObject<UBTTask_MoveTo>(
        BehaviourTree,
        FName("MoveTask")
    );

    FBTCompositeChild Child;
    Child.ChildTask = MoveTask;
    RootNode->Children.Add(Child);

    // 5. Now save the package
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.Error = GWarn;
    SaveArgs.SaveFlags = SAVE_None;

    if (UPackage::SavePackage(Package, BehaviourTree, *PackageFilename, SaveArgs))
    {
        UE_LOG(LogTemp, Log, TEXT("Behavior Tree saved successfully: %s"), *PackageFilename);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save Behavior Tree: %s"), *PackageFilename);
    }

    // 6. Optional: Verify loading
    UBehaviorTree* LoadedBT = LoadObject<UBehaviorTree>(nullptr, *PackageName);
    if (!LoadedBT)
    {
        UE_LOG(LogTemp, Error, TEXT("Verification failed: could not load BehaviorTree: %s"), *PackageName);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Verification successful: BehaviorTree loaded"));
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

