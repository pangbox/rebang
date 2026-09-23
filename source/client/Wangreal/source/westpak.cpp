#include "westpak.h"
#include "wminmax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: This is a workaround to get an exact match.
// It's probably what the original code did, but it'll do for now.

void* __cdecl operator new[](unsigned int);
void __cdecl operator delete[](void*);

extern "C"
{
	__declspec(dllimport) int __cdecl open(const char* filename, int mode, ...);
	__declspec(dllimport) int __cdecl close(int handle);
	__declspec(dllimport) int __cdecl read(int handle, void* buffer,
		unsigned int count);
	__declspec(dllimport) long __cdecl lseek(int handle, long offset,
		int origin);
	__declspec(dllimport) long __cdecl tell(int handle);
	__declspec(dllimport) int __cdecl _access(const char* path, int mode);
	__declspec(dllimport) int __cdecl _mkdir(const char* path);
	__declspec(dllimport) int __cdecl _chdir(const char* path);
	__declspec(dllimport) int __cdecl strcmpi(const char* left,
		const char* right);
}

void __fastcall Decipher(const unsigned int* const, unsigned int* const,
	const unsigned int* const);

struct sPakinfo
{
	char type;
	short next;
	int offset;
	int packedsize;
	int size;
	char filename[1];
};

struct sPak
{
	int num;
	cFile* fp;
	int offset;
	short hash[256];
	HANDLE lock;
	sPakinfo** list;
	sPak* next;
};

class cFilePacked : public cFile
{
public:
	cFilePacked(sPak* pak, sPakinfo* info, int buffersize);
	virtual ~cFilePacked();
	virtual int Read(void* target, int size);
	virtual int GetByte(void);
	virtual int Tell(void);
	virtual void Seek(int position, int origin);

protected:
	char* buffer;
	int current;
	int pivot;
	int trace;
	int size;
	int offset;
	int bufsize;
	sPak* pak;
	sPakinfo* pakinfo;

private:
	void ReadBlock(void);
	friend class cFileLZ;
	friend class cFileLZ2;
};

class cFileLZ : public cFilePacked
{
public:
	cFileLZ(sPak* pak, sPakinfo* info, int buffersize);
	virtual int Read(void* target, int size);
	virtual int GetByte(void);
	virtual int Tell(void);
	virtual void Seek(int position, int origin);

protected:
	virtual void ReadBlock(void);
	int current;
	int offset;
	int trace;
	unsigned char buffer[0x2000];
};

class cFileLZ2 : public cFileLZ
{
public:
	cFileLZ2(sPak* archive, sPakinfo* info, int buffersize)
		: cFileLZ(archive, info, buffersize)
	{
	}

private:
	virtual void ReadBlock(void);
};

class cFileRaw : public cFile
{
public:
	cFileRaw(int handle, int buffersize);
	virtual ~cFileRaw();
	virtual int Read(void* target, int size);
	virtual int GetByte(void);
	virtual int Tell(void);
	virtual void Seek(int position, int origin);

private:
	void ReadBlock(void);
	int current;
	int offset;
	int trace;
	int bufsize;
	char* buffer;
	int fhl;
};

static char logname[128] = { 0 };
static char clonedirectory[128] = { 0 };
sPak* pak = 0;

static unsigned long secutable[8] = { 0x0000ff21, 0x0000834f, 0x0000675f,
	0x00000034, 0x0000f237, 0x0000815f, 0x00004765, 0x00000233 };

cFileMap::cFileMap()
{
	m_hFile = 0;
	m_hMap = 0;
	m_pBuff = 0;
	m_offset = 0;
}

cFileMap::~cFileMap()
{
	if (m_pBuff)
	{
		UnmapViewOfFile(m_pBuff);
		m_pBuff = 0;
	}
	if (m_hMap)
	{
		CloseHandle(m_hMap);
		m_hMap = 0;
	}
	if (m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = 0;
	}
}

