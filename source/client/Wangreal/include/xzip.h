#pragma once

#include "xunzip.h"

#define ZIP_FOLDER 4

HZIP CreateZipZ(void* z, unsigned int len, DWORD flags, DWORD attributes);
ZRESULT ZipAdd(HZIP hz, const char* dstzn, void* src, unsigned int len,
	DWORD flags);
ZRESULT ZipGetMemory(HZIP hz, void** buf, unsigned long* len);
ZRESULT CloseZipZ(HZIP hz);
bool IsZipHandleZ(HZIP hz);
unsigned int FormatZipMessageZ(ZRESULT code, char* buf, unsigned int len);
