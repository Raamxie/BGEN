#include "GeneticSelectionLibrary.h"

FSimulationResult UGeneticSelectionLibrary::TournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize)
{
	// SAFETY CHECK: Empty Population
	if (Population.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("TournamentSelection: Population is empty!"));
		return FSimulationResult(); 
	}

	int32 ActualTournamentSize = FMath::Min(TournamentSize, Population.Num());

	FSimulationResult BestContender;
	bool bFirstSet = false;

	// Note: For NSGA-II, tournament selection typically compares FrontIndex.
	// We will keep this backwards-compatible, but the main flow will use FastNonDominatedSort.
	for (int32 i = 0; i < ActualTournamentSize; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, Population.Num() - 1);
		const FSimulationResult& Contender = Population[RandomIndex];

		if (!bFirstSet || Contender.Fitness > BestContender.Fitness)
		{
			BestContender = Contender;
			bFirstSet = true;
		}
	}

	return BestContender;
}

bool UGeneticSelectionLibrary::IsValidResult(const FSimulationResult& Result)
{
	return !Result.BehaviorTreePath.IsEmpty() && Result.GenerationNumber >= 0;
}

// =========================================================================
// NSGA-II MULTI-OBJECTIVE LOGIC
// =========================================================================

bool UGeneticSelectionLibrary::Dominates(const FSimulationResult& A, const FSimulationResult& B, int32 ActiveObjectives)
{
	bool bIsStrictlyBetter = false;

	// 1. Damage Dealt (Maximize)
	if (ActiveObjectives >= 1)
	{
		if (A.DamageDealt < B.DamageDealt) return false;
		if (A.DamageDealt > B.DamageDealt) bIsStrictlyBetter = true;
	}

	// 2. Damage Taken (Minimize)
	if (ActiveObjectives >= 2)
	{
		if (A.DamageTaken > B.DamageTaken) return false;
		if (A.DamageTaken < B.DamageTaken) bIsStrictlyBetter = true;
	}

	// 3. Distance Travelled (Maximize)
	if (ActiveObjectives >= 3)
	{
		if (A.DistanceTravelled < B.DistanceTravelled) return false;
		if (A.DistanceTravelled > B.DistanceTravelled) bIsStrictlyBetter = true;
	}

	// 4. Tree Utilization (Maximize)
	if (ActiveObjectives >= 4)
	{
		if (A.TreeUtilization < B.TreeUtilization) return false;
		if (A.TreeUtilization > B.TreeUtilization) bIsStrictlyBetter = true;
	}

	// 5. Tree Size (Minimize)
	if (ActiveObjectives >= 5)
	{
		if (A.TreeSize > B.TreeSize) return false;
		if (A.TreeSize < B.TreeSize) bIsStrictlyBetter = true;
	}

	return bIsStrictlyBetter;
}

TArray<TArray<FSimulationResult>> UGeneticSelectionLibrary::FastNonDominatedSort(const TArray<FSimulationResult>& Population, int32 ActiveObjectives)
{
	TArray<TArray<FSimulationResult>> Fronts;
	if (Population.Num() == 0) return Fronts;

	TArray<int32> DominationCounts;
	DominationCounts.Init(0, Population.Num());

	TArray<TArray<int32>> DominatedSets;
	DominatedSets.SetNum(Population.Num());

	TArray<int32> CurrentFrontIndices;

	// Calculate Dominance
	for (int32 p = 0; p < Population.Num(); p++)
	{
		for (int32 q = 0; q < Population.Num(); q++)
		{
			if (p == q) continue;

			if (Dominates(Population[p], Population[q], ActiveObjectives))
			{
				DominatedSets[p].Add(q);
			}
			else if (Dominates(Population[q], Population[p], ActiveObjectives))
			{
				DominationCounts[p]++;
			}
		}

		if (DominationCounts[p] == 0)
		{
			CurrentFrontIndices.Add(p);
		}
	}

	// Build Successive Fronts
	int32 FrontIndex = 0;
	while (CurrentFrontIndices.Num() > 0)
	{
		TArray<FSimulationResult> CurrentFront;
		TArray<int32> NextFrontIndices;

		for (int32 pIndex : CurrentFrontIndices)
		{
			// Assign FrontIndex metadata
			FSimulationResult Individual = Population[pIndex];
			Individual.FrontIndex = FrontIndex;
			CurrentFront.Add(Individual);

			for (int32 qIndex : DominatedSets[pIndex])
			{
				DominationCounts[qIndex]--;
				if (DominationCounts[qIndex] == 0)
				{
					NextFrontIndices.Add(qIndex);
				}
			}
		}

		Fronts.Add(CurrentFront);
		CurrentFrontIndices = NextFrontIndices;
		FrontIndex++;
	}

	return Fronts;
}