bool cFileMap::Open(const char* filename)
{
	HANDLE handle = CreateFileA(filename, 0x80000000, 1, 0, 3, 0x80, 0);
	m_hFile = handle;
	if (handle == INVALID_HANDLE_VALUE)
		return false;
	m_nLen = GetFileSize(handle, 0);
	m_hMap = CreateFileMappingA(m_hFile, 0, 2, 0, 0, 0);
	if (m_hMap)
	{
		if (GetLastError() == 0xb6)
		{
			CloseHandle(m_hMap);
			m_hMap = 0;
		}
	}
	if (m_hMap)
	{
		m_pBuff = (char*)MapViewOfFile(m_hMap, 4, 0, 0, 0);
		m_offset = 0;
	}
	return true;
}

int cFileMap::Read(void* target, int size)
{
	if (m_pBuff)
	{
		size = Min(m_nLen - m_offset, size);
		if (size > 0)
		{
			memcpy(target, m_pBuff + m_offset, size);
			m_offset += size;
			return size;
		}
	}
	return 0;
}

int cFileMap::GetByte(void)
{
	if (m_pBuff && m_offset < m_nLen && m_offset >= 0)
		return (unsigned char)m_pBuff[m_offset++];
	return -1;
}

void cFileMap::Seek(int position, int origin)
{
	switch (origin)
	{
	case 0:
		m_offset = position;
		break;
	case 1:
		m_offset = m_offset + position;
		break;
	case 2:
		m_offset = m_nLen + position;
		break;
	}
}

int cFileMap::Tell(void)
{
	return m_offset;
}

class cFileDefault : public cFile
{
public:
	cFileDefault(int handle) { fhl = handle; }
	~cFileDefault() { close(fhl); }
	virtual int Read(void* target, int size) { return read(fhl, target, size); }
	virtual int GetByte(void)
	{
		unsigned char value;
		if (Read(&value, 1))
			return value;
		return -1;
	}

	virtual void Seek(int position, int origin)
	{
		lseek(fhl, position, origin);
	}
	virtual int Tell(void) { return tell(fhl); }

private:
	int fhl;
};

class cFileMemory : public cFile
{
public:
	cFileMemory(cFile* file)
	{
		m_nLen = file->Length();
		buffer = new char[file->Length()];
		file->Read(buffer, file->Length());
		offset = 0;
		CloseCFile(file);
	}
	~cFileMemory() { delete[] buffer; }
	virtual int Read(void* target, int size)
	{
		if (offset + size > m_nLen)
			size = m_nLen - offset;
		if (size)
		{
			memcpy(target, buffer + offset, size);
			offset += size;
		}
		return size;
	}
	virtual int GetByte(void)
	{
		if (offset < m_nLen)
			return (unsigned char)buffer[offset++];
		return -1;
	}

	virtual void Seek(int position, int origin)
	{
		switch (origin)
		{
		case 0:
			offset = position;
			break;
		case 1:
			offset = offset + position;
			break;
		case 2:
			offset = m_nLen + position;
			break;
		}
	}
	virtual int Tell(void) { return offset; }

private:
	char* buffer;
	int offset;
};

