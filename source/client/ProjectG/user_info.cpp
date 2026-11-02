#include "s5/utilities.h"
#include "minatl.h"
#include "userinfoform.h"
#include "user_info.h"
#include "matchhistory.h"
#include "addfrienddlg.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "netresourcemanager.h"
#include "buddymanager.h"
#include "lobbytask.h"
#include "avatar_task.h"
#include "partsinfo.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;
extern WResourceManager* g_resrcmng;

int float2int(float value);
void RemakeNumber(int value, char* text);

IMPLEMENT_OBJECT(FrUserInfoForm, FrForm)

BEGIN_FRESH_MSGMAP(FrUserInfoForm, FrForm)
ON_FRESH_VI("friend", FRCMD_INIT, FrUserInfoForm::OnFriendInit)
ON_FRESH_VV("friend", FRCMD_LBUTTONUP, FrUserInfoForm::OnFriendUp)
ON_FRESH_VI("whisper", FRCMD_INIT, FrUserInfoForm::OnWhisperInit)
ON_FRESH_VV("whisper", FRCMD_LBUTTONUP, FrUserInfoForm::OnWhisperUp)
ON_FRESH_VI("ignore", FRCMD_INIT, FrUserInfoForm::OnIgnoreInit)
ON_FRESH_VV("ignore", FRCMD_LBUTTONUP, FrUserInfoForm::OnIgnoreUp)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrUserInfoForm::OnCloseUp)
ON_FRESH_VI("ghost", FRCMD_INIT, FrUserInfoForm::OnGhostInit)
ON_FRESH_VV("ghost", FRCMD_LBUTTONUP, FrUserInfoForm::OnGhostdUp)
ON_FRESH_VI("pet", FRCMD_INIT, FrUserInfoForm::OnPetInit)
ON_FRESH_VV("pet", FRCMD_OWNERDRAW, FrUserInfoForm::OnPetOwnerDraw)
ON_FRESH_VI("pet_load", FRCMD_INIT, FrUserInfoForm::OnPetLoadInit)
ON_FRESH_VI("nick", FRCMD_INIT, FrUserInfoForm::OnNickInit)
ON_FRESH_VI("nick", FRCMD_OWNERDRAW, FrUserInfoForm::OnNickOwnerDraw)
ON_FRESH_VI("general", FRCMD_INIT, FrUserInfoForm::OnGeneralInfoInit)
ON_FRESH_VV("general", FRCMD_LBUTTONDOWN, FrUserInfoForm::OnGeneralInfoDown)
ON_FRESH_VI("special", FRCMD_INIT, FrUserInfoForm::OnSpecialInfoInit)
ON_FRESH_VV("special", FRCMD_LBUTTONDOWN, FrUserInfoForm::OnSpecialInfoDown)
ON_FRESH_VI("record", FRCMD_INIT, FrUserInfoForm::OnRecordInfoInit)
ON_FRESH_VV("record", FRCMD_LBUTTONDOWN, FrUserInfoForm::OnRecordInfoDown)
ON_FRESH_VI("trophy", FRCMD_INIT, FrUserInfoForm::OnTrophyInfoInit)
ON_FRESH_VV("trophy", FRCMD_LBUTTONDOWN, FrUserInfoForm::OnTrophyInfoDown)
ON_FRESH_VI("guild", FRCMD_INIT, FrUserInfoForm::OnGuildInfoInit)
ON_FRESH_VV("guild", FRCMD_LBUTTONDOWN, FrUserInfoForm::OnGuildInfoDown)
ON_FRESH_VI("general_back", FRCMD_INIT, FrUserInfoForm::OnGeneralBackInit)
ON_FRESH_VI("rec_course_back", FRCMD_INIT, FrUserInfoForm::OnRecCourseBackInit)
ON_FRESH_VI("tro_normal_back", FRCMD_INIT, FrUserInfoForm::OnTroNormalBackInit)
ON_FRESH_VI("tro_special_back", FRCMD_INIT,
	FrUserInfoForm::OnTroSpecialBackInit)
ON_FRESH_VI("guild_back", FRCMD_INIT, FrUserInfoForm::OnGuildBackInit)
ON_FRESH_VI("btn_season", FRCMD_INIT, FrUserInfoForm::OnTitleSeasonBtnInit)
ON_FRESH_VV("btn_season", FRCMD_LBUTTONUP, FrUserInfoForm::OnTitleSeasonBtnUp)
ON_FRESH_VI("btn_total", FRCMD_INIT, FrUserInfoForm::OnTitleTotalBtnInit)
ON_FRESH_VV("btn_total", FRCMD_LBUTTONUP, FrUserInfoForm::OnTitleTotalBtnUp)
ON_FRESH_VI("eg0", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_ID)
ON_FRESH_VI("eg1", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_SCHOOL)
ON_FRESH_VI("eg2", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_CLUB)
ON_FRESH_VI("eg3", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_EXP)
ON_FRESH_VI("eg4", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_CONNSTATUS)
ON_FRESH_VI("eg5", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_DISCONN)
ON_FRESH_VI("eg6", FRCMD_INIT, FrUserInfoForm::OnEditInit_GI_COMBO)
ON_FRESH_VI("general_level", FRCMD_INIT, FrUserInfoForm::OnAreaInit_GI_Level)
ON_FRESH_VI("general_guild", FRCMD_INIT, FrUserInfoForm::OnAreaInit_GI_ClUB)
ON_FRESH_VI("general_guild", FRCMD_OWNERDRAW,
	FrUserInfoForm::OnAreaDraw_GI_ClUB)
ON_FRESH_VI("general_question", FRCMD_INIT,
	FrUserInfoForm::OnAreaInit_Question_mark)
ON_FRESH_VI("Btn_rmrvisitor", FRCMD_INIT, FrUserInfoForm::OnBtn_RmrVisitorInit)
ON_FRESH_VV("Btn_rmrvisitor", FRCMD_LBUTTONUP,
	FrUserInfoForm::OnBtn_RmrVisitorBtnUp)
ON_FRESH_VI("speclist", FRCMD_INIT, FrUserInfoForm::OnSpecListboxInit)
ON_FRESH_VI("speclist", FRCMD_OWNERDRAW, FrUserInfoForm::OnSpecListboxOwnerDraw)
ON_FRESH_VI("es0_1", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_TROPHY_GOLD)
ON_FRESH_VI("es0_2", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_TROPHY_SILVER)
ON_FRESH_VI("es0_3", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_TROPHY_BRONZE)
ON_FRESH_VI("es1", FRCMD_INIT,
	FrUserInfoForm::OnEditInit_SI_MATCH_WIN_PERCENTAGE)
ON_FRESH_VI("es2", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_AVG_SCORE)
ON_FRESH_VI("es3", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MEAN_SHOTTIME)
ON_FRESH_VI("es4", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_PANGYA_PERCENTAGE)
ON_FRESH_VI("es5", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_FAIRWAY_PERCENTAGE)
ON_FRESH_VI("es6", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_PUTT_PERCENTAGE)
ON_FRESH_VI("es7", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_OB_PERCENTAGE)
ON_FRESH_VI("es8", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_TOTAL_SKINS_PANG)
ON_FRESH_VI("mh1", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY1)
ON_FRESH_VI("mh2", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY2)
ON_FRESH_VI("mh3", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY3)
ON_FRESH_VI("mh4", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY4)
ON_FRESH_VI("mh5", FRCMD_INIT, FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY5)
ON_FRESH_VI("btn_si1", FRCMD_INIT, FrUserInfoForm::OnMatchHistoryBtn1Init)
ON_FRESH_VV("btn_si1", FRCMD_LBUTTONUP, FrUserInfoForm::OnMatchHistoryBtn1Up)
ON_FRESH_VI("btn_si2", FRCMD_INIT, FrUserInfoForm::OnMatchHistoryBtn2Init)
ON_FRESH_VV("btn_si2", FRCMD_LBUTTONUP, FrUserInfoForm::OnMatchHistoryBtn2Up)
ON_FRESH_VI("btn_si3", FRCMD_INIT, FrUserInfoForm::OnMatchHistoryBtn3Init)
ON_FRESH_VV("btn_si3", FRCMD_LBUTTONUP, FrUserInfoForm::OnMatchHistoryBtn3Up)
ON_FRESH_VI("btn_si4", FRCMD_INIT, FrUserInfoForm::OnMatchHistoryBtn4Init)
ON_FRESH_VV("btn_si4", FRCMD_LBUTTONUP, FrUserInfoForm::OnMatchHistoryBtn4Up)
ON_FRESH_VI("btn_si5", FRCMD_INIT, FrUserInfoForm::OnMatchHistoryBtn5Init)
ON_FRESH_VV("btn_si5", FRCMD_LBUTTONUP, FrUserInfoForm::OnMatchHistoryBtn5Up)
ON_FRESH_VI("recordlist", FRCMD_INIT, FrUserInfoForm::OnRecTotalListboxInit)
ON_FRESH_VI("recordlist", FRCMD_OWNERDRAW,
	FrUserInfoForm::OnRecTotalListboxOwnerDraw)
ON_FRESH_VI("btn_rec0", FRCMD_INIT, FrUserInfoForm::OnRecordNormalBtnInit)
ON_FRESH_VV("btn_rec0", FRCMD_LBUTTONUP, FrUserInfoForm::OnRecordNormalBtnUp)
ON_FRESH_VI("btn_rec1", FRCMD_INIT, FrUserInfoForm::OnRecordCourseBtnInit)
ON_FRESH_VV("btn_rec1", FRCMD_LBUTTONUP, FrUserInfoForm::OnRecordCourseBtnUp)
ON_FRESH_VI("btn_rec2", FRCMD_INIT, FrUserInfoForm::OnRecordClassicBtnInit)
ON_FRESH_VV("btn_rec2", FRCMD_LBUTTONUP, FrUserInfoForm::OnRecordClassicBtnUp)
ON_FRESH_VI("er0", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_PLAYTIME)
ON_FRESH_VI("er0", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er0", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er1", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_SHOTS)
ON_FRESH_VI("er1", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er1", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er2", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_HOLES)
ON_FRESH_VI("er2", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er2", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er3_0", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_EARTH_ROUND)
ON_FRESH_VI("er3_0", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er3_0", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er3_1", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_DRIVE)
ON_FRESH_VI("er3_1", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er3_1", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er4", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_BEST_SCORE_3)
ON_FRESH_VI("er4", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er4", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er5", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_BEST_SCORE_2)
ON_FRESH_VI("er6", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_BEST_SCORE_1)
ON_FRESH_VI("er7", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_HOLEINONES)
ON_FRESH_VI("er7", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er7", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er8", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_TOT_ALBATROSS)
ON_FRESH_VI("er8", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er8", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er9", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_LONGEST_DRIVE)
ON_FRESH_VI("er9", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er9", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er10", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_LONGEST_PUTT)
ON_FRESH_VI("er10", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er10", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er11", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_LONGEST_CHIPIN)
ON_FRESH_VI("er11", FRCMD_HOVERON, FrUserInfoForm::OnEditInit_HoverOn)
ON_FRESH_VI("er11", FRCMD_HOVEROFF, FrUserInfoForm::OnEditInit_HoverOff)
ON_FRESH_VI("er12", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_BEST_SCORE_5)
ON_FRESH_VI("er13", FRCMD_INIT, FrUserInfoForm::OnEditInit_RI_BEST_SCORE_4)
ON_FRESH_VI("scorelist", FRCMD_INIT, FrUserInfoForm::OnScoreListInit)
ON_FRESH_VI("scorelist", FRCMD_OWNERDRAW, FrUserInfoForm::OnScoreListOwnerDraw)
ON_FRESH_VI("tri_normalnum", FRCMD_INIT, FrUserInfoForm::OnTrophyNormalNumInit)
ON_FRESH_VI("tri_specialnum", FRCMD_INIT,
	FrUserInfoForm::OnTrophySpecialNumInit)
ON_FRESH_VI("btn_normal", FRCMD_INIT, FrUserInfoForm::OnTrophyNormalBtnInit)
ON_FRESH_VV("btn_normal", FRCMD_LBUTTONUP, FrUserInfoForm::OnTrophyNormalBtnUp)
ON_FRESH_VI("btn_special", FRCMD_INIT, FrUserInfoForm::OnTrophySpecialBtnInit)
ON_FRESH_VV("btn_special", FRCMD_LBUTTONUP,
	FrUserInfoForm::OnTrophySpecialBtnUp)
ON_FRESH_VI("trispecialbox", FRCMD_INIT,
	FrUserInfoForm::OnListBoxInit_TRI_SPECIALBOX)
ON_FRESH_VI("trispecialbox", FRCMD_OWNERDRAW,
	FrUserInfoForm::OnListBoxOwnerDraw_TRI_SPECIALBOX)
ON_FRESH_VI("trinormalbox", FRCMD_INIT,
	FrUserInfoForm::OnListBoxInit_TRI_NORMALBOX)
ON_FRESH_VI("trinormalbox", FRCMD_OWNERDRAW,
	FrUserInfoForm::OnListBoxOwnerDraw_TRI_NORMALBOX)
ON_FRESH_VI("trinotrophy", FRCMD_INIT,
	FrUserInfoForm::OnStaticInit_TRI_NOTROPHY)
ON_FRESH_VI("eguild0", FRCMD_INIT, FrUserInfoForm::OnEditInit_GUILD_NAME)
ON_FRESH_VI("eguild1", FRCMD_INIT, FrUserInfoForm::OnEditInit_GUILD_PANG)
ON_FRESH_VI("eguild2", FRCMD_INIT, FrUserInfoForm::OnEditInit_GUILD_MYPERCENT)
ON_FRESH_VI("guildnotrophy", FRCMD_INIT,
	FrUserInfoForm::OnStaticInit_GUILD_NOTROPHY)
ON_FRESH_VI("myguild", FRCMD_INIT, FrUserInfoForm::OnMyGuildInit)
ON_FRESH_VV("myguild", FRCMD_OWNERDRAW, FrUserInfoForm::OnMyGuildOwnerDraw)
ON_FRESH_VI("guildlist", FRCMD_INIT, FrUserInfoForm::OnGuildTrophyListInit)
ON_FRESH_VI("guildlist", FRCMD_OWNERDRAW,
	FrUserInfoForm::OnGuildTrophyListOwnerDraw)
ON_FRESH_VI("comment", FRCMD_INIT, FrUserInfoForm::OnCommentInit)
END_FRESH_MSGMAP()

