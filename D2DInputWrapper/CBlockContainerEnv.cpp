#include "CBlockContainerEnv.h"
#include "CPatch.h"

void CBlockContainerEnv::InitHooks()
{
	CPatch::RedirectJump(0x5F3D30, (void*)&CBlockContainerEnv::RenderHook);
}

void __fastcall CBlockContainerEnv::RenderHook(CBlockContainerEnv *block, int nop, int a2, D3DRenderData *a3)
{
	block->RenderChild(0, a2, a3);
}
