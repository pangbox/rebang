#include "approachresultdlg.h"
#include "shareddoc.h"
#include "golfdoc.h"
#include "exhibition.h"
#include "netresourcemanager.h"
#include "projectg.h"
#include "wfont.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrApproachResultDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrApproachResultDlg, FrForm)

ON_FRESH_VI("left_area", FRCMD_INIT, FrApproachResultDlg::OnLeftAreaInit)
ON_FRESH_VI("left_area", FRCMD_OWNERDRAW,
	FrApproachResultDlg::OnLeftAreaOwnerDraw)
ON_FRESH_VI("left_list", FRCMD_INIT, FrApproachResultDlg::OnLeftListInit)
ON_FRESH_VI("left_list", FRCMD_OWNERDRAW,
	FrApproachResultDlg::OnLeftListOwnerDraw)
ON_FRESH_VI("right_area", FRCMD_INIT, FrApproachResultDlg::OnRightAreaInit)
ON_FRESH_VI("right_area", FRCMD_OWNERDRAW,
	FrApproachResultDlg::OnRightAreaOwnerDraw)
ON_FRESH_VI("right_list", FRCMD_INIT, FrApproachResultDlg::OnRightListInit)
ON_FRESH_VI("right_list", FRCMD_OWNERDRAW,
	FrApproachResultDlg::OnRightListOwnerDraw)

END_FRESH_MSGMAP()

bool ApproachResultCompare(const void* a, const void* b)
{
	if (((const sApproachResultData*)a)->byRank <
		((const sApproachResultData*)b)->byRank)
		return true;
	return false;
}

FrApproachResultDlg::FrApproachResultDlg()
{
	ClearVariables();
}

FrApproachResultDlg::~FrApproachResultDlg()
{
	m_pSmallFont->Erase();
	m_pBigFont->Erase();
	if (g_resrcmng && m_pSmallFont)
	{
		g_resrcmng->Release(m_pSmallFont);
		m_pSmallFont = NULL;
	}
	if (g_resrcmng && m_pBigFont)
	{
		g_resrcmng->Release(m_pBigFont);
		m_pBigFont = NULL;
	}
	if (m_pPartTidList)
	{
		delete m_pPartTidList;
		m_pPartTidList = NULL;
	}
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
}

bool FrApproachResultDlg::OnInit()
{
	return FrForm::OnInit();
}

void FrApproachResultDlg::OnProc(float delta)
{
	FrForm::OnProc(delta);
	if (m_pExhibition)
		m_pExhibition->Process(delta, true);
	else
		ShowPet();
}

void FrApproachResultDlg::ClearVariables()
{
	(void)&g_HUGE;
	(void)&g_EPSILON;
	static char* s_smallFont[] = { "[font_wind.jpg" };
	static char* s_bigFont[] = { "[bigfont.jpg" };
	m_pLeftArea = NULL;
	m_pLeftList = NULL;
	m_pBack = NULL;
	m_pTab1 = NULL;
	m_pTab2 = NULL;
	m_pExhibition = NULL;
	m_pPartTidList = NULL;
	m_pRightList = NULL;
	m_pRightArea = NULL;
	m_pBigFont = NULL;
	m_pSmallFont = NULL;
	WTITLEFONT big;
	memset(&big, 0, sizeof(big));
	big.filename = s_bigFont;
	big.fontw = 32;
	big.fonth = 32;
	big.numPages = 1;
	big.texw = 256;
	big.texh = 128;
	big.pCharSet = "1234567890ym%-./x ";
	m_pBigFont = g_resrcmng->GetTitleFont();
	m_pBigFont->Create(&big);
	WTITLEFONT smallInfo;
	memset(&smallInfo, 0, sizeof(smallInfo));
	smallInfo.filename = s_smallFont;
	smallInfo.fontw = 16;
	smallInfo.fonth = 16;
	smallInfo.numPages = 1;
	smallInfo.texw = 128;
	smallInfo.texh = 64;
	smallInfo.pCharSet = "1234567890ym%-.?/:";
	m_pSmallFont = g_resrcmng->GetTitleFont();
	m_pSmallFont->Create(&smallInfo);
	char title[32] = "\0";
	char box[32] = "\0";
	switch (GetCurMap())
	{
	case 2:
	case 3:
	case 11:
	case 16:
	case 19:
		strcpy(title, "th_box_1");
		strcpy(box, "treasurebox_a");
		break;
	case 0:
	case 1:
	case 6:
	case 7:
		strcpy(title, "th_box_2");
		strcpy(box, "treasurebox_b");
		break;
	case 10:
	case 13:
	case 15:
		strcpy(title, "th_box_3");
		strcpy(box, "treasurebox_c");
		break;
	case 4:
	case 5:
	case 8:
	case 9:
	case 14:
		strcpy(title, "th_box_4");
		strcpy(box, "treasurebox_d");
		break;
	}
	m_pRankGold = g_pFresh->GetBitmap("rank_gold");
	m_pRankSilver = g_pFresh->GetBitmap("rank_silver");
	m_pRankBronze = g_pFresh->GetBitmap("rank_bronze");
	m_pGoodLuck = g_pFresh->GetBitmap("prize_goodluck");
	m_pTreasureBox = g_pFresh->GetBitmap(box);
	m_pTreasureTitle = g_pFresh->GetBitmap(title);
	m_bApproachEnd = false;
}

