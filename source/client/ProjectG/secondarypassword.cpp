#include "browser.h"
#include "secondarypassword.h"
#include "secondarypassworddlg.h"
#include "gatewayactor.h"
#include "clientsetting.h"
#include "logininfo.h"
#include "messagemanager.h"
#include "../../shared/s5/sharedutilities.h"

extern Fresh* g_pFresh;

using namespace _gateway;

CSecondaryPassword::CSecondaryPassword()
	: m_pExplanePasswordDlg(NULL),
	  m_pPasswordInputDlg(NULL),
	  m_pSelectCertifyDlg(NULL),
	  m_pPasswordNotifyDlg(NULL)
{
}

CSecondaryPassword::~CSecondaryPassword()
{
}

bool CSecondaryPassword::Initialize()
{
	AddFunctionType(1, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnResultRegistPassword);
	AddFunctionType(2, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestRegistPassword);
	AddFunctionType(3, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestStoreCertifyFlag);
	AddFunctionType(4, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSecondaryPassword::
			OnRequestCheckCertifyFlag);
	AddFunctionType(5, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnOpenPasswordExplaneDlg);
	AddFunctionType(6, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnOpenPasswordCreateDlg);
	AddFunctionType(7, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnOpenPasswordLoginDlg);
	AddFunctionType(8, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSecondaryPassword::OnClosePasswordDlg);
	AddFunctionType(9, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnOpenSelectCertifyDlg);
	AddFunctionType(10, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnCloseSelectCertifyDlg);
	AddFunctionType(13, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CSecondaryPassword::OnOpenSelfCertifyWebPage);
	AddFunctionType(12, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestLoginPassword);
	AddFunctionType(11, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnResultLoginPassword);
	AddFunctionType(14, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSecondaryPassword::OnWaitGameServerList);
	AddFunctionType(15, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(void (
			CSecondaryPassword::*)())&CSecondaryPassword::OnNotifyPasswordInfo);
	AddFunctionType(16, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestAvailableContents);
	AddFunctionType(17, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSecondaryPassword::
			OnNotifyCannotUseContents);
	AddFunctionType(19, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSecondaryPassword::
			OnNotifySecondPasswordConfirm);
	AddFunctionType(20, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestQualifyLogin);
	AddFunctionType(21, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CSecondaryPassword::*)())&CSecondaryPassword::
			OnRequestCheckPasswordValid);
	AddFunctionType(22, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(void (
			CSecondaryPassword::*)())&CSecondaryPassword::OnNotifySystemMsg);
	return true;
}

bool CSecondaryPassword::Execute(const MsgObject& msg)
{
	SetFunctionType(msg.param1);
	return Exec(this, (int)&msg);
}

void CSecondaryPassword::OnRequestRegistPassword(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	std::string password = (const char*)pMsg->param2;
	WSendPacket packet(9);
	packet.EncodeStr(password);
	packet.Send(TO_LOGIN);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CSecondaryPassword::OnResultRegistPassword(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	int error = pMsg->param2;
	char buffer[256] = { 0 };
	int message = 0;
	switch (error)
	{
	case 140101:
	case 140102:
	case 140301:
	case 145001:
	case 145002:
	case 145003:
	case 145004:
	case 145005:
	case 730002:
	case 730003:
		message = 105;
		break;
	}
	sprintf(buffer, "%s : %d",
		_systemmsg::CMessageManager::Instance()->GetMsg(
			(_systemmsg::eMsgIdentifier)message),
		error);
	OnOpenSystemMsg(message, buffer);
}

void CSecondaryPassword::OnRequestStoreCertifyFlag(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	if (*(bool*)pMsg->param2 == true)
		pDoc->SetCertifyFlag();
}

void CSecondaryPassword::OnRequestCheckCertifyFlag()
{
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	if (pDoc->GetCertifyFlag() == true)
		OnOpenPasswordLoginDlg();
	else
		OnOpenSelectCertifyDlg();
}

void CSecondaryPassword::OnOpenPasswordExplaneDlg()
{
	if (m_pExplanePasswordDlg)
		return;
	m_pExplanePasswordDlg = CreateForm<FrExplanePassword>(
		g_pFresh->GetManager(), this, "explanepassword", NULL);
	m_pExplanePasswordDlg->EnableDrag(false);
	m_pExplanePasswordDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnPasswordExplaneFormResult, 0);
}

