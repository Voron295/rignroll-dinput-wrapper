#include <Windows.h>

void GetRGBFrom555(WORD color, BYTE &r, BYTE &g, BYTE &b);
void GetRGBFrom565(WORD color, BYTE &r, BYTE &g, BYTE &b);
WORD Get565FromRGB(BYTE r, BYTE g, BYTE b);
WORD Get565From555(WORD color);
WORD Get555FromRGB(BYTE r, BYTE g, BYTE b);
DWORD GetRGBAFrom4444(WORD color);
void GetRGBAFrom4444(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a);
WORD Get4444FromRGBA(BYTE r, BYTE g, BYTE b, BYTE a);