void FrApproachResultDlg::OnLeftAreaInit(int param)
{
	m_pLeftArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (g_view->GetWidth() < 700.0f)
	{
		m_pLeftArea->SetVisible(false);
		m_pLeftArea->Enable(false);
	}
	else
	{
		char name[64];
		sprintf(name, "30in_back_%s",
			(Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->Data
									   : NULL));
		m_pBack = g_pFresh->GetBitmap(name);
		m_pTab1 = g_pFresh->GetBitmap("approach_tab1");
		m_pTab2 = g_pFresh->GetBitmap("approach_tab2");
	}
}

void FrApproachResultDlg::OnLeftAreaOwnerDraw(int)
{
	if (g_view->GetWidth() < 700.0f)
		return;
	{
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		if (device)
		{
			char rank[4];
			WRect area = m_rect;
			const Bitmap* line = g_pFresh->GetBitmap("approach_line");
			if (line)
			{
				WRect rect(area.x + 363.0f, area.y + 2.0f,
					(float)(float)line->Width(), 315.0f);
				device->DrawTexture(line, rect, 0xffffffff, 0);
			}
			if (m_pBack)
			{
				device->DrawTexture(m_pBack,
					WRect(area.x + 2.0f, area.y + 2.0f, (float)m_pBack->Width(),
						(float)m_pBack->Height()),
					0xffffffff, 0);
			}
			if (m_pTab1 && m_pTab2)
			{
				device->DrawTexture(m_pTab1,
					WRect(area.x + 2.0f, area.y + 231.0f,
						(float)m_pTab1->Width(), (float)m_pTab2->Height()),
					0xffffffff, 0);
				device->DrawTexture(m_pTab2,
					WRect(area.x + 258.0f, area.y + 231.0f,
						(float)m_pTab2->Width(), (float)m_pTab2->Height()),
					0xffffffff, 0);
			}
			sApproachResultData* data = NULL;
			std::list<sApproachResultData*>& results =
				Doc()->m_pGolfDoc->GetApproachCurHoleResult();
			for (std::list<sApproachResultData*>::iterator it = results.begin();
				it != results.end(); ++it)
			{
				data = *it;
				if (data)
				{
					if (data->byRank == 1)
						break;
					data = NULL;
				}
			}
			if (data)
			{
				if (!Doc()->m_userInfoMod &&
					(!Doc()->IsCurrentMissionBlind() ||
						(Doc()->IsCurrentMissionBlind() &&
							data->dwGUID == Doc()->m_myInfo.info.dwGuid)) &&
					m_pExhibition)
					m_pExhibition->Display(0.0f, 0.0f);
				sRivalData* rival =
					&Doc()->m_rivalList[Doc()->GetIndex(data->dwGUID)];
				int space = device->SetSpace(0);
				device->SetTextStyle(1);
				device->Print(WPoint(area.x + 210.0f, area.y + 56.0f), 0,
					"\xbc\xf8    \xc0\xa7");
				if (!Doc()->IsCurrentMissionBlind() ||
					(Doc()->IsCurrentMissionBlind() &&
						data->dwGUID == Doc()->m_myInfo.info.dwGuid))
				{
					device->Print(WPoint(area.x + 210.0f, area.y + 93.0f), 0,
						"\xb4\xeb \xc8\xad \xb8\xed");
					device->Print(WPoint(area.x + 210.0f, area.y + 118.0f), 0,
						"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae");
					device->Print(WPoint(area.x + 210.0f, area.y + 143.0f), 0,
						"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3");
				}
				device->Print(WPoint(area.x + 210.0f, area.y + 170.0f), 0,
					"\xc8\xb9\xb5\xe6\xba\xb8\xbb\xf3");
				device->SetTextStyle(0);
				memcpy(rank, "\0", 2);
				memset(rank + 2, 0, 2);
				int time = data->dwRemainTime;
				unsigned int distance = data->dwRemainDistance;
				itoa(data->byRank, rank, 10);
				m_pBigFont->Print(g_view, area.x + 280.0f, area.y + 48.0f, rank,
					0, 0xffffffff, 0);
				if (!Doc()->IsCurrentMissionBlind() ||
					(Doc()->IsCurrentMissionBlind() &&
						data->dwGUID == Doc()->m_myInfo.info.dwGuid))
				{
					g_pFresh->GetManager()->PrintText11(
						WPoint(area.x + 280.0f, area.y + 93.0f), 0,
						rival->nickname, -1.0f, 0xffffffff);
					if (distance == -1)
					{
						device->Print(WPoint(area.x + 280.0f, area.y + 118.0f),
							0, "out");
						device->Print(WPoint(area.x + 280.0f, area.y + 143.0f),
							0, "-");
					}
					else
					{
						device->Print(WPoint(area.x + 280.0f, area.y + 118.0f),
							0, "%d.%d y", distance / 10, distance % 10);
						device->Print(WPoint(area.x + 280.0f, area.y + 143.0f),
							0, "%d.%03d \xc3\xca", time / 1000, time % 1000);
					}
				}
				int boxes =
					data->dwPrizeCount + data->byRankPrize + data->byLuckPrize;
				if (m_pTreasureTitle)
				{
					device->DrawTexture(m_pTreasureTitle,
						WRect(area.x + 280.0f, area.y + 178.0f,
							(float)m_pTreasureTitle->Width(),
							(float)m_pTreasureTitle->Height()),
						0xffffffff, 0);
				}
				char number[3] = "\0";
				itoa(boxes, number, 10);
				m_pSmallFont->Print(g_view, area.x + 280.0f, area.y + 170.0f,
					number, 0, 0xffffffff, 0);
				device->SetSpace(space);
			}
		}
	}
}

