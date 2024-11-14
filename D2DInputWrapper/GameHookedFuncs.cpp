#include "CWinApp.h"
#include "GameHookedFuncs.h"

extern IDirect3D9 *&g_pDirect3D;
extern IDirect3DDevice9 *&g_pDirect3DDevice;
extern IDirect3DDevice9 *g_pD3DDevice;
extern int &g_iWindowWidth;
extern int &g_iWindowHeight;
extern HWND &g_hWnd;
extern int &gDepth;
extern int &gDepthTextures;
extern DWORD &gRenderFlags;

extern CWinApp *&g_pWinApp;
extern CWinApp &g_WinApp;

char *gExePath[2];

void FirstGameInit()
{
	((void(__thiscall *)(CWinApp*))0x63B3E9)(&g_WinApp); // CWinApp::SetCurrentHandles

	g_WinApp.width = 800;
	g_WinApp.height = 600;
	g_WinApp.depth = 3;
	g_WinApp._flags = (eGameDrawFlags)0x3E00;

	g_iWindowWidth = 800;
	g_iWindowHeight = 600;
	gDepth = 3;
	gDepthTextures = 3;

	((int(__cdecl *)())0x6036C0)(); // InitTickCounter
	if (g_WinApp.m_bNeedToInitDirectSound && ((int(__cdecl *)())0x5FCE20)()) // InitDirectSound
		g_WinApp.m_bNeedToInitDirectSound = 0;


	ScreenElement *mainScrElem = ScreenElement::Create(g_iWindowWidth, g_iWindowHeight, 3, 0x3E00);
	g_WinApp.pCurrentScreenElement = mainScrElem;
	g_WinApp.m_pGameStruct = mainScrElem;
	g_WinApp.m_pTasks = mainScrElem;
	g_WinApp.m_pSprites = mainScrElem;

	((int(__cdecl *)(LPCSTR, LPCSTR, double, double *, LPCSTR))0x603B80)("RENDER", "scale_time", (double)700 * 0.005, &g_WinApp.field_170, g_WinApp.m_sConfigFileName); // GetPrivateProfileFloat

	g_WinApp.window = g_hWnd;

	//char *pathToFile = ((char *(__cdecl *)(char *))0x5E4280)(g_WinApp.hlpFilePath);
	int sLen = strlen(g_WinApp.hlpFilePath);
	gExePath[0] = new char[sLen + 1];
	strcpy(gExePath[0], g_WinApp.hlpFilePath);
	gExePath[0][sLen - 1] = 'e';
	gExePath[0][sLen - 2] = 'x';
	gExePath[0][sLen - 3] = 'e';

	gExePath[1] = "";

	g_WinApp._base._vmt->StartGame(&g_WinApp, 1, gExePath);

	g_WinApp.field_148 = 1;

	g_WinApp.m_pMenuFont = ((Font *(__cdecl *)(char *fileName))0x5E8C00)("menu.fon");

	g_WinApp.m_bNetIsOn = false;

	//((void(__cdecl*)(char *, char*))0x510970)("COMMON\\", "IND");
	g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
}

void __fastcall Direct3DSetFog(D3DRenderData *a1, int nop, double fogstart, double fogend, int a6)
{
	if (gRenderFlags & 4)
	{
		g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, 1);
		GraphicsPalette *plt = a1->m_pGraphicsData->GetPalette();
		if (plt)
		{
			g_pDirect3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(plt->palette[a6].r, plt->palette[a6].g, plt->palette[a6].b));
		}
		float start = fogstart;
		float end = fogend;
		g_pDirect3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
		g_pDirect3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&start);
		g_pDirect3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&end);
	}
	else
	{
		g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, 0);
	}
	
	return;
}

void __fastcall Direct3DSetAmbient(D3DRenderData *a1, int nop, double r, double g, double b)
{
	if (g < 0.0)
		g = r;
	if (b < 0.0)
		b = r;
	g_pDirect3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((BYTE)(r * 255.0), (BYTE)(g * 255.0), (BYTE)(b * 255.0)));
	return;
}

