#include "minatl.h"
#include "taskmanager.h"
#include "projectg.h"
#include "../../shared/itemmanager.h"
#include "../../shared/sharedtables.h"
#include "s5/utilities.h"
#include "guildactor.h"
#include "gatewayactor.h"
#include "standinginline.h"
#include "guilddefine.h"
#include "bongdarishopdlg.h"
#include "buddymanager.h"
#include "capturedbg.h"
#include "cardboxdlg.h"
#include "cardsystem.h"
#include "clientsetting.h"
#include "contentsdoc.h"
#include "criticalsection.h"
#include "noticeboard.h"
#include "matchingsystem.h"
#include "frimagenoticedlg.h"
#include "fx.h"
#include "hatmanager.h"
#include "icontentsdoc.h"
#include "intrusion.h"
#include "lobbytask.h"
#include "rankingdlg.h"
#include "binstr.h"
#include "messengerdlg.h"
#include "messagemanager.h"
#include "mousecursor.h"
#include "netresourcemanager.h"
#include "matchhistory.h"
#include "tradedlg.h"
#include "cardremove.h"
#include "offlinetradedlg.h"
#include "onelineboard.h"
#include "parttidlist.h"
#include "pointeventdlg.h"
#include "rankinginfo.h"
#include "realmyroomtask.h"
#include "recycledlg.h"
#include "rssmanager.h"
#include "scratchdlg.h"
#include "securitykeymanager.h"
#include "serverdlg.h"
#include "shoptask.h"
#include "treasure.h"
#include "tutorialtask.h"
#include "ucccopydlg.h"
#include "user_info.h"
#include "avatar_avatar.h"
#include "itembuffactor.h"
#include "s5/supplypack.h"
#include "frgaugebar.h"
#include "golftask.h"
#include "chatmsg.h"
#include "hackingmanager.h"
#include "scenemanager.h"
#include "netsyncactor.h"
#include "golfball.h"
#include "privatetrade.h"
#include "skinnedtheme.h"
#include "uccmanager.h"
#include "avatar_task.h"
#include "commonutil.h"
#include "logininfo.h"
#include "ggc_wrapper.h"
#include "taskmain.h"
#include "xtdebug.h"

extern Fresh* g_pFresh;
extern HWND g_hwnd;
extern bool g_bQuit;
extern FrPointEventDlg* g_pPointEventDlg;
extern CCommodityStatusManager g_CommodityStatusManager;
extern int g_iRecvDisconCode;
int IsAdministrator(unsigned long uid);
void RequestUccInfo(sUserInfo& info);
void RequestUccInfo(sCharacterInfo& info);
void RequestUccInfo(ranktype::sRankUserInfo& info);
void RecvRSS0(int);
void __cdecl TextOut(WView* view, float x, float y, const char* format, ...);

// clang-format off
static const struct
{
	unsigned long code;
	const char* msg;
} s_equipErrorMsg[3] = {
	{ 0, "\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0." },
	{ 1, "\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0." },
	{ 2, "\xbd\xc3\xbb\xf3\xbd\xc4\xc0\xcc \xbe\xc6\xc1\xf7 \xc1\xbe\xb7\xe1\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xc6\xbc\xad, \xc4\xda\xbd\xba\xb8\xa6 \xba\xaf\xb0\xe6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xba\xaf\xb0\xe6\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0." },
};
// clang-format on

#pragma pack(push, 1)
struct sCompactRoomInfo
{
	char title[32];
	unsigned char bPublic : 1;
	unsigned char bAvailable : 1;
	unsigned char bIntrusion : 1;
	unsigned char reserved1 : 5;
	unsigned char nUserLimit;
	unsigned char nUserNum;
	unsigned char nHole;
	unsigned char gameType;
	unsigned short roomGuid;
	unsigned char mapType;
	unsigned long tidMatch;
	sGuildRoomInfo GuildInfo;
	unsigned char reserved2[5];
};
#pragma pack(pop)

struct sPrize
{
	int type;
	int rankFrom;
	int rankTo;
	unsigned long typeId;
	int count;
};

typedef std::list<sPrize*>::iterator PRIZE_LIST_IT;

struct sGiftBoxResult
{
	int type;
	unsigned long value;
	unsigned long reserved;
	unsigned long totalPang;
	unsigned long reserved2;
};

struct sLuckyPouchResult
{
	int type;
	unsigned long value;
	unsigned long count;
	int totalPang;
	unsigned long reserved;
};

struct sIntrest
{
	char group[33];
	unsigned char code;
	char name[33];
};

#pragma pack(push, 1)
struct sGMRoomInfo
{
	char title[32];
	unsigned char flags;
	unsigned char userNum;
	unsigned char userLimit;
	unsigned char holes;
	unsigned char gameType;
	unsigned short roomNumber;
	unsigned char map;
	unsigned char reserved[78];
	unsigned long serverId;
	unsigned char grade;
};
#pragma pack(pop)

#include "workalivecheck.h"
#include "winnernotice.h"

namespace _gateway
{
	struct sTradeBuyBase
	{
		sTradeItem item;
		sItemInfo itemInfo;
		unsigned char countingType;
	};

	struct sTradeSellParam
	{
		std::vector<sTradeItem> items;
		int result;
		__int64 pang;
	};

	struct sTradeBuyParam
	{
		std::vector<sTradeBuyBase> items;
		int result;
		__int64 pang;
		int unknown1;
		int unknown2;
	};

	struct sTradeMinusParam
	{
		std::vector<sTradeItem> items;
		std::string name;
		int state;
		unsigned long uid;
	};
}

sLoadParam g_LoadParam;

class GameGuardMsgHandler
{
public:
	struct _GG_AUTH_DATA
	{
		unsigned long dwIndex;
		unsigned long dwValue1;
		unsigned long dwValue2;
		unsigned long dwValue3;
	};

	GameGuardMsgHandler();
	~GameGuardMsgHandler();

	void Push(void* pData);
	void Process();

private:
	unsigned char m_reserved[16];
	std::list<_GG_AUTH_DATA> m_authList;
	CRITICAL_SECTION m_cs;
	bool m_bPushed;
};

GameGuardMsgHandler::GameGuardMsgHandler()
	: m_bPushed(false)
{
	InitializeCriticalSection(&m_cs);
}

GameGuardMsgHandler::~GameGuardMsgHandler()
{
	DeleteCriticalSection(&m_cs);
}

void GameGuardMsgHandler::Process()
{
	if (!m_bPushed)
		return;

	std::list<_GG_AUTH_DATA> authList;

	EnterCriticalSection(&m_cs);
	std::list<_GG_AUTH_DATA>::iterator it = m_authList.begin();
	for (; it != m_authList.end(); ++it)
	{
		authList.push_back(*it);
	}
	m_bPushed = false;
	m_authList.clear();
	LeaveCriticalSection(&m_cs);

	it = authList.begin();
	for (; it != authList.end(); ++it)
	{
		WSendPacket send(0x85);
		send.EncodeBuffer(&*it, sizeof(_GG_AUTH_DATA));
		send.Send(TO_GAME);
		CProjectG::Instance()->GameGuardLog("CSAuth2 responded.");
	}
}

void GameGuardMsgHandler::Push(void* pData)
{
	EnterCriticalSection(&m_cs);
	m_authList.push_back(*(_GG_AUTH_DATA*)pData);
	m_bPushed = true;
	LeaveCriticalSection(&m_cs);
}

static GameGuardMsgHandler g_gameGuardMsgHandler;

void RecvRSS0(int)
{
}

IMPLEMENT_OBJECT(CTask, IObject)

IActor* CTask::ms_pDummyActor = ActorFactory().CreateActor("Dummy");

CTask::CTask()
	: m_bSyncLoad(false),
	  m_actorList(16, 16),
	  m_pMainActor(NULL),
	  m_pDoc(NULL),
	  m_bLobbyScreenSize(true),
	  m_loadingBackGround(""),
	  m_bProgressBar(true),
	  m_bTip(true),
	  m_pLoadingFresh(NULL),
	  m_pProgressGauge(NULL),
	  m_bPause(false)
{
	m_taskFlag.Reset();
	m_netSyncTime = 0;
	m_bRecieveWhisper = true;
}

CTask::~CTask()
{
	for (sActor* p = m_actorList.Start(); p; p = m_actorList.Next())
	{
		if (p->actor)
			delete p->actor;
		delete[] p;
	}
	m_actorList.Reset();

	if (m_pDoc)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}
	if (m_pLoadingFresh)
	{
		delete m_pLoadingFresh;
		m_pLoadingFresh = NULL;
	}

	if (CFx::Instance())
		CFx::Instance()->ClearFxKeeper();
}

void CTask::SetMainActor(IActor* pActor)
{
	m_pMainActor = pActor;
}

bool CTask::OnTipDlgResult(int result, FrForm* pForm)
{
	m_pTipForm = NULL;
	m_pProgressGauge = NULL;
	return true;
}

FrForm* CTask::LoadTipText(int a, int b)
{
	cFile* hdl = g_resrcmng->GetCFile("tip.txt", 0xffff);
	if (!hdl)
		return NULL;

	std::list<TIP_ITEM> tipList;
	char line[8192];
	while (hdl->Scan("%n", line))
	{
		int j = 0;
		for (int i = 0; line[i]; ++i, ++j)
		{
			if (line[i] == '\\' && line[i + 1] == 'n')
			{
				line[j] = '\n';
				++i;
			}
			else
			{
				line[j] = line[i];
			}
		}
		line[j] = '\0';
		int len = (int)strlen(line);

		cTokenV token;
		token.Init(line, len);

		char* idStr;
		token.GetTokenWithoutFail(&idStr, " \t", 2);
		if (token.GetPos() + 1 > len)
		{
			CloseCFile(hdl);
			return NULL;
		}

		TIP_ITEM item;
		item.id = atoi(idStr);
		item.text = &line[token.GetPos()];
		tipList.push_back(item);
	}
	CloseCFile(hdl);
	do
	{
	} while (xtAlways<0>::Get());

	int index = rand() % tipList.size() + 1;
	FrForm* pForm =
		CreateForm<FrForm>(m_pLoadingFresh->GetManager(), this, "TIP", NULL);
	if (index && pForm)
	{
		std::list<TIP_ITEM>::iterator it = tipList.begin();
		for (; it != tipList.end() && index > 1; ++it)
			--index;

		pForm->SetMessage((*it).text.c_str(), false);
		pForm->EnableDrag(false);
		pForm->Open((FRESH_PFN_RESULT)&CTask::OnTipDlgResult, 8);
		return pForm;
	}
	return NULL;
}

void CTask::SetProgressBar(bool bProgressBar)
{
	m_bProgressBar = bProgressBar;
}

void CTask::SetLoadingBackGround(const std::string& name)
{
	m_loadingBackGround = name;
}

IActor* CTask::AddActor(const char* className, const char* name, int skipLayer,
	const std::string& option)
{
	IActor* pActor = ActorFactory().CreateActor(className);
	if (!pActor)
		return NULL;

	pActor->m_pDoc = m_pDoc;
	pActor->m_pTask = this;

	sActor* p = (sActor*)new char[strlen(className) + sizeof(sActor)];
	p->actor = pActor;
	p->name = name;
	p->actor->RemoveSkipList(skipLayer);
	p->flag = false;
	m_actorList.AddItem(p, p->name, false);

	if (!option.empty())
		pActor->HandleMsg(
			MsgObject(NULL, 10002, (int)option.c_str(), 0, 0, 0, 0));
	return pActor;
}

IActor* CTask::GetActor(const char* name)
{
	sActor* p = m_actorList.Find(name);
	if (!p)
		return NULL;
	return p->actor;
}

void SendPacket(unsigned char step)
{
	WSendPacket send(0x48);
	send.Encode1(step);
	send.Send(TO_GAME);
}

void CTask::OnAnotherHand(int key, void* pParam)
{
	if (key == 0)
	{
		sLoadParam* pLoad = (sLoadParam*)pParam;
		sActor* p = pLoad->pActorList->Start();
		while (p)
		{
			LogOut(0, "Load start %d\n", timeGetTime());
			if (IS_KINDOF(CGolfTask, this) && OnlinePlay() && !IsMassGame() &&
				pLoad->loadedNum % 3 == 0)
			{
				int step;
				if (pLoad->loadedNum == 23)
					step = 10;
				else
					step = pLoad->loadedNum / 3 + 2;
				SendPacket(step);
			}

			p->actor->OnLoad();

			LogOut(0, "Load end %d\n", timeGetTime());

			p = pLoad->pActorList->Next();
			++pLoad->loadedNum;
		}
		pLoad->bLoaded = true;
	}
}

void CTask::Load()
{
	g_LoadParam.pActorList = &m_actorList;
	g_LoadParam.actorNum = m_actorList.GetCount();

	m_pLoadingFresh = new Fresh;
	m_pProgressGauge = NULL;

	if (m_loadingBackGround != "")
		m_pLoadingFresh->Init("loading.xml", false,
			m_loadingBackGround.c_str());
	else
		m_pLoadingFresh->Init("loading.xml", false, NULL);

	m_pLoadingFresh->OpenLayout("LOADING", NULL, true);

	if (m_bTip)
	{
		m_pTipForm = LoadTipText(0, 0);
		if (m_pTipForm)
		{
			m_pProgressGauge = DYNAMIC_CAST(FrGaugeBar,
				m_pTipForm->FindChildByName("progress"));
			if (m_pProgressGauge)
			{
				m_pProgressGauge->SetRange(0, g_LoadParam.actorNum * 100, 0);
				m_pProgressGauge->SetDestPos(100, false);
			}
		}
	}

	if (m_bSyncLoad)
		OnAnotherHand(0, &g_LoadParam);
	else
		m_anotherHand.Work(this, 0, &g_LoadParam, false, 0);
}

void CTask::ApplyScreenSize()
{
	if (COption::Instance())
	{
		if (m_bLobbyScreenSize)
			COption::Instance()->vApplyLobbyScreenSize();
		else
			COption::Instance()->vApplyGameScreenSize();
	}
}

void CTask::Init(const char* wallPaper)
{
	if (m_pLoadingFresh)
	{
		delete m_pLoadingFresh;
		m_pLoadingFresh = NULL;
	}

	sActor* p = m_actorList.Start();
	while (p)
	{
		p->actor->OnInit();
		p = m_actorList.Next();
	}

	if (!IsInited())
	{
		SetInited(true);
		OnInitFinished();
	}

	g_pFresh->GetManager()->GetDesktop()->Init(g_view->GetWidth(),
		g_view->GetHeight());
	g_pFresh->GetManager()->GetDesktop()->SetWallPaper(wallPaper, true);

	if (!IS_EXACTKINDOF(CGolfTask, AfxGetTask()))
		CChatMsg::Instance()->SetActive(true, true, true);
}

void CTask::SetInited(bool bInited)
{
	if (bInited)
		m_taskFlag.Enable(2);
	else
		m_taskFlag.Disable(2);
}

void CTask::PreLoadInit()
{
	sActor* p = m_actorList.Start();
	while (p)
	{
		p->actor->OnPreLoadInit();
		p = m_actorList.Next();
	}
}

void CTask::Reload()
{
	sActor* p = m_actorList.Start();
	while (p)
	{
		p->actor->OnReload();
		p = m_actorList.Next();
	}
}

void CTask::Destroy()
{
	g_pFresh->CloseLayout();

	sActor* p = m_actorList.Start();
	while (p)
	{
		p->actor->OnDestroy();
		p = m_actorList.Next();
	}
}

void CTask::OnProcessWhileLoading(float delta)
{
	static int s_loadedNum = 0;

	if (m_pLoadingFresh)
	{
		m_pLoadingFresh->OnProcess(delta);

		if (s_loadedNum != g_LoadParam.loadedNum)
		{
			s_loadedNum = g_LoadParam.loadedNum;
			if (m_bProgressBar && m_pProgressGauge)
			{
				m_pProgressGauge->SetPos(g_LoadParam.loadedNum * 100 - 100);
				m_pProgressGauge->SetDestPos(g_LoadParam.loadedNum * 100,
					false);
			}
		}
	}
}

void CTask::PushMsg(void* pData)
{
	g_gameGuardMsgHandler.Push(pData);
}

void CTask::PreserveBack(const char* name)
{
	for (sActor* p = m_actorList.Start(); p; p = m_actorList.Next())
		p->actor->OnPreserveBack(name);
}

void CTask::RestorePreserved()
{
	for (sActor* p = m_actorList.Start(); p; p = m_actorList.Next())
		p->actor->OnRestorePreserved();
}

void CTask::ProcessNetSyncActor(bool bForce, float step, int ballState)
{
	sActor* p = m_actorList.Start();
	while (p)
	{
		if (!p->actor->Skipped())
		{
			NetSyncActor* pNetSync =
				(NetSyncActor*)p->actor->DynamicCast(&NetSyncActor::m_RTTI);
			if (pNetSync)
				pNetSync->NSA_Step(bForce, step, ballState);
		}
		p = m_actorList.Next();
	}
}

void CTask::Process(float delta)
{
	g_gameGuardMsgHandler.Process();

	m_pMainActor->OnProcess(delta);

	if (m_bPause)
		return;

	sActor* p = m_actorList.Start();
	while (p)
	{
		if (p->actor != m_pMainActor && !p->actor->Skipped())
			p->actor->OnProcess(delta);

		if (HackingManager::Instance())
			HackingManager::Instance()->CheckCrypticValue();

		p = m_actorList.Next();
	}

	bool bReset = false;
	if (GolfBall().m_playFrame == 0)
		bReset = true;

	if (delta > 0)
	{
		m_netSyncTime += delta;
		if (bReset)
			m_netSyncTime = 0.022f;

		while (m_netSyncTime > 0.02f)
		{
			if (GolfBall().m_playFrame == -1 ||
				GolfBall().m_playFrame == GolfBall().m_renderNum)
				ProcessNetSyncActor(false, 0.02f, GolfBall().m_playFrame);
			else if (GolfBall().m_playFrame == 0)
				ProcessNetSyncActor(true, 0.02f, GolfBall().m_playFrame);
			else
				LogOut(0, "ProcessNetSyncActor skip %d\n",
					GolfBall().m_playFrame);

			m_netSyncTime -= 0.02f;
		}
	}

	if (CSceneManager::Instance() && !CCapturedBg::Instance()->IsCaptured())
	{
		if (!IS_EXACTKINDOF(CShopTask, AfxGetTask()))
			CSceneManager::Instance()->Process(delta);
	}
}

void CTask::OnDisplayWhileLoading()
{
	if (m_pLoadingFresh)
		m_pLoadingFresh->OnDisplay(true);
}

void CTask::Display()
{
	for (int i = IActor::DISP_PRIORITY_0; i <= IActor::DISP_PRIORITY_5; ++i)
	{
		void* pos;
		sActor* p = m_actorList.Start(pos);
		while (p)
		{
			if (p->actor && p->actor->GetDisplayPriority() == i &&
				!p->actor->Skipped())
				p->actor->OnDisplay();
			p = m_actorList.Next(pos);
		}

		if (i == IActor::DISP_PRIORITY_2)
		{
			if (!CCapturedBg::Instance() ||
				!CCapturedBg::Instance()->IsCaptured())
			{
				if (CSceneManager::Instance())
				{
					CSceneManager::Instance()->Prerender(g_view);
					CSceneManager::Instance()->Render(g_view, 0xffffffff);
				}

				if (!m_taskFlag.GetFlag(1))
					g_view->Clear(0, 2);
			}
		}
		else if (i == IActor::DISP_PRIORITY_4 || i == IActor::DISP_PRIORITY_1)
		{
			g_view->Flush(0);
		}
	}
}

void CTask::SendGlobalMsg(MsgObject msg)
{
	m_pMainActor << msg;

	void* pos;
	sActor* p = m_actorList.Start(pos);
	while (p)
	{
		if (p->actor != m_pMainActor)
			p->actor << msg;
		p = m_actorList.Next(pos);
	}
}

void CTask::SendMsgToMainActor(MsgObject msg)
{
	m_pMainActor << msg;
}

extern int g_iRecvPacketType;

