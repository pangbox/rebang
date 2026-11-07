#include "scoreboard.h"
#include "projectg.h"
#include "golfdoc.h"
#include "scoredlg.h"
#include "levelupitemdlg.h"
#include "treasuredlg.h"
#include "intrusion.h"
#include "rival.h"
#include "simpleoverlay.h"
#include "netresourcemanager.h"
#include "partsinfo.h"
#include "frgaugebar.h"
#include "fredit.h"
#include "mathconsts.h"

#include "user_info.h"
#include "wbone.h"
#include "exhibition.h"
#include "treasure.h"
#include "voiceitem.h"
#include "headicon.h"
#include "../../shared/sharedtables.h"
#include "../../shared/gamerewardtable.h"
#include "quitwindow.h"
#include "chatmsg.h"
#include "capturedbg.h"
#include "inputmanager.h"
#include "clientsetting.h"
#include "wminmax.h"
#include "frdesktop.h"
#include "minatl.h"

extern Fresh* g_pFresh;

void ResizeScoreBar23(FrArea* bar2, FrArea* bar3, float x, float w);

const char* SCORE_PPL_DIR = "./score_ppl";
bool ApproachRankCompare(const void* a, const void* b)
{
	const sRivalData* pA = (const sRivalData*)a;
	const sRivalData* pB = (const sRivalData*)b;

	if (pA->quitOrder > pB->quitOrder)
		return true;

	if (pA->quitOrder < pB->quitOrder)
		return false;

	if (pA->approachDistance == pA->approachResultDistance &&
		pB->approachDistance != pB->approachResultDistance)
		return true;

	if (pA->approachDistance != pA->approachResultDistance &&
		pB->approachDistance == pB->approachResultDistance)
		return false;

	if (pA->approachDistance == pA->approachResultDistance &&
		pB->approachDistance == pB->approachResultDistance)
	{
		if (pA->approachResultDistance < pB->approachResultDistance)
			return true;

		if (pA->approachResultDistance > pB->approachResultDistance)
			return false;
	}

	if (pA->approachTime > pB->approachTime)
		return true;

	if (pA->approachTime < pB->approachTime)
		return false;

	return pA->order < pB->order;
}

bool RankCompare(const void* a, const void* b)
{
	const sRivalData* pA = (const sRivalData*)a;
	const sRivalData* pB = (const sRivalData*)b;

	if (pA->quitOrder > pB->quitOrder)
		return true;

	if (pA->quitOrder < pB->quitOrder)
		return false;

	if (pA->rank < pB->rank)
		return true;

	if (pA->rank > pB->rank)
		return false;

	if (pA->totalScore < pB->totalScore)
		return true;

	if (pA->totalScore > pB->totalScore)
		return false;

	if (pA->totalPang > pB->totalPang)
		return true;

	if (pA->totalPang < pB->totalPang)
		return false;

	return pA->order < pB->order;
}

IMPLEMENT_ACTOR(FrRankForm, FrForm)

BEGIN_FRESH_MSGMAP(FrRankForm, FrForm)
ON_FRESH_VI("list", FRCMD_INIT, FrRankForm::OnRankInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrRankForm::OnRankOwnerDraw)
ON_FRESH_VV("list", FRCMD_LBUTTONDOWN, FrRankForm::OnRankLBtnDown)
ON_FRESH_VV("list", FRCMD_RBUTTONUP, FrRankForm::OnRankRBtnUp)
ON_FRESH_VV("list", FRCMD_DBLCLICK, FrRankForm::OnRankDClick)
ON_FRESH_VV("center", FRCMD_LBUTTONUP, FrRankForm::OnCenterBtnUp)
ON_FRESH_VV("info", FRCMD_LBUTTONUP, FrRankForm::OnInfoBtnUp)
ON_FRESH_VV("size", FRCMD_LBUTTONUP, FrRankForm::OnSizeBtnUp)
ON_FRESH_VI("billboard", FRCMD_INIT, FrRankForm::OnBoardInit)
ON_FRESH_VI("bar01", FRCMD_INIT, FrRankForm::OnBar01Init)
ON_FRESH_VI("bar02", FRCMD_INIT, FrRankForm::OnBar02Init)
END_FRESH_MSGMAP()

FrRankForm::FrRankForm()
{
	m_pRankList = NULL;
	m_pBoard = NULL;
	m_firstOID = 0xffffffff;
	m_lastOID = 0xffffffff;
	m_selectedOID = 0xffffffff;
	m_noticeHole = 0;
	m_blinkCount = 0;
	memset(m_holePlayerNum, 0, sizeof(m_holePlayerNum));
}

void FrRankForm::OnProc(const float delta)
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))
		{
			if (g_input->GetDown("PAGE_DOWN", true))
				m_pRankList->GetScrollBar()->ScrollDown(1);
			else if (g_input->GetDown("PAGE_UP", true))
				m_pRankList->GetScrollBar()->ScrollUp(1);
		}
	}

	switch (m_captionType)
	{
	case 0:
		if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
		{
			m_pBoard->SetCaption(MakeStr(
				"RED %d\xc1\xa1/%I64d\xc6\xce  vs  BLUE %d\xc1\xa1/%I64d\xc6\xce",
				Doc()->m_teamResult[0].score, Doc()->m_teamResult[0].pang,
				Doc()->m_teamResult[1].score, Doc()->m_teamResult[1].pang));
		}
		else if (m_firstOID != 0xffffffff)
		{
			if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
			{
				m_pBoard->SetCaption(MakeStr(
					"\xc7\xf6\xc0\xe7 1\xc0\xa7 %s\xb4\xd4, \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae : %d.%d y",
					Doc()->m_rivalList[Doc()->GetIndex(m_firstOID)].nickname,
					Doc()->m_rivalList[Doc()->GetIndex(m_firstOID)]
							.approachResultDistance /
						10,
					Doc()->m_rivalList[Doc()->GetIndex(m_firstOID)]
							.approachResultDistance %
						10));
			}
			else
			{
				m_pBoard->SetCaption(MakeStr(
					"\xc7\xf6\xc0\xe7 1\xc0\xa7 %s\xb4\xd4, \xbd\xba\xc4\xda\xbe\xee %d",
					Doc()->m_rivalList[Doc()->GetIndex(m_firstOID)].nickname,
					Doc()
						->m_rivalList[Doc()->GetIndex(m_firstOID)]
						.totalScore));
			}
		}
		else
		{
			m_pBoard->SetCaption(MakeStr(
				"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc1\xdf\xc0\xce \xc0\xce\xbf\xf8\xc0\xba %d\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
				m_playerNum));
		}
		m_captionPriority = 4;
		break;
	case 2:
	{
		WRect rect;
		m_pBoard->GetClientRect(rect);
		rect.x -= delta * 30.0f;
		m_pBoard->SetClientRect(rect);
		if (rect.x + rect.w < g_view->GetWidth() - 239.0f)
		{
			++m_blinkCount;
			if (m_blinkCount == 2)
			{
				m_blinkCount = 0;
				SetCaptionType(0);
			}
			else
			{
				SetCaptionType(2);
			}
		}
		break;
	}
	case 1:
		m_captionTime += delta;
		if (m_captionTime > 10.0f)
		{
			m_captionTime = 0.0f;
			SetCaptionType(0);
		}
		break;
	}
}

void FrRankForm::OnRankInit(int param)
{
	m_pRankList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
		m_pRankList->AddItem(&(*it));

	m_pRankList->UseRightButton(true);

	m_playerNum = Doc()->m_rivalList.size();
	m_holePlayerNum[0] = Doc()->m_rivalList.size();

	SortRank();

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		m_sizeType = 2;
	else
		m_sizeType = 1;

	m_pMedal[0] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pMedal[1] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pMedal[2] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pMedal[3] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");

	m_pTeamRank[0] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pTeamRank[1] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");

	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");
}

int FrRankForm::GetMedalIndex(unsigned long oid)
{
	unsigned char intrusionCount;
	if (IsLocalContent((localContentType_t)0x2e))
		intrusionCount = CIntrusion::Instance()->GetCalcExpMemberSize();
	else
		intrusionCount = 0;

	if (intrusionCount == 0)
	{
		if (Doc()->m_rivalList.size() < 10)
			return 3;
	}
	else if (intrusionCount < 10)
	{
		return 3;
	}

	unsigned char index = Doc()->GetIndex(oid);
	if (index == 0xff || Doc()->m_rivalList[index].flag > 3)
		return 3;

	unsigned char rank = Doc()->m_rivalList[index].rank;
	unsigned char memberCount = 0;
	unsigned char upperCount = 0;

	if (intrusionCount == 0)
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if ((it->state == 3 && GetHoleIndex(it->hole) >= 4) ||
				it->state != 3)
				++memberCount;

			if (it->state != 3 && it->rank < rank && it->flag > 3)
				++upperCount;
		}
	}
	else
	{
		memberCount = intrusionCount;
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state != 3 && it->rank < rank && it->flag > 3)
				++upperCount;
		}
	}

	rank -= upperCount;

	if (Doc()->m_golfGame.holes == 9)
	{
		if (memberCount <= 14)
		{
			return 3;
		}
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 1;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	}
	else if (Doc()->m_golfGame.holes == 18)
	{
		if (memberCount <= 14)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

void FrRankForm::OnRankOwnerDraw(int param)
{
	if (m_sizeType == 0)
		return;

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect(pItem->pos.x + 7.0f, pItem->pos.y,
		(float)m_pRankList->GetItemWidth() - 10.0f,
		(float)m_pRankList->GetItemHeight());
	int space = pGDI->SetSpace(0);

	sRivalData* pRival = (sRivalData*)pItem->pData;

	if (m_selectedOID == pRival->oid)
		pGDI->Box(rect, 0x999b9dff, 0, 0.0f);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
	{
		if (m_pTeamRank[pRival->team])
			pGDI->DrawTexture(m_pTeamRank[pRival->team],
				WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f,
					(float)m_pTeamRank[pRival->team]->Width(),
					(float)m_pTeamRank[pRival->team]->Height()),
				0xffffffff, 0);

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d",
			pRival->rank);
	}
	else
	{
		if (m_pRank)
			pGDI->DrawTexture(m_pRank,
				WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f,
					(float)m_pRank->Width(), (float)m_pRank->Height()),
				0xffffffff, 0);

		if (pRival->state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1,
				"-");
		else if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH &&
			pRival->approachDistance == 0xffffffff)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1,
				"-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1,
				"%d", pRival->rank);
	}

	if (Doc()->m_golfGame.gameType != GAME_TYPE_NEW_APPROACH &&
		Doc()->m_roomInfo.nUserLimit < 100 && pRival->hole != GetStartHole())
	{
		int medal = GetMedalIndex(pRival->oid);

		if (medal >= 0 && medal <= 2)
		{
			if (pRival->flag > 3)
				medal = 3;

			pGDI->DrawTexture(m_pMedal[medal],
				WRect(pItem->pos.x + 2.0f, pItem->pos.y + 1.0f,
					(float)m_pMedal[medal]->Width(),
					(float)m_pMedal[medal]->Height()),
				0xffffffff, 0);
		}
	}

	if (pRival->oid == MyGuid(true))
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
			pGDI->SetTextColor(pRival->team ? 0xff0060ff : 0xffff0000,
				0xffffffff);
		else
			pGDI->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (pRival->guildUID)
	{
		const Bitmap* pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(pRival->guildMark);

		if (pEmblem)
		{
			pGDI->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 33.0f, pItem->pos.y + 3.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
		}
	}

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0,
			pRival->nickname, 90.0f, 0xffffffff);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		if (pRival->approachTime == -1)
		{
			if (pRival->approachDistance == pRival->approachResultDistance)
				pGDI->Print(WPoint(pItem->pos.x + 210.0f, pItem->pos.y + 3.0f),
					2, "Out");
			else
				pGDI->Print(WPoint(pItem->pos.x + 202.0f, pItem->pos.y + 3.0f),
					2, "-");
			pGDI->Print(WPoint(pItem->pos.x + 254.0f, pItem->pos.y + 3.0f), 2,
				"-");
		}
		else
		{
			if (pRival->approachDistance == pRival->approachResultDistance)
			{
				if (pRival->approachResultDistance == 0xffffffff)
					pGDI->Print(
						WPoint(pItem->pos.x + 210.0f, pItem->pos.y + 3.0f), 2,
						"Out");
				else
					pGDI->Print(
						WPoint(pItem->pos.x + 217.0f, pItem->pos.y + 3.0f), 2,
						"%d.%d y", pRival->approachResultDistance / 10,
						pRival->approachResultDistance % 10);
			}
			else if (pRival->oid == MyGuid(false) &&
				(GOLFDOC()->m_gameMode & 0x40))
				pGDI->Print(WPoint(pItem->pos.x + 202.0f, pItem->pos.y + 3.0f),
					2, "-");
			else
				pGDI->Print(WPoint(pItem->pos.x + 217.0f, pItem->pos.y + 3.0f),
					2, "%d.%d y", pRival->approachDistance / 10,
					pRival->approachDistance % 10);

			pGDI->Print(WPoint(pItem->pos.x + 273.0f, pItem->pos.y + 3.0f), 2,
				"%d.%03d", pRival->approachTime / 1000,
				pRival->approachTime % 1000);
		}
	}
	else
	{
		pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d",
			pRival->totalScore);
		pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2,
			"%I64d", pRival->totalPang);

		if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
				"End");
		else if (pRival->state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
				"Out");
		else
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
				"%d\xc8\xa6", GetHoleIndex(pRival->hole));

		if ((pRival->capability & 0x80) && m_pPCIcon)
			pGDI->DrawTexture(m_pPCIcon,
				WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f,
					(float)m_pPCIcon->Width(), (float)m_pPCIcon->Height()),
				0xffffffff, 0);
	}

	pGDI->SetSpace(space);
}

void FrRankForm::OnRankLBtnDown()
{
	FrListItem* item = m_pRankList->GetItemUnderCursor();
	if (item)
	{
		sRivalData* rival = (sRivalData*)item->pData;
		m_selectedOID = rival->oid;
	}
}

void FrRankForm::OnRankRBtnUp()
{
	FrListItem* item = m_pRankList->GetItemUnderCursor();
	if (item)
	{
		sRivalData* rival = (sRivalData*)item->pData;
		if (rival)
		{
			m_selectedOID = rival->oid;

			if (CUserInfo::Instance() && rival->state != 3)
				CUserInfo::Instance()->SetInfo(rival->uid, m_selectedOID, false,
					true, false, false, rival->nickname);
		}
	}
}

void FrRankForm::OnRankDClick()
{
	OnRankRBtnUp();
}

void FrRankForm::OnBoardInit(int param)
{
	m_pBoard = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pBoard->HidePrivacy(true);

	m_pBoard->SetCaption("");
	SetCaptionType(0);
	m_captionPriority = 4;

	m_captionTime = 0.0f;
}

void FrRankForm::OnBar01Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
			area->SetBgImg("approach_base_01");
	}
}

void FrRankForm::OnBar02Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
			area->SetBgImg("approach_base_02");
	}
}

void FrRankForm::OnCenterBtnUp()
{
	Align();
}

void FrRankForm::Align()
{
	if (m_sizeType)
	{
		int index = 0;
		FrListBox::ITEM_LIST::iterator it;
		for (it = m_pRankList->m_itemList.begin();
			it != m_pRankList->m_itemList.end(); ++it, ++index)
		{
			sRivalData* rival = (sRivalData*)(*it)->pData;
			if (rival->oid == MyGuid(true))
				break;
		}

		FrScrollBar* scroll = m_pRankList->GetScrollBar();
		if (scroll)
		{
			if (m_sizeType == 1)
			{
				int maxRow = Doc()->m_rivalList.size() - 5;
				int row = index - 2;
				scroll->SetCurTopRow(
					row < 0 ? 0 : (row > maxRow ? maxRow : row));
			}
			else
			{
				int maxRow = Doc()->m_rivalList.size() - 10;
				int row = index - 4;
				scroll->SetCurTopRow(
					row < 0 ? 0 : (row > maxRow ? maxRow : row));
			}
		}
	}
}

void FrRankForm::OnInfoBtnUp()
{
	std::map<unsigned long, sBriefUserInfo>::iterator it =
		Doc()->m_briefUserInfoMap.find(m_selectedOID);
	if (it != Doc()->m_briefUserInfoMap.end())
		CUserInfo::Instance()->SetInfo(it->second.dwUid, m_selectedOID, true,
			true, false, false, std::string(it->second.sNick));
}

void FrRankForm::OnSizeBtnUp()
{
	m_sizeType = (m_sizeType + 1) % 3;
	Resize();
}

void FrRankForm::Resize()
{
	if (m_pRankList)
	{
		WRect rect;
		m_pRankList->GetClientRect(rect);

		switch (m_sizeType)
		{
		case 0:
			rect.h = 0.0f;
			break;
		case 1:
			rect.h = 90.0f;
			break;
		case 2:
			rect.h = 180.0f;
			break;
		}

		m_pRankList->SetClientRect(rect);

		rect.h += 70.0f;
		rect.y = 0.0f;
		rect.w = m_pBaseFrm->GetRect().w;
		m_pBaseFrm->SetClientRect(rect);
	}
}

void FrRankForm::SortRank()
{
	int firstCount = 0;
	m_firstOID = m_lastOID = 0xffffffff;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state == 3)
			{
				it->rank = Doc()->m_rivalList.size() - it->quitOrder;
			}
			else
			{
				it->rank = 1;
				for (std::vector<sRivalData>::iterator it2 =
						 Doc()->m_rivalList.begin();
					it2 != Doc()->m_rivalList.end(); ++it2)
				{
					if (it == it2 || it2->state == 3)
						continue;

					if (it2->approachDistance != it2->approachResultDistance)
					{
						if (it->approachDistance == it->approachResultDistance)
							continue;

						if (it->approachTime < it2->approachTime)
							++it->rank;
					}
					else if (it->approachDistance !=
							it->approachResultDistance ||
						it->approachResultDistance >
							it2->approachResultDistance ||
						(it->approachResultDistance ==
								it2->approachResultDistance &&
							it->approachTime < it2->approachTime))
						++it->rank;
				}

				if (it->rank == 1)
				{
					if (it->approachDistance == it->approachResultDistance &&
						it->approachResultDistance != 0xffffffff)
					{
						++firstCount;
						m_firstOID = it->oid;
					}
				}
				else if (it->rank == m_playerNum)
					m_lastOID = it->oid;
			}
		}

		m_pRankList->SortItem(ApproachRankCompare);
	}
	else
	{
		memset(Doc()->m_teamResult, 0, sizeof(Doc()->m_teamResult));

		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state == 3)
			{
				it->rank = Doc()->m_rivalList.size() - it->quitOrder;
			}
			else
			{
				it->rank = 1;
				for (std::vector<sRivalData>::iterator it2 =
						 Doc()->m_rivalList.begin();
					it2 != Doc()->m_rivalList.end(); ++it2)
				{
					if (it != it2 && it2->state != 3)
					{
						if (it->totalScore > it2->totalScore ||
							(it->totalScore == it2->totalScore &&
								it->totalPang < it2->totalPang))
							++it->rank;
					}
				}

				if (it->rank == 1)
				{
					++firstCount;
					m_firstOID = it->oid;
				}
				else if (it->rank == m_playerNum)
					m_lastOID = it->oid;
			}

			Doc()->m_teamResult[it->team].score += it->rank;
			Doc()->m_teamResult[it->team].pang += it->totalPang;
		}

		m_pRankList->SortItem(RankCompare);
	}

	if (firstCount != 1)
		m_firstOID = 0xffffffff;
}

void FrRankForm::SetCaptionType(unsigned char type)
{
	m_captionType = type;

	if (m_pBoard)
	{
		WRect rect;
		m_pBoard->GetClientRect(rect);

		if (m_captionType == 2)
		{
			m_pBoard->SetClippingArea(&WRect(rect.x, rect.y, 219.0f, 16.0f));
			rect.x = 800.0f;
			rect.w = m_pBoard->GDI()->GetTextExtend(m_pBoard->GetCaption());
			m_pBoard->SetClientRect(rect);
		}
		else
		{
			m_pBoard->SetClippingArea(NULL);
			m_pBoard->SetClientRect(WRect(rect.x, rect.y, 219.0f, 16.0f));
		}
	}
}

IMPLEMENT_ACTOR(FrRankForm2, FrForm)

BEGIN_FRESH_MSGMAP(FrRankForm2, FrForm)
ON_FRESH_VI("list", FRCMD_INIT, FrRankForm2::OnRankInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrRankForm2::OnRankOwnerDraw)
END_FRESH_MSGMAP()

FrRankForm2::FrRankForm2()
	: m_pRankList(NULL)
{
}

void FrRankForm2::OnRankInit(int param)
{
	m_pRankList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
		m_pRankList->AddItem(&(*it));

	SortRank();

	m_pMedal[0] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pMedal[1] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pMedal[2] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pMedal[3] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");

	m_pTeamRank[0] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pTeamRank[1] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");

	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");
}

void FrRankForm2::SortRank()
{
	memset(Doc()->m_teamResult, 0, sizeof(Doc()->m_teamResult));

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if (it->state == 3)
		{
			it->rank = Doc()->m_rivalList.size() - it->quitOrder;
			continue;
		}

		it->rank = 1;
		for (std::vector<sRivalData>::iterator it2 = Doc()->m_rivalList.begin();
			it2 != Doc()->m_rivalList.end(); ++it2)
		{
			if (it == it2 || it2->state == 3)
				continue;

			if (it->totalScore > it2->totalScore ||
				(it->totalScore == it2->totalScore &&
					it->totalPang < it2->totalPang))
				it->rank++;
		}

		Doc()->m_teamResult[it->team].score += it->rank;
		Doc()->m_teamResult[it->team].pang += it->totalPang;
	}

	m_pRankList->SortItem(RankCompare);
}

int FrRankForm2::GetMedalIndex(unsigned long oid)
{
	if (Doc()->m_rivalList.size() < 10)
		return 3;

	unsigned char index = Doc()->GetIndex(oid);
	if (index == 0xff || Doc()->m_rivalList[index].flag > 3)
		return 3;

	unsigned char rank = Doc()->m_rivalList[index].rank;
	unsigned char memberCount = 0;
	unsigned char upperCount = 0;

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if ((it->state == 3 && GetHoleIndex(it->hole) >= 4) || it->state != 3)
		{
			memberCount++;

			if (it->state != 3 && it->rank < rank && it->flag > 3)
				upperCount++;
		}
	}

	rank -= upperCount;

	if (Doc()->m_golfGame.holes == 9)
	{
		if (memberCount <= 14)
			return 3;
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 1;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 1;
			if (rank == 2)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
		}
	}
	else if (Doc()->m_golfGame.holes == 18)
	{
		if (memberCount <= 14)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 1;
			if (rank == 2)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
			if (rank == 3)
				return 2;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
			if (rank <= 4)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			if (rank <= 3)
				return 1;
			if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

void FrRankForm2::OnRankOwnerDraw(int param)
{
	if (param == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	FrListItem* pItem = (FrListItem*)param;
	int space = pGDI->SetSpace(0);

	sRivalData* pRival = (sRivalData*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
	{
		if (m_pTeamRank[pRival->team])
			pGDI->DrawTexture(m_pTeamRank[pRival->team],
				WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f,
					(float)m_pTeamRank[pRival->team]->Width(),
					(float)m_pTeamRank[pRival->team]->Height()),
				0xffffffff, 0);

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d",
			pRival->rank);
	}
	else
	{
		if (m_pRank)
			pGDI->DrawTexture(m_pRank,
				WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f,
					(float)m_pRank->Width(), (float)m_pRank->Height()),
				0xffffffff, 0);

		if (pRival->state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1,
				"-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1,
				"%d", pRival->rank);
	}

	if (pRival->hole != GetStartHole())
	{
		int medal = GetMedalIndex(pRival->oid);

		if (medal >= 0 && medal <= 2)
		{
			if (pRival->flag > 3)
				medal = 3;

			pGDI->DrawTexture(m_pMedal[medal],
				WRect(pItem->pos.x + 2.0f, pItem->pos.y + 1.0f,
					(float)m_pMedal[medal]->Width(),
					(float)m_pMedal[medal]->Height()),
				0xffffffff, 0);
		}
	}

	if (pRival->oid == MyGuid(true))
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
			pGDI->SetTextColor(pRival->team ? 0xff0060ff : 0xffff0000,
				0xffffffff);
		else
			pGDI->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (pRival->guildUID)
	{
		const Bitmap* pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(pRival->guildMark);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 33.0f, pItem->pos.y - 3.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
	}

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0,
			pRival->nickname, 90.0f, 0xffffffff);

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d",
		pRival->totalScore);

	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2, "%I64d",
		pRival->totalPang);

	if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
			"End");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival->hole));

	if ((pRival->capability & 0x80) && m_pPCIcon)
		pGDI->DrawTexture(m_pPCIcon,
			WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f,
				(float)m_pPCIcon->Width(), (float)m_pPCIcon->Height()),
			0xffffffff, 0);

	pGDI->SetSpace(space);
}

IMPLEMENT_ACTOR(FrRankForm3, FrForm)

BEGIN_FRESH_MSGMAP(FrRankForm3, FrForm)
ON_FRESH_VI("list", FRCMD_INIT, FrRankForm3::OnRankInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrRankForm3::OnRankOwnerDraw)
ON_FRESH_VV("size", FRCMD_LBUTTONUP, FrRankForm3::OnSizeBtnUp)
ON_FRESH_VI("billboard", FRCMD_INIT, FrRankForm3::OnBoardInit)
ON_FRESH_VI("score_gb", FRCMD_INIT, FrRankForm3::OnScoreGaugeInit)
ON_FRESH_VI("score_info", FRCMD_INIT, FrRankForm3::OnScoreInfoInit)
ON_FRESH_VV("score_info", FRCMD_OWNERDRAW, FrRankForm3::OnScoreInfoOwnerDraw)
END_FRESH_MSGMAP()

FrRankForm3::FrRankForm3()
	: m_pRankList(NULL), m_pBoard(NULL), m_pScoreGauge(NULL), m_pScoreInfo(NULL)
{
	m_noticeHole = 0;
	m_blinkCount = 0;
	m_selectedOID = 0xffffffff;
	m_sizeType = 1;
	LoadBitmaps();
}

