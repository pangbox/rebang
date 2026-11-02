#include "stdafx.h"
#include "exhibition.h"
#include "tutorialmain.h"
#include "../../shared/sharedtables.h"
#include "objectfactory.h"
#include "actor.h"
#include "fresh.h"
#include "rankinginfo.h"
#include "frviewer.h"
#include "frbutton.h"
#include "chatmsg.h"
#include "questinfodlg.h"
#include "frarea.h"
#include "wview.h"

#include "rankingdlg.h"
#include "user_info.h"
#include "inputmanager.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frgraphicinterface.h"
#include "../../shared/localize.h"
#include "shareddoc.h"
#include "wpuppet.h"
#include "mousecursor.h"
#include "taskmanager.h"
#include "packet.h"

#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include "tutorialmain.inl"

extern Fresh* g_pFresh;

struct sQuestReward
{
	unsigned long m_mask;
	unsigned long m_typeId;
	int m_count;
};

extern "C"
{
	static const sQuestReward data_9f7be8[20] = {
		{ 0x1,     0x1A00000F, 3   },
		{ 0x2,     0x18000007, 3   },
		{ 0x4,     0x18000005, 3   },
		{ 0x8,     0x18000008, 3   },
		{ 0x10,    0x1A000010, 500 },
		{ 0x20,    0x18000004, 3   },
		{ 0x40,    0x1A000010, 500 },
		{ 0x80,    0,          1   },
		{ 0x100,   0x1A00000F, 3   },
		{ 0x200,   0x18000028, 1   },
		{ 0x400,   0x18000006, 1   },
		{ 0x800,   0x18000007, 5   },
		{ 0x1000,  0x18000000, 4   },
		{ 0x2000,  0x18000001, 4   },
		{ 0x8000,  0x18000005, 3   },
		{ 0x10000, 0x18000005, 3   },
		{ 0x20000, 0x18000005, 3   },
		{ 0x40000, 0x18000005, 3   },
		{ 0x80000, 0x18000005, 3   },
		{ 0x80000, 0x18000005, 3   },
	};
}

static char s_actorClass[12] = "CTutorial";
static char s_actorName[12] = "Tutorial";

static char s_tutorialTasks[3][16] = { "CGolfTask", "CShopTask",
	"CRealMyRoomTask" };
static char s_tutorialTaskParams[3][16] = { "CGolfDoc", "", "" };
static char s_tutorialMsgTargets[3][16] = { "", "Shop", "RealMyRoom" };
static char s_tutorialMsgParams[3][16] = { "", "SHOPMAIN", "RealMyRoom" };

const CTutorialMain::sTutorialCondition CTutorialMain::m_TutoCondition[8] = {
	{ 0, 0, 1, 1 },
	{ 1, 0, 2, 2 },
	{ 2, 0, 4, 2 },
	{ 3, 0, 5, 2 },
	{ 4, 7, 4, 3 },
	{ 5, 7, 4, 3 },
	{ 6, 7, 4, 3 },
	{ 7, 0, 1, 1 },
};

const CTutorialMain::sChildTable CTutorialMain::m_ChildTable[14] = {
	{ 0,  1,  1 },
	{ 1,  2,  2 },
	{ 2,  4,  2 },
	{ 3,  5,  2 },
	{ 4,  7,  1 },
	{ 5,  7,  1 },
	{ 6,  7,  1 },
	{ 7,  7,  0 },
	{ 8,  9,  2 },
	{ 9,  11, 1 },
	{ 10, 11, 1 },
	{ 11, 12, 2 },
	{ 12, 12, 0 },
	{ 13, 13, 0 },
};

CTutorialMain::sTutorialResult CTutorialMain::m_CompleteItems[3] = {
	{ 0, 3 },
	{ 1, 3 },
	{ 2, 3 },
};

CTutorialMain::sCategoryInfo CTutorialMain::m_sCategoryItem[3] = {
	{ 0, 8, 0, 200 },
	{ 1, 6, 0, 370 },
	{ 2, 6, 0, 0   },
};

CTutorialMain::sArrowInfo CTutorialMain::m_ArrowItems[10] = {
	{ 0, 0, 1 },
	{ 1, 1, 1 },
	{ 2, 2, 1 },
	{ 3, 3, 1 },
	{ 4, 6, 1 },
	{ 7, 7, 1 },
	{ 0, 1, 1 },
	{ 2, 2, 1 },
	{ 3, 3, 1 },
	{ 4, 5, 1 },
};

struct sTutorialImage
{
	int m_id;
	const char* m_name;
};

static const sTutorialImage s_tutorialImages[64] = {
	{ 0,  "tu_btn_01_n"       },
	{ 1,  "tu_btn_02_n"       },
	{ 2,  "tu_btn_03_n"       },
	{ 3,  "tu_btn_04_n"       },
	{ 4,  "tu_btn_05_n"       },
	{ 5,  "tu_btn_06_n"       },
	{ 6,  "tu_btn_07_n"       },
	{ 7,  "tu_btn_08_n"       },
	{ 8,  "tu_btn_09_n"       },
	{ 9,  "tu_btn_10_n"       },
	{ 10, "tu_btn_11_n"       },
	{ 11, "tu_btn_12_n"       },
	{ 12, "tu_btn_13_n"       },
	{ 13, "tu_btn_14_n"       },
	{ 14, "tu_btn_01_o"       },
	{ 15, "tu_btn_02_o"       },
	{ 16, "tu_btn_03_o"       },
	{ 17, "tu_btn_04_o"       },
	{ 18, "tu_btn_05_o"       },
	{ 19, "tu_btn_06_o"       },
	{ 20, "tu_btn_07_o"       },
	{ 21, "tu_btn_08_o"       },
	{ 22, "tu_btn_09_o"       },
	{ 23, "tu_btn_10_o"       },
	{ 24, "tu_btn_11_o"       },
	{ 25, "tu_btn_12_o"       },
	{ 26, "tu_btn_13_o"       },
	{ 27, "tu_btn_14_o"       },
	{ 28, "tu_btn_01_d"       },
	{ 29, "tu_btn_02_d"       },
	{ 30, "tu_btn_03_d"       },
	{ 31, "tu_btn_04_d"       },
	{ 32, "tu_btn_05_d"       },
	{ 33, "tu_btn_06_d"       },
	{ 34, "tu_btn_07_d"       },
	{ 35, "tu_btn_08_d"       },
	{ 36, "tu_btn_09_d"       },
	{ 37, "tu_btn_10_d"       },
	{ 38, "tu_btn_11_d"       },
	{ 39, "tu_btn_12_d"       },
	{ 40, "tu_btn_13_d"       },
	{ 41, "tu_btn_14_d"       },
	{ 42, "tu_btn_lock"       },
	{ 43, "tu_empty_first"    },
	{ 44, "tu_empty_first_n"  },
	{ 45, "tu_off_frame01"    },
	{ 46, "tu_off_frame02"    },
	{ 47, "tu_off_frame03"    },
	{ 48, "message_lock_n"    },
	{ 49, "tu_icon_clear"     },
	{ 50, "tu_icon_new"       },
	{ 51, "tu_arrow_n"        },
	{ 52, "tu_arrow_o"        },
	{ 53, "tu_icon_lock_01"   },
	{ 54, "tu_icon_lock_02"   },
	{ 55, "tu_obtain_01"      },
	{ 56, "tu_obtain_02_nuri" },
	{ 57, "tu_obtain_02_hana" },
	{ 58, "tu_class_01"       },
	{ 59, "tu_class_02"       },
	{ 60, "tu_class_03"       },
	{ 61, "tu_tip_01"         },
	{ 62, "tu_tip_02"         },
	{ 63, "tu_tip_03"         },
};

