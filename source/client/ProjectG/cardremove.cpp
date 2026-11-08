#include "stdafx.h"
#include "cardremove.h"
#include "realmyroommain.h"
#include "ucclibrary.h"
extern Fresh* g_pFresh;

void DrawPartsSlot(FrGraphicInterface* device, sItemInfo* item,
	FrListItem* listItem)
{
	if (!item || !listItem)
		return;
	if (!device)
		device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;

	float x = 220, y = 220;
	int slot;

	if (item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CharacterSlotNum = part->CharacterSlot;
			if (item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
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
						WRect rect(listItem->pos.x + x, listItem->pos.y + 3,
							(float)bitmap->Width(), (float)bitmap->Height());
						device->DrawTexture(bitmap, rect, 0xffffffff, 0);
						x += 14;
					}
				}
			}
		}

		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				WRect rect(listItem->pos.x + x, listItem->pos.y + 3,
					(float)bitmap->Width(), (float)bitmap->Height());
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
				x += 14;
			}
		}
	}

	if (item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if (item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
			item->CaddieSlotNum = 0;
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
						WRect rect(listItem->pos.x + y, listItem->pos.y + 15,
							(float)bitmap->Width(), (float)bitmap->Height());
						device->DrawTexture(bitmap, rect, 0xffffffff, 0);
						y += 14;
					}
				}
			}
		}

		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				WRect rect(listItem->pos.x + y, listItem->pos.y + 15,
					(float)bitmap->Width(), (float)bitmap->Height());
				device->DrawTexture(bitmap, rect, 0xffffffff, 0);
				y += 14;
			}
		}
	}
}

static unsigned long CardRemoveItems[] = { 436207810, 436207821, 436207822,
	436207823 };

bool FrCardRemove::IsCardRemoveItem(unsigned long tid)
{
	bool result = false;

	for (int i = 0; i < 4; ++i)
	{
		if (CardRemoveItems[i] == tid)
		{
			result = true;
			break;
		}
	}

	return result;
}

FrCardRemove::FrCardRemove()
{
	m_pDrawArea = NULL;
	m_pItemList = NULL;
	m_itemTypeId = 0;
	m_itemUid = 0;
	m_partTypeId = 0;
	m_partUid = 0;
	m_cardList.clear();
}

FrCardRemove::~FrCardRemove()
{
}

void FrCardRemove::DrawInDlg(const char* name, float x, float y)
{
	if (!name)
		return;

	const Bitmap* bitmap = g_pFresh->GetBitmap(name);
	if (!bitmap)
		return;

	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (device)
	{
		WRect rect = m_rect;
		rect.x = (x + 2) + rect.x;
		rect.y = (y + 23) + rect.y;
		const BITMAPINFO* info = bitmap->bi;
		rect.w = (unsigned)info->bmiHeader.biWidth;
		rect.h = (unsigned)info->bmiHeader.biHeight;
		device->DrawTexture(bitmap, rect, 0xffffffff, 0);
	}
}

void FrCardRemove::PrintInDlg(const char* text, float x, float y)
{
	if (!text)
		return;

	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;

	WPoint point(m_rect.x, m_rect.y);
	point.x += x;
	point.y += y;
	device->PrintText(point, 0, text, NULL);
}

void FrCardRemove::DrawTexture(const char* name, float x, float y,
	unsigned long color)
{
	if (!name)
		return;

	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;

	const Bitmap* bitmap = g_pFresh->GetBitmap(name);
	if (!bitmap)
		return;

	WRect rect(x, y, (float)bitmap->Width(), (float)bitmap->Height());
	device->DrawTexture(bitmap, rect, color, 0);
}

void FrCardRemove::PrintText(const char* text, float x, float y,
	unsigned long color)
{
	if (!text)
		return;

	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;

	device->SetTextColor(color, 0xffffffff);
	device->PrintText(WPoint(x, y), 0, text, NULL);
	device->SetTextColor(0xff000000, 0xffffffff);
}

bool FrCardRemove::CheckEquiped(unsigned long uid)
{
	bool result = false;
	CHAR_MAP_IT it;

	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
	{
		for (int i = 0; i < 24; ++i)
		{
			if ((*it).second.ItemIdList[i] && (*it).second.ItemIdList[i] == uid)
			{
				result = true;
				goto done;
			}
		}
	}

done:
	return result;
}

