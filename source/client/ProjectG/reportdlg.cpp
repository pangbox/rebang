#include "reportdlg.h"
#include "emoticondlg.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frbutton.h"
#include "frwndmanager.h"
#include "fresh.h"
#include "golftask.h"
#include "avatar_task.h"
#include "pangfbi.h"
#include "actor.h"
#include "projectg.h"
#include "wfont.h"

extern Fresh* g_pFresh;
extern WView* g_view;

IMPLEMENT_OBJECT(FrReportDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrReportDlg, FrForm)

ON_FRESH_VI("id", FRCMD_INIT, FrReportDlg::OnIdInit)
ON_FRESH_BI("id", FRCMD_ENTERKEY, FrReportDlg::OnIdEnterKey)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrReportDlg::OnEmoticonBtnUp)
ON_FRESH_VI("reason", FRCMD_INIT, FrReportDlg::OnReasonInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrReportDlg::OnOKInit)

END_FRESH_MSGMAP()

FrReportDlg::FrReportDlg()
{
	m_pId = NULL;
	m_pReason = NULL;
	m_pOK = NULL;
	g_pFresh->GetManager()->SetExclusiveKey(true);
}

FrReportDlg::~FrReportDlg()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) && !CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);
}

void FrReportDlg::OnIdInit(int param)
{
	m_pId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	m_pId->SetKeyFocus(true);
}

bool FrReportDlg::OnIdEnterKey(int param)
{
	OnOK();

	return false;
}

bool FrReportDlg::OnEmoticonResult(int result, FrForm* pForm)
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
				if (m_pId)
				{
					const char* front = m_pId->GetEditText_Front();
					const char* comp = m_pId->GetEditText_Comp();
					const char* end = m_pId->GetEditText_End();

					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, icon, end)) <
						m_pId->GetWidthLimit())
						m_pId->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, icon, end), 0,
							false, 0);
					m_pId->SetKeyFocus(true);
				}
			}
		}
	}

	m_pEmoticonDlg = NULL;
	return true;
}

void FrReportDlg::OnEmoticonBtnUp()
{
	m_pEmoticonDlg =
		CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
	if (m_pEmoticonDlg)
	{
		m_pEmoticonDlg->Open((FRESH_PFN_RESULT)&FrReportDlg::OnEmoticonResult,
			3);
	}
}

void FrReportDlg::OnReasonInit(int param)
{
	m_pReason = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
	if (m_pReason == NULL)
		return;

	m_pReason->AddString("1.\xbf\xe5\xbc\xb3,\xc5\xb8\xc0\xce\xba\xf1\xb9\xe6");
	m_pReason->AddString("2.\xba\xce\xb8\xf0/\xbc\xba\xc0\xfb\xbf\xe5\xbc\xb3");
	m_pReason->AddString("3.\xb1\xe2\xc5\xb8\xbb\xe7\xc7\xd7");

	m_pReason->SetLine(1, "1.\xbf\xe5\xbc\xb3,\xc5\xb8\xc0\xce\xba\xf1\xb9\xe6",
		0, false, 0);
}

void FrReportDlg::OnProc(const float deltaTime)
{
	std::string id = m_pId->GetLine(1, false);
	if (id.empty())
		m_pOK->Enable(false);
	else
		m_pOK->Enable(true);
}

void FrReportDlg::OnOKInit(int param)
{
	m_pOK = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pOK->Enable(false);
}

void FrReportDlg::OnOK()
{
	char szPath[512];
	strcpy(szPath, g_executeDirectory);

	char szBuf[1024] = {
		0,
	};
	FILE* fp = fopen(MakeStr("%s\\chat_history.txt", szPath), "wt");
	if (fp == NULL)
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(szBuf,
		"\xbd\xc5\xb0\xed\xbd\xc3\xb0\xa3 : %d:%02d:%02d %02d:%02d:%02d\n",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	fwrite(szBuf, strlen(szBuf), 1, fp);

	sprintf(szBuf, "\xbd\xc5\xb0\xed\xb4\xeb\xbb\xf3 \xc0\xaf\xc0\xfa : %s\n",
		m_pId->GetLine(1, false));
	fwrite(szBuf, strlen(szBuf), 1, fp);

	sprintf(szBuf,
		"------------------------------------------------------------\n");
	fwrite(szBuf, strlen(szBuf), 1, fp);

	for (std::list<sChatLine>::iterator it = Doc()->m_chatLineList.begin();
		it != Doc()->m_chatLineList.end(); ++it)
	{
		fwrite((*it).text.c_str(), strlen((*it).text.c_str()), 1, fp);
		fwrite("\n", 1, 1, fp);
	}

	fclose(fp);

	_snprintf(szBuf, 1023, "%s %d %s\\chat_history.txt",
		Doc()->m_myInfo.info.sNick, 4, szPath);

	CPangFBI::Instance()->ProcessReport("crime_log", szBuf);

	FrForm::OnOK();

	if (IS_KINDOF(CGolfTask, AfxGetTask()))

		AfxGetTask()->GetActor("GolfRule") << MsgObject(NULL, 35,
			(int)"\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);

	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))

		AfxGetTask()->GetActor("AvatarChat") << MsgObject(NULL, 35,
			(int)"\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);

	else

		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)"\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
}
