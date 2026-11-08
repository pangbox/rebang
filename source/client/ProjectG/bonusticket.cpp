#include "stdafx.h"
#include "bonusticket.h"
#include <stdarg.h>

struct CBonusTicket::sTicketTable
{
	unsigned char ticket[100];
	int index;

	sTicketTable()
	{
		index = 0;
		memset(ticket, 0, sizeof(ticket));
	}

	int GetTicket()
	{
		int result = ticket[index++];
		if (index >= 100)
			index = 0;
		return result;
	}
};

CBonusTicket& CBonusTicket::Instance()
{
	static CBonusTicket instance;
	return instance;
}

CBonusTicket::CBonusTicket()
{
}

CBonusTicket::~CBonusTicket()
{
	Release();
}

void CBonusTicket::Release()
{
	std::map<std::string, sTicketTable*>::iterator it = m_ticketTable.begin();
	while (it != m_ticketTable.end())
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
			m_ticketTable.erase(it++);
		}
		else
			++it;
	}
	m_ticketTable.clear();
}

void CBonusTicket::DeleteTable(std::string name)
{
	std::map<std::string, sTicketTable*>::iterator it =
		m_ticketTable.find(name);
	if (it != m_ticketTable.end() && it->second)
	{
		delete it->second;
		it->second = NULL;
		m_ticketTable.erase(it);
	}
}

void CBonusTicket::SetTicketFromServer(std::string name, unsigned char* ticket,
	unsigned int size)
{
	if (size != 100)
		return;
	DeleteTable(name);
	sTicketTable* table = new sTicketTable;
	memcpy(table->ticket, ticket, 100);
	m_ticketTable[name] = table;
}

void __cdecl CBonusTicket::SetTicketFromClient(std::string name, int num, ...)
{
	DeleteTable(name);
	std::vector<int> rates;
	rates.reserve(num);
	int total = 0;
	va_list args;
	va_start(args, num);
	for (int i = 0; i < num; ++i)
	{
		int rate = va_arg(args, int);
		total += rate;
		rates.push_back(rate);
	}
	va_end(args);
	if (total > 100)
		rates.clear();
	sTicketTable* table = new sTicketTable;
	SetRandomTicket(*table, rates);
	m_ticketTable[name] = table;
}

void CBonusTicket::SetRandomTicket(sTicketTable& table, std::vector<int>& rate)
{
	int index = 0;
	int value = 0;
	for (std::vector<int>::iterator it = rate.begin(); it != rate.end(); ++it)
	{
		int count = *it;
		for (int i = 0; i < count; ++i)
			table.ticket[index++] = value;
		++value;
	}
	for (int i = 0; i < 100; ++i)
	{
		int other = rand() % 100;
		std::swap(table.ticket[i], table.ticket[other]);
	}
}

int CBonusTicket::GetRandomTicket(std::string name)
{
	std::map<std::string, sTicketTable*>::iterator it =
		m_ticketTable.find(name);
	if (it != m_ticketTable.end())
		return it->second->GetTicket();
	return 0;
}
