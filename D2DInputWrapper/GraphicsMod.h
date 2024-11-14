#ifndef GRAPHICS_MOD_H
#define GRAPHICS_MOD_H
//#define D3D_DEBUG_INFO
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "GraphicsModFuncs.h"

extern IDirect3D9 *&g_pDirect3D;
extern IDirect3DDevice9 *&g_pDirect3DDevice;

enum D2RenderFlags
{
	D2RENDER_FOG = 0x4,
	D2RENDER_SHADOW = 0x8,
	D2RENDER_INTENMAP = 0x10,
	D2RENDER_INTENCITY = 0x40,
	D2RENDER_ZBUF = 0x4000,
	D2RENDER_ALPHAMIR = 0x1000000,
	D2RENDER_MULTITEX = 0x2000000,
	D2RENDER_NEWKEYING = 0x4000000,
};


void InitGraphicsModHooks();
bool InitDirectDraw(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem);
void ShutdownDirect3D();

#endif