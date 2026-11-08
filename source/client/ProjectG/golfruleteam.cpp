#include "golfdoc.h"
#include "golfrule.h"
#include "golfrulebase.h"
#include "clientsetting.h"
#include "polysoup.h"
#include "cardsystem.h"
#include "../../shared/localize.h"

CGolfRuleTeam::CGolfRuleTeam(CGolfRule* pGolfRule)
{
	m_pGolfRule = pGolfRule;
}

CGolfRuleTeam::~CGolfRuleTeam()
{
}

void CGolfRuleTeam::SetPlayer()
{
	if (m_pGolfRule == NULL)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		m_pGolfRule->SendHoleData();

		std::list<sSlotInfo>& slotList = Doc()->m_slotList;

		GOLFDOC()->MakeTeam();
		GOLFDOC()->ResetTeamVars();
		GOLFDOC()->MakePlayer(slotList.size());

		unsigned char index = 0;
		for (std::list<sSlotInfo>::iterator it = slotList.begin();
			it != slotList.end(); ++it, ++index)
		{
			if (it->dwGuid == -1)
			{
				GOLFDOC()->GetPlayer(index)->state = 3;
				continue;
			}

			Doc()->SetIndex(it->dwGuid);

			GOLFDOC()->GetPlayer(index)->index = index;
			sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
			pPlayer->team = it->bTeam;
			m_pGolfRule->AddPlayer(index, it->sNick, it->dwUserUID, it->dwGuid,
				Doc()->m_userInfo[index].caddieInfo.tid);
		}

		GOLFDOC()->RefreshPlayerNum();
	}
	else
	{
		Doc()->m_golfGame.shotTimeLimit = 0;
		COption::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		m_pGolfRule->LoadShot(Doc()->GetFileName());

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(
			m_pGolfRule->GetShotData()->hole);
	}
}

void CGolfRuleTeam::SetGrade()
{
	unsigned char i;

	for (i = 0; i < 2; ++i)
	{
		unsigned char sum = 0;

		for (i = 0; i < GOLFDOC()->m_playerNum; ++i)
		{
			if (PLAYER(i)->team == i)
			{
				if (Doc()->m_userInfo[i].stat.Level < 6)
					++sum;
				else
					sum += (Doc()->m_userInfo[i].stat.Level - 1) / 5 + 1;
			}
		}

		unsigned char grade = sum / 2;
		if (GOLFDOC()->m_maxGrade < grade)
			GOLFDOC()->m_maxGrade = grade;
	}
}

void CGolfRuleTeam::SetReady()
{
	CGolfRuleBase::SetReady();
}

void CGolfRuleTeam::AddPlayer(unsigned char index, const char* name,
	unsigned long uid, unsigned long oid, unsigned long caddie)
{
	if (index >= GOLFDOC()->m_playerNum)
		return;

	float gauge = 0.0f;

	sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
	pPlayer->teammate = 0xff;

	for (int i = 0; i < index; ++i)
	{
		if (GOLFDOC()->GetPlayer(i)->team == pPlayer->team)
		{
			GOLFDOC()->GetPlayer(i)->teammate = index;
			pPlayer->teammate = i;
			break;
		}
	}

	pPlayer->color = TEAM(pPlayer->team)->color;

	pPlayer->uid = uid;
	pPlayer->oid = oid;
	pPlayer->state = 2;
	pPlayer->chatBlock = 0;
	pPlayer->itemNum = 0;
	pPlayer->item = 0;
	pPlayer->totalStroke = 0;
	pPlayer->shotTime = 0;
	pPlayer->rank = 1;

	if (Doc()->m_userInfo[index].caddieInfo.tidPart)
	{
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(
			Doc()->m_userInfo[index].caddieInfo.tidPart);

		if (pCadItem)
		{
			gauge = pCadItem->COM[4];
		}
	}

	gauge += Doc()->GetWtPepPangyaComboGauge(index);

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(index);
		gauge += CCardManager::Instance()->GetCardPeriodComboGauge();
	}

	if (TEAM(pPlayer->team)->gauge < gauge)
	{
		float limit = Doc()->GetComboGaugeLimit(pPlayer->team);
		TEAM(pPlayer->team)->gauge =
			gauge < 0.0f ? 0.0f : (gauge > limit ? limit : gauge);
	}

	pPlayer->gauge = 0;

	if (Doc()->m_curChannel.Type & 8)
		pPlayer->gauge = 0;

	pPlayer->score = 0;
	pPlayer->totalPang = 0;
	pPlayer->bonusPang = 0;

	pPlayer->skinsHolePang = 0;
	pPlayer->holeNum = 0;

	pPlayer->skinsTotalPang = 0;
	pPlayer->skinsSumPang = 0;

	pPlayer->extPrizeNum = 0;
	memset(&pPlayer->prize, 0, sizeof(pPlayer->prize));
	pPlayer->level = Doc()->m_userInfo[index].stat.Level;

	for (int j = 0; j < 18; ++j)
	{
		pPlayer->stroke[j] = 0;
		pPlayer->putt[j] = 0;
		pPlayer->pang[j] = 0;
		pPlayer->skinsPang[j] = 0;
	}

	if (Doc()->m_curChannel.Type & 8)
	{
		for (int k = 0; k < 10; ++k)
		{
			Doc()->m_userInfo[index].userEquip.tidItemSlot[k] = 0;
		}
	}

	sCharacterInfo* pCharInfo = &Doc()->m_userInfo[index].charInfo;
	CPartTidList& tidList = GOLFDOC()->GetPartTidList(index);
	tidList.SetTids(pCharInfo, 0xff);

	pPlayer->itemNum = Doc()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}

int CGolfRuleTeam::GetNumTimeout()
{
	int num = PLAYER(GOLFDOC()->m_currentPlayer)->timeout;

	if (PLAYER(GOLFDOC()->m_currentPlayer)->teammate != 0xff)
		num += PLAYER(PLAYER(GOLFDOC()->m_currentPlayer)->teammate)->timeout;

	return num;
}

bool CGolfRuleTeam::CheckGiveUp()
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->state == 3)
		return false;

	unsigned char limit = GOLFDOC()->GetHoleData().par + 4;
	int state = TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->state;
	unsigned char stroke = TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							   ->stroke[GOLFDOC()->m_currentHole - 1];

	if (state == 1)
		return stroke > limit;

	return stroke >= limit;
}
