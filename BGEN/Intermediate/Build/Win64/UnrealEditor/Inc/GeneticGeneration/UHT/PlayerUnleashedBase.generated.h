// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "PlayerUnleashedBase.h"

#ifdef GENETICGENERATION_PlayerUnleashedBase_generated_h
#error "PlayerUnleashedBase.generated.h already included, missing '#pragma once' in PlayerUnleashedBase.h"
#endif
#define GENETICGENERATION_PlayerUnleashedBase_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Delegate FOnPlayerEvent ********************************************************
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_9_DELEGATE \
GENETICGENERATION_API void FOnPlayerEvent_DelegateWrapper(const FMulticastScriptDelegate& OnPlayerEvent);


// ********** End Delegate FOnPlayerEvent **********************************************************

// ********** Begin Class APlayerUnleashedBase *****************************************************
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execBroadcastPlayerDeath); \
	DECLARE_FUNCTION(execLogDamageReceived);


GENETICGENERATION_API UClass* Z_Construct_UClass_APlayerUnleashedBase_NoRegister();

#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPlayerUnleashedBase(); \
	friend struct Z_Construct_UClass_APlayerUnleashedBase_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend GENETICGENERATION_API UClass* Z_Construct_UClass_APlayerUnleashedBase_NoRegister(); \
public: \
	DECLARE_CLASS2(APlayerUnleashedBase, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GeneticGeneration"), Z_Construct_UClass_APlayerUnleashedBase_NoRegister) \
	DECLARE_SERIALIZER(APlayerUnleashedBase)


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	APlayerUnleashedBase(APlayerUnleashedBase&&) = delete; \
	APlayerUnleashedBase(const APlayerUnleashedBase&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APlayerUnleashedBase); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APlayerUnleashedBase); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APlayerUnleashedBase) \
	NO_API virtual ~APlayerUnleashedBase();


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_10_PROLOG
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_INCLASS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class APlayerUnleashedBase;

// ********** End Class APlayerUnleashedBase *******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_PlayerUnleashedBase_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