void FrRankForm3::OnProc(const float delta)
{
	switch (m_captionType)
	{
	case 0:
		m_pBoard->SetCaption(MakeStr(
			"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc1\xdf\xc0\xce \xc0\xce\xbf\xf8\xc0\xba %d\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
			m_playerNum));
		break;

	case 2:
	{
		WRect rect;
		m_pBoard->GetClientRect(rect);
		rect.x -= delta * 30.0f;
		m_pBoard->SetClientRect(rect);

		if ((rect.x + rect.w) < g_view->GetWidth() - 239.0f)
		{
			if (++m_blinkCount == 2)
			{
				m_blinkCount = 0;
				SetCaptionType(0);
			}
			else
				SetCaptionType(2);
		}
		break;
	}

	case 1:
		m_captionTime += delta;
		if (m_captionTime > 10.0f)
		{
			m_captionTime = 0.0f;
			SetCaptionType(0);
		}
		break;
	}
}

void FrRankForm3::OnRankInit(int param)
{
	m_pRankList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	for (std::vector<sGuildMatchup>::iterator it =
			 Doc()->m_guildMatchupList.begin();
		it != Doc()->m_guildMatchupList.end(); ++it)
	{
		m_pRankList->AddItem(it._Myptr);
	}

	m_pRankList->UseRightButton(false);

	m_playerNum = Doc()->m_guildMatchupList.size() * 2;

	m_pEmblem1 = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);
	m_pEmblem2 = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);

	m_pRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank_02");

	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");

	m_sizeType = (m_sizeType + 1) % 2;
	OnSizeBtnUp();
}

void FrRankForm3::OnRankOwnerDraw(int param)
{
	if (m_sizeType == 0)
		return;

	if (param == NULL)
		return;

	FrListItem* pItem = (FrListItem*)param;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	WRect rect(pItem->pos.x + 2.0f, pItem->pos.y, m_pRankList->GetItemWidth(),
		m_pRankList->GetItemHeight());
	int space = pGDI->SetSpace(0);

	sGuildMatchup* pMatchup = (sGuildMatchup*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);

	sRivalData* pRival1 =
		&Doc()->m_rivalList[Doc()->GetIndex(pMatchup->uid[0])];
	sRivalData* pRival2 =
		&Doc()->m_rivalList[Doc()->GetIndex(pMatchup->uid[1])];

	if (MyGuid(false) == pRival1->oid || MyGuid(false) == pRival2->oid)
	{
		m_pRankList->SelectItem(pItem, true);
		pGDI->Box(rect, 0x999b9dff);
	}

	if (m_pRank)
	{
		pGDI->DrawTexture(m_pRank,
			WRect(pItem->pos.x + 5.0f, pItem->pos.y + 8.0f,
				(float)m_pRank->Width(), (float)m_pRank->Height()),
			0xffffffff, 0);
		pGDI->Print(WPoint(pItem->pos.x + 18.0f, pItem->pos.y + 12.0f), 1,
			"%d \xc1\xb6", pMatchup->team);
	}

	if (m_pEmblem1)
		pGDI->DrawTexture(m_pEmblem1,
			WRect(pItem->pos.x + 39.0f, pItem->pos.y + 3.0f,
				(float)(m_pEmblem1->Width() / 2),
				(float)(m_pEmblem1->Height() / 2)),
			0xffffffff, 0);

	if (pRival1->oid == MyGuid(true))
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
	else if (pRival1->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (CProjectG::Instance()->HidePrivacy() == false)
	{
		pGDI->SetTextColor(0xffcd1836, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0,
			pRival1->nickname, 90.0f, 0xffffffff);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d",
		pRival1->guildPoint);
	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2, "%I64d",
		pRival1->totalPang);

	if (pRival1->state == 2 || GetHoleIndex(pRival1->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
			"End");
	else if (pRival1->state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
			"Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival1->hole));

	if ((pRival1->capability & 0x80) && m_pPCIcon)
		pGDI->DrawTexture(m_pPCIcon,
			WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f,
				(float)m_pPCIcon->Width(), (float)m_pPCIcon->Height()),
			0xffffffff, 0);

	if (m_pEmblem2)
		pGDI->DrawTexture(m_pEmblem2,
			WRect(pItem->pos.x + 39.0f, pItem->pos.y + 21.0f,
				(float)(m_pEmblem2->Width() / 2),
				(float)(m_pEmblem2->Height() / 2)),
			0xffffffff, 0);

	if (pRival2->oid == MyGuid(true))
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
	else if (pRival2->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (CProjectG::Instance()->HidePrivacy() == false)
	{
		pGDI->SetTextColor(0xff1d26ab, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 20.0f), 0,
			pRival2->nickname, 90.0f, 0xffffffff);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 20.0f), 2, "%d",
		pRival2->guildPoint);
	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 20.0f), 2, "%I64d",
		pRival2->totalPang);

	if (pRival2->state == 2 || GetHoleIndex(pRival2->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2,
			"End");
	else if (pRival2->state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2,
			"Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival2->hole));

	if ((pRival2->capability & 0x80) && m_pPCIcon)
		pGDI->DrawTexture(m_pPCIcon,
			WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f,
				(float)m_pPCIcon->Width(), (float)m_pPCIcon->Height()),
			0xffffffff, 0);

	WPoint start;
	WPoint end;
	start.x = pItem->pos.x + 1.0f;
	end.x = m_pRankList->GetItemWidth() + pItem->pos.x;
	start.y = end.y = m_pRankList->GetItemHeight() + pItem->pos.y;

	g_view->DrawLine2D(start, end, 0xff000000, 0xff000000, 0);

	pGDI->SetSpace(space);
}

void FrRankForm3::OnBoardInit(int param)
{
	m_pBoard = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pBoard->m_nFlags.Enable(0x1000);

	m_pBoard->SetCaption("");
	SetCaptionType(0);

	m_captionTime = 0.0f;
}

void FrRankForm3::OnScoreGaugeInit(int param)
{
	m_pScoreGauge = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pScoreGauge->SetRange(0, 10, 5);
}

void FrRankForm3::SetScore(int score1, int score2)
{
	m_score[0] = score1;
	m_score[1] = score2;

	int lower = score1 + 1;
	int upper = score2 + 1;

	m_pScoreGauge->SetRange(0, lower + upper, lower);
}

void FrRankForm3::DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
	unsigned long color, FrGDI::eTextAlign align, bool narrow)
{
	int len = strlen(text);
	int width = 0;
	const Bitmap* digits[16];
	if (align)
	{
		width = 0;
		for (int i = 0; i < len; ++i)
		{
			switch (text[i])
			{
			case '+':
				digits[i] = m_pPlus;
				break;
			case '-':
				digits[i] = m_pMinus;
				break;
			default:
				digits[i] = m_pDigit[text[i] - '0'];
				break;
			}
			width += digits[i]->Width() - (narrow ? (len < 4 ? 0 : 2) : 0);
		}
	}
	WRect rect;
	switch (align)
	{
	case FrGDI::ALIGN_LEFT:
		rect.x = (float)(int)pos.x;
		break;
	case FrGDI::ALIGN_CENTER:
		rect.x = (float)(int)(pos.x - width * 0.5f);
		break;
	case FrGDI::ALIGN_RIGHT:
		rect.x = (float)(int)(pos.x - width);
		break;
	}
	rect.y = pos.y;
	for (int i = 0; i < len; ++i)
	{
		rect.w = (float)digits[i]->Width();
		rect.h = (float)digits[i]->Height();
		gdi->DrawTexture(digits[i], rect, color, 0);
		rect.x += rect.w - (narrow ? (len < 4 ? 0 : 2) : 0);
	}
}

void FrRankForm3::OnSizeBtnUp()
{
	if (m_pRankList == NULL)
		return;

	m_sizeType = (m_sizeType + 1) % 2;

	WRect rect;
	m_pRankList->GetClientRect(rect);

	switch (m_sizeType)
	{
	case 0:
		rect.h = 0.0f;
		break;

	case 1:
		rect.h = m_pRankList->GetItemHeight() * 3.0f;
		break;
	}

	m_pRankList->SetClientRect(rect);

	rect.w = 296.0f;
	rect.y = 0.0f;
	rect.h += 113.0f;
	m_pBaseFrm->SetClientRect(rect);
}

void FrRankForm3::SetCaptionType(unsigned char type)
{
	m_captionType = type;
	if (m_pBoard == NULL)
		return;

	WRect rect;
	m_pBoard->GetClientRect(rect);

	if (m_captionType == 2)
	{
		WRect clip(rect.x, rect.y, 219.0f, 16.0f);
		m_pBoard->SetClippingArea(&clip);

		rect.x = 800.0f;
		rect.w = m_pBoard->GDI()->GetTextExtend(m_pBoard->GetCaption());
		m_pBoard->SetClientRect(rect);
	}
	else
	{
		m_pBoard->SetClippingArea(NULL);
		m_pBoard->SetClientRect(WRect(rect.x, rect.y, 219.0f, 16.0f));
	}
}

void FrRankForm3::OnScoreInfoInit(int param)
{
	m_pScoreInfo = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	m_pEmblem1 = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);
	m_pEmblem2 = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);

	memset(m_score, 0, sizeof(m_score));

	m_pScoreInfo->SetVisible(true);
}

void FrRankForm3::OnScoreInfoOwnerDraw()
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect = m_pScoreInfo->GetRect();

	if (m_pEmblem1)
		pGDI->DrawTexture(m_pEmblem1,
			WRect(rect.x + 10.0f, rect.y + 1.0f, (float)m_pEmblem1->Width(),
				(float)m_pEmblem1->Height()),
			0xffffffff, 0);

	if (m_pEmblem2)
		pGDI->DrawTexture(m_pEmblem2,
			WRect(rect.x + 260.0f, rect.y + 1.0f, (float)m_pEmblem2->Width(),
				(float)m_pEmblem2->Height()),
			0xffffffff, 0);

	DrawDigit(pGDI, MakeStr("%d", m_score[0]),
		WPoint(rect.x + 50.0f, rect.y + 3.0f), 0xffff0000, FrGDI::ALIGN_CENTER,
		false);
	DrawDigit(pGDI, MakeStr("%d", m_score[1]),
		WPoint(rect.x + 240.0f, rect.y + 3.0f), 0xff0000ff, FrGDI::ALIGN_CENTER,
		false);
}

void FrRankForm3::LoadBitmaps()
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		for (int i = 0; i < 10; ++i)
			m_pDigit[i] = g_pFresh->GetManager()->GetBitmap("SCORECARD",
				MakeStr("%d", i));
		m_pPlus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "+");
		m_pMinus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
	}
}

IMPLEMENT_OBJECT(FrRankFormEx, FrForm)

BEGIN_FRESH_MSGMAP(FrRankFormEx, FrForm)
ON_FRESH_VI("stage", FRCMD_INIT, FrRankFormEx::OnStageAreaInit)
ON_FRESH_VV("stage", FRCMD_OWNERDRAW, FrRankFormEx::OnStageAreaOwnerDraw)
ON_FRESH_VI("list", FRCMD_INIT, FrRankFormEx::OnRankInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrRankFormEx::OnRankOwnerDraw)
ON_FRESH_VI("billboard", FRCMD_INIT, FrRankFormEx::OnBoardInit)
END_FRESH_MSGMAP()

CRankExData::CRankExData()
	: m_pRival(NULL),
	  m_motionTime(3.0f),
	  m_elapsedTime(0.0f),
	  m_leaderOID(0xffffffff),
	  m_pendingOID(0xffffffff),
	  m_pRankList(NULL),
	  m_pStageArea(NULL),
	  m_pBoard(NULL)
{
}

CRankExData::~CRankExData()
{
}

void CRankExData::InitAreaStage(FrArea* area)
{
	m_pStageArea = area;
}

void CRankExData::Clear()
{
	if (m_pLeaderPet)
	{
		delete m_pLeaderPet;
		m_pLeaderPet = NULL;
	}
	if (m_pRival)
	{
		delete[] m_pRival;
		m_pRival = NULL;
	}
}

void CRankExData::OnProc(float delta)
{
	m_elapsedTime += delta;
	if (m_leaderOID == 0xffffffff)
	{
		SetLeader();
		return;
	}
	if (m_pendingOID != 0xffffffff)
	{
		ShowLeader();
		return;
	}
	if (m_pLeaderPet)
		m_pLeaderPet->Process(delta, false);
}

int CRankExData::GetMedalIndex(unsigned long oid)
{
	if (Doc()->m_rivalList.size() < 10)
		return 3;

	unsigned char index = Doc()->GetIndex(oid);
	if (index == 0xff || Doc()->m_rivalList[index].flag > 3)
		return 3;

	unsigned char rank = Doc()->m_rivalList[index].rank;
	unsigned char count = 0, better = 0;

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if ((it->state == 3 && GetHoleIndex(it->hole) >= 4) || it->state != 3)
		{
			count++;

			if (it->state != 3 && it->rank < rank && it->flag > 3)
				better++;
		}
	}

	rank -= better;
	if (Doc()->m_golfGame.holes == 9)
	{
		if (count <= 14)
		{
			return 3;
		}
		else if (count <= 18)
		{
			if (rank == 1)
				return 2;
		}
		else if (count <= 22)
		{
			if (rank == 1)
				return 1;
		}
		else if (count <= 26)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	}
	else if (Doc()->m_golfGame.holes == 18)
	{
		if (count <= 14)
		{
			if (rank == 1)
				return 2;
		}
		else if (count <= 18)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else if (count <= 22)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		}
		else if (count <= 26)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

void CRankExData::CreateLeaderPet(const WRect& rect)
{
	m_pLeaderPet = new CExhibition(rect.x + rect.w * 0.5f,
		rect.y + rect.h * 0.5f, 0.3f, false);
	m_pLeaderPet->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
	m_pLeaderPet->SetArea(rect.w, rect.h);
}

void CRankExData::SetActive(bool bActive)
{
	m_pendingOID = 0xffffffff;
	m_leaderOID = 0xffffffff;
	m_motionTime = 3.0f;
	m_elapsedTime = 0.0f;
	if (bActive)
		SetLeader();
}

bool CRankExData::IsMotionFinished()
{
	return Min(m_motionTime + 1.0f, 10.0f) < m_elapsedTime;
}

void CRankExData::ResetStage()
{
	if (m_pLeaderPet && m_pStageArea)
	{
		const WRect& rect = m_pStageArea->GetRect();
		m_pLeaderPet->SetCenter(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f);
		m_pLeaderPet->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
	}
}

void CRankExData::InitRivalData()
{
	int num = Doc()->m_rivalList.size();
	m_pRival = new sRivalData[num];

	for (int i = 0; i < num; i++)
	{
		m_pRival[i] = Doc()->m_rivalList[i];
		m_pRankList->AddItem(&m_pRival[i]);
	}
}

void CRankExData::InitMedalData()
{
	m_pMedal[0] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pMedal[1] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pMedal[2] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pMedal[3] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");
	m_pTeamRank[0] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pTeamRank[1] =
		g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");
	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("30s", "pc_icon_mass_list");
	m_pZeroIcon = g_pFresh->GetManager()->GetBitmap("30s", "icon_zero");
}

void CRankExData::DrawRankItem(FrListItem* item)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		int space = pGDI->SetSpace(0);
		sRivalData* pRival = (sRivalData*)item->pData;
		pGDI->SetTextStyle(0);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);

		if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
		{
			if (m_pTeamRank[pRival->team])
				pGDI->DrawTexture(m_pTeamRank[pRival->team],
					WRect(item->pos.x + 12.0f, item->pos.y + 2.0f,
						(float)m_pTeamRank[pRival->team]->Width(),
						(float)m_pTeamRank[pRival->team]->Height()),
					0xffffffff, 0);
			pGDI->Print(WPoint(item->pos.x + 21.0f, item->pos.y + 3.0f), 1,
				"%d", pRival->rank);
		}
		else
		{
			if (m_pRank)
				pGDI->DrawTexture(m_pRank,
					WRect(item->pos.x + 12.0f, item->pos.y + 2.0f,
						(float)m_pRank->Width(), (float)m_pRank->Height()),
					0xffffffff, 0);
			if (pRival->state == 3)
				pGDI->Print(WPoint(item->pos.x + 21.0f, item->pos.y + 3.0f), 1,
					"-");
			else
				pGDI->Print(WPoint(item->pos.x + 21.0f, item->pos.y + 3.0f), 1,
					"%d", pRival->rank);
		}

		if (Doc()->m_roomInfo.nUserLimit < 100)
		{
			bool bStarted = pRival->hole != Doc()->m_holeOrder[0];
			if (bStarted)
			{
				int medal = GetMedalIndex(pRival->oid);
				if (medal >= 0 && medal <= 2)
				{
					if (pRival->flag > 3)
						medal = 3;
					pGDI->DrawTexture(m_pMedal[medal],
						WRect(item->pos.x + 2.0f, item->pos.y + 1.0f,
							(float)m_pMedal[medal]->Width(),
							(float)m_pMedal[medal]->Height()),
						0xffffffff, 0);
				}
			}
		}

		if (pRival->oid == MyGuid(true))
		{
			if (Doc()->m_golfGame.gameType == GAME_TYPE_30S_TEAM)
				pGDI->SetTextColor(pRival->team ? 0xff0060ff : 0xffff0000,
					0xffffffff);
			else
				pGDI->SetTextColor(0xffff0000, 0xffffffff);
		}
		else if (pRival->state == 3)
			pGDI->SetTextColor(0xff808080, 0xffffffff);
		else
			pGDI->SetTextColor(0xff000000, 0xffffffff);

		const Bitmap* pIcon;
		IFF_STRUCT::sSkin* pSkin;
		if (pRival->titleTypeId &&
			(pSkin = ItemManager()->FindSkin(pRival->titleTypeId)))
			pIcon = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->c.Icon);
		else
			pIcon = g_pFresh->GetManager()->GetBitmap("LEVELS",
				MakeStr("level_%03d", pRival->level + 1));

		if (pIcon)
		{
			WRect rect;
			rect.w = (float)pIcon->Width();
			rect.h = (float)pIcon->Height();
			rect.x = float2int((60.0f - rect.w) * 0.5f) + item->pos.x + 35.0f;
			rect.y = item->pos.y + 8.0f - float2int(rect.h * 0.5f);
			pGDI->DrawTexture(pIcon, rect, 0xffffffff, 0);
		}

		if (pRival->guildUID)
		{
			const Bitmap* pEmblem =
				NetResourceManager::Instance()->GetEmblemByName(
					pRival->guildMark);
			if (pEmblem)
				pGDI->DrawTexture(pEmblem,
					WRect(item->pos.x + 100.0f, item->pos.y + 3.0f - 6.0f,
						(float)pEmblem->Width(), (float)pEmblem->Height()),
					0xffffffff, 0);
		}

		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 127.0f, item->pos.y + 3.0f), 0,
				pRival->nickname, 90.0f, 0xffffffff);

		pGDI->Print(WPoint(item->pos.x + 240.0f, item->pos.y + 3.0f), 2, "%d",
			pRival->totalScore);
		pGDI->Print(WPoint(item->pos.x + 285.0f, item->pos.y + 3.0f), 2,
			"%I64d", pRival->totalPang);

		if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
			pGDI->Print(WPoint(item->pos.x + 323.0f, item->pos.y + 3.0f), 2,
				"End");
		else if (pRival->state == 3)
			pGDI->Print(WPoint(item->pos.x + 323.0f, item->pos.y + 3.0f), 2,
				"Out");
		else
			pGDI->Print(WPoint(item->pos.x + 323.0f, item->pos.y + 3.0f), 2,
				"%d\xc8\xa6", GetHoleIndex(pRival->hole));

		if ((pRival->capability & 0x80) && m_pPCIcon)
			pGDI->DrawTexture(m_pPCIcon,
				WRect(item->pos + WPoint(329.0f, 3.0f),
					(float)m_pPCIcon->Width(), (float)m_pPCIcon->Height()),
				0xffffffff, 0);
		else if (m_pZeroIcon)
			pGDI->DrawTexture(m_pZeroIcon,
				WRect(item->pos + WPoint(331.0f, 6.0f),
					(float)m_pZeroIcon->Width(), (float)m_pZeroIcon->Height()),
				0xffffffff, 0);

		pGDI->SetSpace(space);
	}
}

void CRankExData::DrawStageArea()
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	if (m_leaderOID == 0xffffffff || m_pendingOID != 0xffffffff ||
		!m_pLeaderPet)
		return;

	m_pLeaderPet->Display(0.0f, 0.0f);

	if (!m_pLeaderPet->GetPetFrame())
		return;

	WBone* pHead =
		m_pLeaderPet->GetPetFrame()->GetBonePet()->FindBone("Bip01 Head");
	if (!pHead)
		return;

	WVector* pPivot = &pHead->GetMatrix().pivot;
	const WVector2D& center = m_pLeaderPet->GetCenter();

	float fov = g_view->GetFOV();
	g_view->SetFOV(g_view->GetRatio() * (120.0f * g_DEGTORAD));
	g_view->UpdateCamera();

	const Bitmap* pIcon;
	if (Doc()->m_rivalList[Doc()->GetIndex(m_leaderOID)].titleTypeId)
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(
			Doc()->m_rivalList[Doc()->GetIndex(m_leaderOID)].titleTypeId);
		if (pSkin)
			pIcon = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->c.Icon);
		else
			pIcon = g_pFresh->GetManager()->GetBitmap("LEVELS",
				MakeStr("level_%03d",
					Doc()->m_rivalList[Doc()->GetIndex(m_leaderOID)].level +
						1));
	}
	else
		pIcon = g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("level_%03d",
				Doc()->m_rivalList[Doc()->GetIndex(m_leaderOID)].level + 1));

	if (pIcon)
	{
		WVector screen = g_view->Projection(*pPivot);
		if (0.0f < screen.z && screen.z < 1.0f)
		{
			sRivalData* pLeader =
				&Doc()->m_rivalList[Doc()->GetIndex(m_leaderOID)];
			const char* nickname = pLeader->nickname;
			float textWidth = pGDI->GetTextExtend(nickname);
			WRect rect;
			rect.w = (float)pIcon->Width();
			rect.h = (float)pIcon->Height();
			rect.x = (float)float2int(screen.x + center.x -
				(rect.w + g_view->GetWidth() + textWidth + 5.0f) * 0.5f);
			rect.y = (float)float2int(
				screen.y - 52.0f + center.y - g_view->GetHeight() * 0.5f);
			pGDI->DrawTexture(pIcon,
				WRect(rect.x, rect.y + 8.0f - float2int(rect.h * 0.5f), rect.w,
					rect.h),
				0xffffffff, 0);

			WPoint pos(rect.x + rect.w + 5.0f, rect.y + 2.0f);
			pGDI->SetTextColor(0xffffffff, 0xff000000);
			pGDI->SetTextStyle(2);
			if (pLeader->guildUID)
			{
				const Bitmap* pEmblem =
					NetResourceManager::Instance()->GetEmblemByName(
						pLeader->guildMark);
				if (pEmblem)
					pGDI->DrawTexture(pEmblem,
						WRect(pos.x, pos.y - 6.0f, (float)pEmblem->Width(),
							(float)pEmblem->Height()),
						0xffffffff, 0);
				pos.x += 27.0f;
			}

			if (!CProjectG::Instance()->HidePrivacy())
				g_pFresh->GetManager()->PrintText(pos, 0, nickname, -1.0f,
					0xffffffff);
		}
	}

	g_view->SetFOV(fov);
	g_view->UpdateCamera();
}

void CRankExData::SetLeader()
{
	unsigned char myHole = Doc()->m_rivalList[MyIndex()].hole;
	if (GetHoleIndex(myHole) <= GetHoleIndex(GOLFDOC()->m_currentHole))
	{
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
			return;
	}

	std::vector<sRivalData>& rivals = Doc()->m_rivalList;

	int i;
	for (i = 0; i < (int)rivals.size(); i++)
	{
		sRivalData& rival = rivals[i];
		m_pRival[i].state = rival.state;
		m_pRival[i].totalPang = rival.totalPang;
		m_pRival[i].totalScore = rival.totalScore;
		m_pRival[i].hole = rival.hole;
		m_pRival[i].rank = rival.rank;
		m_pRival[i].quitOrder = rival.quitOrder;
		m_pRival[i].finishOrder = rival.finishOrder;
	}

	m_pRankList->SortItem(RankCompare);

	int count = 0;
	for (i = 0; i < (int)rivals.size(); i++)
	{
		if (m_pRival[i].rank == 1)
			count++;
	}

	if (count == 0)
		return;

	int pick = rand() % count;
	count = 0;
	int leader = rivals.size();
	for (i = 0; i < (int)rivals.size(); i++)
	{
		if (m_pRival[i].rank == 1)
		{
			if (count == pick)
			{
				leader = i;
				m_leaderOID = m_pRival[leader].oid;
				break;
			}
			count++;
		}
	}

	if (m_leaderOID == 0xffffffff)
		return;

	if (!Doc()->m_userInfoMod)
	{
		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(m_pRival[leader].uid, it))
		{
			WSendPacket packet(0x2f);
			packet.Encode4(m_pRival[leader].uid);
			packet.Encode1(5);
			packet.Send(TO_GAME);

			Doc()->m_userInfoMod = 1;
		}
	}

	ShowLeader();

	char buf[256];
	sprintf(buf,
		"\xc7\xf6\xc0\xe7 1\xc0\xa7 %s\xb4\xd4, \xbd\xba\xc4\xda\xbe\xee %d",
		m_pRival[leader].nickname, m_pRival[leader].totalScore);
	m_pBoard->SetCaption(buf);
}

