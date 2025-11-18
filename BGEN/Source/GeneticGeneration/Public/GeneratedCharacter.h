#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GeneratedCharacter.generated.h"

class ACustomAIController;

UCLASS()
class GENETICGENERATION_API AGeneratedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGeneratedCharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void InitializeDelayed();

	void InitializeAI(ACustomAIController* Controller);

public:
	virtual void Tick(float DeltaTime) override;
};
