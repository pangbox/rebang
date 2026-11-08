#include "frgaugebar.h"
#include "frbutton.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include <stdio.h>
#include "wutil.h"

typedef std::map<std::string, std::string> FrParamMap;

IMPLEMENT_OBJECT(FrGaugeBar, FrWnd)

FrGaugeBar::FrGaugeBar()
{
	m_exColor[0] = 0xff707070;
	m_exColor[1] = 0xff707070;
	m_color = 0xffffffff;
	m_bgColor = 0xff404040;
	m_thickness = 2;
	m_lower = 0;
	m_upper = 100;
	m_origin = 0;
	m_expand = 0.0f;
	m_pos = 0.0f;
	m_srcPos = 0.0f;
	m_destPos = 0.0f;
	m_speed = 65.0f;
	CalcBarRect(0);
}

FrGaugeBar::~FrGaugeBar()
{
}

void FrGaugeBar::Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent)
{
	manager->GetDocument();
	m_pItem = &item;
	FrParamMap& param = item.m_param;
	if (param.find("color") != param.end())
		sscanf(param["color"].c_str(), "%x", &m_color);
	if (param.find("bgcolor") != param.end())
		sscanf(param["bgcolor"].c_str(), "%x", &m_bgColor);
	if (param.find("excolor1") != param.end())
		sscanf(param["excolor1"].c_str(), "%x", &m_exColor[0]);
	if (param.find("excolor2") != param.end())
		sscanf(param["excolor2"].c_str(), "%x", &m_exColor[1]);
	if (param.find("border_thickness") != param.end())
		sscanf(param["border_thickness"].c_str(), "%d", &m_thickness);
	WRect bounds;
	bounds.x = (float)item.m_rect.left;
	bounds.y = (float)item.m_rect.top;
	bounds.w = (float)(short)(item.m_rect.right - item.m_rect.left);
	bounds.h = (float)(short)(item.m_rect.bottom - item.m_rect.top);
	Create(item.m_caption.c_str(), item.m_name.c_str(), manager, 1, bounds,
		parent);
}

void FrGaugeBar::SetRange(int lower, int upper, int origin)
{
	m_lower = lower;
	m_upper = upper;
	if (origin < lower)
		m_origin = lower;
	else if (origin > upper)
		m_origin = upper;
	else
		m_origin = origin;
	if (m_pos > (float)upper)
		m_pos = (float)upper;
	else if (m_pos < (float)lower)
		m_pos = (float)lower;
	if (m_destPos > (float)upper)
		m_destPos = (float)upper;
	else if (m_destPos < (float)lower)
		m_destPos = (float)lower;
	CalcBarRect(0);
}

void FrGaugeBar::SetPos(int pos)
{
	if (pos > m_upper)
		m_pos = (float)m_upper;
	else if (pos < m_lower)
		m_pos = (float)m_lower;
	else
		m_pos = (float)pos;
	m_destPos = m_pos;
	CalcBarRect(0);
}

void FrGaugeBar::SetExpand(int expand)
{
	m_expand = (float)expand;
}

void FrGaugeBar::SetDestPos(int pos, bool achilles)
{
	m_achillesRun = achilles;
	if (pos > m_upper)
		m_destPos = (float)m_upper;
	else if (pos < m_lower)
		m_destPos = (float)m_lower;
	else
		m_destPos = (float)pos;
}

void FrGaugeBar::MoveWindow(const WPoint& point)
{
	FrWnd::MoveWindow(point);
	m_barRect.x = (float)m_thickness + m_rect.x;
	m_barRect.y = (float)m_thickness + m_rect.y;
}

void FrGaugeBar::CalcBarRect(const WRect* rectangle)
{
	if (rectangle == 0)
		m_barRect = m_rect;
	else
		m_barRect = *rectangle;
	float thick = (float)(m_thickness * 2);
	float scale = (m_barRect.w - thick) / (float)(m_upper - m_lower);
	float origin = (float)m_origin;
	if (origin < m_pos)
	{
		m_barRect.x = (float)(m_origin - m_lower) * scale + m_barRect.x;
		m_barRect.w = (m_pos - origin) * scale;
	}
	else
	{
		m_barRect.x = (m_pos - (float)m_lower) * scale + m_barRect.x;
		m_barRect.w = (origin - m_pos) * scale;
	}
	m_barRect.x = (float)m_thickness + m_barRect.x;
	m_barRect.y = (float)m_thickness + m_barRect.y;
	m_barRect.h = m_barRect.h - thick;
}

