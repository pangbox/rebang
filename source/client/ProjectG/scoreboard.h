#pragma once

#include <map>
#include <vector>
#include "frform.h"
#include "frgraphicinterface.h"
#include "actor.h"
#include "noticeimage.h"

struct sRivalData;
class CExhibition;
class FrScoreDlg;
class FrLevelupItemForm;
class FrTreasureGauge;
class FrNextHole;
class FrTreasureScore;
class FrTreasureGift;

class FrRankForm : public FrForm
{
public:
	DECLARE_OBJECT(FrRankForm)
	friend class CScore;
	FrRankForm();

	void Align();
	void Resize();
	void SortRank();
	void SetCaptionType(unsigned char type);

protected:
	virtual void OnProc(const float delta);

	void OnRankInit(int param);
	void OnRankOwnerDraw(int param);
	void OnRankLBtnDown();
	void OnRankRBtnUp();
	void OnRankDClick();
	void OnBoardInit(int param);
	void OnBar01Init(int param);
	void OnBar02Init(int param);
	void OnCenterBtnUp();
	void OnInfoBtnUp();
	void OnSizeBtnUp();

private:
	int GetMedalIndex(unsigned long oid);

protected:
	FrListBox* m_pRankList;
	FrStatic* m_pBoard;
	const Bitmap* m_pPCIcon;
	unsigned char m_sizeType;
	unsigned char m_captionType;
	unsigned long m_firstOID;
	unsigned long m_lastOID;
	unsigned long m_selectedOID;
	unsigned char m_playerNum;
	unsigned char m_captionPriority;
	unsigned char m_noticeHole;
	unsigned char m_blinkCount;
	float m_captionTime;
	unsigned char m_holePlayerNum[19];
	const Bitmap* m_pMedal[4];
	const Bitmap* m_pTeamRank[2];
	const Bitmap* m_pRank;

	DECLARE_FRESH_MSGMAP()
};

class FrRankForm2 : public FrForm
{
public:
	DECLARE_OBJECT(FrRankForm2)

	FrRankForm2();

	void SortRank();

protected:
	void OnRankInit(int param);
	void OnRankOwnerDraw(int param);

private:
	int GetMedalIndex(unsigned long oid);

protected:
	FrListBox* m_pRankList;
	const Bitmap* m_pMedal[4];
	const Bitmap* m_pTeamRank[2];
	const Bitmap* m_pRank;
	const Bitmap* m_pPCIcon;

	DECLARE_FRESH_MSGMAP()
};

class FrRankForm3 : public FrForm
{
public:
	DECLARE_OBJECT(FrRankForm3)
	friend class CScore;
	FrRankForm3();

	void SetScore(int score1, int score2);
	void SetCaptionType(unsigned char type);

protected:
	virtual void OnProc(const float delta);

	void OnRankInit(int param);
	void OnRankOwnerDraw(int param);
	void OnBoardInit(int param);
	void OnScoreGaugeInit(int param);
	void DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
		unsigned long color, FrGDI::eTextAlign align, bool narrow);
	void OnSizeBtnUp();
	void OnScoreInfoInit(int param);
	void OnScoreInfoOwnerDraw();
	void LoadBitmaps();

	FrListBox* m_pRankList;
	FrStatic* m_pBoard;
	FrGaugeBar* m_pScoreGauge;
	FrArea* m_pScoreInfo;
	unsigned char m_sizeType;
	unsigned char m_captionType;
	unsigned long m_selectedOID;
	unsigned char m_playerNum;
	unsigned char m_noticeHole;
	unsigned char m_blinkCount;
	float m_captionTime;
	const Bitmap* m_pEmblem1;
	const Bitmap* m_pEmblem2;
	const Bitmap* m_pRank;
	const Bitmap* m_pDigit[10];
	const Bitmap* m_pPlus;
	const Bitmap* m_pMinus;
	const Bitmap* m_pPCIcon;
	int m_score[2];

	DECLARE_FRESH_MSGMAP()
};

class CRankExData
{
public:
	CRankExData();
	~CRankExData();

