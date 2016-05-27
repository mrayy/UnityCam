#ifndef __SHAREDIMAGEMEMORY__
#define __SHAREDIMAGEMEMORY__

#include <SharedMemData.h>
class SharedMutex;


namespace mray
{

class SharedImageMemory
{
public:

	SharedImageMemory(const wchar_t* memName, const wchar_t* writeEventName, const wchar_t* readEventName);

	virtual ~SharedImageMemory(void);

	virtual HRESULT Send(LONGLONG time, int width, int height, const unsigned char* buffer);

private:

	HRESULT Open();
	std::wstring m_memName;
	std::wstring m_writeEventName;
	std::wstring m_readEventName;
	HANDLE m_hWriteEvent;
	SharedMutex* m_pMutex;
	HANDLE m_hSharedFile;
	SharedMemHeader* m_pSharedBuf;

};

}


#endif
