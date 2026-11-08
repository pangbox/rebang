#include "specialbox.h"
#include "specialboxdlg.h"
#include "gatewayactor.h"
#include "messagemanager.h"
#include "fresh.h"
extern Fresh* g_pFresh;
using namespace _gateway;

bool CSpecialBoxHandler::Initialize()
{
	AddFunctionType(0, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CSpecialBoxHandler::OnReqOpenBox);
	AddFunctionType(1, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(void (
			CSpecialBoxHandler::*)())&CSpecialBoxHandler::OnReqWarningMsg);

	return true;
}

bool CSpecialBoxHandler::Execute(const MsgObject& msg)
{
	SetFunctionType(msg.param1);
	return Exec(this, (int)&msg);
}

bool CSpecialBoxHandler::OnSpecialBoxCloseCallback(int result, FrForm* pForm)
{
	m_pSpecialBoxDlg = NULL;

	return true;
}

bool CSpecialBoxHandler::VerifyHaveKeyItem()
{
	std::list<sItemInfo>& itemList = Doc()->m_myItemList;
	std::list<sItemInfo>::iterator it;
	for (it = itemList.begin(); it != itemList.end(); ++it)
	{
		if ((*it).tid == 0x1A00015C)
			break;
	}
	return it != Doc()->m_myItemList.end() ? true : false;
}

void CSpecialBoxHandler::OnReqOpenBox()
{
	if (!VerifyHaveKeyItem())
	{
		CTaskManager::Instance()->PostMsg(NULL, s_gatewayActor[1], 630, 1,
			0x200B3C, 0, 0);
		return;
	}

	m_pSpecialBoxDlg = CreateForm<FrSpecialBoxDlg>(g_pFresh->GetManager(), this,
		"specialBoxOpen", NULL);

	const char* msg = _systemmsg::CMessageManager::Instance()->GetMsg(
		(_systemmsg::eMsgIdentifier)203);

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(0x1A00015C);

	m_pSpecialBoxDlg->SetMessage(MakeStr(msg, pItem->Name));
	m_pSpecialBoxDlg->Open(
		(FRESH_PFN_RESULT)&CSpecialBoxHandler::OnSpecialBoxCloseCallback, 1);
}

void CSpecialBoxHandler::OnReqWarningMsg(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;
	int type = 0;

	switch (pMsg->param2)
	{
	case 0x200B3B:
		type = 200;
		break;
	case 0x200B3C:
		type = 201;
		break;
	case 0x200B3D:
		type = 202;
		break;
	}

	m_pSpecialBoxDlg = CreateForm<FrSpecialBoxDlg>(g_pFresh->GetManager(), this,
		"specialBoxOpen", NULL);
	m_pSpecialBoxDlg->SetMessageType(type);
	m_pSpecialBoxDlg->DisbleProgressBar();
	m_pSpecialBoxDlg->Open(
		(FRESH_PFN_RESULT)&CSpecialBoxHandler::OnSpecialBoxCloseCallback, 1);
}
