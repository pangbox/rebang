#pragma once
#include <string>
#include "rtti.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class FrGuiItem;
class FrWndManager;

class FrStatic : public FrWnd
{
	DECLARE_OBJECT(FrStatic)

public:
	FrStatic();
	virtual ~FrStatic();
	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void SetCaption(const char* caption) { m_caption = caption; }
	void SetCaption11(const char* caption)
	{
		m_caption = caption;
		m_smallFont = true;
	}
	const char* GetCaption() { return m_caption.c_str(); }
	void SetClippingArea(WRect* rect);
	void SetTextStyle(unsigned long style);
	void SetTextColor(unsigned long color, unsigned long outlineColor);
	virtual void Enable(bool enable);

protected:
	virtual void OnDraw();

	std::string m_caption;
	unsigned long m_color;
	unsigned long m_outlineColor;
	unsigned long m_style;
	unsigned long m_align;
	WRect* m_clip;
	bool m_smallFont;
};
