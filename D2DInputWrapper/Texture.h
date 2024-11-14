#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H
#include <iostream>
#include <d3dx9.h>
#include "GraphicsData.h"
#include "StreamManager.h"

#pragma pack(push, 1)
class Texture
{
public:
	char *m_sName;
	char *m_pImageData;
	char *m_dMapEndOfFileAddress;
	GraphicsPalette *m_pPalette;
	__int16 m_iWidth;
	__int16 m_iHeight;
	__int16 m_iDepth;
	__int16 texelsCount;
	int field_1C;
	int m_dOffset;
	char *sectionStart;
	int m_dLvmpStructOffset;
	LPDIRECT3DTEXTURE9 ImageDataSurface;
	FILE *m_pFile;
	int flags;
	int ddCaps;
	int field_3C;
	int m_dTextureFormat;
	int m_dwMimMapCount;
	LPDIRECT3DTEXTURE9 __LPDIRECTDRAWSURFACE7;
	LPDIRECT3DTEXTURE9 m_pSurface;
	StreamManager *m_pStreamManager;

	// virtual table
	virtual ~Texture() = 0;

	static void InitHooks();
	static Texture *GetTextureFromPool(const char *name);
	static int LoadFromFile(Texture *tex, FILE *f, DWORD offset);
};
#pragma pack(pop)

void __fastcall TextureInitSurface(Texture *tex, int nop, DWORD flags);
void __fastcall TextureInitSurface2(Texture *tex, int nop, double unk);

#endif