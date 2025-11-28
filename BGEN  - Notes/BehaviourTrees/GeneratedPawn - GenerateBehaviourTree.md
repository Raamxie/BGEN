```cpp
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
    {        UE_LOG(LogTemp, Log, TEXT("Behavior Tree saved successfully: %s"), *PackageFilename);  
    }    else  
    {  
        UE_LOG(LogTemp, Error, TEXT("Failed to save Behavior Tree: %s"), *PackageFilename);  
    }  
    // 6. Reload to verify  
    UBehaviorTree* LoadedBT = LoadObject<UBehaviorTree>(nullptr, *PackageName);  
    if (LoadedBT)  
    {        UE_LOG(LogTemp, Log, TEXT("Verification successful: %s"), *PackageName);  
  
    }    else  
    {  
        UE_LOG(LogTemp, Error, TEXT("Verification failed: %s"), *PackageName);  
  
    }}```