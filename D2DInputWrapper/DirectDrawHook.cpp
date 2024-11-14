#include "DirectDrawHook.h"
#include "CLog.h"
#include "Direct3DHook.h"
extern CLog *logFile;

// DirectDraw Virtual Table Hooks
void **realDirectDrawTable = 0;
void **hookDirectDrawTable = 0;

HRESULT __stdcall IDirectDraw7_QueryInterface_HOOK(int p, int a, int b);
ULONG __stdcall IDirectDraw7_AddRef_HOOK(int p);
ULONG __stdcall IDirectDraw7_Release_HOOK(int p);
HRESULT __stdcall IDirectDraw7_Compact_HOOK(int p);
HRESULT __stdcall IDirectDraw7_CreateClipper_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirectDraw7_CreatePalette_HOOK(int p, int a, int b, int c, int d);
HRESULT __stdcall IDirectDraw7_CreateSurface_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirectDraw7_DuplicateSurface_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_EnumDisplayModes_HOOK(int p, int a, int b, int c, int d);
HRESULT __stdcall IDirectDraw7_EnumSurfaces_HOOK(int p, int a, int b, int c, int d);
HRESULT __stdcall IDirectDraw7_FlipToGDISurface_HOOK(int p);
HRESULT __stdcall IDirectDraw7_GetCaps_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_GetDisplayMode_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_GetFourCCCodes_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_GetGDISurface_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_GetMonitorFrequency_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_GetScanLine_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_GetVerticalBlankStatus_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_Initialize_HOOK(int p, int a);
HRESULT __stdcall IDirectDraw7_RestoreDisplayMode_HOOK(int p);
HRESULT __stdcall IDirectDraw7_SetCooperativeLevel_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_SetDisplayMode_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirectDraw7_WaitForVerticalBlank_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_GetAvailableVidMem_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirectDraw7_GetSurfaceFromDC_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_RestoreAllSurfaces_HOOK(int p);
HRESULT __stdcall IDirectDraw7_TestCooperativeLevel_HOOK(int p);
HRESULT __stdcall IDirectDraw7_GetDeviceIdentifier_HOOK(int p, int a, int b);
HRESULT __stdcall IDirectDraw7_StartModeTest_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirectDraw7_EvaluateMode_HOOK(int p, int a, int b);

int __stdcall DirectDrawCreateHook(GUID *lpGuid, LPVOID *lplpDD, const IID *const iid, IUnknown *pUnkOuter)
{
	int result = ((int(__stdcall *)(GUID *, LPVOID *, const IID * iid, IUnknown *))0x6303D6)(lpGuid, lplpDD, iid, pUnkOuter);

	if (hookDirectDrawTable)
		free(hookDirectDrawTable);
	hookDirectDrawTable = (void**)malloc(4 * 30);
	hookDirectDrawTable[0] = IDirectDraw7_QueryInterface_HOOK;
	hookDirectDrawTable[1] = IDirectDraw7_AddRef_HOOK;
	hookDirectDrawTable[2] = IDirectDraw7_Release_HOOK;
	hookDirectDrawTable[3] = IDirectDraw7_Compact_HOOK;
	hookDirectDrawTable[4] = IDirectDraw7_CreateClipper_HOOK;
	hookDirectDrawTable[5] = IDirectDraw7_CreatePalette_HOOK;
	hookDirectDrawTable[6] = IDirectDraw7_CreateSurface_HOOK;
	hookDirectDrawTable[7] = IDirectDraw7_DuplicateSurface_HOOK;
	hookDirectDrawTable[8] = IDirectDraw7_EnumDisplayModes_HOOK;
	hookDirectDrawTable[9] = IDirectDraw7_EnumSurfaces_HOOK;
	hookDirectDrawTable[10] = IDirectDraw7_FlipToGDISurface_HOOK;
	hookDirectDrawTable[11] = IDirectDraw7_GetCaps_HOOK;
	hookDirectDrawTable[12] = IDirectDraw7_GetDisplayMode_HOOK;
	hookDirectDrawTable[13] = IDirectDraw7_GetFourCCCodes_HOOK;
	hookDirectDrawTable[14] = IDirectDraw7_GetGDISurface_HOOK;
	hookDirectDrawTable[15] = IDirectDraw7_GetMonitorFrequency_HOOK;
	hookDirectDrawTable[16] = IDirectDraw7_GetScanLine_HOOK;
	hookDirectDrawTable[17] = IDirectDraw7_GetVerticalBlankStatus_HOOK;
	hookDirectDrawTable[18] = IDirectDraw7_Initialize_HOOK;
	hookDirectDrawTable[19] = IDirectDraw7_RestoreDisplayMode_HOOK;
	hookDirectDrawTable[20] = IDirectDraw7_SetCooperativeLevel_HOOK;
	hookDirectDrawTable[21] = IDirectDraw7_SetDisplayMode_HOOK;
	hookDirectDrawTable[22] = IDirectDraw7_WaitForVerticalBlank_HOOK;
	hookDirectDrawTable[23] = IDirectDraw7_GetAvailableVidMem_HOOK;
	hookDirectDrawTable[24] = IDirectDraw7_GetSurfaceFromDC_HOOK;
	hookDirectDrawTable[25] = IDirectDraw7_RestoreAllSurfaces_HOOK;
	hookDirectDrawTable[26] = IDirectDraw7_TestCooperativeLevel_HOOK;
	hookDirectDrawTable[27] = IDirectDraw7_GetDeviceIdentifier_HOOK;
	hookDirectDrawTable[28] = IDirectDraw7_StartModeTest_HOOK;
	hookDirectDrawTable[29] = IDirectDraw7_EvaluateMode_HOOK;

	realDirectDrawTable = **(void****)(lplpDD);
	**(void****)(lplpDD) = hookDirectDrawTable;
	logFile->WriteLine("DirectDraw Hooked");
	return result;
}

