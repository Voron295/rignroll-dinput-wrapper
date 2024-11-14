#include <Windows.h>
#include <windowsx.h>
#include "GraphicsMod.h"
#include "CPatch.h"
#include "CLog.h"
#include "Font.h"
#include "GraphicsData.h"
#include "CWinApp.h"
#include "GameHookedFuncs.h"
#include "ControllerData.h"
#include "Material.h"
#include "Lightning.h"
#include "CBlockContainer0x5.h"
#include "Blocks3D.h"
#include <string>
#include "PlayerId.h"

#pragma comment(lib, "d3d9.lib")	// Ссылка  на d3d9.lib
#pragma comment(lib, "d3dx9.lib")	// Ссылка  на d3dx9.lib
#pragma comment(lib, "winmm.lib")	// Ссылка  на winmm.lib

extern CLog *logFile;

IDirect3D9 *&g_pDirect3D = *((IDirect3D9 **)0x720924);
IDirect3DDevice9 *&g_pDirect3DDevice = *((IDirect3DDevice9 **)0x720928);
IDirect3DDevice9 *g_pD3DDevice = 0;
int &g_iWindowWidth = *(int*)0x7208BC;
int &g_iWindowHeight = *(int*)0x7208C0;
HWND &g_hWnd = *(HWND*)0x720940;
int &gDepth = *(int*)0x7208B0;
int &gDepthTextures = *(int*)0x7208B4;
DWORD &gRenderFlags = *(DWORD*)0x702EAC;

bool g_bIsFullscreenMode = false;

CWinApp *&g_pWinApp = *(CWinApp **)0x721770;
CWinApp &g_WinApp = *(CWinApp*)0x696770;

TheGame *&g_pTheGame = *(TheGame**)0x696CC0;


DWORD &g_bCanBlendMaterials = *(DWORD*)0x7208DC;
DWORD &g_bCanBlendCurrentAlpha = *(DWORD*)0x7208F4;
DWORD &g_bCanSelectArg1 = *(DWORD*)0x7208E8;
DWORD &g_bCanSelectArg2 = *(DWORD*)0x7208F0;
DWORD &g_bCanBlendFactorAlpha = *(DWORD*)0x7208EC;
DWORD &g_dwPrevMaterialHadAlpha = *(DWORD*)0x7208E4;
DWORD &g_bCanModulate2X = *(DWORD*)0x7208E0;

Material *&g_pPrevMaterialApplied = *(Material**)0x702E8C;
int &g_pPrevMaterialAppliedFlag = *(int*)0x67EF8C;
int g_dwPrevMaterialUsedTransparency = *(int *)0x67EF90;
DWORD &g_dwPrevMaterialTexId = *(DWORD*)0x702EA4;
bool g_bGameInitialized = false;


HINSTANCE g_hInstance = 0;
LPD3DXFONT g_pDebugFont = 0;

void InitWindowsFont()
{
	logFile->WriteLine("Trying to init font");

	HRESULT r = 0;
	// init debug font
	r = D3DXCreateFont(g_pDirect3DDevice, 16,
		0,
		0,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Tahoma",
		&g_pDebugFont);

	if (FAILED(r))
		return;

	logFile->WriteLine("Initialized font successfully");
}

void DrawDebugText(int x, int y, DWORD color, const char *str)
{
	if (!g_pDebugFont)
		return;
	logFile->WriteLine("Trying to draw debug text: %s", str);
	RECT TextRect = { x, y, 0, 0 };
	g_pDebugFont->DrawText(NULL, str, -1, &TextRect, DT_CALCRECT, color);
	g_pDebugFont->DrawText(NULL, str, -1, &TextRect, DT_LEFT, color);
}

int TextDrawHook(GraphicsData *surface, int x, int y, int propLen, Font *font, const char *text, unsigned int color, int isRealColor);

int DrawLineHook(GraphicsData *a1, int x1, int y1, int x2, int y2, DWORD renderColor, int isRealColor);


bool SetDirect3DVideoMode(bool fullscreen, int width, int height)
{
	logFile->WriteLine("Trying to set videomode %dx%d", width, height);
	if (g_pDebugFont)
		g_pDebugFont->OnLostDevice();

	D3DPRESENT_PARAMETERS Params;
	ZeroMemory(&Params, sizeof(Params));    //Обнуляем
	Params.hDeviceWindow = g_hWnd;
	Params.Windowed = !fullscreen;
	Params.BackBufferWidth = width;
	Params.BackBufferHeight = height;
	Params.BackBufferCount = 1;
	//Param.BackBufferFormat = D3DFMT_D32;
	Params.EnableAutoDepthStencil = true;
	Params.AutoDepthStencilFormat = D3DFMT_D24S8;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.BackBufferFormat = D3DFMT_X8R8G8B8;
	Params.FullScreen_RefreshRateInHz = 0;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	HRESULT hr = g_pDirect3DDevice->Reset(&Params);
	if (FAILED(hr))
	{
		logFile->WriteLine("Cannot set videomode 0x%x", hr);
		return false;
	}
	else
	{
		g_bIsFullscreenMode = fullscreen;
		g_WinApp.width = width;
		g_WinApp.height = height;
		g_iWindowWidth = width;
		g_iWindowHeight = height;
		if (g_WinApp.pCurrentScreenElement)
		{
			g_WinApp.pCurrentScreenElement->pGraphicsData->width = width;
			g_WinApp.pCurrentScreenElement->pGraphicsData->height = height;
		}
		if (g_pDebugFont)
			g_pDebugFont->OnResetDevice();
	}
	logFile->WriteLine("Videomode set successfully");
	return true;
}