FrUserInfoForm::FrUserInfoForm()
{
	m_seasonType = SEASON_CURRENT;

	memset(m_pTitleBtn, 0, sizeof(m_pTitleBtn));

	memset(m_pTabBtn, 0, sizeof(m_pTabBtn));
	memset(m_pBackArea, 0, sizeof(m_pBackArea));
	memset(m_pGeneralEdit, 0, sizeof(m_pGeneralEdit));
	memset(m_pSpecialEdit, 0, sizeof(m_pSpecialEdit));

	memset(m_pRecordBtn, 0, sizeof(m_pRecordBtn));

	memset(m_pSpecialEdit, 0, sizeof(m_pSpecialEdit));
	memset(m_pRecordEdit, 0, sizeof(m_pRecordEdit));
	memset(m_pTrophyBtn, 0, sizeof(m_pTrophyBtn));
	memset(m_pGuildEdit, 0, sizeof(m_pGuildEdit));
	memset(m_pRecTotalBack, 0, sizeof(m_pRecTotalBack));
	memset(m_pRecAwardWnd, 0, sizeof(m_pRecAwardWnd));

	m_pRecTotalListBox = NULL;
	m_pLevelArea = NULL;
	m_pClubArea = NULL;
	m_pQuestionMark = NULL;
	m_reserved270 = 0;
	m_pSpecialTrophyBox = NULL;
	m_pNormalTrophyBox = NULL;
	m_pNoTrophy = NULL;
	m_pNormalTrophyNum = NULL;
	m_pSpecialTrophyNum = NULL;
	m_pGuildNoTrophy = NULL;
	m_recordType = 0;
	m_trophyType = 0;
	m_pGuildTrophyList = NULL;

	m_oldSeason.Clear();
	m_totalSeason.Clear();

	memset(&m_userInfo, 0, sizeof(m_userInfo));
	m_userInfo.info.dwGuid = -1;
	m_userInfo.info.dwUID = -1;

	m_pFriendBtn = NULL;
	m_pWhisperBtn = NULL;
	m_pIgnoreBtn = NULL;
	m_pPetArea = NULL;
	m_pExhibition = NULL;
	m_pPetLoadGauge = NULL;
	m_pPartTidList = NULL;
	m_pComment = NULL;
	m_pNickArea = NULL;

	for (int i = 0; i < 2; ++i)
	{
		m_skinScrollType[i] = 0;
		m_skinScroll[i] = 0.0f;
	}

	m_pScoreList = NULL;
	m_pScoreFont = NULL;
	m_pMyGuild = NULL;
	m_reserved2a8 = 0;
	m_pGuildIcon = NULL;
	m_bRefresh = false;
	m_bMsnControl = false;
	m_bControlOff = false;
	m_guildPang = 0;
	m_guildPoint = 0;

	memset(m_position.csChannelName, 0, sizeof(m_position.csChannelName));
	m_position.iChannelUid = 0xff;
	m_position.iRoomIdx = 0xffff;
	m_position.iRoomType = -1;
	m_position.iServerGUID = -1;

	m_bChatMsgOpen = false;

	memset(m_pMatchHistoryBtn, 0, 0x58c);

	m_pSpecListBox = NULL;
	m_bMatchHistoryRequest = false;
	m_bUnknownLocation = false;
	m_bReserved244c = false;
	m_delta = 0.0f;
	m_pGhostBtn = NULL;
	m_pRmrVisitorBtn = NULL;
	m_recvSeason = 0;
	m_requestSeason = 0;

	memset(m_classicMapStatistics, 0, sizeof(m_classicMapStatistics));
	for (int i = 0; i < 20; ++i)
		m_classicMapStatistics[i].cBestScore = 0x7f;
	memset(m_oldClassicMapStatistics, 0, sizeof(m_oldClassicMapStatistics));
	for (int i = 0; i < 20; ++i)
		m_oldClassicMapStatistics[i].cBestScore = 0x7f;
	memset(m_totalClassicMapStatistics, 0, sizeof(m_totalClassicMapStatistics));
	for (int i = 0; i < 20; ++i)
		m_totalClassicMapStatistics[i].cBestScore = 0x7f;
	memset(m_pBestScoreMap, 0, sizeof(m_pBestScoreMap));
	m_pAddFriendDlg = NULL;
}

FrUserInfoForm::~FrUserInfoForm()
{
	Clear();
}

void FrUserInfoForm::Clear()
{
	if (CUserInfo::IsInstantiated())
		CUserInfo::Instance()->m_pDlg = NULL;

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	HidePet(true);

	if (g_resrcmng && m_pScoreFont)
	{
		g_resrcmng->Release(m_pScoreFont);
		m_pScoreFont = NULL;
	}
}

void FrUserInfoForm::OnTitleSeasonBtnInit(int param)
{
	m_pTitleBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_seasonType == SEASON_CURRENT)
		m_pTitleBtn[0]->SetStatus(FrButton::PRESSED);
	else
		m_pTitleBtn[0]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnTitleSeasonBtnUp()
{
	if (!IsRecvSeason(4))
		RequestSeasonData(5);

	m_seasonType = SEASON_CURRENT;
	m_pTitleBtn[0]->SetStatus(FrButton::PRESSED);
	m_pTitleBtn[1]->SetStatus(FrButton::NORMAL);
	SetContent(true);
	SetTrophyStatic();
	SetSpecListBoxItem(SEASON_CURRENT);
}

void FrUserInfoForm::OnTitleTotalBtnInit(int param)
{
	m_pTitleBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);

	if (m_seasonType == SEASON_TOTAL)
		m_pTitleBtn[1]->SetStatus(FrButton::PRESSED);
	else
		m_pTitleBtn[1]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnTitleTotalBtnUp()
{
	if (!IsRecvSeason(2))
		RequestSeasonData(0);

	m_seasonType = SEASON_TOTAL;
	m_pTitleBtn[0]->SetStatus(FrButton::NORMAL);
	m_pTitleBtn[1]->SetStatus(FrButton::PRESSED);

	SetContent(true);
	SetTrophyStatic();

	SetSpecListBoxItem(SEASON_TOTAL);
}

void FrUserInfoForm::SetTrophyStatic()
{
	if (m_trophyType == 1)
	{
		if (m_pNoTrophy)
			m_pNoTrophy->SetVisible(m_tabType == TAB_TROPHY &&
				m_pSpecialTrophyBox->m_itemList.size() == 0);
	}
	else if (m_trophyType == 0)
	{
		if (m_pNoTrophy)
			m_pNoTrophy->SetVisible(m_tabType == TAB_TROPHY &&
				m_pNormalTrophyBox->m_itemList.size() == 0);
	}

	if (m_pGuildNoTrophy)
		m_pGuildNoTrophy->SetVisible(m_tabType == TAB_GUILD &&
			m_pGuildTrophyList->m_itemList.size() == 0);
}

