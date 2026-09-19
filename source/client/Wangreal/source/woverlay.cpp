#include "woverlay.h"
#include <stdio.h>

WTVertex* WOverlay::m_vtxList[10];
WTVertex WOverlay::m_vtx[4];

WTVertex* WOverlay::m_vl[5] = {
	&WOverlay::m_vtx[0],
	&WOverlay::m_vtx[1],
	&WOverlay::m_vtx[3],
	&WOverlay::m_vtx[2],
	0,
};

WOverlay::WOverlay(void)
	: m_texHandle(0)
	, m_texWidth(0)
	, m_texHeight(0)
	, m_devTexWidth(0)
	, m_devTexHeight(0)
	, m_clipFlag(false)
	, m_coordMode(0x2200)
{
}
WOverlay::~WOverlay(void)
{
	if (m_texHandle)
	{
		GetResrcManager()->Release(m_texHandle);
	}
}

int WOverlay::Load(const char* name, unsigned long flags)
{
	char hint[260];

	if (m_texHandle)
	{
		GetResrcManager()->Release(m_texHandle);
	}
	m_texHandle = GetResrcManager()->LoadTexture(name, flags | 0x80000, 0, 0);
	if (m_texHandle)
	{
		m_texWidth = GetResrcManager()->GetTextureWidth(m_texHandle);
		m_texHeight = GetResrcManager()->GetTextureHeight(m_texHandle);
		m_devTexWidth = GetResrcManager()->video->GetTextureWidth(m_texHandle);
		m_devTexHeight =
			GetResrcManager()->video->GetTextureHeight(m_texHandle);
		sprintf(hint, "WOverlay:%s", name);
		SetLeakHint(hint);
	}
	return m_texHandle == 0;
}

int WOverlay::Load(const char* name, Bitmap* bitmap, unsigned long flags)
{
	char hint[260];

	if (m_texHandle)
	{
		GetResrcManager()->Release(m_texHandle);
	}
	m_texHandle =
		GetResrcManager()->UploadTexture(name, bitmap, flags | 0x80000, 0);
	if (m_texHandle)
	{
		m_texWidth = GetResrcManager()->GetTextureWidth(m_texHandle);
		m_texHeight = GetResrcManager()->GetTextureHeight(m_texHandle);
		m_devTexWidth = GetResrcManager()->video->GetTextureWidth(m_texHandle);
		m_devTexHeight =
			GetResrcManager()->video->GetTextureHeight(m_texHandle);
		sprintf(hint, "WOverlay:%s", name);
		SetLeakHint(hint);
	}
	return m_texHandle == 0;
}

void WOverlay::SetClippingArea(const WRect* rc)
{
	if (rc == 0)
	{
		m_clipFlag = false;
		return;
	}
	m_clipFlag = true;
	m_clipArea = *rc;
}

void WOverlay::DrawTexture(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, float shear, int flag, unsigned long color)
{
	for (int i = 0; i < 4; ++i)
	{
		float px;
		if (i & 1)
			px = dst.w + dst.x;
		else
			px = dst.x;
		float ofs;
		if (i & 2)
			ofs = shear;
		else
			ofs = 0.0f;
		m_vtx[i].x = (px - 0.5f) + ofs;
		float py;
		if (i & 2)
			py = dst.h + dst.y;
		else
			py = dst.y;
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].y = py - 0.5f;
		m_vtx[i].diffuse = color;
		float u;
		if (i & 1)
			u = src.w + src.x;
		else
			u = src.x;
		m_vtx[i].tu = u;
		float v;
		if (i & 2)
			v = src.h + src.y;
		else
			v = src.y;
		m_vtx[i].tv = v;
	}
	pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20300000, 0, 1);
}

int WOverlay::GetSection(WTVertex* v, float base, float angle)
{
	if (angle < base)
	{
		v->x = m_clipArea.x + m_clipArea.w;
		base = tan(angle);
		v->y = m_clipArea.y - base * m_clipArea.w;
		v->tu = 1.0f;
		angle = tan(angle);
		v->tv = 0.5f - (angle * m_clipArea.w * 0.5f) / m_clipArea.h;
		return 0;
	}
	if (angle == base)
	{
		v->x = m_clipArea.w + m_clipArea.x;
		v->y = m_clipArea.y - m_clipArea.h;
		v->tu = 1.0f;
		v->tv = 0.0f;
		return 1;
	}
	if (angle > base && g_PI - base > angle)
	{
		angle = angle - 1.57079637f;
		v->x = m_clipArea.x - tanf(angle) * m_clipArea.h;
		v->y = m_clipArea.y - m_clipArea.h;
		v->tu = 0.5f - (tanf(angle) * m_clipArea.h * 0.5f) / m_clipArea.w;
		v->tv = 0.0f;
		return 2;
	}
	float upper = g_PI - base;
	if (angle == upper)
	{
		v->x = m_clipArea.x - m_clipArea.w;
		v->y = m_clipArea.y - m_clipArea.h;
		v->tu = 0.0f;
		v->tv = 0.0f;
		return 3;
	}
	if (angle > upper && base + g_PI > angle)
	{
		v->x = m_clipArea.x - m_clipArea.w;
		v->y = tanf(angle) * m_clipArea.w + m_clipArea.y;
		v->tu = 0.0f;
		v->tv = (tanf(angle) * m_clipArea.w * 0.5f) / m_clipArea.h + 0.5f;
		return 4;
	}
	float lower = base + g_PI;
	if (angle == lower)
	{
		v->x = m_clipArea.x - m_clipArea.w;
		v->y = m_clipArea.h + m_clipArea.y;
		v->tu = 0.0f;
		v->tv = 1.0f;
		return 5;
	}
	if (angle > lower && 6.28318548f - base > angle)
	{
		angle = angle - 4.71238899f;
		v->x = tanf(angle) * m_clipArea.h + m_clipArea.x;
		v->y = m_clipArea.y + m_clipArea.h;
		v->tu = (tanf(angle) * m_clipArea.h * 0.5f) / m_clipArea.w + 0.5f;
		v->tv = 1.0f;
		return 6;
	}
	float wrap = 6.28318548f - base;
	if (angle == wrap)
	{
		v->x = m_clipArea.w + m_clipArea.x;
		v->y = m_clipArea.h + m_clipArea.y;
		v->tu = 1.0f;
		v->tv = 1.0f;
		return 7;
	}
	if (angle > wrap)
	{
		v->x = m_clipArea.x + m_clipArea.w;
		v->y = m_clipArea.y - tanf(angle) * m_clipArea.w;
		v->tu = 1.0f;
		v->tv = 0.5f - (tanf(angle) * m_clipArea.w * 0.5f) / m_clipArea.h;
		return 8;
	}
	return -1;
}