int CTask::OnPacketCommon(WReceivedPacket& packet)
{
	packet.Reset();
	unsigned short type = packet.Decode2();
	g_iRecvPacketType = type;

	switch (type)
	{
	case 0x1d3:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long sendType = packet.Decode4();
			unsigned long value = packet.Decode4();

			WSendPacket send(sendType);
			send.Encode4(Doc()->m_myInfo.info.dwGuildId);
			send.Encode4(value);
			send.Send(TO_MSN);
		}
		break;
	case 0x187:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			IActor* pActor = AfxGetTask()->GetActor("PrivateTrade");
			if (pActor == NULL)
				break;

			CPrivateTrade* pTrade = DYNAMIC_CAST(CPrivateTrade, pActor);
			pTrade->OnPacket(&packet);
		}
		break;
	case 0x19c:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 2, 0, 0, 0, 0, 0);

			S5::FrSupplyPackDlg* pDlg = NULL;
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildForm(
				"FrSupplyPackDlg");
			if (pWnd && IS_EXACTKINDOF(S5::FrSupplyPackDlg, pWnd))
				pDlg = DYNAMIC_CAST(S5::FrSupplyPackDlg, pWnd);

			unsigned char result = packet.Decode1();
			if (result == 0)
			{
				if (pDlg == NULL)
					break;

				pDlg->Close((eFormRet)2, true);
				FrForm* pForm =
					CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify");
				pForm->SetMessage(
					"\xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					false);
				pForm->Open(NULL, 3);
				break;
			}

			unsigned long useTid = packet.Decode4();
			unsigned long getTid = packet.Decode4();
			unsigned long getCount = packet.Decode4();

			std::list<sItemInfo>::iterator it = std::find(
				Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(), useTid);
			if (it != Doc()->m_myItemList.end())
			{
				(*it).Common[0]--;
				if ((*it).Common[0] <= 0)
					Doc()->m_myItemList.erase(it);

				AfxGetTask()->GetMainActor()
					<< MsgObject(NULL, 0xdd, 0, 0, 0, 0, 0);
			}

			if ((getTid & 0xfc000000) != 0x14000000)
				break;

			std::list<sItemInfo>::iterator itBall = std::find(
				Doc()->m_ballList.begin(), Doc()->m_ballList.end(), getTid);
			if (itBall != Doc()->m_ballList.end())
			{
				unsigned int added = getCount - (*itBall).Common[0];
				(*itBall).Common[0] = (short)getCount;

				if (pDlg)
					pDlg->UpdateData(added);
			}
		}
		break;
	case 0x11b:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char mode = packet.Decode1();
			unsigned char step = packet.Decode1();

			if (mode == 3)
				packet.DecodeBuffer(Doc()->m_tutorialComplete, 12);

			if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
				AfxPostMsg(NULL, "TutorialMain", 0x151, mode, step, 0, 0);
		}
		break;
	case 0x112:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long code = packet.Decode4();
			if (code >= 3)
				break;

			if (code != s_equipErrorMsg[code].code)
				break;

			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetTimeLimit(3.0f);
			pForm->SetMessage(s_equipErrorMsg[code].msg, false);
			pForm->Open(NULL, 0);
		}
		break;
	case 0xf0:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char value = packet.Decode1();
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x6b, value, 0, 0, 0, 0));
		}
		break;
	case 0x75:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x2b, 1, 0, 0, 0, 0));

			Doc()->m_expRate = packet.Decode4();
			if (GalleryMode())
			{
				Doc()->ClearRivalVars(true);
				break;
			}

			Doc()->ClearRivalVars(false);

			unsigned long guidCaddie = Doc()->m_myInfo.userEquip.guidCaddie;
			CSharedDoc* pDoc = Doc();

			std::map<unsigned int, sCaddieInfo>::const_iterator it =
				pDoc->m_caddieMap.find(guidCaddie);
			if (it != pDoc->m_caddieMap.end())
			{
				IFF_STRUCT::sCaddie* pCaddie =
					ItemManager()->FindCaddie((*it).second.tid);
				if ((*it).second.Rent_flag && (*it).second.Remain_Date == 0 &&
					pCaddie && pCaddie->MonthlyFee > 0)
					guidCaddie = 0;
			}

			WSendPacket send((enumClientPacket)0xc);

			unsigned long equip[4];
			equip[0] = Doc()->m_myInfo.userEquip.guidChar;
			Doc()->m_myInfo.userEquip.guidCaddie = guidCaddie;
			equip[1] = guidCaddie;
			equip[2] = Doc()->m_myInfo.userEquip.guidClubSet;
			equip[3] = Doc()->m_myInfo.userEquip.tidBall;

			send.Encode1(7);
			send.EncodeBuffer(equip, sizeof(equip));
			send.Send(TO_GAME);

			if (Doc()->m_roomInfo.realGameType == 14)
			{
				CTaskManager::Instance()->CreateTask("CLobbyTask", "CGolfDoc",
					false);
				Doc()->m_roomInfo.gameType = 4;
			}
		}
		break;
	case 0x74:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			sUserInfo userInfo;
			sUserInfoExt userInfoExt;

			packet.Decode1();
			int count = packet.Decode1();

			if (!GalleryMode() && IsMassGame(Doc()->m_roomInfo.gameType))
			{
				Doc()->m_myInfo.info.angelicWings = packet.Decode1();
				Doc()->m_myInfo.info.angelicWingsEffect = packet.Decode1();
				Doc()->m_myInfo.info.devilWings = packet.Decode1();

				if (IsLocalContent(S4_CARD_SYSTEM))
				{
					_SYSTEMTIME serverTime;
					packet.DecodeBuffer(&serverTime, sizeof(serverTime));
					Doc()->SetServerTime(serverTime);

					CCardManager::Instance()->m_playerSlot[0] = 0;

					std::map<unsigned int, sCharacterInfo>::iterator it =
						Doc()->m_charMap.find(
							Doc()->m_myInfo.userEquip.guidChar);
					if (it != Doc()->m_charMap.end())
						CCardManager::Instance()->m_charInfo[0] = (*it).second;
				}

				Doc()->SetEquipCharCaddieInfoFromMyInfo();
			}
			else
			{
				CPartTidList partTids;
				int slot = 1;

				if (!IsLocalContent((localContentType_t)0x70) &&
					IsLocalContent(S4_CARD_SYSTEM))
					for (int i = 1; i < 4; ++i)
						Doc()->m_cardAbilityList[i].clear();

				for (int i = 0; i < count; ++i)
				{
					if (IsLocalContent((localContentType_t)0x70))
					{
						packet.DecodeBuffer(&userInfoExt, sizeof(sUserInfoExt));
						userInfo = userInfoExt;
						Doc()->m_userInfo[i] = userInfoExt;
					}
					else
					{
						packet.DecodeBuffer(&userInfo, sizeof(sUserInfo));
						Doc()->m_userInfo[i] = userInfo;
					}

					if (IsLocalContent(S4_UCC))
						RequestUccInfo(userInfo);

					if (IsLocalContent(S4_CARD_SYSTEM) &&
						!IsLocalContent((localContentType_t)0x70))
					{
						CCardManager::Instance()->m_playerSlot[i] = 0xff;

						_SYSTEMTIME serverTime;
						packet.DecodeBuffer(&serverTime, sizeof(serverTime));
						Doc()->SetServerTime(serverTime);

						bool bMine = false;
						if (!stricmp(userInfo.info.sNick, MyNick()))
							bMine = true;

						int cardCount = packet.Decode1();

						sSCardAvilityPeriodInfo cardInfo;
						memset(&cardInfo, 0, sizeof(cardInfo));

						for (int j = 0; j < cardCount; ++j)
						{
							packet.DecodeBuffer(&cardInfo, sizeof(cardInfo));
							if (!bMine)
								Doc()->m_cardAbilityList[slot].push_back(
									cardInfo);
						}

						if (bMine)
						{
							CCardManager::Instance()->m_playerSlot[i] = 0;
							CCardManager::Instance()->m_charInfo[0] =
								userInfo.charInfo;
						}
						else
						{
							CCardManager::Instance()->m_charInfo[slot] =
								userInfo.charInfo;
							CCardManager::Instance()->m_playerSlot[i] = slot++;
						}
					}

					partTids.SetTids(&Doc()->m_userInfo[i].charInfo,
						userInfo.stat.Level);

					IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(
						Doc()->m_userInfo[i].caddieInfo.tid);
					if ((pCaddie &&
							Doc()->m_userInfo[i].stat.Level <
								pCaddie->c.Level) ||
						(Doc()->m_userInfo[i].caddieInfo.Rent_flag &&
							Doc()->m_userInfo[i].caddieInfo.Remain_Date == 0 &&
							pCaddie->MonthlyFee > 0))
						Doc()->m_userInfo[i].caddieInfo.tid = 0;

					IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(
						Doc()->m_userInfo[i].clubInfo.tid);
					if (pClubSet &&
						((Doc()->m_userInfo[i].stat.Level < pClubSet->c.Level &&
							 !pClubSet->c.IsUnderLvl) ||
							(Doc()->m_userInfo[i].stat.Level >
									pClubSet->c.Level &&
								pClubSet->c.IsUnderLvl)))
						Doc()->m_userInfo[i].clubInfo.tid = 0x10000000;
				}
			}

			std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
			for (; it != Doc()->m_slotList.end(); ++it)
			{
				if ((*it).bMaster)
					(*it).bReady = 1;
				else
					(*it).bReady = 0;
			}

			if (IsLocalContent(S4_CARD_SYSTEM))
				CCardManager::Instance()->CalcCardPeriodAndStatus();
		}
		break;
	case 0x50:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			SendMessage(g_hwnd, 0xadb3, 0, 0);

			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"bongdari_shop");
			if (!pWnd)
			{
				FrForm* pForm = DYNAMIC_CAST(FrForm, pWnd);
			}
			else
				((FrBongdariShopDlg*)pWnd)->Clear();

			if (CUserInfo::Instance())
				CUserInfo::Instance()->Clear();

			if (IsLocalContent(S4_TREASURE_HUNTER))
				CTHunter::Instance()->SetTreasurePoint(0);

			CIconToolTip::GetInstance()->GetInstance()->ClearToolTip();

			SetCurMap(packet.Decode1());
			SetGameType(packet.Decode1());
			SetHoleType(packet.Decode1());
			SetHoles(packet.Decode1());
			Doc()->m_roomType = packet.Decode4();
			SetShotTimeLimit(packet.Decode4());
			SetGameTimeLimit(packet.Decode4());

			if (!IsMassGame())
			{
				for (int i = 0; i < 4; ++i)
				{
					if (Doc()->m_userInfo[i].info.dwGuid == MyGuid(false))
						break;
				}
			}

			for (unsigned char i = 0; i < 18; ++i)
			{
				Doc()->m_holeRandom[i] = packet.Decode4();
				Doc()->m_courseMap[i] =
					(unsigned char)(Doc()->m_holeRandom[i] % 3);

				if (Doc()->m_roomInfo.realGameType == 14)
					Doc()->m_pinIndex[i] = packet.Decode1();

				Doc()->m_holeOrder[i] = packet.Decode1();
			}

			memset(Doc()->m_holePar, 0, sizeof(Doc()->m_holePar));

			if (Doc()->m_roomInfo.realGameType == 14)
				SetCurMap(Doc()->m_pinIndex[0]);

			CGimmickContainer* pGimmick = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0xa3,
				pGimmick);
			pGimmick->LoadFromPacket(packet);

			if (Doc()->m_golfGame.gameType != 10 && IsMassGame())
				Doc()->m_approachStartTime = g_CurrentTime;

			Doc()->m_roomSetupStage = 0;
			Doc()->LogGstUpdateHole(-1, 0xff, -1, -1);

			if (IsLocalContent((localContentType_t)0x17))
				Doc()->SetInitWearHalloweenItem();

			if (MESSENGER()->GetMyStatus() != 3)
				MESSENGER()->SetMyStatus((MSN::eStatusType)0);

			for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				it != Doc()->m_slotList.end(); ++it)
				(*it).bSleep = 0;

			Doc()->m_bGameOver = false;

			CMouseCursor::Instance()->SetMode(1);
			CMouseCursor::Instance()->SetActive(false);
			COption::Instance()->vSetMinimumScreenSize(0, 800, 600);
			CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc",
				false);
		}
		break;
	case 0x3e:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			std::string name;
			std::string text;
			unsigned char type = packet.Decode1();
			std::string msg;
			unsigned long color = 0xffffffff;
			bool bChat = false;

			unsigned char bWhisper = type >> 7;
			type &= 0x7f;

			packet.DecodeStr(name);
			packet.DecodeStr(text);

			Doc()->m_chatManager.FilteringHack(text.c_str(), false);

			bool bIgnore = false;
			switch (type)
			{
			case 0:
				if (strcmp(name.c_str(), Doc()->m_myInfo.info.sNick) == 0)
					color = 0xffffcc00;
				else
				{
					std::list<std::string>::iterator it =
						std::find(Doc()->m_ignoreList.begin(),
							Doc()->m_ignoreList.end(), name);
					if (it != Doc()->m_ignoreList.end())
					{
						bIgnore = true;
						break;
					}
				}

				msg = name + " : \b" + text;

				bChat = true;
				if (bWhisper)
					color = 0xff6eecfb;
				break;

			case 1:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc3\xa4\xb3\xce\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 5:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 2:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				Doc()->SetGoStopMode(true);
				break;

			case 8:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xd8\xbc\xad \xbd\xba\xc6\xae\xb6\xf3\xc0\xcc\xc5\xa9\xc6\xf7\xc0\xce\xc6\xae\xb0\xa1 1\xc6\xf7\xc0\xce\xc6\xae\xbe\xbf \xbf\xc3\xb6\xf3\xb0\xa9\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 3:
				msg =
					"\xb0\xad\xc5\xf0\xbf\xa1 \xb0\xfc\xc7\xd1 \xc5\xf5\xc7\xa5\xb0\xa1 \xba\xce\xb0\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 4:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xbc\xf6\xbd\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 6:
				msg = "\xbe\xcb\xb8\xb2 : " + name +
					" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc1\xa2\xbc\xd3\xc1\xdf\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.";
				color = 0xffff7878;
				break;

			case 7:
				msg = "\xbe\xcb\xb8\xb2 : " + text;
				color = 0xffff7878;
				break;

			case 10:
			{
				int pang = packet.Decode4();
				msg = "\xbe\xcb\xb8\xb2 : " +
					std::string(MakeStr(
						"%s\xb4\xd4\xb2\xb2\xbc\xad %d \xc6\xce \xc1\xd6\xb8\xd3\xb4\xcf\xb8\xa6 \xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						name.c_str(), pang));
				color = 0xffff7878;
			}
			break;

			default:
				msg = name + " : " + text;
				break;
			}

			if (IsLocalContent((localContentType_t)0x4f) &&
				Doc()->GetPlayingMode() == 2)
				bIgnore = true;

			if (Doc()->m_gameMode == 5)
				bIgnore = true;

			if (!bIgnore)
			{
				CChatMsg::Instance()->SetChatFacialUser(name);

				CChatMsg::Instance()->AddChatMsg(msg, color, false, bChat);
			}
		}
		break;
	case 0x3f:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			std::string msg;
			packet.DecodeStr(msg);
			CChatMsg::Instance()->AddChatMsg(msg, 0xffff7878, false, false);
		}
		break;
	case 0x40:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			std::string msg;
			packet.DecodeStr(msg);
			CNoticeBoard::Instance()->AddNotice(msg.c_str(), 1, false);
		}
		break;
	case 0x1d5:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long count = packet.Decode4();

			for (unsigned long i = 0; i < count; ++i)
			{
				std::string name("");

				int type = packet.Decode4();
				packet.DecodeStr(name);

				CWinnerNotice::Instance()->SetWinnerData(type, name);
			}
		}
		break;
	case 0x41:
		if (type)
			LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			struct sProtocol
			{
				unsigned long level;
				unsigned char gender;
				std::string nick;
			} protocol;
			protocol.level = packet.Decode4();
			protocol.gender = packet.Decode1();
			packet.DecodeStr(protocol.nick);

			const char* genderName[3] = { "\xb3\xb2", "\xbf\xa9",
				"\xba\xd2\xb8\xed" };
			char buf[1024];
			sprintf(buf,
				"%s(\xb7\xb9\xba\xa7:%d \xbc\xba\xba\xb0:%s)\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				protocol.nick.c_str(), protocol.level,
				genderName[protocol.gender]);

			CChatMsg::Instance()->AddChatMsg(buf, 0xffff7878, false, false);
		}
		break;
	case 0x42:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			switch (packet.Decode1())
			{
			case 0xd3:
			{
				FrWnd* pWnd =
					g_pFresh->GetManager()->GetDesktop()->FindChildForm(
						"server");
				if (pWnd && IS_KINDOF(FrServerDlg, pWnd))
				{
					unsigned char bFinish = packet.Decode1();
					if (bFinish == 0)
					{
						((FrServerDlg*)pWnd)->FinishAutoLogin();
					}
					else
					{
						((FrServerDlg*)pWnd)->BeginAutoLogin();
					}
				}
			}
			break;

			case 0xd2:
			{
				int count = packet.Decode4();

				FrWnd* pWnd =
					g_pFresh->GetManager()->GetDesktop()->FindChildForm(
						"server");
				if (pWnd && IS_KINDOF(FrServerDlg, pWnd))
					((FrServerDlg*)pWnd)->AddLoginReplyCount(count);
			}
			break;

			case 0:
			{
				FrWnd* pWnd = g_pFresh->GetDesktop()->FindChildForm("server");
				if (pWnd && IS_KINDOF(FrServerDlg, pWnd))
					((FrServerDlg*)pWnd)->AddLoginReplyCount(0x15);

				UccManager()->BuildDisplayClothesSet();

				Doc()->ClearInventory();

				if (IS_KINDOF(CShopTask, AfxGetTask()) == true)
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0xc5, 0, 0, 0, 0, 0));

				Doc()->m_packetNickname = packet.DecodeStr();
				Doc()->m_nicknameSuffix = packet.DecodeStr();

				sUserInfoExt userInfoExt;
				sUserInfo userInfo;
				if (IsLocalContent((localContentType_t)0x70))
				{
					packet.DecodeBuffer(&userInfoExt, sizeof(sUserInfoExt));
					userInfo = userInfoExt;
					memcpy(Doc()->m_userInfoExt, userInfoExt.ext,
						sizeof(Doc()->m_userInfoExt));
				}
				else
				{
					packet.DecodeBuffer(&userInfo, sizeof(sUserInfo));
				}

				RequestUccInfo(userInfo);

				_SYSTEMTIME serverTime;
				packet.DecodeBuffer(&serverTime, sizeof(serverTime));
				Doc()->SetServerTime(serverTime);

				unsigned char firstLogin = packet.Decode1();
				packet.Decode1();

				Doc()->SetBsUsableTimes(packet.Decode2());
				Doc()->SetBsUsableBonusTimes(packet.Decode2());
				Doc()->SetBsRemainedBonusTimes(packet.Decode2());

				if (IsLocalContent((localContentType_t)8))
					Doc()->SetFlagBlock(packet.Decode4());

				Doc()->m_controlServerService = packet.Decode4();
				unsigned long unused = packet.Decode4();

				Doc()->m_unused5060 = unused;

				unsigned long serverProperty = packet.Decode4();

				Doc()->m_serverProperty = serverProperty;

				GUILD_USER_INFO guildInfo;
				packet.DecodeBuffer(&guildInfo, sizeof(guildInfo));
				IActor* pGuildActor = AfxGetTask()->GetActor(s_guildActor[1]);

				pGuildActor
					<< MsgObject(NULL, 0x263, 6, (int)&guildInfo, 0, 0, 0);

				S5::UTIL::SetServerProperty(serverProperty);
				S5::THEME::SetSkinnedTheme(serverProperty);

				Doc()->m_myInfo.roomIndex = userInfo.roomIndex;
				Doc()->m_myInfo.info = userInfo.info;
				Doc()->m_myInfo.stat = userInfo.stat;
				Doc()->m_myInfo.trophy = userInfo.trophy;
				Doc()->m_myInfo.userEquip = userInfo.userEquip;
				memcpy(Doc()->m_myInfo.mapStat, userInfo.mapStat,
					sizeof(Doc()->m_myInfo.mapStat));
				memcpy(Doc()->m_myInfo.classicMapStat, userInfo.classicMapStat,
					sizeof(Doc()->m_myInfo.classicMapStat));

				NetResourceManager::Instance()->ResetGuildRSS();

				if (userInfo.info.dwGuildId)
					NetResourceManager::Instance()->RequestGuildRSS(
						userInfo.info.dwGuildId, RecvRSS0);

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 5, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x42, 0, 0, 0, 0, 0));

				if (firstLogin == 1)
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x43, 1, 0, 0, 0, 0));

				if (!COption::Instance()->gIsWhisperAllowed())
				{
					WSendPacket send((enumClientPacket)0x55);
					send.Encode1(0);
					send.Send(TO_GAME);
				}

				if (WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_MSN) &&
					MESSENGER()->GetComplete() == 4)
				{
					sUserPosition pos;
					pos.iServerGUID = Doc()->m_gameServerUID;
					wsprintf(pos.csChannelName, "NONE");
					pos.iRoomIdx = 0xffff;
					pos.iRoomType = -1;

					WSendPacket send(0x23);
					send.EncodeBuffer(&pos, sizeof(pos));
					send.Send(TO_MSN);
				}

				Doc()->SetPeriodDoc();
				Doc()->GetPeriodDoc()->Set_PreSettingContentsDate();
			}
			break;

			case 2:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 6, 0, 0, 0, 0));
				break;

			case 4:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xb, 0, 0, 0, 0));
				break;

			case 5:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 8, 0, 0, 0, 0));
				break;

			case 6:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 7, 0, 0, 0, 0));
				break;

			case 7:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 9, 0, 0, 0, 0));
				break;

			case 8:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xa, 0, 0, 0, 0));
				break;

			case 0xb:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xc, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbc\xad\xb9\xf6 \xb9\xf6\xc0\xfc\xc0\xcc \xb4\xd9\xb8\xa8\xb4\xcf\xb4\xd9. \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;

			case 0xe:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xe, 0, 0, 0, 0));
				break;

			case 0xd6:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xf, 0, 0, 0, 0));
				break;

			case 0xd7:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0x10, 0, 0, 0, 0));
				break;

			case 0x12:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0x15, 0, 0, 0, 0));
				break;

			case 0xc9:
				GetActor("Lobby") << MsgObject(NULL, 0x1e, 0x14, 0, 0, 0, 0);
				GetActor("Lobby") << MsgObject(NULL, 0x25,
					(int)"\xb0\xd4\xc0\xd3\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				break;

			case 0x10:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0x11, 0, 0, 0, 0));
				break;

			case 0x13:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0x12, 0, 0, 0, 0));
				break;

			case 0xdb:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0x13, 0, 0, 0, 0));
				break;

			case 3:
			case 0xe0:
			{
				int code = packet.Decode4();
				if (code == 0)
					break;

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xd, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)MakeStr("%s %d",
						_systemmsg::CMessageManager::Instance()->GetMsg(
							(_systemmsg::eMsgIdentifier)0x76),
						code),
					0, 0, 0, 0));
			}
			break;

			default:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1f, 0xd, 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x43:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned short oldLevel = Doc()->m_myInfo.stat.Level;
			unsigned long newTotal = 0, oldTotal = 0;
			unsigned long oldExp = Doc()->m_myInfo.stat.dwExp;

			packet.DecodeBuffer(&Doc()->m_myInfo.stat,
				sizeof(sPangYaUserStatistics));
			packet.DecodeBuffer(&Doc()->m_myInfo.trophy,
				sizeof(sTrophyStatistics));

			unsigned char mapIndex = packet.Decode1();
			if (mapIndex != 0xff)
				packet.DecodeBuffer(&Doc()->m_myInfo.mapStat[mapIndex],
					sizeof(sMapStatistics));

			mapIndex = packet.Decode1();
			if (mapIndex != 0xff)
				packet.DecodeBuffer(&Doc()->m_myInfo.classicMapStat[mapIndex],
					sizeof(sMapStatistics));

			std::map<unsigned long, sUserInfoTime>::iterator it;
			if (Doc()->FindUserInfoTimeGUID(MyGuid(false), it))
			{
				(*it).second.info.stat = Doc()->m_myInfo.stat;
				(*it).second.info.trophy = Doc()->m_myInfo.trophy;
			}

			newTotal = Doc()->m_myInfo.stat.Level
				? Doc()->m_levelTable[Doc()->m_myInfo.stat.Level - 1].totalExp
				: 0;
			oldTotal =
				oldLevel ? Doc()->m_levelTable[oldLevel - 1].totalExp : 0;
			int gainExp =
				Doc()->m_myInfo.stat.dwExp - oldExp - oldTotal + newTotal;

			if (gainExp > 0)
			{
				CADDIE_MAP_IT itCaddie = Doc()->m_caddieMap.find(
					Doc()->m_myInfo.userEquip.guidCaddie);
				if (itCaddie != Doc()->m_caddieMap.end())
				{
					sCaddieInfo& caddie = (*itCaddie).second;

					caddie.Exp += gainExp;

					for (int i = 0; i < 3; i++)
					{
						if (caddie.Level == i &&
							caddie.Exp >= Doc()->m_bonusPangTable[i].exp)
						{
							caddie.Level++;
							caddie.Exp -= Doc()->m_bonusPangTable[i].exp;
						}
					}
				}
			}
		}
		break;
	case 0x49:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			switch (packet.Decode1())
			{
			case 4:
			{
				unsigned long guid = packet.Decode4();

				sCharacterInfo charInfo;
				packet.DecodeBuffer(&charInfo, sizeof(sCharacterInfo));

				if (IsLocalContent(S4_UCC))
					RequestUccInfo(charInfo);

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x49, guid, (int)&charInfo, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x49, guid, (int)&charInfo, 0, 0, 0));
			}
			break;
			case 1:
			{
				unsigned long guid = packet.Decode4();

				sCaddieInfo caddieInfo;
				packet.DecodeBuffer(&caddieInfo, sizeof(sCaddieInfo));

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4a, guid, (int)&caddieInfo, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4a, guid, (int)&caddieInfo, 0, 0, 0));
			}
			break;
			case 3:
			{
				unsigned long guid = packet.Decode4();

				sClubInfo clubInfo;
				packet.DecodeBuffer(&clubInfo, sizeof(sClubInfo));

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4b, guid, (int)&clubInfo, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4b, guid, (int)&clubInfo, 0, 0, 0));
			}
			break;
			case 2:
			{
				unsigned long guid = packet.Decode4();
				unsigned long value = packet.Decode4();

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4c, guid, value, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4c, guid, value, 0, 0, 0));
			}
			break;
			case 5:
			{
				unsigned long guid = packet.Decode4();

				sMascotInfo mascotInfo;
				packet.DecodeBuffer(&mascotInfo, sizeof(sMascotInfo));

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4e, guid, (int)&mascotInfo, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x4e, guid, (int)&mascotInfo, 0, 0, 0));
			}
			break;
			case 6:
			{
				unsigned long guid = packet.Decode4();
				unsigned long value = packet.Decode4();

				float fValue = 0;
				packet.DecodeBuffer(&fValue, sizeof(fValue));

				if (IS_EXACTKINDOF(ntAvatarChatTask,
						CTaskManager::Instance()->GetPreservedTask()) &&
					guid != MyGuid(false))
					AfxGetPreservedTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x24e, guid, value, (int)fValue, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x24e, guid, value, (int)fValue, 0, 0));
			}
			break;
			}
		}
		break;
	case 0x4a:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
	case 0x7c:
	{
		unsigned short roomIndex = packet.Decode2();
		if (roomIndex != 0xffff)
		{
			AfxPostMsg(NULL, "AvatarChat", 8, 0, 0, 0, 0);
			AfxPostMsg(NULL, "AvatarChat", 2, 0, 0, 0, 0);
			break;
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		Doc()->m_myInfo.roomIndex = 0xffff;
		Doc()->m_myInfo.info.dwGalleryGuid = -1;

		CProjectG::Instance()->FromGolfToLobbySettings();

		if (!IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);

		if (Doc()->m_bReturnLobbyNotice)
		{
			AfxPostMsg(NULL, "Lobby", 2, 0, 0, 0, 0);
			AfxPostMsg(NULL, "Lobby", 0, (int)"ROOMLIST", 0, 0, 0);
		}
		else
		{
			AfxPostMsg(NULL, "Lobby", 2, 0, 0, 0, 0);
			AfxPostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0, 0, 0);
		}

		if (WSingleton<CSceneManager>::IsInstantiated())
			CSceneManager::Instance()->SetWeather(0, false);

		if (NET()->IsConnected(WNetworkSystem::NET_MSN) &&
			CMessengerInfo::Instance()->GetComplete() == 4)
		{
			Doc()->m_roomInfo.roomGuid = 0xffff;
			Doc()->m_roomInfo.gameType = 0xff;

			sUserPosition position;
			position.iServerGUID = Doc()->m_gameServerUID;
			position.iChannelUid = Doc()->m_curChannel.Uid;
			wsprintf(position.csChannelName, "%s", Doc()->m_curChannel.Name);
			position.iRoomIdx = 0xffff;
			position.iRoomType = -1;

			WSendPacket send(0x23);
			send.EncodeBuffer(&position, sizeof(position));
			send.Send(TO_MSN);

			if (CMessengerInfo::Instance()->GetMyStatus() != 3)
				CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)4);
		}

		CIntrusion::Instance()->FinishGame();
	}
	break;
	case 0x45:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned char count = packet.Decode1();
			unsigned char type = packet.Decode1();
			unsigned short result = packet.Decode2();

			sRoomInfo room;
			sCompactRoomInfo compact;

			if (result != 0xffff)
				break;

			std::list<sRoomInfo>& roomList = Doc()->m_roomList;
			switch (type)
			{
			case 0:
			{
				roomList.clear();

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 6, 0, 0, 0, 0, 0));

				for (unsigned char i = 0; i < count; i++)
				{
					if (IsLocalContent((localContentType_t)0x5e))
					{
						memset(&room, 0, sizeof(sRoomInfo));
						packet.DecodeBuffer(&compact, sizeof(sCompactRoomInfo));

						room.bPublic = compact.bPublic;
						room.bAvailable = compact.bAvailable;
						room.bIntrusion = compact.bIntrusion;
						room.roomGuid = compact.roomGuid;
						room.nUserNum = compact.nUserNum;
						room.nUserLimit = compact.nUserLimit;
						room.mapType = compact.mapType;
						room.gameType = compact.gameType;
						room.nHole = compact.nHole;

						strcpy(room.title, compact.title);

						room.GuildInfo = compact.GuildInfo;
					}
					else
					{
						packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					}

					Doc()->m_chatManager.FilteringHack(room.title, false);

					if (IsLocalContent(S3_INTRUSION))
						CIntrusion::Instance()->OnAccessRoom(room);

					roomList.push_back(room);

					std::list<sRoomInfo>::iterator last = roomList.end();
					--last;
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 4, (int)&(*last), 0, 0, 0, 0));

					if (room.roomGuid == Doc()->m_roomInfo.roomGuid)
					{
						Doc()->m_roomInfo = room;
						AfxGetTask()->SendMsgToMainActor(
							MsgObject(NULL, 0x10, 0, 0, 0, 0, 0));
					}
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xd, 0, 0, 0, 0, 0));
			}
			break;
			case 1:
			{
				std::list<sRoomInfo>::iterator it;
				for (unsigned char i = 0; i < count; i++)
				{
					if (IsLocalContent((localContentType_t)0x5e))
					{
						memset(&room, 0, sizeof(sRoomInfo));
						packet.DecodeBuffer(&compact, sizeof(sCompactRoomInfo));

						room.bPublic = compact.bPublic;
						room.bAvailable = compact.bAvailable;
						room.bIntrusion = compact.bIntrusion;
						room.roomGuid = compact.roomGuid;
						room.nUserNum = compact.nUserNum;
						room.nUserLimit = compact.nUserLimit;
						room.mapType = compact.mapType;
						room.gameType = compact.gameType;
						room.nHole = compact.nHole;
						room.tidMatch = compact.tidMatch;

						strcpy(room.title, compact.title);

						room.GuildInfo = compact.GuildInfo;
					}
					else
					{
						packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					}

					Doc()->m_chatManager.FilteringHack(room.title, false);

					if (IsLocalContent(S3_INTRUSION))
						CIntrusion::Instance()->OnAccessRoom(room);

					it = std::find(roomList.begin(), roomList.end(),
						room.roomGuid);
					if (it == roomList.end())
					{
						roomList.push_back(room);

						std::list<sRoomInfo>::iterator last = roomList.end();
						--last;
						AfxGetTask()->SendMsgToMainActor(
							MsgObject(NULL, 4, (int)&(*last), 0, 0, 0, 0));
					}
					else
					{
						*it = room;
					}

					if (room.roomGuid == Doc()->m_roomInfo.roomGuid)
					{
						Doc()->m_roomInfo = room;
						AfxGetTask()->SendMsgToMainActor(
							MsgObject(NULL, 0x10, 0, 0, 0, 0, 0));
					}
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xd, 0, 0, 0, 0, 0));
			}
			break;
			case 2:
			{
				std::list<sRoomInfo>::iterator it;
				for (unsigned char i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&room, sizeof(sRoomInfo));

					Doc()->m_chatManager.FilteringHack(room.title, false);

					if (IsLocalContent(S3_INTRUSION))
						CIntrusion::Instance()->OnAccessRoom(room);

					it = std::find(roomList.begin(), roomList.end(),
						room.roomGuid);
					if (it != roomList.end())
					{
						AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 5,
							(int)&(*it), sizeof(sRoomInfo), 0, 0, 0));
						roomList.erase(it);
					}
				}
			}
			break;
			case 3:
			{
				std::list<sRoomInfo>::iterator it;
				for (unsigned char i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&room, sizeof(sRoomInfo));

					Doc()->m_chatManager.FilteringHack(room.title, false);

					if (IsLocalContent(S3_INTRUSION))
						CIntrusion::Instance()->OnAccessRoom(room);

					it = std::find(roomList.begin(), roomList.end(),
						room.roomGuid);
					if (it != roomList.end())
						*it = room;

					if (room.roomGuid == Doc()->m_roomInfo.roomGuid)
					{
						Doc()->m_roomInfo = room;
						AfxGetTask()->SendMsgToMainActor(
							MsgObject(NULL, 0x10, 0, 0, 0, 0, 0));
					}
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xd, 0, 0, 0, 0, 0));
			}
			break;
			}
		}
		break;
	case 0x44:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned char type = packet.Decode1();
			unsigned char count = packet.Decode1();

			sBriefUserInfo info;
			switch (type)
			{
			case 4:
				Doc()->m_briefUserInfoMap.clear();
				GetActor("Lobby") << MsgObject(NULL, 0xb, 0, 0, 0, 0, 0);
			case 5:
			{
				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));

					if (Doc()->m_briefUserInfoMap.find(info.dwGuid) ==
						Doc()->m_briefUserInfoMap.end())
					{
						Doc()->m_briefUserInfoMap.insert(
							std::pair<const unsigned long, sBriefUserInfo>(
								info.dwGuid, info));
						std::map<unsigned long, sBriefUserInfo>::iterator it =
							Doc()->m_briefUserInfoMap.find(info.dwGuid);
						GetActor("Lobby") << MsgObject(NULL, 0xa,
							(int)&(*it).second, 0, 0, 0, 0);
					}
				}
			}
			break;
			case 1:
			{
				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));

					std::map<unsigned long, sBriefUserInfo>::iterator it =
						Doc()->m_briefUserInfoMap.find(info.dwGuid);
					if (it == Doc()->m_briefUserInfoMap.end())
					{
						Doc()->m_briefUserInfoMap.insert(
							std::pair<const unsigned long, sBriefUserInfo>(
								info.dwGuid, info));
						it = Doc()->m_briefUserInfoMap.find(info.dwGuid);
						GetActor("Lobby") << MsgObject(NULL, 0xa,
							(int)&(*it).second, 0, 0, 0, 0);
					}
					else
					{
						(*it).second = info;

						if (info.dwIdentity & 4)
						{
							if (info.state & 1)
							{
								if (info.state & 0x400)
									GetActor("Lobby") << MsgObject(NULL, 0xa,
										(int)&(*it).second, 0, 0, 0, 0);
							}
						}
					}
				}
			}
			break;
			case 2:
			{
				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));

					std::map<unsigned long, sBriefUserInfo>::iterator it =
						Doc()->m_briefUserInfoMap.find(info.dwGuid);
					if (it != Doc()->m_briefUserInfoMap.end())
					{
						if (info.dwIdentity & 4)
							(*it).second = info;

						GetActor("Lobby")
							<< MsgObject(NULL, 0xc, (int)&(*it).second,
								   sizeof(sBriefUserInfo), 0, 0, 0);

						if (info.dwIdentity & 4)
						{
							if (!(info.state & 0x400))
								Doc()->m_briefUserInfoMap.erase(it);
						}
						else
						{
							Doc()->m_briefUserInfoMap.erase(it);
						}
					}
				}
			}
			break;
			case 3:
			{
				packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));

				std::map<unsigned long, sBriefUserInfo>::iterator it =
					Doc()->m_briefUserInfoMap.find(info.dwGuid);
				if (it != Doc()->m_briefUserInfoMap.end())
				{
					(*it).second = info;

					if (info.roomIndex == Doc()->m_myInfo.roomIndex &&
						info.roomIndex != 0xffff)
					{
						for (std::list<sSlotInfo>::iterator itSlot =
								 Doc()->m_slotList.begin();
							itSlot != Doc()->m_slotList.end(); ++itSlot)
						{
							if ((*itSlot).dwGuid == info.dwGuid)
							{
								(*itSlot).dwIdentity = info.dwIdentity;
								(*itSlot).dwTitle = info.dwTitle;
								break;
							}
						}
					}
				}
			}
			break;
			}
		}
		break;
	case 0x4b:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			if (NET()->IsConnected(WNetworkSystem::NET_LOGIN))
			{
				WSendPacket send(3);
				send.Encode4(Doc()->m_gameServerUID);
				send.Send(TO_LOGIN);
			}

			unsigned char count = packet.Decode1();

			Doc()->m_channelList.clear();

			for (int i = 0; i < count; i++)
			{
				sChannelInfo channel;
				memset(&channel, 0, sizeof(sChannelInfo));
				packet.DecodeBuffer(&channel, sizeof(sChannelInfo));
				Doc()->m_channelList.push_back(channel);
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x1b, 0, 0, 0, 0, 0));
		}
		break;
	case 0x4c:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			switch (packet.Decode1())
			{
			case 1:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1c, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x20, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 3,
					(int)MakeStr(
						"[%s]\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						Doc()->m_curChannel.Name),
					0xffff7878, 0, 0, 0));

				if (Doc()->m_curChannel.Type & 0x4000)
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 3,
						(int)"[EVENT]\xc6\xce\xb9\xe8\xc6\xb2 5\xb0\xe6\xb1\xe2\xb8\xa6 \xc7\xcf\xbd\xc7 \xb6\xa7\xb8\xb6\xb4\xd9 1000\xc6\xce\xc0\xbb \xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9.",
						0xffff7878, 0, 0, 0));

				if (IsLocalContent(S3_MATCH_HISTORY))
					CMatchHistory::Instance()->LoginRequest(MyId());
				break;
			case 2:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x22,
					(int)"\xc3\xa4\xb3\xce\xc0\xc7 \xc1\xa4\xbf\xf8\xbc\xf6\xb0\xa1 \xc3\xca\xb0\xfa\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x39, 0, 0, 0, 0, 0));
				break;
			case 3:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x22,
					(int)"\xc3\xa4\xb3\xce\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x39, 0, 0, 0, 0, 0));
				break;
			case 4:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x22,
					(int)"\xc3\xa4\xb3\xce \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8 \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x39, 0, 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x4d:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->ClearCurrentChannel(false);

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xec, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x3a, 0, 0, 0, 0, 0));

			Doc()->m_roomList.clear();

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 6, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xeb, 0, 0, 0, 0, 0));

			COneLineBoard::Instance()->Clear();
		}
		break;
	case 0xf3:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->m_bReturnLobbyNotice = true;
			Doc()->m_bRebuildUnderBar = false;

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xeb, 0, 0, 0, 0, 0));

			if (!IS_KINDOF(CLobbyTask, AfxGetTask()))
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				AfxPostMsg(NULL, "Lobby", 0, (int)"ROOMLIST", 0, 0, 0);
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0, (int)"ROOMLIST", 0, 0, 0, 0));
			}
		}
		break;
	case 0xf4:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->m_bReturnLobbyNotice = false;
			Doc()->m_bRebuildUnderBar = false;

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xeb, 0, 0, 0, 0, 0));

			if (!IS_KINDOF(CLobbyTask, AfxGetTask()))
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);

			AfxPostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0, 0, 0);
		}
		break;
	case 0xf5:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xeb, 0, 0, 0, 0, 0));
		}
		break;
	case 0x106:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			int result = packet.Decode1();
			switch (result)
			{
			case 0:
			{
				std::string nick;
				packet.DecodeStr(nick);
				if (nick.length() >= 4 && nick.length() <= 16)
					strcpy(Doc()->m_myInfo.info.sNick, nick.c_str());
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xe7, (int)nick.c_str(), 0, 0, 0, 0));
				if (WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_MSN) &&
					CMessengerInfo::Instance()->GetComplete() == 4)
				{
					WSendPacket send(0x22);
					send.EncodeStr(nick);
					send.Send(TO_MSN);
				}
				break;
			}
			case 2:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 3:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xba \\c0xffff0000\\c\xbf\xb5\xb9\xae 4 - 16 \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb 2 - 8\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xcf\xb8\xe7, \\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 4:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.\n\xbb\xf3\xc1\xa1\xbf\xa1\xbc\xad \xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0, 0, 0));
				break;
			case 5:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			default:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x40, result, 0, 0, 0, 0));
			break;
		}
	case 0x4e:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			int result = packet.Decode4();
			switch (result)
			{
			case 0:
			{
				std::string nick;
				packet.DecodeStr(nick);
				__int64 bonusCash = packet.Decode8();
				if (nick.length() >= 4 && nick.length() <= 16)
					strcpy(Doc()->m_myInfo.info.sNick, nick.c_str());
				if (IsLocalContent((localContentType_t)0x82))
				{
				}
				else if (!IsPcBang())
				{
					std::list<sItemInfo>::iterator it =
						std::find(Doc()->m_myItemList.begin(),
							Doc()->m_myItemList.end(), 0x1a000003);
					if (it == Doc()->m_myItemList.end())
					{
						Doc()->m_bonusCash = bonusCash;
						Doc()->RefreshAllCookie();
					}
					else
					{
						(*it).Common[0]--;
						if ((*it).Common[0] <= 0)
							Doc()->m_myItemList.erase(it);
					}
				}
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf6\xb1\xdd\xba\xce\xc5\xcd 1\xc1\xd6\xc0\xcf \xc8\xc4 \xc0\xe7 \xba\xaf\xb0\xe6\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xe7, (int)nick.c_str(), 0, 0, 0, 0));
				if (WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_MSN) &&
					CMessengerInfo::Instance()->GetComplete() == 4)
				{
					WSendPacket send(0x22);
					send.EncodeStr(nick);
					send.Send(TO_MSN);
				}
				break;
			}
			case 2:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 3:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xba \\c0xffff0000\\c\xbf\xb5\xb9\xae 4 - 16 \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb 2 - 8\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xcf\xb8\xe7, \\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 4:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.\n\xbb\xf3\xc1\xa1\xbf\xa1\xbc\xad \xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0, 0, 0));
				break;
			case 5:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 1:
			case 6:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xba\xaf\xb0\xe6 \xbf\xe4\xc3\xbb\xc0\xcc \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 7:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xb4\xeb\xc8\xad\xb8\xed \xba\xaf\xb0\xe6\xc0\xcc \xc0\xcf\xbd\xc3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xdf\xb4\xdc\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 9:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc0\xcc\xc0\xfc \xb4\xeb\xc8\xad\xb8\xed \xba\xaf\xb0\xe6 \xc8\xc4 1\xc1\xd6\xc0\xcf\xc0\xcc \xc1\xf6\xb3\xaa\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 11:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xb4\xeb\xc8\xad\xb8\xed \xb8\xed\xc2\xfb\xc0\xbb \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			default:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x40, result, 0, 0, 0, 0));
			break;
		}
	case 0x4f:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			unsigned char result = packet.Decode1();
			switch (result)
			{
			case 0:
				Doc()->m_myInfo.info.school = packet.Decode4();
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc7\xd0\xb1\xb3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			default:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc7\xd0\xb1\xb3 \xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
			break;
		}
	case 0x6e:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			sCharacterInfo charInfo;
			CPartTidList tidList;
			int total = packet.Decode2();
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				packet.DecodeBuffer(&charInfo, sizeof(charInfo));
				tidList.SetTids(&charInfo, Doc()->m_myInfo.stat.Level);
				Doc()->AddChar(&charInfo);
			}
			if (total == count)
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x2c, 1, 0, 0, 0, 0));
			break;
		}
	case 0x6f:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int total = packet.Decode2();
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				sCaddieInfo caddie;
				packet.DecodeBuffer(&caddie, sizeof(caddie));
				Doc()->AddCaddie(&caddie);
				if (caddie.PCBangCaddie)
				{
					if (Doc()->m_myInfo.userEquip.guidCaddie)
					{
						std::map<unsigned int, sCaddieInfo>::const_iterator it =
							Doc()->m_caddieMap.find(
								Doc()->m_myInfo.userEquip.guidCaddie);
						if (it == Doc()->m_caddieMap.end())
						{
							if (Doc()->m_caddieMap.size() > 0)
							{
								it = Doc()->m_caddieMap.begin();
								Doc()->m_myInfo.userEquip.guidCaddie =
									(*it).first;
							}
						}
					}
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x68, 1, 0, 0, 0, 0));
				}
			}
			if (total == count)
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x2c, 2, 0, 0, 0, 0));
			break;
		}
	case 0xdf:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned char count = packet.Decode1();
			for (unsigned char i = 0; i < count; ++i)
			{
				sMascotInfo mascot;
				packet.DecodeBuffer(&mascot, sizeof(mascot));
				Doc()->AddMascot(&mascot);
				if (mascot.PCBangMascot)
				{
					strcpy(mascot.szMsg,
						COption::Instance()->GetOption()->mPCBangMascotMsg);
					Doc()->AddMascot(&mascot);

					WSendPacket send((enumClientPacket)0x97);
					send.Encode1(2);
					send.Encode4(mascot.guid);
					send.EncodeStr(mascot.szMsg);
					send.Send(TO_GAME);

					if (Doc()->m_myInfo.userEquip.guidMascot)
					{
						std::map<unsigned int, sMascotInfo>::const_iterator it =
							Doc()->m_mascotMap.find(
								Doc()->m_myInfo.userEquip.guidMascot);
						if (it == Doc()->m_mascotMap.end())
							Doc()->m_myInfo.userEquip.guidMascot = 0;
					}
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x68, 2, 0, 0, 0, 0));
				}
			}
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x2c, 0x10, 0, 0, 0, 0));
			break;
		}
	case 0x70:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			sUserEquip equip;
			packet.DecodeBuffer(&equip, sizeof(equip));
			Doc()->SetCurEquipInfo(&equip);
			break;
		}
	case 0x71:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			sItemInfo item;
			int total = packet.Decode2();
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				packet.DecodeBuffer(&item, sizeof(item));
				bool bNew = true;
				switch (item.tid >> 26)
				{
				case 2:
				{
					ITEM_LIST_IT it;
					if (IsLocalContent((localContentType_t)0x57) &&
						IsUccClothes(item.tid))
					{
						ITEM_LIST_IT it;
						for (it = Doc()->m_uccItemList.begin();
							it != Doc()->m_uccItemList.end(); it++)
						{
							if ((*it).tid == item.tid &&
								!strcmp((*it).UccIndex, item.UccIndex) &&
								(*it).Seq == item.Seq)
							{
								bNew = false;
								break;
							}
						}
					}
					for (it = Doc()->m_partsList.begin();
						it != Doc()->m_partsList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 4:
				{
					std::map<unsigned int, sItemInfo>::iterator itClub;
					for (itClub = Doc()->m_clubSetMap.begin();
						itClub != Doc()->m_clubSetMap.end(); itClub++)
					{
						if ((*itClub).second.guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 5:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_ballList.begin();
						it != Doc()->m_ballList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
					if (item.Common[0] > 19000)
					{
						if (item.Common[0] > 20000)
							Doc()->m_specialShotMark = 2;
						else if (Doc()->m_specialShotMark != 2)
							Doc()->m_specialShotMark = 1;
					}
				}
				break;
				case 6:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_myItemList.begin();
						it != Doc()->m_myItemList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
					if (item.Common[0] > 19000)
					{
						if (item.Common[0] > 20000)
							Doc()->m_specialShotMark = 2;
						else if (Doc()->m_specialShotMark != 2)
							Doc()->m_specialShotMark = 1;
					}
				}
				break;
				case 8:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_cadItemList.begin();
						it != Doc()->m_cadItemList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 9:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_setItemList.begin();
						it != Doc()->m_setItemList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 0xe:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_cutinList.begin();
						it != Doc()->m_cutinList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 0x1c:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_auxPartList.begin();
						it != Doc()->m_auxPartList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 0x1d:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_questDropList.begin();
						it != Doc()->m_questDropList.end(); it++)
					{
						if ((*it).guid == item.guid)
						{
							bNew = false;
							break;
						}
					}
				}
				break;
				case 0x12:
				{
					std::list<sFurniture_List>::iterator itFurniture;
					if (IsLocalContent((localContentType_t)0x56))
					{
						for (itFurniture = Doc()->m_furnitureList.begin();
							itFurniture != Doc()->m_furnitureList.end();
							itFurniture++)
						{
							if ((*itFurniture).id == item.guid)
							{
								bNew = false;
								break;
							}
						}
					}
				}
				break;
				}

				if (bNew)
				{
					switch (item.tid >> 26)
					{
					case 2:
						if (IsLocalContent((localContentType_t)0x57) &&
							IsUccClothes(item.tid))
						{
							if (item.status & 1)
							{
								sUccClothes clothes;
								clothes.id = item.guid;
								clothes.typeId = item.tid;
								strncpy(clothes.uccIndex, item.UccIndex, 9);
								strncpy(clothes.uccName, item.ItemName, 41);
								clothes.date = item.ItemDate;
								clothes.downloadCount = 0;
								UccManager()->AddClothes(1, clothes, true,
									false);
							}
							else if (item.status & 2)
							{
								NetResourceManager::Instance()
									->DownloadUccTempClothes(item.tid,
										item.UccIndex, item.Seq, false);
							}
							Doc()->m_uccItemList.push_back(item);
						}
						else
						{
							Doc()->AddMyPartsList(item);
						}
						break;
					case 4:
						Doc()->m_clubSetMap.insert(
							std::pair<const unsigned int, sItemInfo>(item.guid,
								item));
						break;
					case 5:
						Doc()->m_ballList.push_back(item);
						break;
					case 6:
						Doc()->m_myItemList.push_back(item);
						break;
					case 8:
						Doc()->m_cadItemList.push_back(item);
						break;
					case 9:
						Doc()->m_setItemList.push_back(item);
						break;
					case 0xe:
						Doc()->m_cutinList.push_back(item);
						break;
					case 0x1c:
						Doc()->m_auxPartList.push_back(item);
						break;
					case 0x1d:
						Doc()->m_questDropList.push_back(item);
						break;
					}
				}
			}
			if (total == count)
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x2c, 4, 0, 0, 0, 0));
			if (IsLocalContent((localContentType_t)0x17))
				Doc()->SetInitHaveHalloweenItem();
			if (IsLocalContent((localContentType_t)0x57) &&
				AfxGetTask()->GetActor("RealMyRoom"))
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
			break;
		}
	case 0x17d:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->ClearCardStack();
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				sCardStack stack;
				packet.DecodeBuffer(&stack, sizeof(stack));
				Doc()->AddCardStack(stack, false);
			}
			break;
		}
	case 0x12d:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->m_cardList.clear();
			break;
		}
	case 0x130:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int result = packet.Decode4();
			if (result == 0)
			{
				int count = packet.Decode2();
				for (int i = 0; i < count; ++i)
				{
					sCards card;
					packet.DecodeBuffer(&card, sizeof(card));
					if ((card.tid & 0xfc000000) == 0x7c000000)
					{
						std::list<sCards>& cardList = Doc()->m_cardList;
						bool bFound = false;
						std::list<sCards>::iterator it;
						for (it = cardList.begin(); it != cardList.end(); it++)
						{
							if ((*it).uid == card.uid)
							{
								bFound = true;
								break;
							}
						}
						if (!bFound)
						{
							sCards newCard = card;
							cardList.push_back(newCard);
						}
					}
					else
					{
						AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x25,
							(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xc4\xab\xb5\xe5 \xb5\xa5\xc0\xcc\xc5\xcd \xc6\xd0\xc5\xb6\xc0\xd4\xb4\xcf\xb4\xd9",
							0, 0, 0, 0));
					}
				}
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x21a, 0, 0, 0, 0, 0);
			}
			else if (result == 1)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x25,
					(int)"\xc4\xab\xb5\xe5 \xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0));
			}
			break;
		}
	case 0x12e:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			for (int i = 0; i < 4; ++i)
				Doc()->m_cardAbilityList[i].clear();
			break;
		}
	case 0x12f:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				sSCardAvilityPeriodInfo info;
				packet.DecodeBuffer(&info, sizeof(info));
				if ((info.tid & 0xfc000000) == 0x7c000000)
				{
					sSCardAvilityPeriodInfo card = info;
					IFF_STRUCT::sCard* pCard =
						ItemManager()->FindCard(card.tid);
					if (pCard)
					{
						card.Avility = pCard->Avility;
						card.AvilityValue = pCard->AvilityValue;
					}
					card.cardType = (card.tid >> 22) & 0xf;
					Doc()->m_cardAbilityList[0].push_back(card);
				}
				else
				{
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x25,
						(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xc4\xab\xb5\xe5 \xb5\xa5\xc0\xcc\xc5\xcd \xc6\xd0\xc5\xb6\xc0\xd4\xb4\xcf\xb4\xd9",
						0, 0, 0, 0));
				}
			}
			GetActor("RealMyRoom") << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
			break;
		}
	case 0x158:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int result = packet.Decode4();
			if (result == 0)
			{
				sCards card;
				packet.DecodeBuffer(&card, sizeof(card));
				std::list<sCards>::iterator it;
				for (it = Doc()->m_cardList.begin();
					it != Doc()->m_cardList.end(); it++)
				{
					if ((*it).tid == card.tid)
					{
						(*it).cardCount--;
						if ((*it).cardCount < 1)
							it = Doc()->m_cardList.erase(it);
						break;
					}
				}

				sSCardAvilityPeriodInfo info;
				info.uid = card.uid;
				info.tid = card.tid;
				info.cardType = (card.tid >> 22) & 0xf;
				info.useStartTime = card.useStartTime;
				info.useEndTime = card.useEndTime;
				info.partsTid = card.partTid;
				info.partsUid = card.partUid;
				info.slotNum = card.selectedCardSlotNum;
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(card.tid);
				if (pCard)
				{
					info.Avility = pCard->Avility;
					info.AvilityValue = pCard->AvilityValue;
				}
				if (info.cardType == 2)
				{
					if (pCard->Avility == 1)
						Doc()->m_myInfo.stat.dwExp += pCard->AvilityValue;
					if (pCard->Avility != 1 && pCard->Avility != 4 &&
						pCard->Avility != 0x11)
					{
						if (IsLocalContent((localContentType_t)0x6f))
						{
							for (std::list<sSCardAvilityPeriodInfo>::iterator
									 it = Doc()->m_cardAbilityList[0].begin();
								it != Doc()->m_cardAbilityList[0].end(); it++)
							{
								sSCardAvilityPeriodInfo& cur = *it;
								if ((cur.tid & 0x3c00000) == 0x800000)
								{
									if (info.Avility == cur.Avility)
									{
										Doc()->m_cardAbilityList[0].erase(it);
										break;
									}
								}
							}
							Doc()->m_cardAbilityList[0].push_back(info);
						}
						else
						{
							Doc()->m_cardAbilityList[0].push_back(info);
						}
						CCardManager::Instance()->SetSPCardAlarm(true);
					}
				}
				else if (info.cardType == 0 || info.cardType == 1)
				{
					if (info.cardType == 1)
						info.slotNum -= 4;
					std::list<sSCardAvilityPeriodInfo>::iterator it;
					for (it = Doc()->m_cardAbilityList[0].begin();
						it != Doc()->m_cardAbilityList[0].end(); it++)
					{
						if ((*it).partsUid == info.partsUid &&
							(*it).cardType == info.cardType &&
							(*it).slotNum == info.slotNum)
						{
							it = Doc()->m_cardAbilityList[0].erase(it);
							break;
						}
					}
					Doc()->m_cardAbilityList[0].push_back(info);
				}
				CCardManager::Instance()->UpdateCardBuffDlg();
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xdd, 1, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x219, card.tid, 0, 0, 0, 0));
			}
			else if (result == 1)
			{
				unsigned char error = packet.Decode1();
				if (error == 1)
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL,
						0x23,
						(int)"\xc0\xcc \xc4\xab\xb5\xe5\xc0\xc7 \xc8\xbf\xb0\xfa\xb4\xc2 \xc0\xcc\xb9\xcc \xc0\xfb\xbf\xeb\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				else
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL,
						0x23,
						(int)"\xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
			}
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			break;
		}
	case 0x131:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				sSCardAvilityPeriodInfo info;
				packet.DecodeBuffer(&info, sizeof(info));
				std::list<sSCardAvilityPeriodInfo>::iterator it;
				for (it = Doc()->m_cardAbilityList[0].begin();
					it != Doc()->m_cardAbilityList[0].end(); it++)
				{
					if ((*it).uid == info.uid)
					{
						it = Doc()->m_cardAbilityList[0].erase(it);
						break;
					}
				}
			}
			break;
		}
	case 0x15a:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int count = packet.Decode2();
			sSCardAvilityPeriodInfo info;
			for (int i = 0; i < count; ++i)
			{
				packet.DecodeBuffer(&info, sizeof(info));
				Doc()->m_cardAbilityList[0].push_back(info);
			}
			break;
		}
	case 0x159:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int count = packet.Decode2();
			for (int i = 0; i < count; ++i)
			{
				sSCardAvilityPeriodInfo info;
				packet.DecodeBuffer(&info, sizeof(info));
				std::list<sSCardAvilityPeriodInfo>::iterator it;
				for (it = Doc()->m_cardAbilityList[0].begin();
					it != Doc()->m_cardAbilityList[0].end(); it++)
				{
					if ((*it).uid == info.uid)
					{
						(*it).partsTid = info.partsTid;
						(*it).partsUid = info.partsUid;
						break;
					}
				}
			}
			break;
		}
	case 0x17f:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long oldPartsTid = packet.Decode4();
			unsigned long oldPartsUid = packet.Decode4();
			unsigned long newPartsTid = packet.Decode4();
			unsigned long newPartsUid = packet.Decode4();

			for (CARDABILITY_LIST_IT it = Doc()->m_cardAbilityList[0].begin();
				it != Doc()->m_cardAbilityList[0].end(); it++)
			{
				sSCardAvilityPeriodInfo& info = *it;
				if (info.partsTid == oldPartsTid &&
					info.partsUid == oldPartsUid)
				{
					info.partsTid = newPartsTid;
					info.partsUid = newPartsUid;
				}
			}
		}
		break;
	case 0x13a:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			sSCardAvilityPeriodInfo ability;
			memset(&ability, 0, sizeof(ability));
			unsigned long result = packet.Decode4();
			if (result == 0)
			{
				packet.DecodeBuffer(&ability, sizeof(ability));

				CARDABILITY_LIST_IT it;
				for (it = Doc()->m_cardAbilityList[0].begin();
					it != Doc()->m_cardAbilityList[0].end(); it++)
				{
					if ((*it).uid != ability.uid)
						continue;

					if ((*it).Avility == 11)
					{
						int cardSlots =
							CCardManager::Instance()->GetCardPeriodSlot();
						if (Doc()->GetNumAddItemSlotByMascot() == 1 ||
							cardSlots > 1)
							Doc()->m_myInfo.userEquip.tidItemSlot[9] = 0;
						else if (!Doc()->GetNumAddItemSlotByMascot() &&
							!cardSlots)
						{
							Doc()->m_myInfo.userEquip.tidItemSlot[8] = 0;
							Doc()->m_myInfo.userEquip.tidItemSlot[9] = 0;
						}
					}
					it = Doc()->m_cardAbilityList[0].erase(it);
					CCardManager::Instance()->UpdateCardBuffDlg();
					break;
				}
			}
			else if (result == 1)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc4\xab\xb5\xe5\xc0\xc7 \xb1\xe2\xb0\xa3\xc1\xa6 \xc6\xaf\xbc\xf6\xb4\xc9\xb7\xc2 \xbb\xe8\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0));
			}

			GetActor("RealMyRoom") << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
		}
		break;
	case 0x14c:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
			FrCardBoxDlg* pDlg =
				(FrCardBoxDlg*)g_pFresh->GetDesktop()->FindChildForm(
					"FrCardBoxDlg");
			if (pDlg)
			{
				pDlg->RecievedPacket(&packet);
				break;
			}

			unsigned long result = packet.Decode4();

			unsigned long tids[4];
			memset(tids, 0, sizeof(tids));

			if (result == 0)
			{
				sCards card;
				packet.DecodeBuffer(&card, sizeof(card));
				tids[0] = card.tid;

				for (CARD_LIST_IT it = Doc()->m_cardList.begin();
					it != Doc()->m_cardList.end(); ++it)
				{
					if ((*it).tid == card.tid)
					{
						--(*it).cardCount;
						if ((*it).cardCount < 1)
							it = Doc()->m_cardList.erase(it);
						break;
					}
				}

				int count = packet.Decode1();
				unsigned int slot = 1;
				for (int i = 0; i < count; ++i)
				{
					packet.DecodeBuffer(&card, sizeof(card));
					int quantity = packet.Decode4();
					for (int j = 0; j < quantity; ++j)
					{
						tids[slot] = card.tid;
						if (slot < 3)
							++slot;
					}

					bool bFound = false;
					for (CARD_LIST_IT it = Doc()->m_cardList.begin();
						it != Doc()->m_cardList.end(); ++it)
					{
						if ((*it).tid == card.tid)
						{
							(*it).cardCount += quantity;
							bFound = true;
							break;
						}
					}
					if (!bFound)
						Doc()->m_cardList.push_back(card);
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xdd, 1, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 2, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x229, tids[0],
					tids[1], tids[2], tids[3], 0));
			}
			else if (result == 1)
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 2, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc4\xab\xb5\xe5\xc6\xd1 \xbf\xc0\xc7\xc2\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0));
			}
		}
		break;
	case 0x73:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x25,
				(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd \xc6\xd0\xc5\xb6\xc0\xbb \xba\xb8\xb3\xc2\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0));
		}
		break;
	case 0x13d:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (!IsLocalContent((localContentType_t)0x5b))
				break;

			unsigned char slot = packet.Decode1();
			unsigned char item = packet.Decode1();
			unsigned short count = packet.Decode2();

			for (unsigned short i = 0; i < count; ++i)
			{
				sGiftInfo gift;
				packet.DecodeBuffer(&gift, sizeof(gift));

				Doc()->m_chatManager.FilteringHack(gift.sMsg, false);
				Doc()->m_giftList.push_back(gift);
			}

			Doc()->m_cardPackSlot = slot;
			Doc()->m_cardPackItem = item;
			Doc()->m_rmrTabValue = count;
			Doc()->m_bRmrTabPending = true;

			if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
				AfxPostMsg(NULL, "RealMyRoom", 198, 0, 0, 0, 0);
			else if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "",
					true);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 198, 0, 0, 0, 0);

				ntAvatarManager::Instance()->SetActive(false);
			}
			else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
			{
				Doc()->m_bRebuildUnderBar = true;
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "",
					false);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 198, 0, 0, 0, 0);
			}
			else
			{
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "",
					CTaskManager::Instance()->GetPreservedTask() != NULL);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 198, 0, 0, 0, 0);
			}
		}
		break;
	case 0x78:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char mode = packet.Decode1();
			unsigned short pageNum = packet.Decode2();
			unsigned short selPageNum = packet.Decode2();
			unsigned short curPage = packet.Decode2();

			Doc()->m_cardPackPageNum = pageNum;
			Doc()->m_cardPackSelPageNum = selPageNum;
			Doc()->m_cardPackCurPage = curPage;

			if (mode == 1)
			{
				Doc()->m_giftList.clear();
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x2c, 8, 0, 0, 0, 0));

				for (unsigned short i = 0; i < selPageNum; ++i)
				{
					sGiftInfo gift;
					packet.DecodeBuffer(&gift, sizeof(gift));

					Doc()->m_chatManager.FilteringHack(gift.sMsg, false);

					IFF_ITEM_COMMON* common =
						ItemManager()->FindCommonItem(gift.tid);
					if (common && common->Final == true)
						Doc()->m_giftList.push_back(gift);
				}

				if (Doc()->m_sendGiftList.size())
				{
					Doc()->m_bRefreshCamera = true;
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));
				}
			}
			else
			{
				int pages = curPage / pageNum;
				if (curPage / pageNum * pageNum < curPage)
					++pages;

				if (pages)
				{
					WSendPacket send((enumClientPacket)0x92);
					send.Encode4(pages);
					send.Send(TO_GAME);
				}
				else
				{
					WSendPacket send((enumClientPacket)0x92);
					send.Encode4(1);
					send.Send(TO_GAME);
				}
			}
		}
		break;
	case 0x85:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char result = packet.Decode1();
			if (!result)
				break;

			sCaddieInfo caddie;
			packet.DecodeBuffer(&caddie, sizeof(caddie));
			Doc()->AddCaddie(&caddie);
			Doc()->m_myInfo.userEquip.guidCaddie = caddie.guid;
			Doc()->m_userInfo[0].caddieInfo = caddie;
		}
		break;
	case 0x9d:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			Doc()->m_gameServerList.clear();
			unsigned char count = packet.Decode1();

			sGameServerInfo server;
			for (int i = 0; i < count; ++i)
			{
				memset(&server, 0, sizeof(server));
				packet.DecodeBuffer(&server, sizeof(server));

				if ((server.property & 0x1000) == 0)
					Doc()->m_gameServerList.push_back(server);
			}

			Doc()->m_channelList.clear();
			count = packet.Decode1();
			for (int j = 0; j < count; ++j)
			{
				sChannelInfo channel;
				packet.DecodeBuffer(&channel, sizeof(channel));
				Doc()->m_channelList.push_back(channel);
			}

			if (IsLocalContent(S4_GOWITH_FRIEND) ||
				IsLocalContent(S4_INVITE_FRIEND) ||
				IsLocalContent(S4_MATCHING_SYSTEM))
			{
				if (Doc()->m_goWithMode == 1)
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x216, 0, 0, 0, 0, 0));
				}
				else if (Doc()->m_goWithMode == 2)
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x216, 0, 0, 0, 0, 0));
				}
				else if (Doc()->m_goWithMode == 3)
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x216, 0, 0, 0, 0, 0));
				}
				else
				{
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x19, 0, 0, 0, 0, 0));
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x1b, 0, 0, 0, 0, 0));
				}
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x19, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x1b, 0, 0, 0, 0, 0));
			}
		}
		break;
	case 0xfa:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			CMessengerInfo::Instance()->m_serverList.clear();
			MSNServerInfo server;

			int count = packet.Decode1();
			for (int i = 0; i < count; ++i)
			{
				memset(&server, 0, sizeof(sGameServerInfo));
				packet.DecodeBuffer(&server, sizeof(sGameServerInfo));

				CMessengerInfo::Instance()->m_serverList.push_back(server);
			}

			CMessengerInfo::Instance()->SetServerListWaitingState(false);
		}
		break;
	case 0x9f:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			unsigned char result = packet.Decode1();
			if (result == 0)
			{
				unsigned long uid = packet.Decode4();
				sPangYaUserInfo info;
				packet.DecodeBuffer(&info, sizeof(info));

				if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x50, uid, (int)&info, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x50, uid, (int)&info, 0, 0, 0));
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc0\xaf\xc0\xfa\xb8\xa6 \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
		}
		break;
	case 0xa0:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			Doc()->m_rankServerAddr = packet.DecodeStr();
			Doc()->m_rankServerPort = packet.Decode4();

			WNetworkSystem::Instance()->Init((WNetworkSystem::eNetUnit)3);

			WNetworkSystem::Instance()->SendMessage((WNetworkSystem::eNetUnit)3,
				(NetworkUnit::eEvent)0);
		}
		break;
	case 0xab:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			std::string nick;
			char msg[128];

			packet.DecodeStr(nick);
			unsigned char channelId = packet.Decode1();

			CSharedDoc* pDoc = Doc();
			std::list<sChannelInfo>& channels = pDoc->m_channelList;
			CHANNEL_LIST_IT it =
				std::find(channels.begin(), channels.end(), channelId);
			if (it == channels.end())
				sprintf(msg,
					"%s\xb4\xd4\xc0\xba [%s]\xbf\xa1\xbc\xad \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					nick.c_str(), Doc()->m_curGameServer.name);
			else
				sprintf(msg,
					"%s\xb4\xd4\xc0\xba [%s]\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd8 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					nick.c_str(), (*it).Name);

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 3, (int)msg, 0xffff7878, 0, 0, 0));
		}
		break;
	case 0xc4:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (Doc()->m_myInfo.stat.Level > 0)
			{
				if (Doc()->m_myInfo.stat.dwNoMannerGameCount > 0)
					Doc()->m_myInfo.stat.dwNoMannerGameCount--;

				if (Doc()->m_myInfo.stat.dwGameCount > 0)
					Doc()->m_myInfo.stat.dwGameCount--;
			}

			if (Doc()->m_myInfo.stat.dwDisconnectCount > 0)
				Doc()->m_myInfo.stat.dwDisconnectCount--;
		}
		break;
	case 0xb7:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			Doc()->m_newRecordState = packet.Decode1();

			if (IS_KINDOF(CLobbyTask, AfxGetTask()))
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 0x53, 0, 0, 0, 0, 0);
		}
		break;
	case 0x82:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char type = packet.Decode1();
			unsigned char bGM = type >> 7;
			type &= 0x7f;

			bool bMine = type == 0;

			unsigned long color = 0xffd2a2ff;
			if (bGM)
				color = 0xff6eecfb;

			std::string nick, text;
			std::string chat;

			packet.DecodeStr(nick);
			packet.DecodeStr(text);

			if (bMine)
			{
				chat = MyNick();
				chat += ">";
				chat += nick;
			}
			else
			{
				if (!COption::Instance()->gIsWhisperAllowed())
					return 1;

				STRING_LIST_IT it = std::find(Doc()->m_ignoreList.begin(),
					Doc()->m_ignoreList.end(), nick);
				if (it != Doc()->m_ignoreList.end())
					return 1;

				chat = nick + ">" + MyNick();
			}

			Doc()->m_chatManager.FilteringHack(text.c_str(), false);

			chat += " : \x08'";
			chat += text;
			chat += "'";

			if (AfxGetTask()->IsRecieveWhisper())
				CChatMsg::Instance()->AddChatMsg(chat, color, false, true);
			else
			{
				WSendPacket send((enumClientPacket)0xe0);
				send.EncodeStr(nick);
				send.Send(TO_GAME);
			}
		}
		break;
	case 0xb9:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			Doc()->m_roomSetupStage |= packet.Decode4();

			if (Doc()->m_roomSetupStage & 1)
			{
				std::string msg(
					"\xbe\xcb\xb8\xb2 : \xb8\xf0\xb5\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb0\xa1 \xc0\xcc\xba\xa5\xc6\xae \xc0\xc7\xbb\xf3\xc0\xbb \xc0\xd4\xb0\xed \xc0\xd6\xc0\xb8\xb9\xc7\xb7\xce, 2\xb9\xe8\xc0\xc7 \xc5\xac\xb8\xae\xbe\xee \xba\xb8\xb3\xca\xbd\xba\xb0\xa1 \xc0\xfb\xbf\xeb\xb5\xcb\xb4\xcf\xb4\xd9.");
				CChatMsg::Instance()->AddChatMsg(msg, 0xffff7878, false, false);
			}
		}
		break;
	case 0x87:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long result = packet.Decode4();

			const char* msg = NULL;
			if (result == 0)
			{
				msg =
					"\xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8 \xc1\xb6\xc8\xb8\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			}
			else if (result == 2)
			{
				msg =
					"\xbf\xe4\xc3\xbb\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4";
			}
			else if (result == 3)
			{
				if (CUserInfo::Instance()->IsVisible())
					CUserInfo::Instance()->Close();
				break;
			}

			if (msg)
			{
				if (CUserInfo::Instance()->IsVisible())
					CUserInfo::Instance()->Close();

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x23, (int)msg, 0, 0, 0, 0));
				break;
			}

			unsigned char season = packet.Decode1();
			unsigned long uid = packet.Decode4();

			USERINFOTIME_MAP_IT it;
			if (!Doc()->FindUserInfoTimeUID(uid, it))
				break;

			(*it).second.dwUpdate = g_CurrentTime;

			if (!CUserInfo::Instance()->IsVisible())
				break;

			if (!CUserInfo::Instance()->IsRecvedData(season))
				break;

			if (MyUID() != uid)
				CUserInfo::Instance()->SetInfo(uid, 0, true, false, false,
					false, (*it).second.info.info.sNick);
			else
				CUserInfo::Instance()->SetInfo(MyUID(), MyGuid(false), true,
					false, false, false, (*it).second.info.info.sNick);

			CUserInfo::Instance()->Refresh();
		}
		break;
	case 0x8b:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long elapsed = packet.Decode4();
			Doc()->m_approachStartTime = g_CurrentTime - elapsed;
		}
		break;
	case 0x91:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			int result = packet.Decode1();
			switch (result)
			{
			case 4:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 1:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xb0\xed\xbf\xeb\xb1\xe2\xb0\xa3\xc0\xcc \xbe\xc6\xc1\xf7 \xb3\xb2\xbe\xc6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 2:
			{
				unsigned long caddieId = packet.Decode4();
				__int64 pang = packet.Decode8();

				CADDIE_MAP_IT it = Doc()->m_caddieMap.find(caddieId);
				if (it != Doc()->m_caddieMap.end())
					(*it).second.Remain_Date = 30;

				if (Doc()->m_myInfo.stat.i64Pang > pang)
					Doc()->m_myInfo.stat.i64Pang = pang;

				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc4\xb3\xb5\xf0\xb8\xa6 \\c0xffff0000\\c\xc7\xd1\xb4\xde\xb0\xa3 \\c0xff000000\\c\xc0\xe7\xb0\xed\xbf\xeb \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
			break;
			case 0:
			case 3:
			case 5:
			case 6:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xc4\xb3\xb5\xf0 \xc0\xe7\xb0\xed\xbf\xeb\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x5c, result, 0, 0, 0, 0));
		}
		break;
	case 0x9b:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (!GalleryMode())
				break;

			packet.DecodeBuffer(&Doc()->m_userInfo[0], sizeof(sUserInfo));
			Doc()->m_myInfo.info.dwGalleryGuid =
				Doc()->m_userInfo[0].info.dwGuid;

			if (!IsLocalContent(S4_UCC))
				break;

			RequestUccInfo(Doc()->m_userInfo[0]);
		}
		break;
	case 0xa5:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char count = packet.Decode1();
			for (unsigned char i = 0; i < count; ++i)
			{
				unsigned long tid = packet.Decode4();
				unsigned long uid = packet.Decode4();
				unsigned short remain = packet.Decode2();

				ITEM_LIST_IT it;
				switch (tid >> 26)
				{
				case 6:
				{
					unsigned long tids[8];
					tids[0] = 0x1a000020;
					tids[1] = 0x1a000021;
					tids[2] = 0x1a000022;
					tids[3] = 0x1a000023;
					tids[4] = 0x1a000024;
					tids[5] = 0x1a000025;
					tids[6] = 0x1a000026;

					tids[7] = 0x1a000027;

					bool bErase = true;
					for (int j = 0; j < 8; ++j)
					{
						if (tids[j] == tid)
						{
							bErase = false;
							break;
						}
					}

					for (it = Doc()->m_myItemList.begin();
						it != Doc()->m_myItemList.end(); ++it)
					{
						if ((*it).guid == uid)
						{
							(*it).Common[0] = remain;
							if (remain < 1 && bErase)
								Doc()->m_myItemList.erase(it);
							break;
						}
					}
				}
				break;
				case 5:
					for (it = Doc()->m_ballList.begin();
						it != Doc()->m_ballList.end(); ++it)
					{
						if ((*it).guid == uid)
						{
							(*it).Common[0] = remain;
							if (remain < 1)
							{
								Doc()->m_ballList.erase(it);
								Doc()->m_myInfo.userEquip.tidBall = 0x14000000;
							}
							break;
						}
					}
					break;
				case 0x1d:
					for (it = Doc()->m_questDropList.begin();
						it != Doc()->m_questDropList.end(); ++it)
					{
						if ((*it).guid == uid)
						{
							(*it).Common[0] = remain;
							break;
						}
					}

					if (remain == 0 && IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
						AfxGetTask()->GetActor("RealMyRoom")
							<< MsgObject(NULL, 0x67, 0, 0, 0, 0, 0);
					break;
				case 0x1c:
					for (it = Doc()->m_auxPartList.begin();
						it != Doc()->m_auxPartList.end(); ++it)
					{
						if ((*it).guid == uid)
						{
							(*it).Common[0] = remain;
							if (remain < 1)
							{
								CHAR_MAP_IT cit;
								for (cit = Doc()->m_charMap.begin();
									cit != Doc()->m_charMap.end(); ++cit)
								{
									unsigned int slot = (tid >> 21) & 0x1f;
									if (slot < 5 &&
										(*cit).second.tidAuxParts[slot] == tid)
										(*cit).second.tidAuxParts[slot] = 0;
								}
								Doc()->m_auxPartList.erase(it);
							}
							break;
						}
					}
					break;
				case 2:
					for (it = Doc()->m_partsList.begin();
						it != Doc()->m_partsList.end(); ++it)
					{
						if ((*it).guid == uid)
						{
							Doc()->m_partsList.erase(it);
							break;
						}
					}
					break;
				case 7:
				{
					CADDIE_MAP_IT cit = Doc()->m_caddieMap.find(uid);
					if (cit != Doc()->m_caddieMap.end())
						Doc()->m_caddieMap.erase(cit);
				}
				break;
				case 4:
				{
					CLUBSET_MAP_IT cit;
					for (cit = Doc()->m_clubSetMap.begin();
						cit != Doc()->m_clubSetMap.end(); ++cit)
					{
						if ((*cit).second.guid == uid &&
							(*cit).second.tid == tid)
						{
							Doc()->m_clubSetMap.erase(cit);
							break;
						}
					}
				}
				break;
				case 0x1f:
				{
					CARD_LIST_IT cit = Doc()->m_cardList.begin();
					for (; cit != Doc()->m_cardList.end(); ++cit)
					{
						if ((*cit).uid == uid && (*cit).tid == tid)
						{
							if (remain < 1)
								Doc()->m_cardList.erase(cit);
							else
								(*cit).cardCount = remain;
							break;
						}
					}
				}
				break;
				}
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xdd, 0, 0, 0, 0, 0));
		}
		break;
	case 0xa7:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned long infoType = packet.Decode4();
			unsigned long value = packet.Decode4();
			char text[256];

			switch (infoType)
			{
			case 0:
				sprintf(text,
					"< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xba\xf1\xb0\xa1 \xbf\xc3 \xc8\xae\xb7\xfc\xc0\xba 1/%u\xc0\xd4\xb4\xcf\xb4\xd9.",
					value);
				break;
			case 1:
				sprintf(text,
					"< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xc6\xce\xba\xb8\xb3\xca\xbd\xba\xb4\xc2 %u%%\xc0\xd4\xb4\xcf\xb4\xd9.",
					value);
				break;
			case 2:
				sprintf(text,
					"< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xb5\xe5\xb6\xf8\xc8\xae\xb7\xfc\xc0\xba 1/%u\xc0\xd4\xb4\xcf\xb4\xd9.",
					value);
				break;
			case 3:
				sprintf(text,
					"< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe5\xb7\xd3 \xc8\xae\xb7\xfc\xc0\xba %u%%\xc0\xd4\xb4\xcf\xb4\xd9.",
					value);
			}

			if (IS_KINDOF(CLobbyTask, AfxGetTask()))
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 3, (int)text, 0xffff7878, 0, 0, 0);
			else
				CChatMsg::Instance()->AddChatMsg(text, 0xffff7878, false,
					false);
			break;
		}
	case 0xc6:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			if (IsLocalContent(S4_HACKINGTOOL))
			{
				__int64 pang = packet.Decode8();
				__int64 recvPang = packet.Decode8();

				if (recvPang)
				{
					if (Doc()->m_myInfo.stat.i64Pang == pang + recvPang)
					{
						Doc()->m_myInfo.stat.i64Pang = pang;
					}
					else
					{
						WSendPacket send((enumClientPacket)0x33);

						std::string log;
						char text[128];
						sprintf(text,
							"[Pang Hacker] Old Pangya %I64d, Recive  %I64d , Calculation %I64d",
							Doc()->m_myInfo.stat.i64Pang, pang, recvPang);
						log = text;

						send.Encode1(3);
						send.EncodeStr(log);
						send.Send(TO_GAME);
					}
				}
				else
				{
					Doc()->m_myInfo.stat.i64Pang = pang;
				}
			}
			else
			{
				__int64 pang = packet.Decode8();
				Doc()->m_myInfo.stat.i64Pang = pang;
			}
			break;
		}
	case 0xa8:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			unsigned short count = packet.Decode2();
			int msgId;
			for (unsigned short i = 0; i < count; i++)
			{
				sBuyItemResult result;
				packet.DecodeBuffer(&result, sizeof(result));

				if (IS_KINDOF(CShopTask, AfxGetTask()))
				{
					if (IsLocalContent(S3_CADDIE_REPORT) &&
						result.Typeid == 0x1a000042)
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.ItemType = result.ItemType;
						item.tid = 0x1a000042;
						item.HourRemain = result.Time;
						item.Common[0] = 1;
						if (result.Count >= 1)
							item.Common[0] = result.Count;

						ITEM_LIST_IT it;
						it = Doc()->m_myItemList.begin();
						for (; it != Doc()->m_myItemList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0] = item.Common[0];
								break;
							}
						}

						if (it == Doc()->m_myItemList.end())
						{
							item.Common[1] = packet.Decode2();
							item.Common[2] = packet.Decode2();

							_SYSTEMTIME date;
							packet.DecodeBuffer(&date, sizeof(date));
							item.ItemDate = date;
							item.ItemType = 2;

							Doc()->m_myItemList.push_back(item);
						}
					}
					else
					{
						int shopMsg;
						switch (result.Typeid >> 26)
						{
						case 1:
							shopMsg = 0xb5;
							break;
						case 2:
							shopMsg = 0xb6;
							break;
						case 7:
							shopMsg = 0xb7;
							break;
						case 8:
							shopMsg = 0xb8;
							break;
						case 6:
							shopMsg = 0xb9;
							break;
						case 4:
							shopMsg = 0xba;
							break;
						case 5:
							shopMsg = 0xbb;
							break;
						case 0xe:
							shopMsg = 0xbc;
							break;
						case 0xf:
							shopMsg = 0xbd;
							break;
						case 0x1c:
							shopMsg = 0xbe;
							break;
						case 0x10:
							shopMsg = 0xbf;
							break;
						case 0x1f:
							shopMsg = 0x218;
							break;
						case 0x12:
							shopMsg = 0x217;
							break;
						default:
							continue;
						}

						AfxGetTask()->GetActor("Shop") << MsgObject(NULL,
							shopMsg, result.guid, result.Typeid, result.Time,
							result.ItemType, (int)&result);
					}
				}
				else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
				{
					if (IsLocalContent(S3_CADDIE_REPORT) &&
						result.Typeid == 0x1a000042)
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.ItemType = result.ItemType;
						item.tid = 0x1a000042;
						item.HourRemain = result.Time;
						item.Common[0] = 1;
						if (result.Count >= 1)
							item.Common[0] = result.Count;

						ITEM_LIST_IT it;
						it = Doc()->m_myItemList.begin();
						for (; it != Doc()->m_myItemList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0] = item.Common[0];
								break;
							}
						}

						if (it == Doc()->m_myItemList.end())
						{
							item.Common[1] = packet.Decode2();
							item.Common[2] = packet.Decode2();

							_SYSTEMTIME date;
							packet.DecodeBuffer(&date, sizeof(date));
							item.ItemDate = date;
							item.ItemType = 2;

							Doc()->m_myItemList.push_back(item);
						}
					}
					else
					{
						switch (result.Typeid >> 26)
						{
						case 1:
							msgId = 0xc8;
							break;
						case 2:
							msgId = 0xca;
							break;
						case 7:
							msgId = 0xcb;
							break;
						case 8:
							msgId = 0xcc;
							break;

						case 4:
							msgId = 0xcf;
							break;

						case 0xe:
							msgId = 0xd0;
							break;
						case 5:
						case 6:
						case 0x1c:
						case 0x1d:
							msgId = 0xce;
							break;
						case 0xf:
							msgId = 0xc9;
							break;
						case 0x10:
							msgId = 0xcd;
							break;
						case 0x1f:
							msgId = 0xe0;
							break;
						}

						if (IsLocalContent(S4_REAL_MYROOM))
						{
							AfxGetTask()->GetActor("RealMyRoom") << MsgObject(
								NULL, msgId, result.guid, result.Typeid,
								result.Time, result.ItemType, (int)&result);

							if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM) &&
								msgId != 0xc9)
								AfxGetTask()->GetActor("RealMyRoom")
									<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
						}
						else
						{
							AfxGetTask()->GetActor("RealMyRoom") << MsgObject(
								NULL, msgId, result.guid, result.Typeid,
								result.Time, result.ItemType, (int)&result);
						}
					}
				}
				else
				{
					switch (result.Typeid >> 26)
					{
					case 1:
					{
						sCharacterInfo info;
						memset(&info, 0, sizeof(info));
						info.guid = result.guid;
						info.tid = result.Typeid;
						ItemManager()->GetDefCombo(info.tid, info.tidParts);

						Doc()->m_charMap.insert(
							std::map<unsigned int, sCharacterInfo>::value_type(
								info.guid, info));
						break;
					}
					case 2:
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.tid = result.Typeid;
						if (result.ItemType == 2)
						{
							item.HourRemain = result.Time * 24;
							item.ItemType = 2;
						}

						Doc()->AddMyPartsList(item);
						break;
					}
					case 7:
					{
						sCaddieInfo info;
						memset(&info, 0, sizeof(info));
						info.guid = result.guid;
						info.tid = result.Typeid;
						info.Rent_flag = 1;
						info.Remain_Date = 30;

						if (info.tid >= 0x1c00000c && info.tid <= 0x1c00000e)
						{
							info.Level = result.ItemType;
							info.Exp = result.Time;
						}

						Doc()->m_caddieMap.insert(
							std::map<unsigned int, sCaddieInfo>::value_type(
								info.guid, info));
						break;
					}
					case 6:
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.ItemType = result.ItemType;
						item.tid = result.Typeid;
						item.Common[0] = 1;
						if (result.Count >= 1)
							item.Common[0] = result.Count;

						if (result.ItemType)
						{
							if (result.ItemType == 3)
								item.HourRemain = result.Time;
							else if (result.ItemType == 4)
								item.HourRemain = result.Time * 24;
							else if (result.ItemType == 5)
								item.HourRemain = result.Time * 720;
						}

						ITEM_LIST_IT it;
						it = Doc()->m_myItemList.begin();
						for (; it != Doc()->m_myItemList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0] = item.Common[0];
								break;
							}
						}

						if (it == Doc()->m_myItemList.end())
						{
							if (IsLocalContent(S3_CADDIE_REPORT) &&
								item.tid == 0x1a000042)
							{
								item.Common[1] = packet.Decode2();
								item.Common[2] = packet.Decode2();
								item.HourRemain = result.Time;

								_SYSTEMTIME date;
								packet.DecodeBuffer(&date, sizeof(date));
								item.ItemDate = date;
								item.ItemType = 2;
							}

							Doc()->m_myItemList.push_back(item);
						}
						break;
					}
					case 5:
					{
						ITEM_LIST_IT it = Doc()->m_ballList.begin();
						for (; it != Doc()->m_ballList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0] =
									result.Count < 1 ? 1 : result.Count;
								break;
							}
						}

						if (it == Doc()->m_ballList.end())
						{
							sItemInfo item;
							memset(&item, 0, sizeof(item));
							item.guid = result.guid;
							item.ItemType = result.ItemType;
							item.tid = result.Typeid;
							item.Common[0] = 1;
							if (result.Count >= 1)
								item.Common[0] = result.Count;

							Doc()->m_ballList.push_back(item);
						}
						break;
					}
					case 4:
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.tid = result.Typeid;

						Doc()->m_clubSetMap.insert(
							std::map<unsigned int, sItemInfo>::value_type(
								item.guid, item));
						break;
					}
					case 0x1d:
					{
						if (ItemManager()->FindQuestDrop(result.Typeid) == NULL)
							break;

						ITEM_LIST_IT it = Doc()->m_questDropList.begin();
						for (; it != Doc()->m_questDropList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0]++;
								break;
							}
						}

						if (it == Doc()->m_questDropList.end())
						{
							sItemInfo item;
							memset(&item, 0, sizeof(item));
							item.guid = result.guid;
							item.tid = result.Typeid;
							item.Common[0] = 1;

							Doc()->m_questDropList.push_back(item);
						}
						break;
					}
					case 0x10:
					{
						sMascotInfo info;
						memset(&info, 0, sizeof(info));
						info.guid = result.guid;
						info.tid = result.Typeid;
						info.endDate = result.endDate;
						info.Remain_Date = result.Time;
						strcpy(info.szMsg, "PANGYA!");

						Doc()->AddMascot(&info);
						break;
					}
					case 0x1c:
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.ItemType = result.ItemType;
						item.tid = result.Typeid;
						item.Common[0] = 1;
						if (result.Count >= 1)
							item.Common[0] = result.Count;

						if (result.ItemType)
						{
							if (result.ItemType == 3)
								item.HourRemain = result.Time;
							else if (result.ItemType == 4)
								item.HourRemain = result.Time * 24;
							else if (result.ItemType == 5)
								item.HourRemain = result.Time * 720;
						}

						ITEM_LIST_IT it;
						it = Doc()->m_auxPartList.begin();
						for (; it != Doc()->m_auxPartList.end(); ++it)
						{
							if (it->guid == result.guid)
							{
								(*it).Common[0] = item.Common[0];
								break;
							}
						}

						if (it == Doc()->m_auxPartList.end())
							Doc()->m_auxPartList.push_back(item);
						break;
					}
					case 0x1f:
					{
						unsigned long uid = result.guid;
						unsigned long typeId = result.Typeid;
						int cardCount;
						if (result.Count > 0)
							cardCount = result.Count;
						else
							cardCount = 1;

						if (ItemManager()->FindCard(typeId) == NULL)
							break;

						sCards card;
						memset(&card, 0, sizeof(card));
						card.uid = uid;
						card.tid = typeId;
						card.cardCount = cardCount;

						bool bFound = false;
						for (CARD_LIST_IT it = Doc()->m_cardList.begin();
							it != Doc()->m_cardList.end(); ++it)
						{
							if ((*it).tid == typeId)
							{
								(*it).cardCount += cardCount;
								bFound = true;
								break;
							}
						}

						if (!bFound)
							Doc()->m_cardList.push_back(card);
						break;
					}
					case 0xe:
					{
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.guid = result.guid;
						item.tid = result.Typeid;
						item.ItemType = result.ItemType;
						item.HourRemain = result.Time * 24;

						Doc()->m_cutinList.push_back(item);
						break;
					}
					case 0xf:
						break;
					default:
						continue;
					}
				}

				if (IsAngelWing(result.Typeid))
					Doc()->CheckAngelWing();

				if (IsLocalContent((localContentType_t)0x17) &&
					(result.Typeid & 0xfc000000) == 0x8000000 &&
					(result.Typeid & 0x1800) == 0x1800)
					Doc()->SetInitHaveHalloweenItem();
			}
			break;
		}
	case 0x93:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			if (packet.Decode2() == 0x111)
			{
				switch (packet.Decode4())
				{
				case 0:
				{
					__int64 pang = packet.Decode8();
					Doc()->m_myInfo.stat.i64Pang = pang;

					MESSENGER()->Notify1(
						"\xc2\xca\xc1\xf6\xb8\xa6 \xb3\xb2\xb0\xe5\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb4\xd9\xc0\xbd \xb7\xce\xb1\xd7\xc0\xce\xc7\xd2 \xb6\xa7 \xc0\xfc\xb4\xde\xb5\xcb\xb4\xcf\xb4\xd9.");
					break;
				}
				case 3:
					MESSENGER()->Notify1(
						"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc2\xca\xc1\xf6\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6\xc2\xf7\xbc\xad \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				case 7:
				case 0xa:
					MESSENGER()->Notify1(
						"\xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4 \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				case 0xc:
					MESSENGER()->Notify1(
						"\xba\xb8\xc0\xaf\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xcf\xbf\xa9 \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				default:
					MESSENGER()->Notify1(
						"\xc2\xca\xc1\xf6 \xba\xb8\xb3\xbb\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}
			}

			WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN);
			break;
		}
	case 0xd3:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned long value = packet.Decode4();
			if (value == 0)
				break;

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xad, value, 0, 0, 0, 0));
			break;
		}
	case 0xd4:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int result = packet.Decode4();

			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"bongdari_shop");
			FrBongdariShopDlg* pShop = NULL;
			if (pWnd == NULL)
				__rtti_obj = NULL;
			else
			{
				pShop = (FrBongdariShopDlg*)pWnd;
				pShop->EnableExitButton(true);
			}

			switch (result)
			{
			case 0:
			{
				int count = packet.Decode4();
				if (count < 1 || count > 5)
					break;

				sBonusBall balls[5] = { 0 };
				for (int i = 0; i < count; i++)
					packet.DecodeBuffer(&balls[i], sizeof(sBonusBall));

				__int64 pang = packet.Decode8();
				__int64 cookie = packet.Decode8();

				Doc()->m_myInfo.stat.i64Pang = pang;
				Doc()->m_bonusCash = cookie;
				Doc()->RefreshAllCookie();

				if (pShop)
				{
					pShop->InitBongdariShop();
					pShop->SetBonusBall(count, balls);
					pShop->SetBongdariShop();
					pShop->SetDataFee(0);
				}

				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
				break;
			}
			case 2:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9",
					0, 0, 0, 0));
				break;
			case 3:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9",
					0, 0, 0, 0));
				break;
			}
			break;
		}
	case 0xf9:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			Doc()->SetBsUsableTimes((short)packet.Decode4());
			Doc()->SetBsUsableBonusTimes((short)packet.Decode4());

			FrBongdariShopDlg* pShop =
				(FrBongdariShopDlg*)g_pFresh->GetManager()
					->GetDesktop()
					->FindChildByName("bongdari_shop");
			if (pShop == NULL)
			{
				__rtti_obj = NULL;
				break;
			}

			pShop->EnableExitButton(true);
			pShop->ShowDiscountUsableTimes();
			pShop->UpdateUsableTimes();
			break;
		}
	case 0x2d:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			switch (packet.Decode1())
			{
			case 2:
			{
				WNetworkSystem* pNet = WNetworkSystem::Instance();
				MESSENGER()->Notify1(
					"\xb8\xde\xbd\xc5\xc0\xfa\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd1 \xb7\xce\xb1\xd7\xc0\xce\xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xee \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
				if (pNet->IsConnected(WNetworkSystem::NET_MSN))
					pNet->ForceShutDown(WNetworkSystem::NET_MSN);
				break;
			}
			case 0:
			{
				unsigned long uid = packet.Decode4();
				if (uid != MyUID())
					break;

				MESSENGER()->SetComplete((eConnectStatus)2);

				sUserPosition pos;
				pos.iServerGUID = Doc()->m_gameServerUID;
				if (Doc()->m_curChannel.Uid == 0xff)
					pos.iChannelUid = 0xff;
				else
					pos.iChannelUid = Doc()->m_curChannel.Uid;
				wsprintf(pos.csChannelName, "%s", Doc()->m_curChannel.Name);
				if (Doc()->m_roomInfo.roomGuid == 0xffff)
					pos.iRoomIdx = 0xffff;
				else
					pos.iRoomIdx = Doc()->m_roomInfo.roomGuid;
				if (Doc()->m_roomInfo.gameType == -1)
					pos.iRoomType = -1;
				else
					pos.iRoomType = Doc()->m_roomInfo.gameType;

				WSendPacket send(0x23);
				send.EncodeBuffer(&pos, sizeof(pos));
				send.Send(TO_MSN);

				WSendPacket send2(0x14);
				send2.Send(TO_MSN);
				MESSENGER()->SetComplete((eConnectStatus)3);
				break;
			}
			}
			break;
		}
	case 0x31:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
			{
				Doc()->m_mailList.clear();
				WSendPacket send((enumClientPacket)0xe6);
				send.Send(TO_GAME);
			}
			else
			{
				Doc()->m_giftList.clear();
				WSendPacket send((enumClientPacket)0x93);
				send.Send(TO_GAME);
			}
			break;
		}
	case 0x36:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			WSendPacket send((enumClientPacket)0xe6);
			send.Send(TO_GAME);
			break;
		}
	case 0x38:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int count = packet.Decode4();
			for (int i = 0; i < count; i++)
			{
				GUILD_USER_MSN_LIST member;
				packet.DecodeBuffer(&member, sizeof(member));

				sFriend buddy;
				buddy.dwGuildId = member.guildUID;
				buddy.Uid = member.userUID;
				buddy.GuildFriend = 1;
				buddy.Gender = member.sex;
				strcpy(buddy.NickName, member.nickname);
				buddy.IsLogOn = member.online;

				BuddyMGR->AddBuddy(&buddy);
			}
			break;
		}
	case 0x39:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			GUILD_USER_MSN_LIST member;
			packet.DecodeBuffer(&member, sizeof(member));

			sFriend buddy;
			buddy.dwGuildId = member.guildUID;
			buddy.Uid = member.userUID;
			buddy.GuildFriend = 1;
			buddy.Gender = member.sex;
			strcpy(buddy.NickName, member.nickname);
			buddy.IsLogOn = member.online;

			BuddyMGR->AddBuddy(&buddy);

			IActor* pActor = GetActor(s_guildActor[1]);
			if (pActor)
			{
				BRIEFUSERINFO_MAP_IT it;
				if (Doc()->FindUserInfoUID(member.userUID, it) == 1)
				{
					sBriefUserInfo& info = (*it).second;

					GUILD_INFO* pGuild = NULL;
					pActor << MsgObject(NULL, 0x263, 0x21, (int)&pGuild, 0, 0,
						0);
					strncpy(info.szEmblemName, pGuild->guildMark, 11);
				}
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)MakeStr(
					"%s \xb4\xd4\xc0\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					member.nickname),
				0, 0, 0, 0));
			break;
		}
	case 0x3a:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned long uid = packet.Decode4();
			BuddyMGR->DeleteBuddy(uid);

			BRIEFUSERINFO_MAP_IT it;
			if (Doc()->FindUserInfoUID(uid, it) != 1)
				break;

			sBriefUserInfo& info = (*it).second;
			strcpy(info.szEmblemName, "guildmark");
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)MakeStr(
					"%s \xb4\xd4\xc0\xcc \xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					info.sNick),
				0, 0, 0, 0));
			break;
		}
	case 0x3b:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			GUILD_INFO info;
			packet.DecodeBuffer(&info, sizeof(info));

			IActor* pActor = GetActor(s_guildActor[1]);
			if (pActor == NULL)
				break;

			pActor << MsgObject(NULL, 0x263, 0x16, (int)&info, 0, 0, 0);
			break;
		}
	case 0x3c:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			GUILD_INFO info;
			packet.DecodeBuffer(&info, sizeof(info));

			IActor* pActor = GetActor(s_guildActor[1]);
			if (pActor == NULL)
				break;

			pActor << MsgObject(NULL, 0x263, 0x17, (int)&info, 0, 0, 0);
			break;
		}
	case 0x32:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			std::string nick;
			unsigned long uid = packet.Decode4();
			nick = packet.DecodeStr();

			sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
			if (pBuddy)
				strcpy(pBuddy->NickName, nick.c_str());
			break;
		}
	case 0x37:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			std::string msg("");
			std::string guildName = packet.DecodeStr();
			std::string nick = packet.DecodeStr();
			std::string text = packet.DecodeStr();

			STRING_LIST_IT it = std::find(Doc()->m_ignoreList.begin(),
				Doc()->m_ignoreList.end(), nick);
			if (it != Doc()->m_ignoreList.end())
				break;

			const char* line;
			GetActor("Lobby") << MsgObject(NULL, 0x3f, (int)&line, 0, 0, 0, 0);

			msg = "\xb1\xe6\xb5\xe5:" + nick + "> ";
			Doc()->m_chatManager.FilteringHack(text.c_str(), false);
			CChatMsg::Instance()->AddChatMsg(msg, 0xff5bffb0, false, true);
			break;
		}
	case 0x2e:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			switch (packet.Decode2())
			{
			case 0x105:
			case 0x110:
				packet.Decode1();
				break;

			case 0x117:
			{
				unsigned long result = packet.Decode4();
				if (result == 0)
				{
					std::string nick;
					packet.DecodeStr(nick);

					unsigned long uid = packet.Decode4();

					if (IS_KINDOF(CLobbyTask, AfxGetTask()) ||
						IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
						AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x51,
							(int)nick.c_str(), uid, 0, 0, 0));

					MESSENGER()->SetOfflineUserNick(nick.c_str(), uid);
				}
				else if (result == 0xd)
				{
					MESSENGER()->Notify1(
						"\xc8\xb8\xbf\xf8\xc5\xbb\xc5\xf0\xc7\xd1 \xbe\xc6\xc0\xcc\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9.");
				}
				else
				{
					MESSENGER()->Notify1(
						"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
				}
			}
			break;

			case 0x104:
				switch (packet.Decode4())
				{
				case 0:
				{
					sFriend f;
					packet.DecodeBuffer(&f, sizeof(sFriend));

					sFriend* pBuddy = BuddyMGR->GetBuddy(f.Uid);
					if (pBuddy && pBuddy->GuildFriend)
					{
						pBuddy->PangyaFriend = f.PangyaFriend;
						pBuddy->IsAccept = f.IsAccept;
						pBuddy->IsAgree = f.IsAgree;
					}
					else
					{
						BuddyMGR->AddBuddy(&f);
					}

					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8\xb5\xee\xb7\xcf\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					MESSENGER()->Refresh();
				}
				break;

				case 0xb:
					MESSENGER()->Notify1(
						"\xc0\xcc\xb9\xcc \xc4\xa3\xb1\xb8\xb7\xce \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 2:
					MESSENGER()->Notify1(
						"\xb4\xf5 \xc0\xcc\xbb\xf3 \xc4\xa3\xb1\xb8\xb5\xee\xb7\xcf\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 3:
					MESSENGER()->Notify1(
						"\xbb\xf3\xb4\xeb\xb9\xe6 \xc4\xa3\xb1\xb8\xbd\xbd\xb7\xd4\xc0\xcc \xb8\xf0\xb5\xce \xc2\xf7 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 6:
					MESSENGER()->Notify1(
						"\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xc0\xbb \xc4\xa3\xb1\xb8\xb7\xce \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 7:
					MESSENGER()->Notify1(
						"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 8:
					MESSENGER()->Notify1(
						"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9..");
					break;

				case 4:
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8 \xc7\xe3\xb0\xa1 \xb4\xeb\xb1\xe2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.");
					break;

				case 5:
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8\xc3\xdf\xb0\xa1 \xc8\xc4 \xbd\xc2\xc0\xce \xb0\xc5\xc0\xfd \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				default:
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}
				break;

			case 0x106:
			{
				sFriend f;
				packet.DecodeBuffer(&f, sizeof(sFriend));

				sFriend* pBuddy = BuddyMGR->GetBuddy(f.Uid);
				if (pBuddy && pBuddy->GuildFriend)
				{
					pBuddy->PangyaFriend = f.PangyaFriend;
					pBuddy->IsAccept = f.IsAccept;
					pBuddy->IsAgree = f.IsAgree;
				}
				else
				{
					BuddyMGR->AddBuddy(&f);
				}

				MESSENGER()->Refresh();

				std::string msg = "\xbe\xcb\xb8\xb2 : ";
				msg += f.NickName;
				msg +=
					" \xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb\xc0\xbb \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";

				const char* text = msg.c_str();
				MESSENGER()->Notify2(text);
			}
			break;

			case 0x113:
			{
				std::string msg;
				std::string nick;
				unsigned long uid = packet.Decode4();
				nick = packet.DecodeStr();
				msg = packet.DecodeStr();
				if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) &&
					packet.Decode1() == 1)
				{
					std::list<std::string>::iterator it =
						std::find(Doc()->m_ignoreList.begin(),
							Doc()->m_ignoreList.end(), nick);
					if (it != Doc()->m_ignoreList.end())
						break;

					msg = "\xb1\xe6\xb5\xe5:" + nick + "> " + "\x08" + msg;

					Doc()->m_chatManager.FilteringHack(msg.c_str(), false);

					CChatMsg::Instance()->AddChatMsg(msg, 0xff5bffb0, false,
						false);
					break;
				}

				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
				{
					if (!pBuddy->IsLogOn)
					{
						pBuddy->IsLogOn = 1;
						MESSENGER()->Refresh();
					}

					MESSENGER()->RecvChat(pBuddy, msg);
				}
			}
			break;

			case 0x114:
			{
				unsigned char result = packet.Decode1();
				if (result != 3)
					break;

				unsigned long uid = packet.Decode4();
				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy == NULL)
					break;

				MESSENGER()->RecvChat(pBuddy,
					"\xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xc0\xfc\xb4\xde\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				pBuddy->IsLogOn = 0;
				MESSENGER()->Refresh();
			}
			break;

			case 0x10b:
			{
				if (packet.Decode4())
				{
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8\xbb\xe8\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				unsigned long uid = packet.Decode4();
				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
				{
					if (pBuddy->GuildFriend)
						pBuddy->PangyaFriend = 0;
					else
						BuddyMGR->DeleteBuddy(uid);
				}

				MESSENGER()->Refresh();
			}
			break;

			case 0x10c:
			{
				if (packet.Decode4())
				{
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8\xba\xed\xb7\xb0 \xbc\xb3\xc1\xa4\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				unsigned long uid = packet.Decode4();
				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
					pBuddy->IsBlock = 1;

				MESSENGER()->Refresh();
			}
			break;

			case 0x10d:
			{
				if (packet.Decode4())
				{
					MESSENGER()->Notify1(
						"\xba\xed\xb7\xb0 \xc7\xd8\xc1\xf6\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				unsigned long uid = packet.Decode4();
				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
				{
					pBuddy->IsBlock = 0;
					pBuddy->IsBlocked = 0;
				}

				MESSENGER()->Refresh();
			}
			break;

			case 0x119:
				switch (packet.Decode4())
				{
				case 0:
				{
					unsigned long uid = packet.Decode4();
					std::string alias = packet.DecodeStr();

					Doc()->m_chatManager.FilteringHack(alias.c_str(), false);

					sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
					if (pBuddy)
						strncpy(pBuddy->szAlias, alias.c_str(), 10);
				}
				break;

				case 9:
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee\xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 1:
					MESSENGER()->Notify1(
						"\xba\xb0\xb8\xed\xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}
				break;

			case 0x10e:
			{
				if (!IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
				{
					sFriend f;
					packet.DecodeBuffer(&f, sizeof(sFriend));

					sFriend* pBuddy = BuddyMGR->GetBuddy(f.Uid);
					if (pBuddy == NULL)
						break;

					if (f.GuildFriend)
					{
						if (f.PangyaFriend)
						{
							if (pBuddy->IsBlocked)
								break;

							pBuddy->PangyaFriend = 1;
							pBuddy->GuildFriend = 1;
							pBuddy->IsLogOn = 1;
						}
						else
						{
							BuddyMGR->AddBuddy(&f);
						}

						MESSENGER()->Notify2(MakeStr(
							"%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
							f.NickName));
					}
					else
					{
						if (pBuddy->IsBlocked || !pBuddy->IsAccept)
							break;

						pBuddy->IsLogOn = 1;

						MESSENGER()->Notify2(MakeStr(
							"%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
							pBuddy->NickName));
					}

					MESSENGER()->Refresh();
				}
				else
				{
					sFriend* pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
					if (pBuddy == NULL)
						break;

					if (pBuddy->GuildFriend)
					{
						if (pBuddy->PangyaFriend)
						{
							if (pBuddy->IsBlocked)
								break;

							pBuddy->IsLogOn = 1;
						}

						MESSENGER()->Notify2(MakeStr(
							"%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
							pBuddy->NickName));
					}
					else
					{
						pBuddy->IsLogOn = 1;
						if (pBuddy->IsBlocked || !pBuddy->IsAccept)
							break;

						MESSENGER()->Notify2(MakeStr(
							"%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
							pBuddy->NickName));
					}

					MESSENGER()->Refresh();
					if (pBuddy)
						break;
				}
			}
			break;

			case 0x10f:
			{
				unsigned long uid = packet.Decode4();
				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
				{
					if (!IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) &&
						pBuddy->GuildFriend && !pBuddy->PangyaFriend)
						BuddyMGR->DeleteBuddy(uid);
					else
						pBuddy->IsLogOn = 0;
				}

				MESSENGER()->Refresh();
			}
			break;

			case 0x102:
			{
				if (packet.Decode1() == 1)
					BuddyMGR->ClearBuddy();

				packet.Decode2();
				unsigned short count = packet.Decode2();

				sFriend f;
				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&f, sizeof(sFriend));
					if (f.Uid != Doc()->m_myInfo.info.dwUID)
						BuddyMGR->AddBuddy(&f);
				}

				MESSENGER()->SetComplete((eConnectStatus)4);
				MESSENGER()->Refresh();
			}
			break;

			case 0x109:
			{
				unsigned long result = packet.Decode4();
				if (result == 9)
				{
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee\xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				if (result)
				{
					MESSENGER()->Notify1(
						"\xc4\xa3\xb1\xb8 \xc7\xe3\xbf\xeb\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				sFriend* pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
				if (pBuddy == NULL)
					break;

				pBuddy->IsAccept = 1;
				pBuddy->IsAgree = 1;
				strcpy(pBuddy->szAlias, "Friend");

				MESSENGER()->Notify2(MakeStr(
					"%s\xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xb0\xa1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
					pBuddy));
				MESSENGER()->Refresh();
			}
			break;

			case 0x10a:
			{
				std::string unused;
				if (!IS_SUCCESSFUL((Cm::eErrorCode)packet.Decode4()))
					break;

				sFriend* pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
				if (pBuddy)
				{
					pBuddy->IsLogOn = 1;
					pBuddy->IsAccept = 1;
					pBuddy->Server = 0;
					strcpy(pBuddy->szAlias, "Friend");

					MESSENGER()->Notify2(MakeStr(
						"%s\xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xb0\xa1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
						pBuddy));
				}

				MESSENGER()->Refresh();
			}
			break;

			case 0x103:
			{
				Doc()->m_noteList.clear();

				unsigned char count = packet.Decode1();
				for (unsigned char i = 0; i < count; i++)
				{
					sNoteInfo note;
					packet.DecodeBuffer(&note, sizeof(sNoteInfo));

					Doc()->m_chatManager.FilteringHack(note.sNote, false);
					Doc()->m_noteList.push_back(note);
				}

				MESSENGER()->Notify1(
					"\xc2\xca\xc1\xf6\xb0\xa1 \xb5\xb5\xc2\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
			}
			break;

			case 0x112:
				switch (packet.Decode1())
				{
				case 0:
					packet.Decode1();
					Doc()->m_myInfo.stat.i64Pang -= 10;

					MESSENGER()->Notify1(
						"\xc2\xca\xc1\xf6\xb8\xa6 \xb3\xb2\xb0\xe5\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb4\xd9\xc0\xbd \xb7\xce\xb1\xd7\xc0\xce\xc7\xd2 \xb6\xa7 \xc0\xfc\xb4\xde\xb5\xcb\xb4\xcf\xb4\xd9.");
					break;

				case 3:
					MESSENGER()->Notify1(
						"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc2\xca\xc1\xf6\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6\xc2\xf7\xbc\xad \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;

				case 1:
					MESSENGER()->Notify1(
						"\xc2\xca\xc1\xf6 \xba\xb8\xb3\xbb\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}
				break;

			case 0x118:
			{
				WNetworkSystem* pNetwork = NET();
				if (pNetwork->IsConnected(WNetworkSystem::NET_MSN))
					pNetwork->ForceShutDown(WNetworkSystem::NET_MSN);

				FrForm* pForm =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				pForm->SetMessage(
					"\xb4\xd9\xb8\xa5 \xbb\xe7\xbf\xeb\xc0\xda\xb0\xa1 \xc7\xf6\xc0\xe7 \xb0\xe8\xc1\xa4\xc0\xb8\xb7\xce \xc1\xa2\xbc\xd3\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open((FRESH_PFN_RESULT)&CTask::OnOverlappedDlg, 3);
			}
			break;

			case 0x123:
			{
				packet.Decode4();
				unsigned long uid = packet.Decode4();

				sUserPosition pos;
				packet.DecodeBuffer(&pos, sizeof(sUserPosition));

				sFriend* pBuddy = BuddyMGR->GetBuddy(uid);
				if (pBuddy)
					pBuddy->userPosition = pos;
			}
			break;

			case 0x115:
			{
				unsigned long state = packet.Decode4();
				unsigned long guid = packet.Decode4();

				sFriend* pBuddy = BuddyMGR->GetBuddy(guid);
				if (pBuddy)
				{
					pBuddy->Guid = guid;
					switch (state)
					{
					case 0:
						pBuddy->IsLogOn = 1;
						pBuddy->IsPlay = 1;
						break;

					case 1:
						pBuddy->IsLogOn = 1;
						pBuddy->IsDive = 1;
						break;

					case 3:
						pBuddy->IsLogOn = 1;
						pBuddy->IsBusy = 1;
						break;

					case 4:
						pBuddy->IsPlay = 0;
						pBuddy->IsDive = 0;
						pBuddy->IsBusy = 0;
						if (!pBuddy->IsLogOn)
							MESSENGER()->Notify2(MakeStr(
								"%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
								pBuddy));
						pBuddy->IsLogOn = 1;
						break;

					case 5:
						pBuddy->IsPlay = 0;
						pBuddy->IsDive = 0;
						break;
					}
				}

				MESSENGER()->Refresh();
			}
			break;

			case 0x122:
			{
				WSendPacket send(0x16);
				send.Send(TO_MSN);

				MESSENGER()->Close(true);

				if (NET()->IsConnected(WNetworkSystem::NET_MSN))
					NET()->ForceShutDown(WNetworkSystem::NET_MSN);

				MESSENGER()->Notify2(
					"\xb8\xde\xbd\xc5\xc0\xfa\xbc\xad\xb9\xf6\xbf\xa1 \xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe7 \xc1\xa2\xbc\xd3\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9");
			}
			break;
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x48, 0, 0, 0, 0, 0));
		}
		break;
	case 0x98:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			int identity = packet.Decode4();
			Doc()->SetIdentity(identity);

			if (IsPcBang())
				Doc()->AccumulateIdentity(0x80);

			if (IS_KINDOF(CGolfTask, AfxGetTask()))
			{
				unsigned long myIdentity = Doc()->m_myInfo.info.dwIdentity;
				Doc()->m_userInfo[MyIndex()].info.dwIdentity = myIdentity;
			}
		}
		break;
	case 0xaf:
		LogOut(0, "CTask::OnPacketCommon %d %d %d %d %d\n", type, type, type,
			type, type);
		{
			unsigned char count = packet.Decode1();
			std::list<sItemInfo> items;

			for (unsigned char i = 0; i < count; i++)
			{
				sItemInfo item;
				packet.DecodeBuffer(&item, sizeof(sItemInfo));
				items.push_back(item);
			}

			for (std::list<sItemInfo>::iterator it = items.begin();
				it != items.end(); ++it)
			{
				bool bFound = false;
				for (std::list<sItemInfo>::iterator it2 =
						 Doc()->m_myItemList.begin();
					it2 != Doc()->m_myItemList.end(); ++it2)
				{
					if ((*it2).tid == (*it).tid)
					{
						(*it2).Common[0] = (*it).Common[0];
						bFound = true;
						break;
					}
				}

				if (!bFound)
					Doc()->m_myItemList.push_back(*it);
			}

			if (MESSENGER()->GetMyStatus() != 3)
				MESSENGER()->SetMyStatus((MSN::eStatusType)4);
		}
		break;
	case 0xb2:
	{
		unsigned char type = packet.Decode1();
		packet.Decode1();
		unsigned char count = packet.Decode1();

		std::list<sSpecialTrophy>* pList = NULL;
		switch (type)
		{
		case 0:
			pList = &Doc()->m_oldSpecialTrophyList;
			break;

		case 4:
		case 5:
			pList = &Doc()->m_specialTrophyList;
			break;

		default:
			return 1;
		}

		if (pList == NULL)
			break;

		for (unsigned char i = 0; i < count; i++)
		{
			sSpecialTrophy trophy;
			packet.DecodeBuffer(&trophy, sizeof(sSpecialTrophy));
			pList->push_back(trophy);
		}
	}
	break;
	case 0x16f:
	{
		unsigned char type = packet.Decode1();

		sTrophyStatistics* pTrophy = NULL;
		switch (type)
		{
		case 0:
			pTrophy = &Doc()->m_oldSeasonTrophy;
			break;

		case 4:
		case 5:
			pTrophy = &Doc()->m_myInfo.trophy;
			break;

		default:
			return 1;
		}

		if (pTrophy == NULL)
			break;

		packet.DecodeBuffer(pTrophy, sizeof(sTrophyStatistics));
	}
	break;
	case 0xd6:
	{
		packet.Decode1();
		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++)
		{
			sGuildTrophy trophy;
			packet.DecodeBuffer(&trophy, sizeof(sGuildTrophy));
			Doc()->m_guildTrophyList.push_back(trophy);
		}
	}
	break;
	case 0xb8:
	{
		SYSTEMTIME serverTime;
		packet.DecodeBuffer(&serverTime, sizeof(SYSTEMTIME));
		Doc()->SetServerTime(serverTime);
	}
	break;
	case 0xc7:
	{
		std::string nick = packet.DecodeStr();
		std::string msg = packet.DecodeStr();

		Doc()->m_chatManager.FilteringHack(msg.c_str(), false);

		COneLineBoard::Instance()->AddOnelineMsg(nick, msg);
	}
	break;
	case 0x94:
		Doc()->m_bonusCash = packet.Decode8();
		Doc()->RefreshAllCookie();
		break;

	case 0xc9:
		if (packet.Decode4())
		{
			Doc()->m_bonusCash = packet.Decode8();
			Doc()->RefreshAllCookie();
			break;
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		break;
	case 0xc8:
	{
		int type = packet.Decode2();
		unsigned long value = packet.Decode4();

		AfxGetTask()->SendMsgToMainActor(
			MsgObject(NULL, 0x5d, type, value, 0, 0, 0));
	}
	break;
	case 0xcc:
	{
		if (packet.Decode1())
			Doc()->m_pangRate *= 2.0f;

		unsigned short count = packet.Decode2();

		Doc()->m_acquireItemList[0].clear();

		for (int i = 0; i < count; i++)
			Doc()->m_acquireItemList[0].push_back(packet.Decode4());
	}
	break;
	case 0xf8:
	{
		for (int i = 0; i < 4; i++)
			Doc()->m_acquireItemList[i].clear();

		unsigned short count = packet.Decode2();
		for (int j = 0; j < count; j++)
		{
			unsigned long guid = packet.Decode4();
			unsigned char bDouble = packet.Decode1();
			unsigned short itemCount = packet.Decode2();

			if (guid == MyGuid(false))
			{
				if (bDouble)
					Doc()->m_pangRate *= 2.0f;
			}

			int index = -1;
			for (int k = 0; k < 4; k++)
			{
				if (guid == Doc()->m_userInfo[k].info.dwGuid)
				{
					index = k;
					break;
				}
			}

			for (int n = 0; n < itemCount; n++)
			{
				unsigned long tid = packet.Decode4();
				if (index != -1 && tid)
					Doc()->m_acquireItemList[index].push_back(tid);
			}
		}
	}
	break;
	case 0x1ae:
		if (packet.Decode1())
		{
			unsigned long interval = packet.Decode4();
			CTaskManager::Instance()->SetWork(new CWorkAliveCheck(interval));
		}
		else
		{
			CTaskManager::Instance()->ReleaseWork();
		}
		break;
	case 0xd5:
		_GGC_CSAuth2(packet);
		break;
	case 0xcf:
		Doc()->m_bNewMapEvent = true;
		Doc()->m_newMapEventMask = packet.Decode1();

		if (IS_KINDOF(CLobbyTask, AfxGetTask()))
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 0xb2, 0, 0, 0, 0, 0);
		break;
	case 0xd0:
		Doc()->m_bBingoEvent = true;

		if (IS_KINDOF(CLobbyTask, AfxGetTask()))
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 0xb3, 0, 0, 0, 0, 0);
		break;
	case 0xd8:
		Doc()->m_controlServerService = packet.Decode4();

		if (IsLocalContent(S4_REAL_MYROOM))
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 0x22f, 0, 0, 0, 0, 0);
		break;
	case 0xdb:
	{
		unsigned long result = packet.Decode4();

		FrWnd* pWnd =
			g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg");
		FrScratchDlg* pDlg = NULL;
		if (pWnd == NULL)
			__rtti_obj = NULL;
		else
			pDlg = (FrScratchDlg*)pWnd;

		if (result)
		{
			if (result != 0x1b)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1 \xc0\xcc\xbf\xeb \xbf\xc0\xb7\xf9\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc3\xb4\xc2\xb5\xa5 \xba\xd2\xc6\xed\xc0\xbb \xb5\xe5\xb7\xc1 \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9. \xc0\xe7\xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbd\xc3\xb8\xe9 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
			break;
		}

		int count = packet.Decode4();

		sBonusBall balls[11] = { 0 };
		for (int i = 0; i < count; i++)
			packet.DecodeBuffer(&balls[i], sizeof(sBonusBall));

		if (pDlg == NULL)
			break;

		pDlg->EnableNextPageButton(true);
		pDlg->SetScratchItem(count, balls);
		pDlg->InitChangeScratchCardNumber(0);

		if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
			AfxPostMsg(NULL, "RealMyRoom", 0xe1, 0, 0, 0, 0);
	}
	break;
	case 0xdc:
	{
		unsigned long result = packet.Decode4();

		FrWnd* pWnd =
			g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg");
		FrScratchDlg* pDlg = NULL;
		if (pWnd == NULL)
		{
			__rtti_obj = NULL;
		}
		else
		{
			pDlg = (FrScratchDlg*)pWnd;
			pDlg->EnableScratchSerialNumber(true);
		}

		switch (result)
		{
		case 0:
		{
			unsigned long code = packet.Decode4();
			unsigned long typeId = packet.Decode4();

			if (pDlg == NULL)
				break;

			pDlg->ResponseScratchSerialNumber(code, typeId);

			if (code == 0)
				pDlg->InitChangeScratchCardNumber(1);
		}
		break;

		case 1:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc8 \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;

		case 2:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3 \xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;

		case 3:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xaf\xc8\xbf\xb1\xe2\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xad \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;

		case 4:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc3\xd6\xb4\xeb \xc1\xf6\xb1\xde \xc8\xbd\xbc\xf6\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;

		default:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbf\xc3\xb9\xd9\xb8\xa5 \xc4\xed\xc6\xf9 \xb9\xf8\xc8\xa3\xb0\xa1 \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		}
	}
	break;
	case 0xdd:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		if (item.guid == 0)
			break;
		if (item.tid == 0)
			break;
		if (item.Common[0] <= 0)
			break;

		bool bFound = false;
		std::list<sItemInfo>::iterator it;
		for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
			++it)
		{
			if ((*it).guid == item.guid && (*it).tid == item.tid)
			{
				(*it).Common[0] = item.Common[0];
				bFound = true;
				break;
			}
		}

		if (!bFound)
			Doc()->m_myItemList.push_back(item);
	}
	break;
	case 0xde:
		Doc()->m_packetNickname = packet.DecodeStr();
		break;
	case 0x16d:
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xc6\xce\xbe\xdf\xb8\xb6\xc4\xcf \xb9\xd7 \xc1\xf8\xbf\xad\xb4\xeb \xb1\xe2\xb4\xc9\xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		break;
	case 0x164:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		if (!IsLocalContent(S4_OFFLINE_SHOP))
			break;

		int result = packet.Decode2();
		unsigned char mode = packet.Decode1();
		unsigned char bSave = packet.Decode1();

		if (result == 1)
		{
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x22f, 0, 0, 0, 0, 0));

			if (bSave)
			{
				if (mode == 1)
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
						(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb9\xe8\xc4\xa1 \xbc\xba\xb0\xf8",
						0, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
						(int)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc1\xa6\xb0\xc5\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc7\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
			}
			else
			{
				if (mode == 1)
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
						(int)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xb9\xe8\xc4\xa1\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf \xb5\xcb\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
				else
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
						(int)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc1\xa6\xb0\xc5\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc7\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
			}
		}
		else
		{
			if (mode == 1)
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb9\xe8\xc4\xa1 \xbd\xc7\xc6\xd0",
					0, 0, 0, 0));
			else
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb \xc1\xa6\xb0\xc5 \xbd\xc7\xc6\xd0",
					0, 0, 0, 0));
		}
	}
	break;
	case 0x16c:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		if (!IsLocalContent(S4_OFFLINE_SHOP))
			break;

		unsigned long uid = packet.Decode4();
		unsigned long stallKey = packet.Decode4();
		unsigned char result = packet.Decode1();

		if (result != 1)
			break;

		if (MyUID() == uid)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xda\xbd\xc5\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb4\xc2 \xb1\xb8\xb0\xe6\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		}

		std::list<sStall>* pList = Doc()->GetListStallItem();

		sStall stall;
		STALL_LIST_IT it;
		for (it = pList->begin(); it != pList->end(); it++)
		{
			if ((*it).stallKey == stallKey)
			{
				Doc()->SetStallItem(*it);
				break;
			}
		}

		Doc()->SetTradeMode(1);

		if (Doc()->GetStallItem()->stallKey)
		{
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"offlinetradedlg");
			if (pWnd)
				break;

			__rtti_obj = NULL;

			COfflineShopVisitorDlg* pDlg = CreateForm<COfflineShopVisitorDlg>(
				g_pFresh->GetManager(), this, "offlinetradedlg");
			if (pDlg == NULL)
				break;

			pDlg->Open(NULL, 0xc0);
			pDlg->OpenTradeShop(Doc()->m_replayTargetUID);
			pDlg->SetListTradeItem(false);
			pDlg->SetFadeout(false);
		}
		else
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
	}
	break;
	case 0x167:
	{
		unsigned long uid = packet.Decode4();
		unsigned long stallKey = packet.Decode4();
		int slot = packet.Decode1();

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		std::list<sStall>* pList = Doc()->GetListStallItem();
		sStall* pStall = Doc()->GetStallItem();

		if (Doc()->m_replayTargetUID != uid)
			break;

		if (pStall->stallKey == stallKey)
		{
			if (pStall->slotList[slot - 1].itemData.iNum == 0)
				memset(&pStall->slotList[slot - 1].itemData, 0,
					sizeof(sTradeItem));

			Doc()->SetStallItem(*pStall);
			Doc()->ChageListStallitem(pStall);
		}
		else
		{
			if (pList == NULL)
				break;

			STALL_LIST_IT it;
			it = pList->begin();
			if (it != pList->end() && (*it).stallKey == stallKey &&
				(*it).maxSlotNum > 0 && (*it).slotList[0].Idx == slot)
				memset(&(*it).slotList[0].itemData, 0, sizeof(sTradeItem));
			else
				break;
		}

		if (MyUID() == Doc()->m_replayTargetUID)
		{
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"tradeeditdlg");
			if (pWnd == NULL)
				__rtti_obj = NULL;
			else
				((COfflineShopEditDlg*)pWnd)->SetListTradeItem();

			pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"tradebuydlg");
			if (pWnd == NULL)
				__rtti_obj = NULL;
			else
				((FrOfflineTradeFinishEditDlg*)pWnd)->SetListTradeItem(NULL);

			pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"offlinetradedlg");
			COfflineShopDlg* pShopDlg = pWnd
				? (COfflineShopDlg*)pWnd
				: DYNAMIC_CAST(COfflineShopDlg, pWnd);
			if (pShopDlg)
			{
				pShopDlg->OpenTradeShop(Doc()->m_replayTargetUID);
				pShopDlg->SetListTradeItem(false);
			}
		}
		else
		{
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"tradebuydlg");
			if (pWnd == NULL)
				__rtti_obj = NULL;
			else
				pWnd->Close(true);

			pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"offlinetradedlg");
			COfflineShopVisitorDlg* pVisitorDlg = pWnd
				? (COfflineShopVisitorDlg*)pWnd
				: DYNAMIC_CAST(COfflineShopVisitorDlg, pWnd);
			if (pVisitorDlg)
			{
				pVisitorDlg->OpenTradeShop(Doc()->m_replayTargetUID);
				pVisitorDlg->SetListTradeItem(false);
			}
		}
	}
	break;
	case 0x161:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			if (!IsLocalContent((localContentType_t)102))
				break;

			int result = packet.Decode4();
			int error = packet.Decode4();

			if (result == 1)
			{
				__int64 pang = packet.Decode8();
				Doc()->m_myInfo.stat.i64Pang = pang;

				if (MyUID() == Doc()->m_replayTargetUID)
					break;

				WSendPacket send(0x21);
				send.EncodeStr(std::string(MyNick()));
				send.Encode4(MyUID());
				send.Send(TO_GAME);
				break;
			}

			switch (error)
			{
			case 0:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb \xc0\xcc\xbf\xeb\xbf\xa1 \xc1\xa6\xc0\xe7\xb8\xa6 \xb9\xde\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 1:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbc\xba\xb0\xf8",
					0, 0, 0, 0));
				break;
			case 2:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbd\xc7\xc6\xd0",
					0, 0, 0, 0));
				break;
			case 3:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb5\xee\xb7\xcf\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(Error DB)",
					0, 0, 0, 0));
				break;
			case 4:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xe1\xb0\xfa\xb0\xaa\xc0\xbb \xb9\xde\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 5:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0, 0, 0));
				break;
			case 6:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 \xbd\xbd\xb7\xd4 \xb5\xa5\xc0\xcc\xc5\xb8\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 7:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 8:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc0\xcc\xb9\xcc \xbf\xb5\xb1\xb8 \xbd\xbd\xb7\xd4\xc0\xb8\xb7\xce \xbf\xac\xc0\xe5\xb9\xd7 \xb1\xb8\xb8\xc5\xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 9:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 10:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb3\xbc\xf6\xb0\xa1 \xbc\xd2\xc0\xaf\xc7\xd1 \xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 \xb8\xb9\xbe\xc6\xbc\xad \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
					0, 0, 0, 0));
				break;
			case 12:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 13:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbd\xbd\xb7\xd4\xbf\xa1 \xc0\xcc\xb9\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xb5\xee\xb7\xcf \xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xc7\xd8\xc1\xf6 \xc0\xcc\xc8\xc4 \xb4\xd9\xbd\xc3 \xb5\xee\xb7\xcf\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4",
					0, 0, 0, 0));
				break;
			case 14:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 15:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc0\xcc\xb9\xcc \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 16:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbd\xbd\xb7\xd4 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 17:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb7\xae\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9. \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb9\xbc\xf6\xb8\xa6 \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 18:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc6\xc7\xb8\xc5\xc0\xda\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc6\xed\xc1\xfd \xb8\xf0\xb5\xe5\xb7\xce \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			case 11:
			case 19:
			{
				if (MyUID() == Doc()->m_replayTargetUID)
				{
					FrWnd* pEdit =
						g_pFresh->GetManager()->GetDesktop()->FindChildByName(
							"tradeeditdlg");
					if (pEdit == NULL)
						__rtti_obj = NULL;
					else
						pEdit->Close(true);

					FrWnd* pBuy =
						g_pFresh->GetManager()->GetDesktop()->FindChildByName(
							"tradebuydlg");
					if (pBuy == NULL)
						__rtti_obj = NULL;
					else
						pBuy->Close(true);

					FrWnd* pShop =
						g_pFresh->GetManager()->GetDesktop()->FindChildByName(
							"offlinetradedlg");
					if (pShop == NULL)
						__rtti_obj = NULL;
					else
						pShop->Close(true);
				}
				else
				{
					FrWnd* pBuy =
						g_pFresh->GetManager()->GetDesktop()->FindChildByName(
							"tradebuydlg");
					if (pBuy == NULL)
						__rtti_obj = NULL;
					else
						pBuy->Close(true);

					FrWnd* pShop =
						g_pFresh->GetManager()->GetDesktop()->FindChildByName(
							"offlinetradedlg");
					if (pShop == NULL)
						__rtti_obj = NULL;
					else
						pShop->Close(true);
				}

				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xb4\xd9\xbd\xc3 \xbf\xad\xbe\xee \xc1\xd6\xbc\xbc\xbf\xe4",
					0, 0, 0, 0));
			}
			break;
			}
		}
		break;

	case 0x165:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
	case 0x166:
	{
		int page = packet.Decode2();
		int count = packet.Decode2();

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		if (count != 0)
		{
			CTask* pTask = AfxGetTask();
			((CTaskMain*)pTask->GetMainActor())->CreateMaketDlg();

			COfflineTradeDlg* pDlg =
				(COfflineTradeDlg*)g_pFresh->GetManager()
					->GetDesktop()
					->FindChildByName("offlinetraderanking");

			Doc()->ClearMaketList();

			for (int i = 0; i < count; i++)
			{
				static sSaleItem saleItem;
				packet.DecodeBuffer(&saleItem, sizeof(sSaleItem));
				Doc()->AddMaketListItem(saleItem);
			}

			if (pDlg == NULL)
			{
				__rtti_obj = NULL;
				break;
			}

			pDlg->SetListItem();
			pDlg->SetPageData(-1, page);
			break;
		}

		COfflineTradeDlg* pDlg = (COfflineTradeDlg*)g_pFresh->GetManager()
									 ->GetDesktop()
									 ->FindChildByName("offlinetraderanking");

		Doc()->ClearMaketList();

		if (pDlg == NULL)
			__rtti_obj = NULL;
		else
		{
			pDlg->SetListItem();
			pDlg->SetPageData(1, 1);
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xb8\xb6\xc4\xcf\xbf\xa1\xbc\xad \xb0\xcb\xbb\xf6\xb5\xc8 \xbb\xf3\xc7\xb0\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}
	break;

	case 0x162:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
	case 0x163:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		if (!IsLocalContent((localContentType_t)102))
			break;

		int result = packet.Decode4();
		int error = packet.Decode4();

		if (result == 1)
		{
			if (type == 0x162)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb5\xee\xb7\xcf \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
			if (type == 0x163)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xa6\xb0\xc5 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
		}

		switch (error)
		{
		case 0:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb \xc0\xcc\xbf\xeb\xbf\xa1 \xc1\xa6\xc0\xe7\xb8\xa6 \xb9\xde\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 1:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbc\xba\xb0\xf8",
				0, 0, 0, 0));
			break;
		case 2:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbd\xc7\xc6\xd0",
				0, 0, 0, 0));
			break;
		case 3:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb \xb5\xee\xb7\xcf\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(Error DB)",
				0, 0, 0, 0));
			break;
		case 4:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xe1\xb0\xfa\xb0\xaa\xc0\xbb \xb9\xde\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 5:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0));
			break;
		case 6:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 \xbd\xbd\xb7\xd4 \xb5\xa5\xc0\xcc\xc5\xb8\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 7:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 8:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xcc\xb9\xcc \xbf\xb5\xb1\xb8 \xbd\xbd\xb7\xd4\xc0\xb8\xb7\xce \xbf\xac\xc0\xe5\xb9\xd7 \xb1\xb8\xb8\xc5\xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 9:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 10:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb3\xbc\xf6\xb0\xa1 \xbc\xd2\xc0\xaf\xc7\xd1 \xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 \xb8\xb9\xbe\xc6\xbc\xad \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
				0, 0, 0, 0));
			break;
		case 12:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 13:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbd\xbd\xb7\xd4\xbf\xa1 \xc0\xcc\xb9\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xb5\xee\xb7\xcf \xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xc7\xd8\xc1\xf6 \xc0\xcc\xc8\xc4 \xb4\xd9\xbd\xc3 \xb5\xee\xb7\xcf\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0));
			break;
		case 14:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 15:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xcc\xb9\xcc \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 16:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbd\xbd\xb7\xd4 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 17:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb7\xae\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9. \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb9\xbc\xf6\xb8\xa6 \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 18:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc6\xc7\xb8\xc5\xc0\xda\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc6\xed\xc1\xfd \xb8\xf0\xb5\xe5\xb7\xce \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			break;
		case 11:
		{
			if (MyUID() == Doc()->m_replayTargetUID)
			{
				FrWnd* pEdit =
					g_pFresh->GetDesktop()->FindChildByName("tradeeditdlg");
				if (pEdit == NULL)
					__rtti_obj = NULL;
				else
					pEdit->Close(true);

				FrWnd* pBuy =
					g_pFresh->GetDesktop()->FindChildByName("tradebuydlg");
				if (pBuy == NULL)
					__rtti_obj = NULL;
				else
					pBuy->Close(true);

				FrWnd* pShop =
					g_pFresh->GetDesktop()->FindChildByName("offlinetradedlg");
				if (pShop == NULL)
					__rtti_obj = NULL;
				else
					pShop->Close(true);
			}
			else
			{
				FrWnd* pBuy =
					g_pFresh->GetDesktop()->FindChildByName("tradebuydlg");
				if (pBuy == NULL)
					__rtti_obj = NULL;
				else
					pBuy->Close(true);

				FrWnd* pShop =
					g_pFresh->GetDesktop()->FindChildByName("offlinetradedlg");
				if (pShop == NULL)
					__rtti_obj = NULL;
				else
					pShop->Close(true);
			}

			WSendPacket send((enumClientPacket)0xc4);
			send.Encode4(Doc()->m_replayTargetUID);
			send.Send(TO_GAME);

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xb4\xd9\xbd\xc3 \xbf\xad\xbe\xee \xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0));
		}
		break;
		}
	}
	break;

	case 0x169:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			if (!IsLocalContent((localContentType_t)102))
				break;

			packet.Decode4();
			unsigned char slot = packet.Decode1();

			static sTradeItem tradeItem;

			packet.DecodeBuffer(&tradeItem, sizeof(sTradeItem));

			sStall* pStall = Doc()->GetStallItem();

			if (tradeItem.dwTid == 0 && pStall->slotList[slot - 1].IsValid)
			{
				FILETIME end, now;
				SystemTimeToFileTime(&pStall->slotList[slot - 1].time, &end);
				SystemTimeToFileTime(&Doc()->GetServerTime(), &now);

				if ((((__int64)end.dwHighDateTime - now.dwHighDateTime) *
							0x100000000i64 -
						now.dwLowDateTime + end.dwLowDateTime) /
						60 / 60 / 10000000 <=
					0)
					pStall->slotList[slot - 1].IsValid = 0;
			}

			pStall->slotList[slot - 1].itemData = tradeItem;

			Doc()->SetStallItem(*pStall);

			Doc()->ChageListStallitem(pStall);

			FrWnd* pEdit =
				g_pFresh->GetManager()->GetDesktop()->FindChildByName(
					"tradeeditdlg");
			if (pEdit == NULL)
				__rtti_obj = NULL;
			else
			{
				((COfflineShopEditDlg*)pEdit)->m_bExit = 1;
				pEdit->Close(true);

				COfflineShopDlg* pShop = CreateForm<COfflineShopDlg>(
					g_pFresh->GetManager(), this, "offlinetradedlg");
				if (pShop)
				{
					pShop->Open(NULL, FrRET_OK | FrRET_CANCEL);
					pShop->OpenTradeShop(Doc()->m_replayTargetUID);
					pShop->SetListTradeItem(false);
					pShop->SetFadeout(false);
				}
			}

			FrWnd* pBuy = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"tradebuydlg");
			if (pBuy == NULL)
				__rtti_obj = NULL;
			else
				((FrOfflineTradeFinishEditDlg*)pBuy)->SetListTradeItem(NULL);

			COfflineShopDlg* pShop = (COfflineShopDlg*)g_pFresh->GetManager()
										 ->GetDesktop()
										 ->FindChildByName("offlinetradedlg");
			if (pShop == NULL)
			{
				__rtti_obj = NULL;
				break;
			}

			pShop->OpenTradeShop(Doc()->m_replayTargetUID);
			pShop->SetListTradeItem(false);
		}
		break;

	case 0xe1:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();

			if (result == 1)
			{
				std::string nick;
				packet.DecodeStr(nick);
				GetActor("AvatarChat")
					<< MsgObject(NULL, 0xe8, (int)nick.c_str(), 1, 0, 0, 0);

				if (strcmp(nick.c_str(), MyNick()) == 0)
					Doc()->SetStateTrade(STATE_TRADE_OPEN);
			}

			SetTradeResultCode(result, 0xe1, NULL);
		}
		break;

	case 0xe2:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();

			if (result == 1)
			{
				std::string nick;

				packet.DecodeStr(nick);
				unsigned long uid = packet.Decode4();

				GetActor("AvatarChat")
					<< MsgObject(NULL, 0xe8, (int)nick.c_str(), 0, 0, 0, 0);

				if (uid == Doc()->GetTradeUID() || uid == MyUID())
				{
					Doc()->InitTradeData();

					FrWnd* pTrade =
						g_pFresh->GetDesktop()->FindChildByName("tradedlg");
					if (pTrade == NULL)
						__rtti_obj = NULL;
					else
						pTrade->Close(true);

					if (strcmp(nick.c_str(), MyNick()) == 0)
					{
						FrWnd* pEdit = g_pFresh->GetDesktop()->FindChildByName(
							"tradeeditdlg");
						if (pEdit == NULL)
							__rtti_obj = NULL;
						else
							pEdit->Close(true);
					}
					else
					{
						result = 3;
						Doc()->InitTradeData();

						FrWnd* pBuy = g_pFresh->GetDesktop()->FindChildByName(
							"tradebuydlg");
						if (pBuy == NULL)
							__rtti_obj = NULL;
						else
						{
							FrWnd* pNotify =
								g_pFresh->GetDesktop()->FindChildByName(
									"notify_okcancel");
							if (pNotify)
								pNotify->Close(true);

							pBuy->Close(true);
						}
					}
				}
			}

			SetTradeResultCode(result, 0xe2, NULL);
		}
		break;

	case 0xe3:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();

			if (result == 1)
			{
				std::string nick;

				packet.DecodeStr(nick);
				unsigned long uid = packet.Decode4();

				GetActor("AvatarChat")
					<< MsgObject(NULL, 0xe8, (int)nick.c_str(), 2, 0, 0, 0);

				if (strcmp(nick.c_str(), MyNick()) == 0)
				{
					Doc()->SetStateTrade(STATE_TRADE_EDIT);

					FrWnd* pTrade =
						g_pFresh->GetDesktop()->FindChildByName("tradedlg");
					if (pTrade == NULL)
						__rtti_obj = NULL;
					else
						pTrade->Close(true);

					FrWnd* pEdit =
						g_pFresh->GetDesktop()->FindChildByName("tradeeditdlg");
					if (pEdit == NULL)
					{
						__rtti_obj = NULL;
						FrTradeEditDlg* pDlg = CreateForm<FrTradeEditDlg>(
							g_pFresh->GetManager(), this, "tradeeditdlg");
						if (pDlg)
						{
							pDlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
							pDlg->SetFadeout(false);
						}
					}
				}
				else if (uid == Doc()->GetTradeUID())
				{
					result = 4;
					Doc()->InitTradeData();

					FrWnd* pTrade =
						g_pFresh->GetDesktop()->FindChildByName("tradedlg");
					if (pTrade == NULL)
						__rtti_obj = NULL;
					else
						pTrade->Close(true);

					FrWnd* pBuy =
						g_pFresh->GetDesktop()->FindChildByName("tradebuydlg");
					if (pBuy == NULL)
						__rtti_obj = NULL;
					else
						pBuy->Close(true);
				}

				SetTradeResultCode(result, 0xe3, NULL);
			}
			else
				SetTradeResultCode(result, 0xe3, NULL);
		}
		break;

	case 0xe4:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			eTypeSalesTrade saleType = eTypeSalesTrade_None;

			int result = packet.Decode4();

			if (result == 1)
			{
				Doc()->InitTradeData();
				Doc()->SetStateTrade(STATE_TRADE_ENTER);

				saleType = (eTypeSalesTrade)packet.Decode4();

				std::string title("");
				char nick[22] = { 0 };
				packet.DecodeBuffer(nick, sizeof(nick));
				packet.DecodeStr(title);

				unsigned long uid = packet.Decode4();
				Doc()->SetTradeUID(uid);
				Doc()->SetTradeTitle(title);

				sTradeItem item;
				int count = packet.Decode4();

				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&item, sizeof(sTradeItem));

					if (!Doc()->AddListTradeItem(item))
					{
						result = 0;
						break;
					}
				}
			}

			SetTradeResultCode(result, 0xe4, NULL);

			FrTradeDlg* pDlg;
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"tradedlg");
			if (pWnd == NULL)
			{
				__rtti_obj = NULL;
				pDlg = CreateForm<FrTradeDlg>(g_pFresh->GetManager(), this,
					"tradedlg");
				if (pDlg == NULL)
					break;

				pDlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
				pDlg->SetFadeout(false);
			}
			else
				pDlg = (FrTradeDlg*)pWnd;

			if (pDlg == NULL)
				break;

			if (result == 1)
			{
				pDlg->SetPackageSale(saleType);
				pDlg->OpenTradeShop(Doc()->GetTradeUID());
				pDlg->SetListTradeItem(false);
			}
			else
				pDlg->Close(true);
		}
		break;

	case 0xe5:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();

			if (result == 1)
				Doc()->InitTradeData();

			SetTradeResultCode(result, 0xe5, NULL);
		}
		break;

	case 0xe6:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();
			std::string title;

			if (result == 1)
			{
				packet.DecodeStr(title);
				unsigned long uid = packet.Decode4();

				std::string nick;
				packet.DecodeStr(nick);

				CTask* pTask = IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask())
					? AfxGetTask()
					: AfxGetPreservedTask();
				if (pTask)
					pTask->GetActor("AvatarChat") << MsgObject(NULL, 0xe9,
						(int)nick.c_str(), (int)title.c_str(), 0, 0, 0);

				if (Doc()->GetStateTrade() != STATE_TRADE_INIT)
				{
					if (uid == Doc()->GetTradeUID() || uid == MyUID())
					{
						Doc()->SetTradeTitle(title);

						FrWnd* pTrade =
							g_pFresh->GetDesktop()->FindChildByName("tradedlg");
						if (pTrade == NULL)
							__rtti_obj = NULL;
						else
							((FrTradeDlg*)pTrade)->UpdateTradeTitle();

						FrTradeEditDlg* pEdit =
							(FrTradeEditDlg*)g_pFresh->GetDesktop()
								->FindChildByName("tradeeditdlg");
						if (pEdit == NULL)
							__rtti_obj = NULL;
						else
						{
							pEdit->UpdateTradeTitle();
							pEdit->UpdateTradeTitleEdit();
						}
					}
				}
			}
			else
				Doc()->SetTradeTitle(title);

			SetTradeResultCode(result, 0xe6, NULL);
		}
		break;

	case 0xe7:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();
			int count = packet.Decode4();

			if (result == 1)
				Doc()->SetCountVisitor(count);

			SetTradeResultCode(result, 0xe7, NULL);

			FrTradeDlg* pDlg = (FrTradeDlg*)g_pFresh->GetManager()
								   ->GetDesktop()
								   ->FindChildByName("tradedlg");
			if (pDlg == NULL)
			{
				__rtti_obj = NULL;
				break;
			}

			pDlg->UpdateCountVisitor();
		}
		break;

	case 0xe8:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int result = packet.Decode4();

			__int64 income = packet.Decode8();
			if (result == 1)
				Doc()->SetTradeIncome(income);

			SetTradeResultCode(result, 0xe8, NULL);
		}
		break;

	case 0xe9:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			eTypeSalesTrade saleType = eTypeSalesTrade_None;
			int result = packet.Decode4();

			if (result == 1)
			{
				Doc()->SetStateTrade(STATE_TRADE_OPEN);
				Doc()->ClearListTradeItem();

				saleType = (eTypeSalesTrade)packet.Decode4();

				char nick[22] = { 0 };
				packet.DecodeBuffer(nick, sizeof(nick));
				unsigned long uid = packet.Decode4();
				Doc()->SetTradeUID(uid);

				GetActor("AvatarChat")
					<< MsgObject(NULL, 0xe8, (int)nick, 1, 0, 0, 0);

				sTradeItem item;
				int count = packet.Decode4();

				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&item, sizeof(sTradeItem));

					if (!Doc()->AddListTradeItem(item))
					{
						result = 0;
						break;
					}
				}
			}

			SetTradeResultCode(result, 0xe9, NULL);

			if (result != 1)
				break;

			FrWnd* pEdit =
				g_pFresh->GetManager()->GetDesktop()->FindChildByName(
					"tradeeditdlg");
			if (pEdit == NULL)
				__rtti_obj = NULL;
			else
				pEdit->Close(true);

			FrWnd* pTrade =
				g_pFresh->GetManager()->GetDesktop()->FindChildByName(
					"tradedlg");
			if (pTrade == NULL)
			{
				__rtti_obj = NULL;
				FrTradeDlg* pDlg = CreateForm<FrTradeDlg>(
					g_pFresh->GetManager(), this, "tradedlg");
				if (pDlg == NULL)
					break;

				pDlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
				pDlg->SetFadeout(false);

				pDlg->SetPackageSale(saleType);

				pDlg->OpenTradeShop(MyUID());
			}
		}
		break;

	case 0x1d7:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int error = packet.Decode4();
			if (error)
			{
				GetActor(s_gatewayActor[1])
					<< MsgObject(NULL, 0x273, 0, error, 0, 0, 0);
				break;
			}

			_gateway::sTradeSellParam param;

			param.result = 1;
			packet.Decode8();
			param.pang = packet.Decode8();
			unsigned char count = packet.Decode1();

			param.items.reserve(count);

			for (unsigned char i = 0; i < count; i++)
			{
				sTradeItem item;
				packet.DecodeBuffer(&item, sizeof(sTradeItem));
				param.items.push_back(item);
			}

			GetActor(s_gatewayActor[1])
				<< MsgObject(NULL, 0x273, 2, (int)&param, 0, 0, 0);
		}
		break;

	case 0x1d6:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int error = packet.Decode4();
			if (error)
			{
				GetActor(s_gatewayActor[1])
					<< MsgObject(NULL, 0x273, 0, error, 0, 0, 0);
				break;
			}

			_gateway::sTradeBuyParam param;

			param.result = 1;
			param.pang = packet.Decode8();
			unsigned char count = packet.Decode1();
			param.items.reserve(count);
			for (unsigned char i = 0; i < count; i++)
			{
				_gateway::sTradeBuyBase buy;

				packet.DecodeBuffer(&buy.item, sizeof(sTradeItem));
				buy.countingType = packet.Decode1();
				packet.DecodeBuffer(&buy.itemInfo, sizeof(sItemInfo));

				param.items.push_back(buy);
			}

			GetActor(s_gatewayActor[1])
				<< MsgObject(NULL, 0x273, 1, (int)&param, 0, 0, 0);
		}
		break;

	case 0x1d8:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			int error = packet.Decode4();
			if (error)
			{
				GetActor(s_gatewayActor[1])
					<< MsgObject(NULL, 0x273, 0, error, 0, 0, 0);
				break;
			}

			_gateway::sTradeMinusParam param;

			packet.DecodeStr(param.name);
			param.uid = packet.Decode4();
			unsigned char count = packet.Decode1();

			param.items.reserve(count);
			for (unsigned char i = 0; i < count; i++)
			{
				sTradeItem item;
				packet.DecodeBuffer(&item, sizeof(sTradeItem));
				param.items.push_back(item);
			}

			param.state = packet.Decode4();

			GetActor(s_gatewayActor[1])
				<< MsgObject(NULL, 0x273, 3, (int)&param, 0, 0, 0);
		}
		break;
	case 0xea:
	{
		int result = packet.Decode4();
		sTradeItem item;
		unsigned char type = 0;
		__int64 pang;

		if (result == 1)
		{
			type = packet.Decode1();
			pang = packet.Decode8();

			packet.DecodeBuffer(&item, sizeof(item));

			if (type == 1)
			{
				switch (item.dwTid >> 26)
				{
				case 6:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_myItemList.begin();
						it != Doc()->m_myItemList.end(); it++)
					{
						if ((*it).guid == item.dwGuid)
						{
							if ((*it).tid == item.dwTid)
							{
								if ((*it).Common[0] > item.iNum)
									(*it).Common[0] -= item.iNum;
								else if ((*it).Common[0] == item.iNum)
									Doc()->m_myItemList.erase(it);
							}
							break;
						}
					}
				}
				break;

				case 5:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_ballList.begin();
						it != Doc()->m_ballList.end(); it++)
					{
						if ((*it).guid == item.dwGuid)
						{
							if ((*it).tid == item.dwTid)
							{
								if ((*it).Common[0] > item.iNum)
									(*it).Common[0] -= item.iNum;
								else if ((*it).Common[0] == item.iNum)
									Doc()->m_ballList.erase(it);
							}
							break;
						}
					}
				}
				break;

				case 2:
				{
					ITEM_LIST_IT it;
					if (IsLocalContent(S4_UCC) && IsUccClothes(item.dwTid))
					{
						for (it = Doc()->m_uccItemList.begin();
							it != Doc()->m_uccItemList.end(); it++)
						{
							if ((*it).guid == item.dwGuid &&
								(*it).tid == item.dwTid &&
								strcmpi((*it).UccIndex, item.UccIndex) == 0 &&
								(*it).Seq == item.Seq)
							{
								Doc()->m_uccItemList.erase(it);
								if (IsLocalContent(S4_CARD_SYSTEM_CHANGE))
									Doc()->DeleteCardToPartsAttach(item.dwGuid);
								break;
							}
						}
					}
					else
					{
						for (it = Doc()->m_partsList.begin();
							it != Doc()->m_partsList.end(); it++)
						{
							if ((*it).guid == item.dwGuid)
							{
								if ((*it).tid == item.dwTid)
								{
									Doc()->m_partsList.erase(it);
									if (IsLocalContent(S4_CARD_SYSTEM_CHANGE))
										Doc()->DeleteCardToPartsAttach(
											item.dwGuid);
								}
								break;
							}
						}
					}
				}
				break;

				case 4:
				{
					CLUBSET_MAP_IT it;
					for (it = Doc()->m_clubSetMap.begin();
						it != Doc()->m_clubSetMap.end(); it++)
					{
						if ((*it).second.guid == item.dwGuid)
						{
							if ((*it).second.tid == item.dwTid)
								Doc()->m_clubSetMap.erase(it);
							break;
						}
					}
				}
				break;

				case 0x1c:
				{
					ITEM_LIST_IT it;
					for (it = Doc()->m_auxPartList.begin();
						it != Doc()->m_auxPartList.end(); it++)
					{
						if ((*it).guid == item.dwGuid)
						{
							if ((*it).tid == item.dwTid)
							{
								if ((*it).Common[0] > item.iNum)
									(*it).Common[0] -= item.iNum;
								else if ((*it).Common[0] == item.iNum)
									Doc()->m_auxPartList.erase(it);
							}
							break;
						}
					}
				}
				break;

				case 7:
				{
					CADDIE_MAP_IT it = Doc()->m_caddieMap.find(item.dwGuid);
					if (it != Doc()->m_caddieMap.end())
						Doc()->m_caddieMap.erase(it);
				}
				break;

				case 0x1f:
				{
					std::list<sCards>::iterator it;
					for (it = Doc()->m_cardList.begin();
						it != Doc()->m_cardList.end(); it++)
					{
						if ((*it).tid == item.dwTid)
						{
							(*it).cardCount -= item.iNum;
							if ((*it).cardCount < 1)
								Doc()->m_cardList.erase(it);
							break;
						}
					}
				}
				break;
				}

				Doc()->m_myInfo.stat.i64Pang += pang;
				Doc()->AddTradeIncome(pang);
			}
			else if (type == 0)
			{
				unsigned char kind = packet.Decode1();
				sItemInfo itemInfo;
				sCaddieInfo caddieInfo = { 0 };
				sCards card = { 0 };

				switch (kind)
				{
				case 1:
					packet.DecodeBuffer(&itemInfo, sizeof(itemInfo));

					switch (itemInfo.tid >> 26)
					{
					case 5:
					{
						ITEM_LIST_IT it;
						for (it = Doc()->m_ballList.begin();
							it != Doc()->m_ballList.end(); it++)
						{
							if ((*it).guid == itemInfo.guid)
								break;
						}

						if (it != Doc()->m_ballList.end())
							(*it).Common[0] += itemInfo.Common[0];
						else
							Doc()->m_ballList.push_back(itemInfo);
					}
					break;

					case 6:
					{
						ITEM_LIST_IT it;
						for (it = Doc()->m_myItemList.begin();
							it != Doc()->m_myItemList.end(); it++)
						{
							if ((*it).guid == itemInfo.guid)
								break;
						}

						if (it != Doc()->m_myItemList.end())
							(*it).Common[0] += itemInfo.Common[0];
						else
							Doc()->m_myItemList.push_back(itemInfo);
					}
					break;

					case 0x1c:
					{
						ITEM_LIST_IT it;
						for (it = Doc()->m_auxPartList.begin();
							it != Doc()->m_auxPartList.end(); it++)
						{
							if ((*it).guid == itemInfo.guid)
								break;
						}

						if (it != Doc()->m_auxPartList.end())
							(*it).Common[0] += itemInfo.Common[0];
						else
							Doc()->m_auxPartList.push_back(itemInfo);
					}
					break;
					}
					break;

				case 2:
				case 3:
					packet.DecodeBuffer(&itemInfo, sizeof(itemInfo));

					switch (itemInfo.tid >> 26)
					{
					case 2:
						if (IsLocalContent(S4_UCC) &&
							IsUccClothes(itemInfo.tid))
						{
							Doc()->m_uccItemList.push_back(itemInfo);
							UccManager()->AddClothes(1, itemInfo.guid,
								itemInfo.tid, itemInfo.UccIndex, false, false);
						}
						else
						{
							Doc()->AddMyPartsList(itemInfo);
						}
						break;

					case 4:
						Doc()->m_clubSetMap.insert(
							std::map<unsigned int, sItemInfo>::value_type(
								itemInfo.guid, itemInfo));
						break;
					case 5:
						Doc()->m_ballList.push_back(itemInfo);
						break;
					case 6:
						Doc()->m_myItemList.push_back(itemInfo);
						break;
					case 0x1c:
						Doc()->m_auxPartList.push_back(itemInfo);
						break;
					}
					break;

				case 4:
					packet.DecodeBuffer(&caddieInfo, sizeof(caddieInfo));
					Doc()->AddCaddie(&caddieInfo);
					break;

				case 5:
					if (IsLocalContent(S4_CARD_SYSTEM))
					{
						packet.DecodeBuffer(&card, sizeof(card));

						bool bFound = false;
						std::list<sCards>::iterator it =
							Doc()->m_cardList.begin();
						for (; it != Doc()->m_cardList.end(); it++)
						{
							if ((*it).tid == item.dwTid)
							{
								(*it).cardCount += card.cardCount;
								bFound = true;
								break;
							}
						}

						if (!bFound)
							Doc()->m_cardList.push_back(card);
					}
					break;
				}

				Doc()->m_myInfo.stat.i64Pang = pang;
			}
		}

		FrTradeDlg* pDlg =
			(FrTradeDlg*)g_pFresh->GetDesktop()->FindChildByName("tradedlg");
		if (pDlg == NULL)
			__rtti_obj = NULL;
		else
			pDlg->EnableSendingServer(false);

		if (type == 1)
		{
			IFF_ITEM_COMMON* pCommon =
				ItemManager()->FindCommonItem(item.dwTid);
			if (pCommon)
			{
				std::string msg;
				if (item.iNum > 1)
				{
					msg = MakeStr(
						"%s\xc0\xcc(\xb0\xa1) %d\xb0\xb3 \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
						pCommon->Name, item.iNum);
				}
				else
				{
					sItemInfo* pMyItem = GetMyItemInfo(item.dwGuid);
					sUccClothes* pClothes =
						UccManager()->FindClothes(item.dwGuid);
					if (IsLocalContent(S4_UCC) && pMyItem &&
						(pMyItem->status & 1) && pClothes &&
						IsUccClothes(item.dwTid))
						msg = MakeStr(
							"%s\xc0\xcc(\xb0\xa1) \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
							pClothes->uccName);
					else
						msg = MakeStr(
							"%s\xc0\xcc(\xb0\xa1) \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
							pCommon->Name);
				}

				CChatMsg::Instance()->AddChatMsg(msg, 0xffff, true, false);
			}

			SetTradeResultCode(result, 0xe1, NULL);
		}
		else
		{
			SetTradeResultCode(result, 0xea, &item);
		}
	}
	break;
	case 0xeb:
	{
		std::string nick;
		sTradeItem item;

		packet.DecodeStr(nick);
		unsigned long uid = packet.Decode4();

		packet.DecodeBuffer(&item, sizeof(item));

		if (uid == Doc()->GetTradeUID())
		{
			eStateTrade state = (eStateTrade)packet.Decode4();

			if (state == STATE_TRADE_OUTOFSTOCK)
				Doc()->SetStateTrade(state);

			FrTradeBuyDlg* pBuyDlg =
				(FrTradeBuyDlg*)g_pFresh->GetDesktop()->FindChildByName(
					"tradebuydlg");
			if (pBuyDlg == NULL)
			{
				__rtti_obj = NULL;
			}
			else if (!pBuyDlg->IsClosing())
			{
				sTradeItem* pItem = pBuyDlg->GetBuyItem();

				if (pItem != NULL && pItem->iIndex == item.iIndex &&
					pItem->dwGuid == item.dwGuid)
				{
					if (Doc()->GetSizeTradeItem() > 1)
					{
						if (pItem->iNum == item.iNum)
							CChatMsg::Instance()->AddChatMsg(
								"\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xbb\xf3\xc7\xb0\xc0\xba \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
								0xffff, true, false);
						else
						{
							CChatMsg::Instance()->AddChatMsg(
								"\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xbb\xf3\xc7\xb0\xc0\xc7 \xbc\xf6\xb7\xae\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
								0xffff, true, false);
						}
					}

					pBuyDlg->Close(true);

					FrWnd* pNotify = g_pFresh->GetDesktop()->FindChildByName(
						"notify_okcancel");
					if (pNotify == NULL)
						__rtti_obj = NULL;
					else
						pNotify->Close(true);
				}
			}

			FrTradeDlg* pTradeDlg = NULL;
			FrWnd* pWnd = g_pFresh->GetDesktop()->FindChildByName("tradedlg");
			if (pWnd == NULL)
			{
				__rtti_obj = NULL;
			}
			else
			{
				pTradeDlg = (FrTradeDlg*)pWnd;
				pTradeDlg->SetListTradeItem(true);
			}

			Doc()->MinusListTradeItem(item);

			if (pTradeDlg)
				pTradeDlg->SetListTradeItem(false);

			if (Doc()->GetStateTrade() == STATE_TRADE_OUTOFSTOCK)
			{
				if (strcmp(nick.c_str(), MyNick()) != 0)
				{
					if (pTradeDlg)
						pTradeDlg->Close(true);
				}
				else if (pTradeDlg)
				{
					pTradeDlg->ShowStaticOutOfStock(true);
					pTradeDlg->EnableHideBn(false);
				}
				else
				{
					FrTradeDlg* pDlg = CreateForm<FrTradeDlg>(
						g_pFresh->GetManager(), this, "tradedlg");
					if (pDlg)
					{
						pDlg->Open(NULL, 0xc0);
						pDlg->OpenTradeShop(Doc()->GetTradeUID());
						pDlg->SetFadeout(false);
					}
				}

				CChatMsg::Instance()->AddChatMsg(
					"\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xffff, true, false);
			}
		}

		if (Doc()->GetStateTrade() == STATE_TRADE_OUTOFSTOCK)
			GetActor("AvatarChat")
				<< MsgObject(NULL, 0xe8, (int)nick.c_str(), 3, 0, 0, 0);
	}
	break;

	case 0xec:
	{
		unsigned char result = packet.Decode1();

		SetTradeResultCode(15, result, NULL);

		if (result)
			CChatMsg::Instance()->AddChatMsg(
				"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xff37, true, false);
		if (!result)
			CChatMsg::Instance()->AddChatMsg(
				"\xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xce \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xff37, true, false);
	}
	break;

	case 0xed:
	{
		std::string msg =
			"\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";

		sRecycleItem item;
		memset(&item, 0, sizeof(item));

		std::list<sRecycleItem> items;
		items.clear();

		short index = 0;
		int result = packet.Decode4();
		switch (result)
		{
		case 0:
		{
			index = packet.Decode2();

			unsigned char count = packet.Decode1();
			while (count--)
			{
				packet.DecodeBuffer(&item, sizeof(item));
				items.push_back(item);
			}
		}
		break;

		case 2:
			msg =
				"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.";
			break;

		case 3:
			msg =
				"\xbe\xc6\xc0\xcc\xc5\xdb \xba\xb8\xc0\xaf \xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xc1\xb6\xc7\xd5\xc0\xbb \xc7\xd2\xbc\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;

		case 4:
			msg =
				"\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.";
			break;

		default:
			msg =
				"\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		}

		FrRecycleDlg* pDlg =
			(FrRecycleDlg*)g_pFresh->GetDesktop()->FindChildByName(
				"recycledlg");
		if (pDlg == NULL)
		{
			__rtti_obj = NULL;
		}
		else if (result == 0)
		{
			pDlg->SetResultRecycleItem(true, items, index);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0));
		}
		else
		{
			pDlg->SetResultRecycleItem(false, items, index);
			CTask* pTask = AfxGetTask();
			const char* text = msg.c_str();
			pTask->SendMsgToMainActor(
				MsgObject(NULL, 0x23, (int)text, 0, 0, 0, 0));
		}
	}
	break;

	case 0x69:
		switch (packet.Decode1())
		{
		case 4:
			switch (packet.Decode1())
			{
			case 0:
			{
				sCharacterInfo charInfo;
				packet.DecodeBuffer(&charInfo, sizeof(charInfo));
				Doc()->m_charMap[charInfo.guid] = charInfo;
				Doc()->BuildMyPartTidList(charInfo);

				CTask* pTask = AfxGetTask();
				if (IS_KINDOF(CRealMyRoomTask, pTask))
					pTask->SendMsgToMainActor(
						MsgObject(NULL, 0x26c, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x49,
					Doc()->m_myInfo.info.dwGuid, (int)&charInfo, 0, 0, 0));
			}
			break;

			case 1:
			case 3:
				break;

			case 2:
			{
				unsigned long tidItemSlot[10] = { 0 };
				packet.DecodeBuffer(tidItemSlot, sizeof(tidItemSlot));
				memcpy(Doc()->m_myInfo.userEquip.tidItemSlot, tidItemSlot,
					sizeof(tidItemSlot));
			}
			break;

			case 5:
			{
				Doc()->m_myInfo.userEquip.guidChar = packet.Decode4();

				CSharedDoc* pDoc = Doc();
				std::map<unsigned int, sCharacterInfo>::iterator it =
					pDoc->m_charMap.find(pDoc->m_myInfo.userEquip.guidChar);
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x49,
					pDoc->m_myInfo.info.dwGuid, (int)&(*it).second, 0, 0, 0));
			}
			break;

			case 4:
			{
				packet.DecodeBuffer(Doc()->m_myInfo.userEquip.tidSkin,
					sizeof(Doc()->m_myInfo.userEquip.tidSkin));

				std::map<unsigned long, sUserInfoTime>::iterator it;
				if (Doc()->FindUserInfoTimeGUID(MyGuid(false), it))
					memcpy((*it).second.info.userEquip.tidSkin,
						Doc()->m_myInfo.userEquip.tidSkin,
						sizeof(Doc()->m_myInfo.userEquip.tidSkin));
			}
			break;

			case 8:
			{
				sMascotInfo mascotInfo;
				packet.DecodeBuffer(&mascotInfo, sizeof(mascotInfo));

				std::map<unsigned int, sMascotInfo>::iterator it =
					Doc()->m_mascotMap.find(mascotInfo.guid);
				if (it != Doc()->m_mascotMap.end())
					Doc()->m_mascotMap[mascotInfo.guid] = mascotInfo;
			}
			break;

			default:
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9 \xc4\xda\xb5\xe5",
					0, 0, 0, 0));
				break;
			}
			break;

		case 1:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"DB \xbf\xa1\xb7\xaf\xb7\xce \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0));
			break;

		case 5:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xe5\xc2\xf8\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0));
			break;

		case 0:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0));
			break;

		default:
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe1\xb0\xfa \xc4\xda\xb5\xe5",
				0, 0, 0, 0));
			break;
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
		break;

	case 0x15f:
	{
		int count = packet.Decode4();
		char text[1024] = "\0";
		if (count > 0)
		{
			wsprintf(text,
				"%d\xb0\xb3\xc0\xc7 \xc6\xed\xc1\xf6\xb0\xa1 \xba\xb8\xb0\xfc \xb1\xe2\xb0\xa3\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.\n\xb7\xce\xb1\xd7\xbe\xc6\xbf\xf4 \xc8\xa4\xc0\xba \xbc\xad\xb9\xf6 \xc0\xcc\xb5\xbf\xc0\xbb \xc7\xcf\xb1\xe2 \xc0\xfc\xbf\xa1 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xad \xb8\xde\xc0\xcf\xbf\xa1 \xc6\xf7\xc7\xd4\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb\n\xbf\xc5\xb0\xdc\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.\n",
				count);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x23, (int)text, 0, 0, 0, 0));
		}
	}
	break;

	case 0xef:
	{
		CProjectG::Instance()->m_bMsnOffline = true;

		Doc()->m_guildMemberList.clear();
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x68, 0, 0, 0, 0, 0));

		if (IS_KINDOF(CShopTask, AfxGetTask()) == true)
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0xc5, 0, 0, 0, 0, 0));

		char text[1024] = "\0";
		int count = 0;
		if (count > 0)
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x23, (int)text, 0, 0, 0, 0));

		if (!IsLocalContent(S4_GOWITH_FRIEND) &&
			!IsLocalContent((localContentType_t)0x5a))
			break;

		unsigned char type = 0xff;
		unsigned short room = 0xffff;
		unsigned long uid = 0xffffffff;
		if (Doc()->GetDirectMoveRoomInfo(&type, &room, &uid) == true)
		{
			if (room == 0xffff && IsLocalContent((localContentType_t)0x5a))
			{
				CSharedDoc* pDoc = Doc();
				std::list<sChannelInfo>::iterator it =
					std::find(pDoc->m_channelList.begin(),
						pDoc->m_channelList.end(), type);
				sChannelInfo channel = *it;

				pDoc->m_selChannel = channel;

				WSendPacket send((enumClientPacket)4);
				send.Encode1(channel.Uid);
				send.Send(TO_GAME);

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 1, 0, 0, 0, 0, 0));

				if (NET()->IsConnected(WNetworkSystem::NET_MSN) &&
					CMessengerInfo::Instance()->GetComplete() == 4)
				{
					sChannelInfo& selected = Doc()->m_selChannel;

					Doc()->m_roomInfo.roomGuid = 0xffff;
					Doc()->m_roomInfo.gameType = 0xff;

					sUserPosition position;
					position.iServerGUID = Doc()->m_gameServerUID;
					position.iChannelUid = selected.Uid;
					wsprintf(position.csChannelName, selected.Name);
					position.iRoomIdx = 0xffff;
					position.iRoomType = -1;

					WSendPacket send(0x23);
					send.EncodeBuffer(&position, sizeof(position));
					send.Send(TO_MSN);
				}
			}
			else
			{
				WSendPacket send((enumClientPacket)0xac);
				send.Encode1(type);
				send.Encode2(room);
				send.Send(TO_GAME);
			}
		}

		Doc()->ClearDirectMoveRoomInfo();
		Doc()->SetGoWithModeEnd();
	}
	break;

	case 0xf6:
		switch (packet.Decode1())
		{
		case 0:
		{
			short minutes = packet.Decode2();
			CNoticeBoard::Instance()->AddNotice(
				MakeStr(
					"[\xb0\xf8\xc1\xf6] \xc0\xcc \xbc\xad\xb9\xf6\xb4\xc2 %d\xba\xd0\xc8\xc4\xbf\xa1 \xc0\xda\xb5\xbf \xc1\xbe\xb7\xe1\xb5\xc9 \xbf\xb9\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.",
					minutes),
				1, false);
		}
		break;

		case 1:
		{
			short seconds = packet.Decode2();
			CChatMsg::Instance()->AddChatMsg(
				MakeStr(
					"\xbe\xcb\xb8\xb2 : \xbc\xad\xb9\xf6\xb0\xa1 %d\xc3\xca\xc8\xc4\xbf\xa1 \xc0\xda\xb5\xbf \xc1\xbe\xb7\xe1\xb5\xcb\xb4\xcf\xb4\xd9.",
					seconds),
				0xffff7878, false, false);
		}
		break;
		}
		break;

	case 0x1f5:
	{
		if (RANKINGDLG() == NULL)
			break;

		unsigned char result = packet.Decode1();
		unsigned char rankType = packet.Decode1();
		unsigned char genSubType = packet.Decode1();
		unsigned char courseType = packet.Decode1();
		unsigned char recordType = packet.Decode1();
		int page = packet.Decode4();
		int totalPage = packet.Decode4();
		FrRankingDlg::SetRankTypes(rankType, genSubType, courseType, recordType,
			page, totalPage);

		if (result != 0)
		{
			RANKINGDLG()->SetBottomMsg(
				"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb7\xa9\xc5\xb7 \xb1\xe2\xb7\xcf\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");

			if (result == 2)
				break;

			RANKINGDLG()->m_rankInfoList.clear();
			RANKINGDLG()->m_myRankInfo.uid = 0xffffffff;
			RANKINGDLG()->ClearRankList();
			RANKINGDLG()->UpdateMyInfo();
			break;
		}

		RANKINGDLG()->m_rankInfoList.clear();

		unsigned short count = packet.Decode2();
		for (unsigned short i = 0; i < count; i++)
		{
			ranktype::sListInfo info;
			info.uid = packet.Decode4();
			info.rank = packet.Decode4();
			info.preRank = packet.Decode4();
			info.value = packet.Decode4();
			info.level = packet.Decode2();
			info.unknown = packet.Decode1();
			info.id = packet.DecodeStr();
			info.nickname = packet.DecodeStr();

			RANKINGDLG()->m_rankInfoList.push_back(info);
		}

		FrRankingDlg* pDlg = RANKINGDLG();

		unsigned char myRank = packet.Decode1();
		if (myRank == 0)
		{
			pDlg->m_myRankInfo.uid = packet.Decode4();
			pDlg->m_myRankInfo.rank = packet.Decode4();
			pDlg->m_myRankInfo.preRank = packet.Decode4();
			pDlg->m_myRankInfo.value = packet.Decode4();
			pDlg->m_myRankInfo.level = packet.Decode2();
			pDlg->m_myRankInfo.unknown = packet.Decode1();
			pDlg->m_myRankInfo.id = packet.DecodeStr();
			pDlg->m_myRankInfo.nickname = packet.DecodeStr();
		}
		else if (myRank == 1)
		{
			pDlg->m_myRankInfo.uid = 0xffffffff;
		}

		RANKINGDLG()->UpdateRankList();
		RANKINGDLG()->UpdateMyInfo();
	}
	break;

	case 0x1f6:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (RANKINGDLG() == NULL)
				break;

			unsigned char result = packet.Decode1();
			if (result == 2)
			{
				CRankingInfo::Instance()->OnNotify(
					"\xc0\xaf\xc0\xfa \xc1\xa4\xba\xb8\xb8\xa6 \xb0\xa1\xc1\xae\xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					2);
			}
			else if (result == 1)
			{
				RANKINGDLG()->SetBottomMsg(
					"\xbc\xf8\xc0\xa7\xb3\xbb\xbf\xa1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			}
			else
			{
				packet.DecodeBuffer(&RANKINGDLG()->m_userInfo,
					sizeof(ranktype::sRankUserInfo));

				if (IsLocalContent((localContentType_t)0x57))
					RequestUccInfo(RANKINGDLG()->m_userInfo);

				RANKINGDLG()->UpdateUserInfo();
			}
		}
		break;

	case 0x1f7:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		CRankingInfo::Instance()->OnNotify(
			"\xbc\xad\xb9\xf6\xbf\xa1\xbc\xad \xb7\xa9\xc5\xb7 \xc1\xa4\xba\xb8\xb8\xa6 \xb4\xd9\xbd\xc3 \xc1\xa4\xb7\xc4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			2);
		break;

	case 0x1f8:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			FrRankingDlg* pDlg = RANKINGDLG();
			if (pDlg == NULL)
				break;

			if (packet.Decode1())
			{
				pDlg->m_myRankInfo.uid = 0xffffffff;
				CRankingInfo::Instance()->OnNotify(
					"\xbc\xf8\xc0\xa7\xb3\xbb\xbf\xa1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					2);
				break;
			}

			unsigned char rankType = packet.Decode1();
			unsigned char subType = packet.Decode1();
			unsigned char termType = packet.Decode1();
			unsigned char classType = packet.Decode1();
			int pageNum = packet.Decode4();
			int pageTotal = packet.Decode4();
			FrRankingDlg::SetRankTypes(rankType, subType, termType, classType,
				pageNum, pageTotal);

			RANKINGDLG()->m_rankInfoList.clear();
			unsigned short count = packet.Decode2();
			for (unsigned short i = 0; i < count; i++)
			{
				ranktype::sListInfo info;
				info.uid = packet.Decode4();
				info.rank = packet.Decode4();
				info.preRank = packet.Decode4();
				info.value = packet.Decode4();
				info.level = packet.Decode2();
				info.unknown = packet.Decode1();
				info.id = packet.DecodeStr();
				info.nickname = packet.DecodeStr();

				RANKINGDLG()->m_rankInfoList.push_back(info);
			}
			unsigned short select = packet.Decode2();

			RANKINGDLG()->UpdateRankList();
			RANKINGDLG()->SelectRankList(select);
		}
		break;

	case 0xfc:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long result = packet.Decode4();

			if (g_pPointEventDlg)
				g_pPointEventDlg->ResultMessage(result);
		}
		break;

	case 0xfb:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long point = packet.Decode4();
			Doc()->m_myInfo.info.dwPointPointEvent = point;
		}
		break;

	case 0xfd:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long count = packet.Decode4();

			unsigned long
				totalCount[CCommodityStatusManager::MAX_COMMODITY_COUNT] = {
					0
				};

			packet.DecodeBuffer(totalCount, count * sizeof(unsigned long));

			for (unsigned long i = 0; i < count; i++)
				g_CommodityStatusManager.UpdateTotalCount((unsigned short)i,
					totalCount[i]);
		}
		break;

	case 0xfe:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long count = packet.Decode4();

			CCommodityStatusManager::sCommodityStatus
				status[CCommodityStatusManager::MAX_COMMODITY_COUNT] = { 0 };

			packet.DecodeBuffer(status,
				count * sizeof(CCommodityStatusManager::sCommodityStatus));

			for (unsigned long i = 0; i < count; i++)
			{
				g_CommodityStatusManager.UpdateStatus((unsigned short)i,
					status[i].status);
				g_CommodityStatusManager.UpdateMyCount((unsigned short)i,
					status[i].myCount);
			}
		}
		break;

	case 0x10b:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			unsigned char bSuccess = packet.Decode1();

			CGiftBoxEvent* pEvent = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x1d,
				pEvent);

			if (bSuccess == 0)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbc\xb1\xb9\xb0\xbb\xf3\xc0\xda \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}

			if (pEvent->GetOpenedBoxTID())
			{
				_RemoveWareHouseItem(pEvent->GetOpenedBoxTID(),
					AfxGetTask()->GetActor("RealMyRoom"), 1);

				sGiftBoxResult result = { 0 };
				packet.DecodeBuffer(&result, sizeof(result));

				std::string msg;
				switch (result.type)
				{
				case 5:
					_ReFreshMyRoomGift(false, "@HanBit");
					if (pEvent->GetOpenedBoxTID() == 0x1a00003e)
					{
						msg =
							"\xc6\xce\xbe\xdf 3\xc1\xd6\xb3\xe2 \xb1\xe2\xb3\xe4 \xbe\xc6\xc1\xee\xc5\xd8 10\xb0\xb3\xb8\xa6 \xbc\xb1\xb9\xb0\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					}
					break;

				case 6:
					_ReFreshMyRoomGift(false, "@HanBit");
					msg =
						"\xb8\xb6\xbd\xba\xc4\xda\xc6\xae(1\xc0\xcf)\xb4\xc2 \xbc\xb1\xb9\xb0\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					break;

				case 4:
					msg =
						"\xc4\xed\xc6\xf9\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xc4\xed\xc6\xf9\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					break;

				case 3:
				{
					char buf[64] = "";
					sprintf(buf,
						"%d \xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. ",
						result.value);
					msg = buf;
					Doc()->m_myInfo.stat.i64Pang = result.totalPang;
				}
				break;

				default:
					msg =
						"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.";
					break;
				}

				pEvent->SetOpenedBoxTID(0);

				CTask* pTask = AfxGetTask();
				const char* text = msg.c_str();
				pTask->SendMsgToMainActor(
					MsgObject(NULL, 0x26, (int)text, result.value, 0, 0, 0));
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbc\xb1\xb9\xb0\xbb\xf3\xc0\xda \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
		}
		break;

	case 0x107:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

			unsigned char bSuccess = packet.Decode1();

			if (bSuccess == 0)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xbc\xbc\xb9\xe8\xb5\xb7\xba\xc0\xc5\xf5 \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}

			_RemoveWareHouseItem(0x1a00003b,
				AfxGetTask()->GetActor("RealMyRoom"), 1);

			sLuckyPouchResult result = { 0 };
			packet.DecodeBuffer(&result, sizeof(result));

			IFF_ITEM_COMMON* pItem =
				ItemManager()->FindCommonItem(result.value);

			if (pItem)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x26,
					(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
					result.value, 0, 0, 0));
				break;
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x26,
				(int)"\xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
				result.value, 0, 0, 0));

			if (result.value)
				Doc()->m_myInfo.stat.i64Pang = result.totalPang;
		}
		break;

	case 0xf7:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			sGameServerInfo server;
			packet.DecodeBuffer(&server, sizeof(server));
			Doc()->m_curGameServer = server;
		}
		break;

	case 0xce:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char mode = packet.Decode1();

			int total = packet.Decode2();
			int count = packet.Decode2();

			for (int i = 0; i < count; i++)
			{
				Doc()->m_refreshGuidList.push_back(packet.Decode4());

				if (mode == 2 && Doc())
					Doc()->m_refreshCountList.push_back(packet.Decode2());
			}

			if (total == count)
			{
				Doc()->RefreshItemListFromGuidList(mode);

				if (mode == 2 && IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
				{
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(NULL, 0x67, 0, 0, 0, 0, 0);
				}
			}
		}
		break;

	case 0xc3:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned char bSuccess = packet.Decode1();
			unsigned long typeId = packet.Decode4();
			unsigned long count = packet.Decode4();

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
			if (bSuccess == 0)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)MakeStr(
						"%s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe8\xc1\xa6\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9..",
						pItem ? pItem->Name : NULL),
					0, 0, 0, 0));
				break;
			}

			switch (typeId >> 26)
			{
			case 6:
			{
				packet.Decode4();

				std::list<sItemInfo>& itemList = Doc()->m_myItemList;
				ITEM_LIST_IT it;
				for (it = itemList.begin(); it != itemList.end(); ++it)
				{
					if ((*it).tid == typeId)
					{
						(*it).Common[0] -= (short)count;
						if ((*it).Common[0] < 1)
							it = Doc()->m_myItemList.erase(it);
						break;
					}
				}

				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)MakeStr(
						"%d\xb0\xb3\xc0\xc7 %s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbc\xd2\xb8\xf0\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
						count, pItem ? pItem->Name : NULL),
					0, 0, 0, 0));
			}
			break;

			case 14:
			{
				std::list<sItemInfo>& itemList = Doc()->m_cutinList;
				ITEM_LIST_IT it;
				for (it = itemList.begin(); it != itemList.end(); ++it)
				{
					if ((*it).tid == typeId)
					{
						itemList.erase(it);
						break;
					}
				}

				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)MakeStr(
						"%s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe8\xc1\xa6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
						pItem ? pItem->Name : NULL),
					0, 0, 0, 0));
			}
			break;
			}

			if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
			{
				GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xdd, typeId, count, 0, 0, 0);
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
		}
		break;

	case 0x15e:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			Doc()->m_mailList.clear();

			static sMailInfoBrief mail;

			unsigned long count = packet.Decode4();
			for (unsigned long i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&mail, sizeof(mail));
				Doc()->m_mailList.push_back(mail);
			}

			if (Doc()->m_mailList.size() != 0)
			{
				Doc()->m_bRefreshCamera = true;
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));
			}
		}
		break;

	case 0x108:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (Doc()->m_cardPackMode == 1)
			{
				Doc()->m_giftList.clear();
				Doc()->m_cardPackMode = 1;

				WSendPacket send((enumClientPacket)0x92);
				send.Encode4(Doc()->m_cardPackMode);
				send.Send(TO_GAME);
			}

			unsigned short count = packet.Decode2();
			if (count > 0)
			{
				Doc()->m_sendGiftList.clear();

				for (unsigned short i = 0; i < count; i++)
				{
					sGiftInfo gift;
					packet.DecodeBuffer(&gift, sizeof(gift));

					Doc()->m_chatManager.FilteringHack(gift.sMsg, false);

					IFF_ITEM_COMMON* pItem =
						ItemManager()->FindCommonItem(gift.tid);
					if (pItem && pItem->Final == 1)
						Doc()->m_sendGiftList.push_back(gift);
				}

				if (!Doc()->m_sendGiftList.empty())
				{
					Doc()->m_bRefreshCamera = true;
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));
				}
			}
		}
		break;

	case 0x109:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long usableBonus = packet.Decode4();
			unsigned long remainedBonus = packet.Decode4();
			packet.Decode4();

			Doc()->SetBsUsableBonusTimes((short)usableBonus);
			Doc()->SetBsRemainedBonusTimes((short)remainedBonus);

			switch (Doc()->m_pendingMenu)
			{
			case 1:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x200, 0, 0, 0, 0, 0));
				Doc()->m_pendingMenu = 0;
				break;

			case 2:
				if (IS_KINDOF(CLobbyTask, AfxGetTask()))
				{
					AfxGetTask()->GetActor("Lobby")
						<< MsgObject(NULL, 0x201, 0, 0, 0, 0, 0);
				}
				Doc()->m_pendingMenu = 0;
				break;
			}
		}
		break;

	case 0x10e:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			switch (packet.Decode4())
			{
			case 0:
			{
				if (IsAdministrator(0xffffffff))
					break;

				WSendPacket send(0x21);
				send.EncodeStr(std::string(MyNick()));
				send.Encode4(MyUID());
				send.Send(TO_MSN);
			}
			break;

			case 2:
			{
				if (IsAdministrator(0xffffffff) == 1)
					break;

				int count = packet.Decode4();

				std::list<sPrize*> prizeList;

				for (int i = 0; i < count; i++)
				{
					sPrize* pPrize = new sPrize;
					memset(pPrize, 0, sizeof(sPrize));

					packet.DecodeBuffer(pPrize, sizeof(sPrize));
					prizeList.push_back(pPrize);
				}

				char* text = new char[0x200];
				memset(text, 0, 0x200);

				int index = 0;
				std::list<sPrize*>::iterator it;
				for (it = prizeList.begin(); it != prizeList.end(); ++it)
				{
					sPrize* pPrize = *it;

					if (pPrize == NULL)
						continue;

					char name[64] = "";
					char line[256] = "";

					index++;
					IFF_ITEM_COMMON* pItem =
						ItemManager()->FindCommonItem(pPrize->typeId);
					strcpy(name, pItem->Name);

					if (pPrize->type == 0)
					{
						if (pPrize->rankFrom == pPrize->rankTo)
							sprintf(line, "%2d. %d\xb5\xee %s %d\xb0\xb3\n",
								index, pPrize->rankFrom, name, pPrize->count);
						else
							sprintf(line,
								"%2d. %d\xb5\xee\xba\xce\xc5\xcd %d\xb5\xee %s %d\xb0\xb3\n",
								index, pPrize->rankFrom, pPrize->rankTo, name,
								pPrize->count);
					}
					else
					{
						char award[32] = "";

						switch (pPrize->type)
						{
						case 1:
						{
							const char* text =
								"\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5";
							strcpy(award, text);
						}
						break;
						case 2:
						{
							const char* text =
								"\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca";
							strcpy(award, text);
						}
						break;
						case 5:
						{
							const char* text =
								"\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae";
							strcpy(award, text);
						}
						break;
						case 6:
						{
							const char* text = "\xc7\xe0\xbf\xee\xbb\xf3";
							strcpy(award, text);
						}
						break;
						case 4:
						{
							const char* text =
								"\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd";
							strcpy(award, text);
						}
						break;
						case 3:
						{
							const char* text =
								"\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce";
							strcpy(award, text);
						}
						break;
						}

						sprintf(line, "%2d. %s %s %d\xb0\xb3\n", index, award,
							name, pPrize->count);
					}

					strcat(text, line);
				}

				FrForm* pForm =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				pForm->SetTimeLimit(30.0f);

				pForm->SetMessage(text, false);

				pForm->Open(NULL, 0);

				for (it = prizeList.begin(); it != prizeList.end(); ++it)
				{
					if (*it)
					{
						delete *it;
						*it = NULL;
					}
				}

				prizeList.clear();
			}
			break;
			}
		}
		break;

	case 0x10c:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			sUserMatchHistory history[5];
			packet.DecodeBuffer(history, sizeof(history));
			CMatchHistory::Instance()->SetMatchHistory(history);
		}
		break;

	case 0x10f:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long flag = packet.Decode4();

			Doc()->SetFlagBlock(flag);

			if (Doc()->GetFlagBlock() & 0x10)
			{
				FrForm* pForm =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				pForm->SetTimeLimit(12.0f);
				pForm->SetMessage(
					"\xc7\xf6\xc0\xe7 \xc0\xfc\xb1\xa4\xc6\xc7 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xfc\xb1\xa4\xc6\xc7\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(NULL, 0);
			}
		}
		break;

	case 0x10d:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			sLevelUpDone levelUp;
			packet.DecodeBuffer(&levelUp, sizeof(levelUp));

			if (IS_KINDOF(CGolfTask, AfxGetTask()))
			{
				AfxGetTask()->GetActor("Score")
					<< MsgObject(NULL, 0x12d, (int)&levelUp, 0, 0, 0, 0);
			}
			else if (IS_KINDOF(CLobbyTask, AfxGetTask()))
			{
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 0x12d, (int)&levelUp, 0, 0, 0, 0);
			}
			else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x12d, (int)&levelUp, 0, 0, 0, 0);
			}
		}
		break;

	case 0x111:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (IsLocalContent((localContentType_t)0x2e))
			{
				IActor* pLobby = AfxGetTask()->GetActor("Lobby");
				if (pLobby)
					pLobby << MsgObject(NULL, 2, 0, 0, 0, 0, 0);

				CIntrusion::Instance()->PacketAnalysis(packet);
			}
		}
		break;

	case 0x10a:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		g_iRecvDisconCode = packet.Decode4();
		break;

	case 0x110:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		if (packet.Decode1())
		{
			bool bShow =
				IsLocalContent((localContentType_t)0x15) ? true : false;
			Doc()->m_bToppageAd = bShow;
		}
		else
		{
			Doc()->m_bToppageAd = false;
		}
		break;

	case 0x118:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		if (IsLocalContent((localContentType_t)0x14))
		{
			switch (packet.Decode1())
			{
			case 1:
				Doc()->m_toppageFeature = 1;
				break;

			case 0:
				Doc()->m_toppageFeature = 0;
				break;

			case 2:
				Doc()->m_toppageFeature = 2;
				break;

			default:
				Doc()->m_toppageFeature = 0;
				break;
			}
		}
		else
		{
			Doc()->m_toppageFeature = 0;
		}
		break;

	case 0x119:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			CWorldTourEvent* pEvent = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x1e,
				pEvent);

			if (pEvent == NULL)
				break;

			if (IsLocalContent((localContentType_t)0x1e))
			{
				pEvent->SetMapFlag(packet.Decode4());

				if (packet.Decode1())
					pEvent->SetGotGift(true);
				else
					pEvent->SetGotGift(false);

				pEvent->SetCanGift(false);
				if (pEvent->GetMapFlag() == 0xffff && !pEvent->GetGotGift())
					pEvent->SetCanGift(true);
			}
			else
			{
				pEvent->SetMapFlag(0);
				pEvent->SetGotGift(false);
				pEvent->SetCanGift(false);
			}
		}
		break;

	case 0x11a:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			CWorldTourEvent* pEvent = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x1e,
				pEvent);

			if (pEvent == NULL)
				break;

			if (!packet.Decode1())
			{
				pEvent->SetMapFlag(0xffff);
				pEvent->SetGotGift(false);
				pEvent->SetCanGift(true);
			}
			else
			{
				WSendPacket send(0x27);
				send.Encode4(MyUID());
				send.Encode4(Doc()->m_giftIndex);
				send.Send(TO_MSN);
			}
		}
		break;

	case 0x11f:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			CHalloweenEvent* pEvent = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x21,
				pEvent);

			if (pEvent == NULL)
				break;

			pEvent->SetMapFlag(packet.Decode4());
			pEvent->SetGiftCnt(packet.Decode4());
		}
		break;

	case 0x120:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			if (!IsLocalContent((localContentType_t)0x21))
				break;

			CHalloweenEvent* pEvent = NULL;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x21,
				pEvent);

			if (pEvent)
			{
				pEvent->SetGiftCnt(packet.Decode4());
				pEvent->SetShowGiftDlg(true);
			}

			WSendPacket send(0x21);
			send.EncodeStr(std::string(MyNick()));
			send.Encode4(MyUID());
			send.Send(TO_MSN);
		}
		break;

	case 0x122:
		LogOut(0, "CTask::OnPacketCommon %d %d\n", type, g_iRecvPacketType);
		{
			unsigned long type = packet.Decode4();
			switch (type)
			{
			case 0:
				CTHunter::Instance()->SetUsedTikiReport(true);
				if (IsLocalContent((localContentType_t)0x58))
					AfxGetTask()->SendMsgToMainActor(
						MsgObject(NULL, 0x6c, 1, 0, 0, 0, 0));
				break;

			case 1:
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x21b, 0, 0, 0, 0, 0));
				break;

			case 2:
			{
				FrImageNoticeDlg* pDlg = CreateForm<FrImageNoticeDlg>(
					g_pFresh->GetManager(), this, "imagenoticedlg");

				pDlg->SetCaption(
					"\xbd\xc5\xb1\xd4\xb0\xa1\xc0\xd4 \xc0\xcc\xba\xa5\xc6\xae");

				pDlg->SetDlgSize(WRect(-1.0f, -1.0f, 301.0f, 309.0f));

				pDlg->SetBackGroundImage(0, 0,
					WRect(0.0f, 0.0f, 256.0f, 220.0f), "event_new_02_a", true);
				pDlg->SetBackGroundImage(2, 0,
					WRect(256.0f, 0.0f, 314.0f, 247.0f), "event_new_02_b",
					true);

				pDlg->SetOkButton(WRect(114.0f, 247.0f, 0.0f, 0.0f), true);

				pDlg->SetDesc(
					"8\xbf\xf9 31\xc0\xcf \xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");

				pDlg->Open(NULL, 0);
			}
			break;

			case 3:
			{
				FrImageNoticeDlg* pDlg = CreateForm<FrImageNoticeDlg>(
					g_pFresh->GetManager(), this, "imagenoticedlg");

				pDlg->SetCaption(
					"\xbd\xc5\xb1\xd4\xb0\xa1\xc0\xd4 \xc0\xcc\xba\xa5\xc6\xae");

				pDlg->SetDlgSize(WRect(-1.0f, -1.0f, 342.0f, 334.0f));

				pDlg->SetBackGroundImage(0, 0,
					WRect(0.0f, 0.0f, 256.0f, 256.0f), "event_new_03_a", true);
				pDlg->SetBackGroundImage(2, 0,
					WRect(256.0f, 0.0f, 333.0f, 256.0f), "event_new_03_b",
					true);

				pDlg->SetOkButton(WRect(127.0f, 273.0f, 0.0f, 0.0f), true);

				pDlg->SetDesc(
					"8\xbf\xf9 31\xc0\xcf \xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");

				pDlg->Open(NULL, 0);

				WSendPacket send(0x21);
				send.EncodeStr(std::string(MyNick()));
				send.Encode4(MyUID());
			}
			break;

			case 4:
			{
				unsigned long uid = packet.Decode4();
				if (Doc()->m_myInfo.info.dwGuid == uid)
				{
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify");
					pForm->SetTimeLimit(2.0f);
					pForm->SetMessage(
						"\xc7\xf6\xc0\xe7 \xc0\xe5\xc2\xf8\xb5\xc8 \xbe\xc6\xc1\xee\xc5\xd8\xc0\xbb \xb8\xf0\xb5\xce \xbc\xd2\xb8\xf0\xc7\xcf\xbf\xb4\xc0\xb8\xb9\xc7\xb7\xce, \xc0\xe5\xc2\xf8 \xbe\xc6\xc1\xee\xc5\xd8\xc0\xbb \xb1\xe2\xba\xbb \xbe\xc6\xc1\xee\xc5\xd8\xc0\xb8\xb7\xce \xb1\xb3\xc3\xbc \xc7\xd5\xb4\xcf\xb4\xd9.",
						false);
					pForm->Open(NULL, 0);

					Doc()->m_myInfo.userEquip.tidBall = 0x14000000;
				}

				for (int i = 0; i < 4; i++)
				{
					if (uid == Doc()->m_userInfo[i].info.dwGuid)
					{
						Doc()->m_userInfo[i].userEquip.tidBall = 0x14000000;
						AfxGetTask()->GetActor("ball")
							<< MsgObject(NULL, 0x4d, 0x14000000, i, 0, 0, 0);
					}
				}
			}
			break;

			case 5:
			{
				std::string nick = packet.DecodeStr();
				char msg[128] = "\0";
				sprintf(msg,
					"\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xc1\xdf\xc0\xce \xb0\xd4\xc0\xd3\xbf\xa1\xbc\xad '%s'\xb4\xd4\xbf\xa1 \xc0\xc7\xc7\xd8 \xc6\xf8\xc1\xd7\xc0\xcc \xc5\xcd\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.",
					nick.c_str());
				CChatMsg::Instance()->AddChatMsg(msg, 0xffff0000, true, false);
			}
			break;

			case 0xb:
			{
				std::string notice = packet.DecodeStr();
				CMessengerInfo::m_pInstance->Notify2(
					MakeStr("%s", notice.c_str()));
			}
			break;

			case 8:
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x228, 0x122, 1, 0, 0, 0);
				break;

			case 9:
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x228, 0x122, 0, 0, 0, 0);
				break;
			}
		}
		break;
	case 0x129:
	{
		if (!IsLocalContent(S4_TREASURE_HUNTER))
			return 0;

		unsigned char type = packet.Decode1();

		sTreasureItem item;
		switch (type)
		{
		case 0:
			packet.DecodeBuffer(&item, sizeof(item));
			CTHunter::Instance()->SetTHunterGauge(item.byMapIndex,
				item.dwGauge);
			break;
		case 1:
		{
			unsigned char count = packet.Decode1();
			for (unsigned char i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&item, sizeof(item));
				CTHunter::Instance()->SetTHunterGauge(item.byMapIndex,
					item.dwGauge);
			}
			break;
		}
		}
		break;
	}
	case 0x12a:
	{
		if (!IsLocalContent(S4_TREASURE_HUNTER))
			return 0;

		switch (GetGameType())
		{
		case GAME_TYPE_STROKE:
		case GAME_TYPE_TEAM:
		case GAME_TYPE_30S:
		case GAME_TYPE_30S_TEAM:
		{
			unsigned long point = packet.Decode4();
			CTHunter::Instance()->SetTreasurePoint(point);

			GetActor("Score") << MsgObject(NULL, 0x12f, 4, 0, 0, 0, 0);
			break;
		}
		}
		break;
	}
	case 0x12b:
	{
		std::string unused;
		std::vector<sTreasureHunt> repository;
		sTreasureHunt hunt;

		unsigned char count = packet.Decode1();
		repository.reserve(count);
		for (unsigned char i = 0; i < count; i++)
		{
			packet.DecodeBuffer(&hunt, sizeof(hunt));
			repository.push_back(hunt);
		}

		CTHunter::Instance()->ReceiveRepository(repository);
		CTHunter::Instance()->SetReceiveGiftPacket(true);
		Doc()->SetAppGiftDownloadState((CSharedDoc::eAppGiftDownloadState)2);
		break;
	}
	case 0x12c:
	{
		if (!IsLocalContent(S4_TREASURE_HUNTER_AD))
			break;

		std::vector<sTreasureGift> gifts;
		sTreasureGift gift;
		unsigned char count = packet.Decode1();
		for (unsigned char i = 0; i < count; i++)
		{
			packet.DecodeBuffer(&gift, sizeof(gift));
			gifts.push_back(gift);
		}

		CTHunter::Instance()->ReceiveAllGift(gifts);
		CTHunter::Instance()->UpdateItemList("");
		break;
	}
	case 0x138:
	{
		if (!IsLocalContent(S4_MATCHING_SYSTEM))
			break;

		unsigned char result = packet.Decode1();
		if (result == 0)
		{
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
				(int)"\xb0\xfc\xbd\xc9\xbb\xe7 \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		break;
	}
	case 0x126:
	{
		switch (packet.Decode1())
		{
		case 0:
		{
			unsigned char result = packet.Decode1();
			unsigned long typeId = packet.Decode4();
			unsigned long id = packet.Decode4();
			char uccIndex[9];
			char uccName[64];
			strncpy(uccIndex, packet.DecodeStr().c_str(), 9);
			strncpy(uccName, packet.DecodeStr().c_str(), 41);

			GetActor("RealMyRoom") << MsgObject(NULL, 0x214, result,
				(int)&typeId, (int)&id, (int)uccIndex, (int)uccName);
			break;
		}
		case 1:
		{
			sItemInfo item;
			char uccIndex[9];

			packet.Decode4();
			strncpy(uccIndex, packet.DecodeStr().c_str(), 9);
			unsigned char slot = packet.Decode1();
			packet.DecodeBuffer(&item, sizeof(item));

			if (slot == 1)
			{
				bool bFound = false;
				std::list<sItemInfo>& ucc = Doc()->m_uccItemList;
				std::list<sItemInfo>::iterator it;
				for (it = ucc.begin(); it != ucc.end(); it++)
				{
					if ((*it).tid == item.tid &&
						!strcmp((*it).UccIndex, item.UccIndex) &&
						(*it).Seq == item.Seq)
					{
						bFound = true;
						break;
					}
				}

				if (!bFound)
				{
					ucc.push_back(item);

					GetActor("RealMyRoom")
						<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
				}
			}

			if (!(item.status & 1))
				break;

			sUccClothes* pClothes = UccManager()->FindClothes(item.guid);
			if (pClothes)
			{
				pClothes->date = item.ItemDate;
				if (strlen(item.ItemName))
					strcpy(pClothes->uccName, item.ItemName);
			}
			else
			{
				sUccClothes clothes;
				clothes.id = item.guid;
				clothes.date = item.ItemDate;
				clothes.downloadCount = 0;
				clothes.icon = NULL;
				if (strlen(item.ItemName))
					strcpy(clothes.uccName, item.ItemName);
				clothes.typeId = item.tid;
				strncpy(clothes.uccIndex, item.UccIndex, 9);
				UccManager()->AddClothes(slot, clothes, false, false);
			}
			break;
		}
		case 2:
		{
			unsigned long typeId = packet.Decode4();
			char uccIndex[9];
			strncpy(uccIndex, packet.DecodeStr().c_str(), 9);
			unsigned short seq = packet.Decode2();

			unsigned long guid = packet.Decode4();
			unsigned long newId = packet.Decode4();
			unsigned long newTypeId = packet.Decode4();
			char newUccIndex[9];
			strncpy(newUccIndex, packet.DecodeStr().c_str(), 9);
			unsigned short newSeq = packet.Decode2();

			unsigned char result = packet.Decode1();
			if (result == 1)
			{
				UccManager()->FindClothes(typeId, uccIndex);

				sUccClothes clothes;
				clothes.typeId = newTypeId;
				clothes.id = newId;
				strncpy(clothes.uccIndex, newUccIndex, 9);
				UccManager()->AddClothes(1, clothes, false, false);

				FrUccCopyCompletedDlg* pDlg = CreateForm<FrUccCopyCompletedDlg>(
					g_pFresh->GetManager(), this, "ucc_copy_completed");
				pDlg->Init(typeId, uccIndex);
				pDlg->Open(NULL, WPoint(453.0f, 189.0f), 1);

				sItemInfo* pSource = GetMyItemInfo(typeId, uccIndex, seq);

				std::list<sItemInfo>& ucc = Doc()->m_uccItemList;
				std::list<sItemInfo>::iterator it;
				for (it = ucc.begin(); it != ucc.end(); it++)
				{
					if ((*it).guid == guid)
					{
						(*it).guid = newId;
						(*it).tid = newTypeId;
						strncpy((*it).UccIndex, uccIndex, 9);
						(*it).Seq = newSeq;

						(*it).status &= ~2;
						(*it).status |= 1;
						strncpy((*it).CopierNick, Doc()->m_myInfo.info.sNick,
							22);
						const char* pName = pSource->ItemName;
						if (!pSource)
							pName =
								"\xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 \xc1\xdf...";
						strncpy((*it).ItemName, pName, 41);
						break;
					}
				}

				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
					(int)"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3 \xba\xb9\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
			break;
		}
		case 3:
		{
			unsigned long typeId = packet.Decode4();
			char uccIndex[9];
			memset(uccIndex, 0, sizeof(uccIndex));
			strncpy(uccIndex, packet.DecodeStr().c_str(), 9);
			unsigned char result = packet.Decode1();

			if (result == 1)
			{
				std::list<sItemInfo>& ucc = Doc()->m_uccItemList;
				std::list<sItemInfo>::iterator it;
				for (it = ucc.begin(); it != ucc.end(); it++)
				{
					if ((*it).tid == typeId &&
						!strcmpi((*it).UccIndex, uccIndex))
					{
						(*it).status |= 2;
						break;
					}
				}
			}

			GetActor("RealMyRoom") << MsgObject(NULL, 0x215, result,
				(int)&typeId, (int)uccIndex, 0, 0);
			break;
		}
		}
		break;
	}
	case 0x13c:
	{
		CMatchingSystem* pMatching = CMatchingSystem::Instance();
		if (!pMatching)
			break;

		unsigned char count = packet.Decode1();
		if (count > 0)
		{
			sIntrest intrest;
			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&intrest, sizeof(intrest));
				pMatching->SetDisplayIntrest(intrest);
			}
		}
		break;
	}
	case 0x14f:
	{
		packet.Decode1();
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
		{
			sUserInfoTime info;
			memset(&info, 0, sizeof(info));
			if (!Doc()->InsertUserInfoTime(uid, info))
				break;
			if (!Doc()->FindUserInfoTimeUID(uid, it))
				break;
		}

		unsigned short roomIndex = packet.Decode2();
		sUserInfoTime* pInfo = &(*it).second;
		packet.DecodeBuffer(&(*it).second.info.info, sizeof(sPangYaUserInfo));
		pInfo->info.roomIndex = roomIndex;
		unsigned long guildPoint = packet.Decode4();
		(*it).second.dwGuildPoint = guildPoint;
		break;
	}
	case 0x150:
	{
		unsigned char type = packet.Decode1();
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		int found = Doc()->FindUserInfoTimeUID(uid, it);
		if (!found)
		{
			sUserInfoTime info;
			memset(&info, 0, sizeof(info));
			if (Doc()->InsertUserInfoTime(uid, info))
				found = Doc()->FindUserInfoTimeUID(uid, it);
		}

		sPangYaUserStatistics stat;
		packet.DecodeBuffer(&stat, sizeof(stat));

		if (Doc()->m_myInfo.info.dwUID == uid)
		{
			if (type != 0)
			{
				if (type == 5)
					Doc()->m_myInfo.stat = stat;
				else
					Doc()->m_myInfo.stat = stat;
			}
			else
				Doc()->m_oldSeasonStatistics = stat;
		}

		if (!found)
			break;

		if (type == 5)
			(*it).second.info.stat = stat;
		if (type == 0)
			(*it).second.oldSeason.userStat = stat;
		break;
	}
	case 0x14e:
	{
		packet.Decode1();
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		packet.DecodeBuffer(&(*it).second.info.userEquip, sizeof(sUserEquip));
		break;
	}
	case 0x155:
	{
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		GUILD_INFO guild;
		packet.DecodeBuffer(&guild, sizeof(guild));

		(*it).second.dwGuildPang = guild.guildPang;
		if (uid == MyUID())
			(*it).second.dwGuildPoint = guild.guildPoint;
		strcpy((*it).second.info.info.szEmblemName, guild.guildMark);
		strcpy((*it).second.info.info.sGuild, guild.guildName);
		break;
	}
	case 0x153:
	{
		unsigned char type = packet.Decode1();
		unsigned long uid = packet.Decode4();
		unsigned short count = packet.Decode2();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		sGuildTrophy** ppTrophies;
		int* pNum;
		if (type == 5)
		{
			ppTrophies = &(*it).second.guildTrophies;
			pNum = &(*it).second.numGuildTrophies;
		}
		else if (type == 0)
		{
			ppTrophies = &(*it).second.oldSeason.guildTrophies;
			pNum = &(*it).second.oldSeason.numGuildTrophies;
		}
		else
			break;

		if (!ppTrophies)
			break;

		if (*ppTrophies)
		{
			delete[] *ppTrophies;
			*ppTrophies = NULL;
		}

		if (count)
		{
			sGuildTrophy* pTrophies = new sGuildTrophy[count];
			for (int i = 0; i < count; i++)
				packet.DecodeBuffer(&pTrophies[i], sizeof(sGuildTrophy));
			*ppTrophies = pTrophies;
		}
		*pNum = count;
		break;
	}
	case 0x151:
	{
		unsigned char type = packet.Decode1();
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		sTrophyStatistics trophy;
		packet.DecodeBuffer(&trophy, sizeof(trophy));

		if (type == 5)
			(*it).second.info.trophy = trophy;
		if (type == 0)
			(*it).second.oldSeason.trophyStat = trophy;
		break;
	}
	case 0x152:
	{
		unsigned char type = packet.Decode1();
		unsigned long uid = packet.Decode4();
		unsigned short count = packet.Decode2();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		sSpecialTrophy** ppTrophies;
		int* pNum;
		if (type == 5)
		{
			ppTrophies = &(*it).second.trophies;
			pNum = &(*it).second.numTrophies;
		}
		else if (type == 0)
		{
			ppTrophies = &(*it).second.oldSeason.trophies;
			pNum = &(*it).second.oldSeason.numTrophies;
		}
		else
			break;

		if (!ppTrophies)
			break;

		if (*ppTrophies)
		{
			delete[] *ppTrophies;
			*ppTrophies = NULL;
		}

		if (count)
		{
			sSpecialTrophy* pTrophies = new sSpecialTrophy[count];
			for (int i = 0; i < count; i++)
			{
				sSpecialTrophy trophy;
				packet.DecodeBuffer(&trophy, sizeof(trophy));
				pTrophies[i] = trophy;
			}
			*ppTrophies = pTrophies;
		}
		*pNum = count;
		break;
	}
	case 0x154:
	{
		unsigned char type = packet.Decode1();
		unsigned long uid = packet.Decode4();
		unsigned short count = packet.Decode2();
		if (!count)
			break;

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		sUserInfoTime* pInfo = &(*it).second;
		sMapStatistics mapStat[20];
		int size = count * sizeof(sMapStatistics);
		packet.DecodeBuffer(mapStat, size);

		sMapStatistics* pDest;
		if (type == 5)
			pDest = pInfo->info.mapStat;
		else if (type == 0)
			pDest = pInfo->oldSeason.mapStat;
		else if (type == 10)
			pDest = pInfo->oldSeason.classicMapStat;
		else if (type == 51)
			pDest = pInfo->info.classicMapStat;
		else
			break;

		if (pDest)
			memcpy(pDest, mapStat, size);
		break;
	}
	case 0x156:
	{
		unsigned long uid = packet.Decode4();

		std::map<unsigned long, sUserInfoTime>::iterator it;
		if (!Doc()->FindUserInfoTimeUID(uid, it))
			break;

		sCharacterInfo charInfo;
		packet.DecodeBuffer(&charInfo, sizeof(charInfo));
		(*it).second.info.charInfo = charInfo;

		Doc()->SetUserInfoMod(0);

		if (IsLocalContent((localContentType_t)0x57))
			RequestUccInfo(charInfo);
		break;
	}
	case 0x14b:
	{
		switch (packet.Decode1())
		{
		case 0:
		{
			unsigned char content = packet.Decode1();
			unsigned long index = packet.Decode4();
			std::string key = packet.DecodeStr();
			unsigned char result = packet.Decode1();
			SecurityKeyManager()->ResCreateKey((eSKContent)content, index, key,
				result);
			break;
		}
		case 1:
		{
			unsigned char content = packet.Decode1();
			unsigned long index = packet.Decode4();
			std::string key = packet.DecodeStr();
			unsigned char result = packet.Decode1();
			SecurityKeyManager()->ResCheckKey((eSKContent)content, index, key,
				result);
			break;
		}
		}
		break;
	}
	case 0x15b:
	{
		unsigned char count = packet.Decode1();
		unsigned short mode = packet.Decode2();

		char msg[256] = "\0";
		char server[40] = "\0";
		char grade[40] = "\0";
		char map[40] = "\0";

		wsprintf(msg,
			"\xc3\xd1 \xb0\xb3\xbc\xb3\xb5\xc8 \xb9\xe6 \xb0\xb3\xbc\xf6 %d",
			count);
		CChatMsg::Instance()->AddChatMsg(msg, 0xffff7878, false, false);

		for (int i = 0; i < count; i++)
		{
			sGMRoomInfo info;
			packet.DecodeBuffer(&info, sizeof(info));
			if ((info.flags & 3) || mode != 4 || (info.flags & 4))
			{
				wsprintf(server, "%d", info.serverId);

				std::list<sGameServerInfo>& servers = Doc()->m_gameServerList;
				for (std::list<sGameServerInfo>::iterator it = servers.begin();
					it != servers.end(); ++it)
				{
					if ((*it).guid == info.serverId)
					{
						wsprintf(server, "%s", (*it).name);
						break;
					}
				}

				switch (info.grade)
				{
				case 0:
					wsprintf(grade, "%s", "\xb7\xe7\xc5\xb0");
					break;
				case 1:
					wsprintf(grade, "%s", "\xc3\xca\xba\xb8");
					break;
				case 2:
					wsprintf(grade, "%s", "\xc1\xdf\xb1\xde");
					break;
				case 3:
					wsprintf(grade, "%s",
						"\xc0\xda\xc0\xaf"
						"1");
					break;
				case 4:
					wsprintf(grade, "%s",
						"\xc0\xda\xc0\xaf"
						"2");
					break;
				default:
					wsprintf(grade, "%d", info.grade);
					break;
				}

				switch (info.map)
				{
				case 0:
					wsprintf(map, "%s", "\xba\xed\xb7\xe7\xb6\xf3\xb1\xba");
					break;
				case 1:
					wsprintf(map, "%s", "\xba\xed\xb7\xe7\xbf\xf6\xc5\xcd");
					break;
				case 2:
					wsprintf(map, "%s",
						"\xbc\xbc\xc7\xc7\xbe\xc6\xc0\xa9\xb5\xe5");
					break;
				case 3:
					wsprintf(map, "%s", "\xc0\xa9\xb5\xe5\xc8\xfa");
					break;
				case 4:
					wsprintf(map, "%s", "\xc0\xa7\xc1\xee\xc0\xa7\xc1\xee");
					break;
				case 5:
					wsprintf(map, "%s",
						"\xbf\xfe\xbd\xba\xc6\xae\xc0\xa7\xc1\xee");
					break;
				case 6:
					wsprintf(map, "%s", "\xba\xed\xb7\xe7\xb9\xae");
					break;
				case 7:
					wsprintf(map, "%s",
						"\xbd\xc7\xba\xf1\xbe\xc6\xc4\xb3\xb3\xed");
					break;
				case 8:
					wsprintf(map, "%s",
						"\xbe\xc6\xc0\xcc\xbd\xba\xc4\xb3\xb3\xed");
					break;
				case 9:
					wsprintf(map, "%s",
						"\xc8\xad\xc0\xcc\xc6\xae\xc0\xa7\xc1\xee");
					break;
				case 10:
					wsprintf(map, "%s",
						"\xbb\xfe\xc0\xcc\xb4\xd7\xbb\xf7\xb5\xe5");
					break;
				case 11:
					wsprintf(map, "%s", "\xc7\xce\xc5\xa9\xc0\xa9\xb5\xe5");
					break;
				case 13:
					wsprintf(map, "%s",
						"\xb5\xf6\xc0\xce\xc6\xe4\xb8\xa3\xb3\xeb");
					break;
				case 14:
					wsprintf(map, "%s",
						"\xbe\xc6\xc0\xcc\xbd\xba\xbd\xba\xc6\xc4");
					break;
				case 15:
					wsprintf(map, "%s",
						"\xb7\xce\xbd\xba\xc6\xae\xbd\xc3\xbf\xfe\xc0\xcc");
					break;
				case 16:
					wsprintf(map, "%s",
						"\xc0\xcc\xbd\xba\xc5\xcf\xb9\xeb\xb8\xae");
					break;
				case 17:
					wsprintf(map, "%s",
						"\xbd\xba\xc6\xf3\xbc\xc8\xbc\xc5\xc7\xc3");
					break;
				case 19:
					wsprintf(map, "%s", "\xc0\xa7\xc1\xee\xbd\xc3\xc6\xbc");
					break;
				case 0x7f:
					wsprintf(map, "%s", "\xb7\xa3\xb4\xfd");
					break;
				default:
					wsprintf(map, "%d", info.map);
					break;
				}

				wsprintf(msg, "%s %s %d\xb9\xf8\xb9\xe6 %s %d\xc8\xa6", server,
					grade, info.roomNumber, map, info.holes);
				CChatMsg::Instance()->AddChatMsg(msg, 0xffff7878, false, false);
			}
		}
		break;
	}
	case 0x123:
	{
		if (!IsLocalContent((localContentType_t)0x56))
			break;

		if (packet.Decode4())
		{
			sRealMyRoomAuthority authority;
			packet.DecodeBuffer(&authority, sizeof(authority));

			Doc()->m_rmrAuthority.ownerUID = authority.ownerUID;
			Doc()->m_rmrAuthority.bPrivate = authority.bPrivate;
			Doc()->m_rmrAuthority.unknown4 = authority.unknown4;
			strcpy(Doc()->m_rmrAuthority.password, authority.password);

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x21f, 0, 0, 0, 0, 0));
		}
		else
		{
			if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
				AfxPostMsg(NULL, "RealMyRoom", 0x228, 0x123, 0, 0, 0);
			else
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x228, 0x123, 0, 0, 0, 0));
		}
		break;
	}
	case 0x186:
	{
		int type = packet.Decode4();
		if (type == 2)
		{
			int result = packet.Decode4();
			if (result == 1)
			{
				unsigned long typeId = packet.Decode4();
				_RemoveWareHouseItem(typeId,
					AfxGetTask()->GetActor("RealMyRoom"), 1);
			}
		}

		packet.Reset();
		CItemBuff::ProcessPacket(packet);
		break;
	}
	case 0x188:
	{
		if (IsLocalContent((localContentType_t)0x78) == true)
		{
			unsigned long param = packet.Decode4();
			if (IS_KINDOF(CLobbyTask, AfxGetTask()))
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 0x248, param, 0, 0, 0, 0);
		}
		break;
	}
	case 0x189:
	{
		if (IsLocalContent((localContentType_t)0x78) == true)
		{
			unsigned long typeId = packet.Decode4();
			_RemoveWareHouseItem(typeId, AfxGetTask()->GetActor("Lobby"), 100);

			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetTimeLimit(3.0f);
			pForm->SetMessage(
				"\xc0\xcc\xba\xa5\xc6\xae \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9. \xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.",
				false);
			pForm->Open(NULL, 0);
		}
		break;
	}
	case 0x18d:
	{
		unsigned char result = packet.Decode1();

		FrCardRemoveResult* pDlg = CreateForm<FrCardRemoveResult>(
			g_pFresh->GetManager(), this, "card_remove_result");
		if (!pDlg)
			break;

		if (result == 1)
		{
			unsigned long partGuid = packet.Decode4();
			unsigned long remover = packet.Decode4();
			if (FrCardRemove::IsCardRemoveItem(remover) == true)
			{
				pDlg->SetRemover(remover);
				_RemoveWareHouseItem(remover,
					AfxGetTask()->GetActor("RealMyRoom"), 1);

				if (partGuid)
				{
					std::list<unsigned long> cards;
					if (S5::UTIL::ErasePartsCardAll(partGuid, cards))
					{
						for (std::list<unsigned long>::iterator it =
								 cards.begin();
							it != cards.end(); ++it)
							pDlg->PushList(*it);
					}
					pDlg->SetResult(FrCardRemoveResult::RESULT_SUCCESS);
				}
				else
					pDlg->SetResult(FrCardRemoveResult::RESULT_FAIL);
			}
			else
				pDlg->SetResult(FrCardRemoveResult::RESULT_ERROR);
		}
		else if (result == 0)
		{
			unsigned char error = packet.Decode1();
			if (error != 9)
				pDlg->SetResult(FrCardRemoveResult::RESULT_ERROR);
			else
				pDlg->SetResult(FrCardRemoveResult::RESULT_ABNORMAL);
		}

		pDlg->SetCaption("\xbb\xe7\xbf\xeb\xb0\xe1\xb0\xfa");
		pDlg->Open(NULL, 0);
		break;
	}
	case 0x18b:
	{
		if (IsLocalContent((localContentType_t)0x79))
		{
		}
		break;
	}
	case 0x198:
	{
		SetMyOutsideCash(packet.Decode8());
		Doc()->RefreshAllCookie();
		break;
	}
	case 0x19a:
	{
		unsigned char type = packet.Decode1();
		unsigned long count = packet.Decode4();

		std::map<unsigned char, sMapStatistics>& mapStat =
			Doc()->GetMyPastMapStat(type);
		for (unsigned long i = 0; i < count; i++)
		{
			unsigned char map = packet.Decode1();
			sMapStatistics stat;
			packet.DecodeBuffer(&stat, sizeof(stat));
			mapStat.insert(
				std::map<unsigned char, sMapStatistics>::value_type(map, stat));
		}
		break;
	}
	case 0x1a1:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));

		if (!IsLocalContent((localContentType_t)0x8e))
			break;

		unsigned long count = packet.Decode4();
		for (unsigned long i = 0; i < count; i++)
		{
			unsigned long typeId = packet.Decode4();
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x252, (int)&typeId, 0, 0, 0, 0));
		}
		break;
	}
	case 0x1cf:
	{
		unsigned long param1 = packet.Decode4();
		unsigned long param2 = packet.Decode4();

		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 0x26d, 15, param1, param2, 0, 0);
		break;
	}
	case 0x1d0:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 2, 0, 0, 0, 0, 0));
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(NULL, 0x26d, 17, 0, 0, 0, 0);
		break;
	}
	case 0x30:
	case 0x60:
	case 0xff:
	case 0x11e:
	case 0x13b:
	case 0x19e:
		break;

	default:
		return 0;
	}

	return 1;
}