void FrUserInfoForm::OnGeneralInfoInit(int param)
{
	m_pTabBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnGeneralInfoDown()
{
	SetFormA(TAB_GENERAL);
}

void FrUserInfoForm::OnSpecialInfoInit(int param)
{
	m_pTabBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnSpecialInfoDown()
{
	SetFormA(TAB_SPECIAL);
}

void FrUserInfoForm::OnRecordInfoInit(int param)
{
	m_pTabBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnRecordInfoDown()
{
	SetFormA(TAB_RECORD);
}

void FrUserInfoForm::OnTrophyInfoInit(int param)
{
	m_pTabBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnTrophyInfoDown()
{
	SetFormA(TAB_TROPHY);
}

void FrUserInfoForm::OnGuildInfoInit(int param)
{
	m_pTabBtn[4] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnGuildInfoDown()
{
	SetFormA(TAB_GUILD);
}

void FrUserInfoForm::OnGeneralBackInit(int param)
{
	m_pBackArea[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnRecCourseBackInit(int param)
{
	m_pBackArea[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnTroNormalBackInit(int param)
{
	m_pBackArea[4] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnTroSpecialBackInit(int param)
{
	m_pBackArea[5] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnGuildBackInit(int param)
{
	m_pBackArea[6] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

#define ON_EDIT_INIT(name, member) \
	void FrUserInfoForm::OnEditInit_##name(int param) \
	{ \
		member = DYNAMIC_CAST(FrEdit, (FrWnd*)param); \
		member->EnableHover(true, 0.0f); \
	}

ON_EDIT_INIT(GI_ID, m_pGeneralEdit[0])
ON_EDIT_INIT(GI_SCHOOL, m_pGeneralEdit[1])
ON_EDIT_INIT(GI_CLUB, m_pGeneralEdit[2])
ON_EDIT_INIT(GI_EXP, m_pGeneralEdit[3])
ON_EDIT_INIT(GI_CONNSTATUS, m_pGeneralEdit[4])
ON_EDIT_INIT(GI_DISCONN, m_pGeneralEdit[5])
ON_EDIT_INIT(GI_COMBO, m_pGeneralEdit[6])
ON_EDIT_INIT(GI_QUIZLEVEL, m_pGeneralEdit[7])
ON_EDIT_INIT(SI_TROPHY_GOLD, m_pSpecialEdit[0])
ON_EDIT_INIT(SI_TROPHY_SILVER, m_pSpecialEdit[1])
ON_EDIT_INIT(SI_TROPHY_BRONZE, m_pSpecialEdit[2])
ON_EDIT_INIT(SI_MATCH_WIN_PERCENTAGE, m_pSpecialEdit[3])
ON_EDIT_INIT(SI_AVG_SCORE, m_pSpecialEdit[4])
ON_EDIT_INIT(SI_MEAN_SHOTTIME, m_pSpecialEdit[5])
ON_EDIT_INIT(SI_PANGYA_PERCENTAGE, m_pSpecialEdit[6])
ON_EDIT_INIT(SI_FAIRWAY_PERCENTAGE, m_pSpecialEdit[7])
ON_EDIT_INIT(SI_PUTT_PERCENTAGE, m_pSpecialEdit[8])
ON_EDIT_INIT(SI_OB_PERCENTAGE, m_pSpecialEdit[9])
ON_EDIT_INIT(SI_TOTAL_SKINS_PANG, m_pSpecialEdit[10])
ON_EDIT_INIT(RI_TOT_PLAYTIME, m_pRecordEdit[0])
ON_EDIT_INIT(RI_TOT_SHOTS, m_pRecordEdit[1])
ON_EDIT_INIT(RI_TOT_HOLES, m_pRecordEdit[2])
ON_EDIT_INIT(RI_TOT_DRIVE, m_pRecordEdit[4])
ON_EDIT_INIT(RI_EARTH_ROUND, m_pRecordEdit[3])
ON_EDIT_INIT(RI_BEST_SCORE_5, m_pRecordEdit[5])
ON_EDIT_INIT(RI_BEST_SCORE_4, m_pRecordEdit[6])
ON_EDIT_INIT(RI_BEST_SCORE_3, m_pRecordEdit[7])
ON_EDIT_INIT(RI_BEST_SCORE_2, m_pRecordEdit[8])
ON_EDIT_INIT(RI_BEST_SCORE_1, m_pRecordEdit[9])
ON_EDIT_INIT(RI_TOT_HOLEINONES, m_pRecordEdit[10])
ON_EDIT_INIT(RI_TOT_ALBATROSS, m_pRecordEdit[11])
ON_EDIT_INIT(RI_LONGEST_DRIVE, m_pRecordEdit[12])
ON_EDIT_INIT(RI_LONGEST_PUTT, m_pRecordEdit[13])
ON_EDIT_INIT(RI_LONGEST_CHIPIN, m_pRecordEdit[14])
ON_EDIT_INIT(GUILD_NAME, m_pGuildEdit[0])
ON_EDIT_INIT(GUILD_PANG, m_pGuildEdit[1])
ON_EDIT_INIT(GUILD_MYPERCENT, m_pGuildEdit[2])
#undef ON_EDIT_INIT

void FrUserInfoForm::OnAreaInit_GI_Level(int param)
{
	m_pLevelArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pLevelArea)
		m_pLevelArea->SetVisible(false);
}

void FrUserInfoForm::OnAreaInit_GI_ClUB(int param)
{
	m_pClubArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pClubArea)
		m_pClubArea->SetVisible(false);
}

void FrUserInfoForm::OnAreaInit_Question_mark(int param)
{
	m_pQuestionMark = DYNAMIC_CAST(FrArea, (FrWnd*)param);

	if (m_pQuestionMark)
		m_pQuestionMark->SetToolTipText((
			"\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xba \xba\xf1\xb1\xe2\xb3\xca"
			"E\xb0\xa1 \xb5\xc7\xb0\xc5\xb3\xaa \xb0\xd4\xc0\xd3\xc8\xbd\xbc\xf6 50\xc8\xb8\xb8\xa6 "
			"\xc3\xca\xb0\xfa\xc7\xd8\xbe\xdf \xba\xb8\xc0\xcc\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9."));

	const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap("USER_INFO",
		"user_info_question_mark");
	if (bitmap)
	{
		if (m_pQuestionMark)
		{
			m_pQuestionMark->SetBgImg(bitmap);
			m_pQuestionMark->SetVisible(false);
		}
	}
}

void FrUserInfoForm::OnAreaDraw_GI_ClUB(int param)
{
	if (m_userInfo.info.dwGuildId && !(m_userInfo.info.dwIdentity & 0x14))
	{
		const Bitmap* emblem = NetResourceManager::Instance()->GetEmblemByName(
			m_userInfo.info.szEmblemName);
		if (emblem)
		{
			FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
			if (gdi)
			{
				float x = m_pGeneralEdit[2]->GetRect().x +
					m_pGeneralEdit[2]->GetRect().w -
					gdi->GetTextExtend(m_userInfo.info.sGuild) -
					emblem->Width() - 15;
				gdi->DrawTexture(emblem,
					WRect(x, m_pGeneralEdit[2]->GetRect().y + 2,
						(float)emblem->Width(), (float)emblem->Height()),
					0xffffffff, 0);
			}
		}
	}
}

void FrUserInfoForm::OnRecordNormalBtnInit(int param)
{
	m_pRecordBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_recordType == 0)
		m_pRecordBtn[0]->SetStatus(FrButton::PRESSED);
	else
		m_pRecordBtn[0]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnRecordNormalBtnUp()
{
	m_recordType = 0;
	m_pRecordBtn[0]->SetStatus(FrButton::PRESSED);
	m_pRecordBtn[2]->SetStatus(FrButton::NORMAL);
	m_pRecordBtn[3]->SetStatus(FrButton::NORMAL);
	ChangeRecordType();
}

void FrUserInfoForm::OnRecordCourseBtnInit(int param)
{
	m_pRecordBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_recordType == 2)
		m_pRecordBtn[2]->SetStatus(FrButton::PRESSED);
	else
		m_pRecordBtn[2]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnRecordCourseBtnUp()
{
	m_recordType = 2;
	m_pRecordBtn[2]->SetStatus(FrButton::PRESSED);
	m_pRecordBtn[0]->SetStatus(FrButton::NORMAL);
	m_pRecordBtn[3]->SetStatus(FrButton::NORMAL);
	ChangeRecordType();
	UpdateEditMapStatistics(m_seasonType, false);
}

void FrUserInfoForm::OnRecordClassicBtnInit(int param)
{
	m_pRecordBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_recordType == 3)
		m_pRecordBtn[3]->SetStatus(FrButton::PRESSED);
	else
		m_pRecordBtn[3]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnRecordClassicBtnUp()
{
	m_recordType = 3;
	m_pRecordBtn[3]->SetStatus(FrButton::PRESSED);
	m_pRecordBtn[2]->SetStatus(FrButton::NORMAL);
	m_pRecordBtn[0]->SetStatus(FrButton::NORMAL);
	ChangeRecordType();
	UpdateEditMapStatistics(m_seasonType, true);
}

#define MATCH_HISTORY_EDIT_INIT(n, i) \
	void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY##n(int param) \
	{ \
		m_pSpecialEdit[i] = DYNAMIC_CAST(FrEdit, (FrWnd*)param); \
		m_pSpecialEdit[i]->EnableHover(true, 0.f); \
	}

MATCH_HISTORY_EDIT_INIT(1, 11)
MATCH_HISTORY_EDIT_INIT(2, 12)
MATCH_HISTORY_EDIT_INIT(3, 13)
MATCH_HISTORY_EDIT_INIT(4, 14)
MATCH_HISTORY_EDIT_INIT(5, 15)
#undef MATCH_HISTORY_EDIT_INIT

void FrUserInfoForm::OnSpecListboxInit(int param)
{
	m_pSpecListBox = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pSpecListBox->SetVisible(false);

	m_pSpecialBack = g_pFresh->RegisterBitmap("special_back");
	m_pSpecialBack00 = g_pFresh->RegisterBitmap("special_back00");

	m_pSpecListBox->AddItem((void*)m_pSpecialBack);
	m_pSpecListBox->AddItem((void*)m_pSpecialBack00);
}

#define MATCH_HISTORY_BTN_INIT(n, i) \
	void FrUserInfoForm::OnMatchHistoryBtn##n##Init(int param) \
	{ \
		m_pMatchHistoryBtn[i] = DYNAMIC_CAST(FrButton, (FrWnd*)param); \
	}

MATCH_HISTORY_BTN_INIT(1, 0)
MATCH_HISTORY_BTN_INIT(2, 1)
MATCH_HISTORY_BTN_INIT(3, 2)
MATCH_HISTORY_BTN_INIT(4, 3)
MATCH_HISTORY_BTN_INIT(5, 4)
#undef MATCH_HISTORY_BTN_INIT

#define MATCH_HISTORY_BTN_UP(n, i) \
	void FrUserInfoForm::OnMatchHistoryBtn##n##Up() \
	{ \
		MatchHistoryUserBtnUp(i); \
	}

MATCH_HISTORY_BTN_UP(1, 0)
MATCH_HISTORY_BTN_UP(2, 1)
MATCH_HISTORY_BTN_UP(3, 2)
MATCH_HISTORY_BTN_UP(4, 3)
MATCH_HISTORY_BTN_UP(5, 4)
#undef MATCH_HISTORY_BTN_UP

void FrUserInfoForm::MatchHistoryUserBtnUp(int index)
{
	WSendPacket packet((enumClientPacket)0x2f);
	packet.Encode4(m_matchHistory[index].dwUID);
	packet.Encode1(5);
	packet.Send((eSendTo)0);

	m_bMatchHistoryRequest = true;
	m_pMatchHistoryBtn[index]->SetStatus(FrButton::NORMAL);
	CUserInfo::Instance()->m_nickname = m_matchHistory[index].szNickName;
}

void FrUserInfoForm::MsnDisconExeception(float delta, FrListItem* item)
{
	if (m_bMatchHistoryRequest)
	{
		static int s_count = 0;
		static float s_time = 0;

		s_time += delta;
		if (s_time < 0.7f)
		{
			FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
			if (!gdi || !item)
				return;

			gdi->SetTextColor(0xffc0125c, 0xffffffff);
			gdi->SetTextStyle(2);

			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 37, item->pos.y + 212), 0,
				"\xbc\xad\xb9\xf6\xc0\xc7 \xc0\xdb\xbe\xf7\xb7\xae\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.",
				-1.f, 0xffffffff);
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 37, item->pos.y + 228), 0,
				"\xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.",
				-1.f, 0xffffffff);
		}

		if (s_time > 1.f)
		{
			s_time = 0;
			++s_count;
			if (s_count > 3)
			{
				s_count = 0;
				m_bMatchHistoryRequest = false;
			}
		}
	}
}

void FrUserInfoForm::SetMatchHistoryContents()
{
	char text[128] = { 0 };

	if (CMatchHistory::Instance()->IsUpdated() == true)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (m_matchHistory[i].dwUID)
				m_pSpecialEdit[11 + i]->SetLine(1, m_matchHistory[i].szNickName,
					0xff000000, false, 0);
			else
				m_pSpecialEdit[11 + i]->SetLine(1,
					"\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd", 0xff000000, false, 0);
		}
	}
	else
	{
		const char* msg = "\xc8\xae\xc0\xce \xba\xd2\xb0\xa1.";
		strcpy(text, msg);
		for (int i = 0; i < 5; ++i)
			m_pSpecialEdit[11 + i]->SetLine(1, text, 0xff000000, false, 0);
	}
}

void FrUserInfoForm::SetSpecListBoxItem(eSeasonType season)
{
	if (!strcmp(m_userInfo.info.sNick, Doc()->m_myInfo.info.sNick))
	{
		if (m_pTabBtn[2]->GetStatus() == FrButton::PRESSED)
		{
			if (m_pRecordBtn[3])
				m_pRecordBtn[3]->SetVisible(true);
		}
	}
}

void FrUserInfoForm::OnSpecListboxOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* item = (FrListItem*)param;
	if (!gdi || !item)
		return;

	WRect dest;
	WRect rect;
	dest.x = item->pos.x;
	dest.y = item->pos.y;

	if (item->pData == m_pSpecialBack)
	{
		dest.w = (float)m_pSpecialBack->Width();
		dest.h = (float)m_pSpecialBack->Height();
		gdi->DrawTexture(m_pSpecialBack, dest, 0xffffffff, 0);

		int i;
		for (i = 0; i < 3; ++i)
		{
			rect = WRect(dest.x + i * 60 + 82, dest.y + 17, 40.f, 20.f);
			m_pSpecialEdit[i]->SetRect(rect);
			m_pSpecialEdit[i]->SetVisible(true);
		}

		rect = WRect(dest.x + 130, dest.y + 38, 100.f, 20.f);
		m_pSpecialEdit[3]->SetRect(rect);
		m_pSpecialEdit[3]->SetVisible(true);

		for (i = 0; i < 6; ++i)
		{
			rect = WRect(dest.x + 130, dest.y + i * 20 + 82, 100.f, 20.f);
			m_pSpecialEdit[4 + i]->SetRect(rect);
			m_pSpecialEdit[4 + i]->SetVisible(true);
		}

		rect = WRect(dest.x + 130, dest.y + 216, 100.f, 20.f);
		m_pSpecialEdit[10]->SetRect(rect);
		m_pSpecialEdit[10]->SetVisible(true);

		for (i = 0; i < 5; ++i)
			m_pSpecialEdit[11 + i]->SetVisible(false);
		for (i = 0; i < 5; ++i)
			m_pMatchHistoryBtn[i]->SetVisible(false);
	}
	else
	{
		dest.w = (float)m_pSpecialBack00->Width();
		dest.h = (float)m_pSpecialBack00->Height();
		gdi->DrawTexture(m_pSpecialBack00, dest, 0xffffffff, 0);

		int i;
		for (i = 0; i < 11; ++i)
			m_pSpecialEdit[i]->SetVisible(false);

		int y = 0;
		sUserMatchHistory* history = m_matchHistory;
		for (i = 0; i < 5; ++i, ++history)
		{
			if (history->dwUID)
			{
				const Bitmap* icon = (&m_pMaleIcon)[history->nGender];
				gdi->DrawTexture(icon,
					WRect(dest.x + 14, dest.y + y + 45, (float)icon->Width(),
						(float)icon->Height()),
					0xffffffff, 0);
				m_pMatchHistoryBtn[i]->SetVisible(true);

				gdi->SetTextColor(0xff000000, 0xffffffff);
				gdi->SetTextStyle(1);
				g_pFresh->GetManager()->PrintText(
					WPoint(dest.x + 51, dest.y + y + 51), 0,
					history->szNickName, -1.f, 0xffffffff);
				m_pSpecialEdit[11 + i]->SetVisible(false);
			}
			else
			{
				int& index = i;
				m_pMatchHistoryBtn[index]->SetVisible(false);
				rect = WRect(dest.x + 42, dest.y + y + 46, 124.f, 20.f);
				m_pSpecialEdit[11 + index]->SetRect(rect);
				m_pSpecialEdit[11 + index]->SetVisible(true);
			}
			y += 26;
		}

		MsnDisconExeception(m_delta, item);
	}
}

void FrUserInfoForm::UpdateEditUserLocation()
{
	if (m_userInfo.roomIndex != 0xffff)
	{
		std::list<sRoomInfo>::iterator it = std::find(Doc()->m_roomList.begin(),
			Doc()->m_roomList.end(), m_userInfo.roomIndex);
		if (it == Doc()->m_roomList.end())
		{
			m_pGeneralEdit[4]->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd",
				0xff000000, false, 0);
			return;
		}

		int state = 0;
		if ((*it).bPublic || (bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
		{
			if (!(*it).bAvailable ||
				(IsLocalContent(S3_INTRUSION) && (*it).bIntrusion))
				state = 1;
		}
		else
		{
			state = 2;
		}

		switch (state)
		{
		case 0:
			m_pGeneralEdit[4]->SetLine(1,
				MakeStr("[%d]\xb9\xf8 \xb9\xe6 %s", (*it).roomGuid,
					(*it).bSleep ? "\xc0\xe1\xbc\xf6\xc1\xdf"
								 : "\xb4\xeb\xb1\xe2\xc1\xdf"),
				0xff000000, false, 0);
			break;
		case 1:
			m_pGeneralEdit[4]->SetLine(1,
				MakeStr("[%d]\xb9\xf8 \xb9\xe6 %s", (*it).roomGuid,
					(bool)((m_userInfo.info.dwIdentity >> 1) & 1)
						? "\xb0\xfc\xc0\xfc\xc1\xdf"
						: "\xb0\xd4\xc0\xd3\xc1\xdf"),
				0xff000000, false, 0);
			break;
		case 2:
			m_pGeneralEdit[4]->SetLine(1,
				"\xba\xf1\xb0\xf8\xb0\xb3\xb9\xe6\xbf\xa1 \xc1\xa2\xbc\xd3\xc1\xdf",
				0xff000000, false, 0);
			break;
		}
	}
	else if (m_bMsnControl)
	{
		sFriend* buddy = BuddyMGR->GetBuddy(m_userInfo.info.dwUID);
		if (buddy)
		{
			if (buddy->IsLogOn)
			{
				char text[128] = "\0";
				bool bFind = false;
				for (std::list<sGameServerInfo>::iterator it =
						 Doc()->m_gameServerList.begin();
					it != Doc()->m_gameServerList.end(); ++it)
				{
					if ((*it).id == buddy->userPosition.iServerGUID)
					{
						wsprintf(text, "%s", (*it).name);
						bFind = true;
						break;
					}
				}
				if (!bFind)
				{
					m_pGeneralEdit[4]->SetLine(1, "\xc1\xa2\xbc\xd3\xc1\xdf",
						0xff000000, false, 0);
					return;
				}

				wsprintf(text, "%s %s", text,
					buddy->userPosition.csChannelName);
				if (buddy->userPosition.iRoomIdx != 0xffff)
					wsprintf(text, "%s [%d]", text,
						buddy->userPosition.iRoomIdx);
				m_pGeneralEdit[4]->SetLine(1, text, 0xff000000, false, 0);
			}
			else
			{
				m_pGeneralEdit[4]->SetLine(1,
					"\xc1\xa2\xbc\xd3\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xbd",
					0xff000000, false, 0);
			}
		}
		else if (m_bUnknownLocation)
		{
			m_pGeneralEdit[4]->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd",
				0xff000000, false, 0);
		}
		else
		{
			m_pGeneralEdit[4]->SetLine(1, "\xc4\xa3\xb1\xb8\xbe\xc6\xb4\xd4",
				0xff000000, false, 0);
		}
	}
	else
	{
		m_pGeneralEdit[4]->SetLine(1,
			"\xb4\xeb\xb1\xe2\xbd\xc7\xbf\xa1 \xc1\xa2\xbc\xd3\xc1\xdf",
			0xff000000, false, 0);
	}
}

void FrUserInfoForm::OnRecTotalListboxInit(int param)
{
	m_pRecTotalListBox = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pRecTotalListBox->SetVisible(false);

	m_pRecTotalBack[0] = g_pFresh->RegisterBitmap("special_back01");
	m_pRecTotalBack[1] = g_pFresh->RegisterBitmap("special_back02");
	m_pRecTotalBack[2] = g_pFresh->RegisterBitmap("special_back03");
	m_pRecTotalBackNd = g_pFresh->RegisterBitmap("special_back02_nd");

	m_pRecTotalListBox->AddItem((void*)m_pRecTotalBack[0]);
	m_pRecTotalListBox->AddItem((void*)m_pRecTotalBack[1]);
	m_pRecTotalListBox->AddItem((void*)m_pRecTotalBack[2]);
}

void FrUserInfoForm::OnRecTotalListboxOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	WRect dest;
	dest.x = item->pos.x;
	dest.y = item->pos.y;

	WRect rect;
	if (item->pData == m_pRecTotalBack[0])
	{
		dest.w = (float)m_pRecTotalBack[0]->Width();
		dest.h = (float)m_pRecTotalBack[0]->Height();
		gdi->DrawTexture(m_pRecTotalBack[0], dest, 0xffffffff, 0);

		rect = WRect(dest.x + 68, dest.y + 22, 160.f, 20.f);
		m_pRecordEdit[0]->SetRect(rect);
		m_pRecordEdit[0]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 42, 160.f, 20.f);
		m_pRecordEdit[1]->SetRect(rect);
		m_pRecordEdit[1]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 62, 160.f, 20.f);
		m_pRecordEdit[2]->SetRect(rect);
		m_pRecordEdit[2]->SetVisible(true);

		rect = WRect(dest.x + 121, dest.y + 81, 44.f, 20.f);
		m_pRecordEdit[3]->SetRect(rect);
		m_pRecordEdit[3]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 81, 160.f, 20.f);
		m_pRecordEdit[4]->SetRect(rect);
		m_pRecordEdit[4]->SetVisible(true);

		m_pRecordEdit[12]->SetVisible(false);
		m_pRecordEdit[13]->SetVisible(false);
		m_pRecordEdit[14]->SetVisible(false);
	}
	else if (item->pData == m_pRecTotalBack[1])
	{
		dest.w = (float)m_pRecTotalBack[1]->Width();
		dest.h = (float)m_pRecTotalBack[1]->Height();
		gdi->DrawTexture(m_pRecTotalBack[1], dest, 0xffffffff, 0);

		rect = WRect(dest.x + 68, dest.y + 14, 160.f, 20.f);
		m_pRecordEdit[5]->SetRect(rect);
		m_pRecordEdit[5]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 32, 160.f, 20.f);
		m_pRecordEdit[6]->SetRect(rect);
		m_pRecordEdit[6]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 50, 160.f, 20.f);
		m_pRecordEdit[7]->SetRect(rect);
		m_pRecordEdit[7]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 68, 160.f, 20.f);
		m_pRecordEdit[8]->SetRect(rect);
		m_pRecordEdit[8]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 86, 160.f, 20.f);
		m_pRecordEdit[9]->SetRect(rect);
		m_pRecordEdit[9]->SetVisible(true);

		if (dest.y - m_rect.y < 250)
		{
			rect = WRect(dest.x + 38, dest.y + 126, 69.f, 20.f);
			m_pRecordEdit[10]->SetRect(rect);
			m_pRecordEdit[10]->SetVisible(true);

			rect = WRect(dest.x + 145, dest.y + 126, 69.f, 20.f);
			m_pRecordEdit[11]->SetRect(rect);
			m_pRecordEdit[11]->SetVisible(true);
		}
		else
		{
			m_pRecordEdit[10]->SetVisible(false);
			m_pRecordEdit[11]->SetVisible(false);
		}
	}
	else if (item->pData == m_pRecTotalBack[2])
	{
		dest.w = (float)m_pRecTotalBack[2]->Width();
		dest.h = (float)m_pRecTotalBack[2]->Height();
		gdi->DrawTexture(m_pRecTotalBack[2], dest, 0xffffffff, 0);

		rect = WRect(dest.x + 68, dest.y + 52, 160.f, 20.f);
		m_pRecordEdit[12]->SetRect(rect);
		m_pRecordEdit[12]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 74, 160.f, 20.f);
		m_pRecordEdit[13]->SetRect(rect);
		m_pRecordEdit[13]->SetVisible(true);

		rect = WRect(dest.x + 68, dest.y + 96, 160.f, 20.f);
		m_pRecordEdit[14]->SetRect(rect);
		m_pRecordEdit[14]->SetVisible(true);

		m_pRecordEdit[0]->SetVisible(false);
		m_pRecordEdit[1]->SetVisible(false);
		m_pRecordEdit[2]->SetVisible(false);
		m_pRecordEdit[3]->SetVisible(false);
		m_pRecordEdit[4]->SetVisible(false);
	}
}

