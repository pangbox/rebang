#include "privatetradedlg.h"
#include "binstr.h"
#include "frdesktop.h"
#include "frscrollbar.h"
#include "inputmanager.h"
#include "taskmanager.h"
#include "actor.h"

extern Fresh* g_pFresh;
extern CInputManager* g_input;

sWarpItem::sWarpItem(const sWarpItem* item)
	: typeId(item->typeId),
	  id(item->id),
	  pIcon(item->pIcon),
	  name(item->name),
	  iconRect(item->iconRect),
	  count(item->count)
{
}

sWarpItem::sWarpItem(const sItemInfo& item)
	: typeId(item.tid), id(item.guid), pIcon(NULL)
{
	CItemManager& manager = Doc()->m_itemManager;
	if (manager.IsDisplayItemNumber(typeId))
		count = item.Common[0];
	else
		count = 1;
	SetIcon(typeId);
}

sWarpItem::sWarpItem(const sCards& card)
	: typeId(card.tid), id(card.uid), pIcon(NULL), count(card.cardCount)
{
	SetIcon(typeId);
}

void sWarpItem::SetIcon(unsigned long typeId)
{
	CItemManager& manager = Doc()->m_itemManager;
	IFF_ITEM_COMMON* item = manager.FindCommonItem(typeId);
	if (item)
	{
		pIcon = g_pFresh->GetBitmap(item->Icon);
		name = item->Name;
		if (!pIcon)
		{
			pIcon = g_pFresh->GetBitmap("caddie_no");
			name = MakeStr("INVAID(%d)", typeId);
		}
	}
	if (pIcon)
	{
		iconRect.x = iconRect.y = 0;
		iconRect.w = pIcon->Width();
		iconRect.h = pIcon->Height();
	}
}

namespace
{
	template <class T>
	T* FindPrivateTradeChildForm(const char* name)
	{
		FrWnd* trade = g_pFresh->GetManager()->GetDesktop()->FindChildForm(
			"FrPrivateTrade");
		if (trade)
		{
			FrWnd* child = trade->FindChildForm(name);
			if (child)
				return DYNAMIC_CAST(T, child);
		}
		return NULL;
	}

