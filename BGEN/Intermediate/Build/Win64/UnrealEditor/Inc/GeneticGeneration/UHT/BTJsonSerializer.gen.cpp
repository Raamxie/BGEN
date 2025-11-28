// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BTJsonSerializer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeBTJsonSerializer() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GENETICGENERATION_API UClass* Z_Construct_UClass_UBTJsonSerializer();
GENETICGENERATION_API UClass* Z_Construct_UClass_UBTJsonSerializer_NoRegister();
GENETICGENERATION_API UScriptStruct* Z_Construct_UScriptStruct_FBTNodeJSON();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FBTNodeJSON *******************************************************
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBTNodeJSON;
class UScriptStruct* FBTNodeJSON::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FBTNodeJSON.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FBTNodeJSON.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBTNodeJSON, (UObject*)Z_Construct_UPackage__Script_GeneticGeneration(), TEXT("BTNodeJSON"));
	}
	return Z_Registration_Info_UScriptStruct_FBTNodeJSON.OuterSingleton;
}
struct Z_Construct_UScriptStruct_FBTNodeJSON_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/BTJsonSerializer.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NodeClass_MetaData[] = {
		{ "ModuleRelativePath", "Public/BTJsonSerializer.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NodeName_MetaData[] = {
		{ "ModuleRelativePath", "Public/BTJsonSerializer.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Properties_MetaData[] = {
		{ "ModuleRelativePath", "Public/BTJsonSerializer.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_NodeClass;
	static const UECodeGen_Private::FStrPropertyParams NewProp_NodeName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Properties_ValueProp;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Properties_Key_KeyProp;
	static const UECodeGen_Private::FMapPropertyParams NewProp_Properties;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBTNodeJSON>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_NodeClass = { "NodeClass", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBTNodeJSON, NodeClass), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NodeClass_MetaData), NewProp_NodeClass_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_NodeName = { "NodeName", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBTNodeJSON, NodeName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NodeName_MetaData), NewProp_NodeName_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties_ValueProp = { "Properties", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 1, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties_Key_KeyProp = { "Properties_Key", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FMapPropertyParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties = { "Properties", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBTNodeJSON, Properties), EMapPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Properties_MetaData), NewProp_Properties_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FBTNodeJSON_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_NodeClass,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_NodeName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties_ValueProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties_Key_KeyProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewProp_Properties,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBTNodeJSON_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FBTNodeJSON_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
	nullptr,
	&NewStructOps,
	"BTNodeJSON",
	Z_Construct_UScriptStruct_FBTNodeJSON_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBTNodeJSON_Statics::PropPointers),
	sizeof(FBTNodeJSON),
	alignof(FBTNodeJSON),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBTNodeJSON_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FBTNodeJSON_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FBTNodeJSON()
{
	if (!Z_Registration_Info_UScriptStruct_FBTNodeJSON.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBTNodeJSON.InnerSingleton, Z_Construct_UScriptStruct_FBTNodeJSON_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_FBTNodeJSON.InnerSingleton;
}
// ********** End ScriptStruct FBTNodeJSON *********************************************************

// ********** Begin Class UBTJsonSerializer ********************************************************
void UBTJsonSerializer::StaticRegisterNativesUBTJsonSerializer()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UBTJsonSerializer;
UClass* UBTJsonSerializer::GetPrivateStaticClass()
{
	using TClass = UBTJsonSerializer;
	if (!Z_Registration_Info_UClass_UBTJsonSerializer.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("BTJsonSerializer"),
			Z_Registration_Info_UClass_UBTJsonSerializer.InnerSingleton,
			StaticRegisterNativesUBTJsonSerializer,
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
	return Z_Registration_Info_UClass_UBTJsonSerializer.InnerSingleton;
}
UClass* Z_Construct_UClass_UBTJsonSerializer_NoRegister()
{
	return UBTJsonSerializer::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBTJsonSerializer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "BTJsonSerializer.h" },
		{ "ModuleRelativePath", "Public/BTJsonSerializer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBTJsonSerializer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UBTJsonSerializer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBTJsonSerializer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBTJsonSerializer_Statics::ClassParams = {
	&UBTJsonSerializer::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBTJsonSerializer_Statics::Class_MetaDataParams), Z_Construct_UClass_UBTJsonSerializer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UBTJsonSerializer()
{
	if (!Z_Registration_Info_UClass_UBTJsonSerializer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBTJsonSerializer.OuterSingleton, Z_Construct_UClass_UBTJsonSerializer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBTJsonSerializer.OuterSingleton;
}
UBTJsonSerializer::UBTJsonSerializer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UBTJsonSerializer);
UBTJsonSerializer::~UBTJsonSerializer() {}
// ********** End Class UBTJsonSerializer **********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FBTNodeJSON::StaticStruct, Z_Construct_UScriptStruct_FBTNodeJSON_Statics::NewStructOps, TEXT("BTNodeJSON"), &Z_Registration_Info_UScriptStruct_FBTNodeJSON, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBTNodeJSON), 2795968747U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBTJsonSerializer, UBTJsonSerializer::StaticClass, TEXT("UBTJsonSerializer"), &Z_Registration_Info_UClass_UBTJsonSerializer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBTJsonSerializer), 501398300U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_2636736219(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h__Script_GeneticGeneration_Statics::ScriptStructInfo),
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