void FrApproachResultDlg::OnLeftListInit(int param)
{
	m_pLeftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	m_pLeftList->ClearItem();
	std::list<sApproachResultData*>& results =
		Doc()->m_pGolfDoc->GetApproachCurHoleResult();
	for (std::list<sApproachResultData*>::iterator it = results.begin();
		it != results.end(); ++it)
	{
		sApproachResultData* data = *it;
		if (data && data->byRank <= 3 && data->byRank > 1)
			m_pLeftList->AddItem(data);
	}
	m_pLeftList->SortItem(ApproachResultCompare);
}

void FrApproachResultDlg::OnLeftListOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device || !param)
		return;
	FrListItem* item = (FrListItem*)param;
	sApproachResultData* data = (sApproachResultData*)item->pData;
	if (!data)
		return;
	sRivalData* rival = &Doc()->m_rivalList[Doc()->GetIndex(data->dwGUID)];
	WPoint base = item->pos;
	float y;
	const Bitmap* frame = data->byRank <= 3 ? g_pFresh->GetBitmap("rank_frm_1")
											: g_pFresh->GetBitmap("rank_frm_2");
	if (frame)
	{
		WRect rect(base.x + 20.0f - (float)frame->Width() * 0.5f,
			base.y + 6.0f - 2.0f, (float)frame->Width(),
			(float)frame->Height());
		device->DrawTexture(frame, rect, 0xffffffff, 0);
	}
	device->SetTextStyle(1);
	device->SetTextColor(0xffffffff, 0xffffffff);
	{
		WPoint rankPos;
		rankPos.x = base.x + 20.0f;
		y = base.y + 6.0f;
		rankPos.y = y;
		if (data->byRank != 255)
			device->Print(rankPos, 1, "%d", data->byRank);
		else
		{
			if (!data->bExit)
				device->Print(rankPos, 1, "-");
			else
				device->Print(rankPos, 1, "x");
		}
	}
	device->SetTextColor(0xff000000, 0xffffffff);
	if (data->dwGUID == Doc()->m_myInfo.info.dwGuid)
		device->SetTextColor(0xffff0000, 0xffffffff);
	else
		device->SetTextStyle(0);
	if (!Doc()->IsCurrentMissionBlind() ||
		(Doc()->IsCurrentMissionBlind() &&
			data->dwGUID == Doc()->m_myInfo.info.dwGuid))
	{
		if (rival->guildUID)
		{
			const Bitmap* emblem =
				NetResManager()->GetEmblemByName(rival->guildMark);
			if (emblem)
			{
				WRect rect(base.x + 40.0f, y - 7.0f, (float)emblem->Width(),
					(float)emblem->Height());
				device->DrawTexture(emblem, rect, 0xffffffff, 0);
			}
		}
		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText11(WPoint(base.x + 70.0f, y), 0,
				rival->nickname, -1.0f, 0xffffffff);
		if (data->dwRemainTime == -1)
		{
			device->Print(WPoint(base.x + 198.0f, y), 2, "Out");
			device->Print(WPoint(base.x + 252.0f, y), 2, "-");
		}
		else
		{
			if (rival->approachResultDistance == -1)
				device->Print(WPoint(base.x + 198.0f, y), 2, "Out");
			else
				device->Print(WPoint(base.x + 198.0f, y), 2, "%d.%dy",
					data->dwRemainDistance / 10, data->dwRemainDistance % 10);
			device->Print(WPoint(base.x + 266.0f, y), 2, "%d.%03d",
				data->dwRemainTime / 1000, data->dwRemainTime % 1000);
		}
	}
	device->Print(WPoint(base.x + 327.0f, y), 2, "%d",
		data->dwPrizeCount + data->byRankPrize + data->byLuckPrize);
	device->SetTextStyle(0);
}

