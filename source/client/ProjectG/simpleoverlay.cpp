#include "projectg.h"
#include "simpleoverlay.h"

extern WView* g_view;

CSimpleUI::CSimpleUI(const char* name)
	: m_pOverlay(NULL), m_rectList(8, 8)
{
	m_pOverlay = g_resrcmng->GetOverlay(name, 0);
	if (m_pOverlay)
	{
		m_width = (float)m_pOverlay->GetWidth();
		m_height = (float)m_pOverlay->GetHeight();
	}

	m_angle = 0.0f;
	m_color = -1;
}

CSimpleUI::~CSimpleUI()
{
	if (g_resrcmng && m_pOverlay)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}

	DeleteAllItem();
}

void CSimpleUI::Add(const char* name, const WRect& rect, bool bAlloc)
{
	WRect* pRect = new WRect;
	pRect->x = rect.x / m_width;
	pRect->y = rect.y / m_height;
	pRect->w = rect.w / m_width;
	pRect->h = rect.h / m_height;
	m_rectList.AddItem(pRect, name, bAlloc);
}

void CSimpleUI::Render(const char* name, float x, float y, float scale,
	int align)
{
	if (!(m_color & 0xff000000) || !name)
		return;

	WRect* pRect = m_rectList.Find(name);

	if (!pRect)
		return;

	WRect dest;

	dest.w = pRect->w * m_width * scale;

	switch (align & 6)
	{
	case 0:
		dest.x = x;
		break;

	case 4:
		dest.x = x - dest.w;
		break;

	case 2:
		dest.x = x - dest.w * 0.5f;
		break;
	}

	dest.h = pRect->h * m_height * scale;

	switch (align & 0x60)
	{
	case 0:
		dest.y = y;
		break;

	case 0x40:
		dest.y = y - dest.h;
		break;

	case 0x20:
		dest.y = y - dest.h * 0.5f;
		break;
	}

	if (Wabs(m_angle) < g_EPSILON)
	{
		if (m_pOverlay)
			m_pOverlay->Render(g_view, *pRect, dest, 0, m_color, 0.0f, 0);
	}
	else
	{
		if (m_pOverlay)
			m_pOverlay->RenderWithAxis(g_view, *pRect, dest,
				dest.x + dest.w * 0.5f, dest.y + dest.h * 0.5f, m_angle, 0,
				m_color, 0);
	}
}

void CSimpleUI::DeleteAllItem()
{
	for (WRect* pRect = m_rectList.Start(); pRect; pRect = m_rectList.Next())
	{
		if (pRect)
		{
			m_rectList -= pRect;
			delete pRect;
		}
	}

	m_rectList.Reset();
}
