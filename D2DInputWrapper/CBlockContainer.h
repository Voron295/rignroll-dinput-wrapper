#ifndef CBLOCKCONTAINER_CLASS_H
#define CBLOCKCONTAINER_CLASS_H

#include "CSimpleBlock.h"

class CBlockContainer : public CSimpleBlock
{
public:
	CSimpleBlock **m_pSubBlocks;
	int m_dwSubBlocksCount;


	void RenderChild(int id, int flags, D3DRenderData *a3);
};

#endif