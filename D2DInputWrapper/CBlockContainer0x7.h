#ifndef CBLOCKCONTAINER0x7_CLASS_H
#define CBLOCKCONTAINER0x7_CLASS_H

#include "CBlockContainer.h"
#include "CFace.h"

class CBlockContainer0x7 : public CBlockContainer
{
public:
	int m_dwVerticesCount;
	CVertex *m_pVertices;
	CVector *m_pVertexNormals;
	PrimitiveRenderData m_pPrimitiveRenderData;
	int field_68;
	int flag_0x7_or_0x25;
	int field_70;

	static void InitHooks();

	// Hooks
	static void __fastcall InitPrimitiveDataHook(CBlockContainer0x7 *block, int nop, void *a2);
	static int __fastcall RenderHook(CBlockContainer0x7 *block, int not, int a2, D3DRenderData *a3);
};

static_assert (sizeof(CBlockContainer0x7) == 0x74, "Size is not correct");

#endif