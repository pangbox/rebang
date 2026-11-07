#include "minatl.h"
#include "createnickdlg.h"

#include "emoticondlg.h"
#include "logoutdlg.h"
#include "frwndinl.h"
#include "fredit.h"
#include "frbutton.h"
#include "fresh.h"
#include "actor.h"
#include "../../shared/token.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrCreateNickDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrCreateNickDlg, FrForm)

ON_FRESH_VI("editnick", FRCMD_INIT, FrCreateNickDlg::OnEditNickInit)
ON_FRESH_BI("editnick", FRCMD_ENTERKEY, FrCreateNickDlg::OnEditNickEnterKey)
ON_FRESH_VV("editnick", FRCMD_LBUTTONDOWN, FrCreateNickDlg::OnEditNickDown)
ON_FRESH_VI("dispnick", FRCMD_INIT, FrCreateNickDlg::OnDispNickInit)
ON_FRESH_VI("message", FRCMD_INIT, FrCreateNickDlg::OnMessasgeInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrCreateNickDlg::OnOKBtnUp)
ON_FRESH_VV("check", FRCMD_LBUTTONUP, FrCreateNickDlg::OnCheckBtnUp)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrCreateNickDlg::OnCancelBtnUp)
ON_FRESH_VI("emoticon", FRCMD_INIT, FrCreateNickDlg::OnEmoticonInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrCreateNickDlg::OnEmoticonUp)

END_FRESH_MSGMAP()

void NotifyErrorMsg(FrCreateNickDlg* pDlg, const char* msg)
{
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), pDlg, "notify", NULL);
	if (pForm)
	{
		pForm->SetMessage(msg, false);
		pForm->Open(NULL, 3);
	}
}

FrCreateNickDlg::FrCreateNickDlg()
	: m_pEditNick(NULL),
	  m_pDispNick(NULL),
	  m_pEmoticonBtn(NULL),
	  m_pEmoticonDlg(NULL),
	  m_bCheck(false)
{
	m_nick[0] = '\0';
}

void FrCreateNickDlg::CloseDlg(int result)
{
	if (result < 0 || result > 1)
		FrForm::OnCancel();
	else
		FrForm::OnOK();
}

void FrCreateNickDlg::OnEmoticonInit(int param)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrCreateNickDlg::OnEmoticonUp()
{
	if (m_pEmoticonBtn && m_pEmoticonDlg == NULL)
	{
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this,
			"emoticon", this);
		if (m_pEmoticonDlg)
		{
			WPoint pos(0, 0);
			const WRect& rect = m_pEmoticonBtn->GetRect();
			pos.x = rect.x - 200.0f;
			pos.y = rect.y + 24.0f;
			m_pEmoticonDlg->SetFixed(true);
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&FrCreateNickDlg::OnEmoticonResult, pos, 0);
		}
	}
}

bool FrCreateNickDlg::OnEmoticonResult(int result, FrForm* pForm)
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

void FrCreateNickDlg::OnEditNickInit(int param)
{
	m_pEditNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrCreateNickDlg::OnEditNickDown()
{
}

bool FrCreateNickDlg::OnEditNickEnterKey(int param)
{
	OnCheckBtnUp();
	return false;
}

void FrCreateNickDlg::OnDispNickInit(int param)
{
	m_pDispNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pDispNick)
		m_pDispNick->m_nFlags.Enable(0x1000);
}

void FrCreateNickDlg::OnMessasgeInit(int param)
{
	FrEdit* pMessage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	pMessage->AddText(
		"\\c0xffff0000\\c* \xc7\xd1 \xb9\xf8 \xbc\xb3\xc1\xa4\xc7\xd1 \xb4\xeb\xc8\xad\xb8\xed\xc0\xba \xba\xaf\xb0\xe6\xc0\xcc \xbe\xee\xb7\xc6\xbd\xc0\xb4\xcf\xb4\xd9. \\c0xff000000\\c",
		false, true);
}

