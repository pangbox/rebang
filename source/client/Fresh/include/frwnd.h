#pragma once
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

class FrWnd : public IObject, public FrCmdTarget
{
	// Not 100% sure if true.
	friend class Fresh;

protected:
	FrWnd();

public:
	class sToolTipData
	{
		int bFixWnd;
		WPoint posFixWnd;
		unsigned int style;
	};

	virtual const WRTTI* GetRTTI() const;
	virtual ~FrWnd();
	virtual bool Close(bool immediate);
	virtual bool Create(const char* text, const char* name,
		FrWndManager* manager, unsigned long style, const WRect& rect,
		FrWnd* parent);
	virtual void PreCreateWindow(FrWndManager* manager, unsigned long style,
		const WRect& rect, FrWnd* parent);
	virtual void MoveWindow(const WPoint& point);
	virtual void Enable(bool enable);
	virtual void SetVisible(bool visible);
	static const WRTTI m_RTTI;

protected:
	virtual void OnDraw();
	virtual void OnProc(float elapsed);
	virtual void OnResize();
	virtual void OnMouseMove(const WPoint& point);
	virtual bool OnLButtonUp(const WPoint& point);
	virtual bool OnLButtonDown(const WPoint& point);
	virtual bool OnRButtonUp(const WPoint& point);
	virtual bool OnRButtonDown(const WPoint& point);
	virtual void OnDblClick(const WPoint& point);
	virtual void OnWheel(FrInputState& input) { }
	virtual const char* OnSelectText(const FrInputState* input) { return NULL; }
	virtual void OnKeyFocus(CChatMsg* message);
	virtual void OnSetCursor(bool active, const WPoint& point);
	virtual void EnableKeyFocus(FrInputState& input) { m_nFlags.Set(0x10); }
	virtual void SetIconRect(const WRect& rect);

	WFlags m_nFlags;
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
	std::string m_wndToolTip;
	sToolTipData* m_pToolTipData;
};
