#include "stdafx.h"
#include "uccbase.h"
#include "uccdrawdlg.h"
#include "frarea.h"
#include "taskmanager.h"
#include "wresrcmng.h"
#include <list>

static int drawCount;

void CDrawHistory::Add(Bitmap* before, Bitmap* after, IPoint min, IPoint max)
{
	if (!before || !after)
		return;
	if (before->BitsPerPixel() != after->BitsPerPixel() ||
		before->Width() != after->Width() ||
		before->Height() != after->Height())
		return;
	if (m_history.size())
	{
		if (m_current + 1 < m_history.size())
		{
			unsigned i = m_current + 1;
			while (i < m_history.size())
			{
				delete m_history[i];
				++i;
			}
			m_history.resize(m_current + 1);
		}
	}
	min.x = Max(0, min.x - 12);
	min.y = Max(0, min.y - 12);
	max.x = Min((int)before->Width(), max.x + 12);
	max.y = Min((int)before->Height(), max.y + 12);
	sDrawHistory* history = new sDrawHistory;
	history->x = min.x;
	history->y = min.y;
	history->width = max.x - min.x;
	history->height = max.y - min.y;
	history->undo.Create(history->width, history->height,
		before->BitsPerPixel());
	history->redo.Create(history->width, history->height,
		before->BitsPerPixel());
	CopyBitmap(before, &history->undo, history->x, history->y, history->width,
		history->height, 0, 0);
	CopyBitmap(after, &history->redo, history->x, history->y, history->width,
		history->height, 0, 0);
	m_history.push_back(history);
	++m_current;
	int excess = m_history.size() - 30;
	if (excess > 0)
	{
		m_current -= excess;
		for (int i = 0; i < excess; ++i)
			delete m_history[i];
		for (unsigned i = excess; i < m_history.size(); ++i)
			m_history[i - excess] = m_history[i];
		for (unsigned i = 30; i < m_history.size(); ++i)
			m_history[i] = NULL;
		m_history.resize(30);
	}
}

void CDrawHistory::Clear()
{
	for (unsigned i = 0; i < m_history.size(); ++i)
		delete m_history[i];
	m_history.clear();
	m_current = -1;
}

void CDrawHistory::Undo(Bitmap* bitmap)
{
	if (bitmap && IsUndoPossible())
	{
		Draw(bitmap, m_history[m_current], false);
		--m_current;
	}
}

void CDrawHistory::Redo(Bitmap* bitmap)
{
	if (!bitmap)
		return;
	if (!(m_current >= -1 && m_current < (int)m_history.size() - 1 &&
			m_history.size()))
		return;
	++m_current;
	Draw(bitmap, m_history[m_current], true);
}

bool CDrawHistory::IsUndoPossible()
{
	return m_current >= 0 && m_current < m_history.size() && m_history.size();
}

bool CDrawHistory::IsRedoPossible()
{
	return m_current >= -1 && m_current < (int)m_history.size() - 1 &&
		m_history.size();
}

void CDrawHistory::Draw(Bitmap* bitmap, sDrawHistory* history, bool redo)
{
	if (!bitmap || !history)
		return;
	CopyBitmap(redo ? &history->redo : &history->undo, bitmap, 0, 0,
		history->width, history->height, history->x, history->y);
}

void CDrawHistory::CopyBitmap(Bitmap* src, Bitmap* dst, int sx, int sy, int w,
	int h, int dx, int dy)
{
	if (!src || !dst)
		return;
	if (src->BitsPerPixel() != dst->BitsPerPixel())
		return;
	if (sx + w > src->Width() || sy + h > src->Height() ||
		dx + w > dst->Width() || dy + h > dst->Height())
		return;
	int bytes = src->BitsPerPixel() / 8;
	for (int y = sy; y < sy + h; ++y)
		memcpy(dst->vram + (y - sy + dy) * dst->pitch + dx * bytes,
			src->vram + y * src->pitch + sx * bytes, w * bytes);
}

CUccBase::CUccBase()
{
	InitVariables();
}

CUccBase::~CUccBase()
{
	DestroyVariables();
}

