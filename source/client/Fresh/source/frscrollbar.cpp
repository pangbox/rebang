#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "wutil.h"

IMPLEMENT_OBJECT(FrScrollBar, FrWnd)

FrScrollBar::FrScrollBar()
{
	for (int i = 0; i < 3; ++i)
	{
		m_frag[i].n_img = NULL;
		m_frag[i].o_img = NULL;
		m_frames[i] = NULL;
	}
}

FrScrollBar::~FrScrollBar()
{
}

void FrScrollBar::Init(FrWndManager* pManager, FrWnd* parentWnd, int row,
	int rowCapacity, int colCapacity, bool atLeft)
{
	m_dwStyle.Enable(FWS_VISIBLE);
	m_pParentWnd = parentWnd;
	m_itemNum = 0;
	m_curTopRow = 0;
	m_followBottom = true;
	m_showGuide = false;
	m_mouseDown = false;
	FrElementDoc* pDoc = pManager->GetDocument();

	for (int i = 0; i < 3; ++i)
	{
		m_frag[i].n_img = pDoc->GetBitmap(MakeStr("scroll_n_%d", i + 1));
		m_frag[i].o_img = pDoc->GetBitmap(MakeStr("scroll_o_%d", i + 1));

		m_frag[i].width = m_frag[i].n_img->Width();
		m_frag[i].height = m_frag[i].n_img->Height();

		m_frames[i] = pDoc->GetBitmap(MakeStr("scrollbg%d", i + 1));
	}

	Create("scrollbar", "scrollbar", pManager, FWS_VISIBLE, WRect(0, 0, 0, 0),
		parentWnd);
	m_dwStyle.Disable(FWS_NOWHEELEVENT);

	Resize(rowCapacity, colCapacity, atLeft);
}

void FrScrollBar::Resize(int rowCapacity, int colCapacity, bool atLeft)
{
	m_rowCapacity = rowCapacity;
	m_colCapacity = colCapacity;

	m_rect = m_pParentWnd->GetRect();
	m_atLeft = atLeft;
	if (!m_atLeft)
		m_rect.x += m_rect.w - m_frag[0].width;
	m_rect.w = m_frag[0].width;

	SetBarHnY();
}

void FrScrollBar::SetCurTopRow(int row)
{
	m_curTopRow = Min(row, Max(GetCurRowCount() - m_rowCapacity, 0));

	SetBarHnY();
}

void FrScrollBar::AddItem()
{
	++m_itemNum;

	if (m_followBottom)
		m_curTopRow = Max(GetCurRowCount() - m_rowCapacity, 0);

	SetBarHnY();
}

void FrScrollBar::DelItem()
{
	if (m_itemNum > 0)
	{
		--m_itemNum;

		if (m_colCapacity && m_itemNum % m_colCapacity == 0 && m_curTopRow > 0)
			m_curTopRow -= 1;
	}

	SetBarHnY();
}

void FrScrollBar::ClearItem()
{
	m_curTopRow = 0;
	m_itemNum = 0;
	SetBarHnY();
}

void FrScrollBar::ScrollToFirst()
{
	m_curTopRow = 0;
	m_followBottom = false;
	SetBarHnY();
}

void FrScrollBar::ScrollToBottom()
{
	m_followBottom = true;
	m_curTopRow = Max(GetCurRowCount() - m_rowCapacity, 0);

	SetBarHnY();
}

void FrScrollBar::ScrollUp(int delta)
{
	if (delta < 0)
		delta = m_rowCapacity / 2;

	if (m_curTopRow - delta > 0)
		m_curTopRow -= delta;
	else
		m_curTopRow = 0;

	m_followBottom = false;
	SetBarHnY();
}

void FrScrollBar::ScrollDown(int delta)
{
	if (delta < 0)
		delta = m_rowCapacity / 2;

	if (m_curTopRow + m_rowCapacity + delta < GetCurRowCount())
		m_curTopRow += delta;
	else
	{
		m_curTopRow = GetCurRowCount() - m_rowCapacity;
		m_followBottom = true;
	}

	SetBarHnY();
}

void FrScrollBar::SetBarHnY()
{
	if (GetCurRowCount() > 0)
		m_frag[1].height = (m_rect.h - m_frag[0].height - m_frag[2].height) *
			m_rowCapacity / GetCurRowCount();

	m_barH = m_frag[0].height + m_frag[1].height + m_frag[2].height;

	if (GetCurRowCount() - m_rowCapacity > 0)
	{
		float top = floor(m_curTopRow);
		if (m_curTopRow - top > 0.5f)
			m_curTopRow = top + 1.0f;
		else
			m_curTopRow = top;

		m_barY = (int)((m_rect.h - m_barH) * m_curTopRow /
				(GetCurRowCount() - m_rowCapacity) +
			0.5f);
	}
}

