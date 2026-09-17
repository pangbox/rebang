#include "baseobject.h"

void BaseObject::SetConstructionCallback(Callback fn)
{
	ms_fnConstruct = fn;
}

void BaseObject::SetDestructionCallback(Callback fn)
{
	ms_fnDestruct = fn;
}

BaseObject::BaseObject()
{
	m_ref = 0;
	m_id = ms_nextId++;
	++ms_count;
	if (ms_fnConstruct)
		ms_fnConstruct(this);
	m_leakHint = "";
}

BaseObject::~BaseObject()
{
	--ms_count;
	if (ms_fnDestruct)
		ms_fnDestruct(this);
}

const char* BaseObject::GetLeakHint()
{
	return m_leakHint.c_str();
}

void BaseObject::SetLeakHint(const char* hint)
{
	m_leakHint = hint;
}

int BaseObject::ms_nextId = 0;
int BaseObject::ms_count = 0;
BaseObject::Callback BaseObject::ms_fnConstruct = 0;
BaseObject::Callback BaseObject::ms_fnDestruct = 0;
