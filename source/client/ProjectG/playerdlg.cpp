#include "playerdlg.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "inputmanager.h"
#include "taskmanager.h"
#include "cardsystem.h"
#include "mathconsts.h"
#include <float.h>

CStatBar::CStatBar(float gap, float height)
{
	m_gap = gap;
	m_height = height;
	m_bars.reserve(5);
	for (int i = 0; i < 5; ++i)
	{
		sBar bar(MakeStr("stat_gauge_%d", i));
		bar.fMax = i == 0 ? 50.0f : 30.0f;
		m_bars.push_back(bar);
	}
	m_destTime = 0.0f;
}

CStatBar::~CStatBar()
{
}

void CStatBar::SetRange(eLvlType type, int level, int capacity, int penalty)
{
	m_bars[type].fLevel = max(0.0f, (float)level);
	m_bars[type].fCur = 0.0f;
	m_bars[type].fCapacity = min((float)capacity, m_bars[type].fMax);
	m_bars[type].penalty = penalty;
}

void CStatBar::SetDestTime(float time)
{
	m_destTime = time;
}

void CStatBar::Process(float delta)
{
	for (int i = 0; i < 5; ++i)
	{
		if (m_bars[i].fCur < m_bars[i].fLevel)
		{
			if (m_destTime > FLT_EPSILON)
				m_bars[i].fCur += m_bars[i].fLevel / m_destTime * delta;
			else
				m_bars[i].fCur = m_bars[i].fLevel;
			if (m_bars[i].fCur > m_bars[i].fLevel)
				m_bars[i].fCur = m_bars[i].fLevel;
		}
		if (m_bars[i].fCur > m_bars[i].fLevel)
		{
			if (m_destTime > FLT_EPSILON)
				m_bars[i].fCur -= m_bars[i].fLevel / m_destTime * delta;
			else
				m_bars[i].fCur = m_bars[i].fLevel;
			if (m_bars[i].fCur < m_bars[i].fLevel)
				m_bars[i].fCur = m_bars[i].fLevel;
		}
	}
}

void CStatBar::SetPos(eLvlType type, int level, int capacity, int penalty)
{
	m_bars[type].fLevel = max(0.0f, (float)level);
	m_bars[type].fCapacity = (float)capacity;
	m_bars[type].penalty = penalty;
}

void CStatBar::Render(const WPoint& pos)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic)
	{
		for (int i = 0; i < 5; ++i)
		{
			const float& height = m_height;
			sBar& bar = m_bars[i];
			float row = (float)i;
			{
				WRect rect;
				rect.x = pos.x;
				rect.y = (pos.y + (height + m_gap) * row);
				rect.h = m_height;
				rect.w = bar.fCur / bar.fMax * 100.0f;
				graphic->DrawTexture(m_bars[i].pBitmap, rect, -1, 0);
			}
			graphic->SetTextStyle(0);
			graphic->SetTextColor(-1, -1);
			graphic->Print(
				WPoint(pos.x + 100.0f, (pos.y + (height + m_gap) * row) - 2.0f),
				0, "%d/%d", (int)m_bars[i].fCur, (int)m_bars[i].fCapacity);
		}
	}
}

IMPLEMENT_OBJECT(FrSubEquipBar, FrForm)
BEGIN_FRESH_MSGMAP(FrSubEquipBar, FrForm)
ON_FRESH_VI("sub_equip", FRCMD_INIT, FrSubEquipBar::OnInitSubEquipList)
ON_FRESH_VI("sub_equip", FRCMD_LBUTTONDOWN, FrSubEquipBar::OnDownSubEquipList)
ON_FRESH_VI("sub_equip", FRCMD_OWNERDRAW, FrSubEquipBar::OnDrawSubEquipList)
END_FRESH_MSGMAP()

FrSubEquipBar::FrSubEquipBar()
{
	m_pResult = NULL;
}

FrSubEquipBar::~FrSubEquipBar()
{
}

