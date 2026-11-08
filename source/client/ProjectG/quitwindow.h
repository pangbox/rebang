#pragma once

#include "singleton.h"
#include "frform.h"
#include "frbutton.h"

class FrOptionDlg;

class FrQuitForm : public FrForm
{
public:
	DECLARE_OBJECT(FrQuitForm)
	FrQuitForm();

	void EnableOptionBtn(bool bEnable);

	WRect GetOptionBtnRect() { return m_pOptionBtn->GetRect(); }

protected:
	virtual void OnProc(const float delta);
	virtual void OnDraw();

	void OnOptionBtnInit(int param);
	void OnNextBtnInit(int param);
	void OnToLobbyBtnInit(int param);
	void OnExitBtnInit(int param);
	void OnOptionBtnUp();
	void OnNextBtnUp();
	void OnToLobbyBtnUp();
	void OnExitBtnUp();
	void OnCloseBtnUp();
	bool OnToLobbyConfirmResult(int result, FrForm* pForm);
	bool OnExitConfirmResult(int result, FrForm* pForm);

	FrButton* m_pOptionBtn;
	FrButton* m_pNextBtn;
	FrButton* m_pToLobbyBtn;
	FrButton* m_pExitBtn;
	FrButton* m_pHoverBtn;
	float m_time;

	DECLARE_FRESH_MSGMAP()
};

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