void CRankExData::ShowLeader()
{
	std::map<unsigned long, sUserInfoTime>::iterator it;
	int found = 0;
	unsigned long oid = m_pendingOID == 0xffffffff ? m_leaderOID : m_pendingOID;

	std::map<unsigned long, sBriefUserInfo>::iterator itBrief =
		Doc()->m_briefUserInfoMap.find(oid);
	if (itBrief != Doc()->m_briefUserInfoMap.end())
		found = Doc()->FindUserInfoTimeUID(itBrief->second.dwUid, it);

	if (!found)
	{
		if (m_pendingOID == 0xffffffff)
			m_pendingOID = m_leaderOID;
		return;
	}

	if (Doc()->GetUserInfoMod() == 1)
		return;

	m_pendingOID = 0xffffffff;
	CPartTidList parts;
	sCharacterInfo* pInfo = &it->second.info.charInfo;
	parts.SetTids(pInfo, 0xff);
	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pInfo->tid);

	if (m_pLeaderPet)
	{
		m_pLeaderPet->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
		if (IsLocalCountry(30))
		{
			if (!pChar || !pChar->Mtn30sWinner[0])
				m_pLeaderPet->SetModel(parts, pInfo->tidAuxParts,
					"1\xb5\xee\xb8\xf0\xbc\xc7", 0.5f, oid);
			else if (!stricmp(pChar->Mtn30sWinner, "1st_place"))
				m_pLeaderPet->SetModel(parts, pInfo->tidAuxParts,
					"1\xb5\xee\xb8\xf0\xbc\xc7", 0.5f, oid);
			else if (!stricmp(pChar->Mtn30sWinner, "2nd_place"))
				m_pLeaderPet->SetModel(parts, pInfo->tidAuxParts,
					"2\xb5\xee\xb8\xf0\xbc\xc7", 0.5f, oid);
			else
				m_pLeaderPet->SetModel(parts, pInfo->tidAuxParts,
					"1\xb5\xee\xb8\xf0\xbc\xc7", 0.5f, oid);
		}
		else
		{
			m_pLeaderPet->SetModel(parts, pInfo->tidAuxParts,
				pChar && pChar->Mtn30sWinner[0] ? pChar->Mtn30sWinner
												: "1\xb5\xee\xb8\xf0\xbc\xc7",
				0.5f, oid);
		}

		m_motionTime = m_pLeaderPet->GetInitialMotionLength();
	}
}

void CRankExData::ClearRival()
{
	if (m_pRival)
	{
		delete[] m_pRival;
		m_pRival = NULL;
	}
}

void CRankExData::SetRivalData()
{
	int num = Doc()->m_rivalList.size();
	m_pRival = new sRivalData[num];
	m_pRankList->ClearItem();

	for (int i = 0; i < num; i++)
	{
		m_pRival[i] = Doc()->m_rivalList[i];
		m_pRankList->AddItem(&m_pRival[i]);
	}
}

FrRankFormEx::FrRankFormEx()
{
}

FrRankFormEx::~FrRankFormEx()
{
}

void FrRankFormEx::OnStageAreaInit(int param)
{
	FrArea* pArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (pArea)
	{
		char name[64] = "";
		sprintf(name, "30in_back_%s",
			Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->Data : NULL);
		pArea->SetBgImg(name);

		m_rankExData.InitAreaStage(pArea);
		m_rankExData.CreateLeaderPet(pArea->GetRect());
	}
}

void FrRankFormEx::OnStageAreaOwnerDraw()
{
	if (IsVisible())
	{
		char name[64] = "";
		sprintf(name, "30in_back_%s",
			Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->Data : NULL);
		m_rankExData.m_pStageArea->SetBgImg(name);
		m_rankExData.DrawStageArea();
	}
}

void FrRankFormEx::OnRankInit(int param)
{
	m_rankExData.m_pRankList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_rankExData.InitRivalData();
	m_rankExData.InitMedalData();
}

void FrRankFormEx::SortRank()
{
	memset(Doc()->m_teamResult, 0, sizeof(Doc()->m_teamResult));

	std::vector<sRivalData>::iterator it;
	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		if ((*it).state == 3)
		{
			(*it).rank = Doc()->m_rivalList.size() - (*it).quitOrder;
			continue;
		}

		(*it).rank = 1;
		std::vector<sRivalData>::iterator it2;
		for (it2 = Doc()->m_rivalList.begin(); it2 != Doc()->m_rivalList.end();
			++it2)
		{
			if (it == it2 || (*it2).state == 3)
				continue;

			if ((*it).totalScore > (*it2).totalScore ||
				((*it).totalScore == (*it2).totalScore &&
					(*it).totalPang < (*it2).totalPang))
				(*it).rank++;
		}

		Doc()->m_teamResult[(*it).team].score += (*it).rank;
		Doc()->m_teamResult[(*it).team].pang += (*it).totalPang;
	}

	m_rankExData.m_pRankList->SortItem(RankCompare);
}

sRivalData* FrRankFormEx::GetRival()
{
	return m_rankExData.m_pRival;
}

void FrRankFormEx::OnRankOwnerDraw(int param)
{
	if (param)
		m_rankExData.DrawRankItem((FrListItem*)param);
}

void FrRankFormEx::OnBoardInit(int param)
{
	m_rankExData.m_pBoard = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_rankExData.m_pBoard->m_nFlags.Enable(FWF_PRIVACY);
}

void FrRankFormEx::Clear()
{
	m_rankExData.Clear();
}

void FrRankFormEx::SetActive(bool bActive)
{
	m_rankExData.SetActive(bActive);
	Enable(bActive);
	SetVisible(bActive);
}

void FrRankFormEx::SetLeader()
{
	m_rankExData.SetLeader();
}

void FrRankFormEx::ShowLeader()
{
	m_rankExData.ShowLeader();
}

bool FrRankFormEx::OnInit()
{
	Enable(false);
	SetVisible(false);
	return true;
}

void FrRankFormEx::OnProc(const float delta)
{
	if (IsVisible())
		m_rankExData.OnProc(delta);
}

bool FrRankFormEx::IsMotionFinished()
{
	return m_rankExData.IsMotionFinished();
}

void FrRankFormEx::ResetStage()
{
	m_rankExData.ResetStage();
}

static void __stdcall SortPlayerOrder(int* order)
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (TEAM(0)->score > TEAM(1)->score ||
			(TEAM(0)->score == TEAM(1)->score &&
				TEAM(0)->totalPang >= TEAM(1)->totalPang))
		{
			order[0] = 0;
			order[1] = 1;
		}
		else
		{
			order[0] = 1;
			order[1] = 0;
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
	{
		if (PLAYER(0)->state == 3)
		{
			order[0] = 1;
			order[1] = 0;
		}
		else if (PLAYER(1)->state == 3)
		{
			order[0] = 0;
			order[1] = 1;
		}
		else if (PLAYER(0)->score > PLAYER(1)->score ||
			(PLAYER(0)->score == PLAYER(1)->score &&
				PLAYER(0)->totalPang >= PLAYER(1)->totalPang))
		{
			order[0] = 0;
			order[1] = 1;
		}
		else
		{
			order[0] = 1;
			order[1] = 0;
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		int count = 0;
		for (int rank = 0; rank < GOLFDOC()->m_playerNum; rank++)
		{
			for (int i = 0; i < GOLFDOC()->m_playerNum; i++)
			{
				if (PLAYER(i)->state != 3)
				{
					if (PLAYER(i)->rank == rank + 1)
						order[count++] = i;
				}
			}
		}
		for (int i = 0; i < GOLFDOC()->m_playerNum; i++)
		{
			if (PLAYER(i)->state == 3)
				order[count++] = i;
		}
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		order[0] = 0;
		order[1] = 1;
	}
	else
	{
		for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
		{
			if (PLAYER(i)->state == 3 &&
				(Doc()->m_gameMode == 1 || Doc()->m_gameMode == 5))
				continue;
			order[PLAYER(i)->order - 1] = i;
		}
	}
}

IMPLEMENT_OBJECT(FrAwardForm, FrForm)

BEGIN_FRESH_MSGMAP(FrAwardForm, FrForm)
ON_FRESH_VI("tab0", FRCMD_INIT, FrAwardForm::OnTab0Init)
ON_FRESH_VI("tab1", FRCMD_INIT, FrAwardForm::OnTab1Init)
ON_FRESH_VI("userlist", FRCMD_INIT, FrAwardForm::OnUserListInit)
ON_FRESH_VI("userlist", FRCMD_OWNERDRAW, FrAwardForm::OnUserListOwnerDraw)
ON_FRESH_VI("expbar0", FRCMD_INIT, FrAwardForm::OnExpBar0Init)
ON_FRESH_VI("expbar1", FRCMD_INIT, FrAwardForm::OnExpBar1Init)
ON_FRESH_VI("expbar2", FRCMD_INIT, FrAwardForm::OnExpBar2Init)
ON_FRESH_VI("expbar3", FRCMD_INIT, FrAwardForm::OnExpBar3Init)
ON_FRESH_VI("exp0", FRCMD_INIT, FrAwardForm::OnExp0Init)
ON_FRESH_VI("exp1", FRCMD_INIT, FrAwardForm::OnExp1Init)
ON_FRESH_VI("exp2", FRCMD_INIT, FrAwardForm::OnExp2Init)
ON_FRESH_VI("exp3", FRCMD_INIT, FrAwardForm::OnExp3Init)
ON_FRESH_VI("expend0", FRCMD_INIT, FrAwardForm::OnExpEnd0Init)
ON_FRESH_VI("expend1", FRCMD_INIT, FrAwardForm::OnExpEnd1Init)
ON_FRESH_VI("expend2", FRCMD_INIT, FrAwardForm::OnExpEnd2Init)
ON_FRESH_VI("expend3", FRCMD_INIT, FrAwardForm::OnExpEnd3Init)
ON_FRESH_VI("level0", FRCMD_INIT, FrAwardForm::OnLevel0Init)
ON_FRESH_VI("level1", FRCMD_INIT, FrAwardForm::OnLevel1Init)
ON_FRESH_VI("level2", FRCMD_INIT, FrAwardForm::OnLevel2Init)
ON_FRESH_VI("level3", FRCMD_INIT, FrAwardForm::OnLevel3Init)
ON_FRESH_VI("levelup0", FRCMD_INIT, FrAwardForm::OnLevelUp0Init)
ON_FRESH_VI("levelup1", FRCMD_INIT, FrAwardForm::OnLevelUp1Init)
ON_FRESH_VI("levelup2", FRCMD_INIT, FrAwardForm::OnLevelUp2Init)
ON_FRESH_VI("levelup3", FRCMD_INIT, FrAwardForm::OnLevelUp3Init)
ON_FRESH_VI("match_res0", FRCMD_INIT, FrAwardForm::OnMatchRes0Init)
ON_FRESH_VI("match_res1", FRCMD_INIT, FrAwardForm::OnMatchRes1Init)
ON_FRESH_VI("GainedLadderPoint0", FRCMD_INIT,
	FrAwardForm::OnGainedLadderPoint0Init)
ON_FRESH_VI("GainedLadderPoint1", FRCMD_INIT,
	FrAwardForm::OnGainedLadderPoint1Init)
ON_FRESH_VI("LadderPoint0", FRCMD_INIT, FrAwardForm::OnLadderPoint0Init)
ON_FRESH_VI("LadderPoint1", FRCMD_INIT, FrAwardForm::OnLadderPoint1Init)
ON_FRESH_VI("nextlevel", FRCMD_INIT, FrAwardForm::OnNextLevelTextInit)
END_FRESH_MSGMAP()

FrAwardForm::FrAwardForm()
{
	m_pExpSound = NULL;
	m_tickTime = 0.0f;
	m_pUserList = NULL;
	m_playerNum = 0;
	memset(m_pExpBar, 0, sizeof(m_pExpBar));
	memset(m_pExp, 0, sizeof(m_pExp));
	memset(m_pExpEnd, 0, sizeof(m_pExpEnd));
	memset(m_pLevel, 0, sizeof(m_pLevel));
	memset(m_pLevelUp, 0, sizeof(m_pLevelUp));
	memset(m_pMatchRes, 0, sizeof(m_pMatchRes));

	memset(m_order, 0, sizeof(m_order));
	memset(m_level, 0, sizeof(m_level));
	memset(m_gainExp, 0, sizeof(m_gainExp));
	memset(m_curExp, 0, sizeof(m_curExp));
	memset(m_bFinished, 0, sizeof(m_bFinished));

	memset(m_pGainedLadderPoint, 0, sizeof(m_pGainedLadderPoint));
	memset(m_pLadderPoint, 0, sizeof(m_pLadderPoint));
	m_pNextLevelText = NULL;
	m_treasureTime = 0.0f;

	for (int i = 0; i < 6; ++i)
		m_pBonusMascot[i] = NULL;
}

FrAwardForm::~FrAwardForm()
{
}

bool FrAwardForm::Close(bool bResult)
{
	if (m_pExpSound)
		g_audio->StopSfx(m_pExpSound);

	return FrForm::Close(bResult);
}

void FrAwardForm::OnUserListInit(int param)
{
	m_pUserList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		m_playerNum = GOLFDOC()->m_playerNum;
		unsigned char winTeam =
			GOLFDOC()->m_teamGameWinner == 2 ? 0 : GOLFDOC()->m_teamGameWinner;
		for (int i = 0; i < m_playerNum; ++i)
		{
			if (GOLFDOC()->GetPlayer(i)->teammate != 0xff)
			{
				if (GOLFDOC()->GetPlayer(i)->team == winTeam)
				{
					m_order[i >= GOLFDOC()->GetPlayer(i)->teammate] = i;
					GOLFDOC()->GetPlayer(i)->order =
						GOLFDOC()->GetPlayer(i)->rank =
							i >= GOLFDOC()->GetPlayer(i)->teammate ? 2 : 1;
				}
				else
				{
					m_order[i >= GOLFDOC()->GetPlayer(i)->teammate ? 3 : 2] = i;
					GOLFDOC()->GetPlayer(i)->order =
						GOLFDOC()->GetPlayer(i)->rank =
							i >= GOLFDOC()->GetPlayer(i)->teammate ? 4 : 3;
				}
			}
			else if (GOLFDOC()->GetPlayer(i)->team == winTeam)
			{
				m_order[0] = i;
				GOLFDOC()->GetPlayer(i)->order = GOLFDOC()->GetPlayer(i)->rank =
					1;
			}
			else
			{
				m_order[1] = i;
				GOLFDOC()->GetPlayer(i)->order = GOLFDOC()->GetPlayer(i)->rank =
					2;
			}
		}
	}
	else
	{
		m_playerNum = GOLFDOC()->m_playerNum;
		SortPlayerOrder(m_order);
	}

	for (int i = 0; i < m_playerNum; ++i)
		m_pUserList->AddItem((void*)m_order[i]);
}

void FrAwardForm::OnUserListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	int index = (int)item->pData;
	if (index >= GOLFDOC()->m_playerNum)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	WRect rect(item->pos.x, item->pos.y, (float)m_pUserList->GetItemWidth(),
		(float)m_pUserList->GetItemHeight());
	float offsetY = 0.0f;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (index == MyIndex() ||
			(PLAYER(MyIndex())->teammate != 0xff &&
				index == PLAYER(MyIndex())->teammate))
			gdi->Box(rect, 0x999b9dff);

		bool bFirst;
		if (PLAYER(MyIndex())->teammate != 0xff &&
			index < PLAYER(index)->teammate)
			bFirst = true;
		else
			bFirst = false;

		float textY;
		if (m_playerNum < 4)
			textY = 8.0f;
		else
		{
			textY = bFirst ? 11.0f : 5.0f;
			offsetY = bFirst ? 3.0f : -3.0f;
		}

		if (!CProjectG::Instance()->HidePrivacy())
		{
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 98.0f, textY + item->pos.y), 0,
				Doc()->m_userInfo[index].info.sNick, 70.0f, 0xffffffff);
		}

		if (!bFirst)
		{
			if (m_playerNum < 4)
			{
				gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y + 8.0f), 2,
					"%d", TEAM(PLAYER(index)->team)->score);
				gdi->Print(WPoint(item->pos.x + 237.0f, item->pos.y + 8.0f), 2,
					"%d", (int)TEAM(PLAYER(index)->team)->totalPang);
				gdi->SetTextColor(0xff006493, 0xffffffff);
				gdi->Print(WPoint(item->pos.x + 243.0f, item->pos.y + 8.0f), 0,
					"(+%d)", (int)TEAM(PLAYER(index)->team)->bonusPang);
				gdi->SetTextColor(0xff000000, 0xffffffff);
			}
			else
			{
				gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y - 6.0f), 2,
					"%d", TEAM(PLAYER(index)->team)->score);
				gdi->Print(WPoint(item->pos.x + 237.0f, item->pos.y - 6.0f), 2,
					"%d", (int)TEAM(PLAYER(index)->team)->totalPang);
				gdi->SetTextColor(0xff006493, 0xffffffff);
				gdi->Print(WPoint(item->pos.x + 243.0f, item->pos.y - 6.0f), 0,
					"(+%d)", (int)TEAM(PLAYER(index)->team)->bonusPang);
				gdi->SetTextColor(0xff000000, 0xffffffff);
			}
		}
	}
	else
	{
		if (PLAYER(index)->oid == MyGuid(true))
			gdi->Box(rect, 0x999b9dff);
		else if (PLAYER(index)->state == 3)
			gdi->Box(rect, 0x80808080);

		if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE)
		{
			gdi->Print(WPoint(item->pos.x + 19.0f, item->pos.y + 8.0f), 1, "%d",
				PLAYER(index)->order);
		}

		if (!CProjectG::Instance()->HidePrivacy())
		{
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 98.0f, item->pos.y + 8.0f), 0,
				Doc()->m_userInfo[index].info.sNick, 70.0f, 0xffffffff);
		}

		if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE)
		{
			if (PLAYER(index)->score == 0)
				gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y + 8.0f), 2,
					"0");
			else
				gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y + 8.0f), 2,
					"%+d", PLAYER(index)->score);
		}
		else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
			gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y + 8.0f), 2,
				"%I64d", PLAYER(index)->skinsTotalPang);
		else
			gdi->Print(WPoint(item->pos.x + 194.0f, item->pos.y + 8.0f), 2,
				"%d", PLAYER(index)->score);

		gdi->Print(WPoint(item->pos.x + 237.0f, item->pos.y + 8.0f), 2, "%d",
			(int)GOLFDOC()->GetPlayer(index)->totalPang);
		gdi->SetTextColor(0xff006493, 0xffffffff);
		gdi->Print(WPoint(item->pos.x + 243.0f, item->pos.y + 8.0f), 0, "(%+d)",
			(int)GOLFDOC()->GetPlayer(index)->bonusPang);
		gdi->SetTextColor(0xff000000, 0xffffffff);
	}

	bool bDrawn = false;
	sSlotInfo* slot = NULL;
	for (SLOT_LIST_IT it = Doc()->m_slotList.begin();
		it != Doc()->m_slotList.end(); ++it)
	{
		if ((*it).dwGuid == Doc()->m_userInfo[index].info.dwGuid)
		{
			slot = &(*it);
			break;
		}
	}
	if (slot)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (slot->pangma[1] && m_pBonusMastery2)
			{
				WRect rc(item->pos.x + 477.0f, offsetY + item->pos.y + 3.0f,
					(float)m_pBonusMastery2->Width(),
					(float)m_pBonusMastery2->Height());
				gdi->DrawTexture(m_pBonusMastery2, rc, 0xffffffff, 0);
				bDrawn = true;
			}
			else if (slot->pangma[0] && m_pBonusMastery)
			{
				WRect rc(item->pos.x + 474.0f, offsetY + item->pos.y + 3.0f,
					(float)m_pBonusMastery->Width(),
					(float)m_pBonusMastery->Height());
				gdi->DrawTexture(m_pBonusMastery, rc, 0xffffffff, 0);
				bDrawn = true;
			}
		}
	}

	if (bool((Doc()->m_userInfo[index].info.dwIdentity >> 7) & 1))
	{
		if (m_pPCIconBonus)
		{
			WRect rc(item->pos.x + 412.0f, offsetY + item->pos.y + 6.0f,
				(float)m_pPCIconBonus->Width(),
				(float)m_pPCIconBonus->Height());
			gdi->DrawTexture(m_pPCIconBonus, rc, 0xffffffff, 0);
		}
		bDrawn = true;
	}

	unsigned long mascotId = Doc()->m_userInfo[index].mascotInfo.tid;
	if (mascotId)
	{
		IFF_STRUCT::sMascot* mascot = ItemManager()->FindMascot(mascotId);
		if (mascot)
		{
			if (!strcmpi(mascot->c.Name, "\xc7\xcf\xbe\xe7\xc1\xa6\xb8\xae"))
			{
				if (m_pBonusMascot[0])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[0]->Width(),
						(float)m_pBonusMascot[0]->Height());
					gdi->DrawTexture(m_pBonusMascot[0], rc, 0xffffffff, 0);
				}
				return;
			}
			else if (!strcmpi(mascot->c.Name,
						 "\xb1\xee\xb8\xc1\xc1\xa6\xb8\xae"))
			{
				if (m_pBonusMascot[1])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[1]->Width(),
						(float)m_pBonusMascot[1]->Height());
					gdi->DrawTexture(m_pBonusMascot[1], rc, 0xffffffff, 0);
				}
				return;
			}
			else if (mascot->c.TypeId == 0x40000009)
			{
				if (m_pBonusMascot[2])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[2]->Width(),
						(float)m_pBonusMascot[2]->Height());
					gdi->DrawTexture(m_pBonusMascot[2], rc, 0xffffffff, 0);
				}
				return;
			}
			else if (mascot->c.TypeId == 0x4000000a)
			{
				if (m_pBonusMascot[3])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[3]->Width(),
						(float)m_pBonusMascot[3]->Height());
					gdi->DrawTexture(m_pBonusMascot[3], rc, 0xffffffff, 0);
				}
				return;
			}
			else if (mascot->c.TypeId == 0x4000000b)
			{
				if (m_pBonusMascot[4])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[4]->Width(),
						(float)m_pBonusMascot[4]->Height());
					gdi->DrawTexture(m_pBonusMascot[4], rc, 0xffffffff, 0);
				}
				return;
			}
			else if (mascot->c.TypeId == 0x4000000c)
			{
				if (m_pBonusMascot[5])
				{
					WRect rc(item->pos.x + 458.0f, offsetY + item->pos.y + 1.0f,
						(float)m_pBonusMascot[5]->Width(),
						(float)m_pBonusMascot[5]->Height());
					gdi->DrawTexture(m_pBonusMascot[5], rc, 0xffffffff, 0);
				}
				return;
			}
		}
	}

	if (!bDrawn && m_pZeroIconVS)
	{
		WRect rc(item->pos.x + 442.0f, offsetY + item->pos.y + 10.0f,
			(float)m_pZeroIconVS->Width(), (float)m_pZeroIconVS->Height());
		gdi->DrawTexture(m_pZeroIconVS, rc, 0xffffffff, 0);
	}
}

