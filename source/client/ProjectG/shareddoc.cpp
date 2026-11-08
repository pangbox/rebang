ILFILLTU_SHAREDDOC0
#include "exhibition.h"
#include "contentsdoc.h"
#include "cardsystem.h"
#include "partsinfo.h"
#include "golfball.h"
#include "../../shared/sharedtables.h"
#include "lobbytask.h"
#include "shoptask.h"
#include "frcombobox.h"
#include "../../shared/token.h"
#include "club.h"
#include "../../shared/s5/sharedutilities.h"
#include "standinginline.h"
#include "mathconsts.h"
#include <direct.h>

char* GetStr(char* str, char* out);
void LineParse(const char* line, std::vector<std::string>& tokens);

const char* defaultTitles_vs[] = {
	(0, "\xc6\xce\xbe\xdf~ \xc6\xce\xbe\xdf!"),
	(0, "\xb3\xca\xb6\xfb \xb3\xaa\xb6\xfb \xc6\xce\xbe\xdf~"),
	(0, "\xb9\xd9\xb6\xf7 \xc1\xc1\xc0\xba \xb9\xe6~"),
	(0, "\xc6\xce\xbe\xdf\xbf\xcd \xc7\xd4\xb2\xb2\xb6\xf3\xb8\xe9~!"),
	(0, "\xbf\xb9\xbb\xdb \xbd\xba\xc0\xae, \xb8\xda\xc1\xf8 \xbc\xa6!"),
	(0, "\xb3\xaf\xbe\xc6\xb6\xf3 \xbe\xc6\xc1\xee\xc5\xd8~"),
	(0, "\xc0\xfa \xc7\xcf\xb4\xc3\xc0\xc7 \xc6\xce\xc0\xcc \xb5\xc7\xbe\xee~"),
	(0, "\xc6\xce\xbe\xdf\xb0\xa1 \xc3\xd6\xb0\xed!"),
	(0, "\xb4\xd9\xc7\xd4\xb2\xb2 \xc6\xae\xb7\xb9\xc0\xfa \xc7\xe5\xc6\xae!"),
};

const char* defaultTitles_mass[] = {
	(0, "\xc8\xa6\xc0\xce\xbf\xf8\xc0\xbb \xb3\xbb \xc7\xb0\xbe\xc8\xbf\xa1"),
	(0, "\xc6\xce\xbe\xdf\xb4\xeb\xc8\xb8\xb7\xce \xb8\xf0\xc0\xcc\xbc\xbc\xbf\xe4~"),
	(0, "\xb8\xf0\xb5\xce\xb8\xf0\xb5\xce \xc6\xce\xbe\xdf!"),
	(0, "\xb1\xe2\xb7\xcf \xb0\xbb\xbd\xc5\xc0\xc7 \xb1\xd7\xb3\xaf\xb1\xee\xc1\xf6!"),
	(0, "1\xb5\xee \xb1\xe2\xb4\xd9\xb7\xc1~"),
	(0, "\xc6\xae\xb7\xce\xc7\xc7\xb4\xc2 \xb3\xbb\xb2\xa8~"),
};

const char* defaultTitles_battle[] = {
	(0, "\xbd\xac\xbe\xee\xb0\xa1\xb4\xc2 \xc7\xd1 \xc6\xc7!"),
	(0, "\xc7\xd1\xb9\xe6\xbd\xc2\xba\xce! \xc6\xce\xb9\xe8\xc6\xb2~"),
	(0, "\xb4\xeb\xb9\xda \xb2\xde, \xbd\xf1\xbe\xc6\xc1\xf6\xb4\xc2 \xc6\xce"),
	(0, "\xbb\xf5\xb7\xce\xbf\xee \xbd\xc3\xc1\xf0 \xc6\xce\xb9\xe8\xc6\xb2 \xc7\xd1 \xc6\xc7!"),
};

const char* defaultTitles_approach[] = {
	(0, "\xbd\xac\xbe\xee\xb0\xa1\xb4\xc2 \xc7\xd1 \xc6\xc7!"),
	(0, "\xc7\xd1\xb9\xe6\xbd\xc2\xba\xce! \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1~"),
	(0, "\xbd\xc3\xc1\xf0 NEW \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"),
};

const char* defaultTitles_chat[] = {
	(0, "\xbc\xd2\xb3\xe2, \xbc\xd2\xb3\xe0\xb8\xa6 \xb8\xb8\xb3\xaa\xb4\xd9"),
	(0, "\xbc\xf6\xb4\xd9\xc0\xef\xc0\xcc, \xbb\xf5\xc4\xa7\xb6\xbc\xb1\xe2"),
	(0, "\xc0\xce\xbf\xac\xc0\xcc \xb8\xf0\xc0\xcc\xb4\xc2 \xb0\xf7"),
	(0, "\xc6\xce\xbe\xdf! \xc1\xf1\xb0\xc5\xbf\xee \xb8\xb8\xb3\xb2~"),
	(0, "\xc6\xce\xbe\xdf\xbf\xf9\xb5\xe5\xbf\xcd \xc7\xd4\xb2\xb2!"),
	(0, "\xc6\xce\xbe\xdf\xc0\xce\xb5\xe9 \xb4\xd9 \xb8\xf0\xbf\xa9\xb6\xf3~!"),
	(0, "\xc4\xf0 \xb0\xa1\xc0\xcc! \xb3\xaa\xc0\xcc\xbd\xba \xb0\xc9!"),
};

CSharedDoc::CSharedDoc()
	: m_maxWhisperPartner(8), m_nicknameSuffix(""), m_serverProperty(0)
{
	m_bNeedFirstLoginAlarm = true;
	m_pGolfDoc = NULL;
	ClearAll();

	m_unused47cc = 0;
	m_unused47d0 = 0;

	memset(m_fileName, 0, sizeof(m_fileName));

	m_replayState = 0;
	m_replayPosition = 0;
	m_replayDuration = 0;
	m_replayTick = 0;
	m_replayPlayType = -1;
	m_playingMode = (eReplayModeType)1;
	m_replayShotIndex = 0;
	m_replayHoleStroke = 0;

	m_replayVersion = 0x18f1;
	m_rmrMode = 0;

	m_serverTimeTick = GetTickCount();
	m_cookie = 0;
	m_unused5d0c = 0;
	m_goStopMode = false;
	m_cash = 0;
	m_bonusCash = 0;
	m_directMoveType = 0xff;
	m_directMoveRoom = 0xffff;
	m_directMoveUID = 0xffffffff;
}

CSharedDoc::~CSharedDoc()
{
	std::map<unsigned long, sRoomSlot>::iterator it;
	for (it = Doc()->m_roomSlotMap.begin(); it != m_roomSlotMap.end(); ++it)
	{
		if ((*it).second.pExhibition)
		{
			delete (*it).second.pExhibition;
			(*it).second.pExhibition = NULL;
		}
	}
	m_roomSlotMap.clear();

	ClearRivalVars(false);
	UccManager()->Clear();
}

void CSharedDoc::ClearAll()
{
	ClearCommonVars();
	ClearInventory();
	ClearGameVars();
	ClearRivalVars(false);
	InitGameType();
	ClearLobbyStateVars();
	InitTradeData();

	ClearOldSeasonStat();
	ClearServerVars();
	ClearJapanVars();
	ClearRealEquip();

	if (IsLocalContent(S4_UCC))
	{
		ClearUcc();
	}

	if (IsLocalContent(S3_MAP_EVENT))
		m_mapEventList.clear();

	if (IsLocalContent(S4_REPLAY_SYSTEM))
		m_shotDataList.clear();

	SetUserInfoMod(0);
}

void CSharedDoc::ClearUcc()
{
	m_uccItemList.clear();
	m_uccAztecList.clear();
	UccManager()->Clear();
}

void CSharedDoc::ClearJapanVars()
{
	m_unused184 = 0;
	m_provType = -1;

	m_unused1c4 = 0;
	m_gachaTicket[0] = 0;
	m_gachaTicket[1] = 0;

	char cmdLine[260] = { 0 };
	char args[3][260];
	char temp[260];
	char* p;
	int len;

	strcpy(cmdLine, GetCommandLineA());

	for (int i = 0; i < 3; i++)
		args[i][0] = 0;

	if (cmdLine[0] == '"')
		p = GetStr(cmdLine, args[0]);
	else
	{
		sscanf(cmdLine, "%s", args[0]);
		p = cmdLine + strlen(args[0]);
	}

	sscanf(p, "%s %s", args[1], args[2]);

	if (args[1][0] != 0)
	{
		len = strlen(args[1]);

		if (args[1][0] == '"' && args[1][len - 1] == '"')
		{
			strncpy(temp, &args[1][1], len - 2);
			temp[len - 2] = 0;
			strcpy(args[1], temp);
			Doc()->m_commandLineID = temp;
		}
		else
			Doc()->m_commandLineID = args[1];
	}

	len = strlen(args[2]);

	if (args[2][0] == '"' && args[2][len - 1] == '"')
	{
		strncpy(temp, &args[2][1], len - 2);
		temp[len - 2] = 0;
		strcpy(args[2], temp);
		Doc()->m_password = temp;
	}
	else
		Doc()->m_password = args[2];
}

void CSharedDoc::ClearCommonVars()
{
	m_loginServerUID = 0;
	m_bGameOver = false;
	m_holeType = 0;
	ClearGameMode();
	m_roomType = 0;
	m_eventType = 0;
	m_eventFlag = 2;
	m_pangRate = 1.0f;
	m_expRate = 100;
	m_offlinePlay = 0;
	m_myGamePang = 0;
	m_newRecordState = 0xff;
	m_pcBang = 0;
	m_roomSetupStage = 0;

	m_bNewMapEvent = false;
	m_newMapEventMask = 0;
	m_bBingoEvent = false;
	m_newRecordCourse = 0xffffffff;

	memset(m_tutorialComplete, 0, sizeof(m_tutorialComplete));
	m_tutorialStartMode = 15;
	m_gameServerPort = 0;
	m_gameServerUID = 0;
	m_unused80 = 0;
	m_unused88 = 0;

	LoadLevelTable();

	for (unsigned char i = 0; i < 18; i++)
	{
		m_holeRandom[i] = rand();
		m_courseMap[i] = m_holeRandom[i] % 3;
		m_holeOrder[i] = i + 1;
	}
	m_holeOrder[i] = 19;

	m_noteList.clear();

	memset(&m_serverTime, 0, sizeof(m_serverTime));

	m_chatMode = 0;

	m_bTutorialResume = false;
	memset(m_auxPartTidList, 0, sizeof(m_auxPartTidList));

	m_guildBonusPang = 0;
	m_guildPoint = 0;
	m_guildTeam = 0;
	m_bHaveHalloweenItem = false;
	m_bWearHalloweenItem = false;
	m_giftName = "";

	m_giftIndex = -1;

	m_bRefreshCamera = false;
	m_bMessengerAlarm = false;
	m_bMessengerInit = false;
	m_friendMap.clear();
	memset(&m_selFriend, 0, sizeof(m_selFriend));

	m_needParentAgree = 0;
	m_controlServerService = 0;
	m_serverProperty = 0;
	m_flagAbilityItem = 0;
	m_numBonusPangItem = 0;
	m_cardPackPageNum = 0;
	m_cardPackSelPageNum = 0;
	m_cardPackCurPage = 0;
	m_cardPackMode = 1;
	m_cardPackSlot = 0;
	m_cardPackItem = 0;
	m_rmrTabValue = 0;
	m_bCardPackRebuild = false;
	m_bRmrTabPending = false;
	m_specialShotMark = 0;
	m_pendingMenu = 0;
	m_bToppageAd = false;
	m_toppageFeature = 0;

	if (IsLocalContent(S3_MAP_EVENT))
	{
	}

	m_appGiftDownloadState = (eAppGiftDownloadState)0;
	m_bItemStoragePin = false;
	m_bTutorialRedirect = false;
	m_unused5bdc = 0;
	m_replayTargetUID = 0xffffffff;

	if (IsLocalContent(S4_NT_NICKNAME_CHANGE))
	{
		m_forceNicknameChange = 0;
	}

	m_pPeriodDoc = NULL;
}

