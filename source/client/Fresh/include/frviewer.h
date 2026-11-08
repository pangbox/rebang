#pragma once
#include <string>
#include "rtti.h"
#include "frwnd.h"
#include "frscrollbar.h"
#include "background.h"
#include "../../Wangreal/include/wtypes.h"

class FrGuiItem;
class FrWndManager;

class FrViewer : public FrWnd
{
	DECLARE_OBJECT(FrViewer)

public:
	FrViewer();
	virtual ~FrViewer();
	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	bool Open(const char* filename);
	int GetSrcWidth() { return m_pView ? m_pView->GetWidth() : 0; }
	int GetSrcHeight() { return m_pView ? m_pView->GetHeight() : 0; }
	float GetViewWidth() { return m_rect.w; }
	float GetViewHeight() { return m_rect.h; }
	float GetScrollBarOffset()
	{
		return m_pScrBar ? m_pScrBar->GetRect().w : 0.0f;
	}
	void ShowScrollBar(bool show)
	{
		if (m_pScrBar)
			m_pScrBar->SetVisible(show);
	}
	void SetMouseEvent(bool enable);

protected:
	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnSetCursor(bool bInClient, const WPoint& point);
	virtual bool OnLButtonUp(const WPoint& point);
	virtual bool OnLButtonDown(const WPoint& point);
	virtual bool OnRButtonUp(const WPoint& point);
	virtual bool OnRButtonDown(const WPoint& point);
	virtual void OnDblClick(const WPoint& point);

	FrGuiItem* m_pItem;
	std::string m_fileName;
	unsigned long m_bgColor;
	CBackGround* m_pView;
	bool m_mouseEvent;
};
