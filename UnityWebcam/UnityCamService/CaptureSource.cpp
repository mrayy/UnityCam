#include "StdAfx.h"
#include "CaptureSource.h"
#include "Globals.h"

#include "CaptureSharedMemory.h"

#define DEFAULT_WIDTH	1280
#define DEFAULT_HEIGHT	720
#define MIN_WIDTH		640
#define MIN_HEIGHT		480
#define MAX_WIDTH		1280
#define MAX_HEIGHT		720
#define MAX_FPS			30
#define MIN_FPS			30
#define MAX_FRAME_INTERVAL	10000000 / MIN_FPS
#define MIN_FRAME_INTERVAL	10000000 / MAX_FPS

#define DEFAULT_FPS		60

#pragma comment(lib, "wsock32.lib")
#include <winsock2.h>
#include <stdio.h>



std::function<bool()> waitFps_closure() {
	auto startTime = std::chrono::system_clock::now();
	long long counter = 0;
	int fpsCounter = 0;
	return [startTime, counter]() mutable {
		auto now = std::chrono::system_clock::now();
		auto diff = now - startTime;
		long long diffInt = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

		if (diffInt * DEFAULT_FPS > 1000 * counter) {
			counter++;
			return true;
		}
		else {
			return false;
		}
	};
}

const AMOVIESETUP_MEDIATYPE sudMediaTypesCaptureSourceOut = 
{ 
    &MEDIATYPE_Video, 
    &MEDIASUBTYPE_NULL 
};

const AMOVIESETUP_PIN sudCaptureSourceOut =
{
    L"Output",             // Pin string name
    FALSE,                 // Is it rendered
    TRUE,                  // Is it an output
    FALSE,                 // Can we have none
    FALSE,                 // Can we have many
    &CLSID_NULL,           // Connects to filter
    NULL,                  // Connects to pin
    1,                     // Number of types
    &sudMediaTypesCaptureSourceOut      // Pin Media types
};

const AMOVIESETUP_FILTER sudCaptureSource =
{
    &CLSID_UnityCamService,	// Filter CLSID
	CaptureSourceName,			// String name
    MERIT_DO_NOT_USE,		// Filter merit
    1,//0,						// Number pins
    & sudCaptureSourceOut	// Pin details
};

CUnknown * WINAPI CaptureSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);
	//MessageBox(0, L"CreateInstance", L"", NULL);
    CaptureSource *punk = new CaptureSource(lpunk, phr);
    if (punk == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }

	HRESULT hr = S_OK;
	CaptureSharedMemory* pRecv = new CaptureSharedMemory(CS_SHARED_Base_PATH, CS_EVENT_Base_WRITE,CS_EVENT_Base_READ, &hr);
	if(FAILED(hr)) {
		*phr = hr;
		delete pRecv;
		pRecv = NULL;
		delete punk;
		punk = NULL;
		return NULL;
	}
	
	hr = punk->Init(pRecv, DEFAULT_FPS);
	if(FAILED(hr)) {
		*phr = hr;
		delete pRecv;
		pRecv = NULL;
		delete punk;
		punk = NULL;
		return NULL;
	}
    return punk;
}

CaptureSource::CaptureSource(LPUNKNOWN lpunk, HRESULT* phr)
: CSource(L"Source", lpunk, CLSID_UnityCamService, phr)
{
	//MessageBox(0, L"CaptureSource()", L"", 0);;

}

CaptureSource::~CaptureSource(void)
{
	;
}

STDMETHODIMP CaptureSource::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if(ppv == NULL) {
		return E_POINTER;
	}
	if(riid == IID_IQualityControl) {
		*ppv = (IQualityControl*) this;
		AddRef();
		return S_OK;
	}else if(riid == IID_ICamSource) {
		*ppv = (ICamSource*) this;
		AddRef();
		return S_OK;
	}
	return CSource::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CaptureSource::Notify(IBaseFilter *pSelf, Quality q)
{
	return S_OK;
}

STDMETHODIMP CaptureSource::SetSink(IQualityControl *piqc)
{
	return S_OK;
}

HRESULT CaptureSource::Init(CaptureSharedMemory* pReceiver, int fps)
{
	HRESULT hr = S_OK;
	CaptureStream* pStream = new CaptureStream(this, pReceiver, fps, & hr);
	if(FAILED(hr)) {
		delete pStream;
		pStream = NULL;
		return hr;
	}
	return S_OK;
}

