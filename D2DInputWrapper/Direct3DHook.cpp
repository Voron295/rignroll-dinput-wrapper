#include "Direct3DHook.h"
#include "Direct3DDeviceHook.h"
#include "CLog.h"
extern CLog *logFile;

// DirectDraw Virtual Table Hooks
void **realDirect3DTable = 0;
void **hookDirect3DTable = 0;

HRESULT __stdcall IDirect3D7_QueryInterface_HOOK(int p, int a, int b);
ULONG __stdcall IDirect3D7_AddRef_HOOK(int p);
ULONG __stdcall IDirect3D7_Release_HOOK(int p);
HRESULT __stdcall IDirect3D7_EnumDevices_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3D7_CreateDevice_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirect3D7_CreateVertexBuffer_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirect3D7_EnumZBufferFormats_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirect3D7_EvictManagedTextures_HOOK(int p);

void Direct3DHook(IDirect3D7 **lplpDirect3d)
{
	if (hookDirect3DTable)
		free(hookDirect3DTable);

	hookDirect3DTable = (void**)malloc(4 * 8);
	hookDirect3DTable[0] = IDirect3D7_QueryInterface_HOOK;
	hookDirect3DTable[1] = IDirect3D7_AddRef_HOOK;
	hookDirect3DTable[2] = IDirect3D7_Release_HOOK;
	hookDirect3DTable[3] = IDirect3D7_EnumDevices_HOOK;
	hookDirect3DTable[4] = IDirect3D7_CreateDevice_HOOK;
	hookDirect3DTable[5] = IDirect3D7_CreateVertexBuffer_HOOK;
	hookDirect3DTable[6] = IDirect3D7_EnumZBufferFormats_HOOK;
	hookDirect3DTable[7] = IDirect3D7_EvictManagedTextures_HOOK;

	realDirect3DTable = **(void ****)lplpDirect3d;
	**(void ****)lplpDirect3d = hookDirect3DTable;
	logFile->WriteLine("Direct3D7 Hooked");
}

HRESULT __stdcall IDirect3D7_QueryInterface_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3D7_QueryInterface_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DTable[0])(p, a, b);
}

ULONG __stdcall IDirect3D7_AddRef_HOOK(int p)
{
	logFile->WriteLine("IDirect3D7_AddRef_HOOK, 0x%X", *(&p - 1));
	return ((ULONG(__stdcall *)(int))realDirect3DTable[1])(p);
}

ULONG __stdcall IDirect3D7_Release_HOOK(int p)
{
	logFile->WriteLine("IDirect3D7_Release_HOOK, 0x%X", *(&p - 1));
	return ((ULONG(__stdcall *)(int))realDirect3DTable[2])(p);
}

HRESULT __stdcall IDirect3D7_EnumDevices_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3D7_EnumDevices_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DTable[3])(p, a, b);
}

HRESULT __stdcall IDirect3D7_CreateDevice_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3D7_CreateDevice_HOOK, 0x%X", *(&p - 1));
	HRESULT result = ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DTable[4])(p, a, b, c);
	Direct3DDeviceHook((IDirect3DDevice7**)c);
	return result;
}

HRESULT __stdcall IDirect3D7_CreateVertexBuffer_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3D7_CreateVertexBuffer_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DTable[5])(p, a, b, c);
}

HRESULT __stdcall IDirect3D7_EnumZBufferFormats_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3D7_EnumZBufferFormats_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DTable[6])(p, a, b, c);
}

HRESULT __stdcall IDirect3D7_EvictManagedTextures_HOOK(int p)
{
	logFile->WriteLine("IDirect3D7_EvictManagedTextures_HOOK, 0x%X", *(&p - 1));
	return ((HRESULT(__stdcall *)(int))realDirect3DTable[7])(p);
}
