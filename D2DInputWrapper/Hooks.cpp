#include "Hooks.h"
#include "CLog.h"
#include <json\json.h>
#include "GameStructs.h"
extern int g_width;
extern CLog *logFile;
extern Json::Value g_pMainConfig;
extern int g_dwSelectedDeviceId;


int &g_iWindowWidth = *(int*)0x7208BC;
int &g_iWindowHeight = *(int*)0x7208C0;
int &gDepth = *(int*)0x7208B0;
int &gDepthTextures = *(int*)0x7208B4;


#define BYTE1(x)   (*((BYTE*)&(x)+1))

void SetupSidePanelOffsets(int a) 
{
	((void(__cdecl *)(int))0x510E80)(a);
	*(int*)0x6CED34 = g_width - 225;
}

DWORD initWsaHookAddr = 0x4EA910;

__declspec(naked) void InitWSAHook() {
	__asm {
		push ebp
		mov ebp, esp
		jmp initWsaHookAddr
	}
}

void InitWSA(int a) {
	logFile->WriteLine("InitWSA() - 0x%X", a);
	InitWSAHook();
}
char tmpstr[512];

void SendMessageToAllPlayers(void *src, int size)
{
	void *traceback = *(&src-1);
	logFile->WriteLine("SendMessageToAllPlayers(0x%X, %d) - 0x%X", src, size, traceback);
	((void (__cdecl *)(int, void *, int))0x601FC0)(0, src, size);
	void *data = (void*)*(int*)0x723DB8;
	void *end = (void*)*((int*)0x723DB8 + 1);
	sprintf(tmpstr, "sendMessage_0x%X.log", traceback);
	FILE *f = fopen(tmpstr, "wb");
	if (f)
	{
		fwrite(data, 1, (int)end - (int)data, f);
		fclose(f);
	}
}


int dNewHookedAddr = 0x6326D9;
__declspec(naked) void *dNewHooked(int size)
{
	__asm 
	{
		push esi
		push edi
		mov edi, [esp + 4 + 8]
		jmp dNewHookedAddr
	}
}

void *dNew(int size)
{
	int traceback = *(&size - 1);
	void *data = dNewHooked(size);
	logFile->WriteLine("0x%X = new(0x%X) - 0x%X", data, size, traceback);
	return data;
}

void *dMalloc(int size)
{
	int traceback = *(&size - 1);
	void *data = ((void *(__cdecl *)(int, int))0x61D7E9)(size, 0x72E288);
	if (traceback != 0x6326DF)
		logFile->WriteLine("0x%X = malloc(0x%X) - 0x%X", data, size, traceback);
	return data;
}

void dAllocPlayersPool(int a)
{
	int traceback = *(&a - 1);
	logFile->WriteLine("dAllocPlayersPool() - 0x%X", traceback);
}

int __fastcall loadGameFunc(int _this, int dup, char *a2, char *a3, char *a4, char *a5)
{
	char *traceback = *(&a2 - 1);
	int result = ((int (__thiscall *)(int, char *, char *, char *, char *))0x512510)(_this, a2, a3, a4, a5);
	logFile->WriteLine("0x%X = loadGame(0x%X, %s, %s, %s, %s) - 0x%X", result, _this, a2, a3, a4, a5, traceback);
	return result;
}

void *__fastcall HeapAlocInitByName(void *_this, int dup, char *name)
{
	void *result = ((void *(__thiscall *)(void*, char *))0x5D2810)(_this, name);
	logFile->WriteLine("0x%X = HeapAlocInitByName(0x%X, %s)", result, _this, name);
	return result;
}



// Graphics edit
#include <vector>

std::vector<DisplayMode> gDisplayModes;

int &gCurrentDriver = *(int*)0x720904;
int &DirectDrawInitState = *(int*)0x72090C;
int &videoDevicesCount = *(int*)0x72094C;
HWND &hWnd = *(HWND*)0x720940;

