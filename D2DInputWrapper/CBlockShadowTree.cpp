#include "CBlockShadowTree.h"
#include "CPatch.h"

void CBlockShadowTree::InitHooks()
{
	CPatch::RedirectJump(0x5CE474, CBlockShadowTree::RenderHookAsm);
}

__declspec(naked) void __fastcall CBlockShadowTree::RenderHookAsm()
{
	__asm
	{
		mov ecx, ebx
		call CBlockShadowTree::RenderHook
		mov eax, 0x5CE4AC
		jmp eax
	}
}

void __fastcall CBlockShadowTree::RenderHook(CBlockShadowTree *block)
{
	g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDirect3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, block->m_pVertices, sizeof(block->m_pVertices[0]));
}