void FrUserInfoForm::OnTrophyNormalNumInit(int param)
{
	m_pNormalTrophyNum = DYNAMIC_CAST(FrEdit, param);
	m_pNormalTrophyNum->SetVisible(false);
}

void FrUserInfoForm::OnTrophySpecialNumInit(int param)
{
	m_pSpecialTrophyNum = DYNAMIC_CAST(FrEdit, param);
	m_pSpecialTrophyNum->SetVisible(false);
}

void FrUserInfoForm::OnTrophyNormalBtnInit(int param)
{
	m_pTrophyBtn[0] = DYNAMIC_CAST(FrButton, param);

	if (m_trophyType == 0)
		m_pTrophyBtn[0]->SetStatus(FrButton::PRESSED);
	else
		m_pTrophyBtn[0]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnTrophyNormalBtnUp()
{
	m_trophyType = 0;

	m_pTrophyBtn[0]->SetStatus(FrButton::PRESSED);
	m_pTrophyBtn[1]->SetStatus(FrButton::NORMAL);

	ChangeTrophyType();
}

void FrUserInfoForm::OnTrophySpecialBtnInit(int param)
{
	m_pTrophyBtn[1] = DYNAMIC_CAST(FrButton, param);

	if (m_trophyType == 1)
		m_pTrophyBtn[1]->SetStatus(FrButton::PRESSED);
	else
		m_pTrophyBtn[1]->SetStatus(FrButton::NORMAL);
}

void FrUserInfoForm::OnTrophySpecialBtnUp()
{
	m_trophyType = 1;

	m_pTrophyBtn[1]->SetStatus(FrButton::PRESSED);
	m_pTrophyBtn[0]->SetStatus(FrButton::NORMAL);

	ChangeTrophyType();
}

void FrUserInfoForm::ChangeRecordType()
{
	if (m_recordType == 0)
	{
		if (m_pBackArea[3])
			m_pBackArea[3]->SetVisible(false);

		m_pRecTotalListBox->SetVisible(true);
		m_pScoreList->SetVisible(false);
	}
	else if (m_recordType == 2 || m_recordType == 3)
	{
		if (m_pBackArea[3])
			m_pBackArea[3]->SetVisible(true);

		SetVisible((FrWnd**)m_pRecordEdit, 15, false);

		m_pRecTotalListBox->SetVisible(false);
		m_pScoreList->SetVisible(true);
	}
	else if (m_recordType == 1)
	{
		if (m_pBackArea[3])
			m_pBackArea[3]->SetVisible(false);
		if (m_pBackArea[2])
			m_pBackArea[2]->SetVisible(true);

		SetVisible(m_pRecAwardWnd, 6, true);
		SetVisible((FrWnd**)m_pRecordEdit, 15, false);

		m_pRecTotalListBox->SetVisible(false);
		m_pScoreList->SetVisible(false);
	}
}

void FrUserInfoForm::ChangeTrophyType()
{
	if (m_trophyType == 1)
	{
		m_pNormalTrophyNum->SetVisible(false);
		m_pSpecialTrophyNum->SetVisible(true);

		if (m_pBackArea[4])
			m_pBackArea[4]->SetVisible(false);
		if (m_pBackArea[5])
			m_pBackArea[5]->SetVisible(true);

		if (m_pSpecialTrophyBox)
		{
			m_pSpecialTrophyBox->SetVisible(true);

			if (m_pNoTrophy)
				m_pNoTrophy->SetVisible(
					m_pSpecialTrophyBox->m_itemList.size() == 0);
		}

		if (m_pNormalTrophyBox)
			m_pNormalTrophyBox->SetVisible(false);
	}
	else if (m_trophyType == 0)
	{
		m_pNormalTrophyNum->SetVisible(true);
		m_pSpecialTrophyNum->SetVisible(false);

		if (m_pBackArea[4])
			m_pBackArea[4]->SetVisible(true);
		if (m_pBackArea[5])
			m_pBackArea[5]->SetVisible(false);

		if (m_pSpecialTrophyBox)
			m_pSpecialTrophyBox->SetVisible(false);

		if (m_pNormalTrophyBox)
		{
			m_pNormalTrophyBox->SetVisible(true);

			if (m_pNoTrophy)
				m_pNoTrophy->SetVisible(
					m_pNormalTrophyBox->m_itemList.size() == 0);
		}
	}
}

void FrUserInfoForm::OnListBoxInit_TRI_SPECIALBOX(int param)
{
	m_pSpecialTrophyBox = DYNAMIC_CAST(FrListBox, param);
}

void FrUserInfoForm::OnListBoxOwnerDraw_TRI_SPECIALBOX(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	sSpecialTrophy* trophy = (sSpecialTrophy*)item->pData;
	float h = (float)m_pNormalTrophyBox->GetItemHeight();

	IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(trophy->tid);
	if (!match)
		return;

	char name[100];
	strcpy(name, match->Name);
	char* text = name;
	int line = 0;
	char* next = strstr(text, "\n");
	while (next)
	{
		*next = 0;
		gdi->SetTextStyle(1);
		gdi->Print(item->pos + WPoint(5.f, line * 15.f + 30.f), 0, "%s", text);
		++line;
		text = next + 1;
		next = strstr(text, "\n");
	}
	gdi->SetTextStyle(0);
	gdi->Print(item->pos + WPoint(5.f, line * 15.f + 30.f), 0, "%s", text);

	WRect dest;
	dest.y = item->pos.y + 3.f;
	gdi->SetTextStyle(0);

	const Bitmap* bitmap =
		g_pFresh->GetManager()->GetBitmap("TROPHY", match->Icon[0]);
	dest.x = item->pos.x + 185.f;
	dest.w = (float)bitmap->Width();
	dest.h = (float)bitmap->Height();
	gdi->DrawTexture(bitmap, dest, trophy->count ? 0xffffffff : 0x60000000, 0);

	unsigned long count = trophy->count;
	if (count > 1)
	{
		int half = float2int(dest.w * 0.5f);
		gdi->Print(WPoint(half + dest.x, h + item->pos.y - 15.f), 1, "%d",
			count);
	}
}

void FrUserInfoForm::OnListBoxInit_TRI_NORMALBOX(int param)
{
	m_pNormalTrophyBox = DYNAMIC_CAST(FrListBox, param);
}

void FrUserInfoForm::OnListBoxOwnerDraw_TRI_NORMALBOX(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	int index = (int)item->pData;
	if (index < 0 || index >= 13)
		return;

	unsigned short* trophy;
	switch (m_seasonType)
	{
	case SEASON_CURRENT:
		trophy = m_userInfo.trophy.Trophy[index];
		break;
	case SEASON_TOTAL:
		trophy = m_totalSeason.trophy.Trophy[index];
		break;
	default:
		trophy = m_userInfo.trophy.Trophy[index];
		break;
	}

	float h = (float)m_pNormalTrophyBox->GetItemHeight();
	ItemManager()->FindMatch(0x2c000000 | (index << 16));
	IFF_STRUCT::sMatch* match =
		ItemManager()->FindMatch(0x2c000000 | (index << 16));
	if (!match)
		return;

	gdi->SetTextStyle(1);
	gdi->Print(WPoint(item->pos.x + 20.f, item->pos.y + 38.f), 0, "%s",
		match->Name);

	WRect dest;
	dest.y = item->pos.y + 3.f;
	gdi->SetTextStyle(0);

	const Bitmap* bitmap[3];
	for (int i = 0; i < 3; ++i)
	{
		bitmap[i] = g_pFresh->GetManager()->GetBitmap("TROPHY", match->Icon[i]);
		dest.x = i * 60.f + item->pos.x + 75.f;
		if (bitmap[i])
		{
			dest.w = (float)bitmap[i]->Width();
			dest.h = (float)bitmap[i]->Height();
			gdi->DrawTexture(bitmap[i], dest,
				trophy[i] ? 0xffffffff : 0x60000000, 0);

			WPoint num;
			num.x = (int)(dest.w * 0.5f) + dest.x;
			num.y = h + item->pos.y - 15.f;
			gdi->Print(num, 1, "%d", trophy[i]);
		}
	}
}

void FrUserInfoForm::OnStaticInit_TRI_NOTROPHY(int param)
{
	m_pNoTrophy = DYNAMIC_CAST(FrStatic, param);
	if (m_pNoTrophy)
		m_pNoTrophy->SetVisible(false);
}

void FrUserInfoForm::OnMyGuildInit(int param)
{
	m_pMyGuild = DYNAMIC_CAST(FrArea, param);
	m_pGuildIcon = g_pFresh->GetManager()->GetBitmap("TROPHY", "guild");
}

void FrUserInfoForm::OnMyGuildOwnerDraw()
{
	if (!m_userInfo.info.dwGuildId || (m_userInfo.info.dwIdentity & 0x14))
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	WRect rect = m_pMyGuild->GetRect();

	if (m_pGuildIcon)
		gdi->DrawTexture(m_pGuildIcon,
			WRect(rect.x, rect.y, (float)m_pGuildIcon->Width(),
				(float)m_pGuildIcon->Height()),
			0xffffffff, 0);

	const Bitmap* emblem =
		NetResManager()->GetEmblemByName(m_userInfo.info.szEmblemName);
	if (!emblem)
		return;

	gdi->DrawTexture(emblem,
		WRect(rect.x + 21.f, rect.y + 12.f, (float)emblem->Width(),
			(float)emblem->Height()),
		0xffffffff, 0);
}

void FrUserInfoForm::OnGuildTrophyListInit(int param)
{
	m_pGuildTrophyList = DYNAMIC_CAST(FrListBox, param);
	if (m_pGuildTrophyList)
		m_pGuildTrophyList->SetVisible(false);
}

void FrUserInfoForm::OnGuildTrophyListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	sGuildTrophy* trophy = (sGuildTrophy*)item->pData;
	if (!trophy)
		return;

	IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(trophy->TypeID);
	if (!match)
		return;

	gdi->SetTextColor(0xff000000, 0xffffffff);

	char name[100];
	strcpy(name, match->Name);
	char* text = name;
	int line = 0;
	char* next = strstr(text, "\n");
	while (next)
	{
		*next = 0;
		gdi->SetTextStyle(1);
		gdi->Print(item->pos + WPoint(18.f, line * 15.f + 30.f), 0, "%s", text);
		++line;
		text = next + 1;
		next = strstr(text, "\n");
	}
	gdi->SetTextStyle(0);
	gdi->Print(item->pos + WPoint(18.f, line * 15.f + 30.f), 0, "%s", text);

	WRect dest;
	dest.y = item->pos.y + 3.f;
	gdi->SetTextStyle(0);

	const Bitmap* bitmap =
		g_pFresh->GetManager()->GetBitmap("TROPHY", match->Icon[0]);
	dest.x = item->pos.x + 130.f;
	dest.w = (float)bitmap->Width();
	dest.h = (float)bitmap->Height();
	gdi->DrawTexture(bitmap, dest, 0xffffffff, 0);
}

void FrUserInfoForm::OnStaticInit_GUILD_NOTROPHY(int param)
{
	m_pGuildNoTrophy = DYNAMIC_CAST(FrStatic, param);
	if (m_pGuildNoTrophy)
		m_pGuildNoTrophy->SetVisible(false);
}

void FrUserInfoForm::OnWhisperInit(int param)
{
	m_pWhisperBtn = DYNAMIC_CAST(FrButton, param);
}

void FrUserInfoForm::OnNickInit(int param)
{
	m_pNickArea = DYNAMIC_CAST(FrArea, param);

	m_pMaleIcon = g_pFresh->RegisterBitmap("i_male");
	m_pFemaleIcon = g_pFresh->RegisterBitmap("i_female");
}

void FrUserInfoForm::OnNickOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;

	if (m_userInfo.info.dwGuid == -1)
		return;

	const Bitmap* icon = (&m_pMaleIcon)[m_userInfo.info.gender];
	gdi->DrawTexture(icon,
		WRect(m_pNickArea->GetRect().x, m_pNickArea->GetRect().y,
			(float)icon->Width(), (float)icon->Height()),
		0xffffffff, 0);

	if (CProjectG::Instance()->HidePrivacy())
		return;

	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(1);
	g_pFresh->GetManager()->PrintText(
		WPoint(m_pNickArea->GetRect().x + 27.f, m_pNickArea->GetRect().y + 8.f),
		0, m_userInfo.info.sNick, -1.f, 0xffffffff);
}

void FrUserInfoForm::OnWhisperUp()
{
	if (m_userInfo.info.dwUID == -1 || m_userInfo.info.dwUID == MyUID())
		return;

	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 55, (int)m_userInfo.info.sNick, 0, 0, 0, 0);
	else if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 55, (int)m_userInfo.info.sNick, 0, 0, 0, 0);
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 55, (int)m_userInfo.info.sNick, 0, 0, 0, 0);
}

void FrUserInfoForm::OnFriendInit(int param)
{
	m_pFriendBtn = DYNAMIC_CAST(FrButton, param);
}

void FrUserInfoForm::OnFriendUp()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()) ||
		IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		m_bChatMsgOpen = CChatMsg::Instance()->IsActive();

	if (!m_pAddFriendDlg)
	{
		m_pAddFriendDlg = CreateForm<FrAddFriendDlg>(g_pFresh->GetManager(),
			this, "addfriend", NULL);
		m_pAddFriendDlg->CheckRequestNickname(m_userInfo.info.sNick);
		m_pAddFriendDlg->Open(
			(FRESH_PFN_RESULT)&FrUserInfoForm::OnAddFriendFormResult, 1);
	}
}

