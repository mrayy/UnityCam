// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UNREALCAM_API UnrealCamDLL
{
public:
	static bool SendTextureFromDLL(const unsigned char* data, int width, int height);
	static bool importDLL(FString folder, FString name);
	static void freeDLL();

	static bool importMethodSendTexture();
};
