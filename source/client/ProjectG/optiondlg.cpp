#include "frform.h"
#include "optiondlg.h"
#include "taskmanager.h"
#include "golftask.h"
#include "lobbytask.h"
#include "inputmanager.h"
#include "schooldlg.h"
#include "messengerdlg.h"
#include "mousecursor.h"
#include "binstr.h"
#include "gatewayactor.h"
#include "../../shared/s5/sharedutilities.h"

extern Fresh* g_pFresh;

FrOptionDlg::eTabType FrOptionDlg::ms_eCurTab = FrOptionDlg::TAB_GAME;

FrOptionDlg::sTab::sTab()
{
	m_bSelected = true;
	m_pTabBtn = NULL;
	m_num = 0;
	m_ppStatic = NULL;
	m_pppCtrl = NULL;
}

void FrOptionDlg::sTab::SetSelected(bool bSelect)
{
	m_bSelected = bSelect;
	if (!bSelect)
		m_pTabBtn->SetStatus(FrButton::NORMAL);
	if (m_ppStatic)
		for (int i = 0; i < m_num; ++i)
			if (m_ppStatic[i])
			{
				if (m_pbDisable && m_pbDisable[i])
					m_ppStatic[i]->Enable(false);
				else
					m_ppStatic[i]->Enable(m_bSelected);
				m_ppStatic[i]->SetVisible(m_bSelected);
			}
	if (m_pppCtrl)
		for (int i = 0; i < m_num; ++i)
			if (m_pppCtrl[i] && *m_pppCtrl[i])
			{
				if (m_pbDisable && m_pbDisable[i])
					(*m_pppCtrl[i])->Enable(false);
				else
					(*m_pppCtrl[i])->Enable(m_bSelected);
				(*m_pppCtrl[i])->EnableHover(m_bSelected, 0.0f);
				(*m_pppCtrl[i])->SetVisible(m_bSelected);
			}
}

FrOptionDlg::sGameTab::sGameTab()
{
	m_num = 13;
	m_ppStatic = m_pStatic;
	m_pppCtrl = m_ppCtrl;
	m_pbDisable = NULL;
	memset(m_pStatic, 0, sizeof(m_pStatic));
	m_pMsSensiGauge = NULL;
	m_pWhisperBtn = NULL;
	m_pInvitationBtn = NULL;
	m_pFriendConfirmBtn = NULL;
	m_pTransChatWinBtn = NULL;
	m_pUiEffectBtn = NULL;
	m_pNickEdit = NULL;
	m_pChangeNickBtn = NULL;
	m_pSchoolEdit = NULL;
	m_pChangeSchoolBtn = NULL;
	m_pPowerGaugeBtn = NULL;
	m_pHideGUIBtn = NULL;
	m_pHidePIBtn = NULL;
	m_pCutinDisplayBtn = NULL;
	m_ppCtrl[0] = (FrWnd**)&m_pMsSensiGauge;
	m_ppCtrl[1] = (FrWnd**)&m_pWhisperBtn;
	m_ppCtrl[2] = (FrWnd**)&m_pInvitationBtn;
	m_ppCtrl[3] = (FrWnd**)&m_pFriendConfirmBtn;
	m_ppCtrl[4] = (FrWnd**)&m_pTransChatWinBtn;
	m_ppCtrl[5] = (FrWnd**)&m_pUiEffectBtn;
	m_ppCtrl[6] = (FrWnd**)&m_pNickEdit;
	m_ppCtrl[7] = (FrWnd**)&m_pSchoolEdit;
	m_ppCtrl[8] = (FrWnd**)&m_pPowerGaugeBtn;
	m_ppCtrl[9] = (FrWnd**)&m_pHideGUIBtn;
	m_ppCtrl[10] = (FrWnd**)&m_pHidePIBtn;
	m_ppCtrl[11] = (FrWnd**)&m_pTerrainBtn;
	m_ppCtrl[12] = (FrWnd**)&m_pCutinDisplayBtn;
	m_bEnableChangeNick = false;
}

void FrOptionDlg::sGameTab::SetSelected(bool bSelect)
{
	sTab::SetSelected(bSelect);
	if (bSelect)
	{
		if (m_pChangeNickBtn)
		{
			m_pChangeNickBtn->SetVisible(true);
			m_pChangeNickBtn->Enable(m_bEnableChangeNick);
		}
		if (m_pChangeSchoolBtn)
			m_pChangeSchoolBtn->SetVisible(true);
	}
	else
	{
		if (m_pChangeNickBtn)
			m_pChangeNickBtn->SetVisible(false);
		if (m_pChangeSchoolBtn)
			m_pChangeSchoolBtn->SetVisible(false);
	}
}

void FrOptionDlg::sGameTab::Init()
{
	if (OnlinePlay() && !IS_KINDOF(CGolfTask, AfxGetTask()))
		m_bEnableChangeNick = IsEnableChangeNickname();
}

bool FrOptionDlg::sGameTab::IsEnableChangeNickname()
{
	if (IsLocalContent(S4_NT_NICKNAME_CHANGE_2ND))
	{
		if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
		{
			if (g_pFresh->IsCurrentLayout("GAMEROOM"))
				return false;
			if (g_pFresh->IsCurrentLayout("GAMEROOM_EXT"))
				return false;
			if (g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"))
				return false;
		}
		return true;
	}
	return false;
}

FrOptionDlg::sVideoTab::sVideoTab()
{
	m_num = 13;
	m_ppStatic = m_pStatic;
	m_pppCtrl = m_ppCtrl;
	m_pbDisable = m_bDisable;
	memset(m_pStatic, 0, sizeof(m_pStatic));
	memset(m_bDisable, 0, sizeof(m_bDisable));
	m_pFullScreenBtn = NULL;
	m_pFillScreenBtn = NULL;
	m_pWideModeBtn = NULL;
	m_pOverallCmb = NULL;
	m_pScrResCmb = NULL;
	m_pTnlCmb = NULL;
	m_pTexResCmb = NULL;
	m_pShadowBtn = NULL;
	m_pShadowmapBtn = NULL;
	m_pReflectionBtn = NULL;
	m_pLodBtn = NULL;
	m_pNpcBtn = NULL;
	m_pParticleCmb = NULL;
	m_ppCtrl[0] = (FrWnd**)&m_pFullScreenBtn;
	m_ppCtrl[1] = (FrWnd**)&m_pFillScreenBtn;
	m_ppCtrl[2] = (FrWnd**)&m_pWideModeBtn;
	m_ppCtrl[3] = (FrWnd**)&m_pOverallCmb;
	m_ppCtrl[4] = (FrWnd**)&m_pScrResCmb;
	m_ppCtrl[5] = (FrWnd**)&m_pTnlCmb;
	m_ppCtrl[6] = (FrWnd**)&m_pTexResCmb;
	m_ppCtrl[7] = (FrWnd**)&m_pShadowBtn;
	m_ppCtrl[8] = (FrWnd**)&m_pShadowmapBtn;
	m_ppCtrl[9] = (FrWnd**)&m_pReflectionBtn;
	m_ppCtrl[10] = (FrWnd**)&m_pLodBtn;
	m_ppCtrl[11] = (FrWnd**)&m_pNpcBtn;
	m_ppCtrl[12] = (FrWnd**)&m_pParticleCmb;
	if (!sOption::IsSupportShadow())
		m_bDisable[7] = true;
	if (!sOption::IsSupportReflection())
		m_bDisable[9] = true;
	if (!sOption::IsSupportLod())
		m_bDisable[10] = true;
}

FrOptionDlg::sAudioTab::sAudioTab()
{
	m_num = 5;
	m_ppStatic = m_pStatic;
	m_pppCtrl = m_ppCtrl;
	m_pbDisable = m_bDisable;
	memset(m_pStatic, 0, sizeof(m_pStatic));
	if (g_audio->GetMSS())
		memset(m_bDisable, 0, sizeof(m_bDisable));
	else
		memset(m_bDisable, 1, sizeof(m_bDisable));
	m_pEnableBtn = NULL;
	m_pOverallCmb = NULL;
	m_pSfxGauge = NULL;
	m_pBgmGauge = NULL;
	m_pSpeakerCmb = NULL;
	m_ppCtrl[0] = (FrWnd**)&m_pEnableBtn;
	m_ppCtrl[1] = (FrWnd**)&m_pOverallCmb;
	m_ppCtrl[2] = (FrWnd**)&m_pSfxGauge;
	m_ppCtrl[3] = (FrWnd**)&m_pBgmGauge;
	m_ppCtrl[4] = (FrWnd**)&m_pSpeakerCmb;
}

FrOptionDlg::sMacroTab::sMacroTab()
{
	m_num = 8;
	m_ppStatic = m_pStatic;
	m_pppCtrl = m_ppCtrl;
	m_pbDisable = m_bDisable;
	memset(m_pStatic, 0, sizeof(m_pStatic));
	memset(m_bDisable, 0, sizeof(m_bDisable));
	for (int i = 0; i < 8; ++i)
	{
		m_pMacro[i] = NULL;
		m_ppCtrl[i] = (FrWnd**)&m_pMacro[i];
	}
}

IMPLEMENT_OBJECT(FrChangeNickDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrChangeNickDlg, FrForm)
ON_FRESH_VI("sprice", FRCMD_INIT, FrChangeNickDlg::OnPriceStaticInit)
ON_FRESH_VI("price", FRCMD_INIT, FrChangeNickDlg::OnPriceInit)
ON_FRESH_VI("smycookie", FRCMD_INIT, FrChangeNickDlg::OnMyCookieStaticInit)
ON_FRESH_VI("mycookie", FRCMD_INIT, FrChangeNickDlg::OnMyCookieInit)
ON_FRESH_VI("sremaincookie", FRCMD_INIT,
	FrChangeNickDlg::OnRemainCookieStaticInit)
ON_FRESH_VI("remaincookie", FRCMD_INIT, FrChangeNickDlg::OnRemainCookieInit)
ON_FRESH_VI("checkout", FRCMD_INIT, FrChangeNickDlg::OnCheckoutInit)
ON_FRESH_VV("checkout", FRCMD_LBUTTONUP, FrChangeNickDlg::OnCheckoutBtnUp)
END_FRESH_MSGMAP()

FrChangeNickDlg::FrChangeNickDlg()
{
	m_nick = NULL;
	std::list<sItemInfo>::iterator it = std::find(Doc()->m_myItemList.begin(),
		Doc()->m_myItemList.end(), 0x1a000003UL);
	m_bHaveItem = it != Doc()->m_myItemList.end();
	m_pOptionDlg = NULL;
}

void FrChangeNickDlg::OnPriceStaticInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control && (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204)))
		control->SetVisible(false);
}

