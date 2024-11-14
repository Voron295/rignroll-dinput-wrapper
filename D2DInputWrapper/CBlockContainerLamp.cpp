#include "CBlockContainerLamp.h"
#include "CPatch.h"

void CBlockContainerLamp::InitHooks()
{
	//CPatch::RedirectJump(0x5F3890, (void*)&CBlockContainerLamp::RenderHook);
}

void __fastcall CBlockContainerLamp::RenderHook(CBlockContainerLamp *block, int nop, int a2, D3DRenderData *a3)
{

}