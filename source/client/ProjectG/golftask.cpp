#include "stdafx.h"
#include "guilddefine.h"
#include "mathconsts.h"
#include "s5/utilities.h"
#include "commonutil.h"
#include "golftask.h"
#include "mousecursor.h"
#include "clientsetting.h"
#include "messengerdlg.h"
#include "cardsystem.h"
#include "approachdesc.h"
#include "bonusticket.h"
#include "capturedbg.h"
#include "exhibition.h"
#include "chatmsg.h"
#include "golfdoc.h"
#include "quitwindow.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "hackingmanager.h"
#include "logininfo.h"
#include "optiondlg.h"
#include "headicon.h"
#include "user_info.h"
#include "ucclibrary.h"
#include "wind.h"
#include "quadtree.h"
#include "golfball.h"
#include "autotarget.h"
#include "club.h"
#include "matchhistory.h"
#include "guildactor.h"
#include "gatewayactor.h"

extern Fresh* g_pFresh;
void RequestUccInfo(sCharacterInfo& character);
void RequestUccInfo(sUserInfo& user);

extern int g_iRecvPacketType;

#pragma pack(push, 1)
struct sPlayerSyncData
{
	float angle;
	struct
	{
		float x, y, z;
	} lastShotPos;
	struct
	{
		float x, y, z;
	} ballPos;
	unsigned char holes[18];
	unsigned char holeIn[18];
	__int64 holeRecord[18];
	unsigned long ghostResult;
	unsigned char totalScore;
	int ghostAward;
	unsigned char reserved[4];
	unsigned char numItems;
	unsigned long shotItemTypeId;
	float comboGauge;
	unsigned char powerShotLevel;
	unsigned char rivalLevel;
	unsigned char powerShot;
	unsigned char club;
	unsigned long frame;
	unsigned char state;
	__int64 extraPang;
};
#pragma pack(pop)

IMPLEMENT_OBJECT(CGolfTask, CTask)

CGolfTask::CGolfTask(void)
{
	m_bLobbyScreenSize = false;
	IActor::m_defaultSkipLayer = 0x622021;
	m_returnToLobbyTime = -10.0f;
}

void CGolfTask::Init(const char* backGround)
{
	CTask::Init(0);
}

void CGolfTask::Register(void)
{
	SetMainActor(AddActor("CGolfRule", "GolfRule", 0x20021, ""));

	AddActor("CClock", "Clock", 0x20, "");
	AddActor("CGallery", "Gallery", 0, "");
	AddActor("CCamera", "Camera", 0x20, "");

	AddActor("CGolfBg", "GolfBg", 0x22001, "");

	AddActor("CGolfPlayer", "Player", 0x22000, "");
	AddActor("CGolfCaddie", "Caddie", 0x2000, "");
	AddActor("CMascot", "Mascot", 0x2000, "");
	AddActor("CNPC", "NPC", 0x2001, "");

	AddActor("CGrid", "Grid", 0x20, "");
	AddActor("CSpot", "Spot", 0x20, "");
	AddActor("CBall", "Ball", 0x2000, "");

	AddActor("CGuideLine", "GuideLine", 0x20, "");
	AddActor("CTrajectory", "Trajectory", 0, "");
	AddActor("CParticleEffect", "Effect", 0x20001, "");
	AddActor("CFx", "Fx", 0x20001, "");

	AddActor("CAmbientSound", "Ambient", 1, "");

	AddActor("CBubble", "Bubble", 0x20, "");
	AddActor("CLetter", "Letter", 0x20020, "");
	AddActor("CScreen", "Screen", 0x20, "");
	AddActor("CItemWindow", "Item", 0x2000, "");
	AddActor("CSpin", "Spin", 0, "");
	AddActor("CLogo", "Logo", 1, "");
	AddActor("CScore", "Score", 0x222020, "");
	AddActor("CGuideBook", "GuideBook", 0x22020, "");

	AddActor(s_guildActor[0], s_guildActor[1], 0, "");

	AddActor(s_gatewayActor[0], s_gatewayActor[1], 0, GetRTTI()->GetName());

	CTask* pPreservedTask = AfxGetPreservedTask();
	if (pPreservedTask)
	{
		IActor* pTutorial = pPreservedTask->GetActor("TutorialMain");
		if (pTutorial)
		{
			const char* className = 0;
			const char* actorName = 0;
			pTutorial << MsgObject(0, 0x14E, (int)&className, (int)&actorName,
				0, 0, 0);
			AddActor(className, actorName, 0x20, "");
		}
	}
	else
	{
		AddActor("CCutin", "Cutin", 0, "");
	}

	AddActor("ShotPreview", "ShotPreview", 0, "");

	if (DetermineWhetherToAddActor_GroundItemMan())
	{
		AddActor("GroundItemMan", "GroundItemMan", 0, "");
	}
	AddActor("CMagicCarpetHelper", "MagicCarpetHelper", 0, "");
}

void CGolfTask::Load(void)
{
	IActor::m_pLayer = &GOLFDOC()->GameMode();

	new CQuitWindow;

	SetLoadingBackGround("loading_background.jpg");

	g_pFresh->GetDesktop()->SetViewFocus(true);

	CChatMsg::Instance()->ResetUiPosition();

	CTask::Load();
}

void CGolfTask::Destroy(void)
{
	CTask::Destroy();

	g_pFresh->GetDesktop()->SetWallPaper("gameroom_background.jpg", true);
	g_pFresh->GetDesktop()->SetBgColor(0xFFFFFFFF);
	CMouseCursor::Instance()->SetTempActive(false);

	if (CQuitWindow::Instance())
	{
		delete CQuitWindow::Instance();
	}
}

void CGolfTask::Process(float dt)
{
	if (HackingManager::Instance())
	{
		HackingManager::Instance()->ResetCrypticValue(0x14);
	}

	if (CSharedDoc::Instance()->m_playingMode == (eReplayModeType)1)
	{
		if (g_input->GetDown("ESCAPE", true) == 1 &&
			!g_pFresh->GetManager()->HasEscKeyWindow())
		{
			int itemState = 0;
			GetActor("Item")
				<< MsgObject(0, 0x199, (int)&itemState, 0, 0, 0, 0);

			if (CChatMsg::Instance()->IsOpened())
			{
				CChatMsg::Instance()->Close(false, true);
			}
			else if (itemState != 3)
			{
				GetActor("Item") << MsgObject(0, 0x195, 0, 0, 0, 0, 0);
			}
			else if (CMessengerInfo::Instance()->Close(false))
			{
				if (CQuitWindow::Instance())
				{
					CQuitWindow::Instance()->Process();
				}
			}
		}
	}

	CSharedDoc::Instance()->m_chatManager.CheckPenaltyTime();

	if (!CChatMsg::Instance()->IsOpened())
	{
		unsigned char roomType = CSharedDoc::Instance()->m_golfGame.gameType;
		if (roomType != 0xB && roomType != 0xC)
		{
			if (!g_pFresh->GetManager()->IsKeyExclusive() &&
				(g_input->GetDown("ENTER", true) ||
					g_input->GetDown("PADENTER", true)))
			{
				if ((g_input->Get("LCONTROL", true) ||
						g_input->Get("RCONTROL", true)) &&
					(g_input->Get("LSHIFT", true) ||
						g_input->Get("RSHIFT", true)))
				{
					GetActor("GolfRule") << MsgObject(0, 0x37,
						(int)"\xc6\xc0\xbf\xa1\xb0\xd4", 0, 0, 0, 0);
				}
				else if (g_input->Get("LSHIFT", true) ||
					g_input->Get("RSHIFT", true))
				{
					GetActor("GolfRule") << MsgObject(0, 0x37,
						(int)"\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0, 0, 0, 0);
				}
				else if (g_input->Get("LCONTROL", true) ||
					g_input->Get("RCONTROL", true))
				{
					GetActor("GolfRule") << MsgObject(0, 0x65, 0, 0, 0, 0, 0);
				}

				CChatMsg::Instance()->Open(true, true);
			}
		}
	}

	if (g_input->GetDown("\xb8\xc5\xc5\xa9\xb7\xce\xc3\xa2", true) &&
		!g_input->Get("LALT", true) && !g_input->Get("RALT", true) &&
		!g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true))
	{
		FrOptionDlg* pOptionDlg =
			CreateForm<FrOptionDlg>(g_pFresh->GetManager(), this, "option", 0);
		if (pOptionDlg)
		{
			pOptionDlg->Open(0, 1);
			pOptionDlg->SetApplyScrSize(true);
			pOptionDlg->SetCloseResult(FrCANCEL);
			pOptionDlg->OpenTab(FrOptionDlg::TAB_MACRO);
		}
	}

	float taskDt = dt;
	if (IsLocalContent((localContentType_t)0x4F) &&
		CSharedDoc::Instance()->m_playingMode == (eReplayModeType)2)
	{
		if (CSharedDoc::Instance()->m_replayState == 4)
		{
			taskDt = 0;
		}
	}

	CTask::Process(taskDt);

	g_pFresh->OnProcess(dt);

	CChatMsg::Instance()->ProcessWindow(dt);

	if (!(m_returnToLobbyTime > 0.0f))
	{
		return;
	}

	m_returnToLobbyTime -= dt;

	if (!(m_returnToLobbyTime < 0.1f))
	{
		return;
	}

	m_returnToLobbyTime = -10.0f;

	CSharedDoc::Instance()->m_myInfo.roomIndex = 0xFFFF;
	CProjectG::Instance()->FromGolfToLobbySettings();
	CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
	CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (int)"ROOMLIST", 0, 0, 0);
	CTaskManager::Instance()->PostMsg(0, "Lobby", 0x1A, 0, 0, 0, 0);

	switch (CSharedDoc::Instance()->m_lobbyExitReason)
	{
	case 1:
		if (CSharedDoc::Instance()->m_golfGame.gameType != 10)
		{
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23,
				(int)"\xc1\xa2\xbc\xd3\xc0\xcc \xba\xd2\xbe\xc8\xc7\xcf\xbf\xa9 \xb0\xd4\xc0\xd3\xbf\xa1\xbc\xad \xc5\xf0\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0);
		}
		break;
	case 2:
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23,
			(int)"\xc5\xb8\xc0\xd3\xbe\xc6\xbf\xf4 3\xb9\xf8\xc0\xb8\xb7\xce \xbd\xc7\xb0\xdd\xb5\xc7\xbe\xee \xc5\xf0\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0);
		break;
	case 4:
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23,
			(int)"\xb1\xe2\xba\xea\xbe\xf7 3\xb9\xf8\xc0\xb8\xb7\xce \xbd\xc7\xb0\xdd\xb5\xc7\xbe\xee \xc5\xf0\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0);
		break;
	case 8:
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23,
			(int)"\xb0\xd4\xc0\xd3\xc1\xf8\xc7\xe0\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xbe\xf8\xbe\xee\xbc\xad \xc5\xf0\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0);
		break;
	}
}