void CUccBase::InitVariables()
{
	m_pTexture = NULL;
	m_pMask = NULL;
	m_pArea = NULL;
	m_pTexture32 = NULL;
	m_pBackup = NULL;
	m_pBackup32 = NULL;
	m_pDefaultBrush = NULL;
	m_bDragging = false;
	m_refresh = 0;
	m_lastPos.x = -1;
	m_lastPos.y = -1;
	m_zoomRate = 1;
	m_pZoomed = NULL;
	m_mode = MODE_DRAW;
	m_pParent = NULL;
}

void CUccBase::DestroyVariables()
{
	ClearTexCache(m_pTexture);
	if (m_pTexture)
		delete m_pTexture;
	m_pTexture = NULL;
	ClearTexCache(m_pMask);
	if (m_pMask)
		delete m_pMask;
	m_pMask = NULL;
	ClearTexCache(m_pTexture32);
	if (m_pTexture32)
		delete m_pTexture32;
	m_pTexture32 = NULL;
	ClearTexCache(m_pBackup);
	if (m_pBackup)
		delete m_pBackup;
	m_pBackup = NULL;
	ClearTexCache(m_pBackup32);
	if (m_pBackup32)
		delete m_pBackup32;
	m_pBackup32 = NULL;
	ClearTexCache(m_pZoomed);
	if (m_pZoomed)
		delete m_pZoomed;
	m_pZoomed = NULL;
}

void CUccBase::Initialize(FrArea* area, Bitmap* texture, Bitmap* mask)
{
	if (!m_pArea && area)
		SetArea(area);
	if (!m_pTexture && texture && m_pArea)
		SetTexture(texture);
	if (!m_pMask && mask)
		SetMask(mask);
	m_refresh = 7;
}

void CUccBase::Process()
{
	if (m_refresh & 1)
		Refresh2D();
	if (m_refresh & 2)
		Refresh3D();
	if (m_refresh & 4)
		RefreshCache();
}

void CUccBase::MouseEventHandler(eButton button, int event, const WPoint& pos)
{
	switch (m_mode)
	{
	case MODE_DRAW:
		DrawEventHandler(button, event, pos);
		break;
	case MODE_DRAG:
		MoveEventHandler(button, event, pos);
		break;
	case MODE_SPOID:
		SpoidEventHandler(button, event, pos);
		break;
	case MODE_PAINT:
		PaintEventHandler(button, event, pos);
		break;
	case MODE_ZOOMIN:
	case MODE_ZOOMOUT:
		ZoomEventHandler(button, event, pos);
		break;
	}
}

void CUccBase::Refresh2D()
{
	Convert24to32(m_pTexture, m_pTexture32, m_pMask);
	if (m_pArea && m_pTexture32)
		m_pArea->SetBgImg(m_pTexture32);
	m_refresh &= ~1;
}

void CUccBase::Refresh3D()
{
	m_refresh &= ~2;
}

void CUccBase::RefreshCache()
{
	GetGDI();
	if (m_pTexture32)
		RefreshTexCache(m_pTexture32);
	if (IsZoomed() && m_pZoomed)
		RefreshTexCache(m_pZoomed);
	m_refresh &= ~4;
}

