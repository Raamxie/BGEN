// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeneratedCharacter.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGeneratedCharacter() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
GENETICGENERATION_API UClass* Z_Construct_UClass_AGeneratedCharacter();
GENETICGENERATION_API UClass* Z_Construct_UClass_AGeneratedCharacter_NoRegister();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Class AGeneratedCharacter Function InitializeDelayed ***************************
struct Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GeneratedCharacter.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_AGeneratedCharacter, nullptr, "InitializeDelayed", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed_Statics::Function_MetaDataParams), Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AGeneratedCharacter::execInitializeDelayed)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->InitializeDelayed();
	P_NATIVE_END;
}
// ********** End Class AGeneratedCharacter Function InitializeDelayed *****************************

// ********** Begin Class AGeneratedCharacter ******************************************************
void AGeneratedCharacter::StaticRegisterNativesAGeneratedCharacter()
{
	UClass* Class = AGeneratedCharacter::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "InitializeDelayed", &AGeneratedCharacter::execInitializeDelayed },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_AGeneratedCharacter;
UClass* AGeneratedCharacter::GetPrivateStaticClass()
{
	using TClass = AGeneratedCharacter;
	if (!Z_Registration_Info_UClass_AGeneratedCharacter.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("GeneratedCharacter"),
			Z_Registration_Info_UClass_AGeneratedCharacter.InnerSingleton,
			StaticRegisterNativesAGeneratedCharacter,
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
	return Z_Registration_Info_UClass_AGeneratedCharacter.InnerSingleton;
}
UClass* Z_Construct_UClass_AGeneratedCharacter_NoRegister()
{
	return AGeneratedCharacter::GetPrivateStaticClass();
}
struct Z_Construct_UClass_AGeneratedCharacter_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "GeneratedCharacter.h" },
		{ "ModuleRelativePath", "Public/GeneratedCharacter.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AGeneratedCharacter_InitializeDelayed, "InitializeDelayed" }, // 2058860061
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGeneratedCharacter>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AGeneratedCharacter_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGeneratedCharacter_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AGeneratedCharacter_Statics::ClassParams = {
	&AGeneratedCharacter::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x009001A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AGeneratedCharacter_Statics::Class_MetaDataParams), Z_Construct_UClass_AGeneratedCharacter_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AGeneratedCharacter()
{
	if (!Z_Registration_Info_UClass_AGeneratedCharacter.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AGeneratedCharacter.OuterSingleton, Z_Construct_UClass_AGeneratedCharacter_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AGeneratedCharacter.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AGeneratedCharacter);
AGeneratedCharacter::~AGeneratedCharacter() {}
// ********** End Class AGeneratedCharacter ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedCharacter_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AGeneratedCharacter, AGeneratedCharacter::StaticClass, TEXT("AGeneratedCharacter"), &Z_Registration_Info_UClass_AGeneratedCharacter, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AGeneratedCharacter), 3095015862U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedCharacter_h__Script_GeneticGeneration_2228545131(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedCharacter_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_GeneratedCharacter_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
