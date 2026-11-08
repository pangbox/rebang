#include "rankunit.h"
#include "networksystem.h"
#include "projectg.h"

RankUnit::RankUnit()
{
}

RankUnit::~RankUnit()
{
}

bool RankUnit::Init()
{
	NetworkUnit::Init();
	return true;
}

void RankUnit::ShutDown()
{
	Init();
}

bool RankUnit::OnLine()
{
	WReceivedPacket packet;

	if (m_socket.GetPacket(packet) == 3)
	{
		packet.Reset();

		CProjectG::Instance()->OnPacket(packet);
	}

	return true;
}

bool RankUnit::OffLine()
{
	return true;
}

bool RankUnit::Connect()
{
	if (m_socket.m_bConnected)
	{
		setForceTransition(0);
	}
	else
	{
		m_socket.SetEventMsg(WM_USER + 14);

		if (m_socket.Connect(inet_addr(Doc()->m_rankServerAddr.c_str()),
				htons((u_short)Doc()->m_rankServerPort)) == 0)
		{
			setForceTransition(1);
			m_bConnectionComplete = TRUE;

			return false;
		}

		m_connectTime = GetTickCount();
		setTransition(0);
	}

	return true;
}

bool RankUnit::Connecting()

{
	if (GetTickCount() - m_connectTime > 3000 || !m_socket.TryRead())
	{
		setForceTransition(1);
		m_socket.Close();
		m_bConnectionComplete = TRUE;
		return false;
	}

	WReceivedPacket packet;
	if (m_socket.GetPacket(packet) == 3)
	{
		if (OnConnect(packet))
		{
			m_socket.AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
			setForceTransition(0);
			m_bConnectionComplete = TRUE;
			return true;
		}

		return false;
	}

	return true;
}

bool RankUnit::Disconnecting()
{
	if (!m_socket.m_bConnected)
	{
		setForceTransition(1);
	}
	else
	{
		m_socket.Close();
	}
	return true;
}

int RankUnit::OnConnect(WReceivedPacket& packet)
{
	packet.Decode1();
	if (packet.Decode2() != 500)
	{
		return FALSE;
	}

	m_socket.SetParseKey(packet.Decode4());

	unsigned char result = packet.Decode1();
	if (result != 5)
	{
		return FALSE;
	}

	std::string version = packet.DecodeStr();

	return TRUE;
}
