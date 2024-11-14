#include <Windows.h>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include "helper.h"

enum ModType
{
	ModNotLoaded,
	ModDll,
	ModFolder
};

class ModItem
{
public:
	ModItem();
	~ModItem();

	bool ShowConfig(HWND hParentWnd);
	bool Preload(ModType type, std::string path);
	void Load();

	std::wstring m_sModName;
	std::wstring m_sModVersion;
	std::wstring m_sModAuthor;
	std::wstring m_sModDescription;

	bool m_bActive;

private:
	ModType m_eType;
	bool m_bLoaded;
	HMODULE m_hModule;
	void(__stdcall *m_pInstallMod)();
	void(__stdcall *m_pUninstallMod)();
	Json::Value m_hConfig;
	std::string m_sModPath;
};

class ModEngine
{
private:
	std::map<std::wstring, ModItem *> m_pMods;
	std::string m_sGamePath;
	std::string m_sModsPath;
	Json::Value m_pConfig;

public:
	ModEngine();
	~ModEngine();

	void Init(Json::Value config);
	std::vector<ModItem *> GetActiveMods();
	std::vector<ModItem *> GetInactiveMods();
	std::map<std::wstring, ModItem *> GetMods();
	ModItem *GetMod(std::wstring name);
	Json::Value GetConfig();
};
