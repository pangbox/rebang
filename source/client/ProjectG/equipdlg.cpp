#include "equipdlg.h"
#include "frdesktop.h"
#include "cardsystem.h"
#include "projectg.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrEquipDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrEquipDlg, FrForm)

ON_FRESH_VI("additem", FRCMD_INIT, FrEquipDlg::OnEquipSlotInit)
ON_FRESH_VV("additem", FRCMD_LBUTTONUP, FrEquipDlg::OnEquipSlotBtnUp)
ON_FRESH_VI("delitem", FRCMD_INIT, FrEquipDlg::OnUnequipSlotInit)
ON_FRESH_VV("delitem", FRCMD_LBUTTONUP, FrEquipDlg::OnUnequipSlotBtnUp)
ON_FRESH_VI("reset", FRCMD_INIT, FrEquipDlg::OnResetSlotInit)
ON_FRESH_VV("reset", FRCMD_LBUTTONUP, FrEquipDlg::OnResetSlotBtnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrEquipDlg::OnCloseInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrEquipDlg::OnCloseBtnUp)
ON_FRESH_VI("warehouse", FRCMD_INIT, FrEquipDlg::OnWarehouseInit)
ON_FRESH_VI("warehouse", FRCMD_OWNERDRAW, FrEquipDlg::OnWarehouseOwnerDraw)
ON_FRESH_VV("warehouse", FRCMD_LBUTTONDOWN, FrEquipDlg::OnWarehouseBtnDown)
ON_FRESH_VV("warehouse", FRCMD_RBUTTONDOWN, FrEquipDlg::OnWarehouseRBtnDown)
ON_FRESH_VV("warehouse", FRCMD_DBLCLICK, FrEquipDlg::OnWarehouseDblClick)
ON_FRESH_VI("itemslot", FRCMD_INIT, FrEquipDlg::OnItemSlotInit)
ON_FRESH_VI("itemslot", FRCMD_OWNERDRAW, FrEquipDlg::OnItemSlotOwnerDraw)
ON_FRESH_VV("itemslot", FRCMD_DBLCLICK, FrEquipDlg::OnItemSlotDblClick)

END_FRESH_MSGMAP()

FrEquipDlg::FrEquipDlg()
{
	m_pWarehouse = NULL;
	m_pItemSlot = NULL;
	memset(m_mySlot, 0, sizeof(m_mySlot));
	m_pWarehouseBtn[0] = g_pFresh->GetBitmap("equipitem_warehouse_bn_normal");
	m_pWarehouseBtn[1] = g_pFresh->GetBitmap("equipitem_warehouse_bn_over");
	m_pWarehouseBtn[2] = g_pFresh->GetBitmap("equipitem_warehouse_bn_click");
	m_pItemSelectBtn[0] = g_pFresh->GetBitmap("equipitem_itemselect_bn_over");
	m_pItemSelectBtn[1] = g_pFresh->GetBitmap("equipitem_itemselect_bn_click");
	static char* s_fontFile[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_fontFile;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

FrEquipDlg::~FrEquipDlg()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrEquipDlg::Init(unsigned long* slot, bool bSubtract)
{
	if (!m_pWarehouse || !m_pItemSlot)
		return;
	memcpy(m_mySlot, slot, 10 * sizeof(unsigned long));
	m_itemCount.clear();
	if (!bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		std::list<sItemInfo>::iterator it;
		for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
			++it)
		{
			sItemInfo* info = &*it;
			if (ItemManager()->IsNonVisibleItem(info->tid) != true &&
				!ItemManager()->GetCouponKind(info->tid))
			{
				if (m_itemCount.find(info->tid) == m_itemCount.end())
				{
					m_itemCount[info->tid] = info->Common[0];
					m_pWarehouse->AddItem(info);
				}
				else
					m_itemCount[info->tid] += info->Common[0];
			}
		}
		if (bSubtract)
		{
			for (int i = 0; i < 10; ++i)
			{
				FrListBox::ITEM_LIST::iterator item;
				for (item = m_pWarehouse->m_itemList.begin();
					item != m_pWarehouse->m_itemList.end(); ++item)
				{
					sItemInfo* info = (sItemInfo*)(*item)->pData;
					if (info->tid == m_mySlot[i])
					{
						if (--m_itemCount[info->tid] <= 0)
						{
							std::map<unsigned long, int>::iterator found =
								m_itemCount.find(info->tid);
							m_itemCount.erase(found);
							m_pWarehouse->_DelItem(item);
						}
						break;
					}
				}
			}
		}
	}
	if (IsLocalContent(S4_NT_SC_ITEMSLOT))
	{
		int slots = Doc()->GetNumAddItemSlotByMascot() + 8;
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			int cards = CCardManager::Instance()->GetCardPeriodSlot();
			if (cards > 0)
				slots += cards;
		}
		for (int i = 0; i < 10; ++i)
		{
			FrListItem* item = m_pItemSlot->AddItem(&m_mySlot[i]);
			if (i >= slots)
				UnequipItem(item);
		}
	}
	else
	{
		int extra = !IsLocalContent(S4_CARD_SYSTEM);
		for (int i = 0; i < 10 + extra; ++i)
			m_pItemSlot->AddItem(&m_mySlot[i]);
	}
}

