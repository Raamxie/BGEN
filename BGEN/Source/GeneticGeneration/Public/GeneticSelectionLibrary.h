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

	// --- NSGA-II MULTI-OBJECTIVE ALGORITHMS ---

	/** * Determines if Genome A dominates Genome B.
	 * ActiveObjectives controls how many objectives to evaluate (5 down to 1 for tie-breaking).
	 */
	static bool Dominates(const FSimulationResult& A, const FSimulationResult& B, int32 ActiveObjectives = 5);

	/** * Sorts a population into Non-Dominated Pareto Fronts.
	 * Returns an array of Fronts, where Fronts[0] is the best non-dominated set.
	 */
	static TArray<TArray<FSimulationResult>> FastNonDominatedSort(const TArray<FSimulationResult>& Population, int32 ActiveObjectives = 5);

	/** * Selects exactly 'NeededCount' individuals from a Cutoff Front.
	 * It drops the lowest priority objective and re-sorts into Pareto Fronts until the count is met.
	 */
	static TArray<FSimulationResult> PriorityTieBreakingSelection(TArray<FSimulationResult> CutoffFront, int32 NeededCount, int32 CurrentObjectiveCount = 4);

	FSimulationResult ParetoTournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize);

};