#pragma once

class cFile
{
public:
	cFile()
		: m_nLen(0)
	{
	}
	virtual ~cFile() { }
	virtual bool Open(const char* pFilename) { return 0; }
	virtual int Read(void* data, int len) = 0;
	virtual int GetByte() = 0;
	virtual int Tell() = 0;
	virtual void Seek(int pos, int pivot) = 0;
	bool Scan(const char* format, ...);
	int Length() { return m_nLen; }

	int m_nLen;
};

cFile* __cdecl GetCFile(const char* filename, int mode, int unknown);
void __cdecl CloseCFile(cFile* file);
