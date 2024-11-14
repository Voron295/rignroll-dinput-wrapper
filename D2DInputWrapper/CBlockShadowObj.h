#ifndef CBLOCKSHADOWOBJ_CLASS_H
#define CBLOCKSHADOWOBJ_CLASS_H

#include "CBlockGenerator.h"
#include "D3DVERTEX.h"
#include "Material.h"
#include <d3d9.h>

class CBlockShadowStackItem
{
public:
	D3DVERTEX m_pVertices[216];
	int m_dwIndicesCount;
	int m_dwVerticesCount;
	Material *m_pMaterial;
	int field_1B0C;
};

class CBlockShadowStack
{
public:
	CBlockShadowStackItem *shadows;
	int shadowsCount;
};


class CBlockShadowObj : public CBlockGenerator
{
public:
	CBlockShadowStack m_pStack;
	IDirect3DDevice9 *m_pD3DDevice;

	static void InitHooks();

	// Hooks
	static void __fastcall RenderHook(CBlockShadowObj *block, int nop, int a2, D3DRenderData *a3);
};

#endif