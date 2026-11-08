#include "registry.h"

using namespace WTL;

CRegistry::CRegistry()
{
	m_rootKey = HKEY_CURRENT_USER;
	m_lazyWrite = TRUE;
	m_lastError = ERROR_SUCCESS;
}

CRegistry::~CRegistry()
{
	ClearKey();
}

BOOL CRegistry::ClearKey()
{
	m_key.Empty();
	m_rootKey = HKEY_CURRENT_USER;
	m_lazyWrite = TRUE;
	return TRUE;
}

BOOL CRegistry::SetRootKey(HKEY rootKey)
{
	if (rootKey != HKEY_CLASSES_ROOT && rootKey != HKEY_CURRENT_USER &&
		rootKey != HKEY_LOCAL_MACHINE && rootKey != HKEY_USERS)
		return FALSE;
	m_rootKey = rootKey;
	return TRUE;
}

BOOL CRegistry::CreateKey(CString key)
{
	HKEY handle;
	DWORD disposition = 0;
	if (RegCreateKeyEx(m_rootKey, key, 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &handle, &disposition) != ERROR_SUCCESS)
		return FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	m_key = key;
	return TRUE;
}

BOOL CRegistry::DeleteKey(CString key)
{
	if (!KeyExists(key))
		return TRUE;
	if (RegDeleteKey(m_rootKey, key) != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}

BOOL CRegistry::DeleteValue(CString name)
{
	HKEY handle;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_SET_VALUE, &handle) !=
		ERROR_SUCCESS)
		return FALSE;
	LONG result = RegDeleteValue(handle, name);
	RegCloseKey(handle);
	if (result == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

int CRegistry::GetDataSize(CString name)
{
	HKEY handle;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_QUERY_VALUE, &handle) !=
		ERROR_SUCCESS)
		return -1;
	DWORD size = 1;
	LONG result = RegQueryValueEx(handle, name, NULL, NULL, NULL, &size);
	RegCloseKey(handle);
	if (result != ERROR_SUCCESS)
		return -1;
	return size;
}

DWORD CRegistry::GetDataType(CString name)
{
	HKEY handle;
	m_lastError = RegOpenKeyEx(m_rootKey, m_key, 0, KEY_QUERY_VALUE, &handle);
	if (m_lastError != ERROR_SUCCESS)
		return 0;
	DWORD type = REG_SZ;
	m_lastError = RegQueryValueEx(handle, name, NULL, &type, NULL, NULL);
	RegCloseKey(handle);
	if (m_lastError == ERROR_SUCCESS)
		return type;
	return 0;
}

int CRegistry::GetSubKeyCount()
{
	HKEY handle;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_ALL_ACCESS, &handle) !=
		ERROR_SUCCESS)
		return -1;
	char className[256];
	DWORD classLength = 255;
	DWORD subKeys, maxSubKeyLength, values, maxValueNameLength, maxValueLength;
	FILETIME lastWriteTime;
	LONG result = RegQueryInfoKey(handle, className, &classLength, NULL,
		&subKeys, &maxSubKeyLength, NULL, &values, &maxValueNameLength,
		&maxValueLength, NULL, &lastWriteTime);
	RegCloseKey(handle);
	if (result != ERROR_SUCCESS)
		return -1;
	return subKeys;
}

int CRegistry::GetValueCount()
{
	HKEY handle;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_ALL_ACCESS, &handle) !=
		ERROR_SUCCESS)
		return -1;
	char className[256];
	DWORD classLength = 255;
	DWORD subKeys, maxSubKeyLength, values, maxValueNameLength, maxValueLength;
	FILETIME lastWriteTime;
	LONG result = RegQueryInfoKey(handle, className, &classLength, NULL,
		&subKeys, &maxSubKeyLength, NULL, &values, &maxValueNameLength,
		&maxValueLength, NULL, &lastWriteTime);
	RegCloseKey(handle);
	if (result != ERROR_SUCCESS)
		return -1;
	return values;
}

