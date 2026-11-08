#include "loginrsdlg.h"
#include "frbutton.h"
#include "rankinginfo.h"
#include "user_info.h"
#include "mathconsts.h"

IMPLEMENT_OBJECT(FrLoginRsDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrLoginRsDlg, FrForm)

ON_FRESH_VI("cancel", FRCMD_INIT, FrLoginRsDlg::OnCancelInit)

END_FRESH_MSGMAP()

eLoginToRs FrLoginRsDlg::m_connState = LOGINRS_NONE;

FrLoginRsDlg::FrLoginRsDlg()
{
	m_pCancel = NULL;
	m_elapsedTime = 0.0f;
	SetState(LOGINRS_NONE);
}

void FrLoginRsDlg::SetState(eLoginToRs state)
{
	m_connState = state;
}

eLoginToRs FrLoginRsDlg::GetState()
{
	return m_connState;
}

void FrLoginRsDlg::OnProc(const float deltaTime)
{
	static float s_spinTime = 0.0f;
	static float s_waitTime = 1.8f;
	WNetworkSystem* pNet;
	m_elapsedTime += deltaTime;
	if (m_elapsedTime > 15.0f)
	{
		if (m_pCancel)
			m_pCancel->Enable(true);

		SetMessage(
			"\xc1\xf6\xb1\xdd\xc0\xba \xb7\xa9\xc5\xb7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
	}
	else
	{
		char spin[3] = { '-', '/', '|' };

		SetMessage(
			MakeStr(
				"\xb7\xa9\xc5\xb7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. %c",
				spin[(int)s_spinTime % 3]),
			false);

		s_spinTime += deltaTime * 10.0f;
	}

	if (m_connState < LOGINRS_CONNECTED && (pNet = WNetworkSystem::Instance()))
	{
		if (m_connState == LOGINRS_CONNECTING &&
			pNet->IsConnectionComplete(WNetworkSystem::NET_RANK))
		{
			if (!pNet->IsConnected(WNetworkSystem::NET_RANK))
			{
				m_elapsedTime = 16.0f;
				SetMessage(
					"\xc1\xf6\xb1\xdd\xc0\xba \xb7\xa9\xc5\xb7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				m_connState = LOGINRS_DONE;
			}
			else
			{
				s_waitTime = 1.8f;
				m_connState = LOGINRS_CONNECTED;
			}
		}
	}
	else if (m_connState == LOGINRS_CONNECTED)
	{
		s_waitTime -= deltaTime;

		if (s_waitTime < 0.0f)
		{
			m_connState = LOGINRS_DONE;

			if (CUserInfo::Instance())
				CUserInfo::Instance()->Close();

			Close(true);
			RANKING()->Open();
		}
	}
}

void FrLoginRsDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);
	if (m_pCancel)
		m_pCancel->Enable(false);
}
