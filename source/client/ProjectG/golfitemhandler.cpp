#include "golfitemhandler.h"
#include "golftask.h"
#include "noticedlg.h"
#include "messagemanager.h"
#include "chatmsg.h"
#include "fresh.h"
#include "gatewayactor.h"
static const float WARNING_RATE = 80.0f;

extern Fresh* g_pFresh;

using namespace _gateway;

CGolfItemHandler::CGolfItemHandler()
	: m_pWizCityNoticeDlg(NULL)
{
}

CGolfItemHandler::~CGolfItemHandler()
{
}

bool CGolfItemHandler::Initialize()
{
	AddFunctionType(0, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::OnRequestResetCalipers);
	AddFunctionType(1, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CGolfItemHandler::*)())&CGolfItemHandler::OnGetFlagCalipers);
	AddFunctionType(2, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::OnSetFlagCalipersEnable);
	AddFunctionType(3, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CGolfItemHandler::OnSetFlagCalipersDisable);
	AddFunctionType(5, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CGolfItemHandler::*)())&CGolfItemHandler::OnSetCountCalipers);
	AddFunctionType(4, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CGolfItemHandler::*)())&CGolfItemHandler::OnGetCountCalipers);
	AddFunctionType(6, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::OnDecreaseCalipers);

	AddFunctionType(7, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CGolfItemHandler::*)())&CGolfItemHandler::OnSetEnableItemTID);
	AddFunctionType(8, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(void (
			CGolfItemHandler::*)())&CGolfItemHandler::OnGetEnableTimeBooster);

	AddFunctionType(9, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::OnIncSpecialBox);
	AddFunctionType(10, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::OnResetSpecialBox);
	AddFunctionType(11, FUNCTIONTYPE_ARG,
		(void (FunctionMapper::*)())(
			void (CGolfItemHandler::*)())&CGolfItemHandler::OnGetSpecialBox);
	AddFunctionType(12, FUNCTIONTYPE_VOID,
		(void (FunctionMapper::*)())&CGolfItemHandler::
			OnCheckSomeMoreGetSpecialBox);

	AddFunctionType(13, FUNCTIONTYPE_VOID,
		(void (
			FunctionMapper::*)())&CGolfItemHandler::OnRequestWizCityNoticeDlg);

	return true;
}

bool CGolfItemHandler::Execute(const MsgObject& msg)
{
	SetFunctionType(msg.param1);
	return Exec(this, (int)&msg);
}

void CGolfItemHandler::OnRequestResetCalipers()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->ResetCalipersVariable();
}

void CGolfItemHandler::OnGetFlagCalipers(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	*(bool*)pMsg->param2 = pDoc->GetEnableCalipers();
}

void CGolfItemHandler::OnSetFlagCalipersEnable()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->EnableCalipers(true);
}

void CGolfItemHandler::OnSetFlagCalipersDisable()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->EnableCalipers(false);
}

void CGolfItemHandler::OnGetCountCalipers(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	*(int*)pMsg->param2 = pDoc->GetCalipersCount();
}

void CGolfItemHandler::OnSetCountCalipers(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->SetCalipersCount(pMsg->param2);
}

void CGolfItemHandler::OnDecreaseCalipers()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->DeceaseCalipersCount();
}

void CGolfItemHandler::OnSetEnableItemTID(int param)
{
	std::vector<unsigned long>* pList =
		(std::vector<unsigned long>*)((const MsgObject*)param)->param2;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	pDoc->ResetUsableItemList();

	int size = pList->size();
	for (int i = 0; i < size; ++i)
	{
		switch (pList->at(i))
		{
		case 0x1a000011:

			pDoc->EnableTimeBooster(true);
			break;
		case 0x1a000040:
			pDoc->EnableCalipers(true);
			break;
		}
	}
}

void CGolfItemHandler::OnGetEnableTimeBooster(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();
	if (pDoc == NULL)
		return;

	*(bool*)pMsg->param2 = pDoc->GetEnableTimeBooster();
}

void CGolfItemHandler::OnIncSpecialBox()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();

	pDoc->IncreaseSpecialBoxCount();
}

void CGolfItemHandler::OnResetSpecialBox()
{
	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();

	pDoc->ResetSpecialBoxCount();

	const std::list<sItemInfo>& itemList = Doc()->GetConstMyItemList();
	std::list<sItemInfo>::const_iterator it;
	for (it = itemList.begin(); it != itemList.end(); ++it)
	{
		if ((*it).tid == 0x1a00015b)
			break;
	}
	if (it == Doc()->GetConstMyItemList().end())

		pDoc->SetOwnSpecialBoxCount(0);
	else

		pDoc->SetOwnSpecialBoxCount((*it).Common[0]);
}

void CGolfItemHandler::OnGetSpecialBox(int param)
{
	const MsgObject* pMsg = (const MsgObject*)param;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();

	*(int*)pMsg->param2 = pDoc->GetSpecialBoxCount();
}

bool _gateway::IsSpecialBoxArea()
{
	bool b = GetCurMap() == 19;
	return b;
}

void CGolfItemHandler::OnCheckSomeMoreGetSpecialBox()
{
	CGolfTask* pTask =
		(CGolfTask*)AfxGetTask()->DynamicCast(&CGolfTask::m_RTTI);
	if (pTask == NULL)
		return;

	if (!pTask->DetermineWhetherToAddActor_GroundItemMan())
		return;

	if (GetCurMap() != 19)
		return;

	CGolfItemDoc* pDoc = GetDocument<CGolfItemDoc*>();

	int count = pDoc->GetOwnSpecialBoxCount() + pDoc->GetSpecialBoxCount();

	int remain = CalcRemainMoreGetSpecialBoxCount(count);
	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(0x1a00015b);

	const char* str = MakeStr(_systemmsg::CMessageManager::Instance()->GetMsg(
								  (_systemmsg::eMsgIdentifier)300),
		pItem->Name, 50);
	CChatMsg::Instance()->AddChatMsg(str, 0xffff3030, true, false);

	if (remain < 1)
	{
		str = MakeStr(_systemmsg::CMessageManager::Instance()->GetMsg(
						  (_systemmsg::eMsgIdentifier)301),
			pItem->Name);
		CChatMsg::Instance()->AddChatMsg(str, 0xffff3030, true, false);
	}
	else
	{
		float rate = remain * 2.0f;
		if (WARNING_RATE <= rate)
		{
			str = MakeStr(_systemmsg::CMessageManager::Instance()->GetMsg(
							  (_systemmsg::eMsgIdentifier)302),
				pDoc->GetOwnSpecialBoxCount(), remain);
			CChatMsg::Instance()->AddChatMsg(str, 0xffff3030, true, false);
		}
	}
}

void CGolfItemHandler::OnRequestWizCityNoticeDlg()
{
	m_pWizCityNoticeDlg = CreateForm<FrNoticeDlg>(g_pFresh->GetManager(), this,
		"notice_popup", NULL);
	m_pWizCityNoticeDlg->LoadOnly("hat_event_explanation.jpg");
	m_pWizCityNoticeDlg->Open(
		(FRESH_PFN_RESULT)&CGolfItemHandler::OnCloseWizCityNoticeResult, 17);
}

int CGolfItemHandler::CalcRemainMoreGetSpecialBoxCount(int count)
{
	return 50 - count;
}

bool CGolfItemHandler::OnCloseWizCityNoticeResult(int result, FrForm* pForm)
{
	m_pWizCityNoticeDlg = NULL;

	return true;
}
