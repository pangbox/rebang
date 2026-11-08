#include "stdafx.h"
#include "messengerdlg.h"
#include "mathconsts.h"
#include "avatar_main.h"
#include "frcombobox.h"
#include "frcontextmenuctrl.h"
#include "frscrollbar.h"
#include "chatmsg.h"
#include "avatartipdlg.h"
#include "avatarchatlogdlg.h"
#include "avataruserlistdlg.h"
#include "jukeboxdlg.h"
#include "tradedlg.h"
#include "notedlg.h"
#include "reportdlg.h"
#include "addfrienddlg.h"
#include "emoticondlg.h"
#include "clientsetting.h"
#include "gatewayactor.h"
#include "user_info.h"
#include "rankinginfo.h"
#include "quitwindow.h"
#include "mousecursor.h"

extern Fresh* g_pFresh;

BEGIN_FRESH_MSGMAP(ntAvatarChatMain, CTaskMain)
ON_FRESH_VI("AVATARCHAT_MAIN", FRCMD_INIT,
	ntAvatarChatMain::OnAvatarChatMainInit)
ON_FRESH_VV("AVATARCHAT_MAIN", FRCMD_FINISH,
	ntAvatarChatMain::OnAvatarChatMainFinish)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitInit)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_mask_area", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitMaskAreaInit)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_mask", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitMaskInit)