void CUccBase::Draw(int x, int y, Bitmap* brush, bool first)
{
	if (!brush)
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xba\xea\xb7\xaf\xbd\xac \xc1\xa4\xba\xb8\xb8\xa6 \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n\n\xc6\xce\xbe\xdf \xb0\xd4\xc0\xd3\xc0\xbb \xbf\xcf\xc0\xfc\xc8\xf7 \xc1\xbe\xb7\xe1\xc7\xcf\xbc\xcc\xb4\xd9\xb0\xa1 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
			0, 0, 0, 0));
		return;
	}
	if (!IsInitialized())
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc3\xca\xb1\xe2\xc8\xad\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xa9 \xb1\xd7\xb8\xb1 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n\n\xc6\xce\xbe\xdf \xb0\xd4\xc0\xd3\xc0\xbb \xbf\xcf\xc0\xfc\xc8\xf7 \xc1\xbe\xb7\xe1\xc7\xcf\xbc\xcc\xb4\xd9\xb0\xa1 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
			0, 0, 0, 0));
		return;
	}
	if (IsZoomed())
	{
		x = (int)(x / GetZoomRate() + m_zoomArea.x);
		y = (int)(y / GetZoomRate() + m_zoomArea.y);
	}
	if (x - (int)brush->Width() / 2 < m_min.x)
		m_min.x = Max(0, x - (int)brush->Width() / 2);
	if (x + (int)brush->Width() / 2 > m_max.x)
		m_max.x =
			Min((int)m_pTexture->Width() - 1, x + (int)brush->Width() / 2);
	if (y - (int)brush->Height() / 2 < m_min.y)
		m_min.y = Max(0, y - (int)brush->Height() / 2);
	if (y + (int)brush->Height() / 2 > m_max.y)
		m_max.y =
			Min((int)m_pTexture->Height() - 1, y + (int)brush->Height() / 2);
	int width = brush->Width(), height = brush->Height();
	IPoint start((int)(x - brush->Width() / 2), (int)(y - brush->Height() / 2));
	IPoint end(start.x + width, start.y + height);
	int startX = Max(start.x, 0), startY = Max(start.y, 0);
	sRGBA color, brushColor, mask, old, result, shaded;
	UccCurrentBrush()->GetBrushColor(color.r, color.g, color.b);
	color.a = 255;
	++drawCount;
	for (int yy = startY; yy < end.y; ++yy)
	{
		if (yy >= m_pTexture->Height())
			break;
		for (int xx = startX; xx < end.x; ++xx)
		{
			if (xx >= m_pTexture->Width())
				break;
			if (m_pMask)
			{
				m_pMask->GetPixel(xx, yy, mask.r, mask.g, mask.b);
				if (mask.r < 16 && mask.g < 16 && mask.b < 16)
					continue;
			}
			else
			{
				mask.r = 0;
				mask.g = 0;
				mask.b = 0;
			}
			brush->GetPixel(xx - start.x, yy - start.y, brushColor.r,
				brushColor.g, brushColor.b, brushColor.a);
			if (brushColor.a <= 0)
				continue;
			m_pTexture32->GetPixel(xx, yy, old.r, old.g, old.b, old.a);
			sRGBA mixed;
			MixColor(old, brushColor, mixed);
			SetBrightness(mixed, mask, shaded);
			MixColor(old, brushColor, result);
			if (m_pMask)
			{
				m_pTexture->SetPixel(xx, yy, shaded.r, shaded.g, shaded.b, 255);
				m_pTexture32->SetPixel(xx, yy, result.r, result.g, result.b,
					result.a);
				if (IsZoomed() && m_pZoomed)
				{
					IPoint begin;
					begin.x = Max(0, (int)((xx - m_zoomArea.x) * m_zoomRate));
					begin.y = Max(0, (int)((yy - m_zoomArea.y) * m_zoomRate));
					IPoint end;
					end.x = Min((int)m_pZoomed->Width(), begin.x + m_zoomRate);
					end.y = Min((int)m_pZoomed->Height(), begin.y + m_zoomRate);
					for (int zy = begin.y; zy < end.y; ++zy)
						for (int zx = begin.x; zx < end.x; ++zx)
							m_pZoomed->SetPixel(zx, zy, result.r, result.g,
								result.b, result.a);
				}
			}
			else
			{
				m_pTexture->SetPixel(xx, yy, result.r, result.g, result.b, 255);
				m_pTexture32->SetPixel(xx, yy, result.r, result.g, result.b,
					result.a);
				if (IsZoomed() && m_pZoomed)
				{
					IPoint begin;
					begin.x = Max(0, (int)((xx - m_zoomArea.x) * m_zoomRate));
					begin.y = Max(0, (int)((yy - m_zoomArea.y) * m_zoomRate));
					IPoint end;
					end.x = Min((int)m_pZoomed->Width(), begin.x + m_zoomRate);
					end.y = Min((int)m_pZoomed->Height(), begin.y + m_zoomRate);
					for (int zy = begin.y; zy < end.y; ++zy)
						for (int zx = begin.x; zx < end.x; ++zx)
							m_pZoomed->SetPixel(zx, zy, result.r, result.g,
								result.b, result.a);
				}
			}
		}
	}
	if (first)
		MustRefresh(4);
}

