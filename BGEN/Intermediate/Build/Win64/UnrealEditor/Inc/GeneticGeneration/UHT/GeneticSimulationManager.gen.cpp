// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeneticSimulationManager.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGeneticSimulationManager() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ENGINE_API UClass* Z_Construct_UClass_UWorld_NoRegister();
GENETICGENERATION_API UClass* Z_Construct_UClass_UGeneticSimulationManager();
GENETICGENERATION_API UClass* Z_Construct_UClass_UGeneticSimulationManager_NoRegister();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UGeneticSimulationManager Function PlayerDiedListener ********************
struct Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGeneticSimulationManager, nullptr, "PlayerDiedListener", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGeneticSimulationManager::execPlayerDiedListener)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->PlayerDiedListener();
	P_NATIVE_END;
}
// ********** End Class UGeneticSimulationManager Function PlayerDiedListener **********************

// ********** Begin Class UGeneticSimulationManager Function TimerCallback *************************
struct Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGeneticSimulationManager, nullptr, "TimerCallback", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGeneticSimulationManager::execTimerCallback)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->TimerCallback();
	P_NATIVE_END;
}
// ********** End Class UGeneticSimulationManager Function TimerCallback ***************************

// ********** Begin Class UGeneticSimulationManager ************************************************
void UGeneticSimulationManager::StaticRegisterNativesUGeneticSimulationManager()
{
	UClass* Class = UGeneticSimulationManager::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "PlayerDiedListener", &UGeneticSimulationManager::execPlayerDiedListener },
		{ "TimerCallback", &UGeneticSimulationManager::execTimerCallback },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGeneticSimulationManager;
UClass* UGeneticSimulationManager::GetPrivateStaticClass()
{
	using TClass = UGeneticSimulationManager;
	if (!Z_Registration_Info_UClass_UGeneticSimulationManager.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("GeneticSimulationManager"),
			Z_Registration_Info_UClass_UGeneticSimulationManager.InnerSingleton,
			StaticRegisterNativesUGeneticSimulationManager,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UGeneticSimulationManager.InnerSingleton;
}
UClass* Z_Construct_UClass_UGeneticSimulationManager_NoRegister()
{
	return UGeneticSimulationManager::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UGeneticSimulationManager_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Manages the genetic algorithm simulation loop.\n * This object remains persistent across level reloads to store generation data.\n */" },
#endif
		{ "IncludePath", "GeneticSimulationManager.h" },
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Manages the genetic algorithm simulation loop.\nThis object remains persistent across level reloads to store generation data." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GenerationCount_MetaData[] = {
		{ "Category", "Genetic Data" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --- Persistent Data (Survives Level Reloads) ---\n" },
#endif
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "--- Persistent Data (Survives Level Reloads) ---" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BestFitnessAllTime_MetaData[] = {
		{ "Category", "Genetic Data" },
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetWorld_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --- Transient Data (Reset every run) ---\n" },
#endif
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "--- Transient Data (Reset every run) ---" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EnemySpawnPositions_MetaData[] = {
		{ "ModuleRelativePath", "Public/GeneticSimulationManager.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_GenerationCount;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BestFitnessAllTime;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TargetWorld;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EnemySpawnPositions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_EnemySpawnPositions;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGeneticSimulationManager_PlayerDiedListener, "PlayerDiedListener" }, // 3711682013
		{ &Z_Construct_UFunction_UGeneticSimulationManager_TimerCallback, "TimerCallback" }, // 250769484
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGeneticSimulationManager>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_GenerationCount = { "GenerationCount", nullptr, (EPropertyFlags)0x0010000000020001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGeneticSimulationManager, GenerationCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GenerationCount_MetaData), NewProp_GenerationCount_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_BestFitnessAllTime = { "BestFitnessAllTime", nullptr, (EPropertyFlags)0x0010000000020001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGeneticSimulationManager, BestFitnessAllTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BestFitnessAllTime_MetaData), NewProp_BestFitnessAllTime_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_TargetWorld = { "TargetWorld", nullptr, (EPropertyFlags)0x0020080000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGeneticSimulationManager, TargetWorld), Z_Construct_UClass_UWorld_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetWorld_MetaData), NewProp_TargetWorld_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_EnemySpawnPositions_Inner = { "EnemySpawnPositions", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_EnemySpawnPositions = { "EnemySpawnPositions", nullptr, (EPropertyFlags)0x0020080000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGeneticSimulationManager, EnemySpawnPositions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EnemySpawnPositions_MetaData), NewProp_EnemySpawnPositions_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGeneticSimulationManager_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_GenerationCount,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_BestFitnessAllTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_TargetWorld,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_EnemySpawnPositions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGeneticSimulationManager_Statics::NewProp_EnemySpawnPositions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGeneticSimulationManager_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UGeneticSimulationManager_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGeneticSimulationManager_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGeneticSimulationManager_Statics::ClassParams = {
	&UGeneticSimulationManager::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UGeneticSimulationManager_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UGeneticSimulationManager_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGeneticSimulationManager_Statics::Class_MetaDataParams), Z_Construct_UClass_UGeneticSimulationManager_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGeneticSimulationManager()
{
	if (!Z_Registration_Info_UClass_UGeneticSimulationManager.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGeneticSimulationManager.OuterSingleton, Z_Construct_UClass_UGeneticSimulationManager_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGeneticSimulationManager.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGeneticSimulationManager);
UGeneticSimulationManager::~UGeneticSimulationManager() {}
// ********** End Class UGeneticSimulationManager **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneticSimulationManager_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGeneticSimulationManager, UGeneticSimulationManager::StaticClass, TEXT("UGeneticSimulationManager"), &Z_Registration_Info_UClass_UGeneticSimulationManager, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGeneticSimulationManager), 1241180409U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneticSimulationManager_h__Script_GeneticGeneration_1545242584(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneticSimulationManager_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneticSimulationManager_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
