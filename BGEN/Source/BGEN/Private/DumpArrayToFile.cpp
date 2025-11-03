// Fill out your copyright notice in the Description page of Project Settings.


#include "DumpArrayToFile.h"

bool UDumpArrayToFile::DumpFloatArrayToFile(const TArray<float>& Array, const FString& FileName)
{
	if (Array.Num() == 0 || FileName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("DumpFloatArrayToFile: Empty array or filename"));
		return false;
	}

	// Convert float array to string, one number per line
	FString FileContent;
	FileContent += TEXT("Frame Time; FPS\n");
	for (int32 i = 0; i < Array.Num(); ++i)
	{
		FileContent += FString::SanitizeFloat(Array[i]);
		FileContent += TEXT(";");
		FileContent += FString::SanitizeFloat(1.0f / Array[i]);
		FileContent += TEXT("\n"); // new line after each number
	}

	// Full path in ProfilingDir
	FString FullPath = FPaths::ProfilingDir() / FileName;

	// Save or append file
	bool bSuccess = FFileHelper::SaveStringToFile(FileContent, *FullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Float array dumped to file: %s"), *FullPath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to dump float array to file: %s"), *FullPath);
	}

	return bSuccess;
}
