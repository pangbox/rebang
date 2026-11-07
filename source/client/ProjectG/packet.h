#pragma once

#include <string>

enum enumClientPacket;

enum eSendTo
{
	TO_GAME,
	TO_MSN,
	TO_SHOP,
	TO_LOGIN,
	TO_RANK,
	TO_COUNT
};

class WMemBlock
{
public:
	const unsigned char* GetBuffer() const { return m_pBuffer; }

	unsigned char* LockBuffer(int offset) { return m_pBuffer + offset; }

	unsigned int GetLength() const { return m_offset; }

	WMemBlock();
	WMemBlock(unsigned int uSize);
	virtual ~WMemBlock();

	int Alloc(unsigned int uSize);
	int ReAlloc(unsigned int uSize);
	int Free();
	void ResetBuffer();

protected:
	unsigned int GetBufferSize() { return m_uSize; }

	unsigned char* m_pBuffer;
	unsigned int m_uSize;
	unsigned int m_offset;
};

class WSendPacket : public WMemBlock
{
public:
	WSendPacket();
	WSendPacket(enumClientPacket type)
		: WMemBlock(0x40)
	{
		InitPacket(type);
	}
	WSendPacket(int type)
		: WMemBlock(0x40)
	{
		InitPacket(type);
	}
	virtual ~WSendPacket();

	void InitPacket(int nPacketType);
	int GetPacketType() const;
	int GetMark() const;
	unsigned int GetLengthData();

	void Encode1(unsigned char in);
	void Encode2(unsigned short in);
	void Encode4(unsigned long in);
	void Encode8(__int64 in);
	void EncodeStr(const std::string& str);
	void EncodeEncryptedStr(const std::string& str);
	void EncodeBuffer(const void* src, unsigned short len);
	void EncodeData(int OffSet, char* buffer, int Len);

	bool SetSendPacket(const unsigned char* pBuf, unsigned int len,
		unsigned char ParseKey, unsigned char PubKey);
	void MakePacketComplete(int ParseKey);
	void RollBack();
	void Send(eSendTo to);

	static unsigned char ms_nPacketCount;

protected:
	void MakeCheckSum();

	int m_nPacketType;
	unsigned char m_nPandoraKey;
};

class WReceivedPacket : public WMemBlock
{
public:
	WReceivedPacket();
	virtual ~WReceivedPacket();

	bool SetRcvPacket(const unsigned char* pBuf, unsigned int len,
		unsigned char ParseKey, unsigned char PubKey);
	bool IsValid(bool rawpacket);
	void Copy(WReceivedPacket* pCopy);
	void Reset();

	bool DecodeCheckSum();
	unsigned char Decode1();
	unsigned short Decode2();
	unsigned long Decode4();
	__int64 Decode8();
	std::string DecodeStr();
	bool DecodeStr(std::string& str);
	std::string DecodeEncryptedStr(unsigned long key);
	void DecodeBuffer(void* pBuffer, int len);

	unsigned char* GetRemainBuffer();
	int GetRemainLength();

protected:
	int m_nPubKey;
	int m_nEncLen;
	unsigned int m_nLength;
	unsigned char m_nParseKey;
};
