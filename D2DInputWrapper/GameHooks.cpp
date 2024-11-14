#include "Hooks.h"
#include "CPatch.h"
#include "GameStructs.h"
#include <Texture.h>
#include <Mask.h>
#include "CLog.h"
#include "RGB.h"

extern CLog *logFile;
extern DDPIXELFORMAT &g_PixelFormat555;
extern DDPIXELFORMAT &g_PixelFormat4444;
extern DDPIXELFORMAT &g_PixelFormat565;
extern DDPIXELFORMAT &stru_720748;
extern DDPIXELFORMAT g_PixelFormat888;
extern DDPIXELFORMAT g_PixelFormat8888;
extern DDPIXELFORMAT g_PixelFormat8;

extern IDirectDraw7 *&g_pDirectDraw;
extern IDirectDraw7 *&g_pDirectDraw7;
extern IDirectDrawSurface7 *&g_pDirectDrawFrontBuffer;
extern IDirect3D7 *&g_pDirect3D;
extern IDirect3DDevice7 *&g_pDirect3DDevice;
extern IDirectDrawSurface7 *&g_pSurfaceBuffer;
extern IDirectDrawSurface7 *&g_pSurfaceBufferZ;
extern IDirectDrawPalette *&g_pDirectDrawPalette;

bool g_bGameInitialised = false;

int __fastcall TextureInit(Texture *tex, int nop, char flags)
{
	logFile->WriteLine("Init texture, flags: 0x%X: %s, %dx%dx%d, format: %d", flags, tex->m_sName, tex->m_iWidth, tex->m_iHeight, tex->m_iDepth, tex->m_dTextureFormat);
	if (tex->flags & 2) // loaded
		return 1;

	DDSURFACEDESC2 desc = { 0 };
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = tex->m_iWidth;
	desc.dwHeight = tex->m_iHeight;

	if (g_bGameInitialised && !(flags & 2) && tex->texelsCount)
		desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
	else
		desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;

	switch (tex->m_dTextureFormat)
	{
	case 2:
		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat565, sizeof(DDPIXELFORMAT));
		break;

	case 3:
		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat555, sizeof(DDPIXELFORMAT));
		break;

	case 5:
		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat4444, sizeof(DDPIXELFORMAT));
		break;

	case 6:
		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat888, sizeof(DDPIXELFORMAT));
		break;

	case 7:
		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat8888, sizeof(DDPIXELFORMAT));
		break;

	}

	/*if(tex->texelsCount)
	{
	desc.dwFlags |= DDSD_MIPMAPCOUNT;
	desc.dwMipMapCount = tex->m_dwMimMapCount;
	desc.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
	}*/

	IDirectDrawSurface7 *surface;
	HRESULT hr = g_pDirectDraw->CreateSurface(&desc, &surface, 0);
	if (hr)
	{
		logFile->WriteLine("CreateSurface fail: 0x%X", hr);
		return 0;
	}

	DDSURFACEDESC2 lockDesc = { 0 };
	lockDesc.dwSize = sizeof(lockDesc);
	lockDesc.dwFlags = DDSD_ALL;
	if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
	{
		char *source = 0;
		source = tex->m_pImageData;
		if (flags & 1)
		{
			((int(__thiscall *)(StreamManager *, char *offset, int size))0x5E6B60)(tex->m_pStreamManager, tex->sectionStart + tex->m_dOffset,
				tex->m_iWidth * tex->m_iHeight * tex->m_iDepth); // preread data
		}

		if (source || flags & 1)
		{
			for (int i = 0; i < tex->m_iHeight; i++)
			{
				if (flags & 1)
				{
					((int(__thiscall *)(StreamManager *, char *offset, char *dest, signed int size))0x05E6C10)(tex->m_pStreamManager,
						tex->sectionStart + tex->m_dOffset + tex->m_iWidth * i * tex->m_iDepth,
						(char*)lockDesc.lpSurface + lockDesc.lPitch * i, tex->m_iWidth * tex->m_iDepth);
				}
				else
				{
					memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + tex->m_iWidth * i * tex->m_iDepth, tex->m_iWidth * tex->m_iDepth);
				}
			}
		}
		else
		{
			logFile->WriteLine("Source is empty");
		}
		surface->Unlock(0);
		tex->m_pSurface = surface;
		tex->__LPDIRECTDRAWSURFACE7 = surface;
		tex->flags |= 2;
		return 1;
	}
	else
	{
		surface->Release();
	}

	return 1;
}