void FrEquipDlg::OnEquipSlotInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (button)
	{
		if (bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			button->Enable(false);
		else
			button->SetPushDelay(0);
	}
}

void FrEquipDlg::OnEquipSlotBtnUp()
{
	if (m_pWarehouse->GetSelected())
		EquipItem(m_pWarehouse->GetSelected());
}

void FrEquipDlg::OnUnequipSlotInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (button)
	{
		if (bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			button->Enable(false);
		else
			button->SetPushDelay(0);
	}
}

void FrEquipDlg::OnUnequipSlotBtnUp()
{
	FrListItem* item = m_pItemSlot->GetSelected();
	if (item)
	{
		FrListBox::ITEM_LIST::iterator it;
		for (it = m_pItemSlot->m_itemList.begin();
			it != m_pItemSlot->m_itemList.end(); ++it)
			if (*it == item)
				break;
		for (++it; it != m_pItemSlot->m_itemList.end(); ++it)
		{
			if ((*it)->pData)
			{
				m_pItemSlot->SelectItem(*it, true);
				break;
			}
		}
		UnequipItem(item);
	}
}

void FrEquipDlg::OnResetSlotInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (button)
	{
		if (bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			button->Enable(false);
		else
			button->SetPushDelay(0);
	}
}

void FrEquipDlg::OnResetSlotBtnUp()
{
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_pItemSlot->m_itemList.begin();
		it != m_pItemSlot->m_itemList.end(); ++it)
		UnequipItem(*it);
}

void FrEquipDlg::OnCloseInit(int param)
{
}

void FrEquipDlg::OnCloseBtnUp()
{
	Close((eFormRet)(!(int)(Doc()->m_myInfo.info.IsIdentity(2) && true)), true);
}

void FrEquipDlg::OnWarehouseInit(int param)
{
	m_pWarehouse = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pWarehouse->UseRightButton(true);
	m_clockIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_clockIcon.rect = WRect(50, 35, m_clockIcon.pBitmap->Width(),
		m_clockIcon.pBitmap->Height());
	m_deleteIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_deleteIcon.rect = WRect(50, 35, m_deleteIcon.pBitmap->Width(),
		m_deleteIcon.pBitmap->Height());
}

void FrEquipDlg::OnWarehouseOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	sItemInfo* info = (sItemInfo*)item->pData;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* base = item->selected ? m_pWarehouseBtn[2]
		: item->underCursor             ? m_pWarehouseBtn[1]
										: m_pWarehouseBtn[0];
	if (base)
	{
		WRect dest;
		dest.x = item->pos.x;
		dest.y = item->pos.y;
		dest.w = base->Width();
		dest.h = base->Height();
		gdi->DrawTexture(base, dest, 0xffffffff, 0);
	}
	unsigned long& type = info->tid;
	if (!type)
		return;
	IFF_STRUCT::sItem* data = ItemManager()->FindItem(type);
	if (!data)
		return;
	const Bitmap* icon =
		g_pFresh->GetManager()->GetBitmap("ITEMS", data->c.Icon);
	if (icon)
	{
		WRect dest(item->pos.x + 18, item->pos.y + 11, (float)icon->Width(),
			(float)icon->Height());
		gdi->DrawTexture(icon, dest, 0xffffffff, 0);
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(4);
		if (info->ItemType)
		{
			if (!info->Expired)
				gdi->DrawTexture(m_clockIcon.pBitmap,
					m_clockIcon.rect + item->pos + WPoint(0, 0), 0xffffffff, 0);
		}
		else
		{
			const char* count;
			if (type & 0x02000000)
			{
				if (info->Common[0] <= 0)
					return;
				count = MakeStr("%d", info->Common[0]);
			}
			else
				count = MakeStr("%d", m_itemCount[type]);
			if (count && ItemManager()->IsDisplayItemNumber(type))
			{
				unsigned int i = 0;
				float x = item->pos.x + 70;
				for (; i < strlen(count); ++i)
					x -= m_pFont->GetCharWidth(g_view, count[i] - '0');
				m_pFont->Print(g_view, x, item->pos.y + 45, count, 0,
					0xffffffff, 0);
			}
		}
	}
}

