typedef char z_rivit[sizeof(std::vector<sRivalData>::iterator)];
#include "exhibition.h"
#include "clientsetting.h"
#include "invitedlg.h"
#include "userlistdlg.h"
#include "resultdlg.h"
#include "serverdlg.h"
class FrMailBoxDlg;
#include "eventprizedlg.h"
#include "mailboxdlg.h"
#include "messengerdlg.h"
#include "valentineeventdlg.h"
#include "pointeventdlg.h"
#include "netresourcemanager.h"
#include "treasuredlg.h"
#include "topiconinterface.h"
#include "topiconmanager.h"
#include "lobbymain.h"
#include "user_info.h"
#include "chatmsg.h"
#include "buddymanager.h"
#include "cardsystem.h"
#include "contentsdoc.h"
#include "makeroomdlg.h"
#include "levelupitemdlg.h"
#include "standinginline.h"
#include "guilddefine.h"
#include "tooltip.h"
#include "skinnedtheme.h"
#include "gamedatadb.h"
#include "mousecursor.h"
#include "browser.h"
#include "projectg.h"
#include "familymain.h"
#include "rankinginfo.h"
#include "logindlg.h"
#include "detailedroominfodlg.h"
#include "scoredlg.h"
#include "passworddlg.h"
#include "noticedlg.h"
#include "emoticondlg.h"
#include "reportdlg.h"
#include "newrecorddlg.h"
#include "parannickdlg.h"
#include "caddiewarningdlg.h"
#include "equipdlg.h"
#include "worldtoureventdlg.h"
#include "frfindeborteventdlg.h"
#include "frhalloweendlg.h"
#include "christamseventdlg.h"
#include "christmassockeventdlg.h"
#include "frticketexchangedlg.h"
#include "dlgquickstartsetting.h"
#include "createnickdlg.h"
#include "missioneventdlg.h"
#include "bingoeventdlg.h"
#include "birthdayeventdlg.h"
#include "changeroominfodlg.h"
#include "guildnoticedlg.h"
#include "onelinereqdlg.h"
#include "frcombobox.h"
#include "frdesktop.h"
#include "uccmanager.h"
#include "partsinfo.h"
#include "../../shared/s5/sharedutilities.h"
#include "optiondlg.h"
#include "itembuffactor.h"
#include "intrusion.h"
#include "../../shared/sharedtables.h"
#include "s5/utilities.h"
#include "guildactor.h"
#include "userinfoform.h"
#include "caddieinfodlg.h"
#include "mathconsts.h"

// HACK
template <>
inline int Abs(int value)
{
	return value > 0 ? value : -value;
}

class ILoginInfo;

class CLoginInfo
{
public:
	static CLoginInfo* Instance();
	int IsWebLogin() const { return m_bWebLogin; }
	int IsPcBang() const;

private:
	virtual ~CLoginInfo();

	ILoginInfo* m_pLoginInfo;
	int m_bWebLogin;
};

#include "intrusion.h"

class cTopEventIcon
{
public:
	cTopEventIcon(eTopIconID id)
		: m_id(id)
	{
	}

	void Prepare(FrButton* button);

	eTopIconID m_id;
};

void cTopEventIcon::Prepare(FrButton* button)
{
	if (button)
	{
		button->SetPushSound("ui_desktop_icon_click");
		button->EnableHover(true, 0.5f);
		int index = CLoginInfo::Instance()->IsPcBang() ? m_id : m_id - 1;
		WRect rect;
		rect.x = (float)(80 * index) + 1.0f;
		rect.y = 50.0f;
		rect.w = 80.0f;
		rect.h = 80.0f;
		button->SetRect(rect);
	}
}

std::string g_strCate[5] = { "\xb4\xeb\xc0\xfc", "\xb4\xeb\xc8\xb8",
	"\xb9\xe8\xc6\xb2", "\xb4\xeb\xc8\xad", "\xb8\xf0\xb5\xce" };

std::string g_strGameType[8] = { "\xbd\xba\xc6\xae\xb7\xce\xc5\xa9",
	"\xb8\xc5\xc4\xa1", "30\xc0\xce\xb4\xeb\xc8\xb8",
	"30\xc0\xce\xc6\xc0\xc0\xfc", "\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc",
	"\xc6\xce\xb9\xe8\xc6\xb2", "\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1",
	"\xb4\xeb\xc8\xad" };

int GetCateByGameType(int gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_30S:
		return 1;
	case GAME_TYPE_30S_TEAM:
		return 1;
	case GAME_TYPE_GUILD_MATCH:
		return 1;
	case GAME_TYPE_SKINS:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		return 2;
	case GAME_TYPE_AVATARCHAT:
		return 3;
	case GAME_TYPE_STROKE:
		return 0;
	case GAME_TYPE_TEAM:
		return 0;
	case GAME_TYPE_MATCH:
		return 0;
	}
	return 0;
}

long GetShotTimeLimitByGameType(int gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_AVATARCHAT:
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
		return 0;
	case GAME_TYPE_STROKE:
		return 40000;
	case GAME_TYPE_TEAM:
		return 40000;
	case GAME_TYPE_MATCH:
		return 40000;
	case GAME_TYPE_APPROACH:
		return 40000;
	case GAME_TYPE_NEW_APPROACH:
		return 40000;
	}
	return 40000;
}

long GetGameTimeLimitByGameType(int gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
		return 2400000;
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		return 40000;
	case GAME_TYPE_STROKE:
		return 0;
	case GAME_TYPE_TEAM:
		return 0;
	case GAME_TYPE_MATCH:
		return 0;
	case GAME_TYPE_AVATARCHAT:
		return 0;
	}
	return 0;
}

unsigned char GetRandGameTypeByCate(int cate)
{
	switch (cate)
	{
	case 1:
		return GAME_TYPE_30S;
	case 2:
		return GAME_TYPE_NEW_APPROACH;
	case 3:
		return GAME_TYPE_AVATARCHAT;
	case 0:
		return GAME_TYPE_STROKE;
	case 4:
		return GAME_TYPE_STROKE;
	}
	return GAME_TYPE_STROKE;
}

static const char* GetGameTypeText(unsigned char gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_STROKE:
		return K2L_Compatibility("\xbd\xba\xc6\xae\xb7\xce\xc5\xa9");
	case GAME_TYPE_AVATARCHAT:
		return K2L_Compatibility("\xc3\xa4\xc6\xc3\xb9\xe6");
	case GAME_TYPE_MATCH:
		return K2L_Compatibility("\xb8\xc5\xc4\xa1");
	case GAME_TYPE_30S:
		return K2L_Compatibility("\xb4\xeb\xc8\xb8");
	case GAME_TYPE_30S_TEAM:
		return K2L_Compatibility("\xb4\xeb\xc8\xb8\xc6\xc0\xc0\xfc");
	case GAME_TYPE_GUILD_MATCH:
		return K2L_Compatibility("\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc");
	case GAME_TYPE_SKINS:
		return K2L_Compatibility("\xc6\xce\xb9\xe8\xc6\xb2");
	case GAME_TYPE_REALMYROOM:
		return K2L_Compatibility("\xb8\xae\xbe\xf3\xb8\xb6\xc0\xcc\xb7\xeb");
	case GAME_TYPE_NEW_APPROACH:
		return K2L_Compatibility("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1");
	}
	return "";
}

bool ApproachRankUserCompare(const void* a, const void* b);
bool RankUserCompare(const void* a, const void* b);
bool FinishUserCompare(const void* a, const void* b);
bool ConnectionUserCompare(const void* a, const void* b);

bool ApproachRankUserCompare(const void* a, const void* b)
{
	const sRivalData* pA = (const sRivalData*)a;
	const sRivalData* pB = (const sRivalData*)b;

	if (pA->quitOrder > pB->quitOrder)
		return true;

	if (pA->quitOrder < pB->quitOrder)
		return false;

	if (pA->approachResultDistance < pB->approachResultDistance)
		return true;

	if (pA->approachResultDistance > pB->approachResultDistance)
		return false;

	if (pA->approachTime > pB->approachTime)
		return true;

	if (pA->approachTime < pB->approachTime)
		return false;

	return pA->order < pB->order;
}

bool RankUserCompare(const void* a, const void* b)
{
	const sRivalData* pA = (const sRivalData*)a;
	const sRivalData* pB = (const sRivalData*)b;

	if (pA->quitOrder > pB->quitOrder)
		return true;

	if (pA->quitOrder < pB->quitOrder)
		return false;

	if (pA->totalScore < pB->totalScore)
		return true;

	if (pA->totalScore > pB->totalScore)
		return false;

	if (pA->totalPang > pB->totalPang)
		return true;

	if (pA->totalPang < pB->totalPang)
		return false;

	return pA->order < pB->order;
}

static int s_roomCate = 4;
static int s_roomGameType = -1;

IMPLEMENT_ACTOR(CLobbyMain, CTaskMain)
BEGIN_FRESH_MSGMAP(CLobbyMain, CTaskMain)
ON_FRESH_VI("LOGIN", FRCMD_INIT, CLobbyMain::OnLogin_Init)
ON_FRESH_VV("LOGIN", FRCMD_FINISH, CLobbyMain::OnLogin_Finish)
ON_FRESH_VI("BLANK", FRCMD_INIT, CLobbyMain::OnBlank_Init)
ON_FRESH_VV("BLANK", FRCMD_FINISH, CLobbyMain::OnBlank_Finish)

ON_FRESH_VI("SECOND_PWD", FRCMD_INIT, CLobbyMain::OnSecond_Pwd_Init)
ON_FRESH_VV("SECOND_PWD", FRCMD_FINISH, CLobbyMain::OnSecond_Pwd_Finish)

ON_FRESH_VI("TOPPAGE", FRCMD_INIT, CLobbyMain::OnToppage_Init)
ON_FRESH_VV("TOPPAGE", FRCMD_FINISH, CLobbyMain::OnToppage_Finish)
ON_FRESH_VV("TOPPAGE", FRCMD_DESTROY, CLobbyMain::OnToppage_Destroy)
ON_FRESH_VI("TOPPAGE.gameplay", FRCMD_INIT, CLobbyMain::OnToppage_GameInit)
ON_FRESH_VV("TOPPAGE.gameplay", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_GameUp)
ON_FRESH_VI("TOPPAGE.family", FRCMD_INIT, CLobbyMain::OnToppage_FamilyInit)
ON_FRESH_VV("TOPPAGE.family", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_FamilyUp)
ON_FRESH_VI("TOPPAGE.pangyastudy", FRCMD_INIT,
	CLobbyMain::OnToppage_PangysStudyInit)
ON_FRESH_VV("TOPPAGE.pangyastudy", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_PangysStudyUp)

ON_FRESH_VI("TOPPAGE.notice", FRCMD_INIT, CLobbyMain::OnToppage_NoticeInit)
ON_FRESH_VV("TOPPAGE.notice", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_NoticeUp)
ON_FRESH_VI("TOPPAGE.server", FRCMD_INIT, CLobbyMain::OnToppage_ServerInit)
ON_FRESH_VV("TOPPAGE.server", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_ServerUp)
ON_FRESH_VI("TOPPAGE.pcbang", FRCMD_INIT, CLobbyMain::OnToppage_PcbangInit)
ON_FRESH_VV("TOPPAGE.pcbang", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_PcbangUp)
ON_FRESH_VI("TOPPAGE.rental_expired", FRCMD_INIT,
	CLobbyMain::OnToppage_RentalExpiredAlramInit)
ON_FRESH_VV("TOPPAGE.rental_expired", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_RentalExpiredAlramUp)
ON_FRESH_VI("TOPPAGE.event1", FRCMD_INIT, CLobbyMain::OnToppage_Event1Init)
ON_FRESH_VV("TOPPAGE.event1", FRCMD_LBUTTONDOWN, CLobbyMain::OnToppage_Event1Up)

ON_FRESH_VI("TOPPAGE.new_2", FRCMD_INIT, CLobbyMain::OnToppage_NewInit)
ON_FRESH_VI("TOPPAGE.new_2", FRCMD_OWNERDRAW,
	CLobbyMain::OnToppage_NewOwnerDraw)

ON_FRESH_VI("TOPPAGE.TopIcon1", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon1Init)
ON_FRESH_VV("TOPPAGE.TopIcon1", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon1Up)

ON_FRESH_VI("TOPPAGE.TopIcon2", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon2Init)
ON_FRESH_VV("TOPPAGE.TopIcon2", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon2Up)

ON_FRESH_VI("TOPPAGE.TopIcon3", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon3Init)
ON_FRESH_VV("TOPPAGE.TopIcon3", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon3Up)

ON_FRESH_VI("TOPPAGE.TopIcon4", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon4Init)
ON_FRESH_VV("TOPPAGE.TopIcon4", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon4Up)

ON_FRESH_VI("TOPPAGE.TopIcon5", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon5Init)
ON_FRESH_VV("TOPPAGE.TopIcon5", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon5Up)

ON_FRESH_VI("TOPPAGE.TopIcon6", FRCMD_INIT, CLobbyMain::OnToppage_TopIcon6Init)
ON_FRESH_VV("TOPPAGE.TopIcon6", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_TopIcon6Up)

ON_FRESH_VI("TOPPAGE.pointevent", FRCMD_INIT,
	CLobbyMain::OnToppage_PointEventInit)
ON_FRESH_VV("TOPPAGE.pointevent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_PointEventDown)

ON_FRESH_VI("TOPPAGE.valevent_btn", FRCMD_INIT,
	CLobbyMain::OnToppage_ValEventInit)
ON_FRESH_VV("TOPPAGE.valevent_btn", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_ValEventDown)

ON_FRESH_VI("TOPPAGE.worldtourevent", FRCMD_INIT,
	CLobbyMain::OnToppage_WorldTourEventInit)
ON_FRESH_VV("TOPPAGE.worldtourevent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_WorldTourEventDown)

ON_FRESH_VI("TOPPAGE.findebortevent", FRCMD_INIT,
	CLobbyMain::OnToppage_FindEbortEventInit)
ON_FRESH_VV("TOPPAGE.findebortevent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_FindEbortEventDown)

ON_FRESH_VI("TOPPAGE.halloween2007event", FRCMD_INIT,
	CLobbyMain::OnToppage_Halloween2007Init)
ON_FRESH_VV("TOPPAGE.halloween2007event", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_Halloween2007EvenDown)

ON_FRESH_VI("TOPPAGE.Christmas2007Event", FRCMD_INIT,
	CLobbyMain::OnToppage_Christmas2007EventInit)
ON_FRESH_VV("TOPPAGE.Christmas2007Event", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_Christmas2007EventDown)

ON_FRESH_VI("TOPPAGE.ticketexchage", FRCMD_INIT,
	CLobbyMain::OnToppage_TicketExchangeInit)
ON_FRESH_VV("TOPPAGE.ticketexchage", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_TicketExchangeDown)

ON_FRESH_VI("TOPPAGE.MissionEvent", FRCMD_INIT,
	CLobbyMain::OnToppage_MissionEventInit)
ON_FRESH_VV("TOPPAGE.MissionEvent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_MissionEventDown)

ON_FRESH_VI("TOPPAGE.BingoEvent", FRCMD_INIT,
	CLobbyMain::OnToppage_BingoEventInit)
ON_FRESH_VV("TOPPAGE.BingoEvent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_BingoEventDown)

ON_FRESH_VI("TOPPAGE.KoohBirthdayEvent", FRCMD_INIT,
	CLobbyMain::OnToppage_KoohBirthdayEventInit)
ON_FRESH_VV("TOPPAGE.KoohBirthdayEvent", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_KoohBirthdayEventDown)

ON_FRESH_VI("TOPPAGE.treasureisland", FRCMD_INIT,
	CLobbyMain::OnToppage_TreasureIslandInit)
ON_FRESH_VV("TOPPAGE.treasureisland", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnToppage_TreasureIslandLBDown)

ON_FRESH_VI("TOPPAGE.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("TOPPAGE.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("TOPPAGE.ubar.back", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("TOPPAGE.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("TOPPAGE.ubar.front", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("TOPPAGE.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("TOPPAGE.ubar.shop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("TOPPAGE.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.myroom", FRCMD_INIT, CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("TOPPAGE.ubar.myroom", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("TOPPAGE.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("TOPPAGE.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("TOPPAGE.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("TOPPAGE.ubar.gift", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("TOPPAGE.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.cookie", FRCMD_INIT, CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("TOPPAGE.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("TOPPAGE.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("TOPPAGE.ubar.game", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("TOPPAGE.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.server", FRCMD_INIT, CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("TOPPAGE.ubar.server", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("TOPPAGE.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("TOPPAGE.ubar.exit", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("TOPPAGE.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.ubar.tabbtn", FRCMD_INIT, CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("TOPPAGE.ubar.tabbtn", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("TOPPAGE.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("TOPPAGE.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("TOPPAGE.utab.magicbox", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("TOPPAGE.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("TOPPAGE.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("TOPPAGE.utab.tikimagicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("TOPPAGE.utab.scratch", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("TOPPAGE.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("TOPPAGE.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("TOPPAGE.utab.bongdarishop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("TOPPAGE.utab.ranking", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("TOPPAGE.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("TOPPAGE.utab.guild", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("TOPPAGE.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.utab.notice", FRCMD_INIT, CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("TOPPAGE.utab.notice", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("TOPPAGE.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("TOPPAGE.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("TOPPAGE.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("TOPPAGE.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("TOPPAGE.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("TOPPAGE.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("TOPPAGE.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("TOPPAGE.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("TOPPAGE.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("TOPPAGE.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("TOPPAGE.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("TOPPAGE.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("TOPPAGE.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("TOPPAGE.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("TOPPAGE.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("TOPPAGE.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("TOPPAGE.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("TOPPAGE.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("TOPPAGE.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("TOPPAGE.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("TOPPAGE.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("TOPPAGE.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("TOPPAGE.obar.onelinemsg_req", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("TOPPAGE.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("TOPPAGE.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("TOPPAGE.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("TOPPAGE.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("TOPPAGE.obar.messenger", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("TOPPAGE.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("TOPPAGE.obar.myinfo", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("TOPPAGE.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("TOPPAGE.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("TOPPAGE.obar.option", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("TOPPAGE.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("TOPPAGE.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("TOPPAGE.ubar.undertab_tooltip", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabToolTipInit)
ON_FRESH_VI("TOPPAGE.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("TOPPAGE.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("TOPPAGE.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("TOPPAGE.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("TOPPAGE.obar.messenger_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("TOPPAGE.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("TOPPAGE.obar.gift_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("TOPPAGE.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
ON_FRESH_VI("TOPPAGE.treasure_alarm", FRCMD_INIT,
	CTaskMain::OnTreasureAlarmInit)
ON_FRESH_VV("TOPPAGE.treasure_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnTreasureAlarmUp)

ON_FRESH_VI("TOPPAGE.FirstConnect", FRCMD_OWNERDRAW,
	CLobbyMain::OnFirstConnectOwnerDraw)

ON_FRESH_VV("TOPPAGE.Go_UCCShop", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_UCCShopBtnUp)
ON_FRESH_VV("TOPPAGE.Go_CardShop", FRCMD_LBUTTONUP,
	CLobbyMain::OnToppage_CardShopBtnUp)

ON_FRESH_VI("SERVERLIST", FRCMD_INIT, CLobbyMain::OnServer_Init)
ON_FRESH_VV("SERVERLIST", FRCMD_FINISH, CLobbyMain::OnServer_Finish)

ON_FRESH_VI("ROOMLIST", FRCMD_INIT, CLobbyMain::OnRoomList_Init)
ON_FRESH_VV("ROOMLIST", FRCMD_FINISH, CLobbyMain::OnRoomList_Finish)
ON_FRESH_VV("ROOMLIST", FRCMD_DESTROY, CLobbyMain::OnRoomList_Destroy)

ON_FRESH_VI("ROOMLIST.roomlist", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomListInit)
ON_FRESH_VI("ROOMLIST.roomlist", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomListOwnerDraw)
ON_FRESH_VV("ROOMLIST.roomlist", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomListLDown)
ON_FRESH_VV("ROOMLIST.roomlist", FRCMD_RBUTTONUP,
	CLobbyMain::OnRoomList_RoomListRUp)
ON_FRESH_VV("ROOMLIST.roomlist", FRCMD_DBLCLICK,
	CLobbyMain::OnRoomList_RoomListDClick)

ON_FRESH_VI("ROOMLIST.room_no", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortNoInit)
ON_FRESH_VV("ROOMLIST.room_no", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortNoDown)
ON_FRESH_VI("ROOMLIST.room_no", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortNoOwnerDraw)

ON_FRESH_VI("ROOMLIST.btn_cate_total", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateAll_BtnInit)
ON_FRESH_VV("ROOMLIST.btn_cate_total", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateAll_BtnDown)
ON_FRESH_VI("ROOMLIST.btn_cate_vs", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateVS_BtnInit)
ON_FRESH_VV("ROOMLIST.btn_cate_vs", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateVS_BtnDown)
ON_FRESH_VI("ROOMLIST.btn_cate_mass", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateMass_BtnInit)
ON_FRESH_VV("ROOMLIST.btn_cate_mass", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateMass_BtnDown)
ON_FRESH_VI("ROOMLIST.btn_cate_battle", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateBattle_BtnInit)
ON_FRESH_VV("ROOMLIST.btn_cate_battle", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateBattle_BtnDown)
ON_FRESH_VI("ROOMLIST.btn_cate_chat", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateChat_BtnInit)
ON_FRESH_VV("ROOMLIST.btn_cate_chat", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateChat_BtnDown)

ON_FRESH_VI("ROOMLIST.room_hole", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortHoleInit)
ON_FRESH_VV("ROOMLIST.room_hole", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortHoleDown)
ON_FRESH_VI("ROOMLIST.room_hole", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortHoleOwnerDraw)
ON_FRESH_VI("ROOMLIST.room_course", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCourseInit)
ON_FRESH_VV("ROOMLIST.room_course", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCourseDown)
ON_FRESH_VI("ROOMLIST.room_course", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortCourseOwnerDraw)
ON_FRESH_VI("ROOMLIST.room_state", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortStateInit)
ON_FRESH_VV("ROOMLIST.room_state", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortStateDown)
ON_FRESH_VI("ROOMLIST.room_state", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortStateOwnerDraw)

ON_FRESH_VI("ROOMLIST.cate_vs_list", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateVS_ListInit)
ON_FRESH_VV("ROOMLIST.cate_vs_list", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateVS_ListLUp)
ON_FRESH_VI("ROOMLIST.cate_vs_list", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortCateVS_ListOwnerDraw)
ON_FRESH_VI("ROOMLIST.cate_mass_list", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateMass_ListInit)
ON_FRESH_VV("ROOMLIST.cate_mass_list", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateMass_ListLUp)
ON_FRESH_VI("ROOMLIST.cate_mass_list", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortCateMass_ListOwnerDraw)
ON_FRESH_VI("ROOMLIST.cate_battle_list", FRCMD_INIT,
	CLobbyMain::OnRoomList_RoomSortCateBattle_ListInit)
ON_FRESH_VV("ROOMLIST.cate_battle_list", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_RoomSortCateBattle_ListLUp)
ON_FRESH_VI("ROOMLIST.cate_battle_list", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_RoomSortCateBattle_ListOwnerDraw)

ON_FRESH_VI("ROOMLIST.chatbg", FRCMD_INIT, CLobbyMain::OnChatBgInit)
ON_FRESH_VI("ROOMLIST.chatbg", FRCMD_OWNERDRAW, CLobbyMain::OnChatBgOwnerDraw)
ON_FRESH_VI("ROOMLIST.chatwndbtn", FRCMD_INIT, CLobbyMain::OnChatWndBtnInit)
ON_FRESH_VV("ROOMLIST.chatwndbtn", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatWndBtnUp)
ON_FRESH_VI("ROOMLIST.emoticon", FRCMD_INIT, CLobbyMain::OnEmoticonInit)
ON_FRESH_VV("ROOMLIST.emoticon", FRCMD_LBUTTONDOWN, CLobbyMain::OnEmoticonBtnUp)
ON_FRESH_VI("ROOMLIST.language", FRCMD_INIT, CLobbyMain::OnLanguageInit)
ON_FRESH_VI("ROOMLIST.chatinput", FRCMD_INIT, CLobbyMain::OnChatInputInit)
ON_FRESH_BI("ROOMLIST.chatinput", FRCMD_ENTERKEY,
	CLobbyMain::OnChatInputEnterKey)
ON_FRESH_VI("ROOMLIST.chattarget", FRCMD_INIT, CLobbyMain::OnChatTargetInit)
ON_FRESH_VI("ROOMLIST.chattarget", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatTargetBtnDown)
ON_FRESH_BI("ROOMLIST.chattarget", FRCMD_ENTERKEY,
	CLobbyMain::OnChatTargetEnterKey)
ON_FRESH_VI("ROOMLIST.chatview", FRCMD_INIT,
	CLobbyMain::OnRoomList_ChatViewInit)
ON_FRESH_VV("ROOMLIST.report", FRCMD_LBUTTONDOWN, CLobbyMain::OnReportBtnUp)

ON_FRESH_VI("ROOMLIST.makeroom", FRCMD_INIT,
	CLobbyMain::OnRoomList_MakeRoomInit)
ON_FRESH_VV("ROOMLIST.makeroom", FRCMD_LBUTTONUP,
	CLobbyMain::OnRoomList_MakeRoomUp)

ON_FRESH_VI("ROOMLIST.guildinfo", FRCMD_INIT,
	CLobbyMain::OnRoomList_GuildInfoInit)
ON_FRESH_VV("ROOMLIST.guildinfo", FRCMD_LBUTTONUP,
	CLobbyMain::OnRoomList_GuildInfoLButtonUp)

ON_FRESH_VI("ROOMLIST.userlist", FRCMD_INIT,
	CLobbyMain::OnRoomList_UserListInit)
ON_FRESH_VV("ROOMLIST.userlist", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_UserListLBtnUp)
ON_FRESH_VV("ROOMLIST.userlist", FRCMD_RBUTTONUP,
	CLobbyMain::OnRoomList_UserListRBtnUp)
ON_FRESH_VI("ROOMLIST.userlist", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_UserListOwnerDraw)

ON_FRESH_VI("ROOMLIST.user_gender", FRCMD_INIT,
	CLobbyMain::OnRoomList_UserSortGenderInit)
ON_FRESH_VV("ROOMLIST.user_gender", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_UserSortGenderUp)
ON_FRESH_VI("ROOMLIST.user_gender", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_UserSortGenderOwnerDraw)
ON_FRESH_VI("ROOMLIST.user_level", FRCMD_INIT,
	CLobbyMain::OnRoomList_UserSortLevelInit)
ON_FRESH_VV("ROOMLIST.user_level", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_UserSortLevelUp)
ON_FRESH_VI("ROOMLIST.user_level", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_UserSortLevelOwnerDraw)

ON_FRESH_VI("ROOMLIST.user_guild", FRCMD_INIT,
	CLobbyMain::OnRoomList_UserSortGuildInit)
ON_FRESH_VV("ROOMLIST.user_guild", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_UserSortGuildUp)
ON_FRESH_VI("ROOMLIST.user_guild", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_UserSortGuildOwnerDraw)
ON_FRESH_VI("ROOMLIST.user_nick", FRCMD_INIT,
	CLobbyMain::OnRoomList_UserSortNickInit)
ON_FRESH_VV("ROOMLIST.user_nick", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnRoomList_UserSortNickUp)
ON_FRESH_VI("ROOMLIST.user_nick", FRCMD_OWNERDRAW,
	CLobbyMain::OnRoomList_UserSortNickOwnerDraw)

ON_FRESH_VI("ROOMLIST.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("ROOMLIST.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("ROOMLIST.ubar.back", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("ROOMLIST.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("ROOMLIST.ubar.front", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("ROOMLIST.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("ROOMLIST.ubar.shop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("ROOMLIST.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("ROOMLIST.ubar.myroom", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("ROOMLIST.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("ROOMLIST.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("ROOMLIST.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("ROOMLIST.ubar.gift", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("ROOMLIST.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("ROOMLIST.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("ROOMLIST.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("ROOMLIST.ubar.game", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("ROOMLIST.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("ROOMLIST.ubar.server", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("ROOMLIST.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("ROOMLIST.ubar.exit", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("ROOMLIST.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("ROOMLIST.ubar.tabbtn", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("ROOMLIST.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("ROOMLIST.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("ROOMLIST.utab.magicbox", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("ROOMLIST.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("ROOMLIST.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("ROOMLIST.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("ROOMLIST.utab.scratch", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("ROOMLIST.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("ROOMLIST.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("ROOMLIST.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("ROOMLIST.utab.ranking", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("ROOMLIST.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("ROOMLIST.utab.guild", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("ROOMLIST.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("ROOMLIST.utab.notice", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("ROOMLIST.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("ROOMLIST.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("ROOMLIST.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("ROOMLIST.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("ROOMLIST.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("ROOMLIST.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("ROOMLIST.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("ROOMLIST.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("ROOMLIST.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("ROOMLIST.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("ROOMLIST.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("ROOMLIST.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("ROOMLIST.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("ROOMLIST.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("ROOMLIST.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("ROOMLIST.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("ROOMLIST.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("ROOMLIST.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("ROOMLIST.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("ROOMLIST.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("ROOMLIST.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("ROOMLIST.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("ROOMLIST.obar.onelinemsg_req", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("ROOMLIST.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("ROOMLIST.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("ROOMLIST.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("ROOMLIST.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("ROOMLIST.obar.messenger", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("ROOMLIST.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("ROOMLIST.obar.myinfo", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("ROOMLIST.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("ROOMLIST.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("ROOMLIST.obar.option", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("ROOMLIST.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("ROOMLIST.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("ROOMLIST.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("ROOMLIST.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("ROOMLIST.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("ROOMLIST.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("ROOMLIST.obar.messenger_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("ROOMLIST.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("ROOMLIST.obar.gift_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("ROOMLIST.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
ON_FRESH_VI("ROOMLIST.treasure_alarm", FRCMD_INIT,
	CTaskMain::OnTreasureAlarmInit)
ON_FRESH_VV("ROOMLIST.treasure_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnTreasureAlarmUp)

ON_FRESH_VI("GAMEROOM", FRCMD_INIT, CLobbyMain::OnGameRoom_Init)
ON_FRESH_VV("GAMEROOM", FRCMD_FINISH, CLobbyMain::OnGameRoom_Finish)
ON_FRESH_VV("GAMEROOM", FRCMD_DESTROY, CLobbyMain::OnGameRoom_Destroy)

ON_FRESH_VI("GAMEROOM.title", FRCMD_INIT, CLobbyMain::OnGameRoom_TitleInit)

ON_FRESH_VI("GAMEROOM.background", FRCMD_INIT,
	CLobbyMain::OnGameRoom_BackGroundInit)
ON_FRESH_VI("GAMEROOM.pet0", FRCMD_INIT, CLobbyMain::OnGameRoom_Pet0Init)
ON_FRESH_VI("GAMEROOM.pet1", FRCMD_INIT, CLobbyMain::OnGameRoom_Pet1Init)
ON_FRESH_VI("GAMEROOM.pet2", FRCMD_INIT, CLobbyMain::OnGameRoom_Pet2Init)
ON_FRESH_VI("GAMEROOM.pet3", FRCMD_INIT, CLobbyMain::OnGameRoom_Pet3Init)
ON_FRESH_VI("GAMEROOM.user", FRCMD_INIT, CLobbyMain::OnGameRoom_RoomUserInit)
ON_FRESH_VI("GAMEROOM.user", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_RoomUserOwnerDraw)
ON_FRESH_VV("GAMEROOM.user", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_RoomUserLBtnUp)
ON_FRESH_VV("GAMEROOM.user", FRCMD_RBUTTONUP,
	CLobbyMain::OnGameRoom_RoomUserRBtnUp)
ON_FRESH_VV("GAMEROOM.user", FRCMD_DBLCLICK,
	CLobbyMain::OnGameRoom_RoomUserDClick)

ON_FRESH_VI("GAMEROOM.chatbg", FRCMD_INIT, CLobbyMain::OnChatBgInit)
ON_FRESH_VI("GAMEROOM.chatbg", FRCMD_OWNERDRAW, CLobbyMain::OnChatBgOwnerDraw)
ON_FRESH_VI("GAMEROOM.chatwndbtn", FRCMD_INIT, CLobbyMain::OnChatWndBtnInit)
ON_FRESH_VV("GAMEROOM.chatwndbtn", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatWndBtnUp)
ON_FRESH_VI("GAMEROOM.emoticon", FRCMD_INIT, CLobbyMain::OnEmoticonInit)
ON_FRESH_VV("GAMEROOM.emoticon", FRCMD_LBUTTONDOWN, CLobbyMain::OnEmoticonBtnUp)
ON_FRESH_VI("GAMEROOM.language", FRCMD_INIT, CLobbyMain::OnLanguageInit)
ON_FRESH_VI("GAMEROOM.chatinput", FRCMD_INIT, CLobbyMain::OnChatInputInit)
ON_FRESH_BI("GAMEROOM.chatinput", FRCMD_ENTERKEY,
	CLobbyMain::OnChatInputEnterKey)
ON_FRESH_VI("GAMEROOM.chattarget", FRCMD_INIT, CLobbyMain::OnChatTargetInit)
ON_FRESH_VI("GAMEROOM.chattarget", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatTargetBtnDown)
ON_FRESH_BI("GAMEROOM.chattarget", FRCMD_ENTERKEY,
	CLobbyMain::OnChatTargetEnterKey)
ON_FRESH_VI("GAMEROOM.chatview", FRCMD_INIT,
	CLobbyMain::OnRoomList_ChatViewInit)
ON_FRESH_VV("GAMEROOM.report", FRCMD_LBUTTONDOWN, CLobbyMain::OnReportBtnUp)

ON_FRESH_VI("GAMEROOM.map_background", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapBgInit)
ON_FRESH_VI("GAMEROOM.map", FRCMD_INIT, CLobbyMain::OnGameRoom_MapInit)
ON_FRESH_VV("GAMEROOM.map", FRCMD_LBUTTONUP, CLobbyMain::OnGameRoom_MapBtnUp)
ON_FRESH_VI("GAMEROOM.mapprev", FRCMD_INIT, CLobbyMain::OnGameRoom_MapPrevInit)
ON_FRESH_VI("GAMEROOM.mapnext", FRCMD_INIT, CLobbyMain::OnGameRoom_MapNextInit)
ON_FRESH_VV("GAMEROOM.mapprev", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_MapPrevBtnUp)
ON_FRESH_VV("GAMEROOM.mapnext", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_MapNextBtnUp)
ON_FRESH_VI("GAMEROOM.mapcomboex", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapHoleComboInit)
ON_FRESH_VV("GAMEROOM.mapcomboex", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MapHoleComboDown)
ON_FRESH_VI("GAMEROOM.overgauge", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapGaugeInit)
ON_FRESH_VI("GAMEROOM.maplock", FRCMD_INIT, CLobbyMain::OnGameRoom_MapLockInit)

ON_FRESH_VI("GAMEROOM.start_tip", FRCMD_INIT,
	CLobbyMain::OnGameRoom_StartTipInit)
ON_FRESH_VI("GAMEROOM.ready_tip", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ReadyTipInit)
ON_FRESH_VI("GAMEROOM.start", FRCMD_INIT, CLobbyMain::OnGameRoom_StartInit)
ON_FRESH_VV("GAMEROOM.start", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_StartBtnUp)

ON_FRESH_VI("GAMEROOM.ready", FRCMD_INIT, CLobbyMain::OnGameRoom_ReadyInit)
ON_FRESH_VV("GAMEROOM.ready", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_ReadyBtnUp)

ON_FRESH_VI("GAMEROOM.change_team", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ChangeTeamBtnInit)
ON_FRESH_VV("GAMEROOM.change_team", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_ChangeTeamBtnUp)
ON_FRESH_VI("GAMEROOM.invite", FRCMD_INIT, CLobbyMain::OnGameRoom_UserListInit)
ON_FRESH_VV("GAMEROOM.invite", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_UserListBtnUp)
ON_FRESH_VI("GAMEROOM.modify", FRCMD_INIT,
	CLobbyMain::OnGameRoom_RoomOptionInit)
ON_FRESH_VV("GAMEROOM.modify", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_RoomOptionBtnUp)

ON_FRESH_VI("GAMEROOM.powerbar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_PowerBarInit)
ON_FRESH_VI("GAMEROOM.controlbar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ControlBarInit)
ON_FRESH_VI("GAMEROOM.accuracybar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AccuracyBarInit)
ON_FRESH_VI("GAMEROOM.spinbar", FRCMD_INIT, CLobbyMain::OnGameRoom_SpinBarInit)
ON_FRESH_VI("GAMEROOM.curvebar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CurveBarInit)
ON_FRESH_VI("GAMEROOM.epower", FRCMD_INIT, CLobbyMain::OnGameRoom_PowerEditInit)
ON_FRESH_VI("GAMEROOM.econtrol", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ControlEditInit)
ON_FRESH_VI("GAMEROOM.eaccuracy", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AccuracyEditInit)
ON_FRESH_VI("GAMEROOM.espin", FRCMD_INIT, CLobbyMain::OnGameRoom_SpinEditInit)
ON_FRESH_VI("GAMEROOM.ecurve", FRCMD_INIT, CLobbyMain::OnGameRoom_CurveEditInit)

ON_FRESH_VI("GAMEROOM.cur_char", FRCMD_INIT, CLobbyMain::OnGameRoom_CharInit)
ON_FRESH_VV("GAMEROOM.cur_char", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CharDown)
ON_FRESH_VI("GAMEROOM.cur_char", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CharOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_caddie", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CaddieInit)
ON_FRESH_VV("GAMEROOM.cur_caddie", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CaddieDown)
ON_FRESH_VI("GAMEROOM.cur_caddie", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CaddieOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_club", FRCMD_INIT, CLobbyMain::OnGameRoom_ClubInit)
ON_FRESH_VV("GAMEROOM.cur_club", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_ClubDown)
ON_FRESH_VI("GAMEROOM.cur_club", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_ClubOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_aztec", FRCMD_INIT, CLobbyMain::OnGameRoom_AztecInit)
ON_FRESH_VV("GAMEROOM.cur_aztec", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_AztecDown)
ON_FRESH_VI("GAMEROOM.cur_aztec", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_AztecOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_mascot", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MascotInit)
ON_FRESH_VV("GAMEROOM.cur_mascot", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MascotDown)
ON_FRESH_VI("GAMEROOM.cur_mascot", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_MascotOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_equip_item", FRCMD_INIT,
	CLobbyMain::OnGameRoom_EquipItemInit)
ON_FRESH_VV("GAMEROOM.cur_equip_item", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_EquipItemDown)
ON_FRESH_VI("GAMEROOM.cur_equip_item", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_EquipItemOwnerDraw)

ON_FRESH_VI("GAMEROOM.cur_char_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CharSelInit)
ON_FRESH_VV("GAMEROOM.cur_char_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CharSelDown)
ON_FRESH_VI("GAMEROOM.cur_char_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CharSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_caddie_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CaddieSelInit)
ON_FRESH_VV("GAMEROOM.cur_caddie_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CaddieSelDown)
ON_FRESH_VI("GAMEROOM.cur_caddie_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CaddieSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_club_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ClubSelInit)
ON_FRESH_VV("GAMEROOM.cur_club_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_ClubSelDown)
ON_FRESH_VI("GAMEROOM.cur_club_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_ClubSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_aztec_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AztecSelInit)
ON_FRESH_VV("GAMEROOM.cur_aztec_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_AztecSelDown)
ON_FRESH_VI("GAMEROOM.cur_aztec_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_AztecSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.cur_mascot_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MascotSelInit)
ON_FRESH_VV("GAMEROOM.cur_mascot_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MascotSelDown)
ON_FRESH_VI("GAMEROOM.cur_mascot_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_MascotSelOwnerDraw)

ON_FRESH_VI("GAMEROOM.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("GAMEROOM.ubar.back", FRCMD_INIT, CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("GAMEROOM.ubar.back", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("GAMEROOM.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.front", FRCMD_INIT, CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("GAMEROOM.ubar.front", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("GAMEROOM.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.shop", FRCMD_INIT, CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("GAMEROOM.ubar.shop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("GAMEROOM.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("GAMEROOM.ubar.myroom", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("GAMEROOM.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.myroom", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("GAMEROOM.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("GAMEROOM.ubar.itemstorage", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.gift", FRCMD_INIT, CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("GAMEROOM.ubar.gift", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("GAMEROOM.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("GAMEROOM.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("GAMEROOM.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.cookie", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.game", FRCMD_INIT, CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("GAMEROOM.ubar.game", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("GAMEROOM.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("GAMEROOM.ubar.server", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("GAMEROOM.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.server", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.exit", FRCMD_INIT, CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("GAMEROOM.ubar.exit", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("GAMEROOM.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("GAMEROOM.ubar.tabbtn", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("GAMEROOM.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("GAMEROOM.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("GAMEROOM.utab.magicbox", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("GAMEROOM.utab.magicbox", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.magicbox", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("GAMEROOM.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("GAMEROOM.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("GAMEROOM.utab.scratch", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("GAMEROOM.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.scratch", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("GAMEROOM.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("GAMEROOM.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("GAMEROOM.utab.ranking", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("GAMEROOM.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.ranking", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.utab.guild", FRCMD_INIT, CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("GAMEROOM.utab.guild", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("GAMEROOM.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("GAMEROOM.utab.notice", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("GAMEROOM.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.notice", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.utab.maket", FRCMD_INIT, CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("GAMEROOM.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("GAMEROOM.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("GAMEROOM.obar.title", FRCMD_INIT, CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("GAMEROOM.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("GAMEROOM.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("GAMEROOM.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("GAMEROOM.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.caddie", FRCMD_INIT, CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("GAMEROOM.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("GAMEROOM.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("GAMEROOM.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("GAMEROOM.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.aztec", FRCMD_INIT, CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("GAMEROOM.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("GAMEROOM.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.mascot", FRCMD_INIT, CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("GAMEROOM.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("GAMEROOM.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("GAMEROOM.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("GAMEROOM.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("GAMEROOM.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("GAMEROOM.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("GAMEROOM.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("GAMEROOM.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("GAMEROOM.obar.onelinemsg_req", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("GAMEROOM.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("GAMEROOM.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("GAMEROOM.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("GAMEROOM.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.obar.spcard", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("GAMEROOM.obar.messenger", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("GAMEROOM.obar.messenger", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.obar.messenger", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.obar.myinfo", FRCMD_INIT, CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("GAMEROOM.obar.myinfo", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("GAMEROOM.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.obar.myinfo", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM.obar.option", FRCMD_INIT, CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("GAMEROOM.obar.option", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("GAMEROOM.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM.obar.option", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("GAMEROOM.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("GAMEROOM.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("GAMEROOM.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("GAMEROOM.obar.messenger_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("GAMEROOM.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("GAMEROOM.obar.gift_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("GAMEROOM.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
ON_FRESH_VI("GAMEROOM.treasure_alarm", FRCMD_INIT,
	CTaskMain::OnTreasureAlarmInit)
ON_FRESH_VV("GAMEROOM.treasure_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnTreasureAlarmUp)

ON_FRESH_VI("GAMEROOM_EXT", FRCMD_INIT, CLobbyMain::OnGameRoomExt_Init)
ON_FRESH_VV("GAMEROOM_EXT", FRCMD_FINISH, CLobbyMain::OnGameRoomExt_Finish)
ON_FRESH_VV("GAMEROOM_EXT", FRCMD_DESTROY, CLobbyMain::OnGameRoomExt_Destroy)

ON_FRESH_VI("GAMEROOM_EXT.title", FRCMD_INIT, CLobbyMain::OnGameRoom_TitleInit)
ON_FRESH_VI("GAMEROOM_EXT.room_event", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_EventOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXT.user", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_RoomUserInit)
ON_FRESH_VI("GAMEROOM_EXT.user", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw)
ON_FRESH_VV("GAMEROOM_EXT.user", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_RoomUserLBtnUp)
ON_FRESH_VV("GAMEROOM_EXT.user", FRCMD_RBUTTONUP,
	CLobbyMain::OnGameRoom_RoomUserRBtnUp)
ON_FRESH_VV("GAMEROOM_EXT.user", FRCMD_DBLCLICK,
	CLobbyMain::OnGameRoom_RoomUserDClick)
ON_FRESH_VI("GAMEROOM_EXT.30_team1", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamTab1Init)
ON_FRESH_VI("GAMEROOM_EXT.30_team2", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamTab2Init)
ON_FRESH_VI("GAMEROOM_EXT.30_guild1", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildTab1Init)
ON_FRESH_VI("GAMEROOM_EXT.30_guild2", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildTab2Init)
ON_FRESH_VI("GAMEROOM_EXT.30_team_num", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamNumInit)
ON_FRESH_VI("GAMEROOM_EXT.30_team_num", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.30_guild_num", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildNumInit)
ON_FRESH_VI("GAMEROOM_EXT.30_guild_num", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXT.chatbg", FRCMD_INIT, CLobbyMain::OnChatBgInit)
ON_FRESH_VI("GAMEROOM_EXT.chatbg", FRCMD_OWNERDRAW,
	CLobbyMain::OnChatBgOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.chatwndbtn", FRCMD_INIT, CLobbyMain::OnChatWndBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.chatwndbtn", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatWndBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.emoticon", FRCMD_INIT, CLobbyMain::OnEmoticonInit)
ON_FRESH_VV("GAMEROOM_EXT.emoticon", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnEmoticonBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.language", FRCMD_INIT, CLobbyMain::OnLanguageInit)
ON_FRESH_VI("GAMEROOM_EXT.chatinput", FRCMD_INIT, CLobbyMain::OnChatInputInit)
ON_FRESH_BI("GAMEROOM_EXT.chatinput", FRCMD_ENTERKEY,
	CLobbyMain::OnChatInputEnterKey)
ON_FRESH_VI("GAMEROOM_EXT.chattarget", FRCMD_INIT, CLobbyMain::OnChatTargetInit)
ON_FRESH_VI("GAMEROOM_EXT.chattarget", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatTargetBtnDown)
ON_FRESH_BI("GAMEROOM_EXT.chattarget", FRCMD_ENTERKEY,
	CLobbyMain::OnChatTargetEnterKey)
ON_FRESH_VI("GAMEROOM_EXT.chatview", FRCMD_INIT,
	CLobbyMain::OnRoomList_ChatViewInit)
ON_FRESH_VV("GAMEROOM_EXT.report", FRCMD_LBUTTONDOWN, CLobbyMain::OnReportBtnUp)

ON_FRESH_VI("GAMEROOM_EXT.map_background", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapBgInit)
ON_FRESH_VI("GAMEROOM_EXT.map", FRCMD_INIT, CLobbyMain::OnGameRoom_MapInit)
ON_FRESH_VV("GAMEROOM_EXT.map", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_MapBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.mapprev", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapPrevInit)
ON_FRESH_VI("GAMEROOM_EXT.mapnext", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapNextInit)
ON_FRESH_VV("GAMEROOM_EXT.mapprev", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_MapPrevBtnUp)
ON_FRESH_VV("GAMEROOM_EXT.mapnext", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_MapNextBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.mapcomboex", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapHoleComboInit)
ON_FRESH_VV("GAMEROOM_EXT.mapcomboex", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MapHoleComboDown)

ON_FRESH_VI("GAMEROOM_EXT.overgauge", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapGaugeInit)
ON_FRESH_VI("GAMEROOM_EXT.maplock", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MapLockInit)

ON_FRESH_VI("GAMEROOM_EXT.start_tip", FRCMD_INIT,
	CLobbyMain::OnGameRoom_StartTipInit)
ON_FRESH_VI("GAMEROOM_EXT.ready_tip", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ReadyTipInit)
ON_FRESH_VI("GAMEROOM_EXT.start", FRCMD_INIT, CLobbyMain::OnGameRoom_StartInit)
ON_FRESH_VV("GAMEROOM_EXT.start", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_StartBtnUp)

ON_FRESH_VI("GAMEROOM_EXT.ready", FRCMD_INIT, CLobbyMain::OnGameRoom_ReadyInit)
ON_FRESH_VV("GAMEROOM_EXT.ready", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_ReadyBtnUp)

ON_FRESH_VI("GAMEROOM_EXT.change_team", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ChangeTeamBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.change_team", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_ChangeTeamBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.invite", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildInviteInit)
ON_FRESH_VV("GAMEROOM_EXT.invite", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoomExt_GuildInviteLBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.modify", FRCMD_INIT,
	CLobbyMain::OnGameRoom_RoomOptionInit)
ON_FRESH_VV("GAMEROOM_EXT.modify", FRCMD_LBUTTONUP,
	CLobbyMain::OnGameRoom_RoomOptionBtnUp)

ON_FRESH_VI("GAMEROOM_EXT.powerbar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_PowerBarInit)
ON_FRESH_VI("GAMEROOM_EXT.controlbar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ControlBarInit)
ON_FRESH_VI("GAMEROOM_EXT.accuracybar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AccuracyBarInit)
ON_FRESH_VI("GAMEROOM_EXT.spinbar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_SpinBarInit)
ON_FRESH_VI("GAMEROOM_EXT.curvebar", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CurveBarInit)
ON_FRESH_VI("GAMEROOM_EXT.epower", FRCMD_INIT,
	CLobbyMain::OnGameRoom_PowerEditInit)
ON_FRESH_VI("GAMEROOM_EXT.econtrol", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ControlEditInit)
ON_FRESH_VI("GAMEROOM_EXT.eaccuracy", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AccuracyEditInit)
ON_FRESH_VI("GAMEROOM_EXT.espin", FRCMD_INIT,
	CLobbyMain::OnGameRoom_SpinEditInit)
ON_FRESH_VI("GAMEROOM_EXT.ecurve", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CurveEditInit)

ON_FRESH_VI("GAMEROOM_EXT.cur_char", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CharInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_char", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CharDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_char", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CharOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_caddie", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CaddieInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_caddie", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CaddieDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_caddie", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CaddieOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_club", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ClubInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_club", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_ClubDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_club", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_ClubOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_aztec", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AztecInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_aztec", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_AztecDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_aztec", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_AztecOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_mascot", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MascotInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_mascot", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MascotDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_mascot", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_MascotOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_equip_item", FRCMD_INIT,
	CLobbyMain::OnGameRoom_EquipItemInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_equip_item", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_EquipItemDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_equip_item", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_EquipItemOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXT.cur_char_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CharSelInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_char_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CharSelDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_char_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CharSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_caddie_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_CaddieSelInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_caddie_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_CaddieSelDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_caddie_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_CaddieSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_club_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ClubSelInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_club_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_ClubSelDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_club_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_ClubSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_aztec_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_AztecSelInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_aztec_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_AztecSelDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_aztec_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_AztecSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.cur_mascot_sel", FRCMD_INIT,
	CLobbyMain::OnGameRoom_MascotSelInit)
ON_FRESH_VV("GAMEROOM_EXT.cur_mascot_sel", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_MascotSelDown)
ON_FRESH_VI("GAMEROOM_EXT.cur_mascot_sel", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_MascotSelOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXT.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("GAMEROOM_EXT.ubar.back", FRCMD_INIT,
	CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.back", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.back", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.front", FRCMD_INIT,
	CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.front", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.front", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.front", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.shop", FRCMD_INIT,
	CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.shop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.shop", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.myroom", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.myroom", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.myroom", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.itemstorage", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.gift", FRCMD_INIT,
	CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.gift", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.gift", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.cookie", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.cookie", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.game", FRCMD_INIT,
	CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.game", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.game", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.server", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.server", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.server", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.exit", FRCMD_INIT,
	CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.exit", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("GAMEROOM_EXT.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.ubar.exit", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.ubar.tabbtn", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("GAMEROOM_EXT.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("GAMEROOM_EXT.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.magicbox", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.magicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.magicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXT.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXT.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.scratch", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.scratch", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.scratch", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXT.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.ranking", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.ranking", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.ranking", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.utab.guild", FRCMD_INIT,
	CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.guild", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.guild", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.guild", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.notice", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.notice", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.notice", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXT.utab.maket", FRCMD_INIT,
	CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("GAMEROOM_EXT.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("GAMEROOM_EXT.utab.maket", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.utab.maket", FRCMD_HOVEROFF, CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXT.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("GAMEROOM_EXT.obar.title", FRCMD_INIT,
	CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.site", FRCMD_INIT, CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.char", FRCMD_INIT, CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("GAMEROOM_EXT.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.caddie", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("GAMEROOM_EXT.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.club", FRCMD_INIT, CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("GAMEROOM_EXT.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.aztec", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("GAMEROOM_EXT.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.mascot", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("GAMEROOM_EXT.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.onelinemsg_req", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.spcard", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.obar.spcard", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.messenger", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.obar.myinfo", FRCMD_INIT,
	CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.myinfo", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.myinfo", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.obar.myinfo", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.obar.option", FRCMD_INIT,
	CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.option", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.option", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXT.obar.option", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXT.countdown", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_CountdownInit)

ON_FRESH_VI("GAMEROOM_EXT.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("GAMEROOM_EXT.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.messenger_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("GAMEROOM_EXT.obar.gift_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("GAMEROOM_EXT.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXT.treasure_alarm", FRCMD_INIT,
	CTaskMain::OnTreasureAlarmInit)
ON_FRESH_VV("GAMEROOM_EXT.treasure_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnTreasureAlarmUp)

ON_FRESH_VI("GAMEROOM_EXTRES", FRCMD_INIT, CLobbyMain::OnGameRoomExtRes_Init)
ON_FRESH_VV("GAMEROOM_EXTRES", FRCMD_FINISH,
	CLobbyMain::OnGameRoomExtRes_Finish)
ON_FRESH_VV("GAMEROOM_EXTRES", FRCMD_DESTROY,
	CLobbyMain::OnGameRoomExtRes_Destroy)

ON_FRESH_VI("GAMEROOM_EXTRES.title", FRCMD_INIT,
	CLobbyMain::OnGameRoom_TitleInit)
ON_FRESH_VI("GAMEROOM_EXTRES.room_event", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoom_EventOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXTRES.user", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_RoomUserInit)
ON_FRESH_VI("GAMEROOM_EXTRES.user", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw)
ON_FRESH_VV("GAMEROOM_EXTRES.user", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_RoomUserLBtnUp)
ON_FRESH_VV("GAMEROOM_EXTRES.user", FRCMD_RBUTTONUP,
	CLobbyMain::OnGameRoom_RoomUserRBtnUp)
ON_FRESH_VV("GAMEROOM_EXTRES.user", FRCMD_DBLCLICK,
	CLobbyMain::OnGameRoom_RoomUserDClick)
ON_FRESH_VI("GAMEROOM_EXTRES.30_team1", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamTab1Init)
ON_FRESH_VI("GAMEROOM_EXTRES.30_team2", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamTab2Init)
ON_FRESH_VI("GAMEROOM_EXTRES.30_guild1", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildTab1Init)
ON_FRESH_VI("GAMEROOM_EXTRES.30_guild2", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildTab2Init)
ON_FRESH_VI("GAMEROOM_EXTRES.30_team_num", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_TeamNumInit)
ON_FRESH_VI("GAMEROOM_EXTRES.30_team_num", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.30_guild_num", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_GuildNumInit)
ON_FRESH_VI("GAMEROOM_EXTRES.30_guild_num", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.score_gb", FRCMD_INIT,
	CLobbyMain::OnGameRoomExtRes_GuildScoreGaugeInit)

ON_FRESH_VI("GAMEROOM_EXTRES.chatbg", FRCMD_INIT, CLobbyMain::OnChatBgInit)
ON_FRESH_VI("GAMEROOM_EXTRES.chatbg", FRCMD_OWNERDRAW,
	CLobbyMain::OnChatBgOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.chatwndbtn", FRCMD_INIT,
	CLobbyMain::OnChatWndBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.chatwndbtn", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatWndBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.emoticon", FRCMD_INIT, CLobbyMain::OnEmoticonInit)
ON_FRESH_VV("GAMEROOM_EXTRES.emoticon", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnEmoticonBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.language", FRCMD_INIT, CLobbyMain::OnLanguageInit)
ON_FRESH_VI("GAMEROOM_EXTRES.chatinput", FRCMD_INIT,
	CLobbyMain::OnChatInputInit)
ON_FRESH_BI("GAMEROOM_EXTRES.chatinput", FRCMD_ENTERKEY,
	CLobbyMain::OnChatInputEnterKey)
ON_FRESH_VI("GAMEROOM_EXTRES.chattarget", FRCMD_INIT,
	CLobbyMain::OnChatTargetInit)
ON_FRESH_VI("GAMEROOM_EXTRES.chattarget", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnChatTargetBtnDown)
ON_FRESH_BI("GAMEROOM_EXTRES.chattarget", FRCMD_ENTERKEY,
	CLobbyMain::OnChatTargetEnterKey)
ON_FRESH_VI("GAMEROOM_EXTRES.chatview", FRCMD_INIT,
	CLobbyMain::OnRoomList_ChatViewInit)
ON_FRESH_VV("GAMEROOM_EXTRES.report", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnReportBtnUp)

ON_FRESH_VI("GAMEROOM_EXTRES.rankframe", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_FrameInit)
ON_FRESH_VI("GAMEROOM_EXTRES.rankingtab", FRCMD_INIT,
	CLobbyMain::OnGameRoomExtRes_RoomUserRankTabInit)
ON_FRESH_VI("GAMEROOM_EXTRES.ranking", FRCMD_INIT,
	CLobbyMain::OnGameRoomExtRes_RoomUserRankInit)
ON_FRESH_VI("GAMEROOM_EXTRES.ranking", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExtRes_RoomUserRankOwnerDraw)
ON_FRESH_VV("GAMEROOM_EXTRES.ranking", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoomExtRes_RoomUserRankLBtnUp)
ON_FRESH_VV("GAMEROOM_EXTRES.ranking", FRCMD_RBUTTONUP,
	CLobbyMain::OnGameRoomExtRes_RoomUserRankRBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.guildranking", FRCMD_INIT,
	CLobbyMain::OnGameRoomExtRes_RoomGuildRankInit)
ON_FRESH_VI("GAMEROOM_EXTRES.guildranking", FRCMD_OWNERDRAW,
	CLobbyMain::OnGameRoomExtRes_RoomGuildRankOwnerDraw)
ON_FRESH_VV("GAMEROOM_EXTRES.guildranking", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoomExtRes_RoomGuildRankLBtnUp)
ON_FRESH_VV("GAMEROOM_EXTRES.guildranking", FRCMD_RBUTTONUP,
	CLobbyMain::OnGameRoomExtRes_RoomGuildRankRBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.score", FRCMD_INIT,
	CLobbyMain::OnGameRoom_ScoreInit)
ON_FRESH_VV("GAMEROOM_EXTRES.score", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoom_ScoreBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.user_info", FRCMD_INIT,
	CLobbyMain::OnGameRoomExt_UserInfoInit)
ON_FRESH_VV("GAMEROOM_EXTRES.user_info", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoomExt_UserInfoLBtnUp)

ON_FRESH_VI("GAMEROOM_EXTRES.ubar", FRCMD_INIT, CTaskMain::OnUnderBarInit)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.back", FRCMD_INIT,
	CTaskMain::OnUnderBar_BackInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.back", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BackUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.back", FRCMD_INIT,
	CLobbyMain::OnChannelRoomCloseBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.back", FRCMD_LBUTTONDOWN,
	CLobbyMain::OnGameRoomExt_ResCloseBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.back", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.back", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.front", FRCMD_INIT,
	CTaskMain::OnUnderBar_FrontInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.front", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_FrontUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.front", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.front", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.shop", FRCMD_INIT,
	CTaskMain::OnUnderBar_ShopInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.shop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ShopUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.shop", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.shop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.myroom", FRCMD_INIT,
	CTaskMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.myroom", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.myroom", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.myroom", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_INIT,
	CTaskMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.gift", FRCMD_INIT,
	CTaskMain::OnUnderBar_GiftInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.gift", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GiftUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.gift", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.gift", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.cookie", FRCMD_INIT,
	CTaskMain::OnUnderBar_CookieInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.cookie", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_CookieUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.cookie", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.cookie", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.game", FRCMD_INIT,
	CTaskMain::OnUnderBar_GameInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.game", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GameUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.game", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.game", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.server", FRCMD_INIT,
	CTaskMain::OnUnderBar_ServerInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.server", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ServerUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.server", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.server", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.exit", FRCMD_INIT,
	CTaskMain::OnUnderBar_ExitInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.exit", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ExitUp)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.exit", FRCMD_HOVERON, CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.exit", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.ubar.tabbtn", FRCMD_INIT,
	CTaskMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.ubar.tabbtn", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_TabBtnUp)

ON_FRESH_VI("GAMEROOM_EXTRES.utab", FRCMD_INIT, CTaskMain::OnUnderTab_Init)

ON_FRESH_VI("GAMEROOM_EXTRES.utab.magicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.magicbox", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.magicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.magicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_INIT,
	CTaskMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.utab.scratch", FRCMD_INIT,
	CTaskMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.scratch", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.scratch", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.scratch", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_INIT,
	CTaskMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.ranking", FRCMD_INIT,
	CTaskMain::OnUnderBar_RankingInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.ranking", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_RankingUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.ranking", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.ranking", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.guild", FRCMD_INIT,
	CTaskMain::OnUnderBar_GuildInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.guild", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_GuildUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.guild", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.guild", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.notice", FRCMD_INIT,
	CTaskMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.notice", FRCMD_LBUTTONDOWN,
	CTaskMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.notice", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.notice", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.utab.maket", FRCMD_INIT,
	CTaskMain::OnUnderBar_MaketInit)
ON_FRESH_VV("GAMEROOM_EXTRES.utab.maket", FRCMD_LBUTTONUP,
	CTaskMain::OnUnderBar_MaketUp)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.maket", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.utab.maket", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.obar", FRCMD_INIT, CTaskMain::OnOverBar_Init)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.title", FRCMD_INIT,
	CTaskMain::OnOverBar_TitleInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.site", FRCMD_INIT,
	CTaskMain::OnOverBar_SiteInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.char", FRCMD_INIT,
	CTaskMain::OnOverBar_CharInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.char", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CharDown)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.char", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.caddie", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.caddie", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_CaddieDown)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.caddie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.club", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.club", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_ClubDown)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.club", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.aztec", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.aztec", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_AztecDown)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.aztec", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.mascot", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.mascot", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MascotDown)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.mascot", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.char_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CharSelInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.char_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.caddie_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.club_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.club_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.aztec_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.mascot_sel", FRCMD_INIT,
	CTaskMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.pang_cookie", FRCMD_INIT,
	CTaskMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.onelinemsg_req", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.onelinemsg_req", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.onelineview", FRCMD_INIT,
	CTaskMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.onelineview", FRCMD_OWNERDRAW,
	CTaskMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.spcard", FRCMD_INIT,
	CTaskMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.spcard", FRCMD_LBUTTONUP,
	CTaskMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.spcard", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.spcard", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger", FRCMD_INIT,
	CTaskMain::OnOverBar_MessengerInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.messenger", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MessengerUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.myinfo", FRCMD_INIT,
	CTaskMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.myinfo", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.myinfo", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.myinfo", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.option", FRCMD_INIT,
	CTaskMain::OnOverBar_OptionInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.option", FRCMD_LBUTTONDOWN,
	CTaskMain::OnOverBar_OptionUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.option", FRCMD_HOVERON,
	CTaskMain::OnBarHoverOn)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.option", FRCMD_HOVEROFF,
	CTaskMain::OnBarHoverOff)

ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger_bubble", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTaskMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger_desc", FRCMD_INIT,
	CTaskMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.messenger_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.messenger_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.gift_alarm", FRCMD_INIT,
	CTaskMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("GAMEROOM_EXTRES.obar.gift_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("GAMEROOM_EXTRES.obar.tooltip", FRCMD_OWNERDRAW,
	CTaskMain::OnBarTooltipOwnerDraw)

ON_FRESH_VI("GAMEROOM_EXTRES.treasure_alarm", FRCMD_INIT,
	CTaskMain::OnTreasureAlarmInit)

ON_FRESH_VV("GAMEROOM_EXTRES.treasure_alarm", FRCMD_LBUTTONDOWN,
	CTaskMain::OnTreasureAlarmUp)
ON_FRESH_VI("CREATE", FRCMD_INIT, CLobbyMain::OnCreate_Init)
ON_FRESH_VV("CREATE", FRCMD_FINISH, CLobbyMain::OnCreate_Finish)

ON_FRESH_VV("CREATE", FRCMD_DESTROY, CLobbyMain::OnCreate_Destroy)
ON_FRESH_VI("CREATE.pet", FRCMD_INIT, CLobbyMain::OnCreatePetInit)
ON_FRESH_VV("CREATE.pet", FRCMD_OWNERDRAW, CLobbyMain::OnCreatePetOwnerDraw)
ON_FRESH_VI("CREATE.char", FRCMD_INIT, CLobbyMain::OnCreate_CharInit)
ON_FRESH_VI("CREATE.charprev", FRCMD_INIT, CLobbyMain::OnCreate_CharPrevInit)
ON_FRESH_VI("CREATE.charnext", FRCMD_INIT, CLobbyMain::OnCreate_CharNextInit)
ON_FRESH_VI("CREATE.hair", FRCMD_INIT, CLobbyMain::OnCreate_HairInit)
ON_FRESH_VI("CREATE.hairprev", FRCMD_INIT, CLobbyMain::OnCreate_HairPrevInit)
ON_FRESH_VV("CREATE.hairprev", FRCMD_LBUTTONUP,
	CLobbyMain::OnCreate_HairPrevBtnUp)
ON_FRESH_VI("CREATE.hairnext", FRCMD_INIT, CLobbyMain::OnCreate_HairNextInit)
ON_FRESH_VV("CREATE.hairnext", FRCMD_LBUTTONUP,
	CLobbyMain::OnCreate_HairNextBtnUp)
ON_FRESH_VI("CREATE.shirt", FRCMD_INIT, CLobbyMain::OnCreate_ShirtsInit)
ON_FRESH_VI("CREATE.shirtprev", FRCMD_INIT, CLobbyMain::OnCreate_ShirtsPrevInit)
ON_FRESH_VV("CREATE.shirtprev", FRCMD_LBUTTONUP,
	CLobbyMain::OnCreate_ShirtsPrevBtnUp)
ON_FRESH_VI("CREATE.shirtnext", FRCMD_INIT, CLobbyMain::OnCreate_ShirtsNextInit)
ON_FRESH_VV("CREATE.shirtnext", FRCMD_LBUTTONUP,
	CLobbyMain::OnCreate_ShirtsNextBtnUp)
ON_FRESH_VV("CREATE.create", FRCMD_LBUTTONUP, CLobbyMain::OnCreate_CreateBtnUp)
ON_FRESH_VI("CREATE.desc", FRCMD_INIT, CLobbyMain::OnCreate_DescInit)
ON_FRESH_VI("CREATE.warning", FRCMD_INIT, CLobbyMain::OnCreate_WarningInit)
ON_FRESH_VI("CREATE.powerbar", FRCMD_INIT, CLobbyMain::On_PowerBarInit)
ON_FRESH_VI("CREATE.controlbar", FRCMD_INIT, CLobbyMain::On_ControlBarInit)
ON_FRESH_VI("CREATE.accuratebar", FRCMD_INIT, CLobbyMain::On_AccuracyBarInit)
ON_FRESH_VI("CREATE.spinbar", FRCMD_INIT, CLobbyMain::On_SpinBarInit)

ON_FRESH_VI("CREATE.curvebar", FRCMD_INIT, CLobbyMain::On_CurveBarInit)

END_FRESH_MSGMAP()

bool FinishUserCompare(const void* a, const void* b)
{
	if (!a)
		return false;

	if (!b)
		return true;

	const sSlotInfo* pA = (const sSlotInfo*)a;
	const sSlotInfo* pB = (const sSlotInfo*)b;

	return Doc()->m_rivalList[Doc()->GetIndex(pA->dwGuid)].finishOrder <
		Doc()->m_rivalList[Doc()->GetIndex(pB->dwGuid)].finishOrder;
}

bool ConnectionUserCompare(const void* a, const void* b)
{
	if (!a)
		return false;

	if (!b)
		return true;

	const sSlotInfo* pA = (const sSlotInfo*)a;
	const sSlotInfo* pB = (const sSlotInfo*)b;

	return pA->connectionRank < pB->connectionRank;
}

CLobbyMain::CLobbyMain()
{
	Doc()->m_shopName = "";

	m_pLoginDlg = NULL;
	m_pUniteResultDlg = NULL;
	m_pTreasureGift = NULL;
	m_pReservedDlg75c = NULL;
	m_pWaitDlg = NULL;
	m_pCreateChar = NULL;
	m_pCreateDesc = NULL;
	m_pCreateHair = NULL;
	m_pCreateShirt = NULL;
	m_pPointEventDlg = NULL;
	m_pWorldTourEventDlg = NULL;
	m_pFindEbortEventDlg = NULL;
	m_pQuickStartDlg = NULL;
	m_bOverBarFlag0 = false;
	m_bOverBarFlag1 = false;
	m_bCateSelected = false;
	m_pChar = NULL;
	m_pCaddie = NULL;
	m_pAztec = NULL;
	m_pClub = NULL;
	m_pMascot = NULL;
	m_pEquipItem = NULL;
	m_pCharSel = NULL;
	m_pCaddieSel = NULL;
	m_pAztecSel = NULL;
	m_pClubSel = NULL;
	m_pMascotSel = NULL;
	m_masterOID = 0xffffffff;
	m_fKbdManualTime = -10.0f;
	m_roomPassword[0] = 0;
	m_bInitSlot = true;
	m_bRoomSortRev = false;
	m_userSort = USER_SORT_NICK;
	m_bUserSortRev = false;
	m_pTitleFont = NULL;
	m_pRoomUserRankTab = NULL;
	m_bCaddieWarning = false;
	memset(m_caddieWarning, 0, sizeof(m_caddieWarning));

	ResetControl();

	m_bLoginBlock = false;

	m_autoStartTime = 15.0f;

	m_pToppagePointEvent = NULL;

	for (int i = 0; i < 3; i++)
		m_pLoginBmp[i] = NULL;

	m_pNewBmp = NULL;

	m_reserved3b0 = 0;
	m_bUseReport = false;

	m_pToppageWorldTourEvent = NULL;

	m_reserved3cc = 0;

	m_pToppageHalloween2007Event = NULL;
	m_pHalloweenEventDlg = NULL;
	m_pHalloweenEventGiftDlg = NULL;

	m_pToppageChristmas2007Event = NULL;
	m_pChristmasEventDlg = NULL;

	m_pToppageChristmasSockEvent = NULL;
	m_pChristmasSockEventDlg = NULL;

	m_pToppageTicketExchange = NULL;
	m_pTicketExchangeDlg = NULL;

	m_pToppageMissionEvent = NULL;
	m_pMissionEventDlg = NULL;

	m_pToppageBingoEvent = NULL;
	m_pBingoEventDlg = NULL;

	m_pToppageEvent1 = NULL;

	m_reserved1e4 = 0;

	m_pQuickResultDlg = NULL;

	m_pReservedDlg490 = NULL;
	m_pValEventDlg = NULL;
	m_pCreateNickDlg = NULL;
	m_topBtnIndex = 0;
	m_event1Dir = 1.0f;

	CIconManager::Instance()->ClearInformation();
}

CLobbyMain::~CLobbyMain()
{
	for (std::map<unsigned long, sRoomSlot>::iterator it =
			 Doc()->m_roomSlotMap.begin();
		it != Doc()->m_roomSlotMap.end(); it++)
	{
		if (it->second.pExhibition)
		{
			delete it->second.pExhibition;
			it->second.pExhibition = NULL;
		}
	}

	this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
}

void CLobbyMain::OnPreLoadInit()
{
	CTaskMain::OnPreLoadInit();

	CMouseCursor::Instance()->SetAutoHide(false);
}

void CLobbyMain::OnLoad()
{
	CTaskMain::OnLoad();
}

void CLobbyMain::OnInit()
{
	g_pFresh->GetManager()->SetExclusiveKey(true);

	CTaskMain::OnInit();

	if (!m_pTreasureGift)
	{
		m_resultDelay = 1.0f;
	}

	WPolySoup::sCamera* camera =
		GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
	if (camera)
	{
		g_camera = camera->mat;
		g_view->SetFOV(camera->fov * g_DEGTORAD);
	}

	m_bgCameraDir = 1.0f;
	m_bBgCameraPause = false;
	m_bgCameraMoveTime = g_CurrentTime;

	m_cateListTime[0] = 0;
	m_cateListTime[1] = 0;
	m_cateListTime[2] = 0;

	for (int i = 0; i < 3; i++)
	{
		m_pLoginBmp[i] = (Bitmap*)g_pFresh->GetBitmap(MakeStr("login_%02d", i));
	}

	m_pNewBmp = (Bitmap*)g_pFresh->GetBitmap("tap_new2");
}

void CLobbyMain::OnDestroy()
{
	CTaskMain::OnDestroy();
}

void CLobbyMain::ProcessLobbyBgCamera(float dt)
{
	if (!Doc()->m_bBackgroundVideo)
	{
		if (m_bBgCameraPause)
		{
			if (g_CurrentTime - m_bgCameraPauseTime > 2000)
			{
				m_bBgCameraPause = false;
				m_bgCameraDir *= -1.0f;
				m_bgCameraMoveTime = g_CurrentTime;
			}
		}
		else
		{
			if (g_CurrentTime - m_bgCameraMoveTime > 80000)
			{
				m_bBgCameraPause = true;
				m_bgCameraPauseTime = g_CurrentTime;
			}
			else
			{
				float ratio = 1.0f;
				if (g_CurrentTime - m_bgCameraMoveTime < 2000)
					ratio = (g_CurrentTime - m_bgCameraMoveTime) * 0.0005f;
				else if (g_CurrentTime - m_bgCameraMoveTime > 78000)
					ratio = 1.0f -
						(g_CurrentTime - m_bgCameraMoveTime - 78000) * 0.0005f;

				g_camera.pivot +=
					g_camera.xa * m_bgCameraDir * dt * 0.3f * ratio;
			}
		}
	}
}

void CLobbyMain::ProcessAdministrator(float dt)
{
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
	{
		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") ||
			!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT"))
		{
			if (g_input->GetDown("FUNC_10", true))
			{
				if (m_selOID != 0xffffffff)
				{
					WSendPacket packet((enumClientPacket)0x4c);
					packet.Encode4(m_selOID);
					packet.Send(TO_GAME);
				}
			}
		}

		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST"))
		{
			if ((g_input->Get("LCONTROL", false) ||
					g_input->Get("RCONTROL", false)) &&
				g_input->GetDown("INSERT", true))
			{
				if (m_pSelRoom)
				{
					WSendPacket packet((enumClientPacket)0x5d);
					packet.Encode2(m_pSelRoom->roomGuid);
					packet.Send(TO_GAME);
				}
			}
		}
	}
}

void CLobbyMain::ProcessDialogKeyboard(float dt)
{
	if (m_pNoticeDlg)
	{
		if (g_input->GetDown("\xc1\xc2", false))
			m_pNoticeDlg->Prev();
		else if (g_input->GetDown("\xbf\xec", false))
			m_pNoticeDlg->Next();
	}

	if (m_pPCBangNoticeDlg)
	{
		if (g_input->GetDown("\xc1\xc2", false))
			m_pPCBangNoticeDlg->Prev();
		else if (g_input->GetDown("\xbf\xec", false))
			m_pPCBangNoticeDlg->Next();
	}

	if (m_pGuildNoticeDlg)
	{
		if (g_input->GetDown("\xc1\xc2", false))
			((FrGuildNoticeDlg*)m_pGuildNoticeDlg)->Prev();
		else if (g_input->GetDown("\xbf\xec", false))
			((FrGuildNoticeDlg*)m_pGuildNoticeDlg)->Next();
	}

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST") &&
		m_pRoomList && !m_pEmoticonDlg)
	{
		if (g_input->GetDown("\xbb\xf3", false))
			m_pRoomList->GetScrollBar()->ScrollUp(1);

		if (g_input->GetDown("\xc7\xcf", false))
			m_pRoomList->GetScrollBar()->ScrollDown(1);
	}

	if (m_pChatTarget && m_pChatInput)
	{
		if (g_input->GetDown("PAGE_UP", true))
		{
			if ((bool)((m_pChatInput->m_nFlags >> 4) & 1) &&
				Doc()->m_chatHistory.size() > 0)
			{
				std::list<std::string>::iterator it = std::find(
					Doc()->m_chatHistory.begin(), Doc()->m_chatHistory.end(),
					m_pChatInput->GetLine(1, false));
				if (it != Doc()->m_chatHistory.end())
				{
					if (it != Doc()->m_chatHistory.begin())
					{
						CChatMsg::Instance()->SetChatText((*--it).c_str(),
							false);
					}
				}
				else
				{
					CChatMsg::Instance()->SetChatText(
						Doc()->m_chatHistory.back().c_str(), false);
				}
			}
			else if ((bool)((m_pChatTarget->m_nFlags >> 4) & 1) &&
				Doc()->m_reservedChatList.size() > 0)
			{
				CChatMsg::Instance()->SetChatText(
					Doc()->GetPrevChatTarget(m_pChatTarget->GetLine(1, false)),
					false);
			}
		}
		else if (g_input->GetDown("PAGE_DOWN", true))
		{
			if ((bool)((m_pChatInput->m_nFlags >> 4) & 1) &&
				Doc()->m_chatHistory.size() > 0)
			{
				std::list<std::string>::iterator it = std::find(
					Doc()->m_chatHistory.begin(), Doc()->m_chatHistory.end(),
					m_pChatInput->GetLine(1, false));
				if (it != Doc()->m_chatHistory.end())
				{
					if (++it != Doc()->m_chatHistory.end())
						CChatMsg::Instance()->SetChatText((*it).c_str(), false);
					else
						CChatMsg::Instance()->SetChatText("", false);
				}
			}
			else if ((bool)((m_pChatTarget->m_nFlags >> 4) & 1) &&
				Doc()->m_reservedChatList.size() > 0)
			{
				CChatMsg::Instance()->SetChatText(
					Doc()->GetNextChatTarget(m_pChatTarget->GetLine(1, false)),
					false);
			}
		}

		if (!strlen(m_pChatTarget->GetLine(1, false)) ||
			!strcmp(m_pChatTarget->GetLine(1, false),
				"\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4"))
			m_pChatInput->SetFontColor(0xffffffff);
		else if (!strlen(m_pChatTarget->GetLine(1, false)) ||
			!strcmp(m_pChatTarget->GetLine(1, false),
				"\xb1\xe6\xb5\xe5\xbf\xa1\xb0\xd4"))
			m_pChatInput->SetFontColor(0xff5bffb0);
		else
			m_pChatInput->SetFontColor(0xffcf67ff);
	}
}

void CLobbyMain::ProcessLobbyPetSync(float dt)
{
	for (std::map<unsigned long, sRoomSlot>::iterator it =
			 Doc()->m_roomSlotMap.begin();
		it != Doc()->m_roomSlotMap.end(); it++)
	{
		if (it->second.pExhibition)
		{
			it->second.pExhibition->Process(dt, false);

			if (it->first == MyGuid(false) &&
				!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM"))
			{
				if (it->second.pExhibition->IsDirectionChanged())
				{
					float dir = it->second.pExhibition->GetDirection();

					WSendPacket packet((enumClientPacket)0x63);
					packet.Encode1(0);
					packet.EncodeBuffer(&dir, 4);
					packet.Send(TO_GAME);
				}

				if (it->second.pExhibition->IsNextMotion())
				{
					WSendPacket packet((enumClientPacket)0x63);
					packet.Encode1(1);
					packet.EncodeStr(
						std::string(it->second.pExhibition->GetMotionName()));
					packet.Send(TO_GAME);
				}
			}
		}
	}
}

void CLobbyMain::ProcessLobbyUI(float dt)
{
	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "LOGIN") &&
		!m_bLoginBlock)
	{
		if (m_pLoginDlg)
		{
			if ((FrLoginDlg::GetState() == (eLoginState)0 ||
					FrLoginDlg::GetState() == (eLoginState)5) &&
				timeGetTime() - g_mouse->GetLastInputTime() > 60000 &&
				timeGetTime() - g_ime->GetLastInputTime() > 60000)
			{
				m_pLoginDlg->SetFadeout(true);

				CMouseCursor::Instance()->SetActive(false);
			}
		}
		else
		{
			if ((g_mouse->IsUpdated() || g_ime->IsUpdated()) && !g_bQuit)
			{
				m_pLoginDlg = CreateForm<FrLoginDlg>(g_pFresh->GetManager(),
					this, "login", NULL);
				m_pLoginDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnLoginDlgResult, 10);
				CMouseCursor::Instance()->SetActive(true);
			}
		}
	}

	if (m_resultDelay <= 0.0f)
	{
		m_resultDelay += dt;

		bool bTimeOut = false;
		if (m_resultDelay > 0.0f)
			bTimeOut = true;

		if (g_input->GetDown("ESCAPE", true) == 1)
		{
			if (m_pUniteResultDlg)
			{
				if (m_pUniteResultDlg->GetDone())
					if (!m_pUniteResultDlg->CheckChildDlgVisible())
						bTimeOut = true;
			}
			else
			{
				bTimeOut = true;
			}
		}

		if (bTimeOut)
		{
			m_resultDelay = 1.0f;

			if (m_levelupForms.size())
			{
				for (unsigned int i = 0; i < m_levelupForms.size(); i++)
				{
					if (m_levelupForms[i])
					{
						m_levelupForms[i]->Close(FrNONE, true);
						m_levelupForms[i] = NULL;
					}
				}
				m_levelupForms.clear();
			}

			if (m_pUniteResultDlg)
				m_pUniteResultDlg->Close(false);

			if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())
				CUserInfo::Instance()->Close();

			if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(),
					"GAMEROOM_EXTRES"))
			{
				if (Doc()->m_roomInfo.realGameType != 14)
				{
					OpenLayout("GAMEROOM_EXT");
					HandleMsg(MsgObject(NULL, 0xf, 0, 0, 0, 0, 0));
					HandleMsg(MsgObject(NULL, 0x2f, 0, 0, 0, 0, 0));
				}
				else
				{
					SendRoomExitPacket();
				}
			}
		}
	}
	else if (m_pTreasureGift)
	{
		m_pTreasureGift->Close(true);
		m_pTreasureGift = NULL;
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("EMOTICON", false))
	{
		if (!CRankingInfo::Instance()->GetDlg() &&
			!CMessengerInfo::Instance()->IsVisible())
			OnEmoticonBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("REPLAY", false))
	{
		OnGameRoom_StartBtnUp();
	}

	if (m_pLanguage)
	{
		static bool s_bEnglish = true;
		if (g_ime->IsAlphaNumericMode() != s_bEnglish)
		{
			s_bEnglish = g_ime->IsAlphaNumericMode();

			if (s_bEnglish)
				m_pLanguage->SetBgImg("chat_eng");
			else
				m_pLanguage->SetBgImg("chat_kor");

			if (m_pOnelineReqDlg)
				m_pOnelineReqDlg->SetLanguage(s_bEnglish);
		}
	}

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST"))
	{
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_bOverBarFlag0 = true;

		if (((m_pRoomSortCateVSList && m_pRoomSortCateVSList->IsVisible()) ||
				(m_pRoomSortCateMassList &&
					m_pRoomSortCateMassList->IsVisible()) ||
				(m_pRoomSortCateBattleList &&
					m_pRoomSortCateBattleList->IsVisible())) &&
			m_bOverBarFlag0 && g_input->GetButton(LEFT_BUTTON) == 1)
		{
			m_bOverBarFlag0 = false;
			m_pRoomSortCateVSList->SetVisible(false);
		}
	}

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") ||
		!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT"))
	{
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_bOverBarFlag0 = true;

		if (m_bOverBarFlag1 && m_bOverBarFlag0 &&
			g_input->GetButton(LEFT_BUTTON) == 1)
		{
			m_bOverBarFlag0 = false;
			m_bOverBarFlag1 = false;
			SendChangedUserInfo();
			InitControlsSelectedUserInfo();
		}
	}

	if (m_bNewBlink)
	{
		m_newBlinkTime += dt;
		if (m_newBlinkTime > 0.2f)
		{
			m_newBlinkTime -= 0.2f;
			m_newBlinkCount++;

			if (m_pCurTip)
				m_pCurTip->SetVisible(!m_pCurTip->IsVisible());

			if (m_newBlinkCount == 3)
			{
				m_bNewBlink = false;
				m_newBlinkCount = 0;
			}
		}
	}

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
	{
		if (Doc()->m_golfGame.gameTimeLimit + Doc()->m_approachStartTime >
			g_CurrentTime)
		{
			unsigned long sec =
				(Doc()->m_golfGame.gameTimeLimit + Doc()->m_approachStartTime -
					g_CurrentTime) /
				1000;
			HandleMsg(MsgObject(this, 0x10,
				(int)MakeStr("%02d:%02d", sec / 60, sec % 60), 0, 0, 0, 0));
		}
		else
		{
			HandleMsg(MsgObject(this, 0x10, (int)"00:00", 0, 0, 0, 0));
		}
	}

	if (m_fKbdManualTime < 0.0f &&
		(!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") ||
			!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT")))
	{
		bool bChanged = false;
		if (m_bIdle)
		{
			if (g_mouse->IsUpdated() || g_ime->IsUpdated())
			{
				bChanged = true;
				m_bIdle = false;
			}

			if (timeGetTime() - g_mouse->GetLastInputTime() > 300000 &&
				timeGetTime() - g_ime->GetLastInputTime() > 300000)
			{
				std::list<sSlotInfo>::iterator it =
					std::find(Doc()->m_slotList.begin(),
						Doc()->m_slotList.end(), MyGuid(false));
				if (it != Doc()->m_slotList.end() && ((*it).bMaster))
				{
					WSendPacket packet((enumClientPacket)0xf);
					packet.Encode1(0);
					packet.Encode2(0xffff);
					packet.Encode8(0);
					packet.Encode8(0);
					packet.Send(TO_GAME);

					this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
				}
			}
		}
		else
		{
			if (timeGetTime() - g_mouse->GetLastInputTime() > 60000 &&
				timeGetTime() - g_ime->GetLastInputTime() > 60000)
			{
				m_bIdle = true;
				bChanged = true;
			}
		}

		if (bChanged)
		{
			std::list<sSlotInfo>::iterator it =
				std::find(Doc()->m_slotList.begin(), Doc()->m_slotList.end(),
					MyGuid(false));

			if (it == Doc()->m_slotList.end())
				return;

			if ((*it).bMaster)
			{
				WSendPacket packet((enumClientPacket)0xa);
				packet.Encode2(0xffff);
				packet.Encode1(1);
				packet.Encode1(9);
				packet.Encode1(m_bIdle ? 1 : 0);
				packet.Send(TO_GAME);
			}

			WSendPacket packet((enumClientPacket)0x32);
			packet.Encode1(m_bIdle ? 1 : 0);
			packet.Send(TO_GAME);
		}
	}

	if (IsLocalContent((localContentType_t)0x18) &&
		!IsLocalContent((localContentType_t)0x48))
	{
		if (m_pToppageEvent1)
		{
			if (m_pToppageEvent1->IsVisible())
			{
				WRect rect;
				rect = m_pToppageEvent1->GetRect();
				rect.y += dt * m_event1Dir * 8.0f;

				if ((rect.y > m_event1Rect.y + 15.0f && m_event1Dir > 0.0f) ||
					(rect.y < m_event1Rect.y - 15.0f && m_event1Dir < 0.0f))
					m_event1Dir *= -1.0f;

				m_pToppageEvent1->MoveWindow(WPoint(rect.x, rect.y));
			}
		}
	}

	if (Doc()->GetAppGiftDownLoadState() == 2)
	{
		if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		{
			if (!m_pTreasureGift)
			{
				unsigned int count = CTHunter::Instance()->GetGiftSize();

				if (count <= 8)
					m_resultDelay = -9.0f;
				else if (count <= 16)
					m_resultDelay = -12.0f;
				else
					m_resultDelay = -20.0f;

				m_pTreasureGift = CreateForm<FrTreasureGift>(
					g_pFresh->GetManager(), this, "trgift", NULL);
				m_pTreasureGift->SetApproachMode();
				m_pTreasureGift->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnApproachGiftDlgResult, 12);
				m_pTreasureGift->SetTimeLimit(12.0f);
				m_pTreasureGift->EnableDrag(false);
				m_pTreasureGift->SetFixed(true);
				Doc()->SetAppGiftDownloadState(
					(CSharedDoc::eAppGiftDownloadState)0);
			}
		}
	}
}

void CLobbyMain::ProcessAutoStart(float dt)
{
	sRoomInfo* pRoom = &Doc()->m_roomInfo;

	if (m_autoStartTime > 0.0f &&
		!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") &&
		(m_bCanStart || m_bAutoStart) &&
		pRoom->nUserNum >= Max((Doc()->m_curGameServer.property & 2) ? 5 : 10,
							   pRoom->nUserLimit * 3 / 4) &&
		Doc()->m_roomInfo.gameType != GAME_TYPE_GUILD_MATCH &&
		!pRoom->bAdminMaster)
	{
		m_autoStartTime -= dt;

		if (m_autoStartTime < 6.0f && m_autoStartTime + dt > 6.0f)
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xc2\xfc\xbf\xa9\xc0\xce\xbf\xf8\xc0\xcc \xc0\xfc\xbf\xf8 \xc1\xd8\xba\xf1\xbf\xcf\xb7\xe1\xb0\xa1 \xb5\xc7\xbe\xfa\xc0\xb8\xb9\xc7\xb7\xce 5\xc3\xca \xc8\xc4\xbf\xa1 \xb0\xd4\xc0\xd3\xc0\xbb \xc0\xda\xb5\xbf \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		else if (m_autoStartTime < 5.0f && m_autoStartTime + dt > 5.0f)
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xb0\xd4\xc0\xd3\xbd\xc3\xc0\xdb 5\xc3\xca\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		else if (m_autoStartTime < 4.0f && m_autoStartTime + dt > 4.0f)
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xb0\xd4\xc0\xd3\xbd\xc3\xc0\xdb 4\xc3\xca\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		else if (m_autoStartTime < 3.0f && m_autoStartTime + dt > 3.0f)
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xb0\xd4\xc0\xd3\xbd\xc3\xc0\xdb 3\xc3\xca\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		else if (m_autoStartTime < 2.0f && m_autoStartTime + dt > 2.0f)
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xb0\xd4\xc0\xd3\xbd\xc3\xc0\xdb 2\xc3\xca\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		else if (m_autoStartTime < 1.0f && m_autoStartTime + dt > 1.0f)
		{
			m_bAutoStart = false;
			this << MsgObject(NULL, 3,
				(int)"(\xbe\xcb\xb8\xb2)\xb0\xd4\xc0\xd3\xbd\xc3\xc0\xdb 1\xc3\xca\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.",
				0xffff7878, 0, 0, 0);
		}
		else if (m_autoStartTime < 0.0f && m_masterOID == MyGuid(false) &&
			Doc()->m_roomInfo.gameType != GAME_TYPE_GUILD_MATCH)
			OnGameRoom_StartBtnUp();
	}
	else
	{
		if (Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
			m_autoStartTime = 6.0f;
		else
			m_autoStartTime = 15.0f;
	}
}

void CLobbyMain::OnProcess(float dt)
{
	CTaskMain::OnProcess(dt);

	ProcessLobbyBgCamera(dt);
	ProcessLobbyPetSync(dt);

	if (IsLocalContent(S3_GM_TOOLKIT))
		ProcessAdministrator(dt);

	ProcessDialogKeyboard(dt);
	ProcessLobbyUI(dt);
	ProcessAutoStart(dt);
	ResetControlsRoomListSort(dt);

	if (IsLocalContent((localContentType_t)0x4c) &&
		g_pFresh->IsCurrentLayout("TOPPAGE"))
		AnimateWebEventButton(dt);

	if (IsLocalContent(S4_UCC))
	{
		RefreshAllUccClothes();
	}
}

void CLobbyMain::AnimateWebEventButton(float dt)
{
	static float s_time;
	static int s_index;
	static int s_frame;
	s_time += dt;
	if (s_time > 0.3f)
	{
		s_time = 0.0f;
		int frames[4] = { 1, 2, 3, 2 };

		s_index++;
		if (s_index > 3)
			s_index = 0;

		s_frame = frames[s_index];
		char buf[4] = { 0 };
		std::string name = "xmas_main_i";
		name += itoa(s_frame, buf, 10);

		if (m_pToppageChristmas2007Event)
		{
			m_pToppageChristmas2007Event->SetButtonImg(name.c_str(),
				FrButton::NORMAL);
			m_pToppageChristmas2007Event->SetButtonImg(name.c_str(),
				FrButton::OVER);
		}
	}
}

void CLobbyMain::OnDisplay()
{
}

void CLobbyMain::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x5b:
	{
		std::map<unsigned long, sRoomSlot>::iterator it =
			Doc()->m_roomSlotMap.find(msg.param1);
		if (it == Doc()->m_roomSlotMap.end())
			return;

		if (Doc()->m_roomSlotMap[msg.param1].pExhibition == NULL)
			return;

		switch (msg.param2)
		{
		case 0:
			Doc()->m_roomSlotMap[msg.param1].pExhibition->SetDirection(
				*(float*)msg.param3);
			break;

		case 1:
			Doc()->m_roomSlotMap[msg.param1].pExhibition->SetMotion(
				(const char*)msg.param3, false, 0.0f);
			break;
		}
	}
	break;

	case 0x59:
	{
		std::map<unsigned long, sRoomSlot>::iterator it;
		for (it = Doc()->m_roomSlotMap.begin();
			it != Doc()->m_roomSlotMap.end(); ++it)
		{
			if (it->second.pExhibition)
			{
				delete it->second.pExhibition;
				it->second.pExhibition = NULL;
			}
		}
		Doc()->m_roomSlotMap.clear();

		for (int i = 0; i < 4; i++)
		{
			if (m_pPet[i])
				m_pPet[i]->SetMouseEvent(false);
		}
	}
	break;

	case 0x5a:
	{
		sSlotInfo* pSlot = (sSlotInfo*)msg.param1;
		sCharacterInfo* pCharInfo = (sCharacterInfo*)msg.param2;

		sRoomSlot slot;
		slot.pArea = m_pPet[pSlot->connectionRank - 1];
		slot.pExhibition = NULL;
		slot.charInfo = *pCharInfo;

		slot.bAngelWing = pSlot->angelicWings;
		slot.bGachaWing = pSlot->angelicWingsEffect;

		Doc()->m_roomSlotMap[pSlot->dwGuid] = slot;
		ShowPet(pSlot->dwGuid);
	}
	break;

	case 0x4f:
	{
		sFriend* pFriend = BuddyMGR->GetBuddy(m_selUID);

		if (pFriend == NULL)
			return;

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		pForm->SetMessage(MakeStr("%s\xb4\xd4\xc0\xbb \xc4\xa3\xb1\xb8 "
								  "\xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad "
								  "\xbb\xe8\xc1\xa6\xc7\xcf\xbd\xc3\xb0\xda"
								  "\xbd\xc0\xb4\xcf\xb1\xee?",
							  pFriend->NickName),
			false);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnRemoveFriendDlgResult, 0);
	}
	break;

	case 0x44:
		OpenEventPrizeForm((unsigned char)msg.param1);
		break;

	case 0:
	{
		if (g_pFresh == NULL)
			return;

		const char* layout = (const char*)msg.param1;

		if (g_pFresh->IsCurrentLayout(layout))
			return;

		CloseAllDialogs();
		ResetControl();

		if (!stricmp(layout, "TOPPAGE"))
		{
			CIconManager::Instance()->ClearInformation();

			AfxGetTask()->SetWhisper(false);
		}
		else
		{
			AfxGetTask()->SetWhisper(true);
		}

		g_pFresh->OpenLayout(layout, this, false);
		CMouseCursor::Instance()->Toggle(true);

		CheckTikiReport(layout);
	}
	break;

	case 3:
	{
		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") ||
			!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") ||
			!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
		{
			sChatLine line;
			line.text = (const char*)msg.param1;
			line.color = msg.param2;

			Doc()->m_chatLineList.push_back(line);
		}

		if (m_pChatView)
			m_pChatView->AddLine((const char*)msg.param1, msg.param2, false);
	}
	break;

	case 0x60:
	{
		if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM"))
			return;

		std::string nick = (const char*)msg.param1;
		std::string chat = (const char*)msg.param2;

		int face = Doc()->m_chatManager.GetFaceIndex(chat.c_str());
		if (face == 5)
			break;

		if (m_pRoomUser)
		{
			for (std::list<FrListItem*>::iterator it =
					 m_pRoomUser->m_itemList.begin();
				it != m_pRoomUser->m_itemList.end(); ++it)
			{
				sSlotInfo* pSlot = (sSlotInfo*)(*it)->pData;

				if (pSlot)
				{
					if (!strcmp(pSlot->sNick, nick.c_str()))
					{
						if (Doc()->m_roomSlotMap[pSlot->dwGuid].pExhibition)
						{
							Doc()
								->m_roomSlotMap[pSlot->dwGuid]
								.pExhibition->SetChatFacial(face,
									pSlot->tidChar);
						}
						break;
					}
				}
			}
		}
	}
	break;

	case 0xa:
	{
		sBriefUserInfo* pUser = (sBriefUserInfo*)msg.param1;

		if (pUser->dwGuid == MyGuid(false) &&
			(Doc()->m_myInfo.info.dwIdentity & 4))
		{
			if (pUser->state & 0x400)
				return;
		}

		if (pUser->dwGuid != MyGuid(false) &&
			!Doc()->m_myInfo.info.IsIdentity(4) && (pUser->dwIdentity & 0x14))
		{
			if (!(pUser->state & 1))
				return;
		}

		if (m_pUserList)
		{
			m_pUserList->AddItem(pUser);
			RoomList_SortUserList(m_userSort);
		}

		if (m_pUserListDlg)
		{
			m_pUserListDlg->AddUser((void*)msg.param1);
			m_pUserListDlg->SortUserList();
		}
	}
	break;

	case 0xc:
	{
		sBriefUserInfo* pUser = (sBriefUserInfo*)msg.param1;

		if (pUser->dwGuid == MyGuid(false) &&
			(Doc()->m_myInfo.info.dwIdentity & 4))
		{
			if (pUser->state & 0x400)
				return;
		}

		if (m_pUserList)
			m_pUserList->DelItem(pUser, msg.param2);

		if (m_pUserListDlg)
			m_pUserListDlg->DelUser((void*)msg.param1, msg.param2);

		if (pUser->dwGuid == m_selOID)
		{
			m_selOID = 0xffffffff;
			m_selUID = 0xffffffff;

			if (m_pUserInfo)
				m_pUserInfo->Enable(false);
		}
	}
	break;

	case 0xb:
		if (m_pUserList)
			m_pUserList->ClearItem();

		m_selOID = 0xffffffff;
		m_selUID = 0xffffffff;
		break;

	case 4:
	{
		if (m_pRoomList == NULL)
			return;

		sRoomInfo* pRoom = (sRoomInfo*)msg.param1;
		if (s_roomGameType != -1)
		{
			if (pRoom->gameType == s_roomGameType)
				m_pRoomList->AddItem(pRoom);
		}
		else if (GetCateByGameType(pRoom->gameType) == s_roomCate ||
			s_roomCate == 4)
			m_pRoomList->AddItem(pRoom);
	}
	break;

	case 5:
	{
		if (m_pRoomList == NULL)
			return;

		sRoomInfo* pRoom = (sRoomInfo*)msg.param1;
		if (s_roomGameType != -1)
		{
			if (pRoom->gameType == s_roomGameType)
			{
				m_pRoomList->DelItem(pRoom, msg.param2);
				for (int i = m_pRoomList->GetCurrentItemSize(true); i < 6; i++)
					m_pRoomList->AddItem(NULL);
			}
		}
		else if (GetCateByGameType(pRoom->gameType) == s_roomCate ||
			s_roomCate == 4)
		{
			m_pRoomList->DelItem(pRoom, msg.param2);
			for (int i = m_pRoomList->GetCurrentItemSize(true); i < 6; i++)
				m_pRoomList->AddItem(NULL);
		}
	}
	break;

	case 6:
		if (m_pRoomList)
			m_pRoomList->ClearItem();
		break;

	case 0xd:
	{
		if (m_pRoomList == NULL)
			return;

		int count = m_pRoomList->GetCurrentItemSize(true);
		if (count <= 6)
		{
			for (int i = count; i < 6; i++)
				m_pRoomList->AddItem(NULL);
		}
		else
		{
			int del = count - 6;
			std::list<FrListItem*>::iterator it =
				m_pRoomList->m_itemList.begin();
			while (it != m_pRoomList->m_itemList.end() && del > 0)
			{
				if ((*it)->pData == NULL)
				{
					del--;
					it = m_pRoomList->_DelItem(it);
				}
				else
				{
					++it;
				}

				if (del < 0)
					break;
			}
		}

		RoomList_SortRoomList(m_roomSort, 0);
	}
	break;

	case 7:
	{
		if (Doc()->m_roomInfo.gameType == 5 || Doc()->m_roomInfo.gameType == 6)
		{
			if (Doc()->m_bGameOver)
			{
				sSlotInfo* pSlot = (sSlotInfo*)msg.param1;

				if (pSlot->bTeam == 0)
					m_redTeam.push_back(pSlot);
				else
					m_blueTeam.push_back(pSlot);

				RemakeTeam(false);
			}
			else
			{
				RemakeTeam(true);
			}
		}
		else
		{
			if (m_pRoomUser)
			{
				m_pRoomUser->AddItem((void*)msg.param1);
				m_pRoomUser->SortItem(ConnectionUserCompare);
			}
		}

		if (Doc()->m_roomInfo.gameType == 4 || Doc()->m_roomInfo.gameType == 10)
		{
			if (m_pRoomUser == NULL)
				return;

			if (m_pRoomUser->GetCurrentItemSize(true) <= 24)
				return;

			for (std::list<FrListItem*>::iterator it =
					 m_pRoomUser->m_itemList.begin();
				it != m_pRoomUser->m_itemList.end(); ++it)
			{
				if ((*it)->pData == NULL)
				{
					m_pRoomUser->_DelItem(it);
					break;
				}
			}

			m_pRoomUser->SortItem(ConnectionUserCompare);
		}
		else if (IsMassGame(Doc()->m_roomInfo.gameType))
		{
			return;
		}
		else
		{
			sSlotInfo* pSlot = (sSlotInfo*)msg.param1;

			sRoomSlot slot;
			slot.pArea = m_pPet[pSlot->connectionRank - 1];
			slot.pExhibition = NULL;
			slot.charInfo = *(sCharacterInfo*)msg.param2;

			slot.bAngelWing = pSlot->angelicWings;
			slot.bGachaWing = pSlot->angelicWingsEffect;

			Doc()->m_roomSlotMap[pSlot->dwGuid] = slot;

			if (IsLocalContent(S4_INVITE_FRIEND))
			{
				if (!pSlot->IsInvite)
					ShowPet(pSlot->dwGuid);
			}
			else
			{
				ShowPet(pSlot->dwGuid);
			}

			if (m_pRoomUser == NULL)
				return;

			if (m_pRoomUser->GetCurrentItemSize(true) <= 4)
				return;

			for (std::list<FrListItem*>::iterator it =
					 m_pRoomUser->m_itemList.begin();
				it != m_pRoomUser->m_itemList.end(); ++it)
			{
				if ((*it)->pData == NULL)
				{
					m_pRoomUser->_DelItem(it);
					break;
				}
			}

			m_pRoomUser->SortItem(ConnectionUserCompare);
		}
	}
	break;

	case 8:
	{
		unsigned long guid = msg.param2;
		int team = msg.param3;

		if (Doc()->m_roomInfo.gameType == 5 || Doc()->m_roomInfo.gameType == 6)
		{
			if (Doc()->m_bGameOver)
			{
				std::list<sSlotInfo*>::iterator it;
				if (team == 0)
				{
					for (it = m_redTeam.begin(); it != m_redTeam.end(); ++it)
					{
						if ((*it)->dwGuid == guid)
						{
							m_redTeam.erase(it);
							break;
						}
					}
				}
				else
				{
					for (it = m_blueTeam.begin(); it != m_blueTeam.end(); ++it)
					{
						if ((*it)->dwGuid == guid)
						{
							m_blueTeam.erase(it);
							break;
						}
					}
				}

				RemakeTeam(false);
			}
			else
			{
				RemakeTeam(true);
			}
		}
		else
		{
			if (m_pRoomUser && msg.param1)
				m_pRoomUser->DelItem((void*)msg.param1);
		}

		if (Doc()->m_roomInfo.gameType == 4 || Doc()->m_roomInfo.gameType == 10)
		{
			if (m_pRoomUser)
			{
				if (m_pRoomUser->GetCurrentItemSize(true) < 24)
					m_pRoomUser->AddItem(NULL);
			}
		}
		else if (!IsMassGame(Doc()->m_roomInfo.gameType))
		{
			HidePet(guid);

			std::map<unsigned long, sRoomSlot>::iterator it =
				Doc()->m_roomSlotMap.find(guid);
			if (it != Doc()->m_roomSlotMap.end())
			{
				if (it->second.pArea)
					it->second.pArea->SetMouseEvent(false);
				Doc()->m_roomSlotMap.erase(it);
			}

			std::list<sSlotInfo>::iterator itSlot;
			for (it = Doc()->m_roomSlotMap.begin();
				it != Doc()->m_roomSlotMap.end(); ++it)
			{
				itSlot = std::find(Doc()->m_slotList.begin(),
					Doc()->m_slotList.end(), it->first);

				if (itSlot != Doc()->m_slotList.end())
				{
					it->second.pArea = m_pPet[itSlot->connectionRank - 1];

					if (it->second.pExhibition)
					{
						it->second.pExhibition->SetCenter(
							it->second.pArea->GetRect().x +
								it->second.pArea->GetRect().w * 0.5f,
							it->second.pArea->GetRect().y +
								it->second.pArea->GetRect().h * 0.5f);

						if (it->second.bGachaWing)
							it->second.pExhibition->AttachAngelWingFx();
					}
				}
			}

			if (m_pRoomUser)
			{
				if (m_pRoomUser->GetCurrentItemSize(true) < 4)
					m_pRoomUser->AddItem(NULL);
			}
		}

		if (guid == m_selOID)
		{
			m_selOID = 0xffffffff;
			m_selUID = 0xffffffff;
		}
	}
	break;

	case 9:
	{
		sSlotInfo* pSlot = (sSlotInfo*)msg.param1;

		std::map<unsigned long, sRoomSlot>::iterator itRoom =
			Doc()->m_roomSlotMap.find(pSlot->dwGuid);

		if (itRoom != Doc()->m_roomSlotMap.end())
		{
			itRoom->second.bGachaWing = pSlot->angelicWingsEffect;
			itRoom->second.bAngelWing = pSlot->angelicWings;
		}

		if (m_pRoomUser == NULL)
			return;

		for (std::list<FrListItem*>::iterator it =
				 m_pRoomUser->m_itemList.begin();
			it != m_pRoomUser->m_itemList.end(); ++it)
		{
			FrListItem* pItem = *it;
			sSlotInfo* pInfo = (sSlotInfo*)pItem->pData;
			if (pInfo)
			{
				if (pSlot->dwGuid == pInfo->dwGuid)
				{
					pInfo->angelicWings = pSlot->angelicWings;
					pInfo->angelicWingsEffect = pSlot->angelicWingsEffect;
					return;
				}
			}
		}
	}
	break;

	case 0xe:
	{
		if (Doc()->m_roomInfo.nUserNum > 1)
		{
			m_bNewBlink = true;
			m_newBlinkTime = -10.0f;
			m_pCurTip = m_pReadyTip;
		}
		else
		{
			m_pCurTip = m_pStartTip;
		}

		MakeRoomUserList();
		this << MsgObject(NULL, 0xf, 0, 0, 0, 0, 0);
	}
	break;

	case 0xf:
	{
		CSharedDoc* pDoc = Doc();
		std::list<sSlotInfo>& slotList = pDoc->m_slotList;
		bool bMaster = false;

		if (pDoc->m_myInfo.info.IsIdentity(4))
		{
			bMaster = true;
			m_masterOID = MyGuid(false);
		}
		else
		{
			std::list<sSlotInfo>::iterator it;
			for (it = pDoc->m_slotList2.begin(); it != pDoc->m_slotList2.end();
				++it)
			{
				sSlotInfo slot = *it;
				if (slot.bMaster)
				{
					m_masterOID = slot.dwGuid;

					if (m_masterOID == MyGuid(false))
						bMaster = true;
					break;
				}
			}

			for (it = slotList.begin(); it != slotList.end(); ++it)
			{
				if (it->bMaster)
				{
					m_masterOID = it->dwGuid;

					if (it->dwGuid == MyGuid(false))
						bMaster = true;
					break;
				}
			}
		}

		if (bMaster)
		{
			m_bReady = false;
			m_bRoomStateReq = false;
			m_pCurTip = m_pStartTip;
		}

		if (m_pMap)
		{
			if (Doc()->m_roomInfo.realGameType == 14)
			{
				m_pMap->Enable(false);
			}
			else
			{
				m_pMap->Enable(bMaster);
			}
		}

		SetStartBtn(bMaster);
		SetRoomControls(bMaster);

		if (m_pUnused65c)
			m_pUnused65c->Enable(bMaster && Doc()->m_roomInfo.gameType != 6);
	}
	break;

	case 0x71:
		RemakeTeam(true);
		break;

	case 0x10:
	{
		if (m_pTitle == NULL)
			return;

		char szTime[32];
		if (msg.param1)
		{
			strcpy(szTime, (const char*)msg.param1);
		}
		else if (IsMassGame(Doc()->m_roomInfo.gameType))
		{
			if (Doc()->m_roomInfo.gameType == 10)
			{
				sprintf(szTime, "%d\xc3\xca",
					Doc()->m_roomInfo.gameTimeLimit / 1000);
			}

			if (Doc()->m_roomInfo.gameType == 6)
			{
				sprintf(szTime, "");
			}
			else
			{
				sprintf(szTime, "%d\xba\xd0",
					Doc()->m_roomInfo.gameTimeLimit / 60000);
			}
		}
		else
		{
			if (Doc()->m_roomInfo.shotTimeLimit == 0)
				sprintf(szTime,
					"\xbd\xc3\xb0\xa3\xc1\xa6\xc7\xd1\xbe\xf8\xc0\xbd");
			else
				sprintf(szTime, "%d\xc3\xca",
					Doc()->m_roomInfo.shotTimeLimit / 1000);
		}

		const char* name;
		if (!IsMassGame(Doc()->m_roomInfo.gameType) ||
			Doc()->m_roomInfo.gameType == 10)
		{
			name = GetGameTypeName(Doc()->m_roomInfo.gameType);
		}
		else
		{
			IFF_STRUCT::sMatch* pMatch =
				ItemManager()->FindMatch(Doc()->m_roomInfo.tidMatch);
			if (pMatch == NULL)
				name = GetGameTypeName(Doc()->m_roomInfo.gameType);
			else if (Doc()->m_roomInfo.realGameType != 14)
				name = pMatch->Name;
			else
				name = "\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba";
		}

		m_pTitle->SetLine(1,
			MakeStr("[%d]   %s %s    %s %s  %2d\xc8\xa6  (%d/%d)  %s",
				Doc()->m_roomInfo.roomGuid,
				Doc()->m_roomInfo.bPublic ? "" : "[\xba\xf1]",
				Doc()->m_roomInfo.title, name,
				Doc()->m_roomInfo.gameType == 5 ? "\xc6\xc0\xc0\xfc" : "",
				Doc()->m_roomInfo.nHole, Doc()->m_roomInfo.nUserNum,
				Doc()->m_roomInfo.nUserLimit, szTime),
			0, false, 0);
	}
	break;

	case 0x11:
	{
		unsigned char course = (unsigned char)msg.param1;

		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
			0x28000000 | (course >= 0x7f ? 0x7f : course));
		if (pCourse)
		{
			if (m_pMap)
				m_pMap->SetButtonImg(MakeStr("big_%s", pCourse->c.Icon),
					FrButton::NORMAL);

			if (m_pBackGround)
			{
				m_pBackGround->Open(
					MakeStr("background%s.tga", strstr(pCourse->c.Icon, "_")));
			}

			if (m_pMapBg)
			{
				m_pMapBg->Open("map_background.tga");
			}
		}

		if (m_pMapGauge)
		{
			m_pMapGauge->ResetGauge();
			if (course < 0x7f)
			{
				unsigned long gauge = CTHunter::Instance()->GetTHunterGauge(
					(eMapType)Doc()->m_roomInfo.mapType);
				m_pMapGauge->SetPos(gauge < 701 ? 0 : gauge - 700);
			}
			else
			{
				m_pMapGauge->SetPos(0);
			}
		}

		std::list<sSlotInfo>::iterator it = std::find(Doc()->m_slotList.begin(),
			Doc()->m_slotList.end(), MyGuid(false));

		if (it == Doc()->m_slotList.end())
			return;

		SetRoomControls(it->bMaster || Doc()->m_myInfo.info.IsIdentity(4));
	}
	break;

	case 0x12:
	{
		ReLoadHoleItem();

		int sel = msg.param1;
		if (m_pMapHoleCombo)
		{
			if (Doc()->m_roomInfo.nHole == 18)
			{
				if (sel > 0)
					sel = 1;
			}
			m_pMapHoleCombo->SelectItem(sel);
		}
	}
	break;

	case 0x16:
		m_bCreateRoom = msg.param1 != 0;
		break;

	case 0x17:
		*(int*)msg.param1 = m_bCreateRoom;
		break;

	case 0x18:
	{
		for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
			it != Doc()->m_slotList.end(); ++it)
			it->bSleep = 0;

		Doc()->m_bGameOver = false;

		CMouseCursor::Instance()->SetMode(1);
		CMouseCursor::Instance()->SetActive(false);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
	}
	break;

	case 0x1e:
		if (m_pLoginDlg)
		{
			FrLoginDlg::SetState((eLoginState)msg.param1);
			return;
		}

		if (msg.param1 == 13)
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"CREATE",
				0, 0, 0);
		break;

	case 0x1f:
		if (m_pServerDlg)
			m_pServerDlg->SetState((gsLoginState_t)msg.param1);
		break;

	case 0x42:
	{
		_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
			pTheme->GetRoomListBackGroundImgName(), true);

		THEME_BUTTONS buttons;
		if (m_pMakeRoom)
		{
			pTheme->GetMakeRoomButtonImgName(buttons);
			m_pMakeRoom->SetButtonImg(buttons.press, FrButton::PRESSED);
			m_pMakeRoom->SetButtonImg(buttons.normal, FrButton::NORMAL);
			m_pMakeRoom->SetButtonImg(buttons.over, FrButton::OVER);
		}

		if (m_pGuildInfo)
		{
			pTheme->GetQuickStartButtonImgName(buttons);
			m_pGuildInfo->SetButtonImg(buttons.press, FrButton::PRESSED);
			m_pGuildInfo->SetButtonImg(buttons.normal, FrButton::NORMAL);
			m_pGuildInfo->SetButtonImg(buttons.over, FrButton::OVER);
		}

		if (m_pOverBar)
			S5::THEME::ChangeOverbarTheme(S5::THEME::GetCurrentSkinnedTheme(),
				m_pOverBar);
	}
	break;

	case 0x30:
	{
		if (!IsLocalContent(S4_INVITE_FRIEND))
			return;

		if (m_pInviteDlg || Doc()->m_myInfo.roomIndex != 0xffff ||
			!COption::Instance()->gIsInvitationAllowed())
			return;

		m_pInviteDlg =
			CreateForm<FrInviteDlg>(g_pFresh->GetManager(), this, "invite");
		m_pInviteDlg->SetInvaiteInfo(msg.param1, (unsigned char)msg.param2,
			(unsigned short)msg.param3, msg.param4, MyUID());
		std::string* pName = (std::string*)msg.param5;
		m_pInviteDlg->SetMessage(
			MakeStr(
				"(%s)\xb4\xd4\xb2\xb2\xbc\xad \xb4\xe7\xbd\xc5\xc0\xbb "
				"\xc3\xca\xb4\xeb\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.\n"
				"\xc3\xca\xb4\xeb\xbf\xa1 \xc0\xc0\xc7\xcf\xbd\xc3\xb0\xda"
				"\xbd\xc0\xb4\xcf\xb1\xee?",
				pName->c_str(), 0),
			false);
		m_pInviteDlg->SetTimeLimit(5.0f);
		m_pInviteDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnInviteDlgResult, 1);
	}
	break;

	case 0x31:
	{
		if (MyGuid(false) == m_masterOID || MyGuid(false) == msg.param2)
			return;

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "banish_vote");
		pForm->SetMessage((const char*)msg.param1, false);
		pForm->SetTimeLimit(5.0f);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnVoteDlgResult, 1);
	}
	break;

	case 0x23:
	{
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
			(FrCmdTarget*)this, "notify");
		pForm->SetMessage((const char*)msg.param1, false);
		pForm->SetTimeLimit(5.0f);
		pForm->Open(NULL, 3);

		pForm->SetTopmost(true);
		pForm->FindNextTopFocus(false);
	}
	break;

	case 0x25:
	{
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
			(FrCmdTarget*)this, "notify");
		pForm->SetMessage((const char*)msg.param1, false);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnNotifyQuitDlgResult, 3);
	}
	break;

	case 0x27:
		strcpy(m_roomPassword, (const char*)msg.param1);
		break;

	case 0x28:
		*(char**)msg.param1 = m_roomPassword;
		break;

	case 0x29:
	{
		unsigned long guid = msg.param1;
		int state = msg.param2;

		if (state == 2)
		{
			m_bRoomStateReq = false;
			this << MsgObject(this, 0x23,
				(int)"\xc1\xf6\xb1\xdd\xc0\xba \xb7\xb9\xb5\xf0\xc7\xd2 \xbc\xf6 "
					 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 "
					 "\xb4\xd9\xbd\xc3 \xb7\xb9\xb5\xf0\xc7\xd8\xc1\xd6\xbc\xbc"
					 "\xbf\xe4.",
				0, 0, 0, 0);
			return;
		}

		if (guid == MyGuid(false) || guid == 0xffffffff)
		{
			m_bRoomStateReq = false;
			m_bReady = state == 0;
			m_bNewBlink = state == 1;
			m_newBlinkTime = -10.0f;

			if (Doc()->m_roomInfo.gameType == 1 ||
				Doc()->m_roomInfo.gameType == 5)
			{
				if (m_pChangeTeam)
					m_pChangeTeam->Enable(!m_bReady);
			}

			if (m_pRoomUser)
				m_pRoomUser->UseRightButton(!m_bReady);

			if (m_pUserListDlg)
				m_pUserListDlg->EnableRightButton(!m_bReady);
		}

		std::list<sSlotInfo>& slotList = Doc()->m_slotList;
		std::list<sSlotInfo>::iterator it;
		if (guid == 0xffffffff)
		{
			for (it = slotList.begin(); it != slotList.end(); ++it)
				it->bReady = state == 0;
		}
		else
		{
			it = std::find(slotList.begin(), slotList.end(), guid);
			if (it != slotList.end())
				it->bReady = state == 0;
		}

		bool bMaster = false;
		for (it = slotList.begin(); it != slotList.end(); ++it)
		{
			if (it->bMaster)
			{
				if (it->dwGuid == MyGuid(false))
					bMaster = true;
				break;
			}
		}

		if (Doc()->m_myInfo.info.IsIdentity(4))
			bMaster = true;

		SetStartBtn(bMaster);
		SetRoomControls(bMaster);
	}
	break;

	case 0x46:
	{
		std::list<sSlotInfo>::iterator it = std::find(Doc()->m_slotList.begin(),
			Doc()->m_slotList.end(), msg.param1);
		if (it == Doc()->m_slotList.end())
			return;

		if (m_pChangeTeam)
			m_pChangeTeam->SetButtonImg(it->bTeam == 0 ? "btn_teamchange_blue_n"
													   : "btn_teamchange_red_n",
				FrButton::NORMAL);

		if (Doc()->m_roomInfo.gameType != 5)
			return;

		std::list<sSlotInfo*>::iterator itTeam;
		if (msg.param2 == 0)
		{
			m_redTeam.push_back(&(*it));
			for (itTeam = m_blueTeam.begin(); itTeam != m_blueTeam.end();
				++itTeam)
			{
				if ((*itTeam)->dwGuid == it->dwGuid)
				{
					m_blueTeam.erase(itTeam);
					break;
				}
			}
		}
		else
		{
			m_blueTeam.push_back(&(*it));
			for (itTeam = m_redTeam.begin(); itTeam != m_redTeam.end();
				++itTeam)
			{
				if ((*itTeam)->dwGuid == it->dwGuid)
				{
					m_redTeam.erase(itTeam);
					break;
				}
			}
		}

		RemakeTeam(false);
	}
	break;

	case 0x49:
	{
		unsigned long guid = msg.param1;
		sCharacterInfo* pCharInfo = (sCharacterInfo*)msg.param2;

		if (guid == MyGuid(true))
		{
			Doc()->m_myInfo.userEquip.guidChar = pCharInfo->guid;
			Doc()->BuildMyPartTidList(*pCharInfo);
			SetLevelBar();
		}

		std::list<sSlotInfo>::iterator it;
		for (it = Doc()->m_slotList.begin(); it != Doc()->m_slotList.end();
			++it)
		{
			if (it->dwGuid != guid)
				continue;

			sRoomSlot& slot = Doc()->m_roomSlotMap[guid];

			it->tidChar = pCharInfo->tid;

			slot.charInfo = *pCharInfo;

			if (guid == MyGuid(true))
			{
				if (!slot.partTidList.SetTids(&slot.charInfo, 0xff) ||
					!slot.partTidList.IsComboValid())
				{
					slot.partTidList.m_charTid = slot.charInfo.tid;
					slot.partTidList.SetDefaultTids();
				}

				if (slot.pExhibition)
					slot.pExhibition->SetModel(slot.partTidList,
						slot.charInfo.tidAuxParts, NULL, 3.0f, 0);

				if (slot.pExhibition && slot.bGachaWing)
					slot.pExhibition->AttachAngelWingFx();

				SetLevelBar();
				SetRoomCharControl((eBtnDir)2);
			}
			else
			{
				ShowPet(guid);
			}
			break;
		}
	}
	break;

	case 0x4a:
	{
		unsigned long guid = msg.param1;
		if (guid != MyGuid(true))
			return;

		Doc()->m_userInfo[0].caddieInfo = *(sCaddieInfo*)msg.param2;
		Doc()->m_myInfo.userEquip.guidCaddie =
			Doc()->m_userInfo[0].caddieInfo.guid;
		SetLevelBar();
	}
	break;

	case 0x4b:
	{
		unsigned long guid = msg.param1;
		if (guid != MyGuid(true))
			return;

		Doc()->m_userInfo[0].clubInfo = *(sClubInfo*)msg.param2;
		Doc()->m_myInfo.userEquip.guidClubSet =
			Doc()->m_userInfo[0].clubInfo.guid;
		SetLevelBar();
	}
	break;

	case 0x4c:
	{
		unsigned long guid = msg.param1;
		if (guid != MyGuid(true))
			return;

		Doc()->m_myInfo.userEquip.tidBall = msg.param2;
		Doc()->m_userInfo[0].userEquip.tidBall = msg.param2;
		SetLevelBar();
	}
	break;

	case 0x4e:
	{
		unsigned long guid = msg.param1;
		if (guid != MyGuid(true))
			return;

		Doc()->m_userInfo[0].mascotInfo = *(sMascotInfo*)msg.param2;
		Doc()->m_myInfo.userEquip.guidMascot =
			Doc()->m_userInfo[0].mascotInfo.guid;
		SetLevelBar();
	}
	break;

	case 0x2b:
		m_bLockControls = msg.param1 == 1;

		if (m_pStart)
			m_pStart->Enable(!m_bLockControls);

		if (m_pRoomClose)
			m_pRoomClose->Enable(!m_bLockControls);

		m_bNewBlink = !m_bLockControls;

		if (m_pCurTip)
			m_pCurTip->SetVisible(!m_bLockControls);
		break;

	case 0x2c:
		Doc()->m_underBarMask |= msg.param1;

		if (Doc()->m_underBarMask == 0x1f)
			Doc()->SortAllMyItemList();
		break;

	case 0x2a:
		SortRank();
		break;

	case 0x2d:
	{
		if (!Doc()->m_bGameOver)
			return;

		switch (Doc()->m_roomInfo.gameType)
		{
		case 4:
		{
			SortRank();

			if (m_pExtFrame)
				m_pExtFrame->SetVisible(false);
			if (m_pRoomUserRankTab)
				m_pRoomUserRankTab->SetVisible(false);
			if (m_pRoomUserRank)
				m_pRoomUserRank->SetVisible(false);
			if (m_pRoomGuildRank)
				m_pRoomGuildRank->SetVisible(false);
			if (m_pScore)
				m_pScore->SetVisible(false);
			if (m_pUserInfo)
				m_pUserInfo->SetVisible(false);

			if (m_pUniteResultDlg == NULL)
			{
				m_pUniteResultDlg = CreateForm<FrUniteResultDlg>(
					g_pFresh->GetManager(), this, "uniteresult");
				m_pUniteResultDlg->EnableDrag(false);
				m_pUniteResultDlg->SetFixed(true);
				m_pUniteResultDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnResultDlgResult,
					WPoint(68.0f, 48.0f), 0);
			}

			m_resultDelay = -20.0f;
		}
		break;

		case 10:
		{
			if (m_pRoomUser)
			{
				unsigned char rank = 1;
				for (std::list<FrListItem*>::iterator it =
						 m_pRoomUser->m_itemList.begin();
					it != m_pRoomUser->m_itemList.end(); ++it, rank++)
				{
					sSlotInfo* pSlot = (sSlotInfo*)(*it)->pData;
					if (pSlot)
						pSlot->connectionRank = rank;
				}
			}

			Doc()->m_bGameOver = false;
		}
			return;

		default:
		{
			SortRank();

			if (m_pUniteResultDlg == NULL && Doc()->m_golfGame.gameType != 10)
			{
				m_pUniteResultDlg = CreateForm<FrUniteResultDlg>(
					g_pFresh->GetManager(), this, "uniteresult");
				m_pUniteResultDlg->EnableDrag(false);
				m_pUniteResultDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnResultDlgResult,
					WPoint(68.0f, 48.0f), 0);
			}

			m_resultDelay = -20.0f;
		}
		break;
		}
	}
	break;

	case 0x12d:
	{
		FrLevelupItemForm* pForm = CreateForm<FrLevelupItemForm>(
			g_pFresh->GetManager(), (FrCmdTarget*)this, "levelupitem");

		if (pForm == NULL)
			return;

		pForm->SetData(*(sLevelUpDone*)msg.param1);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnLevelupFormResult, 3);

		m_levelupForms.push_back(pForm);
	}
	break;

	case 0x2e:
		m_bInitSlot = false;
		break;

	case 0x32:
		if (m_pUserListDlg)
			m_pUserListDlg->MakeUserList(Doc()->m_briefUserInfoMap);
		break;

	case 0x33:
	{
		if (msg.param1 == 1)
		{
			this << MsgObject(this, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 "
					 "\xbe\xca\xb4\xc2 \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			return;
		}

		EnableRoomEnterControls(false);

		if (m_pDtRoomDlg || !m_pRoomList || !m_pRoomList->GetSelected())
			return;

		FrDtRoomDlg* pDlg = CreateForm<FrDtRoomDlg>(g_pFresh->GetManager(),
			this, "detailroominfo");
		m_pDtRoomDlg = pDlg;

		pDlg->SetRoomInfo(*(sRoomInfo*)m_pRoomList->GetSelected()->pData);

		m_pDtRoomDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnDtRoomDlgResult, 1);
	}
	break;

	case 0x34:
		memcpy(Doc()->m_myInfo.userEquip.tidItemSlot, (void*)msg.param1,
			sizeof(Doc()->m_myInfo.userEquip.tidItemSlot));
		break;

	case 0x35:
		if (IsLocalContent(S3_PARAN_CHANNELING))
		{
			OpenParanNickChange();
			return;
		}
		OpenCaddieWarningForm();
		break;

	case 0x37:
	{
		if (Doc()->AddWhisperPartner(m_pChatTarget, (const char*)msg.param1))
			m_pChatTarget->SetLine(1, (const char*)msg.param1, 0, false, 0);

		if (CUserInfo::Instance()->IsVisible())
			CUserInfo::Instance()->Close();

		if (m_pUserListDlg)
			m_pUserListDlg->Close(true);

		if (m_pChatInput)
			m_pChatInput->SetKeyFocus(true);
	}
	break;

	case 0x38:
		Doc()->AddWhisperPartner(m_pChatTarget, (const char*)msg.param1);
		break;

	case 0x47:
		Doc()->InitRecentWhisperList(m_pChatTarget);
		Doc()->InitReservedChatList(m_pChatTarget, (eReservedChatPartner)0);
		break;

	case 0x3a:
	{
		if (!(Doc()->m_curChannel.Type & 2))
			return;

		std::list<sChannelInfo>::iterator it = Doc()->m_channelList.begin();

		for (; it != Doc()->m_channelList.end(); ++it)
		{
		}
	}
	break;

	case 0x2f:
	{
		bool bTeam =
			Doc()->m_roomInfo.gameType == 1 || Doc()->m_roomInfo.gameType == 5;
		bool bGuild = Doc()->m_roomInfo.gameType == 6;

		std::list<sSlotInfo>::iterator it;

		if (!GalleryMode())
		{
			it = std::find(Doc()->m_slotList.begin(), Doc()->m_slotList.end(),
				MyGuid(false));
			if (it == Doc()->m_slotList.end())
				return;
		}
		else
		{
			bTeam = false;
		}

		if (m_pChangeTeam)
			m_pChangeTeam->Enable(bTeam);

		if (m_pGuildTab[0])
			m_pGuildTab[0]->SetVisible(bGuild);
		if (m_pGuildTab[1])
			m_pGuildTab[1]->SetVisible(bGuild);

		if (m_pTeamTab[0])
		{
			if (bTeam)
				m_pTeamTab[0]->SetBgImg("bar_redteam");
			else
				m_pTeamTab[0]->SetBgImg("bar_base");
		}

		if (m_pTeamTab[1])
			m_pTeamTab[1]->SetVisible(bTeam);

		if (!bTeam)
			return;

		if (m_pChangeTeam)
			m_pChangeTeam->SetButtonImg(it->bTeam == 0 ? "btn_teamchange_blue_n"
													   : "btn_teamchange_red_n",
				FrButton::NORMAL);
	}
	break;

	case 0x3b:
	{
		switch ((unsigned char)msg.param1)
		{
		case 2:
			this << MsgObject(NULL, 0x23,
				(int)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa "
					 "\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 3:
			this << MsgObject(NULL, 0x23,
				(int)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 "
					 "\xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 4:
			this << MsgObject(NULL, 0x23,
				(int)"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 "
					 "\xc6\xb2\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 7:
		case 9:
			this << MsgObject(NULL, 0x23,
				(int)"\xb9\xe6\xb8\xb8\xb5\xe9\xb1\xe2 \xbf\xe4\xc3\xbb\xc0\xcc "
					 "\xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 8:
			this << MsgObject(NULL, 0x23,
				(int)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce "
					 "\xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 5:
			this << MsgObject(NULL, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xb7\xb9\xba\xa7\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe9\xb0\xc5\xb3\xaa \xb9\xe6\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;

		case 13:
			this << MsgObject(NULL, 0x23,
				(int)"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xd8\xbe\xdf "
					 "\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;

		case 18:
			this << MsgObject(NULL, 0x23,
				(int)"\xc8\xaf\xbb\xf3\xc0\xc7 \xc6\xce\xbe\xdf\xbc\xb6\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6 \xb0\xb3\xbc\xb3\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;

		case 12:
		{
			if (Doc()->m_curChannel.Type & 0x80)
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
					(FrCmdTarget*)this, "notify_yesno");
				pForm->SetMessage(
					"\xc7\xf6\xc0\xe7 \xc0\xfb\xb4\xe7\xc7\xd1 \xb8\xc5\xc4\xa1 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\n \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe9\xb0\xed \xb1\xe2\xb4\xd9\xb8\xae\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					false);
				pForm->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnMatchRoomCreateDlgResult,
					0);
				return;
			}

			if ((Doc()->m_curChannel.Type & 0x400) &&
				Doc()->m_myInfo.stat.Level < 11 &&
				!Doc()->m_myInfo.info.IsIdentity(4))
			{
				this << MsgObject(NULL, 0x23,
					(int)"\xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb9\xe6\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return;
			}

			WSendPacket packet((enumClientPacket)8);
			packet.Encode1(0);

			unsigned char gameType = 0x10;
			unsigned char map = FirstMap();

			if (Doc()->m_curChannel.Type & 2)
			{
				packet.Encode4(0);
				packet.Encode4(2400000);

				packet.Encode1(Doc()->m_gameTypeInfo[4].maxPlayer);

				packet.Encode1(4);
				packet.Encode1(18);
			}
			else if (Doc()->m_curChannel.Type & 0x80)
			{
				packet.Encode4(60000);
				packet.Encode4(0);

				packet.Encode1(Doc()->m_gameTypeInfo[3].maxPlayer);

				packet.Encode1(3);
				packet.Encode1(9);
			}
			else if (Doc()->m_curChannel.Type & 0x4000)
			{
				packet.Encode4(40000);
				packet.Encode4(0);

				packet.Encode1(Doc()->m_gameTypeInfo[7].maxPlayer);

				packet.Encode1(7);
				packet.Encode1(6);
			}
			else
			{
				gameType = GetRandGameTypeByCate(m_quickStartCate);

				packet.Encode4(GetShotTimeLimitByGameType(gameType));
				packet.Encode4(GetGameTimeLimitByGameType(gameType));

				packet.Encode1(GetGameTypeInfo(gameType)->maxPlayer);

				packet.Encode1(gameType);
				packet.Encode1(GetGameTypeInfo(gameType)->holes);

				if (gameType == 10)
					map = 0x7f;
			}

			packet.Encode1(map);
			packet.Encode1(gameType == 10 ? 2 : 0);
			packet.EncodeStr(std::string(Doc()->GetDefaultTitle(gameType)));
			packet.EncodeStr(std::string(""));
			packet.Send(TO_GAME);

			this << MsgObject(NULL, 0x16, 1, 0, 0, 0, 0);
		}
			return;

		case 15:
			this << MsgObject(NULL, 0x23,
				(int)"\xc6\xce\xb9\xe8\xc6\xb2\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;

		case 17:
			this << MsgObject(NULL, 0x23,
				(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xb0\xd4\xc0\xd3\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;
		}

		EnableRoomEnterControls(true);
	}
	break;

	case 0x3d:
	{
		std::list<sSlotInfo>::iterator it = std::find(Doc()->m_slotList.begin(),
			Doc()->m_slotList.end(), msg.param1);
		if (it == Doc()->m_slotList.end())
			return;

		it->bSleep = msg.param2 == 1;
	}
	break;

	case 0x3e:
		OpenCreateInfo(msg.param1, (const char*)msg.param2);
		break;

	case 0x3f:
		*(const char**)msg.param1 =
			m_pChatInput ? m_pChatInput->GetLine(1, false) : NULL;
		break;

	case 0x40:
		if (IsLocalContent(S3_PARAN_CHANNELING))
		{
			if (m_pParanNickDlg)
			{
				if (msg.param1 == 0)
					m_pParanNickDlg->CloseDlg();
				else
					m_pParanNickDlg->EnableControls(true);
			}
		}

		if (m_pOptionDlg)
		{
			if (msg.param1 == 0)
			{
				m_pOptionDlg->CloseDlg();
				return;
			}
			m_pOptionDlg->EnableChangeNickControls(true);
		}
		break;

	case 0x51:
	{
		FrUserInfoForm* pForm = USERINFODLG();
		if (pForm == NULL)
			return;

		if (CUserInfo::Instance()->IsVisible() == true)
		{
			if (pForm->IsOpenAddFriendDlg())
			{
				pForm->SetRequestFriendData((const char*)msg.param1,
					msg.param2);
			}
		}

		FrUserListDlg* pDlg = m_pUserListDlg;
		if (pDlg == NULL)
			return;

		if (!pDlg->IsOpenAddFriendDlg())
			return;

		pDlg->SetRequestFriendData((const char*)msg.param1, msg.param2);
	}
	break;

	case 0x52:
	{
		FrUserInfoForm* pForm = USERINFODLG();
		if (pForm)
			return;

		if (CUserInfo::Instance()->IsVisible() == true)
		{
			if (pForm->IsOpenAddFriendDlg())
			{
				pForm->SetRequestFriendData((const char*)msg.param1,
					msg.param2);
			}
		}
	}
	break;

	case 0x53:
		OpenNewRecordForm();
		break;

	case 0x56:
		Doc()->m_bTutorialResume = false;
		CloseAllDialogs();

		CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
		AfxPostMsg(NULL, "TutorialMain", 0, (int)"TUTORIAL_MAIN", 0, 0, 0);
		break;

	case 0x58:
		m_bAutoStart = true;
		break;

	case 0x5c:
	{
		FrCaddieInfoDlg* pDlg = (FrCaddieInfoDlg*)g_pFresh->GetManager()
									->GetDesktop()
									->FindChildByName("caddie_info");
		if (pDlg == NULL)
		{
			__rtti_obj = NULL;
			return;
		}

		pDlg->EndContract(msg.param1);
	}
	break;

	case 0x5e:
	{
		unsigned long guid = msg.param1;

		if (Doc()->m_teamPlayerGuid[0] != guid &&
			Doc()->m_teamPlayerGuid[1] != guid)
			return;

		sRivalData* pRival1 = GUILDMATCHUP(0);
		sRivalData* pRival2 = GUILDMATCHUP(1);

		if (pRival1 == NULL || pRival2 == NULL)
			return;

		int hole = msg.param2;

		if (pRival1->holeStroke[hole - 1] == 0 ||
			pRival2->holeStroke[hole - 1] == 0)
			return;

		char szMsg[1024];
		if (pRival1->holeStroke[hole - 1] < pRival2->holeStroke[hole - 1])
			sprintf(szMsg,
				"%s\xb4\xd4\xc0\xcc %d\xc8\xa6\xbf\xa1\xbc\xad \xbd\xc2\xb8\xae\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
				pRival1->nickname, hole);
		else if (pRival1->holeStroke[hole - 1] > pRival2->holeStroke[hole - 1])
			sprintf(szMsg,
				"%s\xb4\xd4\xc0\xba %d\xc8\xa6\xbf\xa1\xbc\xad \xc6\xd0\xb9\xe8\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9",
				pRival1->nickname, hole);
		else
			sprintf(szMsg,
				"%d\xc8\xa6\xc0\xba \xb9\xab\xbd\xc2\xba\xce\xb0\xa1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				hole);

		this << MsgObject(NULL, 3, (int)szMsg, 0xffff7878, 0, 0, 0);
	}
	break;

	case 0x5f:
		m_masterOID = msg.param1;
		break;

	case 0xb3:
	{
		if (!Doc()->m_bBingoEvent)
			return;

		char szUrl[1024];
		sprintf(szUrl, "www.pangya.com/pang_event_bingo.asp?userid=%s", MyId());

		CBrowser::Instance()->Open(szUrl, false, false);
		Doc()->m_bBingoEvent = false;
	}
	break;

	case 0x61:
	{
		m_bCaddieWarning = true;
		int count = msg.param1;
		memset(m_caddieWarning, 0, sizeof(sCaddieInfo));
		memcpy(m_caddieWarning, (void*)msg.param2, count * sizeof(sCaddieInfo));
	}
	break;

	case 0x1f7:
		GameExtResQuit();
		break;

	case 0x201:
	{
		if (!IsLocalContent((localContentType_t)0x15))
			return;

		if (m_pPointEventDlg == NULL)
			m_pPointEventDlg = CreateForm<FrPointEventDlg>(
				g_pFresh->GetManager(), this, "point_event_dlg");

		if (m_pPointEventDlg == NULL)
			return;

		m_pPointEventDlg->SetMyPoint(Doc()->m_myInfo.info.dwPointPointEvent);
		m_pPointEventDlg->SetIconRect(m_pToppagePointEvent->GetRect());
		m_pPointEventDlg->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnPointEventDlgResult, 0x11);
		m_pPointEventDlg->SetDesc(
			"\xc7\xd8\xb4\xe7 \xc6\xf7\xc0\xce\xc6\xae\xb8\xa6 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb0\xed \xc0\xcc\xba\xa5\xc6\xae\xbf\xa1 \xc2\xfc\xbf\xa9\xc7\xcf\xbc\xbc\xbf\xe4.");
	}
	break;

	case 0x21c:
	{
		if (!IsLocalContent(S4_MATCHING_SYSTEM))
			return;

		std::string name = *(std::string*)msg.param1;
		const char* mode = GetGameTypeText(*(unsigned char*)msg.param2);
		unsigned long uid = *(unsigned long*)msg.param3;

		if (m_pInviteDlg == NULL && Doc()->m_userInfoMod == 0)
		{
			m_pInviteDlg =
				CreateForm<FrInviteDlg>(g_pFresh->GetManager(), this, "invite");
			m_pInviteDlg->SetInvaiteInfo(0, 0, 0, uid, MyUID());
			m_pInviteDlg->SetMessage(
				MakeStr(
					"(%s)\xb4\xd4\xb2\xb2\xbc\xad %s\xb8\xf0\xb5\xe5 \xb8\xc5\xc4\xaa \xbf\xe4\xc3\xbb\xc0\xbb\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.\n\xb8\xc5\xc4\xaa\xbf\xa1 \xc0\xc0\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					name.c_str(), mode),
				false);
			m_pInviteDlg->SetTimeLimit(5.0f);
			m_pInviteDlg->Open(
				(FRESH_PFN_RESULT)&CLobbyMain::OnMatchingDlgResult, 1);
		}
		else
		{
			WSendPacket packet((enumClientPacket)0xb8);
			packet.Encode4(uid);
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}
	}
	break;

	case 0x21d:
	{
		if (!IsLocalContent(S4_MATCHING_SYSTEM))
			return;

		if (m_pQuickResultDlg)
		{
			int result = msg.param1;
			m_pQuickResultDlg->SetContents((const char*)msg.param2);

			if (result == 0)
				m_pQuickResultDlg->SetOkbtnEnable(true);
		}

		Doc()->m_userInfoMod = 0;
	}
	break;

	case 0x230:
		if (m_pUserListDlg)
			m_pUserListDlg->CloseInviteDelayDlg();
		break;

	case 0x240:
	{
		if (!IsLocalContent((localContentType_t)0x72))
			return;

		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(),
			(FrCmdTarget*)this, "notify");
		WRect rect = pForm->GetRect();
		pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
		pForm->SetTimeLimit(12.0f);
		pForm->SetMessage(
			"-\\c0xffff0000\\c\xb0\xe6\xc7\xb0 \xc0\xcc\xba\xa5\xc6\xae\\c0xff000000\\c\xbf\xa1 \xc0\xda\xb5\xbf\xc0\xb8\xb7\xce \xc0\xc0\xb8\xf0\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
			false);
		pForm->Open(NULL, 0);
	}
	break;

	case 0x248:
	{
		if (IsLocalContent((localContentType_t)0x78) != true)
			return;

		if (m_pValEventDlg == NULL)
			m_pValEventDlg = CreateForm<FrValEventDlg>(g_pFresh->GetManager(),
				this, "valentine_event_s4");

		if (m_pValEventDlg == NULL)
			return;

		m_pValEventDlg->SetGiftFlag(msg.param1);
		m_pValEventDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnValEventDlgResult,
			0x19);
	}
	break;

	case 0x249:
		if (msg.param1 == 1)
		{
			if (m_pCreateNickDlg == NULL)
			{
				m_pCreateNickDlg = CreateForm<FrCreateNickDlg>(
					g_pFresh->GetManager(), this, "createnick");
				if (m_pCreateNickDlg)
					m_pCreateNickDlg->Open(
						(FRESH_PFN_RESULT)&CLobbyMain::OnCreateNickResult, 0);
			}
			else
			{
				m_pCreateNickDlg->SetNick((const char*)msg.param2);
			}
		}
		else if (msg.param1 == 0)
		{
			if (m_pCreateNickDlg)
			{
				strcpy(MyNick(), (const char*)msg.param2);
				m_pCreateNickDlg->CloseDlg(1);
				m_pCreateNickDlg = NULL;
			}
		}
		else if (msg.param1 == 3)
		{
			if (!IsLocalContent(S4_NT_NICKNAME_CHANGE))
				return;

			if (m_pCreateNickDlg)
				return;

			m_pCreateNickDlg = CreateForm<FrCreateNickDlg>(
				g_pFresh->GetManager(), this, "createnick");
			if (m_pCreateNickDlg)
			{
				m_pCreateNickDlg->SetNick(MyNick());
				m_pCreateNickDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnCreateNickResult, 0);
			}
		}
		break;

	case 0x6d:
		if (m_pTicketExchangeDlg)
			m_pTicketExchangeDlg->Reponse(msg.param1,
				(unsigned char)msg.param2);
		break;

	case 0x6e:
		if (!IsLocalContent(S4_NT_NICKNAME_CHANGE))
			return;

		if (msg.param1 == 0)
		{
			if (m_pCreateNickDlg)
				m_pCreateNickDlg->ReceiveCheckCode(msg.param2);
		}
		break;

	case 0x6f:
		if (!IsLocalContent(S4_NT_EVENT_MISSION))
			return;

		if (msg.param1 == 0)
		{
			m_pMissionEventDlg = CreateForm<FrMissionEventDlg>(
				g_pFresh->GetManager(), (FrCmdTarget*)this, "mission_event");
			if (m_pMissionEventDlg)
			{
				m_pMissionEventDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnMissionEventResult, 1);
				m_pMissionEventDlg->EnableDrag(false);
			}
		}
		else if (msg.param1 == 1)
		{
			const char* text = NULL;
			switch (msg.param2)
			{
			case 0:
				text =
					"\xbb\xf3\xc7\xb0\xc0\xcc \xbf\xec\xc6\xed\xc0\xb8\xb7\xce \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbf\xec\xc6\xed\xc7\xd4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
				break;
			case 1:
				text =
					"\xc0\xcc\xb9\xcc \xb9\xde\xc0\xb8\xbc\xcc\xb3\xd7\xbf\xe4.";
				break;
			case 2:
				text =
					"\xbb\xf3\xc7\xb0\xc0\xcc \xc0\xfc\xb4\xde\xc0\xcc \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xbd\xc5\xc3\xbb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.";
				break;
			}

			this << MsgObject(NULL, 0x23, (int)text, 0, 0, 0, 0);
		}
		break;

	case 0x70:
		if (!IsLocalContent(S4_NT_EVENT_BINGO))
			return;

		if (msg.param1 == 0)
		{
			m_pBingoEventDlg = CreateForm<FrBingoEventDlg>(
				g_pFresh->GetManager(), (FrCmdTarget*)this, "bingo_event");
			if (m_pBingoEventDlg)
			{
				m_pBingoEventDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnBingoEventResult, 1);
				m_pBingoEventDlg->EnableDrag(false);

				m_pBingoEventDlg->SetBingoData(
					(GlobalEnum::sBingoInfo*)msg.param2);
			}
		}
		else if (msg.param1 == 1)
		{
			if (m_pBingoEventDlg)
			{
				m_pBingoEventDlg->SetBingoData(
					(GlobalEnum::sBingoInfo*)msg.param2);
				m_pBingoEventDlg->SetSelectedData(msg.param3, msg.param4,
					*(unsigned int*)msg.param5);
			}
		}
		else if (msg.param1 == 2)
		{
			if (m_pBingoEventDlg)
			{
				unsigned int* pCount = (unsigned int*)msg.param3;
				m_pBingoEventDlg->SetGiftData(
					(std::vector<unsigned long>*)msg.param2, *pCount);
			}
		}
		break;

	case 0x250:
	{
		if (!IsLocalContent((localContentType_t)0x8e))
			return;

		std::string skin = "BirthdayEventSkin1";
		if (IsLocalContent((localContentType_t)0x9b))
			skin = "BirthdayEventSkin2";

		m_pBirthdayEventDlg = CreateForm<FrBirthdayEventDlg>(
			g_pFresh->GetManager(), (FrCmdTarget*)this, skin.c_str());
		if (m_pBirthdayEventDlg)
		{
			unsigned long* pMission = (unsigned long*)msg.param1;
			unsigned long* pGift = (unsigned long*)msg.param2;

			std::string eventName = "Kooh";
			if (IsLocalContent((localContentType_t)0x94))
				eventName = "Hana";
			else if (IsLocalContent((localContentType_t)0x9b))
				eventName = "Arin";

			m_pBirthdayEventDlg->SetEventName(eventName);
			m_pBirthdayEventDlg->SetMissionData(*pMission, *pGift);

			std::vector<unsigned long>* pMissionList =
				(std::vector<unsigned long>*)msg.param3;
			std::vector<unsigned long>* pGiftList =
				(std::vector<unsigned long>*)msg.param4;
			m_pBirthdayEventDlg->SetMissionInfo(*pMissionList, *pGiftList);
			m_pBirthdayEventDlg->Open(
				(FRESH_PFN_RESULT)&CLobbyMain::OnKoohBirthdayEventDlg, 0x11);
		}
	}
	break;

	case 0x251:
	{
		unsigned long* pCount = (unsigned long*)msg.param1;

		this << MsgObject(NULL, 0x23,
			(int)"\xbb\xf3\xc7\xb0\xc0\xcc \xbf\xec\xc6\xed\xc0\xb8\xb7\xce \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbf\xec\xc6\xed\xc7\xd4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.",
			0, 0, 0, 0);

		if (m_pBirthdayEventDlg)
			m_pBirthdayEventDlg->SetRemainGiftCount(*pCount);
	}
	break;

	case 0x41:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	case 0x6b:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	case 0xb2:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	case 0xb4:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	case 0x211:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	case 0x21b:
		LogOut(0, "CLobbyMain::HandleMsg %d : %d %d %d %d %d\n", msg.message,
			msg.param1, msg.param2, msg.param3, msg.param4, msg.param5);
		break;

	default:
		HandleMsgCommon(msg);
	}
}

void CLobbyMain::_RefreshUccClothes(sSlotInfo* pSlot, unsigned long tid,
	const char* uccIndex)
{
	if (pSlot == NULL)
		return;

	std::map<unsigned long, sRoomSlot>::iterator it =
		Doc()->m_roomSlotMap.find(pSlot->dwGuid);
	if (it == Doc()->m_roomSlotMap.end())
		return;

	if (!IsLocalContent(S4_GUID_BASE))
		return;

	sRoomSlot* pRoomSlot = &it->second;

	if (pRoomSlot && pRoomSlot->pExhibition)
	{
		unsigned char index = (unsigned char)((tid >> 13) & 0x1f);
		if (pRoomSlot->partTidList.m_tid[index] == tid)
		{
			unsigned long uid = pRoomSlot->partTidList.m_uid[index];

			sUccClothes* pClothes = UccManager()->FindClothes(uid);

			if (pClothes && !strcmpi(pClothes->uccIndex, uccIndex))
			{
				pRoomSlot->pExhibition->GetPetFrame()->SetPart(
					&pRoomSlot->partTidList, tid, uid);
				pRoomSlot->pExhibition->GetPetFrame()->SetPart(
					&pRoomSlot->partTidList, tid, uid);
			}
		}
	}
}

void CLobbyMain::OpenPCBangNoticeForm()
{
	if (m_pPCBangNoticeDlg == NULL && CLoginInfo::Instance()->IsPcBang())
	{
		OnToppage_NoticeUp();
	}
	else if (Doc()->m_bShowNotice)
	{
		OnToppage_NoticeUp();
	}
}

bool CLobbyMain::NeedsChangeNick()
{
	if (CLoginInfo::Instance()->IsWebLogin() &&
		Doc()->m_myInfo.info.nChannelingFlag == 1 &&
		!strcmp(Doc()->m_myInfo.info.sNick, Doc()->m_myInfo.info.sDisplayID))
		return true;

	return false;
}

void CLobbyMain::OpenParanNickChange()
{
	if (NeedsChangeNick() == true)
	{
		m_pParanNickDlg = CreateForm<FrParanNickDlg>(g_pFresh->GetManager(),
			this, "changenick_paran");
		m_pParanNickDlg->SetMessage(
			"\xc6\xce\xbe\xdf\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
			false);
		m_pParanNickDlg->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnParanNickChangeResult, 0);
	}
	else
	{
		OpenCaddieWarningForm();
	}
}

void CLobbyMain::OpenCaddieWarningForm()
{
	if (m_bCaddieWarning && Doc()->IsOpenFirstLoginAlarm())
	{
		m_bCaddieWarning = false;

		for (int i = 0; i < 12; i++)
		{
			if (m_caddieWarning[i].guid != 0)
			{
				sCaddieInfo info = m_caddieWarning[i];
				memset(&m_caddieWarning[i], 0, sizeof(sCaddieInfo));

				if (info.guid)
				{
					m_pCaddieWarningDlg = CreateForm<FrCaddieWarningDlg>(
						g_pFresh->GetManager(), this, "caddie_warning");
					m_pCaddieWarningDlg->SetCaddieInfo(info);
					m_pCaddieWarningDlg->Open(
						(FRESH_PFN_RESULT)&CLobbyMain::OnCaddieWarningDlgResult,
						0x40);
				}
				return;
			}
		}
	}
	else
	{
		OpenNoteForm();
	}
}

void CLobbyMain::OpenNoteForm()
{
	if (Doc()->IsOpenFirstLoginAlarm() && Doc()->m_noteList.size() > 0)
	{
		if (m_pMailBoxDlg == NULL)
		{
			m_pMailBoxDlg = CreateForm<FrMailBoxDlg>(g_pFresh->GetManager(),
				this, "mailbox");
			m_pMailBoxDlg->EnableDrag(false);
			m_pMailBoxDlg->Open(
				(FRESH_PFN_RESULT)&CLobbyMain::OnMailBoxDlgResult, 0);
		}
	}
}

void CLobbyMain::OpenEventPrizeForm(unsigned char id)
{
	if (m_pEventPrizeDlg[id] == NULL)
	{
		m_pEventPrizeDlg[id] = CreateForm<FrEventPrizeDlg>(
			g_pFresh->GetManager(), this, "notice_popup");
		m_pEventPrizeDlg[id]->SetEvent(id);
		m_pEventPrizeDlg[id]->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnEventDlgResult, 3);
	}
}

void CLobbyMain::OnToppage_Init(int param)
{
	CTaskMain::OnInit();

	Doc()->ClearUserInfoTimeMap(0xffffffff);
	memset(Doc()->m_userInfo, 0, sizeof(sUserInfo) * 4);

	SetRandomBGM();
	TurnOn3DBackground();

	if (IsLocalContent(S4_TOPPAGE_BTN_LINEUP))
	{
		m_topBtnIndex = CLoginInfo::Instance()->IsPcBang() ? 2 : 1;
	}
}

void CLobbyMain::OnToppage_Finish()
{
	if (Doc()->m_bAutoRefresh)
	{
		if (IsLocalContent(S3_PARAN_CHANNELING))
		{
			Doc()->m_bAutoRefresh = false;
			OpenEventPrizeForm(5);

			if (Doc()->m_bTutorialRedirect == true)
			{
				this << MsgObject(NULL, 0x3e, 0x1a000006,
					(int)"[\xc7\xc7\xc7\xc9 \xbc\xb1\xb9\xb0 \xb8\xde\xc0\xcf]\n\n\xbe\xc8\xb3\xe7~ \xc0\xfc \xb4\xe7\xbd\xc5\xc0\xbb \xbf\xa9\xb1\xe2 \xb5\xa5\xb8\xae\xb0\xed \xbf\xc2 \xbd\xc3\xb0\xa3\xc0\xc7 \xbf\xe4\xc1\xa4 \xc7\xc7\xc7\xc9\xc0\xcc\xb6\xf3\xb0\xed \xc7\xd5\xb4\xcf\xb4\xd9. \xb9\xe6\xb1\xdd \xbf\xb9\xbb\xdb \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xdf\xb3\xd7\xbf\xe4. \xb0\xe8\xbc\xd3 \xc0\xdf \xba\xb8\xbb\xec\xc6\xec\xc1\xd6\xbc\xbc\xbf\xe4.\n\n\xbe\xc6~ \xb1\xd7\xb8\xae\xb0\xed \xc0\xcc\xb0\xc5 \xb9\xde\xc0\xb8\xbc\xbc\xbf\xe4. \xb4\xe7\xbd\xc5\xc0\xcc \xc6\xce\xbe\xdf\xbf\xa1 \xba\xb8\xb4\xd9 \xbd\xb1\xb0\xd4 \xc0\xfb\xc0\xc0\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad \xb5\xe5\xb8\xae\xb4\xc2 \xc7\xe0\xbf\xee\xc0\xc7 \xb8\xf1\xb0\xc9\xc0\xcc\xc0\xd4\xb4\xcf\xb4\xd9.\n\xc0\xcc \xc7\xe0\xbf\xee\xc0\xc7 \xb8\xf1\xb0\xc9\xc0\xcc\xb4\xc2 \xb4\xe7\xbd\xc5\xc0\xcc \xb8\xb8\xb5\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \\c0xffff0000\\c\xc1\xa4\xc8\xae\xb5\xb5\\c0xff000000\\c\xb8\xa6 \xb3\xf4\xc7\xf4\xc1\xd6\xb0\xed \\c0xffff0000\\c\xc6\xce\xbe\xdf\xc0\xd3\xc6\xd1\xc6\xae\xc1\xb8\\c0xff000000\\c\xc0\xbb \xb3\xd0\xc7\xf4\xc1\xd6\xb4\xc2 \xbf\xaa\xc7\xd2\xc0\xbb \xc7\xd1\xb4\xe4\xb4\xcf\xb4\xd9.\n\xc7\xcf\xb3\xaa \xc1\xd6\xc0\xc7\xc7\xd2 \xb0\xcd\xc0\xba\xbf\xe4~ \xb4\xe7\xbd\xc5\xc0\xc7 \xbd\xc7\xb7\xc2\xc0\xcc \xbe\xee\xb4\xc0 \xc1\xa4\xb5\xb5 \xbe\xc8\xc1\xa4\xb5\xc7\xb4\xc2 \xb7\xb9\xba\xa7, \xc1\xef \\c0xffff0000\\c\xc1\xd6\xb4\xcf\xbe\xee"
						 "E\\c0xff000000\\c\xb0\xa1 \xb5\xc7\xb8\xe9 \xc8\xbf\xb0\xfa\xb0\xa1 \xbe\xf8\xbe\xee\xc1\xf6\xb4\xcf\xb1\xee\xbf\xe4~ \xbe\xcb\xbe\xc6\xb5\xce\xbd\xc3\xb0\xed\xbf\xe4.\n\n\xb1\xd7\xb8\xae\xb0\xed \xb4\xeb\xc0\xfc\xb8\xf0\xb5\xe5\xbf\xa1 \xb0\xa1\xb1\xe2 \xc0\xfc\xbf\xa1 \xc7\xd1\xb9\xf8 \xc8\xa5\xc0\xda \xbf\xac\xbd\xc0\xc7\xcf\xb0\xed \xbd\xcd\xb4\xd9\xb8\xe9 \xc6\xa9\xc5\xe4\xb8\xae\xbe\xf3 \xb8\xf0\xb5\xe5\xbf\xa1 \xb5\xe9\xbe\xee\xb0\xa1\xba\xb8\xbc\xbc\xbf\xe4.\n\xc2\xf7\xb1\xd9\xc2\xf7\xb1\xd9 \xb8\xf0\xb5\xce \xb9\xe8\xbf\xec\xbd\xc3\xb8\xe9 \xbc\xb1\xb9\xb0\xb5\xb5 \xc1\xd8\xb4\xd9\xb0\xed \xc7\xcf\xb3\xd7\xbf\xe4. ^^\n\n\xb1\xd7\xb7\xb3 \xb4\xe3\xbf\xa1 \xb6\xc7 \xba\xc1\xbf\xe4. \xbe\xc8\xb3\xe7",
					0, 0, 0);
				Doc()->m_bTutorialRedirect = false;
			}
		}
		else
		{
			Doc()->m_bAutoRefresh = false;
			OpenEventPrizeForm(2);
		}
	}

	if (Doc()->m_bRefreshCamera)
		this << MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0);
}

void CLobbyMain::OnToppage_Destroy()
{
}

void CLobbyMain::OnToppage_GameInit(int param)
{
	m_pToppageGame = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppageGame)
	{
		m_pToppageGame->SetPushSound("ui_desktop_icon_click");
		m_pToppageGame->EnableHover(true, 0.5f);
	}
}

void CLobbyMain::OnToppage_GameUp()
{
	OnUnderBar_GameUp();
}

void CLobbyMain::OnToppage_FamilyInit(int param)
{
	m_pToppageFamily = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppageFamily)
	{
		m_pToppageFamily->SetPushSound("ui_desktop_icon_click");
		m_pToppageFamily->EnableHover(true, 0.5f);
	}
}

void CLobbyMain::OnToppage_FamilyUp()
{
	for (int i = 0; i < 4; ++i)
	{
		memset(&Doc()->m_userInfo[i], 0, sizeof(sUserInfo));

		if (i < 2)
			sprintf(Doc()->m_userInfo[i].info.sNick, "PLAYER%d", i + 1);
		else
		{
			Doc()->m_userInfo[i].info.sID[0] = 0;
			Doc()->m_userInfo[i].info.sNick[0] = 0;
		}

		Doc()->m_userInfo[i].stat.Level = Doc()->m_myInfo.stat.Level;
		Doc()->m_userInfo[i].userEquip.guidMascot =
			Doc()->m_myInfo.userEquip.guidMascot;
		memset(CFamilyMain::ms_itemslotBackup[i], 0,
			sizeof(CFamilyMain::ms_itemslotBackup[i]));
	}

	Doc()->m_golfGame.shotTimeLimit = 0;
	SetCurMap(0);
	SetGameType(0);
	SetHoles(3);
	Doc()->m_holeType = 0;
	Doc()->m_bGameOver = false;
	CMouseCursor::Instance()->SetActive(false);

	CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
	PostMsg(NULL, "Family", 0, (int)"FAMILY", 1, 0, 0);
}

void CLobbyMain::OnToppage_PangysStudyInit(int param)
{
	m_pToppagePangyaStudy = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppagePangyaStudy)
	{
		m_pToppagePangyaStudy->SetPushSound("ui_desktop_icon_click");
		m_pToppagePangyaStudy->EnableHover(true, 0.5f);
	}
}

void CLobbyMain::OnToppage_PangysStudyUp()
{
	CloseAllDialogs();

	CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
	AfxPostMsg(NULL, "TutorialMain", 0, (int)"TUTORIAL_MAIN", 0, 0, 0);
}

void CLobbyMain::OnToppage_NoticeInit(int param)
{
	m_pToppageNotice = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppageNotice)
	{
		m_pToppageNotice->SetPushSound("ui_desktop_icon_click");
		m_pToppageNotice->EnableHover(true, 0.5f);
	}
}

void CLobbyMain::OnToppage_NoticeUp()
{
	OnUnderBar_NoticeUp();

	if (m_pToppageNotice && m_pNoticeDlg)
		m_pNoticeDlg->SetIconRect(m_pToppageNotice->GetRect());
}

void CLobbyMain::OnToppage_PcbangInit(int param)
{
	m_pToppagePcbang = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppagePcbang)
	{
		int bPcBang = CLoginInfo::Instance()->IsPcBang();
		m_pToppagePcbang->SetVisible(bPcBang != 0);
		bPcBang = CLoginInfo::Instance()->IsPcBang();
		m_pToppagePcbang->Enable(bPcBang != 0);

		if (CLoginInfo::Instance()->IsPcBang())
			CIconManager::Instance()->AddTopIconFromOutSide();

		cTopEventIcon icon((eTopIconID)0);
		icon.Prepare(m_pToppagePcbang);
	}
}

void CLobbyMain::OnToppage_PcbangUp()
{
	CloseAllDialogs();
	m_pPCBangNoticeDlg =
		CreateForm<FrNoticeDlg>(g_pFresh->GetManager(), this, "notice_popup");
	m_pPCBangNoticeDlg->LoadInitFile("pcbang.ini");
	m_pPCBangNoticeDlg->SetIconRect(m_pToppagePcbang->GetRect());
	m_pPCBangNoticeDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnPCBangDlgResult,
		0x11);
}

void CLobbyMain::OnToppage_RentalExpiredAlramInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
	{
		if (!S5::RENTALPARTS::IsExistExpiredRentalItem(Doc()->m_partsList))
		{
			pButton->Enable(false);
			pButton->SetVisible(false);
		}
	}
}

void CLobbyMain::OnToppage_RentalExpiredAlramUp()
{
	std::list<sItemInfo> rentalItems;
	S5::RENTALPARTS::CollectRentalItem(rentalItems, Doc()->m_partsList);

	int count = 0;
	for (std::list<sItemInfo>::iterator it = rentalItems.begin();
		it != rentalItems.end(); ++it)
	{
		if (it->Expired)
			count++;
	}

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");

	pForm->SetMessage(
		MakeStr(
			"\xb1\xe2\xb0\xa3\xc0\xcc \xb8\xb8\xb7\xe1\xb5\xc8 \xc0\xc7\xbb\xf3\xc0\xcc <%d>\xb0\xb3 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.\n\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xd2 \xbc\xf6 \xc0\xd6\xc0\xb8\xb8\xe7 \xb1\xe2\xb0\xa3\xc0\xbb \xbf\xac\xc0\xe5\xc7\xcf\xb0\xc5\xb3\xaa \xbb\xe8\xc1\xa6\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			count),
		false);

	FrButton* pCancel =
		DYNAMIC_CAST(FrButton, pForm->FindChildByName("cancel"));
	if (pCancel)
		pCancel->SetVisible(false);

	FrButton* pOk = DYNAMIC_CAST(FrButton, pForm->FindChildByName("ok"));

	pForm->Open(NULL, 0);
}

void CLobbyMain::OnToppage_Event1Init(int param)
{
	m_pToppageEvent1 = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppageEvent1)
	{
		m_pToppageEvent1->SetPushSound("ui_desktop_icon_click");
		m_pToppageEvent1->EnableHover(true, 0.5f);
		m_pToppageEvent1->SetVisible(false);

		if (IsLocalContent((localContentType_t)0x18))
		{
			m_pToppageEvent1->SetVisible(true);
		}

		m_event1Rect = m_pToppageEvent1->GetRect();
	}
}

void CLobbyMain::OnToppage_Event1Up()
{
	if (IsLocalContent((localContentType_t)0x18))
	{
		CloseAllDialogs();

		if (IsLocalContent((localContentType_t)0x48))
			m_pHolesEventDlg = CreateForm<ntHolesEventDlg>(
				g_pFresh->GetManager(), this, "holes_EntranceExam");
		else
			m_pHolesEventDlg = CreateForm<ntHolesEventDlg>(
				g_pFresh->GetManager(), this, "holes_event");

		m_pHolesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(),
			this, "holes_event");
		m_pHolesEventDlg->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnHolesEventDlgResult, 0x11);
		m_pHolesEventDlg->SetIconRect(m_pToppageEvent1->GetRect());
	}
}

void CLobbyMain::OnToppage_NewInit(int param)
{
	m_pToppageNew = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnToppage_NewOwnerDraw(int param)
{
	if (m_pToppagePangyaStudy == NULL)
		return;

	if (m_pNewBmp == NULL)
		return;

	WRect rect;
	rect = m_pToppagePangyaStudy->GetRect();

	g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pNewBmp,
		WRect(rect.x + 58.0f, rect.y + 12.0f, (float)m_pNewBmp->Width(),
			(float)m_pNewBmp->Height()),
		0xffffffff, 0);
}

void CLobbyMain::OnFirstConnectOwnerDraw(int param)
{
	if (m_pToppagePointEvent == NULL)
		return;

	if (!Doc()->m_bToppageAd)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI == NULL)
		return;

	for (int i = 0; i < 3; i++)
	{
		if (m_pLoginBmp[i] == NULL)
			return;
	}

	WRect rect = m_pToppagePointEvent->GetRect();
	rect.y += rect.h - 10.0f;

	if (m_pLoginBmp[0])
	{
		pGDI->DrawTexture(m_pLoginBmp[0],
			WRect(rect.x, rect.y, rect.h - 15.0f,
				(float)m_pLoginBmp[0]->Height()),
			0xffffffff, 0);
		rect.y += (float)m_pLoginBmp[0]->Height();
	}

	if (m_pLoginBmp[1])
	{
		pGDI->DrawTexture(m_pLoginBmp[1],
			WRect(rect.x, rect.y, rect.h - 15.0f,
				(float)m_pLoginBmp[1]->Height() + 18.0f),
			0xffffffff, 0);
		rect.y = rect.y + (float)m_pLoginBmp[1]->Height() + 18.0f;
	}

	if (m_pLoginBmp[2])
	{
		pGDI->DrawTexture(m_pLoginBmp[2],
			WRect(rect.x, rect.y, rect.h - 15.0f,
				(float)m_pLoginBmp[2]->Height()),
			0xffffffff, 0);
		rect.y -= (float)(m_pLoginBmp[0]->Height() + m_pLoginBmp[1]->Height()) +
			18.0f;
	}

	float textX = rect.x + 3.0f;
	pGDI->Print(WPoint(textX, rect.y + 8.0f), 0,
		"\xc3\xe2\xbc\xae\xc6\xf7\xc0\xce\xc6\xae");
	pGDI->Print(WPoint(textX, rect.y + 19.0f), 0,
		"25 \xc6\xf7\xc0\xce\xc6\xae");
	pGDI->Print(WPoint(textX, rect.y + 30.0f), 0, "\xc8\xb9\xb5\xe6!!");
}

bool CLobbyMain::OnValEventDlgResult(int result, FrForm* pForm)
{
	m_pValEventDlg = NULL;
	return true;
}

void CLobbyMain::OnToppage_PointEventInit(int param)
{
	m_pToppagePointEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pToppagePointEvent == NULL)
		return;

	if (IsLocalContent((localContentType_t)0x15))
	{
		m_pToppagePointEvent->SetPushSound("ui_desktop_icon_click");
		m_pToppagePointEvent->EnableHover(true, 0.5f);
		m_pToppagePointEvent->SetVisible(true);
		m_pToppagePointEvent->SetPushDelay(0.0f);

		if (IsLocalContent(S4_TOPPAGE_BTN_LINEUP))
		{
			WRect rect = m_pToppagePointEvent->GetRect();
			float x = GetTopPageBtnRect();
			rect.x = x;
			m_pToppagePointEvent->SetRect(rect);
		}
		else if (!CLoginInfo::Instance()->IsPcBang())
		{
			WRect pcbangRect = m_pToppagePcbang->GetRect();
			WRect rect = m_pToppagePointEvent->GetRect();
			rect.x = pcbangRect.x;
			rect.y = pcbangRect.y - 2.0f;
			m_pToppagePointEvent->SetRect(rect);
		}
		else
		{
			WRect rect = m_pToppagePointEvent->GetRect();
			rect.x = 160.0f;
			rect.y -= 2.0f;
			m_pToppagePointEvent->SetRect(rect);
		}
	}
	else
	{
		m_pToppagePointEvent->SetVisible(false);
	}
}

void CLobbyMain::OnToppage_PointEventDown()
{
	if (IsLocalContent((localContentType_t)0x15))
	{
		CloseAllDialogs();
		WSendPacket packet((enumClientPacket)0x95);
		packet.Send(TO_GAME);

		Doc()->m_pendingMenu = 2;
		Doc()->m_bToppageAd = false;
	}
}

void CLobbyMain::OnToppage_ValEventInit(int param)
{
	m_pToppageValEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pToppageValEvent)
	{
		m_pToppageValEvent->SetVisible(
			IsLocalContent((localContentType_t)0x78));
		if (m_pToppageValEvent->IsVisible())
		{
			m_pToppageValEvent->SetPushDelay(0.1f);

			cTopEventIcon icon((eTopIconID)8);
			icon.Prepare(m_pToppageValEvent);
		}
	}
}

void CLobbyMain::OnToppage_ValEventDown()
{
	if (IsLocalContent((localContentType_t)0x78) == true)
	{
		WSendPacket packet((enumClientPacket)0xdc);
		packet.Send(TO_GAME);
	}
}

void CLobbyMain::OnToppage_WorldTourEventInit(int param)
{
	m_pToppageWorldTourEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pToppageWorldTourEvent == NULL)
		return;

	if (IsLocalContent((localContentType_t)0x1e))
	{
		m_pToppageWorldTourEvent->SetPushSound("ui_desktop_icon_click");
		m_pToppageWorldTourEvent->EnableHover(true, 0.5f);
		m_pToppageWorldTourEvent->SetVisible(true);
		m_pToppageWorldTourEvent->SetPushDelay(0.0f);

		bool bPcBang = CLoginInfo::Instance()->IsPcBang() != 0;
		bool bPointEvent = IsLocalContent((localContentType_t)0x15);

		WRect pcbangRect = m_pToppagePcbang->GetRect();
		WRect pointRect = m_pToppagePointEvent->GetRect();
		WRect rect = m_pToppageWorldTourEvent->GetRect();

		rect.y += 2.0f;

		if (IsLocalContent(S4_TOPPAGE_BTN_LINEUP))
		{
			WRect btnRect = m_pToppageWorldTourEvent->GetRect();
			float x = GetTopPageBtnRect();
			btnRect.x = x;
			m_pToppageWorldTourEvent->SetRect(btnRect);
		}
		else if (bPcBang && bPointEvent)
		{
			rect.x = pointRect.x + pointRect.w;
			m_pToppageWorldTourEvent->SetRect(rect);
		}
		else if (bPcBang || bPointEvent)
		{
			if (bPcBang)
			{
				rect.x = pcbangRect.x + pcbangRect.w;
				m_pToppageWorldTourEvent->SetRect(rect);
			}
			else
			{
				rect.x = pointRect.x + pointRect.w;
				m_pToppageWorldTourEvent->SetRect(rect);
			}
		}
	}
	else
	{
		m_pToppageWorldTourEvent->SetVisible(false);
		m_pToppageWorldTourEvent->Enable(false);
	}
}

void CLobbyMain::OnToppage_WorldTourEventDown()
{
	if (!IsLocalContent((localContentType_t)0x1e))
		return;

	if (m_pToppageWorldTourEvent == NULL)
		return;

	CloseAllDialogs();

	CWorldTourEvent* pEvent =
		(CWorldTourEvent*)CContentsDoc::Instance()->GetContainer(
			(localContentType_t)0x1e);
	if (pEvent == NULL)
		return;

	if (m_pWorldTourEventDlg)
	{
		m_pWorldTourEventDlg->SetMapFlag(pEvent->GetMapFlag());
		return;
	}

	m_pWorldTourEventDlg = CreateForm<FrWorldTourEventDlg>(
		g_pFresh->GetManager(), this, "world_tour_event_dlg");

	if (m_pWorldTourEventDlg == NULL)
		return;

	m_pWorldTourEventDlg->SetMapFlag(pEvent->GetMapFlag());
	m_pWorldTourEventDlg->SetIconRect(m_pToppageWorldTourEvent->GetRect());
	m_pWorldTourEventDlg->Open(
		(FRESH_PFN_RESULT)&CLobbyMain::OnWorldTourEventDlgResult, 0x11);
}

void CLobbyMain::OnToppage_FindEbortEventInit(int param)
{
	m_pToppageFindEbortEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pToppageFindEbortEvent == NULL)
		return;

	if (IsLocalContent((localContentType_t)0x20))
	{
		m_pToppageFindEbortEvent->SetVisible(true);
		m_pToppageFindEbortEvent->Enable(true);
		m_pToppageFindEbortEvent->SetPushDelay(0.0f);

		bool bPcBang = CLoginInfo::Instance()->IsPcBang() != 0;
		bool bPointEvent = IsLocalContent((localContentType_t)0x15);
		bool bWorldTour = IsLocalContent((localContentType_t)0x1e);

		WRect rect = m_pToppageFindEbortEvent->GetRect();

		if (bPcBang)
			rect.x +=
				m_pToppagePcbang->GetRect().x + m_pToppagePcbang->GetRect().w;

		if (bPointEvent)
			rect.x += m_pToppagePointEvent->GetRect().x +
				m_pToppagePointEvent->GetRect().w;

		if (bWorldTour)
			rect.x += m_pToppageWorldTourEvent->GetRect().x +
				m_pToppageWorldTourEvent->GetRect().w;

		rect.y -= 5.0f;
		m_pToppageFindEbortEvent->SetRect(rect);
	}
	else
	{
		m_pToppageFindEbortEvent->SetVisible(false);
		m_pToppageFindEbortEvent->Enable(false);
	}
}

void CLobbyMain::OnToppage_FindEbortEventDown()
{
	if (!IsLocalContent((localContentType_t)0x20))
		return;

	if (m_pToppageFindEbortEvent == NULL)
		return;

	CloseAllDialogs();

	if (m_pFindEbortEventDlg)
		return;

	m_pFindEbortEventDlg = CreateForm<FrFindEbortEventDlg>(
		g_pFresh->GetManager(), this, "find_ebort_event_dlg");

	if (m_pFindEbortEventDlg == NULL)
		return;

	m_pFindEbortEventDlg->SetIconRect(m_pToppageFindEbortEvent->GetRect());
	m_pFindEbortEventDlg->Open(
		(FRESH_PFN_RESULT)&CLobbyMain::OnFindEbortEventDlgResult, 0x11);
}

void CLobbyMain::OnToppage_Halloween2007Init(int param)
{
	m_pToppageHalloween2007Event = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pToppageHalloween2007Event)
	{
		if (IsLocalContent((localContentType_t)0x21))
		{
			if (CContentsDoc::Instance()->GetContainer(
					(localContentType_t)0x21))
			{
				m_pToppageHalloween2007Event->SetVisible(true);
				m_pToppageHalloween2007Event->Enable(true);
			}
			else
			{
				m_pToppageHalloween2007Event->SetVisible(false);
				m_pToppageHalloween2007Event->Enable(false);
			}

			cTopEventIcon icon((eTopIconID)9);
			icon.Prepare(m_pToppageHalloween2007Event);
		}
		else
		{
			m_pToppageHalloween2007Event->SetVisible(false);
			m_pToppageHalloween2007Event->Enable(false);
		}
	}
}

void CLobbyMain::OnToppage_Halloween2007EvenDown()
{
	if (!IsLocalContent((localContentType_t)0x21))
		return;

	if (m_pToppageHalloween2007Event == NULL)
		return;

	CloseAllDialogs();

	if (m_pHalloweenEventDlg)
		return;

	m_pHalloweenEventDlg = CreateForm<FrHalloweenEventDlg>(
		g_pFresh->GetManager(), this, "halloween2007_event_dlg");

	if (m_pHalloweenEventDlg == NULL)
		return;

	m_pHalloweenEventDlg->SetIconRect(m_pToppageHalloween2007Event->GetRect());
	m_pHalloweenEventDlg->Open(
		(FRESH_PFN_RESULT)&CLobbyMain::OnHalloweenEventDlgResult, 0x11);
}

void CLobbyMain::OnToppage_Christmas2007EventInit(int param)
{
	m_pToppageChristmas2007Event = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	CChristmasEvent* pEvent;
	if (IsLocalContent((localContentType_t)0x4c) &&
		CContentsDoc::Instance()->GetContainer((localContentType_t)0x4c,
			pEvent) &&
		pEvent->GetFlag())
	{
		m_pToppageChristmas2007Event->SetVisible(true);
		m_pToppageChristmas2007Event->Enable(true);

		cTopEventIcon((eTopIconID)7).Prepare(m_pToppageChristmas2007Event);
	}
	else
	{
		m_pToppageChristmas2007Event->SetVisible(false);
		m_pToppageChristmas2007Event->Enable(false);
	}
}

void CLobbyMain::OnToppage_Christmas2007EventDown()
{
	if (IsLocalContent((localContentType_t)0x4c))
	{
		if (m_pToppageChristmas2007Event)
		{
			CloseAllDialogs();

			if (m_pChristmasEventDlg == NULL)
			{
				m_pChristmasEventDlg = CreateForm<FrChristmasEventDlg>(
					g_pFresh->GetManager(), this, "christmas2007_event_dlg");

				if (m_pChristmasEventDlg)
				{
					m_pChristmasEventDlg->SetIconRect(
						m_pToppageChristmas2007Event->GetRect());
					m_pChristmasEventDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::
												   OnChristmasEventDlgResult,
						0x11);
				}
			}
		}
	}
}

void CLobbyMain::OnToppage_TicketExchangeInit(int param)
{
	m_pToppageTicketExchange = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	CTicketExchange* pEvent;
	if (IsLocalContent((localContentType_t)0x7a) &&
		CContentsDoc::Instance()->GetContainer((localContentType_t)0x7a,
			pEvent) &&
		pEvent->GetFlag())
	{
		m_pToppageTicketExchange->SetVisible(true);
		m_pToppageTicketExchange->Enable(true);

		cTopEventIcon((eTopIconID)10).Prepare(m_pToppageTicketExchange);

		m_pToppageTicketExchange->SetPushSound("ui_desktop_icon_click");
		m_pToppageTicketExchange->EnableHover(true, 0.5f);
		m_pToppageTicketExchange->SetVisible(true);
		m_pToppageTicketExchange->SetPushDelay(0.0f);

		if (IsLocalContent(S4_TOPPAGE_BTN_LINEUP))
		{
			WRect rect = m_pToppageTicketExchange->GetRect();
			float x = GetTopPageBtnRect();
			rect.x = x;
			m_pToppageTicketExchange->SetRect(rect);
		}
	}
	else
	{
		m_pToppageTicketExchange->SetVisible(false);
		m_pToppageTicketExchange->Enable(false);
	}
}

void CLobbyMain::OnToppage_TicketExchangeDown()
{
	if (IsLocalContent((localContentType_t)0x7a))
	{
		if (m_pToppageTicketExchange)
		{
			CloseAllDialogs();

			if (m_pTicketExchangeDlg == NULL)
			{
				m_pTicketExchangeDlg = CreateForm<FrTicketExchangeDlg>(
					g_pFresh->GetManager(), this, "ticket_exchage_dlg");

				if (m_pTicketExchangeDlg)
				{
					m_pTicketExchangeDlg->SetIconRect(
						m_pTicketExchangeDlg->GetRect());
					m_pTicketExchangeDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::
												   OnTicketExchangeDlgResult,
						0x11);
				}
			}
		}
	}
}

void CLobbyMain::OnToppage_MissionEventInit(int param)
{
	m_pToppageMissionEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	cTopEventIcon((eTopIconID)13).Prepare(m_pToppageMissionEvent);

	if (!IsLocalContent((localContentType_t)0x83))
	{
		m_pToppageMissionEvent->Enable(false);
		m_pToppageMissionEvent->SetVisible(false);
	}
}

void CLobbyMain::OnToppage_MissionEventDown()
{
	if (IsLocalContent((localContentType_t)0x83))
	{
		WSendPacket packet((enumClientPacket)0xe3);
		packet.Send(TO_GAME);

		this << MsgObject(this, 1, 0, 0, 0, 0, 0);
	}
}

void CLobbyMain::OnToppage_BingoEventInit(int param)
{
	m_pToppageBingoEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	cTopEventIcon((eTopIconID)12).Prepare(m_pToppageBingoEvent);

	if (!IsLocalContent((localContentType_t)0x85))
	{
		m_pToppageBingoEvent->Enable(false);
		m_pToppageBingoEvent->SetVisible(false);
	}
}

void CLobbyMain::OnToppage_BingoEventDown()
{
	if (IsLocalContent((localContentType_t)0x85))
	{
		WSendPacket packet((enumClientPacket)0xe5);
		packet.Encode4(0);
		packet.Send(TO_GAME);

		this << MsgObject(this, 1, 0, 0, 0, 0, 0);
	}
}

void CLobbyMain::OnToppage_KoohBirthdayEventInit(int param)
{
	m_pToppageKoohBirthdayEvent = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	std::string name("");
	if (IsLocalContent((localContentType_t)0x8e))
	{
		name = "KoohBirthdayEvent";
	}

	if (IsLocalContent((localContentType_t)0x94))
	{
		name = "HanaBirthdayEvent";
	}
	else if (IsLocalContent((localContentType_t)0x9b))
	{
		name = "ArinBirthdayEvent";
	}

	if (name != "")
	{
		CGDBitmapBox* pBox =
			(CGDBitmapBox*)CGameDataDB::Instance()->GetGameData(
				GAMEDATA_BITMAPBOX, name.c_str());
		m_pToppageKoohBirthdayEvent->SetButtonImg(
			pBox->GetFileName("LobbyIconNormal"), FrButton::NORMAL);
		m_pToppageKoohBirthdayEvent->SetButtonImg(
			pBox->GetFileName("LobbyIconOver"), FrButton::OVER);
		m_pToppageKoohBirthdayEvent->SetButtonImg(
			pBox->GetFileName("LobbyIconBlink"), FrButton::BLINK);

		cTopEventIcon((eTopIconID)6).Prepare(m_pToppageKoohBirthdayEvent);
	}
	else
	{
		m_pToppageKoohBirthdayEvent->Enable(false);
		m_pToppageKoohBirthdayEvent->SetVisible(false);
	}
}

void CLobbyMain::OnToppage_KoohBirthdayEventDown()
{
	WSendPacket packet(0xef);
	packet.Send(TO_GAME);
	this << MsgObject(this, 1, 0, 0, 0, 0, 0);
}

void CLobbyMain::OnToppage_TreasureIslandInit(int param)
{
	m_pToppageTreasureIsland = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	cTopEventIcon((eTopIconID)11).Prepare(m_pToppageTreasureIsland);

	if (!IsLocalContent((localContentType_t)0x8c))
	{
		m_pToppageTreasureIsland->Enable(false);
		m_pToppageTreasureIsland->SetVisible(false);
	}
}

void CLobbyMain::OnToppage_TreasureIslandLBDown()
{
	if (IsLocalContent((localContentType_t)0x8c))
	{
		ShellExecuteA(NULL, NULL,
			"http://qa.pangya.gametree.co.kr/events/bomul/PangyaBomul.aspx",
			NULL, NULL, SW_SHOWMAXIMIZED);
	}
}

void CLobbyMain::OnToppage_ServerInit(int param)
{
	m_pToppageServer = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToppageServer)
	{
		m_pToppageServer->SetPushSound("ui_desktop_icon_click");
		m_pToppageServer->EnableHover(true, 0.5f);
	}
}

void CLobbyMain::OnToppage_ServerUp()
{
	CloseAllDialogs();
	OnUnderBar_ServerUp();
}

void CLobbyMain::CloseAllDialogs()
{
	if (m_pOptionDlg)
		m_pOptionDlg->Close(true);

	if (m_pPCBangNoticeDlg)
		m_pPCBangNoticeDlg->Close(true);

	if (CMessengerInfo::Instance()->IsVisible())
		CMessengerInfo::Instance()->Toggle();

	if (CUserInfo::IsInstantiated())
		CUserInfo::Instance()->Close();

	if (m_pServerDlg)
		m_pServerDlg->Close(false);

	if (m_pPointEventDlg)
	{
		m_pPointEventDlg->Close(true);
	}

	if (m_pCreateNickDlg)
	{
		m_pCreateNickDlg->CloseDlg(0);
	}

	CIconToolTip::GetInstance()->ClearToolTip();

	CTaskMain::CloseAllDialogs();
}

void CLobbyMain::OnServer_Init(int param)
{
	CTaskMain::OnInit();
	SetRandomBGM();
	TurnOn3DBackground();
}

void CLobbyMain::OnServer_Finish()
{
	if (m_pServerDlg == NULL)
	{
		m_pServerDlg =
			CreateForm<FrServerDlg>(g_pFresh->GetManager(), this, "server");

		m_pServerDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnServerDlgResult,
			0x10);
	}
}

bool CLobbyMain::OnMatchRoomCreateDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		WSendPacket packet((enumClientPacket)8);

		packet.Encode1(0);
		packet.Encode4(60000);
		packet.Encode4(0);

		packet.Encode1(Doc()->m_gameTypeInfo[3].maxPlayer);

		packet.Encode1(3);
		packet.Encode1(9);
		packet.Encode1(FirstMap());
		packet.Encode1(0);

		packet.EncodeStr(
			std::string("\xb8\xda\xc1\xf8 \xb4\xeb\xc0\xfc \xc7\xd1\xc6\xc7"));
		packet.EncodeStr(std::string(""));
		packet.Send(TO_GAME);

		this << MsgObject(NULL, 0x16, 1, 0, 0, 0, 0);
	}
	else
	{
		EnableRoomEnterControls(true);
	}

	m_pReportDlg = NULL;
	return true;
}

bool CLobbyMain::OnNewRecordDlgResult(int result, FrForm* form)
{
	m_pNewRecordDlg = NULL;
	return true;
}

void CLobbyMain::OnLogin_Init(int param)
{
	CTaskMain::OnInit();

	if (NET()->IsConnected(WNetworkSystem::NET_MSN))
	{
		CMessengerInfo::Instance()->Close(true);
		NET()->ForceShutDown(WNetworkSystem::NET_MSN);
	}

	Doc()->ClearAll();
	Buddy()->ClearBuddy();

	SetRandomBGM();
	TurnOn3DBackground();
}

extern WInputDev* g_mouse;

void CLobbyMain::OnLogin_Finish()
{
	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();
	CMouseCursor::Instance()->SetActive(true);

	if (!CProjectG::Instance()->CheckUIHack())
	{
		m_bLoginBlock = true;

		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage(
			"\xba\xaf\xc1\xb6\xb5\xc8 \xc6\xc4\xc0\xcf\xc0\xcc \xb9\xdf\xb0\xdf\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc7\xd8\xb4\xe7 \xc6\xc4\xc0\xcf\xc0\xbb \xc1\xa6\xb0\xc5 \xc7\xcf\xb0\xc5\xb3\xaa \xc6\xce\xbe\xdf\xb8\xa6 \xc0\xe7\xbc\xb3\xc4\xa1 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc7\xc7\xe0\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnUIHackDlgResult, 0);
	}
	else
	{
		m_pLoginDlg = CreateForm<FrLoginDlg>(g_pFresh->GetManager(),
			(CTaskMain*)this, "login");
		m_pLoginDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnLoginDlgResult, 10);

		CCardManager::Instance()->InitCardBuffDlgPosition();
		CItemBuff::ResetBuffList();
	}
}

void CLobbyMain::OnBlank_Init(int param)
{
	CTaskMain::OnInit();
}

void CLobbyMain::OnBlank_Finish()
{
	unsigned char change = Doc()->GetForceNicknameChange();
	if (change == 1 || change == 2)
	{
		GetActor("Lobby") << MsgObject(NULL, 0x249, 1, 0, 0, 0, 0);
	}
}

void CLobbyMain::OnSecond_Pwd_Init(int param)
{
	CTaskMain::OnInit();
}

void CLobbyMain::OnSecond_Pwd_Finish()
{
}

bool CLobbyMain::OnMakeRoomDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_pMakeRoomDlg)
		{
			if (LatestVersion() && !m_pMakeRoomDlg->MakeRoom(false))
				return false;
		}
	}
	else
	{
		EnableRoomEnterControls(true);
	}

	m_pMakeRoomDlg = NULL;
	return true;
}

bool CLobbyMain::OnRemoveFriendDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selUID);

		m_selUID = -1;
	}

	return true;
}

bool CLobbyMain::OnEventDlgResult(int result, FrForm* form)
{
	FrEventPrizeDlg* pDlg = (FrEventPrizeDlg*)form;
	m_pEventPrizeDlg[pDlg->GetEvent()] = NULL;

	return true;
}

bool CLobbyMain::OnMailBoxDlgResult(int result, FrForm* form)
{
	m_pMailBoxDlg = NULL;

	return true;
}

bool CLobbyMain::OnScoreDlgResult(int result, FrForm* form)
{
	m_pScoreDlg = NULL;
	return true;
}

bool CLobbyMain::OnResultDlg_Report(int result, FrForm* form)
{
	if (!IsLocalContent(S3_CADDIE_REPORT))
		return false;

	m_pUniteResultDlg = NULL;
	return true;
}

bool CLobbyMain::OnApproachGiftDlgResult(int result, FrForm* form)
{
	m_pTreasureGift = NULL;

	WSendPacket packet(0xc3);
	packet.Encode4(MyGuid(false));
	packet.Send(TO_GAME);

	return true;
}

bool CLobbyMain::OnResultDlgResult(int result, FrForm* form)
{
	m_pUniteResultDlg = NULL;
	m_resultDelay = -0.1f;

	if (m_pRoomUser)
	{
		unsigned char rank = 1;
		for (FrListBox::ITEM_LIST::iterator it =
				 m_pRoomUser->m_itemList.begin();
			it != m_pRoomUser->m_itemList.end(); ++it)
		{
			sSlotInfo* pSlot = (sSlotInfo*)(*it)->pData;
			if (pSlot)
				pSlot->connectionRank = rank;
			rank++;
		}
	}

	Doc()->m_bGameOver = false;
	return true;
}

bool CLobbyMain::OnGuildNoticeDlgResult(int result, FrForm* form)
{
	m_pGuildNoticeDlg = NULL;
	if (m_pGuildNoticeObj)
	{
		delete (BaseObject*)m_pGuildNoticeObj;
		m_pGuildNoticeObj = NULL;
	}

	OpenPCBangNoticeForm();
	Doc()->SetNeedFirstLoginAlarm(false);
	return true;
}

bool CLobbyMain::OnRoomUseReportDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			if (((sItemInfo)*it).tid == 0x1a000041)
			{
				it->Common[0]--;
				if (it->Common[0] < 1)
					Doc()->m_myItemList.erase(it);
				break;
			}
		}

		WSendPacket packet(0xa2);
		packet.EncodeBuffer(&Doc()->m_holeStatistics[0],
			sizeof(sPangYaUserStatistics));
		packet.Send(TO_GAME);

		FrWnd* pOk = form->FindChildByName("ok");
		if (pOk)
			pOk->Enable(false);

		Doc()->m_bGameOver = false;
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	else
	{
		if (m_pRoomClose)
			m_pRoomClose->Enable(true);

		OpenGameExtResQuitForm();
	}

	m_pUseReportDlg = NULL;
	return true;
}

bool CLobbyMain::OnRoomExtResQuitDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrWnd* pOk = form->FindChildByName("ok");
		if (pOk)
			pOk->Enable(false);

		WSendPacket packet((enumClientPacket)0xf);
		packet.Encode1(0);
		packet.Encode2(0xffff);

		bool bEncoded = false;
		if (IsLocalContent(S3_CADDIE_REPORT))
		{
			unsigned char gameType = Doc()->m_roomInfo.gameType;
			if (gameType == 4 || gameType == 5)
			{
				for (std::vector<sRivalData>::iterator it =
						 Doc()->m_rivalList.begin();
					it != Doc()->m_rivalList.end(); ++it)
				{
					if (it->oid == MyGuid(false))
					{
						packet.Encode8(it->totalPang);
						packet.Encode8(it->totalBonusPang);
						bEncoded = true;
						break;
					}
				}
			}
		}

		if (!bEncoded)
		{
			packet.Encode8(0);
			packet.Encode8(0);
		}

		packet.Send(TO_GAME);

		Doc()->m_bGameOver = false;
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	else
	{
		if (m_pRoomClose)
			m_pRoomClose->Enable(true);
	}

	m_pExtResQuitDlg = NULL;
	return true;
}

bool CLobbyMain::OnRoomExt30sResQuitDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrWnd* pOk = form->FindChildByName("ok");

		if (pOk)
			pOk->Enable(false);

		WSendPacket packet((enumClientPacket)0xf);
		packet.Encode1(0);
		packet.Encode2(0xffff);

		packet.Encode8(0);
		packet.Encode8(0);

		packet.Send(TO_GAME);

		Doc()->m_bGameOver = false;
		this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	else
	{
		if (m_pRoomClose)
			m_pRoomClose->Enable(true);
	}

	return true;
}

bool CLobbyMain::OnUserListDlgResult(int result, FrForm* form)
{
	m_pUserListDlg = NULL;
	return true;
}

extern bool g_bQuit;

bool CLobbyMain::OnNotifyQuitDlgResult(int result, FrForm* form)
{
	g_bQuit = true;
	return true;
}

bool CLobbyMain::OnLoginDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_pLoginDlg)
		{
			m_pLoginDlg->Login();
			m_pLoginDlg->SetCloseResult(FrNONE);
			return false;
		}
	}
	else
	{
		if (result == 2)
			g_bQuit = true;
		LogOut(0, "OnLoginDlgResult\n");
		m_pLoginDlg = NULL;
	}

	return true;
}

bool CLobbyMain::OnDtRoomDlgResult(int result, FrForm* form)
{
	if (result != 1)
		EnableRoomEnterControls(true);

	m_pDtRoomDlg = NULL;
	return true;
}

bool CLobbyMain::OnVoteDlgResult(int result, FrForm* form)
{
	WSendPacket packet(0x27);

	if (result == 1)
		packet.Encode1(0);
	else
		packet.Encode1(1);

	packet.Send(TO_GAME);

	return true;
}

bool CLobbyMain::OnInviteDlgResult(int result, FrForm* form)
{
	if (m_pInviteDlg == NULL)
		return false;

	if (result == 1)
	{
		if (IsLocalContent(S4_INVITE_FRIEND))
		{
			if (m_pInviteDlg->GetSvrGUID() == Doc()->m_gameServerUID &&
				m_pInviteDlg->GetChannelIdx() == Doc()->m_curChannel.Uid)
			{
				std::list<sRoomInfo>::iterator it =
					std::find(Doc()->m_roomList.begin(),
						Doc()->m_roomList.end(), m_pInviteDlg->GetRoomIdx());
				if (it == Doc()->m_roomList.end())
				{
					this << MsgObject(this, 0x23,
						(int)"\xc3\xca\xb4\xeb\xb9\xde\xc0\xba \xb9\xe6\xc0\xcc \xc1\xf6\xb1\xdd \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
				}
				else
				{
					FrWnd* pConsent = form->FindChildByName("b_consent");
					if (pConsent)
						pConsent->Enable(false);

					EnableRoomEnterControls(false);

					WSendPacket packet((enumClientPacket)9);
					packet.Encode2(m_pInviteDlg->GetRoomIdx());
					packet.EncodeStr("");
					packet.Send(TO_GAME);

					this << MsgObject(NULL, 0x16, 0, 0, 0, 0, 0);
					this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
				}
			}
			else
			{
				sFriend* pFriend =
					Buddy()->GetBuddy(m_pInviteDlg->GetFromUID());

				if (pFriend)
				{
					if (g_pFresh->IsCurrentLayout("RealMyRoom"))
					{
						AfxGetTask()->GetActor("RealMyRoom")
							<< MsgObject(NULL, 0xdf, 0, 0, 0, 0, 0);
					}

					Doc()->m_gameServerList.clear();
					Doc()->m_channelList.clear();

					WSendPacket packet((enumClientPacket)0x43);
					packet.Send(TO_GAME);

					Doc()->SetGoWithModeStart(pFriend->Uid);
					Doc()->SetGoWithMode(2);
				}
			}
		}
	}
	else if (IsLocalContent(S4_INVITE_FRIEND))
	{
		if (m_pInviteDlg->GetSvrGUID() == Doc()->m_gameServerUID &&
			m_pInviteDlg->GetChannelIdx() == Doc()->m_curChannel.Uid)
		{
			Doc()->SetInviteMode(0);
		}
	}

	m_pInviteDlg = NULL;
	return true;
}

bool CLobbyMain::OnItemEquipResult(int result, FrForm* form)
{
	if (result == 1 || result == 2)
	{
		if (m_pEquipDlg)
			memcpy(m_myItemSlot, m_pEquipDlg->GetMyItemSlot(),
				sizeof(m_myItemSlot));

		m_pEquipDlg = NULL;
		HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

		WSendPacket packet(0x20);
		packet.Encode1(2);
		packet.EncodeBuffer(m_myItemSlot, sizeof(m_myItemSlot));
		packet.Send(TO_GAME);
	}

	return true;
}

bool CLobbyMain::OnPasswordDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_pPasswordDlg)
		{
			FrWnd* pWnd = m_pPasswordDlg->FindChildByName("password");

			FrEdit* pEdit = DYNAMIC_CAST(FrEdit, pWnd);

			std::string password;
			if (pEdit)
			{
				const char* text = pEdit->GetLine(1, false);
				password = text;
				HandleMsg(MsgObject(NULL, 0x27, (int)text, 0, 0, 0, 0));
			}

			FrWnd* pOk = m_pPasswordDlg->FindChildByName("ok");
			if (pOk)
				pOk->Enable(false);

			EnableRoomEnterControls(false);

			WSendPacket packet((enumClientPacket)9);
			packet.Encode2(Doc()->m_roomInfo.roomGuid);
			packet.EncodeStr(password);
			packet.Send(TO_GAME);

			HandleMsg(MsgObject(NULL, 0x16, 0, 0, 0, 0, 0));
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		else
		{
			EnableRoomEnterControls(true);
		}
	}
	else
	{
		EnableRoomEnterControls(true);
	}

	m_pPasswordDlg = NULL;
	return true;
}

bool CLobbyMain::OnPCBangDlgResult(int result, FrForm* form)
{
	m_pPCBangNoticeDlg = NULL;

	if (Doc()->m_bShowNotice)
	{
		OnToppage_NoticeUp();
	}

	return true;
}

bool CLobbyMain::OnCreateInfoResult(int result, FrForm* form)
{
	return true;
}

bool CLobbyMain::OnEmoticonResult(int result, FrForm* form)
{
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (pDlg)
		{
			const char* icon = pDlg->GetSelectedIcon();
			if (icon)
			{
				FrEdit* pEdit = m_pChatInput;
				if (m_pReportDlg)
					pEdit = DYNAMIC_CAST(FrEdit,
						m_pReportDlg->FindChildByName("id"));

				if (pEdit)
				{
					const char* front = pEdit->GetEditText_Front();
					const char* comp = pEdit->GetEditText_Comp();
					const char* end = pEdit->GetEditText_End();

					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, icon, end)) <
						pEdit->GetWidthLimit())
						pEdit->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, icon, end), 0,
							false, 0);
					pEdit->SetKeyFocus(true);
				}
			}
		}
	}

	m_pEmoticonDlg = NULL;
	return true;
}

bool CLobbyMain::OnReportDlgResult(int result, FrForm* form)
{
	m_pReportDlg = NULL;
	return true;
}

bool CLobbyMain::OnParanNickChangeResult(int result, FrForm* form)
{
	m_pParanNickDlg = NULL;

	if (Doc()->m_bAutoRefresh)
	{
		Doc()->m_bAutoRefresh = false;
		OpenEventPrizeForm(2);
	}

	OpenCaddieWarningForm();

	return true;
}

bool CLobbyMain::OnCaddieWarningDlgResult(int result, FrForm* form)
{
	m_pCaddieWarningDlg = NULL;
	OpenNoteForm();
	return true;
}

extern bool g_bQuit;

bool CLobbyMain::OnUIHackDlgResult(int result, FrForm* form)
{
	g_bQuit = true;
	return true;
}

bool CLobbyMain::OnMapSelectDlgResult(int result, FrForm* form)
{
	m_pTreasureCourse = NULL;
	return true;
}

bool CLobbyMain::OnHolesEventDlgResult(int result, FrForm* form)
{
	m_pHolesEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnPointEventDlgResult(int result, FrForm* form)
{
	m_pPointEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnWorldTourEventDlgResult(int result, FrForm* form)
{
	m_pWorldTourEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnFindEbortEventDlgResult(int result, FrForm* form)
{
	m_pFindEbortEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnHalloweenEventDlgResult(int result, FrForm* form)
{
	m_pHalloweenEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnChristmasEventDlgResult(int result, FrForm* form)
{
	m_pChristmasEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnChristmasSockEventDlgResult(int result, FrForm* form)
{
	m_pChristmasSockEventDlg = NULL;
	return true;
}

bool CLobbyMain::OnTicketExchangeDlgResult(int result, FrForm* form)
{
	m_pTicketExchangeDlg = NULL;
	return true;
}

bool CLobbyMain::OnLevelupFormResult(int result, FrForm* form)
{
	if (m_levelupForms.size())
	{
		m_levelupForms[m_levelupForms.size() - 1] = NULL;
		m_levelupForms.erase(m_levelupForms.end() - 1);
	}

	return true;
}

bool CLobbyMain::OnFirstTutorialDlgResult(int result, FrForm* form)
{
	return true;
}

bool CLobbyMain::OnQuickStartDlgResult(int result, FrForm* form)
{
	switch (result)
	{
	case 1:
		if (m_pQuickResultDlg == NULL)
		{
			m_pQuickResultDlg = CreateForm<FrQuickResultDlg>(
				g_pFresh->GetManager(), this, "quick_result");
			if (m_pQuickResultDlg)
			{
				m_pQuickResultDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnQuickResultDlgResult,
					0x40);
				m_pQuickResultDlg->SetContents(MakeStr(
					"\xb8\xc5\xc4\xaa \xb4\xeb\xbb\xf3\xc0\xbb \xb0\xcb\xbb\xf6 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9."));
			}
		}
		break;
	}

	m_pQuickStartDlg = NULL;

	return true;
}

bool CLobbyMain::OnQuickResultDlgResult(int result, FrForm* form)
{
	m_pQuickResultDlg = NULL;
	return true;
}

void CLobbyMain::OpenLayout(const char* name)
{
	this << MsgObject(NULL, 0, (int)name, 0, 0, 0, 0);
}

void CLobbyMain::ResetControl()
{
	m_pToppageGame = NULL;
	m_pToppageFamily = NULL;
	m_pToppagePangyaStudy = NULL;
	m_pToppageReserved = NULL;
	m_pOverBarOption = NULL;
	m_pToppageNotice = NULL;
	m_pToppageServer = NULL;
	m_pInviteDlg = NULL;
	m_pUserListDlg = NULL;
	m_pDtRoomDlg = NULL;
	m_pScoreDlg = NULL;
	m_pPasswordDlg = NULL;
	m_pOptionDlg = NULL;
	m_pEmoticonDlg = NULL;
	m_pReportDlg = NULL;
	m_pServerDlg = NULL;
	m_pGuildNoticeDlg = NULL;
	m_pEquipDlg = NULL;
	m_pReservedDlg428 = NULL;
	m_pMailBoxDlg = NULL;
	m_pPCBangNoticeDlg = NULL;
	m_pUniteResultDlg = NULL;
	m_pNewRecordDlg = NULL;
	m_pParanNickDlg = NULL;
	m_pCaddieWarningDlg = NULL;
	m_pExtResQuitDlg = NULL;
	m_pUseReportDlg = NULL;
	m_pRoomClose = NULL;
	m_pMakeRoom = NULL;
	m_pGuildInfo = NULL;
	m_pUserList = NULL;
	m_pRoomList = NULL;
	m_pSelRoom = NULL;
	m_pLanguage = NULL;
	m_pChatInput = NULL;
	m_pChatView = NULL;
	m_pChatTarget = NULL;
	m_pEmoticon = NULL;
	m_pTitle = NULL;
	m_pChatBg = NULL;
	m_pMapBg = NULL;
	m_pRoomOption = NULL;
	m_pRoomUser = NULL;
	m_pRoomUserRank = NULL;
	m_pRoomGuildRank = NULL;
	m_pMap = NULL;
	m_pMapPrev = NULL;
	m_pMapNext = NULL;
	m_pMapHoleCombo = NULL;
	m_pMapGauge = NULL;
	m_pMapLock = NULL;
	m_pBackGround = NULL;
	m_pStart = NULL;
	m_pReady = NULL;
	m_pUnused6dc = NULL;
	m_pScore = NULL;
	m_pGuildInvite = NULL;
	m_pChangeTeam = NULL;
	m_pTeamNum = NULL;
	m_pTeamTab[0] = NULL;
	m_pTeamTab[1] = NULL;
	m_pGuildTab[0] = NULL;
	m_pGuildTab[1] = NULL;
	m_pRoomSortCateAll = NULL;
	m_pRoomSortCateVS = NULL;
	m_pRoomSortCateMass = NULL;
	m_pRoomSortCateBattle = NULL;
	m_pRoomSortCateChat = NULL;
	m_pRoomSortCateVSList = NULL;
	m_pRoomSortCateMassList = NULL;
	m_pRoomSortCateBattleList = NULL;
	m_pUnused65c = NULL;
	m_pUserInfo = NULL;
	m_pReserved6ec = NULL;
	m_pDlg150 = NULL;
	m_pTreasureCourse = NULL;
	m_pOnelineReqDlg = NULL;
	m_pReserved658 = NULL;
	m_pReserved6f0 = NULL;
	m_pExtFrame = NULL;
	m_pUnused6f8 = NULL;
	m_pInGameBmp = NULL;
	m_pMasterSignBmp = NULL;
	m_pReadySignBmp = NULL;
	m_pTeamSelectBmp = NULL;
	m_pDiveBmp = NULL;
	m_pGuildScoreGauge = NULL;
	m_pKickBmp = NULL;
	m_pReservedBmp7f8 = NULL;
	m_pReservedBmp7fc = NULL;
	m_pRoomEventBmp = NULL;
	m_pRoomEvent02Bmp = NULL;
	m_bNewBlink = false;
	m_newBlinkCount = 0;
	m_newBlinkTime = 0.0f;
	memset(m_pPlayerBaseBmp, 0, sizeof(m_pPlayerBaseBmp));
	memset(m_pReservedBmp7ec, 0, sizeof(m_pReservedBmp7ec));
	memset(m_pPet, 0, sizeof(m_pPet));
	memset(m_pEventPrizeDlg, 0, sizeof(m_pEventPrizeDlg));
	memset(m_pStatBar, 0, sizeof(m_pStatBar));
	memset(m_pStatEdit, 0, sizeof(m_pStatEdit));
	m_pGuildNoticeObj = NULL;
	m_roomSortDelay = 0.0f;
	m_userSortDelay = 0.0f;
	m_pCurTip = NULL;
	m_pStartTip = NULL;
	m_pReadyTip = NULL;
	m_pCountdown = NULL;
	m_pToppageEvent1 = NULL;
}

void CLobbyMain::ShowPet(unsigned long uid)
{
	std::map<unsigned long, sRoomSlot>::iterator it =
		Doc()->m_roomSlotMap.find(uid);
	if (it == Doc()->m_roomSlotMap.end())
		return;

	HidePet(uid);

	if (uid ==
		((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1)
				? Doc()->m_myInfo.info.dwGalleryGuid
				: Doc()->m_myInfo.info.dwGuid))
		SetLevelBar();

	sRoomSlot* pSlot = &it->second;
	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM"))
	{
		std::list<sSlotInfo>::iterator itSlot =
			std::find(Doc()->m_slotList.begin(), Doc()->m_slotList.end(), uid);

		if (itSlot == Doc()->m_slotList.end())
			return;

		pSlot->pArea = m_pPet[(*itSlot).connectionRank - 1];
	}
	else if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT"))
	{
		pSlot->pArea = m_pPet[0];
	}

	if (pSlot == NULL || pSlot->pArea == NULL)
		return;

	if (!g_pFresh->GetManager()->IsValidWindow(pSlot->pArea))
		return;

	pSlot->pArea->SetMouseEvent(uid == MyGuid(false));

	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		if (Doc()->m_myInfo.info.dwGalleryGuid == -1)
			return;

		if (!pSlot->partTidList.SetTids(&pSlot->charInfo, 0xff) ||
			!pSlot->partTidList.IsComboValid())
		{
			pSlot->partTidList.m_charTid = pSlot->charInfo.tid;
			pSlot->partTidList.SetDefaultTids();
		}
	}
	else
	{
		if (!pSlot->partTidList.SetTids(&pSlot->charInfo, 0xff) ||
			!pSlot->partTidList.IsComboValid())
		{
			pSlot->partTidList.m_charTid = pSlot->charInfo.tid;
			pSlot->partTidList.SetDefaultTids();
		}

		if (uid == MyGuid(false))
			Doc()->BuildMyPartTidList();
	}

	WRect rect = pSlot->pArea->GetRect();

	pSlot->pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
		rect.y + rect.h * 0.5f, 0.35f, uid != MyGuid(false));
	pSlot->pExhibition->SetDistanceRange(10.0f, 14.0f);
	pSlot->pExhibition->SetDistance(12.0f, true, false);
	pSlot->pExhibition->SetArea(rect.w, rect.h);
	pSlot->pExhibition->SetModel(pSlot->partTidList,
		pSlot->charInfo.tidAuxParts, NULL, 3.0f, uid);

	if (pSlot->bGachaWing)
		pSlot->pExhibition->AttachAngelWingFx();

	if (uid == MyGuid(false))
	{
		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "CREATE"))
			pSlot->pExhibition->SetControlFlag(15);
		else
			pSlot->pExhibition->SetControlFlag(26);
	}
	else
	{
		pSlot->pExhibition->SetControlFlag(16);
	}
}

void CLobbyMain::HidePet(unsigned long uid)
{
	std::map<unsigned long, sRoomSlot>::iterator it =
		Doc()->m_roomSlotMap.find(uid);

	if (it != Doc()->m_roomSlotMap.end())
	{
		if (it->second.pExhibition)
		{
			delete it->second.pExhibition;
			it->second.pExhibition = NULL;
		}
	}
}

void CLobbyMain::EnableRoomEnterControls(bool enable)
{
	if (m_pRoomList)
		m_pRoomList->Enable(enable);

	if (m_pMakeRoom && !(Doc()->m_curChannel.Type & 0x80))
		m_pMakeRoom->Enable(enable);

	if (m_pGuildInfo)
		m_pGuildInfo->Enable(enable);

	if (enable)
	{
		if (m_pDtRoomDlg)
			m_pDtRoomDlg->EnableJoinBtn(true);
	}

	if ((Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe)
	{
		if (m_pGuildInfo)
			m_pGuildInfo->Enable(false);
	}
}

void CLobbyMain::SetStartBtn(bool bMaster)
{
	if (bMaster)
	{
		std::list<sSlotInfo>& slotList = Doc()->m_slotList;

		switch (Doc()->m_roomInfo.gameType)
		{
		case GAME_TYPE_30S:
		case GAME_TYPE_30S_TEAM:
		case GAME_TYPE_GUILD_MATCH:
		case GAME_TYPE_APPROACH:
		case GAME_TYPE_NEW_APPROACH:
		case GAME_TYPE_USEMAX:
			if (NET()->GetConnectionType() == 0 &&
				slotList.size() <
					((Doc()->m_curGameServer.property & 2)
							? 5
							: Doc()->m_gameTypeInfo[GAME_TYPE_30S].minPlayer) &&
				!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
			{
				m_bCanStart = true;
				for (std::list<sSlotInfo>::iterator it = slotList.begin();
					it != slotList.end(); ++it)
				{
					if (!(*it).bReady && !(*it).bMaster)
					{
						m_bCanStart = false;
						break;
					}
				}
			}
			else
			{
				m_bCanStart = true;
				for (std::list<sSlotInfo>::iterator it = slotList.begin();
					it != slotList.end(); ++it)
				{
					if (!(*it).bReady && !(*it).bMaster)
					{
						m_bCanStart = false;
						break;
					}
				}
			}
			break;

		default:
			if (slotList.size() == 1 &&
				!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
			{
				m_bCanStart = false;
			}
			else
			{
				m_bCanStart = true;

				if (Doc()->m_roomInfo.gameType == GAME_TYPE_TEAM)
				{
					if (Doc()->m_roomInfo.nUserLimit > slotList.size())
						m_bCanStart = false;

					if (m_bCanStart)
					{
						int nRed = 0;
						int nBlue = 0;
						for (std::list<sSlotInfo>::iterator it =
								 slotList.begin();
							it != slotList.end(); ++it)
						{
							if ((*it).bTeam == 0)
								nRed++;
							else if ((*it).bTeam == 1)
								nBlue++;
						}

						if (nRed != nBlue)
							m_bCanStart = false;
					}
				}

				if (m_bCanStart)
				{
					for (std::list<sSlotInfo>::iterator it = slotList.begin();
						it != slotList.end(); ++it)
					{
						if (!(*it).bReady && !(*it).bMaster)
						{
							m_bCanStart = false;
							break;
						}
					}
				}
			}
			break;
		}

		if (m_bCanStart && slotList.size() > 1)
		{
			if (m_pCurTip)
			{
				m_bNewBlink = true;
				m_newBlinkTime = 0.0f;
			}

			if (m_pTreasureCourse)
				m_pTreasureCourse->Close(true);
		}
		else
		{
			m_bNewBlink = false;
			if (m_pCurTip)
				m_pCurTip->SetVisible(false);
		}
	}
}

void CLobbyMain::SetRoomControls(bool bMaster)
{
	unsigned char nUser = (unsigned char)Doc()->m_slotList.size();
	bool bLock = false;

	if (Doc()->m_curChannel.Type & 0x800)
	{
		IFF_STRUCT::sCourse* pCourse =
			ItemManager()->FindCourse(((Doc()->m_roomInfo.mapType > 0x7f &&
										   Doc()->m_roomInfo.mapType != 0xfd)
											  ? Doc()->m_roomInfo.mapType - 0x80
											  : Doc()->m_roomInfo.mapType) |
				0x28000000);

		if (pCourse && pCourse->Difficulty >= 3)
			bLock = true;
	}

	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
		bLock = false;

	if (m_pRoomOption)
	{
		if (bMaster)
		{
			m_pRoomOption->Enable(
				(nUser == 1 || !m_bCanStart ||
					(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1)) &&
				Doc()->m_roomInfo.realGameType != 14);
		}
		else
		{
			m_pRoomOption->Enable(false);
		}
	}

	if (m_pUnused65c)
	{
		if ((Doc()->m_curChannel.Type & 0x80) ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH ||
			Doc()->m_roomInfo.realGameType == 14)
		{
			m_pUnused65c->Enable(false);
		}
		else
		{
			if (bMaster)
				m_pUnused65c->Enable(nUser == 1 || !m_bCanStart ||
					(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1));
			else
				m_pUnused65c->Enable(false);
		}
	}

	if (m_pMapPrev)
	{
		if (bMaster)
		{
			bool bVisible =
				(nUser == 1 || !m_bCanStart ||
					(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1)) &&
				Doc()->m_roomInfo.realGameType != 14;
			m_pMapPrev->SetVisible(bVisible);

			if ((Doc()->m_curChannel.Type & 0x80) ||
				Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
				m_pMapPrev->Enable(false);
			else
				m_pMapPrev->Enable(Doc()->m_roomInfo.mapType != FirstMap());
		}
		else
		{
			m_pMapPrev->SetVisible(false);
		}
	}

	if (m_pMapNext)
	{
		if (bMaster)
		{
			bool bVisible =
				(nUser == 1 || !m_bCanStart ||
					(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1)) &&
				Doc()->m_roomInfo.realGameType != 14;
			m_pMapNext->SetVisible(bVisible);

			if ((Doc()->m_curChannel.Type & 0x80) ||
				Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
				m_pMapNext->Enable(false);
			else
				m_pMapNext->Enable(Doc()->m_roomInfo.mapType < LastMap(false));
		}
		else
		{
			m_pMapNext->SetVisible(false);
		}
	}

	if (m_pMapHoleCombo)
	{
		if (bLock || (Doc()->m_curChannel.Type & 0x90) ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH ||
			Doc()->m_roomInfo.realGameType == 14)
		{
			m_pMapHoleCombo->Enable(false);
		}
		else
		{
			if (bMaster)
				m_pMapHoleCombo->Enable(nUser == 1 || !m_bCanStart ||
					(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1));
			else
				m_pMapHoleCombo->Enable(false);
		}
	}

	if (m_pMapLock)
		m_pMapLock->SetVisible(bLock);

	if (m_pStart)
	{
		if (bMaster)
		{
			m_pStart->SetVisible(true);
			m_pStart->Enable(true);

			if (m_pReady)
			{
				m_pReady->SetVisible(false);

				if (m_pReady)
					m_pReady->Enable(false);
			}

			EnableSettingButton(true);

			if (IsLocalContent(S3_HUNDRED_MODE) &&
				Doc()->m_roomInfo.nUserLimit >= 100)
			{
				if (Doc()->m_myInfo.info.dwIdentity & 4)
					m_pStart->Enable(true);
				else
					m_pStart->Enable(false);
			}
			else
			{
				m_pStart->Enable(Doc()->m_slotList.size() >= 1);
			}
		}
		else if (!m_bRoomStateReq)
		{
			m_pReady->SetButtonImg(m_bReady ? "btn_ready_cancel_n"
											: "btn_ready_n",
				FrButton::NORMAL);
			m_pReady->SetButtonImg(m_bReady ? "btn_ready_cancel_o"
											: "btn_ready_o",
				FrButton::OVER);

			m_pReady->SetVisible(true);
			m_pReady->Enable(true);
			m_pStart->SetVisible(false);
			m_pStart->Enable(false);

			m_pStart->Enable(true);
		}
	}

	if (m_pRoomClose)
	{
		if (bMaster)
		{
			m_pRoomClose->Enable(true);
		}
		else if (!m_bRoomStateReq)
		{
			if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
				m_pRoomClose->Enable(true);
			else
				m_pRoomClose->Enable(!m_bReady);
		}
	}

	if (m_pUnused6dc)
	{
		if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
		{
			m_pUnused6dc->Enable(
				(Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe &&
				Doc()->m_myInfo.info.dwGalleryGuid != -1);
		}
		else
		{
			if (bMaster)
				m_pUnused6dc->Enable(true);
			else
				m_pUnused6dc->Enable(!m_bReady);
		}

		if (Doc()->m_curChannel.Type & 0x80)
			m_pUnused6dc->Enable(false);

		if (Doc()->m_curChannel.Type & 8)
			m_pUnused6dc->Enable(false);
	}

	SetRoomCharControl((eBtnDir)2);
	SetRoomCaddieControl((eBtnDir)2);

	if (Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
	{
		if (m_pUnused65c)
			m_pUnused65c->Enable(false);
		if (m_pMapPrev)
			m_pMapPrev->Enable(false);
		if (m_pMapNext)
			m_pMapNext->Enable(false);
		if (m_pMapHoleCombo)
			m_pMapHoleCombo->Enable(false);

		if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
			RemakeTeam(true);
	}

	if (IsLocalContent(S4_INVITE_FRIEND))
		CMessengerInfo::Instance()->EnableInvite(bMaster);
}

void CLobbyMain::SetRoomCharControl(eBtnDir dir)
{
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		if (Doc()->m_myInfo.info.dwGalleryGuid == -1)
		{
			if (m_pUnused6f8)
				m_pUnused6f8->SetVisible(false);
		}
		else
		{
			if (m_pUnused6f8)
			{
				m_pUnused6f8->SetVisible(true);

				IFF_STRUCT::sChar* pChar =
					ItemManager()->FindChar(Doc()->m_userInfo[0].charInfo.tid);
				if (pChar)
					m_pUnused6f8->SetButtonImg(pChar->c.Icon, FrButton::NORMAL);
			}
		}
	}

	if (!m_bReady && !(bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		std::map<unsigned int, sCharacterInfo>& charMap = Doc()->m_charMap;
		std::map<unsigned int, sCharacterInfo>::const_iterator it =
			charMap.find(Doc()->m_myInfo.userEquip.guidChar);

		if (it == charMap.end())
			return;

		switch (dir)
		{
		case 0:
			if (it == charMap.begin())
				break;

			if (--it != charMap.end())
			{
				WSendPacket packet((enumClientPacket)0xc);
				packet.Encode1(4);
				packet.Encode4((*it).second.guid);
				packet.Send(TO_GAME);
			}
			break;

		case 1:
			if (++it != charMap.end())
			{
				WSendPacket packet((enumClientPacket)0xc);
				packet.Encode1(4);
				packet.Encode4((*it).second.guid);
				packet.Send(TO_GAME);
			}
			break;
		}
	}
}

void CLobbyMain::SetRoomCaddieControl(eBtnDir dir)
{
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		if (Doc()->m_myInfo.info.dwGalleryGuid == -1)
			this << MsgObject(NULL, 0x13, 0, 0, 0, 0, 0);
		else
			this << MsgObject(NULL, 0x13, Doc()->m_userInfo[0].caddieInfo.tid,
				0, 0, 0, 0);
	}

	std::map<unsigned int, sCaddieInfo>& caddies = Doc()->m_caddieMap;
	if (caddies.size() == 0)
	{
		Doc()->m_myInfo.userEquip.guidCaddie = 0;
		this << MsgObject(NULL, 0x13, 0, 0, 0, 0, 0);
		return;
	}

	unsigned long guid = Doc()->m_myInfo.userEquip.guidCaddie;

	std::map<unsigned int, sCaddieInfo>& caddieMap = Doc()->m_caddieMap;
	std::map<unsigned int, sCaddieInfo>::const_iterator it =
		caddieMap.find(guid);

	switch (dir)
	{
	case 0:
	{
		if (it != caddieMap.begin())
		{
			if (--it != caddieMap.end())
			{
				Doc()->m_myInfo.userEquip.guidCaddie = (*it).second.guid;
				this << MsgObject(NULL, 0x13, (*it).second.tid, 0, 0, 0, 0);
				SetLevelBar();
				WSendPacket packet((enumClientPacket)0xc);
				packet.Encode1(1);
				packet.Encode4((*it).second.guid);
				packet.Send(TO_GAME);
				break;
			}
		}
		Doc()->m_myInfo.userEquip.guidCaddie = 0;
		this << MsgObject(NULL, 0x13, 0, 0, 0, 0, 0);
		SetLevelBar();
		WSendPacket packet((enumClientPacket)0xc);
		packet.Encode1(1);
		packet.Encode4(0);
		packet.Send(TO_GAME);
		break;
	}
	case 1:
		if (Doc()->m_myInfo.userEquip.guidCaddie == 0)
			it = caddieMap.begin();
		else
			++it;

		if (it == caddieMap.end())
			break;

		Doc()->m_myInfo.userEquip.guidCaddie = (*it).second.guid;
		this << MsgObject(NULL, 0x13, (*it).second.tid, 0, 0, 0, 0);

		SetLevelBar();

		{
			WSendPacket packet((enumClientPacket)0xc);
			packet.Encode1(1);
			packet.Encode4((*it).second.guid);
			packet.Send(TO_GAME);
		}
		break;

	case 2:
		if (it == caddieMap.end())
		{
			Doc()->m_myInfo.userEquip.guidCaddie = 0;
			this << MsgObject(NULL, 0x13, 0, 0, 0, 0, 0);
		}
		else
		{
			Doc()->m_myInfo.userEquip.guidCaddie = (*it).second.guid;
			this << MsgObject(NULL, 0x13, (*it).second.tid, 0, 0, 0, 0);
		}
		break;
	}
}

void CLobbyMain::MakeRoomUserList()
{
	if (m_pRoomUser == NULL)
		return;

	switch (Doc()->m_roomInfo.gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		m_pRoomUser->ClearItem();

		if (Doc()->m_bGameOver)
		{
			int nNotFinish = 0;
			if (Doc()->m_rivalList.size() > 0)
			{
				for (std::list<sSlotInfo>::iterator it =
						 Doc()->m_slotList.begin();
					it != Doc()->m_slotList.end(); ++it)
				{
					sSlotInfo* pSlot = &(*it);

					unsigned char index = Doc()->GetIndex(pSlot->dwGuid);

					if (Doc()->m_rivalList[index].state == 2)
						m_pRoomUser->AddItem(pSlot);
					else if (Doc()->m_rivalList[index].state == 0)
						nNotFinish++;
				}
			}

			for (int i = m_pRoomUser->GetCurrentItemSize(true); i < 24; i++)
				m_pRoomUser->AddItem(NULL);

			m_pRoomUser->SortItem(FinishUserCompare);

			if (nNotFinish == 0)
				PostMsg(this, "Lobby", 0x2d, 0, 0, 0, 0);
		}
		else
		{
			for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				it != Doc()->m_slotList.end(); ++it)
				m_pRoomUser->AddItem(&(*it));

			for (int i = m_pRoomUser->GetCurrentItemSize(true); i < 24; i++)
				m_pRoomUser->AddItem(NULL);

			if (Doc()->m_slotList.size() > 1)
				m_pRoomUser->SortItem(ConnectionUserCompare);
		}
		break;

	case GAME_TYPE_GUILD_MATCH:
		if (Doc()->m_bGameOver)
		{
			m_redTeam.clear();
			m_blueTeam.clear();

			for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				it != Doc()->m_slotList.end(); ++it)
			{
				unsigned char index = Doc()->GetIndex((*it).dwGuid);

				if (Doc()->m_rivalList[index].state == 2)
				{
					sSlotInfo* pSlot = &(*it);
					if (pSlot->GuildId ==
						Doc()->m_roomInfo.GuildInfo.nGuildID[0])
						m_redTeam.push_back(&(*it));
					else if (pSlot->GuildId ==
						Doc()->m_roomInfo.GuildInfo.nGuildID[1])
						m_blueTeam.push_back(&(*it));
				}
			}

			m_redTeam.sort(FinishUserCompare);
			m_blueTeam.sort(FinishUserCompare);

			RemakeTeam(false);
		}
		else
		{
			RemakeTeam(true);
		}
		break;

	case GAME_TYPE_30S_TEAM:
		if (Doc()->m_bGameOver)
		{
			m_redTeam.clear();
			m_blueTeam.clear();

			int nNotFinish = 0;
			for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				it != Doc()->m_slotList.end(); ++it)
			{
				unsigned char index = Doc()->GetIndex((*it).dwGuid);

				if (Doc()->m_rivalList[index].state == 2)
				{
					sSlotInfo* pSlot = &(*it);
					if (pSlot->bTeam == 0)
						m_redTeam.push_back(pSlot);
					else
						m_blueTeam.push_back(pSlot);
				}
				else if (Doc()->m_rivalList[index].state == 0)
				{
					nNotFinish++;
				}
			}

			m_redTeam.sort(FinishUserCompare);
			m_blueTeam.sort(FinishUserCompare);

			RemakeTeam(false);

			if (nNotFinish == 0)
				PostMsg(this, "Lobby", 0x2d, 0, 0, 0, 0);
		}
		else
		{
			RemakeTeam(true);
		}
		break;

	default:
		m_pRoomUser->ClearItem();

		for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
			it != Doc()->m_slotList.end(); ++it)
			m_pRoomUser->AddItem(&(*it));

		for (unsigned char n = m_pRoomUser->GetCurrentItemSize(true); n < 4;
			n++)
			m_pRoomUser->AddItem(NULL);

		if (Doc()->m_slotList.size() > 1)
			m_pRoomUser->SortItem(ConnectionUserCompare);
		break;
	}

	switch (Doc()->m_roomInfo.gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
	case GAME_TYPE_USEMAX:
		if (!m_bInitSlot)
		{
			sRoomSlot slot;
			slot.pArea = m_pPet[0];
			slot.pExhibition = NULL;
			slot.partTidList = Doc()->GetMyPartTidList();
			slot.charInfo =
				Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
			slot.bAngelWing = Doc()->m_myInfo.info.angelicWings;
			slot.bGachaWing = Doc()->m_myInfo.info.angelicWingsEffect;
			Doc()->m_roomSlotMap[MyGuid(false)] = slot;

			ShowPet(MyGuid(false));
		}
		break;
	}
}

void CLobbyMain::RemakeTeam(bool bRemake)
{
	if (m_pRoomUser == NULL)
		return;

	if (bRemake)
	{
		m_redTeam.clear();
		m_blueTeam.clear();

		for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
			it != Doc()->m_slotList.end(); it++)
		{
			if (Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
			{
				if (it->GuildId == Doc()->m_roomInfo.GuildInfo.nGuildID[0])
					m_redTeam.push_back(&(*it));
				else if (it->GuildId == Doc()->m_roomInfo.GuildInfo.nGuildID[1])
					m_blueTeam.push_back(&(*it));
			}
			else
			{
				if (it->bTeam == 0)
					m_redTeam.push_back(&(*it));
				else
					m_blueTeam.push_back(&(*it));
			}
		}

		if (m_redTeam.size() == 0)
		{
			Doc()->m_roomInfo.GuildInfo.Reset(0);
		}

		if (m_blueTeam.size() == 0)
		{
			Doc()->m_roomInfo.GuildInfo.Reset(1);
		}
	}

	m_pRoomUser->ClearItem();

	int count = Max(m_redTeam.size(), m_blueTeam.size());
	if (count < 12)
		count = 12;

	std::list<sSlotInfo*>::iterator itRed = m_redTeam.begin();
	int redCount = 0;
	int blueCount = 0;
	std::list<sSlotInfo*>::iterator itBlue = m_blueTeam.begin();
	for (int i = 0; i < count; i++)
	{
		if (itRed == m_redTeam.end())
			m_pRoomUser->AddItem(NULL);
		else
		{
			m_pRoomUser->AddItem(*itRed);
			itRed++;
			redCount++;
		}

		if (itBlue == m_blueTeam.end())
			m_pRoomUser->AddItem(NULL);
		else
		{
			m_pRoomUser->AddItem(*itBlue);
			itBlue++;
			blueCount++;
		}
	}

	unsigned long guid = Doc()->m_myInfo.info.dwGuid;
	std::list<sSlotInfo>::iterator it =
		std::find(Doc()->m_slotList.begin(), Doc()->m_slotList.end(), guid);

	if (it != Doc()->m_slotList.end())
	{
		if (m_pGuildInvite)
		{
			if (bRemake == true &&
				strcmp(g_pFresh->GetManager()->GetLayoutID(),
					"GAMEROOM_EXTRES") != 0)
			{
				if ((it->bMaster) &&
					((blueCount < 1 && redCount > 2) ||
						(blueCount > 2 && redCount < 1)))
				{
					m_pGuildInvite->Enable(true);
				}
				else
				{
					m_pGuildInvite->Enable(false);
				}
			}
		}
	}
}

void CLobbyMain::SortRank()
{
	if (Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); it++)
		{
			if (it->state == 3)
			{
				it->rank = Doc()->m_rivalList.size() - it->quitOrder;
			}
			else
			{
				it->rank = 1;
				for (std::vector<sRivalData>::iterator it2 =
						 Doc()->m_rivalList.begin();
					it2 != Doc()->m_rivalList.end(); it2++)
				{
					if (it == it2 || it2->state == 3)
						continue;

					if (it->approachResultDistance >
							it2->approachResultDistance ||
						(it->approachResultDistance ==
								it2->approachResultDistance &&
							it->approachTime < it2->approachTime))
						it->rank++;
				}
			}
		}

		if (m_pRoomUserRank)
			m_pRoomUserRank->SortItem(ApproachRankUserCompare);
	}
	else
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); it++)
		{
			if (it->state == 3)
				continue;

			it->rank = 1;
			for (std::vector<sRivalData>::iterator it2 =
					 Doc()->m_rivalList.begin();
				it2 != Doc()->m_rivalList.end(); it2++)
			{
				if (it == it2 || it2->state == 3)
					continue;

				if (it->totalScore > it2->totalScore ||
					(it->totalScore == it2->totalScore &&
						it->totalPang < it2->totalPang))
					it->rank++;
			}
		}

		if (m_pRoomUserRank)
			m_pRoomUserRank->SortItem(RankUserCompare);
	}

	if (m_pExtFrame)
	{
		if (Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM)
		{
			m_pExtFrame->SetDesc(MakeStr(
				"RED %d\xc1\xa1/%I64d\xc6\xce  vs  BLUE %d\xc1\xa1/%I64d\xc6\xce",
				Doc()->m_teamResult[0].score, Doc()->m_teamResult[0].pang,
				Doc()->m_teamResult[1].score, Doc()->m_teamResult[1].pang));
		}
	}
}

void CLobbyMain::SetLevelBar()
{
	unsigned long tidChar;
	const char* pStat;
	const unsigned long* pParts;
	const unsigned long* pAuxParts;
	unsigned long tidClub;
	const short* pClubStat = NULL;
	unsigned long tidCaddie;
	unsigned char level = 0;

	if (g_pFresh->IsCurrentLayout("CREATE"))
	{
		tidChar = m_createCharType | 0x4000000;
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tidChar);
		pStat = pChar->PCL;
		pParts = NULL;
		pAuxParts = NULL;
		tidClub = 0;
		tidCaddie = 0;
	}
	else
	{
		if (!g_pFresh->IsCurrentLayout("GAMEROOM") &&
			!g_pFresh->IsCurrentLayout("GAMEROOM_EXT"))
			return;

		bool bGM = (Doc()->m_myInfo.info.dwIdentity >> 1) & 1;
		if (bGM)
		{
			tidChar = Doc()->m_userInfo[0].charInfo.tid;
			pStat = Doc()->m_userInfo[0].charInfo.PCL;
			pParts = Doc()->m_userInfo[0].charInfo.tidParts;
			pAuxParts = Doc()->m_userInfo[0].charInfo.tidAuxParts;
			tidClub = Doc()->m_userInfo[0].clubInfo.tid;
			pClubStat = Doc()->m_userInfo[0].clubInfo.PCL;
			tidCaddie = Doc()->m_userInfo[0].caddieInfo.tid;
			level = Doc()->m_userInfo[0].stat.Level;
		}
		else
		{
			unsigned long guidCaddie = Doc()->m_myInfo.userEquip.guidCaddie;

			std::map<unsigned int, sCharacterInfo>::iterator itChar;
			std::map<unsigned int, sCaddieInfo>::iterator itCaddie;

			itChar = Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
			tidChar = itChar->second.tid;
			pStat = itChar->second.PCL;
			pParts = itChar->second.tidParts;
			pAuxParts = itChar->second.tidAuxParts;

			std::map<unsigned int, sItemInfo>::iterator itClub =
				Doc()->m_clubSetMap.find(Doc()->m_myInfo.userEquip.guidClubSet);
			tidClub = itClub->second.tid;
			pClubStat = itClub->second.Common;

			if (guidCaddie == 0)
				tidCaddie = 0;
			else
			{
				itCaddie = Doc()->m_caddieMap.find(guidCaddie);
				tidCaddie = itCaddie->second.tid;
			}

			level = Doc()->m_myInfo.stat.Level;
		}
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		CCardManager::Instance()->SetPlayerIndex(255);
		CCardManager::Instance()->CalcCardPeriodAndStatus();
	}

	for (unsigned char i = 0; i < 5; i++)
	{
		status_t status =
			ItemManager()->GetLevelWithPenalty((eLvlType)i, tidChar, pStat,
				pParts, pAuxParts, tidClub, pClubStat, tidCaddie, level);
		int nLevel = status.level;
		int nPenalty = status.penalty;
		unsigned char nCapacity = ItemManager()->GetCapacity((eLvlType)i,
			tidChar, pStat, pParts, tidClub, pClubStat, tidCaddie, level);

		m_pStatBar[i]->SetDestPos(nLevel, false);
		m_pStatBar[i]->SetExpand(nCapacity - nLevel);
		if (m_pStatEdit[i])
			m_pStatEdit[i]->SetCaption11(
				MakeStatusString3(nLevel, nCapacity, nPenalty));
	}
}

void CLobbyMain::OpenInformation(unsigned long typeId)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (pWnd)
		return;

	__rtti_obj = NULL;
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "information");

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

	FrArea* pPortrait =
		DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pPortrait)
	{
		if (pItem)
			pPortrait->SetBgImg(pItem->Icon);
		else
			pPortrait->SetBgImg("hide");
	}

	if (pItem)
	{
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
		if (pName && pLevel)
		{
			if (pItem->IsUnderLvl)
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));

				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xc7\xcf",
						pItem->Name),
					false, true);
			}
			else
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));

				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xbb\xf3",
						pItem->Name),
					false, true);
			}
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->TypeId);
		if (pDesc)
			pForm->SetMessage(pDesc->Desc, false);
	}
	else
	{
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		if (pName)
			pName->AddText(
				"\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\n\n\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed",
				false, true);

		pForm->SetMessage(
			"\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7",
			false);
	}

	pForm->Open(NULL, FrESCAPE | FrENTER);
}

void CLobbyMain::OpenCreateInfo(unsigned long typeId, const char* text)
{
	std::string message;
	if (text)
		message = text;

	FrWnd* pWnd = g_pFresh->GetDesktop()->FindChildByName("information_large");
	if (pWnd == NULL)
		;
	else
		return;

	__rtti_obj = NULL;
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "information_large");

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

	FrArea* pPortrait =
		DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pItem && pPortrait)
		pPortrait->SetBgImg(pItem->Icon);

	FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
	FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
	if (pItem)
	{
		if (pName && pLevel)
		{
			if (pItem->IsUnderLvl)
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xc7\xcf",
						pItem->Name),
					false, true);
			}
			else
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xbb\xf3",
						pItem->Name),
					false, true);
			}
		}

		if (text == NULL)
		{
			IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->TypeId);
			if (pDesc)
			{
				if (message[0])
					message += "\n\n";

				message += "\\c0xffff0000\\c";
				message += pDesc->Desc;
			}
		}
	}

	pForm->SetMessage(message.c_str(), false);
	pForm->Open(text ? (FRESH_PFN_RESULT)&CLobbyMain::OnCreateInfoResult : NULL,
		FrESCAPE | FrENTER);
}

void CLobbyMain::OpenCaddieInfo(const sCaddieInfo& info)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (pWnd)
		return;

	__rtti_obj = NULL;
	FrCaddieInfoDlg* pDlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(),
		this, "caddie_info");
	if (pDlg)
	{
		pDlg->SetCaddieInfo(info);
		pDlg->Open(NULL, FrESCAPE | FrENTER);
	}
}

int CLobbyMain::GetMedalIndex(unsigned long uid)
{
	if (Doc()->m_rivalList.size() < 10)
		return 3;

	unsigned char index = Doc()->GetIndex(uid);
	if (index == 0xff || Doc()->m_rivalList[index].flag > 3)
		return 3;

	unsigned char rank = Doc()->m_rivalList[index].rank;
	unsigned char memberCount = 0;
	unsigned char upperCount = 0;
	unsigned char intrusionCount = 0;

	if (IsLocalContent(S3_INTRUSION))
		intrusionCount = CIntrusion::Instance()->GetCalcExpMemberSize();

	if (intrusionCount == 0)
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); it++)
		{
			if ((it->state == 3 && GetHoleIndex(it->hole) >= 4) ||
				it->state != 3)
				memberCount++;

			if (it->state != 3 && it->rank < rank && it->flag > 3)
				upperCount++;
		}
	}
	else
	{
		memberCount = intrusionCount;
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); it++)
		{
			if (it->state != 3 && it->rank < rank && it->flag > 3)
				upperCount++;
		}
	}

	rank -= upperCount;

	if (Doc()->m_golfGame.holes == 9)
	{
		if (memberCount <= 14)
			return 3;
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 1;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 1;
			if (rank == 2)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
		}
	}
	else if (Doc()->m_golfGame.holes == 18)
	{
		if (memberCount <= 14)
		{
			if (rank == 1)
				return 2;
		}
		else if (memberCount <= 18)
		{
			if (rank == 1)
				return 1;
			if (rank == 2)
				return 2;
		}
		else if (memberCount <= 22)
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
			if (rank == 3)
				return 2;
		}
		else if (memberCount <= 26)
		{
			if (rank == 1)
				return 0;
			if (rank == 2)
				return 1;
			if (rank <= 4)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			if (rank <= 3)
				return 1;
			if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

bool CLobbyMain::LatestVersion()
{
	if (Doc()->m_packetNickname > "645.00")
	{
		this << MsgObject(this, 35,
			(int)"\xc3\xd6\xbd\xc5 \xb9\xf6\xc1\xaf\xc0\xb8\xb7\xce \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xb0\xa1 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return false;
	}

	return true;
}

bool RoomStateCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	if (pRoom1->bPublic == false && pRoom2->bPublic == true)
		return false;

	if (pRoom1->bPublic == true && pRoom2->bPublic == false)
		return true;

	if (pRoom1->bAvailable == false && pRoom2->bAvailable == true)
		return false;

	if (pRoom1->bAvailable == true && pRoom2->bAvailable == false)
		return true;

	if (pRoom1->bAvailable == true && pRoom2->bAvailable == true)
	{
		if (pRoom1->nUserNum == pRoom1->nUserLimit &&
			pRoom2->nUserNum < pRoom2->nUserLimit)
			return false;

		if (pRoom1->nUserNum < pRoom1->nUserLimit &&
			pRoom2->nUserNum == pRoom2->nUserLimit)
			return true;

		if (pRoom1->bSleep == true && pRoom2->bSleep == false)
			return false;

		if (pRoom1->bSleep == false && pRoom2->bSleep == true)
			return true;

		if (pRoom1->bIntrusion == false && pRoom2->bIntrusion == true)
			return false;

		if (pRoom1->bIntrusion == true && pRoom2->bIntrusion == false)
			return true;
	}

	return false;
}

bool RoomStateRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
	{
		if (pRoom2->bPublic == false && pRoom1->bPublic == true)
			return true;

		return true;
	}

	if (pRoom2->bPublic == false && pRoom1->bPublic == true)
		return false;

	if (pRoom2->bPublic == true && pRoom1->bPublic == false)
		return true;

	if (pRoom2->bAvailable == false && pRoom1->bAvailable == true)
		return false;

	if (pRoom2->bAvailable == true && pRoom1->bAvailable == false)
		return true;

	if (pRoom2->bAvailable == true && pRoom1->bAvailable == true)
	{
		if (pRoom2->nUserNum == pRoom2->nUserLimit &&
			pRoom1->nUserNum < pRoom1->nUserLimit)
			return false;

		if (pRoom2->nUserNum < pRoom2->nUserLimit &&
			pRoom1->nUserNum == pRoom1->nUserLimit)
			return true;

		if (pRoom2->bSleep == true && pRoom1->bSleep == false)
			return false;

		if (pRoom2->bSleep == false && pRoom1->bSleep == true)
			return true;

		if (pRoom2->bIntrusion == false && pRoom1->bIntrusion == true)
			return false;

		if (pRoom2->bIntrusion == true && pRoom1->bIntrusion == false)
			return true;
	}

	return false;
}

bool RoomNoCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->roomGuid < pRoom2->roomGuid;
}

bool RoomNoRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->roomGuid > pRoom2->roomGuid;
}

bool RoomHoleCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->nHole < pRoom2->nHole;
}

bool RoomHoleRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->nHole > pRoom2->nHole;
}

bool RoomCourseCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->mapType < pRoom2->mapType;
}

bool RoomCourseRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit != 100 && pRoom2->nUserLimit == 100)
		return false;

	if (pRoom1->nUserLimit == 100 && pRoom2->nUserLimit != 100)
		return true;

	return pRoom1->mapType > pRoom2->mapType;
}

bool RoomTitleCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	return lstrcmpi(pRoom1->title, pRoom2->title) < 0;
}

bool RoomTitleRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	return lstrcmpi(pRoom1->title, pRoom2->title) > 0;
}

bool RoomGameTypeCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->gameType == pRoom2->gameType)
		return pRoom1->tidMatch > pRoom2->tidMatch;

	return pRoom1->gameType < pRoom2->gameType;
}

bool RoomGameTypeRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->gameType == pRoom2->gameType)
		return pRoom1->tidMatch > pRoom2->tidMatch;

	return pRoom1->gameType > pRoom2->gameType;
}

bool RoomUserCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit > pRoom2->nUserLimit)
		return false;

	if (pRoom1->nUserLimit < pRoom2->nUserLimit)
		return true;

	return pRoom1->nUserNum <= pRoom2->nUserNum ? true : false;
}

bool RoomUserRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)b;

	if (pRoom1->nUserLimit > pRoom2->nUserLimit)
		return true;

	if (pRoom1->nUserLimit < pRoom2->nUserLimit)
		return false;

	return pRoom1->nUserNum <= pRoom2->nUserNum ? true : false;
}

bool RoomGuildCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sRoomInfo* pRoom1 = (const sRoomInfo*)a;
	const sRoomInfo* pRoom2 = (const sRoomInfo*)a;

	int g0 = pRoom1->GuildInfo.nGuildID[0];
	int g1 = pRoom1->GuildInfo.nGuildID[1];
	int myGuild = Doc()->m_myInfo.info.dwGuildId;

	return min(Abs(g0 - myGuild), Abs(g1 - myGuild)) >
		min(Abs(g0 - myGuild), Abs(g1 - myGuild));
}

bool UserGenderCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->gender < pUser2->gender;
}

bool UserGenderRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->gender > pUser2->gender;
}

bool UserLevelCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->level < pUser2->level;
}

bool UserLevelRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->level > pUser2->level;
}

bool UserGuildCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->m_GuildId < pUser2->m_GuildId;
}

bool UserGuildRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return pUser1->m_GuildId > pUser2->m_GuildId;
}

bool UserNicknameCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return lstrcmpi(pUser1->sNick, pUser2->sNick) < 0;
}

bool UserNicknameRevCompare(const void* a, const void* b)
{
	if (a == NULL)
		return false;

	if (b == NULL)
		return true;

	const sBriefUserInfo* pUser1 = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pUser2 = (const sBriefUserInfo*)b;

	return lstrcmpi(pUser1->sNick, pUser2->sNick) > 0;
}

void CLobbyMain::RoomList_LoadRoomImages()
{
	m_pRoomListBoxBmp[0][0] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_n_01");
	m_pRoomListBoxBmp[1][0] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_n_02");
	m_pRoomListBoxBmp[2][0] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_n_03");
	m_pRoomListBoxBmp[0][1] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_o_01");
	m_pRoomListBoxBmp[1][1] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_o_02");
	m_pRoomListBoxBmp[2][1] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_o_03");
	m_pRoomListBoxBmp[0][2] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_d_01");
	m_pRoomListBoxBmp[1][2] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_d_02");
	m_pRoomListBoxBmp[2][2] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_d_03");
	m_pRoomListBoxBmp[0][3] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_e_01");
	m_pRoomListBoxBmp[1][3] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_e_02");
	m_pRoomListBoxBmp[2][3] =
		(Bitmap*)g_pFresh->RegisterBitmap("roomlistbox_e_03");

	m_pRoomSortNoBmp[0] = (Bitmap*)g_pFresh->RegisterBitmap("room_sort_no_n");
	m_pRoomSortNoBmp[1] = (Bitmap*)g_pFresh->RegisterBitmap("room_sort_no_o");
	m_pRoomSortNoBmp[2] = (Bitmap*)g_pFresh->RegisterBitmap("room_sort_no_d");
	m_pRoomSortCateBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_cate_n_down");
	m_pRoomSortCateBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_cate_o");
	m_pRoomSortCateBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_cate_d");
	m_pRoomSortHoleBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_hole_n");
	m_pRoomSortHoleBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_hole_o");
	m_pRoomSortHoleBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_hole_d");
	m_pRoomSortCourseBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_course_n");
	m_pRoomSortCourseBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_course_o");
	m_pRoomSortCourseBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_course_d");
	m_pRoomSortStateUpBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_up_n");
	m_pRoomSortStateUpBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_up_o");
	m_pRoomSortStateUpBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_up_d");
	m_pRoomSortStateDnBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_dn_n");
	m_pRoomSortStateDnBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_dn_o");
	m_pRoomSortStateDnBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("room_sort_state_dn_d");

	m_pGenderBmp[0] = (Bitmap*)g_pFresh->RegisterBitmap("i_male");
	m_pGenderBmp[1] = (Bitmap*)g_pFresh->RegisterBitmap("i_female");
	m_pGenderBmp[2] = (Bitmap*)g_pFresh->RegisterBitmap("i_male_02");
	m_pGenderBmp[3] = (Bitmap*)g_pFresh->RegisterBitmap("i_female_02");
	m_pGenderBmp[4] = (Bitmap*)g_pFresh->RegisterBitmap("i_male_03");
	m_pGenderBmp[5] = (Bitmap*)g_pFresh->RegisterBitmap("i_female_03");

	m_pGenderBmp[6] = (Bitmap*)g_pFresh->RegisterBitmap("i_male_manner");
	m_pGenderBmp[7] = (Bitmap*)g_pFresh->RegisterBitmap("i_female_manner");

	m_pGenderBmp[8] = (Bitmap*)g_pFresh->RegisterBitmap("i_male_angel");
	m_pGenderBmp[9] = (Bitmap*)g_pFresh->RegisterBitmap("i_female_angel");

	m_pUserSortGenderBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_gender_normal");
	m_pUserSortGenderBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_gender_over");
	m_pUserSortGenderBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_gender_click");
	m_pUserSortLevelBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_level_normal");
	m_pUserSortLevelBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_level_over");
	m_pUserSortLevelBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_level_click");

	m_pUserSortGuildBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_guild_normal");
	m_pUserSortGuildBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_guild_over");
	m_pUserSortGuildBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_guild_click");
	m_pUserSortNickBmp[0] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_nick_normal");
	m_pUserSortNickBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_nick_over");
	m_pUserSortNickBmp[2] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_nick_click");

	m_pUserSortArrowBmp[0] = (Bitmap*)g_pFresh->RegisterBitmap("user_sort_up");
	m_pUserSortArrowBmp[1] =
		(Bitmap*)g_pFresh->RegisterBitmap("user_sort_down");

	m_pInGameBmp = (Bitmap*)g_pFresh->RegisterBitmap("ingame");
}

void CLobbyMain::OnRoomList_Init(int param)
{
	CTaskMain::OnInit();

	Doc()->m_roomInfo.gameType = 16;

	Doc()->m_roomInfo.realGameType = 16;

	RoomList_LoadRoomImages();

	_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
	g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
		pTheme->GetRoomListBackGroundImgName(), true);

	SetTimeVariableBGM();
	TurnOff3DBackground();

	for (std::map<unsigned long, sRoomSlot>::iterator it =
			 Doc()->m_roomSlotMap.begin();
		it != Doc()->m_roomSlotMap.end(); ++it)
	{
		if (it->second.pExhibition)
		{
			delete it->second.pExhibition;
			it->second.pExhibition = NULL;
		}
	}
	Doc()->m_roomSlotMap.clear();

	if (IsLocalContent(S4_UCC))
	{
		NetResourceManager::Instance()->SetClearGarbage();
	}

	if (!((float)Doc()->m_myInfo.stat.dwNoMannerGameCount /
				(float)(Doc()->m_myInfo.stat.dwGameCount < 1
						? 1
						: Doc()->m_myInfo.stat.dwGameCount) *
				100.0f <
			3.0f))
	{
		CPartTidList& partTidList = Doc()->GetMyPartTidList();
		sCharacterInfo& charInfo =
			Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];

		for (int i = 0; i < partTidList.m_partNum; i++)
		{
			switch (charInfo.tidParts[i])
			{
			case 0x8016800:
			case 0x8058800:
			case 0x8098800:
			case 0x80dc800:
			case 0x8118800:
			case 0x8160800:
			case 0x8190800:
			case 0x81e2800:
			case 0x8214800:
			case 0x8254800:
				charInfo.tidParts[i] = 0;
				charInfo.ItemIdList[i] = 0;
			}

			switch (partTidList.m_tid[i])
			{
			case 0x8016800:
			case 0x8058800:
			case 0x8098800:
			case 0x80dc800:
			case 0x8118800:
			case 0x8160800:
			case 0x8190800:
			case 0x81e2800:
			case 0x8214800:
			case 0x8254800:
				partTidList.m_tid[i] = 0;
			}
		}
	}

	m_roomSort = (roomSort_t)6;
	m_bRoomSortRev = false;
}

void CLobbyMain::OnRoomList_Finish()
{
	m_pOverBarTitle->SetBgImg("title_roomlist");

	if (Doc()->m_bRefreshCamera)
		HandleMsg(MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));
}

void CLobbyMain::OnRoomList_Destroy()
{
}

void CLobbyMain::OnRoomList_RoomListInit(int param)
{
	m_pRoomList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pRoomList)
	{
		m_pRoomList->UseDummy(true);
		m_pRoomList->UseRightButton(true);

		if (s_roomGameType != -1)
		{
			std::list<sRoomInfo>::iterator it;
			for (it = Doc()->m_roomList.begin(); it != Doc()->m_roomList.end();
				it++)
			{
				if ((*it).gameType == s_roomGameType)
					m_pRoomList->AddItem(&(*it));
			}
		}
		else
		{
			std::list<sRoomInfo>::iterator it;
			for (it = Doc()->m_roomList.begin(); it != Doc()->m_roomList.end();
				it++)
			{
				if (GetCateByGameType((*it).gameType) == s_roomCate ||
					s_roomCate == 4)
					m_pRoomList->AddItem(&(*it));
			}
		}

		HandleMsg(MsgObject(NULL, 13, 0, 0, 0, 0, 0));
	}
}

const char* RoomListGetGameCategory(unsigned char gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
		return K2L_Compatibility("\xb4\xeb\xc0\xfc");

	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
		return K2L_Compatibility("\xb4\xeb\xc8\xb8");

	case GAME_TYPE_SKINS:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		return K2L_Compatibility("\xb9\xe8\xc6\xb2");

	case GAME_TYPE_AVATARCHAT:
		return K2L_Compatibility("\xb4\xeb\xc8\xad");
	default:
		return NULL;
	}
}

unsigned long DecideRoomColor(sRoomInfo* pRoom)
{
	if (pRoom->bAvailable && pRoom->bPublic)
	{
		if (pRoom->realGameType != 14)
		{
			if (!pRoom->bIntrusion)
				return 0xffffffff;
			else
				return WSingleton<CIntrusion>::Instance()->GetActiveColor();
		}
		else
			return 0;
	}
	else
		return pRoom->realGameType == 14 ? 0 : 0xb2ffffff;
}

void CLobbyMain::OnRoomList_RoomListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	sRoomInfo* pRoom = (sRoomInfo*)pItem->pData;

	int state = 0;
	unsigned long color = 0x37ffffff;

	if (pRoom)
	{
		if (pItem->selected)
		{
			state = 2;
		}
		else if (pItem->underCursor)
		{
			state = 1;
		}

		if (pRoom->bAvailable && pRoom->bPublic)
		{
			if (pRoom->bIntrusion)
				color = WSingleton<CIntrusion>::Instance()->GetActiveColor();
			else
				color = 0xffffffff;
		}
		else
			color = 0xb2ffffff;

		if (pRoom->realGameType == 14)
		{
			if (pRoom->bAvailable && pRoom->bPublic)
				color &= 0xffdd77ff;
			else
				color &= 0xffcc66dd;
		}
	}

	pGDI->DrawTexture(m_pRoomListBoxBmp[0][state],
		WRect(pItem->pos.x, pItem->pos.y, 40.0f, 48.0f), color, 0);
	pGDI->DrawTexture(m_pRoomListBoxBmp[1][state],
		WRect(pItem->pos.x + 40.0f, pItem->pos.y, 438.0f, 48.0f), color, 0);
	pGDI->DrawTexture(m_pRoomListBoxBmp[2][state],
		WRect(pItem->pos.x + 478.0f, pItem->pos.y, 40.0f, 48.0f), color, 0);

	if (!pRoom)
		return;

	if (pItem->selected)
	{
		pGDI->DrawTexture(m_pRoomListBoxBmp[0][3],
			WRect(pItem->pos.x, pItem->pos.y, 40.0f, 48.0f), 0xffffffff, 0);
		pGDI->DrawTexture(m_pRoomListBoxBmp[1][3],
			WRect(pItem->pos.x + 40.0f, pItem->pos.y, 438.0f, 48.0f),
			0xffffffff, 0);
		pGDI->DrawTexture(m_pRoomListBoxBmp[2][3],
			WRect(pItem->pos.x + 478.0f, pItem->pos.y, 40.0f, 48.0f),
			0xffffffff, 0);
	}

	if (pRoom->nUserLimit >= 100)
	{
		if (!m_pRoomEventBmp)
			m_pRoomEventBmp = (Bitmap*)g_pFresh->GetBitmap("room_event");
		if (m_pRoomEventBmp)
			pGDI->DrawTexture(m_pRoomEventBmp,
				WRect(pItem->pos.x + 7.0f, pItem->pos.y + 3.0f, 43.0f, 38.0f),
				0xffffffff, 0);

		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(0xff47a5dd, 0xffffffff);
	}
	else
	{
		Print(pItem->pos.x + 17.0f, pItem->pos.y + 18.0f,
			MakeStr("%03d", pRoom->roomGuid), 0.0f, 0, 1.0f, 0xffffb258);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}

	pGDI->PrintText(WPoint(pItem->pos.x + 59.0f, pItem->pos.y + 18.0f), 0,
		RoomListGetGameCategory(pRoom->gameType), NULL);

	switch (pRoom->gameType)
	{
	case GAME_TYPE_GUILD_MATCH:
		if (pRoom->GuildInfo.nGuildID[0])
		{
			const Bitmap* pEmblem =
				NetResourceManager::Instance()->GetEmblemByName(
					pRoom->GuildInfo.szEmblemName[0]);
			if (pEmblem)
				pGDI->DrawTexture(pEmblem,
					WRect(pItem->pos.x + 237.0f, pItem->pos.y + 12.0f,
						(float)pEmblem->Width(), (float)pEmblem->Height()),
					0xffffffff, 0);
		}

		if (pRoom->GuildInfo.nGuildID[1])
		{
			const Bitmap* pEmblem =
				NetResourceManager::Instance()->GetEmblemByName(
					pRoom->GuildInfo.szEmblemName[1]);
			if (pEmblem)
				pGDI->DrawTexture(pEmblem,
					WRect(pItem->pos.x + 294.0f, pItem->pos.y + 12.0f,
						(float)pEmblem->Width(), (float)pEmblem->Height()),
					0xffffffff, 0);
		}

		if (!pGDI)
			break;

		pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0,
			"%s", GetGameTypeName(pRoom->gameType));
		break;

	case GAME_TYPE_APPROACH:
		pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0,
			"%s", GetGameTypeName(pRoom->gameType));
		break;

	default:
	{
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(pRoom->tidMatch);
		if (pMatch)
		{
			char name[80] = { 0 };

			if (pRoom->realGameType != 14)
				strcpy(name, pMatch->Name);
			else
				strcpy(name,
					K2L_Compatibility("\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba"));

			pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0,
				"%s %s", name,
				pRoom->gameType == GAME_TYPE_30S_TEAM ? "\xc6\xc0\xc0\xfc"
													  : "");
		}
		else
		{
			pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0,
				"%s %s", GetGameTypeName(pRoom->gameType),
				pRoom->gameType == GAME_TYPE_30S_TEAM ? "\xc6\xc0\xc0\xfc"
													  : "");
		}
	}
	break;
	}

	pGDI->SetTextStyle(0);

	g_pFresh->GetManager()->PrintText(
		WPoint(pItem->pos.x + 221.0f, pItem->pos.y + 18.0f), 0,
		pRoom->gameType == GAME_TYPE_GUILD_MATCH ? "            VS             "
												 : pRoom->title,
		-1.0f, 0xffffffff);

	if (pRoom->realGameType != 14)
	{
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
			0x28000000 | (pRoom->mapType >= 0x7f ? 0x7f : pRoom->mapType));
		if (pCourse)
		{
			const Bitmap* pMap =
				g_pFresh->GetBitmap(MakeStr("room_%s", pCourse->c.Icon));
			if (pMap)
			{
				float x = pItem->pos.x + 442.0f;
				pGDI->DrawTexture(pMap,
					WRect(x, pItem->pos.y, (float)pMap->Width(),
						(float)pMap->Height()),
					0xffffffff, 0);

				if (Doc()->GetMapEventPangRate(pCourse->c.TypeId & 0x3ffffff) >
					100)
				{
					const Bitmap* pMark =
						g_pFresh->GetBitmap("lobby_mapevent_mark");
					if (pMark)
					{
						float mx = x + (float)(pMap->Width() - pMark->Width());
						pGDI->DrawTexture(pMark,
							WRect(mx, pItem->pos.y, (float)pMark->Width(),
								(float)pMark->Height()),
							0xffffffff, 0);
					}
				}
			}
		}
	}
	else
	{
		float x = pItem->pos.x + 442.0f;
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x2800007f);
		if (pCourse)
		{
			const Bitmap* pMap = g_pFresh->GetBitmap("room_map_17_01");
			if (pMap)
				pGDI->DrawTexture(pMap,
					WRect(x, pItem->pos.y, (float)pMap->Width(),
						(float)pMap->Height()),
					0xffffffff, 0);
		}
	}

	pGDI->Print(WPoint(pItem->pos.x + 387.0f, pItem->pos.y + 18.0f), 2,
		"%d\xc8\xa6", pRoom->nHole);

	if (Doc()->m_curChannel.Type & 0x80)
		pGDI->Print(WPoint(pItem->pos.x + 434.0f, pItem->pos.y + 18.0f), 2,
			"  -  ");
	else
		pGDI->Print(WPoint(pItem->pos.x + 434.0f, pItem->pos.y + 18.0f), 2,
			"%d/%d", pRoom->nUserNum, pRoom->nUserLimit);

	if (pRoom->gameType == 13)
		return;

	if (Doc()->m_curGameServer.eventFlags & 0x20)
	{
		if (Doc()->m_curGameServer.eventValue &
			(1 << ((pRoom->mapType > 0x7f && pRoom->mapType != 0xfd)
					 ? pRoom->mapType - 0x80
					 : pRoom->mapType)))
		{
			const Bitmap* pIcon = g_pFresh->GetBitmap("icon_pangevent");
			pGDI->DrawTexture(pIcon,
				WRect(pItem->pos.x + 492.0f, pItem->pos.y + 4.0f,
					(float)pIcon->Width(), (float)pIcon->Height()),
				0xffffffff, 0);
		}
	}

	if (IsLocalContent(S3_HUNDRED_MODE) && pRoom->nUserLimit >= 100)
	{
		pGDI->SetTextStyle(0);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
	}
}

void CLobbyMain::OnRoomList_RoomListLDown()
{
	FrListItem* pItem = m_pRoomList->GetItemUnderCursor();

	if (pItem)
	{
		if (m_pRoomList)
			m_pRoomList->SelectItem(pItem, true);

		m_pSelRoom = (sRoomInfo*)pItem->pData;
	}
	else
	{
		if (m_pRoomList)
			m_pRoomList->SelectItem(NULL, true);

		m_pSelRoom = NULL;
	}
}

void CLobbyMain::OnRoomList_RoomListRUp()
{
	if (m_pRoomList)
	{
		FrListItem* pItem = m_pRoomList->GetItemUnderCursor();
		if (pItem)
		{
			sRoomInfo* pRoom = (sRoomInfo*)pItem->pData;
			m_pRoomList->SelectItem(pItem, true);

			if (pRoom)
			{
				if (!pRoom->bPublic &&
					!(bool)(Doc()->m_myInfo.info.dwIdentity & 4))
				{
					this << MsgObject(this, 35,
						(int)"\xba\xf1\xb0\xf8\xb0\xb3 \xb9\xe6\xc0\xba \xb7\xeb\xc1\xa4\xba\xb8\xb8\xa6 \xba\xbc\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
				}
				else if (!(Doc()->m_curChannel.Type & 0x80) ||
					!pRoom->bAvailable)
				{
					WSendPacket packet((enumClientPacket)0x2d);
					packet.Encode2(pRoom->roomGuid);
					packet.Send(TO_GAME);
				}

				m_pSelRoom = pRoom;
			}
		}
	}
}

void CLobbyMain::OnRoomList_RoomListDClick()
{
	if (Doc()->m_curChannel.Type & 0x80)
		return;
	if (!m_pSelRoom)
		return;
	if (m_pRoomList && !m_pRoomList->IsEnabled())
		return;

	CIntrusion::Instance()->FinishGame();

	if (m_pSelRoom->bAvailable)
	{
		if (!(bool)(Doc()->m_myInfo.info.dwIdentity & 4))
		{
			if (m_pSelRoom->nUserNum == m_pSelRoom->nUserLimit)
			{
				this << MsgObject(this, 35,
					(int)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
				return;
			}

			switch (m_pSelRoom->gameType)
			{
			case GAME_TYPE_NEW_APPROACH:
				if (Doc()->m_myInfo.stat.Level == 0)
				{
					this << MsgObject(this, 35,
						(int)"\xb7\xe7\xc5\xb0"
							 "F \xb7\xb9\xba\xa7\xc0\xba \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1 \xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
					return;
				}
				break;

			case GAME_TYPE_30S_TEAM:
			{
				std::map<unsigned long, sBriefUserInfo>::iterator it =
					Doc()->m_briefUserInfoMap.find(MyGuid(false));
				if (it != Doc()->m_briefUserInfoMap.end() &&
					(*it).second.gender > 1)
				{
					this << MsgObject(this, 35,
						(int)"\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xb3\xf4\xc0\xba \xc0\xaf\xc0\xfa\xb4\xc2 30\xc0\xce \xc6\xc0\xc0\xfc \xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
					return;
				}
			}
			break;

			case GAME_TYPE_GUILD_MATCH:
			{
				std::string msg;
				bool bShow = false;
				unsigned long guildId = Doc()->m_myInfo.info.dwGuildId;

				if (guildId == 0)
				{
					msg =
						"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xcf\xbc\xc5\xbe\xdf \xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.";
					bShow = true;
				}

				if (m_pSelRoom->GuildInfo.nGuildID[0] != guildId &&
					m_pSelRoom->GuildInfo.nGuildID[0] != 0 &&
					m_pSelRoom->GuildInfo.nGuildID[1] != guildId &&
					m_pSelRoom->GuildInfo.nGuildID[1] != 0)
				{
					msg =
						"\xc5\xb8\xb1\xe6\xb5\xe5\xc0\xc7 \xb4\xeb\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.";
					bShow = true;
				}

				if (bShow)
				{
					this << MsgObject(this, 35, (int)msg.c_str(), 0, 0, 0, 0);
					return;
				}
			}
			break;
			}
		}

		EnableRoomEnterControls(false);

		BOOL bAdmin = (Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe;
		if (bAdmin)
		{
			EnableRoomEnterControls(false);

			WSendPacket packet((enumClientPacket)0x3e);
			packet.Encode2(m_pSelRoom->roomGuid);
			packet.EncodeStr(std::string(""));
			packet.Send(TO_GAME);

			this << MsgObject(NULL, 22, 0, 0, 0, 0, 0);
			this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			return;
		}

		bool bGM = (Doc()->m_myInfo.info.dwIdentity & 0x14) ? true : false;

		bool bInvite = false;
		if (IsLocalContent(S4_INVITE_FRIEND))
		{
			if (Doc()->m_inviteMode)
				bInvite = true;
		}

		if (m_pSelRoom->bPublic || bGM || bInvite)
		{
			if (IsLocalContent(S3_INTRUSION) &&
				m_pSelRoom->bIntrusion == true && !bGM)
			{
				CIntrusion::Instance()->DoJoinRoom(m_pSelRoom->roomGuid);
			}
			else
			{
				WSendPacket packet((enumClientPacket)9);
				packet.Encode2(m_pSelRoom->roomGuid);
				packet.EncodeStr(std::string(""));
				packet.Send(TO_GAME);

				this << MsgObject(NULL, 22, 0, 0, 0, 0, 0);
				this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			}
		}
		else
		{
			Doc()->m_roomInfo = *m_pSelRoom;

			if (!m_pPasswordDlg)
			{
				m_pPasswordDlg = CreateForm<FrPasswordDlg>(
					g_pFresh->GetManager(), this, "password");
				m_pPasswordDlg->Open(
					(FRESH_PFN_RESULT)&CLobbyMain::OnPasswordDlgResult, 3);
			}
		}
	}
	else
	{
		if (m_pSelRoom->bIntrusion)
		{
			Doc()->m_roomInfo = *m_pSelRoom;
			CIntrusion::Instance()->DoJoinRoom(Doc()->m_roomInfo.roomGuid);
		}
		else
		{
			HandleMsg(MsgObject(this, 35,
				(int)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0));
		}
	}
}

void CLobbyMain::OnRoomList_RoomSortNoInit(int param)
{
	m_pRoomSortNo = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bRoomSortDown[0] = false;
}

void CLobbyMain::OnRoomList_RoomSortNoDown()
{
	if (!(m_roomSortDelay > 0.0f))
	{
		if (m_roomSort == ROOM_SORT_NO)
			m_bRoomSortRev ^= 1;

		SetControlsRoomListSort(0);
		m_bRoomSortDown[0] = true;
		m_roomSortDelay = 0.3f;
		RoomList_SortRoomList(ROOM_SORT_NO, 0);
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

void CLobbyMain::OnRoomList_RoomSortNoOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI)
	{
		if (m_pRoomSortNo)
		{
			int state = 0;

			if (m_bRoomSortDown[0])
			{
				state = 2;
			}
			else
			{
				WRect rect = m_pRoomSortNo->GetRect();

				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
					state = 1;
			}

			pGDI->DrawTexture(m_pRoomSortNoBmp[state], m_pRoomSortNo->GetRect(),
				0xffffffff, 0);
		}
	}
}

void CLobbyMain::OnRoomList_RoomSortCateAll_BtnInit(int param)
{
	m_pRoomSortCateAll = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRoomSortCateAll->SetStatus(
		s_roomCate == 4 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::OnRoomList_RoomSortCateAll_BtnDown()
{
	RoomList_FilterRoomList(ROOM_SORT_CATE, 4);
	RoomList_SetCateTabBtn();

	s_roomGameType = -1;
}

void CLobbyMain::OnRoomList_RoomSortCateVS_BtnInit(int param)
{
	m_pRoomSortCateVS = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRoomSortCateVS->SetStatus(
		s_roomCate == 0 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::OnRoomList_RoomSortCateVS_BtnDown()
{
	RoomList_FilterRoomList(ROOM_SORT_CATE, 0);
	RoomList_SetCateTabBtn();

	m_cateListTime[0] = 0.0f;
	s_roomGameType = -1;
}

void CLobbyMain::OnRoomList_RoomSortCateMass_BtnInit(int param)
{
	m_pRoomSortCateMass = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRoomSortCateMass->SetStatus(
		s_roomCate == 1 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::OnRoomList_RoomSortCateMass_BtnDown()
{
	RoomList_FilterRoomList(ROOM_SORT_CATE, 1);
	RoomList_SetCateTabBtn();

	m_cateListTime[1] = 0.0f;
	s_roomGameType = -1;
}

void CLobbyMain::OnRoomList_RoomSortCateBattle_BtnInit(int param)
{
	m_pRoomSortCateBattle = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRoomSortCateBattle->SetStatus(
		s_roomCate == 2 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::OnRoomList_RoomSortCateBattle_BtnDown()
{
	RoomList_FilterRoomList(ROOM_SORT_CATE, 2);
	RoomList_SetCateTabBtn();

	m_cateListTime[2] = 0.0f;
	s_roomGameType = -1;
}

void CLobbyMain::OnRoomList_RoomSortCateChat_BtnInit(int param)
{
	m_pRoomSortCateChat = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pRoomSortCateChat->SetStatus(
		s_roomCate == 3 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::OnRoomList_RoomSortCateChat_BtnDown()
{
	RoomList_FilterRoomList(ROOM_SORT_CATE, 3);
	RoomList_SetCateTabBtn();

	s_roomGameType = -1;
}

void CLobbyMain::OnRoomList_RoomSortHoleInit(int param)
{
	m_pRoomSortHole = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bRoomSortDown[1] = false;
}

void CLobbyMain::OnRoomList_RoomSortHoleDown()
{
	if (!(m_roomSortDelay > 0.0f))
	{
		if (m_roomSort == ROOM_SORT_HOLE)
			m_bRoomSortRev ^= 1;

		SetControlsRoomListSort(0);
		m_bRoomSortDown[1] = true;
		m_roomSortDelay = 0.3f;
		RoomList_SortRoomList(ROOM_SORT_HOLE, 0);
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

void CLobbyMain::OnRoomList_RoomSortHoleOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI)
	{
		if (m_pRoomSortHole)
		{
			int state = 0;

			if (m_bRoomSortDown[1])
			{
				state = 2;
			}
			else
			{
				WRect rect = m_pRoomSortHole->GetRect();

				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
					state = 1;
			}

			pGDI->DrawTexture(m_pRoomSortHoleBmp[state],
				m_pRoomSortHole->GetRect(), 0xffffffff, 0);
		}
	}
}

void CLobbyMain::OnRoomList_RoomSortCourseInit(int param)
{
	m_pRoomSortCourse = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bRoomSortDown[2] = false;
}

void CLobbyMain::OnRoomList_RoomSortCourseDown()
{
	if (!(m_roomSortDelay > 0.0f))
	{
		if (m_roomSort == ROOM_SORT_COURSE)
			m_bRoomSortRev ^= 1;

		SetControlsRoomListSort(0);
		m_bRoomSortDown[2] = true;
		m_roomSortDelay = 0.3f;
		RoomList_SortRoomList(ROOM_SORT_COURSE, 0);
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

void CLobbyMain::OnRoomList_RoomSortCourseOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI)
	{
		if (m_pRoomSortCourse)
		{
			int state = 0;

			if (m_bRoomSortDown[2])
			{
				state = 2;
			}
			else
			{
				WRect rect = m_pRoomSortCourse->GetRect();

				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
					state = 1;
			}

			pGDI->DrawTexture(m_pRoomSortCourseBmp[state],
				m_pRoomSortCourse->GetRect(), 0xffffffff, 0);
		}
	}
}

void CLobbyMain::OnRoomList_RoomSortStateInit(int param)
{
	m_pRoomSortState = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bRoomSortDown[3] = false;
}

void CLobbyMain::OnRoomList_RoomSortStateDown()
{
	if (m_roomSortDelay > 0.0f)
		return;

	if (m_roomSort == 6)
	{
		m_bRoomSortRev ^= 1;
	}

	SetControlsRoomListSort(0);
	m_bRoomSortDown[3] = true;
	m_roomSortDelay = 0.3f;
	RoomList_SortRoomList((roomSort_t)6, 0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

void CLobbyMain::OnRoomList_RoomSortStateOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	if (!pDevice)
		return;

	if (!m_pRoomSortState)
		return;

	int state = 0;

	if (m_bRoomSortDown[3])
		state = 2;
	else
	{
		WRect rect = m_pRoomSortState->GetRect();
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			state = 1;
	}

	pDevice->DrawTexture(m_pRoomSortStateUpBmp[m_bRoomSortRev * 3 + state],
		m_pRoomSortState->GetRect(), 0xffffffff, 0);
}

void CLobbyMain::OnRoomList_RoomSortCateVS_ListInit(int param)
{
	m_pRoomSortCateVSList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pRoomSortCateVSList)
	{
		for (int i = 0; i < 2; i++)
		{
			m_pRoomSortCateVSList->AddItem(&g_strGameType[i]);
		}

		m_pRoomSortCateVSList->SetVisible(false);
		m_pRoomSortCateVSList->EnableScrollBar(false);
	}
}

void CLobbyMain::OnRoomList_RoomSortCateVS_ListLUp()
{
	int count = m_pRoomSortCateVSList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++)
	{
		FrListItem* pItem = m_pRoomSortCateVSList->GetItem(i);
		if (!pItem)
			continue;

		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 21.0f);
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			m_bCateSelected = true;

			RoomList_FilterRoomList((roomSort_t)3, pItem->idx);
			break;
		}
	}

	if (m_pRoomSortCateVSList)
		m_pRoomSortCateVSList->SetVisible(false);
}

void CLobbyMain::OnRoomList_RoomSortCateVS_ListOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;

	if (!pDevice)
		return;
	if (!pItem)
		return;

	std::string* pStr = (std::string*)pItem->pData;
	WRect rect;
	rect.x = pItem->pos.x;
	rect.y = pItem->pos.y;
	rect.w = 56.0f;
	rect.h = 22.0f;

	pDevice->Box(rect, 0x880e4f75);

	rect.x = pItem->pos.x + 1.0f;
	rect.y = pItem->pos.y + 1.0f;
	rect.w = 54.0f;
	rect.h = 20.0f;

	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		pDevice->Box(rect, 0x88082b46);
		pDevice->SetTextStyle(1);
	}
	else
		pDevice->SetTextStyle(0);

	pDevice->SetTextColor(0xffffffff, 0xffffffff);
	pDevice->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0,
		pStr->c_str());
}

void CLobbyMain::OnRoomList_RoomSortCateMass_ListInit(int param)
{
	m_pRoomSortCateMassList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pRoomSortCateMassList)
	{
		for (int i = 2; i < 5; i++)
		{
			m_pRoomSortCateMassList->AddItem(&g_strGameType[i]);
		}

		m_pRoomSortCateMassList->SetVisible(false);
		m_pRoomSortCateMassList->EnableScrollBar(false);
	}
}

void CLobbyMain::OnRoomList_RoomSortCateMass_ListLUp()
{
	int count = m_pRoomSortCateMassList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++)
	{
		FrListItem* pItem = m_pRoomSortCateMassList->GetItem(i);
		if (!pItem)
			continue;

		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 21.0f);
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			m_bCateSelected = true;

			RoomList_FilterRoomList((roomSort_t)3, pItem->idx + 4);
			break;
		}
	}

	if (m_pRoomSortCateMassList)
		m_pRoomSortCateMassList->SetVisible(false);
}

void CLobbyMain::OnRoomList_RoomSortCateMass_ListOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;

	if (!pDevice)
		return;
	if (!pItem)
		return;

	std::string* pStr = (std::string*)pItem->pData;
	WRect rect;
	rect.x = pItem->pos.x;
	rect.y = pItem->pos.y;
	rect.w = 56.0f;
	rect.h = 22.0f;

	pDevice->Box(rect, 0x880e4f75);

	rect.x = pItem->pos.x + 1.0f;
	rect.y = pItem->pos.y + 1.0f;
	rect.w = 54.0f;
	rect.h = 20.0f;

	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		pDevice->Box(rect, 0x88082b46);
		pDevice->SetTextStyle(1);
	}
	else
		pDevice->SetTextStyle(0);

	pDevice->SetTextColor(0xffffffff, 0xffffffff);
	pDevice->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0,
		pStr->c_str());
}

void CLobbyMain::OnRoomList_RoomSortCateBattle_ListInit(int param)
{
	m_pRoomSortCateBattleList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pRoomSortCateBattleList)
	{
		for (int i = 5; i < 7; i++)
		{
			m_pRoomSortCateBattleList->AddItem(&g_strGameType[i]);
		}

		m_pRoomSortCateBattleList->SetVisible(false);
		m_pRoomSortCateBattleList->EnableScrollBar(false);
	}
}

void CLobbyMain::OnRoomList_RoomSortCateBattle_ListLUp()
{
	int count = m_pRoomSortCateBattleList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++)
	{
		FrListItem* pItem = m_pRoomSortCateBattleList->GetItem(i);
		if (!pItem)
			continue;

		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 21.0f);
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			m_bCateSelected = true;
			RoomList_FilterRoomList((roomSort_t)3, pItem->idx ? 10 : 7);
			break;
		}
	}

	if (m_pRoomSortCateBattleList)
		m_pRoomSortCateBattleList->SetVisible(false);
}

void CLobbyMain::OnRoomList_RoomSortCateBattle_ListOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;

	if (!pDevice)
		return;
	if (!pItem)
		return;

	std::string* pStr = (std::string*)pItem->pData;
	WRect rect;
	rect.x = pItem->pos.x;
	rect.y = pItem->pos.y;
	rect.w = 56.0f;
	rect.h = 22.0f;

	pDevice->Box(rect, 0x880e4f75);

	rect.x = pItem->pos.x + 1.0f;
	rect.y = pItem->pos.y + 1.0f;
	rect.w = 54.0f;
	rect.h = 20.0f;

	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		pDevice->Box(rect, 0x88082b46);
		pDevice->SetTextStyle(1);
	}
	else
		pDevice->SetTextStyle(0);

	pDevice->SetTextColor(0xffffffff, 0xffffffff);
	pDevice->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0,
		pStr->c_str());
}

void CLobbyMain::OnRoomList_MakeRoomInit(int param)
{
	m_pMakeRoom = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMakeRoom)
	{
		THEME_BUTTONS buttons;

		_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
		pTheme->GetMakeRoomButtonImgName(buttons);

		m_pMakeRoom->SetButtonImg(buttons.press, FrButton::PRESSED);
		m_pMakeRoom->SetButtonImg(buttons.normal, FrButton::NORMAL);
		m_pMakeRoom->SetButtonImg(buttons.over, FrButton::OVER);
	}
}

void CLobbyMain::OnRoomList_MakeRoomUp()
{
	CloseAllDialogs();

	if (!LatestVersion())
		return;

	if (Doc()->IsControlServerService(2))
	{
		HandleMsg(MsgObject(this, 0x272, 2, 0, 0, 0, 0));
		return;
	}

	if (!m_pServerDlg && Doc()->m_curChannel.Uid == 0xff)
	{
		WSendPacket packet((enumClientPacket)0x43);
		packet.Send(TO_GAME);

		m_pServerDlg = CreateForm<FrServerDlg>(g_pFresh->GetManager(), this,
			"server", NULL);
		m_pServerDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnServerDlgResult,
			WPoint(m_pServerDlg->GetRect().x, 50.0f), FrICON);
		m_pServerDlg->SetIconRect(m_pUnderBarServer->GetRect());
	}
	else
	{
		EnableRoomEnterControls(false);

		m_pMakeRoomDlg = CreateForm<FrMakeRoomDlg>(g_pFresh->GetManager(), this,
			"makeroom", NULL);
		m_pMakeRoomDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnMakeRoomDlgResult,
			FrESCAPE | FrENTER);
	}
}

void CLobbyMain::OnRoomList_GuildInfoInit(int param)
{
	m_pGuildInfo = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pGuildInfo)
	{
		THEME_BUTTONS buttons;

		_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
		if (pTheme)
		{
			pTheme->GetQuickStartButtonImgName(buttons);

			m_pGuildInfo->SetButtonImg(buttons.press, FrButton::PRESSED);
			m_pGuildInfo->SetButtonImg(buttons.normal, FrButton::NORMAL);
			m_pGuildInfo->SetButtonImg(buttons.over, FrButton::OVER);

			m_pGuildInfo->SetPushDelay(0.1f);
		}
	}
}

void CLobbyMain::OnRoomList_GuildInfoLButtonUp()
{
	if (Doc()->IsControlServerService(0x1000000))
	{
		this << MsgObject(this, 35,
			(int)"\xb1\xe6\xb5\xe5 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 "
				 "\xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else
	{
		IActor* pActor = AfxGetTask()->GetActor(s_guildActor[1]);
		if (pActor)
			pActor << MsgObject(this, 0x263, 1, 0, 0, 0, 0);
	}
}

void CLobbyMain::OnRoomList_UserListInit(int param)
{
	m_pUserList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pUserList)
		return;

	m_pUserList->UseRightButton(true);

	for (std::map<unsigned long, sBriefUserInfo>::iterator it =
			 Doc()->m_briefUserInfoMap.begin();
		it != Doc()->m_briefUserInfoMap.end(); ++it)
	{
		if (Doc()->m_myInfo.info.dwIdentity & 4)
			m_pUserList->AddItem((void*)&(*it).second);
		else if (!(sBriefUserInfo((*it).second).dwIdentity & 4))
			m_pUserList->AddItem((void*)&(*it).second);
		else if (sBriefUserInfo((*it).second).state & 1)
			m_pUserList->AddItem((void*)&(*it).second);
	}
}

void CLobbyMain::OnRoomList_UserListLBtnUp()
{
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (pItem)
	{
		sBriefUserInfo* pInfo = (sBriefUserInfo*)pItem->pData;

		if (pInfo)
		{
			m_selOID = pInfo->dwGuid;
			m_selUID = pInfo->dwUid;

			if (m_pUserInfo)
				m_pUserInfo->Enable(true);
		}

		pItem->selected = true;
	}
}

void CLobbyMain::OnRoomList_UserListRBtnUp()
{
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (pItem)
	{
		sBriefUserInfo* pInfo = (sBriefUserInfo*)pItem->pData;

		if (pInfo)
		{
			m_selOID = pInfo->dwGuid;
			m_selUID = pInfo->dwUid;

			if (m_pUserInfo)
				m_pUserInfo->Enable(true);

			if (CUserInfo::IsInstantiated())
			{
				CUserInfo::Instance()->SetInfo(pInfo->dwUid, pInfo->dwGuid,
					true, true, false, false, pInfo->sNick);
			}
		}
		else
		{
			CChatMsg::Instance()->AddChatMsg(
				"\xc0\xdf\xb8\xf8\xb5\xc8 \xb4\xeb\xbb\xf3\xc0\xd4\xb4\xcf\xb4\xd9.",
				7, true, false);
		}

		pItem->selected = true;
	}
}

void CLobbyMain::OnRoomList_UserListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;

	sBriefUserInfo* pInfo = (sBriefUserInfo*)pItem->pData;
	if (!pInfo)
		return;

	unsigned long color;

	if (pInfo->dwIdentity & 0x14)
	{
		if (Doc()->m_myInfo.info.dwIdentity & 0x14)
		{
			color = 0x50ffffff;
		}
		else
		{
			if (!(pInfo->state & 1))
				return;

			color = 0xffffffff;
		}
	}
	else
	{
		if (IsLocalContent((localContentType_t)0x10))
			color = pInfo->nChannelingFlag == 1 ? 0xff33dbff : 0xffffffff;
		else
			color = 0xffffffff;
	}

	if (pItem->selected)
	{
		if (m_selOID == ((sBriefUserInfo*)pItem->pData)->dwGuid)
			pDevice->Box(WRect(pItem->pos.x, pItem->pos.y,
							 (float)m_pUserList->GetItemWidth(),
							 (float)m_pUserList->GetItemHeight()),
				0xff265785);
		else
			pItem->selected = false;
	}

	const Bitmap* pIcon;

	if (pInfo->angelicWings)
		pIcon = m_pGenderBmp[pInfo->gender % 2 + 8];
	else if (pInfo->manner)
		pIcon = m_pGenderBmp[pInfo->gender % 2 + 6];
	else
		pIcon = m_pGenderBmp[pInfo->gender];

	WRect rect(pItem->pos.x + 8.0f, pItem->pos.y, (float)pIcon->Width(),
		(float)pIcon->Height());
	pDevice->DrawTexture(pIcon, rect, 0xffffffff, 0);

	if (pInfo->roomIndex != 0xffff)
	{
		pDevice->DrawTexture(m_pInGameBmp,
			WRect(rect.x, rect.y + 3.0f, (float)m_pInGameBmp->Width(),
				(float)m_pInGameBmp->Height()),
			0xffffffff, 0);
	}

	if (Doc()->m_curChannel.Type & 0x80)
	{
		pIcon = g_pFresh->GetBitmap(
			MakeStr("ladder_%03d", pInfo->dwLadderPoint / 100));
	}
	else
	{
		if (pInfo->dwTitle)
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pInfo->dwTitle);
			if (pSkin)
				pIcon = g_pFresh->GetBitmap(pSkin->c.Icon);
			else
				pIcon = g_pFresh->GetBitmap(
					MakeStr("level_%03d", pInfo->level + 1));
		}
		else
			pIcon =
				g_pFresh->GetBitmap(MakeStr("level_%03d", pInfo->level + 1));
	}

	if (pIcon)
	{
		pDevice->DrawTexture(pIcon,
			WRect(pItem->pos.x + 66.0f - float2int(pIcon->Width() * 0.5f),
				pItem->pos.y + 11.0f - float2int(pIcon->Height() * 0.5f),
				(float)pIcon->Width(), (float)pIcon->Height()),
			0xffffffff, 0);
	}

	if (pInfo->m_GuildId && !(pInfo->dwIdentity & 0x14))
	{
		const Bitmap* pEmblem = NetResourceManager::Instance()->GetEmblemByName(
			pInfo->szEmblemName);

		if (pEmblem)
		{
			pDevice->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 106.0f, pItem->pos.y + 7.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
		}
	}

	if (CProjectG::Instance()->HidePrivacy())
		return;

	if (pItem->underCursor)
	{
		pDevice->SetTextColor(0xffffffff, 0xff124371);
		pDevice->SetTextStyle(2);
	}
	else
	{
		pDevice->SetTextColor(color, 0xffffffff);
		pDevice->SetTextStyle(0);
	}

	g_pFresh->GetManager()->PrintText(
		WPoint(pItem->pos.x + 143.0f, pItem->pos.y + 7.0f), 0, pInfo->sNick,
		(float)m_pUserList->GetItemWidth() - 143.0f, color);
}

void CLobbyMain::OnRoomList_UserSortGenderInit(int param)
{
	m_pUserSortGender = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bUserSortDown[0] = false;
}

void CLobbyMain::OnRoomList_UserSortGenderUp()
{
	if (m_userSortDelay > 0.0f)
		return;

	if (m_userSort == 0)
		m_bUserSortRev ^= 1;
	else
		m_bUserSortRev = false;

	m_bUserSortDown[0] = true;
	m_bUserSortDown[1] = false;
	m_bUserSortDown[2] = false;
	m_bUserSortDown[3] = false;
	m_userSortDelay = 0.3f;
	RoomList_SortUserList((userSort_t)0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

void CLobbyMain::OnRoomList_UserSortGenderOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	if (!pDevice)
		return;

	if (!m_pUserSortGender)
		return;

	int state = 0;

	if (m_bUserSortDown[0])
		state = 2;
	else
	{
		WRect rect = m_pUserSortGender->GetRect();
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			state = 1;
	}

	WRect dest = m_pUserSortGender->GetRect();

	pDevice->DrawTexture(m_pUserSortGenderBmp[state], dest, 0xffffffff, 0);

	if (m_userSort == 0)
		pDevice->DrawTexture(m_pUserSortArrowBmp[!m_bUserSortRev],
			WRect(dest.Right(), dest.y + 5.0f, 12.0f, 12.0f), 0xffffffff, 0);
}

void CLobbyMain::OnRoomList_UserSortLevelInit(int param)
{
	m_pUserSortLevel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bUserSortDown[1] = false;
}

void CLobbyMain::OnRoomList_UserSortLevelUp()
{
	if (m_userSortDelay > 0.0f)
		return;

	if (m_userSort == 1)
		m_bUserSortRev ^= 1;
	else
		m_bUserSortRev = false;

	m_bUserSortDown[0] = false;
	m_bUserSortDown[1] = true;
	m_bUserSortDown[2] = false;
	m_bUserSortDown[3] = false;
	m_userSortDelay = 0.3f;
	RoomList_SortUserList((userSort_t)1);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

void CLobbyMain::OnRoomList_UserSortLevelOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	if (!pDevice)
		return;

	if (!m_pUserSortLevel)
		return;

	int state = 0;

	if (m_bUserSortDown[1])
		state = 2;
	else
	{
		WRect rect = m_pUserSortLevel->GetRect();
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			state = 1;
	}

	WRect dest = m_pUserSortLevel->GetRect();

	pDevice->DrawTexture(m_pUserSortLevelBmp[state], dest, 0xffffffff, 0);

	if (m_userSort == 1)
		pDevice->DrawTexture(m_pUserSortArrowBmp[!m_bUserSortRev],
			WRect(dest.Right(), dest.y + 5.0f, 12.0f, 12.0f), 0xffffffff, 0);
}

void CLobbyMain::OnRoomList_UserSortGuildInit(int param)
{
	m_pUserSortGuild = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bUserSortDown[2] = false;
}

void CLobbyMain::OnRoomList_UserSortGuildUp()
{
	if (m_userSortDelay > 0.0f)
		return;

	if (m_userSort == 2)
		m_bUserSortRev ^= 1;
	else
		m_bUserSortRev = false;

	m_bUserSortDown[0] = false;
	m_bUserSortDown[1] = false;
	m_bUserSortDown[2] = true;
	m_bUserSortDown[3] = false;
	m_userSortDelay = 0.3f;
	RoomList_SortUserList((userSort_t)2);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

void CLobbyMain::OnRoomList_UserSortGuildOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	if (!pDevice)
		return;

	if (!m_pUserSortGuild)
		return;

	int state = 0;

	if (m_bUserSortDown[2])
		state = 2;
	else
	{
		WRect rect = m_pUserSortGuild->GetRect();
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			state = 1;
	}

	WRect dest = m_pUserSortGuild->GetRect();

	pDevice->DrawTexture(m_pUserSortGuildBmp[state], dest, 0xffffffff, 0);

	if (m_userSort == 2)
		pDevice->DrawTexture(m_pUserSortArrowBmp[!m_bUserSortRev],
			WRect(dest.Right(), dest.y + 5.0f, 12.0f, 12.0f), 0xffffffff, 0);
}

void CLobbyMain::OnRoomList_UserSortNickInit(int param)
{
	m_pUserSortNick = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_bUserSortDown[3] = false;
}

void CLobbyMain::OnRoomList_UserSortNickUp()
{
	if (m_userSortDelay > 0.0f)
		return;

	if (m_userSort == 3)
		m_bUserSortRev ^= 1;
	else
		m_bUserSortRev = false;

	m_bUserSortDown[0] = false;
	m_bUserSortDown[1] = false;
	m_bUserSortDown[2] = false;
	m_bUserSortDown[3] = true;
	m_userSortDelay = 0.3f;
	RoomList_SortUserList((userSort_t)3);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

void CLobbyMain::OnRoomList_UserSortNickOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	if (!pDevice)
		return;

	if (!m_pUserSortNick)
		return;

	int state = 0;

	if (m_bUserSortDown[3])
		state = 2;
	else
	{
		WRect rect = m_pUserSortNick->GetRect();
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			state = 1;
	}

	WRect dest = m_pUserSortNick->GetRect();

	pDevice->DrawTexture(m_pUserSortNickBmp[state], dest, 0xffffffff, 0);

	if (m_userSort == 3)
		pDevice->DrawTexture(m_pUserSortArrowBmp[!m_bUserSortRev],
			WRect(dest.Right(), dest.y + 5.0f, 12.0f, 12.0f), 0xffffffff, 0);
}

void CLobbyMain::RoomList_FilterRoomList(roomSort_t sort, int value)
{
	if (m_pRoomList)
	{
		m_pRoomList->ClearItem();

		if (sort == ROOM_SORT_CATE)
		{
			std::list<sRoomInfo>::iterator it = Doc()->m_roomList.begin();
			s_roomCate = value;
			for (; it != Doc()->m_roomList.end(); ++it)
			{
				if (GetCateByGameType((*it).gameType) == s_roomCate ||
					s_roomCate == 4)
					m_pRoomList->AddItem(&(*it));
			}
		}
		else if (sort == ROOM_SORT_GAMETYPE)
		{
			std::list<sRoomInfo>::iterator it = Doc()->m_roomList.begin();
			s_roomGameType = value;
			for (; it != Doc()->m_roomList.end(); ++it)
			{
				if ((*it).gameType == value)
					m_pRoomList->AddItem(&(*it));
			}
		}

		for (int i = m_pRoomList->GetCurrentItemSize(true); i < 6; i++)
			m_pRoomList->AddItem(NULL);
	}
}

void CLobbyMain::RoomList_SortRoomList(roomSort_t sort, int unused)
{
	if (m_pRoomList)
	{
		m_roomSort = sort;

		switch (sort)
		{
		case ROOM_SORT_NO:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomNoRevCompare);
			else
				m_pRoomList->SortItem(RoomNoCompare);
			break;
		case ROOM_SORT_TITLE:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomTitleRevCompare);
			else
				m_pRoomList->SortItem(RoomTitleCompare);
			break;
		case ROOM_SORT_HOLE:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomHoleRevCompare);
			else
				m_pRoomList->SortItem(RoomHoleCompare);
			break;
		case ROOM_SORT_GAMETYPE:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomGameTypeRevCompare);
			else
				m_pRoomList->SortItem(RoomGameTypeCompare);
		case ROOM_SORT_COURSE:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomCourseRevCompare);
			else
				m_pRoomList->SortItem(RoomCourseCompare);
			break;
		case ROOM_SORT_STATE:
			if (m_bRoomSortRev)
				m_pRoomList->SortItem(RoomStateRevCompare);
			else
				m_pRoomList->SortItem(RoomStateCompare);
			break;
		}
	}
}

void CLobbyMain::RoomList_SetCateTabBtn()
{
	m_pRoomSortCateVS->SetStatus(
		s_roomCate == 0 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pRoomSortCateMass->SetStatus(
		s_roomCate == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pRoomSortCateBattle->SetStatus(
		s_roomCate == 2 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pRoomSortCateChat->SetStatus(
		s_roomCate == 3 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pRoomSortCateAll->SetStatus(
		s_roomCate == 4 ? FrButton::PRESSED : FrButton::NORMAL);
}

void CLobbyMain::RoomList_SortUserList(userSort_t sort)
{
	if (m_pUserList)
	{
		m_userSort = sort;

		switch (sort)
		{
		case USER_SORT_GENDER:
			if (m_bUserSortRev)
				m_pUserList->SortItem(UserGenderRevCompare);
			else
				m_pUserList->SortItem(UserGenderCompare);
			break;
		case USER_SORT_LEVEL:
			if (m_bUserSortRev)
				m_pUserList->SortItem(UserLevelRevCompare);
			else
				m_pUserList->SortItem(UserLevelCompare);
			break;
		case USER_SORT_GUILD:
			if (m_bUserSortRev)
				m_pUserList->SortItem(UserGuildRevCompare);
			else
				m_pUserList->SortItem(UserGuildCompare);
			break;
		case USER_SORT_NICK:
			if (m_bUserSortRev)
				m_pUserList->SortItem(UserNicknameRevCompare);
			else
				m_pUserList->SortItem(UserNicknameCompare);
			break;
		}
	}
}

void CLobbyMain::SetControlsRoomListSort(int type)
{
	m_bRoomSortDown[0] = false;
	m_bRoomSortDown[1] = false;
	m_bRoomSortDown[2] = false;
	m_bRoomSortDown[3] = false;

	switch (type)
	{
	case 1:
		if (m_pRoomSortCateVSList)
			m_pRoomSortCateVSList->SetVisible(false);
		if (m_pRoomSortCateMassList)
			m_pRoomSortCateMassList->SetVisible(false);
		if (m_pRoomSortCateBattleList)
			m_pRoomSortCateBattleList->SetVisible(false);
		break;
	default:
		if (m_pRoomSortCateVSList)
			m_pRoomSortCateVSList->SetVisible(false);
		if (m_pRoomSortCateMassList)
			m_pRoomSortCateMassList->SetVisible(false);
		if (m_pRoomSortCateBattleList)
			m_pRoomSortCateBattleList->SetVisible(false);
		break;
	}
}

void CLobbyMain::ResetControlsRoomListSort(float elapsed)
{
	if (m_roomSortDelay > 0.0f)
	{
		m_roomSortDelay -= elapsed;

		if (m_roomSortDelay <= 0.0f)
		{
			m_bRoomSortDown[0] = false;
			m_bRoomSortDown[1] = false;
			m_bRoomSortDown[2] = false;
			m_bRoomSortDown[3] = false;
		}
	}

	if (m_userSortDelay > 0.0f)
	{
		m_userSortDelay -= elapsed;

		if (m_userSortDelay <= 0.0f)
		{
			m_bUserSortDown[0] = false;
			m_bUserSortDown[1] = false;
			m_bUserSortDown[2] = false;
			m_bUserSortDown[3] = false;
		}
	}

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST"))
	{
		if (g_input->GetButton(LEFT_BUTTON) == 3)
		{
			WVector mousePos = g_input->GetMousePoint();
			WVector downPos = g_input->GetButtonDownPos(LEFT_BUTTON);

			WRect vsRect = m_pRoomSortCateVS->GetRect(),
				  vsListRect = m_pRoomSortCateVSList->GetRect();
			WRect massRect = m_pRoomSortCateMass->GetRect(),
				  massListRect = m_pRoomSortCateMassList->GetRect();
			WRect battleRect = m_pRoomSortCateBattle->GetRect(),
				  battleListRect = m_pRoomSortCateBattleList->GetRect();

			if (m_pRoomSortCateVS->GetStatus() == FrButton::PRESSED &&
				vsRect.IsInRect(WPoint(downPos.x, downPos.y)) &&
				(vsRect.IsInRect(WPoint(mousePos.x, mousePos.y)) ||
					vsListRect.IsInRect(WPoint(mousePos.x, mousePos.y))))
			{
				m_cateListTime[0] += elapsed;

				if (m_cateListTime[0] > 0.8f)
					m_pRoomSortCateVSList->SetVisible(true);
			}
			else
			{
				m_cateListTime[0] = 0.0f;
				m_pRoomSortCateVSList->SetVisible(false);
			}

			if (m_pRoomSortCateMass->GetStatus() == FrButton::PRESSED &&
				massRect.IsInRect(WPoint(downPos.x, downPos.y)) &&
				(massRect.IsInRect(WPoint(mousePos.x, mousePos.y)) ||
					massListRect.IsInRect(WPoint(mousePos.x, mousePos.y))))
			{
				m_cateListTime[1] += elapsed;

				if (m_cateListTime[1] > 0.8f)
					m_pRoomSortCateMassList->SetVisible(true);
			}
			else
			{
				m_cateListTime[1] = 0.0f;
				m_pRoomSortCateMassList->SetVisible(false);
			}

			if (m_pRoomSortCateBattle->GetStatus() == FrButton::PRESSED &&
				battleRect.IsInRect(WPoint(downPos.x, downPos.y)) &&
				(battleRect.IsInRect(WPoint(mousePos.x, mousePos.y)) ||
					battleListRect.IsInRect(WPoint(mousePos.x, mousePos.y))))
			{
				m_cateListTime[2] += elapsed;

				if (m_cateListTime[2] > 0.8f)
					m_pRoomSortCateBattleList->SetVisible(true);
			}
			else
			{
				m_cateListTime[2] = 0.0f;
				m_pRoomSortCateBattleList->SetVisible(false);
			}
		}
		else
		{
			if (m_pRoomSortCateVSList->IsVisible())
				OnRoomList_RoomSortCateVS_ListLUp();

			if (m_pRoomSortCateMassList->IsVisible())
				OnRoomList_RoomSortCateMass_ListLUp();

			if (m_pRoomSortCateBattleList->IsVisible())
				OnRoomList_RoomSortCateBattle_ListLUp();

			m_cateListTime[0] = 0.0f;
			m_cateListTime[1] = 0.0f;
			m_cateListTime[2] = 0.0f;
		}
	}
}

void CLobbyMain::OnChatBgInit(int param)
{
	m_pChatBg = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnChatBgOwnerDraw(int param)
{
	if (g_pFresh->GetManager()->GetGDI())
	{
		const WRect& rect = m_pChatBg->GetRect();
		WOverlay::DrawBox(g_view, WRect(rect.x, rect.y, rect.w, rect.h), 0,
			0x58000000, 0);
	}
}

void CLobbyMain::OnChatWndBtnInit(int param)
{
	m_pChatWndBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pChatWndBtn->SetPushDelay(0.0f);

	bool bUp = COption::Instance()->gGetChatWndUp() != 0;

	ToggleChatBtn(bUp);
}

void CLobbyMain::OnChatWndBtnUp()
{
	COption::Instance()->gSetChatWndUp(!COption::Instance()->gGetChatWndUp());

	ToggleChatBtn(true);
}

void CLobbyMain::ToggleChatBtn(bool bMove)
{
	if (COption::Instance()->gGetChatWndUp())
	{
		m_pChatWndBtn->SetButtonImg("chatwnd_down", FrButton::NORMAL);
		if (!bMove)
			return;

		const WRect& bgRect = m_pChatBg->GetRect();

		m_pChatBg->SetRect(
			WRect(bgRect.x, bgRect.y - 200.0f, bgRect.w, bgRect.h + 200.0f));

		const WRect& viewRect = m_pChatView->GetRect();
		m_pChatView->SetRect(WRect(viewRect.x, viewRect.y - 200.0f, viewRect.w,
			viewRect.h + 200.0f));
	}
	else
	{
		m_pChatWndBtn->SetButtonImg("chatwnd_up", FrButton::NORMAL);
		if (!bMove)
			return;

		const WRect& bgRect = m_pChatBg->GetRect();
		m_pChatBg->SetRect(
			WRect(bgRect.x, bgRect.y + 200.0f, bgRect.w, bgRect.h - 200.0f));

		const WRect& viewRect = m_pChatView->GetRect();
		m_pChatView->SetRect(WRect(viewRect.x, viewRect.y + 200.0f, viewRect.w,
			viewRect.h - 200.0f));
	}

	FrScrollBar* bar = m_pChatView->GetScrollBar();
	if (bar)
		bar->ScrollToBottom();
}

void CLobbyMain::ReLoadHoleItem()
{
	if (m_pMapHoleCombo)
	{
		m_pMapHoleCombo->ClearAllListItem();
		m_pMapHoleCombo->AddTexItem("hole_front");
		if (Doc()->m_roomInfo.nHole < 18)
		{
			m_pMapHoleCombo->AddTexItem("hole_back");
			m_pMapHoleCombo->AddTexItem("hole_random");
		}
		m_pMapHoleCombo->AddTexItem("hole_shuffle");
	}
}

void CLobbyMain::OnRoomList_ChatViewInit(int param)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pChatView->HidePrivacy(true);

	Doc()->m_chatLineList.clear();
}

void CLobbyMain::OnLanguageInit(int param)
{
	m_pLanguage = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pLanguage)
	{
		if (g_ime->IsAlphaNumericMode())
			m_pLanguage->SetBgImg("chat_eng");
		else
			m_pLanguage->SetBgImg("chat_kor");
	}
}

void CLobbyMain::OnChatInputInit(int param)
{
	m_pChatInput = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pChatInput)
	{
		m_pChatInput->SetKeyFocus(true);
	}
}

bool CLobbyMain::OnChatInputEnterKey(int param)
{
	if (strlen((const char*)param))
	{
		if (!Doc()->UTIL_SendChatMessage(m_pChatTarget, (const char*)param,
				true))
		{
			m_pChatTarget->SetLine(1, "\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4", 0,
				false, 0);
			HandleMsg(MsgObject(NULL, 3,
				(int)"\xb1\xd3\xb8\xbb \xbb\xf3\xb4\xeb\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \"\xb8\xf0\xb5\xce\xbf\xa1\xb0\xd4\"\xb7\xce \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9.",
				0xffaf00a1, 0, 0, 0));
		}
	}

	return true;
}

void CLobbyMain::OnReportBtnUp()
{
	m_pReportDlg =
		CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report");

	std::string msg;
	msg =
		"1. \xb4\xeb\xc8\xad\xc3\xa2\xc0\xc7 \xc1\xf6\xb3\xad \xb3\xbb\xbf\xeb\xb5\xb5 \xb8\xf0\xb5\xce \xbc\xad\xb9\xf6\xbf\xa1 \xb1\xe2\xb7\xcf\xb5\xcb\xb4\xcf\xb4\xd9.\n";
	msg +=
		"2. \xc1\xb6\xc0\xdb, \xc7\xe3\xc0\xa7, \xb9\xdd\xba\xb9\xbd\xc5\xb0\xed\xb4\xc2 \xc3\xb3\xb9\xfa\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.\n";
	msg +=
		"3. \xbd\xc5\xb0\xed\xc7\xcf\xbd\xc5 \xb3\xbb\xbf\xeb\xc0\xba 48\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xc3\xb3\xb8\xae\xb5\xcb\xb4\xcf\xb4\xd9.";

	m_pReportDlg->SetMessage(msg.c_str(), false);
	m_pReportDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnReportDlgResult, 3);
}

void CLobbyMain::OnChatTargetInit(int param)
{
	m_pChatTarget = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
	m_pChatTarget->HidePrivacy(true);
	Doc()->InitReservedChatList(m_pChatTarget,
		(Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
			? (eReservedChatPartner)3
			: (eReservedChatPartner)2);
}

void CLobbyMain::OnChatTargetBtnDown(int param)
{
	m_pChatInput->SetKeyFocus(false);
}

bool CLobbyMain::OnChatTargetEnterKey(int param)
{
	m_pChatInput->SetKeyFocus(true);
	return false;
}

void CLobbyMain::OnEmoticonInit(int param)
{
	m_pEmoticon = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CLobbyMain::OnEmoticonBtnUp()
{
	if (m_pEmoticon && !m_pEmoticonDlg)
	{
		m_pEmoticonDlg =
			CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
		if (m_pEmoticonDlg)
		{
			WPoint pos(0.0f, 0.0f);

			if (m_pEmoticon)
			{
				pos.x = m_pEmoticon->GetRect().x - 200.0f;
				pos.y = m_pEmoticon->GetRect().y - 260.0f;
			}

			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&CLobbyMain::OnEmoticonResult, pos, 3);
		}
	}
}

void CLobbyMain::OnChannelRoomCloseBtnInit(int param)
{
	m_pRoomClose = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_Init(int param)
{
	CTaskMain::OnInit();

	m_bIdle = false;
	m_bReady = false;
	m_bLockControls = false;
	m_bCanStart = false;
	m_bAutoStart = false;

	if (Doc()->m_myInfo.info.dwIdentity == 2)
	{
		m_bReady = true;
		m_bLockControls = true;
	}

	m_bRoomStateReq = false;
	m_selOID = 0xffffffff;
	m_selUID = 0xffffffff;
	m_pCurTip = NULL;
	m_pStartTip = NULL;
	m_pReadyTip = NULL;

	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();

	if (Doc()->m_roomInfo.realGameType == 14)
	{
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
			"chaos_background.jpg", true);

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xbd\xba\xc6\xf3\xbc\xc8 \xbc\xc5\xc7\xc3 \xb8\xf0\xb5\xe5\xbf\xa1\xbc\xad\xb4\xc2, \n\\c0xffff0000\\c\xb0\xd4\xc0\xd3 \xc1\xdf \xc0\xd4\xc0\xe5\\c0xff000000\\c \xb9\xd7 \\c0xffff0000\\c\xc6\xbc\xc5\xb0\xb8\xae\xc6\xf7\xc6\xae\\c0xff000000\\c \xbb\xe7\xbf\xeb\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9 \xc7\xd5\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}
	else
	{
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
			"gameroom_background.jpg", true);
	}

	SetTimeVariableBGM();
}

void CLobbyMain::OnGameRoom_Finish()
{
	m_pOverBarTitle->SetBgImg("title_vs");

	for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
		it != Doc()->m_slotList.end(); ++it)
	{
		if (m_pRoomUser)
			m_pRoomUser->AddItem(&(*it));

		ShowPet((*it).dwGuid);
	}

	HandleMsg(MsgObject(this, 0x11, Doc()->m_roomInfo.mapType, 0, 0, 0, 0));

	if (Doc()->m_bRefreshCamera)
		HandleMsg(MsgObject(NULL, 0xaa, 1, 0, 0, 0, 0));

	EnableUnderBar(false);

	OpenNewRecordForm();
}

void CLobbyMain::OnGameRoom_Destroy()
{
	EnableUnderBar(true);
}

void CLobbyMain::OnGameRoom_BackGroundInit(int param)
{
	m_pBackGround = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pBackGround)
		m_pBackGround->SetMouseEvent(false);
}

void CLobbyMain::OnGameRoom_Pet0Init(int param)
{
	m_pPet[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_Pet1Init(int param)
{
	m_pPet[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_Pet2Init(int param)
{
	m_pPet[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_Pet3Init(int param)
{
	m_pPet[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::Banish()
{
	if (m_selOID != 0xffffffff)
	{
		WSendPacket packet((enumClientPacket)0x26);
		packet.Encode4(m_selUID);
		packet.Send(TO_GAME);
	}
}

void CLobbyMain::OnGameRoom_RoomUserInit(int param)
{
	m_pRoomUser = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pRoomUser)
	{
		m_pRoomUser->UseRightButton(true);
		m_pRoomUser->UseDummy(true);

		m_pTeamBmp[0] = (Bitmap*)m_pRoomUser->GetBitmap("team_red");
		m_pTeamBmp[1] = (Bitmap*)m_pRoomUser->GetBitmap("team_blue");
		m_pTeamBaseBmp = (Bitmap*)m_pRoomUser->GetBitmap("team_base");
		m_pTeamBase02Bmp = (Bitmap*)m_pRoomUser->GetBitmap("team_base02");
		m_pInviteBmp = (Bitmap*)m_pRoomUser->GetBitmap("invite");

		m_pMasterSignBmp = (Bitmap*)m_pRoomUser->GetBitmap("master_sign");
		m_pReadySignBmp = (Bitmap*)m_pRoomUser->GetBitmap("ready_sign");
		m_pTeamSelectBmp = (Bitmap*)m_pRoomUser->GetBitmap("team_select");

		m_pGenderBmp[0] = (Bitmap*)m_pRoomUser->GetBitmap("i_male");
		m_pGenderBmp[1] = (Bitmap*)m_pRoomUser->GetBitmap("i_female");
		m_pGenderBmp[2] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_02");
		m_pGenderBmp[3] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_02");
		m_pGenderBmp[4] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_03");
		m_pGenderBmp[5] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_03");
		m_pGenderBmp[6] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_manner");
		m_pGenderBmp[7] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_manner");
		m_pGenderBmp[8] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_angel");
		m_pGenderBmp[9] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_angel");

		m_pDiveBmp = (Bitmap*)m_pRoomUser->GetBitmap("dive");

		m_pKickBmp = (Bitmap*)g_pFresh->RegisterBitmap("kick");
	}
}

void CLobbyMain::OnGameRoom_RoomUserOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	if (!pItem->pData)
	{
		pGDI->DrawTexture(m_pTeamBase02Bmp,
			WRect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBase02Bmp->Width(),
				(float)m_pTeamBase02Bmp->Height()),
			0xffffffff, 0);
		return;
	}

	if (IsLocalContent((localContentType_t)0x54))
	{
		if (((sSlotInfo*)pItem->pData)->IsInvite)
		{
			pGDI->DrawTexture(m_pInviteBmp,
				WRect(pItem->pos.x, pItem->pos.y, (float)m_pInviteBmp->Width(),
					(float)m_pInviteBmp->Height()),
				0xffffffff, 0);
		}
	}

	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (!pSlot)
		return;

	if (!Doc()->m_roomSlotMap[pSlot->dwGuid].pExhibition)
	{
		Doc()->m_roomSlotMap[pSlot->dwGuid].pArea =
			m_pPet[pSlot->connectionRank - 1];
		Doc()->m_roomSlotMap[pSlot->dwGuid].pExhibition = NULL;
		Doc()->m_roomSlotMap[pSlot->dwGuid].bAngelWing = pSlot->angelicWings;
		Doc()->m_roomSlotMap[pSlot->dwGuid].bGachaWing =
			pSlot->angelicWingsEffect;

		if (IsLocalContent((localContentType_t)0x54))
		{
			if (!pSlot->IsInvite)
				ShowPet(pSlot->dwGuid);
		}
		else
		{
			ShowPet(pSlot->dwGuid);
		}
	}

	if (IsLocalContent((localContentType_t)0x54))
	{
		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBaseBmp->Width(),
			(float)m_pTeamBaseBmp->Height());

		if (!pSlot->IsInvite)
		{
			if (!m_bBongdariShop2)
			{
				std::map<unsigned long, sRoomSlot>::iterator it =
					Doc()->m_roomSlotMap.find(pSlot->dwGuid);
				if (it != Doc()->m_roomSlotMap.end())
				{
					if ((*it).second.pExhibition)
						(*it).second.pExhibition->Display(0.0f, 0.0f);
				}
			}

			if (Doc()->m_roomInfo.gameType == GAME_TYPE_TEAM)
				pGDI->DrawTexture(m_pTeamBmp[pSlot->bTeam], rect, 0xffffffff,
					0);
			else
				pGDI->DrawTexture(m_pTeamBaseBmp, rect, 0xffffffff, 0);
		}
		else
		{
			pGDI->DrawTexture(m_pInviteBmp, rect, 0xffffffff, 0);
		}

		if (pSlot->tidSkin[3])
		{
			IFF_STRUCT::sSkin* pSkin =
				ItemManager()->FindSkin(pSlot->tidSkin[3]);
			if (pSkin)
			{
				const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
				pGDI->DrawTexture(pBmp, rect, 0xffffffff, 0);
			}
		}

		if (Doc()->m_myInfo.info.dwGuid == m_masterOID && !pSlot->bMaster &&
			!(Doc()->m_curGameServer.property & 4))
		{
			pGDI->DrawTexture(m_pKickBmp,
				WRect(rect.x + rect.w - 8.0f - m_pKickBmp->Width(),
					rect.y + 8.0f, (float)m_pKickBmp->Width(),
					(float)m_pKickBmp->Height()),
				0xffffffff, 0);
		}

		if (pSlot->dwGuid == m_selOID)
			pGDI->DrawTexture(m_pTeamSelectBmp, rect, 0xffffffff, 0);

		int x =
			(m_pTeamBmp[pSlot->bTeam]->Width() - m_pReadySignBmp->Width()) / 2 +
			(int)pItem->pos.x;

		if (pSlot->bMaster)
		{
			rect = WRect((float)x, pItem->pos.y + 6.0f,
				(float)m_pMasterSignBmp->Width(),
				(float)m_pMasterSignBmp->Height());
			pGDI->DrawTexture(m_pMasterSignBmp, rect, 0xffffffff, 0);
		}
		else if (pSlot->bReady)
		{
			rect = WRect((float)x, pItem->pos.y + 6.0f,
				(float)m_pReadySignBmp->Width(),
				(float)m_pReadySignBmp->Height());
			pGDI->DrawTexture(m_pReadySignBmp, rect, 0xffffffff, 0);
		}

		const Bitmap* pCharBmp = NULL;
		if (!pSlot->tidSkin[4])
		{
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->tidChar);
			if (pChar)
				pCharBmp = g_pFresh->GetBitmap(pChar->c.Icon);
		}
		else
		{
			IFF_STRUCT::sSkin* pSkin =
				ItemManager()->FindSkin(pSlot->tidSkin[4]);
			if (pSkin)
				pCharBmp = g_pFresh->GetBitmap(pSkin->Tex);
		}

		if (pCharBmp)
		{
			rect.x = pItem->pos.x + 8.0f;
			rect.y = pItem->pos.y + 36.0f;
			rect.w = (float)pCharBmp->Width();
			rect.h = (float)pCharBmp->Height();

			pGDI->DrawTexture(pCharBmp, rect, 0xffffffff, 0);
		}

		if (pSlot->bSleep)
		{
			pGDI->DrawTexture(m_pDiveBmp,
				WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f,
					(float)m_pDiveBmp->Width(), (float)m_pDiveBmp->Height()),
				0xffffffff, 0);
		}

		const Bitmap* pGenderBmp;
		if (pSlot->angelicWings)
			pGenderBmp = m_pGenderBmp[8 + pSlot->gender % 2];
		else if (pSlot->manner)
			pGenderBmp = m_pGenderBmp[6 + pSlot->gender % 2];
		else
			pGenderBmp = m_pGenderBmp[pSlot->gender];

		if (pGenderBmp)
		{
			pGDI->DrawTexture(pGenderBmp,
				WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f,
					(float)pGenderBmp->Width(), (float)pGenderBmp->Height()),
				0xffffffff, 0);
		}

		const Bitmap* pBmp = NULL;
		if (Doc()->m_curChannel.Type & 0x80)
		{
			pBmp =
				g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->ladderGrade));
		}
		else if (pSlot->dwTitle)
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->dwTitle);
			if (pSkin)
				pBmp = g_pFresh->GetBitmap(pSkin->c.Icon);
		}
		else if (!((sSlotInfo*)pItem->pData)->IsInvite)
		{
			pBmp = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->level + 1));
		}

		if (pBmp)
		{
			pGDI->DrawTexture(pBmp,
				WRect(pItem->pos.x + 90.0f - float2int(pBmp->Width() * 0.5f),
					pItem->pos.y + 68.0f, (float)pBmp->Width(),
					(float)pBmp->Height()),
				0xffffffff, 0);
		}

		if (pSlot->GuildId && pSlot->IsInvite)
		{
			const Bitmap* pEmblem =
				NetResourceManager::Instance()->GetEmblemByName(
					pSlot->szEmblemName);
			if (pEmblem)
			{
				pGDI->DrawTexture(pEmblem,
					WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f,
						(float)pEmblem->Width(), (float)pEmblem->Height()),
					0xffffffff, 0);
			}
		}

		if (CProjectG::Instance()->HidePrivacy())
			return;

		if (pItem->underCursor)
		{
			pGDI->SetTextColor(0xffffffff, 0xff808080);
			pGDI->SetTextStyle(2);
		}
		else
		{
			if (pSlot->dwGuid == MyGuid(true))
				pGDI->SetTextColor(0xffff0000, 0xffffffff);
			else
				pGDI->SetTextColor(0xff000000, 0xffffffff);
			pGDI->SetTextStyle(0);
		}

		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 107.0f), 0,
			pSlot->sNick, -1.0f, 0xffffffff);
	}
	else
	{
		if (!m_bBongdariShop2)
		{
			std::map<unsigned long, sRoomSlot>::iterator it =
				Doc()->m_roomSlotMap.find(pSlot->dwGuid);
			if (it != Doc()->m_roomSlotMap.end())
			{
				if ((*it).second.pExhibition)
					(*it).second.pExhibition->Display(0.0f, 0.0f);
			}
		}

		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBaseBmp->Width(),
			(float)m_pTeamBaseBmp->Height());

		if (Doc()->m_roomInfo.gameType == GAME_TYPE_TEAM)
			pGDI->DrawTexture(m_pTeamBmp[pSlot->bTeam], rect, 0xffffffff, 0);
		else
			pGDI->DrawTexture(m_pTeamBaseBmp, rect, 0xffffffff, 0);

		if (pSlot->tidSkin[3])
		{
			IFF_STRUCT::sSkin* pSkin =
				ItemManager()->FindSkin(pSlot->tidSkin[3]);
			if (pSkin)
			{
				const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
				pGDI->DrawTexture(pBmp, rect, 0xffffffff, 0);
			}
		}

		if (Doc()->m_myInfo.info.dwGuid == m_masterOID && !pSlot->bMaster &&
			!(Doc()->m_curGameServer.property & 4))
		{
			pGDI->DrawTexture(m_pKickBmp,
				WRect(rect.x + rect.w - 8.0f - m_pKickBmp->Width(),
					rect.y + 8.0f, (float)m_pKickBmp->Width(),
					(float)m_pKickBmp->Height()),
				0xffffffff, 0);
		}

		if (pSlot->dwGuid == m_selOID)
			pGDI->DrawTexture(m_pTeamSelectBmp, rect, 0xffffffff, 0);

		int x =
			(m_pTeamBmp[pSlot->bTeam]->Width() - m_pReadySignBmp->Width()) / 2 +
			(int)pItem->pos.x;

		if (pSlot->bMaster)
		{
			rect = WRect((float)x, pItem->pos.y + 6.0f,
				(float)m_pMasterSignBmp->Width(),
				(float)m_pMasterSignBmp->Height());
			pGDI->DrawTexture(m_pMasterSignBmp, rect, 0xffffffff, 0);
		}
		else if (pSlot->bReady)
		{
			rect = WRect((float)x, pItem->pos.y + 6.0f,
				(float)m_pReadySignBmp->Width(),
				(float)m_pReadySignBmp->Height());
			pGDI->DrawTexture(m_pReadySignBmp, rect, 0xffffffff, 0);
		}

		const Bitmap* pCharBmp = NULL;
		if (!pSlot->tidSkin[4])
		{
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->tidChar);
			if (pChar)
				pCharBmp = g_pFresh->GetBitmap(pChar->c.Icon);
		}
		else
		{
			IFF_STRUCT::sSkin* pSkin =
				ItemManager()->FindSkin(pSlot->tidSkin[4]);
			if (pSkin)
				pCharBmp = g_pFresh->GetBitmap(pSkin->Tex);
		}

		if (pCharBmp)
		{
			rect.x = pItem->pos.x + 8.0f;
			rect.y = pItem->pos.y + 36.0f;
			rect.w = (float)pCharBmp->Width();
			rect.h = (float)pCharBmp->Height();

			pGDI->DrawTexture(pCharBmp, rect, 0xffffffff, 0);
		}

		if (pSlot->bSleep)
		{
			pGDI->DrawTexture(m_pDiveBmp,
				WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f,
					(float)m_pDiveBmp->Width(), (float)m_pDiveBmp->Height()),
				0xffffffff, 0);
		}

		const Bitmap* pGenderBmp;
		if (pSlot->angelicWings)
			pGenderBmp = m_pGenderBmp[8 + pSlot->gender % 2];
		else if (pSlot->manner)
			pGenderBmp = m_pGenderBmp[6 + pSlot->gender % 2];
		else
			pGenderBmp = m_pGenderBmp[pSlot->gender];

		if (pGenderBmp)
		{
			pGDI->DrawTexture(pGenderBmp,
				WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f,
					(float)pGenderBmp->Width(), (float)pGenderBmp->Height()),
				0xffffffff, 0);
		}

		const Bitmap* pBmp;
		if (Doc()->m_curChannel.Type & 0x80)
		{
			pBmp =
				g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->ladderGrade));
		}
		else if (pSlot->dwTitle)
		{
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->dwTitle);
			if (pSkin)
				pBmp = g_pFresh->GetBitmap(pSkin->c.Icon);
			else
				pBmp = g_pFresh->GetBitmap(
					MakeStr("level_%03d", pSlot->level + 1));
		}
		else
		{
			pBmp = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->level + 1));
		}

		if (pBmp)
		{
			pGDI->DrawTexture(pBmp,
				WRect(pItem->pos.x + 60.0f, pItem->pos.y + 68.0f,
					(float)pBmp->Width(), (float)pBmp->Height()),
				0xffffffff, 0);
		}

		if (pSlot->GuildId)
		{
			const Bitmap* pEmblem =
				NetResourceManager::Instance()->GetEmblemByName(
					pSlot->szEmblemName);
			if (pEmblem)
			{
				pGDI->DrawTexture(pEmblem,
					WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f,
						(float)pEmblem->Width(), (float)pEmblem->Height()),
					0xffffffff, 0);
			}
		}

		if (CProjectG::Instance()->HidePrivacy())
			return;

		if (pItem->underCursor)
		{
			pGDI->SetTextColor(0xffffffff, 0xff808080);
			pGDI->SetTextStyle(2);
		}
		else
		{
			if (pSlot->dwGuid == MyGuid(true))
				pGDI->SetTextColor(0xffff0000, 0xffffffff);
			else
				pGDI->SetTextColor(0xff000000, 0xffffffff);
			pGDI->SetTextStyle(0);
		}

		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 107.0f), 0,
			pSlot->sNick, -1.0f, 0xffffffff);
	}
}

void CLobbyMain::OnGameRoom_RoomUserLBtnUp()
{
	if (m_pRoomUser == NULL)
		return;

	FrListItem* pItem = m_pRoomUser->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (pSlot == NULL)
		return;

	if (pSlot->dwGuid != m_selOID)
	{
		m_selOID = pSlot->dwGuid;
		m_selUID = pSlot->dwUserUID;
	}

	WRect kickRect;
	if (!(Doc()->m_curGameServer.property & 4))
	{
		if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0)
			kickRect = WRect((float)m_pTeamBmp[pSlot->bTeam]->Width() - 8.0f -
					(float)m_pKickBmp->Width(),
				8.0f, (float)m_pKickBmp->Width(), (float)m_pKickBmp->Height());
		else
			kickRect = WRect(227.0f, 5.0f, (float)m_pKickBmp->Width(),
				(float)m_pKickBmp->Height());
	}

	if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") != 0 &&
		m_masterOID == MyGuid(false) && m_selOID != MyGuid(false) &&
		(kickRect + pItem->pos)
			.IsInRect(g_pFresh->GetManager()->GetMousePos()) &&
		Doc()->m_roomInfo.gameType != GAME_TYPE_GUILD_MATCH)
	{
		if (IsLocalContent((localContentType_t)84))
		{
			if (pSlot->IsInvite)
				return;
		}
		Banish();
	}
}

void CLobbyMain::OnGameRoom_RoomUserRBtnUp()
{
	OnGameRoom_RoomUserLBtnUp();

	if (CUserInfo::IsInstantiated())
	{
		std::map<unsigned long, sBriefUserInfo>::iterator it =
			Doc()->m_briefUserInfoMap.find(m_selOID);
		if (it != Doc()->m_briefUserInfoMap.end())
			(*it).second.roomIndex = Doc()->m_roomInfo.roomGuid;

		CUserInfo::Instance()->SetInfo((*it).second.dwUid, m_selOID, true, true,
			false, false, (*it).second.sNick);
	}
}

void CLobbyMain::OnGameRoom_RoomUserDClick()
{
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		if (Doc()->m_myInfo.info.dwGalleryGuid != m_selOID)
		{
			WSendPacket send((enumClientPacket)0x3f);
			send.Encode4(m_selUID);
			send.Send(TO_GAME);
		}
	}
}

void CLobbyMain::OnGameRoom_TitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
	HandleMsg(MsgObject(this, 16, 0, 0, 0, 0, 0));

	Doc()->SortAllMyItemList();

	if (Doc()->m_roomInfo.nUserLimit >= 100)
		m_pTitle->SetFontColor(0xff47a5dd);
	else
		m_pTitle->SetFontColor(0xffffffff);
}

void CLobbyMain::OnGameRoom_EventOwnerDraw(int param)
{
	if (Doc()->m_roomInfo.nUserLimit >= 100)
	{
		FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
		if (pDevice)
		{
			if (m_pRoomEvent02Bmp == NULL)
				m_pRoomEvent02Bmp =
					(Bitmap*)g_pFresh->GetBitmap("room_event02");
			if (m_pRoomEvent02Bmp)
				pDevice->DrawTexture(m_pRoomEvent02Bmp,
					WRect(8.0f, 45.0f, 43.0f, 32.0f), 0xffffffff, false);
		}
	}
}

void CLobbyMain::OnGameRoom_MapBgInit(int param)
{
	m_pMapBg = DYNAMIC_CAST(FrViewer, param);
	if (m_pMapBg)
		m_pMapBg->SetMouseEvent(false);
}

void CLobbyMain::OnGameRoom_MapGaugeInit(int param)
{
	m_pMapGauge = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pMapGauge)
	{
		m_pMapGauge->SetRange(0, 300, 0);
		if (GetCurRoomMapType() < 127)
		{
			unsigned long gauge = CTHunter::Instance()->GetTHunterGauge(
				(eMapType)Doc()->m_roomInfo.mapType);
			m_pMapGauge->SetDestPos(gauge < 701 ? 0 : gauge - 700, false);
		}
		else
		{
			m_pMapGauge->SetPos(0);
		}
	}
}

void CLobbyMain::SendChangedUserInfo()
{
	if (m_subSelTypeId == 0)
		return;

	switch (m_subSelTypeId >> 26)
	{
	case 1:
	{
		WSendPacket send((enumClientPacket)0x0c);
		send.Encode1(4);
		send.Encode4(m_subSelItemId);
		send.Send(TO_GAME);
		break;
	}
	case 7:
	{
		WSendPacket send((enumClientPacket)0x0c);
		send.Encode1(1);
		send.Encode4(m_subSelItemId);
		send.Send(TO_GAME);
		break;
	}
	case 4:
	{
		WSendPacket send((enumClientPacket)0x0c);
		send.Encode1(3);
		send.Encode4(m_subSelItemId);
		send.Send(TO_GAME);
		break;
	}
	case 5:
	{
		WSendPacket send((enumClientPacket)0x0c);
		send.Encode1(2);
		send.Encode4(m_subSelTypeId);
		send.Send(TO_GAME);
		break;
	}
	case 16:
	{
		WSendPacket send((enumClientPacket)0x0c);
		send.Encode1(5);
		send.Encode4(m_subSelItemId);
		send.Send(TO_GAME);
		break;
	}
	}
}

void CLobbyMain::InitControlsSelectedUserInfo()
{
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	if (m_pCharSel)
		m_pCharSel->SetVisible(false);
	if (m_pCaddieSel)
		m_pCaddieSel->SetVisible(false);
	if (m_pAztecSel)
		m_pAztecSel->SetVisible(false);
	if (m_pClubSel)
		m_pClubSel->SetVisible(false);
	if (m_pMascotSel)
		m_pMascotSel->SetVisible(false);
}

void CLobbyMain::DrawFrameSelectedUserInfo(WRect rect)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	if (rect.IsInRect(
			WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
	{
		const Bitmap* pFrame = g_pFresh->GetBitmap("gameroom_select_frame");
		if (pFrame)
			pDevice->DrawTexture(pFrame,
				WRect(rect.x - 2.0f, rect.y - 2.0f, pFrame->Width(),
					pFrame->Height()),
				0xffffffff, false);
	}
}

void CLobbyMain::OpenNewRecordForm()
{
	if (Doc()->m_newRecordState != 0xff && m_pNewRecordDlg == NULL)
	{
		m_pNewRecordDlg = CreateForm<FrNewRecordDlg>(g_pFresh->GetManager(),
			this, "new_record", NULL);
		m_pNewRecordDlg->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnNewRecordDlgResult, 0);
		Doc()->m_newRecordState = 0xff;
	}

	if (IsLocalContent((localContentType_t)0x18))
	{
		if (Doc()->m_newRecordCourse == -1)
			Doc()->m_newRecordCourse = Doc()->m_myInfo.stat.eventFlag;

		if (Doc()->m_newRecordCourse != Doc()->m_myInfo.stat.eventFlag)
		{
			if (IsLocalContent((localContentType_t)0x48))
				m_pHolesEventDlg = CreateForm<ntHolesEventDlg>(
					g_pFresh->GetManager(), this, "holes_EntranceExam", NULL);
			else
				m_pHolesEventDlg = CreateForm<ntHolesEventDlg>(
					g_pFresh->GetManager(), this, "holes_event", NULL);

			m_pHolesEventDlg->Open(
				(FRESH_PFN_RESULT)&CLobbyMain::OnHolesEventDlgResult, 0x11);
			Doc()->m_newRecordCourse = Doc()->m_myInfo.stat.eventFlag;
		}
	}

	if (IsLocalContent((localContentType_t)0x21))
	{
		CHalloweenEvent* pEvent =
			(CHalloweenEvent*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)0x21);
		if (pEvent)
		{
			if (pEvent->GetShowGiftDlg())
			{
				if (m_pHalloweenEventGiftDlg == NULL)
				{
					m_pHalloweenEventGiftDlg =
						CreateForm<FrHalloweenEventGiftDlg>(
							g_pFresh->GetManager(), this,
							"halloween2007_event_gift_dlg", NULL);
					if (m_pHalloweenEventGiftDlg)
					{
						m_pHalloweenEventGiftDlg->Open(NULL, 0x11);
						m_pHalloweenEventGiftDlg = NULL;
					}
				}
				pEvent->SetShowGiftDlg(false);
			}
		}
	}
}

void CLobbyMain::GameExtResQuit()
{
	if (!IsLocalContent((localContentType_t)20))
		m_bUseReport = false;

	if (m_pUniteResultDlg)
		return;

	if (m_bUseReport && Doc()->m_roomInfo.realGameType != 14)
		OpenUseReportForm();
	else
		OpenGameExtResQuitForm();
}

void CLobbyMain::OpenUseReportForm()
{
	if (m_pUseReportDlg)
		return;

	m_pUseReportDlg =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	m_pUseReportDlg->SetMessage(
		"\xc6\xbc\xc5\xb0 \xb8\xae\xc6\xf7\xc6\xae "
		"\xbf\xeb\xc1\xf6\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf"
		"\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	m_pUseReportDlg->Open(
		(FRESH_PFN_RESULT)&CLobbyMain::OnRoomUseReportDlgResult, 1);
}

void CLobbyMain::OpenGameExtResQuitForm()
{
	if (m_pExtResQuitDlg)
		return;

	m_pExtResQuitDlg =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	m_pExtResQuitDlg->SetMessage(
		"\xb0\xd4\xc0\xd3\xc0\xcc \\c0xffff0000\\c\xbf\xcf\xc0\xfc\xc8\xf7 "
		"\xc1\xbe\xb7\xe1\\c0xff000000\\c\xb5\xc7\xb1\xe2 \xc0\xfc\xbf\xa1 "
		"\xb0\xd4\xc0\xd3\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb8\xe9, "
		"\\c0xffff0000\\c\xb0\xe6\xc7\xe8\xc4\xa1 \xc8\xb9\xb5\xe6\xb0\xfa "
		"\xb0\xa2\xc1\xbe \xb1\xe2\xb7\xcf\xc0\xcc "
		"\xb9\xab\xc8\xbf\xc3\xb3\xb8\xae\\c0xff000000\\c\xb5\xc7\xc1\xf6\xb8\xb8 "
		"\\c0xffff0000\\c\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xba "
		"\xbf\xc3\xb6\xf3\xb0\xa1\xc1\xf6 "
		"\xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.\\c0xff000000\\c\n\n\xb0\xd4\xc0\xd3"
		"\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed \xb7\xce\xba\xf1\xb7\xce "
		"\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
		false);
	m_pExtResQuitDlg->Open(
		(FRESH_PFN_RESULT)&CLobbyMain::OnRoomExtResQuitDlgResult, 1);
}

void CLobbyMain::OnGameRoom_CharInit(int param)
{
	m_pChar = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_CharDown()
{
	WRect rect = m_pCharSel->GetRect();
	WPoint pos(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pos, (PANGYA_ITEM_GROUP)1, 1);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth())
			barRect.x = g_view->GetWidth() - barRect.w;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CLobbyMain::OnGameRoom_CharOwnerDraw(int param)
{
	if (m_pChar == NULL)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	unsigned long color = m_pChar->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

	if (Doc()->m_myInfo.userEquip.guidChar == 0)
	{
		const Bitmap* pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBitmap)
			pDevice->DrawTexture(pBitmap, m_pChar->GetRect(), color, false);
	}
	else
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			const Bitmap* pBitmap =
				g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pDevice->DrawTexture(pBitmap,
					WRect(m_pChar->GetRect().x, m_pChar->GetRect().y,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					color, false);
		}
	}

	if (m_pChar->IsEnabled())
		DrawFrameSelectedUserInfo(m_pChar->GetRect());
}

void CLobbyMain::OnGameRoom_CaddieInit(int param)
{
	m_pCaddie = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_CaddieDown()
{
	WRect rect = m_pCaddieSel->GetRect();
	WPoint pos(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pos, (PANGYA_ITEM_GROUP)7, 1);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth())
			barRect.x = g_view->GetWidth() - barRect.w;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CLobbyMain::OnGameRoom_CaddieOwnerDraw(int param)
{
	if (m_pCaddie == NULL)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	unsigned long color = m_pCaddie->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

	WRect rect = m_pCaddie->GetRect();
	if (Doc()->m_myInfo.userEquip.guidCaddie == 0)
	{
		const Bitmap* pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBitmap)
			pDevice->DrawTexture(pBitmap,
				WRect(m_pCaddie->GetRect().x, m_pCaddie->GetRect().y,
					(float)pBitmap->Width(), (float)pBitmap->Height()),
				color, false);
	}
	else
	{
		std::map<unsigned int, sCaddieInfo>::iterator it =
			Doc()->m_caddieMap.find(Doc()->m_myInfo.userEquip.guidCaddie);

		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			const Bitmap* pBitmap =
				g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pDevice->DrawTexture(pBitmap,
					WRect(m_pCaddie->GetRect().x, m_pCaddie->GetRect().y,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					color, false);
		}

		IFF_STRUCT::sCaddie* pCaddie =
			ItemManager()->FindCaddie((*it).second.tid);
		if (pCaddie && (*it).second.Rent_flag &&
			(*it).second.Remain_Date <= 0 && (*it).second.tidPart == 0 &&
			pCaddie->MonthlyFee)
		{
			g_view->DrawLine2D(WPoint(rect.x, rect.h * 0.5f + rect.y - 1.0f),
				WPoint(rect.x + rect.w - 1.0f, rect.h * 0.5f + rect.y - 1.0f),
				0xffff0000, 0xffff0000, 0);
			g_view->DrawLine2D(WPoint(rect.x, rect.h * 0.5f + rect.y),
				WPoint(rect.x + rect.w - 1.0f, rect.h * 0.5f + rect.y),
				0xffff0000, 0xffff0000, 0);
			g_view->DrawLine2D(WPoint(rect.x, rect.h * 0.5f + rect.y + 1.0f),
				WPoint(rect.x + rect.w - 1.0f, rect.h * 0.5f + rect.y + 1.0f),
				0xffff0000, 0xffff0000, 0);
		}
	}

	if (m_pCaddie->IsEnabled())
		DrawFrameSelectedUserInfo(m_pCaddie->GetRect());
}

void CLobbyMain::OnGameRoom_ClubInit(int param)
{
	m_pClub = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_ClubDown()
{
	WRect rect = m_pClubSel->GetRect();
	WPoint pos(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pos, (PANGYA_ITEM_GROUP)4, 1);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth())
			barRect.x = g_view->GetWidth() - barRect.w;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CLobbyMain::OnGameRoom_ClubOwnerDraw(int param)
{
	if (m_pClub == NULL)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	unsigned long color = m_pClub->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

	if (Doc()->m_myInfo.userEquip.guidClubSet == 0)
	{
		const Bitmap* pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBitmap)
			pDevice->DrawTexture(pBitmap, m_pClub->GetRect(), color, false);
	}
	else
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(Doc()->m_myInfo.userEquip.guidClubSet);
		if (it != Doc()->m_clubSetMap.end())
		{
			IFF_ITEM_COMMON* pItem =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (pItem)
			{
				const Bitmap* pBitmap =
					g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
				if (pBitmap)
					pDevice->DrawTexture(pBitmap,
						WRect(m_pClub->GetRect().x, m_pClub->GetRect().y,
							(float)pBitmap->Width(), (float)pBitmap->Height()),
						color, false);
			}
		}
	}

	if (m_pClub->IsEnabled())
		DrawFrameSelectedUserInfo(m_pClub->GetRect());
}

void CLobbyMain::OnGameRoom_AztecInit(int param)
{
	m_pAztec = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_AztecDown()
{
	WRect rect = m_pAztecSel->GetRect();
	WPoint pos(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pos, (PANGYA_ITEM_GROUP)5, 1);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth())
			barRect.x = g_view->GetWidth() - barRect.w;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CLobbyMain::OnGameRoom_AztecOwnerDraw(int param)
{
	if (m_pAztec == NULL)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	unsigned long color = m_pAztec->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

	if (Doc()->m_myInfo.userEquip.tidBall == 0)
	{
		const Bitmap* pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBitmap)
			pDevice->DrawTexture(pBitmap, m_pAztec->GetRect(), color, false);
	}
	else
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem(Doc()->m_myInfo.userEquip.tidBall);

		if (pItem)
		{
			int count = 0;
			if (IsLocalContent((localContentType_t)0x87))
			{
				sItemInfo* pInfo = NULL;
				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_ballList.begin();
					it != Doc()->m_ballList.end(); ++it)
				{
					if ((*it).tid == Doc()->m_myInfo.userEquip.tidBall)
					{
						pInfo = &(*it);
						count = (*it).Common[0];
						break;
					}
				}

				if (pInfo == NULL)
				{
					Doc()->m_myInfo.userEquip.tidBall = 0x14000000;
					pItem = ItemManager()->FindCommonItem(
						Doc()->m_myInfo.userEquip.tidBall);
				}

				const Bitmap* pBitmap =
					g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
				if (pBitmap)
					pDevice->DrawTexture(pBitmap,
						WRect(m_pAztec->GetRect().x, m_pAztec->GetRect().y,
							(float)pBitmap->Width(), (float)pBitmap->Height()),
						color, false);
			}
			else
			{
				const Bitmap* pBitmap =
					g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
				if (pBitmap)
					pDevice->DrawTexture(pBitmap,
						WRect(m_pAztec->GetRect().x, m_pAztec->GetRect().y,
							(float)pBitmap->Width(), (float)pBitmap->Height()),
						color, false);

				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_ballList.begin();
					it != Doc()->m_ballList.end(); ++it)
				{
					if ((*it).tid == Doc()->m_myInfo.userEquip.tidBall)
					{
						count = (*it).Common[0];
						break;
					}
				}
			}

			if (count > 0)
			{
				unsigned long textColor =
					m_pAztec->IsEnabled() ? 0xffffffff : 0x7fffffff;
				Print(m_pAztec->GetRect().x + 35.0f,
					m_pAztec->GetRect().y + 22.0f, MakeStr("%d", count), 2.0f,
					2, 1.0f, textColor);
			}
		}
	}

	if (m_pAztec->IsEnabled())
		DrawFrameSelectedUserInfo(m_pAztec->GetRect());
}

void CLobbyMain::OnGameRoom_MascotInit(int param)
{
	m_pMascot = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_MascotDown()
{
	WRect rect = m_pMascotSel->GetRect();
	WPoint pos(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pos, (PANGYA_ITEM_GROUP)16, 1);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth())
			barRect.x = g_view->GetWidth() - barRect.w;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CLobbyMain::OnGameRoom_MascotOwnerDraw(int param)
{
	if (m_pMascot == NULL)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	unsigned long color = m_pMascot->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

	if (Doc()->m_myInfo.userEquip.guidMascot == 0)
	{
		const Bitmap* pBitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBitmap)
			pDevice->DrawTexture(pBitmap,
				WRect(m_pMascot->GetRect().x, m_pMascot->GetRect().y,
					(float)pBitmap->Width(), (float)pBitmap->Height()),
				color, false);
	}
	else
	{
		std::map<unsigned int, sMascotInfo>::iterator it =
			Doc()->m_mascotMap.find(Doc()->m_myInfo.userEquip.guidMascot);
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			const Bitmap* pBitmap =
				g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pDevice->DrawTexture(pBitmap,
					WRect(m_pMascot->GetRect().x, m_pMascot->GetRect().y,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					color, false);
		}
	}

	if (m_pMascot->IsEnabled())
		DrawFrameSelectedUserInfo(m_pMascot->GetRect());
}

void CLobbyMain::OnGameRoom_EquipItemInit(int param)
{
	m_pEquipItem = DYNAMIC_CAST(FrViewer, param);
}

void CLobbyMain::OnGameRoom_EquipItemDown()
{
	InitControlsSelectedUserInfo();

	if (m_pEquipDlg == NULL)
	{
		m_pEquipDlg =
			CreateForm<FrEquipDlg>(g_pFresh->GetManager(), this, "equip", NULL);
		if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			m_pEquipDlg->Init(Doc()->m_userInfo[0].userEquip.tidItemSlot, true);
		else
			m_pEquipDlg->Init(Doc()->m_myInfo.userEquip.tidItemSlot, true);

		m_pEquipDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnItemEquipResult, 3);
	}
}

void CLobbyMain::OnGameRoom_EquipItemOwnerDraw(int param)
{
	if (m_pEquipItem)
	{
		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

		if (pGDI)
		{
			unsigned long color =
				m_pEquipItem->IsEnabled() ? 0xffffffff : 0x50b0b0b0;

			const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS",
				"gameroom_equip_item_bn");

			if (pBitmap)
			{
				pGDI->DrawTexture(pBitmap,
					WRect(m_pEquipItem->GetRect().x, m_pEquipItem->GetRect().y,
						(float)pBitmap->Width(), (float)pBitmap->Height()),
					color, 0);
			}

			if (m_pEquipItem->IsEnabled())
				DrawFrameSelectedUserInfo(m_pEquipItem->GetRect());
		}
	}
}

void CLobbyMain::OnGameRoom_CharSelInit(int param)
{
	m_pCharSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);

	if (m_pCharSel)
	{
		m_pCharSel->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoom_CharSelDown()
{
	InitControlsSelectedUserInfo();
}

void CLobbyMain::OnGameRoom_CharSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_charMap.size();
	if (count <= 1)
		return;

	float x = m_pCharSel->GetRect().x;
	float y = m_pCharSel->GetRect().y;
	float w = 0.0f;
	float total = (float)count * 41.0f + 8.0f;

	if (x + total > 800.0f)
		x = 800.0f - total;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBitmap)
	{
		w = (float)count * 38.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	m_subSelTypeId = 0;
	m_subSelItemId = 0;
	y += 4.0f;

	std::map<unsigned int, sCharacterInfo>::iterator it;
	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); it++)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			WRect dest(startX, y, 38.0f, 38.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

			if (dest.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
							dest.h + 6.0f),
						0xffffffff, 0);

				m_subSelTypeId = (*it).second.tid;
				m_subSelItemId = (*it).second.guid;
			}

			startX += 41.0f;
		}
	}
}

void CLobbyMain::OnGameRoom_CaddieSelInit(int param)
{
	m_pCaddieSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);

	if (m_pCaddieSel)
	{
		m_pCaddieSel->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoom_CaddieSelDown()
{
	InitControlsSelectedUserInfo();
}

void CLobbyMain::OnGameRoom_CaddieSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_caddieMap.size();
	if (count <= 1)
		return;

	if (Doc()->m_myInfo.userEquip.guidCaddie)
		count++;

	float x = m_pCaddieSel->GetRect().x;
	float y = m_pCaddieSel->GetRect().y;
	float w = 0.0f;
	float total = (float)count * 41.0f + 8.0f;

	if (x + total > 800.0f)
		x = 800.0f - total;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBitmap)
	{
		w = (float)count * 38.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	m_subSelTypeId = 0;
	m_subSelItemId = 0;
	y += 4.0f;

	std::map<unsigned int, sCaddieInfo>::iterator it;
	for (it = Doc()->m_caddieMap.begin(); it != Doc()->m_caddieMap.end(); it++)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			WRect dest(startX, y, 38.0f, 38.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

			IFF_STRUCT::sCaddie* pCaddie =
				ItemManager()->FindCaddie((*it).second.tid);
			if (pCaddie)
			{
				if ((*it).second.Rent_flag && (*it).second.Remain_Date <= 0 &&
					(*it).second.tidPart == 0 && pCaddie->MonthlyFee)
				{
					g_view->DrawLine2D(
						WPoint(dest.x, dest.y + dest.h * 0.5f - 1.0f),
						WPoint(dest.x + dest.w - 1.0f,
							dest.y + dest.h * 0.5f - 1.0f),
						0xffff0000, 0xffff0000, 0);
					g_view->DrawLine2D(WPoint(dest.x, dest.y + dest.h * 0.5f),
						WPoint(dest.x + dest.w - 1.0f, dest.y + dest.h * 0.5f),
						0xffff0000, 0xffff0000, 0);
					g_view->DrawLine2D(
						WPoint(dest.x, dest.y + dest.h * 0.5f + 1.0f),
						WPoint(dest.x + dest.w - 1.0f,
							dest.y + dest.h * 0.5f + 1.0f),
						0xffff0000, 0xffff0000, 0);
				}
			}

			if (dest.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
							dest.h + 6.0f),
						0xffffffff, 0);

				m_subSelTypeId = (*it).second.tid;
				m_subSelItemId = (*it).second.guid;
			}

			startX += 41.0f;
		}
	}

	if (Doc()->m_myInfo.userEquip.guidCaddie)
	{
		WRect dest(startX, y, 38.0f, 38.0f);

		pBitmap = g_pFresh->GetBitmap("w_caddie_no");
		if (pBitmap)
			pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

		if (dest.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			pBitmap = g_pFresh->GetBitmap("select_frame");
			if (pBitmap)
				pGDI->DrawTexture(pBitmap,
					WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
						dest.h + 6.0f),
					0xffffffff, 0);

			m_subSelTypeId = 0x1c000000;
			m_subSelItemId = 0;
		}
	}
}

void CLobbyMain::OnGameRoom_ClubSelInit(int param)
{
	m_pClubSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);

	if (m_pClubSel)
	{
		m_pClubSel->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoom_ClubSelDown()
{
	InitControlsSelectedUserInfo();
}

void CLobbyMain::OnGameRoom_ClubSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_clubSetMap.size();
	if (count <= 1)
		return;

	float x = m_pClubSel->GetRect().x;
	float y = m_pClubSel->GetRect().y;
	float w = 0.0f;
	float total = (float)count * 41.0f + 8.0f;

	if (x + total > 800.0f)
		x = 800.0f - total;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBitmap)
	{
		w = (float)count * 38.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	m_subSelTypeId = 0;
	m_subSelItemId = 0;
	y += 4.0f;

	std::map<unsigned int, sItemInfo>::iterator it;
	for (it = Doc()->m_clubSetMap.begin(); it != Doc()->m_clubSetMap.end();
		++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 38.0f, 38.0f),
					0xffffffff, 0);

			bool bUsable = Doc()->CheckUnderLevelClubSet((*it).second.tid);
			if (!bUsable)
			{
				WRect rc(startX, y, 38.0f, 38.0f);
				g_view->DrawLine2D(WPoint(rc.x, rc.y + rc.h * 0.5f - 1.0f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f - 1.0f),
					0xffff0000, 0xffff0000, 0);
				g_view->DrawLine2D(WPoint(rc.x, rc.y + rc.h * 0.5f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f), 0xffff0000,
					0xffff0000, 0);
				g_view->DrawLine2D(WPoint(rc.x, rc.y + rc.h * 0.5f + 1.0f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f + 1.0f),
					0xffff0000, 0xffff0000, 0);
			}

			if (WRect(startX, y, 38.0f, 38.0f)
					.IsInRect(WPoint(g_input->GetMousePoint().x,
						g_input->GetMousePoint().y)))
			{
				pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 44.0f, 44.0f),
						0xffffffff, 0);

				if (bUsable == true)
				{
					m_subSelTypeId = (*it).second.tid;
					m_subSelItemId = (*it).second.guid;
				}
			}

			startX += 41.0f;
		}
	}
}

void CLobbyMain::OnGameRoom_AztecSelInit(int param)
{
	m_pAztecSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);

	if (m_pAztecSel)
	{
		m_pAztecSel->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoom_AztecSelDown()
{
	InitControlsSelectedUserInfo();
}

void CLobbyMain::OnGameRoom_AztecSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	if (ItemManager()->FindCommonItem(Doc()->m_myInfo.userEquip.tidBall) ==
		NULL)
		return;

	int count = Doc()->m_ballList.size();
	if (count <= 1)
		return;

	float x = m_pAztecSel->GetRect().x;
	float y = m_pAztecSel->GetRect().y;
	float w = 0.0f;
	float total = (float)count * 41.0f + 8.0f;

	if (x + total > 800.0f)
		x = 800.0f - total;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBitmap)
	{
		w = (float)count * 38.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	m_subSelTypeId = 0;
	m_subSelItemId = 0;
	y += 4.0f;

	std::list<sItemInfo>::const_iterator it;
	for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);
		if (pItem)
		{
			WRect rc(startX, y, 38.0f, 38.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 38.0f, 38.0f),
					0xffffffff, 0);

			int num = (*it).Common[0];
			if (num > 0)
			{
				unsigned long color =
					m_pAztecSel->IsEnabled() ? 0xffffffff : 0x7fffffff;
				Print(startX + 35.0f, y + 22.0f, MakeStr("%d", num), 2.0f, 2,
					1.0f, color);
			}

			if (rc.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 44.0f, 44.0f),
						0xffffffff, 0);

				m_subSelTypeId = (*it).tid;
				m_subSelItemId = (*it).guid;
			}

			startX += 41.0f;
		}
	}
}

void CLobbyMain::OnGameRoom_MascotSelInit(int param)
{
	m_pMascotSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);

	if (m_pMascotSel)
	{
		m_pMascotSel->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoom_MascotSelDown()
{
	InitControlsSelectedUserInfo();
}

void CLobbyMain::OnGameRoom_MascotSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_mascotMap.size();
	if (count == 0)
		return;

	if (Doc()->m_myInfo.userEquip.guidMascot)
		count++;

	float x = m_pMascotSel->GetRect().x;
	float y = m_pMascotSel->GetRect().y;
	float w = 0.0f;
	float total = (float)count * 41.0f + 8.0f;

	if (x + total > 800.0f)
		x = 800.0f - total;

	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBitmap)
	{
		w = (float)count * 38.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBitmap)
	{
		w = 4.0f;
		pGDI->DrawTexture(pBitmap, WRect(x, y, w, 54.0f), 0xffffffff, 0);
	}

	m_subSelTypeId = 0;
	m_subSelItemId = 0;
	y += 4.0f;

	std::map<unsigned int, sMascotInfo>::iterator it;
	for (it = Doc()->m_mascotMap.begin(); it != Doc()->m_mascotMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 38.0f, 38.0f),
					0xffffffff, 0);

			if (WRect(startX, y, 38.0f, 38.0f)
					.IsInRect(WPoint(g_input->GetMousePoint().x,
						g_input->GetMousePoint().y)))
			{
				pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 44.0f, 44.0f),
						0xffffffff, 0);

				m_subSelTypeId = (*it).second.tid;
				m_subSelItemId = (*it).second.guid;
			}

			startX += 41.0f;
		}
	}

	if (Doc()->m_myInfo.userEquip.guidMascot)
	{
		WRect dest(startX, y, 38.0f, 38.0f);

		pBitmap = g_pFresh->GetBitmap("w_caddie_no");
		if (pBitmap)
			pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

		if (dest.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			pBitmap = g_pFresh->GetBitmap("select_frame");
			if (pBitmap)
				pGDI->DrawTexture(pBitmap,
					WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
						dest.h + 6.0f),
					0xffffffff, 0);

			m_subSelTypeId = 0x40000000;
			m_subSelItemId = 0;
		}
	}
}

void CLobbyMain::OnGameRoom_StartTipInit(int param)
{
	m_pStartTip = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pStartTip)
		m_pStartTip->SetVisible(false);
}

void CLobbyMain::OnGameRoom_ReadyTipInit(int param)
{
	m_pReadyTip = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pReadyTip)
		m_pReadyTip->SetVisible(false);
}

void CLobbyMain::OnGameRoom_StartInit(int param)
{
	m_pStart = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_StartBtnUp()
{
	if (m_pStart == NULL || !m_pStart->IsEnabled())
		return;

	if (Doc()->IsControlServerService(2))
	{
		HandleMsg(MsgObject(this, 0x272, 2, 0, 0, 0, 0));
		return;
	}

	if (MyGuid(false) == m_masterOID)
	{
		if (!LatestVersion())
			return;

		if ((Doc()->m_curChannel.Type & 0x800) &&
			!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
		{
			IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
				((Doc()->m_roomInfo.mapType > 0x7f &&
					 Doc()->m_roomInfo.mapType != 0xfd)
						? Doc()->m_roomInfo.mapType - 0x80
						: Doc()->m_roomInfo.mapType) |
				0x28000000);
			if (pCourse == NULL)
				return;

			if (pCourse->Difficulty >= 3 ||
				((Doc()->m_curChannel.Type & 0x800) &&
					((((Doc()->m_roomInfo.mapType > 0x7f &&
						   Doc()->m_roomInfo.mapType != 0xfd)
							  ? Doc()->m_roomInfo.mapType - 0x80
							  : Doc()->m_roomInfo.mapType) |
						 0x28000000) == 1 ||
						(((Doc()->m_roomInfo.mapType > 0x7f &&
							  Doc()->m_roomInfo.mapType != 0xfd)
								 ? Doc()->m_roomInfo.mapType - 0x80
								 : Doc()->m_roomInfo.mapType) |
							0x28000000) == 11 ||
						(((Doc()->m_roomInfo.mapType > 0x7f &&
							  Doc()->m_roomInfo.mapType != 0xfd)
								 ? Doc()->m_roomInfo.mapType - 0x80
								 : Doc()->m_roomInfo.mapType) |
							0x28000000) == 14)))
			{
				this << MsgObject(this, 35,
					(int)"\xb7\xe7\xc5\xb0\xc3\xa4\xb3\xce\xbf\xa1\xbc\xad\xb4\xc2 \xc0\xcc \xb8\xca\xc0\xbb \xc7\xc3\xb7\xb9\xc0\xcc \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return;
			}
		}

		if (m_pStart)
			m_pStart->Enable(false);

		WSendPacket packet((enumClientPacket)14);
		packet.Encode4(MyGuid(false));
		packet.Send(TO_GAME);

		if (IsLocalContent(S4_INVITE_FRIEND))
			CMessengerInfo::Instance()->EnableInvite(false);
	}
	else
	{
		if (m_bLockControls || m_bRoomStateReq)
			return;

		m_bRoomStateReq = true;

		if (m_bReady)
		{
			WSendPacket packet((enumClientPacket)13);
			packet.Encode1(1);
			packet.Send(TO_GAME);
		}
		else
		{
			m_bNewBlink = false;
			if (m_pCurTip)
				m_pCurTip->SetVisible(false);

			WSendPacket packet((enumClientPacket)13);
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}

		EnableSettingButton(m_bReady);
	}
}

void CLobbyMain::OnGameRoom_ReadyInit(int param)
{
	m_pReady = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pReady)
	{
		m_pReady->SetVisible(false);
		m_pReady->Enable(false);
	}
}

void CLobbyMain::OnGameRoom_ReadyBtnUp()
{
	if (Doc()->IsControlServerService(2))
	{
		this << MsgObject(this, 0x272, 2, 0, 0, 0, 0);
		return;
	}

	if (MyGuid(false) == m_masterOID)
		return;

	if (m_bLockControls || m_bRoomStateReq)
		return;

	m_bRoomStateReq = true;

	if (m_bReady)
	{
		WSendPacket send((enumClientPacket)13);
		send.Encode1(1);
		send.Send(TO_GAME);
	}
	else
	{
		m_bNewBlink = false;
		if (m_pCurTip)
			m_pCurTip->SetVisible(false);

		WSendPacket send((enumClientPacket)13);
		send.Encode1(0);
		send.Send(TO_GAME);
	}

	EnableSettingButton(m_bReady);
}

void CLobbyMain::EnableSettingButton(bool enable)
{
	if (m_pOverBarChar)
		m_pOverBarChar->Enable(enable);

	if (m_pOverBarCaddie)
		m_pOverBarCaddie->Enable(enable);

	if (m_pOverBarClub)
		m_pOverBarClub->Enable(enable);

	if (m_pOverBarAztec)
		m_pOverBarAztec->Enable(enable);

	if (m_pOverBarMascot)
		m_pOverBarMascot->Enable(enable);

	CloseAllSubSelection();

	if (m_pChar)
		m_pChar->Enable(enable);

	if (m_pCaddie)
		m_pCaddie->Enable(enable);

	if (m_pClub)
		m_pClub->Enable(enable);

	if (m_pAztec)
		m_pAztec->Enable(enable);

	if (m_pMascot)
		m_pMascot->Enable(enable);

	if (m_pEquipItem)
		m_pEquipItem->Enable(enable);

	InitControlsSelectedUserInfo();

	if (!enable && m_pEquipDlg)
		m_pEquipDlg->Close(FrOK, true);
}

void CLobbyMain::OnGameRoom_RoomOptionInit(int param)
{
	m_pRoomOption = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_RoomOptionBtnUp()
{
	if (m_pCurTip && m_pCurTip->IsVisible() &&
		!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
	{
		HandleMsg(MsgObject(this, 35,
			(int)"\xb8\xf0\xb5\xce \xc1\xd8\xba\xf1\xb0\xa1 \xb5\xc8 \xbb\xf3\xc8\xb2\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc1\xa4\xba\xb8\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
			0, 0, 0, 0));
		return;
	}

	FrChangeRoomInfoDlg* pDlg = CreateForm<FrChangeRoomInfoDlg>(
		g_pFresh->GetManager(), this, "changeroominfo", NULL);
	pDlg->SetDesc(
		"\xb9\xe6\xc0\xc7 \xbf\xc9\xbc\xc7\xc0\xbb \xba\xaf\xb0\xe6\xc7\xcf\xbc\xbc\xbf\xe4.");
	pDlg->Open(NULL, 1);
	pDlg->MoveCursor("ok");
}

void CLobbyMain::OnGameRoom_MapInit(int param)
{
	m_pMap = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMap)
	{
		m_pMap->SetStyle(FrButton::BT_NORMAL);

		if ((Doc()->m_curChannel.Type & 0x80) ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
		{
			m_pMap->SetStyle(FrButton::BT_ALWAYSON);
			m_pMap->SetPushDelay(0.0f);
		}
	}
}

void CLobbyMain::OnGameRoom_MapBtnUp()
{
	if (m_masterOID != MyGuid(false) ||
		(m_bCanStart && Doc()->m_slotList.size() > 1 &&
			!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1)))
		return;

	if (Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH ||
		Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
		return;

	if (m_pTreasureCourse)
		return;

	m_pTreasureCourse = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(),
		this, "treasuremap", NULL);
	if (m_pTreasureCourse)
	{
		m_pTreasureCourse->SetSelectMap(Doc()->m_roomInfo.mapType);
		m_pTreasureCourse->SetMapIndexChanged(false);
		m_pTreasureCourse->LockRandom(
			Doc()->m_golfGame.gameType == GAME_TYPE_AVATARCHAT);
		m_pTreasureCourse->EnableDrag(false);
		m_pTreasureCourse->Open(
			(FRESH_PFN_RESULT)&CLobbyMain::OnMapSelectDlgResult, 3);
	}
}

void CLobbyMain::OnGameRoom_MapPrevInit(int param)
{
	m_pMapPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapPrev)
	{
		m_pMapPrev->Enable(false);
		m_pMapPrev->SetVisible(false);
		m_pMapPrev->SetPushDelay(0.0f);
	}
}

void CLobbyMain::OnGameRoom_MapNextInit(int param)
{
	m_pMapNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapNext)
	{
		m_pMapNext->Enable(false);
		m_pMapNext->SetVisible(false);
		m_pMapNext->SetPushDelay(0.0f);
	}
}

void CLobbyMain::OnGameRoom_MapPrevBtnUp()
{
	if (m_bLockControls)
		return;

	unsigned char map = PrevMap(Doc()->m_roomInfo.mapType);

	WSendPacket send((enumClientPacket)10);
	send.Encode2(0xffff);
	send.Encode1(1);
	send.Encode1(3);
	send.Encode1(map);
	send.Send(TO_GAME);

	HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CLobbyMain::OnGameRoom_MapNextBtnUp()
{
	if (m_bLockControls)
		return;

	unsigned char map = NextMap(Doc()->m_roomInfo.mapType, false);

	WSendPacket send((enumClientPacket)10);
	send.Encode2(0xffff);
	send.Encode1(1);
	send.Encode1(3);
	send.Encode1(map);
	send.Send(TO_GAME);

	HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CLobbyMain::OnGameRoom_MapHoleComboInit(int param)
{
	m_pMapHoleCombo = DYNAMIC_CAST(FrComboCtlEx, (FrWnd*)param);
	ReLoadHoleItem();

	unsigned char holeType = Doc()->m_roomInfo.holeType;
	if (m_pMapHoleCombo)
	{
		if (Doc()->m_roomInfo.nHole == 18)
		{
			if (holeType > 0)
				holeType = 1;
		}
		m_pMapHoleCombo->SelectItem(holeType);
	}
}

void CLobbyMain::OnGameRoom_MapHoleComboDown()
{
	if (!m_pMapHoleCombo)
		return;

	if (m_bLockControls)
		return;

	unsigned char holeType = m_pMapHoleCombo->GetCurrentIdx();
	if (Doc()->m_roomInfo.nHole != 18)
	{
		if (Doc()->m_roomInfo.holeType == holeType)
			return;
	}
	else
	{
		if (holeType == 0)
		{
			if (Doc()->m_roomInfo.holeType == 0)
				return;
		}
		else
		{
			if (Doc()->m_roomInfo.holeType == 3)
				return;
			holeType = 3;
		}
	}

	WSendPacket send((enumClientPacket)10);
	send.Encode2(0xffff);
	send.Encode1(1);
	send.Encode1(5);
	send.Encode1(holeType);
	send.Send(TO_GAME);

	HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CLobbyMain::OnGameRoom_MapLockInit(int param)
{
	m_pMapLock = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pMapLock)
		m_pMapLock->SetVisible(false);
}

void CLobbyMain::OnGameRoom_UserListInit(int param)
{
}

void CLobbyMain::OnGameRoom_UserListBtnUp()
{
	if (m_pUserListDlg)
		return;

	m_pUserListDlg = CreateForm<FrUserListDlg>(g_pFresh->GetManager(), this,
		"userlist", NULL);
	if (!m_pUserListDlg)
		return;

	m_pUserListDlg->MakeUserList(Doc()->m_briefUserInfoMap);
	m_pUserListDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnUserListDlgResult, 3);

	WPoint pos;
	pos.x = g_view->GetWidth() - m_pUserListDlg->GetRect().w - 5.0f;
	pos.y = m_pUserListDlg->GetRect().y;
	m_pUserListDlg->MoveWindow(pos);

	if (m_pUserListDlg)
		m_pUserListDlg->EnableRightButton(!m_bReady);
}

void CLobbyMain::OnGameRoom_ChangeTeamBtnInit(int param)
{
	m_pChangeTeam = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pChangeTeam)
		m_pChangeTeam->SetPushDelay(0.5f);
}

void CLobbyMain::OnGameRoom_ChangeTeamBtnUp()
{
	if (m_bReady || m_bLockControls)
		return;

	CSharedDoc* pDoc = Doc();

	std::list<sSlotInfo>::iterator it = std::find(pDoc->m_slotList.begin(),
		pDoc->m_slotList.end(), MyGuid(false));

	if (it == pDoc->m_slotList.end())
		return;

	(*it).bTeam = 1 - (*it).bTeam;

	WSendPacket send((enumClientPacket)16);
	send.Encode1((*it).bTeam);
	send.Send(TO_GAME);
}

void CLobbyMain::OnGameRoom_PowerBarInit(int param)
{
	m_pStatBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[0]->SetRange(0, 50, 0);
}

void CLobbyMain::OnGameRoom_ControlBarInit(int param)
{
	m_pStatBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[1]->SetRange(0, 30, 0);
}

void CLobbyMain::OnGameRoom_AccuracyBarInit(int param)
{
	m_pStatBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[2]->SetRange(0, 30, 0);
}

void CLobbyMain::OnGameRoom_SpinBarInit(int param)
{
	m_pStatBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[3]->SetRange(0, 30, 0);
}

void CLobbyMain::OnGameRoom_CurveBarInit(int param)
{
	m_pStatBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[4]->SetRange(0, 30, 0);
}

void CLobbyMain::OnGameRoom_PowerEditInit(int param)
{
	m_pStatEdit[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_ControlEditInit(int param)
{
	m_pStatEdit[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_AccuracyEditInit(int param)
{
	m_pStatEdit[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_SpinEditInit(int param)
{
	m_pStatEdit[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_CurveEditInit(int param)
{
	m_pStatEdit[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CLobbyMain::OnGameRoom_ScoreInit(int param)
{
	m_pScore = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pScore)
		m_pScore->Enable(false);
}

void CLobbyMain::OnGameRoom_ScoreBtnUp()
{
	if (m_pScoreDlg)
		return;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		m_pScoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this,
			"score_guild", NULL);
		m_pScoreDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnScoreDlgResult, 1);
		m_pScoreDlg->SetGuildPlayer(m_selOID);
		m_pScoreDlg->SetVisibleCloseBtn(false);
	}
	else
	{
		m_pScoreDlg =
			CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score", NULL);
		m_pScoreDlg->Open((FRESH_PFN_RESULT)&CLobbyMain::OnScoreDlgResult, 1);

		if (m_selOID == (unsigned long)-1)
		{
			m_selOID = MyGuid(false);
			m_selUID = MyUID();
		}

		m_pScoreDlg->SetPlayer(m_selOID);
		m_pScoreDlg->SetVisibleCloseBtn(true);
	}
}

void CLobbyMain::OnGameRoomExt_Init(int param)
{
	OnGameRoom_Init(param);
}

void CLobbyMain::OnGameRoomExt_Finish()
{
	m_pOverBarTitle->SetBgImg("title_mass");

	if (m_bInitSlot)
	{
		sRoomSlot slot;
		slot.pArea = m_pPet[0];
		slot.pExhibition = NULL;
		slot.partTidList = Doc()->GetMyPartTidList();
		slot.charInfo = Doc()->m_charMap[Doc()->m_myInfo.userEquip.guidChar];
		slot.bAngelWing = Doc()->m_myInfo.info.angelicWings;
		slot.bGachaWing = Doc()->m_myInfo.info.angelicWingsEffect;
		Doc()->m_roomSlotMap[MyGuid(false)] = slot;

		ShowPet(MyGuid(false));
	}

	HandleMsg(MsgObject(this, 17, Doc()->m_roomInfo.mapType, 0, 0, 0, 0));

	if (Doc()->m_bRefreshCamera)
		HandleMsg(MsgObject(NULL, 170, 1, 0, 0, 0, 0));

	EnableUnderBar(false);
}

void CLobbyMain::OnGameRoomExt_Destroy()
{
	EnableUnderBar(true);
}

void CLobbyMain::OnGameRoomExt_RoomUserInit(int param)
{
	m_pRoomUser = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pRoomUser)
	{
		m_pRoomUser->UseRightButton(true);
		m_pRoomUser->UseDummy(true);

		m_pInviteBmp = (Bitmap*)m_pRoomUser->GetBitmap("30_player_wait");

		m_pGenderBmp[0] = (Bitmap*)m_pRoomUser->GetBitmap("i_male");
		m_pGenderBmp[1] = (Bitmap*)m_pRoomUser->GetBitmap("i_female");
		m_pGenderBmp[2] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_02");
		m_pGenderBmp[3] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_02");
		m_pGenderBmp[4] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_03");
		m_pGenderBmp[5] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_03");
		m_pGenderBmp[6] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_manner");
		m_pGenderBmp[7] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_manner");
		m_pGenderBmp[8] = (Bitmap*)m_pRoomUser->GetBitmap("i_male_angel");
		m_pGenderBmp[9] = (Bitmap*)m_pRoomUser->GetBitmap("i_female_angel");

		m_pSituBmp[0] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ03");
		m_pSituBmp[1] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ01");
		m_pSituBmp[2] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ02");
		m_pSituBmp[3] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ_red_cp");
		m_pSituBmp[4] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ_red");
		m_pSituBmp[5] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ_blue_cp");
		m_pSituBmp[6] = (Bitmap*)m_pRoomUser->GetBitmap("30in_situ_blue");
		m_pDiveBmp = (Bitmap*)m_pRoomUser->GetBitmap("dive");

		m_pPlayerBaseBmp[0] =
			(Bitmap*)g_pFresh->RegisterBitmap("30_noplayer_base");
		m_pPlayerBaseBmp[1] =
			(Bitmap*)g_pFresh->RegisterBitmap("30_player_base");

		m_pKickBmp = (Bitmap*)g_pFresh->RegisterBitmap("kick");

		m_selUID = m_selOID = (unsigned long)-1;

		if (m_bInitSlot)
			MakeRoomUserList();
	}
}

void CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	sSlotInfo* pSlot = NULL;
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;

	pSlot = (sSlotInfo*)pItem->pData;

	if (IsLocalContent(S4_INVITE_FRIEND))
	{
		if (pSlot)
		{
			if (pSlot->IsInvite)
			{
				pDevice->DrawTexture(m_pInviteBmp,
					WRect(pItem->pos.x, pItem->pos.y,
						(float)m_pInviteBmp->Width(),
						(float)m_pInviteBmp->Height()),
					0xffffffff, 0);
				return;
			}
		}
	}

	pDevice->DrawTexture(m_pPlayerBaseBmp[0],
		WRect(pItem->pos.x, pItem->pos.y, (float)m_pPlayerBaseBmp[1]->Width(),
			(float)m_pPlayerBaseBmp[1]->Height()),
		0xffffffff, 0);

	if (pItem->selected)
		pDevice->Box(WRect(pItem->pos.x, pItem->pos.y,
						 (float)m_pRoomUser->GetItemWidth() + 15.0f,
						 (float)m_pRoomUser->GetItemHeight()),
			0x7f265785, 0, 0.0f);

	if (!pSlot)
		return;

	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);

	int index;
	if (!pSlot->bReady && !pSlot->bMaster)
	{
		index = 0;
	}
	else
	{
		int n = (pSlot->bMaster) ? 1 : 2;
		index = n;

		if (Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM ||
			Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
		{
			if (!pSlot->bTeam)
				index += 2;
			else
				index += 4;
		}
	}

	const Bitmap* pIcon = m_pSituBmp[index];
	rect = WRect(pItem->pos.x + 10.0f, pItem->pos.y + 5.0f,
		(float)pIcon->Width(), (float)pIcon->Height());
	pDevice->DrawTexture(pIcon, rect, 0xffffffff, 0);

	if (pSlot->angelicWings)
		pIcon = m_pGenderBmp[8 + pSlot->gender % 2];
	else if (pSlot->manner)
		pIcon = m_pGenderBmp[6 + pSlot->gender % 2];
	else
		pIcon = m_pGenderBmp[pSlot->gender];

	rect = WRect(pItem->pos.x + 30.0f, pItem->pos.y, (float)pIcon->Width(),
		(float)pIcon->Height());
	pDevice->DrawTexture(pIcon, rect, 0xffffffff, 0);

	if (m_pDiveBmp && pSlot->bSleep)
		pDevice->DrawTexture(m_pDiveBmp,
			WRect(pItem->pos.x + 34.0f, pItem->pos.y + 2.0f,
				(float)m_pDiveBmp->Width(), (float)m_pDiveBmp->Height()),
			0xffffffff, 0);

	const Bitmap* pLevel;
	if (Doc()->m_curChannel.Type & 0x80)
	{
		pLevel =
			g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->ladderGrade));
	}
	else if (pSlot->dwTitle)
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->dwTitle);
		if (pSkin)
			pLevel = g_pFresh->GetBitmap(pSkin->c.Icon);
		else
			pLevel =
				g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->level + 1));
	}
	else
	{
		pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->level + 1));
	}
	if (pLevel)
	{
		pDevice->DrawTexture(pLevel,
			WRect(pItem->pos.x + 80.0f - float2int(pLevel->Width() * 0.5f),
				pItem->pos.y + 13.0f - float2int(pLevel->Height() * 0.5f),
				(float)pLevel->Width(), (float)pLevel->Height()),
			0xffffffff, 0);
	}

	if (pSlot->GuildId && !pSlot->IsIdentity(0x14))
	{
		const Bitmap* pEmblem = NetResourceManager::Instance()->GetEmblemByName(
			pSlot->szEmblemName);

		if (pEmblem)
			pDevice->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 115.0f, pItem->pos.y + 7.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
	}

	unsigned long color =
		(unsigned char)(pSlot->dwIdentity & 0x14) ? 0x50ffffff : 0xffffffff;

	if (pItem->underCursor)
	{
		pDevice->SetTextColor(0xffffffff, 0xff124371);
		pDevice->SetTextStyle(2);
	}
	else
	{
		pDevice->SetTextColor(0xffffffff, 0xffffffff);
		pDevice->SetTextStyle(0);
	}

	if (!CProjectG::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 140.0f, pItem->pos.y + 7.0f), 0, pSlot->sNick,
			-1.0f, color);

	if ((!Doc()->m_bGameOver ||
			(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1)) &&
		MyGuid(false) == m_masterOID && !pSlot->bMaster &&
		Doc()->m_roomInfo.gameType != GAME_TYPE_GUILD_MATCH)
		pDevice->DrawTexture(m_pKickBmp,
			WRect(pItem->pos.x + 226.0f, pItem->pos.y + 7.0f - 2.0f,
				(float)m_pKickBmp->Width(), (float)m_pKickBmp->Height()),
			pItem->underCursor ? 0xffffffff : 0x80ffffff, 0);
}

void CLobbyMain::OnGameRoomExt_CountdownInit(int param)
{
	m_pCountdown = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (m_pCountdown)
	{
		m_pCountdown->SetVisible(false);
	}
}

void CLobbyMain::OnGameRoomExt_ResCloseBtnUp()
{
	if (m_pRoomClose)
		m_pRoomClose->Enable(false);

	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		WSendPacket packet((enumClientPacket)0x0f);
		packet.Encode1(0);
		packet.Encode2(0xffff);
		packet.Encode8(0);
		packet.Encode8(0);
		packet.Send(TO_GAME);

		HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "tolobby", NULL);
		pForm->SetMessage(
			"\xb0\xd4\xc0\xd3\xc0\xcc \\c0xffff0000\\c\xbf\xcf\xc0\xfc\xc8\xf7 \xc1\xbe\xb7\xe1\\c0xff000000\\c\xb5\xc7\xb1\xe2 \xc0\xfc\xbf\xa1 \xb0\xd4\xc0\xd3\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb8\xe9, \\c0xffff0000\\c\xb0\xe6\xc7\xe8\xc4\xa1 \xc8\xb9\xb5\xe6\xb0\xfa \xb0\xa2\xc1\xbe \xb1\xe2\xb7\xcf\xc0\xcc \xb9\xab\xc8\xbf\xc3\xb3\xb8\xae\\c0xff000000\\c\xb5\xcb\xb4\xcf\xb4\xd9.\n\n \x09\x09\x09\x09\x09\x09\x09\x09\x09\xb6\xc7\xc7\xd1 \xb8\xb9\xc0\xba \xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xb7\xce \xc0\xce\xc7\xd8 \xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xb4\xd9\xc0\xbd\xb0\xfa \xb0\xb0\xc0\xba \xbc\xf6\xc4\xa1\xb0\xa1 \xb5\xc7\xb8\xe9 \\c0xffff0000\\c\xbc\xba\xba\xb0 \xc7\xa5\xbd\xc3 \xbe\xc6\xc0\xcc\xc4\xdc\\c0xff000000\\c\xc0\xcc \xc5\xb9\xc7\xd8\xc1\xfc\xb0\xfa \xb5\xbf\xbd\xc3\xbf\xa1 \x09\x09\x09\x09\x09\x09\x09\x09\x09\\c0xffff0000\\c\xbb\xe7\xbf\xeb \xb1\xe2\xb4\xc9\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa6\xbe\xe0\\c0xff000000\\c\xc0\xcc \xbb\xfd\xb1\xe9\xb4\xcf\xb4\xd9.\n\n\n\n\n\n\n\n\xb0\xd4\xc0\xd3\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed \xb7\xce\xba\xf1\xb7\xce \xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		pForm->Open((FRESH_PFN_RESULT)&CLobbyMain::OnRoomExtResQuitDlgResult,
			1);
	}
}

void CLobbyMain::OnGameRoomExt_FrameInit(int param)
{
	m_pExtFrame = DYNAMIC_CAST(FrFrame, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_GuildTab1Init(int param)
{
	m_pGuildTab[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_GuildTab2Init(int param)
{
	m_pGuildTab[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_TeamTab1Init(int param)
{
	m_pTeamTab[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_TeamTab2Init(int param)
{
	m_pTeamTab[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_TeamNumInit(int param)
{
	m_pTeamNum = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_pTeamNum->GetRect();

	pGDI->SetTextStyle(0);

	if (Doc()->m_roomInfo.gameType == GAME_TYPE_TEAM ||
		Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM)
	{
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(rect.x + 209.0f, rect.y + 12.0f), 0,
			MakeStr("(%d)", m_redTeam.size()), -1.0f, 0xffffffff);

		pGDI->SetTextColor(0xff0060ff, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(rect.x + 458.0f, rect.y + 12.0f), 0,
			MakeStr("(%d)", m_blueTeam.size()), -1.0f, 0xffffffff);
	}
	else
	{
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(rect.x + 109.0f, rect.y + 12.0f), 0, "\xb4\xeb\xc8\xb8",
			-1.0f, 0xffffffff);

		float offset = 209.0f;
		if (Doc()->m_slotList.size() >= 100)
			offset = 200.0f;

		g_pFresh->GetManager()->PrintText(
			WPoint(offset + rect.x, rect.y + 12.0f), 0,
			MakeStr("(%d)", Doc()->m_slotList.size()), -1.0f, 0xffffffff);
	}
}

void CLobbyMain::OnGameRoomExtRes_GuildScoreGaugeInit(int param)
{
	m_pGuildScoreGauge = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);

	m_pGuildScoreGauge->SetVisible(false);
	return;

	m_pGuildScoreGauge->SetRange(0, Doc()->m_guildPoint, 0);
}

void CLobbyMain::OnGameRoomExt_GuildNumInit(int param)
{
	m_pGuildNum = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (Doc()->m_roomInfo.gameType != GAME_TYPE_GUILD_MATCH)
		m_pGuildNum->SetVisible(false);
}

void CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_pTeamNum->GetRect();
	pGDI->SetTextStyle(1);

	if (Doc()->m_roomInfo.GuildInfo.nGuildID[0])
	{
		const Bitmap* pEmblem = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem,
				WRect(rect.x + 24.0f, rect.y + 8.0f, (float)pEmblem->Width(),
					(float)pEmblem->Height()),
				0xffffffff, 0);
	}

	pGDI->SetTextColor(0xffff0000, 0xffffffff);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 60.0f, rect.y + 12.0f), 0,
		Doc()->m_roomInfo.GuildInfo.szName[0], -1.0f, 0xffffffff);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 209.0f, rect.y + 12.0f),
		0, MakeStr("(%d)", m_redTeam.size()), -1.0f, 0xffffffff);

	if (!strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
		g_pFresh->GetManager()->PrintText(
			WPoint(rect.x + 180.0f, rect.y + 12.0f), 0,
			MakeStr("%d", Doc()->m_guildScore[0]), -1.0f, 0xffffffff);

	if (Doc()->m_roomInfo.GuildInfo.nGuildID[1])
	{
		const Bitmap* pEmblem = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem,
				WRect(rect.x + 273.0f, rect.y + 8.0f, (float)pEmblem->Width(),
					(float)pEmblem->Height()),
				0xffffffff, 0);
	}

	pGDI->SetTextColor(0xff0060ff, 0xffffffff);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 309.0f, rect.y + 12.0f),
		0, Doc()->m_roomInfo.GuildInfo.szName[1], -1.0f, 0xffffffff);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 458.0f, rect.y + 12.0f),
		0, MakeStr("(%d)", m_blueTeam.size()), -1.0f, 0xffffffff);

	if (!strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
		g_pFresh->GetManager()->PrintText(
			WPoint(rect.x + 429.0f, rect.y + 12.0f), 0,
			MakeStr("%d", Doc()->m_guildScore[1]), -1.0f, 0xffffffff);
}

void CLobbyMain::OnGameRoomExt_GuildInviteInit(int param)
{
	m_pGuildInvite = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void CLobbyMain::OnGameRoomExt_GuildInviteLBtnUp()
{
	if (Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
	{
		std::map<unsigned long, unsigned int> guildCount;
		bool bFound = false;
		char szMsg[256];
		szMsg[0] = '\0';

		std::map<unsigned long, sBriefUserInfo>::iterator it;
		for (it = Doc()->m_briefUserInfoMap.begin();
			it != Doc()->m_briefUserInfoMap.end(); ++it)
		{
			if (it->second.m_GuildId &&
				Doc()->m_myInfo.info.dwGuildId != it->second.m_GuildId &&
				!it->second.bSleep && it->second.roomIndex == 0xffff)
			{
				guildCount[it->second.m_GuildId]++;
			}
		}

		std::vector<unsigned long> guildList;

		for (std::map<unsigned long, unsigned int>::iterator itCount =
				 guildCount.begin();
			itCount != guildCount.end(); ++itCount)
		{
			if (Doc()->m_roomInfo.nUserNum <= itCount->second)
			{
				guildList.push_back(itCount->first);
				bFound = true;
			}
		}

		m_guildInviteDelay = 2.0f;

		if (!bFound)
		{
			m_pNotifyDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			m_pNotifyDlg->SetMessage(
				MakeStr(
					"\xc3\xca\xb4\xeb\xc7\xd2 \xbb\xf3\xb4\xeb \xb1\xe6\xb5\xe5\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					it->second.sNick),
				false);
			m_pNotifyDlg->Open(NULL, 0);

			FrButton* pCancel =
				DYNAMIC_CAST(FrButton, m_pNotifyDlg->FindChildByName("cancel"));
			return;
		}

		{
			unsigned int count = guildList.size();

			for (it = Doc()->m_briefUserInfoMap.begin();
				it != Doc()->m_briefUserInfoMap.end(); ++it)
			{
				for (unsigned int i = 0; i < count; i++)
				{
					if (it->second.m_GuildId == guildList[i])
					{
						if (IsLocalContent(S4_INVITE_FRIEND))
						{
							WSendPacket packet(0xb2);
							packet.EncodeStr(it->second.sNick);
							packet.Encode4(it->second.dwUid);
							packet.Send(TO_GAME);
						}
						else
						{
							WSendPacket packet(0x29);
							packet.Encode4(it->second.dwUid);
							packet.Send(TO_GAME);
						}
					}
				}
			}

			m_pNotifyDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			m_pNotifyDlg->SetMessage(
				MakeStr(
					"\xc3\xca\xb4\xeb \xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xc2\xbd\xc0\xb4\xcf\xb4\xd9",
					it->second.sNick),
				false);
			m_pNotifyDlg->Open(NULL, 0);

			FrButton* pCancel =
				DYNAMIC_CAST(FrButton, m_pNotifyDlg->FindChildByName("cancel"));
			if (pCancel)
				pCancel->SetVisible(false);
		}
	}
	else
	{
		OnGameRoom_UserListBtnUp();
	}
}

void CLobbyMain::OnGameRoomExtRes_RoomUserRankTabInit(int param)
{
	m_pRoomUserRankTab = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pRoomUserRankTab)
	{
		if (Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
		{
			m_pRoomUserRankTab->SetBgImg("approach_tab");
		}
	}
}

void CLobbyMain::OnGameRoomExtRes_RoomUserRankInit(int param)
{
	m_pRoomUserRank = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pRoomUserRank)
		return;

	if (Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH)
	{
		m_pRoomUserRank->SetVisible(false);
		return;
	}

	m_pRoomUserRank->UseRightButton(true);

	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); it++)
		m_pRoomUserRank->AddItem(&(*it));

	if (m_pRoomUserRank->GetScrollBar())
		m_pRoomUserRank->GetScrollBar()->SetGuideVisible(true);

	if (Doc()->m_golfGame.gameType == GAME_TYPE_NEW_APPROACH)
		m_pRoomUserRank->SortItem(ApproachRankUserCompare);
	else
		m_pRoomUserRank->SortItem(RankUserCompare);

	m_pTeamRankBmp[0] = g_pFresh->RegisterBitmap("30in_team_redteam_rank");
	m_pTeamRankBmp[1] = g_pFresh->RegisterBitmap("30in_team_blueteam_rank");
	m_pRankBgBmp = g_pFresh->RegisterBitmap("30in_rank");

	m_pRankBmp[0] = g_pFresh->RegisterBitmap("rank_gold");
	m_pRankBmp[1] = g_pFresh->RegisterBitmap("rank_silver");
	m_pRankBmp[2] = g_pFresh->RegisterBitmap("rank_bronze");
	m_pRankBmp[3] = g_pFresh->RegisterBitmap("rank_noaward");

	m_rankViewKind = 0;
}

void CLobbyMain::OnGameRoomExtRes_RoomUserRankOwnerDraw(int param)
{
	if (param == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	WRect rect(pItem->pos.x, pItem->pos.y,
		(float)m_pRoomUserRank->GetItemWidth(),
		(float)m_pRoomUserRank->GetItemHeight());

	if (!pGDI)
		return;

	sRivalData* pRival = (sRivalData*)pItem->pData;

	if (m_selOID == pRival->oid)
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f),
			0x999b9dff, 0, 0.0f);

	int space = pGDI->SetSpace(0);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);

	if (Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM)
	{
		if (m_pTeamRankBmp[pRival->team])
			pGDI->DrawTexture(m_pTeamRankBmp[pRival->team],
				WRect(rect.x + 5.0f, rect.y + 7.0f - 1.0f,
					(float)m_pTeamRankBmp[pRival->team]->Width(),
					(float)m_pTeamRankBmp[pRival->team]->Height()),
				0xffffffff, 0);

		pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "%d",
			pRival->rank);
	}
	else
	{
		if (m_pRankBgBmp)
			pGDI->DrawTexture(m_pRankBgBmp,
				WRect(rect.x + 5.0f, rect.y + 7.0f - 1.0f,
					(float)m_pRankBgBmp->Width(),
					(float)m_pRankBgBmp->Height()),
				0xffffffff, 0);

		if (pRival->state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1,
				"-");
		else if (Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH &&
			pRival->approachResultDistance == -1)
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1,
				"-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1,
				"%d", pRival->rank);
	}

	if (Doc()->m_roomInfo.gameType != GAME_TYPE_NEW_APPROACH &&
		(!IsLocalContent(S3_HUNDRED_MODE) ||
			Doc()->m_roomInfo.nUserLimit < 100) &&
		pRival->state != 3 && pRival->hole != Doc()->m_holeOrder[0])
	{
		int medal = GetMedalIndex(pRival->oid);

		if (medal >= 0 && medal <= 2)
		{
			if (pRival->flag > 3)
				medal = 3;

			pGDI->DrawTexture(m_pRankBmp[medal],
				WRect(rect.x - 5.0f, rect.y + 7.0f - 2.0f,
					(float)m_pRankBmp[medal]->Width(),
					(float)m_pRankBmp[medal]->Height()),
				0xffffffff, 0);
		}
	}

	if (pRival->guildUID)
	{
		const Bitmap* pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(pRival->guildMark);

		if (pEmblem)
		{
			pGDI->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 24.0f, pItem->pos.y + 1.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
		}
	}

	if (pRival->oid == MyGuid(true))
	{
		if (Doc()->m_roomInfo.gameType == GAME_TYPE_30S_TEAM)
			pGDI->SetTextColor(pRival->team ? 0xff0060ff : 0xffff0000,
				0xffffffff);
		else
			pGDI->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 7.0f), 0,
			pRival->nickname, 80.0f, 0xffffffff);

	if (Doc()->m_roomInfo.gameType == GAME_TYPE_NEW_APPROACH)
	{
		if (pRival->approachTime == -1)
		{
			pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2,
				"Out");
			pGDI->Print(WPoint(pItem->pos.x + 188.0f, pItem->pos.y + 7.0f), 2,
				"-");
		}
		else
		{
			if (pRival->approachResultDistance == -1)
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f),
					2, "Out");
			else
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f),
					2, "%d.%dy", pRival->approachResultDistance / 10,
					pRival->approachResultDistance % 10);

			pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 2,
				"%d.%03d", pRival->approachTime / 1000,
				pRival->approachTime % 1000);
		}
	}
	else
	{
		pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 7.0f), 2, "%d",
			pRival->totalScore);
		pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2,
			"%I64d", pRival->totalPang);

		if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2,
				"End");
		else if (pRival->state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2,
				"Out");
		else
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2,
				"%d\xc8\xa6", GetHoleIndex(pRival->hole));
	}

	pGDI->SetSpace(space);
}

void CLobbyMain::OnGameRoomExtRes_RoomUserRankLBtnUp()
{
	FrListItem* item = m_pRoomUserRank->GetItemUnderCursor();

	if (!item)
		return;

	sRivalData* rival = (sRivalData*)item->pData;
	if (!rival)
		return;

	m_selOID = rival->oid;
	m_selUID = rival->uid;

	if (m_pScoreDlg)
	{
		m_pScoreDlg->SetPlayer(m_selOID);
		return;
	}

	if (m_pScore)
		m_pScore->Enable(true);

	if (m_pUserInfo)
		m_pUserInfo->Enable(true);
}

void CLobbyMain::OnGameRoomExtRes_RoomUserRankRBtnUp()
{
	FrListItem* item = m_pRoomUserRank->GetItemUnderCursor();

	if (!item)
		return;

	sRivalData* rival = (sRivalData*)item->pData;

	if (!rival)
		return;

	m_selOID = rival->oid;
	m_selUID = rival->uid;

	if (m_pScoreDlg)
		m_pScoreDlg->SetPlayer(m_selOID);
	else
	{
		if (m_pScore)
			m_pScore->Enable(true);
	}

	if (CUserInfo::Instance() && rival->state != 3)
	{
		std::map<unsigned long, sBriefUserInfo>::iterator it =
			Doc()->m_briefUserInfoMap.find(rival->oid);
		if (it != Doc()->m_briefUserInfoMap.end())
			it->second.roomIndex = Doc()->m_roomInfo.roomGuid;

		CUserInfo::Instance()->SetInfo(it->second.dwUid, rival->oid, true, true,
			false, false, std::string(it->second.sNick));
	}
}

void CLobbyMain::OnGameRoomExtRes_RoomGuildRankInit(int param)
{
	m_pRoomGuildRank = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pRoomGuildRank)
		return;

	if (Doc()->m_golfGame.gameType != GAME_TYPE_GUILD_MATCH)
	{
		m_pRoomGuildRank->SetVisible(false);
		return;
	}

	m_pRoomGuildRank->UseRightButton(true);

	for (std::vector<sGuildMatchup>::iterator it =
			 Doc()->m_guildMatchupList.begin();
		it != Doc()->m_guildMatchupList.end(); ++it)
		m_pRoomGuildRank->AddItem(it._Myptr);

	if (m_pRoomGuildRank->GetScrollBar())
		m_pRoomGuildRank->GetScrollBar()->SetGuideVisible(true);

	m_pTeamRankBmp[0] = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);
	m_pTeamRankBmp[1] = NetResourceManager::Instance()->GetEmblemByName(
		Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);

	m_pRankBgBmp = g_pFresh->RegisterBitmap("30in_rank_02");
}

void CLobbyMain::OnGameRoomExtRes_RoomGuildRankLBtnUp()
{
	FrListItem* item = m_pRoomGuildRank->GetItemUnderCursor();

	if (!item)
		return;

	sGuildMatchup* matchup = (sGuildMatchup*)item->pData;

	if (!matchup)
		return;

	if (m_pScoreDlg)
	{
		m_pScoreDlg->SetGuildPlayer(matchup->uid[0]);
		return;
	}

	if (m_pScore)
		m_pScore->Enable(true);
}

void CLobbyMain::OnGameRoomExtRes_RoomGuildRankRBtnUp()
{
	OnGameRoomExtRes_RoomGuildRankLBtnUp();
	OnGameRoom_ScoreBtnUp();
}

void CLobbyMain::OnGameRoomExtRes_RoomGuildRankOwnerDraw(int param)
{
	if (param == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	WRect rect(pItem->pos.x + 2.0f, pItem->pos.y,
		(float)m_pRoomGuildRank->GetItemWidth() - 10.0f,
		(float)m_pRoomGuildRank->GetItemHeight());

	if (!pGDI)
		return;

	int space = pGDI->SetSpace(0);

	sGuildMatchup* pMatchup = (sGuildMatchup*)pItem->pData;

	if (m_selOID == pMatchup->uid[0] || m_selOID == pMatchup->uid[1])
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f),
			0x999b9dff, 0, 0.0f);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);

	if (m_pRankBgBmp)
	{
		pGDI->DrawTexture(m_pRankBgBmp,
			WRect(rect.x - 3.0f, rect.y + 6.0f, (float)m_pRankBgBmp->Width(),
				(float)m_pRankBgBmp->Height()),
			0xffffffff, 0);
		pGDI->Print(WPoint(pItem->pos.x + 13.0f, pItem->pos.y + 10.0f), 1,
			"%d \xc1\xb6", pMatchup->team);
	}

	sRivalData* pRival;

	pRival = &Doc()->m_rivalList[Doc()->GetIndex(pMatchup->uid[0])];

	if (m_pTeamRankBmp[0])
		pGDI->DrawTexture(m_pTeamRankBmp[0],
			WRect(pItem->pos.x + 34.0f, pItem->pos.y + 3.0f,
				(float)(m_pTeamRankBmp[0]->Width() / 2),
				(float)(m_pTeamRankBmp[0]->Height() / 2)),
			0xffffffff, 0);

	if (pRival->oid == MyGuid(true))
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 3.0f), 0,
			pRival->nickname, 80.0f, 0xffffffff);

	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 3.0f), 2, "%d",
		pRival->guildPoint);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 3.0f), 2, "%I64d",
		pRival->totalPang);

	if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2,
			"End");
	else if (pRival->state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2,
			"Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival->hole));

	pRival = &Doc()->m_rivalList[Doc()->GetIndex(pMatchup->uid[1])];

	if (m_pTeamRankBmp[1])
		pGDI->DrawTexture(m_pTeamRankBmp[1],
			WRect(pItem->pos.x + 34.0f, pItem->pos.y + 21.0f,
				(float)(m_pTeamRankBmp[1]->Width() / 2),
				(float)(m_pTeamRankBmp[1]->Height() / 2)),
			0xffffffff, 0);

	if (pRival->oid == MyGuid(true))
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xff808080, 0xffffffff);
	else
		pGDI->SetTextColor(0xff000000, 0xffffffff);

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 20.0f), 0,
			pRival->nickname, 80.0f, 0xffffffff);

	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 20.0f), 2, "%d",
		pRival->guildPoint);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 20.0f), 2, "%I64d",
		pRival->totalPang);

	if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2,
			"End");
	else if (pRival->state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2,
			"Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival->hole));

	pGDI->SetSpace(space);
}

void CLobbyMain::OnGameRoomExt_UserInfoInit(int param)
{
	m_pUserInfo = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_pUserInfo)
	{
		m_pUserInfo->Enable(false);
	}
}

void CLobbyMain::OnGameRoomExt_UserInfoLBtnUp()
{
	if (m_rankViewKind == 0)
	{
		if (m_selOID == (unsigned long)-1)
		{
			m_pUserInfo->Enable(false);
			return;
		}

		if (CUserInfo::Instance())
		{
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				Doc()->m_briefUserInfoMap.find(m_selOID);
			if (it != Doc()->m_briefUserInfoMap.end())
			{
				CUserInfo::Instance()->SetInfo(it->second.dwUid, m_selOID, true,
					true, false, false, std::string(it->second.sNick));
			}
		}
	}
}

void CLobbyMain::OnGameRoomExtRes_Init(int param)
{
	CTaskMain::OnInit();

	m_bIdle = false;
	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();

	if (Doc()->m_roomInfo.realGameType == 14)
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
			"chaos_background.jpg", true);
	else
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper(
			"gameroom_background.jpg", true);

	m_bReady = false;
	m_bLockControls = false;
	m_bCanStart = false;

	if (Doc()->m_myInfo.info.dwIdentity == 2)
	{
		m_bReady = true;
		m_bLockControls = true;
	}

	m_bRoomStateReq = false;
	m_selOID = -1;
	m_selUID = -1;

	SetTimeVariableBGM();
}

void CLobbyMain::OnGameRoomExtRes_Finish()
{
	m_pOverBarTitle->SetBgImg("title_mass");

	EnableUnderBar(false);
	m_pUnderBarBongdariShop->Enable(true);

	OpenNewRecordForm();
}

void CLobbyMain::OnGameRoomExtRes_Destroy()
{
	EnableUnderBar(true);
}

void CLobbyMain::OnCreate_Init(int param)
{
	CTaskMain::OnInit();
}

void CLobbyMain::OnCreate_Finish()
{
}

void CLobbyMain::OnCreate_Destroy()
{
	if (m_pCreateNickDlg)
	{
		m_pCreateNickDlg->CloseDlg(1);
		m_pCreateNickDlg = NULL;
	}
}

void CLobbyMain::OnCreatePetInit(int param)
{
	m_pPet[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_createCharType = Doc()->m_myInfo.info.gender % 2;
	SetFirstChar(&Doc()->GetMyPartTidList());

	sRoomSlot slot;
	slot.pArea = m_pPet[0];
	slot.pExhibition = NULL;
	slot.partTidList = Doc()->GetMyPartTidList();

	slot.charInfo.tid = slot.partTidList.m_charTid;
	slot.charInfo.hairClr = slot.partTidList.m_hairColor;
	slot.charInfo.shirtsClr = slot.partTidList.m_shirtsColor;
	slot.charInfo.gift_flag = 1;
	slot.bAngelWing = 0;
	slot.bGachaWing = 0;
	memcpy(slot.charInfo.tidParts, slot.partTidList.m_tid,
		sizeof(slot.charInfo.tidParts));

	std::map<unsigned long, sRoomSlot>::iterator it =
		Doc()->m_roomSlotMap.find(MyGuid(false));
	if (it != Doc()->m_roomSlotMap.end() && it->second.pExhibition)
	{
		delete it->second.pExhibition;
		it->second.pExhibition = NULL;
	}
	Doc()->m_roomSlotMap[MyGuid(false)] = slot;

	ShowPet(MyGuid(false));
}

void CLobbyMain::OnCreatePetOwnerDraw()
{
	if (m_bBongdariShop2)
		return;

	std::map<unsigned long, sRoomSlot>::iterator it =
		Doc()->m_roomSlotMap.find(MyGuid(false));
	if (it != Doc()->m_roomSlotMap.end())
	{
		CExhibition* pExhibition =
			Doc()->m_roomSlotMap[MyGuid(false)].pExhibition;

		if (pExhibition)
			Doc()->m_roomSlotMap[MyGuid(false)].pExhibition->Display(0.0f,
				0.0f);
	}
}

void CLobbyMain::OnCreate_CharInit(int param)
{
	m_pCreateChar = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool CLobbyMain::SetFirstChar(CPartTidList* pTids)
{
	IFF_STRUCT::sChar* pChar =
		ItemManager()->FindChar(0x4000000 | m_createCharType);

	if (pChar == NULL)
		return false;

	pTids->m_charTid = 0x4000000 | m_createCharType;
	pTids->m_defPartNum = pChar->nParts;
	pTids->m_partNum = pChar->nParts + pChar->nAcsries;
	pTids->m_hairColor = 0;
	pTids->m_shirtsColor = 0;

	pTids->SetDefaultTids();

	if (m_pCreateChar)
		m_pCreateChar->SetLine(1, pChar->c.Name, 0, false, 0);

	if (m_pCreateDesc)
	{
		m_pCreateDesc->ClearLine();

		IFF_STRUCT::sDesc* pDesc =
			ItemManager()->FindDesc(0x4000000 | m_createCharType);
		if (pDesc)
			m_pCreateDesc->AddText(pDesc->Desc, false, true);
	}

	return true;
}

void CLobbyMain::OnCreate_CharPrevInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_CharNextInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_CharBtnUp()
{
	CPartTidList* pTids = &Doc()->GetMyPartTidList();

	m_createCharType = (m_createCharType + 1) % 2;

	if (!SetFirstChar(pTids))
	{
		m_createCharType = (m_createCharType + 1) % 2;
		SetFirstChar(pTids);
		return;
	}

	if (Doc()->m_roomSlotMap[MyGuid(false)].pExhibition)
		Doc()->m_roomSlotMap[MyGuid(false)].pExhibition->SetModel(*pTids, NULL,
			NULL, 3.0f, 0);

	m_createShirtColor = 0;
	m_createHairColor = 0;
	if (m_pCreateHair)
		m_pCreateHair->SetLine(1,
			"\xb8\xd3\xb8\xae\xbb\xf6"
			"1",
			0, false, 0);
	if (m_pCreateShirt)
		m_pCreateShirt->SetLine(1,
			"\xbc\xc5\xc3\xf7\xbb\xf6"
			"1",
			0, false, 0);

	SetLevelBar();
}

void CLobbyMain::OnCreate_HairInit(int param)
{
	m_createHairColor = 0;
	m_pCreateHair = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pCreateHair)
		m_pCreateHair->SetLine(1,
			"\xb8\xd3\xb8\xae\xbb\xf6"
			"1",
			0, false, 0);
}

void CLobbyMain::OnCreate_HairPrevInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_HairPrevBtnUp()
{
	CPartTidList* pTids = &Doc()->GetMyPartTidList();

	if (m_createHairColor == 0)
	{
		m_createHairColor = 2;
	}
	else
	{
		--m_createHairColor;
	}

	if (m_pCreateHair)
		m_pCreateHair->SetLine(1,
			MakeStr("\xb8\xd3\xb8\xae\xbb\xf6%d", m_createHairColor + 1), 0,
			false, 0);

	if (Doc()->m_roomSlotMap[MyGuid(false)].pExhibition)
		Doc()
			->m_roomSlotMap[MyGuid(false)]
			.pExhibition->GetPetFrame()
			->SetHairClr(pTids, m_createHairColor);
	else
		pTids->m_hairColor = m_createHairColor;
}

void CLobbyMain::OnCreate_HairNextInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_HairNextBtnUp()
{
	CPartTidList* pTids = &Doc()->GetMyPartTidList();

	if (m_createHairColor == 2)
	{
		m_createHairColor = 0;
	}
	else
	{
		++m_createHairColor;
	}

	if (m_pCreateHair)
		m_pCreateHair->SetLine(1,
			MakeStr("\xb8\xd3\xb8\xae\xbb\xf6%d", m_createHairColor + 1), 0,
			false, 0);

	if (Doc()->m_roomSlotMap[MyGuid(false)].pExhibition)
		Doc()
			->m_roomSlotMap[MyGuid(false)]
			.pExhibition->GetPetFrame()
			->SetHairClr(pTids, m_createHairColor);
	else
		pTids->m_hairColor = m_createHairColor;
}

void CLobbyMain::OnCreate_ShirtsInit(int param)
{
	m_pCreateShirt = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_createShirtColor = 0;
	if (m_pCreateShirt)
		m_pCreateShirt->SetLine(1,
			"\xbc\xc5\xc3\xf7\xbb\xf6"
			"1",
			0, false, 0);
}

void CLobbyMain::OnCreate_ShirtsPrevInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_ShirtsPrevBtnUp()
{
	CPartTidList* pTids = &Doc()->GetMyPartTidList();

	m_createShirtColor = (m_createShirtColor == 0) ? 2 : --m_createShirtColor;

	if (m_pCreateShirt)
		m_pCreateShirt->SetLine(1,
			MakeStr("\xbc\xc5\xc3\xf7\xbb\xf6%d", m_createShirtColor + 1), 0,
			false, 0);

	if (Doc()->m_roomSlotMap[MyGuid(false)].pExhibition)
		Doc()
			->m_roomSlotMap[MyGuid(false)]
			.pExhibition->GetPetFrame()
			->SetShirtsClr(pTids, m_createShirtColor);
	else
		pTids->m_shirtsColor = m_createShirtColor;
}

void CLobbyMain::OnCreate_ShirtsNextInit(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton)
		pButton->SetPushDelay(0.0f);
}

void CLobbyMain::OnCreate_ShirtsNextBtnUp()
{
	CPartTidList* pTids = &Doc()->GetMyPartTidList();

	m_createShirtColor = (m_createShirtColor == 2) ? 0 : ++m_createShirtColor;

	if (m_pCreateShirt)
		m_pCreateShirt->SetLine(1,
			MakeStr("\xbc\xc5\xc3\xf7\xbb\xf6%d", m_createShirtColor + 1), 0,
			false, 0);

	if (Doc()->m_roomSlotMap[MyGuid(false)].pExhibition)
		Doc()
			->m_roomSlotMap[MyGuid(false)]
			.pExhibition->GetPetFrame()
			->SetShirtsClr(pTids, m_createShirtColor);
	else
		pTids->m_shirtsColor = m_createShirtColor;
}

void CLobbyMain::OnCreate_CreateBtnUp()
{
#pragma pack(push, 1)
	struct
	{
		unsigned long charTid;
		unsigned char hairColor : 4;
		unsigned char shirtsColor : 4;
	} info;
#pragma pack(pop)

	info.charTid = 0x4000000 | m_createCharType;
	info.hairColor = m_createHairColor;
	info.shirtsColor = m_createShirtColor;

	WSendPacket packet(8);
	packet.EncodeBuffer(&info, 5);
	packet.Send(TO_LOGIN);

	Doc()->m_bAutoRefresh = true;

	HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

void CLobbyMain::OnCreate_DescInit(int param)
{
	m_pCreateDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void CLobbyMain::OnCreate_WarningInit(int param)
{
	FrEdit* pEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	pEdit->AddText(
		"\\c0xffff0000\\c* \xc7\xd1 \xb9\xf8 \xbc\xb1\xc5\xc3\xc7\xd1 \xc4\xb3\xb8\xaf\xc5\xcd\xb4\xc2 \xb3\xaa\xc1\xdf\xbf\xa1 \xb9\xd9\xb2\xdc \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\\c0xff000000\\c",
		false, true);
	pEdit->AddText(
		"  \xbd\xc5\xc1\xdf\xc8\xf7 \xbc\xb1\xc5\xc3\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
		false, true);
}

void CLobbyMain::On_PowerBarInit(int param)
{
	m_pStatBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[0]->SetRange(0, 50, 0);
}

void CLobbyMain::On_ControlBarInit(int param)
{
	m_pStatBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[1]->SetRange(0, 30, 0);
}

void CLobbyMain::On_AccuracyBarInit(int param)
{
	m_pStatBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[2]->SetRange(0, 30, 0);
}

void CLobbyMain::On_SpinBarInit(int param)
{
	m_pStatBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[3]->SetRange(0, 30, 0);
}

void CLobbyMain::On_CurveBarInit(int param)
{
	m_pStatBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pStatBar[4]->SetRange(0, 30, 0);
}

bool CLobbyMain::OnCreateNickResult(int result, FrForm* form)
{
	m_pCreateNickDlg = NULL;

	if (result == 0)
	{
	}
	else if (result == 1)
	{
		if (IsLocalContent(S4_NT_NICKNAME_CHANGE) && m_pServerDlg)
			m_pServerDlg->ClickDisableList(true);
	}

	return true;
}

bool CLobbyMain::OnMissionEventResult(int result, FrForm* form)
{
	if (!IsLocalContent(S4_NT_EVENT_MISSION))
		return false;

	m_pMissionEventDlg = NULL;

	return true;
}

bool CLobbyMain::OnBingoEventResult(int result, FrForm* form)
{
	if (!IsLocalContent(S4_NT_EVENT_BINGO))
		return false;

	m_pBingoEventDlg = NULL;

	return true;
}

bool CLobbyMain::OnKoohBirthdayEventDlg(int result, FrForm* form)
{
	if (!IsLocalContent((localContentType_t)0x8e))
		return false;

	m_pToppageKoohBirthdayEvent = NULL;

	return true;
}

void CLobbyMain::JoinOtherServerRoom()
{
}

bool CLobbyMain::OnMatchingDlgResult(int result, FrForm* form)
{
	if (!IsLocalContent(S4_MATCHING_SYSTEM))
		return false;

	if (m_pInviteDlg == NULL)
		return false;

	bool bAccept = false;

	switch (result)
	{
	case 1:
		bAccept = true;
		break;

	case 2:
		break;

	default:
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)"\xb8\xc5\xc4\xaa\xc0\xcc \xc3\xeb\xbc\xd2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		break;
	}

	WSendPacket packet((enumClientPacket)0xb8);
	packet.Encode4(m_pInviteDlg->GetFromUID());
	packet.Encode1(bAccept);
	packet.Send(TO_GAME);

	m_pInviteDlg = NULL;

	return true;
}

void CLobbyMain::RefreshAllUccClothes()
{
	if (CUserInfo::Instance()->GetDlg())
		return;

	sUccRefreshItem item;

	for (int i = 0; i < 10; i++)
	{
		if (!UccManager()->GetRefreshItem(item))
			break;

		for (std::list<sSlotInfo*>::iterator it = m_blueTeam.begin();
			it != m_blueTeam.end(); it++)
		{
			_RefreshUccClothes(*it, item.typeId, item.uccIndex);
		}

		for (std::list<sSlotInfo*>::iterator it2 = m_redTeam.begin();
			it2 != m_redTeam.end(); it2++)
		{
			_RefreshUccClothes(*it2, item.typeId, item.uccIndex);
		}

		if (m_pRoomUser)
		{
			for (int j = 0; j < m_pRoomUser->GetCurrentItemSize(false); j++)
			{
				FrListItem* pItem = m_pRoomUser->GetItem(j);
				if (pItem)
				{
					_RefreshUccClothes((sSlotInfo*)pItem->pData, item.typeId,
						item.uccIndex);
				}
			}
		}
	}
}

void CLobbyMain::OnToppage_UCCShopBtnUp()
{
	CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
	AfxPostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0, 0, 0);
	AfxPostMsg(NULL, "Shop", 198, 3, 0, 0, 0);
}

void CLobbyMain::OnToppage_CardShopBtnUp()
{
	CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
	AfxPostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0, 0, 0);
	AfxPostMsg(NULL, "Shop", 198, 4, 0, 0, 0);
}

float CLobbyMain::GetTopPageBtnRect()
{
	if (IsLocalContent(S4_TOPPAGE_BTN_LINEUP))
	{
		float x;
		if (CLoginInfo::Instance()->IsPcBang())
			x = (float)m_topBtnIndex * 80.0f;
		else
			x = (float)m_topBtnIndex * 80.0f;
		m_topBtnIndex++;
		return x;
	}

	return 0.0f;
}

void CLobbyMain::CheckTikiReport(const char* name)
{
	if (!IsLocalContent(S3_CADDIE_REPORT))
		return;

	if (strcmp(name, "GAMEROOM_EXTRES") == 0)
	{
		if ((Doc()->m_myInfo.info.dwIdentity & 2) ||
			(Doc()->m_myInfo.info.dwIdentity & 4) ||
			(Doc()->m_myInfo.info.dwIdentity & 8))
		{
			m_bUseReport = false;
		}
		else
		{
			for (std::list<sItemInfo>::iterator it =
					 Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); it++)
			{
				sItemInfo info = *it;
				if (info.tid == 0x1a000041)
				{
					m_bUseReport = true;
					break;
				}
				else
				{
					m_bUseReport = false;
				}
			}
		}

		if (Doc()->m_roomInfo.gameType == GAME_TYPE_GUILD_MATCH)
			m_bUseReport = false;

		if (Doc()->m_roomInfo.nUserLimit > 30)
			m_bUseReport = false;

		if (m_pUniteResultDlg)
			m_bUseReport = false;

		if (Doc()->m_curChannel.Type & 0x800)
			m_bUseReport = false;

		if (Doc()->m_myInfo.stat.Level <= 5)
			m_bUseReport = false;
	}
}

void CLobbyMain::OnToppage_TopIcon1Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(0, param);
}

void CLobbyMain::OnToppage_TopIcon1Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(0);
}

void CLobbyMain::OnToppage_TopIcon2Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(1, param);
}

void CLobbyMain::OnToppage_TopIcon2Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(1);
}

void CLobbyMain::OnToppage_TopIcon3Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(2, param);
}

void CLobbyMain::OnToppage_TopIcon3Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(2);
}

void CLobbyMain::OnToppage_TopIcon4Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(3, param);
}

void CLobbyMain::OnToppage_TopIcon4Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(3);
}

void CLobbyMain::OnToppage_TopIcon5Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(4, param);
}

void CLobbyMain::OnToppage_TopIcon5Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(4);
}

void CLobbyMain::OnToppage_TopIcon6Init(int param)
{
	CIconManager::Instance()->InitalizeTopIcon(5, param);
}

void CLobbyMain::OnToppage_TopIcon6Up()
{
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(5);
}

ILFILLTU_LOBBYMAIN