int __fastcall TextureReInit(Texture *tex, int nop, double a2)
{
	if (!(tex->flags & 2))
		return 0;

	if (tex->__LPDIRECTDRAWSURFACE7->IsLost() == DDERR_SURFACELOST)
	{
		if (tex->__LPDIRECTDRAWSURFACE7->Restore())
			return 0;

		auto surface = tex->__LPDIRECTDRAWSURFACE7;
		DDSURFACEDESC2 lockDesc = { 0 };
		lockDesc.dwSize = sizeof(lockDesc);
		lockDesc.dwFlags = DDSD_ALL;
		if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
		{
			char *source = 0;
			source = tex->m_pImageData;

			if (source)
			{
				for (int i = 0; i < tex->m_iHeight; i++)
				{
					memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + tex->m_iWidth * i * tex->m_iDepth, tex->m_iWidth * tex->m_iDepth);
				}
			}
			else
			{
				logFile->WriteLine("Source is empty");
			}
			surface->Unlock(0);
			tex->m_pSurface = surface;
			tex->__LPDIRECTDRAWSURFACE7 = surface;
			tex->flags |= 2;
			return 1;
		}
		else
		{
			surface->Release();
		}
	}
	return 1;
}

__declspec(naked) int TextureLoadFromFile(Texture *tex, FILE *f, DWORD offset)
{
	__asm
	{
		push ebx
			push[esp + 4 + 0xC]
			push[esp + 8 + 8]
			push[esp + 0xC + 4]
			mov eax, 0x5E8100
			call eax
			add esp, 0xC
			pop ebx
			retn
	}
	//((int(__cdecl *)(Texture *, FILE *, DWORD))0x5E8100)(tex, f, offset);
}

void __fastcall TextureLoad(Texture *tex)
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
				TextureLoadFromFile(tex, tex->m_pFile, tex->m_dOffset);
			}
			else
			{

			}
		}

		if (tex->m_pImageData && !tex->m_pSurface)
			TextureInit(tex, 0, 2);
	}
}

int __fastcall MaskInit(Mask *msk)
{
	logFile->WriteLine("Init mask: %s, %dx%d, 0x%X", msk->m_sName, msk->m_iWidth, msk->m_iHeight, msk->m_pDataOffset);
	if (!msk->m_pDataOffset)
		return 0;
	GraphicsPalette *palette = *(GraphicsPalette **)(*(DWORD *)((DWORD)g_pCurrentScreenElement + 0xC) + 0x54);

	char *pData = (char*)malloc(msk->m_iHeight * msk->m_iWidth * 4);
	char *data = pData;
	BYTE offset = 0;
	char *internalData = (char*)msk->m_pDataOffset;
	for (int i = 0; i < msk->m_iHeight; i++)
	{
		for (int j = 0; j < msk->m_iWidth; j += offset)
		{
			unsigned char id;
			id = *internalData++;

			//*(BYTE*)data = id;
			offset = id & 0x7F;

			if (id & 0x80)
			{
				memset(data, 0, offset * 4);
				data += offset * 4;
			}
			else
			{
				for (int k = 0; k < offset; k++)
				{
					if (msk->m_dDepth == 1)
					{
						id = *internalData++;
						((RGBA*)data)->r = palette->palette[id].b;
						((RGBA*)data)->g = palette->palette[id].g;
						((RGBA*)data)->b = palette->palette[id].r;
						((RGBA*)data)->a = (!id) ? 0 : 255;
					}
					else if (msk->m_dDepth == 2)
					{
						WORD color = *(WORD*)internalData;
						internalData += 2;
						BYTE r, g, b;
						GetRGBFrom565(color, r, g, b);
						((RGBA*)data)->r = r;
						((RGBA*)data)->g = g;
						((RGBA*)data)->b = b;
						((RGBA*)data)->a = (!color) ? 0 : 255;
					}
					data += 4;
				}
			}
		}
	}

	DDSURFACEDESC2 desc = { 0 };
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = msk->m_iWidth;
	desc.dwHeight = msk->m_iHeight;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;

	//memcpy(&desc.ddpfPixelFormat, &g_PixelFormat8888, sizeof(DDPIXELFORMAT));
	IDirectDrawSurface7 *surface;
	HRESULT hr = g_pDirectDraw->CreateSurface(&desc, &surface, 0);
	if (hr)
	{
		logFile->WriteLine("CreateSurface fail: 0x%X", hr);
		return 0;
	}

	DDCOLORKEY colorKey = { 0 };
	surface->SetColorKey(DDCKEY_SRCBLT, &colorKey);

	DDSURFACEDESC2 lockDesc = { 0 };
	lockDesc.dwSize = sizeof(lockDesc);
	lockDesc.dwFlags = DDSD_ALL;
	if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
	{
		char *source = 0;
		source = (char*)pData;

		if (source)
		{
			for (int i = 0; i < msk->m_iHeight; i++)
			{
				memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + msk->m_iWidth * i * 4, msk->m_iWidth * 4);
			}
		}
		else
		{
			logFile->WriteLine("Source is empty");
		}
		surface->Unlock(0);

		free(pData);
		msk->m_pSurface = surface;
	}
	else
	{
		free(pData);
		return 0;
	}
	return 1;
}