int CTask::_RemoveWareHouseItem(unsigned long uid, IActor* pActor, int count)
{
	unsigned long guid = 0;
	ITEM_LIST_IT it;
	if (uid == 0x1a000042 && IsLocalContent(S3_CADDIE_REPORT))
	{
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 0x204, (int)&guid, 0, 0, 0, 0);
		for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
			++it)
		{
			if ((*it).guid == guid)
				break;
		}
	}
	else
	{
		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			uid);
	}

	if (it == Doc()->m_myItemList.end())
		return 0;

	int i;
	for (i = 0; i < count; i++)
	{
		(*it).Common[0]--;

		if ((*it).Common[0] <= 0)
		{
			Doc()->m_myItemList.erase(it);
			break;
		}
	}

	pActor << MsgObject(NULL, 0xdd, 0, 0, 0, 0, 0);

	return i;
}

int CTask::_RemoveWareHouseItem(unsigned long uid, const char* actorName,
	int count)
{
	return _RemoveWareHouseItem(uid, AfxGetTask()->GetActor(actorName), count);
}

void CTask::_ReFreshMyRoomGift(bool bRefresh, std::string name)
{
	if (bRefresh)
	{
		WSendPacket send(0x21);

		send.EncodeStr(name);
		send.Encode4(MyUID());
		send.Send(TO_MSN);
	}
	else
	{
		Doc()->m_giftList.clear();
		WSendPacket send((enumClientPacket)0x93);
		send.Send(TO_GAME);
	}
}