void CSecondaryPassword::OnOpenPasswordCreateDlg()
{
	if (m_pPasswordInputDlg)
		return;
	m_pPasswordInputDlg = CreateForm<FrPasswordInputDlg>(g_pFresh->GetManager(),
		this, "passwordinputdlg", NULL);
	m_pPasswordInputDlg->EnableDrag(false);
	m_pPasswordInputDlg->SetDlgType(SECONDPWD_INPUT_CREATE);
	m_pPasswordInputDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnInputPasswordFormResult,
		FrENTER);
}

void CSecondaryPassword::OnOpenPasswordLoginDlg()
{
	if (m_pPasswordInputDlg || m_pSelectCertifyDlg)
		return;
	m_pPasswordInputDlg = CreateForm<FrPasswordInputDlg>(g_pFresh->GetManager(),
		this, "passwordinputdlg", NULL);
	m_pPasswordInputDlg->EnableDrag(false);
	m_pPasswordInputDlg->SetDlgType(SECONDPWD_INPUT_LOGIN);
	m_pPasswordInputDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnInputPasswordFormResult,
		FrENTER);
}

void CSecondaryPassword::OnClosePasswordDlg()
{
	if (m_pPasswordInputDlg)
		m_pPasswordInputDlg->Close(FrOK, true);
}

void CSecondaryPassword::OnOpenSelectCertifyDlg()
{
	if (m_pSelectCertifyDlg)
		return;
	m_pSelectCertifyDlg = CreateForm<FrSelectCertification>(
		g_pFresh->GetManager(), this, "selectcertifydlg", NULL);
	m_pSelectCertifyDlg->EnableDrag(false);
	m_pSelectCertifyDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnSelectCertifyFormResult, 0);
}

void CSecondaryPassword::OnCloseSelectCertifyDlg()
{
	if (m_pSelectCertifyDlg)
		m_pSelectCertifyDlg->Close(FrOK, true);
}

void CSecondaryPassword::OnRequestLoginPassword(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	std::string password = (const char*)pMsg->param2;
	WSendPacket packet(10);
	packet.EncodeStr(password);
	packet.Send(TO_LOGIN);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CSecondaryPassword::OnResultLoginPassword(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	int error = pMsg->param2;
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	switch (error)
	{
	case 140101:
	case 140102:
	case 140401:
	case 145005:
	case 145006:
	case 730002:
	case 730003:
	{
		char buffer[256] = { 0 };
		sprintf(buffer, "%s : %d",
			_systemmsg::CMessageManager::Instance()->GetMsg(
				(_systemmsg::eMsgIdentifier)119),
			error);
		OnOpenSystemMsg(119, buffer);
	}
	break;
	case 140402:
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 37,
			(int)_systemmsg::CMessageManager::Instance()->GetMsg(
				(_systemmsg::eMsgIdentifier)111),
			0, 0, 0, 0));
		break;
	case 145008:
		OnOpenSystemMsg(110);
		pDoc->IncreseInvalidInputPassword();
		break;
	}
}

void CSecondaryPassword::OnOpenSelfCertifyWebPage()
{
	if (CBrowser::Instance()->IsOpen())
		return;
	COption::Instance()->vApplyLobbyScreenSize();
	CBrowser::Instance()->SetPagetype(CBrowser::WEBPAGE_SELFCERTIFY);
	CBrowser::Instance()->InitializePostArgument();
	CBrowser::Instance()->AddPostArgument("sn", "9");
	CLoginInfo* loginInfo = CLoginInfo::Instance();
	CBrowser::Instance()->AddPostArgument("mn",
		MakeStr("%d", loginInfo->AuthUid()));
	CBrowser::Instance()->OpenPost(
		"https://qa.pangya.gametree.co.kr/InGame/Certify/Default.aspx", false,
		true);
}

void CSecondaryPassword::OnWaitGameServerList()
{
}

void CSecondaryPassword::OnNotifyPasswordInfo(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	eSecondPasswordState state = (eSecondPasswordState)pMsg->param2;
	int day = pMsg->param3;
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	pDoc->SetPasswordState(state);
	pDoc->SetRemainDay(day);
}

void CSecondaryPassword::OnRequestAvailableContents(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	bool* pAvailable = (bool*)pMsg->param2;
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
	{
		*pAvailable = false;
		return;
	}
	switch (pDoc->GetPasswordState())
	{
	case 1:
	case 3:
	case 4:
		*pAvailable = true;
		return;
	case 2:
		*pAvailable = false;
		break;
	}
	if (pDoc->GetRemainDay() <= 0)
		*pAvailable = true;
}

