#include "threadpool.h"
#include <queue>
#include "concurrentqueue.h"

namespace Py
{
	namespace Threading
	{
		struct ThreadProcContext
		{
			ThreadPoolImpl* implementation;
			HANDLE stopEvent;
		};

		struct InvokedWork
		{
			_OVERLAPPED overlapped;
			ParallelWork* work;
			void (ParallelWork::*callback)();
		};

		class ThreadPoolImpl
		{
		protected:
			virtual void OnInvokeParallelWork(InvokedWork* invokedWork) = 0;
			virtual unsigned int OnThreadProc(void* context) = 0;
			virtual const char* GetThreadPoolName() const = 0;

		public:
			static unsigned int __stdcall ThreadProc(void* context)
			{
				ThreadProcContext* procContext = (ThreadProcContext*)context;
				ThreadPoolImpl* implementation = procContext->implementation;
				HANDLE stopEvent = procContext->stopEvent;
				delete procContext;
				return implementation->OnThreadProc(stopEvent);
			}

			ThreadPoolImpl()
				: m_threadCount(0), m_invokedWorkCount(0)
			{
			}

			virtual ~ThreadPoolImpl() { }

			unsigned int GetThreadCount() const { return m_threadCount; }

			unsigned int GetInvokedWorkCount() const
			{
				return m_invokedWorkCount;
			}

			void InvokeParallelWork(ParallelWork* work,
				void (ParallelWork::*callback)())
			{
				InvokedWork* invokedWork = new InvokedWork();
				memset(invokedWork, 0, sizeof(InvokedWork));
				invokedWork->work = work;
				invokedWork->callback = callback;
				OnInvokeParallelWork(invokedWork);
			}

			int WaitForCompletionAll(unsigned long timeout)
			{
				unsigned long started = GetTickCount();
				while (GetInvokedWorkCount() > 0)
				{
					if (GetTickCount() - started > timeout)
						return 0;
					Sleep(0);
				}
				return 1;
			}

			int SetThreadPriority(ThreadPriority::Enum priority)
			{
				if (m_threads.empty())
					return 0;
				for (std::vector<HANDLE>::iterator thread = m_threads.begin();
					thread != m_threads.end(); ++thread)
				{
					if (!::SetThreadPriority(*thread, priority))
						return 0;
				}
				return 1;
			}

			ThreadPriority::Enum GetThreadPriority() const
			{
				if (m_threads.empty())
					return ThreadPriority::Normal;
				HANDLE thread = *m_threads.begin();
				return (ThreadPriority::Enum)::GetThreadPriority(thread);
			}

			virtual int Initialize(int threadCount)
			{
				if (threadCount == 0)
				{
					SYSTEM_INFO systemInfo;
					GetSystemInfo(&systemInfo);
					threadCount = systemInfo.dwNumberOfProcessors;
				}
				m_threadCount = threadCount;
				m_stopEvents.reserve(m_threadCount);
				m_threads.reserve(m_threadCount);
				for (unsigned int index = 0; index < m_threadCount; ++index)
				{
					HANDLE stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					HANDLE thread = NULL;
					if (!stopEvent)
						goto failed;
					ThreadProcContext* context = new ThreadProcContext;
					context->stopEvent = stopEvent;
					context->implementation = this;
					thread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
						context, 0, NULL);
					if (!thread)
						goto failed;
					m_stopEvents.push_back(stopEvent);
					m_threads.push_back(thread);
				}
				return 1;
failed:
				Release();
				return 0;
			}

			virtual void Release()
			{
				if (m_threads.empty())
					return;
				std::vector<HANDLE>::iterator end = m_stopEvents.end();
				for (std::vector<HANDLE>::iterator it = m_stopEvents.begin();
					it != end; ++it)
					SetEvent(*it);
				if (WaitForMultipleObjects(m_threads.size(), &m_threads[0],
						TRUE, 10000) == WAIT_TIMEOUT)
				{
					end = m_threads.end();
					for (std::vector<HANDLE>::iterator it = m_threads.begin();
						it != end; ++it)
						TerminateThread(*it, 0xffffffff);
				}
				end = m_stopEvents.end();
				for (std::vector<HANDLE>::iterator it = m_stopEvents.begin();
					it != end; ++it)
					CloseHandle(*it);
				end = m_threads.end();
				for (std::vector<HANDLE>::iterator it = m_threads.begin();
					it != end; ++it)
					CloseHandle(*it);
				m_stopEvents.clear();
				m_threads.clear();
			}

