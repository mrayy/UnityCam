

#ifndef __UNITYTEXTUREWRAPPER__
#define __UNITYTEXTUREWRAPPER__

#include "SharedImageWrapper.h"
#include "IUnityGraphics.h"

namespace mray
{
class UNITY_INTERFACE_EXPORT UnityTextureWrapper :public SharedImageWrapper
{
protected:
	void* _data;
	int _width;
	int _height;

#if SUPPORT_D3D11
	ID3D11Texture2D* textureBuf;
#endif
public:
	UnityTextureWrapper();
	~UnityTextureWrapper();

	bool SendImage(void* _TextureNativePtr);

};

}


#endif
