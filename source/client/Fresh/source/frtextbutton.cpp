#include <stdio.h>
#include <string.h>
#include "frtabbutton.h"
#include "frwnd.h"
#include "frdesktop.h"
#include "fremoticon.h"
#include "frform.h"
#include "frframe.h"
#include "frbutton.h"
#include "frstatic.h"
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
#include "commonutil.h"
#include "frtextbutton.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "frcursor.h"
#include "../../../shared/token.h"

IMPLEMENT_OBJECT(FrTextButton, FrWnd)

FrTextButton::FrTextButton()
{
	m_style = 1;
	m_color = 0xffffffff;
	m_outlineColor = 0xff000000;
	m_center = true;
	m_selStyle = 1;
	m_selColor = 0xffffcc00;
	m_selOutlineColor = 0xff000000;
	m_clip = 0;
	m_hlAlpha = 0.0f;
	m_hlAlphaD = 1;
	m_szPushSound = "ui_icon_click";
	m_sepWidth = 5.0f;
	m_underCursor = m_selected = m_tBtnList.end();
}

FrTextButton::~FrTextButton()
{
}

void FrTextButton::Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent)
{
	manager->GetDocument();
	std::map<std::string, std::string>& param = item.m_param;
	if (param.find("style") != param.end())
		sscanf(param["style"].c_str(), "%x", &m_style);
	if (param.find("center") != param.end())
		m_center = strcmpi("true", param["center"].c_str()) == 0;
	if (param.find("color") != param.end())
		sscanf(param["color"].c_str(), "%x", &m_color);
	if (param.find("color2") != param.end())
		sscanf(param["color2"].c_str(), "%x", &m_outlineColor);
	if (param.find("sel_style") != param.end())
		sscanf(param["sel_style"].c_str(), "%x", &m_selStyle);
	if (param.find("select_color") != param.end())
		sscanf(param["select_color"].c_str(), "%x", &m_selColor);
	if (param.find("select_color2") != param.end())
		sscanf(param["select_color2"].c_str(), "%x", &m_selOutlineColor);
	_RectangleSHORT r = item.m_rect;
	Create(item.m_caption.c_str(), item.m_name.c_str(), manager, FWS_VISIBLE,
		WRect(r.left, r.top, r.Width(), r.Height()), parent);
	SetCaption(item.m_caption.c_str());
}

void FrTextButton::SetCaption(const char* caption)
{
	m_caption = caption;
	if (m_center)
	{
		float extent = GDI()->GetTextExtend(caption);
		m_rect.x += (int)(((float)(m_rect.w - extent) + 1.0f) * 0.5f);
		m_rect.w = extent;
	}
	m_tBtnList.clear();
	m_underCursor = m_selected = m_tBtnList.end();
	cTokenV token;
	token.Init(caption, (int)strlen(caption));
	float offset = 0.0f;
	static const char* pSep = (0, "|");
	m_sepWidth = GDI()->GetTextExtend(pSep);
	while (true)
	{
		char* text;
		token.GetToken(&text, pSep, 1);
		if (*text == 0)
			break;
		TButtonItem item;
		item.selected = false;
		item.underCursor = false;
		item.text = text;
		item.left = offset;
		float right = GDI()->GetTextExtend(text) + offset;
		m_rect.w = right;
		item.right = right;
		offset = right + m_sepWidth;
		m_tBtnList.push_back(item);
	}
}

const char* FrTextButton::GetCaption()
{
	return m_caption.c_str();
}

void FrTextButton::SetClippingArea(WRect* rectangle)
{
	if (rectangle)
	{
		if (m_clip == 0)
		{
			m_clip = new WRect(*rectangle);
		}
		else
		{
			m_clip->x = rectangle->x;
			m_clip->y = rectangle->y;
			m_clip->w = rectangle->w;
			m_clip->h = rectangle->h;
		}
	}
	else if (m_clip)
	{
		delete m_clip;
		m_clip = 0;
	}
}

const char* FrTextButton::GetUnderCursor()
{
	if (m_underCursor == m_tBtnList.end())
		return 0;
	return m_underCursor->text.c_str();
}

const char* FrTextButton::GetSelected()
{
	if (m_selected == m_tBtnList.end())
		return 0;
	return m_selected->text.c_str();
}

