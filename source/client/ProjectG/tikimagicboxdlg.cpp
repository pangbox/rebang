#include "tikimagicboxdlg.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrTikiMagicBoxDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrTikiMagicBoxDlg, FrForm)

ON_FRESH_VI("back_img", FRCMD_OWNERDRAW,
	FrTikiMagicBoxDlg::OnBackImageOwnerDraw)
ON_FRESH_VI("myitem_list", FRCMD_INIT, FrTikiMagicBoxDlg::OnMyItemListInit)
ON_FRESH_VI("myitem_list", FRCMD_OWNERDRAW,
	FrTikiMagicBoxDlg::OnMyItemListOwnerDraw)
ON_FRESH_VV("myitem_list", FRCMD_DBLCLICK,
	FrTikiMagicBoxDlg::OnMyItemListDBClick)
ON_FRESH_VV("myitem_list", FRCMD_LBUTTONDOWN,
	FrTikiMagicBoxDlg::OnMyItemListLBDown)
ON_FRESH_VV("myitem_list", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnMyItemListLBUp)
ON_FRESH_VI("mtr_list", FRCMD_INIT, FrTikiMagicBoxDlg::OnMtrListInit)
ON_FRESH_VI("mtr_list", FRCMD_OWNERDRAW, FrTikiMagicBoxDlg::OnMtrListOwnerDraw)
ON_FRESH_VV("mtr_list", FRCMD_DBLCLICK, FrTikiMagicBoxDlg::OnMtrListDBClick)
ON_FRESH_VV("mtr_list", FRCMD_LBUTTONDOWN, FrTikiMagicBoxDlg::OnMtrListLBDown)
ON_FRESH_VV("mtr_list", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnMtrListLBUp)
ON_FRESH_VI("category_list", FRCMD_INIT, FrTikiMagicBoxDlg::OnCategoryListInit)
ON_FRESH_VI("category_list", FRCMD_OWNERDRAW,
	FrTikiMagicBoxDlg::OnCategoryListOwnerDraw)
ON_FRESH_VV("category_list", FRCMD_LBUTTONDOWN,
	FrTikiMagicBoxDlg::OnCategoryListLBDown)
ON_FRESH_VI("character_list", FRCMD_INIT,
	FrTikiMagicBoxDlg::OnCharacterListInit)
ON_FRESH_VI("character_list", FRCMD_OWNERDRAW,
	FrTikiMagicBoxDlg::OnCharacterListOwnerDraw)
ON_FRESH_VV("character_list", FRCMD_LBUTTONDOWN,
	FrTikiMagicBoxDlg::OnCharacterListLBDown)
ON_FRESH_VI("help_btn", FRCMD_INIT, FrTikiMagicBoxDlg::OnHelpBtnInit)
ON_FRESH_VV("help_btn", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnHelpBtnLBUp)
ON_FRESH_VI("close_btn", FRCMD_INIT, FrTikiMagicBoxDlg::OnCloseBtnInit)
ON_FRESH_VV("close_btn", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnCloseBtnLBUp)
ON_FRESH_VI("char_tab", FRCMD_INIT, FrTikiMagicBoxDlg::OnCharTabInit)
ON_FRESH_VV("char_tab", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnCharTabLBUp)
ON_FRESH_VI("item_tab", FRCMD_INIT, FrTikiMagicBoxDlg::OnItemTabInit)
ON_FRESH_VV("item_tab", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnItemTabLBUp)
ON_FRESH_VI("insert_btn", FRCMD_INIT, FrTikiMagicBoxDlg::OnInsertBtnInit)
ON_FRESH_VV("insert_btn", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnInsertBtnLBUp)
ON_FRESH_VI("mix_btn", FRCMD_INIT, FrTikiMagicBoxDlg::OnMixBtnInit)
ON_FRESH_VV("mix_btn", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnMixBtnLBUp)
ON_FRESH_VI("cancel_btn", FRCMD_INIT, FrTikiMagicBoxDlg::OnCancelInit)
ON_FRESH_VV("cancel_btn", FRCMD_LBUTTONUP, FrTikiMagicBoxDlg::OnCancelLBUp)

END_FRESH_MSGMAP()

void FrTikiMagicBoxDlg::OnBackImageOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	DrawBitmapOnDlg(0.0f, 0.0f, "tiki_bg_1");
	DrawBitmapOnDlg(256.0f, 0.0f, "tiki_bg_2");
	DrawBitmapOnDlg(512.0f, 0.0f, "tiki_bg_3");
	DrawBitmapOnDlg(0.0f, 256.0f, "tiki_bg_4");
	DrawBitmapOnDlg(256.0f, 256.0f, "tiki_bg_5");
	DrawBitmapOnDlg(512.0f, 256.0f, "tiki_bg_6");
	DrawBitmapOnDlg(480.0f, 17.0f, "tiki_x_l");
	DrawBitmapOnDlg(38.0f, 137.0f, "tiki_tab_box_left");
	DrawBitmapOnDlg(257.0f, 137.0f, "tiki_tab_box_right");

	char buf[128] = "\0";
	sprintf(buf,
		"%d\xb9\xf8\xc2\xb0 \xc0\xe7\xb7\xe1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb3\xd6\xbe\xee \xc1\xd6\xbc\xbc\xbf\xe4.",
		m_mtrIndex + 1);
	pGDI->SetTextStyle(1);
	PrintTextOnDlg(215.0f, 285.0f, buf, 0xffff0000);
	pGDI->SetTextStyle(0);
}

