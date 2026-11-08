#include "msnunit.h"
#include "networksystem.h"
#include "projectg.h"
#include "messengerdlg.h"

MSNUnit::MSNUnit()
{
	m_connectTryTime = 0;
	m_serverListRequestTime = 0;
}

MSNUnit::~MSNUnit()
{
}

bool MSNUnit::Init()
{
	m_bConnectTried = false;
	MESSENGER()->SetComplete((eConnectStatus)0);
	WNetworkSystem::Instance()->SetParseKey(WNetworkSystem::NET_MSN, false);
	NetworkUnit::Init();
	return true;
}

void MSNUnit::ShutDown()
{
	m_socket.Close();
	Init();
	MESSENGER()->m_serverList.clear();
}

bool MSNUnit::OnLine()
{
	WReceivedPacket packet;

	if (m_socket.GetPacket(packet) == 3)
	{
		CProjectG::Instance()->OnPacket(packet);
	}

	return true;
}

bool MSNUnit::OffLine()
{
	if (CProjectG::Instance()->m_bMsnOffline != true)
	{
		m_connectTryTime = 0;
		return false;
	}

	if (NET()->IsConnected(WNetworkSystem::NET_GAME) &&
		NET()->IsConnectionComplete(WNetworkSystem::NET_GAME))
	{
		if (!IsConnectionComplete() ||
			GetTickCount() - m_connectTryTime > 60000)
		{
			Init();
			setTransition(0);
		}
	}
	return true;
}

bool MSNUnit::Connect()
{
	if (m_socket.m_bConnected)
	{
		setForceTransition(0);
		m_bConnectionComplete = TRUE;
		return true;
	}

	if (!m_bConnectTried)
		ConnectToMSNServer();

	return true;
}

bool MSNUnit::Connecting()
{
	if (m_bConnectTried)
	{
		m_connectTime = GetTickCount();
		m_bConnectTried = false;
	}

	if (GetTickCount() - m_connectTime > 10000 || !m_socket.TryRead())
	{
		setForceTransition(5);
		m_socket.Close();
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
		}
		else
			return false;
	}

	return true;
}

int MSNUnit::OnConnect(WReceivedPacket& packet)
{
	packet.Decode1();

	if (packet.Decode2() != 44)
		return FALSE;

	MESSENGER()->SetComplete((eConnectStatus)1);

	packet.Decode1();
	packet.Decode1();
	m_socket.SetParseKey(packet.Decode4());

	NET()->SetParseKey(WNetworkSystem::NET_MSN, true);

	std::string id = MyId();
	std::string password = Doc()->m_password;

	WSendPacket send(18);
	send.Encode4(MyUID());
	send.EncodeStr(MyNick());
	send.Send(TO_MSN);

	return TRUE;
}

bool MSNUnit::Disconnecting()
{
	if (!m_socket.m_bConnected)
		setForceTransition(1);
	else
		m_socket.Close();
	return true;
}

void MSNUnit::ConnectToMSNServer()
{
	std::string ip = "";
	int port = 0;

	if (!GetConnectServerInfo(&ip, &port))
	{
		RequestMSNServerList();
		setForceTransition(1);
		m_bConnectionComplete = TRUE;
		return;
	}

	m_socket.SetEventMsg(WM_USER + 12);
	m_socket.AsyncSelect(FD_CONNECT);

	hostent* host = gethostbyname(ip.c_str());
	char szIP[21] = "\0";
	wsprintf(szIP, "%s", inet_ntoa(*(in_addr*)host->h_addr_list[0]));
	m_socket.Connect(inet_addr(szIP), htons(port));

	m_bConnectTried = true;
	m_connectTryTime = GetTickCount();
}

bool MSNUnit::GetConnectServerInfo(std::string* ip, int* port)
{
	int minUser = 100000;
	if ((int)MESSENGER()->m_serverList.size() < 1)
		return false;

	int selected = -1;
	int index = 0;
	std::list<MSNServerInfo>::iterator it;
	for (it = MESSENGER()->m_serverList.begin();
		it != MESSENGER()->m_serverList.end(); ++it, ++index)
	{
		if ((*it).bInvalid)
			continue;

		sGameServerInfo info = *it;
		if (info.curUser >= info.maxUser - 150 && rand() % 3)
		{
			(*it).bInvalid = true;
		}
		else if (minUser > info.curUser)
		{
			*ip = std::string(info.addr);
			*port = info.port;
			minUser = info.curUser;
			selected = index;
		}
	}

	if (selected == -1)
		return false;

	MESSENGER()->SetCurrentServerIndex(selected);
	return true;
}

bool MSNUnit::ResetConnect()
{
	m_bConnectTried = false;
	MESSENGER()->InvalidateCurrentServer();
	setTransition(0);
	return true;
}

void MSNUnit::RequestMSNServerList()
{
	if (NET()->IsGetParseKey(WNetworkSystem::NET_GAME) &&
		!MESSENGER()->IsWaitingForServerList())
	{
		unsigned long now = GetTickCount();
		if (now - m_serverListRequestTime > 30000)
		{
			MESSENGER()->m_serverList.clear();
			WSendPacket send((enumClientPacket)0x88);
			send.Send(TO_GAME);
			MESSENGER()->SetServerListWaitingState(true);
			m_serverListRequestTime = now;
		}
	}
}
