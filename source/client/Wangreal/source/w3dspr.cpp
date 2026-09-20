#include "w3dspr.h"
#include "wview.h"
#include "gamath.h"
#include <stdio.h>

W3dSpr::W3dSpr()
{
	static int uv[4][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
	};
	m_handle = 0;
	m_texLoaded = false;
	for (int i = 0; i < 4; ++i)
	{
		vl[i] = &vtx[i];
		vl[i]->tu = (uv[i][0] * 255.0f + 0.5f) / 256.0f;
		vl[i]->tv = (uv[i][1] * 255.0f + 0.5f) / 256.0f;
		vl[i]->diffuse = -1;
	}
	vl[4] = 0;
}

W3dSpr::~W3dSpr()
{
	if (m_texLoaded && GetResrcManager() && m_handle)
	{
		GetResrcManager()->Release(m_handle);
		m_handle = 0;
	}
}

void W3dSpr::SetPos(const WVector& position)
{
	pos = position;
}

void W3dSpr::Render(WView* view, int type, w_spr_align align)
{
	if (!m_handle)
		return;

	WVector ya;
	WVector za;
	WVector xa;
	za = pos - view->GetCamera().pivot;
	xa = WCrossProduct(WVector::UNIT_POS_Y, za);
	za.Normalize();
	xa.Normalize();
	switch (align)
	{
	case CAMERA_XY_ALIGN:
		ya = WCrossProduct(za, xa);
		ya.Normalize();
		break;
	case CAMERA_X_ALIGN:
		ya = WVector::UNIT_POS_Y;
		break;
	}

	vl[0]->SetPosition(pos + xa * w1 + ya * h2);
	vl[1]->SetPosition(pos + xa * w2 + ya * h2);
	vl[2]->SetPosition(pos + xa * w2 + ya * h1);
	vl[3]->SetPosition(pos + xa * w1 + ya * h1);
	view->DrawPolygonFan(vl, (m_handle & 0x7ff) | type, 0x400, false);
}

void W3dSpr::Render(WView* view, float angle, int type)
{
	static int match[4][2] = {
		{ 1, 2 },
		{ 2, 2 },
		{ 2, 1 },
		{ 1, 1 },
	};
	if (!m_handle)
		return;

	WVector ya;
	WVector xa;
	{
		WVector za;
		za = pos - view->GetCamera().pivot;
		xa = WCrossProduct(WVector::UNIT_POS_Y, za);
		ya = WCrossProduct(za, xa);
		za.Normalize();
		xa.Normalize();
		ya.Normalize();
	}

	for (int i = 0; i < 4; ++i)
	{
		float x;
		float y;
		Rotate(match[i][0] == 1 ? w1 : w2, match[i][1] == 1 ? h1 : h2, x, y,
			angle);
		vl[i]->SetPosition(pos + xa * x + ya * y);
	}
	view->DrawPolygonFan(vl, (m_handle & 0x7ff) | type, 0x400, false);
}

void W3dSpr::Rotate(float w, float h, float& rw, float& rh, float angle)
{
	float cos = gaMath::Cos(angle);
	float sin = gaMath::Sin(angle);
	rw = w * cos - h * sin;
	rh = w * sin + h * cos;
}

void W3dSpr::SetRect(float width, float height, float pivotX, float pivotY)
{
	w1 = -pivotX;
	w2 = width - pivotX;
	h1 = -pivotY;
	h2 = height - pivotY;
}

void W3dSpr::SetColor(const int color)
{
	for (int i = 0; i < 4; ++i)
		vl[i]->diffuse = color;
}

int W3dSpr::LoadSprite(const char* name, int flags)
{
	m_handle = GetResrcManager()->LoadTexture(name, flags, 0, 0);
	if (m_handle)
	{
		char hint[260];
		m_texLoaded = true;
		sprintf(hint, "W3dSpr:%s", name);
		SetLeakHint(hint);
		return 0;
	}
	return 1;
}

int W3dSpr::AttachTexture(int handle)
{
	m_handle = handle;
	return 1;
}
