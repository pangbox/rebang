#pragma once

#include "anotherhand.h"

struct sUserInfoTime;
class CExhibition;
class CPartTidList;
class FrAddFriendDlg;
class WTitleFont;

class FrUserInfoForm : public FrForm, public CHandOwner
{
	DECLARE_OBJECT(FrUserInfoForm)

	enum eSeasonType
	{
		SEASON_CURRENT,
		SEASON_TOTAL,
	};

	enum eTabType
	{
		TAB_GENERAL,
		TAB_SPECIAL,
		TAB_RECORD,
		TAB_TROPHY,
		TAB_GUILD,
		TAB_NONE,
	};

public:
	FrUserInfoForm();
	virtual ~FrUserInfoForm();
	void SetRequestFriendData(const char*, unsigned long);
	void RecvedSeasonData(unsigned long);
	int IsTotalTab();
	virtual bool Close(eFormRet, bool);
	void RequestSeasonData(unsigned char);
	void Clear();
	bool OnAddFriendFormResult(int, FrForm*);
	void OnCustomProc(float);
	void SetContent(bool);
	void OnTitleSeasonVisible();
	void SetInfo(sUserInfoTime*);

protected:
	void OnCloseUp();
	void SetVisible(FrWnd** const ppWnd, int num, bool bVisible);
	void OnEditInit_HoverOff(int);
	void OnTitleSeasonBtnInit(int);
	void OnTitleTotalBtnInit(int);
	void SetTrophyStatic();
	void OnGeneralInfoInit(int);
	void OnSpecialInfoInit(int);
	void OnRecordInfoInit(int);
	void OnTrophyInfoInit(int);
	void OnGuildInfoInit(int);
	void OnGeneralBackInit(int);
	void OnRecCourseBackInit(int);
	void OnTroNormalBackInit(int);
	void OnTroSpecialBackInit(int);
	void OnGuildBackInit(int);
	void OnEditInit_GI_ID(int);
	void OnEditInit_GI_SCHOOL(int);
	void OnEditInit_GI_CLUB(int);
	void OnEditInit_GI_EXP(int);
	void OnEditInit_GI_CONNSTATUS(int);
	void OnEditInit_GI_DISCONN(int);
	void OnEditInit_GI_COMBO(int);
	void OnEditInit_GI_QUIZLEVEL(int);
	void OnEditInit_SI_TROPHY_GOLD(int);
	void OnEditInit_SI_TROPHY_SILVER(int);
	void OnEditInit_SI_TROPHY_BRONZE(int);
	void OnEditInit_SI_MATCH_WIN_PERCENTAGE(int);
	void OnEditInit_SI_AVG_SCORE(int);
	void OnEditInit_SI_MEAN_SHOTTIME(int);
	void OnEditInit_SI_PANGYA_PERCENTAGE(int);
	void OnEditInit_SI_FAIRWAY_PERCENTAGE(int);
	void OnEditInit_SI_PUTT_PERCENTAGE(int);
	void OnEditInit_SI_OB_PERCENTAGE(int);
	void OnEditInit_SI_TOTAL_SKINS_PANG(int);
	void OnEditInit_RI_TOT_PLAYTIME(int);
	void OnEditInit_RI_TOT_SHOTS(int);
	void OnEditInit_RI_TOT_HOLES(int);
	void OnEditInit_RI_TOT_DRIVE(int);
	void OnEditInit_RI_EARTH_ROUND(int);
	void OnEditInit_RI_BEST_SCORE_5(int);
	void OnEditInit_RI_BEST_SCORE_4(int);
	void OnEditInit_RI_BEST_SCORE_3(int);
	void OnEditInit_RI_BEST_SCORE_2(int);
	void OnEditInit_RI_BEST_SCORE_1(int);
	void OnEditInit_RI_TOT_HOLEINONES(int);
	void OnEditInit_RI_TOT_ALBATROSS(int);
	void OnEditInit_RI_LONGEST_DRIVE(int);
	void OnEditInit_RI_LONGEST_PUTT(int);
	void OnEditInit_RI_LONGEST_CHIPIN(int);
	void OnEditInit_GUILD_NAME(int);
	void OnEditInit_GUILD_PANG(int);
	void OnEditInit_GUILD_MYPERCENT(int);
	void OnAreaInit_GI_Level(int);
	void OnAreaInit_GI_ClUB(int);
	void OnAreaInit_Question_mark(int);
	void OnAreaDraw_GI_ClUB(int);
	void OnRecordNormalBtnInit(int);
	void OnRecordCourseBtnInit(int);
	void OnRecordClassicBtnInit(int);
	void OnEditInit_SI_MATCH_HISTORY1(int);
	void OnEditInit_SI_MATCH_HISTORY2(int);
	void OnEditInit_SI_MATCH_HISTORY3(int);
	void OnEditInit_SI_MATCH_HISTORY4(int);
	void OnEditInit_SI_MATCH_HISTORY5(int);
	void OnSpecListboxInit(int);
	void OnMatchHistoryBtn1Init(int);
	void OnMatchHistoryBtn2Init(int);
	void OnMatchHistoryBtn3Init(int);
	void OnMatchHistoryBtn4Init(int);
	void OnMatchHistoryBtn5Init(int);
	void OnSpecListboxOwnerDraw(int);
	void OnRecTotalListboxInit(int);
	void OnRecTotalListboxOwnerDraw(int);
	void OnTrophyNormalNumInit(int);
	void OnTrophySpecialNumInit(int);
	void OnTrophyNormalBtnInit(int);
	void OnTrophySpecialBtnInit(int);
	void ChangeRecordType();
	void ChangeTrophyType();
	void OnListBoxInit_TRI_SPECIALBOX(int);
	void OnListBoxInit_TRI_NORMALBOX(int);
	void OnStaticInit_TRI_NOTROPHY(int);
	void OnMyGuildInit(int);
	void OnMyGuildOwnerDraw();
	void OnGuildTrophyListInit(int);
	void OnStaticInit_GUILD_NOTROPHY(int);
	void OnWhisperInit(int);
	void OnNickInit(int);
	void OnNickOwnerDraw(int);
	void OnFriendInit(int);
	void OnIgnoreInit(int);
	void RefreshAllUccClothes();
	void HidePet(bool);
	void OnPetInit(int);
	void OnPetLoadInit(int);
	void ShowPet();
	void OnCommentInit(int);
	void OnEditInit_HoverOn(int);
	void OnScoreListInit(int);
	void OnRecAwardBackInit(int);
	void OnBtn_RmrVisitorInit(int);
	void OnRecordNormalBtnUp();
	void OnMatchHistoryBtn1Up();
	void OnMatchHistoryBtn2Up();
	void OnMatchHistoryBtn3Up();
	void OnMatchHistoryBtn4Up();
	void OnMatchHistoryBtn5Up();
	void OnTrophyNormalBtnUp();
	void OnTrophySpecialBtnUp();
	void OnListBoxOwnerDraw_TRI_SPECIALBOX(int);
	void OnListBoxOwnerDraw_TRI_NORMALBOX(int);
	void OnGuildTrophyListOwnerDraw(int);
	void OnWhisperUp();
	virtual void OnProc(const float delta);
	virtual void OnAnotherHand(int, void*);
	unsigned long GetBestScoreColor(unsigned char);
	unsigned long GetMaxPangColor(unsigned char);
	unsigned long Get30sRecordColor(int);
	unsigned long GetColor(eTabType, unsigned long);
	void OnPetOwnerDraw();
	void SetVisible(eTabType, bool);
	void OnScoreListOwnerDraw(int);
	void OnGhostInit(int);
	void OnGhostdUp();
	void OnBtn_RmrVisitorBtnUp();
	void OnRecordCourseBtnUp();
	void OnRecordClassicBtnUp();
	void OnFriendUp();
	virtual bool OnInit();
	void SetFormA(eTabType);
	void OnGeneralInfoDown();
	void OnSpecialInfoDown();
	void OnRecordInfoDown();
	void OnTrophyInfoDown();
	void OnGuildInfoDown();
	void OnTitleSeasonBtnUp();
	void OnTitleTotalBtnUp();
	void OnIgnoreUp();
	void BuildTotalStat();

private:
	void SetIgnoreButtonImg(bool);
	void SelectContents(eSeasonType, sPangYaUserStatistics**,
		sTrophyStatistics**, std::list<sSpecialTrophy>**,
		std::list<sGuildTrophy>**);
	unsigned long GetSeasonTextColor(eSeasonType);
	void UpdateLevelTextImg(unsigned char);
	void MatchHistoryUserBtnUp(int);
	void MsnDisconExeception(float, FrListItem*);
	void SetMatchHistoryContents();
	void UpdateEditSpecialInfo(eSeasonType, const sPangYaUserStatistics*,
		const sTrophyStatistics*);
	void SetDisconntextAlign();
	void SetDisconnQuestionMark(bool);
	void SetSpecListBoxItem(eSeasonType);
	void UpdateEditSchoolAndGuild(eSeasonType, const sPangYaUserInfo*);
	void UpdateEditExpAndMannerInfo(eSeasonType, const sPangYaUserStatistics*);
	void UpdateEditMapStatistics(eSeasonType, bool);
	void UpdateEditUserLocation();

public:
	struct sSeasonStat
	{
		sPangYaUserStatistics statistics;
		sMapStatistics mapStatistics[20];
		sTrophyStatistics trophy;
		std::list<sSpecialTrophy> specialTrophy;
		std::list<sGuildTrophy> guildTrophy;
		void Clear()
		{
			memset(&statistics, 0, sizeof(statistics));
			memset(mapStatistics, 0, sizeof(mapStatistics));
			for (int i = 0; i < 20; ++i)
			{
				mapStatistics[i].cBestScore = 0x7f;
			}
			memset(&trophy, 0, sizeof(trophy));
			specialTrophy.clear();
			guildTrophy.clear();
		}
	};