void FrCardRemove::SetCardRemoveItem(unsigned long tid, unsigned long uid)
{
	m_itemTypeId = tid;
	m_itemUid = uid;
}

bool FrCardRemove::SetCurPartUid(unsigned long uid)
{
	bool result = false;

	if (uid != m_partUid)
	{
		m_partUid = uid;
		result = true;
	}

	return result;
}

void FrCardRemove::BuildCurPartCardList()
{
	m_cardList.clear();
	std::list<sSCardAvilityPeriodInfo>::iterator
		it = Doc()->m_cardAbilityList[0].begin(),
		end = Doc()->m_cardAbilityList[0].end();
	for (; it != end; ++it)
		if ((*it).partsUid == m_partUid)
			m_cardList.push_back((*it).tid);
}

IMPLEMENT_OBJECT(FrCardRemove, FrForm)
BEGIN_FRESH_MSGMAP(FrCardRemove, FrForm)
ON_FRESH_VI("draw_area", FRCMD_INIT, FrCardRemove::OnDrawAreaInit)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW, FrCardRemove::OnDrawAreaOwnerDraw)
ON_FRESH_VI("item_list", FRCMD_INIT, FrCardRemove::OnItemListInit)
ON_FRESH_VI("item_list", FRCMD_OWNERDRAW, FrCardRemove::OnItemListOwnerDraw)
ON_FRESH_VV("item_list", FRCMD_DBLCLICK, FrCardRemove::OnItemListDBClick)
ON_FRESH_VV("item_list", FRCMD_RBUTTONDOWN, FrCardRemove::OnItemListRBDown)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrCardRemove::OnConfirmInit)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN, FrCardRemove::OnConfirmLBDown)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrCardRemove::OnCancelInit)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN, FrCardRemove::OnCancelLBDown)
END_FRESH_MSGMAP()

void FrCardRemove::OnDrawAreaInit(int param)
{
	m_pDrawArea = DYNAMIC_CAST(FrArea, param);
}

void FrCardRemove::OnDrawAreaOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device || !m_pItemList)
		return;

	WRect pos = m_rect;

	DrawInDlg("card_remove_bg_1", 0, 0);
	DrawInDlg("card_remove_bg_2", 256, 0);
	DrawInDlg("card_remove_bg_3", 512, 0);
	DrawInDlg("card_remove_bg_4", 0, 256);
	DrawInDlg("card_remove_bg_5", 256, 256);
	DrawInDlg("card_remove_bg_6", 512, 256);

	FrListItem* selected = m_pItemList->GetSelected();
	if (!selected)
	{
		device->SetTextColor(0xffff0000, 0xffffffff);
		device->Print(WPoint(pos.x + 410, pos.y + 350), 1,
			"\xc4\xab\xb5\xe5\xb8\xa6\x20\xba\xd0\xb8\xae\xc7\xd2\x20\xc0\xc7\xbb\xf3\xc0\xbb\x20\xbc\xb1\xc5\xc3\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4\x2e");
		device->SetTextColor(0xffffffff, 0xffffffff);
		return;
	}

	sItemInfo* item = (sItemInfo*)selected->pData;
	if (!item)
		return;

	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->tid);
	if (!common)
		return;

	device->SetTextStyle(1);
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->guid))
		device->Print(WPoint(pos.x + 410, pos.y + 50), 1,
			GetUccItemName(item->guid));
	else
		device->Print(WPoint(pos.x + 410, pos.y + 50), 1, common->Name);

	device->SetTextStyle(0);

	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->guid))
		bitmap = GetUccItemIcon(item->guid);

	if (bitmap)
	{
		WRect rect(pos.x + 410 - bitmap->Width() * 0.5f, pos.y + 110,
			(float)bitmap->Width(), (float)bitmap->Height());
		device->DrawTexture(bitmap, rect, 0xffffffff, 0);
	}

	if (SetCurPartUid(item->guid) == true)
	{
		m_partTypeId = item->tid;
		BuildCurPartCardList();
	}

	if (!m_partUid)
		return;

	float index = 0;

	std::list<unsigned long>::iterator end = m_cardList.end();
	for (std::list<unsigned long>::iterator it = m_cardList.begin(); it != end;
		++it)
	{
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(*it);
		const Bitmap* bitmap = g_pFresh->GetBitmap(card->c.Icon);
		if (bitmap)
		{
			const LONG& width = bitmap->bi->bmiHeader.biWidth;
			WRect rect(410 -
					((float)m_cardList.size() * (float)(unsigned)width * 0.5f +
						index * 5) +
					((float)(unsigned)width + 5) * index + pos.x,
				pos.y + 215, (float)bitmap->Width(), (float)bitmap->Height());
			device->DrawTexture(bitmap, rect, 0xffffffff, 0);
			index += 1;
		}
	}

	device->SetTextColor(0xffff0000, 0xffffffff);
	device->Print(WPoint(pos.x + 410, pos.y + 350), 1,
		"\xbb\xf3\xb1\xe2\xc0\xc7\x20\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xb8\xb7\xce\x20\xba\xd0\xb8\xae\x20\xb5\xcb\xb4\xcf\xb4\xd9\x2e");
	device->SetTextColor(0xffffffff, 0xffffffff);
}

