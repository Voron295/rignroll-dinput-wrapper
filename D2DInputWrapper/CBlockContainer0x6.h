#ifndef CBLOCKCONTAINER0X6_CLASS_H
#define CBLOCKCONTAINER0X6_CLASS_H

#include "CBlockSpace.h"
#include "CFace.h"

class CBlockContainer0x6 : public CBlockContainer
{
public:
	int field_38;
	int m_dwVerticesCount;
	CVertex *m_pVertices;
	CVector *m_pVertexNormals;
	PrimitiveRenderData m_pPrimitiveRenderData;
	int flag_0x6_or_0x24;
	CBlockSpace *field_70;
	int field_74;

	static void InitHooks();

	// Hooks
	static void __fastcall InitPrimitiveDataHook(CBlockContainer0x6 *block, int nop, void *a2);
	static int __fastcall RenderHook(CBlockContainer0x6 *block, int nop, int a2, D3DRenderData *a3);
};

static_assert (sizeof(CBlockContainer0x6) == 0x78, "Size is not correct");

#endif