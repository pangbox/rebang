#include "avatarchatlogdlg.h"
#include "reportdlg.h"
#include "fredit.h"
#include "frscrollbar.h"
#include "fresh.h"
#include "chatmsg.h"
#include "golfdoc.h"
extern Fresh* g_pFresh;
IMPLEMENT_OBJECT(FrAvatarChatLogDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrAvatarChatLogDlg, FrForm)

ON_FRESH_VI("chatview", FRCMD_INIT, FrAvatarChatLogDlg::OnChatViewInit)
ON_FRESH_VI("hidechat", FRCMD_INIT, FrAvatarChatLogDlg::OnHideChatInit)
ON_FRESH_VI("showchat", FRCMD_INIT, FrAvatarChatLogDlg::OnShowChatInit)

ON_FRESH_VV("hidechat", FRCMD_LBUTTONUP, FrAvatarChatLogDlg::OnBtnHideChatUp)
ON_FRESH_VV("showchat", FRCMD_LBUTTONUP, FrAvatarChatLogDlg::OnBtnShowChatUp)
ON_FRESH_VV("savechat", FRCMD_LBUTTONUP, FrAvatarChatLogDlg::OnBtnSaveChatUp)
ON_FRESH_VV("accuse", FRCMD_LBUTTONUP, FrAvatarChatLogDlg::OnBtnAccuseUp)
ON_FRESH_VV("closedlg", FRCMD_LBUTTONUP, FrAvatarChatLogDlg::OnBtnCloseDlgUp)

END_FRESH_MSGMAP()

FrAvatarChatLogDlg::FrAvatarChatLogDlg()
	: m_pUnused110(NULL),
	  m_pShowChat(NULL),
	  m_pHideChat(NULL),
	  m_pUnused11c(NULL),
	  m_pUnused120(NULL),
	  m_pChatView(NULL)
{
}

FrAvatarChatLogDlg::~FrAvatarChatLogDlg()
{
}

void FrAvatarChatLogDlg::OnBtnShowChatUp()
{
	if (m_pHideChat)
		m_pHideChat->SetVisible(true);
	if (m_pShowChat)
		m_pShowChat->SetVisible(false);

	GOLFDOC()->m_bShowChat = true;

	CChatMsg::Instance()->AddChatMsg(
		"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xc8\xb0\xbc\xba\xc8\xad\xc7\xd5\xb4\xcf\xb4\xd9.",
		0xffff7878, true, false);
}

void FrAvatarChatLogDlg::OnBtnHideChatUp()
{
	if (m_pHideChat)
		m_pHideChat->SetVisible(false);
	if (m_pShowChat)
		m_pShowChat->SetVisible(true);

	GOLFDOC()->m_bShowChat = false;

	CChatMsg::Instance()->AddChatMsg(
		"\xb0\xd4\xc0\xd3\xb3\xbb \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xbc\xfb\xb1\xe9\xb4\xcf\xb4\xd9.",
		0xffff7878, true, false);
}

void FrAvatarChatLogDlg::OnBtnSaveChatUp()
{
	int num = Doc()->m_chatManager.SaveChatHistory(GOLFDOC()->m_saveChatNum, 1);
	GOLFDOC()->m_saveChatNum = num;
}

bool FrAvatarChatLogDlg::OnReportDlgResult(int result, FrForm* form)
{
	m_pReportDlg = NULL;
	return true;
}

void FrAvatarChatLogDlg::OnBtnAccuseUp()
{
	m_pReportDlg =
		CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report", NULL);
	if (!m_pReportDlg)
		return;

	std::string msg;
	msg =
		"1. \xb4\xeb\xc8\xad\xc3\xa2\xc0\xc7 \xc1\xf6\xb3\xad \xb3\xbb\xbf\xeb\xb5\xb5 \xb8\xf0\xb5\xce \xbc\xad\xb9\xf6\xbf\xa1 \xb1\xe2\xb7\xcf\xb5\xcb\xb4\xcf\xb4\xd9.\n";
	msg +=
		"2. \xc1\xb6\xc0\xdb, \xc7\xe3\xc0\xa7, \xb9\xdd\xba\xb9\xbd\xc5\xb0\xed\xb4\xc2 \xc3\xb3\xb9\xfa\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.\n";
	msg +=
		"3. \xbd\xc5\xb0\xed\xc7\xcf\xbd\xc5 \xb3\xbb\xbf\xeb\xc0\xba 48\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xc3\xb3\xb8\xae\xb5\xcb\xb4\xcf\xb4\xd9.";

	m_pReportDlg->SetMessage(msg.c_str(), false);
	m_pReportDlg->Open((FRESH_PFN_RESULT)&FrAvatarChatLogDlg::OnReportDlgResult,
		3);
}

void FrAvatarChatLogDlg::OnBtnCloseDlgUp()
{
	Close(FrOK, true);
}

void FrAvatarChatLogDlg::OnHideChatInit(int param)
{
	m_pHideChat = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pHideChat)
		return;

	if (GOLFDOC()->m_bShowChat)
	{
		m_pHideChat->SetVisible(true);
	}
	else
	{
		m_pHideChat->SetVisible(false);
	}
}

void FrAvatarChatLogDlg::OnShowChatInit(int param)
{
	m_pShowChat = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pShowChat)
		return;

	if (GOLFDOC()->m_bShowChat)
	{
		m_pShowChat->SetVisible(false);
	}
	else
	{
		m_pShowChat->SetVisible(true);
	}
}

void FrAvatarChatLogDlg::OnChatViewInit(int param)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (!m_pChatView)
		return;

	m_pChatView->HidePrivacy(true);

	for (std::list<sChatLine>::iterator it = Doc()->m_chatLineList.begin();
		it != Doc()->m_chatLineList.end(); ++it)
	{
		sChatLine& line = *it;
		m_pChatView->AddLine(line.text.c_str(),
			CChatMsg::Instance()->ConvertToUIColor(line.color), false);
	}
}

void FrAvatarChatLogDlg::AddChatMsg(const char* msg, int color)
{
	if (m_pChatView->GetLineNum() > 999)
	{
		m_pChatView->DeleteFirstLine();
		m_pChatView->GetScrollBar()->DelItem();
	}

	m_pChatView->AddLine(msg, color, false);
}
