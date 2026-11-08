#include "wmath.h"
#include "missioneventdlg.h"
#include "grounditem.h"
#include "contentsdoc.h"
#include "actor.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrMissionEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMissionEventDlg, FrForm)

ON_FRESH_VI("eventdetail", FRCMD_INIT, FrMissionEventDlg::OnInitDetailButton)
ON_FRESH_VV("eventdetail", FRCMD_LBUTTONDOWN,
	FrMissionEventDlg::OnLButtonDownDetailButton)
ON_FRESH_VI("daygift", FRCMD_INIT, FrMissionEventDlg::OnInitDayGiftButton)
ON_FRESH_VV("daygift", FRCMD_LBUTTONDOWN,
	FrMissionEventDlg::OnLButtonDownDayGiftButton)
ON_FRESH_VI("termgift", FRCMD_INIT, FrMissionEventDlg::OnInitTermGiftButton)
ON_FRESH_VV("termgift", FRCMD_LBUTTONDOWN,
	FrMissionEventDlg::OnLButtonDownTermGiftButton)
ON_FRESH_VI("clear_mission", FRCMD_INIT,
	FrMissionEventDlg::OnInitMissionClearArea)
ON_FRESH_VI("clear_mission", FRCMD_OWNERDRAW,
	FrMissionEventDlg::OnOwnerDrawMissionClearArea)
ON_FRESH_VI("clear_course", FRCMD_INIT,
	FrMissionEventDlg::OnInitCourseClearArea)
ON_FRESH_VI("clear_course", FRCMD_OWNERDRAW,
	FrMissionEventDlg::OnOwnerDrawCourseClearArea)
ON_FRESH_VI("clear_course", FRCMD_HOVERON,
	FrMissionEventDlg::OnHoverOnCourseClearArea)
ON_FRESH_VI("clear_course", FRCMD_HOVEROFF,
	FrMissionEventDlg::OnHoverOffCourseClearArea)

END_FRESH_MSGMAP()

FrMissionEventDlg::FrMissionEventDlg()
{
	// HACK
	(void)&g_EPSILON;
	memset(m_pButton, 0, sizeof(m_pButton));
	memset(m_pBitmap, 0, sizeof(m_pBitmap));
	m_pCourseClearArea = NULL;
	m_pMissionClearArea = NULL;
	m_pDetailDlg = NULL;
	m_pMissionEvent = NULL;
	memset(m_bMissionComplete, 0, sizeof(m_bMissionComplete));
	m_bCourseHover = false;
	m_completeCourseNum = 0;
}

FrMissionEventDlg::~FrMissionEventDlg()
{
}

bool FrMissionEventDlg::OnInit()
{
	m_pBitmap[0] = g_pFresh->RegisterBitmap("missionevent_clear_mark");
	m_pBitmap[1] = g_pFresh->RegisterBitmap("course_bg1");
	m_pBitmap[2] = g_pFresh->RegisterBitmap("course_bg_c1");
	CContentsDoc::Instance()->GetContainer((localContentType_t)131,
		m_pMissionEvent);
	if (!m_pMissionEvent)
		return false;
	m_missionPos[0] = WPoint(215.0f, 250.0f);
	m_missionPos[1] = WPoint(350.0f, 250.0f);
	m_missionPos[2] = WPoint(215.0f, 370.0f);
	m_missionPos[3] = WPoint(215.0f, 450.0f);
	m_missionPos[4] = WPoint(350.0f, 370.0f);
	m_missionPos[5] = WPoint(350.0f, 450.0f);
	m_missionPos[6] = WPoint(485.0f, 370.0f);
	m_textPos[0] = WPoint(256.0f, 301.0f);
	m_textPos[1] = WPoint(388.0f, 301.0f);
	m_textPos[2] = WPoint(253.0f, 423.0f);
	m_textPos[3] = WPoint(392.0f, 505.0f);
	m_textPos[4] = WPoint(546.0f, 359.0f);
	CheckMissionComplete();
	CheckCourseComplete();
	return FrForm::OnInit();
}

bool FrMissionEventDlg::OnDetailDlgResult(int result, FrForm* form)
{
	m_pDetailDlg = NULL;
	return true;
}

void FrMissionEventDlg::CheckMissionComplete()
{
	if (m_pMissionEvent->IsCompleteAllDayMission() == true)
		m_pButton[1]->Enable(true);
	if (m_pMissionEvent->IsCompleteAllTermMission() == true)
		m_pButton[2]->Enable(true);
	for (int i = 0; i < 2; ++i)
		if (m_pMissionEvent->IsCompleteMission(i))
			m_bMissionComplete[i] = true;
	for (int j = 2; j < 7; ++j)
		if (m_pMissionEvent->IsCompleteMission(j))
			m_bMissionComplete[j] = true;
}

void FrMissionEventDlg::CheckCourseComplete()
{
	memset(m_bCourseComplete, 0, sizeof(m_bCourseComplete));
	for (int i = 0; i < m_pMissionEvent->GetMapCount(); ++i)
	{
		if (m_pMissionEvent->IsCompleteCourse(i))
		{
			m_bCourseComplete[i] = 1;
			++m_completeCourseNum;
		}
	}
}

void FrMissionEventDlg::RequestGift(int type)
{
	WSendPacket send((enumClientPacket)228);
	send.Encode4(type);
	send.Send(TO_GAME);
}