void MaskReInit(Mask *msk)
{
	if (msk->m_pSurface->IsLost() == DDERR_SURFACELOST)
	{
		if (msk->m_pSurface->Restore())
			return;
	}

	GraphicsPalette *palette = *(GraphicsPalette **)(*(DWORD *)((DWORD)g_pCurrentScreenElement + 0xC) + 0x54);

	char *pData = (char*)malloc(msk->m_iHeight * msk->m_iWidth * 4);
	char *data = pData;
	BYTE offset = 0;
	char *internalData = (char*)msk->m_pDataOffset;
	for (int i = 0; i < msk->m_iHeight; i++)
	{
		for (int j = 0; j < msk->m_iWidth; j += offset)
		{
			unsigned char id;
			id = *internalData++;

			//*(BYTE*)data = id;
			offset = id & 0x7F;

			if (id & 0x80)
			{
				memset(data, 0, offset * 4);
				data += offset * 4;
			}
			else
			{
				for (int k = 0; k < offset; k++)
				{
					if (msk->m_dDepth == 1)
					{
						id = *internalData++;
						((RGBA*)data)->r = palette->palette[id].b;
						((RGBA*)data)->g = palette->palette[id].g;
						((RGBA*)data)->b = palette->palette[id].r;
						((RGBA*)data)->a = (!id) ? 0 : 255;
					}
					else if (msk->m_dDepth == 2)
					{
						WORD color = *(WORD*)internalData;
						internalData += 2;
						BYTE r, g, b;
						GetRGBFrom565(color, r, g, b);
						((RGBA*)data)->r = r;
						((RGBA*)data)->g = g;
						((RGBA*)data)->b = b;
						((RGBA*)data)->a = (!color) ? 0 : 255;
					}
					data += 4;
				}
			}
		}
	}
	auto surface = msk->m_pSurface;
	DDSURFACEDESC2 lockDesc = { 0 };
	lockDesc.dwSize = sizeof(lockDesc);
	lockDesc.dwFlags = DDSD_ALL;
	if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
	{
		char *source = 0;
		source = (char*)pData;

		if (source)
		{
			for (int i = 0; i < msk->m_iHeight; i++)
			{
				memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + msk->m_iWidth * i * 4, msk->m_iWidth * 4);
			}
		}
		else
		{
			logFile->WriteLine("Source is empty");
		}
		surface->Unlock(0);
		msk->m_pSurface = surface;
	}
	
	free(pData);
}

void DrawMask(GraphicsData *a1, int x, int y, Mask *mask, int a5)
{
	if (!mask)
		return;

	if (!mask->m_pSurface)
	{
		MaskInit(mask);
	}
	if (!mask->m_pSurface)
		return;

	DDBLTFX fx = { 0 };
	fx.dwSize = sizeof(fx);
	fx.dwROP = SRCCOPY;

	RECT targetRect, sourceRect;
	targetRect.left = x;
	targetRect.top = y;
	targetRect.right = x + mask->m_iWidth;
	targetRect.bottom = y + mask->m_iHeight;

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.right = mask->m_iWidth;
	sourceRect.bottom = mask->m_iHeight;

	if (x < 0)
	{
		sourceRect.left = -x;
		targetRect.left = 0;
	}
	if (y < 0)
	{
		sourceRect.top = -y;
		targetRect.top = 0;
	}
	if (sourceRect.right > a1->width)
	{
		sourceRect.right = sourceRect.left + a1->width - sourceRect.right;
		targetRect.right = a1->width;
	}
	if (sourceRect.bottom > a1->height)
	{
		sourceRect.bottom = sourceRect.top + a1->height - sourceRect.bottom;
		targetRect.bottom = a1->height;
	}

	g_pDirect3DDev->EndScene();
	g_pSurfaceBuffer->Blt(&targetRect, mask->m_pSurface, &sourceRect, DDBLT_WAIT | DDBLT_KEYSRC, &fx);
	g_pDirect3DDev->BeginScene();

}