void CUccBase::DrawLine(IPoint from, IPoint to, Bitmap* brush)
{
	if (!brush)
		return;
	int dy = abs(to.y - from.y), dx = abs(to.x - from.x), n = Max(dx, dy);
	float sx = (float)(to.x - from.x) / n, sy = (float)(to.y - from.y) / n;
	WPoint p;
	p.x = (float)from.x;
	p.y = (float)from.y;
	while (true)
	{
		Draw((int)p.x, (int)p.y, brush, false);
		if (n == dx && p.x == to.x)
			break;
		if (n == dy && p.y == to.y)
			break;
		p.x += sx;
		p.y += sy;
	}
	MustRefresh(4);
}

void CUccBase::Undo()
{
	m_history.Undo(m_pTexture);
	m_history32.Undo(m_pTexture32);
	if (IsZoomed())
		_DrawZoomedArea();
	MustRefresh(7);
}

void CUccBase::Redo()
{
	m_history.Redo(m_pTexture);
	m_history32.Redo(m_pTexture32);
	if (IsZoomed())
		_DrawZoomedArea();
	MustRefresh(7);
}

bool CUccBase::IsUndoPossible()
{
	return m_history.IsUndoPossible();
}

bool CUccBase::IsRedoPossible()
{
	return m_history.IsRedoPossible();
}

void CUccBase::SetTexture(Bitmap* texture)
{
	ClearTexCache(m_pTexture);
	if (m_pTexture)
		delete m_pTexture;
	m_pTexture = NULL;
	m_pTexture = new Bitmap;
	*m_pTexture = *texture;
	ClearTexCache(m_pTexture32);
	if (m_pTexture32)
		delete m_pTexture32;
	m_pTexture32 = NULL;
	m_pTexture32 = new Bitmap(m_pTexture->Width(), m_pTexture->Height(), 32);
	MustRefresh(1);
}

void CUccBase::SetMask(Bitmap* mask)
{
	ClearTexCache(m_pMask);
	if (m_pMask)
		delete m_pMask;
	m_pMask = NULL;
	m_pMask = new Bitmap;
	*m_pMask = *mask;
}

void CUccBase::SetDefaultBrush(Bitmap* brush)
{
	ClearTexCache(m_pDefaultBrush);
	if (m_pDefaultBrush)
		delete m_pDefaultBrush;
	m_pDefaultBrush = NULL;
	m_pDefaultBrush = new Bitmap;
	*m_pDefaultBrush = *brush;
}

bool CUccBase::IsMouseOver(const WPoint& pos)
{
	if (!m_pArea || (!m_pTexture && !m_pTexture32) || !m_pArea->IsVisible() ||
		!m_pArea->IsViewFocused())
		return false;
	WRect rect = m_pArea->GetRect();
	int width = 0, height = 0;
	if (IsZoomed())
	{
		width = m_pZoomed->Width();
		height = m_pZoomed->Height();
	}
	else if (m_pTexture)
	{
		width = m_pTexture->Width();
		height = m_pTexture->Height();
	}
	else if (m_pTexture32)
	{
		width = m_pTexture32->Width();
		height = m_pTexture32->Height();
	}
	if (rect.x <= pos.x && pos.x < rect.x + width && rect.y <= pos.y &&
		pos.y < rect.y + height)
		return true;
	return false;
}

Bitmap* CUccBase::GetTexture32BPP(bool original)
{
	if (IsZoomed() && !original)
		return m_pZoomed;
	return m_pTexture32;
}

bool CUccBase::ZoomIn(int x, int y)
{
	if (m_zoomRate >= 8)
		return false;
	++m_zoomRate;
	MakeZoomedArea(x, y);
	return true;
}

bool CUccBase::ZoomOut(int x, int y)
{
	if (m_zoomRate <= 1)
		return false;
	--m_zoomRate;
	MakeZoomedArea(x, y);
	return true;
}

