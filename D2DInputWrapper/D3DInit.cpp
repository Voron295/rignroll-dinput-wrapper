#include "Hooks.h"
#include "CLog.h"
#include <json\json.h>

GUID g_pDeviceGUID;
extern CLog *logFile;

extern IDirectDraw7 *&g_pDirectDraw;
extern IDirectDraw7 *&g_pDirectDraw7;
extern IDirectDrawSurface7 *&g_pDirectDrawFrontBuffer;
extern IDirect3D7 *&g_pDirect3D;
extern IDirect3DDevice7 *&g_pDirect3DDevice;
extern IDirectDrawSurface7 *&g_pSurfaceBuffer;
extern IDirectDrawSurface7 *&g_pSurfaceBufferZ;
extern IDirectDrawPalette *&g_pDirectDrawPalette;
extern DWORD &g_dwVideoMemory;
extern DWORD &g_dwMemoryState;
extern DWORD &g_bCanBlendMaterials;
extern DWORD &g_bCanModulate2X;
extern DWORD &g_bCanSelectArg1;
extern DWORD &g_bCanSelectArg2;
extern DWORD &g_bCanBlendFactorAlpha;
extern DWORD &g_bCanBlendCurrentAlpha;
extern DWORD &g_dwPrevMaterialHadAlpha;

extern int &g_iWindowWidth;
extern int &g_iWindowHeight;
extern int &gDepth;
extern int &gDepthTextures;

extern Json::Value g_pMainConfig;
extern int g_dwSelectedDeviceId;


extern std::vector<DisplayMode> gDisplayModes;
extern std::vector<DeviceInfoStruct> g_pDevicesInfo;

extern int &gCurrentDriver;
extern int &DirectDrawInitState;
extern int &videoDevicesCount;
extern HWND &g_hWnd;

struct EnumZBufferStruct
{
	int depth;
	DDPIXELFORMAT pxFormat;
};

int CALLBACK DDEnumerateCallbackW(GUID *lpGUID, LPWSTR lpDriverDescription, LPWSTR lpDriverName, void *lpContext)
{
	DeviceInfoStruct info;
	info.deviceGUID = lpGUID;
	wcscpy(info.deviceNameW, lpDriverName);
	wcscpy(info.deviceDescW, lpDriverDescription);
	logFile->WriteLine("enumerate: %s, %s, 0x%X", info.deviceName, info.deviceDesc, lpGUID);
	g_pDevicesInfo.push_back(info);
	return D3DENUMRET_OK;
}

HRESULT CALLBACK D3DEnumDevicesCallback7(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext)
{
	if (!(lpD3DDeviceDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION))
		return D3DENUMRET_OK;

	logFile->WriteLine("enumDevices7: DevCaps: 0x%X, GUID: 0x%X", lpDeviceDescription, lpDeviceName, lpD3DDeviceDesc->dwDevCaps, lpD3DDeviceDesc->deviceGUID);
	g_pDeviceGUID = lpD3DDeviceDesc->deviceGUID;
	DWORD caps = lpD3DDeviceDesc->dwDevCaps;
	if (caps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM)
	{
		*(DWORD*)0x7208FC = 3;
	}
	else if (caps & D3DDEVCAPS_TEXTUREVIDEOMEMORY)
	{
		*(DWORD*)0x7208FC = 2;
	}
	else
	{
		*(DWORD*)0x7208FC = (BYTE)caps >> 8;
	}
	if (lpD3DDeviceDesc->wMaxSimultaneousTextures < 2)
	{
		g_bCanBlendMaterials = 0;
		g_bCanSelectArg2 = 1;
		g_bCanBlendCurrentAlpha = 1;
	}
	else
	{
		g_bCanBlendMaterials = 1;
		g_bCanSelectArg2 = ((lpD3DDeviceDesc->dwTextureOpCaps & 0xFF) >> 3) & 1;
		g_bCanBlendCurrentAlpha = (lpD3DDeviceDesc->dwTextureOpCaps >> 15) & 1;
		if (!g_bCanSelectArg2 && !((lpD3DDeviceDesc->dwTextureOpCaps >> 15) & 1))
			g_bCanBlendMaterials = 0;
	}
	g_bCanModulate2X = ((lpD3DDeviceDesc->dpcTriCaps.dwTextureFilterCaps & 0xFF) >> 5) & 1;
	g_dwPrevMaterialHadAlpha = (lpD3DDeviceDesc->dwTextureOpCaps >> 21) & 1;
	g_bCanSelectArg1 = ((lpD3DDeviceDesc->dwTextureOpCaps & 0xFF) >> 2) & 1;
	g_bCanBlendFactorAlpha = (lpD3DDeviceDesc->dwTextureOpCaps >> 13) & 1;
	return D3DENUMRET_CANCEL;
}