static sPak* OpenPAKFILE2(cFile* file)
{
	int namelen = 0;
	if (file == 0)
		return 0;
	sPak* archive = new sPak;
	archive->lock = CreateEventA(0, 0, 1, 0);
	for (int slot = 0; slot < 256; slot++)
		archive->hash[slot] = -1;
	file->Seek(-9, 2);
	int names;
	file->Read(&names, 4);
	file->Read(archive, 4);
	if (archive->num <= 0)
	{
		delete archive;
		return 0;
	}
	archive->list = (sPakinfo**)new char[archive->num * 4];
	archive->list[0] =
		(sPakinfo*)new char[file->Tell() - 8 + (archive->num * 6 - names)];
	file->Seek(names, 0);
	names = 0;
	int index = 0;
	if (archive->num > 0)
	{
		do
		{
			archive->list[index] = (sPakinfo*)((char*)archive->list[0] + names);
			file->Read(&namelen, 1);
			file->Read(archive->list[index], 1);
			if ((archive->list[index]->type & 0xf0) == 0)
				archive->list[index]->type |= 0x10;
			switch (archive->list[index]->type & 0xf0)
			{
			case 0x10:
			{
				file->Read(&archive->list[index]->offset, 4);
				file->Read(&archive->list[index]->packedsize, 4);
				file->Read(&archive->list[index]->size, 4);
				file->Read(archive->list[index]->filename, namelen + 1);
				archive->list[index]->size ^= 0x71;
				for (int b = 0; b < namelen; b++)
					archive->list[index]->filename[b] ^= 0x71;
				int code =
					(int)((strlen(archive->list[index]->filename) & 0xf) *
							0x10 +
						(archive->list[index]->filename[0] & 0xf));
				archive->list[index]->next = archive->hash[code];
				archive->hash[code] = (short)index;
				names = names + 0x15 + namelen;
			}
			break;
			case 0x20:
			{
				unsigned int key[4];
				unsigned int input[2];
				unsigned int output[2];
				file->Read(&archive->list[index]->offset, 4);
				file->Read(&archive->list[index]->packedsize, 4);
				file->Read(&archive->list[index]->size, 4);
				file->Read(archive->list[index]->filename, namelen);
				input[0] = archive->list[index]->offset;
				input[1] = archive->list[index]->size;
				key[0] = 0x0485b576;
				key[1] = 0x05148e02;
				key[2] = 0x05141d96;
				key[3] = 0x028fa9d6;
				Decipher(input, output, key);
				archive->list[index]->offset = output[0];
				archive->list[index]->size = output[1];
				for (int b = 0; b < namelen; b += 8)
				{
					input[0] =
						*(unsigned int*)(archive->list[index]->filename + b);
					input[1] = *(
						unsigned int*)(archive->list[index]->filename + b + 4);
					Decipher(input,
						(unsigned int*)(archive->list[index]->filename + b),
						key);
				}
				int code = (namelen & 0xf) * 0x10 +
					(archive->list[index]->filename[0] & 0xf);
				archive->list[index]->next = archive->hash[code];
				archive->hash[code] = (short)index;
				names = names + 0x14 + namelen;
			}
			break;
			case 0x80:
			{
				file->Read(&archive->list[index]->offset, 4);
				file->Read(&archive->list[index]->packedsize, 4);
				file->Read(&archive->list[index]->size, 4);
				file->Read(archive->list[index]->filename, namelen + 1);
				int code =
					(int)((strlen(archive->list[index]->filename) & 0xf) *
							0x10 +
						(archive->list[index]->filename[0] & 0xf));
				archive->list[index]->next = archive->hash[code];
				archive->hash[code] = (short)index;
				names = names + 0x15 + namelen;
			}
			break;
			}
			index++;
		} while (index < archive->num);
	}
	archive->offset = file->Tell();
	archive->next = 0;
	archive->fp = file;
	return archive;
}

void __cdecl HookPAKFile(const char* filename)
{
	int handle = open(filename, 0x8000, 0);
	if (handle != -1)
	{
		cFile* file = new cFileDefault(handle);
		sPak* archive = OpenPAKFILE2(file);
		archive->next = pak;
		pak = archive;
	}
	else
	{
		cFile* file = GetCFile(filename, 0x8000, 0);
		if (file)
		{
			sPak* archive = OpenPAKFILE2(file);
			archive->next = pak;
			pak = archive;
		}
	}
}

void __cdecl HookPAKFile(cFile* file)
{
	sPak* archive = OpenPAKFILE2(file);
	archive->next = pak;
	pak = archive;
}

