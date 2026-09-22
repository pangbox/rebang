#pragma once

inline WBoneSet::WBoneSet(bool preAlloc)
	: m_keyList(32, 256)
{
	if (preAlloc)
		m_data = new WBoneKey[256];
	else
		m_data = 0;
	m_index = 0;
}

inline WBoneSet::~WBoneSet()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = 0;
	}
}
