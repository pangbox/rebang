#include "stdafx.h"
#include "mathconsts.h"
#include "avatar_task.h"
#include "ntavatar.h"
#include "avatar_avatar.h"
#include "quitwindow.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frwndmanager.h"
#include "partsinfo.h"
#include "mousecursor.h"
#include "frgaugebar.h"
#include "ucclibrary.h"
#include "golfdoc.h"
#include "clientsetting.h"

extern Fresh* g_pFresh;

static const char* s_gatewayClass = "CGatewayActor";
static const char* s_gatewayName = "Gateway";

extern int g_iRecvPacketType;

extern bool g_SALoadingState;

IMPLEMENT_OBJECT(ntAvatarChatTask, CTask);

ntAvatarChatTask::ntAvatarChatTask(void)
{
	m_bLobbyScreenSize = false;
	m_destroying = false;
	InitializeCriticalSection(&m_burdenLock);

	new ntAvatarManager;
}

ntAvatarChatTask::~ntAvatarChatTask(void)
{
	COption::Instance()->gSetAvatarNewBie(0);

	COption::Instance()->vSetMinimumScreenSize(0, 800, 600);

	m_destroying = true;
	m_anotherHand.WaitUntilWorkDone(9999);
	m_anotherHand.BreakWorkingThread();

	DeleteCriticalSection(&m_burdenLock);

	if (ntAvatarManager::IsInstantiated())
	{
		delete ntAvatarManager::Instance();
	}
}

void ntAvatarChatTask::PreLoadInit(void)
{
	sActor* pActor = m_actorList.Start();
	while (pActor)
	{
		pActor->actor->OnPreLoadInit();
		pActor = m_actorList.Next();
	}

	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = 0;
	SetLoadingBackGround("loading_background.jpg");

	CChatMsg::Instance()->ResetUiPosition();

	new CQuitWindow;

	SetGameType(2);

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

	if (m_bTip)
	{
		m_pTipForm = LoadTipText(0, 0);
		if (m_pTipForm)
		{
			m_pProgressGauge = DYNAMIC_CAST(FrGaugeBar,
				m_pTipForm->FindChildByName("progress"));
			if (m_pProgressGauge)
			{
				m_pProgressGauge->SetBarSpeed(200.0f);
			}
		}
	}

	if (CMouseCursor::Instance())
	{
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}
}

void ntAvatarChatTask::Load(void)
{
	m_anotherHand.Work(this, 0, 0, false, 0);
}

void ntAvatarChatTask::Init(const char* backGround)
{
	if (m_pLoadingFresh)
	{
		delete m_pLoadingFresh;
		m_pLoadingFresh = 0;
	}

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());

	g_pFresh->GetDesktop()->SetWallPaper(backGround, true);

	CChatMsg::Instance()->SetActive(true, true, true);
}