int __fastcall unkFuncHook(int game)
{
	int back = 0;
	logFile->WriteLine("unkFuncHook(0x%X) stack:", game);
	for (int i = -3; i < 6; i++)
	{
		logFile->WriteLine("(0x%X)", *(&back + i));
	}
	return ((int (__thiscall *)(int))0x528C20)(game);
}

__declspec(naked) int __fastcall AdjustWinFunc(int winapp, int nop, int width, int height, int a4, int a5)
{
	__asm
	{
		sub esp, 8
			push ebx
			push ebp

			mov eax, 0x5DD855
			jmp eax
	}
}

extern bool g_bGameInitialised;

int __fastcall AdjustWin(int winapp, int nop, int width, int height, int a4, int a5)
{
	if (a5 & 0x1000)
		g_bGameInitialised = true;
	//int res = AdjustWinFunc(winapp, nop, width, height, a4, a5);
	//int res = 1;
	//SetWindowLongA(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	//logFile->WriteLine("AdjustWin(0x%X, %d, %d, 0x%X, 0x%X) from 0x%X;", winapp, width, height, a4, a5, *(&width - 1));
	return 1;
}

int __fastcall WinAppInit(int winapp)
{
	int back = 0;
	int res = ((int(__thiscall *)(int))0x5E2920)(winapp);
	logFile->WriteLine("0x%X = WinAppInit(0x%X) from 0x%X;", res, winapp, *(&back - 2));
	return res;
}

__declspec(naked) int RealInitDirectDraw(int width, int height, int depthMode, int flags, int *errorID, int *tmem, int *vmem)
{
	__asm
	{
		sub esp, 0x3C8
			mov eax, 0x5D7B70
			add eax, 6
			jmp eax
	}
}

int HookInitDirectDraw(int width, int height, int depthMode, int flags, int *errorID, int *tmem, int *vmem)
{
	int backtrace;
	int res = RealInitDirectDraw(width, height, depthMode, flags, errorID, tmem, vmem);
	backtrace = *(&width - 1);
	//int res = ((int (__cdecl *)(int, int, int, int, int*, int*, int*, int*))0x5D7AE0)(width, height, depthMode, flags, a5, errorId, a7, a8);
	if (tmem)
		tmem = (int *)*tmem;
	if (vmem)
		vmem = (int *)*vmem;
	logFile->WriteLine("%d = InitDirectDraw(%d, %d, %d, 0x%X, %d, 0x%X, 0x%X) from 0x%X", res, width, height, depthMode, flags, errorID, tmem, vmem, backtrace);
	return res;
}

int TestGraphicsAccelerator(int drvNum, int *depthMode, int *vpas, int *vpas2, int *vpam, int *vpam2, int *vpal, int *vpal2, int *vpasl, int *vpasl2)
{
	*vpas = 0;
	*vpas2 = 1;
	*vpam = 0;
	*vpam2 = 1;
	*vpal = 0;
	*vpal2 = 1;
	*vpasl = 0;
	*vpasl2 = 1;
	*depthMode = ((int (__cdecl *)())0x5D3A10)(); // get depth
	gCurrentDriver = drvNum;
	return 1;
}

HRESULT __stdcall DdEnumDisplayModes(LPDDSURFACEDESC2 surface, LPVOID data)
{
	std::vector<DisplayMode> *pModes = (std::vector<DisplayMode> *)data;

	DisplayMode mode;
	mode.width = surface->dwWidth;
	mode.height = surface->dwHeight;
	if (surface->ddpfPixelFormat.dwFlags & 0x20)
		mode.depth = 1;
	else if (surface->ddpfPixelFormat.dwFlags & 0x40 && surface->ddpfPixelFormat.dwRGBBitCount == 0x10)
		mode.depth = 2;
	else if (surface->ddpfPixelFormat.dwFlags & 0x40 && surface->ddpfPixelFormat.dwRGBBitCount == 0x20)
		mode.depth = 4;

	if (mode.depth == 4)
	{
		logFile->WriteLine("found resolution: %d, %d, %d", mode.width, mode.height, mode.depth);
		pModes->push_back(mode);
	}
	return D3DENUMRET_OK;
}

