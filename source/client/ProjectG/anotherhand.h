#pragma once

class CHandOwner
{
	friend unsigned int __stdcall AnotherHand_ThreadProc(void* param);

private:
	virtual void OnAnotherHand(int key, void* param) = 0;
};

#define MAX_ANOTHERHAND_WORK 16

class CAnotherHand
{
	friend unsigned int __stdcall AnotherHand_ThreadProc(void* param);

public:
	CAnotherHand();
	virtual ~CAnotherHand();

	bool Work(CHandOwner* owner, int key, void* param, bool bInterrupt,
		int priority);
	bool WaitUntilWorkDone(unsigned long timeout);
	bool IsWorkDone();
	bool IsWorking(int key);
	void Enter();
	void Leave();
	unsigned long GetStartTime();
	void BreakWorkingThread();
	unsigned long GetThreadID() const { return m_threadID; }

	bool m_bUserLock;
	bool m_bQueseLock;

protected:
	void ResetQueue();

	HANDLE m_handle;
	unsigned long m_threadID;
	int m_curWork;
	int m_vacation;
	CRITICAL_SECTION m_csQueue;
	CRITICAL_SECTION m_csUser;

public:
	struct WorkItem
	{
		CHandOwner* owner;
		int key;
		void* param;
		int priority;
		unsigned long startTime;
	};

protected:
	WorkItem m_aWork[MAX_ANOTHERHAND_WORK];
};

#include "anotherhand.inl"
