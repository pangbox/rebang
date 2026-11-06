#include "stdafx.h"

#include "console.h"
#include "networksystem.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "chatmsg.h"
#include "clientsetting.h"
#include "inputmanager.h"
#include "golfdoc.h"
#include "packet.h"
#include "camerawork.h"
#include "club.h"
#include "golfball.h"
#include "polysoup.h"
#include "powergauge.h"
#include "scenemanager.h"
#include "shareddoc.h"
#include "commonutil.h"
#include "quadtree.h"
#include "headicon.h"
#include "taskmanager.h"
#include "tweaker.h"
#include "sky.h"
#include "soundmanager.h"
#include "weather.h"
#include "wind.h"
#include "wutil.h"
#include "wvideo.h"
#include "wfont.h"
#include "wmath.h"
#include "woverlay.h"
#include "wtypes.h"
#include "wresrcmng.h"
#include "wview.h"
#include "../../shared/luasystem/luascriptmanager.h"
#include "contentsdoc.h"
#include "frwnd.h"
#include "fresh.h"
#include "realmyroomtask.h"
#include "shoptask.h"

#include "guildactor.h"
#include "guilddefine.h"
#include "mathconsts.h"

const float s_consoleHeight = 302.0f;
const float s_fadeSpeed = 4.0f;

// clang-format off
struct
{
	const char* name;
	int id;
	const char* usage;
} command_table[] = {
	{ "help",           0,  "help" },
	{ "clear",          1,  "clear" },
	{ "caddie",         2,  "caddie [serial no.] [level(1-4)]" },
	{ "map",            3,  "map [value(0-2)]" },
	{ "hole",           4,  "hole [value(1-18)]" },
	{ "weather",        5,  "weather [value(0-3)]" },
	{ "heal",           6,  "heal [value]" },
	{ "clubset",        7,  "clubset [value(0-3)]" },
	{ "combo",          8,  "combo [value]" },
	{ "tag",            9,  "tag" },
	{ "render",         10, "render [value(0-3)]" },
	{ "fill",           11, "fill [value(0-2)]" },
	{ "fov",            12, "fov [value(10-120)]" },
	{ "dist",           13, "dist" },
	{ "save",           14, "save [filename]" },
	{ "load",           15, "load [filename]" },
	{ "wind",           16, "wind [value(0-9)] [value(0-360)]" },
	{ "world",          17, "world : \xbf\xf9\xb5\xe5\xc0\xc7 \xc5\xa9\xb1\xe2\xb8\xa6 \xbe\xcb\xb7\xc1 \xc1\xdc" },
	{ "cull",           18, "cull [value]" },
	{ "timeout",        19, "timeout : \xc5\xb8\xc0\xd3 \xbe\xc6\xbf\xf4 \xc5\xe4\xb1\xdb (on:default)" },
	{ "cameracheck",    20, "cameracheck : \xc4\xab\xb8\xde\xb6\xf3 \xc3\xbc\xc5\xb7 \xc5\xe4\xb1\xdb (on:default)" },
	{ "slope",          21, "slope [value] (1.7:default)" },
	{ "texture",        22, "texture [filename]" },
	{ "effectlevel",    23, "effectlevel [value(0-2)] (1:default)" },
	{ "mipfilter",      24, "mipfilter [value(0-5)] (1:default)" },
	{ "mparts",         25, "mparts [part name]" },
	{ "dparts",         26, "dparts [typeid]" },
	{ "statistics",     27, "statistics" },
	{ "pangya",         28, "pangya [value(n)] (1:default)" },
	{ "alpangya",       29, "alpangya [value(n)] (1:default)" },
	{ "ball",           30, "ball [serial no.]" },
	{ "scale",          31, "scale [value(1-\xa1\xc4)] (1:default)" },
	{ "trace",          32, "trace [value] (value=\xc7\xc1\xb7\xb9\xc0\xd3 \xb9\xf8\xc8\xa3)" },
	{ "grid",           33, "grid [value]" },
	{ "gridshow",       34, "gridshow [0-1]" },
	{ "drop",           35, "drop" },
	{ "award",          36, "award" },
	{ "ai",             37, "ai" },
	{ "key",            38, "key" },
	{ "special",        39, "special [shot ID( Tomahawk:1, Cobra:2, Spike:3 )]" },
	{ "lowest",         40, "lowest" },
	{ "highest",        41, "highest" },
	{ "debugline",      42, "debugline [value]" },
	{ "god",            43, "god" },
	{ "togid",          44, "toggle id" },
	{ "power",          45, "power value[0-100]" },
	{ "impact",         46, "impact value[0-30]" },
	{ "control",        47, "control value[0-30]" },
	{ "spin",           48, "spin value[0-30]" },
	{ "curve",          49, "curve value[0-30]" },
	{ "scrmode",        50, "windowed, fullscreen toggle" },
	{ "bright",         51, "bright [value(0-1)] (1:default)" },
	{ "holecup",        52, "holecup [0-3] (0:random, 1-3:specific holecup location)" },
	{ "bounce",         53, "bounce [0-\xa1\xc4] (1:default)" },
	{ "roll",           54, "roll [0-\xa1\xc4] (1:default)" },
	{ "condition",      55, "condition [-50-50] (0:default)" },
	{ "temperature",    56, "temperature" },
	{ "humidity",       57, "humidity [0-100]" },
	{ "unbalance",      58, "unbalance impact control" },
	{ "green",          59, "green [value(0-\xa1\xc4)] (0.2:default)" },
	{ "camvel",         60, "camvel [value(0-\xa1\xc4)] (-1:default)" },
	{ "mouse",          61, "mouse [value(0.01-\xa1\xc4)] (1.0:default)" },
	{ "transparent",    62, "transparent (on:default)" },
	{ "plist",          63, "plist : part list of cur character" },
	{ "camtarget",      64, "camtarget [value(0-\xa1\xc4)] (100.0:defalut)" },
	{ "paint",          65, "paint mesh_index (-1:default)" },
	{ "speed",          66, "speed (5.0:default)" },
	{ "listbres",       67, "list bres" },
	{ "lake",           68, "lake speed alpha" },
	{ "showmethemoney", 69, "user pang and cookie is increase" },
	{ "gravity",        70, "gravity modifier [value]" },
	{ "rm_camera",      71, " camera display [limit(0: nolimit, 1: width, 2 : hight)][-90~90]" },
	{ "rm_furniture",   72, " furniture get  display [limit(0: name, 1: x , 2: y, 3:z)][-50~50, 0~10, -20~20]" },
	{ "pivot",          73, " camera posion %f, %f, %f " },
	{ "bar",            74, "input number [2~9]" },
	{ "tw cmd all",     75, "tw cmd all : \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xb8\xf0\xb5\xe7 \xc6\xae\xc0\xa7\xc4\xbf \xc3\xb5\xc1\xa6 \xb8\xed\xb7\xc9\xbe\xee\xb5\xe9 \xb8\xf1\xb7\xcf\xc0\xd4\xb4\xcf\xb4\xd9." },
	{ "tw list",        76, "tw list : \xc6\xae\xc0\xa7\xc4\xbf\xb7\xce \xb5\xee\xb7\xcf\xb5\xc8 \xc5\xac\xb7\xa1\xbd\xba \xb8\xf1\xb7\xcf\xc0\xbb \xc7\xa5\xbd\xc3 \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ "tw using",       77, "tw using : tw list\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xc5\xac\xb7\xa1\xbd\xba\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd4\xc0\xb8\xb7\xce\xbd\xe1, \xc0\xfc\xc3\xbc \xb8\xed\xb7\xc9\xbe\xee\xb0\xa1 \xbe\xc6\xb4\xd1 \xc7\xd8\xb4\xe7 \xc6\xae\xc0\xa7\xc4\xbf\xc0\xc7 \xb8\xed\xb7\xc9\xbe\xee\xb8\xa6 \xb9\xd9\xb7\xce \xc7\xd8\xbc\xae\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ "powerbar",       78, "powerbar setting!" },
	{ "posmask",        79, "posmask %d(TypeID), %d(PosMask)" },
	{ "texname",        80, "texname %d(TypeID), %d(0:normal tex 1:original tex), %d(tex num : 0 ~ 2), %s(tex name)" },
	{ "petname",        81, "petname %d(TypeID), %s(PetName)" },
	{ "guild",          82, "guild test" },
	{ "vectortype",     85, "vectortype [0-1]" },
	{ "vectorspeed",    86, "vectorspeed [%d]  default limitspeed 100" },
	{ "vectorheight",   87, "vectorheight [%d] default bound height 30" },
	{ "vectorcount",    88, "vectorcount [%d] default limit bound count 20" },
	{ "luacmd",         83, "luacmd [index]  index(0:reload,1:runfile \"test.lua\" 2:peras-only)" },
	{ "lua",            84, "lua \"command string\" " },
};
// clang-format on