int __stdcall DDrawInit(GUID *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, D2Struct *lpContext)
{
	logFile->WriteLine("DDrawInit");
	if (lpContext->m_pD3DDevice7)
	{
		lpContext->m_pD3DDevice7->Release();
		lpContext->m_pD3DDevice7 = 0;
	}
	if (lpContext->m_pPalette)
	{
		lpContext->m_pPalette->Release();
		lpContext->m_pPalette = 0;
	}
	if (lpContext->m_pSurfaceBufferZ)
	{
		lpContext->m_pSurfaceBufferZ->Release();
		lpContext->m_pSurfaceBufferZ = 0;
	}
	if (lpContext->m_pSurfaceBuffer)
	{
		lpContext->m_pSurfaceBuffer->Release();
		lpContext->m_pSurfaceBuffer = 0;
	}
	if (lpContext->m_pSurfaceFrontBuffer)
	{
		lpContext->m_pSurfaceFrontBuffer->Release();
		lpContext->m_pSurfaceFrontBuffer = 0;
	}
	if (lpContext->m_pDirect3D7)
	{
		lpContext->m_pDirect3D7->Release();
		lpContext->m_pDirect3D7 = 0;
	}
	if (lpContext->m_pDirectDraw_1)
	{
		lpContext->m_pDirectDraw_1->Release();
		lpContext->m_pDirectDraw_1 = 0;
	}
	if (lpContext->m_pDirectDraw)
	{
		lpContext->m_pDirectDraw->Release();
		lpContext->m_pDirectDraw = 0;
	}
	lpContext->m_lpVideoDeviceGUID = 0;
	lpContext->m_dwDeviceZBufferBitDepth = 0;
	lpContext->m_bFoundVideoMode = 0;
	lpContext->m_dVideoMemory = 0;
	lpContext->m_dwPixelFormatsFlag = 0;
	lpContext->field_50 = 0;
	lpContext->m_dLastError = 0;

	int lastErr = DirectDrawCreateEx(lpGUID, (LPVOID*)&lpContext->m_pDirectDraw, IID_IDirectDraw7, 0);
	if (!lastErr)
	{
		lastErr = lpContext->m_pDirectDraw->SetCooperativeLevel(hWnd, DDSCL_FPUPRESERVE | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
		if (!lastErr)
		{
			lpContext->m_pDirectDraw_1 = lpContext->m_pDirectDraw;
			lpContext->m_pDirectDraw->AddRef();
			if (BYTE1(lpContext->m_dFlags) & 0x10)
			{
				lastErr = lpContext->m_pDirectDraw->QueryInterface(IID_IDirect3D7, (LPVOID*)&lpContext->m_pDirect3D7);
				lpContext->m_dMemory = 2048;
				if (!lastErr)
				{
					lastErr = lpContext->m_pDirect3D7->EnumDevices((LPD3DENUMDEVICESCALLBACK7)0x5D4860, (LPVOID*)lpContext);
					if (lastErr)
						logFile->WriteLine("EnumDevices Error");
				}
				else
				{
					logFile->WriteLine("Query interface Error");
				}
			}
			if (!lastErr)
			{
				lpContext->m_bFoundVideoMode = 1;
				lpContext->m_dVideoMemory = 0x1200000;
				if (BYTE1(lpContext->m_dFlags) & 0x80)
				{
					lastErr = ((int (__cdecl *)(D2Struct*))0x5D6DA0)(lpContext); // GameSetupDisplayMode
					if (lastErr)
						logFile->WriteLine("SetupDisplayMode Error");

				}
				if (gDisplayModes.size() == 0)
				{
					lastErr = lpContext->m_pDirectDraw->EnumDisplayModes(0, 0, 0, DdEnumDisplayModes);
					if (lastErr)
						logFile->WriteLine("EnumDisplayModes Error");
				}
			}
		}
		else
		{
			logFile->WriteLine("Set coop level Error");
		}
	}
	else
	{
		logFile->WriteLine("DirectDrawCreateEx Error (0x%X)", lastErr);
	}
	lpContext->m_dLastError = lastErr;
	if (lastErr)
	{
		logFile->WriteLine("Unknown Error");
		if (lpContext->m_pDirectDraw)
			lpContext->m_pDirectDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		return 1;
	}
	logFile->WriteLine("No Errors");
	strcpy((char*)0x720790, "Emulation Device");
	return 0;
}

IDirectDraw7 *&g_pDirectDraw = *(IDirectDraw7**)0x720930;
IDirectDraw7 *&g_pDirectDraw7 = *(IDirectDraw7**)0x72092C;
IDirectDrawSurface7 *&g_pDirectDrawFrontBuffer = *(IDirectDrawSurface7 **)0x720918;
IDirect3D7 *&g_pDirect3D = *(IDirect3D7 **)0x720924;
IDirect3DDevice7 *&g_pDirect3DDevice = *(IDirect3DDevice7 **)0x720928;
IDirectDrawSurface7 *&g_pSurfaceBuffer = *(IDirectDrawSurface7 **)0x72091C;
IDirectDrawSurface7 *&g_pSurfaceBufferZ = *(IDirectDrawSurface7 **)0x720920;
IDirectDrawPalette *&g_pDirectDrawPalette = *(IDirectDrawPalette **)0x720914;
DWORD &g_dwVideoMemory = *(DWORD *)0x720900;
DWORD &g_dwMemoryState = *(DWORD *)0x6CED90; // need to be 4

DWORD &g_bCanBlendMaterials = *(DWORD *)0x7208DC;
DWORD &g_bCanModulate2X = *(DWORD *)0x7208E0;
DWORD &g_bCanSelectArg1 = *(DWORD *)0x7208E8;
DWORD &g_bCanSelectArg2 = *(DWORD *)0x7208F0;
DWORD &g_bCanBlendFactorAlpha = *(DWORD *)0x7208EC;
DWORD &g_bCanBlendCurrentAlpha = *(DWORD *)0x7208F4;
DWORD &g_dwPrevMaterialHadAlpha = *(DWORD *)0x7208E4;


std::vector<DeviceInfoStruct> g_pDevicesInfo;
//std::vector<DeviceMode> g_pDeviceModes;


int CALLBACK DDEnumerateCallback(GUID *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, void *lpContext)
{
	DeviceInfoStruct info;
	info.deviceGUID = lpGUID;
	strcpy(info.deviceName, lpDriverName);
	strcpy(info.deviceDesc, lpDriverDescription);
	mbstowcs(info.deviceNameW, lpDriverName, 256);
	mbstowcs(info.deviceDescW, lpDriverDescription, 256);
	logFile->WriteLine("enumerate: %s, %s, 0x%X", lpDriverName, lpDriverDescription, lpGUID);
	g_pDevicesInfo.push_back(info);
	return D3DENUMRET_OK;
}

void Enum3DModes()
{
	IDirectDraw7 *pDirectDraw = 0;
	IDirect3D7 *pDirect3D = 0;

	int err = DirectDrawEnumerateA(DDEnumerateCallback, 0);
	for (auto it = g_pDevicesInfo.begin(); it != g_pDevicesInfo.end(); it++)
	{
		int lastErr = DirectDrawCreateEx(it->deviceGUID, (LPVOID*)&pDirectDraw, IID_IDirectDraw7, 0);
		if (!lastErr)
		{
			lastErr = pDirectDraw->EnumDisplayModes(0, 0, &it->pModes, DdEnumDisplayModes);
			if (lastErr)
				logFile->WriteLine("EnumDevices Error");
		}
		pDirectDraw->Release();
	}
}

DDPIXELFORMAT &g_PixelFormat555 = *(DDPIXELFORMAT *)0x720890;
DDPIXELFORMAT &g_PixelFormat4444 = *(DDPIXELFORMAT *)0x720148;
DDPIXELFORMAT &g_PixelFormat565 = *(DDPIXELFORMAT *)0x720168;
DDPIXELFORMAT &stru_720748 = *(DDPIXELFORMAT *)0x720748;
DDPIXELFORMAT g_PixelFormat888;
DDPIXELFORMAT g_PixelFormat8888;
DDPIXELFORMAT g_PixelFormat8;

DWORD &m_dwSupportedPixelFormat = *(DWORD*)0x7208B8;
DWORD &m_dwCurrentDepthMode = *(DWORD*)0x7208B4;

HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT pixelFormat, LPVOID _data)
{
	//D2Struct *data = (D2Struct *)_data;
	if (pixelFormat->dwFlags & DDPF_RGB)
	{
		logFile->WriteLine("PixelFormat: R(0x%X), G(0x%X), B(0x%X), A(0x%X); Total: %d",
			pixelFormat->dwRBitMask, pixelFormat->dwGBitMask, pixelFormat->dwBBitMask, pixelFormat->dwRGBAlphaBitMask, pixelFormat->dwRGBBitCount);

		if (pixelFormat->dwBBitMask == 0x1F
			&& pixelFormat->dwGBitMask == 0x3E0
			&& pixelFormat->dwRBitMask == 0x7C00
			&& !pixelFormat->dwRGBAlphaBitMask)
		{
			memcpy(&g_PixelFormat555, pixelFormat, sizeof(g_PixelFormat555));
			m_dwSupportedPixelFormat |= 4u;
		}
		if (pixelFormat->dwBBitMask == 0xF
			&& pixelFormat->dwGBitMask == 0xF0
			&& pixelFormat->dwRBitMask == 0xF00
			&& pixelFormat->dwRGBAlphaBitMask == 0xF000)
		{
			memcpy(&g_PixelFormat4444, pixelFormat, sizeof(g_PixelFormat4444));
			m_dwSupportedPixelFormat |= 0x10;
		}
		if (pixelFormat->dwBBitMask == 0x1F && pixelFormat->dwGBitMask == 0x7E0 && pixelFormat->dwRBitMask == 0xF800)
		{
			memcpy(&g_PixelFormat565, pixelFormat, sizeof(g_PixelFormat565));
			m_dwSupportedPixelFormat |= 2;
		}
		if (pixelFormat->dwBBitMask == 0xFF
			&& pixelFormat->dwGBitMask == 0xFF00
			&& pixelFormat->dwRBitMask == 0xFF0000
			&& !pixelFormat->dwRGBAlphaBitMask)
		{
			memcpy(&g_PixelFormat888, pixelFormat, sizeof(g_PixelFormat888));
		}
		if (pixelFormat->dwBBitMask == 0xFF
			&& pixelFormat->dwGBitMask == 0xFF00
			&& pixelFormat->dwRBitMask == 0xFF0000
			&& pixelFormat->dwRGBAlphaBitMask == 0xFF000000)
		{
			memcpy(&g_PixelFormat8888, pixelFormat, sizeof(g_PixelFormat8888));
		}
		if (pixelFormat->dwRGBBitCount == 8)
		{
			memcpy(&g_PixelFormat8, pixelFormat, sizeof(g_PixelFormat888));
		}
	}
	else if (pixelFormat->dwFlags & 0x80000
		&& pixelFormat->dwRGBBitCount == 16
		&& pixelFormat->dwRBitMask == 0xFF
		&& pixelFormat->dwGBitMask == 0xFF00)
	{
		memcpy(&stru_720748, pixelFormat, sizeof(stru_720748));
		m_dwSupportedPixelFormat |= 0x80;
	}
	return 1;
}

