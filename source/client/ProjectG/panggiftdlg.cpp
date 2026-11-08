#include "panggiftdlg.h"
#include "frarea.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "commonutil.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrPangGiftDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrPangGiftDlg, FrForm)

ON_FRESH_VI("my_pang", FRCMD_INIT, FrPangGiftDlg::OnMyPangInit)
ON_FRESH_VI("send_pang", FRCMD_INIT, FrPangGiftDlg::OnSendPangInit)
ON_FRESH_VI("total_pang", FRCMD_INIT, FrPangGiftDlg::OnTotalPangInit)
ON_FRESH_VI("item", FRCMD_INIT, FrPangGiftDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrPangGiftDlg::OnItemOwnerDraw)

END_FRESH_MSGMAP()

FrPangGiftDlg::FrPangGiftDlg()
{
	m_pMyPang = NULL;
	m_pSendPang = NULL;
	m_pTotalPang = NULL;

	m_pItemInfo = NULL;
}

void FrPangGiftDlg::OnMyPangInit(int param)
{
	m_pMyPang = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pMyPang)
		m_pMyPang->SetLine(1,
			MakeStr("%I64d\xc6\xce", Doc()->m_myInfo.stat.i64Pang), 0, false,
			0);
}

void FrPangGiftDlg::OnSendPangInit(int param)
{
	m_pSendPang = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPangGiftDlg::OnTotalPangInit(int param)
{
	m_pTotalPang = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPangGiftDlg::SetItemInfo(sGiftInfo* info)
{
	if (m_pSendPang)
	{
		m_pSendPang->SetLine(1, MakeStr("%d\xc6\xce", info->Arg0), 0, false, 0);
	}

	if (m_pTotalPang)
	{
		m_pTotalPang->SetLine(1,
			MakeStr("%I64d\xc6\xce", Doc()->m_myInfo.stat.i64Pang + info->Arg0),
			0, false, 0);
	}

	m_pItemInfo = info;
}

void FrPangGiftDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPangGiftDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(0x1A000010);
	if (pItem == NULL)
		return;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->Icon);
	if (pBitmap)
	{
		WRect rect(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 30.0f,
			(float)pBitmap->Width(), (float)pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(175.0f, 15.0f),
		2, "%s", pItem->Name);
	pGDI->SetTextStyle(0);

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);
	pGDI->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(170.0f, 40.0f),
		2, "%d\xc6\xce", m_pItemInfo->Arg0);
}
