#include <process.h>
#include <time.h>
#include <mmsystem.h>
#include "anotherhand.h"

#define LOCK_QUEUE(hand) \
	EnterCriticalSection(&(hand)->m_csQueue); \
	(hand)->m_bQueseLock = true
#define UNLOCK_QUEUE(hand) \
	(hand)->m_bQueseLock = false; \
	LeaveCriticalSection(&(hand)->m_csQueue)

CAnotherHand::CAnotherHand()
{
	m_handle = NULL;
	m_bUserLock = false;
	m_bQueseLock = false;
	ResetQueue();
	m_threadID = (unsigned long)-1;
	InitializeCriticalSection(&m_csQueue);
	InitializeCriticalSection(&m_csUser);
}

CAnotherHand::~CAnotherHand()
{
	if (m_handle)
	{
		WaitUntilWorkDone(9999);

		CloseHandle(m_handle);
		m_handle = NULL;
	}

	if (m_bUserLock)
	{
		Leave();
	}

	if (m_bQueseLock)
	{
		UNLOCK_QUEUE(this);
	}

	DeleteCriticalSection(&m_csQueue);
	DeleteCriticalSection(&m_csUser);
}

bool CAnotherHand::Work(CHandOwner* owner, int key, void* param,
	bool bInterrupt, int priority)
{
	LOCK_QUEUE(this);

	int index;
	if (!bInterrupt || m_curWork == m_vacation)
		index = m_vacation;
	else
	{
		index = (m_curWork + 1) % MAX_ANOTHERHAND_WORK;
		if (index == m_curWork)
		{
			UNLOCK_QUEUE(this);

			return false;
		}
	}

	int next = (index + 1) % MAX_ANOTHERHAND_WORK;
	if (next == m_curWork)
	{
		UNLOCK_QUEUE(this);

		return false;
	}
	m_vacation = next;

	WorkItem& work = m_aWork[index];
	work.owner = owner;
	work.key = key;
	work.param = param;
	work.priority = priority;
	work.startTime = timeGetTime();

	if (m_handle == NULL)
	{
		m_handle = (HANDLE)_beginthreadex(NULL, 0, AnotherHand_ThreadProc, this,
			0, (unsigned int*)&m_threadID);
		if (m_handle == NULL)
		{
			UNLOCK_QUEUE(this);
			m_threadID = (unsigned long)-1;

			return false;
		}
	}

	UNLOCK_QUEUE(this);

	return true;
}

bool CAnotherHand::WaitUntilWorkDone(unsigned long timeout)
{
	if (m_handle)
		return WaitForSingleObject(m_handle, timeout) == WAIT_TIMEOUT;

	return false;
}

unsigned int __stdcall AnotherHand_ThreadProc(void* param)
{
	srand((unsigned int)time(NULL));

	CAnotherHand* hand = (CAnotherHand*)param;

	while (true)
	{
		CAnotherHand::WorkItem& work = hand->m_aWork[hand->m_curWork];

		if (GetThreadPriority(hand->m_handle) != work.priority)
			SetThreadPriority(hand->m_handle, work.priority);

		work.owner->OnAnotherHand(work.key, work.param);

		LOCK_QUEUE(hand);

		hand->m_curWork = (hand->m_curWork + 1) % MAX_ANOTHERHAND_WORK;
		if (hand->IsWorkDone())
		{
			CloseHandle(hand->m_handle);
			hand->m_handle = NULL;

			UNLOCK_QUEUE(hand);
			return 0;
		}

		UNLOCK_QUEUE(hand);
	}
}

void CAnotherHand::BreakWorkingThread()
{
	if (m_bUserLock)
	{
		Leave();
	}

	if (m_bQueseLock)
	{
		UNLOCK_QUEUE(this);
	}

	if (m_handle)
	{
		DWORD exitCode;
		GetExitCodeThread(m_handle, &exitCode);
		TerminateThread(m_handle, exitCode);
		m_handle = NULL;
	}

	ResetQueue();
}

void CAnotherHand::ResetQueue()
{
	m_curWork = 0;
	m_vacation = 0;
}
