#ifndef SCREENELEMENT_CLASS_H
#define SCREENELEMENT_CLASS_H

#include "Node.h"
#include "GraphicsData.h"
#include "Mask.h"

struct MenuMouse
{
	Mask *mask;
	int x;
	int y;
};

class ScreenElement : public Node
{
public:
	GraphicsData *pGraphicsData;
	int bFlags;
	int field_14;
	double drawTime;
	double unkTime;
	double setupTime;
	MenuMouse *cursor2;
	MenuMouse *cursor;

	static ScreenElement *Create(WORD width, WORD height, WORD depth, DWORD flags);

	virtual ~ScreenElement();

	// 
	virtual DWORD GetWidth();
	virtual DWORD GetHeight();
	virtual DWORD GetDepth();
	virtual DWORD SetActive();
	virtual DWORD Preload();
	virtual void InitGraphicsData(DWORD width, DWORD height, DWORD depth, DWORD flags);
	virtual DWORD InitGraphicsRect(RECT rect);
	virtual DWORD DrawSprites(); // ?
	virtual DWORD ProcessEvents(void *controllerData); // ?
	virtual DWORD Draw(void *controllerData);
	virtual DWORD SetupViewport();
	//virtual DWORD Unk1(int a, int b);
};

#endif