static void ReleasePAK(sPak* archive)
{
	if (archive == 0)
		return;
	if (archive->next)
		ReleasePAK(archive->next);
	delete[] (char*)archive->list[0];
	delete[] (char*)archive->list;
	CloseCFile(archive->fp);
	CloseHandle(archive->lock);
	delete archive;
}

void __cdecl ReleasePAK(void)
{
	ReleasePAK(pak);
	pak = 0;
}

void __cdecl MakeLog(char* name)
{
	strcpy(logname, name);
	FILE* stream = fopen(logname, "wt");
	fprintf(stream, "LogFile (%s %s)\n-------\n", "Oct 21 2011", "12:23:28");
	fclose(stream);
}

static void makedirectory(char* path)
{
	CHAR restore[128];
	char part[128];
	GetCurrentDirectoryA(128, restore);
	char* cursor = path;
	while (*cursor != 0)
	{
		cursor = strchr(cursor + 1, '\\');
		if (cursor == 0)
			cursor = path + strlen(path);
		if (cursor[-1] != ':')
		{
			unsigned int used = (unsigned int)(cursor - path);
			memcpy(part, path, used);
			part[used] = 0;
			_mkdir(part);
			_chdir(part);
		}
	}
	_chdir(restore);
}

void __fastcall ConvertDirectoryMarks(char* target, unsigned int limit,
	const char* source)
{
	strcpy(target, source);
	unsigned int index = 0;
	while (target[index] != 0 && index < limit)
	{
		if (target[index] == '/')
			target[index] = '\\';
		index++;
	}
}

void __cdecl ExportFiles(const char* directory)
{
	strcpy(clonedirectory, directory);
	if (clonedirectory[strlen(clonedirectory) - 1] == '/')
	{
		if (clonedirectory[strlen(clonedirectory) - 1] == '\\')
			return;
	}
	strcat(clonedirectory, "\\");
}

static cFile* GetCFileSub(const char* name, int buffersize, int flag)
{
	if (buffersize < 0)
	{
		cFile* source = GetCFile(name, 0x1000, flag);
		if (source)
			return new cFileMemory(source);
		return 0;
	}
	if (logname[0] != 0)
	{
		FILE* stream = fopen(logname, "at");
		fprintf(stream, "%s\n", name);
		fclose(stream);
	}
	for (sPak* archive = pak; archive != 0; archive = archive->next)
	{
		for (int index = 0; index < archive->num; index++)
		{
			if (strcmpi(name, archive->list[index]->filename) == 0)
			{
				switch (archive->list[index]->type & 0xf)
				{
				case 0:
					return new cFilePacked(archive, archive->list[index],
						buffersize);
				case 1:
					return new cFileLZ(archive, archive->list[index],
						buffersize);
				case 3:
					return new cFileLZ2(archive, archive->list[index],
						buffersize);
				}
			}
		}
	}
	if (flag == 0)
	{
		char path[260];
		ConvertDirectoryMarks(path, 0x104, name);
		int handle = open(path, 0x8000, 0);
		if (handle != -1)
			return new cFileRaw(handle, buffersize);
	}
	return 0;
}

cFile* __cdecl GetCFile(const char* name, int buffersize, int flag)
{
	char directory[260];
	char full[260];
	cFile* file = GetCFileSub(name, buffersize, flag);
	if (file && clonedirectory[0] != 0)
	{
		sprintf(full, "%s%s", clonedirectory, name);
		while (strchr(full, '/') != 0)
			*strchr(full, '/') = '\\';
		if (_access(full, 0) == -1)
		{
			strcpy(directory, full);
			*strrchr(directory, '\\') = 0;
			makedirectory(directory);
			FILE* stream = fopen(full, "wb");
			for (int index = 0; index < file->Length(); index++)
				fputc(file->GetByte(), stream);
			fclose(stream);
		}
		file->Seek(0, 0);
	}
	return file;
}

