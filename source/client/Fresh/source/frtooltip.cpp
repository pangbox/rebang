#include "frtooltip.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "inputmanager.h"
#include "wresrcmng.h"
#include "wutil.h"
#include "commonutil.h"

extern const float TEXT_TOOLTIP_LINE_INTERVAL = 2.0f;
const float CURSOR_PIVOT = 20.0f;
const float TEXT_TOOLTIP_PIVOT_X = 8.0f;
const float TEXT_TOOLTIP_PIVOT_Y = 4.0f;
const float FRMIMG_SIZE = 8.0f;
const float MAX_TOOLTIP_ALPHA = 220.0f;
const float TOOLTIP_FRM_PIVOT = 1.0f;

namespace
{
	int FixRectInView(WRect& rc)
	{
		WRect copy(rc);
		if (rc.x < 0.0f)
			rc.x = 0.0f;
		if (rc.y < 0.0f)
			rc.y = 0.0f;
		if (rc.Right() > g_view->GetWidth())
			rc.x = g_view->GetWidth() - rc.w;
		if (rc.Bottom() > g_view->GetHeight())
			rc.y = g_view->GetHeight() - rc.h;
		return copy != rc;
	}
}

FrToolTip::FrToolTip(FrWndManager* pWndMgr)
	: m_fAlpha(0.0f), m_pWndManager(pWndMgr), m_textColor(0xff000000)
{
	m_Frame.Init();
}

FrToolTip::~FrToolTip()
{
}

void FrToolTip::Move(const WPoint& pos)
{
	m_rcWnd.x = pos.x;
	m_rcWnd.y = pos.y;
}

void FrToolTip::SetToolTipText(const std::string& text)
{
	if (text == "")
		return;
	m_fAlpha = MAX_TOOLTIP_ALPHA;
	if (m_orgText == text)
		return;
	m_orgText = text;
	m_FrText = m_orgText;
	Prepare();
}

void FrToolTip::SetFrameStyle(unsigned long style)
{
	m_Frame.SetStyle(style);
}

void FrToolTip::OnProcess(float deltaTime)
{
	if (m_fAlpha <= 0.0f)
		return;
	if (m_fAlpha > 0.0f)
	{
		m_fAlpha -= deltaTime * MAX_TOOLTIP_ALPHA * 3.0f;
		m_rcWnd.x = m_pWndManager->GetMousePos().x + CURSOR_PIVOT;
		m_rcWnd.y = m_pWndManager->GetMousePos().y + CURSOR_PIVOT;
		FixRectInView(m_rcWnd);
	}
	if (m_fAlpha <= 0.0f)
		Reset();
}

void FrToolTip::OnDisplay()
{
	if (m_fAlpha <= 0.0f)
		return;
	m_Frame.Render(m_rcWnd, ((unsigned char)m_fAlpha << 24) | 0xffffff);
	WPoint pos(m_rcWnd.x + TEXT_TOOLTIP_PIVOT_X,
		m_rcWnd.y + TEXT_TOOLTIP_PIVOT_Y + TOOLTIP_FRM_PIVOT);
	FrGraphicInterface* pGDI = m_pWndManager->GetGDI();
	pGDI->SetTextColor(m_textColor, 0xffffffff);
	float prevAlpha = pGDI->GetAlpha();
	pGDI->SetAlpha(m_fAlpha / 255.0f);
	pGDI->PrintText11(pos, m_FrText);
	pGDI->SetAlpha(prevAlpha);
}

void FrToolTip::Prepare()
{
	m_fAlpha = MAX_TOOLTIP_ALPHA;
	m_rcWnd.x = g_input->GetMousePoint().x + CURSOR_PIVOT;
	m_rcWnd.y = g_input->GetMousePoint().y + CURSOR_PIVOT;
	FrGraphicInterface* pGDI = m_pWndManager->GetGDI();
	unsigned int lines = m_FrText.GetLineSize();
	m_rcWnd.h = (float)pGDI->GetFontHeight11() * lines +
		Max(0, (int)lines - 1) * TEXT_TOOLTIP_LINE_INTERVAL +
		TEXT_TOOLTIP_PIVOT_Y * 2;
	m_rcWnd.w = m_FrText.GetWidthLong(pGDI) + TEXT_TOOLTIP_PIVOT_X * 2;
}

