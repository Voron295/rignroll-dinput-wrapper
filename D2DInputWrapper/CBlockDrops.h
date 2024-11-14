#include "CBlockRenderer.h"

class CBlockDrops : public CBlockRenderer
{
public:
	Material *m_pMaterial;
	struct 
	{
		CVector pos;
		D3DCOLOR color;
		float u;
		float v;
	} *m_pRenderData;

	static void InitHooks();

	// Hooks
	static int __fastcall RenderHook(CBlockDrops *block, int nop, int a2, class D3DRenderData *a3);
	static int __fastcall RenderHook2(CBlockDrops *block, int nop, int a2, class D3DRenderData *a3);
	static int __fastcall RenderHookSnow(CBlockDrops *block, int nop, int a2, class D3DRenderData *a3);
	int RenderNoElements(int a2, D3DRenderData *a3);
};