	int GetCardsAttachedToPart(std::vector<sSCardAvilityPeriodInfo*>& cards,
		unsigned long typeId, unsigned long id)
	{
		if ((typeId >> 26) != PIG_PART)
			return 0;
		std::list<sSCardAvilityPeriodInfo>& list = Doc()->m_cardAbilityList[0];
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = list.begin();
			it != list.end(); ++it)
		{
			sSCardAvilityPeriodInfo* card = &*it;
			bool match = typeId == card->partsTid && id == card->partsUid;
			if (match)
				cards.push_back(&*it);
		}
		return !cards.empty();
	}

	unsigned long GetMyItemCount(unsigned long id, unsigned long typeId)
	{
		CItemManager& manager = Doc()->m_itemManager;
		bool display = manager.IsDisplayItemNumber(typeId);
		if (display)
		{
			switch (typeId >> 26)
			{
			case PIG_ITEM:
			{
				CSharedDoc* doc = Doc();
				std::list<sItemInfo>::iterator it =
					std::find_if(doc->m_myItemList.begin(),
						doc->m_myItemList.end(), _FindByGuid(id));
				if (it != doc->m_myItemList.end())
					return (*it).Common[0];
				break;
			}
			case PIG_AUXPART:
			{
				CSharedDoc* doc = Doc();
				std::list<sItemInfo>::iterator it =
					std::find_if(doc->m_auxPartList.begin(),
						doc->m_auxPartList.end(), _FindByGuid(id));
				if (it != doc->m_auxPartList.end())
					return (*it).Common[0];
				break;
			}
			case PIG_CARD:
			{
				CSharedDoc* doc = Doc();
				std::list<sCards>::iterator it =
					std::find_if(doc->m_cardList.begin(), doc->m_cardList.end(),
						_FindByUid(id));
				if (it != doc->m_cardList.end())
					return (*it).cardCount;
				break;
			}
			}
		}
		return 1;
	}

	std::string GetCardInfoText(unsigned long typeId)
	{
		std::string result;
		CItemManager& manager = Doc()->m_itemManager;
		IFF_ITEM_COMMON* item = manager.FindCommonItem(typeId);
		if (!item)
			result += "unknown card";
		else
			result += item->Name;
		CItemManager& descManager = Doc()->m_itemManager;
		IFF_STRUCT::sDesc* desc = descManager.FindDesc(typeId);
		if (desc)
		{
			result += " : ";
			std::string text(desc->Desc);
			if (false)
				std::_Iter_cat(text.begin());
			std::string::size_type pos = text.find('\n');
			if (pos != std::string::npos)
				result += std::string(text.begin(), text.begin() + pos);
			else
				result += text;
		}
		return result;
	}

	void DrawPartsCardSlot(_sPrivateTradeItemSingle* item, const WRect& rect,
		float alpha, int showEmpty)
	{
		if (!item)
			return;
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		CItemManager& manager = Doc()->m_itemManager;
		IFF_STRUCT::sPart* part = manager.FindPart(item->typeId);
		if (part)
		{
			float bottom = rect.y + rect.h;
			const Bitmap* bitmap = NULL;
			float offset = 5;
			for (int i = 0; i < part->CharacterSlot; ++i)
			{
				bool empty = true;
				for (int j = 0; j < 8; ++j)
				{
					if (item->card[j].slotNum == i + 1 &&
						item->card[j].cardType == 0)
					{
						empty = false;
						CItemManager& cardManager = Doc()->m_itemManager;
						IFF_STRUCT::sCard* cardInfo =
							cardManager.FindCard(item->card[j].typeId);
						if (cardInfo)
							bitmap = g_pFresh->GetBitmap(cardInfo->SlotImg);
						if (bitmap)
						{
							WRect dest(rect.x + offset,
								bottom - ((float)bitmap->Height() + 5) * 2,
								(float)bitmap->Width(),
								(float)bitmap->Height());
							gdi->DrawTexture(bitmap, dest,
								((unsigned char)alpha << 24) | 0xffffff, 0);
							offset += (float)bitmap->Width() + 3;
							break;
						}
					}
				}
				if (showEmpty && empty)
				{
					bitmap = g_pFresh->GetBitmap("card_character_empty");
					if (bitmap)
					{
						WRect dest(rect.x + offset,
							bottom - ((float)bitmap->Height() + 5) * 2,
							(float)bitmap->Width(), (float)bitmap->Height());
						gdi->DrawTexture(bitmap, dest,
							((unsigned char)alpha << 24) | 0xffffff, 0);
						offset += (float)bitmap->Width() + 3;
					}
				}
			}
			offset = 5;
			for (int i = 0; i < part->CaddieSlot; ++i)
			{
				bool empty = true;
				for (int j = 0; j < 8; ++j)
				{
					if (item->card[j].slotNum == i + 1 &&
						item->card[j].cardType == 1)
					{
						empty = false;
						CItemManager& cardManager = Doc()->m_itemManager;
						IFF_STRUCT::sCard* cardInfo =
							cardManager.FindCard(item->card[j].typeId);
						if (cardInfo)
							bitmap = g_pFresh->GetBitmap(cardInfo->SlotImg);
						if (bitmap)
						{
							WRect dest(rect.x + offset,
								bottom - ((float)bitmap->Height() + 5),
								(float)bitmap->Width(),
								(float)bitmap->Height());
							gdi->DrawTexture(bitmap, dest,
								((unsigned char)alpha << 24) | 0xffffff, 0);
							offset += (float)bitmap->Width() + 3;
							break;
						}
					}
				}
				if (showEmpty && empty)
				{
					bitmap = g_pFresh->GetBitmap("card_caddie_empty");
					if (bitmap)
					{
						WRect dest(rect.x + offset,
							bottom - ((float)bitmap->Height() + 5),
							(float)bitmap->Width(), (float)bitmap->Height());
						gdi->DrawTexture(bitmap, dest,
							((unsigned char)alpha << 24) | 0xffffff, 0);
						offset += (float)bitmap->Width() + 3;
					}
				}
			}
		}
	}

	void Formator(const sTradeDropItem& item, _sPrivateTradeItemSingle& result)
	{
		result.typeId = item.typeId;
		result.id = item.id;
		result.count = item.count;
		switch (result.typeId >> 26)
		{
		case PIG_PART:
		{
			int index = 0;
			std::vector<sSCardAvilityPeriodInfo*> cards;
			if (GetCardsAttachedToPart(cards, result.typeId, result.id))
			{
				for (std::vector<sSCardAvilityPeriodInfo*>::iterator it =
						 cards.begin();
					it != cards.end(); ++it)
				{
					if (index >= 8)
						break;
					sSCardAvilityPeriodInfo* card = *it;
					result.card[index].id = card->uid;
					result.card[index].typeId = card->tid;
					result.card[index].ability = card->Avility;
					result.card[index].abilityValue = card->AvilityValue;
					result.card[index].cardType = card->cardType;
					result.card[index].slotNum = card->slotNum;
					++index;
				}
			}
			break;
		}
		case PIG_CLUBSET:
		{
			CSharedDoc* doc = Doc();
			std::map<unsigned int, sItemInfo>::iterator it =
				doc->m_clubSetMap.find(result.id);
			if (it != doc->m_clubSetMap.end())
				for (int i = 0; i < 5; ++i)
					result.c[i] = (*it).second.Common[i];
			break;
		}
		}
	}

	void DrawMyPartsCardSlot(unsigned long typeId, unsigned long id,
		const WRect& rect, float alpha, int showEmpty)
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		CItemManager& manager = Doc()->m_itemManager;
		IFF_STRUCT::sPart* part = manager.FindPart(typeId);
		std::list<sItemInfo>& parts = Doc()->m_partsList;
		std::list<sItemInfo>::iterator item =
			std::find_if(parts.begin(), parts.end(), _FindItem(typeId, id));
		if (part && item != parts.end())
		{
			std::vector<sSCardAvilityPeriodInfo*> cards;
			GetCardsAttachedToPart(cards, typeId, id);
			const Bitmap* bitmap = NULL;
			float bottom = rect.y + rect.h;
			float offset = 5;
			for (int i = 0; i < part->CharacterSlot; ++i)
			{
				bool empty = true;
				for (std::vector<sSCardAvilityPeriodInfo*>::iterator it =
						 cards.begin();
					it != cards.end(); ++it)
				{
					sSCardAvilityPeriodInfo* card = *it;
					if (card->slotNum == i + 1 && card->cardType == 0)
					{
						empty = false;
						CItemManager& cardManager = Doc()->m_itemManager;
						IFF_STRUCT::sCard* cardInfo =
							cardManager.FindCard(card->tid);
						if (cardInfo)
							bitmap = g_pFresh->GetBitmap(cardInfo->SlotImg);
						if (bitmap)
						{
							WRect dest(rect.x + offset,
								bottom - ((float)bitmap->Height() + 5) * 2,
								(float)bitmap->Width(),
								(float)bitmap->Height());
							gdi->DrawTexture(bitmap, dest,
								((unsigned char)alpha << 24) | 0xffffff, 0);
							offset += (float)bitmap->Width() + 3;
							break;
						}
					}
				}
				if (showEmpty && empty)
				{
					bitmap = g_pFresh->GetBitmap("card_character_empty");
					if (bitmap)
					{
						WRect dest(rect.x + offset,
							bottom - ((float)bitmap->Height() + 5) * 2,
							(float)bitmap->Width(), (float)bitmap->Height());
						gdi->DrawTexture(bitmap, dest,
							((unsigned char)alpha << 24) | 0xffffff, 0);
						offset += (float)bitmap->Width() + 3;
					}
				}
			}
			offset = 5;
			for (int i = 0; i < part->CaddieSlot; ++i)
			{
				bool empty = true;
				for (std::vector<sSCardAvilityPeriodInfo*>::iterator it =
						 cards.begin();
					it != cards.end(); ++it)
				{
					sSCardAvilityPeriodInfo* card = *it;
					if (card->slotNum == i + 1 && card->cardType == 1)
					{
						empty = false;
						CItemManager& cardManager = Doc()->m_itemManager;
						IFF_STRUCT::sCard* cardInfo =
							cardManager.FindCard(card->tid);
						if (cardInfo)
							bitmap = g_pFresh->GetBitmap(cardInfo->SlotImg);
						if (bitmap)
						{
							WRect dest(rect.x + offset,
								bottom - ((float)bitmap->Height() + 5),
								(float)bitmap->Width(),
								(float)bitmap->Height());
							gdi->DrawTexture(bitmap, dest,
								((unsigned char)alpha << 24) | 0xffffff, 0);
							offset += (float)bitmap->Width() + 3;
							break;
						}
					}
				}
				if (showEmpty && empty)
				{
					bitmap = g_pFresh->GetBitmap("card_caddie_empty");
					if (bitmap)
					{
						WRect dest(rect.x + offset,
							bottom - ((float)bitmap->Height() + 5),
							(float)bitmap->Width(), (float)bitmap->Height());
						gdi->DrawTexture(bitmap, dest,
							((unsigned char)alpha << 24) | 0xffffff, 0);
						offset += (float)bitmap->Width() + 3;
					}
				}
			}
		}
	}

	template <class T>
	struct less_gana_ptr
	{
		bool operator()(const T a, const T b) const
		{
			CItemManager& manager = Doc()->m_itemManager;
			IFF_ITEM_COMMON* left = manager.FindCommonItem(a->typeId);
			CItemManager& manager2 = Doc()->m_itemManager;
			IFF_ITEM_COMMON* right = manager2.FindCommonItem(b->typeId);
			if (!left || !right)
				return false;
			std::string leftName(left->Name);
			std::string rightName(right->Name);
			return leftName < rightName;
		}
	};

	bool IsSalable(const sItemInfo& item)
	{
		CItemManager& manager = Doc()->m_itemManager;
		IFF_ITEM_COMMON* info = manager.FindCommonItem(item.tid);
		if (info)
			return info->IsSalable != 0;
		return false;
	}

	bool IsEquipParts(const sItemInfo& item)
	{
		for (std::map<unsigned int, sCharacterInfo>::iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			const sCharacterInfo& character = (*it).second;
			for (int i = 0; i < 24; ++i)
				if (character.tidParts[i] &&
					item.guid == character.ItemIdList[i])
					return true;
			for (int i = 0; i < 5; ++i)
				if (character.tidAuxParts[i] &&
					item.tid == character.tidAuxParts[i])
					return true;
		}
		return false;
	}

	void SetListParts(std::vector<sWarpItem*>& items)
	{
		const std::list<sItemInfo>& parts = Doc()->m_partsList;
		for (std::list<sItemInfo>::const_iterator it = parts.begin();
			it != parts.end(); ++it)
			if (!IsEquipParts(*it) && IsSalable(*it))
				items.push_back(new sWarpItem(*it));
		const std::list<sItemInfo>& auxParts = Doc()->m_auxPartList;
		for (std::list<sItemInfo>::const_iterator it = auxParts.begin();
			it != auxParts.end(); ++it)
			if (!IsEquipParts(*it) && IsSalable(*it))
				items.push_back(new sWarpItem(*it));
	}

	void SetListAztec(std::vector<sWarpItem*>& items)
	{
		unsigned long ball = Doc()->m_myInfo.userEquip.tidBall;
		const std::list<sItemInfo>& balls = Doc()->m_ballList;
		items.reserve(balls.size());
		for (std::list<sItemInfo>::const_iterator it = balls.begin();
			it != balls.end(); ++it)
			if ((*it).tid != ball && IsSalable(*it))
				items.push_back(new sWarpItem(*it));
	}

	void SetListClubSet(std::vector<sWarpItem*>& items)
	{
		unsigned long club = Doc()->m_myInfo.userEquip.guidClubSet;
		std::map<unsigned int, sItemInfo>& clubs = Doc()->m_clubSetMap;
		items.reserve(clubs.size());
		for (std::map<unsigned int, sItemInfo>::const_iterator it =
				 clubs.begin();
			it != clubs.end(); ++it)
			if ((*it).second.guid != club && IsSalable((*it).second))
				items.push_back(new sWarpItem((*it).second));
	}

	void SetListHItem(std::vector<sWarpItem*>& items)
	{
		const std::list<sItemInfo>& list = Doc()->m_myItemList;
		for (std::list<sItemInfo>::const_iterator it = list.begin();
			it != list.end(); ++it)
			if (((*it).tid & 0x02000000) && IsSalable(*it))
				items.push_back(new sWarpItem(*it));
	}

	void SetListUitem(std::vector<sWarpItem*>& items)
	{
		const std::list<sItemInfo>& list = Doc()->m_myItemList;
		for (std::list<sItemInfo>::const_iterator it = list.begin();
			it != list.end(); ++it)
			if (!((*it).tid & 0x02000000) && IsSalable(*it))
				items.push_back(new sWarpItem(*it));
	}

	void SetListCard(std::vector<sWarpItem*>& items)
	{
		std::list<sCards>& cards = Doc()->m_cardList;
		items.reserve(cards.size());
		for (std::list<sCards>::const_iterator it = cards.begin();
			it != cards.end(); ++it)
		{
			CItemManager& manager = Doc()->m_itemManager;
			IFF_ITEM_COMMON* info = manager.FindCommonItem((*it).tid);
			if (info && info->IsSalable)
				items.push_back(new sWarpItem(*it));
		}
	}

	void DoSortItem(std::vector<sWarpItem*>& items, eSetItemButton button,
		int sort)
	{
		switch (button)
		{
		case SIB_PART:
			for (std::vector<sWarpItem*>::iterator it = items.begin();;)
			{
				if (it == items.end())
					break;
				sWarpItem* item = *it;

				if (((item->typeId >> 26) == PIG_PART ||
						(item->typeId >> 26) == PIG_AUXPART) &&
					((item->typeId >> 18) & 0xff) != sort)
				{
					delete *it;
					it = items.erase(it);
				}
				else
					++it;
			}
			{
				less_gana_ptr<sWarpItem*> compare;
				std::sort(items.begin(), items.end(), compare);
			}
			break;
		case SIB_CLUB:
		case SIB_CARD:
			switch (sort)
			{
			case 0:
			case 1:
			{
				less_guid_ptr<sWarpItem*> compare;
				std::sort(items.begin(), items.end(), compare);
			}
			break;
			case 2:
			{
				less_gana_ptr<sWarpItem*> compare;
				std::sort(items.begin(), items.end(), compare);
			}
			break;
			}
			break;
		case SIB_ITEM:
		{
			std::vector<sWarpItem*> normal;
			std::vector<sWarpItem*> consumable;
			for (std::vector<sWarpItem*>::iterator it = items.begin();
				it != items.end(); ++it)
			{
				if (((*it)->typeId >> 26) == PIG_ITEM)
				{
					sWarpItem* item = new sWarpItem(*it);
					if ((*it)->typeId & 0x02000000)
						consumable.push_back(item);
					else
						normal.push_back(item);
				}
			}
			std::for_each(items.begin(), items.end(), Delete_Object());
			items.clear();
			switch (sort)
			{
			case 0:
			{
				less_guid_ptr<sWarpItem*> compare;
				std::sort(normal.begin(), normal.end(), compare);
			}
				{
					less_guid_ptr<sWarpItem*> compare;
					std::sort(consumable.begin(), consumable.end(), compare);
				}
				items.assign(normal.begin(), normal.end());
				std::copy(consumable.begin(), consumable.end(),
					std::back_inserter(items));
				break;
			case 3:
			{
				less_guid_ptr<sWarpItem*> compare;
				std::sort(normal.begin(), normal.end(), compare);
			}
				items.assign(normal.begin(), normal.end());
				std::for_each(consumable.begin(), consumable.end(),
					Delete_Object());
				consumable.clear();
				break;
			case 4:
			{
				less_gana_ptr<sWarpItem*> compare;
				std::sort(normal.begin(), normal.end(), compare);
			}
				items.assign(normal.begin(), normal.end());
				std::for_each(consumable.begin(), consumable.end(),
					Delete_Object());
				consumable.clear();
				break;
			case 5:
			{
				less_guid_ptr<sWarpItem*> compare;
				std::sort(consumable.begin(), consumable.end(), compare);
			}
				items.assign(consumable.begin(), consumable.end());
				std::for_each(normal.begin(), normal.end(), Delete_Object());
				normal.clear();
				break;
			case 6:
			{
				less_gana_ptr<sWarpItem*> compare;
				std::sort(consumable.begin(), consumable.end(), compare);
			}
				items.assign(consumable.begin(), consumable.end());
				std::for_each(normal.begin(), normal.end(), Delete_Object());
				normal.clear();
				break;
			}
		}
		break;
		}
	}

	template <class T>
	void GetIconNameDesc(const Bitmap*& icon, std::string& name,
		std::string& desc, const T* item)
	{
		if (item)
		{
			// HACK!
			if (false)
				name.end() - name.begin();
			icon = g_pFresh->GetBitmap(item->c.Icon);
			name = item->c.Name;
			CItemManager& manager = Doc()->m_itemManager;
			IFF_STRUCT::sDesc* info = manager.FindDesc(item->c.TypeId);
			if (info)
				desc = info->Desc;
		}
	}

	void GetAblilityText(std::vector<std::string>& text, short* base,
		short* slot)
	{
		const char* labels[] = { "\xc6\xc4\xbf\xf6", "\xc4\xc1\xc6\xae\xb7\xd1",
			"\xc1\xa4\xc8\xae\xb5\xb5", "\xbd\xba\xc7\xc9",
			"\xc4\xbf\xba\xea" };
		for (int i = 0; i < 5; ++i)
		{
			std::string line;
			if (base[i])
				line = MakeStr("%s +%d", labels[i], base[i]);
			if (slot[i])
			{
				if (line.empty())
					line =
						MakeStr("%s \xbd\xbd\xb7\xd4 +%d", labels[i], slot[i]);
				else
					line += MakeStr(" / %s \xbd\xbd\xb7\xd4 +%d", labels[i],
						slot[i]);
			}
			if (!line.empty())
				text.push_back(line);
		}
	}
}

