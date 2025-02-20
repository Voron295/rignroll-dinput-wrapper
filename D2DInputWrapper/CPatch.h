#pragma once
#include <Windows.h>
#define CALLVOID(a) ((void (__cdecl *)())a)()
#define CALLVOIDRETCHAR(a) ((char (__cdecl *)())a)()
#define READPVOID(a) (*(void **)a)
#define READCHAR(a) (*(char *)a)

class CPatch
{
private:
	inline static void Patch(void* address, void* data, int size)
	{
		unsigned long protect[2];
		VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &protect[0]);
		memcpy(address, data, size);
		VirtualProtect(address, size, protect[0], &protect[1]);
		//logFile->WriteLine("Address: 0x%X, data: 0x%X, size: %d", address, *(DWORD *)data, size);
	}
public:
	inline static void Nop(int address, int size)
	{
		unsigned long protect[2];
		VirtualProtect((void *)address, size, PAGE_EXECUTE_READWRITE, &protect[0]);
		memset((void *)address, 0x90, size);
		VirtualProtect((void *)address, size, protect[0], &protect[1]);
	}
	inline static void RedirectCall(int address, void *func)
	{
		int temp = 0xE8;
		Patch((void *)address, &temp, 1);
		temp = (int)func - ((int)address + 5);
		Patch((void *)((int)address + 1), &temp, 4);
	}
	inline static void RedirectJump(int address, void *func)
	{
		int temp = 0xE9;
		Patch((void *)address, &temp, 1);
		temp = (int)func - ((int)address + 5);
		Patch((void *)((int)address + 1), &temp, 4);
	}
	inline static void SetChar(int address, char value)
	{
		Patch((void *)address, &value, 1);
	}
	inline static void SetShort(int address, short value)
	{
		Patch((void *)address, &value, 2);
	}
	inline static void SetInt(int address, int value)
	{
		Patch((void *)address, &value, 4);
	}
	inline static void SetFloat(int address, float value)
	{
		Patch((void *)address, &value, 4);
	}
	inline static void SetPointer(int address, void *value)
	{
		Patch((void *)address, &value, 4);
	}
	inline static void GetChar(int address, char *value)
	{
		Patch(value, (void *)address, 1);
	}
	inline static void GetShort(int address, short *value)
	{
		Patch(value, (void *)address, 2);
	}
	inline static void GetInt(int address, int *value)
	{
		Patch(value, (void *)address, 4);
	}
	inline static void GetFloat(int address, float *value)
	{
		Patch(value, (void *)address, 4);
	}
	inline static char GetChar(int address)
	{
		char value;
		Patch(&value, (void *)address, 1);
		return value;
	}
	inline static short GetShort(int address)
	{
		short value;
		Patch(&value, (void *)address, 2);
		return value;
	}
	inline static int GetInt(int address)
	{
		int value;
		Patch(&value, (void *)address, 4);
		return value;
	}
	inline static float GetFloat(int address)
	{
		float value;
		Patch(&value, (void *)address, 4);
		return value;
	}
};