bool InitDirect3D()
{
	if ((g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)//Создаем интерфейс Direct3D
		return false;                //Иначе возвращяем false

	D3DPRESENT_PARAMETERS PresParam;        //Структура с помощью которой передаем информацию устройству рендеринга при его создании
	ZeroMemory(&PresParam, sizeof(PresParam));    //Обнуляем

	HRESULT hr = NULL;                //Создаем переменную для записи в неё результатов работы функций

	D3DDISPLAYMODE DisplayMode;            //Структура для получения информации о режиме отображения в виндовс
	hr = g_pDirect3D->GetAdapterDisplayMode(    //Получаем режим отображения
		D3DADAPTER_DEFAULT,              //Используем первичную видеокарту
		&DisplayMode);                //Записываем режим отображения в DisplayMode

	if (FAILED(hr))                  //Если не получилось
		return false;                //Возвращаем false

	PresParam.hDeviceWindow = g_hWnd;        //Дескриптор окна
	PresParam.Windowed = !g_bIsFullscreenMode;            //Оконный режим?
	PresParam.BackBufferWidth = g_iWindowWidth;    //Ширина заднего буфера
	PresParam.BackBufferHeight = g_iWindowHeight;  //Высота заднего буфера
	PresParam.BackBufferCount = 1;          //Количество задних буферов
	//PresParam.BackBufferFormat = D3DFMT_D32;
	PresParam.EnableAutoDepthStencil = true;    //Используем буфер глубины и стенцил буфер
	PresParam.AutoDepthStencilFormat = D3DFMT_D24S8;  //Формат буфера глубины
	PresParam.SwapEffect = D3DSWAPEFFECT_DISCARD;    //Режим смены кадров
	PresParam.BackBufferFormat = DisplayMode.Format;//Устанавливаем формат пикселя определеный в виндовс
	PresParam.FullScreen_RefreshRateInHz = 0;
	PresParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	hr = g_pDirect3D->CreateDevice(        //Создаем устройство рендеринга
		D3DADAPTER_DEFAULT,              //Используем первичную видеокарту
		D3DDEVTYPE_HAL,                //Устройства рендеринга использует возможности видеокарты
		g_hWnd,                    //Дескриптор окна
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,    //Обрабатываем вершинны видеокартой
		&PresParam,                  //Отдаем параметры устройства
		&g_pDirect3DDevice);            //Создаем устройство рендеринга

	logFile->WriteLine("CreateDeviceError: 0x%X", hr);

	*(int*)0x720900 = 0x800000;
	*(int*)0x6CEC88 = 4; // videomemstate
	*(int*)0x6CED90 = 4; // memstate

	g_bCanBlendMaterials = 1;
	g_bCanSelectArg2 = 1;
	g_bCanBlendCurrentAlpha = 1;
	g_bCanModulate2X = 1;
	g_dwPrevMaterialHadAlpha = 1;
	g_bCanSelectArg1 = 1;
	g_bCanBlendFactorAlpha = 1;

	if (SUCCEEDED(hr))                //Если получилось 
		return true;                //Возвращяем true

	hr = g_pDirect3D->CreateDevice(          //Создаем устройство рендеринга
		D3DADAPTER_DEFAULT,              //Используем первичную видеокарту
		D3DDEVTYPE_HAL,                //Устройства рендеринга использует возможности видеокарты
		g_hWnd,                    //Дескриптор окна
		D3DCREATE_MIXED_VERTEXPROCESSING,      //Обрабатываем вершинны смешано (видеокартой и процессором)
		&PresParam,                  //Отдаем параметры устройства
		&g_pDirect3DDevice);            //Создаем устройство рендеринга

	if (SUCCEEDED(hr))                //Если получилось
		return true;                //Возвращяем true

	hr = g_pDirect3D->CreateDevice(          //Создаем устройство рендеринга
		D3DADAPTER_DEFAULT,              //Используем первичную видеокарту
		D3DDEVTYPE_HAL,                //Устройства рендеринга использует возможности видеокарты
		g_hWnd,                    //Дескриптор окна
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,    //Обрабатываем вершинны процесором
		&PresParam,                  //Отдаем параметры устройства
		&g_pDirect3DDevice);            //Создаем устройство рендеринга

	if (SUCCEEDED(hr))                //Если получилось
		return true;                //Возвращяем true

	return false;                  //Возвращяем false
}

bool InitDirectDraw(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem)
{
	ShutdownDirect3D();
	g_iWindowWidth = 800;
	g_iWindowWidth = 600;
	gDepth = 3;
	gDepthTextures = 3;
	logFile->WriteLine("InitDirectDraw(%d, %d, %d, 0x%X) - 0x%X", width, height, depthMode, flags, *(&width - 1));
	return !InitDirect3D();
}

bool ReInitDirectDraw(int flags)
{
	ShutdownDirect3D();
	g_iWindowWidth = 800;
	g_iWindowWidth = 600;
	gDepth = 3;
	gDepthTextures = 3;
	return !InitDirect3D();
}

void GetViewportRect(GraphicsData *gData, RECT *rect)
{
	if (gData->parent)
	{
		*rect = gData->rect;
	}
	else
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = gData->width;
		rect->bottom = gData->height;
	}
}

void __fastcall GraphicsDataSetupViewport(GraphicsData *gData)
{
	if (gData->flags & 0x1000 || (gData->parent && gData->parent->flags & gData->parent->flags_byte[1] & 0x10))
	{
		D3DVIEWPORT9 viewportData;
		RECT view;
		GetViewportRect(gData, &view);

		int width = gData->width;
		int height = gData->height;
		int left = view.left;
		int top = view.top;
		if (view.left < 0)
		{
			width += view.left;
			left = 0;
		}
		if (view.top < 0)
		{
			height += view.top;
			top = 0;
		}

		viewportData.MinZ = 0.0;
		viewportData.MaxZ = 1.0;
		viewportData.X = view.left;
		viewportData.Y = view.top;
		viewportData.Width = width;
		viewportData.Height = height;

		// TODO
		viewportData.X = 0;
		viewportData.Y = 0;
		viewportData.Width = g_iWindowWidth;
		viewportData.Height = g_iWindowHeight;
		
		g_pDirect3DDevice->SetViewport(&viewportData);
	}
}

void __fastcall GraphicsDataClear(GraphicsData *gData, int nop, int a2, int a3)
{
	D3DRECT rect;
	if (gData->parent)
		gData = gData->parent;
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = gData->width;
	rect.y2 = gData->height;
	g_pDirect3DDevice->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(10, 10, 10), 1.0, 0);
}

void __fastcall GraphicsClearBuffers(GraphicsData *gData)
{
	if (gData->flags & 0x4000)
		GraphicsDataClear(gData, 0, 0, 0);
}

char __fastcall GraphicsSwapBuffers(GraphicsData *gData)
{
	g_pDirect3DDevice->EndScene();
	g_pDirect3DDevice->Present(0, 0, 0, 0);
	g_pDirect3DDevice->BeginScene();
	return 0;
}

