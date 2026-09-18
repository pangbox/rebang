#include <math.h>
#include <new>
#include <stdio.h>
#include <string.h>

#include "bitmap.h"

Bitmap::Bitmap()
{
	bi = 0;
	vram = 0;
	m_lock = false;
}

Bitmap::Bitmap(int w, int h, int bpp)
{
	bi = 0;
	vram = 0;
	m_lock = false;
	Create(w, h, bpp);
}

Bitmap::~Bitmap()
{
	if (!m_lock && bi)
		delete[] reinterpret_cast<BYTE*>(bi);
}

Bitmap& Bitmap::operator=(Bitmap& bitmap)
{
	if (!bi || Width() != bitmap.Width() || Height() != bitmap.Height() ||
		BitsPerPixel() != bitmap.BitsPerPixel())
	{
		Create(bitmap.Width(), bitmap.Height(), bitmap.BitsPerPixel());
	}

	bitmap.Update();
	if (BitsPerPixel() <= 8)
	{
		unsigned count = bi->bmiHeader.biClrUsed;
		if (!count)
			count = 256;
		memcpy(bi->bmiColors, bitmap.bi->bmiColors, count * sizeof(RGBQUAD));
	}
	memcpy(vram, bitmap.vram, pitch * bi->bmiHeader.biHeight);
	return *this;
}

void Bitmap::GetPixel(int x, int y, BYTE& r, BYTE& g, BYTE& b, BYTE& a) const
{
	if (x >= 0 && y >= 0 && x < bi->bmiHeader.biWidth &&
		y < bi->bmiHeader.biHeight)
	{
		if (bi)
		{
			if (bi->bmiHeader.biBitCount == 32)
			{
				a = vram[pitch * y + x * 4 + 3];
				r = vram[pitch * y + x * 4 + 2];
				g = vram[pitch * y + x * 4 + 1];
				b = vram[pitch * y + x * 4];
			}
			else if (bi->bmiHeader.biBitCount == 24)
			{
				a = 255;
				r = vram[pitch * y + x * 3 + 2];
				g = vram[pitch * y + x * 3 + 1];
				b = vram[pitch * y + x * 3];
			}
		}
	}
}

void Bitmap::GetPixel(int x, int y, BYTE& r, BYTE& g, BYTE& b) const
{
	if (x < 0 || y < 0 || x >= bi->bmiHeader.biWidth ||
		y >= bi->bmiHeader.biHeight || !bi)
		return;

	if (bi->bmiHeader.biBitCount == 8)
	{
		BYTE index = vram[x + pitch * y];
		r = bi->bmiColors[index].rgbRed;
		g = bi->bmiColors[index].rgbGreen;
		b = bi->bmiColors[index].rgbBlue;
	}
	if (bi->bmiHeader.biBitCount == 24)
	{
		r = vram[x * 3 + pitch * y + 2];
		g = vram[x * 3 + pitch * y + 1];
		b = vram[x * 3 + pitch * y];
	}
	if (bi->bmiHeader.biBitCount == 32)
	{
		r = vram[x * 4 + pitch * y + 2];
		g = vram[x * 4 + pitch * y + 1];
		b = vram[x * 4 + pitch * y];
	}
}

void Bitmap::SetPixel(int x, int y, BYTE r, BYTE g, BYTE b, BYTE a)
{
	if (x < 0 || y < 0 || x >= bi->bmiHeader.biWidth ||
		y >= bi->bmiHeader.biHeight || !bi)
		return;

	if (bi->bmiHeader.biBitCount == 8)
	{
		BYTE index = vram[x + pitch * y];
		bi->bmiColors[index].rgbRed = r;
		bi->bmiColors[index].rgbGreen = g;
		bi->bmiColors[index].rgbBlue = b;
		return;
	}
	if (bi->bmiHeader.biBitCount == 24)
	{
		BYTE* pixel = vram + pitch * y + x * 3;
		*pixel++ = b;
		*pixel++ = g;
		*pixel = r;
		return;
	}
	if (bi->bmiHeader.biBitCount == 32)
	{
		BYTE* pixel = vram + pitch * y + x * 4;
		*pixel++ = b;
		*pixel++ = g;
		*pixel++ = r;
		*pixel = a;
	}
}

BYTE Bitmap::GetAlpha(int x, int y)
{
	if (x < 0 || y < 0 || x >= bi->bmiHeader.biWidth ||
		y >= bi->bmiHeader.biHeight || !bi || bi->bmiHeader.biBitCount != 32)
		return 255;
	return vram[pitch * y + x * 4 + 3];
}

void Bitmap::SetAlpha(int x, int y, BYTE a)
{
	if (x < 0 || y < 0 || x >= bi->bmiHeader.biWidth ||
		y >= bi->bmiHeader.biHeight || !bi || bi->bmiHeader.biBitCount != 32)
		return;
	vram[pitch * y + x * 4 + 3] = a;
}

