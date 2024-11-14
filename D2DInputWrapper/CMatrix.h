#ifndef CMATRIX_CLASS_H
#define CMATRIX_CLASS_H

#include "CVector.h"

class CMatrix
{
public:
	union 
	{
		struct
		{
			CVector right;
			CVector top;
			CVector at;
			CVector pos;
		};
		CVector m[4];
	};

	CMatrix();
};

#endif