void __fastcall TextureRelease(Texture *tex)
{
	if (tex->m_pSurface)
		tex->m_pSurface->Release();
}

void DrawTextureHook(GraphicsData *a1, Texture *tex, RECT *textureRect, int x, int y)
{
	/*CVector pos = { x, y, 0 };
	CVector size = { (float)(textureRect->right - textureRect->left), (float)(textureRect->bottom - textureRect->top), 0.0f };
	CVector uv1 = { (float)textureRect->left / a2->m_iWidth, (float)textureRect->top / a2->m_iHeight, 0 };
	CVector uv2 = { (float)textureRect->right / a2->m_iWidth, (float)textureRect->bottom / a2->m_iHeight, 0 };
	*/
	if (!tex->__LPDIRECTDRAWSURFACE7)
	{
		((int(__thiscall *)(Texture *tex, char))0x5BADC0)(tex, 2);
		//logFile->WriteLine("surface is empty!");
	}
	//if (a2->__LPDIRECTDRAWSURFACE7->IsLost())
		//TextureReInit(a2, 0, 1.0);
	//RenderTexturedQuad(pos, size, { 0, 0, 0 }, a2->__LPDIRECTDRAWSURFACE7, uv1, uv2, 0, 0xFFFFFFFF);
	if (!tex->__LPDIRECTDRAWSURFACE7)
	{
		logFile->WriteLine("surface is empty!");
		return;
	}
	DDBLTFX fx = { 0 };
	fx.dwSize = sizeof(fx);
	fx.dwROP = SRCCOPY;

	RECT targetRect, sourceRect;
	targetRect.left = x;
	targetRect.top = y;
	targetRect.right = x + tex->m_iWidth;
	targetRect.bottom = y + tex->m_iHeight;

	memcpy(&sourceRect, textureRect, sizeof(RECT));
	//logFile->WriteLine("Render Texture rect: %d %d %d %d", textureRect->left, textureRect->top, textureRect->right, textureRect->bottom);
	/*sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.right = tex->m_iWidth;
	sourceRect.bottom = tex->m_iHeight;
	*/
	if (x < 0)
	{
		sourceRect.left = -x;
		targetRect.left = 0;
	}
	if (y < 0)
	{
		sourceRect.top = -y;
		targetRect.top = 0;
	}
	if (sourceRect.right > a1->width)
	{
		sourceRect.right = sourceRect.left + a1->width - sourceRect.right;
		targetRect.right = a1->width;
	}
	if (sourceRect.bottom > a1->height)
	{
		sourceRect.bottom = sourceRect.top + a1->height - sourceRect.bottom;
		targetRect.bottom = a1->height;
	}

	g_pDirect3DDev->EndScene();
	g_pSurfaceBuffer->Blt(&targetRect, tex->__LPDIRECTDRAWSURFACE7, &sourceRect, DDBLT_WAIT | DDBLT_ROP, &fx);
	g_pDirect3DDev->BeginScene();
}

void D3DClearWithColor(GraphicsData *a1, RECT *a2, unsigned int _color, int isRealColor)
{
	
}

// -------------- GraphicsData Functions ----------------------------

// Constructor
GraphicsData *__fastcall GraphicsDataConstructor(GraphicsData *_this, int nop, unsigned __int16 width, unsigned __int16 height, unsigned __int16 depth, int flags)
{
	_this->rect = { 0 };
	_this->m_wPitch = width * depth;
	_this->parent = 0;
	_this->flags = flags;
	_this->width = width;
	_this->height = height;
	_this->depth = depth;
	_this->field_8 = 0;
	_this->bAlphaBlendState = 0;
	_this->m_pPalette = 0;
	_this->opacity_ = 0.5;
	_this->field_5C = 0;
	_this->field_78 = 0;
	_this->m_pMainSurfaceData = 0;
	_this->m_dwCurrentSurface = 0;
	_this->field_14 = 1;
	_this->isD3DInitialized_ = 1;
	_this->lpBackSurface[0] = 0;
	*(DWORD *)0x702EB4 = 3;
	return _this;
}