void WOverlay::ConvertSourceRectByTextureSize(_WRECT& out,
	const _WRECT& in) const
{
	if (m_texWidth == m_devTexWidth || m_devTexWidth <= 0)
	{
		out.x = in.x;
		out.w = in.w;
	}
	else
	{
		out.x = ((float)m_texWidth * in.x) / (float)m_devTexWidth;
		out.w = ((float)m_texWidth * in.w) / (float)m_devTexWidth;
	}
	if (m_texHeight != m_devTexHeight && m_devTexHeight > 0)
	{
		out.y = ((float)m_texHeight * in.y) / (float)m_devTexHeight;
		out.h = ((float)m_texHeight * in.h) / (float)m_devTexHeight;
	}
	else
	{
		out.y = in.y;
		out.h = in.h;
	}
}

void WOverlay::DrawLine(WView* pView, const _WPOINT& a, const _WPOINT& b,
	int flag, unsigned long color)
{
	WTVertex v[2];
	WTVertex* vl[3];

	vl[0] = &v[0];
	v[0].x = a.x;
	v[0].y = a.y;
	v[0].z = 0.001f;
	v[0].rhw = 1.0f;
	v[0].diffuse = color;
	vl[1] = &v[1];
	v[1].x = b.x;
	v[1].y = b.y;
	v[1].z = 0.001f;
	v[1].rhw = 1.0f;
	v[1].diffuse = color;
	vl[2] = 0;
	pView->DrawPolygonFan(vl, flag | 0x4000000, 0, 1);
}

void WOverlay::DrawLine(WView* pView, const _WPOINT& a, const _WPOINT& b,
	unsigned long color1, unsigned long color2, int flag)
{
	WTVertex v[2];
	WTVertex* vl[3];

	vl[0] = &v[0];
	v[0].x = a.x;
	v[0].y = a.y;
	v[0].z = 0.001f;
	v[0].rhw = 1.0f;
	v[0].diffuse = color1;
	vl[1] = &v[1];
	v[1].x = b.x;
	v[1].y = b.y;
	v[1].z = 0.001f;
	v[1].rhw = 1.0f;
	v[1].diffuse = color2;
	vl[2] = 0;
	pView->DrawPolygonFan(vl, flag | 0x4000000, 0, 1);
}

void WOverlay::DrawBox(WView* pView, const WRect& rc, int flag,
	unsigned long color, float z)
{
	for (int i = 0; i < 4; ++i)
	{
		float px;
		if (i & 1)
			px = rc.w + rc.x;
		else
			px = rc.x;
		m_vtx[i].x = px - 0.5f;

		float py;
		if (i & 2)
			py = rc.h + rc.y;
		else
			py = rc.y;
		m_vtx[i].y = py - 0.5f;
		m_vtx[i].z = z;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
	}
	if (flag & 0x7ff)
	{
		for (int i = 0; i < 4; ++i)
		{
			float u;
			if (i & 1)
				u = 0.0f;
			else
				u = 1.0f;
			m_vtx[i].tu = u;
			float v;
			if (i & 2)
				v = 0.0f;
			else
				v = 1.0f;
			m_vtx[i].tv = v;
		}
	}
	pView->DrawPolygonFan(m_vl, flag | 0x20300000, 0, 1);
}

void WOverlay::DrawRainbowBox(WView* pView, const WRect& rc,
	unsigned long* const colors, int flag, float z)
{
	for (int i = 0; i < 4; ++i)
	{
		float px;
		if (i & 1)
			px = rc.w + rc.x;
		else
			px = rc.x;
		m_vtx[i].x = px - 0.5f;

		float py;
		if (i & 2)
			py = rc.h + rc.y;
		else
			py = rc.y;
		m_vtx[i].y = py - 0.5f;
		m_vtx[i].z = z;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = colors[i];
	}
	if (flag & 0x7ff)
	{
		for (int i = 0; i < 4; ++i)
		{
			float u;
			if (i & 1)
				u = 0.0f;
			else
				u = 1.0f;
			m_vtx[i].tu = u;
			float v;
			if (i & 2)
				v = 0.0f;
			else
				v = 1.0f;
			m_vtx[i].tv = v;
		}
	}
	pView->DrawPolygonFan(m_vl, flag | 0x20300000, 0, 1);
}

