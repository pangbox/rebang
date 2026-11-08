#include "passworddlg.h"
#include "fredit.h"

IMPLEMENT_OBJECT(FrPasswordDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrPasswordDlg, FrForm)

ON_FRESH_VI("password", FRCMD_INIT, FrPasswordDlg::OnPasswordInit)
ON_FRESH_BI("password", FRCMD_ENTERKEY, FrPasswordDlg::OnPasswordEnterKey)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrPasswordDlg::OnOkBtnUp)

END_FRESH_MSGMAP()

FrPasswordDlg::FrPasswordDlg()
	: m_pPassword(NULL)
{
}

void FrPasswordDlg::InitContent()
{
	SetMessage(
		"\xc5\xb8\xc0\xce\xc0\xc7 \xbe\xc7\xbf\xeb\xc0\xbb \xb9\xe6\xc1\xf6\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xcf\xbf\xa9 \xc1\xd6\xb9\xce\xb9\xf8\xc8\xa3 \xb5\xda\xc0\xc7 7\xc0\xda\xb8\xae\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
		false);

	m_pPassword->SetCharLimit(7, false);
	m_pPassword->IsEnabled();
}

void FrPasswordDlg::OnPasswordInit(int param)
{
	m_pPassword = DYNAMIC_CAST(FrEdit, param);
	m_pPassword->SetKeyFocus(true);
}

bool FrPasswordDlg::OnPasswordEnterKey(int param)
{
	if (m_pPassword && !m_pPassword->GetStyle().GetFlag(FWS_DISABLED))
		OnFreshOkay();

	return false;
}

void FrPasswordDlg::OnOkBtnUp()
{
	OnFreshOkay();
}
