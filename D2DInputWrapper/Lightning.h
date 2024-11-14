#include "LightNode.h"

#pragma pack(push, 1)
class Lightning
{
public:
	int field_0;
	int m_dwLightningSubCount;
	int field_8;
	LightNode *m_pLightNodes;

	static void InitHooks();
};
#pragma pack(pop)
