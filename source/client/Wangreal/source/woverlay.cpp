#include "wmemblock.inl"
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
	: m_texHandle(0),
	  m_texWidth(0),
	  m_texHeight(0),
	  m_devTexWidth(0),
	  m_devTexHeight(0),
	  m_clipFlag(false),
	  m_coordMode(0x2200)
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

int WOverlay::CrossRect(const WRect& a, const WRect& b, WRect* out)
{
	float bx = b.x;
	float ax = a.x;
	float x = Max(ax, bx);
	out->x = x;
	float by = b.y;
	float ay = a.y;
	float y = Max(ay, by);
	out->y = y;

	float br = b.w + b.x;
	float ar = a.w + a.x;
	float r = Min(ar, br);
	out->w = r - x;

	float bb = b.h + b.y;
	float ab = a.h + a.y;
	float bt = Min(ab, bb);
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
				WRect full(0.0f, 0.0f, pView->GetWidth(), pView->GetHeight());
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
		m_vtx[i].y = py - 0.5f;
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
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

void WOverlay::DrawTexture(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, int flag, unsigned long color, float rot,
	unsigned char mirror, bool clip)
{
	WRect _src(src);
	WRect _dest;
	WRect _clipArea;

	if (m_coordMode & 0x400)
	{
		_dest.x = floor(dst.x);
		_dest.y = floor(dst.y);
		_dest.w = dst.w;
		_dest.h = dst.h;
		if (m_clipFlag)
		{
			_clipArea.x = floor(m_clipArea.x);
			_clipArea.y = floor(m_clipArea.y);
			_clipArea.w = m_clipArea.w;
			_clipArea.h = m_clipArea.h;
		}
	}
	else
	{
		_dest = WRect(dst);
		bool cf = m_clipFlag;
		if (cf)
			_clipArea = m_clipArea;
		if (pView->xGetProjScale() > 1.0f)
		{
			pView->xConvScreenRectByProjScale(_dest);
			if (cf)
				pView->xConvScreenRectByProjScale(_clipArea);
		}
	}
	if (clip == true)
	{
		WRect out;
		WRect screen(0.0f, 0.0f, pView->GetWidth(), pView->GetHeight());
		if (CrossRect(_dest, screen, &out) != 0)
			return;
		if (m_clipFlag && CrossRect(_dest, _clipArea, &out) != 0)
			return;
		if (_dest.w != out.w || _dest.h != out.h)
		{
			WRect src_cliped;
			float ratio;
			if (_dest.w != out.w)
			{
				float delta;
				ratio = src.w / _dest.w;
				delta = out.x - _dest.x;
				src_cliped.x = delta * ratio + src.x;
				src_cliped.w =
					(((out.x + out.w) - (_dest.w + _dest.x)) - delta) * ratio +
					src.w;
			}
			else
			{
				src_cliped.x = src.x;
				src_cliped.w = src.w;
			}
			if (_dest.h != out.h)
			{
				float delta;
				ratio = src.h / _dest.h;
				delta = out.y - _dest.y;
				src_cliped.y = delta * ratio + src.y;
				src_cliped.h =
					(((out.y + out.h) - (_dest.h + _dest.y)) - delta) * ratio +
					src.h;
			}
			else
			{
				src_cliped.y = src.y;
				src_cliped.h = src.h;
			}
			_src.x = src_cliped.x;
			_src.y = src_cliped.y;
			_src.w = src_cliped.w;
			_src.h = src_cliped.h;
			_dest = out;
		}
	}
	float sx, sy, ex, ey;
	if (m_coordMode & 0x400)
	{
		sx = _dest.x - 0.5f;
		ex = sx + _dest.w;
		sy = _dest.y - 0.5f;
		ey = sy + _dest.h;
	}
	for (int i = 0; i < 4; ++i)
	{
		if (m_coordMode & 0x400)
		{
			float px;
			if (i & 1)
				px = ex;
			else
				px = sx;
			m_vtx[i].x = px;
			float py;
			if (i & 2)
				py = ey;
			else
				py = sy;
			m_vtx[i].y = py;
		}
		else
		{
			float px;
			if (i & 1)
				px = _dest.w + _dest.x;
			else
				px = _dest.x;
			m_vtx[i].x = px - 0.5f;
			float py;
			if (i & 2)
				py = _dest.h + _dest.y;
			else
				py = _dest.y;
			m_vtx[i].y = py - 0.5f;
		}
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
		float u;
		if ((unsigned int)(i & 1) == (~(unsigned int)mirror & 1))
			u = _src.w + _src.x;
		else
			u = _src.x;
		m_vtx[i].tu = u;
		float v;
		if ((((unsigned int)i >> 1) & 1) == (~((unsigned int)mirror >> 1) & 1))
			v = _src.h + _src.y;
		else
			v = _src.y;
		m_vtx[i].tv = v;
	}
	float mag;
	if (*(unsigned int*)&rot & 0x80000000)
		mag = -rot;
	else
		mag = rot;
	if (!(mag < g_EPSILON))
	{
		float cosine = cos(rot);
		float sine = sin(rot);
		float px = _dest.w * 0.5f + _dest.x;
		float py = _dest.h * 0.5f + _dest.y;
		for (int j = 0; j < 4; ++j)
		{
			float tx = m_vtx[j].x - px;
			float ty = m_vtx[j].y - py;
			m_vtx[j].x = (tx * cosine - ty * sine) + px;
			m_vtx[j].y = ty * cosine + tx * sine + py;
		}
	}
	pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20300000, 0, 1);
}

