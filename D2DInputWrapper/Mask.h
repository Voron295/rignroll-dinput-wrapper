#ifndef MASK_CLASS_H
#define MASK_CLASS_H

#include <d3dx9.h>
#include <iostream>
#include "GraphicsData.h"

class Mask
{
public:
	char *m_sName;
	__int16 m_iWidth;
	__int16 m_iHeight;
	__int16 m_dDepth;
	__int16 field_A;
	int m_dBlockSize;
	void *m_pDataOffset;
	int m_dBlockPos;
	FILE *m_pFile;
	int flags;
	int field_20;
	LPDIRECT3DTEXTURE9 m_pSurface;

	static void InitHooks();
};

void MaskDraw(GraphicsData *gData, int x, int y, Mask *msk, DWORD flags);

#endif