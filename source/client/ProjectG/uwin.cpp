#include "uwin.h"
#include <sys/stat.h>
#include <io.h>
#include <process.h>
#include <mbstring.h>
#include <tlhelp32.h>
#include <iphlpapi.h>

namespace UWIN
{

	unsigned int GetFileSize(const char* filename)
	{
		struct stat st;
		if (stat(filename, &st) == 0)
			return st.st_size;
		return 0;
	}

	void GenerateCurrentFullPath(const char* filename, char* fullpath)
	{
		if (filename == NULL || fullpath == NULL)
			return;
		GetCurrentDirectory(MAX_PATH, fullpath);
		strcat(fullpath, "\\");
		strcat(fullpath, filename);
	}

	void DeleteReadOnlyFile(const char* filename)
	{
		if (filename == NULL)
			return;
		DWORD attr = GetFileAttributes(filename);
		SetFileAttributes(filename, attr & ~FILE_ATTRIBUTE_READONLY);
		DeleteFile(filename);
	}

	void DeleteReadOnlyFiles(const char* filename)
	{
		if (filename == NULL)
			return;
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(filename, &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			DeleteReadOnlyFile(fd.cFileName);
		} while (FindNextFile(hFind, &fd));
	}

	int IsDots(const char* str)
	{
		if (_tcscmp(str, ".") && _tcscmp(str, ".."))
			return FALSE;
		return TRUE;
	}

	int DeleteDirectory(const char* sPath)
	{
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;

		char DirPath[MAX_PATH];
		char FileName[MAX_PATH];

		_tcscpy(DirPath, sPath);
		_tcscat(DirPath, "\\*");
		_tcscpy(FileName, sPath);
		_tcscat(FileName, "\\");

		hFind = FindFirstFile(DirPath, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
			return FALSE;
		_tcscpy(DirPath, FileName);

		bool bSearch = true;
		while (bSearch)
		{
			if (FindNextFile(hFind, &FindFileData))
			{
				if (IsDots(FindFileData.cFileName))
					continue;
				_tcscat(FileName, FindFileData.cFileName);
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (!DeleteDirectory(FileName))
					{
						FindClose(hFind);
						return FALSE;
					}
					RemoveDirectory(FileName);
					_tcscpy(FileName, DirPath);
				}
				else
				{
					if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
						_chmod(FileName, _S_IWRITE);
					if (!DeleteFile(FileName))
					{
						FindClose(hFind);
						return FALSE;
					}
					_tcscpy(FileName, DirPath);
				}
			}
			else
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					bSearch = false;
				else
				{
					FindClose(hFind);
					return FALSE;
				}
			}
		}
		FindClose(hFind);

		return RemoveDirectory(sPath);
	}

	void SetWindowVisible(HWND hWnd, bool visible)
	{
		LONG style = GetWindowLong(hWnd, GWL_STYLE);
		if (visible)
			SetWindowLong(hWnd, GWL_STYLE, style | WS_VISIBLE);
		else
			SetWindowLong(hWnd, GWL_STYLE, style & ~WS_VISIBLE);
	}

	bool GetExeFilename(HWND hWnd, char* filename)
	{
		if (hWnd == NULL)
			return false;
		bool ret = false;

		DWORD processId = 0;
		GetWindowThreadProcessId(hWnd, &processId);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32* pe = new PROCESSENTRY32;
		pe->dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, pe))
		{
			do
			{
				if (pe->th32ProcessID == processId)
				{
					if (stricmp(pe->szExeFile, "[System Process]") != 0)
					{
						char* p = strrchr(pe->szExeFile, '\\');
						if (p)
							strcpy(filename, p + 1);
						else
							strcpy(filename, pe->szExeFile);
						ret = true;
					}
					break;
				}
			} while (Process32Next(hSnapshot, pe));
		}
		delete pe;
		CloseHandle(hSnapshot);
		return ret;
	}

	bool FindProcessByModuleName(const char* moduleName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		bool ret = false;
		PROCESSENTRY32* pe = new PROCESSENTRY32;
		pe->dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, pe))
		{
			do
			{
				char* p = strrchr(pe->szExeFile, '\\');
				const char* name = p ? p + 1 : pe->szExeFile;
				if (stricmp(name, moduleName) == 0)
				{
					ret = true;
					break;
				}
			} while (Process32Next(hSnapshot, pe));
		}
		delete pe;
		CloseHandle(hSnapshot);
		return ret;
	}

	void* BeginThreadEx(unsigned(__stdcall* startAddress)(void*), void* arg)
	{
		HANDLE hThread =
			(HANDLE)_beginthreadex(NULL, 0, startAddress, arg, 0, NULL);
		CloseHandle(hThread);
		return hThread;
	}

	int axtoi(char* hex)
	{
		int n = 0;
		while (*hex)
		{
			char c = *hex++;
			if (iswdigit(c))
				c = c - '0';
			else if (c >= 'A' && c <= 'F')
				c = c - 'A' + 10;
			else if (c >= 'a' && c <= 'f')
				c = c - 'a' + 10;
			else
				c = 0;
			n = n * 16 + c;
		}
		return n;
	}

	int IP2Integer(const char* ip)
	{
		int len = strlen(ip);
		if (len < 16)
		{
			char buf[20];
			char num[4];
			int part[4];
			memcpy(buf, ip, len);
			buf[len++] = '.';
			buf[len] = 0;

			int j = 0;
			int k = 0;
			for (int i = 0; i < len; ++i)
			{
				if (buf[i] != '.')
				{
					num[j++] = buf[i];
				}
				else
				{
					num[j] = 0;
					part[k++] = atoi(num);
					j = 0;
				}
			}
			return (((((part[0] << 8) | part[1]) << 8) | part[2]) << 8) |
				part[3];
		}
		return -1;
	}

	void AlterChar(std::string& str, char from, char to)
	{
		char* p = (char*)str.c_str();
		for (unsigned int i = 0; i < str.size(); ++i, ++p)
		{
			if (*p == from)
				*p = to;
		}
	}

	int GetLocalMacAddress(char* buf, int len, char sep)
	{
		int total = 0;
		if (buf == NULL && len < 20)
			return 0;

		PIP_ADAPTER_INFO pInfo =
			(PIP_ADAPTER_INFO) new char[sizeof(IP_ADAPTER_INFO)];
		ULONG size = sizeof(IP_ADAPTER_INFO);
		int count = 0;
		if (pInfo)
		{
			DWORD ret = GetAdaptersInfo(pInfo, &size);
			if (ret == ERROR_BUFFER_OVERFLOW)
			{
				delete[] pInfo;
				pInfo = (PIP_ADAPTER_INFO) new char[size];
				ret = GetAdaptersInfo(pInfo, &size);
			}
			if (ret == NO_ERROR)
			{
				PIP_ADAPTER_INFO p = pInfo;
				buf[0] = 0;
				while (p)
				{
					char mac[32];
					sprintf(mac, "%02x%c%02x%c%02x%c%02x%c%02x%c%02x",
						p->Address[0], sep, p->Address[1], sep, p->Address[2],
						sep, p->Address[3], sep, p->Address[4], sep,
						p->Address[5]);
					total += strlen(mac);
					if (total >= len)
						break;
					strcat(buf, mac);
					++count;
					p = p->Next;
				}
			}
			if (pInfo)
				delete[] pInfo;
		}
		return count;
	}

}
