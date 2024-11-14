#ifndef CSIMPLEBLOCK_CLASS_H
#define CSIMPLEBLOCK_CLASS_H

#include "Node.h"
#include "CVector4.h"
#include "GameModule.h"
//#include "D3DRenderData.h"
extern IDirect3DDevice9 *&g_pDirect3DDevice;

class CSimpleBlock : public Node
{
public:
	char *m_sName;
	int type;
	CVector4 pos;
	GameModule *m_pModule;
	char flags;
	char field_29;
	char field_2A;
	char field_2B;
	class CBlockContainer *m_pParentBlock;

	virtual void AddBlock();
	virtual void InsertBlock();
	virtual void PushBlock();
	virtual void ScanWithFunc();
	virtual void Remove();
	virtual void GetPos();
	virtual void ReadFromFile();
	virtual int Render(int a2, class D3DRenderData *a3);
};

#endif