#ifndef CFACE_STRUCT_H
#define CFACE_STRUCT_H

#include <Windows.h>
#include "CVertex.h"
#include "CVector4.h"
#include <d3d9.h>

#pragma pack(push, 1)
struct D3DDP_PTRSTRIDE
{
	LPVOID lpvData;
	int dwStride;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct D3DDRAWPRIMITIVESTRIDEDDATA
{
	D3DDP_PTRSTRIDE position;
	D3DDP_PTRSTRIDE normal;
	D3DDP_PTRSTRIDE diffuse;
	D3DDP_PTRSTRIDE specular;
	D3DDP_PTRSTRIDE textureCoords[8];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PrimitiveVertexInfo
{
	CVector m_pVertexPos;
	CVector m_pVertexNormal;
	int field_18;
	int field_1C;
};
#pragma pack(pop)


const DWORD PrimitiveDataVertexType = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2;

struct PrimitiveDataVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DCOLOR    color;
	float u;
	float v;
	float u2;
	float v2;
};

struct PrimitiveRenderData
{
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuf;
	LPDIRECT3DINDEXBUFFER9 m_pVertexIndicesBuf;
	DWORD m_dwVertexCount;
	DWORD m_dwIndicesCount;
	DWORD m_dwBlockType;
	DWORD m_dwBlockSubType;
	BYTE nop[0x24 - 24];
};

static_assert (sizeof(PrimitiveRenderData) == 0x24, "Size is not correct");

/*
#pragma pack(push, 1) // size = 0x140
struct PrimitiveRenderData
{
	__int16 dwIndexCount;
	__int16 field_2;
	D3DDRAWPRIMITIVESTRIDEDDATA *m_pPrimitiveStrideData;
	int field_8;
	BYTE field_C;
	char field_D;
	char field_E;
	char field_F;
	UV *m_pUVs;
	void *field_14;
	PrimitiveVertexInfo *m_pVerts;
	PrimitiveVertexInfo *m_pVertsMatData;
	WORD *m_pwIndices;
};
#pragma pack(pop)
*/

#pragma pack(push, 1)
struct CFace
{
	__int16 internalFormat;
	__int16 field_2;
	__int16 m_wVertsCount;
	__int16 m_wTexCoordsPerVertexCount;
	WORD *m_pVertsIndices;
	float *m_pTexCoordsU;
	float *m_pTexCoordsV;
	float *m_fIntencityValues;
	CVector *m_pVertices;
	float field_1C;
	CVector4 m_FaceNormal;
	__int16 m_wMaterialId;
	__int16 field_32;
	PrimitiveRenderData m_pPrimitiveRenderData;
};
#pragma pack(pop)
static_assert (sizeof(CFace) == 0x58, "Size is not correct");

#endif