void CGolfTask::Display(void)
{
	CTask::Display();

	if (GOLFDOC()->GameMode() & 0x400000)
	{
		GetActor("Score") << MsgObject(0, 0x12F, 3, 0, 0, 0, 0);
		GetActor("GolfRule") << MsgObject(0, 0x78, 2, 0, 0, 0, 0);
	}

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

	if (!CCapturedBg::Instance()->IsCaptured() ||
		GOLFDOC()->m_tutorialMode != 15)
	{
		g_pFresh->OnDisplay(false);
	}

	if (GOLFDOC()->GameMode() & 0x10)
	{
		if (CCapturedBg::Instance())
		{
			CCapturedBg::Instance()->Capture();
		}
	}

	if ((GOLFDOC()->GameMode() & 1) && OnlinePlay() && !IsMassGame())
	{
		DrawLoadingInfo(g_pFresh);
	}
}

void CGolfTask::OnDisplayWhileLoading(void)
{
	CTask::OnDisplayWhileLoading();

	if (OnlinePlay() && !IsMassGame(Doc()->m_golfGame.gameType))
	{
	}
}

void CGolfTask::UpdateLoadingInfo(unsigned long uid, unsigned char progress)
{
	std::list<sSlotInfo>::iterator it =
		std::find(CSharedDoc::Instance()->m_slotList.begin(),
			CSharedDoc::Instance()->m_slotList.end(), uid);
	if (it != CSharedDoc::Instance()->m_slotList.end())
	{
		m_loadingInfo[it->dwGuid] = progress;
	}
}

void CGolfTask::DrawLoadingInfo(Fresh* pFresh)
{
	FrGraphicInterface* pGDI = pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
	{
		return;
	}

	float y = g_view->GetHeight() - 100.0f;
	int line = 0;

	std::list<sSlotInfo>::iterator it;
	for (it = CSharedDoc::Instance()->m_slotList.begin();
		it != CSharedDoc::Instance()->m_slotList.end(); ++it)
	{
		sSlotInfo& player = *it;
		if (player.dwGuid == CSharedDoc::Instance()->m_myInfo.info.dwGuid)
		{
			continue;
		}

		unsigned char progress = m_loadingInfo[player.dwGuid];

		pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
		pGDI->SetTextStyle(2);

		char* text;
		if (progress == 10)
		{
			text = MakeStr(
				"%s\xb4\xd4\xc0\xba \xb7\xce\xb5\xf9 \xbf\xcf\xb7\xe1 \xc0\xd4\xb4\xcf\xb4\xd9.",
				player.sNick);
		}
		else
		{
			text = MakeStr(
				"%s\xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 %d%% \xb7\xce\xb5\xf9 \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.",
				player.sNick, progress * 10);
		}

		pFresh->GetManager()->PrintText(WPoint(20.0f, y + line * 20), 0, text,
			-1.0f, 0xFFFFFFFF);
		line++;
	}
}

void CGolfTask::OnInitFinished(void)
{
	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)0x11);
		packet.Send(TO_GAME);
	}

	if (CChatMsg::Instance())
	{
		CChatMsg::Instance()->Close(true, true);
		CChatMsg::Instance()->EnableCaretMove(true);
	}

	GetActor("GolfRule") << MsgObject(0, 0x29, 0, 0, 0, 0, 0);

	if (!GalleryMode())
	{
		return;
	}

	switch (CSharedDoc::Instance()->m_golfGame.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	{
		WSendPacket packet((enumClientPacket)0x3f);
		packet.Encode4(CSharedDoc::Instance()->m_userInfo[0].info.dwGuid);
		packet.Send(TO_GAME);
		break;
	}
	}
}

