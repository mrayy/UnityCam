#pragma once

#include <cstdint>

struct SharedMemHeader {

	int32_t maxSize;
	int32_t size;

	unsigned char bufferPtr[1];

};

struct BufferHeader {
	int32_t width;
	int32_t height;
	LONGLONG timestamp;
};