void FrCreateNickDlg::OnOKBtnUp()
{
	if (strcmpi(m_nick, m_pEditNick->GetLine(1, false)) != 0)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		if (pForm)
		{
			pForm->SetMessage(
				"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc0\xaf\xc8\xbf\xc7\xd1\xc1\xf6 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
		return;
	}

	if (!CheckNickByClient())
		return;

	if (IsLocalContent(S4_NT_NICKNAME_CHANGE) && !GetCheckFlag())
	{
		NotifyErrorMsg(this,
			"\xb4\xd0\xb3\xd7\xc0\xd3 \xc1\xdf\xba\xb9\xc3\xbc\xc5\xa9\xb8\xa6 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4!");
		return;
	}

	WSendPacket packet(6);
	packet.EncodeStr(m_pEditNick->GetLine(1, false));
	packet.Send(TO_LOGIN);

	AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
}

void FrCreateNickDlg::OnCheckBtnUp()
{
	if (CheckNickByClient())
	{
		WSendPacket packet(7);
		packet.EncodeStr(m_pEditNick->GetLine(1, false));
		packet.Send(TO_LOGIN);

		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
}

void FrCreateNickDlg::OnCancelBtnUp()
{
	FrLogoutDlg* pDlg =
		CreateForm<FrLogoutDlg>(g_pFresh->GetManager(), this, "quit", NULL);
	pDlg->Open((FRESH_PFN_RESULT)&FrCreateNickDlg::OnQuitDlgResult, 1);
}

bool FrCreateNickDlg::OnQuitDlgResult(int result, FrForm* form)
{
	if (result < 0 || result > 1)
		FrForm::OnCancel();
	else
		FrForm::OnOK();

	return true;
}

bool FrCreateNickDlg::CheckNickByClient()
{
	if (m_pEditNick == NULL)
		return false;

	int len = strlen(m_pEditNick->GetLine(1, false));
	if (len <= 0 || len > 16)
	{
		NotifyErrorMsg(this,
			"\xb4\xeb\xc8\xad\xb8\xed \xb1\xe6\xc0\xcc\xb0\xa1 \xc0\xaf\xc8\xbf\xc7\xd1\xc1\xf6 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		return false;
	}

	char nick[22];
	strcpy(nick, m_pEditNick->GetLine(1, false));

	const char* err = Doc()->m_chatManager.FilteringNick(nick);
	if (err)
	{
		NotifyErrorMsg(this, err);
		return false;
	}

	const char* str = m_pEditNick->GetLine(1, false);
	cTokenV token;
	token.Init(str, strlen(str));

	if (token.GetTokenNum(0, " '", 2) - 1 > 0)
	{
		NotifyErrorMsg(this,
			"[']\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		return false;
	}

	if (strcmpi(str, MyId()) == 0)
	{
		NotifyErrorMsg(this,
			"\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xbe\xc6\xc0\xcc\xb5\xf0\xbf\xcd \xb4\xd9\xb8\xa3\xb0\xd4 \xbc\xb3\xc1\xa4\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.");
		return false;
	}

	return true;
}

void FrCreateNickDlg::OnProc(const float time)
{
	FrForm::OnProc(time);

	static unsigned long s_now;
	static unsigned long s_last = timeGetTime();

	s_now = timeGetTime();
	if (s_now - s_last >= 200)
	{
		s_last = s_now;

		if (strcmpi(m_pEditNick->GetLine(1, false),
				m_pDispNick->GetLine(1, false)) != 0)
			m_pDispNick->SetLine(1, m_pEditNick->GetLine(1, false), 0, false,
				0);
	}
}

void FrCreateNickDlg::ReceiveCheckCode(int code)
{
	if (!IsLocalContent(S4_NT_NICKNAME_CHANGE))
		return;

	bool bCheck;
	switch (code)
	{
	case 0:
	case 8:
		bCheck = true;
		break;

	default:
		bCheck = false;
		break;
	}

	SetCheckFlag(bCheck);
	if (!bCheck)
		NotifyErrorMsg(this,
			"\xb4\xd0\xb3\xd7\xc0\xd3 \xc1\xdf\xba\xb9\xc3\xbc\xc5\xa9\xb8\xa6 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4!");
}

void FrCreateNickDlg::SetNick(const char* nick)
{
	strcpy(m_nick, nick);

	if (m_pDispNick)
		m_pDispNick->SetLine(1, nick, 0, false, 0);

	if (m_pEditNick)
		m_pEditNick->SetLine(1, nick, 0, false, 0);
}
