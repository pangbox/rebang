#pragma once
#include <list>
#include <string>
#include "rtti.h"
#include "frcmdtarget.h"
#include "frwnd.h"
#include "frwndmanager.h"

class FrGroupBox : public FrWnd
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const;
	FrGroupBox();
	virtual ~FrGroupBox();
	void Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent);
};
