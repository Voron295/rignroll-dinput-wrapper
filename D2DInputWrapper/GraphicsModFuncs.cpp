#include "GraphicsModFuncs.h"
#include "CLog.h"
extern CLog *logFile;

extern IDirect3DDevice9 *&g_pDirect3DDevice;

//RGB565 -> RGB888 using tables
BYTE Table5[] = { 0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132,
140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };

BYTE Table6[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61, 65, 69,
73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 130, 134, 138,
142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198,
202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255 };

BYTE Table58[255];
BYTE Table68[255];
bool tablesInit = false;

void InitRGBTables()
{
	BYTE val5 = 0;
	BYTE val6 = 0;
	for (int i = 0; i < 255; i++)
	{
		if (i == Table5[val5 + 1])
			val5++;
			

		if (i == Table6[val6 + 1])
			val6++;

		Table58[i] = val5;
		Table68[i] = val6;
	}
}

void GetRGBFrom565(WORD color, BYTE &r, BYTE &g, BYTE &b)
{
	r = (color & 0xF800) >> 11;
	g = (color & 0x7E0) >> 5;
	b = (color & 0x1F);
	/*
	r = r * 0xFF / 0x1F;
	g = g * 0xFF / 0x3F;
	b = b * 0xFF / 0x1F;*/

	r = Table5[r];
	g = Table6[g];
	b = Table5[b];
}

WORD Get565FromRGB(BYTE r, BYTE g, BYTE b)
{
	if (!tablesInit)
	{
		InitRGBTables();
		tablesInit = true;
	}
		
	
	return (Table58[r] << 11) | (Table68[g] << 5) | Table58[b];
}

WORD Get565From555(WORD color)
{
	BYTE r, g, b;
	r = (color & 0x7C00) >> 10;
	g = (color & 0x3E0) >> 5;
	b = (color & 0x1F);
	b = b * 255 / 31;
	g = g * 255 / 31;
	r = r * 255 / 31;
	return Get565FromRGB(r, g, b);
}

DWORD GetRGBAFrom4444(WORD color)
{
	BYTE out[4];
	out[0] = ((color & 0xF000) >> 12) * 17;
	out[3] = ((color & 0xF00) >> 8) * 17;
	out[2] = ((color & 0xF0) >> 4) * 17;
	out[1] = (color & 0xF) * 17;
	return *(DWORD *)&out;
}

void GetRGBAFrom4444(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a)
{
	a = ((color & 0xF000) >> 12) * 17;
	r = ((color & 0xF00) >> 8) * 17;
	g = ((color & 0xF0) >> 4) * 17;
	b = (color & 0xF) * 17;
}

void GetRGBAFrom555(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a)
{
	a = (color & 0x8000) >> 15;
	r = (color & 0x7C00) >> 10;
	g = (color & 0x3E0) >> 5;
	b = (color & 0x1F);
	a *= 255;
	r = Table5[r];
	g = Table5[g];
	b = Table5[b];
}


IDirect3DTexture9 *CreateDirect3DTextureFromMemory(IDirect3DDevice9 *device, int width, int height, int depth, char *memory, bool useTransparency, RGB transparentColor)
{
	IDirect3DTexture9 *texture;
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = device->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, 0);
	//logFile->WriteLine("Tex Create Err(%d, %d, %d): 0x%X", width, height, depth, hr);
	if (!SUCCEEDED(hr))
		return 0;

	WORD transpColor = (((unsigned int)(unsigned __int8)transparentColor.b >> 3) | 8
		* (transparentColor.g & 0xFC | 32 * (transparentColor.r & 0xF8)));
	
	hr = texture->LockRect(0, &lockedRect, 0, 0);
	if (!SUCCEEDED(hr))
	{
		texture->Release();
		return 0;
	}
	for (int i = 0; i < width * height; i++)
	{
		//bool isTransparent = *((BYTE*)memory + i * 3 + 2) == 0 && *((BYTE*)memory + i * 3 + 1) == 0 && *((BYTE*)memory + i * 3) == 0;
		*((BYTE*)lockedRect.pBits + i * 4) = *((BYTE*)memory + i * depth + 2);
		*((BYTE*)lockedRect.pBits + i * 4 + 1) = *((BYTE*)memory + i * depth + 1);
		*((BYTE*)lockedRect.pBits + i * 4 + 2) = *((BYTE*)memory + i * depth);
		if (depth == 4)
		{
			*((BYTE*)lockedRect.pBits + i * 4 + 3) = *((BYTE*)memory + i * 4 + 3);
		}
		else
		{
			WORD curColor = Get565FromRGB(*((BYTE*)memory + i * depth), *((BYTE*)memory + i * depth + 1), *((BYTE*)memory + i * depth + 2));
			if (useTransparency && curColor == transpColor)
			{
				*((BYTE*)lockedRect.pBits + i * 4 + 0) = 0;
				*((BYTE*)lockedRect.pBits + i * 4 + 1) = 0;
				*((BYTE*)lockedRect.pBits + i * 4 + 2) = 0;
				*((BYTE*)lockedRect.pBits + i * 4 + 3) = 0;
			}
			else
				*((BYTE*)lockedRect.pBits + i * 4 + 3) = 255;
		}
	}
	texture->UnlockRect(0);

	return texture;
}

