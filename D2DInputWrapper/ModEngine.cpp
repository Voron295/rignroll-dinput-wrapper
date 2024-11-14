#include "ModEngine.h"
#include "FileManager.h"

#ifdef _DEBUG
#pragma comment(lib, "jsoncppMTD.lib")
#else
#pragma comment(lib, "jsoncpp.lib")
#endif

ModItem::ModItem()
{
	m_bActive = false;
	m_bLoaded = false;
	m_eType = ModType::ModNotLoaded;
	m_hModule = 0;
	m_pInstallMod = 0;
	m_pUninstallMod = 0;
}

ModItem::~ModItem()
{
	if (m_hModule)
	{
		FreeLibrary(m_hModule);
		m_hModule = 0;
	}
}

bool ModItem::Preload(ModType type, std::string path)
{
	m_sModPath = path;
	m_eType = type;
	if (m_eType == ModType::ModDll)
	{
		m_hModule = LoadLibraryExA(path.c_str(), 0, LOAD_IGNORE_CODE_AUTHZ_LEVEL);
		if (!m_hModule)
			return false;

		void(__stdcall *getModInfo)(WCHAR **, WCHAR **, WCHAR **, WCHAR **) = (void(__stdcall *)(WCHAR **, WCHAR **, WCHAR **, WCHAR **))GetProcAddress(m_hModule, "GetModInfo");
		if (!getModInfo)
			return false;

		WCHAR *name = 0,
			*version = 0,
			*author = 0,
			*description = 0;
		getModInfo(&name, &version, &author, &description);
		if (!name)
			return false;

		m_sModName = name;
		if (version)
			m_sModVersion = version;
		if (author)
			m_sModAuthor = author;
		if (description)
			m_sModDescription = description;

		m_pInstallMod = (void(__stdcall *)())GetProcAddress(m_hModule, "InstallMod");
		m_pUninstallMod = (void(__stdcall *)())GetProcAddress(m_hModule, "UninstallMod");
	}
	else
	{
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//m_sModName = converter.from_bytes(FileManager::GetFileNameFromPath(path.c_str()));
		m_hConfig = ReadJsonFromFile(path + "\\config.json", Json::Value());
		
		// parse config
		if (!m_hConfig["name"].isString())
			return false;

		m_sModName = JsonValueGetWString(m_hConfig["name"]);
		m_sModVersion = JsonValueGetWString(m_hConfig["version"]);
		m_sModAuthor = JsonValueGetWString(m_hConfig["author"]);
		m_sModDescription = JsonValueGetWString(m_hConfig["description"]);
	}
	return true;
}

ModEngine::ModEngine()
{

}

ModEngine::~ModEngine()
{
	for (auto it = m_pMods.begin(); it != m_pMods.end(); it++)
	{
		delete it->second;
	}
	m_pMods.clear();
}

void ModEngine::Init(Json::Value config)
{
	char GamePath[1024];
	GetCurrentDirectoryA(1024, GamePath);
	m_sGamePath = GamePath;
	m_sGamePath += "\\";
	m_sModsPath = m_sGamePath + "mods\\";

	bool modFolder = false;
	DWORD attr = GetFileAttributesA(m_sModsPath.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		if (CreateDirectoryA(m_sModsPath.c_str(), 0)) 
		{
			modFolder = true;
		}
	}
	else if (attr & FILE_ATTRIBUTE_DIRECTORY)
	{
		modFolder = true;
	}
	if (!modFolder)
		return;

	WIN32_FIND_DATAA ffd;
	memset(&ffd, 0, sizeof(ffd));
	HANDLE handle = FindFirstFileA((m_sModsPath + "*").c_str(), &ffd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			ModItem *mod = 0;
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					mod = new ModItem();
					if (!mod->Preload(ModType::ModFolder, m_sModsPath + ffd.cFileName))
					{
						delete mod;
						mod = 0;
					}
				}
			}
			else
			{
				const char *ext = FileManager::GetFileExtFromPath(ffd.cFileName);
				if (strcmp(ext, "dll") == 0)
				{
					mod = new ModItem();
					if (!mod->Preload(ModType::ModDll, m_sModsPath + ffd.cFileName))
					{
						delete mod;
						mod = 0;
					}
				}
			}
			if (mod)
			{
				m_pMods.emplace(mod->m_sModName, mod);
			}
		} while (FindNextFileA(handle, &ffd));
		FindClose(handle);
	}

	m_pConfig = config;

	Json::Value activeMods;

	for (auto it = m_pConfig["active"].begin(); it != m_pConfig["active"].end(); it++)
	{
		std::string name = it->asString();

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wname = converter.from_bytes(name);
		auto found = m_pMods.find(wname);
		if (found != m_pMods.end())
		{
			found->second->m_bActive = true;
			activeMods.append(name);
		}
	}
	m_pConfig["active"] = activeMods;
}

std::vector<ModItem *> ModEngine::GetActiveMods()
{
	std::vector<ModItem *> result;
	for (auto it = m_pMods.begin(); it != m_pMods.end(); it++)
	{
		if (it->second->m_bActive)
			result.push_back(it->second);
	}
	return result;
}

std::vector<ModItem *> ModEngine::GetInactiveMods()
{
	std::vector<ModItem *> result;
	for (auto it = m_pMods.begin(); it != m_pMods.end(); it++)
	{
		if (!it->second->m_bActive)
			result.push_back(it->second);
	}
	return result;
}

std::map<std::wstring, ModItem *> ModEngine::GetMods()
{
	return m_pMods;
}

ModItem *ModEngine::GetMod(std::wstring name)
{
	auto item = m_pMods.find(name);
	if (item != m_pMods.end())
	{
		return item->second;
	}
	return 0;
}

Json::Value ModEngine::GetConfig()
{
	Json::Value activeMods;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	for (auto it = m_pMods.begin(); it != m_pMods.end(); it++)
	{
		if (it->second->m_bActive)
		{
			activeMods.append(converter.to_bytes(it->second->m_sModName));
		}
	}
	m_pConfig["active"] = activeMods;
	return m_pConfig;
}