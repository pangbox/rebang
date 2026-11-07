#pragma once
#include "wflag.h"
#include "rtti.h"
#include "frcmdtarget.h"
#include "objectfactory.h"
#include "../../Wangreal/include/wtypes.h"

struct sFRESH_MSGMAP;

class FrWndManager;
class FrWnd;
class CChatMsg;
struct FrInputState;
class FrScrollBar;
class FrGraphicInterface;
class FrEmoticon;
class FrToolTip;

enum eFrFlags
{
	FWF_NONE = 0x0,
	FWF_TOOLTIPS = 0x1,
	FWF_VIEWFOCUS = 0x8,
	FWF_KEYFOCUS = 0x10,
	FWF_DESTROY = 0x20,
	FWF_FADEOUT = 0x40,
	FWF_INITED = 0x80,
	FWF_FADING = 0x100,
	FWF_TOPFOCUS = 0x200,
	FWF_FADING_EX = 0x400,
	FWF_FADEOUT_EX = 0x800,
	FWF_PRIVACY = 0x1000,
	FWF_FADEELEMENT = 0x2000
};

enum eFrStyle
{
	FWS_NONE = 0x0,
	FWS_VISIBLE = 0x1,
	FWS_DISABLED = 0x2,
	FWS_HASTITLE = 0x4,
	FWS_CHILD = 0x8,
	FWS_TOPMOST = 0x10,
	FWS_MOVEFRAME = 0x20,
	FWS_NOMOUSEEVENT = 0x40,
	FWS_KEYEVENT = 0x80,
	FWS_FIXED = 0x100,
	FWS_NODBLCLICK = 0x200,
	FWS_HOVER = 0x400,
	FWS_NOWHEELEVENT = 0x800
};

class FrWnd : public IObject, public FrCmdTarget
{
	// Not 100% sure if true.
	friend class Fresh;
	friend class FrWndManager;
	friend IObject* FrWndMakeInstance();

public:
	struct sToolTipData
	{
		int bFixWnd;
		WPoint posFixWnd;
		unsigned long style;
	};

	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }
	virtual ~FrWnd();
	void DestroyChild();
	virtual bool Close(bool bFade);
	virtual bool Create(const char* lpszWindowText, const char* lpszWindowName,
		FrWndManager* pManager, unsigned long dwStyle, const WRect& rect,
		FrWnd* pParentWnd);
	virtual void PreCreateWindow(FrWndManager* pManager, unsigned long dwStyle,
		const WRect& rect, FrWnd* pParentWnd);
	bool SendCmdToOwnerTarget(FrCmdTarget* pCmdTarget, eFrCmd cmd, int var1,
		sFRESH_HANDLER* pHandler);
	bool SendCmdToOwnerTarget(eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler);
	void SetOwner(FrCmdTarget* pOwner);
	void AddChild(FrWnd* pChild);
	FrWndManager* WndManager() const { return m_pWndManager; }
	FrGraphicInterface* GDI() const;
	FrEmoticon* Emo() const;
	FrWnd* GetParent() const;
	FrWnd* FindChild(const char* lpszWindowText);
	FrWnd* FindChild(const FrWnd* pWnd);
	FrWnd* FindChildByName(const char* lpszWindowName);
	FrWnd* FindChildByStyle(unsigned long style);
	FrWnd* FindChildForm(const char* lpszWindowName);
	bool IsChild(const FrWnd* pWnd) const;
	void CloseChild(FrWnd* pChildWnd, bool bFade, bool force);
	void CloseChildForm(bool bFade, bool force);
	void RemoveWindow(FrWnd* pWnd);
	void EnumerateChildWindow(bool (*callback)(FrWnd*, void*), void* parm);
	void GetClientRect(WRect& rect) const;
	void SetClientRect(const WRect& rect);
	void ClientToScreen(WPoint& point) const;
	void ClientToScreen(WRect& rect) const;
	void ScreenToClient(WPoint& point) const;
	void ScreenToClient(WRect& rect) const;
	virtual void MoveWindow(const WPoint& point);
	void SetRect(const WRect& rect);
	const WRect& GetRect() { return m_rect; }
	void SetWindowTextA(const char* lpszText);
	void GetWindowTextA(std::string& outText) const;
	void GetWindowTextA(char* lpszTextBuf, unsigned int nBuffMax) const;
	unsigned int GetWindowTextLengthA() const;
	void SetWindowName(const char* lpszName);
	void GetWindowName(std::string& outName) const;
	void GetWindowName(char* lpszNameBuf, unsigned int nBuffMax) const;
	unsigned int GetWindowNameLength() const;
	void EnableToolTip(bool enable) { m_nFlags.Turn(FWF_TOOLTIPS, enable); }
	bool IsEnableToolTip() const { return m_nFlags.GetFlag(FWF_TOOLTIPS); }
	void SetToolTipText(const std::string& text);
	std::string GetToolTipText() const { return m_wndToolTip; }
	void SetFixTooltipWnd(const WPoint& pos);
	void SetUnFixToolTipWnd();
	int IsFixedToolTip() const;
	const WPoint& GetToolTipWndPos() const;
	void SetToolTipFrameStyle(unsigned long style);
	unsigned long GetToolTipFrameStyle() const;
	void OnDisplay(bool checkTopmost, bool drawChild, bool drawOneself);
	void OnProcess(const float deltaTime, FrInputState& istate,
		bool checkTopmost, bool bUnableMode);

	WFlags m_nFlags;

	const WFlags& GetStyle() const { return m_dwStyle; }
	float GetAlpha() const { return m_wndAlpha; }
	void SetAlpha(float alpha) { m_wndAlpha = alpha; }
	float GetAlpha2() const;
	void SetAlpha2(float alpha);
	void SetAlpha2ToChild(float a);
	virtual void Enable(bool enable) { m_dwStyle.Turn(FWS_DISABLED, !enable); }
	bool IsEnabled() const { return !m_dwStyle.GetFlag(FWS_DISABLED); }
	virtual void SetVisible(bool visible);
	bool IsVisible() const { return m_dwStyle.GetFlag(FWS_VISIBLE); }
	void SetKeyEvent(bool enable) { m_dwStyle.Turn(FWS_KEYEVENT, enable); }
	void SetTopmost(bool topmost);
	void SetFixed(bool fixed) { m_dwStyle.Turn(FWS_FIXED, fixed); }
	bool IsFixed() { return m_dwStyle.GetFlag(FWS_FIXED); }
	void UseDblClick(bool use) { m_dwStyle.Turn(FWS_NODBLCLICK, !use); }
	void EnableHover(bool enable, float time);
	void SetWheelEvent(bool enable);
	void HidePrivacy(bool hide) { m_nFlags.Turn(FWF_PRIVACY, hide); }
	void SetFadeout(bool bOut);
	void SetElementFadeOut(bool bOut);
	void SetViewFocus(bool takeFromOthers);
	void ResetViewFocus(FrWnd* pWndStop);
	FrWnd* FindViewFocused(bool enabled_visible);
	bool IsViewFocused() const;
	bool IsTopFocus() const;
	bool SetKeyFocus(bool resetPrevImeData);
	bool HasKeyFocus() { return m_nFlags.GetFlag(FWF_KEYFOCUS); }
	bool ResetKeyFocus();
	void FindNextTopFocus(bool bForce);
	bool SetCapture();
	FrWnd* GetCapture();
	bool ReleaseCapture();
	void SetCursor(int hCursor);
	int GetCursor() const;
	void MoveCursor(const char* name);
	void SetPushSound(const char* name_const);
	bool PlayPushSound();
	FrScrollBar* GetScrollBar() { return m_pScrBar; }
	void SetWheelFocus();

