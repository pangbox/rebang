#include "minatl.h"
#include "addfrienddlg.h"
#include "emoticondlg.h"
#include "fredit.h"
#include "frbutton.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "fresh.h"
#include "golftask.h"
#include "actor.h"
#include "frwnd.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrAddFriendDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrAddFriendDlg, FrForm)

ON_FRESH_VI("id", FRCMD_INIT, FrAddFriendDlg::OnEditNiclInit)
ON_FRESH_BI("id", FRCMD_ENTERKEY, FrAddFriendDlg::OnEditNickEnterKey)
ON_FRESH_VV("id", FRCMD_LBUTTONDOWN, FrAddFriendDlg::OnEditNickDown)
ON_FRESH_VI("nick", FRCMD_INIT, FrAddFriendDlg::OnDispNickInit)
ON_FRESH_VV("check", FRCMD_LBUTTONUP, FrAddFriendDlg::OnCheckBtnUp)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrAddFriendDlg::OnOKBtnUp)
ON_FRESH_VI("emoticon", FRCMD_INIT, FrAddFriendDlg::OnEmoticonInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrAddFriendDlg::OnEmoticonUp)

END_FRESH_MSGMAP()

void FrAddFriendDlg::OnEmoticonInit(int param)
{
	m_pEmoticon = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrAddFriendDlg::OnEmoticonUp()
{
	if (m_pEmoticon && m_pEmoticonDlg == NULL)
	{
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this,
			"emoticon", this);
		if (m_pEmoticonDlg)
		{
			WPoint pos(0, 0);
			const WRect& rect = m_pEmoticon->GetRect();
			pos.x = rect.x - 200.0f;
			pos.y = rect.y + 24.0f;
			m_pEmoticonDlg->SetFixed(true);
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&FrAddFriendDlg::OnEmoticonResult, pos, 0);
		}
	}
}

bool FrAddFriendDlg::OnEmoticonResult(int result, FrForm* pForm)
{
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg)
		{
			const char* icon = pDlg->GetSelectedIcon();
			if (icon)
			{
				FrEdit* pEdit = DYNAMIC_CAST(FrEdit, FindChildByName("id"));
				if (m_pEditNick)
				{
					const char* front = m_pEditNick->GetEditText_Front();
					const char* comp = m_pEditNick->GetEditText_Comp();
					const char* end = m_pEditNick->GetEditText_End();
					m_pEditNick->SetLine(1,
						MakeStr("%s%s%s%s", front, comp, icon, end), 0, false,
						0);
					m_pEditNick->SetKeyFocus(true);
				}
			}
		}
	}
	else if (m_pEditNick)
	{
		m_pEditNick->SetKeyFocus(true);
	}

	m_pEmoticonDlg = NULL;
	return true;
}

void FrAddFriendDlg::SetNick(const char* nick)
{
	if (m_pDispNick)
		m_pDispNick->SetLine(1, nick, 0, false, 0);

	if (m_pEditNick)
		m_pEditNick->SetLine(1, nick, 0, false, 0);
}

void FrAddFriendDlg::SetTargetNick(const char* nick)
{
	if (m_pEditNick)
		m_pEditNick->SetLine(1, nick, 0, false, 0);
}

void FrAddFriendDlg::SetFindUID(unsigned long uid)
{
	m_findUID = uid;
}

void FrAddFriendDlg::CheckRequestNickname(const char* nick)
{
	if (nick == NULL)
		return;

	if (strlen(nick) < 1)
		return;

	if (m_pEditNick)
		m_pEditNick->SetLine(1, nick, 0, false, 0);
}

void FrAddFriendDlg::OnEditNiclInit(int param)
{
	m_pEditNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pEditNick->m_nFlags.Enable(0x1000);

	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		g_pFresh->GetManager()->SetExclusiveKey(true);
}

void FrAddFriendDlg::OnEditNickDown()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		g_pFresh->GetManager()->SetExclusiveKey(true);
}

bool FrAddFriendDlg::OnEditNickEnterKey(int param)
{
	OnCheckBtnUp();
	return false;
}