extern float causticSpeed;
extern int causticAlpha;

extern WInputDev* g_ime;
extern Fresh* g_pFresh;

void PerformPowerBarSetting();

int _POSTMSG(const void* pSender, const char* pTarget, int msg, int param1,
	int param2, int param3, unsigned long param4)
{
	return AfxPostMsg(NULL, pTarget, msg, param1, param2, param3, param4);
}

CConsole::CConsole(void)
	: m_active(false),
	  m_keyStates(NULL),
	  m_history(32, 0),
	  m_lines(32, 0),
	  m_repeatDelay(5.0f)
{
}

CConsole::~CConsole(void)
{
	ShutDown();
}

void CConsole::Init(void)
{
	m_bsStack[0] = 0;
	m_input[0] = 0;

	m_keyStates = new ConsoleKeyState[0x59];
	for (unsigned int i = 0; i < 0x59; i++)
	{
		m_keyStates[i].m_pressed = 0;
	}

	m_alpha = 0.0f;

	m_inputLine[0] = 0;
	m_inputLocked = false;
	m_lineOffset = 0;
	m_state = 1;

	m_pFont = CChatMsg::Instance()->GetMaskedFont();
}

void CConsole::ShutDown(void)
{
	if (m_keyStates)
	{
		delete[] m_keyStates;
		m_keyStates = NULL;
	}

	for (char* line = m_history.Start(); line != NULL; line = m_history.Next())
	{
		if (line)
		{
			delete[] line;
		}
	}
	m_history.Reset();

	for (char* line = m_lines.Start(); line != NULL; line = m_lines.Next())
	{
		if (line)
		{
			delete[] line;
		}
	}
	m_lines.Reset();
}

void CConsole::Process(float elapsed)
{
	return;

	if (m_active)
	{
		m_alpha += elapsed * s_fadeSpeed;
		if (m_alpha > 1.0f)
		{
			m_alpha = 1.0f;
		}
		InputProcess(elapsed);
	}
	else
	{
		m_alpha -= elapsed * s_fadeSpeed;
		if (m_alpha < 0.0f)
		{
			m_alpha = 0.0f;
		}
	}
	g_input->SetActive(!m_active);
}

void CConsole::InputProcess(float elapsed)
{
	int key = 0;
	char text[4];
	char result[0x80];

	text[0] = 0;
	int ch = g_ime->GetState(3, 0);

	if (ch)
	{
		if (ch == 9)
		{
			key = ch;
		}
		else if (ch == 8 || ch == 13 || ch == 6 || ch == 7 || ch == 2 ||
			ch == 3 || ch == 4 || ch == 5)
		{
			key = ch;
			m_input[0] = 0;
		}
		else if (ch & 0x80)
		{
			text[0] = (char)ch;
			text[1] = (char)(ch >> 8);
			text[2] = 0;

			PopFromBsStack();
			PopFromBsStack();
			m_input[0] = 0;
		}
		else if (ch >= 0x20 && ch <= 0x7A)
		{
			text[0] = (char)ch;
			text[1] = 0;
			if (ch == 0x20)
			{
				m_bsStack[0] = 0;
			}
			else
			{
				PopFromBsStack();
			}
			m_input[0] = 0;
		}

		if (text[0])
		{
			if (m_inputLocked)
			{
				m_inputLine[strlen(m_inputLine) - 2] = 0;
				strcat(m_inputLine, text);
			}
			else
			{
				strcat(m_inputLine, text);
			}
			m_inputLine[0x7F] = 0;
		}

		m_inputLocked = false;
	}

	static int s_prevComp;
	int composition = g_ime->GetState(3, -2);
	if (composition)
	{
		if (composition != s_prevComp)
		{
			s_prevComp = composition;
			text[0] = (char)composition;
			text[1] = (char)(composition >> 8);
			text[2] = 0;

			if (m_inputLocked)
			{
				m_inputLine[strlen(m_inputLine) - 2] = 0;
				strcat(m_inputLine, text);
			}
			else
			{
				strcat(m_inputLine, text);
			}
			m_inputLine[0x7F] = 0;
			m_inputLocked = true;
		}
	}
	else if (m_inputLocked)
	{
		m_inputLine[strlen(m_inputLine) - 2] = 0;
		m_inputLocked = false;
	}

	switch (key)
	{
	case 9:
	{
		char* word = m_inputLine;
		if (!m_input[0])
		{
			strcpy(m_input + 1, m_inputLine);
			m_input[0] = 1;
			char* space = strstr(m_inputLine, " ");
			while (space)
			{
				word = space + 1;
				strcpy(m_input + 1, word);
				space = strstr(word, " ");
			}

			for (unsigned int i = 0; i < 0x59; i++)
			{
				m_keyStates[i].m_pressed = 0;
			}
		}

		if (word == m_inputLine)
		{
			if (BrowseCommand(m_input + 1, result) > -1)
			{
				strcpy(word, result);
			}
		}
		else
		{
			char* end = strchr(word, 0);
			if (isalnum(end[-1]))
			{
				char* dst = end;
				while ((*dst = PopFromBsStack()) != 0)
				{
					dst++;
				}
			}
			else
			{
				char c;
				while ((c = PopFromBsStack()) != 0)
				{
					if (isalnum(c))
					{
						char* dst = end;
						*dst = c;
						while ((*++dst = PopFromBsStack()) != 0)
						{
						}
						break;
					}
				}
			}
			m_input[0] = 0;
		}
		break;
	}
	case 13:
		m_bsStack[0] = 0;
		if (strlen(m_inputLine))
		{
			char* line = new char[strlen(m_inputLine) + 1];
			strcpy(line, m_inputLine);
			m_history += line;
			AddText(MakeStr(">%s", line));

			if (!TweakerCommandProc(m_inputLine))
			{
				Command(strlwr(m_inputLine));
			}

			m_inputLine[0] = 0;
			m_lineOffset = Max(0, m_lines.GetCount() - 19);
			m_historyPos = NULL;
			m_historyEdge = 1;
		}
		break;
	case 6:
		if (m_historyEdge >= 0 && m_history.GetCount() > 0)
		{
			m_historyEdge = 0;
			if (m_historyPos)
			{
				char* entry = m_history.Prev(m_historyPos);
				if (m_historyPos)
				{
					strcpy(m_inputLine, entry);
				}
				else
				{
					m_historyEdge = -1;
					m_history.Start(m_historyPos);
				}
			}
			else
			{
				char* entry = m_history.End(m_historyPos);
				strcpy(m_inputLine, entry);
			}
		}
		break;
	case 7:
		if (m_historyEdge <= 0 && m_history.GetCount() > 0)
		{
			m_historyEdge = 0;
			if (m_historyPos)
			{
				char* entry = m_history.Next(m_historyPos);
				if (m_historyPos)
				{
					strcpy(m_inputLine, entry);
				}
				else
				{
					m_historyEdge = 1;
					m_inputLine[0] = 0;
				}
			}
		}
		break;
	case 8:
	{
		int len = strlen(m_inputLine);
		if (len > 0 && !m_inputLocked)
		{
			if (len > 1 && (m_inputLine[len - 1] & 0x80))
			{
				PushIntoBsStack(m_inputLine[len - 1]);
				PushIntoBsStack(m_inputLine[len - 2]);
				m_inputLine[len - 2] = 0;
			}
			else
			{
				if (m_inputLine[len - 1] == ' ')
				{
					m_bsStack[0] = 0;
				}
				else
				{
					PushIntoBsStack(m_inputLine[len - 1]);
				}
				m_inputLine[len - 1] = 0;
			}
		}
		break;
	}
	case 2:
		if (m_lines.GetCount() > 19)
		{
			m_lineOffset = Max(0, m_lineOffset - 1);
		}
		break;
	case 3:
		if (m_lines.GetCount() > 19)
		{
			m_lineOffset = Min(m_lineOffset + 1, m_lines.GetCount() - 19);
		}
		break;
	}
}

