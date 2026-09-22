#pragma once
#include "wmemblock.h"

inline void* WMemFillBlock::Alloc(int size)
{
	if (size <= (1 << (m_level + 1)))
	{
		for (int index = 0; index < m_level; ++index)
		{
			if (size <= (1 << (index + 2)))
			{
				int last = m_lastidx[index];
				for (int step = 0; step < m_num; step += 32)
				{
					if (step + last >= m_num)
						last -= m_num;
					if (m_mask[index][(step + last) >> 5] != 0xffffffff)
					{
						int slot = FindSlot(m_mask[index][(step + last) >> 5]);
						m_mask[index][(step + last) >> 5] |= 1 << slot;
						m_lastidx[index] = step + last;
						return m_mem[index] +
							((step + slot + last) << (index + 2));
					}
				}
				break;
			}
		}
	}
	return malloc(size);
}

inline void WMemFillBlock::Free(void* block)
{
	int offset = (int)((unsigned char*)block - m_ptr);
	if (offset < 0 || offset >= m_size)
	{
		free(block);
		return;
	}
	int level = 0;
	int base = 0;
	for (; level < m_level; ++level)
	{
		int span = m_num << (level + 2);
		if (offset - base < span)
		{
			int slot = (offset - base) >> (level + 2);
			m_mask[level][slot >> 5] &= ~(1 << (slot & 31));
			return;
		}
		base += span;
	}
}

inline int WMemFillBlock::FindSlot(unsigned long used)
{
	int slot = 0;
	do
	{
		if (!(used & (1 << slot)))
			return slot;
		++slot;
	} while (slot < 32);
	return -1;
}
