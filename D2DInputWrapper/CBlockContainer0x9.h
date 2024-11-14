#ifndef CBLOCKCONTAINER0X9_CLASS_H
#define CBLOCKCONTAINER0X9_CLASS_H

#include "CBlockContainer.h"

#pragma pack(push, 1)
struct CBlockContainer0x9Sub
{
	int type;
	int count;
	void *data;
};
#pragma pack(pop)


class CBlockContainer0x9 : public CBlockContainer
{
public:
	CBlockContainer0x9Sub data;
};

#endif