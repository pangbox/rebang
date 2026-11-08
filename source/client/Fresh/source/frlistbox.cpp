#include <stdio.h>
#include <string.h>
#include "frscrollbar.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "chatmsg.h"
#include "commonutil.h"

IMPLEMENT_OBJECT(FrListBox, FrWnd)

BEGIN_FRESH_MSGMAP(FrListBox, FrWnd)
END_FRESH_MSGMAP()

FrListBox::FrListBox()
{
	m_pItem = NULL;
	m_pItemUnderCursor = NULL;
	m_pSelected = NULL;
	m_multiSelect = false;
	m_useRightButton = false;
	m_dummyCount = 0;
	m_useDummy = false;
	m_pPreviousSelected = NULL;
	m_listBgImg = NULL;
	m_rollOver = true;
	m_szPushSound = "ui_icon_click";
}

FrListBox::~FrListBox()
{
	ClearItem();
	m_noRButtonMap.clear();
}

void FrListBox::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();
	std::map<std::string, std::string>& param = item.m_param;
	if (param.find("itemsize") != param.end())
		sscanf(param["itemsize"].c_str(), "%d %d", &m_itemSize.w,
			&m_itemSize.h);
	if (param.find("column") != param.end())
		sscanf(param["column"].c_str(), "%d", &m_colCapacity);
	bool showScrollBar = true;
	if (param.find("show_scrollbar") != param.end())
	{
		int show;
		sscanf(param["show_scrollbar"].c_str(), "%d", &show);
		showScrollBar = show != 0;
	}
	if (item.m_param.find("listbg") != item.m_param.end())
		m_listBgImg = pDoc->GetBitmap(item.m_param["listbg"]);
	bool useKey = false;
	if (param.find("use_key") != param.end())
	{
		useKey = strcmpi(param["use_key"].c_str(), "true") == 0;
	}
	m_pItem = &item;
	unsigned long keyFlag = useKey ? FWS_KEYEVENT : 0;

	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager,
		(FWS_VISIBLE | keyFlag),
		WRect((float)item.m_rect.left, (float)item.m_rect.top,
			(short)(item.m_rect.right - item.m_rect.left),
			(short)(item.m_rect.bottom - item.m_rect.top)),
		pParent);
	m_prevTopRow = 0;
	int rowCapacity = m_itemSize.h ? (int)m_rect.h / m_itemSize.h : 0;
	if (!m_pScrBar)
	{
		m_pScrBar = new FrScrollBar;
		m_pScrBar->Init(pManager, this, 0, rowCapacity, m_colCapacity, false);
		EnableScrollBar(showScrollBar);
		m_pScrBar->Resize(rowCapacity, m_colCapacity, false);
		m_pScrBar->FollowBottom(false);
	}
	RecalcTopBottom();
}

void FrListBox::Resize(int w, int h)
{
	m_itemSize.w = w;
	m_itemSize.h = h;
	int scrollWidth =
		m_pScrBar && m_pScrBar->IsVisible() ? (int)m_pScrBar->GetRect().w : 0;
	int colCapacity =
		m_itemSize.w ? ((int)m_rect.w - scrollWidth) / m_itemSize.w : 1;
	if (colCapacity < 1)
		colCapacity = 1;
	m_colCapacity = colCapacity;
	RecalcTopBottom();
	OnResize();
}

void FrListBox::RecalcColCapacity(bool b)
{
	int scrollWidth =
		m_pScrBar && m_pScrBar->IsVisible() ? (int)m_pScrBar->GetRect().w : 0;
	int colCapacity =
		m_itemSize.w ? ((int)m_rect.w - scrollWidth) / m_itemSize.w : 1;
	if (!b)
		colCapacity = colCapacity > m_colCapacity ? m_colCapacity : colCapacity;
	if (colCapacity < 1)
		colCapacity = 1;
	m_colCapacity = colCapacity;
}

const Bitmap* FrListBox::GetBitmap(const char* name)
{
	FrElementDoc* pDoc = m_pWndManager->GetDocument();
	return pDoc->GetBitmap(name);
}

