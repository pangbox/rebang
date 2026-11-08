#include "loginunit.h"
#include "networksystem.h"
#include "projectg.h"
#include "logininfo.h"

const char* LSIP_ADDRESS[] = { "211.44.251.73" };
int LOGIN_PORT_LIST[] = { 10101 };

LoginUnit::LoginUnit()
{
}

LoginUnit::~LoginUnit()
{
}

bool LoginUnit::Init()
{
	NetworkUnit::Init();
	return true;
}

void LoginUnit::ShutDown()
{
	Init();
}

bool LoginUnit::OnLine()
{
	WReceivedPacket packet;

	if (m_socket.GetPacket(packet) == 3)
	{
		CProjectG::Instance()->OnPacket(packet);
	}

	return true;
}

bool LoginUnit::OffLine()
{
	return true;
}

bool LoginUnit::Connect()
{
	if (m_socket.m_bConnected)
	{
		setForceTransition(0);
	}
	else
	{
		std::string ip;

		ip = LSIP_ADDRESS[rand() % 1];

		int port = LOGIN_PORT_LIST[rand() % 1];

		m_socket.SetEventMsg(WM_USER + 13);
		char szIP[21] = "\0";

		hostent* host = gethostbyname(ip.c_str());

		if (host)
		{
			wsprintf(szIP, "%s", inet_ntoa(*(in_addr*)host->h_addr_list[0]));
		}

		if (host == NULL || m_socket.Connect(inet_addr(szIP), htons(port)) == 0)
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

bool LoginUnit::Connecting()
{
	if (GetTickCount() - m_connectTime > 3000 || !m_socket.TryRead())
	{
		setTransition(1);
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

bool LoginUnit::Disconnecting()
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

int LoginUnit::OnConnect(WReceivedPacket& packet)
{
	packet.Decode1();

	if (packet.Decode2() != 0)
		return FALSE;

	int parseKey = packet.Decode4();
	unsigned long serverUID = packet.Decode4();
	Doc()->m_loginServerUID = serverUID;

	m_socket.SetParseKey(parseKey);

	WSendPacket send(1);
	send.EncodeStr(LOGINID());

	unsigned long provType = LOGININFO()->ProvType();

	send.EncodeStr(LOGINPW());

	send.Encode4(provType);
	send.Encode1(IsWebLogin());
	send.Encode4(AUTHUID());
	switch (provType)
	{
	case 2:
		send.Encode8(0x7fffffffffffffff);
		break;

	case 4:
	{
		NhnLogin* pNhn = (NhnLogin*)LOGININFO()->QueryInterface();
		send.Encode8(pNhn->GetNHNMemberNumber());

		break;
	}

	default:
		return FALSE;
	}

	send.Encode1(IsPcBang());

	send.Send(TO_LOGIN);

	return TRUE;
}
