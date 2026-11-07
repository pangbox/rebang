#include <stdio.h>
#include <string.h>
#include "frcontextmenuctrl.h"
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
#include "frtabbutton.h"
#include "frgroupbox.h"
#include "frmacroitem.h"
#include "frtooltip.h"
#include "commonutil.h"
#include "../../../shared/token.h"

FrContextMenuCtrl::FrContextMenuCtrl()
	: m_pFrame(NULL)
{
	m_style = 0;
	m_color = 0xff000000;
	m_outlineColor = 0xff000000;
	m_center = true;
	m_selStyle = 1;
	m_selColor = 0xffffcc00;
	m_selOutlineColor = 0xff000000;
	m_unableColor = 0xffcccccc;
	m_hlAlpha = 0.0f;
	m_hlAlphaD = 1;
	m_bIsIcon = false;
	m_szPushSound = "ui_icon_click";
	m_underCursor = m_selected = m_MenuList.end();
	for (int i = 0; i < 9; ++i)
	{
		m_pBaseBmp[i] = NULL;
		m_pSubBmp[i] = NULL;
	}
}

FrContextMenuCtrl::~FrContextMenuCtrl()
{
}

void FrContextMenuCtrl::Init(FrGuiItem& item, FrWndManager* pManager,
	FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();
	FrElementFrame* pFrame = pDoc->GetFrame(item.m_resource);
	m_pFrame = pFrame;

	std::map<std::string, std::string>& param = item.m_param;

	std::string filename = pFrame->m_bfrmName;
	if (param.find("bgimg") != param.end())
		filename = param["bgimg"];

	if (param.find("fontcolor") != param.end())
		sscanf(param["fontcolor"].c_str(), "%x", &m_color);

	int start = pFrame->m_aType[0] == 2 ? 6 : 0;
	int end = pFrame->m_aType[0] == 1 ? 3 : 9;

	for (int i = start; i < end; ++i)
		m_pBaseBmp[i] = pDoc->GetBitmap(MakeStr("%s%02d", filename.c_str(), i));

	if (pFrame->m_aType[0] == 0)
	{
		for (int i = 0; i < 9; ++i)
			m_pSubBmp[i] = pDoc->GetBitmap(MakeStr("sfrm%02d", i));
	}

	m_min = m_a = m_b = m_c = WSize(0, 0);
	m_width = m_height = 0;

	WRect rect((float)item.m_rect.left, (float)item.m_rect.top,
		(short)(item.m_rect.right - item.m_rect.left),
		(short)(item.m_rect.bottom - item.m_rect.top));

	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager, 1, rect,
		pParent);

	SetMenuList(item.m_caption.c_str());

	pFrame->m_aPos[0] = 0;
	pFrame->m_aHeight[0] = -1;
}

void FrContextMenuCtrl::SetVisible(bool visible)
{
	FrWnd::SetVisible(visible);

	if (visible)
	{
		for (MENU_LIST::iterator it = m_MenuList.begin();
			it != m_MenuList.end(); ++it)
		{
			(*it).Enable = true;
		}
	}
}

void FrContextMenuCtrl::UnableMenuItem(const char* szMenu)
{
	for (MENU_LIST::iterator it = m_MenuList.begin(); it != m_MenuList.end();
		++it)
	{
		if (!strcmp(it->szText, szMenu))
			it->Enable = false;
	}
}

void FrContextMenuCtrl::ToggleMenuItem(const char* szSrc, const char* szDest)
{
	for (MENU_LIST::iterator it = m_MenuList.begin(); it != m_MenuList.end();
		++it)
	{
		if (!strcmp(it->szText, szSrc) || !strcmp(it->szText, szDest))
			strcpy(it->szText, szDest);
	}
}

