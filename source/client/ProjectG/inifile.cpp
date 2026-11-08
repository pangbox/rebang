#include "wresrcmng.h"
#include "projectg.h"
#include "inifile.h"

static std::string m_FileName;

CIniFile::CIniFile()
{
}

CIniFile::CIniFile(const char* path)
{
	Create(path, M_READ);
}

CIniFile::~CIniFile()
{
}

bool CIniFile::Create(const char* path, IO_MODE mode)
{
	cFile* hdl = NULL;

	if (mode == M_READ)
	{
		hdl = g_resrcmng->GetCFile(path, -1);

		if (hdl == NULL)
			return false;
	}

	if (mode == M_WRITE)
	{
		char szPath[MAX_PATH];
		memcpy(szPath, path, strlen(path) + 1);

		m_FileName = g_executeDirectory;
		m_FileName += "\\";
		m_FileName += path;

		WritePrivateProfileString(NULL, NULL, NULL, m_FileName.c_str());
	}
	else
	{
		CloseCFile(hdl);

		m_FileName = g_executeDirectory;
		m_FileName += "\\";
		m_FileName += path;
	}

	return true;
}

const char* CIniFile::ReadString(const char* section_name, const char* key_name)
{
	static char Buffer[1024];

	if (m_FileName.length() > 0)
	{
		memset(Buffer, 0, sizeof(Buffer));
		GetPrivateProfileString(section_name, key_name, "OCCUR_ERROR", Buffer,
			sizeof(Buffer), m_FileName.c_str());

		if (strcmp(Buffer, "OCCUR_ERROR") != 0)
		{
			return Buffer;
		}
	}

	return "";
}

int CIniFile::ReadInteger(const char* section_name, const char* key_name)
{
	if (m_FileName.length() > 0)
	{
		int value = GetPrivateProfileInt(section_name, key_name, -3432,
			m_FileName.c_str());

		if (value != -3432)
		{
			return value;
		}
	}

	return -12302323;
}

int CIniFile::WriteString(const char* section_name, const char* key_name,
	const char* key_value)
{
	if (m_FileName.length() > 0)
	{
		return WritePrivateProfileString(section_name, key_name, key_value,
				   m_FileName.c_str())
			? 1
			: -12302323;
	}

	return -12302323;
}

int CIniFile::WriteInteger(const char* section_name, const char* key_name,
	int key_value)
{
	if (m_FileName.length() > 0)
	{
		char buffer[20] = {
			0,
		};
		itoa(key_value, buffer, 10);

		return WritePrivateProfileString(section_name, key_name, buffer,
				   m_FileName.c_str())
			? 1
			: -12302323;
	}

	return -12302323;
}
