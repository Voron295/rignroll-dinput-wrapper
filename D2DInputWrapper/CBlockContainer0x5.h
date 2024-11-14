#ifndef CBLOCKCONTAINER0x5_CLASS_H
#define CBLOCKCONTAINER0x5_CLASS_H

#include "CBlockContainer.h"

class CBlockContainer0x5 : public CBlockContainer
{
public:
	int field_38;
	int field_3C;

	//virtual int Render(int a2, D3DRenderData *a3);
	//void RenderChild(int id, int flags, D3DRenderData *a3);

	static void InitHooks();

	// Hooks
	static int __fastcall RenderHook(CBlockContainer0x5 *block, int nop, int a2, class D3DRenderData *a3);
};

#endif