CTradeDragDrop* CTradeDragDrop::m_pCaller = NULL;
sTradeDropItem* CTradeDragDrop::m_pItem = NULL;

CTradeDragDrop::CTradeDragDrop()
{
	if (!m_pItem)
		m_pItem = new sTradeDropItem;
}

CTradeDragDrop::~CTradeDragDrop()
{
	if (m_pItem)
	{
		delete m_pItem;
		m_pItem = NULL;
	}
}

const sTradeDropItem* CTradeDragDrop::GetDropItem()
{
	if (IsPoped())
		return m_pItem;
	return NULL;
}

bool CTradeDragDrop::IsPoped()
{
	return m_pCaller && m_pItem->typeId && m_pItem->id;
}

void CTradeDragDrop::Cancel()
{
	m_pCaller = NULL;
}

bool CTradeDragDrop::Pop()
{
	m_pCaller = this;
	return true;
}

void CTradeDragDrop::SetPopItem(const sTradeDropItem& item)
{
	*m_pItem = item;
}

void CTradeDragDrop::Drop(CTradeDragDrop* target)
{
	if (m_pCaller && target && m_pCaller != target)
	{
		if (target->DropItemDown())
			m_pCaller->DropItemMoved();
	}
	m_pCaller = NULL;
	memset(m_pItem, 0, sizeof(sTradeDropItem));
}

IMPLEMENT_OBJECT(FrNumEdit, FrForm)
BEGIN_FRESH_MSGMAP(FrNumEdit, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrNumEdit::OnViewDraw)
ON_FRESH_VI("NumEdit", FRCMD_INIT, FrNumEdit::OnEditInit)
ON_FRESH_BI("NumEdit", FRCMD_ENTERKEY, FrNumEdit::OnEditEnter)
END_FRESH_MSGMAP()

FrNumEdit::FrNumEdit()
	: m_pEdit(NULL), m_value(0), m_pReturnParam(NULL), m_min(0), m_max(INT_MAX)
{
}

FrNumEdit::~FrNumEdit()
{
}

void FrNumEdit::SetValue(unsigned long value)
{
	SAFE_RANGEVALUE(m_value, m_min, m_max, value);
	char text[32] = "";
	m_pEdit->SetLine(1, itoa(m_value, text, 10), 0, false, 0);
}

void FrNumEdit::SetEditRange(unsigned long min, unsigned long max)
{
	char text[32] = "";
	itoa(max, text, 10);
	m_pEdit->SetCharLimit(strlen(text) + 1, false);
	m_max = max;
	m_min = min;
}

void FrNumEdit::OnProc(const float delta)
{
	if (m_pEdit->m_nFlags.GetFlag(FWF_KEYFOCUS))
	{
		unsigned long value = 0;
		const char* text = m_pEdit->GetLine(1, false);
		std::string numbers;
		for (unsigned int i = 0; i < strlen(text); ++i)
		{
			if (Custom_IsLeadByte(text[i]))
				continue;
			const char* digits = "0123456789";
			for (unsigned int j = 0; j < strlen(digits); ++j)
			{
				if (text[i] == digits[j])
				{
					numbers += text[i];
					break;
				}
			}
		}
		if (numbers.length() > 0)
		{
			value = atoi(numbers.c_str());
			if (value < m_min)
				value = m_min;
			if (value > m_max)
				value = m_max;
		}
		char buffer[32] = "";
		itoa(value, buffer, 10);
		m_pEdit->ForcedEdit(buffer, 0);
	}
}

void FrNumEdit::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap = g_pFresh->GetBitmap("cfrm00");
	gdi->DrawTexture(bitmap, WRect(0, 0, bitmap->Width(), bitmap->Height()),
		GetRect(), 0xffffffff, 0);
}

void FrNumEdit::OnEditInit(int param)
{
	m_pEdit = DYNAMIC_CAST(FrEdit, param);
	char text[32] = "";
	itoa(INT_MAX, text, 10);
	m_pEdit->SetCharLimit(strlen(text) + 1, false);
	m_pEdit->SetWidthLimit(m_pEdit->GetRect().w);
	if (m_value != 0)
	{
		char buffer[32] = "";
		m_pEdit->SetLine(1, itoa(m_value, buffer, 10), 0, false, 0);
	}
}

bool FrNumEdit::OnEditEnter(int param)
{
	unsigned long value = 0;
	if (param && strlen((const char*)param) > 0)
		value = atoi((const char*)param);
	SAFE_RANGEVALUE(m_value, m_min, m_max, value);
	return FrForm::Close(FrOK, true);
}

IMPLEMENT_OBJECT(FrPangEdit, FrForm)
BEGIN_FRESH_MSGMAP(FrPangEdit, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrPangEdit::OnViewDraw)
ON_FRESH_VI("EditPang", FRCMD_INIT, FrPangEdit::OnEditInit)
ON_FRESH_BI("EditPang", FRCMD_ENTERKEY, FrPangEdit::OnEditEnter)
END_FRESH_MSGMAP()

FrPangEdit::FrPangEdit()
	: m_pEdit(NULL), m_pang(0), m_texRate(0)
{
}

FrPangEdit::~FrPangEdit()
{
}

void FrPangEdit::SetPangValue(__int64 pang)
{
	m_pang = pang;
	if (m_pang != 0)
	{
		char text[32] = "";
		m_pEdit->SetLine(1, _i64toa(m_pang, text, 10), 0, false, 0);
	}
}

void FrPangEdit::OnProc(const float delta)
{
	if (m_pEdit->m_nFlags.GetFlag(FWF_KEYFOCUS))
	{
		const char* text = m_pEdit->GetLine(1, false);
		std::string numbers;
		bool changed = false;
		for (unsigned int i = 0; i < strlen(text); ++i)
		{
			if (Custom_IsLeadByte(text[i]))
				continue;
			const char* digits = "0123456789";
			unsigned int j;
			for (j = 0; j < strlen(digits); ++j)
			{
				if (text[i] == digits[j])
				{
					numbers += text[i];
					break;
				}
			}
			if (j == strlen(digits))
				changed = true;
		}
		if (changed)
			m_pEdit->ForcedEdit(numbers.c_str(), 0);
		if (numbers.length() > 0)
		{
			__int64 pang = _atoi64(numbers.c_str());
			if (pang <= 0)
				return;
			__int64 total = pang + pang * m_texRate / 100;
			if (Doc()->m_myInfo.stat.i64Pang <= total)
			{
				pang = Doc()->m_myInfo.stat.i64Pang * 100 / (100 + m_texRate);
				char buffer[32] = "";
				_i64toa(pang, buffer, 10);
				m_pEdit->ForcedEdit(buffer, 0);
			}
		}
	}
}

void FrPangEdit::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap = g_pFresh->GetBitmap("cfrm00");
	WRect source(0, 0, bitmap->Width(), bitmap->Height());
	gdi->DrawTexture(bitmap, source, GetRect(), 0xffffffff, 0);
	bitmap = g_pFresh->GetBitmap("pang_bar_pang");
	if (bitmap)
	{
		WRect rect(GetRect().x + 10, GetRect().y + 5, bitmap->Width(),
			bitmap->Height());
		gdi->DrawTexture(bitmap, rect, 0xffffffff, 0);
	}
}

void FrPangEdit::OnEditInit(int param)
{
	m_pEdit = DYNAMIC_CAST(FrEdit, param);
	char text[32] = "";
	_i64toa(Doc()->m_myInfo.stat.i64Pang, text, 10);
	m_pEdit->SetCharLimit(strlen(text) + 1, false);
	m_pEdit->SetWidthLimit(m_pEdit->GetRect().w);
	if (m_pang != 0)
	{
		char buffer[32] = "";
		m_pEdit->SetLine(1, _i64toa(m_pang, buffer, 10), 0, false, 0);
	}
}

bool FrPangEdit::OnEditEnter(int param)
{
	if (param && strlen((const char*)param) > 0)
		m_pang = _atoi64((const char*)param);
	else
		m_pang = 0;
	return FrForm::Close(FrOK, true);
}

EncapsulatedPang::EncapsulatedPang()
	: m_pang(0), m_pRect(NULL), m_style(STYLE_BAR | STYLE_ICON)
{
}

EncapsulatedPang::~EncapsulatedPang()
{
}

void EncapsulatedPang::SetRect(const WRect& rect)
{
	if (!m_pRect)
		m_pRect = new WRect(rect);
	else
		*m_pRect = rect;
}

int EncapsulatedPang::IsStyle(eStyle style)
{
	return m_style & style;
}

int EncapsulatedPang::SubPang(__int64 pang)
{
	if (m_pang - pang < 0)
		return 0;
	m_pang -= pang;
	return 1;
}

void EncapsulatedPang::AddPangBarStyle(eStyle style)
{
	m_style |= style;
}

void EncapsulatedPang::DrawPang()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi || !m_pRect)
		return;
	float width = m_pRect->w;
	float x = m_pRect->x;
	float y = m_pRect->y;
	unsigned long color = 0xffffffff;
	unsigned char alpha = 255;
	if (IsStyle(STYLE_HOVER) &&
		m_pRect->IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		alpha = 160;
		color = 0xa000ff00;
	}
	if (IsStyle(STYLE_BAR))
	{
		const Bitmap* left = g_pFresh->GetBitmap("pang_bar_01");
		const Bitmap* middle = g_pFresh->GetBitmap("pang_bar_02");
		const Bitmap* right = g_pFresh->GetBitmap("pang_bar_03");
		if (left && middle && right)
		{
			float leftWidth = (float)left->Width();
			float middleWidth = width - (float)right->Width() - leftWidth;
			float middleX = x + leftWidth;
			gdi->DrawTexture(left,
				WRect(x, y, (float)left->Width(), (float)left->Height()), color,
				0);
			gdi->DrawTexture(middle,
				WRect(middleX, y, middleWidth, (float)middle->Height()), color,
				0);
			gdi->DrawTexture(right,
				WRect(middleX + middleWidth, y, (float)right->Width(),
					(float)right->Height()),
				color, 0);
		}
	}
	if (IsStyle(STYLE_ICON))
	{
		const Bitmap* icon = g_pFresh->GetBitmap("pang_bar_pang");
		if (icon)
			gdi->DrawTexture(icon,
				WRect(x + 10, y + 5, (float)icon->Width(),
					(float)icon->Height()),
				color, 0);
	}
	x = x + m_pRect->w - 20;
	y = m_pRect->y + 5;
	char text[64] = "";
	MakePriceStrA(text, m_pang);
	x -= GetStringWidth(text);
	unsigned __int64 textColor = (unsigned char)(alpha * 255 >> 8);
	textColor = (textColor << 8) | (unsigned char)(alpha * 255 >> 8);
	textColor = (textColor << 8) | (unsigned char)(alpha * 209 >> 8);
	textColor = (textColor << 8) | (unsigned char)(alpha * 142 >> 8);
	m_pFont->Print(g_view, x, y, text, 0, (unsigned long)textColor, NULL);
}

