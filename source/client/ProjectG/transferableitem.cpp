#include "stdafx.h"
#include "transferableitem.h"
#include "ucclibrary.h"

CTransferableItem::CTransferableItem()
{
}

CTransferableItem::~CTransferableItem()
{
}

bool CompareTradeItem(const sTradeItem& a, const sTradeItem& b)
{
	return a.dwTid < b.dwTid;
}

void CTransferableItem::OnLoad()
{
	Reset();
}

void CTransferableItem::OnDestory()
{
	m_transferableList.clear();
	m_registrationList.clear();
	m_equipCharList.clear();
	m_registrationListSize = 0;
}

void CTransferableItem::Process(float elapsed)
{
}

void CTransferableItem::Render()
{
}

void CTransferableItem::Reset()
{
	m_transferableList.clear();
	m_registrationList.clear();
	m_equipCharList.clear();
	m_registrationListSize = 0;
}

std::list<sTradeItem>* CTransferableItem::GetList_TransferableItem()
{
	return &m_transferableList;
}

void CTransferableItem::ClearTransferableItemList()
{
	m_transferableList.clear();
}

void CTransferableItem::SetTransferableItemList()
{
	m_transferableList.clear();
	sTradeItem trade;
	std::list<sItemInfo>* list;
	ITEM_LIST_IT parts;
	list = &Doc()->m_partsList;

	for (parts = list->begin(); parts != list->end(); parts++)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem((*parts).tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*parts).guid;
		trade.dwTid = (*parts).tid;
		trade.iNum = 1 - GetNumTradingItem((*parts).guid);
		trade.i64Price = 0;

		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart((*parts).tid);
			if (part)
			{
				trade.CharacterSlotNum = part->CharacterSlot;
				trade.CaddieSlotNum = part->CaddieSlot;
				memset(trade.attachCardTid, 0, sizeof(trade.attachCardTid));
				std::list<sSCardAvilityPeriodInfo>::iterator card;
				for (card = Doc()->m_cardAbilityList[0].begin();
					card != Doc()->m_cardAbilityList[0].end(); card++)
					if ((*card).partsTid == (*parts).tid &&
						(*card).partsUid == (*parts).guid)
					{
						int index = (*card).slotNum - 1;
						if (((*card).tid & 0x3c00000) == 0x400000)
							index += 4;
						((unsigned long*)trade.attachCardTid)[index] =
							(*card).tid;
					}
			}
		}
		m_transferableList.push_back(trade);
	}

	if (IsLocalContent(S4_UCC))
	{
		ITEM_LIST_IT ucc;
		list = &Doc()->m_uccItemList;
		for (ucc = list->begin(); ucc != list->end(); ucc++)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem((*ucc).tid);
			if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
				continue;
			bool equipped = false;
			CHAR_MAP_IT character;
			for (character = Doc()->m_charMap.begin();
				character != Doc()->m_charMap.end(); character++)
				if ((*character).second.tid ==
						((((*ucc).tid >> 18) & 255) | 0x4000000) &&
					(*character).second.ItemIdList[((*ucc).tid >> 13) & 31] ==
						(*ucc).guid)
				{
					equipped = true;
					break;
				}
			if (equipped)
				continue;
			trade.dwGuid = (*ucc).guid;
			trade.dwTid = (*ucc).tid;
			strncpy(trade.UccIndex, (*ucc).UccIndex, 9);
			trade.Seq = (*ucc).Seq;
			trade.iNum = 1;
			trade.i64Price = 0;
			trade.status = (*ucc).status;

			if (IsLocalContent(S4_CARD_SYSTEM))
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart((*ucc).tid);
				if (part)
				{
					trade.CharacterSlotNum = part->CharacterSlot;
					trade.CaddieSlotNum = part->CaddieSlot;
					memset(trade.attachCardTid, 0, sizeof(trade.attachCardTid));
					std::list<sSCardAvilityPeriodInfo>::iterator card;
					for (card = Doc()->m_cardAbilityList[0].begin();
						card != Doc()->m_cardAbilityList[0].end(); card++)
						if ((*card).partsTid == (*ucc).tid &&
							(*card).partsUid == (*ucc).guid)
						{
							int index = (*card).slotNum - 1;
							if (((*card).tid & 0x3c00000) == 0x400000)
								index += 4;
							((unsigned long*)trade.attachCardTid)[index] =
								(*card).tid;
						}
				}
			}
			m_transferableList.push_back(trade);
		}
	}

	ITEM_LIST_IT aux;
	list = &Doc()->m_auxPartList;
	for (aux = list->begin(); aux != list->end(); aux++)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem((*aux).tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*aux).guid;
		trade.dwTid = (*aux).tid;
		trade.iNum = (*aux).Common[0] - GetNumTradingItem((*aux).guid);
		trade.i64Price = 0;
		m_transferableList.push_back(trade);
	}

	SetListEquipParts();
	TRADEITEM_LIST_IT equipped;

	for (equipped = m_transferableList.begin();
		equipped != m_transferableList.end();)
	{
		if (IsEquipParts((*equipped).dwTid, (*equipped).dwGuid))
			equipped = m_transferableList.erase(equipped);
		else
			equipped++;
	}

	ITEM_LIST_IT items;
	list = &Doc()->m_myItemList;

	for (items = list->begin(); items != list->end(); items++)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem((*items).tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*items).guid;
		trade.dwTid = (*items).tid;
		trade.iNum = (*items).Common[0] - GetNumTradingItem((*items).guid) -
			GetNumEquipItem((*items).tid);
		trade.i64Price = 0;
		if (trade.iNum > 0)
			m_transferableList.push_back(trade);
	}

	ITEM_LIST_IT balls;
	list = &Doc()->m_ballList;

	for (balls = list->begin(); balls != list->end(); balls++)
	{
		if ((*balls).tid == Doc()->m_myInfo.userEquip.tidBall)
			continue;
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem((*balls).tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*balls).guid;
		trade.dwTid = (*balls).tid;
		trade.iNum = (*balls).Common[0] - GetNumTradingItem((*balls).guid);
		trade.i64Price = 0;
		m_transferableList.push_back(trade);
	}

	CADDIE_MAP_IT caddie;
	std::map<unsigned int, sCaddieInfo>& caddieMapRef = Doc()->m_caddieMap;

	for (caddie = caddieMapRef.begin(); caddie != caddieMapRef.end(); caddie++)
	{
		if ((*caddie).second.guid == Doc()->m_myInfo.userEquip.guidCaddie)
			continue;
		IFF_ITEM_COMMON* common =
			ItemManager()->FindCommonItem((*caddie).second.tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*caddie).second.guid;
		trade.dwTid = (*caddie).second.tid;
		trade.iNum = 1 - GetNumTradingItem((*caddie).second.guid);
		trade.i64Price = 0;
		m_transferableList.push_back(trade);
	}

	CLUBSET_MAP_IT club;
	std::map<unsigned int, sItemInfo>& clubSetMapRef = Doc()->m_clubSetMap;

	for (club = clubSetMapRef.begin(); club != clubSetMapRef.end(); club++)
	{
		if ((*club).second.guid == Doc()->m_myInfo.userEquip.guidClubSet)
			continue;
		IFF_ITEM_COMMON* common =
			ItemManager()->FindCommonItem((*club).second.tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		memset(&trade, 0, sizeof(trade));
		trade.dwGuid = (*club).second.guid;
		trade.dwTid = (*club).second.tid;
		trade.iNum = 1 - GetNumTradingItem((*club).second.guid);
		trade.i64Price = 0;
		trade.dwUpgradeCost = 0;
		memcpy(trade.arrEnchant, (*club).second.Common,
			sizeof(trade.arrEnchant));
		for (char i = 0; i < 5; ++i)
		{
			for (int j = 0; j < trade.arrEnchant[i]; ++j)
			{
				IFF_STRUCT::sEnchant* enchant =
					ItemManager()->FindEnchant(j | ((i | 0x340) << 20));
				if (enchant)
					trade.dwUpgradeCost += enchant->ReqdPang;
			}
		}
		m_transferableList.push_back(trade);
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		std::list<sCards>::iterator cards;
		std::list<sCards>& cardListRef = Doc()->m_cardList;

		for (cards = cardListRef.begin(); cards != cardListRef.end(); cards++)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem((*cards).tid);
			if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
				continue;
			trade.dwGuid = (*cards).uid;
			trade.dwTid = (*cards).tid;
			trade.iNum = (*cards).cardCount;
			trade.i64Price = 0;
			m_transferableList.push_back(trade);
		}
	}
}

