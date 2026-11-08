#pragma once
#include <string>
#include "rtti.h"
#include "frwnd.h"
#include "../../Wangreal/include/wtypes.h"

class Bitmap;
class FrElementFrame;
class FrGuiItem;
class FrWndManager;

class FrFrame : public FrWnd
{
	DECLARE_OBJECT(FrFrame)

public:
	FrFrame();
	virtual ~FrFrame();
	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	void SetCaption(const char* caption) { m_caption = caption; }
	void SetDesc(const char* desc) { m_desc = desc; }
	void UseEmoAtDesc(bool use) { m_emoAtDesc = use; }
	void SetCaptionFocus(bool bEnable) { m_bCaptionFocus = bEnable; }
	void SetBlink(bool blink);
	void SetCaptionOffset(bool offset) { m_captionOffset = offset; }

protected:
	void Release();
	void UpdateRectInfo(int type, const Bitmap** bitmaps);
	void DrawFrame(int type, float x, float y, const Bitmap** bitmaps);
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);

	FrGuiItem* m_pItem;
	FrElementFrame* m_pFrame;
	WSize m_a;
	WSize m_b;
	WSize m_c;
	WSize m_min;
	float m_width;
	float m_height;
	const Bitmap* m_pBaseBmp[9];
	const Bitmap* m_pSubBmp[9];
	const Bitmap* m_pBlinkBmp[3];
	std::string m_caption;
	std::string m_desc;
	bool m_emoAtDesc;
	bool m_bCaptionFocus;
	bool m_bBlink;
	bool m_bBlinkShow;
	bool m_captionOffset;
	float m_fBlinkTime;
};