void FrUserInfoForm::OnIgnoreInit(int param)
{
	m_pIgnoreBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnIgnoreUp()
{
	if (m_userInfo.info.dwUID == 0xffffffff ||
		m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID)
		return;

	bool bIgnore = false;
	std::list<std::string>::iterator it = std::find(Doc()->m_ignoreList.begin(),
		Doc()->m_ignoreList.end(), m_userInfo.info.sNick);
	if (it == Doc()->m_ignoreList.end())
	{
		Doc()->m_ignoreList.push_back(m_userInfo.info.sNick);
		bIgnore = true;
	}
	else
	{
		Doc()->m_ignoreList.erase(it);
	}
	SetIgnoreButtonImg(bIgnore);
}

void FrUserInfoForm::SetIgnoreButtonImg(bool bIgnore)
{
	if (bIgnore)
	{
		m_pIgnoreBtn->SetButtonImg("btn_i02_02_n", FrButton::NORMAL);
		m_pIgnoreBtn->SetButtonImg("btn_i02_02_o", FrButton::OVER);
		m_pIgnoreBtn->SetButtonImg("btn_i02_02_d", FrButton::PRESSED);
	}
	else
	{
		m_pIgnoreBtn->SetButtonImg("btn_i02_n", FrButton::NORMAL);
		m_pIgnoreBtn->SetButtonImg("btn_i02_o", FrButton::OVER);
		m_pIgnoreBtn->SetButtonImg("btn_i02_d", FrButton::PRESSED);
	}
}

void FrUserInfoForm::OnCloseUp()
{
	Close(FrNONE, true);
}

bool FrUserInfoForm::Close(eFormRet ret, bool bFade)
{
	if (m_anotherHand.IsWorking(0))
		return false;

	return FrForm::Close(ret, bFade);
}

void FrUserInfoForm::OnProc(const float delta)
{
	if (IsLocalContent(S4_UCC))
	{
		RefreshAllUccClothes();
	}

	m_delta = delta;
	if (m_petLoadTime < 3.f)
	{
		m_petLoadTime += delta;
		if (m_petLoadTime >= 3.f || !m_anotherHand.IsWorking(0))
		{
			m_pPetLoadGauge->SetVisible(false);
			m_petLoadTime = 3.f;
		}
	}
}

void FrUserInfoForm::OnAnotherHand(int key, void* param)
{
	if (key == 0)
	{
		ShowPet();
	}
}

void FrUserInfoForm::RefreshAllUccClothes()
{
	if (m_pPartTidList && m_pExhibition && m_pExhibition->GetPetFrame())
	{
		sUccRefreshItem item;
		for (int i = 0; i < 10; i++)
		{
			if (!UccManager()->GetRefreshItem(item))
				break;

			unsigned long tid = item.typeId;
			unsigned char index = (unsigned char)((tid >> 13) & 0x1f);
			if (m_pPartTidList->m_tid[index] == tid)
			{
				unsigned long uid = m_pPartTidList->m_uid[index];

				sUccClothes* pClothes = UccManager()->FindClothes(uid);

				if (pClothes && !strcmpi(pClothes->uccIndex, item.uccIndex))
				{
					if (m_pExhibition && m_pExhibition->GetPetFrame())
					{
						m_pExhibition->GetPetFrame()->SetPart(m_pPartTidList,
							pClothes->typeId, pClothes->id);
						m_pExhibition->GetPetFrame()->SetPart(m_pPartTidList,
							pClothes->typeId, pClothes->id);
					}
				}
			}
		}
	}
}

bool FrUserInfoForm::OnInit()
{
	m_disconnRect = m_pGeneralEdit[5]->GetRect();
	m_tabType = TAB_NONE;

	SetVisible(TAB_GENERAL, false);
	SetVisible(TAB_SPECIAL, false);
	SetVisible(TAB_RECORD, false);
	SetVisible(TAB_TROPHY, false);
	SetVisible(TAB_GUILD, false);

	m_petLoadTime = 0;
	m_pGeneralEdit[0]->HidePrivacy(true);

	m_pGeneralEdit[0]->DisableEditStyle(FrEdit::ES_EMOTICON);

	return true;
}

void FrUserInfoForm::SetFormA(eTabType type)
{
	if (m_tabType != type)
	{
		SetVisible(m_tabType, false);
		m_tabType = type;
		SetVisible(m_tabType, true);

		for (int i = 0; i < 5; ++i)
		{
			if (i == m_tabType)
			{
				if (m_pTabBtn[i])
					m_pTabBtn[i]->SetStatus(FrButton::PRESSED);
			}
			else
			{
				if (m_pTabBtn[i])
					m_pTabBtn[i]->SetStatus(FrButton::NORMAL);
			}
		}
	}
}

void FrUserInfoForm::HidePet(bool bWait)
{
	if (bWait)
	{
		m_anotherHand.WaitUntilWorkDone(9999);
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		if (m_pPartTidList)
		{
			delete m_pPartTidList;
			m_pPartTidList = NULL;
		}
	}
	else
	{
		if (m_pExhibition)
		{
			delete m_pExhibition;
			m_pExhibition = NULL;
		}
		if (m_pPartTidList)
		{
			delete m_pPartTidList;
			m_pPartTidList = NULL;
		}
	}
}

void FrUserInfoForm::SetInfo(sUserInfoTime* pInfo)
{
	int i;
	m_position.iServerGUID = pInfo->userPosition.iServerGUID;
	wsprintf(m_position.csChannelName, "%s", pInfo->userPosition.csChannelName);
	m_position.iRoomIdx = pInfo->userPosition.iRoomIdx;

	m_userInfo = pInfo->info;

	if (IsLocalContent(S3_MATCH_HISTORY))
	{
		if (!strcmpi(m_userInfo.info.sNick, MyNick()))
		{
			CMatchHistory::Instance()->GetMatchHistory(m_matchHistory);
			if (!CMatchHistory::Instance()->IsUpdated())
			{
				CMatchHistory::Instance()->RequestHistory();
			}

			if (!m_pSpecListBox->FindItem((void*)m_pSpecialBack00))
				m_pSpecListBox->AddItem((void*)m_pSpecialBack00);
		}
		else
		{
			if (m_pSpecListBox->FindItem((void*)m_pSpecialBack00))
				m_pSpecListBox->DelItem((void*)m_pSpecialBack00);
		}
	}
	else
	{
		m_pSpecListBox->DelItem((void*)m_pSpecialBack00);
	}

	m_specialTrophy.clear();
	for (i = 0; i < pInfo->numTrophies; ++i)
		m_specialTrophy.push_back(pInfo->trophies[i]);

	m_guildTrophy.clear();
	for (i = 0; i < pInfo->numGuildTrophies; ++i)
		m_guildTrophy.push_back(pInfo->guildTrophies[i]);

	m_guildPang = pInfo->dwGuildPang;
	m_guildPoint = pInfo->dwGuildPoint;

	memcpy(m_classicMapStatistics, pInfo->info.classicMapStat,
		sizeof(m_classicMapStatistics));
	memcpy(m_oldClassicMapStatistics, pInfo->oldSeason.classicMapStat,
		sizeof(m_oldClassicMapStatistics));

	m_oldSeason.statistics = pInfo->oldSeason.userStat;
	for (i = 0; i < 20; ++i)
		m_oldSeason.mapStatistics[i] = pInfo->oldSeason.mapStat[i];
	m_oldSeason.trophy = pInfo->oldSeason.trophyStat;

	m_oldSeason.specialTrophy.clear();
	for (i = 0; i < pInfo->oldSeason.numTrophies; ++i)
		m_oldSeason.specialTrophy.push_back(pInfo->oldSeason.trophies[i]);

	m_oldSeason.guildTrophy.clear();
	for (i = 0; i < pInfo->oldSeason.numGuildTrophies; ++i)
		m_oldSeason.guildTrophy.push_back(pInfo->oldSeason.guildTrophies[i]);

	if (m_userInfo.info.dwUID != 0xffffffff)
	{
		if (m_pFriendBtn)
		{
			if (m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID)
				m_pFriendBtn->Enable(false);
			else
				m_pFriendBtn->Enable(!m_bControlOff);
		}

		if (m_pWhisperBtn)
			m_pWhisperBtn->Enable(
				m_userInfo.info.dwUID != Doc()->m_myInfo.info.dwUID &&
				!m_bControlOff);

		if (m_pIgnoreBtn)
		{
			m_pIgnoreBtn->Enable(
				m_userInfo.info.dwUID != Doc()->m_myInfo.info.dwUID &&
				!m_bControlOff);

			bool bIgnore = false;
			std::list<std::string>::iterator it =
				std::find(Doc()->m_ignoreList.begin(),
					Doc()->m_ignoreList.end(), m_userInfo.info.sNick);
			if (it != Doc()->m_ignoreList.end())
			{
				bIgnore = true;
			}
			SetIgnoreButtonImg(bIgnore);
		}

		if (m_userInfo.userEquip.tidSkin[0])
		{
			IFF_STRUCT::sSkin* pSkin =
				ItemManager()->FindSkin(m_userInfo.userEquip.tidSkin[0]);

			if (pSkin)
			{
				m_skinScrollType[0] = pSkin->HScroll;
				m_skinScrollType[1] = pSkin->VScroll;
			}

			m_skinScroll[0] = 0;
		}
	}

	SetFormA(TAB_GENERAL);
	BuildTotalStat();
	SetContent(false);
	m_anotherHand.Work(this, 0, NULL, true, -1);

	m_bRefresh = false;

	if ((Doc()->m_curChannel.Type & 0x80) &&
		m_userInfo.info.dwUID != Doc()->m_myInfo.info.dwUID)
	{
		m_pTabBtn[1]->Enable(false);
		m_pTabBtn[2]->Enable(false);
		m_pTabBtn[3]->Enable(false);
	}
}

bool ScoreListCompare(const void* a, const void* b)
{
	return ((const sMapStatistics*)a)->cBestScore <
		((const sMapStatistics*)b)->cBestScore;
}

void FrUserInfoForm::SelectContents(eSeasonType season,
	sPangYaUserStatistics** ppStat, sTrophyStatistics** ppTrophy,
	std::list<sSpecialTrophy>** ppSpecial, std::list<sGuildTrophy>** ppGuild)
{
	switch (season)
	{
	default:
	case SEASON_CURRENT:
		*ppStat = &m_userInfo.stat;
		*ppTrophy = &m_userInfo.trophy;
		*ppSpecial = &m_specialTrophy;
		*ppGuild = &m_guildTrophy;
		break;

	case SEASON_TOTAL:
		*ppStat = &m_totalSeason.statistics;
		*ppTrophy = &m_totalSeason.trophy;
		*ppSpecial = &m_totalSeason.specialTrophy;
		*ppGuild = &m_totalSeason.guildTrophy;
		break;
	}
}

unsigned long FrUserInfoForm::GetSeasonTextColor(eSeasonType season)
{
	return 0xff000000;
}

void FrUserInfoForm::UpdateEditSchoolAndGuild(eSeasonType season,
	const sPangYaUserInfo* pInfo)
{
	char text[128] = { 0 };

	unsigned long school = m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID
		? Doc()->m_myInfo.info.school
		: pInfo->school;
	Doc()->LoadSchoolName(text, school);

	if (text[0])
		m_pGeneralEdit[1]->SetLine(1, text, GetSeasonTextColor(season), false,
			0);
	else
		m_pGeneralEdit[1]->SetLine(1,
			"\xbc\xd2\xbc\xd3 \xc7\xd0\xb1\xb3 \xbe\xf8\xc0\xbd",
			GetSeasonTextColor(season), false, 0);

	if (m_userInfo.info.IsIdentity(4))
		m_pGeneralEdit[2]->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd",
			GetSeasonTextColor(season), false, 0);
	else
	{
		sprintf(text, "%s",
			strlen(pInfo->sGuild)
				? pInfo->sGuild
				: "\xbc\xd2\xbc\xd3 \xb1\xe6\xb5\xe5 \xbe\xf8\xc0\xbd",
			GetSeasonTextColor(season));
		m_pGeneralEdit[2]->SetLine(1, text, GetSeasonTextColor(season), false,
			0);
	}
}

void FrUserInfoForm::UpdateLevelTextImg(unsigned char level)
{
	const Bitmap* bitmap = g_pFresh->GetBitmap(
		MakeStr("level_%03d", level + 1 < 71 ? level + 1 : 71));
	if (bitmap)
	{
		m_pLevelArea->SetBgImg(bitmap);
	}
}

bool IsNothingRecord(const sPangYaUserStatistics* pStat)
{
	if (!pStat)
		return false;
	return !pStat->Level || !pStat->dwGameCount;
}

bool IsMyRecord(const sUserInfo* pInfo)
{
	if (!pInfo)
		return false;
	return pInfo->info.dwUID == Doc()->m_myInfo.info.dwUID ||
		Doc()->m_myInfo.info.IsIdentity(4);
}

bool IsHideDisconnectRate(const sPangYaUserStatistics* pStat)
{
	return pStat->dwGameCount <= 50 || pStat->Level < 6;
}

void FrUserInfoForm::UpdateEditExpAndMannerInfo(eSeasonType season,
	const sPangYaUserStatistics* pStat)
{
	char text[128] = { 0 };

	if ((Doc()->m_curChannel.Type & 0x80) &&
		m_userInfo.info.dwUID != Doc()->m_myInfo.info.dwUID)
		sprintf(text, "???");
	else if (pStat->Level < 70)
		sprintf(text, "%d / %d", pStat->dwExp,
			Doc()->m_levelTable[pStat->Level]);
	else
		sprintf(text, "MAX");

	m_pGeneralEdit[3]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (IsNothingRecord(pStat))
	{
		SetDisconnQuestionMark(true);
		sprintf(text, "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd");
	}
	else
	{
		SetDisconnQuestionMark(false);
		if (IsMyRecord(&m_userInfo))
		{
			if (IsHideDisconnectRate(pStat))
			{
				SetDisconnQuestionMark(true);
				sprintf(text, "- - -  [%d / %d]", pStat->dwNoMannerGameCount,
					pStat->dwGameCount);
			}
			else
			{
				sprintf(text, "%.1f%% [%d / %d]",
					pStat->dwNoMannerGameCount * 100.f / pStat->dwGameCount,
					pStat->dwNoMannerGameCount, pStat->dwGameCount);
			}
		}
		else
		{
			if (IsHideDisconnectRate(pStat))
			{
				SetDisconnQuestionMark(true);
				sprintf(text, "- - -");
			}
			else
			{
				sprintf(text, "%.1f %%",
					pStat->dwNoMannerGameCount * 100.f / pStat->dwGameCount);
			}
		}
	}

	m_pGeneralEdit[5]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	sprintf(text, "%d / %d", pStat->nMannerCombo, pStat->nMaxMannerCombo);
	m_pGeneralEdit[6]->SetLine(1, text, GetSeasonTextColor(season), false, 0);
}

