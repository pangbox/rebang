#include "golfdoc.h"
#include "golfrule.h"
#include "golfrulebase.h"
#include "clientsetting.h"
#include "polysoup.h"

CGolfRuleMatch::CGolfRuleMatch(CGolfRule* pGolfRule)
{
	m_pGolfRule = pGolfRule;
}

CGolfRuleMatch::~CGolfRuleMatch()
{
}

void CGolfRuleMatch::SetPlayer()
{
	if (m_pGolfRule == NULL)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		std::list<sSlotInfo>& slotList = Doc()->m_slotList;

		m_pGolfRule->SendHoleData();

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

		m_pGolfRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(
			m_pGolfRule->GetShotData()->hole);
	}
}

void CGolfRuleMatch::SetGrade()
{
	for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; ++i)
	{
		if (GOLFDOC()->GetPlayer(i)->state != 3)
		{
			unsigned char grade;
			if (Doc()->m_userInfo[i].stat.Level < 6)
				grade = 1;
			else
				grade = (Doc()->m_userInfo[i].stat.Level - 1) / 5 + 1;

			if (GOLFDOC()->m_maxGrade < grade)
				GOLFDOC()->m_maxGrade = grade;
		}
	}
}

void CGolfRuleMatch::SetReady()
{
	CGolfRuleBase::SetReady();
}

void CGolfRuleMatch::AddPlayer(unsigned char index, const char* name,
	unsigned long uid, unsigned long oid, unsigned long caddie)
{
	if (index >= GOLFDOC()->m_playerNum)
		return;

	float gauge = 0.0f;

	sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
	pPlayer->teeOrder = 0;
	pPlayer->order = 1;

	for (int i = 0; i < index; ++i)
	{
		if (GOLFDOC()->GetPlayer(i)->state != 3)
		{
			++pPlayer->teeOrder;
			++pPlayer->order;
		}
	}

	pPlayer->color = m_playerColor[index];

	pPlayer->oid = oid;
	pPlayer->uid = uid;
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

	pPlayer->gauge = gauge;

	pPlayer->gauge += Doc()->GetWtPepPangyaComboGauge(index);

	if (Doc()->m_curChannel.Type & 8)

		pPlayer->gauge = 0;

	pPlayer->score = 0;
	pPlayer->totalPang = 0;
	pPlayer->bonusPang = 0;

	pPlayer->skinsHolePang = 0;
	pPlayer->skinsTotalPang = 0;
	pPlayer->skinsSumPang = 0;

	pPlayer->holeNum = 0;

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

	for (int k = 0; k < 10; ++k)
	{
		Doc()->m_userInfo[index].userEquip.tidItemSlot[k] = 0x1800000d;
	}

	sCharacterInfo* pCharInfo = &Doc()->m_userInfo[index].charInfo;

	CPartTidList& tidList = GOLFDOC()->GetPartTidList(index);
	tidList.SetTids(pCharInfo, 0xff);

	pPlayer->itemNum = Doc()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}
