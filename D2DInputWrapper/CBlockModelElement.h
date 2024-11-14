#include "CBlockContainer.h"
#include "CFace.h"
#include "InitPrimDataScan.h"

class CBlockModelElement : public CSimpleBlock
{
public:
	CVertex *m_pVertices;
	__int16 m_dwFacesCount;
	__int16 field_36;
	CFace *m_pFaces;
	PrimitiveRenderData m_pPrimitiveRenderData;
	__int16 m_wFlag;
	__int16 m_wMaterialId;

	static void InitHooks();
	void InitPrimitiveData2(FILE *f);
	void FaceInitPrimitiveData(CFace *face, Material *materials, GraphicsPalette *plt);

	// Hooks
	static void __fastcall RenderHook(CBlockModelElement *block, int nop, int a2, D3DRenderData *a3);
	static void __fastcall InitPrimitiveDataHook(CBlockModelElement *block, int nop, InitPrimDataScan *a2);
	void RenderFace(D3DRenderData *a1, CFace *face);
	static void LoadFromFileFlag3();
};