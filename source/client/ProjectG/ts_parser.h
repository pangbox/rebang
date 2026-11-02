#pragma once

#include <stdio.h>
#include <string.h>
#include <list>

#include "eventactor.h"

enum eMacroType
{
	MACRO_WAIT = 0,
};

enum eTS_Keyword
{
	TSKEY_WAIT = 0,
	TSKEY_KEY_DOWN = 1,
	TSKEY_KEY_DOWN_KEEP = 2,
	TSKEY_KEY_RELEASE = 3,
	TSKEY_MOUSE_CLICK = 4,
	TSKEY_MOUSE_MOVE = 5,
	TSKEY_MISSION = 6,
	TSKEY_TIP = 7,
	TSKEY_TIP_VARROW = 8,
	TSKEY_TIP_HARROW = 9,
	TSKEY_TIP_CUSTOM_ARROW = 10,
	TSKEY_TIP_IMAGE = 11,
	TSKEY_TARGET = 12,
	TSKEY_SEND_MSG = 13,
	TSKEY_CURSOR = 14,
	TSKEY_BLOCK_INPUT = 15,
	TSKEY_FREE_INPUT = 16,
	TSKEY_PUSH_MACRO = 17,
	TSKEY_ONLY_KEY = 18,
	TSKEY_WITH_MOUSE = 19,
	TSKEY_ENABLE = 20,
	TSKEY_DISABLE = 21,
	TSKEY_NULL = 22,
	TSKEY_VIEW_WIDTH = 23,
	TSKEY_VIEW_HEIGHT = 24,
	TSKEY_OPTION_BAR_Y = 25,
	TSKEY_GETPOINT = 26,
	TSKEY_CAST_INT = 27,
	TSKEY_VAL_CUSTOM_ARROW = 28,
	TSKEY_BAR_RIGHTMOST = 29,
	TSKEY_BAR_CENTER = 30,
	TSKEY_SKIP_COUNT = 31,
	TSKEY_POINTNAME = 32,
	TSKEY_TARGETNAME = 33,
	TSKEY_GAME_TYPE = 34,
	TSKEY_GAME_TYPE_TUTORIAL_BASIC = 35,
	TSKEY_IF = 36,
	TSKEY_ELSE = 37,
	TSKEY_ENDIF = 38,
	TSKEY_GREATER = 39,
	TSKEY_LESS = 40,
	TSKEY_EQUAL = 41,
	TSKEY_NOT = 42,
};

struct sTS_TaskMsg
{
	static const int MAX_SYMBOL_LEN = 32;
	taskMsg_t m_msg;
	char m_name[0x20];
};

struct sTS_Keyword
{
	static const int MAX_SYMBOL_LEN = 32;
	eTS_Keyword m_keyword;
	char m_name[0x20];
};

#define TS_TASKMSG_COUNT 9
#define TS_KEYWORD_COUNT 43

extern sTS_TaskMsg gTS_TaskMsgTable[TS_TASKMSG_COUNT];
extern sTS_Keyword gTS_KeywordTable[TS_KEYWORD_COUNT];

struct sTS_Error
{
	static const int MAX_ERROR_MSG_LEN = 128;
	int line;
	char msg[MAX_ERROR_MSG_LEN];
};

struct sTS_ScriptInfo
{
	char m_name[0x20];
};

struct sTS_Macro
{
	sTS_Macro(void)
	{
		m_type = (eMacroType)-1;
		m_text = NULL;
		m_param = 0;
		m_count = 0;
	}

	~sTS_Macro(void)
	{
		if (m_text != NULL)
		{
			delete m_text;
			m_text = NULL;
		}
	}

	sTS_Macro& operator=(sTS_Macro& other)
	{
		m_type = other.m_type;

		if (m_text != NULL)
		{
			delete m_text;
			m_text = NULL;
		}

		if (other.m_text != NULL)
		{
			int length = strlen(other.m_text) + 1;
			m_text = new char[length];
			strcpy(m_text, other.m_text);
		}

		m_param = other.m_param;
		m_count = other.m_count;

		return *this;
	}

	eMacroType m_type;
	char* m_text;
	int m_param;
	int m_count;
};

class ts_Parser
{
public:
	static const int MAX_FILE_LEN = 256;
	static const int MAX_LINE_LEN = 2048;
	static const int MAX_TOKEN_LEN = 64;
	static const int MAX_BUFFER_LEN = 64;
	enum eFileType
	{
		FILETYPE_NONE = 0,
		FILETYPE_BINARY = 1,
		FILETYPE_TEXT = 2,
	};

	ts_Parser(void);
	ts_Parser(const char* fileName);
	virtual ~ts_Parser(void);

	bool PTS_OpenFile(const char* fileName, eFileType type);
	bool PTS_CloseFile(void);
	taskMsg_t GetTaskMessage(const char* name) const;
	eTS_Keyword GetKeyword(const char* name) const;
	void ConvertMacroType(eTS_Keyword keyword, eMacroType* type) const;
	int FindQuotationMarkBlock(const char* text, int& begin, int& end,
		int start, bool flag);
	bool PTS_PopMacro(sTS_Macro* macro);
	bool PTS_LoadScriptInfo(void);
	bool PTS_LoadScript(const char* fileName);

protected:
	inline bool IsOpenedFile(void) const;
	void ClearVariables(void);
	void ClearSymbolTable(void);
	void ClearStringBuffer(void);
	bool Rewind(void);
	bool NextLine(void);
	bool FindToken(const char* token);
	bool RemoveToken(void);
	const char* GetToken(void);
	const char* GetLine(void);
	int InsertSymbolTable(const char* name);
	bool PushMacro(const char* text, sTS_Macro& macro);
	sTS_Macro* CommandAnalyst(const char* text);

	bool m_opened;
	char m_fileName[0x100];
	eFileType m_fileType;
	FILE* m_file;
	char m_line[0x800];
	char m_token[0x40];
	char m_nextLine[0x40];
	short m_unknown990;
	short m_unknown992;
	std::list<sTS_Error*> m_errors;
	std::list<sTS_ScriptInfo*> m_scriptInfos;
	std::list<sTS_Macro*> m_macros;
	std::list<char*> m_symbols;
};

#include "ts_parser.inl"
