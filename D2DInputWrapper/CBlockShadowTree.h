#include "CBlockContainer0x5.h"


struct CBlockShadowTree : public CBlockContainer0x5
{
public:
	Material *field_40;
	struct 
	{
		CVector pos;
		D3DCOLOR color;
		float u;
		float v;
	} m_pVertices[4];
	int field_A4;
	int field_A8;
	float field_AC;
	float field_B0;
	float field_B4;
	CVector field_B8;
	CVector field_C4;
	float field_D0;
	CVector field_D4;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockShadowTree *block);
	static void __fastcall RenderHookAsm();
};

