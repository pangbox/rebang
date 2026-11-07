#pragma once
#include <string>

class BaseObject
{
public:
	typedef void (*ConstructionCallback)(BaseObject*);
	typedef void (*DestructionCallback)(BaseObject*);

	BaseObject();
	virtual ~BaseObject();
	static int GetTotalCount();
	static void SetConstructionCallback(ConstructionCallback fn);
	static void SetDestructionCallback(DestructionCallback fn);
	int GetId() const;
	int GetRef() const;
	int AddRef();
	int Release();
	const char* GetLeakHint();
	void SetLeakHint(const char* hint);

private:
	static int ms_nextId;
	static int ms_count;
	static ConstructionCallback ms_fnConstruct;
	static DestructionCallback ms_fnDestruct;

	int m_id;
	int m_ref;
	std::string m_leakHint;
};
