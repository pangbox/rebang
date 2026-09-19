#pragma once

template <typename T>
class WList
{
private:
	struct listinfo
	{
		char* item;
		char* keycode;
		bool alloc;
		listinfo* prev;
		listinfo* next;
		listinfo* hash;
	};
	listinfo* m_list;
	listinfo* m_surf;
	listinfo* m_pre_alloc;
	listinfo* m_idle;
	int m_blk_len;
	int m_hash_mask;
	int m_hashNum;
	listinfo** m_hash_list;
};