void SetSurfaceDescPixelParams(DDSURFACEDESC2 *desc, int width, int height, int textureFormat, int mipmapCount, int overlay)
{
	memset(desc, 0, sizeof(DDSURFACEDESC2));
	desc->dwSize = 0x7C;
	desc->dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;

	if (overlay)
	{
		desc->ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
		//logFile->WriteLine("surface desc: %d, %d", width, height);
	}
	else
	{
		desc->ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
	}

	if (mipmapCount && g_bCanModulate2X)
	{
		desc->dwFlags |= DDSD_MIPMAPCOUNT;
		desc->dwMipMapCount = mipmapCount;
		desc->ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
	}

	desc->dwWidth = width;
	desc->dwHeight = height;

	if (m_dwCurrentDepthMode == 1) {
		memcpy(&desc->ddpfPixelFormat, &g_PixelFormat8, sizeof(desc->ddpfPixelFormat));
		logFile->WriteLine("pixel format 8");
	}
	else 
	{
		switch (textureFormat - 2)
		{
		case 0:
			memcpy(&desc->ddpfPixelFormat, &g_PixelFormat565, sizeof(desc->ddpfPixelFormat));
			logFile->WriteLine("surface desc 565: %d, %d", width, height);
			break;

		case 1:
			memcpy(&desc->ddpfPixelFormat, &g_PixelFormat555, sizeof(desc->ddpfPixelFormat));
			logFile->WriteLine("surface desc 555: %d, %d", width, height);
			break;

		case 3:
		case 6:
			memcpy(&desc->ddpfPixelFormat, &g_PixelFormat4444, sizeof(desc->ddpfPixelFormat));
			logFile->WriteLine("surface desc 4444: %d, %d", width, height);
			break;

		case 4:
		case 5:
			memcpy(&desc->ddpfPixelFormat, &g_PixelFormat8888, sizeof(desc->ddpfPixelFormat));
			logFile->WriteLine("surface desc 8888: %d, %d", width, height);
			break;
		default:
			logFile->WriteLine("Cannot find pixel format for texture format %d", textureFormat);
			break;
		}
	}
	
}