void FrMissionEventDlg::CalcOffset(int index, float& x, float& y)
{
	int column = index > 7;
	if (column == 0)
	{
		x = 0.0f;
		y = index * 12.0f;
	}
	else if (column == 1)
	{
		x = 65.0f;
		y = (index - 8) * 12.0f;
	}
}

void FrMissionEventDlg::OnInitDetailButton(int param)
{
	m_pButton[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMissionEventDlg::OnLButtonDownDetailButton()
{
	if (!m_pDetailDlg)
	{
		m_pDetailDlg = CreateForm<FrMissionDetail>(g_pFresh->GetManager(), this,
			"mission_detail", NULL);
		if (m_pDetailDlg)
			m_pDetailDlg->Open(
				(FRESH_PFN_RESULT)&FrMissionEventDlg::OnDetailDlgResult, 1);
	}
}

void FrMissionEventDlg::OnInitDayGiftButton(int param)
{
	m_pButton[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pButton[1])
		m_pButton[1]->Enable(false);
}

void FrMissionEventDlg::OnLButtonDownDayGiftButton()
{
	if (m_pMissionEvent->GetDayGiftFlag() == true)
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc0\xcc\xb9\xcc \xc0\xcf\xc0\xcf\xb9\xcc\xbc\xc7 \xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	else
		RequestGift(1);
}

void FrMissionEventDlg::OnInitTermGiftButton(int param)
{
	m_pButton[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pButton[2])
		m_pButton[2]->Enable(false);
}

void FrMissionEventDlg::OnLButtonDownTermGiftButton()
{
	if (m_pMissionEvent->GetTermGiftFlag() == true)
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc0\xcc\xb9\xcc \xb1\xe2\xb0\xa3\xb9\xcc\xbc\xc7 \xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	else
		RequestGift(2);
}

void FrMissionEventDlg::OnInitMissionClearArea(int param)
{
	m_pMissionClearArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrMissionEventDlg::OnOwnerDrawMissionClearArea(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;
	unsigned long color = pGDI->GetTextColor();
	char text[4] = { 0 };
	unsigned char indices[4] = { 0, 1, 2, 5 };
	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);
	for (int i = 0; i < 4; ++i)
	{
		sprintf(text, "%d", m_pMissionEvent->GetCondition(indices[i]));
		WPoint pos(m_textPos[i].x, m_textPos[i].y);
		pGDI->Print(pos, 0, "%s", text);
	}
	pGDI->SetTextColor(color, 0xffffffff);
	unsigned int width = m_pBitmap[0]->Width();
	unsigned int height = m_pBitmap[0]->Height();
	for (int j = 0; j < 7; ++j)
	{
		if (m_bMissionComplete[j])
			pGDI->DrawTexture(m_pBitmap[0],
				WRect(m_missionPos[j].x, m_missionPos[j].y, (float)width,
					(float)height),
				0xffffffff, 0);
	}
}

void FrMissionEventDlg::OnInitCourseClearArea(int param)
{
	m_pCourseClearArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pCourseClearArea)
	{
		m_pCourseClearArea->SetMouseEvent(true);
		m_pCourseClearArea->EnableHover(true, 0.0f);
	}
}

void FrMissionEventDlg::OnOwnerDrawCourseClearArea(int param)
{
	if (!m_bCourseHover)
		return;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;
	const WRect& rect = m_pCourseClearArea->GetRect();
	float left = rect.x, top = rect.y;
	float width = m_pBitmap[1]->Width();
	float height = m_pBitmap[1]->Height();
	pGDI->DrawTexture(m_pBitmap[1], WRect(left, top, width, (float)height),
		0xffffffff, 0);
	float x = 0, y = 0;
	for (int i = 0; i < 15; ++i)
	{
		if (m_bCourseComplete[i])
		{
			CalcOffset(i, x, y);
			m_courseSrcRect = WRect(x, y, 65.0f, 11.0f);
			pGDI->DrawTexture(m_pBitmap[2], m_courseSrcRect,
				WRect(x + rect.x + 15.0f, y + rect.y + 30.0f, 65.0f, 11.0f),
				0xffffffff, 0);
		}
	}
	unsigned long color = pGDI->GetTextColor();
	char text[4] = { 0 };
	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);
	sprintf(text, "%d", m_completeCourseNum);
	pGDI->Print(WPoint(m_textPos[4].x, m_textPos[4].y), 0, "%s", text);
	pGDI->SetTextColor(color, 0xffffffff);
}

void FrMissionEventDlg::OnHoverOnCourseClearArea(int param)
{
	FrArea* pArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (pArea)
		m_bCourseHover = true;
}

void FrMissionEventDlg::OnHoverOffCourseClearArea(int param)
{
	FrArea* pArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (pArea)
		m_bCourseHover = false;
}

IMPLEMENT_OBJECT(FrMissionDetail, FrForm)

BEGIN_FRESH_MSGMAP(FrMissionDetail, FrForm)

ON_FRESH_VI("detailview", FRCMD_INIT, FrMissionDetail::OnInitExplaneViewer)

END_FRESH_MSGMAP()

FrMissionDetail::FrMissionDetail()
{
	m_pExplaneViewer = NULL;
}

FrMissionDetail::~FrMissionDetail()
{
}

void FrMissionDetail::OnInitExplaneViewer(int param)
{
	m_pExplaneViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pExplaneViewer)
		m_pExplaneViewer->Open("missionevent_explanation.jpg");
}
