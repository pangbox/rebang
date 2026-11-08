#include "ccrc32.h"
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <share.h>

struct CRC32Task
{
	CRC_32* calculator;
	char filename[MAX_PATH];
	unsigned char* data;
	unsigned int size;
	HWND window;
	HANDLE thread;
};

unsigned long __stdcall CRC_32::CRC32ThreadProc(void* param)
{
	CRC32Task* task = (CRC32Task*)param;
	unsigned long crc = 0xffffffff;
	HWND window = NULL;
	if (IsWindow(task->window))
	{
		window = task->window;
		PostMessage(window, PBM_SETPOS, 0, 0);
		PostMessage(window, PBM_SETRANGE32, 0, 100);
	}
	const unsigned int blockSize = 102400;
	unsigned char buffer[blockSize];
	if (task->data)
	{
		for (unsigned int pos = 0; pos < task->size; pos += blockSize)
		{
			unsigned int count = task->size - pos;
			if (count > blockSize)
				count = blockSize;
			task->calculator->Calculate(task->data + pos, count, crc, 0);
			if (IsWindow(window))
				PostMessage(window, PBM_SETPOS,
					pos > task->size
						? 100
						: (unsigned int)(100.0 * ((double)pos / task->size)),
					0);
		}
	}
	else if (task->filename)
	{
		__int64 total = 0;
		int file = _sopen(task->filename, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
			_SH_DENYWR);
		if (file != -1)
		{
			_lseeki64(file, 0, SEEK_SET);
			__int64 size = _lseeki64(file, 0, SEEK_END);
			_lseeki64(file, 0, SEEK_SET);
			unsigned int count;
			do
			{
				count = _read(file, buffer, blockSize);
				if (count)
				{
					task->calculator->Calculate(buffer, count, crc, 0);
					if (IsWindow(window))
					{
						total += count;
						PostMessage(window, PBM_SETPOS,
							(unsigned int)((double)total / size * 100), 0);
					}
				}
			} while (count == blockSize);
			_close(file);
		}
	}
	crc ^= 0xffffffff;
	if (IsWindow(window))
		PostMessage(GetParent(window), 0xADB1, (WPARAM)task->thread, crc);
	delete task->data;
	delete task;
	return crc;
}

CRC_32::CRC_32()
{
	const unsigned long polynomial = 0xEDB88320;
	for (int i = 0; i <= 255; i++)
	{
		m_table[i] = Reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			m_table[i] =
				(m_table[i] << 1) ^ (m_table[i] & (1 << 31) ? polynomial : 0);
		m_table[i] = Reflect(m_table[i], 32);
	}
}

