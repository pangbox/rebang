#pragma once
#include "wmath.h"
#include "wlist.h"

struct WBoneKey
{
	char* name;
	WQuat quat;
	WVector pivot;
	float scale;
	unsigned char flags;
};

class WBoneSet;
WBoneSet* __cdecl BlendBoneSet(WBoneSet* first, WBoneSet* second, float ratio);

class WBoneSet
{
public:
	WBoneSet(bool preAlloc = true);
	~WBoneSet();
	void Clear() { m_keyList.Reset(); }
	WBoneKey* FindBoneKey(char* name) { return m_keyList.Find(name); }
	void CopyFrom(WBoneSet* source);
	void Blend(WBoneSet* first, WBoneSet* second, float ratio);

private:
	WList<WBoneKey*> m_keyList;
	WBoneKey* m_data;
	unsigned char m_index;
	static WBoneKey m_preAlloc[256];
	static unsigned char m_curAlloc;
};

#include "wboneset.inl"
