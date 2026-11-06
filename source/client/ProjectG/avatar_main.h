#pragma once

#include "stdafx.h"
#include "taskmain.h"
#include "scenemanager.h"
#include "golfdoc.h"
#include "ntavatar.h"
#include "avatar_avatar.h"

enum eSPAVILITYTYPE;
class ntAvatar;
class FrReportDlg;
class FrNoteDlg;
class FrJukeBoxDlg;
class FrTradeDlg;
class FrEmoticonDlg;
class FrAddFriendDlg;
class FrAvatarTipDlg;
class FrAvatarChatLogDlg;
class FrAvatarUserListDlg;
class FrComboBox;
class FrContextMenuCtrl;

class ntAvatarChatMain : public CTaskMain, public CRenderFuncPtr
{
	friend class ntAvatar;

public:
	DECLARE_OBJECT(ntAvatarChatMain)

	ntAvatarChatMain();
	virtual ~ntAvatarChatMain();

	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnInit();
	virtual void OnProcess(float delta);
	virtual void OnDisplay();
	virtual void OnDestroy();
	virtual void OnPreserveBack(const char* name);
	virtual void OnRestorePreserved();
	virtual void DoReLoad();
	virtual void HandleMsg(const MsgObject& msg);
	virtual void CloseAllDialogs();
	virtual void Display();

protected:
	virtual void OnUnderBar_ShopUp();
	virtual void OnUnderBar_MyRoomUp();
	virtual void OnUnderBar_CookieUp();
	virtual void OnUnderBar_RankingUp();

	CQuadTree& GetPVS();
	void SetLocalPVS(CQuadTree& pvs);
	void LoadBg(const char* name, int hole);
	void ProcessCamera(float delta);
	void ToggleTopViewMode();
	void CheckUIPosition();
	void PrintText(float x, float y, const char* text, float scale, int align,
		float alpha, unsigned long color);
	void RefreshAllUccClothes();

	void InitUI();
	void ProcessUI(float delta);
	void ResetFrWndPointers();
	void ChangePortraitImage(int index);
	void UpdateRightMenu();
	void PopupRightMenu(int x, int y);
	void ToggleJukeBox();
	void ToggleAvatarChatLog();
	void ToggleAvatarUserList();

	void OnAvatarChatMainInit(int param);
	void OnAvatarChatMainFinish();
	void OnPortraitInit(int param);
	void OnPortraitMaskAreaInit(int param);
	void OnPortraitMaskInit(int param);
	void OnPortraitMaskBtnUp();
	void OnPortraitTipButtonInit(int param);
	void OnPortraitTipButtonBtnUp();
	void OnPortraitJukeBoxButtonAreaInit(int param);
	void OnPortraitJukeBoxButtonInit(int param);
	void OnPortraitJukeBoxButtonBtnUp();
	void OnPortraitChatLogButtonInit(int param);
	void OnPortraitChatLogButtonBtnUp();
	void OnAvatarUserListButtonInit(int param);
	void OnAvatarUserListButtonBtnUp();
	void OnChatFrameInit(int param);
	void OnChatViewInit(int param);
	void OnEmoticonInit(int param);
	void OnEmoticonBtnUp();
	void OnLanguageInit(int param);
	void OnReportInit(int param);
	void OnReportBtnUp();
	void OnChatTargetInit(int param);
	void OnChatTargetBtnDown(int param);
	bool OnChatTargetEnterKey(int param);
	void OnChatInputInit(int param);
	bool OnChatInputEnterKey(int param);
	void OnRightMenuInit(int param);
	void OnRightMenuLBtnDown();
	void OnRightMenuRBtnDown();
	void OnAddFriend();
	void OnFollowAvatar();
	void OnKickAvatar();
	void OnNoteToFriend();
	void OnAvatarInfo();
	void OnOpenTradeShop();
	void OnEnterTradeShop();