// HACK!
namespace
{
	inline bool IsValidRange(float f)
	{
		return f < g_HUGE && f > g_EPSILON;
	}
}

CTutorialMain::sTutorialMap CTutorialMain::m_Tutorials[14] = {
	{ K2L_Compatibility("\xc5\xac\xb7\xb4\xbc\xb1\xc5\xc3"),          0, 1,  0, 0, 0,  0,
     0,																					  true },
	{ K2L_Compatibility("\xc5\xb8\xb1\xb8\xb9\xe6\xb9\xfd"),          0, 2,  0, 1, 1,  0,
     0,																					  true },
	{ K2L_Compatibility("\xc6\xce\xbe\xdf\xb0\xd4\xc0\xcc\xc1\xf6"),  0, 3,  0, 2,
     2,																				0, 0, true },
	{ K2L_Compatibility("\xc6\xdb\xc6\xc3\xc0\xc7 \xb1\xe2\xba\xbb"), 0, 4,  0,
     2,																			3,  0, 0, true },
	{ K2L_Compatibility("\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb"),  0, 5,  0, 3,
     4,																				0, 0, true },
	{ K2L_Compatibility("\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba"),          0, 6,  0, 3, 5,  0,
     0,																					  true },
	{ K2L_Compatibility("\xbd\xba\xc4\xda\xbe\xee\xb0\xe8\xbb\xea"),  0, 7,  0, 3,
     6,																				0, 0, true },
	{ K2L_Compatibility("\xc8\xa6\xc0\xce\xc7\xcf\xb1\xe2"),          0, 8,  0, 4, 7,  0,
     0,																					  true },
	{ K2L_Compatibility("\xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2"),          1, 9,  0, 0, 8,  0,
     0,																					  true },
	{ K2L_Compatibility("\xb0\xf8\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2"), 1, 10, 0,
     1,																			9,  0, 0, true },
	{ K2L_Compatibility("\xb9\xd9\xb6\xf7 \xc0\xd0\xb1\xe2"),         1, 11, 0, 1, 10,
     0,																				   0, true },
	{ K2L_Compatibility("\xc5\xb8\xc1\xa1\xc0\xcc\xb5\xbf"),          1, 12, 0, 2, 11, 0,
     0,																					  true },
	{ K2L_Compatibility("\xc6\xc4\xbf\xf6 \xbd\xba\xc7\xc9"),         1, 13, 0, 3, 12,
     0,																				   0, true },
	{ K2L_Compatibility("\xc6\xc4\xbf\xf6 \xc4\xbf\xba\xea"),         1, 14, 0, 3, 13,
     0,																				   0, true },
};

extern unsigned long g_CurrentTime;
extern WMatrix g_camera;

IMPLEMENT_ACTOR(CTutorialMain, CTaskMain)

BEGIN_FRESH_MSGMAP(CTutorialMain, CTaskMain)
ON_FRESH_VI("TUTORIAL_MAIN", FRCMD_INIT, CTutorialMain::OnTutorialMainInit)
ON_FRESH_VV("TUTORIAL_MAIN", FRCMD_FINISH, CTutorialMain::OnTutorialMainFinish)
ON_FRESH_VI("TUTORIAL_MAIN.pet_pipin", FRCMD_INIT,
	CTutorialMain::OnTutorialAreaPipin)
ON_FRESH_VI("TUTORIAL_MAIN.pet_pipin", FRCMD_OWNERDRAW,
	CTutorialMain::OnTutorialAreaPipinOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.tutorial_list", FRCMD_INIT,
	CTutorialMain::OnTutorialListInit)
ON_FRESH_VI("TUTORIAL_MAIN.tutorial_list", FRCMD_OWNERDRAW,
	CTutorialMain::OnTutorialListOwnerDraw)
ON_FRESH_VV("TUTORIAL_MAIN.tutorial_list", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnTutorialListLBtnDown)
ON_FRESH_VV("TUTORIAL_MAIN.tutorial_list", FRCMD_LBUTTONUP,
	CTutorialMain::OnTutorialListLBtnUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.back", FRCMD_INIT,
	CTutorialMain::OnUnderBar_BackInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.back", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_BackUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.back", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.back", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.front", FRCMD_INIT,
	CTutorialMain::OnUnderBar_FrontInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.front", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_FrontUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.front", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.front", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.shop", FRCMD_INIT,
	CTutorialMain::OnUnderBar_ShopInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.shop", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_ShopUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.shop", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.shop", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.myroom", FRCMD_INIT,
	CTutorialMain::OnUnderBar_MyRoomInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.myroom", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_MyRoomUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.myroom", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.myroom", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.itemstorage", FRCMD_INIT,
	CTutorialMain::OnUnderBar_ItemStorageInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.itemstorage", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_ItemStorageUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.itemstorage", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.itemstorage", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.gift", FRCMD_INIT,
	CTutorialMain::OnUnderBar_GiftInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.gift", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_GiftUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.gift", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.gift", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.cookie", FRCMD_INIT,
	CTutorialMain::OnUnderBar_CookieInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.cookie", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_CookieUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.cookie", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.cookie", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.game", FRCMD_INIT,
	CTutorialMain::OnUnderBar_GameInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.game", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_GameUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.game", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.game", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.server", FRCMD_INIT,
	CTutorialMain::OnUnderBar_ServerInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.server", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_ServerUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.server", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.server", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.exit", FRCMD_INIT,
	CTutorialMain::OnUnderBar_ExitInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.exit", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_ExitUp)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.exit", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.exit", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.ubar.tabbtn", FRCMD_INIT,
	CTutorialMain::OnUnderBar_TabBtnInit)
