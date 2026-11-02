#include "stdafx.h"

#include "ghostmain.h"
#include "objectfactory.h"
#include "actor.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "partsinfo.h"
#include "playerdlg.h"
#include "shareddoc.h"
#include "uccmanager.h"
#include "bitmap.h"
#include "frlistbox.h"
#include "frarea.h"
#include "frbutton.h"
#include "wfont.h"
#include "wview.h"
#include "frdesktop.h"
#include "packet.h"
#include "rankingdlg.h"
#include "rankinginfo.h"
#include "user_info.h"
#include "clientsetting.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "ghostdocument.h"
#include "inputmanager.h"
#include "ghosthandler.h"
#include "mascotinfodlg.h"
#include "bongdarishopdlg.h"
#include "recycledlg.h"
#include "equipdlg.h"
#include "commonutil.h"
#include "ghostmanager.h"
#include "taskmanager.h"
#include "cardsystem.h"
#include "mousecursor.h"
#include "type_convert.h"

IMPLEMENT_ACTOR(CGhostMain, CTaskMain)

BEGIN_FRESH_MSGMAP(CGhostMain, CTaskMain)
ON_FRESH_VI("GHOST", FRCMD_INIT, CGhostMain::OnInitGhostMain)
ON_FRESH_VV("GHOST", FRCMD_FINISH, CGhostMain::OnInitGhostFinish)
ON_FRESH_VI("GHOST.start", FRCMD_INIT, CGhostMain::OnInitStart)
ON_FRESH_VV("GHOST.start", FRCMD_LBUTTONDOWN, CGhostMain::OnDownStart)
ON_FRESH_VI("GHOST.pet_user", FRCMD_INIT, CGhostMain::OnInitPlayerZone)
ON_FRESH_VV("GHOST.pet_user", FRCMD_OWNERDRAW, CGhostMain::OnDrawPlayerZone)
ON_FRESH_VI("GHOST.pet_ghost", FRCMD_INIT, CGhostMain::OnInitGhostZone)
ON_FRESH_VV("GHOST.pet_ghost", FRCMD_OWNERDRAW, CGhostMain::OnDrawGhostZone)
ON_FRESH_VI("GHOST.tip_ghost", FRCMD_INIT, CGhostMain::OnInitTipZone)
ON_FRESH_VV("GHOST.tip_ghost", FRCMD_OWNERDRAW, CGhostMain::OnDrawTipZone)
ON_FRESH_VI("GHOST.user_stat", FRCMD_INIT, CGhostMain::OnInitAreaUserStat)
ON_FRESH_VV("GHOST.user_stat", FRCMD_OWNERDRAW, CGhostMain::OnDrawAreaUserStat)
ON_FRESH_VI("GHOST.ghost_stat", FRCMD_INIT, CGhostMain::OnInitAreaGhostStat)
ON_FRESH_VV("GHOST.ghost_stat", FRCMD_OWNERDRAW,
	CGhostMain::OnDrawAreaGhostStat)
ON_FRESH_VI("GHOST.select_course", FRCMD_INIT, CGhostMain::OnInitSelectCourse)
ON_FRESH_VV("GHOST.select_course", FRCMD_LBUTTONDOWN,
	CGhostMain::OnDownSelectCourse)
