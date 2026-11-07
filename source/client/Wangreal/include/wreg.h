#pragma once

#include <windows.h>

class WRegistry
{
public:
	WRegistry();
	~WRegistry();

	void SetLocation(const char* loc);
	void PassCode(int code);
	bool __cdecl Read(char* field, char* buffer, int len);
	int __cdecl Scan(char* field, char* fmt, ...);
	void __cdecl Write(char* field, const char* format, ...);
	void Write(char* field, DWORD value);

private:
	HKEY m_root;
	char m_location[128];
	int m_code;
};
