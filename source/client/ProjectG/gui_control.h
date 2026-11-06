#pragma once

#include "frform.h"
#include "taskmain.h"
#include "wlist.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include "wflag.h"

class Bitmap;
class CAccuracyInfo;
class CAuxMenuUI;
class CBase;
class CCalipers;
class CClubTypeInfo;
class CExtraHoleInfo;
class CFontOverlay;
class CGuildInfo;
class CHolePoseSkins;
class CMiniScore;
class CPangInfo;
class CPowerInfo;
class CRemainTimeInfo;
class CShotCountInfo;
class CSimpleUI;
class CWindInfo;
class FrButton;
class FrChatFuncDlg;
class FrReportDlg;
class IObject;
class WFont;
class WOverlay;
class WRect;
class WPuppet;
class WTitleFont;

class CScreen : public CTaskMain
{
	DECLARE_OBJECT(CScreen)

public:
	virtual ~CScreen(void) { }

	struct sOverlay
	{
		sOverlay(void) { m_pOverlay = NULL; }

		~sOverlay(void)
		{
			if (g_resrcmng && m_pOverlay)
			{
				g_resrcmng->Release(m_pOverlay);
				m_pOverlay = 0;
			}
		}

		char m_name[0x20];
		WOverlay* m_pOverlay;
	};

	virtual void OnDestroy(void);
	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnProcess(float elapsed);
	virtual void OnDisplay(void);

protected:
	CScreen(void);

	void AddOverlay(const char* name, const char* file, int flags);
	void DrawOverlay(const char* name, const WRect& rect, unsigned char alpha,
		int flags);
	void DrawOverlay2(const char* name, const WRect& rect, int color,
		int flags);

private:
	void DrawClubInfo(void);
	void DrawHitBarInfo(void);
	void DrawCammodeInfo(void);
	void DrawWindInfo(void);
	void DrawApproachInfo(void);
	void DrawRemainTime(void);
	void DrawClubBallUI(void);
	void DrawCommandArrow(void);
	void DrawTutorialUI(void);
	bool OnTrainingOptionDlgResult(int ret, FrForm* pForm);

protected:
	virtual void HandleMsg(const MsgObject& msg);

private:
	CFontOverlay* m_pFontOverlay;
	CBase* m_pBase;
	CWindInfo* m_pWindInfo;
	CRemainTimeInfo* m_pRemainTimeInfo;
	CClubTypeInfo* m_pClubTypeInfo;
	CPowerInfo* m_pPowerInfo;
	CAccuracyInfo* m_pAccuracyInfo;
	WPuppet* m_pWindPuppet;
	CMiniScore* m_pMiniScore;
	CAuxMenuUI* m_pAuxMenuUI;
	CPangInfo* m_pPangInfo;
	CHolePoseSkins* m_pHolePoseSkins;
	CExtraHoleInfo* m_pExtraHoleInfo;
	CGuildInfo* m_pGuildInfo;
	CSimpleUI* m_pCammodeUI;
	CSimpleUI* m_pSimpleUI21C;
	CSimpleUI* m_pSimpleUI220;
	CSimpleUI* m_pTutoTitleUI;
	CSimpleUI* m_pCommandArrowUI;
	CSimpleUI* m_pTutoEscMsgUI;
	CSimpleUI* m_pTutorialUIs[12];
	WTitleFont* m_pTitleFont;
	CSimpleUI* m_pMissionUI;
	float m_missionTitleAlpha;
	float m_accumValue;
	float m_fadeValue;
	WList<sOverlay*> m_overlays;
	WFont* m_pFont;
	float m_remainTimeArcShown;
	float m_remainTimeArc;
	unsigned long m_remainTimeColor;
	bool m_hudVisible;
	unsigned long m_hitBarColor;
	int m_msgColorTime;
	unsigned long m_msgColor;
	int m_unknown2B4;
	int m_hitBarFrom;
	int m_hitBarCount;
	unsigned long m_hitBarRangeColor;
	float m_hitBarTime;
	float m_remainTimeY;

	struct sHitMark
	{
		float y;
		unsigned long color;
	};