	void InitAreaStage(FrArea* area);
	void Clear();
	void OnProc(float delta);
	void CreateLeaderPet(const WRect& rect);
	void SetActive(bool bActive);
	bool IsMotionFinished();
	void ResetStage();
	void InitRivalData();
	void InitMedalData();
	void DrawRankItem(FrListItem* item);
	void DrawStageArea();
	void SetLeader();
	void ShowLeader();
	void ClearRival();
	void SetRivalData();

private:
	int GetMedalIndex(unsigned long oid);

public:
	sRivalData* m_pRival;
	float m_motionTime;
	float m_elapsedTime;
	unsigned long m_leaderOID;
	unsigned long m_pendingOID;
	CExhibition* m_pLeaderPet;
	FrListBox* m_pRankList;
	FrArea* m_pStageArea;
	FrStatic* m_pBoard;
	const Bitmap* m_pMedal[4];
	const Bitmap* m_pTeamRank[2];
	const Bitmap* m_pRank;
	const Bitmap* m_pPCIcon;
	const Bitmap* m_pZeroIcon;
};

class FrRankFormEx : public FrForm
{
public:
	DECLARE_OBJECT(FrRankFormEx)
	friend class CScore;
	FrRankFormEx();
	virtual ~FrRankFormEx();

	void SortRank();
	sRivalData* GetRival();
	void Clear();
	void SetActive(bool bActive);
	bool IsMotionFinished();
	void ResetStage();

protected:
	virtual bool OnInit();
	virtual void OnProc(const float delta);

	void OnStageAreaInit(int param);
	void OnStageAreaOwnerDraw();
	void OnRankInit(int param);
	void OnRankOwnerDraw(int param);
	void OnBoardInit(int param);
	void SetLeader();
	void ShowLeader();

	CRankExData m_rankExData;

	DECLARE_FRESH_MSGMAP()
};

class FrAwardForm : public FrForm
{
public:
	DECLARE_OBJECT(FrAwardForm)

	FrAwardForm();
	virtual ~FrAwardForm();

	virtual bool Close(bool bResult);

protected:
	virtual bool OnInit();
	virtual void OnProc(const float delta);

	void CalcResult();
	void CalcResultMatch();

	void OnUserListInit(int param);
	void OnUserListOwnerDraw(int param);
	void OnTab0Init(int param);
	void OnTab1Init(int param);
	void OnExpBar0Init(int param);
	void OnExpBar1Init(int param);
	void OnExpBar2Init(int param);
	void OnExpBar3Init(int param);
	void OnExp0Init(int param);
	void OnExp1Init(int param);
	void OnExp2Init(int param);
	void OnExp3Init(int param);
	void OnExpEnd0Init(int param);
	void OnExpEnd1Init(int param);
	void OnExpEnd2Init(int param);
	void OnExpEnd3Init(int param);
	void OnLevel0Init(int param);
	void OnLevel1Init(int param);
	void OnLevel2Init(int param);
	void OnLevel3Init(int param);
	void OnLevelUp0Init(int param);
	void OnLevelUp1Init(int param);
	void OnLevelUp2Init(int param);
	void OnLevelUp3Init(int param);
	void OnMatchRes0Init(int param);
	void OnMatchRes1Init(int param);
	void OnGainedLadderPoint0Init(int param);
	void OnGainedLadderPoint1Init(int param);
	void OnLadderPoint0Init(int param);
	void OnLadderPoint1Init(int param);
	void OnNextLevelTextInit(int param);

	const char* m_pExpSound;
	float m_tickTime;
	int m_order[4];
	FrListBox* m_pUserList;
	FrGaugeBar* m_pExpBar[4];
	FrStatic* m_pExp[4];
	FrStatic* m_pExpEnd[4];
	FrArea* m_pLevel[4];
	FrArea* m_pLevelUp[4];
	FrArea* m_pMatchRes[2];
	int m_level[4];
	int m_gainExp[4];
	int m_curExp[4];
	bool m_bFinished[4];
	int m_playerNum;
	FrStatic* m_pGainedLadderPoint[2];
	FrStatic* m_pLadderPoint[2];
	FrEdit* m_pNextLevelText;
	unsigned long m_reserved1d0;
	float m_treasureTime;
	const Bitmap* m_pPCIconVS;
	const Bitmap* m_pPCIconBonus;
	const Bitmap* m_pBonusMascot[6];
	const Bitmap* m_pBonusMastery;
	const Bitmap* m_pBonusMastery2;
	const Bitmap* m_pZeroIconVS;

