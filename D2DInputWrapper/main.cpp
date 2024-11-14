#include "ModEngine.h"
#include <iostream>
#include "CPatch.h"
#include "Hooks.h"
#include "CLog.h"
#include "CameraHack.h"
#include "resource.h"
#include <uxtheme.h>
#include <windowsx.h>
#include <locale.h>

HMODULE realDll;
HINSTANCE g_hInstance;
FARPROC DirectInputCreate;
__declspec(naked) void DirectInputCreateFake()			{ _asm { jmp [DirectInputCreate] } }
char GamePath[1024], tmp[1024];
int g_width, g_height;

CLog *logFile = 0;

ModEngine g_pModEngine;

Json::Value g_pMainConfig;
int g_dwSelectedDeviceId = 0;

void ReadConfig(WCHAR *device, int width, int height)
{
	JsonValueSetWString(g_pMainConfig["graphics"]["device"], device);
	g_pMainConfig["graphics"]["width"] = width;
	g_pMainConfig["graphics"]["height"] = height;

	g_pMainConfig = ReadJsonFromFile("mods\\config.json", g_pMainConfig);
}

void SaveConfig()
{
	SaveJsonToFile("mods\\config.json", g_pMainConfig);
}

BOOL CALLBACK StartDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndOwner;
	RECT rcOwner, rcDlg;
	RECT rc;
	DWORD wmId, wmEvent;
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
			EndDialog(hwndDlg, 1);
			break;

		case IDCANCEL:
			EndDialog(hwndDlg, 0);
			break;

		case IDC_BUTTON1:
			if (wmEvent == BN_CLICKED)
			{
				HWND boxInactive = GetDlgItem(hwndDlg, IDC_LIST2);
				HWND boxActive = GetDlgItem(hwndDlg, IDC_LIST1);
				int sel = ListBox_GetCurSel(boxInactive);
				if (sel != -1)
				{
					WCHAR itemName[256];
					ListBox_GetText((HWND)boxInactive, sel, itemName);
					ModItem *mod = g_pModEngine.GetMod(itemName);
					if (mod)
					{
						mod->m_bActive = true;
						ListBox_DeleteString(boxInactive, sel);
						ListBox_AddString(boxActive, itemName);
					}
				}
			}
			break;

		case IDC_BUTTON2:
			if (wmEvent == BN_CLICKED)
			{
				HWND boxInactive = GetDlgItem(hwndDlg, IDC_LIST2);
				HWND boxActive = GetDlgItem(hwndDlg, IDC_LIST1);
				int sel = ListBox_GetCurSel(boxActive);
				if (sel != -1)
				{
					WCHAR itemName[256];
					ListBox_GetText((HWND)boxActive, sel, itemName);
					ModItem *mod = g_pModEngine.GetMod(itemName);
					if (mod)
					{
						mod->m_bActive = false;
						ListBox_DeleteString(boxActive, sel);
						ListBox_AddString(boxInactive, itemName);
					}
				}
			}
			break;

		case IDC_LIST1:
		case IDC_LIST2:
			if (wmEvent == LBN_SELCHANGE)
			{
				int sel = ListBox_GetCurSel((HWND)lParam);
				if (sel != -1)
				{
					WCHAR itemName[256];
					ListBox_GetText((HWND)lParam, sel, itemName);
					ModItem *mod = g_pModEngine.GetMod(itemName);
					if (mod)
					{
						HWND lVer = GetDlgItem(hwndDlg, IDC_STATIC1);
						HWND lAuthor = GetDlgItem(hwndDlg, IDC_STATIC2);
						HWND lDesc = GetDlgItem(hwndDlg, IDC_STATIC3);

						Static_SetText(lVer, std::wstring(L"Версия: " + mod->m_sModVersion).c_str());
						Static_SetText(lAuthor, std::wstring(L"Автор: " + mod->m_sModAuthor).c_str());
						Static_SetText(lDesc, std::wstring(L"Описание: " + mod->m_sModDescription).c_str());
					}

					if (wmId == IDC_LIST1)
						ListBox_SetCurSel(GetDlgItem(hwndDlg, IDC_LIST2), -1);
					else
						ListBox_SetCurSel(GetDlgItem(hwndDlg, IDC_LIST1), -1);
				}
				
			}
			break;

		case IDC_COMBO2:
			if (wmEvent == CBN_SELCHANGE)
			{
				HWND cbDriver = GetDlgItem(hwndDlg, IDC_COMBO2);
				HWND cbMode = GetDlgItem(hwndDlg, IDC_COMBO1);
				int driverId = ComboBox_GetCurSel(cbDriver);
				g_dwSelectedDeviceId = driverId;
				ComboBox_ResetContent(cbMode);
				for (int i = 0; i < g_pDevicesInfo[driverId].pModes.size(); i++)
				{
					WCHAR modeBuf[256];
					wsprintf(modeBuf, L"%dx%d", g_pDevicesInfo[driverId].pModes[i].width, g_pDevicesInfo[driverId].pModes[i].height);
					ComboBox_AddString(cbMode, modeBuf);
				}
				ComboBox_SetCurSel(cbMode, 0);

				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				g_pMainConfig["graphics"]["device"] = converter.to_bytes(g_pDevicesInfo[driverId].deviceNameW);
				g_pMainConfig["graphics"]["width"] = g_pDevicesInfo[driverId].pModes[0].width;
				g_pMainConfig["graphics"]["height"] = g_pDevicesInfo[driverId].pModes[0].height;
			}
			break;

		case IDC_COMBO1:
			if (wmEvent == CBN_SELCHANGE)
			{
				HWND cbDriver = GetDlgItem(hwndDlg, IDC_COMBO2);
				HWND cbMode = GetDlgItem(hwndDlg, IDC_COMBO1);
				int driverId = ComboBox_GetCurSel(cbDriver);
				int modeId = ComboBox_GetCurSel(cbMode);
				g_pMainConfig["graphics"]["width"] = g_pDevicesInfo[driverId].pModes[modeId].width;
				g_pMainConfig["graphics"]["height"] = g_pDevicesInfo[driverId].pModes[modeId].height;
			}
			break;
		}
		break;

	case WM_SHOWWINDOW:

		// Get the owner window and dialog box rectangles. 

		if ((hwndOwner = GetParent(hwndDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}

		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 

		SetWindowPos(hwndDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		Enum3DModes();

		WCHAR *defaultDevice = L"";
		int defaultWidth = 0;
		int defaultHeight = 0;
		if (g_pDevicesInfo.size() > 0)
		{
			defaultDevice = g_pDevicesInfo[0].deviceNameW;
			if (g_pDevicesInfo[0].pModes.size() > 0)
			{
				defaultWidth = g_pDevicesInfo[0].pModes[0].width;
				defaultHeight = g_pDevicesInfo[0].pModes[0].height;
			}
		}
		ReadConfig(defaultDevice, defaultWidth, defaultHeight);

		int deviceId = 0;
		int resId = 0;

		for (int i = 0; i < g_pDevicesInfo.size(); i++)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wdevice = converter.from_bytes(g_pMainConfig["graphics"]["device"].asString());
			if (wcscmp(g_pDevicesInfo[i].deviceNameW, wdevice.c_str()) == 0) 
			{
				deviceId = i;
				for (int j = 0; j < g_pDevicesInfo[i].pModes.size(); j++)
				{
					if (g_pDevicesInfo[i].pModes[j].width == g_pMainConfig["graphics"]["width"].asInt()
						&& g_pDevicesInfo[i].pModes[j].height == g_pMainConfig["graphics"]["height"].asInt())
					{
						resId = j;
						break;
					}
				}
				break;
			}
		}


		for (int i = 0; i < g_pDevicesInfo.size(); i++)
		{
			HWND listHardware = GetDlgItem(hwndDlg, IDC_COMBO2);
			HWND listModes = GetDlgItem(hwndDlg, IDC_COMBO1);
			ComboBox_AddString(listHardware, g_pDevicesInfo[i].deviceDescW);
			if (i == deviceId)
			{
				ComboBox_SetCurSel(listHardware, i);
				for (int j = 0; j < g_pDevicesInfo[i].pModes.size(); j++)
				{
					auto it = &g_pDevicesInfo[i].pModes[j];
					WCHAR modeBuf[256];
					wsprintf(modeBuf, L"%dx%d", it->width, it->height);
					ComboBox_AddString(listModes, modeBuf);
					if (j == resId)
						ComboBox_SetCurSel(listModes, j);
				}
			}
		}

		g_pModEngine.Init(g_pMainConfig["mods"]);

		auto active = g_pModEngine.GetActiveMods();
		auto inactive = g_pModEngine.GetInactiveMods();

		HWND listInactive = GetDlgItem(hwndDlg, IDC_LIST2);
		HWND listActive = GetDlgItem(hwndDlg, IDC_LIST1);

		for (auto it = inactive.begin(); it != inactive.end(); it++)
		{
			ListBox_AddString(listInactive, (*it)->m_sModName.c_str());
		}
		for (auto it = active.begin(); it != active.end(); it++)
		{
			ListBox_AddString(listActive, (*it)->m_sModName.c_str());
		}
		return TRUE;
	}
	return FALSE;
}

