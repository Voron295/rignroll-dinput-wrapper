#ifndef SPRITECONTAINER_CLASS_H
#define SPRITECONTAINER_CLASS_H

#include "Sprite.h"

class SpriteContainer : public Sprite
{
public:
	Sprite *m_pChildren;
	Sprite *activeSprite;

	virtual ~SpriteContainer();
};

#endif