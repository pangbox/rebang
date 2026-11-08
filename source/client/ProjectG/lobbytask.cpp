#include "stdafx.h"
#include "ggc_wrapper.h"

#include "networksystem.h"
#include <windows.h>
#include <mmsystem.h>

#include "clientsetting.h"
#include "lobbytask.h"
#include "objectfactory.h"
#include "actor.h"
#include "fresh.h"
extern Fresh* g_pFresh;
#include "packet.h"
#include "shareddoc.h"
#include "contentsdoc.h"
#include "golfdoc.h"
#include "singleton.h"
#include "user_info.h"
#include "logininfo.h"
#include "commonutil.h"
#include "../../shared/localize.h"
#include "dlgquickstartsetting.h"
#include "messengerdlg.h"
#include "ucclibrary.h"
void RequestUccInfo(sCharacterInfo& character);
void RequestUccInfo(sUserInfo& user);
void RequestUccInfo(sUserInfoTime& user);
void RequestUccInfo(sRoomSlot& slot);
#include "intrusion.h"
#include "mousecursor.h"
#include "projectg.h"
#include "chatmsg.h"
#include "detailedroominfodlg.h"
#include "frticketexchangedlg.h"
#include "s5/utilities.h"

#include "guilddefine.h"
#include "guildactor.h"
#include "gatewayactor.h"

#include <algorithm>

extern HWND g_hwnd;

extern int g_iRecvPacketType;

void RecvRSS(int)
{
}

IMPLEMENT_OBJECT(CLobbyTask, CTask)

CLobbyTask::CLobbyTask(void)
{
	m_bSyncLoad = true;
	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = 0;
}

CLobbyTask::~CLobbyTask(void)
{
}

void CLobbyTask::Init(const char* backGround)
{
	CTask::Init(NULL);

	SendMessageA(g_hwnd, 0xADB4, 0, 0);
}

void CLobbyTask::Register(void)
{
	SetMainActor(AddActor("CLobbyMain", "Lobby", 0, ""));

	AddActor("ntLobbyBg", "LobbyBg", 0, "");
	GOLFDOC()->m_currentHole = 0x13;

	AddActor("CNPC", "NPC", 0, "");

	AddActor(s_guildActor[0], s_guildActor[1], 0, "");

	AddActor(s_gatewayActor[0], s_gatewayActor[1], 0, GetRTTI()->GetName());
}

void CLobbyTask::Load(void)
{
	SetProgressBar(false);
	SetTip(false);

	CTask::Load();
}

void CLobbyTask::Process(float dt)
{
	g_pFresh->OnProcess(dt);
	CTask::Process(dt);
}

void CLobbyTask::Display(void)
{
	if (!Doc()->m_bBackgroundVideo)
	{
		CTask::Display();
	}

	g_pFresh->OnDisplay(Doc()->m_bBackgroundVideo != 0);
}

