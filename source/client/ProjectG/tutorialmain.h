#pragma once

#include "taskmain.h"

#include "wtypes.h"

#include <list>
#include <string>
#include <vector>
#include <windows.h>
#include <mmsystem.h>

class IObject;
class CExhibition;
class Bitmap;
class FrQuestInfoDlg;
class FrQuestGift;
class FrArea;
class FrListBox;

class CPipinTime
{
public:
	CPipinTime(void)
		: m_lastMotionTime(0), m_lastIdleTime(0), m_selected(false)
	{
	}

	~CPipinTime(void) { }

	void Reset(void)
	{
		m_lastMotionTime = 0;
		m_lastIdleTime = 0;
		m_selected = false;
	}

	void SetTime(void)
	{
		m_lastMotionTime = timeGetTime();
		m_lastIdleTime = m_lastMotionTime;
	}

	unsigned long m_lastMotionTime;
	unsigned long m_lastIdleTime;
	bool m_selected;
	std::string m_nextMotion;
};

class CTutorialMain : public CTaskMain
{
	DECLARE_OBJECT(CTutorialMain)

public:
	struct sTutorialMap
	{
		const char* m_name;
		unsigned char m_category;
		unsigned char m_index;
		unsigned char m_state;
		unsigned char m_level;
		unsigned char m_quest;
		int m_pending;
		int m_status;
		bool m_enabled;
	};

	struct sTutorialResult
	{
		unsigned char m_category;
		int m_count;
	};

	struct sCategoryInfo
	{
		unsigned char m_id;
		unsigned char m_count;
		short m_reserved;
		int m_y;
	};

	struct sArrowInfo
	{
		unsigned char m_first;
		unsigned char m_last;
		unsigned char m_show;
	};

	struct sTutorialCondition
	{
		int m_index;
		int m_unlock;
		int m_start;
		int m_count;
	};

	struct sChildTable
	{
		int m_quest;
		int m_first;
		int m_count;
	};

	struct sTutorialInfo
	{
		int m_id;
		bool m_enabled;
		bool m_cleared;
	};

	class CQuestInfo
	{
	public:
		std::vector<int> m_items;
		unsigned char m_parentCount;
		unsigned char m_parentsCleared;
	};

	CTutorialMain(void);
	virtual ~CTutorialMain(void);
	virtual void OnPreserveBack(const char*);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnRestorePreserved(void);

	void EnableAllBtn(void);
	void ShowPet(void);
	void HidePet(void);
	int GetSelectedBtn(int type, WPoint pt);
	void OpenGradeGift(unsigned char grade);
	void OpenQuestGift(void);
	void DecideNextMotion(unsigned char grade);
	bool IsCompleteCategory(int category);
	bool IsCompleteQuest(int category, int index);
	void OpenQuest(int category, int index);
	void SetCompleteStates(void);
	void CheckNodeStates(void);
	virtual void OnProcess(float dt);
	virtual void OnDestroy(void);
	virtual void HandleMsg(const MsgObject& msg);

protected:
	static sTutorialMap m_Tutorials[14];
	static const sTutorialCondition m_TutoCondition[8];
	static const sChildTable m_ChildTable[14];
	static sTutorialResult m_CompleteItems[3];
	static sCategoryInfo m_sCategoryItem[3];
	static sArrowInfo m_ArrowItems[10];
	DECLARE_FRESH_MSGMAP()

	void LoadTutorialMainImages(void);
	void DisableOverBarButtons(void);
	void DisableUnderBarButtons(void);
	void OnTutorialMainInit(int);
	void OnTutorialMainFinish(void);
	void OnTutorialAreaPipin(int param);
	void OnTutorialAreaPipinOwnerDraw(int);
	void OnTutorialListInit(int param);
	void OnTutorialListOwnerDraw(int param);
	void DrawCourseArrow(void);
	void OnTutorialListLBtnDown(void);
	void OnTutorialListLBtnUp(void);
	void CheckEnable(int index);
	void ProcessLobbyBgCamera(float dt);
	void MissionClear(int& index, unsigned long& flags);
	void MissionComplete(unsigned long flags);
	void ProcessAnimateIcon(void);
	bool OnGiftDlgResult(int result, FrForm* form);
	sTutorialInfo* FindTutorialList(int& id);
	void StartSelectedTutorial(void);
	void InitGolfTutorial(void);
	void OpenTutorialInfoDlg(void);
	bool OnTutorialInfoDlgResult(int result, FrForm* form);
	bool OnQuestionFormResult(int result, FrForm* form);
	void SetQuestInformation(sTutorialMap* map);
	void SetQuestInformation(int category, int index);

private:
	int IsInArea(int type, WPoint pt);

	FrListBox* m_tutorialListBox;
	int m_unknown1E4[8];
	FrForm* m_questionForm;
	FrQuestInfoDlg* m_tutorialInfoDlg;
	FrQuestGift* m_questGift;
	int m_unknown210;
	FrArea* m_pipinArea;
	int m_unknown218;
	CExhibition* m_pet;
	float m_camDirection;
	bool m_camPaused;
	unsigned long m_camPauseTime;
	unsigned long m_camMoveTime;
	const Bitmap* m_images[64];
	WPoint m_overBarBtnPos[8];
	WPoint m_underBarBtnPos[6];
	float m_columnX[5];
	int m_listState;
	int m_questState;
	int m_questCategory;
	int m_questNextIndex;
	int m_questIndex;
	int m_questValue;
	bool m_categoryClear[3];
	bool m_questReward;
	unsigned char m_gradeGift;
	CQuestInfo m_questInfo[14];
	std::list<sTutorialInfo> m_tutorialList;
	unsigned char m_pendingTutorial;
	CPipinTime m_pipinTime;
	float m_iconOffsetX;
	float m_iconOffset;
	float m_iconOffsetZ;
	unsigned char m_categoryCount;
};
