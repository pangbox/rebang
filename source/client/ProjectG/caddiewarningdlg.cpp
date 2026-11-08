#include "caddiewarningdlg.h"
#include "frarea.h"
#include "fredit.h"
#include "frbutton.h"
#include "frstatic.h"

IMPLEMENT_OBJECT(FrCaddieWarningDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrCaddieWarningDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrCaddieWarningDlg::OnPortraitInit)
ON_FRESH_VI("vacation", FRCMD_INIT, FrCaddieWarningDlg::OnVacationInit)
ON_FRESH_VI("limit", FRCMD_INIT, FrCaddieWarningDlg::OnLimitInit)
ON_FRESH_VI("name", FRCMD_INIT, FrCaddieWarningDlg::OnNameInit)
ON_FRESH_VI("message", FRCMD_INIT, FrCaddieWarningDlg::OnMessageInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrCaddieWarningDlg::OnOkInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrCaddieWarningDlg::OnOkBtnUp)
ON_FRESH_VI("warning_button", FRCMD_INIT,
	FrCaddieWarningDlg::OnWarningButtonInit)
ON_FRESH_VI("warning_text", FRCMD_INIT, FrCaddieWarningDlg::OnWarningTextInit)

END_FRESH_MSGMAP()

FrCaddieWarningDlg::FrCaddieWarningDlg()
{
	memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));

	m_pPortrait = NULL;
	m_pVacation = NULL;
	m_pLimit = NULL;
	m_pName = NULL;
	m_pMessage = NULL;
	m_pOk = NULL;
	m_pWarningButton = NULL;
	m_pWarningText = NULL;
}

void FrCaddieWarningDlg::SetCaddieInfo(const sCaddieInfo& info)
{
	m_caddieInfo = info;
	m_caddieInfo.byCheckCaddieWarning = 1;
}

void FrCaddieWarningDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, param);
}

void FrCaddieWarningDlg::OnVacationInit(int param)
{
	m_pVacation = DYNAMIC_CAST(FrArea, param);
}

void FrCaddieWarningDlg::OnLimitInit(int param)
{
	m_pLimit = DYNAMIC_CAST(FrArea, param);

	if (m_pLimit)
		m_pLimit->SetVisible(false);
}

void FrCaddieWarningDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, param);
}

void FrCaddieWarningDlg::OnMessageInit(int param)
{
	m_pMessage = DYNAMIC_CAST(FrEdit, param);
}

void FrCaddieWarningDlg::OnOkInit(int param)
{
	m_pOk = DYNAMIC_CAST(FrButton, param);
}

void FrCaddieWarningDlg::OnWarningButtonInit(int param)
{
	m_pWarningButton = DYNAMIC_CAST(FrButton, param);
}

void FrCaddieWarningDlg::OnWarningTextInit(int param)
{
	m_pWarningText = DYNAMIC_CAST(FrStatic, param);
}

void FrCaddieWarningDlg::OnOkBtnUp()
{
	if (m_pOk)
		m_pOk->Enable(false);

	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieInfo.tid);

	if (pCaddie == NULL)
	{
		Close(FrOK, true);
		return;
	}

	if (m_pWarningButton->GetStatus() == FrButton::PRESSED)
	{
		Doc()->m_caddieMap[m_caddieInfo.guid].byCheckCaddieWarning = 1;
		m_caddieInfo.byCheckCaddieWarning = 1;
	}
	else
	{
		Doc()->m_caddieMap[m_caddieInfo.guid].byCheckCaddieWarning = 0;
		m_caddieInfo.byCheckCaddieWarning = 0;
	}

	WSendPacket send((enumClientPacket)0x6b);

	send.Encode4(m_caddieInfo.guid);
	send.Encode1(m_caddieInfo.byCheckCaddieWarning);
	send.Send(TO_GAME);

	Close(FrOK, true);
}

bool FrCaddieWarningDlg::OnInit()
{
	if (m_caddieInfo.tid == 0)
		return false;

	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieInfo.tid);
	bool bInfo = false;

	if (m_pPortrait)
	{
		if (pCaddie)
		{
			bInfo = true;
			m_pPortrait->SetBgImg(pCaddie->c.Icon);
		}
		else
		{
			m_pPortrait->SetBgImg("hide");
		}
	}

	if (bInfo && m_pName)
	{
		char szText[256] = {
			0,
		};

		sprintf(szText, "\xc0\xcc  \xb8\xa7: %s", pCaddie->c.Name);
		unsigned char level;
		if ((level = m_caddieInfo.Level) < 3 && pCaddie->c.InStock != 5)
		{
			unsigned long exp = Doc()->m_bonusPangTable[level].exp;
			unsigned long curExp = m_caddieInfo.Exp;

			strcat(szText,
				MakeStr("\x0a\xb7\xb9  \xba\xa7: %d / %d ( %d / %d )",
					level + 1, 3, curExp, exp));
		}

		if (m_caddieInfo.Rent_flag && pCaddie->MonthlyFee > 0)
		{
			strcat(szText, "\x0a\xc8\xde\xb0\xa1\xc1\xdf");
		}

		m_pName->AddText(szText, false, true);

		SetMessage(
			"\x5c"
			"cT\x5c"
			"c\x5c"
			"c0xffff0000\x5c"
			"c\xc7\xf6\xc0\xe7 \xc4\xb3\xb5\xf0\xb0\xa1 \xc8\xde\xb0\xa1\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.\x0a\xb0\xd4\xc0\xd3 \xbd\xc3\xc0\xdb \xc0\xfc\xbf\xa1 \xc0\xe7\xb0\xed\xbf\xeb\xc7\xcf\xbd\xc3\xb4\xc2 \xb0\xcd \xc0\xd8\xc1\xf6 \xb8\xb6\xbc\xbc\xbf\xe4!\x5c"
			"cN\x5c"
			"c\x5c"
			"c0xff000000\x5c"
			"c\x0a\x0a(\xbe\xcb\xb8\xb2\xc0\xbb \xb9\xde\xc1\xf6 \xbe\xca\xc0\xb8\xbd\xc3\xb7\xc1\xb8\xe9 \xbf\xf9\xb1\xde \xb3\xaf\xc2\xa5 \xbe\xcb\xb8\xb2 \xb1\xe2\xb4\xc9 \xc3\xbc\xc5\xa9\xb8\xa6 \xb2\xf6 \xc8\xc4 \xc8\xae\xc0\xce \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf\xc1\xd6\xbc\xbc\xbf\xe4)",
			false);
	}
	else
	{
		if (m_pName)
			m_pName->AddText(
				"\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\x0a\x0a\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed",
				false, true);

		SetMessage(
			"\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7",
			false);
	}

	if (pCaddie)
	{
		if (m_pWarningButton)
		{
			if (m_caddieInfo.byCheckCaddieWarning == 1)
			{
				m_pWarningButton->SetStatus(FrButton::PRESSED);
			}
			else
			{
				m_pWarningButton->SetStatus(FrButton::NORMAL);
			}
		}

		if (Doc()->m_myInfo.stat.Level < pCaddie->c.Level)
		{
			if (m_pLimit)
				m_pLimit->SetVisible(true);

			SetDesc(
				"\xb7\xb9\xba\xa7 \xc1\xa6\xc7\xd1\xc0\xb8\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xc4\xb3\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9.");
		}
	}

	return true;
}
