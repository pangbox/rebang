#pragma once
#include <list>
#include <string>
#include "rtti.h"
#include "frcmdtarget.h"
#include "frwnd.h"

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

class FrElementDoc;

class FrWndManager
{
public:
	FrElementDoc* GetDocument() const;
	FrWnd* DoCreate(FrGuiItem& item, FrWndManager* manager, FrWnd* parent,
		FrCmdTarget* owner);
};

class FrGroupBox : public FrWnd
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const;
	FrGroupBox();
	virtual ~FrGroupBox();
	void Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent);
};
