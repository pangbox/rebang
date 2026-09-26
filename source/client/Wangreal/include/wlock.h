#pragma once

#include <windows.h>

class WLock
{
public:
	WLock();
	~WLock();

	void Lock();
	void Unlock();
	void UnlockInThread(ULONG threadID);

private:
	ULONG m_threadID;
	CRITICAL_SECTION m_criticalSection;
	bool m_locked;
};

class WInstanceLock
{
public:
	WInstanceLock(WLock* lock)
	{
		m_lock = lock;
		m_lock->Lock();
	}
	~WInstanceLock() { m_lock->Unlock(); }

private:
	WLock* m_lock;
};
