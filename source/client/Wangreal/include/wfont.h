#pragma once
#include "woverlay.h"
#include "wlist.h"

class WFont : public WOverlay
{
public:
	enum eFontStyle
	{
	};

	WFont();
	virtual ~WFont();
	virtual void SetCoordMode(int mode);
	virtual WFont* MakeClone(void);
	virtual float GetTextWidth(WView* view, const char* text);
	virtual void SetFixedWidth(bool fixed);
	virtual void SetFontWidth(int width);
	virtual int GetFontHeight(void);
	virtual void Flush(WView* view);
	virtual void Reset(void);
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned long, Bitmap* bitmap) = 0;
	virtual float GetTextWidthInside(WView* view, const char* text) = 0;

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
public:
	WTitleFont();
	virtual ~WTitleFont();

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned long, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);

private:
	WTITLEFONT m_info;
	int* m_iTexIndex;
	float* m_fWidthIndex;
	char* m_pCharSet;
	int m_numCharSet;
	int m_numPerPage;
	int m_numWidth;
	int m_numTex;
};

class WTexFont : public WFont
{
public:
	WTexFont(const char* filename);
	virtual ~WTexFont();
	int Load(const char* filename, int type);

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned long, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);
};

class WFntFont : public WFont
{
public:
	WFntFont(unsigned char* font);
	virtual ~WFntFont();
	virtual WFont* MakeClone(void);
	int Load(const char* filename);
	virtual void Flush(WView* view);
	void SetFullEnglish(bool full);
	virtual void SetFixedWidth(bool fixed);

private:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned long, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);

	bool m_bFullEnglish;
	bool m_bFixedWidth;
	unsigned char* m_font;
	unsigned char m_midbuff[256];
	Bitmap* m_bitmap[8];
	int m_cachedHandle[8];
	int m_updateFrame[8];
	unsigned short m_cached[128];
	unsigned char m_cachedFontWidth[128];
	int m_cacheLen;
	bool m_clone;
};

class WFixedFont : public WFont
{
public:
	struct w_fixedtext
	{
		int px;
		int py;
		int width;
		char msg[1];
	};

	struct w_flush_area
	{
		w_fixedtext* tex;
		float x;
		float y;
		unsigned long diffuse;
		int type;
	};

	WFixedFont(int size);
	virtual ~WFixedFont();
	virtual WFont* MakeClone(void);
	int Load(const char* filename);
	virtual void Flush(WView* view);
	void Clear(bool all);
	void SetFullEnglish(bool full);
	virtual void SetFixedWidth(bool fixed);
	void Update();
	virtual void Reset();
	virtual int GetFontHeight();

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int, unsigned long, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);

private:
	WList<w_fixedtext*> m_textList;
	w_flush_area m_flush_list[128];
	int m_flush_num;
	bool m_update_flag;
	bool m_bFullEnglish;
	bool m_bFixedWidth;
	unsigned char m_pad[0xb0c - 0xad7];
};
