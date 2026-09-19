#pragma once
#include "wresource.h"
#include "wview.h"

class WOverlay : public WResource
{
public:
	WOverlay(void);
	virtual ~WOverlay(void);
	virtual int Load(const char*, unsigned long);
	virtual void Render(WView*, const _WRECT&, const _WRECT&, int,
		unsigned long, float, unsigned char);
	virtual void SetCoordMode(int);

	int Load(const char*, Bitmap*, unsigned long);
	void SetClippingArea(const WRect*);
	void SetClippingArea(WView*, const WRect*);
	void RenderWithShear(WView*, const _WRECT&, const _WRECT&, float, int,
		unsigned long);
	void ArcClipRender(WView*, const _WRECT&, const _WRECT&, float, float, int,
		unsigned long);
	void RenderWithAxis(WView*, const _WRECT&, const _WRECT&, float, float,
		float, int, unsigned long, unsigned char);

	static void DrawLine(WView*, const _WPOINT&, const _WPOINT&, int,
		unsigned long);
	static void DrawLine(WView*, const _WPOINT&, const _WPOINT&, unsigned long,
		unsigned long, int);
	static void DrawBox(WView*, const WRect&, int, unsigned long, float);
	static void DrawRainbowBox(WView*, const WRect&, unsigned long* const, int,
		float);
	static void DrawLineBox(WView*, const WRect&, int, unsigned long);
	static WRect ConvertRect(WView*, const WRect&, int);
	static void DrawPicture(WView*, const WRect&, int, int, unsigned long);
	static void DrawFrameOverlay1(WView*, WOverlay*, float, float, float, float,
		const RECT&, unsigned long);
	static void DrawFrameOverlay9(WView*, WOverlay** const, const RECT&,
		unsigned long);
	static int CrossRect(const WRect&, const WRect&, WRect*);

protected:
	void DrawTexture(WView*, int, const _WRECT&, const _WRECT&, float, int,
		unsigned long);
	void DrawTexture(WView*, int, const _WRECT&, const _WRECT&, int,
		unsigned long, float, unsigned char, bool);
	void DrawArcClipTexture(WView*, int, const _WRECT&, const _WRECT&, float,
		float, int, unsigned long);
	void DrawTextureWithAxis(WView*, int, const _WRECT&, const _WRECT&, float,
		float, float, int, unsigned long, unsigned char);
	int GetSection(WTVertex*, float, float);
	void ConvertSourceRectByTextureSize(_WRECT&, const _WRECT&) const;
	float GetUnit(WView*, float, wUnitMode);
	void clip_2D_left(WView*, WTVertex*, WTVertex*, int*, int);
	void clip_2D_right(WView*, WTVertex*, WTVertex*, int*, int);
	void clip_2D_top(WView*, WTVertex*, WTVertex*, int*, int);
	void clip_2D_bottom(WView*, WTVertex*, WTVertex*, int*, int);

public:
	unsigned int m_texHandle;
	unsigned int m_texWidth;
	unsigned int m_texHeight;
	unsigned int m_devTexWidth;
	unsigned int m_devTexHeight;
	WRect m_clipArea;
	bool m_clipFlag;
	int m_coordMode;

private:
	static WTVertex* m_vtxList[10];
	static WTVertex m_vtx[4];
	static WTVertex* m_vl[5];

public:
};
