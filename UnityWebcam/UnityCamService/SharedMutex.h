#pragma once

class SharedMutex
{
public:

	SharedMutex(const wchar_t* name, bool create, HRESULT* phr)
	{
		if(create) {
			m_crit = CreateMutex(NULL, FALSE, name);
		}else{
			m_crit = OpenMutex(SYNCHRONIZE, FALSE, name);
		}
		if(m_crit == NULL) {
			*phr = E_UNEXPECTED;
		}
	};

	virtual ~SharedMutex(void)
	{
		if(m_crit != NULL) {
			CloseHandle(m_crit);
			m_crit = NULL;
		}
	};
	
	void Lock()
	{
		WaitForSingleObject(m_crit, INFINITE);
	};
	
	void Unlock()
	{
		ReleaseMutex(m_crit);
	};

private:

	HANDLE m_crit;

};

class CriticalSection
{
public:

	CriticalSection(SharedMutex* pMutex)
	{
		pMutex->Lock();
		m_pMutex = pMutex;
	};

	virtual ~CriticalSection()
	{
		if (m_pMutex != NULL) {
			m_pMutex->Unlock();
			m_pMutex = NULL;
		}
	};

private:

	SharedMutex* m_pMutex;

};
