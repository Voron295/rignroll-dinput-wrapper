#include "CBlockReflection.h"
#include "D3DRenderData.h"
#include "CPatch.h"

void CBlockReflection::InitHooks()
{
	CPatch::RedirectJump(0x4C8A50, CBlockReflection::RenderHook); // CBlockReflection
}

CBlockReflectionItem::~CBlockReflectionItem()
{
	
}

void CBlockReflectionItem::Render(IDirect3DDevice9 *device)
{
	if (m_dwVerticesCount && m_dwIndicesCount)
	{
		device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwVerticesCount, m_dwIndicesCount / 3, m_pIndices, D3DFMT_INDEX16, m_pVertices, sizeof(CVector)* 2 + 8);
	}
}

CBlockReflectionItem *CBlockReflectionStack::PopReflection()
{
	if (m_dwReflectionsCount > 0)
	{
		return m_pReflections[--m_dwReflectionsCount];
	}
	return 0;
}

int __fastcall CBlockReflection::RenderHook(CBlockReflection *block, int nop, int a2, class D3DRenderData *a3)
{
	if (block->reflectionsStack->m_dwReflectionsCount == 0)
		return 0;

	DWORD cull;
	block->m_pD3DDevice->GetRenderState(D3DRS_CULLMODE, &cull);
	block->m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	a3->vmt->SetupFromGameModule(a3, block->m_pTrucksModule);
	a3->ApplyMaterial(block->m_pRefl3Material, 0, 0, 0);
	while (block->reflectionsStack->m_dwReflectionsCount != 0)
	{
		CBlockReflectionItem *item = block->reflectionsStack->PopReflection();
		item->Render(block->m_pD3DDevice);
		item->~CBlockReflectionItem();
	}
	a3->vmt->SetupFromGameModule(a3, block->m_pModule);
	block->m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, cull);
	return 1;
}
