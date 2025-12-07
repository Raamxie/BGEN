// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "PlayerUnleashedBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodePlayerUnleashedBase() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
GENETICGENERATION_API UClass* Z_Construct_UClass_APlayerUnleashedBase();
GENETICGENERATION_API UClass* Z_Construct_UClass_APlayerUnleashedBase_NoRegister();
GENETICGENERATION_API UFunction* Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature();
UPackage* Z_Construct_UPackage__Script_GeneticGeneration();
// ********** End Cross Module References **********************************************************

// ********** Begin Delegate FOnPlayerEvent ********************************************************
struct Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/PlayerUnleashedBase.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FDelegateFunctionParams FuncParams;
};
const UECodeGen_Private::FDelegateFunctionParams Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UPackage__Script_GeneticGeneration, nullptr, "OnPlayerEvent__DelegateSignature", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature_Statics::Function_MetaDataParams), Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUDelegateFunction(&ReturnFunction, Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature_Statics::FuncParams);
	}
	return ReturnFunction;
}
void FOnPlayerEvent_DelegateWrapper(const FMulticastScriptDelegate& OnPlayerEvent)
{
	OnPlayerEvent.ProcessMulticastDelegate<UObject>(NULL);
}
// ********** End Delegate FOnPlayerEvent **********************************************************

// ********** Begin Class APlayerUnleashedBase Function BroadcastPlayerDeath ***********************
struct Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "GeneticGeneration" },
		{ "ModuleRelativePath", "Public/PlayerUnleashedBase.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_APlayerUnleashedBase, nullptr, "BroadcastPlayerDeath", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath_Statics::Function_MetaDataParams), Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(APlayerUnleashedBase::execBroadcastPlayerDeath)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->BroadcastPlayerDeath();
	P_NATIVE_END;
}
// ********** End Class APlayerUnleashedBase Function BroadcastPlayerDeath *************************

// ********** Begin Class APlayerUnleashedBase Function LogDamageReceived **************************
struct Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics
{
	struct PlayerUnleashedBase_eventLogDamageReceived_Parms
	{
		float DamageAmount;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "GeneticGeneration" },
		{ "ModuleRelativePath", "Public/PlayerUnleashedBase.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageAmount;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::NewProp_DamageAmount = { "DamageAmount", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PlayerUnleashedBase_eventLogDamageReceived_Parms, DamageAmount), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::NewProp_DamageAmount,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_APlayerUnleashedBase, nullptr, "LogDamageReceived", Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PropPointers), sizeof(Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PlayerUnleashedBase_eventLogDamageReceived_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::Function_MetaDataParams), Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::PlayerUnleashedBase_eventLogDamageReceived_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(APlayerUnleashedBase::execLogDamageReceived)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_DamageAmount);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->LogDamageReceived(Z_Param_DamageAmount);
	P_NATIVE_END;
}
// ********** End Class APlayerUnleashedBase Function LogDamageReceived ****************************

// ********** Begin Class APlayerUnleashedBase *****************************************************
void APlayerUnleashedBase::StaticRegisterNativesAPlayerUnleashedBase()
{
	UClass* Class = APlayerUnleashedBase::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "BroadcastPlayerDeath", &APlayerUnleashedBase::execBroadcastPlayerDeath },
		{ "LogDamageReceived", &APlayerUnleashedBase::execLogDamageReceived },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_APlayerUnleashedBase;
UClass* APlayerUnleashedBase::GetPrivateStaticClass()
{
	using TClass = APlayerUnleashedBase;
	if (!Z_Registration_Info_UClass_APlayerUnleashedBase.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("PlayerUnleashedBase"),
			Z_Registration_Info_UClass_APlayerUnleashedBase.InnerSingleton,
			StaticRegisterNativesAPlayerUnleashedBase,
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
	return Z_Registration_Info_UClass_APlayerUnleashedBase.InnerSingleton;
}
UClass* Z_Construct_UClass_APlayerUnleashedBase_NoRegister()
{
	return APlayerUnleashedBase::GetPrivateStaticClass();
}
struct Z_Construct_UClass_APlayerUnleashedBase_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "PlayerUnleashedBase.h" },
		{ "ModuleRelativePath", "Public/PlayerUnleashedBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OnPlayerEvent_MetaData[] = {
		{ "Category", "Events" },
		{ "ModuleRelativePath", "Public/PlayerUnleashedBase.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnPlayerEvent;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_APlayerUnleashedBase_BroadcastPlayerDeath, "BroadcastPlayerDeath" }, // 2338565102
		{ &Z_Construct_UFunction_APlayerUnleashedBase_LogDamageReceived, "LogDamageReceived" }, // 215921762
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APlayerUnleashedBase>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_APlayerUnleashedBase_Statics::NewProp_OnPlayerEvent = { "OnPlayerEvent", nullptr, (EPropertyFlags)0x0010000010080000, UECodeGen_Private::EPropertyGenFlags::InlineMulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerUnleashedBase, OnPlayerEvent), Z_Construct_UDelegateFunction_GeneticGeneration_OnPlayerEvent__DelegateSignature, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OnPlayerEvent_MetaData), NewProp_OnPlayerEvent_MetaData) }; // 1968348882
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APlayerUnleashedBase_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerUnleashedBase_Statics::NewProp_OnPlayerEvent,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerUnleashedBase_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_APlayerUnleashedBase_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_GeneticGeneration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerUnleashedBase_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_APlayerUnleashedBase_Statics::ClassParams = {
	&APlayerUnleashedBase::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_APlayerUnleashedBase_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_APlayerUnleashedBase_Statics::PropPointers),
	0,
	0x009001A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerUnleashedBase_Statics::Class_MetaDataParams), Z_Construct_UClass_APlayerUnleashedBase_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_APlayerUnleashedBase()
{
	if (!Z_Registration_Info_UClass_APlayerUnleashedBase.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_APlayerUnleashedBase.OuterSingleton, Z_Construct_UClass_APlayerUnleashedBase_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_APlayerUnleashedBase.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(APlayerUnleashedBase);
APlayerUnleashedBase::~APlayerUnleashedBase() {}
// ********** End Class APlayerUnleashedBase *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h__Script_GeneticGeneration_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_APlayerUnleashedBase, APlayerUnleashedBase::StaticClass, TEXT("APlayerUnleashedBase"), &Z_Registration_Info_UClass_APlayerUnleashedBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(APlayerUnleashedBase), 885142703U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h__Script_GeneticGeneration_4158915440(TEXT("/Script/GeneticGeneration"),
	Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h__Script_GeneticGeneration_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h__Script_GeneticGeneration_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
