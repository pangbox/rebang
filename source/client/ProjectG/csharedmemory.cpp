#include "csharedmemory.h"

cIPCMemServer::cIPCMemServer()
	: m_hMapFile(NULL)
{
}

cIPCMemServer::~cIPCMemServer()
{
	ShutDown();
}

bool cIPCMemServer::Create(const char* szName)
{
	m_hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		0, IPC_MEMORY_SIZE, szName);
	if (m_hMapFile == NULL)
	{
		return false;
	}

	return true;
}

bool cIPCMemServer::Send(void* pData, int nSize, int nOffset)
{
	if (m_hMapFile == NULL)
		return false;

	if (nSize > IPC_MEMORY_SIZE)
		return false;

	void* pView = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (pView == NULL)
		return false;

	memcpy(pView, pData, nSize);

	return true;
}

bool cIPCMemServer::Recv(void* pData, int nSize, int nOffset)
{
	if (m_hMapFile == NULL)
		return false;

	if (nSize > IPC_MEMORY_SIZE)
		return false;

	void* pView = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (pView == NULL)
		return false;

	memcpy(pData, pView, nSize);

	return true;
}

void cIPCMemServer::ShutDown()
{
	if (m_hMapFile)
		CloseHandle(m_hMapFile);
}

cIPCMemClient::cIPCMemClient()
	: m_hMapFile(NULL)
{
}

cIPCMemClient::~cIPCMemClient()
{
	Close();
}

bool cIPCMemClient::Connection(const char* szName)
{
	m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szName);

	if (m_hMapFile == NULL)
	{
		return false;
	}

	return true;
}

bool cIPCMemClient::Send(void* pData, int nSize)
{
	if (m_hMapFile == NULL)
		return false;

	if (nSize > IPC_MEMORY_SIZE)
		return false;

	void* pView = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (pView == NULL)
		return false;

	memcpy(pView, pData, nSize);

	return true;
}

bool cIPCMemClient::Recv(void* pData, int nSize)
{
	if (m_hMapFile == NULL)
		return false;

	void* pView = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (pView == NULL)
		return false;

	memcpy(pData, pView, nSize);

	return true;
}

void cIPCMemClient::Close()
{
	if (m_hMapFile)
		CloseHandle(m_hMapFile);
}
