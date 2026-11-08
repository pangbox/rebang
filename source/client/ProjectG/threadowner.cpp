#include "threadowner.h"
#include <process.h>

namespace Py
{
	namespace Threading
	{
		unsigned int __stdcall ThreadOwner::ThreadProc(void* pParam)
		{
			ThreadOwner* pThis = (ThreadOwner*)pParam;
			HANDLE hFinishEvent = pThis->m_hFinishEvent;
			wchar_t msg[256];
			swprintf(msg,
				L"The Thread 'Win32 Thread' (%#x) be named '%s' was started.\n",
				GetCurrentThreadId(), pThis->m_name);

			OutputDebugStringW(msg);
			if (!pThis->OnThreadProc())
			{
				while (WaitForSingleObject(hFinishEvent, pThis->m_interval))
					pThis->OnUpdateThreadProc();
			}
			swprintf(msg,
				L"The Thread 'Win32 Thread' (%#x) be named '%s' was finished.\n",
				GetCurrentThreadId(), pThis->m_name);

			OutputDebugStringW(msg);
			CloseHandle(hFinishEvent);
			return 0;
		}

		ThreadOwner::ThreadOwner(const wchar_t* name)
			: m_interval(0)
		{
			SetThreadName(name);
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this,
				CREATE_SUSPENDED, NULL);
			m_hFinishEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
		}

		ThreadOwner::~ThreadOwner()
		{
			FinishThread();
			if (m_hThread)
			{
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}

		void ThreadOwner::SetThreadName(const wchar_t* name)
		{
			if (name)
				wcsncpy(m_name, name, 31);
			else
				wcscpy(m_name, L"Unnamed");
		}

		bool ThreadOwner::StartThread(unsigned long interval)
		{
			m_interval = interval;
			return ::ResumeThread(m_hThread) != -1;
		}

		bool ThreadOwner::PauseThread()
		{
			return SuspendThread(m_hThread) != -1;
		}

		bool ThreadOwner::ResumeThread()
		{
			return ::ResumeThread(m_hThread) != -1;
		}

		void ThreadOwner::FinishEvent()
		{
			if (m_hFinishEvent)
			{
				SetEvent(m_hFinishEvent);
				m_hFinishEvent = NULL;
			}
		}

		void ThreadOwner::TerminateThread()
		{
			::TerminateThread(m_hThread, 4);

			CloseHandle(m_hFinishEvent);
			m_hFinishEvent = NULL;
		}

		void ThreadOwner::FinishThread()
		{
			if (m_hFinishEvent)
			{
				SetEvent(m_hFinishEvent);
				m_hFinishEvent = NULL;

				if (WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
				{
					::TerminateThread(m_hThread, 6);
				}
			}
		}
	}
}
