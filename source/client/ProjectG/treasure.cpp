#include "stdafx.h"
class CTMapInfo;
typedef std::vector<CTMapInfo*>::iterator TMAP_IT;
#include "treasure.h"
#include "treasuremap.h"

CTMapInfo::CTMapInfo()
{
	m_TMapIndex = 0;
	m_TMark = 2;
	m_gauge = 0;
}

CTMapInfo::CTMapInfo(const CTMapInfo& rhs)
{
	*this = rhs;
}

CTMapInfo::~CTMapInfo()
{
}

unsigned char CTMapInfo::GetTMapIndex() const
{
	return m_TMapIndex;
}

void CTMapInfo::SetTMapIndex(unsigned char index)
{
	m_TMapIndex = index;
}

unsigned char CTMapInfo::GetTMark() const
{
	return m_TMark;
}

void CTMapInfo::SetTMark(unsigned char mark)
{
	m_TMark = mark;
}

unsigned long CTMapInfo::GetGauge() const
{
	return m_gauge;
}

void CTMapInfo::SetGauge(unsigned long gauge)
{
	m_gauge = gauge;
}

CTMapInfo& CTMapInfo::operator=(const CTMapInfo& rhs)
{
	m_TMapIndex = rhs.m_TMapIndex;
	m_TMark = rhs.m_TMark;
	m_gauge = rhs.m_gauge;
	return *this;
}

CTHunter::CTHunter()
{
	m_treasurePoint = 0;
	m_bReturnLobby = false;
	m_bReceiveGiftPacket = false;
	m_bUsedTikiReport = false;
	m_giftReserveSize = 24;
}

CTHunter::~CTHunter()
{
	TMAP_IT it;
	for (it = m_vecTMap.begin(); it != m_vecTMap.end(); it++)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_vecTMap.clear();
}

void CTHunter::Initialize(int count)
{
	for (int i = 0; i < count; ++i)
		AddTreasure(i, 0);
	m_vecGift.reserve(m_giftReserveSize);
}

void CTHunter::AddTreasure(unsigned char index, unsigned long gauge)
{
	CTMapInfo* pInfo = new CTMapInfo;
	pInfo->SetTMapIndex(index);
	pInfo->SetGauge(gauge);
	m_vecTMap.push_back(pInfo);
}

bool CTHunter::DeleteTreasure(unsigned char index)
{
	std::vector<CTMapInfo*>::iterator it;
	for (it = m_vecTMap.begin(); it != m_vecTMap.end(); it++)
	{
		if (index == (*it)->GetTMapIndex())
		{
			delete *it;
			m_vecTMap.erase(it);
			return true;
		}
	}
	return false;
}

unsigned char CTHunter::FindTreasure(unsigned char index)
{
	std::vector<CTMapInfo*>::iterator it =
		std::find_if(m_vecTMap.begin(), m_vecTMap.end(), THunterFinder(index));
	return (*it)->GetTMapIndex();
}

void CTHunter::SetTHunterGauge(unsigned char index, unsigned long gauge)
{
	if (m_vecTMap.size() > index)
		m_vecTMap[index]->SetGauge(gauge);
}

unsigned long CTHunter::GetTHunterGauge(eMapType mapType)
{
	int index = CMapTypeAdapter::GetMapIndex(mapType);
	if (index == -1)
		return 9999;
	return m_vecTMap[index]->GetGauge();
}

unsigned char CTHunter::GetTHunterMark(eMapType mapType)
{
	int index = CMapTypeAdapter::GetMapIndex(mapType);
	if (index == -1)
		return 2;
	return m_vecTMap[index]->GetTMark();
}

unsigned char CTHunter::GetTreasureMapSize()
{
	unsigned char count = m_vecTMap.size();
	return count;
}

void CTHunter::SetTreasurePoint(unsigned long point)
{
	if (point >= 1000)
		m_treasurePoint = 1000;
	else
		m_treasurePoint = point;
}

unsigned long CTHunter::GetTreasurePoint() const
{
	return m_treasurePoint;
}

void CTHunter::ResetTGift()
{
	m_vecGift.clear();
}

