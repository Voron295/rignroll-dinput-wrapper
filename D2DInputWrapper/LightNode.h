#include <d3d9.h>
#include "D3DRenderData.h"

#pragma pack(push, 1)
struct LightNode
{
	D3DLIGHT9 m_vLight;
	UnkStruct0x80 *field_68;
	float field_6C;
	int field_70;
	int m_dwLightIndex;
};
#pragma pack(pop)
