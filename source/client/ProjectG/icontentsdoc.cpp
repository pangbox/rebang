#include "stdafx.h"
#include "icontentsdoc.h"
#include "gatewaydoc.h"
#include "contentsdoc.h"

CContentsDoc::CContentsDoc()
{
	(void)&g_EPSILON;
	m_bInitialized = 1;
}

CContentsDoc::~CContentsDoc()
{
	Release();
}

bool CContentsDoc::Initialize()
{
	InsertContainer((localContentType_t)29, new CGiftBoxEvent, true);
	InsertContainer((localContentType_t)30, new CWorldTourEvent, false);
	InsertContainer((localContentType_t)33, new CHalloweenEvent, false);
	InsertContainer((localContentType_t)36, new CGiftBoxEvent, false);
	InsertContainer((localContentType_t)76, new CChristmasEvent, false);
	InsertContainer((localContentType_t)122, new CTicketExchange, false);
	InsertContainer((localContentType_t)131, new CMissionEvent, false);
	InsertContainer((localContentType_t)146, new CPeriodContents, false);
	InsertContainer((localContentType_t)151, new _guild::CGuildInfo, false);
	InsertContainer((localContentType_t)156, new CChristmasSockEvent, false);
	InsertContainer((localContentType_t)157, new _gateway::CSecondPwdDoc,
		false);
	InsertContainer((localContentType_t)158, new _gateway::CGolfItemDoc, false);
	InsertContainer((localContentType_t)159, new _gateway::CTradeDoc, false);
	InsertContainer((localContentType_t)162, new CVectorSlideTestContainer,
		false);
	InsertContainer((localContentType_t)163, new CGimmickContainer, false);
	InsertContainer((localContentType_t)160, new _gateway::CSpecialBoxDoc,
		false);
	return m_bInitialized == 1 ? true : false;
}

bool CContentsDoc::Release()
{
	bool result = true;
	std::map<localContentType_t, IContentsDataContainer*>::iterator it;
	for (it = m_containerMap.begin(); it != m_containerMap.end(); ++it)
	{
		if ((*it).second)
		{
			if (!(*it).second->Release())
				result = false;
			delete (*it).second;
			(*it).second = NULL;
		}
	}
	m_containerMap.clear();
	return result;
}

int CContentsDoc::InsertContainer(localContentType_t type,
	IContentsDataContainer* pContainer, bool bForce)
{
	int result = 1;
	if ((IsLocalContent(type) && pContainer) || bForce == true)
	{
		if (pContainer->Initialize())
			m_containerMap.insert(std::make_pair(type, pContainer));
		else
			result = 0;
	}
	else
	{
		if (pContainer)
			delete pContainer;
		else
			result = 0;
	}
	m_bInitialized &= result;
	return result;
}

IContentsDataContainer* CContentsDoc::GetContainer(localContentType_t type)
{
	std::map<localContentType_t, IContentsDataContainer*>::iterator it;
	it = m_containerMap.find(type);
	if (it != m_containerMap.end())
		return (*it).second;
	return NULL;
}
