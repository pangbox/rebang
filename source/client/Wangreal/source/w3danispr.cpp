#include "wmemblock.inl"
#include "w3danispr.h"
#include "wview.h"
#include "gamath.h"
#include <stdio.h>

W3dAniSpr::W3dAniSpr()
{
	static int uv[4][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
	};
	for (int i = 0; i < 4; ++i)
	{
		vl[i] = &vtx[i];
		vl[i]->tv = (uv[i][0] * 255.0f + 0.5f) / 256.0f;
		vl[i]->tu = (uv[i][1] * 255.0f + 0.5f) / 256.0f;
		vl[i]->diffuse = -1;
	}
	vl[4] = 0;
	m_nTotalSprite = 0;
}

W3dAniSpr::~W3dAniSpr()
{
	AllDelSprite();
	int handle;
	while ((handle = m_TextureList.Start()) != 0)
	{
		GetResrcManager()->Release(handle);
		m_TextureList -= handle;
	}
}

void W3dAniSpr::SetPos(const WVector& position)
{
	pos = position;
}

void W3dAniSpr::Render(WView* view, int type, w_spr_align align, int nSprNum)
{
	_WSPRITE* pSpr = FindSprite(nSprNum);
	if (!pSpr)
		return;
	vtx[0].tu = pSpr->fStartV;
	vtx[0].tv = pSpr->fStartU;
	vtx[1].tu = pSpr->fEndV;
	vtx[1].tv = pSpr->fStartU;
	vtx[2].tu = pSpr->fEndV;
	vtx[2].tv = pSpr->fEndU;
	vtx[3].tu = pSpr->fStartV;
	vtx[3].tv = pSpr->fEndU;

	switch (align)
	{
	case CAMERA_XY_ALIGN:
		vl[0]->SetPosition(
			pos + view->GetCamera().xa * w1 + view->GetCamera().ya * h2);
		vl[1]->SetPosition(
			pos + view->GetCamera().xa * w2 + view->GetCamera().ya * h2);
		vl[2]->SetPosition(
			pos + view->GetCamera().xa * w2 + view->GetCamera().ya * h1);
		vl[3]->SetPosition(
			pos + view->GetCamera().xa * w1 + view->GetCamera().ya * h1);
		break;
	case CAMERA_X_ALIGN:
	{
		WVector ya = WVector::UNIT_POS_Y;
		vl[0]->SetPosition(pos + view->GetCamera().xa * w1 + ya * h2);
		vl[1]->SetPosition(pos + view->GetCamera().xa * w2 + ya * h2);
		vl[2]->SetPosition(pos + view->GetCamera().xa * w2 + ya * h1);
		vl[3]->SetPosition(pos + view->GetCamera().xa * w1 + ya * h1);
		break;
	}
	case X_ALIGN:
		vl[0]->SetPosition(pos + WVector(w1, h2, 0));
		vl[1]->SetPosition(pos + WVector(w2, h2, 0));
		vl[2]->SetPosition(pos + WVector(w2, h1, 0));
		vl[3]->SetPosition(pos + WVector(w1, h1, 0));
		break;
	case Y_ALIGN:
		vl[0]->SetPosition(pos + WVector(0, h2, w1));
		vl[1]->SetPosition(pos + WVector(0, h2, w2));
		vl[2]->SetPosition(pos + WVector(0, h1, w2));
		vl[3]->SetPosition(pos + WVector(0, h1, w1));
		break;
	case Z_ALIGN:
		vl[0]->SetPosition(pos + WVector(w1, 0, h2));
		vl[1]->SetPosition(pos + WVector(w2, 0, h2));
		vl[2]->SetPosition(pos + WVector(w2, 0, h1));
		vl[3]->SetPosition(pos + WVector(w1, 0, h1));
		break;
	}

	if (type & 0x04000000)
	{
		SetColor(-1);
		view->DrawPolygonFan(vl, type, 0x400, false);
	}
	else
	{
		view->DrawPolygonFan(vl, (pSpr->hTexture & 0x7ff) | type, 0x400, false);
	}
}