void FrScrollBar::OnDraw()
{
	if (!m_rowCapacity)
		return;

	FrWnd::OnDraw();

	unsigned long color =
		((int)(m_wndAlpha2 * m_wndAlpha * 255.0f) << 24) | 0xffffff;
	bool focused = WndManager()->GetWheelFocus() == this;

	if (m_rowCapacity < GetCurRowCount())
	{
		WRect frame(m_rect.x + 2.0f, m_rect.y, m_frames[0]->Width(),
			m_frames[0]->Height());
		GDI()->DrawTexture(m_frames[0], frame, color, 0);

		frame.y += frame.h;
		frame.h = m_rect.h - m_frames[0]->Height() - m_frames[1]->Height();
		GDI()->DrawTexture(m_frames[1], frame, color, 0);

		frame.y += frame.h;
		frame.h = m_frames[2]->Height();
		GDI()->DrawTexture(m_frames[2], frame, color, 0);

		WRect dst(m_rect.x + 3.0f, m_rect.y + m_barY, m_rect.w,
			m_frag[0].height);
		GDI()->DrawTexture(focused ? m_frag[0].o_img : m_frag[0].n_img, dst,
			color, 0);

		dst.y += dst.h;
		dst.h = m_frag[1].height;
		GDI()->DrawTexture(focused ? m_frag[1].o_img : m_frag[1].n_img, dst,
			color, 0);

		dst.y += dst.h;
		dst.h = m_frag[2].height;
		GDI()->DrawTexture(focused ? m_frag[2].o_img : m_frag[2].n_img, dst,
			color, 0);
	}
	else if (m_showGuide)
	{
		WRect dst(m_rect.x + 2.0f, m_rect.y, m_rect.w, m_frag[0].height);
		GDI()->DrawTexture(focused ? m_frag[0].o_img : m_frag[0].n_img, dst,
			color, 0);

		dst.y += dst.h;
		dst.h = m_rect.h - m_frag[0].height - m_frag[2].height;
		GDI()->DrawTexture(focused ? m_frag[1].o_img : m_frag[1].n_img, dst,
			color, 0);

		dst.y += dst.h;
		dst.h = m_frag[2].height;
		GDI()->DrawTexture(focused ? m_frag[2].o_img : m_frag[2].n_img, dst,
			color, 0);
	}
}

void FrScrollBar::OnResize()
{
	Resize(m_rowCapacity, m_colCapacity, m_atLeft);
}

void FrScrollBar::OnMouseMove(const WPoint& mousePos)
{
	if (GetCapture() == this)
	{
		if (mousePos.y + m_barY - m_dragPrevY < 0)
		{
			m_barY = 0;
		}
		else if (mousePos.y + m_barY + m_barH - m_dragPrevY > m_rect.h)
		{
			m_barY = m_rect.h - m_barH;
		}
		else
		{
			m_barY += mousePos.y - m_dragPrevY;
			m_dragPrevY = mousePos.y;
		}

		int d = (int)(m_rect.h - m_barH);
		m_curTopRow = d ? m_barY * (GetCurRowCount() - m_rowCapacity) / d : 0;
		m_followBottom =
			m_curTopRow < GetCurRowCount() - m_rowCapacity ? false : true;
		m_mouseDown = true;
	}
	else
	{
		m_mouseDown = false;
	}
}

bool FrScrollBar::OnLButtonDown(const WPoint& mousePos)
{
	if (m_rect.y + m_barY > mousePos.y)
	{
		ScrollUp(-1);
	}
	else if (m_rect.y + m_barY + m_barH < mousePos.y)
	{
		ScrollDown(-1);
	}
	else
	{
		if (m_rowCapacity >= GetCurRowCount() || !SetCapture())
		{
			return true;
		}

		m_dragPrevY = mousePos.y;
	}

	return false;
}

bool FrScrollBar::OnLButtonUp(const WPoint& mousePos)
{
	SetBarHnY();

	if (GetCapture() == this)
	{
		ReleaseCapture();
		return false;
	}

	return true;
}

void FrScrollBar::OnWheel(FrInputState& istate)
{
	if (istate.wheelFocused == this)
	{
		if (istate.wheelDelta > 0)
			ScrollUp((int)istate.wheelDelta / 119);
		else
			ScrollDown(-(int)istate.wheelDelta / 119);
	}
}
