#include "minatl.h"
#include "onelinereqdlg.h"
#include "onelineboard.h"
#include "emoticondlg.h"
#include "fredit.h"
#include "frbutton.h"
#include "frarea.h"
#include "frwndmanager.h"
#include "fresh.h"
#include "chatmsg.h"
#include "inputmanager.h"
#include "logininfo.h"
#include "wfont.h"
#include "../../shared/s5/sharedutilities.h"

extern Fresh* g_pFresh;
extern WView* g_view;

IMPLEMENT_OBJECT(FrOnelineReqDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrOnelineReqDlg, FrForm)
ON_FRESH_VI("explain", FRCMD_INIT, FrOnelineReqDlg::OnExplainInit)
ON_FRESH_VI("chatinput", FRCMD_INIT, FrOnelineReqDlg::OnChatInputInit)
ON_FRESH_BI("chatinput", FRCMD_ENTERKEY, FrOnelineReqDlg::OnChatEnterKey)
ON_FRESH_VI("emoticon", FRCMD_INIT, FrOnelineReqDlg::OnEmoticonInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrOnelineReqDlg::OnEmoticonBtnUp)
ON_FRESH_VI("language", FRCMD_INIT, FrOnelineReqDlg::OnLanguageInit)
ON_FRESH_VI("yes", FRCMD_INIT, FrOnelineReqDlg::OnYesInit)
END_FRESH_MSGMAP()

FrOnelineReqDlg::FrOnelineReqDlg()
{
	m_pChatInput = NULL;
	m_pEmoticon = NULL;
	m_pLanguage = NULL;
	m_pEmoticonDlg = NULL;
}

FrOnelineReqDlg::~FrOnelineReqDlg()
{
}

void FrOnelineReqDlg::OnChatInputInit(int param)
{
	m_pChatInput = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pChatInput)
		m_pChatInput->SetKeyFocus(true);
}

bool FrOnelineReqDlg::OnChatEnterKey(int param)
{
	OnOK();
	return false;
}

void FrOnelineReqDlg::OnEmoticonInit(int param)
{
	m_pEmoticon = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrOnelineReqDlg::OnEmoticonBtnUp()
{
	if (m_pEmoticon && !m_pEmoticonDlg)
	{
		m_pEmoticonDlg =
			CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
		if (m_pEmoticonDlg)
		{
			WPoint pos(0.0f, 0.0f);
			if (m_pEmoticon)
			{
				pos.x = m_pEmoticon->GetRect().x - 200.0f;
				pos.y = m_pEmoticon->GetRect().y - 260.0f;
			}
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&FrOnelineReqDlg::OnEmoticonResult, pos, 3);
		}
	}
}

bool FrOnelineReqDlg::OnEmoticonResult(int result, FrForm* pForm)
{
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg)
		{
			const char* icon = pDlg->GetSelectedIcon();
			if (icon)
			{
				FrEdit* pEdit = m_pChatInput;
				if (pEdit)
				{
					const char* front = pEdit->GetEditText_Front();
					const char* comp = pEdit->GetEditText_Comp();
					const char* end = pEdit->GetEditText_End();
					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, icon, end)) <
						pEdit->GetWidthLimit())
						pEdit->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, icon, end), 0,
							false, 0);
					pEdit->SetKeyFocus(true);
				}
			}
		}
	}
	m_pEmoticonDlg = NULL;
	return true;
}

void FrOnelineReqDlg::OnLanguageInit(int param)
{
	m_pLanguage = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pLanguage)
		SetLanguage(g_ime->IsAlphaNumericMode());
}

void FrOnelineReqDlg::SetLanguage(bool english)
{
	if (english)
		m_pLanguage->SetBgImg("chat_eng");
	else
		m_pLanguage->SetBgImg("chat_kor");
}

