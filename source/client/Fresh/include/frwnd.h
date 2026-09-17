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

class FrWnd : public IObject, public FrCmdTarget
{
protected:
	FrWnd();

public:
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
	virtual void OnWheel(FrInputState& input);
	virtual const char* OnSelectText(const FrInputState* input);
	virtual void OnKeyFocus(CChatMsg* message);
	virtual void OnSetCursor(bool active, const WPoint& point);
	virtual void EnableKeyFocus(FrInputState& input);
	virtual void SetIconRect(const WRect& rect);

	char unknown_08[8];
	FrCmdTarget* m_pOwner;
	char unknown_14[0xc0];
};
