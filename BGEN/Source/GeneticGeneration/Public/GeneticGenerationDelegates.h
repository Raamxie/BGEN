#pragma once

#include "CoreMinimal.h"

// One string param: the package path (eg "/Game/Generated/MyEvolvedBT")
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEvolvedTreeSaved, const FString& /*SavedAssetPath*/);

class GENETICGENERATION_API FGeneticGenerationDelegates
{
public:
	// Bind to this in editor module to get notified when runtime saves a new BT asset.
	static FOnEvolvedTreeSaved OnEvolvedTreeSaved;
};
