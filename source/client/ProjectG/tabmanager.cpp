#include "tabmanager.h"
#include <stdarg.h>

CTabManager::CTabManager()
{
}

CTabManager::~CTabManager()
{
}

void CTabManager::Add(int id, ...)
{
	sTabElement* tab = Find(id);
	if (!tab)
	{
		sTabElement element;
		element.id = id;
		m_tabs.push_back(element);
		tab = Find(id);
	}

	va_list args;
	va_start(args, id);
	FrWnd** window = va_arg(args, FrWnd**);
	while (window)
	{
		tab->windows.push_back(window);
		window = va_arg(args, FrWnd**);
	}
	va_end(args);
}

sTabElement* CTabManager::Find(int id)
{
	for (unsigned int i = 0; i < m_tabs.size(); ++i)
	{
		if (m_tabs[i].id == id)
			return &m_tabs[i];
	}
	return NULL;
}

void CTabManager::Open(int id)
{
	for (unsigned int i = 0; i < m_tabs.size(); ++i)
	{
		if (m_tabs[i].id != id)
		{
			for (unsigned int j = 0; j < m_tabs[i].windows.size(); ++j)
			{
				FrWnd** window = m_tabs[i].windows[j];
				if (window && *window)
					(*window)->SetVisible(false);
			}
		}
	}

	sTabElement* tab = Find(id);
	if (tab)
	{
		for (unsigned int i = 0; i < tab->windows.size(); ++i)
		{
			FrWnd** window = tab->windows[i];
			if (window && *window)
				(*window)->SetVisible(true);
		}
	}
}
