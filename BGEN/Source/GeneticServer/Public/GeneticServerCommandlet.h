#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticServerCommandlet.generated.h"

UCLASS()
class UGeneticServerCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UGeneticServerCommandlet();

	virtual int32 Main(const FString& Params) override;

private:
	// Central database of evaluated trees
	TMap<FString, float> GlobalEvaluatedTrees;
};