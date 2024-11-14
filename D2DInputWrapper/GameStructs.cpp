#include "GameStructs.h"
#include "CLog.h"

KeyState *g_pKeyStates = (KeyState*)0x6D1968;

CMatrix *(__cdecl *CMatrixMultiply)(CMatrix *out, CMatrix *a1, CMatrix *a2) = (CMatrix *(__cdecl *)(CMatrix *, CMatrix *, CMatrix *))0x5AB9C0;
CMatrix *(__cdecl *CMatrixSetPos)(CMatrix *out, float x, float y, float z) = (CMatrix *(__cdecl *)(CMatrix *, float, float, float))0x5ABBC0;
CMatrix *(__cdecl *CMatrixSetRotX)(CMatrix *out, float val) = (CMatrix *(__cdecl *)(CMatrix *, float))0x5AC040;
CMatrix *(__cdecl *CMatrixSetRotY)(CMatrix *out, float val) = (CMatrix *(__cdecl *)(CMatrix *, float))0x5ABE60;
CMatrix *(__cdecl *CMatrixSetRotZ)(CMatrix *out, float val) = (CMatrix *(__cdecl *)(CMatrix *, float))0x5ABF50;
void(__cdecl *RenderText)(void *surface, int x, int y, void *font, char *text, unsigned int color, int isRealColor) = (void(__cdecl *)(void *surface, int x, int y, void *font, char *text, unsigned int color, int isRealColor))0x5EA110;
CVector *(__cdecl *CVectorMulMatrix)(CVector *out, CVector *inV, CMatrix *inM) = (CVector *(__cdecl *)(CVector *, CVector *, CMatrix *))0x43F820;

HWND &g_hWnd = *(HWND*)0x720940;
void *&g_pCurrentScreenElement = *(void**)0x69686C;
void *&g_pMatrixFont = *(void**)0x6CED4C;
void *&g_pViewer = *(void **)0x6D2098;
IDirect3DDevice7 *&g_pDirect3DDev = *(IDirect3DDevice7 **)0x720928;
int &g_dwCurrentSpeed = *(int *)0x6F3468;
BOOL &g_bGamePause = *(BOOL *)0x6968BC;
void *&g_pTaskPager = *(void **)0x6F3458;
void *&g_pTheGame = *(void **)0x696CC0;
//IDirectDraw7 *&g_pDirectDraw = *(IDirectDraw7 **)0x72092C;

extern CLog *logFile;
IDirectDrawSurface7 *InitSurfaceFromMemory(unsigned char *data, int width, int height, int depth, DDPIXELFORMAT *format)
{
	DDSURFACEDESC2 desc = { 0 };
	desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = width;
	desc.dwHeight = height;
	desc.ddpfPixelFormat = *format;
	desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
	desc.dwSize = sizeof(desc);

	IDirectDrawSurface7 *surface = 0;
	if (g_pDirectDraw->CreateSurface(&desc, &surface, 0) == DD_OK) 
	{
		DDSURFACEDESC2 lockDesc = { 0 };
		lockDesc.dwSize = sizeof(lockDesc);
		lockDesc.dwFlags = DDSD_ALL;
		if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
		{
			for (int i = 0; i < height; i++)
			{
				memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, data + width * i * depth, width * depth);
			}
			surface->Unlock(0);
			return surface;
		}
		else
		{
			surface->Release();
		}
	}
	return 0;
}

void RenderQuad(CVector pos, CVector size, CVector rotationPoint, float rotation, DWORD color)
{
	RenderTexturedQuad(pos, size, rotationPoint, 0, rotation, color);
}

void RenderTexturedQuad(CVector pos, CVector size, CVector rotationPoint, IDirectDrawSurface7 *tex, float rotation, DWORD color)
{
	RenderTexturedQuad(pos, size, rotationPoint, tex, { 0, 0, 0 }, { 1.0f, 1.0f, 0 }, rotation, color);
}