void WOverlay::DrawLineBox(WView* pView, const WRect& rc, int z,
	unsigned long color)
{
	_WPOINT p[5];

	p[0].x = rc.x;
	p[1].x = rc.w;
	p[4].y = p[1].x + p[0].x;
	p[0].y = rc.y;
	p[1].y = rc.h;
	p[2].y = p[0].y;
	p[3].y = p[0].y;
	p[2].x = p[0].x;
	p[4].x = p[4].y;
	p[4].y = p[1].y + p[0].y;
	p[1].x = p[0].x;
	p[3].x = p[4].x;
	p[1].y = p[4].y;

	DrawLine(pView, p[2], p[3], z | 0x300000, color);
	DrawLine(pView, p[3], p[4], z | 0x300000, color);
	DrawLine(pView, p[4], p[1], z | 0x300000, color);
	DrawLine(pView, p[1], p[2], z | 0x300000, color);
}

float WOverlay::GetUnit(WView* pView, float value, wUnitMode mode)
{
	switch (mode)
	{
	case W_UNIT_XPOS:
		if (m_coordMode & 0x100)
		{
			switch (m_coordMode & 6)
			{
			case 0:
				return (pView->GetWidth() / 640.0f) * value;
			case 2:
				return (value + 320.0f) * (pView->GetWidth() / 640.0f);
			case 4:
				return (value + 640.0f) * (pView->GetWidth() / 640.0f);
			}
		}
		break;
	case W_UNIT_YPOS:
		if (m_coordMode & 0x100)
		{
			switch (m_coordMode & 0x60)
			{
			case 0:
				return (pView->GetHeight() / 480.0f) * value;
			case 0x20:
				return (value + 240.0f) * (pView->GetHeight() / 480.0f);
			case 0x40:
				return (value + 480.0f) * (pView->GetHeight() / 480.0f);
			}
		}
		break;
	case W_UNIT_WIDTH:
		if (m_coordMode & 0x1000)
		{
			return (pView->GetWidth() / 640.0f) * value;
		}
		break;
	case W_UNIT_HEIGHT:
		if (m_coordMode & 0x1000)
		{
			return (pView->GetHeight() / 480.0f) * value;
		}
		break;
	}
	return value;
}

WRect WOverlay::ConvertRect(WView* pView, const WRect& rc, int mode)
{
	float ox;
	if (!(mode & 6))
	{
		ox = 0.0f;
	}
	else
	{
		if (mode & 2)
			ox = 0.5f;
		else
			ox = 1.0f;
		ox *= pView->GetWidth();
	}

	float oy;
	if (!(mode & 0x60))
	{
		oy = 0.0f;
	}
	else
	{
		if (mode & 0x20)
			oy = 0.5f;
		else
			oy = 1.0f;
		oy *= pView->GetHeight();
	}

	WRect ret;
	if (mode & 0x100)
	{
		ret.x = pView->GetWidth() * rc.x / 640.0f + ox;
		ret.y = pView->GetHeight() * rc.y / 480.0f + oy;
	}
	else
	{
		ret.x = rc.x + ox;
		ret.y = rc.y + oy;
	}

	if (mode & 0x1000)
	{
		ret.w = pView->GetWidth() * rc.w / 640.0f;
		ret.h = pView->GetHeight() * rc.h / 480.0f;
	}
	else
	{
		ret.w = rc.w;
		ret.h = rc.h;
	}
	return ret;
}

void WOverlay::DrawPicture(WView* pView, const WRect& rc, int tex, int flag,
	unsigned long color)
{
	for (int i = 0; i < 4; ++i)
	{
		float px;
		if (i & 1)
			px = rc.w + rc.x;
		else
			px = rc.x;
		m_vtx[i].x = px - 0.5f;

		float py;
		if (i & 2)
			py = rc.h + rc.y;
		else
			py = rc.y;
		m_vtx[i].y = py - 0.5f;

		float u;
		if (i & 1)
			u = 1.0f;
		else
			u = 0.0f;
		m_vtx[i].tu = u;

		float v;
		if (i & 2)
			v = 1.0f;
		else
			v = 0.0f;
		m_vtx[i].tv = v;

		m_vtx[i].diffuse = color;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].z = 0.001f;
	}
	pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20000000, 0, 1);
}

