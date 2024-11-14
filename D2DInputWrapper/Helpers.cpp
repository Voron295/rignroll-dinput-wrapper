#include "Helpers.h"

void *allocMemory(unsigned int size)
{
	/*__asm
	{
		push ecx
		push [esp + 8]
		mov eax, 0x6326D3
		call eax
		add esp, 4
		pop ecx
		ret
	}*/
	return ((void *(__cdecl *)(unsigned int))0x6326D3)(size);
}