void CSharedDoc::ClearInventory()
{
	memset(&m_myInfo, 0, sizeof(m_myInfo));
	m_cookie = 0;
	m_cash = 0;
	m_bonusCash = 0;

	m_stringMap.clear();
	m_charMap.clear();
	m_caddieMap.clear();
	m_mascotMap.clear();
	m_partsList.clear();
	m_clubSetMap.clear();
	m_ballList.clear();
	m_myItemList.clear();
	m_recordedItemList.clear();
	m_cadItemList.clear();
	m_setItemList.clear();
	m_giftList.clear();
	m_cutinList.clear();
	m_specialTrophyList.clear();
	m_guildTrophyList.clear();
	m_auxPartList.clear();
	m_questDropList.clear();
	m_questList.clear();
	m_furnitureList.clear();
	m_refreshGuidList.clear();
	m_refreshCountList.clear();

	ClearUserInfoTimeMap(0xffffffff);
}

void CSharedDoc::ClearGameMode()
{
	m_gameMode = 0;
}

void CSharedDoc::ClearUserInfoTimeMap(unsigned long uid)
{
	std::map<unsigned long, sUserInfoTime>::iterator it;

	if (uid == 0xffffffff)
	{
		for (it = m_userInfoTimeMap.begin(); it != m_userInfoTimeMap.end();
			++it)
		{
			if (it->second.trophies)
			{
				delete[] it->second.trophies;
				it->second.trophies = NULL;
			}
			if (it->second.guildTrophies)
			{
				delete[] it->second.guildTrophies;
				it->second.guildTrophies = NULL;
			}

			if (it->second.oldSeason.trophies)
			{
				delete[] it->second.oldSeason.trophies;
				it->second.oldSeason.trophies = NULL;
			}
			if (it->second.oldSeason.guildTrophies)
			{
				delete[] it->second.oldSeason.guildTrophies;
				it->second.oldSeason.guildTrophies = NULL;
			}
		}
		m_userInfoTimeMap.clear();
	}
	else
	{
		it = m_userInfoTimeMap.find(uid);

		if (it != m_userInfoTimeMap.end())
		{
			if (it->second.trophies)
			{
				delete[] it->second.trophies;
				it->second.trophies = NULL;
			}
			if (it->second.guildTrophies)
			{
				delete[] it->second.guildTrophies;
				it->second.guildTrophies = NULL;
			}

			if (it->second.oldSeason.trophies)
			{
				delete[] it->second.oldSeason.trophies;
				it->second.oldSeason.trophies = NULL;
			}
			if (it->second.oldSeason.guildTrophies)
			{
				delete[] it->second.oldSeason.guildTrophies;
				it->second.oldSeason.guildTrophies = NULL;
			}
			m_userInfoTimeMap.erase(it);
		}
	}
}

void CSharedDoc::ClearGameVars()
{
	m_bSentHoleStat = false;
	m_bTreasureResult = false;
	m_bTreasureAward = false;
	memset(m_holeStatistics, 0, sizeof(m_holeStatistics));
	memset(m_teamResult, 0, sizeof(m_teamResult));
	memset(m_guildScore, 0, sizeof(m_guildScore));

	memset(m_guildRoundScore, 0, sizeof(m_guildRoundScore));

	m_holeStatistics[3].dwExp = 0xffffffff;
	m_holeStatistics[2].dwExp = 0xffffffff;
	m_holeStatistics[1].dwExp = 0xffffffff;
	m_holeStatistics[0].dwExp = 0xffffffff;

	m_startHole = 0;
	m_eventType = 0;
	m_eventFlag = 2;

	m_indexMap.clear();
}

void CSharedDoc::ClearRivalVars(bool bAll)
{
	if (!bAll)
		memset(m_userInfo, 0, sizeof(m_userInfo));

	memset(m_teamPlayerGuid, 0, sizeof(m_teamPlayerGuid));

	m_rivalList.clear();
	m_guildMatchupList.clear();
	m_guildMatchFlagList.clear();
}

int CSharedDoc::GetBuffValue(int type, int index)
{
	if (index != 20 && index != 16)
		return -1;

	return 0;
}

void CSharedDoc::ClearOldSeasonStat()
{
	memset(&m_oldSeasonStatistics, 0, sizeof(m_oldSeasonStatistics));
	memset(&m_oldSeasonTrophy, 0, sizeof(m_oldSeasonTrophy));

	m_oldSpecialTrophyList.clear();
	m_oldGuildTrophyList.clear();
}

void CSharedDoc::ClearStatistics()
{
}

void CSharedDoc::ClearCurrentChannel(bool bAll)
{
	memset(&m_curChannel, 0, sizeof(m_curChannel));
	m_curChannel.Uid = 0xff;
}

void CSharedDoc::ClearServerVars()
{
	ClearCurrentChannel(true);
	memset(&m_curGameServer, 0, sizeof(m_curGameServer));
	m_gameServerList.clear();
	m_channelList.clear();
}

void CSharedDoc::ClearLobbyStateVars()
{
	m_shopName = "";
	m_bAutoRefresh = false;
	m_bShowNotice = true;
	m_underBarMask = 0;
	m_bBackgroundVideo = false;
	m_bReturnLobbyNotice = false;
	m_bRebuildUnderBar = false;

	memset(&m_roomInfo, 0, sizeof(m_roomInfo));

	sRoomInfo info;
	m_roomInfo = info;

	m_briefUserInfoMap.clear();
	m_roomList.clear();
	m_slotList.clear();
	m_slotList2.clear();
	m_roomUserList.clear();
	m_guildMemberMap.clear();
}

void CSharedDoc::ClearRealEquip()
{
	memset(&m_realEquip, 0, sizeof(m_realEquip));
}

void CSharedDoc::InitGameType()
{
	m_gameTypeInfo[GAME_TYPE_STROKE].maxPlayer = 4;
	m_gameTypeInfo[GAME_TYPE_STROKE].minPlayer = 2;
	m_gameTypeInfo[GAME_TYPE_STROKE].holes = 3;
	m_gameTypeInfo[GAME_TYPE_STROKE].name =
		K2L_Compatibility("\xbd\xba\xc6\xae\xb7\xce\xc5\xa9");

	m_gameTypeInfo[GAME_TYPE_TEAM].maxPlayer = 4;
	m_gameTypeInfo[GAME_TYPE_TEAM].minPlayer = 2;
	m_gameTypeInfo[GAME_TYPE_TEAM].holes = 6;
	m_gameTypeInfo[GAME_TYPE_TEAM].name = K2L_Compatibility("\xb8\xc5\xc4\xa1");

	m_gameTypeInfo[GAME_TYPE_MATCH].maxPlayer = 2;
	m_gameTypeInfo[GAME_TYPE_MATCH].minPlayer = 2;
	m_gameTypeInfo[GAME_TYPE_MATCH].holes = 9;
	m_gameTypeInfo[GAME_TYPE_MATCH].name =
		K2L_Compatibility("\xb0\xb3\xc0\xce \xb7\xa1\xb4\xf5");

	m_gameTypeInfo[GAME_TYPE_30S].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_30S].minPlayer = 10;
	m_gameTypeInfo[GAME_TYPE_30S].holes = 9;
	m_gameTypeInfo[GAME_TYPE_30S].name =
		K2L_Compatibility("30\xc0\xce \xb4\xeb\xc8\xb8");

	m_gameTypeInfo[GAME_TYPE_30S_TEAM].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_30S_TEAM].minPlayer = 10;
	m_gameTypeInfo[GAME_TYPE_30S_TEAM].holes = 9;
	m_gameTypeInfo[GAME_TYPE_30S_TEAM].name =
		K2L_Compatibility("30\xc0\xce \xb4\xeb\xc8\xb8 \xc6\xc0\xc0\xfc");

	m_gameTypeInfo[GAME_TYPE_GUILD_MATCH].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_GUILD_MATCH].minPlayer = 10;
	m_gameTypeInfo[GAME_TYPE_GUILD_MATCH].holes = 9;
	m_gameTypeInfo[GAME_TYPE_GUILD_MATCH].name =
		K2L_Compatibility("\xb1\xe6\xb5\xe5 \xb4\xeb\xc0\xfc");

	m_gameTypeInfo[GAME_TYPE_REALMYROOM].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_REALMYROOM].minPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_REALMYROOM].holes = 1;
	m_gameTypeInfo[GAME_TYPE_REALMYROOM].name =
		K2L_Compatibility("\xb0\xf8\xb0\xb3 \xb8\xb6\xc0\xcc\xb7\xeb");

	m_gameTypeInfo[GAME_TYPE_SKINS].maxPlayer = 4;
	m_gameTypeInfo[GAME_TYPE_SKINS].minPlayer = 2;
	m_gameTypeInfo[GAME_TYPE_SKINS].holes = 6;
	m_gameTypeInfo[GAME_TYPE_SKINS].name =
		K2L_Compatibility("\xc6\xce\xb9\xe8\xc6\xb2");

	m_gameTypeInfo[GAME_TYPE_APPROACH].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_APPROACH].minPlayer = 10;
	m_gameTypeInfo[GAME_TYPE_APPROACH].holes = 1;
	m_gameTypeInfo[GAME_TYPE_APPROACH].name =
		K2L_Compatibility("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1");

	m_gameTypeInfo[GAME_TYPE_NEW_APPROACH].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_NEW_APPROACH].minPlayer = 6;
	m_gameTypeInfo[GAME_TYPE_NEW_APPROACH].holes = 3;
	m_gameTypeInfo[GAME_TYPE_NEW_APPROACH].name =
		K2L_Compatibility("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1");

	m_gameTypeInfo[GAME_TYPE_AVATARCHAT].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_AVATARCHAT].minPlayer = 10;
	m_gameTypeInfo[GAME_TYPE_AVATARCHAT].holes = 1;
	m_gameTypeInfo[GAME_TYPE_AVATARCHAT].name =
		K2L_Compatibility("\xb4\xeb\xc8\xad\xb9\xe6");

	m_gameTypeInfo[GAME_TYPE_TUTORIAL_BASIC].maxPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_BASIC].minPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_BASIC].holes = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_BASIC].name =
		K2L_Compatibility("\xc3\xb3\xc0\xbd \xbd\xc3\xc0\xdb\xc7\xcf\xb1\xe2");

	m_gameTypeInfo[GAME_TYPE_TUTORIAL_ADV].maxPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_ADV].minPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_ADV].holes = 1;
	m_gameTypeInfo[GAME_TYPE_TUTORIAL_ADV].name =
		K2L_Compatibility("\xc4\xb3\xb5\xf0\xbf\xcd \xb9\xe8\xbf\xec\xb1\xe2");

	m_gameTypeInfo[GAME_TYPE_OFFLINE_GHOST].maxPlayer = 255;
	m_gameTypeInfo[GAME_TYPE_OFFLINE_GHOST].minPlayer = 1;
	m_gameTypeInfo[GAME_TYPE_OFFLINE_GHOST].holes = 18;
	m_gameTypeInfo[GAME_TYPE_OFFLINE_GHOST].name =
		K2L_Compatibility("\xbd\xba\xc6\xae\xb7\xce\xc5\xa9");

	m_gameTypeInfo[GAME_TYPE_USEMAX].maxPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_USEMAX].minPlayer = 30;
	m_gameTypeInfo[GAME_TYPE_USEMAX].holes = 18;
	m_gameTypeInfo[GAME_TYPE_USEMAX].name = K2L_Compatibility(
		"\xba\xd2\xb0\xa1\xbb\xe7\xc0\xc7\xc7\xd1 \xc8\xa5\xb5\xb7");
}

