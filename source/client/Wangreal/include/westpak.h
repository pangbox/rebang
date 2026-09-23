#pragma once

#include "cfile.h"
#include "wlist.h"
#include <windows.h>

class cFileMap : public cFile
{
public:
	cFileMap();
	virtual ~cFileMap();
	virtual bool Open(const char* filename);
	virtual int Read(void* buffer, int size);
	virtual int GetByte(void);
	virtual int Tell(void);
	virtual void Seek(int offset, int origin);

private:
	HANDLE m_hFile;
	HANDLE m_hMap;
	char* m_pBuff;
	int m_offset;
};

void __cdecl HookPAKFile(cFile* file);
void __cdecl HookPAKFile(const char* filename);
void __cdecl MakeLog(char* name);
void __fastcall ConvertDirectoryMarks(char* target, unsigned int limit,
	const char* source);
void __cdecl ExportFiles(const char* directory);
void __cdecl EnumFileList(WList<char*>* list);
void __cdecl CloseCFile(cFile* file);
void __cdecl ReleasePAK(void);
cFile* __cdecl GetCFile(const char* name, int buffersize, int flag);
