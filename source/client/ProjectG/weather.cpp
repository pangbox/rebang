#include "projectg.h"
#include "weather.h"
#include "woverlay.h"

void CCloud::Display()
{
}

void CCloud::DisplayFullScreenOverlay()
{
	WOverlay::DrawBox(g_view,
		WRect(0, 0, g_view->GetWidth(), g_view->GetHeight()), 0, 0x60303030,
		0.001f);
}
