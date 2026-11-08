#include <stdio.h>
#include <string.h>
#include "frtabbutton.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "frdesktop.h"
#include "frcursor.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "frform.h"
#include "frframe.h"
#include "frbutton.h"
#include "frstatic.h"
#include "frtextbutton.h"
#include "frarea.h"
#include "frlistbox.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frgaugebar.h"
#include "frviewer.h"
#include "frcontextmenuctrl.h"
#include "frgroupbox.h"
#include "frmacroitem.h"
#include "frtooltip.h"
#include "../texturecache/rectcache.h"
#include "commonutil.h"
#include "../../../shared/token.h"

IMPLEMENT_OBJECT(FrTabButton, FrWnd)

FrTabButton::FrTabButton()
{
	m_sepImg = NULL;
	m_sepWidth = 0;
	m_selected = NULL;
	m_underCursor = NULL;
	m_pressed = NULL;
	m_btnType = BT_NONE;
	m_pBtnBgImg[0] = m_pBtnBgImg[1] = m_pBtnBgImg[2] = NULL;
	m_szPushSound = "ui_icon_click";
}

FrTabButton::~FrTabButton()
{
	ClearTab();
}

void FrTabButton::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();

	std::map<std::string, std::string>& param = item.m_param;
	if (param.find("sepImg") != param.end())
	{
		m_sepImg = pDoc->GetBitmap(param["sepImg"].c_str());
		if (m_sepImg)
			m_sepWidth = (float)m_sepImg->Width();
	}

	m_pBtnBgImg[0] = pDoc->GetBitmap(param["below_normal"]);
	m_pBtnBgImg[1] = pDoc->GetBitmap(param["below_over"]);
	m_pBtnBgImg[2] = pDoc->GetBitmap(param["below_selected"]);

	_RectangleSHORT r = item.m_rect;
	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager, FWS_VISIBLE,
		WRect(r.left, r.top, r.Width(), r.Height()), pParent);

	ClearTab();
}

void FrTabButton::PreCreateWindow(FrWndManager* pManager, unsigned long dwStyle,
	const WRect& rect, FrWnd* pParentWnd)
{
	FrWnd::PreCreateWindow(pManager, dwStyle, rect, pParentWnd);

	sFRESH_HANDLER chk;

	memset(&chk, 0, sizeof(chk));
	SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, &chk);
	if (chk.pFn)
		m_btnType = BT_DOWN;

	memset(&chk, 0, sizeof(chk));
	SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, &chk);
	if (chk.pFn)
		m_btnType = BT_UP;

	if (m_btnType == BT_NONE)
		m_dwStyle.Enable(FWS_NOMOUSEEVENT);
}

void FrTabButton::ClearTab()
{
	m_rect.w = 0;
	m_rect.h = 0;

	for (TBUTTON_LIST::iterator it = m_tBtnList.begin(); it != m_tBtnList.end();
		++it)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}

	m_tBtnList.clear();

	m_pressed = NULL;
	m_underCursor = NULL;
	m_selected = NULL;
}

void FrTabButton::AddTab(const char* imgName, unsigned long index, WSize* size)
{
	TButtonItem* tab = new TButtonItem;

	FrElementDoc* pDoc = WndManager()->GetDocument();
	if (pDoc)
	{
		char* ext[3] = { "n", "o", "d" };
		char buff[32];
		for (int i = 0; i < 3; ++i)
		{
			sprintf(buff, "%s_%s", imgName, ext[i]);
			tab->bitmap[i] = pDoc->GetBitmap(buff);
		}
	}

	InitButtonItem(tab, index, size);
}

void FrTabButton::AddTab(const char* imgNames[], unsigned long index,
	WSize* size)
{
	TButtonItem* tab = new TButtonItem;

	FrElementDoc* pDoc = WndManager()->GetDocument();
	if (pDoc)
	{
		for (int i = 0; i < 3; ++i)
		{
			tab->bitmap[i] = pDoc->GetBitmap(imgNames[i]);
		}
	}

	InitButtonItem(tab, index, size);
}

void FrTabButton::Select(unsigned long index)
{
	for (TBUTTON_LIST::iterator it = m_tBtnList.begin(); it != m_tBtnList.end();
		++it)
	{
		if ((*it)->index == index)
		{
			Select(*it);
			return;
		}
	}
}

void FrTabButton::UnSelect(unsigned long index)
{
	for (TBUTTON_LIST::iterator it = m_tBtnList.begin(); it != m_tBtnList.end();
		++it)
	{
		if ((*it)->index == index)
		{
			UnSelect(*it);
			return;
		}
	}
}

