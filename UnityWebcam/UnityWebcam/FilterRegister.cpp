

#include "stdafx.h"
#include "FilterRegister.h"
#include <comdef.h>

FilterRegister* FilterRegister::_instance = 0;


FilterRegister::FilterRegister()
{
	_refCount = 0;
	_registered = false;
}
FilterRegister::~FilterRegister()
{
}

void FilterRegister::AddRef()
{
	_refCount++;
	if (_refCount > 0)
	{
		_Register();
	}

}
void FilterRegister::SubRef()
{
	_refCount--;
	if (_refCount <= 0)
	{
		_refCount = 0;
		_UnRegister();
	}

}

//http://www.gdcl.co.uk/2011/June/UnregisteredFilters.htm
typedef HRESULT(STDAPICALLTYPE* FN_DLLGETCLASSOBJECT)(REFCLSID clsid, REFIID iid, void** ppv);

void FilterRegister::_Register()
{
	if (_registered)
		return;

	_registered = true;
}
void FilterRegister::_UnRegister()
{
	if (!_registered)
		return;

	_registered = false;
}

FilterRegister* FilterRegister::Instance()
{
	if (_instance == NULL)
		_instance = new FilterRegister();
	return _instance;
}


