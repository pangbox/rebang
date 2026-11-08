#pragma once
#include "baseobject.h"
#include "singleton.h"
#include "../../client/Wangreal/include/wtypes.h"

class WFont;
class WOverlay;

struct sCompStr
{
	char* pCompStr;
	unsigned char* pCompAttr;
	unsigned long* pCompCls;
	unsigned long dwCompAttrLen;
	unsigned long dwCompClsLen;
};

class CChatMsg : public WSingleton<CChatMsg>
{
public:
	CChatMsg();
	virtual ~CChatMsg();
	void Init();
	void ResetUiPosition();
	void Reset();
	void ClearHistory();
	void ReplaceBackupUiAlpha();
	void SetActive(bool active, bool reset, bool unused);
	void ToggleInsertMode();
	void AddChatMsg(const std::string&, unsigned long, bool, bool);
	void RemoveColor(const std::string&, std::string&);
	void GetChatText(std::string&, std::string&, std::string&);
	void GetChatText_JP(std::string&, std::string&, std::string&, int);
	int GetCurCaretPos() { return m_iCurByte; }
	const char* GetCompChar();
	const char* GetCompositionString() { return m_szComp; }
	void GetCompositionString(sCompStr*);
	void SetChatText(const char*, bool);
	void SetChatFacialUser(std::string& user) { m_chatFacialUser = user; }
	void ResetCurCaretPos()
	{
		m_iCurByte = 0;
		m_iCurChar = 0;
	}
	void CurCaretMoveLeft() { OnMoveLeft(); }
	void CurCaretMoveRight() { OnMoveRight(); }
	int GetNumBytes() { return m_iNumBytes; }
	void Open(bool, bool);
	void Close(bool, bool);
	bool IsOpened() { return m_iState == CHAT_WIN_OPENED; }
	bool IsClosed() { return m_iState == CHAT_WIN_CLOSED; }
	bool IsOpenning()
	{
		return m_iState == CHAT_WIN_LEFT || m_iState == CHAT_WIN_RIGHT;
	}
	void Process(const float);
	void ProcessWindow(const float);
	void ProcessMacro(const float);
	void PrintInChatWindow();
	void PrintInScreen();
	void DrawChatWindow();
	void PrintOnChatRoom();
	std::string GetIncomingChatMsg() const;
	int GetConsolKeyCode() const { return m_iConsolCode; }
	bool IsActive() const { return m_bActive; }
	WFont* GetMaskedFont() const { return m_pMaskedFont; }
	char IsExtChar(int index) { return m_szExt[index]; }
	void SetAlwaysActive();
	bool HasBeenPrinted();
	void EnableCaretMove(bool enable) { m_bCaretMove = enable; }
	void SetBufLen(int len) { m_nMaxBytes = len; }
	void SetBufWidth(float width) { m_fMaxWidth = width; }
	void Toggle(bool);
	const char* MakeShortID(const char*, float);
	unsigned long ConvertToUIColor(unsigned long);
	bool IME_IsCandWinOpened();
	bool IME_IsCompDone();
	bool IME_IsImeOpened();
	bool IME_IsCompInProgress();
	bool IME_IsAlphaNumericMode();
	void IME_SetAlphaNumericMode();
	void IME_SetOpenStatus(bool);
	tagCANDIDATELIST* const IME_GetCandList();

protected:
	void InitFont();
	void ProcessResultString();
	void ProcessCompString();
	void ProcessEditKey(const float);
	void ProcessResultString_JP();
	void ProcessCompString_JP();
	void OnMoveLeft();
	void OnMoveRight();
	void OnMoveEnd();
	void OnMoveHome();
	void OnBackSpace();
	void OnDelete();
	void DeleteCharacter();
	void DeleteSelected();
	void CopySelected();
	void PasteCopied();
	void InsertWCharacter(int);
	void WriteCharacter(int);

protected:
	enum
	{
		CHAT_WIN_CLOSED,
		CHAT_WIN_RIGHT,
		CHAT_WIN_LEFT,
		CHAT_WIN_OPENED
	};

	bool m_bPrinted;
	bool m_bCaretMove;
	int m_nMaxBytes;
	float m_fMaxWidth;
	WFont* m_pMaskedFont;
	WOverlay* m_pChatWindow[3];
	WRect m_rcChatWinSrc[3];
	WRect m_rcChatWinDst[3];
	float m_fVel;
	bool m_bActive;
	bool m_bInsert;
	bool m_bCompInProgress;
	bool m_bShowCaret;
	float m_fCaretTime;
	int m_iState;
	int m_iCompCode;
	int m_iPrevCode;
	int m_iConsolCode;
	char m_szChat[1024];
	char m_szComp[1024];
	char m_szExt[1024];
	bool m_abWide[1024];
	int m_iNumBytes;
	int m_iCurByte;
	int m_iNumChars;
	int m_iCurChar;
	int m_iSelByteS;
	int m_iSelByteE;
	int m_iNumSelBytes;
	int m_iSelCharS;
	int m_iSelCharE;
	int m_iNumSelChars;
	char m_szCopied[1024];
	char m_szCopiedExt[1024];
	bool m_abCopied[1024];
	int m_iNumCopyBytes;
	int m_iNumCopyChars;
	unsigned long m_lastMsgTime;
	unsigned char m_bBackupUiAlpha;
	bool m_bOpen;
	bool m_bToggle;
	std::string m_chatFacialUser;
	unsigned char m_bCompAttr[1024];
	unsigned long m_dwCompCls[256];
	unsigned long m_dwCompStrLen;
	unsigned long m_dwCompAttrLen;
	unsigned long m_dwCompClsLen;
};

inline void CChatMsg::Toggle(bool toggle)
{
	m_bToggle = toggle;
}
