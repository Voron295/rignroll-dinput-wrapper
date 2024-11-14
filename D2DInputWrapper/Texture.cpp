#include "Texture.h"
#include "CPatch.h"
#include "CLog.h"
#include "GraphicsModFuncs.h"
#include "CWinApp.h"
#include "Helpers.h"

extern CLog *logFile;
extern IDirect3DDevice9 *&g_pDirect3DDevice;
extern TheGame *&gTheGame;
extern CWinApp *&g_pWinApp;

void __fastcall TextureInitSurface(Texture *tex, int nop, DWORD flags)
{
	if (!tex->m_pSurface)
	{
		bool needToDelete = false;
		if (!tex->m_pImageData)
		{
			needToDelete = true;
			tex->m_pImageData = ((char* (__cdecl *)(DWORD size))0x6326D3)(tex->m_iDepth * tex->m_iWidth * tex->m_iHeight);
			Texture::LoadFromFile(tex, tex->m_pFile, tex->m_dOffset);
		}

		if (tex->m_dTextureFormat & 2 && tex->flags & 8)
		{
			tex->m_pSurface = CreateDirect3DTextureFromMemory(g_pDirect3DDevice, tex->m_iWidth, tex->m_iHeight, tex->m_iDepth, tex->m_pImageData, true);
		}
		else
		{
			tex->m_pSurface = CreateDirect3DTextureFromMemory(g_pDirect3DDevice, tex->m_iWidth, tex->m_iHeight, tex->m_iDepth, tex->m_pImageData, false);
		}

		//tex->flags |= 2;
	}
	
	
	/*if (needToDelete)
	{
		((char* (__cdecl *)(char*))0x6326FC)(tex->m_pImageData);
		tex->m_pImageData = 0;
	}*/
}

__declspec(naked) int Texture::LoadFromFile(Texture *tex, FILE *f, DWORD offset)
{
	__asm
	{
		push ebx
		push [esp + 4 + 0xC]
		push [esp + 8 + 8]
		push [esp + 0xC + 4]
		mov eax, 0x5E8100
		call eax
		add esp, 0xC
		pop ebx
		retn
	}
	//((int(__cdecl *)(Texture *, FILE *, DWORD))0x5E8100)(tex, f, offset);
}

void __fastcall TextureInitSurface2(Texture *tex, int nop, double unk)
{
	TextureInitSurface(tex, nop, unk);
}

void __fastcall TextureReinit(Texture *tex)
{
	DWORD flags = tex->flags;
	if (flags & 4)
	{
		tex->field_3C++;
		if (!(flags & 1))
		{
			if (tex->m_dOffset || !tex->m_sName)
			{
				tex->m_pImageData = ((char* (__cdecl *)(DWORD size))0x6326D3)(tex->m_iDepth * tex->m_iWidth * tex->m_iHeight);
				Texture::LoadFromFile(tex, tex->m_pFile, tex->m_dOffset);
			}
			else
			{

			}
		}

		if (tex->m_pImageData && !tex->m_pSurface)
			TextureInitSurface(tex, 0, 2);
	}
}

void TextureDrawTexture(GraphicsData *data, Texture *tex, RECT *texCoords, int x, int y)
{
	D3DXVECTOR2 posStart, posEnd, uvStart, uvEnd;
	
	posStart.x = x;
	posStart.y = y;
	posEnd.x = x + (texCoords->right - texCoords->left);
	posEnd.y = y + (texCoords->bottom - texCoords->top);
	uvStart.x = (float)texCoords->left / (float)tex->m_iWidth;
	uvStart.y = (float)texCoords->top / (float)tex->m_iHeight;
	uvEnd.x = (float)texCoords->right / (float)tex->m_iWidth;
	uvEnd.y = (float)texCoords->bottom / (float)tex->m_iHeight;

	if (!tex->m_pSurface)
		TextureInitSurface(tex, 0, 1);

	RenderQuadPrimitive(g_pDirect3DDevice, tex->m_pSurface, &D3DXVECTOR2(tex->m_iWidth, tex->m_iHeight), &posStart, &posEnd, &uvStart, &uvEnd, D3DCOLOR_XRGB(255, 255, 255), false);
	
	//logFile->WriteLine("DrawTexture(%d, %d, %d, %d, %d, %d)", x, y, texCoords->left, texCoords->top, texCoords->right, texCoords->bottom);
}