	bool OnAvatarTipDlgResult(int result, FrForm* form);
	bool OnJukeBoxDlgResult(int result, FrForm* form);
	bool OnAvatarChatLogDlgResult(int result, FrForm* form);
	bool OnAvatarUserListDlgResult(int result, FrForm* form);
	bool OnReportResult(int result, FrForm* form);
	bool OnAddFriendDlgResult(int result, FrForm* form);
	bool OnTradeDlgResult(int result, FrForm* form);
	bool OnNoteDlgResult(int result, FrForm* form);
	bool OnEmoticonResult(int result, FrForm* form);
	bool OnPasswordDlgResult(int result, FrForm* form);

private:
	void StartSpEffect(ntAvatar* avatar, eSPAVILITYTYPE type);
	bool CanUseSPItem(eSPAVILITYTYPE type);
	bool SpCommand(const char* command);

	DECLARE_FRESH_MSGMAP()

protected:
	unsigned long m_reserved1e4[2];
	CWaveInfo m_waveInfo;
	CRiverInfo m_riverInfo;
	float m_glowTime;
	bool m_bGlow;
	int m_glowTex;
	char m_lastNick[22];
	char m_pickedNick[22];
	unsigned long m_pickedUID;
	WTL::CString m_bgName;
	WVector m_camAngle;
	WVector m_camLookAt;
	float m_camDist;
	float m_camTargetDist;
	float m_camDefaultDist;
	float m_camZoom;
	float m_camHeight;
	float m_camTargetHeight;
	float m_camYawVel;
	float m_camPitchVel;
	WVector m_camPos;
	bool m_bTopView;
	float m_topViewHeight;
	unsigned long m_reserved290;
	WVector m_topViewPos;
	bool m_bPortraitLoaded;
	WTitleFont* m_pFont;
	Bitmap* m_pPortrait[10];
	WOverlay* m_pIcon[6];
	WOverlay* m_pMannerIcon[2];
	WOverlay* m_pAngelIcon[2];
	WOverlay* m_pDive;
	WOverlay* m_pStateIcon1;
	WOverlay* m_pStateIcon2;
	WOverlay* m_pStateIcon3;
	WOverlay* m_pStateIcon4;
	WOverlay* m_pStateIcon5;
	WOverlay* m_pStateIcon6;
	WOverlay* m_pStateIconN1;
	WOverlay* m_pStateIconN2;
	WOverlay* m_pStateIconN3;
	WOverlay* m_pStateIconN4;
	WOverlay* m_pStateBar[9];
	WOverlay* m_pEmoticon;
	int m_shadowTex;
	char m_layoutName[260];
	bool m_bLayoutFlag;
	FrArea* m_pPortraitArea;
	FrButton* m_pPortraitMask;
	FrButton* m_pJukeBoxBtn;
	FrButton* m_pTipBtn;
	FrButton* m_pChatLogBtn;
	FrButton* m_pUserListBtn;
	FrFrame* m_pChatFrame;
	FrComboBox* m_pChatTarget;
	FrEdit* m_pChatInput;
	FrButton* m_pEmoticonBtn;
	FrArea* m_pLanguage;
	FrButton* m_pReportBtn;
	FrEdit* m_pChatView;
	FrEmoticonDlg* m_pEmoticonDlg;
	FrReportDlg* m_pReportDlg;
	FrNoteDlg* m_pNoteDlg;
	FrAddFriendDlg* m_pAddFriendDlg;
	FrForm* m_pUnusedDlg;
	FrForm* m_pPasswordDlg;
	FrJukeBoxDlg* m_pJukeBoxDlg;
	FrTradeDlg* m_pTradeDlg;
	FrAvatarTipDlg* m_pTipDlg;
	FrAvatarChatLogDlg* m_pChatLogDlg;
	FrAvatarUserListDlg* m_pUserListDlg;
	FrContextMenuCtrl* m_pRightMenu;
	int m_chatLineCount;
	int m_uiMode;
	unsigned long m_masterOID;
	unsigned char m_preservedMap;
	unsigned char m_preservedHole;
	bool m_bCameraLock;
	LightSet m_light;
	CQuadTree* m_pPVS;
};
