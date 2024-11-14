#include "CBlockContainer.h"
#include "D3DRenderData.h"

class CBlockContainerLamp : public CBlockContainer
{
public:
	UnkStruct0x80 field_38;
	int field_88;
	int field_8C;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockContainerLamp *block, int nop, int a2, D3DRenderData *a3);
};