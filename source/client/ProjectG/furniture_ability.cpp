#include "furniture_ability.h"
#include "standinginline.h"
#include "frlistbox.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "wfont.h"
#include "wresrcmng.h"

extern Fresh* g_pFresh;
extern WResourceManager* g_resrcmng;

IMPLEMENT_OBJECT(CFurniture_AbilityDlg, FrForm)

BEGIN_FRESH_MSGMAP(CFurniture_AbilityDlg, FrForm)

ON_FRESH_VI("myitem_list", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_ItemListInit)
ON_FRESH_VI("myitem_list", FRCMD_OWNERDRAW,
	CFurniture_AbilityDlg::OnFurnitureAbility_ItemListOwnerDraw)
ON_FRESH_VV("myitem_list", FRCMD_LBUTTONDOWN,
	CFurniture_AbilityDlg::OnFurnitureAbility_ItemListBtnDown)
ON_FRESH_VV("myitem_list", FRCMD_MOUSEMOVE,
	CFurniture_AbilityDlg::OnFurnitureAbility_ItemListMouseMove)

ON_FRESH_VI("select_itemlist", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_SelectListInit)
ON_FRESH_VI("select_item_img", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_ImgSelect)

ON_FRESH_VI("Edit_array", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_EditArrayInit)

ON_FRESH_VI("Btn_Array", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_BtnArrayInit)
ON_FRESH_VI("select_text_img", FRCMD_INIT,
	CFurniture_AbilityDlg::OnFurnitureAbility_TextImg)

END_FRESH_MSGMAP()

CFurniture_AbilityDlg::CFurniture_AbilityDlg()
{
	m_reserved = 0;
	m_pBtnArray = NULL;
	m_pEditArray = NULL;

	m_pSelectBitmap = g_pFresh->GetBitmap("rmr_select_n");
	m_pSelectDisableBitmap = g_pFresh->GetBitmap("rmr_select_d");

	m_pLineBitmap = g_pFresh->GetBitmap("rmr_line");
	static char* s_fontWind = "[font_wind.jpg";
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = &s_fontWind;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";

	m_pWindFont = g_resrcmng->GetTitleFont();
	m_pWindFont->Create(&info);
	static char* s_fontBong = "[font_bong.jpg";
	WTITLEFONT info2;
	memset(&info2, 0, sizeof(info2));
	info2.filename = &s_fontBong;
	info2.fontw = 9;
	info2.fonth = 16;
	info2.numPages = 1;
	info2.texw = 128;
	info2.texh = 32;
	info2.pCharSet = "1234567890,-";

	m_pBongFont = g_resrcmng->GetTitleFont();
	m_pBongFont->Create(&info2);

	SetList();
}

CFurniture_AbilityDlg::~CFurniture_AbilityDlg()
{
	if (m_pWindFont)
	{
		m_pWindFont->Erase();
	}

	if (m_pBongFont)
	{
		m_pBongFont->Erase();
	}

	if (g_resrcmng && m_pWindFont)
	{
		g_resrcmng->Release(m_pWindFont);
		m_pWindFont = NULL;
	}
	if (g_resrcmng && m_pBongFont)
	{
		g_resrcmng->Release(m_pBongFont);
		m_pBongFont = NULL;
	}
}

void CFurniture_AbilityDlg::OnFurnitureAbility_ItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void CFurniture_AbilityDlg::OnFurnitureAbility_ItemListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (pItem->pData == NULL)
		return;

	const Bitmap* pSelect;

	if (pItem->selected)
	{
		pSelect = m_pSelectDisableBitmap;
		gdi->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (pItem->underCursor)
	{
		pSelect = m_pSelectBitmap;
		gdi->SetTextColor(0xff000000, 0xffffffff);
	}
	else
	{
		pSelect = NULL;
		gdi->SetTextColor(0xff000000, 0xffffffff);
	}

	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	WRect rect;
	WRect clientRect;
	GetClientRect(clientRect);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			rect = WRect(j * 92.0f + clientRect.x + 126.0f,
				i * 110.0f + 87.0f + clientRect.y,
				(float)m_pLineBitmap->Width(), (float)m_pLineBitmap->Height());
			gdi->DrawTexture(m_pLineBitmap, rect, 0xffffffff, 0);
		}
	}

	if (pSelect)
	{
		gdi->DrawTexture(pSelect,
			WRect(pItem->pos.x - 2.0f, pItem->pos.y, (float)pSelect->Width(),
				(float)pSelect->Height()),

			0xffffffff, 0);
	}

	if ((pInfo->tid & 0xfc000000) == 0x8000000)
	{
		IFF_STRUCT::sChar* pChar =
			ItemManager()->FindChar(((pInfo->tid >> 18) & 0xff) | 0x4000000);

		if (pChar)
		{
			const Bitmap* pBitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", pChar->c.Icon));

			if (pBitmap)
			{
				gdi->DrawTexture(pBitmap,
					WRect(pItem->pos.x + 7.0f, pItem->pos.y + 5.0f,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					0xffffffff, 0);
			}
		}
	}
}

void CFurniture_AbilityDlg::OnFurnitureAbility_ItemListBtnDown()
{
	FrListItem* pItem = m_pItemList->GetItemUnderCursor();

	if (pItem == NULL)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xbc\xb1\xc5\xc3\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	if (pInfo == NULL)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	m_pSelectedItem = pInfo;
}

void CFurniture_AbilityDlg::OnFurnitureAbility_ItemListMouseMove()
{
}

void CFurniture_AbilityDlg::OnFurnitureAbility_ImgSelect(int param)
{
	m_pSelectImg = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pSelectImg)

		m_pSelectImg->SetVisible(false);
}

void CFurniture_AbilityDlg::OnFurnitureAbility_SelectListInit(int param)
{
	m_pSelectList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pSelectList)

		m_pSelectList->SetVisible(false);
}

void CFurniture_AbilityDlg::OnFurnitureAbility_EditArrayInit(int param)
{
	m_pEditArray = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	if (m_pEditArray)

		m_pEditArray->SetVisible(false);
}

void CFurniture_AbilityDlg::OnFurnitureAbility_BtnArrayInit(int param)
{
	m_pBtnArray = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pBtnArray)

		m_pBtnArray->SetVisible(false);
}

void CFurniture_AbilityDlg::OnFurnitureAbility_TextImg(int param)
{
	m_pTextImg = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (m_pTextImg)

		m_pTextImg->SetVisible(false);
}

void CFurniture_AbilityDlg::SetList()
{
	for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); ++it)
	{
		sItemInfo* pInfo = &(*it);

		if (ItemManager()->GetCouponKind(pInfo->tid) != 0)
			continue;

		if (pInfo->Common[4] == 0)
			continue;
		m_pItemList->AddItem(pInfo);
	}
}

void CFurniture_AbilityDlg::GetItemInfo(sItemInfo* info)
{
}