ON_FRESH_VV("AVATARCHAT_MAIN.portrait_mask", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnPortraitMaskBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_tipbtn", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitTipButtonInit)
ON_FRESH_VV("AVATARCHAT_MAIN.portrait_tipbtn", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnPortraitTipButtonBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_jukeboxbtn_area", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitJukeBoxButtonAreaInit)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_jukeboxbtn", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitJukeBoxButtonInit)
ON_FRESH_VV("AVATARCHAT_MAIN.portrait_jukeboxbtn", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnPortraitJukeBoxButtonBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.portrait_chatlogbtn", FRCMD_INIT,
	ntAvatarChatMain::OnPortraitChatLogButtonInit)
ON_FRESH_VV("AVATARCHAT_MAIN.portrait_chatlogbtn", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnPortraitChatLogButtonBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.userlistbtn", FRCMD_INIT,
	ntAvatarChatMain::OnAvatarUserListButtonInit)
ON_FRESH_VV("AVATARCHAT_MAIN.userlistbtn", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnAvatarUserListButtonBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.chatframe", FRCMD_INIT,
	ntAvatarChatMain::OnChatFrameInit)
ON_FRESH_VI("AVATARCHAT_MAIN.chatview", FRCMD_INIT,
	ntAvatarChatMain::OnChatViewInit)
ON_FRESH_VI("AVATARCHAT_MAIN.emoticon", FRCMD_INIT,
	ntAvatarChatMain::OnEmoticonInit)
ON_FRESH_VV("AVATARCHAT_MAIN.emoticon", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnEmoticonBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.language", FRCMD_INIT,
	ntAvatarChatMain::OnLanguageInit)
ON_FRESH_VI("AVATARCHAT_MAIN.report", FRCMD_INIT,
	ntAvatarChatMain::OnReportInit)
ON_FRESH_VV("AVATARCHAT_MAIN.report", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnReportBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.chattarget", FRCMD_INIT,
	ntAvatarChatMain::OnChatTargetInit)
ON_FRESH_VI("AVATARCHAT_MAIN.chattarget", FRCMD_LBUTTONDOWN,
	ntAvatarChatMain::OnChatTargetBtnDown)
ON_FRESH_BI("AVATARCHAT_MAIN.chattarget", FRCMD_ENTERKEY,
	ntAvatarChatMain::OnChatTargetEnterKey)
ON_FRESH_VI("AVATARCHAT_MAIN.chatinput", FRCMD_INIT,
	ntAvatarChatMain::OnChatInputInit)
ON_FRESH_BI("AVATARCHAT_MAIN.chatinput", FRCMD_ENTERKEY,
	ntAvatarChatMain::OnChatInputEnterKey)
ON_FRESH_VI("AVATARCHAT_MAIN.rmenu", FRCMD_INIT,
	ntAvatarChatMain::OnRightMenuInit)
ON_FRESH_VV("AVATARCHAT_MAIN.rmenu", FRCMD_LBUTTONDOWN,
	ntAvatarChatMain::OnRightMenuLBtnDown)
ON_FRESH_VV("AVATARCHAT_MAIN.rmenu", FRCMD_RBUTTONDOWN,
	ntAvatarChatMain::OnRightMenuRBtnDown)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.back", FRCMD_INIT,
	CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.back", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.back", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.front", FRCMD_INIT,
	CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.front", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.front", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.front", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.shop", FRCMD_INIT,
	CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.shop", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnUnderBar_ShopUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.shop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.myroom", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.myroom", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.myroom", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.itemstorage", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.gift", FRCMD_INIT,
	CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.gift", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.gift", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.cookie", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnUnderBar_CookieUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.cookie", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.cookie", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.game", FRCMD_INIT,
	CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.game", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.game", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.server", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.server", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.server", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.exit", FRCMD_INIT,
	CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.exit", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.exit", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("AVATARCHAT_MAIN.ubar.tabbtn", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TabBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.magicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.magicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.magicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.scratch", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.scratch", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.scratch", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.maket", FRCMD_INIT,
	CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.maket", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.maket", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.ranking", FRCMD_LBUTTONUP,
	ntAvatarChatMain::OnUnderBar_RankingUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.ranking", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.ranking", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.guild", FRCMD_INIT,
	CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.guild", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.guild", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.guild", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("AVATARCHAT_MAIN.utab.notice", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.notice", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.utab.notice", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.title", FRCMD_INIT,
	CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.bar1", FRCMD_INIT,
	CTaskMain::OnOverBar_Bar1Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.bar2", FRCMD_INIT,
	CTaskMain::OnOverBar_Bar2Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.bar3", FRCMD_INIT,
	CTaskMain::OnOverBar_Bar3Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.frame_1", FRCMD_INIT,
	CTaskMain::OnOverBar_Frame1Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.frame_2", FRCMD_INIT,
	CTaskMain::OnOverBar_Frame2Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.frame_3", FRCMD_INIT,
	CTaskMain::OnOverBar_Frame3Init)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.site", FRCMD_INIT,
	CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.char", FRCMD_INIT,
	CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.caddie", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.club", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.aztec", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.mascot", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.spcard", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.spcard", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.messenger", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.myinfo", FRCMD_INIT,
	CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.myinfo", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.myinfo", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.myinfo", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.option", FRCMD_INIT,
	CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.option", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.option", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.option", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("AVATARCHAT_MAIN.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("AVATARCHAT_MAIN.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
END_FRESH_MSGMAP()

void ntAvatarChatMain::InitUI()
{
	ResetFrWndPointers();
	m_uiMode = 0;
	m_pPortrait[0] = g_resrcmng->LoadBitmap("m_def.tga", 0, 0);
	m_pPortrait[1] = g_resrcmng->LoadBitmap("f_def.tga", 0, 0);
	m_pPortrait[2] = g_resrcmng->LoadBitmap("a_def.tga", 0, 0);
	m_pPortrait[3] = g_resrcmng->LoadBitmap("c_def.tga", 0, 0);
	m_pPortrait[4] = g_resrcmng->LoadBitmap("d_def.tga", 0, 0);
	m_pPortrait[5] = g_resrcmng->LoadBitmap("e_def.tga", 0, 0);
	m_pPortrait[6] = g_resrcmng->LoadBitmap("g_def.tga", 0, 0);
	m_pPortrait[7] = g_resrcmng->LoadBitmap("h_def.tga", 0, 0);
	m_pPortrait[8] = g_resrcmng->LoadBitmap("i_def.tga", 0, 0);
	m_pPortrait[9] = g_resrcmng->LoadBitmap("j_def.tga", 0, 0);
}

void ntAvatarChatMain::ResetFrWndPointers()
{
	m_pJukeBoxBtn = NULL;
	m_pPortraitArea = NULL;
	m_pPortraitMask = NULL;
	m_pChatView = NULL;
	m_pChatFrame = NULL;
	m_pChatTarget = NULL;
	m_pChatInput = NULL;
	m_pEmoticonBtn = NULL;
	m_pLanguage = NULL;
	m_pReportBtn = NULL;
	m_pEmoticonDlg = NULL;
	m_pReportDlg = NULL;
	m_pNoteDlg = NULL;
	m_pAddFriendDlg = NULL;
	m_pUnusedDlg = NULL;
	m_pPasswordDlg = NULL;
	m_pTradeDlg = NULL;
	m_pJukeBoxDlg = NULL;
	m_pTipDlg = NULL;
	m_pUserListDlg = NULL;
	m_pChatLogDlg = NULL;
	m_pUserListBtn = NULL;
	m_pTipBtn = NULL;
	m_pChatLogBtn = NULL;
	m_pRightMenu = NULL;
}

void ntAvatarChatMain::OnAvatarChatMainInit(int param)
{
}

void ntAvatarChatMain::OnAvatarChatMainFinish()
{
	m_pOverBarTitle->SetBgImg("title_chat");
}

void ntAvatarChatMain::OnPortraitInit(int param)
{
	m_pPortraitArea = DYNAMIC_CAST(FrArea, param);
	if (m_pPortraitArea)
	{
		WRect rect = m_pPortraitArea->GetRect();
		rect.y += m_offsetY;
		m_pPortraitArea->MoveWindow(WPoint(rect.x, rect.y));
	}
	if (ntAvatarManager::Instance()->m_pMyAvatar)
	{
		ChangePortraitImage(
			ntAvatarManager::Instance()->m_pMyAvatar->m_charTypeId & 0x3ffffff);
	}
}

void ntAvatarChatMain::OnPortraitMaskAreaInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (!area)
		return;
	area->SetVisible(true);
	WRect rect = area->GetRect();
	rect.y += m_offsetY;
	area->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnPortraitMaskInit(int param)
{
	m_pPortraitMask = DYNAMIC_CAST(FrButton, param);
	if (!m_pPortraitMask)
		return;
	m_pPortraitMask->SetVisible(true);
	WRect rect = m_pPortraitMask->GetRect();
	rect.y += m_offsetY;
	m_pPortraitMask->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnPortraitMaskBtnUp()
{
	if (CChatMsg::Instance()->IsClosed())
		CChatMsg::Instance()->Open(true, true);
	else
		CChatMsg::Instance()->Close(true, true);
}

void ntAvatarChatMain::OnPortraitTipButtonInit(int param)
{
	m_pTipBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pTipBtn)
		return;
	WRect rect = m_pTipBtn->GetRect();
	rect.y += m_offsetY;
	m_pTipBtn->MoveWindow(WPoint(rect.x, rect.y));
	if (COption::Instance()->gGetAvatarNewBie())
		OnPortraitTipButtonBtnUp();
}

bool ntAvatarChatMain::OnAvatarTipDlgResult(int result, FrForm* form)
{
	if (m_pTipBtn)
		m_pTipBtn->SetStatus(FrButton::NORMAL);
	m_pTipDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnPortraitTipButtonBtnUp()
{
	if (m_pTipDlg)
	{
		m_pTipDlg->Close(true);
	}
	else if (m_pTipBtn)
	{
		m_pTipBtn->SetStatus(FrButton::PRESSED);
		if (CChatMsg::Instance()->IsOpened())
			CChatMsg::Instance()->Close(true, true);
		m_pTipDlg = CreateForm<FrAvatarTipDlg>(g_pFresh->GetManager(), this,
			"avatartip");
		m_pTipDlg->EnableDrag(false);
		m_pTipDlg->SetFixed(true);
		m_pTipDlg->Open(
			(FRESH_PFN_RESULT)&ntAvatarChatMain::OnAvatarTipDlgResult,
			WPoint(10, 65), 21);
		m_pTipDlg->SetIconRect(m_pTipBtn->GetRect());
	}
}

void ntAvatarChatMain::OnPortraitJukeBoxButtonAreaInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (!area)
		return;
	area->SetVisible(true);
	WRect rect = area->GetRect();
	rect.y += m_offsetY;
	area->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnPortraitJukeBoxButtonInit(int param)
{
	m_pJukeBoxBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pJukeBoxBtn)
		return;
	WRect rect = m_pJukeBoxBtn->GetRect();
	rect.y += m_offsetY;
	m_pJukeBoxBtn->MoveWindow(WPoint(rect.x, rect.y));
	m_pJukeBoxBtn->SetPushDelay(0);
	m_pJukeBoxBtn->SetButtonImg(
		FrJukeBoxDlg::m_preserve_isPlaying ? "btn_jukebox_o" : "btn_jukebox_n",
		FrButton::NORMAL);
}

void ntAvatarChatMain::OnPortraitJukeBoxButtonBtnUp()
{
	ToggleJukeBox();
}

bool ntAvatarChatMain::OnJukeBoxDlgResult(int result, FrForm* form)
{
	if (m_pJukeBoxBtn)
		m_pJukeBoxBtn->SetStatus(FrButton::NORMAL);
	m_pJukeBoxDlg = NULL;
	return true;
}

void ntAvatarChatMain::ToggleJukeBox()
{
	if (m_pJukeBoxDlg)
	{
		m_pJukeBoxDlg->Close(true);
	}
	else if (m_pJukeBoxBtn)
	{
		m_pJukeBoxDlg = CreateForm<FrJukeBoxDlg>(g_pFresh->GetManager(), this,
			"jukeboxdlg");
		m_pJukeBoxDlg->Open(
			(FRESH_PFN_RESULT)&ntAvatarChatMain::OnJukeBoxDlgResult, 17);
		m_pJukeBoxDlg->SetFixed(true);
		m_pJukeBoxDlg->SetIconRect(m_pJukeBoxBtn->GetRect());
	}
}

void ntAvatarChatMain::OnPortraitChatLogButtonInit(int param)
{
	m_pChatLogBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pChatLogBtn)
		return;
	WRect rect = m_pChatLogBtn->GetRect();
	rect.y += m_offsetY;
	m_pChatLogBtn->MoveWindow(WPoint(rect.x, rect.y));
	m_pChatLogBtn->SetPushDelay(0);
}

void ntAvatarChatMain::OnPortraitChatLogButtonBtnUp()
{
	ToggleAvatarChatLog();
}

bool ntAvatarChatMain::OnAvatarChatLogDlgResult(int result, FrForm* form)
{
	if (m_pChatLogBtn)
		m_pChatLogBtn->SetStatus(FrButton::NORMAL);
	m_pChatLogDlg = NULL;
	return true;
}

void ntAvatarChatMain::ToggleAvatarChatLog()
{
	if (m_pChatLogDlg)
	{
		m_pChatLogDlg->Close(true);
	}
	else if (m_pChatLogBtn)
	{
		m_pChatLogDlg = CreateForm<FrAvatarChatLogDlg>(g_pFresh->GetManager(),
			this, "avatarchatlogdlg");
		m_pChatLogDlg->SetFixed(true);
		m_pChatLogDlg->Open(
			(FRESH_PFN_RESULT)&ntAvatarChatMain::OnAvatarChatLogDlgResult, 21);
		m_pChatLogDlg->SetIconRect(m_pChatLogBtn->GetRect());
		if (!CChatMsg::Instance()->IsClosed())
			CChatMsg::Instance()->Close(true, true);
	}
}

void ntAvatarChatMain::OnAvatarUserListButtonInit(int param)
{
	m_pUserListBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pUserListBtn)
		return;
	WRect rect = m_pUserListBtn->GetRect();
	rect.x = g_view->GetWidth() - 38;
	m_pUserListBtn->MoveWindow(WPoint(rect.x, rect.y));
	m_pUserListBtn->SetPushDelay(.5f);
}

