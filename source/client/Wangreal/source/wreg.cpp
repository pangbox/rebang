#include "wreg.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

WRegistry::~WRegistry()
{
}

void WRegistry::SetLocation(const char* loc)
{
	static w_reg_pair list[] = {
		{ HKEY_CURRENT_USER,   "HKEY_CURRENT_USER"   },
		{ HKEY_CLASSES_ROOT,   "HKEY_CLASSES_ROOT"   },
		{ HKEY_LOCAL_MACHINE,  "HKEY_LOCAL_MACHINE"  },
		{ HKEY_USERS,          "HKEY_USERS"          },
		{ HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG" }
	};

	char* pos;
	unsigned int i;
	for (i = 0; i < 5; ++i)
	{
		pos = strstr(loc, list[i].name);
		if (pos)
		{
			m_root = list[i].key;
			strcpy(m_location, strchr(pos, '\\') + 1);
			if (m_location[strlen(m_location) - 1] == '\\')
				m_location[strlen(m_location) - 1] = 0;
			return;
		}
	}

	strcpy(m_location, loc);
	if (m_location[strlen(m_location) - 1] == '\\')
		m_location[strlen(m_location) - 1] = 0;
}

bool __cdecl WRegistry::Read(char* field, char* buffer, int len)
{
	char subkey[128];
	bool ret = false;
	DWORD datalen;
	DWORD type;
	HKEY hkey;

	if (strchr(field, '\\'))
	{
		sprintf(subkey, "%s\\%s", m_location, field);
		*strrchr(subkey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	}
	else
	{
		strcpy(subkey, m_location);
	}

	if (RegOpenKeyEx(m_root, subkey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		datalen = len;
		if (RegQueryValueEx(hkey, field, NULL, &type,
				reinterpret_cast<BYTE*>(buffer), &datalen) == ERROR_SUCCESS)
		{
			ret = true;
		}
		RegCloseKey(hkey);
	}

	return ret;
}

int __cdecl WRegistry::Scan(char* field, char* fmt, ...)
{
	char text[512] = { 0 };
	char temp[128] = { 0 };
	int c = 0;
	int j;
	int i;
	float code2;
	va_list marker;

	if (Read(field, text, sizeof(text)) == true)
	{
		va_start(marker, fmt);

		for (i = 0, j = 0; fmt[i]; ++i)
		{
			if (fmt[i] == '%')
			{
				while (text[j] && static_cast<unsigned char>(text[j]) < 32)
				{
					++j;
				}
				unsigned int len = 0;
				while (text[j + len] &&
					static_cast<unsigned char>(text[j + len]) >= 32 &&
					len < 128)
				{
					++len;
				}
				if (len == 0)
				{
					break;
				}

				memcpy(temp, text + j, len);
				++c;
				j += len;
				temp[len] = 0;

				switch (fmt[i + 1])
				{
				case 'x':
				case 'X':
				{
					int value = 0;
					int pos = (temp[0] == '-');
					if (temp[pos])
					{
						while (temp[pos])
						{
							int code = temp[pos];
							if (code >= 'A')
								code = code - 'A' + 10;
							else
								code = code - (code <= '9' ? '0' : 'a' - 10);
							if (code < 0 && code >= 16)
								break;
							if (temp[0] == '-')
								code = -code;
							value = value * 16 + code;
							++pos;
						}
					}
					*va_arg(marker, int*) = value;
				}
				break;
				case 'f':
				case 'F':
				case 'g':
				case 'G':
					code2 = static_cast<float>(atof(temp));
					*va_arg(marker, float*) = code2;
					break;
				case 'd':
				case 'D':
				case 'u':
				case 'U':
					*va_arg(marker, int*) = atoi(temp);
					break;
				case 's':
				case 'S':
					strcpy(va_arg(marker, char*), temp);
					break;
				}
			}
		}
	}

	return c;
}

void __cdecl WRegistry::Write(char* field, const char* format, ...)
{
	char subkey[128];
	char buffer[1024];
	HKEY hkey;
	va_list marker;
	va_start(marker, format);
	_vsnprintf(buffer, sizeof(buffer), format, marker);

	if (strchr(field, '\\'))
	{
		sprintf(subkey, "%s\\%s", m_location, field);
		*strrchr(subkey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	}
	else
	{
		strcpy(subkey, m_location);
	}

	if (RegOpenKeyEx(m_root, subkey, 0, KEY_WRITE, &hkey) != ERROR_SUCCESS)
		RegCreateKey(m_root, subkey, &hkey);

	RegSetValueEx(hkey, field, 0, REG_SZ, reinterpret_cast<BYTE*>(buffer),
		strlen(buffer));
	RegCloseKey(hkey);
}

void WRegistry::Write(char* field, DWORD value)
{
	char subkey[128];
	HKEY hkey;

	if (strchr(field, '\\'))
	{
		sprintf(subkey, "%s\\%s", m_location, field);
		*strrchr(subkey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	}
	else
	{
		strcpy(subkey, m_location);
	}

	if (RegOpenKeyEx(m_root, subkey, 0, KEY_WRITE, &hkey) != ERROR_SUCCESS)
	{
		RegCreateKey(m_root, subkey, &hkey);
	}

	RegSetValueEx(hkey, field, 0, REG_DWORD, reinterpret_cast<BYTE*>(&value),
		sizeof(value));
	RegCloseKey(hkey);
}

WRegistry::WRegistry()
{
	SetLocation("HKEY_CURRENT_USER\\Software\\Ntreev\\PangYa");
}