void FrTabButton::InitButtonItem(TButtonItem* tab, unsigned long index,
	WSize* size)
{
	tab->index = index;
	tab->selected = false;
	tab->underCursor = false;

	tab->rect.x = m_rect.x + m_rect.w;
	if (!m_tBtnList.empty())
		tab->rect.x += m_sepWidth;

	tab->rect.y = m_rect.y;

	if (size)
	{
		tab->rect.w = size->w;
		tab->rect.h = size->h;
	}
	else
	{
		tab->rect.w = (float)tab->bitmap[0]->Width();
		tab->rect.h = (float)tab->bitmap[0]->Height();
	}

	m_rect.w += tab->rect.w;
	if (m_tBtnList.empty())
		m_rect.h = tab->rect.h;
	else
		m_rect.w += m_sepWidth;

	m_tBtnList.push_back(tab);
}

void FrTabButton::Select(TButtonItem* i)
{
	if (m_selected)
		m_selected->selected = false;

	m_selected = i;

	if (i)
		i->selected = true;
}

void FrTabButton::UnSelect(TButtonItem* i)
{
	if (i)
		i->selected = false;

	if (m_selected == i)
		m_selected = NULL;
}

void FrTabButton::OnDraw()
{
	FrWnd::OnDraw();

	SendCmdToOwnerTarget(FRCMD_OWNERDRAW, 0, NULL);

	float x = 0;
	int index;
	for (TBUTTON_LIST::iterator it = m_tBtnList.begin(); it != m_tBtnList.end();
		++it)
	{
		index = 0;
		if ((*it)->selected)
		{
			if ((*it)->bitmap[2])
				index = 2;
		}
		else if ((*it)->underCursor)
		{
			if ((*it)->bitmap[1])
				index = 1;
		}

		(*it)->rect.x = x + m_rect.x;
		(*it)->rect.y = m_rect.y;

		if (it != m_tBtnList.begin())
		{
			if (m_sepImg)
			{
				GDI()->DrawTexture(m_sepImg,
					WRect((*it)->rect.x - m_sepWidth, (*it)->rect.y, m_sepWidth,
						(float)m_sepImg->Height()),
					FrALPHA(0xffffffff, m_wndAlpha2 * m_wndAlpha), 0);
			}
		}

		if (m_pBtnBgImg[index])
		{
			GDI()->DrawTexture(m_pBtnBgImg[index], (*it)->rect,
				FrALPHA(0xffffffff, m_wndAlpha2 * m_wndAlpha), 0);
		}

		GDI()->DrawTexture((*it)->bitmap[index], (*it)->rect,
			FrALPHA(0xffffffff, m_wndAlpha2 * m_wndAlpha), 0);

		x += (*it)->rect.w + m_sepWidth;

		SendCmdToOwnerTarget(FRCMD_OWNERDRAW, (int)(*it), NULL);
	}
}

void FrTabButton::OnProc(const float deltaTime)
{
}

void FrTabButton::OnMouseMove(const WPoint& mousePos)
{
	m_underCursor = NULL;

	for (TBUTTON_LIST::iterator it = m_tBtnList.begin(); it != m_tBtnList.end();
		++it)
	{
		TButtonItem* item = *it;

		if (item->rect.IsInRect(mousePos))
		{
			m_underCursor = item;
			item->underCursor = true;
		}
		else
		{
			item->underCursor = false;
		}
	}
}

void FrTabButton::OnSetCursor(bool bInClient, const WPoint& mousePos)
{
	if (bInClient)
	{
		if (m_underCursor)
		{
			SetCursor(FrCursor::ROLL_OVER);
		}
	}
	else
	{
		if (m_underCursor)
		{
			m_underCursor->underCursor = false;
			m_underCursor = NULL;
		}
	}
}

bool FrTabButton::OnLButtonDown(const WPoint& mousePos)
{
	OnMouseMove(mousePos);

	if (m_underCursor)
	{
		if (m_btnType == BT_DOWN)
			Select(m_underCursor);

		if (m_btnType == BT_UP)
			m_pressed = m_underCursor;

		PlayPushSound();

		SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, NULL);
	}

	return false;
}

bool FrTabButton::OnLButtonUp(const WPoint& mousePos)
{
	if (m_btnType != BT_UP)
		return false;

	OnMouseMove(mousePos);

	if (m_underCursor == m_pressed)
	{
		Select(m_underCursor);
		SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, NULL);
	}

	m_pressed = NULL;

	return false;
}
