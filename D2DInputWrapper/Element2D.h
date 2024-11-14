#ifndef ELEMENT2D_CLASS_H
#define ELEMENT2D_CLASS_H

#include "ScreenElement.h"
#include "SpriteContainer.h"

class Element2D : public ScreenElement
{
public:
	SpriteContainer *sprite;
	int field_3C;

	virtual ~Element2D();
};

#endif