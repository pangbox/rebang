#pragma once
#include <string>
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class Bitmap;
class CBackGround;
class FrGuiItem;
class WOverlay;

class FrDesktop : public FrWnd
{
	DECLARE_OBJECT(FrDesktop)

public:
	FrDesktop();
	virtual ~FrDesktop();

	void Init(float width, float height);
	void SetWallPaper(const char* pszFilename, bool bShow);
	void SetWallPaperFromBitmap(const Bitmap* bitmap, bool bShow);
	void LockWallPaper(bool lock) { m_bLocked = lock; }
	void SetBgColor(unsigned long color) { m_bgColor = color; }
	void Show(bool show) { m_bShow = show; }
	bool IsWallPaperVisible();
	void ResetRect();
	void SetScroll(float du, float dv);
	void SetAniBg(FrGuiItem& aniBg);

protected:
	virtual void OnProc(const float deltaTime);
	virtual void OnDraw();

	CBackGround* m_pBackGround;
	bool m_bShow;
	unsigned long m_bgColor;
	std::string m_bgFilename;
	bool m_bLocked;
	WOverlay* m_pAniBg;
	WRect m_aniBgDest;
	int m_velX;
	int m_velY;
	float m_aniBgSrcX;
	float m_aniBgSrcY;
};
