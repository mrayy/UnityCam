

#include "stdafx.h"
#include "UnityAPI.h"
#include "UnityTextureWrapper.h"
#include "SharedImageWrapper.h"

using namespace mray;


extern "C" UNITY_INTERFACE_EXPORT void* CreateTextureWrapper()
{
	UnityTextureWrapper* w = new UnityTextureWrapper();
	return w;
}
extern "C" UNITY_INTERFACE_EXPORT void DeleteTextureWrapper(void* wrapper)
{
	UnityTextureWrapper* w = static_cast<UnityTextureWrapper*>(wrapper);
	if (!w)
		return;
	delete w;
}
extern "C" UNITY_INTERFACE_EXPORT bool SendTexture(void* wrapper, void* TextureID)
{
	UnityTextureWrapper* w = static_cast<UnityTextureWrapper*>(wrapper);
	if (!w)
		return false;

	return w->SendImage(TextureID);

}

extern "C" __declspec(dllexport) bool SendTexture(const unsigned char* data, int width, int height)
{
	if (!wrapper)
		wrapper = new SharedImageWrapper();
	wrapper->SendImage(data, width, height);
}
