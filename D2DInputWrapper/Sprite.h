#ifndef SPRITE_CLASS_H
#define SPRITE_CLASS_H

#include <Windows.h>
#include "Font.h"
#include "Mask.h"

struct SpriteData
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	float field_18;
	Texture *texture;
	Mask *mskFiles;
};

class Sprite
{
public:
	Sprite *m_pPrev;
	Sprite *m_pNext;
	char *m_sText;
	Font *m_pFont;
	Font *font2;
	int field_18;
	int spriteId;
	int flags;
	int field_24;
	int isActive;
	RECT rect;
	int field_3C;
	int field_40;
	SpriteData spriteData[6];
	int activeFlag;

	virtual ~Sprite();
};

#endif