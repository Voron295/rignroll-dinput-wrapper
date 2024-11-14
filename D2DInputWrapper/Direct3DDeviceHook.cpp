#include "Direct3DDeviceHook.h"
#include "CLog.h"
#include <map>
extern CLog *logFile;

// DirectDraw Virtual Table Hooks
void **realDirect3DDeviceTable = 0;
void **hookDirect3DDeviceTable = 0;

HRESULT __stdcall IDirect3DDevice7_QueryInterface_HOOK(int p, int a, int b);
ULONG __stdcall IDirect3DDevice7_AddRef_HOOK(int p);
ULONG __stdcall IDirect3DDevice7_Release_HOOK(int p);
HRESULT __stdcall IDirect3DDevice7_GetCaps_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_EnumTextureFormats_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_BeginScene_HOOK(int p);
HRESULT __stdcall IDirect3DDevice7_EndScene_HOOK(int p);
HRESULT __stdcall IDirect3DDevice7_GetDirect3D_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_SetRenderTarget_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetRenderTarget_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_Clear_HOOK(int p, int a, int b, int c, int d, int e, int f);
HRESULT __stdcall IDirect3DDevice7_SetTransform_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetTransform_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_SetViewport_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_MultiplyTransform_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetViewport_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_SetMaterial_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_GetMaterial_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_SetLight_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetLight_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_SetRenderState_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetRenderState_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_BeginStateBlock_HOOK(int p);
HRESULT __stdcall IDirect3DDevice7_EndStateBlock_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_PreLoad_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_DrawPrimitive_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitive_HOOK(int p, int a, int b, int c, int d, int e, int f, int g);
HRESULT __stdcall IDirect3DDevice7_SetClipStatus_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_GetClipStatus_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_DrawPrimitiveStrided_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitiveStrided_HOOK(int p, int a, int b, int c, int d, int e, int f, int g);
HRESULT __stdcall IDirect3DDevice7_DrawPrimitiveVB_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitiveVB_HOOK(int p, int a, int b, int c, int d, int e, int f, int g);
HRESULT __stdcall IDirect3DDevice7_ComputeSphereVisibility_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirect3DDevice7_GetTexture_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_SetTexture_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetTextureStageState_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirect3DDevice7_SetTextureStageState_HOOK(int p, int a, int b, int c);
HRESULT __stdcall IDirect3DDevice7_ValidateDevice_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_ApplyStateBlock_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_CaptureStateBlock_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_DeleteStateBlock_HOOK(int p, int a);
HRESULT __stdcall IDirect3DDevice7_CreateStateBlock_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_Load_HOOK(int p, int a, int b, int c, int d, int e);
HRESULT __stdcall IDirect3DDevice7_LightEnable_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetLightEnable_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_SetClipPlane_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetClipPlane_HOOK(int p, int a, int b);
HRESULT __stdcall IDirect3DDevice7_GetInfo_HOOK(int p, int a, int b, int c);

