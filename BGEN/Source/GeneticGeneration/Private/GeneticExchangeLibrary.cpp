#include "GeneticExchangeLibrary.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/PackageName.h"
#include "Misc/Guid.h"

// Format: Ex_{InstanceID}_G{Gen}_F{Fit}_{GUID}
// Example: Ex_Island1_G5_F1250_A8B2...

TArray<FSimulationResult> UGeneticExchangeLibrary::ScanForForeignGenomes(const FString& MyInstanceId)
{
	TArray<FSimulationResult> Results;

	// 1. Get Directory on Disk
	// We assume assets are stored in Content/GeneticExchange
	FString ContentDir = FPaths::ProjectContentDir();
	FString ExchangeDir = ContentDir / TEXT("GeneticExchange");

	// 2. Find Files
	TArray<FString> FoundFiles;
	IFileManager::Get().FindFiles(FoundFiles, *ExchangeDir, TEXT(".uasset"));

	for (const FString& Filename : FoundFiles)
	{
		// Filename is "Ex_Island1_G5_F1250_GUID.uasset"

		// A. Skip Self
		if (Filename.Contains(MyInstanceId)) continue;

		// B. Parse Metadata
		// Remove extension
		FString PureName = FPaths::GetBaseFilename(Filename);
		TArray<FString> Parts;
		PureName.ParseIntoArray(Parts, TEXT("_"), true);

		// Expecting at least 5 parts: Ex, ID, Gen, Fit, GUID
		if (Parts.Num() < 5) continue;

		// Extract Data
		FString ForeignGenString = Parts[2]; // G5
		FString ForeignFitString = Parts[3]; // F1250

		ForeignGenString.RemoveFromStart("G");
		ForeignFitString.RemoveFromStart("F");

		if (!ForeignGenString.IsNumeric() || !ForeignFitString.IsNumeric()) continue;

		int32 Gen = FCString::Atoi(*ForeignGenString);
		float Fit = FCString::Atof(*ForeignFitString);

		// C. Build Result
		FSimulationResult Res;
		Res.Fitness = Fit;
		Res.GenerationNumber = Gen;
		
		// Convert Disk Path to Package Path: /Game/GeneticExchange/Filename
		Res.BehaviorTreePath = FString::Printf(TEXT("/Game/GeneticExchange/%s"), *PureName);

		Results.Add(Res);
	}

	if (Results.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Exchange: Found %d foreign genomes."), Results.Num());
	}

	return Results;
}

FString UGeneticExchangeLibrary::GenerateExchangePackagePath(const FString& InstanceId, int32 Generation, float Fitness)
{
	// Create a unique name embedded with data
	// Truncate fitness to int for cleaner filenames, or keep decimal if preferred
	int32 FitInt = FMath::RoundToInt(Fitness);
	FString Guid = FGuid::NewGuid().ToString(); // Ensure uniqueness

	FString Name = FString::Printf(TEXT("Ex_%s_G%d_F%d_%s"), *InstanceId, Generation, FitInt, *Guid);
	
	return FString::Printf(TEXT("/Game/GeneticExchange/%s"), *Name);
}