static const unsigned long s_bonusExp[3] = { 520, 680, 840 };

void CSharedDoc::LoadLevelTable()
{
	for (int i = 0; i < 71; i++)
	{
		m_levelTable[i].exp = g_LevelTable[i].exp;
		m_levelTable[i].totalExp =
			(i == 0 ? 0 : m_levelTable[i - 1].totalExp) + m_levelTable[i].exp;
	}

	m_bonusPangTable[0].exp = s_bonusExp[0];
	m_bonusPangTable[0].pang = s_bonusExp[0];
	m_bonusPangTable[1].exp = s_bonusExp[1];
	m_bonusPangTable[1].pang = s_bonusExp[0] + s_bonusExp[1];
	m_bonusPangTable[2].exp = s_bonusExp[2];
	m_bonusPangTable[2].pang = s_bonusExp[0] + s_bonusExp[1] + s_bonusExp[2];
}

void CSharedDoc::LoadItemDb()
{
	m_itemManager.Load();

	CPartTidList::SetItemManager(&m_itemManager);
}

void CSharedDoc::InitGolfDoc()
{
	SetCurMap(0);
	SetGameType(GAME_TYPE_STROKE);
	Doc()->m_holeType = 0;
	SetHoles(18);
	SetWeather(0);
	Doc()->m_golfGame.shotTimeLimit = 40000;
	Doc()->m_golfGame.gameTimeLimit = 0;
}

void CSharedDoc::LoadChatFilter()
{
	m_chatManager.Load();
}

bool CSharedDoc::AreWePlayingTogether(unsigned long uid)
{
	switch (m_golfGame.gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
	case GAME_TYPE_MATCH:
	case GAME_TYPE_SKINS:
		if (GOLFDOC() == NULL)
			break;

		for (unsigned char i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state == 3)
				continue;

			if (PLAYER(i)->oid == uid)
				return true;
		}
		break;

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
	{
		std::vector<sRivalData>::iterator it;
		for (it = m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
		{
			if (it->oid == uid)
				return it->state != 3;
		}
	}
	break;
	}

	return false;
}

bool CSharedDoc::SetWinningPrize(unsigned long uid, const sWinningPrize& prize)
{
	if (GOLFDOC() == NULL)
		return false;

	for (unsigned char i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->oid == uid)
		{
			PLAYER(i)->prize = prize;
			return true;
		}
	}

	return false;
}

bool CSharedDoc::SetExtPrize(unsigned long uid, int num, sPrizeInfo* prize)
{
	if (GOLFDOC() == NULL)
		return false;

	for (unsigned char i = 0; i < GetPlayerNum(); i++)
	{
		if (PLAYER(i)->oid == uid)
		{
			if (num > 0)
			{
				int k = 0;
				for (unsigned int j = 0; j < 128; j++)
				{
					if (j >= PLAYER(i)->extPrizeNum && k < num)
					{
						PLAYER(i)->extPrize[j] = prize[k];
						k++;
					}
				}
			}
			PLAYER(i)->extPrizeNum += num;
			return true;
		}
	}

	return false;
}

bool CSharedDoc::CanCompound(unsigned long uid)
{
	IFF_STRUCT::sQuest* pQuest = m_itemManager.FindQuest(uid);
	if (pQuest == NULL)
		return false;

	std::list<sItemInfo>::iterator it;
	for (int i = 0; i < 5; i++)
	{
		unsigned long tid = pQuest->DropTid[i];
		if (tid == 0)
			continue;

		for (it = m_questDropList.begin(); it != m_questDropList.end(); ++it)
		{
			if (tid == (*it).tid)
			{
				if ((*it).Common[0] < pQuest->DropNum[i])
					return false;
				break;
			}
		}

		if (it == m_questDropList.end())
			return false;
	}

	return true;
}

float CSharedDoc::GetAuxPartProperty(unsigned char player, unsigned char type)
{
	if (m_pGolfDoc)
	{
		if (GetPlayerNum() <= player)
		{
			if (type == 0)
				return 0.0f;
			return 1.0f;
		}
	}

	unsigned long* pAuxParts = Doc()->m_userInfo[player].charInfo.tidAuxParts;

	float value = (type == 0) ? 0.0f : 1.0f;

	for (int i = 0; i < 5; i++)
	{
		if (pAuxParts[i] == 0)
			continue;

		IFF_STRUCT::sAuxPart* pAuxPart =
			ItemManager()->FindAuxPart(pAuxParts[i]);
		if (pAuxPart == NULL)
			continue;

		switch (type)
		{
		case IFF_STRUCT::sAuxPart::AUX_DRIVEUP:
			value += pAuxPart->DriveUp;
			break;
		case IFF_STRUCT::sAuxPart::AUX_ITEMDROPUP:
			if (pAuxPart->ItemDropUp > 0)
				value = value * pAuxPart->ItemDropUp * 0.01f;
			break;
		case IFF_STRUCT::sAuxPart::AUX_COMBOUP:
			if (pAuxPart->ComboUp > 0)
				value = value * pAuxPart->ComboUp * 0.01f;
			break;
		case IFF_STRUCT::sAuxPart::AUX_PANGUP:
			if (pAuxPart->PangUp > 0)
				value = value * pAuxPart->PangUp * 0.01f;
			break;
		case IFF_STRUCT::sAuxPart::AUX_COMBO_BONUS:
			if ((pAuxPart->c.TypeId & 0xffff) == 9)
			{
				value = 9.0f;
			}
			else if (IsLocalContent(S3_MYSTIC_RING) &&
				(pAuxPart->c.TypeId & 0xffff) == 11)
			{
				value = 11.0f;
			}
			break;
		}
	}

	return value;
}

float CSharedDoc::GetWtPepPangyaComboGauge(unsigned char player)
{
	unsigned long tids[] = {
		0x08022808,
		0x08022809,
		0x08062808,
		0x08062809,
		0x080a6808,
		0x080a6809,
		0x080e2808,
		0x080e2809,
		0x08128808,
		0x08128809,
		0x0816a808,
		0x0816a809,
		0x081a0808,
		0x081a0809,
		0x081e6808,
		0x081e6809,
		0x08222808,
		0x08222809,
		0x08262808,
		0x08262809,
	};
	float gauge = 0.0f;

	for (int i = 0; i < 24; i++)
	{
		unsigned long tid = Doc()->m_userInfo[player].charInfo.tidParts[i];

		for (int j = 0; j < sizeof(tids) / sizeof(tids[0]); j++)
		{
			if (tids[j] == tid)
			{
				gauge = 16.0f;
				break;
			}
		}

		if (gauge > 0.0f)
			break;
	}

	return gauge;
}

bool CSharedDoc::FindEquipPart(unsigned char player, unsigned long typeId)
{
	bool bFind = false;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
	{
		for (int i = 0; i < GetPlayerNum(); i++)
		{
			if (PLAYER(i)->team == player)
			{
				for (int j = 0; j < 24; j++)
				{
					if (typeId == Doc()->m_userInfo[i].charInfo.tidParts[j])
					{
						bFind = true;
						break;
					}
				}
				if (bFind)
					break;
			}
		}
	}
	else
	{
		for (int j = 0; j < 24; j++)
		{
			if (typeId == Doc()->m_userInfo[player].charInfo.tidParts[j])
			{
				bFind = true;
				break;
			}
		}
	}

	return bFind;
}

float CSharedDoc::GetComboGaugeLimit(unsigned char team)
{
	static unsigned long s_tids[] = {
		0x08008816,
		0x08008817,
		0x08046822,
		0x08046823,
		0x08088816,
		0x08088817,
		0x080c8027,
		0x080c8028,
		0x08106019,
		0x0810601a,
		0x08148025,
		0x08148026,
		0x0818601d,
		0x0818601e,
		0x081c801a,
		0x081c801b,
		0x08208026,
		0x08208027,
		0x08248026,
		0x08248027,
		0x08008818,
		0x08008819,
		0x08046828,
		0x08046829,
		0x0808881b,
		0x0808881c,
		0x080c802e,
		0x080c802f,
		0x0810601c,
		0x0810601d,
		0x0814802d,
		0x0814802e,
		0x08186027,
		0x08186028,
		0x081c801c,
		0x081c801d,
		0x0820802f,
		0x08208030,
		0x0824802b,
		0x0824802c,
	};
	float bonus = 0.0f;

	for (int i = 0; i < sizeof(s_tids) / sizeof(s_tids[0]); i++)
	{
		if (FindEquipPart(team, s_tids[i]))
		{
			bonus = 33.0f;
			break;
		}
	}

	return bonus + 99.0f;
}

void CSharedDoc::AddChar(sCharacterInfo* pInfo)
{
	CPartTidList tidList;

	if (!tidList.SetTids(pInfo, 0xff) || !tidList.IsComboValid())
	{
		tidList.SetDefaultTids();
		memcpy(pInfo->tidParts, tidList.m_tid, sizeof(pInfo->tidParts));
	}

	m_charMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(
		pInfo->guid, *pInfo));
}

void CSharedDoc::AddCaddie(sCaddieInfo* pInfo)
{
	m_caddieMap.insert(
		std::map<unsigned int, sCaddieInfo>::value_type(pInfo->guid, *pInfo));
}

void CSharedDoc::AddMascot(sMascotInfo* pInfo)
{
	std::map<unsigned int, sMascotInfo>::iterator it =
		m_mascotMap.find(pInfo->guid);

	if (it != m_mascotMap.end())
	{
		(*it).second = *pInfo;
	}
	else
	{
		m_mascotMap.insert(std::map<unsigned int, sMascotInfo>::value_type(
			pInfo->guid, *pInfo));
	}
}

sCardStack* CSharedDoc::GetCardInfoToUID(unsigned long uid)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		sCardStack* pCard = &(*it);
		if (uid == pCard->uid && pCard->bValid == 1)
		{
			return pCard;
		}
	}

	return NULL;
}

sCardStack* CSharedDoc::GetCardInfoToTID(unsigned long tid)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		sCardStack* pCard = &(*it);
		if (tid == pCard->typeId && pCard->bValid == 1)
		{
			return pCard;
		}
	}

	return NULL;
}

void CSharedDoc::ReplaceCardCountToTID(unsigned long tid, int count)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		if (tid == (*it).typeId && (*it).bValid == 1)
		{
			(*it).count = count;
			if (count <= 0)
				(*it).bValid = 0;
			return;
		}
	}
}

void CSharedDoc::ReplaceCardCountToUID(unsigned long uid, int count)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		if (uid == (*it).uid && (*it).bValid == 1)
		{
			(*it).count = count;
			if (count <= 0)
				(*it).bValid = 0;
			return;
		}
	}
}

void CSharedDoc::DeleteCardToPartsAttach(unsigned long uid)
{
	std::list<sSCardAvilityPeriodInfo>::iterator it;
	for (it = Doc()->m_cardAbilityList[0].begin();
		it != Doc()->m_cardAbilityList[0].end(); it++)
	{
		if ((*it).partsUid == uid)
		{
			Doc()->m_cardAbilityList[0].erase(it);
			return;
		}
	}
}

