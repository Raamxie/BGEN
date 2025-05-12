// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BGEN/AIAssets/BTNodes/WalkToZero.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWalkToZero() {}

// Begin Cross Module References
AIMODULE_API UClass* Z_Construct_UClass_UBTTaskNode();
BGEN_API UClass* Z_Construct_UClass_UWalkToZero();
BGEN_API UClass* Z_Construct_UClass_UWalkToZero_NoRegister();
UPackage* Z_Construct_UPackage__Script_BGEN();
// End Cross Module References

// Begin Class UWalkToZero
void UWalkToZero::StaticRegisterNativesUWalkToZero()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UWalkToZero);
UClass* Z_Construct_UClass_UWalkToZero_NoRegister()
{
	return UWalkToZero::StaticClass();
}
struct Z_Construct_UClass_UWalkToZero_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "IncludePath", "AIAssets/BTNodes/WalkToZero.h" },
		{ "ModuleRelativePath", "AIAssets/BTNodes/WalkToZero.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWalkToZero>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UWalkToZero_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBTTaskNode,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UWalkToZero_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UWalkToZero_Statics::ClassParams = {
	&UWalkToZero::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UWalkToZero_Statics::Class_MetaDataParams), Z_Construct_UClass_UWalkToZero_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UWalkToZero()
{
	if (!Z_Registration_Info_UClass_UWalkToZero.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UWalkToZero.OuterSingleton, Z_Construct_UClass_UWalkToZero_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UWalkToZero.OuterSingleton;
}
template<> BGEN_API UClass* StaticClass<UWalkToZero>()
{
	return UWalkToZero::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UWalkToZero);
UWalkToZero::~UWalkToZero() {}
// End Class UWalkToZero

// Begin Registration
struct Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_BTNodes_WalkToZero_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UWalkToZero, UWalkToZero::StaticClass, TEXT("UWalkToZero"), &Z_Registration_Info_UClass_UWalkToZero, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UWalkToZero), 1091096379U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_BTNodes_WalkToZero_h_4223477745(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_BTNodes_WalkToZero_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_BTNodes_WalkToZero_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
