#include "stdafx.h"
#include "icontentsdoc.h"
#include "contentsdoc.h"
#include "guildactor.h"
#include "guilddefine.h"
#include "../../shared/s5/sharedutilities.h"
#include "../../shared/LuaSystem/luascriptmanager.h"
#include "../../shared/LuaSystem/luatablehelper.h"

namespace
{
	// HACK!
	inline bool IsZero(float f)
	{
		return Wabs(f) < g_EPSILON;
	}
}

void CGiftBoxEvent::SetOpenedBoxTID(unsigned long tid)
{
	m_openedBoxTID = tid;
}

unsigned long CGiftBoxEvent::GetOpenedBoxTID()
{
	return m_openedBoxTID;
}

bool CGiftBoxEvent::Initialize()
{
	m_openedBoxTID = 0;
	return true;
}

bool CGiftBoxEvent::Release()
{
	return true;
}

void CWorldTourEvent::SetMapFlag(unsigned long flag)
{
	m_mapFlag = flag;
}

void CWorldTourEvent::SetGotGift(bool bGot)
{
	m_bGotGift = bGot;
}

void CWorldTourEvent::SetCanGift(bool bCan)
{
	m_bCanGift = bCan;
}

unsigned long CWorldTourEvent::GetMapFlag() const
{
	return m_mapFlag;
}

bool CWorldTourEvent::GetGotGift() const
{
	return m_bGotGift;
}

bool CWorldTourEvent::GetCanGift() const
{
	return m_bCanGift;
}

bool CWorldTourEvent::Initialize()
{
	m_mapFlag = 0;
	m_bGotGift = false;
	m_bCanGift = false;
	return true;
}

bool CWorldTourEvent::Release()
{
	return Initialize();
}

void CHalloweenEvent::SetMapFlag(unsigned long flag)
{
	m_mapFlag = flag;
}

void CHalloweenEvent::SetGiftCnt(unsigned long cnt)
{
	m_giftCnt = cnt;
}

void CHalloweenEvent::SetShowGiftDlg(bool bShow)
{
	m_bShowGiftDlg = bShow;
}

void CHalloweenEvent::SetEventState(bool bState)
{
	m_bEventState = bState;
}

unsigned long CHalloweenEvent::GetMapFlag() const
{
	return m_mapFlag;
}

unsigned long CHalloweenEvent::GetGiftCnt() const
{
	return m_giftCnt;
}

bool CHalloweenEvent::GetShowGiftDlg() const
{
	return m_bShowGiftDlg;
}

bool CHalloweenEvent::GetEventState() const
{
	return m_bEventState;
}

bool CHalloweenEvent::Initialize()
{
	m_mapFlag = 0;
	m_giftCnt = 0;
	m_bShowGiftDlg = false;
	m_bEventState = false;
	return true;
}

bool CHalloweenEvent::Release()
{
	return Initialize();
}

void CEscapeBeginnerEvent::SetFlag(bool bFlag)
{
	m_bFlag = bFlag;
}

bool CEscapeBeginnerEvent::GetFlag() const
{
	return m_bFlag;
}

bool CEscapeBeginnerEvent::Initialize()
{
	m_bFlag = false;
	return true;
}

bool CEscapeBeginnerEvent::Release()
{
	return Initialize();
}

void CPassingTitleChk::SetFlag(bool bFlag)
{
	m_bFlag = bFlag;
}

bool CPassingTitleChk::GetFlag() const
{
	return m_bFlag;
}

bool CPassingTitleChk::Initialize()
{
	m_bFlag = false;
	return true;
}

bool CPassingTitleChk::Release()
{
	return Initialize();
}

void CChristmasEvent::SetFlag(bool bFlag)
{
	m_bFlag = bFlag;
}

bool CChristmasEvent::GetFlag() const
{
	return m_bFlag;
}

bool CChristmasEvent::Initialize()
{
	m_bFlag = true;
	return true;
}

bool CChristmasEvent::Release()
{
	return Initialize();
}

void CTicketExchange::SetFlag(bool bFlag)
{
	m_bFlag = bFlag;
}

bool CTicketExchange::GetFlag() const
{
	return m_bFlag;
}

bool CTicketExchange::Initialize()
{
	m_bFlag = true;
	return true;
}

bool CTicketExchange::Release()
{
	return Initialize();
}

bool CMissionEvent::Initialize()
{
	m_eventFlag = 0;
	m_bDayGiftFlag = false;
	m_bTermGiftFlag = false;

	return true;
}

bool CMissionEvent::Release()
{
	return true;
}

void CMissionEvent::SetEventFlag(unsigned int flag)
{
	m_eventFlag = flag;
}

unsigned int CMissionEvent::GetEventFlag() const
{
	return m_eventFlag;
}

bool CMissionEvent::IsCompleteMission(int index)
{
	return m_missionInfo[index].complete;
}

bool CMissionEvent::IsCompleteAllDayMission()
{
	for (int i = 0; i < 2; ++i)
	{
		if (!IsCompleteMission(i))
			return false;
	}

	return true;
}

bool CMissionEvent::IsCompleteAllTermMission()
{
	for (int i = 2; i < 7; ++i)
	{
		if (!IsCompleteMission(i))
			return false;
	}

	return true;
}

bool CMissionEvent::IsCompleteCourse(int course)
{
	return (GetCondition(6) & GetMissionField(course)) ? true : false;
}