void WOverlay::DrawTextureWithAxis(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, float ax, float ay, float rot, int flag,
	unsigned long color, unsigned char mirror)
{
	float mag;
	if (*(unsigned int*)&rot & 0x80000000)
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

	for (int i = 0; i < 4; ++i)
	{
		if (m_coordMode & 0x400)
		{
			m_vtx[i].x = floorf((i & 1) ? dst.w + dst.x : dst.x) - 0.5f;
			m_vtx[i].y = floorf((i & 2) ? dst.h + dst.y : dst.y) - 0.5f;
		}
		else
		{
			m_vtx[i].x = ((i & 1) ? dst.w + dst.x : dst.x) - 0.5f;
			m_vtx[i].y = ((i & 2) ? dst.h + dst.y : dst.y) - 0.5f;
		}
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
		float u;
		if ((unsigned int)(i & 1) == (~(unsigned int)mirror & 1))
			u = src.w + src.x;
		else
			u = src.x;
		m_vtx[i].tu = u;
		float v;
		if ((((unsigned int)i >> 1) & 1) == (~((unsigned int)mirror >> 1) & 1))
			v = src.h + src.y;
		else
			v = src.y;
		m_vtx[i].tv = v;
	}

	float cs = cos(rot);
	float sn = sin(rot);
	int code = 0;
	for (int k = 0; k < 4; ++k)
	{
		float ox = m_vtx[k].x - ax;
		float oy = m_vtx[k].y - ay;
		m_vtx[k].x = (ox * cs - oy * sn) + ax;
		m_vtx[k].y = oy * cs + ox * sn + ay;
		if (m_vtx[k].x < 0.0f)
			code |= 1;
		if (m_vtx[k].x > pView->GetWidth())
			code |= 2;
		if (m_vtx[k].y < 0.0f)
			code |= 4;
		if (m_vtx[k].y > pView->GetHeight())
			code |= 8;
	}

	poly[0] = m_vtx[0];
	poly[1] = m_vtx[1];
	poly[2] = m_vtx[3];
	count = 4;
	poly[3] = m_vtx[2];

	if (code & 1)
	{
		clip_2D_left(pView, clipped, poly, &count, count);
		memcpy(poly, clipped, sizeof(WTVertex) * count);
	}
	if (code & 2)
	{
		clip_2D_right(pView, clipped, poly, &count, count);
		memcpy(poly, clipped, sizeof(WTVertex) * count);
	}
	if (code & 4)
	{
		clip_2D_top(pView, clipped, poly, &count, count);
		memcpy(poly, clipped, sizeof(WTVertex) * count);
	}
	if (code & 8)
	{
		clip_2D_bottom(pView, clipped, poly, &count, count);
		memcpy(poly, clipped, sizeof(WTVertex) * count);
	}

	if (code != 0)
	{
		if (count != 0)
		{
			int m;
			for (m = 0; m < count; ++m)
			{
				m_vtxList[m] = &poly[m];
				m_vtxList[m]->rhw = 1.0f;
				m_vtxList[m]->diffuse = color;
				m_vtxList[m]->z = 0.001f;
			}
			m_vtxList[m] = 0;
			pView->DrawPolygonFan(m_vtxList, (tex & 0x7ff) | flag | 0x20300000,
				0, 1);
		}
	}
	else
	{
		pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20300000, 0x400,
			1);
	}
}

