#include "stdafx.h"
#include "mathconsts.h"
#include "s5/utilities.h"
#include "realmyroomtask.h"
#include "guilddefine.h"
#include "guildactor.h"
#include "gatewayactor.h"
#include "realmyroom_avatar.h"
#include "realmyroommain.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frwndmanager.h"
#include "partsinfo.h"
#include "quitwindow.h"
#include "avatar_task.h"
#include "clientsetting.h"
#include "camerawork.h"
#include "itemstorage.h"
#include "transferableitem.h"
#include "mousecursor.h"
#include "frgaugebar.h"
#include "ucclibrary.h"
#include "panggiftdlg.h"

extern Fresh* g_pFresh;
void RequestUccInfo(sCharacterInfo& character);
void RequestUccInfo(sMailIncludeItem& item);

IMPLEMENT_OBJECT(CRealMyRoomTask, CTask)

CRealMyRoomTask::CRealMyRoomTask(void)
{
	m_bExit = false;
	InitializeCriticalSection(&m_cs);

	new CRealMyRoom_AvatarManager;
	new CRealMyRoomData;

	new CCameraWork;

	new CTransferableItem;

	if (IsLocalContent((localContentType_t)0x71))
	{
		new CItemStorage;
	}

	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = 0;
}

CRealMyRoomTask::~CRealMyRoomTask(void)
{
	COption::Instance()->gSetAvatarNewBie(0);

	m_bExit = true;
	m_anotherHand.WaitUntilWorkDone(9999);
	m_anotherHand.BreakWorkingThread();

	DeleteCriticalSection(&m_cs);

	if (CRealMyRoom_AvatarManager::IsInstantiated())
	{
		delete CRealMyRoom_AvatarManager::Instance();
	}

	if (CRealMyRoomData::IsInstantiated())
	{
		delete CRealMyRoomData::Instance();
	}

	if (CCameraWork::IsInstantiated())
	{
		delete CCameraWork::Instance();
	}

	if (CTransferableItem::IsInstantiated())
	{
		delete CTransferableItem::Instance();
	}

	if (IsLocalContent((localContentType_t)0x71))
	{
		if (CItemStorage::IsInstantiated())
		{
			delete CItemStorage::Instance();
		}
	}
}

void CRealMyRoomTask::Register(void)
{
	SetMainActor(AddActor("CRealMyRoomMain", "RealMyRoom", 0, ""));

	AddActor("CCamera", "Camera", 0x20, "");

	AddActor("CItemBuff", "ItemBuff", 0, "");

	AddActor(s_guildActor[0], s_guildActor[1], 0, "");

	AddActor(s_gatewayActor[0], s_gatewayActor[1], 0, GetRTTI()->GetName());

	if (!AfxGetPreservedTask())
	{
		return;
	}

	IActor* pTutorial = AfxGetPreservedTask()->GetActor("TutorialMain");
	if (pTutorial)
	{
		const char* className = 0;
		const char* name = 0;

		pTutorial << MsgObject(0, 0x14E, (int)&className, (int)&name, 0, 0, 0);
		AddActor(className, name, 0, "");
	}
}

void CRealMyRoomTask::PreLoadInit(void)
{
	sActor* pActor = m_actorList.Start();
	while (pActor)
	{
		pActor->actor->OnPreLoadInit();
		pActor = m_actorList.Next();
	}

	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0
				? ((IObject*)__rtti_obj)
					  ->IsExactKindOf(&ntAvatarChatTask::m_RTTI)
				: false))
	{
		new CQuitWindow;
	}

	SetGameType(8);

	pActor = m_actorList.Start();
	while (pActor)
	{
		pActor->actor->OnLoad();
		pActor = m_actorList.Next();
	}

	pActor = m_actorList.Start();
	while (pActor)
	{
		pActor->actor->OnInit();
		pActor = m_actorList.Next();
	}

	if (!IsInited())
	{
		m_taskFlag.Enable(2);
		OnInitFinished();
	}

	g_LoadParam.actorNum = 1;
	g_LoadParam.loadedNum = 0;

	if (m_pProgressGauge)
	{
		m_pProgressGauge->SetRange(0, 100, 0);
	}

	m_pLoadingFresh = new Fresh;
	m_pProgressGauge = NULL;

	if (m_loadingBackGround != "")
	{
		m_pLoadingFresh->Init("loading.xml", false,
			m_loadingBackGround.c_str());
	}
	else
	{
		m_pLoadingFresh->Init("loading.xml", false, NULL);
	}

	m_pLoadingFresh->OpenLayout("LOADING", NULL, true);

	if (!CMouseCursor::Instance())
	{
		return;
	}

	CMouseCursor::Instance()->SetActive(true);
	CMouseCursor::Instance()->SetMode(2);
}

void CRealMyRoomTask::Load(void)
{
	m_anotherHand.Work(this, 0, 0, false, 0);

	g_LoadParam.loadedNum = 1;
	g_LoadParam.bLoaded = true;
}

void CRealMyRoomTask::Init(const char* backGround)
{
	if (m_pLoadingFresh)
	{
		delete m_pLoadingFresh;
		m_pLoadingFresh = 0;
	}

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());

	g_pFresh->GetDesktop()->SetWallPaper(backGround, true);

	SetWhisper(false);
}

void CRealMyRoomTask::OnAnotherHand(int nWork, void* pData)
{
	std::list<CRealMyRoom_AvatarBurden> burdens;

	while (!m_bExit)
	{
		std::list<CRealMyRoom_AvatarBurden>::iterator it;

		EnterCriticalSection(&m_cs);
		for (it = m_requestList.begin(); it != m_requestList.end(); ++it)
		{
			burdens.push_back(*it);
		}
		m_requestList.clear();

		LeaveCriticalSection(&m_cs);

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			it = burdens.begin();
			while (!m_bExit && it != burdens.end())
			{
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar ==
					CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(
						(*it).m_guid))
				{
					if ((*it).LoadAvatarBurden())
					{
						EnterCriticalSection(&m_cs);
						m_completeList.push_back(*it);
						LeaveCriticalSection(&m_cs);
					}

					it = burdens.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		it = burdens.begin();
		while (!m_bExit && it != burdens.end())
		{
			if ((*it).LoadAvatarBurden())
			{
				EnterCriticalSection(&m_cs);
				m_completeList.push_back(*it);
				LeaveCriticalSection(&m_cs);

				if (!m_bExit)
				{
					Sleep(15);
				}
			}

			it = burdens.erase(it);
		}

		DeleteBurdenPet(burdens, m_deleteList);

		if (!m_bExit)
		{
			Sleep(500);
		}
	}

	DeleteBurdenPet(burdens, m_deleteList);
	DeleteBurdenPet(burdens, m_completeList);
}

void CRealMyRoomTask::Process(float dt)
{
	std::list<CRealMyRoom_AvatarBurden> burdens;
	std::list<CRealMyRoom_AvatarBurden>::iterator it;

	EnterCriticalSection(&m_cs);
	for (it = m_completeList.begin(); it != m_completeList.end(); ++it)
	{
		burdens.push_back(*it);
	}
	m_completeList.clear();
	LeaveCriticalSection(&m_cs);

	for (it = burdens.begin(); it != burdens.end(); ++it)
	{
		CRealMyRoom_AvatarBurden& burden = *it;
		CRealMyRoom_Avatar* pAvatar =
			CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(
				burden.m_guid);
		if (pAvatar)
		{
			if (pAvatar->m_pPetFrame)
			{
				pAvatar->m_pPetFrame = NULL;
				RemoveAvatarBurden(pAvatar->m_burden);
			}

			pAvatar->m_burden = burden;
			pAvatar->m_bCharInfoChanged = true;
		}
		else
		{
			RemoveAvatarBurden(burden);
		}
	}

	CTask::Process(dt);

	g_pFresh->OnProcess(dt);

	FrWnd* pFocused = g_pFresh->GetManager()->GetKeyFocused();
	if (pFocused && pFocused->IsVisible())
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
	else
	{
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}
}

void CRealMyRoomTask::RestorePreserved(void)
{
	g_pFresh->GetDesktop()->SetViewFocus(true);

	SetGameType(2);

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());
	g_pFresh->GetDesktop()->SetWallPaper(NULL, true);

	CTask::RestorePreserved();
}

