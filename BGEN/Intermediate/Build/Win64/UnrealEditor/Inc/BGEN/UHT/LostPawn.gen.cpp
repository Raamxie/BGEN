// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BGEN/Actors/LostPawn.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeLostPawn() {}

// Begin Cross Module References
BGEN_API UClass* Z_Construct_UClass_AEvolutionManager_NoRegister();
BGEN_API UClass* Z_Construct_UClass_ALostPawn();
BGEN_API UClass* Z_Construct_UClass_ALostPawn_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_APawn();
ENGINE_API UClass* Z_Construct_UClass_USceneComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_BGEN();
// End Cross Module References

// Begin Class ALostPawn
void ALostPawn::StaticRegisterNativesALostPawn()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ALostPawn);
UClass* Z_Construct_UClass_ALostPawn_NoRegister()
{
	return ALostPawn::StaticClass();
}
struct Z_Construct_UClass_ALostPawn_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Actors/LostPawn.h" },
		{ "ModuleRelativePath", "Actors/LostPawn.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EvolutionManager_MetaData[] = {
		{ "ModuleRelativePath", "Actors/LostPawn.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SceneRoot_MetaData[] = {
		{ "Category", "Components" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** The root transform component */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Actors/LostPawn.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The root transform component" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MeshComp_MetaData[] = {
		{ "Category", "Components" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** A visible mesh so you can see your pawn move */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Actors/LostPawn.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A visible mesh so you can see your pawn move" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_EvolutionManager;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_SceneRoot;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MeshComp;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ALostPawn>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ALostPawn_Statics::NewProp_EvolutionManager = { "EvolutionManager", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ALostPawn, EvolutionManager), Z_Construct_UClass_AEvolutionManager_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EvolutionManager_MetaData), NewProp_EvolutionManager_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ALostPawn_Statics::NewProp_SceneRoot = { "SceneRoot", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ALostPawn, SceneRoot), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SceneRoot_MetaData), NewProp_SceneRoot_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ALostPawn_Statics::NewProp_MeshComp = { "MeshComp", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ALostPawn, MeshComp), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MeshComp_MetaData), NewProp_MeshComp_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ALostPawn_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ALostPawn_Statics::NewProp_EvolutionManager,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ALostPawn_Statics::NewProp_SceneRoot,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ALostPawn_Statics::NewProp_MeshComp,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawn_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ALostPawn_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APawn,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawn_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ALostPawn_Statics::ClassParams = {
	&ALostPawn::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ALostPawn_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawn_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawn_Statics::Class_MetaDataParams), Z_Construct_UClass_ALostPawn_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ALostPawn()
{
	if (!Z_Registration_Info_UClass_ALostPawn.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ALostPawn.OuterSingleton, Z_Construct_UClass_ALostPawn_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ALostPawn.OuterSingleton;
}
template<> BGEN_API UClass* StaticClass<ALostPawn>()
{
	return ALostPawn::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ALostPawn);
ALostPawn::~ALostPawn() {}
// End Class ALostPawn

// Begin Registration
struct Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ALostPawn, ALostPawn::StaticClass, TEXT("ALostPawn"), &Z_Registration_Info_UClass_ALostPawn, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ALostPawn), 3503295850U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_1679565957(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
