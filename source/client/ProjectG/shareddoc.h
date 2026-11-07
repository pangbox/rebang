#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>
#include "../../shared/globalgamedefine.h"
#include "../../shared/globalnetworkdefine.h"
typedef std::map<unsigned long, sUserInfoTime>::iterator USERINFOTIME_MAP_IT;
typedef std::map<unsigned long, sBriefUserInfo>::iterator BRIEFUSERINFO_MAP_IT;
typedef std::map<int, GuildMemberInfo_t>::iterator GUILDMEMBER_MAP_IT;
typedef std::list<sMailInfoBrief>::iterator MAIL_LIST_IT;
typedef std::vector<unsigned long>::iterator ACQUIREITEM_LIST_IT;
typedef std::list<unsigned long>::iterator GUID_LIST_IT;
typedef std::map<unsigned long, unsigned char>::iterator INDEX_MAP_IT;
typedef std::auto_ptr<std::string>::element_type AUTOSTRING_ELEM;
#include "z_ilfill.h"
ILPAIRSS0
#include "polysoup.h"
#include "golfdoc.h"
#include "z_ilfill.h"
ILPAIRSS
ILFILL2
ILNUM_PSS
#include "../../shared/localize.h"
#include "hatmanager.h"
#include "parttidlist.h"
#include "golfrule.h"

class CExhibition;
class CPeriodContents;
class FrArea;
class FrComboBox;
struct sWinningPrize;
struct sTradeItem;
struct sStall;
struct sSaleItem;
struct sCardStack;
struct sPrizeInfo;

struct sRivalData
{
	unsigned long uid;
	unsigned long oid;
	char nickname[22];
	char guildName[21];
	unsigned char team;
	unsigned short level;
	unsigned char state;
	int holeScore[18];
	unsigned char holeStroke[18];
	__int64 holePang[18];
	__int64 totalPang;
	__int64 totalBonusPang;
	int totalScore;
	unsigned char totalStroke;
	unsigned char hole;
	unsigned char rank;
	unsigned char order;
	unsigned char quitOrder;
	unsigned char finishOrder;
	WVector ballPos;
	unsigned long observerState;
	unsigned char observerRank;
	unsigned long titleTypeId;
	unsigned char flag;
	unsigned long guildUID;
	char guildMark[12];
	unsigned long guildPoint;
	unsigned long approachDistance;
	unsigned long approachResultDistance;
	int approachTime;
	unsigned long capability;
	unsigned long mascotTypeId;
	unsigned char pangMastery;
	unsigned char pangNitro;
};

struct sFindRival
{
	unsigned long oid;

	sFindRival(unsigned long value)
		: oid(value)
	{
	}

	bool operator()(sRivalData& rival) { return rival.oid == oid; }
};

enum eStateTrade
{
	STATE_TRADE_INIT,
	STATE_TRADE_OPEN,
	STATE_TRADE_EDIT,
	STATE_TRADE_OUTOFSTOCK,
	STATE_TRADE_ENTER
};

enum eReservedChatPartner
{
};

struct sRoomSlot
{
	FrArea* pArea;
	CExhibition* pExhibition;
	CPartTidList partTidList;
	sCharacterInfo charInfo;

	sRoomSlot()
	{
		pArea = NULL;
		pExhibition = NULL;
	}

	unsigned char bAngelWing : 1;
	unsigned char bGachaWing : 1;
};

#pragma pack(push, 1)
struct sGuildMatchup
{
	unsigned char team;
	unsigned long uid[2];
};
#pragma pack(pop)

struct sGameTypeInfo
{
	const char* name;
	unsigned char maxPlayer;
	unsigned char minPlayer;
	unsigned char holes;
};

struct sGolfGame
{
	unsigned char map;
	IFF_STRUCT::sCourse* pCourse;
	unsigned char gameType;
	char gameTypeName[260];
	unsigned char holes;
	unsigned char weather;
	unsigned long shotTimeLimit;
	unsigned long gameTimeLimit;

	sGolfGame()
		: map(0),
		  pCourse(NULL),
		  gameType(0),
		  weather(0),
		  shotTimeLimit(0),
		  gameTimeLimit(0),
		  holes(18)
	{
		memset(gameTypeName, 0, sizeof(gameTypeName));
	}
};