void WOverlay::DrawFrameOverlay1(WView* pView, WOverlay* pOv, float lw,
	float th, float rw, float bh, const RECT& rc, unsigned long color)
{
	float hx = lw + rw;
	float hy = th + bh;
	float tw = (float)(int)pOv->m_texWidth;
	float hu = 0.5f / tw;
	float ty = (float)(int)pOv->m_texHeight;
	float hv = 0.5f / ty;
	float du = 1.0f / tw;
	float dv = 1.0f / ty;
	float nu = -hu;
	float nv = -hv;
	float L = (float)rc.left;
	float T = (float)rc.top;
	float R = (float)rc.right;
	float B = (float)rc.bottom;
	float mw = (float)(int)pOv->m_texWidth - hx;
	float rx = (float)(int)pOv->m_texWidth - rw;
	float mh = (float)(int)pOv->m_texHeight - hy;
	float ry = (float)(int)pOv->m_texHeight - bh;
	volatile _WRECT raw[9];
	_WRECT s[9];
	_WRECT d;

	raw[0].x = 0.0f;
	raw[0].y = 0.0f;
	raw[0].w = lw;
	raw[0].h = th;
	raw[1].x = lw;
	raw[1].y = 0.0f;
	raw[1].w = mw;
	raw[1].h = th;
	raw[2].x = rx;
	raw[2].y = 0.0f;
	raw[2].w = rw;
	raw[2].h = th;
	raw[3].x = 0.0f;
	raw[3].y = bh;
	raw[3].w = lw;
	raw[3].h = mh;
	raw[4].x = lw;
	raw[4].y = bh;
	raw[4].w = mw;
	raw[4].h = mh;
	raw[5].x = rx;
	raw[5].y = bh;
	raw[5].w = rw;
	raw[5].h = mh;
	raw[6].x = 0.0f;
	raw[6].y = ry;
	raw[6].w = lw;
	raw[6].h = bh;
	raw[7].x = lw;
	raw[7].y = ry;
	raw[7].w = mw;
	raw[7].h = bh;
	raw[8].x = rx;
	raw[8].y = ry;
	raw[8].w = rw;
	raw[8].h = bh;

	s[0].x = raw[0].x * du + hu;
	s[0].y = raw[0].y * dv + hv;
	s[0].w = raw[0].w * du + nu;
	s[0].h = raw[0].h * dv + nv;
	s[1].x = raw[1].x * du + hu;
	s[1].y = raw[1].y * dv + hv;
	s[1].w = raw[1].w * du + nu;
	s[1].h = raw[1].h * dv + nv;
	s[2].x = raw[2].x * du + hu;
	s[2].y = raw[2].y * dv + hv;
	s[2].w = raw[2].w * du + nu;
	s[2].h = raw[2].h * dv + nv;
	s[3].x = raw[3].x * du + hu;
	s[3].y = raw[3].y * dv + hv;
	s[3].w = raw[3].w * du + nu;
	s[3].h = raw[3].h * dv + nv;
	s[4].x = raw[4].x * du + hu;
	s[4].y = raw[4].y * dv + hv;
	s[4].w = raw[4].w * du + nu;
	s[4].h = raw[4].h * dv + nv;
	s[5].x = raw[5].x * du + hu;
	s[5].y = raw[5].y * dv + hv;
	s[5].w = raw[5].w * du + nu;
	s[5].h = raw[5].h * dv + nv;
	s[6].x = raw[6].x * du + hu;
	s[6].y = raw[6].y * dv + hv;
	s[6].w = raw[6].w * du + nu;
	s[6].h = raw[6].h * dv + nv;
	s[7].x = raw[7].x * du + hu;
	s[7].y = raw[7].y * dv + hv;
	s[7].w = raw[7].w * du + nu;
	s[7].h = raw[7].h * dv + nv;
	s[8].x = raw[8].x * du + hu;
	s[8].y = raw[8].y * dv + hv;
	s[8].w = raw[8].w * du + nu;
	s[8].h = raw[8].h * dv + nv;

	d.x = L;
	d.y = T;
	d.w = lw;
	d.h = th;
	pOv->Render(pView, s[0], d, 0x2000000, color, 0.0f, 0);
	d.x = L + lw;
	d.y = T;
	d.w = R - hx;
	d.h = th;
	pOv->Render(pView, s[1], d, 0x2000000, color, 0.0f, 0);
	d.x = (R + L) - rw;
	d.y = T;
	d.w = rw;
	d.h = th;
	pOv->Render(pView, s[2], d, 0x2000000, color, 0.0f, 0);
	d.x = L;
	d.y = T + th;
	d.w = lw;
	d.h = B - hy;
	pOv->Render(pView, s[3], d, 0x2000000, color, 0.0f, 0);
	d.x = L + lw;
	d.y = T + th;
	d.w = R - hx;
	d.h = B - hy;
	pOv->Render(pView, s[4], d, 0x2000000, color, 0.0f, 0);
	d.x = (R + L) - rw;
	d.y = T + th;
	d.w = rw;
	d.h = B - hy;
	pOv->Render(pView, s[5], d, 0x2000000, color, 0.0f, 0);
	d.x = L;
	d.y = (B + T) - bh;
	d.w = lw;
	d.h = bh;
	pOv->Render(pView, s[6], d, 0x2000000, color, 0.0f, 0);
	d.x = L + lw;
	d.y = (B + T) - bh;
	d.w = R - hx;
	d.h = bh;
	pOv->Render(pView, s[7], d, 0x2000000, color, 0.0f, 0);
	d.x = (R + L) - rw;
	d.y = (B + T) - bh;
	d.w = rw;
	d.h = bh;
	pOv->Render(pView, s[8], d, 0x2000000, color, 0.0f, 0);
}

