// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "CustomBehaviourTree.h"

#ifdef GENETICGENERATION_CustomBehaviourTree_generated_h
#error "CustomBehaviourTree.generated.h already included, missing '#pragma once' in CustomBehaviourTree.h"
#endif
#define GENETICGENERATION_CustomBehaviourTree_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UCustomBehaviourTree *****************************************************
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execSaveAsNewAsset); \
	DECLARE_FUNCTION(execMutateTree_Dynamic); \
	DECLARE_FUNCTION(execLoadBehaviorTree);


GENETICGENERATION_API UClass* Z_Construct_UClass_UCustomBehaviourTree_NoRegister();

#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUCustomBehaviourTree(); \
	friend struct Z_Construct_UClass_UCustomBehaviourTree_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend GENETICGENERATION_API UClass* Z_Construct_UClass_UCustomBehaviourTree_NoRegister(); \
public: \
	DECLARE_CLASS2(UCustomBehaviourTree, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GeneticGeneration"), Z_Construct_UClass_UCustomBehaviourTree_NoRegister) \
	DECLARE_SERIALIZER(UCustomBehaviourTree)


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UCustomBehaviourTree(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UCustomBehaviourTree(UCustomBehaviourTree&&) = delete; \
	UCustomBehaviourTree(const UCustomBehaviourTree&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UCustomBehaviourTree); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UCustomBehaviourTree); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UCustomBehaviourTree) \
	NO_API virtual ~UCustomBehaviourTree();


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_11_PROLOG
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_INCLASS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h_14_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UCustomBehaviourTree;

// ********** End Class UCustomBehaviourTree *******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomBehaviourTree_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