Texture *Texture::GetTextureFromPool(const char *name)
{
	return ((Texture *(__cdecl *)(const char *))0x5EF380)(name);
}

int __fastcall TextureCopyDataToSurface(Texture *tex, int nop, DWORD useMmio, int a3, double a4)
{
	return 0;
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = tex->__LPDIRECTDRAWSURFACE7->LockRect(0, &lockedRect, 0, 0);
	if (!SUCCEEDED(hr))
	{
		tex->__LPDIRECTDRAWSURFACE7->Release();
		return 0;
	}

	DWORD source = 0;
	if (useMmio & 1)
		source = (DWORD)tex->sectionStart;
	else
		source = (DWORD)tex->m_pImageData;

	if (useMmio & 1)
		tex->m_pStreamManager->Skip(source + tex->m_dOffset, tex->m_iWidth * tex->m_iHeight * tex->m_iDepth);
	return 1;
}

int __fastcall TextureInit(Texture *tex, int nop, DWORD flags)
{
	if (tex->flags & 1)
		return 1;
	tex->m_pSurface = 0;

	if (!(flags & 2) && (tex->texelsCount || tex->m_iWidth == 1 && tex->m_iHeight == 1))
	{
		IDirect3DTexture9 *texture;
		
		HRESULT hr = g_pDirect3DDevice->CreateTexture(tex->m_iWidth, tex->m_iHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, 0);

		if (!SUCCEEDED(hr))
			return 0;

		

		tex->__LPDIRECTDRAWSURFACE7 = texture;
		// copy
		TextureCopyDataToSurface(tex, 0, flags & 1, 0, 1.0);
		tex->m_pSurface = tex->__LPDIRECTDRAWSURFACE7;
		tex->flags |= 1;
	}
	else
	{
		
	}
	return 1;
}

int __fastcall TextureCreateFromScreen(Texture **tex)
{
	gTheGame->SetActive();
	Texture *texture = ((Texture* (__cdecl *)(DWORD size))0x6326D3)(0x54);
	((void(__thiscall *)(Texture*, int, int, int))0x5BA430)(texture, g_pWinApp->width, g_pWinApp->height, 3);
	return 1;
}

DWORD TextureLoadFromFile1Addr = 0x5E88C8;
DWORD TextureLoadFromFile2Addr = 0x5E88C0;

__declspec(naked) void TextureLoadFromFile1()
{
	__asm
	{
		mov [esp + 0x3C - 0x1C], ecx
		mov[esp + 0x3C - 0x20], edx
		mov[esp + 0x3C - 4], 255
		jmp TextureLoadFromFile1Addr
	}
}

__declspec(naked) void TextureLoadFromFile2()
{
	__asm
	{
		mov[esp + 0x3C - 0x1C], ecx
		mov[esp + 0x3C - 0x20], eax
		mov[esp + 0x3C - 4], 255
		jmp TextureLoadFromFile2Addr
	}
}

void __fastcall TextureConvertColor5Real(WORD color, int nop, BYTE *a, BYTE *b, BYTE *g, BYTE *r)
{
	GetRGBAFrom4444(color, *r, *g, *b, *a);
}

__declspec(naked) void TextureConvertColor5()
{
	__asm
	{
		mov ecx, eax // вносим прочитанные данные в регистр ecx для передачи в функцию
		lea edx, [esp + 0x3C - 0x20] // b
		lea ebx, [esp + 0x3C - 0x1C] // g
		push edx
		push ebx
		lea edx, [esp + 0x3C + 8 - 0x2D] // r
		lea ebx, [esp + 0x3C + 8 - 4] // a
		push edx
		push ebx
		call TextureConvertColor5Real
		mov ebx, [esp + 0x3C - 0x2D] // r

		// exit
		mov eax, 0x5E88C0
		jmp eax
	}
}

void __fastcall TextureConvertColor3Real(WORD color, int nop, BYTE *a, BYTE *b, BYTE *g, BYTE *r)
{
	GetRGBAFrom555(color, *r, *g, *b, *a);
}