void FrApproachResultDlg::OnRightAreaInit(int param)
{
	m_pRightArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pTab3 = g_pFresh->GetBitmap("approach_tab3");
	m_pTab4 = g_pFresh->GetBitmap("approach_tab4");
}

void FrApproachResultDlg::OnRightAreaOwnerDraw(int)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (device)
	{
		WRect area = m_rect;
		if (m_pTab3)
			device->DrawTexture(m_pTab3,
				WRect(area.x + 365.0f, area.y + 2.0f, m_pTab3->Width(),
					m_pTab3->Height()),
				0xffffffff, 0);
		if (m_pTab4)
			device->DrawTexture(m_pTab4,
				WRect(area.x + 621.0f, area.y + 2.0f, m_pTab4->Width(),
					m_pTab4->Height()),
				0xffffffff, 0);
	}
}

void FrApproachResultDlg::OnRightListInit(int param)
{
	m_pRightList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	m_pRightList->ClearItem();
	std::list<sApproachResultData*>& results =
		Doc()->m_pGolfDoc->GetApproachCurHoleResult();
	for (std::list<sApproachResultData*>::iterator it = results.begin();
		it != results.end(); ++it)
	{
		sApproachResultData* data = *it;
		if (data)
			m_pRightList->AddItem(data);
	}
	m_pRightList->SortItem(ApproachResultCompare);
}