void FrEquipDlg::OnWarehouseBtnDown()
{
	// HACK
	if (0)
		OnItemSlotInit(0);
}

void FrEquipDlg::OnWarehouseRBtnDown()
{
	FrListItem* item = m_pWarehouse->GetItemUnderCursor();
	if (item)
		OpenInformation(((sItemInfo*)item->pData)->tid);
}

void FrEquipDlg::OnWarehouseDblClick()
{
	if (!bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		if (m_pWarehouse->GetItemUnderCursor())
		{
			EquipItem(m_pWarehouse->GetItemUnderCursor());
			FrListItem* selected = m_pWarehouse->GetSelected();
			if (selected)
				m_pWarehouse->UnselectItem(selected);
		}
	}
}

void FrEquipDlg::OnItemSlotInit(int param)
{
	m_pItemSlot = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrEquipDlg::OnItemSlotOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	unsigned long* type = (unsigned long*)item->pData;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (IsLocalContent(S3_MASCOT))
	{
		int slots = Doc()->GetNumAddItemSlotByMascot() + 8;
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			CCardManager::Instance()->SetPlayerIndex(0xff);
			CCardManager::Instance()->CalcCardPeriodAndStatus();
			int cards = CCardManager::Instance()->GetCardPeriodSlot();
			if (cards > 0)
				slots += cards;
			if (!Doc()->GetNumAddItemSlotByMascot() && !cards)
			{
				m_mySlot[8] = 0;
				m_mySlot[9] = 0;
			}
			else if (Doc()->GetNumAddItemSlotByMascot() <= 0 || cards <= 0)
				m_mySlot[9] = 0;
		}
		if (item->idx >= slots)
		{
			const Bitmap* bitmap =
				g_pFresh->GetBitmap("equipitem_itemslot_empty");
			WRect dest(item->pos.x + 5, item->pos.y + 5, (float)bitmap->Width(),
				(float)bitmap->Height());
			gdi->DrawTexture(bitmap, dest, 0xffffffff, 0);
			return;
		}
	}
	const Bitmap* base;
	if (item->selected)
		base = m_pItemSelectBtn[1];
	else if (item->underCursor)
		base = m_pItemSelectBtn[0];
	else
		base = NULL;
	if (base)
	{
		WRect dest;
		dest.x = item->pos.x - 9;
		dest.y = item->pos.y - 9;
		dest.w = base->Width();
		dest.h = base->Height();
		gdi->DrawTexture(base, dest, 0xffffffff, 0);
	}
	if (type && *type)
	{
		IFF_STRUCT::sItem* info = ItemManager()->FindItem(*type);
		if (info)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap(info->c.Icon);
			if (bitmap)
			{
				WRect dest(item->pos.x + 4, item->pos.y + 4,
					(float)bitmap->Width(), (float)bitmap->Height());
				gdi->DrawTexture(bitmap, dest, 0xffffffff, 0);
			}
		}
	}
}