void __cdecl EnumFileList(WList<char*>* out)
{
	WList<char*> seen(0x1000, 0x80);
	for (sPak* archive = pak; archive != 0; archive = archive->next)
	{
		for (int index = 0; index < archive->num; index++)
		{
			sPakinfo* info = archive->list[index];
			if (info->type != 2)
			{
				if (seen.Find(info->filename) == 0)
				{
					char* name = new char[strlen(info->filename) + 1];
					strcpy(name, archive->list[index]->filename);
					*out += name;
					seen.AddItem(name, name, false);
				}
			}
		}
	}
}

void __cdecl CloseCFile(cFile* file)
{
	if (file)
		delete file;
}

bool cFile::Scan(const char* format, ...)
{
	int arg = 0;
	char buffer[128];
	int c = -1;
	int length = (int)strlen(format);

	for (int i = 0; i < length; i++)
	{
		if (format[i] == '%')
		{
			if (c == -1)
			{
				c = GetByte();
				if (c == -1)
					return false;
			}
			while (c == 10 || c == 9 || c == 13 || c == 32)
			{
				c = GetByte();
				if (c == -1)
					return false;
			}
			switch (format[i + 1])
			{
			case 'X':
			case 'x':
			{
				int code = 0;
				int flag = 0;
				while ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
					(c >= 'A' && c <= 'F') || (!flag && c == '-'))
				{
					if (c == '-')
						flag |= 1;
					else
					{
						if (c >= 'a')
							c -= 0x20;
						code = code * 16 + (c <= '9' ? c - '0' : c - 'A' + 10);
					}
					c = GetByte();
					if (c == -1)
						return false;
				}
				if (flag)
					code = -code;
				*((int**)&format)[++arg] = code;
			}
			break;
			case 'F':
			case 'G':
			case 'f':
			case 'g':
			{
				int code = 0;
				int flag = 0;
				for (; (c >= '0' && c <= '9') || (!code && c == '-') ||
					(!flag && c == '.');
					code++)
				{
					if (c == '.')
						flag = 1;
					buffer[code] = (char)c;
					if ((c = GetByte()) == -1)
					{
						code++;
						break;
					}
				}
				buffer[code] = 0;
				float code2 = (float)atof(buffer);
				*((int**)&format)[++arg] = *(int*)&code2;
			}
			break;
			case 'D':
			case 'U':
			case 'd':
			case 'u':
			{
				int code;
				for (code = 0; (c >= '0' && c <= '9') || (!code && c == '-');
					code++)
				{
					buffer[code] = (char)c;
					if ((c = GetByte()) == -1)
					{
						code++;
						break;
					}
				}
				buffer[code] = 0;
				*((int**)&format)[++arg] = atoi(buffer);
			}
			break;
			case 'S':
			case 's':
			{
				int code = 0;
				char* target = ((char**)&format)[++arg];
				for (code = 0; c != 10; code++)
				{
					if (c == 9 || c == 13 || c == 32 || c == -1)
						break;
					target[code] = (char)c;
					if ((c = GetByte()) == -1)
					{
						code++;
						break;
					}
				}
				target[code] = 0;
			}
			break;
			case 'N':
			case 'n':
			{
				int code = 0;
				char* target = ((char**)&format)[++arg];
				for (code = 0; c != 10; code++)
				{
					if (c == 9 || c == 13 || c == -1)
						break;
					target[code] = (char)c;
					if ((c = GetByte()) == -1)
					{
						code++;
						break;
					}
				}
				target[code] = 0;
			}
			break;
			}
		}
	}
	if (c != -1 && c != 13)
		Seek(-1, 1);
	return true;
}

