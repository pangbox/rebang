#pragma once

#include <vector>

#include "taskmain.h"
#include "rtti.h"
#include "wtypes.h"
#include "anotherhand.h"
#include "../../shared/globalgamedefine.h"

class FrForm;
class FrArea;
class FrButton;
class FrListBox;
class FrTreasureCourse;
class FrSubEquipBar;
class CExhibition;
class CStatBar;
class CPartTidList;
struct FrListItem;
struct sUserInfo;

class CFamilyMain : public CTaskMain
{
public:
#pragma pack(push, 1)
	class cFamily
	{
	public:
		cFamily(void);
		~cFamily(void);

		void Init(const WRect& rect, std::vector<int>& petList,
			unsigned char petKind);
		void Render(void);
		void Process(float dt);
		const WPoint& GetFramePos(void) const;
		const sUserEquip& GetUserEquip(void) const;
		void SetItemSlot(unsigned long* pItemSlot);
		void ConvertUserInfo(sUserInfo& info);
		void SetUserEquip(int category, unsigned long id);

	private:
		void SetStatus(void);
		void PetRebuild(void);
		void MakePet(std::vector<int>& petList);

		WRect m_rect;
		WPoint m_framePos;
		WPoint m_modelPos;
		CExhibition* m_exhibition;
		CAnotherHand m_anotherHand;
		CPartTidList* m_partTidList;
		sCharacterInfo m_charInfo;
		sCaddieInfo m_caddieInfo;
		sItemInfo m_itemInfo;
		sUserEquip m_userEquip;
		sMascotInfo m_mascot;
		CStatBar* m_statBar;
		unsigned char m_petKind;
		unsigned char m_unknown4DD;
		unsigned char m_unknown4DE;
		unsigned char m_unknown4DF;
	};
#pragma pack(pop)

	DECLARE_OBJECT(CFamilyMain)

	static unsigned char m_mapIndex;
	static unsigned long ms_itemslotBackup[4][10];

	CFamilyMain(void);
	virtual ~CFamilyMain(void);

	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnDisplay(void);
	virtual void OnProcess(float dt);
	virtual void OnDestroy(void);
	virtual void HandleMsg(const MsgObject& msg);

protected:
	static sFRESH_MSGMAP _MsgMap;
	static sFRESH_ENTRY _MsgEntries[];
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	void OnInitFamilyMain(int param);
	void OnInitFamilyFinish(void);
	void TurnOffControls(void);
	void OnInitAreaPet1(int param);
	void OnInitAreaPet2(int param);
	void OnInitAreaPet3(int param);
	void OnInitAreaPet4(int param);
	void OnInitAreaGlobal(int param);
	void OnInitBtnStart(int param);
	void OnInitBtnSelMap(int param);
	void OnInitBtnHoleType(int param);
	void OnInitBtnHoleNum(int param);
	void OnInitBtnTimeLimit(int param);
	void OnInitCreatePet3(int param);
	void OnInitCreatePet4(int param);
	void OnInitDeletePet3(int param);
	void OnInitDeletePet4(int param);
	void OnInitListEquip1(int param);
	void OnInitListEquip2(int param);
	void OnInitListEquip3(int param);
	void OnInitListEquip4(int param);
	void OnDownCreatePet3(void);
	void OnDownCreatePet4(void);
	void OnDownDeletePet3(void);
	void OnDownDeletePet4(void);
	void OnDrawAreaPet1(void);
	void OnDrawAreaPet2(void);
	void OnDrawAreaPet3(void);
	void OnDrawAreaPet4(void);
	void OnDrawAreaGlobal(void);
	void OnDownBtnSelMap(void);
	bool OnResultSelMap(int, FrForm*);
	void OnDownBtnHoleType(void);
	void OnDownBtnHoleNum(void);
	void OnDrawBtnSelMap(void);
	void OnDrawBtnHoleType(void);
	void OnDrawBtnHoleNum(void);
	void OnDrawBtnTimeLimit(void);
	bool OnItemEquipResult(int result, FrForm* pForm);
	void OnDownBtnTimeLimit(void);
	void OnDownBtnStart(void);
	void OnDrawBtnStart(void);
	void OnDrawEquipListBox(FrListItem* pItem);
	void OnDrawListEquip1(int);
	void OnDrawListEquip2(int);
	void OnDrawListEquip3(int);
	void OnDrawListEquip4(int);
	void OnDownEquipListBox(int index);
	virtual bool OnResultSubEquipBar(int result, FrForm* pForm);
	void OnDownListEquip1(int);
	void OnDownListEquip2(int);
	void OnDownListEquip3(int);
	void OnDownListEquip4(int);
	void ReFresh(bool);

private:
	std::vector<cFamily*> m_families;
	std::vector<FrListBox*> m_equipLists;
	std::vector<FrArea*> m_areas;
	std::vector<FrButton*> m_buttons;
	unsigned long m_timeLimit;
	unsigned char m_holeNum;
	unsigned char m_holeType;

	std::vector<int> m_petList;
	FrTreasureCourse* m_selMapDlg;
	FrSubEquipBar* m_subEquipBar;
	int m_curPet;
	FrListItem* m_selectedItem;
	bool m_unknown248;
};
