#include "recycledlg.h"
#include "../../shared/recycletable.inl"
#include "shareddoc.h"
#include "simpleoverlay.h"
#include "exhibition.h"
#include "taskmanager.h"
#include "soundmanager.h"
#include "binstr.h"
#include "mathconsts.h"
#include "setitemdlg.h"
#include "frdesktop.h"

extern Fresh* g_pFresh;

namespace
{
	std::string g_cadieMessage(
		"\134cN\134c\xc4\xab\xb5\xf0\xbf\xa1\xc0\xc7 \xb8\xb6\xb9\xfd\xbb\xf3\xc0\xda\xb7\xce \xbf\xc0\xbd\xc5\xb0\xc9 \xc8\xaf\xbf\xb5\xc7\xd5\xb4\xcf\xb4\xd9.\012\xbe\xc6\xb7\xa1\xbf\xa1\xb4\xc2 \xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb8\xb8\xb5\xe9 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb7\xb9\xbd\xc3\xc7\xc7\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\012\xb7\xb9\xbd\xc3\xc7\xc7\xb8\xa6 \xb5\xfb\xb6\xf3\xbc\xad \xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb8\xb8\xb5\xe9\xbe\xee \xba\xb8\xbc\xbc\xbf\xe4.\134cN\134c");
	std::string g_cadieEventMessage(
		"\134cN\134c\xc4\xab\xb5\xf0\xbf\xa1\xc0\xc7 \xb8\xb6\xb9\xfd\xbb\xf3\xc0\xda\xb7\xce \xbf\xc0\xbd\xc5\xb0\xc9 \xc8\xaf\xbf\xb5\xc7\xd5\xb4\xcf\xb4\xd9.\012\xb9\xdf\xb7\xbb\xc5\xb8\xc0\xce\xb5\xa5\xc0\xcc \xc0\xcc\xba\xa5\xc6\xae\xb0\xa1 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.\012\xc0\xcc\xba\xa5\xc6\xae \xb1\xe2\xb0\xa3 \xb5\xbf\xbe\xc8\xc0\xba \134c0xffff0000\134c\xb3\xb2\xbc\xba \xc0\xaf\xc0\xfa\134c0xff000000\134c\xb8\xb8 \xc1\xb6\xc7\xd5\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.\134cN\134c");
}

IMPLEMENT_OBJECT(FrRecycleDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrRecycleDlg, FrForm)
ON_FRESH_VI("recycledlg", FRCMD_INIT, FrRecycleDlg::OnRecycleDlgInit)
ON_FRESH_VI("about", FRCMD_INIT, FrRecycleDlg::OnAboutBnInit)
ON_FRESH_VV("about", FRCMD_LBUTTONUP, FrRecycleDlg::OnAboutBnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrRecycleDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrRecycleDlg::OnCloseBnUp)
ON_FRESH_VI("level_low", FRCMD_INIT, FrRecycleDlg::OnLevelLowBnInit)
ON_FRESH_VV("level_low", FRCMD_LBUTTONUP, FrRecycleDlg::OnLevelLowBnUp)
ON_FRESH_VI("level_mid", FRCMD_INIT, FrRecycleDlg::OnLevelMidBnInit)
ON_FRESH_VV("level_mid", FRCMD_LBUTTONUP, FrRecycleDlg::OnLevelMidBnUp)
ON_FRESH_VI("level_high", FRCMD_INIT, FrRecycleDlg::OnLevelHighBnInit)
ON_FRESH_VV("level_high", FRCMD_LBUTTONUP, FrRecycleDlg::OnLevelHighBnUp)
ON_FRESH_VI("level_special", FRCMD_INIT, FrRecycleDlg::OnLevelSpecialBnInit)
ON_FRESH_VV("level_special", FRCMD_LBUTTONUP, FrRecycleDlg::OnLevelSpecialBnUp)
ON_FRESH_VI("level_event", FRCMD_INIT, FrRecycleDlg::OnLevelEventBnInit)
ON_FRESH_VV("level_event", FRCMD_LBUTTONUP, FrRecycleDlg::OnLevelEventBnUp)
ON_FRESH_VI("recycle_char_list", FRCMD_INIT,
	FrRecycleDlg::OnCharacterSelectListInit)
ON_FRESH_VV("recycle_char_list", FRCMD_LBUTTONUP,
	FrRecycleDlg::OnCharacterSelectListLBnUp)
ON_FRESH_VI("recycle_char_list", FRCMD_OWNERDRAW,
	FrRecycleDlg::OnCharacterSelectListOwnerDraw)
ON_FRESH_VI("recycle_item_list", FRCMD_INIT,
	FrRecycleDlg::OnRecycleItemListInit)
ON_FRESH_VV("recycle_item_list", FRCMD_LBUTTONUP,
	FrRecycleDlg::OnRecycleItemListLBnUp)
ON_FRESH_VV("recycle_item_list", FRCMD_RBUTTONUP,
	FrRecycleDlg::OnRecycleItemListRBnUp)
ON_FRESH_VI("recycle_item_list", FRCMD_OWNERDRAW,
	FrRecycleDlg::OnRecycleItemListOwnerDraw)
ON_FRESH_VI("recycle_number", FRCMD_INIT, FrRecycleDlg::OnRecycleNumberInit)
ON_FRESH_BI("recycle_number", FRCMD_ENTERKEY,
	FrRecycleDlg::OnRecycleNumberEnterKey)
ON_FRESH_VI("mix", FRCMD_INIT, FrRecycleDlg::OnMixBnInit)
ON_FRESH_VV("mix", FRCMD_LBUTTONUP, FrRecycleDlg::OnMixBnUp)
ON_FRESH_VI("arrow_minimum", FRCMD_INIT, FrRecycleDlg::OnArrowMinimumBnInit)
ON_FRESH_VV("arrow_minimum", FRCMD_LBUTTONDOWN,
	FrRecycleDlg::OnArrowMinimumBnDown)
ON_FRESH_VI("arrow_left", FRCMD_INIT, FrRecycleDlg::OnArrowLeftBnInit)
ON_FRESH_VV("arrow_left", FRCMD_LBUTTONDOWN, FrRecycleDlg::OnArrowLeftBnDown)
ON_FRESH_VV("arrow_left", FRCMD_LBUTTONUP, FrRecycleDlg::OnArrowLeftBnUp)
ON_FRESH_VI("arrow_right", FRCMD_INIT, FrRecycleDlg::OnArrowRightBnInit)
ON_FRESH_VV("arrow_right", FRCMD_LBUTTONDOWN, FrRecycleDlg::OnArrowRightBnDown)
ON_FRESH_VV("arrow_right", FRCMD_LBUTTONUP, FrRecycleDlg::OnArrowRightBnUp)
ON_FRESH_VI("arrow_maximum", FRCMD_INIT, FrRecycleDlg::OnArrowMaximumBnInit)
ON_FRESH_VV("arrow_maximum", FRCMD_LBUTTONDOWN,
	FrRecycleDlg::OnArrowMaximumBnDown)
ON_FRESH_VI("area_picture", FRCMD_INIT, FrRecycleDlg::OnAreaPictureInit)
ON_FRESH_VV("area_picture", FRCMD_OWNERDRAW,
	FrRecycleDlg::OnAreaPictureOwnerDraw)
ON_FRESH_VI("recycle_item_name", FRCMD_INIT,
	FrRecycleDlg::OnRecycleItemNameInit)
ON_FRESH_VI("cadie_message", FRCMD_INIT, FrRecycleDlg::OnCadieMessageInit)
ON_FRESH_VI("select_part", FRCMD_INIT, FrRecycleDlg::OnSelectPartInit)
ON_FRESH_VV("select_part", FRCMD_LBUTTONDOWN, FrRecycleDlg::OnSelectPartLBDown)
ON_FRESH_VI("card_slot", FRCMD_INIT, FrRecycleDlg::OnCardSlotInit)
ON_FRESH_VI("card_slot", FRCMD_OWNERDRAW, FrRecycleDlg::OnCardSlotOwnerDraw)
ON_FRESH_VI("card_slotresult", FRCMD_INIT, FrRecycleDlg::OnCardSlotResultInit)
END_FRESH_MSGMAP()

FrRecycleDlg::FrRecycleDlg()
{
	m_pCloseBn = NULL;
	m_pLevelLowBn = NULL;
	m_pLevelMidBn = NULL;
	m_pLevelHighBn = NULL;
	m_pLevelSpecialBn = NULL;
	m_pLevelEventBn = NULL;
	m_pCharacterSelectList = NULL;
	m_pRecycleItemList = NULL;
	m_pRecycleNumber = NULL;
	m_pMixBn = NULL;
	m_pArrowMinimumBn = NULL;
	m_pArrowLeftBn = NULL;
	m_pArrowRightBn = NULL;
	m_pArrowMaximumBn = NULL;
	m_pRecycleItemName = NULL;
	m_pCadieMessage = NULL;
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&font);
	m_pItemBn[0] = g_pFresh->GetManager()->GetBitmap("RECYCLEDLG",
		"recycle_item_bn_normal");
	m_pItemBn[1] = g_pFresh->GetManager()->GetBitmap("RECYCLEDLG",
		"recycle_item_bn_click");
	m_pSimpleUI = NULL;
	m_listLevel = -1;
	memset(m_characterTypeId, 0, sizeof(m_characterTypeId));
	m_selRecycleIndex = -1;
	m_selCharacter = 0;
	memset(&m_totalItem, 0, sizeof(m_totalItem));
	memset(m_needItem, 0, sizeof(m_needItem));
	memset(m_haveItem, 0, sizeof(m_haveItem));
	m_recycleNumber = 0;
	m_maxRecycleNumber = 0;
	memset(&m_resultItem, 0, sizeof(m_resultItem));
	m_pExhibition = NULL;
	m_bArrowLeftDown = false;
	m_bArrowRightDown = false;
	m_arrowRepeat = 0;
	m_bMixing = false;
	m_arrowState[0] = 0;
	m_arrowState[1] = 0;
	m_arrowBlink[0] = 0;
	m_arrowBlink[1] = 0;
	m_sendDelay = 0;
	m_resultTime = 0;
	m_messageTime = 0;
	m_selTypeId = 0;
	m_selPartId = 0;
	m_bPartSelected = false;
	m_bReserved = false;
	m_reserved[0] = 0;
	m_reserved[1] = 0;
	m_pSelectPartBn = NULL;
	m_pSelectPartDlg = NULL;
	InitDataCharacterSelect();
	InitIndexRecycleItem();
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(2);
		packet.Send(TO_GAME);
	}
}