bool CTask::OnOverlappedDlg(int result, FrForm* pForm)
{
	g_bQuit = true;
	return true;
}

CTaskManager::CTaskManager()
	: m_bChangeTask(false),
	  m_bPreserveTask(false),
	  m_nextTaskName(NULL),
	  m_nextDocName(NULL),
	  m_pCurrentTask(NULL),
	  m_pPreservedTask(NULL),
	  m_pFadeBg(NULL)
{
	m_curHistory = m_history.begin();
	m_naviDirection = 0;
}

CTaskManager::~CTaskManager()
{
	ClearTask();

	if (m_pPreservedTask)
	{
		m_pPreservedTask->Destroy();
		delete m_pPreservedTask;
		m_pPreservedTask = NULL;
	}

	if (m_pFadeBg)
	{
		delete m_pFadeBg;
		m_pFadeBg = NULL;
	}
}

void CTaskManager::CreateTask(const char* task, const char* doc, bool bPreserve)
{
	if (bPreserve)
	{
		if (m_pPreservedTask == NULL)
		{
			m_pPreservedTask = m_pCurrentTask;
			m_pPreservedTask->PreserveBack(task);
			m_pCurrentTask = NULL;
		}

		ClearTask();
	}
	else
	{
		ClearTask();

		if (m_pPreservedTask)
		{
			if (stricmp(m_pPreservedTask->GetRTTI()->GetName(), task) == 0)
			{
				m_pCurrentTask = m_pPreservedTask;
				m_pCurrentTask->RestorePreserved();
				m_pPreservedTask = NULL;
				m_bChangeTask = false;
				m_nextTaskName = NULL;
				m_nextDocName = NULL;

				if (m_pFadeBg)
					m_pFadeBg->ResetScreenSize();

				return;
			}

			m_pPreservedTask->Destroy();
			delete m_pPreservedTask;
			m_pPreservedTask = NULL;
		}
	}

	m_pCurrentTask = (CTask*)ObjectFactory().CreateObject(task);

	m_pCurrentTask->ApplyScreenSize();

	if (m_pFadeBg)
		m_pFadeBg->ResetScreenSize();

	if (doc && doc[0])
	{
		IObject* pDoc = ObjectFactory().CreateObject(doc);
		m_pCurrentTask->m_pDoc = DYNAMIC_CAST(CTaskDoc, pDoc);
		if (m_pCurrentTask->m_pDoc)
			m_pCurrentTask->m_pDoc->m_pTask = m_pCurrentTask;
	}
	else
	{
		m_pCurrentTask->m_pDoc = NULL;
	}

	m_pCurrentTask->Register();
	m_pCurrentTask->PreLoadInit();

	g_LoadParam.loadedNum = 0;
	g_LoadParam.bLoaded = false;
	g_LoadParam.bInitialized = false;
	m_bPreserveTask = bPreserve;
	m_bChangeTask = false;
	m_nextTaskName = NULL;
	m_nextDocName = NULL;
	m_pCurrentTask->Load();
}