void FrContextMenuCtrl::OnDraw()
{
	FrWnd::OnDraw();

	const WPoint& pt = WPoint(m_rect.x, m_rect.y);

	if (m_pFrame->m_layers > 0)
	{
		UpdateRectInfo(0, m_pBaseBmp);
		DrawFrame(0, pt.x, pt.y, m_pBaseBmp);
	}

	for (int i = 1; i < m_pFrame->m_layers; ++i)
	{
		UpdateRectInfo(i, m_pSubBmp);
		DrawFrame(i, pt.x, pt.y, m_pSubBmp);
	}

	for (MENU_LIST::iterator it = m_MenuList.begin(); it != m_MenuList.end();
		++it)
	{
		WPoint p1;
		if (it->nType == SEPARATOR)
		{
			float y = (it->top + it->bottom) * 0.5f + m_rect.y;
			GDI()->Line(WPoint(m_rect.x + 3.0f, y), WPoint(m_rect.x + 15.0f, y),
				0xfff3f3f3, 0xfff3f3f3, 0);
			GDI()->Line(WPoint(m_rect.x + 15.0f, y),
				WPoint(m_rect.x + m_rect.w - 15.0f, y), 0xfff3f3f3, 0xfff3f3f3,
				0);
			GDI()->Line(WPoint(m_rect.x + m_rect.w - 15.0f, y),
				WPoint(m_rect.x + m_rect.w - 3.0f, y), 0xfff3f3f3, 0xfff3f3f3,
				0);
		}
		else
		{
			if (m_bIsIcon)
			{
				p1 = WPoint(m_rect.x, m_rect.y) + WPoint(7.0f, it->top);
				DrawIcon(p1, it->bmp, it->bottom - it->top, it->Enable);
				p1 = WPoint(m_rect.x, m_rect.y) + WPoint(30.0f, it->top);
			}
			else
			{
				p1 = WPoint(m_rect.x, m_rect.y) + WPoint(10.0f, it->top);
			}

			if (it->selected && !it->underCursor && it->Enable)
			{
				GDI()->SetTextStyle(m_selStyle);
				GDI()->SetTextColor(FrALPHA(m_selColor, min(m_hlAlpha, 1.0f)),
					FrALPHA(m_selOutlineColor, min(m_hlAlpha, 1.0f)));
			}
			else if (it->underCursor && it->Enable)
			{
				GDI()->SetTextStyle(m_selStyle);
				GDI()->SetTextColor(FrALPHA(m_selColor, m_wndAlpha),
					FrALPHA(m_selOutlineColor, m_wndAlpha));
			}
			else if (!it->Enable)
			{
				GDI()->SetTextColor(FrALPHA(m_unableColor, m_wndAlpha),
					FrALPHA(m_selOutlineColor, m_wndAlpha));
			}
			else
			{
				GDI()->SetTextStyle(m_style);
				GDI()->SetTextColor(FrALPHA(m_color, m_wndAlpha),
					FrALPHA(m_outlineColor, m_wndAlpha));
			}

			GDI()->PrintText(p1, 0, it->szText, 0);

			GDI()->SetTextStyle(m_style);
			GDI()->SetTextColor(FrALPHA(m_color, m_wndAlpha),
				FrALPHA(m_outlineColor, m_wndAlpha));
		}
	}
}

void FrContextMenuCtrl::OnProc(const float deltaTime)
{
	m_hlAlpha += m_hlAlphaD * deltaTime * 2.0f;

	if (m_hlAlpha < 0.0f)
	{
		m_hlAlpha = 0.0f;
		m_hlAlphaD = 1;
	}
	else if (m_hlAlpha > 3.0f)
	{
		m_hlAlpha = 3.0f;
		m_hlAlphaD = -1;
	}
}

void FrContextMenuCtrl::OnMouseMove(const WPoint& mousePos)
{
	m_underCursor = m_MenuList.end();

	for (MENU_LIST::iterator it = m_MenuList.begin(); it != m_MenuList.end();
		++it)
	{
		if (it->top + m_rect.y <= mousePos.y &&
			it->bottom + m_rect.y > mousePos.y)
		{
			m_underCursor = it;
			it->underCursor = true;
		}
		else
		{
			it->underCursor = false;
		}
	}
}

const char* FrContextMenuCtrl::GetSelectMenuText()
{
	if (m_underCursor->Enable)
		return m_underCursor->szText;

	return "";
}

void FrContextMenuCtrl::SetSelectMenuText(const char* szBuff)
{
	if (m_underCursor->Enable)
	{
		memset(m_underCursor->szText, 0, sizeof(m_underCursor->szText));
		strcpy(m_underCursor->szText, szBuff);
	}
}

void FrContextMenuCtrl::SetMenuList(const char* caption)
{
	FrElementDoc* pDoc = WndManager()->GetDocument();

	m_bIsIcon = false;
	m_MenuList.clear();
	m_underCursor = m_selected = m_MenuList.end();

	cTokenV token;
	token.Init(caption, strlen(caption));

	float fWidth = 0.0f;
	float y = 8.0f;
	sMenuItem item;
	char menu[256];
	char img[256];

	static const char* pImgSep = "@";
	static const char* pSep = "|";

	while (1)
	{
		char* str;
		token.GetToken(&str, pSep, 1);
		if (*str == '\0')
			break;

		memset(menu, 0, sizeof(menu));
		memset(img, 0, sizeof(img));

		const char* p = strstr(str, pImgSep);
		if (p)
		{
			memcpy(menu, str, strlen(str) - strlen(p));
			strcpy(img, p + 1);
			m_bIsIcon = true;
		}
		else
		{
			strcpy(menu, str);
		}

		if (!strstr(menu, "separator") && !strstr(menu, "seperator"))
		{
			item.nType = MENU;
			item.top = y += 4.0f;
			y += GDI()->GetFontHeight() + 3.0f;
		}
		else
		{
			item.nType = SEPARATOR;
			item.top = y;
			y += 10.0f;
		}
		item.bottom = y;

		strncpy(item.szText, menu, 31);
		item.selected = false;
		item.underCursor = false;
		item.Enable = true;
		item.bmp = pDoc->GetBitmap(img);

		float w = GDI()->GetTextExtend(menu);
		if (item.bmp)
			w += item.bmp->Width();
		if (w > fWidth)
			fWidth = w;

		m_MenuList.push_back(item);
	}

	m_rect.w = fWidth + 36.0f;
	m_rect.h = y + 10.0f;
}

