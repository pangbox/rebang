#include "stdafx.h"
#include "ts_parser.h"
#include "shareddoc.h"
#include "golfdoc.h"
#include "polysoup.h"
#include "clientsetting.h"
#include "wview.h"

sTS_TaskMsg gTS_TaskMsgTable[TS_TASKMSG_COUNT] = {
	// HACK!
	{ (0, (taskMsg_t)0x1d7), "PM_RUN" },
	{ (taskMsg_t)0x1bc, "PM_SET_TUTORIAL_UI" },
	{ (taskMsg_t)0x112, "PM_LOOKUP" },
	{ (taskMsg_t)0x87, "PM_SET_MACRO_SHOT" },
	{ (taskMsg_t)0x88, "PM_WAIT" },
	{ (taskMsg_t)0x8a, "PM_GET_CUSTOM_ARROW" },
	{ (taskMsg_t)0x1be, "PM_FLICKER_HOLEINFO" },
	{ (taskMsg_t)0x1bd, "PM_FLICKER_MINISCORE" },
	{ (taskMsg_t)0x8f, "PM_RETURN_LOBBY" },
};

sTS_Keyword gTS_KeywordTable[TS_KEYWORD_COUNT] = {
	// HACK!
	{ (0, TSKEY_WAIT), "TS_WAIT" },
	{ TSKEY_KEY_DOWN, "TS_KEY_DOWN" },
	{ TSKEY_KEY_DOWN_KEEP, "TS_KEY_DOWN_KEEP" },
	{ TSKEY_KEY_RELEASE, "TS_KEY_RELEASE" },
	{ TSKEY_MOUSE_CLICK, "TS_MOUSE_CLICK" },
	{ TSKEY_MOUSE_MOVE, "TS_MOUSE_MOVE" },
	{ TSKEY_MISSION, "TS_MISSION" },
	{ TSKEY_TIP, "TS_TIP" },
	{ TSKEY_TIP_VARROW, "TS_TIP_VARROW" },
	{ TSKEY_TIP_HARROW, "TS_TIP_HARROW" },
	{ TSKEY_TIP_CUSTOM_ARROW, "TS_TIP_CUSTOM_ARROW" },
	{ TSKEY_TIP_IMAGE, "TS_TIP_IMAGE" },
	{ TSKEY_TARGET, "TS_TARGET" },
	{ TSKEY_SEND_MSG, "TS_SEND_MSG" },
	{ TSKEY_CURSOR, "TS_CURSOR" },
	{ TSKEY_BLOCK_INPUT, "TS_BLOCK_INPUT" },
	{ TSKEY_FREE_INPUT, "TS_FREE_INPUT" },
	{ TSKEY_PUSH_MACRO, "PushMacro" },
	{ TSKEY_ONLY_KEY, "ONLY_KEY" },
	{ TSKEY_WITH_MOUSE, "WITH_MOUSE" },
	{ TSKEY_ENABLE, "ENABLE" },
	{ TSKEY_DISABLE, "DISABLE" },

	{ TSKEY_NULL, "NULL" },
	{ TSKEY_VIEW_WIDTH, "VIEW_WIDTH" },
	{ TSKEY_VIEW_HEIGHT, "VIEW_HEIGHT" },
	{ TSKEY_OPTION_BAR_Y, "OPTION_BAR_Y" },

	{ TSKEY_GETPOINT, "GETPOINT" },
	{ TSKEY_CAST_INT, "CAST_INT" },
	{ TSKEY_VAL_CUSTOM_ARROW, "VAL_CUSTOM_ARROW" },
	{ TSKEY_BAR_RIGHTMOST, "BAR_RIGHTMOST" },
	{ TSKEY_BAR_CENTER, "BAR_CENTER" },
	{ TSKEY_SKIP_COUNT, "SKIP_COUNT" },

	{ TSKEY_POINTNAME, "POINTNAME" },
	{ TSKEY_TARGETNAME, "TARGETNAME" },

	{ TSKEY_GAME_TYPE, "GAME_TYPE" },
	{ TSKEY_GAME_TYPE_TUTORIAL_BASIC, "GAME_TYPE_TUTORIAL_BASIC" },

	{ TSKEY_IF, "IF" },
	{ TSKEY_ELSE, "ELSE" },
	{ TSKEY_ENDIF, "ENDIF" },

	{ TSKEY_GREATER, ">" },
	{ TSKEY_LESS, "<" },
	{ TSKEY_EQUAL, "==" },
	{ TSKEY_NOT, "!" },
};