int __fastcall TextureReadHeaderReplaceDepth1(void *texture)
{
	int format = *(DWORD *)((DWORD)texture + 0x40);
	if (format == 6)
		return 3;
	else if (format == 7)
		return 4;
	else if (format > 1)
		return 2;
	
	return 1;
}

int __fastcall TextureReadHeaderReplaceDepth2(void *texture)
{
	int format = *(DWORD *)((DWORD)texture + 0x40);
	logFile->WriteLine("texture replace depth %d", format);
	if (format == 6)
		return 3;
	else if (format == 7)
		return 4;
	else if (format > 1)
		return 2;

	return 1;
}

int __cdecl TextureReadHeaderReplaceDepthDefault()
{
	if (m_dwCurrentDepthMode == 1)
	{
		return 1;
	}
	return 2;
}

__declspec(naked) void __cdecl TextureInitReplaceDepth()
{
	__asm
	{
		xor eax, eax
		mov ax, [esi+0x18]
		retn
	}
}

__declspec(naked) void __cdecl FontReadReplaceDepth()
{
	__asm
	{
		mov ecx, esi
		call TextureReadHeaderReplaceDepth2
		retn
	}
}

__declspec(naked) void __cdecl TextureReadHeaderReplaceDepth()
{
	__asm
	{
		mov ecx, edi
		call TextureReadHeaderReplaceDepth1
		retn
	}
}

