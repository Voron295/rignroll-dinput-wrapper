#include "CBlockContainer.h"

void CBlockContainer::RenderChild(int id, int flags, D3DRenderData *a3)
{
	if (id < m_dwSubBlocksCount && id >= 0)
	{
		auto v4 = this->m_pSubBlocks[id];
		if (v4)
		{
			if (flags & 1)
			{
				for (CSimpleBlock *i = (CSimpleBlock *)v4->m_pNext; i; i = (CSimpleBlock *)i->m_pNext)
					v4 = i;
				if (v4)
				{
					do
					{
						//logFile->WriteLine("Render - 0x%X", *(DWORD*)v4);
						v4->Render(flags, a3);
						v4 = (CSimpleBlock*)v4->m_pPrev;
					} while (v4);
				}
			}
			else if (v4)
			{
				do
				{
					//logFile->WriteLine("Render2 - 0x%X", *(DWORD*)v4);
					v4->Render(flags, a3);
					v4 = (CSimpleBlock*)v4->m_pNext;
				} while (v4);
			}
		}
	}
}