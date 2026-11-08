#include "repaydlg.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "binstr.h"

IMPLEMENT_OBJECT(FrRepayDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrRepayDlg, FrForm)

ON_FRESH_VI("cart", FRCMD_INIT, FrRepayDlg::OnCartInit)
ON_FRESH_VI("cart", FRCMD_OWNERDRAW, FrRepayDlg::OnCartOwnerDraw)
ON_FRESH_VI("price", FRCMD_INIT, FrRepayDlg::OnPriceInit)
ON_FRESH_VI("my_money", FRCMD_INIT, FrRepayDlg::OnMyMoneyInit)
ON_FRESH_VI("money_left", FRCMD_INIT, FrRepayDlg::OnMoneyLeftInit)
ON_FRESH_VI("price_cookie", FRCMD_INIT, FrRepayDlg::OnPrice_CookieInit)
ON_FRESH_VI("my_money_cookie", FRCMD_INIT, FrRepayDlg::OnMyMoney_CookieInit)
ON_FRESH_VI("money_left_cookie", FRCMD_INIT, FrRepayDlg::OnMoneyLeft_CookieInit)

END_FRESH_MSGMAP()

FrRepayDlg::FrRepayDlg()
{
	memset(&m_item, 0, sizeof(m_item));
	m_guid = 0;
	m_pCart = NULL;
	m_pPrice = NULL;
	m_pMyMoney = NULL;
	m_pMoneyLeft = NULL;
	m_pPriceCookie = NULL;
	m_pMyMoneyCookie = NULL;
	m_pMoneyLeftCookie = NULL;
}

void FrRepayDlg::Repay(const IFF_ITEM_COMMON& item, unsigned long guid,
	__int64 myPang, __int64 myCookie)
{
	char buf[32];

	m_item = item;
	m_guid = guid;
	m_pang = 0;
	m_cookie = 0;

	if (m_pCart)
	{
		m_pCart->ClearItem();

		if (m_item.IsCash)
			m_cookie += m_item.UsedPrice;
		else
			m_pang += m_item.UsedPrice;

		m_pCart->AddItem(&m_item);
	}

	if (m_pPrice)
	{
		MakePriceStrA(buf, m_pang);
		strcat(buf, "\xc6\xce");
		m_pPrice->SetLine(1, buf, 0, false, 0);
	}

	if (m_pPriceCookie)
	{
		sprintf(buf, "\xc4\xed\xc5\xb0 %I64d\xb0\xb3", m_cookie);
		m_pPriceCookie->SetLine(1, buf, 0, false, 0);
	}

	if (m_pMyMoney)
	{
		MakePriceStrA(buf, myPang);
		strcat(buf, "\xc6\xce");
		m_pMyMoney->SetLine(1, buf, 0, false, 0);
	}

	if (m_pMyMoneyCookie)
	{
		sprintf(buf, "\xc4\xed\xc5\xb0 %I64d\xb0\xb3", myCookie);
		m_pMyMoneyCookie->SetLine(1, buf, 0, false, 0);
	}

	if (m_pPrice && m_pMyMoney && m_pang)
	{
		MakePriceStrA(buf, m_pang);
		strcat(buf, "\xc6\xce");
		m_pPrice->SetLine(1, buf, 0, false, 0);

		if (Doc()->m_myInfo.stat.i64Pang + m_pang < 0)
		{
			if (m_pMoneyLeft)
			{
				MakePriceStrA(buf, myPang + m_pang);
				strcat(buf, " \xba\xce\xc1\xb7");
				m_pMoneyLeft->SetLine(1, buf, 0, false, 0);
			}

			FrWnd* pOk = FindChildByName("ok");
			if (pOk)
				pOk->Enable(false);
		}
		else
		{
			if (m_pMoneyLeft)
			{
				MakePriceStrA(buf, myPang + m_pang);
				strcat(buf, "\xc6\xce");
				m_pMoneyLeft->SetLine(1, buf, 0, false, 0);
			}

			FrWnd* pOk = FindChildByName("ok");
			if (pOk)
				pOk->Enable(true);
		}
	}
	else if (m_pMoneyLeft)
	{
		m_pMoneyLeft->SetLine(1, m_pMyMoney->GetLine(1, false), 0, false, 0);
	}

	if (m_pPriceCookie && m_pMyMoneyCookie && m_cookie)
	{
		sprintf(buf, "\xc4\xed\xc5\xb0 %I64d\xb0\xb3", m_cookie);
		m_pPriceCookie->SetLine(1, buf, 0, false, 0);

		__int64 cookie = Doc()->m_cookie;
		if (cookie + m_cookie < 0)
		{
			if (m_pMoneyLeftCookie)
			{
				MakePriceStrA(buf, cookie + m_cookie);
				strcat(buf, " \xba\xce\xc1\xb7");
				m_pMoneyLeftCookie->SetLine(1, buf, 0, false, 0);
			}

			FrWnd* pOk = FindChildByName("ok");
			if (pOk)
				pOk->Enable(false);
		}
		else
		{
			if (m_pMoneyLeftCookie)
			{
				sprintf(buf, "\xc4\xed\xc5\xb0 %I64d\xb0\xb3",
					cookie + m_cookie);
				m_pMoneyLeftCookie->SetLine(1, buf, 0, false, 0);
			}

			FrWnd* pOk = FindChildByName("ok");
			if (pOk)
				pOk->Enable(true);
		}
	}
	else if (m_pMoneyLeftCookie)
	{
		m_pMoneyLeftCookie->SetLine(1, m_pMyMoneyCookie->GetLine(1, false), 0,
			false, 0);
	}
}

