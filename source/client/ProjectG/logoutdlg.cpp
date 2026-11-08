#include "logoutdlg.h"
#include "frbutton.h"
#include "fresh.h"
#include "lobbytask.h"
#include "messengerdlg.h"
#include "logininfo.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;
extern bool g_bQuit;

IMPLEMENT_OBJECT(FrLogoutDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrLogoutDlg, FrForm)

ON_FRESH_VI("server", FRCMD_INIT, FrLogoutDlg::OnServerBtnInit)
ON_FRESH_VV("server", FRCMD_LBUTTONUP, FrLogoutDlg::OnServerBtnUp)
ON_FRESH_VI("logout", FRCMD_INIT, FrLogoutDlg::OnLogoutBtnInit)
ON_FRESH_VV("logout", FRCMD_LBUTTONUP, FrLogoutDlg::OnLogoutBtnUp)
ON_FRESH_VI("exit", FRCMD_INIT, FrLogoutDlg::OnExitBtnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, FrLogoutDlg::OnExitBtnUp)

END_FRESH_MSGMAP()

FrLogoutDlg::FrLogoutDlg()
	: m_pServerBtn(NULL), m_pLogoutBtn(NULL), m_pExitBtn(NULL), m_pDescBtn(NULL)
{
}

void FrLogoutDlg::DisableServerSelect()
{
	m_pServerBtn->Enable(false);
}

void FrLogoutDlg::OnProc(const float dt)
{
	FrForm::OnProc(dt);

	if (m_pServerBtn && m_pServerBtn->GetStatus() == FrButton::OVER)
	{
		if (m_pDescBtn != m_pServerBtn)
		{
			m_pDescBtn = m_pServerBtn;
			SetDesc(
				"\xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xad\xb9\xf6\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xd5\xb4\xcf\xb4\xd9.");
		}
	}
	if (m_pLogoutBtn && m_pLogoutBtn->GetStatus() == FrButton::OVER)
	{
		if (m_pDescBtn != m_pLogoutBtn)
		{
			m_pDescBtn = m_pLogoutBtn;
			SetDesc(
				"\xb7\xce\xb1\xd7\xbe\xc6\xbf\xf4 \xc7\xd5\xb4\xcf\xb4\xd9.");
		}
	}
	else if (m_pExitBtn && m_pExitBtn->GetStatus() == FrButton::OVER)
	{
		if (m_pDescBtn != m_pExitBtn)
		{
			m_pDescBtn = m_pExitBtn;
			SetDesc(
				"\xc6\xce\xbe\xdf \xc7\xc1\xb7\xce\xb1\xd7\xb7\xa5\xc0\xbb \xb3\xa1\xb3\xbb\xb0\xed \xc0\xa9\xb5\xb5\xbf\xec\xb7\xce \xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9.");
		}
	}
}

void FrLogoutDlg::OnServerBtnInit(int param)
{
	m_pServerBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pServerBtn == NULL)
		return;

	if (g_pFresh->IsCurrentLayout("SERVERLIST") || Doc()->m_offlinePlay ||
		g_pFresh->IsCurrentLayout("CREATE") ||
		g_pFresh->IsCurrentLayout("AGREEMENT") ||
		g_pFresh->IsCurrentLayout("BLANK"))
	{
		m_pServerBtn->Enable(false);
	}

	if (CLoginInfo::Instance()->IsWebLogin())
	{
		WPoint pos = (WPoint&)m_pServerBtn->GetRect();

		pos.x += 38.0f;

		m_pServerBtn->MoveWindow(pos);
	}
}

void FrLogoutDlg::OnServerBtnUp()
{
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 234, 0, 0, 0, 0, 0));

	Close(FrOK, true);
}

void FrLogoutDlg::OnLogoutBtnInit(int param)
{
	m_pLogoutBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pLogoutBtn == NULL)
		return;

	if (Doc()->m_offlinePlay)
		m_pLogoutBtn->Enable(false);

	if (CLoginInfo::Instance()->IsWebLogin())
		m_pLogoutBtn->SetVisible(false);
}

void FrLogoutDlg::OnLogoutBtnUp()
{
	if (g_pFresh->IsCurrentLayout("REALMYROOM"))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 223, 0, 0, 0, 0, 0);

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_GAME);

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_LOGIN))
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_LOGIN);

	WSendPacket send((enumClientPacket)22);

	send.Send(TO_MSN);

	CMessengerInfo::Instance()->Close(true);

	WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_MSN);

	if (Doc()->m_bTutorialResume == true)

		Doc()->m_bTutorialResume = false;

	if (!IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))

		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);

	CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"LOGIN", 0, 0, 0);
}

void FrLogoutDlg::OnExitBtnInit(int param)
{
	m_pExitBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (IsLocalContent(S3_PARAN_CHANNELING))
		if (CLoginInfo::Instance()->IsWebLogin())
		{
			WPoint pos = (WPoint&)m_pExitBtn->GetRect();

			pos.x -= 38.0f;

			m_pExitBtn->MoveWindow(pos);
		}
}

void FrLogoutDlg::OnExitBtnUp()
{
	g_bQuit = true;
}