bool sceneBeginned = false;

void __fastcall GraphicsDataFlipSurface(GraphicsData *a1)
{
	if (sceneBeginned)
	{
		sceneBeginned = false;
		//logFile->WriteLine("Flip Surface");
		g_pDirect3DDevice->EndScene();

		DWORD result = g_pDirectDrawFrontBuffer->Flip(0, DDFLIP_WAIT);
		if (result != DD_OK)
		{
			g_pDirectDraw->RestoreAllSurfaces();
		}
	}
}

void __fastcall GraphicsDataSetupViewport(GraphicsData *a1)
{
	//logFile->WriteLine("Setup Viewport (flags 0x%X) (parent 0x%X) %d %d %d %d %d %d", a1->flags, a1->parent, a1->width, a1->height);
	if (a1->parent && !sceneBeginned)
	{
		sceneBeginned = true;
		g_pDirect3DDevice->BeginScene();
		g_pDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFF00, 1.0, 0);
	}
	if (!a1->parent)
	{
		return;
	}
	RECT rect;
	((RECT*(__thiscall *)(GraphicsData *, RECT *))0x5B66A0)(a1, &rect);
	D3DVIEWPORT7 viewData;
	memset(&viewData, 0, sizeof(D3DVIEWPORT7));
	viewData.dwX = rect.left;
	viewData.dwY = rect.top;
	viewData.dwWidth = a1->width;
	viewData.dwHeight = a1->height;
	viewData.dvMinZ = 0.0f;
	viewData.dvMaxZ = 1.0f;

	g_pDirect3DDevice->SetViewport(&viewData);
}

void __fastcall ScreenElementClearDevice(GraphicsData *a1)
{
	int a2;
	//logFile->WriteLine("ScreenElement (flags 0x%X) (parent 0x%X) (field_14 %d) %d %d", a1->flags, a1->parent, a1->field_14, a1->width, a1->height);
	GraphicsDataSetupViewport(a1);
}

void __fastcall GraphicsDataClearDevice(GraphicsData *a1, int nop, int color, int isRealColor)
{
	if (!sceneBeginned)
	{
		sceneBeginned = true;
		g_pDirect3DDevice->BeginScene();
	}
	g_pDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0, 0);
}

