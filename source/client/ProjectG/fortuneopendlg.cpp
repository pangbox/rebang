#include "fortuneopendlg.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrFortuneOpenDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFortuneOpenDlg, FrForm)

ON_FRESH_VI("list", FRCMD_INIT, FrFortuneOpenDlg::OnItemListInit)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrFortuneOpenDlg::OnItemListOwnerDraw)
ON_FRESH_VI("total_price", FRCMD_INIT, FrFortuneOpenDlg::OnTotalPriceInit)
ON_FRESH_VI("desc_items", FRCMD_INIT, FrFortuneOpenDlg::OnDescItemsInit)

END_FRESH_MSGMAP()

FrFortuneOpenDlg::FrFortuneOpenDlg()
{
	m_pItemList = NULL;
	m_pTotalPrice = NULL;
	m_pDescItems = NULL;
	m_typeId = 0;
}

FrFortuneOpenDlg::~FrFortuneOpenDlg()
{
}

void FrFortuneOpenDlg::OnItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrFortuneOpenDlg::OnItemListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	sFortune* pFortune = (sFortune*)pItem->pData;
	if (pFortune == NULL)
		return;

	char desc[64];
	const Bitmap* pBitmap;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pFortune->typeId);

	switch (pFortune->typeId >> 26)
	{
	case 6:
		pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xbe\xc6\xc0\xcc\xc5\xdb\xbc\xa5 > \xb0\xd4\xc0\xd3 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xa5";
			strcpy(desc, p);
		}
		break;
	case 2:
		pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xb3\xbb\xbf\xca\xc0\xe5 > \xc4\xb3\xb8\xaf\xc5\xcd \xb2\xd9\xb9\xcc\xb1\xe2";
			strcpy(desc, p);
		}
		break;
	case 5:
		pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xbe\xc6\xc0\xcc\xc5\xdb\xbc\xa5 > \xc0\xe5\xba\xf1 \xbc\xa5";
			strcpy(desc, p);
		}
		break;
	case 4:
		pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xbe\xc6\xc0\xcc\xc5\xdb\xbc\xa5 > \xc0\xe5\xba\xf1 \xbc\xa5";
			strcpy(desc, p);
		}
		break;
	case 9:
		pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xb3\xbb\xbf\xca\xc0\xe5 > \xc4\xb3\xb8\xaf\xc5\xcd \xb2\xd9\xb9\xcc\xb1\xe2";
			strcpy(desc, p);
		}
		break;
	case 16:
		pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xbe\xc6\xc0\xcc\xc5\xdb\xbc\xa5 > \xb0\xd4\xc0\xd3 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xa5";
			strcpy(desc, p);
		}
		break;
	case 28:
		pBitmap = g_pFresh->GetManager()->GetBitmap("AUXPART", pCommon->Icon);
		{
			const char* p =
				"\xbc\xd2\xc0\xaf\xc0\xa7\xc4\xa1: \xb3\xbb\xbf\xca\xc0\xe5 > \xc4\xb3\xb8\xaf\xc5\xcd \xb2\xd9\xb9\xcc\xb1\xe2";
			strcpy(desc, p);
		}
		break;
	default:
		goto draw_text;
	}

	if (pBitmap)
	{
		WRect rect(pItem->pos.x + 10.0f,
			(m_pItemList->GetItemHeight() - pBitmap->Height()) * 0.5f +
				pItem->pos.y,
			(float)pBitmap->Width(), (float)pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xffffffff, 0);
	}

draw_text:
	pGDI->SetTextStyle(1);

	if ((pFortune->typeId & 0xfc000000) == 0x8000000)
	{
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(
			((pFortune->typeId >> 18) & 0xff) | 0x4000000);
		pGDI->Print(WPoint(pItem->pos.x + 80.0f, pItem->pos.y + 10.0f), 0,
			"%s(%s)", pCommon->Name, pChar ? pChar->c.Name : "");
	}
	else
	{
		pGDI->Print(WPoint(pItem->pos.x + 80.0f, pItem->pos.y + 10.0f), 0,
			pCommon->Name);
	}

	pGDI->SetTextStyle(0);
	pGDI->Print(WPoint(pItem->pos.x + 80.0f, pItem->pos.y + 33.0f), 0,
		"\xb0\xb9   \xbc\xf6: %d\xb0\xb3", pFortune->count);
	pGDI->Print(WPoint(pItem->pos.x + 80.0f, pItem->pos.y + 50.0f), 0, desc);
}

