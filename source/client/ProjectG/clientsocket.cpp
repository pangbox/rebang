#include "clientsocket.h"
#include "networkmonitor.h"
#include "exception.h"

#define MAX_RECV_BUFFER 0x3fffc
#define MAX_SEND_BUFFER 0xc000

extern HWND g_hwnd;
extern int g_iSendPacketType;
extern int g_iSendFailType;

WClientSocket::WClientSocket(unsigned int sock, sockaddr_in addr)
	: WSocket(sock, addr), m_rcvBuf(MAX_RECV_BUFFER), m_sndBuf(MAX_SEND_BUFFER)
{
	m_rcvCnt = 0;
	m_sndCnt = 0;
	m_packetHeaderType = 0;
	m_sendSeq = 40;
	m_parseKey = -1;
}

WClientSocket::~WClientSocket()
{
}

void WClientSocket::Close()
{
	WSocket::Close();
	WSocket::CreateSocket();

	m_rcvCnt = 0;
	m_sndCnt = 0;
	m_packetHeaderType = 0;
	m_sendSeq = 40;
	m_parseKey = -1;

	m_rcvBuf.ResetBuffer();
}

int WClientSocket::TryRead()
{
	if (Poll(0))
	{
		if (m_rcvCnt >= MAX_RECV_BUFFER)
			throw WAppException(
				"! socket receive buffer overflow, close connection");

		int n = Read(m_rcvBuf.LockBuffer(m_rcvCnt), MAX_RECV_BUFFER - m_rcvCnt);

		if (n == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return 1;

			n = 0;
		}

		if (n == 0)
		{
			m_bConnected = FALSE;
			return 0;
		}

		m_rcvCnt += n;
		if (m_rcvCnt < 0 || m_rcvCnt > MAX_RECV_BUFFER)
			throw WAppException(
				"! socket receive buffer overflow, close connection");

		return 2;
	}

	return 1;
}

int WClientSocket::GetPacket(WReceivedPacket& packet)
{
	if (m_rcvCnt >= 4)
	{
		const unsigned char* buf = m_rcvBuf.GetBuffer();

		int len = (unsigned short)((buf[2] << 8) | buf[1]);

		if (len + 3 > m_rcvCnt)
		{
			return 4;
		}

		packet.SetRcvPacket(buf + 3, len, m_parseKey, buf[0]);
		m_rcvCnt -= len + 3;

		if (m_rcvCnt > 0)
			memmove(m_rcvBuf.LockBuffer(0), m_rcvBuf.GetBuffer() + len + 3,
				m_rcvCnt);

		CNetworkMonitor::Instance()->TraceInBytes(len + 3);

		bool rawPacket = false;
		if (m_parseKey == -1)
			rawPacket = true;

		if (packet.IsValid(rawPacket))
		{
			return 3;
		}
		else
		{
			packet.Reset();
			m_rcvCnt = 0;
			return 0;
		}
	}

	return 4;
}

int WClientSocket::SendPacket(WSendPacket& packet)
{
	m_cs.Enter();
	if (!PutPacket(packet))
	{
		m_cs.Leave();
		return FALSE;
	}

	Flush();
	m_cs.Leave();
	return TRUE;
}

int WClientSocket::PutPacket(WSendPacket& packet)
{
	if (packet.GetPacketType() != 246)
		g_iSendPacketType = packet.GetPacketType();

	packet.MakePacketComplete(m_parseKey);

	int len = (packet.GetBuffer()[2] << 8) + packet.GetBuffer()[1] + 4;
	if (m_sndCnt + len > MAX_SEND_BUFFER)
		throw WAppException("! socket send buffer overflow");

	memmove(m_sndBuf.LockBuffer(m_sndCnt), packet.GetBuffer(), len);
	m_sendSeq += 3;
	m_sndCnt += len;

	return TRUE;
}

void WClientSocket::Flush()
{
	int sent = 0;

	while (m_sndCnt > 0)
	{
		int n = Write(m_sndBuf.GetBuffer() + sent, m_sndCnt);

		if (n == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				memmove(m_sndBuf.LockBuffer(0), m_sndBuf.GetBuffer() + sent,
					m_sndCnt);
				break;
			}

			g_iSendFailType = g_iSendPacketType;
			m_sndCnt = 0;
			break;
		}

		m_sndCnt -= n;
		sent += n;
	}
}

void WClientSocket::SetPacketHeaderType(int type)
{
	m_packetHeaderType = type;
}

void WClientSocket::AsyncSelect(long events)
{
	WSAAsyncSelect(m_socket, g_hwnd, m_eventMsg, events);
}