void FrAwardForm::OnTab0Init(int param)
{
	FrArea* tab = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrAwardForm::OnTab1Init(int param)
{
	FrArea* tab = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrAwardForm::OnExpBar0Init(int param)
{
	m_pExpBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pExpBar[0]->SetVisible(false);
}

void FrAwardForm::OnExpBar1Init(int param)
{
	m_pExpBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pExpBar[1]->SetVisible(false);
}

void FrAwardForm::OnExpBar2Init(int param)
{
	m_pExpBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pExpBar[2]->SetVisible(false);
}

void FrAwardForm::OnExpBar3Init(int param)
{
	m_pExpBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pExpBar[3]->SetVisible(false);
}

void FrAwardForm::OnExp0Init(int param)
{
	m_pExp[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExp[0]->SetVisible(false);
}

void FrAwardForm::OnExp1Init(int param)
{
	m_pExp[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExp[1]->SetVisible(false);
}

void FrAwardForm::OnExp2Init(int param)
{
	m_pExp[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExp[2]->SetVisible(false);
}

void FrAwardForm::OnExp3Init(int param)
{
	m_pExp[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExp[3]->SetVisible(false);
}

void FrAwardForm::OnExpEnd0Init(int param)
{
	m_pExpEnd[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExpEnd[0]->SetVisible(false);
}

void FrAwardForm::OnExpEnd1Init(int param)
{
	m_pExpEnd[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExpEnd[1]->SetVisible(false);
}

void FrAwardForm::OnExpEnd2Init(int param)
{
	m_pExpEnd[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExpEnd[2]->SetVisible(false);
}

void FrAwardForm::OnExpEnd3Init(int param)
{
	m_pExpEnd[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	m_pExpEnd[3]->SetVisible(false);
}

void FrAwardForm::OnLevel0Init(int param)
{
	m_pLevel[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevel[0]->SetVisible(false);
}

void FrAwardForm::OnLevel1Init(int param)
{
	m_pLevel[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevel[1]->SetVisible(false);
}

void FrAwardForm::OnLevel2Init(int param)
{
	m_pLevel[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevel[2]->SetVisible(false);
}

void FrAwardForm::OnLevel3Init(int param)
{
	m_pLevel[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevel[3]->SetVisible(false);
}

void FrAwardForm::OnLevelUp0Init(int param)
{
	m_pLevelUp[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevelUp[0]->SetVisible(false);
}

void FrAwardForm::OnLevelUp1Init(int param)
{
	m_pLevelUp[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevelUp[1]->SetVisible(false);
}

void FrAwardForm::OnLevelUp2Init(int param)
{
	m_pLevelUp[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevelUp[2]->SetVisible(false);
}

void FrAwardForm::OnLevelUp3Init(int param)
{
	m_pLevelUp[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pLevelUp[3]->SetVisible(false);
}

void FrAwardForm::OnMatchRes0Init(int param)
{
	m_pMatchRes[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE)
		m_pMatchRes[0]->SetVisible(false);
}

void FrAwardForm::OnMatchRes1Init(int param)
{
	m_pMatchRes[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE)
		m_pMatchRes[1]->SetVisible(false);
}

void FrAwardForm::OnGainedLadderPoint0Init(int param)
{
	m_pGainedLadderPoint[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrAwardForm::OnGainedLadderPoint1Init(int param)
{
	m_pGainedLadderPoint[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrAwardForm::OnLadderPoint0Init(int param)
{
	m_pLadderPoint[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrAwardForm::OnLadderPoint1Init(int param)
{
	m_pLadderPoint[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrAwardForm::OnNextLevelTextInit(int param)
{
	if (!m_pNextLevelText)
	{
		m_pNextLevelText = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	}
}

bool FrAwardForm::OnInit()
{
	FrForm::OnInit();

	if (m_pUserList)
	{
		m_pUserList->EnableScrollBar(false);

		WRect rect = m_pUserList->GetRect();
		float itemHeight = (float)m_pUserList->GetItemHeight();
		float height = itemHeight * m_playerNum;
		float diff = height - rect.h;
		rect.h = height;
		m_pUserList->SetRect(rect);

		m_rect.h += diff;
		WPoint pos;
		pos.x = m_rect.x;
		pos.y = g_view->GetHeight() - m_rect.h + 5.f;
		MoveWindow(pos);
		SetRect(m_rect);

		float textHeight = m_pNextLevelText->GetRect().h;
		pos.x = m_rect.x + 25.f;
		pos.y = g_view->GetHeight() - textHeight;
		m_pNextLevelText->MoveWindow(pos);
	}

	if (GetGameType() == 0)
	{
		CalcResult();
	}
	else if (GetGameType() == 1)
	{
		float offset = 0.f;
		if (m_playerNum < 4)
			offset = -3.f;

		for (int i = 0; i < 4; i++)
		{
			bool bUp;
			if (PLAYER(m_order[i])->teammate != 0xff)
				bUp = m_order[i] < PLAYER(m_order[i])->teammate;
			else
				bUp = true;

			WPoint pos;
			const WRect& expBarRect = m_pExpBar[i]->GetRect();
			pos = WPoint(expBarRect.x,
				(bUp ? offset + 3.f : offset - 3.f) + expBarRect.y);
			m_pExpBar[i]->MoveWindow(pos);

			const WRect& expRect = m_pExp[i]->GetRect();
			pos = WPoint(expRect.x,
				(bUp ? offset + 3.f : offset - 3.f) + expRect.y);
			m_pExp[i]->MoveWindow(pos);

			const WRect& expEndRect = m_pExpEnd[i]->GetRect();
			pos = WPoint(expEndRect.x,
				(bUp ? offset + 3.f : offset - 3.f) + expEndRect.y);
			m_pExpEnd[i]->MoveWindow(pos);

			const WRect& levelRect = m_pLevel[i]->GetRect();
			pos = WPoint(levelRect.x, (bUp ? 3.f : -3.f) + levelRect.y);
			pos = WPoint(levelRect.x,
				(bUp ? offset + 3.f : offset - 3.f) + levelRect.y);
			m_pLevel[i]->MoveWindow(pos);

			const WRect& levelUpRect = m_pLevelUp[i]->GetRect();
			pos = WPoint(levelUpRect.x,
				(bUp ? offset + 3.f : offset - 3.f) + levelUpRect.y);
			m_pLevelUp[i]->MoveWindow(pos);
		}

		if ((unsigned char)GOLFDOC()->m_teamGameWinner == 2)
		{
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");
		}
		else
		{
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");
		}

		if (m_playerNum < 4)
		{
			for (int i = 0; i < 2; i++)
				m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x,
					(i == 0 ? -18.f : -43.f) + m_pMatchRes[i]->GetRect().y));
		}

		CalcResult();
	}
	else if (GetGameType() == 7)
	{
		if (PLAYER(0)->rank == PLAYER(1)->rank)
		{
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");
			m_pMatchRes[0]->MoveWindow(WPoint(m_pMatchRes[0]->GetRect().x,
				m_pMatchRes[0]->GetRect().y - 18.f));
			m_pMatchRes[1]->SetVisible(false);
		}
		else
		{
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");

			for (int i = 0; i < 2; i++)
				m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x,
					(i == 0 ? -18.f : -43.f) + m_pMatchRes[i]->GetRect().y));
		}

		CalcResult();
	}
	else if (GetGameType() == 3)
	{
		if (PLAYER(0)->rank == PLAYER(1)->rank)
		{
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");
		}
		else
		{
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");
		}

		for (int i = 0; i < 2; i++)
			m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x,
				(i == 0 ? -18.f : -43.f) + m_pMatchRes[i]->GetRect().y));

		if (PLAYER(0)->state == 3 || PLAYER(1)->state == 3)
			m_pMatchRes[1]->SetVisible(false);

		CalcResultMatch();
	}

	m_pPCIconVS = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_vs");
	m_pPCIconBonus =
		g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_bonus_20");
	m_pBonusMascot[0] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_01");
	m_pBonusMascot[1] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_02");
	m_pBonusMascot[2] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_09");
	m_pBonusMascot[3] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_10");
	m_pBonusMascot[4] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_11");
	m_pBonusMascot[5] =
		g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_12");
	m_pBonusMastery = g_pFresh->GetManager()->GetBitmap("golf", "bonus_ma");
	m_pBonusMastery2 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_ma2");
	m_pZeroIconVS = g_pFresh->GetManager()->GetBitmap("golf", "icon_zero_vs");

	m_pNextLevelText->SetVisible(IsLocalContent((localContentType_t)41));

	for (int i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->oid == Doc()->m_myInfo.info.dwGuid)
		{
			CGolfDoc* golf = GOLFDOC();
			unsigned long exp =
				Doc()->m_userInfo[i].stat.dwExp + golf->m_gameResult[i].exp;
			if (exp > Doc()->m_levelTable[m_level[i]].exp)
			{
				m_pNextLevelText->SetVisible(false);
			}
			else
			{
				std::string text;

				if (m_level[i] < 21)
				{
					text.assign(s_levelUpText[m_level[i]][2]);
				}
				else if (m_level[i] <= 65)
				{
					text.assign(g_LevelTable[m_level[i] + 1].name);
					text += s_levelUpText[21][2];
				}

				m_pNextLevelText->AddLine(text.c_str(), 0, false);
			}
		}
	}

	int result = 0;
	if (GetGameType() == 1)
	{
		if (TEAM(PLAYER(MyIndex())->team)->score <
			TEAM(PLAYER(MyIndex())->team == 0)->score)
			result = 2;

		if (TEAM(PLAYER(MyIndex())->team)->score ==
			TEAM(PLAYER(MyIndex())->team == 0)->score)
			result = 1;
	}
	else if (GetGameType() == 0)
	{
		int myScore = PLAYER(MyIndex())->score;
		int myPang = PLAYER(MyIndex())->totalPang;

		int best = 0;
		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (i == MyIndex())
				continue;

			if (PLAYER(i)->score < PLAYER(best)->score ||
				(PLAYER(i)->score == PLAYER(best)->score &&
					PLAYER(i)->totalPang > PLAYER(best)->totalPang))
				best = i;

			if (PLAYER(i)->score < myScore ||
				(PLAYER(i)->score == myScore && PLAYER(i)->totalPang > myPang))
				result = 2;
		}

		if (result)
		{
			if (PLAYER(best)->score == myScore &&
				PLAYER(best)->totalPang == myPang)
				result = 1;
		}
	}
	else if (GetGameType() == 7)
	{
		if (Doc()->m_myInfo.info.dwGuid != GOLFDOC()->m_skinsDonator)
		{
			result = 2;
		}
		else if (!GOLFDOC()->m_skinsDonator)
		{
			result = 1;
		}
	}

	int state;
	switch (result)
	{
	case 0:
		state = 0x40;
		break;
	case 2:
		state = 0x200;
		break;
	default:
		return true;
	}

	CVoiceItem::Instance()->PlaySoundA(
		Doc()->m_userInfo[MyIndex()].clubInfo.tid,
		(CVoiceItem::eWaveState)state,
		Doc()->m_userInfo[MyIndex()].charInfo.tid & 0x3ffffff);
	return true;
}

void FrAwardForm::OnProc(const float delta)
{
	if (GetGameType() == 3)
		return;

	m_tickTime += delta;

	if (m_tickTime < 1.f)
		return;

	int i = 0;
	if (!m_pExpSound && !m_bFinished[MyIndex()])
	{
		if (m_gainExp[MyIndex()] > 0)
			m_pExpSound = "player_exp_up";
		else if (m_gainExp[MyIndex()] < 0)
			m_pExpSound = "player_exp_up";

		if (m_pExpSound && !g_audio->IsPlaying(m_pExpSound))
			g_audio->PlaySfx(m_pExpSound, NULL, 1, m_pExpSound);
	}

	if (m_tickTime - 1.f < .01f)
		return;

	char buffer[32];
	int amount = (int)((m_tickTime - 1.f) * 100.f);
	m_tickTime -= amount * .01f;

	for (i = 0; i < GetPlayerNum(); i++)
	{
		if (m_bFinished[i] || (GetGameType() != 1 && PLAYER(i)->state == 3))
			continue;

		int maxExp = Doc()->m_levelTable[m_level[i]].exp * 10;

		if (amount > (m_gainExp[i] > 0 ? m_gainExp[i] : -m_gainExp[i]))
		{
			m_bFinished[i] = true;

			amount = m_gainExp[i] > 0 ? m_gainExp[i] : -m_gainExp[i];
			m_curExp[i] += (m_gainExp[i] > 0 ? 1 : -1) * amount;
			m_gainExp[i] = 0;
		}
		else
		{
			m_curExp[i] += (m_gainExp[i] > 0 ? 1 : -1) * amount;
			m_gainExp[i] -= (m_gainExp[i] > 0 ? 1 : -1) * amount;
		}

		if (m_curExp[i] < 0 && !m_bFinished[i])
		{
			m_bFinished[i] = true;
			m_curExp[i] = 0;
		}

		int index = PLAYER(i)->order - 1;
		if (index < 0 || m_playerNum <= index)
			continue;

		if (m_curExp[i] >= maxExp)
		{
			g_audio->PlaySfx("level_up");

			m_curExp[i] -= maxExp;
			m_level[i]++;

			if (m_pExpBar[index])
			{
				m_pExpBar[index]->SetRange(0,
					Doc()->m_levelTable[m_level[i]].exp * 10, 0);
				m_pExpBar[index]->SetPos(m_curExp[i]);
			}

			if (m_pExpEnd[index])
			{
				sprintf(buffer, "/ %d", Doc()->m_levelTable[m_level[i]].exp);
				m_pExpEnd[index]->SetCaption(buffer);
			}

			if (m_pLevel[index])
			{
				sprintf(buffer, "level_%03d", m_level[i] + 1);
				m_pLevel[index]->SetBgImg(buffer);
			}

			AfxGetTask()->GetActor("Player")
				<< MsgObject(NULL, 504, i, 0, 0, 0, 0);
		}
		else
		{
			if (m_pExpBar[index])
				m_pExpBar[index]->SetPos(m_curExp[i]);
		}

		sprintf(buffer, "%d", m_curExp[i] / 10);
		m_pExp[index]->SetCaption(buffer);

		if (m_bFinished[i])
		{
			if (m_level[i] != Doc()->m_userInfo[i].stat.Level)
			{
				if (m_pExp[index])
					m_pExp[index]->SetVisible(false);

				if (m_pExpEnd[index])
					m_pExpEnd[index]->SetVisible(false);

				if (m_pLevelUp[index])
					m_pLevelUp[index]->SetVisible(true);

				std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				for (; it != Doc()->m_slotList.end(); ++it)
				{
					if ((*it).dwGuid == Doc()->m_userInfo[i].info.dwGuid)
					{
						(*it).level = m_level[i];
						break;
					}
				}

				if (CHeadIcon::IsInstantiated())
				{
					unsigned char levels[128];

					for (int j = 0; j < GetPlayerNum(); j++)
					{
						levels[j] = Doc()->m_userInfo[j].stat.Level;
						Doc()->m_userInfo[j].stat.Level = m_level[j];
					}

					CHeadIcon::Instance()->SetLevel();

					for (int j = 0; j < GetPlayerNum(); j++)
						Doc()->m_userInfo[j].stat.Level = levels[j];
				}
			}

			if (i == MyIndex() && m_pExpSound)
				g_audio->StopSfx(m_pExpSound);
		}
	}

	m_treasureTime += delta;
	if (CTHunter::Instance()->IsReceiveGiftPacket() && m_treasureTime > 2.f)
		Doc()->m_bTreasureAward = true;
}

void FrAwardForm::CalcResult()
{
	char buffer[32];

	for (int i = 0; i < GetPlayerNum(); i++)
	{
		if (GetGameType() != 1 && PLAYER(i)->state == 3)
			continue;

		sPlayerData* pPlayer = PLAYER(i);
		int index = pPlayer->order - 1;

		if (index < 0 || m_playerNum <= index)
			continue;

		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			m_level[i] = Doc()->m_userInfo[i].stat.Level;
			m_curExp[i] = Doc()->m_userInfo[i].stat.dwExp;
			m_gainExp[i] = GOLFDOC()->m_gameResult[i].exp;
		}
		else
		{
			unsigned char matchCount = 0;
			unsigned char count2 = 0;
			unsigned char count3 = 0;
			unsigned char count4 = 0;

			for (int j = 0; j < GetHoles(); j++)
			{
				if (GetGameType() == 0)
				{
					switch (
						GOLFDOC()->GetHoleData(Doc()->m_holeOrder[j]).playerNum)
					{
					case 2:
						count2++;
						break;
					case 3:
						count3++;
						break;
					case 4:
						count4++;
						break;
					}
				}
				else if (GetGameType() == 1)
				{
					if (GOLFDOC()->GetHoleData(j).playerNum == 4)
						matchCount++;
				}
			}

			m_level[i] = Doc()->m_userInfo[i].stat.Level;
			m_curExp[i] = Doc()->m_userInfo[i].stat.dwExp;

			if (GetGameType() == 0)
			{
				unsigned char difficulty =
					(!GetCurMapProp()
							? (unsigned char)1
							: (unsigned char)GetCurMapProp()->Difficulty) -
					1;
				m_gainExp[i] =
					g_ExperienceTable[difficulty][count2][pPlayer->rank + 1] +
					g_ExperienceTable[difficulty][count3][pPlayer->rank + 3] +
					g_ExperienceTable[difficulty][count4][pPlayer->rank + 6];
			}
			else if (GetGameType() == 1)
			{
				bool bWin;
				if (GOLFDOC()->m_teamGameWinner == 2 ||
					pPlayer->team == GOLFDOC()->m_teamGameWinner)
					bWin = false;
				else
					bWin = true;

				unsigned char difficulty =
					(!GetCurMapProp()
							? (unsigned char)1
							: (unsigned char)GetCurMapProp()->Difficulty) -
					1;
				m_gainExp[i] = g_ExperienceTable[difficulty][matchCount][bWin];
			}
		}

		m_curExp[i] *= 10;
		m_gainExp[i] *= 10;
		int maxExp = Doc()->m_levelTable[m_level[i]].exp * 10;

		if (!m_gainExp[i])
			m_bFinished[i] = true;

		if (m_pExpBar[index])
		{
			m_pExpBar[index]->SetRange(0, maxExp, 0);
			m_pExpBar[index]->SetPos(m_curExp[i]);
		}

		if (m_pExp[index])
		{
			sprintf(buffer, "%d", m_curExp[i] / 10);
			m_pExp[index]->SetCaption(buffer);
		}

		if (m_pExpEnd[index])
		{
			sprintf(buffer, "/ %d", Doc()->m_levelTable[m_level[i]].exp);
			m_pExpEnd[index]->SetCaption(buffer);
		}

		if (m_pLevel[index])
		{
			sprintf(buffer, "level_%03d", m_level[i] + 1);
			m_pLevel[index]->SetBgImg(buffer);
		}

		if (m_pExpBar[index])
			m_pExpBar[index]->SetVisible(true);

		if (m_pExp[index])
			m_pExp[index]->SetVisible(true);

		if (m_pExpEnd[index])
			m_pExpEnd[index]->SetVisible(true);

		if (m_pLevel[index])
			m_pLevel[index]->SetVisible(true);
	}
}

void FrAwardForm::CalcResultMatch()
{
	char buffer[32] = "";

	for (int i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
			continue;

		sPlayerData* pPlayer = PLAYER(i);
		int index = pPlayer->order - 1;

		if (index < 0 || m_playerNum <= index)
			continue;

		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			m_curExp[i] = Doc()->m_userInfo[i].stat.dwLadderPoint;
			m_gainExp[i] = GOLFDOC()->m_gameResult[i].exp;
			m_curExp[i] += m_gainExp[i];
		}
		else
		{
			unsigned char matchCount = 0;
			unsigned char count2 = 0;
			unsigned char count3 = 0;
			unsigned char count4 = 0;

			for (int j = 0; j < GetHoles(); j++)
			{
				if (GetGameType() == 0)
				{
					switch (
						GOLFDOC()->GetHoleData(Doc()->m_holeOrder[j]).playerNum)
					{
					case 2:
						count2++;
						break;
					case 3:
						count3++;
						break;
					case 4:
						count4++;
						break;
					}
				}
				else if (GetGameType() == 1)
				{
					if (GOLFDOC()->GetHoleData(j).playerNum == 4)
						matchCount++;
				}
			}

			m_level[i] = Doc()->m_userInfo[i].stat.Level;
			m_curExp[i] = Doc()->m_userInfo[i].stat.dwExp;

			unsigned char difficulty =
				(!GetCurMapProp()
						? (unsigned char)1
						: (unsigned char)GetCurMapProp()->Difficulty) -
				1;
			if (GetGameType() == 0)
			{
				m_gainExp[i] =
					g_ExperienceTable[difficulty][count2][pPlayer->rank + 1] +
					g_ExperienceTable[difficulty][count3][pPlayer->rank + 3] +
					g_ExperienceTable[difficulty][count4][pPlayer->rank + 6];
			}
			else if (GetGameType() == 1)
			{
				bool bWin;
				if (GOLFDOC()->m_teamGameWinner == 2 ||
					pPlayer->team == GOLFDOC()->m_teamGameWinner)
					bWin = false;
				else
					bWin = true;

				m_gainExp[i] = g_ExperienceTable[difficulty][matchCount][bWin];
			}
		}

		if (!m_gainExp[i])
			m_bFinished[i] = true;

		if (m_pGainedLadderPoint[index])
		{
			sprintf(buffer, "%d", m_gainExp[i]);
			m_pGainedLadderPoint[index]->SetCaption(buffer);
			m_pGainedLadderPoint[index]->SetVisible(true);
		}

		if (m_pLadderPoint[index])
		{
			sprintf(buffer, "%d", m_curExp[i]);
			m_pLadderPoint[index]->SetCaption(buffer);
			m_pLadderPoint[index]->SetVisible(true);
		}
	}
}

cAquireItem::cAquireItem()
	: m_maxTime(8.0f)
{
	m_pFont = NULL;
	m_time = 0.0f;

	for (int i = 0; i < GetPlayerNum(); ++i)
	{
		std::vector<unsigned long>& list = Doc()->m_acquireItemList[i];
		for (std::vector<unsigned long>::iterator it = list.begin();
			it != list.end(); ++it)
		{
			unsigned long typeId = *it;
			std::map<unsigned long, int>::iterator found =
				m_prizeSlot[i].m_prizeCount.find(typeId);
			if (found == m_prizeSlot[i].m_prizeCount.end())
			{
				m_prizeSlot[i].m_prizeCount.insert(std::make_pair(typeId, 1));
			}
			else
			{
				(*found).second++;
			}
		}
	}

	m_pItemFrame[0] =
		g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_one");
	m_pItemFrame[1] =
		g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_two");
	m_pItemFrame[2] =
		g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_three");

	m_pBubble[0] = g_pFresh->GetBitmap("golf_text_bubble1");
	m_pBubble[1] = g_pFresh->GetBitmap("golf_text_bubble2");
	m_pBubble[2] = g_pFresh->GetBitmap("golf_text_bubble3");

	static char* s_fontFile[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_fontFile;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pFont = NULL;
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);

	m_pEmoti = new CSimpleUI("[emoti_object.png");
	m_pEmoti->Add("0", WRect(0.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("1", WRect(64.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("2", WRect(128.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("3", WRect(192.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("4", WRect(0.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("5", WRect(64.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("6", WRect(128.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("7", WRect(192.f, 192.f, 64.f, 64.f), false);
}

cAquireItem::~cAquireItem()
{
	if (m_pEmoti)
	{
		delete m_pEmoti;
		m_pEmoti = NULL;
	}

	if (m_pFont)
		m_pFont->Erase();

	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}

	for (int i = 0; i < 4; ++i)
	{
		for (std::map<unsigned long, WOverlay*>::iterator it =
				 m_overlay[i].begin();
			it != m_overlay[i].end(); ++it)
		{
			if (g_resrcmng && (*it).second)
			{
				g_resrcmng->Release((*it).second);
				(*it).second = NULL;
			}
		}
		m_overlay[i].clear();
	}
}

void cAquireItem::Reset()
{
	m_time = 0.0f;
}

void cAquireItem::OnProc(float delta)
{
	if (m_time < m_maxTime)
		m_time += delta * 20.0f;
}

void cAquireItem::DisplayPrizeList(int index)
{
	unsigned int count = m_prizeSlot[index].m_prizeCount.size();
	float width = 0.0f;
	float offset = 0.0f;

	if (count <= 0)
		return;

	WVector pos = GetPlayer2DPos(index);

	if (pos.z <= 0.0f || 1.0f <= pos.z)
		return;

	float alpha = (m_maxTime < m_time ? m_maxTime : m_time) / m_maxTime;
	unsigned long diffuse = ((int)(alpha * 255.0f) << 24) | 0x00ffffff;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi && m_pBubble[0])
	{
		float y = pos.y + 65.0f;
		gdi->DrawTexture(m_pBubble[0],
			WRect(pos.x, y, (float)m_pBubble[0]->Width(),
				(float)m_pBubble[0]->Height()),
			diffuse, 0);

		width = (float)m_pBubble[0]->Width();
		WRect rect(pos.x + width, y,
			(float)m_pBubble[1]->Width() * (float)(count / 2),
			(float)m_pBubble[1]->Height());
		gdi->DrawTexture(m_pBubble[1],
			WRect(0.0f, 0.0f, (float)m_pBubble[1]->Width(),
				(float)m_pBubble[1]->Height()),
			rect, diffuse, 0);

		width += (float)m_pBubble[1]->Width() * (float)(count / 2);
		gdi->DrawTexture(m_pBubble[2],
			WRect(pos.x + width, y, (float)m_pBubble[2]->Width(),
				(float)m_pBubble[2]->Height()),
			diffuse, 0);
	}

	offset = 5.0f;
	std::map<unsigned long, int>::iterator it =
		m_prizeSlot[index].m_prizeCount.begin();
	for (; it != m_prizeSlot[index].m_prizeCount.end(); ++it)
	{
		unsigned long typeId = (*it).first;
		WOverlay* overlay = NULL;

		std::map<unsigned long, WOverlay*>::iterator found =
			m_overlay[index].find(typeId);
		if (found != m_overlay[index].end())
		{
			overlay = (*found).second;
		}
		else
		{
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
			if (item)
			{
				switch (typeId >> 26)
				{
				case 2:
					overlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga", item->Icon), 0);
					break;
				case 5:
					overlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga", item->Icon), 0);
					break;
				case 6:
					overlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga", item->Icon), 0);
					break;
				case 29:
					overlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga", item->Icon), 0);
					break;
				}
				if (overlay)
				{
					m_overlay[index].insert(
						std::map<unsigned long, WOverlay*>::value_type(typeId,
							overlay));
				}
			}
		}

		if (overlay)
		{
			WRect dst;
			dst.x = pos.x + offset;
			float y = pos.y + 82.0f;
			dst.y = y;
			dst.w = overlay->GetWidth() * 0.65f;
			dst.h = overlay->GetHeight() * 0.65f;
			overlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), dst, 0,
				diffuse, 0.0f, 0);

			if ((*it).second > 1)
			{
				char* text = MakeStr("%d", (*it).second);
				m_pFont->Print(g_view, pos.x + offset + 17.0f, y + 12.0f, text,
					0, diffuse, NULL);
			}

			offset += 40.0f;
		}
	}

	if (m_pEmoti && m_time < m_maxTime)
	{
		m_pEmoti->Render(MakeStr("%d", (int)m_time), pos.x - 64.0f,
			pos.y - 16.0f, 2.0f, 0);
	}
}

WVector cAquireItem::GetPlayer2DPos(int index)
{
	WMatrix mat;
	AfxGetTask()->GetActor("Player")
		<< MsgObject(NULL, 0x124, index, (int)&mat, 0, 0, 0);
	WVector pos = g_view->Projection(mat.pivot);
	return WVector(pos.x, pos.y, pos.z);
}

IMPLEMENT_OBJECT(FrScoreForm, FrForm)

BEGIN_FRESH_MSGMAP(FrScoreForm, FrForm)
ON_FRESH_VI("bar2", FRCMD_INIT, FrScoreForm::OnBar2Init)
ON_FRESH_VI("bar3", FRCMD_INIT, FrScoreForm::OnBar3Init)
ON_FRESH_VI("front9btn", FRCMD_INIT, FrScoreForm::OnFront9BtnInit)
ON_FRESH_VV("front9btn", FRCMD_LBUTTONUP, FrScoreForm::OnFront9BtnUp)
ON_FRESH_VI("back9btn", FRCMD_INIT, FrScoreForm::OnBack9BtnInit)
ON_FRESH_VV("back9btn", FRCMD_LBUTTONUP, FrScoreForm::OnBack9BtnUp)
ON_FRESH_VI("shotbtn", FRCMD_INIT, FrScoreForm::OnShotBtnInit)
ON_FRESH_VV("shotbtn", FRCMD_LBUTTONUP, FrScoreForm::OnShotBtnUp)
ON_FRESH_VI("graphbtn", FRCMD_INIT, FrScoreForm::OnGraphBtnInit)
ON_FRESH_VV("graphbtn", FRCMD_LBUTTONUP, FrScoreForm::OnGraphBtnUp)
ON_FRESH_VI("versusbtn", FRCMD_INIT, FrScoreForm::OnVersusBtnInit)
ON_FRESH_VV("versusbtn", FRCMD_LBUTTONUP, FrScoreForm::OnVersusBtnUp)
ON_FRESH_VI("puttbtn", FRCMD_INIT, FrScoreForm::OnPuttBtnInit)
ON_FRESH_VV("puttbtn", FRCMD_LBUTTONUP, FrScoreForm::OnPuttBtnUp)
ON_FRESH_VI("pangbtn", FRCMD_INIT, FrScoreForm::OnPangBtnInit)
ON_FRESH_VV("pangbtn", FRCMD_LBUTTONUP, FrScoreForm::OnPangBtnUp)
ON_FRESH_VI("hole_list", FRCMD_INIT, FrScoreForm::OnHoleListInit)
ON_FRESH_VI("hole_list", FRCMD_OWNERDRAW, FrScoreForm::OnHoleListOwnerDraw)
ON_FRESH_VI("par_list", FRCMD_INIT, FrScoreForm::OnParListInit)
ON_FRESH_VI("par_list", FRCMD_OWNERDRAW, FrScoreForm::OnParListOwnerDraw)
ON_FRESH_VI("pang_list", FRCMD_INIT, FrScoreForm::OnPangListInit)
ON_FRESH_VI("pang_list", FRCMD_OWNERDRAW, FrScoreForm::OnPangListOwnerDraw)
ON_FRESH_VI("golfer_list", FRCMD_INIT, FrScoreForm::OnGolferListInit)
ON_FRESH_VI("golfer_list", FRCMD_OWNERDRAW, FrScoreForm::OnGolferListOwnerDraw)
ON_FRESH_VI("record_list", FRCMD_INIT, FrScoreForm::OnRecordListInit)
ON_FRESH_VI("record_list", FRCMD_OWNERDRAW, FrScoreForm::OnRecordListOwnerDraw)
ON_FRESH_VI("total_list", FRCMD_INIT, FrScoreForm::OnTotalListInit)
ON_FRESH_VI("total_list", FRCMD_OWNERDRAW, FrScoreForm::OnTotalListOwnerDraw)
ON_FRESH_VI("total", FRCMD_INIT, FrScoreForm::OnTotalInit)
ON_FRESH_VI("ppl", FRCMD_INIT, FrScoreForm::OnPPLInit)
ON_FRESH_VI("board1", FRCMD_INIT, FrScoreForm::OnBoard1Init)
ON_FRESH_VI("board2", FRCMD_INIT, FrScoreForm::OnBoard2Init)
ON_FRESH_VI("drawlines", FRCMD_INIT, FrScoreForm::OnDrawLinesInit)
ON_FRESH_VI("drawlines", FRCMD_OWNERDRAW, FrScoreForm::OnDrawLinesOwnerDraw)
ON_FRESH_VI("cancel", FRCMD_INIT, FrScoreForm::OnCancelBtnInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrScoreForm::OnCancelBtnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrScoreForm::OnCloseBtnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrScoreForm::OnCloseBtnUp)
ON_FRESH_VI("blankpanel1", FRCMD_INIT, FrScoreForm::OnBlankPanel01Init)
ON_FRESH_VI("blankpanel1", FRCMD_OWNERDRAW,
	FrScoreForm::OnBlankPanel01OwnerDraw)
ON_FRESH_VI("blankpanel2", FRCMD_INIT, FrScoreForm::OnBlankPanel02Init)
ON_FRESH_VI("blankpanel2", FRCMD_OWNERDRAW,
	FrScoreForm::OnBlankPanel02OwnerDraw)
ON_FRESH_VI("blankpanel3", FRCMD_INIT, FrScoreForm::OnBlankPanel03Init)
ON_FRESH_VI("blankpanel3", FRCMD_OWNERDRAW,
	FrScoreForm::OnBlankPanel03OwnerDraw)
END_FRESH_MSGMAP()

FrScoreForm::FrScoreForm()
{
	m_state = STATE_0;

	m_pFront9Btn = NULL;
	m_pBack9Btn = NULL;
	m_pHoleList = NULL;
	m_pParList = NULL;
	m_bFront9 = true;

	m_pCancelBtn = NULL;
	m_pCloseBtn = NULL;

	m_pBar2 = NULL;
	m_pBar3 = NULL;
	m_pPPL = NULL;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_type = 4;
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
		Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
		Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
	{
		m_type = 5;
	}
	else
	{
		m_type = 0;
	}

	m_aniTime = 0;
	m_pPoint = NULL;

	memset(m_pTypeBtn, 0, sizeof(m_pTypeBtn));
	memset(m_pList, 0, sizeof(m_pList));
	memset(m_order, 0, sizeof(m_order));
	memset(m_pDigit, 0, sizeof(m_pDigit));

	m_holePang = 0;
	m_totalPang = 0;
	m_playerNum = 1;

	m_bShowBlankPanel = Doc()->m_roomInfo.realGameType == 14;
	m_pBlankPanel[0] = NULL;
	m_pBlankPanel[1] = NULL;
	m_pBlankPanel[2] = NULL;
}

void FrScoreForm::LoadBitmaps()
{
	const Bitmap* bitmap;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		for (int i = 0; i < 2; ++i)
		{
			bitmap = g_pFresh->GetManager()->GetBitmap("SCORECARD",
				i == 0 ? "red" : "blue");
			if (bitmap)
				m_bitmapMap.insert(
					std::map<unsigned long, const Bitmap*>::value_type(i,
						bitmap));
		}

		m_pResult[0] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
		m_pResult[1] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
		m_pResult[2] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
		m_pResult[3] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
		m_pQuestion =
			g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		m_pResult[0] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
		m_pResult[1] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
		m_pResult[2] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
		m_pResult[3] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
		m_pQuestion =
			g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
	}
	else
	{
		for (int i = 0; i < GetPlayerNum(); ++i)
		{
			if (PLAYER(i)->state != 3)
			{
				unsigned long charId = Doc()->m_userInfo[i].charInfo.tid;

				if (m_bitmapMap.find(charId) == m_bitmapMap.end())
				{
					IFF_STRUCT::sChar* character =
						ItemManager()->FindChar(charId);
					if (character)
					{
						bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS",
							character->c.Icon);
						if (bitmap)
							m_bitmapMap.insert(std::map<unsigned long,
								const Bitmap*>::value_type(charId, bitmap));
					}
				}
			}
		}

		char name[8];
		for (int j = 0; j < 4; ++j)
		{
			switch (j)
			{
			case 0:
				strcpy(name, "1st");
				break;
			case 1:
				strcpy(name, "2nd");
				break;
			case 2:
				strcpy(name, "3rd");
				break;
			case 3:
				strcpy(name, "4th");
				break;
			}

			bitmap = g_pFresh->GetManager()->GetBitmap("SCORECARD", name);
			if (bitmap)
				m_bitmapMap.insert(
					std::map<unsigned long, const Bitmap*>::value_type(j,
						bitmap));

			switch (j)
			{
			case 0:
				strcpy(name, "1st_01");
				break;
			case 1:
				strcpy(name, "2nd_01");
				break;
			case 2:
				strcpy(name, "3rd_01");
				break;
			case 3:
				strcpy(name, "4th_01");
				break;
			}

			bitmap = g_pFresh->GetManager()->GetBitmap("SCORECARD", name);
			if (bitmap)
				m_bitmapMap.insert(
					std::map<unsigned long, const Bitmap*>::value_type(j + 4,
						bitmap));
		}

		if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
		{
			m_pResult[0] =
				g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
			m_pResult[1] =
				g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
			m_pResult[2] =
				g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
			m_pResult[3] = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
			m_pQuestion =
				g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
		}
	}

	for (int i = 0; i < 10; ++i)
		m_pDigit[i] =
			g_pFresh->GetManager()->GetBitmap("SCORECARD", MakeStr("%d", i));

	m_pPlus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "+");
	m_pMinus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");

	m_pPoint = g_pFresh->GetManager()->GetBitmap("SCORECARD", "point");

	m_pArrow = g_pFresh->GetManager()->GetBitmap("SCORECARD", "arrow");

	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass_list");
}

void FrScoreForm::SetState(eState state)
{
	if (m_state == state)
		return;

	m_state = state;

	if (state == STATE_1)
	{
		Enable(false);
		SetVisible(false);
	}
	else
	{
		Enable(true);
		SetVisible(true);

		if (Doc()->m_golfGame.holes == 18)
		{
			bool front = (GetHoleIndex(GOLFDOC()->m_currentHole) < 10 ||
				GOLFDOC()->m_bNoStat);
			m_bFront9 = front;
		}
		else
			m_bFront9 = true;

		SetFrontBackBtn();
	}

	if (m_pPPL)
		m_pPPL->SetVisible(m_bShowPPL && m_state == STATE_3);
}

void FrScoreForm::Resize()
{
	float offset = 0.0f;
	float itemHeight = 0.0f;
	WRect rect = GetRect();

	if (m_pList[0])
	{
		itemHeight = (float)m_pList[0]->GetItemHeight();
		offset = 139.0f;
	}

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_TEAM:
	case GAME_TYPE_GUILD_MATCH:
		rect.w = 547.0f;
		rect.h = itemHeight * 2 + offset;
		break;
	case GAME_TYPE_MATCH:
		rect.w = 547.0f;
		rect.h = GOLFDOC()->GetCurrentPlayerNum() * itemHeight + offset;
		break;
	case GAME_TYPE_SKINS:
		rect.w = 648.0f;
		rect.h = GOLFDOC()->GetPlayerNum() * itemHeight + offset + 22.0f;
		break;
	default:
		rect.w = 612.0f;
		rect.h = GOLFDOC()->GetCurrentPlayerNum() * itemHeight + offset;
		break;
	}

	ResizeScoreBar23(m_pBar2, m_pBar3, rect.x, rect.w);
	SetRect(rect);

	WPoint pos((float)(int)((g_view->GetWidth() - rect.w) * 0.5f),
		(float)(int)((itemHeight * 4 + offset - rect.h) * 0.5f) + 20.0f);
	MoveWindow(pos);
}

void FrScoreForm::SortLists(bool bResize)
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE ||
		Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
	{
		m_playerNum = GOLFDOC()->GetCurrentPlayerNum();
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
		Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		m_playerNum = 2;
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_playerNum = GetPlayerNum();
	}
	else
	{
		m_playerNum = GOLFDOC()->GetCurrentPlayerNum();
	}

	if (bResize)
		Resize();

	for (int i = 0; i < 3; ++i)
	{
		if (m_pList[i])
			m_pList[i]->ClearItem();
	}

	SortPlayerOrder(m_order);

	for (int i = 0; i < 3; ++i)
	{
		if (m_pList[i])
		{
			for (int j = 0; j < m_playerNum; ++j)
				m_pList[i]->AddItem((void*)m_order[j]);
		}
	}
}

void FrScoreForm::ChangeTurn()
{
	UpdatePang();
}

void FrScoreForm::UpdatePang()
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		m_holePang = TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
						 ->pang[GOLFDOC()->m_currentHole - 1];
		m_totalPang = TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->totalPang;
	}
	else
	{
		m_holePang = PLAYER(GOLFDOC()->m_currentPlayer)
						 ->pang[GOLFDOC()->m_currentHole - 1];
		m_totalPang = PLAYER(GOLFDOC()->m_currentPlayer)->totalPang;
	}
}

void FrScoreForm::OnBar2Init(int param)
{
	m_pBar2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrScoreForm::OnBar3Init(int param)
{
	m_pBar3 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrScoreForm::OnFront9BtnInit(int param)
{
	m_pFront9Btn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (Doc()->m_golfGame.holes != 18)
		m_pFront9Btn->SetVisible(false);
}

void FrScoreForm::OnFront9BtnUp()
{
	if (Doc()->m_golfGame.holes != 18)
		return;
	m_bFront9 = true;
	SetFrontBackBtn();
}

void FrScoreForm::OnBack9BtnInit(int param)
{
	m_pBack9Btn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (Doc()->m_golfGame.holes != 18)
		m_pBack9Btn->SetVisible(false);
}

void FrScoreForm::OnBack9BtnUp()
{
	if (Doc()->m_golfGame.holes != 18)
		return;
	m_bFront9 = false;
	SetFrontBackBtn();
}

void FrScoreForm::OnShotBtnUp()
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		if (m_type == 4)
			return;
		m_type = 4;
	}
	else
	{
		if (m_type == 0)
			return;
		m_type = 0;
	}

	SetTypeBtn();
}

void FrScoreForm::OnGraphBtnUp()
{
	if (m_type == 1)
		return;
	m_type = 1;
	SetTypeBtn();
}

void FrScoreForm::OnVersusBtnUp()
{
	if (m_type == 5)
		return;
	m_type = 5;
	SetTypeBtn();
}

void FrScoreForm::OnPuttBtnUp()
{
	if (m_type == 2)
		return;
	m_type = 2;
	SetTypeBtn();
}

void FrScoreForm::OnPangBtnUp()
{
	if (m_type == 3)
		return;
	m_type = 3;
	SetTypeBtn();
}

void FrScoreForm::OnShotBtnInit(int param)
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pTypeBtn[4] = DYNAMIC_CAST(FrButton, (FrWnd*)param);

		if (m_pTypeBtn[4])
		{
			m_pTypeBtn[4]->SetButtonImg("skin_n", FrButton::NORMAL);
			m_pTypeBtn[4]->SetButtonImg("skin_o", FrButton::OVER);
			m_pTypeBtn[4]->SetButtonImg("skin_o", FrButton::PRESSED);
		}
	}
	else
	{
		m_pTypeBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
		Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
		Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
		m_pTypeBtn[0]->SetVisible(false);
}

void FrScoreForm::OnGraphBtnInit(int param)
{
	m_pTypeBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTypeBtn[1])
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
			m_pTypeBtn[1]->SetVisible(false);
	}
}

void FrScoreForm::OnPuttBtnInit(int param)
{
	m_pTypeBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTypeBtn[2])
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
			m_pTypeBtn[2]->SetVisible(false);
	}
}

void FrScoreForm::OnPangBtnInit(int param)
{
	m_pTypeBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrScoreForm::OnVersusBtnInit(int param)
{
	m_pTypeBtn[5] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTypeBtn[5])
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
		{
			if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
				Doc()->m_golfGame.gameType == GAME_TYPE_MATCH)
				m_pTypeBtn[5]->MoveWindow(WPoint(m_pTypeBtn[0]->GetRect().x,
					m_pTypeBtn[0]->GetRect().y));
			m_pTypeBtn[5]->SetVisible(true);
		}
		else
		{
			m_pTypeBtn[5]->SetVisible(false);
		}
	}
}

void FrScoreForm::OnHoleListInit(int param)
{
	m_pHoleList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pHoleList)
		m_pHoleList->AddItem(NULL);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pHoleList->SetItemSize(470, 22);
		m_pHoleList->SetRect(
			WRect(m_pHoleList->GetRect().x - 22.0f, m_pHoleList->GetRect().y,
				m_pHoleList->GetRect().w, m_pHoleList->GetRect().h));
	}
}

void FrScoreForm::OnHoleListOwnerDraw(int param)
{
	if (m_bShowBlankPanel)
		return;

	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pHoleList->GetItemWidth();
	float height = (float)m_pHoleList->GetItemHeight();

	WPoint pos;
	pos.x = width / 18.0f + item->pos.x;
	pos.y = height * 0.5f + item->pos.y - 7.0f;

	int begin = m_bFront9 ? 0 : 9;
	int end = min(begin + 9, (int)GetHoles());
	for (int i = begin; i < end; ++i)
	{
		DrawDigit(gdi, MakeStr("%d", Doc()->m_holeOrder[i]), pos, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		pos.x = (float)(int)(pos.x + width / 9.0f);
	}
}

void FrScoreForm::OnParListInit(int param)
{
	m_pParList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pParList)
		m_pParList->AddItem(NULL);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pParList->SetItemSize(470, 22);
		m_pParList->SetRect(
			WRect(m_pParList->GetRect().x - 22.0f, m_pParList->GetRect().y,
				m_pParList->GetRect().w, m_pParList->GetRect().h));
	}
}

void FrScoreForm::OnParListOwnerDraw(int param)
{
	if (m_bShowBlankPanel)
		return;

	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pParList->GetItemWidth();
	float height = (float)m_pParList->GetItemHeight();

	float offset = width / 18.0f;
	WPoint pos;
	pos.x = offset + item->pos.x;
	pos.y = height * 0.5f + item->pos.y - 7.0f;

	int begin = m_bFront9 ? 0 : 9;
	int end = min(begin + 9, (int)GetHoles());
	int total = 0;
	for (int i = begin; i < end; ++i)
	{
		total += GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par;
		DrawDigit(gdi,
			MakeStr("%d", GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par),
			pos, 0xff9ff5b8, FrGDI::ALIGN_CENTER, false);
		pos.x = (float)(int)(pos.x + width / 9.0f);
	}

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_TEAM:
	case GAME_TYPE_MATCH:
	case GAME_TYPE_GUILD_MATCH:
		break;
	case GAME_TYPE_SKINS:
		pos.x = (float)float2int(offset + item->pos.x + 473.0f);
		DrawDigit(gdi, MakeStr("%d", total), pos, 0xff9ff5b8,
			FrGDI::ALIGN_CENTER, false);
		break;
	default:
		pos.x = (float)(int)(offset + item->pos.x + 355.0f);
		DrawDigit(gdi, MakeStr("%d", total), pos, 0xff9ff5b8,
			FrGDI::ALIGN_CENTER, false);
		break;
	}
}

void FrScoreForm::OnPangListInit(int param)
{
	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pPangList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
		m_pPangList->AddItem(NULL);

		m_pPangList->SetItemSize(470, 22);
	}
}

void FrScoreForm::OnBlankPanel01Init(int param)
{
	m_pBlankPanel[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[0])
		m_pBlankPanel[0]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnBlankPanel01OwnerDraw(int param)
{
	if (m_pBlankPanel[0])
		m_pBlankPanel[0]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnBlankPanel02Init(int param)
{
	m_pBlankPanel[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[1])
		m_pBlankPanel[1]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnBlankPanel02OwnerDraw(int param)
{
	if (m_pBlankPanel[1])
		m_pBlankPanel[1]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnBlankPanel03Init(int param)
{
	m_pBlankPanel[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[2])
		m_pBlankPanel[2]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnBlankPanel03OwnerDraw(int param)
{
	if (m_pBlankPanel[2])
		m_pBlankPanel[2]->SetVisible(m_bShowBlankPanel);
}

void FrScoreForm::OnPangListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pPangList->GetItemWidth();
	float height = (float)m_pPangList->GetItemHeight();
	float offset = width * 0.055555556f;
	WPoint pos;
	pos.x = offset + item->pos.x;
	int start = m_bFront9 ? 0 : 9;
	pos.y = height * 0.5f + item->pos.y - 7.0f;
	int end;
	if (start + 9 < GetHoles())
		end = start + 9;
	else
		end = GetHoles();

	for (int i = start; i < end; i++)
	{
		unsigned long color =
			GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).carry ? 0xffff0000
																: 0xff000000;
		gdi->SetTextStyle(1);
		gdi->SetTextColor(color, 0xffffffff);
		gdi->PrintText(pos, 1,
			MakeStr("%I64d",
				(int)GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).pang),
			0);
		pos.x = (float)(int)(width * 0.11111111f + pos.x);
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		pos.x = (float)(int)(offset + item->pos.x + 473.0f);
		gdi->SetTextStyle(1);
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->PrintText(pos, 1, MakeStr("%I64d", GOLFDOC()->m_skinsCoursePang),
			0);
	}
}

void FrScoreForm::OnGolferListInit(int param)
{
	m_pList[0] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	for (int i = 0; i < m_playerNum; i++)
		m_pList[0]->AddItem((void*)m_order[i]);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pList[0]->SetItemSize(103, 58);
		m_pList[0]->SetRect(WRect(m_pList[0]->GetRect().x - 1.0f,
			m_pList[0]->GetRect().y + 22.0f, m_pList[0]->GetRect().w,
			m_pList[0]->GetRect().h));
	}
}

void FrScoreForm::OnGolferListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	WPoint pos;
	std::map<unsigned long, const Bitmap*>::iterator it;
	unsigned char index = (unsigned char)item->pData;
	int order;
	for (order = 0; order < m_playerNum; order++)
	{
		if (m_order[order] == index)
			break;
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (OnlinePlay())
		{
			if (index == PLAYER(MyIndex())->team)
			{
				WRect rect(item->pos.x, item->pos.y,
					(float)m_pList[0]->GetItemWidth(),
					(float)m_pList[0]->GetItemHeight());
				gdi->Box(rect, 0x809b9dff);
			}
		}
		else
		{
			if (index == PLAYER(GOLFDOC()->m_currentPlayer)->team)
			{
				WRect rect(item->pos.x, item->pos.y,
					(float)m_pList[0]->GetItemWidth(),
					(float)m_pList[0]->GetItemHeight());
				gdi->Box(rect, 0x809b9dff);
			}
		}

		it = m_bitmapMap.find(index);
		if (it != m_bitmapMap.end())
		{
			const Bitmap* bitmap = (*it).second;
			if (bitmap)
			{
				WRect rect(item->pos.x, item->pos.y + 3.0f,
					(float)bitmap->Width(), (float)bitmap->Height());
				gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			}
		}

		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);

		pos.x = item->pos.x;
		pos.y = item->pos.y + 28.0f;
		for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
		{
			float x = pos.x;
			float y = pos.y;
			if (PLAYER(i)->team == index)
			{
				if (Doc()->m_userInfo[i].info.dwGuildId &&
					!(Doc()->m_userInfo[i].info.dwIdentity & 0x14))
				{
					const Bitmap* emblem =
						NetResourceManager::Instance()->GetEmblemByName(
							Doc()->m_userInfo[i].info.szEmblemName);
					if (emblem)
						gdi->DrawTexture(emblem,
							WRect(x, y - 6.0f, (float)emblem->Width(),
								(float)emblem->Height()),
							0xffffffff, 0);
				}
				if (!CProjectG::Instance()->HidePrivacy())
					g_pFresh->GetManager()->PrintText(WPoint(x + 27.0f, y), 0,
						Doc()->m_userInfo[i].info.sNick, 100.0f, 0xffffffff);
				pos.y += 16.0f;
			}
		}
		return;
	}

	if (OnlinePlay())
	{
		unsigned long color = 0;
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			color = 0x809b9dff;
		else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
		{
			if (PLAYER(index)->state == 3)
				color = 0x80808080;
			else if (PLAYER(index)->oid == MyGuid(true))
				color = 0x809b9dff;
		}
		else
		{
			if (PLAYER(index)->oid == MyGuid(true))
				color = 0x809b9dff;
			else if (PLAYER(index)->state == 3)
				color = 0x80808080;
		}

		if (color)
		{
			WRect rect(item->pos.x, item->pos.y,
				(float)m_pList[0]->GetItemWidth(),
				(float)m_pList[0]->GetItemHeight());
			gdi->Box(rect, color);
		}
	}
	else
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
		{
			if (PLAYER(index)->state == 3)
			{
				WRect rect(item->pos.x, item->pos.y, m_pList[0]->GetItemWidth(),
					m_pList[0]->GetItemHeight());
				gdi->Box(rect, 0x80808080);
			}
		}
		if (index == GOLFDOC()->m_currentPlayer)
		{
			WRect rect(item->pos.x, item->pos.y,
				(float)m_pList[0]->GetItemWidth(),
				(float)m_pList[0]->GetItemHeight());
			gdi->Box(rect, 0x809b9dff);
		}
	}

	pos.x = item->pos.x;
	pos.y = item->pos.y + 36.0f;
	if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		sRivalData* rival = GUILDMATCHUP(index);
		if (rival->guildUID)
		{
			const Bitmap* emblem =
				NetResourceManager::Instance()->GetEmblemByName(
					rival->guildMark);
			if (emblem)
				gdi->DrawTexture(emblem,
					WRect(pos.x, pos.y - 6.0f, (float)emblem->Width(),
						(float)emblem->Height()),
					0xffffffff, 0);
		}
		pos.x += 27.0f;
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
		if (CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pos, 0, rival->nickname, 100.0f,
				0xffffffff);
	}
	else if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		if (PLAYER(index)->state == 3)
			order += 4;

		const Bitmap* bitmap = NULL;
		it = m_bitmapMap.find(order);
		if (it != m_bitmapMap.end())
			bitmap = (*it).second;
		if (bitmap)
		{
			WRect rect(item->pos.x + 43.0f, item->pos.y + 12.0f,
				(float)bitmap->Width(), (float)bitmap->Height());
			gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			if (m_pPCIcon &&
				(bool)((Doc()->m_userInfo[index].info.dwIdentity >> 7) & 1))
			{
				rect.x = item->pos.x + 87.0f;
				rect.y += 4.0f;
				rect.w = (float)m_pPCIcon->Width();
				rect.h = (float)m_pPCIcon->Height();
				gdi->DrawTexture(m_pPCIcon, rect, 0xffffffff, 0);
			}
		}
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
		pos.x += 52.0f;
		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pos, 1,
				Doc()->m_userInfo[index].info.sNick, 100.0f, 0xffffffff);
	}
	else
	{
		const Bitmap* bitmap = NULL;
		it = m_bitmapMap.find(order);
		if (it != m_bitmapMap.end())
			bitmap = (*it).second;
		if (bitmap)
		{
			WRect rect(item->pos.x + 43.0f, item->pos.y + 12.0f,
				(float)bitmap->Width(), (float)bitmap->Height());
			gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
			if (m_pPCIcon &&
				(bool)((Doc()->m_userInfo[index].info.dwIdentity >> 7) & 1))
			{
				rect.x = item->pos.x + 87.0f;
				rect.y += 4.0f;
				rect.w = (float)m_pPCIcon->Width();
				rect.h = (float)m_pPCIcon->Height();
				gdi->DrawTexture(m_pPCIcon, rect, 0xffffffff, 0);
			}
		}
		const Bitmap* emblem = NULL;
		if (Doc()->m_userInfo[index].info.dwGuildId &&
			!(Doc()->m_userInfo[index].info.dwIdentity & 0x14))
			emblem = NetResourceManager::Instance()->GetEmblemByName(
				Doc()->m_userInfo[index].info.szEmblemName);
		if (emblem)
			gdi->DrawTexture(emblem,
				WRect(pos.x, pos.y - 6.0f, (float)emblem->Width(),
					(float)emblem->Height()),
				0xffffffff, 0);
		pos.x += 27.0f;
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
		if (!CProjectG::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pos, 0,
				Doc()->m_userInfo[index].info.sNick, 100.0f, 0xffffffff);
	}
}

void FrScoreForm::OnRecordListInit(int param)
{
	m_pList[1] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pList[1])
	{
		for (int i = 0; i < m_playerNum; i++)
			m_pList[1]->AddItem((void*)m_order[i]);
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
	{
		m_pList[1]->SetItemSize(470, 58);
		m_pList[1]->SetRect(WRect(m_pList[1]->GetRect().x - 24.0f,
			m_pList[1]->GetRect().y + 22.0f, m_pList[1]->GetRect().w,
			m_pList[1]->GetRect().h));
	}
}

void FrScoreForm::OnRecordListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	int index = (int)item->pData;
	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		if (OnlinePlay())
		{
			if (index == PLAYER(MyIndex())->team)
				g_pFresh->GetManager()->GetGDI()->Box(
					WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
						m_pList[1]->GetItemHeight()),
					0x809b9dff);
		}
		else
		{
			if (index == PLAYER(GOLFDOC()->m_currentPlayer)->team)
				g_pFresh->GetManager()->GetGDI()->Box(
					WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
						m_pList[1]->GetItemHeight()),
					0x809b9dff);
		}
	}
	else
	{
		if (OnlinePlay())
		{
			unsigned long color = 0;
			if (PLAYER(index)->oid == MyGuid(true))
				color = 0x809b9dff;
			else if (PLAYER(index)->state == 3)
				color = 0x80808080;

			if (color)
				g_pFresh->GetManager()->GetGDI()->Box(
					WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
						m_pList[1]->GetItemHeight()),
					color);
		}
		else
		{
			if (PLAYER(index)->state == 3)
				g_pFresh->GetManager()->GetGDI()->Box(
					WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
						m_pList[1]->GetItemHeight()),
					0x80808080);
			if (index == GOLFDOC()->m_currentPlayer)
				g_pFresh->GetManager()->GetGDI()->Box(
					WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
						m_pList[1]->GetItemHeight()),
					0x809b9dff);
		}
	}

	switch (m_type)
	{
	case 0:
		DrawShot(item);
		break;
	case 1:
		DrawGraph(item);
		break;
	case 2:
		DrawPutt(item);
		break;
	case 3:
		DrawPang(item);
		break;
	case 4:
		DrawSkins(item);
		break;
	case 5:
		DrawVersus(item);
		break;
	}
}

