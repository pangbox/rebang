#pragma once

#include <windows.h>

class WSyncEvent
{
public:
	WSyncEvent()
		: m_handle(NULL), m_count(0)
	{
	}
	~WSyncEvent()
	{
		if (m_handle)
			CloseHandle(m_handle);
	}

private:
	HANDLE m_handle;
	int m_count;
};
