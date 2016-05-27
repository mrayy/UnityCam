#include "stdafx.h"
#include "SharedImageWrapper.h"

#include <Globals.h>

using namespace std;

namespace mray
{

SharedImageWrapper::SharedImageWrapper() {
	totalTime = 0;
	m_sender = new SharedImageMemory(CS_SHARED_Base_PATH, CS_EVENT_Base_WRITE, CS_EVENT_Base_READ);
	_start = std::chrono::system_clock::now();
}

SharedImageWrapper::~SharedImageWrapper()
{
	delete m_sender;
}
bool SharedImageWrapper::Ready()
{
	auto now = std::chrono::system_clock::now();
	auto diff = now - _prev;
	long long diffInt = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
	if (diffInt < 1000 / 30)
		return false;
	return true;
}


bool SharedImageWrapper::SendImage(const unsigned char* data, int width, int height) {

	if (!Ready())
		return false;
	auto now = std::chrono::system_clock::now();
	_prev = now;
	long long diffInt = std::chrono::duration_cast<std::chrono::milliseconds>(now - _start).count();

	//push the captured data to directshow filter
	HRESULT hr = m_sender->Send(diffInt, width, height, data);

	return true;
}

}