void WOverlay::DrawFrameOverlay9(WView* pView, WOverlay** const pOv,
	const RECT& rc, unsigned long color)
{
	float w3 = (float)(int)pOv[3]->m_texWidth;
	float w5 = (float)(int)pOv[5]->m_texWidth;
	float h1 = (float)(int)pOv[1]->m_texHeight;
	float h7 = (float)(int)pOv[7]->m_texHeight;
	float mw = w5 + w3;
	float mh = h7 + h1;
	float L = (float)rc.left;
	float T = (float)rc.top;
	float R = (float)rc.right;
	float B = (float)rc.bottom;
	_WRECT s[9];
	_WRECT d;

	for (int i = 0; i < 9; ++i)
	{
		s[i].x = 0.0f;
		s[i].y = 0.0f;
		s[i].w = 1.0f;
		s[i].h = 1.0f;
	}

	d.x = L;
	d.y = T;
	d.w = w3;
	d.h = h1;
	pOv[0]->Render(pView, s[0], d, 0x2080000, color, 0.0f, 0);
	d.x = L + w3;
	d.y = T;
	d.w = R - mw;
	d.h = h1;
	pOv[1]->Render(pView, s[1], d, 0x2080000, color, 0.0f, 0);
	d.x = (R + L) - w5;
	d.y = T;
	d.w = w5;
	d.h = h1;
	pOv[2]->Render(pView, s[2], d, 0x2080000, color, 0.0f, 0);
	d.x = L;
	d.y = T + h1;
	d.w = w3;
	d.h = B - mh;
	pOv[3]->Render(pView, s[3], d, 0x2080000, color, 0.0f, 0);
	d.x = L + w3;
	d.y = T + h1;
	d.w = R - mw;
	d.h = B - mh;
	pOv[4]->Render(pView, s[4], d, 0x2080000, color, 0.0f, 0);
	d.x = (R + L) - w5;
	d.y = T + h1;
	d.w = w5;
	d.h = B - mh;
	pOv[5]->Render(pView, s[5], d, 0x2080000, color, 0.0f, 0);
	d.x = L;
	d.y = (B + T) - h7;
	d.w = w3;
	d.h = h7;
	pOv[6]->Render(pView, s[6], d, 0x2080000, color, 0.0f, 0);
	d.x = L + w3;
	d.y = (B + T) - h7;
	d.w = R - mw;
	d.h = h7;
	pOv[7]->Render(pView, s[7], d, 0x2080000, color, 0.0f, 0);
	d.x = (R + L) - w5;
	d.y = (B + T) - h7;
	d.w = w5;
	d.h = h7;
	pOv[8]->Render(pView, s[8], d, 0x2080000, color, 0.0f, 0);
}

int WOverlay::CrossRect(const WRect& a, const WRect& b, WRect* out)
{
	float ax = a.x;
	float bx = b.x;
	float x = (ax < bx) ? bx : ax;
	out->x = x;
	float ay = a.y;
	float by = b.y;
	float y = (ay < by) ? by : ay;
	out->y = y;

	float br = b.w + b.x;
	float ar = a.w + a.x;
	float r = (ar > br) ? br : ar;
	out->w = r - x;

	float bb = b.h + b.y;
	float ab = a.h + a.y;
	float bt = (ab > bb) ? bb : ab;
	out->h = bt - y;

	if (out->w < 0.0f || out->h < 0.0f)
		return 1;
	return 0;
}

void WOverlay::SetClippingArea(WView* pView, const WRect* rc)
{
	if (rc || pView)
	{
		m_clipFlag = true;
		if (rc)
			m_clipArea = *rc;
		if (pView)
		{
			if (rc)
			{
				WRect full(0.0f, 0.0f, pView->GetWidth(),
					pView->GetHeight());
				WRect out;
				CrossRect(m_clipArea, full, &out);
				m_clipArea = out;
			}
			else
			{
				m_clipArea =
					WRect(0.0f, 0.0f, pView->GetWidth(), pView->GetHeight());
			}
		}
	}
	else
	{
		m_clipFlag = false;
	}
}

void WOverlay::RenderWithShear(WView* pView, const _WRECT& src,
	const _WRECT& dst, float shear, int flag, unsigned long color)
{
	_WRECT s;
	WRect r;

	ConvertSourceRectByTextureSize(s, src);
	r.x = dst.x;
	r.y = dst.y;
	r.w = dst.w;
	r.h = dst.h;
	DrawTexture(pView, m_texHandle, s, ConvertRect(pView, r, m_coordMode),
		shear, flag, color);
}