void FrToolTip::Reset()
{
	m_fAlpha = 0.0f;
	m_rcWnd = WRect();
	m_orgText = "";
	m_FrText.Clear();
}

FrToolTip::cToolTipFrame::cToolTipFrame()
	: m_style(3)
{
}

FrToolTip::cToolTipFrame::~cToolTipFrame()
{
	for (std::vector<TOOLTIPFRM>::iterator it = m_Frames.begin();
		it != m_Frames.end(); ++it)
	{
		for (int i = 0; i < FRMIDX_MAX; ++i)
		{
			if (g_resrcmng && (*it)[i])
			{
				g_resrcmng->Release((*it)[i]);
				(*it)[i] = NULL;
			}
		}
		(*it).clear();
	}
	m_Frames.clear();
}

void FrToolTip::cToolTipFrame::Init()
{
	m_Frames.reserve(FRTS_MAX);
	for (int iFrm = 0; iFrm < FRTS_MAX; ++iFrm)
	{
		TOOLTIPFRM Frame(FRMIDX_MAX);
		for (int i = 0; i < FRMIDX_MAX; ++i)
		{
			Frame.push_back(NULL);
			WOverlay* overlay = g_resrcmng->GetOverlay(
				MakeStr("tooltip_frm%02d_%02d.tga", iFrm, i), 0);
			if (overlay)
				Frame[i] = overlay;
		}
		m_Frames.push_back(Frame);
	}
}

void FrToolTip::cToolTipFrame::SetStyle(unsigned long style)
{
	m_style = Between<unsigned long>(FRTS_DEFAULT, style, FRTS_MAX - 1);
}

void FrToolTip::cToolTipFrame::Render(const WRect& rc, unsigned long color)
{
	float fw = rc.w - FRMIMG_SIZE * 2;
	float fh = rc.h - FRMIMG_SIZE * 2;
	TOOLTIPFRM& Frame = m_Frames[m_style];
	WRect src(0.0f, 0.0f, 1.0f, 1.0f);
	if (Frame[LT])
		Frame[LT]->Render(g_view, src,
			WRect(rc.x, rc.y, FRMIMG_SIZE, FRMIMG_SIZE), 0, color, 0, 0);
	if (Frame[LM])
		Frame[LM]->Render(g_view, src,
			WRect(rc.x, rc.y + FRMIMG_SIZE, FRMIMG_SIZE, fh), 0, color, 0, 0);
	if (Frame[LB])
		Frame[LB]->Render(g_view, src,
			WRect(rc.x, rc.y + fh + FRMIMG_SIZE, FRMIMG_SIZE, FRMIMG_SIZE), 0,
			color, 0, 0);
	if (Frame[MT])
		Frame[MT]->Render(g_view, src,
			WRect(rc.x + FRMIMG_SIZE, rc.y, fw, FRMIMG_SIZE), 0, color, 0, 0);
	if (Frame[MM])
		Frame[MM]->Render(g_view, src,
			WRect(rc.x + FRMIMG_SIZE, rc.y + FRMIMG_SIZE, fw, fh), 0, color, 0,
			0);
	if (Frame[MB])
		Frame[MB]->Render(g_view, src,
			WRect(rc.x + FRMIMG_SIZE, rc.y + fh + FRMIMG_SIZE, fw, FRMIMG_SIZE),
			0, color, 0, 0);
	if (Frame[RT])
		Frame[RT]->Render(g_view, src,
			WRect(rc.x + fw + FRMIMG_SIZE, rc.y, FRMIMG_SIZE, FRMIMG_SIZE), 0,
			color, 0, 0);
	if (Frame[RM])
		Frame[RM]->Render(g_view, src,
			WRect(rc.x + fw + FRMIMG_SIZE, rc.y + FRMIMG_SIZE, FRMIMG_SIZE, fh),
			0, color, 0, 0);
	if (Frame[RB])
		Frame[RB]->Render(g_view, src,
			WRect(rc.x + fw + FRMIMG_SIZE, rc.y + fh + FRMIMG_SIZE, FRMIMG_SIZE,
				FRMIMG_SIZE),
			0, color, 0, 0);
}