IMPLEMENT_OBJECT(FrMyWareHouse, FrForm)
BEGIN_FRESH_MSGMAP(FrMyWareHouse, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrMyWareHouse::OnViewDraw)
ON_FRESH_VI("ItemList", FRCMD_INIT, FrMyWareHouse::OnListInit)
ON_FRESH_VI("ItemList", FRCMD_OWNERDRAW, FrMyWareHouse::OnListDraw)
ON_FRESH_VV("ItemList", FRCMD_DBLCLICK, FrMyWareHouse::OnListDblClick)
ON_FRESH_VI("SortItem", FRCMD_INIT, FrMyWareHouse::OnSortInit)
ON_FRESH_VI("SortItem", FRCMD_OWNERDRAW, FrMyWareHouse::OnSortDraw)
ON_FRESH_VI("SortItem", FRCMD_LBUTTONUP, FrMyWareHouse::OnSortDown)
ON_FRESH_VI("PartItem", FRCMD_INIT, FrMyWareHouse::OnPartBtnInit)
ON_FRESH_VI("ClubItem", FRCMD_INIT, FrMyWareHouse::OnClubBtnInit)
ON_FRESH_VI("WareItem", FRCMD_INIT, FrMyWareHouse::OnItemBtnInit)
ON_FRESH_VI("CardItem", FRCMD_INIT, FrMyWareHouse::OnCardBtnInit)
ON_FRESH_VV("PartItem", FRCMD_LBUTTONUP, FrMyWareHouse::OnPartBtnDown)
ON_FRESH_VV("ClubItem", FRCMD_LBUTTONUP, FrMyWareHouse::OnClubBtnDown)
ON_FRESH_VV("WareItem", FRCMD_LBUTTONUP, FrMyWareHouse::OnItemBtnDown)
ON_FRESH_VV("CardItem", FRCMD_LBUTTONUP, FrMyWareHouse::OnCardBtnDown)
END_FRESH_MSGMAP()

FrMyWareHouse::FrMyWareHouse()
	: m_pList(NULL), m_pSortList(NULL), m_curButton(SIB_PART)
{
	back_insert_null(m_buttons, 4);
}

FrMyWareHouse::~FrMyWareHouse()
{
	std::for_each(m_items.begin(), m_items.end(), Delete_Object());
}

bool FrMyWareHouse::OnInit()
{
	WRect rect = GetRect();
	rect.x += 15;
	rect.y += 80;
	rect.w -= 30;
	rect.h = 20;
	EncapsulatedPang::SetRect(rect);
	SetPang(Doc()->m_myInfo.stat.i64Pang);
	return FrForm::OnInit();
}

void FrMyWareHouse::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	WRect rect = m_pList->GetScrollBar()->GetRect();
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()) && IsPoped())
		Cancel();
}

bool FrMyWareHouse::DropItemUp()
{
	return true;
}

bool FrMyWareHouse::DropItemDown()
{
	const sTradeDropItem* item = GetDropItem();
	if (!item)
		return false;
	if (!item->typeId && !item->id)
		return false;
	switch (item->typeId >> 26)
	{
	case PIG_PART:
	case PIG_AUXPART:
		UnPushedButtonAll();
		m_buttons[SIB_PART]->SetStatus(FrButton::PRESSED);
		SetItemList(SIB_PART, (eSetSortType)((item->typeId >> 18) & 0xff));
		break;
	case PIG_CLUBSET:
		OnClubBtnDown();
		break;
	case PIG_ITEM:
		OnItemBtnDown();
		break;
	case PIG_CARD:
		OnCardBtnDown();
		break;
	default:
		return false;
	}
	std::vector<sWarpItem*>::iterator it =
		std::find_if(m_items.begin(), m_items.end(), _FindByGuid_Ptr(item->id));
	if (it != m_items.end())
	{
		FrListItem* listItem = FindListItem(item->id, item->typeId);
		if (listItem)
		{
			m_pList->DelItem(listItem);
			m_pList->AddItem(*it);
		}
		else
			m_pList->AddItem(*it);
		m_pList->GetScrollBar()->ScrollToBottom();
		return true;
	}
	return false;
}

void FrMyWareHouse::DropItemMoved()
{
	const sTradeDropItem* item = GetDropItem();
	CItemManager& manager = Doc()->m_itemManager;
	if (manager.IsDisplayItemNumber(item->typeId))
		SubListItemCount(item->id, item->typeId, 1);
	else
		EraseListItem(item->id, item->typeId);
}

FrListItem* FrMyWareHouse::FindListItem(unsigned long id, unsigned long typeId)
{
	int size = m_pList->GetCurrentItemSize(true);
	for (int i = 0; i < size; ++i)
	{
		FrListItem* item = m_pList->GetItem(i);
		if (item && item->pData)
		{
			sWarpItem* data = (sWarpItem*)item->pData;
			if (data->id == id && data->typeId == typeId)
				return item;
		}
	}
	return NULL;
}

bool FrMyWareHouse::SubListItemCount(unsigned long id, unsigned long typeId,
	int count)
{
	FrListItem* item = FindListItem(id, typeId);
	if (item)
	{
		sWarpItem* data = (sWarpItem*)item->pData;
		if (data->count - count >= 0)
		{
			data->count -= count;
			if (data->count == 0)
				m_pList->DelItem(item);
			return true;
		}
	}
	return false;
}

bool FrMyWareHouse::AddListItemCount(unsigned long id, unsigned long typeId,
	int count)
{
	FrListItem* item = FindListItem(id, typeId);
	if (item)
	{
		((sWarpItem*)item->pData)->count += count;
		return true;
	}
	return false;
}

int FrMyWareHouse::GetListItemCount(unsigned long id, unsigned long typeId)
{
	FrListItem* item = FindListItem(id, typeId);
	return item ? ((sWarpItem*)item->pData)->count : 0;
}

bool FrMyWareHouse::SetListItem(unsigned long id, unsigned long typeId,
	int count)
{
	FrListItem* item = FindListItem(id, typeId);
	if (item)
	{
		if (item->pData)
		{
			if (!count)
				EraseListItem(id, typeId);
			else
				((sWarpItem*)item->pData)->count = count;
			return true;
		}
	}
	else if (count > 0)
		return AddListItem(id, typeId, count);
	return false;
}

bool FrMyWareHouse::AddListItem(unsigned long id, unsigned long typeId,
	int count)
{
	if (FindListItem(id, typeId))
		return false;
	for (std::vector<sWarpItem*>::iterator it = m_items.begin();
		it != m_items.end(); ++it)
	{
		sWarpItem* item = *it;
		for (int i = 0; i < 4; ++i)
		{
			if (typeId == item->typeId && id == item->id)
			{
				item->count = count;
				m_pList->AddItem(*it);
				m_pList->GetScrollBar()->ScrollToBottom();
				break;
			}
		}
	}
	return true;
}

bool FrMyWareHouse::EraseListItem(unsigned long id, unsigned long typeId)
{
	FrListItem* item = FindListItem(id, typeId);
	if (item)
	{
		m_pList->DelItem(item);
		return true;
	}
	return false;
}

void FrMyWareHouse::InitSortList(eSetItemButton button)
{
	if (m_pSortList)
	{
		m_pSortList->ClearItem();
		switch (button)
		{
		case SIB_PART:
			for (int i = 0; i < 10; ++i)
			{
				CItemManager& manager = Doc()->m_itemManager;
				IFF_ITEM_COMMON* item =
					manager.FindCommonItem((PIG_CHAR << 26) | i);
				if (item)
					m_pSortList->AddItem((void*)g_pFresh->GetBitmap(
						MakeStr("s_%s", item->Icon)));
			}
			break;
		}
	}
}

void FrMyWareHouse::OnPartBtnInit(int param)
{
	SetButtonStyle(param, SIB_PART);
}

void FrMyWareHouse::OnClubBtnInit(int param)
{
	SetButtonStyle(param, SIB_CLUB);
}

void FrMyWareHouse::OnItemBtnInit(int param)
{
	SetButtonStyle(param, SIB_ITEM);
}

void FrMyWareHouse::OnCardBtnInit(int param)
{
	SetButtonStyle(param, SIB_CARD);
}

void FrMyWareHouse::OnSortInit(int param)
{
	m_pSortList = DYNAMIC_CAST(FrListBox, param);
	InitSortList(m_curButton);
}

void FrMyWareHouse::OnSortDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item && item->pData)
	{
		const Bitmap* bitmap = (const Bitmap*)item->pData;
		WPoint point(item->pos);
		WSize size(bitmap->Width(), bitmap->Height());
		WRect rect(point, size);
		g_pFresh->GetManager()->GetGDI()->DrawTexture(bitmap, rect, 0xffffffff,
			0);
	}
}

void FrMyWareHouse::OnSortDown(int param)
{
	FrListItem* item = m_pSortList->GetSelected();
	if (item && m_curButton == SIB_PART)
		SetItemList(SIB_PART, (eSetSortType)item->idx);
}

void FrMyWareHouse::OnPartBtnDown()
{
	UnPushedButtonAll();
	m_buttons[SIB_PART]->SetStatus(FrButton::PRESSED);
	SetItemList(SIB_PART, SST_ALL);
}

void FrMyWareHouse::OnClubBtnDown()
{
	UnPushedButtonAll();
	m_buttons[SIB_CLUB]->SetStatus(FrButton::PRESSED);
	SetItemList(SIB_CLUB, SST_ALL);
}

void FrMyWareHouse::OnItemBtnDown()
{
	UnPushedButtonAll();
	m_buttons[SIB_ITEM]->SetStatus(FrButton::PRESSED);
	SetItemList(SIB_ITEM, SST_ALL);
}

void FrMyWareHouse::OnCardBtnDown()
{
	UnPushedButtonAll();
	m_buttons[SIB_CARD]->SetStatus(FrButton::PRESSED);
	SetItemList(SIB_CARD, SST_ALL);
}

