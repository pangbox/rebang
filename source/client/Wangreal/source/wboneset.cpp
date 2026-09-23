#include "wboneset.h"

WBoneKey WBoneSet::m_preAlloc[256];
unsigned char WBoneSet::m_curAlloc;

// HACK: Workaround to preserve original COMDAT order

template <>
WBoneKey* WList<WBoneKey*>::Next()
{
	if (m_surf)
	{
		WBoneKey* item = m_surf->item;
		m_surf = m_surf->next == m_list ? 0 : m_surf->next;
		return item;
	}
	return 0;
}

template <>
void WList<WBoneKey*>::AddHash(int hashCode, listinfo* item)
{
	item->hash = m_hash_list[hashCode];
	m_hash_list[hashCode] = item;
}

template <>
WList<WBoneKey*>::listinfo* WList<WBoneKey*>::Alloc()
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

void WBoneSet::AddBoneKey(char* name, WQuat* quat, WVector* pivot, float* scale,
	int flags)
{
	WBoneKey* key = m_keyList.Find(name);
	if (!key)
	{
		if (m_data)
			key = m_data + m_index++;
		else
			key = m_preAlloc + m_curAlloc++;
		key->name = name;
		m_keyList.AddItem(key, name, false);
	}
	key->flags = (unsigned char)flags;
	if (flags & 1)
		key->quat = *quat;
	if (flags & 2)
		key->pivot = *pivot;
	if (flags & 4)
		key->scale = *scale;
}

void WBoneSet::CopyFrom(WBoneSet* a)
{
	m_keyList.Reset();
	for (WBoneKey* key = a->m_keyList.Start(); key; key = a->m_keyList.Next())
		AddBoneKey(key->name, &key->quat, &key->pivot, &key->scale, key->flags);
}

static void BlendBoneKey(WBoneKey* a, WBoneKey* b, WBoneKey* c, float fraction)
{
	if (b->flags & 1)
		c->quat = (a->flags & 1) ? WQuaternionSlerp(a->quat, b->quat, fraction)
								 : b->quat;
	if (b->flags & 2)
		c->pivot = (a->flags & 2) ? a->pivot + (b->pivot - a->pivot) * fraction
								  : b->pivot;
	if (b->flags & 4)
		c->scale = (a->flags & 4)
			? a->scale = (1 - fraction) * a->scale + fraction * b->scale
			: b->scale;
	c->flags = a->flags | b->flags;
}

void WBoneSet::Blend(WBoneSet* a, WBoneSet* b, float fraction)
{
	if (WisEqual(fraction, 0.0f, g_EPSILON))
	{
		CopyFrom(a);
		return;
	}
	if (WisEqual(fraction, 1.0f, g_EPSILON))
	{
		CopyFrom(b);
		return;
	}
	m_keyList.Reset();
	WBoneKey key;
	for (WBoneKey* first = a->m_keyList.Start(); first;
		first = a->m_keyList.Next())
	{
		WBoneKey* second = b->m_keyList.Find(first->name);
		WBoneKey* result;
		if (second)
		{
			BlendBoneKey(first, second, &key, fraction);
			key.name = first->name;
			result = &key;
		}
		else
			result = 0;
		AddBoneKey(result->name, &result->quat, &result->pivot, &result->scale,
			result->flags);
	}
}

WBoneSet* __cdecl BlendBoneSet(WBoneSet* a, WBoneSet* b, float t)
{
	static unsigned char boneSetPos;
	static WBoneSet boneSetList[4] = { false };
	WBoneSet* result = boneSetList + (boneSetPos++ & 3);
	result->Clear();
	result->Blend(a, b, t);
	return result;
}
