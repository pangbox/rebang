#pragma once

#include "singleton.h"
#include "frform.h"
#include "frbutton.h"

class FrOptionDlg;
class FrQuitForm;

class CQuitWindow : public WSingleton<CQuitWindow>, public FrCmdTarget
{
public:
	CQuitWindow();
	virtual ~CQuitWindow();

	void Process();
	void CreateOptionWindow();
	bool IsActive();

	bool HasOptionWindow() { return m_pOptionDlg ? true : false; }

protected:
	bool OnQuitFormResult(int result, FrForm* pForm);
	bool OnOptionResult(int result, FrForm* pForm);

	FrQuitForm* m_pQuitForm;
	FrOptionDlg* m_pOptionDlg;
	bool m_bCursorActive;
};
