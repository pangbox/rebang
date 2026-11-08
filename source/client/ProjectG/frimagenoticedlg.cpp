#include "frimagenoticedlg.h"
#include "frbutton.h"
#include "frarea.h"

extern WView* g_view;

IMPLEMENT_OBJECT(FrImageNoticeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrImageNoticeDlg, FrForm)

ON_FRESH_VI("cancel", FRCMD_INIT, FrImageNoticeDlg::OnInitCloseButton)
ON_FRESH_VI("ok", FRCMD_INIT, FrImageNoticeDlg::OnInitOkButton)
ON_FRESH_VV("ok", FRCMD_LBUTTONDOWN, FrImageNoticeDlg::OnLBDownOkButton)
ON_FRESH_VI("lefttop", FRCMD_INIT, FrImageNoticeDlg::OnInitLeftTop)
ON_FRESH_VI("centertop", FRCMD_INIT, FrImageNoticeDlg::OnInitCenterTop)
ON_FRESH_VI("righttop", FRCMD_INIT, FrImageNoticeDlg::OnInitRightTop)
ON_FRESH_VI("leftmiddle", FRCMD_INIT, FrImageNoticeDlg::OnInitLeftMiddle)
ON_FRESH_VI("centermiddle", FRCMD_INIT, FrImageNoticeDlg::OnInitCenterMiddle)
ON_FRESH_VI("rightmiddle", FRCMD_INIT, FrImageNoticeDlg::OnInitRightMiddle)
ON_FRESH_VI("leftbottom", FRCMD_INIT, FrImageNoticeDlg::OnInitLeftBottom)
ON_FRESH_VI("centerbottom", FRCMD_INIT, FrImageNoticeDlg::OnInitCenterBottom)
ON_FRESH_VI("rightbottom", FRCMD_INIT, FrImageNoticeDlg::OnInitRightBottom)

END_FRESH_MSGMAP()

FrImageNoticeDlg::FrImageNoticeDlg()
{
	m_pCloseButton = NULL;
	m_pOkButton = NULL;
}

FrImageNoticeDlg::~FrImageNoticeDlg()
{
}

void FrImageNoticeDlg::OnInitCloseButton(int param)
{
	m_pCloseButton = DYNAMIC_CAST(FrButton, param);
}

void FrImageNoticeDlg::OnInitOkButton(int param)
{
	m_pOkButton = DYNAMIC_CAST(FrButton, param);
	if (m_pOkButton)
	{
		m_pOkButton->SetVisible(false);
		m_pOkButton->Enable(false);
	}
}

void FrImageNoticeDlg::OnLBDownOkButton()
{
	Close(FrOK, true);
}

void FrImageNoticeDlg::OnInitLeftTop(int param)
{
	m_pBackGround[0][0] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitCenterTop(int param)
{
	m_pBackGround[0][1] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitRightTop(int param)
{
	m_pBackGround[0][2] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitLeftMiddle(int param)
{
	m_pBackGround[1][0] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitCenterMiddle(int param)
{
	m_pBackGround[1][1] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitRightMiddle(int param)
{
	m_pBackGround[1][2] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitLeftBottom(int param)
{
	m_pBackGround[2][0] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitCenterBottom(int param)
{
	m_pBackGround[2][1] = DYNAMIC_CAST(FrArea, param);
}

void FrImageNoticeDlg::OnInitRightBottom(int param)
{
	m_pBackGround[2][2] = DYNAMIC_CAST(FrArea, param);
}

bool FrImageNoticeDlg::SetOkButton(WRect& rect, bool bRelative)
{
	bool bRet = false;
	if (m_pOkButton)
	{
		if (bRelative)
		{
			WRect btnRect = m_pOkButton->GetRect();
			rect = WRect(btnRect.x + rect.x, btnRect.y + rect.y,
				btnRect.w + rect.w, btnRect.h + rect.h);
		}
		m_pOkButton->SetRect(rect);

		m_pOkButton->Enable(true);
		m_pOkButton->SetVisible(true);

		bRet = true;
	}
	return bRet;
}

bool FrImageNoticeDlg::SetCloseButton(WRect& rect, bool bRelative)
{
	bool bRet = false;
	if (m_pCloseButton)
	{
		if (bRelative)
		{
			WRect btnRect = m_pCloseButton->GetRect();
			rect = WRect(btnRect.x + rect.x, btnRect.y + rect.y,
				btnRect.w + rect.w, btnRect.h + rect.h);
		}
		m_pCloseButton->SetRect(rect);
		bRet = true;
	}
	return bRet;
}

void FrImageNoticeDlg::SetDlgSize(WRect& rect)
{
	if (rect.x <= -1.0f)
	{
		rect.x = (float)((int)g_view->GetWidth() / 2 - (int)rect.w / 2);
	}

	if (rect.y <= -1.0f)
	{
		rect.y = (float)((int)g_view->GetHeight() / 2 - (int)rect.h / 2);
	}

	SetRect(rect);

	if (m_pCloseButton)
	{
		WRect closeRect = m_pCloseButton->GetRect();

		closeRect.x = m_rect.x + rect.w - 53.0f;

		m_pCloseButton->SetRect(closeRect);
	}
}

bool FrImageNoticeDlg::SetBackGroundImage(int x, int y, WRect& rect,
	const char* image, bool bRelative)
{
	bool bRet = false;
	if (m_pBackGround[y][x])
	{
		if (bRelative)
		{
			WRect dlgRect = GetRect();
			rect = WRect(dlgRect.x + rect.x, dlgRect.y + rect.y,
				dlgRect.w + rect.w, dlgRect.h + rect.h);
			rect.y += 23.0f;
			rect.h += 23.0f;
		}
		m_pBackGround[y][x]->SetRect(rect);
		m_pBackGround[y][x]->SetBgImg(image);
		bRet = true;
	}
	return bRet;
}