int CGolfTask::OnPacket(WReceivedPacket& packet)
{
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (g_iRecvPacketType)
	{
	case 0xF2:
	{
		unsigned char screenMode = packet.Decode1();
		GetActor("Screen") << MsgObject(0, 0x1CF, screenMode, 0, 0, 0, 0);
		break;
	}
	case 0xC6:
		if (IsLocalContent((localContentType_t)0x87))
		{
			__int64 pang = packet.Decode8();
			__int64 calc = packet.Decode8();

			if (calc != 0)
			{
				if (Doc()->m_myInfo.stat.i64Pang == pang + calc)
				{
					Doc()->m_myInfo.stat.i64Pang = pang;
					Doc()->m_userInfo[MyIndex()].stat.i64Pang = pang;
				}
				else
				{
					WSendPacket report((enumClientPacket)0x33);

					std::string text;
					char buf[128];
					sprintf(buf,
						"[Pang Hacker] Old Pangya %I64d, Recive  %I64d , Calculation %I64d",
						Doc()->m_myInfo.stat.i64Pang, pang, calc);
					text = buf;

					report.Encode1(3);
					report.EncodeStr(text);
					report.Send(TO_GAME);
					break;
				}
			}
			else
			{
				Doc()->m_myInfo.stat.i64Pang = pang;
				Doc()->m_userInfo[MyIndex()].stat.i64Pang = pang;
			}
		}
		else
		{
			__int64 pang = packet.Decode8();
			Doc()->m_myInfo.stat.i64Pang = pang;
			Doc()->m_userInfo[MyIndex()].stat.i64Pang = pang;
		}
		break;
	case 0xC5:
	{
		float gameSpeed;
		packet.DecodeBuffer(&gameSpeed, 4);
		WSingleton<CProjectG>::Instance()->SetGameSpeed(gameSpeed);

		unsigned long uid = packet.Decode4();

		if (uid == MyGuid(false))
		{
			if (!CLoginInfo::Instance()->IsPcBang())
			{
				std::list<sItemInfo>::iterator it =
					std::find(Doc()->m_myItemList.begin(),
						Doc()->m_myItemList.end(), 0x1A000011);

				if (it != Doc()->m_myItemList.end())
				{
					if ((*it).ItemType)
					{
						if ((*it).HourRemain <= 0)
						{
							Doc()->m_myItemList.erase(it);
						}
					}
					else
					{
						(*it).Common[0]--;

						if ((*it).Common[0] == 0)
						{
							Doc()->m_myItemList.erase(it);
						}
					}
				}
			}
		}

		GetActor("Letter") << MsgObject(0, 0x193, 0, 0, 0, 0, 0);
		break;
	}
	case 0x52:
	case 0x65:
		if (CMessengerInfo::Instance()->GetMyStatus() != (MSN::eStatusType)3)
		{
			CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)4);
		}

		GetActor("GolfRule") << MsgObject(0, 0x8F, 1, 0, 0, 0, 0);
		break;
	case 0x113:
		if (IsLocalContent((localContentType_t)0x32))
		{
			unsigned char tickets[100] = { 0 };
			std::string name;
			packet.DecodeStr(name);

			packet.DecodeBuffer(tickets, 100);
			CBonusTicket::Instance().SetTicketFromServer(name, tickets, 100);
		}
		break;
	case 0x51:
	{
		unsigned long uid = packet.Decode4();

		GetActor("GolfRule") << MsgObject(0, 0x243, 0, 0, 0, 0, 0);

		if (IsMassGame())
		{
			GetActor("GolfRule") << MsgObject(0, 0x81, 0, 0, 0, 0, 0);
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("GolfRule")
				<< MsgObject(0, 0x81, Doc()->GetIndex(uid), 0, 0, 0, 0);
		}

		if (IsLocalContent((localContentType_t)0x28))
		{
			CMatchHistory::Instance()->RequestHistory();
		}
		break;
	}
	case 0x9C:
	{
		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 0, 0, 0, 0);
		break;
	}
	case 0x59:
	{
		unsigned char intensity = packet.Decode1();
		unsigned char windColor = packet.Decode1();
		WSingleton<CCardManager>::Instance()->SetWindColor(windColor);
		unsigned char direction = packet.Decode1();
		unsigned char flag = packet.Decode1();
		unsigned char enabled = packet.Decode1();

		Wind().SetActive(true);

		Wind().Init(intensity, direction, flag, enabled);
		break;
	}
	case 0x53:
	{
		unsigned long uid = packet.Decode4();
		sShotProtocol shot;
		packet.DecodeBuffer(&shot, sizeof(shot));

		packet.GetRemainLength();

		GetActor("GroundItemMan")
			<< MsgObject(0, 0x279, (int)&packet, 0, 0, 0, 0);

		if (Doc()->AreWePlayingTogether(uid))
		{
			GOLFDOC()->m_shotInfo = shot;
			GetActor("GolfRule") << MsgObject(0, 0x7A, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x54:
	{
		unsigned long uid = packet.Decode4();
		unsigned long cameraData;
		packet.DecodeBuffer(&cameraData, 4);

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("Camera")
				<< MsgObject(0, 0xF2, (int)&cameraData, 0, 0, 0, 0);
		}
		break;
	}
	case 0x55:
	{
		unsigned long uid = packet.Decode4();
		CGolfDoc::sGaugeInfo data;
		packet.DecodeBuffer(&data, sizeof(data));

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GOLFDOC()->m_gaugeInfo = data;
			GetActor("GolfRule") << MsgObject(0, 0x7B, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x56:
	{
		unsigned long uid = packet.Decode4();
		unsigned char value = packet.Decode1();

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("Player") << MsgObject(0, 0x11F, value, 0, 0, 0, 0);
		}
		break;
	}
	case 0x57:
	{
		unsigned long uid = packet.Decode4();
		unsigned char clubIndex = packet.Decode1();

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GolfClub().SetClub(clubIndex);
			g_audio->PlaySfx("\xc5\xac\xb7\xb4\xb1\xb3\xc3\xbc", 0, 0, 0, 0,
				0.5f, 200.0f);
		}
		break;
	}
	case 0x58:
	{
		unsigned long uid = packet.Decode4();
		unsigned long itemId = packet.Decode4();

		IsLocalContent((localContentType_t)0x51);

		if (OnlinePlay())
		{
			unsigned long playerUid = packet.Decode4();

			if (!Doc()->AreWePlayingTogether(playerUid))
			{
				break;
			}

			GetActor("Item")
				<< MsgObject(0, 0x19F, playerUid, uid, itemId, 0, 0);
		}
		else
		{
			GetActor("Item") << MsgObject(0, 0x19F, -1, uid, itemId, 0, 0);
		}
		break;
	}
	case 0x5A:
	{
		unsigned long uid = packet.Decode4();

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("GolfRule") << MsgObject(0, 0x7C, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x5B:
	{
		unsigned long uid = packet.Decode4();
		unsigned short iconId = packet.Decode2();

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			if (CHeadIcon::Instance())
			{
				CHeadIcon::Instance()->SetIcon(uid, iconId);
			}
		}
		break;
	}
	case 0x5C:
	{
		unsigned long uid = packet.Decode4();

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("Caddie") << MsgObject(0, 0x1DA, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x61:
	{
		unsigned long uid = packet.Decode4();

		if (Doc()->AreWePlayingTogether(uid))
		{
			if (Doc()->m_golfGame.gameType == 7)
			{
				GOLFDOC()->m_skinsState = packet.Decode2();
			}

			GetActor("GolfRule") << MsgObject(0, 0x7D, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x63:
		if (Doc()->m_golfGame.gameType == 7)
		{
			GOLFDOC()->m_skinsWinner = packet.Decode4();
		}

		GetActor("GolfRule") << MsgObject(0, 0x7E, 0, 0, 0, 0, 0);
		break;
	case 0x64:
	case 0x8F:
	case 0xB1:

		if (!GOLFDOC()->m_bNetGameEnd)
		{
			if (CMessengerInfo::Instance()->GetMyStatus() !=
				(MSN::eStatusType)3)
			{
				CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)4);
			}

			unsigned char count = packet.Decode1();

			if (Doc()->m_golfGame.gameType == 7)
			{
				GOLFDOC()->m_skinsWinner = packet.Decode4();
				GOLFDOC()->m_skinsDonator = packet.Decode4();
			}

			for (int i = 0; i < count; i++)
			{
				CGolfDoc::sGameResult result;
				packet.DecodeBuffer(&result, sizeof(result));

				if (Doc()->AreWePlayingTogether(result.oid))
				{
					GOLFDOC()->m_gameResult[Doc()->GetIndex(result.oid)] =
						result;
				}
			}

			if (type == 0x8F)
			{
				TEAM(0)->score = packet.Decode1();
				TEAM(1)->score = packet.Decode1();
				eTeamName winner = (eTeamName)packet.Decode1();
				GOLFDOC()->m_teamGameWinner = winner;
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x80, 0, 0, 0, 0, 0);
		break;
	case 0x8D:
	{
		unsigned long uid = packet.Decode4();

		if (Doc()->AreWePlayingTogether(uid))
		{
			GetActor("GolfRule") << MsgObject(0, 0x83, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x62:
	{
		sShotResult msg;
		packet.DecodeBuffer(&msg, sizeof(msg));

		if (Doc()->AreWePlayingTogether(msg.oid))
		{
			unsigned char index;

			if (IsMassGame())
			{
				index = 0;
			}
			else
			{
				index = Doc()->GetIndex(msg.oid);
			}

			GOLFDOC()->m_shotResult[index] = msg;

			GetActor("GolfRule") << MsgObject(0, 0x79, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x6A:
	{
		if (!IsMassGame())
		{
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char state = packet.Decode1();

		sRivalData& result = Doc()->m_rivalList[Doc()->GetIndex(uid)];

		if (!Doc()->AreWePlayingTogether(uid))
		{
			break;
		}

		switch (state)
		{
		case 3:
		{
			for (std::vector<sRivalData>::iterator it =
					 Doc()->m_rivalList.begin();
				it != Doc()->m_rivalList.end(); ++it)
			{
				if (it->state != 3)
				{
					result.quitOrder--;
				}
			}

			GetActor("Score")
				<< MsgObject(0, 0x133, result.hole - 1, 0, 0, 0, 0);
			break;
		}
		case 2:
		{
			for (std::vector<sRivalData>::iterator it =
					 Doc()->m_rivalList.begin();
				it != Doc()->m_rivalList.end(); ++it)
			{
				if (it->state == 0)
				{
					result.finishOrder--;
				}
			}

			if (uid == MyGuid(true))
			{
				GOLFDOC()->m_bNetGameEnd = true;
				GOLFDOC()->m_bStatUpdated = false;

				if (CMessengerInfo::Instance()->GetMyStatus() !=
					(MSN::eStatusType)3)
				{
					CMessengerInfo::Instance()->SetMyStatus(
						(MSN::eStatusType)4);
				}
			}
			break;
		}
		}

		result.state = state;
		GetActor("Score") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
		break;
	}
	case 0x6B:
	{
		if (!IsMassGame())
		{
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char hole = packet.Decode1();
		unsigned char shots = packet.Decode1();
		int score = packet.Decode4();

		__int64 pang = packet.Decode8();
		__int64 extraPang = packet.Decode8();

		int holeFinished = packet.Decode1();

		if (!Doc()->AreWePlayingTogether(uid))
		{
			break;
		}

		Doc()->LogGstUpdateHole(uid, hole, score, pang);

		std::vector<sRivalData>::iterator end = Doc()->m_rivalList.end();
		std::vector<sRivalData>::iterator it =
			std::find_if(Doc()->m_rivalList.begin(), end, sFindRival(uid));

		if (it == end)
		{
			break;
		}

		if (holeFinished == 1)
		{
			unsigned char holeShots = shots - it->totalStroke;

			it->holeStroke[hole - 1] = holeShots;
			it->holeScore[hole - 1] = score - it->totalScore;
			it->holePang[hole - 1] = pang - it->totalPang;

			it->totalStroke = shots;
			it->totalScore = score;
			it->hole = Doc()->m_holeOrder[GetHoleIndex(hole)];
		}

		it->totalPang = pang;
		it->totalBonusPang = extraPang;

		if (Doc()->m_golfGame.gameType == 6)
		{
			GetActor("GolfRule") << MsgObject(0, 0x5E, uid, hole, 0, 0, 0);
			if (Doc()->m_teamPlayerGuid[0] == uid ||
				Doc()->m_teamPlayerGuid[1] == uid)
			{
				GetActor("GolfRule") << MsgObject(0, 0x9F, uid, 0, 0, 0, 0);
			}
		}

		GetActor("Score") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);

		if (holeFinished != 1)
		{
			break;
		}

		GetActor("Score") << MsgObject(0, 0x136, hole, 0, 0, 0, 0);
		GetActor("Score") << MsgObject(0, 0x137, 0, 0, 0, 0, 0);
		GetActor("Spot") << MsgObject(0, 0xF9, Doc()->GetIndex(uid), it->hole,
			0, 0, 0);
		break;
	}
	case 0x6C:
	{
		if (!IsMassGame())
		{
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char hole = packet.Decode1();

		float pos[2];
		packet.DecodeBuffer(pos, sizeof(pos));

		unsigned int approachDistance;
		int approachPoint;
		if (Doc()->m_golfGame.gameType == 10)
		{
			approachDistance = packet.Decode4();
			approachPoint = packet.Decode4();
		}

		unsigned short distance = packet.Decode2();

		if (!Doc()->AreWePlayingTogether(uid))
		{
			break;
		}

		unsigned char index = Doc()->GetIndex(uid);

		if (Doc()->m_golfGame.gameType == 10)
		{
			Doc()->m_rivalList[index].approachTime = approachPoint;
			Doc()->m_rivalList[index].approachResultDistance = approachDistance;

			if (approachPoint == -1)
			{
				Doc()->m_rivalList[index].approachDistance = approachPoint;
			}

			GetActor("Score")
				<< MsgObject(0, 0x2A, index == MyIndex(), 0, 0, 0, 0);
		}

		GetActor("Spot") << MsgObject(0, 0xF8, index, hole, distance, 0, 0);

		Doc()->m_rivalList[index].ballPos = WVector(pos[0], 0.0f, pos[1]);
		break;
	}
	case 0x77:
		if (Doc()->m_holeStatistics[0].dwExp == 0xFFFFFFFF)
		{
			if (Doc()->m_golfGame.gameType == 10)
			{
				unsigned long uid = packet.Decode4();

				if (uid != 0xFFFFFFFF)
				{
					std::map<unsigned long, sBriefUserInfo>::iterator it =
						Doc()->m_briefUserInfoMap.find(uid);

					if (it != Doc()->m_briefUserInfoMap.end())
					{
						WSendPacket request((enumClientPacket)0x2F);
						request.Encode4(it->second.dwUid);
						request.Encode1(5);
						request.Send(TO_GAME);
					}
				}
			}
			else
			{
				Doc()->m_holeStatistics[0].dwExp = packet.Decode4();
				Doc()->m_roomType = packet.Decode4();
				Doc()->m_eventType = packet.Decode1();
				Doc()->m_eventFlag = packet.Decode1();

				if (Doc()->m_golfGame.gameType == 6)
				{
					Doc()->m_guildBonusPang = packet.Decode4();
					Doc()->m_guildPoint = packet.Decode4();

					Doc()->m_guildRoundScore[0] = packet.Decode4();
					Doc()->m_guildRoundScore[1] = packet.Decode4();
				}
			}

			packet.DecodeBuffer(Doc()->m_awardItem, 0x60);
		}

		if (Doc()->m_golfGame.gameType == 10)
		{
			break;
		}

		if (GOLFDOC()->m_bStatUpdated)
		{
			GetActor("GolfRule") << MsgObject(0, 0x8F, 1, 0, 0, 0, 0);
		}
		break;
	case 0x5F:
	{
		unsigned long uid = packet.Decode4();

		if (!Doc()->AreWePlayingTogether(uid))
		{
			break;
		}

		std::map<unsigned long, sRoomSlot>::iterator slot =
			Doc()->m_roomSlotMap.find(uid);
		if (slot != Doc()->m_roomSlotMap.end())
		{
			if ((*slot).second.pExhibition)
			{
				delete (*slot).second.pExhibition;
				(*slot).second.pExhibition = 0;
			}
			Doc()->m_roomSlotMap.erase(slot);
		}

		std::list<sSlotInfo>& players = Doc()->m_slotList;
		std::list<sSlotInfo>::iterator it;
		it = std::find(players.begin(), players.end(), uid);
		if (it != players.end())
		{
			unsigned char connection = it->connectionRank;

			players.erase(it);

			for (it = players.begin(); it != players.end(); ++it)
			{
				if (it->connectionRank > connection)
				{
					it->connectionRank--;
				}
			}
		}

		if (!IsMassGame())
		{
			GetActor("GolfRule") << MsgObject(0, 0x82, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x7A:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;

		unsigned long uid = packet.Decode4();
		packet.Decode2();

		for (std::list<sSlotInfo>::iterator it = players.begin();
			it != players.end(); it++)
		{
			it->bMaster = (it->dwGuid == uid);
		}
		break;
	}
	case 0x7C:
		Doc()->m_lobbyExitReason = packet.Decode2();
		m_returnToLobbyTime = 5.0f;
		break;
	case 0x5E:
	{
		WVector data;
		packet.DecodeBuffer(&data, sizeof(data));

		GetActor("GolfRule") << MsgObject(0, 0x8D, (int)&data, 0, 0, 0, 0);
		break;
	}
	case 0x88:
		GetActor("GolfRule") << MsgObject(0, 0x93, 0, 0, 0, 0, 0);
		break;
	case 0x89:
	{
		unsigned long uid = packet.Decode4();
		int paused = packet.Decode1();

		if (paused != 0)
		{
			WSingleton<CChatMsg>::Instance()->AddChatMsg(
				MakeStr(
					"%s\xb4\xd4\xc0\xcc \xb0\xd4\xc0\xd3\xc0\xbb \xc0\xe1\xbd\xc3 \xc1\xdf\xc1\xf6\xbd\xc3\xc4\xd7\xbd\xc0\xb4\xcf\xb4\xd9.",
					Doc()->m_userInfo[Doc()->GetIndex(uid)].info.sNick),
				0xFFFF7878, false, false);
		}
		else
		{
			WSingleton<CChatMsg>::Instance()->AddChatMsg(
				MakeStr(
					"%s\xb4\xd4\xc0\xcc \xc1\xdf\xc1\xf6\xb5\xc8 \xb0\xd4\xc0\xd3\xc0\xbb \xc0\xe7\xb0\xb3\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					Doc()->m_userInfo[Doc()->GetIndex(uid)].info.sNick),
				0xFFFF7878, false, false);
		}

		GetActor("GolfRule") << MsgObject(0, 0x84, paused,
			(int)"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc0\xcc \xc0\xe1\xbd\xc3 \xc1\xdf\xb4\xdc\xb5\xc8 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.\n\xb0\xd4\xc0\xd3\xc0\xbb \xb0\xe8\xbc\xd3\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 \xc8\xae\xc0\xce\xc0\xbb \xb4\xad\xb7\xaf\xc1\xd6\xbc\xbc\xbf\xe4.",
			0, 0, 0);
		break;
	}
	case 0x8A:
	{
		if (CMessengerInfo::Instance()->GetMyStatus() != (MSN::eStatusType)3)
		{
			CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)4);
		}

		unsigned char finishOrder = (unsigned char)Doc()->m_rivalList.size();
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state == 0)
			{
				it->finishOrder = finishOrder--;
				it->state = 2;

				for (;; it->hole = Doc()->m_holeOrder[GetHoleIndex(it->hole)])
				{
					it->holePang[it->hole - 1] = 0;
					it->holeStroke[it->hole - 1] =
						Doc()->m_holePar[it->hole - 1] + 5;
					it->holeScore[it->hole - 1] = 5;
					it->totalScore += 5;

					if (it->hole ==
						Doc()->m_holeOrder[Doc()->m_golfGame.holes - 1])
					{
						break;
					}
				}
			}
		}
		break;
	}
	case 0x8E:
		GetActor("GolfRule") << MsgObject(0, 0x94, 0, 0, 0, 0, 0);
		break;
	case 0xA9:
	{
		int count = packet.Decode1();
		for (int i = 0; i < count; i++)
		{
			unsigned long uid = packet.Decode4();
			sWinningPrize prize;
			packet.DecodeBuffer(&prize, sizeof(prize));
			Doc()->SetWinningPrize(uid, prize);
		}
		break;
	}
	case 0x90:
		GetActor("GolfRule") << MsgObject(0, 0x95, 0, 0, 0, 0, 0);
		break;
	case 0x82:
	{
		unsigned char flags = packet.Decode1();
		unsigned char gm = flags >> 7;
		flags &= 0x7F;

		bool own = flags == 0;

		int notify;
		unsigned long color = 0xFFD2A2FF;
		if (gm)
		{
			color = 0xFF6EECFB;
		}

		std::string nickname;
		std::string text;
		std::string message;

		packet.DecodeStr(nickname);
		packet.DecodeStr(message);

		if (own)
		{
			text = MyNick();
			text += ">";
			text += nickname;
		}
		else
		{
			if (!WSingleton<COption>::Instance()->gIsWhisperAllowed())
			{
				break;
			}

			std::list<std::string>::iterator it = std::find(
				WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(),
				WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(),
				nickname);
			if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end())
			{
				break;
			}

			GetActor("GolfRule")
				<< MsgObject(0, 0x3F, (int)&notify, 0, 0, 0, 0);
			GetActor("GolfRule")
				<< MsgObject(0, 0x38, (int)nickname.c_str(), 0, 0, 0, 0);

			text = nickname + ">" + MyNick();
		}

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
			message.c_str(), false);

		text += " : \x08'";
		text += message;
		text += "'";

		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, color, false, true);
	}
	break;
	case 0xAE:
	{
		std::string nickname;
		std::string message;
		std::string text;

		packet.DecodeStr(nickname);
		packet.DecodeStr(message);

		if (!WSingleton<COption>::Instance()->gIsWhisperAllowed())
		{
			break;
		}

		std::list<std::string>::iterator it = std::find(
			WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(),
			WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickname);
		if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end())
		{
			break;
		}

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(
			message.c_str(), false);

		text = nickname + ">" + "\xc6\xc0\xbf\xa1\xb0\xd4";
		text += " : \x08";
		text += message;

		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFF00C6FF, false,
			true);
	}
	break;
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
				"\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n\n\xc6\xaf\xc1\xa4 \xbb\xf3\xb4\xeb\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xa6 \xc2\xf7\xb4\xdc \xc7\xcf\xb7\xc1\xb8\xe9 \xbb\xf3\xb4\xeb \xc1\xa4\xba\xb8\xc3\xa2\xc0\xbb \xb8\xb6\xbf\xec\xbd\xba \xb4\xf5\xba\xed \xc5\xac\xb8\xaf \xc8\xc4 \\c0xffff0000\\c[\xb4\xeb\xc8\xad \xc2\xf7\xb4\xdc \xbe\xc6\xc0\xcc\xc4\xdc]\\c0xff000000\\c\xc0\xbb \xbc\xb1\xc5\xc3 \xc7\xcf\xbd\xc3\xb8\xe9 \xb5\xcb\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open((FRESH_PFN_RESULT)0, 0);
		}
		break;
	case 0x9A:
	{
		unsigned char count = packet.Decode1();
		for (unsigned char i = 0; i < count; i++)
		{
			unsigned long arrow = packet.Decode4();
			GOLFDOC()->m_commandList.push_back(arrow);
		}

		if (OnlinePlay())
		{
			GetActor("Cutin") << MsgObject(0, 0x78, 8, 0, 0, 0, 0);
		}
		break;
	}
	case 0x97:
	{
		if (GOLFDOC()->GameMode() & 0x30E3)
		{
			break;
		}

		unsigned char count = packet.Decode1();
		std::vector<unsigned long> uids;

		uids.reserve(count);

		for (unsigned char i = 0; i < count; i++)
		{
			uids.push_back(packet.Decode4());
		}

		WSendPacket send((enumClientPacket)0x40);

		sPlayerData* pPlayer = PLAYER(0);
		sPlayerSyncData data;

		data.club = GolfClub().GetIndex();
		data.frame = GetPVS().GetFrame();
		data.angle = GOLFDOC()->m_cameraData.angle;

		data.powerShotLevel = pPlayer->bunker;
		data.comboGauge = (float)float2int(pPlayer->gauge);
		data.numItems = pPlayer->itemNum;
		data.shotItemTypeId = pPlayer->item;
		data.rivalLevel = pPlayer->flag;
		data.lastShotPos.x = pPlayer->startPos.x;
		data.lastShotPos.y = pPlayer->startPos.y;
		data.lastShotPos.z = pPlayer->startPos.z;
		data.ballPos.x = pPlayer->pos.x;
		data.ballPos.y = pPlayer->pos.y;
		data.ballPos.z = pPlayer->pos.z;
		data.powerShot = GolfClub().PowerShotType();
		data.extraPang = pPlayer->bonusPang;

		for (int hole = 0; hole < 18; hole++)
		{
			data.holes[hole] = pPlayer->stroke[hole];
			data.holeIn[hole] = pPlayer->putt[hole];
			data.holeRecord[hole] = pPlayer->pang[hole];
		}

		if (IsMassGame())
		{
			data.state = GolfBall().m_playFrame != -1;
		}
		else
		{
			data.state = pPlayer->state;
		}

		send.EncodeBuffer(&data, sizeof(data));

		send.Encode1(count);
		for (unsigned char j = 0; j < count; j++)
		{
			send.Encode4(uids[j]);
		}

		send.Send(TO_GAME);
		break;
	}
	case 0xBC:
		GetActor("GolfRule") << MsgObject(0, 0x9E, 1, 0, 0, 0, 0);
		break;
	case 0xBA:

		if (GalleryMode())
		{
			unsigned char count = packet.Decode1();

			if (count == 0x0B)
			{
				WSingleton<CChatMsg>::Instance()->AddChatMsg(
					"\xbe\xcb\xb8\xb2 : \xb5\xd1\xb7\xaf\xba\xb8\xb1\xe2\xb0\xa1 \xb0\xa1\xb4\xc9\xc7\xd1 \xb0\xd4\xc0\xd3\xc1\xdf\xc0\xce \xb9\xe6\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFFFF7878, false, false);
				break;
			}

			if (count == 0x10)
			{
				WSingleton<CChatMsg>::Instance()->AddChatMsg(
					"\xbe\xcb\xb8\xb2 : \xb7\xce\xb5\xf9\xb5\xc7\xb1\xe2 \xc0\xfc\xbf\xa1 \xb0\xd4\xc0\xd3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc9 \xbf\xec\xb7\xc1\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFFFF7878, false, false);
				break;
			}

			packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));
			Doc()->m_expRate = packet.Decode4();

			count = packet.Decode1();
			Doc()->m_slotList.clear();

			for (unsigned char i = 0; i < count; i++)
			{
				sSlotInfo slot;
				packet.DecodeBuffer(&slot, sizeof(slot));
				Doc()->m_slotList.push_back(slot);
			}

			if (IsMassGame(Doc()->m_roomInfo.gameType))
			{
				packet.DecodeBuffer(&Doc()->m_userInfo[0], sizeof(sUserInfo));

				if (IsLocalContent((localContentType_t)0x57))
				{
					RequestUccInfo(Doc()->m_userInfo[0]);
				}

				Doc()->m_myInfo.info.dwGalleryGuid =
					Doc()->m_userInfo[0].info.dwGuid;
			}
			else
			{
				if (count > 3)
				{
					break;
				}

				for (unsigned char i = 0; i < count; i++)
				{
					packet.DecodeBuffer(&Doc()->m_userInfo[i],
						sizeof(sUserInfo));

					if (IsLocalContent((localContentType_t)0x57))
					{
						RequestUccInfo(Doc()->m_userInfo[i]);
					}
				}
			}

			SetCurMap(packet.Decode1());
			SetGameType(packet.Decode1());
			SetHoleType(packet.Decode1());
			SetHoles(packet.Decode1());
			Doc()->m_roomType = packet.Decode4();
			SetShotTimeLimit(packet.Decode4());
			SetGameTimeLimit(packet.Decode4());

			for (count = 0; count < Doc()->m_golfGame.holes; count++)
			{
				Doc()->m_holeRandom[count] = packet.Decode4();
				Doc()->m_courseMap[count] =
					(unsigned char)(Doc()->m_holeRandom[count] % 3);
				CSharedDoc* doc = Doc();
				doc->m_holeOrder[count] = packet.Decode1();
			}

			GOLFDOC()->m_currentHole = packet.Decode1();
			Doc()->m_roomSetupStage = 0;

			if (AfxGetTask()->IsInited())
			{
				AfxGetTask()->SetInited(false);
				AfxGetTask()->Reload();
			}

			GetActor("GolfBg") << MsgObject(0, 0x140, 0, 0, 0, 0, 0);
			GetActor("GolfBg") << MsgObject(0, 0x13E, 0, 0, 0, 0, 0);

			GetActor("Screen") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);
		}
		else
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			Doc()->m_slotList2.clear();
			Doc()->m_slotList2.push_back(slot);
			Doc()->m_roomInfo.nGalleryNum = 1;

			GetActor("GolfRule") << MsgObject(0, 0x9E, 0, 0, 0, 0, 0);

			IsMyTurn(true);
		}
		break;
	case 0xBB:
	{
		if (!GalleryMode())
		{
			break;
		}

		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 1, 0, 0, 0);

		unsigned char intensity = packet.Decode1();
		unsigned char direction = packet.Decode1();

		Wind().Init(intensity, direction, 0, 1);

		unsigned long remainTime = Doc()->m_golfGame.shotTimeLimit;
		remainTime -= packet.Decode4();

		unsigned long turnUid = packet.Decode4();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++)
		{
			sPlayerSyncData data;
			packet.DecodeBuffer(&data, sizeof(data));

			PLAYER(i)->bunker = data.powerShotLevel;
			PLAYER(i)->gauge = data.comboGauge;
			PLAYER(i)->itemNum = data.numItems;
			PLAYER(i)->item = data.shotItemTypeId;
			PLAYER(i)->flag = data.rivalLevel;
			PLAYER(i)->startPos.x = data.lastShotPos.x;
			PLAYER(i)->startPos.y = data.lastShotPos.y;
			PLAYER(i)->startPos.z = data.lastShotPos.z;
			PLAYER(i)->pos.x = data.ballPos.x;
			PLAYER(i)->pos.y = data.ballPos.y;
			PLAYER(i)->pos.z = data.ballPos.z;

			for (unsigned char hole = 0; hole < 18; hole++)
			{
				PLAYER(i)->stroke[hole] = data.holes[hole];
				PLAYER(i)->putt[hole] = data.holeIn[hole];
				PLAYER(i)->pang[hole] = (int)data.holeRecord[hole];
			}

			PLAYER(i)->oid = Doc()->m_userInfo[i].info.dwGuid;
			PLAYER(i)->teeOrder = 0;

			PLAYER(i)->score = data.ghostResult;
			PLAYER(i)->totalStroke = data.totalScore;

			PLAYER(i)->totalPang = data.ghostAward;
			PLAYER(i)->bonusPang = (int)data.extraPang;

			PLAYER(i)->index = i;
			PLAYER(i)->holeNum = GetHoleIndex(GOLFDOC()->m_currentHole) - 1;
			PLAYER(i)->state = data.state;

			if (PLAYER(i)->oid == turnUid)
			{
				GOLFDOC()->m_currentPlayer = i;

				GolfBall().m_pos = PLAYER(i)->pos;
				GolfBall().m_meshIndex =
					GetPVS().GetMeshIndex(GolfBall().m_pos, false);

				GameCamera().angle = data.angle;

				GolfClub().SetApproachMode();
				GolfClub().SetClub(data.club);
				GolfClub().SetPowerShot(data.powerShot);
				GetPVS().SetFrame(data.frame);
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x78, 0x100000, 0, 0, 0, 0);

		GOLFDOC()->m_remainTime = remainTime;
		break;
	}
	case 0x96:
	{
		if (!GalleryMode())
		{
			break;
		}

		if (!IsMassGame())
		{
			Doc()->m_myInfo.info.dwGalleryGuid = packet.Decode4();
			break;
		}

		packet.DecodeBuffer(&Doc()->m_userInfo[0], sizeof(sUserInfo));

		if (IsLocalContent((localContentType_t)0x57))
		{
			RequestUccInfo(Doc()->m_userInfo[0]);
		}

		IFF_STRUCT::sCaddie* pCaddie =
			ItemManager()->FindCaddie(Doc()->m_userInfo[0].caddieInfo.tid);
		if ((pCaddie &&
				Doc()->m_userInfo[0].stat.Level < (pCaddie->c.Level & 0x7F)) ||
			((Doc()->m_userInfo[0].caddieInfo.Rent_flag) &&
				Doc()->m_userInfo[0].caddieInfo.Remain_Date == 0 &&
				pCaddie->MonthlyFee > 0))
		{
			Doc()->m_userInfo[0].caddieInfo.tid = 0;
		}

		IFF_STRUCT::sClubSet* pClubSet =
			ItemManager()->FindClubSet(Doc()->m_userInfo[0].clubInfo.tid);

		if (pClubSet &&
			((Doc()->m_userInfo[0].stat.Level < pClubSet->c.Level &&
				 !pClubSet->c.IsUnderLvl) ||
				(Doc()->m_userInfo[0].stat.Level > pClubSet->c.Level &&
					pClubSet->c.IsUnderLvl)))
		{
			Doc()->m_userInfo[0].clubInfo.tid = 0x10000000;
		}

		unsigned long items[10];
		memcpy(items, Doc()->m_userInfo[0].userEquip.tidItemSlot,
			sizeof(items));

		unsigned char used = 0;
		for (unsigned char i = 0; i < 10; i++)
		{
			if (items[i] != 0)
			{
				Doc()->m_userInfo[0].userEquip.tidItemSlot[used++] = items[i];
			}
		}

		for (; used < 10; used++)
		{
			Doc()->m_userInfo[0].userEquip.tidItemSlot[used] = 0;
		}

		Doc()->m_myInfo.info.dwGalleryGuid = Doc()->m_userInfo[0].info.dwGuid;

		if (WSingleton<CHeadIcon>::Instance())
		{
			WSingleton<CHeadIcon>::Instance()->SetLevel();
		}

		unsigned char hole = packet.Decode1();

		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		unsigned char intensity = packet.Decode1();
		unsigned char direction = packet.Decode1();
		unsigned char windType = packet.Decode1();

		sPlayerSyncData data;
		packet.DecodeBuffer(&data, sizeof(data));

		GolfClub();

		GetActor("Player") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);

		GetActor("Screen") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);

		GetActor("Player") << MsgObject(0, 0x11C,
			Doc()->m_userInfo[0].clubInfo.tid, 0, 0, 0, 0);

		GetActor("Ball") << MsgObject(0, 0x4C,
			Doc()->m_userInfo[0].userEquip.tidBall, 0, 0, 0, 0);
		GetActor("Screen") << MsgObject(0, 0x4C,
			Doc()->m_userInfo[0].userEquip.tidBall, 0, 0, 0, 0);

		GetActor("Caddie") << MsgObject(0, 0x4A,
			Doc()->m_userInfo[0].caddieInfo.tid,
			Doc()->m_userInfo[0].caddieInfo.Level - 1,
			Doc()->m_userInfo[0].caddieInfo.tidPart, 0, 0);

		GetActor("Mascot") << MsgObject(0, 0x4E,
			Doc()->m_userInfo[0].mascotInfo.guid,
			(int)&Doc()->m_userInfo[0].mascotInfo, 0, 0, 0);

		GOLFDOC()->SetPlayerLevel(0);

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 1, 0, 0, 0);

		GOLFDOC()->m_currentHole = hole;
		GetActor("GolfBg") << MsgObject(0, 0x1E8, 0, 0, 0, 0, 0);

		Wind().Init(intensity, direction, windType, 1);

		sPlayerData* pPlayer = PLAYER(0);

		pPlayer->bunker = data.powerShotLevel;
		pPlayer->gauge = data.comboGauge;
		pPlayer->itemNum = data.numItems;
		pPlayer->item = data.shotItemTypeId;
		pPlayer->flag = data.rivalLevel;
		pPlayer->startPos.x = data.lastShotPos.x;
		pPlayer->startPos.y = data.lastShotPos.y;
		pPlayer->startPos.z = data.lastShotPos.z;
		pPlayer->pos.x = data.ballPos.x;
		pPlayer->pos.y = data.ballPos.y;
		pPlayer->pos.z = data.ballPos.z;

		for (unsigned char i = 0; i < 18; i++)
		{
			pPlayer->stroke[i] = data.holes[i];
			pPlayer->putt[i] = data.holeIn[i];
			pPlayer->pang[i] = (int)data.holeRecord[i];
		}

		GolfBall().m_pos = pPlayer->pos;
		GolfBall().m_meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

		switch (data.state)
		{
		case 0:
			GameCamera().angle = data.angle;
			GolfClub().SetApproachMode();
			break;
		case 2:
			GolfBall().m_pos = GetTeeShotPos();
			GolfBall().m_pos.y += GolfBall().m_radius;
			pPlayer->pos = GolfBall().m_pos;
			pPlayer->startPos = GolfBall().m_pos;
		case 1:
			GolfClub().SelectBestClub();
			GOLFDOC()->m_cameraData.angle = CalcDeltaAngle(
				GOLFDOC()->m_pAutoTarget->GetPoint(0) - GolfBall().m_pos,
				WVector::UNIT_POS_Z);
			break;
		}

		pPlayer->oid = Doc()->m_myInfo.info.dwGalleryGuid;

		pPlayer->teeOrder = 0;
		pPlayer->score =
			Doc()->m_rivalList[Doc()->GetIndex(pPlayer->oid)].totalScore;
		pPlayer->totalStroke =
			Doc()->m_rivalList[Doc()->GetIndex(pPlayer->oid)].totalStroke;

		pPlayer->totalPang = (int)data.holeRecord[hole - 1] +
			(int)Doc()->m_rivalList[Doc()->GetIndex(pPlayer->oid)].totalPang;
		pPlayer->bonusPang = (int)data.extraPang;

		pPlayer->index =
			Doc()->m_rivalList[Doc()->GetIndex(pPlayer->oid)].order;

		pPlayer->holeNum = GetHoleIndex(hole) - 1;
		pPlayer->state = 0;

		GolfClub().SetClub(data.club);
		GolfClub().SetPowerShot(data.powerShot);

		GetActor("GolfRule") << MsgObject(0, 0x78, 0x100000, 0, 0, 0, 0);

		GetPVS().SetFrame(data.frame);
		break;
	}
	case 0x99:
	{
		if (!IsMassGame())
		{
			break;
		}

		unsigned long uid = packet.Decode4();
		Doc()->m_rivalList[Doc()->GetIndex(uid)].observerState =
			packet.Decode4();
		break;
	}
	case 0x95:
		Doc()->m_roomType = packet.Decode4();
		break;
	case 0xA1:
	{
		unsigned long uid = packet.Decode4();
		unsigned char progress = packet.Decode1();

		if (uid == MyGuid(false))
		{
			break;
		}

		UpdateLoadingInfo(uid, progress);
		break;
	}
	case 0xA2:
	{
		if (IsMyTurn(false))
		{
			unsigned short count = packet.Decode2();
			unsigned long typeId;
			if (IsLocalContent((localContentType_t)0x4F))
			{
				typeId = packet.Decode4();
			}
			else
			{
				typeId = 0x1A000000;
			}

			std::list<sItemInfo>::iterator it = std::find(
				Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(), typeId);

			if (it != Doc()->m_myItemList.end())
			{
				(*it).Common[0] = count;
				if (count == 0)
				{
					Doc()->m_myItemList.erase(it);
				}
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x97, 0, 0, 0, 0, 0);
		break;
	}
	case 0xAC:
	{
		std::list<sSlotInfo>& gallery = Doc()->m_slotList2;
		std::list<sSlotInfo>::iterator it;

		unsigned char type = packet.Decode1();

		switch (type)
		{
		case 0:
		{
			gallery.clear();

			unsigned char count = packet.Decode1();

			sSlotInfo slot;
			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&slot, sizeof(slot));
				gallery.push_back(slot);
			}
			break;
		}
		case 1:
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			it = std::find(gallery.begin(), gallery.end(), slot.dwGuid);
			if (it == gallery.end())
			{
				gallery.push_back(slot);
			}
			break;
		}
		case 2:
		{
			unsigned long uid = packet.Decode4();

			it = std::find(gallery.begin(), gallery.end(), uid);
			if (it != gallery.end())
			{
				gallery.erase(it);
			}
			break;
		}
		}

		Doc()->m_roomInfo.nGalleryNum = (unsigned char)gallery.size();
		break;
	}
	case 0x46:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;
		std::list<sSlotInfo>::iterator it;

		unsigned char type = packet.Decode1();
		packet.Decode2();

		switch (type)
		{
		case 0:
		case 5:
		{
			if (type == 0)
			{
				players.clear();
			}

			unsigned char count = packet.Decode1();

			sSlotInfo slot;
			sCharacterInfo characterInfo;

			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&slot, sizeof(slot));
				players.push_back(slot);

				packet.DecodeBuffer(&characterInfo, sizeof(characterInfo));
				if (IsLocalContent((localContentType_t)0x57))
				{
					RequestUccInfo(characterInfo);
				}
			}
			break;
		}
		case 1:
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			if (!IsMassGame())
			{
				sRoomSlot roomSlot;

				sCharacterInfo characterInfo;
				packet.DecodeBuffer(&characterInfo, sizeof(characterInfo));
				if (IsLocalContent((localContentType_t)0x57))
				{
					RequestUccInfo(characterInfo);
				}

				roomSlot.charInfo = characterInfo;
				Doc()->m_roomSlotMap[slot.dwGuid] = roomSlot;
			}

			it = std::find(players.begin(), players.end(), slot.dwGuid);
			if (it == players.end())
			{
				players.push_back(slot);
			}
			break;
		}
		case 2:
		{
			unsigned long uid = packet.Decode4();

			std::map<unsigned long, sRoomSlot>::iterator slot =
				Doc()->m_roomSlotMap.find(uid);
			if (slot != Doc()->m_roomSlotMap.end())
			{
				if ((*slot).second.pExhibition)
				{
					delete (*slot).second.pExhibition;
					(*slot).second.pExhibition = 0;
				}
				Doc()->m_roomSlotMap.erase(slot);
			}

			it = std::find(players.begin(), players.end(), uid);
			if (it != players.end())
			{
				unsigned char connection = it->connectionRank;

				players.erase(it);

				for (it = players.begin(); it != players.end(); ++it)
				{
					if (it->connectionRank > connection)
					{
						it->connectionRank--;
					}
				}
			}
			break;
		}
		case 3:
		{
			sSlotInfo slot;
			unsigned long uid = packet.Decode4();

			packet.DecodeBuffer(&slot, sizeof(slot));

			it = std::find(players.begin(), players.end(), uid);

			std::map<unsigned long, sRoomSlot>::iterator entry =
				Doc()->m_roomSlotMap.find(uid);
			if (entry != Doc()->m_roomSlotMap.end())
			{
				(*entry).second.bGachaWing = slot.angelicWingsEffect;
				(*entry).second.bAngelWing = slot.angelicWings;
			}
			break;
		}
		}

		Doc()->m_roomInfo.nUserNum = (unsigned char)players.size();
		break;
	}
	case 0xAA:
	{
		unsigned long uid = packet.Decode4();
		unsigned char penalty = packet.Decode1();

		if (uid == MyGuid(false))
		{
			break;
		}

		if (Doc()->AreWePlayingTogether(uid))
		{
			PLAYER(Doc()->GetIndex(uid))->chatBlock = penalty;
		}
		break;
	}
	case 0xAB:
	{
		std::string name;
		packet.DecodeStr(name);
		unsigned char channelId = packet.Decode1();

		std::list<sChannelInfo>::iterator it =
			std::find(Doc()->m_channelList.begin(), Doc()->m_channelList.end(),
				channelId);
		char text[128];
		if (it == Doc()->m_channelList.end())
		{
			sprintf(text,
				"%s\xb4\xd4\xc0\xba [%s]\xbf\xa1\xbc\xad \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				name.c_str(), Doc()->m_curGameServer.name);
		}
		else
		{
			sprintf(text,
				"%s\xb4\xd4\xc0\xba [%s]\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd8 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				name.c_str(), it->Name);
		}

		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF7878, false,
			false);
		break;
	}
	case 0x76:
	{
		unsigned long uid = packet.Decode4();
		int state = packet.Decode1();

		std::list<sSlotInfo>::iterator it =
			std::find(Doc()->m_slotList.begin(), Doc()->m_slotList.end(), uid);

		if (it != Doc()->m_slotList.end())
		{
			it->bReady = (state == 0);
		}
		break;
	}
	case 0xC0:
	{
		unsigned char point[2];
		unsigned long uid = packet.Decode4();
		Doc()->m_guildScore[0] = packet.Decode2();
		Doc()->m_guildScore[1] = packet.Decode2();
		point[0] = packet.Decode1();
		point[1] = packet.Decode1();

		sRivalData* pResult = &Doc()->m_rivalList[Doc()->GetIndex(uid)];
		sRivalData* pRival = Doc()->GetRival(uid);
		if (pResult && pRival)
		{
			GetActor("Score") << MsgObject(0, 0x57, Doc()->m_guildScore[0],
				Doc()->m_guildScore[1], 0, 0, 0);
			GetActor("Screen") << MsgObject(0, 0x1CE, Doc()->m_guildScore[0],
				Doc()->m_guildScore[1], 0, 0, 0);

			pResult->guildPoint = point[0];
			pRival->guildPoint = point[1];
		}
		break;
	}
	case 0xCA:
	{
		sPrizeInfo prizes[128] = {};
		unsigned long uid = packet.Decode4();
		int count = packet.Decode1();

		if (count > 0)
		{
			packet.DecodeBuffer(prizes, sizeof(prizes));
			Doc()->SetExtPrize(uid, count, prizes);
		}
		break;
	}
	case 0x147:
	{
		char desc[128] = "\0";
		unsigned char missionType = packet.Decode1();
		unsigned char prizeType = 0;
		unsigned char winnerType = 0;

		if (missionType != 0)
		{
			prizeType = packet.Decode1();
			winnerType = packet.Decode1();

			unsigned long param1 = packet.Decode4();
			unsigned long param2 = packet.Decode4();
			std::string text("\0");
			packet.DecodeStr(text);

			CApproachDesc approachDesc(missionType, param1, param2,
				text.c_str());
			char* pDesc = approachDesc.GetDesc();

			if (pDesc)
			{
				strcpy(desc, pDesc);
			}
		}

		GOLFDOC()->SetApproachMissionType((eApproachMissionType)missionType);
		GOLFDOC()->SetApproachPrizeType((eApproachPrizeType)prizeType);
		GOLFDOC()->SetApproachWinnerType((eApproachWinnerType)winnerType);
		GOLFDOC()->SetApproachMissionDesc(desc);
		break;
	}
	case 0x148:
	{
		unsigned long winnerUid = 0xFFFFFFFF;

		GOLFDOC()->ClearApproachCurHoleResult();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++)
		{
			sApproachResultData result;
			packet.DecodeBuffer(&result, sizeof(result));

			if (result.byRank == 1)
			{
				winnerUid = result.dwGUID;
			}

			if (result.dwGUID == Doc()->m_myInfo.info.dwGuid)
			{
				GOLFDOC()->AddApproachTotalPrize(result.dwPrizeCount);
				GOLFDOC()->AddApproachTotalPrize(result.byLuckPrize);
				GOLFDOC()->AddApproachTotalPrize(result.byRankPrize);
			}

			GOLFDOC()->SetApproachCurHoleResult(result);
		}

		if (winnerUid != 0xFFFFFFFF)
		{
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				Doc()->m_briefUserInfoMap.find(winnerUid);

			if (it != Doc()->m_briefUserInfoMap.end())
			{
				WSendPacket request((enumClientPacket)0x2f);
				request.Encode4(it->second.dwUid);
				request.Encode1(5);
				request.Send(TO_GAME);

				Doc()->m_userInfoMod = 1;
			}
		}
		break;
	}
	case 0x146:
	{
		GOLFDOC()->ClearApproachGameResult();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++)
		{
			sApproachResultData result;
			packet.DecodeBuffer(&result, sizeof(result));
			GOLFDOC()->SetApproachGameResult(result);
		}
		break;
	}
	case 0x149:
	{
		if (CMessengerInfo::Instance()->GetMyStatus() != (MSN::eStatusType)3)
		{
			CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)4);
		}

		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state == 0)
			{
				it->state = 2;
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0xA6, 0, 0, 0, 0, 0);
		Doc()->SetAppGiftDownloadState((CSharedDoc::eAppGiftDownloadState)2);
		break;
	}
	case 0x192:
	{
		std::string imageName;
		CUTIN_ANI_STYPEINFO aniInfo;
		unsigned char result = packet.Decode1();

		if (result == 1)
		{
			unsigned long skinId = packet.Decode4();
			packet.Decode4();
			packet.DecodeBuffer(&aniInfo, sizeof(aniInfo));

			if (COption::Instance()->gGetCutinFlag() == 1)
			{
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(skinId);
				IFF_STRUCT::S5::sCutinInformation* pCutin =
					ItemManager()->FindCutinInformation(pSkin->c.TypeId);

				if (pCutin)
				{
					char text[64] = "";
					imageName = pCutin->Char_Tex;

					GetActor("Cutin") << MsgObject(0, 0x78, 4, 0, 0, 0, 0);

					strcmpi(imageName.c_str(), aniInfo.szCharTex);

					sprintf(text, "%s", imageName.c_str());

					GetActor("Cutin") << MsgObject(0, 0x24C, (int)text,
						(int)&aniInfo, pCutin->Condition, skinId, 0);
					break;
				}
			}
		}

		if (result != 1)
		{
			int mode = packet.Decode2();
			if (mode == 0)
			{
				GetActor("GolfRule")
					<< MsgObject(0, 0x24D, 0, (int)"NON CUTIN_MODE", 0, 0, 0);
			}
			else
			{
				GetActor("GolfRule") << MsgObject(0, 0x24D, 0,
					(int)"GST_RESPONSE_CUTIN_FAILED", 0, 0, 0);
			}
		}
		else
		{
			GetActor("GolfRule") << MsgObject(0, 0x24D, 0,
				(int)"GST_RESPONSE_CUTIN_FAILED", 0, 0, 0);
		}
		break;
	}
	default:
		return OnPacketCommon(packet);
	}

	return 1;
}

bool CGolfTask::DetermineWhetherToAddActor_GroundItemMan(void)
{
	bool bAdd = false;
	switch (Doc()->m_golfGame.gameType)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
		if (Doc()->m_playingMode != 2 && Doc()->m_gameMode != 1)
		{
			bAdd = true;
		}
		break;
	}

	return bAdd;
}
