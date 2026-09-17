#pragma once

#include <windows.h>

class Bitmap
{
public:
	Bitmap();
	Bitmap(int w, int h, int bpp);
	virtual ~Bitmap();
	Bitmap& operator=(Bitmap& bitmap);

	void Create(int w, int h, int bpp);
	void SetBITMAPINFO(BITMAPINFO* _bi, BYTE* _vram);
	void SetPalette(BYTE* pal);
	void SetGrayPalette(BYTE* pal = 0);
	virtual void Update() { }
	void GetPixel(int x, int y, BYTE& r, BYTE& g, BYTE& b, BYTE& a) const;
	void GetPixel(int x, int y, BYTE& r, BYTE& g, BYTE& b) const;
	void SetPixel(int x, int y, BYTE r, BYTE g, BYTE b, BYTE a);
	BYTE GetAlpha(int x, int y);
	void SetAlpha(int x, int y, BYTE a);
	BYTE* GetVram(int y) { return vram + pitch * y; }
	void Save(const char* filename) const;
	void PaintStretch(HWND hwnd, RECT* rect);
	void Paint(HWND hwnd, POINT* p = 0);
	unsigned Width() const { return bi->bmiHeader.biWidth; }
	unsigned Height() const { return bi->bmiHeader.biHeight; }
	unsigned BitsPerPixel() const { return bi->bmiHeader.biBitCount; }
	unsigned Size() const { return pitch * bi->bmiHeader.biHeight; }

	BITMAPINFO* bi;
	int pitch;
	BYTE* vram;

private:
	BYTE* VMem(BITMAPINFO* bi)
	{
		int count = bi->bmiHeader.biBitCount > 8
			? 0
			: (bi->bmiHeader.biClrUsed ? bi->bmiHeader.biClrUsed : 256);
		return reinterpret_cast<BYTE*>(&bi->bmiColors[count]);
	}
	bool m_lock;
};
