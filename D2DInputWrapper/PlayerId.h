#include "AI_Player.h"

class PlayerId
{
public:
	PlayerId *m_pNext;
	PlayerId *m_pLast;
	AI_Player m_pPlayerData;

	static PlayerId *&g_pPlayersPool;
};

static_assert (sizeof(PlayerId) == 0x3E8, "Size is not correct");