void CConsole::Display(void)
{
	if (Wabs(m_alpha) < g_EPSILON)
	{
		return;
	}

	WOverlay::DrawBox(g_view, WRect(0, 0, g_view->GetWidth(), s_consoleHeight),
		0, (int)(m_alpha * 128.0f) << 24, 0.001f);

	int count = m_lines.GetCount();
	int empty = (19 - count > 0) ? 19 - count : 0;

	float y = empty * 15.0f + 5.0f;
	int alpha = (int)(m_alpha * 255.0f);
	int i = 0;
	for (char* line = m_lines.Start(); line != NULL; line = m_lines.Next(), i++)
	{
		if (i == m_lineOffset + 19)
		{
			break;
		}

		if (i >= m_lineOffset)
		{
			m_pFont->Print(g_view, 6.0f, y + 1.0f, line, 0, alpha << 24, NULL);
			m_pFont->Print(g_view, 5.0f, y, line, 0, (alpha << 24) | 0xFFFFFF,
				NULL);
			y += 15.0f;
		}
	}

	float width = m_pFont->Print(g_view, 5.0f, y, MakeStr(">%s", m_inputLine),
		0, (alpha << 24) | 0xFFFFFF, NULL);
	m_pFont->Flush(g_view);

	WOverlay::DrawBox(g_view, WRect(width + 5.0f, y + 10.0f, 7, 2), 0,
		(alpha << 24) | 0xFFFFFF, 0.001f);
	WOverlay::DrawBox(g_view, WRect(g_view->GetWidth() - 12.0f, 5, 10, 292), 0,
		(alpha << 24) | 0x404040, 0.001f);

	count = m_lines.GetCount();
	if (count <= 0 || count <= 19)
	{
		return;
	}

	WOverlay::DrawBox(g_view,
		WRect(g_view->GetWidth() - 10.0f, m_lineOffset * 292.0f / count + 5.0f,
			6.0f, 292.0f * 19 / count),
		0, (alpha << 24) | 0x707070, 0.001f);
}

bool CConsole::PushIntoBsStack(char c)
{
	int len = strlen(m_bsStack);

	if (len < 0x7F)
	{
		m_bsStack[len] = c;
		m_bsStack[len + 1] = 0;
		return true;
	}

	return false;
}

char CConsole::PopFromBsStack(void)
{
	int len = strlen(m_bsStack);
	if (len == 0)
	{
		return 0;
	}

	char c = m_bsStack[len - 1];
	m_bsStack[len - 1] = 0;
	return c;
}

void CConsole::ClearBsStack(void)
{
	m_bsStack[0] = 0;
}

int CConsole::BrowseCommand(const char* partial, char* result)
{
	unsigned int i;
	for (i = 0; i < 0x59; i++)
	{
		m_keyStates[i].m_code = 1;
	}

	int len = strlen(partial);

	int matched;
	for (matched = 0; matched < len; matched++)
	{
		bool done = true;

		for (i = 0; i < 0x59; i++)
		{
			if (m_keyStates[i].m_code)
			{
				int nameLen = strlen(command_table[i].name);

				if (nameLen > matched &&
					partial[matched] == command_table[i].name[matched])
				{
					done = false;
				}
				else
				{
					m_keyStates[i].m_code = 0;
				}
			}
		}

		if (done)
		{
			break;
		}
	}

	bool allBrowsed = true;
	for (i = 0; i < 0x59; i++)
	{
		if (m_keyStates[i].m_code)
		{
			if (!m_keyStates[i].m_pressed)
			{
				allBrowsed = false;
				break;
			}
		}
	}

	if (allBrowsed)
	{
		for (i = 0; i < 0x59; i++)
		{
			m_keyStates[i].m_pressed = 0;
		}
	}

	int best = -1;

	for (i = 0; i < 0x59; i++)
	{
		if (m_keyStates[i].m_code && !m_keyStates[i].m_pressed)
		{
			int nameLen = strlen(command_table[i].name);

			if (nameLen <= matched)
			{
				if (result)
				{
					strcpy(result, command_table[i].name);
				}

				return i;
			}

			if (best == -1 ||
				command_table[best].name[matched] >
					command_table[i].name[matched])
			{
				best = i;
			}
		}
	}

	if (best == -1)
	{
		*result = 0;
		return -1;
	}

	m_keyStates[best].m_pressed = 1;
	if (result)
	{
		strcpy(result, command_table[best].name);
	}

	return command_table[best].id;
}

