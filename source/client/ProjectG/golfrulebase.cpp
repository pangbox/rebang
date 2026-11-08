#include "golfdoc.h"
#include "golfrule.h"
#include "golfrulebase.h"
#include "clientsetting.h"
#include "polysoup.h"
#include "cardsystem.h"
#include "gatewayactor.h"
#include "../../shared/localize.h"

CGolfRuleBase::CGolfRuleBase()
{
	m_pGolfRule = NULL;

	m_playerColor[0] = 0xfd0002;
	m_playerColor[1] = 0xdc00fd;
	m_playerColor[2] = 0x27d628;
	m_playerColor[3] = 0x2778d6;
}

CGolfRuleBase::~CGolfRuleBase()
{
}

void CGolfRuleBase::CheckCalipersCount()
{
	AfxGetTask()->GetActor(s_gatewayActor[1])
		<< MsgObject(NULL, 0x26f, 0, 0, 0, 0, 0);

	std::list<sItemInfo>& itemList = Doc()->m_myItemList;
	std::list<sItemInfo>::iterator it;
	for (it = itemList.begin(); it != itemList.end(); ++it)
	{
		if ((*it).tid == 0x1a000040)
			break;
	}

	if (it == Doc()->m_myItemList.end())
		return;

	if (Doc()->m_gameMode == 1)
		return;

	IActor* pGateway = AfxGetTask()->GetActor(s_gatewayActor[1]);
	pGateway << MsgObject(NULL, 0x26f, 5, (*it).Common[0], 0, 0, 0);
	pGateway << MsgObject(NULL, 0x26f, 2, 0, 0, 0, 0);
}

