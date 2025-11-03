// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeneratedPawn.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGeneratedPawn() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_APawn();
GENETICGENERATION_API UClass* Z_Construct_UClass_AGeneratedPawn();
GENETICGENERATION_API UClass* Z_Construct_UClass_AGeneratedPawn_NoRegister();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Class AGeneratedPawn ***********************************************************
void AGeneratedPawn::StaticRegisterNativesAGeneratedPawn()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_AGeneratedPawn;
UClass* AGeneratedPawn::GetPrivateStaticClass()
{
	using TClass = AGeneratedPawn;
	if (!Z_Registration_Info_UClass_AGeneratedPawn.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("GeneratedPawn"),
			Z_Registration_Info_UClass_AGeneratedPawn.InnerSingleton,
			StaticRegisterNativesAGeneratedPawn,
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
	return Z_Registration_Info_UClass_AGeneratedPawn.InnerSingleton;
}
UClass* Z_Construct_UClass_AGeneratedPawn_NoRegister()
{
	return AGeneratedPawn::GetPrivateStaticClass();
}
struct Z_Construct_UClass_AGeneratedPawn_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "GeneratedPawn.h" },
		{ "ModuleRelativePath", "Public/GeneratedPawn.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGeneratedPawn>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AGeneratedPawn_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APawn,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGeneratedPawn_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AGeneratedPawn_Statics::ClassParams = {
	&AGeneratedPawn::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009001A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AGeneratedPawn_Statics::Class_MetaDataParams), Z_Construct_UClass_AGeneratedPawn_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AGeneratedPawn()
{
	if (!Z_Registration_Info_UClass_AGeneratedPawn.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AGeneratedPawn.OuterSingleton, Z_Construct_UClass_AGeneratedPawn_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AGeneratedPawn.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AGeneratedPawn);
AGeneratedPawn::~AGeneratedPawn() {}
// ********** End Class AGeneratedPawn *************************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedPawn_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AGeneratedPawn, AGeneratedPawn::StaticClass, TEXT("AGeneratedPawn"), &Z_Registration_Info_UClass_AGeneratedPawn, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AGeneratedPawn), 3539269238U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedPawn_h__Script_GeneticGeneration_2988480140(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedPawn_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedPawn_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
