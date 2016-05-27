#pragma once

class SharedMutex;

#define MAX_BUF_SIZE	2000 * 2000 * 3

#include "SharedMemData.h"

class CaptureSharedMemory
{
public:

	CaptureSharedMemory(const wchar_t* memName, const wchar_t* writeEventName, const wchar_t* readEventName, HRESULT* phr);
	virtual ~CaptureSharedMemory(void);

	virtual void Destroy();

	virtual HRESULT Receive(BufferHeader* pDesc, unsigned char** ppBuffer);

	virtual HRESULT NotifyProcessed(unsigned char* pBuffer);

private:

	HANDLE m_hWriteEvent;
	SharedMutex* m_pMutex;
	HANDLE m_hSharedFile;
	SharedMemHeader* m_pSharedBuf;
	int32_t m_iLocalBufSize;
	unsigned char* m_pLocalBuf;

};