FrRecycleDlg::~FrRecycleDlg()
{
	if (IsLocalContent(S4_MATCHING_SYSTEM) && WNetworkSystem::IsInstantiated())
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
	if (m_pSimpleUI)
	{
		delete m_pSimpleUI;
		m_pSimpleUI = NULL;
	}
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrRecycleDlg::OnRecycleDlgInit(int param)
{
}

void FrRecycleDlg::OnAboutBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnAboutBnUp()
{
	FrRecycleAboutDlg* form = CreateForm<FrRecycleAboutDlg>(
		g_pFresh->GetManager(), this, "recycle_about");
	if (form)
		form->Open(NULL, FrESCAPE | FrENTER);
}

void FrRecycleDlg::OnCloseBnInit(int param)
{
	m_pCloseBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnCloseBnUp()
{
	OnFreshOkay();
}

void FrRecycleDlg::OnLevelLowBnInit(int param)
{
	m_pLevelLowBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::PRESSED);
}

void FrRecycleDlg::OnLevelLowBnUp()
{
	m_level = 0;
	SetListRecycleItem(0, 0);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::PRESSED);
	if (m_pLevelMidBn)
		m_pLevelMidBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelHighBn)
		m_pLevelHighBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelEventBn)
		m_pLevelEventBn->SetStatus(FrButton::NORMAL);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(false);
}

void FrRecycleDlg::OnLevelMidBnInit(int param)
{
	m_pLevelMidBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnLevelMidBnUp()
{
	m_level = 1;
	SetListRecycleItem(1, 0);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelMidBn)
		m_pLevelMidBn->SetStatus(FrButton::PRESSED);
	if (m_pLevelHighBn)
		m_pLevelHighBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelEventBn)
		m_pLevelEventBn->SetStatus(FrButton::NORMAL);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(false);
}

void FrRecycleDlg::OnLevelHighBnInit(int param)
{
	m_pLevelHighBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnLevelHighBnUp()
{
	m_level = 2;
	SetListRecycleItem(2, 0);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelMidBn)
		m_pLevelMidBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelHighBn)
		m_pLevelHighBn->SetStatus(FrButton::PRESSED);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelEventBn)
		m_pLevelEventBn->SetStatus(FrButton::NORMAL);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(false);
}

void FrRecycleDlg::OnLevelSpecialBnInit(int param)
{
	m_pLevelSpecialBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pLevelSpecialBn)
	{
		if (IsLocalContent(S4_OPEN_RECYCLE_SPECIAL_TAB))
			m_pLevelSpecialBn->SetVisible(true);
		else
			m_pLevelSpecialBn->SetVisible(false);
	}
}

void FrRecycleDlg::OnLevelSpecialBnUp()
{
	m_level = 3;
	SetListRecycleItem(3, m_selCharacter);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelMidBn)
		m_pLevelMidBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelHighBn)
		m_pLevelHighBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->SetStatus(FrButton::PRESSED);
	if (m_pLevelEventBn)
		m_pLevelEventBn->SetStatus(FrButton::NORMAL);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(true);
}

void FrRecycleDlg::OnLevelEventBnInit(int param)
{
	m_pLevelEventBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (g_NumEventRecycleItem > 0)
		m_pLevelEventBn->SetVisible(true);
	else
		m_pLevelEventBn->SetVisible(false);
}

void FrRecycleDlg::OnLevelEventBnUp()
{
	m_level = 4;
	SetListRecycleItem(4, m_selCharacter);
	if (m_pLevelLowBn)
		m_pLevelLowBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelMidBn)
		m_pLevelMidBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelHighBn)
		m_pLevelHighBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->SetStatus(FrButton::NORMAL);
	if (m_pLevelEventBn)
		m_pLevelEventBn->SetStatus(FrButton::PRESSED);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(false);
}

void FrRecycleDlg::OnCharacterSelectListInit(int param)
{
	m_pCharacterSelectList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->SetVisible(false);
}

void FrRecycleDlg::OnCharacterSelectListLBnUp()
{
	FrListItem* item = m_pCharacterSelectList->GetSelected();
	if (item)
	{
		m_selCharacter = item->idx;
		SetListRecycleItem(m_level, m_selCharacter);
	}
}

void FrRecycleDlg::OnCharacterSelectListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	const unsigned long& typeId = *(unsigned long*)row->pData;
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
	if (!item)
		return;
	const Bitmap* bitmap = g_pFresh->GetBitmap(MakeStr("s_%s", item->Icon));
	if (!bitmap)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	unsigned long color = 0xff808080;
	if (row->selected)
		color = 0xffffffff;
	else if (row->underCursor)
		color = 0xffc0c0c0;
	graphic->DrawTexture(bitmap,
		WRect(row->pos.x + 4.0f, row->pos.y + 4.0f,
			(float)bitmap->Width() * 0.8f, (float)bitmap->Height() * 0.8f),
		color, 0);
}

void FrRecycleDlg::ReInitializeHaveItem()
{
	m_haveItemMap.clear();
}

void FrRecycleDlg::AddHaveItemMap(const sRecycleItem* pItem)
{
	if (pItem->byLevel <= Doc()->m_myInfo.stat.Level)
	{
		if (IsHaveItem(pItem))
			m_haveItemMap[pItem] = 2;
		else
			m_haveItemMap[pItem] = 0;
	}
	else
		m_haveItemMap[pItem] = 1;
}

void FrRecycleDlg::OnRecycleItemListInit(int param)
{
	m_pRecycleItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pRecycleItemList)
		m_pRecycleItemList->UseRightButton(true);
	m_clockIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_clockIcon.rect = WRect(50.0f, 35.0f, (float)m_clockIcon.pBitmap->Width(),
		(float)m_clockIcon.pBitmap->Height());
	m_deleteIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_deleteIcon.rect =
		WRect(50.0f, 35.0f, (float)m_deleteIcon.pBitmap->Width(),
			(float)m_deleteIcon.pBitmap->Height());
}

void FrRecycleDlg::OnRecycleItemListLBnUp()
{
	ClickRecycleItem();
}

void FrRecycleDlg::OnRecycleItemListRBnUp()
{
	FrListItem* item = m_pRecycleItemList->GetSelected();
	if (item)
		OpenInformation(((sRecycleItem*)item->pData)->dwTid);
	ClickRecycleItem();
}

void FrRecycleDlg::OnRecycleItemListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	sRecycleItem* item = (sRecycleItem*)row->pData;
	WPoint pos(row->pos.x + 3.0f, row->pos.y + 3.0f);
	int button = 0;
	int state = 0;
	unsigned long color = 0xffffffff;
	const sRecycleItem* key = item;
	std::map<const sRecycleItem*, int>::const_iterator it =
		m_haveItemMap.find(key);
	if (it == m_haveItemMap.end())
		return;
	switch (it->second)
	{
	case 0:
		if (row->selected)
			button = 1;
		state = 0;
		break;
	case 1:
		color = 0xff909090;
		state = 1;
		break;
	case 2:
		color = 0xff909090;
		state = 2;
		break;
	}
	DrawItem(pos, item->dwTid, item->iNum, 0, m_pItemBn[button], color);
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (item->dwRandSeq > 0)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("recycle_random_mark");
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(pos.x + 2.0f, pos.y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	if ((item->dwTid >> 26) == 2 &&
		item->iIndex >= g_NumLowRecycleItem + g_NumMidRecycleItem +
				g_NumHighRecycleItem + g_NumSpecialRecycleItem)
	{
		IFF_ITEM_COMMON* character = ItemManager()->FindCommonItem(
			((item->dwTid >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* bitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->Icon));
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + 3.0f, row->pos.y + 3.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xeeffffff, 0);
		}
	}
	const Bitmap* bitmap = NULL;
	switch (state)
	{
	case 1:
		bitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", item->byLevel + 1));
		break;
	case 2:
		bitmap = g_pFresh->GetBitmap("recycle_text_owned");
		break;
	}
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + 38.0f - (int)((float)bitmap->Width() * 0.5f),
				row->pos.y + 35.0f - (int)((float)bitmap->Height() * 0.5f),
				(float)bitmap->Width(), (float)bitmap->Height()),
			color, 0);
}

void FrRecycleDlg::OnRecycleNumberInit(int param)
{
	m_pRecycleNumber = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	ControlRecycleNumber(-1);
}

bool FrRecycleDlg::OnRecycleNumberEnterKey(int param)
{
	return false;
}

