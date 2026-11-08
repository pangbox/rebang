#include "wboneset.h"
#include "wmemblock.inl"

WBoneKey WBoneSet::m_preAlloc[256];
unsigned char WBoneSet::m_curAlloc;

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
	if (flags & KEY_ROT)
		key->quat = *quat;
	if (flags & KEY_POS)
		key->pivot = *pivot;
	if (flags & KEY_SCALE)
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
	if (b->flags & KEY_ROT)
		c->quat = (a->flags & KEY_ROT)
			? WQuaternionSlerp(a->quat, b->quat, fraction)
			: b->quat;
	if (b->flags & KEY_POS)
		c->pivot = (a->flags & KEY_POS)
			? a->pivot + (b->pivot - a->pivot) * fraction
			: b->pivot;
	if (b->flags & KEY_SCALE)
		c->scale = (a->flags & KEY_SCALE)
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
		{
			result = 0;
		}
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
