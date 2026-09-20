#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "wtypes.h"

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
