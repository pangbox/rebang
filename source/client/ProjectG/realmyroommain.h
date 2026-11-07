#pragma once

#include <list>
#include <map>

#include "../../shared/globalgamedefine.h"
#include "../../shared/globalnetworkdefine.h"
#include "taskmain.h"
#include "scenemanager.h"
#include "anotherhand.h"
#include "realmyroomdata.h"

class CExhibition;
class CCardBook;
class CFxSequence;
class FrEnchantDlg;
class FrDowngradeDlg;
class FrPangGiftDlg;
class FrMascotInfoDlg;
class FrNoteDlg;
class FrFortuneDlg;
class FrReplayModeDlg;
class FrCardUseDlg;
class FrItemAttachCardDlg;
class FrOpenCardPackDlg;
class FrUccDrawDlg;
class FrUccCopyDlg;
class FrUniteResultDlg;
class FrCardRemove;
class CPost_ReciveDlg;
class CPost_SendDlg;
class CRealMyRoom_AuthorityDlg;
class CFurniture_AbilityDlg;
class FrItemStorageNotice;
class FrItemStorageAccessPanel;
struct sSpecialTrophy;
struct sGiftInfo;
struct sFurniture_List;
enum ePCBangItem
{
	PCITEM_NONE,
	PCITEM_CADDIE,
	PCITEM_MASCOT
};

extern Fresh* g_pFresh;

