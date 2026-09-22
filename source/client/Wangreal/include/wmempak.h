#pragma once

template <class T>
class WMemPak
{
public:
	WMemPak(int count = 8)
	{
		blknum = count;
		pos = count;
		prealloc = 0;
		idle = 0;
	}
	~WMemPak()
	{
		Clear();
		while (prealloc)
		{
			listinfo* block = prealloc;
			prealloc = prealloc->next;
			delete[] (char*)block;
		}
	}
	void Clear()
	{
		if (prealloc)
		{
			listinfo* block = prealloc;
			if (block->next)
			{
				do
				{
					block = block->next;
				} while (block->next);
			}
			block->next = idle;
		}
		else
			prealloc = idle;
		idle = 0;
		pos = blknum;
	}
	T* Alloc()
	{
		if (pos >= blknum)
		{
			listinfo* block = prealloc;
			prealloc = prealloc ? prealloc->next : 0;
			if (block)
			{
				block->next = idle;
				idle = block;
			}
			if (!prealloc)
			{
				prealloc = (listinfo*)new char[sizeof(listinfo) +
					(blknum - 1) * sizeof(T)];
				prealloc->next = 0;
			}
			pos = 0;
		}
		return &prealloc->item[pos++];
	}
	void Free(T* item);

private:
	struct listinfo
	{
		listinfo* next;
		T item[1];
	};
	listinfo* prealloc;
	listinfo* idle;
	int blknum;
	int pos;
};
