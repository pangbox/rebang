#pragma once
#include <list>
#include <string>
#include "rtti.h"
#include "frcmdtarget.h"
#include "frwnd.h"
#include "frwndmanager.h"

class FrGroupBox : public FrWnd
{
	DECLARE_OBJECT(FrGroupBox)

public:
	FrGroupBox();
	virtual ~FrGroupBox();
	void Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent);
};