void FrMyWareHouse::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap = g_pFresh->GetBitmap("cfrm00");
	gdi->DrawTexture(bitmap, WRect(0, 0, bitmap->Width(), bitmap->Height()),
		GetRect(), 0xa0800000, 0);
	DrawPang();
}

void FrMyWareHouse::OnListInit(int param)
{
	m_pList = DYNAMIC_CAST(FrListBox, param);
	SetItemList(m_curButton, SST_ALL);
}

void FrMyWareHouse::OnListDblClick()
{
	FrListItem* item = m_pList->GetSelected();
	if (item)
	{
		sWarpItem* data = (sWarpItem*)item->pData;
		if (data)
		{
			FrPrivateTrade* trade = (FrPrivateTrade*)g_pFresh->GetManager()
										->GetDesktop()
										->FindChildForm("FrPrivateTrade");
			if (trade)
				trade->MoveItem("FrMyWareHouse", data);
		}
	}
}

void FrMyWareHouse::OnListDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item)
	{
		sWarpItem* data = (sWarpItem*)item->pData;
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		WPoint origin(item->pos);
		float scale;
		switch (data->typeId >> 26)
		{
		case PIG_CLUBSET:
			scale = .7f;
			break;
		case PIG_ITEM:
			scale = .8f;
			break;
		case PIG_CARD:
			scale = .6f;
			break;
		default:
			scale = .5f;
			break;
		}
		float width = data->iconRect.w * scale;
		WRect dest(WPoint(origin.x + 3, origin.y + 3),
			WSize(width, data->iconRect.h * scale));
		gdi->DrawTexture(data->pIcon, data->iconRect, dest, 0xffffffff, 0);
		if ((data->typeId >> 26) == PIG_PART)
		{
			dest.x += width;
			DrawMyPartsCardSlot(data->typeId, data->id, dest, 255, 0);
		}
		if ((data->typeId >> 26) == PIG_ITEM)
		{
			dest.x += width - 20;
			dest.y += 25;
			m_pFont->Print(g_view, dest.x, dest.y, MakeStr("%d", data->count),
				0, 0xffffffff, NULL);
		}
		if ((data->typeId >> 26) == PIG_CARD)
		{
			dest.x += width;
			dest.y += 25;
			m_pFont->Print(g_view, dest.x, dest.y, MakeStr("%d", data->count),
				0, 0xffffffff, NULL);
		}
		gdi->SetTextColor(0xffffffff, 0xff000000);
		gdi->SetTextStyle(2);
		gdi->Print11(WPoint(origin.x + 80, origin.y + 17), 0, "%s",
			data->name.c_str());
		gdi->Line(origin, WPoint(origin.x + 235, origin.y), 0xff000000,
			0xff000000, 0);
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
		WRect hit(origin,
			WSize(m_pList->GetItemWidth(), m_pList->GetItemHeight()));
		if (hit.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			if (g_input->GetButton(LEFT_BUTTON) == 1)
				CTradeDragDrop::SetPopItem(
					sTradeDropItem(data->id, data->typeId));
			gdi->LineBox(WRect(hit.x + 1, hit.y + 1, hit.w - 2, hit.h - 2),
				0xffffff00);
			FrTradeToolTip* tooltip =
				FindPrivateTradeChildForm<FrTradeToolTip>("FrTradeToolTip");
			if (tooltip)
			{
				_sPrivateTradeItemSingle tradeItem;
				memset(&tradeItem, 0, sizeof(tradeItem));
				Formator(sTradeDropItem(data->id, data->typeId), tradeItem);
				tooltip->SetToolTip(&tradeItem);
			}
		}
	}
}

void FrMyWareHouse::SetItemList(eSetItemButton button, eSetSortType sort)
{
	InitSortList(button);
	m_pList->ClearItem();
	std::for_each(m_items.begin(), m_items.end(), Delete_Object());
	m_items.clear();
	switch (button)
	{
	case SIB_PART:
		SetListParts(m_items);
		break;
	case SIB_CLUB:
		SetListClubSet(m_items);
		break;
	case SIB_ITEM:
		SetListHItem(m_items);
		SetListUitem(m_items);
		break;
	case SIB_CARD:
		SetListCard(m_items);
		break;
	}
	_sPrivateTradeItem tradeItems;
	memset(&tradeItems, 0, sizeof(tradeItems));
	FrPrivateTrade* trade =
		(FrPrivateTrade*)g_pFresh->GetManager()->GetDesktop()->FindChildForm(
			"FrPrivateTrade");
	if (trade)
		trade->GetMyTradeItems(tradeItems);
	DoSortItem(m_items, button, sort);
	for (std::vector<sWarpItem*>::iterator it = m_items.begin();
		it != m_items.end(); ++it)
	{
		sWarpItem* item = *it;
		bool found = false;
		for (int i = 0; i < 4; ++i)
			if (tradeItems.item[i].typeId == item->typeId &&
				tradeItems.item[i].id == item->id &&
				tradeItems.item[i].count == item->count)
			{
				found = true;
				break;
			}
		if (!found)
			m_pList->AddItem(item);
	}
	m_curButton = button;
	m_pList->GetScrollBar()->ScrollToFirst();
}

void FrMyWareHouse::SetButtonStyle(int param, eSetItemButton button)
{
	FrButton* wnd = DYNAMIC_CAST(FrButton, param);
	if (wnd)
	{
		wnd->SetPushDelay(0);
		wnd->SetStyle(FrButton::BT_3TYPES);
		m_buttons[button] = wnd;
	}
}

void FrMyWareHouse::UnPushedButtonAll()
{
	for (std::vector<FrButton*>::iterator it = m_buttons.begin();
		it != m_buttons.end(); ++it)
		(*it)->SetStatus(FrButton::NORMAL);
}

IMPLEMENT_OBJECT(FrTradeToolTip, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeToolTip, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrTradeToolTip::OnViewDraw)
ON_FRESH_VI("TextList", FRCMD_INIT, FrTradeToolTip::OnTextListInit)
ON_FRESH_VI("TextList", FRCMD_OWNERDRAW, FrTradeToolTip::OnTextListDraw)
END_FRESH_MSGMAP()

FrTradeToolTip::FrTradeToolTip()
	: m_alpha(0),
	  m_pTextList(NULL),
	  m_pIcon(NULL),
	  m_pItem(NULL),
	  m_dragDraw(0),
	  m_pDragItem(NULL)
{
	m_pItem = new _sPrivateTradeItemSingle;
	memset(m_pItem, 0, sizeof(*m_pItem));
}

FrTradeToolTip::~FrTradeToolTip()
{
	if (m_pItem)
	{
		delete m_pItem;
		m_pItem = NULL;
	}
}

void FrTradeToolTip::SetDragItem(const sTradeDropItem* item)
{
	m_pDragItem = item;
}

void FrTradeToolTip::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	if (CTradeDragDrop::IsPoped())
	{
		m_alpha = 255;
		return;
	}
	WRect rect(GetRect());
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		m_alpha = 255;
		m_pTextList->GetScrollBar()->SetVisible(true);
		return;
	}
	if (m_alpha > 0)
	{
		m_pTextList->GetScrollBar()->SetVisible(false);
		m_alpha -= delta * 120;
		if (m_alpha < 0)
			m_alpha = 0;
	}
}

void FrTradeToolTip::SetToolTip(_sPrivateTradeItemSingle* item)
{
	m_alpha = 255;
	m_pTextList->GetScrollBar()->SetVisible(true);
	if ((m_pItem->id != item->id || m_pItem->typeId != item->typeId) &&
		!CTradeDragDrop::IsPoped())
	{
		*m_pItem = *item;
		m_pTextList->ClearItem();
		m_pTextList->GetScrollBar()->ScrollToFirst();
		m_pIcon = NULL;
		m_text.clear();
		std::string name;
		std::string desc;
		std::string ability;
		switch (m_pItem->typeId >> 26)
		{
		case PIG_AUXPART:
		{
			CItemManager& manager = Doc()->m_itemManager;
			GetIconNameDesc(m_pIcon, name, desc,
				manager.FindAuxPart(m_pItem->typeId));
			break;
		}
		case PIG_PART:
		{
			CItemManager& manager = Doc()->m_itemManager;
			GetIconNameDesc(m_pIcon, name, desc,
				manager.FindPart(m_pItem->typeId));
			break;
		}
		case PIG_CLUBSET:
		{
			CItemManager& manager = Doc()->m_itemManager;
			GetIconNameDesc(m_pIcon, name, desc,
				manager.FindClubSet(m_pItem->typeId));
			break;
		}
		case PIG_ITEM:
		{
			CItemManager& manager = Doc()->m_itemManager;
			GetIconNameDesc(m_pIcon, name, desc,
				manager.FindItem(m_pItem->typeId));
			break;
		}
		case PIG_CARD:
		{
			CItemManager& manager = Doc()->m_itemManager;
			GetIconNameDesc(m_pIcon, name, desc,
				manager.FindCard(m_pItem->typeId));
			break;
		}
		}
		m_text.push_back(std::string(""));
		m_text.push_back(name);
		if ((m_pItem->typeId >> 26) == PIG_PART)
		{
			CItemManager& manager = Doc()->m_itemManager;
			IFF_STRUCT::sPart* part = manager.FindPart(m_pItem->typeId);
			if (part)
			{
				m_text.push_back(std::string(""));
				GetAblilityText(m_text, part->Attr, part->Slot);
			}
		}
		if ((m_pItem->typeId >> 26) == PIG_CLUBSET)
		{
			CItemManager& manager = Doc()->m_itemManager;
			IFF_STRUCT::sClubSet* club = manager.FindClubSet(m_pItem->typeId);
			if (club)
			{
				m_text.push_back(std::string(""));
				GetAblilityText(m_text, club->Attr, m_pItem->c);
			}
		}
		for (int i = 0; i < 8; ++i)
		{
			unsigned long typeId = m_pItem->card[i].typeId;
			if (typeId)
			{
				std::string line(" + ");
				line += GetCardInfoText(typeId);
				m_text.push_back(line.c_str());
			}
		}
		m_text.push_back(std::string(""));
		MakeFormatText(desc, m_text, 0);
		for (std::vector<std::string>::iterator it = m_text.begin();
			it != m_text.end(); ++it)
			m_pTextList->AddItem(&*it);
	}
}

void FrTradeToolTip::MakeFormatText(const std::string& text,
	std::vector<std::string>& lines, int width)
{
	std::string line;
	int i = 0;
	if (width)
		line.assign(width, ' ');
	int length = text.length();
	for (; i < length; ++i)
	{
		if (text[i] == '\n')
		{
			m_text.push_back(line);
			line.clear();
			if (width)
				line.assign(width, ' ');
		}
		else
		{
			if (Custom_IsLeadByte(text[i]))
			{
				line += text[i++];
			}
			line += text[i];
			if (line.length() > 40)
			{
				m_text.push_back(line);
				line.clear();
				if (text[i + 1] == ' ')
					line.assign(width + 2, ' ');
				else
					line.assign(width + 3, ' ');
			}
		}
	}
	if (!line.empty())
		m_text.push_back(line);
}