void FrCardRemove::OnItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, param);
	if (!m_pItemList)
		return;
	m_pItemList->UseDblClick(true);
	m_pItemList->UseRightButton(true);

	ITEM_LIST_IT partsEnd = Doc()->m_partsList.end();
	for (ITEM_LIST_IT it = Doc()->m_partsList.begin(); it != partsEnd; ++it)
	{
		sItemInfo& item = *it;
		if (CheckEquiped(item.guid))
			continue;
		for (std::list<sSCardAvilityPeriodInfo>::iterator card =
				 Doc()->m_cardAbilityList[0].begin();
			card != Doc()->m_cardAbilityList[0].end(); ++card)
		{
			if ((*card).partsUid == item.guid)
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(item.tid);
				if (part)
				{
					item.CharacterSlotNum = part->CharacterSlot;
					item.CaddieSlotNum = part->CaddieSlot;
					m_pItemList->AddItem(&item);
				}
				break;
			}
		}
	}

	for (ITEM_LIST_IT it = Doc()->m_uccItemList.begin();
		it != Doc()->m_uccItemList.end(); ++it)
	{
		sItemInfo& item = *it;
		if (CheckEquiped(item.guid))
			continue;
		for (std::list<sSCardAvilityPeriodInfo>::iterator card =
				 Doc()->m_cardAbilityList[0].begin();
			card != Doc()->m_cardAbilityList[0].end(); ++card)
		{
			if ((*card).partsUid == item.guid)
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(item.tid);
				if (part)
				{
					item.CharacterSlotNum = part->CharacterSlot;
					item.CaddieSlotNum = part->CaddieSlot;
					m_pItemList->AddItem(&item);
				}
				break;
			}
		}
	}
}