void CUccBase::MakeZoomedArea(int x, int y)
{
	if (m_zoomRate == 1)
	{
		if (m_pTexture32)
		{
			WRect rect = m_pArea->GetRect();
			rect.w = (float)m_pTexture32->Width();
			rect.h = (float)m_pTexture32->Height();
			m_pArea->SetRect(rect);
			RefreshTexCache(m_pTexture32);
			m_pArea->SetBgImg(m_pTexture32);
		}
	}
	else
	{
		IPoint center;
		center.x = (int)m_pTexture32->Width() / 2;
		center.y = (int)m_pTexture32->Height() / 2;
		IPoint point;
		if (x == -1 && y == -1)
		{
			point.x = (int)(m_zoomArea.x + m_zoomArea.w * .5f);
			point.y = (int)(m_zoomArea.y + m_zoomArea.h * .5f);
		}
		else
		{
			point.x = x;
			point.y = y;
			if (point.x * m_zoomRate < 128)
				point.x = 256 / (2 * m_zoomRate);
			if ((m_pTexture->Width() - point.x) * m_zoomRate < 128)
				point.x = m_pTexture->Width() - 256 / (2 * m_zoomRate);
			if (point.y * m_zoomRate < 128)
				point.y = 256 / (2 * m_zoomRate);
			if ((m_pTexture->Height() - point.y) * m_zoomRate < 128)
				point.y = m_pTexture->Height() - 256 / (2 * m_zoomRate);
		}
		IPoint delta(point.x - center.x, point.y - center.y);
		int width = m_pTexture32->Width(), height = m_pTexture32->Height();
		if (width * m_zoomRate <= 256)
		{
			m_zoomArea.x = 0;
			m_zoomArea.w = (float)width;
		}
		else
		{
			int extra = (width * m_zoomRate - 256) / 2;
			m_zoomArea.x = (float)(extra / m_zoomRate - 1);
			m_zoomArea.w =
				(float)((width * m_zoomRate - 2 * extra) / m_zoomRate);
		}
		if (height * m_zoomRate <= 256)
		{
			m_zoomArea.y = 0;
			m_zoomArea.h = (float)height;
		}
		else
		{
			int extra = (height * m_zoomRate - 256) / 2;
			m_zoomArea.y = (float)(extra / m_zoomRate - 1);
			m_zoomArea.h =
				(float)((height * m_zoomRate - 2 * extra) / m_zoomRate);
		}
		m_zoomArea.x += delta.x;
		m_zoomArea.y += delta.y;
		if (m_zoomArea.x < 0)
			m_zoomArea.x = 0;
		if (m_zoomArea.y < 0)
			m_zoomArea.y = 0;
		if (m_zoomArea.x + m_zoomArea.w > m_pTexture32->Width())
			m_zoomArea.x = m_pTexture32->Width() - m_zoomArea.w;
		if (m_zoomArea.y + m_zoomArea.h > m_pTexture32->Height())
			m_zoomArea.y = m_pTexture32->Height() - m_zoomArea.h;
		_DrawZoomedArea();
	}
}

void CUccBase::_DrawZoomedArea()
{
	int width = Min(256, (int)(m_zoomArea.w * m_zoomRate)),
		height = Min(256, (int)(m_zoomArea.h * m_zoomRate));
	m_pArea->SetBgImg((Bitmap*)NULL);
	ClearTexCache(m_pZoomed);
	if (m_pZoomed)
		delete m_pZoomed;
	m_pZoomed = NULL;
	m_pZoomed = new Bitmap(width, height, 32);
	if (m_pZoomed)
	{
		for (int y = (int)m_zoomArea.y; y < m_zoomArea.y + m_zoomArea.h; ++y)
		{
			for (int x = (int)m_zoomArea.x; x < m_zoomArea.x + m_zoomArea.w;
				++x)
			{
				unsigned char r, g, b, a;
				m_pTexture32->GetPixel(x, y, r, g, b, a);
				IPoint minimum, maximum;
				minimum.x = (int)((x - m_zoomArea.x) * m_zoomRate);
				maximum.x = (int)((x - m_zoomArea.x + 1) * m_zoomRate);
				minimum.y = (int)((y - m_zoomArea.y) * m_zoomRate);
				maximum.y = (int)((y - m_zoomArea.y + 1) * m_zoomRate);
				for (int yy = minimum.y; yy < maximum.y; ++yy)
					for (int xx = minimum.x; xx < maximum.x; ++xx)
						m_pZoomed->SetPixel(xx, yy, r, g, b, a);
			}
		}
		WRect rect = m_pArea->GetRect();
		rect.w = (float)m_pZoomed->Width();
		rect.h = (float)m_pZoomed->Height();
		m_pArea->SetRect(rect);
		m_pArea->SetBgImg(m_pZoomed);
	}
}