__declspec(naked) void __cdecl TextureInitSurfaceDesc1()
{
	__asm
	{
		push 0
		mov esi, [ebp+0x44] // передаём кол-во мипмапов
		push esi

		mov esi, [ebp+0x40] // формат текстуры
		push esi
		xor eax, eax
		xor edx, edx
		mov ax, [ebp+0x14] // width
		mov dx, [ebp+0x16] // height
		push edx
		push eax

		lea edi, [esp + 0x118 + 0x14 - 0xFC] // desc struct
		push edi
		call SetSurfaceDescPixelParams
		add esp, 0x18
		mov eax, 0x5BAF72
		jmp eax
	}
}

__declspec(naked) void __cdecl TextureInitSurfaceDesc2()
{
	__asm
	{
		push 0
		push 0

		mov esi, [ebp + 0x40] // формат текстуры
		push esi
		xor eax, eax
		xor edx, edx
		mov ax, [ebp + 0x14] // width
		mov dx, [ebp + 0x16] // height
		push edx
		push eax

		lea edi, [esp + 0x118 + 0x14 - 0xFC] // desc struct
		push edi
		call SetSurfaceDescPixelParams
		add esp, 0x18
		mov eax, 0x5BB0A8
		push 0
		jmp eax
	}
}

__declspec(naked) void __cdecl MaskInitSurfaceDesc()
{
	__asm
	{
		push 1
		push 0

		push 3 // формат текстуры
		xor eax, eax
		xor ecx, ecx
		mov ax, [ebp + 4] // width
		mov cx, [ebp + 6] // height
		push ecx
		push eax

		lea edi, [esp + 0x128 + 0x14 - 0xF8] // desc struct
		push edi
		call SetSurfaceDescPixelParams
		add esp, 0x18
		mov eax, 0x5BC03D
		push 0
		jmp eax
	}
}