void CSecondaryPassword::OnNotifyCannotUseContents()
{
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	const char* text = MakeStr(_systemmsg::CMessageManager::Instance()->GetMsg(
								   (_systemmsg::eMsgIdentifier)109),
		pDoc->GetRemainDay());
	_systemmsg::CMessageManager::Instance()->NotifyNormalMessage(text);
}

void CSecondaryPassword::OnNotifySecondPasswordConfirm()
{
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
		return;
	pDoc->SetSecondPasswordConfirm();
}

void CSecondaryPassword::OnRequestQualifyLogin(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	bool* pQualify = (bool*)pMsg->param2;
	CSecondPwdDoc* pDoc = GetDocument<CSecondPwdDoc*>();
	if (pDoc == NULL)
	{
		*pQualify = false;
		return;
	}
	unsigned long provider = CLoginInfo::Instance()->ProvType();
	if (provider == 2)
		*pQualify = pDoc->GetSecondPasswordConfirm() == true;
	else if (provider == 4)
		*pQualify = true;
}

void CSecondaryPassword::OnRequestCheckPasswordValid(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	const std::string* password = (const std::string*)pMsg->param2;
	volatile bool* pValid = (volatile bool*)pMsg->param3;
	*pValid = false;
	switch (_util::VerifyInputPassword(*password))
	{
	case _util::PWD_EMPTY:
		OnOpenSystemMsg(106);
		break;
	case _util::PWD_NOT_NUMBER:
		OnOpenSystemMsg(108);
		break;
	case _util::PWD_TOO_SHORT:
		OnOpenSystemMsg(102);
		break;
	case _util::PWD_TOO_LONG:
		OnOpenSystemMsg(101);
		break;
	default:
		*pValid = true;
		break;
	}
	if (*pValid)
	{
		*pValid = false;
		switch (_util::VerifyStringContents_Limit(*password))
		{
		case _util::PWD_SAME_CHARS:
			OnOpenSystemMsg(112);
			break;
		case _util::PWD_ASCENDING:
			OnOpenSystemMsg(113);
			break;
		case _util::PWD_DESCENDING:
			OnOpenSystemMsg(113);
			break;
		default:
			*pValid = true;
			break;
		}
	}
}

void CSecondaryPassword::OnNotifySystemMsg(int param)
{
	OnOpenSystemMsg(((const MsgObject*)param)->param2);
}

void CSecondaryPassword::OnOpenSystemMsg(int msg)
{
	if (m_pPasswordNotifyDlg)
		return;
	m_pPasswordNotifyDlg = CreateForm<FrPasswordNotifyDlg>(
		g_pFresh->GetManager(), this, "passwordnotify", NULL);
	m_pPasswordNotifyDlg->SetNotifyType(msg);
	m_pPasswordNotifyDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnSystemMsgFormResult, FrESCAPE);
}

void CSecondaryPassword::OnOpenSystemMsg(int msg, const char* text)
{
	if (m_pPasswordNotifyDlg)
		return;
	m_pPasswordNotifyDlg = CreateForm<FrPasswordNotifyDlg>(
		g_pFresh->GetManager(), this, "passwordnotify", NULL);
	m_pPasswordNotifyDlg->SetNotifyType(msg);
	m_pPasswordNotifyDlg->SetMessage(text);
	m_pPasswordNotifyDlg->Open(
		(FRESH_PFN_RESULT)&CSecondaryPassword::OnSystemMsgFormResult, FrESCAPE);
}

bool CSecondaryPassword::OnPasswordExplaneFormResult(int result, FrForm* pForm)
{
	m_pExplanePasswordDlg = NULL;
	return true;
}

bool CSecondaryPassword::OnInputPasswordFormResult(int result, FrForm* pForm)
{
	m_pPasswordInputDlg = NULL;
	return true;
}

bool CSecondaryPassword::OnSelectCertifyFormResult(int result, FrForm* pForm)
{
	m_pSelectCertifyDlg = NULL;
	return true;
}

bool CSecondaryPassword::OnSystemMsgFormResult(int result, FrForm* pForm)
{
	m_pPasswordNotifyDlg = NULL;
	return true;
}