Font *FontLoad(FILE *f)
{
	logFile->WriteLine("Try to create font...");
	Font *font = ((Font *(__cdecl *)(int size))0x6326D3)(sizeof(Font)); // new
	font->numChars = 255;
	font->startChar = 0;
	font->bits = 0;
	font->charStart = 0;
	font->charEnd = 0;
	font->flags = 0;
	font->field_20 = -1;
	font->m_pSurface = 0;

	Texture *tex = ((Texture *(__cdecl *)(FILE *, char *, int))0x5E7710)(f, 0, 0); // LoadTexture
	if (tex)
	{
		//logFile->WriteLine("Font texture loaded (%d) %dx%d...", tex->m_iDepth, tex->m_iWidth, tex->m_iHeight);
		font->charWidthPx = tex->m_iWidth / 16;
		font->charHeightPx = tex->m_iHeight / 16 - 1;
		font->charStart = (char*)malloc(256);//((char *(__cdecl *)(int size))0x6326D3)(256);
		font->charEnd = (char*)malloc(256);//((char *(__cdecl *)(int size))0x6326D3)(256);

		// realloc texture image data
		DWORD *pData = (DWORD*)malloc(tex->m_iWidth * tex->m_iHeight * 4);
		for (int i = 0; i < tex->m_iWidth * tex->m_iHeight; i++)
		{
			BYTE r, g, b;
			WORD color = *(WORD*)&tex->m_pImageData[i * 2];
			if (!color)
			{
				pData[i] = 0;
			}
			else
			{
				GetRGBFrom555(color, r, g, b);
				pData[i] = RGBA_MAKE(r, g, b, 255);
			}
		}

		for (int i = 0; i < 256; i++)
		{
			int offset = font->charWidthPx * (i % 16) + tex->m_iWidth * (i / 16) * (font->charHeightPx + 1);
			//logFile->WriteLine("char %d, x: %d, y: %d, offset: %d", i, font->charWidthPx * (i % 16), i / 16 * (font->charHeightPx + 1), offset);
			DWORD *imgData = &pData[offset];

			char charStart;
			char charEnd;
			for (charStart = 0; charStart < font->charWidthPx; charStart++)
			{
				if (imgData[charStart])
					break;
			}
			for (charEnd = charStart; charEnd < font->charWidthPx; charEnd++)
			{
				if (!imgData[charEnd])
					break;
			}
			font->charStart[i] = charStart;
			if (charEnd)
				charEnd += 1;
			font->charEnd[i] = charEnd;
		}

		DDSURFACEDESC2 desc = { 0 };
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.dwWidth = tex->m_iWidth;
		desc.dwHeight = tex->m_iHeight;
		desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat8888, sizeof(DDPIXELFORMAT));
		
		IDirectDrawSurface7 *surface;
		HRESULT hr = g_pDirectDraw->CreateSurface(&desc, &surface, 0);
		if (hr)
		{
			logFile->WriteLine("CreateSurface fail: 0x%X", hr);
			((void(__thiscall *)(Texture *, int))0x5BA400)(tex, 1); // texture destructor
			free(pData);
			return 0;
		}
		logFile->WriteLine("Font surface created (%d)...", tex->m_dTextureFormat);

		DDSURFACEDESC2 lockDesc = { 0 };
		lockDesc.dwSize = sizeof(lockDesc);
		lockDesc.dwFlags = DDSD_ALL;
		if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
		{
			logFile->WriteLine("Locked surface...");
			char *source = 0;
			source = (char*)pData;

			if (source)
			{
				for (int i = 0; i < tex->m_iHeight; i++)
				{
					memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + tex->m_iWidth * i * 4, tex->m_iWidth * 4);
				}
			}
			else
			{
				logFile->WriteLine("Source is empty");
			}
			surface->Unlock(0);

			font->m_pSurface = surface;
		}
		logFile->WriteLine("Try to release texture...");
		((void (__thiscall *)(Texture *, int))0x5BA400)(tex, 1); // texture destructor
		font->bits = (char*)pData;//free(pData);
	}
	
	font->flags |= 1;
	logFile->WriteLine("Font initialized! 0x%X", font);
	return font;
}