	void SetMsnControl(bool bSet) { m_bMsnControl = bSet; }
	void SetControlOff(bool bSet) { m_bControlOff = bSet; }

	bool IsOpenAddFriendDlg() const { return m_pAddFriendDlg ? true : false; }

protected:
	eSeasonType m_seasonType;
	eTabType m_tabType;
	FrButton* m_pTitleBtn[2];
	FrButton* m_pTabBtn[5];
	FrArea* m_pBackArea[7];
	FrEdit* m_pGeneralEdit[8];
	FrArea* m_pLevelArea;
	FrArea* m_pClubArea;
	FrArea* m_pQuestionMark;
	WRect m_disconnRect;
	FrEdit* m_pSpecialEdit[16];
	FrListBox* m_pSpecListBox;
	const Bitmap* m_pSpecialBack;
	const Bitmap* m_pSpecialBack00;
	FrButton* m_pMatchHistoryBtn[5];
	const Bitmap* m_pRecTotalBack[3];
	const Bitmap* m_pRecTotalBackNd;
	FrListBox* m_pRecTotalListBox;
	FrButton* m_pRecordBtn[4];
	int m_recordType;
	FrEdit* m_pRecordEdit[15];
	FrWnd* m_pRecAwardWnd[6];
	int m_trophyType;
	int m_reserved270;
	FrButton* m_pTrophyBtn[2];
	FrListBox* m_pSpecialTrophyBox;
	FrListBox* m_pNormalTrophyBox;
	FrStatic* m_pNoTrophy;
	FrEdit* m_pNormalTrophyNum;
	FrEdit* m_pSpecialTrophyNum;
	FrEdit* m_pGuildEdit[3];
	FrStatic* m_pGuildNoTrophy;
	FrListBox* m_pGuildTrophyList;
	FrArea* m_pMyGuild;
	int m_reserved2a8;
	const Bitmap* m_pGuildIcon;
	unsigned char m_reserved2b0[0x30];
	FrButton* m_pRmrVisitorBtn;
	sUserInfo m_userInfo;
	std::list<sSpecialTrophy> m_specialTrophy;
	std::list<sGuildTrophy> m_guildTrophy;
	sSeasonStat m_oldSeason;
	sSeasonStat m_totalSeason;
	sUserPosition m_position;
	sMapStatistics m_classicMapStatistics[20];
	sMapStatistics m_oldClassicMapStatistics[20];
	sMapStatistics m_totalClassicMapStatistics[20];
	sMapStatistics* m_pBestScoreMap[5];
	int m_guildPang;
	int m_guildPoint;
	FrButton* m_pFriendBtn;
	FrButton* m_pWhisperBtn;
	FrButton* m_pIgnoreBtn;
	FrArea* m_pPetArea;
	FrEdit* m_pComment;
	FrArea* m_pNickArea;
	const Bitmap* m_pMaleIcon;
	const Bitmap* m_pFemaleIcon;
	FrListBox* m_pScoreList;
	WTitleFont* m_pScoreFont;
	float m_skinScroll[2];
	int m_skinScrollType[2];
	bool m_bRefresh;
	bool m_bMsnControl;
	bool m_bControlOff;
	float m_petLoadTime;
	bool m_bChatMsgOpen;
	CExhibition* m_pExhibition;
	CPartTidList* m_pPartTidList;
	FrGaugeBar* m_pPetLoadGauge;
	CAnotherHand m_anotherHand;
	bool m_bMatchHistoryRequest;
	bool m_bUnknownLocation;
	float m_delta;
	bool m_bReserved244c;
	sUserMatchHistory m_matchHistory[5];
	FrAddFriendDlg* m_pAddFriendDlg;
	FrButton* m_pGhostBtn;
	unsigned long m_recvSeason;
	int m_requestSeason;

public:
	int IsRecvSeason(unsigned long season)
	{
		return (m_recvSeason & season) != 0;
	}

	DECLARE_FRESH_MSGMAP()
};

#include "user_info.h"
