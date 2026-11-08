#include "stdafx.h"
#include "intrusion.h"
#include "optiondlg.h"
#include "messengerdlg.h"
#include "mousecursor.h"
#include "bongdarishopdlg.h"
#include "user_info.h"
#include "cardsystem.h"
#include "contentsdoc.h"
#include "chatmsg.h"

extern Fresh* g_pFresh;

#pragma pack(push, 1)
struct sIntrusionScore
{
	unsigned char stroke[18];
	int score[18];
	__int64 pang[18];
};

struct sIntrusionReport
{
	sIntrusionScore score;
	unsigned long source;
	unsigned long target;
};
#pragma pack(pop)

unsigned char CIntrusion::GetCalcExpMemberSize()
{
	return m_calcExpMemberSize;
}

void CIntrusion::FinishGame()
{
	m_calcExpMemberSize = 0;
}

CIntrusion::CIntrusion()
	: m_pDlg(NULL), m_roomNumber(0), m_calcExpMemberSize(0)
{
	(void)&g_EPSILON;
}

CIntrusion::~CIntrusion()
{
}

unsigned long CIntrusion::GetActiveColor()
{
	return 0xc3e5ffd8;
}

bool CIntrusion::OnAccessRoom(sRoomInfo& room)
{
	if (room.bIntrusion != true || room.bAvailable)
		return false;
	room.bAvailable = true;
	return true;
}

void CIntrusion::DoIntrusionAccess(WReceivedPacket& packet,
	eIntrusionPermitCode code)
{
	switch (code)
	{
	case INTRUSION_PERMIT_ROOM:
	{
		Doc()->m_myInfo.info.dwIdentity &= ~2u;
		Doc()->m_myInfo.info.dwGalleryGuid = -1;
		Doc()->m_slotList.clear();
		Doc()->m_rivalList.clear();
		Doc()->m_guildMatchupList.clear();
		Doc()->m_bGameOver = false;
		packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));
		Doc()->m_chatManager.FilteringHack(Doc()->m_roomInfo.title, false);
		Doc()->m_myInfo.roomIndex = Doc()->m_roomInfo.roomGuid;
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN) &&
			CMessengerInfo::Instance()->GetComplete() == 4)
		{
			sUserPosition position;
			memset(&position, 0, sizeof(position));
			position.iServerGUID = Doc()->m_gameServerUID;
			position.iChannelUid = Doc()->m_curChannel.Uid;
			position.iRoomIdx = Doc()->m_roomInfo.roomGuid;
			position.iRoomType = Doc()->m_roomInfo.gameType;
			wsprintf(position.csChannelName, "%s", Doc()->m_curChannel.Name);
			WSendPacket send(35);
			send.EncodeBuffer(&position, sizeof(position));
			send.Send(TO_MSN);
		}
		break;
	}
	case INTRUSION_PERMIT_SLOTS:
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 46, 0, 0, 0, 0, 0);
		Doc()->m_chatLineList.clear();
		std::list<sSlotInfo>& slots = Doc()->m_slotList;
		slots.clear();
		unsigned char count = packet.Decode1();
		for (int i = 0; i < count; ++i)
		{
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));
			if (slot.bMaster)
				slot.bReady = true;
			else
				slot.bReady = false;
			slots.push_back(slot);
		}
		break;
	}
	case INTRUSION_PERMIT_MATCH:
		Doc()->m_expRate = packet.Decode4();
		break;
	case INTRUSION_PERMIT_RIVAL:
	{
		Doc()->ClearRivalVars(false);
		Doc()->m_myInfo.info.angelicWings = packet.Decode1();
		Doc()->m_myInfo.info.angelicWingsEffect = packet.Decode1();
		Doc()->m_myInfo.info.devilWings = packet.Decode1();
		if (IsLocalContent((localContentType_t)81))
		{
			SYSTEMTIME time;
			packet.DecodeBuffer(&time, sizeof(time));
			Doc()->SetServerTime(time);
			CCardManager::Instance()->m_playerSlot[0] = 0;
			unsigned long character = Doc()->m_myInfo.userEquip.guidChar;
			CHAR_MAP_IT it = Doc()->m_charMap.find(character);
			if (it != Doc()->m_charMap.end())
				CCardManager::Instance()->m_charInfo[0] = it->second;
		}
		Doc()->SetEquipCharCaddieInfoFromMyInfo();
		break;
	}
	case INTRUSION_PERMIT_GAME:
	{
		FrWnd* wnd = g_pFresh->GetDesktop()->FindChildByName("bongdari_shop");
		FrBongdariShopDlg* shop = wnd ? (FrBongdariShopDlg*)wnd
									  : DYNAMIC_CAST(FrBongdariShopDlg, wnd);
		if (shop)
			shop->Clear();
		if (CUserInfo::Instance())
			CUserInfo::Instance()->Clear();
		SetCurMap(packet.Decode1());
		SetGameType(packet.Decode1());
		SetHoleType(packet.Decode1());
		SetHoles(packet.Decode1());
		Doc()->m_roomType = packet.Decode4();
		SetShotTimeLimit(packet.Decode4());
		SetGameTimeLimit(packet.Decode4());
		if (!IsMassGame())
			for (int index = 0; index < 4; ++index)
				if (Doc()->m_userInfo[index].info.dwGuid ==
					Doc()->m_myInfo.info.dwGuid)
					break;
		for (int i = 0; i < 18; ++i)
		{
			Doc()->m_holeRandom[i] = packet.Decode4();
			Doc()->m_courseMap[i] = Doc()->m_holeRandom[i] % 3;
			Doc()->m_holeOrder[i] = packet.Decode1();
		}
		CGimmickContainer* gimmick = NULL;
		CContentsDoc::Instance()->GetContainer((localContentType_t)163,
			gimmick);
		gimmick->LoadFromPacket(packet);
		memset(Doc()->m_holePar, 0, sizeof(Doc()->m_holePar));
		Doc()->m_roomSetupStage = 0;
		Doc()->LogGstUpdateHole(-1, 0xff, -1, -1);
		if (IsLocalContent((localContentType_t)23))
			Doc()->SetInitWearHalloweenItem();
		if (CMessengerInfo::Instance()->GetMyStatus() != 3)
			CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)0);
		Doc()->m_bGameOver = false;
		CMouseCursor::Instance()->SetMode(1);
		CMouseCursor::Instance()->SetActive(false);
		COption::Instance()->vSetMinimumScreenSize(false, 800, 600);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
		IActor* score = AfxGetTask()->GetActor("Score");
		if (score)
		{
			score << MsgObject(NULL, 305, 0, 0, 0, 0, 0);
			score << MsgObject(NULL, 42, 0, 0, 0, 0, 0);
		}
		break;
	}
	}
}

