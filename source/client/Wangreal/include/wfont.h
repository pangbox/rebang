#pragma once
#include "woverlay.h"
#include "wlist.h"

class WFont : public WOverlay
{
public:
	enum eFontStyle
	{
	};

	virtual WFont* MakeClone(void);
	virtual float GetTextWidth(WView* view, const char* text);
	virtual void SetFixedWidth(bool fixed);
	virtual void SetFontWidth(int width);
	virtual int GetFontHeight(void);
	virtual void Flush(WView* view);
	virtual void Reset(void);
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned int, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);

	eFontStyle m_eType;
	int m_space;
	float m_scale;
	int m_colorset[10];
	bool m_useOverlay;
	WList<WOverlay*> m_overlayList;
};

typedef struct tagWTITLEFONT
{
	char** filename;
	int numPages;
	int fontw;
	int fonth;
	int texw;
	int texh;
	int flag;
	char* pCharSet;
} WTITLEFONT;

class WTitleFont : public WFont
{
	WTITLEFONT m_info;
	int* m_iTexIndex;
	float* m_fWidthIndex;
	char* m_pCharSet;
	int m_numCharSet;
	int m_numPerPage;
	int m_numWidth;
	int m_numTex;
};
