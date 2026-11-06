#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>

#include "gmtoolkit.h"
#include "frlistbox.h"
#include "chatmsg.h"
#include "commonutil.h"
#include "packet.h"
#include "shareddoc.h"
#include "uccmanager.h"

struct sGMCommand
{
	bool enabled;
	eGMCommandTag tag;
	const char* keyword;
	const char* help;
};

struct sPrize
{
	int type;
	int rankFrom;
	int rankTo;
	unsigned long typeId;
	int count;
};

// clang-format off
static sGMCommand s_commandList[] = {
	{ false, GM_CMD_DUMMY, "dummy", "/dummy :: for Ntreev Soft." },
	{ true, GM_CMD_HELP, "help",
		K2L_Compatibility("/help [cmd] :: \xc1\xf6\xc1\xa4\xc7\xd1 \xb8\xed\xb7\xc9\xc0\xc7 \xbf\xeb\xb5\xb5 \xbc\xb3\xb8\xed.") },
	{ true, GM_CMD_COMMAND, "command",
		K2L_Compatibility("/command :: \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xb8\xae\xbd\xba\xc6\xae \xc3\xe2\xb7\xc2.") },
	{ true, GM_CMD_STATUS, "status",
		K2L_Compatibility("/status :: \xc7\xf6\xc0\xe7 \xbc\xb3\xc1\xa4\xb5\xc8 \xbb\xf3\xc5\xc2\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_VISIBLE, "visible",
		K2L_Compatibility("/visible [on/off] :: \xb7\xce\xba\xf1\xbf\xa1\xbc\xad \xc0\xaf\xc0\xfa\xb0\xa1 \xc0\xda\xbd\xc5\xc0\xbb \xba\xbc \xbc\xf6 \xc0\xd6\xb5\xb5\xb7\xcf \xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_WHISPER, "whisper",
		K2L_Compatibility("/whisper [on/off] :: \xb8\xf0\xb5\xe7 \xc0\xaf\xc0\xfa\xc0\xc7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_CHANNEL, "channel",
		K2L_Compatibility("/channel [on/off] :: \xc7\xe3\xbf\xeb\xb5\xc8 \xc0\xaf\xc0\xfa\xbf\xcd \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xb0\xa1\xb4\xc9\xc7\xcf\xb0\xd4 \xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_LIST, "list",
		K2L_Compatibility("/list :: \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xcc \xc7\xe3\xbf\xeb\xb5\xc8 \xc0\xaf\xc0\xfa \xb8\xae\xbd\xba\xc6\xae\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_OPEN, "open",
		K2L_Compatibility("/open [nick] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xc0\xaf\xc0\xfa\xc0\xc7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_CLOSE, "close",
		K2L_Compatibility("/close [nick] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xc0\xaf\xc0\xfa\xb8\xa6 \xb1\xd3\xbc\xd3\xb8\xbb \xc7\xe3\xbf\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xc1\xa6\xb0\xc5\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_KICK, "kick",
		K2L_Compatibility("/kick [nick][op] :: \xb4\xeb\xbb\xf3\xc0\xbb \xc5\xb1\xc7\xd5\xb4\xcf\xb4\xd9 '-p' \xbf\xc9\xbc\xc7\xc0\xbb \xc1\xd6\xb8\xe9 \xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xbf\xc3\xb6\xf3\xb0\xa9\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_DISCONNECT, "disconnect",
		K2L_Compatibility("/disconnect [nick] :: \xb4\xeb\xbb\xf3\xc0\xbb \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_DISCON_UID, "discon_uid",
		K2L_Compatibility("/discon_uid [uid] :: \xb4\xeb\xbb\xf3\xc0\xbb \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_DESTROY, "destroy",
		K2L_Compatibility("/destroy [num] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xb9\xf8\xc8\xa3\xc0\xc7 \xb9\xe6\xc0\xbb \xc6\xf8\xc6\xc4\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_WIND, "wind",
		K2L_Compatibility("/wind [spd][dir] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xbc\xd3\xb5\xb5\xbf\xcd \xb9\xe6\xc7\xe2\xc0\xb8\xb7\xce \xb9\xd9\xb6\xf7\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_WEATHER, "weather",
		K2L_Compatibility("/weather [type] :: \xb3\xaf\xbe\xbe\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 'fine', 'rain', 'snow', 'cloud'") },
	{ true, GM_CMD_IDENTITY, "identity",
		K2L_Compatibility("/identity [type] :: \xb0\xe8\xc1\xa4 \xb1\xc7\xc7\xd1\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 'admin', 'user'") },
	{ true, GM_CMD_NOTICE, "notice",
		K2L_Compatibility("/notice [msg] :: \xb8\xde\xbc\xbc\xc1\xf6\xb8\xa6 \xc3\xa4\xc6\xc3\xc3\xa2\xbf\xa1 \xb0\xf8\xc1\xf6\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_GIVEITEM, "giveitem",
		K2L_Compatibility("/giveitem [nick][typeid][num] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xbb\xe7\xb6\xf7\xbf\xa1\xb0\xd4 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xb9\xb0.") },
	{ true, GM_CMD_GOLDENBELL, "goldenbell",
		K2L_Compatibility("/goldenbell [typeid][num] :: \xc7\xf6\xc0\xe7 \xb7\xeb\xbe\xc8\xc0\xc7 \xb8\xf0\xb5\xe7 \xbb\xe7\xb6\xf7\xbf\xa1\xb0\xd4 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xb9\xb0.") },
	{ true, GM_CMD_SETPRIZE, "setprize",
		"/setprize [switch];[dest][typeid][num]" },
	{ true, GM_CMD_UNSETPRIZE, "unsetprize",
		K2L_Compatibility("/unsetprize :: setprize\xb7\xce \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xbb\xe8\xc1\xa6 \xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_SHOWPRIZE, "showprize",
		K2L_Compatibility("/showprize :: setprize\xb7\xce \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xc3\xe2\xb7\xc2 \xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ false, GM_CMD_LOADSCRIPT, "loadscript",
		K2L_Compatibility("/loadscript [file] :: \xc6\xc4\xc0\xcf\xbf\xa1 \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xb0\xd4\xc0\xd3\xbf\xa1 \xc0\xfb\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_NOTICEPRIZE, "noticeprize",
		K2L_Compatibility("/noticeprize :: \xbb\xf3\xc7\xb0\xc1\xf6\xb1\xde \xc1\xb6\xb0\xc7 \xb0\xf8\xc1\xf6.") },
	{ false, GM_CMD_GETTID, "gettid",
		K2L_Compatibility("/gettid ITEM_NAME :: \xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 TID\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_SETMISSION, "setmission",
		K2L_Compatibility("/setmission MISSION_NUM :: \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1 \xb9\xcc\xbc\xc7\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_FINDITEM, "finditem",
		K2L_Compatibility("/finditem [category][keyword] :: \xc7\xd8\xb4\xe7\xc7\xcf\xb4\xc2 \xb9\xae\xc0\xda\xbf\xad\xc0\xcc \xb5\xe9\xbe\xee\xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb0\xcb\xbb\xf6.") },
	{ true, GM_CMD_CATEGORY, "category",
		K2L_Compatibility("/category :: finditem \xb8\xed\xb7\xc9\xc0\xbb \xc0\xa7\xc7\xd1 category \xb8\xf1\xb7\xcf \xc3\xe2\xb7\xc2") },
	{ true, GM_CMD_MATCHMAP, "matchmap",
		K2L_Compatibility("/matchmap [mapcount] :: \xb4\xeb\xc8\xb8\xbf\xeb \xb8\xca\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. \xb1\xe2\xba\xbb\xbc\xb3\xc1\xa4\xc0\xba \xba\xed\xb7\xe7 \xb6\xf3\xb1\xba\xc0\xd4\xb4\xcf\xb4\xd9.") },
	{ true, GM_CMD_MATCHHOLE, "matchhole",
		K2L_Compatibility("/matchhole [playhole] :: \xb4\xeb\xc8\xb8\xbf\xeb \xb4\xeb\xc0\xfc\xc8\xa6\xbc\xf6(count)\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 3, 6, 9, 18\xc8\xa6 \xc1\xdf\xbf\xa1 \xbc\xb1\xc5\xc3\xc7\xcf\xbc\xbc\xbf\xe4.") }
};
// clang-format on

CGMToolkit::CGMToolkit(void)
{
	m_bGM = false;
	m_status = 0;
	m_stringBuffer = 0;
	m_stringCount = 0;
	m_toolkitStatus = GM_TOOLKIT_NORMAL;
	m_confirmWaitTag = (eGMCommandTag)0;
	m_pConfirmPacket = 0;

	memset(m_unknown50, 0, sizeof(m_unknown50));
	memset(m_unknown62, 0, sizeof(m_unknown62));

	m_prizeList.clear();

	m_pPrize = 0;
	m_bPrizeSwitch = false;
}

CGMToolkit::~CGMToolkit(void)
{
	ReleaseStringBuffer();

	if (m_pConfirmPacket)
	{
		delete m_pConfirmPacket;
		m_pConfirmPacket = 0;
	}

	if (m_pPrize)
	{
		delete m_pPrize;
		m_pPrize = 0;
	}

	for (std::list<sPrize*>::iterator it = m_prizeList.begin();
		it != m_prizeList.end(); ++it)
	{
		if (*it)
		{
			delete *it;
			*it = 0;
		}
	}

	m_prizeList.clear();
}

bool CGMToolkit::GetStatus(eGMStatusFlag flag)
{
	return (m_status & flag) ? true : false;
}

void CGMToolkit::SetStatus(eGMStatusFlag flag, eGMStatusSwitch statusSwitch)
{
	if (statusSwitch == GM_SWITCH_ON)
	{
		m_status |= flag;
	}
	else if (statusSwitch == GM_SWITCH_OFF)
	{
		m_status &= ~flag;
	}
}

void CGMToolkit::ReleaseStringBuffer(void)
{
	for (int i = 0; i < m_stringCount; i++)
	{
		if (m_stringBuffer[i])
		{
			delete[] m_stringBuffer[i];
			m_stringBuffer[i] = 0;
		}
	}

	if (m_stringBuffer)
	{
		delete[] m_stringBuffer;
		m_stringBuffer = 0;
	}

	m_stringCount = 0;
}

bool CGMToolkit::PhraseSaperator(const char* command)
{
	bool result = false;
	ReleaseStringBuffer();

	int length = strlen(command);

	if (command != 0)
	{
		if (command[0] == '/')
		{
			for (int i = 0; command[i] != '\0'; i++)
			{
				if (command[i] == command[0])
				{
					if (i != 0)
					{
						m_stringCount = 0;
						break;
					}
				}
				else
				{
					m_stringCount = 1;
				}
			}
		}

		if (m_stringCount > 0)
		{
			for (int i = 0; command[i] != '\0'; i++)
			{
				if (command[i] == ' ' && i < length)
				{
					if (command[i + 1] != ' ' && command[i + 1] != '\0')
					{
						m_stringCount++;
					}
				}
			}

			m_stringBuffer = new char*[m_stringCount];
			for (int i = 0; i < m_stringCount; i++)
			{
				m_stringBuffer[i] = 0;
			}

			char word[128] = "\0";
			int remaining = m_stringCount;
			bool inWord = false;
			int position = 0;

			for (int i = 1; command[i] != '\0'; i++)
			{
				if (command[i] != ' ' && command[i] != '\0')
				{
					word[position] = command[i];
					position++;
					inWord = true;
				}

				if ((command[i] == ' ' || command[i + 1] == '\0') &&
					inWord == true)
				{
					if (remaining > 0)
					{
						int wordLength = strlen(word) + 1;

						m_stringBuffer[m_stringCount - remaining] =
							new char[wordLength];
						memset(m_stringBuffer[m_stringCount - remaining], 0,
							wordLength);

						strcpy(m_stringBuffer[m_stringCount - remaining], word);

						memset(word, 0, 64);

						remaining--;
						position = 0;
						inWord = false;
					}
				}
			}

			result = true;
		}
	}

	return result;
}

int CGMToolkit::GetGMCommandTableNumber(void)
{
	for (int i = 0; i < 31; i++)
	{
		if (strcmp(m_stringBuffer[0], s_commandList[i].keyword) == 0)
		{
			return i;
		}
	}

	return 0;
}

eGMToolResult CGMToolkit::Parser(void)
{
	eGMToolkitStatus status = GetToolKitStatus();
	eGMCommandTag tag = GM_CMD_DUMMY;
	int count = 0;
	eGMToolResult result = GM_RESULT_OK;

	if (status == GM_TOOLKIT_NORMAL)
	{
		count = 31;
		int index = GetGMCommandTableNumber();
		tag = s_commandList[index].tag;

		if (s_commandList[index].enabled == false && tag != GM_CMD_DUMMY)
		{
			Display(
				"\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFF7802);
			return result;
		}
	}
	else if (status == GM_TOOLKIT_CONFIRM_WAIT)
	{
		tag = GetConfirmWaitTag();
	}

	switch (tag)
	{
	case GM_CMD_DUMMY:
		result = GM_RESULT_SYNTAX_ERROR;
		break;
	case GM_CMD_HELP:
		result = CommandHelp(count);
		break;
	case GM_CMD_COMMAND:
		result = CommandCommand(count);
		break;
	case GM_CMD_VISIBLE:
		result = CommandVisible(status);
		break;
	case GM_CMD_WHISPER:
		result = CommandWhisper(status);
		break;
	case GM_CMD_CHANNEL:
		result = CommandChannel(status);
		break;
	case GM_CMD_LIST:
		result = CommandList(status);
		break;
	case GM_CMD_OPEN:
		result = CommandOpen(status);
		break;
	case GM_CMD_CLOSE:
		result = CommandClose(status);
		break;
	case GM_CMD_KICK:
		result = CommandKickUser(status);
		break;
	case GM_CMD_DISCONNECT:
		result = CommandDisconnect(status);
		break;
	case GM_CMD_DISCON_UID:
		result = CommandDiscon_UID(status);
		break;
	case GM_CMD_STATUS:
		result = CommandShowStatus(status);
		break;
	case GM_CMD_DESTROY:
		result = CommandDestroy(status);
		break;
	case GM_CMD_WIND:
		result = CommandWind(status);
		break;
	case GM_CMD_WEATHER:
		result = CommandWeather(status);
		break;
	case GM_CMD_IDENTITY:
		result = CommandIdentity(status);
		break;
	case GM_CMD_NOTICE:
		result = CommandNotice(status);
		break;
	case GM_CMD_GIVEITEM:
		result = CommandGiveItem(status);
		break;
	case GM_CMD_GOLDENBELL:
		result = CommandGoldenBell(status);
		break;
	case GM_CMD_SETPRIZE:
		result = CommandSetPrize(status);
		break;
	case GM_CMD_UNSETPRIZE:
		result = CommandUnsetPrize(status);
		break;
	case GM_CMD_SHOWPRIZE:
		result = CommandShowPrize(status);
		break;
	case GM_CMD_NOTICEPRIZE:
		result = CommandNoticePrize(status);
		break;
	case GM_CMD_GETTID:
		result = CommandGetTID(status);
		break;
	case GM_CMD_SETMISSION:
		result = CommandSetMission(status);
		break;
	case GM_CMD_FINDITEM:
		result = CommandFindItem(status);
		break;
	case GM_CMD_CATEGORY:
		result = CommandCategory(status);
		break;
	case GM_CMD_MATCHMAP:
		result = CommandMatchMapSetting(status);
		break;
	case GM_CMD_MATCHHOLE:
		result = CommandMatchHoleSetting(status);
		break;
	}

	return result;
}

bool CGMToolkit::IsAdministrator(void) const
{
	return (Doc()->m_myInfo.info.dwIdentity & 4) ? true : false;
}

bool CGMToolkit::IsGM(void) const
{
	return m_bGM;
}

bool CGMToolkit::Command(const char* command)
{
	bool result = false;

	if (PhraseSaperator(command))
	{
		Display(MakeStr("GM_Command:%s", command), 0x6EECFF);

		switch (Parser())
		{
		case GM_RESULT_SYNTAX_ERROR:
			result = false;
			Display("Syntax error.", 0xFF7802);
			break;
		case GM_RESULT_ARGUMENT_ERROR:
			result = false;
			Display("Argument error.", 0xFF7802);
			break;
		case GM_RESULT_OK:
			result = true;
			break;
		}
	}

	return result;
}

void CGMToolkit::Display(const char* message, unsigned long color)
{
	WSingleton<CChatMsg>::Instance()->AddChatMsg(message, color, true, false);
}

void CGMToolkit::SendStatus(eGMCommandTag tag)
{
	WSendPacket packet((enumClientPacket)0x8c);
	packet.Encode2(tag);
	packet.Encode2(m_status);
	packet.Send(TO_GAME);
}

void CGMToolkit::SendStringBuffer(eGMCommandTag tag, const char* buffer)
{
	std::string str(buffer);

	WSendPacket packet((enumClientPacket)0x8c);
	packet.Encode2(tag);
	packet.EncodeStr(str);
	packet.Send(TO_GAME);
}

void CGMToolkit::SendCharacterBuffer(eGMCommandTag tag, const char* buffer)
{
	int length = strlen(buffer) + 1;

	WSendPacket packet((enumClientPacket)0x8c);
	packet.Encode2(tag);
	packet.EncodeBuffer(buffer, length);
	packet.Send(TO_GAME);
}

void CGMToolkit::SendPrize(FrListBox* listBox)
{
	int count = listBox->GetCurrentItemSize(true);

	for (std::list<sPrize*>::iterator it = m_prizeList.begin();
		it != m_prizeList.end(); ++it)
	{
		sPrize* prize = *it;

		unsigned long lastRank = prize->rankTo;
		unsigned long firstRank = prize->rankFrom;

		if (prize->type == 0)
		{
			for (unsigned long rank = firstRank; rank <= lastRank; rank++)
			{
				if ((int)rank > count)
				{
					break;
				}

				FrListItem* item = listBox->GetItem(rank - 1);

				if (item == 0)
				{
					break;
				}

				sRivalData* player = (sRivalData*)item->pData;

				if (player->state == 3)
				{
					continue;
				}

				WSendPacket packet((enumClientPacket)0x8c);
				packet.Encode2(GM_CMD_SETPRIZE);
				packet.Encode4(player->oid);
				packet.Encode4(prize->typeId);
				packet.Encode4(prize->count);
				packet.Encode1(prize->type);
				packet.Encode1(rank);
				packet.Send(TO_GAME);
			}
		}
		else
		{
			for (int i = 0; i < count; i++)
			{
				FrListItem* item = listBox->GetItem(i);

				if (item == 0)
				{
					break;
				}

				sRivalData* player = (sRivalData*)item->pData;
				unsigned char award = 0;

				switch (prize->type)
				{
				case 1:
					if (Doc()->m_awardItem[1].uid == player->oid)
					{
						award = 0x65;
					}
					break;

				case 2:
					if (Doc()->m_awardItem[2].uid == player->oid)
					{
						award = 0x66;
					}
					break;

				case 5:
					if (Doc()->m_awardItem[5].uid == player->oid)
					{
						award = 0x67;
					}
					break;

				case 6:
					if (Doc()->m_awardItem[0].uid == player->oid)
					{
						award = 0x68;
					}
					break;

				case 4:
					if (Doc()->m_awardItem[4].uid == player->oid)
					{
						award = 0x69;
					}
					break;

				case 3:
					if (Doc()->m_awardItem[3].uid == player->oid)
					{
						award = 0x6A;
					}
					break;

				default:
					break;
				}

				if (award == 0)
				{
					continue;
				}

				if (player->state == 3)
				{
					continue;
				}

				WSendPacket packet((enumClientPacket)0x8c);
				packet.Encode2(GM_CMD_SETPRIZE);
				packet.Encode4(player->oid);
				packet.Encode4(prize->typeId);
				packet.Encode4(prize->count);
				packet.Encode1(prize->type);
				packet.Encode1(award);
				packet.Send(TO_GAME);

				break;
			}
		}
	}
}

bool CGMToolkit::FindItem(unsigned long typeId, char* name) const
{
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);

	if (item == 0)
	{
		return false;
	}

	strcpy(name, item->Name);
	return true;
}

bool CGMToolkit::Confirm(void)
{
	bool result = false;

	if (m_stringCount == 1)
	{
		if (strcmp(m_stringBuffer[0], "y") == 0 ||
			strcmp(m_stringBuffer[0], "Y") == 0)
		{
			result = true;
		}
	}
	else
	{
		result = false;
	}

	if (result && m_pConfirmPacket)
	{
		m_pConfirmPacket->Send(TO_GAME);
	}

	if (m_pConfirmPacket)
	{
		delete m_pConfirmPacket;
		m_pConfirmPacket = 0;
	}

	SetToolKitStatus(GM_TOOLKIT_NORMAL);
	return result;
}

bool CGMToolkit::FindKeywordInString(const char* const string,
	const char* const keyword)
{
	bool result = false;
	int stringLength = strlen(string);
	int keywordLength = strlen(keyword);
	int matched = 0;
	int i;

	for (i = 0; i < stringLength; i++)
	{
		if (string[i] == keyword[matched])
		{
			matched++;
			if (matched == keywordLength)
			{
				result = true;
				break;
			}
		}
		else
		{
			matched = 0;
		}
	}

	return result;
}

eGMToolResult CGMToolkit::CommandHelp(int count)
{
	eGMToolResult result = GM_RESULT_OK;

	if (m_stringCount > 1)
	{
		for (int i = 1; i < count; i++)
		{
			if (strcmp(m_stringBuffer[1], s_commandList[i].keyword) == 0)
			{
				if (s_commandList[i].enabled == true)
				{
					Display(s_commandList[i].help, 0xC6FF85);
				}
				else
				{
					Display(
						"\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xc0\xd4\xb4\xcf\xb4\xd9.",
						0xFF7802);
				}

				result = GM_RESULT_OK;
				break;
			}
			else
			{
				result = GM_RESULT_ARGUMENT_ERROR;
			}
		}
	}
	else
	{
		if (m_stringCount != 1)
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}
		else
		{
			Display(
				"command \xb8\xed\xb7\xc9\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbf\xa9 \xb8\xed\xb7\xc9\xbe\xee \xc8\xae\xc0\xce\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}
	}

	return result;
}

eGMToolResult CGMToolkit::CommandCommand(int count)
{
	for (int i = 1; i < count; i++)
	{
		if (s_commandList[i].enabled == true)
		{
			Display(s_commandList[i].keyword, 0xC6FF85);
		}
		else
		{
			Display(s_commandList[i].keyword, 0xFF7802);
		}
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandVisible(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (m_stringCount > 1)
	{
		if (strcmp(m_stringBuffer[1], "on") == 0)
		{
			SetStatus(GM_STATUS_VISIBLE, GM_SWITCH_ON);
			SendStatus(GM_CMD_VISIBLE);

			Display(
				MakeStr(
					"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, %s",
					"visible on."),
				0xC6FF85);
		}
		else if (strcmp(m_stringBuffer[1], "off") == 0)
		{
			SetStatus(GM_STATUS_VISIBLE, GM_SWITCH_OFF);
			SendStatus(GM_CMD_VISIBLE);
			Display(
				MakeStr(
					"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, %s",
					"visible off."),
				0xC6FF85);
		}
		else
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandWhisper(eGMToolkitStatus status)
{
	if (m_stringCount > 1)
	{
		if (strcmp(m_stringBuffer[1], "on") == 0)
		{
			SetStatus(GM_STATUS_WHISPER, GM_SWITCH_ON);
			SendStatus(GM_CMD_WHISPER);

			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper on.",
				0xC6FF85);
			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel off.",
				0xC6FF85);
		}
		else if (strcmp(m_stringBuffer[1], "off") == 0)
		{
			SetStatus(GM_STATUS_WHISPER, GM_SWITCH_OFF);
			SendStatus(GM_CMD_WHISPER);
			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper off.",
				0xC6FF85);
		}
		else
		{
			return GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandChannel(eGMToolkitStatus status)
{
	if (m_stringCount > 1)
	{
		if (strcmp(m_stringBuffer[1], "on") == 0)
		{
			m_status = (m_status & ~GM_STATUS_WHISPER) | GM_STATUS_CHANNEL;
			SendStatus(GM_CMD_CHANNEL);

			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel on.",
				0xC6FF85);
			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper off.",
				0xC6FF85);
		}
		else if (strcmp(m_stringBuffer[1], "off") == 0)
		{
			SetStatus(GM_STATUS_CHANNEL, GM_SWITCH_OFF);
			SendStatus(GM_CMD_CHANNEL);
			Display(
				"\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel off.",
				0xC6FF85);
		}
		else
		{
			return GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandList(eGMToolkitStatus status)
{
	int count = m_openList.size();

	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			WSingleton<CChatMsg>::Instance()->AddChatMsg(m_openList[i],
				0xC6FF85, true, false);
		}
	}
	else
	{
		Display(
			"\xc3\xdf\xb0\xa1\xb5\xc8 \xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xFF7802);
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandOpen(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		std::string name(m_stringBuffer[1]);

		std::vector<std::string>::iterator it;
		it = std::find(m_openList.begin(), m_openList.end(), name);

		if (it != m_openList.end())
		{
			Display(
				"\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xc0\xcc\xb9\xcc \xc3\xdf\xb0\xa1\xb5\xc8 \xbb\xf3\xb4\xeb\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
		else
		{
			m_openList.push_back(name);
			SendStringBuffer(GM_CMD_OPEN, m_stringBuffer[1]);

			Display(
				"\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xc3\xdf\xb0\xa1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandClose(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		std::string name(m_stringBuffer[1]);

		std::vector<std::string>::iterator it;
		it = std::find(m_openList.begin(), m_openList.end(), name);

		if (it != m_openList.end())
		{
			m_openList.erase(it);

			Display(
				"\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xbb\xe8\xc1\xa6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
			SendStringBuffer(GM_CMD_CLOSE, m_stringBuffer[1]);
		}
		else
		{
			Display(
				"\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandKickUser(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		sBriefUserInfo info;
		std::map<unsigned long, sBriefUserInfo>::iterator it;
		for (it = Doc()->m_briefUserInfoMap.begin();
			it != Doc()->m_briefUserInfoMap.end(); ++it)
		{
			info = it->second;
			if (strcmp(info.sNick, m_stringBuffer[1]) == 0)
			{
				break;
			}
		}

		info = it->second;
		if (it != Doc()->m_briefUserInfoMap.end())
		{
			if ((info.dwIdentity & 4))
			{
				Display(
					"\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb0\xad\xc1\xa6\xc5\xf0\xc0\xe5 \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFF7802);
			}
			else
			{
				WSendPacket packet((enumClientPacket)0x8c);
				packet.Encode2(GM_CMD_KICK);
				packet.Encode4(info.dwGuid);
				if (m_stringCount > 2)
				{
					if (strcmp(m_stringBuffer[2], "-p") == 0)
					{
						packet.Encode1(1);
					}
					else
					{
						result = GM_RESULT_ARGUMENT_ERROR;
					}
				}
				else
				{
					packet.Encode1(0);
				}

				if (result == GM_RESULT_OK)
				{
					packet.Send(TO_GAME);
				}
			}
		}
		else
		{
			Display(
				"\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandDisconnect(eGMToolkitStatus status)
{
	if (m_stringCount <= 1)
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	sBriefUserInfo info;
	std::map<unsigned long, sBriefUserInfo>::iterator it;
	for (it = Doc()->m_briefUserInfoMap.begin();
		it != Doc()->m_briefUserInfoMap.end(); ++it)
	{
		info = it->second;
		if (strcmp(info.sNick, m_stringBuffer[1]) == 0)
		{
			break;
		}
	}

	info = it->second;
	if (it != Doc()->m_briefUserInfoMap.end())
	{
		if ((info.dwIdentity & 4))
		{
			Display(
				"\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
		else
		{
			WSendPacket packet((enumClientPacket)0x8c);
			packet.Encode2(0xB);
			packet.Encode4(info.dwGuid);
			packet.Send(TO_GAME);

			WSendPacket disconnectPacket((enumClientPacket)0x61);
			disconnectPacket.Encode4(info.dwGuid);
			disconnectPacket.Send(TO_GAME);
		}
	}
	else
	{
		Display(
			"\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.",
			0xFF7802);
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandDiscon_UID(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (m_stringCount > 1)
	{
		std::map<unsigned long, sBriefUserInfo>::iterator it;
		if (!Doc()->FindUserInfoUID(atoi(m_stringBuffer[1]), it))
		{
			return GM_RESULT_ARGUMENT_ERROR;
		}

		sBriefUserInfo info = (*it).second;

		if (it != Doc()->m_briefUserInfoMap.end())
		{
			if ((info.dwIdentity & 4))
			{
				Display(
					"\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFF7802);
			}
			else
			{
				WSendPacket packet((enumClientPacket)0x8c);
				packet.Encode2(0xB);
				packet.Encode4(info.dwGuid);
				packet.Send(TO_GAME);

				WSendPacket disconnectPacket((enumClientPacket)0x61);
				disconnectPacket.Encode4(info.dwGuid);
				disconnectPacket.Send(TO_GAME);
			}
		}
		else
		{
			Display(
				"\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandDestroy(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		if (Doc()->m_roomInfo.roomGuid == 0xFFFF)
		{
			int roomNumber = atoi(m_stringBuffer[1]);
			WSendPacket destroyPacket((enumClientPacket)0x60);
			destroyPacket.Encode2(roomNumber);
			destroyPacket.Send(TO_GAME);

			WSendPacket packet((enumClientPacket)0x8c);
			packet.Encode2(GM_CMD_DESTROY);
			packet.Send(TO_GAME);
		}
		else
		{
			Display(
				"\xb7\xce\xba\xf1 \xc0\xcc\xbf\xdc\xc0\xc7 \xc0\xe5\xbc\xd2\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandShowStatus(eGMToolkitStatus status)
{
	if (m_status & GM_STATUS_VISIBLE)
	{
		Display("Visible : on", 0xC6FF85);
	}
	else
	{
		Display("Visible : off", 0xC6FF85);
	}

	if (m_status & GM_STATUS_WHISPER)
	{
		Display("Whisper : on", 0xC6FF85);
	}
	else
	{
		Display("Whisper : off", 0xC6FF85);
	}

	if (m_status & GM_STATUS_CHANNEL)
	{
		Display("Channel : on", 0xC6FF85);
	}
	else
	{
		Display("Channel : off", 0xC6FF85);
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandWind(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (m_stringCount > 2)
	{
		unsigned char roomType = Doc()->m_roomInfo.gameType;
		switch (roomType)
		{
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 14:
			break;
		default:
			if (roomType != 2)
			{
				unsigned char speed = atoi(m_stringBuffer[1]);
				unsigned char direction = atoi(m_stringBuffer[2]);

				if (speed > 18)
				{
					Display("Wind speed value [0-18]", 0xFF7802);
					result = GM_RESULT_ARGUMENT_ERROR;
				}

				if (direction > 360)
				{
					Display("Wind direction value [0-360]", 0xFF7802);
					result = GM_RESULT_ARGUMENT_ERROR;
				}
				else if (result != GM_RESULT_ARGUMENT_ERROR)
				{
					WSendPacket packet((enumClientPacket)0x8c);
					packet.Encode2(GM_CMD_WIND);
					packet.Encode1(speed - 1);
					packet.Encode1(direction);
					packet.Send(TO_GAME);
				}
			}
			else
			{
				Display(
					"\xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xFF7802);
			}
			break;
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandWeather(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		int weather = -1;
		if (strcmp(m_stringBuffer[1], "fine") == 0)
		{
			weather = 0;
		}
		else if (strcmp(m_stringBuffer[1], "cloud") == 0)
		{
			weather = 1;
		}
		else if (strcmp(m_stringBuffer[1], "rain") == 0)
		{
			weather = 2;
		}
		else if (strcmp(m_stringBuffer[1], "snow") == 0)
		{
			weather = 3;
		}

		if (weather != -1)
		{
			WSendPacket packet((enumClientPacket)0x8c);
			packet.Encode2(GM_CMD_WEATHER);
			packet.Encode1(weather);
			packet.Send(TO_GAME);
		}
		else
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandIdentity(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		if (Doc()->m_roomInfo.roomGuid == 0xFFFF)
		{
			unsigned long identity = 0;
			if (strcmp(m_stringBuffer[1], "admin") == 0)
			{
				identity = 0x484;

				Display("\xbc\xb3\xc1\xa4 : Administrator", 0xC6FF85);
			}
			else if (strcmp(m_stringBuffer[1], "user") == 0)
			{
				identity = 0x80;

				Display("\xbc\xb3\xc1\xa4 : Normal user", 0xC6FF85);
				m_bGM = true;
			}

			if (identity != 0)
			{
				if (!IsAdministrator())
				{
					if (identity != 0x80)
					{
						WSendPacket packet((enumClientPacket)0x41);
						packet.Encode4(identity);
						packet.EncodeStr(
							std::string(Doc()->m_myInfo.info.sNick));
						packet.Send(TO_GAME);
					}
				}
				else
				{
					if (identity != 0x484)
					{
						WSendPacket packet((enumClientPacket)0x41);
						packet.Encode4(identity);
						packet.EncodeStr(
							std::string(Doc()->m_myInfo.info.sNick));
						packet.Send(TO_GAME);
					}
				}
			}
			else
			{
				result = GM_RESULT_ARGUMENT_ERROR;
			}
		}
		else
		{
			Display(
				"\xb1\xc7\xc7\xd1 \xba\xaf\xb0\xe6\xc0\xba \xb7\xce\xba\xf1\xbf\xa1\xbc\xad\xb8\xb8 \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else if (m_stringCount != 1)
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}
	else
	{
		if (IsAdministrator())
		{
			Display("You are the Administrator now! (~+_+)~", 0xC6FF85);
		}
		else
		{
			Display("You are a normal user T_T", 0xC6FF85);
		}
	}

	return result;
}

eGMToolResult CGMToolkit::CommandNotice(eGMToolkitStatus status)
{
	if (m_stringCount > 1)
	{
		std::string notice("");

		for (int i = 1; i < m_stringCount; i++)
		{
			notice = notice + m_stringBuffer[i];
			notice = notice + " ";
		}

		WSendPacket packet((enumClientPacket)0x57);
		packet.EncodeStr(notice);
		packet.Send(TO_GAME);
	}
	else
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandGiveItem(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (status == GM_TOOLKIT_NORMAL)
	{
		if (m_stringCount > 1)
		{
			char itemName[0x40] = { 0 };

			std::map<unsigned long, sBriefUserInfo>::iterator it;
			for (it = Doc()->m_briefUserInfoMap.begin();
				it != Doc()->m_briefUserInfoMap.end(); ++it)
			{
				sBriefUserInfo info = it->second;
				if (strcmp(info.sNick, m_stringBuffer[1]) == 0)
				{
					break;
				}
			}

			sBriefUserInfo user = it->second;
			if (it != Doc()->m_briefUserInfoMap.end())
			{
				unsigned long count = 0;
				if (m_stringCount == 3)
				{
					count = 1;
				}
				else if (m_stringCount > 3)
				{
					count = atoi(m_stringBuffer[3]);
				}

				if (count > 0)
				{
					unsigned long typeId = atoi(m_stringBuffer[2]);

					if (!FindItem(typeId, itemName))
					{
						Display(
							"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb TypeID \xc0\xd4\xb4\xcf\xb4\xd9.",
							0xFF7802);
					}
					else if (m_pConfirmPacket == 0)
					{
						m_pConfirmPacket =
							new WSendPacket((enumClientPacket)0x8c);
						m_pConfirmPacket->Encode2(GM_CMD_GIVEITEM);
						m_pConfirmPacket->Encode4(user.dwGuid);
						m_pConfirmPacket->Encode4(typeId);
						m_pConfirmPacket->Encode4(count);

						char text[0x80] = { 0 };

						sprintf(text,
							"\xc5\xb8\xc0\xd4ID : %d, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
							typeId, itemName, count);
						Display(text, 0xC6FF85);
						memset(text, 0, sizeof(text));

						sprintf(text,
							"'%s' \xb4\xd4\xbf\xa1\xb0\xd4 \xc0\xa7\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xf6\xb1\xde \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee? [/Y, /N]",
							m_stringBuffer[1]);
						Display(text, 0xC6FF85);

						SetConfirmWaitTag(GM_CMD_GIVEITEM);
					}
					else
					{
						result = GM_RESULT_SYNTAX_ERROR;
					}
				}
				else
				{
					result = GM_RESULT_ARGUMENT_ERROR;
				}
			}
			else
			{
				Display(
					"\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.",
					0xFF7802);
			}
		}
		else
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		if (Confirm())
		{
			Display(
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc1\xf6\xb1\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}
		else
		{
			Display(
				"'giveitem' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}

		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return result;
}

eGMToolResult CGMToolkit::CommandGoldenBell(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (Doc()->m_roomInfo.roomGuid != 0xFFFF)
	{
		if (status == GM_TOOLKIT_NORMAL)
		{
			if (m_stringCount > 1)
			{
				char itemName[64] = { 0 };
				unsigned long count = 0;

				if (m_stringCount == 2)
				{
					count = 1;
				}
				else if (m_stringCount > 2)
				{
					count = atoi(m_stringBuffer[2]);
				}

				if (count > 0)
				{
					unsigned long typeId = atoi(m_stringBuffer[1]);

					if (!FindItem(typeId, itemName))
					{
						Display(
							"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc5\xb8\xc0\xd4ID \xc0\xd4\xb4\xcf\xb4\xd9.",
							0xFF7802);
					}
					else if (m_pConfirmPacket == 0)
					{
						m_pConfirmPacket =
							new WSendPacket((enumClientPacket)0x8c);
						m_pConfirmPacket->Encode2(GM_CMD_GOLDENBELL);
						m_pConfirmPacket->Encode4(typeId);
						m_pConfirmPacket->Encode4(count);

						char text[128] = { 0 };

						sprintf(text,
							"\xc5\xb8\xc0\xd4ID : %d, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
							typeId, itemName, count);
						Display(text, 0xC6FF85);
						memset(text, 0, sizeof(text));

						sprintf(text,
							"\xc7\xf6\xc0\xe7 \xb7\xeb\xbe\xc8\xc0\xc7 \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4 \xc0\xa7\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xf6\xb1\xde \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]");
						Display(text, 0xC6FF85);

						SetConfirmWaitTag(GM_CMD_GOLDENBELL);
					}
					else
					{
						result = GM_RESULT_SYNTAX_ERROR;
					}
				}
			}
			else
			{
				result = GM_RESULT_ARGUMENT_ERROR;
			}
		}
		else
		{
			if (Confirm())
			{
				Display(
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc1\xf6\xb1\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xC6FF85);
			}
			else
			{
				Display(
					"'goldenbell' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0xC6FF85);
			}

			SetToolKitStatus(GM_TOOLKIT_NORMAL);
		}
	}
	else
	{
		Display(
			"\xb7\xce\xba\xf1\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xFF7802);
	}

	return result;
}

eGMToolResult CGMToolkit::CommandSetPrize(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (status == GM_TOOLKIT_NORMAL)
	{
		if (m_stringCount == 2)
		{
			if (strcmp("on", m_stringBuffer[1]) == 0)
			{
				m_bPrizeSwitch = true;
				Display("SetPrize \xb1\xe2\xb4\xc9 \xc8\xb0\xbc\xba\xc8\xad.",
					0xC6FF85);
			}
			else if (strcmp("off", m_stringBuffer[1]) == 0)
			{
				m_bPrizeSwitch = false;
				Display(
					"SetPrize \xb1\xe2\xb4\xc9 \xba\xf1\xc8\xb0\xbc\xba\xc8\xad.",
					0xC6FF85);
			}
			else
			{
				result = GM_RESULT_ARGUMENT_ERROR;
			}
		}
		else if (m_stringCount >= 3)
		{
			sPrize* prize = new sPrize;

			int itemIndex = 0;
			bool valid = true;

			char itemName[0x40] = { 0 };

			if (strcmp(m_stringBuffer[1], "speeder") == 0)
			{
				prize->type = 1;
				itemIndex = 2;
			}
			else if (strcmp(m_stringBuffer[1], "runner") == 0)
			{
				prize->type = 2;
				itemIndex = 2;
			}
			else if (strcmp(m_stringBuffer[1], "chipin") == 0)
			{
				prize->type = 3;
				itemIndex = 2;
			}
			else if (strcmp(m_stringBuffer[1], "longputt") == 0)
			{
				prize->type = 4;
				itemIndex = 2;
			}
			else if (strcmp(m_stringBuffer[1], "recovery") == 0)
			{
				prize->type = 5;
				itemIndex = 2;
			}
			else if (strcmp(m_stringBuffer[1], "lucky") == 0)
			{
				prize->type = 6;
				itemIndex = 2;
			}
			else
			{
				char number[0x10] = { 0 };

				int firstRank = atoi(m_stringBuffer[1]);

				memset(number, 0, sizeof(number));
				itoa(firstRank, number, 10);

				if (strlen(number) != strlen(m_stringBuffer[1]))
				{
					result = GM_RESULT_ARGUMENT_ERROR;
					valid = false;
				}
				else
				{
					if (firstRank > 0 && firstRank < 301)
					{
						prize->rankFrom = firstRank;

						if (strcmp(m_stringBuffer[2], "to") == 0)
						{
							int lastRank = atoi(m_stringBuffer[3]);

							memset(number, 0, sizeof(number));
							itoa(lastRank, number, 10);

							if (strlen(number) != strlen(m_stringBuffer[3]))
							{
								result = GM_RESULT_ARGUMENT_ERROR;
								valid = false;
							}
							else
							{
								if (m_stringCount < 5)
								{
									result = GM_RESULT_ARGUMENT_ERROR;
									valid = false;
								}
								else
								{
									if (lastRank > 0 && lastRank < 301)
									{
										prize->rankTo = lastRank;
										prize->type = 0;
										itemIndex = 4;
									}
									else
									{
										result = GM_RESULT_ARGUMENT_ERROR;
										valid = false;
									}
								}
							}
						}
						else
						{
							prize->rankTo = firstRank;
							prize->type = 0;
							itemIndex = 2;
						}
					}
					else
					{
						result = GM_RESULT_ARGUMENT_ERROR;
						valid = false;
					}
				}
			}

			if (itemIndex <= m_stringCount && valid)
			{
				unsigned long typeId = atoi(m_stringBuffer[itemIndex]);

				if (FindItem(typeId, itemName))
				{
					prize->typeId = typeId;

					if (itemIndex + 1 < m_stringCount)
					{
						int count = atoi(m_stringBuffer[itemIndex + 1]);
						if (count > 0)
						{
							prize->count = count;
							SetConfirmWaitTag(GM_CMD_SETPRIZE);
						}
						else
						{
							result = GM_RESULT_ARGUMENT_ERROR;
						}
					}
					else
					{
						prize->count = 1;
						SetConfirmWaitTag(GM_CMD_SETPRIZE);
					}

					if (GetToolKitStatus() == GM_TOOLKIT_CONFIRM_WAIT)
					{
						char text[0x80];
						const char* name;

						if (prize->type == 0)
						{
							if (prize->rankTo == prize->rankFrom)
							{
								memset(text, 0, sizeof(text));
								sprintf(text,
									"\xb9\xfc   \xc0\xa7 : %d\xb5\xee",
									prize->rankFrom);
								Display(text, 0xC6FF85);

								memset(text, 0, sizeof(text));
								sprintf(text, "\xbe\xc6\xc0\xcc\xc5\xdb : %s",
									itemName);
								Display(text, 0xC6FF85);

								memset(text, 0, sizeof(text));
								sprintf(text,
									"\xbc\xf6   \xb7\xae : %d\xb0\xb3",
									prize->count);
								Display(text, 0xC6FF85);
							}
							else
							{
								memset(text, 0, sizeof(text));
								sprintf(text,
									"\xb9\xfc   \xc0\xa7 : %d\xb5\xee \xba\xce\xc5\xcd %d\xb5\xee \xb1\xee\xc1\xf6 ",
									prize->rankFrom, prize->rankTo);
								Display(text, 0xC6FF85);

								memset(text, 0, sizeof(text));
								sprintf(text, "\xbe\xc6\xc0\xcc\xc5\xdb : %s",
									itemName);
								Display(text, 0xC6FF85);

								memset(text, 0, sizeof(text));
								sprintf(text,
									"\xbc\xf6   \xb7\xae : %d\xb0\xb3",
									prize->count);
								Display(text, 0xC6FF85);
							}
						}
						else
						{
							char awardName[0x20] = { 0 };

							switch (prize->type)
							{
							case 1:
								name =
									"\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5";
								strcpy(awardName, name);
								break;

							case 2:
								name =
									"\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca";
								strcpy(awardName, name);
								break;

							case 5:
								name =
									"\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae";
								strcpy(awardName, name);
								break;

							case 6:
								name = "\xc7\xe0\xbf\xee\xbb\xf3";
								strcpy(awardName, name);
								break;

							case 4:
								name =
									"\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd";
								strcpy(awardName, name);
								break;

							case 3:
								name =
									"\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce";
								strcpy(awardName, name);
								break;
							}

							memset(text, 0, sizeof(text));
							sprintf(text, "\xb9\xfc   \xc0\xa7 : %s",
								awardName);
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							sprintf(text, "\xbe\xc6\xc0\xcc\xc5\xdb : %s",
								itemName);
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							sprintf(text, "\xbc\xf6   \xb7\xae : %d\xb0\xb3",
								prize->count);
							Display(text, 0xC6FF85);
						}

						sprintf(text,
							"\xbc\xb3\xc1\xa4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]");
						Display(text, 0xC6FF85);

						m_pPrize = prize;
					}
				}
				else
				{
					result = GM_RESULT_ARGUMENT_ERROR;
				}
			}
			else
			{
				result = GM_RESULT_ARGUMENT_ERROR;
			}
		}
		else
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}
	}
	else
	{
		if (Confirm())
		{
			if (m_pPrize != 0)
			{
				m_prizeList.push_back(m_pPrize);
				m_pPrize = 0;
			}

			Display(
				"\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae \xbc\xb3\xc1\xa4\xbf\xcf\xb7\xe1.",
				0xC6FF85);
		}
		else
		{
			if (m_pPrize != 0)
			{
				delete m_pPrize;
				m_pPrize = 0;
			}

			Display(
				"'setprize' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}
		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return result;
}

eGMToolResult CGMToolkit::CommandUnsetPrize(eGMToolkitStatus status)
{
	if (status == GM_TOOLKIT_NORMAL)
	{
		if (m_stringCount >= 1)
		{
			Display(
				"\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae\xb8\xa6 \xc0\xfc\xba\xce \xbb\xe8\xc1\xa6\xc7\xcf\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]",
				0xC6FF85);
			SetConfirmWaitTag(GM_CMD_UNSETPRIZE);
		}
		else
		{
			return GM_RESULT_SYNTAX_ERROR;
		}
	}
	else
	{
		if (Confirm())
		{
			for (std::list<sPrize*>::iterator it = m_prizeList.begin();
				it != m_prizeList.end(); ++it)
			{
				if (*it != 0)
				{
					delete *it;
					*it = 0;
				}
			}

			m_prizeList.clear();

			Display(
				"\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae \xbb\xe8\xc1\xa6\xbf\xcf\xb7\xe1.",
				0xC6FF85);
			m_bPrizeSwitch = false;
		}
		else
		{
			Display(
				"'unsetprize' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xC6FF85);
		}

		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandShowPrize(eGMToolkitStatus status)
{
	char itemName[0x40] = { 0 };
	char text[0x80] = { 0 };

	if (GetPrizeSwitch())
	{
		Display("SetPrize \xb1\xe2\xb4\xc9 \xc8\xb0\xbc\xba\xc8\xad", 0xC6FF85);
	}
	else
	{
		Display("SetPrize \xb1\xe2\xb4\xc9 \xba\xf1\xc8\xb0\xbc\xba\xc8\xad",
			0xC6FF85);
	}

	int index = 0;
	for (std::list<sPrize*>::iterator it = m_prizeList.begin();
		it != m_prizeList.end(); ++it)
	{
		sPrize* prize = *it;

		if (prize == 0)
		{
			continue;
		}

		if (FindItem(prize->typeId, itemName))
		{
			index++;

			memset(text, 0, sizeof(text));

			if (prize->type == 0)
			{
				if (prize->rankFrom == prize->rankTo)
				{
					sprintf(text,
						"[%02d] \xb9\xfc\xc0\xa7 : %d\xb5\xee, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
						index, prize->rankFrom, itemName, prize->count);
				}
				else
				{
					sprintf(text,
						"[%02d] \xb9\xfc\xc0\xa7 : %d\xb5\xee \xba\xce\xc5\xcd %d\xb5\xee \xb1\xee\xc1\xf6, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
						index, prize->rankFrom, prize->rankTo, itemName,
						prize->count);
				}
			}
			else
			{
				char awardName[0x20] = { 0 };

				const char* name;

				switch (prize->type)
				{
				case 1:
					name = "\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5";
					strcpy(awardName, name);
					break;

				case 2:
					name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca";
					strcpy(awardName, name);
					break;

				case 5:
					name =
						"\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae";
					strcpy(awardName, name);
					break;

				case 6:
					name = "\xc7\xe0\xbf\xee\xbb\xf3";
					strcpy(awardName, name);
					break;

				case 4:
					name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd";
					strcpy(awardName, name);
					break;

				case 3:
					name = "\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce";
					strcpy(awardName, name);
					break;
				}

				sprintf(text,
					"[%02d] \xb9\xfc\xc0\xa7 : %s, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
					index, awardName, itemName, prize->count);
			}

			Display(text, 0xC6FF85);
		}
		else
		{
			Display("\xc0\xdf\xb8\xf8\xb5\xc8 \xc1\xb6\xb0\xc7\xb9\xae.",
				0xFF7802);
		}
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandLoadScript(eGMToolkitStatus status)
{
	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandNoticePrize(eGMToolkitStatus status)
{
	int count = m_prizeList.size();

	if (count != 0)
	{
		WSendPacket packet((enumClientPacket)0x8c);
		packet.Encode2(GM_CMD_NOTICEPRIZE);
		packet.Encode4(count);

		for (std::list<sPrize*>::iterator it = m_prizeList.begin();
			it != m_prizeList.end(); ++it)
		{
			sPrize* prize = *it;

			if (prize != 0)
			{
				packet.EncodeBuffer(prize, sizeof(sPrize));
			}
		}

		packet.Send(TO_GAME);
	}
	else
	{
		Display(
			"\xb5\xee\xb7\xcf\xb5\xc8 \xc1\xb6\xb0\xc7\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0xC6FF85);
	}

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandGetTID(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		Display("Succeeded command.", 0xC6FF85);
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandSetMission(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 1)
	{
		int mission = atoi(m_stringBuffer[1]);

		WSendPacket packet((enumClientPacket)0x8c);
		packet.Encode2(GM_CMD_SETMISSION);
		packet.Encode4(mission);
		packet.Send(TO_GAME);

		Display("Succeeded command.", 0xC6FF85);
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandFindItem(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;

	if (m_stringCount > 2)
	{
		bool found = true;

		if (strcmp(m_stringBuffer[1], "\xbe\xc6\xc0\xcc\xc5\xdb") == 0)
		{
			found = FindItem<IFF_STRUCT::sItem>(m_stringBuffer[2],
				ItemManager()->m_ItemMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xc4\xab\xb5\xe5") == 0)
		{
			found = FindItem<IFF_STRUCT::sCard>(m_stringBuffer[2],
				ItemManager()->m_CardMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xc6\xc4\xc3\xf7") == 0)
		{
			found = FindItem<IFF_STRUCT::sPart>(m_stringBuffer[2],
				ItemManager()->m_PartMap);
		}
		else if (strcmp(m_stringBuffer[1],
					 "\xba\xb8\xc1\xb6\xc6\xc4\xc3\xf7") == 0)
		{
			found = FindItem<IFF_STRUCT::sAuxPart>(m_stringBuffer[2],
				ItemManager()->m_AuxPartMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xc5\xac\xb7\xb4") == 0)
		{
			found = FindItem<IFF_STRUCT::sClubSet>(m_stringBuffer[2],
				ItemManager()->m_ClubSetMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xbe\xc6\xc1\xee\xc5\xd8") == 0)
		{
			found = FindItem<IFF_STRUCT::sBall>(m_stringBuffer[2],
				ItemManager()->m_BallMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xbc\xbc\xc6\xae") == 0)
		{
			found = FindItem<IFF_STRUCT::sSetItem>(m_stringBuffer[2],
				ItemManager()->m_SetItemMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xb0\xa1\xb1\xb8") == 0)
		{
			found = FindItem<IFF_STRUCT::sFurniture>(m_stringBuffer[2],
				ItemManager()->m_FurnitureMap);
		}
		else if (strcmp(m_stringBuffer[1], "\xb8\xf0\xb5\xce") == 0)
		{
			found = false;

			found |= FindItem<IFF_STRUCT::sItem>(m_stringBuffer[2],
				ItemManager()->m_ItemMap);
			found |= FindItem<IFF_STRUCT::sCard>(m_stringBuffer[2],
				ItemManager()->m_CardMap);
			found |= FindItem<IFF_STRUCT::sPart>(m_stringBuffer[2],
				ItemManager()->m_PartMap);
			found |= FindItem<IFF_STRUCT::sAuxPart>(m_stringBuffer[2],
				ItemManager()->m_AuxPartMap);
			found |= FindItem<IFF_STRUCT::sClubSet>(m_stringBuffer[2],
				ItemManager()->m_ClubSetMap);
			found |= FindItem<IFF_STRUCT::sBall>(m_stringBuffer[2],
				ItemManager()->m_BallMap);
			found |= FindItem<IFF_STRUCT::sSetItem>(m_stringBuffer[2],
				ItemManager()->m_SetItemMap);
			found |= FindItem<IFF_STRUCT::sFurniture>(m_stringBuffer[2],
				ItemManager()->m_FurnitureMap);
		}
		else
		{
			result = GM_RESULT_ARGUMENT_ERROR;
		}

		if (!found)
		{
			Display(
				"\xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xFF7802);
		}
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandCategory(eGMToolkitStatus status)
{
	eGMToolResult result = GM_RESULT_OK;
	if (m_stringCount > 0)
	{
		Display("\xbe\xc6\xc0\xcc\xc5\xdb", 0xC6FF85);
		Display("\xc4\xab\xb5\xe5", 0xC6FF85);
		Display("\xc6\xc4\xc3\xf7", 0xC6FF85);
		Display("\xba\xb8\xc1\xb6\xc6\xc4\xc3\xf7", 0xC6FF85);
		Display("\xc5\xac\xb7\xb4", 0xC6FF85);
		Display("\xbe\xc6\xc1\xee\xc5\xd8", 0xC6FF85);
		Display("\xbc\xbc\xc6\xae", 0xC6FF85);
		Display("\xb0\xa1\xb1\xb8", 0xC6FF85);
		Display("\xb8\xf0\xb5\xce", 0xC6FF85);
	}
	else
	{
		result = GM_RESULT_ARGUMENT_ERROR;
	}

	return result;
}

eGMToolResult CGMToolkit::CommandMatchMapSetting(eGMToolkitStatus status)
{
	if (m_stringCount != 2)
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	int map = atoi(m_stringBuffer[1]);
	if (map < 0 || map > 20)
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	WSendPacket packet((enumClientPacket)0x8c);
	packet.Encode2(GM_CMD_MATCHMAP);
	packet.Encode4(map);
	packet.Send(TO_GAME);

	return GM_RESULT_OK;
}

eGMToolResult CGMToolkit::CommandMatchHoleSetting(eGMToolkitStatus status)
{
	if (m_stringCount != 2)
	{
		return GM_RESULT_ARGUMENT_ERROR;
	}

	int holeCount = atoi(m_stringBuffer[1]);
	switch (holeCount)
	{
	case 3:
	case 6:
	case 9:
	case 18:
	{
		WSendPacket packet((enumClientPacket)0x8c);
		packet.Encode2(GM_CMD_MATCHHOLE);
		packet.Encode4(holeCount);
		packet.Send(TO_GAME);
		break;
	}
	default:
		return GM_RESULT_ARGUMENT_ERROR;
	}

	return GM_RESULT_OK;
}
