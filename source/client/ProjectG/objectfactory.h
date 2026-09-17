#pragma once
#include "rtti.h"

class IObject
{
public:
	virtual const WRTTI* GetRTTI() const = 0;
	virtual ~IObject();
};

class CObjectFactory
{
public:
	typedef IObject* (*ObjectFunctor)();
	void AddObjectFunctor(ObjectFunctor function, const char* name);
};

CObjectFactory& ObjectFactory();
