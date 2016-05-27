#include "StdAfx.h"
#include "SharedImageMemory.h"
#include <SharedMutex.h>
#include <cstdint>

namespace mray
{

SharedImageMemory::SharedImageMemory(const wchar_t* memName, const wchar_t* writeEventName, const wchar_t* readEventName)
{
	m_pMutex = NULL;
	m_hSharedFile = NULL;
	m_hWriteEvent = NULL;
	m_pSharedBuf = NULL;
	m_memName = memName;
	m_writeEventName = writeEventName;
	m_readEventName = readEventName;
}

SharedImageMemory::~SharedImageMemory(void)
{
	if(m_pMutex != NULL) {
		delete m_pMutex;
		m_pMutex = NULL;
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

HRESULT SharedImageMemory::Send(LONGLONG time, int width, int height, const unsigned char* buffer)
{
	if(buffer == NULL) 
		return E_POINTER;

	HRESULT hr = Open();
	if (hr != S_OK) 
		return hr;

	{
		CriticalSection cs(m_pMutex);
		int32_t imageSize = width * height*3 ;
		int32_t bufSize = imageSize + sizeof(BufferHeader);
		if(m_pSharedBuf->maxSize < bufSize) {
			return E_UNEXPECTED;
		}
		m_pSharedBuf->size = bufSize;
		BufferHeader* pDesc = (BufferHeader*) m_pSharedBuf->bufferPtr;
		pDesc->timestamp = time;
		pDesc->width = width;
		pDesc->height = height;
		unsigned char* pSharedData = m_pSharedBuf->bufferPtr + sizeof(BufferHeader);
		memcpy(pSharedData, buffer, imageSize);
	}
	SetEvent(m_hWriteEvent);
	return S_OK;
}
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	int32_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

HRESULT SharedImageMemory::Open()
{
	DWORD err=0;
	if(m_hWriteEvent == NULL) {
		m_hWriteEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, m_writeEventName.c_str());
		if(m_hWriteEvent == NULL) {
			return S_FALSE;
		}
	}
	if(m_hSharedFile == NULL) {
		m_hSharedFile = OpenFileMapping(FILE_MAP_WRITE, FALSE, m_memName.c_str());
		if (m_hSharedFile == NULL) {
			return S_FALSE;
		}

		m_pSharedBuf = (SharedMemHeader*)MapViewOfFile(m_hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
		
		if(m_pSharedBuf == NULL) {
			return E_UNEXPECTED;
		}
	}
	if(m_pMutex == NULL) {
		HRESULT hr = S_OK;
		m_pMutex = new SharedMutex(m_readEventName.c_str(), false, & hr);
		if (FAILED(hr)) {
			delete m_pMutex;
			m_pMutex = NULL;
			return S_FALSE;
		}
	}
	return S_OK;
}

}