inline void cFilePacked::ReadBlock(void)
{
	if (offset < pivot + size)
	{
		WaitForSingleObject(pak->lock, 0xffffffff);
		if (pak->offset != offset)
			pak->fp->Seek(offset, 0);
		int want = Min(bufsize, pivot - offset + size);
		int start = bufsize - want;
		current = start;
		trace = offset;
		offset += pak->fp->Read(buffer + start, want);
		pak->offset = offset;
		SetEvent(pak->lock);
	}
	else
	{
		current = bufsize;
	}
}

cFilePacked::cFilePacked(sPak* archive, sPakinfo* info, int buffersize)
{
	pak = archive;
	pakinfo = info;
	m_nLen = info->size;
	current = 0;
	trace = info->offset;
	pivot = info->offset;
	offset = info->offset;
	size = info->packedsize;
	int room = (info->packedsize < buffersize) ? info->packedsize : buffersize;
	bufsize = room;
	buffer = new char[room];
	ReadBlock();
}

cFilePacked::~cFilePacked()
{
	delete[] buffer;
}

inline int cFilePacked::GetByte(void)
{
	if (current >= bufsize)
	{
		if (offset >= size + pivot)
			return -1;
		ReadBlock();
	}
	return (unsigned char)buffer[current++];
}

inline int cFilePacked::Read(void* target, int length)
{
	length = Min(size - offset + pivot - current + bufsize, length);
	int done = 0;
	if (length > 0)
	{
		do
		{
			if (current >= bufsize)
				ReadBlock();
			int part = Min(length - done, bufsize - current);
			memcpy((char*)target + done, buffer + current, part);
			current += part;
			done += part;
		} while (done < length);
	}
	return done;
}

void cFilePacked::Seek(int position, int origin)
{
	int noff;
	switch (origin)
	{
	case 0:
		noff = pivot + position;
		break;
	case 1:
		noff = offset - bufsize + current + position;
		break;
	case 2:
		noff = size + pivot + position;
		break;
	}
	noff = Max(pivot, Min(noff, size + pivot));
	if (noff >= trace && noff < offset)
	{
		current = bufsize - offset + noff;
		return;
	}
	offset = noff;
	trace = noff;
	ReadBlock();
}

int cFilePacked::Tell(void)
{
	return offset - bufsize - pivot + current;
}

cFileLZ::cFileLZ(sPak* archive, sPakinfo* info, int buffersize)
	: cFilePacked(archive, info, buffersize)
{
	current = 0;
	offset = 0;
	trace = 0;
}

inline void cFileLZ::ReadBlock(void)
{
	trace = offset;
	unsigned short d1 = (unsigned short)cFilePacked::GetByte();
	int count = 8;
	do
	{
		if (d1 & 1)
		{
			unsigned short d2;
			cFilePacked::Read(&d2, 2);
			unsigned short len = (d2 >> 12) + 2;
			d2 = offset - (d2 & 0xfff);
			while (len > 0)
			{
				unsigned short rest =
					Min(Min((int)len, 0x2000 - (offset & 0x1fff)),
						0x2000 - (d2 & 0x1fff));
				memcpy(buffer + (offset & 0x1fff), buffer + (d2 & 0x1fff),
					rest);
				offset += rest;
				d2 += rest;
				len -= rest;
			}
		}
		else
		{
			buffer[offset & 0x1fff] = (unsigned char)cFilePacked::GetByte();
			offset++;
		}
		d1 >>= 1;
		count--;
	} while (count != 0);
	if (offset > m_nLen)
		offset = m_nLen;
}

inline void cFileLZ2::ReadBlock(void)
{
	trace = offset;
	unsigned int raw = cFilePacked::GetByte();
	unsigned short d1 = (unsigned short)(raw ^ 0xc8);
	int count = 8;
	do
	{
		if (d1 & 1)
		{
			unsigned short d2;
			cFilePacked::Read(&d2, 2);
			d2 ^= secutable[(raw >> 3) & 7];
			unsigned short len = (d2 >> 12) + 2;
			d2 = offset - (d2 & 0xfff);
			while (len > 0)
			{
				unsigned short rest =
					Min(Min((int)len, 0x2000 - (offset & 0x1fff)),
						0x2000 - (d2 & 0x1fff));
				memcpy(buffer + (offset & 0x1fff), buffer + (d2 & 0x1fff),
					rest);
				offset += rest;
				d2 += rest;
				len -= rest;
			}
		}
		else
		{
			buffer[offset & 0x1fff] = (unsigned char)cFilePacked::GetByte();
			offset++;
		}
		d1 >>= 1;
		count--;
	} while (count != 0);
	if (offset > m_nLen)
		offset = m_nLen;
}

