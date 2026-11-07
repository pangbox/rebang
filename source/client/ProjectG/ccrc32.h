#pragma once

#include <vector>

class CRC_32
{
public:
	CRC_32();

	unsigned long CalcCRC(void* data, unsigned int size, HWND hWnd);
	unsigned long CalcCRC(const char* filename, HWND hWnd);
	void Calculate(unsigned char* const data, unsigned int size,
		unsigned long& crc, unsigned long sleepTime);

private:
	unsigned long Reflect(unsigned long ref, char ch);
	static unsigned long __stdcall CRC32ThreadProc(void* param);

	unsigned long m_table[256];
};

class CRC_MT
{
public:
	struct sCRC_CHKTASK
	{
		char filename[260];
		unsigned long crc;
	};

	CRC_MT();
	~CRC_MT();

	int Begin(HWND hWnd);
	void Pause();
	void Resume();
	void Reset();
	int AddWork(const char* filename, unsigned long crc);
	void GetWork(std::vector<sCRC_CHKTASK*>& work);
	int IsTerminate() { return m_bTerminate; }
	int IsWorking() { return !m_work.empty(); }
	HWND GetParentWnd() { return m_hParentWnd; }
	void SetSleepTime(unsigned long time) { m_dwSleepTime = time; }
	unsigned long GetSleepTime() { return m_dwSleepTime; }

private:
	void Terminate();

	HWND m_hParentWnd;
	HANDLE m_hThread;
	int m_bPause;
	int m_bTerminate;
	unsigned long m_dwSleepTime;
	std::vector<sCRC_CHKTASK*> m_work;
};
