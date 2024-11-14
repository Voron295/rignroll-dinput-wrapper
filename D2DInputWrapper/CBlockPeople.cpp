#include "CBlockPeople.h"
#include "CPatch.h"
#include "D3DRenderData.h"

void CBlockPeople::InitHooks()
{
	CPatch::RedirectJump(0x4CCA50, CBlockPeople::RenderHook);
}

int CBlockPeople::RenderHook(CBlockPeople *block, int nop, int a2, D3DRenderData *a3)
{
	if (CBlockContainer0x5::RenderHook(block, 0, a2, a3) && block->m_dwItemsCount)
	{
		DWORD cull;
		g_pDirect3DDevice->GetRenderState(D3DRS_CULLMODE, &cull);
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		for (int i = 0; i < block->m_dwItemsCount; i++)
		{
			a3->ApplyMaterial(block->m_pItemMaterials[i], 1, 0, 0);
			g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
			g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, (void*)block->m_pItems[i].m_pVertices, sizeof(CVector)*2+8);
		}
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, cull);
		block->m_dwItemsCount = 0;
		return 1;
	}
	block->m_dwItemsCount = 0;
	return 0;
}