void WOverlay::DrawArcClipTexture(WView* pView, int tex, const _WRECT& src,
	const _WRECT& dst, float a0, float a1, int flag, unsigned long color)
{
	WTVertex vEnd;
	WTVertex vStart;
	WTVertex center;

	for (int i = 0; i < 4; ++i)
	{
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = color;
		m_vtx[i].tu = (i & 1) ? src.x : src.x + src.w;
		m_vtx[i].tv = (i & 2) ? src.y + src.h : src.y;
		float x = (i & 1) ? dst.x : dst.w + dst.x;
		m_vtx[i].x = x - 0.5f;
		float y = (i & 2) ? dst.h + dst.y : dst.y;
		m_vtx[i].y = y - 0.5f;
	}
	m_clipArea.x = dst.w * 0.5f + dst.x;
	m_clipArea.y = dst.h * 0.5f + dst.y;
	m_clipArea.w = dst.w * 0.5f;
	m_clipArea.h = dst.h * 0.5f;
	float ratio = dst.h / dst.w;
	float ref = (atanf(ratio) * g_PI) / g_PI;
	vStart.z = 0.001f;
	vStart.rhw = 1.0f;
	vStart.diffuse = color;
	vEnd.z = 0.001f;
	vEnd.rhw = 1.0f;
	vEnd.diffuse = color;
	center.z = 0.001f;
	center.rhw = 1.0f;
	center.diffuse = color;
	center.tu = 0.5f;
	center.tv = 0.5f;
	center.x = dst.w * 0.5f + dst.x;
	center.y = dst.h * 0.5f + dst.y;
	int i0 = GetSection(&vStart, ref, a0);
	int i1 = GetSection(&vEnd, ref, a1);
	m_vtxList[0] = &center;
	m_vtxList[1] = &vStart;
	int n = 0;
	if (a0 < a1)
	{
		for (; n < (i1 - i0) / 2; ++n)
			m_vtxList[n + 2] = m_vl[i0 / 2 + n];
	}
	else
	{
		for (; n < (i1 - i0 + 8) / 2; ++n)
			m_vtxList[n + 2] = m_vl[(i0 / 2 + n) % 4];
	}

	m_vtxList[n + 2] = &vEnd;
	m_vtxList[n + 3] = 0;
	pView->DrawPolygonFan(m_vtxList, (tex & 0x7ff) | flag | 0x20300000, 0x800,
		1);
}