void __fastcall CWinAppDrawFrame(CWinApp *win)
{
	if (SUCCEEDED(g_pDirect3DDevice->BeginScene()))
	{
		D3DRECT rect;
		rect.x1 = 0;
		rect.y2 = 0;
		rect.x2 = win->m_pGameStruct->pGraphicsData->width;
		rect.y2 = win->m_pGameStruct->pGraphicsData->height;
		g_pDirect3DDevice->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 1.0, 0);

		g_pDirect3DDevice->EndScene();
		g_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	}
}

int gFacesCount = 0;

DWORD lastTick = 0;
LONGLONG g_llMaxTime = 0;
void DrawFrame()
{
	//while (lastTick - GetTickCount() < 100)
		//Sleep(5);

	gFacesCount = 0;
	if (!g_WinApp.field_148)
	{
		*(int*)0x721794 = 1;
		return;
	}
	g_WinApp.field_148 = 0;

	HRESULT hr = g_pDirect3DDevice->TestCooperativeLevel();//Проверяем потерял ли Direct3DDevice устройство

	if (hr == D3DERR_DEVICELOST)            //Если да то
		return;                   //Выходи из функции  

	hr = g_pDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(10, 10, 10), 1.0, 0);

	if (SUCCEEDED(g_pDirect3DDevice->BeginScene()))
	{
		LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
		LARGE_INTEGER Frequency;

		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&StartingTime);

		if (g_WinApp.bGamePause)
		{
			g_WinApp.pCurrentScreenElement->SetupViewport();
			g_WinApp._base._vmt->Draw2D(&g_WinApp);
		}
		else
		{
			g_WinApp.pCurrentScreenElement->SetupViewport();
			GraphicsDataClear(g_WinApp.pCurrentScreenElement->pGraphicsData, 0, g_WinApp.field_128, 0);
			((int(__thiscall *)(CWinApp*))0x4E0DD0)(&g_WinApp);
			for (ScreenElement *i = g_WinApp.m_pSprites; i; i = (ScreenElement *)i->m_pNext)
			{
				if (i->bFlags & 2 && i != g_WinApp.pCurrentScreenElement)
				{
					i->DrawSprites();
				}
			}

			g_WinApp._base._vmt->Draw2D(&g_WinApp);
			g_WinApp.field_168 = 0.0;
		}
		if (g_WinApp.m_pGameStruct)
		{
			ControllerData cData;
			while (true)
			{
				int endEvents = ((int(__cdecl *)(ControllerData*))0x5ACA20)(&cData);
				if (endEvents)
					break;

				switch (cData.id)
				{
				case 0x200:
				case 0x201:
				case 0x202:
				case 0x203:
				case 0x204:
					g_WinApp.timeDelta = ((double(_stdcall *)())0x6036B0)(); // getTickCount
					endEvents = 0;

					if (g_WinApp.field_124 & 4)
					{
						g_WinApp.mousePosX = (int)(((double)g_WinApp.mousePosX + (double)cData.x) * 0.5);
						g_WinApp.mousePosY = (int)(((double)g_WinApp.mousePosY + (double)cData.y) * 0.5);
						cData.x = g_WinApp.mousePosX;
						cData.y = g_WinApp.mousePosY;
					}
					else
					{
						g_WinApp.mousePosX = cData.x;
						g_WinApp.mousePosY = cData.y;
					}
					
					ScreenElement *foundElement;
					if (g_WinApp.field_124 & 8)
						foundElement = g_WinApp.m_pGameStruct;
					else
						foundElement = ((ScreenElement *(__thiscall *)(CWinApp*, int, int))0x5E1C00)(&g_WinApp, g_WinApp.mousePosX, g_WinApp.mousePosY); // CWinApp::GetElementOnCoords

					if (foundElement)
					{
						if (!g_WinApp.bGamePause)
						{
							((int(__thiscall *)(GraphicsData*, __int16 *, __int16 *))0x5B6490)(foundElement->pGraphicsData, &cData.x, &cData.y);
							foundElement->ProcessEvents(&cData);
						}
					}
					break;

				default:
					((int(__thiscall *)(GraphicsData*, __int16 *, __int16 *))0x5B6490)(g_WinApp.m_pGameStruct->pGraphicsData, &cData.x, &cData.y);
					if (!(g_WinApp.field_124 & 0x60) && cData.id == WM_KEYDOWN)
					{
						g_WinApp.timeDelta = ((double(_stdcall *)())0x6036B0)(); // getTickCount
						int keyid = cData.keyId;
						if (keyid == '\x13')
						{
							int isPaused = g_WinApp.bGamePause == 1;
							g_WinApp.bGamePause ^= 1;
							if (isPaused)
							{
								((void(__cdecl *)(int))0x4E1330)(1); // joySendFFBCommandId
								g_WinApp.field_168 = ((double(_stdcall *)())0x6036B0)() - g_WinApp.field_168;
							}
							else
							{
								((void(__cdecl *)(int))0x4E1330)(0); // joySendFFBCommandId
								g_WinApp.field_168 = ((double(_stdcall *)())0x6036B0)();
							}
						}
						else if (g_WinApp.bGamePause)
						{
							g_WinApp._base._vmt->field_BC(&g_WinApp, keyid);
						}
					}
					if (cData.id == WM_SYSKEYDOWN && cData.keyId == 's')
						g_WinApp._base._vmt->field_B4(&g_WinApp);
					g_WinApp._base._vmt->field_C0(&g_WinApp, &cData);
					break;
				}
			}

			MenuMouse *mouse = 0;
			if (g_WinApp.field_124 & 8)
			{
				mouse = g_WinApp.m_pGameStruct->cursor;
			}
			else
			{
				auto elem = ((ScreenElement *(__thiscall *)(CWinApp*, int, int))0x5E1C00)(&g_WinApp, g_WinApp.mousePosX, g_WinApp.mousePosY);
				if (elem)
					mouse = elem->cursor;
			}

			if (mouse)
				MaskDraw(g_WinApp.m_pGameStruct->pGraphicsData, g_WinApp.mousePosX, g_WinApp.mousePosY, mouse->mask, 0);
		}

		/*Texture *tex = Texture::GetTextureFromPool("m16\\loadpix.txr");
		if (tex)
			RenderQuadPrimitive(g_pDirect3DDevice, tex->m_pSurface, &D3DXVECTOR2(0, 0), &D3DXVECTOR2(640, 480), &D3DXVECTOR2(0, 0), &D3DXVECTOR2(1, 1), D3DCOLOR_XRGB(255, 255, 255), false);
			*/

		//DrawLineHook(g_WinApp.m_pGameStruct->pGraphicsData, 0, 0, 300, 300, 7, 0);
		//DrawLineHook(g_WinApp.m_pGameStruct->pGraphicsData, 300, 0, 0, 300, 7, 0);

		//GraphicsData *surface = gTheGame->pGraphicsData;
		//TextDrawHook(surface, 400, 200, 148, *(Font**)0x6CED44, std::to_string(gFacesCount).c_str(), D3DCOLOR_XRGB(255, 255, 255), 1);

		QueryPerformanceCounter(&EndingTime);
		ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
		ElapsedMicroseconds.QuadPart *= 1000000;
		ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

		char debugTxt[128];
		
		sprintf(debugTxt, "FPS: %lld", 1000000 / ElapsedMicroseconds.QuadPart);
		DrawDebugText(6, 1, D3DCOLOR_XRGB(0, 0, 0), debugTxt);
		DrawDebugText(5, 0, D3DCOLOR_XRGB(255, 255, 255), debugTxt);

		if (PlayerId::g_pPlayersPool && PlayerId::g_pPlayersPool->m_pNext && PlayerId::g_pPlayersPool->m_pNext != PlayerId::g_pPlayersPool)
		{
			dvector3 pos = PlayerId::g_pPlayersPool->m_pNext->m_pPlayerData.vrMatrix.pos;
			sprintf(debugTxt, "Player Pos: %.2f %.2f %.2f", (float)pos.x, (float)pos.y, (float)pos.z);
			DrawDebugText(6, 56, D3DCOLOR_XRGB(0, 0, 0), debugTxt);
			DrawDebugText(5, 55, D3DCOLOR_XRGB(255, 255, 255), debugTxt);
		}

		g_pDirect3DDevice->EndScene();
		g_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);

		lastTick = GetTickCount();
	}

	if (!g_WinApp.m_bNeedToDestroyWnd)
		g_WinApp.field_148 = 1;
}