void CMissionEvent::DecodeMissionInfo(WReceivedPacket& packet)
{
	if (!m_missionInfo.empty())
		ClearMissionInfo();

	int count = packet.Decode4();

	for (int i = 0; i < count; ++i)
	{
		GlobalEnum::sMissionInfo info;
		packet.DecodeBuffer(&info, sizeof(info));

		m_missionInfo.push_back(info);
	}

	unsigned char dayGiftFlag = packet.Decode1();
	SetDayGiftFlag(dayGiftFlag != 0);
	unsigned char termGiftFlag = packet.Decode1();
	SetTermGiftFlag(termGiftFlag != 0);
}

int CMissionEvent::GetMissionField(int index) const
{
	static int s_missionField[] = {
		0x0001,
		0x0002,
		0x0004,
		0x0008,
		0x0010,
		0x0020,
		0x0040,
		0x0080,
		0x0100,
		0x0200,
		0x0400,
		0x0800,
		0x1000,
		0x2000,
		0x4000,
		0x8000,
	};

	if (index < 12)
		return s_missionField[index];
	return s_missionField[index + 1];
}

bool CPeriodContents::Initialize()
{
	return true;
}

bool CPeriodContents::Release()
{
	return true;
}

void CPeriodContents::AddUsableContents(int type, const SYSTEMTIME* pStart,
	const SYSTEMTIME* pEnd, bool bNoEndTime)
{
	if (type >= 4)
		return;

	sPeriodUnit unit;
	unit.startTime = *pStart;
	if (!bNoEndTime)
		unit.endTime = *pEnd;
	unit.bNoEndTime = bNoEndTime;
	unit.bUsable = false;

	m_contentsMap.insert(std::make_pair(type, unit));
}

bool CPeriodContents::IsUsableContents(int type)
{
	std::map<int, sPeriodUnit>::const_iterator it = m_contentsMap.find(type);
	if (it == m_contentsMap.end())
		return false;

	return (*it).second.bUsable;
}

void CPeriodContents::Set_PreSettingContentsDate()
{
	SYSTEMTIME startTime = S5::MakeSysTime(2011, 4, 14, 8);
	SYSTEMTIME endTime = S5::MakeSysTime(2011, 5, 3, 8);

	AddUsableContents(3, &startTime, &endTime, false);

	CheckAllContents();
}

void CPeriodContents::CheckAllContents()
{
	for (std::map<int, sPeriodUnit>::iterator it = m_contentsMap.begin();
		it != m_contentsMap.end(); ++it)
		IsUsable((*it).second);
}

bool CPeriodContents::CheckContent(int type)
{
	std::map<int, sPeriodUnit>::iterator it = m_contentsMap.find(type);
	if (it == m_contentsMap.end())
		return false;

	sPeriodUnit& unit = (*it).second;
	IsUsable(unit);

	return unit.bUsable;
}

void CPeriodContents::IsUsable(sPeriodUnit& unit)
{
	SYSTEMTIME& now = WSingleton<CSharedDoc>::Instance()->GetServerTime();

	if (unit.startTime.wYear <= now.wYear &&
		unit.startTime.wMonth <= now.wMonth &&
		unit.startTime.wDay <= now.wDay && unit.startTime.wHour <= now.wHour)
	{
		unit.bUsable = true;
	}

	if (unit.bNoEndTime == true)
		return;

	unit.bUsable = ItemManager()->IsBetween(&unit.startTime, &unit.endTime, now)
		? true
		: false;
}

void _guild::ExchangeQuotation(char* str)
{
	std::string text(str);
	str_Replace(text, "`", "'");
	strcpy(str, text.c_str());
}

namespace _guild
{
	enum eGuildMsgCode
	{
		GUILD_MSG_1000 = 1000,
		GUILD_MSG_1001 = 1001,
		GUILD_MSG_1005 = 1005,
		GUILD_MSG_1006 = 1006,
		GUILD_MSG_1007 = 1007,
		GUILD_MSG_1008 = 1008,
		GUILD_MSG_1009 = 1009,
		GUILD_MSG_1010 = 1010,
		GUILD_MSG_1011 = 1011,
		GUILD_MSG_1012 = 1012,
		GUILD_MSG_1013 = 1013,
		GUILD_MSG_1014 = 1014,
		GUILD_MSG_1015 = 1015,
		GUILD_MSG_1016 = 1016,
		GUILD_MSG_1017 = 1017,
		GUILD_MSG_1018 = 1018,
		GUILD_MSG_1019 = 1019,
		GUILD_MSG_1020 = 1020,
		GUILD_MSG_1021 = 1021,
		GUILD_MSG_1022 = 1022,
		GUILD_MSG_1024 = 1024,
		GUILD_MSG_1025 = 1025,
		GUILD_MSG_1026 = 1026,
		GUILD_MSG_1027 = 1027,
		GUILD_MSG_1028 = 1028,
		GUILD_MSG_1029 = 1029,
		GUILD_MSG_1030 = 1030,
		GUILD_MSG_1031 = 1031,
		GUILD_MSG_1032 = 1032,
		GUILD_MSG_1033 = 1033,
		GUILD_MSG_1034 = 1034,
		GUILD_MSG_1035 = 1035,
		GUILD_MSG_1036 = 1036,
		GUILD_MSG_1037 = 1037,
		GUILD_MSG_1038 = 1038,
		GUILD_MSG_1039 = 1039,
		GUILD_MSG_1040 = 1040,
		GUILD_MSG_1041 = 1041,
		GUILD_MSG_1042 = 1042,
		GUILD_MSG_1043 = 1043,
		GUILD_MSG_1044 = 1044,
		GUILD_MSG_1045 = 1045,
		GUILD_MSG_1046 = 1046,
		GUILD_MSG_1047 = 1047,
		GUILD_MSG_1048 = 1048,
		GUILD_MSG_1049 = 1049,
		GUILD_MSG_1050 = 1050,
		GUILD_MSG_1051 = 1051,
		GUILD_MSG_1052 = 1052,
		GUILD_MSG_1053 = 1053,
		GUILD_MSG_1054 = 1054,
		GUILD_MSG_1055 = 1055,
		GUILD_MSG_1056 = 1056,
		GUILD_MSG_1057 = 1057,
		GUILD_MSG_1058 = 1058,
		GUILD_MSG_1059 = 1059,
		GUILD_MSG_1060 = 1060,
		GUILD_MSG_1061 = 1061,
		GUILD_MSG_1062 = 1062,
		GUILD_MSG_1063 = 1063,
		GUILD_MSG_1064 = 1064,
		GUILD_MSG_1065 = 1065,
		GUILD_MSG_1066 = 1066,
		GUILD_MSG_1067 = 1067,
		GUILD_MSG_1068 = 1068,
		GUILD_MSG_1069 = 1069,
		GUILD_MSG_1070 = 1070,
		GUILD_MSG_1071 = 1071,
		GUILD_MSG_1072 = 1072,
		GUILD_MSG_1073 = 1073,
		GUILD_MSG_1074 = 1074,
		GUILD_MSG_1075 = 1075,
		GUILD_MSG_1076 = 1076,
		GUILD_MSG_1077 = 1077,
		GUILD_MSG_1078 = 1078,
		GUILD_MSG_1079 = 1079,
		GUILD_MSG_1080 = 1080,
		GUILD_MSG_1081 = 1081,
		GUILD_MSG_1082 = 1082,
		GUILD_MSG_1083 = 1083,
		GUILD_MSG_1084 = 1084,
	};
}