void CSharedDoc::AddCardStack(sCardStack card, bool bReplace)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		if (card.typeId == (*it).typeId && card.bValid == 1)
		{
			if (bReplace == true)
				(*it).count = card.count;
			else
				(*it).count += card.count;

			if ((*it).count <= 0)
				(*it).bValid = 0;
			return;
		}
	}

	if (it == Doc()->m_cardStackList.end())
	{
		Doc()->m_cardStackList.push_back(card);
	}
}

void CSharedDoc::DeleteCardStack(unsigned long uid, int count)
{
	for (std::list<sCardStack>::iterator it = Doc()->m_cardStackList.begin();
		it != Doc()->m_cardStackList.end(); ++it)
	{
		sCardStack* pCard = &(*it);
		if (uid == pCard->uid)
		{
			pCard->count -= count;
			if (pCard->count < 1)
			{
				pCard->count = 0;
				pCard->bValid = 0;
			}
		}
	}
}

void CSharedDoc::AddCard(unsigned long typeId, unsigned long uid,
	unsigned short count)
{
	std::list<sCards>::iterator it;
	for (it = Doc()->m_cardList.begin(); it != Doc()->m_cardList.end(); ++it)
	{
		if ((*it).tid == typeId)
		{
			(*it).cardCount += count;
			return;
		}
	}

	if (it == Doc()->m_cardList.end())
	{
		sCards card;
		memset(&card, 0, sizeof(card));
		card.tid = typeId;
		card.uid = uid;
		card.cardCount = count;
		Doc()->m_cardList.push_back(card);
	}
}

void CSharedDoc::AddFurniture(const sFurniture_List& furniture)
{
	m_furnitureList.push_back(furniture);
}

void CSharedDoc::AddMyPartsList(const sItemInfo& item)
{
	m_partsList.push_back(item);
}

sMascotInfo* CSharedDoc::GetMyMascotInfo(unsigned long uid)
{
	if (uid)
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.find(uid);

		if (it != Doc()->m_mascotMap.end())
		{
			return &(*it).second;
		}
	}

	return NULL;
}

sMascotInfo* CSharedDoc::GetMyEquippedMascotInfo()
{
	if (m_myInfo.userEquip.guidMascot)
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.find(m_myInfo.userEquip.guidMascot);

		if (it != Doc()->m_mascotMap.end())
		{
			return &(*it).second;
		}
	}

	return NULL;
}

int CSharedDoc::GetNumAddItemSlotByMascot(unsigned char player)
{
	int num = 0;
	unsigned long tid = m_userInfo[player].mascotInfo.tid;
	IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(tid);

	if (pMascot)
	{
		if (pMascot->ItemSlot <= 1)
			num = pMascot->ItemSlot;
	}

	return num;
}

int CSharedDoc::GetNumAddItemSlotByMascot()
{
	int num = 0;
	sMascotInfo* pInfo = GetMyEquippedMascotInfo();

	if (pInfo)
	{
		if (pInfo->Remain_Date > 0)
		{
			if (ItemManager()->CompareSystemTime(pInfo->endDate,
					Doc()->GetServerTime()) > 0)
			{
				IFF_STRUCT::sMascot* pMascot =
					ItemManager()->FindMascot(pInfo->tid);

				if (pMascot)
				{
					if (pMascot->ItemSlot <= 1)
						num = pMascot->ItemSlot;
				}
			}
		}
	}

	return num;
}

void CSharedDoc::SendEquipItemSlotByMascot()
{
	sMascotInfo* pInfo = GetMyEquippedMascotInfo();

	if (pInfo)
	{
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pInfo->tid);

		if (pMascot && pMascot->ItemSlot > 0)
		{
			if (ItemManager()->CompareSystemTime(pInfo->endDate,
					Doc()->GetServerTime()) > 0)
				return;
		}
	}

	if (m_myInfo.userEquip.tidItemSlot[9])
	{
		unsigned long slots[10] = { 0 };
		memcpy(slots, Doc()->m_myInfo.userEquip.tidItemSlot, sizeof(slots));
		slots[9] = 0;

		WSendPacket packet((enumClientPacket)0x20);
		packet.Encode1(2);
		packet.EncodeBuffer(slots, sizeof(slots));
		packet.Send(TO_GAME);
	}
}

void CSharedDoc::SendEquipItemSlot()
{
	sMascotInfo* pInfo = GetMyEquippedMascotInfo();
	CCardManager::Instance()->SetPlayerIndex(0xff);
	CCardManager::Instance()->CalcCardPeriodAndStatus();
	int cardSlot = CCardManager::Instance()->GetCardPeriodSlot();

	if (pInfo)
	{
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pInfo->tid);

		if (pMascot && pMascot->ItemSlot > 0)
		{
			if (ItemManager()->CompareSystemTime(pInfo->endDate,
					Doc()->GetServerTime()) > 0 &&
				cardSlot > 0)
				return;
		}
	}

	if (m_myInfo.userEquip.tidItemSlot[9] || m_myInfo.userEquip.tidItemSlot[8])
	{
		unsigned long slots[10];
		memcpy(slots, m_myInfo.userEquip.tidItemSlot, sizeof(slots));

		WSendPacket packet((enumClientPacket)0x20);
		packet.Encode1(2);
		packet.EncodeBuffer(slots, sizeof(slots));
		packet.Send(TO_GAME);
	}
}

void CSharedDoc::SetCurEquipInfo(sUserEquip* pEquip)
{
	m_myInfo.userEquip = *pEquip;
	BuildMyPartTidList();

	if (m_myInfo.userEquip.guidCaddie)
	{
		if (m_myInfo.userEquip.guidCaddie != 0xffffffff)
		{
			if (m_caddieMap.find(m_myInfo.userEquip.guidCaddie) ==
				m_caddieMap.end())
			{
				if (m_caddieMap.size() > 0)
				{
					m_myInfo.userEquip.guidCaddie =
						(*m_caddieMap.begin()).first;
				}
			}
		}
	}

	if (m_myInfo.userEquip.guidMascot)
	{
		if (m_myInfo.userEquip.guidMascot != 0xffffffff)
		{
			if (m_mascotMap.find(m_myInfo.userEquip.guidMascot) ==
				m_mascotMap.end())
			{
				m_myInfo.userEquip.guidMascot = 0;
			}
		}
	}
}

void CSharedDoc::CheckCurrEquipParts()
{
	CPartTidList tidList;
	CPartList partList;
	bool bSendChar = false;
	std::map<unsigned int, sCharacterInfo>::iterator it;

	for (it = m_charMap.begin(); it != m_charMap.end(); ++it)
	{
		bool bChanged = false;
		sCharacterInfo* pInfo = &it->second;

		if (!tidList.SetTids(pInfo, m_myInfo.stat.Level))
		{
			tidList.SetDefaultTids();
			memcpy(pInfo->tidParts, tidList.m_tid, sizeof(pInfo->tidParts));
		}
		else
		{
			partList.Build(&tidList);

			for (int i = 0; i < 24; i++)
			{
				unsigned long tid = pInfo->tidParts[i];
				if (!tid || (tid & 0x600))
					continue;

				std::list<sItemInfo>::iterator itPart;
				for (itPart = m_partsList.begin(); itPart != m_partsList.end();
					itPart++)
				{
					if ((*itPart).tid == tid)
						break;
				}

				if (itPart == m_partsList.end())
				{
					bChanged = true;
					partList.SetPart(&tidList, tid, 0);
				}
			}

			if (!bChanged)
				continue;

			memcpy(pInfo->tidParts, tidList.m_tid, sizeof(pInfo->tidParts));
		}

		if (pInfo->guid == m_myInfo.userEquip.guidChar)
			bSendChar = true;

		WSendPacket packet((enumClientPacket)0x20);
		packet.Encode1(0);
		packet.EncodeBuffer(pInfo, sizeof(sCharacterInfo));
		packet.Send(TO_GAME);
	}

	if (bSendChar)
	{
		WSendPacket packet((enumClientPacket)0x20);
		packet.Encode1(5);
		packet.Encode4(m_myInfo.userEquip.guidChar);
		packet.Send(TO_GAME);
	}
}

bool CSharedDoc::BuildMyPartTidListDefault(unsigned long typeId)
{
	IFF_STRUCT::sChar* pChar = m_itemManager.FindChar(typeId);
	if (pChar)
	{
		m_partTidList.m_charTid = typeId;
		m_partTidList.m_defPartNum = pChar->nParts;
		m_partTidList.m_partNum = pChar->nParts + pChar->nAcsries;
		m_partTidList.m_hairColor = 0;
		m_partTidList.m_shirtsColor = 0;
		m_partTidList.SetDefaultTids();

		memset(m_auxPartTidList, 0, sizeof(m_auxPartTidList));
		return true;
	}

	return false;
}

void CSharedDoc::BuildMyPartTidList()
{
	sCharacterInfo* pInfo;
	sCharacterInfo defInfo;
	std::map<unsigned int, sCharacterInfo>::iterator it;
	it = m_charMap.find(m_myInfo.userEquip.guidChar);

	if (it == m_charMap.end())
	{
		memset(&defInfo, 0, sizeof(defInfo));
		defInfo.tid = 0x4000000 | (Doc()->m_myInfo.info.gender % 2);
		ItemManager()->GetDefCombo(defInfo.tid, defInfo.tidParts);
		pInfo = &defInfo;
	}
	else
		pInfo = &it->second;

	if (!m_partTidList.SetTids(pInfo, Doc()->m_myInfo.stat.Level))
	{
		m_partTidList.SetDefaultTids();
	}

	if (!m_partTidList.IsComboValid())
	{
		m_partTidList.SetDefaultTids();
	}

	memcpy(m_auxPartTidList, pInfo->tidAuxParts, sizeof(m_auxPartTidList));
}

void CSharedDoc::BuildMyPartTidList(sCharacterInfo& info)
{
	if (!m_partTidList.SetTids(&info, Doc()->m_myInfo.stat.Level))
	{
		m_partTidList.SetDefaultTids();
	}

	if (!m_partTidList.IsComboValid())
	{
		m_partTidList.SetDefaultTids();
	}

	memcpy(m_auxPartTidList, info.tidAuxParts, sizeof(m_auxPartTidList));
}

void CSharedDoc::BuildTikiReportList()
{
	if (IsLocalContent(S3_CADDIE_REPORT) && m_caddieReportList.size())
	{
		Doc()->m_rivalList.clear();
		Doc()->m_roomInfo.nUserLimit = 30;

		std::vector<sCaddieReportData>::iterator it;
		for (it = m_caddieReportList.begin(); it != m_caddieReportList.end();
			it++)
		{
			sRivalData rival;
			memset(&rival, 0, sizeof(rival));
			rival.uid = it->uid;
			rival.oid = it->uid;
			strcpy(rival.nickname, it->nickname);
			rival.level = it->level;
			rival.totalScore = it->score;
			rival.totalPang = it->pang;
			rival.totalBonusPang = it->bonusPang;
			rival.guildUID = it->guildUID;
			strcpy(rival.guildMark, it->guildMark);
			rival.mascotTypeId = it->mascotTypeId;
			rival.holeStroke[0] = it->eventType;
			unsigned char team = it->team;
			if (team & 4)
			{
				team ^= 4;
				rival.state = 3;
			}
			rival.team = team;

			if (strcmp(rival.nickname, MyNick()) == 0)
			{
				Doc()->m_eventType = it->eventType;
				Doc()->m_holeStatistics[0].dwExp = it->exp;
				Doc()->m_roomType = it->roomType;
				Doc()->m_golfGame.gameType = it->gameType;
				Doc()->m_eventFlag = it->eventFlag;
			}

			rival.capability = it->premium ? 0x80 : 0;
			rival.pangMastery = 0;
			rival.pangNitro = 0;

			if (it->awardFlag & 1)
				m_awardItem[0].uid = rival.uid;
			if (it->awardFlag & 2)
				m_awardItem[1].uid = rival.uid;
			if (it->awardFlag & 4)
				m_awardItem[2].uid = rival.uid;
			if (it->awardFlag & 8)
				m_awardItem[3].uid = rival.uid;
			if (it->awardFlag & 0x10)
				m_awardItem[4].uid = rival.uid;
			if (it->awardFlag & 0x20)
				m_awardItem[5].uid = rival.uid;

			switch (it->pangItem)
			{
			case 1:
				rival.pangMastery = 1;
				break;
			case 2:
				rival.pangNitro = 1;
				break;
			case 3:
				rival.pangNitro = 1;
				rival.pangMastery = 1;
				break;
			}

			m_rivalList.push_back(rival);
		}
	}
}

