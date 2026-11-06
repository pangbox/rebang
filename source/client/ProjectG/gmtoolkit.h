#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

struct sPrize;
class WSendPacket;

enum eGMToolResult
{
	GM_RESULT_SYNTAX_ERROR,
	GM_RESULT_ARGUMENT_ERROR,
	GM_RESULT_OK,
};

enum eGMToolkitStatus
{
	GM_TOOLKIT_NORMAL,
	GM_TOOLKIT_CONFIRM_WAIT,
};

enum eGMStatusFlag
{
	GM_STATUS_VISIBLE = 1,
	GM_STATUS_WHISPER = 2,
	GM_STATUS_CHANNEL = 4,
};

enum eGMStatusSwitch
{
	GM_SWITCH_OFF,
	GM_SWITCH_ON,
};

enum eGMCommandTag
{
	GM_CMD_DUMMY,
	GM_CMD_HELP,
	GM_CMD_COMMAND,
	GM_CMD_VISIBLE,
	GM_CMD_WHISPER,
	GM_CMD_CHANNEL,
	GM_CMD_STATUS,
	GM_CMD_LIST,
	GM_CMD_OPEN,
	GM_CMD_CLOSE,
	GM_CMD_KICK,
	GM_CMD_DISCONNECT,
	GM_CMD_DISCON_UID,
	GM_CMD_DESTROY,
	GM_CMD_WIND,
	GM_CMD_WEATHER,
	GM_CMD_IDENTITY,
	GM_CMD_NOTICE,
	GM_CMD_GIVEITEM,
	GM_CMD_GOLDENBELL,
	GM_CMD_SETPRIZE,
	GM_CMD_UNSETPRIZE,
	GM_CMD_SHOWPRIZE,
	GM_CMD_LOADSCRIPT,
	GM_CMD_NOTICEPRIZE = 25,
	GM_CMD_GETTID,
	GM_CMD_SETMISSION,
	GM_CMD_FINDITEM,
	GM_CMD_CATEGORY,
	GM_CMD_MATCHMAP,
	GM_CMD_MATCHHOLE,
};

class CGMToolkit : public WSingleton<CGMToolkit>
{
public:
	CGMToolkit();
	virtual ~CGMToolkit();

	bool Command(const char* command);
	void Display(const char* text, unsigned long color);
	bool IsAdministrator() const;
	bool IsGM() const;
	bool GetStatus(eGMStatusFlag flag);
	void SetStatus(eGMStatusFlag flag, eGMStatusSwitch sw);
	void SendStatus(eGMCommandTag tag);
	void SendStringBuffer(eGMCommandTag tag, const char* str);
	void SendCharacterBuffer(eGMCommandTag tag, const char* str);
	void SendPrize(FrListBox* pList);

	bool GetPrizeSwitch() const { return m_bPrizeSwitch; }

protected:
	static const int MAX_HOLE = 18;

	eGMToolResult Parser();
	bool PhraseSaperator(const char* command);
	void ReleaseStringBuffer();
	int GetGMCommandTableNumber();
	bool Confirm();
	bool FindKeywordInString(const char* const str, const char* const keyword);
	eGMCommandTag GetConfirmWaitTag() const { return m_confirmWaitTag; }
	eGMToolkitStatus GetToolKitStatus() const { return m_toolkitStatus; }
	void SetConfirmWaitTag(eGMCommandTag tag)
	{
		m_confirmWaitTag = tag;
		SetToolKitStatus(GM_TOOLKIT_CONFIRM_WAIT);
	}
	void SetToolKitStatus(eGMToolkitStatus status) { m_toolkitStatus = status; }
	bool FindItem(unsigned long typeId, char* name) const;
	template <class T, class M>
	bool FindItem(const char* const keyword, M& table)
	{
		bool bFind = false;
		char buffer[256] = "\0";
		for (typename M::iterator it = table.begin(); it != table.end(); ++it)
		{
			if (FindKeywordInString((*it).second.c.Name, keyword))
			{
				memset(buffer, 0, sizeof(buffer));
				bFind = true;
				sprintf(buffer, "%12d : %s", (*it).second.c.TypeId,
					(*it).second.c.Name);
				Display(buffer, 0xc6ff85);
			}
		}

		return bFind;
	}

	eGMToolResult CommandHelp(int index);
	eGMToolResult CommandCommand(int index);
	eGMToolResult CommandVisible(eGMToolkitStatus status);
	eGMToolResult CommandWhisper(eGMToolkitStatus status);
	eGMToolResult CommandChannel(eGMToolkitStatus status);
	eGMToolResult CommandShowStatus(eGMToolkitStatus status);
	eGMToolResult CommandList(eGMToolkitStatus status);
	eGMToolResult CommandOpen(eGMToolkitStatus status);
	eGMToolResult CommandClose(eGMToolkitStatus status);
	eGMToolResult CommandKickUser(eGMToolkitStatus status);
	eGMToolResult CommandDisconnect(eGMToolkitStatus status);
	eGMToolResult CommandDiscon_UID(eGMToolkitStatus status);
	eGMToolResult CommandDestroy(eGMToolkitStatus status);
	eGMToolResult CommandWind(eGMToolkitStatus status);
	eGMToolResult CommandWeather(eGMToolkitStatus status);
	eGMToolResult CommandIdentity(eGMToolkitStatus status);
	eGMToolResult CommandNotice(eGMToolkitStatus status);
	eGMToolResult CommandGiveItem(eGMToolkitStatus status);
	eGMToolResult CommandGoldenBell(eGMToolkitStatus status);
	eGMToolResult CommandSetPrize(eGMToolkitStatus status);
	eGMToolResult CommandUnsetPrize(eGMToolkitStatus status);
	eGMToolResult CommandShowPrize(eGMToolkitStatus status);
	eGMToolResult CommandLoadScript(eGMToolkitStatus status);
	eGMToolResult CommandNoticePrize(eGMToolkitStatus status);
	eGMToolResult CommandGetTID(eGMToolkitStatus status);
	eGMToolResult CommandSetMission(eGMToolkitStatus status);
	eGMToolResult CommandFindItem(eGMToolkitStatus status);
	eGMToolResult CommandCategory(eGMToolkitStatus status);
	eGMToolResult CommandMatchMapSetting(eGMToolkitStatus status);
	eGMToolResult CommandMatchHoleSetting(eGMToolkitStatus status);

	eGMToolkitStatus m_toolkitStatus;
	eGMCommandTag m_confirmWaitTag;
	WSendPacket* m_pConfirmPacket;
	bool m_bGM;
	unsigned short m_status;
	char** m_stringBuffer;
	int m_stringCount;
	std::vector<std::string> m_openList;
	char m_unknown50[18];
	char m_unknown62[36];
	std::list<sPrize*> m_prizeList;
	sPrize* m_pPrize;
	bool m_bPrizeSwitch;
};
