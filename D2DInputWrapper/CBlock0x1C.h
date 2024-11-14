#ifndef CBLOCK0X1C_CLASS_H
#define CBLOCK0X1C_CLASS_H

#include "CSimpleBlock.h"

class CBlock0x1C : public CSimpleBlock
{
public:
	int field_30;
	int field_34;
	float field_38;
	float field_3C;
	float field_40;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlock0x1C *block, int nop, int a2, D3DRenderData *a3);
};

#endif