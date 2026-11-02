#include "minatl.h"
#include "shopmain.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "checkoutdlg.h"
#include "giftdlg.h"
#include "repaydlg.h"
#include "upgradecaddiedlg.h"
#include "setitemdlg.h"
#include "caddieinfodlg.h"
#include "cardsystem.h"
#include "tooltip.h"
#include "uccmanager.h"
#include "ucclibrary.h"
#include "mascot.h"
#include "messengerdlg.h"
#include "chatmsg.h"
#include "equipskin.h"
#include "s5/utilities.h"
#include "../../shared/titlemanager.h"
#include "frgaugebar.h"
#include "frviewer.h"
#include "frgroupbox.h"
#include "frcombobox.h"
#include "frtextbutton.h"
#include "fredit.h"
#include "frstatic.h"
#include "frlistbox.h"
#include "rankinginfo.h"
#include "user_info.h"
#include "gatewayactor.h"
#include "lobbytask.h"
#include "partsinfo.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;
extern WView* g_view;
extern WResourceManager* g_resrcmng;
int float2int(float value);

namespace S5
{
	bool UnderCategory(unsigned long charTypeId, unsigned long typeId,
		unsigned long category);
}

void SetSubTabImg(const char** const img, int index);

void AddSellMiniButton(IFF_ITEM_COMMON& item, FrListItem*& pItem)
{
	if (item.InStock == 1 || item.InStock == 3)
		pItem->AddNoRBtnIdx(1);
	if (item.InStock == 1 || item.InStock == 2)
		pItem->AddNoRBtnIdx(2);
}

IMPLEMENT_ACTOR(CShopMain, CTaskMain)

BEGIN_FRESH_MSGMAP(CShopMain, CTaskMain)
ON_FRESH_VI("SHOPMAIN", FRCMD_INIT, CShopMain::OnShopMainInit)
ON_FRESH_VV("SHOPMAIN", FRCMD_FINISH, CShopMain::OnShopMainFinish)