void ntAvatarChatMain::OnAvatarUserListButtonBtnUp()
{
	ToggleAvatarUserList();
}

bool ntAvatarChatMain::OnAvatarUserListDlgResult(int result, FrForm* form)
{
	if (m_pUserListBtn)
		m_pUserListBtn->SetStatus(FrButton::NORMAL);
	m_pUserListDlg = NULL;
	return true;
}

void ntAvatarChatMain::ToggleAvatarUserList()
{
	if (m_pUserListDlg)
	{
		m_pUserListDlg->Close(true);
	}
	else if (m_pUserListBtn)
	{
		m_pUserListDlg = CreateForm<FrAvatarUserListDlg>(g_pFresh->GetManager(),
			this, "avataruserlistdlg");
		WPoint point(g_view->GetWidth() - 252, 108);
		m_pUserListDlg->EnableDrag(false);
		m_pUserListDlg->SetFixed(true);
		m_pUserListDlg->Open(
			(FRESH_PFN_RESULT)&ntAvatarChatMain::OnAvatarUserListDlgResult,
			point, 21);
		m_pUserListDlg->SetIconRect(m_pUserListBtn->GetRect());
		m_pUserListDlg->UpdateUserList();
	}
}

void ntAvatarChatMain::OnChatFrameInit(int param)
{
	m_pChatFrame = DYNAMIC_CAST(FrFrame, param);
	if (m_pChatFrame)
	{
		m_pChatFrame->SetVisible(CChatMsg::Instance()->IsOpened());
		WRect rect = m_pChatFrame->GetRect();
		rect.y += m_offsetY;
		m_pChatFrame->MoveWindow(WPoint(rect.x, rect.y));
	}
}

