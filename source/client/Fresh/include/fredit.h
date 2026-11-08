#pragma once
#include <list>
#include <string>
#include "rtti.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class CChatMsg;
class FrGuiItem;
class FrWndManager;
struct FrInputState;

struct FrLine
{
	std::string text;
	bool marginAlign;
	unsigned long animStart;
	bool bIncludeEnterLine;

	FrLine()
		: bIncludeEnterLine(false)
	{
	}
};

class FrEdit : public FrWnd
{
	DECLARE_OBJECT(FrEdit)

public:
	enum eEditStyle
	{
		ES_EMOTICON = 1,
		ES_READ_ONLY = 2,
		ES_NO_EMOTICON = 4
	};

	FrEdit();
	virtual ~FrEdit();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void SetReadOnly(bool readOnly);
	const char* AddLine(const char* text, unsigned long color, bool enter);
	const char* SetLine(int line, const char* text, unsigned long color,
		bool enter, unsigned long animTime);
	const char* GetLine(int line, bool enter);
	void ForcedEdit(const char* text, unsigned long color);
	int GetLineNum();
	void ClearLine();
	void DeleteFirstLine();
	void DeleteEndLine();
	void AddText(const char* text, bool a, bool b);
	void SetCharLimit(int limit, bool b);
	void SetWidthLimit(float limit) { m_widthLimit = limit; }
	const char* GetEditText_Front() { return m_editText[ETT_FRONT].c_str(); }
	const char* GetEditText_Comp() { return m_editText[ETT_COMP].c_str(); }
	const char* GetEditText_End() { return m_editText[ETT_END].c_str(); }
	void SetFontColor(unsigned long color) { m_fontColor = color; }
	float GetWidthLimit() const { return m_widthLimit; }
	int GetLineHeight() const { return m_lineHeight; }
	unsigned long GetAnimTime(int line);
	void SetAutoLine(bool autoLine);
	int GetSelectLine() { return m_selectedLine; }
	void SetSelectLine(int line);
	void EnableEditStyle(eEditStyle style) { m_editProperty.Enable(style); }
	void DisableEditStyle(eEditStyle style) { m_editProperty.Disable(style); }
	bool IsEditStyle(eEditStyle style) { return m_editProperty.GetFlag(style); }
	bool IsEmoticonStyle();

protected:
	virtual void OnProc(const float deltaTime);
	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnMouseMove(const WPoint& point);
	virtual bool OnLButtonUp(const WPoint& point);
	virtual bool OnLButtonDown(const WPoint& point);
	virtual const char* OnSelectText(const FrInputState* input);
	virtual void EnableKeyFocus(FrInputState& input);
	virtual void OnKeyFocus(CChatMsg* im);

	const char* InsertLine(int line, const char* text, unsigned long color,
		bool enter);
	int DeleteLine(int line);
	void CursorInputProcess();
	void AutoCutNextLineProcess();
	void LimitTextMultiLine();
	int GetWholeTextLength(bool b);
	bool IsCursorEndPosition();
	bool IsStringCompress();
	bool PullNextLine();
	void LimitText(std::string& text);
	void RemoveColornLastSpace(const char* text, std::string& out);
	void MakePassword(const char* text, std::string& out);

	FrGuiItem* m_pItem;
	int m_lines;
	int m_lineHeight;
	int m_leftMargin;
	int m_topMargin;
	unsigned long m_font;
	unsigned long m_fontColor;
	unsigned long m_fontColor2;
	unsigned long m_bgColor;
	unsigned long m_borderColor;
	bool m_multiLine;
	int m_charLimit;
	float m_widthLimit;
	bool m_password;
	bool m_bCaretMove;
	WFlags m_editProperty;
	bool m_focusOff;
	bool m_bSymmetry;
	unsigned long m_dwCaretColor;
	unsigned long m_dwCaretColor2;
	bool m_bScrollUpdate;
	int m_OldCaretPos;

	enum eAlign
	{
		leftAlign,
		centerAlign,
		rightAlign
	};

	eAlign m_align;
	bool m_vCenterAlign;

	enum eEditTextType
	{
		ETT_FRONT,
		ETT_COMP,
		ETT_END,
		MAX_ETT
	};

	std::string m_editText[MAX_ETT];
	std::string m_oldEditText;
	int m_selectedLine;
	std::list<FrLine*> m_lineList;
	float m_caret;
	char m_caretDelta;
	bool m_AutoLine;
};
