#include <windows.h>
#include "Font.h"
#include "Element2D.h"
#include "TheGame.h"
#include "ControllerData.h"
struct CWinApp;

#pragma pack(push, 1)
struct CWinAppVmt
{
	int GetClassName;
	int Destroy;
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
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int InitWindow;
	int Run;
	int PreTranslateMessage;
	int(__thiscall *PumpMessage)(CWinApp *);
	int(__thiscall *OnIdle)(CWinApp *, int);
	int(__thiscall *IsIdleMessage)(CWinApp *, int *);
	int(__thiscall *ExitInstance)(CWinApp *);
	int ProcessWndProcException;
	int ProcessMessageFilter;
	int GetMainWnd;
	int field_78;
	int GetNextDocTemplate;
	int field_80;
	int InitApplication;
	int field_88;
	int field_8C;
	int field_90;
	int field_94;
	int field_98;
	void(__thiscall *StartGame)(CWinApp *, signed int, char **);
	void(__thiscall *field_A0)(DWORD);
	void(*DrawFrame)(void);
	void(*ProcessFrame)(void);
	void(__thiscall *ProcessGamepad)(CWinApp *);
	void(__thiscall *Draw2D)(CWinApp *);
	void(__thiscall *field_B4)(CWinApp *);
	int field_B8;
	void(__thiscall *field_BC)(CWinApp *, DWORD);
	void(__thiscall *field_C0)(CWinApp *, ControllerData *);
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CWinThread
{
	CWinAppVmt *_vmt;
	int m_pMainWnd;
	int m_pActiveWnd;
	int m_bAutoDelete;
	int m_hThread;
	int m_nThreadID;
	int m_pThreadParams;
	int m_pfnThreadProc;
};
#pragma pack(pop)

enum eGameDrawFlags
{
	USE_FULLSCREEN = 0x4,
	USE_CLIPPER = 0x10,
	CREATED_FRONT_SURFACE = 0x20,
};

#pragma pack(push, 1)
struct __declspec(align(4)) CWinApp
{
	CWinThread _base;
	int m_hInstance;
	int m_lpCmdLine;
	int m_nCmdShow;
	int m_pszAppName;
	int m_pszAppID;
	int m_pszRegistryKey;
	int m_pDocManager;
	int m_bHelpMode;
	int m_pszExeName;
	int m_pszHelpFilePath;
	int m_pszProfileName;
	int m_hDevMode;
	int m_hDevNames;
	int m_dwPromptContext;
	int m_hLangResourceDLL;
	int m_nWaitCursorCount;
	int m_hcurWaitCursorRestore;
	int m_pRecentFileList;
	HINSTANCE m_pHinstance;
	HINSTANCE m_pPrevHinstance;
	char *m_sCmdLine;
	DWORD m_dwShowCmd;
	char *field_78;
	int field_7C;
	int field_80;
	int field_84;
	char *field_88;
	char *hlpFilePath;
	char *field_90;
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
	Element2D *field_C8;
	int field_CC;
	int field_D0;
	int field_D4;
	int field_D8;
	int field_DC;
	int field_E0;
	int field_E4;
	int field_E8;
	int field_EC;
	ScreenElement *m_pSprites;
	ScreenElement *m_pTasks;
	ScreenElement *m_pGameStruct;
	ScreenElement *pCurrentScreenElement;
	int width;
	int height;
	int depth;
	eGameDrawFlags _flags;
	Font *m_pMenuFont;
	int m_sGamePath;
	LPCSTR m_sConfigFileName;
	int width__;
	int height__;
	int field_124;
	int field_128;
	int field_12C;
	int mousePosX;
	int mousePosY;
	int field_138;
	int m_dUseGamepad;
	int field_140;
	TheGame *field_144;
	int field_148;
	int bGamePause;
	int field_150;
	int m_bNeedToDestroyWnd;
	int m_bNeedToInitDirectSound;
	int field_15C;
	int field_160;
	int field_164;
	double field_168;
	double field_170;
	double timeDelta;
	int field_180;
	HWND window;
	int field_188;
	int field_18C;
	int field_190;
	int field_194;
	int currentPlayerMP;
	int field_19C;
	int m_bNetIsOn;
	int field_1A4;
	int field_1A8;
	int field_1AC;
	int field_1B0;
	int field_1B4;
	DWORD playersMpCount;
	int(__cdecl *TryToAddPlayerMp)(const char *);
	int field_1C0;
};
#pragma pack(pop)