void FrGaugeBar::OnProc(float elapsed)
{
	float step;
	if (m_achillesRun && m_destPos != m_srcPos)
		step = (m_destPos - m_pos) * m_speed * elapsed / (m_destPos - m_srcPos);
	else
		step = elapsed * m_speed;
	if (m_pos > m_destPos)
	{
		step = m_pos - step;
		m_pos = step > m_destPos ? step : m_destPos;
	}
	else if (m_pos < m_destPos)
	{
		step = step + m_pos;
		m_pos = step < m_destPos ? step : m_destPos;
	}
	CalcBarRect(0);
}

void FrGaugeBar::OnDraw()
{
	FrWnd::OnDraw();
	GDI()->Box(m_rect, m_bgColor);
	float value = m_expand * m_rect.w / (float)(m_upper - m_lower);
	float expand = value > 0 ? value : -value;
	if (m_expand > 0.0f)
	{
		expand = Between(0.0f, expand,
			m_rect.x + m_rect.w - m_thickness - (m_barRect.x + m_barRect.w));
		GDI()->Box(m_barRect, m_color);
		GDI()->Box(
			WRect(m_barRect.x + m_barRect.w, m_barRect.y, expand, m_barRect.h),
			m_exColor[0]);
	}
	else if (m_expand < 0.0f)
	{
		expand = Min(m_barRect.w, expand);
		GDI()->Box(
			WRect(m_barRect.x, m_barRect.y, m_barRect.w - expand, m_barRect.h),
			m_color);
		GDI()->Box(WRect(m_barRect.x + m_barRect.w - expand, m_barRect.y,
					   expand, m_barRect.h),
			m_exColor[1]);
	}
	else
	{
		GDI()->Box(m_barRect, m_color);
	}
}

IMPLEMENT_OBJECT(FrGaugeBarEx, FrGaugeBar)

BEGIN_FRESH_MSGMAP(FrGaugeBarEx, FrGaugeBar)
ON_FRESH_VV("left", FRCMD_LBUTTONDOWN, FrGaugeBarEx::OnLLButtonDown)
ON_FRESH_VV("right", FRCMD_LBUTTONDOWN, FrGaugeBarEx::OnRLButtonDown)
END_FRESH_MSGMAP()

FrGaugeBarEx::FrGaugeBarEx()
{
	m_borderColor = 0xff808080;
	m_color2 = 0xffffffff;
	m_thickness = 3;
	m_fPos = 0.0f;
	m_steps = 1;
	m_pLButton = 0;
	m_pRButton = 0;
	CalcBarRect(0);
}

FrGaugeBarEx::~FrGaugeBarEx()
{
}

