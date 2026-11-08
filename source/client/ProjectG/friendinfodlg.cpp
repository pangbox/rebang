#include "friendinfodlg.h"
#include "fredit.h"
#include "frbutton.h"

IMPLEMENT_OBJECT(FrFriendInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFriendInfoDlg, FrForm)

ON_FRESH_VI("id", FRCMD_INIT, FrFriendInfoDlg::OnIDInit)
ON_FRESH_VI("nick", FRCMD_INIT, FrFriendInfoDlg::OnNickInit)
ON_FRESH_VI("sex", FRCMD_INIT, FrFriendInfoDlg::OnSexInit)
ON_FRESH_VI("server", FRCMD_INIT, FrFriendInfoDlg::OnServerInit)
ON_FRESH_VI("channel", FRCMD_INIT, FrFriendInfoDlg::OnChannelInit)
ON_FRESH_VV("whisper", FRCMD_LBUTTONUP, FrFriendInfoDlg::OnWhisperBtnUp)
ON_FRESH_VI("ignore", FRCMD_INIT, FrFriendInfoDlg::OnIgnoreInit)
ON_FRESH_VV("ignore", FRCMD_LBUTTONUP, FrFriendInfoDlg::OnIgnoreBtnUp)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrFriendInfoDlg::OnCloseBtnUp)

END_FRESH_MSGMAP()

void FrFriendInfoDlg::OnIDInit(int param)
{
	m_pID = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrFriendInfoDlg::OnNickInit(int param)
{
	m_pNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrFriendInfoDlg::OnSexInit(int param)
{
	m_pSex = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrFriendInfoDlg::OnServerInit(int param)
{
	m_pServer = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrFriendInfoDlg::OnChannelInit(int param)
{
	m_pChannel = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrFriendInfoDlg::SetInfo(sFriend* pFriend)
{
}

void FrFriendInfoDlg::OnWhisperBtnUp()
{
	AfxGetTask()->GetActor("Lobby")
		<< MsgObject(NULL, 55, (int)m_pNick->GetLine(1, false), 0, 0, 0, 0);

	Close(true);
}

void FrFriendInfoDlg::OnIgnoreInit(int param)
{
	m_pIgnore = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrFriendInfoDlg::OnIgnoreBtnUp()
{
	std::list<std::string>::iterator it = std::find(Doc()->m_ignoreList.begin(),
		Doc()->m_ignoreList.end(), m_pNick->GetLine(1, false));
	if (it != Doc()->m_ignoreList.end())
	{
		Doc()->m_ignoreList.erase(it);
		m_pIgnore->SetButtonImg("i58", FrButton::NORMAL);
	}
	else
	{
		Doc()->m_ignoreList.push_back(m_pNick->GetLine(1, false));
		m_pIgnore->SetButtonImg("i59", FrButton::NORMAL);
	}
}

void FrFriendInfoDlg::OnCloseBtnUp()
{
	Close(true);
}

bool FrFriendInfoDlg::OnInit()
{
	FrWnd* pStatic = FindChildByName("s_id");
	if (pStatic)
		pStatic->SetVisible(false);

	if (m_pID)
		m_pID->SetVisible(false);

	FrWnd* wnds[8];
	wnds[0] = FindChildByName("s_nick");
	wnds[1] = FindChildByName("s_sex");
	wnds[2] = FindChildByName("s_server");
	wnds[3] = FindChildByName("s_channel");
	wnds[4] = m_pNick;
	wnds[5] = m_pSex;
	wnds[6] = m_pServer;
	wnds[7] = m_pChannel;

	for (int i = 0; i < 8; i++)
	{
		if (wnds[i])
		{
			WRect rect;
			rect = wnds[i]->GetRect();
			rect.y -= 14.0f;
			wnds[i]->SetRect(rect);
		}
	}

	return true;
}