void FrRecycleDlg::OnMixBnInit(int param)
{
	m_pMixBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnMixBnUp()
{
	if (m_recycleNumber <= 0)
		return;
	if (!Doc()->ConfirmInsertItem(m_selTypeId, m_recycleNumber))
	{
		FrForm* notice =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		WRect rect = notice->GetRect();
		notice->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
		notice->SetTimeLimit(12.0f);
		notice->SetMessage(
			"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xb6\xc7\xd5 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		notice->Open(NULL, 0);
		return;
	}
	m_bMixing = true;
	EnableTotalControls(false);
	m_sendDelay = 1.5f;
	m_resultTime = 0.0f;
	memset(&m_resultItem, 0, sizeof(m_resultItem));
	if (m_pExhibition)
	{
		char motion[32] = { 0 };
		switch (rand() % 2)
		{
		case 0:
			strcpy(motion, "\xc8\xa6\xc0\xce");
			break;
		case 1:
			strcpy(motion, "\xc8\xa6\xc0\xce\xbf\xf8");
			break;
		}
		m_pExhibition->SetMotion(motion, false, 0.0f);
	}
}

void FrRecycleDlg::OnArrowMinimumBnInit(int param)
{
	m_pArrowMinimumBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnArrowMinimumBnDown()
{
	ControlRecycleNumber(0);
}

void FrRecycleDlg::OnArrowLeftBnInit(int param)
{
	m_pArrowLeftBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnArrowLeftBnDown()
{
	if ((m_haveItem[0].dwTid >> 26) == 2 && m_haveItem[0].iNum > 1)
	{
		if (!m_bPartSelected)
			return;
		if (!m_pSelectPartBn->IsVisible())
			m_pSelectPartBn->SetVisible(true);
	}
	m_bArrowLeftDown = true;
	m_arrowRepeat = 0;
	ControlRecycleNumber(1);
}

void FrRecycleDlg::OnArrowLeftBnUp()
{
	m_bArrowLeftDown = false;
	if (m_pCardSlot)
		m_pCardSlot->SetVisible(true);
}

void FrRecycleDlg::OnArrowRightBnInit(int param)
{
	m_pArrowRightBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnArrowRightBnDown()
{
	if ((m_haveItem[0].dwTid >> 26) == 2 && m_haveItem[0].iNum > 1)
	{
		if (!m_bPartSelected)
			return;
		if (m_pSelectPartBn->IsVisible())
			m_pSelectPartBn->SetVisible(false);
	}
	m_bArrowRightDown = true;
	m_arrowRepeat = 0;
	ControlRecycleNumber(2);
}

void FrRecycleDlg::OnArrowRightBnUp()
{
	m_bArrowRightDown = false;
	if (m_pCardSlot)
		m_pCardSlot->SetVisible(false);
}

void FrRecycleDlg::OnArrowMaximumBnInit(int param)
{
	m_pArrowMaximumBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleDlg::OnArrowMaximumBnDown()
{
	ControlRecycleNumber(3);
}

void FrRecycleDlg::OnAreaPictureInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRecycleDlg::OnAreaPictureOwnerDraw()
{
	DrawRecycleItem();
	DrawArrow();
	if (m_pExhibition)
	{
		static float height = 455.25f;
		WRect rect(m_rect.x, m_rect.y, 607.0f, height);
		m_pExhibition->DisplayClip(rect, 55.0f, 0.0f, 0.0f);
	}
}

void FrRecycleDlg::OnRecycleItemNameInit(int param)
{
	m_pRecycleItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrRecycleDlg::OnCadieMessageInit(int param)
{
	m_pCadieMessage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pCadieMessage)
		SetCadieMessage(g_cadieMessage.c_str());
}

bool FrRecycleDlg::OnInit()
{
	EnableDrag(false);
	InitPositionItemData();
	SetListRecycleItem(0, 0);
	InitListCharacterSelect();
	EnableMixControls(false);
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
	WRect rect = GetRect();
	float distance = g_view->GetWidth() * 0.03375f;
	m_pExhibition = new CExhibition(rect.x + 438.0f, rect.y + 173.0f,
		(g_view->GetWidth() * 0.0015625f -
			(1.f / g_view->GetRatio() - 1.3333334f) * 0.8f) /
			g_view->GetHeight() * 99.000008f,
		true);
	m_pExhibition->SetDistanceRange(distance - 5.f, distance + 5.f);
	m_pExhibition->SetDistance(distance, true, false);
	m_pExhibition->SetArea(164.0f, 346.0f);
	m_pExhibition->SetModel("shop_cadie", (eExhibitType)1);
	m_pSimpleUI = new CSimpleUI("keys.png");
	m_pSimpleUI->Add("RIGHT_S", WRect(40.f, 0.f, 20.f, 20.f), false);
	m_pSimpleUI->Add("RIGHT", WRect(0.f, 40.f, 20.f, 20.f), false);
	m_pSimpleUI->Add("INVALID", WRect(40.f, 40.f, 20.f, 20.f), false);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0xdf, 0, 0, 0, 0, 0));
	return true;
}

void FrRecycleDlg::OnProc(const float delta)
{
	if (m_bArrowLeftDown)
	{
		m_arrowRepeat += delta;
		if (m_arrowRepeat > .2f)
		{
			m_arrowRepeat = 0;
			ControlRecycleNumber(1);
		}
	}
	else if (m_bArrowRightDown)
	{
		m_arrowRepeat += delta;
		if (m_arrowRepeat > .2f)
		{
			m_arrowRepeat = 0;
			ControlRecycleNumber(2);
		}
	}
	if (m_pExhibition)
		m_pExhibition->ProcessBongdariShop(delta);
	if (m_maxRecycleNumber <= 0 || m_selRecycleIndex < 0)
	{
		m_arrowState[0] = -1;
		m_arrowState[1] = -1;
	}
	else if (!m_bMixing)
	{
		m_arrowState[1] = 0;
		m_arrowBlink[0] += delta;
		if (m_arrowBlink[0] > .5f)
		{
			m_arrowBlink[0] = 0;
			m_arrowState[0] = !m_arrowState[0];
		}
	}
	else
	{
		m_arrowState[0] = 1;
		m_arrowBlink[1] += delta;
		if (m_arrowBlink[1] > .5f)
		{
			m_arrowBlink[1] = 0;
			m_arrowState[1] = !m_arrowState[1];
		}
	}
	if (m_sendDelay > 0.f)
	{
		m_sendDelay -= delta;
		if (m_sendDelay <= 0.f)
			SendDataRecycleItem(m_selRecycleIndex, m_recycleNumber);
	}
	if (m_resultTime > 0.f)
	{
		m_resultTime -= delta;
		if (m_resultTime <= 0.f)
			memset(&m_resultItem, 0, sizeof(m_resultItem));
	}
	if (m_messageTime > 0.f)
	{
		m_messageTime -= delta;
		if (m_messageTime <= 0.f)
			SetCadieMessage(g_cadieMessage.c_str());
	}
}

void FrRecycleDlg::OnCancel()
{
}

void FrRecycleDlg::InitPositionItemData()
{
	m_totalPos = WPoint(GetRect().x + 220.0f, GetRect().y + 200.0f);
	for (int i = 0; i < 4; ++i)
		m_needPos[i] =
			WPoint(GetRect().x + (207.0f + i * 50.0f), GetRect().y + 280.0f);
	for (int i = 0; i < 4; ++i)
		m_havePos[i] =
			WPoint(GetRect().x + (26.0f + i * 63.0f), GetRect().y + 375.0f);
	m_resultPos = WPoint(GetRect().x + 427.0f, GetRect().y + 376.0f);
}

void FrRecycleDlg::SetListRecycleItem(int level, int character)
{
	ReInitializeHaveItem();
	memset(&m_totalItem, 0, sizeof(m_totalItem));
	memset(m_needItem, 0, sizeof(m_needItem));
	memset(m_haveItem, 0, sizeof(m_haveItem));
	if (m_pSelectPartBn)
		m_pSelectPartBn->SetVisible(false);
	EnableMixControls(false);
	m_maxRecycleNumber = 0;
	m_pRecycleItemList->ClearItem();
	m_selRecycleIndex = -1;
	m_listLevel = level;
	int first = 0;
	int last = 0;
	switch (level)
	{
	case 0:
		first = 0;
		last = g_NumLowRecycleItem;
		break;
	case 1:
		first = g_NumLowRecycleItem;
		last = first + g_NumMidRecycleItem;
		break;
	case 2:
		first = g_NumLowRecycleItem + g_NumMidRecycleItem;
		last = first + g_NumHighRecycleItem;
		break;
	case 3:
		first =
			g_NumLowRecycleItem + g_NumMidRecycleItem + g_NumHighRecycleItem;
		last = first + g_NumSpecialRecycleItem;
		break;
	case 4:
		first = g_NumLowRecycleItem + g_NumMidRecycleItem +
			g_NumHighRecycleItem + g_NumSpecialRecycleItem;
		last = first + g_NumEventRecycleItem;
		break;
	}
	if (level == 3 || level == 4)
	{
		for (int i = first; i < last; ++i)
		{
			if ((g_ArrayRecycleItem[i].iChar == character ||
					g_ArrayRecycleItem[i].iChar == -1 ||
					g_ArrayRecycleItem[i].iChar == -2) &&
				g_ArrayRecycleItem[i].dwTid)
			{
				m_pRecycleItemList->AddItem(&g_ArrayRecycleItem[i]);
				AddHaveItemMap(&g_ArrayRecycleItem[i]);
			}
		}
	}
	else
	{
		for (int i = first; i < last; ++i)
		{
			m_pRecycleItemList->AddItem(&g_ArrayRecycleItem[i]);
			AddHaveItemMap(&g_ArrayRecycleItem[i]);
		}
	}
}

void FrRecycleDlg::ClickRecycleItem()
{
	FrListItem* row = m_pRecycleItemList->GetSelected();
	if (row)
	{
		sRecycleItem* recipe = (sRecycleItem*)row->pData;
		if (recipe->byLevel <= Doc()->m_myInfo.stat.Level &&
			!IsHaveItem(recipe->dwTid))
		{
			m_selRecycleIndex = recipe->iIndex;
			SetDataTotalItem(m_selRecycleIndex);
			SetMaxEnableRecycle(m_selRecycleIndex);
			ControlRecycleNumber(0);
			if (m_maxRecycleNumber > 0)
				EnableMixControls(true);
			else
				EnableMixControls(false);
		}
		m_selTypeId = recipe->dwTid;
		m_bPartSelected = false;
		m_selPartId = 0;
		if (m_pCardSlot)
			m_pCardSlot->SetVisible(true);
		if ((m_haveItem[0].dwTid >> 26) == 2)
		{
			unsigned int count = 0;
			for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
				it != Doc()->m_partsList.end(); ++it)
			{
				if ((*it).tid != m_haveItem[0].dwTid)
					continue;
				bool equipped = false;
				for (std::map<unsigned int, sCharacterInfo>::iterator
						 character = Doc()->m_charMap.begin();
					character != Doc()->m_charMap.end(); ++character)
				{
					for (int slot = 0; slot < 24; ++slot)
						if ((*it).guid == (*character).second.ItemIdList[slot])
						{
							equipped = true;
							break;
						}
					if (equipped)
						break;
				}
				if (!equipped)
					++count;
			}
			if (m_pSelectPartBn && count > 1)
				m_pSelectPartBn->SetVisible(true);
			else
			{
				m_pSelectPartBn->SetVisible(false);
				m_bPartSelected = false;
			}
		}
		else
		{
			m_pSelectPartBn->SetVisible(false);
			m_bPartSelected = false;
		}
	}
	else
	{
		m_selRecycleIndex = -1;
		m_selTypeId = 0;
		SetDataTotalItem(m_selRecycleIndex);
		SetMaxEnableRecycle(m_selRecycleIndex);
		ControlRecycleNumber(-1);
		EnableMixControls(false);
		if (m_pSelectPartBn)
			m_pSelectPartBn->SetVisible(false);
		m_bPartSelected = false;
		m_selPartId = 0;
	}
}

void FrRecycleDlg::InitDataCharacterSelect()
{
	m_characterTypeId[0] = 0x04000000;
	m_characterTypeId[1] = 0x04000001;
	m_characterTypeId[2] = 0x04000002;
	m_characterTypeId[3] = 0x04000003;
	m_characterTypeId[4] = 0x04000004;
	m_characterTypeId[5] = 0x04000005;
	m_characterTypeId[6] = 0x04000006;
	m_characterTypeId[7] = 0x04000007;
	m_characterTypeId[8] = 0x04000008;
	m_characterTypeId[9] = 0x04000009;
}

void FrRecycleDlg::InitListCharacterSelect()
{
	CSharedDoc* doc = Doc();
	unsigned int characterId = doc->m_myInfo.userEquip.guidChar;
	std::map<unsigned int, sCharacterInfo>::iterator it =
		doc->m_charMap.find(characterId);
	if (it != doc->m_charMap.end())
	{
		const unsigned long& typeId = (*it).second.tid;
		IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
		if (item)
			m_selCharacter = item->TypeId & 0x1ff;
	}
	m_pCharacterSelectList->ClearItem();
	for (int i = 0; i < 10; ++i)
	{
		m_pCharacterSelectList->AddItem(&m_characterTypeId[i]);
		if (i == m_selCharacter)
		{
			FrListItem* row = m_pCharacterSelectList->GetItem(i);
			m_pCharacterSelectList->SelectItem(row, true);
		}
	}
}

void FrRecycleDlg::DrawItem(WPoint pos, unsigned long typeId, int count,
	unsigned char level, const Bitmap* background, unsigned long color)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic || !typeId)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	if (!common)
		return;
	bool scale = false;
	float x = 0.0f, y = 0.0f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if ((typeId >> 26) == 2)
	{
		scale = true;
		x = -2.0f;
		y = -15.0f;
	}
	else if ((typeId >> 26) == 4)
	{
		scale = true;
		x = 1.0f;
		y = 2.0f;
	}
	else if ((typeId >> 26) == 9)
	{
		x = -10.0f;
		y = -21.0f;
	}
	else if ((typeId >> 26) == 28)
	{
		x = -12.0f;
		y = -20.0f;
	}
	else if (ItemManager()->GetCouponKind(typeId))
	{
		x = -5.0f;
		y = 3.0f;
	}
	else if (ItemManager()->GetCouponKind(typeId) == 3)
	{
		x = -7.0f;
		y = 3.0f;
	}
	else if ((typeId >> 26) == 31)
	{
		x = -3.0f;
		y = -9.0f;
	}
	else if ((typeId >> 26) == 16)
	{
		level = 1;
		x = -12.0f;
		y = -20.0f;
	}
	else if ((typeId >> 26) == 18)
	{
		x = -4.5f;
		y = -9.0f;
	}
	if (bitmap)
	{
		if (background)
			graphic->DrawTexture(background,
				WRect(pos.x, pos.y, (float)background->Width(),
					(float)background->Height()),
				color, 0);
		float width = (float)bitmap->Width();
		float height = (float)bitmap->Height();
		if (scale)
		{
			width *= 0.8f;
			height *= 0.8f;
		}
		if ((typeId >> 26) == 7)
		{
			width -= 6.0f;
			height -= 6.0f;
			WRect source(4.0f, 2.0f, width, height);
			WRect target(pos.x + 14.0f, pos.y + 11.0f, width, height);
			graphic->DrawTexture(bitmap, source, target, color, 0);
		}
		else
			graphic->DrawTexture(bitmap,
				WRect(pos.x + x + 14.0f, pos.y + y + 11.0f, width, height),
				color, 0);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(4);
		if (level)
		{
			pos.x -= 4.0f;
			pos.y += 8.0f;
			graphic->DrawTexture(m_clockIcon.pBitmap, m_clockIcon.rect + pos,
				0xffffffff, 0);
		}
		else if (count > 1)
		{
			unsigned int i = 0;
			const char* text = MakeStr("%d", count);
			float left = pos.x + 54.0f;
			for (i = 0; i < strlen(text); ++i)
				left -= m_pFont->GetCharWidth(g_view, text[i] - '0');
			m_pFont->Print(g_view, left, pos.y + 42.0f, text, 0, color, NULL);
		}
	}
}

void FrRecycleDlg::DrawRecycleItem()
{
	if (m_totalItem.dwRandSeq)
	{
		FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
		if (graphic)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("recycle_random_out");
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(m_totalPos.x + 2.0f, m_totalPos.y,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
	else
		DrawItem(m_totalPos, m_totalItem.dwTid, m_totalItem.iNum, 0, NULL,
			0xffffffff);

	for (int slot = 0; slot < 4; ++slot)
		DrawItem(m_needPos[slot], m_needItem[slot].dwTid, m_needItem[slot].iNum,
			0, NULL, 0xffffffff);
	for (int slot = 0; slot < 4; ++slot)
	{
		WPoint& position = m_havePos[slot];
		const sRecycleItem& available = m_haveItem[slot];
		int needed = m_needItem[slot].iNum;
		int remaining = available.iNum - m_recycleNumber * needed;
		unsigned long color = 0xffffffff;
		if (remaining < needed || remaining <= 0)
			color = 0xff909090;
		if (remaining >= 1)
		{
			if ((available.dwTid >> 26) == 2)
			{
				unsigned int count = 0;
				for (std::list<sItemInfo>::iterator it =
						 Doc()->m_partsList.begin();
					it != Doc()->m_partsList.end(); ++it)
				{
					if ((*it).tid != m_haveItem[0].dwTid)
						continue;
					bool equipped = false;
					for (std::map<unsigned int, sCharacterInfo>::iterator
							 character = Doc()->m_charMap.begin();
						character != Doc()->m_charMap.end(); ++character)
					{
						for (int i = 0; i < 24; ++i)
							if ((*it).guid == (*character).second.ItemIdList[i])
							{
								equipped = true;
								break;
							}
						if (equipped)
							break;
					}
					if (!equipped)
						++count;
				}
				IFF_ITEM_COMMON* common =
					ItemManager()->FindCommonItem(m_haveItem[0].dwTid);
				if (m_pSelectPartBn && count > 1 && !m_bPartSelected)
				{
					if (common)
					{
						m_pSelectPartBn->SetButtonImg("select_on",
							FrButton::NORMAL);
						m_pSelectPartBn->SetButtonImg("select_on",
							FrButton::OVER);
						m_pSelectPartBn->SetButtonImg("select_on",
							FrButton::PRESSED);
						WRect rect;
						rect.x = m_rect.x + 41.0f;
						rect.y = m_rect.y + 387.0f;
						rect.w = 40.0f;
						rect.h = 41.0f;
						m_pSelectPartBn->SetRect(rect);
					}
				}
				else if (m_bPartSelected == true)
				{
					if (m_pSelectPartBn)
					{
						if (strlen(common->Icon))
						{
							m_pSelectPartBn->SetButtonImg(common->Icon,
								FrButton::NORMAL);
							m_pSelectPartBn->SetButtonImg(common->Icon,
								FrButton::OVER);
							m_pSelectPartBn->SetButtonImg(common->Icon,
								FrButton::PRESSED);
						}
						WRect rect = m_pSelectPartBn->GetRect();
						rect.x = m_rect.x + 39.0f;
						rect.y = m_rect.y + 372.0f;
						rect.w *= 0.8f;
						rect.h *= 0.8f;
						m_pSelectPartBn->SetRect(rect);
					}
				}
				else
					DrawItem(position, available.dwTid, remaining, 0, NULL,
						color);
			}
			else
				DrawItem(position, available.dwTid, remaining, 0, NULL, color);
		}
		else if ((available.dwTid >> 26) == 2)
			m_pSelectPartBn->SetVisible(false);
	}
	DrawItem(m_resultPos, m_resultItem.dwTid, m_resultItem.iNum, 0, NULL,
		0xffffffff);
	if (m_resultItem.dwTid && (m_resultItem.dwTid >> 26) == 2)
	{
		IFF_STRUCT::sPart* definition =
			ItemManager()->FindPart(m_resultItem.dwTid);
		if (definition)
		{
			sItemInfo part;
			part.tid = m_resultItem.dwTid;
			part.guid = m_resultItem.dwItemid;
			part.CharacterSlotNum = definition->CharacterSlot;
			part.CaddieSlotNum = definition->CaddieSlot;
			DrawPartsSlot(g_pFresh->GetManager()->GetGDI(), &part,
				m_pCardSlotResult);
		}
	}
	if (m_totalItem.dwRandSeq && m_resultItem.dwTid &&
		(m_resultItem.dwTid >> 26) == 2)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(
			((m_resultItem.dwTid >> 18) & 0xff) | 0x04000000);
		if (common)
		{
			FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
			const Bitmap* bitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", common->Icon));
			if (bitmap && graphic)
				graphic->DrawTexture(bitmap,
					WRect(m_resultPos.x + 6.0f, m_resultPos.y + 2.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
}

void FrRecycleDlg::DrawArrow()
{
	if (!m_pSimpleUI)
		return;
	std::string images[2];
	for (int i = 0; i < 2; ++i)
	{
		switch (m_arrowState[i])
		{
		case 0:
			images[i] = "RIGHT";
			break;
		case 1:
			images[i] = "RIGHT_S";
			break;
		default:
			images[i] = "INVALID";
			break;
		}
	}
	m_pSimpleUI->Render(images[0].c_str(), GetRect().x + 282.0f,
		GetRect().y + 392.0f, 1.f, 0);
	m_pSimpleUI->Render(images[1].c_str(), GetRect().x + 410.0f,
		GetRect().y + 392.0f, 1.f, 0);
}

void FrRecycleDlg::EnableTotalControls(bool bEnable)
{
	if (m_pCloseBn)
		m_pCloseBn->Enable(bEnable);
	if (m_pLevelLowBn)
		m_pLevelLowBn->Enable(bEnable);
	if (m_pLevelMidBn)
		m_pLevelMidBn->Enable(bEnable);
	if (m_pLevelHighBn)
		m_pLevelHighBn->Enable(bEnable);
	if (m_pLevelSpecialBn)
		m_pLevelSpecialBn->Enable(bEnable);
	if (m_pLevelEventBn)
		m_pLevelEventBn->Enable(bEnable);
	if (m_pCharacterSelectList)
		m_pCharacterSelectList->Enable(bEnable);
	if (m_pRecycleItemList)
		m_pRecycleItemList->Enable(bEnable);
	if (m_pMixBn)
		m_pMixBn->Enable(bEnable);
	if (m_pArrowMinimumBn)
		m_pArrowMinimumBn->Enable(bEnable);
	if (m_pArrowLeftBn)
		m_pArrowLeftBn->Enable(bEnable);
	if (m_pArrowRightBn)
		m_pArrowRightBn->Enable(bEnable);
	if (m_pArrowMaximumBn)
		m_pArrowMaximumBn->Enable(bEnable);
}

void FrRecycleDlg::EnableMixControls(bool bEnable)
{
	if (m_pMixBn)
		m_pMixBn->Enable(bEnable);
	if (m_pArrowMinimumBn)
		m_pArrowMinimumBn->Enable(bEnable);
	if (m_pArrowLeftBn)
		m_pArrowLeftBn->Enable(bEnable);
	if (m_pArrowRightBn)
		m_pArrowRightBn->Enable(bEnable);
	if (m_pArrowMaximumBn)
		m_pArrowMaximumBn->Enable(bEnable);
}

bool FrRecycleDlg::IsUsableParts(sItemInfo& item)
{
	CSharedDoc* doc = Doc();
	std::map<unsigned int, sCharacterInfo>::iterator it =
		doc->m_charMap.begin();
	std::map<unsigned int, sCharacterInfo>::iterator end = doc->m_charMap.end();
	for (; it != end; ++it)
	{
		if ((*it).second.tid == (m_selCharacter | 0x04000000))
		{
			if (doc->IsEquipParts((*it).second, item.guid, item.tid))
				return false;
			break;
		}
	}
	return true;
}

void FrRecycleDlg::SetDataTotalItem(int index)
{
	const sRecycleItem* recipe = GetRecycleItem((short)index);
	if (recipe)
	{
		m_totalItem.dwTid = recipe->dwTid;
		m_totalItem.iNum = recipe->iNum;
		m_totalItem.dwRandSeq = recipe->dwRandSeq;
		strcpy(m_totalItem.szRandName, recipe->szRandName);
	}
	else
	{
		memset(&m_totalItem, 0, sizeof(m_totalItem));
	}
	if (m_pRecycleItemName)
	{
		m_pRecycleItemName->ClearLine();
		IFF_ITEM_COMMON* item =
			ItemManager()->FindCommonItem(m_totalItem.dwTid);
		if (item)
		{
			std::string name(item->Name);
			if (m_totalItem.dwRandSeq)
				name = m_totalItem.szRandName;
			if (name.length() > 20)
			{
				char truncated[32] = { 0 };
				mbscut(truncated, name.c_str(), 17);
				name = truncated;
				name += "...";
			}
			m_pRecycleItemName->AddText(
				MakeStr("\\cT\\c%s\\cN\\c", name.c_str()), false, true);
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		m_needItem[i].dwTid = GetTidMixItem((short)index, i);
		m_needItem[i].iNum = GetNumMixItem((short)index, i);
		m_haveItem[i].dwTid = m_needItem[i].dwTid;
		m_haveItem[i].iNum = 0;
	}
	for (int slot = 0; slot < 4; ++slot)
	{
		unsigned long tid = m_haveItem[slot].dwTid;
		if (!tid)
			break;
		switch (tid >> 26)
		{
		case 6:
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
				if ((*it).tid == tid)
				{
					m_haveItem[slot].dwItemid = (*it).guid;
					m_haveItem[slot].iNum += (*it).Common[0];
				}
			break;
		case 5:
			for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
				it != Doc()->m_ballList.end(); ++it)
				if ((*it).tid == tid)
				{
					m_haveItem[slot].dwItemid = (*it).guid;
					m_haveItem[slot].iNum += (*it).Common[0];
				}
			break;
		case 28:
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_auxPartList.begin();
				it != Doc()->m_auxPartList.end(); ++it)
				if ((*it).tid == tid)
				{
					m_haveItem[slot].dwItemid = (*it).guid;
					m_haveItem[slot].iNum += (*it).Common[0];
				}
			break;
		case 2:
			for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
				it != Doc()->m_partsList.end(); ++it)
				if ((*it).tid == m_haveItem[slot].dwTid && IsUsableParts(*it))
				{
					m_haveItem[slot].dwItemid = (*it).guid;
					++m_haveItem[slot].iNum;
				}
			break;
		case 7:
			for (std::map<unsigned int, sCaddieInfo>::iterator it =
					 Doc()->m_caddieMap.begin();
				it != Doc()->m_caddieMap.end(); ++it)
				if ((*it).second.tid == tid)
				{
					m_haveItem[slot].dwItemid = (*it).second.guid;
					++m_haveItem[slot].iNum;
				}
			break;
		case 9:
		{
			IFF_STRUCT::sSetItem* set = ItemManager()->FindSetItem(tid);
			if (set)
			{
				int counts[10] = { 0 };
				for (int component = 0; component < set->nElems; ++component)
				{
					for (std::list<sItemInfo>::iterator it =
							 Doc()->m_partsList.begin();
						it != Doc()->m_partsList.end(); ++it)
						if ((*it).tid == set->ElemIds[component])
							++counts[component];
					sCharacterInfo& character =
						Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
					for (int part = 0; part < 24; ++part)
						if (set->ElemIds[component] == character.tidParts[part])
						{
							--counts[component];
							break;
						}
				}
				int count = INT_MAX;
				for (int component = 0; component < set->nElems; ++component)
					if (counts[component] < count)
						count = counts[component];
				m_haveItem[slot].iNum = count;
			}
		}
		break;
		case 4:
			for (std::map<unsigned int, sItemInfo>::iterator it =
					 Doc()->m_clubSetMap.begin();
				it != Doc()->m_clubSetMap.end(); ++it)
				if ((*it).second.tid == tid &&
					Doc()->m_myInfo.userEquip.guidClubSet != (*it).second.guid)
				{
					m_haveItem[slot].dwItemid = (*it).second.guid;
					++m_haveItem[slot].iNum;
					break;
				}
			break;
		case 31:
			for (std::list<sCards>::iterator it = Doc()->m_cardList.begin();
				it != Doc()->m_cardList.end(); ++it)
				if ((*it).tid == tid)
				{
					m_haveItem[slot].dwItemid = (*it).uid;
					m_haveItem[slot].iNum += (*it).cardCount;
				}
			break;
		}
		tid = m_haveItem[slot].dwTid;
		switch (tid >> 26)
		{
		case 4:
		{
			std::map<unsigned int, sItemInfo>::iterator it =
				Doc()->m_clubSetMap.find(Doc()->m_myInfo.userEquip.guidClubSet);
			if (tid == (*it).second.tid && m_haveItem[slot].iNum > 0)
				--m_haveItem[slot].iNum;
		}
		break;
		case 6:
			for (int i = 0; i < 10; ++i)
				if (tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
					--m_haveItem[slot].iNum;
			break;
		case 7:
		{
			std::map<unsigned int, sCaddieInfo>::iterator it =
				Doc()->m_caddieMap.find(Doc()->m_myInfo.userEquip.guidCaddie);
			if (tid == (*it).second.tid)
				--m_haveItem[slot].iNum;
		}
		break;
		}
	}
}

void FrRecycleDlg::ControlRecycleNumber(int mode)
{
	switch (mode)
	{
	case 0:
		m_recycleNumber = 0;
		break;
	case 1:
		--m_recycleNumber;
		break;
	case 2:
		++m_recycleNumber;
		break;
	case 3:
		m_recycleNumber = m_maxRecycleNumber;
		break;
	default:
		m_recycleNumber = 0;
		break;
	}
	if (m_maxRecycleNumber > 0)
	{
		if (m_recycleNumber >= m_maxRecycleNumber)
			m_recycleNumber = m_maxRecycleNumber;
		else if (m_recycleNumber < 0)
			m_recycleNumber = 0;
	}
	else
		m_recycleNumber = 0;
	unsigned long group = m_totalItem.dwTid >> 26;
	if (group < 5 || (group > 6 && group != 28))
		if (m_recycleNumber > 1)
			m_recycleNumber = 1;
	if (m_totalItem.dwRandSeq > 0 && m_recycleNumber > 1)
		m_recycleNumber = 1;
	if (m_pRecycleNumber)
		m_pRecycleNumber->SetLine(1, MakeStr("%d", m_recycleNumber), 0, false,
			0);
}

void FrRecycleDlg::SetMaxEnableRecycle(int index)
{
	int maximum = 0x7fffffff;
	m_maxRecycleNumber = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (!m_needItem[i].dwTid || m_needItem[i].iNum <= 0)
			break;
		if (m_haveItem[i].iNum < m_needItem[i].iNum)
		{
			maximum = 0;
			break;
		}
		int count = m_haveItem[i].iNum / m_needItem[i].iNum;
		if (count < maximum)
			maximum = count;
	}
	m_maxRecycleNumber = maximum;
}

void FrRecycleDlg::SetResultRecycleItem(bool success,
	std::list<sRecycleItem>& items, short index)
{
	if (success)
	{
		sRecycleItem result;
		memset(&result, 0, sizeof(result));
		unsigned long tid = GetTidRecycleItem(index);
		if ((tid >> 26) == 9)
		{
			IFF_STRUCT::sSetItem* set = ItemManager()->FindSetItem(tid);
			if (!set)
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
			else
			{
				int count = set->nElems;
				if (count != items.size())
					AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
						(int)"\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
				else
				{
					for (int i = 0; i < count; ++i)
					{
						bool found = false;
						std::list<sRecycleItem>::iterator it = items.begin();
						for (; it != items.end(); ++it)
							if ((*it).dwTid == set->ElemIds[i])
							{
								found = true;
								break;
							}
						if (!found)
						{
							AfxGetTask()->SendMsgToMainActor(MsgObject(NULL,
								0x23,
								(int)"\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
								0, 0, 0, 0));
							break;
						}
					}
					const sRecycleItem* recipe = GetRecycleItem(index);
					if (recipe)
						result = *recipe;
				}
			}
		}
		else
		{
			if (items.size() == 1)
			{
				std::list<sRecycleItem>::iterator it = items.begin();
				sRecycleItem& item = *it;
				result = item;
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
			}
		}
		if (m_level == 3 || m_level == 4)
			SetListRecycleItem(m_level, m_selCharacter);
		m_totalItem = result;
		m_resultItem = result;
		m_resultItem.iNum =
			m_recycleNumber * GetNumRecycleItem((short)m_selRecycleIndex);
		if (m_pSelectPartBn)
			m_pSelectPartBn->SetVisible(false);
	}
	else
	{
		memset(&m_resultItem, 0, sizeof(m_resultItem));
	}
	m_resultTime = 3.0f;
	m_bMixing = false;
	EnableTotalControls(true);
	SetDataTotalItem(m_selRecycleIndex);
	SetMaxEnableRecycle(m_selRecycleIndex);
	g_audio->PlaySfx(
		"\xbe\xc6\xc0\xcc\xc5\xdb\xc8\xb9\xb5\xe6(\xb7\xb9\xbe\xee)", NULL, 0,
		NULL, NULL, 0.5f, 200.0f);
	if (m_selRecycleIndex >= 0)
	{
		ControlRecycleNumber(0);
		if (m_maxRecycleNumber > 0)
		{
			if (IsHaveItem(m_resultItem.dwTid))
			{
				ControlRecycleNumber(-1);
				EnableMixControls(false);
			}
			else
				EnableMixControls(true);
		}
		else
			EnableMixControls(false);
	}
	else
	{
		ControlRecycleNumber(-1);
		EnableMixControls(false);
	}
}

void FrRecycleDlg::SendDataRecycleItem(int index, int count)
{
	if (index < 0 || count <= 0)
	{
		OnFreshOkay();
	}
	else
	{
		unsigned char items = 0;
		WSendPacket packet((enumClientPacket)0x7e);
		packet.Encode2(index);
		packet.Encode1(count);
		packet.Encode1(0);
		int offset = packet.GetLengthData();
		for (int i = 0; i < 4; ++i)
		{
			if (m_haveItem[i].dwTid)
			{
				unsigned long item[2] = { m_haveItem[i].dwTid,
					m_haveItem[i].dwItemid };
				packet.EncodeBuffer(item, sizeof(item));
				++items;
			}
		}
		packet.EncodeData(3, (char*)&items, 1);
		packet.Send(TO_GAME);
	}
	unsigned long selected = m_selPartId;
	m_bPartSelected = false;
	if (m_haveItem[0].iNum < 3)
	{
		if (m_pSelectPartBn)
		{
			ClickRecycleItem();
			m_pSelectPartBn->SetVisible(false);
			std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			std::list<sItemInfo>::iterator end = Doc()->m_partsList.end();
			for (; it != end; ++it)
			{
				if ((*it).guid != selected && (*it).tid == m_haveItem[0].dwTid)
				{
					m_selPartId = (*it).guid;
					break;
				}
			}
		}
	}
	else
	{
		m_selPartId = 0;
		ClickRecycleItem();
	}
}

namespace
{
	unsigned long g_randItemList[] = { 0x1a0000cd, 0x1a0000ce, 0x1a0000cf };
}

bool RandRecycleItem(unsigned long typeId)
{
	for (int i = 0; i < 3; ++i)
		if (typeId == g_randItemList[i])
			return true;
	return false;
}

void FrRecycleDlg::OpenInformation(unsigned long typeId)
{
	FrWnd* existing =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (!existing)
	{
		FrForm* form = DYNAMIC_CAST(FrForm, existing);
		existing = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			"information_setitem");
		if (!existing)
		{
			FrSetItemDlg* setForm = DYNAMIC_CAST(FrSetItemDlg, existing);
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
			if (item)
			{
				if ((item->TypeId & 0xfc000000) == 0x24000000)
				{
					setForm = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(),
						this, "information_setitem");
					if (setForm)
					{
						setForm->SetSetItem(item->TypeId);
						setForm->Open(NULL, 3);
					}
				}
				else
				{
					form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
						"information");
					if (form)
					{
						unsigned char random = 0;
						if (RandRecycleItem(item->TypeId))
							random = 1;
						FrArea* portrait = DYNAMIC_CAST(FrArea,
							form->FindChildByName("portrait"));
						if (portrait)
						{
							if (random == 0)
								portrait->SetBgImg(item->Icon);
							else if (random == 1)
								portrait->SetBgImg("hide");
							else
								portrait->SetBgImg("hide");
						}
						FrEdit* name =
							DYNAMIC_CAST(FrEdit, form->FindChildByName("name"));
						FrArea* level = DYNAMIC_CAST(FrArea,
							form->FindChildByName("level"));
						if (name && level)
						{
							if (random == 0)
							{
								name->AddText(
									MakeStr("\xc0\xcc\xb8\xa7\x3a\x20\x25\x73",
										item->Name),
									false, true);
								name->AddText("\n", false, true);
							}
							else if (random == 1)
							{
								name->AddText(
									"\xc0\xcc\xb8\xa7: \xb7\xa3\xb4\xfd\xc1\xb6\xc7\xd5\xbe\xc6\xc0\xcc\xc5\xdb",
									false, true);
								name->AddText("\n", false, true);
							}
							if (item->IsUnderLvl)
							{
								level->SetBgImg(
									MakeStr("level_%03d", item->Level + 1));
								name->AddText(
									MakeStr(
										"\xb7\xb9\xba\xa7\x3a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xc0\xcc\xc7\xcf",
										item->Name),
									false, true);
							}
							else
							{
								level->SetBgImg(
									MakeStr("level_%03d", item->Level + 1));
								name->AddText(
									MakeStr(
										"\xb7\xb9\xba\xa7\x3a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xc0\xcc\xbb\xf3",
										item->Name),
									false, true);
							}
						}
						if (random == 0)
						{
							IFF_STRUCT::sDesc* desc =
								ItemManager()->FindDesc(item->TypeId);
							if (desc)
								form->SetMessage(desc->Desc, false);
						}
						else
						{
							form->SetMessage(
								"\xc8\xae\xb7\xfc\xbf\xa1 \xb5\xfb\xb6\xf3 \xc1\xb6\xc7\xd5\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xb0\xe1\xc1\xa4\xb5\xcb\xb4\xcf\xb4\xd9.",
								false);
						}
						form->Open(NULL, 3);
					}
				}
			}
		}
	}
}

bool IsTunaClubset(unsigned long typeId)
{
	if (typeId != 0x10000022 && (typeId <= 0x1000002e || typeId > 0x10000030))
		return false;
	return true;
}

bool CheckRandomRecycleItem(const sRecycleItem* item)
{
	if (item->dwTid != 0x1800000d)
		return false;
	if (item->dwRandSeq == 8)
	{
		std::list<const IFF_STRUCT::sRandomRecycle*> rewards;
		std::list<const IFF_STRUCT::sRandomRecycle*>::const_iterator reward;
		if (ItemManager()->GetRecycleRandomMixOutItems(item->dwRandSeq,
				rewards))
		{
			for (reward = rewards.begin(); reward != rewards.end(); ++reward)
			{
				const IFF_STRUCT::sRandomRecycle* random = *reward;
				if (IsTunaClubset(random->uiTypeId))
				{
					std::map<unsigned int, sItemInfo>::iterator it =
						Doc()->m_clubSetMap.begin();
					std::map<unsigned int, sItemInfo>::iterator end =
						Doc()->m_clubSetMap.end();
					for (; it != end; ++it)
						if (random->uiTypeId == (*it).second.tid)
							return true;
				}
			}
		}
	}
	return false;
}

bool FrRecycleDlg::IsHaveItem(const sRecycleItem* item)
{
	if (item)
	{
		switch (item->dwTid >> 26)
		{
		case 6:
			if (CheckRandomRecycleItem(item))
				return true;
			if (IsHaveItem(item->dwTid))
				return true;
			break;
		default:
			if (IsHaveItem(item->dwTid))
				return true;
			break;
		}
	}
	return false;
}

bool FrRecycleDlg::IsHaveItem(unsigned long typeId)
{
	bool have = false;
	switch (typeId >> 26)
	{
	case 2:
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
			if ((*it).tid == typeId &&
				ItemManager()->FindCommonItem(typeId)->IsCash)
			{
				have = true;
				break;
			}
	}
	break;
	case 4:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.begin();
		std::map<unsigned int, sItemInfo>::iterator end =
			Doc()->m_clubSetMap.end();
		for (; it != end; ++it)
			if ((*it).second.tid == typeId)
			{
				have = true;
				break;
			}
	}
	break;
	case 7:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.begin();
		std::map<unsigned int, sCaddieInfo>::iterator end =
			Doc()->m_caddieMap.end();
		for (; it != end; ++it)
		{
			if ((*it).second.tid == typeId)
			{
				have = true;
				break;
			}
		}
	}
	break;
	case 14:
	{
		std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
		std::list<sItemInfo>::iterator end = Doc()->m_cutinList.end();
		for (; it != end; ++it)
		{
			if ((*it).tid == typeId)
			{
				have = true;
				break;
			}
		}
	}
	break;
	}
	return have;
}