CPartTidList& CSharedDoc::GetMyPartTidList()
{
	return m_partTidList;
}

unsigned long* CSharedDoc::GetMyAuxPartTidList()
{
	return m_auxPartTidList;
}

void CSharedDoc::SetIndex(unsigned long uid)
{
	if (m_indexMap.find(uid) != m_indexMap.end())
		return;

	unsigned char index = (unsigned char)m_indexMap.size();

	if (OnlinePlay())
	{
		if (IsMassGame())
		{
			if (uid == m_myInfo.info.dwGuid)
				m_userInfo[0].info = m_myInfo.info;
		}
	}

	m_indexMap[uid] = index;
}

bool CompareItemInfo(const sItemInfo& a, const sItemInfo& b)
{
	return a.tid < b.tid;
}

void CSharedDoc::SortAllMyItemList()
{
	m_partsList.sort(CompareItemInfo);
	m_ballList.sort(CompareItemInfo);
	m_myItemList.sort(CompareItemInfo);
	m_cadItemList.sort(CompareItemInfo);
	m_setItemList.sort(CompareItemInfo);
}

bool CSharedDoc::ConfirmInsertItem(unsigned long typeId,
	unsigned long count) const
{
	unsigned long category = typeId >> 26;
	if (category >= 5 && category <= 6)
	{
		if (typeId != 0x1a000010)
		{
			for (std::list<sItemInfo>::const_iterator it = m_myItemList.begin();
				it != m_myItemList.end(); ++it)
			{
				if (sItemInfo(*it).tid == typeId)
				{
					if (sItemInfo(*it).Common[0] + count > 20000)
						return false;
					return true;
				}
			}
			return true;
		}
	}

	return true;
}

void CSharedDoc::AddMyItem(const sItemInfo& item)
{
	Doc()->m_myItemList.push_back(item);
}

bool CSharedDoc::DecreaseMyItem(std::list<sItemInfo>::iterator& it)
{
	sItemInfo* pItem = &(*it);
	pItem->Common[0]--;
	if (pItem->Common[0] <= 0)
		DeleteMyItem(it);

	return true;
}

bool CSharedDoc::DeleteMyItem(unsigned long typeId)
{
	std::list<sItemInfo>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end(); it++)
	{
		if ((*it).tid == typeId)
			break;
	}

	return DeleteMyItem(it);
}

bool CSharedDoc::DeleteMyItem(std::list<sItemInfo>::iterator& it)
{
	if (it == m_myItemList.end())
		return false;

	m_myItemList.erase(it);
	return true;
}

sItemInfo* CSharedDoc::FindMyItemByGuid(unsigned long guid)
{
	std::list<sItemInfo>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end(); it++)
	{
		if ((*it).guid == guid)
			return &(*it);
	}

	return NULL;
}

void CSharedDoc::ClearMyGuildInfo()
{
	m_myInfo.info.dwGuildId = 0;
	m_myInfo.info.dwEmblemVer = 0;
	memset(m_myInfo.info.sGuild, 0, sizeof(m_myInfo.info.sGuild));
	memset(m_myInfo.info.szEmblemName, 0, sizeof(m_myInfo.info.szEmblemName));
}

void CSharedDoc::SetEquipCharCaddieInfoFromMyInfo()
{
	std::map<unsigned int, sCharacterInfo>::iterator itChar;
	std::map<unsigned int, sCaddieInfo>::iterator itCaddie;
	std::map<unsigned int, sMascotInfo>::iterator itMascot;
	std::map<unsigned int, sItemInfo>::iterator itClub;

	memset(&m_userInfo[0].charInfo, 0, sizeof(sCharacterInfo));
	memset(&m_userInfo[0].caddieInfo, 0, sizeof(sCaddieInfo));
	memset(&m_userInfo[0].clubInfo, 0, sizeof(sClubInfo));
	memset(&m_userInfo[0].mascotInfo, 0, sizeof(sMascotInfo));

	m_userInfo[0].info = m_myInfo.info;
	m_userInfo[0].stat = m_myInfo.stat;
	m_userInfo[0].userEquip = m_myInfo.userEquip;

	itChar = m_charMap.find(m_myInfo.userEquip.guidChar);
	if (itChar != m_charMap.end())
		m_userInfo[0].charInfo = (*itChar).second;

	itCaddie = m_caddieMap.find(m_myInfo.userEquip.guidCaddie);
	if (itCaddie != m_caddieMap.end())
	{
		m_userInfo[0].caddieInfo = (*itCaddie).second;
		IFF_STRUCT::sCaddie* pCaddie =
			ItemManager()->FindCaddie(m_userInfo[0].caddieInfo.tid);
		if ((pCaddie && m_myInfo.stat.Level < pCaddie->c.Level) ||
			(Doc()->m_userInfo[0].caddieInfo.Rent_flag &&
				Doc()->m_userInfo[0].caddieInfo.Remain_Date == 0 &&
				pCaddie->MonthlyFee > 0))
			m_userInfo[0].caddieInfo.tid = 0;
	}

	itMascot = m_mascotMap.find(m_myInfo.userEquip.guidMascot);
	if (itMascot != m_mascotMap.end())
	{
		m_userInfo[0].mascotInfo = (*itMascot).second;
		IFF_STRUCT::sMascot* pMascot =
			ItemManager()->FindMascot(m_userInfo[0].mascotInfo.tid);
	}

	itClub = m_clubSetMap.find(m_myInfo.userEquip.guidClubSet);
	if (itClub != m_clubSetMap.end())
	{
		IFF_STRUCT::sClubSet* pClubSet =
			ItemManager()->FindClubSet((*itClub).second.tid);
		if (pClubSet &&
			((m_myInfo.stat.Level < pClubSet->c.Level &&
				 !pClubSet->c.IsUnderLvl) ||
				(m_myInfo.stat.Level > pClubSet->c.Level &&
					pClubSet->c.IsUnderLvl)))
		{
			std::map<unsigned int, sItemInfo>::iterator it;
			for (it = m_clubSetMap.begin(); it != m_clubSetMap.end(); ++it)
			{
				if ((*it).second.tid == 0x10000000)
				{
					itClub = it;
					break;
				}
			}
		}
		else
			itClub = m_clubSetMap.find(m_myInfo.userEquip.guidClubSet);

		if (itClub != m_clubSetMap.end())
		{
			m_userInfo[0].clubInfo.tid = (*itClub).second.tid;
			m_userInfo[0].clubInfo.guid = (*itClub).second.guid;
			memcpy(m_userInfo[0].clubInfo.PCL, (*itClub).second.Common,
				sizeof(m_userInfo[0].clubInfo.PCL));
		}
	}
}

sRivalData* CSharedDoc::GetRival(unsigned long uid)
{
	std::vector<sGuildMatchup>::iterator it;
	for (it = m_guildMatchupList.begin(); it != m_guildMatchupList.end(); ++it)
	{
		if (uid == it._Myptr->uid[0])
			return &m_rivalList[Doc()->GetIndex(it._Myptr->uid[1])];

		if (uid == it._Myptr->uid[1])
			return &m_rivalList[Doc()->GetIndex(it._Myptr->uid[0])];
	}

	return NULL;
}

int CSharedDoc::CollapseItemSlot(unsigned char player)
{
	if (player >= 4)
		return 0;

	unsigned long* slots = m_userInfo[player].userEquip.tidItemSlot;
	int numSlots = 10;
	if (IsLocalContent(S3_MASCOT))
		numSlots = GetNumAddItemSlotByMascot(player) + 8;

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(player);
		numSlots += CCardManager::Instance()->GetCardPeriodSlot();
	}

	int count = 0;
	for (int i = 0; i < numSlots; i++)
	{
		if (slots[i])
			count++;
	}

	for (int j = 0; j < count; j++)
	{
		if (slots[j] == 0)
		{
			int k;
			for (k = j + 1; k < numSlots; k++)
			{
				if (slots[k])
				{
					slots[j] = slots[k];
					slots[k] = 0;
					break;
				}
			}
			if (k == numSlots)
				break;
		}
	}

	return count;
}

const char* CSharedDoc::GetDefaultTitle(unsigned char gameType)
{
	const char* title;

	switch (gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
		title = defaultTitles_vs[rand() % 9];
		break;
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_USEMAX:
		title = defaultTitles_mass[rand() % 6];
		break;
	case GAME_TYPE_SKINS:
		title = defaultTitles_battle[rand() % 4];
		break;
	case GAME_TYPE_NEW_APPROACH:
		title = defaultTitles_approach[rand() % 3];
		break;
	case GAME_TYPE_AVATARCHAT:
	case GAME_TYPE_REALMYROOM:
		title = defaultTitles_chat[rand() % 7];
		break;
	default:
		title = "No Title!";
		break;
	}

	return title;
}