void CUccBase::DrawEventHandler(eButton button, int event, const WPoint& pos)
{
	Bitmap* brush = m_pDefaultBrush;
	if (!brush)
		brush = &UccCurrentBrush()->brush;
	if (button == LEFT_BUTTON)
	{
		switch (event)
		{
		case 1:
		{
			m_bDragging = true;
			ClearTexCache(m_pBackup);
			if (m_pBackup)
				delete m_pBackup;
			m_pBackup = NULL;
			m_pBackup = new Bitmap;
			*m_pBackup = *m_pTexture;
			ClearTexCache(m_pBackup32);
			if (m_pBackup32)
				delete m_pBackup32;
			m_pBackup32 = NULL;
			m_pBackup32 = new Bitmap;
			*m_pBackup32 = *m_pTexture32;
			m_min.x = m_pTexture->Width();
			m_min.y = m_pTexture->Height();
			m_max.x = m_max.y = 0;
			if (m_pArea)
			{
				WRect rect = m_pArea->GetRect();
				IPoint p(pos.x - rect.x, pos.y - rect.y);
				if (!(m_lastPos == p))
				{
					Draw(p.x, p.y, brush, true);
					m_lastPos = p;
				}
			}
			else
			{
				Draw((int)pos.x, (int)pos.y, brush, true);
				m_lastPos.x = -1;
				m_lastPos.y = -1;
			}
			break;
		}
		case 2:
		{
			m_bDragging = false;
			m_lastPos.x = -1;
			m_lastPos.y = -1;
			m_history.Add(m_pBackup, m_pTexture, m_min, m_max);
			m_history32.Add(m_pBackup32, m_pTexture32, m_min, m_max);
			MustRefresh(2);
			m_pParent->SetSaved(false);
			break;
		}
		case 3:
		{
			if (!m_bDragging)
				break;
			if (m_pArea)
			{
				WRect rect = m_pArea->GetRect();
				IPoint p(pos.x - rect.x, pos.y - rect.y);
				if (!(m_lastPos == p))
				{
					if (m_lastPos.x == -1 && m_lastPos.y == -1)
					{
						Draw(p.x, p.y, brush, true);
					}
					else
					{
						DrawLine(m_lastPos, p, brush);
					}
					m_lastPos = p;
				}
			}
			else
				Draw((int)pos.x, (int)pos.y, brush, true);
		}
		}
	}
}

void CUccBase::MoveEventHandler(eButton button, int event, const WPoint& pos)
{
	if (!IsZoomed())
		return;
	if (m_zoomArea.x == 0 && m_zoomArea.w == m_pTexture32->Width() &&
		m_zoomArea.y == 0 && m_zoomArea.h == m_pTexture32->Height())
		return;
	if (button == LEFT_BUTTON)
	{
		switch (event)
		{
		case 1:
		{
			m_bDragging = true;
			WRect rect = m_pArea->GetRect();
			IPoint p(pos.x - rect.x, pos.y - rect.y);
			if (!(p == m_lastPos))
				m_lastPos = p;
			break;
		}
		case 2:
			m_bDragging = false;
			m_lastPos.x = -1;
			m_lastPos.y = -1;
			break;
		case 3:
		{
			WRect rect = m_pArea->GetRect();
			IPoint p(pos.x - rect.x, pos.y - rect.y);
			if (!(p == m_lastPos) && m_lastPos.x != -1 && m_lastPos.y != -1)
			{
				IPoint d(m_lastPos.x - p.x, m_lastPos.y - p.y);
				d.x /= m_zoomRate;
				d.y /= m_zoomRate;
				if (d.x || d.y)
				{
					if (m_zoomArea.x + m_zoomArea.w + d.x >
						m_pTexture32->Width())
						d.x = (int)(m_pTexture32->Width() -
							(m_zoomArea.x + m_zoomArea.w));
					if (m_zoomArea.y + m_zoomArea.h + d.y >
						m_pTexture32->Height())
						d.y = (int)(m_pTexture32->Height() -
							(m_zoomArea.y + m_zoomArea.h));
					if (m_zoomArea.x + d.x < 0)
						d.x = (int)-m_zoomArea.x;
					if (m_zoomArea.y + d.y < 0)
						d.y = (int)-m_zoomArea.y;
					m_lastPos = p;
					m_zoomArea.x += d.x;
					m_zoomArea.y += d.y;
					_DrawZoomedArea();
				}
			}
			break;
		}
		}
	}
}

