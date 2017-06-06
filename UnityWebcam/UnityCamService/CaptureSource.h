#pragma once

#include "Globals.h"
#include "ICamSource.h"

#include <chrono>
#include <functional>

class CaptureSharedMemory;

extern const AMOVIESETUP_FILTER sudCaptureSource;
extern const AMOVIESETUP_PIN sudCaptureSourceOut;
#define DECLARE_PTR(type, ptr, expr) type* ptr = (type*)(expr);

class CaptureSource :
	public CSource, public IQualityControl, public ICamSource
{
public:

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:

	CaptureSource(LPUNKNOWN lpunk, HRESULT* phr);

public:

	virtual ~CaptureSource(void);

	DECLARE_IUNKNOWN;

    virtual STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	virtual STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q);

	virtual STDMETHODIMP SetSink(IQualityControl *piqc);

private:
	HRESULT Init(CaptureSharedMemory* pReceiver, int fps);

	class CaptureStream : public CSourceStream, public IAMStreamControl, public IKsPropertySet, public IAMStreamConfig, public IAMPushSource, public IAMLatency
	{
	public:

		CaptureStream(CaptureSource* pOwner, CaptureSharedMemory* pReceiver, int fps, HRESULT* phr);

		virtual ~CaptureStream();

		//////////////////////////////////////////////////////////////////////////
		//  IUnknown
		//////////////////////////////////////////////////////////////////////////
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef() { return GetOwner()->AddRef(); }                                                          
		STDMETHODIMP_(ULONG) Release() { return GetOwner()->Release(); }

		virtual STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

		virtual STDMETHODIMP QuerySupported(REFGUID rguidPropSet, ULONG ulId, PULONG pulTypeSupport);

		virtual STDMETHODIMP Get(REFGUID rguidPropSet, ULONG ulId, LPVOID pInstanceData, ULONG ulInstanceLength, LPVOID pPropertyData, ULONG ulDataLength, PULONG pulBytesReturned);

		virtual STDMETHODIMP Set(REFGUID rguidPropSet, ULONG ulId, LPVOID pInstanceData, ULONG ulInstanceLength, LPVOID pPropertyData, ULONG ulDataLength);

		virtual HRESULT FillBuffer(IMediaSample *pSamp);

		virtual HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pRequest);

		virtual STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q);

		virtual STDMETHODIMP SetSink(IQualityControl *piqc);

        virtual STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt);
        
        virtual STDMETHODIMP GetFormat(AM_MEDIA_TYPE **ppmt);
        
        virtual STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize);
        
		virtual STDMETHODIMP GetStreamCaps(int iIndex, AM_MEDIA_TYPE **ppmt, BYTE *pSCC);

		HRESULT CheckMediaType(const CMediaType *pMediaType);
		HRESULT SetMediaType(const CMediaType *pmt);

		//////////////////////////////////////////////////////////////////////////

		virtual HRESULT STDMETHODCALLTYPE StartAt(
			/* [annotation][in] */
			_In_opt_  const REFERENCE_TIME *ptStart,
			/* [in] */ DWORD dwCookie) ;

		virtual HRESULT STDMETHODCALLTYPE StopAt(
			/* [annotation][in] */
			_In_opt_  const REFERENCE_TIME *ptStop,
			/* [in] */ BOOL bSendExtra,
			/* [in] */ DWORD dwCookie) ;

		virtual HRESULT STDMETHODCALLTYPE GetInfo(
			/* [annotation][out] */
			_Out_  AM_STREAM_INFO *pInfo) ;

// IAMPushSource

		virtual HRESULT STDMETHODCALLTYPE GetPushSourceFlags(
			/* [annotation][out] */
			_Out_  ULONG *pFlags) ;

		virtual HRESULT STDMETHODCALLTYPE SetPushSourceFlags(
			/* [in] */ ULONG Flags) ;

		virtual HRESULT STDMETHODCALLTYPE SetStreamOffset(
			/* [in] */ REFERENCE_TIME rtOffset) ;

		virtual HRESULT STDMETHODCALLTYPE GetStreamOffset(
			/* [annotation][out] */
			_Out_  REFERENCE_TIME *prtOffset) ;

		virtual HRESULT STDMETHODCALLTYPE GetMaxStreamOffset(
			/* [annotation][out] */
			_Out_  REFERENCE_TIME *prtMaxOffset) ;

		virtual HRESULT STDMETHODCALLTYPE SetMaxStreamOffset(
			/* [in] */ REFERENCE_TIME rtMaxOffset) ;


		virtual HRESULT STDMETHODCALLTYPE GetLatency(
			/* [annotation][in] */
			_Out_  REFERENCE_TIME *prtLatency)
		{

			return NOERROR;
		}
	protected:

	    virtual HRESULT GetMediaType(/*int iPos,*/CMediaType *pMediaType);

		virtual HRESULT OnThreadStartPlay(void);

	private:
		long long oldStartTime_;

		std::chrono::system_clock::time_point lastTime;
		std::function<bool()> fpsChecker;
		HRESULT GetScaling(int width, int height, double* pScaling, double* pOffsetX, double* pOffsetY);

		LONGLONG m_llTime;

		LONGLONG m_llFrame;

		REFERENCE_TIME m_avgTimePerFrame;

		int m_iPrefWidth;

		int m_iPrefHeight;
		CMediaType m_mt;

		LONGLONG m_llLastSampleTime;

		CaptureSharedMemory* m_pReceiver;

	};

};