enum IngameGuildErrorCode
{
	GUILD_ERROR_0 = 0,
	GUILD_ERROR_1 = 1,
	GUILD_ERROR_54001 = 54001,
	GUILD_ERROR_54002 = 54002,
	GUILD_ERROR_54003 = 54003,
	GUILD_ERROR_54004 = 54004,
	GUILD_ERROR_54005 = 54005,
	GUILD_ERROR_54006 = 54006,
	GUILD_ERROR_54008 = 54008,
	GUILD_ERROR_54009 = 54009,
	GUILD_ERROR_54010 = 54010,
	GUILD_ERROR_54011 = 54011,
	GUILD_ERROR_54012 = 54012,
	GUILD_ERROR_54013 = 54013,
	GUILD_ERROR_54014 = 54014,
	GUILD_ERROR_54015 = 54015,
	GUILD_ERROR_54016 = 54016,
	GUILD_ERROR_54017 = 54017,
	GUILD_ERROR_54018 = 54018,
	GUILD_ERROR_54019 = 54019,
	GUILD_ERROR_54020 = 54020,
	GUILD_ERROR_54021 = 54021,
	GUILD_ERROR_54022 = 54022,
	GUILD_ERROR_54030 = 54030,
	GUILD_ERROR_54500 = 54500,
	GUILD_ERROR_54501 = 54501,
	GUILD_ERROR_54502 = 54502,
	GUILD_ERROR_54503 = 54503,
	GUILD_ERROR_54504 = 54504,
	GUILD_ERROR_54505 = 54505,
	GUILD_ERROR_54506 = 54506,
	GUILD_ERROR_54507 = 54507,
	GUILD_ERROR_54508 = 54508,
	GUILD_ERROR_54509 = 54509,
	GUILD_ERROR_54510 = 54510,
	GUILD_ERROR_54511 = 54511,
};

