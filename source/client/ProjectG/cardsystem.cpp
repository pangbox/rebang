#include "cardsystem.h"
#include "carddlg.h"
#include "itembuffactor.h"
#include "wind.h"
#include "projectg.h"

CCardManager::CCardManager()
{
	Init();

	for (int i = 0; i < 5; ++i)
		m_playerSlot[i] = 0;
	m_pCardBuffDlg = NULL;
	m_bSPCardAlarm = false;
	m_windColor = 0;
}

CCardManager::~CCardManager()
{
}

void CCardManager::Init()
{
	m_playerIndex = 0;

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			m_cardStatus[i][j] = 0;
			m_periodStatus[i][j] = 0;
		}

		m_powerRangeDown[i] = 0;

		m_periodComboGauge[i] = 0;
		m_periodSlot[i] = 0;
		m_periodPangRate[i] = 0;
		m_periodPangyaZone[i] = 0;
		m_specialClearBonusProb[i] = 0;

		for (int k = 0; k < 13; ++k)
		{
			m_caddieCard[i][k].effect = 0;
			m_caddieCard[i][k].value = 0;
			m_caddieCard[i][k].type = 0;
		}
	}
	m_specialClearBonusCourse = 20;
}

void CCardManager::OnProc(float delta)
{
}

void CCardManager::CalcCardPeriodAndStatus()
{
	Init();

	int slot = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (Doc()->m_gameMode == 1)
		{
			i = 0;

			if (++slot > 5)
				return;
		}
		else
			slot = i;

		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[i].begin();
			it != Doc()->m_cardAbilityList[i].end();)
		{
			if ((*it).cardType == 0 && (*it).partsTid != 0)

				SetCharacterCardAvility(*it, slot);

			else if ((*it).cardType == 1 && (*it).partsTid != 0)

				SetCaddieCardAvility(*it, slot);

			else if ((*it).cardType == 2)
			{
				int ret = SetSpecialCardAvility(*it, slot);

				if (ret == 0)
				{
					it = Doc()->m_cardAbilityList[i].erase(it);

					UpdateCardBuffDlg();
					continue;
				}
			}

			++it;
		}
	}
}

void CCardManager::SetCharacterCardAvility(const sSCardAvilityPeriodInfo& info,
	int slot)
{
	sCharacterInfo charInfo;
	if (slot == 0)
		charInfo = Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
	else
		charInfo = m_charInfo[slot];

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(info.tid);
	if (pCard == NULL)
		return;

	for (int i = 0; i < 24; ++i)
	{
		if (info.partsTid == charInfo.tidParts[i] &&
			info.partsUid == charInfo.ItemIdList[i])
		{
			for (int j = 0; j < 5; ++j)
				if (pCard->COM[j] > 0)
					m_cardStatus[slot][j] += pCard->COM[j];

			switch (pCard->Avility)
			{
			case 1:
				m_powerRangeDown[slot] += pCard->AvilityValue;
				break;
			}
		}
	}
}

int CCardManager::SetSpecialCardAvility(const sSCardAvilityPeriodInfo& info,
	int slot)
{
	_SYSTEMTIME endTime = info.useEndTime;

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(info.tid);
	if (pCard == NULL)
		return -1;

	int ret = ItemManager()->CompareSystemTime(endTime, Doc()->GetServerTime());

	if (Doc()->m_gameMode == 5 || Doc()->GetPlayingMode() == 2)
		ret = 1;

	if (ret <= 0)
		return ret;

	switch (pCard->Avility)
	{
	case 5:
		m_periodStatus[slot][0] += pCard->AvilityValue;
		break;

	case 6:
		m_periodStatus[slot][1] += pCard->AvilityValue;
		break;

	case 7:
		m_periodStatus[slot][2] += pCard->AvilityValue;
		break;

	case 8:
		m_periodStatus[slot][3] += pCard->AvilityValue;
		break;

	case 9:
		m_periodStatus[slot][4] += pCard->AvilityValue;
		break;

	case 10:
		m_periodComboGauge[slot] = pCard->AvilityValue;
		break;

	case 11:
		m_periodSlot[slot] = pCard->AvilityValue;
		break;

	case 2:
		m_periodPangRate[slot] = pCard->AvilityValue;
		break;

	case 12:
		m_periodPangyaZone[slot] = pCard->AvilityValue;
		break;
	case 13:
		m_specialClearBonusProb[slot] = pCard->AvilityValue * 0.01;
		m_specialClearBonusCourse = 2;
		break;
	case 14:
		m_specialClearBonusProb[slot] = pCard->AvilityValue * 0.01;
		m_specialClearBonusCourse = 3;
		break;
	case 15:
		m_specialClearBonusProb[slot] = pCard->AvilityValue * 0.01;
		m_specialClearBonusCourse = 11;
		break;
	case 16:
		m_specialClearBonusProb[slot] = pCard->AvilityValue * 0.01;
		m_specialClearBonusCourse = 6;
		break;
	}

	return ret;
}