HRESULT CALLBACK EnumZBufferFormatsCB(LPDDPIXELFORMAT ppFormat, void *data)
{
	EnumZBufferStruct *zbuf = (EnumZBufferStruct *)data;
	logFile->WriteLine("ZBuf bit count: %d", ppFormat->dwRGBBitCount);
	if (ppFormat->dwRGBBitCount == 32)
	{
		zbuf->depth = 32;
		zbuf->pxFormat = *ppFormat;
		return D3DENUMRET_CANCEL;
	}
	return D3DENUMRET_OK;
}

int GameSetupDisplayMode(int width, int height, int depth, bool windowed)
{
	HRESULT res = g_pDirectDraw->SetDisplayMode(width, height, depth * 8, 0, 0);
	if (!res)
	{
		DDSURFACEDESC2 desc;
		memset(&desc, 0, sizeof(desc));

		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
		desc.dwBackBufferCount = 1;
		res = g_pDirectDraw->CreateSurface(&desc, &g_pDirectDrawFrontBuffer, 0);
		if (res)
		{
			logFile->WriteLine("Front buffer init fail!");
			return res;
		}

		DDSCAPS2 surfCaps = { 0 };
		surfCaps.dwCaps = DDSCAPS_BACKBUFFER;
		res = g_pDirectDrawFrontBuffer->GetAttachedSurface(&surfCaps, &g_pSurfaceBuffer);
		if (res)
		{
			logFile->WriteLine("GetAttachedSurface Front Buffer fail!");
			return res;
		}
		/*IDirectDrawClipper *clip = 0;
		res = g_pDirectDraw->CreateClipper(0, &clip, 0);
		if (res)
		{
		logFile->WriteLine("Create clipper fail!");
		return res;
		}
		clip->SetHWnd(0, hWnd);
		g_pDirectDrawFrontBuffer->SetClipper(clip);
		*/
		/*memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
		desc.dwWidth = width;
		desc.dwHeight = height;
		res = g_pDirectDraw->CreateSurface(&desc, &g_pSurfaceBuffer, 0);
		if (res)
		{
		logFile->WriteLine("Create surface buffer fail!");
		return res;
		}*/

		memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		desc.dwWidth = width;
		desc.dwHeight = height;

		EnumZBufferStruct enumZbuf;

		res = g_pDirect3D->EnumZBufferFormats(g_pDeviceGUID, EnumZBufferFormatsCB, &enumZbuf);
		if (res)
		{
			logFile->WriteLine("EnumZBufferFormats fail!");
			return res;
		}

		DDSURFACEDESC2 zbufDesc = { 0 };
		zbufDesc.dwSize = sizeof(zbufDesc);
		zbufDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		zbufDesc.dwWidth = width;
		zbufDesc.dwHeight = height;
		zbufDesc.ddpfPixelFormat = enumZbuf.pxFormat;
		zbufDesc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		res = g_pDirectDraw->CreateSurface(&zbufDesc, &g_pSurfaceBufferZ, 0);
		if (res)
		{
			logFile->WriteLine("CreateSurface ZBuffer fail!");
			return res;
		}

		res = g_pSurfaceBuffer->AddAttachedSurface(g_pSurfaceBufferZ);
		if (res)
		{
			logFile->WriteLine("AddAttachedSurface ZBuffer fail!");
			return res;
		}

		res = g_pDirect3D->CreateDevice(g_pDeviceGUID, g_pSurfaceBuffer, &g_pDirect3DDevice);
		if (res)
		{
			logFile->WriteLine("CreateDevice fail!");
			return res;
		}

		res = g_pDirect3DDevice->EnumTextureFormats(EnumTextureFormatsCallback, 0);
		if (res)
		{
			logFile->WriteLine("EnumTextureFormats fail!");
			return res;
		}
	}
	return res;
}

