#include <new>
#include <stdio.h>
#include <string.h>

#include "wavi.h"
#include "wminmax.h"

extern "C" __declspec(dllimport) long __cdecl filelength(int);

WAVIEncoder::WAVIEncoder()
{
	m_bOpened = false;
	m_history = 0;
	m_hisNum = 0;
	m_height = 0;
	m_width = 0;
}

WAVIEncoder::~WAVIEncoder()
{
	Release();
}

bool WAVIEncoder::Open(char* filename, int w, int h, int fps, HWND)
{
	m_cur_frame = 0;
	m_width = w;
	m_height = h;
	AVIFileInit();
	if (AVIFileOpenA(&m_pfile, filename, 0x1001, 0))
		return false;

	AVISTREAMINFOA strhdr;
	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType = 0x73646976;
	strhdr.fccHandler = 0x4356534d;
	strhdr.dwScale = 1;
	strhdr.dwRate = fps;
	strhdr.dwSuggestedBufferSize = 0;
	strhdr.dwQuality = -1;
	strhdr.rcFrame.left = 0;
	strhdr.rcFrame.top = 0;
	strhdr.rcFrame.right = w;
	strhdr.rcFrame.bottom = h;
	if (AVIFileCreateStreamA(m_pfile, &m_ps, &strhdr))
		return false;

	AVICOMPRESSOPTIONS opts;
	memset(&opts, 0, sizeof(opts));
	if (AVIMakeCompressedStream(&m_psCompressed, m_ps, &opts, 0))
		return false;

	Bitmap bitmap(w, h, 24);
	if (AVIStreamSetFormat(m_psCompressed, 0, bitmap.bi,
			sizeof(BITMAPINFOHEADER)))
		return false;
	if (m_hisNum > 0)
		SetFrameBuffer(m_hisNum);
	m_bOpened = true;
	strcpy(m_filename, filename);
	return true;
}

sAviResult WAVIEncoder::Close()
{
	if (m_bOpened)
	{
		Flush();
		AVIStreamRelease(m_ps);
		AVIStreamRelease(m_psCompressed);
		AVIFileRelease(m_pfile);
		AVIFileExit();
	}
	sAviResult result;
	result.totalFrames = m_cur_frame;
	m_bOpened = false;
	FILE* file = fopen(m_filename, "rb");
	result.fileSize = file ? filelength(fileno(file)) : 0;
	fclose(file);
	return result;
}

void WAVIEncoder::SetFrameBuffer(int page)
{
	Release();
	m_hisNum = page;
	if (m_height > 0 && m_width > 0)
	{
		m_history = new Bitmap*[page];
		m_cur = 0;
		for (int i = 0; i < m_hisNum; ++i)
			m_history[i] = new Bitmap(m_width, m_height, 24);
	}
}

void WAVIEncoder::Write(Bitmap* bitmap)
{
	if (m_history)
	{
		ScaleBitmap(m_history[m_cur++], bitmap, m_width, m_height, 0, 0, true);
		if (m_cur >= m_hisNum)
			Flush();
	}
	else
	{
		Bitmap* image = ScaleBitmap(bitmap, m_width, m_height, 0, 0, true);
		WriteAVI(image);
		delete image;
	}
}
void WAVIEncoder::WriteAVI(Bitmap* bitmap)
{
	AVIStreamWrite(m_psCompressed, m_cur_frame, 1, bitmap->vram,
		bitmap->bi->bmiHeader.biHeight * bitmap->pitch, 0x10, 0, 0);
	++m_cur_frame;
}

void WAVIEncoder::Flush()
{
	for (int i = 0; i < m_cur; ++i)
		WriteAVI(m_history[i]);
	m_cur = 0;
}

void WAVIEncoder::Release()
{
	if (m_history)
	{
		for (int i = 0; i < m_hisNum; ++i)
			if (m_history[i])
				delete m_history[i];
		delete m_history;
		m_history = 0;
	}
	m_hisNum = 0;
}

Bitmap* WAVIEncoder::ScaleBitmap(Bitmap* bitmap, int w, int h, int px, int py,
	bool flip)
{
	Bitmap* image = new Bitmap(w, h, 24);
	if (w == bitmap->Width() && h == bitmap->Height() &&
		bitmap->BitsPerPixel() == image->BitsPerPixel())
		CopyBitmap(image, bitmap, flip);
	else
		ScaleBitmap(image, bitmap, w, h, px, py, flip);
	return image;
}

void WAVIEncoder::ScaleBitmap(Bitmap* image, Bitmap* bitmap, int w, int h,
	int px, int py, bool flip)
{
	if (image->bi->bmiHeader.biWidth == bitmap->bi->bmiHeader.biWidth &&
		image->bi->bmiHeader.biHeight == bitmap->bi->bmiHeader.biHeight &&
		image->bi->bmiHeader.biBitCount == bitmap->bi->bmiHeader.biBitCount)
	{
		CopyBitmap(image, bitmap, flip);
		return;
	}

	int scale_w = ((bitmap->bi->bmiHeader.biWidth - px) * 256) /
		image->bi->bmiHeader.biWidth;
	int scale_h = ((bitmap->bi->bmiHeader.biHeight - py) * 256) /
		image->bi->bmiHeader.biHeight;
	int extent_div = 0x1000000 / ((scale_h / 16) * (scale_w / 16));
	int v = scale_h * py;
	for (int y = 0; y < image->bi->bmiHeader.biHeight; ++y, v += scale_h)
	{
		int u = scale_w * px;
		int row;
		if (!flip)
			row = y;
		else
			row = image->bi->bmiHeader.biHeight - y - 1;
		BYTE* vram = image->GetVram(row);
		for (int x = 0; x < image->bi->bmiHeader.biWidth;
			++x, u += scale_w, vram += 3)
		{
			int u1 = u + scale_w;
			int r, g, b;
			int v1 = v + scale_h;
			int va = v;
			r = 0;
			g = 0;
			b = 0;
			while (va < v1)
			{
				int dv = Min(256 - (va & 255), v1 - va);
				for (int ua = u; ua < u1;)
				{
					int du = Min(256 - (ua & 255), u1 - ua);
					int extent = (du * dv * extent_div) >> 16;
					int code = (va >> 8) * bitmap->pitch + (ua >> 8) * 3;
					r += bitmap->vram[code + 2] * extent;
					g += bitmap->vram[code + 1] * extent;
					b += bitmap->vram[code] * extent;
					ua += du;
				}
				va += dv;
			}
			int code = (Min((int)(r * 1.2f), 0xff0000) & 0xff0000) |
				((Min((int)(g * 1.2f), 0xff0000) & 0xff0000) >> 8) |
				((Min((int)(b * 1.2f), 0xff0000) & 0xff0000) >> 16);
			memcpy(vram, &code, 3);
		}
	}
}

void WAVIEncoder::CopyBitmap(Bitmap* image, Bitmap* bitmap, bool flip)
{
	for (int y = 0; y < image->bi->bmiHeader.biHeight; ++y)
	{
		if (flip)
			memcpy(image->GetVram(image->bi->bmiHeader.biHeight - y - 1),
				bitmap->GetVram(y), bitmap->pitch);
		else
			memcpy(image->GetVram(y), bitmap->GetVram(y), bitmap->pitch);
	}
}
