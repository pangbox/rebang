#include "chttp.h"

cHttp::cHttp()
{
	m_hSession = NULL;
	m_hUrl = NULL;
	m_hWnd = NULL;
	m_dwDownloaded = 0;
}

cHttp::~cHttp()
{
}

bool cHttp::Init(HWND__* hWnd)
{
	m_hWnd = hWnd;
	return true;
}

void cHttp::SendLog(unsigned int msg, long lParam)
{
	if (m_hWnd)
		SendMessage(m_hWnd, msg, (WPARAM)m_szLog, lParam);
}

void cHttp::Close()
{
	if (m_hUrl)
	{
		InternetCloseHandle(m_hUrl);
		m_hUrl = NULL;
	}
	if (m_hSession)
	{
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}
}

bool cHttp::Open(const char* url)
{
	Close();

	m_hSession = InternetOpenA("cHttp DownLoad", 0, NULL, NULL, 0);
	if (m_hSession == NULL)
		return false;

	m_hUrl =
		InternetOpenUrlA(m_hSession, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (m_hUrl == NULL)
	{
		InternetCloseHandle(m_hSession);
		return false;
	}

	Close();
	return true;
}

bool cHttp::DownLoad(const char* filename, const char* urlFile,
	const char* localPath, const char* url)
{
	char szURL[260];
	char szFile[260];
	char buffer[10240];
	unsigned long dwSize;
	unsigned long dwRead;
	unsigned long dwWritten;

	Close();

	sprintf(szURL, "%s%s", url, urlFile);
	sprintf(szFile, "%s%s", localPath, filename);

	m_hSession = InternetOpenA("cHttp DownLoad", 0, NULL, NULL, 0);
	if (m_hSession == NULL)
		return false;

	m_hUrl = InternetOpenUrlA(m_hSession, szURL, NULL, 0,
		INTERNET_FLAG_DONT_CACHE, 0);
	if (m_hUrl == NULL)
	{
		InternetCloseHandle(m_hSession);
		return false;
	}

	HANDLE hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Close();
		return false;
	}

	bool bRet = true;
	do
	{
		if (!InternetQueryDataAvailable(m_hUrl, &dwSize, 0, 0))
		{
			bRet = false;
			break;
		}
		if (dwSize <= 0)
			break;
		if (dwSize > 10240)
			dwSize = 10240;
		if (!InternetReadFile(m_hUrl, buffer, dwSize, &dwRead) ||
			!WriteFile(hFile, buffer, dwRead, &dwWritten, NULL))
		{
			bRet = false;
			break;
		}
	} while (dwRead != 0);

	CloseHandle(hFile);
	Close();
	return bRet;
}

bool cHttp::DownLoad2(const char* filename, const char* urlFile,
	const char* localPath, const char* url, unsigned long fileSize)
{
	char szFile[260];
	char szURL[260];
	char buffer[10240];
	unsigned long dwSize;
	unsigned long dwRead;
	unsigned long dwWritten;
	HANDLE hFile;

	strcpy(szFile, filename);

	Close();

	sprintf(szURL, "%s%s", url, urlFile);
	sprintf(szFile, "%s%s", localPath, filename);

	m_hSession = InternetOpenA("cHttp DownLoad", 0, NULL, NULL, 0);
	if (m_hSession == NULL)
		return false;

	m_hUrl = InternetOpenUrlA(m_hSession, szURL, NULL, 0,
		INTERNET_FLAG_DONT_CACHE, 0);
	if (m_hUrl == NULL)
	{
		InternetCloseHandle(m_hSession);
		goto FAIL;
	}

	hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Close();
		return false;
	}

	unsigned long dwTotal = 0;
	unsigned long dwTime = timeGetTime();
	unsigned long dwLast = 0;
	m_dwDownloaded = 0;
	do
	{
		if (!InternetQueryDataAvailable(m_hUrl, &dwSize, 0, 0))
			return false;

		InternetReadFile(m_hUrl, buffer, dwSize, &dwRead);
		m_dwDownloaded += dwRead;
		WriteFile(hFile, buffer, dwRead, &dwWritten, NULL);
		dwTotal += dwWritten;

		sprintf(m_szLog, "'%s' %dKb/%dKb", urlFile, dwTotal >> 10,
			fileSize >> 10);
		SendLog(0x40b, (int)(100.0f * ((float)dwTotal / (float)fileSize)));

		if (dwTotal - dwLast >= 100000)
		{
			float fBytes = (float)(dwTotal - dwLast);
			dwLast = dwTotal;
			float fSec = (float)(timeGetTime() - dwTime) * 0.001f;
			dwTime = timeGetTime();
			sprintf(m_szLog, "%d Kb/sec",
				(int)((1.0f / 1024.0f) * (fBytes / fSec)));
			SendLog(0x40a, 0);
		}
	} while (dwRead != 0);

	CloseHandle(hFile);
	InternetCloseHandle(m_hUrl);
	InternetCloseHandle(m_hSession);
	return m_dwDownloaded == fileSize ? true : false;
FAIL:
	return false;
}

char* cHttp::DownLoad(const char* file, const char* url, unsigned long& size)
{
	return DownLoad<char>(file, url, size);
}

unsigned long cHttp::GetFileSize(const char* filename)
{
	WIN32_FIND_DATA fd;
	if (!InternetFindNextFile(m_hUrl, &fd))
		return 0;
	return fd.nFileSizeLow;
}