bool g_bApplicationState = true;

long WINAPI WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	logFile->WriteLine("WndProc: %d 0x%X 0x%X", iMsg, wParam, lParam);
	ControllerData cData;
	RECT rect;
	switch (iMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		cData.id = iMsg;
		cData.x = GET_X_LPARAM(lParam);
		cData.y = GET_Y_LPARAM(lParam);
		cData.keyId = (__int16)wParam;
		((void(__cdecl*)(ControllerData*))0x5ACAF0)(&cData);
		break;

	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
		cData.id = iMsg;
		cData.keyId = (__int16)wParam;
		cData.x = 0;
		cData.y = 0;
		((void(__cdecl*)(ControllerData*))0x5ACAF0)(&cData);
		break;

	case WM_SIZE:
		if (g_bGameInitialized) 
		{
			GetClientRect(hWnd, &rect);
			logFile->WriteLine("WM_SIZE: %d %d", rect.right - rect.left, rect.bottom - rect.top);
			SetDirect3DVideoMode(false, rect.right - rect.left, rect.bottom - rect.top);
		}
		break;

	case WM_DESTROY:              //Если получаем сообщение о разрушении окна
		{
			g_bApplicationState = false;      //Устанавливаем состояния приложения в false (это значит что цикл обработки сообщений остановиться)
			g_WinApp.m_bNeedToDestroyWnd = 1;
			return 0;                //Говорим виндовс что мы это сообщение обработали
		}
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);  //Если нету для нас нужных сообщений, пусть это обрабатывает виндовс
}

void Shutdown()
{
	ShutdownDirect3D();

	if (!DestroyWindow(g_hWnd))            //Если не получилось разрушить окно
		g_hWnd = NULL;                //Устанавливаем дескриптор окна в ноль

	if (!UnregisterClass("rignroll", g_hInstance))  //Если не получилось удалить наше зарегестрированое окно
		g_hInstance = NULL;              //Устанавливаем дескриптор приложения в ноль
}

int __stdcall WinMainHook(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_hInstance = hInstance;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);        //Размер структуры
	wc.style = CS_HREDRAW | CS_VREDRAW;      //Стили класса окна
	wc.lpfnWndProc = WndProc;              //Функция обработки сообщений
	wc.cbClsExtra = 0;                //Количество выделяемой памяти при создании приложения
	wc.cbWndExtra = 0;                //Количество выделяемой памяти при создании приложения
	wc.hInstance = g_hInstance;            //Дескриптор приложения
	wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(130));  //Загружаем стандартную иконку
	wc.hCursor = LoadCursor(0, IDC_ARROW);      //Загружаем стандартный курсор
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//Окно будет закрашено в белый цвет
	wc.lpszMenuName = 0;                //Не используем меню
	wc.lpszClassName = "rignroll";            //Названия класса
	wc.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(130));  //Загружаем стандартную иконку

	if (!RegisterClassEx(&wc))                //Регистрируем класс в Windows
	{
		Shutdown();                    //Освобождаем память
		MessageBox(NULL, "Can`t register window class", "Error", MB_OK | MB_ICONERROR); //Выводим сообщение
		return 0;                    //Завершаем работу приложения
	}

	g_iWindowWidth = 800;
	g_iWindowHeight = 600;


	RECT theRect;
	theRect.top = 50;
	theRect.left = 50;
	theRect.right = g_iWindowWidth + 50;
	theRect.bottom = g_iWindowHeight + 50;
	AdjustWindowRect(&theRect, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0);

	g_hWnd = CreateWindowEx(              //Создаем окно
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,        //Расширенный стиль окна
		"rignroll",                    //Названия класса окна
		"RignRoll 9.0",    //Названия окна
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,//Стиль окна
		theRect.left,
		theRect.top,
		theRect.right - theRect.left,
		theRect.bottom - theRect.top,
		NULL,                    //Это наше главное окно
		NULL,                    //Нету меню
		g_hInstance,                //Дескриптор приложения
		NULL);                    //Дополнительный настроек не используем

	if (g_hWnd == NULL)                //Если не создали окно
	{
		Shutdown();
		MessageBox(NULL, "Can`t create window", "Error", MB_OK | MB_ICONERROR);//Выводим сообщение
		return 0;                  //Завершаем работу приложения
	}

	if (!InitDirect3D())    //Если не смогли инициализировать Direct3D
	{
		logFile->WriteLine("Direct3D Create failed!");
		Shutdown();
		MessageBox(NULL, "Can`t create direct3d", "Error", MB_OK | MB_ICONERROR);//Выводим сообщение
		return 0;                  //Завершаем работу приложения
	}

	logFile->WriteLine("Direct3D Created successfully!");

	g_pD3DDevice = g_pDirect3DDevice;

	ShowWindow(g_hWnd, SW_SHOW);            //Отображаем окно  
	UpdateWindow(g_hWnd);              //Обновляем окно
	SetFocus(g_hWnd);                //Устанавливаем фокус на наше окно
	SetForegroundWindow(g_hWnd);          //Устанавливаем приоритет окна выше среднего

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	g_WinApp.m_pHinstance = hInstance;
	g_WinApp.m_pPrevHinstance = hPrevInstance;
	g_WinApp.m_sCmdLine = lpCmdLine;
	g_WinApp.m_dwShowCmd = nShowCmd;


	InitWindowsFont();
	FirstGameInit();
	ShowCursor(0);
	g_bGameInitialized = true;

	while (!g_WinApp.m_bNeedToDestroyWnd)            //Начинаем бесконечный цикл обработки сообщений
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))//Получаем сообщения
		{
			TranslateMessage(&msg);          //Обрабатываем сообщения
			DispatchMessage(&msg);          //Обрабатываем сообщения
		}
		else
			DrawFrame();              //Если сообщений нету рисуем кадры
	}

	Shutdown();                    //Освобождаем память
	return 0;                    //Завершаем работу приложения
}