void FrApproachResultDlg::OnRightListOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device || !param)
		return;
	FrListItem* item = (FrListItem*)param;
	sApproachResultData* data = (sApproachResultData*)item->pData;
	if (!data)
		return;
	sRivalData* rival = &Doc()->m_rivalList[Doc()->GetIndex(data->dwGUID)];
	WPoint base = item->pos;
	float y;
	const Bitmap* frame = data->byRank <= 3 ? g_pFresh->GetBitmap("rank_frm_1")
											: g_pFresh->GetBitmap("rank_frm_2");
	if (frame)
	{
		WRect rect(base.x + 7.0f - (float)frame->Width() * 0.5f,
			base.y + 9.0f - 2.0f, (float)frame->Width(),
			(float)frame->Height());
		device->DrawTexture(frame, rect, 0xffffffff, 0);
	}
	device->SetTextStyle(1);
	device->SetTextColor(0xffffffff, 0xffffffff);
	{
		WPoint rankPos;
		rankPos.x = base.x + 7.0f;
		y = base.y + 9.0f;
		rankPos.y = y;
		if (data->byRank != 255)
			device->Print(rankPos, 1, "%d", data->byRank);
		else
		{
			if (!data->bExit)
				device->Print(rankPos, 1, "-");
			else
				device->Print(rankPos, 1, "x");
		}
	}
	device->SetTextColor(0xff000000, 0xffffffff);
	if (data->dwGUID == Doc()->m_myInfo.info.dwGuid)
		device->SetTextColor(0xffff0000, 0xffffffff);
	else
		device->SetTextStyle(0);
	if (!Doc()->IsCurrentMissionBlind() ||
		(Doc()->IsCurrentMissionBlind() &&
			data->dwGUID == Doc()->m_myInfo.info.dwGuid))
	{
		if (rival->guildUID)
		{
			const Bitmap* emblem =
				NetResManager()->GetEmblemByName(rival->guildMark);
			if (emblem)
			{
				WRect rect(base.x + 20.0f, y - 6.0f, (float)emblem->Width(),
					(float)emblem->Height());
				device->DrawTexture(emblem, rect, 0xffffffff, 0);
			}
		}
		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText11(WPoint(base.x + 48.0f, y), 0,
				rival->nickname, -1.0f, 0xffffffff);
		if (data->dwRemainDistance == -1)
		{
			device->Print(WPoint(base.x + 163.0f, y), 2, "Out");
			device->Print(WPoint(base.x + 208.0f, y), 2, "-");
		}
		else
		{
			device->Print(WPoint(base.x + 168.0f, y), 2, "%d.%dy",
				data->dwRemainDistance / 10, data->dwRemainDistance % 10);
			device->Print(WPoint(base.x + 222.0f, y), 2, "%d.%03d",
				data->dwRemainTime / 1000, data->dwRemainTime % 1000);
		}
	}
	char number[3] = "\0";
	if (data->dwPrizeCount > 0)
	{
		if (m_pTreasureBox)
		{
			device->DrawTexture(m_pTreasureBox,
				WRect(base.x + 213.0f, y - 5.0f, (float)m_pTreasureBox->Width(),
					(float)m_pTreasureBox->Height()),
				0xffffffff, 0);
		}
		itoa(data->dwPrizeCount, number, 10);
		if (m_pSmallFont)
			m_pSmallFont->Print(g_view, base.x + 230.0f, y + 1.0f, number, 0,
				0xffffffff, 0);
	}
	if (data->byRankPrize > 0)
	{
		const Bitmap* medal = NULL;
		switch (data->byRank)
		{
		case 1:
			medal = m_pRankGold;
			break;
		case 2:
			medal = m_pRankSilver;
			break;
		case 3:
			medal = m_pRankBronze;
			break;
		}
		if (medal)
		{
			WRect rect(base.x + 247.0f, y - 3.5f, (float)medal->Width(),
				(float)medal->Height());
			device->DrawTexture(medal, rect, 0xffffffff, 0);
		}
		itoa(data->byRankPrize, number, 10);
		if (m_pSmallFont)
			m_pSmallFont->Print(g_view, base.x + 248.0f, y + 1.0f, number, 0,
				0xffffffff, 0);
	}
	if (data->byLuckPrize > 0)
	{
		if (m_pGoodLuck)
		{
			device->DrawTexture(m_pGoodLuck,
				WRect(base.x + 259.0f, y - 5.0f, (float)m_pGoodLuck->Width(),
					(float)m_pGoodLuck->Height()),
				0xffffffff, 0);
		}
		itoa(data->byLuckPrize, number, 10);
		if (m_pSmallFont)
			m_pSmallFont->Print(g_view, base.x + 263.0f, y + 1.0f, number, 0,
				0xffffffff, 0);
	}
	device->SetTextStyle(0);
	if (data->dwGUID == Doc()->m_myInfo.info.dwGuid)
		device->SetTextColor(0, 0xffffffff);
}

void FrApproachResultDlg::SetApproachEnd(bool end)
{
	m_bApproachEnd = end;
}

bool FrApproachResultDlg::GetApproachEnd() const
{
	return m_bApproachEnd;
}

