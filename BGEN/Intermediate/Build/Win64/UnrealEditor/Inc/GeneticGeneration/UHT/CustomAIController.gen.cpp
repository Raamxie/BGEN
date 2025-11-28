// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "CustomAIController.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeCustomAIController() {}

// ********** Begin Cross Module References ********************************************************
AIMODULE_API UClass* Z_Construct_UClass_AAIController();
AIMODULE_API UClass* Z_Construct_UClass_UBehaviorTree_NoRegister();
AIMODULE_API UClass* Z_Construct_UClass_UBehaviorTreeComponent_NoRegister();
AIMODULE_API UClass* Z_Construct_UClass_UBlackboardComponent_NoRegister();
GENETICGENERATION_API UClass* Z_Construct_UClass_ACustomAIController();
GENETICGENERATION_API UClass* Z_Construct_UClass_ACustomAIController_NoRegister();
GENETICGENERATION_API UClass* Z_Construct_UClass_UCustomBehaviourTree_NoRegister();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ACustomAIController ******************************************************
void ACustomAIController::StaticRegisterNativesACustomAIController()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_ACustomAIController;
UClass* ACustomAIController::GetPrivateStaticClass()
{
	using TClass = ACustomAIController;
	if (!Z_Registration_Info_UClass_ACustomAIController.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("CustomAIController"),
			Z_Registration_Info_UClass_ACustomAIController.InnerSingleton,
			StaticRegisterNativesACustomAIController,
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
	return Z_Registration_Info_UClass_ACustomAIController.InnerSingleton;
}
UClass* Z_Construct_UClass_ACustomAIController_NoRegister()
{
	return ACustomAIController::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ACustomAIController_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Custom AI Controller that supports runtime-generated behavior trees\n */" },
#endif
		{ "HideCategories", "Collision Rendering Transformation" },
		{ "IncludePath", "CustomAIController.h" },
		{ "ModuleRelativePath", "Public/CustomAIController.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Custom AI Controller that supports runtime-generated behavior trees" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RuntimeBehaviourWrapper_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** A runtime helper object that builds/starts behavior trees */" },
#endif
		{ "ModuleRelativePath", "Public/CustomAIController.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A runtime helper object that builds/starts behavior trees" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlackboardComp_MetaData[] = {
		{ "Category", "AI" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Subobjects created in constructor */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/CustomAIController.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Subobjects created in constructor" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BehaviorComp_MetaData[] = {
		{ "Category", "AI" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/CustomAIController.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssignedTree_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Stored tree assigned via AssignTree */" },
#endif
		{ "ModuleRelativePath", "Public/CustomAIController.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Stored tree assigned via AssignTree" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RuntimeBehaviourWrapper;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BlackboardComp;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BehaviorComp;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AssignedTree;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ACustomAIController>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ACustomAIController_Statics::NewProp_RuntimeBehaviourWrapper = { "RuntimeBehaviourWrapper", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ACustomAIController, RuntimeBehaviourWrapper), Z_Construct_UClass_UCustomBehaviourTree_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RuntimeBehaviourWrapper_MetaData), NewProp_RuntimeBehaviourWrapper_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ACustomAIController_Statics::NewProp_BlackboardComp = { "BlackboardComp", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ACustomAIController, BlackboardComp), Z_Construct_UClass_UBlackboardComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlackboardComp_MetaData), NewProp_BlackboardComp_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ACustomAIController_Statics::NewProp_BehaviorComp = { "BehaviorComp", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ACustomAIController, BehaviorComp), Z_Construct_UClass_UBehaviorTreeComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BehaviorComp_MetaData), NewProp_BehaviorComp_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ACustomAIController_Statics::NewProp_AssignedTree = { "AssignedTree", nullptr, (EPropertyFlags)0x0020080000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ACustomAIController, AssignedTree), Z_Construct_UClass_UBehaviorTree_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssignedTree_MetaData), NewProp_AssignedTree_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ACustomAIController_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ACustomAIController_Statics::NewProp_RuntimeBehaviourWrapper,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ACustomAIController_Statics::NewProp_BlackboardComp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ACustomAIController_Statics::NewProp_BehaviorComp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ACustomAIController_Statics::NewProp_AssignedTree,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ACustomAIController_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ACustomAIController_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AAIController,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ACustomAIController_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ACustomAIController_Statics::ClassParams = {
	&ACustomAIController::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ACustomAIController_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ACustomAIController_Statics::PropPointers),
	0,
	0x009003A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ACustomAIController_Statics::Class_MetaDataParams), Z_Construct_UClass_ACustomAIController_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ACustomAIController()
{
	if (!Z_Registration_Info_UClass_ACustomAIController.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ACustomAIController.OuterSingleton, Z_Construct_UClass_ACustomAIController_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ACustomAIController.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ACustomAIController);
ACustomAIController::~ACustomAIController() {}
// ********** End Class ACustomAIController ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ACustomAIController, ACustomAIController::StaticClass, TEXT("ACustomAIController"), &Z_Registration_Info_UClass_ACustomAIController, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ACustomAIController), 2129715946U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h__Script_GeneticGeneration_1699844594(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