ON_FRESH_VI("SHOPMAIN.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("SHOPMAIN.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("SHOPMAIN.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)

ON_FRESH_VI("SHOPMAIN.shopview", FRCMD_INIT, CShopMain::OnSortOptionComboInit)
ON_FRESH_VV("SHOPMAIN.shopview", FRCMD_LBUTTONDOWN,
	CShopMain::OnSortOptionComboDown)
ON_FRESH_VI("SHOPMAIN.searchgoods", FRCMD_INIT,
	CShopMain::OnSearchGoodsButtonInit)
ON_FRESH_VV("SHOPMAIN.searchgoods", FRCMD_LBUTTONUP,
	CShopMain::OnSearchGoodsLButtonDown)

ON_FRESH_VI("SHOPMAIN.item_list", FRCMD_INIT, CShopMain::OnItemListInit)
ON_FRESH_VI("SHOPMAIN.item_list", FRCMD_OWNERDRAW,
	CShopMain::OnItemListOwnerDraw)
ON_FRESH_VV("SHOPMAIN.item_list", FRCMD_LBUTTONDOWN,
	CShopMain::OnItemListBtnDown)
ON_FRESH_VV("SHOPMAIN.item_list", FRCMD_RBUTTONDOWN,
	CShopMain::OnItemListRBtnDown)
ON_FRESH_VI("SHOPMAIN.tab_main", FRCMD_INIT, CShopMain::OnTabMainInit)
ON_FRESH_VV("SHOPMAIN.tab_main", FRCMD_LBUTTONUP, CShopMain::OnTabMainButtonUp)
ON_FRESH_VI("SHOPMAIN.tab_sub", FRCMD_INIT, CShopMain::OnTabSubInit)
ON_FRESH_VV("SHOPMAIN.tab_sub", FRCMD_LBUTTONUP, CShopMain::OnTabSubButtonUp)

ON_FRESH_VI("SHOPMAIN.buy_all", FRCMD_INIT, CShopMain::OnBuyAllInit)
ON_FRESH_VV("SHOPMAIN.buy_all", FRCMD_LBUTTONUP, CShopMain::OnBuyAllUp)
ON_FRESH_VI("SHOPMAIN.buy_all", FRCMD_HOVERON, CShopMain::OnBuyHoverOn)
ON_FRESH_VI("SHOPMAIN.buy_all", FRCMD_HOVEROFF, CShopMain::OnBuyHoverOff)
ON_FRESH_VI("SHOPMAIN.buy_reset", FRCMD_INIT, CShopMain::OnBuyResetInit)
ON_FRESH_VV("SHOPMAIN.buy_reset", FRCMD_LBUTTONUP, CShopMain::OnBuyResetUp)
ON_FRESH_VI("SHOPMAIN.buy_reset", FRCMD_HOVERON, CShopMain::OnBuyHoverOn)
ON_FRESH_VI("SHOPMAIN.buy_reset", FRCMD_HOVEROFF, CShopMain::OnBuyHoverOff)

ON_FRESH_VI("SHOPMAIN.sidetab_buy", FRCMD_INIT, CShopMain::OnSideTabBuyInit)
ON_FRESH_VV("SHOPMAIN.sidetab_buy", FRCMD_LBUTTONDOWN,
	CShopMain::OnSideTabBuyUp)
ON_FRESH_VI("SHOPMAIN.sidetab_rental", FRCMD_INIT,
	CShopMain::OnSideTabRentalInit)
ON_FRESH_VV("SHOPMAIN.sidetab_rental", FRCMD_LBUTTONDOWN,
	CShopMain::OnSideTabRentalUp)

ON_FRESH_VI("SHOPMAIN.tabtxt1", FRCMD_INIT, CShopMain::OnTabTxt1Init)
ON_FRESH_VI("SHOPMAIN.tabtxt2", FRCMD_INIT, CShopMain::OnTabTxt2Init)
ON_FRESH_VI("SHOPMAIN.tabtxt2", FRCMD_INIT, CShopMain::OnTabTxt3Init)
ON_FRESH_VI("SHOPMAIN.st.selected_name", FRCMD_INIT,
	CShopMain::OnSelectedNameInit)

ON_FRESH_VI("SHOPMAIN.pet", FRCMD_INIT, CShopMain::OnPetInit)
ON_FRESH_VI("SHOPMAIN.pet", FRCMD_OWNERDRAW, CShopMain::OnPetOwnerDraw)
ON_FRESH_VI("SHOPMAIN.skin_pet", FRCMD_INIT, CShopMain::OnSkinPetInit)
ON_FRESH_VI("SHOPMAIN.skin_pet2", FRCMD_INIT, CShopMain::OnSkinPet2Init)
ON_FRESH_VI("SHOPMAIN.item_desc_bg", FRCMD_INIT, CShopMain::OnItemDescBgInit)
ON_FRESH_VI("SHOPMAIN.item_desc_bg", FRCMD_OWNERDRAW,
	CShopMain::OnItemDescBgOwnerDraw)
ON_FRESH_VI("SHOPMAIN.item_desc", FRCMD_INIT, CShopMain::OnItemDescInit)
ON_FRESH_VI("SHOPMAIN.part_cart", FRCMD_INIT, CShopMain::OnPartCartListInit)
ON_FRESH_VI("SHOPMAIN.part_cart", FRCMD_OWNERDRAW,
	CShopMain::OnPartCartListOwnerDraw)
ON_FRESH_VI("SHOPMAIN.auxpart_cart", FRCMD_INIT,
	CShopMain::OnAuxPartCartListInit)
ON_FRESH_VI("SHOPMAIN.auxpart_cart", FRCMD_OWNERDRAW,
	CShopMain::OnAuxPartCartListOwnerDraw)
ON_FRESH_VI("SHOPMAIN.scroll_up", FRCMD_INIT, CShopMain::OnScrollUpInit)
ON_FRESH_VV("SHOPMAIN.scroll_up", FRCMD_LBUTTONUP, CShopMain::OnScrollUpUp)
ON_FRESH_VI("SHOPMAIN.scroll_down", FRCMD_INIT, CShopMain::OnScrollDownInit)
ON_FRESH_VV("SHOPMAIN.scroll_down", FRCMD_LBUTTONUP, CShopMain::OnScrollDownUp)
ON_FRESH_VI("SHOPMAIN.item_slot_frame1", FRCMD_INIT,
	CShopMain::OnItemSlotFrame1Init)
ON_FRESH_VI("SHOPMAIN.item_slot_frame2", FRCMD_INIT,
	CShopMain::OnItemSlotFrame2Init)
ON_FRESH_VI("SHOPMAIN.item_slot", FRCMD_INIT, CShopMain::OnItemSlotInit)
ON_FRESH_VI("SHOPMAIN.item_slot", FRCMD_OWNERDRAW,
	CShopMain::OnItemSlotOwnerDraw)

ON_FRESH_VI("SHOPMAIN.st", FRCMD_INIT, CShopMain::OnStatBoxInit)
ON_FRESH_VI("SHOPMAIN.st.selframe", FRCMD_INIT, CShopMain::OnSelFrameInit)
ON_FRESH_VI("SHOPMAIN.st.selitemicon", FRCMD_INIT, CShopMain::OnSelItemIconInit)
ON_FRESH_VI("SHOPMAIN.st.selitem", FRCMD_INIT, CShopMain::OnSelItemInit)
ON_FRESH_VV("SHOPMAIN.st.selitem", FRCMD_LBUTTONUP, CShopMain::OnSelItemUp)
ON_FRESH_VI("SHOPMAIN.st.selitem", FRCMD_OWNERDRAW,
	CShopMain::OnSelItemOwnerDraw)
ON_FRESH_VI("SHOPMAIN.st.selitemicon", FRCMD_OWNERDRAW,
	CShopMain::OnSelItemIconOwnerDraw)
ON_FRESH_VI("SHOPMAIN.st.selitem_prev", FRCMD_INIT,
	CShopMain::OnSelItemPrevInit)
ON_FRESH_VV("SHOPMAIN.st.selitem_prev", FRCMD_LBUTTONUP,
	CShopMain::OnSelItemPrevUp)
ON_FRESH_VI("SHOPMAIN.st.selitem_next", FRCMD_INIT,
	CShopMain::OnSelItemNextInit)
ON_FRESH_VV("SHOPMAIN.st.selitem_next", FRCMD_LBUTTONUP,
	CShopMain::OnSelItemNextUp)
ON_FRESH_VI("SHOPMAIN.st.stat_frame", FRCMD_INIT, CShopMain::OnStatFrameInit)
ON_FRESH_VI("SHOPMAIN.st.stat_text", FRCMD_INIT, CShopMain::OnStatTextInit)
ON_FRESH_VI("SHOPMAIN.st.stat_part", FRCMD_INIT, CShopMain::OnStatPartInit)
ON_FRESH_VV("SHOPMAIN.st.stat_part", FRCMD_LBUTTONUP, CShopMain::OnStatPartUp)
ON_FRESH_VI("SHOPMAIN.st.stat_part", FRCMD_OWNERDRAW,
	CShopMain::OnStatPartOwnerDraw)
ON_FRESH_VI("SHOPMAIN.st.stat_all", FRCMD_INIT, CShopMain::OnStatAllInit)
ON_FRESH_VV("SHOPMAIN.st.stat_all", FRCMD_LBUTTONUP, CShopMain::OnStatAllUp)
ON_FRESH_VI("SHOPMAIN.st.stat_all", FRCMD_OWNERDRAW,
	CShopMain::OnStatAllOwnerDraw)
ON_FRESH_VI("SHOPMAIN.st.powerbar", FRCMD_INIT, CShopMain::OnPowerBarInit)
ON_FRESH_VI("SHOPMAIN.st.controlbar", FRCMD_INIT, CShopMain::OnControlBarInit)
ON_FRESH_VI("SHOPMAIN.st.impactbar", FRCMD_INIT, CShopMain::OnImpactBarInit)
ON_FRESH_VI("SHOPMAIN.st.spinbar", FRCMD_INIT, CShopMain::OnSpinBarInit)
ON_FRESH_VI("SHOPMAIN.st.curvebar", FRCMD_INIT, CShopMain::OnCurveBarInit)
ON_FRESH_VI("SHOPMAIN.st.epower", FRCMD_INIT, CShopMain::OnPowerEditInit)
ON_FRESH_VI("SHOPMAIN.st.econtrol", FRCMD_INIT, CShopMain::OnControlEditInit)
ON_FRESH_VI("SHOPMAIN.st.eimpact", FRCMD_INIT, CShopMain::OnImpactEditInit)
ON_FRESH_VI("SHOPMAIN.st.espin", FRCMD_INIT, CShopMain::OnSpinEditInit)
ON_FRESH_VI("SHOPMAIN.st.ecurve", FRCMD_INIT, CShopMain::OnCurveEditInit)

ON_FRESH_VI("SHOPMAIN.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("SHOPMAIN.ubar.back", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("SHOPMAIN.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("SHOPMAIN.ubar.front", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("SHOPMAIN.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("SHOPMAIN.ubar.shop", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("SHOPMAIN.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("SHOPMAIN.ubar.myroom", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("SHOPMAIN.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("SHOPMAIN.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("SHOPMAIN.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("SHOPMAIN.ubar.gift", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("SHOPMAIN.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("SHOPMAIN.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("SHOPMAIN.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("SHOPMAIN.ubar.game", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("SHOPMAIN.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("SHOPMAIN.ubar.server", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("SHOPMAIN.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("SHOPMAIN.ubar.exit", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("SHOPMAIN.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("SHOPMAIN.ubar.tabbtn", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("SHOPMAIN.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("SHOPMAIN.utab.magicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("SHOPMAIN.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("SHOPMAIN.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("SHOPMAIN.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("SHOPMAIN.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("SHOPMAIN.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("SHOPMAIN.utab.scratch", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("SHOPMAIN.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("SHOPMAIN.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("SHOPMAIN.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("SHOPMAIN.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("SHOPMAIN.utab.ranking", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("SHOPMAIN.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("SHOPMAIN.utab.guild", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("SHOPMAIN.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("SHOPMAIN.utab.notice", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("SHOPMAIN.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("SHOPMAIN.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("SHOPMAIN.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("SHOPMAIN.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("SHOPMAIN.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("SHOPMAIN.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("SHOPMAIN.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("SHOPMAIN.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("SHOPMAIN.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("SHOPMAIN.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("SHOPMAIN.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("SHOPMAIN.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("SHOPMAIN.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("SHOPMAIN.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("SHOPMAIN.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("SHOPMAIN.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("SHOPMAIN.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("SHOPMAIN.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("SHOPMAIN.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("SHOPMAIN.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("SHOPMAIN.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("SHOPMAIN.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("SHOPMAIN.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("SHOPMAIN.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("SHOPMAIN.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("SHOPMAIN.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("SHOPMAIN.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("SHOPMAIN.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("SHOPMAIN.obar.messenger", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("SHOPMAIN.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("SHOPMAIN.obar.myinfo", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("SHOPMAIN.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("SHOPMAIN.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("SHOPMAIN.obar.option", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OptionUp)

ON_FRESH_VI("SHOPMAIN.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)

ON_FRESH_VI("SHOPMAIN.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("SHOPMAIN.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("SHOPMAIN.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("SHOPMAIN.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("SHOPMAIN.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("SHOPMAIN.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("SHOPMAIN.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("SHOPMAIN.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerGiftAlarmBtnUp)

ON_FRESH_VI("SHOPMAIN.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
END_FRESH_MSGMAP()

void CShopMain::OnShopMainInit(int param)
{
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

void CShopMain::OnShopMainFinish()
{
	m_pOverBarTitle->SetBgImg("title_shop");

	m_pPetArea = m_pPet;

	unsigned long caddieTypeId = !Doc()->m_myInfo.userEquip.guidCaddie
		? 0x1c000000
		: Doc()->m_caddieMap[Doc()->m_myInfo.userEquip.guidCaddie].tid;
	unsigned long mascotTypeId = !Doc()->m_myInfo.userEquip.guidMascot
		? 0x40000000
		: Doc()->m_mascotMap[Doc()->m_myInfo.userEquip.guidMascot].tid;

	ChangeCharacter(Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar].tid);
	ChangeCaddie(caddieTypeId);
	ChangeClubset(
		Doc()->m_clubSetMap[Doc()->m_myInfo.userEquip.guidClubSet].tid);
	ChangeAztec(Doc()->m_myInfo.userEquip.tidBall);
	ChangeMascot(mascotTypeId);
	memcpy(m_skinTypeId, Doc()->m_myInfo.userEquip.tidSkin,
		sizeof(m_skinTypeId));

	ChangeMainTab(m_mainTab, true);
	ChangeSubTab(m_subTab, true);
	m_statTab = (statTab_t)(m_mainTab > MAINTAB_CADDIE);

	memcpy(m_skinTypeId, Doc()->m_myInfo.userEquip.tidSkin,
		sizeof(m_skinTypeId));
	for (int i = 0; i < 6; i++)
	{
		if (m_skinTypeId[i])
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_skinTypeId[i]);
			if (pSkin && !(m_skinTypeId[i] & 0x3c00000))
			{
				m_skinScrollDir[0] = pSkin->HScroll;
				m_skinScrollDir[1] = pSkin->VScroll;
				m_skinScroll[0] = 0.0f;
				m_skinScroll[1] = 0.0f;
			}
		}
	}

	if (Doc()->m_bRefreshCamera)
		HandleMsg(MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));
}

void CShopMain::OnSortOptionComboInit(int param)
{
	FrComboBox* pWnd = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
	m_pSortOptionCombo = pWnd;

	if (pWnd)
	{
		pWnd->SetVisible(false);
		pWnd->Enable(false);
	}
}

void CShopMain::OnSortOptionComboDown()
{
}

void CShopMain::OnSearchGoodsButtonInit(int param)
{
	FrTextButton* pWnd = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
	m_pSearchGoods = pWnd;

	if (pWnd)
	{
		pWnd->SetVisible(false);
		pWnd->Enable(false);
	}
}

void CShopMain::OnSearchGoodsLButtonDown()
{
}

void CShopMain::InitShopDialogs()
{
	m_pCheckoutDlg = NULL;
	m_pOptionDlg = NULL;
	m_pKbdManualDlg = NULL;
	m_pServerDlg = NULL;
	m_pGiftDlg = NULL;
	m_pWaitDlg = NULL;
	m_pNotifyOneDlg = NULL;
	m_fKbdManualTime = -10.0f;
}

void CShopMain::InitShopControls()
{
	m_pSelectedName = NULL;
	m_pStatFrame = NULL;
	m_pStatText = NULL;
	m_pStatPart = NULL;
	m_pStatAll = NULL;
	m_pTabMain = NULL;
	m_pTabSub = NULL;
	m_pBuyAll = NULL;
	m_pBuyReset = NULL;
	m_pItemList = NULL;
	m_pPartCartList = NULL;
	m_pAuxPartCartList = NULL;
	m_pTabTxt[0] = NULL;
	m_pTabTxt[1] = NULL;
	m_pTabTxt[2] = NULL;
	m_pReserved228 = NULL;
	m_pPetArea = NULL;
	m_pExhibition = NULL;
	m_pSelItemIcon = NULL;
	m_pStatBox = NULL;
	m_pSelItem = NULL;
	m_pSelFrame = NULL;
	memset(m_pStatBar, 0, sizeof(m_pStatBar));
	memset(m_pStatEdit, 0, sizeof(m_pStatEdit));
	m_pSelItemPrev = NULL;
	m_pSelItemNext = NULL;
	m_pPopupEdit = NULL;
	m_popupTime = -10.0f;
	memset(m_skinTypeId, 0, sizeof(m_skinTypeId));
	memset(m_skinScrollDir, 0, sizeof(m_skinScrollDir));
	memset(m_skinScroll, 0, sizeof(m_skinScroll));
	m_pPet = NULL;
	m_pSkinPet = NULL;
	m_pItemSlotFrame1 = NULL;
	m_pItemSlotFrame2 = NULL;
	m_statSubject = 0;

	m_pSelArrow = NULL;
	m_selArrowOffset = 0.0f;
	m_selArrowTime = 0.0f;

	memset(m_partsUid, 0, sizeof(m_partsUid));

	m_pMarkBegin = NULL;
}

void CShopMain::CloseAllDialogs()
{
	FrWnd* pWnd = DYNAMIC_CAST(FrWnd,
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
	if (pWnd)
		pWnd->Close(true);
}

void CShopMain::TurnOnItemStatus()
{
	m_pStatBox->SetVisible(true);
	SetLevelBar();
}

void CShopMain::TurnOffItemStatus()
{
	m_pStatBox->SetVisible(false);
	SetLevelBar();
}

bool PartCompare(const void* left, const void* right);

std::string GenerateTextureName_F(unsigned long typeId, const char* uccIndex);
std::string GenerateTextureName_B(unsigned long typeId, const char* uccIndex);

namespace S5
{
	bool UnderCategory(unsigned long charTypeId, unsigned long typeId,
		unsigned long category);
}

bool PartCompare(const void* left, const void* right)
{
	if (left == NULL)
		return false;
	if (right == NULL)
		return true;

	const sDisplayUccClothes* a = (const sDisplayUccClothes*)left;
	const sDisplayUccClothes* b = (const sDisplayUccClothes*)right;

	if (UccManager()->IsDisplayUccClothes(&a->part.c) &&
		!UccManager()->IsDisplayUccClothes(&b->part.c))
		return true;
	if (UccManager()->IsDisplayUccClothes(&b->part.c) &&
		!UccManager()->IsDisplayUccClothes(&a->part.c))
		return false;
	if (UccManager()->IsDisplayUccClothes(&a->part.c) &&
		UccManager()->IsDisplayUccClothes(&b->part.c))
	{
		if (a->rank < b->rank)
		{
			return true;
		}
		else if (a->rank == b->rank)
		{
			if (a->partTypeId < b->partTypeId)
				return true;
			return false;
		}
		else if (a->rank > b->rank)
		{
			return false;
		}
	}

	SYSTEMTIME* aStart = NULL;
	SYSTEMTIME* aEnd = NULL;
	SYSTEMTIME* bStart = NULL;
	SYSTEMTIME* bEnd = NULL;
	bool aSale = ItemManager()->HasSalePeriod(a->part.c.TypeId, &aStart, &aEnd);
	bool bSale = ItemManager()->HasSalePeriod(b->part.c.TypeId, &bStart, &bEnd);

	if (!aSale && bSale)
	{
		// HACK! Forces a specific tail merge that otherwise CL seems to stubbornly refuse.
		if (!aSale)
			goto RF;
	}
	if (aSale && !bSale)
		return true;
	if (aSale && bSale)
	{
		if ((aStart->wYear || aEnd->wYear) && !bStart->wYear && !bEnd->wYear)
			return true;
		if (!aStart->wYear && !aEnd->wYear && (bStart->wYear || bEnd->wYear))
			return false;
	}

	if (a->part.c.New && !b->part.c.New)
		return true;
	if (b->part.c.New && !a->part.c.New)
		return false;

	if (a->part.c.Hit && !b->part.c.Hit)
		return true;
	if (b->part.c.Hit && !a->part.c.Hit)
	{
RF:
		return false;
	}

	if ((a->part.c.TypeId & 0xfc000000) == 0x08000000 &&
		(b->part.c.TypeId & 0xfc000000) == 0x08000000)
	{
		if (a->part.Category < b->part.Category)
			return true;
		if (a->part.Category > b->part.Category)
			return false;
	}

	return a->part.c.TypeId > b->part.c.TypeId;
}

bool SpecialHitNewCompare(const void* left, const void* right)
{
	const IFF_ITEM_COMMON* a = (const IFF_ITEM_COMMON*)left;
	const IFF_ITEM_COMMON* b = (const IFF_ITEM_COMMON*)right;

	SYSTEMTIME* aStart = NULL;
	SYSTEMTIME* aEnd = NULL;
	SYSTEMTIME* bStart = NULL;
	SYSTEMTIME* bEnd = NULL;
	bool aSale = ItemManager()->HasSalePeriod(a->TypeId, &aStart, &aEnd);
	bool bSale = ItemManager()->HasSalePeriod(b->TypeId, &bStart, &bEnd);

	if (!aSale && bSale)
	{
		// HACK! Forces a specific tail merge that otherwise CL seems to stubbornly refuse.
		if (!aSale)
			goto RF;
	}
	if (aSale && !bSale)
		return true;
	if (aSale && bSale)
	{
		if ((aStart->wYear || aEnd->wYear) && !bStart->wYear && !bEnd->wYear)
			return true;
		if (!aStart->wYear && !aEnd->wYear && (bStart->wYear || bEnd->wYear))
			return false;
	}

	if (a->New && !b->New)
		return true;
	if (b->New && !a->New)
		return false;

	if (a->Hit && !b->Hit)
		return true;
	if (b->Hit && !a->Hit)
	{
RF:
		return false;
	}

	return a->TypeId > b->TypeId;
}

bool SkinCompare(const void* left, const void* right)
{
	if (left == NULL)
		return false;
	if (right == NULL)
		return true;

	return ((const IFF_ITEM_COMMON*)left)->TypeId <
		((const IFF_ITEM_COMMON*)right)->TypeId;
}

bool ShopEmptyCompare(const void* left, const void* right)
{
	if (left)
	{
		if (right == NULL)
			return true;
	}
	return false;
}

bool CaddieCompare(const void* left, const void* right)
{
	if (left == NULL || right == NULL)
		return false;

	return ((const IFF_ITEM_COMMON*)left)->TypeId <
		((const IFF_ITEM_COMMON*)right)->TypeId;
}

void CShopMain::OnPetInit(int param)
{
	m_pPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnSkinPetInit(int param)
{
	m_pSkinPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnSkinPet2Init(int param)
{
	m_pSkinPet2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::DrawSkinFrame()
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
		m_anotherHand.Enter();
		if (m_pExhibition)
			m_pExhibition->Display(0.0f, 0.0f);
		m_anotherHand.Leave();
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

void CShopMain::DrawSkinSlot()
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

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
				386.0f - (int)(pLevel->Height() * 0.5f), (float)pLevel->Width(),
				(float)pLevel->Height()),
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

void CShopMain::DrawCutinFrame()
{
	WRect dst;

	if (m_subTab != SUBTAB_CUTIN)
		return;

	FrListItem* pItem = m_pItemList->GetSelected();
	if (!pItem)
		return;

	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	if (!pInfo)
		return;

	WOverlay* pImg = GetCutinImg(pInfo->tid);
	if (!pImg)
		return;

	dst.x = 72.0f;
	dst.y = 67.0f;
	dst.w = (float)pImg->GetWidth();
	dst.h = (float)pImg->GetHeight();
	pImg->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), dst, 0, 0xffffffff,
		0.0f, 0);
}

WOverlay* CShopMain::GetCutinImg(unsigned long typeId)
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

void CShopMain::DeleteCutinImages()
{
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cutinImg.begin();
		it != m_cutinImg.end(); ++it)
	{
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	}
}

void CShopMain::OnPetOwnerDraw(int a2)
{
	if (m_mainTab == MAINTAB_SKIN)
	{
		switch (m_subTab)
		{
		case SUBTAB_SKIN1:
		case SUBTAB_SKIN2:
		case SUBTAB_SKIN3:
			DrawSkinFrame();
			break;
		case SUBTAB_SKIN4:
		case SUBTAB_SKIN5:
			DrawSkinSlot();
			break;
		case SUBTAB_CUTIN:
			DrawCutinFrame();
			break;
		}
	}
	else
	{
		if (!m_anotherHand.IsWorking(0) && !m_bBongdariShop2 && !RANKINGDLG() &&
			!USERINFODLG())
		{
			m_anotherHand.Enter();
			if (m_pExhibition)
				m_pExhibition->Display(0.0f, 0.0f);
			m_anotherHand.Leave();
		}
	}
}

void CShopMain::OnItemDescBgInit(int param)
{
	m_pItemDescBg = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnItemDescBgOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

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
			WRect(m_itemDescOffset.x + 262.0f, y, w[1], h[1]), 0xffffffff, 0);
	}
}

void CShopMain::OnItemDescInit(int param)
{
	m_pItemDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (!m_pItemDesc)
		return;

	m_pItemDesc->SetVisible(false);
	m_itemDescPos = WPoint(m_pItemDesc->GetRect().x,
		m_pItemDesc->GetRect().y + m_pItemDesc->GetRect().h);
}

void CShopMain::SetItemDescRect()
{
	m_pItemDesc->ClearLine();

	switch (m_subTab)
	{
	case SUBTAB_CLUBSET:
	case SUBTAB_CADDIE:
		m_itemDescOffset = WPoint(0.0f, 30.0f);
		break;
	case SUBTAB_AZTEC:
	case SUBTAB_MASCOT:
	case SUBTAB_CARD:
	case SUBTAB_INTERIOR1:
	case SUBTAB_INTERIOR2:
	case SUBTAB_INTERIOR3:
	case SUBTAB_FURNITURE:
	case SUBTAB_DECORATE:
		m_itemDescOffset = WPoint(0.0f, 120.0f);
		break;
	default:
		m_itemDescOffset = WPoint(0.0f, 0.0f);
		break;
	}
}

void CShopMain::ShowItemDesc(unsigned long typeId)
{
	IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(typeId);
	if (!pDesc)
		return;

	m_pItemDesc->ClearLine();
	m_pItemDesc->AddText(pDesc->Desc, false, true);

	int lineNum = m_pItemDesc->GetLineNum();
	while (lineNum > 2)
	{
		m_pItemDesc->DeleteEndLine();
		lineNum--;
	}

	float height = (float)m_pItemDesc->GetLineHeight() * lineNum;
	m_pItemDesc->SetRect(WRect(m_itemDescPos.x + m_itemDescOffset.x,
		m_itemDescPos.y + m_itemDescOffset.y - height, m_pItemDesc->GetRect().w,
		height));
	m_pItemDesc->GetScrollBar()->ScrollToFirst();
}

void CShopMain::OnPowerBarInit(int param)
{
	m_pStatBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[0]->SetRange(0, 50, 0);
}

void CShopMain::OnControlBarInit(int param)
{
	m_pStatBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[1]->SetRange(0, 30, 0);
}

void CShopMain::OnImpactBarInit(int param)
{
	m_pStatBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[2]->SetRange(0, 30, 0);
}

void CShopMain::OnSpinBarInit(int param)
{
	m_pStatBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[3]->SetRange(0, 30, 0);
}

void CShopMain::OnCurveBarInit(int param)
{
	m_pStatBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[4]->SetRange(0, 30, 0);
}

void CShopMain::DrawItemPrice(FrGraphicInterface* pGI, float x, float y,
	IFF_ITEM_COMMON& item, FrListItem* pItem)
{
	if (!pItem)
		return;

	if (item.InStock == 4)
		return;

	if (item.SalePrice > 0 && item.SalePrice < item.Price)
	{
		pGI->SetTextColor(0xffff0000, 0xffffffff);
		pGI->Print11(WPoint(x, y) + pItem->pos, 2,
			item.IsCash
				? MakeStr(K2L_Compatibility("%d\xc4\xed\xc5\xb0"),
					  item.SalePrice)
				: MakeStr(K2L_Compatibility("%d\xc6\xce"), item.SalePrice));
		pGI->SetTextColor(0xff000000, 0xffffffff);

		x -= 40.0f;

		pGI->SetTextColor(0xff000000, 0xffffffff);
		int width = float2int(pGI->Print11(WPoint(x, y) + pItem->pos, 2,
			item.IsCash
				? MakeStr(K2L_Compatibility("%d\xc4\xed\xc5\xb0"), item.Price)
				: MakeStr(K2L_Compatibility("%d\xc6\xce"), item.Price)));
		pGI->SetTextColor(0xff000000, 0xffffffff);

		g_view->DrawLine2D(
			WPoint(x - (float)width - 1.0f,
				(float)float2int((float)pGI->GetFontHeight11() * 0.5f) + y) +
				pItem->pos,
			WPoint(x + 1.0f,
				(float)float2int((float)pGI->GetFontHeight11() * 0.5f) + y) +
				pItem->pos,
			0xff000000, 0xff000000, 0);
	}
	else if (item.Price)
	{
		pGI->Print11(WPoint(x, y) + pItem->pos, 2,
			item.IsCash
				? MakeStr(K2L_Compatibility("%d\xc4\xed\xc5\xb0"), item.Price)
				: MakeStr(K2L_Compatibility("%d\xc6\xce"), item.Price));
	}
}

void CShopMain::OnSelFrameInit(int param)
{
	m_pSelFrame = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnStatBoxInit(int param)
{
	m_pStatBox = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
}

void CShopMain::OnSelItemIconInit(int param)
{
	m_pSelItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnSelItemIconOwnerDraw(int param)
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

	switch (m_subTab)
	{
	case SUBTAB_PACKAGE:
	case SUBTAB_PARTS1:
	case SUBTAB_PARTS2:
	case SUBTAB_PARTS3:
	case SUBTAB_PARTS4:
	case SUBTAB_HAIR:
	case SUBTAB_UCC:
		DrawSelItem(pGI, m_charTypeId);
		DrawCharacterList(pGI, m_charTypeId);
		break;
	case SUBTAB_CLUBSET:
		DrawSelItem(pGI, m_clubSetTypeId);
		break;
	case SUBTAB_AZTEC:
		DrawSelItem(pGI, m_aztecTypeId);
		break;
	case SUBTAB_CADDIE:
		DrawSelItem(pGI, m_caddieTypeId);
		DrawCaddieList(pGI, m_caddieTypeId);
		break;
	case SUBTAB_MASCOT:
		DrawSelItem(pGI, m_mascotTypeId);
		break;
	}
}

void CShopMain::OnSelItemInit(int param)
{
	m_pSelItem = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pSelItem)
		return;

	m_pSelItem->EnableBlink(true);
}

void CShopMain::OnSelItemOwnerDraw(int param)
{
}

void CShopMain::OnTabTxt1Init(int param)
{
	m_pTabTxt[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnTabTxt2Init(int param)
{
	m_pTabTxt[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnTabTxt3Init(int param)
{
	m_pTabTxt[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnSelectedNameInit(int param)
{
	m_pSelectedName = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CShopMain::OnSelItemUp()
{
	m_bSelectItemList = !m_bSelectItemList;
}

void CShopMain::OnSelItemPrevInit(int param)
{
	m_pSelItemPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pSelItemPrev)
		return;

	m_pSelItemPrev->SetPushDelay(0.0f);
}

void CShopMain::OnSelItemPrevUp()
{
	switch (m_mainTab)
	{
	case MAINTAB_CHARACTER:
		for (std::map<unsigned int, IFF_STRUCT::sChar>::reverse_iterator it =
				 ItemManager()->m_CharMap.rbegin();
			it != ItemManager()->m_CharMap.rend(); ++it)
		{
			if ((*it).second.c.TypeId == m_charTypeId &&
				it++ != ItemManager()->m_CharMap.rend())
			{
				ChangeCharacter((*it).second.c.TypeId);
				break;
			}
		}
		break;

	case MAINTAB_CADDIE:
		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::reverse_iterator it =
				 m_visibleCaddie.rbegin();
			it != m_visibleCaddie.rend(); ++it)
		{
			if ((*it).second.c.TypeId == m_caddieTypeId)
			{
				while (it++ != m_visibleCaddie.rend())
				{
					if (!IsVisibleInStock((*it).second.c))
						continue;
					ChangeCaddie((*it).second.c.TypeId);
					break;
				}
			}
		}
		break;

	case MAINTAB_MASCOT:
		for (std::map<unsigned int, IFF_STRUCT::sMascot>::reverse_iterator it =
				 ItemManager()->m_MascotMap.rbegin();
			it != ItemManager()->m_MascotMap.rend(); ++it)
		{
			if ((*it).second.c.TypeId == m_mascotTypeId)
			{
				while (it++ != ItemManager()->m_MascotMap.rend())
				{
					if (!IsVisibleInStock((*it).second.c))
						continue;
					ChangeMascot((*it).second.c.TypeId);
					break;
				}
			}
		}
		break;
	}
}

void CShopMain::OnSelItemNextInit(int param)
{
	m_pSelItemNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pSelItemNext)
		return;

	m_pSelItemNext->SetPushDelay(0.0f);
}

void CShopMain::OnSelItemNextUp()
{
	switch (m_mainTab)
	{
	case MAINTAB_CHARACTER:
		for (std::map<unsigned int, IFF_STRUCT::sChar>::iterator it =
				 ItemManager()->m_CharMap.begin();
			it != ItemManager()->m_CharMap.end(); ++it)
		{
			if ((*it).second.c.TypeId == m_charTypeId &&
				it++ != ItemManager()->m_CharMap.end())
			{
				ChangeCharacter((*it).second.c.TypeId);
				break;
			}
		}
		break;

	case MAINTAB_CADDIE:
		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it =
				 m_visibleCaddie.begin();
			it != m_visibleCaddie.end(); ++it)
		{
			if ((*it).second.c.TypeId == m_caddieTypeId)
			{
				while (it++ != m_visibleCaddie.end())
				{
					if (!IsVisibleInStock((*it).second.c))
						continue;
					ChangeCaddie((*it).second.c.TypeId);
					break;
				}
			}
		}
		break;

	case MAINTAB_MASCOT:
		for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it =
				 ItemManager()->m_MascotMap.begin();
			it != ItemManager()->m_MascotMap.end(); ++it)
		{
			if ((*it).second.c.TypeId == m_mascotTypeId)
			{
				while (it++ != ItemManager()->m_MascotMap.end())
				{
					if (!IsVisibleInStock((*it).second.c))
						continue;
					ChangeMascot((*it).second.c.TypeId);
					break;
				}
			}
		}
		break;
	}
}

void CShopMain::OnStatFrameInit(int param)
{
	m_pStatFrame = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnStatTextInit(int param)
{
	m_pStatText = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CShopMain::OnStatPartInit(int param)
{
	m_pStatPart = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void CShopMain::OnStatPartUp()
{
	ChangeStatTab(STATTAB_PART);
}

void CShopMain::OnStatPartOwnerDraw(int param)
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

	pGI->DrawTexture(m_pStatBtn[m_statSubject][2], m_pStatPart->GetRect(),
		0xffffffff, 0);

	WRect rect = m_pStatAll->GetRect();
	pGI->DrawTexture(m_pStatAllBtn[0], rect, 0xffffffff, 0);
}

void CShopMain::OnStatAllInit(int param)
{
	m_pStatAll = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (!m_pStatAll)
		return;

	m_pStatAll->SetVisible(false);
}

void CShopMain::OnStatAllUp()
{
	ChangeStatTab(STATTAB_ALL);
}

void CShopMain::OnStatAllOwnerDraw(int param)
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

	int state = 0;
	WRect rect = m_pStatAll->GetRect();
	if (m_pStatAll->IsVisible())
	{
		if (m_statTab == STATTAB_ALL)
		{
			state = 2;
		}
		else
		{
			const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(pt))
				state = 1;
		}

		pGI->DrawTexture(m_pStatAllBtn[state], rect, 0xffffffff, 0);
	}
}

void CShopMain::OnTabMainInit(int param)
{
	m_pTabMain = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (!m_pTabMain)
		return;

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
	m_pTabMain->AddTab(imgNames, 5, NULL);
}

void CShopMain::OnTabMainButtonUp()
{
	TButtonItem* pSelected = m_pTabMain->GetSelected();
	if (!pSelected)
		return;

	if (pSelected->index == m_mainTab)
		return;

	ChangeMainTab((mainTab_t)pSelected->index, false);

	switch (pSelected->index)
	{
	case MAINTAB_CHARACTER:
		ChangeSubTab(SUBTAB_PACKAGE, false);
		ChangeCharacter(m_charTypeId);
		break;

	case MAINTAB_ITEM:
		ChangeSubTab(SUBTAB_CLUBSET, false);
		ChangeClubset(m_clubSetTypeId);
		break;

	case MAINTAB_CADDIE:
		ChangeSubTab(SUBTAB_CADDIE, false);
		ChangeCaddie(m_caddieTypeId);
		break;

	case MAINTAB_MASCOT:
		ChangeSubTab(SUBTAB_MASCOT, false);
		ChangeMascot(m_mascotTypeId);
		break;

	case MAINTAB_SKIN:
		ChangeSubTab(SUBTAB_SKIN1, false);
		ChangeSkin(m_charTypeId);
		break;

	case MAINTAB_INTERIOR:
		ChangeSubTab(SUBTAB_DECORATE, false);
		m_anotherHand.WaitUntilWorkDone(9999);
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		break;
	}
}

void CShopMain::OnTabSubInit(int param)
{
	m_pTabSub = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
}

void CShopMain::OnTabSubButtonUp()
{
	TButtonItem* pSelected = m_pTabSub->GetSelected();
	if (!pSelected)
		return;

	ChangeSubTab((subTab_t)pSelected->index, false);
}

void CShopMain::OnItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pItemList)
		return;

	m_pItemList->UseRightButton(true);
	m_pItemList->UseDummy(true);
	m_pItemList->SetPushSound(NULL);
	m_pItemList->AddNoRButtonRect(m_miniBtn[1].rect, 1);
	m_pItemList->AddNoRButtonRect(m_miniBtn[2].rect, 2);
}

void CShopMain::DrawStatus(FrGraphicInterface* pGI, FrListItem* pItem,
	short* const stat, short* const upgrade, float y)
{
	float dy = 4.0f;
	for (int i = 0; i < 5; ++i)
	{
		const Bitmap* pDot[2];
		for (int n = 0; n < 2; ++n)
			pDot[n] = m_pStatDot[i][n];
		int nStat = stat[i];
		int nUpgrade = upgrade[i];
		if (nUpgrade || nStat)
		{
			float x = 86.0f;
			for (int j = 0; j < nStat; ++j)
			{
				pGI->DrawTexture(pDot[0],
					WRect(pItem->pos.x + x + y, pItem->pos.y + dy + y, 5.0f,
						6.0f),
					0xffffffff, 0);
				x -= 6.0f;
			}
			for (int k = 0; k < nUpgrade; ++k)
			{
				pGI->DrawTexture(pDot[1],
					WRect(pItem->pos.x + x + y, pItem->pos.y + dy + y, 5.0f,
						6.0f),
					0xffffffff, 0);
				x -= 6.0f;
			}
			dy += 6.0f;
		}
	}
}

void CShopMain::DrawStatus2(FrGraphicInterface* pGI, FrListItem* pItem,
	short* const stat, short* const upgrade, float y)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		if (stat[i] || upgrade[i])
		{
			pGI->SetTextColor(0xff0000ff, 0xffffffff);
			pGI->Print11(pItem->pos + WPoint(182.0f + y, 36.0f + y + dy), 2,
				MakeStatusString(i, stat[i], upgrade[i]));
			dy += 14.0f;
		}
	}
}

void CShopMain::DrawStatusCard(FrGraphicInterface* pGI, FrListItem* pItem,
	short* const stat, float y)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		if (stat[i])
		{
			pGI->SetTextColor(0xff0000ff, 0xffffffff);
			pGI->Print11(pItem->pos + WPoint(182.0f + y, 36.0f + y + dy), 2,
				MakeCardStatusString(i, stat[i]));
			dy += 14.0f;
		}
	}
}

void CShopMain::DrawPartsSlot(FrGraphicInterface* pGI, FrListItem* pItem)
{
	IFF_STRUCT::sPart* pPart = (IFF_STRUCT::sPart*)pItem->pData;
	if (!pPart)
		return;

	if (!pGI)
	{
		pGI = g_pFresh->GetManager()->GetGDI();
		if (!pGI)
			return;
	}

	float x = 5.0f;
	if (pPart->Beginner && m_pMarkBegin)
	{
		pGI->DrawTexture(m_pMarkBegin,
			WRect(pItem->pos.x + 5.0f, pItem->pos.y + 65.0f,
				(float)m_pMarkBegin->Width(), (float)m_pMarkBegin->Height()),
			0xffffffff, 0);
	}

	for (int i = 0; i < pPart->CharacterSlot; ++i)
	{
		if (m_pEmptyCharCard)
			m_pEmptyCharCard->Render(g_view, WRect(0, 0, 1.0f, 1.0f),
				WRect(x + pItem->pos.x, pItem->pos.y + 54.0f, 12.0f, 12.0f), 0,
				0xffffffff, 0, 0);
		x += 16.0f;
	}

	x = 5.0f;
	for (int j = 0; j < pPart->CaddieSlot; ++j)
	{
		if (m_pEmptyCaddieCard)
			m_pEmptyCaddieCard->Render(g_view, WRect(0, 0, 1.0f, 1.0f),
				WRect(x + pItem->pos.x, pItem->pos.y + 70.0f, 12.0f, 12.0f), 0,
				0xffffffff, 0, 0);
		x += 16.0f;
	}
}

void CShopMain::OnItemListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;
	const Bitmap* pBmp;

	if (pCommon == NULL)
		pBmp = m_pItemBase[3];
	else if (pItem->selected)
	{
		pBmp = m_pItemBase[2];
		pGI->SetTextColor(0xffff0000, 0xffffffff);
		pGI->SetTextStyle(0);
	}
	else if (pItem->underCursor)
	{
		pBmp = m_pItemBase[1];
		pGI->SetTextColor(0xff000000, 0xffffffff);
		pGI->SetTextStyle(0);
	}
	else
	{
		pBmp = m_pItemBase[0];
		pGI->SetTextColor(0xff000000, 0xffffffff);
		pGI->SetTextStyle(0);
	}

	if (pBmp)
	{
		pGI->DrawTexture(pBmp,
			WRect(pItem->pos.x, pItem->pos.y, (float)pBmp->Width(),
				(float)pBmp->Height()),
			0xffffffff, 0);
	}

	if (pCommon == NULL)
		return;

	if ((pCommon->TypeId & 0xfc000000) == 0x38000000 &&
		(pCommon->TypeId & 0x3c00000) == 0x1800000)
	{
		const Bitmap* pTitleBg = g_pFresh->GetBitmap("Title_BG");
		pGI->DrawTexture(pTitleBg,
			WRect(pItem->pos.x + 7.0f, pItem->pos.y + 7.0f,
				(float)pTitleBg->Width(), (float)pTitleBg->Height()),
			0xffffffff, 0);
	}

	pBmp = GetItemIcon(pCommon->TypeId);
	if (UccManager()->IsDisplayUccClothes(pCommon))
	{
		sDisplayUccClothes* pDisplay = (sDisplayUccClothes*)pItem->pData;
		pBmp = GetUccItemIcon(pDisplay->itemId);
	}

	if (pBmp)
	{
		float offsetX;
		float offsetY;
		switch (m_subTab)
		{
		case SUBTAB_PACKAGE:
			if ((pCommon->TypeId & 0xfc000000) == 0x24000000)
			{
				offsetX = 7.0f;
				offsetY = 2.0f;
			}
			else
			{
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;

		case SUBTAB_CADDIE:
			if ((pCommon->TypeId & 0xfc000000) == 0x20000000)
			{
				offsetX = 7.0f;
				offsetY = 2.0f;
			}
			else
			{
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;

		case SUBTAB_PARTS1:
		case SUBTAB_PARTS2:
		case SUBTAB_PARTS3:
		case SUBTAB_PARTS4:
		case SUBTAB_HAIR:
		case SUBTAB_MASCOT:
		case SUBTAB_UCC:
			offsetX = 7.0f;
			offsetY = 2.0f;
			break;

		case SUBTAB_SKIN3:
			offsetX = 7.0f;
			offsetY = 12.0f;
			break;

		case SUBTAB_CARD:
		case SUBTAB_CUTIN:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;

		case SUBTAB_INTERIOR1:
		case SUBTAB_INTERIOR2:
		case SUBTAB_INTERIOR3:
		case SUBTAB_FURNITURE:
		case SUBTAB_DECORATE:
			offsetX = 21.0f;
			offsetY = 12.0f;
			break;

		default:
			offsetX = 16.0f;
			offsetY = 27.0f;
			break;
		}

		pGI->DrawTexture(pBmp,
			WRect(offsetX + pItem->pos.x, offsetY + pItem->pos.y,
				(float)pBmp->Width(), (float)pBmp->Height()),
			0xffffffff, 0);
	}

	if (UccManager()->IsDisplayUccClothes(pCommon))
	{
		if (m_pItemBase[4])
		{
			pGI->DrawTexture(m_pItemBase[4],
				WRect(pItem->pos.x, pItem->pos.y,
					(float)m_pItemBase[4]->Width(),
					(float)m_pItemBase[4]->Height()),
				0xffffffff, 0);
		}
	}

	pGI->SetTextStyle(0);
	switch (m_subTab)
	{
	case SUBTAB_AZTEC:
	{
		IFF_STRUCT::sBall* pBall = (IFF_STRUCT::sBall*)pCommon;
		if ((pCommon->TypeId & 0xfc000000) == 0x24000000)
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				"%s", pCommon->Name);
		}
		else
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				"%s(%d\xb0\xb3)", pCommon->Name, pBall->COM[0]);
		}
	}
	break;

	case SUBTAB_EQUIP:
	case SUBTAB_NOEQUIP:
	{
		IFF_STRUCT::sItem* pItemInfo = (IFF_STRUCT::sItem*)pCommon;
		if (pItemInfo->COM[0] > 0)
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				"%s(%d\xb0\xb3)", pCommon->Name, pItemInfo->COM[0]);
		}
		else
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				pCommon->Name);
		}
	}
	break;

	case SUBTAB_CARD:
		if (IsLocalContent((localContentType_t)0x51))
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				pCommon->Name);
		}
		break;

	case SUBTAB_INTERIOR1:
	case SUBTAB_INTERIOR2:
	case SUBTAB_INTERIOR3:
	case SUBTAB_DECORATE:
		if (IsLocalContent((localContentType_t)0x56))
		{
			IFF_STRUCT::sFurniture* pFurniture =
				(IFF_STRUCT::sFurniture*)pCommon;
			if (pFurniture->COM[0] > 0)
			{
				pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f),
					2, "%s(%d\xb0\xb3)", pCommon->Name, pFurniture->COM[0]);
			}
			else
			{
				pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f),
					2, pCommon->Name);
			}
		}
		break;

	case SUBTAB_FURNITURE:
		if ((pCommon->TypeId & 0xfc000000) == 0x48000000)
		{
			IFF_STRUCT::sFurniture* pFurniture =
				(IFF_STRUCT::sFurniture*)pCommon;
			if (pFurniture->COM[0] > 0)
			{
				pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f),
					2, "%s(%d\xb0\xb3)", pCommon->Name, pFurniture->COM[0]);
			}
			else
			{
				pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f),
					2, pCommon->Name);
			}
		}
		break;

	default:
		if (UccManager()->IsDisplayUccClothes(pCommon))
		{
			sDisplayUccClothes* pDisplay = (sDisplayUccClothes*)pItem->pData;
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				GetUccItemName(pDisplay->itemId));
			g_pFresh->GetManager()->PrintText(
				WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 23.0f), 2,
				pDisplay->name.c_str(), -1.0f, 0xffffffff);
		}
		else
		{
			pGI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
				pCommon->Name);
		}
		break;
	}

	if (IsSelectedRentalTab())
		DrawRentalPrice(182.0f, 23.0f, *pCommon, pItem);
	else
		DrawItemPrice(pGI, 182.0f, 23.0f, *pCommon, pItem);

	switch (m_subTab)
	{
	case SUBTAB_PARTS1:
	case SUBTAB_PARTS2:
	case SUBTAB_PARTS3:
	case SUBTAB_PARTS4:
	case SUBTAB_UCC:
	{
		IFF_STRUCT::sPart* pPart = (IFF_STRUCT::sPart*)pItem->pData;
		DrawStatus2(pGI, pItem, pPart->Attr, pPart->Slot, 0.0f);
		if (IsLocalContent((localContentType_t)0x51))
			DrawPartsSlot(pGI, pItem);
	}
	break;
	}

	if (pItem->selected && pCommon->Level > Doc()->m_myInfo.stat.Level)
	{
		const Bitmap* pLevel =
			g_pFresh->GetBitmap(MakeStr("level_%03d", pCommon->Level + 1));
		if (pLevel)
		{
			pGI->DrawTexture(pLevel,
				WRect(pItem->pos.x + 10.0f, pItem->pos.y + 4.0f,
					(float)pLevel->Width(), (float)pLevel->Height()),
				0xffffffff, 0);
		}
	}

	if (m_pMarkLimited)
	{
		if (pCommon->saleStart.wYear || pCommon->saleEnd.wYear)
		{
			pGI->DrawTexture(m_pMarkLimited,
				WRect(pItem->pos.x, pItem->pos.y,
					(float)m_pMarkLimited->Width(),
					(float)m_pMarkLimited->Height()),
				0xffffffff, 0);
		}
	}

	if (pCommon->New && m_pMarkNew)
	{
		pGI->DrawTexture(m_pMarkNew,
			WRect(pItem->pos.x, pItem->pos.y, (float)m_pMarkNew->Width(),
				(float)m_pMarkNew->Height()),
			0xffffffff, 0);
	}

	if (pCommon->Hit && m_pMarkHot)
	{
		pGI->DrawTexture(m_pMarkHot,
			WRect(pItem->pos.x, pItem->pos.y, (float)m_pMarkHot->Width(),
				(float)m_pMarkHot->Height()),
			0xffffffff, 0);
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	bool bBuy = pCommon->InStock == 1 || pCommon->InStock == 3;

	if ((pCommon->TypeId & 0xfc000000) == 0x38000000 &&
		(pCommon->TypeId & 0x3c00000) == 0x1800000)
	{
		Title* pTitle =
			g_TitleManager->GetTitle((eTitle)(pCommon->TypeId & 0x3fffff));
		if (pTitle)
		{
			if (!pTitle->CanEquip())
			{
				const Bitmap* pNoPurchase =
					g_pFresh->GetBitmap("do not purchase");
				if (pNoPurchase)
				{
					pGI->DrawTexture(pNoPurchase,
						WRect(WPoint(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f),
							pNoPurchase->Width(), pNoPurchase->Height()),
						0xffffffff, 0);
					bBuy = false;
				}
			}
		}
	}

	if (bBuy)
	{
		if (m_miniBtn[1].bEnable == false)
		{
			pGI->DrawTexture(m_miniBtn[1].pNormal,
				m_miniBtn[1].rect + pItem->pos + WPoint(0.0f, 0.0f), 0x88ffffff,
				0);
		}
		else if (pItem->underCursor &&
			(m_miniBtn[1].rect + pItem->pos).IsInRect(mousePos))
		{
			pGI->DrawTexture(m_miniBtn[1].pOver,
				m_miniBtn[1].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xffffffff,
				0);
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_BUY);
		}
		else
		{
			pGI->DrawTexture(m_miniBtn[1].pNormal,
				m_miniBtn[1].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xffffffff,
				0);
		}
	}

	bool bGift = pCommon->InStock == 1 || pCommon->InStock == 2;

	if ((m_pSideTabRental == NULL ||
			m_pSideTabRental->GetStatus() != FrButton::PRESSED ||
			m_mainTab != MAINTAB_CHARACTER) &&
		bGift)
	{
		if (m_miniBtn[2].bEnable == false)
		{
			pGI->DrawTexture(m_miniBtn[2].pNormal,
				m_miniBtn[2].rect + pItem->pos, 0x88ffffff, 0);
		}
		else if (pItem->underCursor &&
			(m_miniBtn[2].rect + pItem->pos).IsInRect(mousePos))
		{
			pGI->DrawTexture(m_miniBtn[2].pOver, m_miniBtn[2].rect + pItem->pos,
				0xffffffff, 0);
			CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_GIFT);
		}
		else
		{
			pGI->DrawTexture(m_miniBtn[2].pNormal,
				m_miniBtn[2].rect + pItem->pos, 0xffffffff, 0);
		}
	}
}

void CShopMain::OnItemListBtnDown()
{
	FrListItem* pItem = m_pItemList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	if (m_anotherHand.IsWorking(0))
	{
		m_pItemList->ToggleItem(pItem);
		return;
	}

	IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;
	CPartTidList* pParts = &Doc()->GetMyPartTidList();

	if (UccManager()->IsDisplayUccClothes(pCommon))
	{
		sDisplayUccClothes* pDisplay = (sDisplayUccClothes*)pItem->pData;
		IFF_STRUCT::sPart* pPart =
			ItemManager()->FindPart(pDisplay->partTypeId);
		sUccClothes* pClothes = UccManager()->FindClothes(pDisplay->itemId);
		if (pPart && pClothes)
		{
			pDisplay->part.PosMask = pPart->PosMask;
			pDisplay->part.HideMask = pPart->HideMask;
			strcpy(pDisplay->part.Data, pPart->Data);
			strcpy(pDisplay->part.Tex[0],
				GenerateTextureName_F(pClothes->typeId, pClothes->uccIndex)
					.c_str());
			strcpy(pDisplay->part.Tex[1],
				GenerateTextureName_B(pClothes->typeId, pClothes->uccIndex)
					.c_str());
			strcpy(pDisplay->part.OrgTex[0], pPart->Tex[0]);
			strcpy(pDisplay->part.OrgTex[1], pPart->Tex[1]);

			memcpy(pDisplay->part.SubPart, pPart->SubPart,
				sizeof(pDisplay->part.SubPart));
		}
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
	if ((m_miniBtn[1].rect + pItem->pos).IsInRect(mousePos))
	{
		bool bBuy = pCommon->InStock == 1 || pCommon->InStock == 3;

		if (m_miniBtn[1].bEnable && bBuy)
		{
			ResetItemList(false);
			ClearCartList(false, pParts);
			m_oldItemList.clear();

			m_pItemList->SelectItem(pItem, true);

			if ((pCommon->TypeId >> 26) == 15 && !HaveCharacter(m_charTypeId))
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 "
						 "\xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 "
						 "\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
			}
			else if ((pCommon->TypeId >> 26) == 1 &&
				HaveCharacter(m_charTypeId))
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 "
						 "\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed "
						 "\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
			}
			else if (SelectItemList(pItem))
			{
				char allowed = 0;
				IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
				actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);

				if (allowed == 1)
				{
					if ((pCommon->TypeId & 0xfc000000) == 0x20000000 &&
						(pCommon->TypeId & 0x1fe000) == 0x6000)
						UpgradeCaddie(m_caddieTypeId);
					else
						Checkout();
				}
				else
				{
					actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
				}
			}
			return;
		}
	}
	else if ((m_miniBtn[2].rect + pItem->pos).IsInRect(mousePos))
	{
		bool bGift = pCommon->InStock == 1 || pCommon->InStock == 2;

		if (m_miniBtn[2].bEnable && bGift)
		{
			ResetItemList(false);
			m_pItemList->SelectItem(pItem, true);

			if (SelectItemList(pItem))
			{
				char allowed = 0;
				IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
				actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);

				if (allowed == 1)
					SendGift(*pCommon);
				else
					actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
			}
			return;
		}
	}

	if (pItem->selected == false)
	{
		unsigned long group = pCommon->TypeId >> 26;
		if (group == 2 || (group > 13 && group <= 15))
			UnselectItemList(pCommon->TypeId);
		else
			m_pItemList->ToggleItem(pItem);
	}
	else
	{
		if ((pCommon->TypeId >> 26) == 31)
			m_cardTypeId = pCommon->TypeId;
		else if ((pCommon->TypeId >> 26) == 9 &&
			(pCommon->TypeId & 0x1fe000) == 0x1fa000)
			m_cardTypeId = pCommon->TypeId;
		else
			m_cardTypeId = 0;

		SelectItemList(pItem);
	}

	if (m_mainTab == MAINTAB_CHARACTER || m_mainTab == MAINTAB_SKIN)
	{
		m_pBuyAll->Enable(!m_pPartCartList->m_itemList.empty() ||
			!m_pAuxPartCartList->m_itemList.empty());
		m_pBuyReset->Enable(!m_pPartCartList->m_itemList.empty() ||
			!m_pAuxPartCartList->m_itemList.empty());
	}
}

void CShopMain::OnItemListRBtnDown()
{
	if (m_pItemList)
	{
		FrListItem* pItem = m_pItemList->GetItemUnderCursor();
		if (pItem)
		{
			if (m_anotherHand.IsWorking(0))
			{
				m_pItemList->ToggleItem(pItem);
				return;
			}

			if (pItem->selected)
				SelectItemList(pItem);

			m_pItemList->SelectItem(pItem, true);

			IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;
			if (pCommon)
			{
				if (!UccManager()->IsDisplayUccClothes(pCommon))
				{
					OpenInformation(pCommon->TypeId);

					if (m_mainTab == MAINTAB_CHARACTER ||
						m_mainTab == MAINTAB_SKIN)
					{
						m_pBuyAll->Enable(
							!m_pPartCartList->m_itemList.empty() ||
							!m_pAuxPartCartList->m_itemList.empty());
						m_pBuyReset->Enable(
							!m_pPartCartList->m_itemList.empty() ||
							!m_pAuxPartCartList->m_itemList.empty());
					}
				}
			}
		}
	}
}

void CShopMain::OnBuyAllInit(int param)
{
	m_pBuyAll = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBuyAll)
	{
		m_pBuyAll->EnableHover(true, 0.0f);
	}
}

void CShopMain::OnBuyAllUp()
{
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((pCommon->TypeId >> 26) == 1)
		{
			if (HaveCharacter(pCommon->TypeId))
				it = m_pPartCartList->_DelItem(it);
			break;
		}
		else if ((pCommon->TypeId >> 26) == 9)
			break;
	}

	bool bHave = false;
	unsigned long typeId = m_charTypeId;
	std::map<unsigned int, sCharacterInfo>::iterator itChar;
	for (itChar = Doc()->m_charMap.begin(); itChar != Doc()->m_charMap.end();
		++itChar)
	{
		if (typeId == (*itChar).second.tid)
		{
			bHave = true;
			break;
		}
	}

	if (!bHave && it == m_pPartCartList->m_itemList.end())
	{
		HandleMsg(MsgObject(NULL, 0x23,
			(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 "
				 "\xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 "
				 "\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9",
			0, 0, 0, 0));
		return;
	}

	Checkout();

	OnBuyHoverOff(0);
}

void CShopMain::OnBuyResetInit(int param)
{
	m_pBuyReset = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBuyReset)
	{
		m_pBuyReset->EnableHover(true, 0.0f);
	}
}

void CShopMain::OnBuyResetUp()
{
	g_audio->PlaySfx("cloth_off", NULL, 0, NULL, NULL, 0.5f, 200.f);

	if (m_mainTab == MAINTAB_CHARACTER)
	{
		std::map<unsigned int, sCharacterInfo>::const_iterator it;
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			if ((*it).second.tid == m_charTypeId)
				break;
		}

		if (m_subTab == SUBTAB_PACKAGE || m_subTab == SUBTAB_PARTS1 ||
			m_subTab == SUBTAB_PARTS2 || m_subTab == SUBTAB_PARTS3 ||
			m_subTab == SUBTAB_PARTS4 || m_subTab == SUBTAB_HAIR ||
			m_subTab == SUBTAB_UCC)
		{
			if (it != Doc()->m_charMap.end())
			{
				Doc()->BuildMyPartTidList((sCharacterInfo&)(*it).second);

				m_pCharParts = (unsigned long*)(*it).second.tidParts;
				m_pAuxParts = (unsigned long*)(*it).second.tidAuxParts;
				m_pCharStat = (char*)(*it).second.PCL;
				m_hairColor = (*it).second.hairClr;
			}
			else
			{
				Doc()->BuildMyPartTidListDefault(m_charTypeId);
			}

			ClearCartList(true, &Doc()->GetMyPartTidList());
			BuildDisplayStand(m_subTab);
			m_anotherHand.Work(this, 0, &m_charTypeId, true, -1);
			m_pStatBox->SetVisible(true);
			SetLevelBar();
		}
	}
	else if (m_mainTab == MAINTAB_SKIN)
	{
		ResetItemList(false);
		ClearCartList(false, NULL);

		memcpy(m_skinTypeId, Doc()->m_myInfo.userEquip.tidSkin,
			sizeof(m_skinTypeId));
		for (int i = 0; i < 6; i++)
		{
			if (m_skinTypeId[i])
			{
				IFF_STRUCT::sSkin* pSkin =
					ItemManager()->FindSkin(m_skinTypeId[i]);
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

	m_pBuyAll->Enable(false);
	m_pBuyReset->Enable(false);

	OnBuyHoverOff(0);
}

void CShopMain::OnBuyHoverOn(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton == m_pBuyAll)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_BUYALL);
	}
	else if (pButton == m_pBuyReset)
	{
		CIconToolTip::GetInstance()->SetToolTip(TOOLTIP_BUYRESET);
	}
}

void CShopMain::OnBuyHoverOff(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	CIconToolTip* pToolTip = CIconToolTip::GetInstance();

	bool bClear = false;
	switch (pToolTip->GetCurTipIndex())
	{
	case TOOLTIP_BUYALL:
		if (pButton == m_pBuyAll)
			bClear = true;
		break;

	case TOOLTIP_BUYRESET:
		if (pButton == m_pBuyReset)
			bClear = true;
		break;
	}

	if (pButton == NULL || bClear)
		CIconToolTip::GetInstance()->ClearToolTip();
}

void CShopMain::OnSideTabBuyInit(int param)
{
	m_pSideTabBuy = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSideTabBuy)
	{
		m_pSideTabBuy->SetPushDelay(0.2f);
		m_pSideTabBuy->SetStatus(FrButton::PRESSED);
	}
}

void CShopMain::OnSideTabBuyUp()
{
	if (m_pSideTabBuy && m_pSideTabRental)
	{
		if (m_pSideTabBuy->GetStatus() == FrButton::PRESSED)
			return;

		m_pSideTabBuy->SetStatus(FrButton::PRESSED);
		m_pSideTabRental->SetStatus(FrButton::NORMAL);

		m_miniBtn[2].bEnable = true;

		ClearCartList(true, &Doc()->GetMyPartTidList());
		SetLevelBar();
		BuildDisplayStand(m_subTab);
		m_anotherHand.Work(this, 0, &m_charTypeId, true, -1);
	}
}

void CShopMain::OnSideTabRentalInit(int param)
{
	m_pSideTabRental = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pSideTabRental)
	{
		m_pSideTabRental->SetPushDelay(0.2f);
		m_pSideTabRental->SetStatus(FrButton::NORMAL);
	}
}

void CShopMain::OnSideTabRentalUp()
{
	if (m_pSideTabBuy && m_pSideTabRental)
	{
		if (m_pSideTabRental->GetStatus() == FrButton::PRESSED)
			return;

		m_pSideTabRental->SetStatus(FrButton::PRESSED);
		m_pSideTabBuy->SetStatus(FrButton::NORMAL);

		m_miniBtn[2].bEnable = false;

		ClearCartList(false, &Doc()->GetMyPartTidList());
		SetLevelBar();
		BuildDisplayRental(m_subTab);
		m_anotherHand.Work(this, 0, &m_charTypeId, true, -1);
	}
}

void CShopMain::SetVisibleSideTab(bool bVisible)
{
	if (m_pSideTabBuy)
		m_pSideTabBuy->SetVisible(bVisible);
	if (m_pSideTabRental)
		m_pSideTabRental->SetVisible(bVisible);
}

void CShopMain::BuildDisplayRental(subTab_t subTab)
{
	m_pItemList->ClearItem();

	if (subTab > SUBTAB_PACKAGE && subTab <= SUBTAB_PARTS4)
	{
		unsigned long category = GetCategoryFlag(subTab);
		for (PartIterator it = ItemManager()->m_PartMap.begin();
			it != ItemManager()->m_PartMap.end(); ++it)
		{
			IFF_STRUCT::sPart* pPart = &(*it).second;
			if (pPart->RentalPrice > 0)
			{
				if (S5::UnderCategory(m_charTypeId, pPart->c.TypeId, category))
				{
					if (pPart->c.InStock &&
						ItemManager()->IsInSalePeriod(pPart->c.TypeId,
							Doc()->GetServerTime()))
					{
						if (!IsAngelWing(pPart->c.TypeId) ||
							(IsMannerPlayer() &&
								Doc()->m_myInfo.stat.Level >= 21))
						{
							FrListItem* pItem = m_pItemList->AddItem(pPart);
							if (pItem && IsItemInCart(pItem))
								m_pItemList->SelectItem(pItem, true);

							if (pPart->c.InStock == 1 || pPart->c.InStock == 3)
								pItem->AddNoRBtnIdx(1);
						}
					}
				}
			}
		}

		m_pItemList->SortItem(PartCompare);
	}

	for (int i = m_pItemList->GetCurrentItemSize(true); i < 8; i++)
		m_pItemList->AddItem(NULL);
}

void CShopMain::DrawRentalPrice(float x, float y, IFF_ITEM_COMMON& item,
	FrListItem* pItem)
{
	if (pItem == NULL)
		return;

	if (item.InStock == 4)
		return;

	if ((item.TypeId & 0xfc000000) == 0x08000000)
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(item.TypeId);
		if (pPart)
		{
			FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
			const char* str =
				MakeStr("%d\xc6\xce(%d\xc0\xcf)", pPart->RentalPrice, 7);
			pGI->Print11(WPoint(x + pItem->pos.x, y + pItem->pos.y), 2, str);
		}
	}
}

bool CShopMain::IsSelectedRentalTab()
{
	if (m_mainTab == MAINTAB_CHARACTER && m_pSideTabRental)
	{
		if (m_pSideTabRental->GetStatus() == FrButton::PRESSED)
			return true;
	}

	return false;
}

void CShopMain::OnPartCartListInit(int param)
{
	m_pPartCartList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void CShopMain::OnPartCartListOwnerDraw(int param)
{
	static int defaultPartPos[10] = { 3, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL)
		return;

	IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;
	if (pCommon == NULL)
		return;

	bool bStat = false;
	short stat[5];
	short upgrade[5];
	memset(stat, 0, sizeof(stat));
	memset(upgrade, 0, sizeof(upgrade));
	int slot;

	switch (pCommon->TypeId >> 26)
	{
	case 2:
	{
		IFF_STRUCT::sPart* pPart = (IFF_STRUCT::sPart*)pCommon;
		for (int i = 0; i < 5; i++)
		{
			if (pPart->Attr[i] || pPart->Slot[i])
			{
				memcpy(stat, pPart->Attr, sizeof(stat));
				memcpy(upgrade, pPart->Slot, sizeof(upgrade));
				bStat = true;
				break;
			}
		}

		switch (pPart->Category)
		{
		case 0:
			slot = 1;
			break;
		case 1:
			slot = 2;
			break;
		case 4:
			slot = 3;
			break;
		case 7:
		case 8:
		case 9:
		{
			int character = (pCommon->TypeId >> 18) & 0xff;
			if (character < 10)
			{
				if (((pCommon->TypeId >> 13) & 0x1f) ==
					(unsigned)defaultPartPos[character])
					slot = 1;
				else
					slot = 2;
			}
			else
				slot = 0;
		}
		break;
		}
	}
	break;

	case 15:
		slot = 0;
		break;

	case 9:
	{
		int sub = (pCommon->TypeId >> 21) & 0x1f;
		if (sub == 6 || sub == 5 || sub == 4 || sub == 8)
			return;
	}
		slot = 1;
		break;

	case 14:
		slot = (pCommon->TypeId >> 22) & 0xf;
		break;

	default:
		return;
	}

	if (slot < 0)
		return;

	float slotW = (float)m_pCartSlot->Width();
	float slotH = (float)m_pCartSlot->Height();
	float iconW = (float)m_pStatIcon[0]->Width();
	float iconH = (float)m_pStatIcon[0]->Height();
	float cancelW = (float)m_pCancelBtn[0]->Width();
	float cancelH = (float)m_pCancelBtn[0]->Height();

	float x = slot > 3 ? 250.0f : 8.0f;
	int row = slot % 4;
	int icon = slot > 3 ? 4 : slot;

	pGI->DrawTexture(m_pCartSlot, WRect(x, row * slotH + 129.0f, slotW, slotH),
		0xffffffff, 0);

	if ((pCommon->TypeId & 0xfc000000) == 0x38000000)
		pGI->DrawTexture(m_pStatIcon[4],
			WRect(x + 4.0f, row * slotH + 134.0f, iconW, iconH), 0xffffffff, 0);
	else
		pGI->DrawTexture(m_pStatIcon[icon],
			WRect(x + 4.0f, row * slotH + 134.0f, iconW, iconH), 0xffffffff, 0);

	pGI->SetTextColor(0xffffffff, 0xff36779a);
	pGI->SetTextStyle(4);

	float textX = x + 8.0f;
	pGI->Print11(WPoint(textX, row * slotH + 156.0f), 0, pCommon->Name);

	pGI->SetTextColor(0xffffffff, 0xffffffff);
	pGI->SetTextStyle(0);
	if (bStat)
		DrawStatus3(pGI, textX, row * slotH + 170.0f, stat, upgrade);

	WRect rect(x + 104.0f, row * slotH + 141.0f, cancelW, cancelH);
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		pGI->DrawTexture(m_pCancelBtn[1], rect, 0xffffffff, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_removeList.push_back(pCommon->TypeId);
	}
	else
		pGI->DrawTexture(m_pCancelBtn[0], rect, 0xffffffff, 0);
}

void CShopMain::DrawStatus3(FrGraphicInterface* pGI, float x, float y,
	short* const stat, short* const upgrade)
{
	float offset = 0.0f;
	for (int i = 0; i < 5; i++)
	{
		if (stat[i] || upgrade[i])
		{
			pGI->Print11(WPoint(x, y + offset), 0,
				MakeStatusString(i, stat[i], upgrade[i]));
			offset += 12.0f;
		}
	}
}

void CShopMain::OnAuxPartCartListInit(int param)
{
	m_pAuxPartCartList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void CShopMain::OnAuxPartCartListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL)
		return;

	IFF_STRUCT::sPart* pPart = (IFF_STRUCT::sPart*)pItem->pData;
	if (pPart == NULL)
		return;

	bool bStat = false;
	short upgrade[5];
	short stat[5];
	memset(stat, 0, sizeof(stat));
	memset(upgrade, 0, sizeof(upgrade));
	for (int i = 0; i < 5; i++)
	{
		if (pPart->Attr[i] || pPart->Slot[i])
		{
			memcpy(stat, pPart->Attr, sizeof(stat));
			memcpy(upgrade, pPart->Slot, sizeof(upgrade));
			bStat = true;
			break;
		}
	}

	int no = pItem->no;

	float slotW = (float)m_pCartSlot->Width();
	float slotH = (float)m_pCartSlot->Height();
	float iconW = (float)m_pStatIcon[0]->Width();
	float iconH = (float)m_pStatIcon[0]->Height();
	float cancelW = (float)m_pCancelBtn[0]->Width();
	float cancelH = (float)m_pCancelBtn[0]->Height();

	float top = no * slotH + 129.0f;

	pGI->DrawTexture(m_pCartSlot, WRect(250.0f, top, slotW, slotH), 0xffffffff,
		0);
	pGI->DrawTexture(m_pStatIcon[4], WRect(254.0f, top + 5.0f, iconW, iconH),
		0xffffffff, 0);

	pGI->SetTextColor(0xffffffff, 0xff36779a);
	pGI->SetTextStyle(4);
	pGI->Print11(WPoint(258.0f, top + 27.0f), 0, pPart->c.Name);
	pGI->SetTextColor(0xffffffff, 0xffffffff);
	pGI->SetTextStyle(0);
	if (bStat)
		DrawStatus3(pGI, 258.0f, top + 41.0f, stat, upgrade);

	WRect rect(354.0f, no * slotH + 141.0f, cancelW, cancelH);
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		pGI->DrawTexture(m_pCancelBtn[1], rect, 0xffffffff, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_removeList.push_back(pPart->c.TypeId);
	}
	else
	{
		pGI->DrawTexture(m_pCancelBtn[0], rect, 0xffffffff, 0);
	}
}

void CShopMain::CheckAuxCartScroll()
{
	FrScrollBar* pScrollBar = m_pAuxPartCartList->GetScrollBar();
	int topRow = pScrollBar->GetCurTopRow_Int();

	if (pScrollBar->GetRowCapacity() < pScrollBar->GetItemNum())
	{
		m_pScrollUp->SetVisible(true);
		m_pScrollDown->SetVisible(true);

		if (topRow == 0)
		{
			m_pScrollUp->Enable(false);
			m_pScrollDown->Enable(true);
		}
		else
		{
			m_pScrollUp->Enable(true);
			m_pScrollDown->Enable(false);
		}

		if (m_pBuyReset)
		{
			WRect resetRect;
			WRect downRect;
			WRect upRect;
			m_pBuyReset->GetClientRect(resetRect);
			m_pScrollDown->GetClientRect(downRect);
			m_pScrollUp->GetClientRect(upRect);

			downRect.x = resetRect.x - downRect.w;
			upRect.x = downRect.x - upRect.w - 2.0f;
			upRect.y = (resetRect.h - downRect.h) * 0.5f + resetRect.y;
			downRect.y = upRect.y;

			m_pScrollUp->SetClientRect(upRect);
			m_pScrollDown->SetClientRect(downRect);
		}
	}
	else
	{
		m_pScrollUp->SetVisible(false);
		m_pScrollDown->SetVisible(false);
	}
}

void CShopMain::TurnOffAuxCartScroll()
{
	m_pScrollUp->SetVisible(false);
	m_pScrollDown->SetVisible(false);
}

void CShopMain::TurnOnAuxCartScroll()
{
	m_pScrollUp->SetVisible(true);
	m_pScrollDown->SetVisible(true);
}

void CShopMain::OnScrollUpInit(int param)
{
	m_pScrollUp = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pScrollUp)
		m_pScrollUp->SetVisible(false);
}

void CShopMain::OnScrollUpUp()
{
	m_pAuxPartCartList->GetScrollBar()->ScrollUp(-1);
	CheckAuxCartScroll();
}

void CShopMain::OnScrollDownInit(int param)
{
	m_pScrollDown = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pScrollDown)
		m_pScrollDown->SetVisible(false);
}

void CShopMain::OnScrollDownUp()
{
	m_pAuxPartCartList->GetScrollBar()->ScrollDown(-1);
	CheckAuxCartScroll();
}

void CShopMain::OnItemSlotFrame1Init(int param)
{
	m_pItemSlotFrame1 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pItemSlotFrame1)
		m_pItemSlotFrame1->SetVisible(false);
}

void CShopMain::OnItemSlotFrame2Init(int param)
{
	m_pItemSlotFrame2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pItemSlotFrame2)
		m_pItemSlotFrame2->SetVisible(false);
}

void CShopMain::BuildMyItemSlot(bool bRebuild)
{
	m_pItemSlot->ClearItem();

	for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); ++it)
	{
		sItemInfo& item = *it;

		if (ItemManager()->GetCouponKind(item.tid))
			continue;

		if (ItemManager()->IsNonVisibleItem(item.tid) == true)
			continue;

		if (item.tid == 0x1a000042)
			continue;

		int count = 0;

		if (bRebuild)
		{
			if (!(item.tid & 0x2000000))
				continue;

			for (int i = 0; i < 10; i++)
				if (item.tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
					count++;

			if (item.Common[0] - count > 0)
				m_pItemSlot->AddItem(&item);
		}
		else if (IsLocalContent(S4_NT_SC_ITEMSLOT))
		{
			CCardManager::Instance()->SetPlayerIndex(0xff);
			CCardManager::Instance()->CalcCardPeriodAndStatus();

			int numSlot = Doc()->GetNumAddItemSlotByMascot() + 8;

			int cardSlot = CCardManager::Instance()->GetCardPeriodSlot();
			if (cardSlot > 0)
				numSlot += cardSlot;

			if (item.tid & 0x2000000)
				continue;

			for (int i = 0; i < 10; i++)
			{
				if (i < numSlot)
				{
					if (item.tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
						count++;
				}
				else
				{
					if (item.tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
						Doc()->m_myInfo.userEquip.tidItemSlot[i] = 0;
				}
			}

			if (item.Common[0] - count > 0)
				m_pItemSlot->AddItem(&item);
		}
		else
		{
			if (item.tid & 0x2000000)
				continue;

			for (int i = 0; i < 10; i++)
				if (item.tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
					count++;

			if (item.Common[0] - count > 0)
				m_pItemSlot->AddItem(&item);
		}
	}
}

void CShopMain::OnItemSlotInit(int param)
{
	m_pItemSlot = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pItemSlot)
	{
		m_pItemSlot->SetVisible(false);
		m_pItemSlot->SetMouseEvent(false);
	}
}

void CShopMain::OnItemSlotOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL)
		return;

	const Bitmap* pBitmap = m_pItemSlotBmp;
	if (pBitmap)
		pGI->DrawTexture(pBitmap,
			WRect(pItem->pos.x, pItem->pos.y, (float)m_pItemSlotBmp->Width(),
				(float)m_pItemSlotBmp->Height()),
			0xffffffff, 0);

	sItemInfo* pInfo = (sItemInfo*)pItem->pData;
	if (pInfo == NULL)
		return;

	IFF_STRUCT::sItem* pIffItem = ItemManager()->FindItem(pInfo->tid);
	if (pIffItem == NULL)
		return;

	pBitmap = g_pFresh->GetBitmap(pIffItem->c.Icon);
	if (pBitmap)
		pGI->DrawTexture(pBitmap,
			WRect(pItem->pos.x + 2.0f, pItem->pos.y + 4.0f,
				(float)pBitmap->Width(), (float)pBitmap->Height()),
			0xffffffff, 0);

	int count = 0;
	if (!(pInfo->tid & 0x2000000))
		for (int i = 0; i < 10; i++)
			if (pInfo->tid == Doc()->m_myInfo.userEquip.tidItemSlot[i])
				count++;

	int num = pInfo->Common[0] - count;
	Print(pItem->pos.x + 45.0f, pItem->pos.y + 40.0f, MakeStr("%d", num), 2.0f,
		2, 1.0f, 0xffffffff);
}