void FrApproachResultDlg::ShowPet()
{
	if (m_pBack)
	{
		sApproachResultData* data = NULL;
		std::list<sApproachResultData*>& results =
			Doc()->m_pGolfDoc->GetApproachCurHoleResult();
		for (std::list<sApproachResultData*>::iterator it = results.begin();
			it != results.end(); ++it)
		{
			data = *it;
			if (data)
			{
				if (data->byRank == 1)
					break;
				data = NULL;
			}
		}
		if (data)
		{
			sRivalData* rival =
				&Doc()->m_rivalList[Doc()->GetIndex(data->dwGUID)];
			if (!Doc()->IsCurrentMissionBlind() || rival->oid == MyGuid(false))
			{
				std::map<unsigned long, sUserInfoTime>::iterator it;
				*(volatile unsigned long*)&it = 0;
				if (Doc()->FindUserInfoTimeUID(
						*(const volatile unsigned long*)&rival->uid, it))
				{
					if (!Doc()->m_userInfoMod)
					{
						WRect origin = m_rect;
						WRect area(origin.x + 5.0f, origin.y - 25.0f,
							m_pBack->Width(), m_pBack->Height());
						m_pExhibition = new CExhibition(area.x + area.w * 0.5f,
							area.y + area.h * 0.5f, 0.33f, false);
						m_pExhibition->SetDistance(g_view->GetWidth() * 0.0275f,
							true, false);
						m_pExhibition->SetArea(area.w, area.h);
						m_pExhibition->SetControlFlag(16);
						m_pPartTidList = new CPartTidList;
						if (!m_pPartTidList->SetTids(&it->second.info.charInfo,
								255) ||
							!m_pPartTidList->IsComboValid())
							m_pPartTidList->SetDefaultTids();
						char motion[64] = "\0";
						strcpy(motion, "1\xb5\xee\xb8\xf0\xbc\xc7");
						m_pExhibition->SetModel(*m_pPartTidList,
							it->second.info.charInfo.tidAuxParts, motion, 0.0f,
							0);
					}
				}
			}
		}
	}
}

IMPLEMENT_OBJECT(FrApproachEndDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrApproachEndDlg, FrForm)

ON_FRESH_VI("draw_area", FRCMD_INIT, FrApproachEndDlg::OnDrawAreaInit)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW, FrApproachEndDlg::OnDrawAreaOwnerDraw)
ON_FRESH_VI("app_list", FRCMD_INIT, FrApproachEndDlg::OnListInit)
ON_FRESH_VI("app_list", FRCMD_OWNERDRAW, FrApproachEndDlg::OnListOwnerDraw)

END_FRESH_MSGMAP()

FrApproachEndDlg::FrApproachEndDlg()
	: m_pDrawArea(NULL),
	  m_pList(NULL),
	  m_pResult1(NULL),
	  m_pResult2(NULL),
	  m_pResult3(NULL),
	  m_pResult4(NULL)
{
}

FrApproachEndDlg::~FrApproachEndDlg()
{
}

void FrApproachEndDlg::OnDrawAreaInit(int param)
{
	m_pDrawArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pDrawArea)
	{
		m_pResult1 = g_pFresh->GetBitmap("approach_result1");
		m_pResult2 = g_pFresh->GetBitmap("approach_result2");
		m_pResult3 = g_pFresh->GetBitmap("approach_result3");
		m_pResult4 = g_pFresh->GetBitmap("approach_result4");
	}
}

void FrApproachEndDlg::OnDrawAreaOwnerDraw(int)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (device)
	{
		WRect area = m_rect;
		if (m_pResult1)
			device->DrawTexture(m_pResult1,
				WRect(area.x + 2.0f, area.y + 23.0f, m_pResult1->Width(),
					m_pResult1->Height()),
				0xffffffff, 0);
		if (m_pResult2)
			device->DrawTexture(m_pResult2,
				WRect(area.x + 258.0f, area.y + 23.0f, m_pResult2->Width(),
					m_pResult2->Height()),
				0xffffffff, 0);
		if (m_pResult3)
			device->DrawTexture(m_pResult3,
				WRect(area.x + 12.0f, area.y + 313.0f, m_pResult3->Width(),
					m_pResult3->Height()),
				0xffffffff, 0);
		if (m_pResult4)
			device->DrawTexture(m_pResult4,
				WRect(area.x + 268.0f, area.y + 313.0f, m_pResult4->Width(),
					m_pResult4->Height()),
				0xffffffff, 0);
	}
}

void FrApproachEndDlg::OnListInit(int param)
{
	m_pList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pList)
	{
		m_pList->ClearItem();
		std::list<sApproachResultData*>& results =
			Doc()->m_pGolfDoc->GetApproachGameResult();
		for (std::list<sApproachResultData*>::iterator it = results.begin();
			it != results.end(); ++it)
		{
			sApproachResultData* data = *it;
			if (data && (data->byRank > 0 || data->byRank <= 30))
				m_pList->AddItem(data);
		}
		m_pList->SortItem(ApproachResultCompare);
	}
}

