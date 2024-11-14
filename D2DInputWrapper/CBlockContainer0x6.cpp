#include "CBlockContainer0x6.h"
#include "CPatch.h"
#include "D3DRenderData.h"

extern IDirect3DDevice9 *&g_pDirect3DDevice;
extern PrimitiveRenderData *g_pCurrentPrimitiveDataProcessing;
extern PrimitiveRenderData *&g_pCurrentPrimitiveDataRendering;
extern CBlockContainer *&g_pCurrentContainerRendering;

void CBlockContainer0x6::InitHooks()
{
	CPatch::RedirectJump(0x5F33A0, &CBlockContainer0x6::InitPrimitiveDataHook);
	CPatch::RedirectJump(0x5F2F00, &CBlockContainer0x6::RenderHook);
}

void __fastcall CBlockContainer0x6::InitPrimitiveDataHook(CBlockContainer0x6 *block, int nop, void *a2)
{
	block->m_pPrimitiveRenderData.m_pVertexBuf = NULL;
	block->m_pPrimitiveRenderData.m_pVertexIndicesBuf = NULL;
	block->m_pPrimitiveRenderData.m_dwVertexCount = 0;
	block->m_pPrimitiveRenderData.m_dwIndicesCount = 0;
	block->m_pPrimitiveRenderData.m_dwBlockType = 6;
	block->m_pPrimitiveRenderData.m_dwBlockSubType = block->flag_0x6_or_0x24;

	PrimitiveDataVertex *vertices;
	WORD *verticesInd;
	if (!FAILED(g_pDirect3DDevice->CreateVertexBuffer(
		block->m_dwVerticesCount * sizeof(PrimitiveDataVertex),	// Размер буфера
		D3DUSAGE_WRITEONLY,	// Используем буфер только  для чтения
		PrimitiveDataVertexType,	// Формат наших вершин
		D3DPOOL_MANAGED,	// DirectX сам управляет пересозданием буфера
		&block->m_pPrimitiveRenderData.m_pVertexBuf,	// Наш указатель  на буфер
		NULL		// Оставим NULL
		)))
	{
		block->m_pPrimitiveRenderData.m_pVertexBuf->Lock(0, block->m_dwVerticesCount * sizeof(PrimitiveDataVertex), (void**)&vertices, 0);
		block->m_pPrimitiveRenderData.m_dwVertexCount = block->m_dwVerticesCount;
		for (int i = 0; i < block->m_dwVerticesCount; i++)
		{
			vertices[i].position.x = block->m_pVertices[i].pos.x;
			vertices[i].position.y = block->m_pVertices[i].pos.y;
			vertices[i].position.z = block->m_pVertices[i].pos.z;

			if (block->m_pVertexNormals)
			{
				vertices[i].normal.x = block->m_pVertexNormals[i].x;
				vertices[i].normal.y = block->m_pVertexNormals[i].y;
				vertices[i].normal.z = block->m_pVertexNormals[i].z;
			}
			else
			{
				vertices[i].normal.x = .0f;
				vertices[i].normal.y = .0f;
				vertices[i].normal.z = 1.0f;
			}

			vertices[i].u = block->m_pVertices[i].u;
			vertices[i].v = block->m_pVertices[i].v;
			vertices[i].u2 = 0;
			vertices[i].v2 = 0;

			vertices[i].color = D3DCOLOR_XRGB(255, 255, 255);
		}
	}
	block->m_pPrimitiveRenderData.m_pVertexBuf->Unlock();
	g_pCurrentPrimitiveDataProcessing = &block->m_pPrimitiveRenderData;
}

int CBlockContainer0x6::RenderHook(CBlockContainer0x6 *block, int nop, int a2, D3DRenderData *a3)
{
	if (!block->CSimpleBlock::Render(a2, a3))
		return 0;

	CMatrix out;
	BOOL useIt = block->field_70->GetMatrix(&out);
	a3->SetupWorldTransform(&out, useIt);

	g_pCurrentPrimitiveDataRendering = &block->m_pPrimitiveRenderData;
	g_pCurrentContainerRendering = block;

	if (a3->IsSphereVisible(&block->pos))
		block->RenderChild(0, a2, a3);

	g_pCurrentPrimitiveDataRendering = 0;
	g_pCurrentContainerRendering = 0;
	return 1;
}