void FrGaugeBarEx::Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent)
{
	FrGaugeBar::Init(item, manager, parent);
	FrElementDoc* document = manager->GetDocument();
	FrParamMap& param = m_pItem->m_param;
	if (param.find("bordercolor") != param.end())
		sscanf(param["bordercolor"].c_str(), "%x", &m_borderColor);
	if (param.find("color2") != param.end())
		sscanf(param["color2"].c_str(), "%x", &m_color2);
	else
		m_color2 = m_color;
	if (param.find("steps") != param.end())
		sscanf(param["steps"].c_str(), "%d", &m_steps);
	m_gaugeRect = m_rect;
	if (param.find("left") != param.end() && param["left"] == "true")
	{
		m_pLButton = new FrButton;
		if (m_pLButton)
		{
			FrGuiItem& info = m_LButtonInfo;
			info = item;
			info.m_caption = "left";
			info.m_name = "left";
			info.m_rect.bottom = 0;
			info.m_rect.right = 0;
			info.m_rect.top = 0;
			info.m_rect.left = 0;
			m_pLButton->SetOwner(this);
			m_pLButton->Init(info, manager, this, true);
			m_pLButton->UseDblClick(false);
			m_pLButton->SetPushDelay(0.0f);
			WRect bounds = m_pLButton->GetRect();
			m_gaugeRect.x = bounds.w + m_gaugeRect.x + 2.0f;
			m_gaugeRect.w = m_gaugeRect.w - (bounds.w + 2.0f);
			bounds.x = bounds.x + 2.0f;
			bounds.y =
				(float)(int)((m_rect.h - bounds.h + 1.0f) * 0.5f) + bounds.y;
			m_pLButton->SetRect(bounds);
		}
	}
	if (param.find("right") != param.end() && param["right"] == "true")
	{
		m_pRButton = new FrButton;
		if (m_pRButton)
		{
			FrGuiItem& info = m_RButtonInfo;
			info = item;
			info.m_caption = "right";
			info.m_name = "right";
			info.m_rect.bottom = 0;
			info.m_rect.right = 0;
			info.m_rect.top = 0;
			info.m_rect.left = 0;
			m_pRButton->SetOwner(this);
			m_pRButton->Init(info, manager, this, false);
			m_pRButton->UseDblClick(false);
			m_pRButton->SetPushDelay(0.0f);
			FrParamMap& skins = m_pItem->m_param;
			FrButton::eButStyle style = FrButton::BT_NORMAL;
			FrButton::eButPushStyle push = FrButton::BP_PUSHBUTTON;
			const Bitmap* pBmp[3];
			pBmp[0] = document->GetBitmap(skins["rnormal"]);
			pBmp[1] = document->GetBitmap(skins["rover"]);
			pBmp[2] = document->GetBitmap(skins["rselected"]);
			if (skins.find("style") != skins.end())
				style = (FrButton::eButStyle)atoi(skins["style"].c_str());
			if (skins.find("pushstyle") != skins.end())
				push =
					(FrButton::eButPushStyle)atoi(skins["pushstyle"].c_str());
			m_pRButton->InitExtern(pBmp[0], pBmp[1], pBmp[2], style, push);
			WRect bounds = m_pRButton->GetRect();
			bounds.x = (m_rect.w - bounds.w - 2.0f) + bounds.x;
			bounds.y =
				(float)(int)((m_rect.h - bounds.h + 1.0f) * 0.5f) + bounds.y;
			m_gaugeRect.w = m_gaugeRect.w - (bounds.w + 2.0f);
			m_pRButton->SetRect(bounds);
		}
	}
}

void FrGaugeBarEx::SetPos(int pos)
{
	FrGaugeBar::SetPos(pos);
	m_fPos = m_pos;
}

void FrGaugeBarEx::SetSteps(int steps)
{
	m_steps = Max(1, steps);
}

int FrGaugeBarEx::GetSteps()
{
	return m_steps;
}

void FrGaugeBarEx::OnDraw()
{
	unsigned long rainbow[4];
	rainbow[0] = m_color;
	rainbow[1] = m_colorP;
	rainbow[2] = m_color;
	rainbow[3] = m_colorP;
	FrWnd::OnDraw();
	if (m_dwStyle & FWS_DISABLED)
	{
		for (int i = 0; i < 4; ++i)
			rainbow[i] = Modulate(rainbow[i], 0xb0b0b0) | 0x50000000;
	}
	GDI()->Box(m_rect, m_bgColor);
	WRect box(m_rect.x, m_rect.y, m_rect.w - 1.0f, m_rect.h - 1.0f);
	GDI()->LineBox(box, m_borderColor);
	GDI()->RainbowBox(m_barRect, rainbow);
	float pitch =
		(m_gaugeRect.w - m_thickness * 2.0f - (m_steps - 1.0f) * 2.0f) /
		m_steps;
	box.x = (float)m_thickness + m_gaugeRect.x + pitch;
	box.y = (float)m_thickness + m_gaugeRect.y;
	box.h = m_gaugeRect.h - m_thickness * 2.0f;
	box.w = 2.0f;
	for (int step = 0; step < m_steps - 1; ++step)
	{
		GDI()->Box(box, m_bgColor, 0, 0.0f);
		box.x += pitch + 2.0f;
	}
}

void FrGaugeBarEx::CalcBarRect(const WRect*)
{
	FrGaugeBar::CalcBarRect(&m_gaugeRect);
	float fade = 1.0f - m_fPos * 0.01f;
	unsigned long packed;
	float v;
	v = (float)((m_color >> 24) & 0xff) * fade;
	v += (float)((m_color2 >> 24) & 0xff) * m_fPos * 0.01f;
	packed = (unsigned char)v;
	packed <<= 8;
	v = (float)((m_color >> 16) & 0xff) * fade;
	v += (float)((m_color2 >> 16) & 0xff) * m_fPos * 0.01f;
	packed |= (unsigned char)v;
	packed <<= 8;
	v = (float)((m_color >> 8) & 0xff) * fade;
	v += (float)((m_color2 >> 8) & 0xff) * m_fPos * 0.01f;
	packed |= (unsigned char)v;
	packed <<= 8;
	v = (float)(m_color & 0xff) * fade;
	v += (float)(m_color2 & 0xff) * m_fPos * 0.01f;
	packed |= (unsigned char)v;
	m_colorP = packed;
}

