#ifndef CBLOCKSPACE_CLASS_H
#define CBLOCKSPACE_CLASS_H

#include "CBlockContainer.h"
#include "CMatrix.h"

class CBlockSpace : public CBlockContainer
{
public:
	int field_38;
	int field_3C;
	CMatrix m_pMatrix;

	BOOL GetMatrix(CMatrix *out);
};

#endif