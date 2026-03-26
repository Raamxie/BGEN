#include "GeneticSelectionLibrary.h"

FSimulationResult UGeneticSelectionLibrary::TournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize)
{
	// SAFETY CHECK 1: Empty Population
	if (Population.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("TournamentSelection: Population is empty!"));
		return FSimulationResult(); 
	}

	// Just return the absolute best one
	FSimulationResult Best = Population[0];
	for (const auto& Res : Population)
	{
		if (Res.Fitness > Best.Fitness) Best = Res;
	}
	return Best;
}

bool UGeneticSelectionLibrary::IsValidResult(const FSimulationResult& Result)
{
	// A result is valid if it has a path and the generation is > 0
	return !Result.BehaviorTreePath.IsEmpty() && Result.GenerationNumber >= 0;
}