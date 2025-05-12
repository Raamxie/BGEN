// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BGEN/AIAssets/LostPawnDefaultBehaviourTree.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeLostPawnDefaultBehaviourTree() {}

// Begin Cross Module References
AIMODULE_API UClass* Z_Construct_UClass_UBehaviorTree();
BGEN_API UClass* Z_Construct_UClass_ULostPawnDefaultBehaviourTree();
BGEN_API UClass* Z_Construct_UClass_ULostPawnDefaultBehaviourTree_NoRegister();
UPackage* Z_Construct_UPackage__Script_BGEN();
// End Cross Module References

// Begin Class ULostPawnDefaultBehaviourTree
void ULostPawnDefaultBehaviourTree::StaticRegisterNativesULostPawnDefaultBehaviourTree()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ULostPawnDefaultBehaviourTree);
UClass* Z_Construct_UClass_ULostPawnDefaultBehaviourTree_NoRegister()
{
	return ULostPawnDefaultBehaviourTree::StaticClass();
}
struct Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "IncludePath", "AIAssets/LostPawnDefaultBehaviourTree.h" },
		{ "ModuleRelativePath", "AIAssets/LostPawnDefaultBehaviourTree.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ULostPawnDefaultBehaviourTree>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBehaviorTree,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::ClassParams = {
	&ULostPawnDefaultBehaviourTree::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::Class_MetaDataParams), Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ULostPawnDefaultBehaviourTree()
{
	if (!Z_Registration_Info_UClass_ULostPawnDefaultBehaviourTree.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ULostPawnDefaultBehaviourTree.OuterSingleton, Z_Construct_UClass_ULostPawnDefaultBehaviourTree_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ULostPawnDefaultBehaviourTree.OuterSingleton;
}
template<> BGEN_API UClass* StaticClass<ULostPawnDefaultBehaviourTree>()
{
	return ULostPawnDefaultBehaviourTree::StaticClass();
}
ULostPawnDefaultBehaviourTree::ULostPawnDefaultBehaviourTree(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(ULostPawnDefaultBehaviourTree);
ULostPawnDefaultBehaviourTree::~ULostPawnDefaultBehaviourTree() {}
// End Class ULostPawnDefaultBehaviourTree

// Begin Registration
struct Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnDefaultBehaviourTree_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ULostPawnDefaultBehaviourTree, ULostPawnDefaultBehaviourTree::StaticClass, TEXT("ULostPawnDefaultBehaviourTree"), &Z_Registration_Info_UClass_ULostPawnDefaultBehaviourTree, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ULostPawnDefaultBehaviourTree), 3135806821U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnDefaultBehaviourTree_h_3613207149(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnDefaultBehaviourTree_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_School_BGEN_BGEN_Source_BGEN_AIAssets_LostPawnDefaultBehaviourTree_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