void CUccBase::SpoidEventHandler(eButton button, int event, const WPoint& pos)
{
	if (button != LEFT_BUTTON)
		return;
	switch (event)
	{
	case 1:
	case 3:
	{
		WRect rect = m_pArea->GetRect();
		IPoint p((int)(pos.x - rect.x), (int)(pos.y - rect.y));
		sRGB color;
		GetTexture32BPP(false)->GetPixel(p.x, p.y, color.r, color.g, color.b);
		if (m_pParent)
			m_pParent->SetCurrentColor(color.r, color.g, color.b);
		break;
	}
	case 2:
		SetMode(MODE_DRAW);
		break;
	}
}

void CUccBase::PaintEventHandler(eButton button, int event, const WPoint& pos)
{
	if (button != LEFT_BUTTON)
		return;
	switch (event)
	{
	case 2:
	{
		m_pBackup = new Bitmap;
		*m_pBackup = *m_pTexture;
		ClearTexCache(m_pBackup32);
		if (m_pBackup32)
			delete m_pBackup32;
		m_pBackup32 = NULL;
		m_pBackup32 = new Bitmap;
		*m_pBackup32 = *m_pTexture32;
		WRect rect = m_pArea->GetRect();
		IPoint p((int)(pos.x - rect.x), (int)(pos.y - rect.y));
		Paint(p.x, p.y, true, 0, 0, 0);
		if (IsZoomed())
			_DrawZoomedArea();
		IPoint minimum(0, 0),
			maximum((int)m_pTexture->Width() - 1,
				(int)m_pTexture->Height() - 1);
		m_history.Add(m_pBackup, m_pTexture, minimum, maximum);
		m_history32.Add(m_pBackup32, m_pTexture32, minimum, maximum);
		MustRefresh(7);
		SetMode(MODE_DRAW);
		m_pParent->SetSaved(false);
	}
	}
}

void CUccBase::Paint(int x, int y, bool first, unsigned char r, unsigned char g,
	unsigned char b)
{
	if (first)
	{
		r = UccCurrentBrush()->r;
		g = UccCurrentBrush()->g;
		b = UccCurrentBrush()->b;
	}
	if (IsZoomed())
	{
		x = (int)(x / GetZoomRate() + m_zoomArea.x);
		y = (int)(y / GetZoomRate() + m_zoomArea.y);
	}
	sRGBA color(r, g, b);
	std::list<IPoint> points;
	int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };
	points.push_back(IPoint(x, y));
	bool visited[512][512];
	for (unsigned yy = 0; yy < m_pTexture->Height(); ++yy)
		for (unsigned xx = 0; xx < m_pTexture->Width(); ++xx)
			visited[yy][xx] = false;
	while (points.size() > 0)
	{
		IPoint p = points.front();
		points.pop_front();
		if (visited[p.y][p.x] == true)
			continue;
		if (!m_pTexture32->GetAlpha(p.x, p.y))
			continue;
		visited[p.y][p.x] = true;
		sRGBA old;
		m_pTexture32->GetPixel(p.x, p.y, old.r, old.g, old.b);
		sRGBA mask, result;
		if (m_pMask)
		{
			m_pMask->GetPixel(p.x, p.y, mask.r, mask.g, mask.b);
			SetBrightness(color, mask, result);
		}
		else
		{
			result = color;
		}
		m_pTexture->SetPixel(p.x, p.y, result.r, result.g, result.b, 255);
		m_pTexture32->SetPixel(p.x, p.y, color.r, color.g, color.b, 255);
		for (int i = 0; i < 4; ++i)
		{
			int xx = p.x + dx[i], yy = p.y + dy[i];
			if (xx >= 0 && yy >= 0 && xx < m_pTexture->Width() &&
				yy < m_pTexture->Height())
			{
				if (m_pTexture32->GetAlpha(xx, yy))
				{
					sRGBA next;
					m_pTexture32->GetPixel(xx, yy, next.r, next.g, next.b);
					if (!visited[yy][xx] && old == next)
						points.push_back(IPoint(xx, yy));
				}
			}
		}
	}
}