void FrSubEquipBar::Initialize()
{
	if (!m_pSubEquipList)
		return;
	switch (m_subType)
	{
	case SUBTYPE_0:
		m_pSubEquipList->SetItemSize(48, 52);
		break;
	case SUBTYPE_1:
		m_pSubEquipList->SetItemSize(40, 40);
		break;
	case SUBTYPE_2:
		m_pSubEquipList->SetItemSize(26, 28);
		break;
	case SUBTYPE_3:
		m_pSubEquipList->SetItemSize(30, 30);
		break;
	}
	int count = 0;
	float margin = 0.0f;
	switch (m_category)
	{
	case CATEGORY_CHARACTER:
		count = Doc()->m_charMap.size();
		break;
	case CATEGORY_CADDIE:
		count = Doc()->m_caddieMap.size() + 1;
		margin = 2.0f;
		break;
	case CATEGORY_CLUB:
		count = Doc()->m_clubSetMap.size();
		break;
	case CATEGORY_BALL:
		count = Doc()->m_ballList.size();
		break;
	case CATEGORY_MASCOT:
		count = Doc()->m_mascotMap.size() + 1;
		margin = 2.0f;
		break;
	}
	if (m_category == CATEGORY_CHARACTER)
	{
		float gap = count * 3.0f + 10.0f;
		WRect rect = m_rect;
		rect.w = m_pSubEquipList->GetItemWidth() * count + gap + 0.5f;
		rect.h = m_pSubEquipList->GetItemHeight() + 13.0f;
		SetRect(rect);
		rect.x += 8.0f;
		rect.y += 5.0f;
		rect.w = m_pSubEquipList->GetItemWidth() * count + gap + 0.5f;
		rect.h = (float)m_pSubEquipList->GetItemHeight();
		m_pSubEquipList->SetRect(rect);
	}
	else if (count > 6)
	{
		WRect rect = m_rect;
		rect.w = m_pSubEquipList->GetItemWidth() * 6 + 41.0f;
		rect.h = m_pSubEquipList->GetItemHeight() + 13.0f;
		SetRect(rect);
		rect.x += 5.0f;
		rect.y += 5.0f;
		rect.w = m_pSubEquipList->GetItemWidth() * 6 + 28.0f;
		rect.h = (float)m_pSubEquipList->GetItemHeight();
		m_pSubEquipList->SetRect(rect);
	}
	else
	{
		float gap = count * 3.0f + margin + 10.0f;
		WRect rect = m_rect;
		rect.w = m_pSubEquipList->GetItemWidth() * count + gap + 0.5f;
		rect.h = m_pSubEquipList->GetItemHeight() + 13.0f;
		SetRect(rect);
		rect.x += 8.0f;
		rect.y += 5.0f;
		rect.w = m_pSubEquipList->GetItemWidth() * count + gap + 0.5f;
		rect.h = (float)m_pSubEquipList->GetItemHeight();
		m_pSubEquipList->SetRect(rect);
	}

	SIZE size;
	size.cx = m_pSubEquipList->GetItemWidth();
	size.cy = m_pSubEquipList->GetItemHeight();
	size.cx += 2;
	m_pSubEquipList->Resize(size.cx, size.cy);
	switch (m_category)
	{
	case CATEGORY_CHARACTER:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.begin();
		while (it != Doc()->m_charMap.end())
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(it->second.tid);
			if (item)
				m_sortList.push_back(_sortitem(item, it->first));
			++it;
		}
		break;
	}
	case CATEGORY_CADDIE:
	{
		if (Doc()->m_caddieMap.size())
		{
			std::map<unsigned int, sCaddieInfo>::iterator it =
				Doc()->m_caddieMap.begin();
			while (it != Doc()->m_caddieMap.end())
			{
				IFF_ITEM_COMMON* item =
					ItemManager()->FindCommonItem(it->second.tid);
				if (item)
					m_sortList.push_back(_sortitem(item, it->second.guid));
				++it;
			}
		}
		break;
	}
	case CATEGORY_CLUB:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.begin();
		while (it != Doc()->m_clubSetMap.end())
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(it->second.tid);
			if (item &&
				((item->Level >= Doc()->m_myInfo.stat.Level &&
					 item->IsUnderLvl) ||
					!item->IsUnderLvl))
				m_sortList.push_back(_sortitem(item, it->second.guid));
			++it;
		}
		break;
	}
	case CATEGORY_BALL:
	{
		std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
		while (it != Doc()->m_ballList.end())
		{
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->tid);
			if (item)
				m_sortList.push_back(_sortitem(item, 0));
			++it;
		}
		break;
	}
	case CATEGORY_MASCOT:
	{
		if (Doc()->m_mascotMap.size())
		{
			std::map<unsigned int, sMascotInfo>::iterator it =
				Doc()->m_mascotMap.begin();
			while (it != Doc()->m_mascotMap.end())
			{
				IFF_ITEM_COMMON* item =
					ItemManager()->FindCommonItem(it->second.tid);
				if (item)
					m_sortList.push_back(_sortitem(item, it->second.guid));
				++it;
			}
		}
		break;
	}
	}
	m_sortList.sort();
	for (std::list<_sortitem>::iterator it = m_sortList.begin();
		it != m_sortList.end(); ++it)
		m_pSubEquipList->AddItem(&*it);
	if (m_category == CATEGORY_MASCOT || m_category == CATEGORY_CADDIE)
		m_pSubEquipList->AddItem(NULL);
}

bool FrSubEquipBar::SetClientEquipInfo(PANGYA_ITEM_GROUP group,
	unsigned long guid)
{
	bool found = false;
	switch (group)
	{
	case PIG_CHAR:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.find(guid);
		if (it != Doc()->m_charMap.end())
			found = true;
		break;
	}
	case PIG_BALL:
	{
		std::list<sItemInfo>::iterator it =
			std::find(Doc()->m_ballList.begin(), Doc()->m_ballList.end(), guid);
		if (it != Doc()->m_ballList.end())
			found = true;
		break;
	}
	case PIG_CLUBSET:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(guid);
		if (it != Doc()->m_clubSetMap.end())
			found = true;
		break;
	}
	case PIG_MASCOT:
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.find(guid);
		if (it != Doc()->m_mascotMap.end())
			found = true;
		break;
	}
	case PIG_CADDIE:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.find(guid);
		if (it != Doc()->m_caddieMap.end())
			found = true;
		break;
	}
	}
	return found;
}

void FrSubEquipBar::OnInitSubEquipList(int param)
{
	m_pSubEquipList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pSubEquipList->SetVisible(true);
}

void FrSubEquipBar::OnDownSubEquipList(int param)
{
	FrListItem* selected = m_pSubEquipList->GetSelected();
	if (selected)
	{
		_sortitem* item = (_sortitem*)selected->pData;
		if (item)
		{
			m_pResult = item->pItem;
			m_lastSelectedGuid = item->guid;
			FrForm::OnOK();
		}
		else if (m_category == CATEGORY_CADDIE || m_category == CATEGORY_MASCOT)
		{
			m_pResult = NULL;
			m_lastSelectedGuid = 0;
			FrForm::OnOK();
		}
	}
}

