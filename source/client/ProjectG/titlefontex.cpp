#include "projectg.h"
#include "wfont.h"
#include "titlefontex.h"

WTitleFontEx::WTitleFontEx()
	: m_pFont(NULL), m_color(0xffffffff)
{
}

WTitleFontEx::~WTitleFontEx()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

float WTitleFontEx::Print(float x, float y, const char* text) const
{
	return m_pFont->Print(g_view, x, y, text, 0, m_color, NULL);
}

void WTitleFontEx::Print(float x, float y, const char* text,
	eTITLEFONTALINE align, float space, float scale) const
{
	if (!text)
		return;

	const char* p = text;
	const char* charSet = m_pFont->GetFontInfo()->pCharSet;
	float oldScale = m_pFont->GetScale();
	m_pFont->SetScale(scale);

	float width = 0.0f;

	switch (align)
	{
	case TFA_CENTER:
		while (*p)
		{
			int c = *p++;
			int index = strchr(charSet, c) - charSet;
			width += m_pFont->GetCharWidth(g_view, index) + space * scale;
		}
		width = (width - space) * 0.5f;
		break;

	case TFA_RIGHT:
		while (*p)
		{
			int c = *p++;
			int index = strchr(charSet, c) - charSet;
			width += m_pFont->GetCharWidth(g_view, index) + space * scale;
		}
		width = width - space;
		break;
	}

	x -= width;

	p = text;
	while (*p)
	{
		int c = *p++;
		int index = strchr(charSet, c) - charSet;
		float w = m_pFont->PutChar(g_view, x, y, index, 0, m_color);
		x += (w + space);
	}

	m_pFont->SetScale(oldScale);
}

float WTitleFontEx::GetFontH() const
{
	return (float)m_pFont->GetFontInfo()->fonth;
}

float WTitleFontEx::GetFontW() const
{
	return (float)m_pFont->GetFontInfo()->fontw;
}

int WTitleFontEx::GetCharIndex(char c)
{
	tagWTITLEFONT* info = m_pFont->GetFontInfo();
	int len = strlen(info->pCharSet);

	for (int i = 0; i < len; i++)
	{
		if (info->pCharSet[i] == c)
			return i;
	}

	return 0;
}

float WTitleFontEx::GetStringWidth(const char* text)
{
	if (!text)
		return 0.0f;

	float width = 0.0f;
	int len = strlen(text);

	for (int i = 0; i < len; i++)
		width += m_pFont->GetCharWidth(g_view, GetCharIndex(text[i]));

	return width;
}

CWindFont::CWindFont()
{
	static char* fileName[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,x";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

COpenTournamentFont::COpenTournamentFont()
{
	static char* fileName[] = { "pot_numbers.tga" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 45;
	info.fonth = 43;
	info.numPages = 1;
	info.texw = 225;
	info.texh = 86;
	info.pCharSet = "1234567890";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

CBlueWindFontSmall::CBlueWindFontSmall()
{
	static char* fileName[] = { "blue_wind_s.tga" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 18;
	info.fonth = 21;
	info.numPages = 1;
	info.texw = 108;
	info.texh = 42;
	info.pCharSet = "0123456789~";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

CBlueWindFontBig::CBlueWindFontBig()
{
	static char* fileName[] = { "blue_wind_b.tga" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 29;
	info.fonth = 33;
	info.numPages = 1;
	info.texw = 174;
	info.texh = 66;
	info.pCharSet = "0123456789+-";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

CBlueHoleSkinFont::CBlueHoleSkinFont()
{
	static char* fileName[] = { "[font_hole_skins_blue.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 32;
	info.pCharSet = "1234567890+,";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

CBarFont::CBarFont()
{
	static char* fileName[] = { "[font_bar.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = fileName;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 32;
	info.pCharSet = "1234567890ym%-.";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}