void FrAddFriendDlg::OnDispNickInit(int param)
{
	m_pDispNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pDispNick)
	{
		m_pDispNick->m_nFlags.Enable(0x1000);
		m_pDispNick->SetLine(1, "<\xc8\xae\xc0\xce \xbe\xc8\xb5\xca>", 0, false,
			0);
	}
}

void FrAddFriendDlg::OnCheckBtnUp()
{
	if (m_pDispNick)
		m_pDispNick->ClearLine();

	if (m_pEditNick)
	{
		const char* nick = m_pEditNick->GetLine(1, false);

		if (strcmp(nick, MyNick()) == 0)
		{
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
				(int)"\xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb4\xd0\xb3\xd7\xc0\xd3\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			return;
		}

		int len = strlen(nick);
		if (nick && len > 0 && len < 22)
		{
			if (WNetworkSystem::Instance()->IsConnected(
					WNetworkSystem::NET_MSN))
			{
				WSendPacket packet(0x17);
				packet.EncodeStr(std::string(nick));
				packet.Send(TO_MSN);
			}
			else
			{
				WSendPacket packet((enumClientPacket)7);
				packet.Encode1(0);
				packet.EncodeStr(std::string(nick));
				packet.Send(TO_GAME);

				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			}
		}
		else
		{
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 35,
				(int)"\xc4\xa3\xb1\xb8\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0);
		}
	}
}

void FrAddFriendDlg::OnOKBtnUp()
{
	if (m_pEditNick)
	{
		if (strlen(m_pEditNick->GetLine(1, false)) == 0)
		{
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 35,
				(int)"\xc4\xa3\xb1\xb8\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0);
			return;
		}

		if (strcmp(m_pEditNick->GetLine(1, false),
				m_pDispNick->GetLine(1, false)) != 0)
		{
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 35,
				(int)"\xba\xb8\xb3\xbb\xb1\xe2 \xc0\xfc\xbf\xa1 \xb9\xde\xb4\xc2 \xc0\xcc\xb0\xa1 \xc8\xae\xbd\xc7\xc8\xf7 \xb8\xc2\xb4\xc2\xc1\xf6\n\xb2\xc0 \\c0xffff0000\\c'\xc8\xae\xc0\xce'\\c0xff000000\\c \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0);
			return;
		}

		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN))
		{
			WSendPacket packet(0x18);
			packet.Encode4(m_findUID);
			packet.EncodeStr(std::string(m_pEditNick->GetLine(1, false)));
			packet.Send(TO_MSN);
		}
		else
		{
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 35,
				(int)"\xb8\xde\xbd\xc5\xc0\xfa \xbc\xad\xb9\xf6\xbf\xcd \xbf\xac\xb0\xe1\xc0\xcc \xb5\xc7\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xc0\xbd\xbf\xa1 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xcf\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
		}
	}

	OnFreshOkay();
}

bool FrAddFriendDlg::OnInit()
{
	SetMessage(
		"\xc3\xa3\xc0\xbb \xc4\xa3\xb1\xb8 \xb4\xeb\xc8\xad\xb8\xed\xbf\xa1 \xc4\xa3\xb1\xb8\xb0\xa1 \xb5\xc7\xb0\xed \xbd\xcd\xc0\xba \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xcf\xb8\xe9 \xc1\xa2\xbc\xd3\xc1\xdf\xc0\xcc\xc1\xf6 \xbe\xca\xc0\xba \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4\xb5\xb5 \xc4\xa3\xb1\xb8 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
		false);
	WndManager();
	return FrForm::OnInit();
}

void FrAddFriendDlg::OnProc(const float delta)
{
	if (m_bFocused == false)
	{
		if (m_elapsed > 0.3f)
		{
			WndManager()->GetDesktop()->ResetKeyFocus();
			m_pEditNick->SetKeyFocus(true);
			m_bFocused = true;
		}

		m_elapsed += delta;
	}
}

void FrAddFriendDlg::OnCancel()
{
	if (m_pEmoticonDlg)
		m_pEmoticonDlg->Close(FrCANCEL, true);

	FrForm::OnCancel();
}
