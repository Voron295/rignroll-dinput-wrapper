#include "CBlockDrops.h"
#include "D3DRenderData.h"
#include "CPatch.h"

void CBlockDrops::InitHooks()
{
	CPatch::RedirectJump(0x4C2840, CBlockDrops::RenderHook); // CBlockDrops
	CPatch::RedirectJump(0x4C2A30, CBlockDrops::RenderHook2); // CBlockDrops
	CPatch::RedirectJump(0x4C0CB0, CBlockDrops::RenderHook2); // CRain
	CPatch::RedirectJump(0x4C0DA0, CBlockDrops::RenderHookSnow); // CSnow
}

int __fastcall CBlockDrops::RenderHook(CBlockDrops *block, int nop, int a2, D3DRenderData *a3)
{
	if (block->m_dwElementsCount)
	{
		block->m_pMaterial->m_fTransparency = 1.0;
		block->m_pMaterial->flags |= 0x40000;
		block->m_pMaterial->flags &= 0xFFFFFFFD;
		block->m_pMaterial->flags |= MATERIAL_IS_TRANSPARENT;
		a3->ApplyMaterial(block->m_pMaterial, 0, 0, 0);
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
		for (int i = 0; i < block->m_dwElementsCount; i += 1023)
		{
			int count = 341;
			if (block->m_dwElementsCount - i < 1023)
				count = (block->m_dwElementsCount - i) / 3;
			g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count, &block->m_pRenderData[i], sizeof(CVector)+12);
		}
	}
	return block->RenderNoElements(a2, a3);
}

int __fastcall CBlockDrops::RenderHook2(CBlockDrops *block, int nop, int a2, D3DRenderData *a3)
{
	if (block->m_dwElementsCount)
	{
		a3->ApplyMaterial(block->m_pMaterial, 0, 0, 0);
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

		for (int i = 0; i < block->m_dwElementsCount; i += 1023)
		{
			int count = 341;
			if (block->m_dwElementsCount - i < 1023)
				count = (block->m_dwElementsCount - i) / 3;
			g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count, &block->m_pRenderData[i], sizeof(CVector)+12);
		}
	}
	return block->RenderNoElements(a2, a3);
}

int __fastcall CBlockDrops::RenderHookSnow(CBlockDrops *block, int nop, int a2, D3DRenderData *a3)
{
	if (block->m_dwElementsCount)
	{
		a3->ApplyMaterial(block->m_pMaterial, 0, 0, 0);
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

		for (int i = 0; i < block->m_dwElementsCount; i += 900)
		{
			int count = 300;
			if (block->m_dwElementsCount - i < 900)
				count = block->m_dwElementsCount - i;
			g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count, &block->m_pRenderData[i], sizeof(CVector)+12);
		}
	}
	return block->RenderNoElements(a2, a3);
}

int CBlockDrops::RenderNoElements(int a2, D3DRenderData *a3)
{
	int result = CBlockContainer0x5::RenderHook(this, 0, a2, a3);
	m_pMaterial->m_fTransparency = 1.0;
	m_pMaterial->flags |= 0x40000;
	m_pMaterial->flags |= MATERIAL_HAS_TTX;
	m_dwElementsCount = 0;
	return result;
}