void __fastcall sub_5ADE30(D3DRenderData *a1)
{
	D3DRenderData *v1; // esi
	int v2; // edx
	LONG v3; // ebx
	LONG v4; // ebp
	int v5; // edi
	DWORD v6; // eax
	DWORD v7; // ecx
	int v8; // edx
	LONG v9; // eax
	DWORD v10; // eax
	int v11; // edi
	CMatrix *v12; // ecx
	double v13; // st7
	double v14; // st6
	float v15; // edx
	float v16; // eax
	RECT a2; // [esp+4h] [ebp-68h]
	D3DVIEWPORT9 v18; // [esp+14h] [ebp-58h]
	D3DMATRIX v17; // [esp+2Ch] [ebp-40h]

	v1 = a1;
	if (g_pDirect3DDevice)
	{
		((void(__thiscall *)(GraphicsData*, RECT*))0x5B66A0)(a1->m_pGraphicsData, &a2);
		v2 = v1->field_7A4;
		v3 = a2.left;
		v4 = a2.top;
		memset(&v18, 0, sizeof(v18));
		v5 = v1->field_7AC;
		v6 = (v1->field_7A0 >> 20) + a2.left;
		v7 = (v2 >> 20) + a2.top;
		v8 = v1->field_7A8 >> 20;
		v1->field_2164 = v6;
		++v8;
		v18.X = v6;
		v9 = a2.right;
		v1->field_2168 = v7;
		v18.Y = v7;
		v10 = v9 - v3;
		v11 = (v5 >> 20) + 1;
		v1->field_216C = v8;
		v1->field_2170 = v11;
		v18.Width = 2 * v8;
		if (2 * v8 > v10)
			v18.Width = v10;
		v18.Height = 2 * v11;
		if (2 * v11 > (unsigned int)(a2.bottom - v4))
			v18.Height = a2.bottom - v4;
		v18.MaxZ = 1.0;
		v18.MinZ = 0.0;
		//g_pDirect3DDevice->SetViewport(&v18);
		v12 = &v1->field_10A0[v1->field_2024];
		if (v1->field_2110 <= 0.0001)
		{
			v17.m[0][0] = 1.0;
			v17.m[0][1] = 0.0;
			v17.m[0][2] = 0.0;
			v17.m[0][3] = 0.0;
			v17.m[1][0] = 0.0;
			v17.m[1][1] = 1.0;
			v17.m[1][2] = 0.0;
			v17.m[1][3] = 0.0;
			v17.m[2][0] = 0.0;
			v17.m[2][1] = 0.0;
			v17.m[2][2] = 1.0;
			v17.m[2][3] = 0.0;
			v17.m[3][0] = 0.0;
			v17.m[3][1] = 0.0;
			v17.m[3][2] = 0.0;
			v17.m[3][3] = 1.0;
		}
		else
		{
			v13 = v1->field_2118 - v1->field_2110;
			v14 = v1->field_2118;

			v17.m[0][0] = v12->right.x;
			v17.m[0][1] = 0.0;
			v17.m[0][2] = 0.0;
			v17.m[0][3] = 0.0;
			v17.m[1][0] = 0.0;
			v17.m[1][1] = v12->at.z;
			v17.m[1][2] = 0.0;
			v17.m[1][3] = 0.0;
			v17.m[2][0] = v12->top.x;
			v17.m[2][1] = v12->top.z;
			v17.m[2][2] = v14 / v13;
			v17.m[2][3] = 1.0;
			v17.m[3][0] = 0.0;
			v17.m[3][1] = 0.0;
			v17.m[3][2] = -(v1->field_2110 * v1->field_2118 / v13);
			v17.m[3][3] = 0.0;
		}
		g_pDirect3DDevice->SetTransform(D3DTS_PROJECTION, &v17);
	}
}

__declspec(naked) void SetTransformHook1()
{
	__asm
	{
		mov ecx, [eax]
		push edx
		push 2
		push eax
		call dword ptr[ecx+0xB0]

		pop edi
		pop esi
		pop ebp
		pop ebx
		add esp, 0x110
		retn 8
	}
}

__declspec(naked) void SetTransformHook2()
{
	__asm
	{
		mov     eax, [esi + 0x64]
		add     esp, 4
		test    eax, eax
		mov     eax, g_pD3DDevice
		jz      short loc_5EB3F0
		mov     edx, [eax]
		push    3
		push    0x16
		push    eax
		call    dword ptr[edx + 0xE4]
		jmp     short loc_5EB3FA

	loc_5EB3F0:
		mov     ecx, [eax]
		push    2
		push    0x16
		push    eax
		call    dword ptr[ecx + 0xE4]

	loc_5EB3FA:
		mov     eax, [ebx + 0x24]
		mov     edx, [edi]
		push    eax
		mov     ecx, edi
		call    dword ptr[edx + 8]
		mov     eax, [esp + 0x1C]
		mov     edx, [ebx]
		push    edi
		push    eax
		mov     ecx, ebx
		call    dword ptr[edx + 0x30]
		mov     eax, g_pD3DDevice
		pop     esi
		test    eax, eax
		jz      short loc_5EB42B
		mov     eax, g_pD3DDevice
		push    0
		push    7
		push    eax
		mov     ecx, [eax]
		call    dword ptr[ecx + 0xE4]

	loc_5EB42B:

		pop     edi
		pop     ebx
		retn    0x14
	}
}

__declspec(naked) void SetTransformHook3()
{
	__asm
	{
		push edx
		push 256
		push eax
		call dword ptr [ecx + 0xB0]
		pop edi
		pop esi
		add esp, 0x70
		retn
	}
}

__declspec(naked) void SetupWorldTransformHook()
{
	__asm
	{
		push ecx
		push 256
		push eax
		call dword ptr[edx + 0xB0]
		mov eax, 0x5AE7B8
		jmp eax
	}
}

void __fastcall D3DRenderDataApplyMat(D3DRenderData *d3d, int nop, Material *a2, int flag1, int flag2, int flag3)
{
	d3d->ApplyMaterial(a2, flag1, flag2, flag3);
}