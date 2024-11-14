#include "CBlockContainer0x5.h"

struct CBlockPeopleItem
{
	struct
	{
		CVector pos;
		CVector normal;
		float u;
		float v;
	} m_pVertices[4];
};

class CBlockPeople : public CBlockContainer0x5
{
public:
	CBlockPeopleItem m_pItems[100];
	int field_3240;
	int field_3244;
	int field_3248;
	int field_324C;
	int field_3250;
	int field_3254;
	int field_3258;
	int field_325C;
	Material *m_pItemMaterials[100];
	int m_dwItemsCount;

	static void InitHooks();

	// Hooks
	static int __fastcall RenderHook(CBlockPeople *block, int nop, int a2, D3DRenderData *a3);
};