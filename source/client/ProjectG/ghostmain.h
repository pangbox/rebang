#pragma once

#include "taskmain.h"

#include "anotherhand.h"
#include "wtypes.h"

#include <string>
#include <vector>

class IObject;
class IActor;
class CStatBar;
class CExhibition;
class CPartTidList;
class FrArea;
class FrListBox;
class FrForm;
struct FrListItem;
class MsgObject;
class Bitmap;

class CGhostMain : public CTaskMain
{
public:
	DECLARE_OBJECT(CGhostMain)

	CGhostMain(void);
	virtual ~CGhostMain(void);

	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnDestroy(void);
	virtual void OnProcess(float dt);
	virtual void OnDisplay(void);
	virtual void HandleMsg(const MsgObject& msg);

protected:
	DECLARE_FRESH_MSGMAP()

	void OnInitGhostFinish(void);
	void OnInitStart(int);
	void OnDownStart(void);
	CExhibition* MakePet(FrArea* area, CPartTidList* parts, int);
	void OnExit(void);
	void HidePet(bool wait);
	void OnInitSelectCourse(int);
	void OnDownSelectCourse(void);
	void OnDrawTipZone(void);
	void OnInitGhostMain(int);
	void OnInitPlayerZone(int);
	void OnDrawPlayerZone(void);
	void OnInitUserEquip(int);
	void OnDrawUserEquip(int);
	void OnDownUserEquip(void);
	void OnInitGhostEquip(int);
	void OnDrawGhostEquip(int);
	void OnInitGhostZone(int);
	void OnDrawGhostZone(void);
	void OnInitTipZone(int);
	void OnInitAreaUserStat(int);
	void OnDrawAreaUserStat(void);
	void OnInitAreaGhostStat(int);
	void OnDrawAreaGhostStat(void);

private:
	virtual bool OnResultSubEquipBar(int, FrForm*);
	void OnProcTip(float dt);
	void RefreshAllUccClothes(void);
	void Initialize(void);
	CStatBar* BuildStatusBar(int index);
	void StatProcess(float dt);
	void UserPetRebuild(void);
	bool CallBackEquipItemResult(int result, FrForm* form);
	bool IsFocusToMain(void);
	void SetUserEquip(PANGYA_ITEM_GROUP group, unsigned long id);
	void SendPacketEquipResult(PANGYA_ITEM_GROUP group, unsigned long typeId);

	CAnotherHand m_hand;
	CExhibition* m_userPet;
	CExhibition* m_ghostPet;
	CPartTidList* m_userParts;
	CPartTidList* m_ghostParts;
	float m_ghostAlpha;
	bool m_ghostVisible;
	std::vector<FrArea*> m_areas;

	class cImgBinder
	{
	public:
		struct sImgInfo
		{
			sImgInfo(const std::string& strImage, const WRect& rect);

			std::string m_strImage;
			WRect m_rect;
		};

		cImgBinder(int);
		~cImgBinder(void);

		void AddImage(std::string strImage, const WPoint& pos);

		WPoint GetPos(void);
		void SetPosX(float x);
		void AddPos(float dx, float dy);
		void AddPos(const WPoint& delta);
		void Render(unsigned char alpha);

		std::vector<sImgInfo> m_vecImage;
	};

	cImgBinder* m_userStatImage;
	cImgBinder* m_ghostStatImage;
	float m_userStatAlpha;
	float m_userStatStopX;
	float m_ghostStatStopX;
	float m_userStatSpeed;
	float m_ghostStatSpeed;
	bool m_showUserStat;
	float m_statTime;
	float m_statSpeedTime;
	FrListBox* m_userEquip;
	FrListBox* m_ghostEquip;
	FrListItem* m_selectedIcon;
	std::vector<std::string> m_userIcons;
	std::vector<std::string> m_ghostIcons;
	float m_equipAlpha;
	FrForm* m_equipDlg;
	CStatBar* m_userStat;
	CStatBar* m_ghostStat;
	WRect m_courseRect;
	const Bitmap* m_courseBitmap;
	int m_unknown408;
	unsigned char m_exitFlag;
	int m_unknown410;
	int m_ghostScore;
	int m_ghostPang;
	int m_unknown41C;
	int m_unknown420;
	cImgBinder* m_tipImage;
	float m_tipAlpha;
	bool m_tipHiding;
	float m_tipWait;
};