int CLobbyTask::OnPacket(WReceivedPacket& packet)
{
	g_iRecvPacketType = packet.Decode2();

	switch (g_iRecvPacketType)
	{
	case 0xC2:
	{
		unsigned long guid = packet.Decode4();
		unsigned char kind = packet.Decode1();

		if (guid == Doc()->m_myInfo.info.dwGuid)
		{
			break;
		}

		switch (kind)
		{
		case 0:
		{
			unsigned long value;
			packet.DecodeBuffer(&value, sizeof(value));
			GetActor("Lobby")
				<< MsgObject(0, 0x5B, guid, kind, (intptr_t)&value, 0, 0);
			break;
		}
		case 1:
		{
			std::string text;
			packet.DecodeStr(text);
			GetActor("Lobby")
				<< MsgObject(0, 0x5B, guid, kind, (intptr_t)text.c_str(), 0, 0);
			break;
		}
		case 2:
		case 3:
			GetActor("Lobby") << MsgObject(0, 0x5B, guid, kind, 0, 0, 0);
			break;
		}
		break;
	}
	case 0x11B:
	{
		unsigned char index = packet.Decode1();
		packet.Decode1();

		if (index == 3)
		{
			packet.DecodeBuffer(Doc()->m_tutorialComplete,
				sizeof(Doc()->m_tutorialComplete));
		}
		else if (index < 3)
		{
			Doc()->m_tutorialComplete[index] = packet.Decode4();
		}
		break;
	}
	case 0xB0:
	{
		Doc()->m_noteList.clear();

		unsigned char count = packet.Decode1();
		for (unsigned char i = 0; i < count; i++)
		{
			sNoteInfo note;
			packet.DecodeBuffer(&note, 0x6D);

			Doc()->m_chatManager.FilteringHack(note.sNote, false);
			Doc()->m_noteList.push_back(note);
		}
		break;
	}
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

		std::string nickname, message;
		std::string text;

		packet.DecodeStr(nickname);
		packet.DecodeStr(message);

		Doc()->m_chatManager.FilteringHack(message.c_str(), false);

		if (own)
		{
			text = Doc()->m_myInfo.info.sNick;
			text += ">";
			text += nickname;
		}
		else
		{
			if (!WSingleton<COption>::Instance()->gIsWhisperAllowed())
			{
				break;
			}

			std::list<std::string>::iterator it =
				std::find(Doc()->m_ignoreList.begin(),
					Doc()->m_ignoreList.end(), nickname);
			if (it != Doc()->m_ignoreList.end())
			{
				break;
			}

			GetActor("Lobby")
				<< MsgObject(0, 0x3F, (intptr_t)&notify, 0, 0, 0, 0);
			GetActor("Lobby")
				<< MsgObject(0, 0x38, (intptr_t)nickname.c_str(), 0, 0, 0, 0);

			text = nickname + ">" + MyNick();
		}

		text += " : \x08'";
		text += message;
		text += "'";

		if (AfxGetTask()->IsRecieveWhisper())
		{
			std::string plain;
			WSingleton<CChatMsg>::Instance()->RemoveColor(text, plain);
			GetActor("Lobby")
				<< MsgObject(0, 3, (intptr_t)plain.c_str(), color, 0, 0, 0);
		}
		else
		{
			WSendPacket send((enumClientPacket)0xE0);
			send.EncodeStr(nickname);
			send.Send(TO_GAME);
		}
		break;
	}
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
			Doc()->m_ignoreList.begin(), Doc()->m_ignoreList.end(), nickname);
		if (it != Doc()->m_ignoreList.end())
		{
			break;
		}

		Doc()->m_chatManager.FilteringHack(message.c_str(), false);

		text = nickname + ">" + "\xc6\xc0\xbf\xa1\xb0\xd4";
		text += " : ";
		text += message;

		std::string plain;
		WSingleton<CChatMsg>::Instance()->RemoveColor(text, plain);
		GetActor("Lobby") << MsgObject(0, 3, (intptr_t)plain.c_str(),
			0xFF00C6FF, 0, 0, 0);
		break;
	}
	case 0x48:
	{
		unsigned short changes = packet.Decode2();

		std::string title, password;

		Doc()->m_roomInfo.gameType = packet.Decode1();
		Doc()->m_roomInfo.mapType = packet.Decode1();
		Doc()->m_roomInfo.nHole = packet.Decode1();
		Doc()->m_roomInfo.holeType = packet.Decode1();
		Doc()->m_roomInfo.nUserLimit = packet.Decode1();
		Doc()->m_roomInfo.nGalleryLimit = packet.Decode1();
		Doc()->m_roomInfo.bSleep = packet.Decode1() == 1;
		Doc()->m_roomInfo.shotTimeLimit = packet.Decode4();
		Doc()->m_roomInfo.gameTimeLimit = packet.Decode4();
		Doc()->m_roomInfo.tidMatch = packet.Decode4();

		packet.DecodeStr(password);
		memset(Doc()->m_roomInfo.password, 0,
			sizeof(Doc()->m_roomInfo.password));
		strncpy(Doc()->m_roomInfo.password, password.c_str(),
			sizeof(Doc()->m_roomInfo.password) - 1);

		packet.DecodeStr(title);
		Doc()->m_chatManager.FilteringHack(title.c_str(), false);
		memset(Doc()->m_roomInfo.title, 0, sizeof(Doc()->m_roomInfo.title));
		strncpy(Doc()->m_roomInfo.title, title.c_str(),
			sizeof(Doc()->m_roomInfo.title) - 1);

		Doc()->m_roomInfo.bPublic = strlen(Doc()->m_roomInfo.password) == 0;

		if (strlen(title.c_str()) > strlen(Doc()->m_roomInfo.title) ||
			strlen(password.c_str()) > strlen(Doc()->m_roomInfo.password))
		{
			int titleLength = strlen(title.c_str());
			int passwordLength = strlen(password.c_str());

			WSingleton<CProjectG>::Instance()->ReportError(6,
				"[%d_%d]_%d_%d_%d_%d_%d_%d(%d_%d)", titleLength, passwordLength,
				changes, Doc()->m_roomInfo.gameType,
				Doc()->m_roomInfo.realGameType, Doc()->m_roomInfo.mapType,
				Doc()->m_roomInfo.nHole, Doc()->m_roomInfo.nUserLimit,
				Doc()->m_roomInfo.roomGuid, Doc()->m_myInfo.info.dwGuid);
		}

		int refresh;
		GetActor("Lobby") << MsgObject(0, 0x17, (intptr_t)&refresh, 0, 0, 0, 0);
		if (g_pFresh->IsCurrentLayout("GAMEROOM") ||
			g_pFresh->IsCurrentLayout("GAMEROOM_EXT") || refresh == 0)
		{
			GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);
			GetActor("Lobby") << MsgObject(0, 0x10, 0, 0, 0, 0, 0);
			GetActor("Lobby")
				<< MsgObject(0, 0x11, Doc()->m_roomInfo.mapType, 0, 0, 0, 0);
			GetActor("Lobby")
				<< MsgObject(0, 0x12, Doc()->m_roomInfo.holeType, 0, 0, 0, 0);
			GetActor("Lobby") << MsgObject(0, 0x2F, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x47:
	{
		WSingleton<CIntrusion>::Instance()->FinishGame();

		GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		if (IsLocalContent((localContentType_t)0x54))
		{
			Doc()->m_inviteMode = 0;
		}

		unsigned char result = packet.Decode1();
		if (result == 0)
		{
			if (packet.Decode1())
			{
				Doc()->m_myInfo.info.dwIdentity |= 2;
				Doc()->m_myInfo.info.dwGalleryGuid = packet.Decode4();
			}
			else
			{
				Doc()->m_myInfo.info.dwIdentity &= ~2;
				Doc()->m_myInfo.info.dwGalleryGuid = 0xFFFFFFFF;
			}

			Doc()->m_slotList.clear();
			Doc()->m_rivalList.clear();
			Doc()->m_guildMatchupList.clear();
			Doc()->m_bGameOver = false;

			packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));

			Doc()->m_chatManager.FilteringHack(Doc()->m_roomInfo.title, false);
			Doc()->m_myInfo.roomIndex = Doc()->m_roomInfo.roomGuid;

			std::map<unsigned long, sUserInfoTime>::iterator info;
			if (Doc()->FindUserInfoTimeGUID(Doc()->m_myInfo.info.dwGuid, info))
			{
				info->second.info.roomIndex = Doc()->m_roomInfo.roomGuid;
			}

			const char* layout;
			if (IsMassGame(Doc()->m_roomInfo.gameType))
			{
				GetActor("Lobby") << MsgObject(0, 0x2E, 0, 0, 0, 0, 0);
				layout = "GAMEROOM_EXT";
			}
			else
			{
				layout = "GAMEROOM";
			}

			Doc()->m_chatLineList.clear();

			if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
					WNetworkSystem::NET_MSN) &&
				WSingleton<CMessengerInfo>::Instance()->GetComplete() == 4)
			{
				sUserPosition position;
				position.iServerGUID = Doc()->m_gameServerUID;
				position.iChannelUid = Doc()->m_curChannel.Uid;
				wsprintfA(position.csChannelName, "%s",
					Doc()->m_curChannel.Name);
				position.iRoomIdx = Doc()->m_roomInfo.roomGuid;
				position.iRoomType = Doc()->m_roomInfo.gameType;

				WSendPacket send(0x23);
				send.EncodeBuffer(&position, sizeof(position));
				send.Send(TO_MSN);
			}

			if (Doc()->m_roomInfo.gameType == 2)
			{
				WSingleton<CMouseCursor>::Instance()->SetActive(false);
				WSingleton<COption>::Instance()->vSetMinimumScreenSize(1, 800,
					600);
				WSingleton<CTaskManager>::Instance()->ChangeTask(
					"ntAvatarChatTask", "CGolfDoc", false);
				WSingleton<CTaskManager>::Instance()->PostMsg(0, "AvatarChat",
					0, (intptr_t)"AVATARCHAT_MAIN", 0, 0, 0);
				break;
			}

			if (Doc()->m_roomInfo.realGameType == 14 &&
				Doc()->m_roomInfo.masterUID == Doc()->m_myInfo.info.dwUID)
			{
				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_myItemList.begin();
					it != Doc()->m_myItemList.end(); ++it)
				{
					if (it->tid == 0x1A0000F7 && it->Common[0] > 0)
					{
						it->Common[0]--;
						if (it->Common[0] < 1)
						{
							Doc()->m_myItemList.erase(it);
						}
						break;
					}
				}
			}

			// HACK! There is probably some dead code around here. This hack is a 3-in-1:
			// - Eats at c2 inline budget
			// - Adjusts symbol numbering
			// - Brings in g_EPSILON constant
			if (false)
			{
				(void)&g_EPSILON;
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
				Doc()->m_slotList.begin();
			}

			GetActor("Lobby") << MsgObject(0, 0, (intptr_t)layout, 0, 0, 0, 0);
		}
		else
		{
			GetActor("Lobby") << MsgObject(0, 0x3B, result, 0, 0, 0, 0);
		}
		break;
	}
	case 0x80:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;

		std::list<sSlotInfo>::iterator it;
		unsigned long uid = packet.Decode4();

		if (uid != 0xFFFFFFFF)
		{
			if (uid == Doc()->m_myInfo.info.dwGuid)
			{
				AfxPostMsg(0, "Lobby", 2, 0, 0, 0, 0);
				AfxPostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
			}
			else
			{
				it = std::find(players.begin(), players.end(), uid);
				if (it != players.end())
				{
					GetActor("Lobby")
						<< MsgObject(0, 8, (intptr_t)&*it, 0, 0, 0, 0);
					players.erase(it);
				}
			}
		}

		GetActor("Lobby") << MsgObject(0, 0x41, (intptr_t)&*it, 0, 0, 0, 0);
		break;
	}
	case 0xA6:
	{
		unsigned long uid = packet.Decode4();

		std::list<sSlotInfo>& players = Doc()->m_slotList;

		for (std::list<sSlotInfo>::iterator it = players.begin();
			it != players.end();)
		{
			if (it->dwGuid == uid || it->IsIdentity(4))
			{
				++it;
			}
			else
			{
				GetActor("Lobby")
					<< MsgObject(0, 8, (intptr_t)&*it, 0, 0, 0, 0);
				it = players.erase(it);
			}
		}

		Doc()->m_roomInfo.nUserNum = (unsigned char)players.size();

		GetActor("Lobby") << MsgObject(0, 0x2F, 0, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x10, 0, 0, 0, 0, 0);
		break;
	}
	case 0xAC:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList2;

		unsigned char type = packet.Decode1();
		switch (type)
		{
		case 0:
		{
			players.clear();

			unsigned char count = packet.Decode1();
			sSlotInfo slot;
			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&slot, sizeof(slot));
				players.push_back(slot);
			}

			GetActor("Lobby") << MsgObject(0, 0x2F, 0, 0, 0, 0, 0);
			GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
			if (Doc()->m_slotList.size() == 0)
			{
				GetActor("Lobby") << MsgObject(0, 0x0E, 0, 0, 0, 0, 0);
			}
			break;
		}
		case 1:
		{
			sSlotInfo slot;
			std::list<sSlotInfo>::iterator it;
			packet.DecodeBuffer(&slot, sizeof(slot));

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

			std::list<sSlotInfo>::iterator it;
			it = std::find(players.begin(), players.end(), uid);
			if (it != players.end())
			{
				players.erase(it);
			}
			break;
		}
		}

		GetActor("Lobby") << MsgObject(0, 0x10, 0, 0, 0, 0, 0);
		break;
	}
	case 0x46:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;

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
				GetActor("Lobby") << MsgObject(0, 0x59, 0, 0, 0, 0, 0);
			}

			unsigned char count = packet.Decode1();
			sSlotInfo slot;
			sCharacterInfo character;
			for (int i = 0; i < count; i++)
			{
				packet.DecodeBuffer(&slot, sizeof(slot));
				players.push_back(slot);

				if (!IsMassGame(Doc()->m_roomInfo.gameType))
				{
					packet.DecodeBuffer(&character, sizeof(character));
					if (IsLocalContent((localContentType_t)0x57))
					{
						RequestUccInfo(character);
					}
					GetActor("Lobby") << MsgObject(0, 0x5A, (intptr_t)&slot,
						(intptr_t)&character, 0, 0, 0);
				}
			}

			if (IsLocalContent((localContentType_t)0x54))
			{
				unsigned char extraCount = packet.Decode1();
				for (int i = 0; i < extraCount; i++)
				{
					packet.DecodeBuffer(&slot, sizeof(slot));
					players.push_back(slot);
				}
			}

			if (type == 0)
			{
				std::map<unsigned int, sCaddieInfo>::iterator caddie;

				CSharedDoc* doc = Doc();
				std::map<unsigned int, sCaddieInfo>& caddies = doc->m_caddieMap;
				caddie = caddies.find(doc->m_myInfo.userEquip.guidCaddie);

				if (caddie == caddies.end())
				{
					GetActor("Lobby") << MsgObject(0, 0x13, 0, 0, 0, 0, 0);
				}
				else
				{
					GetActor("Lobby")
						<< MsgObject(0, 0x13, caddie->second.tid, 1, 0, 0, 0);
				}
				GetActor("Lobby") << MsgObject(0, 0x14,
					Doc()->m_myInfo.userEquip.guidClubSet, 0, 0, 0, 0);
				GetActor("Lobby") << MsgObject(0, 0x15,
					Doc()->m_myInfo.userEquip.tidBall, 0, 0, 0, 0);
			}

			GetActor("Lobby") << MsgObject(0, 0x0E, 0, 0, 0, 0, 0);
			break;
		}
		case 1:
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			std::list<sSlotInfo>::iterator it;
			it = std::find(players.begin(), players.end(), slot.dwGuid);
			if (it != players.end())
			{
				break;
			}

			players.push_back(slot);

			it = --players.end();
			if (IsMassGame(Doc()->m_roomInfo.gameType))
			{
				GetActor("Lobby")
					<< MsgObject(0, 7, (intptr_t)&*it, 0, 0, 0, 0);
			}
			else
			{
				sCharacterInfo character;

				character.guid = slot.dwGuid;
				if (!slot.IsInvite)
				{
					packet.DecodeBuffer(&character, sizeof(character));
					if (IsLocalContent((localContentType_t)0x57))
					{
						RequestUccInfo(character);
					}
				}

				GetActor("Lobby") << MsgObject(0, 7, (intptr_t)&*it,
					(intptr_t)&character, 0, 0, 0);
			}
			break;
		}
		case 2:
		{
			unsigned long uid = packet.Decode4();

			std::list<sSlotInfo>::iterator it;
			it = std::find(players.begin(), players.end(), uid);
			if (it == players.end())
			{
				break;
			}

			sSlotInfo* player = &*it;
			if (player == NULL)
			{
				players.erase(it);
				break;
			}

			unsigned long playerUid = player->dwGuid;
			unsigned char team = player->bTeam & 3;
			unsigned char connection = player->connectionRank;

			players.erase(it);

			for (it = players.begin(); it != players.end(); ++it)
			{
				if (it->connectionRank > connection)
				{
					it->connectionRank--;
				}
			}

			GetActor("Lobby")
				<< MsgObject(0, 8, (intptr_t)player, playerUid, team, 0, 0);
			break;
		}
		case 3:
		{
			sSlotInfo slot;
			unsigned long uid = packet.Decode4();
			packet.DecodeBuffer(&slot, sizeof(slot));

			std::list<sSlotInfo>::iterator it;
			it = std::find(players.begin(), players.end(), uid);
			if (it == players.end())
			{
				break;
			}

			GetActor("Lobby") << MsgObject(0, 9, (intptr_t)&slot, 0, 0, 0, 0);
			break;
		}
		}

		Doc()->m_roomInfo.nUserNum = (unsigned char)players.size();

		GetActor("Lobby") << MsgObject(0, 0x2F, 0, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x10, 0, 0, 0, 0, 0);
		if (Doc()->m_roomInfo.gameType == 6)
		{
			GetActor("Lobby") << MsgObject(0, 0x71, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x1:
	{
		unsigned char result = packet.Decode1();
		switch (result)
		{
		case 0:
		{
			std::string id;
			packet.DecodeStr(id);

			if (stricmp(id.c_str(), MyId()))
			{
				GetActor("Lobby") << MsgObject(0, 0x1E, 0xC, 0, 0, 0, 0);
				break;
			}

			Doc()->m_myInfo.info.dwUID = packet.Decode4();
			Doc()->m_myInfo.info.dwIdentity = packet.Decode4();
			strcpy(Doc()->m_myInfo.info.sID, id.c_str());

			Doc()->m_provType = LOGININFO()->ProvType();
			Doc()->m_myInfo.stat.Level = packet.Decode1();
			Doc()->m_lobbyLoginTime = timeGetTime();

			_GGC_SendUserID(MyId());

			Doc()->m_pcBang = packet.Decode1() == 1;

			unsigned long flagBlock = packet.Decode4();
			int timeBlock = packet.Decode4();

			if (IsLocalContent((localContentType_t)8))
			{
				Doc()->m_myInfo.info.dwFlagBlock = flagBlock;
				Doc()->SetTimeBlock(timeBlock);
			}

			if (IsLocalContent((localContentType_t)0x81))
			{
				std::string nickname;
				packet.DecodeStr(nickname);
				strcpy(Doc()->m_myInfo.info.sNick, nickname.c_str());
			}

			GetActor("Lobby") << MsgObject(0, 0x1E, 4, 0, 0, 0, 0);
			break;
		}
		case 0xDC:
		{
			bool hasSecondPassword = packet.Decode1() == 1;

			GetActor("Lobby")
				<< MsgObject(0, 0, (intptr_t)"SECOND_PWD", 0, 0, 0, 0);
			IActor* pActor = GetActor(s_gatewayActor[1]);
			pActor << MsgObject(0, 0x26D, 3, (intptr_t)&hasSecondPassword, 0, 0,
				0);
			pActor << MsgObject(0, 0x26D, 5, 0, 0, 0, 0);
			break;
		}
		case 0xDD:
		{
			IActor* pActor = GetActor(s_gatewayActor[1]);
			pActor << MsgObject(0, 0x26D, 0x13, 0, 0, 0, 0);
			pActor << MsgObject(0, 0x26D, 7, 0, 0, 0, 0);
			break;
		}
		case 0xDE:
			GetActor(s_gatewayActor[1])
				<< MsgObject(0, 0x26D, 0x13, 0, 0, 0, 0);
			break;
		case 0xDF:
			packet.Decode4();
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xC, 0, 0, 0, 0);
			break;
		case 0xD8:

			if (packet.Decode4() != 0xFFFFFFFF)
			{
				Doc()->SetForceNicknameChange(2);
				GetActor("Lobby")
					<< MsgObject(0, 0, (intptr_t)"BLANK", 0, 0, 0, 0);
			}
			else
			{
				Doc()->SetForceNicknameChange(1);
				if (g_pFresh->IsCurrentLayout("LOGIN") == true)
				{
					GetActor("Lobby")
						<< MsgObject(0, 0, (intptr_t)"BLANK", 0, 0, 0, 0);
				}
			}
			break;
		case 0xD9:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xD, 0, 0, 0, 0);
			break;
		case 2:
			GetActor("Lobby") << MsgObject(0, 0x1E, 6, 0, 0, 0, 0);
			break;
		case 6:
			GetActor("Lobby") << MsgObject(0, 0x1E, 7, 0, 0, 0, 0);
			break;
		case 4:
			GetActor("Lobby") << MsgObject(0, 0x1E, 8, 0, 0, 0, 0);
			break;
		case 5:
			Doc()->m_chatModeName = packet.DecodeStr();
			GetActor("Lobby") << MsgObject(0, 0x1E, 9, 0, 0, 0, 0);
			break;
		case 0x13:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x17, 0, 0, 0, 0);
			break;
		case 7:
			Doc()->m_chatMode = packet.Decode4();
			Doc()->m_chatModeName = packet.DecodeStr();
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xA, 0, 0, 0, 0);
			break;
		case 8:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xB, 0, 0, 0, 0);
			break;
		case 0xC:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x10, 0, 0, 0, 0);
			break;
		case 0xA:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xF, 0, 0, 0, 0);
			break;
		case 9:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xE, 0, 0, 0, 0);
			break;
		case 0xD:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x11, 0, 0, 0, 0);
			break;
		case 0xE:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x12, 0, 0, 0, 0);
			break;
		case 0xF:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x13, 0, 0, 0, 0);
			break;
		case 0x10:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x14, 0, 0, 0, 0);
			break;
		case 0x12:
		case 0xC8:
		case 0xE0:
			break;
		case 0xD4:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x18, 0, 0, 0, 0);
			break;
		case 0xCF:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0x1A, 0, 0, 0, 0);
			break;
		default:
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xC, 0, 0, 0, 0);
			break;
		}
		break;
	}
	case 0xB:
		packet.DecodeStr(Doc()->m_authTicket);
		break;
	case 0x11:
		if (packet.Decode1())
		{
			GetActor("Lobby") << MsgObject(0, 0x23,
				(intptr_t)"\xc4\xb3\xb8\xaf\xc5\xcd \xbc\xb3\xc1\xa4\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe7\xc1\xa2\xbc\xd3\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		break;
	case 0x1CD:
	{
		GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();

		IActor* pActor = GetActor(s_gatewayActor[1]);
		if (result == 0)
		{
			pActor << MsgObject(0, 0x26D, 8, 0, 0, 0, 0);
			pActor << MsgObject(0, 0x26D, 4, 0, 0, 0, 0);
		}
		else
		{
			pActor << MsgObject(0, 0x26D, 1, result, 0, 0, 0);
		}
		break;
	}
	case 0x1CE:
	{
		GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();

		IActor* pActor = GetActor(s_gatewayActor[1]);
		if (result == 0)
		{
			pActor << MsgObject(0, 0x26D, 8, 0, 0, 0, 0);
			pActor << MsgObject(0, 0x26D, 0xE, 0, 0, 0, 0);
		}
		else
		{
			pActor << MsgObject(0, 0x26D, 0xB, result, 0, 0, 0);
		}
		break;
	}
	case 0x10:
	{
		std::string authKey = packet.DecodeStr();
		Doc()->SetLoginAuthKey(authKey);
		break;
	}
	case 0xF:
	{
		Doc()->m_bTutorialResume = true;
		Doc()->m_bTutorialRedirect = true;

		unsigned char eventFlag = packet.Decode1();
		Doc()->m_myInfo.info.gender = eventFlag & 1;

		std::string id;
		packet.DecodeStr(id);
		if (stricmp(id.c_str(), MyId()))
		{
			GetActor("Lobby") << MsgObject(0, 0x1E, 0xC, 0, 0, 0, 0);
			break;
		}

		_GGC_SendUserID(MyId());
		break;
	}
	case 0x2:
	{
		GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		bool bReady = false;
		AfxGetTask()->GetActor(s_gatewayActor[1])
			<< MsgObject(0, 0x26D, 0x14, (intptr_t)&bReady, 0, 0, 0);
		if (!bReady)
		{
			break;
		}

		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(0, 0, (intptr_t)"SERVERLIST", 0, 0, 0, 0);

		Doc()->m_gameServerList.clear();
		int count = packet.Decode1();
		sGameServerInfo info;
		for (int i = 0; i < count; i++)
		{
			memset(&info, 0, sizeof(info));
			packet.DecodeBuffer(&info, sizeof(info));
			Doc()->m_gameServerList.push_back(info);
		}

		GetActor("Lobby") << MsgObject(0, 0x19, 0, 0, 0, 0, 0);
		break;
	}
	case 0x9:
	{
		WSingleton<CMessengerInfo>::Instance()->m_serverList.clear();

		MSNServerInfo info;
		info.bInvalid = false;
		int count = packet.Decode1();
		for (int i = 0; i < count; i++)
		{
			memset(&info, 0, sizeof(sGameServerInfo));
			packet.DecodeBuffer(&info, sizeof(sGameServerInfo));
			WSingleton<CMessengerInfo>::Instance()->m_serverList.push_back(
				info);
		}
		break;
	}
	case 0x4:
		Doc()->m_needParentAgree = packet.Decode1();
		break;
	case 0x3:
	{
		WNetworkSystem* pNetwork = WSingleton<WNetworkSystem>::Instance();

		if (pNetwork->IsConnected(WNetworkSystem::NET_LOGIN))
		{
			pNetwork->ForceShutDown(WNetworkSystem::NET_LOGIN);
		}
		break;
	}
	case 0x5:
	{
		unsigned char kind = packet.Decode1();
		if (kind == 3)
		{
			unsigned long count = packet.Decode4();
			unsigned long typeId = packet.Decode4();

			if (count > 0 && typeId > 0)
			{
				char message[0x400] = { 0 };
				IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(typeId);

				if (pItem)
				{
					sprintf(message,
						" PC\xb9\xe6 \xc3\xe2\xbc\xae \xc0\xcc\xba\xa5\xc6\xae\n\n \\cT\\c\\c0xffff0000\\c%d \\c0xff000000\\c\xb9\xf8 \xc0\xc0\xb8\xf0\n \\cT\\c\\c0xffff0000\\c%s \\c0xff000000\\c\xbe\xc6\xc0\xcc\xc5\xdb \xc1\xf6\xb1\xde",
						count, pItem->c.Name);
				}
				else
				{
					sprintf(message,
						" PC\xb9\xe6 \xc3\xe2\xbc\xae \xc0\xcc\xba\xa5\xc6\xae\n\n \\cT\\c\\c0xffff0000\\c%d \\c0xff000000\\c\xb9\xf8 \xc0\xc0\xb8\xf0\n \\cT\\c\\c0xffff0000\\c\xbe\xc6\xc0\xcc\xc5\xdb \\c0xff000000\\c \xc1\xf6\xb1\xde",
						count);
				}

				GetActor("Lobby")
					<< MsgObject(0, 0x23, (intptr_t)message, 0, 0, 0, 0);
			}
		}
		break;
	}
	case 0x6:
	{
		sOption option;
		sGameOptionInfo info;
		packet.DecodeBuffer(&info, sizeof(info));

		WSingleton<COption>::Instance()->mSetPCBangMascotMsg(info.macro[8]);

		if (WSingleton<COption>::Instance()->mGetSaveMacros())
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				WSingleton<COption>::Instance()->mSetMacroText(i,
					info.macro[i]);
			}

			WSingleton<COption>::Instance()->mSetPCBangMascotMsg(info.macro[i]);
		}
		break;
	}
	case 0x8:
		LOGININFO()->EnablePcBang(packet.Decode4() == 1);
		break;
	case 0xD:
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		std::string message;
		unsigned long result = packet.Decode4();
		switch (result)
		{
		case 0:
		{
			std::string nickname;
			packet.DecodeStr(nickname);
			GetActor("Lobby")
				<< MsgObject(0, 0x249, 0, (intptr_t)nickname.c_str(), 0, 0, 0);
			break;
		}
		case 1:

			message =
				"\xb4\xeb\xc8\xad\xb8\xed \xbc\xb3\xc1\xa4\xc7\xcf\xb4\xc2 \xb0\xfa\xc1\xa4\xbf\xa1\xbc\xad \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 2:

			message =
				"\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.";
			break;
		case 3:

			message =
				"\xb1\xe6\xc0\xcc \xb6\xc7\xb4\xc2 \xc7\xfc\xbd\xc4\xc0\xcc \xb8\xc2\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 4:

			message =
				"\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.";
			break;
		case 5:

			message =
				"\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9..";
			break;
		case 6:

			message =
				"\xc7\xf6\xc0\xe7 DB\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 10:

			message =
				"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xb4\xd0\xb3\xd7\xc0\xd3\xc0\xcc \xc0\xcc\xb9\xcc \xc3\xbc\xc5\xa9\xc7\xd1 \xb4\xd0\xb3\xd7\xc0\xd3\xb0\xfa \xb5\xbf\xc0\xcf\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.";
			break;
		default:
		{
			char code[8] = { 0 };
			message =
				"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xc0\xcc\xc0\xaf\xb7\xce \xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. - Code : ";
			itoa(result, code, 10);
			message += code;
			break;
		}
		}

		if (result != 0)
		{
			GetActor("Lobby")
				<< MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0);
		}
		break;
	}
	case 0xE:
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		std::string message;
		switch (packet.Decode4())
		{
		case 0:
		{
			std::string nickname;
			packet.DecodeStr(nickname);
			GetActor("Lobby")
				<< MsgObject(0, 0x249, 1, (intptr_t)nickname.c_str(), 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x81))
			{
				GetActor("Lobby") << MsgObject(0, 0x6E, 0, 0, 0, 0, 0);
			}

			message =
				"\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.";
			break;
		}
		case 1:

			message =
				"\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xb0\xcb\xbb\xe7\xc7\xcf\xb4\xc2 \xb0\xfa\xc1\xa4\xbf\xa1\xbc\xad \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 2:
			message =
				"\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.";
			break;
		case 3:
			message =
				"\xb1\xe6\xc0\xcc \xb6\xc7\xb4\xc2 \xc7\xfc\xbd\xc4\xc0\xcc \xb8\xc2\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 4:
			message =
				"\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.";
			break;
		case 5:
			message =
				"\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9..";
			break;
		case 6:
			message =
				"\xc7\xf6\xc0\xe7 DB\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 8:
			if (IsLocalContent((localContentType_t)0x81))
			{
				GetActor("Lobby") << MsgObject(0, 0x6E, 0, 8, 0, 0, 0);

				message =
					"\xb5\xbf\xc0\xcf\xc7\xd1 \xb4\xd0\xb3\xd7\xc0\xd3\xc0\xbb \xb1\xd7\xb4\xeb\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.";
			}
			break;
		}

		GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0,
			0, 0);
		break;
	}
	case 0xD2:
	{
		char records[12][0x19];
		memset(records, 0, sizeof(records));
		int count = packet.Decode4();

		for (int i = 0; i < count; i++)
		{
			packet.DecodeBuffer(records[i], 0x19);
		}

		if (count > 0)
		{
			GetActor("Lobby")
				<< MsgObject(0, 0x61, count, (intptr_t)records, 0, 0, 0);
		}
		break;
	}
	case 0x76:
	{
		unsigned long guid = packet.Decode4();
		int value = packet.Decode1();

		GetActor("Lobby") << MsgObject(0, 0x29, guid, value, 0, 0, 0);
		break;
	}
	case 0x6A:

		if (IsMassGame(Doc()->m_roomInfo.gameType))
		{
			unsigned long uid = packet.Decode4();
			unsigned char state = packet.Decode1();

			if (Doc()->AreWePlayingTogether(uid))
			{
				switch (state)
				{
				case 3:
				{
					for (std::vector<sRivalData>::iterator rival =
							 Doc()->m_rivalList.begin();
						rival != Doc()->m_rivalList.end(); ++rival)
					{
						if (rival->state != 3)
						{
							Doc()
								->m_rivalList[Doc()->GetIndex(uid)]
								.quitOrder--;
						}
					}

					std::list<sSlotInfo>::iterator it =
						std::find(Doc()->m_slotList.begin(),
							Doc()->m_slotList.end(), uid);

					if (it != Doc()->m_slotList.end())
					{
						if (Doc()->m_rivalList[Doc()->GetIndex(uid)].state == 2)
						{
							GetActor("Lobby")
								<< MsgObject(0, 8, (intptr_t)&*it, 0, 0, 0, 0);
						}
					}
					break;
				}
				case 2:
				{
					std::list<sSlotInfo>::iterator it =
						std::find(Doc()->m_slotList.begin(),
							Doc()->m_slotList.end(), uid);
					if (it != Doc()->m_slotList.end())
					{
						GetActor("Lobby")
							<< MsgObject(0, 7, (intptr_t)&*it, 0, 0, 0, 0);
					}
					break;
				}
				}

				Doc()->m_rivalList[Doc()->GetIndex(uid)].state = state;
				GetActor("Lobby") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 0x6C:

		if (IsMassGame())
		{
			unsigned long uid = packet.Decode4();
			packet.Decode1();
			float pos[2];
			packet.DecodeBuffer(pos, 8);

			unsigned int distance;
			int point;
			if (Doc()->m_roomInfo.gameType == 10)
			{
				distance = packet.Decode4();
				point = packet.Decode4();
			}

			packet.Decode2();

			if (Doc()->AreWePlayingTogether(uid))
			{
				unsigned char index = Doc()->GetIndex(uid);
				std::vector<sRivalData>& rivals = Doc()->m_rivalList;

				if (!WisEqual(rivals[index].ballPos.x, pos[0], 0.00001f) &&
					!WisEqual(rivals[index].ballPos.z, pos[1], 0.00001f))
				{
					rivals[index].ballPos = WVector(pos[0], 0.0f, pos[1]);

					if (Doc()->m_roomInfo.gameType == 10)
					{
						Doc()->m_rivalList[index].approachDistance = distance;
						Doc()->m_rivalList[index].approachResultDistance =
							distance;
						Doc()->m_rivalList[index].approachTime = point;
					}
				}
			}
		}
		break;
	case 0x6B:

		if (IsMassGame())
		{
			unsigned long uid = packet.Decode4();
			unsigned char hole = packet.Decode1();
			unsigned char shots = packet.Decode1();
			int score = packet.Decode4();

			__int64 pang = packet.Decode8();
			__int64 extraPang = packet.Decode8();

			int finished = packet.Decode1();

			if (Doc()->AreWePlayingTogether(uid))
			{
				Doc()->LogGstUpdateHole(uid, hole, score, pang);
				std::vector<sRivalData>& rivals = Doc()->m_rivalList;
				std::vector<sRivalData>::iterator it =
					std::find_if(rivals.begin(), rivals.end(), sFindRival(uid));

				if (it == rivals.end())
				{
					break;
				}

				sRivalData& rival = *it;

				if (hole != rival.hole)
				{
					break;
				}

				if (finished == 1)
				{
					rival.holeStroke[hole - 1] = shots - rival.totalStroke;
					rival.holeScore[hole - 1] = score - rival.totalScore;
					rival.holePang[hole - 1] = pang - rival.totalPang;

					rival.totalStroke = shots;
					rival.totalScore = score;
					rival.hole = Doc()->m_holeOrder[GetHoleIndex(hole)];
				}

				rival.totalPang = pang;
				rival.totalBonusPang = extraPang;

				if (Doc()->m_golfGame.gameType == 6)
				{
					GetActor("Lobby") << MsgObject(0, 0x5E, uid, hole, 0, 0, 0);
				}

				GetActor("Lobby") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 0x77:
	{
		if (Doc()->m_holeStatistics[0].dwExp != 0xFFFFFFFF)
		{
			break;
		}

		Doc()->m_holeStatistics[0].dwExp = packet.Decode4();
		Doc()->m_roomType = packet.Decode4();
		Doc()->m_eventType = packet.Decode1();
		Doc()->m_eventFlag = packet.Decode1();

		if (Doc()->m_roomInfo.gameType == 6)
		{
			Doc()->m_guildBonusPang = packet.Decode4();
			Doc()->m_guildPoint = packet.Decode4();

			Doc()->m_guildRoundScore[0] = packet.Decode4();
			Doc()->m_guildRoundScore[1] = packet.Decode4();
		}

		packet.DecodeBuffer(Doc()->m_awardItem, 0x60);

		GetActor("Lobby") << MsgObject(0, 0x2D, 0, 0, 0, 0, 0);
		break;
	}
	case 0x7A:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;

		unsigned long uid = packet.Decode4();
		packet.Decode2();

		for (std::list<sSlotInfo>::iterator it = players.begin();
			it != players.end(); ++it)
		{
			if (it->dwGuid == uid)
			{
				it->bMaster = 1;
				it->bReady = 1;
			}
			else
			{
				it->bMaster = 0;
			}
		}

		GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x5F, uid, 0, 0, 0, 0);
		break;
	}
	case 0x7B:
	{
		std::list<sSlotInfo>& players = Doc()->m_slotList;

		unsigned long uid = packet.Decode4();
		unsigned char team = packet.Decode1();

		std::list<sSlotInfo>::iterator it;
		it = std::find(players.begin(), players.end(), uid);
		if (it == players.end())
		{
			break;
		}

		it->bTeam = team & 3;

		GetActor("Lobby") << MsgObject(0, 0x46, uid, team, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		break;
	}
	case 0x7D:
	{
		unsigned char reason = packet.Decode1();

		std::string message;

		switch (reason)
		{
		case 1:

			message =
				"\xb0\xd4\xc0\xd3\xc0\xbb \xbd\xc3\xc0\xdb\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8 \xc7\xca\xbf\xe4\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xc7 \xbc\xf6\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.";
			break;
		case 2:

			message =
				"\xc6\xc0\xc0\xc7 \xc0\xce\xbf\xf8\xbc\xf6\xb0\xa1 \xbc\xad\xb7\xce \xb8\xc2\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 3:

			message =
				"\xb0\xd4\xc0\xd3\xc0\xbb \xbd\xc3\xc0\xdb\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8 \xc7\xca\xbf\xe4\xc7\xd1 Ready\xb5\xc8 \xc0\xaf\xc0\xfa\xc0\xc7 \xbc\xf6\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.";
			break;
		case 4:

			message =
				"\xb0\xd4\xc0\xd3\xc0\xbb \xbd\xc3\xc0\xdb\xc7\xcf\xb1\xe2\xbf\xa1 \xc0\xce\xbf\xf8\xc0\xcc \xb3\xca\xb9\xab \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 6:

			message =
				"\xc3\xb3\xc0\xbd\xbf\xa1 \xb0\xb6\xb7\xaf\xb8\xae\xc7\xd2 \xc0\xaf\xc0\xfa\xb0\xa1 \xbc\xb1\xc5\xc3\xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 7:
		case 8:

			message =
				"\xb0\xd4\xc0\xd3 \xbd\xba\xc5\xb8\xc6\xae\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 9:

			message =
				"\xc3\xd6\xbd\xc5 \xb9\xf6\xc1\xaf\xc0\xb8\xb7\xce \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xb0\xa1 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.";
			break;
		case 12:

			message =
				"\xbb\xf3\xb4\xeb \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xbf\xcd \xb8\xc5\xc4\xa1 \xc7\xc3\xb7\xb9\xc0\xcc\xb8\xa6 \xb4\xf5 \xc0\xcc\xbb\xf3 \xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		case 13:

			message =
				"\xb0\xd4\xc0\xd3\xc0\xbb \xbd\xc3\xc0\xdb\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd1 \xc0\xaf\xc0\xfa\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
			break;
		}

		std::list<sSlotInfo>& players = Doc()->m_slotList;
		std::list<sSlotInfo>::iterator it =
			std::find(players.begin(), players.end(), MyGuid(false));
		if (it == players.end())
		{
			break;
		}

		if (it->bMaster)
		{
			GetActor("Lobby")
				<< MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0);
			GetActor("Lobby") << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		}
		else if (Doc()->m_roomInfo.gameType == 6)
		{
			GetActor("Lobby")
				<< MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0);
		}
		else
		{
			GetActor("Lobby") << MsgObject(0, 0x2B, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x7F:
	{
		std::string nickName;
		packet.DecodeStr(nickName);

		nickName +=
			"\xb4\xd4\xc0\xbb \xb0\xad\xc1\xa6 \xc5\xf0\xc0\xe5\xbd\xc3\xc5\xb0\xb7\xc1 \xc7\xd5\xb4\xcf\xb4\xd9.\n\xc2\xf9\xbc\xba\xc7\xcf\xbd\xca\xb4\xcf\xb1\xee?";
		GetActor("Lobby") << MsgObject(0, 0x31, (intptr_t)nickName.c_str(),
			packet.Decode4(), 0, 0, 0);
		break;
	}
	case 0x35:
	case 0x81:

		if (IsLocalContent((localContentType_t)0x54))
		{
			unsigned short result = packet.Decode2();

			if (result != 0)
			{
				break;
			}

			unsigned long uid = packet.Decode4();
			unsigned char server = packet.Decode1();
			unsigned short room = packet.Decode2();

			unsigned long roomKey = packet.Decode4();
			std::string password = packet.DecodeStr();
			unsigned long moveKey = packet.Decode4();

			Doc()->SetDirectMoveRoomInfo(server, room, uid);

			WSendPacket send(0x24);
			send.Encode4(moveKey);
			send.Send(TO_MSN);

			GetActor("Lobby") << MsgObject(0, 0x30, uid, server, room, roomKey,
				(intptr_t)&password);
		}
		else
		{
			unsigned short room = packet.Decode2();
			unsigned long roomKey = packet.Decode4();
			GetActor("Lobby") << MsgObject(0, 0x30, room, roomKey, 0, 0, 0);
		}
		break;
	case 0x84:
	{
		int count = packet.Decode1();
		if (count)
		{
			sRoomDetail desc;
			packet.DecodeBuffer(&desc, sizeof(desc));

			Doc()->m_roomDetail = desc;

			std::list<sRoomUserInfo>& userList = Doc()->m_roomUserList;
			userList.clear();
			for (int i = 0; i < count; i++)
			{
				sRoomUserInfo info;
				memset(&info, 0, sizeof(info));
				packet.DecodeBuffer(&info, sizeof(info));
				userList.push_back(info);
			}

			GetActor("Lobby") << MsgObject(0, 0x33, 0, 0, 0, 0, 0);
		}
		else
		{
			GetActor("Lobby") << MsgObject(0, 0x33, 1, 0, 0, 0, 0);
		}
		break;
	}
	case 0x86:
	{
		unsigned long count = packet.Decode4();

		for (unsigned long i = 0; i < count; i++)
		{
			unsigned long uid = packet.Decode4();
			unsigned short roomIndex = packet.Decode2();

			std::map<unsigned long, sBriefUserInfo>::iterator it =
				Doc()->m_briefUserInfoMap.find(uid);
			if (it != Doc()->m_briefUserInfoMap.end())
			{
				it->second.roomIndex = roomIndex;
			}
		}
		break;
	}
	case 0x8A:
	{
		unsigned char finishOrder = (unsigned char)Doc()->m_rivalList.size();
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->state == 0)
			{
				it->state = 2;
				it->finishOrder = finishOrder--;

				for (;; it->hole = Doc()->m_holeOrder[GetHoleIndex(it->hole)])
				{
					it->holePang[it->hole - 1] = 0;
					it->holeStroke[it->hole - 1] =
						Doc()->m_holePar[it->hole - 1] + 5;
					it->holeScore[it->hole - 1] = 5;
					it->totalScore += 5;

					if (it->hole == GetEndHole())
					{
						break;
					}
				}
			}
		}

		GetActor("Lobby") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
		break;
	}
	case 0x8C:
	{
		unsigned long guid = packet.Decode4();
		unsigned char value = packet.Decode1();

		GetActor("Lobby") << MsgObject(0, 0x3D, guid, value, 0, 0, 0);
		break;
	}
	case 0x94:

		Doc()->m_bonusCash = packet.Decode8();
		Doc()->RefreshAllCookie();

		GetActor("Lobby") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		if (*Doc()->m_shopName)
		{
			WSingleton<CMouseCursor>::Instance()->SetActive(false);

			if (WSingleton<CUserInfo>::Instance())
			{
				WSingleton<CUserInfo>::Instance()->Close();
			}

			WSingleton<CTaskManager>::Instance()->ChangeTask("CShopTask", "",
				false);
			AfxPostMsg(0, "Shop", 0, (intptr_t)Doc()->m_shopName, 0, 0, 0);
		}
		break;
	case 0x96:

		if (GalleryMode())
		{
			packet.DecodeBuffer(&Doc()->m_userInfo[0], sizeof(sUserInfo));
			Doc()->m_myInfo.info.dwGalleryGuid =
				Doc()->m_userInfo[0].info.dwGuid;

			if (IsLocalContent((localContentType_t)0x57))
			{
				RequestUccInfo(Doc()->m_userInfo[0]);
			}
		}
		break;
	case 0x95:
		Doc()->m_roomType = packet.Decode4();
		break;
	case 0xBA:
	{
		unsigned char result = packet.Decode1();

		if (result == 0xB)
		{
			GetActor("Lobby") << MsgObject(0, 3,
				(intptr_t)"\xbe\xcb\xb8\xb2 : \xb5\xd1\xb7\xaf\xba\xb8\xb1\xe2\xb0\xa1 \xb0\xa1\xb4\xc9\xc7\xd1 \xb0\xd4\xc0\xd3\xc1\xdf\xc0\xce \xb9\xe6\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFFFF7878, 0, 0, 0);
			break;
		}

		if (result == 0x10)
		{
			WSingleton<CChatMsg>::Instance()->AddChatMsg(
				"\xbe\xcb\xb8\xb2 : \xb7\xce\xb5\xf9\xb5\xc7\xb1\xe2 \xc0\xfc\xbf\xa1 \xb0\xd4\xc0\xd3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc9 \xbf\xec\xb7\xc1\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFFFF7878, false, false);
			break;
		}

		packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));
		Doc()->m_expRate = packet.Decode4();

		unsigned char count = packet.Decode1();
		Doc()->m_slotList.clear();

		for (unsigned char i = 0; i < count; i++)
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(sSlotInfo));
			Doc()->m_slotList.push_back(slot);
		}

		CSharedDoc* pDoc = Doc();
		if (IsMassGame(pDoc->m_roomInfo.gameType))
		{
			packet.DecodeBuffer(&pDoc->m_userInfo[0], sizeof(sUserInfo));

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
				packet.DecodeBuffer(&Doc()->m_userInfo[i], sizeof(sUserInfo));

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

		for (int i = 0; i < Doc()->m_golfGame.holes; i++)
		{
			Doc()->m_holeRandom[i] = packet.Decode4();
			Doc()->m_courseMap[i] = (unsigned char)(Doc()->m_holeRandom[i] % 3);
			Doc()->m_holeOrder[i] = packet.Decode1();
		}

		Doc()->m_startHole = packet.Decode1();
		Doc()->m_roomSetupStage = 0;

		GetActor("Lobby") << MsgObject(0, 0x18, 0, 0, 0, 0, 0);
		break;
	}
	case 0xBD:
	{
		unsigned char count = packet.Decode1();
		for (unsigned char i = 0; i < count; i++)
		{
			sGuildMatchup matchup;
			packet.DecodeBuffer(&matchup, sizeof(matchup));

			unsigned long myGuid = Doc()->m_myInfo.info.dwGuid;

			if (myGuid == matchup.uid[0])
			{
				Doc()->m_teamPlayerGuid[0] = matchup.uid[0];
				Doc()->m_teamPlayerGuid[1] = matchup.uid[1];
				Doc()->m_guildTeam = matchup.team;
			}
			else if (myGuid == matchup.uid[1])
			{
				Doc()->m_teamPlayerGuid[0] = matchup.uid[1];
				Doc()->m_teamPlayerGuid[1] = matchup.uid[0];
				Doc()->m_guildTeam = matchup.team;
			}

			Doc()->m_guildMatchupList.push_back(matchup);
		}
		break;
	}
	case 0xBE:
		packet.DecodeBuffer(Doc()->m_guildRefreshData,
			sizeof(Doc()->m_guildRefreshData));
		WSingleton<CUserInfo>::Instance()->Refresh();
		break;
	case 0xBF:
	{
		unsigned long guildUID = packet.Decode4();

		std::map<unsigned long, sBriefUserInfo>::iterator it =
			Doc()->m_briefUserInfoMap.find(
				WSingleton<CUserInfo>::Instance()->GetGuid());

		if (it == Doc()->m_briefUserInfoMap.end())
		{
			if (guildUID == it->second.m_GuildId)
			{
				unsigned short count = packet.Decode2();
				int n = count > 10 ? 10 : count;

				for (int i = 0; i < n; i++)
				{
					sGuildMatchFlagInfo info;
					packet.DecodeBuffer(&info, 0x2E);
					Doc()->m_guildMatchFlagList.push_back(info);
				}

				WSingleton<CUserInfo>::Instance()->Refresh();
			}
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

		sRivalData* pMine = &Doc()->m_rivalList[Doc()->GetIndex(uid)];
		sRivalData* pRival = Doc()->GetRival(uid);
		if (pMine && pRival)
		{
			GetActor("Lobby") << MsgObject(0, 0x57, Doc()->m_guildScore[0],
				Doc()->m_guildScore[1], 0, 0, 0);

			pMine->guildPoint = point[0];
			pRival->guildPoint = point[1];
		}
		break;
	}
	case 0xC1:
		GetActor("Lobby") << MsgObject(0, 0x2B, 1, 0, 0, 0, 0);
		GetActor("Lobby") << MsgObject(0, 0x58, 0, 0, 0, 0, 0);
		break;
	case 0xCD:
	{
		unsigned short count = packet.Decode2();
		Doc()->m_questList.clear();

		for (int i = 0; i < count; i++)
		{
			sQuest quest;
			quest.typeId = packet.Decode4();
			quest.count = packet.Decode1();
			quest.pQuest = ItemManager()->FindQuest(quest.typeId);
			quest.flag = false;

			Doc()->m_questList.push_back(quest);
		}
		break;
	}
	case 0x117:

		if (IsLocalContent((localContentType_t)0x14))
		{
			unsigned long uid = packet.Decode4();
			if (Doc()->AreWePlayingTogether(uid))
			{
				Doc()->m_rivalList[Doc()->GetIndex(uid)].state = 2;
				GetActor("Lobby") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 0x127:

		if (IsLocalContent((localContentType_t)0x54))
		{
			GetActor("Lobby") << MsgObject(0, 0x230, 0, 0, 0, 0, 0);

			unsigned short result = packet.Decode2();
			if (result == 0)
			{
				unsigned long uid = packet.Decode4();
				unsigned char channel = packet.Decode1();
				unsigned short room = packet.Decode2();
				unsigned long serverUID = packet.Decode4();
				std::string name = packet.DecodeStr();
				unsigned long inviteKey = packet.Decode4();

				if (Doc()->m_gameServerUID == Doc()->GetFriendGameSvrUID())
				{
					WSendPacket send((enumClientPacket)0x29);
					send.Encode4(inviteKey);
					send.Send(TO_GAME);
				}
				else if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
							 WNetworkSystem::NET_MSN))
				{
					if (serverUID == MyUID())
					{
						if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
								WNetworkSystem::NET_MSN))
						{
							WSendPacket send(0x26);
							send.Encode4(uid);
							send.Encode1(channel);
							send.Encode2(room);
							send.Encode4(serverUID);
							send.EncodeStr(name);
							send.Encode4(inviteKey);
							send.Send(TO_MSN);
						}
					}
				}
			}
			else
			{
				if (result == 6)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc0\xcc\xb9\xcc \xb0\xd4\xc0\xd3\xb9\xe6\xbf\xa1\xc0\xd4\xc0\xe5\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x14)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc7\xd8\xb4\xe7 \xc0\xaf\xc0\xfa\xb4\xc2 \xc3\xca\xb4\xeb\xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x15)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc7\xd8\xb4\xe7 \xc0\xaf\xc0\xfa\xb4\xc2 \xb1\xe6\xb5\xe5 \xb0\xa1\xc0\xd4\xc0\xbb \xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x16)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xb7\xe7\xc5\xb0\xb7\xb9\xba\xa7\xb8\xb8 \xc0\xd4\xc0\xe5\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x17)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc7\xd8\xb4\xe7 \xc0\xaf\xc0\xfa\xb4\xc2 \xc3\xca\xb4\xeb\xc7\xd2\xbc\xf6 \xc0\xd6\xb4\xc2 \xc0\xa7\xc4\xa1\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 2)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xb9\xe6\xbf\xa1 \xc0\xce\xbf\xf8\xc0\xcc \xb2\xcb\xc3\xa1\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x18)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc3\xa4\xb3\xce\xc0\xc7 \xc1\xa4\xbf\xf8\xbc\xf6\xb0\xa1 \xc3\xca\xb0\xfa\xb5\xc7\xbe\xee \xc3\xca\xb4\xeb\xc7\xd2\xbc\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else if (result == 0x19)
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc3\xca\xb4\xeb \xb4\xeb\xbb\xf3\xc0\xba \xc3\xa4\xb3\xce\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xd2\xbc\xf6 \xbe\xf8\xb4\xc2 \xb7\xb9\xba\xa7\xc0\xd4\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
				else
				{
					GetActor("Lobby") << MsgObject(0, 0x23,
						(intptr_t)"\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}

				Doc()->m_inviteMode = 0;
			}
		}
		break;
	case 0x128:

		if (IsLocalContent((localContentType_t)0x54))
		{
			std::list<sSlotInfo>& players = Doc()->m_slotList;

			unsigned long ruleUID = packet.Decode4();

			Doc()->m_inviteMode = 0;

			for (std::list<sSlotInfo>::iterator it = players.begin();
				it != players.end(); ++it)
			{
				if (ruleUID == it->dwUserUID)
				{
					sSlotInfo* slot = &*it;
					unsigned long uid = slot->dwGuid;
					unsigned char team = slot->bTeam;
					unsigned char connection = slot->connectionRank;

					players.erase(it);

					for (std::list<sSlotInfo>::iterator other = players.begin();
						other != players.end(); ++other)
					{
						if (other->connectionRank > connection)
						{
							other->connectionRank--;
						}
					}

					GetActor("Lobby")
						<< MsgObject(0, 8, (intptr_t)slot, uid, team, 0, 0);

					Doc()->m_roomInfo.nUserNum = (unsigned char)players.size();

					GetActor("Lobby") << MsgObject(0, 0x2F, 0, 0, 0, 0, 0);
					GetActor("Lobby") << MsgObject(0, 0xF, 0, 0, 0, 0, 0);
					GetActor("Lobby") << MsgObject(0, 0x10, 0, 0, 0, 0, 0);
					break;
				}
			}
		}
		break;
	case 0x132:
		if (IsLocalContent((localContentType_t)0x59))
		{
			unsigned long value = 0;
			value = packet.Decode4();
			std::string name = packet.DecodeStr();
			unsigned char kind = packet.Decode1();

			GetActor("Lobby") << MsgObject(0, 0x21C, (intptr_t)&name,
				(intptr_t)&kind, (intptr_t)&value, 0, 0);
		}
		break;
	case 0x134:
		if (IsLocalContent((localContentType_t)0x59))
		{
			packet.Decode1();
			packet.Decode1();
		}
		break;
	case 0x133:
		if (IsLocalContent((localContentType_t)0x59))
		{
			switch (packet.Decode1())
			{
			case 0:
				GetActor("Lobby") << MsgObject(0, 0x21D, 1, 0, 0, 0, 0);
				break;
			case 2:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xb8\xc5\xc4\xaa \xb4\xeb\xbb\xf3\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\n\xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			case 3:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xb4\xeb\xbb\xf3\xc0\xcc \xb8\xc5\xc4\xaa\xc0\xbb \xb0\xc5\xc0\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			case 1:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xb8\xf0\xb5\xe7 \xb4\xeb\xbb\xf3\xc0\xcc \xb8\xc5\xc4\xaa\xc0\xbb \xb0\xc5\xc0\xfd\xc7\xcf\xbf\xa9 \xb8\xc5\xc4\xaa\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.\n \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			case 4:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xc8\xaf\xbb\xf3\xc0\xc7 \xc6\xce\xbe\xdf\xbc\xb6\xbf\xa1\xbc\xad\xb4\xc2 \xb8\xc5\xc4\xaa\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			case 6:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xb8\xc5\xc4\xaa\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			case 5:

				GetActor("Lobby") << MsgObject(0, 0x21D, 0,
					(intptr_t)MakeStr(
						"\xb8\xc5\xc4\xaa\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xd1 \xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb8\xc5\xc4\xaa \xb0\xa1\xb4\xc9\xc7\xd1 \xc0\xa7\xc4\xa1\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
					0, 0, 0);
				break;
			}
		}
		break;
	case 0x135:

		if (IsLocalContent((localContentType_t)0x59))
		{
			unsigned long serverUID = packet.Decode4();
			unsigned char channel = packet.Decode1();
			unsigned short room = packet.Decode2();

			if (serverUID == Doc()->m_gameServerUID)
			{
				WSendPacket send((enumClientPacket)0xAC);
				send.Encode1(channel);
				send.Encode2(room);
				send.Send(TO_GAME);
			}
			else
			{
				if (g_pFresh->IsCurrentLayout("REALMYROOM"))
				{
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(0, 0xDF, 0, 0, 0, 0, 0);
				}

				Doc()->m_gameServerList.clear();
				Doc()->m_channelList.clear();

				WSendPacket send((enumClientPacket)0x43);
				send.Send(TO_GAME);

				Doc()->m_goWithMode = 3;
				Doc()->SetDirectMoveRoomInfo(channel, room, serverUID);
			}
		}
		break;
	case 0x136:
		if (IsLocalContent((localContentType_t)0x59))
		{
			GetActor("Lobby") << MsgObject(0, 0x21D, 0,
				(intptr_t)MakeStr(
					"\xb8\xc5\xc4\xaa\xc0\xcc \xc3\xeb\xbc\xd2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."),
				0, 0, 0);
		}
		break;
	case 0x137:
	{
		CMatchingSystem* pMatching = WSingleton<CMatchingSystem>::Instance();
		if (pMatching)
		{
			unsigned char count = packet.Decode1();

			pMatching->Clear();
			for (unsigned char i = 0; i < count; i++)
			{
				unsigned char code = packet.Decode1();
				pMatching->SetMyIntrest(code);
			}
		}
		break;
	}
	case 0x180:
		if (IsLocalContent((localContentType_t)0x72))
		{
			unsigned short result = packet.Decode2();

			if (result == 1)
			{
				GetActor("Lobby") << MsgObject(0, 0x240, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 0x182:
	{
		unsigned long flagBlock = packet.Decode4();
		Doc()->m_myInfo.info.dwFlagBlock = flagBlock;
		break;
	}
	case 0x18C:

		if (IsLocalContent((localContentType_t)0x7A))
		{
			if (WSingleton<TicketInfo>::IsInstantiated())
			{
				unsigned char result = packet.Decode1();
				unsigned long type = packet.Decode4();

				switch (type)
				{
				case 0x11110000:
				{
					WSingleton<TicketInfo>::Instance()->m_ticketCount =
						packet.Decode4();
					WSingleton<TicketInfo>::Instance()->m_state =
						packet.Decode1();

					unsigned long count =
						WSingleton<TicketInfo>::Instance()->m_itemMap.size();
					for (unsigned long i = 0; i < count; i++)
					{
						unsigned long tid = packet.Decode4();
						unsigned long exchangeCount = packet.Decode4();
						unsigned long maxCount = packet.Decode4();

						std::map<unsigned long,
							TicketInfo::sTicketItem>::iterator it =
							WSingleton<TicketInfo>::Instance()->m_itemMap.find(
								tid);
						if (it !=
							WSingleton<TicketInfo>::Instance()->m_itemMap.end())
						{
							it->second.count = exchangeCount;
							it->second.remain = maxCount;
						}
					}
					break;
				}
				case 0x10000:
				{
					unsigned long ticketCount = packet.Decode4();

					unsigned long tid = packet.Decode4();
					unsigned long exchangeCount = packet.Decode4();
					unsigned long maxCount = packet.Decode4();

					WSingleton<TicketInfo>::Instance()->m_ticketCount =
						ticketCount;
					std::map<unsigned long, TicketInfo::sTicketItem>::iterator
						it = WSingleton<TicketInfo>::Instance()->m_itemMap.find(
							tid);
					if (it !=
						WSingleton<TicketInfo>::Instance()->m_itemMap.end())
					{
						it->second.count = exchangeCount;
						it->second.remain = maxCount;
					}
					break;
				}
				case 0x100000:
				{
					unsigned long ticketCount = packet.Decode4();

					unsigned long tid = packet.Decode4();
					unsigned long maxCount = packet.Decode4();

					WSingleton<TicketInfo>::Instance()->m_ticketCount =
						ticketCount;
					std::map<unsigned long, TicketInfo::sTicketItem>::iterator
						it = WSingleton<TicketInfo>::Instance()->m_itemMap.find(
							tid);
					if (it !=
						WSingleton<TicketInfo>::Instance()->m_itemMap.end())
					{
						it->second.remain = maxCount;
					}
					break;
				}
				case 0x1000000:
				{
					unsigned long ticketCount = packet.Decode4();
					WSingleton<TicketInfo>::Instance()->m_ticketCount =
						ticketCount;
					break;
				}
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(0, 2, 0, 0, 0, 0, 0));
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(0, 0x6D, type, result, 0, 0, 0));
			}
		}
		break;
	case 0x18E:
	{
		if (!IsLocalContent((localContentType_t)0x83))
		{
			break;
		}

		CMissionEvent* pMissionEvent;
		WSingleton<CContentsDoc>::Instance()->GetContainer(
			(localContentType_t)0x83, pMissionEvent);
		if (pMissionEvent == NULL)
		{
			break;
		}

		pMissionEvent->DecodeMissionInfo(packet);

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x6F, 0, 0, 0, 0, 0));
	}
	break;
	case 0x18F:
	{
		if (!IsLocalContent((localContentType_t)0x83))
		{
			break;
		}

		unsigned long value = packet.Decode4();
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x6F, 1, value, 0, 0, 0));
	}
	break;
	case 0x190:
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		if (IsLocalContent((localContentType_t)0x85))
		{
			unsigned long result = packet.Decode4();
			if (result != 0)
			{
				char szMsg[256] = {
					0,
				};

				switch (result)
				{
				case 1:

					wsprintfA(szMsg,
						"\xc0\xdf\xb8\xf8\xb5\xc8 \xbc\xfd\xc0\xda\xb0\xa1 \xc3\xdf\xc3\xb7\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbf\xa1\xb7\xaf\xc4\xda\xb5\xe5 : %d",
						1);
					break;
				case 9:

					wsprintfA(szMsg,
						"\xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe7\xc1\xa2\xbc\xd3 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4 : %d",
						9);
					break;
				case 10:

					wsprintfA(szMsg,
						"\xc3\xdf\xc3\xb7\xb1\xc7\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(0, 0x23, (intptr_t)szMsg, 0, 0, 0, 0));
				break;
			}

			int draw[4];
			packet.DecodeBuffer(draw, 0x10);

			unsigned long number = 0;
			if (draw[0] == 1)
			{
				number = packet.Decode4();
			}

			int count = packet.Decode4();
			unsigned long value = 0;
			if (count > 0)
			{
				value = packet.Decode4();
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x70, draw[0],
				(intptr_t)draw, number, count, (intptr_t)&value));
		}
		break;
	case 0x191:
	{
		if (!IsLocalContent((localContentType_t)0x85))
		{
			break;
		}

		unsigned long value = packet.Decode4();
		int count = packet.Decode4();
		if (count == 0)
		{
			break;
		}

		std::vector<unsigned long> list;
		for (int i = 0; i < count; i++)
		{
			unsigned long item = packet.Decode4();
			list.push_back(item);
		}
		AfxGetTask()->SendMsgToMainActor(
			MsgObject(0, 0x70, 2, (intptr_t)&list, (intptr_t)&value, 0, 0));
	}
	break;
	case 0x19F:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

		unsigned long result = packet.Decode4();
		if (result != 0)
		{
			char szMsg[256] = {
				0,
			};

			switch (result)
			{
			case 1:

				wsprintfA(szMsg,
					"\xbd\xc3\xbd\xba\xc5\xdb \xbf\xa1\xb7\xaf : %d", result);
				break;
			case 9:

				wsprintfA(szMsg,
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe7\xc1\xa2\xbc\xd3 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4 : %d",
					result);
				break;
			case 7:

				wsprintfA(szMsg,
					"\xc7\xf6\xc0\xe7 \xc0\xcc\xba\xa5\xc6\xae \xb1\xe2\xb0\xa3\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9",
					result);
				break;
			case 4:

				wsprintfA(szMsg,
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xc1\xa4\xba\xb8 \xc0\xd4\xb4\xcf\xb4\xd9 : %d",
					result);
				break;
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x23, (intptr_t)szMsg, 0, 0, 0, 0));
			break;
		}

		unsigned long first = packet.Decode4();
		unsigned long second = packet.Decode4();
		unsigned long count = packet.Decode4();
		std::vector<unsigned long> typeIds;
		std::vector<unsigned long> counts;
		for (unsigned long i = 0; i < count; i++)
		{
			unsigned long typeId = packet.Decode4();
			unsigned long itemCount = packet.Decode4();
			typeIds.push_back(typeId);
			counts.push_back(itemCount);
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x250, (intptr_t)&first,
			(intptr_t)&second, (intptr_t)&typeIds, (intptr_t)&counts, 0));
		break;
	}
	case 0x1A0:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

		unsigned long result = packet.Decode4();
		if (result != 0)
		{
			char szMsg[256] = {
				0,
			};

			switch (result)
			{
			case 1:

				wsprintfA(szMsg,
					"\xbd\xc3\xbd\xba\xc5\xdb \xbf\xa1\xb7\xaf : %d", 1);
				break;
			case 9:

				wsprintfA(szMsg,
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe7\xc1\xa2\xbc\xd3 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4 : %d",
					9);
				break;
			case 4:

				wsprintfA(szMsg,
					"\xc0\xdf\xb8\xf8\xb5\xc8 \xc1\xa4\xba\xb8 \xc0\xd4\xb4\xcf\xb4\xd9 : %d",
					4);
				break;
			}

			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x23, (intptr_t)szMsg, 0, 0, 0, 0));
			break;
		}

		unsigned long value = packet.Decode4();
		AfxGetTask()->SendMsgToMainActor(
			MsgObject(0, 0x251, (intptr_t)&value, 0, 0, 0, 0));
		break;
	}
	case 0x19D:
	{
		unsigned long typeId = packet.Decode4();
		unsigned long count = packet.Decode4();
		if (typeId)
		{
			char szMsg[256] = {
				0,
			};

			sprintf(szMsg,
				"\xb9\xe6\xc0\xe5\xbf\xa1 \xb4\xeb\xc7\xd1 \xba\xb8\xbb\xf3\xc0\xb8\xb7\xce \\c0xffff0000\\c%s(%d)\\c0xff000000\\c\xc0\xcc \xc1\xf6\xb1\xde \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				ItemManager()->GetItemName(typeId), count);
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(0, 0x26, (intptr_t)szMsg, typeId, 0, 0, 0));
		}
		break;
	}
	case 0x1B6:
	{
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GUILD_INFO info;
		packet.DecodeBuffer(&info, 0x129);

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 4, (intptr_t)&info, 0, 0, 0);
		break;
	}
	case 0x1B7:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
		}
		else
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, 0x403, 0, 0, 0);
		}
		break;
	}
	case 0x1B8:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
		}
		else
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, 0x402, 0, 0, 0);
		}
		break;
	}
	case 0x1BC:
	{
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		std::list<GUILD_HISTORY> history;

		int entryCount = packet.Decode2();
		for (int i = 0; i < entryCount; i++)
		{
			GUILD_HISTORY entry;
			packet.DecodeBuffer(&entry, 0x31);
			history.push_back(entry);
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 9, (intptr_t)&history, 0, 0, 0);
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
		packet.DecodeBuffer(&info, 0x119);

		GetActor(s_guildActor[1])
			<< MsgObject(0, 0x263, 5, (intptr_t)&info, 0, 0, 0);
		break;
	}
	case 0x1C4:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		unsigned long listParam1 = packet.Decode4();
		unsigned long listParam2 = packet.Decode4();
		int memberCount = packet.Decode2();
		std::list<GUILD_USER_LIST> members;

		for (int i = 0; i < memberCount; i++)
		{
			GUILD_USER_LIST member;
			packet.DecodeBuffer(&member, 0x51);
			members.push_back(member);
		}

		if (memberCount != 0)
		{
			IActor* pActor = GetActor(s_guildActor[1]);

			pActor << MsgObject(0, 0x263, 7, (intptr_t)&members, listParam1,
				listParam2, 0);
		}
		break;
	}
	case 0x1BF:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 0xA, 0, 0, 0, 0);
		break;
	}
	case 0x1C3:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 0x12, 0, 0, 0, 0);
		break;
	}
	case 0x1BA:
	case 0x1BB:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		unsigned long listParam1 = packet.Decode4();
		unsigned long listParam2 = packet.Decode4();
		int guildCount = packet.Decode2();

		std::list<GUILD_LIST> guilds;
		for (int i = 0; i < guildCount; i++)
		{
			GUILD_LIST guild;
			packet.DecodeBuffer(&guild, 0xC4);

			guilds.push_back(guild);
		}

		GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 8, (intptr_t)&guilds,
			listParam1, listParam2, 0);
		break;
	}
	case 0x1C6:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 0xD, 0, 0, 0, 0);
		break;
	}
	case 0x1BE:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x11, result, 0, 0, 0);
		}
		else
		{
			GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 0xE, 0, 0, 0, 0);
		}
		break;
	}
	case 0x1C0:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 0xF, 0, 0, 0, 0);
		break;
	}
	case 0x1C1:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(s_guildActor[1]) << MsgObject(0, 0x263, 0x10, 0, 0, 0, 0);
		break;
	}
	case 0x1C2:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		unsigned long value = packet.Decode4();

		IActor* pActor = GetActor(s_guildActor[1]);
		pActor << MsgObject(0, 0x263, 0x13, value, 0, 0, 0);
		break;
	}
	case 0x1C5:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 0xC, 0, 0, 0, 0);
		break;
	}
	case 0x1B9:
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		unsigned long result = packet.Decode4();
		if (result != 1)
		{
			GetActor(s_guildActor[1])
				<< MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor* pActor = GetActor(s_guildActor[1]);

		pActor << MsgObject(0, 0x263, 0xB, 0, 0, 0, 0);
		break;
	}
	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
