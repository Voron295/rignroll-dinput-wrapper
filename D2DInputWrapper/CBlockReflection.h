#include "CSimpleBlock.h"

class CBlockReflectionItem
{
public:
	struct
	{
		CVector pos;
		CVector normal;
		float u;
		float v;
	} m_pVertices[65];
	__int16 m_pIndices[64];
	int m_dwIndicesCount;
	int m_dwVerticesCount;

	virtual ~CBlockReflectionItem();
	void Render(IDirect3DDevice9 *device);
	
};


class CBlockReflectionStack
{
public:
	CBlockReflectionItem *m_pReflections[100];
	int m_dwReflectionsCount;

	CBlockReflectionItem *PopReflection();
};


class CBlockReflection : public CSimpleBlock
{
public:
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	CBlockReflectionStack *reflectionsStack;
	Material *m_pRefl3Material;
	Material *m_pRefl32Material;
	GameModule *m_pTrucksModule;
	IDirect3DDevice9 *m_pD3DDevice;

	static void InitHooks();

	// Hooks
	static int __fastcall RenderHook(CBlockReflection *block, int nop, int a2, class D3DRenderData *a3);
};

