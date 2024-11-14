#include <Windows.h>

class StreamManager
{
private:
	HMMIO m_pHmmio;
	BYTE m_pBuf[0x20200];
	BYTE *m_pBufEnd;
	DWORD m_dwReadSize;

public:
	StreamManager();
	int Open(LPSTR filename);
	int Close();
	char *GetBufEnd();
	int Skip(DWORD startOffset, DWORD size);
	int Read(DWORD startOffset, char *dest, DWORD size);
};