void CGolfRuleBase::SetPlayer()
{
	if (m_pGolfRule == NULL)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		m_pGolfRule->SendHoleData();

		std::list<sSlotInfo>& slotList = Doc()->m_slotList;

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

void CGolfRuleBase::SetGrade()
{
	for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; ++i)
	{
		if (PLAYER(i)->state != 3)
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

void CGolfRuleBase::SetReady()
{
	if (IsLocalContent(S3_AUTOCALIPERS_ITEM))
		CheckCalipersCount();

	AfxGetTask()->GetActor(s_gatewayActor[1])
		<< MsgObject(NULL, 0x26f, 10, 0, 0, 0, 0);

	m_pGolfRule->ChangeGameMode(1);
}

void CGolfRuleBase::AddPlayer(unsigned char index, const char* name,
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

	pPlayer->gauge = gauge;

	pPlayer->gauge += Doc()->GetWtPepPangyaComboGauge(index);

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(index);
		pPlayer->gauge += CCardManager::Instance()->GetCardPeriodComboGauge();
	}

	if (Doc()->m_curChannel.Type & 8)
	{
		pPlayer->gauge = 0;

		for (int k = 0; k < 10; ++k)
			Doc()->m_userInfo[index].userEquip.tidItemSlot[k] = 0;
	}

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

	sCharacterInfo* pCharInfo = &Doc()->m_userInfo[index].charInfo;
	CPartTidList& tidList = GOLFDOC()->GetPartTidList(index);
	tidList.SetTids(pCharInfo, 0xff);

	pPlayer->itemNum = Doc()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}

void CGolfRuleBase::AddPlayerOffline(unsigned char index, const char* name,
	unsigned long oid, unsigned long caddie)
{
	if (index >= GOLFDOC()->m_playerNum)
		return;

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

	strcpy(Doc()->m_userInfo[index].info.sNick, name);

	pPlayer->oid = oid;
	pPlayer->state = 2;
	pPlayer->chatBlock = 0;
	pPlayer->itemNum = 0;
	pPlayer->item = 0;
	pPlayer->totalStroke = 0;
	pPlayer->shotTime = 0;
	pPlayer->rank = 1;
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

	Doc()->SetIndex(index);
	GOLFDOC()->GetPlayer(index)->index = index;

	Doc()->m_userInfo[index].userEquip.tidItemSlot[0] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[1] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[2] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[3] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[4] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[5] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[6] = 0x18000010;
	Doc()->m_userInfo[index].userEquip.tidItemSlot[7] = 0x18000010;
	GOLFDOC()->GetPlayer(index)->itemNum = 10;

	int character;
	if (index == 0)
		character = COption::Instance()->dGetDefaultCharacter();
	else
		character = rand() % ItemManager()->GetNumItems(PIG_CHAR);

	int club = COption::Instance()->dGetDefaultClub();
	int ball = COption::Instance()->dGetDefaultBall();

	unsigned long parts[24];
	unsigned long charTid = character | 0x4000000;
	ItemManager()->GetDefCombo(charTid, parts);

	if (character == 0)
	{
		parts[3] = 0x8006001;
		parts[5] = 0x800a000;
		parts[6] = 0;
		parts[7] = 0;
		parts[8] = 0x8010002;
		parts[19] = 0x8026800;
	}
	else if (character == 1)
	{
		parts[0] = 0x8040802;
		parts[2] = 0x8044000;
		parts[4] = 0;
		parts[5] = 0;
		parts[7] = 0x804e001;
		parts[18] = 0x8064800;
	}
	else if (character == 2)
	{
		parts[0] = 0x8080801;
		parts[1] = 0;
		parts[2] = 0x8084004;
		parts[3] = 0;
		parts[4] = 0x8088801;
		parts[5] = 0x808a004;
		parts[6] = 0;
		parts[7] = 0x808e003;
		parts[8] = 0x8090800;
		parts[9] = 0x8083001;
		parts[17] = 0x80a2800;
	}
	else if (character == 3)
	{
		parts[2] = 0x80c4003;
		parts[3] = 0;
		parts[5] = 0x80ca003;
		parts[7] = 0x80ce003;
		parts[8] = 0x80d0801;
		parts[10] = 0x80d4800;
		parts[18] = 0x80e4800;
	}
	else if (character == 4)
	{
		parts[2] = 0x8104002;
		parts[3] = 0x8106001;
		parts[4] = 0x8108002;
		parts[5] = 0x810a002;
		parts[9] = 0x8112000;
		parts[10] = 0x8114000;
		parts[17] = 0x8122800;
	}
	else if (character == 5)
	{
		parts[23] = 0x816e801;
	}
	else if (character == 6)
	{
		parts[18] = 0x81a4800;
	}
	else if (character == 7)
	{
		parts[21] = 0x81ea800;
	}
	else if (character == 8)
	{
		parts[20] = 0x8228800;
	}
	else if (character == 9)
	{
		parts[2] = 0x824400b;
		parts[5] = 0;
		parts[20] = 0x8268800;
	}

	GOLFDOC()->SetPartTidList(index, charTid, 0, 0, parts, NULL);

	if (Doc()->m_offlinePlay)
		GOLFDOC()->GetPartTidList(index).SetDefaultTids();

	Doc()->m_userInfo[index].userEquip.guidChar = 0;
	Doc()->m_userInfo[index].userEquip.guidCaddie = 0;
	Doc()->m_userInfo[index].userEquip.guidClubSet = 0;
	Doc()->m_userInfo[index].userEquip.tidBall = ball | 0x14000000;
	Doc()->m_userInfo[index].userEquip.guidMascot = 0;
	Doc()->m_userInfo[index].charInfo.tid = charTid;
	memcpy(Doc()->m_userInfo[index].charInfo.tidParts, parts, sizeof(parts));
	Doc()->m_userInfo[index].charInfo.tidAuxParts[0] = 0x70000080;
	Doc()->m_userInfo[index].caddieInfo.guid = caddie ? index : 0;
	Doc()->m_userInfo[index].caddieInfo.tid = caddie;
	Doc()->m_userInfo[index].clubInfo.tid = club | 0x10000000;

	rand();

	Doc()->m_userInfo[index].stat.Level = 0;
	Doc()->m_userInfo[index].stat.dwExp = rand() % Doc()->m_levelTable[0].exp;

	GOLFDOC()->SetPlayerLevel(index);
}

int CGolfRuleBase::GetNumTimeout()
{
	return PLAYER(GOLFDOC()->m_currentPlayer)->timeout;
}

bool CGolfRuleBase::CheckGiveUp()
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->state == 3)
		return false;

	unsigned char limit = GOLFDOC()->GetHoleData().par + 4;
	int state = PLAYER(GOLFDOC()->m_currentPlayer)->state;
	unsigned char stroke = PLAYER(GOLFDOC()->m_currentPlayer)
							   ->stroke[GOLFDOC()->m_currentHole - 1];

	if (state == 1)
		return stroke > limit;

	return stroke >= limit;
}
