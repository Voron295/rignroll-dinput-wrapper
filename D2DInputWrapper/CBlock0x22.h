#ifndef CBLOCK0X22_CLASS_H
#define CBLOCK0X22_CLASS_H

#include "CSimpleBlock.h"
#include "D3DVERTEX.h"

class CBlock0x22 : public CSimpleBlock
{
public:
	int m_dwCount;
	D3DVERTEX *field_34;
	CVector *m_pVertices;
	int *m_pIndices;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlock0x22 *block, int nop, int a2, D3DRenderData *a3);
};

#endif