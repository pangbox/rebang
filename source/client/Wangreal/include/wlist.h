#pragma once
#include "wmemblock.h"
#include <string.h>

template <class T>
class WList
{
public:
	struct listinfo
	{
		T item;
		char* keycode;
		bool alloc;
		listinfo* prev;
		listinfo* next;
		listinfo* hash;
	};

	__declspec(nothrow) WList(int len = 8, int hashNum = 0);
	__declspec(nothrow) ~WList();
	T Start();
	T Next();
	void Reset();
	void AddItem(const T& item, const char* keycode, bool alloc);
	void operator+=(const T& item);
	void operator-=(const T& item);

protected:
	void DelItem(const T& item);
	listinfo* Link(listinfo* head, listinfo* item);
	listinfo* Unlink(listinfo* head, listinfo* item);

private:
	listinfo* m_list;
	listinfo* m_surf;
	listinfo* m_pre_alloc;
	listinfo* m_idle;
	int m_blk_len;
	int m_hash_mask;
	int m_hashNum;
	listinfo* Alloc();
	listinfo** m_hash_list;
	void AddHash(int hashCode, listinfo* item);
	void DelHash(listinfo* item);
	int HASHCODE(const void* keycode) const;
};

#include "wlist.inl"
