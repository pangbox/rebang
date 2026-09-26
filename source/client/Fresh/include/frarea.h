#pragma once
#include <string>
#include "rtti.h"
#include "frwnd.h"

class Bitmap;
class FrGuiItem;
class FrWndManager;

class FrArea : public FrWnd
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }

	FrArea();
	virtual ~FrArea();

	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	bool SetBgImg(const Bitmap* pBitmap);
	bool SetBgImg(const char* name);
	const Bitmap* GetBgImg() { return m_pBitmap; }
	void SetMouseEvent(bool active);
	void EnableClipping(bool enable) { m_isClipping = enable; }
	void SetClipArea(const WRect& rect) { m_rcClipArea = rect; }
	void GetClipArea(WRect& rect) { rect = m_rcClipArea; }

protected:
	virtual bool ClippingArea(WRect& src, WRect& dst);
	virtual void OnDraw();
	virtual void OnSetCursor(bool bInClient, const WPoint& mousePos);
	virtual bool OnLButtonUp(const WPoint& mousePos);
	virtual bool OnLButtonDown(const WPoint& mousePos);

	FrGuiItem* m_pItem;
	const Bitmap* m_pBitmap;
	bool m_center;
	bool m_stretch;
	bool m_mouseEvent;
	unsigned long m_color;
	std::string m_imgName;
	bool m_isClipping;
	WRect m_rcClipArea;
};
