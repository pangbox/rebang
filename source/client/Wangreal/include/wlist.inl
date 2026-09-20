#pragma once

template <class T>
T WList<T>::Start()
{
	m_surf = m_list;
	return Next();
}

template <class T>
T WList<T>::Next()
{
	if (m_surf)
	{
		T item = m_surf->item;
		m_surf = m_surf->next == m_list ? 0 : m_surf->next;
		return item;
	}
	return 0;
}

template <class T>
typename WList<T>::listinfo* WList<T>::Link(listinfo* head, listinfo* item)
{
	if (!head)
	{
		item->prev = item;
		item->next = item;
		return item;
	}
	item->next = head;
	item->prev = head->prev;
	head->prev = item;
	item->prev->next = item;
	return head;
}

template <class T>
typename WList<T>::listinfo* WList<T>::Unlink(listinfo* head, listinfo* item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	if (item != head)
		return head;
	if (item->next == head)
		return 0;
	return item->next;
}

template <class T>
void WList<T>::AddHash(int hashCode, listinfo* item)
{
	item->hash = m_hash_list[hashCode];
	m_hash_list[hashCode] = item;
}

template <class T>
int WList<T>::HASHCODE(const void* keycode) const
{
	const unsigned char* string = (const unsigned char*)keycode;
	int length = (int)strlen((const char*)string);
	int last;
	if (length > 0)
		last = length - 1;
	else
		last = 0;
	return (string[last] & 0x15 | string[0] & 0x2a | length * 0x40) &
		m_hash_mask;
}

template <class T>
WList<T>::WList(int len, int hashNum)
{
	m_blk_len = len;
	m_hash_list = 0;
	m_list = 0;
	m_idle = 0;
	m_pre_alloc = 0;
	m_surf = 0;
	m_hashNum = hashNum;
	if (hashNum > 0)
	{
		int size = m_hashNum * sizeof(listinfo*);
		m_hash_list = (listinfo**)g_mem.Alloc(size);
		m_hash_mask = m_hashNum - 1;
		for (int i = 0; i < m_hashNum; ++i)
			m_hash_list[i] = 0;
	}
	else
	{
		m_hash_list = 0;
	}
}

template <class T>
WList<T>::~WList()
{
	while (m_pre_alloc)
	{
		listinfo* allocation = m_pre_alloc;
		m_pre_alloc = Unlink(m_pre_alloc, allocation);
		g_mem.Free(allocation);
	}
	if (m_hash_list)
	{
		g_mem.Free(m_hash_list);
		m_hash_list = 0;
	}
}

template <class T>
void WList<T>::Reset()
{
	if (m_list)
	{
		if (m_idle)
		{
			m_list->prev->next = m_idle->next;
			m_idle->next->prev = m_list->prev;
			m_idle->next = m_list;
			m_list->prev = m_idle;
			m_list = 0;
		}
		else
		{
			do
			{
				listinfo* list = Unlink(m_list, m_list);
				m_idle = Link(m_idle, m_list);
				m_list = list;
			} while (m_list);
		}
	}
	for (int i = 0; i < m_hashNum; ++i)
		m_hash_list[i] = 0;
}

template <class T>
void WList<T>::DelHash(listinfo* item)
{
	int hashCode = HASHCODE(item->keycode);
	listinfo* current = m_hash_list[hashCode];
	if (current == item)
	{
		m_hash_list[hashCode] = item->hash;
		return;
	}
	if (current)
	{
		while (current)
		{
			if (current->hash == item)
			{
				current->hash = item->hash;
				break;
			}
			current = current->hash;
		}
	}
}

template <class T>
typename WList<T>::listinfo* WList<T>::Alloc()
{
	if (!m_idle)
	{
		listinfo* block = (listinfo*)g_mem.Alloc(m_blk_len * sizeof(listinfo));
		m_pre_alloc = Link(m_pre_alloc, block);
		for (int i = 1; i < m_blk_len; ++i)
			m_idle = Link(m_idle, block + i);
	}
	listinfo* item = m_idle;
	m_idle = Unlink(m_idle, item);
	return item;
}

template <class T>
void WList<T>::AddItem(const T& item, const char* keycode, bool alloc)
{
	listinfo* info = Alloc();
	info->item = item;
	info->alloc = alloc;
	if (alloc)
	{
		info->keycode = (char*)g_mem.Alloc((int)strlen(keycode) + 1);
		strcpy(info->keycode, keycode);
	}
	else
	{
		info->keycode = (char*)keycode;
	}
	if (keycode)
		AddHash(HASHCODE(keycode), info);
	m_list = Link(m_list, info);
}

template <class T>
void WList<T>::DelItem(const T& item)
{
	listinfo* found = m_list;
	if (found)
	{
		do
		{
			if (found->item == item)
			{
				if (m_surf == found)
					m_surf = m_surf->next == m_list ? 0 : m_surf->next;
				if (found->keycode)
				{
					DelHash(found);
					if (found->alloc)
						g_mem.Free(found->keycode);
				}
				m_list = Unlink(m_list, found);
				m_idle = Link(m_idle, found);
				if (!m_list)
					m_surf = 0;
				break;
			}
			found = found->next;
		} while (found != m_list);
	}
}

template <class T>
void WList<T>::operator+=(const T& item)
{
	AddItem(item, 0, false);
}

template <class T>
void WList<T>::operator-=(const T& item)
{
	DelItem(item);
}