CaptureSource::CaptureStream::CaptureStream(CaptureSource* pOwner, CaptureSharedMemory* pReceiver, int fps, HRESULT* phr)
:CSourceStream(L"Stream", phr, pOwner, L"Output")
{
	oldStartTime_ = 0;
	lastTime = std::chrono::system_clock::now();
	fpsChecker = waitFps_closure();
	m_avgTimePerFrame = 10000000 / fps;
	m_iPrefWidth = DEFAULT_WIDTH;
	m_iPrefHeight = DEFAULT_HEIGHT;
	m_llTime = 0;
	m_llFrame = 0;
	m_pReceiver = pReceiver;
	m_llLastSampleTime = -1;


	GetMediaType(/*1,*/ &m_mt);
}

CaptureSource::CaptureStream::~CaptureStream()
{
	if(m_pReceiver != NULL) {
		m_pReceiver->Destroy();
		m_pReceiver = NULL;
	}
}

HRESULT CaptureSource::CaptureStream::QueryInterface(REFIID riid, void **ppv)
{
	// Standard OLE stuff
	if (riid == _uuidof(IAMStreamConfig))
		*ppv = (IAMStreamConfig*)this;
	else if (riid == _uuidof(IKsPropertySet))
		*ppv = (IKsPropertySet*)this;
	else
		return CSourceStream::QueryInterface(riid, ppv);

	AddRef();
	return S_OK;
}


