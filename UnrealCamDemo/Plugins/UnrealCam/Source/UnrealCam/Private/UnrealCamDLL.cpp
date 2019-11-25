// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCamDLL.h"
#include "Misc/Paths.h"

typedef bool(*_SendTexture)(const unsigned char* data, int width, int height);
_SendTexture m_SendTextureFromDLL;
void* v_dllHandle;

bool UnrealCamDLL::importDLL(FString folder, FString name)
{
	FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;

	if (FPaths::FileExists(filePath))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*filePath);
		if (v_dllHandle != NULL)
		{
			return true;
		}
	}
	return false;
}

bool UnrealCamDLL::importMethodSendTexture()
{
	if (v_dllHandle != NULL)
	{
		m_SendTextureFromDLL = NULL;
		FString procName = "SendTexture";
		m_SendTextureFromDLL = (_SendTexture)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_SendTextureFromDLL != NULL)
		{
			return true;
		}
	}
	return false;
}

bool UnrealCamDLL::SendTextureFromDLL(const unsigned char* data, int width, int height)
{
	if (m_SendTextureFromDLL != NULL)
	{
		bool out = bool(m_SendTextureFromDLL(data, width, height));
		return out;
	}
	return false;
}

void UnrealCamDLL::freeDLL()
{
	if (v_dllHandle != NULL)
	{
		m_SendTextureFromDLL = NULL;

		FPlatformProcess::FreeDllHandle(v_dllHandle);
		v_dllHandle = NULL;
	}
}