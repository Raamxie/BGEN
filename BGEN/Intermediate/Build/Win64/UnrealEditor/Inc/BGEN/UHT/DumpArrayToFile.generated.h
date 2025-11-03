// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "DumpArrayToFile.h"

#ifdef BGEN_DumpArrayToFile_generated_h
#error "DumpArrayToFile.generated.h already included, missing '#pragma once' in DumpArrayToFile.h"
#endif
#define BGEN_DumpArrayToFile_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UDumpArrayToFile *********************************************************
#define FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execDumpFloatArrayToFile);


BGEN_API UClass* Z_Construct_UClass_UDumpArrayToFile_NoRegister();

#define FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUDumpArrayToFile(); \
	friend struct Z_Construct_UClass_UDumpArrayToFile_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend BGEN_API UClass* Z_Construct_UClass_UDumpArrayToFile_NoRegister(); \
public: \
	DECLARE_CLASS2(UDumpArrayToFile, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/BGEN"), Z_Construct_UClass_UDumpArrayToFile_NoRegister) \
	DECLARE_SERIALIZER(UDumpArrayToFile)


#define FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UDumpArrayToFile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UDumpArrayToFile(UDumpArrayToFile&&) = delete; \
	UDumpArrayToFile(const UDumpArrayToFile&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UDumpArrayToFile); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UDumpArrayToFile); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UDumpArrayToFile) \
	NO_API virtual ~UDumpArrayToFile();


#define FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_12_PROLOG
#define FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_INCLASS_NO_PURE_DECLS \
	FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UDumpArrayToFile;

// ********** End Class UDumpArrayToFile ***********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Jana_Documents_GitHub_BGEN_BGEN_Source_BGEN_Public_DumpArrayToFile_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