int WOverlay::GetSection(WTVertex* v, float base, float angle)
{
	if (angle < base)
	{
		v->x = m_clipArea.x + m_clipArea.w;
		base = tanf(angle);
		v->y = m_clipArea.y - base * m_clipArea.w;
		v->tu = 1.0f;
		angle = tanf(angle);
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
		if (p->x * in[j % n].x < 0.0f)
		{
			WTVertex* q = in + j % n;
			float px = p->x;
			d0 = Abs(px);
			float qx = q->x;
			d1 = Abs(qx);
			float px2 = p->x;
			d2 = Abs(px2);
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
	float d1, d0;
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
		if ((float)((p->x - pView->GetWidth())) *
				(in[j % n].x - pView->GetWidth()) <
			0.0f)
		{
			WTVertex* q = &in[j % n];
			float px0 = p->x - pView->GetWidth();
			d0 = Abs(px0);
			float qx = q->x - pView->GetWidth();
			d1 = Abs(qx);
			float px2 = p->x - pView->GetWidth();
			float d2 = Abs(px2);
			float ratio = d0 / (d2 + d1);
			out[*count].x = pView->GetWidth();
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
		if (p->y * in[j % n].y < 0.0f)
		{
			WTVertex* q = in + j % n;
			float py = p->y;
			d0 = Abs(py);
			float qy = q->y;
			d1 = Abs(qy);
			float py2 = p->y;
			d2 = Abs(py2);
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
			d0 = Abs(py0);
			float qy = q->y - pView->GetHeight();
			d1 = Abs(qy);
			float py2 = p->y - pView->GetHeight();
			float d2 = Abs(py2);
			float ratio = d0 / (d2 + d1);
			out[*count].x = (q->x - p->x) * ratio + p->x;
			out[*count].y = pView->GetHeight();
			out[*count].tu = (q->tu - p->tu) * ratio + p->tu;
			out[*count].tv = (q->tv - p->tv) * ratio + p->tv;
			*count = *count + 1;
		}
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
	float px = rc.x;
	m_vtx[0].x = px - 0.5f;
	float py = rc.y;
	m_vtx[0].z = z;
	m_vtx[0].diffuse = color;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].y = py - 0.5f;

	px = rc.w + rc.x;
	m_vtx[1].x = px - 0.5f;
	py = rc.y;
	m_vtx[1].z = z;
	m_vtx[1].diffuse = color;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].y = py - 0.5f;

	px = rc.x;
	m_vtx[2].x = px - 0.5f;
	py = rc.h + rc.y;
	m_vtx[2].z = z;
	m_vtx[2].diffuse = color;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].y = py - 0.5f;

	px = rc.w + rc.x;
	m_vtx[3].x = px - 0.5f;
	py = rc.h + rc.y;
	m_vtx[3].z = z;
	m_vtx[3].diffuse = color;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].y = py - 0.5f;
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
		m_vtx[i].z = z;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].y = py - 0.5f;
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
	WRect r(rc);
	WPoint lt(r.x, r.y);
	WPoint rt(r.Right(), r.y);
	WPoint lb(r.x, r.Bottom());
	WPoint rb(r.Right(), r.Bottom());

	DrawLine(pView, lt, rt, z | 0x300000, color);
	DrawLine(pView, rt, rb, z | 0x300000, color);
	DrawLine(pView, rb, lb, z | 0x300000, color);
	DrawLine(pView, lb, lt, z | 0x300000, color);
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
		ox = (mode & 2 ? 0.5f : 1.0f) * pView->GetWidth();
	}

	float oy;
	if (!(mode & 0x60))
	{
		oy = 0.0f;
	}
	else
	{
		oy = (mode & 0x20 ? 0.5f : 1.0f) * pView->GetHeight();
	}

	WRect ret;
	if (mode & 0x100)
	{
		ret.x = (pView->GetWidth() * rc.x) / 640.0f + ox;
		ret.y = (pView->GetHeight() * rc.y) / 480.0f + oy;
	}
	else
	{
		ret.x = ox + rc.x;
		ret.y = oy + rc.y;
	}

	if (mode & 0x1000)
	{
		ret.w = (pView->GetWidth() * rc.w) / 640.0f;
		ret.h = (pView->GetHeight() * rc.h) / 480.0f;
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
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
	}
	pView->DrawPolygonFan(m_vl, (tex & 0x7ff) | flag | 0x20000000, 0, 1);
}