void FrTradeToolTip::OnTextListInit(int param)
{
	m_pTextList = DYNAMIC_CAST(FrListBox, param);
}

void FrTradeToolTip::OnTextListDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item)
	{
		std::string* text = (std::string*)item->pData;
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		gdi->SetTextColor(((unsigned char)m_alpha << 24) | 0xffffff,
			0xffffffff);
		WPoint point(item->pos.x, item->pos.y + 2);
		gdi->PrintText11(point, 0, text->c_str(), NULL);
		gdi->SetTextColor(0xff000000, 0xffffffff);
	}
}

void FrTradeToolTip::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* background = g_pFresh->GetBitmap("cfrm00");
	WRect source(0, 0, (float)background->Width(), (float)background->Height());
	gdi->DrawTexture(background, source, GetRect(), 0xc8282828, 0);
	if (m_pIcon)
	{
		WRect dest(GetRect().x + 10,
			GetRect().y + (GetRect().h - (float)m_pIcon->Height()) * .5f,
			(float)m_pIcon->Width(), (float)m_pIcon->Height());
		gdi->DrawTexture(m_pIcon, dest,
			((unsigned char)m_alpha << 24) | 0xffffff, 0);
		if ((m_pItem->typeId >> 26) == PIG_PART)
		{
			CItemManager& manager = Doc()->m_itemManager;
			IFF_ITEM_COMMON* character = manager.FindCommonItem(
				((m_pItem->typeId >> 18) & 0xff) | 0x04000000);
			if (character)
			{
				const Bitmap* bitmap =
					g_pFresh->GetBitmap(MakeStr("bar_%s", character->Icon));
				if (bitmap)
				{
					WRect rect(GetRect().x + 5, GetRect().y + 5,
						(float)bitmap->Width(), (float)bitmap->Height());
					gdi->DrawTexture(bitmap, rect,
						((unsigned char)m_alpha << 24) | 0xffffff, 0);
				}
			}
			DrawPartsCardSlot(m_pItem, GetRect(), m_alpha, 1);
		}
	}
	if (m_dragDraw)
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		unsigned long typeId = m_pDragItem ? m_pDragItem->typeId : 0;
		CItemManager& manager = Doc()->m_itemManager;
		IFF_ITEM_COMMON* item = manager.FindCommonItem(typeId);
		if (item)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap(item->Icon);
			if (!bitmap)
				bitmap = g_pFresh->GetBitmap("caddie_no");
			float halfWidth = (float)bitmap->Width() * .5f;
			float halfHeight = (float)bitmap->Height() * .5f;

			gdi->DrawTexture(bitmap,
				WRect(mouse.x - halfWidth, mouse.y - halfHeight,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xc8ffffff, 0);
		}
	}
}

IMPLEMENT_OBJECT(FrPrivateTradeWnd, FrForm)
BEGIN_FRESH_MSGMAP(FrPrivateTradeWnd, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrPrivateTradeWnd::OnViewDraw)
ON_FRESH_VI("ItemList", FRCMD_INIT, FrPrivateTradeWnd::OnListInit)
ON_FRESH_VI("ItemList", FRCMD_OWNERDRAW, FrPrivateTradeWnd::OnListDraw)
ON_FRESH_VV("ItemList", FRCMD_LBUTTONDOWN, FrPrivateTradeWnd::OnListDown)
ON_FRESH_VV("ItemList", FRCMD_DBLCLICK, FrPrivateTradeWnd::OnListDblClick)
ON_FRESH_VI("NumEditBtn01", FRCMD_INIT, FrPrivateTradeWnd::OnNumEditBtn01Init)
ON_FRESH_VV("NumEditBtn01", FRCMD_LBUTTONUP,
	FrPrivateTradeWnd::OnNumEditBtn01Down)
ON_FRESH_VI("NumEditBtn02", FRCMD_INIT, FrPrivateTradeWnd::OnNumEditBtn02Init)
ON_FRESH_VV("NumEditBtn02", FRCMD_LBUTTONUP,
	FrPrivateTradeWnd::OnNumEditBtn02Down)
ON_FRESH_VI("NumEditBtn03", FRCMD_INIT, FrPrivateTradeWnd::OnNumEditBtn03Init)
ON_FRESH_VV("NumEditBtn03", FRCMD_LBUTTONUP,
	FrPrivateTradeWnd::OnNumEditBtn03Down)
ON_FRESH_VI("NumEditBtn04", FRCMD_INIT, FrPrivateTradeWnd::OnNumEditBtn04Init)
ON_FRESH_VV("NumEditBtn04", FRCMD_LBUTTONUP,
	FrPrivateTradeWnd::OnNumEditBtn04Down)
END_FRESH_MSGMAP()

FrPrivateTradeWnd::FrPrivateTradeWnd()
	: m_nick("unknown_nick"),
	  m_avatarIndex(0),
	  m_pPangEdit(NULL),
	  m_pNumEdit(NULL),
	  m_bActiveControl(true),
	  m_confirmState(0),
	  m_pTradeItem(NULL)
{
	back_insert_null(m_numEditBtn, 4);
	m_pTradeItem = new _sPrivateTradeItem;
	memset(m_pTradeItem, 0, sizeof(*m_pTradeItem));
}

FrPrivateTradeWnd::~FrPrivateTradeWnd()
{
	if (m_pTradeItem)
	{
		delete m_pTradeItem;
		m_pTradeItem = NULL;
	}
	m_pList->ClearItem();
	m_numEditBtn.clear();
}

bool FrPrivateTradeWnd::OnInit()
{
	WRect rect(GetRect());
	rect.x += 145;
	rect.y += 5;
	rect.w -= 150;
	rect.h = 20;
	EncapsulatedPang::SetRect(rect);
	return FrForm::OnInit();
}

bool FrPrivateTradeWnd::DropItemUp()
{
	return false;
}

bool FrPrivateTradeWnd::DropItemDown()
{
	if (!m_bActiveControl)
		return false;
	const sTradeDropItem* data = GetDropItem();
	if (data)
	{
		int count = 0;
		for (int i = 0; i < 4; ++i)
		{
			FrListItem* item = m_pList->GetItem(i);
			if (item)
			{
				_sPrivateTradeItemSingle* info =
					(_sPrivateTradeItemSingle*)item->pData;
				if (info)
				{
					if (info->id && info->typeId)
						++count;
					if (data->id == info->id && data->typeId == info->typeId)
					{
						++info->count;
						OnSendItemUpdate();
						return true;
					}
				}
			}
		}
		if (count < 4)
		{
			AddItem(data);
			return true;
		}
	}
	return false;
}

void FrPrivateTradeWnd::DropItemMoved()
{
	if (m_bActiveControl)
	{
		SubItem(GetDropItem());
		DoTradeUnLock();
	}
}

void FrPrivateTradeWnd::SetAvatarIndex(int index)
{
	m_avatarIndex = index;
}

void FrPrivateTradeWnd::GetTradeItems(_sPrivateTradeItem& items)
{
	items.pang = GetPang();
	for (int i = 0; i < 4; ++i)
	{
		FrListItem* item = m_pList->GetItem(i);
		if (item && item->pData)
			items.item[i] = *(_sPrivateTradeItemSingle*)item->pData;
	}
}

void FrPrivateTradeWnd::DoConfirm()
{
	m_confirmState = 1;
}

void FrPrivateTradeWnd::DoPermit()
{
	m_confirmState = 2;
	m_bActiveControl = false;
}

void FrPrivateTradeWnd::DoTradeUnLock()
{
	if (m_confirmState)
	{
		FrPrivateTrade* trade = (FrPrivateTrade*)g_pFresh->GetManager()
									->GetDesktop()
									->FindChildForm("FrPrivateTrade");
		if (trade)
			trade->ButtonContolByConfirmCancel();
		m_confirmState = 0;
	}
}

void FrPrivateTradeWnd::OpenPangEditWnd()
{
	if (m_bActiveControl)
	{
		WRect rect(*GetRectPtr());
		if (!m_pPangEdit &&
			rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			OnSendItemUpdate();
			m_pPangEdit = CreateForm<FrPangEdit>(g_pFresh->GetManager(), this,
				"FrPangEdit");
			m_pPangEdit->EnableDrag(false);
			m_pPangEdit->SetFixed(true);
			m_pPangEdit->SetPangValue(GetPang());
			m_pPangEdit->SetTexRate(5);
			m_pPangEdit->Open(
				(FRESH_PFN_RESULT)&FrPrivateTradeWnd::OnResultEditPang,
				WPoint(rect.x + 10, rect.y + 10), FrESCAPE);
		}
	}
}

void FrPrivateTradeWnd::OpenNumEditWnd(int index)
{
	if (m_bActiveControl && IS_INRANGE(index, 0, 3))
	{
		FrButton* button = m_numEditBtn[index];
		if (!button || button->IsVisible())
		{
			FrListItem* item = m_pList->GetItem(index);
			if (item)
			{
				sTradeDropItem* data = (sTradeDropItem*)item->pData;
				if (data->id || data->typeId)
				{
					WRect pos(button->GetRect().x, button->GetRect().y,
						button->GetRect().w, button->GetRect().h);
					if (!m_pNumEdit)
					{
						OnSendItemUpdate();
						m_pNumEdit = CreateForm<FrNumEdit>(
							g_pFresh->GetManager(), this, "FrNumEdit");
						m_pNumEdit->EnableDrag(false);
						m_pNumEdit->SetFixed(true);
						m_pNumEdit->SetValue(data->count);
						m_pNumEdit->SetReturnParam(item);
						m_pNumEdit->SetEditRange(0,
							GetMyItemCount(data->id, data->typeId));
						m_pNumEdit->Open((FRESH_PFN_RESULT)&FrPrivateTradeWnd::
											 OnResultEditNum,
							WPoint(pos.x + 10, pos.y + 10), FrESCAPE);
					}
				}
			}
		}
	}
}

void FrPrivateTradeWnd::OnSendItemUpdate()
{
	if (m_bActiveControl)
	{
		AfxGetTask()->GetActor("PrivateTrade")
			<< MsgObject(NULL, 0x247, 1, (int)m_pTradeItem, 0, 0, 0);
		FrPrivateTrade* trade = (FrPrivateTrade*)g_pFresh->GetManager()
									->GetDesktop()
									->FindChildForm("FrPrivateTrade");
		if (trade)
			trade->SetTradeUnLockAll();
	}
}

void FrPrivateTradeWnd::OnProc(const float delta)
{
	if (IsStyle(STYLE_HOVER))
		if (g_input->GetButton(LEFT_BUTTON) == 1)
			OpenPangEditWnd();
}

