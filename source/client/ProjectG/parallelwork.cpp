#include "parallelwork.h"

namespace Py
{
	namespace Threading
	{
		ParallelWork::ParallelWork(ParallelWorkPolicy::Enum policy)
			: m_lockCount(0), m_workingFirst(1), m_policy(policy)
		{
		}

		ParallelWork::~ParallelWork()
		{
			if (m_policy == ParallelWorkPolicy::CancelPending)
				InterlockedExchange(&m_workingFirst, 0);
			while (m_lockCount > 0)
				Sleep(0);
		}

		bool ParallelWork::IsWorkingFirst() const
		{
			return m_workingFirst != 0;
		}

		long ParallelWork::IncrementLockCount()
		{
			return InterlockedIncrement(&m_lockCount);
		}

		long ParallelWork::DecrementLockCount()
		{
			return InterlockedDecrement(&m_lockCount);
		}
	}
}
