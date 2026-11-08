#include "avatartipdlg.h"
#include "frviewer.h"

IMPLEMENT_OBJECT(FrAvatarTipDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrAvatarTipDlg, FrForm)

ON_FRESH_VI("tip", FRCMD_INIT, FrAvatarTipDlg::OnTipInit)
ON_FRESH_VV("tip", FRCMD_LBUTTONUP, FrAvatarTipDlg::OnNextBtnUp)
ON_FRESH_VI("prev", FRCMD_INIT, FrAvatarTipDlg::OnPrevInit)
ON_FRESH_VV("prev", FRCMD_LBUTTONUP, FrAvatarTipDlg::OnPrevBtnUp)
ON_FRESH_VI("next", FRCMD_INIT, FrAvatarTipDlg::OnNextInit)
ON_FRESH_VV("next", FRCMD_LBUTTONUP, FrAvatarTipDlg::OnNextBtnUp)
ON_FRESH_VI("cancel", FRCMD_INIT, FrAvatarTipDlg::OnCloseInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrAvatarTipDlg::OnCloseBtnUp)

END_FRESH_MSGMAP()

FrAvatarTipDlg::FrAvatarTipDlg()
{
	m_tipIndex = 0;
}

FrAvatarTipDlg::~FrAvatarTipDlg()
{
}

void FrAvatarTipDlg::OnTipInit(int param)
{
	m_pTip = DYNAMIC_CAST(FrViewer, param);
}

void FrAvatarTipDlg::OnPrevInit(int param)
{
	m_pPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pPrev)
		m_pPrev->SetPushDelay(0);
}

void FrAvatarTipDlg::OnPrevBtnUp()
{
	m_tipIndex--;
	if (m_tipIndex < 0)
		m_tipIndex = 5;

	if (m_pTip)
		m_pTip->Open(MakeStr("tip%d.tga", m_tipIndex + 1));
}

void FrAvatarTipDlg::OnNextInit(int param)
{
	m_pNext = DYNAMIC_CAST(FrButton, param);
	if (m_pNext)
		m_pNext->SetPushDelay(0);
}

void FrAvatarTipDlg::OnNextBtnUp()
{
	m_tipIndex++;
	if (m_tipIndex > 5)
		m_tipIndex = 0;

	if (m_pTip)
		m_pTip->Open(MakeStr("tip%d.tga", m_tipIndex + 1));
}

void FrAvatarTipDlg::OnCloseInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);
}

void FrAvatarTipDlg::OnCloseBtnUp()
{
	Close(true);
}