void FrOnelineReqDlg::OnExplainInit(int param)
{
	FrEdit* pEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	pEdit->AddLine(
		"\\c0xff000000,t\\c\xc0\xfc\xb1\xa4\xc6\xc7 \xbc\xd2\xb0\xb3\\c0xff000000,n\\c",
		0, false);
	pEdit->AddLine(
		"- \xb0\xd4\xc0\xd3 \xb3\xbb\xbf\xa1\xbc\xad \xc0\xaf\xc0\xfa\xb0\xa3 \xbc\xd2\xbd\xc4\xc0\xbb \xc0\xfc\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb1\xa4\xb0\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
		0, false);
	pEdit->AddLine(
		MakeStr(
			"- \xc0\xfc\xb1\xa4\xc6\xc7\xbf\xa1 \xb1\xdb \xb5\xee\xb7\xcf \xbd\xc3 %d\xc4\xed\xc5\xb0\xb0\xa1 \xbc\xd2\xba\xf1\xb5\xcb\xb4\xcf\xb4\xd9.",
			1),
		0, false);
	pEdit->AddLine(
		"- \xc7\xd1\xb1\xdb \xb1\xe2\xba\xbb\xc0\xb8\xb7\xce \xc3\xd6\xb4\xeb 20\xc0\xda\xb1\xee\xc1\xf6 \xbe\xb2\xb1\xe2\xb0\xa1 \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
		0, false);
	pEdit->AddLine("  (\xc0\xcc\xb8\xf0\xc6\xbc\xc4\xdc \xc1\xf6\xbf\xf8)", 0,
		false);
	pEdit->AddLine(
		"\\c0xff000000,t\\c\xc0\xfc\xb1\xa4\xc6\xc7 \xbd\xc5\xc3\xbb\\c0xff000000,n\\c",
		0, false);
	pEdit->AddLine("\\c0xffff0000\\c*\xc1\xd6\xc0\xc7", 0, false);
	pEdit->AddLine(
		"- \xc0\xfc\xb1\xa4\xc6\xc7 \xbd\xc5\xc3\xbb\xc0\xcc \xbf\xcf\xb7\xe1\xb5\xc8 \xb1\xdb\xc0\xba \xbc\xf6\xc1\xa4, \xc3\xeb\xbc\xd2\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9",
		0, false);
	pEdit->AddLine("  \xc7\xd5\xb4\xcf\xb4\xd9.", 0, false);
	pEdit->AddLine(
		"- \xc0\xbd\xb6\xf5\xb1\xdb, \xbf\xe5\xbc\xb3, \xbb\xf3\xbe\xf7\xc0\xfb \xb1\xa4\xb0\xed\xb8\xa6 \xbf\xc3\xb8\xb1 \xbd\xc3 \xb0\xd4\xc0\xd3\xb3\xbb \xc1\xa6\xc0\xe7\xb8\xa6 ",
		0, false);
	pEdit->AddLine(
		"  \xb9\xde\xc0\xbb \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0,
		false);
	pEdit->AddLine(
		"- \xc0\xcc \xbb\xf3\xc7\xb0\xc0\xba \xc3\xbb\xbe\xe0\xc3\xb6\xc8\xb8(\xb1\xb8\xb8\xc5\xc3\xeb\xbc\xd2)\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9.\\c0xff000000\\c",
		0, false);
}

void FrOnelineReqDlg::OnYesInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

IMPLEMENT_OBJECT(FrOnelineReqChkOutDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrOnelineReqChkOutDlg, FrForm)
ON_FRESH_VI("explain", FRCMD_INIT, FrOnelineReqChkOutDlg::OnExplainInit)
ON_FRESH_VI("yes", FRCMD_INIT, FrOnelineReqChkOutDlg::OnYesInit)
END_FRESH_MSGMAP()

FrOnelineReqChkOutDlg::FrOnelineReqChkOutDlg()
{
}

FrOnelineReqChkOutDlg::~FrOnelineReqChkOutDlg()
{
}

void FrOnelineReqChkOutDlg::OnYesInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton && Doc()->m_cookie < 1)
		pButton->Enable(false);
}