bool FrPrivateTradeWnd::OnResultEditPang(int result, FrForm* form)
{
	if (m_bActiveControl && result == FrOK)
	{
		FrPangEdit* edit = DYNAMIC_CAST(FrPangEdit, form);
		if (edit)
		{
			__int64 pang = edit->GetPangValue();
			__int64 tax = edit->GetTex();
			if (Doc()->m_myInfo.stat.i64Pang >= pang + tax)
			{
				SetPang(pang);
				m_pTradeItem->pang = pang;
				FrPrivateTrade* trade = (FrPrivateTrade*)g_pFresh->GetManager()
											->GetDesktop()
											->FindChildForm("FrPrivateTrade");
				if (trade)
					trade->SetPang(tax);
				FrMyWareHouse* warehouse =
					FindPrivateTradeChildForm<FrMyWareHouse>("FrMyWareHouse");
				if (warehouse)
					warehouse->SetPang(
						Doc()->m_myInfo.stat.i64Pang - tax - pang);
				OnSendItemUpdate();
			}
		}
	}
	m_pPangEdit = NULL;
	return true;
}

bool FrPrivateTradeWnd::OnResultEditNum(int result, FrForm* form)
{
	if (m_bActiveControl && result == FrOK)
	{
		FrNumEdit* edit = DYNAMIC_CAST(FrNumEdit, form);
		if (edit)
		{
			unsigned long value = edit->GetValue();
			FrListItem* item = (FrListItem*)edit->GetReturnParam();
			if (item)
			{
				_sPrivateTradeItemSingle* data =
					(_sPrivateTradeItemSingle*)item->pData;
				if (data)
				{
					unsigned long count =
						GetMyItemCount(data->id, data->typeId);
					if (value <= count)
					{
						data->count = value;
						FrMyWareHouse* warehouse =
							FindPrivateTradeChildForm<FrMyWareHouse>(
								"FrMyWareHouse");
						if (warehouse)
							warehouse->SetListItem(data->id, data->typeId,
								count - value);
						OnSendItemUpdate();
					}
				}
			}
		}
	}
	m_pNumEdit = NULL;
	return true;
}

void FrPrivateTradeWnd::AddItem(const sTradeDropItem* data)
{
	if (m_bActiveControl)
	{
		for (int i = 0; i < 4; ++i)
		{
			FrListItem* item = m_pList->GetItem(i);
			if (item)
			{
				_sPrivateTradeItemSingle* info =
					(_sPrivateTradeItemSingle*)item->pData;
				if (!info->id && !info->typeId)
				{
					Formator(*data, *info);
					OnSendItemUpdate();
					break;
				}
			}
		}
	}
}

void FrPrivateTradeWnd::OnUpdateTradeItem(const _sPrivateTradeItem& items)
{
	if (!m_bActiveControl)
	{
		*m_pTradeItem = items;
		SetPang(m_pTradeItem->pang);
	}
}

void FrPrivateTradeWnd::SubItem(const sTradeDropItem* data)
{
	for (int i = 0; i < 4; ++i)
	{
		FrListItem* item = m_pList->GetItem(i);
		if (item)
		{
			_sPrivateTradeItemSingle* info =
				(_sPrivateTradeItemSingle*)item->pData;
			if (info && info->id == data->id && info->typeId == data->typeId)
			{
				memset(info, 0, sizeof(*info));
				OnSendItemUpdate();
				break;
			}
		}
	}
}

void FrPrivateTradeWnd::OnListInit(int param)
{
	m_pList = DYNAMIC_CAST(FrListBox, param);
	for (int i = 0; i < 4; ++i)
		m_pList->AddItem(&m_pTradeItem->item[i]);
}

void FrPrivateTradeWnd::OnListDown()
{
	if (m_bActiveControl)
	{
		FrListItem* item = m_pList->GetSelected();
		if (item)
		{
			_sPrivateTradeItemSingle* data =
				(_sPrivateTradeItemSingle*)item->pData;
			if (data && (data->id || data->typeId))
				CTradeDragDrop::SetPopItem(
					sTradeDropItem(data->id, data->typeId));
		}
	}
}

void FrPrivateTradeWnd::OnListDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	if (!item->pData)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	WRect rect(item->pos,
		WSize(m_pList->GetItemWidth(), m_pList->GetItemHeight()));
	const Bitmap* background = g_pFresh->GetBitmap("cfrm00");
	WRect source(0, 0, background->Width(), background->Height());
	_sPrivateTradeItemSingle* data = (_sPrivateTradeItemSingle*)item->pData;
	if (!data->id && !data->typeId)
	{
		gdi->DrawTexture(background, source, rect, 0xa0ffffff, 0);
		gdi->LineBox(WRect(rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2),
			0xff0000ff);
		if (IS_INRANGE(item->idx, 0, 3))
			m_numEditBtn[item->idx]->SetVisible(false);
		return;
	}
	std::string name(
		"\xbe\xcb\xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb");
	CItemManager& manager = Doc()->m_itemManager;
	IFF_ITEM_COMMON* info = manager.FindCommonItem(data->typeId);
	const Bitmap* bitmap = NULL;
	if (info)
	{
		bitmap = g_pFresh->GetBitmap(info->Icon);
		name = info->Name;
	}
	if (!bitmap)
		bitmap = g_pFresh->GetBitmap("caddie_no");
	gdi->LineBox(WRect(rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2),
		0xffffff00);
	WPoint point(item->pos);
	point.x += (rect.w - bitmap->Width()) * .5f;
	point.y += (rect.h - bitmap->Height()) * .5f;
	WRect icon(point, bitmap->Width(), bitmap->Height());
	if ((data->typeId >> 26) == PIG_ITEM)
	{
		icon.x += 10;
		icon.y += 10;
	}
	gdi->DrawTexture(bitmap, icon, 0xffffffff, 0);
	if ((data->typeId >> 26) == PIG_PART)
		DrawPartsCardSlot(data, rect, 255, 1);
	if ((data->typeId >> 26) == PIG_ITEM || (data->typeId >> 26) == PIG_CARD)
	{
		m_pFont->Print(g_view, rect.x + 10, rect.y + rect.h - 25,
			MakeStr("%d", data->count), 0, 0xffffffff, NULL);
		if (IS_INRANGE(item->idx, 0, 3) && m_bActiveControl)
			m_numEditBtn[item->idx]->SetVisible(true);
	}
	else
	{
		if (IS_INRANGE(item->idx, 0, 3))
			m_numEditBtn[item->idx]->SetVisible(false);
	}
	if (m_confirmState == 1)
		gdi->DrawTexture(background, source, rect, 0x40ffff00, 0);
	if (m_confirmState == 2)
		gdi->DrawTexture(background, source, rect, 0x4000ff00, 0);
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		FrTradeToolTip* tooltip =
			FindPrivateTradeChildForm<FrTradeToolTip>("FrTradeToolTip");
		if (tooltip)
			tooltip->SetToolTip(data);
	}
}

void FrPrivateTradeWnd::OnListDblClick()
{
	if (m_bActiveControl)
	{
		FrListItem* item = m_pList->GetSelected();
		if (item)
		{
			_sPrivateTradeItemSingle* data =
				(_sPrivateTradeItemSingle*)item->pData;
			if (data)
			{
				FrPrivateTrade* trade = (FrPrivateTrade*)g_pFresh->GetManager()
											->GetDesktop()
											->FindChildForm("FrPrivateTrade");
				if (trade)
					trade->MoveItem("FrPrivateTradeWnd", data);
			}
		}
	}
}

void FrPrivateTradeWnd::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* background = g_pFresh->GetBitmap("cfrm00");
	WRect source(0, 0, background->Width(), background->Height());
	gdi->DrawTexture(background, source, GetRect(), 0xa0000080, 0);
	CItemManager& manager = Doc()->m_itemManager;
	IFF_ITEM_COMMON* character =
		manager.FindCommonItem(m_avatarIndex | 0x04000000);
	const Bitmap* bitmap = NULL;
	if (character)
		bitmap = g_pFresh->GetBitmap(MakeStr("w_%s", character->Icon));
	if (!bitmap)
		bitmap = g_pFresh->GetBitmap("caddie_no");
	gdi->DrawTexture(bitmap,
		WRect(GetRect().x - 5, GetRect().y - 5, bitmap->Width(),
			bitmap->Height()),
		0xffffffff, 0);
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xff000000, 0xffffffff);
	g_pFresh->GetManager()->PrintText(
		WPoint(GetRect().x + 40, GetRect().y + 11), 0, m_nick.c_str(), -1,
		0xffffffff);
	gdi->SetTextStyle(0);
	DrawPang();
	if (m_confirmState)
	{
		const WRect& screen = GetRect();
		if (m_confirmState == 1)
			gdi->DrawTexture(background, source,
				WRect(screen.x - 5, screen.y - 5, screen.w + 10, screen.h + 10),
				0x40ffff00, 0);
		else
			gdi->DrawTexture(background, source,
				WRect(screen.x - 5, screen.y - 5, screen.w + 10, screen.h + 10),
				0x4000ff00, 0);
		WRect rect(screen.x - 5, screen.y - 5, screen.w + 10, screen.h + 10);
		gdi->LineBox(WRect(rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2),
			0xff000000);
	}
}

void FrPrivateTradeWnd::OnNumEditBtn01Init(int param)
{
	m_numEditBtn[0] = DYNAMIC_CAST(FrButton, param);
	if (m_numEditBtn[0])
		m_numEditBtn[0]->SetVisible(false);
}

void FrPrivateTradeWnd::OnNumEditBtn02Init(int param)
{
	m_numEditBtn[1] = DYNAMIC_CAST(FrButton, param);
	if (m_numEditBtn[1])
		m_numEditBtn[1]->SetVisible(false);
}

void FrPrivateTradeWnd::OnNumEditBtn03Init(int param)
{
	m_numEditBtn[2] = DYNAMIC_CAST(FrButton, param);
	if (m_numEditBtn[2])
		m_numEditBtn[2]->SetVisible(false);
}

void FrPrivateTradeWnd::OnNumEditBtn04Init(int param)
{
	m_numEditBtn[3] = DYNAMIC_CAST(FrButton, param);
	if (m_numEditBtn[3])
		m_numEditBtn[3]->SetVisible(false);
}

void FrPrivateTradeWnd::OnNumEditBtn01Down()
{
	OpenNumEditWnd(0);
}

void FrPrivateTradeWnd::OnNumEditBtn02Down()
{
	OpenNumEditWnd(1);
}

void FrPrivateTradeWnd::OnNumEditBtn03Down()
{
	OpenNumEditWnd(2);
}

void FrPrivateTradeWnd::OnNumEditBtn04Down()
{
	OpenNumEditWnd(3);
}

