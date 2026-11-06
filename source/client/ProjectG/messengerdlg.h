#pragma once

#include <string>
#include <list>
#include <map>
#include "frform.h"
#include "frlistbox.h"
#include "frcontextmenuctrl.h"
class FrMessengerDlg;
class FrMessengerChatDlg;
class FrNoteDlg;
class FrAddFriendDlg;
class FrOptionDlg;
class FrMemoDlg;
class FrGuildNoticeDlg;
class NewsReader;

namespace MSN
{
	enum eStatusType;
}

enum eConnectStatus;

struct sMsnChat
{
	std::string nick;
	std::string msg;
	unsigned long reserved;

	sMsnChat()
	{
		nick = "";
		msg = "";
		reserved = 0;
	}
};

#include "frbutton.h"
#include "frarea.h"

struct sChatSlot
{
	FrMessengerChatDlg* pDlg;
	unsigned long uid;
	bool bAlarm;
	std::list<sMsnChat> chatList;
	WRect rect;
	std::string tempChat;

	sChatSlot()
	{
		pDlg = NULL;
		uid = 0;
		bAlarm = false;

		tempChat = "";
		memset(&rect, 0, sizeof(rect));
		chatList.clear();
	}
};

class FrMessengerDlg : public FrForm
{
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI() const { return &m_RTTI; }
	friend IObject* FrMessengerDlgMakeInstance();

	FrMessengerDlg();

	virtual bool Close(bool bResult);

	void UpdateFriendList();
	void SetAddFriendNick(const char* nick, unsigned long uid);
	void SetTargetNick(const char* nick);
	void OpenAddFriendNickDlg();

	void EnableInvite(bool bEnable) { m_bEnableInvite = bEnable; }

protected:
	virtual bool OnLButtonDown(const WPoint& pt);

	void OnMessenger_FriendListInit(int param);
	void OnMessenger_FriendListOwnerDraw(int param);
	void OnMessenger_FriendListLBtnDown();
	void OnMessenger_FriendListRBtnDown();
	void OnMessenger_FriendListDBtnDown();
	void OnMessenger_GuildListInit(int param);
	void OnMessenger_GuildListOwnerDraw(int param);
	void OnMessenger_GuildListLBtnDown();
	void OnMessenger_GuildListRBtnDown();
	void OnMessenger_GuildListDBtnDown();
	void OnMessenger_CloseLBtnDown();
	void OnMessenger_MyInfoInit(int param);
	void OnMessenger_MyInfoOwnerDraw(int param);
	void OnMessenger_FriendBtnInit(int param);
	void OnMessenger_FriendLBtnDown();
	void OnMessenger_GuildBtnInit(int param);
	void OnMessenger_GuildLBtnDown();
	void OnMessenger_GuildNoticeBtnInit(int param);
	void OnMessenger_GuildNoticeLBtnDown();
	void OnMessenger_FindLBtnDown();
	void OnMessenger_RightMenuInit(int param);
	void OnMessenger_RightMenuLBtnDown();
	void OnMessenger_StateBtnInit(int param);
	void OnMessenger_StateLBtnDown();
	void OnMessenger_StateMenuInit(int param);
	void OnMessenger_StateMenuLBtnDown();

	void OnChatToFriend();
	void OnNoteToFriend();
	void OnInfoFriend();
	void OnAcceptFriend();
	void OnRemoveFriend();
	void OnBlockFriend();
	void OnBlockCancelFriend();
	void OnAliasToFriend();
	void OnInviteFriend();
	void OnInviteFriendRealMyRoom(bool bRealMyRoom);
	void OnGoWithFriend();

	bool OnAddFriendDlgResult(int result, FrForm* pForm);
	bool OnOptionDlgResult(int result, FrForm* pForm);
	bool OnNoteDlgResult(int result, FrForm* pForm);
	bool OnMemoDlgResult(int result, FrForm* pForm);
	bool OnRemoveFriendDlgResult(int result, FrForm* pForm);
	bool OnBlockFriendDlgResult(int result, FrForm* pForm);
	bool OnBlockCancelFriendDlgResult(int result, FrForm* pForm);
	bool OnGuildNoticeDlgResult(int result, FrForm* pForm);

	void DrawFriendGroup(int param);
	void DrawGuildGroup(int param);
	void SetCount();
	void RestoreOther(unsigned long uid);
	MSN::eStatusType GetBuddyStatus(sFriend* pFriend);

