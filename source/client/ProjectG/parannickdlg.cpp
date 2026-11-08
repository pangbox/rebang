#include "parannickdlg.h"
#include "fredit.h"
#include "fresh.h"
#include "actor.h"
#include "../../shared/token.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrParanNickDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrParanNickDlg, FrForm)

ON_FRESH_VI("nick", FRCMD_INIT, FrParanNickDlg::OnNickInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrParanNickDlg::OnOkBtnUp)

END_FRESH_MSGMAP()

FrParanNickDlg::FrParanNickDlg()
{
}

void FrParanNickDlg::CloseDlg()
{
	FrForm::OnOK();
}

void FrParanNickDlg::OnNickInit(int param)
{
	m_pNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrParanNickDlg::EnableControls(bool bEnable)
{
	m_pNick->Enable(bEnable);
}

void FrParanNickDlg::OnOkBtnUp()
{
	char nick[22];
	const char* err;

	strcpy(nick, m_pNick->GetLine(1, false));
	err = Doc()->m_chatManager.FilteringNick(nick);

	if (err)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);

		if (pForm)
		{
			pForm->SetMessage(err, false);
			pForm->Open(NULL, 3);
		}

		return;
	}

	const char* str = m_pNick->GetLine(1, false);
	cTokenV token;
	token.Init(str, strlen(str));

	int num = token.GetTokenNum(0, " '", 2) - 1;

	if (num > 0)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);

		if (pForm)
		{
			pForm->SetMessage(
				"[']\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}

		return;
	}

	WSendPacket packet((enumClientPacket)142);

	packet.EncodeStr(m_pNick->GetLine(1, false));
	packet.Send(TO_GAME);

	AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);

	m_pNick->Enable(false);
}
