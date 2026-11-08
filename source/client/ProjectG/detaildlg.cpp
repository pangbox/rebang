#include "detaildlg.h"
#include "frviewer.h"
#include "frbutton.h"
#include "frarea.h"

IMPLEMENT_OBJECT(NtDetailDlg, FrForm)

BEGIN_FRESH_MSGMAP(NtDetailDlg, FrForm)

ON_FRESH_VI("close", FRCMD_INIT, NtDetailDlg::OnCloseBtnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, NtDetailDlg::OnCloseBtnUp)
ON_FRESH_VI("caption", FRCMD_INIT, NtDetailDlg::OnCaptionInit)
ON_FRESH_VI("view", FRCMD_INIT, NtDetailDlg::OnViewInit)

END_FRESH_MSGMAP()

NtDetailDlg::NtDetailDlg()
{
}

NtDetailDlg::~NtDetailDlg()
{
}

void NtDetailDlg::OnCloseBtnInit(int param)
{
	FrButton* pClose = DYNAMIC_CAST(FrButton, param);
}

void NtDetailDlg::OnCloseBtnUp()
{
	OnFreshOkay();
}

void NtDetailDlg::OnCaptionInit(int param)
{
	FrArea* pCaption = DYNAMIC_CAST(FrArea, param);
	if (pCaption)
	{
	}
}

void NtDetailDlg::OnViewInit(int param)
{
	m_pView = DYNAMIC_CAST(FrViewer, param);

	if (!m_pView)
		return;

	m_pView->Open("notice_popup08.jpg");
	m_pView->SetKeyFocus(true);
}