	DECLARE_FRESH_MSGMAP()
};

class cAquireItem
{
public:
	struct sPrizeSlot
	{
		std::map<unsigned long, int> m_prizeCount;
	};

	cAquireItem();
	~cAquireItem();

	void Reset();
	void OnProc(float delta);
	void DisplayPrizeList(int index);

protected:
	WVector GetPlayer2DPos(int index);

	const Bitmap* m_pItemFrame[3];
	sPrizeSlot m_prizeSlot[4];
	const Bitmap* m_pBubble[3];
	std::map<unsigned long, WOverlay*> m_overlay[4];
	WTitleFont* m_pFont;
	CSimpleUI* m_pEmoti;
	float m_time;
	float m_maxTime;
};

class FrScoreForm : public FrForm
{
public:
	DECLARE_OBJECT(FrScoreForm)

	enum eState
	{
		STATE_0,
		STATE_1,
		STATE_2,
		STATE_3,
	};

	FrScoreForm();

	void SetState(eState state);
	void Resize();
	void SortLists(bool bSort);
	void ChangeTurn();
	void UpdatePang();
	void SetTypeBtn();
	void SetPPL(const char* name);
	void SetDesc();

protected:
	virtual bool OnInit();
	virtual void OnProc(const float delta);
	virtual void OnDraw();

	void LoadBitmaps();

	void OnBar2Init(int param);
	void OnBar3Init(int param);
	void OnFront9BtnInit(int param);
	void OnFront9BtnUp();
	void OnBack9BtnInit(int param);
	void OnBack9BtnUp();
	void OnShotBtnUp();
	void OnGraphBtnUp();
	void OnVersusBtnUp();
	void OnPuttBtnUp();
	void OnPangBtnUp();
	void OnShotBtnInit(int param);
	void OnGraphBtnInit(int param);
	void OnPuttBtnInit(int param);
	void OnPangBtnInit(int param);
	void OnVersusBtnInit(int param);
	void OnHoleListInit(int param);
	void OnHoleListOwnerDraw(int param);
	void OnParListInit(int param);
	void OnParListOwnerDraw(int param);
	void OnPangListInit(int param);
	void OnBlankPanel01Init(int param);
	void OnBlankPanel01OwnerDraw(int param);
	void OnBlankPanel02Init(int param);
	void OnBlankPanel02OwnerDraw(int param);
	void OnBlankPanel03Init(int param);
	void OnBlankPanel03OwnerDraw(int param);
	void OnPangListOwnerDraw(int param);
	void OnGolferListInit(int param);
	void OnGolferListOwnerDraw(int param);
	void OnRecordListInit(int param);
	void OnRecordListOwnerDraw(int param);
	void OnTotalListInit(int param);
	void OnTotalListOwnerDraw(int param);
	void OnTotalInit(int param);
	void OnPPLInit(int param);
	void OnBoard1Init(int param);
	void OnBoard2Init(int param);
	void OnDrawLinesInit(int param);
	void OnDrawLinesOwnerDraw(int param);
	void OnCancelBtnInit(int param);
	void OnCancelBtnUp();
	void OnCloseBtnInit(int param);
	void OnCloseBtnUp();

