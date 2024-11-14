#ifndef CMENUINGAMEPAUSE_CLASS_H
#define CMENUINGAMEPAUSE_CLASS_H

#include "CMenu.h"

class CMenuInGamePause : public CMenu
{
public:
	int field_168;
	int field_16C;
	class TheGame *m_pTheGame;
	int field_174;

	virtual ~CMenuInGamePause();
};

#endif