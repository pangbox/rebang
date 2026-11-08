#include "hackingmanager.h"

extern HWND g_hwnd;
extern bool g_bQuit;

HackingManager::HackingManager()
{
	m_hackToolNum = 0;
	m_hackTool = NULL;
	m_hHookDll = NULL;
	m_hMapFile = NULL;
	m_pInitHooksDll = NULL;
	m_pInstallHook = NULL;
	m_pUnInstallHook = NULL;
	ClearCrypticList();
}

HackingManager::~HackingManager()
{
	Close();
	if (m_hackTool)
	{
		delete m_hackTool;
		m_hackTool = NULL;
	}
	if (m_hHookDll)
		FreeLibrary(m_hHookDll);
	if (m_hMapFile)
		CloseHandle(m_hMapFile);
}

bool HackingManager::Init()
{
	WFile file("BBH.bin");
	file.Length();
	file.Read(&m_hackToolNum, 4);
	if (m_hackToolNum > 0)
		m_hackTool = new sHackTool[m_hackToolNum];
	for (int i = 0; i < m_hackToolNum; ++i)
		file.Read(&m_hackTool[i], sizeof(sHackTool));
	file.Close();

	m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		0, 0x2800, "ExMessageData");
	m_pMapView = (char*)MapViewOfFile(m_hMapFile,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	m_hHookDll = LoadLibrary("JERR.dll");
	if (m_hHookDll == NULL)
		return false;

	m_pInitHooksDll =
		(InitHooksDllFn)GetProcAddress(m_hHookDll, "InitHooksDll");
	m_pInstallHook = (InstallHookFn)GetProcAddress(m_hHookDll, "InstallHook");
	m_pUnInstallHook =
		(UnInstallHookFn)GetProcAddress(m_hHookDll, "UnInstallHook");

	if (m_pInitHooksDll == NULL || m_pInstallHook == NULL ||
		m_pUnInstallHook == NULL)
		return false;

	m_pInitHooksDll(g_hwnd);
	m_pInstallHook(1);

	return true;
}

void HackingManager::Close()
{
	if (m_pInstallHook)
		m_pUnInstallHook(1);
}

void HackingManager::Process()
{
}

bool HackingManager::AllProcessScan()
{
	return false;
}

void HackingManager::ClearCrypticList()
{
	m_byteList.clear();
	m_intList.clear();
	m_floatList.clear();
	m_checkInterval = 0;
	m_checkCount = 0;
}

void HackingManager::ResetCrypticValue(int range)
{
	int interval = rand() % range;
	m_checkCount = 0;
	m_checkInterval = interval + 1;
}

void HackingManager::CheckCrypticValue()
{
	std::list<WCrypticValue<unsigned char>*>::iterator itByte;
	for (itByte = m_byteList.begin(); itByte != m_byteList.end(); ++itByte)
	{
		if (!(*itByte)->IsSafe())
		{
			g_bQuit = true;
			return;
		}
	}

	std::list<WCrypticValue<int>*>::iterator itInt;
	for (itInt = m_intList.begin(); itInt != m_intList.end(); ++itInt)
	{
		if (!(*itInt)->IsSafe())
		{
			g_bQuit = true;
			return;
		}
	}

	std::list<WCrypticValue<float>*>::iterator itFloat;
	for (itFloat = m_floatList.begin(); itFloat != m_floatList.end(); ++itFloat)
	{
		if (!(*itFloat)->IsSafe())
		{
			g_bQuit = true;
			return;
		}
	}
}

bool HackingManager::CheckExcuteProgam()
{
	char filename[256];
	memset(filename, 0, sizeof(filename));
	strcpy(filename, m_pMapView);
	if (strlen(filename) == 0)
		return false;

	if (stricmp(filename, m_lastExe.c_str()) != 0)
	{
		m_lastExe = filename;
		return CompareHackToolByName(filename);
	}
	return false;
}

bool HackingManager::CompareHackToolByName(const char* filename)
{
	static unsigned char code[32];

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
		return false;
	if (fseek(fp, 0, SEEK_END) != 0)
		return false;
	unsigned long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (size < 32)
		return false;

	fseek(fp, size / 2, SEEK_SET);
	fread(code, 1, 32, fp);
	fclose(fp);

	for (int i = 0; i < m_hackToolNum; ++i)
	{
		if (size == m_hackTool[i].size &&
			memcmp(m_hackTool[i].code, code, 32) == 0)
			return true;
	}
	return false;
}