ON_FRESH_VI("GHOST.user_equip", FRCMD_INIT, CGhostMain::OnInitUserEquip)
ON_FRESH_VI("GHOST.user_equip", FRCMD_OWNERDRAW, CGhostMain::OnDrawUserEquip)
ON_FRESH_VV("GHOST.user_equip", FRCMD_LBUTTONDOWN, CGhostMain::OnDownUserEquip)
ON_FRESH_VI("GHOST.ghost_equip", FRCMD_INIT, CGhostMain::OnInitGhostEquip)
ON_FRESH_VI("GHOST.ghost_equip", FRCMD_OWNERDRAW, CGhostMain::OnDrawGhostEquip)
ON_FRESH_VI("GHOST.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("GHOST.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("GHOST.ubar.back", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("GHOST.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("GHOST.ubar.front", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("GHOST.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("GHOST.ubar.shop", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("GHOST.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.myroom", FRCMD_INIT, CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("GHOST.ubar.myroom", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("GHOST.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("GHOST.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("GHOST.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.itemstorage", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("GHOST.ubar.gift", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("GHOST.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.cookie", FRCMD_INIT, CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("GHOST.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("GHOST.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("GHOST.ubar.game", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("GHOST.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.server", FRCMD_INIT, CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("GHOST.ubar.server", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("GHOST.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("GHOST.ubar.exit", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("GHOST.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.ubar.tabbtn_b", FRCMD_INIT, CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("GHOST.ubar.tabbtn_b", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TabBtnUp)
ON_FRESH_VI("GHOST.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)
ON_FRESH_VI("GHOST.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("GHOST.utab.magicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("GHOST.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("GHOST.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("GHOST.utab.tikimagicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.tikimagicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.scratch", FRCMD_INIT, CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("GHOST.utab.scratch", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("GHOST.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("GHOST.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("GHOST.utab.bongdarishop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.bongdarishop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("GHOST.utab.maket", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("GHOST.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.ranking", FRCMD_INIT, CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("GHOST.utab.ranking", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("GHOST.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("GHOST.utab.guild", FRCMD_LBUTTONUP, CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("GHOST.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.utab.notice", FRCMD_INIT, CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("GHOST.utab.notice", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("GHOST.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("GHOST.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("GHOST.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("GHOST.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("GHOST.obar.char", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("GHOST.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("GHOST.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("GHOST.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("GHOST.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("GHOST.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("GHOST.obar.club", FRCMD_LBUTTONDOWN, CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("GHOST.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("GHOST.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("GHOST.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("GHOST.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("GHOST.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("GHOST.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("GHOST.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("GHOST.obar.char_sel", FRCMD_INIT, CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("GHOST.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("GHOST.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("GHOST.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("GHOST.obar.club_sel", FRCMD_INIT, CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("GHOST.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("GHOST.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("GHOST.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("GHOST.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("GHOST.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("GHOST.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("GHOST.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("GHOST.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("GHOST.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("GHOST.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("GHOST.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("GHOST.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("GHOST.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("GHOST.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("GHOST.obar.messenger", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("GHOST.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("GHOST.obar.myinfo", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("GHOST.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("GHOST.obar.option", FRCMD_LBUTTONUP, CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("GHOST.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GHOST.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GHOST.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("GHOST.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("GHOST.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("GHOST.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("GHOST.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("GHOST.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("GHOST.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("GHOST.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
END_FRESH_MSGMAP()

std::string __stdcall GetIconName(const unsigned long& typeId)
{
	std::string name("w_");
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
	if (item)
		name += item->Icon;
	else
		name = "w_caddie_no";
	return name;
}

void __stdcall GetEquipIcons(std::vector<std::string>& icons,
	const sUserInfo& info)
{
	icons.clear();

	icons.reserve(6);
	icons.assign(6, "");
	icons[0] = GetIconName(info.charInfo.tid);
	icons[1] = GetIconName(info.caddieInfo.tid);
	icons[2] = GetIconName(info.clubInfo.tid);
	icons[3] = GetIconName(info.userEquip.tidBall);
	icons[4] = GetIconName(info.mascotInfo.tid);
	icons[5] = std::string("gameroom_equip_item_bn");
}

CGhostMain::cImgBinder::sImgInfo::sImgInfo(const std::string& strImage,
	const WRect& rect)
	: m_strImage(strImage), m_rect(rect)
{
}

CGhostMain::cImgBinder::cImgBinder(int nReserve)
{
	m_vecImage.reserve(nReserve);
}

CGhostMain::cImgBinder::~cImgBinder(void)
{
}

void CGhostMain::cImgBinder::AddImage(std::string strImage, const WPoint& pos)
{
	const Bitmap* bitmap = g_pFresh->GetBitmap(strImage.c_str());
	if (bitmap)
		m_vecImage.push_back(sImgInfo(strImage,
			WRect(pos,
				WSize((float)bitmap->Width(), (float)bitmap->Height()))));
}

WPoint CGhostMain::cImgBinder::GetPos(void)
{
	WPoint pos(0.0f, 0.0f);
	if (!m_vecImage.empty())
		pos = WPoint(m_vecImage[0].m_rect.x, m_vecImage[0].m_rect.y);
	return pos;
}

void CGhostMain::cImgBinder::SetPosX(float x)
{
	float curX = 0.0f;
	if (!m_vecImage.empty())
		curX = m_vecImage[0].m_rect.x;

	AddPos(x - curX, 0.0f);
}

void CGhostMain::cImgBinder::AddPos(float dx, float dy)
{
	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it)
	{
		(*it).m_rect.x += dx;
		(*it).m_rect.y += dy;
	}
}

void CGhostMain::cImgBinder::AddPos(const WPoint& delta)
{
	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it)
	{
		(*it).m_rect.x += delta.x;
		(*it).m_rect.y += delta.y;
	}
}

void CGhostMain::cImgBinder::Render(unsigned char alpha)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap((*it).m_strImage.c_str());
		if (bitmap)
			gdi->DrawTexture(bitmap, (*it).m_rect, (alpha << 24) | 0xFFFFFF, 0);
	}
}

CGhostMain::CGhostMain(void)
	: m_userPet(0),
	  m_ghostPet(0),
	  m_userParts(0),
	  m_ghostParts(0),
	  m_ghostAlpha(0),
	  m_ghostVisible(false),
	  m_userStatImage(0),
	  m_ghostStatImage(0),
	  m_userStatAlpha(0),
	  m_userStatStopX(0),
	  m_ghostStatStopX(0),
	  m_userStatSpeed(0),
	  m_ghostStatSpeed(0),
	  m_showUserStat(false),
	  m_statTime(0),
	  m_statSpeedTime(0),
	  m_selectedIcon(0),
	  m_equipAlpha(0),
	  m_equipDlg(0),
	  m_userStat(0),
	  m_ghostStat(0),
	  m_courseBitmap(0),
	  m_tipImage(0)
{
	Initialize();
}

CGhostMain::~CGhostMain(void)
{
}

void CGhostMain::Initialize(void)
{
	// HACK!
	(void)&g_HUGE;
	(void)&g_EPSILON;
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 5;

	if (IsLocalContent((localContentType_t)0x59))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(2);
		packet.Send((eSendTo)0);
	}

	memset(WSingleton<CSharedDoc>::Instance()->m_userInfo, 0,
		sizeof(WSingleton<CSharedDoc>::Instance()->m_userInfo));

	CCardManager::Instance()->CalcCardPeriodAndStatus();

	m_areas.reserve(5);
	m_areas.assign(5, (FrArea*)0);

	m_userParts = new CPartTidList;
	m_ghostParts = new CPartTidList;

	m_tipImage = new cImgBinder(2);
	m_tipImage->AddImage("ghost_main_tip1", WPoint(0.0f, 0.0f));
	m_tipImage->AddImage("ghost_main_tip2", WPoint(256.0f, 0.0f));
	m_tipImage->AddPos(200.0f, -120.0f);

	m_tipWait = 5.0f;
	m_tipHiding = false;
	m_userStatImage = new cImgBinder(2);
	m_userStatImage->AddImage("ghost_user_stat1", WPoint(0.0f, 0.0f));
	m_userStatImage->AddImage("ghost_user_stat2", WPoint(256.0f, 0.0f));
	m_userStatImage->AddPos(-(g_view->GetWidth() * 0.5f),
		g_view->GetHeight() - 162.0f - 77.0f);

	m_ghostStatImage = new cImgBinder(2);
	m_ghostStatImage->AddImage("ghost_ghost_stat1", WPoint(0.0f, 0.0f));
	m_ghostStatImage->AddImage("ghost_ghost_stat2", WPoint(256.0f, 0.0f));
	m_ghostStatImage->AddPos(g_view->GetWidth(),
		g_view->GetHeight() - 162.0f - 77.0f);

	m_userStatStopX = 17.0f;
	m_ghostStatStopX = g_view->GetWidth() - 129.0f - 256.0f - 17.0f;
	m_userStatSpeed = g_view->GetWidth() * 0.5f + 17.0f;
	m_ghostStatSpeed = 402.0f;

	m_userStat = BuildStatusBar(0);
	m_ghostStat = BuildStatusBar(1);
}

CStatBar* CGhostMain::BuildStatusBar(int index)
{
	sUserInfo& info = WSingleton<CSharedDoc>::Instance()->m_userInfo[index];

	if (IsLocalContent((localContentType_t)0x51))
	{
		if (index == 1)
			WSingleton<CCardManager>::Instance()->m_charInfo[1] = info.charInfo;
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(index);
	}

	CStatBar* statBar = new CStatBar(10.0f, 5.0f);
	CItemManager* pItemManager = ItemManager();
	unsigned char level[5] = { 0 }, penalty[5] = { 0 }, capacity[5] = { 0 };

	for (int i = 0; i < 5; i++)
	{
		status_t status =
			pItemManager->GetLevelWithPenalty((eLvlType)i, info.charInfo.tid,
				(const char*)&info.charInfo.PCL, info.charInfo.tidParts,
				info.charInfo.tidAuxParts, info.clubInfo.tid, info.clubInfo.PCL,
				info.caddieInfo.tid, info.stat.Level);
		level[i] = status.level;
		penalty[i] = status.penalty;
		capacity[i] = pItemManager->GetCapacity((eLvlType)i, info.charInfo.tid,
			(const char*)&info.charInfo.PCL, info.charInfo.tidParts,
			info.clubInfo.tid, info.clubInfo.PCL, info.caddieInfo.tid,
			info.stat.Level);
	}

	statBar->SetDestTime(1.2f);
	for (int i = 0; i < 5; i++)
		statBar->SetRange((eLvlType)i, level[i], capacity[i], penalty[i]);
	return statBar;
}

void CGhostMain::StatProcess(float dt)
{
	m_userStat->Process(dt);
	m_ghostStat->Process(dt);
}

CExhibition* CGhostMain::MakePet(FrArea* area, CPartTidList* parts,
	int playerIndex)
{
	m_hand.Enter();
	if (area == 0)
	{
		m_hand.Leave();
		return 0;
	}

	WRect rect = area->GetRect();
	CExhibition* pet = new CExhibition(rect.x + rect.w * 0.5f,
		rect.y + rect.h * 0.5f + 15.0f, 0.35f, false);
	pet->SetArea(rect.w, rect.h);

	sUserInfo& info = Doc()->m_userInfo[playerIndex];
	sCharacterInfo* character = &info.charInfo;

	switch (character->tid)
	{
	case 0x4000005:
		pet->SetDistance(9.0f, true, false);
		break;
	case 0x4000000:
		pet->SetDistance(11.0f, true, false);
		break;
	default:
		pet->SetDistance(10.0f, true, false);
		break;
	}

	if (!parts->SetTids(character, 0xFF) || !parts->IsComboValid())
		parts->SetDefaultTids();

	pet->SetModel(*parts, character->tidAuxParts, 0, 3.0f, 0);

	if (info.info.angelicWingsEffect)
		pet->AttachAngelWingFx();

	m_hand.Leave();

	return pet;
}

void CGhostMain::HidePet(bool wait)
{
	if (wait)
		m_hand.WaitUntilWorkDone(9999);

	if (m_userPet)
	{
		delete m_userPet;
		m_userPet = 0;
	}
	if (m_ghostPet)
	{
		delete m_ghostPet;
		m_ghostPet = 0;
	}
}

void CGhostMain::OnPreLoadInit(void)
{
	CTaskMain::OnPreLoadInit();
}

void CGhostMain::OnLoad(void)
{
	CTaskMain::OnLoad();
}

void CGhostMain::OnInit(void)
{
	CTaskMain::OnInit();
}

void CGhostMain::OnDestroy(void)
{
	m_areas.clear();

	if (m_userPet)
	{
		delete m_userPet;
		m_userPet = 0;
	}
	if (m_ghostPet)
	{
		delete m_ghostPet;
		m_ghostPet = 0;
	}
	if (m_userParts)
	{
		delete m_userParts;
		m_userParts = 0;
	}
	if (m_ghostParts)
	{
		delete m_ghostParts;
		m_ghostParts = 0;
	}

	if (m_tipImage)
	{
		delete m_tipImage;
		m_tipImage = 0;
	}
	if (m_userStatImage)
	{
		delete m_userStatImage;
		m_userStatImage = 0;
	}
	if (m_ghostStatImage)
	{
		delete m_ghostStatImage;
		m_ghostStatImage = 0;
	}

	if (m_userStat)
	{
		delete m_userStat;
		m_userStat = 0;
	}
	if (m_ghostStat)
	{
		delete m_ghostStat;
		m_ghostStat = 0;
	}

	CTaskMain::OnDestroy();
}

void CGhostMain::OnProcess(float dt)
{
	if (IsLocalContent((localContentType_t)0x57))
		RefreshAllUccClothes();

	OnProcTip(dt);

	if (m_userStatAlpha < 255.0f)
	{
		m_userStatAlpha += dt * 204.0f;
		if (m_userStatAlpha > 255.0f)
			m_userStatAlpha = 255.0f;
	}

	if (m_showUserStat)
	{
		m_userStat->Process(dt);
		m_ghostStat->Process(dt);
		if (m_equipAlpha < 255.0f)
		{
			m_equipAlpha += dt * 510.0f;
			if (m_equipAlpha > 255.0f)
				m_equipAlpha = 255.0f;
		}
	}
	else
	{
		if (m_statTime < 3.0f)
			m_statTime += dt;

		if (m_statSpeedTime < 4.0f)
			m_statSpeedTime += dt * 1.2f;

		if (m_userStatImage->GetPos().x < m_userStatStopX)
		{
			m_userStatImage->AddPos(
				m_userStatSpeed * m_statTime * m_statTime * dt, 0.0f);
			if (m_userStatImage->GetPos().x > m_userStatStopX)
				m_userStatImage->SetPosX(m_userStatStopX);
		}

		if (m_ghostStatImage->GetPos().x > m_ghostStatStopX)
		{
			m_ghostStatImage->AddPos(-(m_ghostStatSpeed * m_statSpeedTime * dt),
				0.0f);
			if (m_ghostStatImage->GetPos().x < m_ghostStatStopX)
				m_ghostStatImage->SetPosX(m_ghostStatStopX);
		}

		if (m_userStatImage->GetPos().x >= m_userStatStopX &&
			m_ghostStatImage->GetPos().x <= m_ghostStatStopX)
			m_showUserStat = true;
	}

	FrWnd* shop =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (shop != NULL ? shop : (__rtti_obj = shop))
		return;

	bool equipOpen =
		((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			  "equip")) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&FrEquipDlg::m_RTTI)
				: false) != 0;
	bool recycleOpen =
		((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			  "recycledlg")) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&FrRecycleDlg::m_RTTI)
				: false) != 0;

	if (g_input->GetDown("ESCAPE", true) && !equipOpen && !recycleOpen)
	{
		OnExit();
		return;
	}

	if (m_hand.IsWorkDone())
	{
		if (m_userPet)
		{
			m_userPet->SetEnableRotate(!equipOpen && !recycleOpen);
			m_userPet->Process(dt, false);
		}

		if (m_ghostPet)
		{
			if (m_ghostAlpha < 255.0f)
			{
				m_ghostAlpha += dt * 127.5f;
				if (m_ghostAlpha > 255.0f)
					m_ghostAlpha = 255.0f;
			}

			if (!m_ghostVisible && m_ghostAlpha > 32.0f)
				m_ghostVisible = true;

			m_ghostPet->SetAlpha((unsigned char)m_ghostAlpha);

			if (!equipOpen && !recycleOpen)
				m_ghostPet->Process(dt, false);
		}
	}

	CTaskMain::OnProcess(dt);
}

void CGhostMain::OnDisplay(void)
{
	CTaskMain::OnDisplay();
}

void CGhostMain::OnExit(void)
{
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 0;
	if (IsLocalContent((localContentType_t)0x59))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send((eSendTo)0);
	}

	WSingleton<CSharedDoc>::Instance()->m_myInfo.roomIndex = 0xFFFF;
	WSingleton<CSharedDoc>::Instance()->m_roomInfo = sRoomInfo();

	WSendPacket packet((enumClientPacket)0xba);
	packet.Encode1(10);
	packet.Encode4(
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1].info.dwUID);
	packet.Encode1(m_exitFlag);
	packet.Send((eSendTo)0);

	CGhostManager::Instance()->ReleaseDocument();

	if (WSingleton<CSharedDoc>::Instance()->m_bReturnLobbyNotice)
	{
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST",
			0, 0, 0);
	}
	else
	{
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0,
			0, 0);
	}
}

void CGhostMain::OnInitGhostFinish(void)
{
	m_pOverBarTitle->SetBgImg("title_ghost_main");
	m_pOverBarPangCookie->SetVisible(false);

	GetEquipIcons(m_userIcons,
		WSingleton<CSharedDoc>::Instance()->m_userInfo[0]);
	m_userEquip->AddItem(&m_userIcons[0]);
	m_userEquip->AddItem(&m_userIcons[1]);
	m_userEquip->AddItem(&m_userIcons[2]);
	m_userEquip->AddItem(&m_userIcons[3]);
	m_userEquip->AddItem(&m_userIcons[4]);
	m_userEquip->AddItem(&m_userIcons[5]);

	GetEquipIcons(m_ghostIcons,
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1]);
	m_ghostEquip->AddItem(&m_ghostIcons[0]);
	m_ghostEquip->AddItem(&m_ghostIcons[1]);
	m_ghostEquip->AddItem(&m_ghostIcons[2]);
	m_ghostEquip->AddItem(&m_ghostIcons[3]);
	m_ghostEquip->AddItem(&m_ghostIcons[4]);
	m_ghostEquip->AddItem(&m_ghostIcons[5]);

	m_pUnderBarShop->Enable(false);
	m_pUnderBarMyRoom->Enable(false);
	m_pUnderBarRanking->Enable(false);
	m_pUnderBarGift->Enable(false);

	WSendPacket packet((enumClientPacket)0xba);
	packet.Encode1(1);
	packet.Encode4(
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1].info.dwUID);
	packet.EncodeStr(
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1].info.sNick);
	packet.Encode1(m_exitFlag);
	packet.Send((eSendTo)0);
}

void CGhostMain::OnInitStart(int)
{
}

void CGhostMain::OnDownStart(void)
{
	FrWnd* shop = g_pFresh->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (shop != NULL ? shop : (__rtti_obj = shop))
		return;

	CGhostDocument* doc = CGhostManager::Instance()->GetDocument();
	if (doc->IsInvalidate())
	{
		CGhostHandler::Instance()->OpenNotifyDlg(8);
		return;
	}

	std::list<sSlotInfo>& players =
		WSingleton<CSharedDoc>::Instance()->m_slotList;
	players.clear();

	sSlotInfo mySlot;
	memset(&mySlot, 0, sizeof(mySlot));
	sSlotInfo ghostSlot;
	memset(&ghostSlot, 0, sizeof(ghostSlot));

	mySlot.dwGuid = WSingleton<CSharedDoc>::Instance()->m_myInfo.info.dwGuid;
	mySlot.connectionRank = 1;
	mySlot.level = WSingleton<CSharedDoc>::Instance()->m_myInfo.stat.Level;
	strcpy(mySlot.sNick,
		WSingleton<CSharedDoc>::Instance()->m_myInfo.info.sNick);
	players.push_back(mySlot);

	type_convert(doc->GetUserData(), &ghostSlot);
	players.push_back(ghostSlot);

	const GHOST_GAME_DATA* game = doc->GetGameData();
	SetCurMap(game->course);
	SetGameType(0);
	SetHoleType(game->holeType);
	SetHoles(18);
	SetShotTimeLimit(0);
	SetGameTimeLimit(0);

	for (int i = 0; i < 18; i++)
	{
		WSingleton<CSharedDoc>::Instance()->m_holeRandom[i] =
			game->holeRandom[i];
		WSingleton<CSharedDoc>::Instance()->m_courseMap[i] =
			(unsigned char)(WSingleton<CSharedDoc>::Instance()
								->m_holeRandom[i] %
				3);
		WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = game->holeIndex[i];
	}

	CMouseCursor::Instance()->SetMode(1);
	CMouseCursor::Instance()->SetActive(false);
	CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);

	WSendPacket packet((enumClientPacket)0xba);
	packet.Encode1(2);
	packet.Send((eSendTo)0);
}

void CGhostMain::OnInitPlayerZone(int param)
{
	m_areas[0] = DYNAMIC_CAST(FrArea, param);

	FrArea* area = m_areas[0];
	area->SetVisible(true);

	m_userPet = MakePet(area, m_userParts, 0);
}

void CGhostMain::OnDrawPlayerZone(void)
{
	FrWnd* shop = g_pFresh->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (shop != NULL ? shop : (__rtti_obj = shop))
		return;

	if (!m_hand.IsWorking(0))
	{
		if (CRankingInfo::Instance()->GetDlg() == 0 && USERINFODLG() == 0)
		{
			// HACK! Preserve the original out-of-line call.
#pragma inline_depth(0)
			m_hand.Enter();
#pragma inline_depth()
			if (m_userPet)
				m_userPet->Display(0.0f, 0.0f);
			m_hand.Leave();
		}
	}
}

void CGhostMain::OnInitSelectCourse(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	CGhostDocument* doc = CGhostManager::Instance()->GetDocument();
	if (doc == 0)
		return;

	IFF_STRUCT::sCourse* course =
		ItemManager()->FindCourse(0x28000000 | doc->GetGameData()->course);
	if (course == 0)
		return;

	std::string name("mid_");
	name += course->c.Icon;
	m_courseBitmap = g_pFresh->GetBitmap(name.c_str());
	m_courseRect = button->GetRect();

	m_courseRect.x +=
		abs((int)((m_courseBitmap->Width() - m_courseRect.w) * 0.5f));
	m_courseRect.y +=
		abs((int)((m_courseBitmap->Height() - m_courseRect.h) * 0.5f));
	m_courseRect.w = (float)m_courseBitmap->Width();
	m_courseRect.h = (float)m_courseBitmap->Height();
}

void CGhostMain::OnDownSelectCourse(void)
{
}

void CGhostMain::OnInitUserEquip(int param)
{
	m_userEquip = DYNAMIC_CAST(FrListBox, param);
}

void CGhostMain::OnDrawUserEquip(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0 || pGDI == 0)
		return;

	const Bitmap* icon =
		g_pFresh->GetBitmap(((std::string*)pItem->pData)->c_str());
	if (icon == 0)
		return;

	WRect rect(pItem->pos.x, pItem->pos.y, (float)icon->Width(),
		(float)icon->Height());

	pGDI->DrawTexture(icon, rect,
		((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (rect.IsInRect(
			WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
	{
		const Bitmap* frame = g_pFresh->GetBitmap("select_frame");
		if (frame)
			pGDI->DrawTexture(frame,
				WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f,
					rect.h + 6.0f),
				0xFFFFFFFF, 0);
	}
}

void CGhostMain::OnDownUserEquip(void)
{
	if (!m_showUserStat)
		return;

	FrListItem* pItem = m_userEquip->GetSelected();
	if (pItem == 0)
		return;

	if (pItem->idx == 5)
	{
		FrEquipDlg* dlg =
			CreateForm<FrEquipDlg>(g_pFresh->GetManager(), this, "equip", 0);
		if (dlg)
		{
			dlg->Init(Doc()->m_myInfo.userEquip.tidItemSlot, true);
			WRect rect = dlg->GetRect();
			rect.x = g_view->GetWidth() - rect.w - 17.0f;
			rect.y = (g_view->GetHeight() - rect.h) * 0.5f;
			dlg->SetRect(rect);
			dlg->EnableDrag(false);
			dlg->Open((FRESH_PFN_RESULT)&CGhostMain::CallBackEquipItemResult,
				3);
		}
		return;
	}
	{
		unsigned int count = 1;
		switch (pItem->idx)
		{
		case 1:
			count = Doc()->m_caddieMap.size();
			break;
		case 4:
			if (Doc()->m_mascotMap.size() != 0)
				break;
			return;
		}
		if (count == 0)
			return;

		FrSubEquipBar* bar = CreateForm<FrSubEquipBar>(g_pFresh->GetManager(),
			this, "EQUIPBAR_SUB", 0);
		if (bar == 0)
			return;

		m_selectedIcon = pItem;

		int category = 1;
		WPoint pt = pItem->pos;
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

		bar->Open((FRESH_PFN_RESULT)&CGhostMain::OnResultSubEquipBar,
			(FrSubEquipBar::eCategory)category, pt, (FrSubEquipBar::eSubType)1);

		WRect rect = bar->GetRect();
		rect.y -= rect.h;

		WView* view = g_view;

		if (rect.x + rect.w + 5.0f > view->GetWidth())
			rect.x = view->GetWidth() - rect.w - 5.0f;
		bar->SetRect(rect);
	}
}

bool CGhostMain::OnResultSubEquipBar(int result, FrForm* form)
{
	if (result != 1)
		return true;

	FrSubEquipBar* bar = (FrSubEquipBar*)form;
	IFF_ITEM_COMMON* item = bar->GetResult();
	if (item != NULL)
	{
		unsigned long typeId = item->TypeId;
		unsigned long itemGroup = typeId >> 26;
		if (itemGroup == 5)
			SendPacketEquipResult((PANGYA_ITEM_GROUP)itemGroup, typeId);
		else
			SendPacketEquipResult((PANGYA_ITEM_GROUP)itemGroup,
				bar->GetLastSelectedGuid());
	}
	else
	{
		switch (bar->GetCategory())
		{
		case 0x10:
		{
			SendPacketEquipResult((PANGYA_ITEM_GROUP)0x10, 0);
			std::string* icon = (std::string*)m_selectedIcon->pData;
			unsigned long none = 0;
			*icon = GetIconName(none);
			break;
		}
		case 7:
		{
			SendPacketEquipResult((PANGYA_ITEM_GROUP)7, 0);
			std::string* icon = (std::string*)m_selectedIcon->pData;
			unsigned long none = 0;
			*icon = GetIconName(none);
			break;
		}
		}
	}
	return true;
}

void CGhostMain::SetUserEquip(PANGYA_ITEM_GROUP group, unsigned long id)
{
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	sUserInfo* info = &WSingleton<CSharedDoc>::Instance()->m_userInfo[0];
	switch (group)
	{
	case 1:
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			doc->m_charMap.find(id);
		if (it == doc->m_charMap.end())
			return;

		doc->m_myInfo.userEquip.guidChar = id;
		WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

		info->userEquip.guidChar = id;
		info->charInfo = it->second;
		UserPetRebuild();
		if (m_userStat)
		{
			delete m_userStat;
			m_userStat = 0;
		}
		m_userStat = BuildStatusBar(0);

		m_userIcons[0] = GetIconName(info->charInfo.tid);
		break;
	}
	case 16:
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			doc->m_mascotMap.find(id);
		if (it != doc->m_mascotMap.end())
		{
			doc->m_myInfo.userEquip.guidMascot = id;
			info->userEquip.guidMascot = id;
			info->mascotInfo = it->second;
		}
		else
		{
			doc->m_myInfo.userEquip.guidMascot = 0;
			memset(&info->mascotInfo, 0, sizeof(info->mascotInfo));
		}

		if (m_userStat)
		{
			delete m_userStat;
			m_userStat = 0;
		}
		m_userStat = BuildStatusBar(0);
		m_userIcons[4] = GetIconName(info->mascotInfo.tid);
		break;
	}
	case 7:
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			doc->m_caddieMap.find(id);
		if (it != doc->m_caddieMap.end())
		{
			doc->m_myInfo.userEquip.guidCaddie = id;
			info->userEquip.guidCaddie = id;
			info->caddieInfo = it->second;
		}
		else
		{
			doc->m_myInfo.userEquip.guidCaddie = 0;
			memset(&info->caddieInfo, 0, sizeof(info->caddieInfo));
		}

		if (m_userStat)
		{
			delete m_userStat;
			m_userStat = 0;
		}
		m_userStat = BuildStatusBar(0);
		m_userIcons[1] = GetIconName(info->caddieInfo.tid);
		break;
	}
	case 5:
		doc->m_myInfo.userEquip.tidBall = id;
		info->userEquip.tidBall = id;
		m_userIcons[3] = GetIconName(id);
		break;
	case 4:
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			doc->m_clubSetMap.find(id);
		if (it == doc->m_clubSetMap.end())
			return;

		doc->m_myInfo.userEquip.guidClubSet = id;

		info->userEquip.guidClubSet = id;

		info->clubInfo.guid = it->second.guid;
		memcpy(info->clubInfo.PCL, it->second.Common,
			sizeof(info->clubInfo.PCL));
		info->clubInfo.tid = it->second.tid;

		if (m_userStat)
		{
			delete m_userStat;
			m_userStat = 0;
		}
		m_userStat = BuildStatusBar(0);
		m_userIcons[2] = GetIconName(info->clubInfo.tid);
		break;
	}
	}
}

void CGhostMain::UserPetRebuild(void)
{
	if (m_userPet)
	{
		delete m_userPet;
		m_userPet = 0;
	}
	if (m_userParts)
	{
		delete m_userParts;
		m_userParts = 0;
	}

	FrArea* area = m_areas[0];
	m_userParts = new CPartTidList;
	m_userPet = MakePet(area, m_userParts, 0);
}

bool CGhostMain::CallBackEquipItemResult(int result, FrForm* form)
{
	if (form != 0)
	{
		m_equipDlg = form;
		unsigned long* equipItem = Doc()->m_myInfo.userEquip.tidItemSlot;
		memcpy(equipItem, ((FrEquipDlg*)form)->GetMyItemSlot(),
			sizeof(Doc()->m_myInfo.userEquip.tidItemSlot));

		memcpy(Doc()->m_userInfo[0].userEquip.tidItemSlot, equipItem,
			sizeof(Doc()->m_myInfo.userEquip.tidItemSlot));
	}

	m_equipDlg = 0;
	this << MsgObject(0, 1, 0, 0, 0, 0, 0);
	WSendPacket packet((enumClientPacket)0x20);
	packet.Encode1(2);
	packet.EncodeBuffer(Doc()->m_myInfo.userEquip.tidItemSlot,
		sizeof(Doc()->m_myInfo.userEquip.tidItemSlot));
	packet.Send((eSendTo)0);

	return true;
}

bool CGhostMain::IsFocusToMain(void)
{
	FrWnd* shop =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	// HACK!
	if (shop != NULL ? shop : (__rtti_obj = shop))
		return false;

	return true;
}

void CGhostMain::OnInitGhostEquip(int param)
{
	m_ghostEquip = DYNAMIC_CAST(FrListBox, param);
}

void CGhostMain::OnDrawGhostEquip(int param)
{
	if (!m_showUserStat)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0 || pGDI == 0)
		return;

	const Bitmap* icon =
		g_pFresh->GetBitmap(((std::string*)pItem->pData)->c_str());
	const Bitmap* hide = g_pFresh->GetBitmap("ghost_equipicon_hide");
	if (icon)
		pGDI->DrawTexture(icon,
			WRect(pItem->pos.x, pItem->pos.y, (float)icon->Width(),
				(float)icon->Height()),
			((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (hide)
		pGDI->DrawTexture(hide,
			WRect(pItem->pos.x, pItem->pos.y, (float)icon->Width(),
				(float)icon->Height()),
			((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	m_pTitleFont->Print(g_view, m_courseRect.x + 40.0f, m_courseRect.y - 40.0f,
		MakeStr("-%d", m_ghostScore), 0,
		((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	float width = m_pTitleFont->Print(g_view, m_courseRect.x + 40.0f,
		m_courseRect.y - 25.0f, MakeStr("%d", m_ghostPang), 0, 0xFFFFFFFF, 0);

	const Bitmap* pang = g_pFresh->GetBitmap("ghost_pang");
	if (pang)
		pGDI->DrawTexture(pang,
			WRect(width + m_courseRect.x + 50.0f, m_courseRect.y - 25.0f,
				(float)pang->Width(), (float)pang->Height()),
			((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (m_courseBitmap)
		pGDI->DrawTexture(m_courseBitmap, m_courseRect, 0xFFFFFFFF, 0);
}

void CGhostMain::OnInitGhostZone(int param)
{
	m_areas[1] = DYNAMIC_CAST(FrArea, param);

	FrArea* area = m_areas[1];
	area->SetVisible(true);
	m_ghostPet = MakePet(area, m_ghostParts, 1);
	m_ghostPet->SetAlpha(0);
}

void CGhostMain::OnDrawGhostZone(void)
{
	if (((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			  "bongdari_shop")) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&FrBongdariShopDlg::m_RTTI)
				: false) ||
		((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			  "recycledlg")) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&FrRecycleDlg::m_RTTI)
				: false) ||
		((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			  "equip")) != 0
				? ((IObject*)__rtti_obj)->IsKindOf(&FrEquipDlg::m_RTTI)
				: false))
		return;

	if (m_hand.IsWorking(0))
		return;
	if (CRankingInfo::Instance()->GetDlg() == 0 && USERINFODLG() == 0)
	{
		m_hand.Enter();
		if (m_ghostPet && m_ghostVisible)
		{
			m_ghostPet->Display(0.0f, 0.0f);
		}
		m_hand.Leave();
	}
}

void CGhostMain::OnInitTipZone(int param)
{
	m_areas[4] = DYNAMIC_CAST(FrArea, param);

	m_tipAlpha = 0.0f;
}

void CGhostMain::OnDrawTipZone(void)
{
	m_tipImage->Render((unsigned char)m_tipAlpha);
}

void CGhostMain::OnProcTip(float dt)
{
	if (!m_showUserStat)
		return;

	if (!m_tipHiding)
	{
		if (m_tipImage->GetPos().y < 30.0f)
			m_tipImage->AddPos(WPoint(0.0f, dt * 240.0f));

		if (m_tipAlpha < 240.0f)
		{
			m_tipAlpha += dt * 240.0f;
			if (m_tipAlpha > 240.0f)
				m_tipAlpha = 240.0f;
		}

		if (m_tipAlpha == 240.0f && m_tipImage->GetPos().y >= 30.0f)
		{
			m_tipWait -= dt;
			if (m_tipWait < 0.0f)
			{
				m_tipWait = 5.0f;
				m_tipHiding = true;
			}
		}
	}
	else
	{
		if (m_tipImage->GetPos().y > -120.0f)
			m_tipImage->AddPos(WPoint(0.0f, dt * -240.0f));

		if (m_tipAlpha > 0.0f)
		{
			m_tipAlpha -= dt * 0.0f;
			if (m_tipAlpha < 0.0f)
				m_tipAlpha = 0.0f;
		}
	}
}

void CGhostMain::OnInitAreaUserStat(int param)
{
	m_areas[2] = DYNAMIC_CAST(FrArea, param);
}

void CGhostMain::OnDrawAreaUserStat(void)
{
	m_userStatImage->Render((unsigned char)m_userStatAlpha);
	if (!m_showUserStat)
		return;

	WRect rect = m_areas[2]->GetRect();
	m_userStat->Render(WPoint(rect.x + 86.0f, rect.y + 86.0f));

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int level = doc->m_userInfo[0].stat.Level + 1;
	const Bitmap* bitmap =
		g_pFresh->GetBitmap(MakeStr("level_%03d", level < 0x47 ? level : 0x47));
	if (bitmap == 0)
		return;

	gdi->DrawTexture(bitmap,
		WRect(rect.x + 30.0f, rect.y + 3.0f, (float)bitmap->Width(),
			(float)bitmap->Height()),
		0xFFFFFFFF, 0);

	gdi->SetTextColor(0xFFFFFFFF, 0xFF000000);
	gdi->SetTextStyle(2);

	g_pFresh->GetManager()->PrintText(
		WPoint((float)bitmap->Width() + rect.x + 35.0f, rect.y + 5.0f), 0,
		WSingleton<CSharedDoc>::Instance()->m_userInfo[0].info.sNick, -1.0f,
		0xFFFFFFFF);
	gdi->SetTextStyle(0);
	gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
}

void CGhostMain::OnInitAreaGhostStat(int param)
{
	m_areas[3] = DYNAMIC_CAST(FrArea, param);
}

void CGhostMain::OnDrawAreaGhostStat(void)
{
	m_ghostStatImage->Render((unsigned char)m_userStatAlpha);
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;
	if (!m_showUserStat)
		return;

	WRect rect = m_areas[3]->GetRect();
	m_ghostStat->Render(WPoint(rect.x + 210.0f, rect.y + 86.0f));

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int level = doc->m_userInfo[1].stat.Level + 1;
	const Bitmap* bitmap =
		g_pFresh->GetBitmap(MakeStr("level_%03d", level < 0x47 ? level : 0x47));
	if (bitmap == 0)
		return;

	gdi->DrawTexture(bitmap,
		WRect(rect.x + 230.0f, rect.y + 3.0f, (float)bitmap->Width(),
			(float)bitmap->Height()),
		0xFFFFFFFF, 0);

	gdi->SetTextColor(0xFFFFFFFF, 0xFF000000);
	gdi->SetTextStyle(2);

	g_pFresh->GetManager()->PrintText(
		WPoint((float)bitmap->Width() + rect.x + 235.0f, rect.y + 5.0f), 0,
		WSingleton<CSharedDoc>::Instance()->m_userInfo[1].info.sNick, -1.0f,
		0xFFFFFFFF);
	gdi->SetTextStyle(0);
	gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
}

void CGhostMain::OnInitGhostMain(int)
{
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

void CGhostMain::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0:
	{
		if (g_pFresh == 0)
			return;

		const char* layout = (const char*)msg.param1;
		g_pFresh->OpenLayout(layout, this, true);
		switch (msg.param2)
		{
		case 1:
			CGhostHandler::Instance()->OpenNotifyDlg(5);
			break;
		}
		break;
	}

	case 73:
		SetUserEquip((PANGYA_ITEM_GROUP)1,
			((const sCharacterInfo*)msg.param2)->guid);
		break;

	case 74:
	{
		const sCaddieInfo* caddie = (const sCaddieInfo*)msg.param2;
		SetUserEquip((PANGYA_ITEM_GROUP)7, caddie->guid);
		break;
	}

	case 75:
	{
		const sItemInfo* clubSet = (const sItemInfo*)msg.param2;
		SetUserEquip((PANGYA_ITEM_GROUP)4, clubSet->guid);
		break;
	}

	case 76:
		SetUserEquip((PANGYA_ITEM_GROUP)5, msg.param2);
		break;

	case 78:
	{
		const sMascotInfo* mascot = (const sMascotInfo*)msg.param2;
		if (mascot->PCBangMascot)
			COption::Instance()->mSetPCBangMascotMsg(mascot->szMsg);
		SetUserEquip((PANGYA_ITEM_GROUP)16, mascot->guid);
		break;
	}

	default:
		HandleMsgCommon(msg);
		break;
	}
}

void CGhostMain::SendPacketEquipResult(PANGYA_ITEM_GROUP group,
	unsigned long typeId)
{
	WSendPacket packet((enumClientPacket)0xb);
	unsigned char kind = 0;
	switch (group)
	{
	case 1:
		kind = 4;
		break;
	case 7:
		kind = 1;
		break;
	case 4:
		kind = 3;
		break;
	case 5:
		kind = 2;
		break;
	case 16:
		kind = 5;
		break;
	}
	packet.Encode1(kind);
	packet.Encode4(typeId);
	packet.Send((eSendTo)0);
}

void CGhostMain::RefreshAllUccClothes(void)
{
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++)
	{
		if (!UccManager()->GetRefreshItem(item))
			return;

		if (m_userPet && m_userPet->GetPetFrame() && m_userParts)
		{
			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1F);
			if (m_userParts->m_tid[slot] == item.typeId)
			{
				unsigned long serial = m_userParts->m_uid[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
				{
					m_userPet->GetPetFrame()->SetPart(m_userParts,
						pClothes->typeId, pClothes->id);
					m_userPet->GetPetFrame()->SetPart(m_userParts,
						pClothes->typeId, pClothes->id);
				}
			}
		}

		if (m_ghostPet && m_ghostPet->GetPetFrame() && m_ghostParts)
		{
			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1F);
			if (m_ghostParts->m_tid[slot] == item.typeId)
			{
				unsigned long serial = m_ghostParts->m_uid[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
				{
					m_ghostPet->GetPetFrame()->SetPart(m_ghostParts,
						pClothes->typeId, pClothes->id);
					m_ghostPet->GetPetFrame()->SetPart(m_ghostParts,
						pClothes->typeId, pClothes->id);
				}
			}
		}
	}
}
