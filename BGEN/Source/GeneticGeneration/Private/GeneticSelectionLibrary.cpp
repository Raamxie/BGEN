#include "GeneticSelectionLibrary.h"

FSimulationResult UGeneticSelectionLibrary::TournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize)
{
	// SAFETY CHECK 1: Empty Population
	if (Population.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("TournamentSelection: Population is empty!"));
		return FSimulationResult(); 
	}

	// SAFETY CHECK 2: Population smaller than tournament
	if (Population.Num() <= TournamentSize)
	{
		// Just return the absolute best one
		FSimulationResult Best = Population[0];
		for (const auto& Res : Population)
		{
			if (Res.Fitness > Best.Fitness) Best = Res;
		}
		return Best;
	}

	// TOURNAMENT LOOP
	FSimulationResult BestContender = Population[FMath::RandRange(0, Population.Num() - 1)];

	for (int32 i = 0; i < TournamentSize - 1; i++)
	{
		const FSimulationResult& Challenger = Population[FMath::RandRange(0, Population.Num() - 1)];
		// We look for strict improvement
		if (Challenger.Fitness > BestContender.Fitness)
		{
			BestContender = Challenger;
		}
	}
    
	return BestContender;
}

bool UGeneticSelectionLibrary::IsValidResult(const FSimulationResult& Result)
{
	// A result is valid if it has a path and the generation is > 0
	return !Result.BehaviorTreePath.IsEmpty() && Result.GenerationNumber > 0;
}