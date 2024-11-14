#include "CBlock0x1F.h"
#include "CPatch.h"

void CBlock0x1F::InitHooks()
{
	CPatch::RedirectJump(0x5FA720, (void*)&CBlock0x1F::RenderHook);
}

void __fastcall CBlock0x1F::RenderHook(CBlock0x1F *block, int nop, int a2, D3DRenderData *a3)
{

}