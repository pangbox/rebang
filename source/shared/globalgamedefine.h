#pragma once

enum eGameType
{
	GAME_TYPE_STROKE,
	GAME_TYPE_TEAM,
	GAME_TYPE_AVATARCHAT,
	GAME_TYPE_MATCH,
	GAME_TYPE_30S,
	GAME_TYPE_30S_TEAM,
	GAME_TYPE_GUILD_MATCH,
	GAME_TYPE_SKINS,
	GAME_TYPE_REALMYROOM,
	GAME_TYPE_APPROACH,
	GAME_TYPE_NEW_APPROACH,
	GAME_TYPE_TUTORIAL_BASIC,
	GAME_TYPE_TUTORIAL_ADV,
	GAME_TYPE_OFFLINE_GHOST,
	GAME_TYPE_USEMAX,
	GAME_TYPE_MAX
};

enum eGameStyle
{
	GAME_STYLE_NORMAL,
	GAME_STYLE_SPECIAL1,
	GAME_STYLE_SPECIAL2
};

enum eLevel
{
	ROOKIE_F,
	ROOKIE_E,
	ROOKIE_D,
	ROOKIE_C,
	ROOKIE_B,
	ROOKIE_A,
	BEGINNER_E,
	BEGINNER_D,
	BEGINNER_C,
	BEGINNER_B,
	BEGINNER_A,
	JUNIOR_E,
	JUNIOR_D,
	JUNIOR_C,
	JUNIOR_B,
	JUNIOR_A,
	SENIOR_E,
	SENIOR_D,
	SENIOR_C,
	SENIOR_B,
	SENIOR_A,
	AMATUER_E,
	AMATUER_D,
	AMATUER_C,
	AMATUER_B,
	AMATUER_A,
	SEMI_PRO_E,
	SEMI_PRO_D,
	SEMI_PRO_C,
	SEMI_PRO_B,
	SEMI_PRO_A,
	PRO_E,
	PRO_D,
	PRO_C,
	PRO_B,
	PRO_A,
	NATIONAL_PRO_E,
	NATIONAL_PRO_D,
	NATIONAL_PRO_C,
	NATIONAL_PRO_B,
	NATIONAL_PRO_A,
	WORLD_PRO_E,
	WORLD_PRO_D,
	WORLD_PRO_C,
	WORLD_PRO_B,
	WORLD_PRO_A,
	MASTER_E,
	MASTER_D,
	MASTER_C,
	MASTER_B,
	MASTER_A,
	TOP_MASTER_E,
	TOP_MASTER_D,
	TOP_MASTER_C,
	TOP_MASTER_B,
	TOP_MASTER_A,
	WORLD_MASTER_E,
	WORLD_MASTER_D,
	WORLD_MASTER_C,
	WORLD_MASTER_B,
	WORLD_MASTER_A,
	LEGEND_E,
	LEGEND_D,
	LEGEND_C,
	LEGEND_B,
	LEGEND_A,
	INFINITY_LEGEND_E,
	INFINITY_LEGEND_D,
	INFINITY_LEGEND_C,
	INFINITY_LEGEND_B,
	INFINITY_LEGEND_A,
	MAX_LEVELS,
};

enum eTitle
{
	TITLE_LEVEL,
	TITLE_JJANG,
	TITLE_OLD_JJANG,
	TITLE_CYBER_CHAMPIONSHIP,
	TITLE_WCG_CHAMPIONSHIP,
	TITLE_PCBANG_CHAMPIONSHIP,
	TITLE_SQUAD,
	TITLE_OFFICIAL,
	TITLE_CHICK = 15,
	TITLE_PANGYASHOT = 21,
	TITLE_STRAIGHTROAD,
	TITLE_GREENMASTER,
	TITLE_COURSEMASTER,
	TITLE_GOLD,
	TITLE_SILVER,
	TITLE_BRONZE,
	TITLE_MANNERANGEL
};

enum GUILD_CLASS_IDX
{
};

enum GUILD_STATE_IDX
{
};

#pragma pack(push, 1)
struct GUILD_HISTORY
{
	int index;
	unsigned long guildUID;
	char guildName[21];
	GUILD_STATE_IDX state;
	_SYSTEMTIME time;

