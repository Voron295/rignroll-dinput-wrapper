#include "CBlockModelElement.h"
#include "CBlockContainer0x6.h"
#include "CBlockContainer0x7.h"
#include "D3DRenderData.h"
#include "CPatch.h"
#include "CLog.h"
#include "TheGame.h"

extern CLog *logFile;
extern IDirect3DDevice9 *&g_pDirect3DDevice;

extern int gFacesCount;
extern PrimitiveRenderData *g_pCurrentPrimitiveDataProcessing;
extern TheGame *&g_pTheGame;


PrimitiveRenderData *&g_pCurrentPrimitiveDataRendering = *(PrimitiveRenderData**)0x723AB0;
CBlockContainer *&g_pCurrentContainerRendering = *(CBlockContainer**)0x723AB4;

void CBlockModelElement::InitHooks()
{
	CPatch::RedirectJump(0x5F16E0, &CBlockModelElement::RenderHook);
	CPatch::RedirectJump(0x5F0AD0, &CBlockModelElement::InitPrimitiveDataHook);
	CPatch::RedirectJump(0x5F091D, &CBlockModelElement::LoadFromFileFlag3);
}

extern DWORD &gRenderFlags;
void CBlockModelElement::RenderFace(D3DRenderData *a1, CFace *face)
{
	if (!g_pCurrentPrimitiveDataRendering->m_pVertexBuf || !face->m_pPrimitiveRenderData.m_pVertexIndicesBuf)
		return;

	g_pDirect3DDevice->SetStreamSource(0, g_pCurrentPrimitiveDataRendering->m_pVertexBuf, 0, sizeof(PrimitiveDataVertex));
	g_pDirect3DDevice->SetIndices(face->m_pPrimitiveRenderData.m_pVertexIndicesBuf);
	g_pDirect3DDevice->SetFVF(PrimitiveDataVertexType);

	a1->SetRenderMaterial(face->m_wMaterialId, face->internalFormat & 1, 0);
	if (face->internalFormat & 4)
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//int type = *(int *)(((DWORD)&a1) + 0x21AC);
	int type = 6 - (*((__int8*)&(face->internalFormat)) < 0);
	if (FAILED(g_pDirect3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)type, 0, 0, g_pCurrentPrimitiveDataRendering->m_dwVertexCount, 0, face->m_wVertsCount - 2)))
	{
		type = 0;
	}

	if (face->internalFormat & 4)
		g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

void __fastcall CBlockModelElement::RenderHook(CBlockModelElement *block, int nop, int a2, D3DRenderData *a3)
{
	if (!g_pCurrentPrimitiveDataRendering)
		return;

	//logFile->WriteLine("RenderModelElement, faces count: %d", block->m_dwFacesCount);
	if (block->m_wFlag == 3)
	{
		LPDIRECT3DTEXTURE9 d3dtexture = 0;
		if (block->m_pPrimitiveRenderData.m_pVertexIndicesBuf) 
		{
			a3->SetRenderMaterial(block->m_wMaterialId, 1, 0);

			g_pDirect3DDevice->SetStreamSource(0, g_pCurrentPrimitiveDataRendering->m_pVertexBuf, 0, sizeof(PrimitiveDataVertex));
			g_pDirect3DDevice->SetIndices(block->m_pPrimitiveRenderData.m_pVertexIndicesBuf);
			g_pDirect3DDevice->SetFVF(PrimitiveDataVertexType);
			if (FAILED(g_pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_pCurrentPrimitiveDataRendering->m_dwVertexCount, 0, block->m_dwFacesCount)))
			{
				return;
			}
			gFacesCount++;
		}
		return;
	}

	if (!block->m_pFaces)
		return;

	gFacesCount++;

	if (a2 & 1) // render direction
	{
		for (int i = block->m_dwFacesCount - 1; i >= 0; i--)
		{
			block->RenderFace(a3, &block->m_pFaces[i]);
		}
	}
	else 
	{
		for (int i = 0; i < block->m_dwFacesCount; i++)
		{
			block->RenderFace(a3, &block->m_pFaces[i]);
		}
	}
}

void CBlockModelElement::InitPrimitiveData2(FILE *f)
{
	m_pPrimitiveRenderData.m_dwIndicesCount = 0;
	m_pPrimitiveRenderData.m_dwBlockSubType = 10;
	if (m_dwFacesCount > 0 && 
		!FAILED(g_pDirect3DDevice->CreateIndexBuffer(
		m_dwFacesCount * 3 * sizeof(short),	// Размер буфера индексов
		D3DUSAGE_WRITEONLY,	// Используем буфер только  для чтения
		D3DFMT_INDEX16, 	// Формат индексов  (для short - D3DFMT_INDEX16)
		D3DPOOL_MANAGED,	// DirectX сам управляет пересозданием буфера
		&m_pPrimitiveRenderData.m_pVertexIndicesBuf,	// Наш указатель  на буфер
		NULL		// Оставим NULL
		)))
	{
		WORD *vertIds;
		m_pPrimitiveRenderData.m_pVertexIndicesBuf->Lock(0, m_dwFacesCount * 3 * sizeof(short), (void**)&vertIds, 0);

		BYTE faceData[0x14];
		WORD verticesData[6];

		m_pPrimitiveRenderData.m_dwIndicesCount = m_dwFacesCount * 3;

		for (int i = 0; i < m_dwFacesCount; i++)
		{
			((void(__cdecl *)(void *, int, int, FILE *))0x61E18F)(faceData, 1, 0x14, f);
			((void(__cdecl *)(void *, int, int, FILE *))0x61E18F)(verticesData, 1, 0xC, f);
			vertIds[i * 3] = verticesData[0];
			vertIds[i * 3 + 1] = verticesData[2];
			vertIds[i * 3 + 2] = verticesData[4];
			
		}
		m_pPrimitiveRenderData.m_pVertexIndicesBuf->Unlock();
		return;
	}
	m_pPrimitiveRenderData.m_pVertexIndicesBuf = NULL;
}