void FrGaugeBarEx::OnLLButtonDown()
{
	m_fPos = m_fPos - (float)(m_upper - m_lower) / m_steps;
	if (m_fPos < (float)m_lower)
		m_fPos = (float)m_lower;
	m_pos = m_fPos;
	m_destPos = m_pos;
	CalcBarRect(0);
}

void FrGaugeBarEx::OnRLButtonDown()
{
	m_fPos = (float)(m_upper - m_lower) / m_steps + m_fPos;
	if (m_fPos > (float)m_upper)
		m_fPos = (float)m_upper;
	m_pos = m_fPos;
	m_destPos = m_pos;
	CalcBarRect(0);
}

void FrGaugeBarEx::Enable(bool enabled)
{
	if (!enabled)
		m_dwStyle.Enable(FWS_DISABLED);
	else
		m_dwStyle.Disable(FWS_DISABLED);
	if (m_pLButton)
	{
		m_pLButton->Enable(enabled);
	}
	if (m_pRButton)
	{
		m_pRButton->Enable(enabled);
	}
}

void FrGaugeBarEx::MoveWindow(const WPoint& point)
{
	float offset = m_gaugeRect.x - m_rect.x;
	FrWnd::MoveWindow(point);
	m_gaugeRect.x = offset + m_rect.x;
	m_gaugeRect.y = m_rect.y;
	m_barRect.x = (float)m_thickness + m_gaugeRect.x;
	m_barRect.y = (float)m_thickness + m_gaugeRect.y;
}

IMPLEMENT_OBJECT(FrGaugeBarImage, FrGaugeBar)

FrGaugeBarImage::FrGaugeBarImage()
{
	m_pBitImage[0] = 0;
	m_pBitImage[1] = 0;
	m_BitMask = 0;
	m_start = 0.0f;
	m_iPixelCounter = 0;
	m_bOwnerDraw = false;
	m_complete = 0.0f;
	m_thickness = 1;
}

FrGaugeBarImage::~FrGaugeBarImage()
{
	GDI()->InvalidateCache(*m_BitMask);
	if (m_BitMask)
	{
		delete m_BitMask;
		m_BitMask = 0;
	}
}

