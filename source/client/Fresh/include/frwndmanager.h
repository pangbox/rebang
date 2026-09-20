#pragma once
#include <list>
#include <string>
#include "../../Wangreal/include/wtypes.h"
#include "frelement.h"

class Bitmap;
class CChatMsg;
class FrCmdTarget;
class FrCursor;
class FrDesktop;
class FrEdit;
class FrEmoticon;
class FrGraphicInterface;
class FrScrollBar;
class FrWnd;

struct RectangleShort
{
	short left;
	short top;
	short right;
	short bottom;
};

class FrGuiItem
{
public:
	virtual ~FrGuiItem();
	virtual void Init(const void* node);
	virtual const std::list<FrGuiItem*>* GetChildList();

	char unknown_04[0x3c];
	std::string m_name;
	char unknown_5c[0x10];
	RectangleShort m_rect;
};

struct FrInputState
{
	unsigned int mouse;
	bool hoverChecked;
	WPoint mousePos;
	WPoint oldMousePos;
	float wheelDelta;
	FrWnd* keyFocused;
	FrScrollBar* wheelFocused;
	CChatMsg* im;
};

class FrWndManager
{
public:
	enum eFadeState
	{
		OPENING = 0x0,
		OPENED = 0x1,
	};

	FrWndManager(FrElementDoc* pDoc);
	virtual ~FrWndManager();

	bool Init(const char* wallPaper, bool exclusiveKey);
	void Process(float deltaTime);
	void Display(bool drawDesktop);
	void CloseLayout();
	bool CreateLayout(const char* layout, FrCmdTarget* owner, bool firstTime);
	const Bitmap* GetBitmap(const char* resource, const char* id) const;
	__forceinline bool HidePrivacy() { return this->m_hidePrivacy; }
	__forceinline void HidePrivacy(bool hide) { this->m_hidePrivacy = hide; }
	__forceinline FrDesktop* GetDesktop() { return this->m_pDesktop; }
	FrElementDoc* GetDocument() const;
	FrWnd* DoCreate(FrGuiItem& item, FrWndManager* manager, FrWnd* parent,
		FrCmdTarget* owner);
	__forceinline const char* GetLayoutID() { return this->m_layoutID.c_str(); }

protected:
	FrElementDoc* m_pDoc;
	FrDesktop* m_pDesktop;
	FrCursor* m_pCursor;
	FrInputState m_istate;
	bool m_exclusiveKey;
	std::list<FrWnd*> m_topmostList;
	std::string m_layoutID;
	FrWnd* m_pCaptured;
	FrGraphicInterface* m_pDevice;
	unsigned int m_nRefIndex;
	int m_cursorIndex;
	FrEmoticon* m_pEmoticon;
	float m_fadeoutAlpha;
	eFadeState m_state;
	FrEdit* m_pFocusedEdit;
	WPoint m_caretPos;
	bool m_hidePrivacy;
};
