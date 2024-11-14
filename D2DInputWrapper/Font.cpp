#include "Font.h"
#include "CPatch.h"
#include "CLog.h"
#include "GraphicsMod.h"
extern CLog *logFile;

void Font::SurfaceCreateHook(Font *font, Texture *texture)
{
	logFile->WriteLine("SurfaceCreateHook(0x%X, 0x%X)", font, texture);
	RGB transp = {0, 0, 0};
	if (texture->m_pPalette)
	{
		transp.r = texture->m_pPalette->palette[0].r;
		transp.g = texture->m_pPalette->palette[0].g;
		transp.b = texture->m_pPalette->palette[0].b;
	}

	font->m_pSurface = CreateDirect3DTextureFromMemory(g_pDirect3DDevice, texture->m_iWidth, texture->m_iHeight, texture->m_iDepth, texture->m_pImageData, 1, transp);
}

void Font::InitHooks()
{
	CPatch::Nop(0x5E9146, 5);

	CPatch::SetChar(0x5E914F, 0x56); // push esi ; Texture
	CPatch::SetChar(0x5E9150, 0x53); // push ebx ; Font
	CPatch::RedirectCall(0x5E9151, Font::SurfaceCreateHook);
	CPatch::SetChar(0x5E9156, 0x83);
	CPatch::SetChar(0x5E9157, 0xC4);
	CPatch::SetChar(0x5E9158, 0x08);
	CPatch::RedirectJump(0x5E9159, (void*)0x5E9283);
}