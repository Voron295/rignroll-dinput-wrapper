#pragma once

#include <Windows.h>
#include <iostream>

class CLog
{
private:
	FILE *m_pFile;
	char *m_sPath;

	CLog() {
		m_sPath = 0;
		m_pFile = 0;
	};
	~CLog() { if (m_sPath) delete[] m_sPath; memset(this, 0, sizeof(CLog)); };

public:
	static CLog *Open(char *path) {
		CLog *log = new CLog();
		if (!log)
			return 0;
		fopen_s(&log->m_pFile, path, "wb");
		if (!log->m_pFile)
			return 0;
		fclose(log->m_pFile);

		log->m_sPath = new char[strlen(path) + 1];
		strcpy_s(log->m_sPath, strlen(path) + 1, path);
		return log;
	};
	bool WriteLine(char *format, ...) {
		fopen_s(&m_pFile, m_sPath, "ab");
		if (!m_pFile)
			return 0;

		char final[512];
		va_list ap;
		va_start(ap, format);
		vsnprintf_s(final, 512, format, ap);
		va_end(ap);
		fputs(final, m_pFile);
		fputs("\n", m_pFile);
		fflush(m_pFile);

		fclose(m_pFile);
		return true;
	};
	void Close() {
		this->~CLog();
	};
};