void ntAvatarChatMain::OnChatViewInit(int param)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, param);
	if (m_pChatView)
	{
		m_pChatView->SetVisible(true);
		m_pChatView->HidePrivacy(true);
		WRect rect = m_pChatView->GetRect();
		rect.y += m_offsetY;
		m_pChatView->MoveWindow(WPoint(rect.x, rect.y));
		m_pChatView->GetScrollBar()->Resize(8, 1, 1);
		m_pChatView->SetWheelEvent(false);
		m_chatLineCount = 8;
		HandleMsg(MsgObject(NULL, 99, 0, 0, 0, 0, 0));
	}
}

void ntAvatarChatMain::OnChatTargetInit(int param)
{
	m_pChatTarget = DYNAMIC_CAST(FrComboBox, param);
	if (m_pChatTarget)
	{
		m_pChatTarget->SetVisible(CChatMsg::Instance()->IsOpened());
		Doc()->InitReservedChatList(m_pChatTarget, (eReservedChatPartner)2);
		WRect rect = m_pChatTarget->GetRect();
		rect.y += m_offsetY;
		m_pChatTarget->MoveWindow(WPoint(rect.x, rect.y));
	}
}

void ntAvatarChatMain::OnChatTargetBtnDown(int param)
{
	if (m_pChatInput)
		m_pChatInput->SetKeyFocus(true);
}

bool ntAvatarChatMain::OnChatTargetEnterKey(int param)
{
	if (m_pChatTarget && !strlen(m_pChatTarget->GetLine(1, false)))
		m_pChatTarget->SetLine(1,
			K2L_Compatibility("\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4"), 0, false, 0);
	if (m_pChatInput)
		m_pChatInput->SetKeyFocus(true);
	return false;
}

void ntAvatarChatMain::OnChatInputInit(int param)
{
	m_pChatInput = DYNAMIC_CAST(FrEdit, param);
	if (!m_pChatInput)
		return;
	m_pChatInput->SetVisible(CChatMsg::Instance()->IsOpened());
	WRect rect = m_pChatInput->GetRect();
	rect.y += m_offsetY;
	m_pChatInput->MoveWindow(WPoint(rect.x, rect.y));
}

bool ntAvatarChatMain::OnChatInputEnterKey(int param)
{
	const char* text = (const char*)param;
	if (text && *text)
	{
		if (!GOLFDOC()->m_bShowChat)
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xc1\xf6\xb1\xdd\xc0\xba \xb4\xeb\xc8\xad\xc3\xa2\xc0\xbb \xbc\xfb\xb1\xe4 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, true, false);
		}
		else
		{
			if (!Doc()->UTIL_SendChatMessage(m_pChatTarget, text, false))
			{
				m_pChatTarget->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0,
					false, 0);
				CChatMsg::Instance()->AddChatMsg(
					"\xb1\xd3\xb8\xbb \xbb\xf3\xb4\xeb\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \x22\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4\x22\xb7\xce \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9.",
					0xffff7878, true, false);
			}
			if (!OnlinePlay())
			{
				std::string msg(
					ntAvatarManager::Instance()->m_pMyAvatar->GetNickName());
				msg += " : \b";
				msg += text;
				CChatMsg::Instance()->AddChatMsg(msg, 0xffffcc00, false, false);
				ntAvatarManager::Instance()->m_pMyAvatar->SetBalloonText(text);
				int facial = Doc()->m_chatManager.GetFaceIndex(text);
				if (facial != 5)
					ntAvatarManager::Instance()->m_pMyAvatar->SetChatFacial(
						facial);
			}
		}
	}
	else
	{
		CChatMsg::Instance()->Close(true, true);
	}
	return true;
}

void ntAvatarChatMain::OnEmoticonInit(int param)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pEmoticonBtn)
		return;
	m_pEmoticonBtn->SetVisible(CChatMsg::Instance()->IsOpened());
	WRect rect = m_pEmoticonBtn->GetRect();
	rect.y += m_offsetY;
	m_pEmoticonBtn->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnEmoticonBtnUp()
{
	if (!m_pEmoticonDlg)
	{
		m_pEmoticonDlg =
			CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
		if (m_pEmoticonDlg)
		{
			WPoint point(0, 0);
			if (m_pEmoticonBtn)
			{
				point.x = m_pEmoticonBtn->GetRect().x - 200;
				point.y = m_pEmoticonBtn->GetRect().y - 260;
			}
			if (!g_pFresh->GetManager()->IsKeyExclusive())
			{
				g_pFresh->GetManager()->SetExclusiveKey(true);
			}
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&ntAvatarChatMain::OnEmoticonResult, point,
				3);
		}
	}
	else
	{
		m_pEmoticonDlg->Close(FrCANCEL, true);
	}
}

