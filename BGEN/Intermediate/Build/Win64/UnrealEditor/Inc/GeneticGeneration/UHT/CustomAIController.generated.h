// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "CustomAIController.h"

#ifdef GENETICGENERATION_CustomAIController_generated_h
#error "CustomAIController.generated.h already included, missing '#pragma once' in CustomAIController.h"
#endif
#define GENETICGENERATION_CustomAIController_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class ACustomAIController ******************************************************
GENETICGENERATION_API UClass* Z_Construct_UClass_ACustomAIController_NoRegister();

#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_20_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesACustomAIController(); \
	friend struct Z_Construct_UClass_ACustomAIController_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend GENETICGENERATION_API UClass* Z_Construct_UClass_ACustomAIController_NoRegister(); \
public: \
	DECLARE_CLASS2(ACustomAIController, AAIController, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GeneticGeneration"), Z_Construct_UClass_ACustomAIController_NoRegister) \
	DECLARE_SERIALIZER(ACustomAIController)


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_20_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ACustomAIController(ACustomAIController&&) = delete; \
	ACustomAIController(const ACustomAIController&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ACustomAIController); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ACustomAIController); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ACustomAIController) \
	NO_API virtual ~ACustomAIController();


#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_17_PROLOG
#define FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_20_INCLASS_NO_PURE_DECLS \
	FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h_20_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ACustomAIController;

// ********** End Class ACustomAIController ********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Raamxie_Documents_Unreal_Projects_BGEN_BGEN_Source_GeneticGeneration_Public_CustomAIController_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
