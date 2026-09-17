#pragma once

#include <windows.h>

#include "bitmap.h"

struct IAVIFile;
struct IAVIStream;

struct AVISTREAMINFOA
{
	DWORD fccType;
	DWORD fccHandler;
	DWORD dwFlags;
	DWORD dwCaps;
	WORD wPriority;
	WORD wLanguage;
	DWORD dwScale;
	DWORD dwRate;
	DWORD dwStart;
	DWORD dwLength;
	DWORD dwInitialFrames;
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;
	RECT rcFrame;
	DWORD dwEditCount;
	DWORD dwFormatChangeCount;
	char szName[64];
};

struct AVICOMPRESSOPTIONS
{
	DWORD fccType;
	DWORD fccHandler;
	DWORD dwKeyFrameEvery;
	DWORD dwQuality;
	DWORD dwBytesPerSecond;
	DWORD dwFlags;
	void* lpFormat;
	DWORD cbFormat;
	void* lpParms;
	DWORD cbParms;
	DWORD dwInterleaveEvery;
};

extern "C"
{
	void WINAPI AVIFileInit();
	void WINAPI AVIFileExit();
	HRESULT WINAPI AVIFileOpenA(IAVIFile**, const char*, UINT, CLSID*);
	HRESULT WINAPI AVIFileCreateStreamA(IAVIFile*, IAVIStream**,
		AVISTREAMINFOA*);
	HRESULT WINAPI AVIMakeCompressedStream(IAVIStream**, IAVIStream*,
		AVICOMPRESSOPTIONS*, CLSID*);
	HRESULT WINAPI AVIStreamSetFormat(IAVIStream*, LONG, void*, LONG);
	HRESULT WINAPI AVIStreamWrite(IAVIStream*, LONG, LONG, void*, LONG, DWORD,
		LONG*, LONG*);
	ULONG WINAPI AVIStreamRelease(IAVIStream*);
	ULONG WINAPI AVIFileRelease(IAVIFile*);
}

struct sAviResult
{
	int totalFrames;
	long fileSize;
};

class WAVIEncoder
{
public:
	WAVIEncoder();
	virtual ~WAVIEncoder();

	bool Open(char* filename, int w, int h, int fps, HWND hwnd);
	sAviResult Close();
	void SetFrameBuffer(int page);
	void Write(Bitmap* bitmap);
	bool IsOpened() const { return m_bOpened; }

protected:
	void WriteAVI(Bitmap* bitmap);
	void Flush();
	void Release();
	void CopyBitmap(Bitmap* image, Bitmap* bitmap, bool flip);
	void ScaleBitmap(Bitmap* image, Bitmap* bitmap, int w, int h, int px,
		int py, bool flip);
	Bitmap* ScaleBitmap(Bitmap* bitmap, int w, int h, int px, int py,
		bool flip);

private:
	IAVIFile* m_pfile;
	IAVIStream* m_ps;
	IAVIStream* m_psCompressed;
	int m_cur_frame;
	int m_width;
	int m_height;
	Bitmap** m_history;
	int m_hisNum;
	int m_cur;
	bool m_bOpened;
	char m_filename[260];
};
