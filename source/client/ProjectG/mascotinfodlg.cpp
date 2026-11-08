#include "../../shared/sharedtables.h"
#include "mascotinfodlg.h"
#include "mascot.h"
#include "logininfo.h"
#include "actor.h"
#include "../../shared/token.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrMascotInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMascotInfoDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrMascotInfoDlg::OnPortraitInit)
ON_FRESH_VI("vacation", FRCMD_INIT, FrMascotInfoDlg::OnVacationInit)
ON_FRESH_VI("limit", FRCMD_INIT, FrMascotInfoDlg::OnLimitInit)
ON_FRESH_VI("name", FRCMD_INIT, FrMascotInfoDlg::OnNameInit)
ON_FRESH_VI("text_message", FRCMD_INIT, FrMascotInfoDlg::OnTextMessageInit)
ON_FRESH_VI("mascot_msg", FRCMD_INIT, FrMascotInfoDlg::OnMascotMsgInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrMascotInfoDlg::OnOkInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrMascotInfoDlg::OnOkBtnUp)
ON_FRESH_VI("cancel", FRCMD_INIT, FrMascotInfoDlg::OnCancelInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrMascotInfoDlg::OnCancelBtnUp)

END_FRESH_MSGMAP()

FrMascotInfoDlg::FrMascotInfoDlg()
{
	memset(&m_mascotInfo, 0, sizeof(m_mascotInfo));
	m_pPortrait = NULL;
	m_pVacation = NULL;
	m_pLimit = NULL;
	m_pName = NULL;
	m_pMascotMsg = NULL;
	m_pOk = NULL;
	m_pCancel = NULL;
	m_pConfirmDlg = NULL;
	m_pPuppet = NULL;
}

void FrMascotInfoDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrMascotInfoDlg::OnVacationInit(int param)
{
	m_pVacation = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pVacation)
		m_pVacation->SetVisible(false);
}

void FrMascotInfoDlg::OnLimitInit(int param)
{
	m_pLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pLimit)
		m_pLimit->SetVisible(false);
}

void FrMascotInfoDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrMascotInfoDlg::OnTextMessageInit(int param)
{
	m_pTextMessage = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pTextMessage)
		m_pTextMessage->SetVisible(false);
}

void FrMascotInfoDlg::OnMascotMsgInit(int param)
{
	m_pMascotMsg = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pMascotMsg)
		m_pMascotMsg->SetVisible(false);
}

void FrMascotInfoDlg::OnOkInit(int param)
{
	m_pOk = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOk)
		m_pOk->Enable(false);
}

void FrMascotInfoDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMascotInfoDlg::SetMascotInfo(const sMascotInfo& info, WPuppet* pPuppet)
{
	m_mascotInfo = info;
	m_message = m_mascotInfo.szMsg;
	m_pPuppet = pPuppet;
}

void FrMascotInfoDlg::OnCancelBtnUp()
{
	if (strcmp(m_mascotInfo.szMsg, m_message.c_str()))
	{
		IFF_STRUCT::sMascot* pMascot =
			ItemManager()->FindMascot(m_mascotInfo.tid);
		if (m_pPuppet && pMascot)
			CMascot::ChangeMascotMsgInTex(m_mascotInfo.szMsg, pMascot,
				m_pPuppet,
				MakeStr("myroom%02d", (pMascot->c.TypeId & 0x3ffffff)));
	}
	OnFreshCancel();
}

bool FrMascotInfoDlg::OnChangeMsgConfirmDlgResult(int result, FrForm* pForm)
{
	m_pConfirmDlg = NULL;
	if (result == 1)
	{
		if (CLoginInfo::Instance()->IsPcBang() && m_mascotInfo.PCBangMascot)
		{
			WSendPacket send((enumClientPacket)151);
			send.Encode1(1);
			send.Encode4(m_mascotInfo.guid);
			send.EncodeStr(m_message);
			send.Send(TO_GAME);
		}
		else
		{
			WSendPacket send((enumClientPacket)115);
			send.Encode4(m_mascotInfo.guid);
			send.EncodeStr(m_message);
			send.Send(TO_GAME);
		}
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		Close(FrOK, true);
	}
	return true;
}

