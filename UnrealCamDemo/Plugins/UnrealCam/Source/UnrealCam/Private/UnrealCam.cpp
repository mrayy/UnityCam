// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UnrealCam.h"
#include "UnrealCamDLL.h"

#define LOCTEXT_NAMESPACE "FUnrealCamModule"

void FUnrealCamModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UnrealCamDLL::importDLL("ExternalDLL", "UnrealWebcam.dll");
	UnrealCamDLL::importMethodSendTexture();
}

void FUnrealCamModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UnrealCamDLL::freeDLL();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealCamModule, UnrealCam)