#include "D3DRenderData.h"

void FirstGameInit();
void __fastcall Direct3DSetFog(D3DRenderData *a1, int nop, double fogstart, double fogend, int a6);
void __fastcall Direct3DSetAmbient(D3DRenderData *a1, int nop, double r, double g, double b);
void __fastcall sub_5ADE30(D3DRenderData *a1);
void SetTransformHook1();
void SetTransformHook2();
void SetTransformHook3();
void SetupWorldTransformHook();
void __fastcall D3DRenderDataApplyMat(D3DRenderData *d3d, int nop, Material *a2, int flag1, int flag2, int flag3);