#include "helpdlg.h"
#include "frviewer.h"
#include "frarea.h"
#include "projectg.h"

IMPLEMENT_OBJECT(FrHelpDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrHelpDlg, FrForm)

ON_FRESH_VI("title", FRCMD_INIT, FrHelpDlg::OnTitleInit)
ON_FRESH_VI("view", FRCMD_INIT, FrHelpDlg::OnViewInit)
ON_FRESH_VV("view", FRCMD_LBUTTONUP, FrHelpDlg::OnViewBtnUp)

END_FRESH_MSGMAP()

bool FrHelpDlg::OnInit()
{
	if (m_pViewer == NULL)
		return FrForm::OnInit();

	if (m_type == 0)
	{
		m_pViewer->Open("cha_up_help.jpg");

		if (m_pTitle)
			m_pTitle->SetBgImg("cha_up_help");
	}
	else if (m_type == 2)
	{
		m_pViewer->Open("quest_help.jpg");

		if (m_pTitle)
			m_pTitle->SetBgImg("quest_help");
	}
	else
	{
		m_pViewer->Open("club_up_help.jpg");

		if (m_pTitle)
			m_pTitle->SetBgImg("club_up");
	}

	WRect rect = m_rect;
	WRect cancelRect;
	WRect viewRect = m_pViewer->GetRect();
	viewRect.w = m_pViewer->GetSrcWidth() + 5.0f;
	m_pViewer->SetRect(viewRect);

	if (m_pViewer->GetSrcWidth() + 30.0f > rect.w)
	{
		FrWnd* pCancel = FindChildByName("cancel");

		if (pCancel)
		{
			cancelRect = pCancel->GetRect();
			cancelRect.x =
				m_pViewer->GetSrcWidth() + 30.0f - (rect.w - cancelRect.x);
			pCancel->SetRect(cancelRect);
		}

		rect.w = m_pViewer->GetSrcWidth() + 30.0f;
		rect.x = floor((g_view->GetWidth() - rect.w) * 0.5f);
	}

	rect.h = m_pViewer->GetSrcHeight() + 80.0f;
	rect.y = floor((g_view->GetHeight() - rect.h) * 0.5f);

	SetRect(rect);

	return FrForm::OnInit();
}

void FrHelpDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrArea, param);
}

void FrHelpDlg::OnViewInit(int param)
{
	m_pViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_pViewer)

		m_pViewer->GetScrollBar()->SetVisible(false);
}

void FrHelpDlg::OnViewBtnUp()
{
	Close(true);
}