void FrSubEquipBar::OnDrawSubEquipList(int param)
{
	FrListItem* entry = (FrListItem*)param;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!entry || !graphic)
		return;
	_sortitem* data = (_sortitem*)entry->pData;
	IFF_ITEM_COMMON* item = data ? data->pItem : NULL;
	const Bitmap* bitmap = NULL;
	if (item)
	{
		switch (m_subType)
		{
		case SUBTYPE_0:
			bitmap = g_pFresh->GetBitmap(item->Icon);
			break;
		case SUBTYPE_1:
			bitmap = g_pFresh->GetBitmap(MakeStr("w_%s", item->Icon));
			break;
		case SUBTYPE_2:
			bitmap = g_pFresh->GetBitmap(MakeStr("s_%s", item->Icon));
			break;
		case SUBTYPE_3:
			bitmap = g_pFresh->GetBitmap(MakeStr("bar_%s", item->Icon));
			break;
		}
	}
	else
	{
		switch (m_subType)
		{
		case SUBTYPE_0:
			bitmap = g_pFresh->GetBitmap("caddie_no.tga");
			break;
		case SUBTYPE_1:
			bitmap = g_pFresh->GetBitmap("w_caddie_no");
			break;
		case SUBTYPE_2:
			bitmap = g_pFresh->GetBitmap("s_no");
			break;
		case SUBTYPE_3:
			bitmap = g_pFresh->GetBitmap("bar_no");
			break;
		}
	}
	if (bitmap)
	{
		if (entry->idx % m_pSubEquipList->GetColCapacity() == 0)
		{
			const Bitmap* bar = g_pFresh->GetBitmap("select_bar_02");
			if (bar)
				graphic->DrawTexture(bar, m_rect, -1, 0);
		}
		WRect rect(entry->pos.x, entry->pos.y, (float)bitmap->Width(),
			(float)bitmap->Height());
		graphic->DrawTexture(bitmap, rect, -1, 0);
		if (item && (item->TypeId >> 26) == PIG_CADDIE)
		{
			for (std::map<unsigned int, sCaddieInfo>::iterator it =
					 Doc()->m_caddieMap.begin();
				it != Doc()->m_caddieMap.end(); ++it)
			{
				if (item->TypeId == it->second.tid)
				{
					IFF_STRUCT::sCaddie* caddie =
						ItemManager()->FindCaddie(item->TypeId);
					if (caddie && it->second.Rent_flag &&
						it->second.Remain_Date <= 0 &&
						it->second.tidPart == 0 && caddie->MonthlyFee != 0)
					{
						g_view->DrawLine2D(
							WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f),
							WPoint(rect.x + rect.w,
								rect.y + rect.h * 0.5f - 1.0f),
							0xffff0000, 0xffff0000, 0);
						g_view->DrawLine2D(
							WPoint(rect.x, rect.y + rect.h * 0.5f),
							WPoint(rect.x + rect.w, rect.y + rect.h * 0.5f),
							0xffff0000, 0xffff0000, 0);
						g_view->DrawLine2D(
							WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f),
							WPoint(rect.x + rect.w,
								rect.y + rect.h * 0.5f + 1.0f),
							0xffff0000, 0xffff0000, 0);
					}
					break;
				}
			}
		}
		if (rect.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			const Bitmap* frame = g_pFresh->GetBitmap("select_frame");
			if (frame)
				graphic->DrawTexture(frame,
					WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f,
						rect.h + 6.0f),
					-1, 0);
		}
	}
}

void FrSubEquipBar::OnDraw()
{
	FrForm::OnDraw();
}

void FrSubEquipBar::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	if (g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_rect;
		if (!rect.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
			FrForm::OnCancel();
	}
}

void FrSubEquipBar::OnOK()
{
	FrForm::OnOK();
}

void FrSubEquipBar::OnCancel()
{
	FrForm::OnCancel();
}

void FrSubEquipBar::Open(FRESH_PFN_RESULT pFnResult, eCategory category,
	const WPoint& pos, eSubType subType)
{
	m_category = category;
	m_subType = subType;
	Initialize();
	FrForm::Open(pFnResult, pos, FrESCAPE);
}

CEquipHelper::CEquipHelper()
{
}

CEquipHelper::~CEquipHelper()
{
}

IFF_ITEM_COMMON* CEquipHelper::GetItemCommon(eCategory category,
	const sUserEquip& equip)
{
	CSharedDoc* doc = Doc();
	unsigned long tid = 0;
	switch (category)
	{
	case CATEGORY_CHARACTER:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			doc->m_charMap.find(equip.guidChar);
		if (it != doc->m_charMap.end())
			tid = it->second.tid;
		break;
	}
	case CATEGORY_CADDIE:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			doc->m_caddieMap.find(equip.guidCaddie);
		if (it != doc->m_caddieMap.end())
			tid = it->second.tid;
		break;
	}
	case CATEGORY_BALL:
		tid = equip.tidBall;
		break;
	case CATEGORY_CLUB:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			doc->m_clubSetMap.find(equip.guidClubSet);
		if (it != doc->m_clubSetMap.end())
			tid = it->second.tid;
		break;
	}
	case CATEGORY_MASCOT:
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			doc->m_mascotMap.find(equip.guidMascot);
		if (it != doc->m_mascotMap.end())
			tid = it->second.tid;
		break;
	}
	}
	return ItemManager()->FindCommonItem(tid);
}

unsigned long CEquipHelper::GetEquipItemGuid(eCategory category, bool bRandom)
{
	unsigned long guid = 0;
	switch (category)
	{
	case CATEGORY_CHARACTER:
		if (bRandom)
		{
			guid = RandSelectGuid(Doc()->m_charMap);
		}
		else
			return Doc()->m_myInfo.userEquip.guidChar;
		break;
	case CATEGORY_CADDIE:
		if (bRandom)
		{
			unsigned int size = Doc()->m_caddieMap.size();
			if (size && rand() % size)
				guid = RandSelectGuid(Doc()->m_caddieMap);
		}
		else
			return Doc()->m_myInfo.userEquip.guidCaddie;
		break;
	case CATEGORY_BALL:
		if (bRandom)
		{
			int size = Doc()->m_ballList.size();
			if (size == 0)
				return 0;
			std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			int n = rand() % size;
			for (int i = 0; i < n; ++i)
				++it;
			guid = it->tid;
		}
		else
			guid = Doc()->m_myInfo.userEquip.tidBall;
		break;
	case CATEGORY_CLUB:
		if (bRandom)
		{
			guid = RandSelectGuid(Doc()->m_clubSetMap);
		}
		else
			return Doc()->m_myInfo.userEquip.guidClubSet;
		break;
	case CATEGORY_MASCOT:
		if (bRandom)
		{
			unsigned int size = Doc()->m_mascotMap.size();
			if (size && rand() % size)
				guid = RandSelectGuid(Doc()->m_mascotMap);
		}
		else
			return Doc()->m_myInfo.userEquip.guidMascot;
		break;
	}
	return guid;
}

