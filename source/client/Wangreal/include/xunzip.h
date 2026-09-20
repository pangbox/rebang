#pragma once

#include <windows.h>

typedef DWORD ZRESULT;

#define ZR_OK 0x00000000
#define ZR_RECENT 0x00000001
#define ZR_GENMASK 0x0000FF00
#define ZR_NODUPH 0x00000100
#define ZR_NOFILE 0x00000200
#define ZR_NOALLOC 0x00000300
#define ZR_WRITE 0x00000400
#define ZR_NOTFOUND 0x00000500
#define ZR_MORE 0x00000600
#define ZR_CORRUPT 0x00000700
#define ZR_READ 0x00000800
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS 0x00010000
#define ZR_NOTMMAP 0x00020000
#define ZR_MEMSIZE 0x00030000
#define ZR_FAILED 0x00040000
#define ZR_ENDED 0x00050000
#define ZR_MISSIZE 0x00060000
#define ZR_PARTIALUNZ 0x00070000
#define ZR_ZMODE 0x00080000
#define ZR_BUGMASK 0xFF000000
#define ZR_NOTINITED 0x01000000
#define ZR_SEEK 0x02000000
#define ZR_NOCHANGE 0x04000000
#define ZR_FLATE 0x05000000

#define ZIP_HANDLE 1
#define ZIP_FILENAME 2
#define ZIP_MEMORY 3

struct HZIP__
{
	int unused;
};
typedef struct HZIP__* HZIP;

typedef struct ZIPENTRY
{
	int index;
	char name[MAX_PATH];
	DWORD attr;
	FILETIME atime, ctime, mtime;
	long comp_size;
	long unc_size;
} ZIPENTRY;

typedef struct ZIPENTRYW
{
	int index;
	TCHAR name[MAX_PATH];
	DWORD attr;
	FILETIME atime, ctime, mtime;
	long comp_size;
	long unc_size;
} ZIPENTRYW;

unsigned int FormatZipMessageU(ZRESULT code, char* buf, unsigned int len);
HZIP OpenZipU(void* z, unsigned int len, DWORD flags);
ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY* ze);
ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW* ze);
ZRESULT FindZipItemA(HZIP hz, const char* name, bool ic, int* index,
	ZIPENTRY* ze);
ZRESULT FindZipItemW(HZIP hz, const char* name, bool ic, int* index,
	ZIPENTRYW* ze);
ZRESULT UnzipItem(HZIP hz, int index, void* dst, unsigned int len, DWORD flags);
ZRESULT CloseZipU(HZIP hz);
bool IsZipHandleU(HZIP hz);
