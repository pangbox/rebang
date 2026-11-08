#include "wtimer.h"
#include <vector>

CTimerManager::CTimerManager()
{
	for (int i = 0; i < 10; ++i)
		m_timers[i].Clear();
}

CTimerManager::~CTimerManager()
{
	m_timerList.clear();
}

bool CTimerManager::AddTimer(float time, const char* name, CTimerEvent* event)
{
	for (int i = 0; i < 10; ++i)
	{
		if (m_timers[i].active == true)
			continue;

		m_timers[i].active = true;
		m_timers[i].name = name;
		m_timers[i].event = event;
		m_timers[i].time = time;

		m_timerList.push_back(&m_timers[i]);
		return true;
	}
	return false;
}

void CTimerManager::AllClear()
{
	m_timerList.clear();
	for (int i = 0; i < 10; ++i)
		m_timers[i].Clear();
}

void CTimerManager::ClearTimer(const char* name)
{
	for (std::list<sTimer*>::iterator it = m_timerList.begin();
		it != m_timerList.end(); ++it)
	{
		sTimer* timer = *it;
		if (strcmp(timer->name.c_str(), name) == 0)
		{
			timer->Clear();
			m_timerList.erase(it);
			break;
		}
	}
}

void CTimerManager::UpdateTimer(float dt)
{
	std::vector<sTimer*> timers;
	timers.assign(m_timerList.begin(), m_timerList.end());

	m_timerList.clear();

	int count = timers.size();
	for (int i = 0; i < count; ++i)
	{
		sTimer* timer = timers[i];
		timer->elapsed += dt;
		if (timer->time - timer->elapsed < 0.0f)
		{
			CTimerEvent* event = timer->event;
			timer->Clear();
			event->OnTimer();
		}
		else
		{
			m_timerList.push_back(timer);
		}
	}
}

CTimerEvent::CTimerEvent()
{
}

CTimerEvent::~CTimerEvent()
{
	if (!m_timerName.empty())
		ClearTimer(m_timerName.c_str());
}

void CTimerEvent::ClearTimer(const char* name)
{
	CTimerManager::Instance()->ClearTimer(name);
}

bool CTimerEvent::AddTimer(float time, const char* name)
{
	if (CTimerManager::Instance()->AddTimer(time, name, this) == true)
	{
		m_timerName = name;
		return true;
	}
	return false;
}