void WOverlay::DrawFrameOverlay1(WView* view, WOverlay* frame, float sizeL,
	float sizeT, float sizeR, float sizeB, const RECT& rect,
	unsigned long color)
{
	float sizeLR = sizeL + sizeR;
	float sizeTB = sizeT + sizeB;
	float xoffset = 0.5f / frame->GetWidth();
	float yoffset = 0.5f / frame->GetHeight();
	float inv_w = 1.0f / frame->GetWidth();
	float inv_h = 1.0f / frame->GetHeight();
	WRect offsetRect(xoffset, yoffset, -xoffset, -yoffset);
	float x = (float)rect.left;
	float y = (float)rect.top;
	float w = (float)rect.right;
	float h = (float)rect.bottom;
	_WRECT frameRect[9];
	frameRect[0] = WRect(0.0f, 0.0f, sizeL, sizeT);
	frameRect[1] = WRect(sizeL, 0.0f, (float)frame->GetWidth() - sizeLR, sizeT);
	frameRect[2] = WRect((float)frame->GetWidth() - sizeR, 0.0f, sizeR, sizeT);
	frameRect[3] =
		WRect(0.0f, sizeB, sizeL, (float)frame->GetHeight() - sizeTB);
	frameRect[4] = WRect(sizeL, sizeB, (float)frame->GetWidth() - sizeLR,
		(float)frame->GetHeight() - sizeTB);
	frameRect[5] = WRect((float)frame->GetWidth() - sizeR, sizeB, sizeR,
		(float)frame->GetHeight() - sizeTB);
	frameRect[6] = WRect(0.0f, (float)frame->GetHeight() - sizeB, sizeL, sizeB);
	frameRect[7] = WRect(sizeL, (float)frame->GetHeight() - sizeB,
		(float)frame->GetWidth() - sizeLR, sizeB);
	frameRect[8] = WRect((float)frame->GetWidth() - sizeR,
		(float)frame->GetHeight() - sizeB, sizeR, sizeB);

	_WRECT srcRect[9];
	for (int i = 0; i < 9; ++i)
	{
		srcRect[i].x = frameRect[i].x * inv_w + offsetRect.x;
		srcRect[i].y = frameRect[i].y * inv_h + offsetRect.y;
		srcRect[i].w = frameRect[i].w * inv_w + offsetRect.w;
		srcRect[i].h = frameRect[i].h * inv_h + offsetRect.h;
	}

	frame->Render(view, srcRect[0], WRect(x, y, sizeL, sizeT), 0x2000000, color,
		0.0f, 0);
	frame->Render(view, srcRect[1], WRect(x + sizeL, y, w - sizeLR, sizeT),
		0x2000000, color, 0.0f, 0);
	frame->Render(view, srcRect[2],
		WRect(float(x + w) - sizeR, y, sizeR, sizeT), 0x2000000, color, 0.0f,
		0);
	frame->Render(view, srcRect[3], WRect(x, y + sizeT, sizeL, h - sizeTB),
		0x2000000, color, 0.0f, 0);
	frame->Render(view, srcRect[4],
		WRect(x + sizeL, y + sizeT, w - sizeLR, h - sizeTB), 0x2000000, color,
		0.0f, 0);
	frame->Render(view, srcRect[5],
		WRect(float(x + w) - sizeR, y + sizeT, sizeR, h - sizeTB), 0x2000000,
		color, 0.0f, 0);
	frame->Render(view, srcRect[6],
		WRect(x, float(y + h) - sizeB, sizeL, sizeB), 0x2000000, color, 0.0f,
		0);
	frame->Render(view, srcRect[7],
		WRect(x + sizeL, float(y + h) - sizeB, w - sizeLR, sizeB), 0x2000000,
		color, 0.0f, 0);
	frame->Render(view, srcRect[8],
		WRect(float(x + w) - sizeR, float(y + h) - sizeB, sizeR, sizeB),
		0x2000000, color, 0.0f, 0);
}

void WOverlay::DrawFrameOverlay9(WView* view, WOverlay** const frames,
	const RECT& rect, unsigned long color)
{
	float sizeL = (float)frames[3]->GetWidth();
	float sizeR = (float)frames[5]->GetWidth();
	float sizeT = (float)frames[1]->GetHeight();
	float sizeB = (float)frames[7]->GetHeight();
	float sizeLR = sizeR + sizeL;
	float sizeTB = sizeB + sizeT;
	float x = (float)rect.left;
	float y = (float)rect.top;
	float w = (float)rect.right;
	float h = (float)rect.bottom;
	_WRECT srcRect[9];
	for (int i = 0; i < 9; ++i)
		srcRect[i] = WRect(0.0f, 0.0f, 1.0f, 1.0f);

	frames[0]->Render(view, srcRect[0], WRect(x, y, sizeL, sizeT), 0x2080000,
		color, 0.0f, 0);
	frames[1]->Render(view, srcRect[1], WRect(x + sizeL, y, w - sizeLR, sizeT),
		0x2080000, color, 0.0f, 0);
	frames[2]->Render(view, srcRect[2],
		WRect(float(x + w) - sizeR, y, sizeR, sizeT), 0x2080000, color, 0.0f,
		0);
	frames[3]->Render(view, srcRect[3], WRect(x, y + sizeT, sizeL, h - sizeTB),
		0x2080000, color, 0.0f, 0);
	frames[4]->Render(view, srcRect[4],
		WRect(x + sizeL, y + sizeT, w - sizeLR, h - sizeTB), 0x2080000, color,
		0.0f, 0);
	frames[5]->Render(view, srcRect[5],
		WRect(float(x + w) - sizeR, y + sizeT, sizeR, h - sizeTB), 0x2080000,
		color, 0.0f, 0);
	frames[6]->Render(view, srcRect[6],
		WRect(x, float(y + h) - sizeB, sizeL, sizeB), 0x2080000, color, 0.0f,
		0);
	frames[7]->Render(view, srcRect[7],
		WRect(x + sizeL, float(y + h) - sizeB, w - sizeLR, sizeB), 0x2080000,
		color, 0.0f, 0);
	frames[8]->Render(view, srcRect[8],
		WRect(float(x + w) - sizeR, float(y + h) - sizeB, sizeR, sizeB),
		0x2080000, color, 0.0f, 0);
}