STDMETHODIMP CaptureSource::CaptureStream::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if(ppv == NULL) {
		return E_POINTER;
	}
	if(riid == IID_IKsPropertySet) {
		*ppv = (IKsPropertySet*) this;
		AddRef();
		return S_OK;
	}else if(riid == IID_IQualityControl) {
		*ppv = (IQualityControl*) this;
		AddRef();
		return S_OK;
	}else if(riid == IID_IAMStreamConfig) {
		*ppv = (IAMStreamConfig*) this;
		AddRef();
		return S_OK;
	}
	return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CaptureSource::CaptureStream::QuerySupported(REFGUID rguidPropSet, ULONG ulId, PULONG pulTypeSupport)
{

	if (rguidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (ulId != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	// We support getting this property, but not setting it.
	if (pulTypeSupport) *pulTypeSupport = KSPROPERTY_SUPPORT_GET;
	return S_OK;
	/*
	if(rguidPropSet == AMPROPSETID_Pin && ulId == AMPROPERTY_PIN_CATEGORY) {
		*pulTypeSupport = KSPROPERTY_SUPPORT_GET;
		return S_OK;
	}
	return E_NOTIMPL;*/
}

STDMETHODIMP CaptureSource::CaptureStream::Get(REFGUID rguidPropSet, ULONG ulId, LPVOID pInstanceData, ULONG ulInstanceLength, LPVOID pPropertyData, ULONG ulDataLength, PULONG pulBytesReturned)
{

	if (rguidPropSet != AMPROPSETID_Pin)             return E_PROP_SET_UNSUPPORTED;
	if (ulId != AMPROPERTY_PIN_CATEGORY)        return E_PROP_ID_UNSUPPORTED;
	if (pPropertyData == NULL && pulBytesReturned == NULL)   return E_POINTER;

	if (pulBytesReturned) *pulBytesReturned = sizeof(GUID);
	if (pPropertyData == NULL)          return S_OK; // Caller just wants to know the size. 
	if (ulDataLength < sizeof(GUID))  return E_UNEXPECTED;// The buffer is too small.

	*(GUID *)pPropertyData = PIN_CATEGORY_CAPTURE;
	return S_OK;
	/*
	if(rguidPropSet == AMPROPSETID_Pin && ulId == AMPROPERTY_PIN_CATEGORY) {
		if(pPropertyData == NULL) {
			return E_POINTER;
		}
		if(ulDataLength != sizeof(GUID)) {
			return E_INVALIDARG;
		}
		memcpy(pPropertyData, &PIN_CATEGORY_CAPTURE, sizeof(GUID));
		*pulBytesReturned = sizeof(GUID);
		return S_OK;
	}
	return E_NOTIMPL;*/
}

STDMETHODIMP CaptureSource::CaptureStream::Set(REFGUID rguidPropSet, ULONG ulId, LPVOID pInstanceData, ULONG ulInstanceLength, LPVOID pPropertyData, ULONG ulDataLength)
{
	return E_NOTIMPL;
}

HRESULT CaptureSource::CaptureStream::FillBuffer(IMediaSample *pSamp)
{

    CAutoLock lock(m_pFilter->pStateLock());

	HRESULT hr = S_OK;
	VIDEOINFO *pvi = (VIDEOINFO*) m_mt.Format();
	hr = pSamp->SetActualDataLength(pvi->bmiHeader.biSizeImage);
	if(FAILED(hr)) {
		return hr;
	}
	BYTE* pBuf = NULL;
	hr = pSamp->GetPointer(& pBuf);
	if(FAILED(hr)) {
		return hr;
	}
	ZeroMemory(pBuf, pvi->bmiHeader.biSizeImage);
	unsigned char* pRecvBuf = NULL;
	BufferHeader recvDesc;
	ZeroMemory(& recvDesc, sizeof(recvDesc));
	LONGLONG avgTimePF = m_avgTimePerFrame / 10000;
	hr = m_pReceiver->Receive(& recvDesc, & pRecvBuf);


	if(FAILED(hr)) {
		return hr;
	}
	hr = m_pReceiver->NotifyProcessed(pRecvBuf);
	pRecvBuf = NULL;
	if (FAILED(hr)) {
		return hr;
	}
	//		Sleep((DWORD) avgTimePF / 2);

// 	while (!fpsChecker()) {
// 		Sleep(3);
// 	}

	hr = m_pReceiver->Receive(&recvDesc, &pRecvBuf);
	if (FAILED(hr)) {
		return hr;
	}

	HRESULT retCode = S_OK;
	if(hr == S_FALSE) {
		retCode = S_FALSE;
	}
	m_llLastSampleTime = recvDesc.timestamp;

	int destbytes = pvi->bmiHeader.biBitCount / 8;
	int destwidthstep = pvi->bmiHeader.biSizeImage / pvi->bmiHeader.biHeight;
	double scaling = 0.0, ax = 0.0, ay = 0.0;
	hr = GetScaling(recvDesc.width, recvDesc.height, & scaling, & ax, & ay);
	if(FAILED(hr)) {
		return hr;
	}

	for(int y = 0; y < pvi->bmiHeader.biHeight; y ++) {
		for(int x = 0; x < pvi->bmiHeader.biWidth; x ++) {
			double sx = (x - ax) / scaling;
			double sy = (y - ay) / scaling;
			int isx = (int) sx;
			int isy = (int) sy;
			int destoffs = x * destbytes + y * destwidthstep;
			if(isx < 0 || isy < 0 || isx >= recvDesc.width || isy >= recvDesc.height) {
				pBuf[destoffs + 2] = 0x00;
				pBuf[destoffs + 1] = 0x00;
				pBuf[destoffs + 0] = 0x00;
				continue;
			}

			int srcoffs = isx * 3 + isy * (recvDesc.width * 3);
			pBuf[destoffs + 2] = pRecvBuf[srcoffs + 0];
			pBuf[destoffs + 1] = pRecvBuf[srcoffs + 1];
			pBuf[destoffs + 0] = pRecvBuf[srcoffs + 2];
		}
	}

	hr = m_pReceiver->NotifyProcessed(pRecvBuf);
	pRecvBuf = NULL;
	if(FAILED(hr)) {
		return hr;
	}

	REFERENCE_TIME startTime = oldStartTime_;
	REFERENCE_TIME endTime = startTime + m_avgTimePerFrame;

	oldStartTime_ = endTime;

	hr = pSamp->SetTime(& startTime, & endTime);
	if(FAILED(hr)) {
		return hr;
	}
	LONGLONG mtStart = m_llFrame;
	LONGLONG mtEnd = mtStart + 1;
	hr = pSamp->SetMediaTime(& mtStart, & mtEnd);
	if(FAILED(hr)) {
		return hr;
	}
	m_llTime += avgTimePF;
	m_llFrame ++;
	return retCode;
}

HRESULT CaptureSource::CaptureStream::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest)
{
	if(pAlloc == NULL || pRequest == NULL) {
		return E_POINTER;
	}
	HRESULT hr = NOERROR;
	VIDEOINFO *pvi = (VIDEOINFO*) m_mt.Format();
	pRequest->cBuffers = 1;

	if (pvi->bmiHeader.biSizeImage > (DWORD)pRequest->cbBuffer) {
		pRequest->cbBuffer = pvi->bmiHeader.biSizeImage;
	}
	ALLOCATOR_PROPERTIES actual;
	hr=pAlloc->SetProperties(pRequest, &actual);
	if (FAILED(hr)) {
		return hr;
	}
	if (actual.cbBuffer < pRequest->cbBuffer) {
		return E_FAIL;
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// This is called when the output format has been negotiated
//////////////////////////////////////////////////////////////////////////
HRESULT CaptureSource::CaptureStream::SetMediaType(const CMediaType *pmt)
{
	DECLARE_PTR(VIDEOINFOHEADER, pvi, pmt->Format());
	HRESULT hr = CSourceStream::SetMediaType(pmt);
	return hr;
}


// This method is called to see if a given output format is supported
HRESULT CaptureSource::CaptureStream::CheckMediaType(const CMediaType *pMediaType)
{
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)(pMediaType->Format());
	if (*pMediaType != m_mt)
		return E_INVALIDARG;
	return S_OK;
} // CheckMediaType

STDMETHODIMP CaptureSource::CaptureStream::Notify(IBaseFilter *pSelf, Quality q)
{
	return S_OK;
}

STDMETHODIMP CaptureSource::CaptureStream::SetSink(IQualityControl *piqc)
{
	return S_OK;
}

STDMETHODIMP CaptureSource::CaptureStream::SetFormat(AM_MEDIA_TYPE *pmt)
{
	if(pmt == NULL) {
		return E_POINTER;
	}
	VIDEOINFO* pvi = (VIDEOINFO*) pmt->pbFormat;
	if(pvi == NULL) {
		return E_UNEXPECTED;
	}
	m_avgTimePerFrame = pvi->AvgTimePerFrame;
	m_iPrefWidth = pvi->bmiHeader.biWidth;
	m_iPrefHeight = pvi->bmiHeader.biHeight;
	return S_OK;
}

STDMETHODIMP CaptureSource::CaptureStream::GetFormat(AM_MEDIA_TYPE **ppmt)
{
	if(ppmt == NULL) {
		return E_POINTER;
	}
	/*
	HRESULT hr = GetMediaType(& m_mt);
	if(FAILED(hr)) {
		return hr;
	}*/

	*ppmt = CreateMediaType(&m_mt);
	return S_OK;
}

STDMETHODIMP CaptureSource::CaptureStream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	if(piCount == NULL || piSize == NULL) {
		return E_POINTER;
	}
	*piCount = 1;// 1;
	*piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
	return S_OK;
}

STDMETHODIMP CaptureSource::CaptureStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **ppmt, BYTE *pSCC)
{
	if(ppmt == NULL || pSCC == NULL) {
		return E_POINTER;
	}

//	if (iIndex == 0) iIndex = 4;
	CMediaType mt;
	HRESULT hr = GetMediaType(/*iIndex,*/ & mt);
	if(FAILED(hr)) {
		return hr;
	}
	*ppmt = CreateMediaType(& mt);

	VIDEO_STREAM_CONFIG_CAPS* pCaps = (VIDEO_STREAM_CONFIG_CAPS*) pSCC;
	ZeroMemory(pCaps, sizeof(VIDEO_STREAM_CONFIG_CAPS));

	pCaps->guid = FORMAT_VideoInfo;
	pCaps->VideoStandard = 0;
	pCaps->InputSize.cx = MAX_WIDTH;
	pCaps->InputSize.cy = MAX_HEIGHT;
	pCaps->MinCroppingSize.cx = MIN_WIDTH;
	pCaps->MinCroppingSize.cy = MIN_HEIGHT;
	pCaps->MaxCroppingSize.cx = MIN_WIDTH;
	pCaps->MaxCroppingSize.cy = MIN_HEIGHT;
	pCaps->CropGranularityX = MIN_WIDTH;
	pCaps->CropGranularityY = MIN_HEIGHT;
	pCaps->CropAlignX = 1;
	pCaps->CropAlignY = 1;
	pCaps->MinOutputSize.cx = MIN_WIDTH;
	pCaps->MinOutputSize.cy = MIN_HEIGHT;
	pCaps->MaxOutputSize.cx = MAX_WIDTH;
	pCaps->MaxOutputSize.cy = MAX_HEIGHT;
	pCaps->OutputGranularityX = 1;
	pCaps->OutputGranularityY = 1;
	pCaps->StretchTapsX = 0;
	pCaps->StretchTapsY = 0;
	pCaps->ShrinkTapsX = 0;
	pCaps->ShrinkTapsY = 0;
	pCaps->MinFrameInterval = MIN_FRAME_INTERVAL;
	pCaps->MaxFrameInterval = MAX_FRAME_INTERVAL;
	pCaps->MinBitsPerSecond = MIN_WIDTH * MIN_HEIGHT * 3 * 8 * MIN_FPS;
	pCaps->MaxBitsPerSecond = MAX_WIDTH * MAX_HEIGHT * 3 * 8 * MAX_FPS;

	return S_OK;
}

