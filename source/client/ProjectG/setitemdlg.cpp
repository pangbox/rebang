#include "setitemdlg.h"
#include "frarea.h"
#include "fredit.h"
#include "frlistbox.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrSetItemDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrSetItemDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrSetItemDlg::OnPortraitInit)
ON_FRESH_VI("name", FRCMD_INIT, FrSetItemDlg::OnNameInit)
ON_FRESH_VI("component", FRCMD_INIT, FrSetItemDlg::OnComponentInit)
ON_FRESH_VI("component", FRCMD_OWNERDRAW, FrSetItemDlg::OnComponentOwnerDraw)

END_FRESH_MSGMAP()

FrSetItemDlg::FrSetItemDlg()
{
	m_pPortrait = NULL;
	m_pName = NULL;
	m_pComponent = NULL;
	m_pSetItem = NULL;
}

void FrSetItemDlg::SetSetItem(unsigned long typeId)
{
	if ((typeId & 0xFC000000) == 0x24000000)
	{
		m_pSetItem = ItemManager()->FindSetItem(typeId);

		if (m_pPortrait)
		{
			if (m_pSetItem)
				m_pPortrait->SetBgImg(m_pSetItem->c.Icon);
			else
				m_pPortrait->SetBgImg("hide");
		}

		if (m_pSetItem)
		{
			if (m_pName)
			{
				const char* szType = "";

				switch (m_pSetItem->c.InStock)
				{
				case 2:
					szType = "(\xbc\xb1\xb9\xb0\xc0\xfc\xbf\xeb)";
					break;
				case 3:
					szType = "(\xb1\xb8\xc0\xd4\xc0\xfc\xbf\xeb)";
					break;
				case 4:
					szType = "(\xc0\xfc\xbd\xc3\xbf\xeb)";
					break;
				}

				m_pName->AddText(MakeStr("%s\\c0xffff0000\\c%s\\c0xffff0000\\c",
									 m_pSetItem->c.Name, szType),
					0, true);
			}

			if (m_pComponent)
			{
				for (int i = 0; i < m_pSetItem->nElems; i++)
				{
					IFF_ITEM_COMMON* pCommon =
						ItemManager()->FindCommonItem(m_pSetItem->ElemIds[i]);

					if (pCommon)
						m_pComponent->AddItem(pCommon);
				}
			}

			IFF_STRUCT::sDesc* pDesc =
				ItemManager()->FindDesc(m_pSetItem->c.TypeId);
			if (pDesc)
				SetMessage(pDesc->Desc, false);

			_SYSTEMTIME* pSaleS = NULL;
			_SYSTEMTIME* pSaleE = NULL;

			if (ItemManager()->HasSalePeriod(m_pSetItem->c.TypeId, &pSaleS,
					&pSaleE))
			{
				if (pSaleS->wYear && pSaleE->wYear)
				{
					m_pName->AddText(
						MakeStr(
							"\\c0xffff0000\\c\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf\\c0xff000000\\c",
							pSaleS->wMonth, pSaleS->wDay, pSaleE->wMonth,
							pSaleE->wDay),
						0, true);
				}
				else
				{
					m_pName->AddText("\n", 0, true);
				}
			}
			else
			{
				m_pName->AddText("\n", 0, true);
			}
		}
	}
}

void FrSetItemDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, param);
}

void FrSetItemDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, param);
}

void FrSetItemDlg::OnComponentInit(int param)
{
	m_pComponent = DYNAMIC_CAST(FrListBox, param);
}

void FrSetItemDlg::OnComponentOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;

	IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;
	if (!pCommon)
		return;

	const Bitmap* pBmp =
		g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->Icon);
	if (!pBmp)
		pBmp =
			g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", pCommon->Icon);

	if (pBmp)
	{
		WRect dest(pItem->pos.x, pItem->pos.y, (float)pBmp->Width(),
			(float)pBmp->Height());

		pDevice->DrawTexture(pBmp, dest, 0xffffffff, 0);
	}

	pDevice->SetTextColor(0xff000000, 0xffffffff);
	pDevice->SetTextStyle(1);
	pDevice->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 14.0f), 0, "%s",
		pCommon->Name);

	pDevice->SetTextStyle(0);

	const Bitmap* pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS",
		MakeStr("level_%03d", pCommon->Level + 1));
	if (pLevel)
	{
		WRect dest;
		dest.x = pDevice->GetTextExtend("\xb7\xb9    \xba\xa7:") +
			pItem->pos.x + 102.0f;
		dest.y = pItem->pos.y + 31.0f;
		dest.w = pLevel->Width();
		dest.h = pLevel->Height();
		pDevice->DrawTexture(pLevel, dest, 0xffffffff, 0);

		pDevice->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0,
			"\xb7\xb9    \xba\xa7:");

		if (pCommon->Level > Doc()->m_myInfo.stat.Level)
		{
			pDevice->Print(WPoint(dest.Right() + 5.0f, pItem->pos.y + 34.0f), 0,
				"\xc0\xcc\xbb\xf3");

			pDevice->SetTextColor(0xffff0000, 0xffffffff);
		}
		else
		{
			pDevice->Print(WPoint(dest.Right() + 5.0f, pItem->pos.y + 34.0f), 0,
				"\xc0\xcc\xbb\xf3");
		}
	}
}