void FrCardRemove::OnItemListOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	FrListItem* listItem = (FrListItem*)param;
	if (!listItem)
		return;
	sItemInfo* item = (sItemInfo*)listItem->pData;
	if (!item)
		return;
	WPoint pos = listItem->pos;
	pos.y += 2;
	bool expired = item->ItemType == 6 && item->Expired;
	unsigned long color = expired ? 0x80808080 : 0xffffffff;

	if (!listItem->selected)
	{
		if (listItem->underCursor)
		{
			DrawTexture("button_o", pos.x, pos.y, color);
		}
		else
		{
			DrawTexture("button_n", pos.x, pos.y, color);
		}
	}
	else
	{
		DrawTexture("button_o", pos.x, pos.y, color);
	}

	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->tid);
	if (!common)
		return;
	unsigned long characterTid = ((item->tid >> 18) & 255) | 0x4000000;
	IFF_ITEM_COMMON* character = ItemManager()->FindCommonItem(characterTid);
	if (character)
	{
		const Bitmap* bitmap =
			g_pFresh->GetBitmap(MakeStr("s_%s", character->Icon));
		if (bitmap)
		{
			float height = bitmap->Height() * 0.01f,
				  width = bitmap->Width() * 0.01f;
			WRect rect(pos.x + 5, pos.y + 3, width * 90, height * 90);
			device->DrawTexture(bitmap, rect, 0xffffffff, 0);
		}
	}

	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->guid))
		bitmap = GetUccItemIcon(item->guid);
	if (bitmap)
	{
		float height = bitmap->Height() * 0.01f,
			  width = bitmap->Width() * 0.01f;
		WRect rect(pos.x + 30, pos.y + 1, width * 34, height * 34);
		device->DrawTexture(bitmap, rect, 0xffffffff, 0);
	}

	if (listItem->selected == true)
	{
		device->SetTextStyle(1);
		device->SetTextColor(0xffff0000, 0xffffffff);
	}
	else
	{
		device->SetTextStyle(0);
		device->SetTextColor(0xff000000, 0xffffffff);
	}

	if (!listItem->selected && listItem->underCursor)
		device->SetTextStyle(1);

	if (IsLocalContent(S4_UCC) && IsUccClothes(item->guid))
		device->Print(WPoint(pos.x + 61, pos.y + 10), 0,
			GetUccItemName(item->guid));
	else
		device->Print(WPoint(pos.x + 61, pos.y + 10), 0, common->Name);

	if (item->ItemType == 6)
	{
		const Bitmap* clock = g_pFresh->GetBitmap("clock");
		if (clock)
			device->DrawTexture(clock,
				WRect(listItem->pos.x + 198, listItem->pos.y + 6,
					clock->Width(), clock->Height()),
				0xffffffff, 0);
	}

	DrawPartsSlot(device, item, listItem);
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
}

void FrCardRemove::OnItemListDBClick()
{
}

void FrCardRemove::OnItemListRBDown()
{
	FrListItem* item = m_pItemList->GetItemUnderCursor();
	if (!item)
		return;
	if (!item->selected)
		m_pItemList->ToggleItem(item);
	CRealMyRoomMain* actor =
		(CRealMyRoomMain*)AfxGetTask()->GetActor("RealMyRoom");
	if (actor)
		actor->OpenInformation(item);
}

void FrCardRemove::OnConfirmInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button)
		button->SetPushDelay(0.1f);
}

void FrCardRemove::OnConfirmLBDown()
{
	if (m_partUid)
	{
		FrCardRemoveConfirm* form = CreateForm<FrCardRemoveConfirm>(
			g_pFresh->GetManager(), this, "card_remove_confirm", NULL);
		if (!form)
			return;
		form->SetCaption(
			"\xc4\xab\xb5\xe5\xc3\xdf\xc3\xe2\xbe\xd7\x20\xbb\xe7\xbf\xeb\xc8\xae\xc0\xce");
		form->SetPart(m_partTypeId, m_partUid);
		form->Open((FRESH_PFN_RESULT)&FrCardRemove::OnUseCardRemoveConfirm, 0);
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetTimeLimit(5);
		form->SetMessage(
			"\xb8\xd5\xc0\xfa\x20\xc4\xab\xb5\xe5\xb8\xa6\x20\xba\xd0\xb8\xae\xc7\xd2\x20\xc0\xc7\xbb\xf3\xc0\xbb\x20\xbc\xb1\xc5\xc3\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4\x2e",
			false);
		form->Open(NULL, 0);
	}
}

bool FrCardRemove::OnUseCardRemoveConfirm(int ret, FrForm* form)
{
	if (ret == FrOK)
	{
		if (IsCardRemoveItem(m_itemTypeId))
		{
			WSendPacket packet((enumClientPacket)226);
			packet.Encode4(m_itemTypeId);
			packet.Encode4(m_itemUid);
			packet.Encode4(m_partTypeId);
			packet.Encode4(m_partUid);
			packet.Send(TO_GAME);
		}
		Close(true);
	}

	return true;
}

void FrCardRemove::OnCancelInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button)
		button->SetPushDelay(0.1f);
}

void FrCardRemove::OnCancelLBDown()
{
	Close(true);
}

FrCardRemoveConfirm::FrCardRemoveConfirm()
	: m_partTypeId(0), m_partUid(0)
{
}

FrCardRemoveConfirm::~FrCardRemoveConfirm()
{
}