void FrScoreForm::OnTotalListInit(int param)
{
	m_pList[2] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pList[2])
	{
		for (int i = 0; i < m_playerNum; i++)
			m_pList[2]->AddItem((void*)m_order[i]);

		if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
		{
			m_pList[2]->SetItemSize(63, 58);
			m_pList[2]->SetRect(WRect(m_pList[2]->GetRect().x + 104.0f,
				m_pList[2]->GetRect().y + 22.0f, m_pList[2]->GetRect().w,
				m_pList[2]->GetRect().h));
		}
	}
}

void FrScoreForm::OnTotalListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	float width = (float)m_pList[2]->GetItemWidth();
	float height = (float)m_pList[2]->GetItemHeight();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	int index = (int)item->pData;

	if (GetGameType() == GAME_TYPE_TEAM)
	{
		if (OnlinePlay())
		{
			if (index == PLAYER(MyIndex())->team)
			{
				WRect rect(item->pos.x, item->pos.y,
					m_pList[2]->GetItemWidth() * 0.5f,
					(float)m_pList[2]->GetItemHeight());
				gdi->Box(rect, 0x809b9dff);
			}
		}
		else
		{
			if (index == PLAYER(GOLFDOC()->m_currentPlayer)->team)
			{
				WRect rect(item->pos.x, item->pos.y,
					m_pList[2]->GetItemWidth() * 0.5f,
					(float)m_pList[2]->GetItemHeight());
				gdi->Box(rect, 0x809b9dff);
			}
		}
	}
	else
	{
		if (OnlinePlay())
		{
			unsigned long color = 0;
			if (PLAYER(index)->oid == MyGuid(true))
				color = 0x809b9dff;
			else if (PLAYER(index)->state == 3)
				color = 0x80808080;

			if (color)
			{
				WRect rect(item->pos.x, item->pos.y, m_pList[2]->GetItemWidth(),
					m_pList[2]->GetItemHeight());
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
					Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
					rect.w *= 0.5f;
				gdi->Box(rect, color);
			}
		}
		else
		{
			if (PLAYER(index)->state == 3)
			{
				WRect rect(item->pos.x, item->pos.y, m_pList[2]->GetItemWidth(),
					m_pList[2]->GetItemHeight());
				gdi->Box(rect, 0x80808080);
			}
			if (index == GOLFDOC()->m_currentPlayer)
			{
				WRect rect(item->pos.x, item->pos.y, m_pList[2]->GetItemWidth(),
					m_pList[2]->GetItemHeight());
				if (Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
					Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
					rect.w *= 0.5f;
				gdi->Box(rect, 0x809b9dff);
			}
		}
	}

	WPoint pos;
	pos.y = height * 0.5f + item->pos.y - 6.0f;
	if (GetGameType() == GAME_TYPE_SKINS)
		pos.x = width * 0.5f + item->pos.x;
	else
		pos.x = width * 0.25f + item->pos.x;

	switch (m_type)
	{
	case 3:
		if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
		{
			if (index == PLAYER(GOLFDOC()->m_currentPlayer)->team)
				DrawDigit(gdi, MakeStr("%I64d", m_totalPang), pos, 0xffffffff,
					FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(gdi, MakeStr("%d", (int)TEAM(index)->totalPang), pos,
					0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
		else if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
		{
			DrawDigit(gdi,
				MakeStr("%I64d", (int)GUILDMATCHUP(index)->totalPang), pos,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
		else
		{
			if (index == GOLFDOC()->m_currentPlayer)
				DrawDigit(gdi, MakeStr("%I64d", m_totalPang), pos, 0xffffffff,
					FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(gdi, MakeStr("%d", (int)PLAYER(index)->totalPang),
					pos, 0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
		break;

	case 1:
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
			break;
	case 0:
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			DrawDigit(gdi, "?", pos, 0xffffffff, FrGDI::ALIGN_CENTER, false);
		else
			DrawDigit(gdi, MakeStr("%d", PLAYER(index)->totalStroke), pos,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
		break;

	case 5:
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
			DrawDigit(gdi, MakeStr("%d", GUILDMATCHUP(index)->guildPoint), pos,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
		else if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
			DrawDigit(gdi, MakeStr("%d", TEAM(index)->score), pos, 0xffffffff,
				FrGDI::ALIGN_CENTER, false);
		else
			DrawDigit(gdi, MakeStr("%d", PLAYER(index)->totalStroke), pos,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
		break;

	case 2:
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_TEAM ||
			Doc()->m_golfGame.gameType == GAME_TYPE_MATCH ||
			Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
			break;

		int total = 0;
		for (int i = 0; i < (m_bFront9 ? 9 : 18); i++)
			total += PLAYER(index)->putt[i];
		DrawDigit(gdi, MakeStr("%d", total), pos, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		break;
	}

	case 4:
		gdi->SetTextStyle(1);
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->PrintText(pos, 1, MakeStr("%I64d", PLAYER(index)->skinsSumPang),
			0);
	}

	if (Doc()->m_golfGame.gameType == GAME_TYPE_STROKE || IsMassGame())
	{
		pos.x = width * 0.75f + item->pos.x;
		char* format;
		if (PLAYER(index)->score == 0)
			format = "%d";
		else
			format = "%+d";
		DrawDigit(gdi, MakeStr(format, PLAYER(index)->score), pos, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
	}
}

bool FrScoreForm::OnInit()
{
	Enable(m_state != STATE_1);
	SetVisible(m_state != STATE_1);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_SKINS)
		SetRect(WRect(m_rect.x, m_rect.y, m_rect.w + 36.0f, m_rect.h));

	SortLists(false);
	LoadBitmaps();
	return true;
}

void FrScoreForm::OnProc(const float delta)
{
	if (m_state == STATE_1)
		return;

	if (m_aniTime < 5.0f)
		m_aniTime += delta;
}

void FrScoreForm::OnDraw()
{
	if (Doc()->m_golfGame.gameType == 7)
		return;

	WPoint points[2];
	WPoint& p1 = points[0];
	WPoint& p2 = points[1];

	p1.x = p2.x = GetRect().x + 129.0f;
	p1.y = GetRect().y + 116.0f;
	p2.y = m_playerNum * 58.0f + p1.y;

	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.x = p2.x - 24.0f;
		p2.x = p1.x;
		p1.y += 22.0f;
		p2.y += 22.0f;
	}

	for (int i = 0; i < 10; ++i)
	{
		g_view->DrawLine2D(p1, p2, 0xff000000, 0xff000000, 0);
		p1.x += 38.0f;
		p2.x += 38.0f;
		if (Doc()->m_golfGame.gameType == 7)
		{
			p1.x += 9.0f;
			p2.x += 9.0f;
		}
	}

	p1.x += 27.0f;
	p2.x += 27.0f;
	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.x -= 13.0f;
		p2.x -= 13.0f;
	}
	g_view->DrawLine2D(p1, p2, 0xff000000, 0xff000000, 0);

	p1.y = p2.y = GetRect().y + 174.0f;
	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.y = p2.y + 22.0f;
		p2.y = p1.y;
	}

	for (int i = 0; i < m_playerNum - 1; ++i)
	{
		p1.x = GetRect().x + 3.0f;
		p2.x = GetRect().x + 28.0f;
		g_view->DrawLine2D(p1, p2, 0, 0xff000000, 0);

		p1.x = GetRect().x + GetRect().w - 36.0f;
		p2.x = GetRect().x + GetRect().w - 11.0f;
		g_view->DrawLine2D(p1, p2, 0xff000000, 0, 0);

		p1.x = GetRect().x + 28.0f;
		p2.x = GetRect().x + GetRect().w - 36.0f;
		g_view->DrawLine2D(p1, p2, 0xff000000, 0xff000000, 0);

		p1.y += 58.0f;
		p2.y += 58.0f;
	}
}

void FrScoreForm::SetFrontBackBtn()
{
	m_aniTime = 0;
	if (Doc()->m_golfGame.holes == 18)
	{
		if (m_pFront9Btn)
		{
			m_pFront9Btn->SetVisible(true);
			m_pFront9Btn->SetStatus(
				m_bFront9 ? FrButton::PRESSED : FrButton::NORMAL);
		}
		if (m_pBack9Btn)
		{
			m_pBack9Btn->SetVisible(true);
			m_pBack9Btn->SetStatus(
				m_bFront9 ? FrButton::NORMAL : FrButton::PRESSED);
		}
	}
	else
	{
		if (m_pFront9Btn)
			m_pFront9Btn->SetVisible(false);
		if (m_pBack9Btn)
			m_pBack9Btn->SetVisible(false);
	}
}

void FrScoreForm::SetTypeBtn()
{
	m_aniTime = 0;
	for (int i = 0; i < 6; ++i)
	{
		if (m_pTypeBtn[i])
			m_pTypeBtn[i]->SetStatus(
				m_type == i ? FrButton::PRESSED : FrButton::NORMAL);
	}
}

void FrScoreForm::DrawShot(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + min(1 - (int)(m_aniTime * -20.0f), 9), GetHoles());

	WPoint pos;
	pos.x = (float)(int)(width / 18.0f + item->pos.x);
	pos.y = (float)(int)(0.5f * height + item->pos.y - 6.0f);

	if (Doc()->m_golfGame.gameType == 6)
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!GUILDMATCHUP(index)->holeStroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
				GUILDMATCHUP(index)->state != 1 &&
				(!(GOLFDOC()->GameMode() & 0x22000) ||
					GOLFDOC()->IsNetGameEnd()))
			{
				DrawDigit(gdi, "?", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			}
			else
			{
				if (!GUILDMATCHUP(index)
						->holeStroke[Doc()->m_holeOrder[i] - 1] ||
					(GUILDMATCHUP(index)->oid != MyGuid(false) &&
						i > GetHoleIndex(GOLFDOC()->m_currentHole) - 1))
				{
					DrawDigit(gdi, "?", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
						false);
				}
				else
				{
					int par = GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par;
					int stroke = GUILDMATCHUP(index)
									 ->holeStroke[Doc()->m_holeOrder[i] - 1];
					int score = stroke - par < -3
						? -3
						: (stroke - par > 3 ? 3 : stroke - par);

					unsigned long color;
					switch (score)
					{
					case -4:
					case -3:
					case -2:
						color = 0xffff4f4f;
						break;
					case -1:
						color = 0xffffbd42;
						break;
					case 0:
						color = 0xffffffff;
						break;
					case 1:
						color = 0xffb9b9b9;
						break;
					default:
						color = 0xff6f6f71;
						break;
					}
					DrawDigit(gdi, MakeStr("%d", stroke), pos, color,
						FrGDI::ALIGN_CENTER, false);
				}
			}
		}
	}
	else
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
				PLAYER(index)->state != 1 &&
				(!(GOLFDOC()->GameMode() & 0x22000) ||
					GOLFDOC()->IsNetGameEnd()))
			{
				DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			}
			else
			{
				if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1])
				{
					DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
						false);
				}
				else
				{
					int par = GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par;
					int stroke =
						PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1];
					int score = stroke - par < -3
						? -3
						: (stroke - par > 3 ? 3 : stroke - par);

					unsigned long color;
					switch (score)
					{
					case -4:
					case -3:
					case -2:
						color = 0xffff4f4f;
						break;
					case -1:
						color = 0xffffbd42;
						break;
					case 0:
						color = 0xffffffff;
						break;
					case 1:
						color = 0xffb9b9b9;
						break;
					default:
						color = 0xff6f6f71;
						break;
					}
					DrawDigit(gdi, MakeStr("%d", stroke), pos, color,
						FrGDI::ALIGN_CENTER, false);
				}
			}
		}
	}
}

