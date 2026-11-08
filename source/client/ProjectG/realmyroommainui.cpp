#include "s5/utilities.h"
#include "mathconsts.h"
#include "clientsetting.h"
#include "realmyroommain.h"
#include "realmyroom_avatar.h"
#include "exhibition.h"
#include "fortunedlg.h"
#include "fortuneopendlg.h"
#include "enchantdlg.h"
#include "caddieinfodlg.h"
#include "panggiftdlg.h"
#include "ccardbook.h"
#include "post_senddlg.h"
#include "uccdrawdlg.h"
#include "itemstorage.h"
#include "s5/rentalextensiondlg.h"
#include "frrandomboxprogress.h"
#include "cardboxdlg.h"
#include "returngiftdlg.h"
#include "deleteitem.h"
#include "replaymodedlg.h"
#include "carddlg.h"
#include "ucccopydlg.h"
#include "downgradedlg.h"
#include "mascotinfodlg.h"
#include "setitemdlg.h"
#include "resultdlg.h"
#include "post_recivedlg.h"
#include "realmyroom_authoritydlg.h"
#include "cardremove.h"
#include "furniture_ability.h"
#include "realmyroomtask.h"
#include "transferableitem.h"
#include "contentsdoc.h"
#include "cardsystem.h"
#include "tooltip.h"
#include "uccmanager.h"
#include "notedlg.h"
#include "frienddlg.h"
#include "user_info.h"
#include "mousecursor.h"
#include "codetinker.h"
#include "inputmanager.h"
#include "mascot.h"
#include "levelupitemdlg.h"
#include "standinginline.h"
#include "guilddefine.h"
#include "frdesktop.h"
#include "frgroupbox.h"
#include "frcontextmenuctrl.h"
#include "rankinginfo.h"
#include "wbone.h"
#include "partsinfo.h"
#include "equipskin.h"
#include "titles_client.h"
#include "../../shared/s5/sharedutilities.h"
#include "setitemdlg.h"
#include "quadtree.h"
#include "logininfo.h"
#include "uccclothes.h"
#include "userinfoform.h"
#include "s5/supplypack.h"
#include "guildactor.h"
#include "gatewayactor.h"
#include "avatar_task.h"

// HACK! IL symbol padding.
// clang-format off
namespace { typedef int t0; typedef int t1; typedef int t2; typedef int t3; typedef int t4; typedef int t5; typedef int t6; typedef int t7; typedef int t8; typedef int t9; typedef int t10; typedef int t11; typedef int t12; typedef int t13; typedef int t14; typedef int t15; typedef int t16; typedef int t17; typedef int t18; typedef int t19; typedef int t20; typedef int t21; typedef int t22; typedef int t23; typedef int t24; typedef int t25; typedef int t26; typedef int t27; typedef int t28; typedef int t29; typedef int t30; typedef int t31; typedef int t32; typedef int t33; typedef int t34; typedef int t35; typedef int t36; typedef int t37; typedef int t38; typedef int t39; typedef int t40; typedef int t41; typedef int t42; typedef int t43; typedef int t44; typedef int t45; typedef int t46; typedef int t47; typedef int t48; }
// clang-format on

void ReviseItemIconPoint(unsigned long typeId, WPoint& pt)
{
	switch (typeId >> 26)
	{
	case 1:
	case 2:
	case 8:
	case 9:
	case 15:
	case 16:
	case 28:
		pt.x += 7.0f;
		pt.y += 2.0f;
		break;
	case 11:
		pt.x += 9.0f;
		pt.y += 12.0f;
		break;
	case 14:
		if ((typeId & 0x3c00000) == 0x800000)
		{
			pt.x += 7.0f;
			pt.y += 12.0f;
		}
		else
		{
			pt.x += 16.0f;
			pt.y += 27.0f;
		}
		break;
	case 31:
		pt.x += 16.0f;
		pt.y += 12.0f;
		break;
	case 18:
		pt.x += 21.0f;
		pt.y += 12.0f;
		break;
	default:
		pt.x += 16.0f;
		pt.y += 27.0f;
		break;
	}
}

template <class T>
T* FindFrForm(const char* name)
{
	FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildForm(name);
	if (pWnd)
		return DYNAMIC_CAST(T, pWnd);

	return NULL;
}

BEGIN_FRESH_MSGMAP(CRealMyRoomMain, CTaskMain)
ON_FRESH_VI("REALMYROOM", FRCMD_INIT, CRealMyRoomMain::OnMyRoomInit)
ON_FRESH_VV("REALMYROOM", FRCMD_FINISH, CRealMyRoomMain::OnMyRoomFinish)

ON_FRESH_VI("REALMYROOMVISITOR", FRCMD_INIT, CRealMyRoomMain::OnMyRoomInit)
ON_FRESH_VV("REALMYROOMVISITOR", FRCMD_FINISH, CRealMyRoomMain::OnMyRoomFinish)

ON_FRESH_VI("REALMYROOM.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("REALMYROOM.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("REALMYROOM.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)

ON_FRESH_VI("REALMYROOMVISITOR.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("REALMYROOMVISITOR.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("REALMYROOMVISITOR.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)

ON_FRESH_VI("REALMYROOM.middle_ui.item_list", FRCMD_INIT,
	CRealMyRoomMain::OnItemListInit)
ON_FRESH_VI("REALMYROOM.middle_ui.item_list", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnItemListOwnerDraw)
ON_FRESH_VV("REALMYROOM.middle_ui.item_list", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnItemListBtnDown)
ON_FRESH_VV("REALMYROOM.middle_ui.item_list", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnItemListRBtnDown)
ON_FRESH_VV("REALMYROOM.middle_ui.item_list", FRCMD_MOUSEMOVE,
	CRealMyRoomMain::OnItemListMouseMove)
ON_FRESH_VV("REALMYROOM.middle_ui.item_list", FRCMD_DBLCLICK,
	CRealMyRoomMain::OnItemListDClick)
ON_FRESH_VI("REALMYROOM.middle_ui.tab_main", FRCMD_INIT,
	CRealMyRoomMain::OnTabMainInit)
ON_FRESH_VV("REALMYROOM.middle_ui.tab_main", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnTabMainButtonUp)
ON_FRESH_VI("REALMYROOM.middle_ui.tab_sub", FRCMD_INIT,
	CRealMyRoomMain::OnTabSubInit)
ON_FRESH_VV("REALMYROOM.middle_ui.tab_sub", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnTabSubButtonUp)

ON_FRESH_VI("REALMYROOM.middle_ui.gift_page", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnGiftPageOwnerDraw)
ON_FRESH_VV("REALMYROOM.middle_ui.click_area_list", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnGiftPageLBDown)

ON_FRESH_VI("REALMYROOM.middle_ui", FRCMD_INIT, CRealMyRoomMain::OnMiddleUIInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui", FRCMD_INIT,
	CRealMyRoomMain::OnMiddleUIInit)

ON_FRESH_VI("REALMYROOM.middle_ui.tabtxt1", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt1Init)
ON_FRESH_VI("REALMYROOM.middle_ui.tabtxt2", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt2Init)
ON_FRESH_VI("REALMYROOM.middle_ui.tabtxt2", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt3Init)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selected_name", FRCMD_INIT,
	CRealMyRoomMain::OnSelectedNameInit)

ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.tabtxt1", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt1Init)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.tabtxt2", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt2Init)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.tabtxt2", FRCMD_INIT,
	CRealMyRoomMain::OnTabTxt3Init)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selected_name", FRCMD_INIT,
	CRealMyRoomMain::OnSelectedNameInit)

ON_FRESH_VI("REALMYROOM.middle_ui.pet", FRCMD_INIT, CRealMyRoomMain::OnPetInit)
ON_FRESH_VI("REALMYROOM.middle_ui.pet", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnPetOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.skin_pet", FRCMD_INIT,
	CRealMyRoomMain::OnSkinPetInit)
ON_FRESH_VI("REALMYROOM.middle_ui.skin_pet2", FRCMD_INIT,
	CRealMyRoomMain::OnSkinPet2Init)
ON_FRESH_VI("REALMYROOM.middle_ui.item_desc_bg", FRCMD_INIT,
	CRealMyRoomMain::OnItemDescBgInit)
ON_FRESH_VI("REALMYROOM.middle_ui.item_desc_bg", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnItemDescBgOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.item_desc", FRCMD_INIT,
	CRealMyRoomMain::OnItemDescInit)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox", FRCMD_INIT,
	CRealMyRoomMain::OnItemSlotBoxInit)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_INIT,
	CRealMyRoomMain::OnEquipCancelInit)
ON_FRESH_VV("REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnEquipCancelUp)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_INIT,
	CRealMyRoomMain::OnEquipResetInit)
ON_FRESH_VV("REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnEquipResetUp)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.item_slot", FRCMD_INIT,
	CRealMyRoomMain::OnItemSlotInit)
ON_FRESH_VI("REALMYROOM.middle_ui.sbox.item_slot", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnItemSlotOwnerDraw)
ON_FRESH_VV("REALMYROOM.middle_ui.sbox.item_slot", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnItemSlotBtnDown)
ON_FRESH_VV("REALMYROOM.middle_ui.sbox.item_slot", FRCMD_DBLCLICK,
	CRealMyRoomMain::OnItemSlotDClick)
ON_FRESH_VI("REALMYROOM.middle_ui.gift_bg", FRCMD_INIT,
	CRealMyRoomMain::OnGiftBgInit)
ON_FRESH_VI("REALMYROOM.middle_ui.gift_desc", FRCMD_INIT,
	CRealMyRoomMain::OnGiftDescInit)

ON_FRESH_VI("REALMYROOM.middle_ui.furniture_arrange", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureArrangeInit)
ON_FRESH_VV("REALMYROOM.middle_ui.furniture_arrange", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureArrangeButtonUP)

ON_FRESH_VI("REALMYROOM.middle_ui.st", FRCMD_INIT,
	CRealMyRoomMain::OnStatBoxInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selframe", FRCMD_INIT,
	CRealMyRoomMain::OnSelFrameInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitemicon", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemIconInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitem", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemInit)
ON_FRESH_VV("REALMYROOM.middle_ui.st.selitem", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemUp)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitem", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnSelItemOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitemicon", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnSelItemIconOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitem_prev", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemPrevInit)
ON_FRESH_VV("REALMYROOM.middle_ui.st.selitem_prev", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemPrevUp)
ON_FRESH_VI("REALMYROOM.middle_ui.st.selitem_next", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemNextInit)
ON_FRESH_VV("REALMYROOM.middle_ui.st.selitem_next", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemNextUp)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_part", FRCMD_INIT,
	CRealMyRoomMain::OnStatPartInit)
ON_FRESH_VV("REALMYROOM.middle_ui.st.stat_part", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnStatPartUp)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_part", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnStatPartOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_all", FRCMD_INIT,
	CRealMyRoomMain::OnStatAllInit)
ON_FRESH_VV("REALMYROOM.middle_ui.st.stat_all", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnStatAllUp)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_all", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnStatAllOwnerDraw)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_frame", FRCMD_INIT,
	CRealMyRoomMain::OnStatFrameInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.stat_text", FRCMD_INIT,
	CRealMyRoomMain::OnStatTextInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.powerbar", FRCMD_INIT,
	CRealMyRoomMain::OnPowerBarInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.controlbar", FRCMD_INIT,
	CRealMyRoomMain::OnControlBarInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.impactbar", FRCMD_INIT,
	CRealMyRoomMain::OnImpactBarInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.spinbar", FRCMD_INIT,
	CRealMyRoomMain::OnSpinBarInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.curvebar", FRCMD_INIT,
	CRealMyRoomMain::OnCurveBarInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.epower", FRCMD_INIT,
	CRealMyRoomMain::OnPowerEditInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.econtrol", FRCMD_INIT,
	CRealMyRoomMain::OnControlEditInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.eimpact", FRCMD_INIT,
	CRealMyRoomMain::OnImpactEditInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.espin", FRCMD_INIT,
	CRealMyRoomMain::OnSpinEditInit)
ON_FRESH_VI("REALMYROOM.middle_ui.st.ecurve", FRCMD_INIT,
	CRealMyRoomMain::OnCurveEditInit)
ON_FRESH_VI("REALMYROOM.middle_ui.upgrade", FRCMD_INIT,
	CRealMyRoomMain::OnUpgradeInit)
ON_FRESH_VV("REALMYROOM.middle_ui.upgrade", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUpgradeUp)
ON_FRESH_VI("REALMYROOM.middle_ui.upgrade", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOM.middle_ui.upgrade", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)
ON_FRESH_VI("REALMYROOM.middle_ui.downgrade", FRCMD_INIT,
	CRealMyRoomMain::OnDowngradeInit)
ON_FRESH_VV("REALMYROOM.middle_ui.downgrade", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnDowngradeUp)
ON_FRESH_VI("REALMYROOM.middle_ui.downgrade", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOM.middle_ui.downgrade", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)

ON_FRESH_VI("REALMYROOM.rmenu", FRCMD_INIT, CRealMyRoomMain::OnRightMenuInit)
ON_FRESH_VV("REALMYROOM.rmenu", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnRightMenuLBtnDown)
ON_FRESH_VV("REALMYROOM.rmenu", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnRightMenuRBtnDown)

ON_FRESH_VI("REALMYROOM.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("REALMYROOM.ubar.back", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_BackUp)
ON_FRESH_VI("REALMYROOM.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.front", FRCMD_INIT,
	CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("REALMYROOM.ubar.front", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("REALMYROOM.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("REALMYROOM.ubar.shop", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("REALMYROOM.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("REALMYROOM.ubar.myroom", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("REALMYROOM.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("REALMYROOM.ubar.itemstorage", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("REALMYROOM.ubar.itemstorage", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VV("REALMYROOM.ubar.itemstorage", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("REALMYROOM.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("REALMYROOM.ubar.gift", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_GiftUp)
ON_FRESH_VI("REALMYROOM.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("REALMYROOM.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("REALMYROOM.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("REALMYROOM.ubar.game", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("REALMYROOM.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("REALMYROOM.ubar.server", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("REALMYROOM.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("REALMYROOM.ubar.exit", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("REALMYROOM.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.ubar.tabbtn_b", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("REALMYROOM.ubar.tabbtn_b", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("REALMYROOM.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("REALMYROOM.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("REALMYROOM.utab.magicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("REALMYROOM.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.magicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("REALMYROOM.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("REALMYROOM.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("REALMYROOM.utab.scratch", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("REALMYROOM.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("REALMYROOM.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("REALMYROOM.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("REALMYROOM.utab.ranking", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("REALMYROOM.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.utab.guild", FRCMD_INIT,
	CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("REALMYROOM.utab.guild", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("REALMYROOM.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("REALMYROOM.utab.notice", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("REALMYROOM.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.utab.maket", FRCMD_INIT,
	CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("REALMYROOM.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("REALMYROOM.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.middle_ui.chatbg", FRCMD_INIT,
	CRealMyRoomMain::OnEffectViewInit)
ON_FRESH_VI("REALMYROOM.middle_ui.chatbg", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnEffectViewOnwnerDraw)

ON_FRESH_VI("REALMYROOM.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("REALMYROOM.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("REALMYROOM.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("REALMYROOM.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("REALMYROOM.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.caddie", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("REALMYROOM.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("REALMYROOM.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("REALMYROOM.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("REALMYROOM.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("REALMYROOM.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("REALMYROOM.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.mascot", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("REALMYROOM.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("REALMYROOM.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("REALMYROOM.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("REALMYROOM.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("REALMYROOM.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("REALMYROOM.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("REALMYROOM.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("REALMYROOM.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("REALMYROOM.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("REALMYROOM.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("REALMYROOM.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("REALMYROOM.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("REALMYROOM.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("REALMYROOM.obar.messenger", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("REALMYROOM.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.obar.messenger", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.obar.myinfo", FRCMD_INIT,
	CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("REALMYROOM.obar.myinfo", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("REALMYROOM.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOM.obar.option", FRCMD_INIT,
	CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("REALMYROOM.obar.option", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("REALMYROOM.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOM.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOM.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("REALMYROOM.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("REALMYROOM.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("REALMYROOM.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("REALMYROOM.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("REALMYROOM.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("REALMYROOM.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("REALMYROOM.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)

ON_FRESH_VI("REALMYROOM.ftab_main", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureTabMainInit)
ON_FRESH_VV("REALMYROOM.ftab_main", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureTabMainButtonUp)

ON_FRESH_VI("REALMYROOM.fitem_list", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureListInit)
ON_FRESH_VI("REALMYROOM.fitem_list", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnFurnitureListOwnerDraw)
ON_FRESH_VV("REALMYROOM.fitem_list", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnFurnitureListBtnDown)

ON_FRESH_VI("REALMYROOM.farrorup", FRCMD_INIT,
	CRealMyRoomMain::OnFurniturePupInit)
ON_FRESH_VV("REALMYROOM.farrorup", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurniturePupButtonUp)
ON_FRESH_VI("REALMYROOM.farrordown", FRCMD_INIT,
	CRealMyRoomMain::OnFurniturePdownInit)
ON_FRESH_VV("REALMYROOM.farrordown", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurniturePdownButtonUp)
ON_FRESH_VI("REALMYROOM.flist_num", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureListNumInit)
ON_FRESH_VV("REALMYROOM.flist_num", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnFurnitureListNumBtnUp)
ON_FRESH_BI("REALMYROOM.flist_num", FRCMD_ENTERKEY,
	CRealMyRoomMain::OnFurnitureListEnterKey)
ON_FRESH_VI("REALMYROOM.flist_maxnum", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureListMaxNumInit)

ON_FRESH_VI("REALMYROOM.Btn_flistnum", FRCMD_INIT,
	CRealMyRoomMain::OnFurniture_BtnListNumInit)
ON_FRESH_VV("REALMYROOM.Btn_flistnum", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnFurniture_BtnListNumBtnUp)

ON_FRESH_VI("REALMYROOM.freset", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureResetInit)
ON_FRESH_VV("REALMYROOM.freset", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureResetButtonUp)
ON_FRESH_VI("REALMYROOM.fback", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackInit)
ON_FRESH_VV("REALMYROOM.fback", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureBackButtonUp)
ON_FRESH_VI("REALMYROOM.fsave", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureSaveInit)
ON_FRESH_VV("REALMYROOM.fsave", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureSaveButtonUp)
ON_FRESH_VI("REALMYROOM.fout", FRCMD_INIT, CRealMyRoomMain::OnFurnitureOutInit)
ON_FRESH_VV("REALMYROOM.fout", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnFurnitureOutButtonUp)

ON_FRESH_VI("REALMYROOM.ftab_background01", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit1)
ON_FRESH_VI("REALMYROOM.ftab_background02", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit2)
ON_FRESH_VI("REALMYROOM.ftab_background03", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit3)
ON_FRESH_VI("REALMYROOM.ftab_background04", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit4)
ON_FRESH_VI("REALMYROOM.ftab_background05", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit5)
ON_FRESH_VI("REALMYROOM.ftab_background06", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit6)
ON_FRESH_VI("REALMYROOM.ftab_background07", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit7)
ON_FRESH_VI("REALMYROOM.ftab_background08", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureBackGroundInit8)

ON_FRESH_VI("REALMYROOM.ftab_scrollimg01", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureScrollInit1)
ON_FRESH_VI("REALMYROOM.ftab_scrollimg02", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureScrollInit2)
ON_FRESH_VI("REALMYROOM.ftab_scrollimg03", FRCMD_INIT,
	CRealMyRoomMain::OnFurnitureScrollInit3)

ON_FRESH_VI("REALMYROOM.Btn_Menu", FRCMD_INIT, CRealMyRoomMain::OnBtnMenuInit)
ON_FRESH_VV("REALMYROOM.Btn_Menu", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OOnBtnMenuButtonUP)

ON_FRESH_VI("REALMYROOM.Btn_MenuList", FRCMD_INIT,
	CRealMyRoomMain::OnBtn_MenuListInit)
ON_FRESH_VI("REALMYROOM.Btn_MenuList", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnBtn_MenuListOwnerDraw)
ON_FRESH_VV("REALMYROOM.Btn_MenuList", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnBtn_MenuListBtnDown)
ON_FRESH_VV("REALMYROOM.Btn_MenuList", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnBtn_MenuListRBtnDown)
ON_FRESH_VV("REALMYROOM.Btn_MenuList", FRCMD_MOUSEMOVE,
	CRealMyRoomMain::OnBtn_MenuListMouseMove)
ON_FRESH_VV("REALMYROOM.Btn_MenuList", FRCMD_DBLCLICK,
	CRealMyRoomMain::OnBtn_MenuListDClick)

ON_FRESH_VI("REALMYROOM.item_hide_desc_bg", FRCMD_INIT,
	CRealMyRoomMain::OnItemDescBgInit2)
ON_FRESH_VI("REALMYROOM.item_hide_desc_bg", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnItemDescBgOwnerDraw2)
ON_FRESH_VI("REALMYROOM.item_hide_desc", FRCMD_INIT,
	CRealMyRoomMain::OnItemDescInit2)
ON_FRESH_VI("REALMYROOM.item_hide_name", FRCMD_INIT,
	CRealMyRoomMain::OnItemNameInit)

ON_FRESH_VI("REALMYROOM.card_book", FRCMD_INIT, CRealMyRoomMain::OnCardBookInit)
ON_FRESH_VI("REALMYROOM.card_book", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnCardBookOwnerDraw)
ON_FRESH_VI("REALMYROOM.cardpacklist", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackListInit)
ON_FRESH_VI("REALMYROOM.cardpacklist", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnCardPackListDraw)
ON_FRESH_VV("REALMYROOM.cardpacklist", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackListDown)
ON_FRESH_VV("REALMYROOM.cardpacklist", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackListRbUp)
ON_FRESH_VI("REALMYROOM.cardpage_index", FRCMD_INIT,
	CRealMyRoomMain::OnCardBookIndexInit)
ON_FRESH_VV("REALMYROOM.cardpage_index", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardBookIndexLBDown)
ON_FRESH_VV("REALMYROOM.cardpage_index", FRCMD_MOUSEMOVE,
	CRealMyRoomMain::OnCardBookIndexMouseOver)
ON_FRESH_VI("REALMYROOM.card_property", FRCMD_INIT,
	CRealMyRoomMain::OnCardPropertyInit)
ON_FRESH_VI("REALMYROOM.card_pack", FRCMD_INIT,
	CRealMyRoomMain::OnTabCardPackInit)
ON_FRESH_VV("REALMYROOM.card_pack", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnTabCardPackLBDown)
ON_FRESH_VI("REALMYROOM.card_use", FRCMD_INIT, CRealMyRoomMain::OnCardUseInit)
ON_FRESH_VV("REALMYROOM.card_use", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardUseLBDown)
ON_FRESH_VI("REALMYROOM.card_volume1", FRCMD_INIT,
	CRealMyRoomMain::OnTabCardVolume1Init)
ON_FRESH_VV("REALMYROOM.card_volume1", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnTabCardVolume1LBDown)
ON_FRESH_VI("REALMYROOM.card_volume2", FRCMD_INIT,
	CRealMyRoomMain::OnTabCardVolume2Init)
ON_FRESH_VV("REALMYROOM.card_volume2", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnTabCardVolume2LBDown)
ON_FRESH_VI("REALMYROOM.card_volume3", FRCMD_INIT,
	CRealMyRoomMain::OnTabCardVolume3Init)
ON_FRESH_VV("REALMYROOM.card_volume3", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnTabCardVolume3LBDown)
ON_FRESH_VI("REALMYROOM.card_volume4", FRCMD_INIT,
	CRealMyRoomMain::OnTabCardVolume4Init)
ON_FRESH_VV("REALMYROOM.card_volume4", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnTabCardVolume4LBDown)

ON_FRESH_VI("REALMYROOM.cardpack_slot1", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit1)
ON_FRESH_VI("REALMYROOM.cardpack_slot2", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit2)
ON_FRESH_VI("REALMYROOM.cardpack_slot3", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit3)
ON_FRESH_VI("REALMYROOM.cardpack_slot4", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit4)
ON_FRESH_VI("REALMYROOM.cardpack_slot5", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit5)
ON_FRESH_VI("REALMYROOM.cardpack_slot6", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit6)
ON_FRESH_VI("REALMYROOM.cardpack_slot7", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit7)
ON_FRESH_VI("REALMYROOM.cardpack_slot8", FRCMD_INIT,
	CRealMyRoomMain::OnCardPackInit8)

ON_FRESH_VV("REALMYROOM.cardpack_slot1", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown1)
ON_FRESH_VV("REALMYROOM.cardpack_slot2", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown2)
ON_FRESH_VV("REALMYROOM.cardpack_slot3", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown3)
ON_FRESH_VV("REALMYROOM.cardpack_slot4", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown4)
ON_FRESH_VV("REALMYROOM.cardpack_slot5", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown5)
ON_FRESH_VV("REALMYROOM.cardpack_slot6", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown6)
ON_FRESH_VV("REALMYROOM.cardpack_slot7", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown7)
ON_FRESH_VV("REALMYROOM.cardpack_slot8", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardPackLBDown8)

ON_FRESH_VV("REALMYROOM.cardpack_slot1", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown1)
ON_FRESH_VV("REALMYROOM.cardpack_slot2", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown2)
ON_FRESH_VV("REALMYROOM.cardpack_slot3", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown3)
ON_FRESH_VV("REALMYROOM.cardpack_slot4", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown4)
ON_FRESH_VV("REALMYROOM.cardpack_slot5", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown5)
ON_FRESH_VV("REALMYROOM.cardpack_slot6", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown6)
ON_FRESH_VV("REALMYROOM.cardpack_slot7", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown7)
ON_FRESH_VV("REALMYROOM.cardpack_slot8", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardPackRBDown8)

ON_FRESH_VI("REALMYROOM.card_slot1", FRCMD_INIT, CRealMyRoomMain::OnCardInit1)
ON_FRESH_VI("REALMYROOM.card_slot2", FRCMD_INIT, CRealMyRoomMain::OnCardInit2)
ON_FRESH_VI("REALMYROOM.card_slot3", FRCMD_INIT, CRealMyRoomMain::OnCardInit3)
ON_FRESH_VI("REALMYROOM.card_slot4", FRCMD_INIT, CRealMyRoomMain::OnCardInit4)
ON_FRESH_VI("REALMYROOM.card_slot5", FRCMD_INIT, CRealMyRoomMain::OnCardInit5)
ON_FRESH_VI("REALMYROOM.card_slot6", FRCMD_INIT, CRealMyRoomMain::OnCardInit6)
ON_FRESH_VI("REALMYROOM.card_slot7", FRCMD_INIT, CRealMyRoomMain::OnCardInit7)
ON_FRESH_VI("REALMYROOM.card_slot8", FRCMD_INIT, CRealMyRoomMain::OnCardInit8)
ON_FRESH_VI("REALMYROOM.card_slot9", FRCMD_INIT, CRealMyRoomMain::OnCardInit9)
ON_FRESH_VI("REALMYROOM.card_slot10", FRCMD_INIT, CRealMyRoomMain::OnCardInit10)
ON_FRESH_VI("REALMYROOM.card_slot11", FRCMD_INIT, CRealMyRoomMain::OnCardInit11)
ON_FRESH_VI("REALMYROOM.card_slot12", FRCMD_INIT, CRealMyRoomMain::OnCardInit12)

ON_FRESH_VV("REALMYROOM.card_slot1", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown1)
ON_FRESH_VV("REALMYROOM.card_slot2", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown2)
ON_FRESH_VV("REALMYROOM.card_slot3", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown3)
ON_FRESH_VV("REALMYROOM.card_slot4", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown4)
ON_FRESH_VV("REALMYROOM.card_slot5", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown5)
ON_FRESH_VV("REALMYROOM.card_slot6", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown6)
ON_FRESH_VV("REALMYROOM.card_slot7", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown7)
ON_FRESH_VV("REALMYROOM.card_slot8", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown8)
ON_FRESH_VV("REALMYROOM.card_slot9", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown9)
ON_FRESH_VV("REALMYROOM.card_slot10", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown10)
ON_FRESH_VV("REALMYROOM.card_slot11", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown11)
ON_FRESH_VV("REALMYROOM.card_slot12", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnCardLBDown12)

ON_FRESH_VV("REALMYROOM.card_slot1", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown1)
ON_FRESH_VV("REALMYROOM.card_slot2", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown2)
ON_FRESH_VV("REALMYROOM.card_slot3", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown3)
ON_FRESH_VV("REALMYROOM.card_slot4", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown4)
ON_FRESH_VV("REALMYROOM.card_slot5", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown5)
ON_FRESH_VV("REALMYROOM.card_slot6", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown6)
ON_FRESH_VV("REALMYROOM.card_slot7", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown7)
ON_FRESH_VV("REALMYROOM.card_slot8", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown8)
ON_FRESH_VV("REALMYROOM.card_slot9", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown9)
ON_FRESH_VV("REALMYROOM.card_slot10", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown10)
ON_FRESH_VV("REALMYROOM.card_slot11", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown11)
ON_FRESH_VV("REALMYROOM.card_slot12", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnCardRBDown12)

ON_FRESH_VI("REALMYROOMVISITOR.obar.title", FRCMD_INIT,
	CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.site", FRCMD_INIT,
	CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.char", FRCMD_INIT,
	CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("REALMYROOMVISITOR.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.caddie", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("REALMYROOMVISITOR.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.club", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("REALMYROOMVISITOR.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.aztec", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("REALMYROOMVISITOR.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.mascot", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("REALMYROOMVISITOR.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.spcard", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.obar.spcard", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.messenger", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.obar.myinfo", FRCMD_INIT,
	CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.myinfo", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.myinfo", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.obar.myinfo", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.obar.option", FRCMD_INIT,
	CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.option", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.option", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.obar.option", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("REALMYROOMVISITOR.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("REALMYROOMVISITOR.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)

ON_FRESH_VI("REALMYROOMVISITOR.ubar.back", FRCMD_INIT,
	CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.back", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_BackUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.back", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.back", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.front", FRCMD_INIT,
	CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.front", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.front", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.front", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.shop", FRCMD_INIT,
	CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.shop", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.shop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.shop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.myroom", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.myroom", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.myroom", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.itemstorage", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.itemstorage", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.gift", FRCMD_INIT,
	CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.gift", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUnderBar_GiftUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.gift", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.gift", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.cookie", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.cookie", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.game", FRCMD_INIT,
	CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.game", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.game", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.game", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.server", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.server", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.server", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.exit", FRCMD_INIT,
	CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.exit", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.exit", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.exit", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.ubar.tabbtn_b", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("REALMYROOMVISITOR.ubar.tabbtn_b", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st", FRCMD_INIT,
	CRealMyRoomMain::OnStatBoxInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selframe", FRCMD_INIT,
	CRealMyRoomMain::OnSelFrameInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitemicon", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemIconInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnSelItemOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitemicon", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnSelItemIconOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitem_prev", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemPrevInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.st.selitem_prev", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemPrevUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.selitem_next", FRCMD_INIT,
	CRealMyRoomMain::OnSelItemNextInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.st.selitem_next", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnSelItemNextUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_INIT,
	CRealMyRoomMain::OnStatPartInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnStatPartUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnStatPartOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_INIT,
	CRealMyRoomMain::OnStatAllInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnStatAllUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnStatAllOwnerDraw)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_frame", FRCMD_INIT,
	CRealMyRoomMain::OnStatFrameInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.stat_text", FRCMD_INIT,
	CRealMyRoomMain::OnStatTextInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.powerbar", FRCMD_INIT,
	CRealMyRoomMain::OnPowerBarInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.controlbar", FRCMD_INIT,
	CRealMyRoomMain::OnControlBarInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.impactbar", FRCMD_INIT,
	CRealMyRoomMain::OnImpactBarInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.spinbar", FRCMD_INIT,
	CRealMyRoomMain::OnSpinBarInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.curvebar", FRCMD_INIT,
	CRealMyRoomMain::OnCurveBarInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.epower", FRCMD_INIT,
	CRealMyRoomMain::OnPowerEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.econtrol", FRCMD_INIT,
	CRealMyRoomMain::OnControlEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.eimpact", FRCMD_INIT,
	CRealMyRoomMain::OnImpactEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.espin", FRCMD_INIT,
	CRealMyRoomMain::OnSpinEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.st.ecurve", FRCMD_INIT,
	CRealMyRoomMain::OnCurveEditInit)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_INIT,
	CRealMyRoomMain::OnUpgradeInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnUpgradeUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_INIT,
	CRealMyRoomMain::OnDowngradeInit)
ON_FRESH_VV("REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_LBUTTONUP,
	CRealMyRoomMain::OnDowngradeUp)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_HOVERON,
	CRealMyRoomMain::OnTooltipHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_HOVEROFF,
	CRealMyRoomMain::OnTooltipHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("REALMYROOMVISITOR.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.magicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.magicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.magicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.scratch", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.scratch", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.scratch", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab.maket", FRCMD_INIT,
	CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.maket", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.maket", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.ranking", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.ranking", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.ranking", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.utab.guild", FRCMD_INIT,
	CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.guild", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.guild", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.guild", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("REALMYROOMVISITOR.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("REALMYROOMVISITOR.utab.notice", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("REALMYROOMVISITOR.utab.notice", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("REALMYROOMVISITOR.utab.notice", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("REALMYROOMVISITOR.rmenu", FRCMD_INIT,
	CRealMyRoomMain::OnRightMenuInit)
ON_FRESH_VV("REALMYROOMVISITOR.rmenu", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnRightMenuLBtnDown)
ON_FRESH_VV("REALMYROOMVISITOR.rmenu", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnRightMenuRBtnDown)

ON_FRESH_VI("REALMYROOM.item_storage_bgi", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStorageBGI)
ON_FRESH_VI("REALMYROOM.item_storage_bgi", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnOwnerDrawItemStorageBGI)

ON_FRESH_VI("REALMYROOM.item_storage_pang", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStoragePang)
ON_FRESH_VI("REALMYROOM.item_storage_pang", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnOwnerDrawItemStoragePang)

ON_FRESH_VI("REALMYROOM.pang_minus", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStoragePangMinus)
ON_FRESH_VV("REALMYROOM.pang_minus", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStoragePangMinus)
ON_FRESH_VI("REALMYROOM.pang_plus", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStoragePangPlus)
ON_FRESH_VV("REALMYROOM.pang_plus", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStoragePangPlus)

ON_FRESH_VI("REALMYROOM.item_storage_list", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStorageList)
ON_FRESH_VI("REALMYROOM.item_storage_list", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnOwnerDrawItemStorageList)
ON_FRESH_VV("REALMYROOM.item_storage_list", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStorageList)
ON_FRESH_VV("REALMYROOM.item_storage_list", FRCMD_RBUTTONDOWN,
	CRealMyRoomMain::OnRBDownItemStorageList)
ON_FRESH_VV("REALMYROOM.item_storage_list", FRCMD_DBLCLICK,
	CRealMyRoomMain::OnDBClickItemStorageList)
ON_FRESH_VV("REALMYROOM.item_storage_list", FRCMD_MOUSEMOVE,
	CRealMyRoomMain::OnMouseMoveItemStorageList)

ON_FRESH_VI("REALMYROOM.item_storage_index", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStorageIndexMsg)
ON_FRESH_VI("REALMYROOM.item_storage_index", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnOwnerDrawItemStorageIndexMsg)
ON_FRESH_VV("REALMYROOM.item_storage_index", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStorageIndexMsg)

ON_FRESH_VI("REALMYROOM.item_storage_page_next", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStoragePageNext)
ON_FRESH_VV("REALMYROOM.item_storage_page_next", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStoragePageNext)
ON_FRESH_VI("REALMYROOM.item_storage_page_prev", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStoragePagePrev)
ON_FRESH_VV("REALMYROOM.item_storage_page_prev", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStoragePagePrev)

ON_FRESH_VI("REALMYROOM.item_storage_close", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStorageClose)
ON_FRESH_VV("REALMYROOM.item_storage_close", FRCMD_LBUTTONDOWN,
	CRealMyRoomMain::OnLBDownItemStorageClose)

ON_FRESH_VI("REALMYROOM.item_storage_dragdrop", FRCMD_INIT,
	CRealMyRoomMain::OnInitItemStorageDragDrop)
ON_FRESH_VI("REALMYROOM.item_storage_dragdrop", FRCMD_OWNERDRAW,
	CRealMyRoomMain::OnOwnerDrawItemStorageDragDrop)

END_FRESH_MSGMAP()

void SetMainTabImg(const char** const img, int index)
{
	img[0] = MakeStr("stab_%d_n", index);
	img[1] = MakeStr("stab_%d_d", index);
	img[2] = MakeStr("stab_%d_d", index);
}

void SetSubTabImg(const char** const img, int index)
{
	img[0] = MakeStr("stext_%d_n", index);
	img[1] = MakeStr("stext_%d_n", index);
	img[2] = MakeStr("stext_%d_n", index);
}

bool EmptyCompares(const void* a, const void* b)
{
	if (a)
	{
		if (b == NULL)
			return true;
	}

	return false;
}

void CRealMyRoomMain::OnAnotherHand(int key, void* param)
{
	if (key == 0)
		ShowPet(*(unsigned long*)param);
}

void CRealMyRoomMain::OpenLayout(const char* name)
{
	WSendPacket packet((enumClientPacket)0x5c);
	packet.Send(TO_GAME);

	g_pFresh->OpenLayout(name, this, false);
}

void CRealMyRoomMain::CloseLayout()
{
	if (m_pLayout)
		m_pLayout->Close(true);

	g_pFresh->CloseLayout();
}

void CRealMyRoomMain::ShowPet(unsigned long typeId)
{
	if (typeId == 0)
		return;

	m_anotherHand.Enter();
	if (m_pPetArea == NULL)
	{
		m_anotherHand.Leave();
		return;
	}

	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}

	WRect rect = m_pPetArea->GetRect();
	CExhibition* pExhibition = NULL;
	bool bResult;

	switch (typeId >> 26)
	{
	case 1:
		if (m_subTab == 13 || m_subTab == 14 || m_subTab == 15)
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f, 0.35f, true);
			pExhibition->SetArea(rect.w, rect.h);
			pExhibition->SetDistanceRange(16.0f, 20.0f);
			pExhibition->SetDistance(18.0f, true, false);
		}
		else
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pExhibition->SetArea(rect.w, rect.h);
			pExhibition->SetDistanceRange(8.0f, 12.0f);
			pExhibition->SetDistance(10.0f, true, false);
		}

		if (IsLocalContent(S4_UCC) && m_pUccDrawDlg &&
			m_pUccDrawDlg->m_pItemInfo)
		{
			unsigned long tid = m_pUccDrawDlg->m_pItemInfo->tid;
			char uccIndex[9];
			strncpy(uccIndex, m_pUccDrawDlg->m_pItemInfo->UccIndex, 9);

			sItemInfo* pInfo = GetMyItemInfo(tid, uccIndex, 0);

			char uccIndexList[24][9];
			pExhibition->SetAutoRecoverCamera(false);
			strcpy(uccIndexList[(tid >> 13) & 0x1f], uccIndex);

			CPartTidList parts = Doc()->GetMyPartTidList();
			parts.SetTid(tid, pInfo ? pInfo->guid : 0, uccIndex,
				pInfo ? pInfo->Seq : 0);
			bResult = pExhibition->SetModel(parts, Doc()->GetMyAuxPartTidList(),
				NULL, 3.0f, 0);
		}
		else
		{
			bResult = pExhibition->SetModel(Doc()->GetMyPartTidList(),
				Doc()->GetMyAuxPartTidList(), NULL, 3.0f, 0);
		}
		break;

	case 7:
	case 8:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistanceRange(9.0f, 13.0f);
		pExhibition->SetDistance(11.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 16:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistanceRange(4.0f, 8.0f);
		pExhibition->SetDistance(6.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 5:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistance(1.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 4:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistance(6.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 11:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistance(18.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 6:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistance(6.0f, true, false);
		pExhibition->SetDistanceRange(5.0f, 7.0f);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 18:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f, 0.0f, true);
			pExhibition->SetArea(rect.w * 0.5f, rect.h * 0.5f);
			pExhibition->SetDistance(15.0f, true, false);
			bResult = pExhibition->SetModel(typeId);
		}
		break;

	case 31:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistanceRange(5.0f, 7.0f);
		if ((typeId & 0x3c00000) == 0xc00000)
			pExhibition->SetDistance(5.0f, true, false);
		else
			pExhibition->SetDistance(6.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	default:
		m_anotherHand.Leave();
		return;
	}

	if (!bResult)
	{
		delete pExhibition;
	}
	else
	{
		pExhibition->SetControlFlag(15);
		m_pExhibition = pExhibition;
	}

	m_anotherHand.Leave();
}

void CRealMyRoomMain::HidePet(bool bWait)
{
	if (bWait)
	{
		m_anotherHand.WaitUntilWorkDone(9999);
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
	}
	else
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
	}
}

void CRealMyRoomMain::OnMyRoomInit(int param)
{
	m_trophyList.clear();

	SetConstantBGM("myroom.mp3", false, false);
	TurnOff3DBackground();
}

void CRealMyRoomMain::OnMyRoomFinish()
{
	m_pOverBarTitle->SetBgImg("title_myroom");
	m_pPetArea = m_pPet;
	Reinitialize(false);

	if (Doc()->m_bRefreshCamera)
		HandleMsg(MsgObject(NULL, 170, 1, 0, 0, 0, 0));
}

void CRealMyRoomMain::Reinitialize(bool bKeepCaddie)
{
	if (MyUID() != Doc()->m_replayTargetUID)
		return;

	ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
	ChangeClubset(Doc()->m_myInfo.userEquip.guidClubSet);
	ChangeAztec(Doc()->m_myInfo.userEquip.tidBall);

	if (!bKeepCaddie)
	{
		ChangeCaddie(Doc()->m_myInfo.userEquip.guidCaddie);
		ChangeMascot(Doc()->m_myInfo.userEquip.guidMascot);
	}

	ChangeMainTab(m_mainTab, true);
	ChangeSubTab(m_subTab, true);
	m_statTab = (statTab_t)(m_mainTab > 2);
	SetLevelBar();
	m_selTab = (selTab_t)0;

	memcpy(m_skinTypeId, Doc()->m_myInfo.userEquip.tidSkin,
		sizeof(m_skinTypeId));
	for (int i = 0; i < 6; i++)
	{
		if (m_skinTypeId[i])
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[i]);
			if (pSkin && (m_skinTypeId[i] & 0x3c00000) == 0)
			{
				m_skinScrollDir[0] = pSkin->HScroll;
				m_skinScrollDir[1] = pSkin->VScroll;
				m_skinScroll[0] = 0.0f;
				m_skinScroll[1] = 0.0f;
			}
		}
	}
}

void CRealMyRoomMain::ReinitializePC(ePCBangItem item)
{
	if (item == PCITEM_CADDIE)
		ChangeCaddie(Doc()->m_realEquip.guidCaddie);
	else if (item == PCITEM_MASCOT)
		ChangeMascot(Doc()->m_realEquip.guidMascot);
}

void CRealMyRoomMain::InitMyRoomDialogs()
{
	m_p424 = NULL;
	m_pOptionDlg = NULL;
	m_pKbdManualDlg = NULL;
	m_pServerDlg = NULL;
	m_pGiftDlg = NULL;
	m_pWaitDlg = NULL;
	m_pNotifyOneDlg = NULL;
	m_fKbdManualTime = -10.0f;
	m_pEnchantDlg = NULL;
	m_pDowngradeDlg2 = NULL;
	m_pUpgrade = NULL;
	m_pDowngradeDlg = NULL;
	m_pPangGiftDlg = NULL;
	m_pMascotInfoDlg = NULL;
	m_pNoteDlg = NULL;
	m_pFortuneDlg = NULL;
	m_pFortuneOpenDlg = NULL;
	m_pReplayModeDlg = NULL;
	m_pPostReciveDlg = NULL;
	m_pPostSendDlg = NULL;
	m_pAuthorityDlg = NULL;
	if (IsLocalContent(S4_FURNITURE_ABILITY))
		m_pFurnitureAbilityDlg = NULL;

	m_b2c4 = true;
	m_pUccDrawDlg = NULL;
	m_pUccCopyDlg = NULL;
	m_pUniteResultDlg = NULL;
	m_pCardUseDlg = NULL;
	m_pCardRemoveDlg = NULL;
	m_pItemStorageNotice = NULL;
	m_pItemStorageAccessPanel = NULL;
	m_bItemStorageVisible = false;
	m_itemStoragePos = WPoint(0.0f, 0.0f);
	m_pItemStorageBG = NULL;
	m_pItemStoragePang = NULL;
	m_pItemStoragePangMinus = NULL;
	m_pItemStoragePangPlus = NULL;
	m_pItemStorageList = NULL;
	m_pItemStorageIndexMsg = NULL;
	m_pItemStoragePageNext = NULL;
	m_pItemStoragePagePrev = NULL;
	m_pItemStorageClose = NULL;
	m_pItemStorageDragDrop = NULL;
	SetItemStorageVisible(false);
}

void CRealMyRoomMain::InitMyRoomControls()
{
	m_pMiddleUI = NULL;
	m_pSelectedName = NULL;
	m_pStatPart = NULL;
	m_pStatAll = NULL;
	m_pTabMain = NULL;
	m_pTabSub = NULL;
	m_p2fc = NULL;
	m_p300 = NULL;
	m_pItemList = NULL;
	m_pTabTxt[0] = NULL;
	m_pTabTxt[1] = NULL;
	m_pTabTxt[2] = NULL;
	m_pPetArea = NULL;
	m_pExhibition = NULL;
	m_pSelItem = NULL;
	memset(m_pStatBar, 0, sizeof(m_pStatBar));
	memset(m_pStatEdit, 0, sizeof(m_pStatEdit));
	m_pSelItemPrev = NULL;
	m_pSelItemNext = NULL;
	memset(m_skinTypeId, 0, sizeof(m_skinTypeId));
	memset(m_skinScrollDir, 0, sizeof(m_skinScrollDir));
	memset(m_skinScroll, 0, sizeof(m_skinScroll));
	m_pPet = NULL;
	m_pSkinPet = NULL;
	m_pItemSlot = NULL;
	m_pEquipCancel = NULL;
	m_pEquipReset = NULL;
	m_pItemSlotBox = NULL;
	m_b784 = false;
	m_p398 = NULL;
	m_p318 = NULL;
	m_pSelItemIcon = NULL;
	m_pStatBox = NULL;
	m_pSelFrame = NULL;
	m_pSelect[3] = NULL;
	m_pSelectDivide = NULL;
	m_pMasterSign = NULL;
	m_pTeamBase = NULL;
	m_pDowngrade = NULL;

	m_pItemDescBg = NULL;
	m_pItemDesc = NULL;
	m_pItemDescBg2 = NULL;
	m_pItemDesc2 = NULL;
	m_pGiftBg = NULL;
	m_pGiftDesc = NULL;

	m_pFurnitureArrange = NULL;

	m_pEffectView = NULL;
	m_p688 = NULL;

	m_pSkinPet2 = NULL;
	m_pStatFrame = NULL;
	m_pStatText = NULL;

	m_pFurnitureWnd[0] = NULL;
	m_pFurnitureWnd[1] = NULL;
	m_pFurnitureUI[0] = NULL;
	m_pFurnitureUI[1] = NULL;
	m_pFurnitureUI[2] = NULL;
	m_pFurnitureUI[3] = NULL;
	m_pFurnitureUI[4] = NULL;
	m_pFurnitureUI[5] = NULL;
	m_pFurnitureUI[6] = NULL;
	m_pFurnitureUI[7] = NULL;
	m_pFurnitureUI[8] = NULL;

	m_pRightMenu = NULL;

	m_pFurnitureTabMain = NULL;
	m_pFurnitureList = NULL;

	m_uiHideMode = 0;
	m_prevUiHideMode = 0;
	memset(m_bFlag, 1, sizeof(m_bFlag));
	m_bFlag[3] = false;

	m_pFurnitureReset = NULL;
	m_pFurnitureBack = NULL;
	m_pFurnitureSave = NULL;
	m_pFurnitureOut = NULL;

	for (int i = 0; i < 8; i++)
		m_pFurnitureBackGround[i] = NULL;

	for (int j = 0; j < 3; j++)
		m_pFurnitureScroll[j] = NULL;

	m_pFurnitureListNum = NULL;
	m_pFurnitureListMaxNum = NULL;
	m_pFurnitureBtnListNum = NULL;
	m_pFurniturePup = NULL;
	m_pFurniturePdown = NULL;
	m_pBtnMenuList = NULL;

	m_p408[0] = NULL;
	m_p408[1] = NULL;
	m_p408[2] = NULL;
	m_p408[3] = NULL;
	m_p408[4] = NULL;
	m_p408[5] = NULL;

	m_bd44 = false;
	m_bShowCharacter = true;
	m_bd46 = false;

	m_offlineShopEdit = 0;
}

void CRealMyRoomMain::InitMyRoomVariables()
{
	m_bSelectItemList = false;
	m_caddieTypeId = 0;
	m_caddieUid = 0;
	m_caddieParts = 0;
	m_mascotTypeId = 0;
	m_mascotUid = 0;
	m_charTypeId = 0;
	m_charUid = 0;
	m_clubSetUid = 0;
	m_aztecTypeId = 0;
	m_pCharStat = NULL;
	m_pCharParts = NULL;
	if (IsLocalContent(S4_UCC))
		memset(m_partsUid, 0, sizeof(m_partsUid));

	m_pAuxParts = NULL;
	m_statTab = (statTab_t)0;
	m_mainTab = (mainTab_t)0;
	m_subTab = (subTab_t)21;
	m_pClubSetStat = NULL;
	m_selTab = (selTab_t)0;
	m_statSubject = 0;

	m_giftMaxPage = 0;
	m_giftPageNum = 0;
	m_giftPage = Doc()->m_cardPackMode;
	m_giftPage = Doc()->m_cardPackSlot;
	m_giftPageGroup = 0;

	for (int i = 0; i < 7; i++)
		m_giftPageRect[i] = WRect(0.0f, 0.0f, 0.0f, 0.0f);

	m_pEffectView = NULL;
	m_p688 = NULL;
	m_pSkinPet2 = NULL;
	m_pStatFrame = NULL;
	m_pStatText = NULL;
	m_pFurnitureWnd[0] = NULL;
	m_pFurnitureWnd[1] = NULL;
	m_pFurnitureUI[0] = NULL;
	m_pFurnitureUI[1] = NULL;
	m_pFurnitureUI[2] = NULL;
	m_pFurnitureUI[3] = NULL;
	m_pFurnitureUI[4] = NULL;
	m_pFurnitureUI[5] = NULL;
	m_pFurnitureUI[6] = NULL;
	m_pFurnitureUI[7] = NULL;
	m_pFurnitureUI[8] = NULL;
	m_bGiftMoveReady = true;
	m_bc26 = false;
	m_bc24 = false;

	m_furnitureTab = (eFurniture_Tab)0;
	m_furniturePage = 0;
	m_furnitureMaxPage = 1;
	m_pMarkBegin = NULL;
}

extern const char* dot_files[5][2];

void CRealMyRoomMain::LoadMyRoomImages()
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 2; j++)
			m_pStatDot[i][j] = g_pFresh->RegisterBitmap(dot_files[i][j]);

	m_pSelect[0] = g_pFresh->RegisterBitmap("select_up");
	m_pSelect[1] = g_pFresh->RegisterBitmap("select_mid");
	m_pSelect[2] = g_pFresh->RegisterBitmap("select_low");
	m_pSelect[3] = g_pFresh->RegisterBitmap("select_empty");

	for (int k = 0; k < 5; k++)
	{
		m_pSelectNum[k][0] =
			g_pFresh->RegisterBitmap(MakeStr("select_%d_n", k + 1));
		m_pSelectNum[k][1] =
			g_pFresh->RegisterBitmap(MakeStr("select_%d_o", k + 1));
	}
	m_pSelectDivide = g_pFresh->RegisterBitmap(MakeStr("select_divide"));

	m_pSelectCursor[0] = g_pFresh->RegisterBitmap("select_cursor1");
	m_pSelectCursor[1] = g_pFresh->RegisterBitmap("select_cursor0");

	m_pStatChar[0] = g_pFresh->RegisterBitmap("stat_char_n");
	m_pStatChar[1] = g_pFresh->RegisterBitmap("stat_char_o");
	m_pStatChar[2] = g_pFresh->RegisterBitmap("stat_char_d");
	m_pStatClub[0] = g_pFresh->RegisterBitmap("stat_club_n");
	m_pStatClub[1] = g_pFresh->RegisterBitmap("stat_club_o");
	m_pStatClub[2] = g_pFresh->RegisterBitmap("stat_club_d");
	m_pStatCaddie[0] = g_pFresh->RegisterBitmap("stat_caddie_n");
	m_pStatCaddie[1] = g_pFresh->RegisterBitmap("stat_caddie_o");
	m_pStatCaddie[2] = g_pFresh->RegisterBitmap("stat_caddie_d");

	m_pStatAllBtn[0] = g_pFresh->RegisterBitmap("stat_all_n");
	m_pStatAllBtn[1] = g_pFresh->RegisterBitmap("stat_all_o");
	m_pStatAllBtn[2] = g_pFresh->RegisterBitmap("stat_all_d");

	m_pItemBase[0] = g_pFresh->RegisterBitmap("item_base_n");
	m_pItemBase[1] = g_pFresh->RegisterBitmap("item_base_o");
	m_pItemBase[2] = g_pFresh->RegisterBitmap("item_base_d");
	m_pItemBase[3] = g_pFresh->RegisterBitmap("item_base_empty");

	m_miniBtn[0].pNormal = g_pFresh->RegisterBitmap("giftmove_btn_n");
	m_miniBtn[0].pOver = g_pFresh->RegisterBitmap("giftmove_btn_o");
	m_miniBtn[0].rect =
		WRect(103.0f, 66.0f, (float)m_miniBtn[0].pNormal->Width(),
			(float)m_miniBtn[0].pNormal->Height());

	m_miniBtn[4].pNormal = g_pFresh->RegisterBitmap("answer_btn_n");
	m_miniBtn[4].pOver = g_pFresh->RegisterBitmap("answer_btn_o");
	m_miniBtn[4].rect =
		WRect(144.0f, 66.0f, (float)m_miniBtn[4].pNormal->Width(),
			(float)m_miniBtn[4].pNormal->Height());

	m_miniBtn[1].pNormal = g_pFresh->RegisterBitmap("equip_btn_n");
	m_miniBtn[1].pOver = g_pFresh->RegisterBitmap("equip_btn_o");
	m_miniBtn[1].rect =
		WRect(103.0f, 66.0f, (float)m_miniBtn[1].pNormal->Width(),
			(float)m_miniBtn[1].pNormal->Height());

	m_miniBtn[2].pNormal = g_pFresh->RegisterBitmap("delete_btn_n");
	m_miniBtn[2].pOver = g_pFresh->RegisterBitmap("delete_btn_o");
	m_miniBtn[2].rect =
		WRect(144.0f, 66.0f, (float)m_miniBtn[2].pNormal->Width(),
			(float)m_miniBtn[2].pNormal->Height());

	m_miniBtn[3].pNormal = g_pFresh->RegisterBitmap("reemploy_btn_n");
	m_miniBtn[3].pOver = g_pFresh->RegisterBitmap("reemploy_btn_o");
	m_miniBtn[3].rect =
		WRect(144.0f, 66.0f, (float)m_miniBtn[3].pNormal->Width(),
			(float)m_miniBtn[3].pNormal->Height());

	m_miniBtn[5].pNormal = g_pFresh->RegisterBitmap("clock");
	m_miniBtn[5].pOver = g_pFresh->RegisterBitmap("clock");
	m_miniBtn[5].rect =
		WRect(32.0f, 30.0f, (float)m_miniBtn[5].pNormal->Width(),
			(float)m_miniBtn[5].pNormal->Height());

	if (IsLocalContent(S4_REPLAY_SYSTEM))
	{
		m_miniBtn[7].pNormal = g_pFresh->RegisterBitmap("replaymode_btn_n");
		m_miniBtn[7].pOver = g_pFresh->RegisterBitmap("replaymode_btn_o");
		m_miniBtn[7].rect =
			WRect(144.0f, 66.0f, (float)m_miniBtn[7].pNormal->Width(),
				(float)m_miniBtn[7].pNormal->Height());

		m_miniBtn[8].pNormal = g_pFresh->RegisterBitmap("replaydel_btn_n");
		m_miniBtn[8].pOver = g_pFresh->RegisterBitmap("replaydel_btn_o");
		m_miniBtn[8].rect =
			WRect(103.0f, 66.0f, (float)m_miniBtn[8].pNormal->Width(),
				(float)m_miniBtn[8].pNormal->Height());
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		m_miniBtn[9].pNormal = g_pFresh->RegisterBitmap("card_btn_n");
		m_miniBtn[9].pOver = g_pFresh->RegisterBitmap("card_btn_o");
		m_miniBtn[9].rect =
			WRect(144.0f, 66.0f, (float)m_miniBtn[9].pNormal->Width(),
				(float)m_miniBtn[9].pNormal->Height());
	}

	if (IsLocalContent(S4_UCC))
	{
		m_miniBtn[11].pNormal = g_pFresh->RegisterBitmap("ucc_draw_n");
		m_miniBtn[11].pOver = g_pFresh->RegisterBitmap("ucc_draw_o");

		m_miniBtn[13].pNormal = g_pFresh->RegisterBitmap("ucc_redraw_btn_n");
		m_miniBtn[13].pOver = g_pFresh->RegisterBitmap("ucc_redraw_btn_o");

		m_miniBtn[12].pNormal = g_pFresh->RegisterBitmap("ucc_copy_n");
		m_miniBtn[12].pOver = g_pFresh->RegisterBitmap("ucc_copy_o");

		m_uccCopyRect =
			WRect(103.0f, 66.0f, (float)m_miniBtn[11].pNormal->Width(),
				(float)m_miniBtn[11].pNormal->Height());
		m_uccDrawRect =
			WRect(144.0f, 66.0f, (float)m_miniBtn[11].pNormal->Width(),
				(float)m_miniBtn[11].pNormal->Height());

		m_pUccState[0] = g_pFresh->RegisterBitmap("ucc_state_original");
		m_pUccState[1] = g_pFresh->RegisterBitmap("ucc_state_copied");
		m_pUccState[2] = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	}

	if (IsLocalContent(S4_REAL_MYROOM))
	{
		m_miniBtn[10].pNormal = g_pFresh->RegisterBitmap("rmr_arrange_btn_n");
		m_miniBtn[10].pOver = g_pFresh->RegisterBitmap("rmr_arrange_btn_o");

		m_miniBtn[10].rect =
			WRect(144.0f, 66.0f, (float)m_miniBtn[10].pNormal->Width(),
				(float)m_miniBtn[10].pNormal->Height());
	}

	m_pGender[0] = g_pFresh->RegisterBitmap("i_male");
	m_pGender[1] = g_pFresh->RegisterBitmap("i_female");
	m_pMasterSign = g_pFresh->RegisterBitmap("master_sign");
	m_pTeamBase = g_pFresh->RegisterBitmap("team_base");

	m_pItemSlotBmp[0] = g_pFresh->RegisterBitmap("item_slot_n");
	m_pItemSlotBmp[1] = g_pFresh->RegisterBitmap("item_slot_o");
	m_pItemSlotBmp[2] = g_pFresh->RegisterBitmap("item_slot_d");
	m_pItemSlotBmp[3] = g_pFresh->RegisterBitmap("item_slot_empty");

	m_pItemDescBmp[0] = g_pFresh->RegisterBitmap("itemdesc_01");
	m_pItemDescBmp[1] = g_pFresh->RegisterBitmap("itemdesc_02");

	m_pGiftPage[0] = g_pFresh->RegisterBitmap("gift_arrow_white_2");
	m_pGiftPage[1] = g_pFresh->RegisterBitmap("gift_number_01");
	m_pGiftPage[2] = g_pFresh->RegisterBitmap("gift_number_02");
	m_pGiftPage[3] = g_pFresh->RegisterBitmap("gift_number_03");
	m_pGiftPage[4] = g_pFresh->RegisterBitmap("gift_arrow_white");

	m_pArrangeItemBase[0] = g_pFresh->RegisterBitmap("rmr_arg_item_base_n");
	m_pArrangeItemBase[1] = g_pFresh->RegisterBitmap("rmr_arg_item_base_d");
	m_pArrangeItemBase[2] = g_pFresh->RegisterBitmap("rmr_arg_item_base_s");
	m_pArrangeItemBase[3] = g_pFresh->RegisterBitmap("rmr_arg_item_base_empty");

	m_pTipFrame[0] = g_pFresh->RegisterBitmap("rmr_tip_frm_01");
	m_pTipFrame[1] = g_pFresh->RegisterBitmap("rmr_tip_frm_02");
	m_pTipFrame[2] = g_pFresh->RegisterBitmap("rmr_tip_frm_03");
	m_pTipFrame[3] = g_pFresh->RegisterBitmap("rmr_tip_frm_04");
	m_pTipFrame[4] = g_pFresh->RegisterBitmap("rmr_tip_frm_05");
	m_pTipFrame[5] = g_pFresh->RegisterBitmap("rmr_tip_frm_06");
	m_pTipFrame[6] = g_pFresh->RegisterBitmap("rmr_tip_frm_07");
	m_pTipFrame[7] = g_pFresh->RegisterBitmap("rmr_tip_frm_08");
	m_pTipFrame[8] = g_pFresh->RegisterBitmap("rmr_tip_frm_09");

	m_pMenuIcon[0] = g_pFresh->RegisterBitmap("rmr_f_wardrobe");
	m_pMenuIcon[1] = g_pFresh->RegisterBitmap("rmr_f_post");
	m_pMenuIcon[2] = g_pFresh->RegisterBitmap("rmr_f_arrange");
	m_pMenuIcon[3] = g_pFresh->RegisterBitmap("rmr_f_shop");
	m_pMenuIcon[4] = g_pFresh->RegisterBitmap("rmr_f_shop");
	m_pMenuIcon[5] = g_pFresh->RegisterBitmap("rmr_f_album");
	m_pMenuIcon[6] = g_pFresh->RegisterBitmap("rmr_f_tv");
	m_pMenuIcon[7] = g_pFresh->RegisterBitmap("rmr_f_visitors_book");
	m_pMenuIcon[8] = g_pFresh->RegisterBitmap("rmr_f_authority");
	m_pMenuIcon[12] = g_pFresh->RegisterBitmap("rmr_f_close");
	m_pMenuIcon[9] = g_pFresh->RegisterBitmap("rmr_f_camera");
	m_pMenuIcon[10] = g_pFresh->RegisterBitmap("rmr_f_hide");

	m_pMenuText[0] = g_pFresh->RegisterBitmap("rmr_f_wardrobe_text");
	m_pMenuText[1] = g_pFresh->RegisterBitmap("rmr_f_post_text");
	m_pMenuText[2] = g_pFresh->RegisterBitmap("rmr_f_arrange_text");
	m_pMenuText[3] = g_pFresh->RegisterBitmap("rmr_f_shop_text");
	m_pMenuText[4] = g_pFresh->RegisterBitmap("rmr_f_shop_text");
	m_pMenuText[5] = g_pFresh->RegisterBitmap("rmr_f_album_text");
	m_pMenuText[6] = g_pFresh->RegisterBitmap("rmr_f_tv_text");
	m_pMenuText[7] = g_pFresh->RegisterBitmap("rmr_f_visitors_book_text");
	m_pMenuText[8] = g_pFresh->RegisterBitmap("rmr_f_authority_text");
	m_pMenuText[12] = g_pFresh->RegisterBitmap("rmr_f_close_text");
	m_pMenuText[9] = g_pFresh->RegisterBitmap("rmr_f_camera_text");
	m_pMenuText[10] = g_pFresh->RegisterBitmap("rmr_f_hide_text");

	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		m_pMenuIcon[11] = g_pFresh->RegisterBitmap("rmr_f_secbox");
		m_pMenuText[11] = g_pFresh->RegisterBitmap("rmr_f_secbox_text");
	}

	m_pMenuOver = g_pFresh->RegisterBitmap("rmr_menu_o");
	m_pMenuDisable = g_pFresh->RegisterBitmap("rmr_menu_d");

	m_pMarkBegin = g_pFresh->RegisterBitmap("mark_begin");
}

void CRealMyRoomMain::LoadItemImages()
{
	for (CharIterator it = Doc()->m_itemManager.m_CharMap.begin();
		it != Doc()->m_itemManager.m_CharMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (PartIterator it = Doc()->m_itemManager.m_PartMap.begin();
		it != Doc()->m_itemManager.m_PartMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end() &&
			((*it).second.c.TypeId & 0x600) == 0)
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	CaddieIterator itCaddie;
	for (itCaddie = Doc()->m_itemManager.m_CaddieMap.begin();
		itCaddie != Doc()->m_itemManager.m_CaddieMap.end(); ++itCaddie)
	{
		if (m_itemIcon.find((*itCaddie).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*itCaddie).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*itCaddie).second.c.TypeId, pBitmap));
		}
	}

	for (CadItemIterator it = Doc()->m_itemManager.m_CadItemMap.begin();
		it != Doc()->m_itemManager.m_CadItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (ItemIterator it = Doc()->m_itemManager.m_ItemMap.begin();
		it != Doc()->m_itemManager.m_ItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (ClubSetIterator it = Doc()->m_itemManager.m_ClubSetMap.begin();
		it != Doc()->m_itemManager.m_ClubSetMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (BallIterator it = Doc()->m_itemManager.m_BallMap.begin();
		it != Doc()->m_itemManager.m_BallMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (SkinIterator it = Doc()->m_itemManager.m_SkinMap.begin();
		it != Doc()->m_itemManager.m_SkinMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (HairStyleIterator it = Doc()->m_itemManager.m_HairStyleMap.begin();
		it != Doc()->m_itemManager.m_HairStyleMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (AuxPartIterator it = Doc()->m_itemManager.m_AuxPartMap.begin();
		it != Doc()->m_itemManager.m_AuxPartMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (QuestDropIterator it = Doc()->m_itemManager.m_QuestDropMap.begin();
		it != Doc()->m_itemManager.m_QuestDropMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (QuestIterator it = Doc()->m_itemManager.m_QuestMap.begin();
		it != Doc()->m_itemManager.m_QuestMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap = g_pFresh->RegisterBitmap((*it).second.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.TypeId, pBitmap));
		}
	}

	for (MascotIterator it = Doc()->m_itemManager.m_MascotMap.begin();
		it != Doc()->m_itemManager.m_MascotMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->RegisterBitmap((*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}
}

void CRealMyRoomMain::CloseAllDialogs()
{
	FrWnd* pWnd = DYNAMIC_CAST(FrWnd,
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
	if (pWnd)
		pWnd->Close(true);

	CTaskMain::CloseAllDialogs();
}

void CRealMyRoomMain::OnMiddleUIInit(int param)
{
	m_pMiddleUI = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
}

void CRealMyRoomMain::OnTabTxt1Init(int param)
{
	m_pTabTxt[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnTabTxt2Init(int param)
{
	m_pTabTxt[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnTabTxt3Init(int param)
{
	m_pTabTxt[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnSelectedNameInit(int param)
{
	m_pSelectedName = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::ChangeSubTab(subTab_t tab, bool bForce)
{
	if (!bForce && m_subTab == tab)
		return;

	bool bChangePet = false;
	if (((m_subTab == 21 || m_subTab <= 4 || m_subTab == 22 || m_subTab == 26 ||
			 m_subTab == 16 || m_subTab == 33) &&
			(tab == 13 || tab == 14 || tab == 15)) ||
		((m_subTab == 13 || m_subTab == 14 || m_subTab == 15 ||
			 m_subTab == 33) &&
			m_mainTab == 0) ||
		(m_mainTab == 0 && m_subTab == 16))
	{
		bChangePet = true;
	}

	if (tab == 5)
		tab = (subTab_t)1;

	m_subTab = tab;

	m_bSelectItemList = false;
	BuildDisplayStand(m_subTab);
	SetLevelBar();

	m_pItemDesc->ClearLine();
	m_pTabSub->Select(m_subTab);

	if (m_pGiftBg)
		m_pGiftBg->SetVisible(false);

	if (m_pGiftDesc)
		m_pGiftDesc->SetVisible(false);

	m_pStatBox->SetVisible(false);

	if (!m_pItemList->IsVisible())
	{
		m_pItemList->SetVisible(true);
		SetCardBookVisible(false);
	}

	switch (m_subTab)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 26:
		m_pItemList->SetMultiSelect(true);
		if (bChangePet || m_uiHideMode != 1)
			SafeLoadPet(&m_charTypeId, bChangePet);

		m_pStatBox->SetVisible(true);
		break;

	case 6:
		if (m_pSelectedName)
		{
			m_pSelectedName->SetVisible(true);
			m_pSelectedName->SetCaption11(
				"\xbc\xb1\xc5\xc3\xc7\xd1 \xc5\xac\xb7\xb4");
		}

		m_pItemList->SetMultiSelect(false);
		ChangeClubset(m_clubSetUid);
		m_pStatBox->SetVisible(true);
		break;

	case 7:
		m_pItemList->SetMultiSelect(false);
		ChangeAztec(m_aztecTypeId);
		break;

	case 8:
		m_pItemList->SetMultiSelect(false);
		HidePet(true);
		break;

	case 9:
		m_pItemList->SetMultiSelect(false);
		HidePet(true);
		break;

	case 10:
		m_pItemList->SetMultiSelect(false);
		m_pStatBox->SetVisible(true);
		break;

	case 12:
		m_pItemList->SetMultiSelect(false);
		break;

	case 13:
	case 14:
	case 15:
	case 16:
		m_pItemList->SetMultiSelect(false);
		SafeLoadPet(&m_charTypeId, bChangePet);
		break;

	case 34:
		TurnOffItemStatus();
		HidePet(true);
		break;

	case 17:
		if (!IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
		{
			m_pItemList->SetMultiSelect(false);
			BuildDisplayStand((subTab_t)17);
			HidePet(true);

			if (m_pGiftBg)
				m_pGiftBg->SetVisible(true);

			if (m_pGiftDesc)
				m_pGiftDesc->SetVisible(true);

			if (IsLocalCountry(91))
			{
				m_giftMaxPage = Doc()->m_cardPackItem;
				m_giftPage = Doc()->m_cardPackSlot;
			}
			else
			{
				unsigned long pageNum = Doc()->m_cardPackPageNum;
				unsigned long curPage = Doc()->m_cardPackCurPage;
				if (pageNum && curPage)
				{
					m_giftMaxPage = curPage / pageNum;
					if (curPage % pageNum)
						m_giftMaxPage = m_giftMaxPage + 1;
				}

				m_giftPage = Doc()->m_cardPackMode;
			}
		}
		break;

	case 18:
		m_pItemList->SetMultiSelect(false);
		break;

	case 19:
		m_pItemList->SetMultiSelect(false);
		break;

	case 20:
		m_pItemList->SetMultiSelect(false);
		break;

	case 23:
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			m_pItemList->SetMultiSelect(false);
			HidePet(true);
		}
		break;

	case 27:
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			m_pItemList->SetMultiSelect(false);
			m_pItemList->SetVisible(false);
			HidePet(true);

			SetCardBookVisible(true);
		}
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			m_pItemList->SetMultiSelect(false);

			if (m_pFurnitureArrange)
				m_pFurnitureArrange->SetVisible(true);

			HidePet(true);
		}
		break;
	}

	switch (m_subTab)
	{
	case 8:
		if (m_pItemSlotBox)
			m_pItemSlotBox->SetVisible(true);
		break;

	default:
		if (m_pItemSlotBox)
			m_pItemSlotBox->SetVisible(false);
		break;
	}

	switch (m_subTab)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 21:
	case 22:
	case 26:
		m_pUpgrade->SetVisible(true);
		m_pDowngrade->SetVisible(true);
		break;

	default:
		m_pUpgrade->SetVisible(false);
		m_pDowngrade->SetVisible(false);
		break;
	}

	SetPetAreaRect();
	SetItemDesc();

	if (m_pItemList && m_pItemList->GetScrollBar())
		m_pItemList->GetScrollBar()->SetCurTopRow(0);
}

void CRealMyRoomMain::SetPetAreaRect()
{
	switch (m_subTab)
	{
	case 13:
	case 14:
	case 15:
		m_pPetArea = m_pSkinPet;
		break;

	case 16:
		m_pPetArea = m_pSkinPet2;
		break;

	default:
		m_pPetArea = m_pPet;
		break;
	}
}

void CRealMyRoomMain::ChangeMainTab(mainTab_t tab, bool bForce)
{
	if (bForce)
		m_pTabMain->Select(m_mainTab);
	else if (m_mainTab == tab)
		return;

	m_mainTab = tab;
	m_pTabSub->ClearTab();

	const char* img[3] = { NULL };

	if (m_pEffectView)
		m_pEffectView->SetVisible(true);

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(false);

	switch (m_mainTab)
	{
	case 0:
		m_pTabTxt[0]->SetBgImg("mt_character");
		TurnOnItemSelect();

		SetSubTabImg(img, 22);
		m_pTabSub->AddTab(img, 21, NULL);

		for (int i = 1; i <= 4; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}

		if (IsLocalContent(S4_UCC))
		{
			SetSubTabImg(img, 27);
			m_pTabSub->AddTab(img, 26, NULL);
		}

		SetSubTabImg(img, 23);
		m_pTabSub->AddTab(img, 22, NULL);
		break;

	case 2:
		m_pTabTxt[0]->SetBgImg("mt_caddie");
		TurnOffItemSelect();

		for (int i = 10; i < 11; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;

	case 1:
		m_pTabTxt[0]->SetBgImg("mt_item");
		if (m_pSelectedName)
			m_pSelectedName->SetVisible(false);
		TurnOffItemSelect();

		for (int i = 6; i <= 9; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}

		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			SetSubTabImg(img, 28);
			m_pTabSub->AddTab(img, 27, NULL);
		}
		break;

	case 3:
		m_pTabTxt[0]->SetBgImg("mt_mascot");
		TurnOffItemSelect();

		for (int i = 12; i < 13; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;

	case 4:
		m_pTabTxt[0]->SetBgImg("mt_skin");
		TurnOffItemSelect();

		for (int i = 13; i <= 16; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}

		SetSubTabImg(img, 34);
		m_pTabSub->AddTab(img, 33, NULL);

		SetSubTabImg(img, 35);
		m_pTabSub->AddTab(img, 34, NULL);
		break;

	case 5:
		if (IsLocalContent((localContentType_t)0x5b))
		{
			if (!Doc()->m_bRmrTabPending)
			{
				WSendPacket packet((enumClientPacket)0x92);
				packet.Encode4(1);
				packet.Send(TO_GAME);
				break;
			}

			m_pTabTxt[0]->SetBgImg("mt_mybox");
			TurnOffItemSelect();

			int offset;
			if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
				offset = 1;
			else
				offset = 0;

			for (int i = 17 + offset; i <= 20; i++)
			{
				SetSubTabImg(img, i + 1);
				m_pTabSub->AddTab(img, i, NULL);
			}

			if (IsLocalContent(S3_CADDIE_REPORT))
			{
				SetSubTabImg(img, 25);
				m_pTabSub->AddTab(img, 24, NULL);
			}

			if (IsLocalContent(S4_REPLAY_SYSTEM))
			{
				SetSubTabImg(img, 24);
				m_pTabSub->AddTab(img, 23, NULL);
			}
		}
		else
		{
			m_pTabTxt[0]->SetBgImg("mt_mybox");
			TurnOffItemSelect();

			int offset;
			if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
				offset = 1;
			else
				offset = 0;

			for (int i = 17 + offset; i <= 20; i++)
			{
				SetSubTabImg(img, i + 1);
				m_pTabSub->AddTab(img, i, NULL);
			}

			if (IsLocalContent(S3_CADDIE_REPORT))
			{
				SetSubTabImg(img, 25);
				m_pTabSub->AddTab(img, 24, NULL);
			}

			if (IsLocalContent(S4_REPLAY_SYSTEM))
			{
				SetSubTabImg(img, 24);
				m_pTabSub->AddTab(img, 23, NULL);
			}
		}
		break;

	case 6:
		m_pTabTxt[0]->SetBgImg("mt_interior");
		TurnOffItemSelect();

		SetSubTabImg(img, 22);
		m_pTabSub->AddTab(img, 32, NULL);

		for (int i = 28; i <= 31; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;
	}

	m_pItemDesc->SetVisible(false);
	m_pItemDescBg->SetVisible(false);

	m_pUnderBarMyRoom->Enable(false);

	if (!m_pExhibition)
		ChangeSubTab(m_subTab, true);
}

void CRealMyRoomMain::ChangeStatTab(statTab_t tab)
{
	m_statTab = tab;
	SetLevelBar();
}

void CRealMyRoomMain::ChangeSelTab(selTab_t tab)
{
	m_selTab = tab;
}

void CRealMyRoomMain::CharacterViewControl(bool bShow)
{
	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		if (bShow == true)
		{
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetState(2);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(false);
		}
		else
		{
			CRealMyRoom_AvatarManager::Instance()
				->m_pMyAvatar->ChangeCharacterInfo();
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetState(0);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(!bShow);
		}
	}
}

void CRealMyRoomMain::ChangeCharacter(unsigned long charUid)
{
	std::map<unsigned int, sCharacterInfo>::iterator it =
		Doc()->m_charMap.find(charUid);
	if (it == Doc()->m_charMap.end())
		return;

	sCharacterInfo& info = (*it).second;
	m_charTypeId = (*it).second.tid;
	m_pCharParts = (*it).second.tidParts;
	if (IsLocalContent(S4_UCC))
		memcpy(m_partsUid, (*it).second.ItemIdList, sizeof(m_partsUid));

	m_pAuxParts = (*it).second.tidAuxParts;
	m_pCharStat = (*it).second.PCL;
	m_charUid = (*it).second.guid;
	Doc()->m_myInfo.userEquip.guidChar = (*it).second.guid;

	if (MyUID() != Doc()->m_replayTargetUID)
	{
		if (m_mainTab == 0)
		{
			Doc()->BuildMyPartTidList(info);

			sCharacterInfo& curInfo =
				Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
			CRealMyRoom_Avatar* pAvatar =
				CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(
					Doc()->m_myInfo.info.dwGuid);
			if (pAvatar && pAvatar->m_charInfo.guid != curInfo.guid)
			{
				((CRealMyRoomTask*)GetTask())
					->RemoveAvatarBurden(
						CRealMyRoom_AvatarBurden(pAvatar->m_guid,
							pAvatar->m_charInfo, pAvatar->m_pPetFrame));

				pAvatar->SetCharactorInfo(curInfo);

				((CRealMyRoomTask*)GetTask())
					->AddAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid,
						curInfo, NULL));

				if (!m_bShowCharacter)
				{
					CFxSequence* pSeq =
						CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq)
						pSeq->m_pos = WVector(pAvatar->m_pos.x,
							pAvatar->m_pos.y + 5.0f, pAvatar->m_pos.z);
				}
			}
		}

		return;
	}

	if (m_mainTab == 0)
	{
		Doc()->BuildMyPartTidList(info);

		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_charTypeId, false);

		sCharacterInfo& curInfo =
			Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
		CRealMyRoom_Avatar* pAvatar =
			CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(
				Doc()->m_myInfo.info.dwGuid);
		if (pAvatar && pAvatar->m_charInfo.guid != curInfo.guid)
		{
			((CRealMyRoomTask*)GetTask())
				->RemoveAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid,
					pAvatar->m_charInfo, pAvatar->m_pPetFrame));

			pAvatar->SetCharactorInfo(curInfo);

			((CRealMyRoomTask*)GetTask())
				->AddAvatarBurden(
					CRealMyRoom_AvatarBurden(pAvatar->m_guid, curInfo, NULL));

			if (!m_bShowCharacter)
			{
				CFxSequence* pSeq =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq)
					pSeq->m_pos = WVector(pAvatar->m_pos.x,
						pAvatar->m_pos.y + 5.0f, pAvatar->m_pos.z);
			}
		}

		TurnOnItemStatus();

		m_bSelectItemList = false;
		if (m_pSelItemNext && m_pSelItemPrev)
		{
			m_pSelItemNext->Enable(false);
			m_pSelItemPrev->Enable(false);

			for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end();
				++it)
			{
				if ((*it).second.guid == charUid)
				{
					++it;
					if (it != Doc()->m_charMap.end())
						m_pSelItemNext->Enable(true);
					break;
				}
			}

			for (std::map<unsigned int, sCharacterInfo>::reverse_iterator rit =
					 Doc()->m_charMap.rbegin();
				rit != Doc()->m_charMap.rend(); ++rit)
			{
				if ((*rit).second.guid == charUid)
				{
					++rit;
					if (rit != Doc()->m_charMap.rend())
						m_pSelItemPrev->Enable(true);
					break;
				}
			}
		}

		if (m_charTypeId)
		{
			const char* name = ItemManager()->GetItemName(m_charTypeId);
			if (name && m_pSelectedName)
				m_pSelectedName->SetCaption11(name);
		}
	}
	else if (m_mainTab == 4)
	{
		Doc()->BuildMyPartTidList(info);
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_charTypeId, false);
	}
	else if (m_statTab == 1)
	{
		SetLevelBar();
	}

	AutoScroll();
}

bool CRealMyRoomMain::ChangeCaddie(unsigned long caddieUid)
{
	bool bResult = false;
	std::map<unsigned int, sCaddieInfo>::iterator it =
		Doc()->m_caddieMap.find(caddieUid);

	if (it == Doc()->m_caddieMap.end())
	{
		m_caddieTypeId = 0;
		m_caddieUid = 0;
		Doc()->m_myInfo.userEquip.guidCaddie = 0;

		if (m_subTab == 10)
		{
			HidePet(true);
			m_pItemList->UnselectAllItem();

			TurnOnItemStatus();
			if (m_pSelectedName)
				m_pSelectedName->SetCaption11("\xc4\xb3\xb5\xf0");
		}

		if (caddieUid == 0)
			Doc()->m_realEquip.guidCaddie = 0;
	}
	else
	{
		m_caddieUid = (*it).second.guid;
		m_caddieTypeId = (*it).second.tid;

		if ((*it).second.tidPart)
			m_caddieParts = (*it).second.tidPart;
		else
			m_caddieParts = (*it).second.tid;

		IFF_STRUCT::sCaddie* pCaddie =
			Doc()->m_itemManager.FindCaddie(m_caddieTypeId);
		if (pCaddie)
		{
			if (pCaddie->c.Level <= Doc()->m_myInfo.stat.Level)
			{
				Doc()->m_myInfo.userEquip.guidCaddie = (*it).second.guid;
				Doc()->m_realEquip.guidCaddie = (*it).second.guid;
				bResult = true;
			}
		}

		if (m_subTab == 10)
		{
			BuildDisplayStand((subTab_t)10);
			SafeLoadPet(&m_caddieParts, false);
			TurnOnItemStatus();

			m_bSelectItemList = false;
			m_pSelItemNext->Enable(false);
			m_pSelItemPrev->Enable(false);

			for (std::map<unsigned int, sCaddieInfo>::iterator iter =
					 Doc()->m_caddieMap.begin();
				iter != Doc()->m_caddieMap.end(); ++iter)
			{
				if ((*iter).second.guid == caddieUid)
				{
					++iter;
					if (iter != Doc()->m_caddieMap.end())
						m_pSelItemNext->Enable(true);
					break;
				}
			}

			for (std::map<unsigned int, sCaddieInfo>::reverse_iterator rit =
					 Doc()->m_caddieMap.rbegin();
				rit != Doc()->m_caddieMap.rend(); ++rit)
			{
				if ((*rit).second.guid == caddieUid)
				{
					++rit;
					if (rit != Doc()->m_caddieMap.rend())
						m_pSelItemPrev->Enable(true);
					break;
				}
			}

			if (bResult)
			{
				const char* name = ItemManager()->GetItemName(m_caddieTypeId);
				if (name && m_pSelectedName)
					m_pSelectedName->SetCaption11(name);
			}
		}
	}

	return bResult;
}

void CRealMyRoomMain::ChangeMascot(unsigned long uid)
{
	MASCOT_MAP_IT it = Doc()->m_mascotMap.find(uid);

	if (it == Doc()->m_mascotMap.end())
	{
		m_mascotTypeId = 0;
		m_mascotUid = 0;
		Doc()->m_myInfo.userEquip.guidMascot = 0;

		if (m_subTab == 12)
		{
			m_anotherHand.WaitUntilWorkDone(9999);
			if (m_pExhibition)
			{
				delete m_pExhibition;
				m_pExhibition = NULL;
			}
			m_pItemList->UnselectAllItem();
		}

		if (uid == 0)
		{
			Doc()->m_realEquip.guidMascot = 0;
		}
	}
	else
	{
		m_mascotTypeId = (*it).second.tid;
		m_mascotUid = (*it).second.guid;
		Doc()->m_myInfo.userEquip.guidMascot = (*it).second.guid;
		Doc()->m_realEquip.guidMascot = (*it).second.guid;

		if (m_subTab == 12)
		{
			BuildDisplayStand((subTab_t)12);
			SafeLoadPet(&m_mascotTypeId, false);
			TurnOffItemStatus();

			m_bSelectItemList = false;
			m_pSelItemPrev->Enable(false);
			m_pSelItemNext->Enable(false);

			for (MASCOT_MAP_IT it2 = Doc()->m_mascotMap.begin();
				it2 != Doc()->m_mascotMap.end(); ++it2)
			{
				if ((*it2).second.guid == uid)
				{
					if (++it2 != Doc()->m_mascotMap.end())
						m_pSelItemNext->Enable(true);
					break;
				}
			}

			for (std::map<unsigned int, sMascotInfo>::reverse_iterator rit =
					 Doc()->m_mascotMap.rbegin();
				rit != Doc()->m_mascotMap.rend(); ++rit)
			{
				if ((*rit).second.guid == uid)
				{
					if (++rit != Doc()->m_mascotMap.rend())
						m_pSelItemPrev->Enable(true);
					break;
				}
			}
		}
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
		Doc()->SendEquipItemSlot();
	else
		Doc()->SendEquipItemSlotByMascot();
}

int CRealMyRoomMain::ChangeClubset(unsigned long uid)
{
	CLUBSET_MAP_IT it = Doc()->m_clubSetMap.find(uid);
	if (it == Doc()->m_clubSetMap.end())
	{
		return 0;
	}

	m_clubSetTypeId = (*it).second.tid;
	m_clubSetUid = (*it).second.guid;
	m_pClubSetStat = (*it).second.Common;

	int result = 0;
	IFF_STRUCT::sClubSet* pClubSet =
		Doc()->m_itemManager.FindClubSet(m_clubSetTypeId);
	if (pClubSet)
	{
		int level = pClubSet->c.Level;
		int myLevel = Doc()->m_myInfo.stat.Level;

		if (pClubSet->c.IsUnderLvl)
		{
			if (level >= myLevel)
			{
				Doc()->m_myInfo.userEquip.guidClubSet = (*it).second.guid;
				result = 1;
			}
			else
			{
				for (CLUBSET_MAP_IT it2 = Doc()->m_clubSetMap.begin();
					it2 != Doc()->m_clubSetMap.end(); ++it2)
				{
					if ((*it2).second.tid == 0x10000000)
					{
						Doc()->m_myInfo.userEquip.guidClubSet =
							(*it2).second.guid;
						m_clubSetTypeId = (*it2).second.tid;
						m_clubSetUid = (*it2).second.guid;
						m_pClubSetStat = (*it2).second.Common;
						break;
					}
				}
				result = 2;
			}
		}
		else if (level <= myLevel)
		{
			Doc()->m_myInfo.userEquip.guidClubSet = (*it).second.guid;
			result = 1;
		}
	}

	if (m_subTab == 6)
	{
		SafeLoadPet(&m_clubSetTypeId, false);
		TurnOnItemStatus();

		m_bSelectItemList = false;
		m_pSelItemPrev->Enable(false);
		m_pSelItemNext->Enable(false);

		for (CLUBSET_MAP_IT it3 = Doc()->m_clubSetMap.begin();
			it3 != Doc()->m_clubSetMap.end(); ++it3)
		{
			if ((*it3).second.guid == uid)
			{
				if (++it3 != Doc()->m_clubSetMap.end())
					m_pSelItemNext->Enable(true);
				break;
			}
		}

		for (std::map<unsigned int, sItemInfo>::reverse_iterator rit =
				 Doc()->m_clubSetMap.rbegin();
			rit != Doc()->m_clubSetMap.rend(); ++rit)
		{
			if ((*rit).second.guid == uid)
			{
				if (++rit != Doc()->m_clubSetMap.rend())
					m_pSelItemPrev->Enable(true);
				break;
			}
		}
	}

	return result;
}

void CRealMyRoomMain::ChangeAztec(unsigned long typeId)
{
	m_aztecTypeId = typeId;
	Doc()->m_myInfo.userEquip.tidBall = typeId;

	if (m_subTab == 7)
	{
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();

		m_bSelectItemList = false;
		m_pSelItemPrev->Enable(false);
		m_pSelItemNext->Enable(false);

		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			if (it->tid == typeId)
			{
				if (++it != Doc()->m_ballList.end())
					m_pSelItemNext->Enable(true);
				break;
			}
		}

		for (std::list<sItemInfo>::reverse_iterator rit =
				 Doc()->m_ballList.rbegin();
			rit != Doc()->m_ballList.rend(); ++rit)
		{
			if ((*rit).tid == typeId)
			{
				if (++rit != Doc()->m_ballList.rend())
					m_pSelItemPrev->Enable(true);
				break;
			}
		}
	}
}

const Bitmap* CRealMyRoomMain::GetItemIcon(unsigned long typeId)
{
	const Bitmap* pIcon = NULL;

	std::map<unsigned long, const Bitmap*>::iterator it =
		m_itemIcon.find(typeId);
	if (it == m_itemIcon.end())
	{
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
		if (pCommon)
		{
			pIcon = g_pFresh->RegisterBitmap(pCommon->Icon);
			if (pIcon)
			{
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						pCommon->TypeId, pIcon));
			}
		}
	}
	else
	{
		return (*it).second;
	}

	return pIcon;
}

void CRealMyRoomMain::DrawSelItem(FrGraphicInterface* pGI, unsigned long typeId)
{
	WRect rect = m_pSelItemIcon->GetRect();

	const Bitmap* pIcon = GetItemIcon(typeId);
	if (pIcon)
		pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
}

void CRealMyRoomMain::DrawCharacterList(FrGraphicInterface* pGI,
	unsigned long param)
{
	if (m_bSelectItemList && !Doc()->m_charMap.empty())
	{
		int count = Doc()->m_charMap.size();
		int index = 0;
		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));
		int rows = count / cols + (count % cols ? 1 : 0);

		float height = (float)(rows - 1) + rows * 50.0f;
		float width = (float)(cols - 1) + cols * 46.0f;
		float x = 18.0f;
		float y = 419.0f - height;

		pGI->DrawTexture(m_pSelect[0],
			WRect(13.0f, y - 5.0f, width + 10.0f, height + 10.0f), 0xffffffff,
			0);

		WRect selRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect overRect(0.0f, 0.0f, 0.0f, 0.0f);

		for (CHAR_MAP_IT it = Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			const Bitmap* pIcon = GetItemIcon(it->second.tid);
			if (pIcon)
			{
				++index;
				WRect rect(x, y, 46.0f, 50.0f);
				pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
				if (index % cols)
				{
					x += 47.0f;
				}
				else
				{
					y += 51.0f;
					x = 18.0f;
				}

				if (m_charUid == it->second.guid)
					selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f,
						rect.h + 8.0f);

				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
				if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
					mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
				{
					overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
						rect.w + 8.0f, rect.h + 8.0f);

					if (g_input->GetButton(LEFT_BUTTON) == 1)
					{
						if (!m_anotherHand.IsWorking(0))
						{
							ChangeCharacter(it->second.guid);
						}
					}
				}
			}
		}

		if (selRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);
		}

		if (overRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
		}
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CRealMyRoomMain::DrawCaddieList(FrGraphicInterface* pGI,
	unsigned long param)
{
	if (m_bSelectItemList && !Doc()->m_caddieMap.empty())
	{
		int count = Doc()->m_caddieMap.size();
		int index = 0;
		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));
		int rows = count / cols + (count % cols ? 1 : 0);

		float height = (float)(rows - 1) + rows * 50.0f;
		float width = (float)(cols - 1) + cols * 46.0f;
		float x = 18.0f;
		float y = 419.0f - height;

		pGI->DrawTexture(m_pSelect[0],
			WRect(13.0f, y - 5.0f, width + 10.0f, height + 10.0f), 0xffffffff,
			0);

		WRect selRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect overRect(0.0f, 0.0f, 0.0f, 0.0f);

		for (CADDIE_MAP_IT it = Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
			const Bitmap* pIcon = GetItemIcon(it->second.tid);
			if (pIcon)
			{
				++index;
				WRect rect(x, y, 46.0f, 50.0f);
				pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
				if (index % cols)
				{
					x += 47.0f;
				}
				else
				{
					y += 51.0f;
					x = 18.0f;
				}

				if (m_caddieUid == it->second.guid)
					selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f,
						rect.h + 8.0f);

				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
				if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
					mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
				{
					overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
						rect.w + 8.0f, rect.h + 8.0f);

					if (g_input->GetButton(LEFT_BUTTON) == 1)
					{
						if (!m_anotherHand.IsWorking(0))
						{
							ChangeCaddie(it->second.guid);
						}
					}
				}
			}
		}

		if (selRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);
		}

		if (overRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
		}
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CRealMyRoomMain::DrawMascotList(FrGraphicInterface* pGI,
	unsigned long param)
{
	if (m_bSelectItemList && !Doc()->m_mascotMap.empty())
	{
		int count = Doc()->m_mascotMap.size();
		int index = 0;
		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));
		int rows = count / cols + (count % cols ? 1 : 0);

		float height = (float)(rows - 1) + rows * 50.0f;
		float width = (float)(cols - 1) + cols * 46.0f;
		float x = 18.0f;
		float y = 419.0f - height;

		pGI->DrawTexture(m_pSelect[0],
			WRect(13.0f, y - 5.0f, width + 10.0f, height + 10.0f), 0xffffffff,
			0);

		WRect selRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect overRect(0.0f, 0.0f, 0.0f, 0.0f);

		for (MASCOT_MAP_IT it = Doc()->m_mascotMap.begin();
			it != Doc()->m_mascotMap.end(); ++it)
		{
			const Bitmap* pIcon = GetItemIcon((*it).second.tid);
			if (pIcon)
			{
				++index;
				WRect rect(x, y, 46.0f, 50.0f);
				pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
				if (index % cols)
				{
					x += 47.0f;
				}
				else
				{
					y += 51.0f;
					x = 18.0f;
				}

				if (m_mascotUid == (*it).second.guid)
					selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f,
						rect.h + 8.0f);

				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
				if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
					mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
				{
					overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
						rect.w + 8.0f, rect.h + 8.0f);

					if (g_input->GetButton(LEFT_BUTTON) == 1)
					{
						if (!m_anotherHand.IsWorking(0))
						{
							ChangeMascot((*it).second.guid);
						}
					}
				}
			}
		}

		if (selRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);
		}

		if (overRect.w != 0.0f)
		{
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
		}
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CRealMyRoomMain::DrawAztecList(FrGraphicInterface* pGI,
	unsigned long param)
{
	if (m_bSelectItemList && !Doc()->m_ballList.empty())
	{
		int count = Doc()->m_ballList.size();
		int index = 0;
		int line = 1;
		int rows = count / 4 + (count % 4 ? 1 : 0);

		float x = 18.0f;
		float y = 407.0f - rows * 50.0f + (rows - 1) * 5.0f;
		float top = y - 12.0f;
		float height = rows * 50.0f + 15.0f;

		pGI->DrawTexture(m_pSelect[0], WRect(6.0f, top, 223.0f, 12.0f),
			0xffffffff, 0);
		pGI->DrawTexture(m_pSelect[1], WRect(6.0f, top + 12.0f, 223.0f, height),
			0xffffffff, 0);
		pGI->DrawTexture(m_pSelect[2],
			WRect(6.0f, height + top + 12.0f, 223.0f, 20.0f), 0xffffffff, 0);

		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			unsigned long typeId = it->tid;
			const Bitmap* pIcon = GetItemIcon(typeId);
			if (pIcon)
			{
				++index;
				WRect rect(x, y, 46.0f, 50.0f);
				pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
				if (index % 4)
				{
					x += 51.0f;
				}
				else
				{
					y += 55.0f;
					x = 18.0f;
					++line;
				}

				WRect selRect(rect.x - 6.0f, rect.y - 6.0f, rect.w + 12.0f,
					rect.h + 12.0f);

				if (m_aztecTypeId == typeId)
					pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff,
						0);

				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
				if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
					mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
				{
					pGI->DrawTexture(m_pSelectCursor[1], selRect, 0xffffffff,
						0);

					if (g_input->GetButton(LEFT_BUTTON) == 1)
					{
						if (!m_anotherHand.IsWorking(0))
						{
							ChangeAztec(typeId);
						}
					}
				}
			}
		}

		if (rows == line && 4 - index % 4 != 0)
		{
			for (int i = 0; i < 4 - index % 4; ++i)
			{
				pGI->DrawTexture(m_pSelect[3], WRect(x, y, 46.0f, 50.0f),
					0xffffffff, 0);
				x += 51.0f;
			}
		}
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CRealMyRoomMain::DrawClubList(FrGraphicInterface* pGI, unsigned long param)
{
	if (m_bSelectItemList && !Doc()->m_clubSetMap.empty())
	{
		int count = Doc()->m_clubSetMap.size();
		int index = 0;
		int line = 1;
		int rows = count / 4 + (count % 4 ? 1 : 0);

		float x = 18.0f;
		float y = 407.0f - rows * 50.0f + (rows - 1) * 5.0f;
		float top = y - 12.0f;
		float height = rows * 50.0f + 15.0f;

		pGI->DrawTexture(m_pSelect[0], WRect(6.0f, top, 223.0f, 12.0f),
			0xffffffff, 0);
		pGI->DrawTexture(m_pSelect[1], WRect(6.0f, top + 12.0f, 223.0f, height),
			0xffffffff, 0);
		pGI->DrawTexture(m_pSelect[2],
			WRect(6.0f, height + top + 12.0f, 223.0f, 20.0f), 0xffffffff, 0);

		for (CLUBSET_MAP_IT it = Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end(); ++it)
		{
			const Bitmap* pIcon = GetItemIcon((*it).second.tid);
			if (pIcon)
			{
				++index;
				WRect rect(x, y, 46.0f, 50.0f);
				pGI->DrawTexture(pIcon, rect, 0xffffffff, 0);
				if (index % 4)
				{
					x += 51.0f;
				}
				else
				{
					y += 55.0f;
					x = 18.0f;
					++line;
				}

				WRect selRect(rect.x - 6.0f, rect.y - 6.0f, rect.w + 12.0f,
					rect.h + 12.0f);

				if (m_clubSetUid == (*it).second.guid)
					pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff,
						0);

				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
				if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
					mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
				{
					pGI->DrawTexture(m_pSelectCursor[1], selRect, 0xffffffff,
						0);

					if (g_input->GetButton(LEFT_BUTTON) == 1)
					{
						if (!m_anotherHand.IsWorking(0))
						{
							ChangeClubset((*it).second.guid);
						}
					}
				}
			}
		}

		if (rows == line && 4 - index % 4 != 0)
		{
			for (int i = 0; i < 4 - index % 4; ++i)
			{
				pGI->DrawTexture(m_pSelect[3], WRect(x, y, 46.0f, 50.0f),
					0xffffffff, 0);
				x += 51.0f;
			}
		}
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CRealMyRoomMain::OnPetInit(int param)
{
	m_pPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnSkinPetInit(int param)
{
	m_pSkinPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnSkinPet2Init(int param)
{
	m_pSkinPet2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::DrawSkinFrame()
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

	if (m_skinTypeId[0])
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[0]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
			{
				WRect area = m_pPetArea->GetRect();
				float width = (float)pBmp->Width(),
					  height = (float)pBmp->Height();

				if (m_skinScroll[0] > width)
					m_skinScroll[0] -= width;
				else if (m_skinScroll[0] < 0.0f)
					m_skinScroll[0] += width;

				if (m_skinScroll[1] > height)
					m_skinScroll[1] -= height;
				else if (m_skinScroll[1] < 0.0f)
					m_skinScroll[1] += height;

				WRect src(m_skinScroll[0], m_skinScroll[1],
					width - m_skinScroll[0], height - m_skinScroll[1]);
				WRect dst;

				dst.x = area.x;
				dst.y = area.y;
				dst.w = src.w;
				dst.h = src.h;

				float bottom = area.y + area.h;
				while (dst.y < bottom)
				{
					float right = area.x + area.w;
					while (dst.x < right)
					{
						pGI->DrawTexture(pBmp, src, dst, 0xffffffff, 0);

						dst.x += dst.w;
						dst.w = Min(width, area.x + area.w - dst.x);

						src.x = 0.0f;
						src.w = dst.w;
					}

					dst.x = area.x;
					dst.y += dst.h;
					dst.w = width - m_skinScroll[0];
					dst.h = Min(height, area.y + area.h - dst.y);

					src.x = m_skinScroll[0];
					src.y = 0.0f;
					src.w = dst.w;
					src.h = dst.h;
				}
			}
		}
	}

	if (!m_anotherHand.IsWorking(0))
	{
		if (!m_bBongdariShop2 && !CRankingInfo::Instance()->GetDlg() &&
			!(CUserInfo::Instance() ? CUserInfo::Instance()->GetDlg() : NULL))
		{
			m_anotherHand.Enter();
			if (m_pExhibition)
				m_pExhibition->Display(0.0f, 0.0f);
			m_anotherHand.Leave();
		}
	}

	if (m_skinTypeId[1])
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[1]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
				pGI->DrawTexture(pBmp, m_pPetArea->GetRect(), 0xffffffff, 0);
		}
	}

	if (m_skinTypeId[2])
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[2]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
				pGI->DrawTexture(pBmp, m_pPetArea->GetRect(), 0xffffffff, 0);
		}
	}
}

void CRealMyRoomMain::DrawSkinSlot()
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI)
	{
		pGI->DrawTexture(m_pTeamBase,
			WRect(116.0f, 308.0f, (float)m_pTeamBase->Width(),
				(float)m_pTeamBase->Height()),
			0xffffffff, 0);

		if (m_skinTypeId[3])
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[3]);
			if (pSkin)
			{
				const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
				if (pBmp)
					pGI->DrawTexture(pBmp,
						WRect(116.0f, 308.0f, (float)pBmp->Width(),
							(float)pBmp->Height()),
						0xffffffff, 0);
			}
		}

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(m_charTypeId);
		if (pChar)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pChar->c.Icon);
			if (pBmp)
				pGI->DrawTexture(pBmp,
					WRect(124.0f, 344.0f, (float)pBmp->Width(),
						(float)pBmp->Height()),
					0xffffffff, 0);
		}

		if (m_skinTypeId[4])
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[4]);
			if (pSkin)
			{
				const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
				if (pBmp)
					pGI->DrawTexture(pBmp,
						WRect(124.0f, 344.0f, (float)pBmp->Width(),
							(float)pBmp->Height()),
						0xffffffff, 0);
			}
		}

		const Bitmap* pGender = m_pGender[Doc()->m_myInfo.info.gender];
		pGI->DrawTexture(pGender,
			WRect(121.0f, 406.0f, (float)pGender->Width(),
				(float)pGender->Height()),
			0xffffffff, 0);

		const Bitmap* pLevel;
		if (m_skinTypeId[5])
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[5]);
			if (pSkin)
				pLevel = g_pFresh->GetBitmap(pSkin->c.Icon);
			else
				pLevel = g_pFresh->GetBitmap(
					MakeStr("level_%03d", Doc()->m_myInfo.stat.Level + 1));
		}
		else
		{
			pLevel = g_pFresh->GetBitmap(
				MakeStr("level_%03d", Doc()->m_myInfo.stat.Level + 1));
		}

		if (pLevel)
		{
			pGI->DrawTexture(pLevel,
				WRect(206.0f - (int)(pLevel->Width() * 0.5f),
					386.0f - (int)(pLevel->Height() * 0.5f),
					(float)pLevel->Width(), (float)pLevel->Height()),
				0xffffffff, 0);
		}

		pGI->DrawTexture(m_pMasterSign,
			WRect((int)(179.5f - m_pMasterSign->Width() * 0.5f), 313.0f,
				(float)m_pMasterSign->Width(), (float)m_pMasterSign->Height()),
			0xffffffff, 0);

		pGI->SetTextColor(0xffffffff, 0xff808080);
		pGI->SetTextStyle(2);
		g_pFresh->GetManager()->PrintText(WPoint(139.0f, 415.0f), 0,
			Doc()->m_myInfo.info.sNick, -1.0f, 0xffffffff);
	}
}

void CRealMyRoomMain::OnPetOwnerDraw(int param)
{
	if (m_mainTab == 4)
	{
		switch (m_subTab)
		{
		case 13:
		case 14:
		case 15:
			DrawSkinFrame();
			break;
		case 16:
		case 34:
			DrawSkinSlot();
			break;
		case 33:
			DrawCutinFrame();
			break;
		}
	}
	else if (m_mainTab != 4)
	{
		if (m_subTab == 27)
		{
			FrListItem* pItem = m_pCardPackList->GetSelected();
			if (pItem)
			{
				sCards* pCard = (sCards*)pItem->pData;
				if (pCard && m_pExhibition)
				{
					if ((pCard->tid & 0x3c00000) == 0x1000000 &&
						pCard->cardCount < 10)
					{
						WBone* pRoot = m_pExhibition->GetModel()->GetRootBone();
						WBone* pBone = pRoot->FindBone("card_pack_Dummy", 0);
						if (pBone)
							pBone->SetInvisible(false, true);

						for (int i = 1; i < 10 - pCard->cardCount; ++i)
						{
							pBone = pRoot->FindBone(
								MakeStr("card_pack%02d_Dummy", i), 0);
							if (pBone)
								pBone->SetInvisible(false, true);
						}
					}
				}
			}
		}

		if (!m_anotherHand.IsWorking(0) && !m_bBongdariShop2 && !RANKINGDLG() &&
			(!CUserInfo::Instance() || !CUserInfo::Instance()->GetDlg()))
		{
			m_anotherHand.Enter();
			if (m_pExhibition)
				m_pExhibition->Display(0.0f, 0.0f);
			m_anotherHand.Leave();
		}
	}
}

void CRealMyRoomMain::OnFurnitureArrangeInit(int param)
{
	m_pFurnitureArrange = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureArrange)
	{
		m_pFurnitureArrange->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureArrangeButtonUP()
{
	if (!m_bFlag[3])
		ControlUiHideMode(4);
	else
		ControlUiHideMode(2);
}

void CRealMyRoomMain::OnItemDescBgInit2(int param)
{
	m_pItemDescBg2 = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pItemDescBg2)
	{
		m_pItemDescBg2->SetVisible(false);
	}
}

void CRealMyRoomMain::OnItemDescBgOwnerDraw2(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		WRect rect;
		float w[9];
		float h[9];
		for (int i = 0; i < 9; ++i)
		{
			w[i] = (float)m_pTipFrame[i]->Width();
			h[i] = (float)m_pTipFrame[i]->Height();
		}

		float nameHeight = m_pItemName->GetRect().h;
		float descHeight = (float)(m_pItemDesc2->GetLineNum() + 1) *
			m_pItemDesc2->GetLineHeight();
		float width = m_pItemDesc2->GetRect().w;
		float height = descHeight + nameHeight;
		float left = m_itemDesc2Pos.x - m_pItemDescBg2->GetRect().w;
		float top = 488.0f - height;

		m_pItemName->SetRect(
			WRect(left + 10.0f, top + 15.0f, width, nameHeight));
		m_pItemDesc2->SetRect(
			WRect(left + 10.0f, top + 45.0f, width, descHeight));

		rect.w = width + 10.0f;

		pGDI->DrawTexture(m_pTipFrame[0], WRect(left, top, w[0], h[0]),
			0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[1],
			WRect(left + w[0], top, rect.w - (w[2] + w[0]), h[1]), 0xffffffff,
			0);
		pGDI->DrawTexture(m_pTipFrame[2],
			WRect(left + (rect.w - w[2]), top, w[2], h[2]), 0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[3],
			WRect(left, top + h[1], w[3], height - (h[6] + h[0])), 0xffffffff,
			0);
		pGDI->DrawTexture(m_pTipFrame[4],
			WRect(left + w[3], top + h[1], rect.w - (w[5] + w[3]),
				height - (h[7] + h[1])),
			0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[5],
			WRect(left + (rect.w - w[5]), top + h[2], w[5],
				height - (h[8] + h[2])),
			0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[6],
			WRect(left, top + (height - h[6]), w[6], h[6]), 0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[7],
			WRect(left + w[6], top + (height - h[7]), rect.w - (w[8] + w[6]),
				h[7]),
			0xffffffff, 0);
		pGDI->DrawTexture(m_pTipFrame[8],
			WRect(left + (rect.w - w[8]), top + (height - h[8]), w[8], h[8]),
			0xffffffff, 0);
	}
}

void CRealMyRoomMain::OnItemDescBgInit(int param)
{
	m_pItemDescBg = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnItemDescBgOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		if (m_pItemDesc->GetLineNum() > 0)
		{
			float w[6];
			float h[6];
			for (int i = 0; i < 2; ++i)
			{
				w[i] = (float)m_pItemDescBmp[i]->Width();
				h[i] = (float)m_pItemDescBmp[i]->Height();
			}

			float y = m_itemDescOffset.y + 320.0f;
			pGDI->DrawTexture(m_pItemDescBmp[0],
				WRect(m_itemDescOffset.x + 6.0f, y, w[0], h[0]), 0xffffffff, 0);
			pGDI->DrawTexture(m_pItemDescBmp[1],
				WRect(m_itemDescOffset.x + 262.0f, y, w[1], h[1]), 0xffffffff,
				0);
		}
	}
}

void CRealMyRoomMain::OnItemDescInit2(int param)
{
	m_pItemDesc2 = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pItemDesc2)
	{
		m_pItemDesc2->SetVisible(false);
		m_furnitureDescPos = WPoint(m_pItemDesc2->GetRect().x,
			m_pItemDesc2->GetRect().y + m_pItemDesc2->GetRect().h);
		m_itemDesc2Pos = m_furnitureDescPos;
	}
}

void CRealMyRoomMain::OnItemNameInit(int param)
{
	m_pItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pItemName)
	{
		m_pItemName->SetVisible(false);
	}
}

void CRealMyRoomMain::OnItemDescInit(int param)
{
	m_pItemDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pItemDesc)
	{
		m_pItemDesc->SetVisible(false);
		m_itemDescPos = WPoint(m_pItemDesc->GetRect().x,
			m_pItemDesc->GetRect().y + m_pItemDesc->GetRect().h);
	}
}

void CRealMyRoomMain::SetItemDesc()
{
	m_pItemDesc->ClearLine();

	switch (m_subTab)
	{
	case 6:
	case 10:
		m_itemDescOffset = WPoint(0.0f, 30.0f);
		break;
	case 7:
	case 9:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 23:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		m_itemDescOffset = WPoint(0.0f, 120.0f);
		break;
	default:
		m_itemDescOffset = WPoint(0.0f, 0.0f);
		break;
	}
}

void CRealMyRoomMain::OnItemSlotBoxInit(int param)
{
	m_pItemSlotBox = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
	if (m_pItemSlotBox)
	{
		m_pItemSlotBox->SetVisible(false);

		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage)
			{
				pStorage->AddBackGroundUI(m_pItemSlotBox, false);
			}
		}
	}
}

void CRealMyRoomMain::OnEquipCancelInit(int param)
{
	m_pEquipCancel = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pEquipCancel->SetPushDelay(0.0f);
	m_pEquipCancel->EnableHover(true, 0.0f);

	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			pStorage->AddBackGroundUI(m_pEquipCancel, false);
		}
	}
}

void CRealMyRoomMain::OnEquipCancelUp()
{
	if (m_pItemSlot)
	{
		FrListItem* pSelected = m_pItemSlot->GetSelected();
		if (pSelected)
		{
			FrListBox::ITEM_LIST::iterator it;
			for (it = m_pItemSlot->m_itemList.begin();
				it != m_pItemSlot->m_itemList.end(); ++it)
			{
				if (*it == pSelected)
					break;
			}

			for (++it; it != m_pItemSlot->m_itemList.end(); ++it)
			{
				unsigned long* pTypeId = (unsigned long*)(*it)->pData;
				if (pTypeId && *pTypeId)
				{
					m_pItemSlot->SelectItem(*it, true);
					break;
				}
			}

			if (pSelected->pData)
				UnequipItem((unsigned long*)pSelected->pData);
		}
	}

	OnTooltipHoverOff(0);
}

void CRealMyRoomMain::OnEquipResetInit(int param)
{
	m_pEquipReset = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pEquipReset->SetPushDelay(0.0f);
	m_pEquipReset->EnableHover(true, 0.0f);

	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			pStorage->AddBackGroundUI(m_pEquipReset, false);
		}
	}
}

void CRealMyRoomMain::OnEquipResetUp()
{
	if (m_pItemSlot)
	{
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pItemSlot->m_itemList.begin();
			it != m_pItemSlot->m_itemList.end(); ++it)
		{
			FrListItem* pItem = *it;
			unsigned long* pTypeId = (unsigned long*)pItem->pData;
			if (pTypeId)
			{
				UnequipItem(pTypeId);
			}
		}
	}

	OnTooltipHoverOff(0);
}

void CRealMyRoomMain::OnItemSlotInit(int param)
{
	m_pItemSlot = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void CRealMyRoomMain::OnItemSlotOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem)
	{
		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

		if (pItem->pData)
		{
			unsigned long typeId = *(unsigned long*)pItem->pData;

			int state = 0;
			if (pItem->selected)
				state = 2;
			else if (pItem->underCursor)
				state = 1;

			if (typeId == 0)
				state = 0;

			const Bitmap* pBmp = m_pItemSlotBmp[state];
			if (pBmp)
			{
				pGDI->DrawTexture(pBmp,
					WRect(pItem->pos.x, pItem->pos.y, (float)pBmp->Width(),
						(float)pBmp->Height()),
					0xffffffff, 0);
			}

			IFF_STRUCT::sItem* pInfo = ItemManager()->FindItem(typeId);
			if (pInfo)
			{
				const Bitmap* pIcon = g_pFresh->GetBitmap(pInfo->c.Icon);
				if (pIcon)
				{
					pGDI->DrawTexture(pIcon,
						WRect(pItem->pos.x + 2.0f, pItem->pos.y + 4.0f,
							(float)pIcon->Width(), (float)pIcon->Height()),
						0xffffffff, 0);
				}
			}
		}
	}
}

void CRealMyRoomMain::OnItemSlotBtnDown()
{
	LogOut(0, "OnItemSlotBtnDown\n");
}

void CRealMyRoomMain::OnItemSlotDClick()
{
	if (IsLocalContent(S4_ITEM_STORAGE) && IsItemStorageVisible())
		return;

	FrListItem* pItem = m_pItemSlot->GetItemUnderCursor();
	if (pItem)
	{
		unsigned long* pTypeId = (unsigned long*)pItem->pData;
		if (pTypeId)
			UnequipItem(pTypeId);
	}
}

void CRealMyRoomMain::OnGiftBgInit(int param)
{
	m_pGiftBg = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pGiftBg)
	{
		m_pGiftBg->SetVisible(false);
		m_pGiftBg->SetMouseEvent(false);
	}
}

void CRealMyRoomMain::OnGiftDescInit(int param)
{
	m_pGiftDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pGiftDesc)
	{
		m_pGiftDesc->SetVisible(false);
	}
}

void CRealMyRoomMain::SetGiftDescString(const char* sender, const char* message,
	const char* date, const char* time)
{
	m_pGiftDesc->ClearLine();
	if (sender == NULL)
	{
		m_pGiftDesc->AddText(
			MakeStr(
				"\xb5\xb5\xc2\xf8\xc7\xd1 \xbc\xb1\xb9\xb0\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				sender),
			false, true);
		return;
	}

	LogOut(0, "SetGiftDescString\n");
	m_pGiftDesc->AddText(
		MakeStr(
			"\\c0xffffffff,t\\c\xb9\xde\xb4\xc2\xc0\xcc\\c0xffffffff,n\\c : %s\xb4\xd4\xb2\xb2\xbc\xad \xbc\xb1\xb9\xb0\xc0\xbb "
			"\xba\xb8\xb3\xbb\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
			sender),
		false, true);
	m_pGiftDesc->AddText(
		MakeStr("\\c0xffffffff,t\\c\xb3\xbb\xbf\xeb\\c0xffffffff,n\\c : %s",
			message),
		false, true);
	m_pGiftDesc->AddText(
		MakeStr(
			"\\c0xffffffff,t\\c\xba\xb8\xb3\xbd\xb3\xaf\xc2\xa5\\c0xffffffff,n\\c : %s",
			date),
		false, true);
	m_pGiftDesc->AddText(
		MakeStr(
			"\\c0xffffffff,t\\c\xba\xb8\xb3\xbd\xbd\xc3\xb0\xa3\\c0xffffffff,n\\c : %s",
			time),
		false, true);
}

void CRealMyRoomMain::OnStatFrameInit(int param)
{
	m_pStatFrame = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnStatTextInit(int param)
{
	m_pStatText = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnPowerBarInit(int param)
{
	m_pStatBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[0]->SetRange(0, 50, 0);
}

void CRealMyRoomMain::OnControlBarInit(int param)
{
	m_pStatBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[1]->SetRange(0, 30, 0);
}

void CRealMyRoomMain::OnImpactBarInit(int param)
{
	m_pStatBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[2]->SetRange(0, 30, 0);
}

void CRealMyRoomMain::OnSpinBarInit(int param)
{
	m_pStatBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[3]->SetRange(0, 30, 0);
}

void CRealMyRoomMain::OnCurveBarInit(int param)
{
	m_pStatBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[4]->SetRange(0, 30, 0);
}

void CRealMyRoomMain::TurnOnItemSelect()
{
	m_pSelItem->SetVisible(true);
	m_pSelItemPrev->SetVisible(true);
	m_pSelItemNext->SetVisible(true);
}

void CRealMyRoomMain::TurnOffItemSelect()
{
	m_pSelItem->SetVisible(false);
	m_pSelItemPrev->SetVisible(false);
	m_pSelItemNext->SetVisible(false);
}

void CRealMyRoomMain::TurnOnStat()
{
	m_pStatBox->SetVisible(true);
}

void CRealMyRoomMain::TurnOffStat()
{
	m_pStatBox->SetVisible(false);
}

void CRealMyRoomMain::OnSelFrameInit(int param)
{
	m_pSelFrame = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnStatBoxInit(int param)
{
	m_pStatBox = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
}

void CRealMyRoomMain::OnSelItemIconInit(int param)
{
	m_pSelItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnSelItemIconOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		switch (m_subTab)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 21:
		case 22:
		case 26:
			DrawSelItem(pGDI, m_charTypeId);
			DrawCharacterList(pGDI, m_charTypeId);
			break;
		case 6:
			DrawSelItem(pGDI, m_clubSetTypeId);
			DrawClubList(pGDI, m_clubSetTypeId);
			break;
		case 7:
			DrawSelItem(pGDI, m_aztecTypeId);
			DrawAztecList(pGDI, m_aztecTypeId);
			break;
		case 10:
			DrawSelItem(pGDI, m_caddieTypeId);
			DrawCaddieList(pGDI, m_caddieTypeId);
			break;
		case 12:
			DrawSelItem(pGDI, m_mascotTypeId);
			DrawMascotList(pGDI, m_mascotTypeId);
			break;
		}
	}
}

void CRealMyRoomMain::OnSelItemInit(int param)
{
	m_pSelItem = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSelItem)
		m_pSelItem->EnableBlink(true);
}

void CRealMyRoomMain::OnSelItemOwnerDraw(int param)
{
}

void CRealMyRoomMain::OnSelItemUp()
{
	m_bSelectItemList = !m_bSelectItemList;
}

void CRealMyRoomMain::OnSelItemPrevInit(int param)
{
	m_pSelItemPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSelItemPrev)
	{
		m_pSelItemPrev->SetPushDelay(0.1f);
	}
}

void CRealMyRoomMain::OnSelItemPrevUp()
{
	if (m_anotherHand.IsWorking(0))
		return;

	switch (m_subTab)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 22:
	case 26:
	{
		for (std::map<unsigned int, sCharacterInfo>::reverse_iterator it =
				 Doc()->m_charMap.rbegin();
			it != Doc()->m_charMap.rend(); ++it)
		{
			if ((*it).second.guid == m_charUid &&
				++it != Doc()->m_charMap.rend())
			{
				ChangeCharacter((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 10:
	{
		for (std::map<unsigned int, sCaddieInfo>::reverse_iterator it =
				 Doc()->m_caddieMap.rbegin();
			it != Doc()->m_caddieMap.rend(); ++it)
		{
			if ((*it).second.guid == m_caddieUid &&
				++it != Doc()->m_caddieMap.rend())
			{
				ChangeCaddie((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 12:
	{
		for (std::map<unsigned int, sMascotInfo>::reverse_iterator it =
				 Doc()->m_mascotMap.rbegin();
			it != Doc()->m_mascotMap.rend(); ++it)
		{
			if ((*it).second.guid == m_mascotUid &&
				++it != Doc()->m_mascotMap.rend())
			{
				ChangeMascot((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 6:
	{
		for (std::map<unsigned int, sItemInfo>::reverse_iterator it =
				 Doc()->m_clubSetMap.rbegin();
			it != Doc()->m_clubSetMap.rend(); ++it)
		{
			if ((*it).second.guid == m_clubSetUid &&
				++it != Doc()->m_clubSetMap.rend())
			{
				ChangeClubset((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 7:
	{
		for (std::list<sItemInfo>::reverse_iterator it =
				 Doc()->m_ballList.rbegin();
			it != Doc()->m_ballList.rend(); ++it)
		{
			if ((*it).tid == m_aztecTypeId && ++it != Doc()->m_ballList.rend())
			{
				ChangeAztec((*it).tid);
				break;
			}
		}
	}
	break;
	}
}

void CRealMyRoomMain::OnSelItemNextInit(int param)
{
	m_pSelItemNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSelItemNext)
	{
		m_pSelItemNext->SetPushDelay(0.1f);
	}
}

void CRealMyRoomMain::OnSelItemNextUp()
{
	if (m_anotherHand.IsWorking(0))
		return;

	switch (m_subTab)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 22:
	case 26:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			if ((*it).second.guid == m_charUid &&
				++it != Doc()->m_charMap.end())
			{
				ChangeCharacter((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 10:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it;
		for (it = Doc()->m_caddieMap.begin(); it != Doc()->m_caddieMap.end();
			++it)
		{
			if ((*it).second.guid == m_caddieUid &&
				++it != Doc()->m_caddieMap.end())
			{
				ChangeCaddie((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 12:
	{
		std::map<unsigned int, sMascotInfo>::iterator it;
		for (it = Doc()->m_mascotMap.begin(); it != Doc()->m_mascotMap.end();
			++it)
		{
			if ((*it).second.guid == m_mascotUid &&
				++it != Doc()->m_mascotMap.end())
			{
				ChangeMascot((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 6:
	{
		std::map<unsigned int, sItemInfo>::iterator it;
		for (it = Doc()->m_clubSetMap.begin(); it != Doc()->m_clubSetMap.end();
			++it)
		{
			if ((*it).second.guid == m_clubSetUid &&
				++it != Doc()->m_clubSetMap.end())
			{
				ChangeClubset((*it).second.guid);
				break;
			}
		}
	}
	break;
	case 7:
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			if ((*it).tid == m_aztecTypeId && ++it != Doc()->m_ballList.end())
			{
				ChangeAztec((*it).tid);
				break;
			}
		}
	}
	break;
	}
}

void CRealMyRoomMain::OnStatPartInit(int param)
{
	m_pStatPart = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void CRealMyRoomMain::OnStatPartUp()
{
	m_statTab = (statTab_t)0;
	SetLevelBar();
}

void CRealMyRoomMain::OnStatPartOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		int state = 0;
		WRect rect = m_pStatPart->GetRect();
		if (m_pStatPart->IsVisible())
		{
			if (m_statTab == 0)
			{
				state = 2;
			}
			else
			{
				const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
				if (rect.IsInRect(pt))
					state = 1;
			}

			if (m_statSubject < 3)
				pGDI->DrawTexture(m_pStatChar[m_statSubject * 3 + state], rect,
					0xffffffff, 0);
		}
	}
}

void CRealMyRoomMain::OnStatAllInit(int param)
{
	m_pStatAll = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void CRealMyRoomMain::OnStatAllUp()
{
	m_statTab = (statTab_t)1;
	SetLevelBar();
}

void CRealMyRoomMain::OnStatAllOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI)
	{
		int state = 0;
		WRect rect = m_pStatAll->GetRect();
		if (m_pStatAll->IsVisible())
		{
			if (m_statTab == 1)
			{
				state = 2;
			}
			else
			{
				const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
				if (rect.IsInRect(pt))
					state = 1;
			}

			pGDI->DrawTexture(m_pStatAllBtn[state], rect, 0xffffffff, 0);
		}
	}
}

void CRealMyRoomMain::OnTabMainInit(int param)
{
	m_pTabMain = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pTabMain)
	{
		const char* imgNames[3] = { NULL };

		for (int i = 0; i < 5; ++i)
		{
			imgNames[0] = MakeStr("stab_%d_n", i + 1);
			imgNames[1] = MakeStr("stab_%d_d", i + 1);
			imgNames[2] = MakeStr("stab_%d_d", i + 1);
			m_pTabMain->AddTab(imgNames, i, NULL);
		}

		imgNames[0] = MakeStr("stab_%d_n", 7);
		imgNames[1] = MakeStr("stab_%d_d", 7);
		imgNames[2] = MakeStr("stab_%d_d", 7);
		m_pTabMain->AddTab(imgNames, 6, NULL);

		imgNames[0] = MakeStr("stab_%d_n", 6);
		imgNames[1] = MakeStr("stab_%d_d", 6);
		imgNames[2] = MakeStr("stab_%d_d", 6);
		m_pTabMain->AddTab(imgNames, 5, NULL);
	}
}

void CRealMyRoomMain::OnTabMainButtonUp()
{
	TButtonItem* pSelected = m_pTabMain->GetSelected();
	if (pSelected == NULL)
		return;

	unsigned long index = pSelected->index;
	if (m_anotherHand.IsWorking(0))
	{
		m_pTabMain->Select(m_mainTab);
		return;
	}

	ChangeMainTab((mainTab_t)index, false);

	switch (index)
	{
	case 0:
		ChangeSubTab((subTab_t)21, false);
		ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
		break;
	case 1:
		ChangeSubTab((subTab_t)6, false);
		break;
	case 2:
		ChangeSubTab((subTab_t)10, false);
		ChangeCaddie(Doc()->m_myInfo.userEquip.guidCaddie);
		break;
	case 3:
		ChangeSubTab((subTab_t)12, false);
		ChangeMascot(Doc()->m_myInfo.userEquip.guidMascot);
		break;
	case 4:
		ChangeSubTab((subTab_t)13, false);
		TurnOffItemStatus();
		break;
	case 5:
		if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
			ChangeSubTab((subTab_t)18, false);
		else
			ChangeSubTab((subTab_t)17, false);
		TurnOffItemStatus();
		m_anotherHand.WaitUntilWorkDone(9999);
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		break;
	case 6:
		ChangeSubTab((subTab_t)32, false);
		TurnOffItemStatus();
		break;
	}

	if (index == 6)
		m_pFurnitureArrange->SetVisible(true);
	else
		m_pFurnitureArrange->SetVisible(false);

	Net_EquipDresserParts();
}

void CRealMyRoomMain::OnTabSubInit(int param)
{
	m_pTabSub = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
}

void CRealMyRoomMain::OnTabSubButtonUp()
{
	TButtonItem* pItem = m_pTabSub->GetSelected();
	if (pItem)
	{
		ChangeSubTab((subTab_t)pItem->index, false);
	}
}

void CRealMyRoomMain::OnGiftPageOwnerDraw(int param)
{
	if (m_subTab != 17)
		return;

	for (int i = 0; i < 5; i++)
	{
		if (!m_pGiftPage[i])
			return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	unsigned long count = 0;
	unsigned long start = m_giftPageGroup * 5;

	if (IsLocalContent((localContentType_t)0x5b))
	{
		m_giftMaxPage = Doc()->m_cardPackItem;
		count = Doc()->m_cardPackSlot;

		if (start < m_giftMaxPage)
		{
			if (m_giftMaxPage - start >= 5)
				count = 5;
			else
				count = m_giftMaxPage - start;
		}
		else if (start == m_giftMaxPage)
		{
			count = 1;
		}
		else if (start > m_giftMaxPage)
		{
			count = m_giftMaxPage % 5;
		}
	}
	else
	{
		unsigned long pageNum = Doc()->m_cardPackPageNum;
		unsigned long total = Doc()->m_cardPackCurPage;

		if (pageNum && total)
		{
			m_giftMaxPage = total / pageNum;
			if (m_giftMaxPage * pageNum < total)
				m_giftMaxPage++;
		}

		if (start < m_giftMaxPage)
		{
			if (m_giftMaxPage - start >= 5)
				count = 5;
			else
				count = m_giftMaxPage - start;
		}
		else if (start == m_giftMaxPage)
		{
			count = 1;
		}
		else if (start > m_giftMaxPage)
		{
			count = m_giftMaxPage % 5;
		}
	}

	if (count == 1)
	{
		if (m_giftPageGroup == 0)
			return;
	}

	m_giftPageNum = count;

	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);
	float x = 577.0f - (float)(m_pGiftPage[2]->Width() * ((count + 2) / 2));
	if (count % 2)
		x -= (float)(m_pGiftPage[2]->Width() / 2);

	x -= (float)m_pGiftPage[1]->Width();

	pGDI->DrawTexture(m_pGiftPage[1],
		WRect(x, 517.0f, (float)m_pGiftPage[1]->Width(),
			(float)m_pGiftPage[1]->Height()),
		0xffffffff, 0);
	x += (float)m_pGiftPage[1]->Width();

	rect.x = x;
	rect.y = 517.0f;
	rect.w = (float)m_pGiftPage[2]->Width();
	rect.h = (float)m_pGiftPage[2]->Height();
	pGDI->DrawTexture(m_pGiftPage[2], rect, 0xffffffff, 0);

	rect.x = rect.x + (float)(m_pGiftPage[2]->Width() / 2) -
		(float)(m_pGiftPage[0]->Width() / 2);
	rect.y = rect.y + (float)(m_pGiftPage[2]->Height() / 2) -
		(float)(m_pGiftPage[0]->Height() / 2);
	rect.w = (float)m_pGiftPage[0]->Width();
	rect.h = (float)m_pGiftPage[0]->Height();
	rect.y -= 0.5f;
	m_giftPageRect[0] = rect;

	if (m_giftPageGroup > 0)
		pGDI->DrawTexture(m_pGiftPage[0], rect, 0xffffffff, 0);
	else
		pGDI->DrawTexture(m_pGiftPage[0], rect, 0xff999999, 0);

	x += (float)m_pGiftPage[2]->Width();

	unsigned long index = 1;
	for (unsigned long i = 0; i < count; i++)
	{
		rect.x = x;
		rect.y = 517.0f;
		rect.w = (float)m_pGiftPage[2]->Width();
		rect.h = (float)m_pGiftPage[2]->Height();
		pGDI->DrawTexture(m_pGiftPage[2], rect, 0xffffffff, 0);

		rect.x = (rect.x + (float)(m_pGiftPage[2]->Width() / 2));
		rect.y += 3.0f;
		m_giftPageRect[index] = rect;
		m_giftPageRect[index].x -= 5.0f;
		m_giftPageRect[index].y -= 5.0f;
		index++;

		pGDI->SetTextColor(0xffffffff, 0xffffffff);

		if (m_giftPageNum < m_giftPage)
		{
			m_giftPage = m_giftPageNum;
			if (IsLocalContent((localContentType_t)0x5b))
				Doc()->m_cardPackSlot = (unsigned char)m_giftPage;
			else
				Doc()->m_cardPackMode = (unsigned char)m_giftPage;
		}

		if (i + 1 == m_giftPage)
			pGDI->SetTextStyle(1);
		else
			pGDI->SetTextColor(0xff99ccff, 0xffffffff);

		pGDI->Print(WPoint(rect.x, rect.y), 1, "%d",
			m_giftPageGroup * 5 + i + 1);
		pGDI->SetTextStyle(0);

		x = (x + (float)m_pGiftPage[2]->Width());
	}

	rect.x = x;
	rect.y = 517.0f;
	rect.w = (float)m_pGiftPage[2]->Width();
	rect.h = (float)m_pGiftPage[2]->Height();
	pGDI->DrawTexture(m_pGiftPage[2],
		WRect(x, 517.0f, (float)m_pGiftPage[2]->Width(),
			(float)m_pGiftPage[2]->Height()),
		0xffffffff, 0);

	rect.x = rect.x + (float)(m_pGiftPage[2]->Width() / 2) -
		(float)(m_pGiftPage[4]->Width() / 2);
	rect.y = rect.y + (float)(m_pGiftPage[2]->Height() / 2) -
		(float)(m_pGiftPage[4]->Height() / 2);
	rect.w = (float)m_pGiftPage[4]->Width();
	rect.h = (float)m_pGiftPage[4]->Height();
	rect.y -= 0.5f;
	m_giftPageRect[index] = rect;

	if (m_giftPageGroup * 5 < m_giftMaxPage)
	{
		if (m_giftMaxPage - m_giftPageGroup * 5 > 5)
			pGDI->DrawTexture(m_pGiftPage[4], rect, 0xffffffff, 0);
		else
			pGDI->DrawTexture(m_pGiftPage[4], rect, 0xff999999, 0);
	}
	else
	{
		pGDI->DrawTexture(m_pGiftPage[4], rect, 0xff999999, 0);
	}

	pGDI->DrawTexture(m_pGiftPage[3],
		WRect(x + (float)m_pGiftPage[2]->Width(), 517.0f,
			(float)m_pGiftPage[3]->Width(), (float)m_pGiftPage[3]->Height()),
		0xffffffff, 0);

	if (IsLocalContent((localContentType_t)0x5b))
	{
		if (Doc()->m_bCardPackRebuild == true)
		{
			Doc()->m_bCardPackRebuild = false;
			BuildDisplayStand((subTab_t)17);
		}
	}
	else
	{
		if (Doc()->m_bCardPackRebuild == true)
		{
			Doc()->m_bCardPackRebuild = false;
			BuildDisplayStand((subTab_t)17);
		}
	}

	WPoint mouse(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
	bool bOver = false;
	int over = 0;

	if (m_giftPageRect[0].IsInRect(mouse.x, mouse.y))
	{
		over = 1;
		bOver = true;
	}

	for (unsigned long j = 0; j < m_giftPageNum; j++)
	{
		if (m_giftPageRect[j + 1].IsInRect(mouse.x, mouse.y))
			bOver = true;

		over++;
	}

	if (m_giftPageRect[over + 1].IsInRect(mouse.x, mouse.y))
		bOver = true;

	if (bOver)
		CMouseCursor::Instance()->SetCursor(1);
}

void CRealMyRoomMain::OnGiftPageLBDown()
{
	if (m_subTab == 17)
	{
		if (!IsLocalContent((localContentType_t)0x5b))
		{
			if (Doc()->m_cardPackCurPage <= Doc()->m_cardPackPageNum)
				return;
		}

		static DWORD s_lastTick = GetTickCount();

		if (GetTickCount() - s_lastTick > 350)
		{
			s_lastTick = GetTickCount();

			const WPoint* pMouse = &g_pFresh->GetManager()->GetMousePos();
			unsigned long page = 0;
			unsigned short index = 0;

			if (m_giftPageRect[0].IsInRect(*pMouse))
			{
				if (m_giftPageGroup > 0)
				{
					m_giftPageGroup--;
					m_giftPage = 1;
					page = m_giftPageGroup * 5 + 1;
				}

				index = 1;
			}

			for (unsigned long i = 0; i < m_giftPageNum; i++)
			{
				unsigned long num = i + 1;
				if (m_giftPageRect[num].IsInRect(*pMouse))
				{
					if (m_giftPage != num)
					{
						m_giftPage = num;
						page = m_giftPageGroup * 5 + m_giftPage;
					}
				}

				index++;
			}

			if (m_giftPageRect[index + 1].IsInRect(*pMouse))
			{
				if (m_giftPageGroup * 5 < m_giftMaxPage)
				{
					if (m_giftMaxPage - m_giftPageGroup * 5 > 5)
					{
						m_giftPageGroup++;
						m_giftPage = 1;
						page = m_giftPageGroup * 5 + 1;
					}
				}
			}

			if (page)
			{
				WSendPacket packet((enumClientPacket)0x92);
				packet.Encode4(page);
				packet.Send(TO_GAME);
			}
		}

		if (!IsLocalContent((localContentType_t)0x5b))
		{
			Doc()->m_cardPackMode = m_giftPage;
		}
	}
}

void CRealMyRoomMain::OnCardBookInit(int param)
{
	FrArea* pArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (pArea && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pCardBookArea, pArea);
	}
}

void CRealMyRoomMain::OnCardBookOwnerDraw(int param)
{
	if (m_pCardBook)
	{
		m_pCardBook->Draw();
	}
}

void CRealMyRoomMain::SetCardBookVisible(bool bVisible)
{
	if (m_pCardBook)
	{
		m_pCardBook->Show(bVisible);
		m_bCardBookVisible = bVisible;
	}

	if (m_pCardPackList)
	{
		m_pCardPackList->SetVisible(bVisible);
		m_pCardPackList->UnselectAllItem();
		HidePet(false);
	}
}

void CRealMyRoomMain::BuildCardPackList()
{
	if (m_pCardPackList)
	{
		m_pCardPackList->ClearItem();

		std::list<sCards>::iterator it = Doc()->m_cardList.begin();
		while (it != Doc()->m_cardList.end())
		{
			sCards* pCards = &(*it);
			int type = (pCards->tid >> 22) & 0xf;
			if (type == 4 || type == 3)
			{
				m_pCardPackList->AddItem(pCards);
			}
			++it;
		}

		if (Doc()->m_cardList.size() % 2)
			m_pCardPackList->AddItem(NULL);

		for (int i = 6 - m_pCardPackList->GetCurrentItemSize(true); i > 0; --i)
		{
			m_pCardPackList->AddItem(NULL);
		}

		m_pCardPackList->UseRightButton(true);
		m_pCardPackList->UseDummy(true);
		HidePet(false);
	}
}

void CRealMyRoomMain::OnCardPackListInit(int param)
{
	m_pCardPackList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildCardPackList();

	if (m_pCardPackList)
		m_pCardPackList->SetVisible(false);
}

void CRealMyRoomMain::OnCardPackListDraw(int param)
{
	if (m_pCardPackList && m_subTab == 27)
	{
		FrListItem* pItem = (FrListItem*)param;

		if (pItem)
		{
			FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
			pGDI->SetTextStyle(0);

			const Bitmap* pBmp;
			if (!pItem->pData)
			{
				pBmp = m_pItemBase[3];
			}
			else if (pItem->selected)
			{
				pBmp = m_pItemBase[2];
				pGDI->SetTextColor(0xffff0000, 0xffffffff);
			}
			else if (pItem->underCursor)
			{
				pBmp = m_pItemBase[1];
				pGDI->SetTextColor(0xff000000, 0xffffffff);
			}
			else
			{
				pBmp = m_pItemBase[0];
				pGDI->SetTextColor(0xff000000, 0xffffffff);
			}

			if (pBmp)
			{
				pGDI->DrawTexture(pBmp,
					WRect(0.0f, 0.0f, (float)pBmp->Width(),
						(float)pBmp->Height()),
					WRect(pItem->pos.x, pItem->pos.y,
						(float)m_pCardPackList->GetItemWidth(),
						(float)m_pCardPackList->GetItemHeight()),
					0xffffffff, 0);
			}

			sCards* pCards = (sCards*)pItem->pData;
			if (pCards)
			{
				const char* name =
					"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb";
				const char* icon = "caddie_no";

				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pCards->tid);
				if (pCard)
				{
					name = pCard->c.Name;
					icon = pCard->c.Icon;
				}

				const Bitmap* pIcon = g_pFresh->GetBitmap(icon);
				float width = (float)m_pCardPackList->GetItemWidth();
				if (pIcon)
				{
					pGDI->DrawTexture(pIcon,
						WRect(pItem->pos.x + 12.0f, pItem->pos.y + 12.0f,
							(float)pIcon->Width(), (float)pIcon->Height()),
						0xffffffff, 0);
				}

				pGDI->Print11(WPoint(width - 12.0f, 12.0f) + pItem->pos, 2,
					"%s", name);

				float fontHeight = (float)pGDI->GetFontHeight11();
				if ((pCards->tid & 0x3c00000) == 0x1000000)
				{
					pGDI->Print11(WPoint(width - 12.0f, fontHeight) +
							WPoint(0.0f, 17.0f) + pItem->pos,
						2,
						MakeStr(
							"\xb3\xb2\xc0\xba \xc4\xab\xb5\xe5 \xc6\xd1 %d\xb0\xb3",
							pCards->cardCount));
				}
				else
				{
					pGDI->Print11(WPoint(width - 12.0f, fontHeight) +
							WPoint(0.0f, 17.0f) + pItem->pos,
						2, MakeStr("%d\xb0\xb3", pCards->cardCount));
				}

				if (m_pCardBook)
				{
					if (pItem == m_pCardPackList->GetSelected())
						m_pCardBook->DrawCardPackHelp(pCards->tid);
				}
			}
		}
	}
}

void CRealMyRoomMain::OnCardPackListDown()
{
	if (m_pCardPackList)
	{
		if (m_subTab == 27)
		{
			FrListItem* pItem = m_pCardPackList->GetItemUnderCursor();
			if (pItem && pItem->pData)
			{
				if (m_anotherHand.IsWorking(0))
				{
					m_pCardPackList->ToggleItem(pItem);
				}
				else if (pItem->selected)
				{
					g_audio->PlaySfx("ui_desktop_icon_click", NULL, 0, NULL,
						NULL, 0.5f, 200.0f);
					sCards* pCards = (sCards*)pItem->pData;
					if (m_pCardBook)
						m_pCardBook->m_pUseBtn->SetVisible(true);
					ShowPet(pCards->tid);
				}
				else
				{
					m_pCardPackList->SelectItem(pItem, true);
				}
			}
		}
	}
}

void CRealMyRoomMain::OnCardPackListRbUp()
{
	if (m_pCardPackList)
	{
		if (m_subTab == 27)
		{
			FrListItem* pItem = m_pCardPackList->GetItemUnderCursor();
			if (pItem && pItem->pData)
			{
				if (m_anotherHand.IsWorking(0))
				{
					m_pCardPackList->ToggleItem(pItem);
				}
				else
				{
					if (pItem->selected)
						OnCardPackListDown();
					else
						m_pCardPackList->ToggleItem(pItem);

					OpenInformation(&((sCards*)pItem->pData)->tid, NULL, NULL);
				}
			}
		}
	}
}

void CRealMyRoomMain::OnCardBookIndexInit(int param)
{
	FrListBox* pList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (pList && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pIndexList, pList);
	}
}

void CRealMyRoomMain::OnCardBookIndexLBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->PageIndexBehavior(g_pFresh->GetManager()->GetMousePos(),
			1);
	}
}

void CRealMyRoomMain::OnCardBookIndexMouseOver()
{
	if (m_pCardBook)
	{
		m_pCardBook->PageIndexBehavior(g_pFresh->GetManager()->GetMousePos(),
			2);
	}
}

void CRealMyRoomMain::OnCardUseInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pUseBtn, pButton);
	}
}

void CRealMyRoomMain::OnCardUseLBDown()
{
	if (m_pCardPackList->IsVisible())
	{
		FrListItem* pItem = m_pCardPackList->GetSelected();

		if (pItem)
		{
			sCards* pCards = (sCards*)pItem->pData;

			if (pCards)
			{
				if (!FindFrForm<FrCardBoxDlg>("FrCardBoxDlg"))
				{
					FrCardBoxDlg* pDlg = CreateForm<FrCardBoxDlg>(
						g_pFresh->GetManager(), this, "FrCardBoxDlg", NULL);
					pDlg->Init(pCards->tid, pCards->uid, pCards->cardCount);
					pDlg->Open(
						(FRESH_PFN_RESULT)&CRealMyRoomMain::OnCardBoxDlgResult,
						1);

					HidePet(false);
				}
			}
		}
	}
	else
	{
		if (m_pCardBook)
			m_pCardBook->UseCard();
	}
}

bool CRealMyRoomMain::OnCardBoxDlgResult(int result, FrForm* pForm)
{
	return true;
}

void CRealMyRoomMain::OnCardPropertyInit(int param)
{
	FrArea* pArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (pArea && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pPropertyArea, pArea);
	}
}

void CRealMyRoomMain::OnTabCardPackInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pTab[0], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

void CRealMyRoomMain::OnTabCardPackLBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->ButtonTabControl(0);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackList)
	{
		m_pCardPackList->SetVisible(true);
		m_pCardPackList->UnselectAllItem();
	}
}

void CRealMyRoomMain::OnTabCardVolume1Init(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pTab[1], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

void CRealMyRoomMain::OnTabCardVolume1LBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->ButtonTabControl(1);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackList)
		m_pCardPackList->SetVisible(false);

	HidePet(false);
}

void CRealMyRoomMain::OnTabCardVolume2Init(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pTab[2], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

void CRealMyRoomMain::OnTabCardVolume2LBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->ButtonTabControl(2);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackList)
		m_pCardPackList->SetVisible(false);

	HidePet(false);
}

void CRealMyRoomMain::OnTabCardVolume3Init(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pTab[3], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

void CRealMyRoomMain::OnTabCardVolume3LBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->ButtonTabControl(3);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackList)
		m_pCardPackList->SetVisible(false);

	HidePet(false);
}

void CRealMyRoomMain::OnTabCardVolume4Init(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (pButton && m_pCardBook)
	{
		m_pCardBook->BoundControl(m_pCardBook->m_pTab[4], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

void CRealMyRoomMain::OnTabCardVolume4LBDown()
{
	if (m_pCardBook)
	{
		m_pCardBook->ButtonTabControl(4);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackList)
		m_pCardPackList->SetVisible(false);

	HidePet(false);
}

void CRealMyRoomMain::BoundCardPackPage(FrButton* pButton, unsigned char index)
{
	pButton->SetVisible(false);
	pButton->Enable(false);
}

#define IMPLEMENT_CARDPACK_INIT(n, i) \
	void CRealMyRoomMain::OnCardPackInit##n(int param) \
	{ \
		BoundCardPackPage(DYNAMIC_CAST(FrButton, (FrWnd*)param), i); \
	}

IMPLEMENT_CARDPACK_INIT(1, 0)
IMPLEMENT_CARDPACK_INIT(2, 1)
IMPLEMENT_CARDPACK_INIT(3, 2)
IMPLEMENT_CARDPACK_INIT(4, 3)
IMPLEMENT_CARDPACK_INIT(5, 4)
IMPLEMENT_CARDPACK_INIT(6, 5)
IMPLEMENT_CARDPACK_INIT(7, 6)
IMPLEMENT_CARDPACK_INIT(8, 7)

void CRealMyRoomMain::CardPackBehavior(unsigned char slot, unsigned char action)
{
	if (m_pCardBook)
	{
		if (IsItemStorageVisible())
			return;

		CCardPackPage* pPage = m_pCardBook->GetCardPackPage();

		if (pPage)
		{
			m_pCardBook->SetSelectedSlot(slot);
			pPage->SelectSlot(action, slot);

			if (m_pCardBook->m_pUseBtn)
			{
				if (pPage->IsSelectedSlot())
				{
					m_pCardBook->m_pUseBtn->SetVisible(true);
				}
				else
				{
					m_pCardBook->m_pUseBtn->SetVisible(false);
				}
			}
		}
	}
}

#define IMPLEMENT_CARDPACK_LBDOWN(n, i) \
	void CRealMyRoomMain::OnCardPackLBDown##n() \
	{ \
		CardPackBehavior(i, 1); \
	}

IMPLEMENT_CARDPACK_LBDOWN(1, 0)
IMPLEMENT_CARDPACK_LBDOWN(2, 1)
IMPLEMENT_CARDPACK_LBDOWN(3, 2)
IMPLEMENT_CARDPACK_LBDOWN(4, 3)
IMPLEMENT_CARDPACK_LBDOWN(5, 4)
IMPLEMENT_CARDPACK_LBDOWN(6, 5)
IMPLEMENT_CARDPACK_LBDOWN(7, 6)
IMPLEMENT_CARDPACK_LBDOWN(8, 7)

#define IMPLEMENT_CARDPACK_RBDOWN(n, i) \
	void CRealMyRoomMain::OnCardPackRBDown##n() \
	{ \
		CardPackBehavior(i, 2); \
	}

IMPLEMENT_CARDPACK_RBDOWN(1, 0)
IMPLEMENT_CARDPACK_RBDOWN(2, 1)
IMPLEMENT_CARDPACK_RBDOWN(3, 2)
IMPLEMENT_CARDPACK_RBDOWN(4, 3)
IMPLEMENT_CARDPACK_RBDOWN(5, 4)
IMPLEMENT_CARDPACK_RBDOWN(6, 5)
IMPLEMENT_CARDPACK_RBDOWN(7, 6)
IMPLEMENT_CARDPACK_RBDOWN(8, 7)

void CRealMyRoomMain::BoundCardPage(FrButton* pButton, unsigned char index)
{
	if (pButton && m_pCardBook)
	{
		CCardPage* pPage = m_pCardBook->GetCardPage();

		if (pPage)
		{
			pPage->BoundSlotControl(index, pButton);
		}

		pButton->UseRightButton(true);

		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage)
			{
				pStorage->AddBackGroundUI(pButton, false);
			}
		}
	}
}

#define IMPLEMENT_CARD_INIT(n, i) \
	void CRealMyRoomMain::OnCardInit##n(int param) \
	{ \
		BoundCardPage(DYNAMIC_CAST(FrButton, (FrWnd*)param), i); \
	}

IMPLEMENT_CARD_INIT(1, 0)
IMPLEMENT_CARD_INIT(2, 1)
IMPLEMENT_CARD_INIT(3, 2)
IMPLEMENT_CARD_INIT(4, 3)
IMPLEMENT_CARD_INIT(5, 4)
IMPLEMENT_CARD_INIT(6, 5)
IMPLEMENT_CARD_INIT(7, 6)
IMPLEMENT_CARD_INIT(8, 7)
IMPLEMENT_CARD_INIT(9, 8)
IMPLEMENT_CARD_INIT(10, 9)
IMPLEMENT_CARD_INIT(11, 10)
IMPLEMENT_CARD_INIT(12, 11)

void CRealMyRoomMain::CardBehavior(unsigned char slot, unsigned char action)
{
	if (m_pCardBook)
	{
		if (IsItemStorageVisible())
			return;

		CCardPage* pPage = m_pCardBook->GetCardPage();

		if (pPage)
		{
			m_pCardBook->SetSelectedSlot(slot);
			pPage->SelectSlot(action, slot);

			if (m_pCardBook->m_pUseBtn)
			{
				if (pPage->IsSelectedSlot())
				{
					if (pPage->GetCardSlot()[slot].GetCards()->tid ==
						0x7c800026)
					{
						m_pCardBook->m_pUseBtn->SetVisible(false);
					}
					else
					{
						m_pCardBook->m_pUseBtn->SetVisible(true);
					}
				}
				else
				{
					m_pCardBook->m_pUseBtn->SetVisible(false);
				}
			}
		}
	}
}
#define IMPLEMENT_CARD_LBDOWN(n, i) \
	void CRealMyRoomMain::OnCardLBDown##n() \
	{ \
		CardBehavior(i, 1); \
	}

IMPLEMENT_CARD_LBDOWN(1, 0)
IMPLEMENT_CARD_LBDOWN(2, 1)
IMPLEMENT_CARD_LBDOWN(3, 2)
IMPLEMENT_CARD_LBDOWN(4, 3)
IMPLEMENT_CARD_LBDOWN(5, 4)
IMPLEMENT_CARD_LBDOWN(6, 5)
IMPLEMENT_CARD_LBDOWN(7, 6)
IMPLEMENT_CARD_LBDOWN(8, 7)
IMPLEMENT_CARD_LBDOWN(9, 8)
IMPLEMENT_CARD_LBDOWN(10, 9)
IMPLEMENT_CARD_LBDOWN(11, 10)
IMPLEMENT_CARD_LBDOWN(12, 11)

#define IMPLEMENT_CARD_RBDOWN(n, i) \
	void CRealMyRoomMain::OnCardRBDown##n() \
	{ \
		CardBehavior(i, 2); \
	}

IMPLEMENT_CARD_RBDOWN(1, 0)
IMPLEMENT_CARD_RBDOWN(2, 1)
IMPLEMENT_CARD_RBDOWN(3, 2)
IMPLEMENT_CARD_RBDOWN(4, 3)
IMPLEMENT_CARD_RBDOWN(5, 4)
IMPLEMENT_CARD_RBDOWN(6, 5)
IMPLEMENT_CARD_RBDOWN(7, 6)
IMPLEMENT_CARD_RBDOWN(8, 7)
IMPLEMENT_CARD_RBDOWN(9, 8)
IMPLEMENT_CARD_RBDOWN(10, 9)
IMPLEMENT_CARD_RBDOWN(11, 10)
IMPLEMENT_CARD_RBDOWN(12, 11)

void CRealMyRoomMain::OnItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pItemList)
	{
		m_pItemList->UseRightButton(true);
		m_pItemList->UseDummy(true);
		m_pItemList->SetPushSound(NULL);
		m_pItemList->AddNoRButtonRect(m_miniBtn[0].rect, 0);
		m_pItemList->AddNoRButtonRect(m_miniBtn[4].rect, 4);
		m_pItemList->AddNoRButtonRect(m_miniBtn[1].rect, 1);
		m_pItemList->AddNoRButtonRect(m_miniBtn[2].rect, 2);
		m_pItemList->AddNoRButtonRect(m_miniBtn[3].rect, 3);
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			m_pItemList->AddNoRButtonRect(m_miniBtn[7].rect, 7);
			m_pItemList->AddNoRButtonRect(m_miniBtn[8].rect, 8);
		}

		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			m_pItemList->AddNoRButtonRect(m_miniBtn[9].rect, 9);
		}

		if (IsLocalContent(S4_REAL_MYROOM))
		{
			m_pItemList->AddNoRButtonRect(m_miniBtn[10].rect, 10);
		}

		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage)
			{
				pStorage->SetItemListArea(m_pItemList->GetRect());
			}
		}
	}
}

void CRealMyRoomMain::DrawStatus(FrGraphicInterface* graphic, FrListItem* item,
	short* const x, short* const y)
{
	float dy = 4.0f;
	for (int i = 0; i < 5; ++i)
	{
		const Bitmap* pBmp[2];
		for (int k = 0; k < 2; ++k)
			pBmp[k] = m_pStatDot[i][k];
		int count = x[i];
		int slot = y[i];
		if (slot || count)
		{
			float dx = 86.0f;
			for (; count > 0; --count)
			{
				graphic->DrawTexture(pBmp[0],
					WRect(item->pos.x + dx, item->pos.y + dy, 5.0f, 6.0f),
					0xffffffff, 0);
				dx -= 6.0f;
			}

			for (; slot > 0; --slot)
			{
				graphic->DrawTexture(pBmp[1],
					WRect(item->pos.x + dx, item->pos.y + dy, 5.0f, 6.0f),
					0xffffffff, 0);
				dx -= 6.0f;
			}

			dy += 6.0f;
		}
	}
}

void CRealMyRoomMain::DrawStatus2(FrGraphicInterface* graphic, FrListItem* item,
	short* const x, short* const y, float alpha)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		if (x[i] || y[i])
		{
			graphic->SetTextColor(0xff0000ff, 0xffffffff);
			graphic->Print11(item->pos +
					WPoint(182.0f + alpha, 36.0f + alpha + dy),
				2, MakeStatusString(i, x[i], y[i]));
			dy += 14.0f;
		}
	}
}

void CRealMyRoomMain::DrawStatusCard(FrGraphicInterface* graphic,
	FrListItem* item, short* const x, float alpha)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		if (x[i])
		{
			graphic->SetTextColor(0xff0000ff, 0xffffffff);
			graphic->Print11(item->pos +
					WPoint(182.0f + alpha, 36.0f + alpha + dy),
				2, MakeCardStatusString(i, x[i]));
			dy += 14.0f;
		}
	}
}

void CRealMyRoomMain::OnItemListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item == NULL)
		return;

	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();

	const Bitmap* pBase;
	if (item->pData == NULL)
	{
		pBase = m_pItemBase[3];
	}
	else if (item->selected)
	{
		pBase = m_pItemBase[2];
		g->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (item->underCursor)
	{
		pBase = m_pItemBase[1];
		g->SetTextColor(0xff000000, 0xffffffff);
	}
	else
	{
		pBase = m_pItemBase[0];
		g->SetTextColor(0xff000000, 0xffffffff);
	}

	g->SetTextStyle(0);

	WRect baseRect = WRect(0.0f, 0.0f, 168.0f, 80.0f) + item->pos;
	WRect iconRect = WRect(0.0f, 0.0f, 52.0f, 52.0f) + item->pos;

	if (pBase)
		g->DrawTexture(pBase, WRect(item->pos, pBase->Width(), pBase->Height()),
			0xffffffff, 0);

	sItemInfo* pItem = (sItemInfo*)item->pData;
	if (pItem == NULL)
		return;

	unsigned long typeId = 0;
	const char* name =
		"\xbe\xcb\xbc\xf6\xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb";
	const char* icon = "caddie_no";
	int count = -1;
	bool bPang = false;
	bool bRest = false;
	bool bReply = false;
	unsigned char itemType = 0;
	int day = 0;
	int hour = 0;
	bool bPart = false;
	int partDay = 0;
	int partHour = 0;
	bool bPCBang = false;
	const Bitmap* pUccIcon = NULL;
	unsigned char timeFlag = 0;
	unsigned long course = 0;
	SYSTEMTIME date;

	switch (m_subTab)
	{
	default:
	{
		typeId = pItem->tid;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
		if (pCommon)
		{
			name = pCommon->Name;
			icon = pCommon->Icon;
			bPang = !pCommon->IsCash;
			count = pItem->Common[0];
			itemType = pItem->ItemType;
			day = pItem->HourRemain / 24;
			hour = pItem->HourRemain % 24;
		}

		if (IsLocalContent(S4_UCC) && IsUccClothes(pItem->tid))
		{
			name = GetUccItemName(pItem);
			pUccIcon = GetUccItemIcon(pItem);

			if (m_subTab == 21 && !(pItem->status & 1))
			{
				const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

				sItemInfo* pInfo = (sItemInfo*)item->pData;
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);

				if (pPart && (pPart->Category == 7 || pPart->Category == 9))
				{
					int index = (pPart->Category == 7) ? 0 : 1;

					if (item->underCursor &&
						((&m_uccCopyRect)[index] + item->pos).IsInRect(mouse))
					{
						g->DrawTexture(m_miniBtn[12].pOver,
							(&m_uccCopyRect)[index] + item->pos +
								WPoint(0.0f, 0.0f),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip(
							UCC_TOOLTIP_COPY);
					}
					else
					{
						g->DrawTexture(m_miniBtn[12].pNormal,
							(&m_uccCopyRect)[index] + item->pos +
								WPoint(0.0f, 0.0f),
							0xffffffff, 0);
					}
				}

				if (pPart && (pPart->Category == 7 || pPart->Category == 8))
				{
					int btn;
					eBarButton tip;
					if (pInfo->status & 2)
					{
						btn = 13;
						tip = UCC_TOOLTIP_REDRAW;
					}
					else
					{
						btn = 11;
						tip = UCC_TOOLTIP_DRAW;
					}

					if (item->underCursor &&
						(m_uccDrawRect + item->pos).IsInRect(mouse))
					{
						g->DrawTexture(m_miniBtn[btn].pOver,
							m_uccDrawRect + item->pos + WPoint(0.0f, 0.0f),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip(tip);
					}
					else
					{
						g->DrawTexture(m_miniBtn[btn].pNormal,
							m_uccDrawRect + item->pos + WPoint(0.0f, 0.0f),
							0xffffffff, 0);
					}
				}
			}
		}
	}
	break;

	case 8:
	{
		typeId = pItem->tid;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
		if (pCommon)
		{
			name = pCommon->Name;
			icon = pCommon->Icon;
			bPang = !pCommon->IsCash;
			count = pItem->Common[0];
			itemType = pItem->ItemType;

			for (int i = 0; i < 10; ++i)
			{
				if (typeId == Doc()->m_myInfo.userEquip.tidItemSlot[i])
					--count;
			}
		}
	}
	break;

	case 10:
	{
		sCaddieInfo* pInfo = (sCaddieInfo*)pItem;
		typeId = pInfo->tid;
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(typeId);
		if (pCaddie)
		{
			name = pCaddie->c.Name;
			icon = pCaddie->c.Icon;
			count = 1;
			day = pInfo->Remain_Date;

			if (pInfo->Rent_flag && pCaddie->MonthlyFee > 0)
				itemType = 1;

			if (pInfo->tidPart & 0x1fff)
			{
				bPart = true;
				partDay = pInfo->Remain_Partdate / 24;
				partHour = pInfo->Remain_Partdate % 24;
			}
		}

		if (pInfo->Rent_flag && pInfo->Remain_Date <= 0 &&
			pInfo->tidPart == 0 && pCaddie->MonthlyFee)
			bRest = true;
	}
	break;

	case 12:
	{
		sMascotInfo* pInfo = (sMascotInfo*)pItem;
		typeId = pInfo->tid;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
		if (pCommon)
		{
			name = pCommon->Name;
			icon = pCommon->Icon;
			count = 1;
			day = pInfo->Remain_Date / 24;
			hour = pInfo->Remain_Date % 24;
			timeFlag = pCommon->TimeFlag;
		}

		bPCBang = pInfo->PCBangMascot;
	}
	break;

	case 17:
		if (!IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
		{
			sGiftInfo* pGift = (sGiftInfo*)item->pData;
			typeId = pGift->tid;
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
			if (pCommon)
			{
				name = pCommon->Name;
				icon = pCommon->Icon;
				count = pGift->Arg0;
				itemType = pGift->ItemType;
			}

			if (pGift->sFromID[0] != '@')
				bReply = true;
		}
		break;

	case 18:
	{
		sSpecialTrophy* pTrophy = (sSpecialTrophy*)pItem;
		typeId = pTrophy->tid & 0xfffffff0;
		int grade = pTrophy->tid & 0xf;
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(typeId);
		if (pMatch)
		{
			name = pMatch->Name;
			icon = pMatch->Icon[grade];
			count = pTrophy->count;
		}
	}
	break;

	case 20:
	{
		sQuest* pQuest = (sQuest*)pItem;
		typeId = pQuest->typeId;
		IFF_STRUCT::sQuest* pIffQuest = ItemManager()->FindQuest(typeId);
		if (pIffQuest)
		{
			name = pIffQuest->Name;
			icon = pIffQuest->Icon;
			count = 1;
		}
	}
	break;

	case 23:
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			sRecordedItemInfo* pRecord = (sRecordedItemInfo*)item->pData;
			typeId = pRecord->itemInfo.tid;
			course = pRecord->course;
			date = pRecord->date;
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
			if (pCommon)
			{
				name = pCommon->Name;
				icon = pCommon->Icon;
				count = 1;
			}
		}
		break;

	case 27:
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			sCards* pCards = (sCards*)item->pData;
			if (pCards->tid)
			{
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pCards->tid);
				if (pCard)
				{
					typeId = pCards->tid;
					name = pCard->c.Name;
					icon = pCard->c.Icon;
					count = pCards->cardCount;
				}
			}
		}
		break;

	case 26:
	{
		typeId = pItem->tid;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
		if (pCommon)
		{
			icon = pCommon->Icon;
			bPang = !pCommon->IsCash;
			count = 1;
			itemType = 0;
			day = 0;
			hour = 0;
		}

		name = GetUccItemName(pItem);
		pUccIcon = GetUccItemIcon(pItem);

		if (!(pItem->status & 1))
		{
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->tid);

			if (pPart && (pPart->Category == 7 || pPart->Category == 9))
			{
				int index = (pPart->Category == 7) ? 0 : 1;

				if (item->underCursor &&
					((&m_uccCopyRect)[index] + item->pos).IsInRect(mouse))
				{
					g->DrawTexture(m_miniBtn[12].pOver,
						(&m_uccCopyRect)[index] + item->pos +
							WPoint(0.0f, 0.0f),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip(UCC_TOOLTIP_COPY);
				}
				else
				{
					g->DrawTexture(m_miniBtn[12].pNormal,
						(&m_uccCopyRect)[index] + item->pos +
							WPoint(0.0f, 0.0f),
						0xffffffff, 0);
				}
			}

			if (pPart && (pPart->Category == 7 || pPart->Category == 8))
			{
				int btn;
				eBarButton tip;
				if (pItem->status & 2)
				{
					btn = 13;
					tip = UCC_TOOLTIP_REDRAW;
				}
				else
				{
					btn = 11;
					tip = UCC_TOOLTIP_DRAW;
				}

				if (item->underCursor &&
					(m_uccDrawRect + item->pos).IsInRect(mouse))
				{
					g->DrawTexture(m_miniBtn[btn].pOver,
						m_uccDrawRect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip(tip);
				}
				else
				{
					g->DrawTexture(m_miniBtn[btn].pNormal,
						m_uccDrawRect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
				}
			}
		}
	}
	break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	{
		sFurniture_List* pFurniture = (sFurniture_List*)pItem;
		typeId = pFurniture->typeId;
		if ((typeId & 0xfc000000) == 0x48000000)
		{
			IFF_STRUCT::sFurniture* pIffFurniture =
				ItemManager()->FindFurniture(typeId);
			if (pIffFurniture)
			{
				name = pIffFurniture->c.Name;
				icon = pIffFurniture->c.Icon;
				count = 1;
			}
		}
	}
	break;
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(icon);
	if (pUccIcon)
		pIcon = pUccIcon;

	CItemStorage* pStorage = CItemStorage::Instance();
	if (pStorage)
	{
		if (pStorage->GetDragingItemIndex(
				sStoredItemDragDrop::MODE_ITEM_LIST) == item->idx)
			pIcon = NULL;
	}

	WPoint iconPos;
	if (!S5::ISCutinItem(typeId))
		iconPos = WPoint(item->pos.x, item->pos.y);
	else
		iconPos = WPoint(item->pos.x, item->pos.y - 12.0f);

	ReviseItemIconPoint(typeId, iconPos);

	if (pIcon)
		g->DrawTexture(pIcon, WRect(iconPos, pIcon->Width(), pIcon->Height()),
			0xffffffff, 0);

	if (itemType >= 2 && typeId != 0x1a000042 && itemType != 6)
		g->DrawTexture(m_miniBtn[5].pNormal, m_miniBtn[5].rect + iconPos,
			0xffffffff, 0);

	if (bRest)
	{
		const Bitmap* pRest = g_pFresh->GetBitmap("caddie_rest");
		g->DrawTexture(pRest, WRect(iconPos, pRest->Width(), pRest->Height()),
			0xffffffff, 0);
	}

	g->SetTextStyle(0);

	if (m_subTab == 18)
	{
		char text[100] = { 0 };
		strcpy(text, name);

		char* pLine = text;
		int line = 0;
		char* pNext = strstr(pLine, "\n");
		while (pNext)
		{
			*pNext = 0;
			g->Print11(item->pos + WPoint(182.0f, 7.0f + line * 15.0f), 2,
				pLine);
			++line;
			pLine = pNext + 1;
			pNext = strstr(pLine, "\n");
		}
		g->Print11(item->pos + WPoint(182.0f, 7.0f + line * 15.0f), 2, pLine);

		sprintf(text, "%d", count);
		g->Print11(item->pos + WPoint(180.0f, 67.0f), 2, text);
	}
	else
	{
		WPoint pos = item->pos + WPoint(182.0f, 7.0f);
		g->Print11(WPoint(pos), 2, "%s", name);
	}

	if (IsLocalContent(S4_UCC) && IsUccClothes(typeId))
	{
		sItemInfo* pInfo = (sItemInfo*)item->pData;
		WPoint p(item->pos.x + 51.0f, item->pos.y + 50.0f);
		if ((pInfo->status & 1) && pInfo->Seq == 1 && m_pUccState[0])
		{
			g->DrawTexture(m_pUccState[0],
				WRect(p.x, p.y, (float)m_pUccState[0]->Width(),
					(float)m_pUccState[0]->Height()),
				0xffffffff, 0);
		}
		else if ((pInfo->status & 1) && pInfo->Seq > 1 && m_pUccState[1])
		{
			g->DrawTexture(m_pUccState[1],
				WRect(p.x, p.y, (float)m_pUccState[1]->Width(),
					(float)m_pUccState[1]->Height()),
				0xffffffff, 0);
		}
		else if ((pInfo->status & 2) && m_pUccState[2])
		{
			g->DrawTexture(m_pUccState[2],
				WRect(p.x, p.y, (float)m_pUccState[2]->Width(),
					(float)m_pUccState[2]->Height()),
				0xffffffff, 0);
		}

		if (pInfo->status & 1)
		{
			g->SetTextColor(0xff9e2d3e, 0xffffffff);
			g->Print11(item->pos + WPoint(182.0f, 21.0f), 2, pInfo->CopierNick);
			g->SetTextColor(0xff000000, 0xffffffff);
		}
	}

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	int tab = m_subTab;

	bool bBuff = false;
	GetActor("ItemBuff") << MsgObject(NULL, 0x246, 0, (int)&bBuff, (int)item,
		(int)&mouse, typeId);
	if (bBuff)
		tab = 35;

	WPoint pos1 = item->pos + WPoint(182.0f, 23.0f);
	WPoint pos2(pos1.x, pos1.y + 16.0f);

	switch (tab)
	{
	case 17:
		if (!IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
		{
			if (item->underCursor &&
				(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[0].pOver,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_MOVEGIFT);
			}
			else
				g->DrawTexture(m_miniBtn[0].pNormal,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);

			if (bReply)
			{
				if (item->underCursor &&
					(m_miniBtn[4].rect + item->pos).IsInRect(mouse))
				{
					g->DrawTexture(m_miniBtn[4].pOver,
						m_miniBtn[4].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_REPLY);
				}
				else
					g->DrawTexture(m_miniBtn[4].pNormal,
						m_miniBtn[4].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
			}
		}
		break;

	case 8:
		if (item->underCursor &&
			(m_miniBtn[1].rect + item->pos).IsInRect(mouse))
		{
			g->DrawTexture(m_miniBtn[1].pOver,
				m_miniBtn[1].rect + item->pos + WPoint(0.0f, 0.0f), 0xffffffff,
				0);
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_EQUIP);
		}
		else
			g->DrawTexture(m_miniBtn[1].pNormal,
				m_miniBtn[1].rect + item->pos + WPoint(0.0f, 0.0f), 0xffffffff,
				0);

		if (bPang)
		{
			if (item->underCursor &&
				(m_miniBtn[2].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[2].pOver,
					m_miniBtn[2].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_DELETE);
			}
			else
				g->DrawTexture(m_miniBtn[2].pNormal,
					m_miniBtn[2].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
		}
		break;

	case 10:
		if (bRest)
		{
			if (item->underCursor &&
				(m_miniBtn[3].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[3].pOver,
					m_miniBtn[3].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_REEMPLOY);
			}
			else
				g->DrawTexture(m_miniBtn[3].pNormal,
					m_miniBtn[3].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
		}
		break;

	case 9:
		switch (typeId)
		{
		case 0x1a00003b:
			if (item->underCursor &&
				(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[0].pOver,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_HANDSEL);
			}
			else
				g->DrawTexture(m_miniBtn[0].pNormal,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
			break;

		case 0x1a000003:
		case 0x1a00003e:
		case 0x1a000048:
		case 0x1a000052:
		case 0x1a000054:
		case 0x1a0000bc:
		case 0x1a0000d6:
		case 0x1a000105:
		case 0x1a00012f:
		case 0x1a000130:
		case 0x1a000131:
		case 0x1a00014b:
			if (item->underCursor &&
				(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[0].pOver,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_OPENGIFT);
			}
			else
				g->DrawTexture(m_miniBtn[0].pNormal,
					m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
			break;

		default:
		{
			IFF_STRUCT::sItem* pIffItem = ItemManager()->FindItem(typeId);
			if (pIffItem)
			{
				if (pIffItem->IsRandomBox())
				{
					if (item->underCursor &&
						(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
					{
						g->DrawTexture(m_miniBtn[0].pOver,
							m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip(
							TOOLTIP_OPENGIFT);
					}
					else
						g->DrawTexture(m_miniBtn[0].pNormal,
							m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
							0xffffffff, 0);
				}
			}

			if (FrCardRemove::IsCardRemoveItem(typeId))
			{
				if (item->underCursor &&
					(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
					g->DrawTexture(m_miniBtn[0].pOver,
						m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
				else
					g->DrawTexture(m_miniBtn[0].pNormal,
						m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
			}
		}
		break;
		}
		break;

	case 24:
		if (typeId == 0x1a000042)
		{
			if (IsLocalContent(S3_CADDIE_REPORT))
			{
				if (item->underCursor &&
					(m_miniBtn[0].rect + item->pos).IsInRect(mouse))
				{
					g->DrawTexture(m_miniBtn[0].pOver,
						m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_REPORT);
				}
				else
					g->DrawTexture(m_miniBtn[0].pNormal,
						m_miniBtn[0].rect + item->pos + WPoint(0.0f, 0.0f),
						0xffffffff, 0);
			}
		}
		break;

	case 23:
		if (IsLocalContent(S4_REPLAY_SYSTEM))
		{
			if (item->underCursor &&
				(m_miniBtn[7].rect + item->pos).IsInRect(mouse))
				g->DrawTexture(m_miniBtn[7].pOver,
					m_miniBtn[7].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
			else
				g->DrawTexture(m_miniBtn[7].pNormal,
					m_miniBtn[7].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);

			if (item->underCursor &&
				(m_miniBtn[8].rect + item->pos).IsInRect(mouse))
				g->DrawTexture(m_miniBtn[8].pOver,
					m_miniBtn[8].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
			else
				g->DrawTexture(m_miniBtn[8].pNormal,
					m_miniBtn[8].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
		}
		break;

	case 27:
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			if (item->underCursor &&
				(m_miniBtn[9].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[9].pOver,
					m_miniBtn[9].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_CARD);
			}
			else
				g->DrawTexture(m_miniBtn[9].pNormal,
					m_miniBtn[9].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
		}
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (item->underCursor &&
				(m_miniBtn[10].rect + item->pos).IsInRect(mouse))
			{
				g->DrawTexture(m_miniBtn[10].pOver,
					m_miniBtn[10].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip(RMR_RM_DECO);
			}
			else
				g->DrawTexture(m_miniBtn[10].pNormal,
					m_miniBtn[10].rect + item->pos + WPoint(0.0f, 0.0f),
					0xffffffff, 0);
		}
		break;
	}

	if (count == -1)
	{
		g->Print11(pos1, 2, "%d", typeId);
		return;
	}

	unsigned long group = typeId >> 26;
	switch (group)
	{
	case 2:
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);
		if (pPart)
		{
			DrawStatus2(g, item, pPart->Attr, pPart->Slot, 0.0f);
			if (m_subTab != 17)
				DrawPartsSlot(g, item);
		}

		sItemInfo* pInfo = (sItemInfo*)item->pData;
		if (pInfo && pInfo->ItemType == 6)
		{
			if (pInfo->Expired)
			{
				const Bitmap* pExpired =
					g_pFresh->GetBitmap("rental_item_expired");
				if (pExpired)
					g->DrawTexture(pExpired,
						WRect(item->pos, pExpired->Width(), pExpired->Height()),
						0xffffffff, 0);
				return;
			}

			const Bitmap* pClock = g_pFresh->GetBitmap("myroom_rental_clock");
			if (pClock)
				g->DrawTexture(pClock,
					WRect(item->pos.x + 70.0f, item->pos.y + 43.0f,
						pClock->Width(), pClock->Height()),
					0xffffffff, 0);
		}
		else if (pInfo->ItemType < 2)
			return;

		g->SetTextColor(0xff4d4d4d, 0xffffffff);
		if (day > 0)
		{
			g->Print11(pos1, 2,
				"\xb3\xb2\xc0\xba\xb1\xe2\xb0\xa3 : %d\xc0\xcf %d\xbd\xc3\xb0\xa3",
				day, hour);
		}
		else
		{
			g->SetTextColor(0xffff0000, 0xffffffff);
			g->Print11(pos1, 2,
				"\xb3\xb2\xc0\xba\xb1\xe2\xb0\xa3 : %d\xbd\xc3\xb0\xa3", hour);
		}
	}
	break;

	case 28:
		if ((typeId & 0x1f0000) == 0)
			g->Print11(pos1, 2, "%d\xb0\xb3", count);
		break;

	case 5:
		if (!ItemManager()->IsBasicAztecFamily(typeId))
			g->Print11(pos1, 2, "%d\xb0\xb3", count);
		break;

	case 6:
		if (itemType >= 2)
		{
			if (typeId == 0x1a000042 && IsLocalContent(S3_CADDIE_REPORT))
			{
				sItemInfo* pInfo = (sItemInfo*)item->pData;

				g->SetTextColor(0xff4d4d4d, 0xffffffff);
				g->Print11(pos1, 2, "%d\xb3\xe2 %d\xbf\xf9 %d\xc0\xcf",
					pInfo->ItemDate.wYear, pInfo->ItemDate.wMonth,
					pInfo->ItemDate.wDay);
				g->SetTextColor(0xff4d4d4d, 0xffffffff);
				g->Print11(pos2, 2, " %d\xbd\xc3 %d \xba\xd0",
					pInfo->ItemDate.wHour, pInfo->ItemDate.wMinute);
			}
			else if (day == 0 && hour == 0)
			{
				g->SetTextColor(0xff4d4d4d, 0xffffffff);
				g->Print11(pos1, 2, "\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3:");
				g->SetTextColor(0xffff0000, 0xffffffff);
				g->Print11(pos2, 2, "1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8");
			}
			else
			{
				g->SetTextColor(0xff4d4d4d, 0xffffffff);
				g->Print11(pos1, 2, "\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3:");
				g->Print11(pos2, 2, "%d\xc0\xcf %d\xbd\xc3\xb0\xa3", day, hour);
			}
		}
		else if (IsLocalContent(S4_REPLAY_SYSTEM) && m_subTab == 23)
		{
			IFF_STRUCT::sCourse* pCourse =
				ItemManager()->FindCourse(course | 0x28000000);
			if (pCourse)
			{
				WPoint pos = item->pos + WPoint(182.0f, 22.0f);
				g->Print11(WPoint(pos), 2, pCourse->c.Name);
			}

			char szDate[50] = { 0 };
			char szTime[50] = { 0 };
			wsprintf(szDate, "%d\xb3\xe2%02d\xbf\xf9%02d\xc0\xcf", date.wYear,
				date.wMonth, date.wDay);
			WPoint datePos = item->pos + WPoint(182.0f, 33.0f);
			g->Print11(WPoint(datePos), 2, szDate);
			wsprintf(szTime, "%02d\xbd\xc3%02d\xba\xd0", date.wHour,
				date.wMinute);
			WPoint timePos = item->pos + WPoint(182.0f, 44.0f);
			g->Print11(WPoint(timePos), 2, szTime);
		}
		else if (typeId == 0x1a000006)
		{
		}
		else if (typeId != 0x1a000010)
		{
			g->Print11(pos1, 2, "%d\xb0\xb3", count);
		}
		else
		{
			g->Print11(pos1, 2, "%d\xc6\xce", count);
		}
		break;

	case 16:
		if (m_subTab == 17)
			return;
		if (bPCBang)
			return;

		if (!timeFlag)
		{
			g->SetTextColor(0xff4d4d4d, 0xffffffff);
			g->Print11(pos1, 2, "\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3:");
			g->Print11(pos2, 2, "\xb9\xab\xc1\xa6\xc7\xd1", day, hour);
		}
		else if (day == 0 && hour == 0)
		{
			g->SetTextColor(0xff4d4d4d, 0xffffffff);
			g->Print11(pos1, 2, "\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3:");
			g->SetTextColor(0xffff0000, 0xffffffff);
			g->Print11(pos2, 2, "1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8");
		}
		else
		{
			g->SetTextColor(0xff4d4d4d, 0xffffffff);
			g->Print11(pos1, 2, "\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3:");
			g->Print11(pos2, 2, "%d\xc0\xcf %d\xbd\xc3\xb0\xa3", day, hour);
		}
		break;

	case 7:
		if (itemType == 1)
		{
			if (day == 0 && hour == 0)
			{
				g->SetTextColor(0xffff0000, 0xffffffff);
				g->Print11(pos1, 2, "\xc8\xde\xb0\xa1\xc1\xdf");
			}
			else
			{
				g->SetTextColor(0xff4d4d4d, 0xffffffff);
				g->Print11(pos1, 2,
					"\xb3\xb2\xc0\xba \xb0\xed\xbf\xeb\xb1\xe2\xb0\xa3:");
				g->Print11(pos2, 2, "%d\xc0\xcf", day);

				if (bPart)
				{
					g->SetTextColor(0xff4d4d4d, 0xffffffff);
					g->Print11(WPoint(pos2.x, pos2.y + 16.0f), 2,
						"\xb2\xd9\xb9\xcc\xb1\xe2 \xb3\xb2\xc0\xba \xb1\xe2\xb0\xa3:");
					g->Print11(WPoint(pos2.x, pos2.y + 32.0f), 2,
						"%d\xc0\xcf %d\xbd\xc3\xb0\xa3", partDay, partHour);
				}
			}
		}
		else if (bPart)
		{
			g->SetTextColor(0xff4d4d4d, 0xffffffff);
			g->Print11(pos1, 2,
				"\xb2\xd9\xb9\xcc\xb1\xe2 \xb3\xb2\xc0\xba \xb1\xe2\xb0\xa3:");
			g->Print11(pos2, 2, "%d\xc0\xcf %d\xbd\xc3\xb0\xa3", partDay,
				partHour);
		}
		break;

	case 4:
	{
		int cost = 0;
		if (m_subTab != 17)
		{
			sItemInfo* pInfo = (sItemInfo*)item->pData;
			for (char i = 0; i < 5; ++i)
			{
				for (int j = 0; j < pInfo->Common[i]; ++j)
				{
					IFF_STRUCT::sEnchant* pEnchant =
						ItemManager()->FindEnchant(j | ((i | 0x340) << 20));
					if (pEnchant)
						cost += pEnchant->ReqdPang;
				}
			}
		}

		g->SetTextColor(0xff009000, 0xffffffff);
		g->Print11(pos1, 2,
			"\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xc7\xd1 \xba\xf1\xbf\xeb:");
		g->Print11(pos2, 2, "%d\xc6\xce", cost);
	}
	break;

	case 31:
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			g->Print11(pos1, 2, "%d\xb0\xb3", count);

			sCards* pCards = (sCards*)item->pData;
			IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pCards->tid);
			DrawStatusCard(g, item, pCard->COM, 0.0f);
		}
		break;

	case 18:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (group == 18)
			{
				sFurniture_List* pFurniture = (sFurniture_List*)item->pData;
				if (GetArrangement(typeId, pFurniture->id))
				{
					const Bitmap* pCheck =
						g_pFresh->GetBitmap("rmr_arg_item_check");
					g->DrawTexture(pCheck,
						WRect(item->pos.x + 8.0f, item->pos.y + 62.0f,
							(float)pCheck->Width(), (float)pCheck->Height()),
						0xffffffff, 0);
				}
			}
		}
		break;

	case 14:
		if ((typeId & 0x3c00000) == 0x1800000)
		{
			Title* pTitle =
				g_TitleManager->GetTitle((eTitle)(typeId & 0x3fffff));
			if (pTitle)
			{
				if (!pTitle->CanEquip())
				{
					const Bitmap* pNotFit = g_pFresh->GetBitmap("do not fit");
					if (pNotFit)
						g->DrawTexture(pNotFit,
							WRect(
								WPoint(item->pos.x + 1.0f, item->pos.y + 1.0f),
								pNotFit->Width(), pNotFit->Height()),
							0xffffffff, 0);
				}
			}
		}
		break;
	}
}

void CRealMyRoomMain::MoveGift(sGiftInfo* pGift)
{
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pGift->tid);

	if (!Doc()->ConfirmInsertItem(pGift->tid, pGift->Arg0))
	{
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
			(FrCmdTarget*)this, "notify");
		WRect rect = pForm->GetRect();
		pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
		pForm->SetTimeLimit(12.0f);
		pForm->SetMessage(
			"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 "
			"\xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3"
			"\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
			"\xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 "
			"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 0);
		return;
	}

	switch (pGift->tid >> 26)
	{
	default:
		if (pCommon->IsCash)
		{
			if (GetItemNumInMyPocket(pGift->tid))
			{
				FrReturnGiftDlg* pDlg = CreateForm<FrReturnGiftDlg>(
					g_pFresh->GetManager(), (FrCmdTarget*)this, "notify_yesno");
				pDlg->SetMessage(
					MakeStr(
						"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 "
						"\\c0xffff0000\\c\xc4\xed\xc5\xb0\xbe\xc6\xc0\xcc\xc5\xdb"
						"\\c0xff000000\\c\xc0\xd4\xb4\xcf\xb4\xd9.\n"
						"\\c0xffff0000\\c%d \xc4\xed\xc5\xb0\\c0xff000000\\c\xb7\xce "
						"\xc8\xaf\xba\xd2\xb9\xde\xc0\xbb \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n"
						"\xc4\xed\xc5\xb0\xb7\xce \xc0\xfc\xc8\xaf\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
						(pCommon->SalePrice > 0 &&
							pCommon->SalePrice < pCommon->Price)
							? pCommon->SalePrice
							: pCommon->Price),
					false);
				pDlg->m_giftIdx = pGift->guid;
				pDlg->m_typeId = pGift->tid;
				pDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnReturnGiftDlgResult,
					0);
				return;
			}
		}
		break;

	case 8:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it;
		for (it = Doc()->m_caddieMap.begin(); it != Doc()->m_caddieMap.end();
			++it)
		{
			if ((*it).second.tid == (((pGift->tid >> 21) & 0x1f) | 0x1c000000))
			{
				IFF_STRUCT::sCaddie* pCaddie =
					ItemManager()->FindCaddie((*it).second.tid);
				if ((*it).second.Remain_Date == 0 && pCaddie &&
					pCaddie->MonthlyFee > 0 && !pCaddie->c.IsCash)
				{
					this << MsgObject(NULL, 0x23,
						(int)"\xc8\xde\xb0\xa1\xc1\xdf\xc0\xce \xc4\xb3\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9. "
							 "\xc0\xe7\xb0\xed\xbf\xeb \xc8\xc4 \xbc\xb1\xb9\xb0 "
							 "\xbf\xc5\xb1\xe2\xb1\xe2\xb8\xa6 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4",
						0, 0, 0, 0);
					return;
				}
				break;
			}
		}
		break;
	}

	case 5:
	case 6:
		if (pGift->tid == 0x1a000010)
		{
			m_pPangGiftDlg = CreateForm<FrPangGiftDlg>(g_pFresh->GetManager(),
				(FrCmdTarget*)this, "panggift");
			m_pPangGiftDlg->SetItemInfo(pGift);
			m_pPangGiftDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPangGiftDlgResult, 1);
			return;
		}
		break;

	case 2:
	case 4:
		if (pCommon->IsCash)
		{
			if (GetItemNumInMyPocket(pGift->tid))
			{
				FrReturnGiftDlg* pDlg = CreateForm<FrReturnGiftDlg>(
					g_pFresh->GetManager(), (FrCmdTarget*)this, "notify_yesno");
				pDlg->SetMessage(
					MakeStr(
						"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 "
						"\\c0xffff0000\\c\xc4\xed\xc5\xb0\xbe\xc6\xc0\xcc\xc5\xdb"
						"\\c0xff000000\\c\xc0\xd4\xb4\xcf\xb4\xd9.\n"
						"\\c0xffff0000\\c%d \xc4\xed\xc5\xb0\\c0xff000000\\c\xb7\xce "
						"\xc8\xaf\xba\xd2\xb9\xde\xc0\xbb \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n"
						"\xc4\xed\xc5\xb0\xb7\xce \xc0\xfc\xc8\xaf\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
						(pCommon->SalePrice > 0 &&
							pCommon->SalePrice < pCommon->Price)
							? pCommon->SalePrice
							: pCommon->Price),
					false);
				pDlg->m_giftIdx = pGift->guid;
				pDlg->m_typeId = pGift->tid;
				pDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnReturnGiftDlgResult,
					0);
				return;
			}
		}
		break;

	case 14:
		break;
	}

	m_pMoveGift = pGift;
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	pForm->SetMessage(
		"\xbc\xb1\xb9\xb0\xc0\xbb \xbf\xc5\xb1\xe2\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	pForm->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::OnMoveGiftDlgResult, 1);
}

bool CRealMyRoomMain::OnCardRemoveDlgResult(int result, FrForm* form)
{
	m_pCardRemoveDlg = NULL;
	return true;
}

void CRealMyRoomMain::OpenTikiReport(sItemInfo* pItem)
{
	if (IsLocalContent(S3_CADDIE_REPORT))
	{
		if (pItem)
		{
			if (pItem->guid == m_tikiReportUid)
			{
				HandleMsg(MsgObject(NULL, 0x23,
					(int)"\xc0\xaf\xc8\xbf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba "
						 "\xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae "
						 "\xc0\xd4\xb4\xcf\xb4\xd9. ",
					0, 0, 0, 0));
			}
			else
			{
				m_tikiReportUid = pItem->guid;
				m_tikiReportTypeId = pItem->tid;

				m_tikiReportState = (pItem->Common[1] << 15) + pItem->Common[2];

				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_yesno", NULL);
				pForm->SetMessage(
					"30\xc0\xce \xb4\xeb\xc8\xb8 \xb1\xe2\xb7\xcf\xc0\xce "
					"\xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae\xb8\xa6 "
					"\xbf\xad\xb6\xf7 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					false);
				pForm->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnOpenTikiReport, 1);
			}
		}
	}
}

void OnOpenedBoxDlg(unsigned long typeId, FRESH_PFN_RESULT pFnResult,
	FrCmdTarget* pTarget)
{
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), pTarget,
		"notify_yesno", NULL);
	pForm->SetMessage(
		"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
		"\xbf\xad\xbe\xee \xba\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);

	pForm->Open(pFnResult, 1);
}

void CRealMyRoomMain::OpenNewYearMoney(sItemInfo* pItem)
{
	if (pItem)
	{
		m_newYearMoneyUid = pItem->guid;
		OnOpenedBoxDlg(pItem->tid,
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnOpenNewYearMoneyDlgResult,
			this);
	}
}

void CRealMyRoomMain::OpenEventGiftBox(sItemInfo* pItem)
{
	if (pItem)
	{
		CGiftBoxEvent* pEvent;
		if (CContentsDoc::Instance()->GetContainer((localContentType_t)0x1d,
				pEvent))
		{
			m_eventGiftUid = pItem->guid;

			pEvent->SetOpenedBoxTID(pItem->tid);

			if (pEvent->GetOpenedBoxTID())
			{
				OnOpenedBoxDlg(pItem->tid,
					(FRESH_PFN_RESULT)&CRealMyRoomMain::
						OnOpenEventGiftDlgResult,
					this);
			}
		}
	}
}

void CRealMyRoomMain::OpenS4EventGiftBox(sItemInfo* pItem)
{
	if (pItem)
	{
		CGiftBoxEvent* pEvent;
		if (CContentsDoc::Instance()->GetContainer((localContentType_t)0x1d,
				pEvent))
		{
			m_boxTypeId = pItem->tid;

			pEvent->SetOpenedBoxTID(pItem->tid);

			if (pEvent->GetOpenedBoxTID())
			{
				OnOpenedBoxDlg(pItem->tid,
					(FRESH_PFN_RESULT)&CRealMyRoomMain::
						OnOpenS4EventGiftDlgResult,
					this);
			}
		}
	}
}

void CRealMyRoomMain::OnItemListBtnDown()
{
	FrListItem* pItem = m_pItemList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	if (IsItemStorageVisible() ? m_anotherHand.IsWorking(0)
							   : m_anotherHand.IsWorking(0) || m_subTab == 22)
	{
		m_pItemList->ToggleItem(pItem);
		return;
	}

	if (IsLocalContent(S4_ITEM_STORAGE) == true &&
		IsItemStorageVisible() == true)
	{
		m_pItemList->ToggleItem(pItem);

		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			sStoredItemInfo storedItemInfo;
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;

			if (pStorage->CheckStoredItemInfo(storedItemInfo, *pInfo) == true)
			{
				sStoredItemDragDrop dragDrop;
				memset(&dragDrop, 0, sizeof(dragDrop));
				dragDrop.mode = sStoredItemDragDrop::MODE_NONE;
				dragDrop.index = pItem->idx;
				dragDrop.pItemInfo = pInfo;
				dragDrop.storedItemInfo = storedItemInfo;
				memcpy(dragDrop.storedItemInfo.itemInfo.UccIndex,
					pInfo->UccIndex,
					sizeof(dragDrop.storedItemInfo.itemInfo.UccIndex));

				pStorage->SetDragDropItem(dragDrop);
			}
		}
		return;
	}

	if (m_mainTab == 0)
	{
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar == NULL ||
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetPetFrame() ==
				NULL)
		{
			m_pItemList->ToggleItem(pItem);
			return;
		}
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
	switch (m_subTab)
	{
	case 17:
		if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
			break;

		if ((m_miniBtn[0].rect + pItem->pos).IsInRect(mousePos))
		{
			sGiftInfo* pGift = (sGiftInfo*)pItem->pData;
			if (pGift)
				MoveGift(pGift);
		}

		if ((m_miniBtn[4].rect + pItem->pos).IsInRect(mousePos))
		{
			sGiftInfo* pGift = (sGiftInfo*)pItem->pData;
			if (pGift && pGift->sFromID[0] != '@')
			{
				m_pNoteDlg = CreateForm<FrNoteDlg>(g_pFresh->GetManager(), this,
					"note", NULL);
				m_pNoteDlg->SetNick(pGift->sFromID, pGift->sMsg);
				m_pNoteDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnNoteDlgResult, 1);
			}
		}
		break;

	case 8:
		if ((m_miniBtn[1].rect + pItem->pos).IsInRect(mousePos))
		{
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;
			if (pInfo)
			{
				EquipItem(pInfo);
				return;
			}
		}

		if ((m_miniBtn[2].rect + pItem->pos).IsInRect(mousePos))
		{
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;
			if (pInfo)
			{
				IFF_ITEM_COMMON* pCommon =
					ItemManager()->FindCommonItem(pInfo->tid);
				if (!pCommon->IsCash)
				{
					int equipped = 0;
					if (!(pInfo->tid & 0x2000000))
					{
						for (int i = 0; i < 10; i++)
							if (pInfo->tid ==
								Doc()->m_myInfo.userEquip.tidItemSlot[i])
								equipped++;
					}

					int count = pInfo->Common[0] - equipped;
					if (count > 0)
					{
						FrDeleteItemDlg* pDlg = CreateForm<FrDeleteItemDlg>(
							g_pFresh->GetManager(), this, "delete", NULL);
						pDlg->SetDelItem(pInfo->tid, count);
						pDlg->Open(NULL, 1);
					}
				}
			}
		}
		break;

	case 10:
		if ((m_miniBtn[3].rect + pItem->pos).IsInRect(mousePos))
		{
			sCaddieInfo* pCaddie = (sCaddieInfo*)pItem->pData;
			if (pCaddie)
			{
				IFF_STRUCT::sCaddie* pIffCaddie =
					ItemManager()->FindCaddie(pCaddie->tid);
				if (pCaddie->Rent_flag && pCaddie->Remain_Date <= 0 &&
					pCaddie->tidPart == 0 && pIffCaddie->MonthlyFee)
				{
					OpenCaddieInfo(*pCaddie);
				}
			}
		}
		break;

	case 9:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		bool bHandled = false;
		GetActor("ItemBuff") << MsgObject(NULL, 0x246, 1, (int)&bHandled,
			(int)pItem, (int)&mousePos, pInfo->tid);
		if (bHandled)
			break;

		if ((m_miniBtn[0].rect + pItem->pos).IsInRect(mousePos))
			ExecuteMiniButtonDown(pInfo);
		break;
	}

	case 24:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		if (pInfo && (m_miniBtn[0].rect + pItem->pos).IsInRect(mousePos))
		{
			if (pInfo->tid == 0x1a000042)
				OpenTikiReport(pInfo);
		}
		break;
	}

	case 23:
		if (!IsLocalContent(S4_REPLAY_SYSTEM))
			break;

		if ((m_miniBtn[7].rect + pItem->pos).IsInRect(mousePos))
		{
			sRecordedItemInfo* pRecorded = (sRecordedItemInfo*)pItem->pData;
			if (pRecorded->bValid == 1)
			{
				if (m_pReplayModeDlg == NULL)
				{
					m_pReplayModeDlg = CreateForm<FrReplayModeDlg>(
						g_pFresh->GetManager(), this, "replaymode", NULL);
					Doc()->SetFileName(pRecorded->fileName);
					m_pReplayModeDlg->SetReplayMode(1);
					m_pReplayModeDlg->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::
											   OnReplayModeOpenDlgResult,
						1);
				}
			}
			else
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xb9\xf6\xc1\xaf\xc0\xcc \xb3\xb7\xbe\xc6\xbc\xad "
						 "\xb8\xae\xc7\xc3\xb7\xb9\xc0\xcc \xc7\xd2 \xbc\xf6 "
						 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
			}
		}

		if ((m_miniBtn[8].rect + pItem->pos).IsInRect(mousePos))
		{
			sRecordedItemInfo* pRecorded = (sRecordedItemInfo*)pItem->pData;
			if (m_pReplayModeDlg == NULL)
			{
				m_pReplayModeDlg = CreateForm<FrReplayModeDlg>(
					g_pFresh->GetManager(), this, "replaytape_del", NULL);
				Doc()->SetFileName(pRecorded->fileName);
				m_pReplayModeDlg->SetReplayMode(2);
				m_pReplayModeDlg->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::
										   OnReplayDelOpenDlgResult,
					1);
			}
		}
		break;

	case 27:
	{
		if (!IsLocalContent(S4_CARD_SYSTEM))
			break;

		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		m_cardTypeId = pInfo->tid;

		if ((m_miniBtn[9].rect + pItem->pos).IsInRect(mousePos))
		{
			IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pInfo->tid);
			if (pCard == NULL)
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xc0\xaf\xc8\xbf\xc7\xd1 \xc4\xab\xb5\xe5\xb0\xa1 "
						 "\xbe\xc6\xb4\xcf\xb9\xc7\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 "
						 "\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				break;
			}

			if (!pCard->c.Final)
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xc0\xaf\xc8\xbf\xc7\xd1 \xc4\xab\xb5\xe5\xb0\xa1 "
						 "\xbe\xc6\xb4\xcf\xb9\xc7\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 "
						 "\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				break;
			}

			unsigned long cardType = (pInfo->tid >> 22) & 0xf;
			if (cardType == 3)
			{
				m_pOpenCardPackDlg = CreateForm<FrOpenCardPackDlg>(
					g_pFresh->GetManager(), this, "cardpack_open", NULL);
				m_pOpenCardPackDlg->m_cardId = pInfo->guid;
				m_pOpenCardPackDlg->m_cardTypeId = pInfo->tid;
				m_pOpenCardPackDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnCardPackDlgResult, 1);
			}
			else
			{
				bool bApplied = false;
				if (cardType == 2)
				{
					for (std::list<sSCardAvilityPeriodInfo>::iterator it =
							 Doc()->m_cardAbilityList[0].begin();
						it != Doc()->m_cardAbilityList[0].end(); ++it)
					{
						if ((*it).cardType == 2)
						{
							if ((*it).Avility ==
								ItemManager()->FindCard(pInfo->tid)->Avility)
							{
								this << MsgObject(NULL, 0x23,
									(int)"\xc0\xcc \xc4\xab\xb5\xe5\xc0\xc7 "
										 "\xc8\xbf\xb0\xfa\xb4\xc2 \xc0\xcc\xb9\xcc "
										 "\xc0\xfb\xbf\xeb\xb5\xc7\xbe\xee "
										 "\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
									0, 0, 0, 0);
								bApplied = true;
							}
						}
					}

					if (bApplied)
						break;
				}

				m_pCardUseDlg = CreateForm<FrCardUseDlg>(g_pFresh->GetManager(),
					this, "usecard", NULL);
				m_pCardUseDlg->m_cardId = pInfo->guid;
				m_pCardUseDlg->m_cardTypeId = pInfo->tid;
				m_pCardUseDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnCardDlgResult, 1);
			}
		}
		break;
	}

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (!IsLocalContent(S4_REAL_MYROOM))
			break;

		if (pItem->pData &&
			(m_miniBtn[10].rect + pItem->pos).IsInRect(mousePos))
		{
			if (!m_bFlag[3])
				ControlUiHideMode(4);
			else
				ControlUiHideMode(2);
		}
		break;
	}

	if (IsLocalContent(S4_UCC))
	{
		if (m_subTab == 21 || m_subTab == 26)
		{
			sItemInfo* pUccInfo = (sItemInfo*)pItem->pData;
			if (pUccInfo)
			{
				IFF_STRUCT::sPart* pPart =
					ItemManager()->FindPart(pUccInfo->tid);

				if (pPart && (pPart->Category == 7 || pPart->Category == 8))
				{
					if (!(pUccInfo->status & 1) &&
						(m_uccDrawRect + pItem->pos).IsInRect(mousePos) &&
						IsUccClothes(pUccInfo->tid))
					{
						if (m_pUccDrawDlg == NULL)
						{
							CIconToolTip::GetInstance()->ClearToolTip();

							m_pUccDrawDlg = CreateForm<FrUccDrawDlg>(
								g_pFresh->GetManager(), this, "ucc_draw", NULL);
							m_pUccDrawDlg->SetUccDrawInfo((eDrawMode)0,
								pUccInfo, false);
							if (m_pUccDrawDlg->IsInitialized())
							{
								m_pUccDrawDlg->Open(
									(FRESH_PFN_RESULT)&CRealMyRoomMain::
										OnUccDrawDlgResult,
									WPoint(373.0f, 72.0f), 1);

								unsigned long charTypeId =
									Doc()->GetMyPartTidList().m_charTid;
								ShowPet(charTypeId);

								m_pItemList->SetVisible(false);
							}
							else
							{
								this << MsgObject(NULL, 0x23,
									(int)MakeStr(
										"\xbc\xad\xb9\xf6\xb7\xce\xba\xce\xc5\xcd "
										"\xc7\xca\xbf\xe4\xc7\xd1 "
										"\xc1\xa4\xba\xb8\xb8\xa6 \xbe\xf2\xc1\xf6 "
										"\xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9"
										"(\xc4\xda\xb5\xe5:%d)",
										2),
									0, 0, 0, 0);
							}
						}
					}
				}

				if (pPart && (pPart->Category == 7 || pPart->Category == 9))
				{
					int index;
					if (pPart->Category == 7)
						index = 0;
					else
						index = 1;

					if (!(pUccInfo->status & 1) &&
						((&m_uccCopyRect)[index] + pItem->pos)
							.IsInRect(mousePos) &&
						IsUccClothes(pUccInfo->tid))
					{
						if (m_pUccCopyDlg == NULL)
						{
							CIconToolTip::GetInstance()->ClearToolTip();
							m_pItemList->SetVisible(false);
							m_pUccCopyDlg = CreateForm<FrUccCopyDlg>(
								g_pFresh->GetManager(), this, "ucc_copy", NULL);
							m_pUccCopyDlg->SetItem(pUccInfo);
							m_pUccCopyDlg->Open(
								(FRESH_PFN_RESULT)&CRealMyRoomMain::
									OnUccCopyDlgResult,
								WPoint(359.0f, 71.0f), 1);
						}
					}
				}

				if (m_pItemList)
				{
					if (!(pUccInfo->status & 1) && IsUccClothes(pUccInfo->tid))
					{
						m_pItemList->UnselectItem(pItem);
						return;
					}

					if (IsUccClothes(pUccInfo->tid) &&
						!UccManager()->IsDownloadComplete(pUccInfo->tid,
							pUccInfo->UccIndex))
					{
						m_pItemList->UnselectItem(pItem);
						return;
					}
				}
			}
		}
	}

	if (!pItem->selected)
		UnselectItemList(pItem);
	else
		SelectItemList(pItem);
}

void CRealMyRoomMain::OnItemListMouseMove()
{
	if (m_pItemList->GetItemUnderCursor() == NULL)
		return;

	if (IsItemStorageVisible() ? m_anotherHand.IsWorking(0)
							   : m_anotherHand.IsWorking(0) || m_subTab == 22)
		return;

	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			pStorage->SetDrag(sStoredItemDragDrop::MODE_ITEM_LIST);
		}
	}
}

void CRealMyRoomMain::OnItemListRBtnDown()
{
	if (m_pItemList == NULL)
		return;

	FrListItem* pItem = m_pItemList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	if (!m_anotherHand.IsWorking(0))
	{
		if (m_mainTab != 0 ||
			(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame()))
		{
			if (IsItemStorageVisible())
			{
				m_pItemList->ToggleItem(pItem);
				OpenInformation(pItem);
				return;
			}

			switch (m_subTab)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 21:
			case 26:
			{
				sItemInfo* pInfo = (sItemInfo*)pItem->pData;
				if (pInfo == NULL)
					return;

				if (pInfo->ItemType == 6 && pInfo->Expired)
				{
					m_pItemList->ToggleItem(pItem);
					OpenInformation(pItem);
					return;
				}
				break;
			}
			}

			if (m_subTab == 9)
			{
				sItemInfo* pInfo = (sItemInfo*)pItem->pData;
				if (pInfo == NULL)
					return;

				if (pInfo->tid == 0x1a000004)
				{
					if (m_pFortuneDlg == NULL)
					{
						IFF_STRUCT::sDesc* pDesc =
							ItemManager()->FindDesc(0x1a000004);
						m_pFortuneDlg = CreateForm<FrFortuneDlg>(
							g_pFresh->GetManager(), this, "fortune", NULL);
						m_pFortuneDlg->SetMessage(pDesc ? pDesc->Desc : NULL,
							false);
						m_pFortuneDlg->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::
												OnFortuneDlgResult,
							1);
					}
				}
				else
					OpenInformation(pItem);
			}
			else
				OpenInformation(pItem);

			if (m_subTab != 22)
			{
				bool bValid = true;
				if (IsLocalContent(S4_UCC))
				{
					sItemInfo* pInfo = (sItemInfo*)pItem->pData;
					if (IsUccClothes(pInfo->tid))
					{
						if (!(pInfo->status & 1) ||
							!UccManager()->IsDownloadComplete(pInfo->tid,
								pInfo->UccIndex))
							bValid = false;
					}
				}

				if (bValid)
				{
					if (pItem->selected)
						SelectItemList(pItem);
					else
						m_pItemList->ToggleItem(pItem);
					return;
				}
			}
		}
	}

	m_pItemList->ToggleItem(pItem);
}

void CRealMyRoomMain::OnItemListDClick()
{
	if (!IsLocalContent(S4_ITEM_STORAGE) || !IsItemStorageVisible())
	{
		FrListItem* pItem = m_pItemList->GetItemUnderCursor();
		if (pItem)
		{
			if (m_subTab == 8)
			{
				EquipItem((sItemInfo*)pItem->pData);
			}
		}
	}
	else if (IsLocalContent(S4_ITEM_STORAGE) == true &&
		IsItemStorageVisible() == true)
	{
		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			if (!pStorage->IsLock())
			{
				FrListItem* pItem = m_pItemList->GetItemUnderCursor();
				if (pItem)
				{
					pStorage->ReqPushStoredItemInfo(*pItem);
				}
			}
		}
	}
}

void CRealMyRoomMain::OnPowerEditInit(int param)
{
	m_pStatEdit[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::OnControlEditInit(int param)
{
	m_pStatEdit[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::OnImpactEditInit(int param)
{
	m_pStatEdit[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::OnSpinEditInit(int param)
{
	m_pStatEdit[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::OnCurveEditInit(int param)
{
	m_pStatEdit[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CRealMyRoomMain::OnUpgradeInit(int param)
{
	m_pUpgrade = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pUpgrade->SetVisible(false);

	m_pUpgrade->EnableHover(true, 0.0f);
}

void CRealMyRoomMain::OnUpgradeUp()
{
	IFF_STRUCT::sClubSet* pClubSet =
		Doc()->m_itemManager.FindClubSet(m_clubSetTypeId);
	if (pClubSet)
	{
		if ((pClubSet->c.Level > Doc()->m_myInfo.stat.Level &&
				!pClubSet->c.IsUnderLvl) ||
			(pClubSet->c.Level < Doc()->m_myInfo.stat.Level &&
				pClubSet->c.IsUnderLvl))
		{
			HandleMsg(MsgObject(NULL, 0x23,
				(int)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 "
					 "\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 "
					 "\xc8\xc4\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0.",
				0, 0, 0, 0));
			return;
		}
	}

	m_pDowngradeDlg = CreateForm<FrDowngradeDlg>(g_pFresh->GetManager(), this,
		"downgrade", NULL);
	if (m_mainTab == 0)
		m_pDowngradeDlg->SetInfo(m_charTypeId, m_pCharStat, m_pCharParts,
			m_pAuxParts, 0);
	else if (m_mainTab == 1)
		m_pDowngradeDlg->SetInfo(Doc()->m_myInfo.userEquip.guidClubSet, 0);

	m_pDowngradeDlg->Open(
		(FRESH_PFN_RESULT)&CRealMyRoomMain::OnUpgradeDlgResult, 3);

	OnTooltipHoverOff(0);
}

void CRealMyRoomMain::OnDowngradeInit(int param)
{
	m_pDowngrade = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pDowngrade->SetVisible(false);

	m_pDowngrade->EnableHover(true, 0.0f);
}

void CRealMyRoomMain::OnDowngradeUp()
{
	IFF_STRUCT::sClubSet* pClubSet =
		Doc()->m_itemManager.FindClubSet(m_clubSetTypeId);
	if (pClubSet)
	{
		if ((pClubSet->c.Level > Doc()->m_myInfo.stat.Level &&
				!pClubSet->c.IsUnderLvl) ||
			(pClubSet->c.Level < Doc()->m_myInfo.stat.Level &&
				pClubSet->c.IsUnderLvl))
		{
			HandleMsg(MsgObject(NULL, 0x23,
				(int)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 "
					 "\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 "
					 "\xc8\xc4\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0.",
				0, 0, 0, 0));
			return;
		}
	}

	m_pDowngradeDlg = CreateForm<FrDowngradeDlg>(g_pFresh->GetManager(), this,
		"downgrade", NULL);
	if (m_mainTab == 0)
		m_pDowngradeDlg->SetInfo(m_charTypeId, m_pCharStat, m_pCharParts,
			m_pAuxParts, 1);
	else if (m_mainTab == 1)
		m_pDowngradeDlg->SetInfo(Doc()->m_myInfo.userEquip.guidClubSet, 1);

	m_pDowngradeDlg->Open(
		(FRESH_PFN_RESULT)&CRealMyRoomMain::OnUpgradeDlgResult, 3);

	OnTooltipHoverOff(0);
}

void CRealMyRoomMain::OnTooltipHoverOn(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton == m_pUpgrade)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_UPGRADE);
	}
	else if (pButton == m_pDowngrade)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_DOWNGRADE);
	}
	else if (pButton == m_pEquipCancel)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_EQUIPCANCEL);
	}
	else if (pButton == m_pEquipReset)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_EQUIPRESET);
	}
}

void CRealMyRoomMain::OnTooltipHoverOff(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	CIconToolTip* pToolTip = CIconToolTip::GetInstance();

	bool bClear = false;
	switch (pToolTip->GetCurTipIndex())
	{
	case TOOLTIP_UPGRADE:
		if (pButton == m_pUpgrade)
			bClear = true;
		break;

	case TOOLTIP_DOWNGRADE:
		if (pButton == m_pDowngrade)
			bClear = true;
		break;

	case TOOLTIP_EQUIPCANCEL:
		if (pButton == m_pEquipCancel)
			bClear = true;
		break;

	case TOOLTIP_EQUIPRESET:
		if (pButton == m_pEquipReset)
			bClear = true;
		break;
	}

	if (pButton == NULL || bClear)
		CIconToolTip::GetInstance()->ClearToolTip();
}

void CRealMyRoomMain::GetTotalStatus(unsigned char* const level,
	unsigned char* const capacity, unsigned char* const penalty)
{
	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(255);
		CCardManager::Instance()->CalcCardPeriodAndStatus();
	}

	for (int i = 0; i < 5; i++)
	{
		status_t status =
			ItemManager()->GetLevelWithPenalty((eLvlType)i, m_charTypeId,
				m_pCharStat, m_pCharParts, m_pAuxParts, m_clubSetTypeId,
				m_pClubSetStat, m_caddieTypeId, Doc()->m_myInfo.stat.Level);
		level[i] = status.level;
		penalty[i] = status.penalty;

		capacity[i] = ItemManager()->GetCapacity((eLvlType)i, m_charTypeId,
			m_pCharStat, m_pCharParts, m_clubSetTypeId, m_pClubSetStat,
			m_caddieTypeId, Doc()->m_myInfo.stat.Level);
	}
}

void CRealMyRoomMain::GetCharacterStatus(unsigned char* const level,
	unsigned char* const capacity)
{
	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(255);
		CCardManager::Instance()->CalcCardPeriodAndStatus();
	}

	if (ItemManager()->FindChar(m_charTypeId))
	{
		for (int i = 0; i < 5; i++)
		{
			unsigned char itemLevel = ItemManager()->GetItemLevel((eLvlType)i,
				m_pCharParts, m_pAuxParts);
			capacity[i] = itemLevel +
				ItemManager()->GetCharCapacity((eLvlType)i, m_charTypeId,
					m_pCharParts, m_pAuxParts, Doc()->m_myInfo.stat.Level);
			level[i] = itemLevel +
				ItemManager()->GetCharLevel((eLvlType)i, m_charTypeId,
					m_pCharStat);
		}
	}
}

void CRealMyRoomMain::GetCaddieStatus(unsigned char* const level,
	unsigned char* const capacity)
{
	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieTypeId);
	if (pCaddie)
	{
		for (int i = 0; i < 5; i++)
			level[i] += pCaddie->Attr[i];
	}
}

void CRealMyRoomMain::GetClubSetStatus(unsigned char* const level,
	unsigned char* const slot)
{
	std::map<unsigned int, sItemInfo>::iterator it =
		Doc()->m_clubSetMap.find(m_clubSetUid);
	if (it != Doc()->m_clubSetMap.end())
	{
		for (int i = 0; i < 5; i++)
			level[i] = (*it).second.Common[i];
	}

	IFF_STRUCT::sClubSet* pClubSet =
		ItemManager()->FindClubSet(m_clubSetTypeId);
	if (pClubSet)
	{
		for (int i = 0; i < 5; i++)
		{
			slot[i] += pClubSet->Slot[i];
			level[i] += pClubSet->Attr[i];
		}
	}
}

void CRealMyRoomMain::SetLevelBar()
{
	unsigned char level[5];
	unsigned char capacity[5];
	unsigned char penalty[5];

	memset(level, 0, sizeof(level));
	memset(capacity, 0, sizeof(capacity));
	memset(penalty, 0, sizeof(penalty));

	for (int i = 0; i < 5; i++)
	{
		if (m_pStatBar[i])
		{
			m_pStatBar[i]->SetDestPos(0, false);
			m_pStatBar[i]->SetExpand(0);
		}
	}

	switch (m_statTab)
	{
	case 1:
		GetTotalStatus(level, capacity, penalty);
		break;

	case 0:
		switch (m_subTab)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 21:
		case 22:
		case 26:
			GetCharacterStatus(level, capacity);
			m_statSubject = 0;
			break;
		case 6:
			GetClubSetStatus(level, capacity);
			m_statSubject = 1;
			break;
		case 10:
			GetCaddieStatus(level, capacity);
			m_statSubject = 2;
			break;
		}
		break;
	}

	for (int i = 0; i < 5; i++)
	{
		if (m_pStatBar[i])
		{
			m_pStatBar[i]->SetDestPos(level[i], false);
			m_pStatBar[i]->SetExpand(capacity[i] - level[i]);
		}

		if (m_pStatEdit[i])
			m_pStatEdit[i]->SetCaption11(
				MakeStatusString3(level[i], capacity[i], penalty[i]));
	}
}

void CRealMyRoomMain::BuildDisplayStand(subTab_t subTab)
{
	if (m_pItemList == NULL)
		return;

	m_pItemList->ClearItem();

	switch (subTab)
	{
	case 4:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (S5::UnderCategory(m_charTypeId, pInfo->tid, category))
			{
				if (IsAngelWing(pInfo->tid))
				{
					if (!IsMannerPlayer() || Doc()->m_myInfo.stat.Level < 21)
						continue;
				}

				m_pItemList->AddItem(pInfo);
			}
		}

		for (int i = 0; i < 24; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pListItem->pData;

				std::map<unsigned int, sCharacterInfo>::iterator itChar =
					Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
				if (itChar == Doc()->m_charMap.end())
					continue;

				int slot = (pInfo->tid >> 13) & 0x1f;
				if (itChar->second.ItemIdList[slot] == 0)
				{
					if (pInfo->tid != m_pCharParts[i])
						continue;
				}
				else
				{
					if (pInfo->tid != m_pCharParts[i] ||
						pInfo->guid != m_partsUid[i])
						continue;
				}

				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
				if (pPart)
				{
					pInfo->CharacterSlotNum = pPart->CharacterSlot;
					pInfo->CaddieSlotNum = pPart->CaddieSlot;
				}

				m_pItemList->SelectItem(pListItem, false);
				break;
			}
		}

		for (std::list<sItemInfo>::iterator it = Doc()->m_auxPartList.begin();
			it != Doc()->m_auxPartList.end(); ++it)
			m_pItemList->AddItem(&(*it));

		for (int i = 0; i < 5; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				if (((sItemInfo*)pListItem->pData)->tid == m_pAuxParts[i])
				{
					m_pItemList->SelectItem(pListItem, false);
					break;
				}
			}
		}
	}
	break;

	case 1:
	case 2:
	case 3:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (S5::UnderCategory(m_charTypeId, pInfo->tid, category))
			{
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
				if (pPart)
				{
					pInfo->CharacterSlotNum = pPart->CharacterSlot;
					pInfo->CaddieSlotNum = pPart->CaddieSlot;
				}

				m_pItemList->AddItem(pInfo);
			}
		}

		for (int i = 0; i < 24; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pListItem->pData;

				std::map<unsigned int, sCharacterInfo>::iterator itChar =
					Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
				if (itChar == Doc()->m_charMap.end())
					continue;

				int slot = (pInfo->tid >> 13) & 0x1f;
				if (itChar->second.ItemIdList[slot] == 0)
				{
					if (pInfo->tid != m_pCharParts[i])
						continue;
				}
				else
				{
					if (pInfo->tid != m_pCharParts[i] ||
						pInfo->guid != m_partsUid[i])
						continue;
				}

				m_pItemList->SelectItem(pListItem, false);
				break;
			}
		}
	}
	break;

	case 26:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_uccItemList.begin();
			it != Doc()->m_uccItemList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (S5::UnderCategory(m_charTypeId, pInfo->tid, category))
			{
				if (IsLocalContent((localContentType_t)0x51))
				{
					IFF_STRUCT::sPart* pPart =
						ItemManager()->FindPart(pInfo->tid);
					if (pPart)
					{
						pInfo->CharacterSlotNum = pPart->CharacterSlot;
						pInfo->CaddieSlotNum = pPart->CaddieSlot;
					}
				}

				m_pItemList->AddItem(pInfo);
			}
		}

		for (int i = 0; i < 24; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pListItem->pData;

				if (IsLocalContent((localContentType_t)0x51))
				{
					std::map<unsigned int, sCharacterInfo>::iterator itChar =
						Doc()->m_charMap.find(
							Doc()->m_myInfo.userEquip.guidChar);
					if (itChar == Doc()->m_charMap.end())
						continue;

					bool bFind = false;
					int slot = (pInfo->tid >> 13) & 0x1f;
					if (itChar->second.ItemIdList[slot] == 0)
					{
						if (pInfo->tid == m_pCharParts[i])
							bFind = true;
					}
					else if (pInfo->tid == m_pCharParts[i] &&
						pInfo->guid == m_partsUid[i])
						bFind = true;

					if (bFind)
					{
						m_pItemList->SelectItem(pListItem, false);
						break;
					}
				}
				else if (pInfo->tid == m_pCharParts[i])
				{
					m_pItemList->SelectItem(pListItem, false);
					break;
				}
			}
		}
	}
	break;

	case 21:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (S5::UnderCategory(m_charTypeId, pInfo->tid, category))
			{
				if (IsLocalContent((localContentType_t)0x51))
				{
					IFF_STRUCT::sPart* pPart =
						ItemManager()->FindPart(pInfo->tid);
					if (pPart)
					{
						pInfo->CharacterSlotNum = pPart->CharacterSlot;
						pInfo->CaddieSlotNum = pPart->CaddieSlot;
					}
				}

				m_pItemList->AddItem(pInfo);
			}
		}

		for (int i = 0; i < 24; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pListItem->pData;

				std::map<unsigned int, sCharacterInfo>::iterator itChar =
					Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
				if (itChar == Doc()->m_charMap.end())
					continue;

				int slot = (pInfo->tid >> 13) & 0x1f;
				if (itChar->second.ItemIdList[slot] == 0)
				{
					if (pInfo->tid != m_pCharParts[i])
						continue;
				}
				else
				{
					if (pInfo->tid != m_pCharParts[i] ||
						pInfo->guid != m_partsUid[i])
						continue;
				}

				m_pItemList->SelectItem(pListItem, false);
				break;
			}
		}

		for (std::list<sItemInfo>::iterator it = Doc()->m_auxPartList.begin();
			it != Doc()->m_auxPartList.end(); ++it)
			m_pItemList->AddItem(&(*it));

		for (int i = 0; i < 5; i++)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				FrListItem* pListItem = *it;
				if (pListItem && pListItem->pData &&
					((sItemInfo*)pListItem->pData)->tid == m_pAuxParts[i])
				{
					m_pItemList->SelectItem(pListItem, false);
					break;
				}
			}
		}

		if (IsLocalContent((localContentType_t)0x57))
		{
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_uccItemList.begin();
				it != Doc()->m_uccItemList.end(); ++it)
			{
				sItemInfo* pInfo = &(*it);
				if (S5::UnderCategory(m_charTypeId, pInfo->tid, category))
				{
					if (IsLocalContent((localContentType_t)0x51))
					{
						IFF_STRUCT::sPart* pPart =
							ItemManager()->FindPart(pInfo->tid);
						if (pPart)
						{
							pInfo->CharacterSlotNum = pPart->CharacterSlot;
							pInfo->CaddieSlotNum = pPart->CaddieSlot;
						}
					}

					m_pItemList->AddItem(pInfo);
				}
			}

			for (int i = 0; i < 24; i++)
			{
				for (FrListBox::ITEM_LIST::iterator it =
						 m_pItemList->m_itemList.begin();
					it != m_pItemList->m_itemList.end(); ++it)
				{
					FrListItem* pListItem = *it;
					sItemInfo* pInfo = (sItemInfo*)pListItem->pData;

					if (IsLocalContent((localContentType_t)0x51))
					{
						std::map<unsigned int, sCharacterInfo>::iterator
							itChar = Doc()->m_charMap.find(
								Doc()->m_myInfo.userEquip.guidChar);
						if (itChar == Doc()->m_charMap.end())
							continue;

						bool bFind = false;
						int slot = (pInfo->tid >> 13) & 0x1f;
						if (itChar->second.ItemIdList[slot] == 0)
						{
							if (pInfo->tid == m_pCharParts[i])
								bFind = true;
						}
						else if (pInfo->tid == m_pCharParts[i] &&
							pInfo->guid == m_partsUid[i])
							bFind = true;

						if (bFind)
						{
							m_pItemList->SelectItem(pListItem, false);
							break;
						}
					}
					else if (pInfo->tid == m_pCharParts[i])
					{
						m_pItemList->SelectItem(pListItem, false);
						break;
					}
				}
			}
		}
	}
	break;

	case 22:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (!((*it).tid & 0x600))
			{
				std::map<unsigned int, sCharacterInfo>::const_iterator itChar;
				for (itChar = Doc()->m_charMap.begin();
					itChar != Doc()->m_charMap.end(); ++itChar)
				{
					if ((((*it).tid >> 18) & 0xff) ==
						((*itChar).second.tid & 0x3ffffff))
						break;
				}

				if (itChar == Doc()->m_charMap.end())
				{
					if (IsLocalContent((localContentType_t)0x51))
					{
						IFF_STRUCT::sPart* pPart =
							ItemManager()->FindPart(pInfo->tid);
						if (pPart)
						{
							(*it).CharacterSlotNum = pPart->CharacterSlot;
							(*it).CaddieSlotNum = pPart->CaddieSlot;
						}
					}

					m_pItemList->AddItem(&(*it));
				}
			}
		}

		if (IsLocalContent((localContentType_t)0x57))
		{
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_uccItemList.begin();
				it != Doc()->m_uccItemList.end(); ++it)
			{
				sItemInfo* pInfo = &(*it);
				if (pInfo->tid & 0x600)
					continue;

				std::map<unsigned int, sCharacterInfo>::const_iterator itChar;
				for (itChar = Doc()->m_charMap.begin();
					itChar != Doc()->m_charMap.end(); ++itChar)
				{
					if (((pInfo->tid >> 18) & 0xff) ==
						((*itChar).second.tid & 0x3ffffff))
						break;
				}

				if (itChar == Doc()->m_charMap.end())
					m_pItemList->AddItem(&(*it));
			}
		}
	}
	break;

	case 6:
		for (std::map<unsigned int, sItemInfo>::const_iterator it =
				 Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end(); ++it)
		{
			FrListItem* pListItem = m_pItemList->AddItem((void*)&(*it).second);

			if (m_clubSetUid == 0)
			{
				if (Doc()->m_myInfo.userEquip.guidClubSet == (*it).second.guid)
				{
					m_clubSetUid = (*it).second.guid;
					m_pItemList->SelectItem(pListItem, true);
				}
			}
			else if (m_clubSetUid == (*it).second.guid)
			{
				m_pItemList->SelectItem(pListItem, true);
			}
		}
		break;

	case 7:
		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			FrListItem* pListItem = m_pItemList->AddItem(&(*it));
			if (Doc()->m_myInfo.userEquip.tidBall == (*it).tid)
				m_pItemList->SelectItem(pListItem, true);
		}
		break;

	case 8:
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);

			if (ItemManager()->GetCouponKind(pInfo->tid))
				continue;

			if (!(pInfo->tid & 0x2000000))
			{
				int equipped = 0;
				for (int i = 0; i < 10; i++)
				{
					if (pInfo->tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
						equipped++;
				}

				if (pInfo->Common[0] - equipped <= 0)
					continue;

				FrListItem* pListItem = m_pItemList->AddItem(pInfo);
				pListItem->AddNoRBtnIdx(1);

				IFF_ITEM_COMMON* pCommon =
					ItemManager()->FindCommonItem(pInfo->tid);
				if (pCommon && !pCommon->IsCash)
					pListItem->AddNoRBtnIdx(2);
			}
		}

		if (m_pItemSlot)
		{
			m_pItemSlot->ClearItem();

			int slotNum = Doc()->GetNumAddItemSlotByMascot() + 8;

			if (IsLocalContent((localContentType_t)0x84))
			{
				if (IsLocalContent((localContentType_t)0x51))
				{
					CCardManager::Instance()->SetPlayerIndex(0xff);
					CCardManager::Instance()->CalcCardPeriodAndStatus();

					int cardSlot =
						CCardManager::Instance()->GetCardPeriodSlot();
					if (cardSlot > 0)
					{
						if (cardSlot < 1 || cardSlot > 77)
							slotNum += cardSlot;
						else
							slotNum += cardSlot;
					}
				}

				for (int i = 0; i < 10; i++)
				{
					unsigned long* pTypeId =
						&Doc()->m_myInfo.userEquip.tidItemSlot[i];
					if (i < slotNum)
					{
						if (i <= 1 || pTypeId == NULL)
							m_pItemSlot->AddItem(pTypeId);
						else
							m_pItemSlot->AddItem(pTypeId);
					}
					else
						UnequipItem(pTypeId);
				}

				for (int n = m_pItemSlot->GetCurrentItemSize(true); n < 10; n++)
					m_pItemSlot->AddItem(NULL);
			}
			else
			{
				if (IsLocalContent((localContentType_t)0x51))
				{
					CCardManager::Instance()->SetPlayerIndex(0xff);
					CCardManager::Instance()->CalcCardPeriodAndStatus();
					slotNum += CCardManager::Instance()->GetCardPeriodSlot();
				}

				for (int i = 0; i < slotNum; i++)
					m_pItemSlot->AddItem(
						&Doc()->m_myInfo.userEquip.tidItemSlot[i]);

				for (int n = m_pItemSlot->GetCurrentItemSize(true); n < 10; n++)
					m_pItemSlot->AddItem(NULL);
			}
		}
	}
	break;

	case 9:
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);

			if (ItemManager()->GetCouponKind(pInfo->tid))
				continue;

			if (ItemManager()->IsNonVisibleItem(pInfo->tid) == true)
				continue;

			if (!(pInfo->tid & 0x2000000))
				continue;

			FrListItem* pListItem = m_pItemList->AddItem(pInfo);

			if (CCodeTinker().IsNeedOpenMiniButton(pInfo->tid))
				pListItem->AddNoRBtnIdx(0);
		}
		break;

	case 24:
		if (IsLocalContent((localContentType_t)0x14))
		{
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
			{
				sItemInfo* pInfo = &(*it);
				if (pInfo->tid & 0x2000000)
				{
					if (pInfo->tid == 0x1a000042)
					{
						FrListItem* pListItem = m_pItemList->AddItem(pInfo);
						pListItem->AddNoRBtnIdx(0);
					}
				}
			}
		}
		break;

	case 13:
	case 14:
	case 15:
	case 16:
	case 34:
	{
		unsigned long category = GetCategoryFlag(subTab);

		for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);

			if (category == 5 || ((pInfo->tid >> 22) & 0xf) == category)
			{
				FrListItem* pListItem = m_pItemList->AddItem(pInfo);

				eEquipSkin index;
				if (S5::UTIL::GetEquipSkinIndex(index, pInfo->tid))
				{
					if (Doc()->m_myInfo.userEquip.tidSkin[index] == pInfo->tid)
						m_pItemList->SelectItem(pListItem, true);
				}
			}
		}
	}
	break;

	case 33:
		for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);

			if (S5::ISCutinItem(pInfo->tid))
			{
				IFF_STRUCT::S5::sCutinInformation* pCutin =
					ItemManager()->FindCutinInformation(pInfo->tid);
				if (pCutin == NULL)
					continue;

				if ((m_charTypeId & 0x3ffffff) == pCutin->CharIndex ||
					pCutin->CharIndex >= 100)
				{
					FrListItem* pListItem = m_pItemList->AddItem(pInfo);

					eEquipSkin index;
					if (S5::UTIL::GetEquipSkinIndex(index, pInfo->tid))
					{
						if (Doc()->m_myInfo.userEquip.tidSkin[index] ==
							pInfo->tid)
							m_pItemList->SelectItem(pListItem, true);
					}
				}
			}
		}
		break;

	case 10:
		for (std::map<unsigned int, sCaddieInfo>::const_iterator it =
				 Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
			FrListItem* pListItem = m_pItemList->AddItem((void*)&(*it).second);

			IFF_STRUCT::sCaddie* pCaddie =
				ItemManager()->FindCaddie((*it).second.tid);
			if ((*it).second.Rent_flag && (*it).second.Remain_Date <= 0 &&
				(*it).second.tidPart == 0 && pCaddie->MonthlyFee)
				pListItem->AddNoRBtnIdx(3);

			if (m_caddieUid == 0)
			{
				if (Doc()->m_myInfo.userEquip.guidCaddie == (*it).second.guid)
				{
					m_caddieUid = (*it).second.guid;
					m_pItemList->SelectItem(pListItem, true);
				}
			}
			else if (m_caddieUid == (*it).second.guid)
			{
				m_pItemList->SelectItem(pListItem, true);
			}
		}
		break;

	case 12:
		for (std::map<unsigned int, sMascotInfo>::const_iterator it =
				 Doc()->m_mascotMap.begin();
			it != Doc()->m_mascotMap.end(); ++it)
		{
			FrListItem* pListItem = m_pItemList->AddItem((void*)&(*it).second);
			if (Doc()->m_myInfo.userEquip.guidMascot == (*it).second.guid)
				m_pItemList->SelectItem(pListItem, true);
		}
		break;

	case 17:
		if (IsLocalContent((localContentType_t)0x60))
			break;

		if (Doc()->m_giftList.empty())
		{
			m_pGiftDesc->ClearLine();
			m_pGiftDesc->AddText(
				MakeStr(
					"\xb5\xb5\xc2\xf8\xc7\xd1 \xbc\xb1\xb9\xb0\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0),
				false, true);
			break;
		}

		for (std::list<sGiftInfo>::iterator it = Doc()->m_giftList.begin();
			it != Doc()->m_giftList.end(); ++it)
		{
			FrListItem* pListItem = m_pItemList->AddItem(&(*it));
			pListItem->AddNoRBtnIdx(0);

			if ((*it).sFromID[0] != '@')
				pListItem->AddNoRBtnIdx(4);
		}
		break;

	case 18:
	{
		m_trophyList.clear();

		sTrophyStatistics* pCur = &Doc()->m_myInfo.trophy;
		sTrophyStatistics* pOld = &Doc()->m_oldSeasonTrophy;

		for (int i = 0; i <= 12; i++)
		{
			for (int j = 2; j >= 0; j--)
			{
				if (pCur->Trophy[i][j] > 0 || pOld->Trophy[i][j] > 0)
				{
					sSpecialTrophy trophy;
					trophy.count = pCur->Trophy[i][j] + pOld->Trophy[i][j];
					trophy.tid = 0x2c000000 | (i << 16) | j;
					trophy.guid = 0;
					m_trophyList.push_back(trophy);
				}
			}
		}

		for (std::list<sSpecialTrophy>::iterator it =
				 Doc()->m_specialTrophyList.begin();
			it != Doc()->m_specialTrophyList.end(); ++it)
			m_trophyList.push_back(*it);

		for (std::list<sSpecialTrophy>::iterator it =
				 Doc()->m_oldSpecialTrophyList.begin();
			it != Doc()->m_oldSpecialTrophyList.end(); ++it)
			m_trophyList.push_back(*it);

		for (std::list<sSpecialTrophy>::reverse_iterator it =
				 m_trophyList.rbegin();
			it != m_trophyList.rend(); ++it)
			m_pItemList->AddItem(&(*it));
	}
	break;

	case 19:
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			sItemInfo* pInfo = &(*it);
			if (ItemManager()->GetCouponKind(pInfo->tid))
				m_pItemList->AddItem(pInfo);
		}
		break;

	case 20:
		for (std::list<sQuest>::iterator it = Doc()->m_questList.begin();
			it != Doc()->m_questList.end(); ++it)
			m_pItemList->AddItem(&(*it));
		break;

	case 23:
		if (IsLocalContent((localContentType_t)0x4f))
		{
			Doc()->ReplaySavFileCheck();

			for (std::list<sRecordedItemInfo>::iterator it =
					 Doc()->m_recordedItemList.begin();
				it != Doc()->m_recordedItemList.end(); ++it)
				m_pItemList->AddItem(&(*it));
		}
		break;

	case 27:
		if (IsLocalContent((localContentType_t)0x51))
		{
			for (std::list<sCards>::iterator it = Doc()->m_cardList.begin();
				it != Doc()->m_cardList.end(); ++it)
				m_pItemList->AddItem(&(*it));
		}

		BuildCardPackList();

		if (IsLocalContent((localContentType_t)0x51) == true)
		{
			if (m_pCardBook)
			{
				if (m_pCardBook->IsShow())
					m_pCardBook->ReBuildCurrentPage();
			}
		}
		break;

	case 32:
		for (std::list<sFurniture_List>::iterator it =
				 Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
			m_pItemList->AddItem(&(*it));
		break;

	case 28:
	case 29:
	case 30:
		if (IsLocalContent((localContentType_t)0x56))
		{
			unsigned long category = GetCategoryFlag(subTab);

			for (std::list<sFurniture_List>::iterator it =
					 Doc()->m_furnitureList.begin();
				it != Doc()->m_furnitureList.end(); ++it)
			{
				if ((((*it).typeId >> 11) & 0x7ff) == category)
					m_pItemList->AddItem(&(*it));
			}
		}
		break;

	case 31:
		if (IsLocalContent((localContentType_t)0x56))
		{
			for (std::list<sFurniture_List>::iterator it =
					 Doc()->m_furnitureList.begin();
				it != Doc()->m_furnitureList.end(); ++it)
			{
				int category = ((*it).typeId >> 11) & 0x7ff;
				if ((category >= 0 && category < 7) || category >= 10)
					m_pItemList->AddItem(&(*it));
			}
		}
		break;
	}

	m_pItemList->ResetTopRow();

	for (int n = m_pItemList->GetCurrentItemSize(true); n < 8; n++)
		m_pItemList->AddItem(NULL);

	if (subTab != 18)
		m_pItemList->SortItem(EmptyCompares);
}

void CRealMyRoomMain::ShowMainModel()
{
	SetLevelBar();

	if (m_pExhibition)
	{
		m_pExhibition->ResetCameraRecoverTime();
		m_pExhibition->ParseBoundingBoxScript();
	}

	m_anotherHand.Enter();
	if (m_pExhibition)
		m_pExhibition->GetPetFrame()->Refresh(&Doc()->GetMyPartTidList());

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		CRealMyRoom_AvatarManager::Instance()
			->m_pMyAvatar->ChangeCharacterInfo();

		CPetFrame* pPetFrame =
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetPetFrame();
		if (pPetFrame)
			pPetFrame->Refresh(&Doc()->GetMyPartTidList());
	}
	m_anotherHand.Leave();
}

void CRealMyRoomMain::DrawSelectItemList(FrListItem* pItem)
{
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	if (pInfo != NULL)
	{
	}
	else
	{
		m_pItemDesc2->SetVisible(false);
		return;
	}

	IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pInfo->tid);

	m_pItemDesc2->SetVisible(true);

	m_itemDesc2Pos = pItem->pos;
	m_furnitureDescPos = pItem->pos;

	m_pItemDesc2->ClearLine();
	if (pDesc)
	{
		m_pItemDesc2->AddText(pDesc->Desc, false, true);

		int lineNum = m_pItemDesc2->GetLineNum();
		float height = (float)m_pItemDesc2->GetLineHeight() * lineNum;
		m_pItemDesc2->SetRect(WRect(m_itemDescOffset.x + m_furnitureDescPos.x,
			m_furnitureDescPos.y + m_itemDescOffset.y - height,
			m_pItemDesc2->GetRect().w, height));

		m_pItemDesc2->GetScrollBar()->ScrollToFirst();
	}
	else
	{
		m_pItemDesc2->SetVisible(false);
		m_pItemDescBg2->SetVisible(false);
	}
}

bool CRealMyRoomMain::SelectItemList(unsigned long typeId)
{
	if (m_pItemList == NULL)
		return false;

	FrListItem* pSelected = NULL;
	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
		if (pInfo == NULL)
			continue;

		if (pInfo->tid == typeId)
		{
			pSelected = *it;
			break;
		}
	}

	if (pSelected == NULL)
		return false;

	SelectItemList(pSelected);
	return true;
}

bool CRealMyRoomMain::SelectItemList(FrListItem* pItem)
{
	switch (m_subTab)
	{
	case 4:
	case 21:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		g_audio->PlaySfx("item_select_cloth");

		if ((pInfo->tid & 0xfc000000) == 0x70000000)
		{
			unsigned long oldTypeId = m_pAuxParts[(pInfo->tid >> 21) & 0x1f];
			m_pAuxParts[(pInfo->tid >> 21) & 0x1f] = pInfo->tid;

			if (m_pExhibition)
				m_pExhibition->GetPetFrame()->SetAuxPart(
					(pInfo->tid >> 21) & 0x1f, pInfo->tid);
			else if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame())
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame()
					->SetAuxPart((pInfo->tid >> 21) & 0x1f, pInfo->tid);

			if (oldTypeId)
			{
				for (FrListBox::ITEM_LIST::iterator it =
						 m_pItemList->m_itemList.begin();
					it != m_pItemList->m_itemList.end(); ++it)
				{
					sItemInfo* pData = (sItemInfo*)(*it)->pData;
					if (pData == NULL)
						continue;

					if ((*it)->selected && pData->tid == oldTypeId)
						m_pItemList->UnselectItem(*it);
				}
			}

			ShowMainModel();
			return true;
		}
	}

	case 1:
	case 2:
	case 3:
	case 26:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
		if (pPart == NULL)
			return false;

		if (pInfo->ItemType == 6)
		{
			if (pInfo->Expired)
			{
				m_pItemList->ToggleItem(pItem);

				S5::FrRentalExtensionDlg* pDlg =
					CreateForm<S5::FrRentalExtensionDlg>(g_pFresh->GetManager(),
						this, "FrRentalExtensionDlg");
				pDlg->Initialize(pInfo);
				pDlg->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::
							   OnRentalExtensionDlgResult,
					3);
				return false;
			}
		}

		if (IsAngelWing(pInfo->tid) && !IsMannerPlayer())
		{
			if (pItem->selected)
				m_pItemList->ToggleItem(pItem);

			this << MsgObject(NULL, 0x23,
				(int)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xc0\xbd", 0, 0,
				0, 0);
			return false;
		}

		const char* pMsg = NULL;
		if (!pPart->c.IsUnderLvl && pPart->c.Level > Doc()->m_myInfo.stat.Level)
		{
			pMsg =
				"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. "
				"\xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.";
		}
		else if (pPart->c.IsUnderLvl == 1 &&
			pPart->c.Level < Doc()->m_myInfo.stat.Level)
		{
			pMsg =
				"\xc0\xcc\xb9\xcc \xc2\xf8\xbf\xeb\xb0\xa1\xb4\xc9\xc7\xd1 \xb7\xb9\xba\xa7\xc0\xbb "
				"\xb3\xd1\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xc7\xa5\xbd\xc3\xb5\xc8 "
				"\xb7\xb9\xba\xa7 \xc0\xcc\xc7\xcf\xb8\xb8 \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 "
				"\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		}

		if (pMsg)
		{
			if (pItem->selected)
				m_pItemList->ToggleItem(pItem);

			this << MsgObject(NULL, 0x23, (int)pMsg, 0, 0, 0, 0);
			return false;
		}

		CPartTidList* pTids = &Doc()->GetMyPartTidList();

		if (!pTids->CanSetPart(pInfo->tid))
		{
			m_pItemList->UnselectItem(pItem);

			if (pPart->EquippableWith[0])
			{
				char buf[256];
				sprintf(buf,
					"\xba\xbb \xbb\xf3\xc7\xb0\xc0\xba \xc7\xf6\xc0\xe7 \xc0\xc7\xbb\xf3\xbf\xa1\xb4\xc2 "
					"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				this << MsgObject(NULL, 0x23, (int)buf, 0, 0, 0, 0);
			}
			else
			{
				this << MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xbb\xf3\xc7\xb0\xc0\xbb \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 "
						 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
			}
			return false;
		}

		g_audio->PlaySfx("item_select_cloth");

		unsigned long overlapped = pTids->GetOverlapped(pInfo->tid);
		if (overlapped)
		{
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				sItemInfo* pData = (sItemInfo*)(*it)->pData;
				if (pData == NULL)
					continue;

				if ((*it)->selected && *it != pItem &&
					pTids->IsEquipped(pData->tid, pData->guid))
				{
					if (pTids->GetOverlapped(pData->tid) & overlapped)
					{
						m_pItemList->UnselectItem(*it);
						UnselectItemList(*it);
					}
				}
			}
		}

		CPartTidList tids = Doc()->GetMyPartTidList();

		unsigned long result = 0;
		if (m_pExhibition && m_pExhibition->GetPetFrame())
			result = m_pExhibition->GetPetFrame()->SetPart(pTids, pInfo->tid,
				pInfo->guid);

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetPetFrame())
			CRealMyRoom_AvatarManager::Instance()
				->m_pMyAvatar->GetPetFrame()
				->SetPart(&tids, pInfo->tid, pInfo->guid);

		if (result)
		{
			sCharacterInfo& info =
				Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
			for (int i = 0; i < pTids->m_partNum; i++)
			{
				info.tidParts[i] = pTids->m_tid[i];
				info.ItemIdList[i] = pTids->m_uid[i];
			}
		}
		else
		{
			this << MsgObject(NULL, 0x23,
				(int)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
		}

		ShowMainModel();

		m_pCharParts = pTids->m_tid;
		memcpy(m_partsUid, pTids->m_uid, sizeof(m_partsUid));
		return true;
	}

	case 8:
	case 9:
	case 23:
	case 24:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		SafeLoadPet(&((sItemInfo*)pItem->pData)->tid, false);
		g_audio->PlaySfx("ui_desktop_icon_click");
		return true;

	case 13:
	case 14:
	case 15:
	case 16:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		eEquipSkin index;
		if (S5::UTIL::GetEquipSkinIndex(index, pInfo->tid))
		{
			m_skinTypeId[index] = pInfo->tid;
			g_audio->PlaySfx("ui_desktop_icon_click");
		}

		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pInfo->tid);
		if (pSkin == NULL)
			return true;

		if ((pInfo->tid & 0x3c00000) == 0 && m_skinTypeId[0])
		{
			m_skinScrollDir[0] = pSkin->HScroll;
			m_skinScrollDir[1] = pSkin->VScroll;
			m_skinScroll[0] = 0.0f;
			m_skinScroll[1] = 0.0f;
		}

		Doc()->m_myInfo.userEquip.tidSkin[index] = pInfo->tid;
		return true;
	}

	case 34:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		Title* pTitle =
			g_TitleManager->GetTitle((eTitle)(pInfo->tid & 0x3fffff));
		if (pTitle)
		{
			if (!pTitle->CanEquip())
			{
				if (pItem->selected)
				{
					m_pItemList->ToggleItem(pItem);
					m_pItemList->SelectItem(m_pItemList->GetPreviousSelected(),
						true);
				}

				this << MsgObject(NULL, 0x23,
					(int)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xc1\xb6\xb0\xc7\xc0\xbb "
						 "\xb0\xae\xc3\xdf\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
				return false;
			}
		}

		eEquipSkin index;
		if (S5::UTIL::GetEquipSkinIndex(index, pInfo->tid))
		{
			m_skinTypeId[index] = pInfo->tid;
			g_audio->PlaySfx("ui_desktop_icon_click");
		}

		if (ItemManager()->FindSkin(pInfo->tid) == NULL)
			return true;

		Doc()->m_myInfo.userEquip.tidSkin[index] = pInfo->tid;
		return true;
	}

	case 33:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		if (S5::ISCutinItem(pInfo->tid))
			g_audio->PlaySfx("ui_desktop_icon_click");

		return true;
	}

	case 6:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		int result = ChangeClubset(pInfo->guid);
		if (result == 0)
		{
			this << MsgObject(NULL, 0x23,
				(int)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. "
					 "\xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
		}
		else if (result == 2)
		{
			this << MsgObject(NULL, 0x23,
				(int)"\xc2\xf8\xbf\xeb \xb7\xb9\xba\xa7\xc0\xbb \xc3\xca\xb0\xfa\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. "
					 "\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}

		g_audio->PlaySfx("item_select_club");
		return true;
	}

	case 7:
		ChangeAztec(((sItemInfo*)pItem->pData)->tid);
		g_audio->PlaySfx("item_select_aztec");
		return true;

	case 10:
	{
		sCaddieInfo* pInfo = (sCaddieInfo*)pItem->pData;

		if (!ChangeCaddie(pInfo->guid))
		{
			this << MsgObject(NULL, 0x23,
				(int)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. "
					 "\xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
		}

		g_audio->PlaySfx("ui_desktop_icon_click");
		return true;
	}

	case 12:
	{
		sMascotInfo* pInfo = (sMascotInfo*)pItem->pData;

		ChangeMascot(pInfo->guid);
		g_audio->PlaySfx("ui_desktop_icon_click");
		return true;
	}

	case 22:
		return true;

	case 18:
	case 19:
		SafeLoadPet(&((sItemInfo*)pItem->pData)->tid, false);

	case 27:
		g_audio->PlaySfx("ui_desktop_icon_click");
		return true;

	case 17:
	{
		if (IsLocalContent((localContentType_t)0x60))
			return true;

		sGiftInfo* pGift = (sGiftInfo*)pItem->pData;
		char szDate[32] = "";
		char szTime[32] = "";

		sscanf(pGift->sDate, "%s %s", szDate, szTime);

		char* p = strchr(szTime, '.');
		if (p)
			*p = 0;

		SetGiftDescString(pGift->sFromID, pGift->sMsg, szDate, szTime);
	}
	break;
	}

	g_audio->PlaySfx("ui_desktop_icon_click");
	return true;
}

bool CRealMyRoomMain::UnselectItemFromItemList(unsigned long typeId)
{
	if (m_pItemList == NULL)
		return false;

	if ((typeId & 0xfc000000) != 0x8000000)
		return false;

	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;

		if (pInfo == NULL)
			continue;

		if (pInfo->tid == typeId)
		{
			m_pItemList->UnselectItem(*it);
			return true;
		}
	}

	return false;
}

bool CRealMyRoomMain::UnselectItemList(unsigned long typeId)
{
	if (m_pItemList == NULL)
		return false;

	FrListItem* pItem = NULL;
	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
		if (pInfo == NULL)
			continue;

		if (pInfo->tid == typeId)
		{
			pItem = *it;
			break;
		}
	}

	if (pItem == NULL)
		return false;

	UnselectItemList(pItem);
	return true;
}

void CRealMyRoomMain::UnselectItemList(FrListItem* pItem)
{
	switch (m_subTab)
	{
	case 4:
	case 21:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		if ((pInfo->tid & 0xfc000000) == 0x70000000)
		{
			m_pAuxParts[(pInfo->tid >> 21) & 0x1f] = 0;
			if (m_pExhibition)
				m_pExhibition->GetPetFrame()->SetAuxPart(
					(pInfo->tid >> 21) & 0x1f, 0);
			else if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame())
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame()
					->SetAuxPart((pInfo->tid >> 21) & 0x1f, 0);

			ShowMainModel();
			return;
		}
	}

	case 1:
	case 2:
	case 3:
	case 26:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		CPartTidList* pTids = &Doc()->GetMyPartTidList();

		CPartTidList tids = Doc()->GetMyPartTidList();

		unsigned long oldTids[24];
		memcpy(oldTids, pTids->m_tid, sizeof(oldTids));

		if (pTids->IsEquipped(pInfo->tid, pInfo->guid))
		{
			unsigned long result = 0;
			if (m_pExhibition && m_pExhibition->GetPetFrame())
				result = m_pExhibition->GetPetFrame()->SetPart(pTids,
					pInfo->tid, pInfo->guid);

			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
				CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->GetPetFrame())
				result = CRealMyRoom_AvatarManager::Instance()
							 ->m_pMyAvatar->GetPetFrame()
							 ->SetPart(&tids, pInfo->tid, pInfo->guid);

			if (result)
			{
				sCharacterInfo& info =
					Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
				for (int i = 0; i < pTids->m_partNum; i++)
				{
					info.tidParts[i] = pTids->m_tid[i];
					info.ItemIdList[i] = pTids->m_uid[i];
				}

				m_pCharParts = pTids->m_tid;
			}
		}
		else
		{
			m_pItemList->SelectItem(pItem, true);
		}

		for (int i = 0; i < 24; i++)
		{
			if (oldTids[i] && pInfo->tid != oldTids[i] &&
				pTids->m_tid[i] != oldTids[i] && oldTids[i] != 2 &&
				oldTids[i] != 3)
			{
				UnselectItemFromItemList(oldTids[i]);
			}
		}

		if (IsLocalContent(S4_UCC) && IsUccClothes(pInfo->tid))
		{
			unsigned char part = (pInfo->tid >> 13) & 0x1f;
			unsigned long charTid =
				Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar].tid;
			unsigned long tid = (((charTid << 5) | part) << 13) | 0x8000400;
			if (m_pExhibition && m_pExhibition->GetPetFrame())
				m_pExhibition->GetPetFrame()->SetPart(pTids, tid, 0);

			memcpy(m_partsUid, pTids->m_uid, sizeof(m_partsUid));
		}

		ShowMainModel();
	}
	break;

	case 12:
		ChangeMascot(0);
		break;

	case 10:
		ChangeCaddie(0);
		TurnOffItemStatus();
		break;

	case 13:
	case 14:
	case 15:
	case 16:
	case 34:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		eEquipSkin index;
		if (S5::UTIL::GetEquipSkinIndex(index, pInfo->tid))
		{
			m_skinTypeId[index] = 0;
			Doc()->m_myInfo.userEquip.tidSkin[index] = 0;
		}
	}
	break;

	case 22:
	case 33:
		break;

	default:
		m_pItemList->SelectItem(pItem, true);
		break;
	}
}

int CRealMyRoomMain::GetItemNumInMyPocket(unsigned long typeId)
{
	int count = 0;

	if (typeId == 0 || (typeId & 0xfc000000) == 0x4000000)
	{
		for (std::map<unsigned int, sCharacterInfo>::iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).second.tid) == NULL)
				continue;

			if (typeId == 0 || (*it).second.tid == typeId)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x8000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).tid) == NULL)
				continue;

			if (typeId == 0 || (*it).tid == typeId)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x18000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).tid) == NULL)
				continue;

			if (typeId == 0 || (*it).tid == typeId)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x1c000000)
	{
		for (std::map<unsigned int, sCaddieInfo>::iterator it =
				 Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).second.tid) == NULL)
				continue;

			if (typeId == 0 || (*it).second.tid == typeId)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x20000000)
	{
		count = 0;
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x3c000000)
	{
		for (std::map<unsigned int, sCharacterInfo>::iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			IFF_STRUCT::sHairStyle* pHair =
				ItemManager()->FindHairStyle(typeId);
			if (pHair == NULL)
				continue;

			if (pHair->cHairID == (*it).second.hairClr &&
				(pHair->cCharID | 0x4000000) == (*it).second.tid)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x10000000)
	{
		for (std::map<unsigned int, sItemInfo>::iterator it =
				 Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).second.tid) == NULL)
				continue;

			if (typeId == 0 || (*it).second.tid == typeId)
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x14000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).tid) == NULL)
				continue;

			if (typeId == 0 || (*it).tid == typeId)
				count++;
		}
	}

	unsigned long type = typeId >> 26;
	if (type == 9 && (typeId & 0x3e00000) == 0x200000)
	{
		IFF_STRUCT::sSetItem* pSet = ItemManager()->FindSetItem(typeId);

		for (unsigned char i = 0; i < pSet->nElems; i++)
		{
			if (typeId && (pSet->ElemIds[i] & 0xfc000000) == 0x4000000)
				continue;

			for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
				it != Doc()->m_partsList.end(); ++it)
			{
				if (ItemManager()->FindCommonItem((*it).tid) == NULL)
					continue;

				if (typeId == 0 || (*it).tid == pSet->ElemIds[i])
					count++;
			}
		}
	}

	if (typeId == 0 || type == 14)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			if (ItemManager()->FindCommonItem((*it).tid) == NULL)
				continue;

			if (typeId == 0 || (*it).tid == typeId)
				count++;
		}
	}

	return count;
}

void CRealMyRoomMain::Net_EquipCharacter()
{
	if (m_userEquip.guidChar != Doc()->m_myInfo.userEquip.guidChar)
	{
		WSendPacket packet((enumClientPacket)0x20);
		packet.Encode1(5);
		packet.Encode4(Doc()->m_myInfo.userEquip.guidChar);
		packet.Send(TO_GAME);
	}
}

void CRealMyRoomMain::Net_EquipDresserParts()
{
	std::map<unsigned int, sCharacterInfo>::const_iterator it =
		m_charInfoMap.begin();
	std::map<unsigned int, sCharacterInfo>::const_iterator itDoc =
		Doc()->m_charMap.begin();
	for (; it != m_charInfoMap.end() && itDoc != Doc()->m_charMap.end();
		++it, ++itDoc)
	{
		const sCharacterInfo* pInfo = &(*it).second;
		const sCharacterInfo* pDocInfo = &(*itDoc).second;

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pInfo->tid);
		if (pChar == NULL)
			continue;

		bool bChanged = false;
		int i;
		int partNum;
		unsigned char nParts;
		int nAcsries;

		for (i = 0, nAcsries = pChar->nAcsries, nParts = pChar->nParts,
			partNum = nAcsries + (int)nParts;
			i < partNum; i++)
		{
			if (pInfo->tidParts[i] != pDocInfo->tidParts[i] ||
				pInfo->ItemIdList[i] != pDocInfo->ItemIdList[i])
			{
				bChanged = true;
				break;
			}
		}

		for (i = 0; i < 5; i++)
		{
			if (pInfo->tidAuxParts[i] != pDocInfo->tidAuxParts[i])
			{
				bChanged = true;
				break;
			}
		}

		if (!bChanged)
			continue;

		sCharacterInfo info = *pDocInfo;

		for (i = 0; i < partNum; i++)
		{
			info.tidParts[i] = pDocInfo->tidParts[i];
			info.ItemIdList[i] = pDocInfo->ItemIdList[i];
		}

		for (i = nParts + nAcsries; i < 24; i++)
		{
			info.tidParts[i] = 0;
			info.ItemIdList[i] = 0;
		}

		memcpy(info.tidAuxParts, pDocInfo->tidAuxParts,
			sizeof(info.tidAuxParts));

		WSendPacket packet(0x20);
		packet.Encode1(0);
		packet.EncodeBuffer(&info, sizeof(info));
		packet.Send(TO_GAME);
	}
}

void CRealMyRoomMain::Net_EquipCaddie()
{
	if (m_userEquip.guidCaddie == Doc()->m_myInfo.userEquip.guidCaddie)
		return;

	if (Doc()->m_myInfo.userEquip.guidCaddie)
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.find(Doc()->m_myInfo.userEquip.guidCaddie);
		if (it == Doc()->m_caddieMap.end())
			return;
	}

	WSendPacket packet((enumClientPacket)0x20);
	packet.Encode1(1);
	packet.Encode4(Doc()->m_myInfo.userEquip.guidCaddie);
	packet.Send(TO_GAME);
}

void CRealMyRoomMain::Net_EquipItem()
{
	int i;
	for (i = 0; i < 10; i++)
	{
		if (m_userEquip.tidItemSlot[i] !=
			Doc()->m_myInfo.userEquip.tidItemSlot[i])
			break;
	}

	if (i == 10)
		return;

	WSendPacket packet(0x20);
	packet.Encode1(2);
	packet.EncodeBuffer(Doc()->m_myInfo.userEquip.tidItemSlot,
		sizeof(unsigned long) * 10);
	packet.Send(TO_GAME);
}

void CRealMyRoomMain::Net_EquipGear()
{
	if (m_userEquip.tidBall != Doc()->m_myInfo.userEquip.tidBall ||
		m_userEquip.guidClubSet != Doc()->m_myInfo.userEquip.guidClubSet)
	{
		WSendPacket packet(0x20);
		packet.Encode1(3);
		packet.Encode4(Doc()->m_myInfo.userEquip.tidBall);
		packet.Encode4(Doc()->m_myInfo.userEquip.guidClubSet);
		packet.Send(TO_GAME);
	}
}

void CRealMyRoomMain::Net_EquipSkin()
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (m_userEquip.tidSkin[i] != Doc()->m_myInfo.userEquip.tidSkin[i])
			break;
	}

	if (i == 6)
		return;

	for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
		it != Doc()->m_cutinList.end(); ++it)
	{
		eEquipSkin index;
		if (!S5::UTIL::GetEquipSkinIndex(index, (*it).tid))
			continue;

		if ((*it).ItemType == 1 &&
			(*it).tid == Doc()->m_myInfo.userEquip.tidSkin[index])
			(*it).ItemType = 2;
	}

	WSendPacket packet(0x20);
	packet.Encode1(4);
	packet.EncodeBuffer(Doc()->m_myInfo.userEquip.tidSkin,
		sizeof(unsigned long) * 6);
	packet.Send(TO_GAME);
}

void CRealMyRoomMain::Net_StartPeriodItem(unsigned long uid)
{
	WSendPacket packet(0x20);
	packet.Encode1(7);
	packet.Encode4(uid);
	packet.Send(TO_GAME);
}

void CRealMyRoomMain::Net_EquipMascot()
{
	sMascotInfo* pMascot = Doc()->GetMyEquippedMascotInfo();

	if (pMascot)
	{
		if (ItemManager()->CompareSystemTime(pMascot->endDate,
				Doc()->GetServerTime()) <= 0 &&
			!pMascot->PCBangMascot)
			Doc()->m_myInfo.userEquip.guidMascot = 0;
	}

	if (m_userEquip.guidMascot == Doc()->m_myInfo.userEquip.guidMascot)
		return;

	if (Doc()->m_myInfo.userEquip.guidMascot)
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.find(Doc()->m_myInfo.userEquip.guidMascot);
		if (it == Doc()->m_mascotMap.end())
			return;
	}

	WSendPacket packet((enumClientPacket)0x20);
	packet.Encode1(8);
	packet.Encode4(Doc()->m_myInfo.userEquip.guidMascot);
	packet.Send(TO_GAME);
}

bool CRealMyRoomMain::OnEnchantDlgResult(int result, FrForm* form)
{
	m_pEnchantDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnPangGiftDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		sGiftInfo* pInfo = ((FrPangGiftDlg*)form)->GetItemInfo();

		WSendPacket packet((enumClientPacket)0x24);
		packet.Encode4(pInfo->guid);
		packet.Encode4(pInfo->tid);
		packet.Send(TO_GAME);

		Sleep(50);
	}

	m_pPangGiftDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnReturnGiftDlgResult(int result, FrForm* form)
{
	if (result != 1)
		return true;
	FrReturnGiftDlg* pDlg = DYNAMIC_CAST(FrReturnGiftDlg, form);
	if (pDlg == NULL)
		return true;
	{
		WSendPacket packet((enumClientPacket)0x4d);
		packet.Encode4(pDlg->m_giftIdx);
		packet.Encode4(pDlg->m_typeId);
		packet.Send(TO_GAME);
	}

	return true;
}

bool CRealMyRoomMain::OnMoveGiftDlgResult(int result, FrForm* form)
{
	if (result == 1 && m_pMoveGift)
	{
		m_bGiftMoveReady = false;

		HandleMsg(MsgObject(NULL, 0x24,
			(int)"\xbc\xb1\xb9\xb0 \xbf\xc5\xb1\xe2\xb1\xe2\xb8\xa6 \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

		WSendPacket packet((enumClientPacket)0x24);
		packet.Encode4(m_pMoveGift->guid);
		packet.Encode4(m_pMoveGift->tid);
		packet.Send(TO_GAME);
	}

	return true;
}

bool CRealMyRoomMain::OnMascotInfoDlgResult(int result, FrForm* form)
{
	m_pMascotInfoDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnUpgradeDlgResult(int result, FrForm* form)
{
	m_pDowngradeDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnNoteDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		WSendPacket packet((enumClientPacket)0x3c);
		packet.Encode2(0x111);

		packet.Encode4(0xffffffff);
		packet.EncodeStr(std::string(m_pNoteDlg->GetNote()));
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}

	m_pNoteDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnFunritureSaveMsgdlgResult(int result, FrForm* form)
{
	SetCameraDisplayType(0);
	return true;
}

bool CRealMyRoomMain::OnOfflineShopEditMsgDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		m_offlineShopEdit = 1;

		WSendPacket packet((enumClientPacket)0xc4);
		packet.Encode4(Doc()->m_replayTargetUID);
		packet.Send(TO_GAME);
	}

	return true;
}

bool CRealMyRoomMain::OnFurnitrueListDeleteDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrListItem* pItem = m_pFurnitureList->GetItemUnderCursor();
		if (pItem == NULL)
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xbb\xe8\xc1\xa6\xc7\xd2 \xb0\xa1\xb1\xb8\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);

			pForm->Open(NULL, 3);
			return true;
		}

		sFurniture_List* pFurniture = (sFurniture_List*)pItem->pData;
		pItem->selected = false;
		if (pFurniture)
		{
			m_pFurnitureList->UnselectItem(pItem);

			for (std::list<sFurniture_List>::iterator it =
					 Doc()->m_furnitureList.begin();
				it != Doc()->m_furnitureList.end(); ++it)
			{
				if ((*it).id == pFurniture->id)
					(*it).bArrange = false;
			}

			CRealMyRoomData::Instance()->ListFurnitureSelect(pFurniture);
			CRealMyRoomData::Instance()->SetState(6);
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xbb\xe8\xc1\xa6\xc7\xd2 \xb0\xa1\xb1\xb8\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
	}

	return true;
}

bool CRealMyRoomMain::OnCaddieInfoDlgResult(int result, FrForm* form)
{
	if (result == 2)
	{
		if (m_p398)
		{
			m_p398->Enable(true);

			if (form)
			{
				sCaddieInfo info = ((FrCaddieInfoDlg*)form)->GetCaddieInfo();

				CaddieIterator end = ItemManager()->m_CaddieMap.end();
				for (CaddieIterator it = ItemManager()->m_CaddieMap.begin();
					it != end; ++it)
				{
					if ((*it).second.c.TypeId == info.tid)
					{
						if ((*it).second.c.Level > Doc()->m_myInfo.stat.Level)
							m_p398->Enable(false);
						break;
					}
				}
			}
		}
	}

	return true;
}

bool CRealMyRoomMain::OnOpenTikiReport(int result, FrForm* form)
{
	if (!IsLocalContent(S3_CADDIE_REPORT))
		return false;

	if (result == 1)
	{
		if (m_tikiReportUid && m_tikiReportState)
		{
			WSendPacket packet((enumClientPacket)0xa3);
			packet.Encode4(m_tikiReportUid);
			packet.Encode4(m_tikiReportState);
			packet.Send(TO_GAME);

			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
	}
	else
	{
		m_tikiReportUid = 0;
	}

	m_tikiReportState = 0;
	return true;
}

bool CRealMyRoomMain::OnOpenNewYearMoneyDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_newYearMoneyUid)
		{
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

			WSendPacket packet((enumClientPacket)0x90);
			packet.Encode4(m_newYearMoneyUid);
			packet.Send(TO_GAME);
		}
	}

	m_newYearMoneyUid = 0;
	return true;
}

bool CRealMyRoomMain::OnOpenEventGiftDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_eventGiftUid)
		{
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

			WSendPacket packet((enumClientPacket)0x91);
			packet.Encode4(m_eventGiftUid);
			packet.Send(TO_GAME);
		}
	}

	m_eventGiftUid = 0;
	return true;
}

bool CRealMyRoomMain::OnOpenS4EventGiftDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_boxTypeId)
		{
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

			WSendPacket packet((enumClientPacket)0xaa);
			packet.Encode4(m_boxTypeId);
			packet.Send(TO_GAME);
		}
	}

	m_boxTypeId = 0;
	return true;
}

void CRealMyRoomMain::OpenCaddieInfo(const sCaddieInfo& info)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (!pWnd)
	{
		FrCaddieInfoDlg* pDlg = DYNAMIC_CAST(FrCaddieInfoDlg, pWnd);

		pDlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this,
			"caddie_info");
		if (pDlg)
		{
			pDlg->SetCaddieInfo(info);
			pDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnCaddieInfoDlgResult, 3);

			if (m_p398)
				m_p398->Enable(false);
		}
	}
}

void CRealMyRoomMain::OpenInformation(FrListItem* pItem)
{
	if (pItem)
	{
		m_infoOffsetX = pItem->pos.x - 300.0f;
		m_infoOffsetY = pItem->pos.y - 450.0f;
	}

	switch (m_subTab)
	{
	case 17:
		if (!IsLocalContent((localContentType_t)0x60))
		{
			sGiftInfo* pGift = (sGiftInfo*)pItem->pData;

			OpenInformation(&pGift->tid, pGift, NULL);
		}
		break;

	default:
	{
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		OpenInformation(&pInfo->tid, NULL, &pInfo->guid);
	}
	break;

	case 10:
		OpenCaddieInfo(*(sCaddieInfo*)pItem->pData);
		break;

	case 12:
	{
		sMascotInfo* pInfo = (sMascotInfo*)pItem->pData;
		m_pMascotInfoDlg = CreateForm<FrMascotInfoDlg>(g_pFresh->GetManager(),
			this, "mascot_info");

		m_pMascotInfoDlg->SetMascotInfo(*pInfo,
			m_pExhibition ? m_pExhibition->GetModel() : NULL);
		m_pMascotInfoDlg->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnMascotInfoDlgResult, 1);
	}
	break;
	}
}

void CRealMyRoomMain::OpenInformation(unsigned long* pTypeId, sGiftInfo* pGift,
	unsigned long* pUid)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (pWnd)
		return;

	FrForm* pInfo = DYNAMIC_CAST(FrForm, pWnd);

	pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
		"information_setitem");
	if (pWnd)
		return;

	FrSetItemDlg* pSetItemDlg = DYNAMIC_CAST(FrSetItemDlg, pWnd);

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(*pTypeId);
	unsigned long uid = pUid ? *pUid : 0;

	if (!pItem)
		return;

	if ((pItem->TypeId & 0xfc000000) == 0x24000000)
	{
		pSetItemDlg = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(), this,
			"information_setitem");
		if (!pSetItemDlg)
			return;

		pSetItemDlg->SetSetItem(pItem->TypeId);
		pSetItemDlg->Open(NULL, 3);
	}
	else
	{
		pInfo = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information");
		if (!pInfo)
			return;

		FrArea* pPortrait =
			DYNAMIC_CAST(FrArea, pInfo->FindChildByName("portrait"));
		if (pPortrait)
		{
			if (IsLocalContent(S4_UCC) &&
				IsLocalContent((localContentType_t)0x5f) && uid &&
				IsUccClothes(*pTypeId))
			{
				const Bitmap* pIcon = GetUccItemIcon(uid);
				if (pIcon)
					pPortrait->SetBgImg(pIcon);
			}
			else
			{
				pPortrait->SetBgImg(pItem->Icon);
			}
		}

		FrEdit* pName = DYNAMIC_CAST(FrEdit, pInfo->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pInfo->FindChildByName("level"));

		bool bNewLine = false;
		if (pName && pLevel)
		{
			char szName[128] = {
				0,
			};

			if (IsLocalContent(S4_UCC) &&
				IsLocalContent((localContentType_t)0x5f) && uid &&
				IsUccClothes(*pTypeId))
			{
				strncpy(szName, GetUccItemName(uid), sizeof(szName));
			}
			else
			{
				strncpy(szName, pItem->Name, sizeof(szName));
			}

			if (IsAngelWing(pItem->TypeId) && !IsMannerPlayer())
			{
				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s \\c0xffff0000\\c(\xc2\xf8\xbf\xeb\xba\xd2\xb0\xa1)\\c0xff000000\\c\n\n",
						szName),
					false, true);
			}
			else
			{
				pName->AddText(MakeStr("\xc0\xcc\xb8\xa7: %s\n\n", szName),
					false, true);
			}

			if (pItem->IsUnderLvl)
				pName->AddText(
					"\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf\n",
					false, true);
			else
				pName->AddText(
					"\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3\n",
					false, true);

			pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));

			unsigned char type = 0;
			int day = 0;
			int hour = 0;
			bool bExpired = false;

			if (pGift)
			{
				if (pItem->TimeFlag)
				{
					type = 1;
					day = pGift->Arg0;
					hour = 0;
				}
			}
			else
			{
				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_cutinList.begin();
					it != Doc()->m_cutinList.end(); ++it)
				{
					if (&(*it).tid == pTypeId)
						break;
				}

				if (it == Doc()->m_cutinList.end())
				{
					for (it = Doc()->m_myItemList.begin();
						it != Doc()->m_myItemList.end(); ++it)
					{
						if (&(*it).tid == pTypeId)
							break;
					}
				}

				if (it != Doc()->m_myItemList.end())
				{
					type = (*it).ItemType;
					day = (*it).HourRemain / 24;
					hour = (*it).HourRemain % 24;
					bExpired = (*it).Expired;
				}
				else
				{
					for (it = Doc()->m_partsList.begin();
						it != Doc()->m_partsList.end(); ++it)
					{
						if (&(*it).tid == pTypeId)
							break;
					}

					if (it != Doc()->m_partsList.end())
					{
						type = (*it).ItemType;
						day = (*it).HourRemain / 24;
						hour = (*it).HourRemain % 24;
						bExpired = (*it).Expired;
					}
				}
			}

			if (type > 0)
			{
				bNewLine = true;
				pName->AddText("\n", false, true);
			}

			switch (type)
			{
			case 1:
				pName->AddText(
					MakeStr(
						"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3",
						day, hour),
					false, true);
				break;

			case 2:
			case 3:
			case 4:
			case 5:
				if (bExpired)
				{
					pName->AddText(
						"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3\\c0xff000000\\c\xc0\xcc \\c0xffff0000\\c\xb8\xb8\xb7\xe1\\c0xff000000\\c\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n",
						false, true);
				}
				else if (IsLocalContent(S3_CADDIE_REPORT) &&
					pItem->TypeId == 0x1a000042)
				{
					pName->AddText(
						MakeStr(
							"\xbf\xad\xb6\xf7 \xb0\xa1\xb4\xc9 \xb1\xe2\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3\n",
							day, hour),
						false, true);
				}
				else if (day == 0 && hour == 0)
				{
					pName->AddText(
						"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: 1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8\n",
						false, true);
				}
				else
				{
					pName->AddText(
						MakeStr(
							"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3\n",
							day, hour),
						false, true);
				}
				break;
			}
		}

		if (IsLocalContent((localContentType_t)0x51))
		{
			char szDesc[512] = {
				0,
			};

			FrEdit* pAbility =
				DYNAMIC_CAST(FrEdit, pInfo->FindChildByName("avility"));
			FrStatic* pStaticAbility = DYNAMIC_CAST(FrStatic,
				pInfo->FindChildByName("static_avility"));

			if (pAbility)
			{
				if (bNewLine == true)
				{
					pAbility->AddText("\n\n", false, true);

					WRect rect;
					rect = pStaticAbility->GetRect();
					rect.y = rect.y + 25.0f;
					pStaticAbility->SetRect(rect);
				}
				else
				{
					WRect rect;
					rect = pStaticAbility->GetRect();
					rect.y = rect.y - 3.0f;
					pStaticAbility->SetRect(rect);
				}

				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(*pTypeId);
				if (pPart)
				{
					for (int i = 0; i < 5; i++)
					{
						if (pPart->Attr[i] || pPart->Slot[i])
						{
							pAbility->AddText(
								MakeStr("\\c0xff0000ff\\c%s\\c0xff000000\\c",
									MakeStatusString(i, pPart->Attr[i],
										pPart->Slot[i])),
								false, true);
						}
					}
				}

				for (std::list<sSCardAvilityPeriodInfo>::iterator it =
						 Doc()->m_cardAbilityList[0].begin();
					it != Doc()->m_cardAbilityList[0].end(); ++it)
				{
					if ((*it).partsTid == *pTypeId && (*it).partsUid == uid)
					{
						if ((*it).cardType == 0)
						{
							IFF_STRUCT::sCard* pCard =
								ItemManager()->FindCard((*it).tid);
							if (pCard)
							{
								for (int i = 0; i < 5; i++)
								{
									if (pCard->COM[i] > 0)
									{
										pAbility->AddText(
											MakeStr(
												"\\c0xff60ee0e\\c%s\\c0xff000000\\c",
												MakeCardStatusString(i,
													pCard->COM[i])),
											false, true);
									}
								}
							}
						}
						else if ((*it).cardType == 1)
						{
							IFF_STRUCT::sDesc* pDesc =
								ItemManager()->FindDesc((*it).tid);
							if (pDesc)
							{
								strcpy(szDesc, pDesc->Desc);

								for (int i = 0; i < 512; i++)
								{
									if (szDesc[i] == '\n')
									{
										szDesc[i] = 0;
										break;
									}
								}

								pAbility->AddText(
									MakeStr(
										"\\c0xffff0000\\c%s\\c0xff000000\\c",
										szDesc),
									false, true);
							}
						}
					}
				}
			}
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->TypeId);
		if (pDesc)
			pInfo->SetMessage(pDesc->Desc, false);

		pInfo->Open(NULL, 3);
	}
}

void CRealMyRoomMain::EquipItem(sItemInfo* pItem)
{
	int slots = 10;
	if (IsLocalContent(S3_MASCOT))
		slots = Doc()->GetNumAddItemSlotByMascot() + 8;

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(0xff);
		CCardManager::Instance()->CalcCardPeriodAndStatus();
		int cards = CCardManager::Instance()->GetCardPeriodSlot();
		if (cards > 0)
			slots += cards;
		else if (Doc()->GetNumAddItemSlotByMascot() == 1)
			Doc()->m_myInfo.userEquip.tidItemSlot[9] = 0;
		else
		{
			Doc()->m_myInfo.userEquip.tidItemSlot[8] = 0;
			Doc()->m_myInfo.userEquip.tidItemSlot[9] = 0;
		}
	}

	int i;
	for (i = 0; i < slots; ++i)
		if (Doc()->m_myInfo.userEquip.tidItemSlot[i] == 0)
			break;

	if (i == slots)
	{
		this << MsgObject(NULL, 0x23,
			(int)"\xb4\xf5 \xc0\xcc\xbb\xf3 \xc0\xe5\xc2\xf8\xc7\xd2 \xbc\xf6 "
				 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
			0, 0, 0, 0);
	}
	else
	{
		Doc()->m_myInfo.userEquip.tidItemSlot[i] = pItem->tid;

		int count = 0;
		for (int j = 0; j < 10; ++j)
		{
			if (pItem->tid == Doc()->m_myInfo.userEquip.tidItemSlot[j])
				count++;
		}

		if (pItem->Common[0] - count <= 0)
		{
			m_anotherHand.WaitUntilWorkDone(9999);
			if (m_pExhibition)
			{
				delete m_pExhibition;
				m_pExhibition = NULL;
			}

			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				sItemInfo* info = (sItemInfo*)(*it)->pData;
				if (info == NULL)
					continue;

				if (pItem->tid == info->tid)
				{
					m_pItemList->_DelItem(it);
					break;
				}
			}

			m_pItemList->SortItem(EmptyCompares);
		}
	}
}

void CRealMyRoomMain::UnequipItem(unsigned long* pTypeId)
{
	for (ITEM_LIST_IT it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); ++it)
	{
		if (it->tid == *pTypeId)
		{
			if (!IsInItemList(*pTypeId))
			{
				m_pItemList->AddItem(&*it);
				m_pItemList->SortItem(EmptyCompares);
			}
		}
	}

	*pTypeId = 0;
}

bool CRealMyRoomMain::IsInItemList(unsigned long typeId)
{
	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		FrListItem* item = *it;
		sItemInfo* info = (sItemInfo*)item->pData;
		if (info)
		{
			if (info->tid == typeId)
				return true;
		}
	}

	return false;
}

void CRealMyRoomMain::SafeLoadPet(unsigned long* pTypeId, bool bForce)
{
	m_anotherHand.WaitUntilWorkDone(9999);
	if (false == bForce)
	{
		if (m_pExhibition)
		{
			if (m_pExhibition->GetTypeId() == *pTypeId)
				return;
		}
	}

	m_anotherHand.Work(this, 0, pTypeId, true, -1);
}

void CRealMyRoomMain::TurnOnItemStatus()
{
	if (m_pStatPart)
		m_pStatPart->Enable(true);
	m_statTab = (statTab_t)0;
	SetLevelBar();
}

void CRealMyRoomMain::TurnOffItemStatus()
{
	if (m_pStatPart)
		m_pStatPart->Enable(false);
	m_statTab = (statTab_t)1;
	SetLevelBar();
}

void CRealMyRoomMain::AutoScroll()
{
	if (m_pItemList && m_pItemList->GetScrollBar())
		m_pItemList->GetScrollBar()->SetCurTopRow(0);
}

bool CRealMyRoomMain::OnFortuneOpenDlgResult(int result, FrForm* form)
{
	ITEM_LIST_IT it = Doc()->m_myItemList.end();

	if (m_pFortuneOpenDlg)
	{
		it = std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
			((FrFortuneOpenDlg*)m_pFortuneOpenDlg)->GetTypeId());
	}

	m_pFortuneOpenDlg = NULL;

	if (it != Doc()->m_myItemList.end())
	{
		--it->Common[0];
		if (it->Common[0] <= 0)
			Doc()->m_myItemList.erase(it);
	}

	HandleMsg(MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0));

	return true;
}

bool CRealMyRoomMain::OnFortuneDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);

		{
			WSendPacket packet((enumClientPacket)0x59);
			packet.Encode4(m_pFortuneDlg->GetTypeId());
			packet.Send(TO_GAME);
		}

		m_pFortuneDlg = NULL;

		return true;
	}

	m_pFortuneDlg = NULL;
	return true;
}

void CRealMyRoomMain::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.message)
	{
	case 0xe4:
	{
		sSlotInfo* pSlot = (sSlotInfo*)msg.param1;
		sCharacterInfo* pCharInfo = (sCharacterInfo*)msg.param2;

		CRealMyRoom_Avatar* pAvatar =
			CRealMyRoom_AvatarManager::Instance()->CreateAvatar(*pSlot,
				*pCharInfo, this);

		((CRealMyRoomTask*)GetTask())
			->AddAvatarBurden(
				CRealMyRoom_AvatarBurden(pSlot->dwGuid, *pCharInfo, NULL));

		if (pSlot->dwGuid == Doc()->m_myInfo.info.dwGuid)
		{
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar = pAvatar;
			Doc()->m_myInfo.userEquip.guidChar = pCharInfo->guid;
			Doc()->SendTradeCloseShop();

			WPolySoup::sPoint* pPoint = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
			if (pPoint == NULL)
				pPoint = GOLFDOC()->m_pPolySoup->FindPoint(
					MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));

			WVector pos = pPoint->pos;
			pos = GetPVS().GetGroundPoint(pPoint->pos, false, NULL, false);

			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetPosition(
				pos);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAngle(0.0f);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Stop();

			m_camLookAt =
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos;
			m_camLookAt.y += 4.6f;
			m_camLookAt.z -= 46.0f;

			if (Doc()->m_myInfo.info.dwUID == Doc()->m_replayTargetUID)
			{
				SetUIState(1);
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
				{
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetState(2);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(
						false);
				}
				SetCameraDisplayType(1);
			}
			else
			{
				SetUIState(0);
				SetCameraDisplayType(4);
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
				{
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->ChangeCharacterInfo();
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetState(0);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(
						true);
				}
			}
		}
		else
		{
			WPolySoup::sPoint* pPoint = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
			if (pPoint == NULL)
				pPoint = GOLFDOC()->m_pPolySoup->FindPoint(
					MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));

			WVector pos = pPoint->pos;
			pAvatar->SetPosition(pos);
			pAvatar->SetAngle(pSlot->location[2]);
		}
	}
	break;

	case 0xe5:
	{
		CRealMyRoom_Avatar* pAvatar =
			CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(msg.param1);
		if (pAvatar == NULL)
			return;

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			if (strcmp(pAvatar->GetIDName(),
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->GetIDName()) == 0)
			{
				Doc()->SendTradeCloseShop();
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SubState(
					0x7e);
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar = NULL;
			}

			if (IsLocalContent(S3_GM_TOOLKIT))
			{
				BRIEFUSERINFO_MAP_IT it;
				it = Doc()->m_briefUserInfoMap.find((int)pAvatar->m_guid);
				if (((sBriefUserInfo)(*it).second).dwIdentity & 4)
				{
					if (((sBriefUserInfo)(*it).second).state & 1)
					{
						std::string str(MakeStr(
							"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
							pAvatar->GetNickName()));
						CChatMsg::Instance()->AddChatMsg(str, 0xffff00, false,
							false);
					}
				}
				else
				{
					std::string str(MakeStr(
						"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						pAvatar->GetNickName()));
					CChatMsg::Instance()->AddChatMsg(str, 0xffff00, false,
						false);
				}
			}
			else
			{
				std::string str(MakeStr(
					"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					pAvatar->GetNickName()));
				CChatMsg::Instance()->AddChatMsg(str, 0xffff00, false, false);
			}
		}

		if (IsLocalContent(S3_GM_TOOLKIT))
		{
			BRIEFUSERINFO_MAP_IT it;
			it = Doc()->m_briefUserInfoMap.find((int)pAvatar->m_guid);
			if (((sBriefUserInfo)(*it).second).dwIdentity & 4)
			{
				if (((sBriefUserInfo)(*it).second).state & 1)
				{
					CFxSequence* pFx =
						CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pFx)
						pFx->Pos() = WVector(pAvatar->m_pos.x,
							pAvatar->m_pos.y + 5.0f, pAvatar->m_pos.z);
				}
			}
			else
			{
				CFxSequence* pFx =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pFx)
					pFx->Pos() = WVector(pAvatar->m_pos.x,
						pAvatar->m_pos.y + 5.0f, pAvatar->m_pos.z);
			}
		}
		else
		{
			CFxSequence* pFx =
				CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pFx)
				pFx->Pos() = WVector(pAvatar->m_pos.x, pAvatar->m_pos.y + 5.0f,
					pAvatar->m_pos.z);
		}

		((CRealMyRoomTask*)GetTask())
			->RemoveAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid,
				pAvatar->m_charInfo, pAvatar->m_pPetFrame));

		CRealMyRoom_AvatarManager::Instance()->DeleteAvatar(
			pAvatar->GetIDName());
	}
	break;

	case 0x5b:
	{
		CRealMyRoom_Avatar* pAvatar =
			CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(msg.param1);
		if (pAvatar == NULL ||
			pAvatar == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
			return;

		switch (msg.param2)
		{
		case 4:
		{
			WVector pos(*(float*)msg.param3, 0.0f, *(float*)msg.param4);
			pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
			pAvatar->SetPosition(pos);
			pAvatar->SetAngle(*(float*)msg.param5);

			if (IsLocalContent(S3_GM_TOOLKIT))
			{
				BRIEFUSERINFO_MAP_IT it;
				it = Doc()->m_briefUserInfoMap.find(msg.param1);
				if (((sBriefUserInfo)(*it).second).dwIdentity & 4)
				{
					if (((sBriefUserInfo)(*it).second).state & 1)
					{
						CFxSequence* pFx = CFx::Instance()->OpenSequence(
							"Talk_Room.seq", true);
						if (pFx)
							pFx->Pos() = WVector(pAvatar->m_pos.x,
								pAvatar->m_pos.y + 5.0f, pAvatar->m_pos.z);
					}
				}
			}
			else
			{
				CFxSequence* pFx =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pFx)
					pFx->Pos() = WVector(pos.x, pos.y + 5.0f, pos.z);
			}
		}
		break;

		case 5:
			pAvatar->SetAction(msg.param3);
			break;

		case 6:
		{
			WVector pos = pAvatar->GetTargetPosition();
			pos.x += *(float*)msg.param3;
			pos.z += *(float*)msg.param4;
			pAvatar->SetTargetPosition(pos);
			pAvatar->SetPendingTargetAngle(*(float*)msg.param5);
		}
		break;

		case 7:
			pAvatar->ChangeMotion(4, (const char*)msg.param3, 0.2f);
			break;

		case 8:
			pAvatar->SetState(msg.param3);
			break;

		case 9:
			return;
		}
	}
	break;

	case 0:
		if (g_pFresh)
			g_pFresh->OpenLayout((const char*)msg.param1, this, false);
		break;

	case 0x16:
		m_bMsgFlag = msg.param1 != 0;
		break;

	case 0x17:
		*(int*)msg.param1 = m_bMsgFlag;
		break;

	case 0xc4:
		if (msg.param1 < 7)
		{
			if (msg.param1 == 5)
				m_mainTab = (mainTab_t)6;
			else
				m_mainTab = (mainTab_t)msg.param1;
		}
		else
		{
			m_mainTab = (mainTab_t)0;
			m_subTab = (subTab_t)1;
		}

		if (msg.param2 == 0)
		{
			m_subTab = (subTab_t)0x15;
			break;
		}

		if (msg.param1 == 6)
			m_subTab = (subTab_t)0x20;
		else
			m_subTab = (subTab_t)msg.param2;
		break;

	case 0xc3:
		*(int*)msg.param1 = m_mainTab;
		*(int*)msg.param2 = m_subTab;
		break;

	case 0xdd:
		BuildDisplayStand(m_subTab);
		break;

	case 0xc6:
	{
		int menu = msg.param1;
		if (menu == 0)
		{
			if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
				SelectMenu(1);
			else
			{
				m_mainTab = (mainTab_t)5;
				m_subTab = (subTab_t)0x11;
				ChangeMainTab((mainTab_t)5, true);
				ChangeSubTab((subTab_t)0x11, true);
				TurnOffItemStatus();
				m_anotherHand.WaitUntilWorkDone(9999);
				if (m_pExhibition)
				{
					delete m_pExhibition;
					m_pExhibition = NULL;
				}
			}
		}
		else if (menu == 1)
		{
			m_mainTab = (mainTab_t)0;
			m_subTab = (subTab_t)0x15;
			ChangeMainTab((mainTab_t)0, true);
			ChangeSubTab((subTab_t)0x15, true);
			ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
		}
		else if (menu == 2)
		{
			if (IsLocalContent(S4_REPLAY_SYSTEM))
			{
				m_mainTab = (mainTab_t)5;
				m_subTab = (subTab_t)0x17;
				ChangeMainTab((mainTab_t)5, true);
				ChangeSubTab((subTab_t)0x17, true);
				m_anotherHand.WaitUntilWorkDone(9999);
				if (m_pExhibition)
				{
					delete m_pExhibition;
					m_pExhibition = NULL;
				}
			}
		}
		else if (menu == 3)
		{
			if (IsLocalContent(S4_UCC))
			{
				m_mainTab = (mainTab_t)0;
				m_subTab = (subTab_t)0x1a;
				ChangeMainTab((mainTab_t)0, true);
				ChangeSubTab((subTab_t)0x1a, true);
				ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
			}
		}
		else if (menu == 4)
		{
			if (IsLocalContent(S4_CARD_SYSTEM))
			{
				m_mainTab = (mainTab_t)1;
				m_subTab = (subTab_t)0x1b;
				ChangeMainTab((mainTab_t)1, true);
				ChangeSubTab((subTab_t)0x1b, true);
				m_anotherHand.WaitUntilWorkDone(9999);
				if (m_pExhibition)
				{
					delete m_pExhibition;
					m_pExhibition = NULL;
				}
			}
		}
		else if (menu == 5)
		{
			if (IsLocalContent(S4_REAL_MYROOM))
			{
				m_mainTab = (mainTab_t)6;
				m_subTab = (subTab_t)0x20;
				ChangeMainTab((mainTab_t)6, true);
				ChangeSubTab((subTab_t)0x20, true);
				m_anotherHand.WaitUntilWorkDone(9999);
				if (m_pExhibition)
				{
					delete m_pExhibition;
					m_pExhibition = NULL;
				}
				OnTabMainButtonUp();
			}
		}
		else if (menu == 6)
		{
			if (IsLocalContent(S4_ITEM_STORAGE))
			{
				SelectMenu(0xb);
				m_mainTab = (mainTab_t)0;
				m_subTab = (subTab_t)0x15;
				ChangeMainTab((mainTab_t)0, true);
				ChangeSubTab((subTab_t)0x15, true);
				ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
			}
			else
			{
				m_mainTab = (mainTab_t)0;
				m_subTab = (subTab_t)0x15;
				ChangeMainTab((mainTab_t)0, true);
				ChangeSubTab((subTab_t)0x15, true);
				ChangeCharacter(Doc()->m_myInfo.userEquip.guidChar);
			}
		}
	}
	break;

	case 0xc7:
		if (m_pNotifyOneDlg)
		{
			m_pNotifyOneDlg->Close((eFormRet)1, true);
			m_pNotifyOneDlg = NULL;
		}
		break;

	case 0xc8:
	{
		sCharacterInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		info.gift_flag = 1;
		ItemManager()->GetDefCombo(info.tid, info.tidParts);

		Doc()->m_charMap.insert(
			std::map<unsigned int, sCharacterInfo>::value_type(info.guid,
				info));
	}
	break;

	case 0xca:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		item.HourRemain = msg.param3 * 24;
		item.ItemType = msg.param4;
		item.gift_flag = 1;

		if (IsLocalContent(S4_UCC) && IsUccClothes(item.tid))
		{
			strncpy(item.UccIndex, ((sBuyItemResult*)msg.param5)->UccIndex, 9);
			item.Seq = 1;
			Doc()->m_uccItemList.push_back(item);
			break;
		}

		Doc()->AddMyPartsList(item);
	}
	break;

	case 0xcb:
	{
		sCaddieInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		info.gift_flag = 1;
		info.Rent_flag = 1;
		info.Remain_Date = 30;

		if (info.tid >= 0x1c00000c && info.tid <= 0x1c00000e)
		{
			info.Level = (unsigned char)msg.param4;
			info.Exp = msg.param3;
		}

		Doc()->m_caddieMap.insert(
			std::map<unsigned int, sCaddieInfo>::value_type(info.guid, info));
	}
	break;

	case 0xcc:
	{
		int date = msg.param3;
		CADDIE_MAP_IT it = Doc()->m_caddieMap.find(msg.param1);
		if (it == Doc()->m_caddieMap.end())
			return;

		sCaddieInfo& info = (*it).second;
		info.tidPart = msg.param2;
		info.Remain_Partdate = date;
		if (info.Remain_Date < date)
			info.Remain_Date = date;
	}
	break;

	case 0xc9:
	{
		unsigned long typeId = msg.param2;
		Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];

		IFF_STRUCT::sHairStyle* pHair = ItemManager()->FindHairStyle(typeId);
		if (pHair == NULL)
			return;

		for (CHAR_MAP_IT it = Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			if ((*it).second.tid == (pHair->cCharID | 0x4000000))
			{
				(*it).second.hairClr = pHair->cHairID;
				break;
			}
		}

		if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
		{
			CPartTidList& parts = Doc()->GetMyPartTidList();
			m_pExhibition->GetPetFrame()->SetHairClr(&parts, pHair->cHairID);
		}
	}
	break;

	case 0xcd:
	{
		sMascotInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
		strcpy(info.szMsg, "PANGYA!");
		info.Remain_Date = pResult->Time;
		info.endDate = pResult->endDate;

		Doc()->AddMascot(&info);
	}
	break;

	case 0xe0:
	{
		unsigned long uid = msg.param1;
		unsigned long typeId = msg.param2;
		sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
		if (ItemManager()->FindCard(typeId) == NULL)
			return;

		int count = pResult->Count;

		sCards card;
		memset(&card, 0, sizeof(card));
		card.uid = uid;
		card.tid = typeId;
		card.cardCount = count;

		for (std::list<sCards>::iterator it = Doc()->m_cardList.begin();
			it != Doc()->m_cardList.end(); ++it)
		{
			if ((*it).tid == typeId)
			{
				(*it).cardCount += count;
				return;
			}
		}

		Doc()->m_cardList.push_back(card);
	}
	break;

	case 0xce:
	{
		if (msg.param2 == 0x1a000010)
		{
			Doc()->m_myInfo.stat.i64Pang += msg.param3;
			break;
		}

		int period = 0;
		unsigned char periodType = 0;
		int count;
		std::list<sItemInfo>* pList = NULL;

		switch ((unsigned long)msg.param2 >> 26)
		{
		case 6:
			period = msg.param3;
			count = ((sBuyItemResult*)msg.param5)->Count;
			periodType = (unsigned char)msg.param4;
			pList = &Doc()->m_myItemList;
			break;

		case 5:
			period = msg.param3;
			count = ((sBuyItemResult*)msg.param5)->Count;
			periodType = (unsigned char)msg.param4;
			pList = &Doc()->m_ballList;
			break;

		case 0x1c:
		{
			sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
			if (ItemManager()->FindAuxPart(pResult->Typeid) == NULL)
				return;

			count = pResult->Count;
			pList = &Doc()->m_auxPartList;
		}
		break;

		case 0x1d:
			if (ItemManager()->FindQuestDrop(msg.param2) == NULL)
				return;

			count = 1;
			pList = &Doc()->m_questDropList;
			break;

		default:
			return;
		}

		if (pList == NULL)
			return;

		bool bFound = false;
		if (msg.param4 == 0)
		{
			for (ITEM_LIST_IT it = pList->begin(); it != pList->end(); ++it)
			{
				if (it->guid == msg.param1)
				{
					int time = 0;
					if (periodType)
					{
						if (periodType == 3)
							time = period;
						else if (periodType == 4)
							time = period * 24;
						else if (periodType == 5)
							time = period * 720;

						Net_StartPeriodItem(it->guid);
					}

					it->HourRemain = it->HourRemain + time;
					it->Common[0] = count;
					if (it->ItemType && it->HourRemain > 0)
						Net_StartPeriodItem(it->guid);

					bFound = true;
					break;
				}
			}
		}

		if (!bFound)
		{
			sItemInfo item;
			memset(&item, 0, sizeof(item));
			item.guid = msg.param1;
			item.gift_flag = 1;
			item.tid = msg.param2;
			item.Common[0] = 1;
			if (count >= 1)
				item.Common[0] = count;
			item.ItemType = periodType;
			if (periodType)
			{
				if (periodType == 3)
					item.HourRemain = period;
				else if (periodType == 4)
					item.HourRemain = period * 24;
				else if (periodType == 5)
					item.HourRemain = period * 720;

				Net_StartPeriodItem(msg.param1);
				item.ItemType = 2;
			}

			pList->push_back(item);
		}

		BuildDisplayStand(m_subTab);
	}
	break;

	case 0xcf:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;

		Doc()->m_clubSetMap.insert(
			std::map<unsigned int, sItemInfo>::value_type(item.guid, item));
	}
	break;

	case 0xd0:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		item.ItemType = msg.param4;
		item.HourRemain = msg.param3 * 24;

		Doc()->m_cutinList.push_back(item);
	}
	break;

	case 0xd1:
	{
		unsigned long uid = msg.param1;
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pItemList->m_itemList.begin();
			it != m_pItemList->m_itemList.end(); ++it)
		{
			FrListItem* item = *it;
			sItemInfo* info = (sItemInfo*)item->pData;
			if (info == NULL)
				break;

			if (info->guid == uid)
			{
				m_pItemList->_DelItem(it);
				break;
			}
		}

		m_pItemList->SortItem(EmptyCompares);

		for (GIFT_LIST_IT git = Doc()->m_giftList.begin();
			git != Doc()->m_giftList.end(); ++git)
		{
			if ((*git).guid == uid)
			{
				git = Doc()->m_giftList.erase(git);
				break;
			}
		}

		m_bGiftMoveReady = true;
	}
	break;

	case 0xd2:
		if (m_pEnchantDlg)
			m_pEnchantDlg->SetResultCode((unsigned char)msg.param1);
		SetLevelBar();
		break;

	case 0xd3:
		if (m_pDowngradeDlg)
			m_pDowngradeDlg->SetLevelBar();
		if (m_pDowngradeDlg2)
			m_pDowngradeDlg2->SetLevelBar();
		SetLevelBar();
		break;

	case 0xd4:
		if (m_pEnchantDlg == NULL)
		{
			m_pEnchantDlg = CreateForm<FrEnchantDlg>(g_pFresh->GetManager(),
				this, "enchant");
			m_pEnchantDlg->SetType((unsigned char)msg.param1,
				(unsigned char)msg.param2);
			m_pEnchantDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnEnchantDlgResult, 1);
		}
		break;

	case 0x26c:
		m_charInfoMap = Doc()->m_charMap;
		break;

	case 0x49:
		m_anotherHand.WaitUntilWorkDone(9999);
		ChangeCharacter(((sCharacterInfo*)msg.param2)->guid);
		break;

	case 0x4a:
	{
		m_anotherHand.WaitUntilWorkDone(9999);
		sCaddieInfo* pInfo = (sCaddieInfo*)msg.param2;
		ChangeCaddie(pInfo->guid);
		SetLevelBar();
	}
	break;

	case 0x4b:
	{
		m_anotherHand.WaitUntilWorkDone(9999);
		sItemInfo* pInfo = (sItemInfo*)msg.param2;
		ChangeClubset(pInfo->guid);
		SetLevelBar();
	}
	break;

	case 0x4c:
		m_anotherHand.WaitUntilWorkDone(9999);
		ChangeAztec(msg.param2);
		break;

	case 0x4e:
	{
		sMascotInfo* pInfo = (sMascotInfo*)msg.param2;
		if (Doc()->m_myInfo.userEquip.guidMascot != pInfo->guid)
		{
			m_anotherHand.WaitUntilWorkDone(9999);
			ChangeMascot(pInfo->guid);
			Doc()->m_myInfo.userEquip.guidMascot = pInfo->guid;
		}

		if (m_pExhibition && m_pExhibition->GetModel() && pInfo->tid)
		{
			IFF_STRUCT::sMascot* pMascot =
				ItemManager()->FindMascot(pInfo->tid);
			if (pMascot)
				CMascot::ChangeMascotMsgInTex(pInfo->szMsg, pMascot,
					m_pExhibition ? m_pExhibition->GetModel() : NULL,
					MakeStr("myroom%02d", pMascot->c.TypeId & 0x3ffffff));
		}

		if (pInfo->PCBangMascot)
			COption::Instance()->mSetPCBangMascotMsg(pInfo->szMsg);

		BuildDisplayStand(m_subTab);
	}
	break;

	case 0x68:
		if (CLoginInfo::Instance()->IsPcBang())
		{
			if (GetFlags(1))
			{
				if (msg.param1 == 0)
					Reinitialize(true);
				else
					ReinitializePC((ePCBangItem)msg.param1);
			}
		}
		else
			Reinitialize(false);

		Net_EquipCharacter();
		Net_EquipDresserParts();
		Net_EquipCaddie();
		Net_EquipItem();
		Net_EquipGear();
		Net_EquipMascot();
		break;

	case 0xdf:
		Net_EquipDresserParts();
		Net_EquipCharacter();
		Net_EquipCaddie();
		Net_EquipItem();
		Net_EquipGear();
		Net_EquipSkin();
		Net_EquipMascot();
		break;

	case 0xe1:
		BuildDisplayStand(m_subTab);
		break;

	case 0x204:
		if (IsLocalContent(S3_CADDIE_REPORT))
		{
			*(unsigned long*)msg.param1 = m_tikiReportUid;
			m_tikiReportUid = 0;
		}
		break;

	case 0x209:
		m_camLimitType = msg.param1;
		if (m_camLimitType == 1)
		{
			m_camLimit[0] = (float)msg.param2;
			m_camLimit[1] = (float)msg.param3;
		}
		else if (m_camLimitType == 2)
		{
			m_camLimit[2] = (float)msg.param2;
			if (m_camLimit[2] < -90.0f)
				m_camLimit[2] = -90.0f;
			m_camLimit[3] = (float)msg.param3;
			if (m_camLimit[3] > 45.0f)
				m_camLimit[3] = 45.0f;
		}
		break;

	case 0x20a:
	{
		float x = (float)msg.param1;
		float z = (float)msg.param2;
		char name[32];
		strcpy(name, (const char*)msg.param3);

		GOLFDOC()->m_pPolySoup->SetAdditionModel(name, WVector(x, 0.0f, z));
		GOLFDOC()->m_pPolySoup->EndLoad(false);
	}
	break;

	case 0x22b:
		g_camera.pivot =
			WVector((float)msg.param1, (float)msg.param2, (float)msg.param3);
		break;

	case 0x20b:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_bShowCharacter)
				ChangeSubTab((subTab_t)0x15, false);

			if (Doc()->m_myInfo.info.dwUID == Doc()->m_replayTargetUID)
				SetMiddleUIControl(!m_bShowCharacter);

			CharacterViewControl(m_bShowCharacter);
		}

	case 0x220:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pAuthorityDlg)
			{
				m_pAuthorityDlg->Close(true);
				m_pAuthorityDlg = NULL;
			}
		}
		break;

	case 0x212:
		if (m_pUccDrawDlg)
			m_pUccDrawDlg->Upload();
		break;

	case 0x214:
	{
		unsigned char bSuccess = (unsigned char)msg.param1;
		unsigned long uid = *(unsigned long*)msg.param2;
		unsigned long typeId = *(unsigned long*)msg.param3;
		const char* uccIndex = (const char*)msg.param4;
		const char* uccName = (const char*)msg.param5;

		if (m_pUccDrawDlg == NULL)
			return;

		if (m_pUccDrawDlg->m_pItemInfo->tid != typeId ||
			strcmp(m_pUccDrawDlg->m_pItemInfo->UccIndex, uccIndex) != 0)
			return;

		m_pUccDrawDlg->m_bWaiting = false;
		if (bSuccess == false)
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3 \xb5\xee\xb7\xcf\xbf\xa1 "
				"\xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 0);
			break;
		}

		for (int i = 0; i < 2; ++i)
		{
			CUccClothes* pClothes = m_pUccDrawDlg->m_pClothes[i];
			Bitmap* pTexture = pClothes->GetTexture();
			const char* texName = pClothes->GetTextureName();
			w_texlist* pTex = g_resrcmng->FindTexture(texName);
			if (pTex == NULL)
				g_resrcmng->UploadTexture(texName, pTexture, 0, NULL);
			else
				g_resrcmng->FixTexture(pTex->texhandle, pTexture, 0, NULL);
		}

		sUccClothes clothes;
		SYSTEMTIME st;
		GetSystemTime(&st);
		clothes.id = uid;
		clothes.date = st;
		clothes.downloadCount = 1;
		clothes.typeId = typeId;
		strncpy(clothes.uccIndex, uccIndex, 9);
		strncpy(clothes.uccName, uccName, 41);

		std::map<std::string, Bitmap*>::iterator it =
			UccManager()->m_icon.find(GenerateIconFilename(typeId, uccIndex));
		if (it != UccManager()->m_icon.end())
			clothes.icon = (*it).second;

		UccManager()->AddClothes(1, clothes, true, false);

		for (ITEM_LIST_IT itItem = Doc()->m_uccItemList.begin();
			itItem != Doc()->m_uccItemList.end(); ++itItem)
		{
			sItemInfo* pItem = &(*itItem);
			if (pItem && pItem->tid == typeId &&
				strcmp(pItem->UccIndex, uccIndex) == 0)
			{
				pItem->status &= ~2;
				pItem->status |= 1;
				break;
			}
		}

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xcc "
			"\xbf\xcf\xbc\xba\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 0);

		m_pUccDrawDlg->Close((eFormRet)1, true);
	}
	break;

	case 0x213:
	{
		unsigned long typeId = *(unsigned long*)msg.param1;
		const char* uccIndex = (const char*)msg.param2;
		const char* errName = (const char*)msg.param3;

		const char* errors[] = { "", "ERR_FILE", "ERR_UCC", "ERR_KEY",
			"ERR_USER", "ERR_SRVVAR" };
		int code = 0;
		for (unsigned int i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
		{
			if (strcmpi(errors[i], errName) == 0)
			{
				code = i;
				break;
			}
		}

		if (m_pUccDrawDlg == NULL)
			return;

		if (m_pUccDrawDlg->m_pItemInfo->tid != typeId ||
			strcmp(m_pUccDrawDlg->m_pItemInfo->UccIndex, uccIndex) != 0)
			return;

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			MakeStr(
				"\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. "
				"\xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)",
				code + 10),
			false);
		pForm->Open(NULL, 1);

		m_pUccDrawDlg->CloseWaitingForm();
	}
	break;

	case 0x215:
	{
		unsigned char result = (unsigned char)msg.param1;
		unsigned long typeId = *(unsigned long*)msg.param2;
		const char* uccIndex = (const char*)msg.param3;

		if (m_pUccDrawDlg == NULL)
			return;

		if (m_pUccDrawDlg->m_pItemInfo->tid != typeId ||
			strcmp(m_pUccDrawDlg->m_pItemInfo->UccIndex, uccIndex) != 0)
			return;

		m_pUccDrawDlg->m_bWaiting = false;
		if (result == 1)
		{
			UccManager()->SetUccTempClothes(typeId, uccIndex,
				m_pUccDrawDlg->m_pClothes[0]->GetTexture32BPP(true),
				m_pUccDrawDlg->m_pClothes[1]->GetTexture32BPP(true));
			m_pUccDrawDlg->SetSaved(true);
			return;
		}

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xc7 \xc0\xd3\xbd\xc3 "
			"\xc0\xfa\xc0\xe5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 1);
	}
	break;

	case 0x219:
	{
		unsigned long typeId = msg.param1;
		bool bFail = msg.param2 == 1;

		if (m_pOverBarSpecialCard && m_pSpecialCardDlg == NULL)
			m_pOverBarSpecialCard->EnableBlink(
				CCardManager::Instance()->ShowSPCardAlarm());

		if (bFail == false)
		{
			if (m_pCardUseDlg == NULL)
			{
				FrCardUseDlg* pDlg = CreateForm<FrCardUseDlg>(
					g_pFresh->GetManager(), this, "usecard_result");
				pDlg->m_cardTypeId = typeId;
				pDlg->Open(NULL, 0);
			}

			if (m_pCardBook)
				m_pCardBook->ReBuildCurrentPage();
		}
	}
	break;

	case 0x21a:
		if (m_pCardBook && m_pCardBook->m_bShow == true)
			m_pCardBook->ReBuildCurrentPage();
		BuildCardPackList();
		break;

	case 0x229:
		if (m_pCardBook)
			m_pCardBook->ReBuildCurrentPage();
		BuildCardPackList();
		break;

	case 0x22a:
	{
		unsigned char level = (unsigned char)msg.param2;
		Doc()->m_myInfo.stat.dwExp = msg.param1;
		Doc()->m_myInfo.stat.Level = level;
	}
	break;

	case 0x221:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			m_pPostSendDlg = NULL;
			m_pPostSendDlg = CreateForm<CPost_SendDlg>(g_pFresh->GetManager(),
				this, "write_post");
			FrForm::ms_bHasTail = false;
			m_pPostSendDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostSendDlgResult, 0x81);
			m_pPostSendDlg->SetFadeout(false);
		}
		break;

	case 0x223:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pPostReciveDlg)
				m_pPostReciveDlg->SetLoadPostList();
		}
		break;

	case 0x224:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pPostReciveDlg)
				m_pPostReciveDlg->OnPostDescDlgCreate();
		}
		break;

	case 0x226:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pPostReciveDlg)
				m_pPostReciveDlg->OnLoadPostList();
		}
		break;

	case 0x225:
		if (msg.param1 == 0x13e)
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "rmr_notify");
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xc2\xbd\xc0\xb4\xcf\xb4\xd9. \n"
				"\xc6\xed\xc1\xf6 \xbe\xb2\xb1\xe2\xb8\xa6 \xb0\xe8\xbc\xd3 "
				"\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
				false);
			pForm->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostSendDlg2Result, 2);
		}
		break;

	case 0x228:
	{
		int type = msg.param1;
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");

		switch (type)
		{
		case 0x122:
			if (msg.param2)
				pForm->SetMessage(
					"\xb8\xb6\xc0\xcc\xb7\xeb\xc0\xc7 \xb1\xc7\xc7\xd1\xc0\xbb "
					"\xbc\xb3\xc1\xa4\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage(
					"\xb8\xb6\xc0\xcc\xb7\xeb\xc0\xc7 \xb1\xc7\xc7\xd1\xc0\xbb "
					"\xbc\xb3\xc1\xa4\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			break;

		case 0x124:
		{
			int bSave = msg.param2;
			pForm->Close(true);

			FrForm* pSaveForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			if (bSave)
				pSaveForm->SetMessage(
					"\xc7\xf6\xc0\xe7 \xb9\xe8\xc4\xa1\xb8\xa6 "
					"\xc0\xfa\xc0\xe5\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else
				pSaveForm->SetMessage(
					"\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 \xb0\xa1\xb1\xb8\xb8\xa6 "
					"\xb3\xf5\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.. ",
					false);

			pSaveForm->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnFunritureSaveMsgdlgResult,
				3);
		}
		break;

		case 0x125:
			pForm->SetMessage(
				"\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 \xc0\xd6\xb4\xc2 \xb0\xa1\xb1\xb8 "
				"\xc1\xa4\xba\xb8\xb8\xa6 \xb0\xa1\xc1\xae\xbf\xc0\xc1\xf6 "
				"\xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x123:
			pForm->SetMessage(
				"\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 \xb5\xe9\xbe\xee\xb0\xa5 \xbc\xf6 "
				"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x141:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xc7\xd4\xc0\xc7 \xc6\xed\xc1\xf6\xb5\xe9\xc0\xbb "
				"\xc0\xd0\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x143:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xc0\xc7 \xb3\xbb\xbf\xeb\xc0\xbb "
				"\xc0\xd0\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x145:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xbf\xa1 \xc3\xb7\xba\xce\xb5\xc8 "
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbf\xc5\xb1\xe2\xc1\xf6 "
				"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x15d:
			if (msg.param2 == 9)
				pForm->SetMessage(
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee "
					"\xc0\xd6\xbe\xee \xc1\xf6\xbf\xec\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xc1\xf6\xbf\xec\xc1\xf6 "
					"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			break;

		case 0x13f:
		{
			int code = msg.param2;
			if (code == 1)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc6\xed\xc1\xf6 \xbc\xf6\xbd\xc5\xc0\xbb "
					"\xb0\xc5\xba\xce\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 2)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xc0\xaf\xc0\xfa\xb8\xa6 "
					"\xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 3)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc7\xd1 "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 4)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 "
					"\xb4\xf5 \xb8\xb9\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xba\xb8\xb3\xbb\xb7\xc1\xb0\xed "
					"\xbd\xc3\xb5\xb5\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9..",
					false);
			else if (code == 5)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
					"\xba\xb8\xb3\xbb\xb7\xc1\xb0\xed \xbd\xc3\xb5\xb5\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 6)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc \xc6\xed\xc1\xf6\xb7\xce \xb9\xde\xc0\xba "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 7)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xb7\xba\xce\xc7\xd2 \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 8)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xc6\xed\xc1\xf6 \xb9\xdf\xbc\xdb\xc0\xbb \xc7\xd2 \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 9)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n"
					"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xbf\xec\xc6\xed\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6 "
					"\xc2\xf7\xbc\xad \xc7\xf6\xc0\xe7\xb4\xc2 \xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xb9\xde\xc0\xbb "
					"\xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 10)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xc7\xd4 \xb1\xe2\xb4\xc9\xc0\xc7 \xba\xce\xba\xd0 "
					"\xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (code == 0x16)
				pForm->SetMessage(
					"\xb8\xde\xc0\xcf \xc3\xb7\xba\xce \xb0\xa1\xb4\xc9\xc7\xd1 \xc3\xd6\xb4\xeb "
					"\xb0\xb3\xbc\xf6\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbb\xc1\xf6 "
					"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
		}
		break;
		}

		if (type != 0x124)
			pForm->Open(NULL, 0);

		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 2, 0, 0, 0, 0, 0);
	}
	break;

	case 0x48:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pPostSendDlg)
				m_pPostSendDlg->UpdateFriendList();
		}
		break;

	case 0x50:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (m_pPostSendDlg)
				m_pPostSendDlg->SetReceiverInfo(msg.param1,
					*(sPangYaUserInfo*)msg.param2);
		}

		if (CUserInfo::Instance())
		{
			FrUserInfoForm* pDlg = CUserInfo::Instance()->GetDlg();
			if (pDlg)
			{
				if (CUserInfo::Instance()->IsVisible() == true)
				{
					if (pDlg->IsOpenAddFriendDlg())
					{
						sPangYaUserInfo* pInfo = (sPangYaUserInfo*)msg.param2;
						pDlg->SetRequestFriendData(pInfo->sNick, pInfo->dwUID);
					}
				}
			}
		}
		break;

	case 0x205:
		if (IsLocalContent(S3_CADDIE_REPORT))
		{
			if (m_pUniteResultDlg == NULL)
			{
				SYSTEMTIME time = *(SYSTEMTIME*)msg.param1;
				m_pUniteResultDlg = CreateForm<FrUniteResultDlg>(
					g_pFresh->GetManager(), this, "uniteresult");
				m_pUniteResultDlg->EnableDrag(false);
				m_pUniteResultDlg->SetTikiReportState(SYSTEMTIME(time));
				m_pUniteResultDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnResultDlg_Report,
					WPoint(68.0f, 48.0f), 1);
			}
		}
		break;

	case 0x22f:
		if (g_pFresh->IsCurrentLayout("REALMYROOM"))
		{
			SetListMenu();
			BuildDisplayStand(m_subTab);
		}
		break;

	case 0x233:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			SetItemStorageVisible(true);
		break;

	case 0x234:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			SetItemStorageVisible(false);
			if (m_pItemStorageAccessPanel)
				m_pItemStorageAccessPanel->Close((eFormRet)2, true);
		}
		break;

	case 0x235:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			OpenItemStorageAccessPanel(1);
		break;

	case 0x236:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			OpenItemStorageNotice();
		break;

	case 0x237:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			OpenItemStorageAccessPanel(2);
		break;

	case 0x238:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			OpenItemStorageAccessPanel(3);
		break;

	case 0x239:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
			OpenItemStorageAccessPanel(4);
		break;

	case 0x23a:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 0x233, 0, 0, 0, 0, 0);
			if (m_bItemStorageVisible == true)
				BuildDisplayStand(m_subTab);
		}
		break;

	case 0x23d:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			if (m_bItemStorageVisible == true)
			{
				CItemStorage* pStorage = CItemStorage::Instance();
				if (pStorage)
				{
					unsigned long page = pStorage->GetCurPage();
					WSendPacket packet((enumClientPacket)0xcf);
					packet.Encode4(0x63);
					packet.Encode2((unsigned char)page);
					packet.Send(TO_GAME);
				}
			}
		}
		break;

	case 0x23e:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			if (m_bItemStorageVisible == true)
			{
				CItemStorage* pStorage = CItemStorage::Instance();
				if (pStorage)
				{
					unsigned long page = pStorage->GetCurPage();
					if (page == 0)
						page = 1;

					WSendPacket packet((enumClientPacket)0xcf);
					packet.Encode4(0x63);
					packet.Encode2((unsigned char)page);
					packet.Send(TO_GAME);
				}
			}
		}
		break;

	case 0x23f:
	{
		int result = msg.param2;
		bool bRefresh = false;
		if (result == 0)
		{
			if (msg.param1 == 0x17b)
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
					(int)"\xc6\xd0\xbd\xba\xbf\xf6\xb5\xe5\xb0\xa1 \xbc\xb3\xc1\xa4\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0));
		}
		else if (result == 0x64)
		{
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x23, (int)"\xbd\xc7\xc6\xd0", 0, 0, 0, 0));
		}
		else if (result == 0x65)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x66)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0,
				0, 0));
		}
		else if (result == 0x67)
		{
		}
		else if (result == 0x68)
		{
		}
		else if (result == 0x6a)
		{
		}
		else if (result == 0x6b)
		{
			bRefresh = true;
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc1\xa4\xbb\xf3 \xc3\xb3\xb8\xae \xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x6c)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc3\xb3\xb8\xae\xc7\xd2\xbc\xf6 \xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb \xc3\xd6\xb4\xeb \xb0\xb3\xbc\xf6\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x6d)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc3\xa2\xb0\xed\xbf\xa1 \xba\xb8\xb0\xfc\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x6e)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xb1\xe2\xb0\xa3\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc3\xa2\xb0\xed\xbf\xa1 \xb3\xd6\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x6f)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xb8\xc0\xaf\xc7\xd1 \xb1\xdd\xbe\xd7\xba\xb8\xb4\xd9 \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x70)
		{
			bRefresh = true;
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x71)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc3\xa2\xb0\xed\xb0\xa1 \xc0\xcc\xb9\xcc \xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x72)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xba\xb8\xc0\xaf\xc1\xdf\xc0\xce \xc3\xa2\xb0\xed\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x73)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc6\xed\xc1\xfd\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0,
				0, 0));
		}
		else if (result == 0x74)
		{
			bRefresh = true;
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc3\xa2\xb0\xed\xb0\xa1 \xc0\xe1\xb1\xdd\xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x75)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc6\xd0\xbd\xba\xbf\xf6\xb5\xe5\xb0\xa1 \xc6\xb2\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x69)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc6\xd0\xbd\xba\xbf\xf6\xb5\xe5\xb4\xc2 \xbc\xfd\xc0\xda 4\xc0\xda\xb8\xae\xb7\xce\xb8\xb8 \xbc\xb3\xc1\xa4 \xb0\xa1\xb4\xc9 \xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x76)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6\xb7\xae \xbf\xa1\xb7\xaf",
				0, 0, 0, 0));
		}
		else if (result == 0x77)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xb8\xc0\xaf\xc7\xd1 \xb1\xdd\xbe\xd7\xba\xb8\xb4\xd9 \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x78)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"1\xc8\xb8 \xc0\xd4,\xc3\xe2\xb1\xdd \xc7\xd2\xbc\xf6\xc0\xd6\xb4\xc2 \xb9\xfc\xc0\xa7\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x79)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)"\xbf\xac\xbc\xd3\xb5\xc8 \xbc\xfd\xc0\xda\xb7\xce \xbc\xb3\xc1\xa4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}
		else if (result == 0x7a)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
				(int)" \xb8\xb6\xc0\xcc\xb7\xeb \xc3\xd6\xb4\xeb \xba\xb8\xc0\xaf \xbc\xf6\xb7\xae\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
		}

		if (bRefresh)
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 0x234, 0, 0, 0, 0, 0);
	}
	break;

	case 0x23c:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage && m_pItemStorageList)
			{
				pStorage->InitIndexControl(pStorage->GetTotalPage(), 5);
				CIndexControl* pIndex = pStorage->GetIndexControl();
				if (pIndex)
					pIndex->SelectIndex(0);
			}
		}
		break;

	case 0x23b:
		if (IsLocalContent(S4_ITEM_STORAGE) == true)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage && m_pItemStorageList)
			{
				pStorage->InitIndexControl(pStorage->GetTotalPage(), 5);
				CIndexControl* pIndex = pStorage->GetIndexControl();
				if (pIndex)
					pIndex->SelectIndex(pIndex->GetCurIndexCount());
			}
		}
		break;

	case 0x242:
		if (IsInItemList(msg.param1))
			*(bool*)msg.param2 = true;
		else
			*(bool*)msg.param2 = false;
		break;

	case 0x244:
		if (m_pPostReciveDlg)
			m_pPostReciveDlg->EnableMiniBtns(msg.param1 != 0);
		break;

	case 0x245:
		if (!IsLocalContent(S4_FURNITURE_ABILITY))
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 "
				"\xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
				false);
			pForm->Open(NULL, 3);
		}
		else
		{
			unsigned long typeId = msg.param1;
			int count = msg.param2;
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_item");
			if (pForm == NULL)
				return;

			FrArea* pIcon =
				DYNAMIC_CAST(FrArea, pForm->FindChildByName("itemicon"));
			FrEdit* pName =
				DYNAMIC_CAST(FrEdit, pForm->FindChildByName("itemname"));

			if (pIcon)
			{
				if (pItem)
					pIcon->SetBgImg(pItem->Icon);
				else if (typeId)
				{
					WRect rect = pIcon->GetRect();
					pIcon->SetRect(
						WRect(rect.x + 15.0f, rect.y + 7.0f, rect.w, rect.h));
					pIcon->SetBgImg("item1_16");
				}
				else
					pIcon->SetBgImg("hide");
			}

			if (pName)
			{
				if (pItem)
					pName->AddText(pItem->Name, false, true);
				else if (typeId)
					pName->AddText(MakeStr("%d\xc6\xce", typeId), false, true);
				else
					pName->AddText("none", false, true);

				pName->AddText("\n", false, true);
			}

			pForm->SetMessage(
				MakeStr(
					"\xb0\xa1\xb1\xb8\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 %s "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb %d\xb0\xb3 "
					"\xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					pItem->Name, count),
				false);
			pForm->Open(NULL, 2);
		}
		break;

	case 0x12d:
	{
		FrLevelupItemForm* pForm = CreateForm<FrLevelupItemForm>(
			g_pFresh->GetManager(), (FrCmdTarget*)this, "levelupitem");

		if (pForm == NULL)
			return;

		pForm->SetData(*(sLevelUpDone*)msg.param1);
		pForm->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::OnLevelupFormResult, 3);

		m_levelupForms.push_back(pForm);
	}
	break;

	case 0x266:
		UnselectItemList(msg.param1);
		break;

	case 0x267:
		SelectItemList(msg.param1);
		break;

	case 3:
	case 0xaa:
	case 0xc0:
		break;
	case 0xc1:
	case 0xd5:
	case 0xd6:
	case 0xd7:
	case 0xd8:
		break;
	case 0xd9:
	case 0xda:
	case 0xdb:
	case 0xdc:
	case 0xde:
	case 0xe8:
		break;
	case 0xe9:
	case 0x1fc:
	case 0x211:
	case 0x232:
		break;

	default:
		HandleMsgCommon(msg);
	}
}

bool CRealMyRoomMain::OnReplayModeOpenDlgResult(int result, FrForm* form)
{
	m_pReplayModeDlg = NULL;

	if (result == 1)
	{
		memset(&Doc()->m_roomInfo, 0, sizeof(sRoomInfo));
		Doc()->m_playingMode = (eReplayModeType)2;
		SetGameType(0);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
	}

	return true;
}

bool CRealMyRoomMain::OnReplayDelOpenDlgResult(int result, FrForm* form)
{
	m_pReplayModeDlg = NULL;

	if (result == 1)
	{
		BuildDisplayStand((subTab_t)23);
	}

	return true;
}

void CRealMyRoomMain::OnFurniturePupInit(int param)
{
	m_pFurniturePup = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurniturePup)
	{
		m_pFurniturePup->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurniturePupButtonUp()
{
	m_furniturePage--;

	m_furniturePage = Between(0, m_furniturePage, m_furnitureMaxPage - 1);

	m_pFurnitureListNum->ClearLine();
	m_pFurnitureListMaxNum->ClearLine();
	m_pFurnitureListNum->AddText(MakeStr("%3d", m_furniturePage + 1), false,
		true);
	m_pFurnitureListMaxNum->AddText(MakeStr("%3d", m_furnitureMaxPage), false,
		true);

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurniturePdownInit(int param)
{
	m_pFurniturePdown = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurniturePdown)
	{
		m_pFurniturePdown->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurniturePdownButtonUp()
{
	m_furniturePage++;

	m_furniturePage = Between(0, m_furniturePage, m_furnitureMaxPage - 1);

	m_pFurnitureListNum->ClearLine();
	m_pFurnitureListMaxNum->ClearLine();
	m_pFurnitureListNum->AddText(MakeStr("%3d", m_furniturePage + 1), false,
		true);
	m_pFurnitureListMaxNum->AddText(MakeStr("%3d", m_furnitureMaxPage), false,
		true);

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurnitureListNumInit(int param)
{
	m_pFurnitureListNum = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pFurnitureListNum)
	{
		m_pFurnitureListNum->SetCharLimit(2, false);
		m_pFurnitureListNum->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureListNumBtnUp()
{
	if (m_pFurnitureListNum->IsVisible())
	{
		m_pFurnitureListNum->SetKeyEvent(true);
	}
}

bool CRealMyRoomMain::OnFurnitureListEnterKey(int param)
{
	OnFurniture_BtnListNumBtnUp();

	return false;
}

void CRealMyRoomMain::OnFurnitureListMaxNumInit(int param)
{
	m_pFurnitureListMaxNum = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pFurnitureListMaxNum)
	{
		m_pFurnitureListMaxNum->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurniture_BtnListNumInit(int param)
{
	m_pFurnitureBtnListNum = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureBtnListNum)
	{
		m_pFurnitureBtnListNum->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurniture_BtnListNumBtnUp()
{
	int page = atoi(m_pFurnitureListNum->GetLine(1, false));
	m_furniturePage = page - 1;

	m_furniturePage = Between(0, m_furniturePage, m_furnitureMaxPage - 1);

	m_pFurnitureListNum->ClearLine();
	m_pFurnitureListMaxNum->ClearLine();
	m_pFurnitureListNum->AddText(MakeStr("%3d", m_furniturePage + 1), false,
		true);
	m_pFurnitureListMaxNum->AddText(MakeStr("%3d", m_furnitureMaxPage), false,
		true);

	if (m_pFurnitureListNum->IsVisible())
	{
		m_pFurnitureListNum->SetKeyEvent(true);
	}

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurnitureResetInit(int param)
{
	m_pFurnitureReset = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureReset)
	{
		m_pFurnitureReset->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureResetButtonUp()
{
	CRealMyRoomData::Instance()->ResetList(false);

	for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin(); it != Doc()->m_furnitureList.end(); ++it) // NOFORMAT
	{
		(*it).bArrange = false;
	}

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurnitureBackInit(int param)
{
	m_pFurnitureBack = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureBack)
	{
		m_pFurnitureBack->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackButtonUp()
{
	std::list<sFurniture_List> saveList;

	for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin(); it != Doc()->m_furnitureList.end(); ++it) // NOFORMAT
	{
		(*it).bArrange = false;
	}

	CRealMyRoomData::Instance()->ResotreList();

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurnitureSaveInit(int param)
{
	m_pFurnitureSave = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureSave)
	{
		m_pFurnitureSave->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureSaveButtonUp()
{
	if (MyUID() == Doc()->m_replayTargetUID)
	{
		m_pFurnitureCamera->mat = g_camera;
		m_pFurnitureCamera->fov = g_view->GetFOV();
		m_nearClip = g_view->GetClipNearValue();
		m_farClip = g_view->GetClipFarValue();

		SetCameraDisplayType(2);

		if (CRealMyRoomData::Instance()->SaveModel(false))
		{
			if (MyUID() == Doc()->m_replayTargetUID)
			{
				RMR_UpdateDB();
				CRealMyRoomData::Instance()->SaveModel(true);
			}
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			MakeStr(
				"\xc7\xf6\xc0\xe7\xb4\xc2 \xc0\xfa\xc0\xe5\xc7\xd2\xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9."),
			false);
		pForm->Open(NULL, 3);
	}
}

void CRealMyRoomMain::OnFurnitureOutInit(int param)
{
	m_pFurnitureOut = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFurnitureOut)
	{
		m_pFurnitureOut->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureOutButtonUp()
{
	m_pPointFx->SetActive(false, true);
	if (m_prevUiHideMode == 1)
	{
		if (m_mainTab == 6) ControlUiHideMode(1); // NOFORMAT
		else
			ControlUiHideMode(2);
	}
	else
	{
		ControlUiHideMode(2);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit1(int param)
{
	m_pFurnitureBackGround[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[0])
	{
		m_pFurnitureBackGround[0]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit2(int param)
{
	m_pFurnitureBackGround[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[1])
	{
		m_pFurnitureBackGround[1]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit3(int param)
{
	m_pFurnitureBackGround[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[2])
	{
		m_pFurnitureBackGround[2]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit4(int param)
{
	m_pFurnitureBackGround[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[3])
	{
		m_pFurnitureBackGround[3]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit5(int param)
{
	m_pFurnitureBackGround[4] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[4])
	{
		m_pFurnitureBackGround[4]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit6(int param)
{
	m_pFurnitureBackGround[5] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[5])
	{
		m_pFurnitureBackGround[5]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit7(int param)
{
	m_pFurnitureBackGround[6] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[6])
	{
		m_pFurnitureBackGround[6]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureBackGroundInit8(int param)
{
	m_pFurnitureBackGround[7] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureBackGround[7])
	{
		m_pFurnitureBackGround[7]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureScrollInit1(int param)
{
	m_pFurnitureScroll[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureScroll[0])
	{
		m_pFurnitureScroll[0]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureScrollInit2(int param)
{
	m_pFurnitureScroll[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureScroll[1])
	{
		m_pFurnitureScroll[1]->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureScrollInit3(int param)
{
	m_pFurnitureScroll[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFurnitureScroll[2])
	{
		m_pFurnitureScroll[2]->SetVisible(false);
	}
}

void CRealMyRoomMain::BuildDisplayStandFurniture()
{
	int added = 0;
	int count = 0;
	int type = -1;

	if (!m_pFurnitureList)
		return;

	m_pFurnitureList->ClearItem();

	if (m_furnitureTab == 0)
	{
		for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
		{
			if (++count > m_furniturePage * 11)
			{
				FrListItem* pItem = m_pFurnitureList->AddItem(&(*it));
				added++;

				if ((*it).bArrange)
					m_pFurnitureList->SelectItem(pItem, true);

				if (added >= 11)
					break;
			}
		}
	}
	else if (m_furnitureTab == 4)
	{
		for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
		{
			int subType = (*it).typeId >> 11 & 0x7ff;

			if ((subType >= 0 && subType < 7) || subType >= 10)
			{
				count++;

				if (added < 11)
				{
					if (count > m_furniturePage * 11)
					{
						FrListItem* pItem = m_pFurnitureList->AddItem(&(*it));
						added++;

						if ((*it).bArrange)
							m_pFurnitureList->SelectItem(pItem, true);
					}
				}
			}
		}
	}
	else
	{
		if (m_furnitureTab == 1)
			type = 7;
		else if (m_furnitureTab == 2)
			type = 9;
		else if (m_furnitureTab == 3)
			type = 8;

		for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
		{
			if (((*it).typeId >> 11 & 0x7ff) == type)
			{
				count++;

				if (added < 10)
				{
					if (count > m_furniturePage * 11)
					{
						FrListItem* pItem = m_pFurnitureList->AddItem(&(*it));
						added++;

						if ((*it).bArrange)
							m_pFurnitureList->SelectItem(pItem, true);
					}
				}
			}
		}
	}

	int remain = 11 - added % 11;
	for (int i = 0; i < remain; i++)
		m_pFurnitureList->AddItem(NULL);

	if (m_furnitureTab == 0)
		m_furnitureMaxPage = Doc()->m_furnitureList.size() / 11;
	else
		m_furnitureMaxPage = count / 11;

	if (Doc()->m_furnitureList.size() % 11 != 0)
	{
		m_furnitureMaxPage++;

		if (m_furnitureMaxPage != 1)
		{
			m_pFurniturePup->Enable(true);
			m_pFurniturePdown->Enable(true);
		}
		else
		{
			m_pFurniturePup->Enable(false);
			m_pFurniturePdown->Enable(false);
		}
	}
	else if (m_furnitureMaxPage == 0)
	{
		m_furnitureMaxPage = 1;
		m_pFurniturePup->Enable(false);
		m_pFurniturePdown->Enable(false);
	}

	if (m_furniturePage == 0)
		m_pFurniturePup->Enable(false);

	if (m_furniturePage + 1 == m_furnitureMaxPage)
		m_pFurniturePdown->Enable(false);

	m_pFurnitureListNum->ClearLine();
	m_pFurnitureListNum->AddText(MakeStr("%3d", m_furniturePage + 1), false,
		true);
	m_pFurnitureListMaxNum->ClearLine();
	m_pFurnitureListMaxNum->AddText(MakeStr("%3d", m_furnitureMaxPage), false,
		true);
}

void CRealMyRoomMain::OnFurnitureTabMainInit(int param)
{
	m_pFurnitureTabMain = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pFurnitureTabMain)
	{
		for (int i = 0; i < 5; i++)
		{
			m_pFurnitureTabMain->AddTab(MakeStr("rmr_arg_tab%d", i + 1), i,
				NULL);
		}

		m_pFurnitureTabMain->Select(m_furnitureTab);
		m_pFurnitureTabMain->SetVisible(false);
	}
}

void CRealMyRoomMain::OnFurnitureTabMainButtonUp()
{
	TButtonItem* pSelected = m_pFurnitureTabMain->GetSelected();
	if (pSelected)
	{
		ChangeFMainTab((eFurniture_Tab)pSelected->index, false);
	}
}

void CRealMyRoomMain::ChangeFMainTab(eFurniture_Tab tab, bool bForce)
{
	if (m_furnitureTab != tab)
		m_furniturePage = 0;

	m_furnitureTab = tab;

	if (tab == 2 || tab == 3 || tab == 1)
	{
		m_pFurnitureList->SetMultiSelect(false);
	}
	else
	{
		m_pFurnitureList->SetMultiSelect(true);
	}

	BuildDisplayStandFurniture();
}

void CRealMyRoomMain::OnFurnitureListInit(int param)
{
	m_pFurnitureList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pFurnitureList)
	{
		m_pFurnitureList->SetVisible(false);
		m_pFurnitureList->UseDummy(true);
		m_pFurnitureList->SetMultiSelect(true);
	}
}

void CRealMyRoomMain::OnFurnitureListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const Bitmap* pBase;
	float offsetY = 0.0f;
	char text[512];

	if (!pItem)
	{
		if (m_pItemName && m_pItemName->IsVisible())
			m_pItemName->SetElementFadeOut(true);
		if (m_pItemDesc2 && m_pItemDesc2->IsVisible())
			m_pItemDesc2->SetElementFadeOut(true);
		if (m_pItemDescBg2 && m_pItemDescBg2->IsVisible())
			m_pItemDescBg2->SetElementFadeOut(true);

		return;
	}

	if (!pItem->pData)
	{
		pBase = m_pArrangeItemBase[3];
	}
	else
	{
		if (pItem->selected)
		{
			pBase = m_pArrangeItemBase[0];
			pGDI->SetTextColor(0xffff0000, 0xffffffff);
		}
		else if (pItem->underCursor)
		{
			offsetY = 3.0f;
			pBase = m_pArrangeItemBase[0];
			pGDI->SetTextColor(0xff000000, 0xffffffff);
		}
		else
		{
			pBase = m_pArrangeItemBase[0];
			pGDI->SetTextColor(0xff000000, 0xffffffff);
		}
	}

	pGDI->SetTextStyle(0);

	if (pBase)
	{
		pGDI->DrawTexture(pBase,
			WRect(pItem->pos.x, pItem->pos.y - offsetY, (float)pBase->Width(),
				(float)pBase->Height()),
			0xffffffff, 0);
	}

	sFurniture_List* pFurniture = (sFurniture_List*)pItem->pData;
	if (!pFurniture)
		return;

	IFF_STRUCT::sFurniture* pIff =
		ItemManager()->FindFurniture(pFurniture->typeId);
	if (pIff)
	{
		const Bitmap* pIcon = g_pFresh->GetBitmap(pIff->c.Icon);
		if (pIcon)
		{
			pGDI->DrawTexture(pIcon,
				WRect(pItem->pos.x + 3.0f, pItem->pos.y + 3.0f - offsetY,
					(float)pIcon->Width() * 0.66f,
					(float)pIcon->Height() * 0.66f),
				0xffffffff, 0);
		}
	}

	if (pItem->selected)
	{
		const Bitmap* pSelect = m_pArrangeItemBase[2];
		if (pSelect)
		{
			pGDI->DrawTexture(pSelect,
				WRect(pItem->pos.x, pItem->pos.y - offsetY,
					(float)pSelect->Width(), (float)pSelect->Height()),
				0xffffffff, 0);
		}
	}

	if (pItem->underCursor)
	{
		if (m_pItemName && !m_pItemName->IsVisible())
			m_pItemName->SetElementFadeOut(false);
		if (m_pItemDesc2 && !m_pItemDesc2->IsVisible())
			m_pItemDesc2->SetElementFadeOut(false);
		if (m_pItemDescBg2 && !m_pItemDescBg2->IsVisible())
			m_pItemDescBg2->SetElementFadeOut(false);

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pFurniture->typeId);
		sprintf(text, " [%s]", pIff->c.Name);

		m_pItemDesc2->ClearLine();
		m_pItemName->ClearLine();

		m_pItemName->AddText(text, false, true);

		if (pDesc)
			m_pItemDesc2->AddText(pDesc->Desc, false, true);

		m_itemDesc2Pos = pItem->pos;
	}
}

void CRealMyRoomMain::OnFurnitureListBtnDown()
{
	FrListItem* pItem = m_pFurnitureList->GetItemUnderCursor();
	if (!pItem)
		return;

	sFurniture_List* pFurniture = (sFurniture_List*)pItem->pData;

	if (!pItem->selected && (pFurniture->typeId >> 11 & 0x7ff) != 8 &&
		(pFurniture->typeId >> 11 & 0x7ff) != 9)
	{
		pItem->selected = true;

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		pForm->SetMessage(
			"\xbc\xb1\xc5\xc3\xc7\xd1 \xb0\xa1\xb1\xb8\xc0\xc7 \xbc\xb3\xc4\xa1\xb8\xa6 \xc1\xa6\xb0\xc5\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		pForm->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnFurnitrueListDeleteDlgResult,
			0);
		return;
	}

	for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin(); it != Doc()->m_furnitureList.end(); ++it) // NOFORMAT
	{
		int subType = (*it).typeId >> 11 & 0x7ff;

		if (subType == 8 || subType == 9)
		{
			if ((pFurniture->typeId >> 11 & 0x7ff) == subType &&
				(*it).id != pFurniture->id)
				(*it).bArrange = false;
		}
	}

	pFurniture->bArrange = true;
	int type = pFurniture->typeId >> 11 & 0x7ff;

	if (type == 14)
	{
		IFF_STRUCT::sFurniture* pIff =
			ItemManager()->FindFurniture(pFurniture->typeId);
		pFurniture->x = pIff->Pos[0];
		pFurniture->y = pIff->Pos[1];
		pFurniture->z = pIff->Pos[2];
		pFurniture->r = pIff->Pos[3];
	}

	if (CRealMyRoomData::Instance()->AddModel(*pFurniture))
	{
		pFurniture->bArrange = true;

		if (type == 14 && CRealMyRoomData::Instance()->m_pSelModel)
		{
			WVector screen = g_view->Projection(
				CRealMyRoomData::Instance()->m_pSelModel->m_pModel->mat.pivot);
			g_input->SetMousePoint(screen.x, screen.y);
		}

		if (m_furnitureTab == 0 && (type == 8 || type == 9))
		{
			int size = m_pFurnitureList->GetCurrentItemSize(true);

			for (int i = 0; i < size; i++)
			{
				FrListItem* pListItem = m_pFurnitureList->GetItem(i);
				if (pListItem && pListItem != pItem)
				{
					sFurniture_List* pListFurniture =
						(sFurniture_List*)pListItem->pData;
					if (pListFurniture &&
						(pListFurniture->typeId >> 11 & 0x7ff) == type &&
						pListItem->selected)
						m_pFurnitureList->UnselectItem(pListItem);
				}
			}
		}
	}
	else
	{
		pFurniture->bArrange = false;
		pItem->selected = false;
	}
}

void CRealMyRoomMain::OnBtnMenuInit(int param)
{
	m_pBtnMenu = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CRealMyRoomMain::OOnBtnMenuButtonUP()
{
	m_pBtnMenuList->SetVisible(true);
}

void CRealMyRoomMain::OnBtn_MenuListInit(int param)
{
	m_pBtnMenuList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pBtnMenuList)
	{
		m_pBtnMenuList->SetVisible(false);

		m_pBtnMenuList->UseDummy(true);
		m_pBtnMenuList->SetMultiSelect(false);

		SetListMenu();
	}
}

void CRealMyRoomMain::SetListMenu()
{
	int skipMenu[13];
	int skipCount;

	if (m_pBtnMenuList)
		m_pBtnMenuList->ClearItem();

	skipMenu[0] = 5;
	skipMenu[1] = 6;
	skipMenu[2] = 7;
	skipMenu[3] = 4;
	skipCount = 4;

	if (!IsLocalContent(S4_OFFLINE_SHOP) ||
		Doc()->IsControlServerService(0x80000))
		skipMenu[skipCount++] = 3;

	if (Doc()->IsControlServerService(0x40000))
		skipMenu[skipCount++] = 1;

	if (!IsLocalContent(S4_ITEM_STORAGE))
		skipMenu[skipCount++] = 11;

	switch (m_uiHideMode)
	{
	case 0:
	case 1:
		skipMenu[skipCount++] = 9;
		skipMenu[skipCount++] = 10;
		break;

	case 2:
		skipMenu[skipCount++] = 9;
		skipMenu[skipCount++] = 10;

	case 4:
		skipMenu[skipCount++] = 11;
		break;
	}

	bool bArrange = false;
	for (FURNITURE_LIST_IT it = Doc()->m_furnitureList.begin(); it != Doc()->m_furnitureList.end(); ++it)  // NOFORMAT
	{
		if (((*it).typeId & 0x3ff800) == 0x2800 && (*it).bArrange)
		{
			bArrange = true;
			break;
		}
	}

	if (!bArrange)
		skipMenu[skipCount++] = 3;

	if (m_pBtnMenuList)
	{
		for (int i = 0; i < 13; i++)
		{
			if (i == 12)
			{
				int remain = 2 - m_pBtnMenuList->GetCurrentItemSize(true) % 3;

				for (int j = 0; j < remain; j++)
					m_pBtnMenuList->AddItem(NULL);

				m_pBtnMenuList->AddItem((void*)13);
				break;
			}

			bool bSkip = false;
			for (int j = 0; j < skipCount; j++)
			{
				if (skipMenu[j] == i)
				{
					bSkip = true;
					break;
				}
			}

			if (!bSkip)
				m_pBtnMenuList->AddItem((void*)(i + 1));
		}
	}
}

void CRealMyRoomMain::OnBtn_MenuListOwnerDraw(int param)
{
	if (!m_pBtnMenuList)
		return;

	if (!m_pBtnMenuList->IsVisible())
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	FrListItem* pItem = (FrListItem*)param;
	if (pItem && pItem->pData)
	{
		int index = (int)pItem->pData - 1;

		const Bitmap* pIcon = m_pMenuIcon[index];
		if (pIcon)
		{
			pGDI->DrawTexture(pIcon,
				WRect(pItem->pos.x, pItem->pos.y, (float)pIcon->Width(),
					(float)pIcon->Height()),
				0xffffffff, 0);
		}

		if ((pItem->selected || pItem->underCursor) && index != 12)
		{
			const Bitmap* pText = m_pMenuText[index];
			if (pText)
			{
				pGDI->DrawTexture(pText,
					WRect(pItem->pos.x, pItem->pos.y, (float)pText->Width(),
						(float)pText->Height()),
					0xffffffff, 0);
			}
		}

		const Bitmap* pState = NULL;
		if (pItem->selected && index != 12)
			pState = m_pMenuDisable;
		else if (pItem->underCursor)
			pState = m_pMenuOver;

		if (pState)
		{
			pGDI->DrawTexture(pState,
				WRect(pItem->pos.x, pItem->pos.y, (float)pState->Width(),
					(float)pState->Height()),
				0xffffffff, 0);
		}
	}
}

void CRealMyRoomMain::OnBtn_MenuListBtnDown()
{
	if (m_pBtnMenuList)
	{
		FrListItem* pItem = m_pBtnMenuList->GetItemUnderCursor();
		if (pItem)
		{
			int menu = (int)pItem->pData;
			if (menu)
			{
				SelectMenu(menu - 1);
			}
		}
	}
}

void CRealMyRoomMain::OnBtn_MenuListRBtnDown()
{
}

void CRealMyRoomMain::OnBtn_MenuListMouseMove()
{
}

void CRealMyRoomMain::OnBtn_MenuListDClick()
{
}

void CRealMyRoomMain::SelectMenu(unsigned char menu)
{
	switch (menu)
	{
	case 0:
		if (GetFlags(1) == false)
		{
			ControlUiHideMode(1);

			if (m_bCardBookVisible == true)
				SetCardBookVisible(true);
		}
		else
		{
			ControlUiHideMode(2);
			SetCardBookVisible(false);
		}
		break;

	case 1:
		if (Doc()->IsControlServerService(0x40000))
		{
			this << MsgObject(this, 0x23,
				(int)"\xbf\xec\xc6\xed\xc7\xd4 \xb1\xe2\xb4\xc9\xc0\xc7 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		else if (m_pPostReciveDlg == NULL)
		{
			m_pPostReciveDlg = CreateForm<CPost_ReciveDlg>(
				g_pFresh->GetManager(), this, "postbox");
			FrForm::EnableTail(false);
			m_pPostReciveDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostDlgResult,
				FrESCAPE | FrRET_CANCEL);
			m_pPostReciveDlg->SetFadeout(false);
		}
		break;

	case 2:
		if (GetFlags(8) == false)
		{
			ControlUiHideMode(4);
			SetCardBookVisible(false);
		}
		else
		{
			ControlUiHideMode(2);
			SetCardBookVisible(false);
		}

		m_furnitureTab = (eFurniture_Tab)0;
		break;

	case 8:
		if (m_pAuthorityDlg == NULL)
		{
			m_pAuthorityDlg = CreateForm<CRealMyRoom_AuthorityDlg>(
				g_pFresh->GetManager(), this, "realmyroom_authority");
			FrForm::EnableTail(false);
			m_pAuthorityDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnAuthDlgResult,
				FrESCAPE | FrRET_CANCEL);
			m_pAuthorityDlg->SetFadeout(false);
		}
		break;

	case 9:
		if (GetFlags(8) == true)
		{
			SetCameraDisplayType(5);
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xb0\xa1\xb1\xb8 \xb9\xe8\xc4\xa1 \xb8\xf0\xb5\xe5\xbf\xa1\xbc\xad\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb1\xe2\xb4\xc9\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
		break;

	case 11:
		if (IsLocalContent(S4_ITEM_STORAGE))
		{
			if (IsItemStorageVisible() == false)
			{
				WSendPacket packet((enumClientPacket)0xd5);
				packet.Send(TO_GAME);

				Net_EquipDresserParts();
				Net_EquipCharacter();
				Net_EquipCaddie();
				Net_EquipItem();
				Net_EquipGear();
				Net_EquipSkin();
				Net_EquipMascot();
				if (IsLocalContent(S4_CARD_SYSTEM)) Doc()->SendEquipItemSlot(); // NOFORMAT
				else
					Doc()->SendEquipItemSlotByMascot();
			}
			else
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x234, 0, 0, 0, 0, 0);
			}
		}
		break;

	case 10:
		if (GetFlags(8) == true)
		{
			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
			{
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_bHide)
				{
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(
						false);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos =
						m_startPos;
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetPosition(m_startPos);
				}
				else
				{
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos =
						m_startPos;
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetPosition(m_startPos);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(
						true);
				}
			}
		}
		else
		{
			FrForm* pForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			pForm->SetMessage(
				"\xb0\xa1\xb1\xb8 \xb9\xe8\xc4\xa1 \xb8\xf0\xb5\xe5\xbf\xa1\xbc\xad\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb1\xe2\xb4\xc9\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
		break;

	case 12:
		if (m_pBtnMenuList)
			m_pBtnMenuList->SetVisible(false);
		break;
	}
}

void CRealMyRoomMain::SetUIState(unsigned long state)
{
	if (m_uiHideMode & state)
		m_uiHideMode &= ~state;
	else
		m_uiHideMode |= state;

	CRealMyRoomData::Instance()->m_pSelModel = NULL;

	ChangeFlags(state);
}

void CRealMyRoomMain::ChangeFlags(unsigned long flag)
{
	memset(m_bFlag, 0, sizeof(m_bFlag));

	switch (flag)
	{
	case 1:
		m_bFlag[0] = !m_bFlag[0];
		break;

	case 2:
		m_bFlag[1] = !m_bFlag[1];
		break;

	case 4:
		m_bFlag[2] = !m_bFlag[2];
		m_bFlag[0] = m_bFlag[2];
		m_bFlag[1] = m_bFlag[2];
		break;

	case 8:
		m_bFlag[3] = !m_bFlag[3];
		break;
	}
}

bool CRealMyRoomMain::GetFlags(unsigned long flag)
{
	switch (flag)
	{
	case 1:
		return m_bFlag[0];
	case 2:
		return m_bFlag[1];
	case 4:
		return m_bFlag[2];
	case 8:
		return m_bFlag[3];
	}

	return false;
}

void CRealMyRoomMain::UIDisplayControl(bool bShow)
{
	SetOverAndUnderBarVisible(bShow);

	if (bShow) SetCameraDisplayType(1);  // NOFORMAT
	else
		SetCameraDisplayType(4);
}

void CRealMyRoomMain::FurnitureDisplayControl(bool bShow)
{
	if (bShow)
		BuildDisplayStandFurniture();

	bool bFadeOut = !bShow;

	if (m_pFurnitureList)
		m_pFurnitureList->SetVisible(bShow);

	if (m_pFurnitureTabMain)
		m_pFurnitureTabMain->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureReset)
		m_pFurnitureReset->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureBack)
		m_pFurnitureBack->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureSave)
		m_pFurnitureSave->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureOut)
		m_pFurnitureOut->SetElementFadeOut(bFadeOut);

	if (m_pFurniturePup)
		m_pFurniturePup->SetElementFadeOut(bFadeOut);
	if (m_pFurniturePdown)
		m_pFurniturePdown->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureListNum)
	{
		m_pFurnitureListNum->SetVisible(bShow);
		m_pFurnitureListNum->ClearLine();
		m_pFurnitureListNum->AddText(MakeStr("%3d", m_furniturePage + 1), false,
			true);
	}

	if (m_pFurnitureListMaxNum)
	{
		m_pFurnitureListMaxNum->SetVisible(bShow);
		m_pFurnitureListMaxNum->ClearLine();
		m_pFurnitureListMaxNum->AddText(MakeStr("%3d", m_furnitureMaxPage),
			false, true);
	}

	if (m_pFurnitureBtnListNum)
		m_pFurnitureBtnListNum->SetElementFadeOut(bFadeOut);

	for (int i = 0; i < 8; i++)
	{
		if (m_pFurnitureBackGround[i])
			m_pFurnitureBackGround[i]->SetElementFadeOut(bFadeOut);
	}

	for (int j = 0; j < 3; j++)
	{
		if (m_pFurnitureScroll[j])
			m_pFurnitureScroll[j]->SetElementFadeOut(bFadeOut);
	}

	m_pFurnitureTabMain->Select(m_furnitureTab);
}

void CRealMyRoomMain::PostDlgCreate()
{
	if (m_pPostReciveDlg == NULL)
	{
		m_pPostReciveDlg = CreateForm<CPost_ReciveDlg>(g_pFresh->GetManager(),
			this, "postbox", NULL);
		FrForm::EnableTail(false);
		m_pPostReciveDlg->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostDlgResult, 0x81);
		m_pPostReciveDlg->SetFadeout(false);
	}
	else
	{
		m_pPostReciveDlg->OnInit();
	}
}

void CRealMyRoomMain::ControlUiHideMode(int mode)
{
	if (mode != m_uiHideMode && mode != 5)
		m_prevUiHideMode = m_uiHideMode;

	m_pPointFx->SetActive(false, true);

	switch (mode)
	{
	case 1:
		SetUIState(1);
		SetOverAndUnderBarVisible(true);
		SetCameraDisplayType(1);
		m_bShowCharacter = true;
		SetMiddleUIVisible(true);
		FurnitureDisplayControl(false);
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetState(2);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(false);
		}
		break;

	case 2:
		SetUIState(2);
		SetOverAndUnderBarVisible(true);
		m_bShowCharacter = false;
		SetMiddleUIVisible(false);
		FurnitureDisplayControl(false);
		SetCameraDisplayType(4);
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			CRealMyRoom_AvatarManager::Instance()
				->m_pMyAvatar->ChangeCharacterInfo();
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetState(0);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(true);
		}
		break;

	case 4:
		m_pPointFx->SetActive(true, true);
		SetUIState(8);
		SetOverAndUnderBarVisible(false);
		m_bShowCharacter = false;
		SetMiddleUIVisible(false);
		FurnitureDisplayControl(true);
		SetCameraDisplayType(4);
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			CRealMyRoom_AvatarManager::Instance()
				->m_pMyAvatar->ChangeCharacterInfo();
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetState(0);
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetHide(true);
		}
		break;

	case 5:
		if (m_prevUiHideMode == 1)
		{
			ControlUiHideMode(m_mainTab == 6 ? 1 : 2);
			return;
		}
		else
		{
			ControlUiHideMode(2);
			return;
		}
		break;
	}

	m_uiHideMode = mode;
	SetListMenu();
}

void CRealMyRoomMain::SetOverAndUnderBarVisible(bool bShow)
{
	bool bFadeOut = !bShow;

	if (bShow)
		ShowOverAndUnderBar(true);

	for (int i = 0; i < 3; i++)
	{
		if ((&m_pOverBar)[i] && (&m_pOverBar)[i]->IsVisible())
			(&m_pOverBar)[i]->SetElementFadeOut(bFadeOut);
	}

	if (bFadeOut)
		ShowOverAndUnderBar(false);
}

void CRealMyRoomMain::SetMiddleUIControl(bool bShow)
{
	if (bShow)
		SetCameraDisplayType(1);
	else
		SetCameraDisplayType(4);
}

void CRealMyRoomMain::SetMiddleUIVisible(bool bShow)
{
	bool bFadeOut = !bShow;

	if (bShow)
	{
		if (m_pMiddleUI)
			m_pMiddleUI->SetElementFadeOut(bFadeOut);
		ChangeMainTab(m_mainTab, true);
	}
	else
	{
		if (m_pMiddleUI && m_pMiddleUI->IsVisible())
			m_pMiddleUI->SetElementFadeOut(bFadeOut);
	}
}

void CRealMyRoomMain::Furniture_ShowUI(bool bShow)
{
	bool bFadeOut = !bShow;

	if (m_pFurnitureWnd[0])
		m_pFurnitureWnd[0]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureWnd[1])
		m_pFurnitureWnd[1]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[0])
		m_pFurnitureUI[0]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[1])
		m_pFurnitureUI[1]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[2])
		m_pFurnitureUI[2]->SetElementFadeOut(bFadeOut);

	if (m_pFurnitureUI[3])
		m_pFurnitureUI[3]->SetElementFadeOut(bFadeOut);

	if (m_pFurnitureUI[4])
		m_pFurnitureUI[4]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[5])
		m_pFurnitureUI[5]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[6])
		m_pFurnitureUI[6]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[7])
		m_pFurnitureUI[7]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureUI[8])
		m_pFurnitureUI[8]->SetElementFadeOut(bFadeOut);
}

void CRealMyRoomMain::Furniture_ShowUI_Left(bool bEnable)
{
	if (m_pFurnitureUI[4])
		m_pFurnitureUI[4]->Enable(bEnable);
	if (m_pFurnitureUI[5])
		m_pFurnitureUI[5]->Enable(bEnable);
	if (m_pFurnitureUI[6])
		m_pFurnitureUI[6]->Enable(bEnable);
	if (m_pFurnitureUI[7])
		m_pFurnitureUI[7]->Enable(bEnable);
	if (m_pFurnitureUI[8])
		m_pFurnitureUI[8]->Enable(bEnable);
}

void CRealMyRoomMain::OnEffectViewInit(int param)
{
	m_pEffectView = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CRealMyRoomMain::OnEffectViewOnwnerDraw(int param)
{
	if (g_pFresh->GetManager()->GetGDI())
	{
		const WRect& rect = m_pEffectView->GetRect();
		WOverlay::DrawBox(g_view, WRect(rect.x, rect.y, rect.w, rect.h), 0,
			0x18000000, 0);
	}
}

void CRealMyRoomMain::OnRightMenuInit(int param)
{
	m_pRightMenu = DYNAMIC_CAST(FrWnd, (FrWnd*)param);
	if (m_pRightMenu)
	{
		m_pRightMenu->SetVisible(false);
		m_pRightMenu->SetFixed(true);
	}
}

void CRealMyRoomMain::OnRightMenuLBtnDown()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		if (m_bItemStorageVisible == true)
			return;
	}

	const char* text = ((FrContextMenuCtrl*)m_pRightMenu)->GetSelectMenuText();

	if (IsLocalContent(S4_FURNITURE_ABILITY))
	{
		if (!strcmp(text, "\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb"))
		{
			FurnitureUse();
			return;
		}

		if (!strcmp(text, "\xb0\xa1\xb1\xb8\xb1\xb8\xb5\xbf"))
		{
			FurnitureRun();
			return;
		}
	}

	if (!strcmp(text, "\xb0\xa1\xb1\xb8\xbc\xb1\xc5\xc3"))
	{
		FurnitureFunctionChange();
		return;
	}

	if (!strcmp(text, "\xb0\xa1\xb1\xb8\xc0\xcc\xb5\xbf"))
	{
		CRealMyRoomData::Instance()->SetState(2);
		return;
	}

	if (!strcmp(text, "\xb0\xa1\xb1\xb8\xc8\xb8\xc0\xfc"))
	{
		CRealMyRoomData::Instance()->SetState(3);
		return;
	}

	if (!strcmp(text, "\xb0\xa1\xb1\xb8\xc1\xa6\xb0\xc5"))
	{
		FrListItem* pItem = m_pFurnitureList->FindItem(
			CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture);
		if (pItem)
			m_pFurnitureList->UnselectItem(pItem);

		for (std::list<sFurniture_List>::iterator it =
				 Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
		{
			if ((*it).id ==
				CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->id)
				(*it).bArrange = false;
		}

		CRealMyRoomData::Instance()->SetState(6);
		return;
	}
}

void CRealMyRoomMain::OnRightMenuRBtnDown()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		if (m_bItemStorageVisible == true)
			return;
	}

	m_pRightMenu->SetVisible(false);
}

void CRealMyRoomMain::UpdateRightMenu()
{
	if (CRealMyRoomData::Instance()->m_pSelModel)
	{
		int subType =
			(CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->typeId >>
				11) &
			0x7ff;
		if (subType >= 7 && subType <= 9)
		{
			m_pRightMenu->SetVisible(false);
			return;
		}

		if (MyUID() == Doc()->m_replayTargetUID ||
			!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			if (m_bFlag[3])
			{
				((FrContextMenuCtrl*)m_pRightMenu)
					->SetMenuList(
						"\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb@rmr_i_use|\xb0\xa1\xb1\xb8\xc0\xcc\xb5\xbf@rmr_i_move|separator|\xb0\xa1\xb1\xb8\xc8\xb8\xc0\xfc@rmr_i_rotate|separator|\xb0\xa1\xb1\xb8\xc1\xa6\xb0\xc5@rmr_i_delete");
			}
			else
			{
				IFF_STRUCT::sFurniture* pFurniture =
					ItemManager()->FindFurniture(CRealMyRoomData::Instance()
							->m_pSelModel->m_pFurniture->typeId);
				if (pFurniture->IsFunction)
				{
					((FrContextMenuCtrl*)m_pRightMenu)
						->SetMenuList(
							"\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb@rmr_i_use|\xb0\xa1\xb1\xb8\xbc\xb1\xc5\xc3@rmr_i_select");
				}
				else
				{
					m_pRightMenu->SetVisible(false);
				}
			}
		}
		else
		{
			if (subType == 5 || subType == 13)
			{
				if (CRealMyRoomData::Instance()
							->m_pSelModel->m_pFurniture->typeId == 0x48006820 ||
					CRealMyRoomData::Instance()
							->m_pSelModel->m_pFurniture->typeId == 0x48006821)
				{
					m_pRightMenu->SetVisible(false);
				}
				else
				{
					((FrContextMenuCtrl*)m_pRightMenu)
						->SetMenuList(
							"\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb@rmr_i_use|\xb0\xa1\xb1\xb8\xbc\xb1\xc5\xc3@rmr_i_select");
				}
			}
			else
			{
				m_pRightMenu->SetVisible(false);
			}
		}

		if (IsLocalContent(S4_FURNITURE_ABILITY) && subType == 13)
		{
			if (IsLocalContent(S4_FURNITURE_ABILITY))
			{
				if (CRealMyRoomData::Instance()
							->m_pSelModel->m_pFurniture->typeId == 0x48006820 ||
					CRealMyRoomData::Instance()
							->m_pSelModel->m_pFurniture->typeId == 0x48006821)
				{
					((FrContextMenuCtrl*)m_pRightMenu)
						->UnableMenuItem("\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb");
				}
				else
				{
					((FrContextMenuCtrl*)m_pRightMenu)
						->UnableMenuItem("\xb0\xa1\xb1\xb8\xbc\xb1\xc5\xc3");
				}
			}
		}
		else
		{
			((FrContextMenuCtrl*)m_pRightMenu)
				->UnableMenuItem("\xb0\xa1\xb1\xb8\xbb\xe7\xbf\xeb");
		}
	}
	else
	{
		m_pRightMenu->SetVisible(false);
	}
}

void CRealMyRoomMain::PopupRightMenu(int x, int y)
{
	if (MyUID() == Doc()->m_replayTargetUID ||
		!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		m_pRightMenu->MoveWindow(WPoint((float)x, (float)y));
		m_pRightMenu->SetVisible(true);
		m_pRightMenu->SetTopmost(true);
	}
	else if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
	{
		m_pRightMenu->MoveWindow(WPoint((float)x, (float)y));
		m_pRightMenu->SetVisible(true);
		m_pRightMenu->SetTopmost(true);
	}

	UpdateRightMenu();
}

void CRealMyRoomMain::ExecuteMiniButtonDown(sItemInfo* pItem)
{
	switch (pItem->tid)
	{
	case 0x1a000003:
		OnOverBar_OptionUp();
		break;

	case 0x1a00003b:
		if (IsLocalContent((localContentType_t)0x40))
			OpenNewYearMoney(pItem);
		break;

	case 0x1a00003e:
	case 0x1a000048:
		OpenEventGiftBox(pItem);
		break;

	case 0x1a000054:
	case 0x1a0000bc:
		OpenS4EventGiftBox(pItem);
		break;

	case 0x1a000052:
		if (IsLocalContent((localContentType_t)0x24))
			OpenEventXmasGiftBox(pItem);
		break;

	case 0x1a000105:
	case 0x1a00014b:
		if (g_pFresh->GetManager()->GetDesktop()->FindChildForm(
				"FrSupplyPackDlg") == NULL)
		{
			S5::FrSupplyPackDlg* pDlg = CreateForm<S5::FrSupplyPackDlg>(
				g_pFresh->GetManager(), NULL, "FrSupplyPackDlg", NULL);
			pDlg->Initialize(pItem->tid);
			pDlg->EnableDrag(false);
			pDlg->Open(NULL, 0);
		}
		break;

	case 0x1a00012f:
	case 0x1a000130:
	case 0x1a000131:
	{
		IActor* pActor = GetActor(s_guildActor[1]);
		if (s_guildActor[1])
			pActor << MsgObject(NULL, 0x263, 0x1b, pItem->tid, 0, 0, 0);
	}
	break;

	case 0x1a00015b:
	{
		IActor* pActor = GetActor(s_gatewayActor[1]);
		pActor << MsgObject(NULL, 0x276, 0, 0, 0, 0, 0);
	}
	break;

	default:
	{
		IFF_STRUCT::sItem* pIffItem = ItemManager()->FindItem(pItem->tid);
		if (pIffItem)
		{
			if (pIffItem->IsRandomBox())
				OpenNewRandomBox(pItem);
		}

		if (FrCardRemove::IsCardRemoveItem(pItem->tid))
		{
			if (m_pCardRemoveDlg == NULL)
			{
				m_pCardRemoveDlg = CreateForm<FrCardRemove>(
					g_pFresh->GetManager(), this, "card_remove", NULL);
				m_pCardRemoveDlg->SetCardRemoveItem(pItem->tid, pItem->guid);
				m_pCardRemoveDlg->SetCaption(
					"\xc4\xab\xb5\xe5\xc3\xdf\xc3\xe2\xbe\xd7");
				m_pCardRemoveDlg->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnCardRemoveDlgResult,
					0);
			}
		}
	}
	break;
	}
}

void CRealMyRoomMain::OnUnderBar_BackUp()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		if (IsItemStorageVisible() == true)
			SetItemStorageVisible(false);
		else
			CTaskMain::OnUnderBar_BackUp();
	}
	else
	{
		CTaskMain::OnUnderBar_BackUp();
	}
}

void CRealMyRoomMain::FurnitureUse()
{
	if (!IsLocalContent(S4_FURNITURE_ABILITY))
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	unsigned long typeId =
		CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->typeId;

	if ((typeId & 0xfc000000) == 0x48000000 && (typeId & 0x3ff800) == 0x6800)
	{
		unsigned long uid =
			CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->id;

		if (!CRealMyRoomData::Instance()->IsArragementFurniture(uid))
		{
			if (MyUID() == Doc()->m_replayTargetUID)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				pForm->SetMessage(
					"\xb9\xe8\xc4\xa1\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xb9\xe8\xc4\xa1\xc7\xd1 \xb4\xd9\xc0\xbd \xc0\xfa\xc0\xe5\xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc0\xfa\xc0\xe5\xc7\xcf\xbd\xc5 \xc8\xc4\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(NULL, 3);
				return;
			}

			WSendPacket packet((enumClientPacket)0xd9);
			packet.Encode4(uid);
			packet.Encode4(-1);
			packet.Send(TO_GAME);
		}
		else
		{
			WSendPacket packet((enumClientPacket)0xd9);
			packet.Encode4(uid);
			packet.Encode4(-1);
			packet.Send(TO_GAME);
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb0\xa1\xb1\xb8 \xbb\xe7\xbf\xeb \xb9\xd7 \xb1\xb8\xb5\xbf \xb8\xde\xb4\xba \xbf\xad\xb1\xe2\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
	}
}

void CRealMyRoomMain::FurnitureRun()
{
	if (!IsLocalContent(S4_FURNITURE_ABILITY))
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	if (m_pFurnitureAbilityDlg == NULL)
	{
		m_pFurnitureAbilityDlg = CreateForm<CFurniture_AbilityDlg>(
			g_pFresh->GetManager(), this, "myitem_choice", NULL);
		FrForm::EnableTail(false);
		m_pFurnitureAbilityDlg->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnFurniture_AbilityDlgResult,
			0x81);
		m_pFurnitureAbilityDlg->SetFadeout(false);
	}
}

void CRealMyRoomMain::FurnitureFunctionChange()
{
	IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(
		CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->typeId);

	if (pFurniture->IsFunction > 100)
	{
		if (MyUID() == Doc()->m_replayTargetUID ||
			!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
			SelectMenu(3);
		else
			SelectMenu(4);
	}
	else
	{
		switch (pFurniture->IsFunction)
		{
		case 1:
			SelectMenu(0);
			break;

		case 2:
			PostDlgCreate();
			break;

		case 5:
			if (IsLocalContent(S4_ITEM_STORAGE) == true)
				SelectMenu(11);
			break;
		}
	}
}

void CRealMyRoomMain::OnUnderBar_ItemStorageUp()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true)
		SelectMenu(11);

	CIconToolTip::GetInstance()->ClearToolTip();
}

void CRealMyRoomMain::OnUnderBar_MyRoomUp()
{
	if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb4\xd9\xb8\xa5\xc0\xaf\xc0\xfa\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae \xc1\xdf\xc0\xce \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xb8\xb7\xce \xb0\xa5 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	CTaskMain::OnUnderBar_MyRoomUp();
}

void CRealMyRoomMain::OnUnderBar_GiftUp()
{
	if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb4\xd9\xb8\xa5\xc0\xaf\xc0\xfa\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae \xc1\xdf\xc0\xce \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xbc\xb1\xb9\xb0\xc7\xd4\xc0\xb8\xb7\xce \xb0\xa5 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	SelectMenu(1);
}

bool CRealMyRoomMain::OnPostDlgResult(int result, FrForm* form)
{
	if (form)
		FrForm::EnableTail(true);

	SetListMenu();
	m_pPostReciveDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnFurniture_AbilityDlgResult(int result, FrForm* form)
{
	if (form)
		FrForm::EnableTail(true);

	if (result == 1)
	{
		unsigned long typeId =
			CRealMyRoomData::Instance()->m_pSelModel->m_pFurniture->typeId;

		if ((typeId & 0xfc000000) == 0x48000000 &&
			(typeId & 0x3ff800) == 0x6800)
		{
			if (m_pFurnitureAbilityDlg)
			{
				sItemInfo* pInfo = NULL;
				m_pFurnitureAbilityDlg->GetItemInfo(pInfo);
				if (pInfo == NULL)
				{
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify", NULL);
					pForm->SetMessage(
						"\xb0\xa1\xb1\xb8 \xb1\xb8\xb5\xbf\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						false);
					pForm->Open(NULL, 3);
				}
				else if (ItemManager()->FindCommonItem(pInfo->tid))
				{
					FurnitureUse();
				}
			}
			else
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				pForm->SetMessage(
					"\xb0\xa1\xb1\xb8 \xb1\xb8\xb5\xbf\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(NULL, 3);
			}
		}
		else
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pForm->SetMessage(
				"\xb0\xa1\xb1\xb8 \xbb\xe7\xbf\xeb \xb9\xd7 \xb1\xb8\xb5\xbf \xb8\xde\xb4\xba \xbf\xad\xb1\xe2\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xb0\xa1\xb1\xb8\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xc3\xdf\xb0\xa1\xc7\xcf\xb1\xe2\xb8\xa6 \xc1\xdf\xb4\xdc\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
	}

	m_pFurnitureAbilityDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnPostSendDlgResult(int result, FrForm* form)
{
	if (form)
		FrForm::EnableTail(true);

	if (result == 1)
	{
		if (m_pPostSendDlg)
		{
			WSendPacket packet((enumClientPacket)0xbb);
			packet.Encode4(MyUID());
			packet.Encode4(m_pPostSendDlg->GetSendUID());

			packet.Encode1(m_pPostSendDlg->GetSendListSize());
			packet.Encode8(m_pPostSendDlg->GetSendServiceCharge());
			packet.EncodeStr("");
			packet.EncodeStr(m_pPostSendDlg->GetLetterMsg());

			sMailIncludeItem item;
			std::list<sMailIncludeItem>* pList = m_pPostSendDlg->GetSendList();

			std::list<sMailIncludeItem>::iterator it;
			for (it = pList->begin(); it != pList->end(); it++)
			{
				item = *it;
				packet.EncodeBuffer(&item, 0x35);
			}

			packet.Send(TO_GAME);

			m_pPostSendDlg = NULL;
		}
	}
	else
	{
		m_pPostReciveDlg = NULL;
		m_pPostSendDlg = NULL;

		m_pPostReciveDlg = CreateForm<CPost_ReciveDlg>(g_pFresh->GetManager(),
			this, "postbox", NULL);
		FrForm::EnableTail(false);
		m_pPostReciveDlg->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostDlgResult, 0x81);
		m_pPostReciveDlg->SetFadeout(false);
	}

	return true;
}

bool CRealMyRoomMain::OnPostSendDlg2Result(int result, FrForm* form)
{
	m_pPostSendDlg = NULL;

	if (result == 1)
	{
		m_pPostSendDlg = CreateForm<CPost_SendDlg>(g_pFresh->GetManager(), this,
			"write_post", NULL);
		FrForm::EnableTail(false);
		m_pPostSendDlg->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnPostSendDlgResult, 0x81);
		m_pPostSendDlg->SetFadeout(false);
	}

	return true;
}

bool CRealMyRoomMain::OnAuthDlgResult(int result, FrForm* form)
{
	if (form)
		FrForm::EnableTail(true);

	if (result == 1)
	{
		if (m_pAuthorityDlg)
		{
			bool bAuth = m_pAuthorityDlg->GetAuthSetting();
			m_pAuthorityDlg->GetPassWord();

			if (strlen(Doc()->m_rmrAuthority.password) == 0 && bAuth == true)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"rmr_notify", NULL);
				pForm->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3 \xc0\xd4\xb7\xc2\xc0\xcc \xb5\xc7\xc1\xf6 \xbe\xca\xbe\xc6\xbc\xad \xb1\xc7\xc7\xd1 \xbc\xb3\xc1\xa4\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				pForm->Open(
					(FRESH_PFN_RESULT)&CRealMyRoomMain::OnAuthDlgFaileResult,
					2);
			}
			else
			{
				Doc()->m_rmrAuthority.bPrivate =
					m_pAuthorityDlg->GetAuthSetting();

				WSendPacket packet((enumClientPacket)0xb0);
				packet.EncodeBuffer(&Doc()->m_rmrAuthority, 0x6b);
				packet.Send(TO_GAME);
			}
		}
	}

	SetListMenu();
	m_pAuthorityDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnAuthDlgFaileResult(int result, FrForm* form)
{
	if (result == 1)
		SelectMenu(8);

	return true;
}

bool CRealMyRoomMain::GetArrangement(unsigned long typeId, unsigned long uid)
{
	if ((typeId & 0xfc000000) != 0x48000000)
		return false;

	for (cModel* pModel = CRealMyRoomData::Instance()->m_modelList.Start(); pModel; pModel = CRealMyRoomData::Instance()->m_modelList.Next()) // NOFORMAT
	{
		if (pModel)
		{
			if (pModel->m_pFurniture->id == uid)
				return true;
		}
	}

	return false;
}

void CRealMyRoomMain::RMR_UpdateDB()
{
	std::list<sFurniture_List> updateList;

	if (Doc()->m_furnitureList.size() <= 0)
		return;

	updateList.clear();

	for (std::list<sFurniture_List>::iterator it =
			 Doc()->m_furnitureList.begin();
		it != Doc()->m_furnitureList.end(); ++it)
	{
		IFF_STRUCT::sFurniture* pFurniture =
			ItemManager()->FindFurniture((*it).typeId);
		if (pFurniture == NULL)
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pForm->SetMessage(
				"\xc7\xf6\xc0\xe7\xb4\xc2 \xc0\xfa\xc0\xe5\xc7\xd2\xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
			continue;
		}

		(*it).bArrange = false;

		sFurniture_List furniture;
		cModel* pModel =
			CRealMyRoomData::Instance()->GetModelData((*it).id, (*it).typeId);

		if (pModel && (pModel->m_pFurniture->typeId & 0xfc000000) == 0x48000000)
		{
			pModel->m_pFurniture->bArrange = true;
			(*it).bArrange = true;

			furniture = *pModel->m_pFurniture;
			if (pModel->m_pModel)
			{
				furniture.x = pModel->m_pModel->pos.x;
				furniture.y = pModel->m_pModel->pos.y;
				furniture.z = pModel->m_pModel->pos.z;
			}

			int change =
				CRealMyRoomData::Instance()->IsFunritureValueChange(*pModel);

			if (change == -1 || (change > 0 && change <= 2))
				updateList.push_back(furniture);
		}
		else
		{
			if (CRealMyRoomData::Instance()->IsModelData_SaveList((*it).id,
					(*it).typeId))
			{
				memset(&furniture, 0, sizeof(furniture));
				furniture.typeId = (*it).typeId;
				furniture.id = (*it).id;
				furniture.x = (*it).x;
				furniture.y = (*it).y;
				furniture.z = (*it).z;
				furniture.r = (*it).r;
				furniture.bArrange = false;

				updateList.push_back(furniture);
			}
		}
	}

	if (updateList.size() <= 0)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xba\xaf\xb0\xe6\xc7\xd1 \xb3\xbb\xbf\xeb\xc0\xcc \xbe\xf8\xc0\xb8\xb9\xc7\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 3);
	}
	else
	{
		WSendPacket packet(0xae);
		packet.Encode2(updateList.size());

		for (std::list<sFurniture_List>::iterator it = updateList.begin(); it != updateList.end(); ++it) // NOFORMAT
			packet.EncodeBuffer(&(*it), 0x1b);

		packet.Send(TO_GAME);
	}
}

void CRealMyRoomMain::LoadCardImages()
{
	for (CardIterator it = ItemManager()->m_CardMap.begin(); it != ItemManager()->m_CardMap.end(); ++it) // NOFORMAT
	{
		if (m_cardImg.find((*it).second.c.TypeId) == m_cardImg.end())
		{
			char filename[64];
			sprintf(filename, "%s.png", (*it).second.Image);
			WOverlay* pOverlay = g_resrcmng->GetOverlay(filename, 0x80000000);

			if (pOverlay)
				m_cardImg.insert(std::map<unsigned int, WOverlay*>::value_type(
					(*it).second.c.TypeId, pOverlay));
		}
	}
}

void CRealMyRoomMain::DeleteCardImages()
{
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cardImg.begin(); it != m_cardImg.end(); ++it) // NOFORMAT
	{
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	}
}

WOverlay* CRealMyRoomMain::GetCardImg(unsigned long typeId)
{
	WOverlay* pOverlay = NULL;

	std::map<unsigned int, WOverlay*>::iterator it = m_cardImg.find(typeId);
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);

	if (it == m_cardImg.end())
	{
		pCard = ItemManager()->FindCard(typeId);

		if (pCard)
		{
			char filename[64];
			sprintf(filename, "%s.png", pCard->Image);
			pOverlay = g_resrcmng->GetOverlay(filename, 0);
			if (pOverlay)
				m_cardImg.insert(std::map<unsigned int, WOverlay*>::value_type(
					pCard->c.TypeId, pOverlay));
		}
	}
	else
	{
		pOverlay = (*it).second;
	}

	return pOverlay;
}

void CRealMyRoomMain::DrawCard()
{
	if (m_cardTypeId)
	{
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_cardTypeId);

		if (pCard)
		{
			WOverlay* pOverlay = GetCardImg(m_cardTypeId);
			if (pOverlay)
				pOverlay->Render(g_view, WRect(0, 0, 1.0, 1.0),
					WRect(92.0f, 117.0f, 248.0f, 499.0f), 0, 0xffffffff, 0, 0);
		}
	}
}

void CRealMyRoomMain::DrawPartsSlot(FrGraphicInterface* pGDI, FrListItem* pItem)
{
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;
	if (pInfo == NULL)
		return;

	if (pGDI == NULL)
	{
		pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI == NULL)
			return;
	}

	int i;
	float charX = 5.0f;
	float caddieX = 5.0f;

	if (pInfo->CharacterSlotNum == 0 && pInfo->CaddieSlotNum == 0 &&
		m_pMarkBegin)
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
		if (pPart)
		{
			if (pPart->Beginner == 1)
			{
				pGDI->DrawTexture(m_pMarkBegin,
					WRect(pItem->pos.x + 5.0f, pItem->pos.y + 65.0f,
						(float)m_pMarkBegin->Width(),
						(float)m_pMarkBegin->Height()),
					0xffffffff, 0);
			}
		}
	}

	if (pInfo->CharacterSlotNum <= -1 || pInfo->CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
		if (pPart)
		{
			pInfo->CharacterSlotNum = pPart->CharacterSlot;
			if (pInfo->CharacterSlotNum <= -1 || pInfo->CharacterSlotNum >= 5)
				pInfo->CharacterSlotNum = 0;
		}
		else
		{
			pInfo->CharacterSlotNum = 0;
		}
	}

	for (i = 1; i < pInfo->CharacterSlotNum + 1; i++)
	{
		bool bFound = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (pInfo->tid == (*it).partsTid && pInfo->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard((*it).tid);
				if (pCard)
				{
					if (i == (*it).slotNum)
					{
						if ((*it).cardType == 0)
						{
							bFound = true;
							const Bitmap* pBitmap =
								g_pFresh->GetBitmap(pCard->SlotImg);
							if (pBitmap)
							{
								pGDI->DrawTexture(pBitmap,
									WRect(charX + pItem->pos.x,
										pItem->pos.y + 54.0f,
										(float)pBitmap->Width(),
										(float)pBitmap->Height()),
									0xffffffff, 0);
								charX += 16.0f;
							}
						}
					}
				}
			}
		}

		if (!bFound)
		{
			const Bitmap* pBitmap = g_pFresh->GetBitmap("card_character_empty");
			if (pBitmap)
			{
				pGDI->DrawTexture(pBitmap,
					WRect(charX + pItem->pos.x, pItem->pos.y + 54.0f,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					0xffffffff, 0);
				charX += 16.0f;
			}
		}
	}

	if (pInfo->CaddieSlotNum <= -1 || pInfo->CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->tid);
		if (pPart)
		{
			pInfo->CaddieSlotNum = pPart->CaddieSlot;
			if (pInfo->CaddieSlotNum <= -1 || pInfo->CaddieSlotNum >= 5)
				pInfo->CaddieSlotNum = 0;
		}
		else
		{
			pInfo->CaddieSlotNum = 0;
		}
	}

	for (i = 1; i < pInfo->CaddieSlotNum + 1; i++)
	{
		bool bFound = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (pInfo->tid == (*it).partsTid && pInfo->guid == (*it).partsUid)
			{
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard((*it).tid);
				if (pCard)
				{
					if (i == (*it).slotNum)
					{
						if ((*it).cardType == 1)
						{
							bFound = true;
							const Bitmap* pBitmap =
								g_pFresh->GetBitmap(pCard->SlotImg);
							if (pBitmap)
							{
								pGDI->DrawTexture(pBitmap,
									WRect(caddieX + pItem->pos.x,
										pItem->pos.y + 70.0f,
										(float)pBitmap->Width(),
										(float)pBitmap->Height()),
									0xffffffff, 0);
								caddieX += 16.0f;
								break;
							}
						}
					}
				}
			}
		}

		if (!bFound)
		{
			const Bitmap* pBitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (pBitmap)
			{
				pGDI->DrawTexture(pBitmap,
					WRect(caddieX + pItem->pos.x, pItem->pos.y + 70.0f,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.0f;
			}
		}
	}
}

bool CRealMyRoomMain::OnCardDlgResult(int result, FrForm* form)
{
	if (result == 1 && form)
	{
		FrCardUseDlg* pDlg = (FrCardUseDlg*)form;
		unsigned long cardType = (pDlg->m_cardTypeId >> 22) & 0xf;
		if (cardType == 2)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

			WSendPacket packet((enumClientPacket)0xb5);
			packet.Encode4(pDlg->m_cardTypeId);
			packet.Send(TO_GAME);
		}
		else if (cardType == 0 || cardType == 1)
		{
			m_pItemAttachCardDlg = CreateForm<FrItemAttachCardDlg>(
				g_pFresh->GetManager(), this, "itemattachcarddlg");
			m_pItemAttachCardDlg->m_cardId = pDlg->m_cardId;
			m_pItemAttachCardDlg->m_cardTypeId = pDlg->m_cardTypeId;
			m_pItemAttachCardDlg->m_cardType = (pDlg->m_cardTypeId >> 22) & 0xf;
			m_pItemAttachCardDlg->Open(
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnItemAttachCardDlgResult,
				1);
		}
	}

	m_pCardUseDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnCardPackDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrOpenCardPackDlg* pDlg = (FrOpenCardPackDlg*)form;
		if (pDlg)
		{
			if ((pDlg->m_cardTypeId & 0x3c00000) == 0xc00000)
			{
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 1, 0, 0, 0, 0, 0));

				WSendPacket packet((enumClientPacket)0xc2);
				packet.Encode4(pDlg->m_cardTypeId);
				packet.Encode4(pDlg->m_cardId);
				packet.Send(TO_GAME);
			}
		}
	}

	m_pOpenCardPackDlg = NULL;
	return true;
}

bool CRealMyRoomMain::OnItemAttachCardDlgResult(int result, FrForm* form)
{
	if (form)
	{
		if (result == 1)
		{
			FrItemAttachCardDlg* pDlg = (FrItemAttachCardDlg*)form;

			if (pDlg->m_selectSlot == 0xff)
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xc6\xc4\xc3\xf7\xc0\xc7 \xbd\xbd\xb7\xd4\xc0\xcc \xbc\xb1\xc5\xc3\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return true;
			}

			if (((pDlg->m_cardTypeId >> 22) & 0xf) == 0 &&
					pDlg->m_selectSlot > 0 && pDlg->m_selectSlot < 5 ||
				((pDlg->m_cardTypeId >> 22) & 0xf) == 1 &&
					pDlg->m_selectSlot > 4 && pDlg->m_selectSlot < 9)
			{
				sCards card;
				card.uid = pDlg->m_cardId;
				card.tid = pDlg->m_cardTypeId;
				card.partTid = pDlg->m_itemTypeId;
				card.partUid = pDlg->m_itemId;
				card.selectedCardSlotNum = pDlg->m_selectSlot;

				WSendPacket packet((enumClientPacket)0xc0);
				packet.EncodeBuffer(&card, sizeof(sCards));
				packet.Send(TO_GAME);

				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			}
			else
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xbd\xbd\xb7\xd4\xb0\xfa \xc4\xab\xb5\xe5\xc0\xc7 \xc5\xb8\xc0\xd4\xc0\xcc \xb4\xde\xb6\xf3\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
			}
		}
	}

	return true;
}

bool CRealMyRoomMain::OnResultDlg_Report(int result, FrForm* form)
{
	m_pUniteResultDlg = NULL;

	return true;
}

bool CRealMyRoomMain::OnUccDrawDlgResult(int result, FrForm* form)
{
	m_pUccDrawDlg = NULL;
	unsigned long charTypeId = Doc()->GetMyPartTidList().m_charTid;
	m_pItemList->SetVisible(true);
	if (g_view)
	{
		ShowPet(charTypeId);
	}

	return true;
}

bool CRealMyRoomMain::OnUccCopyDlgResult(int result, FrForm* form)
{
	m_pUccCopyDlg = NULL;
	m_pItemList->SetVisible(true);

	return true;
}

void CRealMyRoomMain::OpenEventXmasGiftBox(sItemInfo* pItem)
{
	if (pItem)
	{
		CGiftBoxEvent* pEvent =
			(CGiftBoxEvent*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)0x24);

		if (pEvent)
		{
			m_xmasGiftUid = pItem->guid;

			pEvent->SetOpenedBoxTID(pItem->tid);

			if (pEvent->GetOpenedBoxTID())
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_yesno");
				pForm->SetMessage(
					"\xc5\xa9\xb8\xae\xbd\xba\xb8\xb6\xbd\xba \xbc\xb1\xb9\xb0 \xbb\xf3\xc0\xda\xb8\xa6 \xbf\xad\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					false);
				pForm->Open((FRESH_PFN_RESULT)&CRealMyRoomMain::
								OnOpenEventXmasGiftDlgResult,
					1);
			}
		}
	}
}

bool CRealMyRoomMain::OnOpenEventXmasGiftDlgResult(int result, FrForm* form)
{
	return true;
}

bool CheckRandomBoxTypeID(unsigned long typeId)
{
	IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(typeId);

	if (pItem && pItem->IsRandomBox())
		return true;

	return false;
}

void CRealMyRoomMain::OpenNewRandomBox(sItemInfo* pItem)
{
	if (pItem)
	{
		if (CheckRandomBoxTypeID(pItem->tid))
		{
			m_boxTypeId = pItem->tid;
			OnOpenedBoxDlg(pItem->tid,
				(FRESH_PFN_RESULT)&CRealMyRoomMain::OnOpenNewRandomBoxDlgResult,
				this);
		}
		else
		{
			std::string msg =
				"\xbf\xad \xbc\xf6 \xbe\xf8\xb4\xc2 \xbc\xb1\xb9\xb0 \xbb\xf3\xc0\xda\xc0\xd4\xb4\xcf\xb4\xd9.";
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x26, (int)msg.c_str(), 0, 0, 0, 0));
		}
	}
}

bool CRealMyRoomMain::OnOpenNewRandomBoxDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_boxTypeId)
		{
			FrRandomBoxProgress* pDlg = CreateForm<FrRandomBoxProgress>(
				g_pFresh->GetManager(), this, "newRandomBox");
			if (!pDlg)
				return false;

			pDlg->SetBoxTid(m_boxTypeId);

			const char* name = ItemManager()->GetItemName(m_boxTypeId);
			if (name)
			{
				pDlg->SetExplane(MakeStr(
					"%\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbf\xa9\xb4\xc2 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					name));
			}

			pDlg->SetFixed(true);
			pDlg->Open(NULL, 1);
		}
	}

	m_boxTypeId = 0;

	return true;
}

void CRealMyRoomMain::OpenItemStorageNotice()
{
	if (!m_pItemStorageNotice)
		m_pItemStorageNotice = CreateForm<FrItemStorageNotice>(
			g_pFresh->GetManager(), this, "item_storage_notice");

	if (m_pItemStorageNotice)
	{
		m_pItemStorageNotice->SetCaption("\xbe\xcb\xb8\xb2");
		m_pItemStorageNotice->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnResultItemStorageNotice, 9);
	}
}

bool CRealMyRoomMain::OnResultItemStorageNotice(int result, FrForm* form)
{
	OpenItemStorageAccessPanel(0);
	m_pItemStorageNotice = NULL;

	return true;
}

void CRealMyRoomMain::OpenItemStorageAccessPanel(unsigned char type)
{
	if (!m_pItemStorageAccessPanel)
		m_pItemStorageAccessPanel = CreateForm<FrItemStorageAccessPanel>(
			g_pFresh->GetManager(), this, "item_storage_access_panel");

	if (m_pItemStorageAccessPanel)
	{
		WRect rect = m_pItemStorageAccessPanel->GetRect();

		rect.x = 100.0f;
		rect.y = 75.0f;
		m_pItemStorageAccessPanel->SetRect(rect);

		m_pItemStorageAccessPanel->SetFormType(
			(FrItemStorageAccessPanel::eFormType)type);
		m_pItemStorageAccessPanel->Open(
			(FRESH_PFN_RESULT)&CRealMyRoomMain::OnResultItemStorageAccessPanel,
			9);
	}
}

bool CRealMyRoomMain::OnResultItemStorageAccessPanel(int result, FrForm* form)
{
	bool bRet = true;

	if (form)
	{
		if (result == 1)
		{
			((FrItemStorageAccessPanel*)form)->SendPasswordToServer();
		}
		else
		{
			if (((FrItemStorageAccessPanel*)form)->GetFormType() ==
				FrItemStorageAccessPanel::eFORM_INIT_PASSWORD)
				bRet = false;
		}
	}

	m_pItemStorageAccessPanel = NULL;

	return bRet;
}

void CRealMyRoomMain::OnInitItemStorageBGI(int param)
{
	m_pItemStorageBG = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	CItemStorage* pStorage = CItemStorage::Instance();

	if (pStorage)
	{
		pStorage->AddFrontUI(m_pItemStorageBG);

		FrButton** ppButton = &m_pUnderBarBack;
		for (int i = 0; i < 21; ++i, ++ppButton)
		{
			if (*ppButton && i != 3)
			{
				pStorage->AddBackGroundUI(*ppButton, false);
			}
		}

		pStorage->AddBackGroundUI(m_pOverBarCaddie, false);
		pStorage->AddBackGroundUI(m_pOverBarCaddieSel, false);
		pStorage->AddBackGroundUI(m_pOverBarAztec, false);
		pStorage->AddBackGroundUI(m_pOverBarAztecSel, false);
		pStorage->AddBackGroundUI(m_pOverBarClub, false);
		pStorage->AddBackGroundUI(m_pOverBarClubSel, false);
		pStorage->AddBackGroundUI(m_pOverBarMascot, false);
		pStorage->AddBackGroundUI(m_pOverBarMascotSel, false);

		pStorage->AddBackGroundUI(m_pOverBarOneLineReqBtn, false);

		pStorage->AddBackGroundUI(m_pUnderBarTabBtn, false);

		pStorage->AddBackGroundUI(m_pUpgrade, false);
		pStorage->AddBackGroundUI(m_pDowngrade, false);

		pStorage->AddBackGroundUI(m_pSelItem, false);

		pStorage->AddBackGroundUI(m_pPet, true);

		pStorage->AddBackGroundUI(m_pFurnitureArrange, false);

		pStorage->AddBackGroundUI(m_pBtnMenu, true);
		pStorage->AddBackGroundUI(m_pBtnMenuList, true);
	}

	if (!Doc()->m_bItemStoragePin)
	{
		if (IsLocalContent(S4_ITEM_STORAGE))
		{
			if (!m_bItemStorageVisible)
			{
				if (!Doc()->m_bItemStoragePin)
				{
					WSendPacket packet((enumClientPacket)0xd5);
					packet.Send(TO_GAME);

					Net_EquipDresserParts();
					Net_EquipCharacter();
					Net_EquipCaddie();
					Net_EquipItem();
					Net_EquipGear();
					Net_EquipSkin();
					Net_EquipMascot();
					if (IsLocalContent(S4_CARD_SYSTEM)) Doc()->SendEquipItemSlot(); // NOFORMAT
					else
						Doc()->SendEquipItemSlotByMascot();
				}
			}
			else
			{
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0x234, 0, 0, 0, 0, 0);
			}
		}
	}
}

void CRealMyRoomMain::OnOwnerDrawItemStorageBGI(int param)
{
	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->DrawBackgroundImage();
	}
}

void CRealMyRoomMain::OnInitItemStoragePang(int param)
{
	m_pItemStoragePang = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->AddFrontUI(m_pItemStoragePang);
	}
}

void CRealMyRoomMain::OnOwnerDrawItemStoragePang(int param)
{
}

void CRealMyRoomMain::OnInitItemStoragePangMinus(int param)
{
	m_pItemStoragePangMinus = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pItemStoragePangMinus)
	{
		m_pItemStoragePangMinus->SetPushDelay(0.1f);

		if (CItemStorage::Instance())
		{
			CItemStorage::Instance()->AddFrontUI(m_pItemStoragePangMinus);
		}
	}
}

void CRealMyRoomMain::OnLBDownItemStoragePangMinus()
{
	if (CItemStorage::Instance())
	{
		if (!CItemStorage::Instance()->IsLock())
		{
			CItemStorage::Instance()->OpenPangBank(
				FrItemStoragePangBank::eTYPE_TAKE);
		}
	}
}

void CRealMyRoomMain::OnInitItemStoragePangPlus(int param)
{
	m_pItemStoragePangPlus = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pItemStoragePangPlus)
	{
		m_pItemStoragePangPlus->SetPushDelay(0.1f);

		if (CItemStorage::Instance())
		{
			CItemStorage::Instance()->AddFrontUI(m_pItemStoragePangPlus);
		}
	}
}

void CRealMyRoomMain::OnLBDownItemStoragePangPlus()
{
	if (CItemStorage::Instance())
	{
		if (!CItemStorage::Instance()->IsLock())
		{
			CItemStorage::Instance()->OpenPangBank(
				FrItemStoragePangBank::eTYPE_STORE);
		}
	}
}

void CRealMyRoomMain::OnInitItemStorageList(int param)
{
	m_pItemStorageList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pItemStorageList)
	{
		m_pItemStorageList->UseDblClick(true);
		m_pItemStorageList->UseRightButton(true);

		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage)
		{
			pStorage->AddFrontUI(m_pItemStorageList);
			pStorage->SetItemStorageArea(m_pItemStorageList->GetRect());
		}
	}
}

void CRealMyRoomMain::OnOwnerDrawItemStorageList(int param)
{
	CItemStorage* pStorage = CItemStorage::Instance();

	if (pStorage)
	{
		FrListItem* pItem = (FrListItem*)param;

		if (pItem)
		{
			pStorage->DrawStoredItemInfo((const FrListBox*&)m_pItemStorageList,
				*pItem);
		}
	}
}

void CRealMyRoomMain::OnLBDownItemStorageList()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true &&
		m_bItemStorageVisible == true)
	{
		if (m_pItemStorageList)
		{
			FrListItem* pItem = m_pItemStorageList->GetItemUnderCursor();

			if (CItemStorage::Instance() &&
				!CItemStorage::Instance()->IsLock() && pItem)
			{
				sStoredItemInfo* pInfo = (sStoredItemInfo*)pItem->pData;

				if (pInfo)
				{
					sStoredItemDragDrop dragDrop;
					memset(&dragDrop, 0, sizeof(dragDrop));

					dragDrop.index = pItem->idx;
					dragDrop.mode = sStoredItemDragDrop::MODE_NONE;
					dragDrop.storedItemInfo = *pInfo;

					CItemStorage* pStorage = CItemStorage::Instance();
					if (pStorage)
						pStorage->SetDragDropItem(dragDrop);
				}

				m_pItemStorageList->SelectItem(pItem, true);
			}
		}
	}
}

void CRealMyRoomMain::OnRBDownItemStorageList()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true &&
		m_bItemStorageVisible == true)
	{
		if (m_pItemStorageList)
		{
			CItemStorage* pStorage = CItemStorage::Instance();
			if (pStorage)
			{
				if (!pStorage->IsLock())
				{
					FrListItem* pItem =
						m_pItemStorageList->GetItemUnderCursor();

					if (pItem)
					{
						sStoredItemInfo* pInfo = (sStoredItemInfo*)pItem->pData;

						if (pInfo)
						{
							unsigned long typeId = pInfo->GetTypeID();
							unsigned long guid = pInfo->GetGuid();

							OpenInformation(&typeId, NULL, &guid);
						}

						m_pItemStorageList->SelectItem(pItem, true);
					}
				}
			}
		}
	}
}

void CRealMyRoomMain::OnDBClickItemStorageList()
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true &&
		m_bItemStorageVisible == true)
	{
		if (m_pItemStorageList)
		{
			if (CItemStorage::Instance())
			{
				if (!CItemStorage::Instance()->IsLock())
				{
					FrListItem* pItem =
						m_pItemStorageList->GetItemUnderCursor();

					if (pItem)
					{
						m_pItemStorageList->SelectItem(pItem, true);

						if (CItemStorage::Instance())
						{
							CItemStorage::Instance()->ReqPopStoredItemInfo(
								*pItem);
						}
					}
				}
			}
		}
	}
}

void CRealMyRoomMain::OnMouseMoveItemStorageList()
{
	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->SetDrag(
			sStoredItemDragDrop::MODE_STORAGE_LIST);
	}
}

void CRealMyRoomMain::OnInitItemStorageIndexMsg(int param)
{
	m_pItemStorageIndexMsg = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->AddFrontUI(m_pItemStorageIndexMsg);
	}
}

void CRealMyRoomMain::OnOwnerDrawItemStorageIndexMsg(int param)
{
	if (CItemStorage::Instance())
	{
		WPoint pos(165.0f, 386.0f);
		CItemStorage::Instance()->IndexControlDraw(pos);
	}
}

void CRealMyRoomMain::OnLBDownItemStorageIndexMsg()
{
	if (CItemStorage::Instance())
	{
		if (!CItemStorage::Instance()->IsLock())
		{
			CItemStorage::Instance()->IndexControlClick();
		}
	}
}

void CRealMyRoomMain::OnInitItemStoragePageNext(int param)
{
	m_pItemStoragePageNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pItemStoragePageNext)
	{
		m_pItemStoragePageNext->SetPushDelay(0.1f);
		m_pItemStoragePageNext->SetVisible(false);
	}
}

void CRealMyRoomMain::OnLBDownItemStoragePageNext()
{
	if (CItemStorage::Instance())
	{
		if (!CItemStorage::Instance()->IsLock())
		{
			CItemStorage::Instance()->IndexControlNext();
		}
	}
}

void CRealMyRoomMain::OnInitItemStoragePagePrev(int param)
{
	m_pItemStoragePagePrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pItemStoragePagePrev)
	{
		m_pItemStoragePagePrev->SetPushDelay(0.1f);
		m_pItemStoragePagePrev->SetVisible(false);
	}
}

void CRealMyRoomMain::OnLBDownItemStoragePagePrev()
{
	if (CItemStorage::Instance())
	{
		if (!CItemStorage::Instance()->IsLock())
		{
			CItemStorage::Instance()->IndexControlPrev();
		}
	}
}

void CRealMyRoomMain::OnInitItemStorageClose(int param)
{
	m_pItemStorageClose = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pItemStorageClose)
	{
		m_pItemStorageClose->SetPushDelay(0.1f);
	}

	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->AddFrontUI(m_pItemStorageClose);
	}
}

void CRealMyRoomMain::OnLBDownItemStorageClose()
{
	CItemStorage* pStorage = CItemStorage::Instance();

	if (pStorage)
	{
		if (!pStorage->IsLock())
		{
			SetItemStorageVisible(false);
		}
	}

	if (IS_KINDOF(ntAvatarChatTask, AfxGetPreservedTask()))
	{
		m_pUnderBarGame->Enable(false);
		m_pUnderBarServer->Enable(false);
	}
}

void CRealMyRoomMain::OnInitItemStorageDragDrop(int param)
{
	m_pItemStorageDragDrop = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->AddFrontUI(m_pItemStorageDragDrop);
	}
}

void CRealMyRoomMain::OnOwnerDrawItemStorageDragDrop(int param)
{
	if (CItemStorage::Instance())
	{
		CItemStorage::Instance()->DragDropItemDraw();
	}
}

void CRealMyRoomMain::SetItemStorageVisible(bool bVisible)
{
	if (IsLocalContent(S4_ITEM_STORAGE) == true)
	{
		m_bItemStorageVisible = bVisible;

		CItemStorage* pStorage = CItemStorage::Instance();

		if (pStorage)
		{
			if (bVisible == true)
			{
				pStorage->SetVisibleFrontUI(true);
				pStorage->SetEnableBackGroundUI(false);
				pStorage->SetVisibleBackGroundUI(false);

				pStorage->InitIndexControl(pStorage->GetTotalPage(), 5);

				if (m_pItemStorageList)
				{
					pStorage->MakeStoredItemInfo(m_pItemStorageList);

					if (m_pItemStoragePageNext && m_pItemStoragePagePrev)
					{
						if (pStorage->NeedPageIndexer() == true)
						{
							m_pItemStoragePageNext->SetVisible(true);
							m_pItemStoragePagePrev->SetVisible(true);
						}
						else
						{
							m_pItemStoragePageNext->SetVisible(false);
							m_pItemStoragePagePrev->SetVisible(false);
						}
					}
				}
			}
			else
			{
				pStorage->SetVisibleFrontUI(false);
				pStorage->SetEnableBackGroundUI(true);
				pStorage->SetVisibleBackGroundUI(true);

				if (m_pItemStorageList)
				{
					m_pItemStorageList->ClearItem();
				}

				pStorage->ClearStoredItemInfo();

				if (m_pItemStoragePageNext && m_pItemStoragePagePrev)
				{
					m_pItemStoragePageNext->SetVisible(false);
					m_pItemStoragePagePrev->SetVisible(false);
				}
			}
		}
	}
}

void CRealMyRoomMain::DrawCutinFrame()
{
	WRect dst;

	if (m_subTab == (subTab_t)33)
	{
		FrListItem* pItem = m_pItemList->GetSelected();
		if (pItem)
		{
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;

			if (pInfo)
			{
				WOverlay* pImg = GetCutinImg(pInfo->tid);
				if (pImg)
				{
					dst.x = 72.0f;
					dst.y = 67.0f;
					dst.w = (float)pImg->GetWidth();
					dst.h = (float)pImg->GetHeight();
					pImg->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), dst, 0,
						0xffffffff, 0.0f, 0);
				}
			}
		}
	}
}

WOverlay* CRealMyRoomMain::GetCutinImg(unsigned long typeId)
{
	WOverlay* pOverlay;

	std::map<unsigned int, WOverlay*>::iterator it = m_cutinImg.find(typeId);

	if (it == m_cutinImg.end())
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(typeId);

		if (pSkin)
		{
			char name[64];
			sprintf(name, "%s.jpg", pSkin->Tex);
			pOverlay = g_resrcmng->GetOverlay(name, 0);

			if (pOverlay)
				m_cutinImg.insert(std::map<unsigned int, WOverlay*>::value_type(
					pSkin->c.TypeId, pOverlay));
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		pOverlay = (*it).second;
	}

	return pOverlay;
}

void CRealMyRoomMain::DeleteCutinImages()
{
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cutinImg.begin(); it != m_cutinImg.end(); ++it) // NOFORMAT
	{
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	}
}

bool CRealMyRoomMain::OnRentalExtensionDlgResult(int result, FrForm* form)
{
	S5::FrRentalExtensionDlg* pDlg =
		DYNAMIC_CAST(S5::FrRentalExtensionDlg, form);

	if (result == 3)
	{
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);

		const sItemInfo* pInfo = pDlg->GetItemInfo();
		WSendPacket packet((enumClientPacket)0xe8);
		packet.Encode4(pInfo->guid);
		packet.Send(TO_GAME);
	}
	else if (result == 4)
	{
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);

		const sItemInfo* pInfo = pDlg->GetItemInfo();
		WSendPacket packet((enumClientPacket)0xe9);
		packet.Encode4(pInfo->guid);
		packet.Send(TO_GAME);
	}

	return true;
}

ILFILLTU_REALMYROOMMAINUI
