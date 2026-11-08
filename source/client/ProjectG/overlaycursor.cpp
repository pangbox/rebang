#include "projectg.h"
#include "wresrcmng.h"
#include "mousecursor.h"
#include "overlaycursor.h"
#include "fresh.h"
#include "frwndmanager.h"
extern Fresh* g_pFresh;

COverlayCursor::COverlayCursor()
{
	m_cursor = CT_NORMAL;
	m_ppRect = NULL;
}

COverlayCursor::~COverlayCursor()
{
	std::map<int, sOCItem>::iterator it;

	for (it = m_items.begin(); it != m_items.end(); it++)
	{
		WOverlay* pOverlay = it->second.pOverlay;
		if (g_resrcmng && pOverlay)
			g_resrcmng->Release(pOverlay);
	}
}

void COverlayCursor::Add(eCursorType type, const char* filename, int x, int y)
{
	if (!filename)
		return;

	if (!strlen(filename))
		return;

	WOverlay* pOverlay = g_resrcmng->GetOverlay(filename, 0);

	if (!pOverlay)
		return;
	sOCItem item;

	item.type = type;
	item.pOverlay = pOverlay;
	item.x = x;
	item.y = y;

	m_items.insert(std::map<int, sOCItem>::value_type(type, item));
}

void COverlayCursor::Render()
{
	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	if (m_cursor == CT_NORMAL)
	{
		CMouseCursor::Instance()->ForceMove(mousePos.x, mousePos.y, -1.0f);
		CMouseCursor::Instance()->Toggle(true);
		return;
	}

	std::map<int, sOCItem>::iterator it = m_items.find(m_cursor);
	if (it == m_items.end())
	{
		CMouseCursor::Instance()->Toggle(true);
		return;
	}

	if (m_ppRect && *m_ppRect)
	{
		const volatile WRect& bounds = (*m_ppRect)->GetRect();
		WRect rect;
		rect.x = bounds.x;
		static_cast<volatile float&>(rect.y) = bounds.y;
		rect.w = bounds.w;
		rect.h = bounds.h;

		if (!(rect.x <= mousePos.x && mousePos.x <= rect.x + rect.w &&
				rect.y <= mousePos.y && mousePos.y <= rect.y + rect.h))
		{
			CMouseCursor::Instance()->ForceMove(mousePos.x, mousePos.y, -1.0f);
			CMouseCursor::Instance()->Toggle(true);
			return;
		}
	}

	if (!(*m_ppRect)->IsViewFocused())
	{
		CMouseCursor::Instance()->Toggle(true);
		return;
	}

	CMouseCursor::Instance()->Toggle(false);

	WOverlay* pOverlay;
	{
		WRect dst;
		pOverlay = it->second.pOverlay;
		dst.x = mousePos.x - it->second.x;
		dst.y = mousePos.y - it->second.y;

		dst.w = pOverlay->GetWidth();
		dst.h = pOverlay->GetHeight();
		pOverlay->Render(g_view, WRect(0, 0, 1.0f, 1.0f), dst, 0, 0xffffffff, 0,
			0);
	}
}