bool CTransferableItem::GetTransferableItem(sItemInfo& item, sTradeItem& trade,
	bool bCheck)
{
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item.tid);
	int count = bCheck ? item.Common[0] : 1;

	if (!common)
		return false;

	if (common->IsSalable == 1 || common->IsSalable == 3)
	{
		if (!count)
			return false;
	}
	else
	{
		return false;
	}

	trade.iNum = count;
	trade.dwGuid = item.guid;
	trade.dwTid = item.tid;
	trade.i64Price = 0;

	if (IsLocalContent(S4_UCC))
	{
		if (IsUccClothes(item.tid))
		{
			strncpy(trade.UccIndex, item.UccIndex, 8);
			trade.Seq = item.Seq;
			trade.status = item.status;
		}
	}

	return true;
}

void CTransferableItem::SetTransferableEtcItemList()
{
	sTradeItem trade;
	CADDIE_MAP_IT caddie;
	std::map<unsigned int, sCaddieInfo>& caddieMapRef = Doc()->m_caddieMap;

	for (caddie = caddieMapRef.begin(); caddie != caddieMapRef.end(); caddie++)
	{
		if ((*caddie).second.guid == Doc()->m_myInfo.userEquip.guidCaddie)
			continue;
		memset(&trade, 0, sizeof(trade));
		IFF_ITEM_COMMON* common =
			ItemManager()->FindCommonItem((*caddie).second.tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		trade.dwGuid = (*caddie).second.guid;
		trade.dwTid = (*caddie).second.tid;
		trade.iNum = 1;
		trade.i64Price = 0;
		m_transferableList.push_back(trade);
	}

	CLUBSET_MAP_IT club;
	std::map<unsigned int, sItemInfo>& clubSetMapRef = Doc()->m_clubSetMap;

	for (club = clubSetMapRef.begin(); club != clubSetMapRef.end(); club++)
	{
		if ((*club).second.guid == Doc()->m_myInfo.userEquip.guidClubSet)
			continue;
		memset(&trade, 0, sizeof(trade));
		IFF_ITEM_COMMON* common =
			ItemManager()->FindCommonItem((*club).second.tid);
		if (!common || (common->IsSalable != 1 && common->IsSalable != 3))
			continue;
		memset(&trade, 0, sizeof(trade));
		trade.dwGuid = (*club).second.guid;
		trade.dwTid = (*club).second.tid;
		trade.iNum = 1;
		trade.i64Price = 0;
		trade.dwUpgradeCost = 0;
		memcpy(trade.arrEnchant, (*club).second.Common,
			sizeof(trade.arrEnchant));
		for (char i = 0; i < 5; ++i)
			for (int j = 0; j < trade.arrEnchant[i]; ++j)
			{
				IFF_STRUCT::sEnchant* enchant =
					ItemManager()->FindEnchant(j | ((i | 0x340) << 20));
				if (enchant)
					trade.dwUpgradeCost += enchant->ReqdPang;
			}
		m_transferableList.push_back(trade);
	}
}

void CTransferableItem::SetListEquipParts()
{
	m_equipCharList.clear();
	CHAR_MAP_IT it;
	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); it++)
		for (int i = 0; i < 24; ++i)
			if ((*it).second.tidParts[i])
				m_equipCharList.push_back((*it).second.guid);
}

