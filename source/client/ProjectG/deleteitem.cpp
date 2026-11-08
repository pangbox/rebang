#include "deleteitem.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "frarea.h"
#include "fredit.h"
#include "actor.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrDeleteItemDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrDeleteItemDlg, FrForm)

ON_FRESH_VI("item", FRCMD_INIT, FrDeleteItemDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrDeleteItemDlg::OnItemOwnerDraw)
ON_FRESH_VI("desc", FRCMD_INIT, FrDeleteItemDlg::OnDescInit)
ON_FRESH_VI("warn", FRCMD_INIT, FrDeleteItemDlg::OnWarnInit)
ON_FRESH_VI("item_num", FRCMD_INIT, FrDeleteItemDlg::OnItemNumInit)
ON_FRESH_VI("num", FRCMD_INIT, FrDeleteItemDlg::OnNumInit)
ON_FRESH_BI("num", FRCMD_ENTERKEY, FrDeleteItemDlg::OnNumEnterKey)
ON_FRESH_VI("num_prev", FRCMD_INIT, FrDeleteItemDlg::OnNumPrevInit)
ON_FRESH_VV("num_prev", FRCMD_LBUTTONUP, FrDeleteItemDlg::OnNumPrevBtnUp)
ON_FRESH_VI("num_next", FRCMD_INIT, FrDeleteItemDlg::OnNumNextInit)
ON_FRESH_VV("num_next", FRCMD_LBUTTONUP, FrDeleteItemDlg::OnNumNextBtnUp)
ON_FRESH_VV("check", FRCMD_LBUTTONUP, FrDeleteItemDlg::OnCheckBtnUp)

END_FRESH_MSGMAP()

FrDeleteItemDlg::FrDeleteItemDlg()
{
	m_typeId = 0;
}

void FrDeleteItemDlg::SetDelItem(unsigned long typeId, unsigned long count)
{
	m_typeId = typeId;
	m_count = count;

	if (m_pItemNum)
		m_pItemNum->SetLine(1, MakeStr("%d", count), 0, false, 0);

	IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(m_typeId);

	if (m_pDesc)
		m_pDesc->AddText(pDesc ? pDesc->Desc : "", false, true);
}

void FrDeleteItemDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDeleteItemDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_pItem->GetRect();

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(m_typeId);
	if (!pItem)
		return;

	const Bitmap* pBmp =
		g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->Icon);

	if (!pBmp)
		pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", pItem->Icon);

	if (pBmp)
		pGDI->DrawTexture(pBmp,
			WRect(rect.x, rect.y, (float)pBmp->Width(), (float)pBmp->Height()),
			0xffffffff, 0);

	pGDI->Print(WPoint(rect.x + 100.0f, rect.y), 2, "\xc0\xcc\xb8\xa7:");
	pGDI->Print(WPoint(rect.x + 110.0f, rect.y), 0, pItem->Name);

	pGDI->Print(WPoint(rect.x + 100.0f, rect.y + 30.0f), 2,
		"\xb7\xb9\xba\xa7:");

	pBmp = g_pFresh->GetManager()->GetBitmap("LEVELS",
		MakeStr("level_%03d", pItem->Level + 1));
	if (pBmp)
		pGDI->DrawTexture(pBmp,
			WRect(rect.x + 110.0f, rect.y + 27.0f, (float)pBmp->Width(),
				(float)pBmp->Height()),
			0xffffffff, 0);

	pGDI->Print(WPoint(rect.x + 170.0f, rect.y + 30.0f), 0, "\xc0\xcc\xbb\xf3");
}

void FrDeleteItemDlg::OnDescInit(int param)
{
	m_pDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDeleteItemDlg::OnWarnInit(int param)
{
	FrEdit* pWarn = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (pWarn)
	{
		pWarn->AddText(
			"\xb3\xaa\xbf\xa1\xb0\xd4 \xc7\xca\xbf\xe4\xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe9\xc0\xbb \xbb\xe8\xc1\xa6\xc7\xcf\xbf\xa9 \xc3\xa2\xb0\xed\xb8\xa6 \xc1\xa4\xb8\xae\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n"
			"\xc7\xca\xbf\xe4\xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe8\xc1\xa6\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false, true);
	}
}

void FrDeleteItemDlg::OnItemNumInit(int param)
{
	m_pItemNum = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDeleteItemDlg::OnNumInit(int param)
{
	m_pNum = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pNum)
	{
		m_pNum->SetLine(1, "1", 0, false, 0);
	}
}

bool FrDeleteItemDlg::OnNumEnterKey(int param)
{
	return false;
}

void FrDeleteItemDlg::OnNumPrevInit(int param)
{
	m_pNumPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumPrev)
	{
		m_pNumPrev->SetPushDelay(0);
	}
}

void FrDeleteItemDlg::OnNumPrevBtnUp()
{
	if (m_pNum)
	{
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num > 1)
			m_pNum->SetLine(1, MakeStr("%d", num - 1), 0, false, 0);
	}
}

void FrDeleteItemDlg::OnNumNextInit(int param)
{
	m_pNumNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumNext)
	{
		m_pNumNext->SetPushDelay(0);
	}
}

void FrDeleteItemDlg::OnNumNextBtnUp()
{
	if (m_pNum)
	{
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num < 99)
			m_pNum->SetLine(1, MakeStr("%d", num + 1), 0, false, 0);
	}
}

void FrDeleteItemDlg::OnCheckBtnUp()
{
	int num = atoi(m_pNum->GetLine(1, false));
	if (num <= 0)
	{
		AfxGetTask()->GetMainActor() << MsgObject(NULL, 0x23,
			(int)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbc\xf6\xb7\xae\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (num > m_count)
	{
		AfxGetTask()->GetMainActor() << MsgObject(NULL, 0x23,
			(int)"\xc7\xd1\xb5\xb5\xb8\xa6 \xc3\xca\xb0\xfa \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel");
	pForm->SetMessage(
		"\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xbb\xe8\xc1\xa6\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	pForm->Open((FRESH_PFN_RESULT)&FrDeleteItemDlg::OnConfirmResult, 3);
}

bool FrDeleteItemDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		WSendPacket packet((enumClientPacket)100);
		packet.Encode4(m_typeId);
		packet.Encode4(atoi(m_pNum->GetLine(1, false)));
		packet.Send(TO_GAME);

		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		Close(FrOK, true);
	}

	return true;
}