void WOverlay::DrawTexture(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, int flag, unsigned long color, float rot,
	unsigned char mirror, bool clip)
{
	float sx = src.x, sy = src.y, sw = src.w, sh = src.h;
	WRect d;
	WRect c;

	if (m_coordMode & 0x400)
	{
		d.x = floor(dst.x);
		d.y = floor(dst.y);
		d.w = dst.w;
		d.h = dst.h;
		if (m_clipFlag)
		{
			c.x = floor(m_clipArea.x);
			c.y = floor(m_clipArea.y);
			c.w = m_clipArea.w;
			c.h = m_clipArea.h;
		}
	}
	else
	{
		d = (const WRect&)dst;
		bool cf = m_clipFlag;
		if (cf)
			c = m_clipArea;
		if (pView->xGetProjScale() > 1.0f)
		{
			pView->xConvScreenRectByProjScale(d);
			if (cf)
				pView->xConvScreenRectByProjScale(c);
		}
	}
	if (clip == true)
	{
		WRect screen;
		WRect out;
		screen.h = pView->GetHeight();
		screen.w = pView->GetWidth();
		screen.x = 0.0f;
		screen.y = 0.0f;
		if (CrossRect(d, screen, &out) != 0)
			return;
		if (m_clipFlag && CrossRect(d, c, &out) != 0)
			return;
		if (d.w != out.w || d.h != out.h)
		{
			if (d.w != out.w)
			{
				float ratio = src.w / d.w;
				float delta = out.x - d.x;
				c.x = delta * ratio + src.x;
				c.w = (((out.x + out.w) - (d.w + d.x)) - delta) * ratio + src.w;
			}
			else
			{
				c.x = src.x;
				c.w = src.w;
			}
			if (d.h != out.h)
			{
				float ratio = src.h / d.h;
				float delta = out.y - d.y;
				c.y = delta * ratio + src.y;
				c.h = (((out.y + out.h) - (d.h + d.y)) - delta) * ratio + src.h;
			}
			else
			{
				c.y = src.y;
				c.h = src.h;
			}
			sx = c.x;
			sy = c.y;
			sw = c.w;
			sh = c.h;
			d = out;
		}
	}
	float fx, fy, fx2, fy2;
	if (m_coordMode & 0x400)
	{
		fx = d.x - 0.5f;
		fx2 = fx + d.w;
		fy = d.y - 0.5f;
		fy2 = fy + d.h;
	}
	unsigned int mv = ~(mirror >> 1) & 1;
	unsigned int mu = ~mirror & 1;
	for (int i = 0; i < 4; ++i)
	{
		if ((m_coordMode & 0x400) == 0)
		{
			float px;
			if (i & 1)
				px = d.w + d.x;
			else
				px = d.x;
			m_vtx[i].x = px - 0.5f;
			float py;
			if (i & 2)
				py = d.h + d.y;
			else
				py = d.y;
			m_vtx[i].y = py - 0.5f;
		}
		else
		{
			float px;
			if (i & 1)
				px = fx2;
			else
				px = fx;
			m_vtx[i].x = px;
			float py;
			if (i & 2)
				py = fy2;
			else
				py = fy;
			m_vtx[i].y = py;
		}
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
		float u;
		if ((unsigned int)(i & 1) == mu)
			u = sw + sx;
		else
			u = sx;
		m_vtx[i].tu = u;
		float v;
		if ((unsigned int)((i >> 1) & 1) == mv)
			v = sh + sy;
		else
			v = sy;
		m_vtx[i].tv = v;
	}
	if (rot != 0.0f)
	{
		float cs = (float)cos((double)rot);
		float sn = (float)sin((double)rot);
		float cx = d.w * 0.5f + d.x;
		float cy = d.h * 0.5f + d.y;
		for (int j = 0; j < 4; ++j)
		{
			float ox = m_vtx[j].x - cx;
			float oy = m_vtx[j].y - cy;
			m_vtx[j].x = (ox * cs - oy * sn) + cx;
			m_vtx[j].y = oy * cs + ox * sn + cy;
		}
	}
	pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20300000, 0, 1);
}

void WOverlay::DrawArcClipTexture(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, float a0, float a1, int flag, unsigned long color)
{
	WTVertex vEnd;
	WTVertex vStart;
	WTVertex center;

	m_vtx[0].z = 0.001f;
	m_vtx[0].diffuse = color;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].tu = src.x + src.w;
	m_vtx[0].tv = src.y;
	m_vtx[0].x = (dst.w + dst.x) - 0.5f;
	m_vtx[1].z = 0.001f;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].diffuse = color;
	m_vtx[0].y = dst.y - 0.5f;
	m_vtx[1].tu = src.x;
	m_vtx[1].tv = src.y;
	m_vtx[1].x = dst.x - 0.5f;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = color;
	m_vtx[1].y = dst.y - 0.5f;
	m_vtx[2].z = 0.001f;
	m_vtx[2].tu = src.x + src.w;
	m_vtx[2].tv = src.y + src.h;
	m_vtx[2].x = (dst.w + dst.x) - 0.5f;
	m_vtx[3].z = 0.001f;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = color;
	m_vtx[2].y = (dst.h + dst.y) - 0.5f;
	m_vtx[3].tu = src.x;
	m_vtx[3].tv = src.y + src.h;
	m_vtx[3].x = dst.x - 0.5f;
	m_vtx[3].y = (dst.h + dst.y) - 0.5f;
	m_clipArea.x = dst.w * 0.5f + dst.x;
	m_clipArea.y = dst.h * 0.5f + dst.y;
	m_clipArea.w = dst.w * 0.5f;
	m_clipArea.h = dst.h * 0.5f;
	float base = (float)atan((double)(dst.h / dst.w));
	vStart.z = 0.001f;
	vEnd.z = 0.001f;
	center.z = 0.001f;
	float ref = (base * g_PI) / g_PI;
	center.tu = 0.5f;
	center.tv = 0.5f;
	vStart.diffuse = color;
	vEnd.diffuse = color;
	center.diffuse = color;
	vStart.rhw = 1.0f;
	vEnd.rhw = 1.0f;
	center.rhw = 1.0f;
	center.x = dst.w * 0.5f + dst.x;
	center.y = dst.h * 0.5f + dst.y;
	int i0 = GetSection(&vStart, ref, a0);
	int i1 = GetSection(&vEnd, ref, a1);
	m_vtxList[1] = &vStart;
	m_vtxList[0] = &center;
	int n = 0;
	if (a1 <= a0)
	{
		int span = ((i1 - i0) + 8) / 2;
		if (span > 0)
		{
			int k = i0 / 2;
			do
			{
				int idx = k % 4;
				m_vtxList[n + 2] = m_vl[idx];
				++n;
				++k;
			} while (n < span);
		}
	}
	else
	{
		int span = (i1 - i0) / 2;
		if (span > 0)
		{
			WTVertex** srcp = m_vl + i0 / 2;
			WTVertex** dstp = m_vtxList + 2;
			for (int k = span; k != 0; --k)
			{
				*dstp = *srcp;
				++srcp;
				++dstp;
			}
			n = span;
		}
	}
	m_vtxList[n + 2] = &vEnd;
	m_vtxList[n + 3] = 0;
	pView->DrawPolygonFan(m_vtxList, (tex & 0x7ff) | flag | 0x20300000, 0x800,
		1);
}