enum eReplayModeType
{
};

#pragma pack(push, 1)
struct sRecordedItemInfo
{
	sItemInfo itemInfo;
	unsigned long course;
	SYSTEMTIME date;
	char fileName[32];
	unsigned char bValid;
};
#pragma pack(pop)

class CGolfDoc;

struct sChatLine
{
	std::string text;
	unsigned long color;
};

ILFILL_SHARED_ITEMMANAGER
#include "../../shared/itemmanager.h"
typedef std::map<unsigned long, sRoomSlot>::iterator ROOMSLOT_MAP_IT;
typedef std::list<sItemInfo>::iterator ITEM_LIST_IT;
typedef std::list<sRecordedItemInfo>::iterator RECORDEDITEM_LIST_IT;
typedef std::list<sGiftInfo>::iterator GIFT_LIST_IT;
typedef std::map<unsigned long, sFriend>::iterator FRIEND_MAP_IT;
typedef std::map<unsigned int, std::string>::iterator STRING_MAP_IT;
typedef std::map<unsigned int, sCharacterInfo>::iterator CHAR_MAP_IT;
typedef std::map<unsigned int, sCaddieInfo>::iterator CADDIE_MAP_IT;
typedef std::map<unsigned int, sMascotInfo>::iterator MASCOT_MAP_IT;
typedef std::map<unsigned int, sItemInfo>::iterator CLUBSET_MAP_IT;
typedef std::vector<sRivalData>::iterator RIVAL_LIST_IT;
typedef std::vector<sCaddieReportData>::iterator CADDIEREPORT_LIST_IT;
typedef std::list<std::string>::iterator STRING_LIST_IT;
typedef std::list<sNoteInfo>::iterator NOTE_LIST_IT;
typedef std::list<sSpecialTrophy>::iterator SPECIALTROPHY_LIST_IT;
typedef std::list<sGuildTrophy>::iterator GUILDTROPHY_LIST_IT;
typedef std::vector<sGuildMatchup>::iterator GUILDMATCHUP_LIST_IT;
typedef std::list<sGuildMatchFlagInfo>::iterator GUILDMATCHFLAG_LIST_IT;
typedef std::list<sQuest>::iterator QUEST_LIST_IT;
typedef std::list<sTradeItem>::iterator TRADEITEM_LIST_IT;
typedef std::list<sStall>::iterator STALL_LIST_IT;
typedef std::list<sSaleItem>::iterator SALEITEM_LIST_IT;
typedef std::list<sFurniture_List>::iterator FURNITURE_LIST_IT;
typedef std::list<sCards>::iterator CARD_LIST_IT;
typedef std::list<sSCardAvilityPeriodInfo>::iterator CARDABILITY_LIST_IT;
typedef std::list<sCardStack>::iterator CARDSTACK_LIST_IT;
typedef std::list<sBurnningSpCard>::iterator BURNNINGSPCARD_LIST_IT;
typedef std::list<sMapEventInfo>::iterator MAPEVENT_LIST_IT;
typedef std::list<CGolfRule::sShotData>::iterator SHOTDATA_LIST_IT;
typedef std::list<sItemAttachCard>::iterator ITEMATTACHCARD_LIST_IT;
typedef std::map<unsigned char, sMapStatistics>::iterator MAPSTATISTICS_MAP_IT;
typedef std::list<unsigned short>::iterator COUNT_LIST_IT;
typedef std::list<sRoomInfo>::iterator ROOM_LIST_IT;
typedef std::list<sChannelInfo>::iterator CHANNEL_LIST_IT;
typedef std::list<sSlotInfo>::iterator SLOT_LIST_IT;
typedef std::list<sGameServerInfo>::iterator GAMESERVER_LIST_IT;
typedef std::vector<sGameServerInfo>::iterator GAMESERVER_VECTOR_IT;
typedef std::list<sRoomUserInfo>::iterator ROOMUSER_LIST_IT;
typedef std::list<sChatLine>::iterator CHATLINE_LIST_IT;
typedef std::map<std::string, std::list<std::string> >::iterator
	VISGROUP_MAP_IT;

