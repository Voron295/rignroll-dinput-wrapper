#include "CBlockShadowObj.h"
#include "CPatch.h"
#include "D3DRenderData.h"

WORD &g_pCBlockShadowIndices = *(WORD *)0x6F4C00;

void CBlockShadowObj::InitHooks()
{
	CPatch::RedirectJump(0x57C360, (void*)&CBlockShadowObj::RenderHook);
}

void __fastcall CBlockShadowObj::RenderHook(CBlockShadowObj *block, int nop, int a2, D3DRenderData *a3)
{
	DWORD cull;
	g_pDirect3DDevice->GetRenderState(D3DRS_CULLMODE, &cull);
	g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	while (block->m_pStack.shadowsCount)
	{
		CBlockShadowStackItem *shadow = &block->m_pStack.shadows[--block->m_pStack.shadowsCount];
		if (shadow->m_dwVerticesCount && shadow->m_dwIndicesCount && shadow->m_pMaterial)
		{
			a3->ApplyMaterial(shadow->m_pMaterial, 1, 0, 0);
			g_pDirect3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, shadow->m_dwVerticesCount, shadow->m_dwIndicesCount / 3, &g_pCBlockShadowIndices, D3DFMT_INDEX16, shadow->m_pVertices, sizeof(D3DVERTEX));
		}
	}
}