void FrUserInfoForm::UpdateEditSpecialInfo(eSeasonType season,
	const sPangYaUserStatistics* pStat, const sTrophyStatistics* pTrophy)
{
	char text[128] = { 0 };
	const char* pNoRecord = "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd";
	int bTotal = IsTotalTab();

	int trophy[3] = { 0, 0, 0 };
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 13; ++j)
			trophy[i] += pTrophy->Trophy[j][i];

	sprintf(text, "%d ", trophy[0]);
	m_pSpecialEdit[0]->SetLine(1, text, GetSeasonTextColor(season), false, 0);
	sprintf(text, "%d ", trophy[1]);
	m_pSpecialEdit[1]->SetLine(1, text, GetSeasonTextColor(season), false, 0);
	sprintf(text, "%d ", trophy[2]);
	m_pSpecialEdit[2]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwTeamGames == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f %%",
			(float)pStat->dwTeamWin / pStat->dwTeamGames * 100.f);
	m_pSpecialEdit[3]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwHole - pStat->dwMatchHole == 0)
		strcpy(text, pNoRecord);
	else if (IsLocalCountry(30))
		sprintf(text, "%.2f \xc5\xb8",
			min(100.f,
				pStat->iTotalScore * 18.f /
						(pStat->dwHole - pStat->dwMatchHole) +
					72.f));
	else
		sprintf(text, "%.2f \xc5\xb8",
			pStat->iTotalScore * 18.f / (pStat->dwHole - pStat->dwMatchHole) +
				72.f);
	m_pSpecialEdit[4]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwDrive + pStat->dwPutt == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f \xc3\xca",
			(float)pStat->dwShotTime / (pStat->dwDrive + pStat->dwPutt));
	m_pSpecialEdit[5]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwDrive == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f %%",
			(float)pStat->dwPangya / pStat->dwDrive * 100.f);
	m_pSpecialEdit[6]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwHole - pStat->dwMatchHole == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f %%",
			(float)pStat->dwFairway / (pStat->dwHole - pStat->dwMatchHole) *
				100.f);
	m_pSpecialEdit[7]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwPutt == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f %%",
			(float)pStat->dwPuttIn / pStat->dwPutt * 100.f);
	m_pSpecialEdit[8]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->dwDrive + pStat->dwPutt == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%.1f %%",
			(float)pStat->dwOB / (pStat->dwDrive + pStat->dwPutt) * 100.f);
	m_pSpecialEdit[9]->SetLine(1, text, GetSeasonTextColor(season), false, 0);

	if (pStat->i64TotalSkinsPang == 0)
		strcpy(text, pNoRecord);
	else
		sprintf(text, "%I64d", pStat->i64TotalSkinsPang);
	m_pSpecialEdit[10]->SetLine(1, text, GetSeasonTextColor(season), false, 0);
}

void FrUserInfoForm::UpdateEditMapStatistics(eSeasonType season, bool bClassic)
{
	sMapStatistics* pMapStat;
	switch (season)
	{
	case SEASON_CURRENT:
		pMapStat = bClassic ? m_classicMapStatistics : m_userInfo.mapStat;
		break;

	case SEASON_TOTAL:
		pMapStat = bClassic ? m_totalClassicMapStatistics
							: m_totalSeason.mapStatistics;
		break;
	default:
		pMapStat = bClassic ? m_classicMapStatistics : m_userInfo.mapStat;
		break;
	}

	if (!pMapStat)
		return;

	if (m_pScoreList)
	{
		m_pScoreList->ClearItem();

		for (int i = 0; i < 20; ++i, ++pMapStat)
		{
			if (pMapStat->bMap == 17)
				continue;

			IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
				(pMapStat->bMap <= 127 || pMapStat->bMap == 253
						? pMapStat->bMap
						: pMapStat->bMap - 128) |
				0x28000000);
			if (pCourse && pCourse->c.Final)
			{
				m_pScoreList->AddItem(pMapStat);
			}
		}

		m_pScoreList->SortItem(ScoreListCompare);
	}
}

void FrUserInfoForm::SetDisconntextAlign()
{
	if (!m_pQuestionMark)
		return;

	FrEdit* pEdit = m_pGeneralEdit[5];
	WRect rect;
	rect = pEdit->GetRect();

	if (m_pQuestionMark->IsVisible())
	{
		if (rect.x == m_disconnRect.x)
		{
			rect.x -= m_pQuestionMark->GetRect().w;
			pEdit->SetRect(rect);
		}
	}
	else
	{
		if (rect.x != m_disconnRect.x)
		{
			rect.x += m_pQuestionMark->GetRect().w;
			pEdit->SetRect(rect);
		}
	}
}

void FrUserInfoForm::SetDisconnQuestionMark(bool bShow)
{
	if (m_pQuestionMark)
	{
		m_pQuestionMark->SetVisible(bShow);
		SetDisconntextAlign();
	}
}

void SortMapStat(sMapStatistics** const ppBest, sMapStatistics* pMapStat)
{
	for (int i = 0; i < 20; ++i)
	{
		if (i == 17)
			continue;
		sMapStatistics* pStat = &pMapStat[i];
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(
			(pStat->bMap <= 127 || pStat->bMap == 253 ? pStat->bMap
													  : pStat->bMap - 128) |
			0x28000000);
		if (!pCourse || !pCourse->c.Final)
			continue;
		unsigned char difficulty = pCourse->Difficulty;
		if (ppBest[difficulty - 1] != NULL)
		{
			if (pStat->cBestScore == 0x7f)
			{
				continue;
			}
			if (pStat->cBestScore < ppBest[difficulty - 1]->cBestScore)
			{
			}
			else if (pStat->cBestScore == ppBest[difficulty - 1]->cBestScore)
			{
				if (!pStat->i64MaxPang ||
					pStat->i64MaxPang <= ppBest[difficulty - 1]->i64MaxPang)
					continue;
			}
			else
			{
				continue;
			}
		}
		ppBest[difficulty - 1] = pStat;
	}
}

void FrUserInfoForm::SetContent(bool bRefresh)
{
	if (m_userInfo.info.dwUID == -1)
		return;

	m_bRefresh = bRefresh;

	const sPangYaUserInfo* pInfo = &m_userInfo.info;

	sPangYaUserStatistics* pStat = NULL;
	sTrophyStatistics* pTrophy = NULL;
	std::list<sSpecialTrophy>* pSpecialTrophy = NULL;
	std::list<sGuildTrophy>* pGuildTrophy = NULL;

	memset(m_pBestScoreMap, 0, sizeof(m_pBestScoreMap));

	if (m_seasonType == SEASON_CURRENT)
		SortMapStat(m_pBestScoreMap, m_userInfo.mapStat);
	else if (m_seasonType == SEASON_TOTAL)
		SortMapStat(m_pBestScoreMap, m_totalSeason.mapStatistics);

	SelectContents(m_seasonType, &pStat, &pTrophy, &pSpecialTrophy,
		&pGuildTrophy);

	m_pGeneralEdit[0]->SetLine(1, m_userInfo.info.sNick, 0xff000000, false, 0);

	UpdateEditSchoolAndGuild(m_seasonType, pInfo);
	UpdateEditExpAndMannerInfo(m_seasonType, pStat);
	UpdateEditUserLocation();

	UpdateLevelTextImg(pStat->Level);
	UpdateEditSpecialInfo(m_seasonType, pStat, pTrophy);

	SetMatchHistoryContents();

	char text[128] = { 0 };
	const char* noRecord = "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd";
	const char* none = "\xbe\xf8\xc0\xbd";

	unsigned long visitTime = pStat->dwVisitTime;
	int hour = visitTime / 3600;
	int minute = (visitTime - hour * 3600) / 60;

	if (hour == 0)
		sprintf(text, "%d \xba\xd0", minute);
	else if (hour < 24)
		sprintf(text, "%d \xbd\xc3\xb0\xa3 %d \xba\xd0", hour, minute);
	else
		sprintf(text, "%d \xc0\xcf %d \xbd\xc3\xb0\xa3 %d \xba\xd0", hour / 24,
			hour % 24, minute);
	m_pRecordEdit[0]->SetLine(1, text, 0xff000000, false, 0);

	sprintf(text, "%d ", (int)(pStat->dwDistance * 0.0000228595f));
	m_pRecordEdit[3]->SetLine(1, text, 0xff000000, false, 0);

	if (pStat->dwDistance == 0)
		strcpy(text, noRecord);
	else
		sprintf(text, "%.1f\xb8\xb8 y ", pStat->dwDistance * 0.0001f);
	m_pRecordEdit[4]->SetLine(1, text, 0xff000000, false, 0);

	if (pStat->dwDrive + pStat->dwPutt == 0)
	{
		strcpy(text, noRecord);
	}
	else
	{
		RemakeNumber(pStat->dwDrive + pStat->dwPutt, text);
		strcat(text, " \xc5\xb8");
	}
	m_pRecordEdit[1]->SetLine(1, text, 0xff000000, false, 0);

	if (pStat->dwHole == 0)
	{
		strcpy(text, noRecord);
	}
	else
	{
		RemakeNumber(pStat->dwHole, text);
		strcat(text, " \xc8\xa6");
	}
	m_pRecordEdit[2]->SetLine(1, text, 0xff000000, false, 0);

	if (Wabs(pStat->fLongest) < g_EPSILON)
		strcpy(text, noRecord);
	else
		sprintf(text, "%.2f y", pStat->fLongest);
	m_pRecordEdit[12]->SetLine(1, text, 0xff000000, false, 0);

	if (Wabs(pStat->fLongestPuttIn) < g_EPSILON)
		strcpy(text, noRecord);
	else
		sprintf(text, "%.2f y", pStat->fLongestPuttIn);
	m_pRecordEdit[13]->SetLine(1, text, 0xff000000, false, 0);

	if (Wabs(pStat->fLongestChipIn) < g_EPSILON)
		strcpy(text, noRecord);
	else
		sprintf(text, "%.2f y", pStat->fLongestChipIn);
	m_pRecordEdit[14]->SetLine(1, text, 0xff000000, false, 0);

	char bestScore[5][128];
	if (m_seasonType == SEASON_CURRENT || m_seasonType == SEASON_TOTAL)
	{
		for (int i = 4; i >= 0; --i)
		{
			if (m_pBestScoreMap[i] == NULL)
			{
				strcpy(bestScore[i], noRecord);
			}
			else
			{
				if (m_pBestScoreMap[i]->cBestScore == 0)
					sprintf(bestScore[i], "%+d [%I64dp]",
						m_pBestScoreMap[i]->cBestScore,
						m_pBestScoreMap[i]->i64MaxPang);
				else if (m_pBestScoreMap[i]->cBestScore == 0x7f)
					strcpy(bestScore[i], noRecord);
				else
					sprintf(bestScore[i], "%+d [%I64dp]",
						m_pBestScoreMap[i]->cBestScore,
						m_pBestScoreMap[i]->i64MaxPang);
			}
		}
	}

	if (m_seasonType == SEASON_CURRENT || m_seasonType == SEASON_TOTAL)
	{
		m_pRecordEdit[5]->SetLine(1, bestScore[4], 0xff000000, false, 0);
		m_pRecordEdit[6]->SetLine(1, bestScore[3], 0xff000000, false, 0);
	}
	m_pRecordEdit[7]->SetLine(1, bestScore[2], 0xff000000, false, 0);
	m_pRecordEdit[8]->SetLine(1, bestScore[1], 0xff000000, false, 0);
	m_pRecordEdit[9]->SetLine(1, bestScore[0], 0xff000000, false, 0);

	sprintf(text, "%d", pStat->wHoleInOne);
	m_pRecordEdit[10]->SetLine(1, text, 0xff000000, false, 0);

	sprintf(text, "%d", pStat->wAlbatross);
	m_pRecordEdit[11]->SetLine(1, text, 0xff000000, false, 0);

	bool bClassic;
	if (m_pRecordBtn[3]->GetStatus() == FrButton::PRESSED)
		bClassic = true;
	else
		bClassic = false;
	UpdateEditMapStatistics(m_seasonType, bClassic);

	int normalNum = 0;
	if (m_pNormalTrophyBox)
	{
		m_pNormalTrophyBox->ClearItem();

		for (int i = 12; i >= 0; --i)
		{
			if (pTrophy->Trophy[i][0] || pTrophy->Trophy[i][1] ||
				pTrophy->Trophy[i][2])
			{
				m_pNormalTrophyBox->AddItem((void*)i);
				normalNum += pTrophy->Trophy[i][0] + pTrophy->Trophy[i][1] +
					pTrophy->Trophy[i][2];
			}
		}
	}

	if (m_pNormalTrophyNum)
	{
		sprintf(text, "%d", normalNum);
		m_pNormalTrophyNum->SetLine(1, text, 0xff000000, false, 0);
	}

	int specialNum = 0;
	if (m_pSpecialTrophyBox)
	{
		m_pSpecialTrophyBox->ClearItem();

		for (std::list<sSpecialTrophy>::iterator it = pSpecialTrophy->begin();
			it != pSpecialTrophy->end(); ++it)
		{
			m_pSpecialTrophyBox->AddItem(&(*it));
			specialNum += (*it).count;
		}
	}

	if (m_pSpecialTrophyNum)
	{
		sprintf(text, "%d", specialNum);
		m_pSpecialTrophyNum->SetLine(1, text, 0xff000000, false, 0);
	}

	if (m_pGuildTrophyList)
	{
		m_pGuildTrophyList->ClearItem();

		for (std::list<sGuildTrophy>::reverse_iterator it =
				 pGuildTrophy->rbegin();
			it != pGuildTrophy->rend(); ++it)
			m_pGuildTrophyList->AddItem(&(*it));
	}

	if (m_userInfo.info.dwGuildId)
	{
		sprintf(text, "%s", m_userInfo.info.sGuild);
		m_pGuildEdit[0]->SetLine(1, text, 0xff000000, false, 0);

		sprintf(text, "%d", m_guildPang);
		m_pGuildEdit[1]->SetLine(1, text, 0xff000000, false, 0);

		sprintf(text, "%d", m_guildPoint);
		m_pGuildEdit[2]->SetLine(1, text, 0xff000000, false, 0);
	}
	else
	{
		strcpy(text, none);
		m_pGuildEdit[0]->SetLine(1, text, 0xff000000, false, 0);

		strcpy(text, none);
		m_pGuildEdit[1]->SetLine(1, text, 0xff000000, false, 0);

		strcpy(text, none);
		m_pGuildEdit[2]->SetLine(1, text, 0xff000000, false, 0);
	}
}

