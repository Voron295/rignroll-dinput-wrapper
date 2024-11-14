#include <Windows.h>
#include "D3DRenderData.h"

#pragma pack(push, 1)
struct IniParamsUnk
{
	int vmt;
	int field_4;
	int field_8;
	int field_C;
	double field_10;
	double field_18;
	double field_20;
	double field_28;
	double field_30;
	double field_38;
	double field_40;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	float field_6C;
	int field_70;
	int field_74;
	int field_78;
	float field_7C;
	double field_80;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AnmObj
{
	DWORD field_0;
	int id;
	char flags;
	char field_9;
	char field_A;
	char field_B;
	struct TheGameMod *m_pTheGameMod;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TheGameMod
{
	int vmt;
	int m_pPrev;
	TheGameMod *m_pNext;
	AnmObj *field_C[1000];
	int m_dwFlags;
	char *m_sModuleName;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct TheGameModuleLoader
{
	AnmObj *field_0[400];
	TheGameMod *m_pModules[100];
	int field_7D0;
	int(__cdecl *m_pLoadingModuleCallback)(AnmObj *, DWORD);
	int(__cdecl *m_pDestroyModuleCallback)(DWORD, DWORD);
};
#pragma pack(pop)



#pragma pack(push, 1)
struct ScreenElementMovieData
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	HANDLE m_pFile;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	int field_74;
	int field_78;
	int field_7C;
	int field_80;
	int field_84;
	int field_88;
	int field_8C;
	int field_90;
	int field_94;
	int field_98;
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int field_AC;
	int field_B0;
	int field_B4;
	int field_B8;
	int field_BC;
	int field_C0;
	int field_C4;
	int field_C8;
	int field_CC;
	int field_D0;
	int field_D4;
	int field_D8;
	int field_DC;
	int field_E0;
	int field_E4;
	int field_E8;
	int field_EC;
	int field_F0;
	int field_F4;
	int field_F8;
	int field_FC;
	int field_100;
	int field_104;
	int field_108;
	int field_10C;
	int field_110;
	int field_114;
	int field_118;
	int field_11C;
	int field_120;
	int field_124;
	int field_128;
	int field_12C;
	int field_130;
	int field_134;
	int field_138;
	int field_13C;
	int field_140;
	int field_144;
	int field_148;
	int field_14C;
	int field_150;
	int field_154;
	int field_158;
	int field_15C;
	int field_160;
	int field_164;
	int field_168;
	int field_16C;
	int field_170;
	int field_174;
	int field_178;
	int field_17C;
	int field_180;
	int field_184;
	int field_188;
	int field_18C;
	int field_190;
	int field_194;
	D3DRenderData *m_pD3DRenderData;
	GameModule *m_pGameModule;
	IniParamsUnk *ini_params;
	class CBlockSpace *SkyDomeSpace;
	class CBlockContainer *SkyDomeKey;
	int field_1AC;
	class CBlockObserver *m_pObserverBlock;
	int field_1B4;
	double field_1B8;
	double field_1C0;
	double field_1C8;
	int field_1D0;
	class CBlockContainer *Foreground_object;
	class CSimpleBlock *Background_object;
	class CBlockAnmObj *Viewer_object;
	int DrawLoadingBarCb;
	int field_1E4;
	int field_1E8;
	int field_1EC;
	int field_1F0;
	int field_1F4;
	double field_1F8;
	int field_200;
	int field_204;
	int field_208;
	CVector field_20C;
	double field_218;
	double field_220;
	int field_228;
	int field_22C;
	CBlockSpace *field_230;
	int field_234;
	double field_238;
	int field_240;
	int field_244;
	char *m_sConfigFileName;
	char *home_install;
	char *base_install;
	CMatrix field_254;
	class CBlockRoom *field_284;
	TheGameModuleLoader m_pModuleLoader;
};
#pragma pack(pop)

template<size_t S> class Sizer { }; Sizer<sizeof(TheGameModuleLoader)> foo;

static_assert (sizeof(ScreenElementMovieData) == 0xA64, "Size is not correct");