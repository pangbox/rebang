#pragma once

#include "singleton.h"
#include "wlist.h"

class WFont;

struct ConsoleKeyState
{
	char m_code;
	char m_pressed;
};

class CConsole : public WSingleton<CConsole>
{
public:
	static void Print(const char* text)
	{
		if (IsInstantiated())
		{
			Instance()->AddText(text);
		}
	}

	CConsole(void);
	virtual ~CConsole(void);

	void Init(void);
	void ShutDown(void);
	void Display(void);
	void Process(float elapsed);

protected:
	bool PushIntoBsStack(char c);
	char PopFromBsStack(void);
	void ClearBsStack(void);
	int BrowseCommand(const char* partial, char* result);
	void AddText(const char* text);
	void BadArgument(int command);
	void Command(const char* line);
	int TweakerCommandProc(const char* line);
	void InputProcess(float elapsed);

	float m_alpha;
	bool m_active;

	int m_state;
	void* m_historyPos;
	int m_historyEdge;

	char m_bsStack[0x80];
	char m_input[0x104];
	ConsoleKeyState* m_keyStates;
	WFont* m_pFont;
	WList<char*> m_history;
	WList<char*> m_lines;
	bool m_inputLocked;

	int m_lineOffset;
	char m_inputLine[0x80];
	float m_repeatDelay;
};