ON_FRESH_VV("TUTORIAL_MAIN.ubar.tabbtn", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_TabBtnUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar", FRCMD_INIT, CTutorialMain::OnOverBar_Init)
ON_FRESH_VI("TUTORIAL_MAIN.utab", FRCMD_INIT, CTutorialMain::OnUnderTab_Init)
ON_FRESH_VI("TUTORIAL_MAIN.ubar", FRCMD_INIT, CTutorialMain::OnUnderBarInit)
ON_FRESH_VI("TUTORIAL_MAIN.utab.magicbox", FRCMD_INIT,
	CTutorialMain::OnUnderBar_MagicBoxInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.magicbox", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_MagicBoxUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.magicbox", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.magicbox", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_INIT,
	CTutorialMain::OnUnderBar_TikiMagicBoxInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_TikiMagicBoxUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.scratch", FRCMD_INIT,
	CTutorialMain::OnUnderBar_ScratchInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.scratch", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_ScratchUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.scratch", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.scratch", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.bongdarishop", FRCMD_INIT,
	CTutorialMain::OnUnderBar_BongdariShopInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.bongdarishop", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnUnderBar_BongdariShopUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.bongdarishop", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.bongdarishop", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.maket", FRCMD_INIT,
	CTutorialMain::OnUnderBar_MaketInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.maket", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_MaketUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.maket", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.maket", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.ranking", FRCMD_INIT,
	CTutorialMain::OnUnderBar_RankingInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.ranking", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_RankingUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.ranking", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.ranking", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.guild", FRCMD_INIT,
	CTutorialMain::OnUnderBar_GuildInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.guild", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_GuildUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.guild", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.guild", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.utab.notice", FRCMD_INIT,
	CTutorialMain::OnUnderBar_NoticeInit)
ON_FRESH_VV("TUTORIAL_MAIN.utab.notice", FRCMD_LBUTTONUP,
	CTutorialMain::OnUnderBar_NoticeUp)
ON_FRESH_VI("TUTORIAL_MAIN.utab.notice", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.utab.notice", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.obar.title", FRCMD_INIT,
	CTutorialMain::OnOverBar_TitleInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.site", FRCMD_INIT,
	CTutorialMain::OnOverBar_SiteInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.char", FRCMD_INIT,
	CTutorialMain::OnOverBar_CharInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.char", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnOverBar_CharDown)
ON_FRESH_VI("TUTORIAL_MAIN.obar.char", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_CharOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.caddie", FRCMD_INIT,
	CTutorialMain::OnOverBar_CaddieInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.caddie", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnOverBar_CaddieDown)
ON_FRESH_VI("TUTORIAL_MAIN.obar.caddie", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_CaddieOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.club", FRCMD_INIT,
	CTutorialMain::OnOverBar_ClubInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.club", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnOverBar_ClubDown)
ON_FRESH_VI("TUTORIAL_MAIN.obar.club", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_ClubOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.aztec", FRCMD_INIT,
	CTutorialMain::OnOverBar_AztecInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.aztec", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnOverBar_AztecDown)
ON_FRESH_VI("TUTORIAL_MAIN.obar.aztec", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_AztecOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.mascot", FRCMD_INIT,
	CTutorialMain::OnOverBar_MascotInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.mascot", FRCMD_LBUTTONDOWN,
	CTutorialMain::OnOverBar_MascotDown)
ON_FRESH_VI("TUTORIAL_MAIN.obar.mascot", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_MascotOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.char_sel", FRCMD_INIT,
	CTutorialMain::OnOverBar_CharSelInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.char_sel", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_CharSelOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.caddie_sel", FRCMD_INIT,
	CTutorialMain::OnOverBar_CaddieSelInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.caddie_sel", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_CaddieSelOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.club_sel", FRCMD_INIT,
	CTutorialMain::OnOverBar_ClubSelInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.club_sel", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_ClubSelOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.aztec_sel", FRCMD_INIT,
	CTutorialMain::OnOverBar_AztecSelInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.aztec_sel", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_AztecSelOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.mascot_sel", FRCMD_INIT,
	CTutorialMain::OnOverBar_MascotSelInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.mascot_sel", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_MascotSelOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.pang_cookie", FRCMD_INIT,
	CTutorialMain::OnOverBar_PangCookieInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.pang_cookie", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_PangCookieOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.onelinemsg_req", FRCMD_INIT,
	CTutorialMain::OnOverBar_OneLineReqBtnInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.onelinemsg_req", FRCMD_LBUTTONUP,
	CTutorialMain::OnOverBar_OneLineReqBtnUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.onelineview", FRCMD_INIT,
	CTutorialMain::OnOverBar_OneLineViewInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.onelineview", FRCMD_OWNERDRAW,
	CTutorialMain::OnOverBar_OneLineViewOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.spcard", FRCMD_INIT,
	CTutorialMain::OnOverBar_SpecialCardInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.spcard", FRCMD_LBUTTONUP,
	CTutorialMain::OnOverBar_SpecialCardUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.spcard", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.obar.spcard", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger", FRCMD_INIT,
	CTutorialMain::OnOverBar_MessengerInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.messenger", FRCMD_LBUTTONUP,
	CTutorialMain::OnOverBar_MessengerUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.obar.myinfo", FRCMD_INIT,
	CTutorialMain::OnOverBar_MyInfoInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.myinfo", FRCMD_LBUTTONUP,
	CTutorialMain::OnOverBar_MyInfoUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.myinfo", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.obar.myinfo", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.obar.option", FRCMD_INIT,
	CTutorialMain::OnOverBar_OptionInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.option", FRCMD_LBUTTONUP,
	CTutorialMain::OnOverBar_OptionUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.option", FRCMD_HOVERON,
	CTutorialMain::OnBarHoverOn)
ON_FRESH_VI("TUTORIAL_MAIN.obar.option", FRCMD_HOVEROFF,
	CTutorialMain::OnBarHoverOff)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger_bubble", FRCMD_INIT,
	CTutorialMain::OnMessengerBubbleInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger_bubble", FRCMD_OWNERDRAW,
	CTutorialMain::OnMessengerBubbleOwnerDraw)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger_desc", FRCMD_INIT,
	CTutorialMain::OnMessengerBubbleEditInit)
ON_FRESH_VI("TUTORIAL_MAIN.obar.messenger_alarm", FRCMD_INIT,
	CTutorialMain::OnMessengerAlarmBtnInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.messenger_alarm", FRCMD_LBUTTONUP,
	CTutorialMain::OnMessengerAlarmBtnUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.gift_alarm", FRCMD_INIT,
	CTutorialMain::OnMessengerGiftAlarmBtnInit)
ON_FRESH_VV("TUTORIAL_MAIN.obar.gift_alarm", FRCMD_LBUTTONUP,
	CTutorialMain::OnMessengerGiftAlarmBtnUp)
ON_FRESH_VI("TUTORIAL_MAIN.obar.tooltip", FRCMD_OWNERDRAW,
	CTutorialMain::OnBarTooltipOwnerDraw)
END_FRESH_MSGMAP()

CTutorialMain::CTutorialMain(void)
{
	m_questState = -1;
	m_questCategory = -1;
	m_questNextIndex = -1;
	m_questIndex = -1;
	m_tutorialListBox = 0;
	m_tutorialInfoDlg = 0;
	m_questGift = 0;
	m_listState = 0;
	m_pipinArea = 0;
	m_pet = 0;
	m_bBongdariShop2 = false;
	m_questReward = false;

	m_gradeGift = 0;

	m_camDirection = 1.0f;
	m_camPaused = false;
	m_camMoveTime = g_CurrentTime;

	m_overBarBtnPos[0] = WPoint(40.0f, 90.0f);
	m_overBarBtnPos[1] = WPoint(129.0f, 90.0f);
	m_overBarBtnPos[2] = WPoint(218.0f, 58.0f);
	m_overBarBtnPos[3] = WPoint(218.0f, 119.0f);
	m_overBarBtnPos[4] = WPoint(307.0f, 46.0f);
	m_overBarBtnPos[5] = WPoint(307.0f, 90.0f);
	m_overBarBtnPos[6] = WPoint(307.0f, 134.0f);
	m_overBarBtnPos[7] = WPoint(396.0f, 90.0f);

	m_underBarBtnPos[0] = WPoint(129.0f, 91.0f);
	m_underBarBtnPos[1] = WPoint(218.0f, 64.0f);
	m_underBarBtnPos[2] = WPoint(218.0f, 117.0f);
	m_underBarBtnPos[3] = WPoint(307.0f, 91.0f);
	m_underBarBtnPos[4] = WPoint(396.0f, 64.0f);
	m_underBarBtnPos[5] = WPoint(396.0f, 117.0f);

	m_columnX[0] = 121.0f;
	m_columnX[1] = 215.0f;
	m_columnX[2] = 303.0f;
	m_columnX[3] = 395.0f;
	m_columnX[4] = 486.0f;
	m_pendingTutorial = 0xFF;
	m_iconOffsetX = 0.0f;
	m_iconOffset = 0.0f;
	m_iconOffsetZ = 0.0f;

	m_categoryCount = IsLocalContent((localContentType_t)70) ? 2 : 1;
}

CTutorialMain::~CTutorialMain(void)
{
	if (m_pet)
	{
		delete m_pet;
		m_pet = 0;
	}
}

void CTutorialMain::OnPreserveBack(const char* name)
{
	HidePet();

	CTaskMain::OnPreserveBack(name);
}

void CTutorialMain::OnRestorePreserved(void)
{
	CTaskMain::OnRestorePreserved();

	WSingleton<CChatMsg>::Instance()->SetActive(true, true, true);
}

void CTutorialMain::OnLoad(void)
{
	CTaskMain::OnLoad();

	LoadTutorialMainImages();
}

void CTutorialMain::OnInit(void)
{
	m_tutorialList.clear();

	for (int i = 0; i < 14; i++)
	{
		m_Tutorials[i].m_status = 0;
	}

	int parents = 0;
	for (int category = 0; category < 3; category++)
	{
		int end = GetAccumulateLimit(category);
		for (int i = GetGradeBegin_Grade(category); i < end; i++)
		{
			int next = m_ChildTable[i].m_first;
			for (int j = 0; j < m_ChildTable[i].m_count; j++)
				m_questInfo[i].m_items.push_back(next++);

			for (int k = GetGradeBegin_Grade(category); k < end; k++)
			{
				if (i == k)
					continue;
				if (m_Tutorials[i].m_level - m_Tutorials[k].m_level < -1)
					break;

				if (m_Tutorials[i].m_level - 1 == m_Tutorials[k].m_level)
				{
					for (unsigned int j = 0; j < m_questInfo[k].m_items.size();
						j++)
					{
						if (m_Tutorials[i].m_quest == m_questInfo[k].m_items[j])
							parents++;
					}
				}
			}

			m_questInfo[i].m_parentCount = parents;
			m_questInfo[i].m_parentsCleared = 0;
			parents = 0;
		}
	}

	CTaskMain::OnInit();
}

void CTutorialMain::LoadTutorialMainImages(void)
{
	for (int i = 0; i < 64; i++)
		m_images[i] = g_pFresh->RegisterBitmap(s_tutorialImages[i].m_name);
}

void CTutorialMain::EnableAllBtn(void)
{
	for (int i = 0; i < 14; i++)
		m_Tutorials[i].m_enabled = true;
}

void CTutorialMain::SetCompleteStates(void)
{
	for (int category = 0; category < m_categoryCount; category++)
	{
		CSharedDoc* doc = Doc();
		int end = GetAccumulateLimit(category);
		for (int i = GetGradeBegin_Grade(category); i < end; i++)
		{
			m_Tutorials[i].m_status = 0;
			switch (category)
			{
			case 0:
				if (doc->m_tutorialComplete[0] & data_9f7be8[i].m_mask)
					m_Tutorials[i].m_status = 1;
				break;

			case 1:
				if (doc->m_tutorialComplete[0] == 0xFF)
				{
					if (doc->m_tutorialComplete[1] & data_9f7be8[i].m_mask)
						m_Tutorials[i].m_status = 1;
				}
				else
				{
					m_Tutorials[i].m_status = 2;
				}
				break;

			case 2:
				if (doc->m_tutorialComplete[0] == 0xFF &&
					doc->m_tutorialComplete[1] == 0x3F00)
				{
					if (doc->m_tutorialComplete[2] & data_9f7be8[i].m_mask)
						m_Tutorials[i].m_status = 1;
				}
				else
				{
					m_Tutorials[i].m_status = 2;
					continue;
				}
				break;
			}
		}
	}
}

void CTutorialMain::CheckNodeStates(void)
{
	int sameLevel = 0;

	for (int category = 0; category < m_categoryCount; category++)
	{
		switch ((unsigned char)category)
		{
		case 0:
			break;
		case 1:
			if (Doc()->m_tutorialComplete[0] != 0xFF)
				return;
			break;
		case 2:
			if (Doc()->m_tutorialComplete[0] != 0xFF ||
				Doc()->m_tutorialComplete[1] != 0x3F00)
				return;
			break;
		default:
			return;
		}

		int end = GetAccumulateLimit(category);
		for (int i = GetGradeBegin_Grade(category); i < end; i++)
			m_questInfo[i].m_parentsCleared = 0;

		for (int i = GetGradeBegin_Grade(category); i < end; i++)
		{
			for (unsigned int j = 0; j < m_questInfo[i].m_items.size(); j++)
			{
				int child = m_questInfo[i].m_items[j];
				if (m_Tutorials[i].m_status != 1)
				{
					if (m_Tutorials[child].m_status == 0)
						m_Tutorials[child].m_status = 2;
				}
				else
				{
					if (m_questInfo[child].m_parentCount >
						m_questInfo[child].m_parentsCleared)
						m_questInfo[child].m_parentsCleared++;
				}

				if (m_questInfo[child].m_parentCount ==
					m_questInfo[child].m_parentsCleared)
				{
					if (m_Tutorials[child].m_status != 1)
						m_Tutorials[child].m_status = 0;
				}

				if (m_questInfo[child].m_parentCount >= 2 &&
					m_Tutorials[child].m_status != 1)
				{
					for (int k = GetGradeBegin_Grade(category); k < end; k++)
					{
						if (m_Tutorials[child].m_level ==
							m_Tutorials[k].m_level)
							sameLevel++;
					}

					if (sameLevel > 2 &&
						m_questInfo[child].m_parentsCleared >= 1)
						m_Tutorials[child].m_status = 0;

					sameLevel = 0;
				}
			}
		}

		for (int c = 0; c < 3; c++)
			m_CompleteItems[c].m_count =
				IsCompleteCategory(m_CompleteItems[c].m_category) == true ? 4
																		  : 3;
	}
}

void CTutorialMain::ShowPet(void)
{
	if (m_pet)
	{
		delete m_pet;
		m_pet = 0;
	}

	bool loaded;
	CExhibition* pet = new CExhibition(85.0f, 245.0f, 0.35f, false);
	if (pet)
	{
		pet->SetArea(150.0f, 340.0f);
		pet->SetDistanceRange(9.0f, 13.0f);
		pet->SetDistance(11.0f, true, false);
		loaded = pet->SetModel("pipin", EXHIBIT_CADDIE);
		pet->SetControlFlag(0);
		pet->SetDirection(-0.45f);
		m_pet = pet;
		m_pipinTime.m_nextMotion = "\xb1\xde\xc1\xa4\xb0\xc5";

		m_pipinTime.Reset();
		m_pipinTime.SetTime();
	}

	if (!loaded)
	{
		if (pet)
			delete pet;
	}
}

void CTutorialMain::HidePet(void)
{
	if (m_pet)
	{
		delete m_pet;
		m_pet = 0;
	}
}

void CTutorialMain::ProcessLobbyBgCamera(float dt)
{
	if (Doc()->m_bBackgroundVideo)
		return;

	if (m_camPaused)
	{
		if (g_CurrentTime - m_camPauseTime > 2000)
		{
			m_camDirection *= -1.0f;
			m_camPaused = false;
			m_camMoveTime = g_CurrentTime;
		}
		return;
	}

	unsigned long elapsed = g_CurrentTime - m_camMoveTime;
	if (elapsed > 80000)
	{
		m_camPaused = true;
		m_camPauseTime = g_CurrentTime;
		return;
	}

	float speed = 1.0f;
	if (elapsed < 2000)
		speed = elapsed * 0.0005f;
	else if (elapsed > 78000)
		speed = 1.0f - (elapsed - 78000) * 0.0005f;

	g_camera.pivot += g_camera.xa * m_camDirection * dt * 0.3f * speed;
}

void CTutorialMain::OnProcess(float dt)
{
	static float s_motionTime;
	static int s_idleElapsed;
	static int s_motionElapsed;

	if (m_pet)
	{
		m_pet->Process(dt, false);

		w_motion_data* motion = m_pet->GetMotionData();
		s_motionTime += dt;
		if (s_motionTime > motion->e - motion->s)
		{
			if (m_pipinTime.m_nextMotion.size() > 2 &&
				strcmp(m_pipinTime.m_nextMotion.c_str(), motion->name) != 0)
			{
				m_pet->SetMotion(m_pipinTime.m_nextMotion.c_str(), false, 0.0f);
				s_motionTime = 0.0f;
				m_pipinTime.m_nextMotion = "";
			}
		}

		s_idleElapsed = timeGetTime() - m_pipinTime.m_lastIdleTime;
		if (s_idleElapsed > 16000)
		{
			DecideNextMotion(3);
			m_pipinTime.m_lastIdleTime = timeGetTime();
			s_idleElapsed = 0;
		}
		else
		{
			s_motionElapsed = timeGetTime() - m_pipinTime.m_lastMotionTime;
			if (s_motionElapsed > 5000)
			{
				DecideNextMotion(rand() % 3);
				m_pipinTime.m_lastMotionTime = timeGetTime();
			}
		}
	}

	ProcessAnimateIcon();

	CTaskMain::OnProcess(dt);
}

void CTutorialMain::ProcessAnimateIcon(void)
{
	static float s_iconAngle;
	m_iconOffset = cos(s_iconAngle * g_DEGTORAD) * 5.0f;
	s_iconAngle += 1.0f;
	if (s_iconAngle > 360.0f)
		s_iconAngle = 0.0f;
}

void CTutorialMain::OnDestroy(void)
{
	m_pipinTime.Reset();
	m_tutorialList.clear();
	CTaskMain::OnDestroy();
}

void CTutorialMain::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0:
	{
		if (g_pFresh == 0)
			return;

		const char* layout = (const char*)msg.param1;
		g_pFresh->OpenLayout(layout, this, true);

		ShowPet();

		SetCompleteStates();
		CheckNodeStates();
		DrawCourseArrow();
		break;
	}

	case 0x151:
	{
		unsigned char openGift = (unsigned char)msg.param2;

		SetCompleteStates();
		CheckNodeStates();
		for (int i = 0; i < 14; i++)
			m_Tutorials[i].m_enabled = true;

		DrawCourseArrow();

		if (openGift)
			OpenQuestGift();
		break;
	}

	case 0x14E:
	{
		*(char**)msg.param1 = s_actorClass;
		*(char**)msg.param2 = s_actorName;
		break;
	}

	case 0x14F:
		*(int*)msg.param1 = m_questState;
		*(int*)msg.param2 = m_questCategory;
		*(int*)msg.param3 = m_questNextIndex;
		*(int*)msg.param4 = m_questIndex;
		*(int*)msg.param5 = m_questValue;
		break;

	case 0x150:
	{
		int index = msg.param1;
		unsigned long result = msg.param2;

		m_listState = result;
		m_Tutorials[index].m_enabled = false;

		if (msg.param3 == 1 && m_questReward)
		{
			m_questReward = false;
			WSendPacket packet((enumClientPacket)0xA6);
			packet.Encode1((unsigned char)msg.param4);
			packet.Encode1(m_Tutorials[index].m_category);
			packet.Encode4(result);
			packet.Send(TO_GAME);
		}
		else
		{
			m_Tutorials[index].m_enabled = true;
		}
		break;
	}

	case 0x153:
		m_gradeGift = (unsigned char)msg.param1;
		break;

	case 0x18:
		if (msg.param1 == 0)
			return;

		m_questState = 0;
		m_questCategory = 0;
		m_questNextIndex = 1;
		m_questIndex = 0;
		StartSelectedTutorial();
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

void CTutorialMain::DisableOverBarButtons(void)
{
	if (m_pOverBarChar)
	{
		m_pOverBarChar->Enable(false);
	}

	if (m_pOverBarCaddie)
	{
		m_pOverBarCaddie->Enable(false);
	}

	if (m_pOverBarClub)
	{
		m_pOverBarClub->Enable(false);
	}

	if (m_pOverBarAztec)
	{
		m_pOverBarAztec->Enable(false);
	}

	if (m_pOverBarMascot)
	{
		m_pOverBarMascot->Enable(false);
	}

	if (m_pOverBarOneLineReqBtn)
	{
		m_pOverBarOneLineReqBtn->Enable(false);
	}
}

void CTutorialMain::DisableUnderBarButtons(void)
{
	if (m_pUnderBarBongdariShop)
	{
		m_pUnderBarBongdariShop->SetPushSound(0);
		m_pUnderBarBongdariShop->EnableHover(false, 0.0f);
		m_pUnderBarBongdariShop->Enable(false);
	}

	if (m_pUnderBarServer)
	{
		m_pUnderBarServer->SetPushSound(0);
		m_pUnderBarServer->EnableHover(false, 0.0f);
		m_pUnderBarServer->Enable(false);
	}

	if (m_pUnderBarMagicBox)
	{
		m_pUnderBarMagicBox->EnableHover(false, 0.0f);
		m_pUnderBarMagicBox->Enable(false);
	}

	if (m_pUnderBarScratch)
	{
		m_pUnderBarScratch->EnableHover(false, 0.0f);
		m_pUnderBarScratch->Enable(false);
	}
}

void CTutorialMain::OnTutorialMainInit(int)
{
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

void CTutorialMain::OnTutorialMainFinish(void)
{
	DisableOverBarButtons();
	DisableUnderBarButtons();

	if (Doc()->m_bRefreshCamera)
	{
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

void CTutorialMain::OnTutorialAreaPipin(int param)
{
	m_pipinArea = DYNAMIC_CAST(FrArea, param);
}

void CTutorialMain::OnTutorialAreaPipinOwnerDraw(int)
{
	if (!m_bBongdariShop2 && !WSingleton<CRankingInfo>::Instance()->GetDlg() &&
		!USERINFODLG())
	{
		if (m_pet)
			m_pet->Display(0.0f, 0.0f);
	}
}

void CTutorialMain::OnTutorialListInit(int param)
{
	m_tutorialListBox = DYNAMIC_CAST(FrListBox, param);
	if (m_tutorialListBox == 0)
		return;

	m_tutorialListBox->UseRightButton(true);
	m_tutorialListBox->UseDummy(true);
	m_tutorialListBox->SetPushSound(0);

	m_tutorialListBox->ClearItem();
	for (int i = 0; i < 3; i++)
	{
		m_tutorialListBox->AddItem(&m_CompleteItems[i]);
	}

	for (int c = 0; c < 3; c++)
		m_categoryClear[c] = false;
}

void CTutorialMain::OnTutorialListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item == 0)
		return;

	float x = item->pos.x;
	float rowY[3] = { 0.0f };
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	int topRow = m_tutorialListBox->GetScrollBar()->GetCurTopRow_Int();
	if (topRow == 0)
	{
		rowY[0] = 119.0f;
		rowY[1] = 314.0f;
	}
	else if (topRow == 1)
	{
		rowY[1] = 119.0f;
		rowY[2] = 314.0f;
	}

	switch (item->idx)
	{
	case 0:
	{
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[0], 30.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[0], 526.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[0], 30.0f, 196.0f),
			0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[58],
			WRect(200.0f, rowY[0] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[61],
			WRect(550.0f, rowY[0] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		WPoint pt;
		pt.x = g_input->GetMousePoint().x - x;
		pt.y = g_input->GetMousePoint().y - rowY[0];

		int hit = IsInArea(item->idx, pt);
		for (int i = 0; i < 8; i++)
		{
			if (m_Tutorials[i].m_pending == 0)
			{
				const Bitmap* img;
				if (m_Tutorials[i].m_status == 2)
				{
					img = m_images[42];
				}
				else
				{
					img = m_images[i];
				}

				if (hit != -1 && hit == i && m_Tutorials[i].m_status != 2)
				{
					pGDI->DrawTexture(m_images[hit + 14],
						WRect(x + m_overBarBtnPos[hit].x,
							rowY[0] + m_overBarBtnPos[hit].y, 86.0f, 48.0f),
						0xFFFFFFFF, 0);
				}
				else
				{
					pGDI->DrawTexture(img,
						WRect(x + m_overBarBtnPos[i].x,
							rowY[0] + m_overBarBtnPos[i].y, 85.0f, 48.0f),
						0xFFFFFFFF, 0);
				}
			}
			else if (m_pendingTutorial != 0xFF)
			{
				pGDI->DrawTexture(m_images[i + 28],
					WRect(x + m_overBarBtnPos[i].x,
						rowY[0] + m_overBarBtnPos[i].y, 85.0f, 48.0f),
					0xFFFFFFFF, 0);
			}

			if (m_Tutorials[i].m_status == 1)
			{
				pGDI->DrawTexture(m_images[49],
					WRect(x + m_overBarBtnPos[i].x + 60.0f,
						rowY[0] + m_overBarBtnPos[i].y + 22.0f, 19.0f, 19.0f),
					0xFFFFFFFF, 0);
			}
		}

		if (m_CompleteItems[0].m_count == 3)
		{
			pGDI->DrawTexture(m_images[53],
				WRect(668.0f, rowY[0] + 60.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
			pGDI->DrawTexture(m_images[54],
				WRect(668.0f, rowY[0] + m_iconOffset + 58.0f, 79.0f, 97.0f),
				0xFFFFFFFF, 0);
		}
		else
		{
			pGDI->DrawTexture(m_images[55],
				WRect(668.0f, rowY[0] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF, 0);
		}

		float offsetY = 0.0f;
		float arrowX = 121.0f;
		float arrowY = rowY[0] + 107.0f;
		for (int i = 0; i < 6; i++)
		{
			if (m_ArrowItems[i].m_show == 1)
			{
				pGDI->DrawTexture(m_images[51],
					WRect(arrowX + x, arrowY + offsetY, 10.0f, 10.0f),
					0xFFFFFFFF, 0);
			}
			else
			{
				pGDI->DrawTexture(m_images[52],
					WRect(arrowX + x, arrowY + offsetY, 10.0f, 10.0f),
					0xFFFFFFFF, 0);
			}

			if (i == 1)
			{
				offsetY = -22.0f;
				arrowX += 89.0f;
			}
			else if (i == 2)
			{
				offsetY = 22.0f;
			}
			else
			{
				offsetY = 0.0f;
				arrowX += 89.0f;
			}
		}
		break;
	}
	case 1:
	{
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[1], 30.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[1], 526.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[1], 30.0f, 196.0f),
			0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[59],
			WRect(200.0f, rowY[1] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		if (!IsLocalContent((localContentType_t)0x46))
		{
			pGDI->DrawTexture(m_images[63],
				WRect(550.0f, rowY[1] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

			pGDI->DrawTexture(m_images[48],
				WRect(x + 234.0f, rowY[1] + 67.0f, 123.0f, 80.0f), 0xFFFFFFFF,
				0);
			return;
		}

		pGDI->DrawTexture(m_images[62],
			WRect(550.0f, rowY[1] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		const Bitmap* course;
		if (Doc()->m_tutorialComplete[0] == 0xFF)
			course = m_images[44];
		else
			course = m_images[43];
		pGDI->DrawTexture(course,
			WRect(x + 40.0f, rowY[1] + 91.0f, 85.0f, 48.0f), 0xFFFFFFFF, 0);

		WPoint pt;
		pt.x = g_input->GetMousePoint().x - x;
		pt.y = g_input->GetMousePoint().y - rowY[1];

		int hit = IsInArea(item->idx, pt);
		for (int i = 0; i < 6; i++)
		{
			int index = i + 8;
			if (m_Tutorials[index].m_pending == 0)
			{
				const Bitmap* img;
				if (m_Tutorials[index].m_status == 2)
				{
					img = m_images[42];
				}
				else
				{
					img = m_images[index];
				}

				if (hit != -1 && hit == i && m_Tutorials[index].m_status != 2)
				{
					pGDI->DrawTexture(m_images[i + 22],
						WRect(x + m_underBarBtnPos[i].x,
							rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f),
						0xFFFFFFFF, 0);
				}
				else
				{
					pGDI->DrawTexture(img,
						WRect(x + m_underBarBtnPos[i].x,
							rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f),
						0xFFFFFFFF, 0);
				}
			}
			else if (m_pendingTutorial != 0xFF)
			{
				pGDI->DrawTexture(m_images[i + 36],
					WRect(x + m_underBarBtnPos[i].x,
						rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f),
					0xFFFFFFFF, 0);
			}

			if (m_Tutorials[index].m_status == 1)
			{
				pGDI->DrawTexture(m_images[49],
					WRect(x + m_underBarBtnPos[i].x + 60.0f,
						rowY[1] + m_underBarBtnPos[i].y + 22.0f, 19.0f, 19.0f),
					0xFFFFFFFF, 0);
			}
		}

		if (m_CompleteItems[1].m_count == 3)
		{
			pGDI->DrawTexture(m_images[53],
				WRect(668.0f, rowY[1] + 60.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
			pGDI->DrawTexture(m_images[54],
				WRect(668.0f, rowY[1] + m_iconOffset + 58.0f, 79.0f, 97.0f),
				0xFFFFFFFF, 0);
		}
		else
		{
			int mode = Doc()->m_myInfo.info.gender % 2;

			if (mode == 0)
			{
				pGDI->DrawTexture(m_images[56],
					WRect(668.0f, rowY[1] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF,
					0);
			}
			else
			{
				pGDI->DrawTexture(m_images[57],
					WRect(668.0f, rowY[1] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF,
					0);
			}
		}

		float arrowX = 211.0f;
		float arrowY = rowY[1] + 108.0f;
		for (int i = 6; i < 10; i++)
		{
			if (m_ArrowItems[i].m_show == 1)
			{
				pGDI->DrawTexture(m_images[51],
					WRect(arrowX + x, arrowY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}
			else
			{
				pGDI->DrawTexture(m_images[52],
					WRect(arrowX + x, arrowY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}

			arrowX += 89.0f;
		}
		break;
	}
	case 2:
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[2], 30.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[2], 526.0f, 196.0f),
			0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[2], 30.0f, 196.0f),
			0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[60],
			WRect(200.0f, rowY[2] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[63],
			WRect(550.0f, rowY[2] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[48],
			WRect(x + 234.0f, rowY[2] + 67.0f, 123.0f, 80.0f), 0xFFFFFFFF, 0);
		break;
	}

	pGDI->DrawTexture(m_images[50], WRect(670.0f, 63.0f, 18.0f, 19.0f),
		0xFFFFFFFF, 0);
}

void CTutorialMain::DrawCourseArrow(void)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = m_ArrowItems[i].m_first; j <= m_ArrowItems[i].m_last; j++)
		{
			int first = GetGradeBegin_Grade(m_Tutorials[j].m_category);
			if (m_Tutorials[j].m_status == 1 || m_Tutorials[j].m_status == 0)
			{
				m_ArrowItems[first + i].m_show = 0;
			}
			else if (m_Tutorials[j].m_status == 2)
			{
				m_ArrowItems[first + i].m_show = 1;
			}
		}
	}
}

int CTutorialMain::GetSelectedBtn(int type, WPoint pt)
{
	return IsInArea(type, pt);
}

int CTutorialMain::IsInArea(int type, WPoint pt)
{
	WPoint* pos;
	unsigned char count = 0;

	switch (type)
	{
	case 0:
		pos = m_overBarBtnPos;
		count = 8;
		break;
	case 1:
		pos = m_underBarBtnPos;
		count = 6;
		break;
	}

	for (unsigned char i = 0; i < count; i++)
	{
		if (pt.x > pos[i].x && pos[i].x + 85.0f > pt.x && pt.y > pos[i].y &&
			pos[i].y + 48.0f > pt.y)
			return i;
	}

	return -1;
}

void CTutorialMain::OnTutorialListLBtnDown(void)
{
	if (m_tutorialListBox == 0)
		return;

	FrListItem* item = m_tutorialListBox->GetItemUnderCursor();
	if (item == 0)
		return;

	if (!IsLocalContent((localContentType_t)0x46))
	{
		if (item->idx >= 1)
			return;
	}

	float rowY[3] = { 0.0f };
	int topRow = m_tutorialListBox->GetScrollBar()->GetCurTopRow_Int();
	if (topRow == 0)
	{
		rowY[0] = 119.0f;
		rowY[1] = 314.0f;
	}
	else if (topRow == 1)
	{
		rowY[1] = 119.0f;
		rowY[2] = 314.0f;
	}

	WPoint pt;
	pt.x = g_input->GetMousePoint().x - item->pos.x;
	pt.y = g_input->GetMousePoint().y - rowY[item->idx];

	int btn = IsInArea(item->idx, pt);
	if (btn == -1)
		return;

	switch (item->idx)
	{
	case 0:
		break;
	case 1:
		btn += 8;
		break;
	}

	if (m_Tutorials[btn].m_status != 2)
	{
		m_Tutorials[btn].m_pending = 1;
		m_pendingTutorial = btn;
	}

	OpenQuest(m_Tutorials[btn].m_category, btn);
}

void CTutorialMain::OnTutorialListLBtnUp(void)
{
	if (m_tutorialListBox == 0)
		return;

	FrListItem* item = m_tutorialListBox->GetItemUnderCursor();
	if (item == 0)
		return;

	int end = GetAccumulateLimit(item->idx);
	for (int i = GetGradeBegin_Grade(item->idx); i < end; i++)
	{
		if (m_Tutorials[i].m_pending == 1)
		{
			m_Tutorials[i].m_pending = 0;
			break;
		}
	}
}

void CTutorialMain::SetQuestInformation(sTutorialMap* map)
{
	m_questState = map->m_state;
	m_questCategory = map->m_category;
	m_questNextIndex = map->m_index;
	m_questIndex = map->m_quest;
}

void CTutorialMain::SetQuestInformation(int category, int index)
{
	m_questCategory = category;
	m_questState = 0;
	m_questNextIndex = index + 1;
	m_questIndex = index;
}

void CTutorialMain::OpenQuest(int category, int index)
{
	if (!m_Tutorials[index].m_enabled)
		return;

	if (m_Tutorials[index].m_status == 2)
	{
		m_questionForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		m_questionForm->SetMessage(
			"\xc0\xcc\xc0\xfc \xb5\xee\xb1\xde\xc0\xbb \xb8\xb6\xc4\xa3 \xc8\xc4 \xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		m_questionForm->Open((FRESH_PFN_RESULT)0,
			WPoint((g_view->GetWidth() - m_questionForm->GetRect().w) * 0.5f,
				80.0f),
			3);
		m_questionForm->EnableDrag(false);

		m_Tutorials[index].m_pending = 0;
		m_pendingTutorial = 0xFF;
		return;
	}

	SetQuestInformation(category, index);
	if (IsCompleteQuest(category, index))
	{
		m_questReward = false;

		m_questionForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);

		m_questionForm->SetMessage(
			"\xc0\xcc\xb9\xcc \xbf\xcf\xb7\xe1\xb5\xc8 \xc4\xf9\xbd\xba\xc6\xae\xc0\xcc\xb9\xc7\xb7\xce, \xc5\xac\xb8\xae\xbe\xee \xb1\xe2\xb3\xe4 \xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \n\xb1\xd7\xb7\xa1\xb5\xb5 \xb4\xd9\xbd\xc3 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		m_questionForm->EnableDrag(false);
		m_questionForm->Open(
			(FRESH_PFN_RESULT)&CTutorialMain::OnQuestionFormResult,
			WPoint((g_view->GetWidth() - m_questionForm->GetRect().w) * 0.5f,
				80.0f),
			3);
		m_questionForm->MoveCursor("ok");
	}
	else
	{
		m_questReward = true;
		OpenTutorialInfoDlg();
	}
}

void CTutorialMain::OpenQuestGift(void)
{
	m_questGift =
		CreateForm<FrQuestGift>(g_pFresh->GetManager(), this, "quest_gift", 0);
	m_questGift->SetQuestExplane(m_questIndex);
	m_questGift->Open((FRESH_PFN_RESULT)&CTutorialMain::OnGiftDlgResult,
		WPoint((g_view->GetWidth() - m_questGift->GetRect().w) * 0.5f, 80.0f),
		2);
}

void CTutorialMain::OpenGradeGift(unsigned char grade)
{
	m_questGift =
		CreateForm<FrQuestGift>(g_pFresh->GetManager(), this, "quest_gift", 0);
	m_questGift->SetData(grade);
	m_questGift->Open((FRESH_PFN_RESULT)0,
		WPoint((g_view->GetWidth() - m_questGift->GetRect().w + 10.0f) * 0.5f,
			80.0f),
		2);

	m_gradeGift = 0;
}

void CTutorialMain::DecideNextMotion(unsigned char grade)
{
	switch (grade)
	{
	case 0:
		m_pipinTime.m_nextMotion = "\xb1\xe2\xba\xbb";
		break;
	case 1:
		m_pipinTime.m_nextMotion = "\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2";
		break;
	case 2:
		m_pipinTime.m_nextMotion = "\xc2\xf9\xbd\xba";
		break;
	case 3:
		m_pipinTime.m_nextMotion = "\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2";
		break;
	case 4:
		m_pipinTime.m_nextMotion = "\xb1\xde\xc1\xa4\xb0\xc5";
		break;
	case 5:
		m_pipinTime.m_nextMotion = "\xc8\xa6\xc0\xce";
		break;
	case 6:
		m_pipinTime.m_nextMotion = "\xc8\xa6\xc0\xce\xbf\xf8";
		break;
	}
}

void CTutorialMain::MissionComplete(unsigned long flags)
{
	for (int i = 0; i < 8; i++)
	{
		if (data_9f7be8[i].m_mask & flags)
			MissionClear(i, flags);

		if (i == 7 && (flags & 0x80))
			m_categoryClear[0] = true;
	}
}

void CTutorialMain::MissionClear(int& index, unsigned long& flags)
{
	sTutorialInfo* info = FindTutorialList(index);
	if (info)
	{
		if (flags & data_9f7be8[index].m_mask)
		{
			info->m_cleared = true;
			CheckEnable(index);
		}

		if (index == 7 && (flags & 0x80))
			m_categoryClear[0] = true;
	}
}

void CTutorialMain::OpenTutorialInfoDlg(void)
{
	if (m_tutorialInfoDlg == 0)
	{
		m_tutorialInfoDlg = CreateForm<FrQuestInfoDlg>(g_pFresh->GetManager(),
			this, "quest_info", 0);
		m_tutorialInfoDlg->SetQuestExplane(m_questIndex);
		m_tutorialInfoDlg->Open(
			(FRESH_PFN_RESULT)&CTutorialMain::OnTutorialInfoDlgResult, 3);
	}
}

bool CTutorialMain::OnTutorialInfoDlgResult(int result, FrForm* form)
{
	if (result == 0)
		return true;

	m_tutorialInfoDlg = 0;
	if (result == 1)
		StartSelectedTutorial();

	if (m_pendingTutorial != 0xFF)
	{
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}

	return true;
}

bool CTutorialMain::OnGiftDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (m_gradeGift)
			OpenGradeGift(m_gradeGift);
	}
	return true;
}

void CTutorialMain::StartSelectedTutorial(void)
{
	bool valid = true;
	if (m_questState < 0 || (unsigned int)m_questState > 3)
		valid = false;

	if (m_questCategory < 0 || m_questNextIndex < 0 || m_questIndex < 0)
		valid = false;

	if (!valid)
	{
		HandleMsg(MsgObject(0, 0x23,
			(intptr_t)"\xbf\xac\xbd\xc0\xb8\xf0\xb5\xe5 \xbc\xb3\xc1\xa4\xc0\xcc \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		return;
	}

	if (m_questState > 0)
	{
		HandleMsg(
			MsgObject(0, 0x23, (intptr_t)"Not supported yet.", 0, 0, 0, 0));
		return;
	}

	if (m_pet)
		HidePet();

	switch (m_questState)
	{
	case 0:
		InitGolfTutorial();
		WSingleton<CMouseCursor>::Instance()->SetMode(1);
		WSingleton<CMouseCursor>::Instance()->SetActive(false);
		break;
	}

	WSingleton<CTaskManager>::Instance()->ChangeTask(
		s_tutorialTasks[m_questState], s_tutorialTaskParams[m_questState],
		true);

	int length = strlen(s_tutorialMsgParams[m_questState]);
	if (length != 0)
		WSingleton<CTaskManager>::Instance()->PostMsg(0,
			s_tutorialMsgTargets[m_questState], 0,
			(intptr_t)s_tutorialMsgParams[m_questState], 0, 0, 0);
}

void CTutorialMain::InitGolfTutorial(void)
{
	Doc()->m_tutorialStartMode = m_questNextIndex;
	Doc()->m_gameMode = 0;

	SetCurMap(0xFD);
	SetGameType(12);

	for (unsigned char i = 0; i < 18; i++)
		Doc()->m_holeOrder[i] = 0;

	SetShotTimeLimit(0);

	SetHoles(1);
	Doc()->m_holeOrder[0] = m_questNextIndex < 14 ? 1 : 2;

	SetWeather(0);

	for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
		it != Doc()->m_slotList.end(); ++it)
		(*it).bSleep = 0;

	Doc()->m_bGameOver = false;
}

CTutorialMain::sTutorialInfo* CTutorialMain::FindTutorialList(int& id)
{
	sTutorialInfo* found = 0;
	for (std::list<sTutorialInfo>::iterator it = m_tutorialList.begin();
		it != m_tutorialList.end(); ++it)
	{
		sTutorialInfo* info = &(*it);
		if (info->m_id == id)
		{
			found = info;
			break;
		}
	}
	return found;
}

void CTutorialMain::CheckEnable(int index)
{
	int unlock = m_TutoCondition[index].m_unlock;
	if (unlock == 0)
	{
		for (int i = 0; i < m_TutoCondition[index].m_count; i++)
		{
			int id = m_TutoCondition[index].m_start + i;
			sTutorialInfo* info = FindTutorialList(id);
			if (info == 0)
				return;

			info->m_enabled = true;
		}
	}
	else
	{
		int cleared = 0;
		for (int i = 0; i < m_TutoCondition[index].m_count; i++)
		{
			int id = m_TutoCondition[index].m_start + i;
			sTutorialInfo* info = FindTutorialList(id);
			if (info == 0)
				return;

			if (info->m_cleared == true)
				cleared++;
		}

		if (cleared == m_TutoCondition[index].m_count)
		{
			sTutorialInfo* info = FindTutorialList(unlock);
			if (info == 0)
				return;

			info->m_enabled = true;
		}
	}
}

bool CTutorialMain::IsCompleteQuest(int category, int index)
{
	bool complete = false;
	if (m_Tutorials[index].m_category == category)
	{
		if (Doc()->m_tutorialComplete[category] & data_9f7be8[index].m_mask)
			complete = true;
	}
	return complete;
}

bool CTutorialMain::IsCompleteCategory(int category)
{
	int start;
	int end;

	switch (category)
	{
	case 0:
		start = 0;
		end = 8;
		break;
	case 1:
		start = 8;
		end = 14;
		break;
	case 2:
		start = 14;
		end = 20;
		break;
	}

	bool complete = true;
	for (int i = start; i < end; ++i)
	{
		if ((Doc()->m_tutorialComplete[category] & data_9f7be8[i].m_mask) == 0)
		{
			complete = false;
			break;
		}
	}

	return complete;
}

bool CTutorialMain::OnQuestionFormResult(int result, FrForm* form)
{
	if (result == 0)
		return true;

	m_questionForm = 0;
	if (result == 1)
		OpenTutorialInfoDlg();

	if (m_pendingTutorial != 0xFF)
	{
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}

	return true;
}