void FrRepayDlg::OnCartInit(int param)
{
	m_pCart = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrRepayDlg::OnCartOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGDI = m_pCart->WndManager()->GetGDI();
	IFF_ITEM_COMMON* pData = (IFF_ITEM_COMMON*)pItem->pData;

	const Bitmap* pBitmap =
		m_pCart->WndManager()->GetBitmap("ITEMS", pData->Icon);
	if (pBitmap == NULL)
		pBitmap =
			m_pCart->WndManager()->GetBitmap("ITEMS_FASHION", pData->Icon);

	if (pBitmap)
	{
		WRect rect(pItem->pos.x, pItem->pos.y, pBitmap->Width(),
			pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xffffffff, 0);
	}

	pGDI->SetTextColor(0xff000000, 0xffffffff);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 14.0f), 0, "%s",
		pData->Name);
	pGDI->SetTextStyle(0);

	if (pData->IsCash)
	{
		pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0,
			"\xbf\xf8\xb7\xa1\xb0\xa1\xb0\xdd:   \xc4\xed\xc5\xb0 %d\xb0\xb3",
			pData->Price);
		pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 54.0f), 0,
			"\xc8\xaf\xba\xd2\xb0\xa1\xb0\xdd:   \xc4\xed\xc5\xb0 %d\xb0\xb3",
			pData->UsedPrice);
	}
	else
	{
		pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0,
			"\xbf\xf8\xb7\xa1\xb0\xa1\xb0\xdd:   %d\xc6\xce", pData->Price);
		pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 54.0f), 0,
			"\xc8\xaf\xba\xd2\xb0\xa1\xb0\xdd:   %d\xc6\xce", pData->UsedPrice);
	}

	if ((pData->TypeId & 0xfc000000) == 0x1c000000)
	{
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 74.0f), 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
		float x =
			pGDI->GetTextExtend("\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") + 95.0f;
		x += pItem->pos.x;
		pGDI->Print(WPoint(x, pItem->pos.y + 74.0f), 0,
			"\xb1\xb8\xc0\xd4\xc0\xcf\xb7\xce \xba\xce\xc5\xcd %d\xc0\xcf \xb5\xbf\xbe\xc8\xb8\xb8",
			30);
	}
}

void FrRepayDlg::OnPriceInit(int param)
{
	m_pPrice = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRepayDlg::OnMyMoneyInit(int param)
{
	m_pMyMoney = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRepayDlg::OnMoneyLeftInit(int param)
{
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRepayDlg::OnPrice_CookieInit(int param)
{
	m_pPriceCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRepayDlg::OnMyMoney_CookieInit(int param)
{
	m_pMyMoneyCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRepayDlg::OnMoneyLeft_CookieInit(int param)
{
	m_pMoneyLeftCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}
