#pragma once

#include "frform.h"
#include <string>

enum eLoginState;

class FrLoginDlg : public FrForm
{
	DECLARE_OBJECT(FrLoginDlg)

	FrLoginDlg();
	virtual ~FrLoginDlg();

	void SetNoticeMsg(const char* msg);
	static void SetState(eLoginState state);
	static eLoginState GetState();
	void EnableLoginControls(bool bEnable);
	void BeginConnect();
	void Login();

protected:
	virtual bool OnInit();
	virtual void OnProc(const float delta);

	bool OnJoinDlgResult(int result, FrForm* form);
	bool OnFindDlgResult(int result, FrForm* form);
	bool OnDisconPrevSession(int result, FrForm* form);

	void OnIdInit(int param);
	void OnPasswordInit(int param);
	bool OnEnterKey(int param);
	void OnAutoIdInit(int param);
	void OnAutoIdBtnUp();
	void OnViewInit(int param);
	const char* QueryWelcomeMessage();
	void OnLoginInit(int param);
	void OnLoginBtnUp();
	void OnJoinBtnInit(int param);
	void OnFindBtnInit(int param);
	void OnJoinBtnUp();
	void OnFindBtnUp();
	void ResetInput(const char* msg);
	void OnHideStaticInit(int param);
	void OnParanAreaInit(int param);
	void OnHideAreaInit(int param);
	void OnHideAreaDraw();
	void OnLoginNoticeInit(int param);
	void OnHideBtnInit(int param);

	static eLoginState m_connState;

	std::string m_id;
	std::string m_passwd;
	FrEdit* m_pId;
	FrEdit* m_pPassword;
	FrEdit* m_pView;
	FrButton* m_pLogin;
	FrButton* m_pAutoId;
	unsigned long m_reserved15c;
	FrButton* m_pJoin;
	FrButton* m_pFind;
	unsigned long m_reserved168;
	int m_failCount;
	unsigned long m_reserved170;

private:
	bool PrepareWebLogin();
	void BeginClientLogin();

	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap() const;
};
