#include "CBlock0x1C.h"
#include "CPatch.h"

void CBlock0x1C::InitHooks()
{
	CPatch::RedirectJump(0x5FA720, (void*)&CBlock0x1C::RenderHook);
}

void __fastcall CBlock0x1C::RenderHook(CBlock0x1C *block, int nop, int a2, D3DRenderData *a3)
{

}