void W3dAniSpr::Render(WView* view, float angle, int type, int nSprNum)
{
	static int match[4][2] = {
		{ 1, 2 },
		{ 2, 2 },
		{ 2, 1 },
		{ 1, 1 },
	};
	_WSPRITE* pSpr = FindSprite(nSprNum);
	if (!pSpr)
		return;
	vtx[0].tu = pSpr->fStartV;
	vtx[0].tv = pSpr->fStartU;
	vtx[1].tu = pSpr->fEndV;
	vtx[1].tv = pSpr->fStartU;
	vtx[2].tu = pSpr->fEndV;
	vtx[2].tv = pSpr->fEndU;
	vtx[3].tu = pSpr->fStartV;
	vtx[3].tv = pSpr->fEndU;
	for (int i = 0; i < 4; ++i)
	{
		float x;
		float y;
		Rotate(match[i][0] == 1 ? w1 : w2, match[i][1] == 1 ? h1 : h2, x, y,
			angle);
		vl[i]->SetPosition(
			pos + view->GetCamera().xa * x + view->GetCamera().ya * y);
	}
	if (type & 0x04000000)
	{
		SetColor(-1);
		view->DrawPolygonFan(vl, type, 0x400, false);
	}
	else
	{
		view->DrawPolygonFan(vl, (pSpr->hTexture & 0x7ff) | type, 0x400, false);
	}
}

void W3dAniSpr::Render(WView* view, const WVector& angle, int type, int nSprNum)
{
	static int match[4][2] = {
		{ 1, 2 },
		{ 2, 2 },
		{ 2, 1 },
		{ 1, 1 },
	};
	_WSPRITE* pSpr = FindSprite(nSprNum);
	if (!pSpr)
		return;
	vtx[0].tu = pSpr->fStartV;
	vtx[0].tv = pSpr->fStartU;
	vtx[1].tu = pSpr->fEndV;
	vtx[1].tv = pSpr->fStartU;
	vtx[2].tu = pSpr->fEndV;
	vtx[2].tv = pSpr->fEndU;
	vtx[3].tu = pSpr->fStartV;
	vtx[3].tv = pSpr->fEndU;
	for (int i = 0; i < 4; ++i)
	{
		WVector p =
			WVector(match[i][0] == 1 ? w1 : w2, match[i][1] == 1 ? h1 : h2, 0) *
			RotMat(angle);
		vl[i]->SetPosition(pos + p);
	}
	if (type & 0x04000000)
	{
		SetColor(-1);
		view->DrawPolygonFan(vl, type, 0x400, false);
	}
	else
	{
		view->DrawPolygonFan(vl, (pSpr->hTexture & 0x7ff) | type, 0x400, false);
	}
}

void W3dAniSpr::Render(WView* view, const WMatrix& rot, int type, int nSprNum)
{
	static int match[4][2] = {
		{ 1, 2 },
		{ 2, 2 },
		{ 2, 1 },
		{ 1, 1 },
	};
	_WSPRITE* pSpr = FindSprite(nSprNum);
	if (!pSpr)
		return;
	vtx[0].tu = pSpr->fStartV;
	vtx[0].tv = pSpr->fStartU;
	vtx[1].tu = pSpr->fEndV;
	vtx[1].tv = pSpr->fStartU;
	vtx[2].tu = pSpr->fEndV;
	vtx[2].tv = pSpr->fEndU;
	vtx[3].tu = pSpr->fStartV;
	vtx[3].tv = pSpr->fEndU;
	for (int i = 0; i < 4; ++i)
	{
		WVector p =
			WVector(match[i][0] == 1 ? w1 : w2, match[i][1] == 1 ? h1 : h2, 0) *
			rot;
		vtx[i].SetPosition(pos + p);
	}
	if (type & 0x04000000)
	{
		SetColor(-1);
		view->DrawPolygonFan(vl, type, 0x400, false);
	}
	else
	{
		view->DrawPolygonFan(vl, (pSpr->hTexture & 0x7ff) | type, 0x400, false);
	}
}

void W3dAniSpr::Rotate(float w, float h, float& rw, float& rh, float angle)
{
	float cos = gaMath::Cos(angle);
	float sin = gaMath::Sin(angle);
	rw = w * cos - h * sin;
	rh = w * sin + h * cos;
}

