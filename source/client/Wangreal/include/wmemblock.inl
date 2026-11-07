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