bool ntAvatarChatMain::OnEmoticonResult(int result, FrForm* form)
{
	if (!CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* dlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (dlg)
		{
			const char* text = dlg->GetSelectedIcon();
			if (text)
			{
				if (m_pChatInput && m_pChatInput->IsVisible())
				{
					const char* front = m_pChatInput->GetEditText_Front();
					const char* comp = m_pChatInput->GetEditText_Comp();
					const char* end = m_pChatInput->GetEditText_End();
					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, text, end)) <
						m_pChatInput->GetWidthLimit())
						m_pChatInput->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, text, end), 0,
							false, 0);
					m_pChatInput->SetKeyFocus(true);
				}
				else
				{
					Doc()->m_chatManager.SendChatMessage(text, NULL, false);
					if (!OnlinePlay())
					{
						std::string msg(Doc()
								->m_userInfo[GOLFDOC()->m_currentPlayer]
								.info.sNick);
						msg += " : \b";
						msg += text;
						CChatMsg::Instance()->AddChatMsg(msg, 0xffffcc00, false,
							false);
					}
				}
			}
		}
	}
	m_pEmoticonDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnLanguageInit(int param)
{
	m_pLanguage = DYNAMIC_CAST(FrArea, param);
	if (!m_pLanguage)
		return;
	m_pLanguage->SetVisible(CChatMsg::Instance()->IsOpened());
	if (g_ime->IsAlphaNumericMode())
		m_pLanguage->SetBgImg("chat_eng");
	else
		m_pLanguage->SetBgImg("chat_kor");
	WRect rect = m_pLanguage->GetRect();
	rect.y += m_offsetY;
	m_pLanguage->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnReportInit(int param)
{
	m_pReportBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_pReportBtn)
		return;
	m_pReportBtn->SetVisible(CChatMsg::Instance()->IsOpened());
	WRect rect = m_pReportBtn->GetRect();
	rect.y += m_offsetY;
	m_pReportBtn->MoveWindow(WPoint(rect.x, rect.y));
}

void ntAvatarChatMain::OnReportBtnUp()
{
	m_pReportDlg =
		CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report");
	std::string desc;
	desc =
		"1. \xb4\xeb\xc8\xad\xc3\xa2\xc0\xc7 \xc1\xf6\xb3\xad \xb3\xbb\xbf\xeb\xb5\xb5 \xb8\xf0\xb5\xce \xbc\xad\xb9\xf6\xbf\xa1 \xb1\xe2\xb7\xcf\xb5\xcb\xb4\xcf\xb4\xd9.\x0a";
	desc +=
		"2. \xc1\xb6\xc0\xdb, \xc7\xe3\xc0\xa7, \xb9\xdd\xba\xb9\xbd\xc5\xb0\xed\xb4\xc2 \xc3\xb3\xb9\xfa\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.\x0a";
	desc +=
		"3. \xbd\xc5\xb0\xed\xc7\xcf\xbd\xc5 \xb3\xbb\xbf\xeb\xc0\xba 48\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xc3\xb3\xb8\xae\xb5\xcb\xb4\xcf\xb4\xd9.";
	m_pReportDlg->SetMessage(desc.c_str(), false);
	m_pReportDlg->Open((FRESH_PFN_RESULT)&ntAvatarChatMain::OnReportResult, 3);
}