void FrListBox::SelectItem(FrListItem* item, bool b)
{
	if (item)
		item->selected = true;
	if (!m_multiSelect && m_pSelected && m_pSelected != item)
		m_pSelected->selected = false;
	if (b)
	{
		m_pPreviousSelected = m_pSelected;
		m_pSelected = item;
		int pos = -1;
		int n = 0;
		for (ITEM_LIST::iterator it = m_itemList.begin();
			it != m_itemList.end(); ++it, ++n)
		{
			if (it == m_itemTop)
				pos = 0;
			if (it == m_itemBottom)
				pos = 1;
			if (*it == item)
			{
				if (pos == -1)
				{
					int capacity = m_pScrBar->GetRowCapacity() *
						m_pScrBar->GetColCapacity();
					if (!capacity)
						break;
					capacity /= 2;
					if (!capacity)
						break;
					int count = (m_pScrBar->GetCurTopRow_Int() *
										m_pScrBar->GetColCapacity() -
									n - 1 + capacity) /
						capacity;
					for (int i = 0; i < count; ++i)
						m_pScrBar->ScrollUp(-1);
				}
				else if (pos == 1)
				{
					int capacity = m_pScrBar->GetRowCapacity() *
						m_pScrBar->GetColCapacity();
					if (!capacity)
						break;
					capacity /= 2;
					if (!capacity)
						break;
					int count = (n -
									m_pScrBar->GetCurTopRow_Int() *
										m_pScrBar->GetColCapacity() -
									capacity) /
						capacity;
					for (int i = 0; i < count; ++i)
						m_pScrBar->ScrollDown(-1);
				}
				break;
			}
		}
	}
}

void FrListBox::UnselectItem(FrListItem* item)
{
	if (item)
	{
		item->selected = false;
		if (m_pSelected == item)
			m_pSelected = NULL;
	}
}

void FrListBox::UnselectAllItem()
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		UnselectItem(*it);
}

void FrListBox::ToggleItem(FrListItem* item)
{
	if (item->selected)
		UnselectItem(item);
	else
		SelectItem(item, true);
}

FrListItem* FrListBox::AddItem(void* data)
{
	FrListItem* pItem = new FrListItem;
	pItem->pData = data;
	pItem->idx = m_itemList.size();
	if (!data)
		++m_dummyCount;
	m_itemList.push_back(pItem);
	if (m_pScrBar)
		m_pScrBar->AddItem();
	RecalcTopBottom();
	return pItem;
}

FrListItem* FrListBox::FindItem(FrListItem* item)
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		if (*it == item)
			return *it;
	return NULL;
}

FrListItem* FrListBox::FindItem(void* data)
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		if ((*it)->pData == data)
			return *it;
	return NULL;
}

FrListItem* FrListBox::FindItem(void* data, int size)
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		if ((*it)->pData && !memcmp((*it)->pData, data, size))
			return *it;
	return NULL;
}

FrListItem* FrListBox::GetItem(int no)
{
	int n = 0;
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it, ++n)
		if ((*it)->pData && no == n)
			return *it;
	return NULL;
}

FrListBox::ITEM_LIST::iterator FrListBox::_DelItem(ITEM_LIST::iterator it)
{
	FrListItem* pItem = *it;
	if (!pItem->pData)
		--m_dummyCount;
	if (pItem == m_pItemUnderCursor)
		m_pItemUnderCursor = NULL;
	if (pItem == m_pSelected)
	{
		if (!m_multiSelect)
		{
			ITEM_LIST::iterator next = it;
			++next;
			if (next != m_itemList.end())
				SelectItem(*next, true);
			else
			{
				next--;
				if (next != m_itemList.begin())
					SelectItem(*--next, true);
				else
					m_pSelected = NULL;
			}
		}
		else
			m_pSelected = NULL;
	}
	delete pItem;
	it = m_itemList.erase(it);
	if (m_pScrBar)
		m_pScrBar->DelItem();
	RecalcTopBottom();
	return it;
}