void RenderTexturedQuad(CVector pos, CVector size, CVector rotationPoint, IDirectDrawSurface7 *tex, CVector uv1, CVector uv2, float rotation, DWORD color)
{
	CMatrix rotP, rot;
	CVector x1, x2, y1, y2, ox1, ox2, oy1, oy2;
	CMatrixSetPos(&rotP, -rotationPoint.x, -rotationPoint.y, -rotationPoint.z);
	CMatrixSetRotX(&rot, rotation);

	x1.x = pos.x;
	x1.y = pos.y;
	x1.z = 0;

	x2.x = pos.x + size.x;
	x2.y = pos.y;
	x2.z = 0;

	y1.x = pos.x;
	y1.y = pos.y + size.y;
	y1.z = 0;

	y2.x = pos.x + size.x;
	y2.y = pos.y + size.y;
	y2.z = 0;

	CVectorMulMatrix(&ox1, &x1, &rotP);
	CVectorMulMatrix(&ox2, &x2, &rotP);
	CVectorMulMatrix(&oy1, &y1, &rotP);
	CVectorMulMatrix(&oy2, &y2, &rotP);

	CVectorMulMatrix(&x1, &ox1, &rot);
	CVectorMulMatrix(&x2, &ox2, &rot);
	CVectorMulMatrix(&y1, &oy1, &rot);
	CVectorMulMatrix(&y2, &oy2, &rot);

	CMatrixSetPos(&rotP, rotationPoint.x, rotationPoint.y, rotationPoint.z);
	CVectorMulMatrix(&ox1, &x1, &rotP);
	CVectorMulMatrix(&ox2, &x2, &rotP);
	CVectorMulMatrix(&oy1, &y1, &rotP);
	CVectorMulMatrix(&oy2, &y2, &rotP);

	TLVERTEX vertices[4];
	for (int i = 0; i < 4; i++)
	{
		vertices[i].diffuse = color;
		vertices[i].specular = 0xFF000000;
		vertices[i].w = 1.0f;
		vertices[i].z = .0001f;
	}
	vertices[0].x = ox1.x;
	vertices[0].y = ox1.y;
	vertices[0].u = uv1.x;
	vertices[0].v = uv1.y;
	vertices[1].x = ox2.x;
	vertices[1].y = ox2.y;
	vertices[1].u = uv2.x;
	vertices[1].v = uv1.y;
	vertices[2].x = oy2.x;
	vertices[2].y = oy2.y;
	vertices[2].u = uv2.x;
	vertices[2].v = uv2.y;
	vertices[3].x = oy1.x;
	vertices[3].y = oy1.y;
	vertices[3].u = uv1.x;
	vertices[3].v = uv2.y;
	DWORD err = 0;
	if (g_pDirect3DDev)
	{
		g_pDirect3DDev->SetTexture(0, tex);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
		//g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, TRUE);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, 1);
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

		g_pDirect3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vertices, 4, 0);
		g_pDirect3DDev->SetTexture(0, 0);
	}
}

void RenderTriangle(CVector top, CVector left, CVector right, CVector rotationPoint, float rotation, DWORD color)
{
	CMatrix rotP, rot;
	CVector oTop, oLeft, oRight;
	CMatrixSetPos(&rotP, -rotationPoint.x, -rotationPoint.y, -rotationPoint.z);
	CMatrixSetRotX(&rot, rotation);

	CVectorMulMatrix(&oTop, &top, &rotP);
	CVectorMulMatrix(&oLeft, &left, &rotP);
	CVectorMulMatrix(&oRight, &right, &rotP);

	CVectorMulMatrix(&top, &oTop, &rot);
	CVectorMulMatrix(&left, &oLeft, &rot);
	CVectorMulMatrix(&right, &oRight, &rot);

	CMatrixSetPos(&rotP, rotationPoint.x, rotationPoint.y, rotationPoint.z);
	CVectorMulMatrix(&oTop, &top, &rotP);
	CVectorMulMatrix(&oLeft, &left, &rotP);
	CVectorMulMatrix(&oRight, &right, &rotP);

	TLVERTEX vertices[3];
	for (int i = 0; i < 3; i++)
	{
		vertices[i].diffuse = color;
		vertices[i].specular = 0xFF000000;
		vertices[i].u = 0;
		vertices[i].v = 0;
		vertices[i].w = 1.0f;
		vertices[i].z = .0001f;
	}
	vertices[0].x = oTop.x;
	vertices[0].y = oTop.y;
	vertices[1].x = oRight.x;
	vertices[1].y = oRight.y;
	vertices[2].x = oLeft.x;
	vertices[2].y = oLeft.y;
	DWORD err = 0;
	if (g_pDirect3DDev)
	{

		//((void(__cdecl *)(int))0x5B00B0)(1);
		//err = g_pDirect3DDev->BeginScene();
		err = g_pDirect3DDev->SetTexture(0, 0);
		err = g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLOROP, 1);
		err = g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, 1);
		err = g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
		DWORD state = 0, alpha = 0;
		err = g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		err = g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		err = g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
		err = g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		err = g_pDirect3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vertices, 3, 0);
		//err = g_pDirect3DDev->EndScene();
	}
}