bool ntAvatarChatMain::OnReportResult(int result, FrForm* form)
{
	m_pReportDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnRightMenuInit(int param)
{
	m_pRightMenu = DYNAMIC_CAST(FrContextMenuCtrl, param);
	if (!m_pRightMenu)
		return;
	m_pRightMenu->SetVisible(false);
	m_pRightMenu->SetFixed(true);
}

void ntAvatarChatMain::OnNoteToFriend()
{
	if (Doc()->m_myInfo.stat.i64Pang < 10)
	{
		FrForm* dlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		dlg->SetMessage(
			MakeStr(
				"\xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbb\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 %d\xc6\xce\xc0\xcc \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
				10),
			false);
		dlg->SetTimeLimit(5);
		dlg->SetFixed(true);
		WPoint point((g_view->GetWidth() - dlg->GetRect().w) * .5f,
			(g_view->GetHeight() - dlg->GetRect().h) * .5f);
		dlg->Open(NULL, point, 3);
	}
	else
	{
		ntAvatar* avatar =
			ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
		if (!m_pNoteDlg && avatar)
		{
			m_pNoteDlg =
				CreateForm<FrNoteDlg>(g_pFresh->GetManager(), this, "note");
			m_pNoteDlg->SetNick(avatar->GetNickName(), NULL);
			m_pNoteDlg->SetFixed(true);
			WPoint point((g_view->GetWidth() - m_pNoteDlg->GetRect().w) * .5f,
				(g_view->GetHeight() - m_pNoteDlg->GetRect().h) * .5f);
			m_pNoteDlg->Open(
				(FRESH_PFN_RESULT)&ntAvatarChatMain::OnNoteDlgResult, point, 1);
			WRect rect = m_pNoteDlg->GetRect();
			m_pNoteDlg->Adjust(rect);
			m_pNoteDlg->MoveWindow(WPoint(rect.x, rect.y));
		}
		if (!g_pFresh->GetManager()->IsKeyExclusive())
			g_pFresh->GetManager()->SetExclusiveKey(true);
	}
}

bool ntAvatarChatMain::OnNoteDlgResult(int result, FrForm* form)
{
	if (result == 1 &&
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick))
	{
		WSendPacket packet((enumClientPacket)60);
		packet.Encode2(0x111);
		packet.Encode4(m_pickedUID);
		packet.EncodeStr(std::string(m_pNoteDlg->GetNote()));
		packet.Encode1(0);
		packet.Send(TO_GAME);
	}
	m_pNoteDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnAddFriend()
{
	ntAvatar* avatar =
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
	if (avatar)
	{
		if (!CMessengerInfo::Instance()->IsVisible())
		{
			CMessengerInfo::Instance()->Open(true);
			if (CMessengerInfo::Instance()->IsVisible() == true)
			{
				FrMessengerDlg* dlg =
					CMessengerInfo::Instance()->GetMessengerDlg();
				if (dlg)
				{
					dlg->OpenAddFriendNickDlg();
					CMessengerInfo::Instance()->SetTargetNick(
						avatar->GetNickName());
				}
			}
		}
	}
	if (!g_pFresh->GetManager()->IsKeyExclusive())
		g_pFresh->GetManager()->SetExclusiveKey(true);
}

bool ntAvatarChatMain::OnAddFriendDlgResult(int result, FrForm* form)
{
	m_pAddFriendDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnAvatarInfo()
{
	ntAvatar* avatar =
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
	if (avatar && CUserInfo::Instance())
	{
		std::map<unsigned long, sBriefUserInfo>::iterator it =
			Doc()->m_briefUserInfoMap.find(avatar->GetGuid());
		if (it != Doc()->m_briefUserInfoMap.end())
			CUserInfo::Instance()->SetInfo(it->second.dwUid, avatar->m_guid,
				true, true, false, false, it->second.sNick);
	}
}

void ntAvatarChatMain::OnFollowAvatar()
{
	if (ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick))
	{
		ntAvatar* avatar = ntAvatarManager::Instance()->m_pMyAvatar;
		if (!avatar->IsHoldState())
			avatar->Follow(m_pickedNick);
	}
}

void ntAvatarChatMain::OnKickAvatar()
{
	ntAvatar* avatar =
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
	if (ntAvatarManager::Instance()->m_pMyAvatar->m_bGM)
	{
		if (avatar)
		{
			WSendPacket packet((enumClientPacket)38);
			packet.Encode4(avatar->m_uid);
			packet.Send(TO_GAME);
		}
	}
}

bool ntAvatarChatMain::OnTradeDlgResult(int result, FrForm* form)
{
	m_pTradeDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnOpenTradeShop()
{
	if (Doc()->IsControlServerService(32))
	{
		this << MsgObject(this, 35,
			(int)"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else
	{
		bool allowed = false;
		IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
		actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);
		if (!allowed)
		{
			actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
		}
		else
		{
			ntAvatarManager::Instance()->m_pMyAvatar->Stop();
			if (!Doc()->m_myInfo.stat.Level)
			{
				SetTradeResultCode(23, 225, NULL);
			}
			else
			{
				switch (Doc()->GetStateTrade())
				{
				case 0:
					Doc()->SendTradeEditShop();
					break;
				case 1:
				case 3:
				case 4:
					if (!m_pTradeDlg)
					{
						m_pTradeDlg = CreateForm<FrTradeDlg>(
							g_pFresh->GetManager(), this, "tradedlg");
						if (m_pTradeDlg)
						{
							bool package = false;
							IActor* gateway = GetActor(s_gatewayActor[1]);
							if (gateway)
								gateway << MsgObject(NULL, 627, 6,
									(int)&package, 0, 0, 0);
							m_pTradeDlg->Open(
								(FRESH_PFN_RESULT)&ntAvatarChatMain::
									OnTradeDlgResult,
								192);
							m_pTradeDlg->SetPackageSale(package == true
									? eTypeSalesTrade_Package
									: eTypeSalesTrade_Each);
							m_pTradeDlg->OpenTradeShop(Doc()->GetTradeUID());
							m_pTradeDlg->SetFadeout(false);
						}
						Doc()->SendTradeShowVisitor();
						Doc()->SendTradeIncome();
					}
					break;
				case 2:
					if (!Doc()->SendTradeEditShop())
						SetTradeResultCode(0, 225, NULL);
					break;
				}
			}
		}
	}
}

void ntAvatarChatMain::OnEnterTradeShop()
{
	bool allowed = false;
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);
	if (!allowed)
	{
		actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
	}
	else
	{
		ntAvatar* avatar =
			ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
		if (!avatar)
			return;
		if (WVectorLen(avatar->GetPosition() -
				ntAvatarManager::Instance()->m_pMyAvatar->GetPosition()) >
			19.2f)
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xcf\xbd\xc3\xb7\xc1\xb8\xe9 \xb4\xf5 \xb0\xa1\xb1\xee\xc0\xcc \xb0\xa1\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
				0xffff00, true, false);
		}
		else
		{
			ntAvatarManager::Instance()->m_pMyAvatar->Stop();
			if (!Doc()->GetStateTrade())
			{
				Doc()->SendTradeEnterShop(avatar->GetUID());
				return;
			}
			CChatMsg::Instance()->AddChatMsg(
				"\xbb\xf3\xc1\xa1\xc0\xbb \xb0\xb3\xbc\xb3\xc1\xdf\xbf\xa1\xb4\xc2 \xb4\xd9\xb8\xa5 \xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0xffff00, true, false);
		}
	}
}

bool ntAvatarChatMain::OnPasswordDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
		if (m_pPasswordDlg)
		{
			FrWnd* child = m_pPasswordDlg->FindChildByName("password");
			FrEdit* edit = DYNAMIC_CAST(FrEdit, child);
			std::string password;
			if (edit)
			{
				const char* line = edit->GetLine(1, false);
				password = line;
				HandleMsg(MsgObject(NULL, 39, (int)line, 0, 0, 0, 0));
			}
			FrWnd* ok = m_pPasswordDlg->FindChildByName("ok");
			if (ok)
				ok->Enable(false);
			WSendPacket packet((enumClientPacket)9);
			packet.Encode2(Doc()->m_roomInfo.roomGuid);
			packet.EncodeStr(password);
			packet.Send(TO_GAME);
			HandleMsg(MsgObject(NULL, 22, 0, 0, 0, 0, 0));
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
	}
	m_pPasswordDlg = NULL;
	return true;
}