TArray<FSimulationResult> UGeneticSelectionLibrary::PriorityTieBreakingSelection(TArray<FSimulationResult> CutoffFront, int32 NeededCount, int32 CurrentObjectiveCount)
{
	TArray<FSimulationResult> Selected;

	// Base Case 1: The front fits perfectly
	if (CutoffFront.Num() <= NeededCount)
	{
		return CutoffFront;
	}

	// Base Case 2: We are down to 1 objective (Damage Dealt). 
	// Sort lexicographically down the priority list as a final tie-breaker.
	if (CurrentObjectiveCount <= 1)
	{
		CutoffFront.Sort([](const FSimulationResult& A, const FSimulationResult& B) {
			if (A.DamageDealt != B.DamageDealt) return A.DamageDealt > B.DamageDealt;
			if (A.DamageTaken != B.DamageTaken) return A.DamageTaken < B.DamageTaken;
			if (A.DistanceTravelled != B.DistanceTravelled) return A.DistanceTravelled > B.DistanceTravelled;
			if (A.TreeUtilization != B.TreeUtilization) return A.TreeUtilization > B.TreeUtilization;
			return A.TreeSize < B.TreeSize;
		});

		for (int32 i = 0; i < NeededCount; i++)
		{
			Selected.Add(CutoffFront[i]);
		}
		return Selected;
	}

	// Recursive Case: Drop the lowest priority objective and re-sort into new sub-fronts
	TArray<TArray<FSimulationResult>> SubFronts = FastNonDominatedSort(CutoffFront, CurrentObjectiveCount);

	for (TArray<FSimulationResult>& SubFront : SubFronts)
	{
		if (Selected.Num() + SubFront.Num() <= NeededCount)
		{
			// This sub-front fits completely
			Selected.Append(SubFront);
		}
		else
		{
			// The cutoff reached again, recurse with one less objective
			int32 RemainingNeeded = NeededCount - Selected.Num();
			if (RemainingNeeded > 0)
			{
				TArray<FSimulationResult> TieBroken = PriorityTieBreakingSelection(SubFront, RemainingNeeded, CurrentObjectiveCount - 1);
				Selected.Append(TieBroken);
			}
			break; // We have enough individuals now
		}
	}

	return Selected;
}

FSimulationResult UGeneticSelectionLibrary::ParetoTournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize)
{
	if (Population.Num() == 0) return FSimulationResult(); 

	int32 ActualTournamentSize = FMath::Min(TournamentSize, Population.Num());

	FSimulationResult BestContender;
	bool bFirstSet = false;

	for (int32 i = 0; i < ActualTournamentSize; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, Population.Num() - 1);
		const FSimulationResult& Contender = Population[RandomIndex];

		// In NSGA-II, a LOWER FrontIndex is better.
		// If tied, we fallback to the highest priority objective (Damage Dealt) as a tie-breaker.
		if (!bFirstSet || 
			Contender.FrontIndex < BestContender.FrontIndex || 
			(Contender.FrontIndex == BestContender.FrontIndex && Contender.DamageDealt > BestContender.DamageDealt))
		{
			BestContender = Contender;
			bFirstSet = true;
		}
	}
	return BestContender;
}