void CTaskManager::ChangeTask(const char* task, const char* doc, bool bPreserve)
{
	MESSENGER()->CloseWindow();

	m_nextTaskName = task;
	m_nextDocName = doc;
	m_bChangeTask = true;
	m_bPreserveTask = bPreserve;

	g_audio->SetFadeOutTime(1.0f);

	if (m_pFadeBg)
	{
		delete m_pFadeBg;
		m_pFadeBg = NULL;
	}
	m_pFadeBg = new CBackGround(NULL, true);
	m_fadeAlpha = 1.0f;
}

void CTaskManager::PreProcess()
{
	if (m_bChangeTask)
		CreateTask(m_nextTaskName, m_nextDocName, m_bPreserveTask);
}

void CTaskManager::ProcessTask(float delta)
{
	if (m_bChangeTask)
		return;

	if (m_pFadeBg)
	{
		float step = (m_pCurrentTask->m_bSyncLoad ? 0.8f : 1.0f) * delta;
		if (step > 0.04f)
			step = 0.04f;

		m_fadeAlpha -= step;
		if (m_fadeAlpha < 0.0f)
		{
			m_fadeAlpha = 0.0f;
			delete m_pFadeBg;
			m_pFadeBg = NULL;
		}
	}

	if (!g_LoadParam.bLoaded)
	{
		m_pCurrentTask->OnProcessWhileLoading(delta);
		Sleep(30);
		return;
	}

	if (!g_LoadParam.bInitialized)
	{
		m_pCurrentTask->Init(NULL);
		g_LoadParam.bInitialized = true;
	}

	while (!m_postMsgQueue.empty())
	{
		sPostMsg& postMsg = m_postMsgQueue.front();
		m_pCurrentTask->GetActor(postMsg.target.c_str()) << postMsg.msg;
		m_postMsgQueue.pop();
	}

	m_pCurrentTask->Process(delta);
}

