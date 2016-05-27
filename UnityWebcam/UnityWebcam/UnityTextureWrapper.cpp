
#include "stdafx.h"
#include "UnityTextureWrapper.h"

#include "UnityHelpers.h"

namespace mray
{
UnityTextureWrapper::UnityTextureWrapper() 
{
	_data = 0;
	_width = 0;
	_height = 0;
#if SUPPORT_D3D11
	textureBuf = 0;
#endif
}
UnityTextureWrapper::~UnityTextureWrapper()
{
#if SUPPORT_D3D11
	if (textureBuf)
		textureBuf->Release();
#endif
	if (_data)
		delete[] _data;
	_data = 0;
}

bool UnityTextureWrapper::SendImage(void* _TextureNativePtr)
{
	if (!_TextureNativePtr)
		return false;

	if (!Ready())
		return false;

#if SUPPORT_D3D9
	// D3D9 case
	if (g_GraphicsDeviceType == kUnityGfxRendererD3D9)
	{
	}
#endif

#if SUPPORT_D3D11
	// D3D11 case
	if (g_GraphicsDeviceType == kUnityGfxRendererD3D11)
	{
		ID3D11DeviceContext* ctx = nullptr;
		g_D3D11GraphicsDevice->GetImmediateContext(&ctx);

		// update native texture from code
		if (_TextureNativePtr)
		{
			ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)_TextureNativePtr;

			D3D11_TEXTURE2D_DESC desc;
			d3dtex->GetDesc(&desc);

			if (_width != desc.Width || _height != desc.Height)
			{
				if (textureBuf)
				{
					textureBuf->Release();
					textureBuf = 0;
				}
				D3D11_TEXTURE2D_DESC textureDesc;
				ZeroMemory(&textureDesc, sizeof(textureDesc));
				textureDesc.Width = desc.Width;
				textureDesc.Height = desc.Height;
				textureDesc.MipLevels = 1;
				textureDesc.ArraySize = 1;
				textureDesc.Format = desc.Format;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.SampleDesc.Quality = 0;
				textureDesc.Usage = D3D11_USAGE_STAGING;
				textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				textureDesc.MiscFlags = 0;
				g_D3D11GraphicsDevice->CreateTexture2D(&textureDesc, NULL, &textureBuf);
				_width = desc.Width;
				_height = desc.Height;

				if (_data)
					delete[] _data;
				_data = new uchar[_width*_height * 3];

			}
			ctx->CopyResource(textureBuf, d3dtex);
			D3D11_MAPPED_SUBRESOURCE  mapResource;
			HRESULT hr = ctx->Map(textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource);
			if (SUCCEEDED(hr))
			{
				if (desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM||
					desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
				{
					uchar*src = (uchar*)mapResource.pData;
					uchar*ptr = (uchar*)_data;
					int count= _width*_height * 4;

					while (count>0)
					{
						for (int i = 0; i < _width; ++i){
							ptr[0] = src[i * 4 + 0];
							ptr[1] = src[i * 4 + 1];
							ptr[2] = src[i * 4 + 2];
							ptr += 3;
							count -= 4;
						}
						src += mapResource.RowPitch;
					}
					ctx->Unmap(textureBuf, 0);
				}
				else{
				}
				SharedImageWrapper::SendImage((const uchar*)_data, _width, _height);
			}


		}

	}
#endif

#if SUPPORT_OPENGL
	// OpenGL case
	if (g_GraphicsDeviceType == kUnityGfxRendererOpenGL)
	{
		// update native texture from code
		if (_TextureNativePtr)
		{
			GLuint gltex = (GLuint)(int32_t)(_TextureNativePtr);
			glBindTexture(GL_TEXTURE_2D, gltex);
			int width, height;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

			if (width != _width || height != _height)
			{
				delete[] _data;
				_data = new uchar[width*height*3];
				_width = width;
				_height = height;
			}

			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, _data);

			return SharedImageWrapper::SendImage((const uchar*)_data, width, height);
		}
	}
#endif
	return false;
}

}