void FrTikiMagicBoxDlg::OnMyItemListInit(int param)
{
	BindListBox(m_pMyItemList, param);
}

void FrTikiMagicBoxDlg::OnMyItemListOwnerDraw(int param)
{
}

void FrTikiMagicBoxDlg::OnMyItemListDBClick()
{
}

void FrTikiMagicBoxDlg::OnMyItemListLBDown()
{
}

void FrTikiMagicBoxDlg::OnMyItemListLBUp()
{
}

void FrTikiMagicBoxDlg::OnMtrListInit(int param)
{
	BindListBox(m_pMtrList, param);
}

void FrTikiMagicBoxDlg::OnMtrListOwnerDraw(int param)
{
}

void FrTikiMagicBoxDlg::OnMtrListDBClick()
{
}

void FrTikiMagicBoxDlg::OnMtrListLBDown()
{
}

void FrTikiMagicBoxDlg::OnMtrListLBUp()
{
}

void FrTikiMagicBoxDlg::OnCategoryListInit(int param)
{
	BindListBox(m_pCategoryList, param);
}

void FrTikiMagicBoxDlg::OnCategoryListOwnerDraw(int param)
{
}

void FrTikiMagicBoxDlg::OnCategoryListLBDown()
{
}

void FrTikiMagicBoxDlg::OnCharacterListInit(int param)
{
	BindListBox(m_pCharacterList, param);
}

void FrTikiMagicBoxDlg::OnCharacterListOwnerDraw(int param)
{
}

void FrTikiMagicBoxDlg::OnCharacterListLBDown()
{
}

void FrTikiMagicBoxDlg::OnHelpBtnInit(int param)
{
	BindButton(m_pHelpBtn, param);
}

void FrTikiMagicBoxDlg::OnHelpBtnLBUp()
{
}

void FrTikiMagicBoxDlg::OnCloseBtnInit(int param)
{
	BindButton(m_pCloseBtn, param);
}

void FrTikiMagicBoxDlg::OnCloseBtnLBUp()
{
}

void FrTikiMagicBoxDlg::OnCharTabInit(int param)
{
	BindButton(m_pCharTab, param);
}

void FrTikiMagicBoxDlg::OnCharTabLBUp()
{
}

void FrTikiMagicBoxDlg::OnItemTabInit(int param)
{
	BindButton(m_pItemTab, param);
}

void FrTikiMagicBoxDlg::OnItemTabLBUp()
{
}

void FrTikiMagicBoxDlg::OnInsertBtnInit(int param)
{
	BindButton(m_pInsertBtn, param);
}

void FrTikiMagicBoxDlg::OnInsertBtnLBUp()
{
}

void FrTikiMagicBoxDlg::OnMixBtnInit(int param)
{
	BindButton(m_pMixBtn, param);
}

void FrTikiMagicBoxDlg::OnMixBtnLBUp()
{
}

void FrTikiMagicBoxDlg::OnCancelInit(int param)
{
	BindButton(m_pCancel, param);
}

void FrTikiMagicBoxDlg::OnCancelLBUp()
{
}

FrTikiMagicBoxDlg::FrTikiMagicBoxDlg()
{
	m_mtrIndex = 0;
}

FrTikiMagicBoxDlg::~FrTikiMagicBoxDlg()
{
}

void FrTikiMagicBoxDlg::BindButton(FrButton*& pButton, int param)
{
	pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
	{
		pButton->SetPushDelay(0.1f);
		m_ctrlList.push_back(pButton);
	}
}

void FrTikiMagicBoxDlg::BindListBox(FrListBox*& pListBox, int param)
{
	pListBox = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (pListBox)
	{
		pListBox->ClearItem();
		m_ctrlList.push_back(pListBox);
	}
}

void FrTikiMagicBoxDlg::DrawBitmapOnDlg(float x, float y, const char* name)
{
	if (name == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap = g_pFresh->GetBitmap(name);
	if (pBitmap)
	{
		WPoint pos = GetRect().TopLeft();
		WRect rect(pos.x + x, pos.y + y, pBitmap->Width(), pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xffffffff, 0);
	}
}

void FrTikiMagicBoxDlg::PrintTextOnDlg(float x, float y, const char* text,
	unsigned long color)
{
	if (text == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	WRect rect = GetRect();
	pGDI->SetTextColor(color, 0xffffffff);
	pGDI->PrintText(WPoint(rect.x + x, rect.y + y), 0, text, 0);
	pGDI->SetTextColor(0xff000000, 0xffffffff);
}

int FrTikiMagicBoxDlg::InsertMaterial(const sTikiMagicBoxMtr& mtr)
{
	return 0;
}

int FrTikiMagicBoxDlg::DeleteMaterial(const sTikiMagicBoxMtr& mtr)
{
	return 0;
}
