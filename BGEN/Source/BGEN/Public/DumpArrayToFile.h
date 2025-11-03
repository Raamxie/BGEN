// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DumpArrayToFile.generated.h"

/**
 * 
 */
UCLASS()
class BGEN_API UDumpArrayToFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="File|Debug")
	static bool DumpFloatArrayToFile(const TArray<float>& Array, const FString& FileName);
};
