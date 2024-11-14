#include "Mask.h"
#include "CPatch.h"
#include "CWinApp.h"
#include "GraphicsModFuncs.h"

extern CWinApp &g_WinApp;
extern IDirect3DDevice9 *&g_pDirect3DDevice;

void __fastcall MaskInitSurface(Mask *msk)
{
	GraphicsPalette *palette = g_WinApp.pCurrentScreenElement->pGraphicsData->m_pPalette;//gTheGame->m_pLoadRes->m_pPaletteTextures[0];
	
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
						//id = id & 0x7F;
						((RGBA*)data)->r = palette->palette[id].r;
						((RGBA*)data)->g = palette->palette[id].g;
						((RGBA*)data)->b = palette->palette[id].b;
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

	msk->m_pSurface = CreateDirect3DTextureFromMemory(g_pDirect3DDevice, msk->m_iWidth, msk->m_iHeight, 4, pData, false);
	free(pData);
	msk->flags |= 2;
}

void MaskDraw(GraphicsData *gData, int x, int y, Mask *msk, DWORD flags)
{
	if (!msk || !(msk->flags & 1))
	{
		return;
	}
	if (!msk->m_pSurface)
	{
		MaskInitSurface(msk);
	}
	D3DXVECTOR2 posStart, posEnd, uvStart, uvEnd;

	posStart.x = x;
	posStart.y = y;
	posEnd.x = x + msk->m_iWidth;
	posEnd.y = y + msk->m_iHeight;
	uvStart.x = 0.0f;
	uvStart.y = 0.0f;
	uvEnd.x = 1.0f;
	uvEnd.y = 1.0f;

	//g_pDirect3DDevice->BeginScene();
	RenderQuadPrimitive(g_pDirect3DDevice, msk->m_pSurface, &D3DXVECTOR2( msk->m_iWidth, msk->m_iHeight ), &posStart, &posEnd, &uvStart, &uvEnd, D3DCOLOR_XRGB(255, 255, 255));
	//g_pDirect3DDevice->EndScene();
}

void Mask::InitHooks()
{
	CPatch::RedirectJump(0x5BBFF0, MaskInitSurface);
	CPatch::RedirectJump(0x5B8910, MaskDraw);
}