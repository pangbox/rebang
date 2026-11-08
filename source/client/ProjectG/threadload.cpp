#include "threadload.h"
#include <process.h>
#include <mmsystem.h>

sParam g_loadParam = { 0 };

CThreadLoad::CThreadLoad()
	: m_hThread(NULL)
{
	g_loadParam.result = NULL;
	g_loadParam.bComplete = true;
}

CThreadLoad::~CThreadLoad()
{
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

unsigned int __stdcall ThreadProc(void* pParam)
{
	sParam* p = (sParam*)pParam;

	switch (p->type)
	{
	case LOAD_PUPPET:
		p->result = g_resrcmng->GetPuppet(p->filename, false, true, false);
		break;

	case LOAD_TEXTURE:
		p->result = (void*)g_resrcmng->LoadTexture(p->filename, 0, 0, NULL);
		break;

	case LOAD_OVERLAY:
		p->result = g_resrcmng->GetOverlay(p->filename, 0);
		break;
	default:
		break;
	}

	g_loadParam.bComplete = true;

	return 0;
}

void CThreadLoad::LoadPuppet(const char* filename)
{
	Load(filename, LOAD_PUPPET);
}

void CThreadLoad::LoadTexture(const char* filename)
{
	Load(filename, LOAD_TEXTURE);
}

void CThreadLoad::LoadOverlay(const char* filename)
{
	Load(filename, LOAD_OVERLAY);
}

void CThreadLoad::Load(const char* filename, int type)
{
	strcpy(g_loadParam.filename, filename);
	g_loadParam.result = NULL;
	g_loadParam.type = type;
	g_loadParam.bComplete = false;
	if (g_loadParam.bComplete)
		StopLoading();
	g_loadParam.startTime = timeGetTime();

	unsigned int threadId;
	m_hThread =
		(HANDLE)_beginthreadex(NULL, 0, ThreadProc, &g_loadParam, 0, &threadId);

	if (m_hThread == NULL)
	{
		MessageBoxA(NULL, "_beginthreadex() Failed", "\xb0\xe6\xb0\xed", MB_OK);
		return;
	}

	SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
}

void CThreadLoad::StopLoading()
{
	DWORD exitCode;
	if (m_hThread)
	{
		GetExitCodeThread(m_hThread, &exitCode);
		ExitThread(exitCode);
	}
}
