#include <iostream>
#include "CameraHack.h"
#include "CPatch.h"
#include <math.h>
#include "resources.h"

#define PI 3.141592653

float CameraHack::m_fPosX = 0;
float CameraHack::m_fPosY = 0;
float CameraHack::m_fPosZ = 0;

float CameraHack::m_fRotX = 0;
float CameraHack::m_fRotZ = 0;

bool CameraHack::m_bEnabled = 0;

long CameraHack::m_fMouseX = 0;
long CameraHack::m_fMouseY = 0;

DWORD CameraHack::m_bDoTeleport = 0;

CMatrix CameraHack::m_pCameraMatrix;

void CameraHack::SetupHooks()
{
	CPatch::SetPointer(0x64F304, ViewerRun);
	CPatch::RedirectCall(0x53677C, CameraProcess);
	CPatch::RedirectCall(0x4E1107, DrawTheHud);
}

void __fastcall CameraHack::ViewerRun(void *_this)
{
	((void(__thiscall *)(void *))0x536310)(_this);

	// changing back view
	if (g_pKeyStates['N'].bIsPressed)
	{
		BYTE currentView = CPatch::GetChar(0x595511);
		if (currentView >= 2)
			currentView = 0;
		else
			currentView++;

		CPatch::SetChar(0x595511, currentView);
	}

}

void CameraHack::Process(CMatrix *processed)
{
	POINT p;
	if (g_pKeyStates[0x65].bIsPressed)
	{
		if (!m_bEnabled)
		{
			//memcpy(&m_pCameraMatrix, processed, sizeof(CMatrix));
			m_fPosX = processed->pos.x;
			m_fPosY = processed->pos.y;
			m_fPosZ = processed->pos.z;
			
			((void(__cdecl *)())0x50BDE0)(); // set cursor at center
			if (GetCursorPos(&p))
			{
				ScreenToClient(g_hWnd, &p);
				m_fMouseX = p.x;
				m_fMouseY = p.y;
			}
		}
		m_bEnabled = !m_bEnabled;
	}
	if (m_bEnabled)
	{
		LONG offX, offY;
		if (GetCursorPos(&p))
		{
			ScreenToClient(g_hWnd, &p);
			offX = p.x - m_fMouseX;
			offY = p.y - m_fMouseY;
		}
		((void(__cdecl *)())0x50BDE0)(); // set cursor at center
		if (GetCursorPos(&p))
		{
			ScreenToClient(g_hWnd, &p);
			m_fMouseX = p.x;
			m_fMouseY = p.y;
		}

		CMatrix rotX, rotZ, pos, offset, rotMat, out1, out2;

		if (g_pKeyStates[1].bIsPressed || g_pKeyStates[2].bIsPressed)
		{
			offX = 0;
			offY = 0;
		}

		m_fRotX += (float)offX * -0.01f;
		m_fRotZ += (float)offY * -0.01f;

		CMatrixSetRotX(&rotX, m_fRotX);
		CMatrixSetRotY(&rotZ, m_fRotZ);

		float offsetY = 0.0f;
		float offsetX = 0.0f;
		float offsetZ = 0.0f;
		if (GetKeyState(0x68) & 0x8000)
			offsetY = 0.1f;
		if (GetKeyState(0x62) & 0x8000)
			offsetY = -0.1f;
		if (GetKeyState(0x66) & 0x8000)
			offsetX = 0.1f;
		if (GetKeyState(0x64) & 0x8000)
			offsetX = -0.1f;
		if (GetKeyState(0x67) & 0x8000)
			offsetZ = 0.1f;
		if (GetKeyState(0x69) & 0x8000)
			offsetZ = -0.1f;
		if (GetKeyState(0x60) & 0x8000)
		{
			offsetY *= 10.0f;
			offsetX *= 10.0f;
			offsetZ *= 10.0f;
		}
		
			
		CMatrixSetPos(&offset, offsetX, offsetY, offsetZ);
		CMatrixSetPos(&pos, m_fPosX, m_fPosY, m_fPosZ);

		CMatrixMultiply(&rotMat, &rotZ, &rotX);
		CMatrixMultiply(&out1, &offset, &rotMat);
		CMatrixMultiply(&m_pCameraMatrix, &out1, &pos);
		m_fPosX = m_pCameraMatrix.pos.x;
		m_fPosY = m_pCameraMatrix.pos.y;
		m_fPosZ = m_pCameraMatrix.pos.z;
		//CMatrixMultiply(&out1, &rotMat, &pos);
		//CMatrixMultiply(&out2, &rotMat, &m_pCameraMatrix);

		//memcpy(&m_pCameraMatrix, &out2, sizeof(CMatrix));
		memcpy(processed, &m_pCameraMatrix, sizeof(CMatrix));


		if (g_pKeyStates[0x6E].bIsPressed && !m_bDoTeleport) // teleport
		{
			m_bDoTeleport = 1;
		}
	}
}