int CALLBACK WinMainProc(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	setlocale(LC_ALL, "Russian");
	logFile = CLog::Open("dinput.log");

	int result = DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_DIALOG1), GetDesktopWindow(), StartDialogProc);
	g_pMainConfig["mods"] = g_pModEngine.GetConfig();
	SaveConfig();

	if (!result)
		return 0;
	
	GetCurrentDirectoryA(1024, GamePath);
	sprintf(tmp, "%s\\patch.ini", GamePath);

	//g_width = GetPrivateProfileIntA("RESOLUTION", "Width", 0, tmp);
	//g_height = GetPrivateProfileIntA("RESOLUTION", "Height", 0, tmp);

	g_width = g_pMainConfig["graphics"]["width"].asInt();
	g_height = g_pMainConfig["graphics"]["height"].asInt();

	if (g_width > 0 && g_height > 0)
	{
		CPatch::Nop(0x512EE2, 2);
		CPatch::SetInt(0x512EFC, g_width); // width
		CPatch::SetInt(0x512EF7, g_height); // height
		CPatch::RedirectCall(0x510E62, &SetupSidePanelOffsets);
		CPatch::RedirectCall(0x523133, &SetupSidePanelOffsets);
		CPatch::RedirectCall(0x528EBA, &SetupSidePanelOffsets);
		//CPatch::SetInt(0x512ED3, 4); // depth
		//logFile->WriteLine("Set resolution %dx%d", width, height);

		// cab chan
		float float1 = (float)g_width / (float)g_height * 1.15f;
		float float2 = 1.15f;
		CPatch::SetInt(0x4DE3CA, *(DWORD*)&float1);
		CPatch::SetInt(0x4DE3D7, *(DWORD*)&float2);
	}



	// User Calls
	// Init WSA
	//CPatch::RedirectJump(0x4EA90D, &InitWSA);

	// SendMessageToAllPlayers
	/*CPatch::RedirectJump(0x602180, &SendMessageToAllPlayers);
	CPatch::RedirectJump(0x6326D3, &dNew);
	CPatch::RedirectJump(0x61D7D7, &dMalloc);*/
	//CPatch::RedirectJump(0x512510, &loadGameFunc);

	//CPatch::SetPointer(0x64D720, &loadGameFunc);
	//CPatch::RedirectJump(0x401F30, &dAllocPlayersPool);

	//CPatch::RedirectCall(0x5D254B, &HeapAlocInitByName);

	//CPatch::RedirectJump(0x515930, &TestGraphicsAccelerator);
	CPatch::RedirectJump(0x5D7B70, &InitDirectDraw);
	CPatch::RedirectCall(0x5E3B9D, &InitDirectDrawForMenu);
	//CPatch::SetPointer(0x5D75AE, &DDrawInit);
	//CPatch::SetPointer(0x5D7C64, &DDrawInit);
	//CPatch::SetPointer(0x5D7E80, &DDrawInit);


	//CPatch::RedirectJump(0x512EE2, (void*)0x512F79);//CPatch::RedirectJump(0x5DD850, &AdjustWin);
	CPatch::RedirectJump(0x5DD850, &AdjustWin);
	//CPatch::RedirectJump(0x5D7B70, &HookInitDirectDraw);

	// test ddraw reinit

	// убираем все вызовы кроме одного
	//CPatch::RedirectJump(0x5D7500, ReInitDirectDraw);
	CPatch::SetChar(0x5D7500, 0xC3);
	CPatch::Nop(0x5E3D68, 5);

	CPatch::SetInt(0x675D40, 0); // remove demo

	// set default depth 4 (32bit)
	CPatch::SetChar(0x5d80fe, 3);

	// remove testGraphicsAccelerator
	CPatch::RedirectJump(0x515C6C, (void*)0x515D36);

	// remove CWnd::SetWindowPos
	CPatch::RedirectJump(0x6337CA, (void*)0x633816);

	// remove PalettesStuff
	CPatch::SetChar(0x5BA360, 0xC3);

	// remove demo show
	CPatch::RedirectJump(0x5289E4, (void*)0x528B46);

	// patch 8bit texture loading
	CPatch::SetChar(0x5E8B72, 0x18);
	CPatch::SetChar(0x5E8B8E, 0x1A);

	// enum texture format nop
	CPatch::SetChar(0x5D83B0, 0xC3);

	// BeginScene - EndScene
	CPatch::SetChar(0x5B6110, 0xC3);
	CPatch::SetChar(0x5B6120, 0xC3);

	// remove flip surface
	//CPatch::SetChar(0x5B6AA0, 0xC3);

	// -----------------

	CPatch::RedirectJump(0x5D2DE0, LogError);
	//CPatch::RedirectCall(0x5EB612, SetDefaultVectors);

	CPatch::Nop(0x5F8ED2, 2); // disable LODS

	CPatch::SetInt(0x512ED3, 4);
	CPatch::SetInt(0x5D3A36, 3);
	//CPatch::RedirectJump(0x5BB070, (void*)0x5BAE25);
	CPatch::RedirectJump(0x5BAE25, TextureInitSurfaceDesc1);
	CPatch::RedirectJump(0x5BB070, TextureInitSurfaceDesc2);
	CPatch::Nop(0x5BB0AC, 0xB);
	CPatch::Nop(0x5BB0C1, 12);
	CPatch::RedirectJump(0x5BC02C, MaskInitSurfaceDesc);
	CPatch::Nop(0x5BC042, 4);
	CPatch::Nop(0x5BC058, 0x13);
	CPatch::RedirectJump(0x5E9146, FontInitSurfaceDesc);//CPatch::RedirectJump(0x5E9153, FontInitSurfaceDesc);
	CPatch::Nop(0x5E9185, 0x17);
	CPatch::Nop(0x5E9172, 4);

	CPatch::SetChar(0x5130CB, 4);
	CPatch::SetChar(0x50C0EF, 4); // patch texture init with params
	CPatch::RedirectJump(0x50C11E, CreateTextureFromScreenCopy);
	// patch mask test
	CPatch::SetChar(0x5BC081, 0xEB);
	// patch texture header load
	CPatch::RedirectCall(0x5E7DB0, TextureReadHeaderReplaceDepth);
	CPatch::RedirectCall(0x5E7973, TextureReadHeaderReplaceDepthDefault);//CPatch::RedirectJump(0x5E7973, (void*)0x5E7992);
	//patch texture init
	CPatch::RedirectCall(0x5BB618, TextureInitReplaceDepth);
	//patch font load
	//CPatch::SetInt(0x5E9146, 0x1424748B);
	//CPatch::RedirectCall(0x5E914A, FontReadReplaceDepth);

	CPatch::RedirectJump(0x5D82B0, EnumTextureFormatsCallback);
	//CPatch::Nop(0x5B828D, 2);
	/*
	CPatch::RedirectJump(0x528A2D, (void*)0x528AD8); // AdjustWin1
	CPatch::SetInt(0x528AD9, 0x3A00);

	CPatch::RedirectJump(0x528CF2, (void*)0x528D47); // AdjustWin2
	CPatch::SetInt(0x528D48, 0xA00);
	CPatch::SetChar(0x528D4D, 2);

	CPatch::SetChar(0x50BE10, 0xA);
	*/

	// winappinit
	CPatch::SetPointer(0x64D880, &WinAppInit);
	CPatch::SetPointer(0x6519AC, &WinAppInit);
	CPatch::SetPointer(0x6526B4, &WinAppInit);

	// unk thegame func
	CPatch::SetPointer(0x64EB88, &unkFuncHook);

	// patch main menu screen
	/*CPatch::SetInt(0x528D4F, 768);
	CPatch::SetInt(0x528D54, 1366);
	CPatch::RedirectJump(0x528CF2, (void*)0x528D47);
	CPatch::Nop(0x528A2D, 0x21);
	*/
	//CPatch::SetInt(0x528D48, 0x3E00);
	//CPatch::SetChar(0x528D4D, 2);
	//CPatch::RedirectJump(0x528A2D, (void*)0x528D47);


	// redirect first direct draw init
	//CPatch::RedirectJump(0x5E3B9D, InitDirectDrawForMenu);


	CPatch::SetInt(0x571F47, 0x10000000); // available memory
	
	CPatch::RedirectJump(0x5E295F, (void*)0x5E2A8D);

	//CPatch::SetPointer(0x64D8D4, CWinAppDrawFrame);

	// fillbycolor
	CPatch::SetChar(0x5D4290, 0xC3);
	CPatch::SetChar(0x5B6AA0, 0xC3);
	CPatch::RedirectJump(0x5D8580, (void*)0x5D8599);
	
	SetupGameHooks();

	//CPatch::Nop(0x5131A4, 7);
	//CPatch::Nop(0x512E14, 7);
	

	CameraHack::SetupHooks();

	return ((int (__stdcall *)(HINSTANCE, HINSTANCE, LPSTR, int))0x63186A)(hInst, hPrevInst, lpCmdLine, nShowCmd);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	char path[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		GetSystemDirectoryA(path, MAX_PATH - 10);
		strcat(path, "\\dinput.dll");
		realDll = LoadLibraryA(path);
		if (realDll == false)
		{
			MessageBoxA(0, "Cannot load system dinput.dll!", "D2 Plugin Loader", MB_ICONERROR);
			ExitProcess(0);
		}

		DirectInputCreate = GetProcAddress(realDll, "DirectInputCreateA");
		g_hInstance = hModule;
		CPatch::RedirectCall(0x61FF25, WinMainProc);
		break;
	}

	case DLL_PROCESS_DETACH:
		FreeLibrary(realDll);
	break;

	}
	return TRUE;
}