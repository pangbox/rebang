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

enum enumGuiType
{
	GI_NONE = 0x0,
	GI_FORM = 0x1,
	GI_STATIC = 0x2,
	GI_TEXTBUTTON = 0x3,
	GI_EDIT = 0x4,
	GI_COMBOBOX = 0x5,
	GI_COMBOCTLEX = 0x6,
	GI_BUTTON = 0x7,
	GI_FRAME = 0x8,
	GI_RESOURCE = 0x9,
	GI_BITMAP = 0xA,
	GI_AREA = 0xB,
	GI_LISTBOX = 0xC,
	GI_GAUGEBAR = 0xD,
	GI_GAUGEBAREX = 0xE,
	GI_GAUGEBARIMAGE = 0xF,
	GI_VIEWER = 0x10,
	GI_CONTEXTMENU = 0x11,
	GI_TABBUTTON = 0x12,
	GI_GROUPBOX = 0x13,
	GI_MACROITEM = 0x14,
	GI_LAST = 0x15,
};

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

	enumGuiType m_type;
	std::string m_resource;
	std::string m_caption;
	std::string m_name;
	std::map<std::string, std::string> m_param;
	unsigned int m_flag;
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
