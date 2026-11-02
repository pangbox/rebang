#include "stdafx.h"

#include "familymain.h"
#include "shareddoc.h"
#include "treasuredlg.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "frarea.h"
#include "frbutton.h"
#include "frviewer.h"
#include "frlistbox.h"
#include "objectfactory.h"
#include "actor.h"
#include "../../shared/globalgamedefine.h"

#include "equipdlg.h"
#include "exhibition.h"
#include "playerdlg.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "commonutil.h"
#include "mousecursor.h"
#include "uccmanager.h"
#include "inputmanager.h"

#include <algorithm>
#include <string>
#include "../../shared/itemmanager.h"
#include "../../shared/classdefine.h"
#include "frdesktop.h"
#include "rankingdlg.h"
#include "rankinginfo.h"
#include "user_info.h"
#include "parttidlist.h"
#include "cardsystem.h"
#include "wview.h"
#include "taskmanager.h"
#include "packet.h"

unsigned long CFamilyMain::ms_itemslotBackup[4][10];

CFamilyMain::cFamily::cFamily(void)
	: m_exhibition(0),
	  m_partTidList(0),
	  m_charInfo(),
	  m_statBar(0),
	  m_petKind(0)
{
	memset(&m_charInfo, 0, sizeof(m_charInfo));
	memset(&m_userEquip, 0, sizeof(m_userEquip));
	memset(&m_mascot, 0, sizeof(m_mascot));
	memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
	memset(&m_itemInfo, 0, sizeof(m_itemInfo));
}

CFamilyMain::cFamily::~cFamily(void)
{
	if (m_exhibition)
	{
		delete m_exhibition;
		m_exhibition = 0;
	}
	if (m_partTidList)
	{
		delete m_partTidList;
		m_partTidList = 0;
	}
	if (m_statBar)
	{
		delete m_statBar;
		m_statBar = 0;
	}
}

void CFamilyMain::cFamily::SetItemSlot(unsigned long* pItemSlot)
{
	memcpy(m_userEquip.tidItemSlot, pItemSlot, sizeof(m_userEquip.tidItemSlot));
}

void CFamilyMain::cFamily::ConvertUserInfo(sUserInfo& info)
{
	info.roomIndex = 0;
	memcpy(&info.caddieInfo, &m_caddieInfo, sizeof(m_caddieInfo));
	memcpy(&info.charInfo, &m_charInfo, sizeof(m_charInfo));

	sClubInfo clubSet;
	clubSet.guid = m_itemInfo.guid;
	clubSet.tid = m_itemInfo.tid;

	memcpy(clubSet.PCL, m_itemInfo.Common, sizeof(clubSet.PCL));
	info.clubInfo = clubSet;

	memcpy(info.info.sID, &Doc()->GetPangyaUserInfo(), 0x10D);

	memcpy(info.mapStat, Doc()->m_myInfo.mapStat, sizeof(info.mapStat));
	info.mascotInfo = m_mascot;

	memcpy(&info.stat, &Doc()->m_myInfo.stat, sizeof(sPangYaUserStatistics));
	memcpy(&info.trophy, &Doc()->m_myInfo.trophy, sizeof(info.trophy));
	memcpy(&info.userEquip, &m_userEquip, sizeof(m_userEquip));
}

const WPoint& CFamilyMain::cFamily::GetFramePos(void) const
{
	return m_framePos;
}

const sUserEquip& CFamilyMain::cFamily::GetUserEquip(void) const
{
	return m_userEquip;
}

void CFamilyMain::cFamily::SetUserEquip(int category, unsigned long id)
{
	switch (category)
	{
	case 1:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			WSingleton<CSharedDoc>::Instance()->m_charMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_charMap.end())
		{
			m_userEquip.guidChar = id;
			m_charInfo = it->second;
			if (IsLocalContent((localContentType_t)0x51) &&
				IsLocalContent((localContentType_t)0x64))
			{
				WSingleton<CCardManager>::Instance()
					->m_charInfo[m_petKind + 1] = m_charInfo;
			}
			PetRebuild();
		}
		return;
	}

	case 0x10:
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			WSingleton<CSharedDoc>::Instance()->m_mascotMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_mascotMap.end())
		{
			m_userEquip.guidMascot = id;
			m_mascot = it->second;
		}
		else
		{
			memset(&m_mascot, 0, sizeof(m_mascot));
		}
		break;
	}

	case 7:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
		{
			m_userEquip.guidCaddie = id;
			m_caddieInfo = it->second;
		}
		else
		{
			memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
		}
		break;
	}

	case 5:
		m_userEquip.tidBall = id;
		return;

	case 4:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			WSingleton<CSharedDoc>::Instance()->m_clubSetMap.find(id);
		if (it == WSingleton<CSharedDoc>::Instance()->m_clubSetMap.end())
		{
			return;
		}
		m_userEquip.guidClubSet = id;
		m_itemInfo = it->second;
		break;
	}

	default:
		return;
	}

	SetStatus();
}

void CFamilyMain::cFamily::Init(const WRect& rect, std::vector<int>& petList,
	unsigned char petKind)
{
	m_rect = rect;
	const Bitmap* frame = g_pFresh->GetBitmap("family_info_frm");
	if (frame)
	{
		m_framePos.x = m_rect.x + (m_rect.w - frame->Width()) * 0.5f;
		m_framePos.y = m_rect.y + m_rect.h - frame->Height();
	}

	m_modelPos.x = m_framePos.x + 45.0f;
	m_modelPos.y = m_framePos.y + 50.0f;
	m_petKind = petKind;

	MakePet(petList);
}

void CFamilyMain::cFamily::Process(float dt)
{
	if (!m_exhibition)
	{
		return;
	}

	m_statBar->Process(dt);

	FrWnd* shop =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (!(shop != NULL ? shop : (__rtti_obj = shop)))
	{
		m_exhibition->SetEnableRotate(true);
		if (g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"treasuremap") ||
			CRankingInfo::Instance()->GetDlg() ||
			(CUserInfo::Instance() && CUserInfo::Instance()->GetDlg()))
		{
			m_exhibition->SetEnableRotate(false);
		}

		m_exhibition->Process(dt, false);
	}
}

