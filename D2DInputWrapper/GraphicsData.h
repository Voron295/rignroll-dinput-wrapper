#ifndef GRAPHICSDATA_CLASS_H
#define GRAPHICSDATA_CLASS_H

#include <Windows.h>

struct CRect
{
	float left;
	float top;
	float right;
	float bottom;
};

#pragma pack(push, 1)
struct RGB
{
	BYTE r;
	BYTE g;
	BYTE b;
};
#pragma pack(pop)

struct RGBA
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

#pragma pack(push, 1)
struct GraphicsPalette
{
	void **opac;
	void *field_4;
	char *field_8;
	void *field_C;
	void **field_10;
	void *field_14;
	RGB palette[256];
	int field_318;
	int field_31C;
	int field_320;
	int field_324;
	int m_dwDepthMode;
	int field_32C;
};
#pragma pack(pop)

#pragma pack(push, 1)
class GraphicsData
{
public:
	__int16 width;
	__int16 height;
	__int16 depth;
	__int16 m_wPitch;
	__int16 field_8;
	__int16 field_A;
	union {
		BYTE flags_byte[4];
		int flags;
	};

	int field_10;
	int field_14;
	void *lpSurface[6];
	void *m_pMainSurfaceData;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int someState_;
	GraphicsPalette *m_pPalette;
	float opacity;
	char field_5C;
	char field_5D;
	char field_5E;
	char field_5F;
	GraphicsData *parent;
	RECT rect;
	int field_74;
	int field_78;

	GraphicsPalette *GetPalette();
};
#pragma pack(pop)

#endif