void CShopMain::OnPowerEditInit(int param)
{
	m_pStatEdit[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CShopMain::OnControlEditInit(int param)
{
	m_pStatEdit[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CShopMain::OnImpactEditInit(int param)
{
	m_pStatEdit[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CShopMain::OnSpinEditInit(int param)
{
	m_pStatEdit[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CShopMain::OnCurveEditInit(int param)
{
	m_pStatEdit[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

bool CShopMain::OnBuyDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pPartCartList->m_itemList.begin();
			it != m_pPartCartList->m_itemList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

			if ((pItem->TypeId & 0xfc000000) == 0x08000000 &&
				IsUccClothes(pItem->TypeId))
				RestoreShoppersOldItem(pItem->TypeId,
					Doc()->GetMyPartTidList());
		}

		ResetItemList(false);
		ClearCartList(false, NULL);
		m_oldItemList.clear();
	}

	m_pCheckoutDlg = NULL;
	return true;
}

bool CShopMain::OnGiftDlgResult(int result, FrForm* pForm)
{
	m_pGiftDlg = NULL;
	return true;
}

bool CShopMain::OnSellDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		FrRepayDlg* pDlg = DYNAMIC_CAST(FrRepayDlg, pForm);
		if (pDlg)
		{
			this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);

			int type;
			switch (pDlg->GetTid() >> 26)
			{
			case 1:
				type = 0;
				break;
			case 7:
				type = 1;
				break;
			default:
				type = 2;
				break;
			}

			WSendPacket send((enumClientPacket)0x1e);
			send.Encode1(type);
			send.Encode1(1);
			send.Encode4(pDlg->GetTid());
			send.Encode4(pDlg->GetGuid());
			send.Send(TO_GAME);
		}
	}

	return true;
}

bool CShopMain::OnReemployeeResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		for (CADDIE_MAP_IT it = Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
		}
	}

	return true;
}

CShopMain::CShopMain()
{
	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		m_cardTypeId = 0;
		LoadCardImages();
	}

	m_cutinImg.clear();
	InitalizeUpgradeCaddieInfo();
	BuildVisibleCaddieList();
}

CShopMain::~CShopMain()
{
	if (IsLocalContent(S4_CARD_SYSTEM))
		DeleteCardImages();

	DeleteCutinImages();

	m_anotherHand.WaitUntilWorkDone(9999);
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
}

void CShopMain::OnPreLoadInit()
{
	CTaskMain::OnPreLoadInit();

	InitShopControls();
	m_pCheckoutDlg = NULL;
	m_pOptionDlg = NULL;
	m_pKbdManualDlg = NULL;
	m_pServerDlg = NULL;
	m_pGiftDlg = NULL;
	m_pWaitDlg = NULL;
	m_pNotifyOneDlg = NULL;
	m_fKbdManualTime = -10.0f;

	m_bSelectItemList = false;
	m_caddieTypeId = 0x1c000000;
	m_charTypeId = 0;
	m_mascotTypeId = 0x40000000;
	m_clubSetTypeId = 0x0c000000;
	m_aztecTypeId = 0x14000000;
	m_pCharStat = NULL;
	m_pCharParts = NULL;
	m_pAuxParts = NULL;
	m_hairColor = 0;
	m_removeList.clear();
	m_statTab = STATTAB_PART;
	m_mainTab = MAINTAB_CHARACTER;

	m_pSortOptionCombo = NULL;
	m_pSearchGoods = NULL;
	m_subTab = SUBTAB_PARTS1;
}

void CShopMain::OnLoad()
{
	CTaskMain::OnLoad();
	LoadShopImages();
}

void CShopMain::OnInit()
{
	CTaskMain::OnInit();
}

void CShopMain::OnDestroy()
{
	CTaskMain::OnDestroy();
}

void CShopMain::OnProcess(float delta)
{
	CTaskMain::OnProcess(delta);

	if (m_anotherHand.IsWorking(0))
	{
		Sleep(10);
	}
	else
	{
		m_anotherHand.Enter();

		bool bInput;
		switch (m_mainTab)
		{
		case MAINTAB_ITEM:
		case MAINTAB_SKIN:
			bInput = false;
			break;
		default:
			bInput = true;
			break;
		}

		if (m_pExhibition)
			m_pExhibition->Process(delta, bInput);

		m_anotherHand.Leave();
	}

	if (m_popupTime > 0.0f)
	{
		m_popupTime -= delta;

		if (m_popupTime < 0.1f)
		{
			m_popupTime = -10.0f;
			if (m_pPopupEdit)
				m_pPopupEdit->SetVisible(false);
		}
	}

	if (m_pCheckoutDlg)
		m_pCheckoutDlg->Process();

	if (m_mainTab == MAINTAB_SKIN || m_mainTab == MAINTAB_INTERIOR)
	{
		for (int i = 0; i < 2; i++)
		{
			float speed = i == 0 ? 138.0f : 228.0f;
			switch (m_skinScrollDir[i])
			{
			case 0:
				m_skinScroll[i] = 0.0f;
				break;
			case 1:
				m_skinScroll[i] -= speed * delta * 0.25f;
				break;
			case 2:
				m_skinScroll[i] += speed * delta * 0.25f;
				break;
			}
		}
	}

	if (g_input->GetDown("ESCAPE", true) == 1 &&
		!g_pFresh->GetManager()->HasEscKeyWindow())
		CMessengerInfo::Instance()->Close(false);

	if (!m_removeList.empty())
	{
		Doc()->GetMyPartTidList();

		for (std::list<unsigned long>::iterator it = m_removeList.begin();
			it != m_removeList.end(); ++it)
			UnselectItemList(*it);

		m_removeList.clear();

		if (m_mainTab == MAINTAB_CHARACTER || m_mainTab == MAINTAB_SKIN)
		{
			m_pBuyAll->Enable(!m_pPartCartList->m_itemList.empty() ||
				!m_pAuxPartCartList->m_itemList.empty());
			m_pBuyReset->Enable(!m_pPartCartList->m_itemList.empty() ||
				!m_pAuxPartCartList->m_itemList.empty());
		}
	}

	int tip = CIconToolTip::GetInstance()->GetCurTipIndex();
	if (tip == TOOLTIP_BUY || tip == TOOLTIP_GIFT)
		OnBarHoverOff(0);

	m_selArrowTime += delta;
	m_selArrowOffset = fabs(sin(m_selArrowTime * 4.0f)) * 5.0f;
}

void CShopMain::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x241:
	{
		FrWnd* pWnd =
			g_pFresh->GetManager()->GetDesktop()->FindChildForm("notify");
		if (pWnd)
		{
			((FrForm*)pWnd)->SetMessage((const char*)msg.param1, false);
		}
	}
	break;

	case 0x6a:
	{
		if (m_mainTab != msg.param1)
			break;

		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			if (m_charTypeId == it->second.tid)
			{
				if (Doc()->m_myInfo.userEquip.guidChar != it->second.guid)
				{
					WSendPacket send((enumClientPacket)0x20);
					send.Encode1(5);
					send.Encode4(it->second.guid);
					send.Send(TO_GAME);
				}
				break;
			}
		}

		if (it == Doc()->m_charMap.end())
			break;

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(m_charTypeId);
		if (!pChar)
			break;

		sCharacterInfo info = it->second;
		bool bChanged = false;
		int i;
		for (i = 0; i < pChar->nParts + pChar->nAcsries; i++)
		{
			if (info.tidParts[i] != m_pCharParts[i])
			{
				IFF_STRUCT::sPart* pPart =
					ItemManager()->FindPart(m_pCharParts[i]);
				if (pPart)
				{
					unsigned char level = Doc()->m_myInfo.stat.Level;
					if (!pPart->c.IsUnderLvl && pPart->c.Level > level)
						continue;
					if (pPart->c.IsUnderLvl == 1 && pPart->c.Level < level)
						continue;
				}

				if (!IsUccClothes(m_pCharParts[i]))
				{
					info.tidParts[i] = m_pCharParts[i];
					info.ItemIdList[i] = m_partsUid[i];
					bChanged = true;
				}
			}
		}

		for (i = pChar->nParts + pChar->nAcsries; i < 24; i++)
			info.tidParts[i] = 0;

		for (i = 0; i < 5; i++)
		{
			if (info.tidAuxParts[i] != m_pAuxParts[i])
			{
				IFF_STRUCT::sAuxPart* pAuxPart =
					ItemManager()->FindAuxPart(m_pAuxParts[i]);
				if (!pAuxPart ||
					pAuxPart->c.Level <= Doc()->m_myInfo.stat.Level)
				{
					info.tidAuxParts[i] = m_pAuxParts[i];
					bChanged = true;
				}
			}
		}

		if (info.hairClr != m_hairColor)
		{
			info.hairClr = m_hairColor;
			bChanged = true;
		}

		if (bChanged)
		{
			WSendPacket send((enumClientPacket)0x20);
			send.Encode1(0);
			send.EncodeBuffer(&info, sizeof(info));
			send.Send(TO_GAME);
		}
	}
	break;

	case 0xc4:
		if (msg.param1 < 6)
		{
			m_mainTab = (mainTab_t)msg.param1;

			if (msg.param2 < 36)
				m_subTab = (subTab_t)msg.param2;

			if (m_mainTab == MAINTAB_CHARACTER && m_subTab > SUBTAB_HAIR)
				m_subTab = SUBTAB_PACKAGE;
		}
		else if (msg.param1 == 6)
		{
			m_mainTab = MAINTAB_INTERIOR;
			m_subTab = SUBTAB_DECORATE;
		}
		else
		{
			m_mainTab = MAINTAB_CHARACTER;
			m_subTab = SUBTAB_PACKAGE;
		}
		break;

	case 0xc3:
		*(int*)msg.param1 = m_mainTab;
		*(int*)msg.param2 = m_subTab;
		break;

	case 0:
		OpenLayout((const char*)msg.param1);
		break;

	case 0xc6:
		if (msg.param1 == 3)
		{
			m_mainTab = MAINTAB_CHARACTER;
			m_subTab = SUBTAB_UCC;
			ChangeMainTab(MAINTAB_CHARACTER, true);
			ChangeSubTab(SUBTAB_UCC, true);

			unsigned long typeId = msg.param2;
			if (typeId == 0)
				typeId = m_charTypeId;

			ChangeCharacter(typeId);
		}
		else if (msg.param1 == 4)
		{
			if (IsLocalContent(S4_CARD_SYSTEM))
			{
				m_mainTab = MAINTAB_ITEM;
				m_subTab = SUBTAB_CARD;
				ChangeMainTab(MAINTAB_ITEM, true);
				ChangeSubTab(SUBTAB_CARD, true);
				HidePet(true);
			}
		}
		break;

	case 0x49:
	{
		sCharacterInfo* pInfo = (sCharacterInfo*)msg.param2;
		CTask* pTask = CTaskManager::Instance()->GetPreservedTask();
		if (pTask && pTask->GetActor("AvatarChat"))
			pTask->GetActor("AvatarChat")
				<< MsgObject(NULL, 0x49, msg.param1, msg.param2, 0, 0, 0);
		else
			Doc()->m_myInfo.userEquip.guidChar = pInfo->guid;

		ChangeCharacter(pInfo->tid);
	}
	break;

	case 0x4a:
	{
		sCaddieInfo* pInfo = (sCaddieInfo*)msg.param2;
		if (pInfo->tid)
			ChangeCaddie(pInfo->tid);

		Doc()->m_myInfo.userEquip.guidCaddie = pInfo->guid;
	}
	break;

	case 0x4b:
	{
		sItemInfo* pInfo = (sItemInfo*)msg.param2;
		ChangeClubset(pInfo->tid);
		Doc()->m_myInfo.userEquip.guidClubSet = pInfo->guid;
	}
	break;

	case 0x4c:
		ChangeAztec(msg.param2);
		Doc()->m_myInfo.userEquip.tidBall = msg.param2;
		break;

	case 0x4e:
	{
		sMascotInfo* pInfo = (sMascotInfo*)msg.param2;
		if (pInfo->tid)
			ChangeMascot(pInfo->tid);

		Doc()->m_myInfo.userEquip.guidMascot = pInfo->guid;
	}
	break;

	case 0xc7:
		if (m_pNotifyOneDlg)
			m_pNotifyOneDlg->Close((eFormRet)1, true);
		break;

	case 0xb0:
		break;

	case 0xb5:
	{
		sCharacterInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		ItemManager()->GetDefCombo(info.tid, info.tidParts);

		Doc()->m_charMap.insert(
			std::map<unsigned int, sCharacterInfo>::value_type(info.guid,
				info));
		Doc()->m_myInfo.userEquip.guidChar = info.guid;
	}
	break;

	case 0xb6:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		if (msg.param4 == 6 || msg.param4 == 2)
		{
			item.HourRemain = msg.param3 * 24;
			item.ItemType = msg.param4;
		}

		int slot = (item.tid >> 13) & 0x1f;
		m_partsUid[slot] = item.guid;

		if (IsUccClothes(item.tid))
		{
			strncpy(item.UccIndex, ((sBuyItemResult*)msg.param5)->UccIndex, 9);
			item.Seq = 1;
			Doc()->m_uccItemList.push_back(item);
		}
		else
		{
			Doc()->AddMyPartsList(item);
		}
	}
	break;

	case 0xb7:
	{
		sCaddieInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		info.Rent_flag = 1;
		info.Remain_Date = 30;

		if (info.tid >= 0x1c00000c && info.tid <= 0x1c00000e)
		{
			info.Level = msg.param4;
			info.Exp = msg.param3;
		}

		Doc()->m_caddieMap.insert(
			std::map<unsigned int, sCaddieInfo>::value_type(info.guid, info));

		m_caddieTypeId = info.tid;
		BuildVisibleCaddieList();
	}
	break;

	case 0xb8:
	{
		unsigned long typeId = msg.param2;
		int time = msg.param3;
		unsigned int uid = msg.param1;

		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.find(uid);
		if (it == Doc()->m_caddieMap.end())
			break;

		if (!ItemManager()->FindCadItem(typeId))
			break;

		unsigned char days = (unsigned short)time / 24;
		sCaddieInfo& caddie = it->second;
		caddie.tidPart = typeId;
		caddie.Remain_Partdate = time;
		if (caddie.Remain_Date < days)
			caddie.Remain_Date = days;
	}
	break;

	case 0xb9:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		item.ItemType = msg.param4;
		item.HourRemain = msg.param3;
		item.Common[0] = ((sBuyItemResult*)msg.param5)->Count;

		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(item.tid);

		bool bFound = false;
		if (item.ItemType)
		{
			switch (item.ItemType)
			{
			case 3:
				item.HourRemain *= 1;
				break;
			case 4:
				item.HourRemain *= 24;
				break;
			case 5:
				item.HourRemain *= 720;
				break;
			}
		}

		if (!item.ItemType)
		{
			for (ITEM_LIST_IT it = Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
			{
				if (it->tid == item.tid)
				{
					it->HourRemain += item.HourRemain;
					it->Common[0] = item.Common[0];
					bFound = true;
				}
			}
		}

		if (item.ItemType && item.HourRemain > 0)
			Net_StartPeriodItem(item.guid);

		if (!bFound)
			Doc()->m_myItemList.push_back(item);
	}

	case 0xc5:
		SetMyItemSlot();
		break;

	case 0xba:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;

		Doc()->m_clubSetMap.insert(
			std::map<unsigned int, sItemInfo>::value_type(item.guid, item));
	}
	break;

	case 0xbb:
	{
		unsigned long uid = msg.param1;
		sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
		unsigned long typeId = msg.param2;

		IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);
		if (!pBall)
			break;

		ITEM_LIST_IT it;
		for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end();
			++it)
		{
			if (it->tid == typeId)
			{
				if (pResult->Count > 0)
				{
					it->Common[0] = pResult->Count;
				}
				else
				{
					it->Common[0] += pBall->COM[0];
				}
				break;
			}
		}

		if (it == Doc()->m_ballList.end())
		{
			sItemInfo item;
			memset(&item, 0, sizeof(item));
			item.guid = uid;
			item.tid = typeId;
			item.Common[0] =
				pResult->Count > 0 ? pResult->Count : pBall->COM[0];
			Doc()->m_ballList.push_back(item);
		}
	}
	break;

	case 0xbc:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		item.ItemType = msg.param4;
		item.HourRemain = msg.param3 * 24;

		Doc()->m_cutinList.push_back(item);

		for (ITEM_LIST_IT it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			LogOut(0, "cutin %d %d\n", it->guid, it->tid);
		}
	}
	break;

	case 0xbd:
	{
		IFF_STRUCT::sHairStyle* pHair =
			ItemManager()->FindHairStyle(msg.param2);
		if (!pHair)
			break;

		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			if ((it->second.tid & 0x3ffffff) == pHair->cCharID)
			{
				it->second.hairClr = pHair->cHairID;
				break;
			}
		}
	}
	break;

	case 0xbe:
	{
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.guid = msg.param1;
		item.tid = msg.param2;
		item.ItemType = msg.param4;
		item.HourRemain = msg.param3;
		item.Common[0] = ((sBuyItemResult*)msg.param5)->Count;

		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(item.tid);

		bool bFound = false;
		if (item.ItemType)
		{
			switch (item.ItemType)
			{
			case 3:
				item.HourRemain *= 1;
				break;
			case 4:
				item.HourRemain *= 24;
				break;
			case 5:
				item.HourRemain *= 720;
				break;
			}
		}

		if (!item.ItemType)
		{
			for (ITEM_LIST_IT it = Doc()->m_auxPartList.begin();
				it != Doc()->m_auxPartList.end(); ++it)
			{
				if (it->tid == item.tid)
				{
					it->HourRemain += item.HourRemain;
					it->Common[0] = item.Common[0];
					bFound = true;
				}
			}
		}

		if (item.ItemType && item.HourRemain > 0)
			Net_StartPeriodItem(item.guid);

		if (!bFound)
			Doc()->m_auxPartList.push_back(item);
	}
	break;

	case 0xbf:
	{
		sMascotInfo info;
		memset(&info, 0, sizeof(info));
		info.guid = msg.param1;
		info.tid = msg.param2;
		strcpy(info.szMsg, "PANGYA!");
		sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
		info.Remain_Date = pResult->Time;
		info.endDate = pResult->endDate;

		Doc()->AddMascot(&info);
	}
	break;

	case 0x218:
	{
		unsigned long uid = msg.param1;
		unsigned long typeId = msg.param2;
		sBuyItemResult* pResult = (sBuyItemResult*)msg.param5;
		int cardCount;
		if (pResult->Count > 0)
			cardCount = pResult->Count;
		else
			cardCount = 1;

		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
		if (!pCard)
			break;

		sCards card;
		memset(&card, 0, sizeof(card));
		card.uid = uid;
		card.tid = typeId;
		card.cardCount = cardCount;

		CARD_LIST_IT it = std::find_if(Doc()->m_cardList.begin(),
			Doc()->m_cardList.end(), _FindByUid(uid));
		if (it == Doc()->m_cardList.end())
			Doc()->m_cardList.push_back(card);
		else
			(*it).cardCount = cardCount;
	}
	break;

	case 3:
		if (m_pPopupEdit)
		{
			if (!m_pPopupEdit->IsVisible())
			{
				g_audio->PlaySfx("notice_dingdong", NULL, 0, NULL, NULL, 0.5f,
					200.0f);
				m_pPopupEdit->SetVisible(true);
			}

			m_popupTime = 7.0f;
			m_pPopupEdit->AddLine((const char*)msg.param1,
				CChatMsg::Instance()->ConvertToUIColor(msg.param2), false);
		}
		break;

	case 0xc0:
	{
		bool bEnable = msg.param1 != 0;
		for (int i = 0; i < 7; i++)
		{
			m_miniBtn[i].bEnable = bEnable;
		}

		if (m_pSideTabRental)
		{
			if (m_pSideTabRental->GetStatus() == FrButton::PRESSED)
				m_miniBtn[2].bEnable = false;
		}
	}
	break;

	case 0x1fe:
		this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
		break;

	case 0x1ff:
		if (m_pGiftDlg)
			m_pGiftDlg->GiftProgress();
		break;

	case 0x217:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			sFurniture_List furniture;
			unsigned long uid = msg.param1;
			unsigned long typeId = msg.param2;

			IFF_STRUCT::sFurniture* pFurniture =
				ItemManager()->FindFurniture(typeId);
			if (pFurniture)
			{
				memset(&furniture, 0, sizeof(furniture));
				furniture.id = uid;
				furniture.typeId = typeId;
				furniture.x = pFurniture->Pos[0];
				furniture.y = pFurniture->Pos[1];
				furniture.z = pFurniture->Pos[2];
				furniture.r = pFurniture->Pos[3];
				furniture.bArrange = 0;

				Doc()->AddFurniture(furniture);
			}
		}
		break;

	case 0x24f:
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage((const char*)msg.param1, false);
		pForm->Open(NULL, 3);
		pForm->SetTopmost(true);
		pForm->FindNextTopFocus(false);

		BuildVisibleCaddieList();
		ChangeCaddie(msg.param2);
		SetLevelBar();
	}
	break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