unsigned long CEquipHelper::GetEquipItemTid(eCategory category, bool bRandom)
{
	unsigned long tid = 0;
	switch (category)
	{
	case CATEGORY_CHARACTER:
		if (bRandom)
			tid = MapFindEquipTid(Doc()->m_charMap, 0);
		else
			tid = MapFindEquipTid(Doc()->m_charMap,
				Doc()->m_myInfo.userEquip.guidChar);
		break;
	case CATEGORY_CADDIE:
		if (bRandom)
			tid = MapFindEquipTid(Doc()->m_caddieMap, 0);
		else
			tid = MapFindEquipTid(Doc()->m_caddieMap,
				Doc()->m_myInfo.userEquip.guidCaddie);
		break;
	case CATEGORY_BALL:
		if (bRandom)
		{
			int size = Doc()->m_ballList.size();
			if (size != 0)
			{
				std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
				int n = rand() % size;
				for (int i = 0; i < n; ++i)
					++it;
				tid = it->tid;
			}
		}
		else
		{
			for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
				it != Doc()->m_ballList.end(); ++it)
			{
				if (it->tid == Doc()->m_myInfo.userEquip.tidBall)
					return it->tid;
			}
		}
		break;
	case CATEGORY_CLUB:
		if (bRandom)
			tid = MapFindEquipTid(Doc()->m_clubSetMap, 0);
		else
			tid = MapFindEquipTid(Doc()->m_clubSetMap,
				Doc()->m_myInfo.userEquip.guidClubSet);
		break;
	case CATEGORY_MASCOT:
		if (bRandom)
			tid = MapFindEquipTid(Doc()->m_mascotMap, 0);
		else
			tid = MapFindEquipTid(Doc()->m_mascotMap,
				Doc()->m_myInfo.userEquip.guidMascot);
		break;
	}
	return tid;
}

unsigned long CEquipHelper::GetGuidFromTid(eCategory category,
	unsigned long tid)
{
	unsigned long guid = 0;
	switch (category)
	{
	case CATEGORY_CHARACTER:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.begin();
		std::map<unsigned int, sCharacterInfo>::iterator end =
			Doc()->m_charMap.end();
		for (; it != end; ++it)
		{
			if (it->second.tid == tid)
			{
				guid = it->second.guid;
				break;
			}
		}
		break;
	}
	case CATEGORY_CADDIE:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.begin();
		std::map<unsigned int, sCaddieInfo>::iterator end =
			Doc()->m_caddieMap.end();
		for (; it != end; ++it)
		{
			if (it->second.tid == tid)
			{
				guid = it->second.guid;
				break;
			}
		}
		break;
	}
	case CATEGORY_BALL:
	{
		std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
		std::list<sItemInfo>::iterator end = Doc()->m_ballList.end();
		for (; it != end; ++it)
		{
			if (it->tid == tid)
			{
				guid = it->guid;
				break;
			}
		}
		break;
	}
	case CATEGORY_CLUB:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.begin();
		std::map<unsigned int, sItemInfo>::iterator end =
			Doc()->m_clubSetMap.end();
		for (; it != end; ++it)
		{
			if (it->second.tid == tid)
			{
				guid = it->second.guid;
				break;
			}
		}
		break;
	}
	case CATEGORY_MASCOT:
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.begin();
		std::map<unsigned int, sMascotInfo>::iterator end =
			Doc()->m_mascotMap.end();
		for (; it != end; ++it)
		{
			if (it->second.tid == tid)
			{
				guid = it->second.guid;
				break;
			}
		}
		break;
	}
	}
	return guid;
}