int __fastcall CameraHack::CameraProcess(void *_this, int nop, CMatrix *mat, double someParam)
{
	int result = ((int(__thiscall *)(void *, CMatrix*, double))0x599FC0)(_this, mat, someParam);

	Process(mat);
	return result;
}

IDirectDrawSurface7 *g_pSpeedometerSurface = 0;
IDirectDrawSurface7 *g_pDigitalFont = 0;
IDirectDrawSurface7 *g_pDigitalFontPercent = 0;
extern DDPIXELFORMAT &g_PixelFormat4444;
IDirectDrawSurface7 *g_pAkppSurface = 0;

void CameraHack::RenderDigits(char *text, int posX, int posY, int height, DWORD color)
{
	if (!g_pDigitalFont)
	{
		g_pDigitalFont = InitSurfaceFromMemory(g_pDigitalFontTexture, 160, 24, 2, &g_PixelFormat4444);
		g_pDigitalFontPercent = InitSurfaceFromMemory(g_pDigitalFontPercentTexture, 16, 24, 2, &g_PixelFormat4444);
	}
	if (g_pDigitalFont)
	{
		int pos = posX;
		float step = height * 16.0f / 24.0f;
		while (*text)
		{
			if (*text >= '0' && *text <= '9')
			{
				int charId = *text - '0';
				RenderTexturedQuad({ pos, posY, 0 }, { step, height, 0 }, { 0, 0, 0 }, g_pDigitalFont, { charId * 0.1f, 0, 0 }, {(charId + 1) * 0.1f, 1.0, 0}, 0, color);
				pos += step;
			}
			else if (*text == '%')
			{
				RenderTexturedQuad({ pos, posY, 0 }, { step, height, 0 }, { 0, 0, 0 }, g_pDigitalFontPercent, 0, color);
				pos += step;
			}
			text++;
		}
	}
}