int RenderTextureFontHook(Font *font, RECT *targetPos, IDirect3DTexture9 *texture, RECT *srcPos)
{
	//g_pDirect3DDevice->BeginScene();
	D3DXVECTOR2 posStart, posEnd, uvStart, uvEnd;
	posStart.x = targetPos->left;
	posStart.y = targetPos->top;
	posEnd.x = targetPos->right;
	posEnd.y = targetPos->bottom;

	int width = font->charWidthPx * 16;
	int height = (font->charHeightPx + 1) * 16;

	uvStart.x = (float)(srcPos->left+1) / (float)width;
	uvStart.y = (float)(srcPos->top+1) / (float)height;
	uvEnd.x = (float)(srcPos->right+1) / (float)width;
	uvEnd.y = (float)(srcPos->bottom+1) / (float)height;
	//RenderQuadPrimitive(g_pDirect3DDevice, texture, &posStart, &posEnd, &uvStart, &uvEnd, D3DCOLOR_XRGB(255, 255, 255));
	//g_pDirect3DDevice->EndScene();
	return 1;
}

DWORD ConvertRGBForLine(BYTE r, BYTE g, BYTE b)
{
	return D3DCOLOR_XRGB(r, g, b);
}

int DrawLineHook(GraphicsData *a1, int x1, int y1, int x2, int y2, DWORD renderColor, int isRealColor)
{
	//g_pDirect3DDevice->BeginScene(); 

	if (!isRealColor)
	{
		GraphicsPalette *palette = a1->GetPalette();
		renderColor = D3DCOLOR_XRGB(palette->palette[renderColor].r, palette->palette[renderColor].g, palette->palette[renderColor].b);
	}

	D3DXVECTOR2 posStart, posEnd;
	posStart.x = x1;
	posStart.y = y1;
	posEnd.x = x2;
	posEnd.y = y2;

	RenderLinePrimitive(g_pDirect3DDevice, &posStart, &posEnd, renderColor);
	//g_pDirect3DDevice->EndScene();
	return 1;
}

int TextDrawHook(GraphicsData *surface, int x, int y, int propLen, Font *font, const char *text, unsigned int color, int isRealColor)
{
	D3DXVECTOR2 posStart, posEnd, uvStart, uvEnd;
	posStart.x = x;
	posStart.y = y - font->charHeightPx;
	posEnd.x = x + font->charWidthPx;
	posEnd.y = y;

	int width = font->charWidthPx * 16;
	int height = (font->charHeightPx) * 16;

	GraphicsPalette *palette = surface->GetPalette();
	if (palette && !isRealColor)
	{
		color = D3DCOLOR_XRGB(palette->palette[color].r, palette->palette[color].g, palette->palette[color].b);
	}

	double widthUvChar = 0.0625;
	double heightUvChar = 0.0625;
	double pixelUvH = 1.0 / (double)height;
	double pixelUvW = 1.0 / (double)width;

	while (*text)
	{
		BYTE _char = *text;
		int posX = _char % 16;
		int posY = _char / 16;

		int charProp = 0;
		int charProp2 = 0;
		if (font->flags & 1) // has proportional
		{
			if (*text < font->startChar || *text > font->numChars)
			{
				charProp = font->charWidthPx;
			}
			else
			{
				charProp = font->charProp[*text - font->startChar];
				charProp2 = font->field_10[*text - font->startChar];
			}
		}
		else 
		{
			charProp = font->charWidthPx;
		}

		uvStart.x = posX * widthUvChar;
		uvStart.y = posY * heightUvChar + pixelUvH;
		uvEnd.x = (posX + 1) * widthUvChar;
		uvEnd.y = (posY + 1) * heightUvChar;

		RenderQuadPrimitive(g_pDirect3DDevice, font->m_pSurface, &D3DXVECTOR2( width, height), &posStart, &posEnd, &uvStart, &uvEnd, color);

		//if (charProp2)
		//	charProp = charProp2;

		posStart.x += charProp;
		posEnd.x += charProp;
		text++;
	}

	return 0;
}

int __fastcall AdjustWinAndGraphics(CWinApp *app, int nop, int width, int height, int depth, int flags)
{
	return 1;
}

