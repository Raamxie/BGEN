#pragma once

#include "CoreMinimal.h" // Good practice to include this
#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"
#include "UBTTask_RunBehaviorRuntime.generated.h"

UCLASS()
class GENETICGENERATION_API UBTTask_RunBehaviorRuntime : public UBTTask_RunBehavior
{
	GENERATED_BODY()

public:
	// Expose the protected variable to our GA
	void SetBehaviorAsset(UBehaviorTree* InAsset)
	{
		BehaviorAsset = InAsset;
	}
};