HRESULT CaptureSource::CaptureStream::GetMediaType(CMediaType *pMediaType)
{
	int iPos = 1;
	if (iPos <  0) return E_INVALIDARG;
	if (iPos > 2) return VFW_S_NO_MORE_ITEMS;

	if(pMediaType == NULL) {
		return E_POINTER;
	}

	struct MediaType
	{
		MediaType(){}
		MediaType(int w,int h):width(w),height(h){}
		int width, height;
	};

	static MediaType _media[] = {
		MediaType(640, 480),
		MediaType(1280, 720),
		MediaType(1920, 1080),
	};

	VIDEOINFO *pvi = (VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
	ZeroMemory(pvi, sizeof(VIDEOINFO));
	pvi->AvgTimePerFrame = m_avgTimePerFrame;
	BITMAPINFOHEADER *pBmi = &(pvi->bmiHeader);
	pBmi->biSize = sizeof(BITMAPINFOHEADER);
	pBmi->biWidth = _media[iPos].width;// m_iPrefWidth;
	pBmi->biHeight = _media[iPos].height;// m_iPrefHeight;
	pBmi->biPlanes = 1;
	pBmi->biBitCount = 24;
	pBmi->biCompression = BI_RGB;
	pvi->bmiHeader.biSizeImage = DIBSIZE(pvi->bmiHeader);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	const GUID subtype=GetBitmapSubtype(&pvi->bmiHeader);
	pMediaType->SetSubtype(&subtype);
	pMediaType->SetSampleSize(DIBSIZE(*pBmi));
	pMediaType->SetTemporalCompression(FALSE);
	return S_OK;
}

HRESULT CaptureSource::CaptureStream::OnThreadStartPlay(void)
{
	m_llTime = 0;
	m_llFrame = 0;
	m_llLastSampleTime = -1;

	return CSourceStream::OnThreadStartPlay();
}

HRESULT CaptureSource::CaptureStream::GetScaling(int width, int height, double* pScaling, double* pOffsetX, double* pOffsetY)
{
	if(pScaling == NULL || pOffsetX == NULL || pOffsetY == NULL) {
		return E_POINTER;
	}
	VIDEOINFO *pvi = (VIDEOINFO*) m_mt.Format();
	BITMAPINFOHEADER *pBmi = &(pvi->bmiHeader);
	
	double aw = pBmi->biWidth;
	double ah = pBmi->biHeight;
	double sw = width;
	double sh = height;
	double wscaling = aw / sw;
	double hscaling = ah / sh;
	double scaling = min(wscaling, hscaling);
	if(scaling < 0.0001) {
		return E_UNEXPECTED;
	}

	double dw = scaling * sw;
	double dh = scaling * sh;
	double ax = (aw - dw) / 2.0;
	double ay = (ah - dh) / 2.0;
	*pScaling = scaling;
	*pOffsetX = ax;
	*pOffsetY = ay;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

HRESULT CaptureSource::CaptureStream::StartAt(
	/* [annotation][in] */
	_In_opt_  const REFERENCE_TIME *ptStart,
	/* [in] */ DWORD dwCookie)
{

	return NOERROR;
}

 HRESULT CaptureSource::CaptureStream::StopAt(
	/* [annotation][in] */
	_In_opt_  const REFERENCE_TIME *ptStop,
	/* [in] */ BOOL bSendExtra,
	/* [in] */ DWORD dwCookie)
 {

	 return NOERROR;
 }

 HRESULT CaptureSource::CaptureStream::GetInfo(
	/* [annotation][out] */
	_Out_  AM_STREAM_INFO *pInfo)
 {
	 return NOERROR;

 }
////

 HRESULT CaptureSource::CaptureStream::GetPushSourceFlags(
	/* [annotation][out] */
	_Out_  ULONG *pFlags)
 {
	 *pFlags = AM_PUSHSOURCECAPS_INTERNAL_RM;
	 return NOERROR;

 }

 HRESULT CaptureSource::CaptureStream::SetPushSourceFlags(
	 /* [in] */ ULONG Flags)
 {
	 return E_NOTIMPL;

 }

 HRESULT CaptureSource::CaptureStream::SetStreamOffset(
	 /* [in] */ REFERENCE_TIME rtOffset)
 {
	 return NOERROR;

 }

 HRESULT CaptureSource::CaptureStream::GetStreamOffset(
	/* [annotation][out] */
	_Out_  REFERENCE_TIME *prtOffset)
 {
	 *prtOffset = 0;
	 return NOERROR;
 }

 HRESULT CaptureSource::CaptureStream::GetMaxStreamOffset(
	/* [annotation][out] */
	_Out_  REFERENCE_TIME *prtMaxOffset)
 {
	 *prtMaxOffset = 0;
	 return NOERROR;

 }

 HRESULT CaptureSource::CaptureStream::SetMaxStreamOffset(
	 /* [in] */ REFERENCE_TIME rtMaxOffset)
 {
	 return NOERROR;
 }