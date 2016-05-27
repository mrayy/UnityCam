

#include "stdafx.h"
#include "UnityHelpers.h"

#include <windows.h>
#include "IUnityGraphics.h"

using namespace mray;

FuncPtr Debug;
FuncFloatRetPtr GetEngineTimePtr;

void LogMessage(const std::string& msg, ELogLevel level)
{
	std::string m;
	if (level == ELL_INFO)
		m = "Info: ";
	if (level == ELL_WARNING)
		m = "Warning: ";
	if (level == ELL_SUCCESS)
		m = "Success: ";
	if (level == ELL_ERROR)
		m = "Error: ";

	m += msg;
#if UNITY_WIN
	OutputDebugStringA(m.c_str());
#else
	printf("%s", m.c_str());
#endif
	LogManager::Instance()->LogMessage(m);
// 	if (Debug)
// 		Debug(m.c_str());
}
float GetEngineTime()
{
	if (GetEngineTimePtr)
	{
		return GetEngineTimePtr();
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_SetDebugFunction(FuncPtr f)
{
	Debug = f;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_SetGetEngineTime(FuncFloatRetPtr f)
{
	GetEngineTimePtr = f;
}

LogManager* LogManager::s_instance=0;
LogManager* LogManager::Instance()
{
	if (!s_instance)
	{
		s_instance = new LogManager();
	}
	return s_instance;
}

LogManager::LogManager()
{
	fileName = "UnityWebcameraLog.txt";
	m_logFile = fopen(fileName.c_str(), "w");
	fclose(m_logFile);
	m_logFile = 0;
}
LogManager::~LogManager()
{
	fclose(m_logFile);
}
void LogManager::LogMessage(const std::string& msg)
{
	m_logFile = fopen(fileName.c_str(), "a");
	fprintf(m_logFile, "%s\n", msg.c_str());
	fclose(m_logFile);
	m_logFile = 0;
}

void ReadImage(int TexID)
{
}


void BlitImage( void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{

	if ( !_TextureNativePtr)
		return;



#if SUPPORT_D3D9
	// D3D9 case
	if (g_GraphicsDeviceType == kGfxRendererD3D9)
	{
		// Update native texture from code
		if (_TextureNativePtr)
		{
			IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)_TextureNativePtr;
			D3DSURFACE_DESC desc;
			d3dtex->GetLevelDesc(0, &desc);
			D3DLOCKED_RECT lr;
			d3dtex->LockRect(0, &lr, nullptr, 0);

			//uchar* data = new uchar[desc.Width*desc.Height * 4];

			//memcpy((unsigned char*)lr.pBits, ifo->imageData, desc.Width*desc.Height * 3);
			CopyToTexture(ifo, (uchar*)lr.pBits,ifo->format);

			d3dtex->UnlockRect(0);
			//delete [] data;
		}
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
			//ctx->UpdateSubresource(d3dtex, 0, nullptr, ifo->imageData, desc.Width * 3, 0);
			
			uchar* data = 0;
			int pitch = 0;

			ctx->UpdateSubresource(d3dtex, 0, nullptr, data, pitch, 0);
			//delete[] data;

		}

		ctx->Release();
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
			int texWidth, texHeight;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);

		}
	}
#endif
}