IMPLEMENT_OBJECT(FrPlayerDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrPlayerDlg, FrForm)
ON_FRESH_VI("base", FRCMD_INIT, FrPlayerDlg::OnBaseInit)
ON_FRESH_VV("base", FRCMD_LBUTTONUP, FrPlayerDlg::OnBaseBtnUp)
ON_FRESH_VI("select", FRCMD_INIT, FrPlayerDlg::OnSelectInit)
ON_FRESH_VI("pet", FRCMD_INIT, FrPlayerDlg::OnPetInit)
ON_FRESH_VV("pet", FRCMD_OWNERDRAW, FrPlayerDlg::OnPetOwnerDraw)
ON_FRESH_VI("charprev", FRCMD_INIT, FrPlayerDlg::OnCharPrevInit)
ON_FRESH_VV("charprev", FRCMD_LBUTTONUP, FrPlayerDlg::OnCharPrevBtnUp)
ON_FRESH_VI("charnext", FRCMD_INIT, FrPlayerDlg::OnCharNextInit)
ON_FRESH_VV("charnext", FRCMD_LBUTTONUP, FrPlayerDlg::OnCharNextBtnUp)
ON_FRESH_VI("caddie", FRCMD_INIT, FrPlayerDlg::OnCaddieInit)
ON_FRESH_VI("caddie_limit", FRCMD_INIT, FrPlayerDlg::OnCaddieLimitInit)
ON_FRESH_VI("caddieprev", FRCMD_INIT, FrPlayerDlg::OnCaddiePrevInit)
ON_FRESH_VV("caddieprev", FRCMD_LBUTTONUP, FrPlayerDlg::OnCaddiePrevBtnUp)
ON_FRESH_VI("caddienext", FRCMD_INIT, FrPlayerDlg::OnCaddieNextInit)
ON_FRESH_VV("caddienext", FRCMD_LBUTTONUP, FrPlayerDlg::OnCaddieNextBtnUp)
ON_FRESH_VI("club", FRCMD_INIT, FrPlayerDlg::OnClubInit)
ON_FRESH_VI("club_limit", FRCMD_INIT, FrPlayerDlg::OnClubLimitInit)
ON_FRESH_VI("clubprev", FRCMD_INIT, FrPlayerDlg::OnClubPrevInit)
ON_FRESH_VV("clubprev", FRCMD_LBUTTONUP, FrPlayerDlg::OnClubPrevBtnUp)
ON_FRESH_VI("clubnext", FRCMD_INIT, FrPlayerDlg::OnClubNextInit)
ON_FRESH_VV("clubnext", FRCMD_LBUTTONUP, FrPlayerDlg::OnClubNextBtnUp)
ON_FRESH_VI("ball", FRCMD_INIT, FrPlayerDlg::OnBallInit)
ON_FRESH_VI("ballprev", FRCMD_INIT, FrPlayerDlg::OnBallPrevInit)
ON_FRESH_VV("ballprev", FRCMD_LBUTTONUP, FrPlayerDlg::OnBallPrevBtnUp)
ON_FRESH_VI("ballnext", FRCMD_INIT, FrPlayerDlg::OnBallNextInit)
ON_FRESH_VV("ballnext", FRCMD_LBUTTONUP, FrPlayerDlg::OnBallNextBtnUp)
ON_FRESH_VI("name", FRCMD_INIT, FrPlayerDlg::OnNameInit)
ON_FRESH_VI("remove", FRCMD_INIT, FrPlayerDlg::OnRemoveInit)
ON_FRESH_VV("remove", FRCMD_LBUTTONUP, FrPlayerDlg::OnRemoveBtnUp)
ON_FRESH_VI("index", FRCMD_INIT, FrPlayerDlg::OnIndexInit)
ON_FRESH_VI("powerbar", FRCMD_INIT, FrPlayerDlg::OnPowerBarInit)
ON_FRESH_VI("controlbar", FRCMD_INIT, FrPlayerDlg::OnControlBarInit)
ON_FRESH_VI("impactbar", FRCMD_INIT, FrPlayerDlg::OnImpactBarInit)
ON_FRESH_VI("spinbar", FRCMD_INIT, FrPlayerDlg::OnSpinBarInit)
ON_FRESH_VI("curvebar", FRCMD_INIT, FrPlayerDlg::OnCurveBarInit)
ON_FRESH_VI("epower", FRCMD_INIT, FrPlayerDlg::OnPowerEditInit)
ON_FRESH_VI("econtrol", FRCMD_INIT, FrPlayerDlg::OnControlEditInit)
ON_FRESH_VI("eimpact", FRCMD_INIT, FrPlayerDlg::OnImpactEditInit)
ON_FRESH_VI("espin", FRCMD_INIT, FrPlayerDlg::OnSpinEditInit)
ON_FRESH_VI("ecurve", FRCMD_INIT, FrPlayerDlg::OnCurveEditInit)
END_FRESH_MSGMAP()

FrPlayerDlg::~FrPlayerDlg()
{
	HidePet();
}

void FrPlayerDlg::Init(int index)
{
	m_index = index;
	if (m_pIndex)
		m_pIndex->SetBgImg(MakeStr("%dp", index + 1));
	if (m_pRemove)
		m_pRemove->SetVisible(m_index >= 2);
	SetVisible(strlen(Doc()->m_userInfo[m_index].info.sID) != 0);
}

void FrPlayerDlg::ShowPet(bool bShow)
{
	HidePet();
	if (m_pPet && strlen(Doc()->m_userInfo[m_index].info.sID))
	{
		WRect rect = m_pPet->GetRect();
		m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.35f, true);
		m_pExhibition->SetDistanceRange(15.0f, 25.0f);
		m_pExhibition->SetDistance(20.0f, true, false);
		m_pExhibition->SetArea(rect.w, rect.h);
		m_pPartTidList = new CPartTidList;
		if (bShow)
			SetRandomState();
		else
			SetState();
	}
}

void FrPlayerDlg::HidePet()
{
	if (m_pExhibition)
	{
		delete (m_pExhibition);
		m_pExhibition = NULL;
	}
	if (m_pPartTidList)
	{
		delete m_pPartTidList;
		m_pPartTidList = NULL;
	}
}

void FrPlayerDlg::SetState()
{
	if (strlen(Doc()->m_userInfo[m_index].info.sID))
	{
		if (m_pName)
			m_pName->SetLine(1, Doc()->m_userInfo[m_index].info.sID, 0, false,
				0);
		SetCharControl(BD_CURRENT);
		SetCaddieControl(BD_CURRENT);
		SetClubControl(BD_CURRENT);
		SetBallControl(BD_CURRENT);
	}
}

void FrPlayerDlg::SetRandomState()
{
	if (m_pName)
		m_pName->SetLine(1, Doc()->m_userInfo[m_index].info.sID, 0, false, 0);
	SetCharControl(BD_RANDOM);
	SetCaddieControl(BD_RANDOM);
	SetClubControl(BD_RANDOM);
	SetBallControl(BD_RANDOM);
}

void FrPlayerDlg::OnCustomProc(float delta)
{
	if (m_pExhibition)
		m_pExhibition->Process(delta, true);
}

void FrPlayerDlg::OnPetOwnerDraw()
{
	if (m_pExhibition)
		m_pExhibition->Display(0.0f, 0.0f);
}

void FrPlayerDlg::SetTeam(unsigned char team)
{
	m_team = team;
	if (m_pBase)
	{
		switch (team)
		{
		case 0:
			m_pBase->SetButtonImg("team_base_red", FrButton::NORMAL);
			break;
		case 1:
			m_pBase->SetButtonImg("team_base_blue", FrButton::NORMAL);
			break;
		case 2:
			m_pBase->SetButtonImg("team_base", FrButton::NORMAL);
			break;
		}
	}
}