	void SetFrontBackBtn();
	void DrawShot(FrListItem* item);
	void DrawVersus(FrListItem* item);
	void DrawGraph(FrListItem* item);
	void DrawPutt(FrListItem* item);
	void DrawPang(FrListItem* item);
	void DrawSkins(FrListItem* item);
	void DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
		unsigned long color, FrGDI::eTextAlign align, bool narrow);
	unsigned long GetScoreColor(int score);
	void ConditionalStrCat(char* dest, const char* src);

	eState m_state;
	FrButton* m_pFront9Btn;
	FrButton* m_pBack9Btn;
	FrButton* m_pTypeBtn[6];
	FrListBox* m_pHoleList;
	FrListBox* m_pParList;
	FrListBox* m_pPangList;
	FrListBox* m_pList[3];
	FrButton* m_pCancelBtn;
	FrButton* m_pCloseBtn;
	FrArea* m_pBar2;
	FrArea* m_pBar3;
	FrArea* m_pPPL;
	bool m_bShowPPL;
	int m_order[4];
	bool m_bFront9;
	int m_type;
	float m_aniTime;
	__int64 m_holePang;
	__int64 m_totalPang;
	int m_playerNum;
	std::map<unsigned long, const Bitmap*> m_bitmapMap;
	const Bitmap* m_pDigit[10];
	const Bitmap* m_pPlus;
	const Bitmap* m_pMinus;
	const Bitmap* m_pPoint;
	const Bitmap* m_pResult[4];
	const Bitmap* m_pQuestion;
	const Bitmap* m_pArrow;
	const Bitmap* m_pPCIcon;
	bool m_bShowBlankPanel;
	FrArea* m_pBlankPanel[3];

	DECLARE_FRESH_MSGMAP()
};

class FrMatchItems : public FrForm
{
public:
	DECLARE_OBJECT(FrMatchItems)

	FrMatchItems();
	virtual ~FrMatchItems();

	void Reset()
	{
		m_bAnimate = true;
		m_bPlaySound = true;
	}

protected:
	virtual bool OnInit();

	void OnViewInit(int param);
	void OnViewOwnerDraw(int param);

	std::map<unsigned long, WOverlay*> m_overlayMap;
	unsigned long m_reserved11c;
	WPoint m_pos;
	unsigned long m_reserved128[6];
	FrArea* m_pView;
	CSimpleUI* m_pEmoti;
	bool m_bAnimate;
	bool m_bPlaySound;
	int m_frame;
	unsigned long m_reserved150;
	float m_aniTime;

	DECLARE_FRESH_MSGMAP()
};
class CScore : public IActor, public FrCmdTarget
{
public:
	class CScoreBg
	{
	public:
		CScoreBg();
		~CScoreBg();

		void ScoreBgCommand(int command);

	protected:
		void SetDesktopWallPaper(bool bVisible, bool bCaptured);

		Bitmap* m_pBitmap;
		bool m_bCaptured;
	};

protected:
	CScore();

public:
	virtual ~CScore() { }

	DECLARE_OBJECT(CScore)

	virtual void OnLoad();
	virtual void OnReload();
	virtual void OnProcess(float delta);
	virtual void OnDisplay();
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

protected:
	void OnCloseBtnUp();
	bool OnLevelupFormResult(int result, FrForm* form);
	bool OnRankFormExResult(int result, FrForm* form);
	bool OnGuildScoreFormResult(int result, FrForm* form);
	bool UpdateRivals();

private:
	void ImageSortByCharacterIndex(std::vector<sNoticeImage>& images);
	void CheckOpenedDlg();

protected:
	int m_resultState;
	float m_resultTime;
	int m_panelState;
	int m_bShowRank3;
	int m_bRank3Enabled;
	int m_reserved30;
	FrScoreForm* m_pScoreForm;
	FrRankForm* m_pRankForm;
	FrRankForm2* m_pRankForm2;
	FrRankForm3* m_pRankForm3;
	FrRankFormEx* m_pRankFormEx;
	FrAwardForm* m_pAwardForm;
	FrMatchItems* m_pMatchItems;
	FrScoreDlg* m_pGuildScoreDlg;
	std::vector<FrLevelupItemForm*> m_levelupForms;
	FrTreasureGauge* m_pTreasureGauge;
	FrNextHole* m_pNextHole;
	FrTreasureScore* m_pTreasureScore;
	FrTreasureGift* m_pTreasureGift;
	float m_fadeAlpha;
	WPoint m_rankPos;
	cAquireItem* m_pAquireItem;
	std::vector<sNoticeImage> m_noticeImages;
	CScoreBg m_scoreBg;

	DECLARE_FRESH_MSGMAP()
};