void CIntrusion::PacketAnalysis(WReceivedPacket& packet)
{
	if (!m_pDlg)
	{
		unsigned char command = packet.Decode1();
		unsigned char code = packet.Decode1();
		switch (command)
		{
		case 3:
		{
			m_roomNumber = packet.Decode2();
			unsigned long passed = packet.Decode4();
			unsigned long limit = packet.Decode4();
			packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));
			Doc()->m_chatManager.FilteringHack(Doc()->m_roomInfo.title, false);
			Doc()->m_myInfo.roomIndex = Doc()->m_roomInfo.roomGuid;
			m_pDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"IntrusionAlarm");
			m_pDlg->SetCaption(
				"30\xc0\xce \xb4\xeb\xc8\xb8 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5 \xc8\xae\xc0\xce");
			m_pDlg->SetDesc(
				" \xa1\xd8 '\xbf\xb9'\xb8\xa6 \xb4\xa9\xb8\xa3\xb8\xe9 \xc1\xef\xbd\xc3 \xb0\xd4\xc0\xd3\xc0\xcc \xbd\xc3\xc0\xdb\xb5\xcb\xb4\xcf\xb4\xd9.");
			sInfoTime info;
			GetInfoTime(&info, passed, limit);
			const char* message = MakeStr(
				"\xc7\xd8\xb4\xe7 \xb0\xd4\xc0\xd3\xc0\xba \xb0\xd4\xc0\xd3 \xbd\xc3\xc0\xdb \xc8\xc4\012\\c0xffff0000\\c'%2d\xba\xd0%2d\xc3\xca'\\c0xff000000\\c\xb0\xa1 \xc1\xf6\xb3\xb5\xb0\xed \\c0xffff0000\\c'%2d\xba\xd0%2d\xc3\xca'\\c0xff000000\\c\xb0\xa1\012\xb3\xb2\xbe\xc6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012\012\xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
				info.passMin, info.passSec, info.remainMin, info.remainSec);
			m_pDlg->SetMessage(message, false);
			m_pDlg->Open((FRESH_PFN_RESULT)&CIntrusion::OnAlarmDlgResult, 1);
			break;
		}
		case 7:
		{
			std::string nick = packet.DecodeStr();
			CChatMsg::Instance()->AddChatMsg(
				MakeStr(
					"(\xbe\xcb\xb8\xb2) \\c0xffffffff\\c%s\\c0xffff7878\\c\xb4\xd4\xc0\xcc \xb0\xd4\xc0\xd3\xc1\xdf\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
					nick.c_str()),
				0xffff7878, true, false);
			packet.DecodeBuffer(&Doc()->m_roomInfo, sizeof(sRoomInfo));
			Doc()->m_chatManager.FilteringHack(Doc()->m_roomInfo.title, false);
			unsigned long match = Doc()->m_roomInfo.tidMatch;
			if (Doc()->m_roomType != match)
			{
				IFF_STRUCT::sMatch* info = ItemManager()->FindMatch(match);
				if (info)
				{
					Doc()->m_roomType = match;
					CChatMsg::Instance()->AddChatMsg(
						MakeStr(
							"(\xbe\xcb\xb8\xb2) \xb4\xeb\xc8\xb8 \xb5\xee\xb1\xde\xc0\xcc \\c0xffffffff\\c[ %s ]\\c0xffff7878\\c\xc0\xb8\xb7\xce \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
							info->Name),
						0xffff7878, true, false);
				}
			}
			std::list<sSlotInfo>& slots = Doc()->m_slotList;
			slots.clear();
			int count = packet.Decode1();
			unsigned long state = packet.Decode4();
			for (int i = 0; i < count; ++i)
			{
				sSlotInfo slot;
				packet.DecodeBuffer(&slot, sizeof(slot));
				if (slot.bMaster)
					slot.bReady = true;
				else
					slot.bReady = false;
				slots.push_back(slot);
			}
			PlayersUpdate(state);
			break;
		}
		case 4:
			DoIntrusionAccess(packet, (eIntrusionPermitCode)code);
			break;
		case 5:
			m_pDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"IntrusionAlarm");
			m_pDlg->SetCaption(
				"30\xc0\xce \xb4\xeb\xc8\xb8 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5");
			m_pDlg->SetDesc(
				" \xa1\xd8 '\xbf\xb9'\xb8\xa6 \xb4\xa9\xb8\xa3\xb8\xe9 \xb4\xd9\xbd\xc3 \xc0\xd4\xc0\xe5\xc0\xbb \xbd\xc3\xb5\xb5\xc7\xd5\xb4\xcf\xb4\xd9.");
			m_pDlg->SetMessage(
				"\xc0\xaf\xc0\xfa\xb0\xa1 \xb8\xb9\xbe\xc6 \xc0\xd4\xc0\xe5\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
				false);
			m_pDlg->Open((FRESH_PFN_RESULT)&CIntrusion::OnDelayDlgResult, 1);
			break;
		case 6:
			DenyAnlaysis((eIntrusionDenyCode)code);
			break;
		case 9:
		{
			unsigned long oid = packet.Decode4();
			unsigned char state = packet.Decode4();
			unsigned long myOid = Doc()->m_myInfo.info.dwGuid;
			if (oid == myOid)
			{
				for (RIVAL_LIST_IT it = Doc()->m_rivalList.begin();
					it != Doc()->m_rivalList.end(); ++it)
					it->quitOrder = state;
			}
			else
			{
				sIntrusionReport report;
				memset(&report, 0, sizeof(report));
				RIVAL_LIST_IT it = std::find_if(Doc()->m_rivalList.begin(),
					Doc()->m_rivalList.end(), sFindRival(myOid));
				if (it != Doc()->m_rivalList.end())
				{
					sIntrusionScore score;
					memset(&score, 0, sizeof(score));
					memcpy(score.score, it->holeScore, sizeof(score.score));
					memcpy(score.stroke, it->holeStroke, sizeof(score.stroke));
					memcpy(score.pang, it->holePang, sizeof(score.pang));
					report.score = score;
					report.source = oid;
					report.target = Doc()->m_myInfo.info.dwGuid;
				}
				else
				{
					report.source = oid;
					report.target = -1;
				}
				WSendPacket send((enumClientPacket)154);
				send.Encode1(2);
				send.EncodeBuffer(&report, sizeof(report));
				send.Send(TO_GAME);
			}
			break;
		}
		case 10:
			RivalDataUpdate(packet);
			break;
		case 8:
			m_calcExpMemberSize = packet.Decode1();
			break;
		}
	}
}

