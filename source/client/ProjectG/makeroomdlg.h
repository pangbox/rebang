#pragma once

#include "frform.h"

enum eHoleType
{
	FRONT_HOLE,
	BACK_HOLE,
	START_RANDOM,
	ALL_RANDOM
};

class FrTabButton;
class FrTreasureCourse;

static const char* s_gameTypeTitle[16] = {
	K2L_Compatibility("\xbd\xba\xc6\xae\xb7\xce\xc5\xa9"),
	K2L_Compatibility("\xb8\xc5\xc4\xa1"),
	K2L_Compatibility("\xc3\xa4\xc6\xc3\xb9\xe6"),
	K2L_Compatibility("\xb7\xa1\xb4\xf5"),
	K2L_Compatibility("\xb4\xeb\xc8\xb8"),
	K2L_Compatibility("\xb4\xeb\xc8\xb8\xc6\xc0\xc0\xfc"),
	K2L_Compatibility("\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc"),
	K2L_Compatibility("\xc6\xce\xb9\xe8\xc6\xb2"), "", "",
	K2L_Compatibility("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"), "", "",
	K2L_Compatibility("\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba")
};

class FrMakeRoomDlg : public FrForm
{
	DECLARE_OBJECT(FrMakeRoomDlg)

	FrMakeRoomDlg();
	virtual ~FrMakeRoomDlg();

	virtual bool OnInit();
	virtual void OnProc(const float dt);

	bool MakeRoom(bool bQuick);

protected:
	void OnMakeVsInit(int param);
	void OnMakeVsDown();
	void OnMakeMassInit(int param);
	void OnMakeMassDown();
	void OnMakeBattleInit(int param);
	void OnMakeBattleDown();
	void OnMakeChatInit(int param);
	void OnMakeChatDown();
	void OnMakeChaosInit(int param);
	void OnMakeChaosDown();
	void OnPictureInit(int param);
	void OnTextInit(int param);
	void OnTitleInit(int param);
	bool OnTitleEnterKey(int param);
	void OnPasswordInit(int param);
	void OnMapPrevInit(int param);
	void OnMapPrevUp();
	void OnMapNextInit(int param);
	void OnMapNextUp();
	void OnMapPrevChatInit(int param);
	void OnMapNextChatInit(int param);
	void SetDefaultTitle();
	void ChangeGameType(unsigned char type, bool bInit);
	void ChangeUserLimit(unsigned char limit);
	void ChangeHole(unsigned char hole);
	void ChangeTimeLimit(unsigned long time);
	void ChangeCourse(unsigned char course);
	void ChangeHoleType(eHoleType type);
	void OnGameTypeInit(int param);
	void OnGameTypeButtonUp();
	void OnGameTypeOwnerDraw(int param);
	void OnUserLimitInit(int param);
	void OnUserLimitButtonUp();
	void OnUserLimitOwnerDraw(int param);
	void OnHoleInit(int param);
	void OnHoleButtonUp();
	void OnHoleOwnerDraw(int param);
	void OnTimeLimitInit(int param);
	void OnTimeLimitButtonUp();
	void OnTimeLimitOwnerDraw(int param);
	void OnCourseInit(int param);
	bool OnMapSelectDlgResult(int result, FrForm* form);
	void OnCourseUp();
	void OnCourseOwnerDraw(int param);
	void OnHoleTypeInit(int param);
	void DrawHoleTypeButton(FrGraphicInterface* gi, eHoleType type, WRect rect,
		bool bEnable);
	void OnHoleTypeOwnerDraw(int param);
	void OnCourseChatInit(int param);
	void OnCourseChatUp();
	void OnHoleChatInit(int param);
	void OnHoleChatOwnerDraw(int param);
	void OnHoleMinInit(int param);
	void OnHoleMinUp();
	void OnHolePrevInit(int param);
	void OnHolePrevUp();
	void OnHoleNextInit(int param);
	void OnHoleNextUp();
	void OnHoleMaxInit(int param);
	void OnHoleMaxUp();
	void OnOverGauge_Init(int param);
	bool IsBtnEnable();

	FrButton* m_pMakeBtn[5];
	FrViewer* m_pPicture;
	FrEdit* m_pTitle;
	FrArea* m_pText;
	FrEdit* m_pPassword;
	unsigned long m_unknown134;
	FrButton* m_pMapPrev;
	FrButton* m_pMapNext;
	FrButton* m_pMapPrevChat;
	FrButton* m_pMapNextChat;
	FrTabButton* m_pGameType;
	FrTabButton* m_pUserLimit;
	FrTabButton* m_pHole;
	FrTabButton* m_pTimeLimit;
	FrViewer* m_pCourse;
	FrArea* m_pHoleType;
	FrViewer* m_pCourseChat;
	FrArea* m_pHoleChat;
	FrButton* m_pHoleMin;
	FrButton* m_pHolePrev;
	FrButton* m_pHoleNext;
	FrButton* m_pHoleMax;
	FrGaugeBarImage* m_pOverGauge;
	const Bitmap* m_pHoleTypeBtn[4][2];
	const Bitmap* m_pMapSelectO;
	unsigned long m_unknown1a0;
	FrTreasureCourse* m_pMapSelectDlg;
	int m_gameTypeGroup;
	unsigned char m_gameType;
	unsigned char m_userLimit;
	unsigned long m_timeLimit;
	unsigned char m_hole;
	unsigned char m_course;
	unsigned char m_holeType;
	int m_chatHole;
	unsigned long m_gameTime;
	unsigned long m_shotTime;
	float m_courseX;
	float m_btnDelay;

	DECLARE_FRESH_MSGMAP()
};