void DrawLoadingBar()
{
	static int &loadingBarTotal = *(int*)0x675468;
	static int &loadingBarProgress = *(int*)0x6CED70;
	static Texture *&loadPixTxr = *(Texture**)0x6CED7C;
	static Texture *&loadRunnerTxr = *(Texture**)0x6CED80;
	g_WinApp.pCurrentScreenElement->SetupViewport();
	if (*(int*)0x6CED84)
	{
		int progress = loadingBarProgress;
		loadingBarProgress++;
		if (progress < 0)
			progress = 0;
		if (progress > loadingBarTotal)
			progress = loadingBarTotal;

		if (!(progress % 10))
		{
			int scrWidth = g_WinApp.pCurrentScreenElement->GetWidth();
			int scrHeight = g_WinApp.pCurrentScreenElement->GetHeight();
			int loadPixX = (scrWidth - loadPixTxr->m_iWidth) / 2;
			int loadPixY = (scrHeight - loadPixTxr->m_iHeight) / 2;
			RenderQuadPrimitive(g_pDirect3DDevice,
				loadPixTxr->m_pSurface,
				&D3DXVECTOR2(loadPixTxr->m_iWidth, loadPixTxr->m_iHeight),
				&D3DXVECTOR2(loadPixX, loadPixY),
				&D3DXVECTOR2(loadPixX + loadPixTxr->m_iWidth, loadPixY + loadPixTxr->m_iHeight),
				&D3DXVECTOR2(0, 0),
				&D3DXVECTOR2(1, 1),
				D3DCOLOR_XRGB(255, 255, 255),
				false
			);
			
			double uv1 = ((double)-loadPixX) / ((double)loadRunnerTxr->m_iWidth);
			double width = scrWidth * loadingBarProgress / loadingBarTotal;
			double uv2 = ((double)(-loadPixX + width)) / ((double)loadRunnerTxr->m_iWidth);

			RenderQuadPrimitive(g_pDirect3DDevice,
				loadRunnerTxr->m_pSurface,
				&D3DXVECTOR2(loadRunnerTxr->m_iWidth, loadRunnerTxr->m_iHeight),
				&D3DXVECTOR2(0, loadPixY + 552.0),
				&D3DXVECTOR2(width, loadPixY + 552.0 + loadRunnerTxr->m_iHeight),
				&D3DXVECTOR2(uv1, 0),
				&D3DXVECTOR2(uv2, 1.0),
				D3DCOLOR_XRGB(255, 255, 255),
				false
			);
			
			GraphicsSwapBuffers(g_WinApp.pCurrentScreenElement->pGraphicsData);
		}

	}
}

void StartTheGame(char *path, int flags)
{
	*(int*)0x6CED84 = 1;
	((BOOL(__cdecl *)())0x510510)(); // InitLoadingBar

	while (1)
	{
		//DrawLoadingBar();
		Texture *tex = Texture::GetTextureFromPool("m16\\loadpix.txr");
		if (tex)
			RenderQuadPrimitive(g_pDirect3DDevice, tex->m_pSurface, &D3DXVECTOR2(tex->m_iWidth, tex->m_iHeight), &D3DXVECTOR2(0, 0), &D3DXVECTOR2(640, 480), &D3DXVECTOR2(0, 0), &D3DXVECTOR2(1, 1), D3DCOLOR_XRGB(255, 255, 255), false);

		g_pDirect3DDevice->EndScene();
		g_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
		g_pDirect3DDevice->BeginScene();
	}
}

void PreloadGame(TheGame *game)
{
	FILE *f = fopen("winApp.txt", "wb");
	fwrite(&g_WinApp, sizeof(g_WinApp), 1, f);
	fclose(f);
	f = fopen("theGame.txt", "wb");
	fwrite(g_WinApp.field_144, sizeof(TheGame), 1, f);
	fclose(f);
}

int GetVideoMem(void *a1)
{
	return 0x2000000;
}

void InitColorRenderStates(int a1)
{
	g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	if (gRenderFlags & D2RENDER_ZBUF)
		g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
	else
		g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, 0);
	g_pDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pDirect3DDevice->SetRenderState(D3DRS_DITHERENABLE, 1);
	g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	g_pDirect3DDevice->SetRenderState(D3DRS_SPECULARENABLE, 1);
	g_pDirect3DDevice->SetRenderState(D3DRS_COLORVERTEX, 1);
	g_pDirect3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 1);
	g_pDirect3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, 0);
	g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	
	if (a1 == 1)
	{
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		D3DMATERIAL9 mat;
		mat.Power = 0.0;
		mat.Ambient.r = 0.0;
		mat.Ambient.g = 0.0;
		mat.Ambient.b = 0.0;
		mat.Ambient.a = 1.0;
		mat.Diffuse.r = 0.0;
		mat.Diffuse.g = 0.0;
		mat.Diffuse.b = 0.0;
		mat.Diffuse.a = 1.0;
		mat.Emissive.r = 0.0;
		mat.Emissive.g = 0.0;
		mat.Emissive.b = 0.0;
		mat.Emissive.a = 1.0;
		mat.Specular.r = 0.0;
		mat.Specular.g = 0.0;
		mat.Specular.b = 0.0;
		mat.Specular.a = 1.0;

		g_pDirect3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 1);
		g_pDirect3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 1);
		g_pDirect3DDevice->SetMaterial(&mat);
		g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	}
	else
	{

	}
}

void InitRenderStates(Material *a1, Material *a2)
{
	g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	//g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	g_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, 1);
	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	g_pDirect3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDirect3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDirect3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	
	if (gRenderFlags & 0x2000000)
	{
		g_bCanBlendMaterials = 0;
		g_bCanBlendCurrentAlpha = 0;
		g_bCanSelectArg2 = 0;
		g_bCanBlendFactorAlpha = 0;
		g_dwPrevMaterialHadAlpha = 0;
	}
	else if (g_bCanBlendMaterials)
	{
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		if (a1)
		{
			if (a1->m_pTexture)
				g_pDirect3DDevice->SetTexture(0, a1->m_pTexture->m_pSurface);
		}

		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		if (g_bCanSelectArg1)
		{
			g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		}
		else
		{
			g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		}

		if (a2)
		{
			if (a2->m_pTexture)
				g_pDirect3DDevice->SetTexture(1, a2->m_pTexture->m_pSurface);
		}
		if (g_bCanBlendFactorAlpha)
		{
			g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
		}

		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	}
}

void Direct3DBeginScene()
{
	//g_pDirect3DDevice->BeginScene();
}

void Direct3DEndScene()
{
	//g_pDirect3DDevice->EndScene();
}

void ProcessGameTask(TaskManager *mng, TaskNode *task)
{
	((void (__thiscall *)(TaskManager *, TaskNode *))0x5D3820)(mng, task);
}