void FrPlayerDlg::OnBaseInit(int param)
{
	m_pBase = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrPlayerDlg::OnBaseBtnUp()
{
	AfxGetTask()->GetActor("Family")
		<< MsgObject(NULL, 0xe3, m_index, 0, 0, 0, 0);
}

void FrPlayerDlg::OnSelectInit(int param)
{
	m_pSelect = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pSelect->SetVisible(false);
}

void FrPlayerDlg::Select()
{
	if (m_pSelect)
		m_pSelect->SetVisible(true);
}

void FrPlayerDlg::UnSelect()
{
	if (m_pSelect)
		m_pSelect->SetVisible(false);
}

void FrPlayerDlg::OnPetInit(int param)
{
	m_pPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnCharPrevInit(int param)
{
	m_pCharPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCharPrev->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnCharPrevBtnUp()
{
	SetCharControl(BD_PREV);
}

void FrPlayerDlg::OnCharNextInit(int param)
{
	m_pCharNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCharNext->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnCharNextBtnUp()
{
	SetCharControl(BD_NEXT);
}

void FrPlayerDlg::OnCaddieInit(int param)
{
	m_pCaddie = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnCaddieLimitInit(int param)
{
	m_pCaddieLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnCaddiePrevInit(int param)
{
	m_pCaddiePrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCaddiePrev->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnCaddiePrevBtnUp()
{
	SetCaddieControl(BD_PREV);
}

void FrPlayerDlg::OnCaddieNextInit(int param)
{
	m_pCaddieNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCaddieNext->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnCaddieNextBtnUp()
{
	SetCaddieControl(BD_NEXT);
}

void FrPlayerDlg::OnClubInit(int param)
{
	m_pClub = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnClubLimitInit(int param)
{
	m_pClubLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnClubPrevInit(int param)
{
	m_pClubPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pClubPrev->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnClubPrevBtnUp()
{
	SetClubControl(BD_PREV);
}

void FrPlayerDlg::OnClubNextInit(int param)
{
	m_pClubNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pClubNext->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnClubNextBtnUp()
{
	SetClubControl(BD_NEXT);
}

void FrPlayerDlg::OnBallInit(int param)
{
	m_pBall = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::OnBallPrevInit(int param)
{
	m_pBallPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pBallPrev->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnBallPrevBtnUp()
{
	SetBallControl(BD_PREV);
}

void FrPlayerDlg::OnBallNextInit(int param)
{
	m_pBallNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pBallNext->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnBallNextBtnUp()
{
	SetBallControl(BD_NEXT);
}

void FrPlayerDlg::OnRemoveInit(int param)
{
	m_pRemove = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRemove->SetPushDelay(0.0f);
}

void FrPlayerDlg::OnRemoveBtnUp()
{
	SetVisible(false);
	if (m_pEmptyDlg)
		m_pEmptyDlg->SetVisible(true);
	Doc()->m_userInfo[m_index].info.sID[0] = 0;
	Doc()->m_userInfo[m_index].info.sNick[0] = 0;
	AfxGetTask()->GetActor("Family")
		<< MsgObject(NULL, 0xe2, m_index, 0, 0, 0, 0);
}

void FrPlayerDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

const char* FrPlayerDlg::GetName()
{
	if (m_pName)
		return m_pName->GetLine(1, false);
	return NULL;
}

void FrPlayerDlg::SetName(const char* name)
{
	if (m_pName)
		m_pName->SetLine(1, name, 0, false, 0);
	strcpy(Doc()->m_userInfo[m_index].info.sID, name);
	strcpy(Doc()->m_userInfo[m_index].info.sNick, name);
}

void FrPlayerDlg::OnIndexInit(int param)
{
	m_pIndex = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerDlg::SetLevelBar()
{
	unsigned long charTid = Doc()->m_userInfo[m_index].charInfo.tid;
	char* charPCL = Doc()->m_userInfo[m_index].charInfo.PCL;
	unsigned long charGuid = Doc()->m_userInfo[m_index].userEquip.guidChar;
	unsigned int index = m_index;
	unsigned long clubTid = Doc()->m_userInfo[index].clubInfo.tid;
	short* clubPCL = Doc()->m_userInfo[index].clubInfo.PCL;
	unsigned long caddieGuid = Doc()->m_userInfo[index].userEquip.guidCaddie;
	std::map<unsigned int, sCharacterInfo>::iterator it;
	it = Doc()->m_charMap.find(charGuid);
	unsigned long* parts = it->second.tidParts;
	unsigned long* auxParts = it->second.tidAuxParts;
	unsigned long caddieTid;
	if (!caddieGuid)
		caddieTid = 0;
	else
	{
		std::map<unsigned int, sCaddieInfo>::iterator caddie;
		caddie = Doc()->m_caddieMap.find(caddieGuid);
		caddieTid = caddie->second.tid;
	}
	unsigned char upgrade = Doc()->m_myInfo.stat.Level;
	int penalty = CalcPowerPenalty(upgrade);
	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(0xff);
		CCardManager::Instance()->CalcCardPeriodAndStatus();
	}
	for (int i = 0; i < 5; ++i)
	{
		int maximum = ItemManager()->GetLevel((eLvlType)i, charTid, charPCL,
			parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
		int level = maximum;
		if (i != 0)
		{
			if (i > 0 && i <= 2)
			{
				status_t status = ItemManager()->GetLevelWithPenalty(
					(eLvlType)i, charTid, charPCL, parts, auxParts, clubTid,
					clubPCL, caddieTid, upgrade);
				level = status.level;
			}
		}
		else
		{
			if (m_pLevelBar[i])
			{
				m_pLevelBar[1]->SetExpand(-max(level - penalty - 20, 0));
				m_pLevelBar[2]->SetExpand(-max(level - penalty - 20, 0));
			}
		}
		SetLevelNum((unsigned char)i, (unsigned char)level,
			(unsigned char)maximum);
		if (m_pLevelBar[i])
			m_pLevelBar[i]->SetDestPos(maximum, false);
	}
}

void FrPlayerDlg::SetLevelNum(unsigned char type, unsigned char level,
	unsigned char max)
{
	if (m_pLevelNum[type])
		m_pLevelNum[type]->SetLine(1,
			MakeStr("\\c0xffff0000\\c%d\\c0xff000000\\c/%d", level, max), 0,
			false, 0);
}

void FrPlayerDlg::SetCharControl(eButtonDir dir)
{
	std::map<unsigned int, sCharacterInfo>& items = Doc()->m_charMap;
	unsigned int size = items.size();
	if (size == 0)
		return;
	std::map<unsigned int, sCharacterInfo>::iterator it;
	if (dir == BD_RANDOM)
	{
		unsigned int n = rand() % size;
		for (it = items.begin(); it != items.end() && n; ++it, --n)
		{
		}
	}
	else
		it = items.find(Doc()->m_userInfo[m_index].userEquip.guidChar);
	switch (dir)
	{
	case BD_PREV:
		--it;
		break;
	case BD_NEXT:
		++it;
		break;
	}
	Doc()->m_userInfo[m_index].userEquip.guidChar = it->second.guid;
	Doc()->m_userInfo[m_index].charInfo = it->second;
	SetCharacter();
	SetLevelBar();
	if (m_pCharPrev)
		m_pCharPrev->Enable(it != items.begin());
	if (m_pCharNext)
		m_pCharNext->Enable(++it != items.end());
}

void FrPlayerDlg::SetCaddieControl(eButtonDir dir)
{
	std::map<unsigned int, sCaddieInfo>& items = Doc()->m_caddieMap;
	unsigned int size = items.size();
	if (size == 0)
	{
		Doc()->m_userInfo[m_index].userEquip.guidCaddie = 0;
		SetCaddie(0);
		if (m_pCaddiePrev)
			m_pCaddiePrev->Enable(false);
		if (m_pCaddieNext)
			m_pCaddieNext->Enable(false);
		return;
	}
	unsigned long guid;
	std::map<unsigned int, sCaddieInfo>::iterator it;
	if (dir == BD_RANDOM)
	{
		guid = rand() % size;
		for (it = items.begin(); it != items.end() && guid; ++it, --guid)
		{
		}
	}
	else
	{
		guid = Doc()->m_userInfo[m_index].userEquip.guidCaddie;
		it = items.find(guid);
	}
	switch (dir)
	{
	case BD_PREV:
		if (it == items.begin())
		{
			Doc()->m_userInfo[m_index].userEquip.guidCaddie = 0;
			SetCaddie(0);
			if (m_pCaddiePrev)
				m_pCaddiePrev->Enable(false);
		}
		else
		{
			--it;
			Doc()->m_userInfo[m_index].caddieInfo = it->second;
			SetCaddie(it->second.tid);
		}
		if (m_pCaddieNext)
			m_pCaddieNext->Enable(true);
		break;
	case BD_NEXT:
		if (!guid)
			it = items.begin();
		else
			++it;
		if (it == items.end())
			return;
		Doc()->m_userInfo[m_index].caddieInfo = it->second;
		SetCaddie(it->second.tid);
		if (m_pCaddiePrev)
			m_pCaddiePrev->Enable(true);
		if (m_pCaddieNext)
			m_pCaddieNext->Enable(++it != items.end());
		break;
	case BD_CURRENT:
		if (!guid)
		{
			SetCaddie(0);
			if (m_pCaddiePrev)
				m_pCaddiePrev->Enable(false);
			if (m_pCaddieNext)
				m_pCaddieNext->Enable(items.size() > 0);
		}
		else
		{
			SetCaddie(it->second.tid);
			if (m_pCaddiePrev)
				m_pCaddiePrev->Enable(true);
			if (m_pCaddieNext)
				m_pCaddieNext->Enable(++it != items.end());
		}
		break;
	case BD_RANDOM:
		Doc()->m_userInfo[m_index].caddieInfo = it->second;
		SetCaddie(it->second.tid);
		if (m_pCaddiePrev)
			m_pCaddiePrev->Enable(true);
		if (m_pCaddieNext)
			m_pCaddieNext->Enable(++it != items.end());
		break;
	default:
		return;
	}
	SetLevelBar();
}

void FrPlayerDlg::SetClubControl(eButtonDir dir)
{
	unsigned int size = Doc()->m_clubSetMap.size();
	if (size == 0)
		return;
	std::map<unsigned int, sItemInfo>::iterator it;
	if (dir == BD_RANDOM)
	{
		unsigned int n = rand() % size;
		for (it = Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end() && n; ++it, --n)
		{
		}
	}
	else
		it = Doc()->m_clubSetMap.find(Doc()->m_userInfo[m_index].clubInfo.guid);
	switch (dir)
	{
	case BD_PREV:
		--it;
		break;
	case BD_NEXT:
		++it;
		break;
	}
	Doc()->m_userInfo[m_index].clubInfo.tid = it->second.tid;
	Doc()->m_userInfo[m_index].clubInfo.guid = it->second.guid;
	memcpy(Doc()->m_userInfo[m_index].clubInfo.PCL, it->second.Common,
		sizeof(it->second.Common));
	SetClub(it->second.tid);
	SetLevelBar();
	if (m_pClubPrev)
		m_pClubPrev->Enable(it != Doc()->m_clubSetMap.begin());
	if (m_pClubNext)
		m_pClubNext->Enable(++it != Doc()->m_clubSetMap.end());
}

void FrPlayerDlg::SetBallControl(eButtonDir dir)
{
	std::list<sItemInfo>& items = Doc()->m_ballList;
	unsigned int size = items.size();
	if (size == 0)
		return;
	std::list<sItemInfo>::iterator it;
	if (dir == BD_RANDOM)
	{
		unsigned int n = rand() % size;
		for (it = items.begin(); it != items.end() && n; ++it, --n)
		{
		}
	}
	else
	{
		unsigned long tid = Doc()->m_userInfo[m_index].userEquip.tidBall;
		for (it = items.begin(); it != items.end() && it->tid != tid; ++it)
		{
		}
	}
	switch (dir)
	{
	case BD_PREV:
		--it;
		break;
	case BD_NEXT:
		++it;
		break;
	}
	Doc()->m_userInfo[m_index].userEquip.tidBall = it->tid;
	SetBall(it->tid);
	SetLevelBar();
	m_pBallNext->Enable(++it != items.end());
	if (m_pBallPrev)
		m_pBallPrev->Enable(it != items.begin());
}

void FrPlayerDlg::SetCharacter()
{
	if (m_pPartTidList)
		m_pPartTidList->SetTids(&Doc()->m_userInfo[m_index].charInfo, 0xff);
	if (m_pExhibition)
		m_pExhibition->SetModel(*m_pPartTidList,
			Doc()->m_userInfo[m_index].charInfo.tidAuxParts, NULL, 3.0f,
			m_index);
}

void FrPlayerDlg::SetCaddie(unsigned long guid)
{
	if (!m_pCaddie)
		return;
	Doc()->m_userInfo[m_index].caddieInfo.tid = guid;
	IFF_STRUCT::sCaddie* item = ItemManager()->FindCaddie(guid);
	if (item)
		m_pCaddie->SetBgImg(item->c.Icon);
	else
		m_pCaddie->SetBgImg("caddie_no");
	if (m_pCaddieLimit)
	{
		if (item)
		{
			if (Doc()->m_myInfo.stat.Level < item->c.Level)
				m_pCaddieLimit->SetBgImg("unused");
			else
				m_pCaddieLimit->SetBgImg("caddie_rest");
			guid = Doc()->m_userInfo[m_index].caddieInfo.guid;
			Doc()->m_userInfo[m_index].userEquip.guidCaddie = guid;
			if (guid)
			{
				std::map<unsigned int, sCaddieInfo>& items = Doc()->m_caddieMap;
				std::map<unsigned int, sCaddieInfo>::iterator it;
				it = items.find(guid);
				if (it != items.end())
				{
					IFF_STRUCT::sCaddie* caddie =
						ItemManager()->FindCaddie(it->second.tid);
					if (caddie)
					{
						if ((it->second.Rent_flag &&
								it->second.Remain_Date == 0 &&
								caddie->MonthlyFee > 0) ||
							Doc()->m_myInfo.stat.Level < caddie->c.Level)
							m_pCaddieLimit->SetVisible(true);
						else
							m_pCaddieLimit->SetVisible(false);
					}
				}
				else
					m_pCaddieLimit->SetVisible(false);
			}
			else
				m_pCaddieLimit->SetVisible(false);
		}
		else
			m_pCaddieLimit->SetVisible(false);
	}
}

void FrPlayerDlg::SetClub(unsigned long guid)
{
	if (m_pClub)
	{
		IFF_STRUCT::sClubSet* item = ItemManager()->FindClubSet(guid);
		if (item)
		{
			m_pClub->SetBgImg(item->c.Icon);
			if (m_pClubLimit)
				m_pClubLimit->SetVisible(
					Doc()->m_myInfo.stat.Level < item->c.Level);
		}
	}
}

void FrPlayerDlg::SetBall(unsigned long tid)
{
	if (m_pBall)
	{
		IFF_STRUCT::sBall* item = ItemManager()->FindBall(tid);
		if (item && m_pBall)
			m_pBall->SetBgImg(item->c.Icon);
	}
}

void FrPlayerDlg::OnPowerBarInit(int param)
{
	m_pLevelBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
}

void FrPlayerDlg::OnControlBarInit(int param)
{
	m_pLevelBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pLevelBar[1]->SetRange(0, 30, 0);
}

void FrPlayerDlg::OnImpactBarInit(int param)
{
	m_pLevelBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pLevelBar[2]->SetRange(0, 30, 0);
}

void FrPlayerDlg::OnSpinBarInit(int param)
{
	m_pLevelBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pLevelBar[3]->SetRange(0, 30, 0);
}

void FrPlayerDlg::OnCurveBarInit(int param)
{
	m_pLevelBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pLevelBar[4]->SetRange(0, 30, 0);
}

void FrPlayerDlg::OnPowerEditInit(int param)
{
	m_pLevelNum[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPlayerDlg::OnControlEditInit(int param)
{
	m_pLevelNum[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPlayerDlg::OnImpactEditInit(int param)
{
	m_pLevelNum[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPlayerDlg::OnSpinEditInit(int param)
{
	m_pLevelNum[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrPlayerDlg::OnCurveEditInit(int param)
{
	m_pLevelNum[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

IMPLEMENT_OBJECT(FrPlayerEmptyDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrPlayerEmptyDlg, FrForm)
ON_FRESH_VI("base", FRCMD_INIT, FrPlayerEmptyDlg::OnBaseInit)
ON_FRESH_VI("create", FRCMD_INIT, FrPlayerEmptyDlg::OnCreateInit)
ON_FRESH_VV("create", FRCMD_LBUTTONUP, FrPlayerEmptyDlg::OnCreateBtnUp)
ON_FRESH_VI("index", FRCMD_INIT, FrPlayerEmptyDlg::OnIndexInit)
END_FRESH_MSGMAP()

void FrPlayerEmptyDlg::Init(int index)
{
	m_index = index;
	if (m_pIndex)
		m_pIndex->SetBgImg(MakeStr("%dp", index + 1));
	SetVisible(strlen(Doc()->m_userInfo[m_index].info.sID) == 0);
}

void FrPlayerEmptyDlg::OnBaseInit(int param)
{
	m_pBase = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrPlayerEmptyDlg::OnCreateInit(int param)
{
	m_pCreate = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCreate->SetPushDelay(0.0f);
}

void FrPlayerEmptyDlg::OnCreateBtnUp()
{
	SetVisible(false);
	if (m_pPlayerDlg)
	{
		sprintf(Doc()->m_userInfo[m_index].info.sID, "PLAYER%d", m_index + 1);
		sprintf(Doc()->m_userInfo[m_index].info.sNick, "PLAYER%d", m_index + 1);
		m_pPlayerDlg->ShowPet(true);
		m_pPlayerDlg->SetVisible(true);
	}
	AfxGetTask()->GetActor("Family")
		<< MsgObject(NULL, 0xe2, m_index, 1, 0, 0, 0);
	AfxGetTask()->GetActor("Family")
		<< MsgObject(NULL, 0xe3, m_index, 0, 0, 0, 0);
}

void FrPlayerEmptyDlg::OnIndexInit(int param)
{
	m_pIndex = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}
