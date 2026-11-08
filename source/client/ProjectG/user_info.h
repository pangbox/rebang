#pragma once

#include <string>
#include "frcmdtarget.h"

class FrUserInfoForm;

class CUserInfo : public WSingleton<CUserInfo>, public FrCmdTarget
{
	friend class FrUserInfoForm;

public:
	CUserInfo();
	virtual ~CUserInfo();

	void SetInfo(unsigned long uid, unsigned long guid, bool bCheckTime,
		bool bOption, bool bMsnControl, bool bControlOff, std::string nickname);
	void Process(float delta);
	void Clear();
	void Refresh();
	void Close();
	bool IsVisible();
	bool IsRecvedData(unsigned char season);
	bool OnUserInfoFormResult(int result, FrForm* pForm);

	unsigned long GetGuid() { return m_guid; }

	FrUserInfoForm* GetDlg() { return m_pDlg; }

protected:
	FrUserInfoForm* m_pDlg;
	unsigned long m_guid;
	unsigned long m_uid;
	bool m_bCheckTime;
	bool m_bRequest;
	std::string m_nickname;
};

inline FrUserInfoForm* USERINFODLG()
{
	if (!CUserInfo::IsInstantiated())
		return NULL;
	return CUserInfo::Instance()->GetDlg();
}