void FrChangeNickDlg::OnPriceInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control)
	{
		if (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204))
		{
			control->SetVisible(false);
		}
		else
		{
			if (IsLocalCountry(30))
				control->SetCaption(
					MakeStr("\x25\x64\xb0\xb3\x28\x43\x4b\x29", 49));
			else
				control->SetCaption(MakeStr("\x25\x64\xb0\xb3", 49));
		}
	}
}

void FrChangeNickDlg::OnMyCookieStaticInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control && (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204)))
		control->SetVisible(false);
}

void FrChangeNickDlg::OnMyCookieInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control)
	{
		if (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204))
			control->SetVisible(false);
		else
		{
			if (IsLocalCountry(30))
				control->SetCaption(
					MakeStr("\x25\x49\x36\x34\x64\xb0\xb3\x28\x43\x4b\x29",
						Doc()->m_cookie));
			else
				control->SetCaption(
					MakeStr("\x25\x49\x36\x34\x64\xb0\xb3", Doc()->m_cookie));
		}
	}
}

void FrChangeNickDlg::OnRemainCookieStaticInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control && (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204)))
		control->SetVisible(false);
}

void FrChangeNickDlg::OnRemainCookieInit(int param)
{
	FrStatic* control = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (control)
	{
		if (m_bHaveItem || (Doc()->m_myInfo.info.dwIdentity & 0x204))
		{
			control->SetVisible(false);
		}
		else
		{
			if (Doc()->m_cookie < 49)
				control->SetCaption(
					MakeStr("\x25\x49\x36\x34\x64\xb0\xb3\x20\xba\xce\xc1\xb7",
						49 - Doc()->m_cookie));
			else if (IsLocalCountry(30))
				control->SetCaption(
					MakeStr("\x25\x49\x36\x34\x64\xb0\xb3\x28\x43\x4b\x29",
						Doc()->m_cookie - 49));
			else
				control->SetCaption(MakeStr("\x25\x49\x36\x34\x64\xb0\xb3",
					Doc()->m_cookie - 49));
		}
	}
}

void FrChangeNickDlg::OnCheckoutInit(int param)
{
	FrButton* control = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (control && Doc()->m_cookie < 49 && !m_bHaveItem &&
		!(Doc()->m_myInfo.info.dwIdentity & 0x204))
		control->Enable(false);
}

void FrChangeNickDlg::OnCheckoutBtnUp()
{
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel");
	form->SetMessage(
		"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\xba\xaf\xb0\xe6\x20\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
		false);
	form->Open((FRESH_PFN_RESULT)&FrChangeNickDlg::OnChangeNickConfirmDlgResult,
		1);
}

bool FrChangeNickDlg::OnInit()
{
	char message[1024] = { 0 };
	const char* warning =
		"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\x2d\xb4\xd0\xb3\xd7\xc0\xd3\xc0\xbb\x20\xba\xaf\xb0\xe6\xc7\xcf\xbd\xc3\xb1\xe2\x20\xc0\xfc\xbf\xa1\x20\xba\xaf\xb0\xe6\xc7\xcf\xb0\xed\xc0\xda\x0a\x20\x20\xc7\xcf\xb4\xc2\x20\xb4\xd0\xb3\xd7\xc0\xd3\xc0\xbb\x20\xb2\xc0\x20\xc8\xae\xc0\xce\xc7\xcf\xbf\xa9\x20\xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0\x2e\x0a";
	strcpy(message, warning);
	strcat(message,
		"\x2d\xc0\xcc\x20\xbb\xf3\xc7\xb0\xc0\xba\x20\xc3\xbb\xbe\xe0\xc3\xb6\xc8\xb8\x28\xb1\xb8\xb8\xc5\xc3\xeb\xbc\xd2\x29\xb0\xa1\x20\xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1\x20\xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9\x2e\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\x0a");
	strcat(message,
		MakeStr(
			"\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\x27\x25\x73\x27\xbf\xa1\xbc\xad\x0a\x27\x25\x73\x27\x28\xc0\xb8\x29\xb7\xce\x20\xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9\x2e",
			Doc()->m_myInfo.info.sNick, m_nick));
	if (Doc()->m_myInfo.info.dwIdentity & 0x204)
		strcat(message,
			"\xc0\xcc\x20\xbe\xc6\xc0\xcc\xb5\xf0\xb4\xc2\x20\xb9\xab\xb7\xe1\xb7\xce\x20\xb4\xeb\xc8\xad\xb8\xed\x20\xba\xaf\xb0\xe6\xc0\xcc\x20\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9\x2e");
	if (m_bHaveItem)
	{
		strcat(message,
			"\x0a\x0a\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\xba\xaf\xb0\xe6\xc7\xd2\x20\xb6\xa7\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xb4\xeb\xc8\xad\xb8\xed\x20\xb8\xed\xc2\xfb\x20\xbe\xc6\xc0\xcc\xc5\xdb\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xc0\xcc\x20\x31\xb0\xb3\x20\xbc\xd2\xb8\xf0\xb5\xcb\xb4\xcf\xb4\xd9\x2e");
	}
	else
	{
		int provider = Doc()->m_provType;
		strcat(message,
			"\x0a\x0a\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\xba\xaf\xb0\xe6\xc7\xd2\x20\xb6\xa7\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\x34\x39\xc4\xed\xc5\xb0\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb0\xa1\x20\xc2\xf7\xb0\xa8\xb5\xcb\xb4\xcf\xb4\xd9\x2e");
		if (Doc()->m_cookie >= 49)
		{
			strcat(message,
				MakeStr(
					"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x2c\x74\x5c\x63\xba\xaf\xb0\xe6\x20\xc8\xc4\x20\xc3\xd6\xc1\xbe\x20\x25\x73\x20\xc0\xdc\xbe\xd7\x3a\x25\x49\x36\x34\x64\xbf\xf8\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x2c\x6e\x5c\x63",
					provider == 2 ? "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac"
								  : "\xc7\xd1\xc4\xda\xc0\xce",
					Doc()->m_cash));
		}
		else
		{
			__int64 cash = Doc()->m_cash - S5::ExchangeOwnCookieToCash(49, 100);
			if (cash >= 0)
				strcat(message,
					MakeStr(
						"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x2c\x74\x5c\x63\xba\xaf\xb0\xe6\x20\xc8\xc4\x20\xc3\xd6\xc1\xbe\x20\x25\x73\x20\xc0\xdc\xbe\xd7\x3a\x25\x49\x36\x34\x64\xbf\xf8\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x2c\x6e\x5c\x63",
						provider == 2 ? "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac"
									  : "\xc7\xd1\xc4\xda\xc0\xce",
						cash));
			else
				strcat(message,
					MakeStr(
						"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x2c\x74\x5c\x63\x25\x73\x20\xc0\xdc\xbe\xd7\xc0\xcc\x20\xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9\x2e",
						provider == 2 ? "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac"
									  : "\xc7\xd1\xc4\xda\xc0\xce"));
		}
	}
	SetMessage(message, false);
	return true;
}

bool FrChangeNickDlg::OnChangeNickConfirmDlgResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		if (Doc()->m_cookie >= 49 || m_bHaveItem ||
			(Doc()->m_myInfo.info.dwIdentity & 0x204))
		{
			WSendPacket packet((enumClientPacket)0x38);
			packet.EncodeStr(std::string(m_nick));
			packet.Send(TO_GAME);
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			if (m_pOptionDlg)
				m_pOptionDlg->EnableChangeNickControls(false);
			Close(FrOK, true);
		}
		else
		{
			MsgObject msg(NULL, 35,
				(int)"\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xc4\xed\xc5\xb0\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb0\xa1\x20\xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9\x2e\x0a\xbb\xf3\xc1\xa1\xbf\xa1\xbc\xad\x20\xc4\xed\xc5\xb0\xb8\xa6\x20\xc3\xe6\xc0\xfc\xc7\xd8\x20\xc1\xd6\xbc\xbc\xbf\xe4\x2e",
				0, 0, 0, 0);
			AfxGetTask()->GetActor("Lobby") << msg;
			if (m_pOptionDlg)
				m_pOptionDlg->EnableChangeNickControls(false);
		}
	}
	else if (m_pOptionDlg)
		m_pOptionDlg->EnableChangeNickControls(true);
	m_pOptionDlg = NULL;
	return true;
}

