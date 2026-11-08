#include <fstream>
#include <wininet.h>
#include "g_starmode.h"
#include "familymain.h"
#include "shareddoc.h"
#include "parttidlist.h"
#include "mousecursor.h"
#include "actor.h"

static const char settingsHeader[] = "SINGLE_PLAY_CHARACTER_SETTINGS\n";
static const char settingsError[] = "ERROR!\n";

CGStarMode::CGStarMode()
{
	memset(m_parts, 0, sizeof(m_parts));
	m_items = NULL;
	m_characters = NULL;
}

CGStarMode::~CGStarMode()
{
	if (m_items)
	{
		delete[] m_items;
		m_items = NULL;
	}
	if (m_characters)
	{
		delete[] m_characters;
		m_characters = NULL;
	}
}

void CGStarMode::GStarModeInit()
{
	SetPlayerData();
	for (int i = 0; i < 4; ++i)
	{
		memset(&Doc()->m_userInfo[i], 0, sizeof(sUserInfo));
		if (i < 2)
			sprintf(Doc()->m_userInfo[i].info.sNick, "PLAYER%d", i + 1);
		else
		{
			Doc()->m_userInfo[i].info.sID[0] = 0;
			Doc()->m_userInfo[i].info.sNick[0] = 0;
		}
		Doc()->m_userInfo[i].stat.Level = Doc()->m_myInfo.stat.Level;
		Doc()->m_userInfo[i].userEquip.guidMascot =
			Doc()->m_myInfo.userEquip.guidMascot;
		memset(CFamilyMain::ms_itemslotBackup[i], 0,
			sizeof(CFamilyMain::ms_itemslotBackup[i]));
	}
	Doc()->m_golfGame.shotTimeLimit = 0;
	SetCurMap(0);
	Doc()->m_golfGame.gameType = 0;
	strcpy(Doc()->m_golfGame.gameTypeName, Doc()->m_gameTypeInfo[0].name);
	Doc()->m_golfGame.holes = 3;
	Doc()->m_holeType = 0;
	Doc()->m_bGameOver = false;
	CMouseCursor::Instance()->SetActive(false);
	CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
	CTaskManager::Instance()->PostMsg(NULL, "Family", 0, (int)"FAMILY", 1, 0,
		0);
}

void CGStarMode::SetPlayerData()
{
	CPartTidList parts;
	char playerName[22];
	(void)playerName;
	Doc()->m_ballList.clear();
	Doc()->m_charMap.clear();
	Doc()->m_caddieMap.clear();
}

void CGStarMode::GetAllItemFromItemManager()
{
}

void ErrorMessageToMyRoom(void* message)
{
	AfxPostMsg(NULL, "RealMyRoom", 35, (int)message, 0, 0, 0);
}

bool CGStarMode::SavePartsInfoCharacterMap(
	const std::map<unsigned int, sCharacterInfo>& characters,
	const char* filename)
{
	std::fstream file(filename, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << settingsHeader;
		file.close();
	}
	for (std::map<unsigned int, sCharacterInfo>::const_iterator it =
			 characters.begin();
		it != characters.end(); ++it)
	{
		if (!SavePartsInfoCharacter(it->second, filename, false,
				ErrorMessageToMyRoom))
		{
			std::fstream errorFile(filename, std::ios::out | std::ios::trunc);
			if (errorFile.is_open())
			{
				errorFile << settingsError;
				errorFile.close();
			}
			return false;
		}
	}
	return true;
}