__declspec(naked) void TextureConvertColor3()
{
	__asm
	{
		mov ecx, eax // вносим прочитанные данные в регистр ecx для передачи в функцию
		lea edx, [esp + 0x3C - 0x20] // b
		lea ebx, [esp + 0x3C - 0x1C] // g
		push edx
		push ebx
		lea edx, [esp + 0x3C + 8 - 0x2D] // r
		lea ebx, [esp + 0x3C + 8 - 4] // a
		push edx
		push ebx
		call TextureConvertColor3Real
		mov ebx, [esp + 0x3C - 0x2D] // r

		// exit
		mov eax, 0x5E88C0
		jmp eax
	}
}



void __fastcall TextureConvertColor2Real(WORD color, int nop, BYTE *a, BYTE *b, BYTE *g, BYTE *r)
{
	GetRGBFrom565(color, *r, *g, *b);
	*a = 255;
}

__declspec(naked) void TextureConvertColor2()
{
	__asm
	{
		mov ecx, eax // вносим прочитанные данные в регистр ecx для передачи в функцию
		lea edx, [esp + 0x3C - 0x20] // b
		lea ebx, [esp + 0x3C - 0x1C] // g
		push edx
		push ebx
		lea edx, [esp + 0x3C + 8 - 0x2D] // r
		lea ebx, [esp + 0x3C + 8 - 4] // a
		push edx
		push ebx
		call TextureConvertColor2Real
		mov ebx, [esp + 0x3C - 0x2D] // r

		// exit
		mov eax, 0x5E88C0
		jmp eax
	}
}

void __fastcall TextureInitColorKey(TheGame *_this, Texture *tex, Material *mat)
{
	if (!tex->m_pImageData)
	{
		tex->m_pImageData = ((char* (__cdecl *)(DWORD size))0x6326D3)(tex->m_iDepth * tex->m_iWidth * tex->m_iHeight);
		Texture::LoadFromFile(tex, tex->m_pFile, tex->m_dOffset);
	}

	if (tex->m_dTextureFormat & 2 && tex->flags & 8)
	{
		RGB color = {0, 0, 0};
		if (_this->_movie.m_pGameModule->m_pPaletteTextures && _this->_movie.m_pGameModule->m_pPaletteTextures[0])
		{
			color = _this->_movie.m_pGameModule->m_pPaletteTextures[0]->palette[mat->m_dwColorId];
		}
		tex->m_pSurface = CreateDirect3DTextureFromMemory(g_pDirect3DDevice, tex->m_iWidth, tex->m_iHeight, tex->m_iDepth, tex->m_pImageData, true, color);
	}

	//tex->flags |= 2;
}

__declspec(naked) void TextureInitColorKeyAsm()
{
	__asm
	{
		push ebx
		push edi
		push esi

		mov ecx, ebx // TheGame
		mov edx, edi // Texture
		push esi
		call TextureInitColorKey

		pop esi
		pop edi
		pop ebx
		mov eax, 0x5DB51F
		jmp eax
	}
}

void Texture::InitHooks()
{
	//CPatch::RedirectJump(0x5BADC0, TextureInit);
	CPatch::RedirectJump(0x5BB5F0, TextureCopyDataToSurface);
	CPatch::RedirectJump(0x5B8150, TextureDrawTexture);
	CPatch::RedirectJump(0x5BA7A0, TextureReinit);
	CPatch::RedirectJump(0x5BADC0, TextureInitSurface);
	CPatch::RedirectJump(0x5BBB80, TextureInitSurface2);
	CPatch::RedirectJump(0x50C090, TextureCreateFromScreen);

	CPatch::RedirectJump(0x5E8893, TextureLoadFromFile1);
	CPatch::RedirectJump(0x5E88B8, TextureLoadFromFile2);

	CPatch::Nop(0x5E8939, 14); // remove edit texture type

	// fix converting colors
	CPatch::RedirectJump(0x5E8845, TextureConvertColor5);
	CPatch::RedirectJump(0x5E8876, TextureConvertColor3);
	CPatch::RedirectJump(0x5E889D, TextureConvertColor2);

	// fix set color key
	CPatch::RedirectJump(0x5DB463, TextureInitColorKeyAsm);
}