bool _guild::CGuildInfo::Initialize()
{
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1000,
		"\xb1\xe6\xb5\xe5\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1001,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xbc\xd2\xb0\xb3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1005,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xbb\xfd\xbc\xba\xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1006,
		"\xb1\xe6\xb5\xe5\xb8\xb6\xc5\xa9\xb8\xa6 \xb5\xee\xb7\xcf \xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1007,
		"\xb1\xe6\xb5\xe5\xb8\xed\xc0\xbb \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1008,
		"\xbd\xc2\xc0\xce(\xbd\xc2\xb1\xde)|\xb0\xad\xb5\xee|\xb1\xe6\xb5\xe5\xbf\xf8 \xc3\xdf\xb9\xe6|\xb1\xe6\xb5\xe5\xc0\xe5 \xc0\xa7\xc0\xd3|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1009,
		"\xbd\xc2\xc0\xce(\xbd\xc2\xb1\xde)|\xb1\xe6\xb5\xe5\xbf\xf8 \xc3\xdf\xb9\xe6|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1010, "\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1011,
		"\xb4\xf5 \xc0\xcc\xbb\xf3 \xbd\xc2\xb1\xde\xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1012,
		"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb0\xad\xb5\xee\xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1036,
		"\xb1\xe6\xb5\xe5\xc0\xe5\xc0\xbb \xc0\xa7\xc0\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb5\xee\xb1\xde\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1037,
		"\xb0\xcb\xbb\xf6\xbe\xee\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1038,
		"\xb0\xa1\xc0\xd4\xc7\xd2 \xb1\xe6\xb5\xe5\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));

	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1013,
		"\xc0\xcc\xb9\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1014,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb \xb1\xdb\xc0\xbb \xc0\xdb\xbc\xba\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1015,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb\xc0\xbb \xc3\xeb\xbc\xd2\xc7\xd5\xb4\xcf\xb4\xd9. \xc1\xf8\xc7\xe0\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1016,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb\xc0\xcc \xc3\xeb\xbc\xd2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1017,
		"\xb0\xad\xc1\xa6 \xc5\xbb\xc5\xf0\xbd\xc3\xc4\xd7\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1018,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb\xc0\xbb \xb0\xc5\xba\xce\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1019,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb\xc0\xbb \xbd\xc2\xc0\xce\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1020,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1021,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1022,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb5\xee\xb1\xde\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1047,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1024,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xc6\xf3\xbc\xe2\xc7\xd5\xb4\xcf\xb4\xd9. (\xb1\xe6\xb5\xe5\xb8\xa6 \xc6\xf3\xbc\xe2\xc7\xcf\xb8\xe9 24\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xbb\xf5\xb7\xce \xb1\xe6\xb5\xe5\xb8\xa6 \xb8\xb8\xb5\xe9 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.)"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1025,
		"\xb1\xe6\xb5\xe5\xb8\xa6 \xc6\xf3\xbc\xe2\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1026,
		"\xb1\xe6\xb5\xe5 \xbc\xd2\xb0\xb3\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1027,
		"\xb1\xe6\xb5\xe5 \xb0\xf8\xc1\xf6\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1028,
		"\xb1\xe6\xb5\xe5 \xc0\xcc\xb8\xa7\xc0\xbb \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1029,
		"\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xb1\xe6\xb5\xe5\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1030,
		"\xb0\xb3\xc0\xce \xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1031,
		"\xb1\xe6\xb5\xe5\xb0\xa1 \xbb\xfd\xbc\xba\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1032,
		"\xbc\xb1\xc5\xc3\xc7\xd1 \xba\xce\xb1\xe6\xb5\xe5\xc0\xe5\xbf\xa1\xb0\xd4 \xb1\xe6\xb5\xe5\xc0\xe5\xc0\xbb \xc0\xa7\xc0\xd3\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1033,
		"%s \xb1\xc7\xc7\xd1\xc0\xbb \xc0\xa7\xc0\xd3\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.\x0a%s\xc0\xcc \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1034,
		"%s \xb1\xc7\xc7\xd1\xc0\xb8\xb7\xce \xba\xaf\xb0\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1061, "\xb0\xa1\xc0\xd4\xbd\xc5\xc3\xbb"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1062, "\xb0\xa1\xc0\xd4\xc3\xeb\xbc\xd2"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1063, "\xb0\xa1\xc0\xd4\xbd\xc2\xc0\xce"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1064, "\xb0\xa1\xc0\xd4\xb0\xc5\xc0\xfd"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1065,
		"\xb1\xe6\xb5\xe5\xbf\xf8\xb5\xee\xb1\xde\xba\xaf\xb0\xe6"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1066,
		"\xb1\xe6\xb5\xe5\xbf\xf8 \xc3\xdf\xb9\xe6"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1067, "\xb1\xe6\xb5\xe5\xc5\xbb\xc5\xf0"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1068, "\xb1\xe6\xb5\xe5\xb0\xb3\xbc\xb3"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1069, "\xb1\xe6\xb5\xe5\xc6\xf3\xbc\xe2"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1070,
		"\xb1\xe6\xb5\xe5\xc0\xe5 \xc0\xa7\xc0\xd3"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1071,
		"\xb1\xe6\xb5\xe5\xc0\xe5 \xc0\xd3\xb8\xed"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1072,
		"\xba\xce\xb1\xe6\xb5\xe5\xc0\xe5 \xc0\xd3\xb8\xed"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1073,
		"\xb1\xe6\xb5\xe5\xbf\xf8 \xbd\xc2\xc0\xce"));

	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1074,
		"\xb1\xe6\xb5\xe5\xb8\xb6\xc5\xa9\xba\xed\xb7\xb0"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1075,
		"\xb1\xe6\xb5\xe5\xb8\xb6\xc5\xa9\xba\xed\xb7\xb0\xc7\xd8\xc1\xa6"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1076,
		"GM\xbf\xa1\xc0\xc7\xc7\xd1\xc3\xdf\xb9\xe6"));

	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1077, "\xb1\xe6\xb5\xe5\xc0\xe5"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1078, "\xba\xce\xb1\xe6\xb5\xe5\xc0\xe5"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1079, "\xb1\xe6\xb5\xe5\xbf\xf8"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1080,
		"\xb0\xa1\xc0\xd4\xb4\xeb\xb1\xe2\xc0\xda"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1081, "\xbd\xc2\xc0\xce(\xbd\xc2\xb1\xde)"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1082, "\xb0\xad\xb5\xee"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1083,
		"\xb1\xe6\xb5\xe5\xbf\xf8 \xc3\xdf\xb9\xe6"));
	m_guildMsgMap.insert(
		std::make_pair(GUILD_MSG_1084, "\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1035,
		"\xb0\xcb\xbb\xf6\xb5\xc8 \xb1\xe6\xb5\xe5\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1039,
		"\xb1\xe6\xb5\xe5\xb8\xed\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1040,
		"\xb0\xa1\xc0\xd4\xc7\xd1 \xb1\xe6\xb5\xe5\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1048,
		"%s \xb4\xd4\xc0\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1049,
		"%s \xb4\xd4\xc0\xcc \xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1050,
		"\xb5\xee\xb7\xcf\xb5\xc8 \xb1\xe6\xb5\xe5\xb8\xb6\xc5\xa9\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1051,
		"\xb0\xa1\xb7\xce 22, \xbc\xbc\xb7\xce 20 \xc5\xa9\xb1\xe2\xb0\xa1 \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1052,
		"png \xc6\xc4\xc0\xcf\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1053,
		"\xb1\xe6\xb5\xe5 \xb8\xb6\xc5\xa9 \xbe\xf7\xb7\xce\xb5\xe5\xbf\xa1 \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1054,
		"\xb1\xe6\xb5\xe5 \xb8\xb6\xc5\xa9 \xbe\xf7\xb7\xce\xb5\xe5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1056,
		"\xc6\xc4\xc0\xcf \xc0\xd0\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1057,
		"\xc6\xc4\xc0\xcf \xc3\xa3\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1058,
		"\xc6\xc4\xc0\xcf \xc5\xa9\xb1\xe2\xb0\xa1 \xb3\xca\xb9\xab \xc5\xae\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1059,
		"32\xba\xf1\xc6\xae png \xc6\xc4\xc0\xcf\xb8\xb8 \xb5\xee\xb7\xcf\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9."));

	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1041,
		"\\c0xffff0000\\c\xbf\xb5\xb9\xae %d - %d \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb %d - %d\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1042,
		"\\c0xffff0000\\c\xbf\xb5\xb9\xae %d\xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb %d\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xcc\xbb\xf3 \xc0\xd4\xb7\xc2\xc7\xcf\xbf\xa9\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1043,
		"\\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1044,
		"\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb9\xae\xc0\xda\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1045,
		"\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb4\xdc\xbe\xee\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1046,
		"\xc0\xd4\xb7\xc2\xb5\xc8 \xb1\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1055,
		"\xb1\xe6\xb5\xe5 \xb8\xb6\xc5\xa9 \xb5\xee\xb7\xcf\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xed\xb0\xb4 \xc1\xf6\xbf\xf8 \xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."));

	m_guildMsgMap.insert(std::make_pair(GUILD_MSG_1060,
		"\xb3\xbb\xbf\xeb\xc0\xcc \xb9\xd9\xb2\xee\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9."));

	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_0,
		"\xc1\xa4\xc0\xc7\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xbf\xc0\xb7\xf9\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_1,
		"\xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54001,
		"\xb1\xe6\xb5\xe5 \xc1\xa4\xba\xb8 \xc3\xb3\xb8\xae \xbd\xc7\xc6\xd0\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54002,
		"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xb0\xe8\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54003,
		"\xc0\xcc\xb9\xcc \xb5\xbf\xc0\xcf\xc7\xd1 \xc0\xcc\xb8\xa7\xc0\xc7 \xb1\xe6\xb5\xe5\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54004,
		"\xb7\xb9\xba\xa7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54005,
		"\xc0\xcc\xb9\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xb5\xc7\xbe\xee \xc0\xd6\xb0\xc5\xb3\xaa \xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54006,
		"\xb1\xe6\xb5\xe5 \xb0\xb3\xbc\xb3/\xc6\xf3\xbc\xe2/\xc5\xbb\xc5\xf0 \xc8\xc4 24\xbd\xc3\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xaa\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54019,
		"\xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(\xb1\xe6\xb5\xe5 \xbb\xfd\xbc\xba \xb9\xf8\xc8\xa3)"));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54008,
		"\xb8\xed\xb7\xc9\xc0\xbb \xbd\xc7\xc7\xe0\xc7\xd2 \xb1\xc7\xc7\xd1\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54009,
		"\xbd\xc5\xc3\xbb\xc7\xd1 \xb1\xe6\xb5\xe5\xc0\xc7 \xc0\xce\xbf\xf8\xc0\xcc \xb8\xf0\xb5\xce \xc2\xf7\xbc\xad \xb4\xf5\xb4\xc2 \xb0\xa1\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54010,
		"\xbf\xe4\xc3\xbb\xc7\xd1 \xc0\xaf\xc0\xfa\xb0\xa1 \xb1\xe6\xb5\xe5\xbf\xf8\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54011,
		"\xba\xce\xb1\xe6\xb5\xe5\xc0\xe5\xc0\xcc \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54012,
		"\xba\xce\xb1\xe6\xb5\xe5\xc0\xe5 \xc1\xa6\xc7\xd1 \xc0\xce\xbf\xf8 \xc3\xca\xb0\xfa\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54013,
		"\xb1\xe6\xb5\xe5\xc0\xe5 \xc3\xdf\xb9\xe6"));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54014,
		"\xb3\xb2\xbe\xc6\xc0\xd6\xb4\xc2 \xb1\xe6\xb5\xe5\xbf\xf8\xc0\xcc \xc0\xd6\xb1\xe2 \xb6\xa7\xb9\xae\xbf\xa1 \xb1\xe6\xb5\xe5\xb8\xa6 \xc6\xf3\xbc\xe2\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54015,
		"\xb1\xe6\xb5\xe5\xb0\xa1 \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54016,
		"\xb1\xe6\xb5\xe5\xbf\xf8\xc0\xcc \xb3\xb2\xbe\xc6 \xc0\xd6\xb4\xc2 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad \xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54017,
		"\xc0\xcc\xb9\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54018,
		"\xb0\xa1\xc0\xd4 \xb4\xeb\xb1\xe2 \xc1\xdf\xc0\xce \xb1\xe6\xb5\xe5\xbf\xf8\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));

	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54019,
		"\xb0\xa1\xc0\xd4 \xbd\xc5\xc3\xbb \xbb\xf3\xc5\xc2\xb0\xa1 \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54020,
		"\xb0\xa1\xc0\xd4 \xb4\xeb\xb1\xe2\xc0\xda\xb0\xa1 \xbe\xc6\xb4\xcf\xb9\xc7\xb7\xce \xb1\xe6\xb5\xe5 \xb0\xa1\xc0\xd4 \xc3\xb6\xc8\xb8\xb8\xa6 \xbf\xe4\xc3\xbb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54021,
		"\xb0\xa1\xc0\xd4 \xb4\xeb\xb1\xe2\xc0\xda\xb4\xc2 \xb1\xe6\xb5\xe5\xb8\xa6 \xc5\xbb\xc5\xf0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));

	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54030,
		"\xb1\xe6\xb5\xe5 \xb4\xeb\xc0\xfc \xb0\xe1\xb0\xfa \xc0\xfa\xc0\xe5 \xbf\xc0\xb7\xf9"));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54500,
		"\xb5\xa5\xc0\xcc\xc5\xcd \xba\xa3\xc0\xcc\xbd\xba \xbf\xc0\xb7\xf9"));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54501,
		"\xbd\xc3\xbd\xba\xc5\xdb \xbd\xc3\xb0\xa3 \xba\xaf\xc8\xaf \xbd\xc7\xc6\xd0"));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54502,
		"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54503,
		"\xb1\xe6\xb5\xe5 \xbb\xfd\xbc\xba\xc0\xba \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xc0\xb8\xb3\xaa, \xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe8\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54504,
		"\xc7\xf6\xc0\xe7 \xb5\xee\xb1\xde\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb1\xe2\xb4\xc9\xc0\xd4\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54505,
		"\xb1\xe6\xb5\xe5\xc0\xe5\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54506,
		"\xba\xce\xb1\xe6\xb5\xe5\xc0\xe5\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54507,
		"\xb0\xfc\xb8\xae\xb1\xc7\xc7\xd1\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54508,
		"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54509,
		"\xb0\xa1\xc0\xd4 \xb4\xeb\xb1\xe2\xc0\xda\xb0\xa1 \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54510,
		"\xc0\xcc\xb9\xcc \xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54511,
		"\xc0\xaf\xc8\xbf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xb1\xe6\xb5\xe5\xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe7\xc1\xa2\xbc\xd3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."));
	m_guildMsgMap.insert(std::make_pair(GUILD_ERROR_54022,
		"\xb1\xe6\xb5\xe5 \xb8\xb6\xc5\xa9 \xb5\xee\xb7\xcf\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xed\xb0\xb4 \xc1\xf6\xbf\xf8 \xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."));

	ClearAllData();

	return true;
}

