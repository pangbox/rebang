#include "golfdoc.h"
#include "bonusticket.h"
#include "caddiebonus.h"

CCaddieBonus::CCaddieBonus()
{
	RegisterBonus(W_BIGBONGDARI, "W_BIGBONGDARI");
	RegisterBonus(R_BIGBONGDARI, "R_BIGBONGDARI");
}

void CCaddieBonus::SetTicket()
{
	switch (Doc()->m_golfGame.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		CBonusTicket::Instance().SetTicketFromClient("W_BIGBONGDARI", 4, 50, 30,
			15, 5);
		CBonusTicket::Instance().SetTicketFromClient("R_BIGBONGDARI", 4, 49, 20,
			30, 1);
		break;
	}
}

__int64 CCaddieBonus::GetBonusPang(eCaddie caddie, __int64 pang)
{
	if (!IsHaveCaddie(caddie))
		return 0;

	__int64 bonus = 0;
	std::map<int, std::string>::iterator it = m_bonus.find(caddie);
	if (it == m_bonus.end())
	{
		return 0;
	}

	int ticket = CBonusTicket::Instance().GetRandomTicket(it->second);

	switch (caddie)
	{
	case W_BIGBONGDARI:
		bonus = GetWBigPontaBonus(ticket);
		break;
	case R_BIGBONGDARI:
		bonus = GetRBigPontaBonus(ticket, pang);
		break;
	}

	return bonus;
}

CCaddieBonus::~CCaddieBonus()
{
	Release();
}

bool CCaddieBonus::IsHaveCaddie(eCaddie caddie)
{
	if ((caddie | 0x1c000000) ==
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.tid)
		return true;

	return false;
}

void CCaddieBonus::Release()
{
	std::map<int, std::string>::iterator it = m_bonus.begin();
	while (it != m_bonus.end())
	{
		std::string name = (*it).second;
		if (name != "")
			CBonusTicket::Instance().DeleteTable((*it).second);
		it++;
	}
	m_bonus.clear();
}

void CCaddieBonus::RegisterBonus(int caddie, std::string name)
{
	m_bonus[caddie] = name;
}

__int64 CCaddieBonus::GetWBigPontaBonus(int ticket)
{
	__int64 bonus = 0;
	switch (ticket)
	{
	case 0:
		bonus = 1;
		break;
	case 1:
		bonus = 5;
		break;
	case 2:
		bonus = 15;
		break;
	case 3:
		bonus = 50;
		break;
	}
	return bonus;
}

__int64 CCaddieBonus::GetRBigPontaBonus(int ticket, __int64 pang)
{
	__int64 bonus = 0;
	switch (ticket)
	{
	case 0:
		bonus = pang / 100;
		break;
	case 1:
		bonus = pang * 10 / 100;
		break;
	case 2:
		bonus = pang * 20 / 100;
		break;
	case 3:
		bonus = pang * 50 / 100;
		break;
	}

	if (bonus < 1)
		return 1;

	return bonus;
}
