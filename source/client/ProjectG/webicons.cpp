#include "frbutton.h"
#include "webicons.h"

void CWebIcons_WebEvent1::OnInit(FrButton& button)
{
	button.SetVisible(true);
	button.SetPushSound("ui_desktop_icon_click");
	button.SetStyle(FrButton::BT_NORMAL);
	button.SetButtonImg(m_normalImage.c_str(), FrButton::NORMAL);
	button.SetButtonImg(m_overImage.c_str(), FrButton::OVER);
}

void CWebIcons_WebEvent1::OnButtonDown()
{
	// Compiled out
}

void CWebIcons_WebEvent2::OnInit(FrButton& button)
{
	button.SetVisible(true);
	button.SetPushSound("ui_desktop_icon_click");
	button.SetStyle(FrButton::BT_NORMAL);
	button.SetButtonImg(m_normalImage.c_str(), FrButton::NORMAL);
	button.SetButtonImg(m_overImage.c_str(), FrButton::OVER);
}

void CWebIcons_WebEvent2::OnButtonDown()
{
	// Compiled out
}