void ntAvatarChatTask::AddAvatarBurden(const ntAvatarBurden& burden)
{
	EnterCriticalSection(&m_burdenLock);
	m_addList.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

void ntAvatarChatTask::RemoveAvatarBurden(const ntAvatarBurden& burden)
{
	if (m_destroying)
	{
		return;
	}

	EnterCriticalSection(&m_burdenLock);
	m_removeList.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

void ntAvatarChatTask::DeleteBurdenPet(std::list<ntAvatarBurden>& burdens,
	std::list<ntAvatarBurden>& pending)
{
	EnterCriticalSection(&m_burdenLock);
	for (std::list<ntAvatarBurden>::iterator pendingIt = pending.begin();
		pendingIt != pending.end(); ++pendingIt)
	{
		burdens.push_back(*pendingIt);
	}
	pending.clear();
	LeaveCriticalSection(&m_burdenLock);

	std::list<ntAvatarBurden>::iterator it = burdens.begin();
	while (it != burdens.end())
	{
		CPetFrame* pPetFrame = (*it).m_pPetFrame;
		if (pPetFrame != NULL && pPetFrame != (CPetFrame*)-1)
		{
			delete pPetFrame;
		}
		it = burdens.erase(it);
	}
}

void ntAvatarChatTask::KeepUniqueCompleteList(const ntAvatarBurden& burden)
{
	std::list<ntAvatarBurden>::iterator it;
	it = std::find(m_completeList.begin(), m_completeList.end(), burden);
	if (it != m_completeList.end())
	{
		m_removeList.push_back(*it);
		m_completeList.erase(it);
	}
}

void ntAvatarChatTask::OnAnotherHand(int nWork, void* pData)
{
	std::list<ntAvatarBurden> burdens;

	while (!m_destroying)
	{
		EnterCriticalSection(&m_burdenLock);
		for (std::list<ntAvatarBurden>::iterator addIt = m_addList.begin();
			addIt != m_addList.end(); ++addIt)
		{
			burdens.push_back(*addIt);
		}
		m_addList.clear();
		LeaveCriticalSection(&m_burdenLock);

		if (burdens.size() > 1)
		{
			burdens.reverse();
			burdens.sort();
			burdens.unique();
		}

		std::list<ntAvatarBurden>::iterator it = burdens.begin();
		while (!m_destroying && it != burdens.end())
		{
			if ((*it).m_pPetFrame == (CPetFrame*)-1)
			{
				(*it).m_pPetFrame = NULL;
				EnterCriticalSection(&m_burdenLock);
				KeepUniqueCompleteList(*it);
				LeaveCriticalSection(&m_burdenLock);

				unsigned long key = (*it).m_guid;
				it = burdens.erase(it);
				if ((*it).m_guid == key)
				{
					it = burdens.erase(it);
				}
			}
			else
			{
				++it;
			}
		}

		if (ntAvatarManager::Instance()->m_pMyAvatar)
		{
			it = burdens.begin();
			while (!m_destroying && it != burdens.end())
			{
				if (ntAvatarManager::Instance()->m_pMyAvatar ==
					ntAvatarManager::Instance()->FindAvatarByGuid((*it).m_guid))
				{
					if ((*it).Load())
					{
						EnterCriticalSection(&m_burdenLock);

						KeepUniqueCompleteList(*it);
						m_completeList.push_back(*it);
						LeaveCriticalSection(&m_burdenLock);
					}

					it = burdens.erase(it);

					g_LoadParam.loadedNum = 1;
					g_LoadParam.bLoaded = true;
					if (!g_SALoadingState)
					{
						g_SALoadingState = true;
					}
				}
				else
				{
					++it;
				}
			}
		}

		it = burdens.begin();
		while (!m_destroying && it != burdens.end())
		{
			if ((*it).Load())
			{
				EnterCriticalSection(&m_burdenLock);

				KeepUniqueCompleteList(*it);
				m_completeList.push_back(*it);
				LeaveCriticalSection(&m_burdenLock);

				if (!m_destroying)
				{
					Sleep(15);
				}
			}

			it = burdens.erase(it);
		}

		DeleteBurdenPet(burdens, m_removeList);

		if (!m_destroying)
		{
			Sleep(500);
		}
	}

	DeleteBurdenPet(burdens, m_removeList);
	DeleteBurdenPet(burdens, m_completeList);
}

void ntAvatarChatTask::Register(void)
{
	SetMainActor(AddActor("ntAvatarChatMain", "AvatarChat", 0, ""));

	AddActor("CNPC", "NPC", 0, "");

	if (IsLocalContent((localContentType_t)0x77))
	{
		AddActor("CPrivateTrade", "PrivateTrade", 0, "");
	}

	AddActor(s_gatewayClass, s_gatewayName, 0, GetRTTI()->GetName());
}

void ntAvatarChatTask::PreserveBack(const char* name)
{
	CTask::PreserveBack(name);

	WSingleton<CChatMsg>::Instance()->Close(true, true);
}

void ntAvatarChatTask::RestorePreserved(void)
{
	if (CSharedDoc::Instance()->m_golfGame.gameType == 8)
	{
		if (CFx::Instance())
		{
			CFx::Instance()->SetActive(true);
		}
	}

	SetGameType(2);

	CTask::RestorePreserved();

	FrDesktop* pDesktop = g_pFresh->GetDesktop();
	pDesktop->SetViewFocus(true);
	pDesktop->Init(g_view->GetWidth(), g_view->GetHeight());
	pDesktop->SetWallPaper(0, true);

	CChatMsg::Instance()->ResetUiPosition();
}

void ntAvatarChatTask::Destroy(void)
{
	CTask::Destroy();

	delete CQuitWindow::Instance();
}

void ntAvatarChatTask::ShowQuitDialog(void)
{
	if (CChatMsg::Instance()->IsOpened())
	{
		CChatMsg::Instance()->Close(true, true);
	}
	else
	{
		if (CQuitWindow::Instance())
		{
			CQuitWindow::Instance()->Process();
		}
	}
}

void ntAvatarChatTask::Process(float dt)
{
	if (g_input->GetDown("ESCAPE", true) == 1 &&
		!g_pFresh->GetManager()->HasEscKeyWindow())
	{
		if (CSharedDoc::Instance()->GetStateTrade())
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb\xc1\xdf\xbf\xa1\xb4\xc2 \xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad \xb3\xaa\xb0\xa5\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFFFF00, true, false);
		}
		else
		{
			ShowQuitDialog();
		}
	}

	std::list<ntAvatarBurden> completed;

	EnterCriticalSection(&m_burdenLock);
	for (std::list<ntAvatarBurden>::iterator completeIt =
			 m_completeList.begin();
		completeIt != m_completeList.end(); ++completeIt)
	{
		completed.push_back(*completeIt);
	}
	m_completeList.clear();
	LeaveCriticalSection(&m_burdenLock);

	for (std::list<ntAvatarBurden>::iterator it = completed.begin();
		it != completed.end(); ++it)
	{
		ntAvatarBurden& burden = *it;

		ntAvatar* pAvatar =
			ntAvatarManager::Instance()->FindAvatarByGuid(burden.m_guid);
		if (pAvatar)
		{
			if (pAvatar->m_pPetFrame)
			{
				RemoveAvatarBurden(
					ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid,
						pAvatar->m_charInfo, pAvatar->m_pPetFrame));

				pAvatar->m_pPetFrame = NULL;
			}

			pAvatar->m_burden = burden;
			pAvatar->m_bBurdenLoaded = true;
		}
		else
		{
			RemoveAvatarBurden(burden);
		}
	}

	CTask::Process(dt);

	g_pFresh->OnProcess(dt);

	CChatMsg::Instance()->ProcessWindow(dt);

	FrWnd* pKeyFocused = g_pFresh->GetManager()->GetKeyFocused();
	if (pKeyFocused && pKeyFocused->IsVisible())
	{
		g_pFresh->GetManager()->SetExclusiveKey(true);
	}
	else
	{
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}
}

