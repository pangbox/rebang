#include "uselzo.h"

CCompressBuffer::CCompressBuffer()
{
	Init();
}

CCompressBuffer::~CCompressBuffer()
{
	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}
}

void CCompressBuffer::Init()
{
	static int iResult = 1;
	if (iResult == 1)
	{
		iResult = lzo_init();
	}

	m_bInitialized = iResult;
	m_bBufferUsed = false;
	m_iSize = 0;
	m_Buffer = NULL;
}

int CCompressBuffer::Compress(unsigned char* pSrc, int srcSize)
{
	if (m_bBufferUsed)
	{
		m_iSize = 0;
		return 1;
	}

	if (srcSize == 0)
	{
		return 3;
	}

	if (pSrc == NULL)
	{
		return 2;
	}

	m_bBufferUsed = true;

	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}

	m_Buffer = new unsigned char[Max(MAX_BUF_LEN, srcSize * 2)];
	memset(m_Buffer, 0, Max(MAX_BUF_LEN, srcSize * 2));

	int size = srcSize;
	for (int i = 0; i < eHeaderLen - 1; i++)
	{
		m_Buffer[eHeaderLen - 1 - i] = size % 255;
		size = (size - m_Buffer[eHeaderLen - 1 - i]) / 255;
	}

	if (m_bInitialized != LZO_E_OK)
	{
		m_Buffer[0] = eNotCompressed;
		for (int i = 0; i < srcSize; i++)
		{
			m_Buffer[i + eHeaderLen] = pSrc[i];
		}

		m_iSize = srcSize + eHeaderLen;
		return 4;
	}

	m_Buffer[0] = eCompressed;

	unsigned char temp[OUT_LEN];
	lzo_uint uDestLen;
	int res =
		lzo1x_1_compress(pSrc, srcSize, m_Buffer + eHeaderLen, &uDestLen, temp);
	if (res != LZO_E_OK)
	{
		char log[128];
		sprintf(log, "::COMPRESS -> The result should be zero! (res : %d)",
			res);
	}

	m_iSize = uDestLen + eHeaderLen;
	return 0;
}

int CCompressBuffer::Decompress(unsigned char* pSrc, int srcSize)
{
	if (m_bBufferUsed)
	{
		m_iSize = 0;
		return 1;
	}

	if (pSrc == NULL)
	{
		return 2;
	}

	if (srcSize == 0)
	{
		return 3;
	}

	int WrittenOrgSize = 0;
	int mul = 1;
	for (int i = 0; i < eHeaderLen - 1; i++)
	{
		WrittenOrgSize += pSrc[eHeaderLen - 1 - i] * mul;
		mul *= 255;
	}

	m_bBufferUsed = true;

	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}

	m_Buffer = new unsigned char[Max(WrittenOrgSize + 10, MAX_BUF_LEN)];
	memset(m_Buffer, 0, Max(WrittenOrgSize + 10, MAX_BUF_LEN));

	if (pSrc[0] == eNotCompressed)
	{
		for (int i = 0; i < srcSize - eHeaderLen; i++)
		{
			m_Buffer[i] = pSrc[i + eHeaderLen];
		}

		if (WrittenOrgSize == srcSize - eHeaderLen)
		{
			m_iSize = srcSize - eHeaderLen;
		}
		else
		{
			m_iSize = 0;
			return 5;
		}

		return 0;
	}
	else if (pSrc[0] == eCompressed)
	{
		if (m_bInitialized != LZO_E_OK)
		{
			m_iSize = 0;
			return 7;
		}

		lzo_uint uDestLen;
		int res = lzo1x_decompress(pSrc + eHeaderLen, srcSize - eHeaderLen,
			m_Buffer, &uDestLen, NULL);
		if (res != LZO_E_OK)
		{
			char log[128];
			sprintf(log,
				"::DECOMPRESS -> The result should be zero! (res : %d)", res);
			return 8;
		}

		if (WrittenOrgSize == uDestLen)
		{
			m_iSize = uDestLen;
		}
		else
		{
			m_iSize = 0;
			return 6;
		}

		return 0;
	}
	else
	{
		return 9;
	}
}

unsigned char* CCompressBuffer::GetNewBuffer()
{
	if (m_iSize == 0)
	{
		return NULL;
	}
	else
	{
		return m_Buffer;
	}
}

int CCompressBuffer::GetNewSize()
{
	return m_iSize;
}