IMPLEMENT_OBJECT(FrPrivateTrade, FrForm)
BEGIN_FRESH_MSGMAP(FrPrivateTrade, FrForm)
ON_FRESH_VV("RenderArea", FRCMD_OWNERDRAW, FrPrivateTrade::OnViewDraw)
ON_FRESH_VI("Confirm", FRCMD_INIT, FrPrivateTrade::OnInitConfirmBtn)
ON_FRESH_VI("Permit", FRCMD_INIT, FrPrivateTrade::OnInitPermitBtn)
ON_FRESH_VI("Cancel", FRCMD_INIT, FrPrivateTrade::OnInitCancelBtn)
ON_FRESH_VV("Confirm", FRCMD_LBUTTONUP, FrPrivateTrade::OnDownConfirmBtn)
ON_FRESH_VV("Permit", FRCMD_LBUTTONUP, FrPrivateTrade::OnDownPermitBtn)
ON_FRESH_VV("Cancel", FRCMD_LBUTTONUP, FrPrivateTrade::OnDownCancelBtn)
END_FRESH_MSGMAP()

FrPrivateTrade::FrPrivateTrade()
	: m_pDropItem(NULL),
	  m_pConfirmBtn(NULL),
	  m_pPermitBtn(NULL),
	  m_pCancelBtn(NULL)
{
}

FrPrivateTrade::~FrPrivateTrade()
{
}

void FrPrivateTrade::SetCompanion(const std::string& nick, unsigned long guid,
	unsigned long typeId)
{
	m_companionNick = nick;
	m_companionGuid = guid;
	m_companionTypeId = typeId;
}

bool FrPrivateTrade::OnInit()
{
	FrForm::OnInit();
	FrPrivateTradeWnd* tradeWnd = CreateForm<FrPrivateTradeWnd>(
		g_pFresh->GetManager(), NULL, "FrPrivateTradeWnd", this);
	if (!tradeWnd)
		return false;
	tradeWnd->SetFixed(true);
	tradeWnd->EnableDrag(false);
	const WRect& yourRect = tradeWnd->GetRect();
	tradeWnd->Open(NULL, WPoint(GetRect().x + 16, GetRect().y + 16),
		FrMODALESS | FrNOTAIL);
	m_pYourWnd = tradeWnd;
	m_pYourWnd->EnableActiveControl(false);
	m_pYourWnd->SetNick(m_companionNick);
	m_pYourWnd->SetAvatarIndex(m_companionTypeId & 0x03ffffff);

	tradeWnd = CreateForm<FrPrivateTradeWnd>(g_pFresh->GetManager(), NULL,
		"FrPrivateTradeWnd", this);
	if (!tradeWnd)
		return false;
	tradeWnd->SetFixed(true);
	tradeWnd->EnableDrag(false);
	tradeWnd->Open(NULL, WPoint(GetRect().x + 16, yourRect.y + yourRect.h + 16),
		FrMODALESS | FrNOTAIL);
	m_pMyWnd = tradeWnd;
	m_pMyWnd->SetNick(std::string(Doc()->m_myInfo.info.sNick));
	CSharedDoc* doc = Doc();
	std::map<unsigned int, sCharacterInfo>::iterator it =
		doc->m_charMap.find(doc->m_myInfo.userEquip.guidChar);
	if (it != doc->m_charMap.end())
		m_pMyWnd->SetAvatarIndex((*it).second.tid & 0x03ffffff);
	m_pMyWnd->AddPangBarStyle(STYLE_HOVER);
	{
		WRect pangRect(tradeWnd->GetRect());
		pangRect.x = pangRect.x + pangRect.w - 150;
		pangRect.y = pangRect.y + pangRect.h + 5;
		pangRect.w = 150;
		pangRect.h = 20;
		EncapsulatedPang::SetRect(pangRect);
	}
	FrMyWareHouse* warehouse = CreateForm<FrMyWareHouse>(g_pFresh->GetManager(),
		NULL, "FrMyWareHouse", this);
	if (!warehouse)
		return false;
	warehouse->SetFixed(true);
	warehouse->EnableDrag(false);
	warehouse->Open(NULL,
		WPoint(GetRect().x + GetRect().w - warehouse->GetRect().w - 16,
			GetRect().y + (GetRect().h - warehouse->GetRect().h) * .5f),
		FrMODALESS | FrNOTAIL);
	m_pWareHouse = warehouse;
	FrTradeToolTip* tooltip = CreateForm<FrTradeToolTip>(g_pFresh->GetManager(),
		NULL, "FrTradeToolTip", this);
	if (!tooltip)
		return false;
	tooltip->SetFixed(true);
	tooltip->EnableDrag(false);
	tooltip->Open(NULL,
		WPoint(GetRect().x + 16,
			GetRect().y + GetRect().h - tooltip->GetRect().h - 16),
		FrMODALESS | FrNOTAIL);
	m_pToolTip = tooltip;
	return true;
}

void FrPrivateTrade::GetMyTradeItems(_sPrivateTradeItem& items)
{
	m_pMyWnd->GetTradeItems(items);
}

void FrPrivateTrade::MoveItem(const char* from, const void* item)
{
	CTradeDragDrop* source = NULL;
	CTradeDragDrop* target = NULL;
	if (_stricmp(from, "FrMyWareHouse") == 0)
	{
		const sWarpItem* data = (const sWarpItem*)item;
		if (!data)
			return;
		source = m_pWareHouse;
		target = m_pMyWnd;
		source->SetPopItem(sTradeDropItem(data->id, data->typeId));
	}
	if (_stricmp(from, "FrPrivateTradeWnd") == 0)
	{
		const _sPrivateTradeItemSingle* data =
			(const _sPrivateTradeItemSingle*)item;
		if (!data)
			return;
		source = m_pMyWnd;
		target = m_pWareHouse;
		source->SetPopItem(sTradeDropItem(data->id, data->typeId));
	}
	if (source && target && target->DropItemDown())
		source->DropItemMoved();
}

void FrPrivateTrade::OnProc(const float delta)
{
	static CTradeDragDrop* source = NULL;
	FrForm::OnProc(delta);
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if (g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect(m_pMyWnd->GetRect());
		if (rect.IsInRect(mouse))
			source = m_pMyWnd;
		else
		{
			WRect rect(m_pWareHouse->GetRect());
			if (rect.IsInRect(mouse))
				source = m_pWareHouse;
			else
			{
				m_pDropItem = NULL;
				source = NULL;
			}
		}
		if (source)
			source->Pop();
	}
	if (g_input->GetButton(LEFT_BUTTON) == 3 && source)
	{
		m_pDropItem = source->GetDropItem();
		if (m_pDropItem)
		{
			m_pToolTip->SetDragDraw(true);
			m_pToolTip->SetDragItem(m_pDropItem);
		}
		else
		{
			m_pDropItem = NULL;
			source = NULL;
		}
	}
	if (g_input->GetButton(LEFT_BUTTON) == 2)
	{
		CTradeDragDrop* target = NULL;
		WRect rect(m_pMyWnd->GetRect());
		if (rect.IsInRect(mouse))
			target = m_pMyWnd;
		else
		{
			WRect rect(m_pWareHouse->GetRect());
			if (rect.IsInRect(mouse))
				target = m_pWareHouse;
		}
		CTradeDragDrop::Drop(target);
		m_pDropItem = NULL;
		source = NULL;
	}
	if (!m_pDropItem)
		m_pToolTip->SetDragDraw(false);
}

void FrPrivateTrade::UpdateFrTradeItem(const _sPrivateTradeItem& items)
{
	m_pYourWnd->OnUpdateTradeItem(items);
	SetTradeUnLockAll();
}

void FrPrivateTrade::FinalFrWait(const _sPrivateTradeItem& items)
{
	_sPrivateTradeItem current;
	memset(&current, 0, sizeof(current));
	m_pYourWnd->GetTradeItems(current);
	if (memcmp(&current, &items, sizeof(items)))
	{
		m_pYourWnd->OnUpdateTradeItem(items);
		SetTradeUnLockAll();
	}
	m_pYourWnd->DoConfirm();
}

void FrPrivateTrade::SetTradeUnLockAll()
{
	m_pYourWnd->DoTradeUnLock();
	m_pMyWnd->DoTradeUnLock();
}

void FrPrivateTrade::FinalFrOK(const _sPrivateTradeItem& items)
{
	_sPrivateTradeItem current;
	memset(&current, 0, sizeof(current));
	m_pYourWnd->GetTradeItems(current);
	if (memcmp(&current, &items, sizeof(items)))
		m_pYourWnd->OnUpdateTradeItem(items);
	m_pYourWnd->DoPermit();
}

void FrPrivateTrade::ButtonContolByConfirmCancel()
{
	m_pConfirmBtn->Enable(true);
	m_pPermitBtn->Enable(false);
	m_pCancelBtn->Enable(true);
}

void FrPrivateTrade::OnViewDraw()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap = g_pFresh->GetBitmap("cfrm00");
	WRect source(0, 0, bitmap->Width(), bitmap->Height());
	gdi->DrawTexture(bitmap, source, GetRect(), 0xffffffff, 0);
	DrawPang();
}

void FrPrivateTrade::OnInitConfirmBtn(int param)
{
	m_pConfirmBtn = DYNAMIC_CAST(FrButton, param);
}

void FrPrivateTrade::OnInitPermitBtn(int param)
{
	m_pPermitBtn = DYNAMIC_CAST(FrButton, param);
}

void FrPrivateTrade::OnInitCancelBtn(int param)
{
	m_pCancelBtn = DYNAMIC_CAST(FrButton, param);
}

void FrPrivateTrade::OnDownConfirmBtn()
{
	m_pConfirmBtn->Enable(false);
	m_pPermitBtn->Enable(true);
	m_pMyWnd->DoConfirm();
	_sPrivateTradeItem items;
	memset(&items, 0, sizeof(items));
	m_pMyWnd->GetTradeItems(items);
	AfxGetTask()->GetActor("PrivateTrade")
		<< MsgObject(NULL, 0x247, 2, (int)&items, 0, 0, 0);
}

void FrPrivateTrade::OnDownPermitBtn()
{
	m_pConfirmBtn->Enable(false);
	m_pPermitBtn->Enable(false);
	m_pMyWnd->DoPermit();
	_sPrivateTradeItem mine;
	_sPrivateTradeItem yours;
	memset(&mine, 0, sizeof(mine));
	memset(&yours, 0, sizeof(yours));
	m_pMyWnd->GetTradeItems(mine);
	m_pYourWnd->GetTradeItems(yours);
	AfxGetTask()->GetActor("PrivateTrade")
		<< MsgObject(NULL, 0x247, 3, (int)&mine, (int)&yours, 0, 0);
}

void FrPrivateTrade::OnDownCancelBtn()
{
	FrForm::Close(FrCANCEL, true);
}
