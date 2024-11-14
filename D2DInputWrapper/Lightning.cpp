#include "Lightning.h"
#include "CPatch.h"

int SetLightHook1jmp = 0x4CD295;
__declspec(naked) void SetLightHook1()
{
	__asm
	{
		mov     edx, [eax]
		push    eax
		call    dword ptr[edx + 0xCC]
		jmp		SetLightHook1jmp
	}
}

int SetLightHook2jmp = 0x4CD2ED;
__declspec(naked) void SetLightHook2()
{
	__asm
	{
		push    edx
		push    eax
		call    dword ptr[ecx + 0xCC]
		jmp		SetLightHook2jmp
	}
}

int SetLightHook3jmp = 0x4CD3FF;
__declspec(naked) void SetLightHook3()
{
	__asm
	{
		push    edx
		push    eax
		call    dword ptr[ecx + 0xCC]
		jmp		SetLightHook3jmp
	}
}

int SetLightHook4jmp = 0x4CD47D;
__declspec(naked) void SetLightHook4()
{
	__asm
	{
		push    edx
		push    eax
		call    dword ptr[ecx + 0xCC]
		jmp		SetLightHook4jmp
	}
}

int SetLightHook5jmp = 0x4CD689;
__declspec(naked) void SetLightHook5()
{
	__asm
	{
		mov     edx, [eax]
		push    eax
		call    dword ptr[edx + 0xCC]
		jmp		SetLightHook5jmp
	}
}

int SetLightHook6jmp = 0x4CD6F4;
__declspec(naked) void SetLightHook6()
{
	__asm
	{
		push    edx
		push    eax
		call    dword ptr[ecx + 0xCC]
		jmp		SetLightHook6jmp
	}
}


void Lightning::InitHooks()
{
	// патчи EnableLight
	CPatch::SetChar(0x4CD04E, 0xD4);
	CPatch::SetChar(0x4CD6A0, 0xD4);
	CPatch::SetChar(0x4CD761, 0xD4);
	CPatch::SetChar(0x4CD2AC, 0xD4);
	CPatch::SetChar(0x4CD33E, 0xD4);
	CPatch::SetChar(0x4CD3E6, 0xD4);
	CPatch::SetChar(0x4CD417, 0xD4);
	CPatch::SetChar(0x4CD4BD, 0xD4);
	CPatch::SetChar(0x4CD4E8, 0xD4);
	CPatch::SetChar(0x4CD552, 0xD4);
	CPatch::SetChar(0x4CD58E, 0xD4);

	// патчи SetLight
	CPatch::RedirectJump(0x4CD28F, SetLightHook1);
	CPatch::RedirectJump(0x4CD2E8, SetLightHook2);
	CPatch::RedirectJump(0x4CD3FA, SetLightHook3);
	CPatch::RedirectJump(0x4CD478, SetLightHook4);
	CPatch::RedirectJump(0x4CD683, SetLightHook5);
	CPatch::RedirectJump(0x4CD6EF, SetLightHook6);
}