void CCardManager::SetCaddieCardAvility(const sSCardAvilityPeriodInfo& info,
	int slot)
{
	sCharacterInfo charInfo;
	if (slot == 0)
		charInfo = Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
	else
		charInfo = m_charInfo[slot];

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(info.tid);
	if (pCard == NULL)
		return;

	for (int i = 0; i < 24; ++i)
	{
		if (info.partsTid == charInfo.tidParts[i] &&
			info.partsUid == charInfo.ItemIdList[i])
		{
			int effect = pCard->Avility;
			int index = effect - 1;
			m_caddieCard[slot][index].effect = effect;
			m_caddieCard[slot][index].type = pCard->RareType;

			if (m_caddieCard[slot][index].type <= pCard->RareType)
			{
				if (m_caddieCard[slot][index].value < pCard->AvilityValue)
					m_caddieCard[slot][index].value = pCard->AvilityValue;
			}
		}
	}
}

int CCardManager::GetTotalRange(bool bPowerRange)
{
	int range = (int)GetCardRangeUp();
	if (bPowerRange == true)
		range += (int)GetCardPowerRangeUp();
	return range - GetPowerRangeDown();
}

float CCardManager::GetTotalPangyaZone()
{
	float zone = CCardManager::Instance()->GetCardPangyaZoneUp();
	zone += CCardManager::Instance()->GetSpecialCardPangyaZoneUp();
	if (1.0f == Wind().GetGlobalIntensity())
		zone += CCardManager::Instance()->GetCaddieCardPangYaZoneUpWind1();
	return zone;
}

int CCardManager::GetPowerRangeDown()
{
	return m_powerRangeDown[m_playerIndex];
}

int CCardManager::GetCardPeriodStatus(int index)
{
	int status = 0;
	if (m_periodStatus[m_playerIndex][index] > 0)
		status = m_periodStatus[m_playerIndex][index];
	return status;
}

int CCardManager::GetCardStatusSlot(int index)
{
	int status = 0;
	if (m_cardStatus[m_playerIndex][index] > 0)
		status = m_cardStatus[m_playerIndex][index];
	return status;
}

float CCardManager::GetCardPeriodComboGauge()
{
	return m_periodComboGauge[m_playerIndex];
}

int CCardManager::GetCardPeriodSlot()
{
	return m_periodSlot[m_playerIndex];
}

float CCardManager::GetCardPeriodPangRate()
{
	return m_periodPangRate[m_playerIndex];
}

int CCardManager::GetCardSuccesssProbUp()
{
	return m_caddieCard[m_playerIndex][0].value;
}

float CCardManager::GetCardRangeUp()
{
	return m_caddieCard[m_playerIndex][1].value;
}

__int64 CCardManager::GetCardBounceBonusUp()
{
	return m_caddieCard[m_playerIndex][3].value;
}

float CCardManager::GetCardPowerRangeUp()
{
	return m_caddieCard[m_playerIndex][4].value;
}

int CCardManager::GetCardComboGaugeUp()
{
	return m_caddieCard[m_playerIndex][5].value;
}

float CCardManager::GetCardPangyaZoneUp()
{
	return m_caddieCard[m_playerIndex][6].value * 0.5f;
}

unsigned char CCardManager::GetWindPowerDown()
{
	return m_caddieCard[m_playerIndex][2].value;
}

float CCardManager::GetSpecialCardPangyaZoneUp()
{
	return m_periodPangyaZone[m_playerIndex] * 0.5f;
}

float CCardManager::GetSpecialClearBonusProb(unsigned char course)
{
	if (course > 19 || course != m_specialClearBonusCourse)

		return 0.0f;

	return m_specialClearBonusProb[m_playerIndex];
}

float CCardManager::GetCaddieCardPangYaZoneUpWind1()
{
	return m_caddieCard[m_playerIndex][10].value * 0.5f;
}

void CCardManager::UpdateCardBuffDlg()
{
	if (m_pCardBuffDlg)

		CItemBuff::UpdateBuffDlg(m_pCardBuffDlg);
}

void CCardManager::InitCardBuffDlgPosition()
{
	_WPOINT pos = { -1.0f, 0.0f };
	FrSPCardBuffDlg::SetPosition(*(WPoint*)&pos);
	m_bSPCardAlarm = false;
}

void CCardManager::ResetSpecialBuffTick()
{
	FrSPCardBuffDlg::SetSpecialBuffStartTick(g_CurrentTime);
}

void CCardManager::SetPlayerIndex(unsigned char index)
{
	if (index == 0xff)
		m_playerIndex = 0;
	else if (Doc()->m_gameMode == 5)
		CCardManager::Instance()->m_playerIndex = index;
	else if (Doc()->GetPlayingMode() == 2)
		CCardManager::Instance()->m_playerIndex = 2;
	else if (Doc()->m_gameMode == 1)
		CCardManager::Instance()->m_playerIndex = index + 1;
	else
		m_playerIndex = m_playerSlot[index];
}

void CCardManager::SetOffLinePlayerIndex(unsigned char index)
{
	m_playerIndex = index;
}
