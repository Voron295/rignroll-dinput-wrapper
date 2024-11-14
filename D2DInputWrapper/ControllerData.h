#ifndef CONTROLLERDATA_H
#define CONTROLLERDATA_H

#pragma pack(push, 1)
struct ControllerData
{
	__int16 id;
	__int16 x;
	__int16 y;
	__int16 keyId;
};
#pragma pack(pop)

#endif