void CSharedDoc::Log30sScore()
{
	if (IsMassGame() || IsGuildGame())
	{
		mkdir("score");

		char filename[128];
		SYSTEMTIME st;
		GetLocalTime(&st);

		sprintf(filename, "score/score_30s_%d%02d%02d_%02d%02d%02d.txt",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		FILE* fp = fopen(filename, "wt");
		if (fp)
		{
			fprintf(fp, "## 30s Game Result ##\n\n");
			fprintf(fp, "* room name : %s\n", Doc()->m_roomInfo.title);
			fprintf(fp, "* map no. : %d\n",
				(unsigned char)(Doc()->m_golfGame.map > 127 &&
							Doc()->m_golfGame.map != 253
						? Doc()->m_golfGame.map - 128
						: Doc()->m_golfGame.map));
			fprintf(fp, "* grade : %d\n",
				(unsigned char)(Doc()->m_roomType >> 16));
			fprintf(fp, "* game finished at : %d-%02d-%02d %02d:%02d\n",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
			fprintf(fp, "* num of players : %d\n\n", m_rivalList.size());

			fprintf(fp,
				"nick / id                                rank  score  pang \n");
			fprintf(fp,
				"-----------------------------------------------------------\n");

			for (int i = 0; i < (int)m_rivalList.size(); i++)
			{
				sRivalData* pRival = &m_rivalList[i];

				if (pRival)
				{
					fprintf(fp, "%18s   %2d    ", pRival->nickname,
						pRival->rank);

					if (pRival->totalScore == 0)
					{
						fprintf(fp, "  0   %I64d\n", pRival->totalPang);
						continue;
					}

					fprintf(fp, "%+2d   %I64d\n", pRival->totalScore,
						pRival->totalPang);
				}
			}

			fclose(fp);
		}
	}
}

void CSharedDoc::LogGstUpdateHole(unsigned long uid, unsigned char hole,
	int score, __int64 pang)
{
}

void CSharedDoc::LoadSchoolName(char* const name, unsigned long id)
{
	char line[8192];
	char code[64];
	cFile* file = g_resrcmng->GetCFile("school.txt", 0xffff);

	if (!file)
	{
		name[0] = 0;
		return;
	}

	while (file->Scan("%n", line))
	{
		int length = strlen(line);
		cTokenV token;
		token.Init(line, length);

		token.GetToken(code, " ", 1);
		token.GetToken(name, "(\r\n", 3);

		if (id == atoi(code))
		{
			CloseCFile(file);
			return;
		}
	}

	CloseCFile(file);
	name[0] = 0;
}

bool CSharedDoc::InitRecentWhisperList(FrComboBox* combo)
{
	m_recentWhisperList.clear();
	return UpdateChatTarget(combo);
}

bool CSharedDoc::InitReservedChatList(FrComboBox* combo,
	eReservedChatPartner partner)
{
	if (!combo)
		return false;

	m_reservedChatList.clear();
	if (m_whisperPartner.size() > 0)
		combo->SetLine(1, m_whisperPartner.c_str(), 0, false, 0);

	if (partner & 1)
	{
		m_reservedChatList.push_back("\xc6\xc0\xbf\xa1\xb0\xd4");
	}
	else if (strcmpi(m_whisperPartner.c_str(), "\xc6\xc0\xbf\xa1\xb0\xd4") == 0)
	{
		m_whisperPartner = "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4";
		combo->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0, false, 0);
	}

	if (partner & 2)
	{
		m_reservedChatList.push_back("\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4");
	}

	if (Doc()->m_myInfo.info.dwGuildId)
	{
		m_reservedChatList.push_back("\xb1\xe6\xb5\xe5\xbf\xa1\xb0\xd4");
	}

	if (strlen(combo->GetLine(1, false)) == 0)
		combo->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0, false, 0);

	return UpdateChatTarget(combo);
}

bool CSharedDoc::AddWhisperPartner(FrComboBox* combo, const char* name)
{
	if (!combo)
		return false;

	bool bSet = false;
	const char* partner = NULL;

	if (name)
	{
		partner = name;
		bSet = true;
	}
	else if (strlen(combo->GetLine(1, false)) &&
		strcmp(MyNick(), combo->GetLine(1, false)))
	{
		partner = combo->GetLine(1, false);
	}

	if (partner)
	{
		std::list<std::string>::iterator it;

		it = std::find(m_reservedChatList.begin(), m_reservedChatList.end(),
			partner);
		if (it != m_reservedChatList.end())
		{
			if (!bSet)
				m_whisperPartner = partner;
			return true;
		}

		if (strcmpi("\xc6\xc0\xbf\xa1\xb0\xd4", partner) == 0)
		{
			return false;
		}

		it = std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(),
			partner);
		if (it != m_recentWhisperList.end())
		{
			if (*it == m_recentWhisperList.back())
				return true;

			if (!bSet)
				m_whisperPartner = partner;
			m_recentWhisperList.push_back(partner);
			m_recentWhisperList.erase(it);

			return UpdateChatTarget(combo);
		}

		if (m_recentWhisperList.size() == m_maxWhisperPartner)
		{
			if (!bSet)
				m_whisperPartner = partner;
			m_recentWhisperList.push_back(partner);
			m_recentWhisperList.erase(m_recentWhisperList.begin());
			return UpdateChatTarget(combo);
		}

		if (!bSet)
			m_whisperPartner = partner;
		m_recentWhisperList.push_back(partner);
		return UpdateChatTarget(combo);
	}

	return false;
}

bool CSharedDoc::UpdateChatTarget(FrComboBox* combo)
{
	if (!combo)
		return false;

	std::list<std::string>::iterator it;
	combo->ClearList();

	for (it = m_recentWhisperList.begin(); it != m_recentWhisperList.end();
		++it)
	{
		combo->AddString((*it).c_str());
	}
	for (it = m_reservedChatList.begin(); it != m_reservedChatList.end(); ++it)
	{
		combo->AddString((*it).c_str());
	}

	return true;
}

bool CSharedDoc::UTIL_SendChatMessage(FrComboBox* combo, const char* msg,
	bool bWhisper)
{
	if (!combo)
	{
		return false;
	}

	if (strlen(combo->GetLine(1, false)) == 0)
		combo->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0, false, 0);

	if (!AddWhisperPartner(combo, NULL))
		return false;

	const char* target = NULL;
	bool bTeam = false;

	if (strcmpi(combo->GetLine(1, false), "\xc6\xc0\xbf\xa1\xb0\xd4") == 0)
	{
		bTeam = true;
	}
	else if (strcmpi(combo->GetLine(1, false),
				 "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4") &&
		strcmpi(combo->GetLine(1, false), "\xc6\xc0\xbf\xa1\xb0\xd4"))
	{
		target = combo->GetLine(1, false);
	}

	if (bWhisper)
		m_chatManager.SendChatMessage2(msg, target, bTeam);
	else
		m_chatManager.SendChatMessage(msg, target, bTeam);

	return true;
}

const char* CSharedDoc::GetPrevChatTarget(const char* name)
{
	std::list<std::string>::iterator it =
		std::find(m_reservedChatList.begin(), m_reservedChatList.end(), name);
	if (it != m_reservedChatList.end())
	{
		if (it != m_reservedChatList.begin())
		{
			--it;
			return (*it).c_str();
		}
		if (m_recentWhisperList.size() > 0)
			return m_recentWhisperList.back().c_str();
	}
	else if (m_recentWhisperList.size() > 0)
	{
		it = std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(),
			name);
		if (it != m_recentWhisperList.end())
		{
			if (it != m_recentWhisperList.begin())
			{
				--it;
				return (*it).c_str();
			}
			return m_recentWhisperList.front().c_str();
		}
	}
	return m_reservedChatList.back().c_str();
}

const char* CSharedDoc::GetNextChatTarget(const char* name)
{
	std::list<std::string>::iterator it =
		std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(), name);
	if (it != m_recentWhisperList.end())
	{
		if (++it != m_recentWhisperList.end())
		{
			return (*it).c_str();
		}
		else if (m_reservedChatList.size() > 0)
		{
			return m_reservedChatList.front().c_str();
		}
	}
	else if (m_reservedChatList.size() > 0)
	{
		it = std::find(m_reservedChatList.begin(), m_reservedChatList.end(),
			name);
		if (it != m_reservedChatList.end())
		{
			if (++it != m_reservedChatList.end())
			{
				return (*it).c_str();
			}
		}
	}

	return "";
}

void CSharedDoc::CheckAngelWing()
{
	HaveAngelWing(false);

	std::list<sItemInfo>::iterator it;
	for (it = m_partsList.begin(); it != m_partsList.end(); it++)
	{
		if (IsAngelWing((*it).tid))
		{
			HaveAngelWing(true);
			return;
		}
	}
}

void CSharedDoc::SetInitHaveHalloweenItem()
{
	SetHaveHalloweenItem(false);

	std::list<sItemInfo>::iterator it;
	for (it = m_partsList.begin(); it != m_partsList.end(); it++)
	{
		if (((*it).tid & 0x1800) == 0x1800)
		{
			SetHaveHalloweenItem(true);
			return;
		}
	}
}

void CSharedDoc::SetInitWearHalloweenItem()
{
	SetWearHalloweenItem(false);

	std::map<unsigned int, sCharacterInfo>::iterator it;
	for (it = m_charMap.begin(); it != m_charMap.end(); ++it)
	{
		sCharacterInfo* pInfo = &(*it).second;

		if (pInfo && pInfo->guid == m_myInfo.userEquip.guidChar)
		{
			for (int i = 0; i < 24; i++)
			{
				if ((pInfo->tidParts[i] & 0x1800) == 0x1800)
				{
					SetWearHalloweenItem(true);
					return;
				}
			}
			return;
		}
	}
}

int CSharedDoc::IsControlServerService(int service)
{
	return m_controlServerService & service;
}

void CSharedDoc::InitTradeData()
{
	m_stateTrade = STATE_TRADE_INIT;
	m_listTradeItem.clear();

	m_tradeUID = 0xffffffff;
	m_tradeTitle = "";
	m_countVisitor = 0;
	m_tradeIncome = 0;

	m_tradeMode = 0;
}

void CSharedDoc::SetTradeUID(unsigned long uid)
{
	if (uid != 0xffffffff)
		m_tradeUID = uid;
}

bool CSharedDoc::AddListTradeItem(sTradeItem item)
{
	if (m_listTradeItem.size() >= 6)
		return false;
	if (item.dwGuid == 0)
		return false;
	if (item.dwTid == 0)
		return false;
	if (item.iNum <= 0)
		return false;
	if (item.i64Price <= 0)
		return false;

	std::list<sTradeItem>::iterator it;
	for (it = m_listTradeItem.begin(); it != m_listTradeItem.end(); it++)
	{
		if ((*it).dwGuid == item.dwGuid)
		{
			unsigned long type = item.dwTid >> 26;
			if (type < 5 || (type > 6 && type != 31))
				return false;
		}
	}

	m_listTradeItem.push_back(item);
	if (IsLocalContent(S4_UCC))
	{
		if (strlen(item.UccIndex) > 0 && item.Seq > 0 && (item.status & 1) &&
			IsUccClothes(item.dwTid))
		{
			sUccClothes clothes;
			clothes.id = item.dwGuid;
			clothes.typeId = item.dwTid;
			strncpy(clothes.uccIndex, item.UccIndex, 9);
			strncpy(clothes.uccName,
				"\xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 "
				"\xc1\xdf...",
				41);
			clothes.downloadCount = 0;
			UccManager()->AddClothes(0, clothes, true, false);
		}
	}

	return true;
}

bool CSharedDoc::MinusListTradeItem(sTradeItem item)
{
	std::list<sTradeItem>::iterator it;
	for (it = m_listTradeItem.begin(); it != m_listTradeItem.end(); it++)
	{
		if ((*it).iIndex == item.iIndex)
		{
			if ((*it).dwGuid == item.dwGuid && (*it).dwTid == item.dwTid)
			{
				if ((*it).iNum > item.iNum)
				{
					(*it).iNum -= item.iNum;
					return true;
				}
				else if ((*it).iNum == item.iNum)
				{
					m_listTradeItem.erase(it);
					return true;
				}
			}
			break;
		}
	}

	return false;
}

void CSharedDoc::ChageListStallitem(sStall* stall)
{
	std::list<sStall>::iterator it;
	std::list<sStall>* pList = Doc()->GetListStallItem();
	if (stall == NULL)
	{
		sStall* pStall = GetStallItem();
		for (it = pList->begin(); it != pList->end(); it++)
		{
			if (pStall->stallKey == (*it).stallKey)
				*it = *pStall;
		}
	}
	else
	{
		for (it = pList->begin(); it != pList->end(); it++)
		{
			if (stall->stallKey == (*it).stallKey)
				*it = *stall;
		}
	}
}

bool CSharedDoc::SendOfflineTradeEditShop()
{
	if (!IsLocalContent(S4_OFFLINE_SHOP))
		return true;

	if (Doc()->GetStallItem() == NULL || Doc()->GetStallItem()->stallKey == 0)
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc1\xf8\xbf\xad\xc0\xe5 \xb1\xb8\xb8\xc5\xb8\xa6 "
				 "\xc7\xcf\xc1\xf6\xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf"
				 "\xb4\xd9. \xb1\xb8\xb8\xc5\xc7\xcf\xb0\xed \xc0\xcc\xbf\xeb"
				 "\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.~",
			0, 0, 0, 0));
		return true;
	}

	Doc()->SetTradeMode(5);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 562, 0, 0, 0, 0, 0));

	return true;
}

