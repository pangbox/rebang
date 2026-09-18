#include <ft2build.h>
#include FT_FREETYPE_H
#include <new>
#include <string.h>
#include <math.h>
#include "wmath.h"

class cFile
{
public:
	virtual void Unknown0() = 0;
	virtual void Unknown1() = 0;
	virtual void Read(void* buffer, int size) = 0;

	int m_nLen;
};

cFile* __cdecl GetCFile(const char* filename, int mode, int unknown);
void __cdecl CloseCFile(cFile* file);

class CFreeType2
{
public:
	CFreeType2();
	~CFreeType2();
	bool Load(const char* fname, int fntsize, int fontindex, bool preloadall);
	bool ChangFontSize(int fontsize);
	bool Render(unsigned short code, int* w, int* h);
	void Write24(uchar* img24, int pitch);
	void Write32a(uchar* img32, int pitch);

private:
	FT_Library m_Library;
	FT_Face m_Face;
	uchar* m_pFileBuffer;
	int m_FontSize;
	unsigned short m_LastLetter;
};

CFreeType2::CFreeType2()
{
	m_pFileBuffer = 0;
	m_LastLetter = 0;
	m_Face = 0;
	m_Library = 0;
}

CFreeType2::~CFreeType2()
{
	if (m_Face)
		FT_Done_Face(m_Face);
	if (m_Library)
		FT_Done_FreeType(m_Library);
	if (m_pFileBuffer)
		delete m_pFileBuffer;
}

bool CFreeType2::Load(const char* fname, int fntsize, int fontindex,
	bool preloadall)
{
	if (FT_Init_FreeType(&m_Library))
		return false;

	m_FontSize = fntsize;
	if (preloadall == true)
	{
		cFile* file = GetCFile(fname, 0x8000, 0);
		if (file)
		{
			int length = file->m_nLen;
			m_pFileBuffer = new uchar[length];
			file->Read(m_pFileBuffer, length);
			CloseCFile(file);
			if (!FT_New_Memory_Face(m_Library, m_pFileBuffer, length, fontindex,
					&m_Face))
				goto loaded;
		}
		return false;
	}

	if (FT_New_Face(m_Library, fname, fontindex, &m_Face))
		return false;

loaded:
	if (FT_Set_Pixel_Sizes(m_Face, 0, fntsize))
		return false;
	return true;
}

bool CFreeType2::ChangFontSize(int fontsize)
{
	if (m_Face && m_FontSize != fontsize)
	{
		if (!FT_Set_Pixel_Sizes(m_Face, 0, fontsize))
		{
			m_FontSize = fontsize;
			return true;
		}
	}
	return false;
}

bool CFreeType2::Render(unsigned short code, int* w, int* h)
{
	if (m_LastLetter != code)
	{
		if (!FT_Get_Char_Index(m_Face, code))
			return false;
		if (FT_Load_Char(m_Face, code, 6))
			return false;
	}

	*w = m_Face->glyph->advance.x >> 6;
	*h = m_Face->glyph->bitmap.rows - m_Face->glyph->bitmap_top + m_FontSize;
	m_LastLetter = code;
	return true;
}

void CFreeType2::Write24(uchar* img24, int pitch)
{
	FT_GlyphSlot glyph = m_Face->glyph;
	int u;
	int v;
	int off;
	switch (glyph->bitmap.pixel_mode)
	{
	case FT_PIXEL_MODE_MONO:
	{
		for (v = 0, off = 0; v < glyph->bitmap.rows; ++v, off = (off + 7) & ~7)
		{
			for (u = 0; u < glyph->bitmap.width; ++u, ++off)
			{
				if (glyph->bitmap.buffer[off >> 3] & (0x80 >> (off & 7)))
				{
					memset(img24 + (glyph->bitmap_left + u) * 3 +
							(v - glyph->bitmap_top + m_FontSize) * pitch,
						255, 3);
				}
			}
		}
		break;
	}
	case FT_PIXEL_MODE_GRAY:
	{
		for (v = 0; v < glyph->bitmap.rows; ++v)
		{
			for (u = 0; u < glyph->bitmap.width; ++u)
			{
				memset(img24 + (glyph->bitmap_left + u) * 3 +
						(v - glyph->bitmap_top + m_FontSize) * pitch,
					glyph->bitmap.buffer[u + glyph->bitmap.width * v], 3);
			}
		}
		break;
	}
	}
}

void CFreeType2::Write32a(uchar* img32, int pitch)
{
	FT_GlyphSlot glyph = m_Face->glyph;
	int u;
	int v;
	int off;
	switch (glyph->bitmap.pixel_mode)
	{
	case FT_PIXEL_MODE_MONO:
	{
		for (v = 0, off = 0; v < glyph->bitmap.rows; ++v, off = (off + 7) & ~7)
		{
			for (u = 0; u < glyph->bitmap.width; ++u, ++off)
			{
				if (glyph->bitmap.buffer[off >> 3] & (0x80 >> (off & 7)))
					img32[(glyph->bitmap_left + u) * 4 +
						(v - glyph->bitmap_top + m_FontSize) * pitch + 3] = 255;
				else
					img32[(glyph->bitmap_left + u) * 4 +
						(v - glyph->bitmap_top + m_FontSize) * pitch + 3] = 0;
			}
		}
		break;
	}
	case FT_PIXEL_MODE_GRAY:
	{
		for (v = 0; v < glyph->bitmap.rows; ++v)
		{
			for (u = 0; u < glyph->bitmap.width; ++u)
			{
				img32[(glyph->bitmap_left + u) * 4 +
					(v - glyph->bitmap_top + m_FontSize) * pitch + 3] =
					glyph->bitmap.buffer[u + glyph->bitmap.width * v];
			}
		}
		break;
	}
	}
}