void CShopMain::OnAnotherHand(int key, void* pParam)
{
	if (key == 0)
		ShowPet(*(unsigned long*)pParam);
}

void CShopMain::OpenLayout(const char* name)
{
	WSendPacket packet((enumClientPacket)0x5c);
	packet.Send(TO_GAME);

	WSendPacket packet2((enumClientPacket)0x3d);
	packet2.Send(TO_GAME);

	g_pFresh->OpenLayout(name, this, false);
}

void CShopMain::SetPetCtrl(unsigned long typeId)
{
	WRect rect = m_pPetArea->GetRect();
	CExhibition* pExhibition = NULL;
	bool bResult;

	switch (typeId >> 26)
	{
	case 1:
		if (m_subTab == SUBTAB_SKIN1 || m_subTab == SUBTAB_SKIN2 ||
			m_subTab == SUBTAB_SKIN3)
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f, 0.35f, true);
			pExhibition->SetArea(rect.w, rect.h);
			pExhibition->SetDistanceRange(16.0f, 20.0f);
			pExhibition->SetDistance(18.0f, true, false);
			pExhibition->SetControlFlag(15);
		}
		else
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pExhibition->SetArea(rect.w, rect.h);
			pExhibition->SetDistanceRange(8.0f, 12.0f);
			pExhibition->SetDistance(10.0f, true, false);
		}

		bResult = pExhibition->SetModel(Doc()->GetMyPartTidList(),
			Doc()->GetMyAuxPartTidList(), NULL, 3.0f, 0);
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

	case 16:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistanceRange(4.0f, 8.0f);
		pExhibition->SetDistance(6.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		{
			IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(typeId);
			CMascot::ChangeMascotMsgInTex("\xb8\xde\xbc\xbc\xc1\xf6", pMascot,
				pExhibition->GetModel(), "default");
		}
		break;

	case 6:
		pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
			rect.y + rect.h * 0.5f, 0.0f, true);
		pExhibition->SetArea(rect.w, rect.h);
		pExhibition->SetDistance(6.0f, true, false);
		bResult = pExhibition->SetModel(typeId);
		break;

	case 31:
	{
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
		if (pCard)
		{
			if ((typeId & 0x3c00000) == 0x1000000)
			{
				pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
					rect.y + rect.h * 0.5f - 150.0f, 0.35f, true);
				pExhibition->SetArea(rect.w, rect.h);
				pExhibition->SetDistanceRange(4.0f, 8.0f);
				pExhibition->SetDistance(6.0f, true, false);
				bResult = pExhibition->SetModel(MakeStr("%s", pCard->Image),
					(eExhibitType)12);
			}
			else
			{
				pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
					rect.y + rect.h * 0.5f, 0.0f, true);
				pExhibition->SetArea(rect.w, rect.h);
				pExhibition->SetDistanceRange(4.0f, 8.0f);
				pExhibition->SetDistance(5.0f, true, false);
				bResult = pExhibition->SetModel(MakeStr("%s", pCard->Image),
					(eExhibitType)12);
			}
			pExhibition->SetMotion("default", true, 0.0f);
		}
	}
	break;

	case 9:
	{
		int sub = (typeId >> 21) & 0x1f;
		if (sub == 6 || sub == 5 || sub == 4 || sub == 8)
		{
			pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
				rect.y + rect.h * 0.5f, 0.0f, true);
			pExhibition->SetArea(rect.w, rect.h);
			pExhibition->SetDistance(6.0f, true, false);
			bResult = pExhibition->SetModel(typeId);
		}
	}
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

	default:
		return;
	}

	if (!bResult)
		delete pExhibition;
	else
		m_pExhibition = pExhibition;
}

