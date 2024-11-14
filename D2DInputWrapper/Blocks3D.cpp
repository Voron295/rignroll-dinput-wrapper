#include "Blocks3D.h"
#include "CPatch.h"
#include "CBlockPeople.h"
#include "CBlockContainer0x6.h"
#include "CBlockContainer0x7.h"
#include "CBlockVehicle.h"
#include "CBlockContainerEnv.h"
#include "CBlock0x1C.h"
#include "CBlock0x1F.h"
#include "CBlock0x22.h"
#include "CBlockMarineNode.h"
#include "CBlockModelElement.h"
#include "CBlockShadowObj.h"
#include "CBlockShadowTree.h"
#include "CBlockDrops.h"
#include "CBlockReflection.h"

void __fastcall CBlockContainerTDTN_Render(CBlockContainer0x5 *block, int nop, int a2, D3DRenderData *render)
{
	block->RenderHook(block, nop, a2, render);
}

int __fastcall TaskParticlesBlockRender(char *a1, int nop, char *a2)
{
	IDirect3DDevice9 *device = *(IDirect3DDevice9 **)(a2 + 0x30);
	char *data = (char *)(a1 + 0x3C);
	int count = *(int *)(a1 + 0x24);
	device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	struct
	{
		CVector pos;
		D3DCOLOR color;
	} vertices[100];

	for (int i = 0; i < count; i++)
	{
		vertices[i].pos = (*(CVector**)data)[i];
		vertices[i].color = (*(D3DCOLOR**)(data + 0x10))[i];
	}

	HRESULT result = device->DrawPrimitiveUP(D3DPT_POINTLIST, count, (void*)vertices, sizeof(CVector)+4);
	return result;
}

void Blocks3D::InitHooks()
{
	CBlockContainerEnv::InitHooks();
	CBlockVehicle::InitHooks();
	CBlock0x1C::InitHooks();
	CBlock0x1F::InitHooks();
	CBlock0x22::InitHooks();
	CBlockModelElement::InitHooks();
	CBlockMarineNode::InitHooks();
	CBlockContainer0x5::InitHooks();
	CBlockShadowTree::InitHooks();
	CBlockShadowObj::InitHooks();
	CBlockContainer0x6::InitHooks();
	CBlockContainer0x7::InitHooks();
	CBlockPeople::InitHooks();
	CBlockDrops::InitHooks();
	CBlockReflection::InitHooks();

	CPatch::RedirectJump(0x4BCC00, CBlockContainerTDTN_Render); // CBlockContainerTDTN
	//CPatch::RedirectJump(0x4C0DA0, (void*)0x4C0DBC); // CBlockSnow
	CPatch::RedirectJump(0x5CE900, TaskParticlesBlockRender);

	CPatch::Nop(0x5F8ED2, 2); // Remove render lods
}