void CTaskManager::DisplayTask() const
{
	if (!g_LoadParam.bInitialized)
	{
		GetCurrentTask()->OnDisplayWhileLoading();
	}
	else
	{
		if (!m_bChangeTask)
			m_pCurrentTask->Display();
	}

	if (m_pFadeBg)
		m_pFadeBg->Draw(((int)(m_fadeAlpha * 255.0f) << 24) | 0xffffff);
}

void CTaskManager::ClearTask()
{
	if (CCapturedBg::Instance())
		CCapturedBg::Instance()->Reset();

	if (m_pCurrentTask)
	{
		m_pCurrentTask->Destroy();
		delete m_pCurrentTask;
		m_pCurrentTask = NULL;
	}

	if (HackingManager::Instance())
		HackingManager::Instance()->ClearCrypticList();
}

CTask* CTaskManager::GetCurrentTask() const
{
	return m_pCurrentTask;
}

int CTaskManager::PostMsg(const IActor* sender, const char* target, int message,
	int param1, int param2, int param3, unsigned long time)
{
	sPostMsg postMsg;
	postMsg.msg.sender = sender;
	postMsg.msg.message = message;
	postMsg.msg.param1 = param1;
	postMsg.msg.param2 = param2;
	postMsg.msg.param3 = param3;
	postMsg.msg.time = time;
	postMsg.target = target;

	m_postMsgQueue.push(postMsg);
	return 0;
}