BOOL CRegistry::KeyExists(CString key, HKEY rootKey)
{
	HKEY handle;
	if (!rootKey)
		rootKey = m_rootKey;
	LONG result = RegOpenKeyEx(rootKey, key, 0, KEY_ALL_ACCESS, &handle);
	RegCloseKey(handle);
	if (result == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

BOOL CRegistry::SetKey(CString key, BOOL create)
{
	HKEY handle;
	DWORD disposition;
	if (key.GetLength() == 0)
	{
		m_key.Empty();
		return TRUE;
	}
	if (create)
	{
		if (RegCreateKeyEx(m_rootKey, key, 0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS, NULL, &handle, &disposition) == ERROR_SUCCESS)
		{
			m_key = key;
			if (!m_lazyWrite)
				RegFlushKey(handle);
			RegCloseKey(handle);
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		m_lastError = RegOpenKeyEx(m_rootKey, key, 0, KEY_ALL_ACCESS, &handle);
		if (m_lastError != ERROR_SUCCESS)
			return FALSE;
		m_key = key;
		if (!m_lazyWrite)
			RegFlushKey(handle);
		RegCloseKey(handle);
		return TRUE;
	}
}

BOOL CRegistry::ValueExists(CString name)
{
	HKEY handle;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_ALL_ACCESS, &handle) !=
		ERROR_SUCCESS)
		return FALSE;
	LONG result = RegQueryValueEx(handle, name, NULL, NULL, NULL, NULL);
	RegCloseKey(handle);
	if (result == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

void CRegistry::RenameValue(CString oldName, CString newName)
{
}

double CRegistry::ReadFloat(CString name, double defaultValue)
{
	HKEY handle;
	double value;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return defaultValue;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&value, &size) !=
		ERROR_SUCCESS)
		value = defaultValue;
	RegCloseKey(handle);
	return value;
}

CString CRegistry::ReadString(CString name, CString defaultValue)
{
	HKEY handle;
	char value[256];
	DWORD type = REG_SZ;
	DWORD size = 255;
	BOOL success = TRUE;
	type = GetDataType(name);
	if (type != REG_SZ && type != REG_EXPAND_SZ)
		return defaultValue;
	m_lastError = RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle);
	if (m_lastError != ERROR_SUCCESS)
		return defaultValue;
	m_lastError =
		RegQueryValueEx(handle, name, NULL, &type, (BYTE*)value, &size);
	if (m_lastError != ERROR_SUCCESS)
		success = FALSE;
	RegCloseKey(handle);
	if (!success)
		return defaultValue;
	return CString(value);
}

DWORD CRegistry::ReadDword(CString name, DWORD defaultValue)
{
	HKEY handle;
	DWORD value;
	DWORD type = REG_DWORD;
	DWORD size = sizeof(value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return defaultValue;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&value, &size) !=
		ERROR_SUCCESS)
		value = defaultValue;
	RegCloseKey(handle);
	return value;
}

int CRegistry::ReadInt(CString name, int defaultValue)
{
	HKEY handle;
	int value;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return defaultValue;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&value, &size) !=
		ERROR_SUCCESS)
		value = defaultValue;
	RegCloseKey(handle);
	return value;
}

BOOL CRegistry::ReadBool(CString name, BOOL defaultValue)
{
	HKEY handle;
	BOOL value;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return defaultValue;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&value, &size) !=
		ERROR_SUCCESS)
		value = defaultValue;
	RegCloseKey(handle);
	return value;
}

COLORREF CRegistry::ReadColor(CString name, COLORREF defaultValue)
{
	HKEY handle;
	COLORREF value;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return defaultValue;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&value, &size) !=
		ERROR_SUCCESS)
		value = defaultValue;
	RegCloseKey(handle);
	return value;
}

BOOL CRegistry::ReadFont(CString name, CFont* value)
{
	HKEY handle;
	LOGFONT font;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(font);
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)&font, &size) !=
		ERROR_SUCCESS)
		success = FALSE;
	RegCloseKey(handle);
	if (success)
	{
		value->Detach();
		value->CreateFontIndirect(&font);
	}
	return success;
}

BOOL CRegistry::ReadPoint(CString name, CPoint* value)
{
	HKEY handle;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(*value);
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)value, &size) !=
		ERROR_SUCCESS)
		success = FALSE;
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::ReadSize(CString name, CSize* value)
{
	HKEY handle;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(*value);
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)value, &size) !=
		ERROR_SUCCESS)
		success = FALSE;
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::ReadRect(CString name, CRect* value)
{
	HKEY handle;
	DWORD type = REG_BINARY;
	DWORD size = sizeof(*value);
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_READ, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(handle, name, NULL, &type, (BYTE*)value, &size) !=
		ERROR_SUCCESS)
		success = FALSE;
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteBool(CString name, BOOL value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&value,
			sizeof(value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteString(CString name, CString value)
{
	HKEY handle;
	char buffer[256];
	BOOL success = TRUE;
	if (value.GetLength() > 254)
		return FALSE;
	strcpy(buffer, value);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_SZ, (const BYTE*)buffer,
			strlen(buffer) + 1) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteFloat(CString name, double value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&value,
			sizeof(value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteInt(CString name, int value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&value,
			sizeof(value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteDword(CString name, DWORD value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&value,
			sizeof(value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteColor(CString name, COLORREF value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&value,
			sizeof(value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteFont(CString name, CFont* value)
{
	HKEY handle;
	LOGFONT font;
	BOOL success = TRUE;
	value->GetLogFont(&font);
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)&font,
			sizeof(font)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WritePoint(CString name, CPoint* value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)value,
			sizeof(*value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteSize(CString name, CSize* value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)value,
			sizeof(*value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}

BOOL CRegistry::WriteRect(CString name, CRect* value)
{
	HKEY handle;
	BOOL success = TRUE;
	if (RegOpenKeyEx(m_rootKey, m_key, 0, KEY_WRITE, &handle) != ERROR_SUCCESS)
		return FALSE;
	if (RegSetValueEx(handle, name, 0, REG_BINARY, (const BYTE*)value,
			sizeof(*value)) != ERROR_SUCCESS)
		success = FALSE;
	if (!m_lazyWrite)
		RegFlushKey(handle);
	RegCloseKey(handle);
	return success;
}