void FrApproachEndDlg::OnListOwnerDraw(int param)
{
	if (m_pList)
	{
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		if (device && param)
		{
			FrListItem* item = (FrListItem*)param;
			sApproachResultData* data = (sApproachResultData*)item->pData;
			if (data)
			{
				WRect area = m_rect;
				WPoint base = item->pos;
				float offset = 4.0f;
				for (int i = 0; i < 2; ++i)
				{
					sRivalData* rival =
						&Doc()->m_rivalList[Doc()->GetIndex(data->dwGUID)];
					const Bitmap* frame = data->byRank <= 3
						? g_pFresh->GetBitmap("rank_frm_1")
						: g_pFresh->GetBitmap("rank_frm_2");
					if (frame)
					{
						WRect rect(base.x + 22.0f -
								(float)frame->Width() * 0.5f,
							base.y + offset - 2.0f, (float)frame->Width(),
							(float)frame->Height());
						device->DrawTexture(frame, rect, 0xffffffff, 0);
					}
					device->SetTextStyle(1);
					device->SetTextColor(0xffffffff, 0xffffffff);
					float y;
					if (data->byRank < 255)
					{
						WPoint pos;
						pos.x = base.x + 22.0f;
						pos.y = y = base.y + offset;
						device->Print(pos, 1, "%d", data->byRank);
					}
					else
					{
						WPoint pos;
						pos.x = base.x + 22.0f;
						pos.y = y = base.y + offset;
						device->Print(pos, 1, "-");
					}
					device->SetTextColor(0xff000000, 0xffffffff);
					if (data->dwGUID == Doc()->m_myInfo.info.dwGuid)
						device->SetTextColor(0xffff0000, 0xffffffff);
					else
						device->SetTextStyle(0);
					if (rival)
					{
						if (rival->guildUID)
						{
							const Bitmap* emblem =
								NetResManager()->GetEmblemByName(
									rival->guildMark);
							if (emblem)
							{
								WRect rect(base.x + 33.0f, y - 5.0f,
									(float)emblem->Width(),
									(float)emblem->Height());
								device->DrawTexture(emblem, rect, 0xffffffff,
									0);
							}
						}
						if (!CProjectG::Instance()->HidePrivacy())
							g_pFresh->GetManager()->PrintText11(
								WPoint(base.x + 60.0f, y), 0, rival->nickname,
								-1.0f, 0xffffffff);
					}
					unsigned int distance = data->dwRemainDistance;
					int time = data->dwRemainTime;
					if (distance == -1)
					{
						device->Print(WPoint(base.x + 152.0f, y), 0, "out");
						device->Print(WPoint(base.x + 212.0f, y), 0, "-");
					}
					else
					{
						device->Print(WPoint(base.x + 152.0f, y), 0, "%d.%d y",
							distance / 10, distance % 10);
						device->Print(WPoint(base.x + 212.0f, y), 0,
							"%d.%03d\xc3\xca", time / 1000, time % 1000);
					}
					device->Print(WPoint(base.x + 320.0f, y), 2, "%d",
						data->dwPrizeCount + data->byRankPrize +
							data->byLuckPrize);
					device->SetTextStyle(0);
					base.y = area.y;
					offset = 355.0f;
					if (data->dwGUID == Doc()->m_myInfo.info.dwGuid)
						device->SetTextColor(0, 0xffffffff);
					std::list<sApproachResultData*>& results =
						Doc()->m_pGolfDoc->GetApproachGameResult();
					std::list<sApproachResultData*>::iterator it =
						results.begin();
					if (it == results.end())
						break;
					while (true)
					{
						data = *it;
						if (data && data->dwGUID == Doc()->m_myInfo.info.dwGuid)
							break;
						++it;
						if (it == results.end())
							return;
					}
				}
			}
		}
	}
}

IMPLEMENT_OBJECT(FrAppTreasureGiftDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrAppTreasureGiftDlg, FrForm)

END_FRESH_MSGMAP()

FrAppTreasureGiftDlg::FrAppTreasureGiftDlg()
{
}

FrAppTreasureGiftDlg::~FrAppTreasureGiftDlg()
{
}
