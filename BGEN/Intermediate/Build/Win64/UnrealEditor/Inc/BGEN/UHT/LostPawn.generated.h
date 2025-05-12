// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Actors/LostPawn.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef BGEN_LostPawn_generated_h
#error "LostPawn.generated.h already included, missing '#pragma once' in LostPawn.h"
#endif
#define BGEN_LostPawn_generated_h

#define FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesALostPawn(); \
	friend struct Z_Construct_UClass_ALostPawn_Statics; \
public: \
	DECLARE_CLASS(ALostPawn, APawn, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/BGEN"), NO_API) \
	DECLARE_SERIALIZER(ALostPawn)


#define FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ALostPawn(ALostPawn&&); \
	ALostPawn(const ALostPawn&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ALostPawn); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ALostPawn); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ALostPawn) \
	NO_API virtual ~ALostPawn();


#define FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_9_PROLOG
#define FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_12_INCLASS_NO_PURE_DECLS \
	FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> BGEN_API UClass* StaticClass<class ALostPawn>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_School_BGEN_BGEN_Source_BGEN_Actors_LostPawn_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
