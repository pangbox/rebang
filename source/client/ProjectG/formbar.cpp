#include "formbar.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "frbutton.h"
#include "fredit.h"
#include "frarea.h"

extern Fresh* g_pFresh;
IMPLEMENT_OBJECT(FrFormBar, FrForm)
BEGIN_FRESH_MSGMAP(FrFormBar, FrForm)

ON_FRESH_VI("gauge_area", FRCMD_INIT, FrFormBar::OnInitGaugeArea)
ON_FRESH_VV("gauge_area", FRCMD_OWNERDRAW, FrFormBar::OnDrawGaugeArea)
ON_FRESH_VI("ok", FRCMD_INIT, FrFormBar::OnInitBtnOK)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrFormBar::OnDownBtnOK)
ON_FRESH_VI("no", FRCMD_INIT, FrFormBar::OnInitBtnNO)
ON_FRESH_VV("no", FRCMD_LBUTTONUP, FrFormBar::OnDownBtnNO)
ON_FRESH_VI("msg", FRCMD_INIT, FrFormBar::OnInitEdMsg)

END_FRESH_MSGMAP()

FrFormBar::FrFormBar()
	: m_pGaugeArea(NULL),
	  m_pGaugeImg(NULL),
	  m_pFrameImg(NULL),
	  m_min(0.0f),
	  m_max(100.0f),
	  m_pos(0.0f),
	  m_gaugePivot(1.0f, 1.0f),
	  m_pEdMsg(NULL),
	  m_autoCloseTime(0.0f),
	  m_autoCloseRemain(0.0f)
{
	m_btn.first = m_btn.second = NULL;
}

FrFormBar::~FrFormBar()
{
}

void FrFormBar::Attach(const WRect& rect, const char* image)
{
	const Bitmap* bitmap = g_pFresh->GetBitmap(image);

	if (bitmap == NULL)
		return;

	sATTACHRES res;
	res.pBitmap = bitmap;
	res.rect = rect;
	m_attachList.push_back(res);
}

void FrFormBar::SetFixedWnd()
{
	m_dwStyle.Enable(FWS_FIXED);
	EnableDrag(false);
}

bool FrFormBar::Open(FRESH_PFN_RESULT pFnResult, unsigned long flag)
{
	Init();
	return FrForm::Open(pFnResult, flag);
}

void FrFormBar::SetGaugeSize(const WSize& size)
{
	WRect rect;
	rect.x = m_rect.x;
	rect.y = m_rect.y;
	rect.w = size.w;
	rect.h = size.h;
	SetRect(rect);

	ReCalcRects();
}

void FrFormBar::MoveBar(const WPoint& pos)
{
	m_barPos = pos;
}

WPoint FrFormBar::GetRelativeBarPos()
{
	return m_barPos;
}

void FrFormBar::Init()
{
	MakeDefault();
	m_orgRect = m_rect;
}

void FrFormBar::SetFrameImg(const char* image)
{
	m_pFrameImg = g_pFresh->GetBitmap(image);
}

void FrFormBar::SetGaugeImg(const char* image)
{
	m_pGaugeImg = g_pFresh->GetBitmap(image);
}

void FrFormBar::SetRange(int min, int max)
{
	m_min = (float)min;
	m_max = (float)max;
}

void FrFormBar::SetPos(float pos)
{
	m_pos = pos;
}

void FrFormBar::SetGaugeFrameSize(const WSize& size)
{
	WRect rect = GetRect();

	if (rect.w > size.w)
		m_frameSize.w = size.w;
	else
	{
		m_frameSize.w = rect.w - 2.0f;
	}

	if (rect.h > size.h)
		m_frameSize.h = size.h;
	else
	{
		m_frameSize.h = rect.h - 2.0f;
	}

	ReCalcRects();
}

void FrFormBar::SetGaugePivot(const WPoint& pivot)
{
	m_gaugePivot = pivot;
	ReCalcRects();
}