bool CSharedDoc::SendOfflineTradeVisitor(unsigned long uid)
{
	if (!IsLocalContent(S4_OFFLINE_SHOP))
		return true;

	if (Doc()->GetStallItem() == NULL || Doc()->GetStallItem()->stallKey == 0)
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xb9\xe6\xb9\xae\xc7\xcf\xbd\xc5 \xb9\xe6\xc0\xc7 \xc0\xaf\xc0\xfa"
				 "\xb4\xc2 \xc1\xc2\xc6\xc7\xc0\xbb \xb9\xe8\xc4\xa1\xc7\xcf\xb0\xc5"
				 "\xb3\xaa, \xb1\xb8\xc0\xd4\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc"
				 "\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		return true;
	}

	std::list<sStall>* pList = Doc()->GetListStallItem();
	sStall stall;
	std::list<sStall>::iterator it;
	for (it = pList->begin(); it != pList->end(); ++it)
	{
		if ((*it).typeID == uid)
		{
			WSendPacket packet((enumClientPacket)0xcd);
			packet.Encode4(Doc()->m_replayTargetUID);
			packet.Encode4((*it).stallKey);
			packet.Encode1(1);
			packet.Send(TO_GAME);

			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			break;
		}
	}

	return true;
}

bool CSharedDoc::SendTradeOpenShop()
{
	if (GetStateTrade() != STATE_TRADE_EDIT)
		return false;
	if (GetListTradeItem()->size() <= 0)
		return false;

	WSendPacket packet((enumClientPacket)0x74);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeCloseShop()
{
	if (GetStateTrade() == STATE_TRADE_INIT)
		return false;

	WSendPacket packet((enumClientPacket)0x75);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeEditShop()
{
	WSendPacket packet((enumClientPacket)0x76);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeEnterShop(unsigned long uid)
{
	if (uid == 0xffffffff)
		return false;

	WSendPacket packet((enumClientPacket)0x77);
	packet.Encode4(uid);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeExitShop(unsigned long uid)
{
	if (uid == 0xffffffff)
		return false;

	WSendPacket packet((enumClientPacket)0x78);
	packet.Encode4(uid);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeTitle(std::string title)
{
	if (title.length() <= 0)
		return false;

	WSendPacket packet((enumClientPacket)0x79);
	packet.EncodeStr(title);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeShowVisitor()
{
	switch (m_stateTrade)
	{
	case STATE_TRADE_OPEN:
	case STATE_TRADE_OUTOFSTOCK:
		break;
	default:
		return false;
	}

	WSendPacket packet((enumClientPacket)0x7a);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeIncome()
{
	switch (m_stateTrade)
	{
	case STATE_TRADE_OPEN:
	case STATE_TRADE_OUTOFSTOCK:
		break;
	default:
		return false;
	}

	WSendPacket packet((enumClientPacket)0x7b);
	packet.Send(TO_GAME);

	return true;
}

bool CSharedDoc::SendTradeBuyItem(unsigned long uid, sTradeItem item)
{
	if (m_stateTrade != STATE_TRADE_ENTER)
		return false;
	if (uid == 0 || uid == 0xffffffff)
		return false;
	if (item.dwGuid == 0)
		return false;
	if (item.dwTid == 0)
		return false;
	if (item.i64Price == 0)
		return false;
	if (item.iNum <= 0)
		return false;

	WSendPacket packet((enumClientPacket)0x7d);
	packet.Encode4(uid);
	packet.EncodeBuffer(&item, sizeof(sTradeItem));
	packet.Send(TO_GAME);

	return true;
}

void CSharedDoc::SetInitAbilityItem()
{
	m_flagAbilityItem = 0;
	m_numBonusPangItem = 0;

	for (int i = 0; i < 24; i++)
	{
		unsigned long flag = ItemManager()->GetIndexAbilityItem(
			m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tidParts[i]);
		m_flagAbilityItem |= flag;

		if (flag & 1)
		{
			m_numBonusPangItem++;
		}
	}
}

int CSharedDoc::GetIndexPangyaLogoImage()
{
	int index = 0;
	switch (m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid & 0x3ffffff)
	{
	case 0x0c:
		index = 1;
		break;
	case 0x80:
		index = 2;
		break;
	case 0x11:
		index = 3;
		break;
	case 0x10:
		index = 4;
		break;
	case 0x14:
		index = 6;
		break;

	case 0x13:
		index = 8;
		break;

	case 0x18:
		index = 11;
		break;
	case 0x19:
		index = 13;
		break;
	case 0x1b:
		index = 16;
		break;

	case 0x27:
		index = 19;
		break;
	case 0x2b:
		index = 20;
		break;
	case 0x22:
	case 0x2f:
	case 0x30:
		index = 21;
		break;
	case 0x26:
		index = 23;
		break;

	case 0x2c:
		index = 26;
		break;

	case 0x33:
		index = 27;
		break;
	case 0x35:
		index = 28;
		break;
	case 0x34:
		index = 29;
		break;
	case 0x3a:
		index = 31;
		break;

	case 0x3f:
	case 0x40:
		if ((m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid & 0x3ffffff) ==
			9)
			index = 58;
		break;

	case 0x3e:
		index = 62;
		break;
	case 0x43:
		index = 67;
		break;
	case 0x44:
	case 0x45:
		index = 68;
		break;
	}

	return index;
}

bool CSharedDoc::IsOverlapPart(unsigned long typeId)
{
	std::list<sItemInfo>::iterator it;

	for (it = m_partsList.begin(); it != m_partsList.end(); it++)
	{
		if ((*it).tid == typeId)
		{
			return true;
		}
	}

	return false;
}

int OnlinePlay()
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		return FALSE;

	if (IS_KINDOF(CLobbyTask, AfxGetTask()) ||
		IS_KINDOF(CShopTask, AfxGetTask()))
		return TRUE;

	if (Doc()->m_gameMode != 0 || Doc()->m_golfGame.gameType == 11 ||
		Doc()->m_golfGame.gameType == 12)
		return FALSE;

	return TRUE;
}

unsigned char GetCurMap()
{
	return (Doc()->m_golfGame.map > 127 && Doc()->m_golfGame.map != 0xfd)
		? Doc()->m_golfGame.map - 128
		: Doc()->m_golfGame.map;
}

bool IsCurMapSnow()
{
	unsigned char map = GetCurMap();
	if (map == 8)
		return true;
	if (map == 14)
		return true;
	if (map == 17)
		return true;
	return false;
}

bool SetCurMap(unsigned char map)
{
	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
		((map > 127 && map != 0xfd) ? map - 128 : map) | 0x28000000);
	if (pCourse == NULL)
	{
		return false;
	}

	Doc()->m_golfGame.map = map;
	Doc()->m_golfGame.pCourse = pCourse;
	return true;
}

std::string CSharedDoc::GetLoginAuthKey()
{
	return Doc()->m_loginAuthKey;
}

unsigned short CSharedDoc::GetSecurityKey()
{
	return Doc()->m_securityKey;
}

void CSharedDoc::SetLoginAuthKey(std::string key)
{
	Doc()->m_loginAuthKey = key;
}

void CSharedDoc::SetSecurityKey(unsigned short key)
{
	Doc()->m_securityKey = key;
}

void CSharedDoc::RefreshItemListFromGuidList(unsigned char mode)
{
	std::list<sItemInfo>::iterator it;
	std::list<unsigned long>::iterator itGuid;
	std::list<unsigned short>::iterator itCount;

	it = m_myItemList.begin();
	while (it != m_myItemList.end())
	{
		itGuid = m_refreshGuidList.begin();
		itCount = m_refreshCountList.begin();
		for (; itGuid != m_refreshGuidList.end(); ++itGuid, ++itCount)
		{
			if ((*it).guid == *itGuid)
				break;
		}

		if (itGuid == m_refreshGuidList.end())
		{
			it = m_myItemList.erase(it);
			continue;
		}

		if (mode == 2 && (*it).Common[0] != *itCount)
		{
			(*it).Common[0] = *itCount;
		}

		it++;
	}

	if (mode == 2)
	{
		it = m_questDropList.begin();
		while (it != m_questDropList.end())
		{
			itGuid = m_refreshGuidList.begin();
			itCount = m_refreshCountList.begin();
			for (; itGuid != m_refreshGuidList.end(); ++itGuid, ++itCount)
			{
				if ((*it).guid == *itGuid)
					break;
			}

			if (itGuid == m_refreshGuidList.end())
			{
				it = m_questDropList.erase(it);
				continue;
			}

			if ((*it).Common[0] != *itCount)
			{
				(*it).Common[0] = *itCount;
			}

			it++;
		}
	}

	m_refreshGuidList.clear();
	m_refreshCountList.clear();
}

void CSharedDoc::UpdateGachaTickets()
{
	unsigned long typeId[2];
	unsigned short count[2];
	sItemInfo item;
	std::list<sItemInfo>::iterator it;

	typeId[0] = 0x1a000080;
	typeId[1] = 0x1a000083;
	count[0] = m_gachaTicket[0];
	count[1] = m_gachaTicket[1];

	memset(&item, 0, sizeof(sItemInfo));

	for (int i = 0; i < 2; i++)
	{
		for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();)
		{
			if ((*it).tid == typeId[i])
			{
				if (count[i] == 0)
					Doc()->m_myItemList.erase(it);
				else
					(*it).Common[0] = count[i];
				break;
			}

			it++;
		}

		if (count[i] != 0)
		{
			if (it == Doc()->m_myItemList.end())
			{
				item.tid = typeId[i];
				item.Common[0] = count[i];
				Doc()->m_myItemList.push_back(item);
			}
		}
	}
}

unsigned long CSharedDoc::GetMyDisconPangPenalty()
{
	unsigned long penalty = 0;

	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_STROKE:
		penalty = 100;
		break;

	case GAME_TYPE_MATCH:
	case GAME_TYPE_SKINS:
		penalty = 350;
		break;

	case GAME_TYPE_TEAM:
		penalty = 300;
		break;

	case GAME_TYPE_30S:
		penalty = 150;
		break;

	case GAME_TYPE_30S_TEAM:
		penalty = 200;
		break;
	}

	if (m_myInfo.stat.Level >= 6)
	{
		penalty += ((m_myInfo.stat.Level - 1) / 5) * 100;

		if (m_myInfo.stat.dwGameCount > 50)
			penalty += ((m_myInfo.stat.dwNoMannerGameCount * 100 /
							m_myInfo.stat.dwGameCount) -
						   10) *
				10;
	}

	return penalty;
}

void CSharedDoc::LoadVisGroup(const char* filename)
{
	cFile* fp = g_resrcmng->GetCFile("visgroup.def", 0xffff);

	if (fp)
	{
		std::vector<std::string> tokens;

		int len = fp->Length();
		char* buf = new char[len + 1];
		fp->Read(buf, len);
		CloseCFile(fp);

		buf[len] = 0;
		char* p = buf;

		for (;;)
		{
			while (*p == '\n' || *p == '\r')
				p++;

			if (*p == 0)
				break;

			char line[256];
			char* q = line;
			while (*p != '\n' && *p != '\r' && *p != 0)
			{
				*q = *p++;
				q++;
			}

			*q = 0;

			LineParse(line, tokens);

			if (tokens.size() > 1)
			{
				std::string& key = tokens[0];

				for (unsigned int i = 1; i < tokens.size(); i++)
					m_visGroupMap[key].push_back(tokens[i]);
			}
		}

		delete[] buf;
	}
}

void CSharedDoc::SetFileName(char* const filename)
{
	strcpy(m_fileName, filename);
}

void CSharedDoc::ReplaySavFileCheck()
{
	Doc()->m_recordedItemList.clear();

	char fileName[32] = { 0 };

	for (int i = 0; i < 100; i++)
	{
		sprintf(fileName, "save/pangya_%03d.sav", i);
		FILE* fp = fopen(fileName, "rb");

		if (fp == NULL)
			continue;

		unsigned long version = 0;
		unsigned long uid = 0xffffffff;
		unsigned char course = 0;
		SYSTEMTIME date;
		int mode = 0;

		fread(&version, 4, 1, fp);
		fread(&uid, 4, 1, fp);
		fread(&course, 1, 1, fp);
		fread(&date, sizeof(SYSTEMTIME), 1, fp);
		fread(&mode, 4, 1, fp);

		if (version == m_replayVersion && uid == MyUID())
		{
			sRecordedItemInfo info;

			if (mode == 1)
				info.itemInfo.tid = 0x1a000043;
			else if (mode == 2)
				info.itemInfo.tid = 0x1a000050;

			info.itemInfo.Common[0] = 1;
			info.course = course;
			info.date = date;
			info.bValid = 1;
			strcpy(info.fileName, fileName);

			Doc()->m_recordedItemList.push_back(info);
		}

		fclose(fp);

		if (Doc()->m_recordedItemList.size() >= 10)
			break;
	}
}

std::list<std::string>* CSharedDoc::GetVisGroup(const char* name)
{
	std::map<std::string, std::list<std::string> >::iterator it =
		m_visGroupMap.find(name);

	if (it == m_visGroupMap.end())
		return NULL;

	return &it->second;
}

bool CSharedDoc::CheckUnderLevelClubSet(unsigned long typeId)
{
	bool bRet = true;
	IFF_STRUCT::sClubSet* pClubSet = Doc()->m_itemManager.FindClubSet(typeId);

	if (pClubSet->c.IsUnderLvl)
	{
		if (pClubSet->c.Level < m_myInfo.stat.Level)
			bRet = false;
	}

	return bRet;
}

void CSharedDoc::InitMapEvent()
{
}

bool CSharedDoc::IsMapEventActive(int course)
{
	return false;
}

unsigned long CSharedDoc::GetMapEventPangRate(int course)
{
	return 100;
}

unsigned long CSharedDoc::GetMapEventExpRate(int course)
{
	return 100;
}

void CSharedDoc::SetIdentity(int identity)
{
	m_myInfo.info.dwIdentity = identity;
}

void CSharedDoc::AccumulateIdentity(int identity)
{
	m_myInfo.info.dwIdentity |= identity;
}

__int64 CSharedDoc::RemainOwnCash(unsigned long provider, __int64 cookie)
{
	__int64 remain = Doc()->m_cookie;

	switch (provider)
	{
	case 2:
	case 4:
		remain -= cookie;
		break;
	}

	return remain;
}

void CSharedDoc::RefreshAllCookie()
{
	Doc()->m_cookie =
		S5::ExchangeOwnCashToCookie(Doc()->m_cash, Doc()->m_bonusCash, 100);
}

bool CSharedDoc::CanUseRookieChannelMap(unsigned int course)
{
	bool bRet = true;

	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
		(course > 127 && course != 0xfd ? course - 128 : course) | 0x28000000);
	if (pCourse)
	{
		if (pCourse->Difficulty > 3)
			return false;
	}

	if (course == 8 || course == 1 || course == 9 || course == 14 ||
		course == 6)
		bRet = false;

	return bRet;
}

void CSharedDoc::SetServerTime(_SYSTEMTIME& time)
{
	Doc()->m_serverTime = time;

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		if (CCardManager::IsInstantiated())
			CCardManager::Instance()->ResetSpecialBuffTick();
	}

	Doc()->m_serverTimeTick = GetTickCount();
}

SYSTEMTIME& CSharedDoc::GetServerTime()
{
	return Doc()->m_serverTime;
}

void CSharedDoc::SetDirectMoveRoomInfo(unsigned char type, unsigned short room,
	unsigned long uid)
{
	m_directMoveType = type;
	m_directMoveRoom = room;
	m_directMoveUID = uid;
}

bool CSharedDoc::GetDirectMoveRoomInfo(unsigned char* type,
	unsigned short* room, unsigned long* uid)
{
	if (m_directMoveUID == 0xffffffff || m_directMoveRoom == 0xffff)
		return false;

	*type = m_directMoveType;
	*room = m_directMoveRoom;
	*uid = m_directMoveUID;

	return true;
}

void CSharedDoc::ClearDirectMoveRoomInfo()
{
	m_directMoveType = 0xff;
	m_directMoveRoom = 0xffff;
	m_directMoveUID = 0xffffffff;
}

int CSharedDoc::FindUserInfoTimeUID(unsigned long uid,
	std::map<unsigned long, sUserInfoTime>::iterator& it)
{
	it = m_userInfoTimeMap.find(uid);
	return it != m_userInfoTimeMap.end() ? TRUE : FALSE;
}

int CSharedDoc::FindUserInfoTimeGUID(unsigned long guid,
	std::map<unsigned long, sUserInfoTime>::iterator& it)
{
	if (!m_userInfoTimeMap.empty())
	{
		for (std::map<unsigned long, sUserInfoTime>::iterator i =
				 m_userInfoTimeMap.begin();
			i != m_userInfoTimeMap.end(); ++i)
		{
			if ((*i).second.info.info.dwGuid == guid)
			{
				it = i;
				return TRUE;
			}
		}
	}

	it = m_userInfoTimeMap.end();
	return FALSE;
}

int CSharedDoc::FindUserInfo(unsigned long uid,
	std::map<unsigned long, sBriefUserInfo>::iterator& it)
{
	if (!m_briefUserInfoMap.empty())
	{
		std::map<unsigned long, sBriefUserInfo>::iterator i =
			m_briefUserInfoMap.find(uid);
		if (i != m_briefUserInfoMap.end())
		{
			it = i;
			return TRUE;
		}
	}

	return FALSE;
}

int CSharedDoc::FindUserInfoUID(unsigned long uid,
	std::map<unsigned long, sBriefUserInfo>::iterator& it)
{
	if (m_briefUserInfoMap.empty())
		return FALSE;

	for (std::map<unsigned long, sBriefUserInfo>::iterator i =
			 m_briefUserInfoMap.begin();
		i != m_briefUserInfoMap.end(); ++i)
	{
		if (uid == (*i).second.dwUid)
		{
			it = i;
			return TRUE;
		}
	}

	return FALSE;
}

int CSharedDoc::FindUserInfoTimeUserID(const char* id,
	std::map<unsigned long, sUserInfoTime>::iterator& it)
{
	if (!m_userInfoTimeMap.empty())
	{
		char* lwrId = strlwr((char*)id);

		for (std::map<unsigned long, sUserInfoTime>::iterator i =
				 m_userInfoTimeMap.begin();
			i != m_userInfoTimeMap.end(); ++i)
		{
			char* lwr = strlwr((*i).second.info.info.sID);
			if (strcmp(lwrId, lwr) == 0)
			{
				it = i;
				return TRUE;
			}
		}
	}

	it = m_userInfoTimeMap.end();
	return FALSE;
}

int CSharedDoc::InsertUserInfoTime(unsigned long uid, sUserInfoTime& info)
{
	m_userInfoTimeMap[uid] = info;
	return TRUE;
}

CPeriodContents* CSharedDoc::GetPeriodDoc()
{
	return m_pPeriodDoc;
}

void CSharedDoc::SetPeriodDoc()
{
	m_pPeriodDoc = NULL;
	CContentsDoc::Instance()->GetContainer((localContentType_t)0x92,
		m_pPeriodDoc);
}

const eApproachMissionType s_blindMissions[2] = {
	(eApproachMissionType)4,
	(eApproachMissionType)21,
};

bool CSharedDoc::IsCurrentMissionBlind()
{
	bool bRet = false;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
	{
		for (int i = 0;
			i < sizeof(s_blindMissions) / sizeof(s_blindMissions[0]); i++)
		{
			if (s_blindMissions[i] == GOLFDOC()->GetApproachMissionType())
			{
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

int CSharedDoc::IsEquipParts(unsigned long uid, unsigned long typeId)
{
	std::map<unsigned int, sCharacterInfo>::iterator it;

	if (typeId >> 26 == 2)
	{
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			for (int i = 0; i < 24; i++)
			{
				if ((*it).second.tidParts[i] &&
					uid == (*it).second.ItemIdList[i])
					return TRUE;
			}
		}
	}
	else if (typeId >> 26 == 28)
	{
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			for (int i = 0; i < 5; i++)
			{
				if ((*it).second.tidAuxParts[i] &&
					typeId == (*it).second.tidAuxParts[i])
					return TRUE;
			}
		}
	}

	return FALSE;
}

bool CSharedDoc::IsEquipParts(sCharacterInfo& info, unsigned long uid,
	unsigned long typeId)
{
	if (typeId >> 26 == 2)
	{
		for (int i = 0; i < 24; i++)
		{
			if (info.tidParts[i])
			{
				if (uid == info.ItemIdList[i])
					return true;
			}
		}
	}
	else if (typeId >> 26 == 28)
	{
		for (int i = 0; i < 5; i++)
		{
			if (info.tidAuxParts[i])
			{
				if (typeId == info.tidAuxParts[i])
					return true;
			}
		}
	}

	return false;
}

std::map<unsigned char, sMapStatistics>& CSharedDoc::GetMyPastMapStat(
	unsigned char season)
{
	unsigned char index = 0;

	switch (season)
	{
	case 0:
		index = 0;
		break;
	case 10:
		index = 1;
		break;
	case 51:
		index = 2;
		break;
	case 5:
		index = 3;
		break;
	}

	return m_pastMapStat[index];
}

float CalcPowerBarPosition()
{
	WVector cup = GetHoleCupPos();
	WVector diff = GolfBall().m_pos - cup;
	float range = GolfClub().GetRange() * 3.2f;

	diff.y = 0.0f;
	if (diff.Magnitude() > range)
		return s_barEnd;

	char buf[32] = { 0 };
	sPlayerData* pPlayer = PLAYER(GOLFDOC()->m_currentPlayer);
	cup = GOLFDOC()->GetHoleData().pin;
	WVector dist2 = cup - pPlayer->pos;
	dist2.y = 0.0f;
	float dist = sqrtf(dist2 * dist2) * 0.3125f;

	if (dist < 100.0f)
		sprintf(buf, "%.1f", dist);
	else
		sprintf(buf, "%.0f", dist);

	float value = (float)atof(buf);
	value *= 3.2f;

	return (value / range) * s_powerBarWidth + s_barStart;
}

void PerformPowerBarSetting()
{
	// HACK: these seem to be DCE'd dependencies in the original
	if (0)
	{
		WFlags flag;
		flag.Set(0);
		flag.Reset();
	}

	float pos = CalcPowerBarPosition();

	CTask* pTask = AfxGetTask();
	IActor* pScreen = pTask->GetActor("Screen");
	if (pScreen)
		pScreen << MsgObject(NULL, 0xa1, (int)&pos, 0, 0, 0, 0);
}

ILFILLTU_SHAREDDOC
