#pragma once
#include <list>
#include <string>
#include "rtti.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class FrGuiItem;
class FrWndManager;

class FrTextButton : public FrWnd
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }

	FrTextButton();
	virtual ~FrTextButton();
	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void SetCaption(const char* caption);
	const char* GetCaption();
	void SetClippingArea(WRect* rect);
	const char* GetUnderCursor();
	const char* GetSelected();
	void Select(int index);
	void Unselect(int index);

	struct TButtonItem
	{
		std::string text;
		float left;
		float right;
		bool underCursor;
		bool selected;
	};

	typedef std::list<TButtonItem> TBUTTON_LIST;

protected:
	void Select(TBUTTON_LIST::iterator it);
	void Unselect(TBUTTON_LIST::iterator it);
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);
	virtual void OnMouseMove(const WPoint& point);
	virtual bool OnLButtonUp(const WPoint& point);
	virtual void OnSetCursor(bool bInClient, const WPoint& point);

	std::string m_caption;
	unsigned long m_color;
	unsigned long m_outlineColor;
	unsigned long m_style;
	unsigned long m_selColor;
	unsigned long m_selOutlineColor;
	unsigned long m_selStyle;
	bool m_center;
	WRect* m_clip;
	float m_hlAlpha;
	char m_hlAlphaD;
	float m_sepWidth;
	TBUTTON_LIST m_tBtnList;
	TBUTTON_LIST::iterator m_underCursor;
	TBUTTON_LIST::iterator m_selected;
};
