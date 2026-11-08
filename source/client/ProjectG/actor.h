#pragma once

#include <string>
#include <vector>
#include <map>

class IActor;
class CTask;
class CTaskDoc;

class MsgObject
{
public:
	MsgObject() { }
	MsgObject(const IActor* _sender, int _message, int _param1, int _param2,
		int _param3, int _param4, int _param5)
		: sender(_sender),
		  message(_message),
		  param1(_param1),
		  param2(_param2),
		  param3(_param3),
		  param4(_param4),
		  param5(_param5)
	{
		time = 0;
	}

	const IActor* sender;
	int message;
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	unsigned long time;
};

struct sActor
{
	IActor* actor;
	bool flag;
	const char* name;
};

typedef std::allocator<const char*>::size_type ActorClassSize;

class CActorFactory
{
public:
	CActorFactory();
	~CActorFactory();

	void AddActorType(const std::string& name, const std::string& type)
	{
		m_actorTypeList[name] = type;
	}
	void AddActorClass(const char* className);
	IActor* CreateActor(const char* className);

private:
	std::map<const std::string, std::string> m_actorTypeList;
	std::vector<const char*> m_actorClassList;
};

CActorFactory& ActorFactory();

#define IMPLEMENT_ACTOR(className, baseClass) \
	IObject* className##MakeInstance() \
	{ \
		return new className; \
	} \
	struct __imp##className \
	{ \
		__imp##className() \
		{ \
			ObjectFactory().AddObjectFunctor(className##MakeInstance, \
				#className); \
			ActorFactory().AddActorClass(#className); \
		} \
	}; \
	const WRTTI className::m_RTTI(#className, &baseClass::m_RTTI); \
	static __imp##className __impl##className;

class IActor : public IObject
{
	DECLARE_OBJECT(IActor)
	friend class CTask;
	friend IActor* operator<<(IActor* pActor, const MsgObject& msg);

public:
	IActor();
	virtual ~IActor();

	int SendMsg(const IActor* sender, const char* target, int message,
		int param1, int param2, int param3, unsigned long time);
	int PostMsg(const IActor* sender, const char* target, int message,
		int param1, int param2, int param3, unsigned long time);
	void AddActor(const char* className, const char* name, bool bSkip,
		const std::string& option);
	IActor* GetActor(const char* name) const;

	static unsigned long m_defaultSkipLayer;
	static unsigned long* m_pLayer;

	virtual void OnPreLoadInit() { }
	virtual void OnLoad() { }
	virtual void OnInit() { }
	virtual void OnReload() { }
	virtual void OnProcess(float delta) { }
	virtual void OnDisplay() { }
	virtual void OnDestroy() { }
	virtual void OnPreserveBack(const char* name) { }
	virtual void OnRestorePreserved() { }

protected:
	virtual void HandleMsg(const MsgObject& msg);
	virtual void StoreDelayedMsg(const MsgObject& msg) const;

	enum eDispPriority
	{
		DISP_PRIORITY_0,
		DISP_PRIORITY_1,
		DISP_PRIORITY_2,
		DISP_PRIORITY_3,
		DISP_PRIORITY_4,
		DISP_PRIORITY_5
	};

	virtual eDispPriority GetDisplayPriority() { return m_dispPriority; }
	virtual bool Skipped() const;

	CTaskDoc* GetDocument() const;
	CTask* GetTask() const;
	void AddSkipList(unsigned long layer);
	void RemoveSkipList(unsigned long layer);

	WFlags m_flag;
	eDispPriority m_dispPriority;
	unsigned long m_skipLayer;
	CTask* m_pTask;
	CTaskDoc* m_pDoc;
};

inline IActor* operator<<(IActor* pActor, const MsgObject& msg)
{
	if (pActor == NULL)
		return NULL;
	pActor->HandleMsg(msg);
	return pActor;
}