void __fastcall TheGameProcess(TheGame *game)
{
	game->_movie.field_238 = 0.0;
	game->field_BDC = 0;
	if (game->_movie.field_1F0)
	{
		game->m_dTimePassed = game->_movie.field_1F8;
		game->_movie.field_1F0 = 0;
	}
	if (!game->m_pTaskManager.tasksProcessed)
	{

	}
	//((void(__thiscall *)(TaskManager*))0x5D37B0)(&game->m_pTaskManager); // TaskManager::Process
	TaskManager *mng = &game->m_pTaskManager;
	for (TaskNode *i = mng->tasks; i; i = mng->tasks)
	{
		if (i->tasksCount > mng->tasksProcessed)
			break;
		ProcessGameTask(mng, i);
	}
	for (TaskNode *j = mng->field_8; j; j = mng->field_8)
	{
		if (j->tasksCount > ((DWORD (__thiscall *)(TaskManager*))0x5D3800)(mng))
			break;
		ProcessGameTask(mng, j);
	}
	mng->tasksProcessed++;

	if (game->field_BDC == 0)
	{
		if (game->field_BD0)
		{
			if (!game->_movie.field_1AC || !*(DWORD *)(game->_movie.field_1AC + 20))
			{
				((int(__thiscall *)(TheGame *))0x4E0770)(game);
				game->field_BD0 = 0;
			}
		}
	}
}

void Direct3DClearRectWithColor(GraphicsData *a1, RECT *a2, DWORD color, int bIsRealColor)
{
	D3DRECT rect;
	rect.x1 = a2->left;
	rect.y1 = a2->top;
	rect.x2 = a2->right;
	rect.y2 = a2->bottom;
	if (!bIsRealColor)
	{
		GraphicsPalette *plt = a1->GetPalette();
		if (plt)
			color = D3DCOLOR_XRGB(plt->palette[color].r, plt->palette[color].g, plt->palette[color].b);
	}
	g_pDirect3DDevice->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0, 0);
}

__declspec(naked) int __fastcall CBlockContainer0x5Render(CBlockContainer0x5 *block, int nop, int a2, D3DRenderData *a3)
{
	__asm
	{
		jmp CBlockContainer0x5::Render
	}
}

void __fastcall TaskParticlesRender(void *vertices, int count)
{
	g_pDirect3DDevice->SetFVF(D3DFVF_DIFFUSE | D3DFVF_XYZ);
	g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, count / 2, vertices, sizeof(CVector)+4);
}

__declspec(naked) void TaskParticlesRenderHook()
{
	__asm
	{
		mov ecx, [ebx + 0x50]
		mov edx, [esp + 0x68 - 0x5C]
		call TaskParticlesRender
		mov eax, 0x594977
		jmp eax
	}
}

