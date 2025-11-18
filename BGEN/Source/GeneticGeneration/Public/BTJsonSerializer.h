#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BTJsonSerializer.generated.h"

USTRUCT()
struct FBTNodeJSON
{
	GENERATED_BODY()

	UPROPERTY()
	FString NodeClass;

	UPROPERTY()
	FString NodeName;

	UPROPERTY()
	TMap<FString, FString> Properties;
	
	TArray<FBTNodeJSON> Children;
	
	TArray<FBTNodeJSON> Decorators;
	
	TArray<FBTNodeJSON> Services;
};

UCLASS()
class GENETICGENERATION_API UBTJsonSerializer : public UObject
{
	GENERATED_BODY()

public:

	static bool SerializeBehaviorTreeToJSON(UBehaviorTree* BT, FString& OutJSON);

	static UBehaviorTree* DeserializeJSONToBehaviorTree(
		const FString& JSON,
		const FString& PackagePath,
		const FString& AssetName
	);

private:
	static FBTNodeJSON SerializeNode(UBTNode* Node);
	static UBTNode* CreateNodeFromJSON(const FBTNodeJSON& Data, UObject* Outer);
};