class CSharedDoc : public WSingleton<CSharedDoc>
{
public:
	enum eAppGiftDownloadState
	{
	};

	CSharedDoc();
	virtual ~CSharedDoc();

	int FindUserInfoTimeUID(unsigned long uid,
		std::map<unsigned long, sUserInfoTime>::iterator& it);
	std::map<unsigned char, sMapStatistics>& GetMyPastMapStat(
		unsigned char season);
	std::string GetLoginAuthKey();
	int IsControlServerService(int service);
	void ClearUserInfoTimeMap(unsigned long uid);
	void SetRmrMode(unsigned char mode) { m_rmrMode = mode; }
	unsigned char GetRmrMode() { return m_rmrMode; }
	unsigned long GetFlagBlock() { return m_myInfo.info.dwFlagBlock; }
	short GetBsUsableTimes() { return m_myInfo.info.iBongdariShopUsableTimes; }
	short GetBsUsableBonusTimes()
	{
		return m_myInfo.info.iBongdariShopBonusTimes;
	}
	short GetBsRemainedBonusTimes()
	{
		return m_myInfo.info.iBongdariShopRemainedBonus;
	}
	int GetTimeBlock() { return m_myInfo.info.iTimeBlock; }
	void SetTimeBlock(int timeBlock) { m_myInfo.info.iTimeBlock = timeBlock; }
	void SetBsUsableTimes(short times)
	{
		m_myInfo.info.iBongdariShopUsableTimes = times;
	}
	void SetBsUsableBonusTimes(short times)
	{
		m_myInfo.info.iBongdariShopBonusTimes = times;
	}
	void SetBsRemainedBonusTimes(short times)
	{
		m_myInfo.info.iBongdariShopRemainedBonus = times;
	}

	void HaveAngelWing(bool bHave) { m_bHaveAngelWing = bHave; }
	void SetIndex(unsigned long uid);
	unsigned char GetIndex(unsigned long guid);
	float GetWtPepPangyaComboGauge(unsigned char player);
	float GetComboGaugeLimit(unsigned char team);
	SYSTEMTIME& GetServerTime();
	int CollapseItemSlot(unsigned char slot);

	unsigned long GetMapEventPangRate(int course);
	bool CanCompound(unsigned long uid);
	bool CanUseRookieChannelMap(unsigned int course);
	__int64 RemainOwnCash(unsigned long provider, __int64 cookie);

	const std::list<sItemInfo>& GetConstMyItemList() { return m_myItemList; }

