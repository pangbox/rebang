#pragma once
#include <string>
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class CBackGround;
class WOverlay;

class FrDesktop : public FrWnd
{
private:
	CBackGround* m_pBackGround;
	bool m_bShow;
	unsigned int m_bgColor;
	std::string m_bgFilename;
	bool m_bLocked;
	WOverlay* m_pAniBg;
	WRect m_aniBgDest;
	int m_velX;
	int m_velY;
	float m_aniBgSrcX;
	float m_aniBgSrcY;
};
