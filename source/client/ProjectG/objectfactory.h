#pragma once
#include "rtti.h"
#include <string>
#include <map>

class IObject
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }
	bool IsExactKindOf(const WRTTI* pRTTI) const { return GetRTTI() == pRTTI; }
	bool IsKindOf(const WRTTI* pRTTI) const
	{
		const WRTTI* pBase = GetRTTI();
		while (pBase)
		{
			if (pBase == pRTTI)
				return true;
			pBase = pBase->GetBaseRTTI();
		}
		return false;
	}
	void* DynamicCast(const WRTTI* pRTTI) { return IsKindOf(pRTTI) ? this : 0; }
	virtual ~IObject();

protected:
	IObject();
};

class CObjectFactory
{
public:
	CObjectFactory() { }
	~CObjectFactory() { Reset(); }
	IObject* CreateObject(const char* name)
	{
		std::map<const std::string, IObject* (*)()>::iterator it =
			m_objectFuncList.find(name);
		if (it != m_objectFuncList.end())
			return (*it).second();
		return NULL;
	}
	void AddObjectType(IObject* object, const char* name);
	void AddObjectFunctor(IObject* (*function)(), const char* name);
	void Reset();

protected:
	std::map<const std::string, IObject*> m_objectTypeList;
	std::map<const std::string, IObject* (*)()> m_objectFuncList;
};

CObjectFactory& ObjectFactory();

#define DECLARE_OBJECT(className) \
public: \
	static const WRTTI m_RTTI; \
	virtual const WRTTI* GetRTTI() const \
	{ \
		return &m_RTTI; \
	} \
	friend IObject* className##MakeInstance();

#define IMPLEMENT_OBJECT(className, baseClass) \
	IObject* className##MakeInstance() \
	{ \
		return new className; \
	} \
	struct __s##className \
	{ \
		__s##className() \
		{ \
			ObjectFactory().AddObjectFunctor(className##MakeInstance, \
				#className); \
		} \
	}; \
	const WRTTI className::m_RTTI(#className, &baseClass::m_RTTI); \
	static __s##className __impl##className;