void CShopMain::ShowPet(unsigned long typeId)
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

	SetPetCtrl(typeId);

	m_anotherHand.Leave();
}

void CShopMain::HidePet(bool bWait)
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

void CShopMain::LoadItemImages()
{
	for (std::map<unsigned int, IFF_STRUCT::sChar>::const_iterator it =
			 ItemManager()->m_CharMap.begin();
		it != ItemManager()->m_CharMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sPart>::iterator it =
			 ItemManager()->m_PartMap.begin();
		it != ItemManager()->m_PartMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end() &&
			((*it).second.c.TypeId & 0x600) == 0)
		{
			const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap(
				"ITEMS_FASHION", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it =
			 ItemManager()->m_CaddieMap.begin();
		it != ItemManager()->m_CaddieMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCadItem>::iterator it =
			 ItemManager()->m_CadItemMap.begin();
		it != ItemManager()->m_CadItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sItem>::iterator it =
			 ItemManager()->m_ItemMap.begin();
		it != ItemManager()->m_ItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sClubSet>::iterator it =
			 ItemManager()->m_ClubSetMap.begin();
		it != ItemManager()->m_ClubSetMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sBall>::iterator it =
			 ItemManager()->m_BallMap.begin();
		it != ItemManager()->m_BallMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it =
			 ItemManager()->m_SetItemMap.begin();
		it != ItemManager()->m_SetItemMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap(
				"ITEMS_FASHION", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSkin>::iterator it =
			 ItemManager()->m_SkinMap.begin();
		it != ItemManager()->m_SkinMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("SKINS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator it =
			 ItemManager()->m_HairStyleMap.begin();
		it != ItemManager()->m_HairStyleMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap =
				g_pFresh->GetManager()->GetBitmap("HAIRS", (*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it =
			 ItemManager()->m_MascotMap.begin();
		it != ItemManager()->m_MascotMap.end(); ++it)
	{
		if (m_itemIcon.find((*it).second.c.TypeId) == m_itemIcon.end())
		{
			const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("MASCOTS",
				(*it).second.c.Icon);
			if (pBitmap)
				m_itemIcon.insert(
					std::map<unsigned long, const Bitmap*>::value_type(
						(*it).second.c.TypeId, pBitmap));
		}
	}
}

const char* dot_files[5][2] = {
	{ "ab_power",    "ab_power_slot"    },
	{ "ab_control",  "ab_control_slot"  },
	{ "ab_accuracy", "ab_accuracy_slot" },
	{ "ab_spin",     "ab_spin_slot"     },
	{ "ab_curve",    "ab_curve_slot"    },
};

void CShopMain::LoadShopImages()
{
	m_pMarkNew = g_pFresh->RegisterBitmap("mark_new");
	m_pMarkHot = g_pFresh->RegisterBitmap("mark_hot");
	m_pMarkLimited = g_pFresh->RegisterBitmap("mark_lim");
	m_pMarkBegin = g_pFresh->RegisterBitmap("mark_begin");

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 2; j++)
			m_pStatDot[i][j] = g_pFresh->RegisterBitmap(dot_files[i][j]);
	}

	m_pSelect[0] = g_pFresh->RegisterBitmap("select_up");
	m_pSelect[1] = g_pFresh->RegisterBitmap("select_mid");
	m_pSelect[2] = g_pFresh->RegisterBitmap("select_low");
	m_pSelect[3] = g_pFresh->RegisterBitmap("select_empty");

	m_pSelectCursor[0] = g_pFresh->RegisterBitmap("select_cursor1");
	m_pSelectCursor[1] = g_pFresh->RegisterBitmap("select_cursor0");

	m_pStatBtn[0][0] = g_pFresh->RegisterBitmap("stat_char_n");
	m_pStatBtn[0][1] = g_pFresh->RegisterBitmap("stat_char_o");
	m_pStatBtn[0][2] = g_pFresh->RegisterBitmap("stat_char_d");
	m_pStatBtn[1][0] = g_pFresh->RegisterBitmap("stat_club_n");
	m_pStatBtn[1][1] = g_pFresh->RegisterBitmap("stat_club_o");
	m_pStatBtn[1][2] = g_pFresh->RegisterBitmap("stat_club_d");
	m_pStatBtn[2][0] = g_pFresh->RegisterBitmap("stat_caddie_n");
	m_pStatBtn[2][1] = g_pFresh->RegisterBitmap("stat_caddie_o");
	m_pStatBtn[2][2] = g_pFresh->RegisterBitmap("stat_caddie_d");
	m_pStatAllBtn[0] = g_pFresh->RegisterBitmap("stat_all_none");

	m_pStatAllBtn[1] = g_pFresh->RegisterBitmap("stat_all_o");
	m_pStatAllBtn[2] = g_pFresh->RegisterBitmap("stat_all_d");

	for (int i = 0; i < 5; i++)
		m_pStatIcon[i] = g_pFresh->RegisterBitmap(MakeStr("sicon%d", i + 1));

	m_pCartSlot = g_pFresh->RegisterBitmap("it_fa02");
	m_pCancelBtn[0] = g_pFresh->RegisterBitmap("cancel_btn_n");
	m_pCancelBtn[1] = g_pFresh->RegisterBitmap("cancel_btn_o");

	m_miniBtn[1].pNormal = g_pFresh->RegisterBitmap("buy_btn_n");
	m_miniBtn[1].pOver = g_pFresh->RegisterBitmap("buy_btn_o");
	m_miniBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_miniBtn[1].pNormal->Width(),
			(float)m_miniBtn[1].pNormal->Height());
	m_miniBtn[1].bEnable = true;

	m_miniBtn[2].pNormal = g_pFresh->RegisterBitmap("gf_btn_n");
	m_miniBtn[2].pOver = g_pFresh->RegisterBitmap("gf_btn_o");
	m_miniBtn[2].rect =
		WRect(103.0f, 66.0f, (float)m_miniBtn[2].pNormal->Width(),
			(float)m_miniBtn[2].pNormal->Height());
	m_miniBtn[2].bEnable = true;

	m_pItemBase[0] = g_pFresh->RegisterBitmap("item_base_n");
	m_pItemBase[1] = g_pFresh->RegisterBitmap("item_base_o");
	m_pItemBase[2] = g_pFresh->RegisterBitmap("item_base_d");
	m_pItemBase[3] = g_pFresh->RegisterBitmap("item_base_empty");
	m_pItemBase[4] = g_pFresh->RegisterBitmap("item_base_award");

	m_pGender[0] = g_pFresh->RegisterBitmap("i_male");
	m_pGender[1] = g_pFresh->RegisterBitmap("i_female");
	m_pMasterSign = g_pFresh->RegisterBitmap("master_sign");
	m_pTeamBase = g_pFresh->RegisterBitmap("team_base");

	m_pSelArrow = g_pFresh->RegisterBitmap("selection_arrow");

	m_pItemSlotBmp = g_pFresh->RegisterBitmap("item_slot_n");

	m_pItemDescBmp[0] = g_pFresh->RegisterBitmap("itemdesc_01");
	m_pItemDescBmp[1] = g_pFresh->RegisterBitmap("itemdesc_02");
}

bool CShopMain::HaveCharacter(unsigned long typeId)
{
	std::map<unsigned int, sCharacterInfo>::iterator end =
		Doc()->m_charMap.end();
	for (std::map<unsigned int, sCharacterInfo>::iterator it =
			 Doc()->m_charMap.begin();
		it != end; ++it)
	{
		if (typeId == (*it).second.tid)
			return true;
	}

	return false;
}

void CShopMain::BuildDisplayStand(subTab_t subTab)
{
	if (m_mainTab == MAINTAB_CHARACTER)
	{
		SetVisibleSideTab(true);
		if (IsSelectedRentalTab())
		{
			BuildDisplayRental(subTab);
			return;
		}
	}
	else
	{
		SetVisibleSideTab(false);
	}

	m_pItemList->ClearItem();

	switch (subTab)
	{
	case SUBTAB_PACKAGE:
		Build_Package(subTab, 0);
		break;

	case SUBTAB_HAIR:
		Build_Hair(subTab, 0);
		break;

	case SUBTAB_PARTS1:
	case SUBTAB_PARTS2:
	case SUBTAB_PARTS3:
	case SUBTAB_PARTS4:
	case SUBTAB_UCC:
		Build_Parts(subTab, 0);
		break;

	case SUBTAB_CLUBSET:
		Build_Clubset(subTab, 0);
		break;

	case SUBTAB_AZTEC:
		Build_Aztec(subTab, 0);
		break;

	case SUBTAB_EQUIP:
		Build_Equip(subTab, 0);
		break;

	case SUBTAB_NOEQUIP:
		Build_NoEquip(subTab, 0);
		break;

	case SUBTAB_CARD:
		Build_Card(subTab, 0);
		break;

	case SUBTAB_INTERIOR1:
	case SUBTAB_INTERIOR2:
	case SUBTAB_INTERIOR3:
		Build_Interior(subTab, 0);
		break;

	case SUBTAB_FURNITURE:
		Build_Furniture(subTab, 0);
		break;

	case SUBTAB_SKIN1:
	case SUBTAB_SKIN2:
	case SUBTAB_SKIN3:
	case SUBTAB_SKIN4:
	case SUBTAB_SKIN5:
		Build_Decorate(subTab, 0);
		break;

	case SUBTAB_CUTIN:
		Build_Cutin(subTab, 0);
		break;

	case SUBTAB_CADDIE:
		Build_Caddie(subTab, 0);
		break;

	case SUBTAB_MASCOT:
		Build_Mascot(subTab, 0);
		break;
	}

	for (int i = m_pItemList->GetCurrentItemSize(true); i < 8; i++)
		m_pItemList->AddItem(NULL);
}

void CShopMain::Net_StartPeriodItem(unsigned long typeId)
{
	WSendPacket send(32);
	send.Encode1(7);
	send.Encode4(typeId);
	send.Send(TO_GAME);
}

void CShopMain::SetLevelBar()
{
	for (int i = 0; i < 5; i++)
	{
		if (m_pStatBar[i])
		{
			m_pStatBar[i]->SetDestPos(0, false);
			m_pStatBar[i]->SetExpand(0);
		}
	}

	switch (m_subTab)
	{
	case SUBTAB_PACKAGE:
	case SUBTAB_PARTS1:
	case SUBTAB_PARTS2:
	case SUBTAB_PARTS3:
	case SUBTAB_PARTS4:
	case SUBTAB_HAIR:
	case SUBTAB_UCC:
		SetLevelBar_Char();
		break;

	case SUBTAB_CLUBSET:
		SetLevelBar_Clubset();
		break;

	case SUBTAB_CADDIE:
	{
		IFF_STRUCT::sCaddie* pCaddie =
			ItemManager()->FindCaddie(m_caddieTypeId);
		if (pCaddie)
		{
			for (int i = 0; i < 5; i++)
			{
				unsigned char level = (unsigned char)pCaddie->Attr[i];

				if (m_pStatBar[i])
					m_pStatBar[i]->SetDestPos(level, false);

				if (m_pStatEdit[i])
					m_pStatEdit[i]->SetCaption11(
						MakeStatusString2(level, 0, 0));
			}
		}

		m_statSubject = 2;
	}
	break;
	}
}

void CShopMain::OpenInformation(unsigned long typeId)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (pWnd)
		return;
	FrForm* pForm = DYNAMIC_CAST(FrForm, pWnd);

	pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
		"information_setitem");
	if (pWnd)
		return;
	FrSetItemDlg* pSetDlg = DYNAMIC_CAST(FrSetItemDlg, pWnd);

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
	if (pItem == NULL)
		return;

	if ((pItem->TypeId & 0xfc000000) == 0x24000000)
	{
		pSetDlg = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(), this,
			"information_setitem");
		if (!pSetDlg)
			return;

		pSetDlg->SetSetItem(pItem->TypeId);
		pSetDlg->Open(NULL, FrESCAPE | FrENTER);
	}
	else
	{
		pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information");
		if (!pForm)
			return;

		FrArea* pPortrait =
			DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
		if (pPortrait)
			pPortrait->SetBgImg(pItem->Icon);

		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
		if (pName && pLevel)
		{
			const char* stock = "";
			if (pItem->TypeId != 0x1c000080)
			{
				switch (pItem->InStock)
				{
				case 2:
					stock = "(\xbc\xb1\xb9\xb0\xc0\xfc\xbf\xeb)";
					break;
				case 3:
					stock = "(\xb1\xb8\xc0\xd4\xc0\xfc\xbf\xeb)";
					break;
				case 4:
					stock = "(\xc0\xfc\xbd\xc3\xbf\xeb)";
					break;
				}
			}

			pName->AddText(
				MakeStr(
					"\xc0\xcc\xb8\xa7: %s\\c0xffff0000\\c%s\\c0xff000000\\c",
					pItem->Name, stock),
				false, true);

			_SYSTEMTIME* periodTimes[2] = { 0, 0 };
			if (ItemManager()->HasSalePeriod(pItem->TypeId, &periodTimes[0],
					&periodTimes[1]) &&
				periodTimes[0]->wYear && periodTimes[1]->wYear)
			{
				if (typeId)
				{
					pName->AddText(
						MakeStr(
							"\\c0xffff0000\\c\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf\\c0xff000000\\c",
							periodTimes[0]->wMonth, periodTimes[0]->wDay,
							periodTimes[1]->wMonth, periodTimes[1]->wDay),
						false, true);
				}
				else
				{
					pName->AddText(
						MakeStr(
							"\\c0xffff0000\\c\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf\\c0xff000000\\c",
							periodTimes[0]->wMonth, periodTimes[0]->wDay,
							periodTimes[1]->wMonth, periodTimes[1]->wDay),
						false, true);
				}
			}
			else
			{
				pName->AddText("\n", false, true);
			}

			if (pItem->IsUnderLvl)
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr("\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf",
						pItem->Name),
					false, true);
			}
			else
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr("\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3",
						pItem->Name),
					false, true);
			}
		}

		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			char szDesc[512];

			FrEdit* pAbility =
				DYNAMIC_CAST(FrEdit, pForm->FindChildByName("avility"));
			if (pAbility)
			{
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);
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
			}
		}

		IFF_STRUCT::sDesc* pDesc;
		if (pLevel)
			pDesc = ItemManager()->FindDesc(pItem->TypeId);
		else
			pDesc = ItemManager()->FindDesc(pItem->TypeId);
		if (pDesc)
			pForm->SetMessage(pDesc->Desc, false);

		pForm->Open(NULL, FrESCAPE | FrENTER);
	}
}