void Bitmap::SetPalette(BYTE* pal)
{
	for (int i = 0; i < 256; ++i)
	{
		bi->bmiColors[i].rgbBlue = pal[2];
		bi->bmiColors[i].rgbGreen = pal[1];
		bi->bmiColors[i].rgbRed = pal[0];
		pal += 3;
	}
}

void Bitmap::SetGrayPalette(BYTE* pal)
{
	int i;
	if (pal)
	{
		for (i = 0; i < 256; ++i)
		{
			bi->bmiColors[i].rgbBlue = min(i + pal[2], 255);
			bi->bmiColors[i].rgbGreen = min(i + pal[1], 255);
			bi->bmiColors[i].rgbRed = min(i + pal[0], 255);
		}
	}
	else
		for (i = 0; i < 256; ++i)
		{
			bi->bmiColors[i].rgbRed = static_cast<BYTE>(i);
			bi->bmiColors[i].rgbGreen = static_cast<BYTE>(i);
			bi->bmiColors[i].rgbBlue = static_cast<BYTE>(i);
		}
}

void Bitmap::SetBITMAPINFO(BITMAPINFO* _bi, BYTE* _vram)
{
	vram = _vram;
	bi = _bi;
	m_lock = true;
	int bits = bi->bmiHeader.biBitCount * bi->bmiHeader.biWidth;
	pitch = ((bits / 8) + 3) & ~3;
}

void Bitmap::Create(int w, int h, int bpp)
{
	if (!m_lock && bi)
		delete[] reinterpret_cast<BYTE*>(bi);

	pitch = ((bpp / 8) * w + 3) & ~3;
	unsigned header_size =
		sizeof(BITMAPINFOHEADER) + (bpp > 8 ? 0 : sizeof(RGBQUAD) * 256);
	bi = reinterpret_cast<BITMAPINFO*>(
		::operator new[](pitch * h + header_size));
	m_lock = false;
	memset(bi, 0, header_size);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage = 0;
	bi->bmiHeader.biClrUsed = 0;
	bi->bmiHeader.biWidth = w;
	bi->bmiHeader.biHeight = h;
	bi->bmiHeader.biBitCount = static_cast<WORD>(bpp);
	vram = VMem(bi);
}

void Bitmap::Save(const char* filename) const
{
#pragma pack(push, 1)
	struct bmp_info
	{
		long size;
		long width;
		long height;
		short planes;
		short bitcount;
		long compression;
		long sizeimage;
		long xpelspermeter;
		long ypelspermeter;
		long clrused;
		long clrimportant;
	} info = { 40, 0, 0, 1, 24, 0, 0, 0, 0, 0, 0 };
	struct bmp_head
	{
		char signature[2];
		long size;
		short reserved1;
		short reserved2;
		long offbits;
	} header = {
		{ 'B', 'M' },
        14, 0, 0, 54
	};
#pragma pack(pop)

	FILE* file = fopen(filename, "wb");
	if (file)
	{
		info.width = bi->bmiHeader.biWidth;
		info.height = bi->bmiHeader.biHeight;
		fwrite(&header, 1, sizeof(header), file);
		fwrite(&info, 1, sizeof(info), file);
		for (int y = info.height - 1; y >= 0; --y)
		{
			for (int x = 0; x < info.width; ++x)
			{
				BYTE* pixel = vram + pitch * y + x * 3;
				fputc(pixel[0], file);
				fputc(pixel[1], file);
				fputc(pixel[2], file);
			}
			for (int pad = ((info.width * 3 + 3) & ~3) - info.width * 3;
				pad > 0; --pad)
				fputc(0, file);
		}
		fclose(file);
	}
}

void Bitmap::PaintStretch(HWND hwnd, RECT* rect)
{
	HDC hdc = GetDC(hwnd);
	StretchDIBits(hdc, rect->left, rect->top, rect->right - rect->left,
		rect->bottom - rect->top, 0, 0, bi->bmiHeader.biWidth,
		abs(bi->bmiHeader.biHeight), vram, bi, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

void Bitmap::Paint(HWND hwnd, POINT* p)
{
	HDC hdc = GetDC(hwnd);
	DWORD w = bi->bmiHeader.biWidth;
	DWORD h = bi->bmiHeader.biHeight;
	if (p)
	{
		SetDIBitsToDevice(hdc, p->x, p->y, w, h, 0, h, h, h, vram, bi,
			DIB_RGB_COLORS);
	}
	else
	{
		SetDIBitsToDevice(hdc, 0, 0, w, h, 0, h, h, h, vram, bi,
			DIB_RGB_COLORS);
	}
	ReleaseDC(hwnd, hdc);
}
