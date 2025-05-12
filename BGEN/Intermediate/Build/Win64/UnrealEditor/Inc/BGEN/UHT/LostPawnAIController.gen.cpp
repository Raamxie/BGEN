// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BGEN/AIAssets/LostPawnAIController.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeLostPawnAIController() {}

// Begin Cross Module References
AIMODULE_API UClass* Z_Construct_UClass_AAIController();
BGEN_API UClass* Z_Construct_UClass_ALostPawnAIController();
BGEN_API UClass* Z_Construct_UClass_ALostPawnAIController_NoRegister();
UPackage* Z_Construct_UPackage__Script_BGEN();
// End Cross Module References

// Begin Class ALostPawnAIController
void ALostPawnAIController::StaticRegisterNativesALostPawnAIController()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ALostPawnAIController);
UClass* Z_Construct_UClass_ALostPawnAIController_NoRegister()
{
	return ALostPawnAIController::StaticClass();
}
struct Z_Construct_UClass_ALostPawnAIController_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "HideCategories", "Collision Rendering Transformation" },
		{ "IncludePath", "AIAssets/LostPawnAIController.h" },
		{ "ModuleRelativePath", "AIAssets/LostPawnAIController.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ALostPawnAIController>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ALostPawnAIController_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AAIController,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawnAIController_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ALostPawnAIController_Statics::ClassParams = {
	&ALostPawnAIController::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009002A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ALostPawnAIController_Statics::Class_MetaDataParams), Z_Construct_UClass_ALostPawnAIController_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ALostPawnAIController()
{
	if (!Z_Registration_Info_UClass_ALostPawnAIController.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ALostPawnAIController.OuterSingleton, Z_Construct_UClass_ALostPawnAIController_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ALostPawnAIController.OuterSingleton;
}
template<> BGEN_API UClass* StaticClass<ALostPawnAIController>()
{
	return ALostPawnAIController::StaticClass();
}
ALostPawnAIController::ALostPawnAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(ALostPawnAIController);
ALostPawnAIController::~ALostPawnAIController() {}
// End Class ALostPawnAIController

// Begin Registration
struct Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnAIController_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ALostPawnAIController, ALostPawnAIController::StaticClass, TEXT("ALostPawnAIController"), &Z_Registration_Info_UClass_ALostPawnAIController, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ALostPawnAIController), 2525648027U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnAIController_h_3181226217(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnAIController_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnAIController_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
