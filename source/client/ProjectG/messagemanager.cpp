#include "actor.h"
#include "messagemanager.h"

using namespace _systemmsg;

CMessageManager::CMessageManager()
{
}

CMessageManager::~CMessageManager()
{
}

bool CMessageManager::InsertMessage(eMsgIdentifier id, const char* msg)
{
	std::map<eMsgIdentifier, const char*>::const_iterator it =
		m_msgMap.find(id);
	if (it != m_msgMap.end())
	{
		return false;
	}

	m_msgMap.insert(std::make_pair(id, msg));

	return true;
}

bool CMessageManager::InsertMessageGroup(sMsgInfo* info, int num)
{
	int count = info[num].id;
	for (int i = 0; i < count; ++i)
	{
		std::map<eMsgIdentifier, const char*>::const_iterator it =
			m_msgMap.find(info[i].id);
		if (it != m_msgMap.end())
		{
			return false;
		}

		m_msgMap.insert(std::make_pair(info[i].id, info[i].msg));
	}

	return true;
}

const char* CMessageManager::GetMsg(eMsgIdentifier id)
{
	std::map<eMsgIdentifier, const char*>::const_iterator it =
		m_msgMap.find(id);
	if (it == m_msgMap.end())
	{
		return NULL;
	}

	return (*it).second;
}

void CMessageManager::NotifyNormalMessage(eMsgIdentifier id)
{
	const char* msg = GetMsg(id);

	if (msg == NULL)
	{
		msg =
			"\xc1\xf6\xc1\xa4\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xb8\xde\xbd\xc3\xc1\xf6 \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9.";
	}

	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35, (int)msg, 0, 0, 0, 0));
}

void CMessageManager::NotifyNormalMessage(const char* msg)
{
	if (msg == NULL)
	{
		return;
	}

	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35, (int)msg, 0, 0, 0, 0));
}