void FrListBox::DelItem(FrListItem* item)
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		if (*it == item)
		{
			_DelItem(it);
			break;
		}
}

void FrListBox::DelItem(void* data)
{
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
		if ((*it)->pData == data)
		{
			_DelItem(it);
			break;
		}
}

void FrListBox::DelItem(void* data, int size)
{
	if (!data)
		return;
	for (ITEM_LIST::iterator it = m_itemList.begin(); it != m_itemList.end();
		++it)
	{
		FrListItem* pItem = *it;
		if (pItem->pData && !memcmp(pItem->pData, data, size))
		{
			_DelItem(it);
			break;
		}
	}
}

void FrListBox::ClearItem()
{
	if (!m_itemList.empty())
	{
		sequence_delete(m_itemList.begin(), m_itemList.end());
		m_itemList.clear();
	}
	m_pItemUnderCursor = NULL;
	m_pSelected = NULL;
	if (m_pScrBar)
		m_pScrBar->ClearItem();
	RecalcTopBottom();
}

void FrListBox::SortItem(bool (*compare)(const void*, const void*))
{
	if (compare)
	{
		m_itemList.sort(FrListBoxGreater(compare));
		RecalcTopBottom();
	}
}

void FrListBox::RecalcTopBottom()
{
	int skip = m_pScrBar ? m_pScrBar->GetCurTopRow_Int() * m_colCapacity : 0;
	ITEM_LIST::iterator it = m_itemList.begin();
	for (; skip > 0 && it != m_itemList.end(); ++it, --skip)
	{
	}
	m_itemTop = it;
	int x = 0, y = 0;
	for (; it != m_itemList.end(); ++it)
	{
		if (x / m_itemSize.w >= m_colCapacity)
		{
			y += m_itemSize.h;
			x = 0;
			if (y + m_itemSize.h > m_rect.h)
				break;
		}
		x += m_itemSize.w;
	}
	m_itemBottom = it;
}

void FrListBox::EnableScrollBar(bool enable)
{
	if (m_pScrBar)
	{
		m_pScrBar->SetVisible(enable);
		m_pScrBar->Enable(enable);
	}
	RecalcColCapacity(false);
}

void FrListBox::SetCursorToFirstItem()
{
	ITEM_LIST::iterator it = m_itemList.begin();
	if (it != m_itemList.end())
	{
		FrListItem* pItem = *it;
		pItem->underCursor = true;
		m_pItemUnderCursor = pItem;
	}
}

void FrListBox::SetCursorToLastItem()
{
	ITEM_LIST::iterator it = m_itemList.end();
	if (--it != m_itemList.end())
	{
		FrListItem* pItem = *it;
		pItem->underCursor = true;
		m_pItemUnderCursor = pItem;
	}
}

void FrListBox::OnDraw()
{
	FrWnd::OnDraw();
	if (!m_pItem)
		return;
	if (m_listBgImg)
	{
		float alpha = GDI()->GetAlpha();
		GDI()->SetAlpha(GetAlpha());
		GDI()->DrawTexture(m_listBgImg, m_rect, 0xffffffff, 0);
		GDI()->SetAlpha(alpha);
	}
	SendCmdToOwnerTarget(FRCMD_OWNERDRAW, 0, NULL);
	int x = 0, y = 0, n = 0;
	for (ITEM_LIST::iterator it = m_itemTop;
		it != m_itemBottom && it != m_itemList.end(); ++it)
	{
		if (!m_itemSize.w || x / m_itemSize.w >= m_colCapacity)
		{
			x = 0;
			y += m_itemSize.h;
		}
		(*it)->no = n++;
		(*it)->pos = WPoint(m_rect.x, m_rect.y) + WPoint((float)x, (float)y);
		x += m_itemSize.w;
		SendCmdToOwnerTarget(FRCMD_OWNERDRAW, (int)*it, NULL);
	}
}

void FrListBox::OnProc(float deltaTime)
{
	if (m_pScrBar && m_prevTopRow != m_pScrBar->GetCurTopRow_Int())
	{
		m_prevTopRow = m_pScrBar->GetCurTopRow_Int();
		RecalcTopBottom();
	}
}