protected:
	FrWnd(FrWnd&);
	FrWnd();
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime) { }
	virtual void OnResize() { }
	virtual void OnMouseMove(const WPoint& mousePos) { }
	virtual bool OnLButtonUp(const WPoint& mousePos) { return false; }
	virtual bool OnLButtonDown(const WPoint& mousePos) { return false; }
	virtual bool OnRButtonUp(const WPoint& point) { return false; }
	virtual bool OnRButtonDown(const WPoint& point) { return false; }
	virtual void OnDblClick(const WPoint& point) { }
	virtual void OnWheel(FrInputState& input) { }
	virtual const char* OnSelectText(const FrInputState* input) { return NULL; }
	virtual void OnKeyFocus(CChatMsg* message) { }
	virtual void OnSetCursor(bool bInClient, const WPoint& mousePos) { }
	virtual void EnableKeyFocus(FrInputState& input) { m_nFlags.Enable(0x10); }
	virtual void SetIconRect(const WRect& rect) { }

private:
	void DoFadeDisplay();
	void DoFadeProcess(const float deltaTime, bool bExtend);
	void CheckHover(float deltaTime, bool bInClient, bool& hoverChecked);

protected:
	FrWnd* m_pParentWnd;
	FrCmdTarget* m_pOwner;
	std::list<FrWnd*> m_childList;
	WRect m_rect;
	WFlags m_dwStyle;
	std::string m_wndText;
	std::string m_wndName;
	float m_wndAlpha;
	float m_wndAlpha2;
	unsigned int m_nRefID;
	FrWndManager* m_pWndManager;
	FrScrollBar* m_pScrBar;
	float m_fadeTime;
	WRect m_iconRect;
	const char* m_szPushSound;
	float m_dblClickTimeout;
	bool m_dblClicked;
	WPoint m_dblClickPos;
	bool m_hoverOn;
	float m_hoverTime;
	float m_accHoverTime;

private:
	std::string m_wndToolTip;
	static FrToolTip* m_pToolTip;
	sToolTipData* m_pToolTipData;
	static FrToolTip* ToolTip();
	static bool IsToolTipInstantiated() { return m_pToolTip != NULL; }
	static void CreateToolTip(FrWndManager* pManager);
	static void DestroyToolTip();
};
