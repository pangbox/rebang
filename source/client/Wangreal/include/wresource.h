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

	__forceinline void SetResourceManager(WResourceManager* resrcMng)
	{
		m_resrcMng = resrcMng;
	}

protected:
	__forceinline WResourceManager* GetResrcManager() const
	{
		return m_resrcMng;
	}

private:
	WResourceManager* m_resrcMng;
};