bool CTransferableItem::IsEquipParts(unsigned long uid)
{
	bool result = false;
	std::list<unsigned long>::iterator it;

	for (it = m_equipCharList.begin(); it != m_equipCharList.end(); it++)
	{
		if (uid == *it)
		{
			result = true;
			break;
		}
	}

	return result;
}

bool CTransferableItem::IsUccEquipped(sItemInfo* item)
{
	bool result = false;
	CHAR_MAP_IT it;

	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); it++)
	{
		sCharacterInfo* character = &(*it).second;
		if (character)
		{
			result = true;
			break;
		}
		break;
	}

	return result;
}

int CTransferableItem::GetNumEquipItem(unsigned long typeId)
{
	int count = 0;
	for (int i = 0; i < 10; ++i)
		if (typeId == Doc()->m_myInfo.userEquip.tidItemSlot[i])
			++count;
	return count;
}

void CTransferableItem::Set_RegistrationListSize(int size)
{
	m_registrationListSize = size;
}

bool CTransferableItem::SetRegistrationItem(unsigned long uid)
{
	if (m_registrationList.size() >= m_registrationListSize)
		return false;

	TRADEITEM_LIST_IT it;

	for (it = m_transferableList.begin(); it != m_transferableList.end(); it++)
	{
		if ((*it).dwGuid == uid)
		{
			sTradeItem trade = *it;
			m_registrationList.push_back(trade);
			return true;
		}
	}

	return false;
}

bool CTransferableItem::SetRegistrationItem(unsigned long uid, int count)
{
	sTradeItem trade;
	memset(&trade, 0, sizeof(trade));
	if (m_registrationList.size() < m_registrationListSize &&
		IsTransferableItem(uid, trade, count))
	{
		trade.iNum = count;
		m_registrationList.push_back(trade);
		return true;
	}

	return false;
}