void FrEquipDlg::OnItemSlotDblClick()
{
	if (!bool((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		FrListItem* item = m_pItemSlot->GetItemUnderCursor();
		if (item && item->pData && *(unsigned long*)item->pData)
			UnequipItem(item);
	}
}

unsigned long* FrEquipDlg::GetMyItemSlot()
{
	return m_mySlot;
}

void FrEquipDlg::EquipItem(FrListItem* item)
{
	sItemInfo* info = (sItemInfo*)item->pData;
	unsigned long& type = info->tid;
	if (info->tid & 0x02000000)
	{
		if (Doc()->m_gameMode == 1)
		{
			AfxGetTask()->GetActor("Family") << MsgObject(NULL, 35,
				(int)"\xc0\xe5\xc2\xf8\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xc3\xa4 \xc8\xbf\xb7\xc2\xc0\xbb \xb9\xdf\xc8\xd6\xc7\xcf\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
		}
		else
		{
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
				(int)"\xc0\xe5\xc2\xf8\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xc3\xa4 \xc8\xbf\xb7\xc2\xc0\xbb \xb9\xdf\xc8\xd6\xc7\xcf\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
		}
	}
	else
	{
		int slots = 10;
		if (IsLocalContent(S3_MASCOT))
			slots = Doc()->GetNumAddItemSlotByMascot() + 8;
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			CCardManager::Instance()->SetPlayerIndex(0xff);
			CCardManager::Instance()->CalcCardPeriodAndStatus();
			if (IsLocalContent(S4_NT_SC_ITEMSLOT))
			{
				int cards = CCardManager::Instance()->GetCardPeriodSlot();
				if (cards > 0)
					slots += cards;
			}
			else
				slots += CCardManager::Instance()->GetCardPeriodSlot();
		}
		int i;
		for (i = 0; i < slots; ++i)
			if (!m_mySlot[i])
				break;
		if (i == slots)
		{
			if (Doc()->m_gameMode == 1)
			{
				AfxGetTask()->GetActor("Family") << MsgObject(NULL, 35,
					(int)"\xb4\xf5 \xc0\xcc\xbb\xf3 \xc0\xe5\xc2\xf8\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
			}
			else
			{
				AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
					(int)"\xb4\xf5 \xc0\xcc\xbb\xf3 \xc0\xe5\xc2\xf8\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
			}
		}
		else
		{
			m_mySlot[i] = type;
			std::map<unsigned long, int>::iterator it = m_itemCount.find(type);
			if (it != m_itemCount.end())
			{
				if (--m_itemCount[type] <= 0)
				{
					m_itemCount.erase(it);
					m_pWarehouse->DelItem(item);
				}
			}
		}
	}
}

void FrEquipDlg::UnequipItem(FrListItem* item)
{
	unsigned long* type = (unsigned long*)item->pData;
	if (type && *type)
	{
		if (m_itemCount.find(*type) == m_itemCount.end())
		{
			m_itemCount[*type] = 1;
			std::list<sItemInfo>::iterator it;
			for (it = Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
				if (it->tid == *type)
				{
					m_pWarehouse->AddItem(&*it);
					break;
				}
		}
		else
			m_itemCount[*type] = m_itemCount[*type] + 1;
		*type = 0;
	}
	m_pItemSlot->UnselectItem(item);
}

void FrEquipDlg::OpenInformation(unsigned long typeId)
{
	FrEdit* name;
	FrForm* form;
	FrWnd* existing =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (!existing)
	{
		form = DYNAMIC_CAST(FrForm, existing);
		form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information",
			NULL);
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
		FrArea* portrait =
			DYNAMIC_CAST(FrArea, form->FindChildByName("portrait"));
		if (portrait)
		{
			if (common)
				portrait->SetBgImg(common->Icon);
			else
				portrait->SetBgImg("hide");
		}
		if (common)
		{
			name = DYNAMIC_CAST(FrEdit, form->FindChildByName("name"));
			FrArea* level =
				DYNAMIC_CAST(FrArea, form->FindChildByName("level"));
			if (name && level)
			{
				if (common->IsUnderLvl)
					name->AddText(
						MakeStr(
							"\xc0\xcc\xb8\xa7: %s\012\012\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf",
							common->Name),
						false, true);
				else
					name->AddText(
						MakeStr(
							"\xc0\xcc\xb8\xa7: %s\012\012\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3",
							common->Name),
						false, true);
				level->SetBgImg(MakeStr("level_%03d", common->Level + 1));
			}
			if (common)
			{
				std::list<sItemInfo>::iterator it =
					std::find(Doc()->m_myItemList.begin(),
						Doc()->m_myItemList.end(), typeId);
				if (it != Doc()->m_myItemList.end())
				{
					int days = it->HourRemain / 24;
					int hours = it->HourRemain % 24;
					unsigned char itemType = it->ItemType;
					switch (itemType)
					{
					case 1:
						name->AddText(
							MakeStr(
								"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3",
								days, hours),
							false, true);
						break;
					case 2:
					case 3:
					case 4:
					case 5:
					{
						if (it->Expired)
							name->AddText(
								"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3\\c0xff000000\\c\xc0\xcc \\c0xffff0000\\c\xb8\xb8\xb7\xe1\\c0xff000000\\c\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\012",
								false, true);
						else if (!days && !hours)
							name->AddText(
								"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: 1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8\012",
								false, true);
						else
							name->AddText(
								MakeStr(
									"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3\012",
									days, hours),
								false, true);
					}
					}
				}
			}
			IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(common->TypeId);
			if (desc)
				form->SetMessage(desc->Desc, false);
		}
		else
		{
			name = DYNAMIC_CAST(FrEdit, form->FindChildByName("name"));
			if (name)
				name->AddText(
					"\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\012\012\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed",
					false, true);
			form->SetMessage(
				"\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7",
				false);
		}
		form->Open(NULL, 3);
	}
}
