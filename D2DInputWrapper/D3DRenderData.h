#ifndef D3DRENDERDATA_CLASS_H
#define D3DRENDERDATA_CLASS_H

#include "GameModule.h"
#include "CVertex.h"
#include "CVector4.h"

#pragma pack(push, 1)
struct D3DRenderDataVmt
{
	int field_0;
	int field_4;
	void(__thiscall *SetupFromGameModule)(class D3DRenderData *, GameModule *);
	int field_C;
	int field_10;
	int field_14;
	int field_18;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct UnkStruct0x80
{
	int field_0;
	CVector field_4;
	CVector field_10;
	float field_1C;
	float field_20;
	float field_24;
	float field_28;
	float someAngle;
	float field_30;
	float field_34;
	float field_38;
	float field_3C;
	float field_40;
	int field_44;
	int field_48;
	int field_4C;
};
#pragma pack(pop)


#pragma pack(push, 1)
class D3DRenderData
{
public:
	D3DRenderDataVmt *vmt;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	char field_40;
	char gap_41[1159];
	CVector field_4C8[22];
	char gap_5D0[268];
	int field_6DC;
	char gap_6E0[168];
	int field_788;
	int field_78C;
	int m_dwTextureFilesCount;
	int m_dwMaterialsCount;
	int m_dwMasksCount;
	GraphicsData *m_pGraphicsData;
	int field_7A0;
	int field_7A4;
	int field_7A8;
	int field_7AC;
	int field_7B0;
	int field_7B4;
	int field_7B8;
	int field_7BC;
	int field_7C0;
	int field_7C4;
	int field_7C8;
	int field_7CC;
	int field_7D0;
	int field_7D4;
	int field_7D8;
	int field_7DC;
	int field_7E0;
	int field_7E4;
	int field_7E8;
	int field_7EC;
	int field_7F0;
	int field_7F4;
	int field_7F8;
	int field_7FC;
	int field_800;
	int field_804;
	int field_808;
	int field_80C;
	int field_810;
	int field_814;
	int field_818;
	int field_81C;
	int field_820;
	int field_824;
	int field_828;
	int field_82C;
	int field_830;
	int field_834;
	int field_838;
	int field_83C;
	int field_840;
	int field_844;
	int field_848;
	int field_84C;
	int field_850;
	int field_854;
	int field_858;
	int field_85C;
	int field_860;
	int field_864;
	int field_868;
	int field_86C;
	int field_870;
	int field_874;
	int field_878;
	int field_87C;
	int field_880;
	int field_884;
	int field_888;
	int field_88C;
	int field_890;
	int field_894;
	int field_898;
	int field_89C;
	int field_8A0;
	int field_8A4;
	int field_8A8;
	int field_8AC;
	int field_8B0;
	int field_8B4;
	int field_8B8;
	int field_8BC;
	CMatrix field_8C0[20];
	CMatrix field_C80[20];
	CMatrix field_1040;
	CMatrix field_1070;
	CMatrix field_10A0[20];
	CMatrix field_1460[20];
	CMatrix field_1820;
	UnkStruct0x80 field_1850[20];
	UnkStruct0x80 field_1E90;
	RECT field_1EE0[20];
	int m_dwCurrentMatrix;
	int field_2024;
	int field_2028;
	int field_202C;
	int field_2030;
	int field_2034;
	int field_2038;
	CVertex *m_pVertices;
	CVector *m_pVertexNormals;
	Material *m_pMaterials;
	Texture **m_pTextureFiles;
	Mask **m_pMask;
	int m_pUseClipping;
	int field_2054;
	int field_2058;
	int field_205C;
	int field_2060;
	int field_2064;
	int field_2068;
	int field_206C;
	int field_2070;
	int field_2074;
	int field_2078;
	int field_207C;
	int field_2080;
	int field_2084;
	int field_2088;
	int field_208C;
	int field_2090;
	int field_2094;
	int field_2098;
	int field_209C;
	int field_20A0;
	int field_20A4;
	int field_20A8;
	int field_20AC;
	int field_20B0;
	int field_20B4;
	int field_20B8;
	int field_20BC;
	int field_20C0;
	int field_20C4;
	int field_20C8;
	int field_20CC;
	int field_20D0;
	int field_20D4;
	int field_20D8;
	int field_20DC;
	int field_20E0;
	int field_20E4;
	int field_20E8;
	int field_20EC;
	int field_20F0;
	int field_20F4;
	BYTE field_20F8;
	BYTE field_20F9;
	BYTE field_20FA;
	BYTE field_20FB;
	int field_20FC;
	int field_2100;
	int field_2104;
	int field_2108;
	int field_210C;
	double field_2110;
	double field_2118;
	double field_2120;
	double field_2128;
	double field_2130;
	double field_2138;
	double field_2140;
	int field_2148;
	float field_214C;
	int field_2150;
	int field_2154;
	float field_2158;
	float field_215C;
	int field_2160;
	int field_2164;
	int field_2168;
	int field_216C;
	int field_2170;
	CMatrix field_2174;
	int field_21A4;
	int field_21A8;
	int field_21AC;
	GameModule *m_pCurrentGameModule;
	int field_21B4;
	Texture *m_pCurrentTexture;
	int field_21BC;
	double field_21C0;
	double field_21C8;
	int bUsingTexture;
	int field_21D4;
	int field_21D8;
	int field_21DC;
	float field_21E0;
	float field_21E4;
	float field_21E8;
	float field_21EC;
	int field_21F0;
	int field_21F4;
	int field_21F8;
	char gap_21FC[11304];
	int field_4E24;
	int field_4E28;
	int field_4E2C;
	int field_4E30;
	int field_4E34;
	int field_4E38;
	int field_4E3C;
	int field_4E40;
	int field_4E44;
	int field_4E48;
	int field_4E4C;
	int field_4E50;
	int field_4E54;
	int field_4E58;
	int field_4E5C;
	int field_4E60;
	int field_4E64;
	int field_4E68;
	int field_4E6C;
	int field_4E70;
	int field_4E74;
	int field_4E78;
	int field_4E7C;
	int field_4E80;
	int field_4E84;
	int field_4E88;
	int field_4E8C;
	int field_4E90;
	int field_4E94;
	int field_4E98;
	int field_4E9C;
	int field_4EA0;
	int field_4EA4;
	int field_4EA8;
	int field_4EAC;
	int field_4EB0;
	int field_4EB4;
	int field_4EB8;
	int field_4EBC;
	int field_4EC0;
	int field_4EC4;
	int field_4EC8;
	int field_4ECC;
	int field_4ED0;
	int field_4ED4;
	int field_4ED8;
	int field_4EDC;
	int field_4EE0;
	int field_4EE4;
	int field_4EE8;
	int field_4EEC;
	int field_4EF0;
	int field_4EF4;
	int field_4EF8;
	int field_4EFC;
	int field_4F00;
	int field_4F04;
	int field_4F08;
	int field_4F0C;
	int field_4F10;
	int field_4F14;
	int field_4F18;
	int field_4F1C;
	int field_4F20;
	int field_4F24;
	int field_4F28;
	int field_4F2C;
	int field_4F30;
	int field_4F34;
	int field_4F38;
	int field_4F3C;
	int field_4F40;
	int field_4F44;
	int field_4F48;
	int field_4F4C;
	int field_4F50;
	int field_4F54;
	int field_4F58;
	int field_4F5C;
	int field_4F60;
	int field_4F64;
	int field_4F68;
	int field_4F6C;
	int field_4F70;
	int field_4F74;
	int field_4F78;
	int field_4F7C;
	int field_4F80;
	int field_4F84;
	int field_4F88;
	int field_4F8C;
	int field_4F90;
	int field_4F94;
	int field_4F98;
	int field_4F9C;
	int field_4FA0;
	int field_4FA4;
	int field_4FA8;
	int field_4FAC;
	int field_4FB0;
	int field_4FB4;
	int field_4FB8;
	int field_4FBC;
	int field_4FC0;
	int field_4FC4;
	int field_4FC8;
	int field_4FCC;
	int field_4FD0;
	int field_4FD4;
	int field_4FD8;
	int field_4FDC;
	int field_4FE0;
	int field_4FE4;
	int field_4FE8;
	int field_4FEC;
	int field_4FF0;
	int field_4FF4;
	int field_4FF8;
	int field_4FFC;
	int field_5000;
	int field_5004;
	int field_5008;
	int field_500C;
	int field_5010;
	int field_5014;
	int field_5018;
	int field_501C;
	int field_5020;
	int field_5024;
	int field_5028;
	int field_502C;
	int field_5030;
	int field_5034;
	int field_5038;
	int field_503C;
	int field_5040;
	int field_5044;
	int field_5048;
	int field_504C;
	int field_5050;
	int field_5054;
	int field_5058;
	int field_505C;
	int field_5060;
	int field_5064;
	int field_5068;
	int field_506C;
	int field_5070;
	int field_5074;
	int field_5078;
	int field_507C;
	int field_5080;
	int field_5084;
	int field_5088;
	int field_508C;
	int field_5090;
	int field_5094;
	int field_5098;
	int field_509C;
	int field_50A0;
	int field_50A4;
	int field_50A8;
	int field_50AC;
	int field_50B0;
	int field_50B4;
	int field_50B8;
	int field_50BC;
	int field_50C0;
	int field_50C4;
	int field_50C8;
	int field_50CC;
	int field_50D0;
	int field_50D4;
	int field_50D8;
	int field_50DC;
	int field_50E0;
	int field_50E4;
	int field_50E8;
	int field_50EC;
	int field_50F0;
	int field_50F4;
	int field_50F8;
	int field_50FC;
	int field_5100;
	int field_5104;
	int field_5108;
	int field_510C;
	int field_5110;
	int field_5114;
	int field_5118;
	int field_511C;
	int field_5120;
	int field_5124;
	int field_5128;
	int field_512C;
	int field_5130;
	int field_5134;
	int field_5138;
	int field_513C;
	int field_5140;
	int field_5144;
	int field_5148;
	int field_514C;
	int field_5150;
	int field_5154;
	int field_5158;
	int field_515C;
	int field_5160;
	int field_5164;
	int field_5168;
	int field_516C;
	int field_5170;
	int field_5174;
	int field_5178;
	int field_517C;
	int field_5180;
	int field_5184;
	int field_5188;
	int field_518C;
	int field_5190;
	int field_5194;
	int field_5198;
	int field_519C;
	int field_51A0;
	int field_51A4;
	int field_51A8;
	int field_51AC;
	int field_51B0;
	int field_51B4;
	int field_51B8;
	int field_51BC;
	int field_51C0;
	int field_51C4;
	int field_51C8;
	int field_51CC;
	int field_51D0;
	int field_51D4;
	int field_51D8;
	int field_51DC;
	int field_51E0;
	int field_51E4;
	int field_51E8;
	int field_51EC;

	void SetRenderMaterial(int matId, int flag1, int flag2);
	void ApplyMaterial(Material *mat, int flag1, int flag2, int flag3);
	int SetupMaterialsWithBlending(Material *mat1, Material *mat2, DWORD blendType);
	bool IsSphereVisible(CVector4 *pos);
	void SetupWorldTransform(CMatrix *mat1, BOOL mult);
};
#pragma pack(pop)


#endif