bool _guild::CGuildInfo::Release()
{
	ClearAllData();

	return true;
}

void _guild::CGuildInfo::ClearAllData()
{
	m_myGuildInfo.classIdx = (GUILD_CLASS_IDX)0;
	m_myGuildInfo.masterUID = -1;
	m_myGuildInfo.guildUID = 0;
	m_myGuildInfo.guildPang = 0;
	m_myGuildInfo.guildPoint = 0;
	m_myGuildInfo.memberCount = 0;
	memset(&m_myGuildInfo.createTime, 0, sizeof(m_myGuildInfo.createTime));
	memset(m_myGuildInfo.introduce, 0, sizeof(m_myGuildInfo.introduce));
	memset(m_myGuildInfo.guildMark, 0, sizeof(m_myGuildInfo.guildMark));
	memset(m_myGuildInfo.masterNickname, 0,
		sizeof(m_myGuildInfo.masterNickname));
	memset(m_myGuildInfo.guildName, 0, sizeof(m_myGuildInfo.guildName));
	memset(m_myGuildInfo.notice, 0, sizeof(m_myGuildInfo.notice));

	m_allMemberCount = 0;
	m_allGuildCount = 0;
	ClearMemberPage();
	ClearHistoryList();
}

void _guild::CGuildInfo::ClearHistoryList()
{
	if (!m_historyList.empty())
		m_historyList.clear();
}