void WOverlay::clip_2D_left(WView* pView, WTVertex* out, WTVertex* in,
	int* count, int n)
{
	*count = 0;
	if (n <= 0)
		return;
	WTVertex* p = in;
	int j = 1;
	float d0, d1, d2;
	for (int i = n; i > 0; --i, ++p, ++j)
	{
		if (p->x > 0.0f)
		{
			out[*count].x = p->x;
			out[*count].y = p->y;
			out[*count].tu = p->tu;
			out[*count].tv = p->tv;
			*count = *count + 1;
		}
		WTVertex* q = in + j % n;
		if (p->x * q->x < 0.0f)
		{
			float px = p->x;
			d0 = (px > 0.0f) ? px : -px;
			float qx = q->x;
			d1 = (qx > 0.0f) ? qx : -qx;
			float px2 = p->x;
			d2 = (px2 > 0.0f) ? px2 : -px2;
			out[*count].x = 0.0f;
			float ratio = d0 / (d2 + d1);
			out[*count].y = (q->y - p->y) * ratio + p->y;
			out[*count].tu = (q->tu - p->tu) * ratio + p->tu;
			out[*count].tv = (q->tv - p->tv) * ratio + p->tv;
			*count = *count + 1;
		}
	}
}

void WOverlay::clip_2D_right(WView* pView, WTVertex* out, WTVertex* in,
	int* count, int n)
{
	*count = 0;
	if (n <= 0)
		return;
	WTVertex* p = in;
	int j = 1;
	float d1, d0, d2;
	for (int i = n; i > 0; --i, ++p, ++j)
	{
		if (p->x < pView->GetWidth())
		{
			out[*count].x = p->x;
			out[*count].y = p->y;
			out[*count].tu = p->tu;
			out[*count].tv = p->tv;
			*count = *count + 1;
		}
		if ((in[j % n].x - pView->GetWidth()) * (p->x - pView->GetWidth()) <
			0.0f)
		{
			WTVertex* q = &in[j % n];
			float px = p->x - pView->GetWidth();
			d0 = (px > 0.0f) ? px : -px;
			float qx = q->x - pView->GetWidth();
			d1 = (qx > 0.0f) ? qx : -qx;
			px = p->x - pView->GetWidth();
			d2 = (px > 0.0f) ? px : -px;
			out[*count].x = pView->GetWidth();
			float ratio = d0 / (d2 + d1);
			out[*count].y = (q->y - p->y) * ratio + p->y;
			out[*count].tu = (q->tu - p->tu) * ratio + p->tu;
			out[*count].tv = (q->tv - p->tv) * ratio + p->tv;
			*count = *count + 1;
		}
	}
}

void WOverlay::clip_2D_top(WView* pView, WTVertex* out, WTVertex* in,
	int* count, int n)
{
	*count = 0;
	if (n <= 0)
		return;
	WTVertex* p = in;
	int j = 1;
	float d0, d1, d2;
	for (int i = n; i > 0; --i, ++p, ++j)
	{
		if (p->y > 0.0f)
		{
			out[*count].x = p->x;
			out[*count].y = p->y;
			out[*count].tu = p->tu;
			out[*count].tv = p->tv;
			*count = *count + 1;
		}
		WTVertex* q = in + j % n;
		if (p->y * q->y < 0.0f)
		{
			float py = p->y;
			d0 = (py > 0.0f) ? py : -py;
			float qy = q->y;
			d1 = (qy > 0.0f) ? qy : -qy;
			float py2 = p->y;
			d2 = (py2 > 0.0f) ? py2 : -py2;
			float ratio = d0 / (d2 + d1);
			out[*count].x = (q->x - p->x) * ratio + p->x;
			out[*count].y = 0.0f;
			out[*count].tu = (q->tu - p->tu) * ratio + p->tu;
			out[*count].tv = (q->tv - p->tv) * ratio + p->tv;
			*count = *count + 1;
		}
	}
}

void WOverlay::clip_2D_bottom(WView* pView, WTVertex* out, WTVertex* in,
	int* count, int n)
{
	*count = 0;
	if (n <= 0)
		return;
	WTVertex* p = in;
	int j = 1;
	float d1, d0;
	for (int i = n; i > 0; --i, ++p, ++j)
	{
		if (p->y < pView->GetHeight())
		{
			out[*count].x = p->x;
			out[*count].y = p->y;
			out[*count].tu = p->tu;
			out[*count].tv = p->tv;
			*count = *count + 1;
		}
		if ((p->y - pView->GetHeight()) * (in[j % n].y - pView->GetHeight()) <
			0.0f)
		{
			WTVertex* q = &in[j % n];
			float py0 = p->y - pView->GetHeight();
			d0 = (py0 > 0.0f) ? py0 : -py0;
			float qy = q->y - pView->GetHeight();
			d1 = (qy > 0.0f) ? qy : -qy;
			float py2 = p->y - pView->GetHeight();
			float d2 = (py2 > 0.0f) ? py2 : -py2;
			float ratio = d0 / (d2 + d1);
			out[*count].x = (q->x - p->x) * ratio + p->x;
			out[*count].y = pView->GetHeight();
			out[*count].tu = (q->tu - p->tu) * ratio + p->tu;
			out[*count].tv = (q->tv - p->tv) * ratio + p->tv;
			*count = *count + 1;
		}
	}
}

