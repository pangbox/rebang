#pragma once

#include "frform.h"

class FrEdit;
class FrButton;
class FrEmoticonDlg;

class FrCreateNickDlg : public FrForm
{
	DECLARE_OBJECT(FrCreateNickDlg)

	FrCreateNickDlg();

	virtual void OnProc(const float time);

	void CloseDlg(int result);
	void SetNick(const char* nick);
	void ReceiveCheckCode(int code);

	void SetCheckFlag(bool bCheck) { m_bCheck = bCheck; }
	bool GetCheckFlag() const { return m_bCheck; }

protected:
	void OnEmoticonInit(int param);
	void OnEditNickInit(int param);
	void OnDispNickInit(int param);
	void OnMessasgeInit(int param);
	void OnEmoticonUp();
	void OnCancelBtnUp();
	void OnOKBtnUp();
	void OnCheckBtnUp();
	void OnEditNickDown();
	bool OnEditNickEnterKey(int param);

private:
	bool CheckNickByClient();
	bool OnEmoticonResult(int result, FrForm* form);
	bool OnQuitDlgResult(int result, FrForm* form);

protected:
	FrEdit* m_pEditNick;
	FrEdit* m_pDispNick;
	FrButton* m_pEmoticonBtn;
	char m_nick[22];
	FrEmoticonDlg* m_pEmoticonDlg;
	bool m_bCheck;

	DECLARE_FRESH_MSGMAP()
};