void Direct3DDeviceHook(IDirect3DDevice7 **lplpDevice)
{
	if (hookDirect3DDeviceTable)
		free(hookDirect3DDeviceTable);

	hookDirect3DDeviceTable = (void**)malloc(4 * 49);
	hookDirect3DDeviceTable[0] = IDirect3DDevice7_QueryInterface_HOOK;
	hookDirect3DDeviceTable[1] = IDirect3DDevice7_AddRef_HOOK;
	hookDirect3DDeviceTable[2] = IDirect3DDevice7_Release_HOOK;
	hookDirect3DDeviceTable[3] = IDirect3DDevice7_GetCaps_HOOK;
	hookDirect3DDeviceTable[4] = IDirect3DDevice7_EnumTextureFormats_HOOK;
	hookDirect3DDeviceTable[5] = IDirect3DDevice7_BeginScene_HOOK;
	hookDirect3DDeviceTable[6] = IDirect3DDevice7_EndScene_HOOK;
	hookDirect3DDeviceTable[7] = IDirect3DDevice7_GetDirect3D_HOOK;
	hookDirect3DDeviceTable[8] = IDirect3DDevice7_SetRenderTarget_HOOK;
	hookDirect3DDeviceTable[9] = IDirect3DDevice7_GetRenderTarget_HOOK;
	hookDirect3DDeviceTable[10] = IDirect3DDevice7_Clear_HOOK;
	hookDirect3DDeviceTable[11] = IDirect3DDevice7_SetTransform_HOOK;
	hookDirect3DDeviceTable[12] = IDirect3DDevice7_GetTransform_HOOK;
	hookDirect3DDeviceTable[13] = IDirect3DDevice7_SetViewport_HOOK;
	hookDirect3DDeviceTable[14] = IDirect3DDevice7_MultiplyTransform_HOOK;
	hookDirect3DDeviceTable[15] = IDirect3DDevice7_GetViewport_HOOK;
	hookDirect3DDeviceTable[16] = IDirect3DDevice7_SetMaterial_HOOK;
	hookDirect3DDeviceTable[17] = IDirect3DDevice7_GetMaterial_HOOK;
	hookDirect3DDeviceTable[18] = IDirect3DDevice7_SetLight_HOOK;
	hookDirect3DDeviceTable[19] = IDirect3DDevice7_GetLight_HOOK;
	hookDirect3DDeviceTable[20] = IDirect3DDevice7_SetRenderState_HOOK;
	hookDirect3DDeviceTable[21] = IDirect3DDevice7_GetRenderState_HOOK;
	hookDirect3DDeviceTable[22] = IDirect3DDevice7_BeginStateBlock_HOOK;
	hookDirect3DDeviceTable[23] = IDirect3DDevice7_EndStateBlock_HOOK;
	hookDirect3DDeviceTable[24] = IDirect3DDevice7_PreLoad_HOOK;
	hookDirect3DDeviceTable[25] = IDirect3DDevice7_DrawPrimitive_HOOK;
	hookDirect3DDeviceTable[26] = IDirect3DDevice7_DrawIndexedPrimitive_HOOK;
	hookDirect3DDeviceTable[27] = IDirect3DDevice7_SetClipStatus_HOOK;
	hookDirect3DDeviceTable[28] = IDirect3DDevice7_GetClipStatus_HOOK;
	hookDirect3DDeviceTable[29] = IDirect3DDevice7_DrawPrimitiveStrided_HOOK;
	hookDirect3DDeviceTable[30] = IDirect3DDevice7_DrawIndexedPrimitiveStrided_HOOK;
	hookDirect3DDeviceTable[31] = IDirect3DDevice7_DrawPrimitiveVB_HOOK;
	hookDirect3DDeviceTable[32] = IDirect3DDevice7_DrawIndexedPrimitiveVB_HOOK;
	hookDirect3DDeviceTable[33] = IDirect3DDevice7_ComputeSphereVisibility_HOOK;
	hookDirect3DDeviceTable[34] = IDirect3DDevice7_GetTexture_HOOK;
	hookDirect3DDeviceTable[35] = IDirect3DDevice7_SetTexture_HOOK;
	hookDirect3DDeviceTable[36] = IDirect3DDevice7_GetTextureStageState_HOOK;
	hookDirect3DDeviceTable[37] = IDirect3DDevice7_SetTextureStageState_HOOK;
	hookDirect3DDeviceTable[38] = IDirect3DDevice7_ValidateDevice_HOOK;
	hookDirect3DDeviceTable[39] = IDirect3DDevice7_ApplyStateBlock_HOOK;
	hookDirect3DDeviceTable[40] = IDirect3DDevice7_CaptureStateBlock_HOOK;
	hookDirect3DDeviceTable[41] = IDirect3DDevice7_DeleteStateBlock_HOOK;
	hookDirect3DDeviceTable[42] = IDirect3DDevice7_CreateStateBlock_HOOK;
	hookDirect3DDeviceTable[43] = IDirect3DDevice7_Load_HOOK;
	hookDirect3DDeviceTable[44] = IDirect3DDevice7_LightEnable_HOOK;
	hookDirect3DDeviceTable[45] = IDirect3DDevice7_GetLightEnable_HOOK;
	hookDirect3DDeviceTable[46] = IDirect3DDevice7_SetClipPlane_HOOK;
	hookDirect3DDeviceTable[47] = IDirect3DDevice7_GetClipPlane_HOOK;
	hookDirect3DDeviceTable[48] = IDirect3DDevice7_GetInfo_HOOK;

	realDirect3DDeviceTable = **(void****)lplpDevice;
	**(void****)lplpDevice = hookDirect3DDeviceTable;
	logFile->WriteLine("Direct3DDevice7 Hooked");
}