void CIntrusion::RivalDataUpdate(WReceivedPacket& packet)
{
	unsigned long oid = packet.Decode4();
	unsigned char stroke = packet.Decode1();
	unsigned char hole = packet.Decode1();
	int score = packet.Decode4();
	__int64 pang = packet.Decode8();
	sIntrusionReport report;
	memset(&report, 0, sizeof(report));
	packet.DecodeBuffer(&report, sizeof(report));
	if (oid != Doc()->m_myInfo.info.dwGuid)
	{
		RIVAL_LIST_IT it = std::find_if(Doc()->m_rivalList.begin(),
			Doc()->m_rivalList.end(), sFindRival(oid));
		if (it != Doc()->m_rivalList.end())
		{
			it->totalStroke = stroke;
			it->hole = hole;
			it->totalScore = score;
			it->totalPang = pang;
			memcpy(it->holeStroke, report.score.stroke, sizeof(it->holeStroke));
			memcpy(it->holeScore, report.score.score, sizeof(it->holeScore));
			memcpy(it->holePang, report.score.pang, sizeof(it->holePang));
			IActor* scoreActor = AfxGetTask()->GetActor("Score");
			if (scoreActor)
				scoreActor << MsgObject(NULL, 316, 0, 0, 0, 0, 0);
			IActor* spotActor = AfxGetTask()->GetActor("Spot");
			if (spotActor)
				spotActor << MsgObject(NULL, 251, 0, 0, 0, 0, 0);
		}
	}
}