void CRealMyRoomTask::Destroy(void)
{
	CTask::Destroy();

	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0
				? ((IObject*)__rtti_obj)
					  ->IsExactKindOf(&ntAvatarChatTask::m_RTTI)
				: false))
	{
		delete CQuitWindow::Instance();
	}
}

void CRealMyRoomTask::Display(void)
{
	CTask::Display();

	if (g_pFresh->GetDesktop())
	{
		g_pFresh->GetDesktop()->OnDisplay(true, false, true);
	}

	g_pFresh->OnDisplay(true);
}

void CRealMyRoomTask::AddAvatarBurden(const CRealMyRoom_AvatarBurden& burden)
{
	EnterCriticalSection(&m_cs);
	m_requestList.push_back(burden);
	LeaveCriticalSection(&m_cs);
}

void CRealMyRoomTask::RemoveAvatarBurden(const CRealMyRoom_AvatarBurden& burden)
{
	if (m_bExit)
	{
		return;
	}

	EnterCriticalSection(&m_cs);
	m_deleteList.push_back(burden);
	LeaveCriticalSection(&m_cs);
}

void CRealMyRoomTask::DeleteBurdenPet(
	std::list<CRealMyRoom_AvatarBurden>& burdens,
	std::list<CRealMyRoom_AvatarBurden>& pending)
{
	std::list<CRealMyRoom_AvatarBurden>::iterator it;

	EnterCriticalSection(&m_cs);
	for (it = pending.begin(); it != pending.end(); ++it)
	{
		burdens.push_back(*it);
	}
	pending.clear();
	LeaveCriticalSection(&m_cs);

	for (it = burdens.begin(); it != burdens.end();)
	{
		if ((*it).m_pPetFrame)
		{
			delete (*it).m_pPetFrame;
		}
		it = burdens.erase(it);
	}
}

extern int g_iRecvPacketType;

struct sPangCashResult
{
	__int64 pang;
	__int64 bonusCash;
	unsigned long result;
	unsigned long reserved14;
};