		protected:
			std::vector<HANDLE> m_stopEvents;
			std::vector<HANDLE> m_threads;
			unsigned int m_threadCount;
			volatile long m_invokedWorkCount;
		};

		class ConcurrentQueueThreadPool : public ThreadPoolImpl
		{
		public:
			ConcurrentQueueThreadPool() { }

		protected:
			virtual const char* GetThreadPoolName() const
			{
				return "ConcurrentQueueThreadPool";
			}

			virtual unsigned int OnThreadProc(void* context)
			{
				while (WaitForSingleObject((HANDLE)context, 1) != WAIT_OBJECT_0)
				{
					while (!m_queue.Empty())
					{
						InvokedWork* invokedWork = 0;
						if (m_queue.Pop(invokedWork) && invokedWork != 0)
						{
							ParallelWork* work = invokedWork->work;
							if (work->IsWorkingFirst())
								(work->*invokedWork->callback)();
							InvokedWork* completedWork = invokedWork;
							InterlockedDecrement(&m_invokedWorkCount);
							completedWork->work->DecrementLockCount();
							delete invokedWork;
						}
					}
				}
				return 0;
			}

			virtual void OnInvokeParallelWork(InvokedWork* invokedWork)
			{
				invokedWork->work->IncrementLockCount();
				InterlockedIncrement(&m_invokedWorkCount);
				m_queue.Push(invokedWork);
			}

		private:
			ConcurrentQueue<InvokedWork*, std::deque<InvokedWork*> > m_queue;
		};

		class IocpPool : public ThreadPoolImpl
		{
		public:
			IocpPool() { }

			virtual int Initialize(int threadCount)
			{
				m_kernel32 = LoadLibrary("kernel32.dll");
				if (!m_kernel32)
					return 0;
				m_CreateIoCompletionPort =
					(CreateIoCompletionPortFunc)GetProcAddress(m_kernel32,
						"CreateIoCompletionPort");
				m_GetQueuedCompletionStatus =
					(GetQueuedCompletionStatusFunc)GetProcAddress(m_kernel32,
						"GetQueuedCompletionStatus");
				m_PostQueuedCompletionStatus =
					(PostQueuedCompletionStatusFunc)GetProcAddress(m_kernel32,
						"PostQueuedCompletionStatus");
				if (!m_CreateIoCompletionPort || !m_GetQueuedCompletionStatus ||
					!m_PostQueuedCompletionStatus ||
					!ThreadPoolImpl::Initialize(threadCount))
					return 0;
				m_completionPort = m_CreateIoCompletionPort((HANDLE)-1, NULL, 0,
					m_threadCount);
				return 1;
			}

			virtual void Release()
			{
				if (m_completionPort)
				{
					for (unsigned int index = 0; index < m_threadCount; ++index)
						m_PostQueuedCompletionStatus(m_completionPort, 0, 1,
							NULL);
					ThreadPoolImpl::Release();
					CloseHandle(m_completionPort);
					FreeLibrary(m_kernel32);
					m_completionPort = NULL;
					m_kernel32 = NULL;
				}
			}

		protected:
			virtual const char* GetThreadPoolName() const { return "IocpPool"; }

			virtual unsigned int OnThreadProc(void* context)
			{
				for (;;)
				{
					unsigned long bytes = 0;
					unsigned long key = 0;
					_OVERLAPPED* overlapped;
					if (!m_GetQueuedCompletionStatus(m_completionPort, &bytes,
							&key, &overlapped, INFINITE) &&
						GetLastError() == ERROR_INVALID_HANDLE)
						break;
					if (key == 1)
						break;
					InvokedWork* invokedWork = (InvokedWork*)overlapped;
					if (key == 0 && invokedWork)
					{
						ParallelWork* work = invokedWork->work;
						if (work->IsWorkingFirst())
							(work->*invokedWork->callback)();
						InterlockedDecrement(&m_invokedWorkCount);
						invokedWork->work->DecrementLockCount();
						delete invokedWork;
					}
				}
				return 0;
			}

