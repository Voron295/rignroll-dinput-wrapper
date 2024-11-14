#ifndef CVERTEX_CLASS_H
#define CVERTEX_CLASS_H

#pragma pack(push, 1)
struct UV
{
	float u;
	float v;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct CVertex
{
	CVector pos;
	float u;
	float v;
	UV *m_pUVs;
	char field_18;
	BYTE field_19[3];
};
#pragma pack(pop)

#endif