void _guild::CGuildInfo::ClearMemberPage()
{
	MEMBER_MAP_IT it = m_memberPageMap.begin();
	for (; it != m_memberPageMap.end(); ++it)
		(*it).second.clear();
}

void _guild::CGuildInfo::ClearMemberList(int page)
{
	MEMBER_MAP_IT it = m_memberPageMap.find(page);
	if (it != m_memberPageMap.end())
	{
		std::list<GUILD_USER_LIST>& members = (*it).second;
		members.clear();
	}
}

void _guild::sExchangerQuota::operator()(GUILD_USER_LIST& info)
{
	ExchangeQuotation(info.message);
}

void _guild::CGuildInfo::SetMemberPage(int page,
	std::list<GUILD_USER_LIST>* pList)
{
	std::for_each(pList->begin(), pList->end(), sExchangerQuota());

	MEMBER_MAP_IT it = m_memberPageMap.find(page);
	if (it != m_memberPageMap.end())
	{
		(*it).second.swap(*pList);
	}
	else
	{
		std::list<GUILD_USER_LIST> members;
		m_memberPageMap.insert(std::make_pair(page, members));
		m_memberPageMap[page].swap(*pList);
	}
}

void _guild::CGuildInfo::ClearSearchPage()
{
	SEARCH_MAP_IT it = m_searchPageMap.begin();
	for (; it != m_searchPageMap.end(); ++it)
		(*it).second.clear();
}