void FrUserInfoForm::BuildTotalStat()
{
	m_totalSeason.Clear();

	sPangYaUserStatistics& old = m_oldSeason.statistics;
	sPangYaUserStatistics& cur = m_userInfo.stat;

	m_totalSeason.statistics.Level = cur.Level;
	m_totalSeason.statistics.dwExp = cur.dwExp;
	m_totalSeason.statistics.dwGameCount = cur.dwGameCount;
	m_totalSeason.statistics.dwNoMannerGameCount = cur.dwNoMannerGameCount;
	m_totalSeason.statistics.nMannerCombo = cur.nMannerCombo;
	m_totalSeason.statistics.nMaxMannerCombo = cur.nMaxMannerCombo;
	m_totalSeason.statistics.dwTeamGames = cur.dwTeamGames + old.dwTeamGames;
	m_totalSeason.statistics.dwTeamWin = cur.dwTeamWin + old.dwTeamWin;
	m_totalSeason.statistics.dwHole = cur.dwHole + old.dwHole;
	m_totalSeason.statistics.dwMatchHole = cur.dwMatchHole + old.dwMatchHole;
	m_totalSeason.statistics.iTotalScore = cur.iTotalScore + old.iTotalScore;
	m_totalSeason.statistics.dwDrive = cur.dwDrive + old.dwDrive;
	m_totalSeason.statistics.dwPutt = cur.dwPutt + old.dwPutt;
	m_totalSeason.statistics.dwShotTime = cur.dwShotTime + old.dwShotTime;
	m_totalSeason.statistics.dwPangya = cur.dwPangya + old.dwPangya;
	m_totalSeason.statistics.dwFairway = cur.dwFairway + old.dwFairway;
	m_totalSeason.statistics.dwPuttIn = cur.dwPuttIn + old.dwPuttIn;
	m_totalSeason.statistics.dwOB = cur.dwOB + old.dwOB;
	m_totalSeason.statistics.i64TotalSkinsPang =
		cur.i64TotalSkinsPang + old.i64TotalSkinsPang;
	m_totalSeason.statistics.dwVisitTime = cur.dwVisitTime + old.dwVisitTime;
	m_totalSeason.statistics.dwDistance = cur.dwDistance + old.dwDistance;

	m_totalSeason.statistics.fLongest =
		cur.fLongest > old.fLongest ? cur.fLongest : old.fLongest;
	m_totalSeason.statistics.fLongestPuttIn =
		cur.fLongestPuttIn > old.fLongestPuttIn ? cur.fLongestPuttIn
												: old.fLongestPuttIn;
	m_totalSeason.statistics.fLongestChipIn =
		cur.fLongestChipIn > old.fLongestChipIn ? cur.fLongestChipIn
												: old.fLongestChipIn;

	m_totalSeason.statistics.wHoleInOne = cur.wHoleInOne + old.wHoleInOne;
	m_totalSeason.statistics.wAlbatross = cur.wAlbatross + old.wAlbatross;

	for (int i = 0; i < 5; ++i)
	{
		m_totalSeason.statistics.cBestScore[i] = 0x7f;

		if (old.cBestScore[i] != 0x7f)
		{
			m_totalSeason.statistics.cBestScore[i] = old.cBestScore[i];
			m_totalSeason.statistics.i64MaxPang[i] = old.i64MaxPang[i];
		}

		if (cur.cBestScore[i] != 0x7f)
		{
			if (m_totalSeason.statistics.cBestScore[i] > cur.cBestScore[i])
			{
				m_totalSeason.statistics.cBestScore[i] = cur.cBestScore[i];
				m_totalSeason.statistics.i64MaxPang[i] = cur.i64MaxPang[i];
			}
			else if (m_totalSeason.statistics.cBestScore[i] ==
				cur.cBestScore[i])
			{
				if (m_totalSeason.statistics.i64MaxPang[i] < cur.i64MaxPang[i])
					m_totalSeason.statistics.i64MaxPang[i] = cur.i64MaxPang[i];
			}
		}
	}

	for (int i = 0; i < 20; ++i)
	{
		m_totalSeason.mapStatistics[i].bMap = i;
		m_totalSeason.mapStatistics[i].cBestScore = 0x7f;

		if (m_oldSeason.mapStatistics[i].cBestScore != 0x7f)
			m_totalSeason.mapStatistics[i] = m_oldSeason.mapStatistics[i];

		if (m_userInfo.mapStat[i].cBestScore != 0x7f &&
			m_userInfo.mapStat[i].cBestScore <
				m_totalSeason.mapStatistics[i].cBestScore)
			m_totalSeason.mapStatistics[i] = m_userInfo.mapStat[i];
	}

	for (int i = 0; i < 20; ++i)
	{
		m_totalClassicMapStatistics[i].bMap = i;
		m_totalClassicMapStatistics[i].cBestScore = 0x7f;

		if (m_oldClassicMapStatistics[i].cBestScore != 0x7f)
			m_totalClassicMapStatistics[i] = m_oldClassicMapStatistics[i];

		if (m_classicMapStatistics[i].cBestScore != 0x7f &&
			m_classicMapStatistics[i].cBestScore <
				m_totalClassicMapStatistics[i].cBestScore)
			m_totalClassicMapStatistics[i] = m_classicMapStatistics[i];
	}

	sTrophyStatistics& oldTrophy = m_oldSeason.trophy;
	sTrophyStatistics& curTrophy = m_userInfo.trophy;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 13; ++j)
		{
			m_totalSeason.trophy.Trophy[j][i] +=
				curTrophy.Trophy[j][i] + oldTrophy.Trophy[j][i];
		}
	}

	m_totalSeason.specialTrophy.clear();

	for (std::list<sSpecialTrophy>::iterator itSpecial =
			 m_oldSeason.specialTrophy.begin();
		itSpecial != m_oldSeason.specialTrophy.end(); ++itSpecial)
		m_totalSeason.specialTrophy.push_back(*itSpecial);

	for (std::list<sSpecialTrophy>::iterator itSpecial =
			 m_specialTrophy.begin();
		itSpecial != m_specialTrophy.end(); ++itSpecial)
		m_totalSeason.specialTrophy.push_back(*itSpecial);

	m_totalSeason.guildTrophy.clear();

	for (std::list<sGuildTrophy>::iterator itGuild =
			 m_oldSeason.guildTrophy.begin();
		itGuild != m_oldSeason.guildTrophy.end(); ++itGuild)
		m_totalSeason.guildTrophy.push_back(*itGuild);

	for (std::list<sGuildTrophy>::iterator itGuild = m_guildTrophy.begin();
		itGuild != m_guildTrophy.end(); ++itGuild)
		m_totalSeason.guildTrophy.push_back(*itGuild);
}

unsigned long FrUserInfoForm::GetBestScoreColor(unsigned char index)
{
	if (m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID)
		return 0xff000000;

	bool bRed;
	if (m_userInfo.stat.cBestScore[index] == 0x7f)
		bRed = false;
	else if (Doc()->m_myInfo.stat.cBestScore[index] == 0x7f)
		bRed = true;
	else
		bRed = m_userInfo.stat.cBestScore[index] <
			Doc()->m_myInfo.stat.cBestScore[index];

	return bRed ? 0xffffa045 : 0xff000000;
}

unsigned long FrUserInfoForm::GetMaxPangColor(unsigned char index)
{
	if (m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID)
		return 0xffffffff;

	bool bRed = m_userInfo.stat.i64MaxPang[index] >
		Doc()->m_myInfo.stat.i64MaxPang[index];

	return bRed ? 0xffffa045 : 0xff000000;
}

unsigned long FrUserInfoForm::Get30sRecordColor(int index)
{
	if (m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID)
		return 0xff000000;

	sTrophyStatistics* pTheir = &m_userInfo.trophy;
	sTrophyStatistics* pMine = &Doc()->m_myInfo.trophy;
	int mine = 0, their = 0;

	for (int i = 0; i < 13; ++i)
	{
		mine += pMine->Trophy[i][index];
		their += pTheir->Trophy[i][index];
	}

	return their > mine ? 0xffffa045 : 0;
}

unsigned long FrUserInfoForm::GetColor(eTabType tab, unsigned long index)
{
	if (m_userInfo.info.dwUID == Doc()->m_myInfo.info.dwUID &&
		(tab != TAB_GENERAL || index != 5))
		return 0xff000000;

	bool bRed = false;

	switch (tab)
	{
	case TAB_GENERAL:
		if (index != 5)
			break;
		{
			if (m_userInfo.stat.dwGameCount == 0)
				return 0xff000000;

			unsigned char c = 0xff -
				(unsigned char)((float)m_userInfo.stat.dwNoMannerGameCount /
					m_userInfo.stat.dwGameCount * 255.0f);
			return 0xffff0000 | (c << 8) | c;
		}

	case TAB_RECORD:
		switch (index)
		{
		case 3:
			if (m_userInfo.stat.dwTeamGames == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwTeamGames == 0)
				bRed = true;
			else
				bRed = (float)m_userInfo.stat.dwTeamWin /
						m_userInfo.stat.dwTeamGames >
					(float)Doc()->m_myInfo.stat.dwTeamWin /
						Doc()->m_myInfo.stat.dwTeamGames;
			break;

		case 4:
			if (m_userInfo.stat.dwHole - m_userInfo.stat.dwMatchHole == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwHole -
					Doc()->m_myInfo.stat.dwMatchHole ==
				0)
				bRed = true;
			else
				bRed = (float)m_userInfo.stat.iTotalScore /
						(m_userInfo.stat.dwHole - m_userInfo.stat.dwMatchHole) <
					(float)Doc()->m_myInfo.stat.iTotalScore /
						(Doc()->m_myInfo.stat.dwHole -
							Doc()->m_myInfo.stat.dwMatchHole);
			break;

		case 5:
			if (m_userInfo.stat.dwDrive + m_userInfo.stat.dwPutt == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwDrive + Doc()->m_myInfo.stat.dwPutt == 0)
				bRed = true;
			else
				bRed = (float)m_userInfo.stat.dwShotTime /
						(m_userInfo.stat.dwDrive + m_userInfo.stat.dwPutt) <
					(float)Doc()->m_myInfo.stat.dwShotTime /
						(Doc()->m_myInfo.stat.dwDrive +
							Doc()->m_myInfo.stat.dwPutt);
			break;

		case 6:
			if (m_userInfo.stat.dwDrive == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwDrive == 0)
				bRed = true;
			else
				bRed =
					(float)m_userInfo.stat.dwPangya / m_userInfo.stat.dwDrive >
					(float)Doc()->m_myInfo.stat.dwPangya /
						Doc()->m_myInfo.stat.dwDrive;
			break;

		case 7:
			if (m_userInfo.stat.dwHole - m_userInfo.stat.dwMatchHole == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwHole -
					Doc()->m_myInfo.stat.dwMatchHole ==
				0)
				bRed = true;
			else
				bRed = (float)m_userInfo.stat.dwFairway /
						(m_userInfo.stat.dwHole - m_userInfo.stat.dwMatchHole) >
					(float)Doc()->m_myInfo.stat.dwFairway /
						(Doc()->m_myInfo.stat.dwHole -
							Doc()->m_myInfo.stat.dwMatchHole);
			break;

		case 8:
			if (m_userInfo.stat.dwPutt == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwPutt == 0)
				bRed = true;
			else
				bRed =
					(float)m_userInfo.stat.dwPuttIn / m_userInfo.stat.dwPutt >
					(float)Doc()->m_myInfo.stat.dwPuttIn /
						Doc()->m_myInfo.stat.dwPutt;
			break;

		case 9:
			if (m_userInfo.stat.dwDrive + m_userInfo.stat.dwPutt == 0)
			{
				bRed = false;
				break;
			}
			if (Doc()->m_myInfo.stat.dwDrive + Doc()->m_myInfo.stat.dwPutt == 0)
				bRed = true;
			else
				bRed = (float)m_userInfo.stat.dwOB /
						(m_userInfo.stat.dwDrive + m_userInfo.stat.dwPutt) <
					(float)Doc()->m_myInfo.stat.dwOB /
						(Doc()->m_myInfo.stat.dwDrive +
							Doc()->m_myInfo.stat.dwPutt);
			break;
		}
		break;

	case TAB_SPECIAL:
		switch (index)
		{
		case 0:
			bRed =
				m_userInfo.stat.dwVisitTime > Doc()->m_myInfo.stat.dwVisitTime;
			break;

		case 4:
			bRed = m_userInfo.stat.dwDistance > Doc()->m_myInfo.stat.dwDistance;
			break;

		case 1:
			bRed = m_userInfo.stat.dwDrive + m_userInfo.stat.dwPutt >
				Doc()->m_myInfo.stat.dwDrive + Doc()->m_myInfo.stat.dwPutt;
			break;

		case 2:
			bRed = m_userInfo.stat.dwHole > Doc()->m_myInfo.stat.dwHole;
			break;

		case 12:
			bRed = m_userInfo.stat.fLongest > Doc()->m_myInfo.stat.fLongest;
			break;

		case 13:
			bRed = m_userInfo.stat.fLongestPuttIn >
				Doc()->m_myInfo.stat.fLongestPuttIn;
			break;

		case 14:
			bRed = m_userInfo.stat.fLongestChipIn >
				Doc()->m_myInfo.stat.fLongestChipIn;
			break;

		case 10:
			bRed = m_userInfo.stat.wHoleInOne > Doc()->m_myInfo.stat.wHoleInOne;
			break;

		case 11:
			bRed = m_userInfo.stat.wAlbatross > Doc()->m_myInfo.stat.wAlbatross;
			break;
		}
		break;
	}

	return bRed ? 0xffffa045 : 0;
}