ts_Parser::ts_Parser(void)
{
	ClearVariables();
}

ts_Parser::ts_Parser(const char* fileName)
{
	ClearVariables();
}

ts_Parser::~ts_Parser(void)
{
	PTS_CloseFile();
	ClearVariables();
}

void ts_Parser::ClearVariables(void)
{
	m_opened = false;
	memset(m_fileName, 0, sizeof(m_fileName));
	m_fileType = FILETYPE_NONE;
	m_file = NULL;

	memset(m_line, 0, sizeof(m_line));
	memset(m_token, 0, sizeof(m_token));
	memset(m_nextLine, 0, sizeof(m_nextLine));

	m_unknown990 = 0;
	m_unknown992 = 0;

	std::list<sTS_Error*>::iterator errorIt;
	for (errorIt = m_errors.begin(); errorIt != m_errors.end(); ++errorIt)
	{
		if (*errorIt != NULL)
		{
			delete *errorIt;
			*errorIt = NULL;
		}
	}
	m_errors.clear();

	std::list<sTS_ScriptInfo*>::iterator infoIt;
	for (infoIt = m_scriptInfos.begin(); infoIt != m_scriptInfos.end();
		++infoIt)
	{
		if (*infoIt != NULL)
		{
			delete *infoIt;
			*infoIt = NULL;
		}
	}
	m_scriptInfos.clear();

	std::list<sTS_Macro*>::iterator macroIt;
	for (macroIt = m_macros.begin(); macroIt != m_macros.end(); ++macroIt)
	{
		if (*macroIt != NULL)
		{
			delete *macroIt;
			*macroIt = NULL;
		}
	}
	m_macros.clear();

	ClearSymbolTable();
}