void _guild::CGuildInfo::ClearSearchList(int page)
{
	SEARCH_MAP_IT it = m_searchPageMap.find(page);
	if (it != m_searchPageMap.end())
	{
		std::list<GUILD_LIST>& guilds = (*it).second;
		guilds.clear();
	}
}

void _guild::CGuildInfo::SetMyGuildInfo(GUILD_INFO& info)
{
	m_myGuildInfo = info;

	ExchangeQuotation(m_myGuildInfo.introduce);
	ExchangeQuotation(m_myGuildInfo.notice);
}

void _guild::CGuildInfo::SetMyGuildUserInfo(GUILD_USER_INFO& info)
{
	static_cast<GUILD_USER_INFO&>(m_myGuildInfo) = info;

	ExchangeQuotation(m_myGuildInfo.introduce);
	ExchangeQuotation(m_myGuildInfo.notice);
}

void _guild::CGuildInfo::SetHistoryList(std::list<GUILD_HISTORY>* pList)
{
	ClearHistoryList();

	std::list<GUILD_HISTORY>::iterator it = pList->begin();
	for (; it != pList->end(); ++it)
		m_historyList.push_back(*it);
}

void _guild::CGuildInfo::SetSearchList(int page, std::list<GUILD_LIST>* pList)
{
	SEARCH_MAP_IT it = m_searchPageMap.find(page);
	if (it != m_searchPageMap.end())
	{
		(*it).second.swap(*pList);
	}
	else
	{
		std::list<GUILD_LIST> guilds;
		m_searchPageMap.insert(std::make_pair(page, guilds));
		m_searchPageMap[page].swap(*pList);
	}
}

void _guild::CGuildInfo::SetGuildEmblem(GUILD_USER_INFO* pInfo)
{
	strncpy(m_myGuildInfo.guildMark, pInfo->guildMark,
		sizeof(m_myGuildInfo.guildMark) - 1);
}

void _guild::CGuildInfo::SetGuildName(GUILD_INFO* pInfo)
{
	strncpy(m_myGuildInfo.guildName, pInfo->guildName,
		sizeof(m_myGuildInfo.guildName) - 1);
}

void _guild::CGuildInfo::SetGuildIntroduce(const std::string& introduce)
{
	strcpy(m_myGuildInfo.introduce, introduce.c_str());
}

void _guild::CGuildInfo::SetGuildNotice(const std::string& notice)
{
	strcpy(m_myGuildInfo.notice, notice.c_str());
}

const GUILD_INFO& _guild::CGuildInfo::GetMyGuildInfo() const
{
	return m_myGuildInfo;
}

const char* _guild::CGuildInfo::GuildMsg(int index)
{
	MESSAGE_MAP_IT it = m_guildMsgMap.find(index);
	if (it == m_guildMsgMap.end())
		return NULL;

	return (*it).second;
}

bool _guild::CGuildInfo::IsGuildMember() const
{
	switch (m_myGuildInfo.classIdx)
	{
	case 1:
	case 2:
	case 3:
		return true;
	default:
		return false;
	}
}

bool _guild::CGuildInfo::IsGuildMember(GUILD_CLASS_IDX classIdx)
{
	switch (classIdx)
	{
	case 1:
	case 2:
	case 3:
		return true;
	default:
		return false;
	}
}

bool _guild::CGuildInfo::IsManager() const
{
	switch (m_myGuildInfo.classIdx)
	{
	case 1:
	case 2:
		return true;
	default:
		return false;
	}
}

bool _guild::CGuildInfo::IsManager(GUILD_CLASS_IDX classIdx)
{
	switch (classIdx)
	{
	case 1:
	case 2:
		return true;
	default:
		return false;
	}
}

int _guild::CGuildInfo::GetMaxMemberPage()
{
	if (GetAllMemberCount() == 0)
		return 1;

	int maxPage = GetAllMemberCount() / 15;
	if (GetAllMemberCount() % 15 != 0)
		maxPage += 1;

	return maxPage;
}

int _guild::CGuildInfo::GetMaxSearchPage()
{
	if (GetAllGuildCount() == 0)
		return 1;

	int maxPage = GetAllGuildCount() / 15;
	if (GetAllGuildCount() % 15 != 0)
		maxPage += 1;

	return maxPage;
}

const std::list<GUILD_USER_LIST>* _guild::CGuildInfo::GetMemberList(int page)
{
	MEMBER_MAP_IT it = m_memberPageMap.find(page);
	if (it == m_memberPageMap.end())
		return NULL;

	return &(*it).second;
}

const std::list<GUILD_LIST>* _guild::CGuildInfo::GetSearchList(int page)
{
	SEARCH_MAP_IT it = m_searchPageMap.find(page);
	if (it == m_searchPageMap.end())
		return NULL;

	return &(*it).second;
}

void CChristmasSockEvent::SetFlag(bool bFlag)
{
	m_bFlag = bFlag;
}

bool CChristmasSockEvent::GetFlag() const
{
	return m_bFlag;
}

bool CChristmasSockEvent::Initialize()
{
	m_bFlag = true;
	return true;
}

bool CChristmasSockEvent::Release()
{
	return Initialize();
}

bool CVectorSlideTestContainer::Initialize()
{
	m_bNewTypeVectorSlide = true;
	m_limitSpeed = 80;
	m_boundHeight = 25;
	m_limitVectorSlideCount = 35;

	return true;
}

bool CVectorSlideTestContainer::Release()
{
	return Initialize();
}