void CameraHack::DrawTheHud()
{
	void *vmt = *(void**)g_pCurrentScreenElement;
	DWORD getWidth = *(DWORD *)((DWORD)vmt + 0x14);
	DWORD getHeight = *(DWORD *)((DWORD)vmt + 0x18);
	DWORD width = ((int(__thiscall *)(void *))getWidth)(g_pCurrentScreenElement);
	DWORD height = ((int(__thiscall *)(void *))getHeight)(g_pCurrentScreenElement);
	void *surface = *(void **)((DWORD)g_pCurrentScreenElement + 0xC);

	if (g_pViewer && g_pDirect3DDev)
	{
		float fBias = -3.0;
		g_pDirect3DDev->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD)(&fBias)));
	}
	if (g_pViewer && g_pDirect3DDev && !g_bGamePause && *(DWORD*)((DWORD)g_pViewer + 0x578) == 1 && *(DWORD *)((DWORD)g_pTheGame + 0x10) & 2)
	{
		if (g_pSpeedometerSurface == 0)
		{
			g_pSpeedometerSurface = InitSurfaceFromMemory(g_pSpeedometerTexture, 123, 93, 2, &g_PixelFormat4444);
			g_pAkppSurface = InitSurfaceFromMemory(g_pAkppTexture, 80, 48, 2, &g_PixelFormat4444);
		}
		if (g_pSpeedometerSurface)
		{
			void *vehicle = *(void **)((DWORD)g_pViewer + 0x268);
			CVector *currentSpeedV = (CVector *)((DWORD)vehicle + 0x1B0);
			float currentRpm = *(float *)((DWORD)vehicle + 0x51E8) * 0.6;
			float currentFuel = *(float *)((DWORD)vehicle + 0x51FC);
			int kppType = *(int *)((DWORD)vehicle + 0x5220);
			float currentEngine = 1.0f - *(float *)((DWORD)vehicle + 0x5200);
			float currentSpeed = sqrt(currentSpeedV->x * currentSpeedV->x + currentSpeedV->y * currentSpeedV->y + currentSpeedV->z * currentSpeedV->z) * 3.6;
			CVector speedometerPos = { 100, height - 70.0, 0 };
			float startAngle = -PI * 6.0f / 8.0f;
			float stepAngle = PI / 8.0f;
			void *font = *(void **)((DWORD)g_pTaskPager + 0x50C);

			RenderTexturedQuad({ speedometerPos.x - 59, speedometerPos.y - 53, 0 }, { 123, 93, 0 }, { 0, 0, 0 }, g_pSpeedometerSurface, 0, 0xFFFFFFFF);

			RenderTriangle(
			{ speedometerPos.x, speedometerPos.y - 25.0, 0 },
			{ speedometerPos.x - 2.0, speedometerPos.y + 5.0, 0 },
			{ speedometerPos.x + 2.0, speedometerPos.y + 5.0, 0 },
			speedometerPos, startAngle + currentSpeed / 10.0 * stepAngle, 0xFFFFFF00);

			char digitsSpeed[16];
			char digitsRpm[16];
			char digitsEngine[16];
			char digitsFuel[16];
			sprintf(digitsSpeed, "%03d", (int)currentSpeed);
			sprintf(digitsRpm, "%02d", (int)currentRpm);
			sprintf(digitsFuel, "%03d%%", (int)(currentFuel * 100));
			sprintf(digitsEngine, "%03d%%", (int)(currentEngine * 100));
			RenderDigits(digitsSpeed, speedometerPos.x - 50, speedometerPos.y + 45, 20, 0xFFFFFFFF);
			RenderDigits(digitsRpm, speedometerPos.x + 10, speedometerPos.y + 45, 20, 0xFFb11818);
			RenderDigits(digitsFuel, speedometerPos.x + 70, speedometerPos.y + 20, 20, 0xFFFFFFFF);
			RenderDigits(digitsEngine, speedometerPos.x + 70, speedometerPos.y + 45, 20, 0xFFFFFFFF);

			if (!kppType)
			{
				RenderTexturedQuad({ speedometerPos.x + 70, speedometerPos.y - 5, 0 }, { 53.3, 20, 0 }, { 0, 0, 0 }, g_pAkppSurface, { 0, 0.5f, 0 }, { 0.8f, 1.0f, 0 }, 0, 0xFFFFFFFF);
			}
			else
			{
				RenderTexturedQuad({ speedometerPos.x + 70, speedometerPos.y - 5, 0 }, { 53.3, 20, 0 }, { 0, 0, 0 }, g_pAkppSurface, { 0, 0, 0 }, { 0.8f, 0.5f, 0 }, 0, 0xFFFFFFFF);
			}
			if (*(DWORD*)0x6F346C <= 1)
			{
				if (*(DWORD*)0x6F346C != 0)
					RenderTexturedQuad({ speedometerPos.x + 130, speedometerPos.y - 5, 0 }, { 13.3, 20, 0 }, { 0, 0, 0 }, g_pAkppSurface, { 0.8f, 0.5f, 0 }, { 1.0, 1.0f, 0 }, 0, 0xFFFFFFFF);
				else
					RenderTexturedQuad({ speedometerPos.x + 130, speedometerPos.y - 5, 0 }, { 13.3, 20, 0 }, { 0, 0, 0 }, g_pAkppSurface, { 0.8f, .0f, 0 }, { 1.0, 0.5f, 0 }, 0, 0xFFFFFFFF);
			}
			else
			{
				char digitsGear[16];
				sprintf(digitsGear, "%d", *(DWORD*)0x6F346C - 1);
				RenderDigits(digitsGear, speedometerPos.x + 130, speedometerPos.y - 5, 20, 0xFFFFFFFF);
			}
		}
	}

	((void(__cdecl *)())0x56CDD0)(); // draw hud

	width -= 150;

	if (m_bEnabled)
	{
		char textBuf[512];
		sprintf(textBuf, "%.1f, %.1f, %.1f", m_fPosX, m_fPosY, m_fPosZ);
		RenderText(surface, width, 65, g_pMatrixFont, textBuf, 0, 0);
	}

	if (m_bDoTeleport)
	{
		void *vehicle = (void*)(*(DWORD *)((DWORD)g_pViewer + 0x268) + 0x10);
		if (m_bDoTeleport == 1)
		{
			*(DWORD *)((DWORD)vehicle + 0x174) = 1;
			void *room = *(void **)((DWORD)vehicle + 0x320);
			((void(__thiscall *)(void *, void *, CMatrix *))0x54C860)(vehicle, room, &m_pCameraMatrix);
			m_bDoTeleport = 2;
		}
		else
		{
			m_bDoTeleport++;
		}
		if (m_bDoTeleport == 10)
		{
			*(DWORD *)((DWORD)vehicle + 0x174) = 0;
			m_bDoTeleport = 0;
		}
	}
	
}