HRESULT __stdcall IDirectDraw7_QueryInterface_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_QueryInterface, 0x%X, (0x%X, 0x%X, 0x%X)", *(&p - 1), p, a, b);
	HRESULT result = ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[0])(p, a, b);
	/*if (*(GUID*)a == IID_IDirect3D7)
	{
		Direct3DHook((IDirect3D7**)b);
	}*/
	return result;
}

ULONG __stdcall IDirectDraw7_AddRef_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_AddRef, 0x%X", *(&p - 1));
	return ((ULONG(__stdcall *)(int))realDirectDrawTable[1])(p);
}

ULONG __stdcall IDirectDraw7_Release_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_Release, 0x%X", *(&p - 1));
	return ((ULONG(__stdcall *)(int))realDirectDrawTable[2])(p);
}

HRESULT __stdcall IDirectDraw7_Compact_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_Compact_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirectDrawTable[3])(p);
}

HRESULT __stdcall IDirectDraw7_CreateClipper_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirectDraw7_CreateClipper_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirectDrawTable[4])(p, a, b, c);
}

HRESULT __stdcall IDirectDraw7_CreatePalette_HOOK(int p, int a, int b, int c, int d)
{
	logFile->WriteLine("IDirectDraw7_CreatePalette_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int, int))realDirectDrawTable[5])(p, a, b, c, d);
}

HRESULT __stdcall IDirectDraw7_CreateSurface_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirectDraw7_CreateSurface_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirectDrawTable[6])(p, a, b, c);
}

HRESULT __stdcall IDirectDraw7_DuplicateSurface_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_DuplicateSurface_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[7])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_EnumDisplayModes_HOOK(int p, int a, int b, int c, int d)
{
	logFile->WriteLine("IDirectDraw7_EnumDisplayModes_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int, int))realDirectDrawTable[8])(p, a, b, c, d);
}

HRESULT __stdcall IDirectDraw7_EnumSurfaces_HOOK(int p, int a, int b, int c, int d)
{
	logFile->WriteLine("IDirectDraw7_EnumSurfaces_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int, int))realDirectDrawTable[9])(p, a, b, c, d);
}

HRESULT __stdcall IDirectDraw7_FlipToGDISurface_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_FlipToGDISurface_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirectDrawTable[10])(p);
}

HRESULT __stdcall IDirectDraw7_GetCaps_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_GetCaps_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[11])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_GetDisplayMode_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_GetDisplayMode_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[12])(p, a);
}

HRESULT __stdcall IDirectDraw7_GetFourCCCodes_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_GetFourCCCodes_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[13])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_GetGDISurface_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_GetGDISurface_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[14])(p, a);
}

HRESULT __stdcall IDirectDraw7_GetMonitorFrequency_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_GetMonitorFrequency_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[15])(p, a);
}

HRESULT __stdcall IDirectDraw7_GetScanLine_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_GetScanLine_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[16])(p, a);
}

HRESULT __stdcall IDirectDraw7_GetVerticalBlankStatus_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_GetVerticalBlankStatus_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[17])(p, a);
}

HRESULT __stdcall IDirectDraw7_Initialize_HOOK(int p, int a)
{
	logFile->WriteLine("IDirectDraw7_Initialize_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int))realDirectDrawTable[18])(p, a);
}

HRESULT __stdcall IDirectDraw7_RestoreDisplayMode_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_RestoreDisplayMode_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirectDrawTable[19])(p);
}

HRESULT __stdcall IDirectDraw7_SetCooperativeLevel_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_SetCooperativeLevel_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[20])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_SetDisplayMode_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirectDraw7_SetDisplayMode_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirectDrawTable[21])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirectDraw7_WaitForVerticalBlank_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_WaitForVerticalBlank_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[22])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_GetAvailableVidMem_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirectDraw7_GetAvailableVidMem_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirectDrawTable[23])(p, a, b, c);
}

HRESULT __stdcall IDirectDraw7_GetSurfaceFromDC_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_GetSurfaceFromDC_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[24])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_RestoreAllSurfaces_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_RestoreAllSurfaces_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirectDrawTable[25])(p);
}

HRESULT __stdcall IDirectDraw7_TestCooperativeLevel_HOOK(int p)
{
	logFile->WriteLine("IDirectDraw7_TestCooperativeLevel_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirectDrawTable[26])(p);
}

HRESULT __stdcall IDirectDraw7_GetDeviceIdentifier_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_GetDeviceIdentifier_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[27])(p, a, b);
}

HRESULT __stdcall IDirectDraw7_StartModeTest_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirectDraw7_StartModeTest_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirectDrawTable[28])(p, a, b, c);
}

HRESULT __stdcall IDirectDraw7_EvaluateMode_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirectDraw7_EvaluateMode_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirectDrawTable[29])(p, a, b);
}
