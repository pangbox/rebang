#pragma once
#include "wresource.h"
#include "wvideo.h"

class WView;

struct _WSPRITE
{
	int hTexture;
	float fStartU;
	float fStartV;
	float fEndU;
	float fEndV;
};

class W3dAniSpr : public WResource
{
public:
	enum w_spr_align
	{
		CAMERA_XY_ALIGN,
		CAMERA_X_ALIGN,
		X_ALIGN,
		Y_ALIGN,
		Z_ALIGN,
		NO_ALIGN,
	};

	W3dAniSpr();
	virtual ~W3dAniSpr();
	void SetPos(const WVector& position);
	void SetRect(float width, float height, float pivotX, float pivotY);
	void SetColor(const int color);
	void Render(WView* view, int type, w_spr_align align, int nSprNum);
	void Render(WView* view, float angle, int type, int nSprNum);
	void Render(WView* view, const WVector& angle, int type, int nSprNum);
	void Render(WView* view, const WMatrix& rot, int type, int nSprNum);
	int LoadSprite(const char* filename, int type);
	int LoadSpritesInOneTexture(const char* filename, int type, float fSprSizeX,
		float fSprSizeY);
	int LoadTexture(int handle, float fSprSizeX, float fSprSizeY);
	int GetSpriteNum() const;

private:
	void Rotate(float w, float h, float& rw, float& rh, float angle);
	void AddSprite(_WSPRITE* sprite);
	void DelSprite(_WSPRITE* sprite);
	void AllDelSprite();
	_WSPRITE* FindSprite(int index);

	WList<int> m_TextureList;
	WList<_WSPRITE*> m_SpriteList;
	int m_nTotalSprite;
	float w1;
	float w2;
	float h1;
	float h2;
	WVector pos;
	WTVertex vtx[4];
	WTVertex* vl[5];
};
