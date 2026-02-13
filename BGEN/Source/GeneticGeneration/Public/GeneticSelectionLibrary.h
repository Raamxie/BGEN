#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneticSimulationManager.h" 
#include "GeneticSelectionLibrary.generated.h"

UCLASS()
class GENETICGENERATION_API UGeneticSelectionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** * Tournament Selection:
	 * Picks 'TournamentSize' random individuals, returns the best one among them.
	 * Returns an empty result if Population is empty.
	 */
	static FSimulationResult TournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize = 3);

	/** Helper to check if a result is actually usable */
	static bool IsValidResult(const FSimulationResult& Result);
};