void WOverlay::Render(WView* pView, const _WRECT& src, const _WRECT& dst,
	int flag, unsigned long color, float rot, unsigned char mirror)
{
	_WRECT s;
	WRect r;

	ConvertSourceRectByTextureSize(s, src);
	r.x = dst.x;
	r.y = dst.y;
	r.w = dst.w;
	r.h = dst.h;
	DrawTexture(pView, m_texHandle, s, ConvertRect(pView, r, m_coordMode), flag,
		color, rot, mirror, true);
}

void WOverlay::ArcClipRender(WView* pView, const _WRECT& src, const _WRECT& dst,
	float a0, float a1, int flag, unsigned long color)
{
	_WRECT s;
	WRect r;

	ConvertSourceRectByTextureSize(s, src);
	r.x = dst.x;
	r.y = dst.y;
	r.w = dst.w;
	r.h = dst.h;
	DrawArcClipTexture(pView, m_texHandle, s,
		ConvertRect(pView, r, m_coordMode), a0, a1, flag, color);
}

void WOverlay::DrawTextureWithAxis(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, float ax, float ay, float rot, int flag,
	unsigned long color, unsigned char mirror)
{
	float mag;
	if (rot < 0.0f)
		mag = -rot;
	else
		mag = rot;
	if (mag < g_EPSILON)
	{
		DrawTexture(pView, tex, src, dst, flag, color, 0.0f, mirror, true);
		return;
	}

	WTVertex clipped[8];
	WTVertex poly[8];
	int count;

	unsigned int mu = ~(unsigned int)mirror & 1;
	unsigned int mv = ~(unsigned int)(mirror >> 1) & 1;
	for (int i = 0; i < 4; ++i)
	{
		float py;
		if ((m_coordMode & 0x400) == 0)
		{
			float px;
			if (i & 1)
				px = dst.w + dst.x;
			else
				px = dst.x;
			m_vtx[i].x = px - 0.5f;
			if (i & 2)
				py = dst.h + dst.y;
			else
				py = dst.y;
		}
		else
		{
			float px;
			if (i & 1)
				px = dst.w + dst.x;
			else
				px = dst.x;
			m_vtx[i].x = (float)floor((double)px) - 0.5f;
			float t;
			if (i & 2)
				t = dst.h + dst.y;
			else
				t = dst.y;
			py = (float)floor((double)t);
		}
		m_vtx[i].diffuse = color;
		m_vtx[i].y = py - 0.5f;
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		float u;
		if ((unsigned int)(i & 1) == mu)
			u = src.w + src.x;
		else
			u = src.x;
		m_vtx[i].tu = u;
		float v;
		if ((unsigned int)((i >> 1) & 1) == mv)
			v = src.h + src.y;
		else
			v = src.y;
		m_vtx[i].tv = v;
	}

	float cs = (float)cos((double)rot);
	float sn = (float)sin((double)rot);
	unsigned char code = 0;
	for (int k = 0; k < 4; ++k)
	{
		float ox = m_vtx[k].x - ax;
		float oy = m_vtx[k].y - ay;
		m_vtx[k].x = (ox * cs - oy * sn) + ax;
		m_vtx[k].y = oy * cs + ox * sn + ay;
		if (m_vtx[k].x < 0.0f)
			code = (unsigned char)(code | 1);
		if (pView->GetWidth() < m_vtx[k].x)
			code = (unsigned char)(code | 2);
		if (m_vtx[k].y < 0.0f)
			code = (unsigned char)(code | 4);
		if (pView->GetHeight() < m_vtx[k].y)
			code = (unsigned char)(code | 8);
	}

	poly[0] = m_vtx[0];
	poly[1] = m_vtx[1];
	poly[2] = m_vtx[3];
	poly[3] = m_vtx[2];
	count = 4;

	if (code & 1)
	{
		clip_2D_left(pView, clipped, poly, &count, count);
		for (int c = 0; c < count; ++c)
			poly[c] = clipped[c];
	}
	if (code & 2)
	{
		clip_2D_right(pView, clipped, poly, &count, count);
		for (int c = 0; c < count; ++c)
			poly[c] = clipped[c];
	}
	if (code & 4)
	{
		clip_2D_top(pView, clipped, poly, &count, count);
		for (int c = 0; c < count; ++c)
			poly[c] = clipped[c];
	}
	if (code & 8)
	{
		clip_2D_bottom(pView, clipped, poly, &count, count);
		for (int c = 0; c < count; ++c)
			poly[c] = clipped[c];
	}

	if (code == 0)
	{
		pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20300000, 0x400,
			1);
	}
	else if (count != 0)
	{
		int m = 0;
		if (count > 0)
		{
			WTVertex* p = poly;
			do
			{
				m_vtxList[m] = p;
				p->rhw = 1.0f;
				m_vtxList[m]->diffuse = color;
				m_vtxList[m]->z = 0.001f;
				++m;
				++p;
			} while (m < count);
		}
		m_vtxList[m] = 0;
		pView->DrawPolygonFan(m_vtxList, (tex & 0x7ff) | flag | 0x20300000, 0,
			1);
	}
}

void WOverlay::RenderWithAxis(WView* pView, const _WRECT& src,
	const _WRECT& dst, float ax, float ay, float rot, int flag,
	unsigned long color, unsigned char mirror)
{
	_WRECT s;
	WRect r;

	ConvertSourceRectByTextureSize(s, src);
	r.x = dst.x;
	r.y = dst.y;
	r.w = dst.w;
	r.h = dst.h;
	DrawTextureWithAxis(pView, m_texHandle, s,
		ConvertRect(pView, r, m_coordMode), ax, ay, rot, flag, color, mirror);
}
