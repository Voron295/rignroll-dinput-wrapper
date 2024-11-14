#ifndef CBLOCKMARINENODE_CLASS_H
#define CBLOCKMARINENODE_CLASS_H

#include "CSimpleBlock.h"
#include "TheGame.h"

class CBlockMarineNode : public CSimpleBlock
{
public:
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	Task field_4C;
	int field_6C;
	TheGame *m_pTheGame;
	int field_74;
	int field_78;
	int field_7C;
	int field_80;
	int field_84;
	CVector4 field_88;
	Material *field_98;
	float field_9C;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockMarineNode *block, int nop, int a2, D3DRenderData *a3);
};

#endif