void ntAvatarChatTask::Display(void)
{
	CTask::Display();

	if (g_pFresh->GetDesktop())
	{
		g_pFresh->GetDesktop()->OnDisplay(true, false, true);
	}

	if (!CChatMsg::Instance()->HasBeenPrinted())
	{
		CChatMsg::Instance()->PrintInScreen();
		CChatMsg::Instance()->DrawChatWindow();
		CChatMsg::Instance()->PrintInChatWindow();
	}

	g_pFresh->OnDisplay(true);
}

int ntAvatarChatTask::OnPacket(WReceivedPacket& packet)
{
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (g_iRecvPacketType)
	{
	case 0x46:
	{
		static sSlotInfo slotInfo;
		static sCharacterInfo charInfo;

		unsigned char mode = packet.Decode1();
		unsigned short check = packet.Decode2();

		if (check == 0xFFFF)
		{
			std::list<sSlotInfo>& players =
				WSingleton<CSharedDoc>::Instance()->m_slotList;

			switch (mode)
			{
			case 0:
			case 5:
			{
				if (mode == 0)
				{
					players.clear();
				}
				int count = packet.Decode1();

				for (int i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&slotInfo, 0x152);

					if (!IsMassGame(WSingleton<CSharedDoc>::Instance()
								->m_roomInfo.gameType))
					{
						packet.DecodeBuffer(&charInfo, 0x1BC);
						if (IsLocalContent((localContentType_t)0x57))
						{
							RequestUccInfo(charInfo);
						}
					}

					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (int)&slotInfo,
						(int)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 7:
			{
				packet.Decode1();

				packet.DecodeBuffer(&slotInfo, 0x152);

				if (!IsMassGame(WSingleton<CSharedDoc>::Instance()
							->m_roomInfo.gameType))
				{
					packet.DecodeBuffer(&charInfo, 0x1BC);
					if (IsLocalContent((localContentType_t)0x57))
					{
						RequestUccInfo(charInfo);
					}
				}

				std::list<sSlotInfo>::iterator it;
				it = std::find(players.begin(), players.end(), slotInfo.dwGuid);
				if (it == players.end())
				{
					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (int)&slotInfo,
						(int)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 1:
			{
				packet.DecodeBuffer(&slotInfo, 0x152);

				if (!IsMassGame(WSingleton<CSharedDoc>::Instance()
							->m_roomInfo.gameType))
				{
					packet.DecodeBuffer(&charInfo, 0x1BC);
					if (IsLocalContent((localContentType_t)0x57))
					{
						RequestUccInfo(charInfo);
					}
				}

				std::list<sSlotInfo>::iterator it;
				it = std::find(players.begin(), players.end(), slotInfo.dwGuid);
				if (it == players.end())
				{
					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (int)&slotInfo,
						(int)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 2:
			{
				unsigned long uid = packet.Decode4();

				std::list<sSlotInfo>::iterator it;
				it = std::find(players.begin(), players.end(), uid);

				if (it != players.end())
				{
					players.erase(it);

					GetActor("AvatarChat")
						<< MsgObject(0, 0xE5, uid, 0, 0, 0, 0);
				}
				break;
			}

			case 3:
			{
				unsigned long uid = packet.Decode4();

				packet.DecodeBuffer(&slotInfo, 0x152);

				std::list<sSlotInfo>::iterator it;
				it = std::find(players.begin(), players.end(), uid);
				if (it != players.end())
				{
					GetActor("AvatarChat")
						<< MsgObject(0, 0xE6, uid, (int)&slotInfo, 0, 0, 0);
				}
				break;
			}
			}

			WSingleton<CSharedDoc>::Instance()->m_roomInfo.nUserNum =
				(unsigned char)players.size();
		}
		break;
	}

	case 0x7A:
	{
		unsigned long guid = packet.Decode4();
		unsigned short value = packet.Decode2();
		GetActor("AvatarChat") << MsgObject(0, 0x5F, guid, value, 0, 0, 0);
		break;
	}

	case 0x7F:
	{
		std::string nickName;
		packet.DecodeStr(nickName);

		nickName +=
			"\xb4\xd4\xc0\xbb \xb0\xad\xc1\xa6 \xc5\xf0\xc0\xe5\xbd\xc3\xc5\xb0\xb7\xc1 \xc7\xd5\xb4\xcf\xb4\xd9.\n\xc2\xf9\xbc\xba\xc7\xcf\xbd\xca\xb4\xcf\xb1\xee?";
		GetActor("AvatarChat") << MsgObject(0, 0x31, (int)nickName.c_str(),
			packet.Decode4(), 0, 0, 0);
		break;
	}

	case 0x3F:
	{
		std::string text;
		packet.DecodeStr(text);
		GetActor("avatarChatMain")
			<< MsgObject(0, 3, (int)text.c_str(), 0xFFFF7878, 0, 0, 0);
		break;
	}

	case 0x3E:
	{
		std::string nickName;
		std::string message;
		unsigned char chatType = packet.Decode1();
		std::string text;

		unsigned long color = 0;
		bool bCommand = false;
		bool bBalloon = false;

		bool bWhisper = (chatType >> 7) != 0;
		chatType &= 0x7F;

		packet.DecodeStr(nickName);
		packet.DecodeStr(message);

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
			message.c_str(), false);

		switch (chatType)
		{
		case 0:
		{
			ntAvatar* pAvatar =
				ntAvatarManager::Instance()->FindAvatarByNickName(
					nickName.c_str());
			if (pAvatar)
			{
				ntAvatar* pMyAvatar = ntAvatarManager::Instance()->m_pMyAvatar;
				if (pMyAvatar &&
					(pAvatar->m_pos - pMyAvatar->m_pos).Magnitude() > 256.0f)
				{
					return 1;
				}
			}

			if (strcmp(nickName.c_str(),
					WSingleton<CSharedDoc>::Instance()->m_myInfo.info.sNick) ==
				0)
			{
				color = 0xFFFFCC00;
			}
			else
			{
				std::list<std::string>::iterator it = std::find(
					WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(),
					WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(),
					nickName);
				if (it !=
					WSingleton<CSharedDoc>::Instance()->m_ignoreList.end())
				{
					return 1;
				}

				color = 0xFFFFFFFF;
			}

			if (message.c_str()[0] == '!')
			{
				bCommand = true;
			}

			pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(
				nickName.c_str());
			if (pAvatar)
			{
				pAvatar->SetBalloonText(message.c_str());
			}

			bBalloon = true;
			if (bWhisper)
			{
				color = 0xFF6EECFB;
			}
			else
			{
				pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(
					nickName.c_str());
				if (pAvatar)
				{
					pAvatar->SetBalloonText(message.c_str());
				}

				GetActor("AvatarChat") << MsgObject(0, 0x60,
					(int)nickName.c_str(), (int)message.c_str(), 0, 0, 0);
			}

			text = nickName + " : \x08" + message;
			break;
		}

		case 1:

			text = "\xbe\xcb\xb8\xb2 : " + nickName +
				" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc3\xa4\xb3\xce\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 5:

			text = "\xbe\xcb\xb8\xb2 : " + nickName +
				" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 2:

			text = "\xbe\xcb\xb8\xb2 : " + nickName +
				" \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 3:

			text =
				"\xb0\xad\xc5\xf0\xbf\xa1 \xb0\xfc\xc7\xd1 \xc5\xf5\xc7\xa5\xb0\xa1 \xba\xce\xb0\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 4:

			text = "\xbe\xcb\xb8\xb2 : " + nickName +
				" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xbc\xf6\xbd\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 6:

			text = "\xbe\xcb\xb8\xb2 : " + nickName +
				" \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc1\xa2\xbc\xd3\xc1\xdf\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 7:
			text = "\xbe\xcb\xb8\xb2 : " + message;
			color = 0xFFFF7878;
			break;

		case 10:

			text = "\xbe\xcb\xb8\xb2 : " +
				std::string(MakeStr(
					"%s\xb4\xd4\xb2\xb2\xbc\xad %d \xc6\xce \xc1\xd6\xb8\xd3\xb4\xcf\xb8\xa6 \xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					nickName.c_str(), packet.Decode4()));
			color = 0xFFFF7878;
			break;

		default:
			text = nickName + " : " + message;
			break;
		}

		if (chatType != 0 || (!bCommand && GOLFDOC()->IsShowChat()))
		{
			CChatMsg::Instance()->AddChatMsg(text, color, true, bBalloon);
		}
		break;
	}

	case 0x82:
	{
		unsigned char chatType = packet.Decode1();
		bool bWhisper = (chatType >> 7) != 0;
		chatType &= 0x7F;

		bool bMine = (chatType == 0);

		unsigned long color = 0xFFD2A2FF;
		if (bWhisper)
		{
			color = 0xFF6EECFB;
		}

		std::string nickName, message;
		std::string text;

		packet.DecodeStr(nickName);
		packet.DecodeStr(message);

		if (bMine)
		{
			text = WSingleton<CSharedDoc>::Instance()->m_myInfo.info.sNick;
			text += ">";
			text += nickName;
		}
		else
		{
			if (!COption::Instance()->gIsWhisperAllowed())
			{
				break;
			}

			std::list<std::string>::iterator it = std::find(
				WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(),
				WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(),
				nickName);
			if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end())
			{
				break;
			}

			int reply;
			GetActor("AvatarChat")
				<< MsgObject(0, 0x3F, (int)&reply, 0, 0, 0, 0);
			GetActor("AvatarChat")
				<< MsgObject(0, 0x38, (int)nickName.c_str(), 0, 0, 0, 0);

			text = nickName + ">" + MyNick();
		}

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
			message.c_str(), false);

		text += " : \x08'";
		text += message;
		text += "'";

		if (AfxGetTask()->IsRecieveWhisper())
		{
			CChatMsg::Instance()->AddChatMsg(text, color, true, true);
		}
		else
		{
			WSendPacket send((enumClientPacket)0xe0);
			send.EncodeStr(nickName);
			send.Send(TO_GAME);
		}
		break;
	}

	case 0xC2:
	{
		unsigned long guid = packet.Decode4();
		int subType = packet.Decode1();

		switch (subType)
		{
		case 4:
		{
			float x, y, z;
			packet.DecodeBuffer(&x, 4);
			packet.DecodeBuffer(&y, 4);
			packet.DecodeBuffer(&z, 4);
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, (int)&x, (int)&y, (int)&z);
			break;
		}

		case 5:
		{
			unsigned long value = packet.Decode4();
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, value, 0, 0);
			break;
		}

		case 6:
		{
			float x, y, z;
			packet.DecodeBuffer(&x, 4);
			packet.DecodeBuffer(&y, 4);
			packet.DecodeBuffer(&z, 4);
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, (int)&x, (int)&y, (int)&z);
			break;
		}

		case 7:
		{
			std::string text;
			packet.DecodeStr(text);
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, (int)text.c_str(), 0, 0);
			break;
		}

		case 8:
		{
			unsigned long value = packet.Decode4();
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, value, 0, 0);
			break;
		}

		case 9:
			GetActor("AvatarChat")
				<< MsgObject(0, 0x5B, guid, subType, 0, 0, 0);
			break;
		}
		break;
	}

	case 0x9C:
	{
		unsigned char weather = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		CSceneManager::Instance()->SetWeather(weather, false);
		break;
	}

	case 0x8C:
	{
		unsigned long guid = packet.Decode4();
		unsigned char value = packet.Decode1();
		GetActor("AvatarChat") << MsgObject(0, 0x3D, guid, value, 0, 0, 0);
		break;
	}

	case 0x76:
	{
		unsigned long guid = packet.Decode4();
		int value = packet.Decode1();

		GetActor("AvatarChat") << MsgObject(0, 0x29, guid, value, 0, 0, 0);
		break;
	}

	case 0x92:
		GOLFDOC()->m_bStopRequest = packet.Decode1() == 1;

		if (!GOLFDOC()->IsReported())
		{
			break;
		}

		if (GOLFDOC()->m_bStopRequest)
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(
				"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xbf\xa1 \xb4\xeb\xc7\xd8\xbc\xad \xc0\xcc\xb9\xcc \xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open((FRESH_PFN_RESULT)0, 4);

			GOLFDOC()->SetReport(false);
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(
				"\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n\n\t\t\t\t\t\t\t\t\t  \xc6\xaf\xc1\xa4 \xbb\xf3\xb4\xeb\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xa6 \xc2\xf7\xb4\xdc \xc7\xcf\xb7\xc1\xb8\xe9 \xbb\xf3\xb4\xeb \xc1\xa4\xba\xb8\xc3\xa2\xc0\xbb \xb8\xb6\xbf\xec\xbd\xba \xb4\xf5\xba\xed \xc5\xac\xb8\xaf \xc8\xc4 \\c0xffff0000\\c[\xb4\xeb\xc8\xad \xc2\xf7\xb4\xdc \xbe\xc6\xc0\xcc\xc4\xdc]\\c0xff000000\\c\xc0\xbb \xbc\xb1\xc5\xc3 \xc7\xcf\xbd\xc3\xb8\xe9 \xb5\xcb\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open((FRESH_PFN_RESULT)0, 0);
		}
		break;

	case 0x196:
		break;

	case 0x19B:
	{
		unsigned long guid = packet.Decode4();
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar)
		{
			for (int i = 0; i < 5; i++)
			{
				SpecialPrizeItem& item =
					pAvatar->GetMySpItem((eSPAVILITYTYPE)i);

				float value = 0;
				packet.DecodeBuffer(&value, 4);
				item.setvalue(value);
			}
		}
		break;
	}

	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
