#pragma once

#include "wfont.h"
#include "list.h"
#include <vector>

class CRenderer;
class CFreeType2;

class CTextOut : public WFont
{
public:
	CTextOut(CRenderer* renderer);
	virtual ~CTextOut(void);
	bool Load(const char* path, int size, int fontIndex, bool preload);
	void Clear(void);
	void SetFontSize(int size);
	int GetWidth(const char* text);
	int Print(float x, float y, const char* text, unsigned long renderState,
		unsigned long diffuse);
	virtual float PrintInside(WView* view, float x, float y, const char* text,
		int renderState, unsigned long diffuse, Bitmap* bitmap);
	virtual float GetTextWidthInside(WView* view, const char* text);
	virtual void Flush(WView* view);

private:
	struct _IMAGESET
	{
		int texture;
		int x;
		int y;
		int width;
		int height;
		_IMAGESET* next;
	};

	struct _TEXTSET
	{
		bool valid;
		int width;
		int fontSize;
		unsigned long lastRenderTick;
		_IMAGESET* images;
		char text[1];
	};

	list::multimap_str<_TEXTSET*> m_TextSet;

	_TEXTSET* GetTextSet(const char* text, int fontSize);
	void UpdateText(_TEXTSET* set);
	void UpdateTexture(void);
	void UseNextTexture(void);
	virtual void Reset(void);

	Bitmap m_WorkImage;
	int m_WorkTextureCnt;
	int m_WorkTextureNum;
	int m_BeginWorkTextureCnt;
	int m_WorkX;
	int m_WorkY[2];
	bool m_bUpdateTexture;
	bool m_bResetTexture;
	unsigned long m_RecentCheckTime;
	std::vector<int> m_TextList;
	struct _DISPLAYLIST
	{
		_TEXTSET* set;
		float x;
		float y;
		unsigned long renderState;
		unsigned long diffuse;
	};

	std::vector<_DISPLAYLIST> m_DisplayList;
	CFreeType2* m_pFontSet;
	CRenderer* m_pRenderer;
	int m_iFontSize;
};