			virtual void OnInvokeParallelWork(InvokedWork* invokedWork)
			{
				invokedWork->work->IncrementLockCount();
				InterlockedIncrement(&m_invokedWorkCount);
				m_PostQueuedCompletionStatus(m_completionPort,
					sizeof(*invokedWork), 0, &invokedWork->overlapped);
			}

		private:
			typedef HANDLE(__stdcall* CreateIoCompletionPortFunc)(HANDLE,
				HANDLE, unsigned long, unsigned long);
			typedef int(__stdcall* GetQueuedCompletionStatusFunc)(HANDLE,
				unsigned long*, unsigned long*, _OVERLAPPED**, unsigned long);
			typedef int(__stdcall* PostQueuedCompletionStatusFunc)(HANDLE,
				unsigned long, unsigned long, _OVERLAPPED*);

			HANDLE m_completionPort;
			HINSTANCE m_kernel32;
			CreateIoCompletionPortFunc m_CreateIoCompletionPort;
			GetQueuedCompletionStatusFunc m_GetQueuedCompletionStatus;
			PostQueuedCompletionStatusFunc m_PostQueuedCompletionStatus;
		};

		class UnimplementPool : public ThreadPoolImpl
		{
		public:
			virtual int Initialize(int threadCount) { return 1; }

			virtual void Release() { }

		protected:
			virtual const char* GetThreadPoolName() const
			{
				return "UnimplementPool";
			}

			virtual unsigned int OnThreadProc(void* context) { return 0; }

			virtual void OnInvokeParallelWork(InvokedWork* invokedWork)
			{
				(invokedWork->work->*invokedWork->callback)();
				delete invokedWork;
			}
		};

		int __fastcall ThreadPool::SetProcessPriority(
			ProcessPriority::Enum priority)
		{
			return SetPriorityClass(GetCurrentProcess(), priority);
		}

		ProcessPriority::Enum __fastcall ThreadPool::GetProcessPriority()
		{
			return (ProcessPriority::Enum)GetPriorityClass(GetCurrentProcess());
		}

		ThreadPool::ThreadPool(ThreadPoolPolicy::Enum policy,
			unsigned int threadCount)
			: m_implementation(NULL)
		{
			switch (policy)
			{
			case ThreadPoolPolicy::MultiThread:
				m_implementation = new ConcurrentQueueThreadPool;
				m_implementation->Initialize(threadCount);
				break;
			case ThreadPoolPolicy::Iocp:
				m_implementation = new IocpPool;
				if (m_implementation->Initialize(threadCount))
					break;
				delete m_implementation;
				m_implementation = new ConcurrentQueueThreadPool;
				m_implementation->Initialize(1);
				break;
			case ThreadPoolPolicy::SingleThread:
				m_implementation = new ConcurrentQueueThreadPool;
				m_implementation->Initialize(1);
				break;
			case ThreadPoolPolicy::Unimplemented:
				m_implementation = new UnimplementPool;
				m_implementation->Initialize(0);
				break;
			}
		}

		ThreadPool::~ThreadPool()
		{
			m_implementation->Release();
			delete m_implementation;
		}

		unsigned int ThreadPool::GetThreadCount() const
		{
			return m_implementation->GetThreadCount();
		}

		unsigned int ThreadPool::GetInvokedWorkCount() const
		{
			return m_implementation->GetInvokedWorkCount();
		}

		void ThreadPool::InternalInvokeParallelWork(ParallelWork* work,
			void (ParallelWork::*callback)())
		{
			m_implementation->InvokeParallelWork(work, callback);
		}

		int ThreadPool::WaitForCompletionAll(unsigned long timeout)
		{
			return m_implementation->WaitForCompletionAll(timeout);
		}

		ThreadPriority::Enum ThreadPool::GetThreadPriority() const
		{
			return m_implementation->GetThreadPriority();
		}

		int ThreadPool::SetThreadPriority(ThreadPriority::Enum priority)
		{
			return m_implementation->SetThreadPriority(priority);
		}

	}
}