bool CGimmickContainer::Initialize()
{
	m_randomSeed = rand();
	m_minDistBtwItems = 40.0f;

	for (int hole = 0; hole < 18; hole++)
	{
		std::vector<GimmickDispositionInformation> positions;
		m_gimmickPositionMap[hole] = positions;
	}

	m_carpetSeqName = "wizcity_magic_carpet_bounding.seq";
	m_crashSeqName[GIMMICK_ITEM_PANG] = "wizcity_field_item_pangcoin_crash.seq";
	m_crashSeqName[GIMMICK_ITEM_BOX] = "wizcity_field_item_randombox_crash.seq";

	return true;
}

bool CGimmickContainer::Release()
{
	return Initialize();
}

void CGimmickContainer::AddGimmickPosition(unsigned char hole,
	const std::vector<GimmickDispositionInformation>& positions)
{
	m_gimmickPositionMap[hole] = positions;
}

std::vector<GimmickDispositionInformation>& CGimmickContainer::
	GetGimmickPosition(unsigned char hole)
{
	return m_gimmickPositionMap[hole];
}

void CGimmickContainer::ResetGimmikFlag(unsigned char hole,
	GimmickDispositionInformation::eFlag flag)
{
	std::vector<GimmickDispositionInformation>& positions =
		GetGimmickPosition(hole);

	for (unsigned int i = 0; i < positions.size(); i++)
		positions[i].flag = flag;
}

GimmickDispositionInformation* CGimmickContainer::FindDisposeInfo(
	unsigned char hole, GIMMICK_ITEM_TYPE type,
	GimmickDispositionInformation::eFlag flag)
{
	std::vector<GimmickDispositionInformation>& positions =
		GetGimmickPosition(hole);

	for (unsigned int i = 0; i < positions.size(); i++)
	{
		bool bType = false;
		bool bFlag = false;

		if (type == GIMMICK_ITEM_ANY)
			bType = true;
		else if (positions[i].type == type)
			bType = true;

		if (flag == GimmickDispositionInformation::GIMMICK_ANY)
			bFlag = true;
		else if (positions[i].flag == flag)
			bFlag = true;

		if (bType && bFlag)
			return &positions[i];
	}

	return NULL;
}

int CGimmickContainer::GetNumDisposeInfo(unsigned char hole,
	GIMMICK_ITEM_TYPE type, GimmickDispositionInformation::eFlag flag)
{
	std::vector<GimmickDispositionInformation>& positions =
		GetGimmickPosition(hole);

	int count = 0;
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		bool bType = false;
		bool bFlag = false;

		if (type == GIMMICK_ITEM_ANY)
			bType = true;
		else if (positions[i].type == type)
			bType = true;

		if (flag == GimmickDispositionInformation::GIMMICK_ANY)
			bFlag = true;
		else if (positions[i].flag == flag)
			bFlag = true;

		if (bType && bFlag)
			count++;
	}

	return count;
}

void CGimmickContainer::LoadFromPacket(WReceivedPacket& packet)
{
	Release();

	m_randomSeed = packet.Decode4();

	for (int hole = 0; hole < 18; hole++)
	{
		int count = packet.Decode1();

		for (int i = 0; i < count; i++)
		{
			GimmickDispositionInformation info;
			packet.DecodeBuffer(&info, sizeof(info));
			std::vector<GimmickDispositionInformation>& positions =
				m_gimmickPositionMap[info.hole];
			positions.push_back(info);
		}
	}
}

bool CGimmickContainer::LoadFromScript(const char* filename)
{
	Release();

	lua_system::lua_table_helper table =
		lua_system::lua_script_manger::GetInstance()->GetTable(filename);

	for (int hole = 1; hole <= 18; hole++)
	{
		std::vector<GimmickDispositionInformation> positions;

		int boxCount = table.GetValueByName<int>("gimmick_randombox_count");
		int pangCount = table.GetValueByName<int>("gimmick_pang_count");

		GimmickDispositionInformation info;

		for (int i = 0; i < boxCount; i++)
		{
			info.index = i;
			info.type = GIMMICK_ITEM_BOX;
			positions.push_back(info);
		}

		for (int i = 0; i < pangCount; i++)
		{
			int index = boxCount + i;

			info.type = GIMMICK_ITEM_PANG;
			info.index = index;
			positions.push_back(info);
		}

		AddGimmickPosition(hole, positions);
	}

	m_randomSeed = table.GetValueByName<int>("gimmick_randomseed");
	m_minDistBtwItems =
		(float)table.GetValueByName<int>("coin_object_placement_gap");

	m_carpetSeqName =
		table.GetValueByName<const char*>("FxForBoundOnMagicCarpet");
	m_crashSeqName[GIMMICK_ITEM_PANG] =
		table.GetValueByName<const char*>("FxForCrashFieldPang");
	m_crashSeqName[GIMMICK_ITEM_BOX] =
		table.GetValueByName<const char*>("FxForCrashFieldRandomBox");

	return true;
}

bool CGimmickContainer::NeedToMakeTriPtArray(const char* texName,
	FieldItem::eDisposeTextureType* pType) const
{
	if (strstr(texName, "ct_green_b") != NULL)
	{
		if (pType != NULL)
			*pType = FieldItem::DISPOSE_TEXTURE_COIN;
		return true;
	}

	if (strcmp(texName, "ct_booster.dds") == 0)
	{
		if (pType != NULL)
			*pType = FieldItem::DISPOSE_TEXTURE_BOOSTER;
		return true;
	}

	return false;
}

void CGimmickContainer::AddItemAcquired(const FieldItem::AcquiredItemInfo& info)
{
	m_acquiredItemInfoArray.push_back(info);
}
