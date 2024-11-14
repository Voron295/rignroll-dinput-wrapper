void SetupSidePanelOffsets(int a);
void InitWSA(int a);
void SendMessageToAllPlayers(void *src, int size);
void *dNew(int size);
void *dMalloc(int size);
void dAllocPlayersPool(int a);
int __fastcall loadGameFunc(int _this, int dup, char *a2, char *a3, char *a4, char *a5);
void *__fastcall HeapAlocInitByName(void *_this, int dup, char *name);

#include <ddraw.h>
#include <d3d.h>
#include <vector>
#include <Texture.h>
#pragma comment(lib, "DDRAW.LIB")
#pragma comment(lib, "dxguid.lib")


#pragma pack(push, 1)
struct D2Struct
{
	int m_dWidth;
	int m_dHeight;
	int m_dPixelFormat;
	int m_dFlags;
	int m_dMemory;
	int m_bIsNotDepth8bit;
	int m_bFoundVideoMode;
	int m_dwDeviceZBufferBitDepth;
	int m_dVideoMemory;
	int m_dwPixelFormatsFlag;
	IDirectDraw7 *m_pDirectDraw;
	IDirectDraw7 *m_pDirectDraw_1;
	IDirect3D7 *m_pDirect3D7;
	IDirect3DDevice7 *m_pD3DDevice7;
	LPDIRECTDRAWSURFACE7 m_pSurfaceFrontBuffer;
	IDirectDrawSurface7 *m_pSurfaceBuffer;
	IDirectDrawSurface7 *m_pSurfaceBufferZ;
	int field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	GUID *m_lpVideoDeviceGUID;
	GUID m_gVideoDeviceGUID;
	IDirectDrawPalette *m_pPalette;
	char m_sErrorString[80];
	int m_dLastError;
};
#pragma pack(pop)

struct DisplayMode
{
	int width;
	int height;
	int depth;
};

int InitDirectDraw(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem);
int __stdcall DDrawInit(GUID *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, D2Struct *lpContext);
int TestGraphicsAccelerator(int drvNum, int *depthMode, int *vpas, int *vpas2, int *vpam, int *vpam2, int *vpal, int *vpal2, int *vpasl, int *vpasl2);

int __fastcall AdjustWin(int winapp, int nop, int width, int height, int a4, int a5);
int HookInitDirectDraw(int width, int height, int depthMode, int flags, int *errorID, int *tmem, int *vmem);
int __fastcall WinAppInit(int winapp);
int __fastcall unkFuncHook(int game);

HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT pixelFormat, LPVOID _data);
void __cdecl TextureReadHeaderReplaceDepth();
void __cdecl TextureInitReplaceDepth();
void __cdecl FontReadReplaceDepth();
void __cdecl TextureInitSurfaceDesc1();
void __cdecl TextureInitSurfaceDesc2();
void __cdecl MaskInitSurfaceDesc();
void __cdecl FontInitSurfaceDesc();
void __cdecl CreateTextureFromScreenCopy();
int __cdecl TextureReadHeaderReplaceDepthDefault();
int __cdecl LogError(char *a1, char *a2);

void *SetDefaultVectors(void *out, float x, float y, float z);

int InitDirectDrawForMenu(int width, int height, int depthMode, int flags, int *errorid, int *tmem, int *vmem);

void DrawTextureHook(GraphicsData *a1, Texture *a2, RECT *textureRect, int x, int y);
void __fastcall CWinAppDrawFrame(void *winapp);

void Enum3DModes();

void SetupGameHooks();


struct DeviceMode
{
	int width;
	int height;
	int depth;
};

struct DeviceInfoStruct
{
	GUID *deviceGUID;
	char deviceName[256];
	char deviceDesc[256];
	WCHAR deviceNameW[256];
	WCHAR deviceDescW[256];
	std::vector<DeviceMode> pModes;
};

extern std::vector<DeviceInfoStruct> g_pDevicesInfo;
