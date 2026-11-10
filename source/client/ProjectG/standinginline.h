#pragma once

#include "frform.h"
#include "wlocalize.h"

class FrViewer;
class FrListBox;

static const char* g_szArrayName[] = { K2L_Compatibility("\xb4\xa9\xb8\xae"),
	K2L_Compatibility("\xc7\xcf\xb3\xaa"),
	K2L_Compatibility("\xbe\xc6\xc0\xfa"),
	K2L_Compatibility("\xbc\xbc\xbd\xc7\xb8\xae\xbe\xc6"),
	K2L_Compatibility("\xb8\xc6\xbd\xba"), K2L_Compatibility("\xc4\xed"),
	K2L_Compatibility("\xbe\xc6\xb8\xb0"),
	K2L_Compatibility("\xc4\xab\xc1\xee"),
	K2L_Compatibility("\xb7\xe7\xbd\xc3\xbe\xc6"),
	K2L_Compatibility("\xb3\xda"),

	K2L_Compatibility("\xc5\xac\xb7\xb4"),

	K2L_Compatibility("\xbe\xc6\xc0\xcc\xc5\xdb"),

	K2L_Compatibility("\xb1\xe2\xc5\xb8") };

class CStandingInLine : public FrForm
{
	DECLARE_OBJECT(CStandingInLine)

public:
	CStandingInLine();
	virtual ~CStandingInLine();

	int GetSelectNum() { return m_selectNum; }

protected:
	void OnView_TotalInit(int param);
	void OnView_TotalOwnerDraw(int param);
	void OnView_TotalButtonUp();

	void OnArrayListInit(int param);
	void OnArrayListOwnerDraw(int param);
	void OnArrayListBtnDown();
	void OnArrayListRBtnDown();

	FrViewer* m_pViewTotal;
	FrListBox* m_pArrayList;
	int m_selectNum;

	DECLARE_FRESH_MSGMAP()
};