__declspec(naked) void __cdecl FontInitSurfaceDesc()
{
	__asm
	{
		lea edi, [esp + 0x134 - 0xF8] // desc struct

		mov esi, [esp + 0x134 - 0x120] // texture struct
		push esi // save it

		//-----------

		push 1
		push 0

		xor eax, eax
		mov ax, [esi + 0x40]
		push eax // формат текстуры
		xor eax, eax
		xor edx, edx
		mov ax, [esi + 0x14] // width
		mov dx, [esi + 0x16] // height
		push edx
		push eax

		push edi
		call SetSurfaceDescPixelParams
		add esp, 0x18
		//------------

		pop esi // restore texture struct
		xor ebp, ebp
		mov bp, [esi + 0x18] // depth

		push 0
		lea edx, [esp + 0x138 - 0x124]
		mov eax, 0x5E9176
		jmp eax
	}
}

void CreateTextureFromScreenCopy1(char *imageData, char *surface, int pitch, int width, int height)
{
	while (height)
	{
		memcpy(imageData, surface, width * 4);
		imageData += width * 4;
		surface += width * 4;
		height--;
	}
}

__declspec(naked) void __cdecl CreateTextureFromScreenCopy()
{
	__asm
	{
		push esi
		push ebx
		mov ebx, [esp + 0x28 - 0x14]
		push ebx
		push eax
		push edx
		call CreateTextureFromScreenCopy1
		add esp, 0x14
		mov ebx, 0x50C14F
		jmp ebx
	}
}

int __cdecl LogError(char *a1, char *a2)
{
	logFile->WriteLine("LogError: %s, %s (0x%X)", a1, a2, *(&a1 - 1));
	((void (__cdecl *)(int))0x61E854)(0); // exit
	return 0;
}

IDirectDrawSurface7 *speedometerSurf = 0;
extern unsigned char g_pSpeedometerTexture[22878];

void __fastcall CWinAppDrawFrame(void *winapp)
{
	if (g_pDirectDraw->TestCooperativeLevel() != DD_OK)
	{
		return;
	}

	D3DVIEWPORT7 viewData;
	memset(&viewData, 0, sizeof(D3DVIEWPORT7));
	viewData.dwX = viewData.dwY = 0;
	viewData.dwWidth = g_iWindowWidth;
	viewData.dwHeight = g_iWindowHeight;
	viewData.dvMinZ = 0.0f;
	viewData.dvMaxZ = 1.0f;
	
	g_pDirect3DDevice->SetViewport(&viewData);

	g_pDirect3DDevice->BeginScene();
	g_pDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFF00, 1.0, 0);

	((void(__thiscall *)(void*))0x5E1FB0)(winapp);

	g_pDirect3DDevice->EndScene();
	
	DWORD result = g_pDirectDrawFrontBuffer->Flip(0, DDFLIP_WAIT);
	if (result != DD_OK)
	{
		g_pDirectDraw->RestoreAllSurfaces();
	}
}