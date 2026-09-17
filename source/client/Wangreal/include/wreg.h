#pragma once

#include <windows.h>

class WRegistry
{
	struct w_reg_pair
	{
		HKEY key;
		char* name;
	};

	HKEY m_root;
	char m_location[128];
	int m_code;

public:
	WRegistry();
	~WRegistry();

	void SetLocation(const char* loc);
	bool __cdecl Read(char* field, char* buffer, int len);
	int __cdecl Scan(char* field, char* fmt, ...);
	void __cdecl Write(char* field, const char* format, ...);
	void Write(char* field, DWORD value);
};