void FrTextButton::Select(int index)
{
	TBUTTON_LIST::iterator it = m_tBtnList.begin();
	for (; it != m_tBtnList.end(); ++it)
	{
		if (index <= 0)
			break;
		--index;
	}
	if (it == m_tBtnList.end())
		--it;
	Select(it);
}

void FrTextButton::Unselect(int index)
{
	TBUTTON_LIST::iterator it = m_tBtnList.begin();
	for (; it != m_tBtnList.end(); ++it)
	{
		if (index <= 0)
			break;
		--index;
	}
	if (it == m_tBtnList.end())
		--it;
	Unselect(it);
}

void FrTextButton::Select(TBUTTON_LIST::iterator item)
{
	if (m_selected != m_tBtnList.end())
		m_selected->selected = false;
	m_selected = item;
	if (item != m_tBtnList.end())
		item->selected = true;
}

void FrTextButton::Unselect(TBUTTON_LIST::iterator item)
{
	if (item != m_tBtnList.end())
		item->selected = false;
	if (m_selected == item)
		m_selected = m_tBtnList.end();
}

void FrTextButton::OnDraw()
{
	FrWnd::OnDraw();
	if (m_clip)
		GDI()->SetClippingArea(m_clip);
	TBUTTON_LIST::iterator it = m_tBtnList.begin();
	while (it != m_tBtnList.end())
	{
		TButtonItem& item = *it;
		WPoint left = WPoint(m_rect.x, m_rect.y) + WPoint(item.left, 0.0f);
		WPoint right = WPoint(m_rect.x, m_rect.y) + WPoint(item.right, 0.0f);
		if (item.selected && !item.underCursor)
		{
			GDI()->SetTextStyle(m_selStyle);
			GDI()->SetTextColor(
				FrALPHA(m_selColor, m_hlAlpha < 1.0f ? m_hlAlpha : 1.0f),
				FrALPHA(m_selOutlineColor,
					m_hlAlpha < 1.0f ? m_hlAlpha : 1.0f));
		}
		else if (item.underCursor)
		{
			GDI()->SetTextStyle(m_selStyle);
			GDI()->SetTextColor(FrALPHA(m_selColor, m_wndAlpha),
				FrALPHA(m_selOutlineColor, m_wndAlpha));
		}
		else
		{
			GDI()->SetTextStyle(m_style);
			GDI()->SetTextColor(FrALPHA(m_color, m_wndAlpha),
				FrALPHA(m_outlineColor, m_wndAlpha));
		}
		GDI()->Print(left, 0, item.text.c_str());
		GDI()->SetTextStyle(m_style);
		GDI()->SetTextColor(FrALPHA(m_color, m_wndAlpha),
			FrALPHA(m_outlineColor, m_wndAlpha));
		++it;
		if (it == m_tBtnList.end())
			break;
		static const char* pSep = (0, "|");
		GDI()->Print(right, 0, pSep);
	}
	if (m_clip)
		GDI()->SetClippingArea(0);
}

void FrTextButton::OnProc(float elapsed)
{
	m_hlAlpha += (float)m_hlAlphaD * elapsed * 2.0f;
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

void FrTextButton::OnMouseMove(const WPoint& point)
{
	m_underCursor = m_tBtnList.end();
	TBUTTON_LIST::iterator it = m_tBtnList.begin();
	for (; it != m_tBtnList.end(); ++it)
	{
		if (it->left + m_rect.x <= point.x &&
			m_sepWidth + it->right + m_rect.x > point.x)
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

void FrTextButton::OnSetCursor(bool inside, const WPoint& point)
{
	if (inside)
	{
		if (m_underCursor != m_tBtnList.end())
			SetCursor(FrCursor::ROLL_OVER);
	}
	else if (m_underCursor != m_tBtnList.end())
	{
		m_underCursor->underCursor = false;
		m_underCursor = m_tBtnList.end();
	}
}

bool FrTextButton::OnLButtonUp(const WPoint& point)
{
	if (m_underCursor->selected)
		return false;
	PlayPushSound();
	OnMouseMove(point);
	Select(m_underCursor);
	SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, 0);
	return true;
}
