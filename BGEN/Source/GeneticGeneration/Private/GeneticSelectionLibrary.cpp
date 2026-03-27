#include "GeneticSelectionLibrary.h"

FSimulationResult UGeneticSelectionLibrary::TournamentSelection(const TArray<FSimulationResult>& Population, int32 TournamentSize)
{
	// SAFETY CHECK: Empty Population
	if (Population.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("TournamentSelection: Population is empty!"));
		return FSimulationResult(); 
	}

	// SAFETY CHECK: Ensure tournament size isn't larger than the population
	int32 ActualTournamentSize = FMath::Min(TournamentSize, Population.Num());

	FSimulationResult BestContender;
	bool bFirstSet = false;

	// Pick N random individuals and find the best among them
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
	// A result is valid if it has a path and the generation is > 0
	return !Result.BehaviorTreePath.IsEmpty() && Result.GenerationNumber >= 0;
}