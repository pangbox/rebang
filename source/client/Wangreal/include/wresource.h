#pragma once
#include "baseobject.h"

class WResourceManager;

class WResource : public BaseObject
{
public:
	__forceinline WResource()
		: m_resrcMng(0)
	{
	}

	__forceinline virtual ~WResource() { }

protected:
	__forceinline WResourceManager* GetResrcManager() const
	{
		return m_resrcMng;
	}

private:
	WResourceManager* m_resrcMng;
};
