#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneticSimulationManager.h" // For FSimulationResult
#include "GeneticExchangeLibrary.generated.h"

UCLASS()
class GENETICGENERATION_API UGeneticExchangeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** * Scans the GeneticExchange folder for assets created by other instances.
	 * Returns a list of SimulationResults containing the Package Path and extracted Fitness.
	 */
	static TArray<FSimulationResult> ScanForForeignGenomes(const FString& MyInstanceId);

	/** Generates a unique package name containing metadata for saving */
	static FString GenerateExchangePackagePath(const FString& InstanceId, int32 Generation, float Fitness);

	/** Checks the Generated folder for an existing evaluated tree. Returns Fitness if found, or -1.0f if new. */
	static float CheckIfTreeAlreadyEvaluated(const FString& TreeHash, FString& OutFoundPath);
};