#include "stdafx.h"
#include "actor.h"
#include "taskmanager.h"

CActorFactory& ActorFactory()
{
	static CActorFactory factory;
	return factory;
}

CActorFactory::CActorFactory()
{
}

CActorFactory::~CActorFactory()
{
}

IActor* CActorFactory::CreateActor(const char* className)
{
	for (std::vector<const char*>::iterator it = m_actorClassList.begin();
		it != m_actorClassList.end(); ++it)
	{
		if (strcmp(*it, className) == 0)
		{
			IObject* object = ObjectFactory().CreateObject(*it);
			return DYNAMIC_CAST(IActor, object);
		}
	}
	return NULL;
}

void CActorFactory::AddActorClass(const char* className)
{
	m_actorClassList.push_back(className);
}

IMPLEMENT_OBJECT(IActor, IObject)

unsigned long IActor::m_defaultSkipLayer = 0;
unsigned long* IActor::m_pLayer = NULL;

IActor::IActor()
	: m_skipLayer(0), m_pTask(NULL), m_pDoc(NULL)
{
	m_dispPriority = DISP_PRIORITY_2;
	m_skipLayer = m_defaultSkipLayer;
}

IActor::~IActor()
{
}

CTaskDoc* IActor::GetDocument() const
{
	return m_pDoc;
}

CTask* IActor::GetTask() const
{
	return m_pTask;
}

void IActor::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
		StoreDelayedMsg(msg);
}

int IActor::PostMsg(const IActor* sender, const char* target, int message,
	int param1, int param2, int param3, unsigned long time)
{
	return AfxPostMsg(sender, target, message, param1, param2, param3, time);
}

int IActor::SendMsg(const IActor* sender, const char* target, int message,
	int param1, int param2, int param3, unsigned long time)
{
	MsgObject msg;
	msg.message = message;
	msg.sender = sender;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;
	msg.time = time;
	if (target)
	{
		IActor* actor = m_pTask->GetActor(target);
		if (actor)
			actor->HandleMsg(msg);
	}
	else
	{
		m_pTask->SendGlobalMsg(msg);
	}
	return 0;
}

void IActor::AddActor(const char* className, const char* name, bool bSkip,
	const std::string& option)
{
	m_pTask->AddActor(className, name, bSkip, option);
}

IActor* IActor::GetActor(const char* name) const
{
	return m_pTask->GetActor(name);
}

void IActor::StoreDelayedMsg(const MsgObject& msg) const
{
}

bool IActor::Skipped() const
{
	if (m_pLayer && (m_skipLayer & *m_pLayer))
		return true;
	return false;
}

void IActor::AddSkipList(unsigned long layer)
{
	m_skipLayer |= layer;
}

void IActor::RemoveSkipList(unsigned long layer)
{
	m_skipLayer &= ~layer;
}

class CDummy : public IActor
{
public:
	CDummy() { }
	virtual ~CDummy() { }
	DECLARE_OBJECT(CDummy)
	virtual void HandleMsg(const MsgObject& msg) { }
};

IMPLEMENT_ACTOR(CDummy, IActor)