void CUccBase::ZoomEventHandler(eButton button, int event, const WPoint& pos)
{
	if (button != LEFT_BUTTON)
		return;
	switch (event)
	{
	case 2:
	{
		WRect rect = m_pArea->GetRect();
		int x = (int)(pos.x - rect.x), y = (int)(pos.y - rect.y);
		if (IsZoomed())
		{
			x = (int)(x / GetZoomRate() + m_zoomArea.x);
			y = (int)(y / GetZoomRate() + m_zoomArea.y);
		}
		if (m_mode == MODE_ZOOMIN)
		{
			ZoomIn(x, y);
			if (m_zoomRate == 8)
				SetMode(MODE_DRAW);
		}
		else if (m_mode == MODE_ZOOMOUT)
		{
			ZoomOut(x, y);
			if (m_zoomRate == 1)
				SetMode(MODE_DRAW);
		}
		if (m_pParent)
			m_pParent->AfterZoom();
	}
	}
}

CUccBaseItemDraw::CUccBaseItemDraw()
{
	m_textureName[0] = 0;
	m_uccIndex[0] = 0;
	m_typeID = 0;
}

CUccBaseItemDraw::~CUccBaseItemDraw()
{
}

void CUccBaseItemDraw::InitializeItemDraw(int side, FrArea* area,
	Bitmap* texture, Bitmap* mask, unsigned long typeID, const char* index)
{
	CUccBase::Initialize(area, texture, mask);
	strncpy(m_uccIndex, index, 9);
	m_typeID = typeID;
	GenerateTextureName(side, typeID, m_uccIndex);
	m_history.Clear();
	m_history32.Clear();
	Convert24to32(m_pTexture, m_pTexture32, m_pMask);
}

bool CUccBaseItemDraw::IsInitialized()
{
	return strlen(m_uccIndex) && m_typeID && CUccBase::IsInitialized();
}

void CUccBaseItemDraw::Refresh2D()
{
	if (m_pArea)
	{
		Bitmap* bitmap = GetTexture32BPP(false);
		if (bitmap)
			m_pArea->SetBgImg(bitmap);
	}
	m_refresh &= ~1;
}

void CUccBaseItemDraw::Refresh3D()
{
	w_texlist* texture = g_resrcmng->FindTexture(m_textureName);
	if (texture)
	{
		if (m_pTexture)
			g_resrcmng->FixTexture(texture->texhandle, m_pTexture, 0, 0);
	}
	else
	{
		if (m_textureName && m_pTexture)
			g_resrcmng->UploadTexture(m_textureName, m_pTexture, 0, 0);
	}
	m_refresh &= ~2;
}

void CUccBaseItemDraw::GenerateTextureName(int side, unsigned long typeID,
	const char* index)
{
	const char* check = index;
	if (!check)
		check = m_uccIndex;
	if (!strlen(check))
		return;
	if (side == 0)
		strcpy(m_textureName, GenerateTextureName_F(typeID, index).c_str());
	else
		strcpy(m_textureName, GenerateTextureName_B(typeID, index).c_str());
}

void CUccBaseItemDraw::Process()
{
	CUccBase::Process();
}
