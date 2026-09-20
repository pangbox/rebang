#pragma once

class cFile
{
public:
	virtual ~cFile() { }
	virtual bool Open(const char* pFilename) { return 0; }
	virtual int Read(void* data, int len);
	virtual int GetByte();
	virtual int Tell();
	virtual void Seek(int pos, int pivot);
	int Length() { return m_nLen; }

	int m_nLen;
};

cFile* __cdecl GetCFile(const char* filename, int mode, int unknown);
void __cdecl CloseCFile(cFile* file);