void FrGaugeBarImage::Init(FrGuiItem& item, FrWndManager* manager,
	FrWnd* parent)
{
	FrGaugeBar::Init(item, manager, parent);
	FrElementDoc* document = manager->GetDocument();
	FrParamMap& param = m_pItem->m_param;
	m_bOwnerDraw = atoi(param["drawstyle"].c_str()) == 1;
	m_pBitImage[0] = document->GetBitmap(param["normal"]);
	m_pBitImage[1] = document->GetBitmap(param["mask"]);
	if (m_pBitImage[0] == 0 || m_pBitImage[1] == 0)
		MessageBoxA(0,
			"\xc0\xcc\xb9\xcc\xc1\xf6 \xc6\xc4\xc0\xcf\xc0\xcc "
			"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			"\xb0\xe6\xb0\xed", 0);
	long width0 = m_pBitImage[0]->bi->bmiHeader.biWidth;
	if (width0 != m_pBitImage[1]->bi->bmiHeader.biWidth)
		MessageBoxA(0,
			"\xb8\xb6\xbd\xba\xc5\xa9\xc0\xc7 \xb3\xd0\xc0\xcc\xb0\xa1 "
			"\xc6\xb2\xb8\xb3\xb4\xcf\xb4\xd9.",
			"\xb0\xe6\xb0\xed", 0);
	m_gaugeRect.x = m_rect.x;
	m_gaugeRect.y = m_rect.y;
	unsigned long w = m_pBitImage[0]->bi->bmiHeader.biWidth;
	m_gaugeRect.w = (float)w;
	m_gaugeRect.h =
		(float)(unsigned long)m_pBitImage[0]->bi->bmiHeader.biHeight;
	m_BitMask = new Bitmap;
	m_BitMask->Create(m_pBitImage[1]->bi->bmiHeader.biWidth,
		m_pBitImage[1]->bi->bmiHeader.biHeight,
		m_pBitImage[1]->bi->bmiHeader.biBitCount);
	memcpy(m_BitMask->vram, m_pBitImage[1]->vram,
		m_pBitImage[1]->Height() * m_pBitImage[1]->pitch);
	m_start = 0.0f;
	SetRange(0, (int)m_gaugeRect.w, 0);
}

const Bitmap* FrGaugeBarImage::GetGaugeSkin(int skin)
{
	if (skin == 1)
		return m_BitMask;
	return m_pBitImage[0];
}

void FrGaugeBarImage::ResetGauge()
{
	memcpy(m_BitMask->vram, m_pBitImage[1]->vram,
		m_pBitImage[1]->Height() * m_pBitImage[1]->pitch);
	GDI()->RefreshTextureCacheInfo(m_BitMask);
	m_start = 0.0f;
	m_pos = 0.0f;
	m_gaugeRect.x = m_rect.x;
	m_gaugeRect.y = m_rect.y;
	m_iPixelCounter = 0;
}

int FrGaugeBarImage::GetPixelPerHeight()
{
	return m_iPixelCounter;
}

float FrGaugeBarImage::GetComplete()
{
	return m_complete;
}

void FrGaugeBarImage::OnDraw()
{
	if (m_pBitImage[0] != 0 && m_pBitImage[1] != 0)
	{
		FrWnd::OnDraw();
		if (!m_bOwnerDraw)
		{
			const Bitmap* skin = m_pBitImage[0];
			unsigned long h = skin->Height();
			unsigned long w = skin->Width();
			WRect box;
			box.x = m_gaugeRect.x;
			box.y = m_gaugeRect.y;
			box.w = (float)w;
			box.h = (float)h;
			GDI()->DrawTexture(skin, box, 0xffffffff, 0);
			skin = m_BitMask;
			h = skin->Height();
			w = skin->Width();
			box.x = m_gaugeRect.x;
			box.y = m_gaugeRect.y;
			box.w = (float)w;
			box.h = (float)h;
			GDI()->DrawTexture(skin, box, 0xffffffff, 0);
		}
		else
		{
			SendCmdToOwnerTarget(FRCMD_OWNERDRAW, 0, 0);
		}
	}
}

void FrGaugeBarImage::CalcBarRect(const WRect*)
{
	m_barRect = m_gaugeRect;
	float thick = (float)(m_thickness * 2);
	float scale = (m_barRect.w - thick) / (float)(m_upper - m_lower);
	float origin = (float)m_origin;
	if (origin < m_pos)
	{
		m_barRect.x = (float)(m_origin - m_lower) * scale + m_barRect.x;
		m_barRect.w = (m_pos - origin) * scale;
	}
	else
	{
		m_barRect.x = (m_pos - (float)m_lower) * scale + m_barRect.x;
		m_barRect.w = (origin - m_pos) * scale;
	}
	m_barRect.x = (float)m_thickness + m_barRect.x;
	m_barRect.y = (float)m_thickness + m_barRect.y;
	m_barRect.h = m_barRect.h - thick;
	if (m_start != m_barRect.w && m_pBitImage[1] != 0)
	{
		int column = (int)floor(m_start);
		while (column < (int)m_barRect.w)
		{
			for (unsigned int row = 0; row < m_pBitImage[1]->Height(); ++row)
				m_BitMask->SetAlpha(column, row, 0);
			m_iPixelCounter = m_iPixelCounter + 1;
			column = column + 1;
		}
		m_start = m_barRect.w;
		m_complete = (m_start * 100.0f) / m_barRect.w;
		GDI()->RefreshTextureCacheInfo(m_BitMask);
	}
}

void FrGaugeBarImage::OnProc(float elapsed)
{
	float step;
	if (m_achillesRun && m_destPos != m_srcPos)
		step = (m_destPos - m_pos) * m_speed * elapsed / (m_destPos - m_srcPos);
	else
		step = elapsed * m_speed;
	if (m_pos > m_destPos)
	{
		step = m_pos - step;
		m_pos = step > m_destPos ? step : m_destPos;
	}
	else if (m_pos < m_destPos)
	{
		step = step + m_pos;
		m_pos = step < m_destPos ? step : m_destPos;
	}
	CalcBarRect(0);
}

void FrGaugeBarImage::MoveWindow(const WPoint& point)
{
	FrWnd::MoveWindow(point);
	m_gaugeRect.x = point.x;
	m_gaugeRect.y = point.y;
}
