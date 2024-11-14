#include "ScreenElement.h"
#include "Helpers.h"

ScreenElement::~ScreenElement()
{
	((void(__thiscall *)(ScreenElement*))0x5D8670)(this);
}

ScreenElement *ScreenElement::Create(WORD width, WORD height, WORD depth, DWORD flags)
{
	ScreenElement *elem = (ScreenElement*)allocMemory(0x38); // new
	((void(__thiscall *)(ScreenElement*, DWORD, DWORD, DWORD, DWORD))0x5D85C0)(elem, width, height, depth, flags);
	return elem;
}

DWORD ScreenElement::GetWidth()
{
	return pGraphicsData->width;
}

DWORD ScreenElement::GetHeight()
{
	return pGraphicsData->height;
}

DWORD ScreenElement::GetDepth()
{
	return pGraphicsData->depth;
}

DWORD ScreenElement::SetActive()
{
	return ((DWORD(__thiscall *)(ScreenElement*))0x4DDD90)(this);
}

DWORD ScreenElement::Preload()
{
	return ((DWORD(__thiscall *)(ScreenElement*))0x5D8560)(this);
}

void ScreenElement::InitGraphicsData(DWORD width, DWORD height, DWORD depth, DWORD flags)
{
	return ((void(__thiscall *)(ScreenElement*, DWORD, DWORD, DWORD, DWORD))0x5DA100)(this, width, height, depth, flags);
}

DWORD ScreenElement::InitGraphicsRect(RECT rect)
{
	return ((DWORD(__thiscall *)(ScreenElement*, RECT))0x5DA2E0)(this, rect);
}

DWORD ScreenElement::DrawSprites()
{
	return 0;
}

DWORD ScreenElement::ProcessEvents(void *controllerData)
{
	return 0;
}

DWORD ScreenElement::Draw(void *controllerData)
{
	return ((DWORD(__thiscall *)(ScreenElement*, void *))0x4DDDA0)(this, controllerData);
}

DWORD ScreenElement::SetupViewport()
{
	return ((DWORD(__thiscall *)(ScreenElement*))0x50FEC0)(this);
}