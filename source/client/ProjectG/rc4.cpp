#include "rc4.h"
#include <stdlib.h>

#define swap_byte(a, b) \
	{ \
		unsigned char swapByte = *(a); \
		*(a) = *(b); \
		*(b) = swapByte; \
	}

CRC4::CRC4()
{
	m_pKeyData = (unsigned char*)malloc(17);
}

CRC4::CRC4(unsigned char* key, int len)
{
}

CRC4::~CRC4()
{
	free(m_pKeyData);
}

void CRC4::SetKey(unsigned char* key)
{
	memcpy(m_pKeyData, key, 4);
}

void CRC4::PrepareKey(unsigned char* key, int len)
{
	unsigned char index1, index2;
	unsigned char* state;
	short counter;
	state = &m_key.state[0];
	for (counter = 0; counter < 256; counter++)
		state[counter] = (unsigned char)counter;

	m_key.x = m_key.y = 0;
	index1 = 0;
	index2 = 0;

	for (counter = 0; counter < 256; counter++)
	{
		index2 = (state[counter] + key[index1] + index2) % 256;
		swap_byte(&state[counter], &state[index2]);
		index1 = (index1 + 1) % len;
	}
}

void CRC4::Conversion(unsigned char* buffer, int len)
{
	PrepareKey(m_pKeyData, 4);

	unsigned char x;
	unsigned char y;
	unsigned char* state;
	unsigned char xorIndex;
	int counter;
	x = m_key.x;
	y = m_key.y;

	state = &m_key.state[0];

	for (counter = 0; counter < len; counter++)
	{
		x = (x + 1) % 256;
		y = (state[x] + y) % 256;
		swap_byte(&state[x], &state[y]);
		xorIndex = state[x] + state[y] % 256;
		buffer[counter] ^= state[xorIndex];
	}

	m_key.x = 0;
	m_key.y = 0;
}
