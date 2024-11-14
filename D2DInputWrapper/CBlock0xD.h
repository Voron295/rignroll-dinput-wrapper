#ifndef CBLOCK0XD_CLASS_H
#define CBLOCK0XD_CLASS_H

#include "CBlockContainer0x9.h"

#pragma pack(push, 1)
struct CBlock0xDSub
{
	int flags;
	int field_4;
	int count;
	int *data;
};
#pragma pack(pop)


struct CBlock0xD : public CSimpleBlock
{
public:
	CBlock0xDSub field_30;
	CBlockContainer0x9Sub data;
};



#endif