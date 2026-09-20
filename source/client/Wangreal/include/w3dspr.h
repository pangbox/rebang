#pragma once
#include "wresource.h"
#include "wvideo.h"

class WView;

class W3dSpr : public WResource
{
public:
	enum w_spr_align
	{
		CAMERA_XY_ALIGN,
		CAMERA_X_ALIGN,
		NO_ALIGN,
	};

	W3dSpr();
	virtual ~W3dSpr();
	void SetPos(const WVector& position);
	void Render(WView* view, int type, w_spr_align align);
	void SetRect(float width, float height, float pivotX, float pivotY);
	void SetColor(const int color);
	int LoadSprite(const char* name, int flags);
	int AttachTexture(int handle);
	void Render(WView* view, float angle, int type);

private:
	void Rotate(float w, float h, float& rw, float& rh, float angle);

	int m_handle;
	float w1;
	float w2;
	float h1;
	float h2;
	WVector pos;
	WTVertex vtx[4];
	WTVertex* vl[5];
	bool m_texLoaded;
};
