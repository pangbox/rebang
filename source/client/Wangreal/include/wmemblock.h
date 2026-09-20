#pragma once

class WMemFillBlock
{
public:
	WMemFillBlock(int level, int num);
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
