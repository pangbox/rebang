#include "s5/rentalextensiondlg.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fredit.h"
#include "frbutton.h"

extern Fresh* g_pFresh;

namespace S5
{
	IMPLEMENT_OBJECT(FrRentalExtensionDlg, FrForm)

	BEGIN_FRESH_MSGMAP(FrRentalExtensionDlg, FrForm)

	ON_FRESH_VV("ar_dialog", FRCMD_OWNERDRAW,
		FrRentalExtensionDlg::OnDialogAreaDraw)
	ON_FRESH_VV("bt_extension", FRCMD_LBUTTONUP,
		FrRentalExtensionDlg::OnExtensionBtnUp)
	ON_FRESH_VV("bt_destroy", FRCMD_LBUTTONUP,
		FrRentalExtensionDlg::OnDestroyBtnUp)
	ON_FRESH_VV("bt_cancel", FRCMD_LBUTTONUP,
		FrRentalExtensionDlg::OnCancelBtnUp)

	END_FRESH_MSGMAP()

	FrRentalExtensionDlg::FrRentalExtensionDlg()

	{
		m_pItemInfo = NULL;
	}

	FrRentalExtensionDlg::~FrRentalExtensionDlg()
	{
		if (m_pItemInfo)
		{
			delete m_pItemInfo;
			m_pItemInfo = NULL;
		}
	}

	void FrRentalExtensionDlg::Initialize(const sItemInfo* pItemInfo)
	{
		m_pItemInfo = new sItemInfo;
		*m_pItemInfo = *pItemInfo;

		FrEdit* pEdit = DYNAMIC_CAST(FrEdit, FindChildByName("ed_name"));
		if (pEdit)
			pEdit->AddText(ItemManager()->GetItemName(m_pItemInfo->tid), false,
				true);

		pEdit = DYNAMIC_CAST(FrEdit, FindChildByName("ed_desc"));
		if (pEdit)
		{
			pEdit->AddText(
				"\\c0xffff0000\\c\xc7\xd8\xb4\xe7 \xc0\xc7\xbb\xf3\xc0\xc7 \xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3\xc0\xcc \xb8\xb8\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
				false, true);

			pEdit->AddText(
				MakeStr(
					"\\c0xff000000\\c- \xbb\xe7\xbf\xeb \xb1\xe2\xb0\xa3\xc0\xbb \xbf\xac\xc0\xe5\xc7\xcf\xbd\xc7 \xb0\xe6\xbf\xec \xc1\xef\xbd\xc3 %s \xc2\xf7\xb0\xa8 \xb5\xcb\xb4\xcf\xb4\xd9.",
					"\xc6\xce\xc0\xcc"),
				false, true);
		}

		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(m_pItemInfo->tid);
		pEdit = DYNAMIC_CAST(FrEdit, FindChildByName("ed_price"));
		if (pEdit && pPart)
		{
			pEdit->AddText(
				MakeStr(
					"\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5 \xba\xf1\xbf\xeb : %d%s(%d\xc0\xcf)",
					pPart->RentalPrice, "\xc6\xce", 7),
				false, true);
		}

		FrButton* pExtension =
			DYNAMIC_CAST(FrButton, FindChildByName("bt_extension"));
		FrButton* pDestroy =
			DYNAMIC_CAST(FrButton, FindChildByName("bt_destroy"));
		if (!pPart)
		{
			if (pExtension)
			{
				pExtension->Enable(false);
				pExtension->SetToolTipText(
					"\xb4\xeb\xbf\xa9 \xb1\xe2\xb4\xc9 \xbb\xe7\xbf\xeb \xba\xd2\xb0\xa1");
			}

			if (pDestroy)
			{
				pDestroy->Enable(false);
				pDestroy->SetToolTipText(
					"\xb4\xeb\xbf\xa9 \xb1\xe2\xb4\xc9 \xbb\xe7\xbf\xeb \xba\xd2\xb0\xa1");
			}
		}
		else
		{
			if (pExtension)
			{
				bool bCookie = false;
				if (bCookie)
				{
					if (MyCookie() < pPart->RentalPrice)
					{
						pExtension->Enable(false);
						pExtension->SetToolTipText(
							"\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
						pExtension->EnableToolTip(true);
					}
				}
				else if (MyPang() < pPart->RentalPrice)
				{
					pExtension->Enable(false);
					pExtension->SetToolTipText(
						"\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
					pExtension->EnableToolTip(true);
				}
			}

			if (pDestroy)
			{
				for (int i = 0; i < 12; i++)
				{
					if (m_pItemInfo->attachCard[i])
					{
						pDestroy->Enable(false);
						pDestroy->SetToolTipText(
							"\xc4\xab\xb5\xe5 \xc3\xdf\xc3\xe2\xbe\xd7\xc0\xb8\xb7\xce \xc4\xab\xb5\xe5\xb8\xa6 \xb8\xd5\xc0\xfa \xc3\xdf\xc3\xe2\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.");
						pDestroy->EnableToolTip(true);
						break;
					}
				}
			}
		}
	}

	std::string FrRentalExtensionDlg::SetChildWindowPos(
		const tweaker_param& param)

	{
		float x, y = 0;
		FrWnd* pChild = FindChildByName(param[0].to_cstr());
		if (pChild)
		{
			x = param[1].to_float();
			y = param[2].to_float();

			WRect rect = pChild->GetRect();
			rect.x = x + m_rect.x;
			rect.y = y + m_rect.y;
			pChild->SetRect(rect);

			return "changed child window position.";
		}
		return "not found child window";
	}

	std::string FrRentalExtensionDlg::GetChildWindowPos(
		const tweaker_param& param)
	{
		FrWnd* pChild = FindChildByName(param[0].to_cstr());
		if (pChild)
		{
			WRect rect = pChild->GetRect();
			return MakeStr("x : %f, y : %f", rect.x - m_rect.x,
				rect.y - m_rect.y);
		}
		return "not found child window";
	}

	void FrRentalExtensionDlg::OnDialogAreaDraw()

	{
		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem(m_pItemInfo->tid);
		if (pItem)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pItem->Icon);
			pGDI->DrawTexture(pBmp,
				WRect(m_rect.x + 35.0f, m_rect.y + 25.0f, (float)pBmp->Width(),
					(float)pBmp->Height()),
				0xffffffff, 0);
		}
	}

	void FrRentalExtensionDlg::OnExtensionBtnUp()
	{
		FrForm::Close(FrYES, true);
	}

	void FrRentalExtensionDlg::OnDestroyBtnUp()

	{
		FrForm::Close(FrNO, true);
	}

	void FrRentalExtensionDlg::OnCancelBtnUp()
	{
		FrForm::Close(FrCANCEL, true);
	}
}