	FrListBox* m_pFriendList;
	FrListBox* m_pGuildList;
	FrContextMenuCtrl* m_pRightMenu;
	FrContextMenuCtrl* m_pStateMenu;
	const Bitmap* m_pStatusIcon[5];
	const Bitmap* m_pSexIcon[3][2];
	const Bitmap* m_pGroupIcon[2];
	unsigned long m_selectUID;
	FrButton* m_pTabBtn[2];
	FrButton* m_pNoticeBtn;
	FrButton* m_pStateBtn;
	unsigned long m_unknown168;
	FrArea* m_pMyInfo;
	int m_friendCount;
	int m_friendOnCount;
	int m_guildCount;
	int m_guildOnCount;
	FrNoteDlg* m_pNoteDlg;
	FrAddFriendDlg* m_pAddFriendDlg;
	FrOptionDlg* m_pOptionDlg;
	unsigned long m_unknown18c;
	FrMemoDlg* m_pMemoDlg;
	sFriend m_groupRow[3];
	std::list<sFriend*> m_friendOnList;
	std::list<sFriend*> m_friendOffList;
	std::list<sFriend*> m_guildList;
	std::list<sFriend*> m_guildOnList;
	std::list<sFriend*> m_guildOffList;
	bool m_bEnableInvite;

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap() const;
};

struct MSNServerInfo : public sGameServerInfo
{
	MSNServerInfo() { bInvalid = false; }

	bool bInvalid;
};

class CMessengerInfo : public WSingleton<CMessengerInfo>, public FrCmdTarget
{
	friend class MSNUnit;
	friend class FrMessengerDlg;
	friend class FrMessengerChatDlg;
	friend class CTask;
	friend class CLobbyTask;

public:
	CMessengerInfo();

	FrMessengerDlg* GetMessengerDlg() { return m_pMessengerDlg; }

	void Process(float delta);
	void Open(bool bForce);
	bool Close(bool bForce);
	void Toggle();
	void CloseWindow();
	void Refresh();
	bool IsVisible();

	void Notify1(const char* msg);
	void Notify2(const char* msg);
	void Notify3();
	void Notify4(bool bAlarm);
	void AlarmBtnUp(unsigned long uid);
	void RecvChat(sFriend* pFriend, std::string msg);
	void RepositionPopupDlg(FrMessengerChatDlg* pDlg, int index);

	void SetOfflineUserNick(const char* nick, unsigned long uid);
	void SetTargetNick(const char* nick);
	void SetMyStatus(MSN::eStatusType status);
	void SetGameFocus(bool bFocus);
	void InvalidateCurrentServer();

	MSN::eStatusType GetMyStatus() { return m_myStatus; }

	bool IsGameFocus() { return m_bGameFocus; }

	void SetComplete(eConnectStatus status) { m_complete = status; }
	eConnectStatus GetComplete() { return m_complete; }

	bool IsWaitingForServerList() { return m_bWaitServerList; }
	void SetServerListWaitingState(bool bWait) { m_bWaitServerList = bWait; }

	void SetCurrentServerIndex(int index) { m_curServerIndex = index; }

	void EnableInvite(bool bEnable) { m_bEnableInvite = bEnable; }

protected:
	bool OnMessengerDlgResult(int result, FrForm* pForm);
	bool OnMessengerChatDlgResult(int result, FrForm* pForm);
	void SaveCurrentFocus();
	void ApplySavedFocus();

	std::list<sChatSlot> m_chatSlotList;

public:
	WRect m_dlgRect;

	WRect m_alarmRect;

protected:
	std::map<unsigned long, sUserInfoTime> m_userInfoTimeMap;
	std::list<MSNServerInfo> m_serverList;
	FrMessengerDlg* m_pMessengerDlg;
	float m_notifyTime;
	float m_checkTime;
	bool m_bNotify;
	bool m_bGameFocus;
	bool m_bAutoAway;
	eConnectStatus m_complete;
	bool m_bWaitServerList;
	int m_curServerIndex;
	MSN::eStatusType m_myStatus;
	int m_popupIndex;
	struct GroupExpansion
	{
		bool friendOnline;
		bool friendOffline;
		bool unknown;
	} m_groupExpansion;
	std::list<unsigned long> m_focusList;
	NewsReader* m_pNewsReader;
	FrGuildNoticeDlg* m_pGuildNoticeDlg;
	unsigned long m_unknownAC;
	unsigned long m_unknownB0;
	bool m_bEnableInvite;
};

inline CMessengerInfo* MESSENGER()
{
	return CMessengerInfo::Instance();
}