CTaskDoc::CTaskDoc()
{
}

CTaskDoc::~CTaskDoc()
{
}

CTask* CTaskDoc::GetTask() const
{
	return m_pTask;
}

void CTaskManager::NaviOpenLayout(const char* task, const char* doc,
	const char* actor, const char* layout)
{
	switch (m_naviDirection)
	{
	case 1:
		--m_curHistory;
		break;

	case 0:
		if (IsNaviForwardEnable())
		{
			if (IsNaviForwardSameHistory(layout))
			{
				++m_curHistory;
			}
			else
			{
				m_history.erase(++m_curHistory, m_history.end());
				NaviAddHistory(task, doc, actor, layout);
			}
		}
		else
		{
			NaviAddHistory(task, doc, actor, layout);
		}
		break;
	}

	CTask* pTask = AfxGetTask();
	((CTaskMain*)pTask->GetMainActor())->CheckNaviButtons();

	m_naviDirection = 0;
}

void CTaskManager::NaviAddHistory(const char* task, const char* doc,
	const char* actor, const char* layout)
{
	history_t history;
	history.task = task;
	history.doc = doc;
	history.layout = layout;
	history.actor = actor;

	m_curHistory = m_history.insert(m_history.end(), history);
}

void CTaskManager::NaviClearHistory()
{
	m_history.clear();
}

