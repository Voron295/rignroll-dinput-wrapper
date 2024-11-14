#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include "Mask.h"
#include "Texture.h"

// Material flags
#define MATERIAL_HAS_TEXTURE 1
#define MATERIAL_HAS_TTX 2
#define MATERIAL_MOVE 4
#define MATERIAL_HAS_ROTATION 8
#define MATERIAL_HAS_MASK 0x10
#define MATERIAL_IS_TRANSPARENT 0x20
#define MATERIAL_HAS_ITX 0x40
#define MATERIAL_NOZ 0x80
#define MATERIAL_NOF 0x100
#define MATERIAL_NOTILE 0x200
#define MATERIAL_ATT 0x400
#define MATERIAL_ENV 0x800
#define MATERIAL_USE_COL 0x1000
#define MATERIAL_HAS_REFLECTION 0x2000
#define MATERIAL_HAS_ALPHAMIR 0x4000
#define MATERIAL_NOTILEU 0x8000
#define MATERIAL_NOTILEV 0x10000
#define MATERIAL_HAS_WAVE 0x100000
#define MATERIAL_HAS_BUMP 0x80000

class Material
{
public:
	char name[32];
	int flags;
	int m_dwPower;
	int m_dwColorId;
	int m_dwInternalId;
	int m_dwInternalTexCoordsId;
	int m_dwParentId_;
	int m_dwTextureId;
	int m_dwMaskId;
	int m_dwEnvId;
	int someColor_;
	Mask *m_pMask;
	Texture *m_pTexture;
	float m_fMoveX;
	float m_fMoveY;
	float m_fTransparency;
	float m_fSpecular;
	float env1;
	float env2;
	float m_fReflection;
	float m_fRotation;
	float m_fRotationPointX;
	float m_fRotationPointY;
	int field_78;
};

#endif