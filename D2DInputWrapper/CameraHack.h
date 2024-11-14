#include "GameStructs.h"
class CameraHack 
{
public:
	static void SetupHooks();

private:
	static float m_fPosX;
	static float m_fPosY;
	static float m_fPosZ;

	static float m_fRotX;
	static float m_fRotZ;

	static bool m_bEnabled;

	static long m_fMouseX;
	static long m_fMouseY;

	static DWORD m_bDoTeleport;
	static CMatrix m_pCameraMatrix;

	static void Process(CMatrix *processed);
	static int __fastcall CameraProcess(void *_this, int nop, CMatrix *mat, double someParam);
	static void DrawTheHud();
	static void __fastcall ViewerRun(void *_this);
	static void RenderDigits(char *text, int posX, int posY, int height, DWORD color);
};