void CShopMain::OpenCaddieInfo(const sCaddieInfo& info)
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
			pDlg->Open((FRESH_PFN_RESULT)&CShopMain::OnReemployeeResult, 3);
		}
	}
}

void CShopMain::ResetItemList(bool bResetSel)
{
	if (m_pItemList)
	{
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pItemList->m_itemList.begin();
			it != m_pItemList->m_itemList.end(); ++it)
		{
			if ((*it)->selected)
			{
				if (bResetSel)
				{
					IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;
					if ((pItem->TypeId & 0xfc000000) == 0x4000000)
						continue;
				}

				m_pItemList->UnselectItem(*it);
			}
		}
	}
}

int CShopMain::GetItemNumInMyPocket(unsigned long typeId, bool bRental)
{
	int count = 0;

	if (typeId == 0 || (typeId & 0xfc000000) == 0x4000000)
	{
		for (std::map<unsigned int, sCharacterInfo>::const_iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).second.tid == typeId) &&
				(!bRental || !pItem->IsCash))
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x8000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).tid == typeId) &&
				(!bRental || !pItem->IsCash))
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x18000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);

			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).tid == typeId) &&
				(!bRental || !pItem->IsCash))
			{
				if ((*it).ItemType)
					count = 100000;
				else
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x1c000000)
	{
		for (std::map<unsigned int, sCaddieInfo>::iterator it =
				 Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).second.tid == typeId) &&
				(!bRental || !pItem->IsCash))
				count++;
		}
	}

	if (typeId == 0 || (typeId & 0xfc000000) == 0x14000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).tid == typeId) &&
				(!bRental || !pItem->IsCash))
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
				IFF_ITEM_COMMON* pItem =
					ItemManager()->FindCommonItem((*it).tid);
				if (pItem == NULL)
					continue;

				if ((typeId == 0 || (*it).tid == pSet->ElemIds[i]) &&
					(!bRental || !pItem->IsCash))
					count++;
			}
		}
	}

	if (typeId == 0 || type == 14)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).tid == typeId) &&
				(!bRental || !pItem->IsCash))
				count++;
		}
	}

	if (typeId == 0 || type == 15)
	{
		count = 1;
	}

	if (type == 4)
	{
		for (std::map<unsigned int, sItemInfo>::iterator it =
				 Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (pItem == NULL)
				continue;

			if ((typeId == 0 || (*it).second.tid == typeId) &&
				(!bRental || !pItem->IsCash))
				count++;
		}
	}

	return count;
}

