// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "CustomBehaviourTree.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeCustomBehaviourTree() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GENETICGENERATION_API UClass* Z_Construct_UClass_UCustomBehaviourTree();
GENETICGENERATION_API UClass* Z_Construct_UClass_UCustomBehaviourTree_NoRegister();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UCustomBehaviourTree Function LoadBehaviorTree ***************************
struct Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics
{
	struct CustomBehaviourTree_eventLoadBehaviorTree_Parms
	{
		FString AssetPath;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "GeneticGeneration" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/// Load an existing BehaviorTree (e.g. \"/Game/AI/BT_Original\")\n" },
#endif
		{ "ModuleRelativePath", "Public/CustomBehaviourTree.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Load an existing BehaviorTree (e.g. \"/Game/AI/BT_Original\")" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssetPath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_AssetPath;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_AssetPath = { "AssetPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CustomBehaviourTree_eventLoadBehaviorTree_Parms, AssetPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssetPath_MetaData), NewProp_AssetPath_MetaData) };
void Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CustomBehaviourTree_eventLoadBehaviorTree_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CustomBehaviourTree_eventLoadBehaviorTree_Parms), &Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_AssetPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCustomBehaviourTree, nullptr, "LoadBehaviorTree", Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::CustomBehaviourTree_eventLoadBehaviorTree_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::CustomBehaviourTree_eventLoadBehaviorTree_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCustomBehaviourTree::execLoadBehaviorTree)
{
	P_GET_PROPERTY(FStrProperty,Z_Param_AssetPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->LoadBehaviorTree(Z_Param_AssetPath);
	P_NATIVE_END;
}
// ********** End Class UCustomBehaviourTree Function LoadBehaviorTree *****************************

// ********** Begin Class UCustomBehaviourTree Function MutateTree_Dynamic *************************
struct Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics
{
	struct CustomBehaviourTree_eventMutateTree_Dynamic_Parms
	{
		FString SearchPath;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "GeneticGeneration" },
		{ "CPP_Default_SearchPath", "/Game/BehaviourTrees/Tasks" },
		{ "ModuleRelativePath", "Public/CustomBehaviourTree.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SearchPath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_SearchPath;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_SearchPath = { "SearchPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CustomBehaviourTree_eventMutateTree_Dynamic_Parms, SearchPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SearchPath_MetaData), NewProp_SearchPath_MetaData) };
void Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CustomBehaviourTree_eventMutateTree_Dynamic_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CustomBehaviourTree_eventMutateTree_Dynamic_Parms), &Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_SearchPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCustomBehaviourTree, nullptr, "MutateTree_Dynamic", Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::CustomBehaviourTree_eventMutateTree_Dynamic_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::CustomBehaviourTree_eventMutateTree_Dynamic_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCustomBehaviourTree::execMutateTree_Dynamic)
{
	P_GET_PROPERTY(FStrProperty,Z_Param_SearchPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->MutateTree_Dynamic(Z_Param_SearchPath);
	P_NATIVE_END;
}
// ********** End Class UCustomBehaviourTree Function MutateTree_Dynamic ***************************

// ********** Begin Class UCustomBehaviourTree Function SaveAsNewAsset *****************************
struct Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics
{
	struct CustomBehaviourTree_eventSaveAsNewAsset_Parms
	{
		FString DestinationPackagePath;
		bool bOverwriteExisting;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "GeneticGeneration" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/// Save current tree into a new package: DestinationPackagePath is like \"/Game/Generated/MyEvolvedBT\"\n" },
#endif
		{ "CPP_Default_bOverwriteExisting", "false" },
		{ "ModuleRelativePath", "Public/CustomBehaviourTree.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Save current tree into a new package: DestinationPackagePath is like \"/Game/Generated/MyEvolvedBT\"" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DestinationPackagePath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_DestinationPackagePath;
	static void NewProp_bOverwriteExisting_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bOverwriteExisting;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_DestinationPackagePath = { "DestinationPackagePath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CustomBehaviourTree_eventSaveAsNewAsset_Parms, DestinationPackagePath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DestinationPackagePath_MetaData), NewProp_DestinationPackagePath_MetaData) };
void Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_bOverwriteExisting_SetBit(void* Obj)
{
	((CustomBehaviourTree_eventSaveAsNewAsset_Parms*)Obj)->bOverwriteExisting = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_bOverwriteExisting = { "bOverwriteExisting", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CustomBehaviourTree_eventSaveAsNewAsset_Parms), &Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_bOverwriteExisting_SetBit, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CustomBehaviourTree_eventSaveAsNewAsset_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CustomBehaviourTree_eventSaveAsNewAsset_Parms), &Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_DestinationPackagePath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_bOverwriteExisting,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCustomBehaviourTree, nullptr, "SaveAsNewAsset", Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::CustomBehaviourTree_eventSaveAsNewAsset_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::CustomBehaviourTree_eventSaveAsNewAsset_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCustomBehaviourTree::execSaveAsNewAsset)
{
	P_GET_PROPERTY(FStrProperty,Z_Param_DestinationPackagePath);
	P_GET_UBOOL(Z_Param_bOverwriteExisting);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->SaveAsNewAsset(Z_Param_DestinationPackagePath,Z_Param_bOverwriteExisting);
	P_NATIVE_END;
}
// ********** End Class UCustomBehaviourTree Function SaveAsNewAsset *******************************

// ********** Begin Class UCustomBehaviourTree *****************************************************
void UCustomBehaviourTree::StaticRegisterNativesUCustomBehaviourTree()
{
	UClass* Class = UCustomBehaviourTree::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "LoadBehaviorTree", &UCustomBehaviourTree::execLoadBehaviorTree },
		{ "MutateTree_Dynamic", &UCustomBehaviourTree::execMutateTree_Dynamic },
		{ "SaveAsNewAsset", &UCustomBehaviourTree::execSaveAsNewAsset },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UCustomBehaviourTree;
UClass* UCustomBehaviourTree::GetPrivateStaticClass()
{
	using TClass = UCustomBehaviourTree;
	if (!Z_Registration_Info_UClass_UCustomBehaviourTree.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("CustomBehaviourTree"),
			Z_Registration_Info_UClass_UCustomBehaviourTree.InnerSingleton,
			StaticRegisterNativesUCustomBehaviourTree,
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
	return Z_Registration_Info_UClass_UCustomBehaviourTree.InnerSingleton;
}
UClass* Z_Construct_UClass_UCustomBehaviourTree_NoRegister()
{
	return UCustomBehaviourTree::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UCustomBehaviourTree_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Runtime helper to load, mutate and save a Behavior Tree.\n */" },
#endif
		{ "IncludePath", "CustomBehaviourTree.h" },
		{ "ModuleRelativePath", "Public/CustomBehaviourTree.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Runtime helper to load, mutate and save a Behavior Tree." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UCustomBehaviourTree_LoadBehaviorTree, "LoadBehaviorTree" }, // 2916374827
		{ &Z_Construct_UFunction_UCustomBehaviourTree_MutateTree_Dynamic, "MutateTree_Dynamic" }, // 3330002182
		{ &Z_Construct_UFunction_UCustomBehaviourTree_SaveAsNewAsset, "SaveAsNewAsset" }, // 860733668
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCustomBehaviourTree>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCustomBehaviourTree_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCustomBehaviourTree_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCustomBehaviourTree_Statics::ClassParams = {
	&UCustomBehaviourTree::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCustomBehaviourTree_Statics::Class_MetaDataParams), Z_Construct_UClass_UCustomBehaviourTree_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCustomBehaviourTree()
{
	if (!Z_Registration_Info_UClass_UCustomBehaviourTree.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCustomBehaviourTree.OuterSingleton, Z_Construct_UClass_UCustomBehaviourTree_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCustomBehaviourTree.OuterSingleton;
}
UCustomBehaviourTree::UCustomBehaviourTree(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCustomBehaviourTree);
UCustomBehaviourTree::~UCustomBehaviourTree() {}
// ********** End Class UCustomBehaviourTree *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCustomBehaviourTree, UCustomBehaviourTree::StaticClass, TEXT("UCustomBehaviourTree"), &Z_Registration_Info_UClass_UCustomBehaviourTree, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCustomBehaviourTree), 3494348665U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h__Script_GeneticGeneration_2723908859(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