IMPLEMENT_OBJECT(FrCardRemoveConfirm, FrForm)
BEGIN_FRESH_MSGMAP(FrCardRemoveConfirm, FrForm)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW,
	FrCardRemoveConfirm::OnDrawAreaOwnerDraw)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrCardRemoveConfirm::OnConfirmInit)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN,
	FrCardRemoveConfirm::OnConfirmLBDown)
ON_FRESH_VI("btn_cancel", FRCMD_INIT, FrCardRemoveConfirm::OnCancelInit)
ON_FRESH_VV("btn_cancel", FRCMD_LBUTTONDOWN,
	FrCardRemoveConfirm::OnCancelLBDown)
END_FRESH_MSGMAP()

void FrCardRemoveConfirm::OnDrawAreaOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	WPoint point(m_rect.x, m_rect.y);
	point.x += m_rect.w * 0.5f;
	point.y += 55;
	device->PrintText(point, 1,
		"\xc4\xab\xb5\xe5\xc3\xdf\xc3\xe2\xbe\xd7\xc0\xbb\x20\xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
		NULL);
}

void FrCardRemoveConfirm::OnConfirmInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button)
		button->SetPushDelay(0.1f);
}

void FrCardRemoveConfirm::OnConfirmLBDown()
{
	if (m_partTypeId && m_partUid)
		Close(FrOK, true);
	else
		Close(true);
}

void FrCardRemoveConfirm::OnCancelInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button)
		button->SetPushDelay(0.1f);
}

void FrCardRemoveConfirm::OnCancelLBDown()
{
	Close(true);
}

IMPLEMENT_OBJECT(FrCardRemoveResult, FrForm)
BEGIN_FRESH_MSGMAP(FrCardRemoveResult, FrForm)
ON_FRESH_VI("draw_area", FRCMD_OWNERDRAW,
	FrCardRemoveResult::OnDrawAreaOwnerDraw)
ON_FRESH_VI("btn_confirm", FRCMD_INIT, FrCardRemoveResult::OnConfirmInit)
ON_FRESH_VV("btn_confirm", FRCMD_LBUTTONDOWN,
	FrCardRemoveResult::OnConfirmLBDown)
END_FRESH_MSGMAP()

FrCardRemoveResult::FrCardRemoveResult()
	: m_result(RESULT_ERROR),
	  m_pCover(NULL),
	  m_pCoverMask(NULL),
	  m_coverRect(0.f, 0.f, 0.f, 0.f),
	  m_maskRect(0.f, 0.f, 0.f, 0.f),
	  m_state(0),
	  m_remover(0)
{
}

FrCardRemoveResult::~FrCardRemoveResult()
{
}

bool FrCardRemoveResult::OnInit()
{
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(m_remover);

	if (common)
	{
		m_pCover =
			g_pFresh->GetBitmap(MakeStr("%s_for_resultdlg", common->Icon));
		m_pCoverMask = g_pFresh->GetBitmap("card_remover_covermask");
		if (m_pCover)
		{
			m_coverRect.w = m_pCover->Width();
			m_coverRect.h = m_pCover->Height();
		}
		if (m_pCoverMask)
		{
			m_maskRect.w = m_pCoverMask->Width();
			m_maskRect.h = 0;
		}
	}

	return true;
}

void FrCardRemoveResult::OnProc(const float dt)
{
	if (m_result == RESULT_ERROR || m_result == RESULT_ABNORMAL)
	{
		FrWnd* button = FindChildByName("btn_confirm");
		if (button && !button->IsVisible())
			button->SetVisible(true);
		return;
	}

	switch (m_state)
	{
	case 0:
	{
		m_state = 1;
		WRect rect = m_rect;
		SetRect(WRect(rect.x, rect.y, rect.w, rect.h + 50));
		break;
	}
	case 1:
		m_coverRect.x = m_rect.x + (m_rect.w - m_coverRect.w) * 0.5f;
		m_coverRect.y = m_rect.y + 30;
		m_maskRect.x = m_coverRect.x;
		m_maskRect.y = m_coverRect.y + 115;
		m_maskRect.h += dt * 20;
		if (m_maskRect.h > 40)
		{
			m_maskRect.h = 40;
			m_state = 2;
			WRect rect = m_rect;
			SetRect(WRect(rect.x, rect.y, rect.w, rect.h - 50));
		}
		break;
	case 2:
	{
		FrWnd* button = FindChildByName("btn_confirm");
		if (button && !button->IsVisible())
			button->SetVisible(true);
		break;
	}
	}
}