void CTaskManager::NaviGoForward()
{
	std::list<history_t>::iterator it = m_curHistory;

	if (it != m_history.end())
	{
		if (++it != m_history.end())
		{
			history_t history = *it;

			if (AfxGetTask()->GetActor(history.actor) == NULL)
			{
				CTaskManager::Instance()->ChangeTask(history.task, history.doc,
					false);
			}

			AfxPostMsg(NULL, history.actor, 0, (int)history.layout, 1, 0, 0);
			m_naviDirection = 0;
		}
	}
}

void CTaskManager::NaviGoBackward()
{
	std::list<history_t>::iterator it = m_curHistory;

	if (it != m_history.begin())
	{
		history_t history = *--it;

		if (AfxGetTask()->GetActor(history.actor) == NULL)
		{
			CTaskManager::Instance()->ChangeTask(history.task, history.doc,
				false);
		}

		AfxPostMsg(NULL, history.actor, 0, (int)history.layout, 1, 0, 0);
		m_naviDirection = 1;
	}
}

bool CTaskManager::IsNaviForwardEnable()
{
	std::list<history_t>::iterator it = m_curHistory;

	if (++it == m_history.end())
		return false;

	return true;
}

bool CTaskManager::IsNaviBackwardEnable()
{
	if (m_curHistory == m_history.begin())
		return false;

	return true;
}

bool CTaskManager::IsNaviForwardSameHistory(const char* layout)
{
	std::list<history_t>::iterator it = m_curHistory;

	if (stricmp((*++it).layout, layout) == 0)
		return true;

	return false;
}

void CTaskManager::NaviDisplayDebugInfo()
{
	int i = 0;
	float y = 200.0f;

	for (std::list<history_t>::iterator it = m_history.begin();
		it != m_history.end(); ++it)
	{
		history_t history = *it;
		const char* mark;
		if (it == m_curHistory)
			mark = "*";
		else
			mark = "";

		TextOut(g_view, 5.0f, y, "%d: (%s)%s%s", i, history.actor,
			history.layout, mark);
		y += 15.0f;
		i++;
	}
}

void CTaskManager::AddNewItemToDoc(unsigned int type, WReceivedPacket& packet)
{
	switch (type)
	{
	case 1:
	{
		sCharacterInfo info;
		packet.DecodeBuffer(&info, sizeof(info));
		ItemManager()->GetDefCombo(info.tid, info.tidParts);

		Doc()->m_charMap.insert(
			std::map<unsigned int, sCharacterInfo>::value_type(info.guid,
				info));
	}
	break;

	case 2:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));

		if (IsLocalContent(S4_UCC) && IsUccClothes(item.tid))
		{
			Doc()->m_uccItemList.push_back(item);
		}
		else
		{
			Doc()->AddMyPartsList(item);
		}

		if (IsAngelWing(item.tid))
		{
			Doc()->CheckAngelWing();
		}

		if (IsLocalContent((localContentType_t)0x17))
		{
			if ((item.tid & 0x1800) == 0x1800)
				Doc()->SetInitHaveHalloweenItem();
		}
	}
	break;

	case 4:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));
		Doc()->m_clubSetMap.insert(
			std::map<unsigned int, sItemInfo>::value_type(item.guid, item));
	}
	break;

	case 5:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));

		ITEM_LIST_IT it;
		for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end();
			++it)
		{
			if ((*it).guid == item.guid)
			{
				(*it).Common[0] = (item.Common[0] < 1) ? 1 : item.Common[0];
				break;
			}
		}

		if (it == Doc()->m_ballList.end())
			Doc()->m_ballList.push_back(item);
	}
	break;

	case 6:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));

		if (item.ItemType)
		{
			if (item.ItemType == 3)
				item.HourRemain = item.Common[0];
			else if (item.ItemType == 4)
				item.HourRemain = item.Common[0] * 24;
			else if (item.ItemType == 5)
				item.HourRemain = item.Common[0] * 720;
		}
		else
		{
			for (ITEM_LIST_IT it = Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
			{
				if ((*it).guid == item.guid)
				{
					(*it).Common[0] = (item.Common[0] < 1) ? 1 : item.Common[0];
					(*it).HourRemain += item.HourRemain;
					return;
				}
			}
		}

		Doc()->m_myItemList.push_back(item);
	}
	break;

	case 7:
	{
		sCaddieInfo info;
		packet.DecodeBuffer(&info, sizeof(info));

		Doc()->m_caddieMap.insert(
			std::map<unsigned int, sCaddieInfo>::value_type(info.guid, info));
	}
	break;

	case 8:
	{
		unsigned long guid = 0;
		unsigned long partTid = 0;
		int date = 0;
		packet.DecodeBuffer(&guid, 4);
		packet.DecodeBuffer(&partTid, 4);
		packet.DecodeBuffer(&date, 4);

		CADDIE_MAP_IT it = Doc()->m_caddieMap.find(guid);
		if (it != Doc()->m_caddieMap.end())
		{
			sCaddieInfo& info = (*it).second;
			info.tidPart = partTid;
			info.Remain_Partdate = date;
			if (info.Remain_Date < date)
				info.Remain_Date = date;
		}
	}
	break;

	case 0xe:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));
		item.HourRemain = item.Common[0] * 24;

		for (ITEM_LIST_IT it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			if ((*it).tid == item.tid || (*it).guid == item.guid)
				return;
		}

		Doc()->m_cutinList.push_back(item);
	}
	break;

	case 0xf:
	{
		unsigned long typeId = 0;
		packet.DecodeBuffer(&typeId, 4);

		sCharacterInfo info;
		packet.DecodeBuffer(&info, sizeof(info));
		ItemManager()->GetDefCombo(info.tid, info.tidParts);

		Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];

		IFF_STRUCT::sHairStyle* pHair = ItemManager()->FindHairStyle(typeId);
		if (pHair == NULL)
			return;

		for (CHAR_MAP_IT it = Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			if ((*it).second.tid == (pHair->cCharID | 0x4000000))
			{
				(*it).second.hairClr = pHair->cHairID;
				break;
			}
		}
	}
	break;

	case 0x10:
	{
		sMascotInfo info;
		packet.DecodeBuffer(&info, sizeof(info));
		strcpy(info.szMsg, "PANGYA!");

		Doc()->AddMascot(&info);
	}
	break;

	case 0x1c:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));

		ITEM_LIST_IT it;
		for (it = Doc()->m_auxPartList.begin();
			it != Doc()->m_auxPartList.end(); ++it)
		{
			if ((*it).guid == item.guid)
			{
				(*it).Common[0] = (item.Common[0] < 1) ? 1 : item.Common[0];
				break;
			}
		}

		if (it == Doc()->m_auxPartList.end())
			Doc()->m_auxPartList.push_back(item);
	}
	break;

	case 0x1d:
	{
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(item));

		IFF_STRUCT::sQuestDrop* pQuestDrop =
			ItemManager()->FindQuestDrop(item.tid);
		if (pQuestDrop == NULL)
			return;

		ITEM_LIST_IT it;
		for (it = Doc()->m_questDropList.begin();
			it != Doc()->m_questDropList.end(); ++it)
		{
			if ((*it).guid == item.guid)
			{
				(*it).Common[0]++;
				break;
			}
		}

		if (it == Doc()->m_questDropList.end())
			Doc()->m_questDropList.push_back(item);
	}
	break;

	case 0x1f:
	{
		sCards card;
		packet.DecodeBuffer(&card, sizeof(card));

		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(card.tid);
		if (pCard == NULL)
			return;

		for (std::list<sCards>::iterator it = Doc()->m_cardList.begin();
			it != Doc()->m_cardList.end(); ++it)
		{
			if ((*it).tid == card.tid)
			{
				(*it).cardCount += (card.cardCount < 1) ? 1 : card.cardCount;
				return;
			}
		}

		Doc()->m_cardList.push_back(card);
	}
	break;
	}
}

#include "mathconsts.h"