void InitGraphicsModHooks()
{
	// редирект всех инитов
	CPatch::RedirectJump(0x5D7B70, InitDirectDraw);

	// убираем все вызовы кроме одного
	CPatch::RedirectJump(0x5D7500, ReInitDirectDraw);
	CPatch::Nop(0x5E3D68, 5);

	// BeginScene - EndScene
	CPatch::RedirectJump(0x5B6110, Direct3DBeginScene);
	CPatch::RedirectJump(0x5B6120, Direct3DEndScene);

	// установка правильного положения и размера окна и графики
	CPatch::RedirectJump(0x5DD850, AdjustWinAndGraphics);
	//CPatch::RedirectJump(0x512F84, (void*)0x5130C8);
	CPatch::RedirectJump(0x5D7150, GetVideoMem);

	// set default depth 4 (32bit)
	CPatch::SetChar(0x5d80fe, 3);

	// nop enumtextureformats
	CPatch::SetChar(0x5D83B0, 0xC3);

	CPatch::RedirectJump(0x5B61A0, ConvertRGBForLine);

	// remove testGraphicsAccelerator
	CPatch::RedirectJump(0x515C6C, (void*)0x515D36);

	// patch font::init_surface func
	Font::InitHooks();

	// patch texture funcs
	Texture::InitHooks();

	// patch mask funcs
	Mask::InitHooks();

	// loadmodule
	CPatch::SetChar(0x5DB4AD, 0xEB);

	// patch graphicsData::setupViewport
	CPatch::RedirectJump(0x5B6B80, GraphicsDataSetupViewport);

	// patch graphicsData::clear
	CPatch::RedirectJump(0x5B6D90, GraphicsDataClear);

	// patch graphicsData::clearBuffers
	CPatch::RedirectJump(0x5B6E60, GraphicsClearBuffers);

	// patch graphicsData::swapBuffers
	CPatch::RedirectJump(0x5B6AA0, GraphicsSwapBuffers);

	// replace cwinapp::drawframe
	CPatch::RedirectCall(0x61FF25, WinMainHook);

	// remove CWnd::SetWindowPos
	CPatch::RedirectJump(0x6337CA, (void*)0x633816);

	// remove PalettesStuff
	CPatch::SetChar(0x5BA360, 0xC3);

	// remove demo show
	CPatch::RedirectJump(0x5289E4, (void*)0x528B46);

	// patch draw loading bar
	CPatch::RedirectJump(0x5105F0, DrawLoadingBar);

	// DrawText patch
	CPatch::RedirectJump(0x5EA150, TextDrawHook);

	// DrawLine patch
	CPatch::RedirectJump(0x5B7330, DrawLineHook);

	// remove net code
	CPatch::Nop(0x51A6B5, 19);

	//CPatch::RedirectJump(0x510B20, StartTheGame);
	//CPatch::RedirectJump(0x5C48D0, PreloadGame);

	CPatch::SetChar(0x50BE20, 0xC3);

	// patch 8bit texture loading
	CPatch::SetChar(0x5E8B72, 0x18);
	CPatch::SetChar(0x5E8B8E, 0x1A);

	// patch iostream??
	CPatch::SetChar(0x56E5BF, 0xEB);

	// patch SpriteContainer::SetActiveSprite
	CPatch::Nop(0x527E74, 2);

	// patch GetDeviceIdentifier
	CPatch::RedirectJump(0x4B9717, (void*)0x4B974F);

	// patch InitRenderStates
	CPatch::RedirectCall(0x5E17F4, InitRenderStates);
	CPatch::RedirectJump(0x5B00B0, InitColorRenderStates);

	// patch SetTexture??? and Reset Textures
	CPatch::SetInt(0x5AE9D3, 0x10C);
	CPatch::SetInt(0x5AEA4A, 0x104);
	CPatch::SetInt(0x5AEA5C, 0x104);
	CPatch::SetInt(0x5AEA6E, 0x104);
	CPatch::SetInt(0x5AEA80, 0x104);
	CPatch::SetInt(0x5AEA92, 0x104);

	// patch SetFog
	CPatch::RedirectJump(0x5ADC10, Direct3DSetFog);
	CPatch::RedirectJump(0x5ADD80, Direct3DSetAmbient);

	// set viewport
	CPatch::SetChar(0x5ADF03, 0xBC);

	// set transform
	CPatch::SetChar(0x5AE05D, 0xB0); 
	CPatch::RedirectJump(0x5AE49E, SetTransformHook1);
	CPatch::RedirectJump(0x5EB3D5, SetTransformHook2);
	CPatch::RedirectJump(0x5AE928, SetTransformHook3); // reset transform

	CPatch::RedirectJump(0x5AE79E, SetupWorldTransformHook);

	/*CPatch::SetChar(0x5AE7A2, 0xff); // set world transform
	CPatch::SetChar(0x5AE7A3, 0x92);
	CPatch::SetChar(0x5AE7A4, 0xb0);
	CPatch::SetChar(0x5AE7A5, 0x00);
	CPatch::SetChar(0x5AE7A6, 0x00);
	CPatch::SetChar(0x5AE7A7, 0x00);
	CPatch::Nop(0x5AE7A8, 16);*/

	Lightning::InitHooks(); // light patches

	// set render state
	//CPatch::SetChar(0x5EB3ED, 0xE4);
	//CPatch::SetChar(0x5EB3F9, 0xE4);
	//CPatch::SetChar(0x5EB42A, 0xE4);

	CPatch::RedirectJump(0x5ADE30, sub_5ADE30);

	// remove waitforprimarysurface
	CPatch::SetChar(0x5D43B0, 0xC3);

	// TaskWeatherAndTime::SetPuddle
	CPatch::RedirectCall(0x5A8B0D, TextureInitSurface2);

	// Draw map and etc
	CPatch::Nop(0x5763A5, 10); // Set Render State
	CPatch::Nop(0x5756AC, 8); // Set Render State

	// Rewrite of Block::Render functions
	Blocks3D::InitHooks();

	CPatch::RedirectJump(0x615600, (void*)0x6156C1); // Terrain::Render

	// Temporary nopped functions
	//CPatch::RedirectJump(0x5F15E0, D3DRenderDataRenderPrimitive); // D3DRenderData::DrawPrimitive
	//CPatch::RedirectJump(0x5B0FB0, (void*)0x5B166C); // D3DRenderData::DoSomeRenderingSetup
	//CPatch::RedirectJump(0x5AF2E0, D3DRenderDataRenderFace); // D3DRenderData::RenderFace
	CPatch::SetChar(0x5C6A80, 0xC3); // unk draw primitive func
	CPatch::SetChar(0x5C8AA0, 0xC3); // Tree Generator Callback
	CPatch::SetChar(0x5AA6D0, 0xC3); // Sea Generator Callback
	//CPatch::RedirectJump(0x5F3690, (void*)0x5F36AB); // CBlockContainer0x5::Render
	
	//CPatch::RedirectJump(0x57C360, (void*)0x57C3DC); // CBlockShadowObj::Render
	//CPatch::RedirectJump(0x4BCC00, (void*)0x4BCD04); // CBlockContainerTDTN::Render
	//CPatch::RedirectJump(0x4CCA50, (void*)0x4CCB15); // CBlockPeople::Render
	//CPatch::RedirectJump(0x4C2840, (void*)0x4C285C); // CBlockDrops::Render
	//CPatch::RedirectJump(0x4C2A30, (void*)0x4C2A4C); // CBlockDrops2::Render
	//CPatch::RedirectJump(0x4C0DA0, (void*)0x4C0DBC); // CBlockSnow::Render
	//CPatch::RedirectJump(0x4C8A50, (void*)0x4C8A64); // CBlockReflection::Render
	CPatch::RedirectJump(0x59494B, TaskParticlesRenderHook); // TaskParticles::Render
	//CPatch::RedirectJump(0x5930A0, (void*)0x5930BF); // CBlockContainerSortNode::Render
	CPatch::RedirectJump(0x4C51C0, (void*)0x4C5706); // CGlowNode::Render
	//CPatch::RedirectJump(0x59A930, (void*)); // Unk::Render
	CPatch::RedirectJump(0x59E9D0, (void*)0x59EA2B);
	CPatch::RedirectJump(0x5B2FA0, (void*)0x5B386D); // Some Glow Node Render
	CPatch::RedirectJump(0x569B57, (void*)0x569B7F); // TaskPager Fix

	CPatch::SetChar(0x595200, 0xC3); // Mirrors::Process
	CPatch::SetChar(0x595230, 0xC3); // Mirrors::Process

	//CPatch::SetChar(0x4CE820, 0xC3); // Vehicle::Render
	//CPatch::SetChar(0x5DF660, 0xC3); // RoomRender

	//CPatch::RedirectJump(0x5F3690, CBlockContainer0x5Render);
	//CPatch::SetChar(0x5F36CB, 0xEB);
	CPatch::SetChar(0x5DD460, 0xC3); // some while(true) problem

	CPatch::RedirectJump(0x5B6F20, Direct3DClearRectWithColor); // unk clear direct3d
	
	//CPatch::RedirectCall(0x56CE1E, TaskPagerDraw);

	//CPatch::RedirectJump(0x56F650, TheGameProcess); // TheGame::Process

	CPatch::RedirectJump(0x5AE9C4, (void*)0x5AE9D7);

	CPatch::RedirectJump(0x5B0FB0, D3DRenderDataApplyMat);
	CPatch::RedirectJump(0x5A64B6, (void*)0x5A6779); // taskwipers some copy surface process

	//CPatch::SetChar(0x4DF380, 0xC3);

}

void ShutdownDirect3D()
{

	if (g_pDirect3DDevice != NULL)          //Если мы еще не освободили интерфейс рендеринга
	{
		g_pDirect3DDevice->Release();        //То освобождаем его
		g_pDirect3DDevice = NULL;          //И устанавливаем в ноль
	}

	if (g_pDirect3D != NULL)              //Если мы еще не освободили интерфейс d3d
	{
		g_pDirect3D->Release();            //То освобождаем его
		g_pDirect3D = NULL;              //И устанавливаем в ноль
	}
}