class CRealMyRoomMain : public CTaskMain,
						public CRenderFuncPtr,
						public CHandOwner
{
	DECLARE_OBJECT(CRealMyRoomMain)

public:
	enum mainTab_t
	{
	};
	enum subTab_t
	{
	};
	enum statTab_t
	{
	};
	enum selTab_t
	{
	};
	enum eFurniture_Tab
	{
	};

	struct miniBtn_t
	{
		WRect rect;
		const Bitmap* pNormal;
		const Bitmap* pOver;
		int reserved;
	};

	CRealMyRoomMain();
	virtual ~CRealMyRoomMain();
	virtual void OnPreLoadInit();
	virtual void DoReLoad();
	virtual void OnInit();
	virtual void OnLoad();
	virtual void OnDestroy();
	virtual void OnProcess(float);
	virtual void Display();

protected:
	void DrawMouseCursor();

public:
	virtual void OnDisplay();

protected:
	void LoadBg(const char*, int);
	void CheckUIPosition();
	void ProcessCamera(float);
	void CalculationCameraMat(WMatrix&, WVector, WVector, WVector, float, float,
		float);
	void ToggleTopViewMode();
	void SetCameraDisplayType(int);
	void RefreshAllUccClothes();

	virtual void OnAnotherHand(int, void*);
	void OpenLayout(const char*);
	void CloseLayout();
	void ShowPet(unsigned long);
	void HidePet(bool);
	void OnMyRoomInit(int);
	void OnMyRoomFinish();
	void Reinitialize(bool);
	void ReinitializePC(ePCBangItem);
	void InitMyRoomDialogs();
	void InitMyRoomControls();
	void InitMyRoomVariables();
	void LoadMyRoomImages();
	void LoadItemImages();
	virtual void CloseAllDialogs();
	void OnMiddleUIInit(int);
	void OnTabTxt1Init(int);
	void OnTabTxt2Init(int);
	void OnTabTxt3Init(int);
	void OnSelectedNameInit(int);
	void ChangeSubTab(subTab_t, bool);
	void SetPetAreaRect();
	void ChangeMainTab(mainTab_t, bool);
	void ChangeStatTab(statTab_t);
	void ChangeSelTab(selTab_t);
	void CharacterViewControl(bool);
	void ChangeCharacter(unsigned long);
	bool ChangeCaddie(unsigned long);
	void ChangeMascot(unsigned long);
	int ChangeClubset(unsigned long);
	void ChangeAztec(unsigned long);
	const Bitmap* GetItemIcon(unsigned long);
	void DrawSelItem(FrGraphicInterface*, unsigned long);
	void DrawCharacterList(FrGraphicInterface*, unsigned long);
	void DrawCaddieList(FrGraphicInterface*, unsigned long);
	void DrawMascotList(FrGraphicInterface*, unsigned long);
	void DrawAztecList(FrGraphicInterface*, unsigned long);
	void DrawClubList(FrGraphicInterface*, unsigned long);
	void OnPetInit(int);
	void OnSkinPetInit(int);
	void OnSkinPet2Init(int);
	void DrawSkinFrame();
	void DrawSkinSlot();
	void OnPetOwnerDraw(int);
	void OnFurnitureArrangeInit(int);
	void OnFurnitureArrangeButtonUP();
	void OnItemDescBgInit2(int);
	void OnItemDescBgOwnerDraw2(int);
	void OnItemDescBgInit(int);
	void OnItemDescBgOwnerDraw(int);
	void OnItemDescInit2(int);
	void OnItemNameInit(int);
	void OnItemDescInit(int);
	void SetItemDesc();
	void OnItemSlotBoxInit(int);
	void OnEquipCancelInit(int);
	void OnEquipCancelUp();
	void OnEquipResetInit(int);
	void OnEquipResetUp();
	void OnItemSlotInit(int);
	void OnItemSlotOwnerDraw(int);
	void OnItemSlotBtnDown();
	void OnItemSlotDClick();
	void OnGiftBgInit(int);
	void OnGiftDescInit(int);
	void SetGiftDescString(const char*, const char*, const char*, const char*);
	void OnStatFrameInit(int);
	void OnStatTextInit(int);
	void OnPowerBarInit(int);
	void OnControlBarInit(int);
	void OnImpactBarInit(int);
	void OnSpinBarInit(int);
	void OnCurveBarInit(int);
	void TurnOnItemSelect();
	void TurnOffItemSelect();
	void TurnOnStat();
	void TurnOffStat();
	void OnSelFrameInit(int);
	void OnStatBoxInit(int);
	void OnSelItemIconInit(int);
	void OnSelItemIconOwnerDraw(int);
	void OnSelItemInit(int);
	void OnSelItemOwnerDraw(int);
	void OnSelItemUp();
	void OnSelItemPrevInit(int);
	void OnSelItemPrevUp();
	void OnSelItemNextInit(int);
	void OnSelItemNextUp();
	void OnStatPartInit(int);
	void OnStatPartUp();
	void OnStatPartOwnerDraw(int);
	void OnStatAllInit(int);
	void OnStatAllUp();
	void OnStatAllOwnerDraw(int);
	void OnTabMainInit(int);
	void OnTabMainButtonUp();
	void OnTabSubInit(int);
	void OnTabSubButtonUp();
	void OnGiftPageOwnerDraw(int);
	void OnGiftPageLBDown();
	void OnCardBookInit(int);
	void OnCardBookOwnerDraw(int);
	void SetCardBookVisible(bool);
	void BuildCardPackList();
	void OnCardPackListInit(int);
	void OnCardPackListDraw(int);
	void OnCardPackListDown();
	void OnCardPackListRbUp();
	void OnCardBookIndexInit(int);
	void OnCardBookIndexLBDown();
	void OnCardBookIndexMouseOver();
	void OnCardUseInit(int);
	void OnCardUseLBDown();
	bool OnCardBoxDlgResult(int, FrForm*);
	void OnCardPropertyInit(int);
	void OnTabCardPackInit(int);
	void OnTabCardPackLBDown();
	void OnTabCardVolume1Init(int);
	void OnTabCardVolume1LBDown();
	void OnTabCardVolume2Init(int);
	void OnTabCardVolume2LBDown();
	void OnTabCardVolume3Init(int);
	void OnTabCardVolume3LBDown();
	void OnTabCardVolume4Init(int);
	void OnTabCardVolume4LBDown();
	void BoundCardPackPage(FrButton*, unsigned char);
	void OnCardPackInit1(int);
	void OnCardPackInit2(int);
	void OnCardPackInit3(int);
	void OnCardPackInit4(int);
	void OnCardPackInit5(int);
	void OnCardPackInit6(int);
	void OnCardPackInit7(int);
	void OnCardPackInit8(int);
	void CardPackBehavior(unsigned char, unsigned char);
	void OnCardPackLBDown1();
	void OnCardPackLBDown2();
	void OnCardPackLBDown3();
	void OnCardPackLBDown4();
	void OnCardPackLBDown5();
	void OnCardPackLBDown6();
	void OnCardPackLBDown7();
	void OnCardPackLBDown8();
	void OnCardPackRBDown1();
	void OnCardPackRBDown2();
	void OnCardPackRBDown3();
	void OnCardPackRBDown4();
	void OnCardPackRBDown5();
	void OnCardPackRBDown6();
	void OnCardPackRBDown7();
	void OnCardPackRBDown8();
	void BoundCardPage(FrButton*, unsigned char);
	void OnCardInit1(int);
	void OnCardInit2(int);
	void OnCardInit3(int);
	void OnCardInit4(int);
	void OnCardInit5(int);
	void OnCardInit6(int);
	void OnCardInit7(int);
	void OnCardInit8(int);
	void OnCardInit9(int);
	void OnCardInit10(int);
	void OnCardInit11(int);
	void OnCardInit12(int);
	void CardBehavior(unsigned char, unsigned char);
	void OnCardLBDown1();
	void OnCardLBDown2();
	void OnCardLBDown3();
	void OnCardLBDown4();
	void OnCardLBDown5();
	void OnCardLBDown6();
	void OnCardLBDown7();
	void OnCardLBDown8();
	void OnCardLBDown9();
	void OnCardLBDown10();
	void OnCardLBDown11();
	void OnCardLBDown12();
	void OnCardRBDown1();
	void OnCardRBDown2();
	void OnCardRBDown3();
	void OnCardRBDown4();
	void OnCardRBDown5();
	void OnCardRBDown6();
	void OnCardRBDown7();
	void OnCardRBDown8();
	void OnCardRBDown9();
	void OnCardRBDown10();
	void OnCardRBDown11();
	void OnCardRBDown12();
	void OnItemListInit(int);
	void DrawStatus(FrGraphicInterface*, FrListItem*, short* const,
		short* const);
	void DrawStatus2(FrGraphicInterface*, FrListItem*, short* const,
		short* const, float);
	void DrawStatusCard(FrGraphicInterface*, FrListItem*, short* const, float);
	void OnItemListOwnerDraw(int);
	void MoveGift(sGiftInfo*);
	bool OnCardRemoveDlgResult(int, FrForm*);
	void OpenTikiReport(sItemInfo*);
	void OpenNewYearMoney(sItemInfo*);
	void OpenEventGiftBox(sItemInfo*);
	void OpenS4EventGiftBox(sItemInfo*);
	void OnItemListBtnDown();
	void OnItemListMouseMove();
	void OnItemListRBtnDown();
	void OnItemListDClick();
	void OnPowerEditInit(int);
	void OnControlEditInit(int);
	void OnImpactEditInit(int);
	void OnSpinEditInit(int);
	void OnCurveEditInit(int);
	void OnUpgradeInit(int);
	void OnUpgradeUp();
	void OnDowngradeInit(int);
	void OnDowngradeUp();
	void OnTooltipHoverOn(int);
	void OnTooltipHoverOff(int);
	void GetTotalStatus(unsigned char* const, unsigned char* const,
		unsigned char* const);
	void GetCharacterStatus(unsigned char* const, unsigned char* const);
	void GetCaddieStatus(unsigned char* const, unsigned char* const);
	void GetClubSetStatus(unsigned char* const, unsigned char* const);
	void SetLevelBar();
	void BuildDisplayStand(subTab_t);
	void ShowMainModel();
	void DrawSelectItemList(FrListItem*);
	bool SelectItemList(unsigned long);
	bool SelectItemList(FrListItem*);
	bool UnselectItemFromItemList(unsigned long);
	bool UnselectItemList(unsigned long);
	void UnselectItemList(FrListItem*);
	int GetItemNumInMyPocket(unsigned long);
	void Net_EquipCharacter();
	void Net_EquipDresserParts();
	void Net_EquipCaddie();
	void Net_EquipItem();
	void Net_EquipGear();
	void Net_EquipSkin();
	void Net_StartPeriodItem(unsigned long);
	void Net_EquipMascot();
	bool OnEnchantDlgResult(int, FrForm*);
	bool OnPangGiftDlgResult(int, FrForm*);
	bool OnReturnGiftDlgResult(int, FrForm*);
	bool OnMoveGiftDlgResult(int, FrForm*);
	bool OnMascotInfoDlgResult(int, FrForm*);
	bool OnUpgradeDlgResult(int, FrForm*);
	bool OnNoteDlgResult(int, FrForm*);
	bool OnFunritureSaveMsgdlgResult(int, FrForm*);
	bool OnOfflineShopEditMsgDlgResult(int, FrForm*);
	bool OnFurnitrueListDeleteDlgResult(int, FrForm*);
	bool OnCaddieInfoDlgResult(int, FrForm*);
	bool OnOpenTikiReport(int, FrForm*);
	bool OnOpenNewYearMoneyDlgResult(int, FrForm*);
	bool OnOpenEventGiftDlgResult(int, FrForm*);
	bool OnOpenS4EventGiftDlgResult(int, FrForm*);
	void OpenCaddieInfo(const sCaddieInfo&);

public:
	void OpenInformation(FrListItem*);
	void OpenInformation(unsigned long*, sGiftInfo*, unsigned long*);

protected:
	void EquipItem(sItemInfo*);
	void UnequipItem(unsigned long*);
	bool IsInItemList(unsigned long);
	void SafeLoadPet(unsigned long*, bool);
	void TurnOnItemStatus();
	void TurnOffItemStatus();
	void AutoScroll();
	bool OnFortuneOpenDlgResult(int, FrForm*);
	bool OnFortuneDlgResult(int, FrForm*);

public:
	virtual void HandleMsg(const MsgObject&);

protected:
	bool OnReplayModeOpenDlgResult(int, FrForm*);
	bool OnReplayDelOpenDlgResult(int, FrForm*);
	void OnFurniturePupInit(int);
	void OnFurniturePupButtonUp();
	void OnFurniturePdownInit(int);
	void OnFurniturePdownButtonUp();
	void OnFurnitureListNumInit(int);
	void OnFurnitureListNumBtnUp();
	bool OnFurnitureListEnterKey(int);
	void OnFurnitureListMaxNumInit(int);
	void OnFurniture_BtnListNumInit(int);
	void OnFurniture_BtnListNumBtnUp();
	void OnFurnitureResetInit(int);
	void OnFurnitureResetButtonUp();
	void OnFurnitureBackInit(int);
	void OnFurnitureBackButtonUp();
	void OnFurnitureSaveInit(int);
	void OnFurnitureSaveButtonUp();
	void OnFurnitureOutInit(int);
	void OnFurnitureOutButtonUp();
	void OnFurnitureBackGroundInit1(int);
	void OnFurnitureBackGroundInit2(int);
	void OnFurnitureBackGroundInit3(int);
	void OnFurnitureBackGroundInit4(int);
	void OnFurnitureBackGroundInit5(int);
	void OnFurnitureBackGroundInit6(int);
	void OnFurnitureBackGroundInit7(int);
	void OnFurnitureBackGroundInit8(int);
	void OnFurnitureScrollInit1(int);
	void OnFurnitureScrollInit2(int);
	void OnFurnitureScrollInit3(int);
	void BuildDisplayStandFurniture();
	void OnFurnitureTabMainInit(int);
	void OnFurnitureTabMainButtonUp();
	void ChangeFMainTab(eFurniture_Tab, bool);
	void OnFurnitureListInit(int);
	void OnFurnitureListOwnerDraw(int);
	void OnFurnitureListBtnDown();
	void OnBtnMenuInit(int);
	void OOnBtnMenuButtonUP();
	void OnBtn_MenuListInit(int);
	void SetListMenu();
	void OnBtn_MenuListOwnerDraw(int);
	void OnBtn_MenuListBtnDown();
	void OnBtn_MenuListRBtnDown();
	void OnBtn_MenuListMouseMove();
	void OnBtn_MenuListDClick();
	void SelectMenu(unsigned char);

public:
	void SetUIState(unsigned long);
	void ChangeFlags(unsigned long);
	bool GetFlags(unsigned long);

protected:
	void UIDisplayControl(bool);
	void FurnitureDisplayControl(bool);
	void PostDlgCreate();
	void ControlUiHideMode(int);
	void SetOverAndUnderBarVisible(bool);
	void SetMiddleUIControl(bool);
	void SetMiddleUIVisible(bool);
	void Furniture_ShowUI(bool);
	void Furniture_ShowUI_Left(bool);
	void OnEffectViewInit(int);
	void OnEffectViewOnwnerDraw(int);
	void OnRightMenuInit(int);
	void OnRightMenuLBtnDown();
	void OnRightMenuRBtnDown();
	void UpdateRightMenu();
	void PopupRightMenu(int, int);

private:
	void ExecuteMiniButtonDown(sItemInfo*);

protected:
	void OnUnderBar_BackUp();
	void FurnitureUse();
	void FurnitureRun();
	void FurnitureFunctionChange();
	void OnUnderBar_ItemStorageUp();
	virtual void OnUnderBar_MyRoomUp();
	void OnUnderBar_GiftUp();
	bool OnPostDlgResult(int, FrForm*);
	bool OnFurniture_AbilityDlgResult(int, FrForm*);
	bool OnPostSendDlgResult(int, FrForm*);
	bool OnPostSendDlg2Result(int, FrForm*);
	bool OnAuthDlgResult(int, FrForm*);
	bool OnAuthDlgFaileResult(int, FrForm*);

public:
	bool GetArrangement(unsigned long, unsigned long);
	void RMR_UpdateDB();

protected:
	void LoadCardImages();
	void DeleteCardImages();
	WOverlay* GetCardImg(unsigned long);
	void DrawCard();
	void DrawPartsSlot(FrGraphicInterface*, FrListItem*);
	bool OnCardDlgResult(int, FrForm*);
	bool OnCardPackDlgResult(int, FrForm*);
	bool OnItemAttachCardDlgResult(int, FrForm*);
	bool OnResultDlg_Report(int, FrForm*);
	bool OnUccDrawDlgResult(int, FrForm*);
	bool OnUccCopyDlgResult(int, FrForm*);
	void OpenEventXmasGiftBox(sItemInfo*);
	bool OnOpenEventXmasGiftDlgResult(int, FrForm*);
	void OpenNewRandomBox(sItemInfo*);
	bool OnOpenNewRandomBoxDlgResult(int, FrForm*);

public:
	void OpenItemStorageNotice();
	bool OnResultItemStorageNotice(int, FrForm*);
	void OpenItemStorageAccessPanel(unsigned char);
	bool OnResultItemStorageAccessPanel(int, FrForm*);

protected:
	void OnInitItemStorageBGI(int);
	void OnOwnerDrawItemStorageBGI(int);
	void OnInitItemStoragePang(int);
	void OnOwnerDrawItemStoragePang(int);
	void OnInitItemStoragePangMinus(int);
	void OnLBDownItemStoragePangMinus();
	void OnInitItemStoragePangPlus(int);
	void OnLBDownItemStoragePangPlus();
	void OnInitItemStorageList(int);
	void OnOwnerDrawItemStorageList(int);
	void OnLBDownItemStorageList();
	void OnRBDownItemStorageList();
	void OnDBClickItemStorageList();
	void OnMouseMoveItemStorageList();
	void OnInitItemStorageIndexMsg(int);
	void OnOwnerDrawItemStorageIndexMsg(int);
	void OnLBDownItemStorageIndexMsg();
	void OnInitItemStoragePageNext(int);
	void OnLBDownItemStoragePageNext();
	void OnInitItemStoragePagePrev(int);
	void OnLBDownItemStoragePagePrev();
	void OnInitItemStorageClose(int);
	void OnLBDownItemStorageClose();
	void OnInitItemStorageDragDrop(int);
	void OnOwnerDrawItemStorageDragDrop(int);

public:
	void SetItemStorageVisible(bool);

protected:
	void DrawCutinFrame();
	WOverlay* GetCutinImg(unsigned long);
	void DeleteCutinImages();
	bool OnRentalExtensionDlgResult(int, FrForm*);

public:
	LightSet m_light;
	WTL::CString m_bgName;
	bool m_bMsgFlag;
	bool m_bAvatarMove;
	int m_glowBallTex;
	FrWnd* m_pRightMenu;
	WVector m_camAngle;
	WVector m_camLookAt;
	float m_fov;
	float m_targetFov;
	float m_defaultFov;
	float m_camDistance;
	float m_camHeight;
	float m_avatarCamHeight;
	float m_motionTime;
	int m_reserved248;
	WVector m_savedCamAngle;
	bool m_bTopView;
	float m_topViewFov;
	int m_reserved260;
	WVector m_topViewOffset;
	float m_glowTime;
	float m_camLimit[4];
	int m_camLimitType;
	WTitleFont* m_pTitleFont;
	int m_reserved28c[11];
	int m_reserved2b8;
	int m_shadowTex;
	CFxSequence* m_pPointFx;
	bool m_b2c4;
	WVector m_startPos;
	WVector m_camTargetPos;
	float m_camTargetFov;
	FrGroupBox* m_pMiddleUI;
	FrStatic* m_pSelectedName;
	FrViewer* m_pStatPart;
	FrViewer* m_pStatAll;
	FrTabButton* m_pTabMain;
	FrTabButton* m_pTabSub;
	FrWnd* m_p2fc;
	FrWnd* m_p300;
	FrListBox* m_pItemList;
	FrArea* m_pTabTxt[3];
	FrArea* m_pPetArea;
	FrWnd* m_p318;
	FrArea* m_pSelFrame;
	FrArea* m_pSelItemIcon;
	FrButton* m_pSelItem;
	FrGroupBox* m_pStatBox;
	FrGaugeBar* m_pStatBar[5];
	FrStatic* m_pStatEdit[5];
	FrButton* m_pSelItemPrev;
	FrButton* m_pSelItemNext;
	FrArea* m_pItemDescBg;
	FrEdit* m_pItemDesc;
	FrViewer* m_pItemDescBg2;
	FrEdit* m_pItemDesc2;
	FrEdit* m_pItemName;
	FrArea* m_pSkinPet;
	FrArea* m_pSkinPet2;
	FrArea* m_pPet;
	FrListBox* m_pItemSlot;
	FrButton* m_pFurnitureArrange;
	FrGroupBox* m_pItemSlotBox;
	FrButton* m_pEquipCancel;
	FrButton* m_pEquipReset;
	FrButton* m_pUpgrade;
	FrButton* m_pDowngrade;
	FrWnd* m_p398;
	FrViewer* m_pGiftBg;
	FrEdit* m_pGiftDesc;
	FrArea* m_pStatFrame;
	FrArea* m_pStatText;
	FrTabButton* m_pFurnitureTabMain;
	FrListBox* m_pFurnitureList;
	FrListBox* m_pBtnMenuList;
	FrButton* m_pFurniturePup;
	FrButton* m_pFurniturePdown;
	FrEdit* m_pFurnitureListNum;
	FrEdit* m_pFurnitureListMaxNum;
	FrButton* m_pFurnitureBtnListNum;
	FrButton* m_pFurnitureReset;
	FrButton* m_pFurnitureBack;
	FrButton* m_pFurnitureSave;
	FrButton* m_pFurnitureOut;
	FrArea* m_pFurnitureBackGround[8];
	FrArea* m_pFurnitureScroll[3];
	FrWnd* m_p408[6];
	FrButton* m_pBtnMenu;
	FrForm* m_p424;
	FrEnchantDlg* m_pEnchantDlg;
	FrDowngradeDlg* m_pDowngradeDlg;
	FrDowngradeDlg* m_pDowngradeDlg2;
	FrPangGiftDlg* m_pPangGiftDlg;
	FrMascotInfoDlg* m_pMascotInfoDlg;
	FrNoteDlg* m_pNoteDlg;
	FrFortuneDlg* m_pFortuneDlg;
	FrForm* m_pFortuneOpenDlg;
	FrReplayModeDlg* m_pReplayModeDlg;
	FrCardUseDlg* m_pCardUseDlg;
	FrItemAttachCardDlg* m_pItemAttachCardDlg;
	FrOpenCardPackDlg* m_pOpenCardPackDlg;
	FrForm* m_pReserved458;
	FrUccDrawDlg* m_pUccDrawDlg;
	FrUccCopyDlg* m_pUccCopyDlg;
	FrUniteResultDlg* m_pUniteResultDlg;
	FrCardRemove* m_pCardRemoveDlg;
	FrForm* m_pReserved46c[2];
	CPost_ReciveDlg* m_pPostReciveDlg;
	CPost_SendDlg* m_pPostSendDlg;
	CRealMyRoom_AuthorityDlg* m_pAuthorityDlg;
	CFurniture_AbilityDlg* m_pFurnitureAbilityDlg;
	const Bitmap* m_pItemBase[4];
	const Bitmap* m_pReserved494[3];
	const Bitmap* m_pStatDot[5][2];
	const Bitmap* m_pSelect[4];
	const Bitmap* m_pSelectCursor[2];
	const Bitmap* m_pStatChar[3];
	const Bitmap* m_pStatClub[3];
	const Bitmap* m_pStatCaddie[3];
	const Bitmap* m_pStatAllBtn[3];
	const Bitmap* m_pSelectNum[5][3];
	const Bitmap* m_pSelectDivide;
	const Bitmap* m_pGender[2];
	const Bitmap* m_pMasterSign;
	const Bitmap* m_pTeamBase;
	const Bitmap* m_pItemSlotBmp[4];
	const Bitmap* m_pItemDescBmp[2];
	const Bitmap* m_pReserved578[4];
	const Bitmap* m_pTipFrame[9];
	const Bitmap* m_pGiftPage[5];
	const Bitmap* m_pUccState[3];
	const Bitmap* m_pMenuIcon[13];
	const Bitmap* m_pMenuText[13];
	const Bitmap* m_pMenuOver;
	const Bitmap* m_pMenuDisable;
	const Bitmap* m_pReserved63c;
	const Bitmap* m_pArrangeItemBase[4];
	const Bitmap* m_pMarkBegin;
	FrWnd* m_pFurnitureWnd[2];
	int m_reserved65c;
	FrWnd* m_pFurnitureUI[9];
	FrArea* m_pEffectView;
	FrWnd* m_p688;
	int m_reserved68c[2];
	FrWnd* m_pLayout;
	float m_infoOffsetX;
	float m_infoOffsetY;
	WPoint m_itemDesc2Pos;
	std::map<unsigned long, const Bitmap*> m_itemIcon;
	std::map<unsigned int, WOverlay*> m_cardImg;
	std::map<unsigned int, WOverlay*> m_cutinImg;
	int m_reserved6cc[3];
	bool m_bSelectItemList;
	char* m_pCharStat;
	unsigned long* m_pCharParts;
	unsigned long m_partsUid[24];
	unsigned long* m_pAuxParts;
	unsigned long m_charTypeId;
	unsigned long m_charUid;
	unsigned long m_caddieTypeId;
	unsigned long m_caddieUid;
	unsigned long m_caddieParts;
	unsigned long m_mascotTypeId;
	unsigned long m_mascotUid;
	unsigned long m_clubSetUid;
	unsigned long m_clubSetTypeId;
	short* m_pClubSetStat;
	unsigned long m_aztecTypeId;
	statTab_t m_statTab;
	mainTab_t m_mainTab;
	subTab_t m_subTab;
	selTab_t m_selTab;
	bool m_b784;
	WPoint m_itemDescOffset;
	WPoint m_itemDescPos;
	WPoint m_furnitureDescPos;
	eFurniture_Tab m_furnitureTab;
	int m_furniturePage;
	int m_furnitureMaxPage;
	sGiftInfo* m_pMoveGift;
	unsigned long m_newYearMoneyUid;
	unsigned long m_eventGiftUid;
	unsigned long m_xmasGiftUid;
	unsigned long m_boxTypeId;
	int m_reserved7c0[4];
	unsigned long m_tikiReportUid;
	unsigned long m_tikiReportTypeId;
	unsigned long m_tikiReportState;
	CExhibition* m_pExhibition;
	CAnotherHand m_anotherHand;
	std::map<unsigned int, sCharacterInfo> m_charInfoMap;
	sUserEquip m_userEquip;
	miniBtn_t m_miniBtn[14];
	std::list<sSpecialTrophy> m_trophyList;
	int m_statSubject;
	unsigned long m_skinTypeId[6];
	float m_skinScroll[2];
	int m_skinScrollDir[2];
	unsigned long m_cardTypeId;
	unsigned long m_giftMaxPage;
	unsigned long m_giftPageNum;
	unsigned long m_giftPage;
	unsigned long m_giftPageGroup;
	WRect m_giftPageRect[7];
	bool m_bc24;
	bool m_bGiftMoveReady;
	bool m_bc26;
	WRect m_uccCopyRect;
	WRect m_uccDrawRect;
	WPolySoup::sCamera* m_pRoomCamera;
	WPolySoup::sCamera* m_pFurnitureCamera;
	WPolySoup::sCamera* m_pDefaultCamera;
	WPolySoup::sCamera* m_pDefaultCamera2;
	WPolySoup::sCamera* m_pSaveCamera[4];
	WPolySoup::sCamera* m_pUICamera;
	WPolySoup::sPoint* m_pLimitPoint;
	float m_nearClip;
	float m_farClip;
	WVector m_roomBound[8];
	WVector m_camTarget;
	bool m_bCamTarget;
	int m_camDisplayType;
	int m_reservedcec[8];
	WMatrix m_savedCamMat;
	float m_savedCamFov;
	int m_reservedd40;
	bool m_bd44;
	bool m_bShowCharacter;
	bool m_bd46;
	int m_reservedd48;
	int m_uiHideMode;
	int m_prevUiHideMode;
	bool m_bFlag[5];
	int m_offlineShopEdit;
	WOverlay* m_pSexMark[6];
	WOverlay* m_pMannerMark[2];
	WOverlay* m_pAngelMark[2];
	bool m_bCardBookVisible;
	CCardBook* m_pCardBook;
	FrItemStorageNotice* m_pItemStorageNotice;
	FrItemStorageAccessPanel* m_pItemStorageAccessPanel;
	bool m_bItemStorageVisible;
	WPoint m_itemStoragePos;
	FrArea* m_pItemStorageBG;
	FrArea* m_pItemStoragePang;
	FrButton* m_pItemStoragePangMinus;
	FrButton* m_pItemStoragePangPlus;
	FrListBox* m_pItemStorageList;
	FrListBox* m_pItemStorageIndexMsg;
	FrButton* m_pItemStoragePageNext;
	FrButton* m_pItemStoragePagePrev;
	FrButton* m_pItemStorageClose;
	FrArea* m_pItemStorageDragDrop;
	FrListBox* m_pCardPackList;

	bool IsItemStorageVisible() const { return m_bItemStorageVisible; }

	DECLARE_FRESH_MSGMAP()
};
