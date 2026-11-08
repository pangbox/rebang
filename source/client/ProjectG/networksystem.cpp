#include "networksystem.h"
#include "gameunit.h"
#include "msnunit.h"
#include "loginunit.h"
#include "rankunit.h"
#include "packet.h"

WNetworkSystem::WNetworkSystem()
{
	m_pUnit[NET_GAME] = new GameUnit;
	m_pUnit[NET_MSN] = new MSNUnit;

	m_pUnit[NET_LOGIN] = new LoginUnit;
	m_pUnit[NET_RANK] = new RankUnit;

	memset(m_bParseKey, 0, sizeof(m_bParseKey));
	m_connectionType = 0;
}

WNetworkSystem::~WNetworkSystem()
{
	for (int i = 0; i < NET_MAX; i++)
		delete m_pUnit[i];
}

int WNetworkSystem::Init(eNetUnit unit)
{
	if (unit == NET_MAX)
	{
		for (int i = 0; i < NET_MAX; i++)
		{
			m_pUnit[i]->Init();
		}
	}
	else
		m_pUnit[unit]->Init();

	return TRUE;
}

int WNetworkSystem::Process()
{
	for (int i = 0; i < NET_MAX; i++)
	{
		m_pUnit[i]->Process();
	}

	return TRUE;
}

int WNetworkSystem::SendMessage(eNetUnit unit, NetworkUnit::eEvent event)
{
	m_pUnit[unit]->setTransition(event);
	return TRUE;
}

void WNetworkSystem::ForceShutDown(eNetUnit unit)
{
	m_pUnit[unit]->ShutDown();
}

void WNetworkSystem::WriteSendTime()
{
	m_sendTime = GetTickCount();
}

int WNetworkSystem::IsConnected(eNetUnit unit)
{
	return m_pUnit[unit]->IsConnected();
}

int WNetworkSystem::IsConnectionComplete(eNetUnit unit)
{
	return m_pUnit[unit]->IsConnectionComplete();
}

void WNetworkSystem::SendTTL()
{
	unsigned long time = GetTickCount();

	if (time - m_sendTime > 5000)
	{
		WSendPacket packet((enumClientPacket)1);
		packet.Send(TO_GAME);
	}
}
