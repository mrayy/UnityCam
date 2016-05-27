#include "StdAfx.h"
#include "CaptureSharedMemory.h"
#include "SharedMutex.h"

CaptureSharedMemory::CaptureSharedMemory(const wchar_t* memName, const wchar_t* writeEventName, const wchar_t* readEventName, HRESULT* phr)
{
	m_pMutex = NULL;
	m_iLocalBufSize = 0;
	m_pLocalBuf = NULL;
	m_hSharedFile = NULL;
	m_hWriteEvent = NULL;
	m_pSharedBuf = NULL;

	HRESULT hr = S_OK;
	if(memName == NULL || writeEventName == NULL || readEventName == NULL) {
		*phr = E_POINTER;
		return;
	}

	m_pMutex = new SharedMutex(readEventName, TRUE, & hr);
	if(FAILED(hr)) {
		*phr = hr;
		return;
	}

	CriticalSection cs(m_pMutex);
	m_hSharedFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(SharedMemHeader) + MAX_BUF_SIZE, memName);
	if (m_hSharedFile == NULL) {
		*phr = E_UNEXPECTED;
		return;
	}

	m_pSharedBuf = (SharedMemHeader*)MapViewOfFile(m_hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
	if(m_pSharedBuf == NULL) {
		*phr = E_UNEXPECTED;
		return;
	}
	ZeroMemory(m_pSharedBuf, sizeof(SharedMemHeader)+MAX_BUF_SIZE);
	m_pSharedBuf->maxSize = MAX_BUF_SIZE;
	m_pSharedBuf->size = sizeof(BufferHeader)+3;
	BufferHeader* pBufDesc = (BufferHeader*)m_pSharedBuf->bufferPtr;
	pBufDesc->width = 1;
	pBufDesc->height = 1;
	pBufDesc->timestamp = 0;

	m_hWriteEvent = CreateEvent(NULL, FALSE, FALSE, writeEventName);
	if (m_hWriteEvent == NULL) {
		*phr = E_UNEXPECTED;
		return;
	}
}

CaptureSharedMemory::~CaptureSharedMemory(void)
{
	if(m_pMutex != NULL) {
		delete m_pMutex;
		m_pMutex = NULL;
	}
	if(m_pLocalBuf != NULL) {
		delete[] m_pLocalBuf;
		m_pLocalBuf = NULL;
	}
	if(m_hWriteEvent != NULL) {
		CloseHandle(m_hWriteEvent);
		m_hWriteEvent = NULL;
	}
	if(m_hSharedFile != NULL) {
		CloseHandle(m_hSharedFile);
		m_hSharedFile = NULL;
	}
	m_pSharedBuf = NULL;
}

void CaptureSharedMemory::Destroy()
{
	delete this;
}

HRESULT CaptureSharedMemory::Receive(BufferHeader* pDesc, unsigned char** ppBuffer)
{
	if(pDesc == NULL || ppBuffer == NULL) {
		return E_POINTER;
	}
	DWORD dwEvent = WaitForSingleObject(m_hWriteEvent, 0);
	if (dwEvent == WAIT_OBJECT_0 || m_pLocalBuf == NULL) {
		CriticalSection cs(m_pMutex);
		if(m_iLocalBufSize != m_pSharedBuf->size) {
			if(m_pLocalBuf != NULL) {
				delete[] m_pLocalBuf;
				m_pLocalBuf = NULL;
			}
			//MessageBox(0, L"Receive", L"", NULL);
			m_pLocalBuf = new unsigned char[m_pSharedBuf->size];
			m_iLocalBufSize = m_pSharedBuf->size;
		}
		memcpy(m_pLocalBuf, m_pSharedBuf->bufferPtr, m_iLocalBufSize);
	}

	memcpy(pDesc, m_pLocalBuf, sizeof(BufferHeader));
	*ppBuffer = m_pLocalBuf + sizeof(BufferHeader);
	return S_OK;
}

HRESULT CaptureSharedMemory::NotifyProcessed(unsigned char* pBuffer)
{
	if(pBuffer == NULL) {
		return E_POINTER;
	}
	return S_OK;
}
