
#ifndef __UNITYAPI__
#define __UNITYAPI__

#include "IUnityInterface.h"

extern "C" UNITY_INTERFACE_EXPORT void* CreateTextureWrapper();
extern "C" UNITY_INTERFACE_EXPORT void DeleteTextureWrapper(void* wrapper);
extern "C" UNITY_INTERFACE_EXPORT bool SendTexture(void* wrapper,void* TextureID);


#endif
