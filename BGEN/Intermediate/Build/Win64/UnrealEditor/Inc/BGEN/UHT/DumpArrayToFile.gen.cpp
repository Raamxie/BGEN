// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "DumpArrayToFile.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeDumpArrayToFile() {}

// ********** Begin Cross Module References ********************************************************
BGEN_API UClass* Z_Construct_UClass_UDumpArrayToFile();
BGEN_API UClass* Z_Construct_UClass_UDumpArrayToFile_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
UPackage* Z_Construct_UPackage__Script_BGEN();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UDumpArrayToFile Function DumpFloatArrayToFile ***************************
struct Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics
{
	struct DumpArrayToFile_eventDumpFloatArrayToFile_Parms
	{
		TArray<float> Array;
		FString FileName;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "File|Debug" },
		{ "ModuleRelativePath", "Public/DumpArrayToFile.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Array_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FileName_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Array_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Array;
	static const UECodeGen_Private::FStrPropertyParams NewProp_FileName;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_Array_Inner = { "Array", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_Array = { "Array", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DumpArrayToFile_eventDumpFloatArrayToFile_Parms, Array), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Array_MetaData), NewProp_Array_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_FileName = { "FileName", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DumpArrayToFile_eventDumpFloatArrayToFile_Parms, FileName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FileName_MetaData), NewProp_FileName_MetaData) };
void Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((DumpArrayToFile_eventDumpFloatArrayToFile_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(DumpArrayToFile_eventDumpFloatArrayToFile_Parms), &Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_Array_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_Array,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_FileName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDumpArrayToFile, nullptr, "DumpFloatArrayToFile", Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::PropPointers), sizeof(Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::DumpArrayToFile_eventDumpFloatArrayToFile_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04442401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::DumpArrayToFile_eventDumpFloatArrayToFile_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDumpArrayToFile::execDumpFloatArrayToFile)
{
	P_GET_TARRAY_REF(float,Z_Param_Out_Array);
	P_GET_PROPERTY(FStrProperty,Z_Param_FileName);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=UDumpArrayToFile::DumpFloatArrayToFile(Z_Param_Out_Array,Z_Param_FileName);
	P_NATIVE_END;
}
// ********** End Class UDumpArrayToFile Function DumpFloatArrayToFile *****************************

// ********** Begin Class UDumpArrayToFile *********************************************************
void UDumpArrayToFile::StaticRegisterNativesUDumpArrayToFile()
{
	UClass* Class = UDumpArrayToFile::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "DumpFloatArrayToFile", &UDumpArrayToFile::execDumpFloatArrayToFile },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UDumpArrayToFile;
UClass* UDumpArrayToFile::GetPrivateStaticClass()
{
	using TClass = UDumpArrayToFile;
	if (!Z_Registration_Info_UClass_UDumpArrayToFile.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("DumpArrayToFile"),
			Z_Registration_Info_UClass_UDumpArrayToFile.InnerSingleton,
			StaticRegisterNativesUDumpArrayToFile,
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
	return Z_Registration_Info_UClass_UDumpArrayToFile.InnerSingleton;
}
UClass* Z_Construct_UClass_UDumpArrayToFile_NoRegister()
{
	return UDumpArrayToFile::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UDumpArrayToFile_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "IncludePath", "DumpArrayToFile.h" },
		{ "ModuleRelativePath", "Public/DumpArrayToFile.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UDumpArrayToFile_DumpFloatArrayToFile, "DumpFloatArrayToFile" }, // 3444206283
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDumpArrayToFile>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UDumpArrayToFile_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
	(UObject* (*)())Z_Construct_UPackage__Script_BGEN,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDumpArrayToFile_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UDumpArrayToFile_Statics::ClassParams = {
	&UDumpArrayToFile::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDumpArrayToFile_Statics::Class_MetaDataParams), Z_Construct_UClass_UDumpArrayToFile_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UDumpArrayToFile()
{
	if (!Z_Registration_Info_UClass_UDumpArrayToFile.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDumpArrayToFile.OuterSingleton, Z_Construct_UClass_UDumpArrayToFile_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UDumpArrayToFile.OuterSingleton;
}
UDumpArrayToFile::UDumpArrayToFile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UDumpArrayToFile);
UDumpArrayToFile::~UDumpArrayToFile() {}
// ********** End Class UDumpArrayToFile ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h__Script_BGEN_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UDumpArrayToFile, UDumpArrayToFile::StaticClass, TEXT("UDumpArrayToFile"), &Z_Registration_Info_UClass_UDumpArrayToFile, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDumpArrayToFile), 1707434868U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h__Script_BGEN_1202888041(TEXT("/Script/BGEN"),
	Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h__Script_BGEN_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h__Script_BGEN_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
