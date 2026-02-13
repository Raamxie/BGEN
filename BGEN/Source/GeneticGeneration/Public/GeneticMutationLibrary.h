#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomBehaviourTree.h" 
#include "GeneticMutationLibrary.generated.h"

UCLASS()
class GENETICGENERATION_API UGeneticMutationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** * Mutates the provided tree wrapper. 
	 * Returns a log string describing the change, or empty string if no mutation occurred.
	 */
	UFUNCTION(BlueprintCallable, Category = "Genetic Mutation")
	static FString MutateTree(UCustomBehaviourTree* Wrapper, float MutationRate);

private:
	// --- Actual Mutation Logic ---
	static FString AddNewTask(UCustomBehaviourTree* Wrapper);
	static FString AddNewComposite(UCustomBehaviourTree* Wrapper);
	static FString SwapTask(UCustomBehaviourTree* Wrapper);
	static FString ShuffleChildren(UCustomBehaviourTree* Wrapper);
};