bool CTransferableItem::IsTransferableItem(unsigned long uid, sTradeItem& trade,
	int count)
{
	TRADEITEM_LIST_IT it;

	for (it = m_transferableList.begin(); it != m_transferableList.end(); it++)
	{
		if ((*it).dwGuid == uid)
		{
			int requested = static_cast<volatile int&>(count);
			if (requested)
			{
				if (Min((*it).iNum, requested) < requested)
					return false;
				(*it).iNum -= requested;
			}
			trade = *it;
			return true;
		}
	}

	return false;
}

bool CTransferableItem::IsRegisterationItem(unsigned long uid,
	sTradeItem& trade)
{
	TRADEITEM_LIST_IT it;

	for (it = m_registrationList.begin(); it != m_registrationList.end(); it++)
	{
		if ((*it).dwGuid == uid)
		{
			TRADEITEM_LIST_IT available;
			for (available = m_transferableList.begin();
				available != m_transferableList.end(); available++)
				if ((*available).dwGuid == uid)
				{
					(*available).iNum += (*it).iNum;
					break;
				}
			trade = *it;
			m_registrationList.erase(it);
			return true;
		}
	}

	return false;
}

bool CTransferableItem::IsRegisterationItem(unsigned long uid)
{
	TRADEITEM_LIST_IT it;
	for (it = m_registrationList.begin(); it != m_registrationList.end(); it++)
		if ((*it).dwGuid == uid)
			return true;
	return false;
}

std::list<sTradeItem>* CTransferableItem::GetList_RegistrationItem()
{
	return &m_registrationList;
}

void CTransferableItem::ResetRegistrationItemList()
{
	m_registrationList.clear();
	SetTransferableItemList();
}

int CTransferableItem::Get_RegistrationListSize()
{
	return m_registrationList.size();
}

bool CTransferableItem::DelRegistrationItem(unsigned long uid)
{
	if (m_registrationList.size())
	{
		sTradeItem trade;
		if (IsRegisterationItem(uid, trade))
		{
			m_transferableList.sort(CompareTradeItem);
			return true;
		}
	}

	return false;
}

std::list<sTradeItem>::iterator CTransferableItem::
	DelRegistrationItem_itor_return(unsigned long uid)
{
	if (!m_registrationList.size())
		return TRADEITEM_LIST_IT();

	TRADEITEM_LIST_IT it;

	for (it = m_registrationList.begin(); it != m_registrationList.end(); it++)
	{
		if ((*it).dwGuid == uid)
		{
			TRADEITEM_LIST_IT available;
			for (available = m_transferableList.begin();
				available != m_transferableList.end(); available++)
				if ((*available).dwGuid == uid)
				{
					(*available).iNum += (*it).iNum;
					break;
				}
			return m_registrationList.erase(it);
		}
	}

	return TRADEITEM_LIST_IT();
}

void CTransferableItem::SetMailInfoPage(sMailBoxListForPage& page)
{
	m_mailBoxPage.mailList.clear();
	m_mailBoxPage = page;
}

void CTransferableItem::SetMailInfo(sMailInfo& info)
{
	m_mailInfo.m_lMailItemList.clear();
	m_mailInfo = info;
}

sMailBoxListForPage* CTransferableItem::GetMailBoxPageList()
{
	return &m_mailBoxPage;
}

sMailInfo* CTransferableItem::GetMailInfo()
{
	return &m_mailInfo;
}

int CTransferableItem::GetNumTradingItem(unsigned long typeId)
{
	int count = 0;
	TRADEITEM_LIST_IT it;
	for (it = m_registrationList.begin(); it != m_registrationList.end(); it++)
		if ((*it).dwGuid == typeId)
			count += (*it).iNum;
	return count;
}

bool CTransferableItem::IsEquipParts(unsigned long charTypeId,
	unsigned long uid)
{
	bool result = false;
	std::list<unsigned long>::iterator it;

	for (it = m_equipCharList.begin(); it != m_equipCharList.end(); it++)
	{
		if (*it && uid == *it)
		{
			m_equipCharList.erase(it);
			result = true;
			break;
		}
	}

	return result;
}

bool CTransferableItem::IsCheckCardParts(unsigned long charTypeId,
	unsigned long typeId)
{
	std::list<sSCardAvilityPeriodInfo>::iterator it;
	for (it = Doc()->m_cardAbilityList[0].begin();
		it != Doc()->m_cardAbilityList[0].end(); it++)
		if ((*it).partsTid == typeId && (*it).partsUid == charTypeId)
			break;
	if (it == Doc()->m_cardAbilityList[0].end())
		return true;
	return false;
}
