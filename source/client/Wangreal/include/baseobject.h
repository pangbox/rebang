#pragma once
#include <string>

class BaseObject
{
public:
	typedef void (*ConstructionCallback)(BaseObject*);
	typedef void (*DestructionCallback)(BaseObject*);

	BaseObject();
	virtual ~BaseObject();
	static int GetTotalCount() { return ms_count; }
	static void SetConstructionCallback(ConstructionCallback fn);
	static void SetDestructionCallback(DestructionCallback fn);
	int GetId() const { return m_id; }
	int GetRef() const { return m_ref; }
	int AddRef() { return ++m_ref; }
	int Release()
	{
		int count = --m_ref;
		if (count == 0)
			delete this;
		return count;
	}
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
