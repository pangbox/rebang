#pragma once

#include <memory>
#include <deque>
#include <queue>
#include "actor.h"
#include "work.h"
#include "anotherhand.h"
#include "scenemanager.h"
#include "tooltip.h"
#include "frcmdtarget.h"

static const char* s_taskNames[] = { "CLobbyTask", "CShopTask",
	"CRealMyRoomTask", "ntAvatarChatTask", "CGolfTask" };
class IActor;
class CBackGround;
class Fresh;
class FrForm;
class FrGaugeBar;
class WReceivedPacket;
struct sActor;

class CTaskDoc : public IObject
{
	friend class CTaskManager;

public:
	CTaskDoc();
	virtual ~CTaskDoc();

	CTask* GetTask() const;

protected:
	CTask* m_pTask;
};

class CTask : public IObject, public CHandOwner, public FrCmdTarget
{
	DECLARE_OBJECT(CTask);
	friend class CTaskManager;

public:
	struct TIP_ITEM
	{
		int id;
		std::string text;
	};

	CTask();
	virtual ~CTask();

	IActor* AddActor(const char* className, const char* name, int skipLayer,
		const std::string& option);
	IActor* GetActor(const char* name);
	void PushMsg(void* pData);
	void SetInited(bool bInited);
	IActor* GetMainActor() const { return m_pMainActor; }
	CTaskDoc* GetDocument() const { return m_pDoc; }

	virtual void ApplyScreenSize();
	virtual void Register() { }
	virtual void Init(const char* wallPaper);
	virtual void PreLoadInit();
	virtual void Reload();
	virtual void ProcessNetSyncActor(bool bForce, float step, int ballState);
	virtual void Process(float delta);
	virtual void Display();
	virtual void Destroy();
	virtual void SendGlobalMsg(MsgObject msg);
	virtual void SendMsgToMainActor(MsgObject msg);
	virtual int OnPacket(WReceivedPacket& packet) { return 0; }
	virtual void OnInitFinished() { }

	bool IsInited() { return m_taskFlag.GetFlag(2); }
	void SetPause(bool bPause) { m_bPause = bPause; }
	bool IsRecieveWhisper() { return m_bRecieveWhisper; }
	void SetWhisper(bool bWhisper) { m_bRecieveWhisper = bWhisper; }

protected:
	virtual void OnProcessWhileLoading(float delta);
	virtual void OnDisplayWhileLoading();
	virtual void Load();
	virtual void PreserveBack(const char* name);
	virtual void RestorePreserved();
	virtual void OnAnotherHand(int key, void* pParam);
	void SetMainActor(IActor* pActor);
	void SetProgressBar(bool bProgressBar);
	void SetLoadingBackGround(const std::string& name);
	FrForm* LoadTipText(int a, int b);
	bool OnTipDlgResult(int result, FrForm* pForm);
	bool OnOverlappedDlg(int result, FrForm* pForm);
	int OnPacketCommon(WReceivedPacket& packet);
	void SetTip(bool bTip) { m_bTip = bTip; }

private:
	int _RemoveWareHouseItem(unsigned long uid, IActor* pActor, int count);
	int _RemoveWareHouseItem(unsigned long uid, const char* actorName,
		int count);
	void _ReFreshMyRoomGift(bool bRefresh, std::string name);

protected:
	static IActor* ms_pDummyActor;
	bool m_bSyncLoad;
	WList<sActor*> m_actorList;
	IActor* m_pMainActor;
	CTaskDoc* m_pDoc;
	bool m_bLobbyScreenSize;
	std::string m_loadingBackGround;
	bool m_bProgressBar;
	bool m_bTip;
	Fresh* m_pLoadingFresh;
	FrForm* m_pTipForm;
	FrGaugeBar* m_pProgressGauge;
	CAnotherHand m_anotherHand;
	WFlags m_taskFlag;
	bool m_bRecieveWhisper;
	bool m_bPause;
	float m_netSyncTime;
};

struct sLoadParam
{
	WList<sActor*>* pActorList;
	int actorNum;
	int loadedNum;
	bool bInitialized;
	bool bLoaded;
};

extern sLoadParam g_LoadParam;

class CTaskManager : public WSingleton<CTaskManager>
{
public:
	struct sPostMsg
	{
		std::string target;
		MsgObject msg;
	};

	CTaskManager();
	virtual ~CTaskManager();

	void CreateTask(const char* task, const char* doc, bool bPreserve);
	void ChangeTask(const char* task, const char* doc, bool bPreserve);
	void ClearTask();
	void PreProcess();
	void ProcessTask(float delta);
	void DisplayTask() const;
	CTask* GetCurrentTask() const;
	CTask* GetPreservedTask() const { return m_pPreservedTask; }
	bool IsTaskChanging() { return m_bChangeTask; }

	int PostMsg(const IActor* sender, const char* target, int message,
		int param1, int param2, int param3, unsigned long time);
	void AddNewItemToDoc(unsigned int type, WReceivedPacket& packet);

	void NaviOpenLayout(const char* task, const char* doc, const char* actor,
		const char* layout);
	void NaviGoForward();
	void NaviGoBackward();
	bool IsNaviForwardEnable();
	bool IsNaviBackwardEnable();
	void NaviDisplayDebugInfo();

	void SetWork(CWork* pWork) { m_pWork.reset(pWork); }
	void ReleaseWork() { delete m_pWork.release(); }

protected:
	struct history_t
	{
		const char* task;
		const char* doc;
		const char* actor;
		const char* layout;
	};
	typedef std::list<history_t> HISTORY_LIST;
	typedef HISTORY_LIST::iterator HISTORY_IT;

	void NaviAddHistory(const char* task, const char* doc, const char* actor,
		const char* layout);
	void NaviClearHistory();
	bool IsNaviForwardSameHistory(const char* layout);

	bool m_bChangeTask;
	bool m_bPreserveTask;
	const char* m_nextTaskName;
	const char* m_nextDocName;
	CTask* m_pCurrentTask;
	CTask* m_pPreservedTask;
	CBackGround* m_pFadeBg;
	float m_fadeAlpha;
	std::queue<sPostMsg> m_postMsgQueue;
	HISTORY_LIST m_history;
	HISTORY_IT m_curHistory;
	int m_naviDirection;
	std::auto_ptr<CWork> m_pWork;
};

namespace ranktype
{
	struct sListInfo
	{
		unsigned long uid;
		unsigned long rank;
		unsigned long preRank;
		unsigned long value;
		unsigned short level;
		unsigned char unknown;
		std::string id;
		std::string nickname;
	};
}

typedef std::list<ranktype::sListInfo>::iterator RANKLIST_IT;

inline int AfxPostMsg(const IActor* sender, const char* target, int message,
	int param1, int param2, int param3, unsigned long time)
{
	return CTaskManager::Instance()->PostMsg(sender, target, message, param1,
		param2, param3, time);
}

inline CTask* AfxGetTask()
{
	if (!CTaskManager::IsInstantiated())
		return NULL;
	return CTaskManager::Instance()->GetCurrentTask();
}

inline CTaskDoc* AfxGetDocument()
{
	CTask* task = AfxGetTask();
	return task ? task->GetDocument() : NULL;
}

inline CTask* AfxGetPreservedTask()
{
	if (CTaskManager::Instance() == NULL)
		return NULL;
	return CTaskManager::Instance()->GetPreservedTask();
}
