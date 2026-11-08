#include "shopunit.h"

ShopUnit::ShopUnit()
{
}

ShopUnit::~ShopUnit()
{
}

bool ShopUnit::Init()
{
	NetworkUnit::Init();
	return true;
}

void ShopUnit::ShutDown()
{
}

bool ShopUnit::OnLine()
{
	return true;
}

bool ShopUnit::OffLine()
{
	return true;
}

bool ShopUnit::Connect()
{
	if (m_socket.m_bConnected)
	{
		setTransition(0);
	}
	else
	{
		m_socket.SetEventMsg(WM_USER + 11);
		if (m_socket.Connect(inet_addr("127.0.0.1"), htons(7777)) == 0)
		{
			m_bConnectionComplete = TRUE;
			return false;
		}

		if (OnConnect())
		{
			m_socket.AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
			setTransition(0);
			return true;
		}

		return false;
	}

	return true;
}

bool ShopUnit::Connecting()
{
	setTransition(0);
	m_bConnectionComplete = TRUE;
	return true;
}

bool ShopUnit::Disconnecting()
{
	if (m_socket.m_bConnected)
	{
		m_socket.Close();

		setTransition(2);
	}
	else
	{
		setForceTransition(2);
	}
	return true;
}

int ShopUnit::OnConnect()
{
	std::string id = MyId();

	WSendPacket send((enumClientPacket)2);

	send.Encode1(0);
	send.EncodeStr(id);
	send.EncodeStr(std::string("hohoho"));
	send.Send(TO_SHOP);

	return TRUE;
}
