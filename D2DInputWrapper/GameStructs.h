#pragma once
#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

#include <Windows.h>
#include <d3d.h>
#include "Texture.h"

#pragma pack(push, 1)
struct KeyState
{
	char bIsPressed;
	char bBlockPressing;
};
#pragma pack(pop)

extern KeyState *g_pKeyStates;

#pragma pack(push, 1)
struct CVector
{
	float x;
	float y;
	float z;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct CMatrix
{
	CVector right;
	CVector top;
	CVector at;
	CVector pos;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TLVERTEX
{
	float x;
	float y;
	float z;
	float w;
	D3DCOLOR diffuse;
	D3DCOLOR specular;
	float u;
	float v;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Font
{
	int flags;
	int startChar;
	int numChars;
	char *charEnd;
	char *charStart;
	int charHeightPx;
	int charWidthPx;
	char *bits;
	int field_20;
	LPDIRECTDRAWSURFACE7 m_pSurface;
};
#pragma pack(pop)


extern CMatrix *(__cdecl *CMatrixMultiply)(CMatrix *out, CMatrix *a1, CMatrix *a2);
extern CMatrix *(__cdecl *CMatrixSetPos)(CMatrix *out, float x, float y, float z);
extern CMatrix *(__cdecl *CMatrixSetRotX)(CMatrix *out, float val);
extern CMatrix *(__cdecl *CMatrixSetRotY)(CMatrix *out, float val);
extern CMatrix *(__cdecl *CMatrixSetRotZ)(CMatrix *out, float val);
extern CVector *(__cdecl *CVectorMulMatrix)(CVector *out, CVector *inV, CMatrix *inM);
extern void(__cdecl *RenderText)(void *surface, int x, int y, void *font, char *text, unsigned int color, int isRealColor);

extern HWND &g_hWnd;

extern void *&g_pCurrentScreenElement;
extern void *&g_pMatrixFont;
extern void *&g_pViewer;
extern IDirect3DDevice7 *&g_pDirect3DDev;
extern int &g_dwCurrentSpeed;
extern BOOL &g_bGamePause;
extern void *&g_pTaskPager;
extern IDirectDraw7 *&g_pDirectDraw;
extern void *&g_pTheGame;

void RenderQuad(CVector pos, CVector size, CVector rotationPoint, float rotation, DWORD color);
void RenderTexturedQuad(CVector pos, CVector size, CVector rotationPoint, IDirectDrawSurface7 *tex, float rotation, DWORD color);
void RenderTexturedQuad(CVector pos, CVector size, CVector rotationPoint, IDirectDrawSurface7 *tex, CVector uv1, CVector uv2, float rotation, DWORD color);
void RenderTriangle(CVector up, CVector left, CVector right, CVector rotationPoint, float rotation, DWORD color);
IDirectDrawSurface7 *InitSurfaceFromMemory(unsigned char *data, int width, int height, int depth, DDPIXELFORMAT *format);

#endif