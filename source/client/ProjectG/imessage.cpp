#include "imessage.h"

MemBlock::MemBlock()
{
	m_pBuffer = m_Buffer;
	m_uSize = MAX_MSG_SIZE;
	m_offset = 0;
}

MemBlock::MemBlock(unsigned int uSize)
{
	m_pBuffer = m_Buffer;
	m_uSize = MAX_MSG_SIZE;
	m_offset = 0;
}

void MemBlock::Reset()
{
	m_pBuffer = m_Buffer;
	memset(m_Buffer, 0, sizeof(m_Buffer));
	m_offset = 0;
	m_uSize = MAX_MSG_SIZE;
}

MemBlock::~MemBlock()
{
}

int MemBlock::Alloc(unsigned int uSize)
{
	return 1;
}

int MemBlock::ReAlloc(unsigned int uSize)
{
	return 1;
}

int MemBlock::Free()
{
	return 1;
}

ISendMsg::ISendMsg()
{
	m_nPacketType = 0;
	m_offset = 0;
}

ISendMsg::~ISendMsg()
{
}

ISendMsg::ISendMsg(unsigned short nPaketType)
{
	m_offset = 0;

	Encode2(0);
	m_nPacketType = nPaketType;
	Encode2(nPaketType);
}

void ISendMsg::MakeCheckSum()
{
	unsigned char sum = 0;
	for (unsigned int i = 4; i < m_offset; i++)
		sum += m_pBuffer[i];

	Encode1(sum);
}

void ISendMsg::MakePacketComplete()
{
	unsigned short len = m_offset - 2;

	m_Buffer[0] = (unsigned char)len;
	m_Buffer[1] = (unsigned char)(len >> 8);
}

void ISendMsg::Encode1(unsigned char in)
{
	m_pBuffer[m_offset++] = in;
}

void ISendMsg::Encode2(unsigned short in)
{
	m_pBuffer[m_offset++] = (unsigned char)in;
	m_pBuffer[m_offset++] = (unsigned char)(in >> 8);
}

void ISendMsg::Encode4(unsigned long in)
{
	m_pBuffer[m_offset++] = (unsigned char)in;
	m_pBuffer[m_offset++] = (unsigned char)(in >> 8);
	m_pBuffer[m_offset++] = (unsigned char)(in >> 16);
	m_pBuffer[m_offset++] = (unsigned char)(in >> 24);
}

void ISendMsg::EncodeBuffer(const void* src, unsigned short len)
{
	memcpy(&m_pBuffer[m_offset], src, len);
	m_offset += len;
}

void ISendMsg::EncodeStr(const char* data)
{
	unsigned short len = strlen(data);

	if (len == 0)
		return;

	Encode2(len);

	memcpy(&m_pBuffer[m_offset], data, len);

	m_offset += len;
}

void ISendMsg::FixData(int OffSet, char* Buffer, int Len)
{
	LockBuffer(OffSet);
	memcpy(&m_pBuffer[OffSet + 4], Buffer, Len);
}

IRecvMsg::IRecvMsg()
{
}

IRecvMsg::~IRecvMsg()
{
}

unsigned char IRecvMsg::Decode1()
{
	if (m_offset + 1 > m_nLength)
	{
		m_offset = m_nLength;

		return 0;
	}

	return m_pBuffer[m_offset++];
}

unsigned short IRecvMsg::Decode2()
{
	if (m_offset + 2 > m_nLength)
	{
		m_offset = m_nLength;

		return 0;
	}

	unsigned short value = (m_pBuffer[m_offset + 1] << 8) | m_pBuffer[m_offset];
	m_offset += 2;

	return value;
}

unsigned long IRecvMsg::Decode4()
{
	if (m_offset + 4 > m_nLength)
	{
		m_offset = m_nLength;
		return 0;
	}

	unsigned long value =
		(((((m_pBuffer[m_offset + 3] << 8) | m_pBuffer[m_offset + 2]) << 8) |
			 m_pBuffer[m_offset + 1])
			<< 8) |
		m_pBuffer[m_offset];
	m_offset += 4;

	return value;
}

std::string IRecvMsg::DecodeStr()
{
	unsigned short len = Decode2();

	if (len == 0)
		return "";

	if (m_offset + len > m_nLength)
	{
		m_offset = m_nLength;
		return "";
	}

	std::string str("");
	str.append((const char*)&m_pBuffer[m_offset], len);
	m_offset += len;

	return str;
}

void IRecvMsg::DecodeBuffer(void* pBuffer, int len)
{
	if (m_offset + len > m_nLength)
	{
		m_offset = m_nLength;
		memset(pBuffer, 0, len);
		return;
	}

	memcpy(pBuffer, &m_pBuffer[m_offset], len);
	m_offset += len;
}

bool IRecvMsg::DecodeCheckSum()
{
	unsigned char sum = m_pBuffer[m_offset++];

	for (unsigned int i = 0; i < m_nLength - 1; i++)
		sum -= m_pBuffer[i];

	if (sum != 0)
	{
		return false;
	}

	return true;
}

bool IRecvMsg::IsValid(bool rawpacket)
{
	return true;
}

void IRecvMsg::SetRcvPacket(const char* pBuf, int Len)
{
	memset(m_Buffer, 0, sizeof(m_Buffer));
	m_pBuffer = m_Buffer;
	m_nLength = Len;
	m_uSize = MAX_MSG_SIZE;
	m_offset = 0;

	memcpy(m_Buffer, pBuf, Len);

	m_nLength = Len;
}