	GUILD_HISTORY()
		: index(-1), guildUID(0), state((GUILD_STATE_IDX)0)
	{
		memset(guildName, 0, sizeof(guildName));
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GUILD_USER_INFO
{
	unsigned long guildUID;
	char guildName[21];
	int guildPang;
	int guildPoint;
	int memberCount;
	char guildMark[12];
	char notice[101];
	char introduce[101];
	GUILD_CLASS_IDX classIdx;
	unsigned long masterUID;
	char masterNickname[22];

	GUILD_USER_INFO()
		: guildUID(0),
		  guildPang(0),
		  guildPoint(0),
		  memberCount(0),
		  classIdx((GUILD_CLASS_IDX)0),
		  masterUID(-1)
	{
		memset(guildName, 0, sizeof(guildName));
		memset(guildMark, 0, sizeof(guildMark));
		memset(notice, 0, sizeof(notice));
		memset(introduce, 0, sizeof(introduce));
		memset(masterNickname, 0, sizeof(masterNickname));
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GUILD_INFO : public GUILD_USER_INFO
{
	_SYSTEMTIME createTime;

	GUILD_INFO() { }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GUILD_LIST
{
	unsigned long guildUID;
	char guildName[21];
	int guildPang;
	int guildPoint;
	int memberCount;
	_SYSTEMTIME createTime;
	char introduce[101];
	unsigned long unknown9a;
	unsigned long masterUID;
	char masterNickname[22];
	char guildMark[12];

	GUILD_LIST()
		: guildUID(0),
		  guildPang(0),
		  guildPoint(0),
		  memberCount(0),
		  unknown9a(0),
		  masterUID(-1)
	{
		memset(guildName, 0, sizeof(guildName));
		memset(guildMark, 0, sizeof(guildMark));
		memset(introduce, 0, sizeof(introduce));
		memset(masterNickname, 0, sizeof(masterNickname));
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GUILD_USER_LIST
{
	unsigned long guildUID;
	unsigned long userUID;
	GUILD_CLASS_IDX classIdx;
	char message[25];
	char guildName[21];
	char nickname[22];
	unsigned char online;

	GUILD_USER_LIST()
		: guildUID(0), userUID(-1), classIdx((GUILD_CLASS_IDX)0), online(0)
	{
		memset(nickname, 0, sizeof(nickname));
		memset(guildName, 0, sizeof(guildName));
		memset(message, 0, sizeof(message));
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GUILD_USER_MSN_LIST
{
	unsigned long userUID;
	unsigned long guildUID;
	unsigned char sex;
	char userID[22];
	char nickname[22];
	unsigned char unknown35;
	unsigned char online;

	GUILD_USER_MSN_LIST()
		: userUID(-1), guildUID(0), sex(0), unknown35(0), online(0)
	{
		memset(nickname, 0, sizeof(nickname));
		memset(userID, 0, sizeof(userID));
	}
};
#pragma pack(pop)

struct Delete_Object
{
	template <typename T>
	void operator()(T ptr) const
	{
		delete ptr;
		ptr = NULL;
	}
};

struct Delete_SecondObject
{
	template <typename T>
	void operator()(T& pair) const
	{
		delete pair.second;
		pair.second = NULL;
	}
};

namespace GlobalEnum
{
#pragma pack(push, 1)
	struct sMissionInfo
	{
		int condition;
		bool complete;

		sMissionInfo()
			: condition(0), complete(false)
		{
		}
	};
#pragma pack(pop)
}

namespace GlobalEnum
{
	struct sBingoInfo
	{
		unsigned long type;
		int tryCount;
		unsigned long aimedPoint;
		unsigned long completeLine;
	};
}

#pragma pack(push, 1)
struct sTreasureHunt
{
	unsigned long dwUID;
	unsigned long dwTid;
	unsigned short wCount;
	unsigned char byItemType;

	sTreasureHunt()
		: dwUID(0), dwTid(0), wCount(0), byItemType(0)
	{
	}

	sTreasureHunt(const sTreasureHunt& rhs)
	{
		dwUID = rhs.dwUID;
		dwTid = rhs.dwTid;
		wCount = rhs.wCount;
		byItemType = rhs.byItemType;
	}

	void operator=(const sTreasureHunt& rhs)
	{
		dwUID = rhs.dwUID;
		dwTid = rhs.dwTid;
		wCount = rhs.wCount;
		byItemType = rhs.byItemType;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sTreasureGift
{
	unsigned long dwUID;
	unsigned long dwTid;
	unsigned long dwItemGuid;
	unsigned short wCount;
	unsigned char byItemType;
	int iHourRemain;
	unsigned short reserved;

	sTreasureGift()
		: dwUID(0),
		  dwTid(0),
		  dwItemGuid(0),
		  wCount(0),
		  byItemType(0),
		  iHourRemain(0),
		  reserved(0)
	{
	}

	void operator=(const sTreasureGift& rhs)
	{
		dwUID = rhs.dwUID;
		dwTid = rhs.dwTid;
		dwItemGuid = rhs.dwItemGuid;
		wCount = rhs.wCount;
		byItemType = rhs.byItemType;
		iHourRemain = rhs.iHourRemain;
		reserved = rhs.reserved;
	}

	void Reset()
	{
		dwUID = 0;
		dwTid = 0;
		dwItemGuid = 0;
		wCount = 0;
		byItemType = 0;
		iHourRemain = 0;
		reserved = 0;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sTreasureItem
{
	unsigned char byMapIndex;
	unsigned long dwGauge;

	sTreasureItem()
		: byMapIndex(0), dwGauge(0)
	{
	}
};

struct stGameClass
{
	eGameStyle style;
	unsigned char gameType;

	stGameClass(eGameStyle s, unsigned char type)
	{
		style = s;
		gameType = type;
	}
};

#pragma pack(pop)
static stGameClass s_gameClass((eGameStyle)255, 255);
enum eMapType
{
	MAP_BLUE_LAGOON = 0,
	MAP_BLUE_WATER = 1,
	MAP_SEPIA_WIND = 2,
	MAP_WIND_HILL = 3,
	MAP_WIZ_WIZ = 4,
	MAP_WEST_WIZ = 5,
	MAP_BLUE_MOON = 6,
	MAP_SILVIA_CANNON = 7,
	MAP_ICE_CANNON = 8,
	MAP_WHITE_WIZ = 9,
	MAP_SHINING_SAND = 10,
	MAP_PINK_WIND = 11,
	MAP_MAP_12 = 12,
	MAP_DEEP_INFERNO = 13,
	MAP_ICE_SPA = 14,
	MAP_LOST_SEAWAY = 15,
	MAP_EASTERN_VALLEY = 16,
	MAP_MAP_17 = 17,
	MAP_MAP_18 = 18,
	MAP_WIZ_CITY = 19,
	MAP_RANDOM = 127,
};

struct EncryptKey
{
	unsigned char m_byKey[16];

	EncryptKey()
	{
		memset(m_byKey, 0, sizeof(m_byKey));
		for (int i = 0; i < 16; i++)
			m_byKey[i] = (unsigned char)(rand() % 255);
	}
	~EncryptKey() { }

	unsigned char GetKey(int index) { return m_byKey[index]; }
};

#pragma pack(push, 1)

struct sRoomDetail
{
	unsigned char holes;
	unsigned long gameTime;
	unsigned char map;
	unsigned char gameType;
	unsigned char holeOrder;
	unsigned long matchTypeId;
};

struct sPangYaUserInfo
{
	char sID[22];
	char sNick[22];
	char sGuild[21];
	char szEmblemName[12];
	unsigned long school;
	unsigned long dwIdentity;
	unsigned long dwGalleryGuid;
	unsigned long dwGuid;
	unsigned long dwRank[3];
	unsigned long dwGuildId;
	unsigned long dwEmblemVer;
	unsigned char DoTutorial : 1;
	unsigned char gender : 1;
	unsigned char manner : 1;
	unsigned char angelicWings : 1;
	unsigned char angelicWingsEffect : 1;
	unsigned char devilWings : 1;
	unsigned char RookieFEvent : 2;
	unsigned char NewYearEvent : 1;
	unsigned char Reserved : 7;
	short iBongdariShopUsableTimes;
	short iBongdariShopBonusTimes;
	short iBongdariShopRemainedBonus;
	unsigned long dwPointPointEvent;
	unsigned long dwFlagBlock;
	int iTimeBlock;
	int nChannelingFlag;
	char sDisplayID[128];
	unsigned long dwUID;

	bool IsIdentity(unsigned long identity)
	{
		return (dwIdentity & identity) ? true : false;
	}
};

struct sPangYaUserStatistics
{
	unsigned long dwDrive;
	unsigned long dwPutt;
	unsigned long dwVisitTime;
	unsigned long dwShotTime;
	float fLongest;
	unsigned long dwPangya;
	unsigned long dwTimeOut;
	unsigned long dwOB;
	unsigned long dwDistance;
	unsigned long dwHole;
	unsigned long dwMatchHole;
	unsigned short wHoleInOne;
	unsigned short wBunker;
	unsigned long dwFairway;
	unsigned short wAlbatross;
	unsigned long dwHoleIn;
	unsigned long dwPuttIn;
	float fLongestPuttIn;
	float fLongestChipIn;
	unsigned long dwExp;
	unsigned char Level;
	__int64 i64Pang;
	int iTotalScore;
	char cBestScore[6];
	__int64 i64MaxPang[6];
	unsigned long dwGameCount;
	unsigned long dwDisconnectCount;
	unsigned long dwTeamWin;
	unsigned long dwTeamGames;
	unsigned long dwLadderPoint;
	unsigned long dwLadderWin;
	unsigned long dwLadderLose;
	unsigned long dwLadderDraw;
	unsigned long dwLadderHoles;
	unsigned int nMannerCombo;
	unsigned int nMaxMannerCombo;
	unsigned long dwNoMannerGameCount;
	__int64 i64TotalSkinsPang;
	unsigned long dwSkinsWin;
	unsigned long dwSkinsLose;
	unsigned long dwSkinsGames;
	int iStrikePoint;
	unsigned short iAllInCount;
	unsigned long eventValue;
	unsigned long eventFlag;
	unsigned long dwSeasonCount;
	__int64 i64SumPang;
};

struct sTrophyStatistics
{
	unsigned short Trophy[13][3];
};

struct sUserEquip
{
	unsigned long guidCaddie;
	unsigned long guidChar;
	unsigned long guidClubSet;
	unsigned long tidBall;
	unsigned long tidItemSlot[10];
	unsigned long guidSkin[6];
	unsigned long tidSkin[6];
	unsigned long guidMascot;
};

struct sMapStatistics
{
	unsigned char bMap;
	unsigned long dwDrive;
	unsigned long dwPutt;
	unsigned long dwHole;
	unsigned long dwFairway;
	unsigned long dwHoleIn;
	unsigned long dwPuttIn;
	int iTotalScore;
	char cBestScore;
	__int64 i64MaxPang;
	unsigned long tidChar;
	unsigned char eventScore;

	sMapStatistics() { bMap = 0xff; }
};

struct sCharacterInfo
{
	unsigned long tid;
	unsigned long guid;
	unsigned long tidParts[24];
	unsigned char hairClr;
	unsigned char shirtsClr : 4;
	unsigned char gift_flag : 4;
	char PCL[5];
	char Purchase;
	unsigned long tidAuxParts[5];
	char UccIndexList[24][9];
	unsigned long ItemIdList[24];

	sCharacterInfo()
	{
		tid = 0;
		guid = 0;
		hairClr = 0;
		shirtsClr = 0;
		gift_flag = 0;
		Purchase = 0;
		memset(tidParts, 0, sizeof(tidParts));
		memset(PCL, 0, sizeof(PCL));
		memset(tidAuxParts, 0, sizeof(tidAuxParts));
		memset(ItemIdList, 0, sizeof(ItemIdList));
		memset(UccIndexList, 0, sizeof(UccIndexList));
	}
};

struct sMyInfo
{
	unsigned short roomIndex;
	sPangYaUserInfo info;
	sPangYaUserStatistics stat;
	sTrophyStatistics trophy;
	sUserEquip userEquip;
	sMapStatistics mapStat[20];
	sMapStatistics classicMapStat[20];
};

struct sCaddieInfo
{
	unsigned long guid;
	unsigned long tid;
	unsigned long tidPart;
	unsigned char Level;
	unsigned long Exp;
	unsigned char gift_flag : 1;
	unsigned char Rent_flag : 1;
	unsigned short Remain_Date;
	unsigned short Remain_Partdate;
	char Purchase;
	unsigned char byCheckCaddieWarning;
	bool PCBangCaddie;
};

struct sClubInfo
{
	unsigned long guid;
	unsigned long tid;
	short PCL[5];
};

struct sMascotInfo
{
	unsigned long guid;
	unsigned long tid;
	unsigned char Level;
	unsigned long Exp;
	char szMsg[30];
	unsigned short Remain_Date;
	char Purchase;
	_SYSTEMTIME endDate;
	bool PCBangMascot;
};

struct sUserInfo : public sMyInfo
{
	sCharacterInfo charInfo;
	sCaddieInfo caddieInfo;
	sClubInfo clubInfo;
	sMascotInfo mascotInfo;
};

struct sSpecialTrophy
{
	unsigned long guid;
	unsigned long tid;
	unsigned long count;
};

struct sGuildTrophy
{
	unsigned long TypeID;
};

struct sSeasonRecord
{
	sPangYaUserStatistics userStat;
	sMapStatistics mapStat[20];
	sMapStatistics classicMapStat[20];
	sTrophyStatistics trophyStat;
	int numTrophies;
	sSpecialTrophy* trophies;
	int numGuildTrophies;
	sGuildTrophy* guildTrophies;
};

#pragma pack(pop)

#pragma pack(push, 1)
struct sUserInfoExt : public sUserInfo
{
	sUserInfoExt()
	{
		memset(&charInfo, 0, sizeof(charInfo));
		memset(&caddieInfo, 0, sizeof(caddieInfo));
		memset(&clubInfo, 0, sizeof(clubInfo));
		memset(&mascotInfo, 0, sizeof(mascotInfo));
		memset(&info, 0, sizeof(info));
		memset(&stat, 0, sizeof(stat));
		memset(&trophy, 0, sizeof(trophy));
		memset(&userEquip, 0, sizeof(userEquip));
		memset(mapStat, 0, sizeof(mapStat));
		memset(ext, 0, sizeof(ext));
	}

	unsigned char ext[0x400];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sUserMatchHistory
{
	int nGender;
	char szNickName[22];
	char szLoginID[22];
	unsigned long dwUID;

	sUserMatchHistory()
		: nGender(0), dwUID(0)
	{
		memset(szNickName, 0, sizeof(szNickName));
		memset(szLoginID, 0, sizeof(szLoginID));
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sLevelUpDone
{
	unsigned char bDone;
	unsigned char bLevel;
	unsigned char bItemType;

	sLevelUpDone()
		: bDone(0), bLevel(0), bItemType(0)
	{
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sBriefUserInfo
{
	unsigned long dwUid;
	unsigned long dwGuid;
	unsigned short roomIndex;
	char sNick[22];
	unsigned char level;
	unsigned long dwIdentity;
	unsigned long dwTitle;
	unsigned long dwLadderPoint;
	unsigned char bSleep : 1;
	unsigned char gender : 3;
	unsigned char manner : 1;
	unsigned char angelicWings : 1;
	unsigned char Reserve : 2;
	unsigned long m_GuildId;
	char szEmblemName[12];
	unsigned short state;
	int nChannelingFlag;
	unsigned char reserved[0x80];

	bool IsIdentity(unsigned long identity)
	{
		return (dwIdentity & identity) ? true : false;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sUserPosition
{
	unsigned short iRoomIdx;
	int iRoomType;
	int iServerGUID;
	unsigned char iChannelUid;
	char csChannelName[64];

	sUserPosition()
	{
		iRoomIdx = 0xffff;
		iRoomType = -1;
		iServerGUID = -1;
		iChannelUid = 0xff;
		memset(csChannelName, 0, sizeof(csChannelName));
	}
};

struct sUserInfoTime
{
	unsigned long dwUpdate;
	sUserInfo info;
	int numTrophies;
	sSpecialTrophy* trophies;
	int numGuildTrophies;
	sGuildTrophy* guildTrophies;
	unsigned long dwGuildPang;
	unsigned long dwGuildPoint;
	sSeasonRecord oldSeason;
	sUserPosition userPosition;
	unsigned long hasSeasonData;
};

struct sItemInfo
{
	unsigned long guid;
	unsigned long tid;
	int HourRemain;
	short Common[5];
	char Purchase;
	unsigned char gift_flag : 1;
	unsigned char Expired : 1;
	unsigned char ItemFlag : 2;
	unsigned char ItemType : 3;
	_SYSTEMTIME ItemDate;
	unsigned char Reserve : 1;
	unsigned char IsValid : 1;
	char ItemName[41];
	char UccIndex[9];
	unsigned char status;
	unsigned short Seq;
	char CopierNick[22];
	unsigned long attachCard[12];
	unsigned short CharacterSlotNum;
	unsigned short CaddieSlotNum;

	sItemInfo()
	{
		gift_flag = 0;
		Expired = 0;
		ItemFlag = 0;
		ItemType = 0;
		Reserve = 0;
		IsValid = 0;
		guid = 0;
		tid = 0;
		HourRemain = 0;
		Purchase = 0;
		memset(Common, 0, sizeof(Common));
		memset(&ItemDate, 0, sizeof(ItemDate));
		memset(ItemName, 0, sizeof(ItemName));
		memset(UccIndex, 0, sizeof(UccIndex));
		memset(CopierNick, 0, sizeof(CopierNick));
		memset(attachCard, 0, sizeof(attachCard));
		CharacterSlotNum = 0;
		CaddieSlotNum = 0;
		status = 0;
		Seq = 0;
	}

	bool operator==(const unsigned long& type) const { return tid == type; }
};

#pragma pack(pop)

#pragma pack(push, 1)
struct sGuildRoomInfo
{
	unsigned long nGuildID[2];
	char szName[2][21];
	char szEmblemName[2][12];

	void Reset()
	{
		memset(nGuildID, 0, sizeof(nGuildID));

		memset(szName, 0, sizeof(szName));
		memset(szEmblemName, 0, sizeof(szEmblemName));
	}

	void Reset(int index)
	{
		if (index >= 2)
			return;

		nGuildID[index] = 0;
		szName[index][0] = 0;
		szEmblemName[index][0] = 0;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sRoomInfo
{
	char title[32];
	char password[16];
	bool bPublic;
	bool bAvailable;
	bool bIntrusion;
	unsigned char nUserLimit;
	unsigned char nUserNum;
	EncryptKey RoomKey;
	unsigned char nGalleryNum;
	unsigned char nGalleryLimit;
	unsigned char nHole;
	unsigned char gameType;
	unsigned short roomGuid;
	unsigned char holeType;
	unsigned char mapType;
	unsigned long shotTimeLimit;
	unsigned long gameTimeLimit;
	unsigned long tidMatch;
	bool bSleep;
	bool bAdminMaster;
	sGuildRoomInfo GuildInfo;
	unsigned long pangMultiply;
	unsigned long expMultiply;
	int masterUID;
	unsigned char realGameType;

	sRoomInfo()
	{
		title[0] = 0;
		password[0] = 0;

		bAvailable = false;
		tidMatch = 0;
		bSleep = false;
		bAdminMaster = false;

		realGameType = 0;
		roomGuid = 0xffff;
		bPublic = true;

		GuildInfo.Reset();
	}

	bool operator==(const unsigned long& number) const
	{
		return roomGuid == number;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sSlotInfo
{
	unsigned long dwGuid;
	char sNick[22];
	char sGuild[21];
	unsigned char connectionRank;
	unsigned long dwIdentity;
	unsigned long dwTitle;
	unsigned long tidChar;
	unsigned long tidSkin[6];
	unsigned char bTeam : 2;
	unsigned char bSleep : 1;
	unsigned char bMaster : 2;
	unsigned char gender : 3;
	unsigned char manner : 1;
	unsigned char bReady : 1;
	unsigned char stateReserved : 6;
	unsigned char level;
	unsigned char angelicWings : 1;
	unsigned char angelicWingsEffect : 1;
	unsigned char Reserved : 6;
	unsigned char ladderGrade;
	unsigned long GuildId;
	char szEmblemName[12];
	unsigned long dwUserUID;
	unsigned long state;
	unsigned short subRoomIndex;
	unsigned long action;
	float location[3];
	unsigned long iStateTrade;
	char strTradeTitle[64];
	unsigned long tidMascot;
	unsigned char pangma[2];
	unsigned long nChannelingFlag;
	char sDisplayID[128];
	unsigned char IsInvite : 1;
	unsigned char guestReserved : 7;

	sSlotInfo()
	{
		dwUserUID = 0;

		dwGuid = 0;

		level = 0;
		tidChar = 0;
		connectionRank = 0;

		bTeam = 0;
		bSleep = 0;
		bMaster = 0;
		gender = 0;
		dwIdentity = 0;

		manner = 0;
		nChannelingFlag = 0;
		dwTitle = 0x30000000;
		IsInvite = 0;

		memset(sDisplayID, 0, sizeof(sDisplayID));

		memset(szEmblemName, 0, sizeof(szEmblemName));

		memset(tidSkin, 0, sizeof(tidSkin));
	}

	bool operator==(const unsigned long& id) const { return dwGuid == id; }

	bool IsIdentity(unsigned long flag)
	{
		return dwIdentity & flag ? true : false;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sChannelInfo
{
	char Name[64];
	unsigned short Max_Num;
	unsigned short Current_Num;
	unsigned char Uid;
	unsigned long Type;
	int Property;

	bool operator==(const unsigned char& channel) const
	{
		return Uid == channel;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sGameServerInfo
{
	char name[40];
	unsigned long guid;
	int maxUser;
	int curUser;
	char addr[18];
	int port;
	unsigned long property;
	int angelicWingsNum;
	unsigned long eventFlags;
	unsigned long eventValue;
	unsigned short iconIndex;

	sGameServerInfo()
	{
		name[0] = 0;
		addr[0] = 0;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sFriend
{
	char NickName[22];
	char szAlias[11];
	unsigned long Uid;
	unsigned long Guid;
	unsigned long Server;
	unsigned long dwGuildId;
	char szEmblemName[12];
	sUserPosition userPosition;

	unsigned char State;
	unsigned char Channel;
	unsigned char GameLevel;
	unsigned char Gender : 1;
	unsigned char IsLogOn : 1;
	unsigned char IsAccept : 1;
	unsigned char IsAgree : 1;
	unsigned char IsBlock : 1;
	unsigned char IsPlay : 1;
	unsigned char IsDive : 1;
	unsigned char IsBusy : 1;
	unsigned char PangyaFriend : 1;
	unsigned char GuildFriend : 1;
	unsigned char IsBlocked : 1;
	unsigned char Reserved : 5;

	sFriend() { Clear(); }

	void Clear()
	{
		memset(NickName, 0, sizeof(NickName));
		memset(szAlias, 0, sizeof(szAlias));
		Uid = 0xffffffff;

		Guid = 0xffffffff;
		Server = 0;

		dwGuildId = 0xffffffff;

		memset(szEmblemName, 0, sizeof(szEmblemName));

		memset(userPosition.csChannelName, 0,
			sizeof(userPosition.csChannelName));
		userPosition.iChannelUid = 0xff;

		userPosition.iRoomType = 0xffffffff;
		userPosition.iServerGUID = 0xffffffff;

		GameLevel = 0;

		Gender = 0;
		IsLogOn = 0;
		IsAccept = 0;
		IsAgree = 0;
		IsBlock = 0;
		IsPlay = 0;
		IsDive = 0;
		IsBusy = 0;

		PangyaFriend = 0;
		GuildFriend = 0;
		IsBlocked = 0;
		Reserved = 0;
		userPosition.iRoomIdx = 0xffff;
		State = 5;
		Channel = 0xff;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sNoteInfo
{
	unsigned long uid;
	unsigned short reserved;
	char sNick[22];
	char sNote[64];
	char sTime[16];
	bool bReply;

	sNoteInfo() { clear(); }

	void clear()
	{
		uid = 0xffffffff;
		reserved = 0;
		memset(sNick, 0, sizeof(sNick));
		memset(sNote, 0, sizeof(sNote));
		memset(sTime, 0, sizeof(sTime));
		bReply = false;
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sMailIncludeItem
{
	unsigned long dwIDX;
	unsigned long dwTID;
	unsigned char btItemType;
	int iCount;
	int iTimeCount;
	short soCom[5];
	unsigned short CharacterSlotNum;
	unsigned short CaddieSlotNum;
	unsigned long dwSetTID;
	int iSetItemCount;
	char csUCCIndex[9];
	unsigned long reserved30;
	unsigned char reserved34;

	sMailIncludeItem();
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sMailInfoBrief
{
	unsigned long id;
	char sender[22];
	char reserved[0x6e];
	bool bRead;
	int itemCount;
	sMailIncludeItem item;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sGiftInfo
{
	unsigned long guid;
	unsigned long tid;
	unsigned short Arg0;
	char sFromID[128];
	char sMsg[80];
	char sDate[32];
	unsigned char ItemType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sMailInfo
{
	unsigned long dwIndex;
	char szFromNick[22];
	char szDate[32];
	char szContent[800];
	bool bHaveItem;
	std::list<sMailIncludeItem> m_lMailItemList;
	bool bRead;

	sMailInfo()
	{
		dwIndex = 0xffffffff;
		memset(szFromNick, 0, sizeof(szFromNick));
		memset(szDate, 0, sizeof(szDate));
		memset(szContent, 0, sizeof(szContent));
		bHaveItem = false;
		m_lMailItemList.clear();
	}
};
#pragma pack(pop)

class CMapTypeAdapter
{
public:
	enum eMapIndex
	{
		MI_NONE = -1,
		MI_BLUE_LAGOON = 0,
		MI_BLUE_WATER = 1,
		MI_SEPIA_WIND = 2,
		MI_WIND_HILL = 3,
		MI_WIZ_WIZ = 4,
		MI_WEST_WIZ = 5,
		MI_BLUE_MOON = 6,
		MI_SILVIA_CANNON = 7,
		MI_ICE_CANNON = 8,
		MI_WHITE_WIZ = 9,
		MI_SHINING_SAND = 10,
		MI_PINK_WIND = 11,
		MI_MAP_12 = 12,
		MI_DEEP_INFERNO = 13,
		MI_ICE_SPA = 14,
		MI_LOST_SEAWAY = 15,
		MI_EASTERN_VALLEY = 16,
		MI_WIZ_CITY = 19,
		MI_RANDOM = 127,
	};

	enum eMapFlag
	{
		MF_NONE = 0,
		MF_BLUE_LAGOON = 0x1,
		MF_BLUE_WATER = 0x2,
		MF_SEPIA_WIND = 0x4,
		MF_WIND_HILL = 0x8,
		MF_WIZ_WIZ = 0x10,
		MF_WEST_WIZ = 0x20,
		MF_BLUE_MOON = 0x40,
		MF_SILVIA_CANNON = 0x80,
		MF_ICE_CANNON = 0x100,
		MF_WHITE_WIZ = 0x200,
		MF_SHINING_SAND = 0x400,
		MF_PINK_WIND = 0x800,
		MF_DEEP_INFERNO = 0x1000,
		MF_ICE_SPA = 0x2000,
		MF_LOST_SEAWAY = 0x4000,
		MF_EASTERN_VALLEY = 0x8000,
		MF_WIZ_CITY = 0x40000,
	};

	static eMapFlag GetMapFlag(eMapType type)
	{
		eMapFlag flag;
		switch (type)
		{
		case MAP_BLUE_LAGOON:
			flag = MF_BLUE_LAGOON;
			break;
		case MAP_BLUE_WATER:
			flag = MF_BLUE_WATER;
			break;
		case MAP_SEPIA_WIND:
			flag = MF_SEPIA_WIND;
			break;
		case MAP_WIND_HILL:
			flag = MF_WIND_HILL;
			break;
		case MAP_WIZ_WIZ:
			flag = MF_WIZ_WIZ;
			break;
		case MAP_WEST_WIZ:
			flag = MF_WEST_WIZ;
			break;
		case MAP_BLUE_MOON:
			flag = MF_BLUE_MOON;
			break;
		case MAP_SILVIA_CANNON:
			flag = MF_SILVIA_CANNON;
			break;
		case MAP_ICE_CANNON:
			flag = MF_ICE_CANNON;
			break;
		case MAP_WHITE_WIZ:
			flag = MF_WHITE_WIZ;
			break;
		case MAP_SHINING_SAND:
			flag = MF_SHINING_SAND;
			break;
		case MAP_PINK_WIND:
			flag = MF_PINK_WIND;
			break;
		case MAP_DEEP_INFERNO:
			flag = MF_DEEP_INFERNO;
			break;
		case MAP_ICE_SPA:
			flag = MF_ICE_SPA;
			break;
		case MAP_LOST_SEAWAY:
			flag = MF_LOST_SEAWAY;
			break;
		case MAP_EASTERN_VALLEY:
			flag = MF_EASTERN_VALLEY;
			break;
		case MAP_WIZ_CITY:
			flag = MF_WIZ_CITY;
			break;
		}

		return flag;
	}

	static eMapFlag GetMapFlag(eMapIndex index)
	{
		eMapFlag flag = MF_NONE;

		switch (index)
		{
		case MI_BLUE_LAGOON:
			flag = MF_BLUE_LAGOON;
			break;
		case MI_BLUE_WATER:
			flag = MF_BLUE_WATER;
			break;
		case MI_SEPIA_WIND:
			flag = MF_SEPIA_WIND;
			break;
		case MI_WIND_HILL:
			flag = MF_WIND_HILL;
			break;
		case MI_WIZ_WIZ:
			flag = MF_WIZ_WIZ;
			break;
		case MI_WEST_WIZ:
			flag = MF_WEST_WIZ;
			break;
		case MI_BLUE_MOON:
			flag = MF_BLUE_MOON;
			break;
		case MI_SILVIA_CANNON:
			flag = MF_SILVIA_CANNON;
			break;
		case MI_ICE_CANNON:
			flag = MF_ICE_CANNON;
			break;
		case MI_WHITE_WIZ:
			flag = MF_WHITE_WIZ;
			break;
		case MI_SHINING_SAND:
			flag = MF_SHINING_SAND;
			break;
		case MI_PINK_WIND:
			flag = MF_PINK_WIND;
			break;
		case MI_MAP_12:
			flag = MF_NONE;
			break;
		case MI_DEEP_INFERNO:
			flag = MF_DEEP_INFERNO;
			break;
		case MI_ICE_SPA:
			flag = MF_ICE_SPA;
			break;
		case MI_LOST_SEAWAY:
			flag = MF_LOST_SEAWAY;
			break;
		case MI_EASTERN_VALLEY:
			flag = MF_EASTERN_VALLEY;
			break;

		case MI_WIZ_CITY:
			flag = MF_WIZ_CITY;
			break;
		}

		return flag;
	}

	static eMapIndex GetMapIndex(eMapType type)
	{
		switch (type)
		{
		case MAP_BLUE_LAGOON:
			return MI_BLUE_LAGOON;
		case MAP_BLUE_WATER:
			return MI_BLUE_WATER;
		case MAP_SEPIA_WIND:
			return MI_SEPIA_WIND;
		case MAP_WIND_HILL:
			return MI_WIND_HILL;
		case MAP_WIZ_WIZ:
			return MI_WIZ_WIZ;
		case MAP_WEST_WIZ:
			return MI_WEST_WIZ;
		case MAP_BLUE_MOON:
			return MI_BLUE_MOON;
		case MAP_SILVIA_CANNON:
			return MI_SILVIA_CANNON;
		case MAP_ICE_CANNON:
			return MI_ICE_CANNON;
		case MAP_WHITE_WIZ:
			return MI_WHITE_WIZ;
		case MAP_SHINING_SAND:
			return MI_SHINING_SAND;
		case MAP_PINK_WIND:
			return MI_PINK_WIND;
		case MAP_MAP_12:
			return MI_MAP_12;
		case MAP_DEEP_INFERNO:
			return MI_DEEP_INFERNO;
		case MAP_ICE_SPA:
			return MI_ICE_SPA;
		case MAP_LOST_SEAWAY:
			return MI_LOST_SEAWAY;
		case MAP_EASTERN_VALLEY:
			return MI_EASTERN_VALLEY;
		case MAP_WIZ_CITY:
			return MI_WIZ_CITY;
		case MAP_RANDOM:
			return MI_RANDOM;
		default:
			return MI_NONE;
		}
	}
};

struct sPouchPrize
{
	unsigned long dwTid;
	unsigned long dwGuid;
	int iCount;
	unsigned long dwFlag1;
	unsigned long dwFlag2;

	sPouchPrize() { }
};

#pragma pack(push, 1)
struct sApproachResultData
{
	bool bExit;
	unsigned long dwGUID;
	unsigned long dwRealUID;
	unsigned char byRank;
	unsigned long dwPrizeCount;
	unsigned long dwRemainDistance;
	unsigned long dwRemainTime;
	unsigned char byRankPrize;
	unsigned char byLuckPrize;

	sApproachResultData& operator=(sApproachResultData& rhs)
	{
		bExit = rhs.bExit;
		dwGUID = rhs.dwGUID;
		dwRealUID = rhs.dwRealUID;
		byRank = rhs.byRank;
		dwPrizeCount = rhs.dwPrizeCount;
		dwRemainDistance = rhs.dwRemainDistance;
		dwRemainTime = rhs.dwRemainTime;
		byRankPrize = rhs.byRankPrize;
		byLuckPrize = rhs.byLuckPrize;
		return *this;
	}
};
#pragma pack(pop)

enum eCUTINANISTLYE
{
	CUTIN_NONE,
	CUTIN_TORIGHT,
	CUTIN_TOLEFT,
	CUTIN_ZOOM_IN,
	CUTIN_ZOOM_OUT,
	CUTIN_FADE_IN,
	CUTIN_FADE_OUT,
	CUTIN_ROTATE,
	CUTIN_UNTIL,
	CUTIN_REVERSE,
};

enum eCUTINANI_DRAWPOS
{
	CUTIN_DP_NONE,
	CUTIN_DP_CENTER,
	CUTIN_DP_RIGHT,
	CUTIN_DP_LEFT,
	CUTIN_DP_TOP,
	CUTIN_DP_BOTTOM,
};

struct CUTIN_ANI_STYPEINFO
{
	eCUTINANI_DRAWPOS PosType;
	eCUTINANISTLYE CharType;
	eCUTINANISTLYE BgType;
	eCUTINANISTLYE PatternType;
	eCUTINANISTLYE TextType;
	eCUTINANISTLYE OutType;
	char szCharTex[40];
	char szBgTex[40];
	char szPattern[40];
	char szText[40];

	CUTIN_ANI_STYPEINFO() { Reset(); }

	void Reset()
	{
		PosType = CUTIN_DP_NONE;
		CharType = CUTIN_NONE;
		BgType = CUTIN_NONE;
		PatternType = CUTIN_NONE;
		TextType = CUTIN_NONE;
		OutType = CUTIN_NONE;

		memset(szCharTex, 0, sizeof(szCharTex));
		memset(szBgTex, 0, sizeof(szBgTex));
		memset(szPattern, 0, sizeof(szPattern));
		memset(szText, 0, sizeof(szText));
	}
};

static bool IsNoEffectPrize(int type)
{
	switch (type)
	{
	case 3:
	case 4:
	case 5:
		return true;
	}

	return false;
}

enum GIMMICK_ITEM_TYPE
{
	GIMMICK_ITEM_ANY = -1,
	GIMMICK_ITEM_PANG,
	GIMMICK_ITEM_BOX,
};

namespace FieldItem
{
	enum eDisposeTextureType
	{
		DISPOSE_TEXTURE_NONE,
		DISPOSE_TEXTURE_COIN,
		DISPOSE_TEXTURE_BOOSTER,
	};

	struct AcquiredItemInfo
	{
		GIMMICK_ITEM_TYPE type;
		unsigned long index;
		unsigned long count;
		eDisposeTextureType textureType;

		void EncodeTo(WSendPacket& packet) const

		{
			packet.Encode1((unsigned char)type);
			packet.Encode4(index);
			packet.Encode1((unsigned char)count);
			packet.Encode1((unsigned char)textureType);
		}
	};
}

struct GimmickDispositionInformation
{
	enum eFlag
	{
		GIMMICK_UNUSED,
		GIMMICK_USED,
		GIMMICK_ANY,
	};

	GIMMICK_ITEM_TYPE type;
	unsigned long index;
	eFlag flag;
	unsigned long unknownC;
	unsigned char hole;

	GimmickDispositionInformation()
		: type(GIMMICK_ITEM_ANY),
		  index(0),
		  flag(GIMMICK_UNUSED),
		  unknownC(20),
		  hole(0)
	{
	}
};

#pragma pack(push, 1)
struct sSCardAvilityPeriodInfo
{
	unsigned long uid;
	unsigned long tid;
	unsigned long partsTid;
	unsigned long partsUid;
	int Avility;
	unsigned int AvilityValue;
	int slotNum;
	_SYSTEMTIME useStartTime;
	_SYSTEMTIME useEndTime;
	int cardType;
	unsigned char valid;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct sCards
{
	unsigned long uid;
	unsigned long tid;
	unsigned long partTid;
	unsigned long partUid;
	int selectedCardSlotNum;
	int cardCount;
	_SYSTEMTIME useStartTime;
	_SYSTEMTIME useEndTime;
	unsigned char valid;
	unsigned char bUseDT;
};

namespace IFF_STRUCT
{
	struct sQuest;
}

struct sQuest
{
	unsigned long typeId;
	unsigned long count;
	IFF_STRUCT::sQuest* pQuest;
	unsigned char flag;
};

struct sItemAttachCard
{
	unsigned long typeId;
	unsigned long id;
};

struct sCaddieReportData
{
	unsigned long uid;
	__int64 pang;
	__int64 bonusPang;
	unsigned long roomType;
	unsigned long exp;
	unsigned long mascotTypeId;
	unsigned char premium;
	char pangItem;
	unsigned short level;
	unsigned char reserved24[2];
	char score;
	unsigned char awardFlag;
	unsigned char eventType;
	unsigned char reserved29[22];
	char nickname[22];
	unsigned char reserved55[4];
	unsigned long guildUID;
	char guildMark[12];
	unsigned char gameType;
	unsigned char reserved6a[3];
	unsigned char team;
	unsigned char eventFlag;
	unsigned char reserved6f[16];
};

struct sRoomUserInfo
{
	unsigned long uid;
	unsigned char level;
	unsigned char hole : 5;
	unsigned char reserved05 : 3;
	unsigned long capability;
	unsigned char reserved0a[4];
	unsigned long ladderPoint;
};

struct sGuildMatchFlagInfo
{
	unsigned char reserved[0x2e];
};

struct sMapEventInfo
{
	unsigned char reserved[0x28];
};

struct sBurnningSpCard
{
	unsigned char reserved[0x14];
};

struct GuildMemberInfo_t
{
	unsigned char reserved[0x40];
};

struct sFurniture_List
{
	unsigned long id;
	unsigned long typeId;
	unsigned short reserved;
	float x;
	float y;
	float z;
	float r;
	unsigned char bArrange : 1;
};

struct sAwardItem
{
	unsigned long uid;
	int pang;
};

struct sRealMyRoomAuthority
{
	unsigned long ownerUID;
	unsigned short unknown4;
	bool bPrivate;
	char password[100];
};
#pragma pack(pop)

inline sMailIncludeItem::sMailIncludeItem()
{
	dwIDX = 0xffffffff;
	dwTID = 0xffffffff;

	iCount = 0;
	iTimeCount = 0;
	soCom[0] = soCom[1] = soCom[2] = soCom[3] = soCom[4] = 0;
	CharacterSlotNum = 0;
	CaddieSlotNum = 0;
	dwSetTID = 0xffffffff;
	iSetItemCount = 0;
	memset(csUCCIndex, 0, sizeof(csUCCIndex));
	reserved30 = 0;
	reserved34 = 0;
}
