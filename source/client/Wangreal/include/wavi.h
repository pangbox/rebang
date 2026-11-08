#pragma once

#include <windows.h>
#include <vfw.h>

#include "bitmap.h"

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
	void Write(Bitmap* bitmap);
	sAviResult Close();
	void SetFrameBuffer(int page);
	bool IsOpened() const { return m_bOpened; }

protected:
	void WriteAVI(Bitmap* bitmap);
	void Flush();
	Bitmap* ScaleBitmap(Bitmap* bitmap, int w, int h, int px, int py,
		bool flip);
	void ScaleBitmap(Bitmap* image, Bitmap* bitmap, int w, int h, int px,
		int py, bool flip);

	void CopyBitmap(Bitmap* image, Bitmap* bitmap, bool flip);
	void Release();

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