void FrRecycleDlg::SetCadieMessage(const char* msg)
{
	if (m_pCadieMessage)
	{
		m_pCadieMessage->ClearLine();
		m_pCadieMessage->AddText(msg, false, true);
	}
}

bool FrRecycleDlg::SetEventMessage(int type)
{
	bool result = false;
	if (type == 4 && Doc()->m_myInfo.info.gender)
	{
		SetCadieMessage(g_cadieEventMessage.c_str());
		m_messageTime = 5.0f;
		result = true;
	}
	return result;
}

IMPLEMENT_OBJECT(FrRecycleAboutDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrRecycleAboutDlg, FrForm)
ON_FRESH_VI("recycle_about", FRCMD_INIT, FrRecycleAboutDlg::OnAboutInit)
ON_FRESH_VI("aboutviewer", FRCMD_INIT, FrRecycleAboutDlg::OnAboutViewerInit)
ON_FRESH_VI("index01", FRCMD_INIT, FrRecycleAboutDlg::OnIndex01Init)
ON_FRESH_VV("index01", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnIndex01BnUp)
ON_FRESH_VI("index02", FRCMD_INIT, FrRecycleAboutDlg::OnIndex02Init)
ON_FRESH_VV("index02", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnIndex02BnUp)
ON_FRESH_VI("index03", FRCMD_INIT, FrRecycleAboutDlg::OnIndex03Init)
ON_FRESH_VV("index03", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnIndex03BnUp)
ON_FRESH_VI("index04", FRCMD_INIT, FrRecycleAboutDlg::OnIndex04Init)
ON_FRESH_VV("index04", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnIndex04BnUp)
ON_FRESH_VI("index05", FRCMD_INIT, FrRecycleAboutDlg::OnIndex05Init)
ON_FRESH_VV("index05", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnIndex05BnUp)
ON_FRESH_VI("backward", FRCMD_INIT, FrRecycleAboutDlg::OnBackwardBnInit)
ON_FRESH_VV("backward", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnBackwardBnUp)
ON_FRESH_VI("forward", FRCMD_INIT, FrRecycleAboutDlg::OnForwardBnInit)
ON_FRESH_VV("forward", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnForwardBnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrRecycleAboutDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrRecycleAboutDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrRecycleAboutDlg::FrRecycleAboutDlg()
{
	for (int i = 0; i < 5; ++i)
		m_pIndex[i] = NULL;
	m_aboutIndex = 0;
	m_pBackwardBn = NULL;
	m_pForwardBn = NULL;
}

FrRecycleAboutDlg::~FrRecycleAboutDlg()
{
}

void FrRecycleAboutDlg::OnAboutInit(int param)
{
}

void FrRecycleAboutDlg::OnAboutViewerInit(int param)
{
	m_pViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex01Init(int param)
{
	m_pIndex[0] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex01BnUp()
{
	SetImgAboutIndex(0);
}

void FrRecycleAboutDlg::OnIndex02Init(int param)
{
	m_pIndex[1] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex02BnUp()
{
	SetImgAboutIndex(1);
}

void FrRecycleAboutDlg::OnIndex03Init(int param)
{
	m_pIndex[2] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex03BnUp()
{
	SetImgAboutIndex(2);
}

void FrRecycleAboutDlg::OnIndex04Init(int param)
{
	m_pIndex[3] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex04BnUp()
{
	SetImgAboutIndex(3);
}

void FrRecycleAboutDlg::OnIndex05Init(int param)
{
	m_pIndex[4] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnIndex05BnUp()
{
	SetImgAboutIndex(4);
}

void FrRecycleAboutDlg::OnBackwardBnInit(int param)
{
	m_pBackwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnBackwardBnUp()
{
	--m_aboutIndex;
	SetImgAboutIndex(m_aboutIndex);
	m_pBackwardBn->SetPushDelay(0);
}

void FrRecycleAboutDlg::OnForwardBnInit(int param)
{
	m_pForwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnForwardBnUp()
{
	++m_aboutIndex;
	SetImgAboutIndex(m_aboutIndex);
	m_pForwardBn->SetPushDelay(0);
}

void FrRecycleAboutDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrRecycleAboutDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrRecycleAboutDlg::OnInit()
{
	SetImgAboutIndex(0);
	return true;
}

void FrRecycleAboutDlg::SetImgAboutIndex(int index)
{
	for (int i = 0; i < 5; ++i)
		m_pIndex[i]->Unselect(false);
	if (m_pBackwardBn)
		m_pBackwardBn->Enable(true);
	if (m_pForwardBn)
		m_pForwardBn->Enable(true);
	if (index <= 0)
	{
		index = 0;
		m_pIndex[0]->Select(false);
		if (m_pBackwardBn)
			m_pBackwardBn->Enable(false);
	}
	else if (index >= 4)
	{
		index = 4;
		m_pIndex[4]->Select(false);
		if (m_pForwardBn)
			m_pForwardBn->Enable(false);
	}
	m_pIndex[index]->Select(false);
	m_aboutIndex = index;
	if (m_pViewer)
		m_pViewer->Open(MakeStr("help_cadie_%02d.jpg", index + 1));
}

void FrRecycleDlg::DrawPartsSlot(FrGraphicInterface* graphic, sItemInfo* item,
	FrArea* area)
{
	int slot;
	if (!item || !area)
		return;
	if (!graphic)
		graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float characterX = 1.0f;
	float caddieX = 1.0f;
	if ((int)item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CharacterSlotNum = part->CharacterSlot;
			if ((int)item->CharacterSlotNum <= -1 ||
				item->CharacterSlotNum >= 5)
				item->CharacterSlotNum = 0;
		}
		else
		{
			item->CharacterSlotNum = 0;
		}
	}
	for (slot = 1; slot < item->CharacterSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->tid == (*it).partsTid && item->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 0)
				{
					found = true;
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(area->GetRect().x + characterX,
								area->GetRect().y + 15.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						characterX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(area->GetRect().x + characterX,
						area->GetRect().y + 15.0f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
				characterX += 16.0f;
			}
		}
	}
	if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
		{
			item->CaddieSlotNum = 0;
		}
	}
	for (slot = 1; slot < item->CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->tid == (*it).partsTid && item->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 1)
				{
					found = true;
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr(card->SlotImg));
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(area->GetRect().x + caddieX,
								area->GetRect().y + 31.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						caddieX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(area->GetRect().x + caddieX,
						area->GetRect().y + 31.0f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.0f;
			}
		}
	}
}

void FrRecycleDlg::OnSelectPartInit(int param)
{
	m_pSelectPartBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSelectPartBn)
	{
		m_pSelectPartBn->SetPushDelay(0.1f);
		m_pSelectPartBn->SetVisible(false);
	}
}

void FrRecycleDlg::OnSelectPartLBDown()
{
	if (!m_pSelectPartDlg)
	{
		m_pSelectPartDlg = CreateForm<FrSelectPartDlg>(g_pFresh->GetManager(),
			this, "select_part_dlg");
		if (m_pSelectPartDlg)
		{
			m_selPartId = 0;
			m_bPartSelected = false;
			m_pSelectPartDlg->SetPartTID(m_haveItem[0].dwTid);
			m_pSelectPartDlg->Open(
				(FRESH_PFN_RESULT)&FrRecycleDlg::OnSelectPartDlgResult,
				WPoint(68.0f, 48.0f), 0);
		}
	}
}

bool FrRecycleDlg::OnSelectPartDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		m_selPartId = m_pSelectPartDlg->m_selectedId;
		m_bPartSelected = true;
	}
	else
	{
		m_bPartSelected = false;
		m_selPartId = 0;
	}
	m_pSelectPartDlg = NULL;
	return true;
}

void FrRecycleDlg::OnCardSlotInit(int param)
{
	m_pCardSlot = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRecycleDlg::OnCardSlotResultInit(int param)
{
	m_pCardSlotResult = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrRecycleDlg::OnCardSlotOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	if (Doc()->m_partsList.empty())
		return;
	sItemInfo* item = NULL;
	std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
	std::list<sItemInfo>::iterator end = Doc()->m_partsList.end();
	for (; it != end; ++it)
	{
		item = &*it;
		if (item->guid == m_selPartId)
			break;
	}
	if (!item->tid)
		return;
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
	if (part)
	{
		item->CharacterSlotNum = part->CharacterSlot;
		item->CaddieSlotNum = part->CaddieSlot;
		if ((int)item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
			item->CharacterSlotNum = 0;
		if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
			item->CaddieSlotNum = 0;
	}
	else
	{
		item->CharacterSlotNum = 0;
		item->CaddieSlotNum = 0;
	}
	if (m_bPartSelected == true)
	{
		if (m_pSelectPartBn && m_pSelectPartBn->IsVisible())
			DrawPartsSlot(graphic, item, m_pCardSlot);
	}
	else
	{
		if (m_pSelectPartBn && !m_pSelectPartBn->IsVisible() && m_selPartId &&
			(m_haveItem[0].dwTid >> 26) == 2 && m_haveItem[0].iNum > 0)
			DrawPartsSlot(graphic, item, m_pCardSlot);
	}
}

IMPLEMENT_OBJECT(FrSelectPartDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrSelectPartDlg, FrForm)
ON_FRESH_VI("list_box", FRCMD_INIT, FrSelectPartDlg::OnInitListBox)
ON_FRESH_VV("list_box", FRCMD_LBUTTONUP, FrSelectPartDlg::OnLBUpListBox)
ON_FRESH_VI("list_box", FRCMD_OWNERDRAW, FrSelectPartDlg::OnOwnerDrawListBox)
END_FRESH_MSGMAP()

FrSelectPartDlg::FrSelectPartDlg()
{
	m_pListBox = NULL;
	m_partTypeId = 0;
	m_selectedId = 0;
}

FrSelectPartDlg::~FrSelectPartDlg()
{
	m_partTypeId = 0;
}

void FrSelectPartDlg::SetPartTID(unsigned long typeId)
{
	m_partTypeId = typeId;
	if (!m_pListBox)
		return;
	m_pListBox->ClearItem();
	for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
		it != Doc()->m_partsList.end(); ++it)
	{
		sItemInfo* item = &*it;
		if (item->tid != m_partTypeId)
			continue;
		bool equipped = false;
		for (std::map<unsigned int, sCharacterInfo>::iterator character =
				 Doc()->m_charMap.begin();
			character != Doc()->m_charMap.end(); ++character)
		{
			for (int slot = 0; slot < 24; ++slot)
			{
				if (item->guid == (*character).second.ItemIdList[slot])
				{
					equipped = true;
					break;
				}
			}
			if (equipped)
				break;
		}
		if (!equipped)
			m_pListBox->AddItem(item);
	}
}

void FrSelectPartDlg::OnInitListBox(int param)
{
	m_pListBox = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrSelectPartDlg::OnLBUpListBox()
{
}

void FrSelectPartDlg::OnOwnerDrawListBox(int param)
{
	if (!m_pListBox)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	sItemInfo* item = (sItemInfo*)row->pData;
	if (!item)
		return;
	const Bitmap* images[] = { g_pFresh->RegisterBitmap("item_base_n"),
		g_pFresh->RegisterBitmap("item_base_o"),
		g_pFresh->RegisterBitmap("item_base_d"),
		g_pFresh->RegisterBitmap("item_base_empty") };
	const Bitmap* bitmap;
	if (!row->pData)
	{
		bitmap = images[3];
	}
	else
	{
		if (row->selected)
		{
			bitmap = images[2];
			m_selectedId = item->guid;
			Close(FrOK, true);
		}
		else if (row->underCursor)
			bitmap = images[2];
		else
			bitmap = images[0];
	}
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + 5.0f, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	unsigned long typeId = item->tid;
	const char* name =
		"\xbe\xcb\xbc\xf6\xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb";
	const char* icon = "caddie_no";
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	if (common)
	{
		name = common->Name;
		icon = common->Icon;
	}
	bitmap = g_pFresh->GetBitmap(icon);
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + 7.0f, row->pos.y + 2.0f, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->Print11(row->pos + WPoint(182.0f, 7.0f), 2, "%s", name);
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
	if (part)
	{
		DrawStatus2(graphic, row, part->Attr, part->Slot, 0.0f);
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			item->CharacterSlotNum = part->CharacterSlot;
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CharacterSlotNum <= -1 ||
				item->CharacterSlotNum >= 5)
				item->CharacterSlotNum = 0;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
	}
	else
	{
		item->CharacterSlotNum = 0;
		item->CaddieSlotNum = 0;
	}
	DrawPartsSlot(graphic, row);
}

static const char* MakeStatusString(int stat, int value, int slot)
{
	FrWndManager* pManager = g_pFresh->GetManager();
	const char* names[] = { "\xc6\xc4\xbf\xf6", "\xc4\xc1\xc6\xae\xb7\xd1",
		"\xc1\xa4\xc8\xae\xb5\xb5", "\xbd\xba\xc7\xc9", "\xc4\xbf\xba\xea" };
	if (value)
	{
		if (slot)
			return MakeStr("%s+%d/%s+%d", names[stat], value,
				"\xbd\xbd\xb7\xd4", slot);
		return MakeStr("%s+%d", names[stat], value);
	}
	if (slot)
		return MakeStr("%s%s+%d", names[stat], "\xbd\xbd\xb7\xd4", slot);
	return "";
}

void FrSelectPartDlg::DrawStatus2(FrGraphicInterface* graphic, FrListItem* row,
	short* const x, short* const y, float alpha)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		if (x[i] || y[i])
		{
			graphic->SetTextColor(0xff0000ff, 0xffffffff);
			graphic->Print11(row->pos +
					WPoint(182.0f + alpha, 36.0f + alpha + dy),
				2, MakeStatusString(i, x[i], y[i]));
			dy += 14.0f;
		}
	}
}

void FrSelectPartDlg::DrawPartsSlot(FrGraphicInterface* graphic,
	FrListItem* row)
{
	int slot;
	sItemInfo* item = (sItemInfo*)row->pData;
	if (!item)
		return;
	if (!graphic)
		graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float characterX = 7.0f;
	float caddieX = 7.0f;
	if ((int)item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CharacterSlotNum = part->CharacterSlot;
			if ((int)item->CharacterSlotNum <= -1 ||
				item->CharacterSlotNum >= 5)
				item->CharacterSlotNum = 0;
		}
		else
		{
			item->CharacterSlotNum = 0;
		}
	}
	for (slot = 1; slot < item->CharacterSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->tid == (*it).partsTid && item->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 0)
				{
					found = true;
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + characterX, row->pos.y + 54.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						characterX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + characterX, row->pos.y + 54.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				characterX += 16.0f;
			}
		}
	}
	if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
		{
			item->CaddieSlotNum = 0;
		}
	}
	for (slot = 1; slot < item->CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->tid == (*it).partsTid && item->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 1)
				{
					found = true;
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr(card->SlotImg));
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						caddieX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.0f;
			}
		}
	}
}