// TODO: workaround for codegen mismatch
#pragma optimize("", off)
unsigned long CRC_32::Reflect(unsigned long ref, char ch)
{
	unsigned long value = 0;
	for (int i = 1; i < ch + 1; i++)
	{
		if (ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}
#pragma optimize("", on)

void CRC_32::Calculate(unsigned char* const data, unsigned int size,
	unsigned long& crc, unsigned long sleepTime)
{
	unsigned char* buffer = data;
	int remaining = 128;
	while (size--)
	{
		crc = (crc >> 8) ^ m_table[(crc & 0xff) ^ *buffer++];
		if (sleepTime)
		{
			if (!--remaining)
			{
				remaining = 128;
				Sleep(sleepTime);
			}
		}
	}
}

unsigned long CRC_32::CalcCRC(void* data, unsigned int size, HWND hWnd)
{
	if (!data || !size)
		return 0;
	if (!IsWindow(hWnd))
	{
		unsigned long crc = 0xffffffff;
		Calculate((unsigned char*)data, size, crc, 0);
		return crc ^ 0xffffffff;
	}

	CRC32Task* task = new CRC32Task;
	DWORD threadId;
	HANDLE thread = CreateThread(NULL, 0, CRC32ThreadProc, task,
		CREATE_SUSPENDED, &threadId);
	if (thread)
	{
		task->calculator = this;
		task->filename[0] = 0;
		task->data = new unsigned char[size];
		memcpy(task->data, data, size);
		task->size = size;
		task->window = hWnd;
		task->thread = thread;
		ResumeThread(thread);
	}
	else
		delete task;
	return (unsigned long)thread;
}

unsigned long CRC_32::CalcCRC(const char* filename, HWND hWnd)
{
	DWORD attributes = GetFileAttributes(filename);
	if (attributes == INVALID_FILE_ATTRIBUTES ||
		attributes & FILE_ATTRIBUTE_DIRECTORY)
		return 0;
	CRC32Task* task = new CRC32Task;
	task->calculator = this;
	_tcsncpy(task->filename, filename, MAX_PATH);
	task->data = NULL;
	task->size = 0;
	task->window = hWnd;
	task->thread = NULL;
	if (!IsWindow(hWnd))
		return CRC32ThreadProc(task);

	DWORD threadId;
	HANDLE thread = CreateThread(NULL, 0, CRC32ThreadProc, task,
		CREATE_SUSPENDED, &threadId);
	if (thread)
	{
		task->thread = thread;
		ResumeThread(thread);
	}
	else
		delete task;
	return (unsigned long)thread;
}

namespace
{
	struct _delete
	{
		template <typename T>
		void operator()(const T* p) const
		{
			delete p;
		}
	};
}

CRC_MT::CRC_MT()
	: m_hParentWnd(NULL),
	  m_hThread(NULL),
	  m_bPause(FALSE),
	  m_bTerminate(FALSE),
	  m_dwSleepTime(0)
{
	m_work.reserve(100);
}

CRC_MT::~CRC_MT()
{
	Terminate();
	Reset();
}

void CRC_MT::GetWork(std::vector<sCRC_CHKTASK*>& work)
{
	work.assign(m_work.begin(), m_work.end());
}

int CRC_MT::AddWork(const char* filename, unsigned long crc)
{
	if (m_hThread)
		return FALSE;
	sCRC_CHKTASK* task = new sCRC_CHKTASK;
	_tcsncpy(task->filename, filename, MAX_PATH);
	task->crc = crc;
	m_work.push_back(task);
	return TRUE;
}

__declspec(noinline) void CRC_MT::Reset()
{
	m_hThread = NULL;
	std::for_each(m_work.begin(), m_work.end(), _delete());
	m_work.clear();
}

void CRC_MT::Terminate()
{
	m_bTerminate = TRUE;
	if (m_bPause)
		Resume();
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
	}
}

unsigned int __stdcall ThreadCRCCheckProc(void* param)
{
	CRC_MT* worker = (CRC_MT*)param;
	std::vector<CRC_MT::sCRC_CHKTASK*> work;
	worker->GetWork(work);
	DWORD start = GetTickCount();
	unsigned long sleepTime = worker->GetSleepTime();
	CRC_32 calculator;
	unsigned char buffer[102400];
	for (std::vector<CRC_MT::sCRC_CHKTASK*>::iterator it = work.begin();
		it != work.end(); ++it)
	{
		CRC_MT::sCRC_CHKTASK* task = *it;
		unsigned long crc = 0xffffffff;
		int file = _sopen(task->filename, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
			_SH_DENYWR);
		if (file != -1)
		{
			_lseeki64(file, 0, SEEK_SET);
			__int64 size = _lseeki64(file, 0, SEEK_END);
			_lseeki64(file, 0, SEEK_SET);
			unsigned int count;
			do
			{
				count = _read(file, buffer, sizeof(buffer));
				if (count)
					calculator.Calculate(buffer, count, crc, 0);
				if (worker->IsTerminate())
				{
					_close(file);
					return 0;
				}
			} while (count == sizeof(buffer));
			_close(file);
		}
		crc ^= 0xffffffff;
		if (task->crc != crc && IsWindow(worker->GetParentWnd()))
		{
			PostMessage(worker->GetParentWnd(), 0xADB2, 0, (LPARAM)task);
			return 0;
		}
	}
	if (IsWindow(worker->GetParentWnd()))
		PostMessage(worker->GetParentWnd(), 0xADB1, GetTickCount() - start, 0);
	worker->Reset();
	return 0;
}

int CRC_MT::Begin(HWND hWnd)
{
	if (m_hThread)
		return FALSE;
	m_hParentWnd = hWnd;
	m_hThread =
		(HANDLE)_beginthreadex(NULL, 0, ThreadCRCCheckProc, this, 0, NULL);
	return m_hThread != NULL;
}

void CRC_MT::Pause()
{
	if (m_hThread && !m_bPause)
	{
		SuspendThread(m_hThread);
		m_bPause = TRUE;
	}
}

void CRC_MT::Resume()
{
	if (m_hThread && m_bPause)
	{
		ResumeThread(m_hThread);
		m_bPause = FALSE;
	}
}
