#include "realmyroom_authoritydlg.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "wlocalize.h"

extern Fresh* g_pFresh;

const char* ListName[] = { K2L_Compatibility("\xb0\xf8  \xb0\xb3"),
	K2L_Compatibility("\xba\xf1\xb0\xf8\xb0\xb3") };

IMPLEMENT_OBJECT(CRealMyRoom_AuthorityDlg, FrForm)

BEGIN_FRESH_MSGMAP(CRealMyRoom_AuthorityDlg, FrForm)
ON_FRESH_VI("kind_authority", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnAuth_KindPowerInit)
ON_FRESH_VI("kind_authority", FRCMD_OWNERDRAW,
	CRealMyRoom_AuthorityDlg::OnAuth_KindPowerOwnerdraw)
ON_FRESH_VV("kind_authority", FRCMD_LBUTTONDOWN,
	CRealMyRoom_AuthorityDlg::OnAuth_KindPowerBtnDown)
ON_FRESH_VI("authority_pw", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnAuth_PasswordInit)
ON_FRESH_BI("authority_pw", FRCMD_ENTERKEY,
	CRealMyRoom_AuthorityDlg::OnAuth_PWEnterKey)
ON_FRESH_VI("auth_frm03", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnAuthEditBgInit)
ON_FRESH_VI("storage_lock", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnInitStorageLock)
ON_FRESH_VV("storage_lock", FRCMD_LBUTTONDOWN,
	CRealMyRoom_AuthorityDlg::OnLBDownStorageLock)
ON_FRESH_VI("storage_unlock", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnInitStorageUnLock)
ON_FRESH_VV("storage_unlock", FRCMD_LBUTTONDOWN,
	CRealMyRoom_AuthorityDlg::OnLBDownStorageUnLock)
ON_FRESH_VI("storage_changepass", FRCMD_INIT,
	CRealMyRoom_AuthorityDlg::OnInitStorageChangePass)
ON_FRESH_VV("storage_changepass", FRCMD_LBUTTONUP,
	CRealMyRoom_AuthorityDlg::OnLBDownStorageChangePass)
END_FRESH_MSGMAP()

CRealMyRoom_AuthorityDlg::CRealMyRoom_AuthorityDlg()
{
	m_bAuthSetting = Doc()->m_rmrAuthority.bPrivate;
	OnLoadImage();
	m_bStorageLock = Doc()->m_bItemStorageLock;
}

CRealMyRoom_AuthorityDlg::~CRealMyRoom_AuthorityDlg()
{
}

void CRealMyRoom_AuthorityDlg::OnAuth_KindPowerInit(int param)
{
	m_pKindList = DYNAMIC_CAST(FrListBox, param);
	if (m_pKindList)
	{
		m_pKindList->SetMultiSelect(false);
		m_pKindList->AddItem((void*)ListName[0]);
		m_pKindList->AddItem((void*)ListName[1]);
		FrListItem* pItem =
			m_pKindList->FindItem((void*)ListName[m_bAuthSetting]);
		m_pKindList->SelectItem(pItem, true);
	}
}

void CRealMyRoom_AuthorityDlg::OnAuth_KindPowerOwnerdraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	int index = pItem->no;
	const Bitmap* pBitmap;
	if (!pItem->pData)
		pBitmap = m_pKindBmp[index][2];
	else if (pItem->selected)
	{
		pBitmap = m_pKindBmp[index][2];
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (pItem->underCursor)
	{
		pBitmap = m_pKindBmp[index][1];
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}
	else
	{
		pBitmap = m_pKindBmp[index][0];
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}
	if (pBitmap)
	{
		WRect dest(pItem->pos.x, pItem->pos.y, (float)pBitmap->Width(),
			(float)pBitmap->Height());
		pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);
	}
}

void CRealMyRoom_AuthorityDlg::OnAuth_KindPowerBtnDown()
{
	FrListItem* pItem = m_pKindList->GetItemUnderCursor();
	if (pItem)
	{
		if (pItem->no == m_bAuthSetting)
			m_pKindList->SelectItem(pItem, true);
		if (pItem->no == 0)
			m_bAuthSetting = false;
		else
			m_bAuthSetting = true;
		m_pPassword->SetVisible(m_bAuthSetting);
	}
}

void CRealMyRoom_AuthorityDlg::OnAuth_PasswordInit(int param)
{
	m_pPassword = DYNAMIC_CAST(FrEdit, param);
	if (m_pPassword)
	{
		m_pPassword->SetVisible(m_bAuthSetting);
		if (m_bAuthSetting == true)
			m_pPassword->AddLine(MakeStr("%s", Doc()->m_rmrAuthority.password),
				0, false);
	}
}

void CRealMyRoom_AuthorityDlg::OnAuthEditBgInit(int param)
{
}

void CRealMyRoom_AuthorityDlg::OnInitStorageLock(int param)
{
	m_pStorageLock = DYNAMIC_CAST(FrButton, param);
	if (m_pStorageLock)
	{
		m_pStorageLock->SetPushDelay(0.1f);
		if (m_bStorageLock == true)
			m_pStorageLock->SetButtonImg("rmr_set_lock_o", FrButton::NORMAL);
		else
			m_pStorageLock->SetButtonImg("rmr_set_lock_n", FrButton::NORMAL);
	}
}

