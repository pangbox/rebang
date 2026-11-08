#include "stdafx.h"
#include "objectfactory.h"

const WRTTI IObject::m_RTTI("IObject", NULL);

IObject::IObject()
{
}

IObject::~IObject()
{
}

CObjectFactory& ObjectFactory()
{
	static CObjectFactory factory;
	return factory;
}

void CObjectFactory::Reset()
{
	for (std::map<const std::string, IObject*>::iterator it =
			 m_objectTypeList.begin();
		it != m_objectTypeList.end(); ++it)
		delete (*it).second;
	m_objectTypeList.erase(m_objectTypeList.begin(), m_objectTypeList.end());
	m_objectFuncList.erase(m_objectFuncList.begin(), m_objectFuncList.end());
}

void CObjectFactory::AddObjectType(IObject* object, const char* name)
{
	m_objectTypeList[name] = object;
}

void CObjectFactory::AddObjectFunctor(IObject* (*function)(), const char* name)
{
	m_objectFuncList[name] = function;
}