IMPLEMENT_OBJECT(FrOptionDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrOptionDlg, FrForm)
ON_FRESH_VI("ok", FRCMD_INIT, FrOptionDlg::OnOkBtnInit)
ON_FRESH_VI("game", FRCMD_INIT, FrOptionDlg::OnGameTabInit)
ON_FRESH_VV("game", FRCMD_LBUTTONDOWN, FrOptionDlg::OnGameTabUp)
ON_FRESH_VI("video", FRCMD_INIT, FrOptionDlg::OnVideoTabInit)
ON_FRESH_VV("video", FRCMD_LBUTTONDOWN, FrOptionDlg::OnVideoTabUp)
ON_FRESH_VI("audio", FRCMD_INIT, FrOptionDlg::OnAudioTabInit)
ON_FRESH_VV("audio", FRCMD_LBUTTONDOWN, FrOptionDlg::OnAudioTabUp)
ON_FRESH_VI("macro", FRCMD_INIT, FrOptionDlg::OnMacroTabInit)
ON_FRESH_VV("macro", FRCMD_LBUTTONDOWN, FrOptionDlg::OnMacroTabUp)
ON_FRESH_VI("g_ssensi", FRCMD_INIT, FrOptionDlg::gOnMsSensiStaticInit)
ON_FRESH_VI("g_sensi", FRCMD_INIT, FrOptionDlg::gOnMsSensiGaugeInit)
ON_FRESH_VI("g_swhisper", FRCMD_INIT, FrOptionDlg::gOnWhisperStaticInit)
ON_FRESH_VI("g_whisper", FRCMD_INIT, FrOptionDlg::gOnWhisperBtnInit)
ON_FRESH_VV("g_whisper", FRCMD_LBUTTONUP, FrOptionDlg::gOnWhisperBtnUp)
ON_FRESH_VI("g_sinvite", FRCMD_INIT, FrOptionDlg::gOnInvitationStaticInit)
ON_FRESH_VI("g_invite", FRCMD_INIT, FrOptionDlg::gOnInvitationBtnInit)
ON_FRESH_VV("g_invite", FRCMD_LBUTTONUP, FrOptionDlg::gOnInvitationBtnUp)
ON_FRESH_VI("g_sconfirm", FRCMD_INIT, FrOptionDlg::gOnFriendConfirmStaticInit)
ON_FRESH_VI("g_confirm", FRCMD_INIT, FrOptionDlg::gOnFriendConfirmBtnInit)
ON_FRESH_VV("g_confirm", FRCMD_LBUTTONUP, FrOptionDlg::gOnFriendConfirmBtnUp)
ON_FRESH_VI("g_schatwin", FRCMD_INIT, FrOptionDlg::gOnTransChatWinStaticInit)
ON_FRESH_VI("g_chatwin", FRCMD_INIT, FrOptionDlg::gOnTransChatWinBtnInit)
ON_FRESH_VV("g_chatwin", FRCMD_LBUTTONUP, FrOptionDlg::gOnTransChatWinBtnUp)
ON_FRESH_VI("g_suieffect", FRCMD_INIT, FrOptionDlg::gOnUiEffectStaticInit)
ON_FRESH_VI("g_uieffect", FRCMD_INIT, FrOptionDlg::gOnUiEffectBtnInit)
ON_FRESH_VV("g_uieffect", FRCMD_LBUTTONUP, FrOptionDlg::gOnUiEffectBtnUp)
ON_FRESH_VI("g_snick", FRCMD_INIT, FrOptionDlg::gOnNickStaticInit)
ON_FRESH_VI("g_nick", FRCMD_INIT, FrOptionDlg::gOnNickEditInit)
ON_FRESH_BI("g_nick", FRCMD_ENTERKEY, FrOptionDlg::gOnNickEditEnterKey)
ON_FRESH_VI("g_changenick", FRCMD_INIT, FrOptionDlg::gOnChangeNickBtnInit)
ON_FRESH_VV("g_changenick", FRCMD_LBUTTONUP, FrOptionDlg::gOnChangeNickBtnUp)
ON_FRESH_VI("g_sschool", FRCMD_INIT, FrOptionDlg::gOnSchoolStaticInit)
ON_FRESH_VI("g_school", FRCMD_INIT, FrOptionDlg::gOnSchoolEditInit)
ON_FRESH_BI("g_school", FRCMD_ENTERKEY, FrOptionDlg::gOnSchoolEditEnterKey)
ON_FRESH_VI("g_changeschool", FRCMD_INIT, FrOptionDlg::gOnChangeSchoolBtnInit)
ON_FRESH_VV("g_changeschool", FRCMD_LBUTTONUP,
	FrOptionDlg::gOnChangeSchoolBtnUp)
ON_FRESH_VI("g_spowergauge", FRCMD_INIT, FrOptionDlg::gOnPowerGaugeStaticInit)
ON_FRESH_VI("g_powergauge", FRCMD_INIT, FrOptionDlg::gOnPowerGaugeBtnInit)
ON_FRESH_VV("g_powergauge", FRCMD_LBUTTONUP, FrOptionDlg::gOnPowerGaugeBtnUp)
ON_FRESH_VI("g_shidegui", FRCMD_INIT, FrOptionDlg::gOnHideGUIStaticInit)
ON_FRESH_VI("g_hidegui", FRCMD_INIT, FrOptionDlg::gOnHideGUIBtnInit)
ON_FRESH_VV("g_hidegui", FRCMD_LBUTTONUP, FrOptionDlg::gOnHideGUIBtnUp)
ON_FRESH_VI("g_shidepi", FRCMD_INIT, FrOptionDlg::gOnHidePIStaticInit)
ON_FRESH_VI("g_hidepi", FRCMD_INIT, FrOptionDlg::gOnHidePIBtnInit)
ON_FRESH_VV("g_hidepi", FRCMD_LBUTTONUP, FrOptionDlg::gOnHidePIBtnUp)
ON_FRESH_VI("g_stooltip", FRCMD_INIT, FrOptionDlg::gOnTerrainTooltipStaticInit)
ON_FRESH_VI("g_btntooltip", FRCMD_INIT, FrOptionDlg::gOnTerrainBtnInit)
ON_FRESH_VV("g_btntooltip", FRCMD_LBUTTONUP, FrOptionDlg::gOnTerrainBtnUp)
ON_FRESH_VI("g_cutin", FRCMD_INIT, FrOptionDlg::gOnCutinDisplayStaticInit)
ON_FRESH_VI("g_btncutin", FRCMD_INIT, FrOptionDlg::gOnCutinDisplayBtnInit)
ON_FRESH_VV("g_btncutin", FRCMD_LBUTTONUP, FrOptionDlg::gOnCutinDisplayBtnUp)
ON_FRESH_VI("v_sfullscr", FRCMD_INIT, FrOptionDlg::vOnFullScreenStaticInit)
ON_FRESH_VI("v_fullscr", FRCMD_INIT, FrOptionDlg::vOnFullScreenBtnInit)
ON_FRESH_VV("v_fullscr", FRCMD_LBUTTONUP, FrOptionDlg::vOnFullScreenBtnUp)
ON_FRESH_VI("v_sfillscr", FRCMD_INIT, FrOptionDlg::vOnFillScreenStaticInit)
ON_FRESH_VI("v_fillscr", FRCMD_INIT, FrOptionDlg::vOnFillScreenBtnInit)
ON_FRESH_VV("v_fillscr", FRCMD_LBUTTONUP, FrOptionDlg::vOnFillScreenBtnUp)
ON_FRESH_VI("v_swidemode", FRCMD_INIT, FrOptionDlg::vOnWideModeStaticInit)
ON_FRESH_VI("v_widemode", FRCMD_INIT, FrOptionDlg::vOnWideModeBtnInit)
ON_FRESH_VV("v_widemode", FRCMD_LBUTTONUP, FrOptionDlg::vOnWideModeBtnUp)
ON_FRESH_VI("v_soverall", FRCMD_INIT, FrOptionDlg::vOnOverallStaticInit)
ON_FRESH_VI("v_overall", FRCMD_INIT, FrOptionDlg::vOnOverallCmbInit)
ON_FRESH_VI("v_sres", FRCMD_INIT, FrOptionDlg::vOnScrResStaticInit)
ON_FRESH_VI("v_res", FRCMD_INIT, FrOptionDlg::vOnScrResCmbInit)
ON_FRESH_VI("v_stnl", FRCMD_INIT, FrOptionDlg::vOnTnlStaticInit)
ON_FRESH_VI("v_tnl", FRCMD_INIT, FrOptionDlg::vOnTnlCmbInit)
ON_FRESH_VI("v_stex", FRCMD_INIT, FrOptionDlg::vOnTexResStaticInit)
ON_FRESH_VI("v_tex", FRCMD_INIT, FrOptionDlg::vOnTexResCmbInit)
ON_FRESH_VI("v_sshadow", FRCMD_INIT, FrOptionDlg::vOnShadowStaticInit)
ON_FRESH_VI("v_shadow", FRCMD_INIT, FrOptionDlg::vOnShadowBtnInit)
ON_FRESH_VV("v_shadow", FRCMD_LBUTTONUP, FrOptionDlg::vOnShadowBtnUp)
ON_FRESH_VI("v_ssdwmap", FRCMD_INIT, FrOptionDlg::vOnShadowmapStaticInit)
ON_FRESH_VI("v_sdwmap", FRCMD_INIT, FrOptionDlg::vOnShadowmapBtnInit)
ON_FRESH_VV("v_sdwmap", FRCMD_LBUTTONUP, FrOptionDlg::vOnShadowmapBtnUp)
ON_FRESH_VI("v_srefl", FRCMD_INIT, FrOptionDlg::vOnReflectionStaticInit)
ON_FRESH_VI("v_refl", FRCMD_INIT, FrOptionDlg::vOnReflectionBtnInit)
ON_FRESH_VV("v_refl", FRCMD_LBUTTONUP, FrOptionDlg::vOnReflectionBtnUp)
ON_FRESH_VI("v_slod", FRCMD_INIT, FrOptionDlg::vOnLodStaticInit)
ON_FRESH_VI("v_lod", FRCMD_INIT, FrOptionDlg::vOnLodBtnInit)
ON_FRESH_VV("v_lod", FRCMD_LBUTTONUP, FrOptionDlg::vOnLodBtnUp)
ON_FRESH_VI("v_snpc", FRCMD_INIT, FrOptionDlg::vOnNpcStaticInit)
ON_FRESH_VI("v_npc", FRCMD_INIT, FrOptionDlg::vOnNpcBtnInit)
ON_FRESH_VV("v_npc", FRCMD_LBUTTONUP, FrOptionDlg::vOnNpcBtnUp)
ON_FRESH_VI("v_sparticle", FRCMD_INIT, FrOptionDlg::vOnParticleStaticInit)
ON_FRESH_VI("v_particle", FRCMD_INIT, FrOptionDlg::vOnParticleCmbInit)
ON_FRESH_VI("a_senable", FRCMD_INIT, FrOptionDlg::aOnEnableStaticInit)
ON_FRESH_VI("a_enable", FRCMD_INIT, FrOptionDlg::aOnEnableBtnInit)
ON_FRESH_VV("a_enable", FRCMD_LBUTTONUP, FrOptionDlg::aOnEnableBtnUp)
ON_FRESH_VI("a_soverall", FRCMD_INIT, FrOptionDlg::aOnOverallStaticInit)
ON_FRESH_VI("a_overall", FRCMD_INIT, FrOptionDlg::aOnOverallCmbInit)
ON_FRESH_VI("a_ssfx", FRCMD_INIT, FrOptionDlg::aOnSfxStaticInit)
ON_FRESH_VI("a_sfx", FRCMD_INIT, FrOptionDlg::aOnSfxGaugeInit)
ON_FRESH_VI("a_sbgm", FRCMD_INIT, FrOptionDlg::aOnBgmStaticInit)
ON_FRESH_VI("a_bgm", FRCMD_INIT, FrOptionDlg::aOnBgmGaugeInit)
ON_FRESH_VI("a_sspeaker", FRCMD_INIT, FrOptionDlg::aOnSpeakerStaticInit)
ON_FRESH_VI("a_speaker", FRCMD_INIT, FrOptionDlg::aOnSpeakerCmbInit)
ON_FRESH_VI("m_smacro1", FRCMD_INIT, FrOptionDlg::mOnMacroStatic1Init)
ON_FRESH_VI("m_macro1", FRCMD_INIT, FrOptionDlg::mOnMacro1Init)
ON_FRESH_VV("m_macro1", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro1", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro2", FRCMD_INIT, FrOptionDlg::mOnMacroStatic2Init)
ON_FRESH_VI("m_macro2", FRCMD_INIT, FrOptionDlg::mOnMacro2Init)
ON_FRESH_VV("m_macro2", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro2", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro3", FRCMD_INIT, FrOptionDlg::mOnMacroStatic3Init)
ON_FRESH_VI("m_macro3", FRCMD_INIT, FrOptionDlg::mOnMacro3Init)
ON_FRESH_VV("m_macro3", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro3", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro4", FRCMD_INIT, FrOptionDlg::mOnMacroStatic4Init)
ON_FRESH_VI("m_macro4", FRCMD_INIT, FrOptionDlg::mOnMacro4Init)
ON_FRESH_VV("m_macro4", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro4", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro5", FRCMD_INIT, FrOptionDlg::mOnMacroStatic5Init)
ON_FRESH_VI("m_macro5", FRCMD_INIT, FrOptionDlg::mOnMacro5Init)
ON_FRESH_VV("m_macro5", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro5", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro6", FRCMD_INIT, FrOptionDlg::mOnMacroStatic6Init)
ON_FRESH_VI("m_macro6", FRCMD_INIT, FrOptionDlg::mOnMacro6Init)
ON_FRESH_VV("m_macro6", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro6", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro7", FRCMD_INIT, FrOptionDlg::mOnMacroStatic7Init)
ON_FRESH_VI("m_macro7", FRCMD_INIT, FrOptionDlg::mOnMacro7Init)
ON_FRESH_VV("m_macro7", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro7", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("m_smacro8", FRCMD_INIT, FrOptionDlg::mOnMacroStatic8Init)
ON_FRESH_VI("m_macro8", FRCMD_INIT, FrOptionDlg::mOnMacro8Init)
ON_FRESH_VV("m_macro8", FRCMD_LBUTTONDOWN,
	FrOptionDlg::mOnMacroEnterKeyLBtnDown)
ON_FRESH_BI("m_macro8", FRCMD_ENTERKEY, FrOptionDlg::mOnMacroEnterKey)
ON_FRESH_VI("t_hint", FRCMD_INIT, FrOptionDlg::tOnHintInit)
ON_FRESH_VI("", FRCMD_HOVERON, FrOptionDlg::OnHoverOn)
ON_FRESH_VI("", FRCMD_HOVEROFF, FrOptionDlg::OnHoverOff)
END_FRESH_MSGMAP()

FrOptionDlg::FrOptionDlg()
	: m_bApplyScrSize(false)
{
	m_option = *COption::Instance()->GetOption();
	m_bApply = false;
	m_bOK = false;
	m_pOkBtn = NULL;
	m_pTab[TAB_GAME] = &m_gameTab;
	m_pTab[TAB_VIDEO] = &m_videoTab;
	m_pTab[TAB_AUDIO] = &m_audioTab;
	m_pTab[TAB_MACRO] = &m_macroTab;
	m_pChangeNickDlg = NULL;
	m_pSchoolDlg = NULL;
	m_pHint = NULL;
	m_hoverCount = 0;
	g_pFresh->GetManager()->SetExclusiveKey(true);
}

FrOptionDlg::~FrOptionDlg()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) && !CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);
}