void CRealMyRoom_AuthorityDlg::OnLBDownStorageLock()
{
	m_bStorageLock = true;
	if (Doc()->m_bItemStorageLock != true)
	{
		m_pStorageLock->SetButtonImg("rmr_set_lock_o", FrButton::NORMAL);
		m_pStorageUnLock->SetButtonImg("rmr_set_unlock_n", FrButton::NORMAL);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 568, 0, 0, 0, 0, 0);
		Close(FrCANCEL, true);
	}
}

void CRealMyRoom_AuthorityDlg::OnInitStorageUnLock(int param)
{
	m_pStorageUnLock = DYNAMIC_CAST(FrButton, param);
	if (m_pStorageLock)
	{
		m_pStorageUnLock->SetPushDelay(0.1f);
		if (m_bStorageLock == false)
			m_pStorageUnLock->SetButtonImg("rmr_set_unlock_o",
				FrButton::NORMAL);
		else
			m_pStorageUnLock->SetButtonImg("rmr_set_unlock_n",
				FrButton::NORMAL);
	}
}

void CRealMyRoom_AuthorityDlg::OnLBDownStorageUnLock()
{
	m_bStorageLock = false;
	if (Doc()->m_bItemStorageLock != false)
	{
		m_pStorageLock->SetButtonImg("rmr_set_lock_n", FrButton::NORMAL);
		m_pStorageUnLock->SetButtonImg("rmr_set_unlock_o", FrButton::NORMAL);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 569, 0, 0, 0, 0, 0);
		Close(FrCANCEL, true);
	}
}

void CRealMyRoom_AuthorityDlg::OnInitStorageChangePass(int param)
{
	m_pStorageChangePass = DYNAMIC_CAST(FrButton, param);
	if (m_pStorageChangePass)
		m_pStorageChangePass->SetPushDelay(0.1f);
}

void CRealMyRoom_AuthorityDlg::OnLBDownStorageChangePass()
{
	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 567, 0, 0, 0, 0, 0);
	Close(FrCANCEL, true);
}

bool CRealMyRoom_AuthorityDlg::OnAuth_PWEnterKey(int param)
{
	OnFreshOkay();
	return true;
}

void CRealMyRoom_AuthorityDlg::OnLoadImage()
{
	m_pKindBmp[0][0] = g_pFresh->RegisterBitmap("rmr_btn_open_n");
	m_pKindBmp[0][1] = g_pFresh->RegisterBitmap("rmr_btn_open_o");
	m_pKindBmp[0][2] = g_pFresh->RegisterBitmap("rmr_btn_open_d");
	m_pKindBmp[1][0] = g_pFresh->RegisterBitmap("rmr_btn_close_n");
	m_pKindBmp[1][1] = g_pFresh->RegisterBitmap("rmr_btn_close_o");
	m_pKindBmp[1][2] = g_pFresh->RegisterBitmap("rmr_btn_close_d");
}

void CRealMyRoom_AuthorityDlg::OnOK()
{
	std::string password(m_pPassword->GetLine(true, 0));
	char buffer[100];
	sprintf(buffer, "%s", m_pPassword->GetLine(true, 0));
	FrForm::OnOK();
}

bool CRealMyRoom_AuthorityDlg::GetAuthSetting()
{
	return m_bAuthSetting;
}

void CRealMyRoom_AuthorityDlg::GetPassWord()
{
	std::string password;
	if (m_pPassword)
	{
		password = m_pPassword->GetLine(true, 0);
		strcpy(Doc()->m_rmrAuthority.password, password.c_str());
	}
}

void CRealMyRoom_AuthorityDlg::OnCancle()
{
	FrForm::OnCancel();
}

IMPLEMENT_OBJECT(CRealMyRoom_EnterSetting, FrForm)

BEGIN_FRESH_MSGMAP(CRealMyRoom_EnterSetting, FrForm)
ON_FRESH_VI("input_pw", FRCMD_INIT, CRealMyRoom_EnterSetting::OnInputPwInit)
ON_FRESH_BI("input_pw", FRCMD_ENTERKEY,
	CRealMyRoom_EnterSetting::OnInputPwEnterKey)
END_FRESH_MSGMAP()

CRealMyRoom_EnterSetting::CRealMyRoom_EnterSetting()
{
}

CRealMyRoom_EnterSetting::~CRealMyRoom_EnterSetting()
{
}

void CRealMyRoom_EnterSetting::OnOK()
{
	if (m_pInputPw)
		m_password = m_pInputPw->GetLine(true, 0);
	FrForm::OnOK();
}

void CRealMyRoom_EnterSetting::OnCancle()
{
	FrForm::OnCancel();
}

std::string CRealMyRoom_EnterSetting::GetPassWord()
{
	return m_password;
}

void CRealMyRoom_EnterSetting::OnInputPwInit(int param)
{
	m_pInputPw = DYNAMIC_CAST(FrEdit, param);
	if (m_pInputPw)
		m_pInputPw->ClearLine();
}

bool CRealMyRoom_EnterSetting::OnInputPwEnterKey(int param)
{
	OnFreshOkay();
	return true;
}