void FrOnelineReqChkOutDlg::OnExplainInit(int param)
{
	__int64 cookie = Doc()->m_cookie;
	__int64 remainingCookie = cookie - 1;
	unsigned long provider = LOGININFO()->ProvType();
	Doc()->RemainOwnCash(provider, 1);
	bool insufficient = false;
	if (remainingCookie < 0)
	{
		remainingCookie = 0;
		insufficient = true;
	}
	int waiting = COneLineBoard::Instance()->GetWaitMsgNum();
	int minutes = COneLineBoard::Instance()->GetWaitMsgTime() / 60000;
	FrEdit* pEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	pEdit->AddLine(
		MakeStr(
			"\xc0\xfc\xb1\xa4\xc6\xc7 \xbd\xc5\xc3\xbb\xc0\xbb \xc7\xcf\xbd\xc3\xb8\xe9 \\c0xffff0000,t\\c%d\xc4\xed\xc5\xb0\\c0xff000000,n\\c\xb0\xa1 \xbc\xd2\xba\xf1\xb5\xc7\xb8\xe7, ",
			1),
		0, false);
	pEdit->AddLine(
		MakeStr(
			"\xc7\xf6\xc0\xe7 %d\xb8\xed\xc0\xc7 \xb4\xeb\xb1\xe2\xc0\xda\xb0\xa1 \xc0\xd6\xbe\xee \xbe\xe0 %d\xba\xd0\xb5\xda\xbf\xa1 \xc7\xa5\xbd\xc3\xb5\xcb\xb4\xcf\xb4\xd9.",
			waiting, minutes),
		0, false);
	pEdit->AddLine(" ", 0, false);
	pEdit->AddLine(
		"\xb0\xe8\xbc\xd3 \xc1\xf8\xc7\xe0\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		0, false);
	pEdit->AddLine(" ", 0, false);
	pEdit->AddLine(
		MakeStr(
			"     \\c0xff000000,t\\c\xb0\xe1\xc1\xa6\xc7\xd2 \xc4\xed\xc5\xb0\\c0xff000000,n\\c:     %d",
			1),
		0, false);
	pEdit->AddLine(
		MakeStr(
			"     \\c0xff000000,t\\c\xba\xb8\xc0\xaf\xc7\xd1 \xc4\xed\xc5\xb0\\c0xff000000,n\\c:     %I64d",
			cookie),
		0, false);
	pEdit->AddLine(
		MakeStr(
			"        \\c0xff000000,t\\c\xb3\xb2\xb4\xc2 \xc4\xed\xc5\xb0\\c0xff000000,n\\c:     %d",
			remainingCookie),
		0, false);

	const char* cashName = NULL;
	switch (Doc()->m_provType)
	{
	case 2:
		cashName = "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac";
		break;
	case 4:
		cashName = "\xc7\xd1\xc4\xda\xc0\xce";
		break;
	default:
		return;
	}
	// HACK: this hack is needed to make VC7.1 not elide the null check
	for (int display = 0; display < 1; ++display)
	{
		if (cashName)
		{
			if (insufficient)
				pEdit->AddLine(
					MakeStr(
						"\\c0xffff0000,t\\c%s \xc0\xdc\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
						cashName),
					0, false);
			else
			{
				__int64 cash = Doc()->m_cash;
				__int64 bonus =
					S5::ExchangeOwnCookieToCash(MyBonusCash() - 1, 100);
				if (bonus < 0)
					cash = Doc()->m_cash + bonus;
				pEdit->AddLine(
					MakeStr(
						"     \\c0xffff0000,t\\c\xb1\xb8\xb8\xc5 \xc8\xc4 \xc3\xd6\xc1\xbe %s \xc0\xdc\xbe\xd7\xc0\xba %I64d\xbf\xf8\\c0xff000000,n\\c\xc0\xd4\xb4\xcf\xb4\xd9.",
						cashName, cash > 0 ? cash : 0),
					0, false);
			}
		}
	}
}