void FrScoreForm::DrawVersus(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	int i = m_bFront9 ? 0 : 9;
	int end = min(i + min(1 - (int)(m_aniTime * -20.0f), 9), GetHoles());

	WPoint pos;
	WRect dest;
	pos.x = (float)(int)(width / 18.0f + item->pos.x);
	pos.y = (float)(int)(0.5f * height + item->pos.y - 6.0f);

	for (; i < end; ++i, pos.x = (float)(int)(width / 9.0f + pos.x))
	{
		if (Doc()->m_golfGame.gameType == 1)
		{
			if (!TEAM(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		}
		else if (Doc()->m_golfGame.gameType == 6)
		{
			if (!GUILDMATCHUP(index)->holeStroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		}
		else
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		}

		if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
			(GOLFDOC()->GameMode() & ~0x22000))
		{
			DrawDigit(gdi, "?", pos, 0xffffffff, FrGDI::ALIGN_CENTER, false);
			continue;
		}

		CSharedDoc* doc = Doc();
		const Bitmap* bitmap;
		unsigned char result =
			doc->m_pGolfDoc->GetHoleData(doc->m_holeOrder[i]).result;
		switch (result)
		{
		case 0:
		case 1:
			bitmap = m_pResult[index == result ? 0 : 1];
			break;
		case 2:
			bitmap = m_pResult[2];
			break;
		case 3:
			bitmap = m_pQuestion;
			break;
		default:
			continue;
		}

		if (bitmap)
		{
			dest.x = (float)(int)(pos.x - bitmap->Width() * 0.5f);
			dest.y = pos.y;
			dest.w = (float)bitmap->Width();
			dest.h = (float)bitmap->Height();
			gdi->DrawTexture(bitmap, dest, 0xffffffff, 0);
		}
	}
}

void FrScoreForm::DrawGraph(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	float middle = 0.5f * height + item->pos.y;
	float scale = (height - 5.0f) / 6.0f;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + min(1 - (int)(m_aniTime * -20.0f), 9),
		Doc()->m_golfGame.holes);

	_WPOINT points[2];
	_WPOINT& p1 = points[0];
	_WPOINT& p2 = points[1];
	p1.x = (float)(int)item->pos.x;
	p2.x = (float)(int)((end - start) * width / 9.0f + p1.x);
	p1.y = p2.y = middle;
	g_view->DrawLine2D(p1, p2, 0xff95ffb3, 0xff95ffb3, 0);

	int score;
	if ((unsigned char)PLAYER(index)->stroke[Doc()->m_holeOrder[start] - 1] >
			0 &&
		(PLAYER(index)->state ||
			start != GetHoleIndex(GOLFDOC()->m_currentHole) - 1 ||
			GOLFDOC()->GameMode() == 0x2000))
	{
		int par = GOLFDOC()->GetHoleData(Doc()->m_holeOrder[start]).par;
		unsigned char stroke =
			PLAYER(index)->stroke[Doc()->m_holeOrder[start] - 1];
		score = stroke - par < -3 ? -3 : (stroke - par > 3 ? 3 : stroke - par);
		unsigned long color = GetScoreColor(score);
		p1.x = (float)(int)(width / 18.0f + item->pos.x);
		p1.y = (float)(int)(score * scale + middle);
		for (int i = start + 1; i < end; ++i)
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] ||
				(!PLAYER(index)->state &&
					i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
					GOLFDOC()->GameMode() != 0x2000))
				break;

			int par = GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par;
			unsigned char stroke =
				PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1];
			score =
				stroke - par < -3 ? -3 : (stroke - par > 3 ? 3 : stroke - par);
			unsigned long nextColor = GetScoreColor(score);
			p2.x = (float)(int)(width / 9.0f + p1.x);
			p2.y = (float)(int)(score * scale + middle);
			g_view->DrawLine2D(p1, p2, color, nextColor, 0);
			p1 = p2;
			color = nextColor;
		}
	}

	if (m_pPoint)
	{
		WRect rect(0, 0, 4, 4);
		rect.x = (float)(int)(width / 18.0f + item->pos.x - 2.0f);
		for (int i = start; i < end;
			++i, rect.x = (float)(int)(width / 9.0f + rect.x))
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] ||
				(!PLAYER(index)->state &&
					i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
					GOLFDOC()->GameMode() != 0x2000))
				break;

			int par = GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).par;
			unsigned char stroke =
				PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1];
			score =
				stroke - par < -3 ? -3 : (stroke - par > 3 ? 3 : stroke - par);
			rect.y = (float)(int)(score * scale + middle - 2.0f);
			gdi->DrawTexture(m_pPoint, rect, 0xff696969, 0);
		}
	}
}

void FrScoreForm::DrawPutt(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + min(1 - (int)(m_aniTime * -20.0f), 9), GetHoles());

	WPoint pos;
	pos.x = (float)(int)(width / 18.0f + item->pos.x);
	pos.y = (float)(int)(0.5f * height + item->pos.y - 6.0f);

	for (int i = start; i < end;
		++i, pos.x = (float)(int)(width / 9.0f + pos.x))
	{
		if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
			i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
			break;

		int putt = PLAYER(index)->putt[Doc()->m_holeOrder[i] - 1];

		if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
			PLAYER(index)->state != 1 &&
			(!(GOLFDOC()->GameMode() & 0x22000) || GOLFDOC()->IsNetGameEnd()))
		{
			DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
		else
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1])
				DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			else
				DrawDigit(gdi, MakeStr("%d", putt), pos, 0xff40c967,
					FrGDI::ALIGN_CENTER, false);
		}
	}
}

void FrScoreForm::DrawPang(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + min(1 - (int)(m_aniTime * -20.0f), 9), GetHoles());

	WPoint pos;
	pos.x = (float)(int)(width / 18.0f + item->pos.x);
	pos.y = (float)(int)(0.5f * height + item->pos.y - 6.0f);

	if (Doc()->m_golfGame.gameType == 1)
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!TEAM(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
				index == PLAYER(GOLFDOC()->m_currentPlayer)->team)
				pang = m_holePang;
			else
				pang = TEAM(index)->pang[Doc()->m_holeOrder[i] - 1];

			if (!TEAM(index)->stroke[Doc()->m_holeOrder[i] - 1])
				DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			else
				DrawDigit(gdi, MakeStr("%I64d", pang), pos, 0xffe98919,
					FrGDI::ALIGN_CENTER, true);
		}
	}
	else if (Doc()->m_golfGame.gameType == 6)
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!GUILDMATCHUP(index)->holeStroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
				(GOLFDOC()->GameMode() & ~0x22000))
				pang = m_holePang;
			else
				pang = GUILDMATCHUP(index)->holePang[Doc()->m_holeOrder[i] - 1];

			if (!GUILDMATCHUP(index)->holeStroke[Doc()->m_holeOrder[i] - 1] ||
				(GUILDMATCHUP(index)->oid != MyGuid(false) &&
					i > GetHoleIndex(GOLFDOC()->m_currentHole) - 1))
				DrawDigit(gdi, "?", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			else
				DrawDigit(gdi, MakeStr("%d", pang), pos, 0xffe98919,
					FrGDI::ALIGN_CENTER, true);
		}
	}
	else
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 &&
				index == GOLFDOC()->m_currentPlayer)
				pang = m_holePang;
			else
				pang = PLAYER(index)->pang[Doc()->m_holeOrder[i] - 1];

			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1])
				DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER,
					false);
			else
				DrawDigit(gdi, MakeStr("%d", pang), pos, 0xffe98919,
					FrGDI::ALIGN_CENTER, true);
		}
	}
}

void FrScoreForm::DrawSkins(FrListItem* item)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int index = (int)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + min(1 - (int)(m_aniTime * -20.0f), 9), GetHoles());

	WPoint pos;
	pos.x = (float)(int)(width / 18.0f + item->pos.x);
	pos.y = (float)(int)(0.5f * height + item->pos.y - 6.0f);

	if (Doc()->m_golfGame.gameType == 7)
	{
		for (int i = start; i < end;
			++i, pos.x = (float)(int)(width / 9.0f + pos.x))
		{
			if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1] &&
				i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			int pang;
			unsigned long color;
			bool dash = false;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
			{
				if (PLAYER(index)->state == 3)
				{
					if (!PLAYER(index)->stroke[Doc()->m_holeOrder[i] - 1])
						dash = true;
				}
				else if (!GOLFDOC()->m_bNoStat && m_state != STATE_3)
					dash = true;
			}
			if (!dash)
			{
				pang = PLAYER(index)->skinsPang[Doc()->m_holeOrder[i] - 1];
				if (PLAYER(index)->state == 3)
					color = 0xff282828;
				else if (GOLFDOC()->GetHoleData(Doc()->m_holeOrder[i]).carry &&
					pang > 0)
					color = 0xffff0000;
				else
					color = 0xff000000;

				gdi->SetTextStyle(1);
				gdi->SetTextColor(color, 0xffffffff);
				if (pang == -1)
				{
					WRect rect;
					rect.x = pos.x - ((float)m_pArrow->Width()) * 0.5f;
					rect.y = pos.y;
					rect.w = (float)m_pArrow->Width();
					rect.h = (float)m_pArrow->Height();
					gdi->DrawTexture(m_pArrow, rect, 0xffffffff, 0);
					continue;
				}
				gdi->PrintText(pos, 1, MakeStr("%d", pang), NULL);
				continue;
			}
			DrawDigit(gdi, "-", pos, 0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
	}
}

void FrScoreForm::DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
	unsigned long color, FrGDI::eTextAlign align, bool narrow)
{
	int len = strlen(text);
	int width = 0;
	const Bitmap* digits[16];
	if (align)
	{
		width = 0;
		for (int i = 0; i < len; ++i)
		{
			switch (text[i])
			{
			case '+':
				digits[i] = m_pPlus;
				break;
			case '-':
				digits[i] = m_pMinus;
				break;
			case '?':
				digits[i] = m_pQuestion;
				break;
			default:
				digits[i] = m_pDigit[text[i] - '0'];
				break;
			}
			width += digits[i]->Width() - (narrow ? (len < 4 ? 0 : 2) : 0);
		}
	}
	WRect rect;
	switch (align)
	{
	case FrGDI::ALIGN_LEFT:
		rect.x = (float)(int)pos.x;
		break;
	case FrGDI::ALIGN_CENTER:
		rect.x = (float)(int)(pos.x - width * 0.5f);
		break;
	case FrGDI::ALIGN_RIGHT:
		rect.x = (float)(int)(pos.x - width);
		break;
	}
	rect.y = pos.y;
	for (int i = 0; i < len; ++i)
	{
		rect.w = (float)digits[i]->Width();
		rect.h = (float)digits[i]->Height();
		gdi->DrawTexture(digits[i], rect, color, 0);
		rect.x += rect.w - (narrow ? (len < 4 ? 0 : 2) : 0);
	}
}

unsigned long FrScoreForm::GetScoreColor(int score)
{
	switch (score)
	{
	case -4:
	case -3:
	case -2:
		return 0xffff0000;
	case -1:
		return 0xffffbb4f;
	case 0:
		return 0xffffffff;
	case 1:
		return 0xff82c3ff;
	default:
		return 0xff0036ff;
	}
}

void FrScoreForm::OnTotalInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (!area)
		return;

	if (Doc()->m_golfGame.gameType == 1 || Doc()->m_golfGame.gameType == 3 ||
		Doc()->m_golfGame.gameType == 6)
		area->SetBgImg("header04");
	else if (Doc()->m_golfGame.gameType == 7)
	{
		area->SetBgImg("header03_skins");
		area->SetRect(area->GetRect() + WRect(-52.0f, 0.0f, 0.0f, 0.0f));
	}
	else
		area->SetBgImg("header03");
}

void FrScoreForm::OnPPLInit(int param)
{
	m_pPPL = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (!m_pPPL)
		return;

	if (Doc()->m_golfGame.gameType == 1 || Doc()->m_golfGame.gameType == 6 ||
		Doc()->m_golfGame.gameType == 3)
	{
		m_pPPL->SetRect(WRect(m_pPPL->GetRect().x - 65.0f, m_pPPL->GetRect().y,
			m_pPPL->GetRect().w, m_pPPL->GetRect().h));
	}
	else if (Doc()->m_golfGame.gameType == 7)
	{
		m_pPPL->SetRect(m_pPPL->GetRect() + WRect(43.0f, 0.0f, 0.0f, 0.0f));
	}

	m_pPPL->SetVisible(false);
	m_bShowPPL = false;
}