void ntAvatarChatMain::OnRightMenuLBtnDown()
{
	if (m_pRightMenu)
	{
		m_pRightMenu->SetVisible(false);
		const char* text = m_pRightMenu->GetSelectMenuText();
		if (IsLocalContent((localContentType_t)119))
		{
			if (!strcmp(text, "1:1\xb0\xc5\xb7\xa1"))
			{
				ntAvatar* avatar =
					ntAvatarManager::Instance()->FindAvatarByNickName(
						m_pickedNick);
				if (avatar)
				{
					SendMsg(NULL, "PrivateTrade", 583, 0, avatar->GetGuid(),
						(int)avatar->GetNickName(), 0);
					return;
				}
			}
		}
		if (!strcmp(text, "\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2"))
			OnFollowAvatar();
		else if (!strcmp(text, "\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"))
			OnAvatarInfo();
		else if (!strcmp(text, "\xb1\xd3\xbc\xd3\xb8\xbb"))
		{
			ntAvatar* avatar =
				ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
			if (avatar)
				this << MsgObject(NULL, 55, (int)avatar->GetNickName(), 0, 0, 0,
					0);
		}
		else if (!strcmp(text, "\xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb"))
			OnAddFriend();
		else if (!strcmp(text, "\xc2\xca\xc1\xf6"))
			OnNoteToFriend();
		else if (!strcmp(text, "\xbb\xf3\xc1\xa1\xb8\xb8\xb5\xe9\xb1\xe2") ||
			!strcmp(text, "\xbb\xf3\xc1\xa1\xc6\xed\xc1\xfd"))
			OnOpenTradeShop();
		else if (!strcmp(text, "\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6"))
			OnEnterTradeShop();
		else if (!strcmp(text, "\xb0\xad\xc1\xa6\xc5\xf0\xc0\xe5"))
			OnKickAvatar();
	}
}

void ntAvatarChatMain::OnRightMenuRBtnDown()
{
	if (m_pRightMenu)
		m_pRightMenu->SetVisible(false);
}