void FrListBox::SetClientRect(const WRect& rect)
{
	FrWnd::SetClientRect(rect);
	OnResize();
}

void FrListBox::OnResize()
{
	RecalcTopBottom();
	if (m_pScrBar)
	{
		int rows = m_itemSize.h ? (int)m_rect.h / m_itemSize.h : 0;
		m_pScrBar->Resize(rows, m_colCapacity, false);
	}
}

int FrListBox::GetCurrentItemSize(bool b)
{
	int size = m_itemList.size();
	if (!b)
		size -= m_dummyCount;
	return size;
}

void FrListBox::OnMouseMove(const WPoint& point)
{
	bool found = false;
	for (ITEM_LIST::iterator it = m_itemTop;
		it != m_itemBottom && it != m_itemList.end(); ++it)
	{
		if ((*it)->pos.x <= point.x && point.x < (*it)->pos.x + m_itemSize.w &&
			(*it)->pos.y <= point.y && point.y < (*it)->pos.y + m_itemSize.h)
		{
			(*it)->underCursor = true;
			m_pItemUnderCursor = *it;
			if (!m_useDummy || (*it)->pData)
				found = true;
		}
		else
			(*it)->underCursor = false;
	}
	SendCmdToOwnerTarget(FRCMD_MOUSEMOVE, 0, NULL);
	if (!found)
		m_pItemUnderCursor = NULL;
}

void FrListBox::AddNoRButtonRect(const WRect& rect, int idx)
{
	m_noRButtonMap[idx] = rect;
}

void FrListBox::OnSetCursor(bool active, const WPoint& point)
{
	if (active)
	{
		if (m_pItemUnderCursor && m_rollOver)
		{
			if (m_useRightButton)
			{
				std::list<int>::iterator it;
				for (it = m_pItemUnderCursor->noRBtnIdxList.begin();
					it != m_pItemUnderCursor->noRBtnIdxList.end(); ++it)
				{
					WRect rect;
					rect = m_noRButtonMap[*it];
					if (point.x > rect.x + m_pItemUnderCursor->pos.x &&
						point.x < rect.w + m_pItemUnderCursor->pos.x + rect.x &&
						point.y > rect.y + m_pItemUnderCursor->pos.y &&
						point.y < rect.h + m_pItemUnderCursor->pos.y + rect.y)
					{
						SetCursor(1);
						return;
					}
				}
				SetCursor(7);
			}
			else
				SetCursor(1);
		}
	}
	else if (!m_dwStyle.GetFlag(FWS_KEYEVENT) && m_pItemUnderCursor)
	{
		m_pItemUnderCursor->underCursor = false;
		m_pItemUnderCursor = NULL;
	}
}

bool FrListBox::OnLButtonDown(const WPoint& point)
{
	OnMouseMove(point);
	if (m_pItemUnderCursor)
	{
		ToggleItem(m_pItemUnderCursor);
		PlayPushSound();
	}
	SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, NULL);
	return false;
}

bool FrListBox::OnLButtonUp(const WPoint& point)
{
	OnMouseMove(point);
	if (m_pItemUnderCursor)
		SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, NULL);
	return false;
}

bool FrListBox::OnRButtonDown(const WPoint& point)
{
	if (m_useRightButton)
	{
		OnMouseMove(point);
		if (m_pItemUnderCursor)
		{
			ToggleItem(m_pItemUnderCursor);
			PlayPushSound();
		}
		SendCmdToOwnerTarget(FRCMD_RBUTTONDOWN, 0, NULL);
	}
	return false;
}

bool FrListBox::OnRButtonUp(const WPoint& point)
{
	if (m_useRightButton)
	{
		OnMouseMove(point);
		if (m_pItemUnderCursor)
			SendCmdToOwnerTarget(FRCMD_RBUTTONUP, 0, NULL);
	}
	return false;
}

void FrListBox::OnDblClick(const WPoint& point)
{
	OnMouseMove(point);
	SendCmdToOwnerTarget(FRCMD_DBLCLICK, 0, NULL);
}

