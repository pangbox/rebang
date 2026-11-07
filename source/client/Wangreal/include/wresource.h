#pragma once
#include "baseobject.h"

class WResourceManager;

class WResource : public BaseObject
{
public:
	WResource()
		: m_resrcMng(0)
	{
	}

	virtual ~WResource() { }

	void SetResourceManager(WResourceManager* resrcMng)
	{
		m_resrcMng = resrcMng;
	}

protected:
	WResourceManager* GetResrcManager() const { return m_resrcMng; }

private:
	WResourceManager* m_resrcMng;
};