void FrFortuneOpenDlg::OnTotalPriceInit(int param)
{
	m_pTotalPrice = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrFortuneOpenDlg::OnDescItemsInit(int param)
{
	m_pDescItems = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrFortuneOpenDlg::SetItemList(unsigned long* items, int num)
{
	std::list<sFortune>::iterator it;

	m_fortuneList.clear();

	short count;
	for (int i = 0; i < num; i++)
	{
		it = std::find(m_fortuneList.begin(), m_fortuneList.end(), items[i]);

		switch (items[i] >> 26)
		{
		case 5:
		{
			IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(items[i]);
			if (pBall)
				count = pBall->COM[0];
			break;
		}
		case 6:
			if (items[i] & 0x2000000)
			{
				IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(items[i]);
				if (pItem)
					count = pItem->COM[0];
			}
			else
			{
				count = 1;
			}
			break;
		default:
			count = 1;
			break;
		}

		if (it == m_fortuneList.end())
		{
			sFortune fortune;
			fortune.typeId = items[i];
			fortune.count = count;
			m_fortuneList.push_back(fortune);
		}
		else
		{
			(*it).count += count;
		}
	}

	int total = 0;
	if (m_pItemList)
	{
		for (std::list<sFortune>::iterator it = m_fortuneList.begin();
			it != m_fortuneList.end(); ++it)
		{
			sFortune* pFortune = &(*it);
			IFF_ITEM_COMMON* pCommon =
				ItemManager()->FindCommonItem(pFortune->typeId);

			if ((pFortune->typeId >> 26) != 2 && pCommon && pCommon->IsCash)
			{
				if (pCommon->SalePrice == 0)
				{
					if ((pFortune->typeId >> 26) == 6 &&
						!(pFortune->typeId & 0x2000000))
					{
						unsigned int unit = pCommon->Price * 10 / 10;
						if (unit < 1)
							unit = 1;
						total += pFortune->count * unit;
					}
					else
					{
						switch (pFortune->typeId >> 26)
						{
						case 5:
						{
							IFF_STRUCT::sBall* pBall =
								ItemManager()->FindBall(pFortune->typeId);
							if (pBall)
								total += pFortune->count * pCommon->Price * 10 /
									pBall->COM[0];
							break;
						}
						case 6:
						{
							IFF_STRUCT::sItem* pItem =
								ItemManager()->FindItem(pFortune->typeId);
							if (pItem)
								total += pFortune->count * pCommon->Price * 10 /
									pItem->COM[0];
							break;
						}
						default:
							total += pCommon->Price * 10;
							break;
						}
					}
				}
				else
				{
					if ((pFortune->typeId >> 26) == 6 &&
						!(pFortune->typeId & 0x2000000))
					{
						unsigned int unit = pCommon->SalePrice * 10 / 10;
						if (unit < 1)
							unit = 1;
						total += pFortune->count * unit;
					}
					else
					{
						switch (pFortune->typeId >> 26)
						{
						case 5:
						{
							IFF_STRUCT::sBall* pBall =
								ItemManager()->FindBall(pFortune->typeId);
							if (pBall)
								total += pFortune->count * pCommon->SalePrice *
									10 / pBall->COM[0];
							break;
						}
						case 6:
						{
							IFF_STRUCT::sItem* pItem =
								ItemManager()->FindItem(pFortune->typeId);
							if (pItem)
								total += pFortune->count * pCommon->SalePrice *
									10 / pItem->COM[0];
							break;
						}
						default:
							total += pCommon->SalePrice * 10;
							break;
						}
					}
				}

				m_pItemList->AddItem(pFortune);
			}
		}

		m_pItemList->SetKeyFocus(true);
	}

	if (m_pTotalPrice)
	{
		switch (m_typeId & 0x1ffffff)
		{
		case 25:
		case 27:
		case 144:
			m_pTotalPrice->SetCaption(MakeStr(
				"\xbe\xc6\xc0\xcc\xc5\xdb \xb0\xa1\xb0\xdd \xc7\xd5\xb0\xe8 : %d \xc4\xed\xc5\xb0",
				total / 10));
			break;
		}
	}
}

void FrFortuneOpenDlg::SetItemNewList(unsigned long* items, int num)
{
	m_fortuneList.clear();

	for (int i = 0; i < num; i++)
	{
		std::list<sFortune>::iterator it =
			std::find(m_fortuneList.begin(), m_fortuneList.end(), items[i]);

		short count = 1;
		switch (items[i] >> 26)
		{
		case 5:
			count = 1;
			break;
		case 6:
			count = 1;
			break;
		case 28:
			count = 1;
			break;
		}

		if (it == m_fortuneList.end())
		{
			sFortune fortune;
			fortune.typeId = items[i];
			fortune.count = count;
			m_fortuneList.push_back(fortune);
		}
		else
		{
			(*it).count++;
		}
	}

	int total = 0;
	if (m_pItemList)
	{
		for (std::list<sFortune>::iterator it = m_fortuneList.begin();
			it != m_fortuneList.end(); ++it)
		{
			sFortune* pFortune = &(*it);
			IFF_ITEM_COMMON* pCommon =
				ItemManager()->FindCommonItem(pFortune->typeId);
			if (pCommon)
			{
				if (pCommon->SalePrice == 0)
				{
					if ((pFortune->typeId >> 26) == 6 &&
						!(pFortune->typeId & 0x2000000))
					{
						unsigned int unit = pCommon->Price * 10 / 10;
						if (unit < 1)
							unit = 1;
						total += pFortune->count * unit;
					}
					else
					{
						switch (pFortune->typeId >> 26)
						{
						case 5:
						{
							IFF_STRUCT::sBall* pBall =
								ItemManager()->FindBall(pFortune->typeId);
							if (pBall)
								total += pFortune->count * pCommon->Price * 10 /
									pBall->COM[0];
							break;
						}
						case 6:
						{
							IFF_STRUCT::sItem* pItem =
								ItemManager()->FindItem(pFortune->typeId);
							if (pItem)
								total += pFortune->count * pCommon->Price * 10 /
									pItem->COM[0];
							break;
						}
						default:
							total += pCommon->Price * 10;
							break;
						}
					}
				}
				else
				{
					if ((pFortune->typeId >> 26) == 6 &&
						!(pFortune->typeId & 0x2000000))
					{
						unsigned int unit = pCommon->SalePrice * 10 / 10;
						if (unit < 1)
							unit = 1;
						total += pFortune->count * unit;
					}
					else
					{
						switch (pFortune->typeId >> 26)
						{
						case 5:
						{
							IFF_STRUCT::sBall* pBall =
								ItemManager()->FindBall(pFortune->typeId);
							if (pBall)
								total += pFortune->count * pCommon->SalePrice *
									10 / pBall->COM[0];
							break;
						}
						case 6:
						{
							IFF_STRUCT::sItem* pItem =
								ItemManager()->FindItem(pFortune->typeId);
							if (pItem)
								total += pFortune->count * pCommon->SalePrice *
									10 / pItem->COM[0];
							break;
						}
						default:
							total += pCommon->SalePrice * 10;
							break;
						}
					}
				}

				m_pItemList->AddItem(pFortune);
			}
		}

		m_pItemList->SetKeyFocus(true);
	}
}

void FrFortuneOpenDlg::SetTypeId(unsigned long typeId)
{
	m_typeId = typeId;

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
	if (m_pDescItems && pCommon)
		m_pDescItems->SetCaption(MakeStr(
			"%s \xc0\xbb(\xb8\xa6) \xbf\xad\xbe\xee \xb4\xd9\xc0\xbd\xb0\xfa \xb0\xb0\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe9\xc0\xbb \xc8\xb9\xb5\xe6\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			pCommon->Name));
}