void FrUserInfoForm::OnPetInit(int param)
{
	m_pPetArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnPetOwnerDraw()
{
	g_view->Clear(0, 2);

	if (m_pPetLoadGauge && m_pPetLoadGauge->IsVisible())
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;

	WRect area = m_pPetArea->GetRect();

	if (m_userInfo.userEquip.tidSkin[0])
	{
		IFF_STRUCT::sSkin* pSkin =
			ItemManager()->FindSkin(m_userInfo.userEquip.tidSkin[0]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
			{
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

				while (dst.y < area.y + area.h)
				{
					while (dst.x < area.x + area.w)
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

	if (m_userInfo.userEquip.tidSkin[1])
	{
		IFF_STRUCT::sSkin* pSkin =
			ItemManager()->FindSkin(m_userInfo.userEquip.tidSkin[1]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
				pGI->DrawTexture(pBmp, area, 0xffffffff, 0);
		}
	}

	if (m_userInfo.userEquip.tidSkin[2])
	{
		IFF_STRUCT::sSkin* pSkin =
			ItemManager()->FindSkin(m_userInfo.userEquip.tidSkin[2]);
		if (pSkin)
		{
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->Tex);
			if (pBmp)
				pGI->DrawTexture(pBmp, area, 0xffffffff, 0);
		}
	}
}

void FrUserInfoForm::OnPetLoadInit(int param)
{
	m_pPetLoadGauge = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	if (m_pPetLoadGauge)
	{
		m_pPetLoadGauge->SetRange(0, 120, 0);
		m_pPetLoadGauge->SetPos(0);
		m_pPetLoadGauge->SetDestPos(120, false);
		m_pPetLoadGauge->SetVisible(true);
	}
}

void FrUserInfoForm::ShowPet()
{
	m_anotherHand.Enter();
	HidePet(false);
	if (!m_pPetArea)
	{
		m_anotherHand.Leave();
		return;
	}
	WRect rect = m_pPetArea->GetRect();
	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f,
		rect.y + rect.h * 0.5f, 0.35f, true);
	m_pExhibition->SetDistance(g_view->GetWidth() * 0.0234375f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);
	m_pPartTidList = new CPartTidList;
	sCharacterInfo* info = &m_userInfo.charInfo;
	if (!m_pPartTidList->SetTids(info, 0xff) || !m_pPartTidList->IsComboValid())
		m_pPartTidList->SetDefaultTids();
	m_pExhibition->SetModel(*m_pPartTidList, info->tidAuxParts, NULL, 3.f, 0);
	if (!m_userInfo.info.angelicWings && m_userInfo.info.angelicWingsEffect)
		m_pExhibition->AttachAngelWingFx();
	m_anotherHand.Leave();
}

void FrUserInfoForm::OnCustomProc(float delta)
{
	if (m_anotherHand.IsWorking(0))
	{
		Sleep(20);
	}
	else
	{
		m_anotherHand.Enter();
		if (m_pExhibition)
			m_pExhibition->Process(delta, true);
		m_anotherHand.Leave();
	}
	if ((!(Doc()->m_curChannel.Type & 0x80) ||
			m_userInfo.info.dwUID == MyUID()) &&
		g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) &&
		!g_input->Get("RALT", true))
	{
		g_audio->PlaySfx("ui_icon_click");
		switch (m_tabType)
		{
		case TAB_GENERAL:
			SetFormA(TAB_SPECIAL);
			break;
		case TAB_SPECIAL:
			SetFormA(TAB_RECORD);
			break;
		case TAB_RECORD:
			SetFormA(TAB_TROPHY);
			break;
		case TAB_TROPHY:
			SetFormA(TAB_GUILD);
			break;
		case TAB_GUILD:
			SetFormA(TAB_GENERAL);
			break;
		}
	}
	if (m_userInfo.userEquip.tidSkin[0])
	{
		for (int i = 0; i < 2; ++i)
		{
			float speed = i == 0 ? 138.f : 228.f;
			switch (m_skinScrollType[i])
			{
			case 0:
				m_skinScroll[i] = 0;
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
}

void FrUserInfoForm::SetVisible(FrWnd** const ppWnd, int num, bool bVisible)
{
	for (int i = 0; i < num; ++i)
	{
		if (ppWnd[i])
			ppWnd[i]->SetVisible(bVisible);
	}
}

void FrUserInfoForm::SetVisible(eTabType type, bool bVisible)
{
	switch (type)
	{
	case TAB_GENERAL:
	{
		SetVisible((FrWnd**)m_pGeneralEdit, 8, bVisible);
		if (m_pLevelArea)
			m_pLevelArea->SetVisible(bVisible);
		if (m_pClubArea)
			m_pClubArea->SetVisible(bVisible);
		if (m_pBackArea[0])
			m_pBackArea[0]->SetVisible(bVisible);
		if (m_pQuestionMark)
		{
			if (bVisible && !IsHideDisconnectRate(&m_userInfo.stat))
				SetDisconnQuestionMark(false);
			else
				SetDisconnQuestionMark(bVisible);
		}
		if (m_pRmrVisitorBtn)
		{
			int bVisitor = 0;
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x231, (int)&bVisitor, 2, 3, 0, 0));
			if (!bVisitor || m_userInfo.info.dwUID == MyUID())
				m_pRmrVisitorBtn->SetVisible(false);
			else
				m_pRmrVisitorBtn->SetVisible(bVisible);
		}
		break;
	}
	case TAB_SPECIAL:
	{
		SetVisible((FrWnd**)m_pSpecialEdit, 16, bVisible);
		for (int j = 0; j < 5; ++j)
			m_pMatchHistoryBtn[j]->SetVisible(false);
		m_pSpecListBox->SetVisible(bVisible);
		break;
	}
	case TAB_RECORD:
		if (m_pRecordBtn[0])
			m_pRecordBtn[0]->SetVisible(bVisible);
		if (m_pRecordBtn[2])
			m_pRecordBtn[2]->SetVisible(bVisible);
		if (m_pRecordBtn[3])
			m_pRecordBtn[3]->SetVisible(bVisible);
		if (!bVisible)
		{
			SetVisible((FrWnd**)m_pRecordEdit, 15, false);
			m_pScoreList->SetVisible(false);
			m_pRecTotalListBox->SetVisible(false);
			if (m_pRecTotalListBox)
				m_pRecTotalListBox->SetVisible(false);
			if (m_pBackArea[3])
				m_pBackArea[3]->SetVisible(false);
		}
		else
		{
			ChangeRecordType();
		}
		break;
	case TAB_TROPHY:
		if (m_pTrophyBtn[0])
			m_pTrophyBtn[0]->SetVisible(bVisible);
		if (m_pTrophyBtn[1])
			m_pTrophyBtn[1]->SetVisible(bVisible);
		if (!bVisible)
		{
			if (m_pNormalTrophyNum)
				m_pNormalTrophyNum->SetVisible(false);
			if (m_pSpecialTrophyNum)
				m_pSpecialTrophyNum->SetVisible(false);
			if (m_pSpecialTrophyBox)
				m_pSpecialTrophyBox->SetVisible(false);
			if (m_pNormalTrophyBox)
				m_pNormalTrophyBox->SetVisible(false);
			if (m_pNoTrophy)
				m_pNoTrophy->SetVisible(false);
			if (m_pBackArea[4])
				m_pBackArea[4]->SetVisible(false);
			if (m_pBackArea[5])
				m_pBackArea[5]->SetVisible(false);
		}
		else
		{
			ChangeTrophyType();
		}
		break;
	case TAB_GUILD:
	{
		SetVisible((FrWnd**)m_pGuildEdit, 3, bVisible);
		m_pMyGuild->SetVisible(bVisible);
		if (m_pBackArea[6])
			m_pBackArea[6]->SetVisible(bVisible);
		if (m_pGuildTrophyList)
			m_pGuildTrophyList->SetVisible(bVisible);
		m_pGuildNoTrophy->SetVisible(
			bVisible && m_pGuildTrophyList->m_itemList.size() == 0);
		break;
	}
	}
}

void FrUserInfoForm::OnCommentInit(int param)
{
	m_pComment = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pComment)
		m_pComment->SetVisible(false);
}

void FrUserInfoForm::OnEditInit_HoverOn(int param)
{
	if (!m_pComment)
		return;
	FrEdit* edit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (!edit)
		return;
	const char* text = "";
	if (strlen(text) == 0)
		return;
	m_pComment->ClearLine();
	m_pComment->AddText(text, false, false);
	m_pComment->MoveWindow(WPoint(edit->GetRect().x, edit->GetRect().y));
	m_pComment->SetVisible(true);
}

void FrUserInfoForm::OnEditInit_HoverOff(int param)
{
	if (m_pComment)
		m_pComment->SetVisible(false);
}

void FrUserInfoForm::OnScoreListInit(int param)
{
	m_pScoreList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pScoreList)
	{
		m_pScoreList->SetVisible(false);
		static char* fileName[] = { "[font_wind.jpg" };
		WTITLEFONT info;
		memset(&info, 0, sizeof(info));
		info.filename = fileName;
		info.fontw = 16;
		info.fonth = 16;
		info.numPages = 1;
		info.texw = 128;
		info.texh = 64;
		info.pCharSet = "1234567890ym%-.?/:";
		m_pScoreFont = g_resrcmng->GetTitleFont();
		m_pScoreFont->Create(&info);
	}
}

void FrUserInfoForm::OnScoreListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	sMapStatistics* stat = (sMapStatistics*)item->pData;
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
		(stat->bMap <= 0x7f || stat->bMap == 0xfd ? stat->bMap
												  : stat->bMap - 0x80) |
		0x28000000);
	if (course)
	{
		const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap("CAPTION",
			MakeStr("room_%s", course->c.Icon));
		gdi->DrawTexture(bitmap,
			WRect(item->pos.x, item->pos.y + 7, (float)bitmap->Width(),
				(float)bitmap->Height()),
			stat->cBestScore == 0x7f ? 0x60000000 : 0xffffffff, 0);
	}
	if (stat->cBestScore != 0x7f)
	{
		const char* score = MakeStr("%d", stat->cBestScore);
		m_pScoreFont->Print(g_view, item->pos.x + 130, item->pos.y + 20, score,
			0, 0xffffffff, NULL);
		if (stat->tidChar)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(stat->tidChar);
			if (common)
			{
				const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap(
					"ITEMS", MakeStr("bar_%s", common->Icon));
				if (bitmap)
					gdi->DrawTexture(bitmap,
						WRect(item->pos.x + 79, item->pos.y + 13,
							(float)bitmap->Width(), (float)bitmap->Height()),
						0xffffffff, 0);
			}
		}
		else
		{
			m_pScoreFont->Print(g_view, item->pos.x + 88, item->pos.y + 20, "-",
				0, 0xffffffff, NULL);
		}
		m_pScoreFont->Print(g_view, item->pos.x + 188, item->pos.y + 20,
			MakeStr("%I64d", stat->i64MaxPang), 0, 0xffffffff, NULL);
	}
	else
	{
		m_pScoreFont->Print(g_view, item->pos.x + 88, item->pos.y + 20, "-", 0,
			0xffffffff, NULL);
		m_pScoreFont->Print(g_view, item->pos.x + 140, item->pos.y + 20, "-", 0,
			0xffffffff, NULL);
		m_pScoreFont->Print(g_view, item->pos.x + 200, item->pos.y + 20, "-", 0,
			0xffffffff, NULL);
	}
}

bool FrUserInfoForm::OnAddFriendFormResult(int result, FrForm* pForm)
{
	m_pAddFriendDlg = NULL;
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (AfxGetTask()->GetActor("GolfRule") && !m_bChatMsgOpen)
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(NULL, 98, 0, 0, 0, 0, 0);
	}
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		if (AfxGetTask()->GetActor("AvatarChat") && !m_bChatMsgOpen)
			AfxGetTask()->GetActor("AvatarChat")
				<< MsgObject(NULL, 98, 0, 0, 0, 0, 0);
	}
	return true;
}

void FrUserInfoForm::OnTitleSeasonVisible()
{
	m_seasonType = SEASON_CURRENT;
	m_pTitleBtn[0]->SetStatus(FrButton::PRESSED);
	m_pTitleBtn[1]->SetStatus(FrButton::NORMAL);
	SetContent(true);
	SetTrophyStatic();
	SetSpecListBoxItem(SEASON_CURRENT);
}

void FrUserInfoForm::SetRequestFriendData(const char* nick, unsigned long uid)
{
	if (!nick)
		return;
	if (m_pAddFriendDlg)
	{
		m_pAddFriendDlg->SetNick(nick);
		m_pAddFriendDlg->SetFindUID(uid);
	}
}

CUserInfo::CUserInfo()
	: m_pDlg(NULL),
	  m_guid(-1),
	  m_uid(-1),
	  m_bCheckTime(false),
	  m_bRequest(false)
{
}

CUserInfo::~CUserInfo()
{
}

void CUserInfo::Clear()
{
	if (m_pDlg)
		m_pDlg->Clear();
}

bool CUserInfo::IsVisible()
{
	return m_pDlg ? true : false;
}

void CUserInfo::SetInfo(unsigned long uid, unsigned long guid, bool bCheckTime,
	bool bOption, bool bMsnControl, bool bControlOff, std::string nickname)
{
	if (!OnlinePlay())
		return;
	if (uid == -1)
		return;
	if (!m_pDlg)
	{
		m_pDlg = CreateForm<FrUserInfoForm>(g_pFresh->GetManager(), this,
			"userinfo", NULL);
		if (!m_pDlg)
			return;
		m_pDlg->Open((FRESH_PFN_RESULT)&CUserInfo::OnUserInfoFormResult, 1);
		m_pDlg->EnableDrag(false);
		m_pDlg->SetTopmost(bControlOff);
		m_pDlg->SetMsnControl(bMsnControl);
		m_pDlg->SetControlOff(bControlOff);
		m_pDlg->FindNextTopFocus(false);
		m_nickname = nickname;
	}
	m_guid = guid;
	m_uid = uid;
	m_bRequest = true;
	m_bCheckTime = bCheckTime;
	std::map<unsigned long, sUserInfoTime>::iterator it;
	int bFind = Doc()->FindUserInfoTimeUID(uid, it);
	if (bFind)
	{
		if (!bCheckTime || g_CurrentTime - it->second.dwUpdate < 60000)
		{
			if (MyUID() == m_uid)
			{
				std::map<unsigned int, sCharacterInfo>::iterator itChar =
					Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
				if (itChar != Doc()->m_charMap.end())
					it->second.info.charInfo = itChar->second;
				if (!IS_KINDOF(CGolfTask, AfxGetTask()))
					it->second.info.stat.i64Pang = Doc()->m_myInfo.stat.i64Pang;
				if (IsLocalContent(S4_CARD_SYSTEM))
				{
					it->second.info.stat.dwExp = Doc()->m_myInfo.stat.dwExp;
					it->second.info.stat.Level = Doc()->m_myInfo.stat.Level;
				}
				it->second.info.userEquip = Doc()->m_myInfo.userEquip;
			}
			if (bMsnControl)
				it->second.info.roomIndex = 0xffff;
			m_pDlg->SetInfo(&it->second);
			m_bRequest = false;
			return;
		}
	}
	if (bOption && nickname.size())
	{
		WSendPacket packet((enumClientPacket)0x2f);
		packet.Encode4(uid);
		packet.Encode1(5);
		packet.Send(TO_GAME);
	}
}

void CUserInfo::Refresh()
{
	if (m_pDlg)
		m_pDlg->SetContent(true);
}

void CUserInfo::Process(float delta)
{
	if (m_bRequest)
	{
		SetInfo(m_uid, m_guid, m_bCheckTime, false, false, false, "");
		if (m_bRequest &&
			WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN))
			SetInfo(m_uid, m_guid, m_bCheckTime, false, true, false, "");
	}
	if (m_pDlg)
		m_pDlg->OnCustomProc(delta);
}

bool CUserInfo::OnUserInfoFormResult(int result, FrForm* pForm)
{
	m_bRequest = false;
	m_guid = -1;
	m_uid = -1;
	m_pDlg = NULL;
	return true;
}

void CUserInfo::Close()
{
	if (m_pDlg)
		m_pDlg->Close(FrNONE, true);
}

bool CUserInfo::IsRecvedData(unsigned char season)
{
	if (m_pDlg)
	{
		unsigned long flag = 0;
		if (season == 0)
			flag = 2;
		else if (season == 5)
			flag = 4;
		m_pDlg->RecvedSeasonData(flag);
		if (m_pDlg->IsTotalTab())
		{
			if (!m_pDlg->IsRecvSeason(2))
				m_pDlg->RequestSeasonData(0);
			else if (!m_pDlg->IsRecvSeason(4))
				m_pDlg->RequestSeasonData(5);
			else
				return true;
		}
		else
			return true;
	}
	return false;
}

void FrUserInfoForm::OnRecAwardBackInit(int param)
{
	m_pBackArea[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUserInfoForm::OnGhostInit(int param)
{
	m_pGhostBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pGhostBtn->Enable(false);
	if (!(Doc()->m_curChannel.Type & 0x800))
	{
		if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		{
			if (!stricmp("ROOMLIST", g_pFresh->GetManager()->GetLayoutID()) &&
				Doc()->m_myInfo.roomIndex == 0xffff)
				m_pGhostBtn->Enable(true);
		}
	}
}

void FrUserInfoForm::OnGhostdUp()
{
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify", NULL);
	form->SetTimeLimit(5.f);
	form->SetMessage(
		"\xbc\xad\xba\xf1\xbd\xba \xc1\xd8\xba\xf1 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
		false);
	form->Open(NULL, 3);
}

void FrUserInfoForm::OnBtn_RmrVisitorInit(int param)
{
	m_pRmrVisitorBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUserInfoForm::OnBtn_RmrVisitorBtnUp()
{
	m_anotherHand.WaitUntilWorkDone(9999);
	Doc()->m_replayTargetUID = m_userInfo.info.dwUID;
	Doc()->SetRmrMode(MyUID() != Doc()->m_replayTargetUID ? 5 : 1);
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x222, 0, 0, 0, 0, 0));
}

void FrUserInfoForm::RequestSeasonData(unsigned char season)
{
	if (m_requestSeason)
		return;
	WSendPacket packet((enumClientPacket)0x2f);
	packet.Encode4(m_userInfo.info.dwUID);
	packet.Encode1(season);
	packet.Send(TO_GAME);
	m_requestSeason = 1;
}

void FrUserInfoForm::RecvedSeasonData(unsigned long season)
{
	m_recvSeason |= season;
	m_requestSeason = 0;
}

int FrUserInfoForm::IsTotalTab()
{
	return m_seasonType == SEASON_TOTAL;
}