void ReloadFont(Font *font)
{
	if (!font->m_pSurface)
		return;

	while (font->m_pSurface->Restore());
	if (font->bits)
	{
		DWORD *pData = (DWORD*)font->bits;
		DDSURFACEDESC2 desc = { 0 };
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.dwWidth = font->charWidthPx * 16;
		desc.dwHeight = (font->charHeightPx + 1) * 16;
		desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

		memcpy(&desc.ddpfPixelFormat, &g_PixelFormat8888, sizeof(DDPIXELFORMAT));

		IDirectDrawSurface7 *surface = font->m_pSurface;

		DDSURFACEDESC2 lockDesc = { 0 };
		lockDesc.dwSize = sizeof(lockDesc);
		lockDesc.dwFlags = DDSD_ALL;
		if (surface->Lock(0, &lockDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
		{
			logFile->WriteLine("Locked surface...");
			char *source = 0;
			source = (char*)pData;

			if (source)
			{
				for (int i = 0; i < desc.dwHeight; i++)
				{
					memcpy(((BYTE*)lockDesc.lpSurface) + lockDesc.lPitch * i, source + desc.dwWidth * i * 4, desc.dwWidth * 4);
				}
			}
			else
			{
				logFile->WriteLine("Source is empty");
			}
			surface->Unlock(0);

			font->m_pSurface = surface;
		}
	}
}

void RenderTextHook(GraphicsData *surface, int x, int y, int maxLen, Font *font, const char *text, DWORD color, BOOL isRealColor)
{
	//RenderTexturedQuad({ 0, 0, 0 }, { font->charWidthPx * 16, (font->charHeightPx + 1) * 16, 0 }, { 0, 0, 0 }, font->m_pSurface, 0, 0xFFFFFFFF);
	//return;
	if (font->m_pSurface->IsLost())
		ReloadFont(font);

	if (surface->parent)
	{
		x += surface->rect.left;
		y += surface->rect.top;
	}
	int startX = x;
	int charH = font->charHeightPx;
	int charW = font->charWidthPx;
	y -= charH;
	//logFile->WriteLine("charH = %d, charW = %d", charH, charW);
	int startMaxLen = maxLen;
	DWORD _color = 0xFFFFFFFF;
	
	GraphicsPalette *plt = 0;
	while (surface->parent && !surface->m_pPalette)
		surface = surface->parent;

	plt = surface->m_pPalette;
	if (isRealColor)
		_color = color;
	else if (plt)
		_color = RGBA_MAKE(plt->palette[color].r, plt->palette[color].g, plt->palette[color].b, 255);
	
	TLVERTEX vertices[4];
	for (int i = 0; i < 4; i++)
	{
		vertices[i].diffuse = _color;
		vertices[i].specular = 0xFF000000;
		vertices[i].w = 1.0f;
		vertices[i].z = .0001f;
	}

	g_pDirect3DDev->SetTexture(0, font->m_pSurface);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDirect3DDev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, 1);
	g_pDirect3DDev->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);


	while (*text)
	{
		unsigned char _char = *text;
		if (_char == '\n')
		{
			y += charH;
			x = startX;
			maxLen = startMaxLen;
			text++;
			continue;
		}

		int startOffset = font->charStart[_char];
		int endOffset = font->charEnd[_char];
		if (endOffset - startOffset > maxLen)
			return;

		x -= startOffset;
		float charOffsetX = (_char % 16) * 0.0625 + 0.00695;
		float charOffsetY = (_char / 16) * 0.0625;

		//logFile->WriteLine("%c - %f %f", _char, charOffsetX, charOffsetY);

		vertices[0].x = x;
		vertices[0].y = y;
		vertices[0].u = charOffsetX;
		vertices[0].v = charOffsetY + 0.00695;
		vertices[1].x = x + charW;
		vertices[1].y = y;
		vertices[1].u = charOffsetX + 0.0625;
		vertices[1].v = charOffsetY + 0.00695;
		vertices[2].x = x + charW;
		vertices[2].y = y + charH;
		vertices[2].u = charOffsetX + 0.0625;
		vertices[2].v = charOffsetY + 0.0625;
		vertices[3].x = x;
		vertices[3].y = y + charH;
		vertices[3].u = charOffsetX;
		vertices[3].v = charOffsetY + 0.0625;
		
		g_pDirect3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vertices, 4, 0);

		x += endOffset;
		maxLen += startOffset - endOffset;
		text++;
	}
	g_pDirect3DDev->SetTexture(0, 0);
}

void __fastcall FontRelease(Font *font)
{
	if (font->bits)
		free(font->bits);
	if (font->charStart)
		free(font->charStart);
	if (font->charEnd)
		free(font->charEnd);

	if (font->m_pSurface)
		font->m_pSurface->Release();
}

void SetupGameHooks()
{
	CPatch::RedirectJump(0x5BADC0, TextureInit);
	CPatch::RedirectJump(0x5BBFF0, MaskInit);
	CPatch::RedirectJump(0x5BA7A0, TextureLoad);
	//CPatch::RedirectJump(0x5BBB80, TextureReInit);
	CPatch::RedirectJump(0x5B8910, DrawMask);
	CPatch::RedirectJump(0x5B8150, DrawTextureHook);
	CPatch::RedirectJump(0x5BAC20, TextureRelease);
	//CPatch::RedirectJump(0x5B6F20, D3DClearWithColor);
	CPatch::RedirectJump(0x5E8DC0, FontLoad);
	CPatch::RedirectJump(0x5BC280, FontRelease);
	CPatch::RedirectJump(0x5EA150, RenderTextHook);

	// patch graphicsDataConstructor
	CPatch::RedirectJump(0x5B69E0, GraphicsDataConstructor);
	CPatch::RedirectJump(0x5B6AA0, GraphicsDataFlipSurface);
	CPatch::RedirectJump(0x5B6B80, GraphicsDataSetupViewport);
	CPatch::RedirectJump(0x5B6D90, GraphicsDataClearDevice);
	CPatch::RedirectJump(0x50FEC3, ScreenElementClearDevice);

	CPatch::SetChar(0x5AD00B, 0xEB);

	CPatch::RedirectJump(0x516556, (void*)0x516624); // patch cmenugraphics
	
}