int CRealMyRoomTask::OnPacket(WReceivedPacket& packet)
{
	g_iRecvPacketType = packet.Decode2();

	switch (g_iRecvPacketType)
	{
	case 0x16E:
	{
		static sSlotInfo slot;
		static sCharacterInfo charInfo;

		std::list<sSlotInfo>& players =
			WSingleton<CSharedDoc>::Instance()->m_slotList;

		packet.DecodeBuffer(&slot, 0x152);
		packet.DecodeBuffer(&charInfo, 0x1BC);

		if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0
					? ((IObject*)__rtti_obj)
						  ->IsExactKindOf(&ntAvatarChatTask::m_RTTI)
					: false))
		{
			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_slotList.begin();
				it != WSingleton<CSharedDoc>::Instance()->m_slotList.end();
				++it)
			{
				if (sSlotInfo(*it).dwGuid == slot.dwGuid)
				{
					break;
				}
			}

			if (it == WSingleton<CSharedDoc>::Instance()->m_slotList.end())
			{
				players.push_back(slot);
			}
		}

		GetActor("RealMyRoom")
			<< MsgObject(0, 0xE4, (int)&slot, (int)&charInfo, 0, 0, 0);
		break;
	}

	case 0xB6:
	{
		unsigned char count = packet.Decode1();
		if (count >= 0x80)
		{
			count = 0x80;
		}

		unsigned long ids[0x80];
		for (unsigned char i = 0; i < count; i++)
		{
			ids[i] = packet.Decode4();
		}
		GetActor("RealMyRoom") << MsgObject(0, 0xD5, (int)ids, count, 0, 0, 0);
		break;
	}

	case 0x13D:
		if (IsLocalContent((localContentType_t)0x5B))
		{
			unsigned char value5055 = packet.Decode1();
			unsigned char value5056 = packet.Decode1();
			unsigned short count = packet.Decode2();

			for (unsigned short i = 0; i < count; i++)
			{
				sGiftInfo gift;

				packet.DecodeBuffer(&gift, 0xFB);

				WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
					gift.sMsg, false);
				WSingleton<CSharedDoc>::Instance()->m_giftList.push_back(gift);
			}

			WSingleton<CSharedDoc>::Instance()->m_cardPackSlot = value5055;
			WSingleton<CSharedDoc>::Instance()->m_cardPackItem = value5056;
			WSingleton<CSharedDoc>::Instance()->m_rmrTabValue = count;
			WSingleton<CSharedDoc>::Instance()->m_bRmrTabPending = 1;
		}
		break;

	case 0x78:
	{
		unsigned char result = packet.Decode1();
		unsigned short pageSize = packet.Decode2();
		unsigned short count = packet.Decode2();
		unsigned short total = packet.Decode2();

		if (result == 1)
		{
			WSingleton<CSharedDoc>::Instance()->m_cardPackPageNum = pageSize;
			WSingleton<CSharedDoc>::Instance()->m_cardPackSelPageNum = count;
			WSingleton<CSharedDoc>::Instance()->m_cardPackCurPage = total;

			WSingleton<CSharedDoc>::Instance()->m_giftList.clear();

			for (unsigned short i = 0; i < count; i++)
			{
				sGiftInfo gift;

				packet.DecodeBuffer(&gift, 0xFB);

				WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
					gift.sMsg, false);
				WSingleton<CSharedDoc>::Instance()->m_giftList.push_back(gift);
			}

			WSingleton<CSharedDoc>::Instance()->m_bCardPackRebuild = 1;
		}
		else
		{
			int pages = (int)total / (int)pageSize;
			if ((int)pageSize * pages < (int)total)
			{
				pages++;
			}

			if (pages)
			{
				WSendPacket request((enumClientPacket)0x92);
				request.Encode4(pages);
				request.Send(TO_GAME);
			}
			else
			{
				WSendPacket request((enumClientPacket)0x92);
				request.Encode4(1);
				request.Send(TO_GAME);
			}
		}
		break;
	}

	case 0xDA:
	{
		unsigned long result = packet.Decode4();

		GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		break;
	}

	case 0x79:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xC7, 0, 0, 0, 0, 0);

		unsigned long status = packet.Decode4();

		switch (status)
		{
		case 1:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xbc\xb1\xb9\xb0 \xbf\xc5\xb1\xe2\xb1\xe2\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 0:
		{
			unsigned long result = packet.Decode4();
			unsigned long guid = packet.Decode4();
			unsigned long typeId = packet.Decode4();

			unsigned short period = packet.Decode2();

			unsigned char flag = packet.Decode1();

			int msg;
			switch (typeId >> 26)
			{
			case 1:
				msg = 0xC8;
				break;
			case 2:
				msg = 0xCA;
				break;
			case 7:
				msg = 0xCB;
				break;
			case 8:
				msg = 0xCC;
				period *= 24;
				break;
			case 4:
				msg = 0xCF;
				break;
			case 14:
				msg = 0xD0;
				break;
			case 5:
			case 6:
			case 28:
			case 29:
				msg = 0xCE;
				break;
			case 15:
				msg = 0xC9;
				break;
			case 16:
				msg = 0xCD;
				break;
			case 31:
				msg = 0xE0;
				break;
			}

			GetActor("RealMyRoom")
				<< MsgObject(0, msg, guid, typeId, period, flag, 0);
			GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
			break;
		}

		case 3:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbb\xf3\xc7\xb0 \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 4:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 12:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb5\xf0\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 14:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 13:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc8\xde\xb0\xa1\xc1\xdf\xc0\xce \xc4\xb3\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe7\xb0\xed\xbf\xeb \xc8\xc4 \xbc\xb1\xb9\xb0 \xbf\xc5\xb1\xe2\xb1\xe2\xb8\xa6 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0);
			break;

		case 5:
		{
			unsigned long result = packet.Decode4();
			unsigned char count = packet.Decode1();
			sBuyItemResult item;

			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&item, 0x26);

				int msg;
				switch (item.Typeid >> 26)
				{
				case 1:
					msg = 0xC8;
					break;
				case 2:
					msg = 0xCA;
					break;
				case 7:
					msg = 0xCB;
					break;
				case 5:
					msg = 0xCE;
					break;
				case 4:
					msg = 0xCF;
					break;
				case 6:
					msg = 0xCE;
					break;
				case 14:
					msg = 0xD0;
					break;
				case 28:
					msg = 0xCE;
					break;
				}

				GetActor("RealMyRoom")
					<< MsgObject(0, msg, item.guid, item.Typeid, 0, 0, 0);
			}

			GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
			break;
		}

		case 6:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xbc\xb1\xb9\xb0\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 16:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb1\xe2\xb0\xa3\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 17:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb0\xb9\xbc\xf6\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		}
		break;
	}

	case 0x67:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();
		switch (result)
		{
		case 5:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb9\xb0\xb0\xc7\xc0\xbb \xc8\xaf\xba\xd2\xb9\xde\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		default:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc8\xaf\xba\xd2\xc0\xcc \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		}

		unsigned char count = packet.Decode1();
		for (int i = 0; i < count; i++)
		{
			sBuyItemResult item;
			packet.DecodeBuffer(&item, 0x26);

			int msg;
			switch (item.Typeid >> 26)
			{
			case 1:
				msg = 0xD6;
				break;
			case 2:
				msg = 0xD7;
				break;
			case 7:
				msg = 0xD8;
				break;
			case 6:
				msg = 0xD9;
				break;
			case 4:
				msg = 0xDA;
				break;
			case 5:
				msg = 0xDB;
				break;
			case 14:
				msg = 0xDC;
				break;
			default:
				continue;
			}

			GetActor("RealMyRoom")
				<< MsgObject(0, msg, item.guid, item.Typeid, 0, 0, 0);
		}

		WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang =
			packet.Decode8();

		WSingleton<CSharedDoc>::Instance()->m_bonusCash = packet.Decode8();
		WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();
		break;
	}

	case 0xA4:
	{
		sPangCashResult reply;
		packet.DecodeBuffer(&reply, 0x18);

		WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang += reply.pang;

		WSingleton<CSharedDoc>::Instance()->m_bonusCash += reply.bonusCash;
		WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();

		GetActor("RealMyRoom") << MsgObject(0, 0xD1, reply.result, 0, 0, 0, 0);
		break;
	}

	case 0xA3:
	{
		int result = packet.Decode1();

		switch (result)
		{
		case 1:
		{
			unsigned char type = packet.Decode1();
			unsigned char stat = packet.Decode1();
			unsigned long id = packet.Decode4();

			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			if (type & 1)
			{
				std::map<unsigned int, sItemInfo>::iterator it =
					pDoc->m_clubSetMap.find(id);
				if (it != pDoc->m_clubSetMap.end())
				{
					(*it).second.Common[stat]++;
				}
			}
			else
			{
				std::map<unsigned int, sCharacterInfo>::iterator it =
					pDoc->m_charMap.find(id);
				if (it != pDoc->m_charMap.end())
				{
					(*it).second.PCL[stat]++;
				}
			}

			__int64 pang = packet.Decode8();
			WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang -= pang;
			break;
		}

		case 2:
		{
			unsigned char type = packet.Decode1();
			unsigned char stat = packet.Decode1();
			unsigned long id = packet.Decode4();

			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			if (type & 1)
			{
				std::map<unsigned int, sItemInfo>::iterator it =
					pDoc->m_clubSetMap.find(id);
				if (it != pDoc->m_clubSetMap.end())
				{
					(*it).second.Common[stat]--;
				}
			}
			else
			{
				std::map<unsigned int, sCharacterInfo>::iterator it =
					pDoc->m_charMap.find(id);
				if (it != pDoc->m_charMap.end())
				{
					(*it).second.PCL[stat]--;
				}
			}

			__int64 pang = packet.Decode8();
			WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang -= pang;
			break;
		}
		}

		GetActor("RealMyRoom") << MsgObject(0, 0xD2, result, 0, 0, 0, 0);
		break;
	}

	case 0xCB:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();

		switch (result)
		{
		case 0:
		{
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();
			int third = packet.Decode2();

			GetActor("RealMyRoom")
				<< MsgObject(0, 0xDE, second, first, third, 0, 0);
			break;
		}

		case 1:
		case 2:
		case 3:
		case 4:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc1\xb6\xc7\xd5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;

		case 5:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc3\xe0\xc7\xcf\xc7\xd5\xb4\xcf\xb4\xd9. \xc0\xcc\xba\xa5\xc6\xae\xbf\xa1 \xb4\xe7\xc3\xb7\xb5\xc7\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xda\xbc\xbc\xc7\xd1 \xbb\xe7\xc7\xd7\xc0\xba \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xb8\xa6 \xc2\xfc\xc1\xb6\xc7\xcf\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			break;

		case 6:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xc0\xcc\xba\xa5\xc6\xae\xbf\xa1 \xb4\xe7\xc3\xb7\xc0\xcc \xbe\xc8\xb5\xc7\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;
		}
		break;
	}

	case 0xE0:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();
		unsigned long mascotId = packet.Decode4();

		switch (result)
		{
		case 1:
		case 2:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb8\xde\xbc\xbc\xc1\xf6 \xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;

		case 3:
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 4:
		{
			GetActor("RealMyRoom") << MsgObject(0, 0x23,
				(int)"\xb8\xde\xbc\xbc\xc1\xf6\xb0\xa1 \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);

			std::string message;
			packet.DecodeStr(message);

			WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang =
				packet.Decode8();

			sMascotInfo* pMascot =
				WSingleton<CSharedDoc>::Instance()->GetMyMascotInfo(mascotId);

			if (pMascot)
			{
				memset(pMascot->szMsg, 0, sizeof(pMascot->szMsg));
				strncpy(pMascot->szMsg, message.c_str(), 0x1D);
			}

			GetActor("RealMyRoom")
				<< MsgObject(0, 0x4E, pMascot->guid, (int)pMascot, 0, 0, 0);
			break;
		}
		}
		break;
	}

	case 0x124:
		if (IsLocalContent((localContentType_t)0x56))
		{
			unsigned long result = packet.Decode4();

			GetActor("RealMyRoom")
				<< MsgObject(0, 0x228, 0x124, result, 0, 0, 0);
		}
		break;

	case 0x125:
		if (IsLocalContent((localContentType_t)0x56))
		{
			WSingleton<CSharedDoc>::Instance()->m_furnitureList.clear();

			unsigned long result = packet.Decode4();
			if (result == 1)
			{
				int count = packet.Decode2();

				sFurniture_List furniture;
				WSingleton<CRealMyRoomData>::Instance()->m_modelList.Reset();
				for (int i = 0; i < count; i++)
				{
					memset(&furniture, 0, sizeof(furniture));

					packet.DecodeBuffer(&furniture, sizeof(furniture));

					WSingleton<CRealMyRoomData>::Instance()->AddModel(
						furniture);
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(0, 0x22F, 0, 0, 0, 0, 0));

				if (count > 0)
				{
					if (MyUID() ==
						WSingleton<CSharedDoc>::Instance()->m_replayTargetUID)
					{
						WSingleton<CRealMyRoomData>::Instance()->SaveModel(
							true);
					}
				}
			}
			else
			{
				GetActor("RealMyRoom")
					<< MsgObject(0, 0x228, 0x125, result, 0, 0, 0);
			}

			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		}
		break;

	case 0x13F:
	{
		unsigned char result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x181:
		if (NET()->IsConnected(WNetworkSystem::NET_MSN))
		{
			WSendPacket request(0x27);
			request.EncodeStr(MyId());
			request.EncodeStr(MyId());
			request.Send((eSendTo)1);

			WSingleton<CSharedDoc>::Instance()->m_giftName = "";

			WSingleton<CSharedDoc>::Instance()->m_giftIndex = -1;
		}
		break;

	case 0x13E:
	{
		if (NET()->IsConnected(WNetworkSystem::NET_MSN))
		{
			WSendPacket request(0x27);

			request.Encode4(MyUID());
			request.Encode4(WSingleton<CSharedDoc>::Instance()->m_giftIndex);

			request.Send((eSendTo)1);

			WSingleton<CSharedDoc>::Instance()->m_giftIndex = -1;

			WSingleton<CSharedDoc>::Instance()->m_giftName = "";
		}

		bool notified = false;
		int count = packet.Decode4();

		sMailIncludeItem item;

		for (int i = 0; i < count; i++)
		{
			packet.DecodeBuffer(&item, 0x35);

			if (item.iCount != 0)
			{
				if (!notified)
				{
					GetActor("RealMyRoom") << MsgObject(0, 0x242, item.dwTID,
						(int)&notified, 0, 0, 0);
				}

				switch (item.dwTID >> 26)
				{
				case 6:
				{
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_myItemList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_myItemList.end();
						++it)
					{
						if (it->guid == item.dwIDX)
						{
							if (it->tid == item.dwTID)
							{
								if (it->Common[0] > (int)item.iCount)
								{
									it->Common[0] -= (short)item.iCount;
								}
								else if (it->Common[0] == (int)item.iCount)
								{
									WSingleton<CSharedDoc>::Instance()
										->m_myItemList.erase(it);
								}
							}
							break;
						}
					}
					break;
				}

				case 5:
				{
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_ballList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_ballList.end();
						++it)
					{
						if (it->guid == item.dwIDX)
						{
							if (it->tid == item.dwTID)
							{
								if (it->Common[0] > (int)item.iCount)
								{
									it->Common[0] -= (short)item.iCount;
								}
								else if (it->Common[0] == (int)item.iCount)
								{
									WSingleton<CSharedDoc>::Instance()
										->m_ballList.erase(it);
								}
							}
							break;
						}
					}
					break;
				}

				case 2:
					if (IsLocalContent((localContentType_t)0x57) &&
						IsUccClothes(item.dwTID))
					{
						std::list<sItemInfo>::iterator it;
						for (it = WSingleton<CSharedDoc>::Instance()
								 ->m_uccItemList.begin();
							it !=
							WSingleton<CSharedDoc>::Instance()
								->m_uccItemList.end();
							++it)
						{
							if (it->guid == item.dwIDX && it->tid == item.dwTID)
							{
								WSingleton<CSharedDoc>::Instance()
									->m_uccItemList.erase(it);
								break;
							}
						}
					}
					else
					{
						std::list<sItemInfo>::iterator it;
						for (it = WSingleton<CSharedDoc>::Instance()
								 ->m_partsList.begin();
							it !=
							WSingleton<CSharedDoc>::Instance()
								->m_partsList.end();
							++it)
						{
							if (it->guid == item.dwIDX)
							{
								if (it->tid == item.dwTID)
								{
									WSingleton<CSharedDoc>::Instance()
										->m_partsList.erase(it);
								}
								break;
							}
						}
					}
					break;

				case 4:
				{
					std::map<unsigned int, sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_clubSetMap.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_clubSetMap.end();
						++it)
					{
						if ((*it).second.guid == item.dwIDX)
						{
							if ((*it).second.tid == item.dwTID)
							{
								WSingleton<CSharedDoc>::Instance()
									->m_clubSetMap.erase(it);
							}
							break;
						}
					}
					break;
				}

				case 0x1C:
				{
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_auxPartList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_auxPartList.end();
						++it)
					{
						if (it->guid == item.dwIDX)
						{
							if (it->tid == item.dwTID)
							{
								if (it->Common[0] > (int)item.iCount)
								{
									it->Common[0] -= (short)item.iCount;
								}
								else if (it->Common[0] == (int)item.iCount)
								{
									WSingleton<CSharedDoc>::Instance()
										->m_auxPartList.erase(it);
								}
							}
							break;
						}
					}
					break;
				}

				case 7:
				{
					std::map<unsigned int, sCaddieInfo>& caddies =
						WSingleton<CSharedDoc>::Instance()->m_caddieMap;
					std::map<unsigned int, sCaddieInfo>::iterator it =
						caddies.find(item.dwIDX);
					if (it !=
						WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
					{
						caddies.erase(it);
					}
					break;
				}

				case 0x1F:
				{
					std::list<sCards>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_cardList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_cardList.end();
						it++)
					{
						if ((*it).tid == item.dwTID)
						{
							(*it).cardCount -= item.iCount;
							if ((*it).cardCount < 1)
							{
								WSingleton<CSharedDoc>::Instance()
									->m_cardList.erase(it);
							}
							break;
						}
					}
					break;
				}
				}
			}
		}

		if (notified)
		{
			GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
		}

		GetActor("RealMyRoom") << MsgObject(0, 0x225, 0x13E, 0, 0, 0, 0);
		break;
	}

	case 0x141:
	{
		int result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x140:
	{
		static sMailBoxListForPage mailPage;
		static sMailInfoBrief mailBrief;

		mailPage.totalPage = packet.Decode4();
		mailPage.curPage = packet.Decode4();
		mailPage.mailCount = packet.Decode4();
		mailPage.mailList.clear();

		for (int i = 0; i < mailPage.mailCount; i++)
		{
			memset(&mailBrief, 0x30, 0xC2);

			packet.DecodeBuffer(&mailBrief, 0xC2);
			mailPage.mailList.push_back(mailBrief);
			if (IsLocalContent((localContentType_t)0x57))
			{
				RequestUccInfo(mailBrief.item);
			}
		}

		WSingleton<CTransferableItem>::Instance()->SetMailInfoPage(mailPage);

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x223, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x143:
	{
		int result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x142:
	{
		static sMailInfo mailInfo;
		static sMailIncludeItem includeItem;

		mailInfo.dwIndex = packet.Decode4();

		strncpy(mailInfo.szFromNick, packet.DecodeStr().c_str(), 0x16);
		strncpy(mailInfo.szDate, packet.DecodeStr().c_str(), 0x20);
		strncpy(mailInfo.szContent, packet.DecodeStr().c_str(), 0x320);

		mailInfo.bHaveItem = packet.Decode1() == 1;

		mailInfo.m_lMailItemList.clear();
		if (mailInfo.bHaveItem)
		{
			int count = packet.Decode4();

			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&includeItem, 0x35);
				mailInfo.m_lMailItemList.push_back(includeItem);
				if (IsLocalContent((localContentType_t)0x57))
				{
					RequestUccInfo(includeItem);
				}
			}
		}

		WSingleton<CTransferableItem>::Instance()->SetMailInfo(mailInfo);

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x224, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x15C:
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x226, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;

	case 0x15D:
	{
		unsigned long result = packet.Decode4();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x15D, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x145:
	{
		int result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x145, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x144:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);

		int count = packet.Decode4();

		sMailIncludeItem item;

		for (int i = 0; i < count; i++)
		{
			packet.DecodeBuffer(&item, 0x35);
			int num = (int)item.iCount < 1 ? 1 : item.iCount;

			unsigned long typeId = item.dwTID;
			unsigned long guid = item.dwIDX;
			unsigned char kind = item.btItemType;
			unsigned long flag = item.iTimeCount;

			if (typeId == 0x1A000010)
			{
				continue;
			}

			switch (typeId >> 26)
			{
			case 1:
				continue;

			case 2:
				if (IsLocalContent((localContentType_t)0x57) &&
					IsUccClothes(typeId))
				{
					WSendPacket request((enumClientPacket)0xB1);
					request.Encode1(1);
					request.Encode4(guid);
					request.Encode1(1);
					request.Send((eSendTo)0);
				}
				else
				{
					sItemInfo info;
					memset(&info, 0, sizeof(info));
					info.guid = guid;
					info.tid = typeId;
					info.ItemType = kind;
					info.HourRemain = (int)flag / 60 / 24;
					info.IsValid = 1;

					WSingleton<CSharedDoc>::Instance()->AddMyPartsList(info);
				}
				break;

			case 7:
			{
				sCaddieInfo caddie;
				memset(&caddie, 0, sizeof(caddie));
				caddie.guid = guid;
				caddie.tid = typeId;

				if (typeId == 0x1C000001 || typeId == 0x1C000002 ||
					typeId == 0x1C000003 || typeId == 0x1C000007)
				{
					caddie.Remain_Date = (int)flag / 60 / 24;
					caddie.Rent_flag = 1;
				}
				else
				{
					caddie.Remain_Date = 0;
					caddie.Rent_flag = 0;
				}

				if (typeId >= 0x1C00000C && typeId <= 0x1C00000E)
				{
					caddie.Level = kind;
					caddie.Exp = flag;
				}

				unsigned short days = caddie.Remain_Date;
				std::map<unsigned int, sCaddieInfo>::iterator it =
					WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(guid);
				if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
				{
					it->second.Remain_Date = days;
					break;
				}

				WSingleton<CSharedDoc>::Instance()->m_caddieMap.insert(
					std::map<unsigned int, sCaddieInfo>::value_type(guid,
						caddie));
				break;
			}

			case 8:
			{
				int hours = (int)flag / 60;
				std::map<unsigned int, sCaddieInfo>::iterator it =
					WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(guid);
				if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
				{
					break;
				}

				sCaddieInfo& caddie = it->second;
				caddie.tidPart = typeId;
				caddie.Remain_Partdate = hours;
				if (caddie.Remain_Date < hours)
				{
					caddie.Remain_Date = hours;
				}
				break;
			}

			case 6:
			{
				unsigned long tid = item.dwTID;
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.guid = guid;
				info.tid = tid;
				info.ItemType = kind;
				info.Common[0] = num;
				info.IsValid = 1;

				if (info.ItemType)
				{
					if (info.ItemType == 3)
					{
						info.HourRemain = flag;
					}
					else if (info.ItemType == 4)
					{
						info.HourRemain = flag * 24;
					}
					else if (info.ItemType == 5)
					{
						info.HourRemain = flag * 24 * 30;
					}
					else if (info.ItemType == 2)
					{
						info.HourRemain = (int)flag / 60;
					}
				}

				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()
						 ->m_myItemList.begin();
					it !=
					WSingleton<CSharedDoc>::Instance()->m_myItemList.end();
					++it)
				{
					if (it->guid == guid)
					{
						it->Common[0] = num;
						break;
					}
				}

				if (it ==
					WSingleton<CSharedDoc>::Instance()->m_myItemList.end())
				{
					if (IsLocalContent((localContentType_t)0x14))
					{
						if (tid == 0x1A000042)
						{
							info.Common[1] = packet.Decode2();
							info.Common[2] = packet.Decode2();

							info.HourRemain = 0xA8;

							SYSTEMTIME endTime;
							packet.DecodeBuffer(&endTime, sizeof(endTime));
							memcpy(&info.ItemDate, &endTime, sizeof(endTime));
							info.ItemType = 2;
						}
					}

					WSingleton<CSharedDoc>::Instance()->m_myItemList.push_back(
						info);
				}
				break;
			}

			case 5:
			{
				std::list<sItemInfo>::iterator it;
				for (it =
						 WSingleton<CSharedDoc>::Instance()->m_ballList.begin();
					it != WSingleton<CSharedDoc>::Instance()->m_ballList.end();
					++it)
				{
					if (it->guid == guid)
					{
						it->Common[0] = num;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_ballList.end())
				{
					sItemInfo info;
					memset(&info, 0, sizeof(info));
					info.guid = guid;
					info.tid = typeId;
					info.Common[0] = num;
					info.ItemType = 0;
					info.IsValid = 1;

					WSingleton<CSharedDoc>::Instance()->m_ballList.push_back(
						info);
				}
				break;
			}

			case 3:
			case 4:
			{
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.guid = item.dwIDX;
				info.tid = typeId;
				info.IsValid = 1;
				memcpy(info.Common, item.soCom, sizeof(item.soCom));

				CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
				// HACK! Otherwise pair ctor gets inlined...
#pragma inline_depth(0)
				doc->m_clubSetMap.insert(
					std::map<unsigned int, sItemInfo>::value_type(item.dwIDX,
						info));
#pragma inline_depth()
				break;
			}

			case 29:
			{
				IFF_STRUCT::sQuestDrop* pDrop =
					ItemManager()->FindQuestDrop(typeId);
				if (pDrop)
				{
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_questDropList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()
							->m_questDropList.end();
						++it)
					{
						if (it->guid == guid)
						{
							it->Common[0]++;
							break;
						}
					}

					if (it ==
						WSingleton<CSharedDoc>::Instance()
							->m_questDropList.end())
					{
						sItemInfo info;
						memset(&info, 0, sizeof(info));
						info.guid = guid;
						info.tid = typeId;
						info.Common[0] = 1;

						WSingleton<CSharedDoc>::Instance()
							->m_questDropList.push_back(info);
					}
				}
				break;
			}

			case 16:
			{
				sMascotInfo mascot;
				memset(&mascot, 0, sizeof(mascot));
				mascot.guid = guid;
				strcpy(mascot.szMsg, "PANGYA!");
				mascot.tid = typeId;
				int hours = (int)flag / 60;
				mascot.Remain_Date = hours;

				SYSTEMTIME now;
				GetLocalTime(&now);
				FILETIME fileTime;
				SystemTimeToFileTime(&now, &fileTime);

				*(__int64*)&fileTime += hours * 36000000000i64;

				FileTimeToSystemTime(&fileTime, &now);

				mascot.endDate = now;

				WSingleton<CSharedDoc>::Instance()->AddMascot(&mascot);
				break;
			}

			case 28:
			{
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.guid = guid;
				info.tid = typeId;
				info.ItemType = kind;
				info.Common[0] = num;
				info.IsValid = 1;

				if (info.ItemType)
				{
					if (info.ItemType == 3)
					{
						info.HourRemain = flag;
					}
					else if (info.ItemType == 4)
					{
						info.HourRemain = flag * 24;
					}
					else if (info.ItemType == 5)
					{
						info.HourRemain = flag * 24 * 30;
					}
					else if (info.ItemType == 2)
					{
						info.HourRemain = (int)flag / 60;
					}
				}

				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()
						 ->m_auxPartList.begin();
					it !=
					WSingleton<CSharedDoc>::Instance()->m_auxPartList.end();
					++it)
				{
					if (it->guid == guid)
					{
						it->Common[0] = num;
						break;
					}
				}

				if (it ==
					WSingleton<CSharedDoc>::Instance()->m_auxPartList.end())
				{
					WSingleton<CSharedDoc>::Instance()->m_auxPartList.push_back(
						info);
				}
				break;
			}

			case 31:
			{
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
				if (pCard)
				{
					sCards card;
					memset(&card, 0, sizeof(card));
					card.uid = guid;
					card.tid = typeId;
					card.cardCount = num;
					card.valid = 1;

					bool found = false;
					std::list<sCards>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()
							 ->m_cardList.begin();
						it !=
						WSingleton<CSharedDoc>::Instance()->m_cardList.end();
						++it)
					{
						if ((*it).tid == typeId)
						{
							(*it).cardCount = num;
							found = true;
							break;
						}
					}

					if (!found)
					{
						WSingleton<CSharedDoc>::Instance()
							->m_cardList.push_back(card);
					}
				}
				break;
			}

			case 14:
			{
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.guid = guid;
				info.tid = typeId;
				info.ItemType = kind;
				info.IsValid = 1;
				info.HourRemain = flag * 24;

				WSingleton<CSharedDoc>::Instance()->m_cutinList.push_back(info);
				break;
			}

			case 18:
			{
				sFurniture_List furniture;
				IFF_STRUCT::sFurniture* pFurniture =
					ItemManager()->FindFurniture(typeId);
				if (pFurniture)
				{
					memset(&furniture, 0, sizeof(furniture));

					furniture.id = guid;
					furniture.typeId = typeId;
					furniture.x = pFurniture->Pos[0];
					furniture.y = pFurniture->Pos[1];
					furniture.z = pFurniture->Pos[2];
					furniture.r = pFurniture->Pos[3];
					furniture.bArrange = 0;

					WSingleton<CSharedDoc>::Instance()->AddFurniture(furniture);
				}
				break;
			}
			}
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
			(int)"\xc6\xed\xc1\xf6\xbf\xa1 \xc6\xf7\xc7\xd4\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xb8\xb7\xce \xc0\xcc\xb5\xbf\xbd\xc3\xc4\xd7\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));

		GetActor("RealMyRoom") << MsgObject(0, 0x226, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
		break;
	}

	case 0x116:
		if (IsLocalContent((localContentType_t)0x14))
		{
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			WSingleton<CSharedDoc>::Instance()->m_caddieReportList.clear();

			long count = packet.Decode4();
			SYSTEMTIME reportTime;
			packet.DecodeBuffer(&reportTime, sizeof(reportTime));

			if (count == 0)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
					(int)"\xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae \xbf\xad\xb6\xf7\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. ",
					0, 0, 0, 0));
				break;
			}
			else if (count == -2)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
					(int)"\xb5\xa5\xc0\xcc\xc5\xb8\xb0\xa1 \xc0\xaf\xc8\xbf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae \xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}
			else if (count == -1)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
					(int)"\xb3\xaf\xc2\xa5\xb0\xa1 \xc0\xaf\xc8\xbf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae \xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
				break;
			}

			if (IsLocalContent((localContentType_t)0x58))
			{
				WSingleton<CSharedDoc>::Instance()->m_bTreasureResult = 1;
			}

			unsigned long serial = 0;
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(0, 0x204, (int)&serial, 0, 0, 0, 0);
			std::list<sItemInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin();
				it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end();
				++it)
			{
				if (it->guid == serial)
				{
					break;
				}
			}

			if (it == WSingleton<CSharedDoc>::Instance()->m_myItemList.end())
			{
				return 0;
			}

			for (int i = 0; i < count; ++i)
			{
				--it->Common[0];

				if (it->Common[0] <= 0)
				{
					WSingleton<CSharedDoc>::Instance()->m_myItemList.erase(it);
					GetActor("RealMyRoom") << MsgObject(0, 0xDD, 0, 0, 0, 0, 0);
					break;
				}
			}

			for (int j = 0; j < count; ++j)
			{
				sCaddieReportData report;
				packet.DecodeBuffer(&report, sizeof(report));
				WSingleton<CSharedDoc>::Instance()
					->m_caddieReportList.push_back(report);
			}

			WSingleton<CSharedDoc>::Instance()->BuildTikiReportList();

			GetActor("RealMyRoom")
				<< MsgObject(0, 0x205, (int)&reportTime, 0, 0, 0, 0);
		}
		break;

	case 0x17A:
	case 0x17C:
		break;

	case 0x121:
	{
		unsigned char result = packet.Decode1();

		switch (result)
		{
		case 0:
		{
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			std::list<sPouchPrize> prizes;
			unsigned long itemId = packet.Decode4();
			int count = packet.Decode4();

			std::list<sItemInfo>::iterator it = std::find(
				WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(),
				WSingleton<CSharedDoc>::Instance()->m_myItemList.end(), itemId);
			if (it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end())
			{
				it->Common[0]--;
				if (it->Common[0] < 1)
				{
					WSingleton<CSharedDoc>::Instance()->m_myItemList.erase(it);
				}
			}

			std::string message;

			char text[64] = { 0 };
			sPouchPrize prize;
			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&prize, 0x14);
				IFF_ITEM_COMMON* common =
					ItemManager()->FindCommonItem(prize.dwTid);
			}

			switch (prize.dwTid >> 26)
			{
			case 5:
				sprintf(text,
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb %d \xb0\xb3 \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
					prize.iCount);
				break;

			case 6:
				if (prize.dwTid == 0x1A000010)
				{
					sprintf(text,
						"%d \xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
						prize.iCount);
					WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.i64Pang +=
						prize.iCount;
				}
				else
				{
					sprintf(text,
						"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb %d \xb0\xb3 \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
						prize.iCount);
				}
				break;

			default:
				sprintf(text,
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
				break;
			}

			message = text;
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x26, (int)message.c_str(), prize.dwTid, 0, 0, 0));
			GetActor("RealMyRoom") << MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);
			break;
		}

		case 1:
		{
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			unsigned long error = packet.Decode4();

			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			std::string message =
				"\xc1\xa4\xc0\xc7\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xbf\xc0\xb7\xf9.";

			switch (error)
			{
			case 0:
				message =
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc4\xda\xb5\xe5 \xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xfc\xb8\xae\xc0\xda\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7\xc7\xcf\xbc\xbc\xbf\xe4.";
				break;

			case 1:
				message =
					"\xbc\xad\xb9\xf6\xc0\xc7 \xbf\xe4\xc3\xbb\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.";
				break;

			case 2:
				message =
					"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6\xb7\xae\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.\n \xc0\xe7\xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.";
				break;

			case 3:
				message =
					"\xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";
				break;
			}

			const char* messageText = message.c_str();
			pForm->SetMessage(messageText, false);
			pForm->Open((FRESH_PFN_RESULT)0, 3);
			break;
		}
		}
		break;
	}

	case 0x1A2:
	{
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned long result = packet.Decode4();
		unsigned long itemId = packet.Decode4();
		unsigned long typeId = packet.Decode4();
		unsigned long quantity = packet.Decode4();

		const char* failTitle =
			"\xbb\xf3\xc0\xda \xbf\xad\xb1\xe2\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";

		char text[256] = "";

		switch (result)
		{
		case 0:
			if (!ItemManager()->FindCommonItem(typeId))
			{
				char wrongType[64] = "";
				strcat(wrongType,
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc5\xb8\xc0\xd4ID \xc0\xd4\xb4\xcf\xb4\xd9.");

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(0, 0x23, (int)wrongType, 0, 0, 0, 0));
				GetActor("RealMyRoom")
					<< MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);
				break;
			}

			switch (typeId >> 26)
			{
			case 5:
				sprintf(text,
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb %d \xb0\xb3 \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
					quantity);
				break;

			case 6:
				if (typeId == 0x1A000010)
				{
					sprintf(text,
						"%d \xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
						quantity);
				}
				else if (quantity > 1)
				{
					sprintf(text,
						"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb %d \xb0\xb3 \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
						quantity);
				}
				else
				{
					sprintf(text,
						"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
				}
				break;

			default:
				sprintf(text,
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
				break;
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x26, (int)text, typeId, 0, 0, 0));
			GetActor("RealMyRoom") << MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x60))
			{
				WSingleton<CSharedDoc>::Instance()->m_mailList.clear();
				WSendPacket request(0x15E);
				request.Send(TO_GAME);
			}
			else
			{
				WSingleton<CSharedDoc>::Instance()->m_giftList.clear();
				WSendPacket request((enumClientPacket)0x93);
				request.Send(TO_GAME);
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23,
				(int)"\xbb\xf3\xc7\xb0\xc0\xcc \xbf\xec\xc6\xed\xc0\xb8\xb7\xce \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbf\xec\xc6\xed\xc7\xd4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0));
			break;

		case 4:
			strcpy(text, failTitle);
			strcat(text,
				"\x0a\xbc\xf6\xb7\xae\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x23, (int)text, 0, 0, 0, 0));
			break;

		case 8:
			strcpy(text, failTitle);
			strcat(text,
				"\x0a\xbe\xc6\xc0\xcc\xc5\xdb \xc1\xbe\xb7\xf9 \xbf\xa1\xb7\xaf\xc0\xd4\xb4\xcf\xb4\xd9.");

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x23, (int)text, 0, 0, 0, 0));
			break;

		case 0x200B3B:
		case 0x200B3C:
		case 0x200B3D:
		{
			IActor* pActor = GetActor(s_gatewayActor[1]);
			pActor << MsgObject(0, 0x276, 1, result, 0, 0, 0);
			break;
		}

		default:
			strcpy(text, failTitle);
			strcat(text, "\n");
			strcat(text, MakeStr("Error : %d", result));

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x23, (int)text, 0, 0, 0, 0));
			break;
		}
		break;
	}

	case 0x175:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			unsigned long result = packet.Decode4();
			unsigned long flags = packet.Decode4();

			if (result == 0)
			{
				if (flags & 2)
				{
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(0, 0x236, 0, 0, 0, 0, 0);
					WSingleton<CSharedDoc>::Instance()->m_bItemStoragePin =
						true;
				}
				else if ((flags & 4) && (flags & 8))
				{
					if (!WSingleton<CSharedDoc>::Instance()->m_bItemStoragePin)
					{
						WSingleton<CSharedDoc>::Instance()->m_bItemStoragePin =
							true;
					}
					else
					{
						AfxGetTask()->GetActor("RealMyRoom")
							<< MsgObject(0, 0x235, 0, 0, 0, 0, 0);
					}

					WSingleton<CSharedDoc>::Instance()->m_bItemStorageLock =
						true;
				}
				else if ((flags & 4) && (flags & 0x10))
				{
					if (!WSingleton<CSharedDoc>::Instance()->m_bItemStoragePin)
					{
						WSingleton<CSharedDoc>::Instance()->m_bItemStoragePin =
							true;
					}
					else
					{
						WSendPacket request((enumClientPacket)0xCE);
						request.EncodeStr("\0");
						request.Send(TO_GAME);
					}

					WSingleton<CSharedDoc>::Instance()->m_bItemStorageLock =
						false;
				}
			}
			else
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23F, 0x175, result, 0, 0, 0);
			}

			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage)
			{
				pStorage->UnLock();
			}
		}
		break;

	case 0x17B:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			unsigned long result = packet.Decode4();

			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(0, 0x23F, 0x17B, result, 0, 0, 0);

			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage)
			{
				pStorage->UnLock();
			}
		}
		break;

	case 0x171:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			unsigned long result = packet.Decode4();

			if (result != 0)
			{
				if (result == 0x66)
				{
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(0, 0x234, 0, 0, 0, 0, 0);
				}
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23F, 0x171, result, 0, 0, 0);
			}
			else
			{
				WSendPacket request((enumClientPacket)0xD7);
				request.Send(TO_GAME);

				WSendPacket pageRequest((enumClientPacket)0xCF);
				pageRequest.Encode4(0x63);
				pageRequest.Encode2(1);
				pageRequest.Send(TO_GAME);
			}

			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage)
			{
				pStorage->UnLock();
			}
		}
		break;

	case 0x172:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();

			if (pStorage)
			{
				pStorage->ClearStoredItemInfo();

				unsigned long totalPage = packet.Decode2();
				unsigned long curPage = packet.Decode2();
				unsigned int count = packet.Decode1();

				for (unsigned int i = 0; i < count; i++)
				{
					sStoredItemInfo info;
					memset(&info, 0, sizeof(info));

					packet.DecodeBuffer(&info, sizeof(info));

					pStorage->AddStoredItemInfo(info);
				}

				pStorage->SetTotalPage(totalPage);
				pStorage->SetCurPage(curPage);

				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23A, 0, 0, 0, 0, 0);

				pStorage->UnLock();
			}
		}
		break;

	case 0x174:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			unsigned long result = packet.Decode4();

			if (result == 0)
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23D, result, result, 0, 0, 0);
			}
			else
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23F, 0x174, result, 0, 0, 0);
			}

			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage)
			{
				pStorage->UnLock();
			}
		}
		break;

	case 0x173:
		if (IsLocalContent((localContentType_t)0x71) == true)
		{
			unsigned long result = packet.Decode4();

			if (result == 0)
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23E, result, result, 0, 0, 0);
			}
			else
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(0, 0x23F, 0x173, result, 0, 0, 0);
			}

			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage)
			{
				pStorage->UnLock();
			}
		}
		break;

	case 0x176:
	{
		unsigned long result = packet.Decode4();

		if (result != 0)
		{
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(0, 0x23F, 0x176, result, 0, 0, 0);
		}

		CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage)
		{
			pStorage->UnLock();
		}
		break;
	}

	case 0x177:
	{
		CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage)
		{
			pStorage->SetStoredPang(packet.Decode8());

			pStorage->UnLock();
		}
		break;
	}

	case 0x178:
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		std::string message;

		unsigned long result = packet.Decode4();

		switch (result)
		{
		case 0:
		{
			message =
				"\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed \xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9";
			unsigned char lock = packet.Decode1();
			WSingleton<CSharedDoc>::Instance()->m_bItemStorageLock =
				(lock == 1);
			break;
		}

		case 0x65:
			message =
				"\xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9";
			break;

		case 0x64:
			message =
				"DataBase\xc1\xf6\xbf\xac \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;

		case 0x75:
			message =
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;
		}

		const char* messageText = message.c_str();
		pForm->SetMessage(messageText, false);
		pForm->Open((FRESH_PFN_RESULT)0, 3);
		break;
	}

	case 0x179:
	{
		unsigned long result = packet.Decode4();

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		switch (result)
		{
		case 0:
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 0x65:
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 0x64:
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 0x68:
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xc0\xc7 \xb1\xe6\xc0\xcc\xb0\xa1 \xc0\xdf\xb8\xf8 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9..",
				false);
			break;
		case 0x72:
			pForm->SetMessage(
				"'\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed'\xc0\xc7 \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xbc\xb3\xc1\xa4 \xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case 0x75:
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		}

		pForm->Open((FRESH_PFN_RESULT)0, 3);
		break;
	}

	case 0x184:
		if (IsLocalContent((localContentType_t)0x76))
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			switch (packet.Decode1())
			{
			case 1:
				pForm->SetMessage(
					"\xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 2:
				pForm->SetMessage(
					"\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xb0\xa1\xb1\xb8\xb8\xa6 \xb4\xf5 \xc0\xcc\xbb\xf3 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 5:
				pForm->SetMessage(
					"\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb4\xf5 \xc0\xcc\xbb\xf3 \xbe\xf2\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 0:
				pForm->SetMessage(
					"\xc0\xcc \xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc3\xbd\xba\xc5\xdb \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 3:
				pForm->SetMessage(
					"\xb9\xe8\xc4\xa1\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xb9\xe8\xc4\xa1\xc7\xd1 \xb4\xd9\xc0\xbd \xc0\xfa\xc0\xe5\xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc0\xfa\xc0\xe5\xc7\xcf\xbd\xc5 \xc8\xc4\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 4:
				pForm->SetMessage(
					"\xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xb4\xc2 \xb5\xa5\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 6:
				pForm->SetMessage(
					"\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
				break;
			}

			pForm->Open((FRESH_PFN_RESULT)0, 3);
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(
				"\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
				false);
			pForm->Open((FRESH_PFN_RESULT)0, 3);
		}
		break;

	case 0x183:
		if (IsLocalContent((localContentType_t)0x76))
		{
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();

			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(0, 0x245, first, second, 0, 0, 0);
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(
				"\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
				false);
			pForm->Open((FRESH_PFN_RESULT)0, 3);
		}
		break;

	case 0x194:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0)
		{
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo>* pList = (typeId & 0xFC000000) == 0x08000000
				? &WSingleton<CSharedDoc>::Instance()->m_partsList
				: &WSingleton<CSharedDoc>::Instance()->m_myItemList;
			std::list<sItemInfo>::iterator it =
				std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end())
			{
				it->Expired = 0;
				it->HourRemain = 0xA8;
				pForm->SetMessage(
					"\xb4\xeb\xbf\xa9 \xb1\xe2\xb0\xa3\xc0\xcc \xbf\xac\xc0\xe5 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			}
			else
			{
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			}
		}
		else
		{
			switch (result)
			{
			case 1:
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 2:
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 3:
				pForm->SetMessage(
					"\xb1\xe2\xb0\xa3\xc0\xcc \xb8\xb8\xb7\xe1\xb5\xc8 \xc0\xc7\xbb\xf3\xb8\xb8 \xb1\xe2\xb0\xa3\xc0\xbb \xbf\xac\xc0\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 5:
				pForm->SetMessage(
					"\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			default:
				pForm->SetMessage(
					MakeStr(
						"\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)",
						result),
					false);
				break;
			}
		}

		pForm->Open((FRESH_PFN_RESULT)0, 3);
		break;
	}

	case 0x195:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0)
		{
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo>* pList = (typeId & 0xFC000000) == 0x08000000
				? &WSingleton<CSharedDoc>::Instance()->m_partsList
				: &WSingleton<CSharedDoc>::Instance()->m_myItemList;
			std::list<sItemInfo>::iterator it =
				std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end())
			{
				pList->erase(it);
				pForm->SetMessage(
					"\xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe8\xc1\xa6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);

				GetMainActor() << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
			}
			else
			{
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			}
		}
		else
		{
			switch (result)
			{
			case 1:
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 2:
				pForm->SetMessage(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 4:
				pForm->SetMessage(
					"\xc4\xab\xb5\xe5\xb0\xa1 \xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xbb\xe8\xc1\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 5:
				pForm->SetMessage(
					"\xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe8\xc1\xa6 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			default:
				pForm->SetMessage(
					MakeStr(
						"\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)",
						result),
					false);
				break;
			}
		}

		pForm->Open((FRESH_PFN_RESULT)0, 3);
		break;
	}

	case 0x1B3:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 0x1A, 0x407, 0, 0, 0);
		break;
	}

	case 0x1BD:
	{
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GUILD_USER_INFO info;
		packet.DecodeBuffer(&info, sizeof(info));
		GetActor(s_guildActor[1])
			<< MsgObject(0, 0x263, 5, (int)&info, 0, 0, 0);
		break;
	}

	case 0x1B4:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = AfxGetTask()->GetActor(s_guildActor[1]);
		pActor << MsgObject(0, 0x263, 0x1A, 0x405, 0, 0, 0);

		std::string name;
		packet.DecodeStr(name);
		pActor << MsgObject(0, 0x263, 3, (int)&name, 0, 0, 0);
		break;
	}

	case 0x1B5:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);
		pActor << MsgObject(0, 0x263, 0x1A, 0x40F, 0, 0, 0);
		pActor << MsgObject(0, 0x263, 0x19, 0, 0, 0, 0);
		break;
	}

	case 0x1C7:
	{
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		unsigned long guildNumber = packet.Decode4();
		std::string name;
		packet.DecodeStr(name);

		GetActor(s_guildActor[1])
			<< MsgObject(0, 0x263, 0x15, guildNumber, (int)&name, 0, 0);
		break;
	}

	case 0x1C8:
	{
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 0x1A, 0x41D, 0, 0, 0);
		break;
	}

	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