void ntAvatarChatMain::UpdateRightMenu()
{
	ntAvatar* avatar =
		ntAvatarManager::Instance()->FindAvatarByNickName(m_pickedNick);
	if (avatar)
	{
		if (avatar != ntAvatarManager::Instance()->m_pMyAvatar)
		{
			if (ntAvatarManager::Instance()->m_pMyAvatar->m_bGM)
				m_pRightMenu->SetMenuList(
					"\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6@store|separator|\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2@follow|\xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb@friend_request|\xb1\xd3\xbc\xd3\xb8\xbb@whisper|\xc2\xca\xc1\xf6@chat_note|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2@information|separator|\xb0\xad\xc1\xa6\xc5\xf0\xc0\xe5@exit");
			else if (avatar->m_bGM == true)
				m_pRightMenu->SetMenuList(
					"\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2@follow|\xb1\xd3\xbc\xd3\xb8\xbb@whisper");
			else if (IsLocalContent((localContentType_t)119))
				m_pRightMenu->SetMenuList(
					"1:1\xb0\xc5\xb7\xa1@store|separator|\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6@store|separator|\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2@follow|\xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb@friend_request|\xb1\xd3\xbc\xd3\xb8\xbb@whisper|\xc2\xca\xc1\xf6@chat_note|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2@information");
			else
				m_pRightMenu->SetMenuList(
					"\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6@store|separator|\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2@follow|\xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb@friend_request|\xb1\xd3\xbc\xd3\xb8\xbb@whisper|\xc2\xca\xc1\xf6@chat_note|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2@information");
			if (!(avatar->GetState() & 0x80))
				m_pRightMenu->UnableMenuItem(
					"\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6");
			if (WVectorLen(
					ntAvatarManager::Instance()->m_pMyAvatar->GetPosition() -
					avatar->GetPosition()) > 64)
			{
				m_pRightMenu->UnableMenuItem(
					"\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
				if (IsLocalContent((localContentType_t)119))
					m_pRightMenu->UnableMenuItem("1:1\xb0\xc5\xb7\xa1");
			}
			if (ntAvatarManager::Instance()->m_pMyAvatar->GetState() & 0x80)
			{
				m_pRightMenu->UnableMenuItem(
					"\xbb\xf3\xc1\xa1\xb1\xb8\xb0\xe6");
				m_pRightMenu->UnableMenuItem(
					"\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
				m_pRightMenu->UnableMenuItem(
					"\xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb");
				m_pRightMenu->UnableMenuItem("\xc2\xca\xc1\xf6");
				m_pRightMenu->UnableMenuItem(
					"\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2");
				m_pRightMenu->UnableMenuItem(
					"\xb0\xad\xc1\xa6\xc5\xf0\xc0\xe5");
				if (IsLocalContent((localContentType_t)119))
					m_pRightMenu->UnableMenuItem("1:1\xb0\xc5\xb7\xa1");
			}
		}
		else
		{
			if (Doc()->GetStateTrade())
				m_pRightMenu->SetMenuList(
					"\xbb\xf3\xc1\xa1\xc6\xed\xc1\xfd@store");
			else
				m_pRightMenu->SetMenuList(
					"\xbb\xf3\xc1\xa1\xb8\xb8\xb5\xe9\xb1\xe2@store_make|separator|\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2@information");
		}
	}
	else
	{
		m_pRightMenu->SetVisible(false);
	}
}

void ntAvatarChatMain::PopupRightMenu(int x, int y)
{
	if (m_pRightMenu)
	{
		m_pRightMenu->MoveWindow(WPoint((float)x, (float)y));
		m_pRightMenu->SetVisible(true);
		m_pRightMenu->SetTopmost(true);
		UpdateRightMenu();
	}
}

void ntAvatarChatMain::ChangePortraitImage(int index)
{
	if (index >= 0 && (unsigned int)index < 10)
	{
		if (m_pPortraitArea)
			m_pPortraitArea->SetBgImg(m_pPortrait[index]);
	}
}

void ntAvatarChatMain::CloseAllDialogs()
{
	if (m_pTipDlg)
		m_pTipDlg->Close(true);
	if (m_pUserListDlg)
		m_pUserListDlg->Close(true);
	if (m_pChatLogDlg)
		m_pChatLogDlg->Close(true);
	CTaskMain::CloseAllDialogs();
}

void ntAvatarChatMain::OnUnderBar_ShopUp()
{
	if (ntAvatarManager::Instance()->m_pMyAvatar)
		ntAvatarManager::Instance()->m_pMyAvatar->Stop();
	CTaskMain::OnUnderBar_ShopUp();
}

void ntAvatarChatMain::OnUnderBar_MyRoomUp()
{
	if (ntAvatarManager::Instance()->m_pMyAvatar)
		ntAvatarManager::Instance()->m_pMyAvatar->Stop();
	CTaskMain::OnUnderBar_MyRoomUp();
}

void ntAvatarChatMain::OnUnderBar_CookieUp()
{
	if (ntAvatarManager::Instance()->m_pMyAvatar)
		ntAvatarManager::Instance()->m_pMyAvatar->Stop();
	CTaskMain::OnUnderBar_CookieUp();
}

void ntAvatarChatMain::OnUnderBar_RankingUp()
{
	if (ntAvatarManager::Instance()->m_pMyAvatar)
		ntAvatarManager::Instance()->m_pMyAvatar->Stop();
	CTaskMain::OnUnderBar_RankingUp();
}

void ntAvatarChatMain::ProcessUI(float delta)
{
	if (g_pFresh->GetManager()->GetModalForm())
	{
		ntAvatar* avatar = ntAvatarManager::Instance()->m_pMyAvatar;
		if (avatar->IsMoving())
			avatar->Stop();
	}
	if (!CQuitWindow::Instance()->IsActive())
	{
		if ((g_input->Get("LCONTROL", false) ||
				g_input->Get("RCONTROL", false)) &&
			g_input->GetDown("HIDE", false))
		{
			m_uiMode = (m_uiMode + 1) % 3;
			g_pFresh->Toggle(m_uiMode != 2);
			CMouseCursor::Instance()->Toggle(m_uiMode != 2);
			ShowOverAndUnderBar(m_uiMode != 1);
			if (m_uiMode == 2 && !CChatMsg::Instance()->IsClosed())
				CChatMsg::Instance()->Close(true, true);
			ntAvatarManager::Instance()->m_bShowName = m_uiMode == 0;
		}
	}
	else
	{
		m_uiMode = 0;
		g_pFresh->Toggle(true);
	}
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("EMOTICON", false) && !RANKINGDLG() &&
		!CMessengerInfo::Instance()->IsVisible())
		OnEmoticonBtnUp();
	static bool alphaNumeric = true;
	if (m_pLanguage && g_ime->IsAlphaNumericMode() != alphaNumeric)
	{
		alphaNumeric = g_ime->IsAlphaNumericMode();
		if (alphaNumeric)
			m_pLanguage->SetBgImg("chat_eng");
		else
			m_pLanguage->SetBgImg("chat_kor");
	}
	if (m_pChatInput && m_pChatTarget && !CChatMsg::Instance()->IsClosed())
	{
		if (g_input->GetDown("PAGE_UP", true))
		{
			if (m_pChatInput->HasKeyFocus() && Doc()->m_chatHistory.size() > 0)
			{
				std::list<std::string>::iterator it = std::find(
					Doc()->m_chatHistory.begin(), Doc()->m_chatHistory.end(),
					m_pChatInput->GetLine(1, false));
				if (it != Doc()->m_chatHistory.end())
				{
					if (it != Doc()->m_chatHistory.begin())
						CChatMsg::Instance()->SetChatText((*--it).c_str(),
							false);
				}
				else
					CChatMsg::Instance()->SetChatText(
						Doc()->m_chatHistory.back().c_str(), false);
			}
			else if (m_pChatTarget->HasKeyFocus() &&
				Doc()->m_reservedChatList.size() > 0)
				CChatMsg::Instance()->SetChatText(
					Doc()->GetPrevChatTarget(m_pChatTarget->GetLine(1, false)),
					false);
		}
		else if (g_input->GetDown("PAGE_DOWN", true))
		{
			if (m_pChatInput->HasKeyFocus() && Doc()->m_chatHistory.size() > 0)
			{
				std::list<std::string>::iterator it = std::find(
					Doc()->m_chatHistory.begin(), Doc()->m_chatHistory.end(),
					m_pChatInput->GetLine(1, false));
				if (it != Doc()->m_chatHistory.end())
				{
					++it;
					if (it != Doc()->m_chatHistory.end())
						CChatMsg::Instance()->SetChatText((*it).c_str(), false);
					else
						CChatMsg::Instance()->SetChatText("", false);
				}
			}
			else if (m_pChatTarget->HasKeyFocus() &&
				Doc()->m_reservedChatList.size() > 0)
				CChatMsg::Instance()->SetChatText(
					Doc()->GetNextChatTarget(m_pChatTarget->GetLine(1, false)),
					false);
		}
	}
	if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) &&
		g_input->GetDown("JUKEBOX", true))
		ToggleJukeBox();
	if (ntAvatarManager::Instance()->m_pMyAvatar->GetState() & 0x80)
	{
		TurnOffUnderTabBar();
		EnableOverAndUnderBar(false);
	}
	else
		EnableOverAndUnderBar(true);
	if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"))
		ntAvatarManager::Instance()->m_pMyAvatar->AddState(8);
	else
		ntAvatarManager::Instance()->m_pMyAvatar->SubState(8);
	if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg"))
		ntAvatarManager::Instance()->m_pMyAvatar->AddState(16);
	else
		ntAvatarManager::Instance()->m_pMyAvatar->SubState(16);
	if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("recycledlg"))
		ntAvatarManager::Instance()->m_pMyAvatar->AddState(32);
	else
		ntAvatarManager::Instance()->m_pMyAvatar->SubState(32);
	if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("rankingdlg"))
		ntAvatarManager::Instance()->m_pMyAvatar->AddState(64);
	else
		ntAvatarManager::Instance()->m_pMyAvatar->SubState(64);
}