void CIntrusion::PlayersUpdate(int state)
{
	std::list<sSlotInfo>& slots = Doc()->m_slotList;
	std::vector<sRivalData>& rivals = Doc()->m_rivalList;
	for (std::list<sSlotInfo>::iterator slot = slots.begin();
		slot != slots.end(); ++slot)
	{
		RIVAL_LIST_IT it = std::find_if(rivals.begin(), rivals.end(),
			sFindRival(slot->dwGuid));
		if (it != rivals.end())
		{
			it->quitOrder = state;
		}
		else if (slot->dwGuid == -1)
		{
			GOLFDOC()->GetPlayer(0)->state = 3;
		}
		else
		{
			Doc()->SetIndex(slot->dwGuid);
			sRivalData rival;
			memset(&rival, 0, sizeof(rival));
			strcpy(rival.nickname, slot->sNick);
			strcpy(rival.guildName, slot->sGuild);
			rival.uid = slot->dwUserUID;
			rival.order = state;
			rival.quitOrder = state;
			rival.capability = slot->dwIdentity;
			strcpy(rival.guildMark, slot->szEmblemName);
			rival.finishOrder = slots.size();
			rival.flag = slot->gender;
			rival.oid = slot->dwGuid;
			rival.guildUID = slot->GuildId;
			rival.hole = Doc()->m_holeOrder[0];
			rival.level = slot->level;
			rival.pangMastery = slot->pangma[0];
			rival.pangNitro = slot->pangma[1];
			rival.ballPos = GOLFDOC()->GetHoleData().tee;
			rival.team = slot->bTeam;
			rival.observerState = 1;
			rival.rank = 1;
			rival.observerRank = 1;
			rival.state = 0;
			rival.mascotTypeId = slot->tidMascot;
			rival.titleTypeId = slot->dwTitle;
			rivals.push_back(rival);
		}
	}
	IActor* score = AfxGetTask()->GetActor("Score");
	if (score)
		score << MsgObject(NULL, 316, 0, 0, 0, 0, 0);
	IActor* spot = AfxGetTask()->GetActor("Spot");
	if (spot)
		spot << MsgObject(NULL, 251, 0, 0, 0, 0, 0);
}

void CIntrusion::GetInfoTime(sInfoTime* info, unsigned long passTime,
	unsigned long remainTime)
{
	int passed = passTime / 1000;
	int remaining = remainTime / 1000 - passTime / 1000;
	info->passMin = passed / 60;
	info->passSec = passed - info->passMin * 60;
	info->remainMin = remaining / 60;
	info->remainSec = remaining - info->remainMin * 60;
}

bool CIntrusion::OnAlarmDlgResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		WSendPacket packet((enumClientPacket)154);
		packet.Encode1(1);
		packet.Encode2(m_roomNumber);
		packet.Send(TO_GAME);
		CSharedDoc* doc = Doc();
		unsigned long caddie = doc->m_myInfo.userEquip.guidCaddie;
		std::map<unsigned int, sCaddieInfo>& caddies = doc->m_caddieMap;
		CADDIE_MAP_IT it = caddies.find(caddie);
		if (it != caddies.end())
		{
			IFF_STRUCT::sCaddie* info =
				ItemManager()->FindCaddie(it->second.tid);
			if (it->second.Rent_flag && !it->second.Remain_Date && info &&
				info->MonthlyFee > 0)
				caddie = 0;
		}
		unsigned long equipment[4];
		memset(equipment, 0, sizeof(equipment));
		equipment[0] = Doc()->m_myInfo.userEquip.guidChar;
		Doc()->m_myInfo.userEquip.guidCaddie = caddie;
		equipment[1] = caddie;
		equipment[2] = Doc()->m_myInfo.userEquip.guidClubSet;
		equipment[3] = Doc()->m_myInfo.userEquip.tidBall;
		WSendPacket equipPacket((enumClientPacket)12);
		equipPacket.Encode1(7);
		equipPacket.EncodeBuffer(equipment, sizeof(equipment));
		equipPacket.Send(TO_GAME);
	}
	else
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 59, 0, 0, 0, 0, 0);
		Doc()->m_myInfo.roomIndex = 0xffff;
	}
	m_pDlg = NULL;
	return true;
}

