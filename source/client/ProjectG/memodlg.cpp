#include "memodlg.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "fresh.h"
#include "golftask.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrMemoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMemoDlg, FrForm)

ON_FRESH_VI("memo", FRCMD_INIT, FrMemoDlg::OnMemoInit)
ON_FRESH_VV("memo", FRCMD_LBUTTONDOWN, FrMemoDlg::OnMemoLButtonDown)
ON_FRESH_BI("memo", FRCMD_ENTERKEY, FrMemoDlg::OnMemoEnterKey)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrMemoDlg::OnOkBtnUp)

END_FRESH_MSGMAP()

void FrMemoDlg::OnMemoInit(int param)
{
	m_pMemo = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pMemo->SetCharLimit(10, false);
	m_pMemo->SetKeyFocus(true);

	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
}

void FrMemoDlg::OnMemoLButtonDown()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
}

bool FrMemoDlg::OnMemoEnterKey(int param)
{
	if (!m_pMemo->GetStyle().GetFlag(FWS_DISABLED))
		OnOkBtnUp();

	return false;
}

void FrMemoDlg::OnOkBtnUp()
{
	if (strlen(m_pMemo->GetLine(1, false)) == 0)
		m_pMemo->SetLine(1, "Friend", 0, false, 0);

	OnFreshOkay();
}

const char* FrMemoDlg::GetMemo()
{
	if (m_pMemo == NULL)
		return NULL;

	const char* memo = m_pMemo->GetLine(1, false);
	Doc()->m_chatManager.FilteringHack(memo, true);

	return memo;
}

void FrMemoDlg::OnProc(const float deltaTime)
{
	if (m_bFocusSet == false)
	{
		if (m_focusTime > 0.3f)
		{
			WndManager()->GetDesktop()->ResetKeyFocus();
			m_pMemo->SetKeyFocus(true);
			m_bFocusSet = true;
		}

		m_focusTime += deltaTime;
	}
}
