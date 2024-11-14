#ifndef CVEHICLEBLOCK_CLASS_H
#define CVEHICLEBLOCK_CLASS_H

#include "CBlockSpace.h"
#include "D3DRenderData.h"

class CBlockVehicle : public CSimpleBlock
{
public:
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	CBlockSpace *m_pBlockSpace;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	int field_74;

	//virtual int Render(int a2, D3DRenderData *a3);

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockVehicle *block, int nop, int a2, D3DRenderData *a3);
};

#endif