void FrCardRemoveResult::OnDrawAreaOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;

	WRect rect = m_rect;
	FrWnd* button = FindChildByName("btn_confirm");
	if (button)
		rect.h = button->GetRect().y - rect.y;

	if (m_result == RESULT_ERROR)
	{
		device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f), 1,
			"\xc0\xe1\xbd\xc3\x20\xc8\xc4\x20\xb4\xd9\xbd\xc3\x20\xbd\xc3\xb5\xb5\xc7\xd8\x20\xc1\xd6\xbc\xbc\xbf\xe4\x2e");
	}
	else if (m_result == RESULT_ABNORMAL)
	{
		device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + 120), 1,
			"\xc4\xab\xb5\xe5\x20\xb0\xfc\xb7\xc3\x20\xc0\xcc\xbb\xf3\x20\xc7\xf6\xbb\xf3\xc0\xcc\x20\xc8\xae\xc0\xce\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9\x2e");
		device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + 135), 1,
			"\xb0\xed\xb0\xb4\x20\xbc\xbe\xc5\xcd\xbf\xa1\x20\xb9\xae\xc0\xc7\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4\x2e");
	}
	else
	{
		if (m_state == 1)
		{
			device->DrawTexture(m_pCover, m_coverRect, 0xffffffff, 0);
			device->DrawTexture(m_pCoverMask, m_maskRect, 0xffffffff, 0);
			device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + 210), 1,
				"\xc4\xab\xb5\xe5\x20\xc3\xdf\xc3\xe2\xbe\xd7\xc0\xbb\x20\xbb\xe7\xbf\xeb\x20\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9\x2e");
		}
		if (m_state == 2)
		{
			if (m_result == RESULT_SUCCESS)
			{
				device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + 120), 1,
					"\xc4\xab\xb5\xe5\x20\xc3\xdf\xc3\xe2\xbf\xa1\x20\xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9\x21");
				device->Print(WPoint(rect.x + rect.w * 0.5f, rect.y + 135), 1,
					"\xc3\xdf\xc3\xe2\xb5\xc8\x20\xc4\xab\xb5\xe5\xb4\xc2\x20\xc4\xab\xb5\xe5\xba\xcf\xc0\xb8\xb7\xce\x20\xc0\xcc\xb5\xbf\x20\xb5\xcb\xb4\xcf\xb4\xd9\x2e");
				float index = 0;
				float centerX = rect.w * 0.5f;
				std::list<unsigned long>::iterator end = m_cardList.end();
				for (std::list<unsigned long>::iterator it = m_cardList.begin();
					it != end; ++it)
				{
					if (*it)
					{
						IFF_STRUCT::sCard* card = ItemManager()->FindCard(*it);
						const Bitmap* bitmap =
							g_pFresh->GetBitmap(card->c.Icon);
						if (bitmap)
						{
							const LONG& width = bitmap->bi->bmiHeader.biWidth;
							WRect dest(centerX -
									((float)m_cardList.size() *
											(float)(unsigned)width * 0.5f +
										index * 5) +
									((float)(unsigned)width + 5) * index +
									rect.x,
								rect.y + 40, (float)bitmap->Width(),
								(float)bitmap->Height());
							device->DrawTexture(bitmap, dest, 0xffffffff, 0);
							index += 1;
						}
					}
				}
			}
			else if (m_result == RESULT_FAIL)
			{
				device->Print(
					WPoint(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f), 1,
					"\xc4\xab\xb5\xe5\x20\xc3\xdf\xc3\xe2\xbf\xa1\x20\xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9\x21");
			}
		}
	}
}

void FrCardRemoveResult::OnConfirmInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button)
	{
		button->SetPushDelay(0.1f);
		button->SetVisible(false);
	}
}

void FrCardRemoveResult::OnConfirmLBDown()
{
	Close(true);
}