void CShopMain::Checkout()
{
	if (Doc()->IsControlServerService(4))
	{
		this << MsgObject(this, 0x23,
			(int)"\xbb\xf3\xc1\xa1 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	int partNum = m_pPartCartList->m_itemList.size();
	int auxNum = m_pAuxPartCartList->m_itemList.size();
	if (partNum + auxNum == 0)
	{
		HandleMsg(MsgObject(NULL, 0x23,
			(int)"\xb1\xb8\xb8\xc5\xc7\xd2 \xbb\xf3\xc7\xb0\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0",
			0, 0, 0, 0));
		return;
	}

	FrListBox::ITEM_LIST::iterator it;
	for (it = m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if (pItem->InStock == 0 || pItem->InStock == 2 || pItem->InStock == 4)
			goto cannot_buy;
	}

	for (it = m_pAuxPartCartList->m_itemList.begin();
		it != m_pAuxPartCartList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if (pItem->InStock == 0 || pItem->InStock == 2 || pItem->InStock == 4)
		{
cannot_buy:
			HandleMsg(MsgObject(NULL, 0x23,
				(int)"\xb1\xb8\xb8\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0));
			return;
		}
	}

	FrCheckoutDlg::CartItem item;
	for (it = m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		item.pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((item.pItem->TypeId & 0xfc000000) == 0x48000000)
		{
			for (std::list<sFurniture_List>::iterator furn =
					 Doc()->m_furnitureList.begin();
				furn != Doc()->m_furnitureList.end(); ++furn)
			{
				IFF_STRUCT::sFurniture* pFurniture =
					ItemManager()->FindFurniture((*furn).typeId);
				if (!pFurniture)
					break;

				if (!pFurniture->IsOwn && (*furn).typeId == item.pItem->TypeId)
				{
					HandleMsg(MsgObject(NULL, 0x23,
						(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb9\xb0\xb0\xc7\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0));
					return;
				}
			}
		}
	}

	m_pCheckoutDlg =
		CreateForm<FrCheckoutDlg>(g_pFresh->GetManager(), this, "checkout");

	for (it = m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		item.pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((item.pItem->TypeId & 0xfc000000) == 0x24000000)
		{
			IFF_STRUCT::sSetItem* pSet =
				ItemManager()->FindSetItem(item.pItem->TypeId);
			if (pSet)
			{
				item.ownedNum = 0;
				for (int i = 0; i < pSet->nElems; ++i)
				{
					if (pSet->ElemIds[i] && (pSet->ElemIds[i] >> 26) != 1 &&
						(pSet->ElemIds[i] >> 26) != 6)
					{
						item.ownedNum +=
							GetItemNumInMyPocket(pSet->ElemIds[i], false);
					}
				}
			}
		}
		else
			item.ownedNum = GetItemNumInMyPocket(item.pItem->TypeId, false);

		item.period = 5;
		m_pCheckoutDlg->Cart().push_back(item);
	}

	for (it = m_pAuxPartCartList->m_itemList.begin();
		it != m_pAuxPartCartList->m_itemList.end(); ++it)
	{
		item.pItem = (IFF_ITEM_COMMON*)(*it)->pData;
		item.ownedNum = GetItemNumInMyPocket(item.pItem->TypeId, false);

		item.period = 5;
		m_pCheckoutDlg->Cart().push_back(item);
	}

	m_pCheckoutDlg->Checkout(Doc()->m_myInfo.stat.i64Pang, Doc()->m_cookie,
		IsSelectedRentalTab());
	m_pCheckoutDlg->Open((FRESH_PFN_RESULT)&CShopMain::OnBuyDlgResult, 0x81);
}

void CShopMain::SendGift(const IFF_ITEM_COMMON& item)
{
	if (Doc()->IsControlServerService(0xc))
	{
		this << MsgObject(this, 0x23,
			(int)"\xbc\xb1\xb9\xb0\xc7\xd4 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (bool((Doc()->m_myInfo.info.dwIdentity >> 8) & 1))
	{
		this << MsgObject(NULL, 0x23,
			(int)"\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb8\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe8\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (!m_pPartCartList)
		return;

	if (m_pPartCartList->m_itemList.empty() &&
		m_pAuxPartCartList->m_itemList.empty())
	{
		this << MsgObject(NULL, 0x23,
			(int)"\xb1\xb8\xb8\xc5\xc7\xd2 \xbb\xf3\xc7\xb0\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0",
			0, 0, 0, 0);
		return;
	}

	if (item.InStock == 0 || item.InStock == 3 || item.InStock == 4)
	{
		HandleMsg(MsgObject(NULL, 0x23,
			(int)"\xbc\xb1\xb9\xb0\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}

	m_pGiftDlg =
		CreateForm<FrGiftDlg>(g_pFresh->GetManager(), this, "send_gift");
	m_pGiftDlg->SendGift(item, Doc()->m_myInfo.stat.i64Pang, Doc()->m_cookie);

	m_pGiftDlg->Open((FRESH_PFN_RESULT)&CShopMain::OnGiftDlgResult, 0x81);
}

void CShopMain::Repay(const IFF_ITEM_COMMON& item, unsigned long uid)
{
	FrRepayDlg* pDlg =
		CreateForm<FrRepayDlg>(g_pFresh->GetManager(), this, "repay");
	if (pDlg)
	{
		pDlg->Repay(item, uid, Doc()->m_myInfo.stat.i64Pang, Doc()->m_cookie);
		pDlg->Open((FRESH_PFN_RESULT)&CShopMain::OnSellDlgResult, 0x81);
	}
}

void CShopMain::ClearCartList(bool bRestore, CPartTidList* pParts)
{
	if (!m_pPartCartList)
		return;

	FrListBox::ITEM_LIST::iterator it = m_pPartCartList->m_itemList.begin();
	while (it != m_pPartCartList->m_itemList.end())
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((pItem->TypeId >> 26) == 1)
		{
			if (bRestore)
				++it;
			else
				it = m_pPartCartList->_DelItem(it);

			continue;
		}
		else if ((pItem->TypeId >> 26) == 2 && pParts)
		{
			if (pParts->IsEquipped(pItem->TypeId, 0) &&
				!m_anotherHand.IsWorking(0))
			{
				m_pExhibition->GetPetFrame()->SetPart(pParts, pItem->TypeId, 0);

				RestoreShoppersOldItem(pItem->TypeId, *pParts);
			}
		}
		else if ((pItem->TypeId >> 26) == 15 && pParts)
		{
			if (!m_anotherHand.IsWorking(0))
			{
				bool bFound = false;
				unsigned long tidChar =
					m_pExhibition->GetPetFrame()->GetTidChar();
				CHAR_MAP_IT charIt = Doc()->m_charMap.begin();
				for (; charIt != Doc()->m_charMap.end(); ++charIt)
				{
					if (charIt->second.tid == tidChar)
					{
						m_hairColor = charIt->second.hairClr;

						CPartTidList& myParts = Doc()->GetMyPartTidList();
						m_pExhibition->GetPetFrame()->SetHairClr(&myParts,
							m_hairColor);
						bFound = true;
						break;
					}
				}

				if (!bFound)
					m_hairColor = 0;
			}
		}

		it = m_pPartCartList->_DelItem(it);
	}

	FrListBox::ITEM_LIST::iterator auxIt =
		m_pAuxPartCartList->m_itemList.begin();
	while (auxIt != m_pAuxPartCartList->m_itemList.end())
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*auxIt)->pData;
		if ((pItem->TypeId & 0xfc000000) == 0x08000000 && pParts)
		{
			if (pParts->IsEquipped(pItem->TypeId, 0) &&
				!m_anotherHand.IsWorking(0))
			{
				m_pExhibition->GetPetFrame()->SetPart(pParts, pItem->TypeId, 0);

				RestoreShoppersOldItem(pItem->TypeId, *pParts);
			}
		}

		auxIt = m_pAuxPartCartList->_DelItem(auxIt);
	}

	CheckAuxCartScroll();
}

FrListBox::ITEM_LIST::iterator CShopMain::DeleteItemFromCartList(
	unsigned long typeId)
{
	for (FrListBox::ITEM_LIST::iterator it =
			 m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		FrListItem* pListItem = *it;
		if (((IFF_ITEM_COMMON*)pListItem->pData)->TypeId == typeId)
			return m_pPartCartList->_DelItem(it);
	}

	return m_pPartCartList->m_itemList.end();
}

FrListBox::ITEM_LIST::iterator CShopMain::DeleteItemFromAuxCartList(
	unsigned long typeId)
{
	for (FrListBox::ITEM_LIST::iterator it =
			 m_pAuxPartCartList->m_itemList.begin();
		it != m_pAuxPartCartList->m_itemList.end(); ++it)
	{
		FrListItem* pListItem = *it;
		if (((IFF_ITEM_COMMON*)pListItem->pData)->TypeId == typeId)
		{
			FrListBox::ITEM_LIST::iterator next =
				m_pAuxPartCartList->_DelItem(it);
			CheckAuxCartScroll();
			return next;
		}
	}

	return m_pAuxPartCartList->m_itemList.end();
}

bool CShopMain::SelectItemList(unsigned long typeId)
{
	if (!m_pItemList)
		return false;

	FrListItem* pListItem = NULL;
	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;
		if (!pItem)
			continue;

		if (pItem->TypeId == typeId)
		{
			pListItem = *it;
			break;
		}
	}

	if (!pListItem)
		return false;

	SelectItemList(pListItem);
	return true;
}

bool CShopMain::SelectItemList(FrListItem* pItem)
{
	unsigned long* pTypeId =
		(unsigned long*)&((IFF_ITEM_COMMON*)pItem->pData)->TypeId;
	IFF_ITEM_COMMON* pData = (IFF_ITEM_COMMON*)pItem->pData;

	switch (*pTypeId >> 26)
	{
	case 1:
	{
		Doc()->BuildMyPartTidListDefault(*pTypeId);
		CPartTidList& parts = Doc()->GetMyPartTidList();

		parts.m_hairColor = m_hairColor;

		m_charTypeId = *pTypeId;
		m_pCharParts = parts.m_tid;

		SafeLoadPet(&m_charTypeId, true);

		IFF_ITEM_COMMON* pHair = NULL;
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pPartCartList->m_itemList.begin();
			it != m_pPartCartList->m_itemList.end(); ++it)
		{
			pHair = (IFF_ITEM_COMMON*)(*it)->pData;
			if ((pHair->TypeId & 0xfc000000) == 0x3c000000)
				break;

			pHair = NULL;
		}

		m_pAuxPartCartList->ClearItem();
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);
		if (pHair)
			m_pPartCartList->AddItem(pHair);

		m_pStatBox->SetVisible(true);
		SetLevelBar();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 2:
	{
		CPartTidList& parts = Doc()->GetMyPartTidList();

		unsigned long overlapped = parts.GetOverlapped(pData->TypeId);
		if (!parts.CanSetPart(pData->TypeId))
		{
			m_pItemList->UnselectItem(pItem);
			if (((IFF_STRUCT::sPart*)pData)->EquippableWith[0])
			{
				char buf[256];
				sprintf(buf,
					"\xba\xbb \xbb\xf3\xc7\xb0\xc0\xba \xc7\xf6\xc0\xe7 \xc0\xc7\xbb\xf3\xbf\xa1\xb4\xc2 \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				this << MsgObject(NULL, 0x23, (int)buf, 0, 0, 0, 0);
			}
			else
			{
				this << MsgObject(NULL, 0x23,
					(int)"\\c0xffff0000\\c\xbb\xf3\xc7\xb0\xc0\xbb \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
			}
			return false;
		}

		TakeOff_PartNCartList(parts, overlapped);

		TakeOff_AuxPartNCartList(parts, overlapped);

		unsigned long oldTypeId = 0;
		sDisplayUccClothes* pUcc = (sDisplayUccClothes*)pItem->pData;
		if (UccManager()->IsDisplayUccClothes(pData))
		{
			unsigned long typeId = *pTypeId;
			for (std::list<oldItem_t>::iterator old = m_oldItemList.begin();
				old != m_oldItemList.end();)
			{
				if ((((*old).typeId >> 13) & 0x1f) == ((typeId >> 13) & 0x1f))
				{
					UnselectItemFromItemList((*old).typeId);
					old = m_oldItemList.erase(old);
				}
				else
					++old;
			}

			for (FrListBox::ITEM_LIST::iterator it =
					 m_pItemList->m_itemList.begin();
				it != m_pItemList->m_itemList.end(); ++it)
			{
				if (*it == pItem)
					continue;

				IFF_ITEM_COMMON* pOther = (IFF_ITEM_COMMON*)(*it)->pData;
				if (!pOther)
					continue;

				if ((pOther->TypeId & 0x3e000) == (typeId & 0x3e000))
					m_pItemList->UnselectItem(*it);
			}

			oldTypeId = pUcc->itemId;
		}

		PutOnNBackupShoppersOldItem(pData->TypeId, parts, oldTypeId);

		if (!UccManager()->IsDisplayUccClothes((IFF_ITEM_COMMON*)pUcc))
		{
			unsigned char category = ((IFF_STRUCT::sPart*)pData)->Category;
			if (category == 0 || category == 1 || category == 4 ||
				category == 7 || category == 8 || category == 9)
			{
				m_pPartCartList->AddItem(pData);
			}
			else
			{
				m_pAuxPartCartList->AddItem(pData);
				CheckAuxCartScroll();
			}
		}

		m_pCharParts = parts.m_tid;
		ShowMainModel(parts);
		if (m_hairColor != parts.m_hairColor)
			m_pExhibition->GetPetFrame()->SetHairClr(&parts, m_hairColor);
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 9:
	{
		unsigned long kind = (*pTypeId >> 21) & 0x1f;
		if (kind == 6 || kind == 5 || kind == 4 || kind == 8)
		{
			m_pPartCartList->ClearItem();
			m_pPartCartList->AddItem(pData);

			SafeLoadPet(pTypeId, false);
			TurnOffItemStatus();
			g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
			ShowItemDesc(*pTypeId);
		}
		else
		{
			m_pPartCartList->ClearItem();

			IFF_STRUCT::sSetItem* pSet = (IFF_STRUCT::sSetItem*)pItem->pData;
			CPartTidList& parts = Doc()->GetMyPartTidList();

			if ((pSet->c.TypeId & 0x3e00000) == 0x200000 ||
				(pSet->ElemIds[0] & 0xfc000000) == 0x04000000)
			{
				IFF_STRUCT::sChar* pChar;
				if ((pSet->ElemIds[0] & 0xfc000000) == 0x04000000)
					pChar = ItemManager()->FindChar(pSet->ElemIds[0]);
				else
					pChar = ItemManager()->FindChar(
						((pSet->c.TypeId >> 13) & 0xff) | 0x04000000);

				if (pChar)
				{
					for (int i = 0; i < pChar->nParts + pChar->nAcsries; ++i)
					{
						if (parts.m_tid[i] && !(parts.m_tid[i] & 0x600))
						{
							m_pExhibition->GetPetFrame()->SetPart(&parts,
								parts.m_tid[i], 0);
						}
					}
				}
			}

			for (int i = 0; i < pSet->nElems; ++i)
			{
				if ((pSet->ElemIds[i] & 0xfc000000) == 0x08000000)
				{
					PutOnNBackupShoppersOldItem(pSet->ElemIds[i], parts, 0);
				}
			}

			m_pCharParts = parts.m_tid;
			m_pExhibition->GetPetFrame()->SetHairClr(&parts, m_hairColor);

			IFF_ITEM_COMMON* pHair = NULL;
			for (FrListBox::ITEM_LIST::iterator it =
					 m_pPartCartList->m_itemList.begin();
				it != m_pPartCartList->m_itemList.end(); ++it)
			{
				pHair = (IFF_ITEM_COMMON*)(*it)->pData;
				if ((pHair->TypeId & 0xfc000000) == 0x3c000000)
					break;

				pHair = NULL;
			}

			ShowMainModel(parts);

			m_pAuxPartCartList->ClearItem();
			m_pPartCartList->ClearItem();
			m_pPartCartList->AddItem(pSet);
			if (pHair)
				m_pPartCartList->AddItem(pHair);

			m_pStatBox->SetVisible(true);
			SetLevelBar();
			g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		break;
	}

	case 6:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		SafeLoadPet(pTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;

	case 15:
	{
		CPartTidList& parts = Doc()->GetMyPartTidList();
		m_pExhibition->GetPetFrame()->SetHairClr(&parts,
			((IFF_STRUCT::sHairStyle*)pData)->cHairID);

		FrListBox::ITEM_LIST::iterator it = m_pPartCartList->m_itemList.begin();
		while (it != m_pPartCartList->m_itemList.end())
		{
			IFF_ITEM_COMMON* pHair = (IFF_ITEM_COMMON*)(*it)->pData;

			if ((pHair->TypeId & 0xfc000000) == 0x3c000000)
			{
				UnselectItemFromItemList(pHair->TypeId);
				it = DeleteItemFromCartList(pHair->TypeId);
			}
			else
				++it;
		}

		m_pPartCartList->AddItem(pData);

		m_hairColor = ((IFF_STRUCT::sHairStyle*)pData)->cHairID;

		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_hair", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 4:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		m_clubSetTypeId = *pTypeId;
		SafeLoadPet(&m_clubSetTypeId, false);
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_club", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(m_clubSetTypeId);
		break;

	case 5:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		m_aztecTypeId = *pTypeId;
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("item_select_aztec", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(m_aztecTypeId);
		break;

	case 7:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		m_caddieTypeId = *pTypeId;
		SafeLoadPet(&m_caddieTypeId, false);
		TurnOnItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;

	case 8:
		if ((*pTypeId & 0x1fe000) != 0x6000)
		{
			m_pPartCartList->ClearItem();
			m_pPartCartList->AddItem(pData);
			SafeLoadPet(pTypeId, false);
			TurnOnItemStatus();
		}
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;

	case 16:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		m_mascotTypeId = *pTypeId;
		SafeLoadPet(&m_mascotTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;

	case 14:
	{
		eEquipSkin index;
		if (S5::UTIL::GetEquipSkinIndex(index, *pTypeId))
		{
			if (m_skinTypeId[index])
				DeleteItemFromCartList(m_skinTypeId[index]);

			m_skinTypeId[index] = *pTypeId;
			IFF_STRUCT::sSkin* pSkin = (IFF_STRUCT::sSkin*)pItem->pData;
			if (!(*pTypeId & 0x3c00000))
			{
				m_skinScrollDir[0] = pSkin->HScroll;
				m_skinScrollDir[1] = pSkin->VScroll;
				m_skinScroll[0] = 0;
				m_skinScroll[1] = 0;
			}
		}
		else
			m_pPartCartList->ClearItem();

		m_pPartCartList->AddItem(pData);
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 18:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		SafeLoadPet(pTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;

	case 31:
		m_pPartCartList->ClearItem();
		m_pPartCartList->AddItem(pData);

		SafeLoadPet(pTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(*pTypeId);
		break;
	}

	pItem->selected = true;

	return true;
}

void CShopMain::UnselectItemList(unsigned long typeId)
{
	CPartTidList& parts = Doc()->GetMyPartTidList();

	switch (typeId >> 26)
	{
	case 2:
	{
		DeleteItemFromCartList(typeId);
		DeleteItemFromAuxCartList(typeId);
		UnselectItemFromItemList(typeId);

		unsigned long oldTids[24];
		memcpy(oldTids, parts.m_tid, sizeof(oldTids));

		if (parts.IsEquipped(typeId, 0))
		{
			m_pExhibition->GetPetFrame()->SetPart(&parts, typeId, 0);
			RestoreShoppersOldItem(typeId, parts);
			m_pCharParts = parts.m_tid;
		}

		for (int i = 0; i < 24; ++i)
		{
			if (oldTids[i] && typeId != oldTids[i] &&
				parts.m_tid[i] != oldTids[i] && oldTids[i] != 2 &&
				oldTids[i] != 3)
			{
				UnselectItemFromItemList(oldTids[i]);
				DeleteItemFromCartList(oldTids[i]);
				DeleteItemFromAuxCartList(oldTids[i]);
			}
		}

		parts.m_hairColor = m_hairColor;
		ShowMainModel(parts);
		break;
	}

	case 7:
	case 31:
		break;

	case 9:
	{
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		IFF_STRUCT::sSetItem* pSet = ItemManager()->FindSetItem(typeId);

		for (int i = 0; i < pSet->nElems; ++i)
		{
			unsigned long tid = pSet->ElemIds[i];
			if (parts.IsEquipped(tid, 0))
			{
				m_pExhibition->GetPetFrame()->SetPart(&parts, tid, 0);
				RestoreShoppersOldItem(tid, parts);
				m_pCharParts = parts.m_tid;
			}
		}

		for (FrListBox::ITEM_LIST::iterator it =
				 m_pItemList->m_itemList.begin();
			it != m_pItemList->m_itemList.end(); ++it)
		{
			IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

			if (!pItem)
				continue;

			if ((pItem->TypeId & 0xfc000000) == 0x04000000)
			{
				SelectItemList(*it);
				m_pItemList->SelectItem(*it, true);
				break;
			}
		}

		parts.m_hairColor = m_hairColor;
		ShowMainModel(parts);
		break;
	}

	case 15:
	{
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		unsigned char hairColor = 0;
		CPartTidList& myParts = Doc()->GetMyPartTidList();

		for (CHAR_MAP_IT charIt = Doc()->m_charMap.begin();
			charIt != Doc()->m_charMap.end(); ++charIt)
		{
			if (charIt->second.tid ==
				m_pExhibition->GetPetFrame()->GetTidChar())
			{
				hairColor = charIt->second.hairClr;
				break;
			}
		}

		m_hairColor = hairColor;

		myParts.m_hairColor = m_hairColor;
		ShowMainModel(myParts);
		break;
	}

	case 14:
	{
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		eEquipSkin index;
		if (S5::UTIL::GetEquipSkinIndex(index, typeId))
		{
			m_skinTypeId[index] = Doc()->m_myInfo.userEquip.tidSkin[index];
			if (!(typeId & 0x3c00000))
			{
				if (!m_skinTypeId[index])
				{
					m_skinScrollDir[0] = 0;
					m_skinScrollDir[1] = 0;
				}
				else
				{
					IFF_STRUCT::sSkin* pSkin =
						ItemManager()->FindSkin(m_skinTypeId[index]);
					m_skinScrollDir[0] = pSkin->HScroll;
					m_skinScrollDir[1] = pSkin->VScroll;
				}

				m_skinScroll[0] = 0;
				m_skinScroll[1] = 0;
			}
		}
		break;
	}

	default:
		UnselectItemFromItemList(typeId);
		m_pStatBox->SetVisible(false);
		SetLevelBar();
		break;
	}
}

bool CShopMain::UnselectItemFromItemList(unsigned long typeId)
{
	bool bResult = false;
	if (!m_pItemList)
		return bResult;

	for (FrListBox::ITEM_LIST::iterator it = m_pItemList->m_itemList.begin();
		it != m_pItemList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;
		if (!pItem)
			continue;

		if (pItem->TypeId == typeId)
		{
			m_pItemList->UnselectItem(*it);
			bResult = true;
		}
	}

	return bResult;
}

void CShopMain::TakeOff_PartNCartList(CPartTidList& parts, unsigned long typeId)
{
	FrListBox::ITEM_LIST::iterator it = m_pPartCartList->m_itemList.begin();
	while (it != m_pPartCartList->m_itemList.end())
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((pItem->TypeId >> 26) == 2)
		{
			unsigned long overlapped = parts.GetOverlapped(pItem->TypeId);
			if (typeId & overlapped)
			{
				UnselectItemFromItemList(pItem->TypeId);
				it = DeleteItemFromCartList(pItem->TypeId);

				if (parts.IsEquipped(pItem->TypeId, 0))
				{
					m_pExhibition->GetPetFrame()->SetPart(&parts, pItem->TypeId,
						0);
				}

				RestoreShoppersOldItem(pItem->TypeId, parts);
				UnselectItemList(pItem->TypeId);
			}
			else
				++it;
		}
		else if ((pItem->TypeId >> 26) == 9)
		{
			IFF_STRUCT::sSetItem* pSet = (IFF_STRUCT::sSetItem*)pItem;
			for (int i = 0; i < pSet->nElems; ++i)
			{
				unsigned long tid = pSet->ElemIds[i];

				if (parts.IsEquipped(tid, 0))
				{
					m_pExhibition->GetPetFrame()->SetPart(&parts, tid, 0);
				}

				RestoreShoppersOldItem(tid, parts);
			}

			UnselectItemFromItemList(pItem->TypeId);
			it = DeleteItemFromCartList(pItem->TypeId);
		}
		else
			++it;
	}
}

void CShopMain::TakeOff_AuxPartNCartList(CPartTidList& parts,
	unsigned long typeId)
{
	FrListBox::ITEM_LIST::iterator it = m_pAuxPartCartList->m_itemList.begin();
	while (it != m_pAuxPartCartList->m_itemList.end())
	{
		IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((pItem->TypeId & 0xfc000000) == 0x08000000)
		{
			unsigned long overlapped = parts.GetOverlapped(pItem->TypeId);
			if (overlapped & typeId)
			{
				UnselectItemFromItemList(pItem->TypeId);
				it = DeleteItemFromAuxCartList(pItem->TypeId);

				if (parts.IsEquipped(pItem->TypeId, 0))
				{
					m_pExhibition->GetPetFrame()->SetPart(&parts, pItem->TypeId,
						0);
				}

				RestoreShoppersOldItem(pItem->TypeId, parts);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

void CShopMain::SetLevelBar_Char()
{
	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(m_charTypeId);
	if (pChar)
	{
		for (int i = 0; i < 5; ++i)
		{
			unsigned char item = ItemManager()->GetItemLevel((eLvlType)i,
				m_pCharParts, m_pAuxParts);
			unsigned char limit = item +
				ItemManager()->GetCharCapacity((eLvlType)i, m_charTypeId,
					m_pCharParts, m_pAuxParts, Doc()->m_myInfo.stat.Level);
			unsigned char value = item +
				ItemManager()->GetCharLevel((eLvlType)i, m_charTypeId,
					m_pCharStat);

			if (m_pStatBar[i])
			{
				m_pStatBar[i]->SetDestPos(value, false);
				m_pStatBar[i]->SetExpand(limit - value);
			}

			if (m_pStatEdit[i])
			{
				m_pStatEdit[i]->SetCaption11(
					MakeStatusString2(value, limit, 0));
			}
		}
	}

	m_statSubject = 0;
}

void CShopMain::SetLevelBar_Clubset()
{
	IFF_STRUCT::sClubSet* pClub = ItemManager()->FindClubSet(m_clubSetTypeId);
	if (pClub)
	{
		for (int i = 0; i < 5; ++i)
		{
			unsigned char value = (unsigned char)pClub->Attr[i];
			unsigned char limit = (unsigned char)pClub->Slot[i];

			if (m_pStatBar[i])
			{
				m_pStatBar[i]->SetDestPos(value, false);
				m_pStatBar[i]->SetExpand(limit - value);
			}

			if (m_pStatEdit[i])
			{
				m_pStatEdit[i]->SetCaption11(
					MakeStatusString2(value, limit, 0));
			}
		}
	}

	m_statSubject = 1;
}

void CShopMain::Build_Package(subTab_t subTab, int category)
{
	FrListItem* pItem;

	IFF_STRUCT::sChar& ch = Doc()->m_itemManager.m_CharMap[m_charTypeId];
	if (ch.c.InStock &&
		ItemManager()->IsInSalePeriod(ch.c.TypeId, Doc()->GetServerTime()))
	{
		pItem = m_pItemList->AddItem(&ch);
		AddSellMiniButton(ch.c, pItem);

		if (IsItemInCart(pItem))
			m_pItemList->SelectItem(pItem, true);

		for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
			it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
		{
			IFF_STRUCT::sSetItem& item = (*it).second;
			if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 1 &&
				((item.c.TypeId >> 13) & 0xff) == (m_charTypeId & 0x3ffffff) &&
				ItemManager()->IsInSalePeriod(item.c.TypeId,
					Doc()->GetServerTime()))
			{
				pItem = m_pItemList->AddItem(&item);
				AddSellMiniButton(item.c, pItem);
			}
		}
	}

	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Hair(subTab_t subTab, int category)
{
	for (HairStyleIterator it = Doc()->m_itemManager.m_HairStyleMap.begin();
		it != Doc()->m_itemManager.m_HairStyleMap.end(); ++it)
	{
		IFF_STRUCT::sHairStyle& item = (*it).second;
		if (item.c.InStock && (m_charTypeId & 0x3ffffff) == item.cCharID &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			FrListItem* pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
			if (IsItemInCart(pItem))
				m_pItemList->SelectItem(pItem, true);
		}
	}

	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Parts(subTab_t subTab, int category)
{
	unsigned long flag = GetCategoryFlag(subTab);

	if (subTab == SUBTAB_UCC)
	{
		CUccManager* pUcc = UccManager();
		std::list<sDisplayUccClothes>& clothesList = pUcc->m_displayClothes;

		for (std::list<sDisplayUccClothes>::iterator it = clothesList.begin();
			it != clothesList.end(); ++it)
		{
			sDisplayUccClothes& clothes = *it;
			if (((clothes.partTypeId >> 18) & 0xff) ==
				(m_charTypeId & 0x3ffffff))
				m_pItemList->AddItem(&clothes.part);
		}
	}

	for (PartIterator it = Doc()->m_itemManager.m_PartMap.begin();
		it != Doc()->m_itemManager.m_PartMap.end(); ++it)
	{
		IFF_STRUCT::sPart& item = (*it).second;
		if (S5::UnderCategory(m_charTypeId, item.c.TypeId, flag) &&
			item.c.InStock &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			if (IsAngelWing(item.c.TypeId) &&
				(!IsMannerPlayer() || Doc()->m_myInfo.stat.Level < 21))
				continue;

			FrListItem* pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
			if (pItem && IsItemInCart(pItem))
				m_pItemList->SelectItem(pItem, true);
		}
	}

	m_pItemList->SortItem(PartCompare);
}

void CShopMain::Build_Clubset(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (ClubSetIterator it = Doc()->m_itemManager.m_ClubSetMap.begin();
		it != Doc()->m_itemManager.m_ClubSetMap.end(); ++it)
	{
		IFF_STRUCT::sClubSet& item = (*it).second;
		if (item.c.InStock &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		IFF_STRUCT::sSetItem& item = (*it).second;
		if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 4 &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Aztec(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (BallIterator it = Doc()->m_itemManager.m_BallMap.begin();
		it != Doc()->m_itemManager.m_BallMap.end(); ++it)
	{
		IFF_STRUCT::sBall& item = (*it).second;
		if (item.c.InStock &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		IFF_STRUCT::sSetItem& item = (*it).second;
		if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 5 &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Equip(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (ItemIterator it = Doc()->m_itemManager.m_ItemMap.begin();
		it != Doc()->m_itemManager.m_ItemMap.end(); ++it)
	{
		IFF_STRUCT::sItem& item = (*it).second;
		if (item.c.InStock && !(item.c.TypeId & 0x2000000) &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		IFF_STRUCT::sSetItem& item = (*it).second;
		if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 6 &&
			((item.c.TypeId >> 13) & 0xff) == 0xfe &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_NoEquip(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (ItemIterator it = Doc()->m_itemManager.m_ItemMap.begin();
		it != Doc()->m_itemManager.m_ItemMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pCommon = &(*it).second.c;

		if (pCommon->InStock && (pCommon->TypeId & 0x2000000) &&
			ItemManager()->IsInSalePeriod(pCommon->TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(pCommon);
			if (pCommon->InStock == 1 || pCommon->InStock == 3)
			{
				if (pCommon->InStock == 7)
					pItem->AddNoRBtnIdx(1);
			}

			if (pCommon->InStock == 1 || pCommon->InStock == 2)
			{
				if ((pCommon->IsCash) && pCommon->InStock == 7)
					pItem->AddNoRBtnIdx(2);
			}
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		IFF_STRUCT::sSetItem& item = (*it).second;
		if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 6 &&
			((item.c.TypeId >> 13) & 0xff) == 0xff &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Card(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (CardIterator it = Doc()->m_itemManager.m_CardMap.begin();
		it != Doc()->m_itemManager.m_CardMap.end(); ++it)
	{
		IFF_STRUCT::sCard& item = (*it).second;
		if (item.c.InStock && !(item.c.TypeId & 0x2000000) &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	for (SetItemIterator it = Doc()->m_itemManager.m_SetItemMap.begin();
		it != Doc()->m_itemManager.m_SetItemMap.end(); ++it)
	{
		IFF_STRUCT::sSetItem& item = (*it).second;
		if (item.c.InStock && ((item.c.TypeId >> 21) & 0x1f) == 8 &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Interior(subTab_t subTab, int category)
{
	int flag = GetCategoryFlag(subTab);
	for (FurnitureIterator it = Doc()->m_itemManager.m_FurnitureMap.begin();
		it != Doc()->m_itemManager.m_FurnitureMap.end(); ++it)
	{
		IFF_STRUCT::sFurniture& item = (*it).second;
		if (item.c.InStock && !(item.c.TypeId & 0x2000000) &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			if (((item.c.TypeId >> 11) & 0x7ff) == flag)
			{
				FrListItem* pItem = m_pItemList->AddItem(&item);
				AddSellMiniButton(item.c, pItem);
			}
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Furniture(subTab_t subTab, int category)
{
	for (FurnitureIterator it = Doc()->m_itemManager.m_FurnitureMap.begin();
		it != Doc()->m_itemManager.m_FurnitureMap.end(); ++it)
	{
		IFF_STRUCT::sFurniture& item = (*it).second;
		if (item.c.InStock && !(item.c.TypeId & 0x2000000) &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			int group = (item.c.TypeId >> 11) & 0x7ff;

			if ((group >= 0 && group < 7) || group >= 10)
			{
				FrListItem* pItem = m_pItemList->AddItem(&item);
				if (pItem)
				{
					if (item.c.InStock == 1 || item.c.InStock == 3)
						pItem->AddNoRBtnIdx(1);
					if (item.c.InStock == 1 || item.c.InStock == 2)
						pItem->AddNoRBtnIdx(2);
				}
			}
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Decorate(subTab_t subTab, int category)
{
	int flag = GetCategoryFlag(subTab);

	for (SkinIterator it = Doc()->m_itemManager.m_SkinMap.begin();
		it != Doc()->m_itemManager.m_SkinMap.end(); ++it)
	{
		if (flag == 5 || (((*it).second.c.TypeId >> 22) & 0xf) == flag)
		{
			if ((*it).second.c.InStock)
			{
				FrListItem* pItem = m_pItemList->AddItem(&(*it).second);
				AddSellMiniButton((*it).second.c, pItem);
				if (IsItemInCart(pItem))
				{
					m_pItemList->SelectItem(pItem, true);

					if ((((*it).second.c.TypeId >> 22) & 0xf) == 0 &&
						m_skinTypeId[0])
					{
						m_skinScrollDir[0] = (*it).second.HScroll;
						m_skinScrollDir[1] = (*it).second.VScroll;
						m_skinScroll[0] = 0;
						m_skinScroll[1] = 0;
					}
				}
			}
		}
	}

	m_pItemList->SortItem(SkinCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Cutin(subTab_t subTab, int category)
{
	for (SkinIterator it = Doc()->m_itemManager.m_SkinMap.begin();
		it != Doc()->m_itemManager.m_SkinMap.end(); ++it)
	{
		if (!S5::ISCutinItem((*it).second.c.TypeId))
			continue;

		if (!(*it).second.c.InStock)
			continue;

		IFF_STRUCT::S5::sCutinInformation* pInfo =
			ItemManager()->FindCutinInformation((*it).second.c.TypeId);

		if (pInfo == NULL)
			continue;

		if ((m_charTypeId & 0x3ffffff) == pInfo->CharIndex ||
			pInfo->CharIndex >= 100)
		{
			FrListItem* pItem = m_pItemList->AddItem(&(*it).second);

			AddSellMiniButton((*it).second.c, pItem);
			if (IsItemInCart(pItem))
				m_pItemList->SelectItem(pItem, true);
		}
	}

	m_pItemList->SortItem(SkinCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Caddie(subTab_t subTab, int category)
{
	FrListItem* pItem;

	for (CaddieIterator it = Doc()->m_itemManager.m_CaddieMap.begin();
		it != Doc()->m_itemManager.m_CaddieMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pCommon = &(*it).second.c;
		if (IsVisibleInStock(*pCommon) && pCommon->TypeId == m_caddieTypeId)
		{
			pItem = m_pItemList->AddItem(pCommon);
			AddSellMiniButton(*pCommon, pItem);
		}
	}

	for (CadItemIterator it = Doc()->m_itemManager.m_CadItemMap.begin();
		it != Doc()->m_itemManager.m_CadItemMap.end(); ++it)
	{
		IFF_STRUCT::sCadItem& item = (*it).second;
		if (item.c.InStock &&
			((item.c.TypeId >> 21) & 0x1f) == (m_caddieTypeId & 0x3ffffff) &&
			ItemManager()->IsInSalePeriod(item.c.TypeId,
				Doc()->GetServerTime()))
		{
			pItem = m_pItemList->AddItem(&item);
			AddSellMiniButton(item.c, pItem);
		}
	}

	m_pItemList->SortItem(CaddieCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

void CShopMain::Build_Mascot(subTab_t subTab, int category)
{
	for (MascotIterator it = Doc()->m_itemManager.m_MascotMap.begin();
		it != Doc()->m_itemManager.m_MascotMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pCommon = &(*it).second.c;
		if (IsVisibleInStock(*pCommon))
		{
			FrListItem* pItem = m_pItemList->AddItem(pCommon);
			AddSellMiniButton(*pCommon, pItem);
		}
	}

	m_pItemList->SortItem(ShopEmptyCompare);
	m_pItemList->SortItem(SpecialHitNewCompare);
}

bool CShopMain::IsItemInCart(FrListItem* pItem)
{
	IFF_ITEM_COMMON* pCommon = (IFF_ITEM_COMMON*)pItem->pData;

	for (FrListBox::ITEM_LIST::iterator it =
			 m_pPartCartList->m_itemList.begin();
		it != m_pPartCartList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pCart = (IFF_ITEM_COMMON*)(*it)->pData;
		if (pCart)
		{
			if (pCommon->TypeId == pCart->TypeId)
				return true;
		}
	}

	for (FrListBox::ITEM_LIST::iterator it =
			 m_pAuxPartCartList->m_itemList.begin();
		it != m_pAuxPartCartList->m_itemList.end(); ++it)
	{
		IFF_ITEM_COMMON* pCart = (IFF_ITEM_COMMON*)(*it)->pData;
		if (pCart)
		{
			if (pCommon->TypeId == pCart->TypeId)
				return true;
		}
	}

	return false;
}

void CShopMain::PutOnNBackupShoppersOldItem(unsigned long typeId,
	CPartTidList& parts, unsigned long oldTypeId)
{
	CPartTidList oldParts = parts;

	if (m_pExhibition && m_pExhibition->GetPetFrame())
	{
		if (parts.IsEquipped(typeId, 0))
			m_pExhibition->GetPetFrame()->SetPart(&parts, typeId, 0);

		m_pExhibition->ResetCameraRecoverTime();
		unsigned long changed =
			m_pExhibition->GetPetFrame()->SetPart(&parts, typeId, oldTypeId);

		for (int i = 0; i < parts.m_partNum; i++, changed >>= 1)
		{
			if (changed & 1)
			{
				if (!(oldParts.m_tid[i] & 0x600) &&
					(oldParts.m_tid[i] & 0xfc000000) == 0x8000000)
				{
					oldItem_t item;
					item.typeId = typeId;
					item.oldTypeId = oldParts.m_tid[i];
					item.oldUid = oldParts.m_uid[i];
					m_oldItemList.push_back(item);
				}
			}
		}
	}
}

void CShopMain::RestoreShoppersOldItem(unsigned long typeId,
	CPartTidList& parts)
{
	for (std::list<oldItem_t>::iterator it = m_oldItemList.begin();
		it != m_oldItemList.end();)
	{
		if ((*it).typeId == typeId)
		{
			m_pExhibition->GetPetFrame()->SetPart(&parts, (*it).oldTypeId,
				(*it).oldUid);
			it = m_oldItemList.erase(it);
		}
		else
			++it;
	}
}

void CShopMain::ChangeSubTab(subTab_t subTab, bool bForce)
{
	if (!bForce && m_subTab == subTab)
		return;

	bool bReload = false;

	if (((m_subTab <= 5 || m_subTab == 16) &&
			(subTab == 13 || subTab == 14 || subTab == 15)) ||
		(m_subTab == 33 && (subTab == 13 || subTab == 14 || subTab == 15)) ||
		((m_subTab == 13 || m_subTab == 14 || m_subTab == 15 ||
			 m_subTab == 33) &&
			m_mainTab == MAINTAB_CHARACTER) ||
		(m_mainTab == MAINTAB_CHARACTER && m_subTab == 16 && subTab == 0))
		bReload = true;

	m_subTab = subTab;
	m_bSelectItemList = false;
	m_pItemList->SetMultiSelect(
		subTab == SUBTAB_PARTS4 || subTab == SUBTAB_UCC);

	BuildDisplayStand(m_subTab);

	m_pTabSub->Select(m_subTab);

	switch (m_subTab)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
	case 26:
		TurnOnItemStatus();
		if (bReload)
			SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 4:
		TurnOnItemStatus();
		if (bReload)
			SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 6:
		m_pSelectedName->SetVisible(true);
		m_pSelectedName->SetCaption11(
			"\xbc\xb1\xc5\xc3\xc7\xd1 \xc5\xac\xb7\xb4");
		TurnOnItemStatus();
		SafeLoadPet(&m_clubSetTypeId, false);
		break;

	case 7:
		TurnOffItemStatus();
		SafeLoadPet(&m_aztecTypeId, false);
		break;

	case 27:
		if (IsLocalContent(S4_CARD_SYSTEM))
		{
			TurnOffItemStatus();
			HidePet(true);
		}
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (IsLocalContent(S4_REAL_MYROOM_TEST_ENABLE))
			{
				TurnOffItemStatus();
				HidePet(true);
			}
		}
		break;

	case 10:
		TurnOnItemStatus();
		break;

	case 12:
		TurnOffItemStatus();
		break;

	case 13:
		TurnOffItemStatus();
		SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 14:
		TurnOffItemStatus();
		SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 15:
		TurnOffItemStatus();
		SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 16:
		TurnOffItemStatus();
		SafeLoadPet(&m_charTypeId, bReload);
		break;

	case 8:
	case 9:
	case 34:
		TurnOffItemStatus();
		HidePet(true);
		break;
	}

	SetMyItemSlot();
	SetItemDescRect();
	SetPetAreaRect();
	SetAutoScroll();
}

void CShopMain::SetPetAreaRect()
{
	switch (m_subTab)
	{
	case 13:
	case 14:
	case 15:
		m_pPetArea = m_pSkinPet;
		break;

	case 16:
	case 33:
		m_pPetArea = m_pSkinPet2;
		break;

	default:
		m_pPetArea = m_pPet;
		break;
	}
}

void CShopMain::SetMyItemSlot()
{
	switch (m_subTab)
	{
	case SUBTAB_NOEQUIP:
		BuildMyItemSlot(true);
		m_pItemSlotFrame1->SetVisible(true);
		m_pItemSlotFrame2->SetVisible(true);
		m_pItemSlot->SetVisible(true);
		break;

	case SUBTAB_EQUIP:
		BuildMyItemSlot(false);
		m_pItemSlotFrame1->SetVisible(true);
		m_pItemSlotFrame2->SetVisible(true);
		m_pItemSlot->SetVisible(true);
		break;

	default:
		m_pItemSlotFrame1->SetVisible(false);
		m_pItemSlotFrame2->SetVisible(false);
		m_pItemSlot->SetVisible(false);
		break;
	}
}

void CShopMain::SetAutoScroll()
{
	FrListItem* pSelected = m_pItemList->GetSelected();
	int index = 0;

	if (pSelected)
	{
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pItemList->m_itemList.begin();
			it != m_pItemList->m_itemList.end(); ++it, ++index)
		{
			if (*it == pSelected)
				break;
		}
	}

	m_pItemList->ResetTopRow();
	m_pItemList->GetScrollBar()->SetCurTopRow(index / 2);
}

void CShopMain::ChangeMainTab(mainTab_t mainTab, bool bForce)
{
	if (bForce)
		m_pTabMain->Select(m_mainTab);
	else if (m_mainTab == mainTab)
		return;

	m_mainTab = mainTab;
	if (mainTab != MAINTAB_CHARACTER)
	{
		if (!m_pSideTabBuy || !m_pSideTabRental)
			return;

		if (m_pSideTabBuy->GetStatus() != FrButton::PRESSED)
		{
			m_pSideTabBuy->SetStatus(FrButton::PRESSED);
			m_pSideTabRental->SetStatus(FrButton::NORMAL);
			m_miniBtn[2].bEnable = true;
		}
	}

	m_pTabSub->ClearTab();

	const char* img[3] = { NULL };

	switch (m_mainTab)
	{
	case MAINTAB_CHARACTER:
		m_pTabTxt[0]->SetBgImg("mt_character");
		m_pBuyAll->SetVisible(true);
		m_pBuyAll->Enable(false);
		m_pBuyReset->SetVisible(true);
		m_pBuyReset->Enable(false);

		for (int i = 0; i <= 5; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}

		img[0] = MakeStr("stext_%d_n", 27);
		img[1] = MakeStr("stext_%d_n", 27);
		img[2] = MakeStr("stext_%d_n", 27);
		m_pTabSub->AddTab(img, 26, NULL);
		break;

	case MAINTAB_CADDIE:
		m_pTabTxt[0]->SetBgImg("mt_caddie");
		m_pBuyAll->SetVisible(false);
		m_pBuyReset->SetVisible(false);

		for (int i = 10; i < 11; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;

	case MAINTAB_ITEM:
		m_pTabTxt[0]->SetBgImg("mt_item");
		m_pSelectedName->SetVisible(false);
		m_pBuyAll->SetVisible(false);
		m_pBuyReset->SetVisible(false);

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

	case MAINTAB_MASCOT:
		m_pTabTxt[0]->SetBgImg("mt_mascot");
		m_pBuyAll->SetVisible(false);
		m_pBuyReset->SetVisible(false);

		for (int i = 12; i < 13; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;

	case MAINTAB_SKIN:
		m_pTabTxt[0]->SetBgImg("mt_skin");
		m_pBuyAll->SetVisible(true);
		m_pBuyReset->SetVisible(true);

		for (int i = 13; i <= 16; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}

		img[0] = MakeStr("stext_%d_n", 34);
		img[1] = MakeStr("stext_%d_n", 34);
		img[2] = MakeStr("stext_%d_n", 34);
		m_pTabSub->AddTab(img, 33, NULL);

		img[0] = MakeStr("stext_%d_n", 35);
		img[1] = MakeStr("stext_%d_n", 35);
		img[2] = MakeStr("stext_%d_n", 35);
		m_pTabSub->AddTab(img, 34, NULL);
		break;

	case MAINTAB_INTERIOR:
		m_pTabTxt[0]->SetBgImg("mt_interior");
		m_pBuyAll->SetVisible(false);
		m_pBuyReset->SetVisible(false);

		SetSubTabImg(img, 33);
		m_pTabSub->AddTab(img, 32, NULL);
		for (int i = 28; i < 32; i++)
		{
			SetSubTabImg(img, i + 1);
			m_pTabSub->AddTab(img, i, NULL);
		}
		break;
	}

	switch (m_mainTab)
	{
	case MAINTAB_CHARACTER:
	case MAINTAB_CADDIE:
		TurnOnItemSelect();
		break;

	default:
		TurnOffItemSelect();
		break;
	}

	m_pItemDesc->SetVisible(false);
	m_pItemDescBg->SetVisible(false);

	TurnOffItemStatus();
	TurnOffAuxCartScroll();

	ClearCartList(false, NULL);
}

void CShopMain::ChangeStatTab(statTab_t statTab)
{
	m_statTab = statTab;
}

void CShopMain::ChangeCharacter(unsigned long typeId)
{
	std::map<unsigned int, sCharacterInfo>::iterator it;
	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
	{
		if ((*it).second.tid == typeId)
			break;
	}

	m_pBuyAll->Enable(false);
	m_pBuyReset->Enable(false);

	if (m_charTypeId != typeId)
	{
		m_charTypeId = typeId;
		m_pPartCartList->ClearItem();
		m_pAuxPartCartList->ClearItem();
	}

	if (m_subTab == SUBTAB_PACKAGE || m_subTab == SUBTAB_PARTS1 ||
		m_subTab == SUBTAB_PARTS2 || m_subTab == SUBTAB_PARTS3 ||
		m_subTab == SUBTAB_PARTS4 || m_subTab == SUBTAB_HAIR ||
		m_subTab == SUBTAB_UCC)
	{
		bool bWinner = false;
		if (m_subTab == SUBTAB_UCC &&
			typeId == UccManager()->GetWinnerCharacterTypeId() &&
			IS_EXACTKINDOF(CLobbyTask, AfxGetPreservedTask()))
			bWinner = true;

		if (it != Doc()->m_charMap.end())
		{
			Doc()->BuildMyPartTidList((*it).second);
			m_pCharParts = (*it).second.tidParts;
			m_pAuxParts = (*it).second.tidAuxParts;
			m_pCharStat = (*it).second.PCL;
			m_hairColor = (*it).second.hairClr;
			if (m_subTab == SUBTAB_PACKAGE)
			{
				ChangeSubTab(SUBTAB_PARTS1, false);
				m_pTabSub->Select(m_subTab);
			}
		}
		else
		{
			Doc()->BuildMyPartTidListDefault(typeId);
			if (!bWinner)
				ChangeSubTab(SUBTAB_PACKAGE, false);
			m_pTabSub->Select(m_subTab);
			m_hairColor = 0;
		}

		ClearCartList(true, &Doc()->GetMyPartTidList());
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_charTypeId, false);
		TurnOnItemStatus();

		m_bSelectItemList = false;

		for (CharIterator itNext = ItemManager()->m_CharMap.begin();
			itNext != ItemManager()->m_CharMap.end(); ++itNext)
		{
			if ((*itNext).second.c.TypeId == typeId)
			{
				if (++itNext == ItemManager()->m_CharMap.end())
					m_pSelItemNext->Enable(false);
				else
					m_pSelItemNext->Enable(true);
				break;
			}
		}

		for (std::map<unsigned int, IFF_STRUCT::sChar>::reverse_iterator rit =
				 ItemManager()->m_CharMap.rbegin();
			rit != ItemManager()->m_CharMap.rend(); ++rit)
		{
			if ((*rit).second.c.TypeId == typeId)
			{
				if (++rit == ItemManager()->m_CharMap.rend())
					m_pSelItemPrev->Enable(false);
				else
					m_pSelItemPrev->Enable(true);
				break;
			}
		}

		if (typeId)
		{
			const char* name = ItemManager()->GetItemName(typeId);
			if (name)
				m_pSelectedName->SetCaption11(name);
		}
	}
	else if (m_mainTab == MAINTAB_SKIN)
	{
		Doc()->BuildMyPartTidList((*it).second);
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_charTypeId, false);
	}

	SetAutoScroll();
}

void CShopMain::ChangeCaddie(unsigned long typeId)
{
	m_caddieTypeId = typeId;

	if (m_subTab == SUBTAB_CADDIE)
	{
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_caddieTypeId, false);
		TurnOnItemStatus();

		m_bSelectItemList = false;

		for (CaddieIterator it = ItemManager()->m_CaddieMap.begin();
			it != ItemManager()->m_CaddieMap.end(); ++it)
		{
			if ((*it).second.c.TypeId == typeId)
			{
				while (++it != ItemManager()->m_CaddieMap.end())
				{
					if (IsVisibleInStock((*it).second.c))
						break;
				}

				if (it == ItemManager()->m_CaddieMap.end())
					m_pSelItemNext->Enable(false);
				else
					m_pSelItemNext->Enable(true);
				break;
			}
		}

		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::reverse_iterator rit =
				 ItemManager()->m_CaddieMap.rbegin();
			rit != ItemManager()->m_CaddieMap.rend(); ++rit)
		{
			if ((*rit).second.c.TypeId == typeId)
			{
				while (++rit != ItemManager()->m_CaddieMap.rend())
				{
					if (IsVisibleInStock((*rit).second.c))
						break;
				}

				if (rit == ItemManager()->m_CaddieMap.rend())
					m_pSelItemPrev->Enable(false);
				else
					m_pSelItemPrev->Enable(true);
				break;
			}
		}

		if (typeId)
		{
			const char* name = ItemManager()->GetItemName(typeId);
			if (name)
				m_pSelectedName->SetCaption11(name);
		}
	}
}

void CShopMain::ChangeMascot(unsigned long typeId)
{
	m_mascotTypeId = typeId;

	if (m_subTab == SUBTAB_MASCOT)
	{
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_mascotTypeId, false);
		TurnOffItemStatus();
	}
}

void CShopMain::ChangeSkin(unsigned long typeId)
{
	m_charTypeId = typeId;

	SafeLoadPet(&m_charTypeId, false);

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

void CShopMain::ChangeClubset(unsigned long typeId)
{
	m_clubSetTypeId = typeId;

	if (m_subTab == SUBTAB_CLUBSET)
	{
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_clubSetTypeId, false);
		TurnOnItemStatus();
	}
}

void CShopMain::ChangeAztec(unsigned long typeId)
{
	m_aztecTypeId = typeId;

	if (m_subTab == SUBTAB_AZTEC)
	{
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();
	}
}

void CShopMain::TurnOnItemSelect()
{
	m_pSelItem->SetVisible(true);
	m_pSelItemPrev->SetVisible(true);
	m_pSelItemNext->SetVisible(true);
}

void CShopMain::TurnOffItemSelect()
{
	m_pSelItem->SetVisible(false);
	m_pSelItemPrev->SetVisible(false);
	m_pSelItemNext->SetVisible(false);
}

const Bitmap* CShopMain::GetItemIcon(unsigned long typeId)
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

void CShopMain::DrawSelItem(FrGraphicInterface* pGI, unsigned long typeId)
{
	WRect rect = m_pSelItemIcon->GetRect();

	const Bitmap* pIcon = GetItemIcon(typeId);
	if (!pIcon)
		return;

	unsigned long color = 0xffffffff;
	switch (typeId >> 26)
	{
	case 1:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
		{
			if ((*it).second.tid == typeId)
				break;
		}

		if (it == Doc()->m_charMap.end())
			color = 0xff808080;
	}
	break;

	case 7:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it;
		for (it = Doc()->m_caddieMap.begin(); it != Doc()->m_caddieMap.end();
			++it)
		{
			if ((*it).second.tid == typeId)
				break;
		}

		if (it == Doc()->m_caddieMap.end())
			color = 0xff808080;
	}
	break;

	case 5:
	{
		std::list<sItemInfo>::iterator it;
		for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end();
			++it)
		{
			if ((*it).tid == typeId)
				break;
		}

		if (it == Doc()->m_ballList.end())
			color = 0xff808080;
	}
	break;

	case 4:
	{
		std::map<unsigned int, sItemInfo>::iterator it;
		for (it = Doc()->m_clubSetMap.begin(); it != Doc()->m_clubSetMap.end();
			++it)
		{
			if ((*it).second.tid == typeId)
				break;
		}

		if (it == Doc()->m_clubSetMap.end())
			color = 0xff808080;
	}
	break;

	case 16:
	{
		std::map<unsigned int, sMascotInfo>::iterator it;
		for (it = Doc()->m_mascotMap.begin(); it != Doc()->m_mascotMap.end();
			++it)
		{
			if ((*it).second.tid == typeId)
				break;
		}

		if (it == Doc()->m_mascotMap.end())
			color = 0xff808080;
	}
	break;

	case 18:
	{
		std::list<sFurniture_List>::iterator it;
		for (it = Doc()->m_furnitureList.begin();
			it != Doc()->m_furnitureList.end(); ++it)
		{
			if ((*it).typeId == typeId)
				break;
		}

		if (it == Doc()->m_furnitureList.end())
			color = 0xff808080;
	}
	break;
	}

	pGI->DrawTexture(pIcon, rect, color, 0);
}

void CShopMain::DrawCharacterList(FrGraphicInterface* pGI, unsigned long param)
{
	if (m_bSelectItemList && !ItemManager()->m_CharMap.empty())
	{
		int count = 0;
		int index = 0;

		for (CharIterator it = ItemManager()->m_CharMap.begin();
			it != ItemManager()->m_CharMap.end(); ++it)
		{
			if (IsVisibleInStock((*it).second.c))
				++count;
		}

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

		for (CharIterator it = ItemManager()->m_CharMap.begin();
			it != ItemManager()->m_CharMap.end(); ++it)
		{
			if (IsVisibleInStock((*it).second.c))
			{
				unsigned long tid = (*it).second.c.TypeId;
				const Bitmap* pIcon = g_pFresh->GetBitmap((*it).second.c.Icon);
				if (pIcon)
				{
					WRect rect(x, y, 46.0f, 50.0f);
					++index;

					unsigned long color = 0xffffffff;
					std::map<unsigned int, sCharacterInfo>::iterator it2;
					for (it2 = Doc()->m_charMap.begin();
						it2 != Doc()->m_charMap.end(); ++it2)
					{
						if ((*it2).second.tid == tid)
							break;
					}

					if (it2 == Doc()->m_charMap.end())
						color = 0xff808080;

					pGI->DrawTexture(pIcon, rect, color, 0);
					if (index % cols)
					{
						x += 47.0f;
					}
					else
					{
						y += 51.0f;
						x = 18.0f;
					}

					if (m_charTypeId == tid)
						selRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

					const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
					if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
						mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
					{
						overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

						if (g_input->GetButton(LEFT_BUTTON) == 1)
						{
							if (m_charTypeId != tid)
								ChangeCharacter(tid);
						}
					}
				}
			}
		}

		if (selRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);

		if (overRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CShopMain::DrawCaddieList(FrGraphicInterface* pGI, unsigned long param)
{
	if (m_bSelectItemList && !ItemManager()->m_CaddieMap.empty())
	{
		int count = 0;
		int index = 0;

		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it =
				 m_visibleCaddie.begin();
			it != m_visibleCaddie.end(); ++it)
		{
			if (IsVisibleInStock((*it).second.c))
				++count;
		}

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

		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it =
				 m_visibleCaddie.begin();
			it != m_visibleCaddie.end(); ++it)
		{
			if (IsVisibleInStock((*it).second.c))
			{
				unsigned long tid = (*it).second.c.TypeId;
				const Bitmap* pIcon = g_pFresh->GetBitmap((*it).second.c.Icon);
				if (pIcon)
				{
					WRect rect(x, y, 46.0f, 50.0f);
					++index;

					unsigned long color = 0xffffffff;
					std::map<unsigned int, sCaddieInfo>::iterator it2;
					for (it2 = Doc()->m_caddieMap.begin();
						it2 != Doc()->m_caddieMap.end(); ++it2)
					{
						if ((*it2).second.tid == tid)
							break;
					}

					if (it2 == Doc()->m_caddieMap.end())
						color = 0xff808080;

					pGI->DrawTexture(pIcon, rect, color, 0);
					if (index % cols)
					{
						x += 47.0f;
					}
					else
					{
						y += 51.0f;
						x = 18.0f;
					}

					if (m_caddieTypeId == tid)
						selRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

					const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
					if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
						mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
					{
						overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

						if (g_input->GetButton(LEFT_BUTTON) == 1)
							ChangeCaddie(tid);
					}
				}
			}
		}

		if (selRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);

		if (overRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CShopMain::DrawMascotList(FrGraphicInterface* pGI, unsigned long param)
{
	if (m_bSelectItemList && !ItemManager()->m_MascotMap.empty())
	{
		int count = ItemManager()->m_MascotMap.size();
		int index = 0;

		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));
		int rows = count / cols + (count % cols ? 1 : 0);

		float width = (float)(cols - 1) + cols * 46.0f + 10.0f;
		float height = (float)(rows - 1) + rows * 50.0f;
		float x = 18.0f;
		float y = 419.0f - height;
		float top = y - 5.0f;

		pGI->DrawTexture(m_pSelect[0], WRect(13.0f, top, width, height + 10.0f),
			0xffffffff, 0);

		WRect selRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect overRect(0.0f, 0.0f, 0.0f, 0.0f);

		pGI->DrawTexture(m_pSelect[0], WRect(13.0f, top, width, height + 25.0f),
			0xffffffff, 0);

		for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it =
				 ItemManager()->m_MascotMap.begin();
			it != ItemManager()->m_MascotMap.end(); ++it)
		{
			if (IsVisibleInStock((*it).second.c))
			{
				unsigned long tid = (*it).second.c.TypeId;
				const Bitmap* pIcon = g_pFresh->GetBitmap((*it).second.c.Icon);
				if (pIcon)
				{
					WRect rect(x, y, 46.0f, 50.0f);
					++index;

					unsigned long color = 0xffffffff;
					std::map<unsigned int, sMascotInfo>::iterator it2;
					for (it2 = Doc()->m_mascotMap.begin();
						it2 != Doc()->m_mascotMap.end(); ++it2)
					{
						if ((*it2).second.tid == tid)
							break;
					}

					if (it2 == Doc()->m_mascotMap.end())
						color = 0xff808080;

					pGI->DrawTexture(pIcon, rect, color, 0);
					if (index % cols)
					{
						x += 47.0f;
					}
					else
					{
						y += 51.0f;
						x = 18.0f;
					}

					if (m_mascotTypeId == tid)
						selRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

					const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
					if (mouse.x >= rect.x && rect.x + rect.w >= mouse.x &&
						mouse.y >= rect.y && rect.y + rect.h >= mouse.y)
					{
						overRect = WRect(rect.x - 4.0f, rect.y - 4.0f,
							rect.w + 8.0f, rect.h + 8.0f);

						if (g_input->GetButton(LEFT_BUTTON) == 1)
							ChangeMascot(tid);
					}
				}
			}
		}

		if (selRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[0], selRect, 0xffffffff, 0);

		if (overRect.w != 0.0f)
			pGI->DrawTexture(m_pSelectCursor[1], overRect, 0xffffffff, 0);
	}

	if (m_bSelectItemList && g_input->GetButton(LEFT_BUTTON) == 1)
	{
		WRect rect = m_pSelItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_bSelectItemList = false;
	}
}

void CShopMain::DrawSelArrow(FrGraphicInterface* pGI)
{
}

bool CShopMain::IsVisibleInStock(IFF_ITEM_COMMON& item)
{
	if (item.InStock != 0 && item.InStock != 5 && item.InStock != 6 &&
		ItemManager()->IsInSalePeriod(item.TypeId, Doc()->GetServerTime()))
		return true;
	return false;
}

void CShopMain::SafeLoadPet(unsigned long* pTypeId, bool bForce)
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

void CShopMain::ShowMainModel(CPartTidList& parts)
{
	SetLevelBar();

	m_anotherHand.Enter();
	if (m_pExhibition && !m_anotherHand.IsWorking(0))
	{
		m_pExhibition->ResetCameraRecoverTime();
		m_pExhibition->ParseBoundingBoxScript();
		m_pExhibition->GetPetFrame()->Refresh(&parts);
	}
	m_anotherHand.Leave();
}

void CShopMain::LoadCardImages()
{
	for (CardIterator it = ItemManager()->m_CardMap.begin();
		it != ItemManager()->m_CardMap.end(); ++it)
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

	m_pEmptyCharCard = g_resrcmng->GetOverlay("card_character_empty.tga", 0);
	m_pEmptyCaddieCard = g_resrcmng->GetOverlay("card_caddie_empty.tga", 0);
}

void CShopMain::DeleteCardImages()
{
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cardImg.begin();
		it != m_cardImg.end(); ++it)
	{
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	}

	if (g_resrcmng && m_pEmptyCharCard)
	{
		g_resrcmng->Release(m_pEmptyCharCard);
		m_pEmptyCharCard = NULL;
	}
	if (g_resrcmng && m_pEmptyCaddieCard)
	{
		g_resrcmng->Release(m_pEmptyCaddieCard);
		m_pEmptyCaddieCard = NULL;
	}
}

WOverlay* CShopMain::GetCardImg(unsigned long typeId)
{
	WOverlay* pOverlay = NULL;
	std::map<unsigned int, WOverlay*>::iterator it = m_cardImg.find(typeId);
	if (it == m_cardImg.end())
	{
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
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

void CShopMain::DrawCard()
{
	IFF_STRUCT::sCard* pCard = NULL;
	unsigned long cardId = 0;

	if (!m_cardTypeId)
		return;

	if ((m_cardTypeId >> 26) == 0x1f)
	{
		pCard = ItemManager()->FindCard(m_cardTypeId);
		cardId = m_cardTypeId;
	}
	else if ((m_cardTypeId >> 26) == 9 && (m_cardTypeId & 0x1fe000) == 0x1fa000)
	{
		IFF_STRUCT::sSetItem* pSet = ItemManager()->FindSetItem(m_cardTypeId);
		if (!pSet)
			return;

		cardId = pSet->ElemIds[0];
		pCard = ItemManager()->FindCard(cardId);
	}
	else
	{
		return;
	}

	if (pCard && cardId)
	{
		WOverlay* pImg = GetCardImg(cardId);
		if (pImg)
			pImg->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(92.0f, 117.0f, 248.0f, 499.0f), 0, 0xffffffff, 0, 0);
	}
}

void CShopMain::InitalizeUpgradeCaddieInfo()
{
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000001, 0x1c000010));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000002, 0x1c000011));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000003, 0x1c000012));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000004, 0x1c000013));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000005, 0x1c000014));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000006, 0x1c000015));
	m_upgradeCaddie.insert(std::map<unsigned long, unsigned long>::value_type(
		0x1c000007, 0x1c000016));
}

void CShopMain::BuildVisibleCaddieList()
{
	m_visibleCaddie.clear();

	for (CaddieIterator it = ItemManager()->m_CaddieMap.begin();
		it != ItemManager()->m_CaddieMap.end(); ++it)
	{
		IFF_STRUCT::sCaddie& caddie = (*it).second;
		bool bHave = false;

		std::map<unsigned long, unsigned long>::iterator itUp;
		unsigned long from;
		unsigned long to;
		for (itUp = m_upgradeCaddie.begin(); itUp != m_upgradeCaddie.end();
			++itUp)
		{
			from = (*itUp).first;
			to = (*itUp).second;
			if (caddie.c.TypeId == from || caddie.c.TypeId == to)
				break;
		}

		if (itUp != m_upgradeCaddie.end())
		{
			std::map<unsigned int, sCaddieInfo>::iterator itMy =
				Doc()->m_caddieMap.begin();
			if (caddie.c.TypeId == from)
			{
				for (; itMy != Doc()->m_caddieMap.end(); ++itMy)
				{
					if ((*itMy).second.tid == from)
					{
						bHave = true;
						break;
					}
				}
				if (!bHave)
					continue;
			}
			else if (caddie.c.TypeId == to)
			{
				for (; itMy != Doc()->m_caddieMap.end(); ++itMy)
				{
					if ((*itMy).second.tid == from)
						break;
					if ((*itMy).second.tid == to)
					{
						bHave = true;
						break;
					}
				}
				if (itMy != Doc()->m_caddieMap.end() && !bHave)
					continue;
			}
			else
			{
				continue;
			}
		}
		m_visibleCaddie.insert(
			std::map<unsigned int, IFF_STRUCT::sCaddie>::value_type((*it).first,
				caddie));
	}
}

void CShopMain::UpgradeCaddie(unsigned long typeId)
{
	unsigned long upgradeId = 0;

	std::map<unsigned long, unsigned long>::iterator it =
		m_upgradeCaddie.find(typeId);
	if (it != m_upgradeCaddie.end())
	{
		upgradeId = (*it).second;
	}
	else
	{
		HandleMsg(MsgObject(NULL, 0x23,
			(int)"\xc7\xd8\xb4\xe7 \xc4\xb3\xb5\xf0\xb4\xc2 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9 \xc7\xd5\xb4\xcf\xb4\xd9",
			0, 0, 0, 0));
	}

	CaddieIterator itOld;
	CaddieIterator itNew;
	itOld = ItemManager()->m_CaddieMap.find(typeId);
	itNew = ItemManager()->m_CaddieMap.find(upgradeId);

	short oldStat[5];
	short newStat[5];
	int oldPrice;
	int newPrice;

	if (itOld != ItemManager()->m_CaddieMap.end())
	{
		memcpy(oldStat, (*itOld).second.Attr, sizeof(oldStat));
		oldPrice = (*itOld).second.MonthlyFee;
	}

	if (itNew != ItemManager()->m_CaddieMap.end())
	{
		memcpy(newStat, (*itNew).second.Attr, sizeof(newStat));
		newPrice = (*itNew).second.MonthlyFee;
	}

	int stat[5];
	stat[0] = newStat[0] - oldStat[0];
	stat[1] = newStat[1] - oldStat[1];
	stat[2] = newStat[2] - oldStat[2];
	stat[3] = newStat[3] - oldStat[3];
	stat[4] = newStat[4] - oldStat[4];

	m_pUpgradeCaddieDlg = CreateForm<FrUpgradeCaddieDlg>(g_pFresh->GetManager(),
		this, "upgradecaddie", NULL);
	if (m_pUpgradeCaddieDlg)
	{
		m_pUpgradeCaddieDlg->SetUpgradeInfo(typeId, stat, oldPrice, newPrice);
		m_pUpgradeCaddieDlg->Open(NULL, 3);
	}
}