void ts_Parser::ClearSymbolTable(void)
{
	std::list<char*>::iterator it;
	for (it = m_symbols.begin(); it != m_symbols.end(); ++it)
	{
		if (*it != NULL)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_symbols.clear();
}

bool ts_Parser::PTS_OpenFile(const char* fileName, eFileType type)
{
	bool result = false;

	if (IsOpenedFile() == result)
	{
		if (fileName != NULL)
		{
			memset(m_fileName, 0, sizeof(m_fileName));
			strcpy(m_fileName, fileName);

			m_fileType = type;

			switch (m_fileType)
			{
			case FILETYPE_NONE:
				result = false;
				break;
			case FILETYPE_BINARY:
				m_file = fopen(m_fileName, "rb");
				break;
			case FILETYPE_TEXT:
				m_file = fopen(m_fileName, "rt");
				break;
			}

			if (m_file != NULL)
			{
				m_opened = true;
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
	}

	return result;
}

bool ts_Parser::PTS_CloseFile(void)
{
	if (IsOpenedFile() == false)
	{
		if (m_file != NULL)
		{
			fclose(m_file);
			m_file = NULL;
			m_opened = false;
			return true;
		}
	}

	return false;
}

taskMsg_t ts_Parser::GetTaskMessage(const char* name) const
{
	if (name == NULL)
	{
		return (taskMsg_t)-1;
	}

	taskMsg_t result = (taskMsg_t)-1;
	for (int i = 0; i < TS_TASKMSG_COUNT; i++)
	{
		if (strcmp(name, gTS_TaskMsgTable[i].m_name) == 0)
		{
			result = gTS_TaskMsgTable[i].m_msg;
			break;
		}
		else
		{
			result = (taskMsg_t)-1;
		}
	}

	return result;
}

eTS_Keyword ts_Parser::GetKeyword(const char* name) const
{
	if (name == NULL)
	{
		return (eTS_Keyword)-1;
	}

	eTS_Keyword result = (eTS_Keyword)-1;
	for (int i = 0; i < TS_KEYWORD_COUNT; i++)
	{
		if (strcmp(name, gTS_KeywordTable[i].m_name) == 0)
		{
			result = gTS_KeywordTable[i].m_keyword;
			break;
		}
		else
		{
			result = (eTS_Keyword)-1;
		}
	}

	return result;
}

void ts_Parser::ConvertMacroType(eTS_Keyword keyword, eMacroType* type) const
{
	if (type != NULL)
	{
		switch (keyword)
		{
		case 0:
			*type = (eMacroType)0;
			break;
		case 1:
			*type = (eMacroType)1;
			break;
		case 2:
			*type = (eMacroType)2;
			break;
		case 3:
			*type = (eMacroType)3;
			break;
		case 4:
			*type = (eMacroType)4;
			break;
		case 5:
			*type = (eMacroType)5;
			break;
		case 6:
			*type = (eMacroType)8;
			break;
		case 7:
			*type = (eMacroType)9;
			break;
		case 8:
			*type = (eMacroType)10;
			break;
		case 9:
			*type = (eMacroType)11;
			break;
		case 10:
			*type = (eMacroType)12;
			break;
		case 11:
			*type = (eMacroType)13;
			break;
		case 12:
			*type = (eMacroType)21;
			break;
		case 13:
			*type = (eMacroType)22;
			break;
		case 14:
			*type = (eMacroType)23;
			break;
		case 15:
			*type = (eMacroType)24;
			break;
		case 16:
			*type = (eMacroType)25;
			break;
		case 32:
		case 33:
			break;
		default:
			*type = (eMacroType)-1;
			break;
		}
	}
}

int ts_Parser::InsertSymbolTable(const char* name)
{
	int length = strlen(name) + 1;

	char* symbol = new char[length];
	memset(symbol, 0, length);
	strcpy(symbol, name);

	m_symbols.push_back(symbol);

	return (intptr_t)symbol;
}

int ts_Parser::FindQuotationMarkBlock(const char* text, int& begin, int& end,
	int index, bool withMarks)
{
	if (index > 5)
	{
		return 0;
	}

	int count = 0;
	int marks[10] = { 0 };
	int length = strlen(text);

	for (int i = 0; i < length; i++)
	{
		if (text[i] == '"')
		{
			marks[count++] = i;
		}
	}

	int first = marks[index * 2 - 2];
	int last = marks[index * 2 - 1];

	if (last - first > 0)
	{
		if (!withMarks)
		{
			first++;
			last--;
		}

		begin = first;
		end = last;

		return last - first + 1;
	}

	return 0;
}

void ts_Parser::ClearStringBuffer(void)
{
	memset(m_line, 0, sizeof(m_line));
	memset(m_token, 0, sizeof(m_token));
	memset(m_nextLine, 0, sizeof(m_nextLine));
}

bool ts_Parser::Rewind(void)
{
	if (!IsOpenedFile())
		return false;

	bool result = true;
	fpos_t pos = 0;
	if (fsetpos(m_file, &pos) != 0)
	{
		result = false;
	}

	return result;
}

bool ts_Parser::NextLine(void)
{
	if (!IsOpenedFile())
		return false;

	bool result = true;
	if (!feof(m_file))
	{
		fgets(m_nextLine, sizeof(m_nextLine), m_file);
		if (feof(m_file))
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	return result;
}

bool ts_Parser::FindToken(const char* token)
{
	if (!IsOpenedFile())
	{
		return false;
	}
	else
	{
		fpos_t startPos = 0;
		fpos_t pos = 0;
		if (fgetpos(m_file, &startPos) != 0)
		{
			return false;
		}

		memset(m_line, 0, sizeof(m_line));
		memset(m_token, 0, sizeof(m_token));

		while (!feof(m_file))
		{
			if (fgetpos(m_file, &pos) != 0)
			{
				return false;
			}

			fgets(m_line, sizeof(m_line), m_file);
			sscanf(m_line, "%s", m_token);

			if (strcmp(m_token, token) == 0)
			{
				return fsetpos(m_file, &pos) == 0 ? true : false;
			}
		}

		fsetpos(m_file, &startPos);

		return false;
	}
}

bool ts_Parser::RemoveToken(void)
{
	if (!IsOpenedFile())
		return false;

	bool result = true;
	if (fscanf(m_file, "%s", m_token) != 0)
	{
		result = false;
	}

	return result;
}

const char* ts_Parser::GetToken(void)
{
	if (!IsOpenedFile())
		return NULL;

	fpos_t pos;
	fscanf(m_file, "%s", m_token);

	if (fgetpos(m_file, &pos) != 0)
	{
		return NULL;
	}

	size_t length = strlen(m_token);

	pos -= length;
	return fsetpos(m_file, &pos) == 0 ? m_token : NULL;
}

const char* ts_Parser::GetLine(void)
{
	if (!IsOpenedFile())
		return NULL;
	if (feof(m_file))
		return NULL;

	fpos_t pos;
	if (fgetpos(m_file, &pos) != 0)
		return NULL;

	fgets(m_line, sizeof(m_line), m_file);

	if (fsetpos(m_file, &pos) != 0)
		return NULL;

	return m_line;
}

bool ts_Parser::PTS_LoadScriptInfo(void)
{
	if (!IsOpenedFile())
		return false;

	Rewind();

	ClearStringBuffer();

	std::list<sTS_ScriptInfo*>::iterator it;
	for (it = m_scriptInfos.begin(); it != m_scriptInfos.end(); ++it)
	{
		if (*it != NULL)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_scriptInfos.clear();

	ClearSymbolTable();

	if (FindToken("PTS_SCRIPT_INFO"))
	{
		RemoveToken();
		if (strcmp(GetToken(), "{") == 0)
		{
			RemoveToken();
			while (strcmp(GetToken(), "}") != 0)
			{
				sTS_ScriptInfo* info = new sTS_ScriptInfo;

				if (info != NULL)
				{
					memset(info, 0, sizeof(sTS_ScriptInfo));

					strcpy(info->m_name, GetToken());
					RemoveToken();

					m_scriptInfos.push_back(info);
				}
			}
		}
	}

	int count = m_scriptInfos.size();
	return count > 0 ? true : false;
}

bool ts_Parser::PTS_LoadScript(const char* fileName)
{
	if (!IsOpenedFile())
		return false;

	bool found = false;
	std::list<sTS_ScriptInfo*>::iterator it;
	for (it = m_scriptInfos.begin(); it != m_scriptInfos.end(); ++it)
	{
		if (*it != NULL)
		{
			if (strcmp((*it)->m_name, fileName) == 0)
			{
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		return false;
	}

	std::list<sTS_Macro*>::iterator macroIt;
	for (macroIt = m_macros.begin(); macroIt != m_macros.end(); ++macroIt)
	{
		if (*macroIt != NULL)
		{
			delete *macroIt;
			*macroIt = NULL;
		}
	}
	m_macros.clear();

	ClearStringBuffer();

	Rewind();

	ClearSymbolTable();

	if (FindToken("#BEGIN_SCRIPT_DECLARE"))
	{
		if (FindToken(fileName))
		{
			RemoveToken();
			if (strcmp(GetToken(), "{") == 0)
			{
				RemoveToken();
				while (strcmp(GetToken(), "}") != 0)
				{
					sTS_Macro* macro = CommandAnalyst(GetLine());
					if (macro != NULL)
					{
						m_macros.push_back(macro);
					}
					NextLine();
				}
			}
		}
	}

	int count = m_macros.size();
	return count > 0 ? true : false;
}

bool ts_Parser::PTS_PopMacro(sTS_Macro* macro)
{
	bool result = m_macros.size() > 0;

	if (macro == NULL)
	{
		result = false;
	}
	else
	{
		if (result == true)
		{
			std::list<sTS_Macro*>::iterator it = m_macros.begin();

			if (it != m_macros.end())
			{
				if (*it != NULL)
				{
					*macro = **it;
					delete *it;
					*it = NULL;
					m_macros.erase(it);
				}
				else
				{
					result = false;
				}
			}
			else
			{
				result = false;
			}
		}
	}

	return result;
}

sTS_Macro* ts_Parser::CommandAnalyst(const char* text)
{
	if (text == NULL)
		return NULL;

	char keyword[0x40] = "";

	sTS_Macro* macro = new sTS_Macro;
	if (macro != NULL)
	{
		int length = strlen(text) + 1;
		char* buffer = new char[length];
		memset(buffer, 0, length);
		strcpy(buffer, text);

		sscanf(buffer, "%s", keyword);

		bool pushed = false;
		switch (GetKeyword(keyword))
		{
		case 0x11:
			pushed = PushMacro(buffer, *macro);
			break;
		}

		if (!pushed)
		{
			delete macro;
			macro = NULL;
		}

		if (buffer != NULL)
		{
			delete buffer;
		}
	}

	return macro;
}

extern "C"
{
	// HACK!
	static const volatile float BAR_OFFSET[2] = { 500.0f, 140.0f };
}

bool ts_Parser::PushMacro(const char* text, sTS_Macro& macro)
{
	char argument[0x40] = "";
	char command[0x40] = "";
	char buffer[0x800] = "";

	bool result = false;
	sscanf(text, "%s %s", argument, command);

	switch (GetKeyword(command))
	{
	case TSKEY_TIP:
	case TSKEY_TIP_CUSTOM_ARROW:
	{
		int begin = 0;
		int end;
		int length = FindQuotationMarkBlock(text, begin, end, 1, false);

		if (length <= 1)
			break;

		macro.m_type = (eMacroType)7;
		macro.m_text = new char[length + 1];
		memset(macro.m_text, 0, length + 1);
		memcpy(macro.m_text, text + begin, length);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_TIP_VARROW:
	{
		int begin = 0;
		int end = 0;
		int length = FindQuotationMarkBlock(text, begin, end, 1, true);

		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, text + begin, length);

		int values[4] = { 0 };
		char names[4][0x40] = { "\0" };
		char numbers[0x40] = "";

		sscanf(text + end + 1, "%d %d %d %d %s %s %s %s", &values[0],
			&values[1], &values[2], &values[3], names[0], names[1], names[2],
			names[3]);

		for (int i = 0; i < 4; i++)
		{
			switch (GetKeyword(names[i]))
			{
			case TSKEY_VIEW_WIDTH:
				values[i] += (int)g_view->GetWidth();
				break;
			case TSKEY_VIEW_HEIGHT:
				values[i] += (int)g_view->GetHeight();
				break;
			case TSKEY_OPTION_BAR_Y:
				values[i] += (int)WSingleton<COption>::Instance()->gGetBar_Y();
				break;
			}
		}

		sprintf(numbers, "%d %d %d %d", values[0], values[1], values[2],
			values[3]);

		int size = strlen(buffer) + strlen(numbers) + 2;

		macro.m_type = (eMacroType)8;
		macro.m_text = new char[size];
		memset(macro.m_text, 0, size);
		sprintf(macro.m_text, "%s %s", buffer, numbers);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_TIP_HARROW:
	{
		int begin = 0;
		int end;
		int textLength = strlen(text);

		FindQuotationMarkBlock(text, begin, end, 1, true);

		int length = textLength - begin;

		if (length <= 1)
			break;

		macro.m_type = (eMacroType)9;
		macro.m_text = new char[length + 1];
		memset(macro.m_text, 0, length + 1);
		memcpy(macro.m_text, text + begin, length);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_TIP_IMAGE:
	{
		int begin = 0;
		int end;
		int textLength = strlen(text);

		FindQuotationMarkBlock(text, begin, end, 1, true);

		int length = textLength - begin;

		if (length <= 1)
			break;

		macro.m_type = (eMacroType)11;
		macro.m_text = new char[length + 1];
		memset(macro.m_text, 0, length + 1);
		memcpy(macro.m_text, text + begin, length);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_WAIT:
	{
		int delay = -1;
		sscanf(text, "%s %s %d", argument, argument, &delay);

		macro.m_type = MACRO_WAIT;
		macro.m_text = NULL;
		macro.m_param = delay;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_KEY_DOWN:
	{
		int param = -1;
		int count = 1;
		sscanf(text, "%s %s %s %d %d", argument, argument, command, &param,
			&count);

		int length = strlen(command);
		if (length == 0)
			break;

		macro.m_type = (eMacroType)1;
		macro.m_text = new char[strlen(command) + 1];
		memset(macro.m_text, 0, strlen(command) + 1);
		strcpy(macro.m_text, command);

		macro.m_param = param;
		macro.m_count = count;

		if (count < 1)
		{
			macro.m_count = 1;
		}
		result = true;
		break;
	}

	case TSKEY_KEY_DOWN_KEEP:
	{
		int param = -1;
		int count = 1;
		sscanf(text, "%s %s %s %d %d", argument, argument, command, &param,
			&count);

		int length = strlen(command);
		if (length == 0)
			break;

		macro.m_type = (eMacroType)1;
		macro.m_text = new char[strlen(command) + 1];
		memset(macro.m_text, 0, strlen(command) + 1);
		strcpy(macro.m_text, command);

		macro.m_param = param;
		macro.m_count = count;

		if (count < 1)
		{
			macro.m_count = 1;
		}
		result = true;
		break;
	}

	case TSKEY_KEY_RELEASE:
		macro.m_type = (eMacroType)3;
		macro.m_text = NULL;
		macro.m_param = 0;

		macro.m_count = 1;
		result = true;
		break;

	case TSKEY_MOUSE_CLICK:
	{
		int x = 0;
		int y = 0;
		sscanf(text, "%s %s %d %d", argument, command, &x, &y);

		macro.m_type = (eMacroType)4;
		sprintf(argument, "%d", x);
		macro.m_text = new char[strlen(argument) + 1];
		memset(macro.m_text, 0, strlen(argument) + 1);
		strcpy(macro.m_text, argument);

		macro.m_param = y;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_MOUSE_MOVE:
	{
		int x = 0;
		int y = 0;
		int param = 0;
		sscanf(text, "%s %s %d %d %d", argument, command, &x, &y, &param);

		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%d %d", x, y);

		int length = strlen(buffer);
		if (length == 0)
			break;

		macro.m_type = (eMacroType)5;
		macro.m_text = new char[strlen(buffer) + 1];
		memset(macro.m_text, 0, strlen(buffer) + 1);
		strcpy(macro.m_text, buffer);

		macro.m_param = param;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_MISSION:
		macro.m_type = (eMacroType)6;
		macro.m_text = NULL;

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;

	case TSKEY_TARGET:
	{
		int param = 0;

		memset(command, 0, sizeof(command));
		sscanf(text, "%s %s %d", argument, command, &param);

		int length = strlen(command);
		if (length == 0)
			break;

		macro.m_type = (eMacroType)12;
		macro.m_text = new char[strlen(command) + 1];
		memset(macro.m_text, 0, strlen(command) + 1);
		strcpy(macro.m_text, command);

		macro.m_param = param;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_SEND_MSG:
	{
		char keys[3][0x40] = { "\0" };
		char values[3][0x40] = { "\0" };

		memset(argument, 0, sizeof(argument));
		memset(command, 0, sizeof(command));

		int begin = 0;
		int end = 0;
		int length = FindQuotationMarkBlock(text, begin, end, 1, false);

		if (end - begin > 0x3F)
		{
			result = false;
			break;
		}

		memcpy(argument, text + begin, length);

		sscanf(text + end + 2, "%s %s %s %s %s %s %s", command, keys[0],
			keys[1], keys[2], values[0], values[1], values[2]);

		taskMsg_t msg = GetTaskMessage(command);

		for (int i = 0; i < 3; i++)
		{
			switch (GetKeyword(keys[i]))
			{
			case TSKEY_GETPOINT:
				sprintf(values[i], "%d",
					(int)(intptr_t)&GOLFDOC()
						->m_pPolySoup->FindPoint(values[i])
						->pos);
				break;
			case TSKEY_CAST_INT:
				sprintf(values[i], "%d", InsertSymbolTable(values[i]));
				break;
			case TSKEY_BAR_RIGHTMOST:
				sprintf(values[i], "%f", atof(values[i]) + BAR_OFFSET[0]);
				break;
			case TSKEY_BAR_CENTER:
				sprintf(values[i], "%f", atof(values[i]) + BAR_OFFSET[1]);
				break;
			}
		}

		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, argument, msg, values[0], values[1], values[2]);

		macro.m_type = (eMacroType)13;
		macro.m_text = new char[strlen(buffer) + 1];
		memset(macro.m_text, 0, strlen(buffer) + 1);
		strcpy(macro.m_text, buffer);

		macro.m_param = 0;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_CURSOR:
	{
		sscanf(text, "%s %s %s", argument, command, buffer);

		int length = strlen(buffer);
		if (length == 0)
			break;

		int enable;
		switch (GetKeyword(buffer))
		{
		case TSKEY_ENABLE:
			enable = 1;
			break;
		case TSKEY_DISABLE:
			enable = 0;
			break;
		default:
			goto done;
		}

		memset(argument, 0, sizeof(argument));
		sprintf(argument, "%d", enable);

		macro.m_type = (eMacroType)14;
		macro.m_text = new char[strlen(argument) + 1];
		memset(macro.m_text, 0, strlen(argument) + 1);
		strcpy(macro.m_text, argument);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_BLOCK_INPUT:
	{
		result = false;
		sscanf(text, "%s %s %s", argument, command, buffer);

		int withMouse;
		switch (GetKeyword(buffer))
		{
		case TSKEY_ONLY_KEY:
			withMouse = 0;
			break;
		case TSKEY_WITH_MOUSE:
			withMouse = 1;
			break;
		default:
			goto done;
		}

		macro.m_type = (eMacroType)15;
		macro.m_text = new char[2];
		memset(macro.m_text, 0, 2);
		sprintf(macro.m_text, "%d", withMouse);

		macro.m_param = 0;

		macro.m_count = 1;
		result = true;
		break;
	}

	case TSKEY_FREE_INPUT:
		macro.m_type = (eMacroType)16;
		macro.m_text = NULL;

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;

	case TSKEY_POINTNAME:
	case TSKEY_TARGETNAME:
		sscanf(text, "%s %s %s", argument, command, buffer);

		switch (GetKeyword(command))
		{
		case TSKEY_POINTNAME:
			macro.m_type = (eMacroType)0x20;
			break;
		case TSKEY_TARGETNAME:
			macro.m_type = (eMacroType)0x21;
			break;
		}

		macro.m_text = new char[strlen(buffer) + 1];
		memset(macro.m_text, 0, strlen(buffer) + 1);
		strcpy(macro.m_text, buffer);

		macro.m_param = -1;
		macro.m_count = 1;
		result = true;
		break;

	default:
		result = false;
		break;
	}

done:
	return result;
}
