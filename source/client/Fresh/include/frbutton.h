#pragma once
#include "rtti.h"
#include "frwnd.h"

class Bitmap;
class FrGuiItem;
class FrWndManager;

enum eButtonMoreStyles
{
	FWS_BT_3TYPES = 0x10000,
	FWS_BT_HIGHLIGHT = 0x20000
};

class FrButton : public FrWnd
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }

	enum eButMode
	{
		NORMAL,
		OVER,
		PRESSED,
		BLINK
	};

	enum eButStyle
	{
		BT_NONE,
		BT_NORMAL,
		BT_3TYPES,
		BT_ALWAYSON
	};

	enum eButPushStyle
	{
		BP_NONE,
		BP_PUSHBUTTON,
		BP_CHECKBUTTON,
		BP_RADIOBUTTON
	};

	enum eButDrawStyle
	{
		BD_NONE,
		BD_OWNERDRAW
	};

	FrButton();
	virtual ~FrButton();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent,
		bool bParseParam);
	void InitExtern(const Bitmap* pBmpNormal, const Bitmap* pBmpOver,
		const Bitmap* pBmpSelected, eButStyle style, eButPushStyle pushStyle);
	eButMode GetStatus() { return m_status; }
	void SetStatus(eButMode status);
	void SetPushDelay(float delay)
	{
		m_pushDelay = delay;
		m_pushedTime = delay + 0.1f;
	}
	void SetButtonImg(const char* name, eButMode mode);
	void SetButtonBelowImg(const char* name, eButMode mode);
	virtual void Enable(bool enable);
	void UseRightButton(bool enable) { m_useRightButton = enable; }
	void EnableBlink(bool enable);
	void SetStyle(eButStyle style) { m_style = style; }

protected:
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);
	virtual bool OnLButtonDown(const WPoint& mousePos);
	virtual bool OnLButtonUp(const WPoint& mousePos);
	virtual bool OnRButtonDown(const WPoint& mousePos);
	virtual bool OnRButtonUp(const WPoint& mousePos);
	virtual void OnSetCursor(bool bInClient, const WPoint& mousePos);
	virtual void PreCreateWindow(FrWndManager* pManager, unsigned long dwStyle,
		const WRect& rect, FrWnd* pParentWnd);

	FrGuiItem* m_pItem;
	const Bitmap* m_pBitmap[4];
	const Bitmap* m_pBtnBg[4];
	eButMode m_status;
	eButStyle m_style;
	eButPushStyle m_pushStyle;
	eButDrawStyle m_drawStyle;
	float m_pushDelay;
	float m_pushedTime;
	float m_fBlinktime;
	bool m_bPressed;
	bool m_bPrevPressed;
	bool m_useRightButton;
	bool m_blink;
	bool m_blinkEnable;
};
