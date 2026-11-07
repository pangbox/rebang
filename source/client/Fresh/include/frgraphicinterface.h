#pragma once
#include "../../Wangreal/include/wtypes.h"
#include "../../Wangreal/include/woverlay.h"

namespace FrGDI
{
	enum eTextStyleFlags
	{
		NONE = 0,
		THICK = 1,
		OUTLINE = 2,
		SHADOW = 4,
		ITALIC = 8,
		ANTIALIASED = 16,
	};

	enum eTextAlign
	{
		ALIGN_LEFT = 0,
		ALIGN_CENTER = 1,
		ALIGN_RIGHT = 2,
	};
}

class Bitmap;
class TexCacheManager;
class WFont;
class WView;
class FrTEXT;

extern WView* g_view;

class FrGraphicInterface
{
public:
	FrGraphicInterface(WView* view);
	virtual ~FrGraphicInterface();
	void RefreshTexCache(const Bitmap& bitmap);
	void InvalidateCache(const Bitmap& bitmap);
	void Line(const WPoint& start, const WPoint& end, unsigned long color,
		unsigned long type) const;
	void Line(const WPoint& start, const WPoint& end, unsigned long first,
		unsigned long second, unsigned long type) const;
	void Box(const _WRECT& rc, unsigned long color, unsigned long z = 0,
		float alpha = 0.0f) const;
	void RainbowBox(const _WRECT& rect, unsigned long* const colors,
		unsigned long type = 0, float depth = 0.0f) const;
	void LineBox(const _WRECT& rc, unsigned long color, unsigned long z = 0,
		float alpha = 0.0f) const;
	void DrawTexture(const Bitmap* bitmap, const WRect& dst,
		unsigned long color, int flags) const;
	void DrawTexture(const Bitmap* bitmap, const WRect& src, const WRect& dst,
		unsigned long color, int flags) const;
	void UpdateTextureCacheInfo(const Bitmap* bitmap);
	void RefreshTextureCacheInfo(const Bitmap* bitmap);
	void SetClippingArea(WRect* rect);
	void SetClippingArea11(WRect* rect);
	void SetAlpha(float alpha);
	float GetAlpha();
	void SetTextColor(unsigned long color, unsigned long outlineColor);
	void SetTextStyle(unsigned long style);
	float Print(const WPoint& point, unsigned long style, const char* format,
		...);
	float Print11(const WPoint& point, unsigned long align, const char* format,
		...);
	float PrintText(const WPoint& point, unsigned long style, const char* text,
		Bitmap* bitmap);
	void PrintText(const WPoint& point, const FrTEXT& text);
	void PrintText(const WPoint& point, unsigned long align,
		const FrTEXT& text);
	float PrintText11(const WPoint& point, unsigned long style,
		const char* text, Bitmap* bitmap);
	void PrintText11(const WPoint& point, const FrTEXT& text);
	void PrintText11(const WPoint& point, unsigned long align,
		const FrTEXT& text);
	float GetTextExtend(const char* text);
	float GetTextExtend11(const char* text);
	int GetFontHeight();
	int GetFontHeight11();
	int SetSpace(int space);
	int SetSpace11(int space);
	void SetScale(float scale);
	void SetScale11(float scale);
	float GetViewWidth();
	float GetViewHeight();
	void Reset();
	unsigned long GetTextColor() const { return m_textColor; }
	unsigned long GetTextOutlineColor() const { return m_textOutlineColor; }

protected:
	WView* m_pView;
	TexCacheManager* m_pCacheManager;
	float m_alpha;
	unsigned long m_textColor;
	unsigned long m_textOutlineColor;
	unsigned long m_textStyle;
	WFont* m_pFont12;
	WFont* m_pFont11;
	int m_maxTexCache;
	int m_cacheTexSize;
};

inline void FrGraphicInterface::Line(const WPoint& start, const WPoint& end,
	unsigned long first, unsigned long second, unsigned long type) const
{
	g_view->DrawLine2D(start, end, first, second, type);
}

inline void FrGraphicInterface::Box(const _WRECT& rc, unsigned long color,
	unsigned long z, float alpha) const
{
	WOverlay::DrawBox(g_view, rc, z, color, alpha);
}

inline void FrGraphicInterface::RainbowBox(const _WRECT& rect,
	unsigned long* const colors, unsigned long type, float depth) const
{
	WOverlay::DrawRainbowBox(g_view, rect, colors, type, depth);
}

inline void FrGraphicInterface::LineBox(const _WRECT& rc, unsigned long color,
	unsigned long z, float alpha) const
{
	WOverlay::DrawLineBox(g_view, rc, z, color);
}

inline unsigned long FrALPHA(unsigned long color, float alpha)
{
	return ((int)((color >> 24) * alpha) << 24) | (color & 0x00ffffff);
}

inline float FrGraphicInterface::GetViewWidth()
{
	return m_pView ? m_pView->GetWidth() : 0.0f;
}

inline float FrGraphicInterface::GetViewHeight()
{
	return m_pView ? m_pView->GetHeight() : 0.0f;
}