void FrListBox::EnableKeyFocus(FrInputState& input)
{
	FrWnd::EnableKeyFocus(input);
	if (!m_pItemUnderCursor)
		SetCursorToFirstItem();
}

void FrListBox::OnKeyFocus(CChatMsg* im)
{
	if (!im->IsActive())
		return;
	im->Process(0.01f);
	if (im->GetConsolKeyCode() == 4)
	{
		if (!m_pItemUnderCursor)
			SetCursorToFirstItem();
		else
		{
			for (ITEM_LIST::iterator it = m_itemList.begin();
				it != m_itemList.end(); ++it)
			{
				if (*it == m_pItemUnderCursor)
				{
					--it;
					if (it != m_itemList.end())
					{
						FrListItem* pItem = *it;
						m_pItemUnderCursor->underCursor = false;
						pItem->underCursor = true;
						m_pItemUnderCursor = pItem;
					}
					break;
				}
			}
		}
	}
	if (im->GetConsolKeyCode() == 5)
	{
		if (!m_pItemUnderCursor)
			SetCursorToFirstItem();
		else
		{
			for (ITEM_LIST::iterator it = m_itemList.begin();
				it != m_itemList.end(); ++it)
			{
				if (*it == m_pItemUnderCursor)
				{
					++it;
					if (it != m_itemList.end())
					{
						FrListItem* pItem = *it;
						m_pItemUnderCursor->underCursor = false;
						pItem->underCursor = true;
						m_pItemUnderCursor = pItem;
					}
					break;
				}
			}
		}
	}
	if (im->GetConsolKeyCode() == 6)
	{
		if (!m_pItemUnderCursor)
			SetCursorToFirstItem();
		else
		{
			for (ITEM_LIST::iterator it = m_itemList.begin();
				it != m_itemList.end(); ++it)
			{
				if (*it == m_pItemUnderCursor)
				{
					for (int n = 0; it != m_itemList.end() && n < m_colCapacity;
						--it, ++n)
					{
					}
					if (it != m_itemList.end())
					{
						FrListItem* pItem = *it;
						m_pItemUnderCursor->underCursor = false;
						pItem->underCursor = true;
						m_pItemUnderCursor = pItem;
					}
					break;
				}
			}
		}
	}
	if (im->GetConsolKeyCode() == 7)
	{
		if (!m_pItemUnderCursor)
			SetCursorToFirstItem();
		else
		{
			for (ITEM_LIST::iterator it = m_itemList.begin();
				it != m_itemList.end(); ++it)
			{
				if (*it == m_pItemUnderCursor)
				{
					for (int n = 0; it != m_itemList.end() && n < m_colCapacity;
						++it, ++n)
					{
					}
					if (it != m_itemList.end())
					{
						FrListItem* pItem = *it;
						m_pItemUnderCursor->underCursor = false;
						pItem->underCursor = true;
						m_pItemUnderCursor = pItem;
					}
					break;
				}
			}
		}
	}
}

bool FrListBox::MoveTopItem(int n)
{
	bool result = true;
	ITEM_LIST::iterator it = m_itemTop;
	if (n > 0)
	{
		if (++it == m_itemList.end())
			return false;
	}
	else
	{
		if (it == m_itemList.begin())
			return false;
		--it;
		if (it == m_itemList.begin())
			result = false;
	}
	m_itemTop = it;
	int x = 0, y = 0;
	for (; it != m_itemList.end(); ++it)
	{
		if (x / m_itemSize.w >= m_colCapacity)
		{
			y += m_itemSize.h;
			x = 0;
			if (y + m_itemSize.h > m_rect.h)
				break;
		}
		x += m_itemSize.w;
	}
	m_itemBottom = it;
	if (n > 0 && it == m_itemList.end())
		result = false;
	return result;
}

void FrListBox::SetMouseEvent(bool active)
{
	if (active)
		m_dwStyle.Disable(FWS_NOMOUSEEVENT);
	else
		m_dwStyle.Enable(FWS_NOMOUSEEVENT);
}
