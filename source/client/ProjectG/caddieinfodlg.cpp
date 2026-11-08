#include "caddieinfodlg.h"
#include "actor.h"
#include "projectg.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrCaddieInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrCaddieInfoDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrCaddieInfoDlg::OnPortraitInit)
ON_FRESH_VI("vacation", FRCMD_INIT, FrCaddieInfoDlg::OnVacationInit)
ON_FRESH_VI("limit", FRCMD_INIT, FrCaddieInfoDlg::OnLimitInit)
ON_FRESH_VI("name", FRCMD_INIT, FrCaddieInfoDlg::OnNameInit)
ON_FRESH_VI("contract", FRCMD_INIT, FrCaddieInfoDlg::OnContractInit)
ON_FRESH_VI("caddiefee", FRCMD_INIT, FrCaddieInfoDlg::OnCaddieFeeInit)
ON_FRESH_VI("signup", FRCMD_INIT, FrCaddieInfoDlg::OnSignUpInit)
ON_FRESH_VV("signup", FRCMD_LBUTTONUP, FrCaddieInfoDlg::OnSignUpBtnUp)
ON_FRESH_VI("cancel", FRCMD_INIT, FrCaddieInfoDlg::OnCancelInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrCaddieInfoDlg::OnCancelBtnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrCaddieInfoDlg::OnOkInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrCaddieInfoDlg::OnOkBtnUp)
ON_FRESH_VI("warning_button", FRCMD_INIT, FrCaddieInfoDlg::OnWarningButtonInit)
ON_FRESH_VI("warning_text", FRCMD_INIT, FrCaddieInfoDlg::OnWarningTextInit)

END_FRESH_MSGMAP()

FrCaddieInfoDlg::FrCaddieInfoDlg()
{
	memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
	m_pPortrait = NULL;
	m_pVacation = NULL;
	m_pLimit = NULL;
	m_pName = NULL;
	m_pMessage = NULL;
	m_pContract = NULL;
	m_pCaddieFee = NULL;
	m_pSignUp = NULL;
	m_pOk = NULL;
	m_pCancel = NULL;
	m_pWarningButton = NULL;
	m_pWarningText = NULL;
	m_pContractDlg = NULL;
}

void FrCaddieInfoDlg::SetCaddieInfo(const sCaddieInfo& info)
{
	m_caddieInfo = Doc()->m_caddieMap[info.guid];
	if (m_caddieInfo.byCheckCaddieWarning == 1)
		m_pWarningButton->SetStatus(FrButton::PRESSED);
	else
		m_pWarningButton->SetStatus(FrButton::NORMAL);
	CaddieIterator it = ItemManager()->m_CaddieMap.begin();
	CaddieIterator end = ItemManager()->m_CaddieMap.end();
	for (; it != end; ++it)
	{
		if ((*it).second.c.TypeId == m_caddieInfo.tid)
		{
			if ((*it).second.c.Level > Doc()->m_myInfo.stat.Level)
				m_pSignUp->Enable(false);
			break;
		}
	}
}

void FrCaddieInfoDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnVacationInit(int param)
{
	m_pVacation = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnLimitInit(int param)
{
	m_pLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnContractInit(int param)
{
	m_pContract = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnCaddieFeeInit(int param)
{
	m_pCaddieFee = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnSignUpInit(int param)
{
	m_pSignUp = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnOkInit(int param)
{
	m_pOk = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOk)
		m_pOk->SetVisible(false);
}

void FrCaddieInfoDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnWarningButtonInit(int param)
{
	m_pWarningButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnWarningTextInit(int param)
{
	m_pWarningText = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrCaddieInfoDlg::OnSignUpBtnUp()
{
	FrStatic* pFee;
	if (m_pSignUp)
		m_pSignUp->Enable(false);
	if (m_pCancel)
		m_pCancel->Enable(false);
	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieInfo.tid);
	if (!pCaddie)
		return;
	if (!m_pContractDlg)
		m_pContractDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
			"cad_contract", NULL);
	m_pContractDlg->SetMessage(
		"\xc4\xb3\xb5\xf0\xb8\xa6 \xc0\xe7\xb0\xed\xbf\xeb\xc7\xcf\xb8\xe9 \xbe\xc6\xb7\xa1\xbf\xcd \xb0\xb0\xc0\xcc \xb1\xdd\xbe\xd7\xc0\xcc \\c0xffff0000\\c\xb0\xa8\xbc\xd2\\c0xff000000\\c\xc7\xd5\xb4\xcf\xb4\xd9.\n\n\xc4\xb3\xb5\xf0\xb8\xa6 \\c0xffff0000\\c\xc7\xd1\xb4\xde\xb0\xa3 \\c0xff000000\\c\xc0\xe7\xb0\xed\xbf\xeb \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	pFee =
		DYNAMIC_CAST(FrStatic, (FrWnd*)m_pContractDlg->FindChildByName("fee"));
	if (pFee)
		pFee->SetCaption(MakeStr("%d", pCaddie->MonthlyFee));
	pFee = DYNAMIC_CAST(FrStatic,
		(FrWnd*)m_pContractDlg->FindChildByName("mypang"));
	if (pFee)
		pFee->SetCaption(MakeStr("%I64d", Doc()->m_myInfo.stat.i64Pang));
	pFee = DYNAMIC_CAST(FrStatic,
		(FrWnd*)m_pContractDlg->FindChildByName("remainpang"));
	if (pFee)
		pFee->SetCaption(MakeStr("%I64d",
			Doc()->m_myInfo.stat.i64Pang - pCaddie->MonthlyFee));
	m_pContractDlg->Open(
		(FRESH_PFN_RESULT)&FrCaddieInfoDlg::OnSignUpConfirmDlgResult, 1);
}

void FrCaddieInfoDlg::OnOkBtnUp()
{
	SendCaddieWarningCheckOption();
	Close(FrOK, true);
}

void FrCaddieInfoDlg::OnCancelBtnUp()
{
	SendCaddieWarningCheckOption();
	Close(FrCANCEL, true);
}

bool FrCaddieInfoDlg::OnSignUpConfirmDlgResult(int result, FrForm* form)
{
	m_pContractDlg = NULL;
	if (result == 1 || result == 2)
		SendCaddieWarningCheckOption();
	if (result == 1)
	{
		WSendPacket send((enumClientPacket)57);
		send.Encode4(m_caddieInfo.guid);
		send.Send(TO_GAME);
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		g_audio->PlaySfx("caddie_return");
		g_audio->PlayBGM("caddie_return.mp3", true, true);
		Close(FrOK, true);
	}
	else
	{
		if (m_pSignUp && m_pSignUp->IsVisible())
			m_pSignUp->Enable(true);
		if (m_pCancel && m_pCancel->IsVisible())
			m_pCancel->Enable(true);
	}
	return true;
}

void FrCaddieInfoDlg::SendCaddieWarningCheckOption()
{
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
}

bool FrCaddieInfoDlg::OnInit()
{
	if (!m_caddieInfo.tid)
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
			m_pPortrait->SetBgImg("hide");
	}
	if (bInfo && m_pName)
	{
		char text[256];
		sprintf(text, "\xc0\xcc  \xb8\xa7: %s", pCaddie->c.Name);
		unsigned char level;
		if ((level = m_caddieInfo.Level) <= 3 && pCaddie->c.InStock != 5)
		{
			unsigned long exp = 0, currentExp = 0;
			if (level < 3)
			{
				exp = Doc()->m_bonusPangTable[level].exp;
				currentExp = m_caddieInfo.Exp;
			}
			strcat(text,
				MakeStr("\n\xb7\xb9  \xba\xa7: %d / %d ( %d / %d )", level + 1,
					4, currentExp, exp));
		}
		if (m_caddieInfo.Rent_flag && pCaddie->MonthlyFee > 0)
		{
			if (m_caddieInfo.Remain_Date > 0)
				strcat(text,
					MakeStr(
						"\n\xb3\xb2\xc0\xba \xb0\xed\xbf\xeb\xb1\xe2\xb0\xa3: %d\xc0\xcf",
						m_caddieInfo.Remain_Date));
			else
				strcat(text, "\n\xc8\xde\xb0\xa1\xc1\xdf");
		}
		if (m_caddieInfo.tidPart & 0x1fff)
		{
			IFF_STRUCT::sCadItem* pPart =
				ItemManager()->FindCadItem(m_caddieInfo.tidPart);
			strcat(text,
				MakeStr(
					"\n\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5: \\c0xffff0000\\c%s\\c0xff000000\\c",
					pPart->c.Name));
			if (m_caddieInfo.Remain_Partdate > 24)
				strcat(text,
					MakeStr(
						"\n\xc4\xb3\xb5\xf0\xb2\xd9\xb9\xcc\xb1\xe2 \xb3\xb2\xc0\xba \xb1\xe2\xb0\xa3: \\c0xffff0000\\c%d\xc0\xcf\\c0xff000000\\c",
						m_caddieInfo.Remain_Partdate / 24));
			else
				strcat(text,
					MakeStr(
						"\n\xc4\xb3\xb5\xf0\xb2\xd9\xb9\xcc\xb1\xe2 \xb3\xb2\xc0\xba \xb1\xe2\xb0\xa3: \\c0xffff0000\\c%d\xbd\xc3\xb0\xa3\\c0xff000000\\c",
						m_caddieInfo.Remain_Partdate));
			if (m_caddieInfo.Rent_flag && pCaddie->MonthlyFee > 0 &&
				m_caddieInfo.Remain_Date > 0)
				HideContractControls();
		}
		m_pName->AddText(text, false, true);
		if (m_caddieInfo.Remain_Date > 0 &&
				(m_caddieInfo.gift_flag || m_caddieInfo.Rent_flag) &&
				pCaddie->MonthlyFee > 0 ||
			pCaddie->MonthlyFee == 0 || m_caddieInfo.guid == -1)
		{
			IFF_STRUCT::sDesc* pDesc =
				ItemManager()->FindDesc(pCaddie->c.TypeId);
			if (pDesc)
				SetMessage(pDesc->Desc, false);
		}
		else
			SetMessage(
				MakeStr(
					"\xc7\xf6\xc0\xe7 \xc4\xb3\xb5\xf0\xb4\xc2 \xc8\xde\xb0\xa1\xc1\xdf\xc0\xcc\xb9\xc7\xb7\xce \xc4\xb3\xb5\xf0 \xb2\xd9\xb9\xcc\xb1\xe2\xb8\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. %d \xc6\xce\xc0\xbb \xc1\xf6\xb1\xde\xc7\xcf\xbf\xa9 \xc4\xb3\xb5\xf0\xb8\xa6 \xc8\xde\xb0\xa1\xba\xb9\xb1\xcd\xbd\xc3\xc5\xb0\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					pCaddie->MonthlyFee),
				false);
	}
	else
	{
		if (m_pName)
			m_pName->AddText(
				"\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\n\n\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed",
				false, true);
		SetMessage(
			"\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7",
			false);
	}
	if (pCaddie)
	{
		if (m_caddieInfo.Rent_flag && pCaddie->MonthlyFee > 0)
		{
			m_pWarningButton->SetVisible(true);
			m_pWarningText->SetVisible(true);
		}
		else
		{
			m_pWarningButton->SetVisible(false);
			m_pWarningText->SetVisible(false);
		}
		if (m_caddieInfo.Rent_flag && !m_caddieInfo.Remain_Date &&
			pCaddie->MonthlyFee > 0 && m_pCaddieFee)
		{
			char fee[64];
			sprintf(fee, "%d \xc6\xce", pCaddie->MonthlyFee);
			m_pCaddieFee->SetLine(1, fee, 0, 0, false);
			SetDesc(
				"'\xc1\xf6\xb1\xde' \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc4\xb3\xb5\xf0\xb8\xa6 \xc0\xe7\xb0\xed\xbf\xeb \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		}
		else
			HideContractControls();
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

void FrCaddieInfoDlg::HideContractControls()
{
	if (m_pVacation)
		m_pVacation->SetVisible(false);
	if (m_pMessage)
		m_pMessage->SetVisible(false);
	if (m_pContract)
		m_pContract->SetVisible(false);
	if (m_pCaddieFee)
		m_pCaddieFee->SetVisible(false);
	if (m_pSignUp)
		m_pSignUp->SetVisible(false);
	if (m_pLimit)
		m_pLimit->SetVisible(false);
	if (m_pCancel)
		m_pCancel->SetVisible(false);
	if (m_pOk)
		m_pOk->SetVisible(true);
	SetDesc("");
}

void FrCaddieInfoDlg::EndContract(int result)
{
	if (result == 2)
	{
		m_caddieInfo.Remain_Date = 30;
		HideContractControls();
		if (m_pOk && m_pOk->IsVisible())
			m_pOk->Enable(true);
	}
	else
	{
		if (m_pSignUp && m_pSignUp->IsVisible())
			m_pSignUp->Enable(true);
		if (m_pCancel && m_pCancel->IsVisible())
			m_pCancel->Enable(true);
	}
}
