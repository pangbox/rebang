#include "club.h"
#include "golfdoc.h"
#include "cardsystem.h"
#include "wind.h"
#include "../../shared/localize.h"
#include "powergauge.h"

const float ELF_EAR_AREA = 1.0f;
const float LUCKY_PANGYA_AREA = 2.0f;
const float AIRNIGHT_LUCKY_AREA = 5.0f;
const float WING_AREA = 0.5f;

const int WING_PARTS_POS[10] = { 11, 12, 12, 14, 12, 16, 8, 17, 10, 10 };
const unsigned long WING_PARTS[10][6] = {
	{ 0x08016805, 0x08016806, 0x08016803, 0x08016802, 0xffffffff, 0xffffffff },
	{ 0x08058805, 0x08058806, 0x08058803, 0x08058807, 0x08058808, 0x08058802 },
	{ 0x08098805, 0x08098806, 0x08098803, 0x08098802, 0xffffffff, 0xffffffff },
	{ 0x080dc805, 0x080dc806, 0x080dc803, 0x080dc807, 0x080dc808, 0x080dc802 },
	{ 0x08118805, 0x08118806, 0x08118803, 0x08118802, 0xffffffff, 0xffffffff },
	{ 0x08160805, 0x08160806, 0x08160803, 0x08160807, 0x08160808, 0x08160802 },
	{ 0x08190805, 0x08190806, 0x08190803, 0x08190807, 0x08190808, 0x08190802 },
	{ 0x081e2805, 0x081e2806, 0x081e2807, 0x081e2802, 0xffffffff, 0xffffffff },
	{ 0x08214805, 0x08214806, 0x08214807, 0x08214808, 0x08214809, 0x08214802 },
	{ 0x08254802, 0x08254803, 0x08254804, 0x08254805, 0x08254806, 0x08254807 },
};

namespace
{
	sEarPartsPos aEarParts[10] = { { 0x11 }, { 0x11 }, { 0x13 }, { 0x11 },
		{ 0x14 }, { 0x15 }, { 0x10 }, { 0x13 }, { 0x11 }, { 0x11 } };
}

bool CPowerGauge::Init()
{
	m_luckyItemList.push_back(0x1800000a);
	m_luckyItemList.push_back(0x18000007);
	m_luckyItemList.push_back(0x18000002);
	m_luckyItemList.push_back(0x18000012);

	return true;
}

CPowerGauge::CPowerGauge()
	: m_debugPangYaArea(1.0f), m_debugAirNightLuckyPangYaArea(0.0f)
{
	Init();
}

CPowerGauge::~CPowerGauge()
{
	m_luckyItemList.clear();
}

void CPowerGauge::SetDebugPangYaArea(float area)
{
	m_debugPangYaArea = area;
}

void CPowerGauge::SetDebugAirNightLuckyPangYaArea(float area)
{
	m_debugAirNightLuckyPangYaArea = area;
}

float CPowerGauge::GetPangYaArea()
{
	float area = LUCKY_PANGYA_AREA;

	area += GetAddLuckyPangYaArea();
	area += GetAddElfEarArea();
	area += GetAddWingArea();
	area += GetAddAirnightLuckyArea();

	float cardArea = GetAddCardArea();

	area = (area + cardArea) + GetLuckyNecklaceArea();

	area += GetAddAuxPartArea();

	return area;
}

float CPowerGauge::GetAddLuckyPangYaArea()
{
	unsigned char cur = GOLFDOC()->m_currentPlayer;
	unsigned long item = GOLFDOC()->GetPlayer(cur)->item;
	std::list<unsigned long>::iterator it =
		std::find(m_luckyItemList.begin(), m_luckyItemList.end(), item);
	if (it != m_luckyItemList.end())
		return LUCKY_PANGYA_AREA;

	return 0.0f;
}

float CPowerGauge::GetAddElfEarArea()
{
	if (!IsLocalContent(S3_ELFEAR))
		return 0.0f;

	float intensity;

	sCharacterInfo& charInfo =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo;

	unsigned long index = charInfo.tid & 0x3ffffff;

	unsigned long part = charInfo.tidParts[aEarParts[index].pos];
	if (part)
	{
		switch (part & 0x1ff)
		{
		case 0:
		{
			intensity = Wind().GetGlobalIntensity();
			if (intensity < 4.0f)
				return 0.0f;
			else if (intensity >= 4.0f && intensity <= 7.0f)
				return ELF_EAR_AREA;
			else
				return LUCKY_PANGYA_AREA;
		}

		case 1:
		case 2:
		case 3:
		case 4:
		case 7:
		case 12:
			intensity = Wind().GetGlobalIntensity();
			if (intensity >= 7.0f)
				return ELF_EAR_AREA;
			break;
		}
	}

	return 0.0f;
}

float CPowerGauge::GetAddAirnightLuckyArea()
{
	if (GolfClub().GetType() == 3)
	{
		return 0.0f;
	}

	if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid ==
		0x10000012)
	{
		return AIRNIGHT_LUCKY_AREA;
	}

	return 0.0f;
}

float CPowerGauge::GetAddWingArea()
{
	float area = 0.0f;

	sCharacterInfo& charInfo =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo;

	unsigned long index = charInfo.tid & 0x3ffffff;

	for (int i = 0; i < 6; i++)
	{
		if (charInfo.tidParts[WING_PARTS_POS[index]] == WING_PARTS[index][i])
		{
			area = WING_AREA;
			break;
		}
	}

	return area;
}

float CPowerGauge::GetAddCardArea()
{
	if (GolfClub().GetType() == 3)
	{
		return 0.0f;
	}
	else
	{
		float area = CCardManager::Instance()->GetTotalPangyaZone();
		return area;
	}
}

float CPowerGauge::GetLuckyNecklaceArea()
{
	IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(0x1a000006);
	unsigned char level =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].stat.Level;
	if (pItem && level <= pItem->c.Level)
	{
		if (level < 11)
		{
			return ELF_EAR_AREA;
		}
	}

	return 0.0f;
}

bool CPowerGauge::IsEquipSpecialAuxPart(unsigned long typeId)
{
	sCharacterInfo& charInfo =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo;

	if (charInfo.tidAuxParts[(typeId >> 21) & 0x1f] == typeId)
		return true;
	return false;
}

float CPowerGauge::GetAddAuxPartArea()
{
	float area = 0.0f;

	if (IsEquipSpecialAuxPart(0x70010012))
	{
		float intensity = Wind().GetGlobalIntensity();
		if (intensity > 0.0f && intensity < 4.0f)
			area = WING_AREA;
	}

	return area;
}
