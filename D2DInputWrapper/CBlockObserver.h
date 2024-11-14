#ifndef CBLOCKOBSERVER_CLASS_H
#define CBLOCKOBSERVER_CLASS_H

#include "CSimpleBlock.h"

class CBlockObserver : public Node
{
public:
	char m_sName[32];
	char m_sRoom[32];
	class CBlockRoom *m_pRoomBlock;
	class CBlockSpace *m_pRootBlock;
	float field_54;
	float field_58;
	int field_5C;
	int field_60;
	int m_dwCullDirection;

};

#endif