__declspec(naked) void CBlockModelElement::LoadFromFileFlag3()
{
	__asm
	{
		mov ecx, esi
		push ebp
		call CBlockModelElement::InitPrimitiveData2
		mov eax, 0x5F0993
		jmp eax
	}
}

void CBlockModelElement::FaceInitPrimitiveData(CFace *face, Material *materials, GraphicsPalette *plt)
{
	float r = 1.0f, g = 1.0f, b = 1.0f;
	Material *mat = &materials[face->m_wMaterialId];
	if (!(mat->flags & MATERIAL_HAS_TEXTURE))
	{
		r = plt->palette[mat->m_dwColorId].r * 0.00392156862745098f;
		g = plt->palette[mat->m_dwColorId].g * 0.00392156862745098f;
		b = plt->palette[mat->m_dwColorId].b * 0.00392156862745098f;
	}

	if (face->internalFormat & 1 && gRenderFlags & 0x40)
	{
		// no color or intencity
	}
	else
	{
		// colored or with intencity
	}

	if (face->m_wTexCoordsPerVertexCount > 1)
	{
		// use more than one set of UVs
	}



	g_pDirect3DDevice->CreateIndexBuffer(
		face->m_wVertsCount * sizeof(short),	// Размер буфера индексов
		D3DUSAGE_WRITEONLY,	// Используем буфер только  для чтения
		D3DFMT_INDEX16, 	// Формат индексов  (для short - D3DFMT_INDEX16)
		D3DPOOL_MANAGED,	// DirectX сам управляет пересозданием буфера
		&face->m_pPrimitiveRenderData.m_pVertexIndicesBuf,	// Наш указатель  на буфер
		NULL		// Оставим NULL
		);
	WORD *vertIds;
	m_pPrimitiveRenderData.m_dwIndicesCount += face->m_wVertsCount;
	face->m_pPrimitiveRenderData.m_pVertexIndicesBuf->Lock(0, face->m_wVertsCount * sizeof(short), (void**)&vertIds, 0);
	memcpy(vertIds, face->m_pVertsIndices, face->m_wVertsCount * sizeof(short));

	PrimitiveDataVertex *vertices = NULL;
	if (g_pCurrentPrimitiveDataProcessing)
	{
		g_pCurrentPrimitiveDataProcessing->m_pVertexBuf->Lock(0, 0, (void**)&vertices, 0);
		for (int j = 0; j < face->m_wVertsCount; j++)
		{
			if (face->internalFormat & 2)
			{
				vertices[vertIds[j]].u = face->m_pTexCoordsU[j];
				vertices[vertIds[j]].v = face->m_pTexCoordsV[j];
			}

			if (face->m_wTexCoordsPerVertexCount > 1)
			{
				
			}

			if (!(face->internalFormat & 1 && gRenderFlags & 0x40))
			{
				float transp = 1.0;
				if (mat->flags & MATERIAL_IS_TRANSPARENT)
					transp = 1.0 - mat->m_fTransparency;

				if (face->internalFormat & 0x10)
				{
					if (face->internalFormat & 0x20)
					{
						vertices[vertIds[j]].color = D3DCOLOR_ARGB((BYTE)(transp * 255.0f),
							(BYTE)(r * face->m_fIntencityValues[j] * 255.0f),
							(BYTE)(g * face->m_fIntencityValues[j] * 255.0f),
							(BYTE)(b * face->m_fIntencityValues[j] * 255.0f)
							);
					}
					else
					{
						vertices[vertIds[j]].color = D3DCOLOR_ARGB((BYTE)(transp * 255.0f),
							(BYTE)(r * face->field_1C * 255.0f),
							(BYTE)(g * face->field_1C * 255.0f),
							(BYTE)(b * face->field_1C * 255.0f)
							);
					}
				}
				else
				{
					vertices[vertIds[j]].color = D3DCOLOR_ARGB((BYTE)(transp * 255.0f),
						(BYTE)(r * face->field_1C * 255.0f),
						(BYTE)(g * face->field_1C * 255.0f),
						(BYTE)(b * face->field_1C * 255.0f)
						);
				}
			}
		}
		g_pCurrentPrimitiveDataProcessing->m_pVertexBuf->Unlock();
	}

	face->m_pPrimitiveRenderData.m_pVertexIndicesBuf->Unlock();
}

void __fastcall CBlockModelElement::InitPrimitiveDataHook(CBlockModelElement *block, int nop, InitPrimDataScan *a2)
{
	block->m_pPrimitiveRenderData.m_pVertexBuf = NULL;
	block->m_pPrimitiveRenderData.m_dwVertexCount = 0;


	if (block->m_wFlag != 3)  // составной
	{
		if (!block->m_pFaces)
			return;

		for (int i = 0; i < block->m_dwFacesCount; i++)
		{
			CFace *face = &block->m_pFaces[i];

			block->FaceInitPrimitiveData(face, a2->m_pMaterials, a2->m_pPalette);
		}
	}
}