void FrFormBar::OnProc(const float deltaTime)
{
	FrForm::OnProc(deltaTime);

	if (m_autoCloseTime > 0.0f)
	{
		if (m_autoCloseRemain > 0.0f)
		{
			m_autoCloseRemain -= deltaTime;
			if (m_autoCloseRemain < 0.0f)
				m_autoCloseRemain = 0.0f;
		}

		m_pos = (m_autoCloseRemain / m_autoCloseTime) * m_max;

		if (m_autoCloseRemain == 0.0f)
		{
			if (m_btn.second->IsVisible())
				FrForm::Close(FrNO, true);
			else
				FrForm::Close(FrCANCEL, true);
		}
	}
}

void FrFormBar::OnInitEdMsg(int param)
{
	m_pEdMsg = DYNAMIC_CAST(FrEdit, param);
	if (m_pEdMsg)
		m_pEdMsg->SetVisible(false);
}

void FrFormBar::OnInitBtnOK(int param)
{
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (pBtn == NULL)
		return;

	pBtn->SetVisible(false);
	m_btn.first = pBtn;
}

void FrFormBar::OnDrawGaugeArea()
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	ReCalcRects();

	for (std::list<sATTACHRES>::iterator it = m_attachList.begin();
		it != m_attachList.end(); ++it)
	{
		WRect src(0.0f, 0.0f, (float)(*it).pBitmap->Width(),
			(float)(*it).pBitmap->Height());

		WRect dest = GetRect();
		dest.x += (*it).rect.x;
		dest.y += (*it).rect.y;
		dest.w = (*it).rect.w;
		dest.h = (*it).rect.h;
		pDevice->DrawTexture((*it).pBitmap, src, dest, 0xffffffff, false);
	}

	WRect rect = m_gaugeRect;
	float value = m_pos + m_min;

	if (m_max != m_min && value < m_max)
		rect.w = (value / (m_max - m_min)) * rect.w;

	pDevice->DrawTexture(m_pGaugeImg, rect, 0xffffffff, false);

	pDevice->DrawTexture(m_pFrameImg, m_frameRect, 0xffffffff, false);
}

void FrFormBar::OnDownBtnOK()
{
	FrForm::Close(FrOK, true);
}

void FrFormBar::OnInitBtnNO(int param)
{
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (pBtn == NULL)
		return;

	pBtn->SetVisible(false);
	m_btn.second = pBtn;
}

void FrFormBar::OnDownBtnNO()
{
	FrForm::Close(FrNO, true);
}

void FrFormBar::OnInitGaugeArea(int param)
{
	m_pGaugeArea = DYNAMIC_CAST(FrArea, param);
}

void FrFormBar::EnableButton(bool bEnable)
{
	m_btn.first->SetVisible(bEnable);
	m_btn.first->Enable(bEnable);

	m_btn.second->SetVisible(bEnable);
	m_btn.second->Enable(bEnable);
}

void FrFormBar::SetDesc(const char* desc)
{
	if (m_pEdMsg)
	{
		m_pEdMsg->SetVisible(true);
		m_pEdMsg->SetLine(1, desc, 0, 0, 0);
	}
}

void FrFormBar::MakeDefault()
{
	m_pFrameImg = g_pFresh->GetBitmap("formbar_frame");
	m_pGaugeImg = g_pFresh->GetBitmap("formbar_gauge_n");

	SetGaugeFrameSize(WSize(228.0f, 18.0f));
	SetGaugePivot(WPoint(1.0f, 1.0f));

	WRect rect = GetRect();
	m_barPos.x = (rect.w - m_frameSize.w) * 0.5f;
	m_barPos.y = (rect.h - m_frameSize.h) * 0.8f;
}

void FrFormBar::ReCalcRects()
{
	WRect rect = GetRect();

	m_frameRect.x = rect.x + m_barPos.x;
	m_frameRect.y = rect.y + m_barPos.y;
	m_frameRect.w = m_frameSize.w;
	m_frameRect.h = m_frameSize.h;

	m_gaugeRect.x = m_frameRect.x + m_gaugePivot.x;
	m_gaugeRect.y = m_frameRect.y + m_gaugePivot.y;
	m_gaugeRect.w = m_frameRect.w - (m_gaugePivot.x + m_gaugePivot.x);
	m_gaugeRect.h = m_frameRect.h - (m_gaugePivot.y + m_gaugePivot.y);
}

void FrFormBar::SetAutoCloseTimer(float time)
{
	m_autoCloseTime = m_autoCloseRemain = time;

	m_pos = m_max;
}
