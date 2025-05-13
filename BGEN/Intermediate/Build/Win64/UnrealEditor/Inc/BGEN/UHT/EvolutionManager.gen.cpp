// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BGEN/Actors/EvolutionManager.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeEvolutionManager() {}

// Begin Cross Module References
BGEN_API UClass* Z_Construct_UClass_AEvolutionManager();
BGEN_API UClass* Z_Construct_UClass_AEvolutionManager_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
UPackage* Z_Construct_UPackage__Script_BGEN();
// End Cross Module References

// Begin Class AEvolutionManager
void AEvolutionManager::StaticRegisterNativesAEvolutionManager()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AEvolutionManager);
UClass* Z_Construct_UClass_AEvolutionManager_NoRegister()
{
	return AEvolutionManager::StaticClass();
}
struct Z_Construct_UClass_AEvolutionManager_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Actors/EvolutionManager.h" },
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpawnedActors_MetaData[] = {
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpawnCount_MetaData[] = {
		{ "Category", "Simulation" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Category: Simulation\n" },
#endif
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Category: Simulation" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SimulationSpeedMultiplier_MetaData[] = {
		{ "Category", "Simulation" },
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TickSpeed_MetaData[] = {
		{ "Category", "Simulation" },
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DistanceFromCenter_MetaData[] = {
		{ "Category", "Placement" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Category: Placement\n" },
#endif
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Category: Placement" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AcceptanceRange_MetaData[] = {
		{ "Category", "Placement" },
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PawnMesh_MetaData[] = {
		{ "Category", "Visual" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Category: Visual\n" },
#endif
		{ "ModuleRelativePath", "Actors/EvolutionManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Category: Visual" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_SpawnedActors_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_SpawnedActors;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SpawnCount;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SimulationSpeedMultiplier;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_TickSpeed;
	static const UECodeGen_Private::FIntPropertyParams NewProp_DistanceFromCenter;
	static const UECodeGen_Private::FIntPropertyParams NewProp_AcceptanceRange;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PawnMesh;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AEvolutionManager>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnedActors_Inner = { "SpawnedActors", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnedActors = { "SpawnedActors", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, SpawnedActors), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpawnedActors_MetaData), NewProp_SpawnedActors_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnCount = { "SpawnCount", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, SpawnCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpawnCount_MetaData), NewProp_SpawnCount_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SimulationSpeedMultiplier = { "SimulationSpeedMultiplier", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, SimulationSpeedMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SimulationSpeedMultiplier_MetaData), NewProp_SimulationSpeedMultiplier_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_TickSpeed = { "TickSpeed", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, TickSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TickSpeed_MetaData), NewProp_TickSpeed_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_DistanceFromCenter = { "DistanceFromCenter", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, DistanceFromCenter), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DistanceFromCenter_MetaData), NewProp_DistanceFromCenter_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_AcceptanceRange = { "AcceptanceRange", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, AcceptanceRange), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AcceptanceRange_MetaData), NewProp_AcceptanceRange_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEvolutionManager_Statics::NewProp_PawnMesh = { "PawnMesh", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEvolutionManager, PawnMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PawnMesh_MetaData), NewProp_PawnMesh_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AEvolutionManager_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnedActors_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnedActors,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SpawnCount,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_SimulationSpeedMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_TickSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_DistanceFromCenter,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_AcceptanceRange,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEvolutionManager_Statics::NewProp_PawnMesh,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AEvolutionManager_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AEvolutionManager_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AEvolutionManager_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AEvolutionManager_Statics::ClassParams = {
	&AEvolutionManager::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AEvolutionManager_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AEvolutionManager_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AEvolutionManager_Statics::Class_MetaDataParams), Z_Construct_UClass_AEvolutionManager_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AEvolutionManager()
{
	if (!Z_Registration_Info_UClass_AEvolutionManager.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AEvolutionManager.OuterSingleton, Z_Construct_UClass_AEvolutionManager_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AEvolutionManager.OuterSingleton;
}
template<> BGEN_API UClass* StaticClass<AEvolutionManager>()
{
	return AEvolutionManager::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AEvolutionManager);
AEvolutionManager::~AEvolutionManager() {}
// End Class AEvolutionManager

// Begin Registration
struct Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_EvolutionManager_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AEvolutionManager, AEvolutionManager::StaticClass, TEXT("AEvolutionManager"), &Z_Registration_Info_UClass_AEvolutionManager, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AEvolutionManager), 228607279U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_EvolutionManager_h_1621159915(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_EvolutionManager_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_EvolutionManager_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
