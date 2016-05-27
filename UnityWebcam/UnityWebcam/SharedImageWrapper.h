#ifndef HELLOLIB_H__
#define HELLOLIB_H__

#include "SharedImageMemory.h"
#include <chrono>

namespace mray
{

class  SharedImageWrapper {
private:
	long long totalTime;
	std::chrono::system_clock::time_point _prev, _start;
	SharedImageMemory* m_sender;
public:
	SharedImageWrapper();
	virtual ~SharedImageWrapper();
	bool SendImage(const unsigned char* data, int width, int height);
	bool Ready();
};
}


#endif