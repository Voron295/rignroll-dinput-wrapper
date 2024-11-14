#ifndef CBLOCKCONTAINERENV_CLASS_H
#define CBLOCKCONTAINERENV_CLASS_H

#include "CBlockContainer.h"

class CBlockContainerEnv : public CBlockContainer
{
public:
	double fogStart;
	double fogEnd;
	double field_48;
	double field_50;
	double field_58;
	int field_60;
	int field_64;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockContainerEnv *block, int nop, int a2, D3DRenderData *a3);
};

#endif