bool CGStarMode::SavePartsInfoCharacter(const sCharacterInfo& character,
	const char* filename, bool truncate, void (*onError)(void*))
{
	int mode = std::ios::app;
	if (truncate)
		mode = std::ios::trunc;
	std::fstream file(filename, std::ios::out | mode);
	if (!file.is_open())
		return false;
	IFF_STRUCT::sChar* info = ItemManager()->FindChar(character.tid);
	bool result = true;
	if (info)
	{
		char buffer[101];
		if (_snprintf(buffer, 100, "%d", character.tid & 0x3ffffff) < 0)
			buffer[100] = 0;
		unsigned int length = strlen(buffer);
		for (int i = 0; i < 10; ++i)
		{
			unsigned long tid = character.tidParts[i];
			if (tid)
			{
				if (((tid >> 18) & 0xff) > 9 || ((tid >> 13) & 0x1f) > 9 ||
					((tid >> 11) & 3) > 9 || ((tid >> 9) & 3) > 9 ||
					(tid & 0x1ff) > 9)
				{
					result = false;
					const char* partName = NULL;
					IFF_STRUCT::sPart* part =
						ItemManager()->FindPart(character.tidParts[i]);
					if (part)
						partName = part->c.Name;
					if (onError)
						onError(MakeStr(
							"\xb4\xd9\xc0\xbd\xc0\xbb \xc0\xfa\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\n[ %s: %s ]\n",
							info->c.Name, partName));
					break;
				}
				if (_snprintf(buffer + length, 100 - length, "|%d%d%d%d%d",
						(tid >> 18) & 0xff, (tid >> 13) & 0x1f, (tid >> 11) & 3,
						(tid >> 9) & 3, tid & 0x1ff) < 0)
					buffer[100] = 0;
			}
			else if (_snprintf(buffer + length, 100 - length, "|%d", 0) < 0)
				buffer[100] = 0;
			length = strlen(buffer);
		}
		if (_snprintf(buffer + length, 100 - length, "#%s\n", info->c.Name) < 0)
			buffer[100] = 0;
		if (result)
			file << buffer;
	}
	file.close();
	return result;
}

bool CGStarMode::LoadFile(const char* filename)
{
	char buffer[10240] = "";
	bool result = false;
	char delimiters[] = "\t\r\n|";
	FILE* file = fopen(filename, "rt");
	if (file)
	{
		if (fgets(buffer, sizeof(buffer), file) &&
			strcmp(buffer, settingsError) != 0 &&
			strcmp(buffer, settingsHeader) == 0)
		{
			while (fgets(buffer, sizeof(buffer), file))
			{
				char* token = strtok(buffer, delimiters);
				if (!token)
					goto done;
				int character = atoi(token);
				for (int part = 0; part < 10; ++part)
				{
					token = strtok(NULL, delimiters);
					if (!token)
						goto done;
					Parse(character, part, token);
				}
			}
			result = true;
		}
done:
		fclose(file);
	}
	return result;
}

void CGStarMode::Parse(int character, int part, char* text)
{
	if (!atoi(text))
	{
		m_parts[character][part] = 0;
		return;
	}
	char digit[10];
	unsigned char fields[5];
	for (int i = 0; i < 5; ++i)
	{
		lstrcpyn(digit, text + i, 2);
		fields[i] = atoi(digit);
	}
	m_parts[character][part] = (2 << 26) | (fields[0] << 18) |
		(fields[1] << 13) | (fields[2] << 11) | (fields[3] << 9) | fields[4];
}

void CGStarMode::SetParts(int character, unsigned long* parts)
{
	unsigned long total = 0;
	for (int i = 0; i < 24; ++i)
		total += m_parts[character][i];
	if (total)
		memcpy(parts, m_parts[character], sizeof(m_parts[character]));
}

void CGStarMode::SkipServer()
{
	// HACK: preserve emission order by adding DCE'd dependencies
	if (0)
		SavePartsInfoCharacterMap(Doc()->m_charMap, NULL);
	WNetworkSystem* network = NET();
	if (network->IsConnected(WNetworkSystem::NET_GAME))
		network->ForceShutDown(WNetworkSystem::NET_GAME);
}

void CGStarMode::Skip30vs()
{
	WSendPacket packet(4);
	packet.Encode1(8);
	packet.Send(TO_GAME);
	AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
}

bool CGStarMode::IsInternetConnect()
{
	DWORD flags;
	if (InternetGetConnectedState(&flags, 0))
	{
		if (flags & INTERNET_CONNECTION_CONFIGURED)
			goto configured;
		if (flags & INTERNET_CONNECTION_LAN)
			return true;
	}
	return false;
configured:
	return true;
}