void FrScoreForm::SetPPL(const char* name)
{
	if (m_pPPL)
	{
		const Bitmap* bitmap = m_pPPL->GetBgImg();
		if (m_pPPL->SetBgImg(name))
		{
			m_bShowPPL = true;
			m_pPPL->SetVisible(true);
		}
		else
		{
			m_bShowPPL = false;
			m_pPPL->SetBgImg(bitmap);
			m_pPPL->SetVisible(false);
		}
	}
}

void FrScoreForm::OnBoard1Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area && Doc()->m_golfGame.gameType == 7)
	{
		area->SetBgImg("header01_skins");
		area->SetRect(WRect(area->GetRect().x - 1.0f, area->GetRect().y,
			area->GetRect().w, area->GetRect().h));
	}
	else
		area->SetBgImg("header01");
}

void FrScoreForm::OnBoard2Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area && Doc()->m_golfGame.gameType == 7)
	{
		area->SetBgImg("header02_skins");
		area->SetRect(WRect(area->GetRect().x - 32.0f, area->GetRect().y,
			area->GetRect().w, area->GetRect().h));
	}
	else
		area->SetBgImg("header02");
}

void FrScoreForm::OnDrawLinesInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (Doc()->m_golfGame.gameType != 7)
		area->SetVisible(false);
}

void FrScoreForm::OnDrawLinesOwnerDraw(int param)
{
	WPoint p1;
	WPoint p2;

	p1.x = p2.x = GetRect().x + 129.0f;
	p1.y = GetRect().y + 126.0f;
	p2.y = m_playerNum * 58.0f + p1.y;

	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.x = p2.x - 22.0f;
		p2.x = p1.x;
		p1.y += 22.0f;
		p2.y += 22.0f;
	}

	for (int i = 0; i < 10; ++i)
	{
		g_view->DrawLine2D(p1, p2, 0xff9c9c9c, 0xff9c9c9c, 0);
		p1.x += 38.0f;
		p2.x += 38.0f;
		if (Doc()->m_golfGame.gameType == 7)
		{
			p1.x += 14.0f;
			p2.x += 14.0f;
		}
	}

	p1.x += 27.0f;
	p2.x += 27.0f;
	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.x -= 13.0f;
		p2.x -= 13.0f;
	}

	p1.y = p2.y = GetRect().y + 184.0f;
	if (Doc()->m_golfGame.gameType == 7)
	{
		p1.y = p2.y + 22.0f;
		p2.y = p1.y;
	}

	for (int i = 0; i < m_playerNum - 1; ++i)
	{
		p1.x = GetRect().x + 3.0f;
		p2.x = GetRect().x + 28.0f;
		g_view->DrawLine2D(p1, p2, 0xff9c9c9c, 0xff9c9c9c, 0);

		p1.x = GetRect().x + GetRect().w - 36.0f;
		p2.x = GetRect().x + GetRect().w - 11.0f;
		g_view->DrawLine2D(p1, p2, 0xff9c9c9c, 0xff9c9c9c, 0);

		p1.x = GetRect().x + 28.0f;
		p2.x = GetRect().x + GetRect().w - 36.0f;
		g_view->DrawLine2D(p1, p2, 0xff9c9c9c, 0xff9c9c9c, 0);

		p1.y += 58.0f;
		p2.y += 58.0f;
	}
}

void FrScoreForm::OnCancelBtnInit(int param)
{
	m_pCancelBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCancelBtn->SetVisible(false);
}

void FrScoreForm::OnCancelBtnUp()
{
	Close(FrOK, true);
}

void FrScoreForm::OnCloseBtnInit(int param)
{
	if (!m_pCloseBtn)
		m_pCloseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pCloseBtn)
		m_pCloseBtn->SetVisible(false);
}

void FrScoreForm::OnCloseBtnUp()
{
	SetVisible(false);
}

