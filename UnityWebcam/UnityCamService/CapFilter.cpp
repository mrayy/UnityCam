
#include "StdAfx.h"
#include <initguid.h>
#include "CaptureSource.h"

#define CreateComObject(clsid, iid, var) CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, iid, (void **)&var);

STDAPI AMovieSetupRegisterServer( CLSID   clsServer, LPCWSTR szDescription, LPCWSTR szFileName, LPCWSTR szThreadingModel = L"Both", LPCWSTR szServerType     = L"InprocServer32" );
STDAPI AMovieSetupUnregisterServer( CLSID clsServer );

CFactoryTemplate g_Templates[] = 
{
	{ CaptureSourceName, &CLSID_UnityCamService, CaptureSource::CreateInstance, NULL, &sudCaptureSource }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


// Stack Overflow - "Fake" DirectShow video capture device
// http://stackoverflow.com/questions/1376734/fake-directshow-video-capture-device
STDAPI RegisterFilters(BOOL bRegister)
{
    HRESULT hr = NOERROR;
    WCHAR achFileName[MAX_PATH];
    char achTemp[MAX_PATH];
    ASSERT(g_hInst != 0);


    if( 0 == GetModuleFileNameA(g_hInst, achTemp, sizeof(achTemp))) 
        return AmHresultFromWin32(GetLastError());

    MultiByteToWideChar(CP_ACP, 0L, achTemp, lstrlenA(achTemp) + 1, 
                       achFileName, NUMELMS(achFileName));
  
    hr = CoInitialize(0);
    if(bRegister)
    {
		hr = AMovieSetupRegisterServer(CLSID_UnityCamService, CaptureSourceName, achFileName, L"Both", L"InprocServer32");
    }

    if(SUCCEEDED(hr)) {
        IFilterMapper2 *fm = NULL;
        hr = CreateComObject(CLSID_FilterMapper2, IID_IFilterMapper2, fm);

        if(SUCCEEDED(hr)) {
            if(bRegister) {
                IMoniker *pMoniker = 0;
                REGFILTER2 rf2;
                rf2.dwVersion = 1;
                rf2.dwMerit = MERIT_DO_NOT_USE;
                rf2.cPins = 1;
                rf2.rgPins = &sudCaptureSourceOut;
				hr = fm->RegisterFilter(CLSID_UnityCamService, CaptureSourceName, 0, &CLSID_VideoInputDeviceCategory, NULL, &rf2);
				if (SUCCEEDED(hr))
				{
					//MessageBox(0, L"RegisterFilter", L"Success", NULL);
				}
				else{
					//MessageBox(0, L"RegisterFilter", L"Failed", NULL);
				}
            }else{
                hr = fm->UnregisterFilter(&CLSID_VideoInputDeviceCategory, 0, CLSID_UnityCamService);
            }
        }
		if(fm) {
			fm->Release();
			fm = NULL;
		}
    }

	if(SUCCEEDED(hr) && !bRegister) {
		hr = AMovieSetupUnregisterServer(CLSID_UnityCamService);
	}
	if (!SUCCEEDED(hr))
	{
	//	MessageBox(0, L"Failed to RegisterFilters", L"Error", NULL);
	}
	else{
		//MessageBox(0, L"Success to RegisterFilters", L"Success", NULL);

	}

    CoFreeUnusedLibraries();
    CoUninitialize();
    return hr;
}

STDAPI DllRegisterServer()
{
    HRESULT res= RegisterFilters(TRUE);

	//this quick hack lets unity read the virtual camera too!
	// thanks for:
	//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/cd2b9d2d-b961-442d-8946-fdc038fed530/where-to-specify-device-id-in-the-filter?forum=windowsdirectshowdevelopment
	HKEY hKey;

	LPOLESTR guidString;
	StringFromCLSID(CLSID_VideoInputDeviceCategory, &guidString);
	std::wstring inputCat = guidString;
	std::wstring str_video_capture_device_key = L"CLSID\\" + inputCat + L"\\Instance";

	//std::string str_video_capture_device_key("SOFTWARE\\Classes\\CLSID\\{860BB310-5D01-11d0-BD3B-00A0C911CE86}\\Instance\\");

	LPOLESTR olestr_CLSID;
	StringFromCLSID(CLSID_UnityCamService, &olestr_CLSID);

	std::wstring wstr_CLSID(olestr_CLSID);

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr_CLSID[0], (int)wstr_CLSID.size(), NULL, 0, NULL, NULL);
	std::string str2(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr_CLSID[0], (int)wstr_CLSID.size(), &str2[0], size_needed, NULL, NULL);

	str_video_capture_device_key.append(wstr_CLSID);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, str_video_capture_device_key.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	LPCSTR value = ("DevicePath");
	LPCSTR data = "foo:bar";
	RegSetValueExA(hKey, value, 0, REG_SZ, (LPBYTE)data, strlen(data) + 1);
	RegCloseKey(hKey);
	if (!SUCCEEDED(res))
	{
		//MessageBox(0, L"Failed to RegisterServer", L"Error", NULL);
	}
	else{
		//MessageBox(0, L"Success to RegisterServer", L"Success", NULL);

	}
	return res;
}

STDAPI DllUnregisterServer()
{
	HRESULT res = RegisterFilters(FALSE);
	if (!SUCCEEDED(res))
	{
		//MessageBox(0, L"Failed to DllUnregisterServer", L"Error", NULL);
	}
	else{
		//MessageBox(0, L"Success to DllUnregisterServer", L"Success", NULL);

	}
	return res;
}
