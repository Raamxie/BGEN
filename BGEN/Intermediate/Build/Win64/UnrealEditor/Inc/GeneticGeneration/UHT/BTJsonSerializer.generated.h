// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "BTJsonSerializer.h"

#ifdef GENETICGENERATION_BTJsonSerializer_generated_h
#error "BTJsonSerializer.generated.h already included, missing '#pragma once' in BTJsonSerializer.h"
#endif
#define GENETICGENERATION_BTJsonSerializer_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FBTNodeJSON *******************************************************
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_10_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FBTNodeJSON_Statics; \
	GENETICGENERATION_API static class UScriptStruct* StaticStruct();


struct FBTNodeJSON;
// ********** End ScriptStruct FBTNodeJSON *********************************************************

// ********** Begin Class UBTJsonSerializer ********************************************************
GENETICGENERATION_API UClass* Z_Construct_UClass_UBTJsonSerializer_NoRegister();

#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_31_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUBTJsonSerializer(); \
	friend struct Z_Construct_UClass_UBTJsonSerializer_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend GENETICGENERATION_API UClass* Z_Construct_UClass_UBTJsonSerializer_NoRegister(); \
public: \
	DECLARE_CLASS2(UBTJsonSerializer, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GeneticGeneration"), Z_Construct_UClass_UBTJsonSerializer_NoRegister) \
	DECLARE_SERIALIZER(UBTJsonSerializer)


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_31_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBTJsonSerializer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UBTJsonSerializer(UBTJsonSerializer&&) = delete; \
	UBTJsonSerializer(const UBTJsonSerializer&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBTJsonSerializer); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBTJsonSerializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBTJsonSerializer) \
	NO_API virtual ~UBTJsonSerializer();


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_28_PROLOG
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_31_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_31_INCLASS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h_31_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UBTJsonSerializer;

// ********** End Class UBTJsonSerializer **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_BTJsonSerializer_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