int cFileLZ::Read(void* target, int length)
{
	length = Min(m_nLen - current, length);
	int done = 0;
	if (length > 0)
	{
		do
		{
			if (current >= offset)
				ReadBlock();
			int part = Min(Min(length - done, offset - current),
				0x2000 - (current & 0x1fff));
			memcpy((char*)target + done, buffer + (current & 0x1fff), part);
			current += part;
			done += part;
		} while (done < length);
	}
	return done;
}

int cFileLZ::GetByte(void)
{
	if (current >= offset)
	{
		if (current >= m_nLen)
			return -1;
		ReadBlock();
	}
	return (unsigned char)buffer[current++ & 0x1fff];
}

void cFileLZ::Seek(int position, int origin)
{
	int noff = position;
	switch (origin)
	{
	case 0:
		noff = position;
		break;
	case 1:
		noff = current + position;
		break;
	case 2:
		noff = m_nLen + position;
		break;
	default:
		noff = position;
		break;
	}
	noff = Max(0, Min(noff, m_nLen));
	if (noff < trace)
	{
		current = 0;
		offset = 0;
		cFilePacked::Seek(0, 0);
	}
	while (offset < noff)
		ReadBlock();
	current = noff;
}

int cFileLZ::Tell(void)
{
	return current;
}

inline void cFileRaw::ReadBlock(void)
{
	if (offset < m_nLen)
	{
		int want = Min(m_nLen - offset, bufsize);
		current = bufsize - want;
		trace = offset;
		offset += read(fhl, buffer + current, want);
	}
	else
	{
		current = bufsize;
	}
}

cFileRaw::cFileRaw(int handle, int buffersize)
{
	fhl = handle;
	m_nLen = lseek(handle, 0, 2);
	current = 0;
	offset = 0;
	bufsize = Min(m_nLen, buffersize);
	buffer = new char[bufsize];
	lseek(fhl, 0, 0);
	ReadBlock();
}

cFileRaw::~cFileRaw()
{
	if (buffer)
		delete[] buffer;
	close(fhl);
}

int cFileRaw::GetByte(void)
{
	if (current >= bufsize)
	{
		if (offset >= m_nLen)
			return -1;
		ReadBlock();
	}
	return (unsigned char)buffer[current++];
}

int cFileRaw::Read(void* target, int length)
{
	length = Min(m_nLen - current - offset + bufsize, length);
	int done = 0;
	while (done < length)
	{
		if (current >= bufsize)
			ReadBlock();
		int part = Min(length - done, bufsize - current);
		memcpy((char*)target + done, buffer + current, part);
		current += part;
		done += part;
	}
	return done;
}

void cFileRaw::Seek(int position, int origin)
{
	int noff;
	switch (origin)
	{
	default:
		noff = position;
		break;
	case 0:
		noff = position;
		break;
	case 1:
		noff = offset - bufsize + current + position;
		break;
	case 2:
		noff = m_nLen + position;
		break;
	}
	noff = Max(0, Min(noff, m_nLen));
	if (noff >= trace && noff < offset)
	{
		current = bufsize - offset + noff;
		return;
	}
	lseek(fhl, noff, 0);
	int place = tell(fhl);
	offset = place;
	trace = place;
	ReadBlock();
}

int cFileRaw::Tell(void)
{
	return offset - bufsize + current;
}