bool FrContextMenuCtrl::OnLButtonDown(const WPoint& mousePos)
{
	if (m_underCursor->selected)
		return false;

	PlayPushSound();
	OnMouseMove(mousePos);
	SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, NULL);

	return false;
}

bool FrContextMenuCtrl::OnLButtonUp(const WPoint& mousePos)
{
	if (m_underCursor->selected)
		return false;

	PlayPushSound();
	OnMouseMove(mousePos);
	SetVisible(false);

	return true;
}

void FrContextMenuCtrl::Select(MENU_LIST::iterator i)
{
	if (m_selected != m_MenuList.end())
		m_selected->selected = false;

	m_selected = i;

	if (m_selected != m_MenuList.end())
		m_selected->selected = true;
}

void FrContextMenuCtrl::Unselect(MENU_LIST::iterator i)
{
	if (i != m_MenuList.end())
		i->selected = false;

	if (m_selected == i)
		m_selected = m_MenuList.end();
}

void FrContextMenuCtrl::UpdateRectInfo(int layer, const Bitmap** bmp)
{
	if (!bmp[0])
		return;

	m_min.w = bmp[0]->Width() + bmp[2]->Width();

	int type = m_pFrame->m_aType[layer];
	switch (type)
	{
	case 0:
		m_min.h = bmp[0]->Height() + bmp[6]->Height();
		break;

	case 1:
		m_min.h = bmp[0]->Height();
		break;

	case 2:
		m_min.h = bmp[6]->Height();
		break;
	}

	float width = max(m_min.w, m_rect.w);
	float height =
		m_pFrame->m_aHeight[layer] < 0 ? m_rect.h : m_pFrame->m_aHeight[layer];
	height = max(m_min.h, height);

	if (layer == 0)
	{
		m_width = width;
		m_height = height;
	}

	m_a.w = bmp[0]->Width();
	m_b.w = width - m_min.w;
	m_c.w = bmp[2]->Width();
	m_a.h = type == 2 ? 0.0f : bmp[0]->Height();
	m_b.h = type == 0 ? height - m_min.h : 0.0f;
	m_c.h = type == 1 ? 0.0f : bmp[6]->Height();
}

void FrContextMenuCtrl::DrawFrame(int layer, float x, float y,
	const Bitmap** bmp)
{
	if (m_pFrame->m_aInvisible[layer])
		return;

	if (m_pFrame->m_aPos[layer] < 0)
		y += m_pFrame->m_aPos[layer] + m_rect.h - (m_a.h + m_b.h + m_c.h);
	else
		y += m_pFrame->m_aPos[layer];

	unsigned long color = IsViewFocused() ? m_pFrame->m_color
										  : (m_pFrame->m_color & 0xff000000) |
			((m_pFrame->m_color & 0xffffff) * 4 / 5);
	color = FrALPHA(color, m_wndAlpha);

	int type = m_pFrame->m_aType[layer];
	if (type == 0 || type == 1)
	{
		if (bmp[0])
		{
			GDI()->DrawTexture(bmp[0], WRect(x, y, m_a.w, m_a.h), color, 0);
			GDI()->DrawTexture(bmp[1], WRect(x + m_a.w, y, m_b.w, m_a.h), color,
				0);
			GDI()->DrawTexture(bmp[2],
				WRect(x + m_a.w + m_b.w, y, m_c.w, m_a.h), color, 0);
		}
	}

	if (type == 0)
	{
		if (bmp[3])
		{
			GDI()->DrawTexture(bmp[3], WRect(x, y + m_a.h, m_a.w, m_b.h), color,
				0);
			GDI()->DrawTexture(bmp[4],
				WRect(x + m_a.w, y + m_a.h, m_b.w, m_b.h), color, 0);
			GDI()->DrawTexture(bmp[5],
				WRect(x + m_a.w + m_b.w, y + m_a.h, m_c.w, m_b.h), color, 0);
		}
	}

	if (type == 0 || type == 2)
	{
		if (bmp[6])
		{
			GDI()->DrawTexture(bmp[6],
				WRect(x, y + m_a.h + m_b.h, m_a.w, m_c.h), color, 0);
			GDI()->DrawTexture(bmp[7],
				WRect(x + m_a.w, y + m_a.h + m_b.h, m_b.w, m_c.h), color, 0);
			GDI()->DrawTexture(bmp[8],
				WRect(x + m_a.w + m_b.w, y + m_a.h + m_b.h, m_c.w, m_c.h),
				color, 0);
		}
	}
}

void FrContextMenuCtrl::DrawIcon(WPoint& pt, const Bitmap* bmp, float fHeight,
	bool bEnable)
{
	if (bmp)
	{
		unsigned long color = 0xffffffff;
		if (!bEnable)
			color = FrALPHA(color, 0.2f);

		float x = pt.x - (20.0f - bmp->Width()) * 0.5f;
		float y = pt.y - ((fHeight - bmp->Height()) * 0.5f + 3.0f);
		GDI()->DrawTexture(bmp, WRect(x, y, bmp->Width(), bmp->Height()), color,
			0);
	}
}
