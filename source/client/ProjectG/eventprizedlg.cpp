#include "eventprizedlg.h"
#include "frbutton.h"
#include "frviewer.h"

IMPLEMENT_OBJECT(FrEventPrizeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrEventPrizeDlg, FrForm)

ON_FRESH_VI("cancel", FRCMD_INIT, FrEventPrizeDlg::OnCancelInit)
ON_FRESH_VI("view", FRCMD_INIT, FrEventPrizeDlg::OnViewInit)
ON_FRESH_VV("view", FRCMD_LBUTTONUP, FrEventPrizeDlg::OnViewBtnUp)

END_FRESH_MSGMAP()

bool FrEventPrizeDlg::OnInit()
{
	if (m_pView == NULL)
		return FrForm::OnInit();

	m_pView->Open(MakeStr("event%02d.jpg", m_event));

	int width = m_pView->GetSrcWidth();
	if (width > 600)
		width = 600;

	width -= (int)m_pView->GetViewWidth();

	int height = m_pView->GetSrcHeight();
	if (height > 400)
		height = 400;

	height -= (int)m_pView->GetViewHeight();

	WRect rect = m_pView->GetRect();
	rect.w += width;
	rect.h += height;
	m_pView->SetRect(rect);

	if (m_pCancel)
	{
		rect = m_pCancel->GetRect();
		rect.x += width;
		m_pCancel->MoveWindow(WPoint(rect.x, rect.y));
	}

	rect = GetRect();
	rect.x -= width >> 1;
	rect.y -= height >> 1;
	rect.w += width;
	rect.h += height;
	SetRect(rect);

	FrWnd* pWnd = FindChildByName("prev");
	if (pWnd)
		pWnd->SetVisible(false);

	pWnd = FindChildByName("next");
	if (pWnd)
		pWnd->SetVisible(false);

	return FrForm::OnInit();
}

void FrEventPrizeDlg::OnViewInit(int param)
{
	m_pView = DYNAMIC_CAST(FrViewer, param);
}

void FrEventPrizeDlg::OnViewBtnUp()
{
	Close(true);
}

void FrEventPrizeDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);
}
