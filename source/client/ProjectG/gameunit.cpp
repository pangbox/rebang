#include "gameunit.h"
#include "networksystem.h"
#include "projectg.h"
#include "taskmanager.h"
#include "logininfo.h"
#include "packetversion.h"
#include "../../shared/encryption.h"
#include "../../shared/localize.h"

GameUnit::GameUnit()
{
}

GameUnit::~GameUnit()
{
}

bool GameUnit::Init()
{
	NetworkUnit::Init();

	WNetworkSystem::Instance()->SetParseKey(WNetworkSystem::NET_GAME, false);

	return true;
}

void GameUnit::ShutDown()
{
	CTaskManager::Instance()->ReleaseWork();
	Init();
}

bool GameUnit::OnLine()
{
	WReceivedPacket packet;

	unsigned long time = timeGetTime();

	while (m_socket.GetPacket(packet) == 3)
	{
		CProjectG::Instance()->OnPacket(packet);

		if (timeGetTime() - time > 1000)
			break;
	}

	return true;
}

bool GameUnit::OffLine()
{
	return true;
}

bool GameUnit::Connect()
{
	if (m_socket.m_bConnected)
	{
		setForceTransition(0);
		return true;
	}

	std::string addr = Doc()->m_gameServerAddr;
	unsigned long port = Doc()->m_gameServerPort;

	m_socket.SetEventMsg(WM_USER + 10);
	if (m_socket.Connect(inet_addr(addr.c_str()), htons((u_short)port)) == 0)
	{
		setForceTransition(1);
		m_bConnectionComplete = TRUE;

		NET()->ForceShutDown(WNetworkSystem::NET_MSN);

		if (IsLocalContent((localContentType_t)0x5a))
		{
			CProjectG::Instance()->SetMoveLoginServer(MyId(),
				Doc()->m_password.c_str());

			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"LOGIN", 0,
				0, 0);
		}

		return false;
	}

	m_connectTime = GetTickCount();
	setTransition(0);

	return true;
}

bool GameUnit::Connecting()
{
	if (GetTickCount() - m_connectTime > 30000 || !m_socket.TryRead())
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

bool GameUnit::Disconnecting()
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

int GameUnit::OnConnect(WReceivedPacket& packet)
{
	packet.Decode1();

	if (packet.Decode2() != 0x3d)
	{
		return FALSE;
	}

	CTaskManager::Instance()->ReleaseWork();

	packet.Decode1();
	packet.Decode1();
	int parseKey = packet.Decode1();

	m_socket.SetParseKey(parseKey);

	NET()->SetParseKey(WNetworkSystem::NET_GAME, true);

	std::string id = MyId();
	std::string password = Doc()->m_password;

	unsigned long version = PY_PACKET_VERSION;
	Decrypt(&version, sizeof(version));

	WSendPacket send((enumClientPacket)2);
	send.EncodeStr(id);
	send.Encode4(MyUID());
	send.Encode4(CLoginInfo::Instance()->AuthUid());
	send.Encode2(0x6696);
	send.EncodeStr(Doc()->GetLoginAuthKey());
	send.EncodeStr(std::string(PY_CLIENT_VERSION));
	send.Encode4(version);

	send.Encode4(CLoginInfo::Instance()->IsPcBang());

	send.Send(TO_GAME);

	return TRUE;
}
