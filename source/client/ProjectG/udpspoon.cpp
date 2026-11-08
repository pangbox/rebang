#include "udpspoon.h"

unsigned int __stdcall SubSpoonThread(void* Param)
{
	UDPSpoon* pSpoon = (UDPSpoon*)Param;
	pSpoon->Run();
	return 0;
}

void MakeSpoon(const char* IP, int Port, sockaddr_in* Sa)
{
	memset(Sa, 0, sizeof(sockaddr_in));
	Sa->sin_addr.s_addr = inet_addr(IP);
	Sa->sin_family = AF_INET;
	Sa->sin_port = htons(Port);
}

UDPSpoon::UDPSpoon()
{
	WSADATA wsa;
	memset(&wsa, 0, sizeof(wsa));
	WSAStartup(2, &wsa);

	m_bFinish = false;
}

UDPSpoon::~UDPSpoon()
{
	CloseHandle(m_hSignal);
	WSACleanup();
}

void UDPSpoon::Error(const char* Msg)
{
	m_Param.ProcErr(Msg);
}

bool UDPSpoon::Open(SpoonParam& Param)
{
	m_Param = Param;

	LPPROTOENT lpProtocol = getprotobyname("UDP");

	if (lpProtocol == NULL)
	{
		Error("getprotobyname(UDP) Failed");
		return false;
	}

	m_Socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);

	if (m_Socket == INVALID_SOCKET)
	{
		Error("WSASocket Faield");
		return false;
	}

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (m_Param.bindPort != 0)
	{
		sin.sin_port = htons(Param.bindPort);
	}
	else
	{
		sin.sin_port = htons(0);
	}

	if (bind(m_Socket, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		Error("bind Failed");
		return false;
	}

	m_hSignal = CreateEvent(NULL, FALSE, FALSE, NULL);

	return true;
}

void UDPSpoon::Close()
{
	m_bFinish = true;
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	closesocket(m_Socket);
}

void UDPSpoon::Run()
{
	static char Buffer[4096];
	WSANETWORKEVENTS NetworkEvents;
	sockaddr_in AddrFrom;
	int iAddrSize;

	while (!m_bFinish)
	{
		if (WSAWaitForMultipleEvents(1, &m_hSignal, FALSE, 10, FALSE) ==
			WSA_WAIT_EVENT_0)
		{
			if (WSAEnumNetworkEvents(m_Socket, m_hSignal, &NetworkEvents) ==
				SOCKET_ERROR)
			{
				return;
			}

			if (NetworkEvents.lNetworkEvents & FD_READ)
			{
				if (NetworkEvents.iErrorCode[FD_READ_BIT] == 0)
				{
					memset(Buffer, 0, 1024);
					iAddrSize = sizeof(AddrFrom);
					int nRead = recvfrom(m_Socket, Buffer, 4096, 0,
						(sockaddr*)&AddrFrom, &iAddrSize);
					if (nRead == SOCKET_ERROR)
					{
						GetLastError();
					}
					else
					{
						m_Param.ProcMsg(AddrFrom, Buffer, nRead);
					}
				}
			}
		}

		Sleep(1);
	}
}

int UDPSpoon::Send(const char* Buffer, int Len, sockaddr_in* Addr)
{
	return sendto(m_Socket, Buffer, Len, 0, (sockaddr*)Addr,
		sizeof(sockaddr_in));
}