void CTHunter::DecideTMark()
{
	unsigned long gauge = m_vecTMap[0]->GetGauge();
	int count = m_vecTMap.size();
	std::vector<unsigned char> indices;
	for (int i = 1; i < count; ++i)
		if (max(m_vecTMap[i]->GetGauge(), gauge))
			gauge = m_vecTMap[i]->GetGauge();
	for (int i = 0; i < count; ++i)
		if (m_vecTMap[i]->GetGauge() == gauge)
			indices.push_back(i);
	for (unsigned int i = 0; i < indices.size(); ++i)
		m_vecTMap[indices[i]]->SetTMark(1);
	indices.clear();
	gauge = m_vecTMap[0]->GetGauge();
	for (int i = 1; i < count; ++i)
		if (min(m_vecTMap[i]->GetGauge(), gauge))
			gauge = m_vecTMap[i]->GetGauge();
	for (int i = 0; i < count; ++i)
		if (m_vecTMap[i]->GetGauge() == gauge)
			indices.push_back(i);
	for (unsigned int i = 0; i < indices.size(); ++i)
		m_vecTMap[indices[i]]->SetTMark(0);
}

void CTHunter::ReceiveAllGift(std::vector<sTreasureGift>& gifts)
{
	if (!m_vecGift.empty())
		m_vecGift.clear();
	m_vecGift.swap(gifts);
}

void CTHunter::UpdateItemList(std::string str)
{
	int count = m_vecGift.size();
	for (int i = 0; i < count; ++i)
	{
		sTreasureGift gift;
		gift.Reset();
		gift = m_vecGift[i];
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		switch (gift.dwTid >> 26)
		{
		case 28:
		{
			ITEM_LIST_IT it;
			for (it = Doc()->m_auxPartList.begin();
				it != Doc()->m_auxPartList.end(); it++)
			{
				if ((*it).guid == gift.dwItemGuid)
				{
					(*it).Common[0] += gift.wCount;
					break;
				}
			}
			if (it == Doc()->m_auxPartList.end())
			{
				item.tid = gift.dwTid;
				item.guid = gift.dwItemGuid;
				item.Common[0] = 1;
				if (gift.wCount >= 1)
					item.Common[0] = gift.wCount;
				item.gift_flag = 0;
				item.Expired = 0;
				item.ItemType = gift.byItemType;
				memset(&item.ItemDate, 0, sizeof(item.ItemDate));
				item.Purchase = 0;
				item.IsValid = 1;
				Doc()->m_auxPartList.push_back(item);
			}
			break;
		}
		case 6:
		{
			if (gift.dwTid == 436207632)
			{
				Doc()->m_myInfo.stat.i64Pang += gift.wCount;
				break;
			}
			ITEM_LIST_IT it;
			for (it = Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); it++)
			{
				if ((*it).guid == gift.dwItemGuid)
				{
					(*it).Common[0] += gift.wCount;
					break;
				}
			}
			if (it == Doc()->m_myItemList.end())
			{
				item.tid = gift.dwTid;
				item.guid = gift.dwItemGuid;
				item.Common[0] = 1;
				if (gift.wCount >= 1)
					item.Common[0] = gift.wCount;
				item.gift_flag = 0;
				item.Expired = 0;
				item.ItemType = gift.byItemType;
				item.HourRemain = gift.iHourRemain;
				memset(&item.ItemDate, 0, sizeof(item.ItemDate));
				item.Purchase = 0;
				item.IsValid = 1;
				Doc()->m_myItemList.push_back(item);
			}
			break;
		}
		case 31:
			Doc()->AddCard(gift.dwTid, gift.dwItemGuid, gift.wCount);
			break;
		}
	}
}

void CTHunter::ReceiveRepository(std::vector<sTreasureHunt>& repository)
{
	if (!m_vecRepository.empty())
		m_vecRepository.clear();
	m_vecRepository.swap(repository);
}

sTreasureHunt CTHunter::GetGiftInfo(unsigned char index)
{
	return m_vecRepository[index];
}

void CTHunter::ReAdjustRepository()
{
	std::map<unsigned long, sTreasureHunt> items;
	sTreasureHunt item;
	std::map<unsigned long, sTreasureHunt>::iterator found;
	for (std::vector<sTreasureHunt>::iterator it = m_vecRepository.begin();
		it != m_vecRepository.end(); ++it)
	{
		item = *it;
		found = items.find(item.dwTid);
		if (found == items.end())
			items.insert(std::make_pair(item.dwTid, item));
		else
			(*found).second.wCount += item.wCount;
	}
	unsigned int size = items.size();
	m_vecRepository.clear();
	m_vecRepository.reserve(size);
	std::map<unsigned long, sTreasureHunt>::iterator itMap;
	for (itMap = items.begin(); itMap != items.end(); ++itMap)
		m_vecRepository.push_back((*itMap).second);
	items.clear();
}
