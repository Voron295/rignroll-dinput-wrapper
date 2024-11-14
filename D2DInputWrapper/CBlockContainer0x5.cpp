#include "CBlockContainer0x5.h"
#include "CLog.h"
#include "CPatch.h"

extern DWORD &gRenderFlags;

extern CLog *logFile;

void CBlockContainer0x5::InitHooks()
{
	CPatch::RedirectJump(0x5F3690, (void*)&CBlockContainer0x5::RenderHook);
}

int __fastcall CBlockContainer0x5::RenderHook(CBlockContainer0x5 *block, int nop, int a2, D3DRenderData *a3)
{
	int noz = 0;
	*(int*)0x720740 += 1;
	int result = ~block->flags & 1;
	if (result)
	{
		result = ((int(__thiscall *)(D3DRenderData*, CVector*))0x5B4F50)(a3, &block->pos.pos);
		if (result)
		{
			if (block->m_sName && gRenderFlags & 0x4000 && !((int(__cdecl *)(const char*, const char*, int))0x61D670)(block->m_sName, "noz", 3u))
			{
				(*((BYTE*)&(gRenderFlags)+1)) &= 0xBFu;
				noz = 1;
				g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
				g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC, 8);
			}
			block->RenderChild(0, a2, a3);
			if (noz)
			{
				gRenderFlags |= 0x4000u;
				g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, 1);
				g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC, 4);
			}
			result = 1;
		}
	}
	return result;
}