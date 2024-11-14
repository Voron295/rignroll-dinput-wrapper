#ifndef CBLOCK0X1F_CLASS_H
#define CBLOCK0X1F_CLASS_H

#include "CSimpleBlock.h"

class CBlock0x1F : public CSimpleBlock
{
public:
	int m_dwTransparency;
	int m_dwAlignType;
	CVector4 field_38;
	int count;
	float *m_pBlendValues;
	int *m_pMasksId;
	CVector m_pPosition;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlock0x1F *block, int nop, int a2, D3DRenderData *a3);
};

#endif