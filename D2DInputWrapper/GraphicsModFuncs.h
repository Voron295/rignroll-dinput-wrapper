#include <Windows.h>
#include <d3dx9.h>
#include "D3DRenderData.h"
#include "CFace.h"

void GetRGBFrom565(WORD color, BYTE &r, BYTE &g, BYTE &b);
WORD Get565FromRGB(BYTE r, BYTE g, BYTE b);
WORD Get565From555(WORD color);
DWORD GetRGBAFrom4444(WORD color);
void GetRGBAFrom4444(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a);
void GetRGBAFrom555(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a);

IDirect3DTexture9 *CreateDirect3DTextureFromMemory(IDirect3DDevice9 *device, int width, int height, int depth, char *memory, bool useTransparency, RGB transparentColor = {0, 0, 0});
HRESULT RenderQuadPrimitive(IDirect3DDevice9 *device, IDirect3DTexture9 *texture, LPD3DXVECTOR2 realSize, LPD3DXVECTOR2 posStart, LPD3DXVECTOR2 posEnd, LPD3DXVECTOR2 uvStart, LPD3DXVECTOR2 uvEnd, D3DCOLOR color, bool useAlpha = true);
HRESULT RenderLinePrimitive(IDirect3DDevice9 *device, LPD3DXVECTOR2 posStart, LPD3DXVECTOR2 posEnd, D3DCOLOR color);
void __fastcall D3DRenderDataRenderFace(D3DRenderData *data, int nop, CFace *face);
void D3DRenderDataRenderPrimitive(D3DRenderData *a1, PrimitiveRenderData *data, Material *material, CVector *pos, D3DPRIMITIVETYPE primType);