#ifndef GAMEMODULE_CLASS_H
#define GAMEMODULE_CLASS_H

#include <Windows.h>
#include <iostream>
#include "Material.h"
#include "Sound.h"
#include "Ovl.h"
//#include "CBlockRoom.h"
//#include "CBlockSpace.h"
#include "CMatrix.h"

class GameModule
{
public:
	GameModule *m_pPrev;
	GameModule *m_pNext;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int m_dwPaletteTexturesCount;
	FILE *m_pFile;
	LPCVOID m_pMappedFile;
	HANDLE field_2C;
	HANDLE field_30;
	HMMIO *field_34;
	void(*m_pTextureFilesLoadCallback)(void);
	char *moduleName;
	class CBlockRoom *m_pRoomBlocks;
	class CBlockSpace *field_44;
	int m_iMaterialCount;
	Material *m_pMaterials;
	int m_iTextureFilesCount;
	Texture **m_pTextureFiles;
	int m_iBackFileCount;
	Texture **m_pBackFiles;
	int m_iPaletteFileCount;
	GraphicsPalette **m_pPaletteTextures;
	int m_iMaskCount;
	Mask **m_pMasks;
	int m_iSoundCount;
	Sound **m_pSounds;
	class CSimpleBlock *rooms;
	char **m_pEffectsNames;
	char **m_pEffects2;
	int m_iEffectCount;
	Ovl *ovlFile;
	int mainPaletteId;
	int field_90;
	int field_94;
	int fileSystemFlags;
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int field_AC;
	int field_B0;

	virtual ~GameModule();
};

#endif