	void ClearCardStack() { m_cardStackList.clear(); }
	void ClearAll();
	void ClearUcc();
	void ClearJapanVars();
	void ClearInventory();
	void ClearGameVars();
	void ClearRivalVars(bool bAll);
	int GetBuffValue(int type, int index);
	void ClearOldSeasonStat();
	void ClearCurrentChannel(bool bAll);
	void ClearRealEquip();
	void LoadItemDb();
	void InitGolfDoc();
	void LoadChatFilter();
	bool AreWePlayingTogether(unsigned long uid);
	bool SetWinningPrize(unsigned long uid, const sWinningPrize& prize);
	bool SetExtPrize(unsigned long uid, int num, sPrizeInfo* prize);
	float GetAuxPartProperty(unsigned char player, unsigned char type);
	bool FindEquipPart(unsigned char player, unsigned long typeId);
	void AddChar(sCharacterInfo* info);
	void AddCaddie(sCaddieInfo* info);
	void AddMascot(sMascotInfo* info);
	sCardStack* GetCardInfoToUID(unsigned long uid);
	sCardStack* GetCardInfoToTID(unsigned long tid);
	void ReplaceCardCountToTID(unsigned long tid, int count);
	void ReplaceCardCountToUID(unsigned long uid, int count);
	void DeleteCardToPartsAttach(unsigned long uid);
	void AddCardStack(sCardStack card, bool bReplace);
	void DeleteCardStack(unsigned long uid, int count);
	void AddCard(unsigned long uid, unsigned long tid, unsigned short count);
	void AddFurniture(const sFurniture_List& furniture);
	void AddMyPartsList(const sItemInfo& item);
	sMascotInfo* GetMyMascotInfo(unsigned long uid);
	sMascotInfo* GetMyEquippedMascotInfo();
	int GetNumAddItemSlotByMascot(unsigned char player);
	int GetNumAddItemSlotByMascot();
	void SendEquipItemSlotByMascot();
	void SendEquipItemSlot();
	void SetCurEquipInfo(sUserEquip* equip);
	void CheckCurrEquipParts();
	void AddMyItem(const sItemInfo& item);
	bool DecreaseMyItem(std::list<sItemInfo>::iterator& it);
	bool DeleteMyItem(unsigned long uid);
	bool DeleteMyItem(std::list<sItemInfo>::iterator& it);
	sItemInfo* FindMyItemByGuid(unsigned long uid);
	void ClearMyGuildInfo();
	const sPangYaUserInfo& GetPangyaUserInfo() { return m_myInfo.info; }
	void SetEquipCharCaddieInfoFromMyInfo();
	sRivalData* GetRival(unsigned long oid);
	const char* GetDefaultTitle(unsigned char level);
	void Log30sScore();
	void LogGstUpdateHole(unsigned long uid, unsigned char hole, int score,
		__int64 pang);
	void LoadSchoolName(char* const name, unsigned long id);
	bool InitRecentWhisperList(FrComboBox* combo);
	bool InitReservedChatList(FrComboBox* combo, eReservedChatPartner partner);
	bool AddWhisperPartner(FrComboBox* combo, const char* name);
	bool UpdateChatTarget(FrComboBox* combo);
	bool UTIL_SendChatMessage(FrComboBox* combo, const char* msg,
		bool bWhisper);
	const char* GetPrevChatTarget(const char* name);
	const char* GetNextChatTarget(const char* name);
	void CheckAngelWing();
	void SetInitHaveHalloweenItem();
	void SetInitWearHalloweenItem();
	void SortAllMyItemList();
	bool ConfirmInsertItem(unsigned long typeId, unsigned long count) const;
	bool BuildMyPartTidListDefault(unsigned long typeId);
	void BuildMyPartTidList();
	void BuildMyPartTidList(sCharacterInfo& info);
	void BuildTikiReportList();
	CPartTidList& GetMyPartTidList();
	unsigned long* GetMyAuxPartTidList();
	bool IsOverlapPart(unsigned long typeId);
	int GetIndexPangyaLogoImage();
	void SetInitAbilityItem();
	void InitTradeData();
	eStateTrade GetStateTrade() { return m_stateTrade; }
	void SetStateTrade(eStateTrade state) { m_stateTrade = state; }
	void ClearListTradeItem() { m_listTradeItem.clear(); }
	std::list<sTradeItem>* GetListTradeItem() { return &m_listTradeItem; }
	int GetSizeTradeItem() { return (int)m_listTradeItem.size(); }
	unsigned long GetTradeUID() { return m_tradeUID; }
	const char* GetTradeTitle() { return m_tradeTitle.c_str(); }
	void SetTradeTitle(std::string title) { m_tradeTitle = title; }
	int GetCountVisitor() { return m_countVisitor; }
	void SetCountVisitor(int count) { m_countVisitor = count; }
	void ClearListStallItem() { m_listStallItem.clear(); }
	std::list<sStall>* GetListStallItem() { return &m_listStallItem; }
	void AddListStallItem(sStall item) { m_listStallItem.push_back(item); }
	sStall* GetStallItem() { return &m_stallItem; }
	void SetStallItem(sStall item) { m_stallItem = item; }
	__int64 GetTradeIncome() { return m_tradeIncome; }
	void SetTradeIncome(__int64 income) { m_tradeIncome = income; }
	void AddTradeIncome(__int64 income) { m_tradeIncome += income; }
	std::list<sSaleItem>* GetMaketList() { return &m_maketList; }
	void ClearMaketList() { m_maketList.clear(); }
	void AddMaketListItem(sSaleItem item) { m_maketList.push_back(item); }
	void SetFlagBlock(unsigned long flag) { m_myInfo.info.dwFlagBlock = flag; }
	bool AddListTradeItem(sTradeItem item);
	bool MinusListTradeItem(sTradeItem item);
	void SetTradeUID(unsigned long uid);
	void ChageListStallitem(sStall* stall);
	bool SendOfflineTradeEditShop();
	bool SendOfflineTradeVisitor(unsigned long uid);
	bool SendTradeOpenShop();
	bool SendTradeCloseShop();
	bool SendTradeEditShop();
	bool SendTradeEnterShop(unsigned long uid);
	bool SendTradeExitShop(unsigned long uid);
	bool SendTradeTitle(std::string title);
	bool SendTradeShowVisitor();
	bool SendTradeIncome();
	bool SendTradeBuyItem(unsigned long uid, sTradeItem item);
	unsigned short GetSecurityKey();
	void SetLoginAuthKey(std::string key);
	void SetSecurityKey(unsigned short key);
	void RefreshItemListFromGuidList(unsigned char mode);
	void UpdateGachaTickets();
	unsigned long GetMyDisconPangPenalty();
	void LoadVisGroup(const char* filename);
	void SetTradeMode(unsigned char mode) { m_tradeMode = mode; }
	void SetHaveHalloweenItem(bool bHave) { m_bHaveHalloweenItem = bHave; }
	void SetWearHalloweenItem(bool bWear) { m_bWearHalloweenItem = bWear; }
	bool IsHaveHalloweenItem() { return m_bHaveHalloweenItem; }
	unsigned long GetFlagAbilityItem() { return m_flagAbilityItem; }
	unsigned char GetNumBonusPangItem() { return m_numBonusPangItem; }
	bool IsWearHalloweenItem() { return m_bWearHalloweenItem; }
	eReplayModeType GetPlayingMode() { return m_playingMode; }
	void SetPlayingMode(eReplayModeType mode) { m_playingMode = mode; }
	unsigned char GetTradeMode() { return m_tradeMode; }
	void SetFileName(char* const filename);
	void ReplaySavFileCheck();
	std::list<std::string>* GetVisGroup(const char* name);
	bool CheckUnderLevelClubSet(unsigned long typeId);
	void InitMapEvent();
	bool IsMapEventActive(int course);
	unsigned long GetMapEventExpRate(int course);
	void SetIdentity(int identity);
	void AccumulateIdentity(int identity);
	void RefreshAllCookie();
	void SetServerTime(_SYSTEMTIME& time);
	void SetDirectMoveRoomInfo(unsigned char type, unsigned short room,
		unsigned long uid);
	bool GetDirectMoveRoomInfo(unsigned char* type, unsigned short* room,
		unsigned long* uid);
	void ClearDirectMoveRoomInfo();
	int FindUserInfoTimeGUID(unsigned long guid,
		std::map<unsigned long, sUserInfoTime>::iterator& it);
	int FindUserInfo(unsigned long uid,
		std::map<unsigned long, sBriefUserInfo>::iterator& it);
	int FindUserInfoUID(unsigned long uid,
		std::map<unsigned long, sBriefUserInfo>::iterator& it);
	int FindUserInfoTimeUserID(const char* id,
		std::map<unsigned long, sUserInfoTime>::iterator& it);
	int InsertUserInfoTime(unsigned long uid, sUserInfoTime& info);
	CPeriodContents* GetPeriodDoc();
	void SetPeriodDoc();
	bool IsCurrentMissionBlind();
	int IsEquipParts(unsigned long uid, unsigned long typeId);
	bool IsEquipParts(sCharacterInfo& info, unsigned long uid,
		unsigned long typeId);
	void SetFriendGameSvrUID(unsigned long uid) { m_friendGameSvrUID = uid; }
	unsigned long GetFriendGameSvrUID() { return m_friendGameSvrUID; }
	void SetInviteMode(int mode) { m_inviteMode = mode; }
	int IsInviteMode() { return m_inviteMode; }
	void SetForceNicknameChange(unsigned char force)
	{
		m_forceNicknameChange = force;
	}
	unsigned char GetForceNicknameChange() const
	{
		return m_forceNicknameChange;
	}
	void SetGoWithModeStart(int uid) { m_goWithFriendUID = uid; }
	void SetGoWithModeEnd()
	{
		m_goWithMode = 0;
		m_goWithFriendUID = -1;
	}
	int GetNowGoWithMode() { return m_goWithMode; }
	int GetGoWithFriendUID() { return m_goWithFriendUID; }
	void SetGoWithMode(int mode) { m_goWithMode = mode; }

protected:
	void ClearCommonVars();
	void ClearGameMode();
	void ClearStatistics();
	void ClearServerVars();
	void ClearLobbyStateVars();
	void InitGameType();
	void LoadLevelTable();

public:
	std::string m_password;
	std::string m_gameServerAddr;
	std::string m_unusedString60;
	unsigned long m_gameServerPort;
	unsigned long m_unused80;
	unsigned long m_gameServerUID;
	unsigned long m_unused88;
	unsigned long m_loginServerUID;
	bool m_bSentHoleStat;
	unsigned long m_needParentAgree;
	std::string m_rankServerAddr;
	unsigned long m_rankServerPort;
	unsigned long m_unusedB8[4];
	unsigned long m_lobbyLoginTime;
	sRoomDetail m_roomDetail;
	sGameTypeInfo m_gameTypeInfo[15];
	unsigned char m_unused150[8];
	SYSTEMTIME m_serverTime;
	std::string m_unusedString168;
	unsigned long m_unused184;
	std::string m_unusedString188;
	int m_provType;
	std::string m_commandLineID;
	unsigned char m_unused1c4;
	int m_gachaTicket[2];
	std::list<unsigned long> m_refreshGuidList;
	std::list<unsigned short> m_refreshCountList;
	CItemManager m_itemManager;
	sMyInfo m_myInfo;
	unsigned char m_userInfoExt[0x400];
	std::map<unsigned int, std::string> m_stringMap;
	std::map<unsigned int, sCharacterInfo> m_charMap;
	std::map<unsigned int, sCaddieInfo> m_caddieMap;
	std::map<unsigned int, sMascotInfo> m_mascotMap;
	std::list<sItemInfo> m_partsList;
	std::map<unsigned int, sItemInfo> m_clubSetMap;
	std::list<sItemInfo> m_ballList;
	std::list<sItemInfo> m_myItemList;
	std::list<sRecordedItemInfo> m_recordedItemList;
	std::list<sItemInfo> m_cadItemList;
	std::list<sItemInfo> m_setItemList;
	std::list<sGiftInfo> m_giftList;
	std::list<sGiftInfo> m_sendGiftList;
	std::list<sMailInfoBrief> m_mailList;
	std::list<sItemInfo> m_cutinList;
	std::list<sSpecialTrophy> m_specialTrophyList;
	std::list<sGuildTrophy> m_guildTrophyList;
	std::list<sItemInfo> m_auxPartList;
	std::list<sItemInfo> m_questDropList;
	std::list<sFurniture_List> m_furnitureList;
	std::list<sCards> m_cardList;
	std::list<sSCardAvilityPeriodInfo> m_cardAbilityList[4];
	std::list<sQuest> m_questList;
	__int64 m_cookie;
	__int64 m_cash;
	__int64 m_bonusCash;
	std::list<sItemInfo> m_uccItemList;
	std::list<sItemInfo> m_uccAztecList;
	std::map<unsigned long, sRoomSlot> m_roomSlotMap;
	std::map<unsigned long, unsigned char> m_indexMap;
	sUserInfo m_userInfo[4];
	sPangYaUserStatistics m_holeStatistics[4];
	std::vector<unsigned long> m_acquireItemList[4];
	std::vector<sRivalData> m_rivalList;
	std::vector<sCaddieReportData> m_caddieReportList;
	std::vector<sGuildMatchup> m_guildMatchupList;
	std::list<sGuildMatchFlagInfo> m_guildMatchFlagList;
	unsigned char m_guildRefreshData[0x1c];
	bool m_bGameOver;
	int m_gameMode;
	unsigned char m_holeType;
	unsigned long m_roomType;
	unsigned char m_eventType;
	unsigned char m_eventFlag;
	sAwardItem m_awardItem[12];
	unsigned char m_courseMap[18];
	unsigned char m_pinIndex[18];
	unsigned char m_holeOrder[19];
	unsigned long m_holeRandom[18];
	unsigned long m_approachStartTime;
	float m_pangRate;
	int m_expRate;
	bool m_bFastForward;
	sGolfGame m_golfGame;
	unsigned char m_holePar[18];
	struct
	{
		int score;
		__int64 pang;
	} m_teamResult[2];
	short m_guildScore[2];
	unsigned long m_guildRoundScore[2];
	unsigned long m_unused47bc;
	__int64 m_myGamePang;
	unsigned long m_unused47c8;
	unsigned long m_unused47cc;
	unsigned char m_unused47d0;
	unsigned long m_replayTargetUID;
	sRealMyRoomAuthority m_rmrAuthority;
	bool m_bTreasureResult;
	bool m_bTreasureAward;
	sPangYaUserStatistics m_oldSeasonStatistics;
	sTrophyStatistics m_oldSeasonTrophy;
	std::list<sSpecialTrophy> m_oldSpecialTrophyList;
	std::list<sGuildTrophy> m_oldGuildTrophyList;
	sRoomInfo m_roomInfo;
	std::map<unsigned long, sBriefUserInfo> m_briefUserInfoMap;
	std::list<sRoomInfo> m_roomList;
	std::list<sChannelInfo> m_channelList;
	std::list<sSlotInfo> m_slotList;
	std::list<sSlotInfo> m_slotList2;
	std::list<sGameServerInfo> m_gameServerList;
	std::vector<sGameServerInfo> m_gameServerVector;
	std::list<sRoomUserInfo> m_roomUserList;
	const char* m_shopName;
	unsigned long m_underBarMask;
	sChannelInfo m_curChannel;
	sGameServerInfo m_curGameServer;
	std::list<unsigned long> m_guildMemberList;
	std::map<int, GuildMemberInfo_t> m_guildMemberMap;
	bool m_bAutoRefresh;
	bool m_bShowNotice;
	bool m_bBackgroundVideo;
	bool m_bReturnLobbyNotice;
	bool m_bRebuildUnderBar;
	sChannelInfo m_selChannel;
	std::map<unsigned long, sFriend> m_friendMap;
	sFriend m_selFriend;
	ChatManager m_chatManager;
	std::list<sChatLine> m_chatLineList;
	std::list<std::string> m_chatHistory;
	std::list<std::string> m_ignoreList;
	unsigned long m_lobbyExitReason;
	std::list<std::string> m_recentWhisperList;
	std::list<std::string> m_reservedChatList;
	unsigned long m_maxWhisperPartner;
	std::string m_whisperPartner;
	unsigned long m_chatMode;
	std::list<sNoteInfo> m_noteList;
	std::string m_chatModeName;
	std::string m_packetNickname;
	std::string m_nicknameSuffix;
	unsigned long m_controlServerService;
	int m_offlinePlay;
	unsigned long m_pcBang;
	unsigned long m_roomSetupStage;
	unsigned long m_serverProperty;
	unsigned char m_startHole;
	bool m_bTutorialResume;
	unsigned char m_newRecordState;
	bool m_bNewMapEvent;
	unsigned long m_newMapEventMask;
	bool m_bBingoEvent;
	unsigned long m_newRecordCourse;
	unsigned long m_tutorialComplete[3];
	unsigned long m_tutorialStartMode;
	std::string m_giftName;
	int m_giftIndex;
	bool m_bRefreshCamera;
	bool m_bMessengerAlarm;
	bool m_bMessengerInit;
	struct
	{
		unsigned long exp, totalExp;
	} m_levelTable[71];
	struct
	{
		unsigned long exp, pang;
	} m_bonusPangTable[3];
	unsigned long m_teamPlayerGuid[2];
	unsigned long m_guildBonusPang;
	unsigned long m_guildPoint;
	unsigned char m_guildTeam;
	std::string m_authTicket;
	CGolfDoc* m_pGolfDoc;
	unsigned long m_cardPackPageNum;
	unsigned long m_cardPackSelPageNum;
	unsigned long m_cardPackCurPage;
	unsigned long m_cardPackMode;
	bool m_bCardPackRebuild;
	unsigned char m_cardPackSlot;
	unsigned char m_cardPackItem;
	unsigned short m_rmrTabValue;
	bool m_bRmrTabPending;
	unsigned long m_pendingMenu;
	unsigned long m_unused5060;
	unsigned long m_specialShotMark;
	bool m_bToppageAd;
	unsigned long m_toppageFeature;
	sUserEquip m_realEquip;
	std::list<sMapEventInfo> m_mapEventList;
	std::list<CGolfRule::sShotData> m_shotDataList;
	bool m_bItemStoragePin;
	bool m_bItemStorageLock;
	bool m_bTutorialRedirect;
	CPartTidList m_partTidList;
	unsigned long m_auxPartTidList[5];
	bool m_bHaveAngelWing;
	std::string m_loginAuthKey;
	unsigned short m_securityKey;
	unsigned long m_flagAbilityItem;
	unsigned char m_numBonusPangItem;
	eStateTrade m_stateTrade;
	std::list<sTradeItem> m_listTradeItem;
	std::list<sItemAttachCard> m_listAttachCard;
	unsigned long m_tradeUID;
	std::string m_tradeTitle;
	int m_countVisitor;
	unsigned char m_tradeMode;
	sStall m_stallItem;
	std::list<sStall> m_listStallItem;
	std::list<sSaleItem> m_maketList;
	unsigned char m_rmrMode;
	__int64 m_tradeIncome;
	bool m_bHaveHalloweenItem;
	bool m_bWearHalloweenItem;
	void SetReplayState(int state) { m_replayState = state; }
	int GetReplayState() { return m_replayState; }
	char* GetFileName() { return m_fileName; }
	int GetReplayPlayType() { return m_replayPlayType; }
	void SetReplayPlayType(int type) { m_replayPlayType = type; }
	int m_replayState;
	int m_replayPlayType;
	eReplayModeType m_playingMode;
	char m_fileName[32];
	float m_replayPosition;
	unsigned long m_replayDuration;
	unsigned long m_replayTick;
	int m_replayShotIndex;
	unsigned char m_replayHoleStroke;
	unsigned long m_replayVersion;
	unsigned long m_serverTimeTick;
	unsigned char m_unused5bdc;
	CPeriodContents* m_pPeriodDoc;
	int m_goWithMode;
	int m_goWithFriendUID;
	sRoomInfo m_inviteRoomInfo;
	unsigned char m_directMoveType;
	unsigned short m_directMoveRoom;
	unsigned long m_directMoveUID;
	unsigned long m_friendGameSvrUID;
	int m_inviteMode;
	unsigned long m_unused5cb0;
	std::map<unsigned long, sUserInfoTime> m_userInfoTimeMap;
	int m_userInfoMod;
	int GetUserInfoMod() { return m_userInfoMod; }
	void SetUserInfoMod(int mod) { m_userInfoMod = mod; }
	unsigned char m_forceNicknameChange;
	std::map<unsigned char, sMapStatistics> m_pastMapStat[4];
	std::map<std::string, std::list<std::string> > m_visGroupMap;
	bool m_goStopMode;
	void SetGoStopMode(bool bMode) { m_goStopMode = bMode; }
	bool GetGoStopMode() { return m_goStopMode; }
	void SetAppGiftDownloadState(eAppGiftDownloadState state)
	{
		m_appGiftDownloadState = state;
	}

	eAppGiftDownloadState GetAppGiftDownLoadState() const
	{
		return m_appGiftDownloadState;
	}
	bool IsOpenFirstLoginAlarm() const { return m_bNeedFirstLoginAlarm; }
	void SetNeedFirstLoginAlarm(bool bNeed) { m_bNeedFirstLoginAlarm = bNeed; }
	eAppGiftDownloadState m_appGiftDownloadState;
	unsigned char m_unused5d0c;
	std::list<sCardStack> m_cardStackList;
	std::list<sBurnningSpCard> m_burnningSpCardList;
	bool m_bNeedFirstLoginAlarm;
};

int OnlinePlay();
bool SetCurMap(unsigned char map);
unsigned char GetCurMap();
bool IsCurMapSnow();

ILFILL3

#include "golftask.h"
#include "voiceitem.h"
#include "gdvoiceitem.h"
#include "icontentsdoc.h"
#include "courseorder.h"
#include "shareddoc.inl"
