#pragma once

#include <string>
#include <list>
#include <stack>
#include <vector>
#include <utility>
#include "singleton.h"

inline int float2int(float value)
{
	return (int)value;
}

inline int Ceil(float value, float tolerance)
{
	if (value > 0)
	{
		int result = (int)value;
		if (value - result < tolerance)
			return result;

		return result + 1;
	}
	else
	{
		int result = (int)value;
		if (result - value < tolerance)
			return result;

		return result - 1;
	}
}

namespace WTL
{
	class CString;
}

class Bitmap;
class cFile;
class WResourceManager;

void G_CreateDirectory(const char* path);
char* __cdecl MakeStr(const char* format, ...);
unsigned long ModifyColor(unsigned long color, float bright);
float Random(float min, float max, int seed = -1);
unsigned long htoi(const char* str);
bool str_Replace(std::string& str, const std::string& from,
	const std::string& to);

void SaveJPG(const char* filename, const Bitmap* bitmap, int quality);
void SaveBMP(const char* filename, const Bitmap* bitmap);
std::string GetMyComputerName();

void InitPakedFile();
bool InitPakedFile2();

extern int g_iLoadTotalNum;
extern bool g_bExitDrawLoading;
extern int g_iLoadNextNum;
extern int g_iLoadCurNum;

bool __stdcall GetCRCCheckList(
	std::list<std::pair<std::string, unsigned long> >& result);

WTL::CString __stdcall GetRegIntegratedPakName();

const char* LoadStr(unsigned int id);
bool IsPakFile(const char* name);
bool IsIntegratedPakFile(const char* name);
bool IsExeFile(const char* name);
void DeleteReadOnlyFile(const char* name);
char* FindTag(char* begin, char* end, char* tag);
void TrimExt(const char* source, char* dest);
void GetModuleName(char* name);

void LineParse(const char* line, std::vector<std::string>& tokens);
char* GetStr(char* source, char* dest);
const char* GetStrConst(const char* source, char* dest);
const char* GetCharsTillSpace(const char* source, char* dest);
void LimitText(char* text, int size);
void RemakeNumber(int value, char* text);

template <class T>
void sequence_delete(T first, T last)
{
	while (first != last)
		delete *first++;
}

class WFile
{
public:
	WFile();
	WFile(const char* filename);
	virtual ~WFile();

	cFile* Open(const char* filename, unsigned int option);
	virtual void Close();
	virtual int Read(void* buffer, int size);
	virtual int GetByte();
	virtual int Tell();
	virtual void Seek(int offset, int origin);
	virtual int Length();

	static void SetResourceManager(WResourceManager* pResMng);

protected:
	const char* m_filename;
	cFile* m_pFile;

	static WResourceManager* m_pResMng;
};

template <class T>
class CStackableSingleton : public WSingleton<T>
{
public:
	virtual ~CStackableSingleton() { }

protected:
	CStackableSingleton() { }

public:
	static void PushInstance();
	static void PopInstance();

	static std::stack<T*, std::list<T*> > m_InstanceStack;
};

template <class T>
void CStackableSingleton<T>::PushInstance()
{
	m_InstanceStack.push(WSingleton<T>::m_pInstance);
	WSingleton<T>::m_pInstance = NULL;
}

template <class T>
void CStackableSingleton<T>::PopInstance()
{
	if (m_InstanceStack.empty())
	{
		WSingleton<T>::m_pInstance = NULL;
	}
	else
	{
		WSingleton<T>::m_pInstance = m_InstanceStack.top();
		m_InstanceStack.pop();
	}
}

template <class T>
std::stack<T*, std::list<T*> > CStackableSingleton<T>::m_InstanceStack;