bool FrMascotInfoDlg::OnInit()
{
	if (!m_mascotInfo.tid)
		return false;
	if ((m_mascotInfo.tid & 0x3ffffff) == 4)
		m_pMascotMsg->SetCharLimit(8, true);
	IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_mascotInfo.tid);
	bool bInfo = false;
	if (m_pPortrait)
	{
		if (pMascot)
		{
			bInfo = true;
			m_pPortrait->SetBgImg(pMascot->c.Icon);
		}
		else
			m_pPortrait->SetBgImg("hide");
	}
	if (bInfo && m_pName)
	{
		char text[512] = { 0 };
		sprintf(text, "\xc0\xcc  \xb8\xa7: %s", pMascot->c.Name);
		if (pMascot->ItemDropUp > 0)
			strcat(text,
				MakeStr(
					"\n\xbe\xc6\xc0\xcc\xc5\xdb \xb5\xe5\xb7\xd3\xb7\xfc : %d%%",
					pMascot->ItemDropUp));
		if (pMascot->PangUp > 0)
			strcat(text,
				MakeStr(
					"\n\xc3\xdf\xb0\xa1 \xc6\xce \xc8\xb9\xb5\xe6\xb7\xfc : %d%%",
					pMascot->PangUp - 100));
		if (pMascot->ExpUp > 0)
			strcat(text,
				MakeStr(
					"\n\xc3\xdf\xb0\xa1 \xb0\xe6\xc7\xe8\xc4\xa1 \xc8\xb9\xb5\xe6\xb7\xfc : %d%%",
					pMascot->ExpUp - 100));
		if (pMascot->ItemSlot > 0)
			strcat(text,
				MakeStr(
					"\n\xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 : %d\xc4\xad",
					pMascot->ItemSlot));
		unsigned short days = m_mascotInfo.Remain_Date / 24;
		unsigned short hours = m_mascotInfo.Remain_Date % 24;
		strcat(text, "\n\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3 : ");
		bool special = false;
		if (IsLocalContent((localContentType_t)108))
		{
			if (m_mascotInfo.tid == 0x40000006)
			{
				strcat(text, "\xb9\xab\xc1\xa6\xc7\xd1");
				special = true;
			}
			if (m_mascotInfo.tid == 0x40000004)
			{
				strcat(text, "PC\xb9\xe6 \xb9\xab\xc1\xa6\xc7\xd1");
				special = true;
			}
			if (m_mascotInfo.tid == 0x40000013)
			{
				strcat(text, "\xb9\xab\xc1\xa6\xc7\xd1");
				special = true;
			}
		}
		if (!special)
		{
			if (days > 0)
				strcat(text, MakeStr("%d\xc0\xcf ", days));
			if (hours > 0)
				strcat(text, MakeStr("%d\xbd\xc3\xb0\xa3", hours));
			if (days <= 0 && hours <= 0)
				strcat(text, "\xc1\xbe\xb7\xe1");
		}
		m_pName->AddText(text, false, true);
		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pMascot->c.TypeId);
		if (pDesc)
			SetMessage(pDesc->Desc, false);
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
	if (pMascot)
	{
		if (Doc()->m_myInfo.stat.Level < pMascot->c.Level)
		{
			if (m_pLimit)
				m_pLimit->SetVisible(true);
			SetDesc(
				"\xb7\xb9\xba\xa7 \xc1\xa6\xc7\xd1\xc0\xb8\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		}
		if (pMascot->ShowMsg)
		{
			if (m_pMascotMsg)
			{
				m_pMascotMsg->SetVisible(true);
				m_pMascotMsg->SetLine(1, m_mascotInfo.szMsg, 0, 0, false);
			}
			if (m_pTextMessage)
				m_pTextMessage->SetVisible(true);
			if (m_pOk)
				m_pOk->Enable(true);
		}
	}
	return true;
}

void FrMascotInfoDlg::OnProc(const float dt)
{
	if (m_pMascotMsg)
	{
		if (stricmp(m_pMascotMsg->GetLine(1, false), m_message.c_str()))
		{
			m_message = m_pMascotMsg->GetLine(1, false);
			IFF_STRUCT::sMascot* pMascot =
				ItemManager()->FindMascot(m_mascotInfo.tid);
			if (m_pPuppet && pMascot)
				CMascot::ChangeMascotMsgInTex(m_message.c_str(), pMascot,
					m_pPuppet,
					MakeStr("myroom%02d", (pMascot->c.TypeId & 0x3ffffff)));
		}
	}
}

void FrMascotInfoDlg::OnOkBtnUp()
{
	if (!strcmp(m_message.c_str(), m_mascotInfo.szMsg))
		return;
	if (m_mascotInfo.tid == 0x40000004)
	{
		if (strlen(m_message.c_str()) > 8)
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
				(int)"\xb8\xde\xbc\xbc\xc1\xf6 \xc3\xd6\xb4\xeb \xb1\xe6\xc0\xcc\xb4\xc2 \xbf\xb5\xb9\xae 8\xb1\xdb\xc0\xda, \xc7\xd1\xb1\xdb 4\xb1\xdb\xc0\xda \xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			return;
		}
	}
	else if (strlen(m_message.c_str()) > 16)
	{
		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
			(int)"\xb8\xde\xbc\xbc\xc1\xf6 \xc3\xd6\xb4\xeb \xb1\xe6\xc0\xcc\xb4\xc2 \xbf\xb5\xb9\xae 16\xb1\xdb\xc0\xda, \xc7\xd1\xb1\xdb 8\xb1\xdb\xc0\xda \xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}
	cTokenV token;
	token.Init(m_message.c_str(), strlen(m_message.c_str()));
	int count = token.GetTokenNum(0, "'", 1) - 1;
	if (count > 0)
	{
		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
			(int)"[']\xb4\xc2 \xb8\xde\xbc\xbc\xc1\xf6\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else
	{
		char text[256];
		strcpy(text, m_message.c_str());
		if (Doc()->m_chatManager.Filtering(text))
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
				(int)"\xc0\xfb\xb4\xe7\xc7\xcf\xc1\xf6\xbe\xca\xc0\xba \xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		else
		{
			if (!m_pConfirmDlg)
				m_pConfirmDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"mascot_msg", NULL);
			if (m_pConfirmDlg)
			{
				m_pConfirmDlg->SetMessage(
					"\xb8\xb6\xbd\xba\xc4\xda\xc6\xae \xb8\xde\xbc\xbc\xc1\xf6\xb8\xa6 \xba\xaf\xb0\xe6 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					false);
				m_pConfirmDlg->Open((FRESH_PFN_RESULT)&FrMascotInfoDlg::
										OnChangeMsgConfirmDlgResult,
					1);
			}
		}
	}
}
