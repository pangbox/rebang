#include <stdlib.h>
#include "frstatic.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "commonutil.h"

IMPLEMENT_OBJECT(FrStatic, FrWnd)

FrStatic::FrStatic()
{
	m_style = 0;
	m_align = 0;
	m_clip = NULL;
	m_smallFont = false;
	m_color = 0xff808080;
	m_outlineColor = 0xffffffff;
}

FrStatic::~FrStatic()
{
}

void FrStatic::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();
	m_caption = item.m_caption;

	std::map<std::string, std::string>& param = item.m_param;
	if (param.find("style") != param.end())
		m_style = atoi(param["style"].c_str());

	if (param.find("align") != param.end())
		m_align = atoi(param["align"].c_str());

	if (param.find("color") != param.end())
		m_color = htoi(param["color"].c_str());

	if (m_style & 6)
		m_outlineColor = htoi(param["color2"].c_str());

	_RectangleSHORT r = item.m_rect;
	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager, FWS_VISIBLE,
		WRect(r.left, r.top, r.Width(), r.Height()), pParent);
}

void FrStatic::SetClippingArea(WRect* clip)
{
	if (clip)
	{
		if (!m_clip)
			m_clip = new WRect(*clip);
		else
		{
			m_clip->x = clip->x;
			m_clip->y = clip->y;
			m_clip->w = clip->w;
			m_clip->h = clip->h;
		}
	}
	else if (m_clip)
	{
		delete m_clip;
		m_clip = NULL;
	}
}

void FrStatic::SetTextStyle(unsigned long style)
{
	m_style = style;
}

void FrStatic::SetTextColor(unsigned long color, unsigned long outline_color)
{
	m_color = color;
	m_outlineColor = outline_color;
}

void FrStatic::OnDraw()
{
	FrWnd::OnDraw();

	if (WndManager()->HidePrivacy() && m_nFlags.GetFlag(FWF_PRIVACY))
		return;

	if (m_clip)
		GDI()->SetClippingArea(m_clip);

	if (IsEnabled())
		GDI()->SetTextColor(FrALPHA(m_color, m_wndAlpha2 * m_wndAlpha),
			FrALPHA(m_outlineColor, m_wndAlpha2 * m_wndAlpha));
	else
		GDI()->SetTextColor(FrALPHA(m_color, 0.4f), 0xffffffff);

	GDI()->SetTextStyle(m_style);
	if (m_smallFont)
		WndManager()->PrintText11(WPoint(m_rect.x, m_rect.y), m_align,
			m_caption.c_str(), -1.0f, 0xffffffff);
	else
		WndManager()->PrintText(WPoint(m_rect.x, m_rect.y), m_align,
			m_caption.c_str(), -1.0f, 0xffffffff);

	if (m_clip)
		GDI()->SetClippingArea(NULL);
}

void FrStatic::Enable(bool enable)
{
	m_dwStyle.Turn(FWS_DISABLED, !enable);
}