void FrScoreForm::SetDesc()
{
	char buf[256];
	unsigned long oid = 0xffffffff;

	switch (GetGameType())
	{
	case GAME_TYPE_STROKE:
		for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			if (PLAYER(i)->rank == 1)
			{
				if (oid == 0xffffffff)
				{
					oid = PLAYER(i)->oid;
					__int64 pang = 0;
					unsigned char hole = GetHoleIndex(GOLFDOC()->m_currentHole);
					for (unsigned char j = 0; j < hole; j++)
					{
						if (j == hole - 1 && i == GOLFDOC()->m_currentPlayer)
							pang += m_holePang;
						else
							pang += PLAYER(i)->pang[Doc()->m_holeOrder[j] - 1];
					}
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 %s\xb4\xd4\xc0\xcc \xbd\xba\xc4\xda\xbe\xee %d, \xc6\xce %I64d",
						Doc()->m_userInfo[i].info.sNick, PLAYER(i)->score,
						pang);
					ConditionalStrCat(buf,
						"\xb7\xce \xbc\xb1\xb5\xce\xc0\xd4\xb4\xcf\xb4\xd9.");
				}
				else
				{
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xc7 "
						"\xbd\xba\xc4\xda\xbe\xee\xb8\xa6 \xba\xb8\xbf\xa9\xc1\xd6\xb4\xc2 "
						"\xc4\xab\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9.");
					break;
				}
			}
		}
		if (oid == 0xffffffff)
			sprintf(buf,
				"\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xc7 "
				"\xbd\xba\xc4\xda\xbe\xee\xb8\xa6 \xba\xb8\xbf\xa9\xc1\xd6\xb4\xc2 "
				"\xc4\xab\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9.");
		break;

	case GAME_TYPE_MATCH:
		if (PLAYER(0)->score > PLAYER(1)->score)
		{
			sprintf(buf,
				"\xc7\xf6\xc0\xe7 %s\xb4\xd4\xc0\xcc \xbd\xba\xc4\xda\xbe\xee %d vs %d",
				Doc()->m_userInfo[0].info.sNick, PLAYER(0)->score,
				PLAYER(1)->score);
			ConditionalStrCat(buf,
				"\xb7\xce \xbc\xb1\xb5\xce\xc0\xd4\xb4\xcf\xb4\xd9.");
		}
		else if (PLAYER(0)->score < PLAYER(1)->score)
		{
			sprintf(buf,
				"\xc7\xf6\xc0\xe7 %s\xb4\xd4\xc0\xcc \xbd\xba\xc4\xda\xbe\xee %d vs %d",
				Doc()->m_userInfo[1].info.sNick, PLAYER(1)->score,
				PLAYER(0)->score);
			ConditionalStrCat(buf,
				"\xb7\xce \xbc\xb1\xb5\xce\xc0\xd4\xb4\xcf\xb4\xd9.");
		}
		else
		{
			__int64 pang[2] = { 0, 0 };
			unsigned char hole = GetHoleIndex(GOLFDOC()->m_currentHole);
			for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
			{
				for (unsigned char j = 0; j < hole; j++)
				{
					if (j == hole - 1 && i == GOLFDOC()->m_currentPlayer)
						pang[i] += m_holePang;
					else
						pang[i] += PLAYER(i)->pang[Doc()->m_holeOrder[j] - 1];
				}
			}

			if (pang[0] > pang[1])
			{
				sprintf(buf,
					"\xc7\xf6\xc0\xe7 \xbd\xba\xc4\xda\xbe\xee\xb4\xc2 \xb5\xbf\xc1\xa1\xc0\xcc\xc1\xf6\xb8\xb8, "
					"%s\xb4\xd4\xc0\xcc \xc6\xce %d",
					Doc()->m_userInfo[0].info.sNick, pang[0]);
				ConditionalStrCat(buf,
					"\xb7\xce \xc0\xcc\xb1\xe2\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			}
			else if (pang[0] < pang[1])
			{
				sprintf(buf,
					"\xc7\xf6\xc0\xe7 \xbd\xba\xc4\xda\xbe\xee\xb4\xc2 \xb5\xbf\xc1\xa1\xc0\xcc\xc1\xf6\xb8\xb8, "
					"%s\xb4\xd4\xc0\xcc \xc6\xce %d",
					Doc()->m_userInfo[1].info.sNick, pang[1]);
				ConditionalStrCat(buf,
					"\xb7\xce \xc0\xcc\xb1\xe2\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			}
			else
			{
				sprintf(buf,
					"\xb5\xbf\xc1\xa1\xc0\xcf \xb0\xe6\xbf\xec\xbf\xa1\xb4\xc2 \xc6\xce\xc0\xb8\xb7\xce "
					"\xbc\xf8\xc0\xa7\xb8\xa6 \xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.");
			}
		}
		break;

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		for (unsigned char i = 0; i < Doc()->m_rivalList.size(); i++)
		{
			if (Doc()->m_rivalList[i].rank == 1)
			{
				if (oid != 0xffffffff)
				{
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xc7 "
						"\xbd\xba\xc4\xda\xbe\xee\xb8\xa6 \xba\xb8\xbf\xa9\xc1\xd6\xb4\xc2 "
						"\xc4\xab\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9.");
					break;
				}
				oid = Doc()->m_rivalList[i].oid;
				if (GetGameType() == GAME_TYPE_NEW_APPROACH)
				{
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 %s\xb4\xd4\xc0\xcc \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae %d.%d y, "
						"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3 %d.%03d",
						Doc()->m_rivalList[Doc()->GetIndex(oid)].nickname,
						Doc()->m_rivalList[Doc()->GetIndex(oid)]
								.approachResultDistance /
							10,
						Doc()->m_rivalList[Doc()->GetIndex(oid)]
								.approachResultDistance %
							10,
						Doc()->m_rivalList[Doc()->GetIndex(oid)].approachTime /
							1000,
						Doc()->m_rivalList[Doc()->GetIndex(oid)].approachTime %
							1000);
				}
				else
				{
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 %s\xb4\xd4\xc0\xcc \xbd\xba\xc4\xda\xbe\xee %d, \xc6\xce %I64d",
						Doc()->m_rivalList[Doc()->GetIndex(oid)].nickname,
						Doc()->m_rivalList[Doc()->GetIndex(oid)].totalScore,
						Doc()->m_rivalList[Doc()->GetIndex(oid)].totalPang);
				}
				ConditionalStrCat(buf,
					"\xb7\xce \xbc\xb1\xb5\xce\xc0\xd4\xb4\xcf\xb4\xd9.");
			}
		}
		break;

	case GAME_TYPE_TEAM:
		if (TEAM(0)->score == TEAM(1)->score)
		{
			__int64 pang[2] = { 0, 0 };
			unsigned char hole = GetHoleIndex(GOLFDOC()->m_currentHole);
			for (unsigned char i = 0; i < 2; i++)
			{
				for (unsigned char j = 0; j < hole; j++)
				{
					if (j == hole - 1 &&
						i == PLAYER(GOLFDOC()->m_currentPlayer)->team)
						pang[i] += m_holePang;
					else
						pang[i] += TEAM(i)->pang[Doc()->m_holeOrder[j] - 1];
				}
			}

			if (pang[0] == pang[1])
			{
				sprintf(buf,
					"\xb5\xbf\xc1\xa1\xc0\xcf \xb0\xe6\xbf\xec\xbf\xa1\xb4\xc2 \xc6\xce\xc0\xb8\xb7\xce "
					"\xbc\xf8\xc0\xa7\xb8\xa6 \xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.");
			}
			else
			{
				if (pang[0] > pang[1])
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 \xbd\xba\xc4\xda\xbe\xee\xb4\xc2 \xb5\xbf\xc1\xa1\xc0\xcc\xc1\xf6\xb8\xb8, "
						"RED \xc6\xc0\xc0\xcc \xc6\xce %d",
						pang[0]);
				else
					sprintf(buf,
						"\xc7\xf6\xc0\xe7 \xbd\xba\xc4\xda\xbe\xee\xb4\xc2 \xb5\xbf\xc1\xa1\xc0\xcc\xc1\xf6\xb8\xb8, "
						"BLUE \xc6\xc0\xc0\xcc \xc6\xce %d",
						pang[1]);

				ConditionalStrCat(buf,
					"\xb7\xce \xc0\xcc\xb1\xe2\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			}
		}
		else
		{
			if (TEAM(0)->score > TEAM(1)->score)
				sprintf(buf, "\xc7\xf6\xc0\xe7 RED \xc6\xc0\xc0\xcc %d vs %d",
					TEAM(0)->score, TEAM(1)->score);
			else
				sprintf(buf, "\xc7\xf6\xc0\xe7 BLUE \xc6\xc0\xc0\xcc %d vs %d",
					TEAM(1)->score, TEAM(0)->score);

			ConditionalStrCat(buf,
				"\xb7\xce \xc0\xcc\xb1\xe2\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		}
		break;

	default:
		sprintf(buf,
			"\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xc7 "
			"\xbd\xba\xc4\xda\xbe\xee\xb8\xa6 \xba\xb8\xbf\xa9\xc1\xd6\xb4\xc2 "
			"\xc4\xab\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9.");
		break;
	}

	FrForm::SetDesc(buf);
}

void FrScoreForm::ConditionalStrCat(char* dest, const char* src)
{
	int len = strlen(dest);

	if (dest[len - 1] == '0' || dest[len - 1] == '3' || dest[len - 1] == '6')
		strcat(dest, K2L_Compatibility("\xc0\xb8"));

	strcat(dest, src);
}

IMPLEMENT_OBJECT(FrMatchItems, FrForm)

BEGIN_FRESH_MSGMAP(FrMatchItems, FrForm)
ON_FRESH_VI("view", FRCMD_INIT, FrMatchItems::OnViewInit)
ON_FRESH_VI("view", FRCMD_OWNERDRAW, FrMatchItems::OnViewOwnerDraw)
END_FRESH_MSGMAP()

FrMatchItems::FrMatchItems()
{
	m_pEmoti = NULL;
}

FrMatchItems::~FrMatchItems()
{
	if (m_pEmoti)
	{
		delete m_pEmoti;
		m_pEmoti = NULL;
	}
}

bool FrMatchItems::OnInit()
{
	m_pos.x = (g_view->GetWidth() - 440.0f) * 0.5f;
	m_pos.y = g_view->GetHeight() - 100.0f;

	m_pEmoti = new CSimpleUI("[emoti_object.png");
	m_pEmoti->Add("0", WRect(0.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("1", WRect(64.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("2", WRect(128.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("3", WRect(192.f, 128.f, 64.f, 64.f), false);
	m_pEmoti->Add("4", WRect(0.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("5", WRect(64.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("6", WRect(128.f, 192.f, 64.f, 64.f), false);
	m_pEmoti->Add("7", WRect(192.f, 192.f, 64.f, 64.f), false);

	m_frame = 0;
	m_bAnimate = true;
	m_bPlaySound = true;
	m_reserved150 = 0;
	m_aniTime = -3.0f;
	return true;
}

void FrMatchItems::OnViewInit(int param)
{
	m_pView = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrMatchItems::OnViewOwnerDraw(int param)
{
	unsigned long* items = Doc()->m_userInfo[MyIndex()].userEquip.tidItemSlot;

	WRect rect;
	rect.x = m_pos.x + 8.0f;
	rect.y = m_pos.y + 28.0f;
	rect.h = 54.0f;
	rect.w = 54.0f;

	int last = 7;
	WOverlay* pFailOverlay = NULL;
	IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(0x1800000d);
	if (pItem)
		pFailOverlay = g_resrcmng->GetOverlay(
			MakeStr("%s.tga?items.bres", pItem->c.Icon), 0);

	int count = 10;
	if (IsLocalContent(S3_MASCOT))
		count = 8;

	for (int i = 0; i < count; i++)
	{
		if (items[i])
		{
			std::map<unsigned long, WOverlay*>::iterator it =
				m_overlayMap.find(items[i]);
			if (it == m_overlayMap.end())
			{
				IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(items[i]);
				if (pItem)
				{
					WOverlay* pOverlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga?items.bres", pItem->c.Icon), 0);
					if (pOverlay)
						m_overlayMap.insert(
							std::map<unsigned long, WOverlay*>::value_type(
								items[i], pOverlay));
				}
			}

			if (items[i] == 0x1800000d)
			{
				last = i - 1;
				break;
			}
		}
	}

	for (int i = 0; i < count; i++)
	{
		if (items[i])
		{
			std::map<unsigned long, WOverlay*>::iterator it =
				m_overlayMap.find(items[i]);
			if (it != m_overlayMap.end())
			{
				WOverlay* pOverlay = (*it).second;
				if (i == last && m_aniTime < 0.0f && m_bAnimate)
				{
					if (pFailOverlay)
						pFailOverlay->Render(g_view, WRect(0.f, 0.f, 1.f, 1.f),
							rect, 0, 0xffffffff, 0, 0);
				}
				else
					pOverlay->Render(g_view, WRect(0.f, 0.f, 1.f, 1.f), rect, 0,
						0xffffffff, 0, 0);
			}
			rect.x += 45.0f;
		}
	}

	if (m_bAnimate)
	{
		m_aniTime += 0.1f;
		m_frame = (int)(m_aniTime * 5.0f);

		if (m_frame == 8)
		{
			m_aniTime = -3.0f;
			m_bAnimate = false;
		}
		else if (m_frame >= 0)
		{
			if (m_bPlaySound)
			{
				g_audio->PlaySfx("play_fail_item");
				m_bPlaySound = false;
			}

			if (m_pEmoti)
				m_pEmoti->Render(MakeStr("%d", m_frame), m_pos.x + last * 45.0f,
					m_pos.y + 8.0f, 1.0f, 0);
		}
	}
}

IMPLEMENT_ACTOR(CScore, IActor)

BEGIN_FRESH_MSGMAP(CScore, FrCmdTarget)
ON_FRESH_VV(".score.close", FRCMD_LBUTTONUP, CScore::OnCloseBtnUp)
END_FRESH_MSGMAP()

CScore::CScore()
	: m_resultState(0),
	  m_resultTime(0.0f),
	  m_panelState(0),
	  m_bShowRank3(1),
	  m_bRank3Enabled(1),
	  m_reserved30(0),
	  m_pScoreForm(NULL),
	  m_pRankForm(NULL),
	  m_pRankForm2(NULL),
	  m_pRankForm3(NULL),
	  m_pRankFormEx(NULL),
	  m_pAwardForm(NULL),
	  m_pMatchItems(NULL),
	  m_pGuildScoreDlg(NULL),
	  m_pTreasureGauge(NULL),
	  m_pNextHole(NULL),
	  m_pTreasureScore(NULL),
	  m_pTreasureGift(NULL),
	  m_fadeAlpha(0.0f),
	  m_pAquireItem(NULL)
{
	m_dispPriority = DISP_PRIORITY_5;

	m_noticeImages.reserve(10);

	LoadNoticeImageFile(MakeStr("%s/score_ppl.ini", SCORE_PPL_DIR),
		m_noticeImages, 10, NULL);

	ImageSortByCharacterIndex(m_noticeImages);
}

void CScore::ImageSortByCharacterIndex(std::vector<sNoticeImage>& images)
{
	if (images.size() < 10)
		return;

	const char* prefix = "score_notice_";
	std::vector<sNoticeImage> sorted;
	sorted.reserve(images.size());

	for (int i = 0; i < 10; i++)
		sorted.push_back(sNoticeImage());

	for (unsigned int j = 0; j < images.size(); j++)
	{
		int index = 0;
		sscanf(images[j].name + strlen(prefix), "%d", &index);

		if (index < 10)
			strncpy(sorted[index].name, images[j].name, 32);
	}

	images.assign(sorted.begin(), sorted.end());
}

void CScore::OnDestroy()
{
	if (m_pRankFormEx)
	{
		m_pRankFormEx->Close(FrOK, true);
		m_pRankFormEx = NULL;
	}

	if (m_pGuildScoreDlg)
	{
		m_pGuildScoreDlg->Close(true);
		m_pGuildScoreDlg = NULL;
	}

	if (m_pAquireItem)
	{
		delete m_pAquireItem;
		m_pAquireItem = NULL;
	}
}

void CScore::OnLoad()
{
	WPoint rankExPos;

	if (IsMassGame() && GetGameType() != GAME_TYPE_GUILD_MATCH)
	{
		if (IsObserver(0xffffffff))
		{
			m_pRankForm2 =
				CreateForm<FrRankForm2>(g_pFresh->GetManager(), this, "rank2");
			m_pRankForm2->SetFixed(true);
			m_pRankForm2->EnableDrag(false);
			m_pRankForm2->Open(NULL, 12);
			m_pRankForm2->SetVisible(false);
		}
		else
		{
			m_pRankForm =
				CreateForm<FrRankForm>(g_pFresh->GetManager(), this, "rank");

			m_rankPos.x = g_view->GetWidth() - m_pRankForm->GetRect().w;
			m_rankPos.y = 40.0f;
			m_pRankForm->SetFixed(true);
			m_pRankForm->EnableDrag(false);
			m_pRankForm->Open(NULL, m_rankPos, 12);
			m_pRankForm->SetVisible(false);
			m_pRankForm->Resize();
		}

		if (GetGameType() != GAME_TYPE_NEW_APPROACH)
		{
			m_pRankFormEx = CreateForm<FrRankFormEx>(g_pFresh->GetManager(),
				this, "rankex");
			m_pRankFormEx->EnableDrag(false);

			rankExPos.x =
				(float)(int)((g_view->GetWidth() - m_pRankFormEx->GetRect().w) *
					0.5f);
			rankExPos.y = (float)(int)((g_view->GetHeight() -
										   m_pRankFormEx->GetRect().h) *
				0.5f);
			m_pRankFormEx->SetFixed(true);
			m_pRankFormEx->Open((FRESH_PFN_RESULT)&CScore::OnRankFormExResult,
				rankExPos, 4);
			m_pRankFormEx->SetVisible(false);
			m_pRankFormEx->ResetStage();
		}
	}

	if (GetGameType() == GAME_TYPE_GUILD_MATCH)
	{
		m_pRankForm3 =
			CreateForm<FrRankForm3>(g_pFresh->GetManager(), this, "rank3");

		m_rankPos.x = g_view->GetWidth() - m_pRankForm3->GetRect().w + 5.0f;
		m_rankPos.y = 40.0f;
		m_pRankForm3->SetFixed(true);
		m_pRankForm3->EnableDrag(false);
		m_pRankForm3->Open(NULL, m_rankPos, 12);
		m_pRankForm3->SetVisible(false);

		m_pGuildScoreDlg =
			CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score_guild");

		WPoint pos;
		pos.x =
			(g_view->GetWidth() - m_pGuildScoreDlg->GetRect().w + 1.0f) * 0.5f;
		pos.y = m_pGuildScoreDlg->GetRect().y;
		m_pGuildScoreDlg->EnableDrag(false);
		m_pGuildScoreDlg->SetFixed(true);
		m_pGuildScoreDlg->Open(
			(FRESH_PFN_RESULT)&CScore::OnGuildScoreFormResult, pos, 12);
		m_pGuildScoreDlg->SetVisible(false);

		m_pGuildScoreDlg->SetVisibleCloseBtn(false);
	}
	else
	{
		if (GetGameType() == GAME_TYPE_SKINS)
		{
			m_pScoreForm = CreateForm<FrScoreForm>(g_pFresh->GetManager(), this,
				"score_skins");
			m_pScoreForm->EnableDrag(false);
			m_pScoreForm->SetFixed(true);
			m_pScoreForm->Open(NULL, 12);
			m_pScoreForm->Resize();
			m_pScoreForm->SetState(FrScoreForm::STATE_1);
			m_pScoreForm->SetTypeBtn();
			m_pScoreForm->SetVisible(false);
			m_pScoreForm->SetCaptionOffset(true);
			m_pScoreForm->DescHidePrivacy();
		}
		else
		{
			m_pScoreForm =
				CreateForm<FrScoreForm>(g_pFresh->GetManager(), this, "score");
			m_pScoreForm->EnableDrag(false);
			m_pScoreForm->SetFixed(true);
			m_pScoreForm->Open(NULL, 12);
			m_pScoreForm->Resize();
			m_pScoreForm->SetState(FrScoreForm::STATE_1);
			m_pScoreForm->SetTypeBtn();
			m_pScoreForm->SetVisible(false);
			m_pScoreForm->SetCaptionOffset(true);
			m_pScoreForm->DescHidePrivacy();

			m_pTreasureGauge = CreateForm<FrTreasureGauge>(
				g_pFresh->GetManager(), this, "tPointdlg");
			m_pTreasureGauge->EnableDrag(false);
			m_pTreasureGauge->SetFixed(true);
			m_pTreasureGauge->Open(NULL, 12);
			m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);

			m_pNextHole = CreateForm<FrNextHole>(g_pFresh->GetManager(), this,
				"nexthole");
			m_pNextHole->EnableDrag(false);
			m_pNextHole->SetFixed(true);
			m_pNextHole->Open(NULL, 12);
			m_pNextHole->SetState((FrNextHole::eState)1);
		}

		if (GetGameType() == GAME_TYPE_MATCH)
		{
			m_pMatchItems =
				CreateForm<FrMatchItems>(g_pFresh->GetManager(), this, "items");
			m_pMatchItems->SetFixed(true);
			m_pMatchItems->MoveWindow(
				WPoint((g_view->GetWidth() - 440.0f) * 0.5f,
					g_view->GetHeight() - 100.0f));
			m_pMatchItems->Open(NULL, 12);
			m_pMatchItems->SetVisible(false);
		}
	}
}

void CScore::OnReload()
{
	this << MsgObject(NULL, 305, 0, 0, 0, 0, 0);
	this << MsgObject(NULL, 42, 0, 0, 0, 0, 0);
}

void CScore::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode < 7)
		return;

	if (IsLocalContent((localContentType_t)79) && Doc()->m_playingMode == 2)
		return;

	if (m_pRankForm)
	{
		if (CCapturedBg::Instance()->IsCaptured() ||
			(GOLFDOC()->m_gameMode & 0x1e84))
		{
			if (GetGameType() != GAME_TYPE_NEW_APPROACH ||
				!(GOLFDOC()->m_gameMode & 0x880))
				m_pRankForm->SetVisible(false);
		}
		else
		{
			bool bShow = !g_pFresh->GetDesktop()->IsWallPaperVisible();
			if (GetGameType() == GAME_TYPE_NEW_APPROACH)
				bShow = false;

			m_pRankForm->SetVisible(bShow);

			if (bShow)
			{
				switch (GOLFDOC()->m_gameMode)
				{
				case 0x40:
					if (GetGameType() == GAME_TYPE_NEW_APPROACH)
					{
						m_rankPos.x =
							g_view->GetWidth() - m_pRankForm->GetRect().w;
						m_pRankForm->MoveWindow(m_rankPos);
						break;
					}
					m_rankPos.x =
						Min(m_rankPos.x + delta * 600.0f, g_view->GetWidth());
					m_pRankForm->MoveWindow(m_rankPos);
					break;

				case 0x100:
				case 0x40000:
					m_rankPos.x =
						Min(m_rankPos.x + delta * 600.0f, g_view->GetWidth());
					m_pRankForm->MoveWindow(m_rankPos);
					break;

				case 0x8:
				{
					float screenX;
					float screenY;
					SendMsg(this, "Screen", 454, (int)&screenX, (int)&screenY,
						0, 0);

					float x = g_view->GetWidth() - m_pRankForm->GetRect().w;

					if (x - 60.0f > screenX)
						m_rankPos.x = Max(m_rankPos.x - delta * 600.0f, x);
					else
						m_rankPos.x = Min(m_rankPos.x + delta * 600.0f,
							g_view->GetWidth());

					m_pRankForm->MoveWindow(m_rankPos);
					break;
				}

				case 0x10:
				case 0x20:
					break;

				default:
					m_pRankForm->SetVisible(false);
					break;
				}
			}
		}
	}

	if (m_pRankForm3)
	{
		m_pRankForm3->SetVisible(m_bShowRank3 == 1);

		if (!CQuitWindow::Instance()->IsActive() &&
			!CUserInfo::Instance()->IsVisible() &&
			!CChatMsg::Instance()->IsOpened() &&
			!g_pFresh->GetManager()->IsKeyExclusive() &&
			g_input->GetDown("RANK", true) && GOLFDOC()->m_tutorialMode >= 15)
		{
			if (m_bShowRank3 == 0)
			{
				if (GOLFDOC()->m_gameMode & 0xfffffbfe)
				{
					m_bRank3Enabled = 1;
					m_bShowRank3 = 1;
				}
			}
			else
			{
				m_bRank3Enabled = 0;
				m_bShowRank3 = 0;
			}
		}
	}

	if (m_pScoreForm && !CQuitWindow::Instance()->IsActive() &&
		!CUserInfo::Instance()->IsVisible() &&
		!CChatMsg::Instance()->IsOpened() &&
		!g_pFresh->GetManager()->IsKeyExclusive() &&
		g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) &&
		!g_input->Get("RALT", true) && !g_input->Get("RCONTROL", true) &&
		!g_input->Get("LCONTROL", true) && GOLFDOC()->m_tutorialMode >= 15)
	{
		if (m_panelState == 0)
		{
			if (GOLFDOC()->m_gameMode & 0xfffffbfe)
			{
				m_panelState = 1;
				m_pScoreForm->SetState(FrScoreForm::STATE_2);
				m_pScoreForm->SetDesc();

				if (GOLFDOC()->m_gameMode & 0xfffffbde)
				{
					if (m_pTreasureGauge)
						m_pTreasureGauge->SetState((FrTreasureGauge::eState)2);

					if (m_pNextHole)
						m_pNextHole->SetState((FrNextHole::eState)2);
				}
			}
		}
		else if (m_panelState == 1)
		{
			m_panelState = 0;
			m_pScoreForm->SetState(FrScoreForm::STATE_1);

			if (GOLFDOC()->m_gameMode & 0xfffffbde)
			{
				if (m_pTreasureGauge)
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);

				if (m_pNextHole)
					m_pNextHole->SetState((FrNextHole::eState)1);
			}
		}
	}

	if (m_pGuildScoreDlg && !CQuitWindow::Instance()->IsActive() &&
		!CUserInfo::Instance()->IsVisible() &&
		!CChatMsg::Instance()->IsOpened() &&
		!g_pFresh->GetManager()->IsKeyExclusive() &&
		g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) &&
		!g_input->Get("RALT", true) && !g_input->Get("RCONTROL", true) &&
		!g_input->Get("LCONTROL", true) && GOLFDOC()->m_tutorialMode >= 15)
	{
		if (m_panelState == 0)
		{
			if (GOLFDOC()->m_gameMode & 0xfffffbfe)
			{
				m_panelState = 1;
				m_pGuildScoreDlg->SetVisible(true);
				m_pGuildScoreDlg->SetGuildPlayer(MyGuid(false));
				m_pGuildScoreDlg->SetVisibleCloseBtn(false);
				m_pGuildScoreDlg->SetVisibleCancelBtn(false);
			}
		}
		else if (m_panelState == 1)
		{
			m_panelState = 0;
			m_pGuildScoreDlg->SetVisible(false);
		}
	}

	if (m_panelState == 2)
	{
		if (m_fadeAlpha < 1.0f)
		{
			m_fadeAlpha += delta;
			m_fadeAlpha = Min(1.0f, m_fadeAlpha);
			unsigned char alpha = (unsigned char)(m_fadeAlpha * 255.0f);
			g_pFresh->GetDesktop()->SetBgColor((alpha << 24) | 0xffffff);
		}
	}
	else if (m_panelState == 3)
	{
		if (m_fadeAlpha > 0.0f)
		{
			m_fadeAlpha -= delta;
			unsigned char alpha =
				(unsigned char)float2int(Max(0.0f, m_fadeAlpha) * 255.0f);
			g_pFresh->GetDesktop()->SetBgColor((alpha << 24) | 0xffffff);

			if (m_fadeAlpha < 0.0f)
			{
				m_panelState = 0;
				m_fadeAlpha = 0.0f;
			}
		}
	}

	if (m_resultState == 1 && Doc()->m_gameMode == 0)
	{
		m_resultTime += delta;

		if (m_resultTime > 3.0f)
		{
			if (!m_pAwardForm)
			{
				m_pAwardForm = CreateForm<FrAwardForm>(g_pFresh->GetManager(),
					this, "award");
				m_pAwardForm->SetFixed(true);
				m_pAwardForm->Open(NULL, 12);
				m_pAwardForm->EnableDrag(false);

				if (m_pAquireItem)
					m_pAquireItem->Reset();
				else
					m_pAquireItem = new cAquireItem;
			}
		}

		if (m_pAquireItem)
			m_pAquireItem->OnProc(delta);
	}

	if (Doc()->m_bTreasureAward == true &&
		CTHunter::Instance()->IsReceiveGiftPacket())
	{
		m_resultTime += delta;

		if (m_resultTime > 2.0f)
		{
			if (!m_pTreasureScore)
			{
				m_pTreasureScore = CreateForm<FrTreasureScore>(
					g_pFresh->GetManager(), this, "trscore");
				m_pTreasureScore->Open(NULL, 12);
				m_pTreasureScore->EnableDrag(false);
				m_pTreasureScore->SetFixed(true);
			}

			if (!m_pTreasureGift)
			{
				m_pTreasureGift = CreateForm<FrTreasureGift>(
					g_pFresh->GetManager(), this, "trgift");
				m_pTreasureGift->Open(NULL, 12);
				m_pTreasureGift->EnableDrag(false);
				m_pTreasureGift->SetFixed(true);
			}
		}
	}
}

void CScore::OnDisplay()
{
	if (IsMassGame())
		return;

	if (m_resultState == 1)
	{
		if (m_pAquireItem && m_panelState != 1)
		{
			for (int i = 0; i < GetPlayerNum(); i++)
				m_pAquireItem->DisplayPrizeList(i);
		}
	}
}

void CScore::OnCloseBtnUp()
{
	m_panelState = 0;
}

void CScore::CheckOpenedDlg()
{
	if (GetGameType() == GAME_TYPE_NEW_APPROACH)
	{
		if (m_pRankForm)
		{
			m_pRankForm->Close(true);
			m_pRankForm = NULL;
		}
	}
	else
	{
		m_resultTime = 0.0f;
		m_panelState = 3;
		m_fadeAlpha = 1.0f;

		if (!m_levelupForms.empty())
		{
			for (unsigned int i = 0; i < m_levelupForms.size(); i++)
			{
				if (m_levelupForms[i])
				{
					m_levelupForms[i]->Close(FrNONE, true);
					m_levelupForms[i] = NULL;
				}
			}

			m_levelupForms.clear();
		}

		if (m_pScoreForm)
			m_pScoreForm->SetState(FrScoreForm::STATE_1);

		if (m_pTreasureGauge)
			m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);

		if (m_pNextHole)
			m_pNextHole->SetState((FrNextHole::eState)1);

		if (m_pAwardForm)
		{
			m_pAwardForm->Close(true);
			m_pAwardForm = NULL;
		}

		if (m_pAquireItem)
		{
			delete m_pAquireItem;
			m_pAquireItem = NULL;
		}
	}
}

void CScore::HandleMsg(const MsgObject& msg)
{
	LogOut(0, "CScore::HandleMsg %d\n", msg.message);

	switch (msg.message)
	{
	case 416:
		if (m_pScoreForm)
			m_pScoreForm->UpdatePang();
		break;

	case 301:
	{
		FrLevelupItemForm* pForm = CreateForm<FrLevelupItemForm>(
			g_pFresh->GetManager(), this, "levelupitem");
		if (!pForm)
			break;

		pForm->SetData(*(sLevelUpDone*)msg.param1);
		pForm->SetTopmost(true);
		pForm->Open((FRESH_PFN_RESULT)&CScore::OnLevelupFormResult, 3);
		m_levelupForms.push_back(pForm);
		break;
	}

	case 302:
		if (msg.param1 == 1)
			CheckOpenedDlg();

		m_resultState = msg.param1;
		break;

	case 303:
		if (msg.param1 == 0)
		{
			m_panelState = 0;

			if (IsMassGame())
			{
				if (m_pRankFormEx)
					m_pRankFormEx->SetActive(false);
			}

			if (m_pScoreForm)
				m_pScoreForm->SetState(FrScoreForm::STATE_1);

			if (m_pTreasureGauge)
				m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);

			if (m_pNextHole)
				m_pNextHole->SetState((FrNextHole::eState)1);

			if (GetGameType() == GAME_TYPE_MATCH && m_pMatchItems)
			{
				m_pMatchItems->SetVisible(false);
				m_pMatchItems->Reset();
			}

			if (GetGameType() == GAME_TYPE_GUILD_MATCH)
			{
				if (m_pGuildScoreDlg)
					m_pGuildScoreDlg->SetVisible(false);

				if (m_bRank3Enabled == 1)
					m_bShowRank3 = 1;
			}
		}
		else if (msg.param1 == 1)
		{
			m_panelState = 2;
			m_fadeAlpha = 0.0f;
			if (m_pRankForm)
				m_pRankForm->SetVisible(false);

			if (m_pRankForm3)
				m_pRankForm3->SetVisible(m_bShowRank3 == 1);

			if (IsMassGame())
			{
				if (m_pRankFormEx)
					m_pRankFormEx->SetActive(true);

				if (m_pScoreForm)
					m_pScoreForm->SetState(FrScoreForm::STATE_1);

				if (m_pTreasureGauge)
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)3);

				if (m_pNextHole)
					m_pNextHole->SetState((FrNextHole::eState)2);

				if (GetGameType() == GAME_TYPE_MATCH && m_pMatchItems)
				{
					m_pMatchItems->SetVisible(false);
					m_pMatchItems->Reset();
				}

				if (GetGameType() == GAME_TYPE_GUILD_MATCH)
				{
					if (m_pGuildScoreDlg)
					{
						m_pGuildScoreDlg->SetVisible(true);
						m_pGuildScoreDlg->SetGuildPlayer(MyGuid(false));
						m_pGuildScoreDlg->SetVisibleCloseBtn(false);
						m_pGuildScoreDlg->SetVisibleCancelBtn(false);
					}

					m_bShowRank3 = 0;
				}
			}
			else
			{
				if (m_pScoreForm)
				{
					if (m_noticeImages.size() > 0)
					{
						int index =
							Doc()->GetMyPartTidList().m_charTid & 0x3ffffff;
						if (&m_noticeImages[index])
						{
							m_pScoreForm->SetPPL(MakeStr("%s/%s", SCORE_PPL_DIR,
								m_noticeImages[index].name));
						}
					}

					m_pScoreForm->SetState(FrScoreForm::STATE_3);
					m_pScoreForm->SetDesc();
				}

				if (m_pTreasureGauge)
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)3);

				if (m_pNextHole)
					m_pNextHole->SetState((FrNextHole::eState)2);

				if (m_pMatchItems)
				{
					if (GetGameType() == GAME_TYPE_MATCH &&
						!GOLFDOC()->m_bNetGameEnd)
					{
						unsigned char div = Doc()->m_golfGame.holes / 9;
						unsigned char holeIndex =
							GetHoleIndex(GOLFDOC()->m_currentHole);

						if (div == 0)
							div = 1;

						if (holeIndex > 0)
						{
							if (holeIndex % div == 0)
								m_pMatchItems->SetVisible(true);
							else
								m_pMatchItems->SetVisible(false);
						}
						else
						{
							m_pMatchItems->SetVisible(false);
						}
					}
				}
			}
		}
		else if (msg.param1 == 4)
		{
			if (m_pTreasureGauge)
				m_pTreasureGauge->SetTPoint();
		}

		m_scoreBg.ScoreBgCommand(msg.param1);
		break;

	case 304:
		m_panelState = 3;
		m_fadeAlpha = 1.0f;
		if (m_pRankForm)
		{
			if (GetGameType() != GAME_TYPE_NEW_APPROACH)
				m_pRankForm->SetVisible(true);
		}

		if (m_pRankForm3)
			m_pRankForm3->SetVisible(m_bShowRank3 == 1);

		if (m_pRankFormEx)
			m_pRankFormEx->SetActive(false);

		if (m_pScoreForm)
			m_pScoreForm->SetState(FrScoreForm::STATE_1);

		if (m_pMatchItems)
			m_pMatchItems->SetVisible(false);
		break;

	case 305:
		if (m_pScoreForm)
			m_pScoreForm->SortLists(true);
		break;

	case 306:
		if (m_pScoreForm)
			m_pScoreForm->UpdatePang();
		break;

	case 42:
		if (IsMassGame())
		{
			if (m_pRankForm)
			{
				m_pRankForm->SortRank();

				if (msg.param1)
					m_pRankForm->Align();
			}

			if (m_pRankForm2)
				m_pRankForm2->SortRank();

			if (m_pRankFormEx)
				m_pRankFormEx->SortRank();
		}
		break;

	case 307:
		if (GetGameType() == GAME_TYPE_30S && m_pRankForm)
		{
			m_pRankForm->m_playerNum--;
			m_pRankForm->m_holePlayerNum[msg.param1]--;
		}
		break;

	case 308:
		if (GetGameType() == GAME_TYPE_30S && m_pRankForm)
		{
			unsigned char prevRank = Doc()->m_rivalList[MyIndex()].rank;
			int score = Doc()->m_rivalList[MyIndex()].totalScore + msg.param1;
			__int64 pang = Doc()->m_rivalList[MyIndex()].totalPang;
			unsigned char rank = 1;

			for (std::vector<sRivalData>::iterator it =
					 Doc()->m_rivalList.begin();
				it != Doc()->m_rivalList.end(); ++it)
			{
				if (score > (*it).totalScore ||
					(score == (*it).totalScore && pang < (*it).totalPang))
					rank++;
			}

			if (prevRank - rank >= 3)
			{
				m_pRankForm->m_pBoard->SetCaption(MakeStr(
					"%s\xb4\xd4 \xc0\xcc\xb9\xf8\xbf\xa1 \xc8\xa6\xc0\xce\xc7"
					"\xcf\xb8\xe9 %d\xb8\xed\xc0\xbb \xc0\xcc\xb1\xe6 \xbc\xf6 "
					"\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					MyNick(), prevRank - rank));
				m_pRankForm->m_captionPriority = 1;
				m_pRankForm->SetCaptionType(2);
			}
		}
		break;

	case 309:
		if (GetGameType() == GAME_TYPE_30S)
		{
			if (m_pRankForm)
			{
				m_pRankForm->m_pBoard->SetCaption(
					"\xc0\xda~ \xb8\xb6\xc1\xf6\xb8\xb7 \xc8\xa6\xc0\xd4\xb4\xcf"
					"\xb4\xd9. \xb8\xb7\xc6\xc7 \xc6\xc4\xc0\xcc\xc6\xc3!");
				m_pRankForm->m_captionPriority = 1;
				m_pRankForm->SetCaptionType(1);
			}

			if (m_pRankForm3)
			{
				m_pRankForm3->m_pBoard->SetCaption(
					"\xc0\xda~ \xb8\xb6\xc1\xf6\xb8\xb7 \xc8\xa6\xc0\xd4\xb4\xcf"
					"\xb4\xd9. \xb8\xb7\xc6\xc7 \xc6\xc4\xc0\xcc\xc6\xc3!");
				m_pRankForm3->SetCaptionType(1);
			}
		}
		break;

	case 310:
		if (GetGameType() == GAME_TYPE_30S && m_pRankForm)
		{
			++m_pRankForm->m_holePlayerNum[msg.param1];
			--m_pRankForm->m_holePlayerNum[msg.param1 - 1];

			if (m_pRankForm->m_captionPriority > 2 &&
				m_pRankForm->m_noticeHole != GOLFDOC()->m_currentHole)
			{
				unsigned char maxNum = 0;
				unsigned char maxHole = 0;

				for (int i = 0; i < 19; i++)
				{
					if (m_pRankForm->m_holePlayerNum[i] > maxNum)
					{
						maxNum = m_pRankForm->m_holePlayerNum[i];
						maxHole = i + 1;
					}
				}

				if (GetHoleIndex(maxHole) >=
					GetHoleIndex(GOLFDOC()->m_currentHole) + 5)
				{
					m_pRankForm->m_noticeHole = GOLFDOC()->m_currentHole;

					m_pRankForm->m_pBoard->SetCaption(MakeStr(
						"%s\xb4\xd4 \xb8\xb9\xc0\xcc \xb5\xda\xc3\xc4\xc1\xae "
						"\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xbc\xad\xb5\xce\xb8"
						"\xa3\xbc\xbc\xbf\xe4.",
						MyNick()));
					m_pRankForm->m_captionPriority = 2;
					m_pRankForm->SetCaptionType(2);
				}
			}
		}
		break;

	case 311:
		if (GetGameType() == GAME_TYPE_30S && m_pRankForm)
		{
			if (m_pRankForm->m_captionPriority > 3)
			{
				if (m_pRankForm->m_lastOID == MyGuid(true))
				{
					m_pRankForm->m_pBoard->SetCaption(MakeStr(
						"%s\xb4\xd4 \xb2\xc3\xc2\xee\xbf\xb9\xbf\xe4. \xc8\xfb"
						"\xb3\xbb\xbc\xbc\xbf\xe4.",
						Doc()
							->m_rivalList[Doc()->GetIndex(MyGuid(true))]
							.nickname));
					m_pRankForm->m_captionPriority = 3;
					m_pRankForm->SetCaptionType(1);
				}
			}
		}
		break;

	case 312:
		if (m_pRankFormEx)
			*(bool*)msg.param1 = m_pRankFormEx->IsMotionFinished();
		else if (GetGameType() == GAME_TYPE_GUILD_MATCH)
			*(bool*)msg.param1 = m_fadeAlpha >= 1.0f ? true : false;
		else
			*(bool*)msg.param1 = true;
		break;

	case 10011:
	{
		WPoint pos;

		if (m_pRankForm)
		{
			pos.x = g_view->GetWidth() - m_pRankForm->GetRect().w;
			pos.y = 40.0f;
			m_pRankForm->MoveWindow(pos);
		}

		if (m_pRankForm2)
		{
			pos.x = (g_view->GetWidth() - m_pRankForm2->GetRect().w) * 0.5f;
			pos.y = 40.0f;
			m_pRankForm->MoveWindow(pos);
		}

		if (m_pRankForm3)
		{
			pos.x = g_view->GetWidth() - m_pRankForm3->GetRect().w + 5.0f;
			pos.y = 40.0f;
			m_pRankForm3->MoveWindow(pos);
		}

		if (m_pRankFormEx)
		{
			pos.x = (float)float2int(
				(g_view->GetWidth() - m_pRankFormEx->GetRect().w) * 0.5f);
			pos.y = (float)float2int(
				(g_view->GetHeight() - m_pRankFormEx->GetRect().h) * 0.5f);
			m_pRankFormEx->MoveWindow(pos);
			m_pRankFormEx->ResetStage();
		}

		if (m_pScoreForm)
		{
			pos.x = (float)float2int(
				(g_view->GetWidth() - m_pScoreForm->GetRect().w) * 0.5f);
			pos.y = 20.0f;
			m_pScoreForm->MoveWindow(pos);
		}

		if (m_pAwardForm)
		{
			float barY = COption::Instance()->gGetBar_Y();
			pos.x = (float)float2int(
				(g_view->GetWidth() - m_pAwardForm->GetRect().w) * 0.5f);
			pos.y = barY - 100.0f;
			m_pAwardForm->MoveWindow(pos);
		}
		break;
	}

	case 313:
	{
		bool* pOver = (bool*)msg.param1;
		*pOver = m_panelState != 0;

		if (m_pRankForm)
		{
			WRect rect;
			m_pRankForm->GetClientRect(rect);
			if (rect.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
				*pOver = true;
		}

		if (m_pRankForm3)
		{
			WRect rect;
			m_pRankForm3->GetClientRect(rect);
			if (rect.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
				*pOver = true;
		}
		break;
	}

	case 314:
	{
		unsigned char birdie = 0;
		unsigned char eagle = 0;
		unsigned char albatross = 0;

		for (unsigned char hole = GetStartHole(); hole <= GetEndHole(); hole++)
		{
			switch (GOLFDOC()->GetHoleData(hole).playerNum)
			{
			case 2:
				birdie++;
				break;

			case 3:
				eagle++;
				break;

			case 4:
				albatross++;
				break;
			}
		}

		unsigned char difficulty = GetDifficulty() - 1;

		*(int*)msg.param1 = g_ExperienceTable[difficulty][birdie][2] +
			g_ExperienceTable[difficulty][eagle][4] +
			g_ExperienceTable[difficulty][albatross][7];
		break;
	}

	case 315:
		if (m_pRankForm2)
		{
			switch (msg.param1)
			{
			case -1:
				m_pRankForm2->SetVisible(!m_pRankForm2->IsVisible());
				break;

			case 0:
				m_pRankForm2->SetVisible(false);
				break;

			case 1:
				m_pRankForm2->SetVisible(true);
				break;
			}
		}
		break;

	case 87:
		m_pRankForm3->SetScore(msg.param1, msg.param2);
		break;

	case 316:
		UpdateRivals();
		break;
	}
}

bool CScore::OnLevelupFormResult(int result, FrForm* form)
{
	if (!m_levelupForms.empty())
	{
		m_levelupForms[m_levelupForms.size() - 1] = NULL;
		m_levelupForms.erase(m_levelupForms.end() - 1);
	}

	return true;
}

bool CScore::OnRankFormExResult(int result, FrForm* form)
{
	if (m_pRankFormEx)
	{
		m_pRankFormEx->Clear();
		m_pRankFormEx = NULL;
	}

	return true;
}

bool CScore::UpdateRivals()
{
	if (m_pRankForm)
	{
		FrListBox* pList = m_pRankForm->m_pRankList;

		if (pList)
		{
			pList->ClearItem();
			for (std::vector<sRivalData>::iterator it =
					 Doc()->m_rivalList.begin();
				it != Doc()->m_rivalList.end(); ++it)
				pList->AddItem(&(*it));

			pList->UseRightButton(true);

			m_pRankForm->m_playerNum = Doc()->m_rivalList.size();
			m_pRankForm->m_holePlayerNum[0] = Doc()->m_rivalList.size();
			m_pRankForm->SortRank();
		}
	}

	if (m_pRankFormEx)
	{
		if (m_pRankFormEx->GetRival())
			m_pRankFormEx->m_rankExData.ClearRival();

		m_pRankFormEx->m_rankExData.SetRivalData();
	}

	return true;
}

CScore::CScoreBg::CScoreBg()
{
	m_pBitmap = NULL;
	m_bCaptured = false;
}

CScore::CScoreBg::~CScoreBg()
{
	if (m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	m_bCaptured = false;
	g_pFresh->GetDesktop()->LockWallPaper(false);
}

void CScore::CScoreBg::SetDesktopWallPaper(bool bVisible, bool bCaptured)
{
	if (m_bCaptured)
	{
		g_pFresh->GetDesktop()->SetWallPaperFromBitmap(m_pBitmap, bVisible);
		if (m_pBitmap)
		{
			delete m_pBitmap;
			m_pBitmap = NULL;
		}
	}
	else if (bCaptured && bVisible)
	{
		g_pFresh->GetDesktop()->SetWallPaper("scoreboard_background.jpg",
			bVisible);
	}
}

void CScore::CScoreBg::ScoreBgCommand(int command)
{
	switch (command)
	{
	case 0:
		SetDesktopWallPaper(false, false);
		break;

	case 1:
	case 2:
		SetDesktopWallPaper(true, true);
		break;

	case 3:
		if (GOLFDOC()->m_gameMode == 0x400000)
		{
			if (m_pBitmap)
			{
				delete m_pBitmap;
				m_pBitmap = NULL;
			}
			m_pBitmap = new Bitmap((int)g_view->GetWidth(),
				(int)g_view->GetHeight(), 24);

			if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0,
					(int)m_pBitmap) == 1)
			{
				m_bCaptured = true;
				g_pFresh->GetDesktop()->LockWallPaper(true);
			}
			else
			{
				if (m_pBitmap)
				{
					delete m_pBitmap;
					m_pBitmap = NULL;
				}
				m_bCaptured = false;
				g_pFresh->GetDesktop()->LockWallPaper(false);
			}
		}
		break;
	}
}

bool CScore::OnGuildScoreFormResult(int result, FrForm* form)
{
	if (m_pGuildScoreDlg)
		m_pGuildScoreDlg = NULL;

	return true;
}