void FrOptionDlg::SetApplyScrSize(bool bApply)
{
	m_bApplyScrSize = bApply;
}

void FrOptionDlg::OpenTab(eTabType tab)
{
	if (tab >= TAB_GAME && tab < TAB_NUM)
		SetTab(tab);
}

void FrOptionDlg::OnOkBtnInit(int param)
{
	m_pOkBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::OnGameTabInit(int param)
{
	m_pTab[TAB_GAME]->m_pTabBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTab[TAB_GAME]->Init();
}

void FrOptionDlg::OnGameTabUp()
{
	SetTab(TAB_GAME);
}

void FrOptionDlg::OnVideoTabInit(int param)
{
	m_pTab[TAB_VIDEO]->m_pTabBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTab[TAB_VIDEO]->Init();
}

void FrOptionDlg::OnVideoTabUp()
{
	SetTab(TAB_VIDEO);
}

void FrOptionDlg::OnAudioTabInit(int param)
{
	m_pTab[TAB_AUDIO]->m_pTabBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTab[TAB_AUDIO]->Init();
}

void FrOptionDlg::OnAudioTabUp()
{
	SetTab(TAB_AUDIO);
}

void FrOptionDlg::OnMacroTabInit(int param)
{
	m_pTab[TAB_MACRO]->m_pTabBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTab[TAB_MACRO]->Init();
}

void FrOptionDlg::OnMacroTabUp()
{
	SetTab(TAB_MACRO);
}

void FrOptionDlg::gOnMsSensiStaticInit(int param)
{
	m_gameTab.m_pStatic[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnMsSensiGaugeInit(int param)
{
	m_gameTab.m_pMsSensiGauge = DYNAMIC_CAST(FrGaugeBarEx, (FrWnd*)param);
}

void FrOptionDlg::gOnWhisperStaticInit(int param)
{
	m_gameTab.m_pStatic[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnWhisperBtnInit(int param)
{
	m_gameTab.m_pWhisperBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnWhisperBtnUp()
{
	m_option.gWhisper =
		m_gameTab.m_pWhisperBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnInvitationStaticInit(int param)
{
	m_gameTab.m_pStatic[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnInvitationBtnInit(int param)
{
	m_gameTab.m_pInvitationBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnInvitationBtnUp()
{
	m_option.gInvitation =
		m_gameTab.m_pInvitationBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnFriendConfirmStaticInit(int param)
{
	m_gameTab.m_pStatic[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnFriendConfirmBtnInit(int param)
{
	m_gameTab.m_pFriendConfirmBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnFriendConfirmBtnUp()
{
	m_option.gFriendConfirm =
		m_gameTab.m_pFriendConfirmBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnTransChatWinStaticInit(int param)
{
	m_gameTab.m_pStatic[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnTransChatWinBtnInit(int param)
{
	m_gameTab.m_pTransChatWinBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnTransChatWinBtnUp()
{
	m_option.gTransChatWin =
		m_gameTab.m_pTransChatWinBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnUiEffectStaticInit(int param)
{
	m_gameTab.m_pStatic[5] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnTerrainTooltipStaticInit(int param)
{
	m_gameTab.m_pStatic[11] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnUiEffectBtnInit(int param)
{
	m_gameTab.m_pUiEffectBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnUiEffectBtnUp()
{
	m_option.gUiEffect =
		m_gameTab.m_pUiEffectBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnNickStaticInit(int param)
{
	m_gameTab.m_pStatic[6] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnNickEditInit(int param)
{
	m_gameTab.m_pNickEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_gameTab.m_pNickEdit->HidePrivacy(true);
	if (!m_gameTab.m_bEnableChangeNick)
	{
		m_gameTab.m_pNickEdit->SetReadOnly(true);
		m_gameTab.m_pNickEdit->SetFontColor(0xff888888);
	}
}

void FrOptionDlg::gOnChangeNickBtnInit(int param)
{
	m_gameTab.m_pChangeNickBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnChangeNickBtnUp()
{
	char allowed = 0;
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	MsgObject msg(NULL, 621, 16, (int)&allowed, 0, 0, 0);
	actor << msg;
	if (!allowed)
	{
		msg.sender = NULL;
		msg.message = 621;
		msg.param1 = 17;
		msg.param2 = 0;
		msg.param3 = 0;
		msg.param4 = 0;
		msg.param5 = 0;
		msg.time = 0;
		actor << msg;
		return;
	}
	if (!m_gameTab.m_bEnableChangeNick || !m_gameTab.m_pNickEdit ||
		m_pChangeNickDlg)
		return;
	if (m_bApply)
		m_bOK = false;
	char nick[24];
	strcpy(nick, m_gameTab.m_pNickEdit->GetLine(1, false));
	const char* error = Doc()->m_chatManager.FilteringNick(nick);
	if (error)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		if (form)
		{
			form->SetMessage(error, false);
			form->Open(NULL, 3);
		}
		m_bApply = false;
		return;
	}
	const char* text = m_gameTab.m_pNickEdit->GetLine(1, false);
	cTokenV token;
	token.Init(text, strlen(text));
	int count = token.GetTokenNum(0, " '", 2) - 1;
	if (count > 0)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		if (form)
		{
			form->SetMessage(
				"\x5b\x27\x5d\xb4\xc2\x20\xb4\xeb\xc8\xad\xb8\xed\xbf\xa1\x20\xbb\xe7\xbf\xeb\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e",
				false);
			form->Open(NULL, 3);
		}
	}
	else if (strcmpi(Doc()->m_myInfo.info.sNick, text))
	{
		m_pChangeNickDlg = CreateForm<FrChangeNickDlg>(g_pFresh->GetManager(),
			this, "changenick");
		if (m_pChangeNickDlg)
		{
			m_pChangeNickDlg->SetOptionDlg(this);
			m_pChangeNickDlg->SetNick(m_gameTab.m_pNickEdit->GetLine(1, false));
			m_pChangeNickDlg->SetCloseResult(FrNONE);
			m_pChangeNickDlg->Open(
				(FRESH_PFN_RESULT)&FrOptionDlg::OnChangeNickConfirmDlgResult,
				0);
		}
		EnableChangeNickControls(false);
	}
	else if (m_bApply)
		CloseDlg();
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		if (form)
		{
			form->SetMessage(
				"\xbb\xf5\xb7\xce\xbf\xee\x20\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\xc0\xd4\xb7\xc2\xc7\xd8\x20\xc1\xd6\xbc\xbc\xbf\xe4",
				false);
			form->Open(NULL, 3);
		}
	}
}

bool FrOptionDlg::gOnNickEditEnterKey(int param)
{
	return false;
}

void FrOptionDlg::gOnSchoolStaticInit(int param)
{
	m_gameTab.m_pStatic[7] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnSchoolEditInit(int param)
{
	m_gameTab.m_pSchoolEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_gameTab.m_pSchoolEdit->SetReadOnly(true);
}

void FrOptionDlg::gOnChangeSchoolBtnInit(int param)
{
	m_gameTab.m_pChangeSchoolBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (IS_KINDOF(CLobbyTask, AfxGetTask()) &&
		WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		m_gameTab.m_pChangeSchoolBtn->Enable(true);
	else
		m_gameTab.m_pChangeSchoolBtn->Enable(false);
}

void FrOptionDlg::gOnChangeSchoolBtnUp()
{
	if (m_gameTab.m_pSchoolEdit && !m_pSchoolDlg)
	{
		m_pSchoolDlg = CreateForm<FrSchoolDlg>(g_pFresh->GetManager(), this,
			"search_school");
		m_pSchoolDlg->Open(
			(FRESH_PFN_RESULT)&FrOptionDlg::OnChangeSchoolDlgResult, 1);
	}
}

bool FrOptionDlg::gOnSchoolEditEnterKey(int param)
{
	return false;
}

void FrOptionDlg::gOnPowerGaugeStaticInit(int param)
{
	m_gameTab.m_pStatic[8] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnPowerGaugeBtnInit(int param)
{
	m_gameTab.m_pPowerGaugeBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnPowerGaugeBtnUp()
{
	m_option.gPowerGauge =
		m_gameTab.m_pPowerGaugeBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnHideGUIStaticInit(int param)
{
	m_gameTab.m_pStatic[9] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnHideGUIBtnInit(int param)
{
	m_gameTab.m_pHideGUIBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnHideGUIBtnUp()
{
	m_option.gCaptureHideGUI =
		m_gameTab.m_pHideGUIBtn->GetStatus() == FrButton::PRESSED;
	CProjectG::Instance()->ResetCaptuerNum();
}

void FrOptionDlg::gOnHidePIStaticInit(int param)
{
	m_gameTab.m_pStatic[10] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnHidePIBtnInit(int param)
{
	m_gameTab.m_pHidePIBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnTerrainBtnInit(int param)
{
	m_gameTab.m_pTerrainBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (COption::Instance()->gGetTerrainTooltip())
		m_gameTab.m_pTerrainBtn->SetStatus(FrButton::PRESSED);
}

void FrOptionDlg::gOnTerrainBtnUp()
{
}

void FrOptionDlg::gOnCutinDisplayStaticInit(int param)
{
	m_gameTab.m_pStatic[12] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::gOnCutinDisplayBtnInit(int param)
{
	m_gameTab.m_pCutinDisplayBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::gOnCutinDisplayBtnUp()
{
	m_option.gCutinDisplay =
		m_gameTab.m_pCutinDisplayBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::gOnHidePIBtnUp()
{
	m_option.gCaptureHidePI =
		m_gameTab.m_pHidePIBtn->GetStatus() == FrButton::PRESSED;
	CProjectG::Instance()->ResetCaptuerNum();
}

void FrOptionDlg::vOnFullScreenStaticInit(int param)
{
	m_videoTab.m_pStatic[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnFullScreenBtnInit(int param)
{
	m_videoTab.m_pFullScreenBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::vOnFullScreenBtnUp()
{
	m_option.vWindowMode =
		m_videoTab.m_pFullScreenBtn->GetStatus() != FrButton::PRESSED;
	char text[16];
	strcpy(text,
		COption::Instance()->vGetScrResText(m_option.vScreenWidth,
			m_option.vScreenHeight, m_option.vScreenColor,
			m_option.vWindowMode != 0));
	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(),
		m_option.vWindowMode != 0, m_option.vWideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();
	bool first = true;
	for (int i = 0; i < COption::Instance()->vGetScrResNum(); ++i)
	{
		const char* mode = COption::Instance()->vGetScrResText(i);
		if (strlen(mode))
		{
			m_videoTab.m_pScrResCmb->AddString(mode);
			if (first)
			{
				first = false;
				m_videoTab.m_pScrResCmb->SetLine(1, mode, 0, false, 0);
			}
			if (!strcmp(mode, text))
				m_videoTab.m_pScrResCmb->SetLine(1, mode, 0, false, 0);
		}
	}
}

void FrOptionDlg::vOnFillScreenStaticInit(int param)
{
	m_videoTab.m_pStatic[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnFillScreenBtnInit(int param)
{
	m_videoTab.m_pFillScreenBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::vOnFillScreenBtnUp()
{
	if (!m_videoTab.m_pFillScreenBtn ||
		m_videoTab.m_pFillScreenBtn->IsEnabled())
		m_option.vFillMode =
			m_videoTab.m_pFillScreenBtn->GetStatus() == FrButton::PRESSED;
}

void FrOptionDlg::vOnWideModeStaticInit(int param)
{
	m_videoTab.m_pStatic[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnWideModeBtnInit(int param)
{
	m_videoTab.m_pWideModeBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::vOnWideModeBtnUp()
{
	m_option.vWideMode =
		m_videoTab.m_pWideModeBtn->GetStatus() == FrButton::PRESSED;
	char text[16];
	strcpy(text,
		COption::Instance()->vGetScrResText(m_option.vScreenWidth,
			m_option.vScreenHeight, m_option.vScreenColor,
			m_option.vWindowMode != 0));
	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(),
		m_option.vWindowMode != 0, m_option.vWideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();
	bool first = true;
	for (int i = 0; i < COption::Instance()->vGetScrResNum(); ++i)
	{
		const char* mode = COption::Instance()->vGetScrResText(i);
		if (strlen(mode))
		{
			m_videoTab.m_pScrResCmb->AddString(mode);
			if (first)
			{
				first = false;
				m_videoTab.m_pScrResCmb->SetLine(1, mode, 0, false, 0);
			}
			if (!strcmp(mode, text))
				m_videoTab.m_pScrResCmb->SetLine(1, mode, 0, false, 0);
		}
	}
}

void FrOptionDlg::vOnOverallStaticInit(int param)
{
	m_videoTab.m_pStatic[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnOverallCmbInit(int param)
{
	m_videoTab.m_pOverallCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::vOnScrResStaticInit(int param)
{
	m_videoTab.m_pStatic[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnScrResCmbInit(int param)
{
	m_videoTab.m_pScrResCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::vOnTnlStaticInit(int param)
{
	m_videoTab.m_pStatic[5] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnTnlCmbInit(int param)
{
	m_videoTab.m_pTnlCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::vOnTexResStaticInit(int param)
{
	m_videoTab.m_pStatic[6] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnTexResCmbInit(int param)
{
	m_videoTab.m_pTexResCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::vOnShadowStaticInit(int param)
{
	m_videoTab.m_pStatic[7] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (!sOption::IsSupportShadow())
		m_videoTab.m_pStatic[7]->SetTextColor(0xff808080, -1);
}

void FrOptionDlg::vOnShadowBtnInit(int param)
{
	m_videoTab.m_pShadowBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!sOption::IsSupportShadow())
		m_videoTab.m_pShadowBtn->Enable(false);
}

void FrOptionDlg::vOnShadowBtnUp()
{
	m_option.vProjectionShadow =
		m_videoTab.m_pShadowBtn->GetStatus() == FrButton::PRESSED;
	m_option.vOverall = -1;
	m_videoTab.m_pOverallCmb->SetLine(1,
		m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
}

void FrOptionDlg::vOnShadowmapStaticInit(int param)
{
	m_videoTab.m_pStatic[8] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnShadowmapBtnInit(int param)
{
	m_videoTab.m_pShadowmapBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::vOnShadowmapBtnUp()
{
	m_option.vShadowmap =
		m_videoTab.m_pShadowmapBtn->GetStatus() == FrButton::PRESSED;
	m_option.vOverall = -1;
	m_videoTab.m_pOverallCmb->SetLine(1,
		m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
}

void FrOptionDlg::vOnReflectionStaticInit(int param)
{
	m_videoTab.m_pStatic[9] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (!sOption::IsSupportReflection())
		m_videoTab.m_pStatic[9]->SetTextColor(0xff808080, -1);
}

void FrOptionDlg::vOnReflectionBtnInit(int param)
{
	m_videoTab.m_pReflectionBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!sOption::IsSupportReflection())
		m_videoTab.m_pReflectionBtn->Enable(false);
}

void FrOptionDlg::vOnReflectionBtnUp()
{
	m_option.vReflection =
		m_videoTab.m_pReflectionBtn->GetStatus() == FrButton::PRESSED;
	m_option.vOverall = -1;
	m_videoTab.m_pOverallCmb->SetLine(1,
		m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
}

void FrOptionDlg::vOnLodStaticInit(int param)
{
	m_videoTab.m_pStatic[10] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (!sOption::IsSupportLod())
		m_videoTab.m_pStatic[10]->SetTextColor(0xff808080, -1);
}

void FrOptionDlg::vOnLodBtnInit(int param)
{
	m_videoTab.m_pLodBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!sOption::IsSupportLod())
		m_videoTab.m_pLodBtn->Enable(false);
}

void FrOptionDlg::vOnLodBtnUp()
{
	m_option.vLod = m_videoTab.m_pLodBtn->GetStatus() == FrButton::PRESSED;
	m_option.vOverall = -1;
	m_videoTab.m_pOverallCmb->SetLine(1,
		m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
}

void FrOptionDlg::vOnNpcStaticInit(int param)
{
	m_videoTab.m_pStatic[11] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnNpcBtnInit(int param)
{
	m_videoTab.m_pNpcBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::vOnNpcBtnUp()
{
	m_option.vNpc = m_videoTab.m_pNpcBtn->GetStatus() == FrButton::PRESSED;
	m_option.vOverall = -1;
	m_videoTab.m_pOverallCmb->SetLine(1,
		m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
}

void FrOptionDlg::vOnParticleStaticInit(int param)
{
	m_videoTab.m_pStatic[12] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::vOnParticleCmbInit(int param)
{
	m_videoTab.m_pParticleCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::aOnEnableStaticInit(int param)
{
	m_audioTab.m_pStatic[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::aOnEnableBtnInit(int param)
{
	m_audioTab.m_pEnableBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOptionDlg::aOnEnableBtnUp()
{
	if (m_audioTab.m_pEnableBtn->GetStatus() == FrButton::PRESSED)
	{
		m_option.aMssOn = 1;
		g_audio->SetVolume(m_option.aSfxVolume);
		g_audio->SetBGMVolume(m_option.aBGMVolume);
	}
	else
	{
		m_option.aMssOn = 0;
		g_audio->SetVolume(0.0f);
		g_audio->SetBGMVolume(0.0f);
	}
}

void FrOptionDlg::aOnOverallStaticInit(int param)
{
	m_audioTab.m_pStatic[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::aOnOverallCmbInit(int param)
{
	m_audioTab.m_pOverallCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::aOnSfxStaticInit(int param)
{
	m_audioTab.m_pStatic[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::aOnSfxGaugeInit(int param)
{
	m_audioTab.m_pSfxGauge = DYNAMIC_CAST(FrGaugeBarEx, (FrWnd*)param);
}

void FrOptionDlg::aOnBgmStaticInit(int param)
{
	m_audioTab.m_pStatic[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::aOnBgmGaugeInit(int param)
{
	m_audioTab.m_pBgmGauge = DYNAMIC_CAST(FrGaugeBarEx, (FrWnd*)param);
}

void FrOptionDlg::aOnSpeakerStaticInit(int param)
{
	m_audioTab.m_pStatic[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::aOnSpeakerCmbInit(int param)
{
	m_audioTab.m_pSpeakerCmb = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic1Init(int param)
{
	m_macroTab.m_pStatic[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic2Init(int param)
{
	m_macroTab.m_pStatic[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

bool FrOptionDlg::mOnMacroEnterKey(int param)
{
	OnOK();
	Close(FrOK, true);
	return false;
}

void FrOptionDlg::mOnMacroEnterKeyLBtnDown()
{
	OnLButtonDown(g_pFresh->GetManager()->GetMousePos());
}

void FrOptionDlg::mOnMacroStatic3Init(int param)
{
	m_macroTab.m_pStatic[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic4Init(int param)
{
	m_macroTab.m_pStatic[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic5Init(int param)
{
	m_macroTab.m_pStatic[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic6Init(int param)
{
	m_macroTab.m_pStatic[5] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic7Init(int param)
{
	m_macroTab.m_pStatic[6] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacroStatic8Init(int param)
{
	m_macroTab.m_pStatic[7] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro1Init(int param)
{
	m_macroTab.m_pMacro[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro2Init(int param)
{
	m_macroTab.m_pMacro[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro3Init(int param)
{
	m_macroTab.m_pMacro[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro4Init(int param)
{
	m_macroTab.m_pMacro[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro5Init(int param)
{
	m_macroTab.m_pMacro[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro6Init(int param)
{
	m_macroTab.m_pMacro[5] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro7Init(int param)
{
	m_macroTab.m_pMacro[6] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrOptionDlg::mOnMacro8Init(int param)
{
	m_macroTab.m_pMacro[7] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrOptionDlg::OnInit()
{
	SetGameColtrols();
	InitVideoControls();
	SetVideoColtrols(true);
	SetAudioColtrols();
	SetMacroColtrols();
	eTabType tab = ms_eCurTab;
	ms_eCurTab = TAB_NUM;
	SetTab(tab);
	m_pTab[ms_eCurTab]->m_pTabBtn->SetStatus(FrButton::PRESSED);
	return FrForm::OnInit();
}

bool FrOptionDlg::OnLButtonDown(const WPoint& pt)
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) &&
		g_input->GetButton(LEFT_BUTTON) == 1)
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
		CMessengerInfo::Instance()->SetGameFocus(false);
	}
	return FrForm::OnLButtonDown(pt);
}

void FrOptionDlg::OnProc(const float delta)
{
	if (g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) &&
		!g_input->Get("RALT", true))
	{
		switch (ms_eCurTab)
		{
		case TAB_GAME:
			SetTab(TAB_VIDEO);
			break;
		case TAB_VIDEO:
			SetTab(TAB_AUDIO);
			break;
		case TAB_AUDIO:
			SetTab(TAB_MACRO);
			break;
		case TAB_MACRO:
			SetTab(TAB_GAME);
			break;
		}
	}
	if (m_gameTab.m_pTabBtn && m_gameTab.m_pTabBtn->IsEnabled() &&
		m_gameTab.m_pMsSensiGauge)
	{
		float sensitivity = m_gameTab.m_pMsSensiGauge->GetPos();
		if (sensitivity > 50.0f)
			sensitivity *= 0.02f;
		else
			sensitivity = sensitivity * 0.01f + 0.5f;
		if (!WisEqual(m_option.gMouseSensitivity, sensitivity, g_EPSILON))
		{
			m_option.gMouseSensitivity = sensitivity;
			g_input->SetSensitivity(sensitivity);
		}
	}
	if (m_videoTab.m_pTabBtn && m_videoTab.m_pTabBtn->IsEnabled())
	{
		bool overall = false;
		bool changed = false;
		bool custom = false;
		if (m_videoTab.m_pOverallCmb)
		{
			const char* oldText = m_option.vGetOverallText(m_option.vOverall);
			const char* text = m_videoTab.m_pOverallCmb->GetLine(1, false);
			if (strcmp(oldText, text))
			{
				m_option.vResetByOverall(text);
				overall = true;
			}
		}
		if (!overall && m_videoTab.m_pScrResCmb)
		{
			const char* oldText = COption::Instance()->vGetScrResText(
				m_option.vScreenWidth, m_option.vScreenHeight,
				m_option.vScreenColor, m_option.vWindowMode != 0);
			const char* text = m_videoTab.m_pScrResCmb->GetLine(1, false);
			if (strcmp(oldText, text))
			{
				COption::Instance()->vGetScrResLvl(text, &m_option.vScreenWidth,
					&m_option.vScreenHeight, &m_option.vScreenColor);
				changed = true;
				custom = true;
			}
		}
		if (m_videoTab.m_pFillScreenBtn)
		{
			if (!m_option.vWindowMode)
			{
				m_videoTab.m_pFillScreenBtn->Enable(false);
			}
			else
			{
				m_videoTab.m_pFillScreenBtn->Enable(true);
				if (m_option.vFillMode)
					m_gameTab.m_pPowerGaugeBtn->Enable(false);
				else
					m_gameTab.m_pPowerGaugeBtn->Enable(true);
			}
		}
		if (!overall)
		{
			if (m_videoTab.m_pTexResCmb)
			{
				const char* oldText = m_option.vGetTexResText(m_option.vDDSRes);
				const char* text = m_videoTab.m_pTexResCmb->GetLine(1, false);
				if (strcmp(oldText, text))
				{
					m_option.vDDSRes = m_option.vGetTexResLvl(text);
					changed = true;
					custom = true;
				}
			}
			if (m_videoTab.m_pParticleCmb)
			{
				const char* oldText =
					m_option.vGetParticleText(m_option.vEffectLevel);
				const char* text = m_videoTab.m_pParticleCmb->GetLine(1, false);
				if (strcmp(oldText, text))
				{
					m_option.vEffectLevel = m_option.vGetParticleLvl(text);
					changed = true;
					custom = true;
				}
			}
		}
		if (overall)
		{
			SetVideoColtrols(false);
		}
		else if (changed)
		{
			SetVideoColtrols(false);
			if (custom)
			{
				m_option.vOverall = -1;
				m_videoTab.m_pOverallCmb->SetLine(1,
					m_option.vGetOverallText(m_option.vOverall), 0, false, 0);
			}
		}
	}
	if (m_audioTab.m_pTabBtn && m_audioTab.m_pTabBtn->IsEnabled())
	{
		if (m_audioTab.m_pSfxGauge)
		{
			float volume = m_audioTab.m_pSfxGauge->GetPos() * 0.01f;
			if (!WisEqual(m_option.aSfxVolume, volume, g_EPSILON))
			{
				m_option.aSfxVolume = volume;
				if (m_option.aMssOn == 1)
					COption::Instance()->aApplySfxVolume(volume);
			}
		}
		if (m_audioTab.m_pBgmGauge)
		{
			float volume = m_audioTab.m_pBgmGauge->GetPos() * 0.01f;
			if (!WisEqual(m_option.aBGMVolume, volume, g_EPSILON))
			{
				m_option.aBGMVolume = volume;
				if (m_option.aMssOn == 1)
					COption::Instance()->aApplyBgmVolume(volume);
			}
		}
		if (m_audioTab.m_pSpeakerCmb)
		{
			const char* oldText =
				m_option.aGetSpeakerText(m_option.aMssSpeaker);
			const char* text = m_audioTab.m_pSpeakerCmb->GetLine(1, false);
			if (strcmp(oldText, text))
				m_option.aMssSpeaker = m_option.aGetSpeakerLvl(text);
		}
	}
}

void FrOptionDlg::SetGameColtrols()
{
	float sensitivity = m_option.gMouseSensitivity;
	if (sensitivity > 1.0f)
		sensitivity *= 50.0f;
	else
		sensitivity = (sensitivity - 0.5f) * 100.0f;
	if (sensitivity < 0.0f)
		sensitivity = 0.0f;
	else if (sensitivity > 100.0f)
		sensitivity = 100.0f;
	m_gameTab.m_pMsSensiGauge->SetPos((int)sensitivity);
	if (m_option.gWhisper && m_gameTab.m_pWhisperBtn)
		m_gameTab.m_pWhisperBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gInvitation && m_gameTab.m_pInvitationBtn)
		m_gameTab.m_pInvitationBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gFriendConfirm && m_gameTab.m_pFriendConfirmBtn)
		m_gameTab.m_pFriendConfirmBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gTransChatWin && m_gameTab.m_pTransChatWinBtn)
		m_gameTab.m_pTransChatWinBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gUiEffect && m_gameTab.m_pUiEffectBtn)
		m_gameTab.m_pUiEffectBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gPowerGauge && m_gameTab.m_pPowerGaugeBtn)
		m_gameTab.m_pPowerGaugeBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gCaptureHideGUI && m_gameTab.m_pHideGUIBtn)
		m_gameTab.m_pHideGUIBtn->SetStatus(FrButton::PRESSED);
	if (m_option.gCaptureHidePI && m_gameTab.m_pHidePIBtn)
		m_gameTab.m_pHidePIBtn->SetStatus(FrButton::PRESSED);
	if (m_gameTab.m_pNickEdit && Doc()->m_myInfo.info.sNick[0])
		m_gameTab.m_pNickEdit->SetLine(1, Doc()->m_myInfo.info.sNick, 0, false,
			0);
	if (m_option.gCutinDisplay && m_gameTab.m_pCutinDisplayBtn)
		m_gameTab.m_pCutinDisplayBtn->SetStatus(FrButton::PRESSED);
	if (m_gameTab.m_pSchoolEdit && Doc()->m_myInfo.info.school)
	{
		char name[128];
		Doc()->LoadSchoolName(name, Doc()->m_myInfo.info.school);
		m_gameTab.m_pSchoolEdit->SetLine(1, name, 0, false, 0);
	}
}

void FrOptionDlg::InitVideoControls()
{
	for (int i = 0; i < 3; ++i)
		m_videoTab.m_pOverallCmb->AddString(m_option.vGetOverallText(i));
	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(),
		m_option.vWindowMode != 0, m_option.vWideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();
	for (int i = 0; i < COption::Instance()->vGetScrResNum(); ++i)
	{
		const char* text = COption::Instance()->vGetScrResText(i);
		if (strlen(text))
			m_videoTab.m_pScrResCmb->AddString(text);
	}
	if (sOption::sbHwTnlSupport)
		m_videoTab.m_pTnlCmb->AddString(m_option.vGetTnlText(1));
	for (int i = 0; i < 3; ++i)
		m_videoTab.m_pTexResCmb->AddString(m_option.vGetTexResText(i));
	for (int i = 0; i < 3; ++i)
		m_videoTab.m_pParticleCmb->AddString(m_option.vGetParticleText(i));
}

void FrOptionDlg::SetVideoColtrols(bool bReset)
{
	if (!m_option.vWindowMode)
		m_videoTab.m_pFullScreenBtn->SetStatus(FrButton::PRESSED);
	if (m_option.vFillMode)
		m_videoTab.m_pFillScreenBtn->SetStatus(FrButton::PRESSED);
	if (m_option.vWideMode)
		m_videoTab.m_pWideModeBtn->SetStatus(FrButton::PRESSED);
	if (bReset)
		m_option.vResetByOverall(m_option.vGetOverallText(m_option.vOverall));
	{
		const char* text = m_option.vGetOverallText(m_option.vOverall);
		m_videoTab.m_pOverallCmb->SetLine(1, text, 0, false, 0);
	}
	{
		const char* text = COption::Instance()->vGetScrResText(
			m_option.vScreenWidth, m_option.vScreenHeight,
			m_option.vScreenColor, m_option.vWindowMode != 0);
		m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);
	}
	{
		const char* text = m_option.vGetTnlText(m_option.vTnLMode);
		m_videoTab.m_pTnlCmb->SetLine(1, text, 0, false, 0);
	}
	{
		const char* text = m_option.vGetTexResText(m_option.vDDSRes);
		m_videoTab.m_pTexResCmb->SetLine(1, text, 0, false, 0);
	}
	if (sOption::IsSupportShadow())
		m_videoTab.m_pShadowBtn->SetStatus(m_option.vProjectionShadow == 1
				? FrButton::PRESSED
				: FrButton::NORMAL);
	m_videoTab.m_pShadowmapBtn->SetStatus(
		m_option.vShadowmap == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	if (sOption::IsSupportReflection())
		m_videoTab.m_pReflectionBtn->SetStatus(
			m_option.vReflection == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	if (sOption::IsSupportLod())
		m_videoTab.m_pLodBtn->SetStatus(
			m_option.vLod == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	m_videoTab.m_pNpcBtn->SetStatus(
		m_option.vNpc == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	{
		const char* text = m_option.vGetParticleText(m_option.vEffectLevel);
		m_videoTab.m_pParticleCmb->SetLine(1, text, 0, false, 0);
	}
}

void FrOptionDlg::SetAudioColtrols()
{
	COption* option = COption::Instance();
	if (m_option.aMssOn)
		m_audioTab.m_pEnableBtn->SetStatus(FrButton::PRESSED);
	for (int i = 0; i < 3; ++i)
		m_audioTab.m_pOverallCmb->AddString(m_option.aGetOverallText(i));
	for (int i = 0; i < 6; ++i)
		m_audioTab.m_pSpeakerCmb->AddString(m_option.aGetSpeakerText(i));
	{
		const char* text = m_option.aGetOverallText(m_option.aOverall);
		m_audioTab.m_pOverallCmb->SetLine(1, text, 0, false, 0);
	}
	{
		const char* text = m_option.aGetSpeakerText(m_option.aMssSpeaker);
		m_audioTab.m_pSpeakerCmb->SetLine(1, text, 0, false, 0);
	}
	int sfx = int(m_option.aSfxVolume * 100.0f);
	if (sfx < 0)
		sfx = 0;
	else if (sfx > 100)
		sfx = 100;
	m_audioTab.m_pSfxGauge->SetPos(sfx);
	int bgm = int(m_option.aBGMVolume * 100.0f);
	if (bgm < 0)
		bgm = 0;
	else if (bgm > 100)
		bgm = 100;
	m_audioTab.m_pBgmGauge->SetPos(bgm);
}

void FrOptionDlg::SetMacroColtrols()
{
	for (int i = 0; i < 8; ++i)
	{
		Doc()->m_chatManager.FilteringHack(m_option.mMacro[i], false);
		m_macroTab.m_pMacro[i]->SetLine(1, m_option.mMacro[i], 0, false, 0);
	}
}

void FrOptionDlg::SetTab(eTabType tab)
{
	COption* option = COption::Instance();
	if (m_hoverCount)
		m_hoverCount = 0;
	if (tab >= TAB_GAME && tab < TAB_NUM && tab != ms_eCurTab)
	{
		ms_eCurTab = tab;
		g_audio->PlaySfx("ui_button_ok_click");
		for (int i = 0; i < TAB_NUM; ++i)
		{
			m_pTab[i]->SetSelected(i == tab);
			m_pTab[i]->m_pTabBtn->SetStatus(
				i == tab ? FrButton::PRESSED : FrButton::NORMAL);
			if (!g_audio->GetMSS())
			{
				if (tab == TAB_AUDIO)
				{
					m_pHint->ClearLine();
					m_pHint->AddText(
						"\xbb\xe7\xbf\xeb\xc7\xd2\x20\xbc\xf6\x20\xc0\xd6\xb4\xc2\x20\xbf\xc0\xb5\xf0\xbf\xc0\x20\xc0\xe5\xc4\xa1\xb0\xa1\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e",
						false, true);
					m_pHint->SetVisible(true);
				}
				else
					m_pHint->SetVisible(false);
			}
		}
	}
}

void FrOptionDlg::OnCancel()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) && !CChatMsg::Instance()->IsOpened())
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
		CMessengerInfo::Instance()->SetGameFocus(true);
	}
	float sensitivity = COption::Instance()->gGetMouseSensitivity();
	if (sensitivity < 0.5f)
		sensitivity = 0.5f;
	else if (sensitivity > 2.0f)
		sensitivity = 2.0f;
	g_input->SetSensitivity(sensitivity);
	if (m_audioTab.m_pEnableBtn->GetStatus() == FrButton::PRESSED)
	{
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
		g_audio->SetVolume(COption::Instance()->aGetSfxVolume());
	}
	else
	{
		g_audio->SetVolume(0.0f);
		g_audio->SetBGMVolume(0.0f);
	}
	FrForm::OnCancel();
}

void FrOptionDlg::OnOK()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) && !CChatMsg::Instance()->IsOpened())
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
		CMessengerInfo::Instance()->SetGameFocus(true);
	}
	m_bApply = true;
	m_bOK = true;
	CloseDlg();
}

void FrOptionDlg::CloseDlg()
{
	if (m_gameTab.m_pTerrainBtn->GetStatus() == FrButton::PRESSED)
		m_option.gTerrainTooltip = 1;
	else
		m_option.gTerrainTooltip = 0;
	CMouseCursor::Instance()->SetTerrainToolTip(m_option.gTerrainTooltip != 0);
	if (!m_bApply)
	{
		EnableChangeNickControls(true);
		return;
	}
	{
		for (int i = 0; i < 8; ++i)
		{
			strncpy(m_option.mMacro[i],
				m_macroTab.m_pMacro[i]->GetLine(1, false), 63);
			m_option.mMacro[i][63] = 0;
			Doc()->m_chatManager.FilteringHack(m_option.mMacro[i], true);
		}
		COption::Instance()->ApplyChange(m_option, m_bApplyScrSize);
		sGameOptionInfo info;
		memset(&info, 0, sizeof(info));
		for (int i = 0; i < 8; ++i)
			strcpy(info.macro[i], m_option.mMacro[i]);
		WSendPacket packet((enumClientPacket)0x69);
		packet.EncodeBuffer(&info, sizeof(info));
		packet.Send(TO_GAME);
		FrForm::OnOK();
	}
}

void FrOptionDlg::EnableChangeNickControls(bool bEnable)
{
	if (m_gameTab.m_pChangeNickBtn && m_gameTab.m_pChangeNickBtn->IsVisible())
		m_gameTab.m_pChangeNickBtn->Enable(bEnable);
	if (m_pOkBtn && m_pOkBtn->IsVisible())
		m_pOkBtn->Enable(bEnable);
}

void FrOptionDlg::tOnHintInit(int param)
{
	m_pHint = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pHint->SetVisible(false);
}

const char* FrOptionDlg::MakeHint(eTabType tab, int index) const
{
	COption* option = COption::Instance();
	switch (tab)
	{
	case TAB_GAME:
		switch (index)
		{
		case 0:
			return "\xb8\xb6\xbf\xec\xbd\xba\x20\xb0\xa8\xb5\xb5\xb8\xa6\x20\xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 1:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc\x20\xb1\xd3\xb8\xbb\xc0\xbb\x20\xba\xb8\xb3\xbe\x20\xbc\xf6\x20\xc0\xd6\xb5\xb5\xb7\xcf\x20\xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 2:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7\x20\xc3\xca\xb4\xeb\xb8\xa6\x20\xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 3:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7\x20\xc4\xa3\xb1\xb8\x20\xb5\xee\xb7\xcf\xbf\xe4\xc3\xbb\xbf\xa1\x20\xc0\xda\xb5\xbf\xc0\xb8\xb7\xce\x20\xb5\xbf\xc0\xc7\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 4:
			return "\xb4\xeb\xc8\xad\xc7\xa5\xbd\xc3\xbf\xb5\xbf\xaa\x20\xb9\xd9\xc5\xc1\xc0\xbb\x20\xc5\xf5\xb8\xed\xc7\xcf\xb0\xd4\x20\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 5:
			return "\xc3\xa2\xc0\xbb\x20\xbf\xad\xb0\xc5\xb3\xaa\x20\xb4\xdd\xc0\xbb\x20\xb6\xa7\x20\xbe\xd6\xb4\xcf\xb8\xde\xc0\xcc\xbc\xc7\x20\xc8\xbf\xb0\xfa\xb8\xa6\x20\xc1\xdd\xb4\xcf\xb4\xd9\x2e";
		case 6:
			return "\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb\x20\xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 7:
			return "\xc7\xd0\xb1\xb3\x20\xb7\xa9\xc5\xb7\xbf\xa1\x20\xbb\xe7\xbf\xeb\xb5\xc9\x20\xbc\xd2\xbc\xd3\x20\xc7\xd0\xb1\xb3\xb8\xa6\x20\xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 8:
			return "\xc7\xe2\xbb\xf3\xb5\xc8\x20\xc3\xd6\xbd\xc5\xc0\xc7\x20\xc5\xb8\xb1\xb8\xb9\xd9\xb8\xa6\x20\xbb\xe7\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9\x28\xb1\xc7\xc0\xe5\x29\x2e";
		case 9:
			return "\xc4\xb8\xc3\xc4\xbd\xc3\x20\x47\x55\x49\x20\xba\xce\xba\xd0\xc0\xbb\x20\xb0\xa8\xc3\xdf\xb0\xed\x20\xc0\xfa\xc0\xe5\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 10:
			return "\xc4\xb8\xc3\xc4\xbd\xc3\x20\xb0\xb3\xc0\xce\xc1\xa4\xba\xb8\xb8\xa6\x20\xb0\xa8\xc3\xdf\xb0\xed\x20\xc0\xfa\xc0\xe5\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 11:
			return "\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba\xc5\xf8\xc6\xc1\xc0\xbb\x20\xc4\xd1\xb0\xc5\xb3\xaa\x20\xb2\xfc\xb4\xcf\xb4\xd9\x2e";
		case 12:
			return "\xc4\xc6\xc0\xce\x20\xc0\xcc\xb9\xcc\xc1\xf6\xb0\xa1\x20\xb0\xd4\xc0\xd3\xc1\xdf\xbf\xa1\x20\xbe\xc8\xba\xb8\xc0\xcc\xb5\xb5\xb7\xcf\x20\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		}
		break;
	case TAB_VIDEO:
		switch (index)
		{
		case 0:
			return "\xc6\xce\xbe\xdf\xb8\xa6\x20\xc0\xfc\xc3\xbc\xc8\xad\xb8\xe9\xc0\xb8\xb7\xce\x20\xc1\xf1\xb1\xe6\x20\xbc\xf6\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e";
		case 1:
			return "\xc6\xce\xbe\xdf\xb8\xa6\x20\xc0\xfc\xc3\xbc\xc3\xa2\x20\xc8\xad\xb8\xe9\xc0\xb8\xb7\xce\x20\xc1\xf1\xb1\xe6\x20\xbc\xf6\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e";
		case 2:
			return "\xc6\xce\xbe\xdf\xb8\xa6\x20\xbf\xcd\xc0\xcc\xb5\xe5\x20\xc8\xad\xb8\xe9\xbf\xa1\xbc\xad\x20\xc1\xf1\xb1\xe6\x20\xbc\xf6\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e";
		case 5:
			return "\xb1\xd7\xb7\xa1\xc7\xc8\x20\xc3\xb3\xb8\xae\xb9\xe6\xbd\xc4\xc0\xbb\x20\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0\x28\xc0\xe7\xbd\xc3\xc0\xdb\xc7\xca\xbf\xe4\x29\x2e";
		case 3:
			return "\xbf\xa9\xb7\xaf\xb0\xa1\xc1\xf6\x20\xba\xf1\xb5\xf0\xbf\xc0\x20\xbc\xb3\xc1\xa4\xc0\xbb\x20\xc7\xd1\xb2\xa8\xb9\xf8\xbf\xa1\x20\xb9\xd9\xb2\xdc\x20\xbc\xf6\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e";
		case 4:
			return "\xc8\xad\xb8\xe9\xc0\xc7\x20\xc7\xd8\xbb\xf3\xb5\xb5\xb8\xa6\x20\xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 6:
			return "\xc0\xfb\xc1\xa4\xc7\xd1\x20\xb1\xd7\xb7\xa1\xc7\xc8\x20\xbc\xbc\xb9\xd0\xb5\xb5\xb8\xa6\x20\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0\x28\xc0\xe7\xbd\xc3\xc0\xdb\xc7\xca\xbf\xe4\x29\x2e";
		case 7:
			return "\xc6\xce\xbe\xdf\x20\xc1\xd6\xc0\xce\xb0\xf8\xb5\xe9\xc0\xc7\x20\xb1\xd7\xb8\xb2\xc0\xda\xb8\xa6\x20\xc4\xd1\xb0\xc5\xb3\xaa\x20\xb2\xfc\xb4\xcf\xb4\xd9\x2e";
		case 8:
			return "\xb3\xaa\xb9\xab\xb3\xaa\x20\xb0\xc7\xb9\xb0\x20\xb5\xee\xc0\xc7\x20\xb1\xd7\xb8\xb2\xc0\xda\xb8\xa6\x20\xc4\xd1\xb0\xc5\xb3\xaa\x20\xb2\xfc\xb4\xcf\xb4\xd9\x2e";
		case 9:
			return "\xb9\xb0\xc7\xa5\xb8\xe9\xc0\xc7\x20\xb9\xdd\xbb\xe7\x20\xc8\xbf\xb0\xfa\xb8\xa6\x20\xbb\xe7\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 10:
			return "\x4c\x4f\x44\x20\xb1\xe2\xb4\xc9\xc0\xbb\x20\xc8\xb0\xbc\xba\xc8\xad\x20\xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9\x2e";
		case 11:
			return "\xc6\xce\xbe\xdf\xbc\xb6\x20\xb0\xf7\xb0\xf7\xc0\xc7\x20\xc0\xdb\xc0\xba\x20\xc4\xa3\xb1\xb8\xb5\xe9\xc0\xbb\x20\xb8\xb8\xb3\xaa\xba\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f";
		case 12:
			return "\xbf\xa9\xb7\xaf\xb0\xa1\xc1\xf6\x20\xc6\xaf\xbc\xf6\xc8\xbf\xb0\xfa\xb5\xe9\xc0\xc7\x20\xc7\xb0\xc1\xfa\xc0\xbb\x20\xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		}
		break;
	case TAB_AUDIO:
		switch (index)
		{
		case 0:
			return "\xb8\xf0\xb5\xe7\x20\xbc\xd2\xb8\xae\xb8\xa6\x20\xc4\xd1\xb0\xc5\xb3\xaa\x20\xb2\xfc\xb4\xcf\xb4\xd9\x2e";
		case 1:
			return "\xc0\xfb\xc1\xa4\xc7\xd1\x20\xc0\xbd\xc1\xfa\xc0\xbb\x20\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0\x2e";
		case 2:
			return "\xc8\xbf\xb0\xfa\xc0\xbd\xc0\xc7\x20\xba\xbc\xb7\xfd\xc0\xbb\x20\xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 3:
			return "\xb9\xe8\xb0\xe6\xc0\xbd\xbe\xc7\xc0\xc7\x20\xba\xbc\xb7\xfd\xc0\xbb\x20\xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		case 4:
			return "\xbd\xba\xc7\xc7\xc4\xbf\x20\xc5\xb8\xc0\xd4\xc0\xbb\x20\xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9\x2e";
		}
		break;
	case TAB_MACRO:
		return NULL;
	}
	return NULL;
}

void FrOptionDlg::ProcessHoverMessage(FrWnd* pWnd)
{
	if (m_pHint && m_pTab[ms_eCurTab]->m_bSelected)
	{
		if (m_hoverCount > 0)
		{
			if (pWnd)
			{
				for (int i = 0; i < m_pTab[ms_eCurTab]->m_num; ++i)
				{
					if (*m_pTab[ms_eCurTab]->m_pppCtrl[i] == pWnd)
					{
						const char* hint = MakeHint(ms_eCurTab, i);
						if (hint)
						{
							m_pHint->ClearLine();
							m_pHint->AddText(hint, false, true);
							m_pHint->SetVisible(true);
						}
					}
				}
			}
		}
		else if (m_hoverCount == 0)
			m_pHint->SetVisible(false);
	}
}

void FrOptionDlg::OnHoverOn(int param)
{
	++m_hoverCount;
	ProcessHoverMessage(DYNAMIC_CAST(FrWnd, (FrWnd*)param));
}

void FrOptionDlg::OnHoverOff(int param)
{
	--m_hoverCount;
	ProcessHoverMessage(NULL);
}

bool FrOptionDlg::OnChangeNickConfirmDlgResult(int result, FrForm* form)
{
	m_pChangeNickDlg = NULL;
	EnableChangeNickControls(true);
	return true;
}

bool FrOptionDlg::OnChangeSchoolDlgResult(int result, FrForm* form)
{
	if (result == FrOK && m_pSchoolDlg)
	{
		if (m_pSchoolDlg->m_schoolName.length() > 0)
		{
			char name[1024];
			strcpy(name, m_pSchoolDlg->m_schoolName.c_str());
			m_gameTab.m_pSchoolEdit->SetLine(1, strtok(name, "("), 0, false, 0);
		}
		else
			m_gameTab.m_pSchoolEdit->SetLine(1, "", 0, false, 0);
		WSendPacket packet((enumClientPacket)0x3b);
		packet.Encode4(m_pSchoolDlg->m_schoolCode);
		packet.Send(TO_GAME);
		IActor* actor = AfxGetTask()->GetActor("Lobby");
		if (actor)
			actor << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	m_pSchoolDlg = NULL;
	return true;
}