void CFamilyMain::cFamily::Render(void)
{
	FrWnd* shop =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (!(shop != NULL ? shop : (__rtti_obj = shop)))
	{
		if (!m_anotherHand.IsWorking(0) &&
			!CRankingInfo::Instance()->GetDlg() &&
			!(CUserInfo::Instance() && CUserInfo::Instance()->GetDlg()))
		{
			// HACK! Force out-of-line call here to match the original.
#pragma inline_depth(0)
			m_anotherHand.Enter();
#pragma inline_depth()
			if (m_exhibition)
			{
				m_exhibition->Display(0.0f, 0.0f);
			}
			m_anotherHand.Leave();
		}
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
	{
		return;
	}

	const Bitmap* frame = g_pFresh->GetBitmap("family_info_frm");
	if (frame)
	{
		pGDI->DrawTexture(frame,
			WRect(m_framePos,
				WSize((float)frame->Width(), (float)frame->Height())),
			0xFFFFFFFF, 0);
	}

	const Bitmap* charFrame = g_pFresh->GetBitmap("family_info_char");
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(m_charInfo.tid);
	if (item && charFrame)
	{
		const Bitmap* text =
			g_pFresh->GetBitmap(MakeStr("text_%s", item->Icon));
		if (text)
		{
			WPoint pos = m_framePos;
			pos.y -= 30.0f;
			pos.x += (m_rect.w - charFrame->Width()) * 0.5f;
			pGDI->DrawTexture(charFrame,
				WRect(pos.x, pos.y, (float)charFrame->Width(),
					(float)charFrame->Height()),
				0xFFFFFFFF, 0);

			pos.y += (charFrame->Height() - text->Height()) * 0.5f;
			pos.x += (charFrame->Width() - text->Width()) * 0.5f;
			pGDI->DrawTexture(text,
				WRect(pos.x, pos.y, (float)text->Width(),
					(float)text->Height()),
				0xFFFFFFFF, 0);
		}
	}

	m_statBar->Render(m_modelPos);
}

void CFamilyMain::cFamily::MakePet(std::vector<int>& petList)
{
	static int index = 0;

	int count = (int)WSingleton<CSharedDoc>::Instance()->m_charMap.size();
	if (++index >= count)
	{
		// HACK!
		*(volatile int*)&index = 0;
		std::random_shuffle(petList.begin(), petList.end());
	}

	int pick = petList[index];

	std::map<unsigned int, sCharacterInfo>::iterator it =
		WSingleton<CSharedDoc>::Instance()->m_charMap.begin();
	for (int i = 0; i < pick; i++)
	{
		++it;
	}

	sCharacterInfo* pCharInfo = &it->second;
	m_charInfo = *pCharInfo;
	m_userEquip = WSingleton<CSharedDoc>::Instance()->m_myInfo.userEquip;

	m_userEquip.guidChar = pCharInfo->guid;
	m_userEquip.guidCaddie = CEquipHelper::Instance()->GetEquipItemGuid(
		CEquipHelper::CATEGORY_CADDIE, true);
	m_userEquip.guidClubSet = CEquipHelper::Instance()->GetEquipItemGuid(
		CEquipHelper::CATEGORY_CLUB, true);
	m_userEquip.tidBall = CEquipHelper::Instance()->GetEquipItemTid(
		CEquipHelper::CATEGORY_BALL, true);
	m_userEquip.guidMascot = CEquipHelper::Instance()->GetEquipItemGuid(
		CEquipHelper::CATEGORY_MASCOT, true);

	std::map<unsigned int, sItemInfo>::iterator itItem =
		WSingleton<CSharedDoc>::Instance()->m_clubSetMap.find(
			m_userEquip.guidClubSet);
	if (itItem != WSingleton<CSharedDoc>::Instance()->m_clubSetMap.end())
	{
		m_itemInfo = itItem->second;
	}

	std::map<unsigned int, sCaddieInfo>::iterator itCaddie =
		WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(
			m_userEquip.guidCaddie);
	if (itCaddie != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
	{
		m_caddieInfo = itCaddie->second;
	}

	std::map<unsigned int, sMascotInfo>::iterator itMascot =
		WSingleton<CSharedDoc>::Instance()->m_mascotMap.find(
			m_userEquip.guidMascot);
	if (itMascot != WSingleton<CSharedDoc>::Instance()->m_mascotMap.end())
	{
		m_mascot = itMascot->second;
	}

	if (IsLocalContent((localContentType_t)0x51) &&
		IsLocalContent((localContentType_t)0x64))
	{
		unsigned char slot = m_petKind + 1;
		WSingleton<CCardManager>::Instance()->m_charInfo[slot] = m_charInfo;
		WSingleton<CCardManager>::Instance()->m_playerSlot[slot] = slot;
	}

	SetStatus();

	m_anotherHand.Enter();

	m_exhibition = new CExhibition(m_rect.x + m_rect.w * 0.5f,
		m_rect.y + m_rect.h * 0.5f - 70.0f, 0.35f, false);
	m_exhibition->SetArea(m_rect.w, m_rect.h);

	if (pCharInfo->tid == 0x4000005 || pCharInfo->tid == 0x4000009)
	{
		m_exhibition->SetDistance(11.0f, true, false);
	}
	else
	{
		m_exhibition->SetDistance(13.0f, true, false);
	}

	m_partTidList = new CPartTidList;
	if (!m_partTidList->SetTids(pCharInfo, 0xFF) ||
		!m_partTidList->IsComboValid())
	{
		m_partTidList->SetDefaultTids();
	}

	m_exhibition->SetModel(*m_partTidList, pCharInfo->tidAuxParts, 0, 3.0f, 0);
	if (WSingleton<CSharedDoc>::Instance()
			->GetPangyaUserInfo()
			.angelicWingsEffect)
	{
		m_exhibition->AttachAngelWingFx();
	}

	m_anotherHand.Leave();
}

void CFamilyMain::cFamily::SetStatus(void)
{
	if (IsLocalContent((localContentType_t)0x51) &&
		IsLocalContent((localContentType_t)0x64))
	{
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(m_petKind);
	}

	if (m_statBar)
	{
		delete m_statBar;
		m_statBar = 0;
	}
	m_statBar = new CStatBar(9.0f, 6.0f);
	CItemManager* pItemManager = ItemManager();
	unsigned char level[5] = { 0 }, penalty[5] = { 0 }, capacity[5] = { 0 };

	for (int i = 0; i < 5; i++)
	{
		status_t status = pItemManager->GetLevelWithPenalty((eLvlType)i,
			m_charInfo.tid, (const char*)&m_charInfo.PCL, m_charInfo.tidParts,
			m_charInfo.tidAuxParts, m_itemInfo.tid, m_itemInfo.Common,
			m_caddieInfo.tid,
			WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.Level);
		level[i] = status.level;
		penalty[i] = status.penalty;
		capacity[i] = pItemManager->GetCapacity((eLvlType)i, m_charInfo.tid,
			(const char*)&m_charInfo.PCL, m_charInfo.tidParts, m_itemInfo.tid,
			m_itemInfo.Common, m_caddieInfo.tid,
			WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.Level);
	}

	m_statBar->SetDestTime(1.2f);
	for (int i = 0; i < 5; i++)
	{
		m_statBar->SetRange((eLvlType)i, level[i], capacity[i], penalty[i]);
	}
}

void CFamilyMain::cFamily::PetRebuild(void)
{
	if (m_exhibition)
	{
		delete m_exhibition;
		m_exhibition = 0;
	}
	if (m_partTidList)
	{
		delete m_partTidList;
		m_partTidList = 0;
	}
	if (m_statBar)
	{
		delete m_statBar;
		m_statBar = 0;
	}

	SetStatus();

	m_anotherHand.Enter();

	m_exhibition = new CExhibition(m_rect.x + m_rect.w * 0.5f,
		m_rect.y + m_rect.h * 0.5f - 70.0f, 0.35f, false);
	m_exhibition->SetArea(m_rect.w, m_rect.h);

	if (m_charInfo.tid == 0x4000005 || m_charInfo.tid == 0x4000009)
	{
		m_exhibition->SetDistance(11.0f, true, false);
	}
	else
	{
		m_exhibition->SetDistance(13.0f, true, false);
	}

	m_partTidList = new CPartTidList;
	if (!m_partTidList->SetTids(&m_charInfo, 0xFF) ||
		!m_partTidList->IsComboValid())
	{
		m_partTidList->SetDefaultTids();
	}

	m_exhibition->SetModel(*m_partTidList, m_charInfo.tidAuxParts, 0, 3.0f, 0);
	if (WSingleton<CSharedDoc>::Instance()
			->GetPangyaUserInfo()
			.angelicWingsEffect)
	{
		m_exhibition->AttachAngelWingFx();
	}

	m_anotherHand.Leave();
}

IMPLEMENT_ACTOR(CFamilyMain, CTaskMain)

// HACK! m_mapIndex is unexpectedly in middle of the msgmap syms.
const sFRESH_MSGMAP* CFamilyMain::GetMessageMap() const
{
	return &CFamilyMain::_MsgMap;
}

sFRESH_MSGMAP CFamilyMain::_MsgMap = { &CTaskMain::_MsgMap,
	&CFamilyMain::_MsgEntries[0] };
unsigned char CFamilyMain::m_mapIndex = 0xFF;
// clang-format off
sFRESH_ENTRY CFamilyMain::_MsgEntries[] = {
ON_FRESH_VI("FAMILY", FRCMD_INIT, CFamilyMain::OnInitFamilyMain)
ON_FRESH_VV("FAMILY", FRCMD_FINISH, CFamilyMain::OnInitFamilyFinish)
ON_FRESH_VI("FAMILY.pet_area1", FRCMD_INIT, CFamilyMain::OnInitAreaPet1)
ON_FRESH_VI("FAMILY.pet_area2", FRCMD_INIT, CFamilyMain::OnInitAreaPet2)
ON_FRESH_VI("FAMILY.pet_area3", FRCMD_INIT, CFamilyMain::OnInitAreaPet3)
ON_FRESH_VI("FAMILY.pet_area4", FRCMD_INIT, CFamilyMain::OnInitAreaPet4)
ON_FRESH_VI("FAMILY.globalgui", FRCMD_INIT, CFamilyMain::OnInitAreaGlobal)
ON_FRESH_VV("FAMILY.pet_area1", FRCMD_OWNERDRAW, CFamilyMain::OnDrawAreaPet1)
ON_FRESH_VV("FAMILY.pet_area2", FRCMD_OWNERDRAW, CFamilyMain::OnDrawAreaPet2)
ON_FRESH_VV("FAMILY.pet_area3", FRCMD_OWNERDRAW, CFamilyMain::OnDrawAreaPet3)
ON_FRESH_VV("FAMILY.pet_area4", FRCMD_OWNERDRAW, CFamilyMain::OnDrawAreaPet4)
ON_FRESH_VV("FAMILY.globalgui", FRCMD_OWNERDRAW, CFamilyMain::OnDrawAreaGlobal)
ON_FRESH_VI("FAMILY.start_btn", FRCMD_INIT, CFamilyMain::OnInitBtnStart)
ON_FRESH_VI("FAMILY.course_btn", FRCMD_INIT, CFamilyMain::OnInitBtnSelMap)
ON_FRESH_VI("FAMILY.limit_time_btn", FRCMD_INIT, CFamilyMain::OnInitBtnHoleType)
ON_FRESH_VI("FAMILY.hole_type_btn", FRCMD_INIT, CFamilyMain::OnInitBtnHoleNum)
ON_FRESH_VI("FAMILY.hole_num_btn", FRCMD_INIT, CFamilyMain::OnInitBtnTimeLimit)
ON_FRESH_VI("FAMILY.create_pet3", FRCMD_INIT, CFamilyMain::OnInitCreatePet3)
ON_FRESH_VI("FAMILY.create_pet4", FRCMD_INIT, CFamilyMain::OnInitCreatePet4)
ON_FRESH_VI("FAMILY.delete_pet3", FRCMD_INIT, CFamilyMain::OnInitDeletePet3)
ON_FRESH_VI("FAMILY.delete_pet4", FRCMD_INIT, CFamilyMain::OnInitDeletePet4)
ON_FRESH_VV("FAMILY.start_btn", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownBtnStart)
ON_FRESH_VV("FAMILY.course_btn", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownBtnSelMap)
ON_FRESH_VV("FAMILY.limit_time_btn", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownBtnHoleType)
ON_FRESH_VV("FAMILY.hole_type_btn", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownBtnHoleNum)
ON_FRESH_VV("FAMILY.hole_num_btn", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownBtnTimeLimit)
ON_FRESH_VV("FAMILY.create_pet3", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownCreatePet3)
ON_FRESH_VV("FAMILY.create_pet4", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownCreatePet4)
ON_FRESH_VV("FAMILY.delete_pet3", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownDeletePet3)
ON_FRESH_VV("FAMILY.delete_pet4", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownDeletePet4)
ON_FRESH_VV("FAMILY.start_btn", FRCMD_OWNERDRAW, CFamilyMain::OnDrawBtnStart)
ON_FRESH_VV("FAMILY.course_btn", FRCMD_OWNERDRAW, CFamilyMain::OnDrawBtnSelMap)
ON_FRESH_VV("FAMILY.limit_time_btn", FRCMD_OWNERDRAW, CFamilyMain::OnDrawBtnHoleType)
ON_FRESH_VV("FAMILY.hole_type_btn", FRCMD_OWNERDRAW, CFamilyMain::OnDrawBtnHoleNum)
ON_FRESH_VV("FAMILY.hole_num_btn", FRCMD_OWNERDRAW, CFamilyMain::OnDrawBtnTimeLimit)
ON_FRESH_VI("FAMILY.pet_equip1", FRCMD_INIT, CFamilyMain::OnInitListEquip1)
ON_FRESH_VI("FAMILY.pet_equip2", FRCMD_INIT, CFamilyMain::OnInitListEquip2)
ON_FRESH_VI("FAMILY.pet_equip3", FRCMD_INIT, CFamilyMain::OnInitListEquip3)
ON_FRESH_VI("FAMILY.pet_equip4", FRCMD_INIT, CFamilyMain::OnInitListEquip4)
ON_FRESH_VI("FAMILY.pet_equip1", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownListEquip1)
ON_FRESH_VI("FAMILY.pet_equip2", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownListEquip2)
ON_FRESH_VI("FAMILY.pet_equip3", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownListEquip3)
ON_FRESH_VI("FAMILY.pet_equip4", FRCMD_LBUTTONDOWN, CFamilyMain::OnDownListEquip4)
ON_FRESH_VI("FAMILY.pet_equip1", FRCMD_OWNERDRAW, CFamilyMain::OnDrawListEquip1)
ON_FRESH_VI("FAMILY.pet_equip2", FRCMD_OWNERDRAW, CFamilyMain::OnDrawListEquip2)
ON_FRESH_VI("FAMILY.pet_equip3", FRCMD_OWNERDRAW, CFamilyMain::OnDrawListEquip3)
ON_FRESH_VI("FAMILY.pet_equip4", FRCMD_OWNERDRAW, CFamilyMain::OnDrawListEquip4)
ON_FRESH_VI("FAMILY.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("FAMILY.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("FAMILY.ubar.back", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("FAMILY.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("FAMILY.ubar.front", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("FAMILY.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("FAMILY.ubar.shop", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("FAMILY.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.myroom", FRCMD_INIT, CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("FAMILY.ubar.myroom", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("FAMILY.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.itemstorage", FRCMD_INIT, CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("FAMILY.ubar.itemstorage", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("FAMILY.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.itemstorage", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("FAMILY.ubar.gift", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("FAMILY.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.cookie", FRCMD_INIT, CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("FAMILY.ubar.cookie", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("FAMILY.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("FAMILY.ubar.game", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("FAMILY.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.server", FRCMD_INIT, CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("FAMILY.ubar.server", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("FAMILY.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("FAMILY.ubar.exit", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("FAMILY.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.ubar.tabbtn_b", FRCMD_INIT, CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("FAMILY.ubar.tabbtn_b", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_TabBtnUp)
ON_FRESH_VI("FAMILY.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("FAMILY.utab.magicbox", FRCMD_INIT, CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("FAMILY.utab.magicbox", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("FAMILY.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.tikimagicbox", FRCMD_INIT, CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("FAMILY.utab.tikimagicbox", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("FAMILY.utab.tikimagicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.tikimagicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.scratch", FRCMD_INIT, CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("FAMILY.utab.scratch", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("FAMILY.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.bongdarishop", FRCMD_INIT, CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("FAMILY.utab.bongdarishop", FRCMD_LBUTTONDOWN, CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("FAMILY.utab.bongdarishop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.bongdarishop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.ranking", FRCMD_INIT, CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("FAMILY.utab.ranking", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("FAMILY.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("FAMILY.utab.guild", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("FAMILY.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.notice", FRCMD_INIT, CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("FAMILY.utab.notice", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("FAMILY.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("FAMILY.utab.maket", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("FAMILY.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("FAMILY.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("FAMILY.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("FAMILY.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("FAMILY.obar.char", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("FAMILY.obar.char", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("FAMILY.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("FAMILY.obar.caddie", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("FAMILY.obar.caddie", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("FAMILY.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("FAMILY.obar.club", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("FAMILY.obar.club", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("FAMILY.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("FAMILY.obar.aztec", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("FAMILY.obar.aztec", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("FAMILY.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("FAMILY.obar.mascot", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("FAMILY.obar.mascot", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("FAMILY.obar.char_sel", FRCMD_INIT, CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("FAMILY.obar.char_sel", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("FAMILY.obar.caddie_sel", FRCMD_INIT, CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("FAMILY.obar.caddie_sel", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("FAMILY.obar.club_sel", FRCMD_INIT, CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("FAMILY.obar.club_sel", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("FAMILY.obar.aztec_sel", FRCMD_INIT, CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("FAMILY.obar.aztec_sel", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("FAMILY.obar.mascot_sel", FRCMD_INIT, CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("FAMILY.obar.mascot_sel", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("FAMILY.obar.pang_cookie", FRCMD_INIT, CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("FAMILY.obar.pang_cookie", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("FAMILY.obar.onelinemsg_req", FRCMD_INIT, CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("FAMILY.obar.onelinemsg_req", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("FAMILY.obar.onelineview", FRCMD_INIT, CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("FAMILY.obar.onelineview", FRCMD_OWNERDRAW, CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("FAMILY.obar.spcard", FRCMD_INIT, CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("FAMILY.obar.spcard", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("FAMILY.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.obar.messenger", FRCMD_INIT, CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("FAMILY.obar.messenger", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("FAMILY.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("FAMILY.obar.myinfo", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("FAMILY.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("FAMILY.obar.option", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("FAMILY.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("FAMILY.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("FAMILY.obar.messenger_bubble", FRCMD_INIT, CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("FAMILY.obar.messenger_bubble", FRCMD_OWNERDRAW, CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("FAMILY.obar.messenger_desc", FRCMD_INIT, CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("FAMILY.obar.messenger_alarm", FRCMD_INIT, CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("FAMILY.obar.messenger_alarm", FRCMD_LBUTTONUP, CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("FAMILY.obar.gift_alarm", FRCMD_INIT, CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("FAMILY.obar.gift_alarm", FRCMD_LBUTTONUP, CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("FAMILY.obar.tooltip", FRCMD_OWNERDRAW, CTaskMain::OnBarTooltipOwnerDraw)
{ 0, FRCMD_NONE, FrFuncNone, 0 }
};
// clang-format on

void CFamilyMain::OnInitFamilyMain(int param)
{
	SetTimeVariableBGM();
	TurnOff3DBackground();

	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;

	m_families.reserve(4);
	m_areas.reserve(5);
	m_buttons.reserve(9);
	m_equipLists.reserve(4);

	m_families.assign(4, (cFamily*)0);
	m_areas.assign(5, (FrArea*)0);
	m_buttons.assign(9, (FrButton*)0);
	m_equipLists.assign(4, (FrListBox*)0);

	sUserInfo info;

	for (int i = 1; i < 4; i++)
	{
		WSingleton<CSharedDoc>::Instance()->m_userInfo[i] =
			WSingleton<CSharedDoc>::Instance()->m_userInfo[0];
	}

	m_timeLimit = 0;
	if (m_mapIndex == 0xFF)
	{
		m_mapIndex = rand() % 20;
	}

	if (m_mapIndex == 0x11 || m_mapIndex == 0x0C)
	{
		m_mapIndex = 0;
	}

	if (ItemManager()->FindCourse(0x28000000 | m_mapIndex) == 0)
	{
		do
		{
			if (++m_mapIndex >= 20)
			{
				m_mapIndex = 0;
			}
		} while (ItemManager()->FindCourse(0x28000000 | m_mapIndex) == 0);
	}

	m_holeNum = 3;
	m_holeType = 0;
	m_timeLimit = 0;
	SetGameType(0);
	WSingleton<CSharedDoc>::Instance()->m_bGameOver = false;

	int count = WSingleton<CSharedDoc>::Instance()->m_charMap.size();

	m_petList.reserve(count);

	for (int i = 0; i < count; i++)
	{
		m_petList.push_back(i);
	}

	std::random_shuffle(m_petList.begin(), m_petList.end());
}

void CFamilyMain::OnInitFamilyFinish(void)
{
	m_pOverBarTitle->SetBgImg("title_family_renew");

	for (int i = 0; i < 2; i++)
	{
		m_families[i] = new cFamily;
		m_families[i]->Init(m_areas[i]->GetRect(), m_petList, i);

		cFamily* family = m_families[i];
		FrListBox* list = m_equipLists[i];

		list->AddItem(CEquipHelper::Instance()->GetItemCommon(
			CEquipHelper::CATEGORY_CHARACTER, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(
			CEquipHelper::CATEGORY_CADDIE, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(
			CEquipHelper::CATEGORY_CLUB, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(
			CEquipHelper::CATEGORY_BALL, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(
			CEquipHelper::CATEGORY_MASCOT, family->GetUserEquip()));
		list->AddItem(0);

		WRect rect = list->GetRect();

		rect.x = family->GetFramePos().x;
		rect.y = family->GetFramePos().y;
		list->SetRect(rect);
	}

	TurnOffControls();
	m_pUnderBarBongdariShop->Enable(true);
}

void CFamilyMain::TurnOffControls(void)
{
	m_pUnderBarMagicBox->Enable(false);
	m_pUnderBarShop->Enable(false);
	m_pUnderBarMyRoom->Enable(false);

	m_pUnderBarItemStorage->Enable(false);

	m_pUnderBarCookie->Enable(false);
	m_pUnderBarServer->Enable(false);
	m_pUnderBarRanking->Enable(false);
	m_pUnderBarGuild->Enable(false);
	m_pOverBarSpecialCard->Enable(false);
	m_pUnderBarGift->Enable(false);
	m_pUnderBarGame->Enable(false);

	m_pOverBarChar->SetVisible(false);
	m_pOverBarCaddie->SetVisible(false);
	m_pOverBarAztec->SetVisible(false);
	m_pOverBarMascot->SetVisible(false);
	m_pOverBarClub->SetVisible(false);
}

void CFamilyMain::OnInitAreaPet1(int param)
{
	m_areas[0] = DYNAMIC_CAST(FrArea, param);
}

void CFamilyMain::OnInitAreaPet2(int param)
{
	m_areas[1] = DYNAMIC_CAST(FrArea, param);
}

void CFamilyMain::OnInitAreaPet3(int param)
{
	m_areas[2] = DYNAMIC_CAST(FrArea, param);
}

void CFamilyMain::OnInitAreaPet4(int param)
{
	m_areas[3] = DYNAMIC_CAST(FrArea, param);
}

void CFamilyMain::OnInitAreaGlobal(int param)
{
	m_areas[4] = DYNAMIC_CAST(FrArea, param);
}

void CFamilyMain::OnDrawAreaPet1(void)
{
	if (g_pFresh->GetManager()->GetGDI() == 0)
	{
		return;
	}

	if (m_families[0])
	{
		m_families[0]->Render();
	}
}

void CFamilyMain::OnDrawAreaPet2(void)
{
	if (g_pFresh->GetManager()->GetGDI() == 0)
	{
		return;
	}

	if (m_families[1])
	{
		m_families[1]->Render();
	}
}

void CFamilyMain::OnDrawAreaPet3(void)
{
	if (g_pFresh->GetManager()->GetGDI() == 0)
	{
		return;
	}

	if (m_families[2])
	{
		m_families[2]->Render();
	}
}

void CFamilyMain::OnDrawAreaPet4(void)
{
	if (g_pFresh->GetManager()->GetGDI() == 0)
	{
		return;
	}

	if (m_families[3])
	{
		m_families[3]->Render();
	}
}

void CFamilyMain::OnDrawAreaGlobal(void)
{
	ReFresh(false);
}

void CFamilyMain::OnInitBtnStart(int param)
{
	m_buttons[0] = DYNAMIC_CAST(FrButton, param);
}

void CFamilyMain::OnInitBtnSelMap(int param)
{
	m_buttons[1] = DYNAMIC_CAST(FrButton, param);
}

void CFamilyMain::OnInitBtnHoleType(int param)
{
	m_buttons[2] = DYNAMIC_CAST(FrButton, param);
	m_buttons[2]->SetPushDelay(0.1f);
}

void CFamilyMain::OnInitBtnHoleNum(int param)
{
	m_buttons[3] = DYNAMIC_CAST(FrButton, param);
	m_buttons[3]->SetPushDelay(0.1f);
}

void CFamilyMain::OnInitBtnTimeLimit(int param)
{
	m_buttons[4] = DYNAMIC_CAST(FrButton, param);
	m_buttons[4]->SetPushDelay(0.1f);
}

void CFamilyMain::OnInitCreatePet3(int param)
{
	m_buttons[5] = DYNAMIC_CAST(FrButton, param);
}

void CFamilyMain::OnInitCreatePet4(int param)
{
	m_buttons[6] = DYNAMIC_CAST(FrButton, param);
}

void CFamilyMain::OnInitDeletePet3(int param)
{
	m_buttons[7] = DYNAMIC_CAST(FrButton, param);
	m_buttons[7]->SetVisible(false);
}

void CFamilyMain::OnInitDeletePet4(int param)
{
	m_buttons[8] = DYNAMIC_CAST(FrButton, param);
	m_buttons[8]->SetVisible(false);
}

void CFamilyMain::OnDownBtnSelMap(void)
{
	if (m_selMapDlg == 0)
	{
		m_selMapDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(), this,
			"treasuremap", 0);
		if (m_selMapDlg)
		{
			m_selMapDlg->SetSelectMap(m_mapIndex);
			m_selMapDlg->SetMapIndexChanged(false);
			m_selMapDlg->EnableDrag(false);
			m_selMapDlg->EnableRandomBtn(false);

			m_selMapDlg->Open((FRESH_PFN_RESULT)&CFamilyMain::OnResultSelMap,
				3);
		}
	}
}

bool CFamilyMain::OnResultSelMap(int, FrForm*)
{
	if (m_selMapDlg)
	{
		m_mapIndex = m_selMapDlg->GetCurMapIdx();
		SetCurMap(m_mapIndex);
		m_selMapDlg = 0;
	}
	return true;
}

void CFamilyMain::OnDownBtnHoleType(void)
{
	m_holeType++;

	if (m_holeType > 3)
	{
		m_holeType = 0;
	}

	SetHoleType(m_holeType);
}

void CFamilyMain::OnDownBtnHoleNum(void)
{
	unsigned char holes[4] = { 3, 6, 9, 18 };

	for (int i = 0; i < 4; ++i)
	{
		if (holes[i] == m_holeNum)
		{
			++i;
			if (i >= 4)
			{
				i = 0;
			}

			m_holeNum = holes[i];
			break;
		}
	}

	SetHoles(m_holeNum);
}

void CFamilyMain::OnDownBtnTimeLimit(void)
{
	unsigned long limits[6] = { 0, 30000, 40000, 50000, 60000, 90000 };

	for (int i = 0; i < 6; ++i)
	{
		if (limits[i] == m_timeLimit)
		{
			++i;
			if (i >= 6)
			{
				i = 0;
			}

			m_timeLimit = limits[i];
			break;
		}
	}

	SetShotTimeLimit(m_timeLimit);
}

void CFamilyMain::OnDownCreatePet3(void)
{
	if (m_unknown248)
	{
		return;
	}

	m_unknown248 = true;
	m_buttons[5]->SetVisible(false);

	m_families[2] = new cFamily;
	m_families[2]->Init(m_areas[2]->GetRect(), m_petList, 2);

	cFamily* family = m_families[2];
	FrListBox* list = m_equipLists[2];
	m_equipLists[2]->SetVisible(true);

	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CHARACTER, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CADDIE, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CLUB, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_BALL, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_MASCOT, family->GetUserEquip()));
	list->AddItem(0);

	WRect rect = list->GetRect();

	rect.x = family->GetFramePos().x;
	rect.y = family->GetFramePos().y;
	list->SetRect(rect);
	m_unknown248 = false;
	m_buttons[7]->SetVisible(true);
}

void CFamilyMain::OnDownCreatePet4(void)
{
	if (m_unknown248)
	{
		return;
	}

	m_unknown248 = true;
	m_buttons[6]->SetVisible(false);

	m_families[3] = new cFamily;
	m_families[3]->Init(m_areas[3]->GetRect(), m_petList, 3);

	cFamily* family = m_families[3];
	FrListBox* list = m_equipLists[3];
	m_equipLists[3]->SetVisible(true);

	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CHARACTER, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CADDIE, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_CLUB, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_BALL, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(
		CEquipHelper::CATEGORY_MASCOT, family->GetUserEquip()));
	list->AddItem(0);

	WRect rect = list->GetRect();

	rect.x = family->GetFramePos().x;
	rect.y = family->GetFramePos().y;
	list->SetRect(rect);
	m_unknown248 = false;
	m_buttons[8]->SetVisible(true);
}

void CFamilyMain::OnDownDeletePet3(void)
{
	cFamily* family = m_families[2];
	delete family;
	m_families[2] = 0;

	m_buttons[7]->SetVisible(false);
	m_buttons[5]->SetVisible(true);
	m_buttons[5]->EnableHover(true, 0.0f);
	m_equipLists[2]->ClearItem();
	m_equipLists[2]->SetVisible(false);
}

void CFamilyMain::OnDownDeletePet4(void)
{
	cFamily* family = m_families[3];
	delete family;
	m_families[3] = 0;

	m_buttons[8]->SetVisible(false);
	m_buttons[6]->SetVisible(true);
	m_buttons[6]->EnableHover(true, 0.0f);
	m_equipLists[3]->ClearItem();
	m_equipLists[3]->SetVisible(false);
}

void CFamilyMain::OnDrawBtnStart(void)
{
}

void CFamilyMain::OnDrawBtnSelMap(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
	{
		return;
	}

	IFF_STRUCT::sCourse* course =
		ItemManager()->FindCourse(0x28000000 | m_mapIndex);
	if (course == 0)
	{
		return;
	}

	std::string name("mid_");
	name += course->c.Icon;
	const Bitmap* icon = g_pFresh->GetBitmap(name.c_str());

	if (icon)
	{
		WRect rect = m_buttons[1]->GetRect();
		rect.x += (rect.w - icon->Width() + 1.0f) * 0.5f;
		rect.y += (rect.h - icon->Height() + 1.0f) * 0.5f;
		rect.w = (float)icon->Width();
		rect.h = (float)icon->Height();

		pGDI->DrawTexture(icon, rect, 0xFFFFFFFF, 0);
	}
}

void CFamilyMain::OnDrawBtnHoleType(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
	{
		return;
	}

	const WRect& rect = m_buttons[2]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);

	std::string holeTypes[4] = { "FRONT", "BACK", "RANDOM", "SHUFFLE" };
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0,
		MakeStr("\xc1\xf8\xc7\xe0\xb9\xe6\xbd\xc4:%s",
			holeTypes[m_holeType].c_str()),
		0);
	pGDI->SetTextStyle(0);
}

void CFamilyMain::OnDrawBtnHoleNum(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
	{
		return;
	}

	const WRect& rect = m_buttons[3]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0,
		MakeStr("\xc8\xa6 \xbc\xf6 : %d \xc8\xa6", m_holeNum), 0);
	pGDI->SetTextStyle(0);
}

void CFamilyMain::OnDrawBtnTimeLimit(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
	{
		return;
	}

	const WRect& rect = m_buttons[4]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);
	unsigned long seconds = m_timeLimit / 1000;
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0,
		seconds == 0
			? "\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3:\xb9\xab\xc1\xa6\xc7\xd1"
			: MakeStr("\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3:%d\xc3\xca", seconds),
		0);
	pGDI->SetTextStyle(0);
}

void CFamilyMain::OnInitListEquip1(int param)
{
	m_equipLists[0] = DYNAMIC_CAST(FrListBox, param);
}

void CFamilyMain::OnInitListEquip2(int param)
{
	m_equipLists[1] = DYNAMIC_CAST(FrListBox, param);
}

void CFamilyMain::OnInitListEquip3(int param)
{
	m_equipLists[2] = DYNAMIC_CAST(FrListBox, param);
}

void CFamilyMain::OnInitListEquip4(int param)
{
	m_equipLists[3] = DYNAMIC_CAST(FrListBox, param);
}

void CFamilyMain::OnDownListEquip1(int)
{
	OnDownEquipListBox(0);
}

void CFamilyMain::OnDownListEquip2(int)
{
	OnDownEquipListBox(1);
}

void CFamilyMain::OnDownListEquip3(int)
{
	OnDownEquipListBox(2);
}

void CFamilyMain::OnDownListEquip4(int)
{
	OnDownEquipListBox(3);
}

void CFamilyMain::OnDownEquipListBox(int index)
{
	FrListItem* pItem = m_equipLists[index]->GetSelected();
	if (pItem == 0)
	{
		return;
	}

	bool open = false;
	if (pItem->pData == 0)
	{
		if (pItem->idx == 5)
		{
			m_curPet = index;
			FrEquipDlg* dlg = CreateForm<FrEquipDlg>(g_pFresh->GetManager(),
				this, "equip", 0);
			if (dlg)
			{
				dlg->Init(ms_itemslotBackup[m_curPet], true);
				dlg->Open((FRESH_PFN_RESULT)&CFamilyMain::OnItemEquipResult, 3);
			}
		}
		else
		{
			switch (pItem->idx)
			{
			case 1:
				if (WSingleton<CSharedDoc>::Instance()->m_caddieMap.size() != 0)
				{
					open = true;
				}
				break;
			case 4:
				if (WSingleton<CSharedDoc>::Instance()->m_mascotMap.size() != 0)
				{
					open = true;
				}
				break;
			}
		}
	}
	else
	{
		open = true;
	}

	if (m_subEquipBar == 0 && open)
	{
		m_selectedItem = pItem;
		m_curPet = index;

		m_subEquipBar = CreateForm<FrSubEquipBar>(g_pFresh->GetManager(), this,
			"EQUIPBAR_SUB", 0);
		if (m_subEquipBar)
		{
			int category = 1;
			WPoint pt(pItem->pos.x, pItem->pos.y);
			switch (pItem->idx)
			{
			case 4:
				category = 0x10;
				break;
			case 1:
				category = 7;
				break;
			case 0:
				category = 1;
				break;
			case 2:
				category = 4;
				break;
			case 3:
				category = 5;
				break;
			}

			m_subEquipBar->Open(
				(FRESH_PFN_RESULT)&CFamilyMain::OnResultSubEquipBar,
				(FrSubEquipBar::eCategory)category, pt,
				(FrSubEquipBar::eSubType)2);

			WRect rect = m_subEquipBar->GetRect();
			rect.y -= rect.h;

			WView* view = g_view;

			if (rect.x + rect.w + 5.0f > view->GetWidth())
			{
				rect.x = view->GetWidth() - rect.w - 5.0f;
			}
			m_subEquipBar->SetRect(rect);
		}
	}
}

bool CFamilyMain::OnItemEquipResult(int result, FrForm* pForm)
{
	if (result == 1 || result == 2)
	{
		cFamily* family = m_families[m_curPet];
		family->SetItemSlot(((FrEquipDlg*)pForm)->GetMyItemSlot());

		memcpy(ms_itemslotBackup[m_curPet],
			((FrEquipDlg*)pForm)->GetMyItemSlot(),
			sizeof(ms_itemslotBackup[0]));
	}
	return true;
}

bool CFamilyMain::OnResultSubEquipBar(int result, FrForm* pForm)
{
	if (result == 1)
	{
		cFamily* family = m_families[m_curPet];
		IFF_ITEM_COMMON* item = m_subEquipBar->GetResult();
		if (item)
		{
			int group = item->TypeId >> 26;
			if (group == 5)
			{
				family->SetUserEquip(group, item->TypeId);
			}
			else
			{
				family->SetUserEquip(group,
					m_subEquipBar->GetLastSelectedGuid());
			}

			m_selectedItem->pData = item;
		}
		else
		{
			switch (m_subEquipBar->GetCategory())
			{
			case 0x10:
				family->SetUserEquip(0x10, 0);
				m_selectedItem->pData = 0;
				break;
			case 7:
				family->SetUserEquip(7, 0);
				m_selectedItem->pData = 0;
				break;
			}
		}
	}

	m_subEquipBar = 0;
	m_selectedItem = 0;
	return true;
}

void CFamilyMain::OnDrawListEquip1(int item)
{
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

void CFamilyMain::OnDrawListEquip2(int item)
{
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

void CFamilyMain::OnDrawListEquip3(int item)
{
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

void CFamilyMain::OnDrawListEquip4(int item)
{
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

void CFamilyMain::OnDrawEquipListBox(FrListItem* pItem)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pItem == 0 || pGDI == 0)
	{
		return;
	}

	const Bitmap* bar;
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)pItem->pData;
	if (item)
	{
		bar = g_pFresh->GetBitmap(MakeStr("bar_%s", item->Icon));
	}
	else
	{
		bar = g_pFresh->GetBitmap("bar_no");
	}

	if (bar)
	{
		WRect rect(pItem->pos.x + 5.0f, pItem->pos.y + 5.0f,
			bar->Width() + 0.5f, bar->Height() + 0.5f);
		if (pItem->idx != 5)
		{
			pGDI->DrawTexture(bar, rect, 0xFFFFFFFF, 0);
		}
		else
		{
			const Bitmap* button =
				g_pFresh->GetBitmap("gameroom_equip_item_bn");
			if (button)
			{
				pGDI->DrawTexture(button, rect, 0xFFFFFFFF, 0);
			}
		}

		if (rect.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			const Bitmap* frame = g_pFresh->GetBitmap("select_frame");
			if (frame)
			{
				pGDI->DrawTexture(frame,
					WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f,
						rect.h + 6.0f),
					0xFFFFFFFF, 0);
			}
		}
	}
}

void CFamilyMain::OnDownBtnStart(void)
{
	SetShotTimeLimit(m_timeLimit);
	SetCurMap(m_mapIndex);
	SetGameType(0);
	SetHoles(m_holeNum);
	SetHoleType(m_holeType);
	WSingleton<CSharedDoc>::Instance()->m_slotList.clear();

	if ((0x28000000 |
			((m_mapIndex > 0x7F && m_mapIndex != 0xFD) ? m_mapIndex - 0x80
													   : m_mapIndex)) == 0x11 ||
		(0x28000000 |
			((m_mapIndex > 0x7F && m_mapIndex != 0xFD) ? m_mapIndex - 0x80
													   : m_mapIndex)) == 0x0C)
	{
		m_mapIndex = 0;
		SetCurMap(m_mapIndex);
	}

	for (int i = 0; i < 4; i++)
	{
		sSlotInfo slot;
		memset(&slot, 0, sizeof(slot));
		memset(&WSingleton<CSharedDoc>::Instance()->m_userInfo[i], 0,
			sizeof(sUserInfo));

		cFamily* pFamily = m_families[i];
		if (pFamily == NULL)
		{
			slot.dwGuid = (unsigned long)-1;
			memset(ms_itemslotBackup[i], 0, sizeof(ms_itemslotBackup[i]));
			WSingleton<CSharedDoc>::Instance()->m_slotList.push_back(slot);
			continue;
		}

		pFamily->ConvertUserInfo(
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i]);
		slot.connectionRank = (unsigned char)(i + 1);
		slot.dwGuid = i;

		slot.level = WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.Level;
		strncpy(slot.sNick, MakeStr("PLAYER%d", i + 1), 0x15);
		strcpy(WSingleton<CSharedDoc>::Instance()->m_userInfo[i].info.sNick,
			MakeStr("PLAYER%d", i + 1));

		memcpy(ms_itemslotBackup[i],
			WSingleton<CSharedDoc>::Instance()
				->m_userInfo[i]
				.userEquip.tidItemSlot,
			sizeof(ms_itemslotBackup[i]));
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i].caddieInfo.tid);

		if (pCaddie != NULL &&
			(WSingleton<CSharedDoc>::Instance()->m_userInfo[i].stat.Level <
					(pCaddie->c.Level) ||
				((WSingleton<CSharedDoc>::Instance()
						 ->m_userInfo[i]
						 .caddieInfo.Rent_flag) &&
					WSingleton<CSharedDoc>::Instance()
							->m_userInfo[i]
							.caddieInfo.Remain_Date == 0 &&
					pCaddie->MonthlyFee > 0)))
		{
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i].caddieInfo.tid =
				0;
		}

		IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i].clubInfo.tid);

		if (pClubSet != NULL &&
				WSingleton<CSharedDoc>::Instance()->m_userInfo[i].stat.Level <
					(pClubSet->c.Level) &&
				!(pClubSet->c.IsUnderLvl) ||
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i].stat.Level >
					(pClubSet->c.Level) &&
				(pClubSet->c.IsUnderLvl))
		{
			WSingleton<CSharedDoc>::Instance()->m_userInfo[i].clubInfo.tid =
				0x10000000;
		}

		WSingleton<CSharedDoc>::Instance()->m_slotList.push_back(slot);
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_holeType)
	{
	case 2:
	{
		int hole = rand() % 18;
		for (int i = 0; i < 18; i++)
		{
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] =
				(unsigned char)(hole + 1);
			hole++;
			if (hole >= 18)
			{
				hole = 0;
			}
		}
		break;
	}

	case 0:
	{
		for (int i = 0; i < 18; i++)
		{
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] =
				(unsigned char)(i + 1);
		}
		break;
	}

	case 1:
	{
		int hole = 9;
		for (int i = 0; i < 18; i++)
		{
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] =
				(unsigned char)(hole + 1);
			hole++;
			if (hole >= 18)
			{
				hole = 0;
			}
		}
		break;
	}

	case 3:
	{
		for (unsigned char i = 0; i < 18; i++)
		{
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = i + 1;
		}

		std::random_shuffle(WSingleton<CSharedDoc>::Instance()->m_holeOrder,
			WSingleton<CSharedDoc>::Instance()->m_holeOrder + 17);
		break;
	}
	}

	for (int i = 0; i < 18; i++)
	{
		WSingleton<CSharedDoc>::Instance()->m_courseMap[i] =
			(unsigned char)(rand() % 3);
	}

	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;

	WSingleton<CMouseCursor>::Instance()->SetMode(1);
	WSingleton<CMouseCursor>::Instance()->SetActive(false);
	WSingleton<CTaskManager>::Instance()->ChangeTask("CGolfTask", "CGolfDoc",
		false);

	WSendPacket packet(0x5A);
	packet.Encode1(1);
	packet.Send(TO_GAME);
}

CFamilyMain::CFamilyMain(void)
	: m_selMapDlg(0), m_subEquipBar(0), m_unknown248(false)
{
	m_curPet = 0;
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;
	g_pFresh->GetManager()->SetExclusiveKey(true);
}

CFamilyMain::~CFamilyMain(void)
{
	g_pFresh->GetManager()->SetExclusiveKey(false);
	this << MsgObject(0, 2, 0, 0, 0, 0, 0);
}

void CFamilyMain::OnPreLoadInit(void)
{
	// HACK!
	(void)&g_HUGE;
	(void)&g_EPSILON;
	CTaskMain::OnPreLoadInit();

	WSingleton<CMouseCursor>::Instance()->SetAutoHide(false);
}

void CFamilyMain::OnLoad(void)
{
	CTaskMain::OnLoad();
}

void CFamilyMain::OnInit(void)
{
	CTaskMain::OnInit();
}

void CFamilyMain::OnDestroy(void)
{
	std::for_each(m_families.begin(), m_families.end(), Delete_Object());
	m_families.clear();
	m_areas.clear();
	m_buttons.clear();

	CTaskMain::OnDestroy();
}

void CFamilyMain::OnProcess(float dt)
{
	for (std::vector<cFamily*>::iterator it = m_families.begin();
		it != m_families.end(); ++it)
	{
		if (*it)
		{
			(*it)->Process(dt);
		}
	}

	CTaskMain::OnProcess(dt);
}

void CFamilyMain::OnDisplay(void)
{
}

void CFamilyMain::ReFresh(bool)
{
}

void CFamilyMain::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0:
	{
		if (g_pFresh == 0)
		{
			return;
		}

		const char* layout = (const char*)msg.param1;
		if (WSingleton<CUserInfo>::Instance())
		{
			WSingleton<CUserInfo>::Instance()->Close();
		}
		g_pFresh->OpenLayout(layout, this, true);
		break;
	}

	case 1:
	case 2:
	case 0xE2:
	case 0xE3:
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}
