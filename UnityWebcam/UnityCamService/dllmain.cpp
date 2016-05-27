


#include "stdafx.h"

#include <string>


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);


 
//http://stackoverflow.com/questions/6646702/get-file-name-without-extension
#ifdef UNICODE //Test to see if we're using wchar_ts or not.
typedef std::wstring StringType;
#else
typedef std::string StringType;
#endif

StringType GetBaseFilename(const TCHAR *filename)
{
	StringType fName(filename);
	size_t pos = fName.rfind(L"\\");
	if (pos != StringType::npos)
	{
		fName = fName.substr(pos+1, fName.length()-pos);
	}


	 pos = fName.rfind(L".");
	if (pos == StringType::npos)  //No extension.
		return fName;

	if (pos == 0)    //. is at the front. Not an extension.
		return fName;

	return fName.substr(0, pos);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//TCHAR ModuleName[256];
	//::GetModuleFileName(hModule, ModuleName, 255);

	//CaptureSourceName= GetBaseFilename(ModuleName);
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		//MessageBox(0, L" DLL_PROCESS_ATTACH", L"", 0);
		break;
	case DLL_THREAD_DETACH:
		//MessageBox(0, L" DLL_THREAD_DETACH", L"", 0);
		break;
	case DLL_PROCESS_DETACH:
		//MessageBox(0, L" DLL_PROCESS_DETACH", L"", 0);
		break;
	}
	return DllEntryPoint((HINSTANCE)(hModule), ul_reason_for_call, lpReserved);
}
