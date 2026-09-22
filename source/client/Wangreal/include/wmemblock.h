#pragma once
#include <stdlib.h>
#include <string.h>

class WMemFillBlock
{
public:
	WMemFillBlock(int num, int level);
	~WMemFillBlock();
	inline void* Alloc(int size);
	void Free(void* data);

private:
	int FindSlot(unsigned long used);
	unsigned char* m_mem[6];
	unsigned long* m_mask[6];
	unsigned char* m_ptr;
	int m_lastidx[6];
	int m_num;
	int m_size;
	int m_level;
};

extern WMemFillBlock g_mem;

inline WMemFillBlock::WMemFillBlock(int num, int level)
{
	m_num = (num + 31) & ~31;
	m_level = level;
	m_size = ((1 << level) - 1) * m_num * 4;
	m_ptr = (unsigned char*)malloc(m_size + m_num / 8 * level);
	int i = 0;
	int offset = 0;
	for (; i < m_level; ++i)
	{
		m_mem[i] = m_ptr + offset;
		offset += m_num << (i + 2);
	}
	for (int i = 0; i < m_level; ++i)
	{
		m_mask[i] = (unsigned long*)(m_ptr + offset);
		memset(m_mask[i], 0, m_num / 8);
		offset += m_num / 8;
	}
	for (int i = 0; i < m_level; ++i)
		m_lastidx[i] = 0;
}

inline WMemFillBlock::~WMemFillBlock()
{
	free(m_ptr);
}