//LPD3DXSPRITE renderSprite = 0;

struct CVertexRHW
{
	float	x, y, z, rhw;
	DWORD	color;
	float	tx, ty;
};


HRESULT RenderQuadPrimitive(IDirect3DDevice9 *device, IDirect3DTexture9 *texture, LPD3DXVECTOR2 realSize, LPD3DXVECTOR2 posStart, LPD3DXVECTOR2 posEnd, LPD3DXVECTOR2 uvStart, LPD3DXVECTOR2 uvEnd, D3DCOLOR color, bool useAlpha)
{
	float width = realSize->x;
	float height = realSize->y;
	static CVertexRHW spriteV[] =
	{
		{ 0.0f, 0.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f, },
		{ 1.0f, 0.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0.0f, },
		{ 0.0f, 1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 1.0f, },
		{ 1.0f, 1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 1.0f, },
	};

	static LPDIRECT3DVERTEXBUFFER9 MyVertexBuffer = 0;
	static LPDIRECT3DINDEXBUFFER9 MyIndexBuffer = 0;

	short inds[6] =
	{
		0, 1, 2,
		0, 2, 3
	};
	spriteV[0].x = posStart->x;
	spriteV[0].y = posStart->y;
	spriteV[1].x = posEnd->x;
	spriteV[1].y = posStart->y;
	spriteV[3].x = posEnd->x;
	spriteV[3].y = posEnd->y;
	spriteV[2].x = posStart->x;
	spriteV[2].y = posEnd->y;

	spriteV[0].tx = uvStart->x + 0.5f / width;
	spriteV[0].ty = uvStart->y + 0.5f / height;
	spriteV[1].tx = uvEnd->x + 0.5f / width;
	spriteV[1].ty = uvStart->y + 0.5f / height;
	spriteV[3].tx = uvEnd->x + 0.5f / width;
	spriteV[3].ty = uvEnd->y + 0.5f / height;
	spriteV[2].tx = uvStart->x + 0.5f / width;
	spriteV[2].ty = uvEnd->y + 0.5f / height;
	// устанавливаем формат вершин

	//device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);

	device->SetRenderState(D3DRS_FOGENABLE, 0);
	
	DWORD culling;
	device->GetRenderState(D3DRS_CULLMODE, &culling);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	device->SetTexture(0, texture);
	HRESULT result = device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, spriteV, sizeof(CVertexRHW));

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);

	device->SetRenderState(D3DRS_FOGENABLE, 1);

	device->SetRenderState(D3DRS_CULLMODE, culling);
	return result;
}

HRESULT RenderLinePrimitive(IDirect3DDevice9 *device, LPD3DXVECTOR2 posStart, LPD3DXVECTOR2 posEnd, D3DCOLOR color)
{
	static CVertexRHW spriteV[] =
	{
		{ 0.0f, 0.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255)},
		{ 1.0f, 1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255)}
	};

	spriteV[0].x = posStart->x;
	spriteV[0].y = posStart->y;
	spriteV[1].x = posEnd->x;
	spriteV[1].y = posEnd->y;
	spriteV[0].color = color;
	spriteV[1].color = color;


	// Set the source blend state.
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// Set the destination blend state.

	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	device->SetRenderState(D3DRS_FOGENABLE, 0);

	// устанавливаем формат вершин
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->SetTexture(0, 0);
	HRESULT result = device->DrawPrimitiveUP(D3DPT_LINELIST, 1, spriteV, sizeof(CVertexRHW));

	device->SetRenderState(D3DRS_FOGENABLE, 1);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	return result;
}

void __fastcall D3DRenderDataRenderFace(D3DRenderData *data, int nop, CFace *face)
{

}

PrimitiveRenderData *&g_pCurrentRenderingPrimitiveData = *(PrimitiveRenderData**)0x723AB0;

void D3DRenderDataRenderPrimitive(D3DRenderData *a1, PrimitiveRenderData *data, Material *material, CVector *pos, D3DPRIMITIVETYPE primType)
{
	/*if (data->m_pwIndices)
	{
		DWORD numVertices = g_pCurrentRenderingPrimitiveData->dwIndexCount;
		DWORD indexCount = data->dwIndexCount;
		DWORD primitiveCount = indexCount / 3;
		WORD *lpwIndices = data->m_pwIndices;

		VOID *lpVertices = g_pCurrentRenderingPrimitiveData->m_pPrimitiveStrideData->position.lpvData;
		DWORD vertStride = g_pCurrentRenderingPrimitiveData->m_pPrimitiveStrideData->position.dwStride;

		HRESULT result = g_pDirect3DDevice->DrawIndexedPrimitiveUP(primType, 0, numVertices, primitiveCount, lpwIndices, D3DFMT_INDEX16, lpVertices, vertStride);

	}*/
}