std::map<int, std::string> gCalls;


HRESULT __stdcall IDirect3DDevice7_QueryInterface_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_QueryInterface_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_QueryInterface_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[0])(p, a, b);
}


ULONG __stdcall IDirect3DDevice7_AddRef_HOOK(int p)
{
	logFile->WriteLine("IDirect3DDevice7_AddRef_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_AddRef_HOOK");
	}
	return ((ULONG(__stdcall *)(int))realDirect3DDeviceTable[1])(p);
}

ULONG __stdcall IDirect3DDevice7_Release_HOOK(int p)
{
	logFile->WriteLine("IDirect3DDevice7_Release_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_Release_HOOK");
	}
	return ((ULONG(__stdcall *)(int))realDirect3DDeviceTable[2])(p);
}

HRESULT __stdcall IDirect3DDevice7_GetCaps_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetCaps_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetCaps_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[3])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_EnumTextureFormats_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_EnumTextureFormats_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_EnumTextureFormats_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[4])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_BeginScene_HOOK(int p)
{
	logFile->WriteLine("IDirect3DDevice7_BeginScene_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_BeginScene_HOOK");
	}
	return ((HRESULT(__stdcall *)(int))realDirect3DDeviceTable[5])(p);
}

HRESULT __stdcall IDirect3DDevice7_EndScene_HOOK(int p)
{
	logFile->WriteLine("IDirect3DDevice7_EndScene_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_EndScene_HOOK");
	}
	return ((HRESULT(__stdcall *)(int))realDirect3DDeviceTable[6])(p);
}

HRESULT __stdcall IDirect3DDevice7_GetDirect3D_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetDirect3D_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetDirect3D_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[7])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_SetRenderTarget_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetRenderTarget_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetRenderTarget_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[8])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetRenderTarget_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetRenderTarget_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetRenderTarget_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[9])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_Clear_HOOK(int p, int a, int b, int c, int d, int e, int f)
{
	logFile->WriteLine("IDirect3DDevice7_Clear_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_Clear_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int, int))realDirect3DDeviceTable[10])(p, a, b, c, d, e, f);
}

HRESULT __stdcall IDirect3DDevice7_SetTransform_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetTransform_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetTransform_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[11])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetTransform_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetTransform_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetTransform_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[12])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_SetViewport_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_SetViewport_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetViewport_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[13])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_MultiplyTransform_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_MultiplyTransform_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_MultiplyTransform_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[14])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetViewport_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetViewport_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetViewport_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[15])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_SetMaterial_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_SetMaterial_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetMaterial_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[16])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_GetMaterial_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetMaterial_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetMaterial_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[17])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_SetLight_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetLight_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetLight_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[18])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetLight_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetLight_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetLight_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[19])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_SetRenderState_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetRenderState_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetRenderState_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[20])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetRenderState_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetRenderState_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetRenderState_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[21])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_BeginStateBlock_HOOK(int p)
{
	logFile->WriteLine("IDirect3DDevice7_BeginStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_BeginStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int))realDirect3DDeviceTable[22])(p);
}

HRESULT __stdcall IDirect3DDevice7_EndStateBlock_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_EndStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_EndStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[23])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_PreLoad_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_PreLoad_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_PreLoad_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[24])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_DrawPrimitive_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirect3DDevice7_DrawPrimitive_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawPrimitive_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirect3DDeviceTable[25])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitive_HOOK(int p, int a, int b, int c, int d, int e, int f, int g)
{
	logFile->WriteLine("IDirect3DDevice7_DrawIndexedPrimitive_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawIndexedPrimitive_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int, int, int))realDirect3DDeviceTable[26])(p, a, b, c, d, e, f, g);
}

HRESULT __stdcall IDirect3DDevice7_SetClipStatus_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_SetClipStatus_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetClipStatus_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[27])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_GetClipStatus_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_GetClipStatus_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetClipStatus_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[28])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_DrawPrimitiveStrided_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirect3DDevice7_DrawPrimitiveStrided_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawPrimitiveStrided_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirect3DDeviceTable[29])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitiveStrided_HOOK(int p, int a, int b, int c, int d, int e, int f, int g)
{
	logFile->WriteLine("IDirect3DDevice7_DrawIndexedPrimitiveStrided_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawIndexedPrimitiveStrided_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int, int, int))realDirect3DDeviceTable[30])(p, a, b, c, d, e, f, g);
}

HRESULT __stdcall IDirect3DDevice7_DrawPrimitiveVB_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirect3DDevice7_DrawPrimitiveVB_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawPrimitiveVB_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirect3DDeviceTable[31])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirect3DDevice7_DrawIndexedPrimitiveVB_HOOK(int p, int a, int b, int c, int d, int e, int f, int g)
{
	logFile->WriteLine("IDirect3DDevice7_DrawIndexedPrimitiveVB_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DrawIndexedPrimitiveVB_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int, int, int))realDirect3DDeviceTable[32])(p, a, b, c, d, e, f, g);
}

HRESULT __stdcall IDirect3DDevice7_ComputeSphereVisibility_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirect3DDevice7_ComputeSphereVisibility_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_ComputeSphereVisibility_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirect3DDeviceTable[33])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirect3DDevice7_GetTexture_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetTexture_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetTexture_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[34])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_SetTexture_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetTexture_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetTexture_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[35])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetTextureStageState_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3DDevice7_GetTextureStageState_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetTextureStageState_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DDeviceTable[36])(p, a, b, c);
}

