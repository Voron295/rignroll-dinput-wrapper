#ifndef FONT_CLASS_H
#define FONT_CLASS_H

#include <Windows.h>
#include "Texture.h"

#pragma pack(push, 1)
class Font
{
public:
	int flags;
	int startChar;
	int numChars;
	char *charProp;
	char *field_10;
	int charHeightPx;
	int charWidthPx;
	char *bits;
	int field_20;
	LPDIRECT3DTEXTURE9 m_pSurface;


	static void InitHooks();
	static void SurfaceCreateHook(Font *font, Texture *texture);
};
#pragma pack(pop)

#endif