	sHitMark m_hitMarks[11];
	int m_unknown324[20];
	float m_blinkTime;
	WFlags m_drawFlags;
	float m_cammodeAlpha;
	float m_ballStateCut;
	bool m_missionMarkShown[0x10];
	unsigned char m_tutoMsgBlinkCount;
	unsigned char m_tutoMsgShown;
	float m_tutoMsgBlinkPeriod;
	float m_tutoMsgBlinkTime;
	unsigned char m_missionMarkBlinkCount;
	float m_missionMarkBlinkPeriod;
	float m_missionMarkBlinkTime;
	int m_selectedIndex;
	float m_bannerTime;
	int m_tutoTitleState;
	unsigned char m_tutoTitleAlpha;
	float m_tutoTitleX;
	float m_tutoTitleY;
	float m_tutoTitleTime;
	int m_missionTitleState;
	float m_missionTitleX;
	float m_missionTitleY;
	float m_missionTitleTime;
	int m_tutorialNo;
	float m_turnStartTime;
	bool m_turnStarted;
	bool m_unknown3E1;
	bool m_unknown3E2;
	bool m_unknown3E3;
	unsigned long m_bannerGameMode;
	float m_narrowFlicker[3];
	float m_clubAngle;
	int m_clubSpinDir;
	float m_clubSwing;
	CCalipers* m_pCalipers;
	int m_clubBoundingBox;
	unsigned char m_playerCount;
};

class CMiniScore
{
public:
	CMiniScore(void);
	~CMiniScore(void);

	void ChangeCharacter(void);
	WRect GetIconRect(int flags);
	void Flicker(void);
	void ResetUiPosition(void);
	void Show(void);
	void Hide(void);
	void StartFloating(void);
	void Process(float elapsed);
	void Display(void);

private:
	friend class CScreen;

	CShotCountInfo* m_pShotCountInfo;
	WOverlay* m_pOverlay04;
	const Bitmap* m_pCharBitmap[4];
	const Bitmap* m_pVsIcon;
	WOverlay* m_pOverlay1C;
	WOverlay* m_pOverlay20;
	WOverlay* m_pBestRecord[2];
	CSimpleUI* m_pSimpleUI;
	WFont* m_pFont;
	WRect m_iconUV;
	WRect m_iconRect;
	WRect m_charRect;
	float m_offsetY;
	float m_posX;
	int m_state;
	float m_stateTime;
	unsigned char m_alpha;
	unsigned long m_gameMode;
};

class CAuxMenuUI : public FrCmdTarget
{
public:
	CAuxMenuUI(void);
	virtual ~CAuxMenuUI(void);

	void Display(void);
	void SetActive(bool active, int index);
	void ChangeTurn(void);
	void SetPause(void);
	void OpenChatRecord(void);
	void SetUnplayable(void);
	void Process(float elapsed);

private:
	friend class CScreen;

	bool OnChatFuncDlgResult(int ret, FrForm* pForm);
	bool OnDropFailDlgResult(int ret, FrForm* pForm);
	bool OnDropDlgResult(int ret, FrForm* pForm);

	WOverlay* m_pBase;

	WOverlay* m_pItemOverlay[2];
	int m_hoverIndex;
	int m_active;
	int m_index;
	float m_fade;
	FrForm* m_pDropDlg;
	FrForm* m_pDropFailDlg;
	unsigned char m_unknown28;
	FrChatFuncDlg* m_pChatFuncDlg;
};

class CDrivingDistInfo
{
public:
	CDrivingDistInfo(void);
	~CDrivingDistInfo(void);

	void OnChangeTurn(void);
	void Process(float elapsed);
	void Display(WFont* pFont);

private:
	WOverlay* m_pPlayerOverlay;
	WOverlay* m_pTopOverlay;
	bool m_unknown08;
	bool m_unknown09;
	unsigned char m_bestPlayer;
	unsigned char m_unknown0B;
	unsigned long m_bestColor;
	unsigned long m_myColor;
	float m_shownDistance;
	float m_bestDistance;
	float m_myDistance;
};

class FrChatFuncDlg : public FrForm
{
	DECLARE_OBJECT(FrChatFuncDlg)

public:
	FrChatFuncDlg(void);
	virtual ~FrChatFuncDlg(void);

	void AddChatMsg(const char* msg, int color);

protected:
	void OnBtnShowChatUp(void);
	void OnBtnHideChatUp(void);
	void OnBtnSaveChatUp(void);
	bool OnReportDlgResult(int ret, FrForm* pForm);
	void OnBtnAccuseUp(void);
	void OnBtnCloseDlgUp(void);
	void OnHideChatInit(intptr_t param);
	void OnShowChatInit(intptr_t param);
	void OnChatViewInit(intptr_t param);

protected:
	DECLARE_FRESH_MSGMAP()

private:
	void* m_pUnknown110;
	FrButton* m_pBtnShowChat;
	FrButton* m_pBtnHideChat;
	void* m_pUnknown11C;
	void* m_pUnknown120;
	FrEdit* m_pChatView;
	FrReportDlg* m_pReportDlg;
	int m_unknown12C;
};