void W3dAniSpr::SetRect(float width, float height, float pivotX, float pivotY)
{
	w1 = -pivotX;
	w2 = width - pivotX;
	h1 = -pivotY;
	h2 = height - pivotY;
}

void W3dAniSpr::SetColor(const int color)
{
	for (int i = 0; i < 4; ++i)
		vl[i]->diffuse = color;
}

int W3dAniSpr::LoadSprite(const char* filename, int type)
{
	WResourceManager* manager = GetResrcManager();
	int handle = manager->LoadTexture(filename, type, 0, 0);
	_WSPRITE* sprite = new _WSPRITE;
	sprite->fEndU = 1.0f;
	sprite->fEndV = 1.0f;
	sprite->hTexture = handle;
	sprite->fStartU = 0.0f;
	sprite->fStartV = 0.0f;
	AddSprite(sprite);
	m_TextureList += handle;
	if (handle)
	{
		char buf[260];
		sprintf(buf, "W3dAniSpr:%s", filename);
		SetLeakHint(buf);
	}
	return handle ? 0 : 1;
}

int W3dAniSpr::LoadSpritesInOneTexture(const char* filename, int type,
	float fSprSizeX, float fSprSizeY)
{
	WResourceManager* manager = GetResrcManager();
	int handle = manager->LoadTexture(filename, type, 0, 0);
	float fWidth = (float)GetResrcManager()->GetTextureWidth(handle);
	float fHeight = (float)GetResrcManager()->GetTextureHeight(handle);
	int nx = (int)(fWidth / fSprSizeX);
	int ny = (int)(fHeight / fSprSizeY);
	float fUnitX = 1.0f / (fWidth / fSprSizeX);
	float fUnitY = 1.0f / (fHeight / fSprSizeY);
	float fx = 0.0f;
	float fy = 0.0f;
	for (int y = 0; y < ny; ++y)
	{
		for (int x = 0; x < nx; ++x)
		{
			_WSPRITE* sprite = new _WSPRITE;
			sprite->hTexture = handle;
			sprite->fStartU = fy;
			sprite->fStartV = fx;
			sprite->fEndU = fy + fUnitY;
			sprite->fEndV = fx + fUnitX;
			AddSprite(sprite);
			fx += fUnitX;
		}
		fx = 0.0f;
		fy += fUnitY;
	}
	m_TextureList += handle;
	return 0;
}

int W3dAniSpr::LoadTexture(int handle, float fSprSizeX, float fSprSizeY)
{
	float fWidth = (float)GetResrcManager()->GetTextureWidth(handle);
	float fHeight = (float)GetResrcManager()->GetTextureHeight(handle);
	int nx = (int)(fWidth / fSprSizeX);
	int ny = (int)(fHeight / fSprSizeY);
	float fUnitX = 1.0f / (fWidth / fSprSizeX);
	float fUnitY = 1.0f / (fHeight / fSprSizeY);
	float fx = 0.0f;
	float fy = 0.0f;
	for (int y = 0; y < ny; ++y)
	{
		for (int x = 0; x < nx; ++x)
		{
			_WSPRITE* sprite = new _WSPRITE;
			sprite->hTexture = handle;
			sprite->fStartU = fy;
			sprite->fStartV = fx;
			sprite->fEndU = fy + fUnitY;
			sprite->fEndV = fx + fUnitX;
			AddSprite(sprite);
			fx += fUnitX;
		}
		fx = 0.0f;
		fy += fUnitY;
	}
	return 1;
}

void W3dAniSpr::AddSprite(_WSPRITE* sprite)
{
	m_SpriteList += sprite;
	++m_nTotalSprite;
}

void W3dAniSpr::DelSprite(_WSPRITE* sprite)
{
	if (sprite)
	{
		m_SpriteList -= sprite;
		delete sprite;
		--m_nTotalSprite;
	}
}

void W3dAniSpr::AllDelSprite()
{
	_WSPRITE* sprite;
	while ((sprite = m_SpriteList.Start()) != 0)
		DelSprite(sprite);
	m_SpriteList.Reset();
}

_WSPRITE* W3dAniSpr::FindSprite(int index)
{
	_WSPRITE* sprite;
	sprite = m_SpriteList.Start();
	for (int i = 0; i < index && sprite; ++i)
		sprite = m_SpriteList.Next();
	return sprite;
}