int CConsole::TweakerCommandProc(const char* line)
{
	static tweaker* s_pUsingTweaker;
	if (stricmp(line, "tw cmd all") == 0)
	{
		std::vector<std::string> list;
		tweaker_database::instance()->get_tweaker_command_all(list);
		if (list.empty())
		{
			AddText(
				"\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xc6\xae\xc0\xa7\xc4\xbf \xb8\xed\xb7\xc9\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return 1;
		}

		std::vector<std::string>::iterator it;
		it = list.begin();
		for (; it != list.end(); ++it)
		{
			AddText((*it).c_str());
		}

		return 1;
	}

	if (stricmp(line, "tw list") == 0)
	{
		std::vector<std::string> list;
		tweaker_database::instance()->get_tweaker_names(list);

		if (list.empty())
		{
			AddText(
				"\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xc6\xae\xc0\xa7\xc4\xbf\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return 1;
		}

		for (std::vector<std::string>::iterator it = list.begin();
			it != list.end(); ++it)
		{
			AddText((*it).c_str());
		}

		return 1;
	}

	if (strncmp(line, "tw using", 8) == 0)
	{
		char name[128] = { 0 };
		char command[128] = { 0 };
		_snscanf(line + 9, 127, "%s %s", name, command);
		std::string fullName(name);
		fullName += " ";
		fullName += command;
		s_pUsingTweaker = tweaker_database::instance()->get_tweaker(fullName);
		if (s_pUsingTweaker)
		{
			AddText(
				"\xc0\xd4\xb7\xc2\xb5\xc8 \xc6\xae\xc0\xa7\xc4\xbf\xb8\xa6 \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9. \xb4\xd9\xc0\xbd\xc0\xba \xbf\xb9\xbe\xe0\xb5\xc8 \xb8\xed\xb7\xc9\xbe\xee\xb5\xe9 \xc0\xd4\xb4\xcf\xb4\xd9.");
			std::vector<std::string> list;
			s_pUsingTweaker->get_command_list(list, false);

			for (std::vector<std::string>::iterator it = list.begin();
				it != list.end(); ++it)
			{
				AddText((*it).c_str());
			}
		}
		else
		{
			AddText(
				"\xc6\xae\xc0\xa7\xc4\xbf\xb0\xa1 \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.(tw list\xb8\xa6 \xc5\xeb\xc7\xd8 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc5 \xc8\xc4 \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xbc\xbf\xe4.)");
		}

		return 1;
	}

	if (s_pUsingTweaker)
	{
		if (tweaker_database::instance()->is_exist_tweaker(s_pUsingTweaker))
		{
			if (s_pUsingTweaker->is_exist_command(line))
			{
				std::string result = s_pUsingTweaker->run_command(line);
				AddText(result.c_str());
				return 1;
			}
		}
		else
		{
			AddText(
				"\xc0\xcc\xc0\xfc \xbb\xe7\xbf\xeb\xc0\xbb \xbc\xb1\xbe\xf0\xc7\xd1 \xc6\xae\xc0\xa7\xc4\xbf\xb0\xa1 \xb4\xf5\xc0\xcc\xbb\xf3 \xc0\xaf\xc8\xbf\xc7\xcf\xc1\xf6 \xbe\xca\xbe\xc6\xbc\xad \xc0\xda\xb5\xbf \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
			s_pUsingTweaker = NULL;
		}
	}
	else
	{
		if (tweaker_database::instance()->is_exist_command(line))
		{
			std::string result =
				tweaker_database::instance()->send_command(line);

			AddText(result.c_str());
			return 1;
		}
	}

	return 0;
}

void CConsole::Command(const char* line)
{
	for (unsigned char i = 0; i < 0x59; i++)
	{
		int len = strlen(command_table[i].name);
		if (strncmp(line, command_table[i].name, len) == 0 &&
			(line[len] == ' ' || line[len] == '\0'))
		{
			int id = command_table[i].id;

			switch (id)
			{
			case 66:
			{
				float speed;
				sscanf(line + len + 1, "%f", &speed);

				if (speed < 1.0f)
				{
					BadArgument(66);
					return;
				}
				m_repeatDelay = speed;
				return;
			}

			case 65:
			{
				int triangle = -1;
				sscanf(line + len + 1, "%d", &triangle);

				GetPVS().PaintIndex(triangle);
				return;
			}

			case 64:
			{
				int target = -1;
				sscanf(line + len + 1, "%d", &target);

				if (target <= 0)
				{
					BadArgument(64);
					return;
				}
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x9D, target,
					0, 0, 0);
				return;
			}

			case 63:
			{
				CPartTidList& tids =
					GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);
				std::map<unsigned int, IFF_STRUCT::sPart>& parts =
					ItemManager()->m_PartMap;

				FILE* fp = fopen("PartList.txt", "wt");
				int index = 1;

				std::map<unsigned int, IFF_STRUCT::sPart>::iterator part;
				for (part = parts.begin(); part != parts.end(); ++part)
				{
					unsigned long tid = (*part).second.c.TypeId;
					if ((tids.m_charTid & 0x3FFFFFF) == ((tid >> 18) & 0xFF) &&
						(tid & 0x600) == 0)
					{
						fprintf(fp, "%d(%2d): %s\n", tid, index,
							(*part).second.c.Name);
						AddText(MakeStr("%d(%2d): %s", (*part).second.c.TypeId,
							index, (*part).second.c.Name));
						index++;
					}
				}

				std::map<unsigned int, IFF_STRUCT::sAuxPart>& auxParts =
					ItemManager()->m_AuxPartMap;
				std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator aux;
				for (aux = auxParts.begin(); aux != auxParts.end(); ++aux)
				{
					fprintf(fp, "%d(%2d): %s\n", (*aux).second.c.TypeId, index,
						(*aux).second.c.Name);
					AddText(MakeStr("%d(%2d): %s", (*aux).second.c.TypeId,
						index, (*aux).second.c.Name));
					index++;
				}

				fclose(fp);
				return;
			}

			case 62:
				AddText(CSceneManager::Instance()->ToggleTransparent()
						? "Transparent ON"
						: "Transparent OFF");
				return;

			case 61:
			{
				float sensitivity = 0.0f;
				sscanf(line + len + 1, "%f", &sensitivity);

				if (sensitivity < 0.01f)
				{
					BadArgument(61);
					return;
				}

				g_input->SetSensitivity(sensitivity);
				return;
			}

			case 60:
			{
				int velocity = -1;
				sscanf(line + len + 1, "%d", &velocity);

				if (velocity < 0)
				{
					BadArgument(60);
					return;
				}
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x9B, velocity,
					0, 0, 0);
				return;
			}

			case 59:
			{
				float slope = -1.0f;
				sscanf(line + len + 1, "%f", &slope);

				if (slope < 0.0f)
				{
					BadArgument(59);
					return;
				}
				AddText(
					MakeStr("%.2f->%.2f", GetPVS().GetGreenFactor(), slope));

				GetPVS().SetGreenFactor(slope);
				GetPVS().CalcGreenPlane();
				return;
			}

			case 58:
			{
				float unbalance[2];
				sscanf(line + len + 1, "%f %f", &unbalance[0], &unbalance[1]);

				if (unbalance[0] > 0.0f)
				{
					AddText(MakeStr("impact : %.1f->%.1f",
						GOLFDOC()->m_timeScale, unbalance[0]));
					GOLFDOC()->m_timeScale = unbalance[0];

					GolfClub().SetAccuracy();
				}

				if (unbalance[1] > 0.0f)
				{
					AddText(MakeStr("control : %.1f->%.1f",
						GOLFDOC()->m_maxTimeScale, unbalance[1]));
					GOLFDOC()->m_maxTimeScale = unbalance[1];
				}
			}

				return;

			case 55:
			{
				float condition;
				sscanf(line + len + 1, "%f", &condition);

				if (condition < -50.0f || condition > 50.0f)
				{
					BadArgument(55);
				}
				else
				{
					GetPVS().SetCondition(condition);
					CTaskManager::Instance()->PostMsg(0, "Screen", 0x1A3, 0, 0,
						0, 0);
				}

				return;
			}

			case 53:
			{
				float bounce;
				sscanf(line + len + 1, "%f", &bounce);

				if (bounce < 0.0f)
				{
					BadArgument(53);
					return;
				}
				GetPVS().SetBounceFactor(bounce);
				return;
			}

			case 54:
			{
				float roll;
				sscanf(line + len + 1, "%f", &roll);

				if (roll < 0.0f)
				{
					BadArgument(54);
					return;
				}
				GetPVS().SetRollFactor(roll);
				return;
			}

			case 52:
			{
				int pin = -1;
				sscanf(line + len + 1, "%d", &pin);

				if (pin >= 0 && pin <= 3)
				{
					for (unsigned char hole = 0; hole < 18; hole++)
					{
						CSharedDoc::Instance()->m_courseMap[hole] =
							pin == 0 ? rand() % 3 : pin - 1;
					}

					return;
				}
				BadArgument(52);
				return;
			}

			case 51:
			{
				float bright;
				sscanf(line + len + 1, "%f", &bright);

				if (bright < 0.0f || bright > 1.0f)
				{
					BadArgument(51);
					return;
				}
				GOLFDOC()->UpdateBright(bright);
				return;
			}

			case 45:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 100)
				{
					AddText(MakeStr("power : Lv%d->Lv%d",
						(int)PLAYER(GOLFDOC()->m_currentPlayer)->power + 15,
						level));
					level -= 15;

					PLAYER(GOLFDOC()->m_currentPlayer)->power = level;

					CTaskManager::Instance()->PostMsg(0, "Spot", 0xF5, 0, 0, 0,
						0);
					GolfClub().SetAccuracy();
					return;
				}
				BadArgument(45);
				return;
			}

			case 46:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30)
				{
					AddText(MakeStr("impact : Lv%d->Lv%d",
						(int)PLAYER(GOLFDOC()->m_currentPlayer)->accuracy,
						level));

					PLAYER(GOLFDOC()->m_currentPlayer)->accuracy = level;
					GolfClub().SetAccuracy();
					return;
				}
				BadArgument(46);
				return;
			}

			case 47:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30)
				{
					AddText(MakeStr("control : Lv%d->Lv%d",
						(int)PLAYER(GOLFDOC()->m_currentPlayer)->control,
						level));

					PLAYER(GOLFDOC()->m_currentPlayer)->control = level;
					return;
				}
				BadArgument(47);
				return;
			}

			case 48:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30)
				{
					AddText(MakeStr("spin : Lv%d->Lv%d",
						(int)PLAYER(GOLFDOC()->m_currentPlayer)->spin, level));

					PLAYER(GOLFDOC()->m_currentPlayer)->spin = level;
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x271A, 0, 0,
						0, 0);
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x163, 0, 0, 0,
						0);
					return;
				}
				BadArgument(48);
				return;
			}

			case 49:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30)
				{
					AddText(MakeStr("curve : Lv%d->Lv%d",
						(int)PLAYER(GOLFDOC()->m_currentPlayer)->curve, level));

					PLAYER(GOLFDOC()->m_currentPlayer)->curve = level;
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x271A, 0, 0,
						0, 0);
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x163, 0, 0, 0,
						0);
					return;
				}
				BadArgument(49);
				return;
			}

			case 50:
				COption::Instance()->vToggleFullscreen();
				return;

			case 44:
				if (CHeadIcon::Instance())
				{
					CHeadIcon::Instance()->Toggle();
				}

				return;

			case 43:
				GOLFDOC()->m_bGodMode = !GOLFDOC()->m_bGodMode;
				PerformPowerBarSetting();
				AddText(GOLFDOC()->m_bGodMode ? "You are the God now! (~+_+)~"
											  : "You are a man. T_T");
				return;

			case 42:
			{
				int flags;
				sscanf(line + len + 1, "%d", &flags);

				if (CSceneManager::Instance())
				{
					CSceneManager::Instance()->m_debugDraw.bits = flags;
				}

				return;
			}

			case 40:
				if (CSceneManager::Instance())
				{
					if ((unsigned char)(CSceneManager::Instance()
											->m_debugDraw.bits >>
							4) &
						1)
					{
						CSceneManager::Instance()->m_debugDraw.bits &= ~0x10u;
						return;
					}
					CSceneManager::Instance()->m_debugDraw.bits |= 0x10u;
				}

				return;

			case 41:
				if (CSceneManager::Instance())
				{
					if ((unsigned char)(CSceneManager::Instance()
											->m_debugDraw.bits >>
							5) &
						1)
					{
						CSceneManager::Instance()->m_debugDraw.bits &= ~0x20u;
						return;
					}
					CSceneManager::Instance()->m_debugDraw.bits |= 0x20u;
				}

				return;

			case 38:
				g_input->IncludeKey(0);
				return;

			case 37:
				if (IsMyTurn(true) && (GOLFDOC()->GameMode() & ~0x402) &&
					CSharedDoc::Instance()->m_playingMode == 1)
				{
					unsigned char index;

					if (WNetworkSystem::Instance()->IsConnected(
							WNetworkSystem::NET_GAME))
					{
						index = MyIndex();
					}
					else
					{
						index = GOLFDOC()->m_currentPlayer;
					}

					if (CSharedDoc::Instance()
							->m_userInfo[index]
							.info.dwIdentity &
						1)
					{
						AddText("AUTO PLAYER OFF");
					}
					else
					{
						AddText("AUTO PLAYER ON");
					}

					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x90,
						index, 0, 0, 0);
				}

				return;

			case 35:
				CSceneManager::Instance()->DropTest();
				return;

			case 33:
			{
				int size;
				sscanf(line + len + 1, "%d", &size);
				CTaskManager::Instance()->PostMsg(0, "Grid", 0x1D3, size, 0, 0,
					0);
				return;
			}

			case 34:
			{
				int show;
				sscanf(line + len + 1, "%d", &show);

				if (show == 1)
				{
					CTaskManager::Instance()->PostMsg(0, "Grid", 0x163, 0, 0, 0,
						0);
				}
				else
				{
					CTaskManager::Instance()->PostMsg(0, "Grid", 0x164, 0, 0, 0,
						0);
				}

				return;
			}

			case 32:
			{
				int frame;
				sscanf(line + len + 1, "%d", &frame);
				if (frame < 0 || frame > GolfBall().m_renderNum)
				{
					BadArgument(32);
					return;
				}
				GolfBall().m_pos = GolfBall().PreData(frame)->pos;
				return;
			}

			case 27:
				GOLFDOC()->m_pPolySoup->MakeStatistics();
				return;

			case 22:
			{
				char name[64];
				sscanf(line + len + 1, "%s", name);
				if (g_resrcmng->FindTexture(name) == NULL)
				{
					AddText("Such Texture Is Not Found");
				}

				return;
			}

			case 21:
			{
				float slope;
				sscanf(line + len + 1, "%f", &slope);
				if (slope < 0.0f)
				{
					BadArgument(21);
					return;
				}
				GetPVS().SetSlopeFactor(slope);
				return;
			}

			case 18:
			{
				if (COption::Instance()->vGetTnLMode())
				{
					AddText("SoftRender is required");
					return;
				}

				float distance;
				sscanf(line + len + 1, "%f", &distance);
				GOLFDOC()->m_pPolySoup->SetCullDistance(distance * 3.2f);
				return;
			}

			case 17:
			{
				Waabb box = GOLFDOC()->m_pPolySoup->FindModel(NULL)->aabb;
				AddText(
					MakeStr("%.0f %.0f %.0f", (box.max.x - box.min.x) * 0.3125f,
						(box.max.y - box.min.y) * 0.3125f,
						(box.max.z - box.min.z) * 0.3125f));
				return;
			}

			case 16:
			{
				float direction = Wind().GetGlobalDirection();
				float intensity;
				int count =
					sscanf(line + len + 1, "%f %f", &intensity, &direction);

				if (intensity > 10.0f)
				{
					BadArgument(16);
					return;
				}

				Wind().SetIntensity(floorf(intensity));
				Wind().SetDirection(
					count == 2 ? direction * g_DEGTORAD : direction);
				GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
				if (WSingleton<CSceneManager>::Instance()->m_pWeather != NULL)
				{
					WSingleton<CSceneManager>::Instance()
						->m_pWeather->UpdateWind();
				}

				return;
			}

			case 71:
				if (IsLocalContent((localContentType_t)0x56))
				{
					int limit = -1;
					float angle1;
					float angle2;
					sscanf(line + len + 1, " %d %f %f", &limit, &angle1,
						&angle2);
					_POSTMSG(this, "RealMyRoom", 0x209, limit, (int)angle1,
						(int)angle2, 0);
				}

				return;

			case 72:
				if (IsLocalContent((localContentType_t)0x56))
				{
					char* name = new char[0x20];
					float x;
					float y;
					float z;
					sscanf(line + len + 1, "%s %f %f %f", name, &x, &y, &z);

					if (*name != '\0')
					{
						_POSTMSG(this, "RealMyRoom", 0x20A, (int)x, (int)y,
							(int)name, 0);
					}
				}

				return;

			case 73:
				if (IsLocalContent((localContentType_t)0x56))
				{
					WVector pos;
					sscanf(line + len + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
					_POSTMSG(this, "RealMyRoom", 0x22B, (int)pos.x, (int)pos.y,
						(int)pos.z, 0);
				}

				return;

			case 14:
			{
				char name[32];
				sscanf(line + len + 1, "%s", name);
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x85,
					(intptr_t)name, 0, 0, 0);
				return;
			}

			case 15:
			{
				char* name = new char[0x20];
				sscanf(line + len + 1, "%s", name);

				if (fopen(MakeStr("%s.sav", name), "rb") != NULL)
				{
					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x86,
						(intptr_t)name, 0, 0, 0);
					return;
				}
				AddText("No Save File");
				return;
			}

			case 13:
			{
				int player = 0;
				while (player < GOLFDOC()->GetPlayerNum())
				{
					char text[64];
					if (GOLFDOC()->GetPlayer(player)->state != 0)
					{
						sprintf(text, "%d : -", player);
					}
					else
					{
						WVector diff =
							GetHoleCupPos() - GOLFDOC()->GetPlayer(player)->pos;
						diff.y = 0;
						float dist = diff.Magnitude();
						sprintf(text, "%d : %.2fy(%.2f)", player,
							dist * 0.3125f, dist);
					}
					AddText(text);
					player++;
				}

				return;
			}

			case 0:
				for (unsigned int k = 1; k < 0x59; k++)
				{
					AddText(command_table[k].usage);
				}

				return;

			case 1:
			{
				char* text = m_lines.Start();
				while (text != NULL)
				{
					if (text)
					{
						delete text;
					}
					text = m_lines.Next();
				}
				m_lines.Reset();

				m_lineOffset = 0;
				return;
			}

			case 2:
			{
				int serial;
				int level;
				int cadItem;
				int count = sscanf(line + len + 1, "%d %d %d", &serial, &level,
					&cadItem);

				if (count >= 3 && count != EOF && level >= 1 && level <= 4)
				{
					unsigned long caddieTypeId;
					bool valid = true;

					if (serial == -1)
					{
						caddieTypeId = 0;
					}
					else
					{
						caddieTypeId = serial | 0x1C000000;
						if (ItemManager()->FindCaddie(caddieTypeId) == NULL)
						{
							valid = false;
						}
						else if (caddieTypeId != 0 && cadItem != 0 &&
							((((unsigned long)cadItem >> 21) & 0x1F) ^
								caddieTypeId) &
								0x3FFFFFF)
						{
							valid = false;
						}
					}

					if (valid && cadItem != 0 &&
						ItemManager()->FindCadItem(cadItem) == NULL)
					{
						valid = false;
					}

					if (valid)
					{
						_POSTMSG(this, "Caddie", 0x4A, caddieTypeId, level - 1,
							cadItem, 0);
						return;
					}
				}
				AddText(
					"caddie [serial no.] [level(1-4)] [caddie item typeid]");
				return;
			}

			case 3:
			{
				int map = -1;
				sscanf(line + len + 1, "%d", &map);

				if (map == -1)
				{
					for (int course = 0; course < ItemManager()->GetCourseNum();
						course++)
					{
						if (course >= ItemManager()->GetCourseNum())
						{
							continue;
						}
						char text[60];
						int number = (course > 0x7F && course != 0xFD)
							? course - 0x80
							: course;
						IFF_STRUCT::sCourse* info =
							ItemManager()->FindCourse(0x28000000 | number);
						sprintf(text, "%d:%s", course, info->c.Name);
						AddText(text);
					}

					return;
				}

				if (map == GetCurMap())
				{
					return;
				}

				if (SetCurMap(map))
				{
					g_audio->StopSfx(NULL);

					for (int hole = 1; hole <= 18; hole++)
					{
						GOLFDOC()->GetHoleData(hole).playerNum = 0;
						GOLFDOC()->GetHoleData(hole).result = 3;
						GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
					}

					CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x140, 0, 0,
						0, 0);
					CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x13E, 0, 0,
						0, 0);
				}
				else
				{
					AddText("map [serial no.]");
				}

				return;
			}

			case 4:
			{
				int hole;
				sscanf(line + len + 1, "%d", &hole);

				if (hole == GOLFDOC()->m_currentHole)
				{
					return;
				}

				if (hole >= 1 && hole <= 18)
				{
					g_audio->StopSfx(NULL);

					GOLFDOC()->m_currentHole = hole;
					CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x140, 0, 0,
						0, 0);
				}
				else
				{
					AddText("Invalid Argument");
				}

				return;
			}

			case 5:
			{
				int weather;
				sscanf(line + len + 1, "%d", &weather);

				if (weather > 3)
				{
					BadArgument(5);
					return;
				}

				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x142, weather,
					0, 0, 0);

				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x13D, weather,
					0, 0, 0);
				if (weather < 2)
				{
					GOLFDOC()->m_pSky->GenerateStars(-1);
					GOLFDOC()->m_pSky->GenerateLensFlare();
					GOLFDOC()->m_pSky->GenerateMoons();
				}

				CTaskManager::Instance()->PostMsg(0, "Screen", 0x1A3, 0, 0, 0,
					0);
				return;
			}

			case 6:
			{
				int amount;
				sscanf(line + len + 1, "%d", &amount);
				return;
			}

			case 7:
			{
				int clubSet = 0;
				sscanf(line + len + 1, "%d", &clubSet);

				AfxPostMsg(0, "Player", 0x11C, clubSet | 0x10000000, 1, 0, 0);
				return;
			}

			case 8:
			{
				float combo;
				sscanf(line + len + 1, "%f", &combo);

				if (CSharedDoc::Instance()->m_golfGame.gameType == 1)
				{
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
						->gauge = Between(0.0f,
						TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge +
							combo,
						CSharedDoc::Instance()->GetComboGaugeLimit(
							PLAYER(GOLFDOC()->m_currentPlayer)->team));
					return;
				}
				PLAYER(GOLFDOC()->m_currentPlayer)->gauge = Between(0.0f,
					PLAYER(GOLFDOC()->m_currentPlayer)->gauge + combo,
					CSharedDoc::Instance()->GetComboGaugeLimit(
						GOLFDOC()->m_currentPlayer));
				return;
			}

			case 9:
				CSceneManager::Instance()->ShowTag();
				return;

			case 10:
				if (COption::Instance()->vGetTnLMode() != 0)
				{
					int fillMode;
					sscanf(line + len + 1, "%d", &fillMode);

					g_resrcmng->VideoReference()->Command(WX_VDEV_FILLMODE,
						fillMode, 0);
					return;
				}
				else
				{
					int mode;
					sscanf(line + len + 1, "%d", &mode);

					if (mode >= 0 && mode <= 3)
					{
						CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x118,
							mode, 0, 0, 0);
						CTaskManager::Instance()->PostMsg(0, "Player", 0x118,
							mode, 0, 0, 0);
						CTaskManager::Instance()->PostMsg(0, "Caddie", 0x118,
							mode, 0, 0, 0);
						return;
					}
					BadArgument(10);
					return;
				}

			case 11:
			{
				int fillMode;
				if (sscanf(line + len + 1, "%d", &fillMode) != 1 ||
					fillMode < 0 || fillMode > 2)
				{
					AddText("fill [value(0-2)]");
					return;
				}

				g_resrcmng->VideoReference()->Command(WX_VDEV_FILLMODE,
					fillMode, 0);
				return;
			}

			case 12:
			{
				int fov;
				sscanf(line + len + 1, "%d", &fov);

				if (fov >= 10 && fov <= 120)
				{
					g_view->SetFOV(fov * g_DEGTORAD);
					return;
				}
				BadArgument(12);
				return;
			}

			case 19:
				if (CSharedDoc::Instance()->m_golfGame.shotTimeLimit == 0)
				{
					CSharedDoc::Instance()->m_golfGame.shotTimeLimit = 40000;
					GOLFDOC()->m_remainTime = 40000;
					AddText("TIME LIMIT ON");
					return;
				}
				CSharedDoc::Instance()->m_golfGame.shotTimeLimit = 0;
				AddText("TIME LIMIT OFF");
				return;

			case 20:
			{
				bool checking = CCameraWork::Instance()->ToggleCameraChecking();

				AddText(
					MakeStr(" CameraChecking: %s", checking ? "on" : "off"));
			}

				return;

			case 23:
			{
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 2)
				{
					COption::Instance()->vSetParticleLevel(level);
					return;
				}
				BadArgument(23);
				return;
			}

			case 24:
			{
				int filter;
				if (sscanf(line + len + 1, "%d", &filter) != 0 && filter >= 0 &&
					filter <= 5)
				{
					g_resrcmng->VideoReference()->Command(
						W_VDEV_MIP_TEXSTAGE_FILTER, filter, 0);
					return;
				}
				BadArgument(24);
				AddText("0: D3DTEXF_NONE");
				AddText("1: D3DTEXF_POINT");
				AddText("2: D3DTEXF_LINEAR");
				AddText("3: D3DTEXF_ANISOTROPIC");
				AddText("4: D3DTEXF_FLATCUBIC");
				AddText("5: D3DTEXF_GAUSSIANCUBIC");
				return;
			}

			case 25:
			{
				char cmd[32];
				char name[256] = { 0 };
				sscanf(line, "%s %s", cmd, name);

				if (name[0] == 0)
				{
					BadArgument(25);
					return;
				}

				int index = atoi(name);
				CPartTidList& tids =
					GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);
				int count = 1;

				std::map<unsigned int, IFF_STRUCT::sPart>::iterator part;
				for (part = ItemManager()->m_PartMap.begin();
					part != ItemManager()->m_PartMap.end(); ++part)
				{
					unsigned long tid = (*part).second.c.TypeId;
					if ((tids.m_charTid & 0x3FFFFFF) == ((tid >> 18) & 0xFF) &&
						(tid & 0x600) == 0)
					{
						if (count == index)
						{
							CTaskManager::Instance()->PostMsg(0, "Player",
								0x117, (*part).second.c.TypeId, 0, 0, 0);
							return;
						}
						count++;
					}
				}

				std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator aux;
				for (aux = ItemManager()->m_AuxPartMap.begin();
					aux != ItemManager()->m_AuxPartMap.end(); ++aux)
				{
					if (count == index)
					{
						CTaskManager::Instance()->PostMsg(0, "Player", 0x117,
							(*aux).second.c.TypeId, 0, 0, 0);
						return;
					}
					count++;
				}

				AddText(MakeStr("can't find %s", name));
				return;
			}

			case 26:
			{
				char cmd[32];
				char arg[32] = { 0 };
				sscanf(line, "%s %s", cmd, arg);

				if (arg[0] == 0)
				{
					BadArgument(26);
					return;
				}

				unsigned long typeId = strtoul(arg, 0, 10);
				CPartTidList& tids =
					GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);
				std::map<unsigned int, IFF_STRUCT::sPart>& parts =
					ItemManager()->m_PartMap;
				std::map<unsigned int, IFF_STRUCT::sAuxPart>& auxParts =
					ItemManager()->m_AuxPartMap;

				std::map<unsigned int, IFF_STRUCT::sPart>::iterator part;
				for (part = parts.begin(); part != parts.end(); ++part)
				{
					if ((*part).second.c.TypeId == typeId)
					{
						if (((tids.m_charTid ^
								 (((*part).second.c.TypeId >> 18) & 0xFF)) &
								0x3FFFFFF) == 0)
						{
							CTaskManager::Instance()->PostMsg(0, "Player",
								0x117, (*part).second.c.TypeId, 0, 0, 0);
							return;
						}
						AddText(
							MakeStr("wrong character's typeid : %d", typeId));
					}
				}

				std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator aux;
				for (aux = auxParts.begin(); aux != auxParts.end(); ++aux)
				{
					if ((*aux).second.c.TypeId == typeId)
					{
						CTaskManager::Instance()->PostMsg(0, "Player", 0x117,
							(*aux).second.c.TypeId, 0, 0, 0);
						return;
					}
				}

				AddText(MakeStr("can't find %s", arg));
				return;
			}

			case 28:
			{
				int area = -1;
				sscanf(line + len + 1, "%d", &area);
				if (area < 1)
				{
					BadArgument(28);
					return;
				}
				AddText(MakeStr(" Pangya Area: x%d", area));
				CPowerGauge::Instance()->SetDebugPangYaArea((float)area);
				return;
			}

			case 29:
			{
				int area = -1;
				sscanf(line + len + 1, "%d", &area);
				if (area < 1)
				{
					BadArgument(29);
					return;
				}
				AddText(MakeStr("AirNight Lucky Pangya Area: %d", area));
				CPowerGauge::Instance()->SetDebugAirNightLuckyPangYaArea(
					(float)area);
				return;
			}

			case 30:
			{
				int serial;
				int count = sscanf(line + len, "%d", &serial);
				if (count == 0 || count == EOF)
				{
					AddText("ball [serial no.]");
					return;
				}
				unsigned long ballId = serial | 0x14000000;
				CTaskManager::Instance()->PostMsg(0, "Ball", 0x4C, ballId, 0, 0,
					0);
				CTaskManager::Instance()->PostMsg(0, "Screen", 0x4C, ballId, 0,
					0, 0);
				return;
			}

			case 31:
			{
				int scale = 0;
				sscanf(line + len + 1, "%d", &scale);
				if (scale < 1)
				{
					BadArgument(31);
					return;
				}
				AddText(MakeStr("Picture Scale = x%d", scale));
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x8C, scale, 0,
					0, 0);
				return;
			}

			case 36:
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x78, 0x20000,
					0, 0, 0);
				return;

			case 39:
			{
				int rule = 0;
				sscanf(line + len + 1, "%d", &rule);

				if (rule == 1)
				{
					rule = 0x10;
				}
				else if (rule == 2)
				{
					rule = 0x20;
				}
				else if (rule == 3)
				{
					rule = 0x40;
				}

				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x91, rule, 0,
					0, 0);
				CPowerGauge::Instance()->SetDebugPangYaArea(10.0f);
				AddText(MakeStr(" Pangya Area: x%d", 10));

				if (CSharedDoc::Instance()->m_golfGame.gameType == 1)
				{
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
						->gauge = Between(0.0f,
						TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge +
							100.0f,
						CSharedDoc::Instance()->GetComboGaugeLimit(
							PLAYER(GOLFDOC()->m_currentPlayer)->team));
				}
				else
				{
					PLAYER(GOLFDOC()->m_currentPlayer)->gauge = Between(0.0f,
						PLAYER(GOLFDOC()->m_currentPlayer)->gauge + 100.0f,
						CSharedDoc::Instance()->GetComboGaugeLimit(
							GOLFDOC()->m_currentPlayer));
				}

				AddText(MakeStr(" Combo: +%d", 100));
				return;
			}

			case 67:
			{
				// HACK!
				do
				{
					std::string name;
					FreshBitmapMap bres = g_pFresh->ListBres(name);

					int total = 0;
					CConsole::Print(MakeStr("[%s]", name.c_str()));

					for (FreshBitmapMap::iterator it = bres.begin();
						it != bres.end(); ++it)
					{
						CConsole::Print(MakeStr("%-30s: %d bytes",
							(*it).second.name.c_str(), (*it).second.size));
						total += (*it).second.size;
					}

					CConsole::Print(MakeStr("total %d bytes loaded", total));
				} while (0);
				return;
			}

			case 68:
				sscanf(line + len + 1, "%f %d", &causticSpeed, &causticAlpha);

				if (causticAlpha >= 0xFF)
				{
					causticAlpha = 0xFF;
				}

				return;

			case 69:
			{
				WSendPacket packet((enumClientPacket)0x86);
				packet.Send(TO_GAME);
				return;
			}

			case 70:
			{
				float gravity;
				sscanf(line + len + 1, "%f", &gravity);
				GetPVS().GravityModifier(1.0f / gravity);
				return;
			}

			case 74:
			{
				int n = 0;
				sscanf(line + len + 1, "%d", &n);

				switch (n)
				{
				case 2:
					GOLFDOC()->m_debugBar = 138.0f;
					break;
				case 3:
					GOLFDOC()->m_debugBar = 138.5f;
					break;
				case 4:
					GOLFDOC()->m_debugBar = 139.0f;
					break;
				case 5:
					GOLFDOC()->m_debugBar = 139.5f;
					break;
				case 6:
					GOLFDOC()->m_debugBar = 140.0f;
					break;
				case 7:
					GOLFDOC()->m_debugBar = 140.5f;
					break;
				case 8:
					GOLFDOC()->m_debugBar = 141.5f;
					break;
				case 9:
					GOLFDOC()->m_debugBar = 142.0f;
					break;
				default:
					GOLFDOC()->m_debugBar = 0;
					break;
				}
			}

				return;

			case 78:
			{
				float first = 0.0f;
				float second = 0.0f;
				sscanf(line + len + 1, "%f %f", &first, &second);
				return;
			}

			case 79:
			{
				CTask* pTask = !CTaskManager::Instance()
					? 0
					: CTaskManager::Instance()->GetCurrentTask();
				if (((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CShopTask::m_RTTI)
							: false) ||
					((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CRealMyRoomTask::m_RTTI)
							: false))
				{
					unsigned long typeId = 0;
					unsigned long posmask = 0;

					sscanf(line + len + 1, "%d %d", &typeId, &posmask);
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsPosmask(typeId, posmask));
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				}
				else
				{
					AddText(
						"You can use this function only in the shop and myroom");
				}

				return;
			}

			case 80:
			{
				CTask* pTask = !CTaskManager::Instance()
					? 0
					: CTaskManager::Instance()->GetCurrentTask();
				if (((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CShopTask::m_RTTI)
							: false) ||
					((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CRealMyRoomTask::m_RTTI)
							: false))
				{
					unsigned long typeId = 0;
					unsigned long first = 0;
					unsigned long second = 0;
					char texName[256] = { 0 };

					sscanf(line + len + 1, "%d %d %d %s", &typeId, &first,
						&second, texName);
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsTexName(typeId, first,
						second, texName));
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				}
				else
				{
					AddText(
						"You can use this function only in the shop and myroom");
				}

				return;
			}

			case 81:
			{
				CTask* pTask = !CTaskManager::Instance()
					? 0
					: CTaskManager::Instance()->GetCurrentTask();
				if (((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CShopTask::m_RTTI)
							: false) ||
					((__rtti_obj = pTask) != 0
							? ((IObject*)__rtti_obj)
								  ->IsExactKindOf(&CRealMyRoomTask::m_RTTI)
							: false))
				{
					char petName[256] = { 0 };
					unsigned long typeId = 0;

					sscanf(line + len + 1, "%d %s", &typeId, petName);
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsPetName(typeId, petName));
					pTask->SendMsgToMainActor(
						MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				}

				return;
			}

			case 84:
				if (lua_system::lua_script_manger::GetInstance()->RunString(
						line + len + 1))
				{
					AddText("lua code Success!");
				}

				return;

			case 83:
			{
				int index;
				sscanf(line + len + 1, "%d", &index);

				switch (index)
				{
				case 0:
					lua_system::lua_script_manger::GetInstance()->ReloadAll();
					AddText("luafile Reload All.");
					break;
				case 1:
					if (lua_system::lua_script_manger::GetInstance()->RunString(
							"dofile(\"test.lua\")"))
					{
						AddText("luafile load ok. [test.lua]");
					}
					break;
				case 2:
				{
					if (lua_system::lua_script_manger::GetInstance()->RunString(
							"dofile(\"test.lua\")"))
					{
						AddText("luafile load ok. [test.lua]");
					}

					CGimmickContainer* pContainer = 0;
					WSingleton<CContentsDoc>::Instance()->GetContainer(
						(localContentType_t)0xA3, pContainer);

					pContainer->LoadFromScript("test_table");
					break;
				}
				default:
					AddText("luacmd exec error. plz check luacmd index");
					break;
				}

				return;
			}

			case 85:
			{
				CVectorSlideTestContainer* pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer(
					(localContentType_t)0xA2, pContainer);

				if (pContainer)
				{
					int newType;

					sscanf(line + len + 1, "%d", &newType);
					pContainer->SetNewTypeVectorSlide(newType != 0);
				}

				return;
			}

			case 86:
			{
				CVectorSlideTestContainer* pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer(
					(localContentType_t)0xA2, pContainer);

				if (pContainer)
				{
					int speed;

					sscanf(line + len + 1, "%d", &speed);
					pContainer->SetLimitSpeed(speed);
				}

				return;
			}

			case 87:
			{
				CVectorSlideTestContainer* pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer(
					(localContentType_t)0xA2, pContainer);

				if (pContainer)
				{
					int height;

					sscanf(line + len + 1, "%d", &height);
					pContainer->SetBoundHeight(height);
				}

				return;
			}

			case 88:
			{
				CVectorSlideTestContainer* pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer(
					(localContentType_t)0xA2, pContainer);

				if (pContainer)
				{
					int count;

					sscanf(line + len + 1, "%d", &count);
					pContainer->SetLimitVectorSlideCount(count);
				}

				return;
			}
			}
			break;
		}
	}

	AddText(
		"Bad Command. Type 'help' -> list command or 'tweaker' -> list tweaker command");
}

void CConsole::BadArgument(int command)
{
	AddText(MakeStr("Bad Argument : %s", command_table[command].usage));
}

void CConsole::AddText(const char* text)
{
	char* line = new char[strlen(text) + 1];
	strcpy(line, text);
	m_lines += line;

	m_lineOffset = m_lines.GetCount() - 19;
}
