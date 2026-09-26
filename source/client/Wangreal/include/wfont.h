#pragma once
#include "woverlay.h"
#include "wlist.h"

class WFont : public WOverlay
{
public:
	WFont(void);
	virtual ~WFont(void);
	virtual WFont* MakeClone(void) { return 0; }

	void SetColorSet(int idx, unsigned long color) { m_colorset[idx] = color; }
	void SetScale(float scale) { m_scale = scale; }
	void SetSpace(int space) { m_space = space; }
	virtual void SetCoordMode(int coordMode)
	{
		WOverlay::SetCoordMode(coordMode | 0x400);
	}
	enum eFontStyle
	{
		NORMAL,
		MASKED,
		BOLD,
		SHADOW,
		OUTLINE
	};
	void SetMode(eFontStyle type) { m_eType = type; }
	float Print(WView* view, float x, float y, const char* text, int type,
		unsigned long diffuse, Bitmap* bmp);
	int GetSpace(void) const { return m_space; }
	float GetScale(void) const { return m_scale; }
	virtual float GetTextWidth(WView* view, const char* text);
	virtual void SetFixedWidth(bool flag) { }
	virtual void SetFontWidth(int width) { }
	virtual int GetFontHeight(void) { return 0; }
	virtual void Flush(WView* view);
	virtual void Reset(void) { }

protected:
	float PrintOut(WView* view, float x, float y, const char* text, int type,
		unsigned long diffuse, bool draw, Bitmap* bmp);
	float GetFontScale(WView* view);
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int type, unsigned long diffuse, Bitmap* bmp) = 0;
	virtual float GetTextWidthInside(WView* view, const char* text) = 0;

	eFontStyle m_eType;
	int m_space;

private:
	void ResetOverlay(void);

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
	WTitleFont(void);
	WTitleFont(tagWTITLEFONT* info);
	virtual ~WTitleFont(void);
	void Create(tagWTITLEFONT* info);
	void Erase(void);
	tagWTITLEFONT* GetFontInfo(void) { return &m_info; }
	float PutChar(WView* view, float x, float y, const int nChar, int type,
		unsigned long diffuse);
	float GetCharWidth(WView* view, const int nChar);

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* pText,
		int type, unsigned long diffuse, Bitmap* bmp);
	virtual float GetTextWidthInside(WView* view, const char* pText);

private:
	tagWTITLEFONT m_info;
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
	virtual ~WTexFont(void);
	int Load(const char* filename, int flag);

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int type, unsigned long diffuse, Bitmap* bmp);
	virtual float GetTextWidthInside(WView* view, const char* pText);
};

class WFntFont : public WFont
{
public:
	WFntFont(unsigned char* ptr);
	virtual ~WFntFont(void);
	virtual WFont* MakeClone(void);
	int Load(const char* fntName);
	virtual void Flush(WView* view);
	void SetFullEnglish(bool flag) { m_bFullEnglish = flag; }
	virtual void SetFixedWidth(bool flag) { m_bFixedWidth = flag; }

private:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int type, unsigned long diffuse, Bitmap* bmp);
	virtual float GetTextWidthInside(WView* view, const char* text);
	void Release(void);
	int GetFontIdx(unsigned short code);
	void FlushFont(void);

protected:
	int GetCacheNum(int idx) { return idx / 16; }
	int GetFont(unsigned short code, unsigned char* ptr, int width);
	int AddCache(unsigned short code);
	int PutChar(WView* view, unsigned short code, float x, float y,
		unsigned long type, unsigned long diffuse, int bGetWidth);

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
	WFixedFont(int maxTexture);
	WFixedFont(int w, int h, int alias, unsigned char* ptr, int maxTexture);
	virtual ~WFixedFont(void);
	virtual WFont* MakeClone(void);
	int Load(const char* fntName);
	virtual void Flush(WView* view);
	void Clear(bool flag);
	void SetFullEnglish(bool flag) { m_bFullEnglish = flag; }
	virtual void SetFixedWidth(bool flag) { m_bFixedWidth = flag; }
	void Update(void);
	virtual void Reset(void);
	virtual int GetFontHeight(void) { return m_iFontHeight; }

protected:
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int type, unsigned long diffuse, Bitmap* bmp);
	virtual float GetTextWidthInside(WView* view, const char* text);

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

	struct w_temp_pair
	{
		w_fixedtext* tex;
		w_fixedtext* newone;
		char msg[1];
	};

private:
	void Init(int maxTexture);
	unsigned char* GetFontAddr(unsigned short code, int* width);
	int GetTexture(const char* text);
	w_fixedtext* GetNewPart(const char* text);
	void AddOutArea(w_fixedtext* tex, float x, float y, int type,
		unsigned long diffuse);
	int WriteToBitmap(int width, unsigned char* mid);
	void Write2Buffer(unsigned char* font, unsigned char* buff, int len);
	void ReArrange(void);
	void PrintInTex(w_fixedtext* f, float x, float y, Bitmap* bmp, int type,
		unsigned long diffuse);

	WList<w_fixedtext*> m_textList;
	w_flush_area m_flush_list[128];
	int m_flush_num;
	bool m_update_flag;

	void CreateBitmap(int i);

	bool m_bFullEnglish;
	bool m_bFixedWidth;
	int m_bAntialiased;
	int m_nMaxTexture;
	int* m_texIndex;
	Bitmap** m_bitmap;
	bool* m_update;
	unsigned char* m_font;
	static unsigned char m_mid[65536];
	int m_iFontWidth;
	int m_iFontHeight;
	int m_nMaxWidth;
	int m_nowX;
	int m_nowY;
	int m_bytePerChar;
	bool m_clone;
};