void ReleaseDirectDraw()
{
	((void(__cdecl *)())0x5D81B0)();
}

int InitDirectDraw(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem)
{
	//TestInitDirectDraw();
	logFile->WriteLine("DDrawInit");
	ReleaseDirectDraw();

	SetWindowLongA(g_hWnd, GWL_STYLE, WS_POPUP);
	SetWindowLongA(g_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

	bool windowed = false;
	
	width = g_pMainConfig["graphics"]["width"].asInt();
	height = g_pMainConfig["graphics"]["height"].asInt();
	g_iWindowWidth = width;
	g_iWindowHeight = height;

	*(DWORD *)(0x696770 + 0x100) = width;
	*(DWORD *)(0x696770 + 0x104) = height;
	*(DWORD *)(0x696770 + 0x124) = 0; // some flags
	//logFile->WriteLine("Flags: 0x%X", *(DWORD *)(0x696770 + 0x124));

	//gDepth = 3;
	//gDepthTextures = 3;
	g_dwVideoMemory = 0x2000000;
	g_dwMemoryState = 4;

	int lastErr = DirectDrawCreateEx(0, (LPVOID*)&g_pDirectDraw, IID_IDirectDraw7, 0);
	if (!lastErr)
	{
		DWORD coopLvl = DDSCL_NORMAL;
		if (!windowed)
			coopLvl |= DDSCL_FPUPRESERVE | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN;
		lastErr = g_pDirectDraw->SetCooperativeLevel(g_hWnd, coopLvl);
		if (!lastErr)
		{
			g_pDirectDraw7 = g_pDirectDraw;
			g_pDirectDraw->AddRef();

			lastErr = g_pDirectDraw->QueryInterface(IID_IDirect3D7, (LPVOID*)&g_pDirect3D);
			if (!lastErr)
			{
				lastErr = g_pDirect3D->EnumDevices(D3DEnumDevicesCallback7, 0);
				if (lastErr)
					logFile->WriteLine("EnumDevices Error");
			}
			else
			{
				logFile->WriteLine("Query interface Error");
			}
			if (!lastErr)
			{
				/*if (gDisplayModes.size() == 0)
				{
				lastErr = g_pDirectDraw->EnumDisplayModes(0, 0, 0, DdEnumDisplayModes);
				if (lastErr)
				logFile->WriteLine("EnumDisplayModes Error");
				}
				*/

				lastErr = GameSetupDisplayMode(width, height, depthMode, windowed);
				if (lastErr)
					logFile->WriteLine("SetupDisplayMode Error");
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
	if (lastErr)
	{
		logFile->WriteLine("Unknown Error");
		if (g_pDirectDraw7)
			g_pDirectDraw7->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);
		return 1;
	}
	logFile->WriteLine("No Errors");
	strcpy((char*)0x720790, "Emulation Device");

	//g_pDirectDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
	//ReleaseDirectDraw();
	//exit(0);
	return 1;
}

int InitDirectDrawForMenu(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem)
{
	return InitDirectDraw(800, 600, 4, 0xE00, 0, 0, 0);
}