HRESULT __stdcall IDirect3DDevice7_SetTextureStageState_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3DDevice7_SetTextureStageState_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetTextureStageState_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DDeviceTable[37])(p, a, b, c);
}

HRESULT __stdcall IDirect3DDevice7_ValidateDevice_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_ValidateDevice_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_ValidateDevice_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[38])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_ApplyStateBlock_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_ApplyStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_ApplyStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[39])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_CaptureStateBlock_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_CaptureStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_CaptureStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[40])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_DeleteStateBlock_HOOK(int p, int a)
{
	logFile->WriteLine("IDirect3DDevice7_DeleteStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_DeleteStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int))realDirect3DDeviceTable[41])(p, a);
}

HRESULT __stdcall IDirect3DDevice7_CreateStateBlock_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_CreateStateBlock_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_CreateStateBlock_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[42])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_Load_HOOK(int p, int a, int b, int c, int d, int e)
{
	logFile->WriteLine("IDirect3DDevice7_Load_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_Load_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int, int, int))realDirect3DDeviceTable[43])(p, a, b, c, d, e);
}

HRESULT __stdcall IDirect3DDevice7_LightEnable_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_LightEnable_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_LightEnable_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[44])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetLightEnable_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetLightEnable_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetLightEnable_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[45])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_SetClipPlane_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_SetClipPlane_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_SetClipPlane_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[46])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetClipPlane_HOOK(int p, int a, int b)
{
	logFile->WriteLine("IDirect3DDevice7_GetClipPlane_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetClipPlane_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int))realDirect3DDeviceTable[47])(p, a, b);
}

HRESULT __stdcall IDirect3DDevice7_GetInfo_HOOK(int p, int a, int b, int c)
{
	logFile->WriteLine("IDirect3DDevice7_GetInfo_HOOK, 0x%X", *(&p - 1));
	if (gCalls.find(*(&p - 2)) == gCalls.end())
	{
		gCalls.emplace(*(&p - 2), "IDirect3DDevice7_GetInfo_HOOK");
	}
	return ((HRESULT(__stdcall *)(int, int, int, int))realDirect3DDeviceTable[48])(p, a, b, c);
}
