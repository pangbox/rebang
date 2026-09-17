#pragma once
#include <string>

class BaseObject
{
public:
	typedef void (*Callback)(BaseObject*);

	static void SetConstructionCallback(Callback fn);
	static void SetDestructionCallback(Callback fn);

	BaseObject();
	virtual ~BaseObject();

	const char* GetLeakHint();
	void SetLeakHint(const char* hint);

private:
	int m_id;
	int m_ref;
	std::string m_leakHint;

	static int ms_nextId;
	static int ms_count;
	static Callback ms_fnConstruct;
	static Callback ms_fnDestruct;
};
