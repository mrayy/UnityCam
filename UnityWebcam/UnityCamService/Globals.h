#pragma once

#include <string>


#define CaptureSourceName  L"UnityCam"

DEFINE_GUID(CLSID_UnityCamService,
	0x5c2cd55c, 0x92ad, 0x4999, 0x86, 0x66, 0x91, 0x2b, 0xd3, 0xe7, 0x0, 0x1);

#define CS_EVENT_Base_WRITE	CaptureSourceName L"_Write"
#define CS_EVENT_Base_READ	CaptureSourceName L"_Read"
#define CS_SHARED_Base_PATH	CaptureSourceName L"_Data"
 