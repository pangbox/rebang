#include "wlock.h"

WLock::WLock()
	: m_threadID(-1), m_locked(false)
{
	InitializeCriticalSection(&m_criticalSection);
}

WLock::~WLock()
{
	DeleteCriticalSection(&m_criticalSection);
}

void WLock::Lock()
{
	ULONG threadID = GetCurrentThreadId();
	if (threadID == m_threadID)
	{
		*(int*)0xDEAD20CC = 0;
	}

	EnterCriticalSection(&m_criticalSection);
	m_threadID = threadID;
	m_locked = true;
}

void WLock::Unlock()
{
	if (m_locked)
	{
		m_threadID = -1;
		m_locked = false;
		LeaveCriticalSection(&m_criticalSection);
	}
}

void WLock::UnlockInThread(ULONG threadID)
{
	if (threadID == m_threadID && m_locked)
	{
		m_threadID = -1;
		m_locked = false;
		LeaveCriticalSection(&m_criticalSection);
	}
}