bool CIntrusion::OnDelayDlgResult(int result, FrForm* form)
{
	if (result != FrOK)
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 59, 0, 0, 0, 0, 0);
		m_pDlg = NULL;
		return true;
	}
	m_pDlg = NULL;
	return true;
}

bool CIntrusion::OnDenyDlgResult(int result, FrForm* form)
{
	AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 59, 0, 0, 0, 0, 0);
	m_pDlg = NULL;
	return true;
}

void CIntrusion::DenyAnlaysis(eIntrusionDenyCode code)
{
	if (!m_pDlg)
	{
		m_pDlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "IntrusionDeny");
		m_pDlg->SetCaption(
			"30\xc0\xce \xb4\xeb\xc8\xb8 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5");
		m_pDlg->SetDesc(
			" \xa1\xd8 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5 \xb0\xe1\xb0\xfa");
		switch (code)
		{
		case INTRUSION_DENY_CROWDED:
			m_pDlg->SetMessage(
				"\xc0\xce\xbf\xf8\xc0\xcc \xb0\xa1\xb5\xe6 \xc2\xf7\xbc\xad \xb9\xe6\xbf\xa1 \xc0\xd4\xc0\xe5 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_LATE:
			m_pDlg->SetMessage(
				"\xb0\xd4\xc0\xd3 \xbd\xc3\xc0\xdb \xc8\xc4 5\xba\xd0\xc0\xcc \xc1\xf6\xb3\xad \xb9\xe6\xbf\xa1\xb4\xc2 \xc0\xd4\xc0\xe5 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_PRIVATE:
			m_pDlg->SetMessage(
				"\xba\xf1\xb0\xf8\xb0\xb3 \xb9\xe6\xbf\xa1\xb4\xc2 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_RECENT:
			m_pDlg->SetMessage(
				"\xc0\xcc\xc0\xfc\xbf\xa1 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5\xc7\xd1 \xb1\xe2\xb7\xcf\xc0\xcc \xc0\xd6\xbe\xee\xbc\xad \xb4\xe7\xba\xd0\xb0\xa3 \xc0\xd4\xc0\xe5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_CLOSED:
			m_pDlg->SetMessage(
				"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_PLAYED:
			m_pDlg->SetMessage(
				"\xc7\xd8\xb4\xe7 \xb9\xe6\xbf\xa1\xbc\xad \xc7\xc3\xb7\xb9\xc0\xcc\xc7\xd1 \xb1\xe2\xb7\xcf\xc0\xcc \xc0\xd6\xbe\xee\xbc\xad \xb0\xd4\xc0\xd3\xc1\xdf \xc0\xd4\xc0\xe5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		case INTRUSION_DENY_KICKED:
			m_pDlg->SetMessage(
				"\xc7\xd8\xb4\xe7 \xb9\xe6\xbf\xa1\xbc\xad \xb0\xad\xc1\xa6 \xc5\xf0\xc0\xe5\xb4\xe7\xc7\xd1 \xb1\xe2\xb7\xcf\xc0\xcc \xc0\xd6\xbe\xee\xbc\xad \xc0\xd4\xc0\xe5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;
		}
		m_pDlg->Open((FRESH_PFN_RESULT)&CIntrusion::OnDenyDlgResult, 3);
	}
}

bool CIntrusion::IsIntrusionGame(unsigned short roomNumber)
{
	bool result = false;
	for (std::list<sRoomInfo>::iterator it = Doc()->m_roomList.begin();
		it != Doc()->m_roomList.end(); it++)
		if ((*it).roomGuid == roomNumber)
		{
			result = (*it).bIntrusion;
			break;
		}
	return result;
}

void CIntrusion::DoJoinRoom(unsigned short roomNumber)
{
	m_roomNumber = roomNumber;
	WSendPacket packet((enumClientPacket)154);
	packet.Encode1(0);
	packet.Encode2(m_roomNumber);
	packet.Send(TO_GAME);
}
