#include "messengerdlg.h"
#include "messengerchatdlg.h"
#include "buddymanager.h"
#include "notedlg.h"
#include "memodlg.h"
#include "addfrienddlg.h"
#include "guildnoticedlg.h"
#include "newsreader.h"
#include "user_info.h"
#include "golftask.h"
#include "lobbytask.h"
#include "shoptask.h"
#include "realmyroomtask.h"
#include "familytask.h"
#include "avatar_task.h"
#include "taskmanager.h"
#include "networksystem.h"
#include "projectg.h"
#include "inputmanager.h"
#include "chatmsg.h"
#include "netresourcemanager.h"

extern Fresh* g_pFresh;

bool FriendCompare(const void* left, const void* right)
{
	const sFriend* a = (const sFriend*)left;
	const sFriend* b = (const sFriend*)right;
	if (!a)
		return false;
	if (!b)
		return true;
	if (a->Uid == -1 || b->Uid == -1)
		return false;
	if (!a->IsLogOn && b->IsLogOn)
		return false;
	if (a->IsLogOn == 1 && !b->IsLogOn)
		return true;
	if (!a->IsBlock && b->IsBlock)
		return true;
	if (a->IsBlock == 1 && !b->IsBlock)
		return false;
	if (!a->IsAccept && b->IsAccept)
		return false;
	if (a->IsAccept == 1 && !b->IsAccept)
		return true;
	if (!a->IsAccept && !b->IsAccept)
	{
		if (a->IsAgree == 1 && !b->IsAgree)
			return true;
		if (!a->IsAgree && b->IsAgree)
			return true;
	}
	return false;
}

IMPLEMENT_OBJECT(FrMessengerDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrMessengerDlg, FrForm)
ON_FRESH_VI("friendlist", FRCMD_INIT,
	FrMessengerDlg::OnMessenger_FriendListInit)
ON_FRESH_VI("friendlist", FRCMD_OWNERDRAW,
	FrMessengerDlg::OnMessenger_FriendListOwnerDraw)
ON_FRESH_VV("friendlist", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_FriendListLBtnDown)
ON_FRESH_VV("friendlist", FRCMD_RBUTTONDOWN,
	FrMessengerDlg::OnMessenger_FriendListRBtnDown)
ON_FRESH_VV("friendlist", FRCMD_DBLCLICK,
	FrMessengerDlg::OnMessenger_FriendListDBtnDown)
ON_FRESH_VI("guildlist", FRCMD_INIT, FrMessengerDlg::OnMessenger_GuildListInit)
ON_FRESH_VI("guildlist", FRCMD_OWNERDRAW,
	FrMessengerDlg::OnMessenger_GuildListOwnerDraw)
ON_FRESH_VV("guildlist", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_GuildListLBtnDown)
ON_FRESH_VV("guildlist", FRCMD_RBUTTONDOWN,
	FrMessengerDlg::OnMessenger_GuildListRBtnDown)
ON_FRESH_VV("guildlist", FRCMD_DBLCLICK,
	FrMessengerDlg::OnMessenger_GuildListDBtnDown)
ON_FRESH_VV("cancel", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_CloseLBtnDown)
ON_FRESH_VI("myinfo", FRCMD_INIT, FrMessengerDlg::OnMessenger_MyInfoInit)
ON_FRESH_VI("myinfo", FRCMD_OWNERDRAW,
	FrMessengerDlg::OnMessenger_MyInfoOwnerDraw)
ON_FRESH_VI("friend_btn", FRCMD_INIT, FrMessengerDlg::OnMessenger_FriendBtnInit)
ON_FRESH_VV("friend_btn", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_FriendLBtnDown)
ON_FRESH_VI("guild_btn", FRCMD_INIT, FrMessengerDlg::OnMessenger_GuildBtnInit)
ON_FRESH_VV("guild_btn", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_GuildLBtnDown)
ON_FRESH_VI("notice", FRCMD_INIT,
	FrMessengerDlg::OnMessenger_GuildNoticeBtnInit)
ON_FRESH_VV("notice", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_GuildNoticeLBtnDown)
ON_FRESH_VV("find_btn", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_FindLBtnDown)
ON_FRESH_VI("rmenu", FRCMD_INIT, FrMessengerDlg::OnMessenger_RightMenuInit)
ON_FRESH_VV("rmenu", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_RightMenuLBtnDown)
ON_FRESH_VI("state", FRCMD_INIT, FrMessengerDlg::OnMessenger_StateBtnInit)
ON_FRESH_VV("state", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_StateLBtnDown)
ON_FRESH_VI("state_menu", FRCMD_INIT, FrMessengerDlg::OnMessenger_StateMenuInit)
ON_FRESH_VV("state_menu", FRCMD_LBUTTONDOWN,
	FrMessengerDlg::OnMessenger_StateMenuLBtnDown)
END_FRESH_MSGMAP()

FrMessengerDlg::FrMessengerDlg()
	: m_pRightMenu(NULL),
	  m_pStateMenu(NULL),
	  m_pMyInfo(NULL),
	  m_pNoteDlg(NULL),
	  m_pAddFriendDlg(NULL),
	  m_pOptionDlg(NULL),
	  m_pMemoDlg(NULL)
{
	memset(m_groupRow, 0, sizeof(m_groupRow));
	m_groupRow[0].IsLogOn = true;
	m_groupRow[0].State = MESSENGER()->m_groupExpansion.friendOnline;
	m_groupRow[0].Uid = -1;
	m_groupRow[1].IsLogOn = false;
	m_groupRow[1].State = MESSENGER()->m_groupExpansion.friendOffline;
	m_groupRow[1].Uid = -1;
	m_groupRow[2].GuildFriend = false;
	m_groupRow[2].IsLogOn = false;
	m_groupRow[2].IsAccept = false;
	m_groupRow[2].IsAgree = false;
	m_groupRow[2].IsBlock = false;
	m_groupRow[2].IsPlay = false;
	m_groupRow[2].IsDive = false;
	m_groupRow[2].State = 5;
	m_groupRow[2].Uid = -1;
	m_pFriendList = NULL;
	m_pGuildList = NULL;
	m_pNoticeBtn = NULL;
	m_pStateBtn = NULL;
	m_unknown168 = 0;
	m_friendCount = 0;
	m_friendOnCount = 0;
	m_guildCount = 0;
	m_guildOnCount = 0;
	if (IsLocalContent(S4_INVITE_FRIEND))
		m_bEnableInvite = false;
}

bool FrMessengerDlg::Close(bool bResult)
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	for (std::list<sChatSlot>::iterator it = slots.begin(); it != slots.end();
		++it)
	{
		if ((*it).pDlg)
		{
			(*it).pDlg->FindNextTopFocus(false);
			break;
		}
	}
	FrForm::Close(true);
	return true;
}

void FrMessengerDlg::SetAddFriendNick(const char* nick, unsigned long uid)
{
	if (m_pAddFriendDlg)
	{
		m_pAddFriendDlg->SetNick(nick);
		m_pAddFriendDlg->SetFindUID(uid);
	}
}

void FrMessengerDlg::SetTargetNick(const char* nick)
{
	if (m_pAddFriendDlg)
		m_pAddFriendDlg->SetTargetNick(nick);
}

void FrMessengerDlg::OpenAddFriendNickDlg()
{
	OnMessenger_FindLBtnDown();
}

bool FrMessengerDlg::OnLButtonDown(const WPoint& pt)
{
	m_pRightMenu->SetVisible(false);
	m_pStateMenu->SetVisible(false);
	FrForm::OnLButtonDown(pt);
	return false;
}

void FrMessengerDlg::OnMessenger_MyInfoInit(int param)
{
	m_pMyInfo = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrMessengerDlg::OnMessenger_MyInfoOwnerDraw(int param)
{
	if (!m_pMyInfo)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	float alpha = m_wndAlpha2 * m_wndAlpha;
	unsigned long color = ((int)(alpha * 255.0f) << 24) | 0xffffff;
	gdi->SetAlpha(alpha);
	WRect rect = m_pMyInfo->GetRect();
	WPoint pos = rect.TopLeft();
	if (m_pTabBtn[1]->GetStatus() == FrButton::PRESSED)
	{
		const Bitmap* bitmap = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_myInfo.info.szEmblemName);
		if (bitmap)
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 1, pos.y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				color, 0);
	}
	if (m_pStateBtn->IsVisible() && m_pStateBtn)
	{
		const Bitmap* bitmap = m_pSexIcon[0][Doc()->m_myInfo.info.gender];
		if (bitmap)
		{
			gdi->DrawTexture(bitmap,
				WRect(pos.x + 1, pos.y - 3, (float)bitmap->Width(),
					(float)bitmap->Height()),
				color, 0);
			MSN::eStatusType status = MESSENGER()->GetMyStatus();
			if (status != 5 && m_pStatusIcon[status])
				gdi->DrawTexture(m_pStatusIcon[status],
					WRect(pos.x + 1, pos.y,
						(float)m_pStatusIcon[status]->Width(),
						(float)m_pStatusIcon[status]->Height()),
					color, 0);
		}
		g_pFresh->GetManager()->PrintText(WPoint(pos.x + 30, pos.y + 6), 0,
			Doc()->m_myInfo.info.sNick, 90.0f, 0xffffffff);
	}
	else
	{
		g_pFresh->GetManager()->PrintText(WPoint(pos.x + 30, pos.y + 6), 0,
			Doc()->m_myInfo.info.sGuild, 90.0f, 0xffffffff);
	}
	gdi->SetAlpha(1.0f);
}

void FrMessengerDlg::OnMessenger_FriendListInit(int param)
{
	m_pFriendList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pFriendList)
		return;
	m_pFriendList->UseRightButton(true);
	m_pGroupIcon[0] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "down_btn_n");
	m_pGroupIcon[1] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "up_btn_n");
	m_pSexIcon[0][0] = m_pFriendList->GetBitmap("i_male");
	m_pSexIcon[0][1] = m_pFriendList->GetBitmap("i_female");
	m_pSexIcon[1][0] = m_pFriendList->GetBitmap("i_male_02");
	m_pSexIcon[1][1] = m_pFriendList->GetBitmap("i_female_02");
	m_pSexIcon[2][0] = m_pFriendList->GetBitmap("i_male_03");
	m_pSexIcon[2][1] = m_pFriendList->GetBitmap("i_female_03");
	m_pStatusIcon[0] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "play_icon");
	m_pStatusIcon[1] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "dive_icon");
	m_pStatusIcon[2] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "block_icon");
	m_pStatusIcon[3] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "busy_icon");
	m_pStatusIcon[4] = NULL;
	m_selectUID = -1;
	UpdateFriendList();
}

void FrMessengerDlg::OnMessenger_FriendListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	if (pFriend->Uid == -1)
	{
		DrawFriendGroup(param);
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	gdi->SetAlpha(m_wndAlpha2 * m_wndAlpha);
	const Bitmap* bitmap = m_pSexIcon[0][pFriend->Gender];
	WRect rect(item->pos.x + 1, item->pos.y, (float)bitmap->Width(),
		(float)bitmap->Height());
	if (pFriend->Uid == m_selectUID)
		gdi->Box(WRect(rect.x, rect.y, m_pFriendList->GetItemWidth(),
					 m_pFriendList->GetItemHeight()),
			((int)(m_wndAlpha2 * m_wndAlpha * 152.40234375f) << 24) | 0x9b9dff);
	gdi->DrawTexture(bitmap, rect,
		((int)((pFriend->IsAccept && pFriend->IsLogOn ? 255.0f : 80.0f) *
			 m_wndAlpha2 * m_wndAlpha)
			<< 24) |
			0xffffff,
		0);
	MSN::eStatusType status = GetBuddyStatus(pFriend);
	if (status != 5)
		gdi->DrawTexture(m_pStatusIcon[status],
			WRect(rect.x, rect.y + 3, (float)m_pStatusIcon[status]->Width(),
				(float)m_pStatusIcon[status]->Height()),
			FrALPHA(0xffffffff, m_wndAlpha2 * m_wndAlpha), 0);
	if (item->underCursor)
	{
		gdi->SetTextColor(0xffffffff, 0xff808080);
		gdi->SetTextStyle(2);
	}
	else
	{
		gdi->SetTextColor(0xff4c4c4c, 0xffffffff);
		gdi->SetTextStyle(0);
	}
	g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 20, item->pos.y + 7),
		0, pFriend->szAlias, -1.0f, 0xffffffff);
	if (item->underCursor)
	{
		gdi->SetTextColor(0xffffffff, 0xff808080);
		gdi->SetTextStyle(2);
	}
	else
	{
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
	}
	if (pFriend->dwGuildId)
	{
		const Bitmap* emblem = NetResourceManager::Instance()->GetEmblemByName(
			pFriend->szEmblemName);
		if (emblem)
			gdi->DrawTexture(emblem,
				WRect(item->pos.x + 70, item->pos.y + 0.3f,
					(float)emblem->Width(), (float)emblem->Height()),
				FrALPHA(0xffffffff, m_wndAlpha2 * m_wndAlpha), 0);
	}
	g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 90, item->pos.y + 7),
		0, pFriend->NickName, 90.0f, 0xffffffff);
	gdi->SetAlpha(1.0f);
	gdi->SetTextStyle(0);
}

void FrMessengerDlg::OnMessenger_FriendListLBtnDown()
{
	FindNextTopFocus(false);
	m_pRightMenu->SetVisible(false);
	m_pStateMenu->SetVisible(false);
	FrListItem* item = m_pFriendList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	if (pFriend->Uid == -1)
	{
		if (pFriend->IsLogOn)
		{
			MESSENGER()->m_groupExpansion.friendOnline =
				!MESSENGER()->m_groupExpansion.friendOnline;
			m_groupRow[0].State = MESSENGER()->m_groupExpansion.friendOnline;
		}
		else
		{
			MESSENGER()->m_groupExpansion.friendOffline =
				!MESSENGER()->m_groupExpansion.friendOffline;
			m_groupRow[1].State = MESSENGER()->m_groupExpansion.friendOffline;
		}
		UpdateFriendList();
	}
	else
	{
		m_selectUID = pFriend->Uid;
	}
}

void FrMessengerDlg::OnMessenger_FriendListRBtnDown()
{
	OnMessenger_FriendListLBtnDown();
	FrListItem* item = m_pFriendList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	WPoint pos(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
	m_pRightMenu->MoveWindow(pos);
	if (pFriend->Uid == -1)
		return;
	m_pRightMenu->SetVisible(true);
	if (pFriend->IsLogOn && pFriend->IsAccept)
	{
		m_pRightMenu->ToggleMenuItem("\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2",
			"\xb4\xeb\xc8\xad\xc7\xcf\xb1\xe2");
		if (IsLocalContent(S4_INVITE_FRIEND) ||
			IsLocalContent(S4_GOWITH_FRIEND))
		{
			if (pFriend->IsPlay || pFriend->IsBusy)
			{
				m_pRightMenu->UnableMenuItem(
					"\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
				m_pRightMenu->UnableMenuItem(
					"\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
			}
		}
	}
	else
	{
		m_pRightMenu->ToggleMenuItem("\xb4\xeb\xc8\xad\xc7\xcf\xb1\xe2",
			"\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2");
		if (!pFriend->IsAccept)
			m_pRightMenu->UnableMenuItem(
				"\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2");
		if (IsLocalContent(S4_INVITE_FRIEND) ||
			IsLocalContent(S4_GOWITH_FRIEND))
		{
			m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
			m_pRightMenu->UnableMenuItem("\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
		}
	}
	if (pFriend->IsAccept)
	{
		m_pRightMenu->UnableMenuItem("\xc4\xa3\xb1\xb8\xc7\xe3\xb0\xa1");
	}
	else
	{
		m_pRightMenu->UnableMenuItem("\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2");
		m_pRightMenu->UnableMenuItem("\xba\xb0\xb8\xed\xc0\xd4\xb7\xc2");
	}
	if (pFriend->IsAgree && !pFriend->IsAccept)
	{
		m_pRightMenu->UnableMenuItem("\xc4\xa3\xb1\xb8\xc7\xe3\xb0\xa1");
		m_pRightMenu->UnableMenuItem("\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2");
		m_pRightMenu->UnableMenuItem("\xba\xb0\xb8\xed\xc0\xd4\xb7\xc2");
	}
	if (pFriend->IsBlock)
	{
		m_pRightMenu->ToggleMenuItem("\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2",
			"\xc2\xf7\xb4\xdc\xc7\xd8\xc1\xa6");
		m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
		m_pRightMenu->UnableMenuItem("\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
	}
	else
	{
		m_pRightMenu->ToggleMenuItem("\xc2\xf7\xb4\xdc\xc7\xd8\xc1\xa6",
			"\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2");
	}
	if (IsLocalContent(S4_INVITE_FRIEND) && !m_bEnableInvite)
		m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
}

void FrMessengerDlg::OnMessenger_FriendListDBtnDown()
{
	FrListItem* item = m_pFriendList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (pFriend && pFriend->Uid != -1)
	{
		m_selectUID = pFriend->Uid;
		if (pFriend->IsLogOn && pFriend->IsAccept)
			OnChatToFriend();
		else
			OnNoteToFriend();
	}
}

void FrMessengerDlg::OnMessenger_GuildListInit(int param)
{
	m_pGuildList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (!m_pGuildList)
		return;
	m_pGuildList->UseRightButton(true);
	m_pSexIcon[0][0] = m_pGuildList->GetBitmap("i_male");
	m_pSexIcon[0][1] = m_pGuildList->GetBitmap("i_female");
	m_pSexIcon[1][0] = m_pGuildList->GetBitmap("i_male_02");
	m_pSexIcon[1][1] = m_pGuildList->GetBitmap("i_female_02");
	m_pSexIcon[2][0] = m_pGuildList->GetBitmap("i_male_03");
	m_pSexIcon[2][1] = m_pGuildList->GetBitmap("i_female_03");
	m_pStatusIcon[0] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "play_icon");
	m_pStatusIcon[1] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "dive_icon");
	m_pStatusIcon[2] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "block_icon");
	m_pStatusIcon[3] =
		g_pFresh->GetManager()->GetBitmap("MESSENGER", "busy_icon");
	m_pStatusIcon[4] = NULL;
	m_selectUID = -1;
	UpdateFriendList();
	m_pGuildList->SetVisible(false);
}

void FrMessengerDlg::OnMessenger_GuildListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) && pFriend->Uid == -1)
	{
		DrawGuildGroup(param);
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	gdi->SetAlpha(m_wndAlpha * m_wndAlpha2);
	if (!IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) && pFriend->Uid == -1)
	{
		gdi->SetTextColor(0xff4c4c4c, 0xffffffff);
		gdi->SetTextStyle(0);
		g_pFresh->GetManager()->PrintText(
			WPoint(item->pos.x + 30, item->pos.y + 10), 0,
			"\xc1\xa2\xbc\xd3\xc7\xd1 \xb1\xe6\xb5\xe5\xc4\xa3\xb1\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
			-1.0f, 0xffffffff);
		gdi->SetAlpha(1.0f);
		return;
	}
	const Bitmap* bitmap = m_pSexIcon[0][pFriend->Gender];
	WRect rect(item->pos.x + 1, item->pos.y, (float)bitmap->Width(),
		(float)bitmap->Height());
	if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
	{
		if (pFriend->Uid == m_selectUID)
			gdi->Box(WRect(rect.x, rect.y, m_pGuildList->GetItemWidth(),
						 m_pGuildList->GetItemHeight()),
				((int)(m_wndAlpha * m_wndAlpha2 * 152.40234375f) << 24) |
					0x9b9dff);
	}
	else if (pFriend->Uid == m_selectUID)
	{
		gdi->Box(WRect(rect.x, rect.y, m_pGuildList->GetItemWidth(),
					 m_pGuildList->GetItemHeight()),
			0x999b9dff);
	}
	gdi->DrawTexture(bitmap, rect,
		((int)((!IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) ||
						   (pFriend->IsAccept && pFriend->IsLogOn)
					   ? 255.0f
					   : 80.0f) *
			 m_wndAlpha * m_wndAlpha2)
			<< 24) |
			0xffffff,
		0);
	MSN::eStatusType status = GetBuddyStatus(pFriend);
	if (status != 5 && (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD) || status != 2))
		gdi->DrawTexture(m_pStatusIcon[status],
			WRect(rect.x, rect.y + 3, (float)m_pStatusIcon[status]->Width(),
				(float)m_pStatusIcon[status]->Height()),
			FrALPHA(0xffffffff, m_wndAlpha * m_wndAlpha2), 0);
	if (item->underCursor)
	{
		gdi->SetTextColor(0xffffffff, 0xff808080);
		gdi->SetTextStyle(2);
	}
	else
	{
		if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
			gdi->SetTextColor(0xff4c4c4c, 0xffffffff);
		else
			gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
	}
	g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 45, item->pos.y + 7),
		0, pFriend->NickName, 90.0f, 0xffffffff);
	gdi->SetAlpha(1.0f);
	if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
		gdi->SetTextStyle(0);
}

void FrMessengerDlg::OnMessenger_GuildListLBtnDown()
{
	FindNextTopFocus(false);
	m_pRightMenu->SetVisible(false);
	m_pStateMenu->SetVisible(false);
	FrListItem* item = m_pGuildList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	if (pFriend->Uid == -1)
	{
		if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
		{
			if (pFriend->IsLogOn)
			{
				MESSENGER()->m_groupExpansion.friendOnline =
					!MESSENGER()->m_groupExpansion.friendOnline;
				m_groupRow[0].State =
					MESSENGER()->m_groupExpansion.friendOnline;
			}
			else
			{
				MESSENGER()->m_groupExpansion.friendOffline =
					!MESSENGER()->m_groupExpansion.friendOffline;
				m_groupRow[1].State =
					MESSENGER()->m_groupExpansion.friendOffline;
			}
		}
		UpdateFriendList();
	}
	else
	{
		m_selectUID = pFriend->Uid;
	}
}

void FrMessengerDlg::OnMessenger_GuildListRBtnDown()
{
	OnMessenger_GuildListLBtnDown();
	FrListItem* item = m_pGuildList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (!pFriend)
		return;
	WPoint pos(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
	m_pRightMenu->MoveWindow(pos);
	if (pFriend->Uid == -1)
		return;
	m_pRightMenu->SetVisible(true);
	m_pRightMenu->UnableMenuItem("\xc4\xa3\xb1\xb8\xc7\xe3\xb0\xa1");
	m_pRightMenu->UnableMenuItem("\xba\xb0\xb8\xed\xc0\xd4\xb7\xc2");
	m_pRightMenu->UnableMenuItem(
		"\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb\xbb\xe8\xc1\xa6");
	m_pRightMenu->UnableMenuItem("\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2");
	if (pFriend->IsLogOn)
	{
		m_pRightMenu->ToggleMenuItem("\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2",
			"\xb4\xeb\xc8\xad\xc7\xcf\xb1\xe2");
		if (IsLocalContent(S4_INVITE_FRIEND) ||
			IsLocalContent(S4_GOWITH_FRIEND))
		{
			if (pFriend->IsPlay || pFriend->IsBusy)
			{
				m_pRightMenu->UnableMenuItem(
					"\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
				m_pRightMenu->UnableMenuItem(
					"\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
			}
		}
	}
	else
	{
		m_pRightMenu->ToggleMenuItem("\xb4\xeb\xc8\xad\xc7\xcf\xb1\xe2",
			"\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2");
		if (IsLocalContent(S4_INVITE_FRIEND) ||
			IsLocalContent(S4_GOWITH_FRIEND))
		{
			m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
			m_pRightMenu->UnableMenuItem("\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
		}
	}
	if (pFriend->IsBlock)
	{
		m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
		m_pRightMenu->UnableMenuItem("\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2");
	}
	if (IsLocalContent(S4_INVITE_FRIEND) && !m_bEnableInvite)
		m_pRightMenu->UnableMenuItem("\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2");
}

void FrMessengerDlg::OnMessenger_GuildListDBtnDown()
{
	FrListItem* item = m_pGuildList->GetItemUnderCursor();
	if (!item)
		return;
	sFriend* pFriend = (sFriend*)item->pData;
	if (pFriend && pFriend->Uid != -1)
	{
		m_selectUID = pFriend->Uid;
		if (pFriend->IsLogOn)
			OnChatToFriend();
		else
			OnNoteToFriend();
	}
}

void FrMessengerDlg::OnMessenger_RightMenuInit(int param)
{
	m_pRightMenu = (FrContextMenuCtrl*)DYNAMIC_CAST(FrWnd, (FrWnd*)param);
	if (!m_pRightMenu)
		return;
	m_pRightMenu->SetVisible(false);
	m_pRightMenu->SetFixed(true);
}

void FrMessengerDlg::OnMessenger_RightMenuLBtnDown()
{
	m_pRightMenu->SetVisible(false);
	FindNextTopFocus(false);
	const char* text = m_pRightMenu->GetSelectMenuText();
	if (!strcmp(text, "\xc4\xa3\xb1\xb8\xc7\xe3\xb0\xa1"))
	{
		OnAcceptFriend();
		return;
	}
	if (!strcmp(text, "\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb\xbb\xe8\xc1\xa6"))
	{
		OnRemoveFriend();
		return;
	}
	if (!strcmp(text, "\xc2\xf7\xb4\xdc\xc7\xcf\xb1\xe2"))
	{
		OnBlockFriend();
		return;
	}
	if (!strcmp(text, "\xc2\xf7\xb4\xdc\xc7\xd8\xc1\xa6"))
	{
		OnBlockCancelFriend();
		return;
	}
	if (!strcmp(text, "\xba\xb0\xb8\xed\xc0\xd4\xb7\xc2"))
	{
		OnAliasToFriend();
		return;
	}
	if (!strcmp(text, "\xb4\xeb\xc8\xad\xc7\xcf\xb1\xe2"))
	{
		OnChatToFriend();
		return;
	}
	if (!strcmp(text, "\xc2\xca\xc1\xf6\xba\xb8\xb3\xbb\xb1\xe2"))
	{
		OnNoteToFriend();
		return;
	}
	if (!strcmp(text, "\xc1\xa4\xba\xb8\xba\xb8\xb1\xe2"))
	{
		OnInfoFriend();
		return;
	}
	if (IsLocalContent(S4_GOWITH_FRIEND) &&
		!strcmp(text, "\xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2"))
	{
		MESSENGER()->Notify1(
			"\xc4\xa3\xb1\xb8 \xb5\xfb\xb6\xf3\xb0\xa1\xb1\xe2 \xb1\xe2\xb4\xc9\xc0\xba \xc1\xd8\xba\xf1\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		return;
	}
	if (IsLocalContent(S4_INVITE_FRIEND) &&
		!strcmp(text, "\xc3\xca\xb4\xeb\xc7\xcf\xb1\xe2"))
	{
		OnInviteFriend();
		return;
	}
	if (IsLocalContent(S4_REAL_MYROOM) &&
		IsLocalContent(S4_REAL_MYROOM_TEST_ENABLE) &&
		!strcmp(text, "\xb9\xe6\xb9\xae\xc7\xcf\xb1\xe2"))
		OnInviteFriendRealMyRoom(
			m_pTabBtn[1]->GetStatus() == FrButton::PRESSED);
}

void FrMessengerDlg::OnMessenger_StateMenuInit(int param)
{
	m_pStateMenu = (FrContextMenuCtrl*)DYNAMIC_CAST(FrWnd, (FrWnd*)param);
	if (!m_pStateMenu)
		return;
	m_pStateMenu->SetVisible(false);
	m_pStateMenu->SetFixed(true);
}

void FrMessengerDlg::OnMessenger_StateMenuLBtnDown()
{
	FindNextTopFocus(false);
	const char* text = m_pStateMenu->GetSelectMenuText();
	if (!strcmp(text, "\xbf\xc2\xb6\xf3\xc0\xce"))
		MESSENGER()->SetMyStatus((MSN::eStatusType)4);
	else if (!strcmp(text, "\xb4\xd9\xb8\xa5\xbf\xeb\xb9\xab\xc1\xdf"))
		MESSENGER()->SetMyStatus((MSN::eStatusType)3);
}

void FrMessengerDlg::OnInfoFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (pFriend && CUserInfo::Instance())
		CUserInfo::Instance()->SetInfo(pFriend->Uid, 0, true, true, true, true,
			pFriend->NickName);
}

void FrMessengerDlg::OnNoteToFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	WPoint pos(m_rect.x + 60, m_rect.y + 60);
	if (Doc()->m_myInfo.stat.i64Pang < 10)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", this);
		form->SetMessage(
			MakeStr(
				"\xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbb\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 %d\xc6\xce\xc0\xcc \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
				10),
			false);
		form->SetTimeLimit(5.0f);
		form->SetFixed(true);
		form->Open(NULL, pos, 3);
		return;
	}
	if (!m_pNoteDlg)
	{
		m_pNoteDlg =
			CreateForm<FrNoteDlg>(g_pFresh->GetManager(), this, "note", this);
		m_pNoteDlg->SetNick(pFriend->NickName, NULL);
		m_pNoteDlg->SetFixed(true);
		m_pNoteDlg->Open((FRESH_PFN_RESULT)&FrMessengerDlg::OnNoteDlgResult,
			pos, 1);
		WRect rect = m_pNoteDlg->GetRect();
		m_pNoteDlg->Adjust(rect);
		m_pNoteDlg->MoveWindow(WPoint(rect.x, rect.y));
	}
}

void FrMessengerDlg::OnChatToFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	if (pFriend->IsBlock)
	{
		MESSENGER()->Notify1(
			"\xc2\xf7\xb4\xdc\xc7\xd1 \xbb\xf3\xb4\xeb\xbf\xcd\xb4\xc2 \xb4\xeb\xc8\xad\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc2\xf7\xb4\xdc\xc0\xbb \xc7\xd8\xc1\xa6\xc7\xcf\xbd\xc3\xb0\xed \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		return;
	}
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	int count = slots.size();
	if (count < 10)
	{
		for (std::list<sChatSlot>::iterator it = slots.begin();
			it != slots.end(); ++it)
		{
			if (!(*it).pDlg && m_selectUID == (*it).uid)
				MESSENGER()->AlarmBtnUp(m_selectUID);
			if ((*it).uid == pFriend->Uid && (*it).pDlg)
			{
				(*it).pDlg->FindNextTopFocus(false);
				return;
			}
		}
		sChatSlot slot;
		slot.uid = pFriend->Uid;
		slot.pDlg = CreateForm<FrMessengerChatDlg>(g_pFresh->GetManager(), this,
			"messenger_chat");
		slot.pDlg->SetFixed(true);
		slot.pDlg->Open(NULL, 12);
		slot.pDlg->SetTopmost(true);
		slot.pDlg->SetUID(slot.uid);
		slot.pDlg->FindNextTopFocus(false);
		slots.push_back(slot);
		MESSENGER()->RepositionPopupDlg(slot.pDlg, count);
	}
	else
	{
		MESSENGER()->Notify1(
			"\xc0\xcc\xb9\xcc \xb4\xeb\xc8\xad \xbd\xbd\xb7\xb5\xc0\xcc \xb2\xcb \xc3\xa1\xbd\xc0\xb4\xcf\xb4\xd9.");
	}
}

void FrMessengerDlg::OnAcceptFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	WSendPacket packet(0x19);
	packet.Encode4(pFriend->Uid);
	packet.Send(TO_MSN);
}

void FrMessengerDlg::OnRemoveFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	WPoint pos(m_rect.x + 60.0f, m_rect.y + 60.0f);
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", this);
	form->SetMessage(
		MakeStr(
			"\\c0xffff0000\\c%s\xb4\xd4\\c0xff000000\\c\xc0\xbb \xc4\xa3\xb1\xb8 \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \\c0xffff0000\\c\xbb\xe8\xc1\xa6\\c0xff000000\\c\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			pFriend->NickName),
		false);
	form->SetFixed(true);
	form->Open((FRESH_PFN_RESULT)&FrMessengerDlg::OnRemoveFriendDlgResult, pos,
		0);
	WRect rect = form->GetRect();
	form->Adjust(rect);
	form->MoveWindow(WPoint(rect.x, rect.y));
}

void FrMessengerDlg::OnBlockFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	WPoint pos(m_rect.x + 60.0f, m_rect.y + 60.0f);
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", this);
	form->SetMessage(
		MakeStr(
			"\\c0xffff0000\\c%s\xb4\xd4\\c0xff000000\\c\xc0\xbb \xc4\xa3\xb1\xb8 \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \\c0xffff0000\\c\xc2\xf7\xb4\xdc\\c0xff000000\\c\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			pFriend->NickName),
		false);
	form->SetFixed(true);
	form->Open((FRESH_PFN_RESULT)&FrMessengerDlg::OnBlockFriendDlgResult, pos,
		0);
	WRect rect = form->GetRect();
	form->Adjust(rect);
	form->MoveWindow(WPoint(rect.x, rect.y));
}

void FrMessengerDlg::OnBlockCancelFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	WPoint pos(m_rect.x + 60.0f, m_rect.y + 60.0f);
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", this);
	form->SetMessage(
		MakeStr(
			"\\c0xffff0000\\c%s\xb4\xd4\\c0xff000000\\c\xc0\xc7 \xc2\xf7\xb4\xdc\xc0\xbb \\c0xffff0000\\c\xc7\xd8\xc1\xa6\\c0xff00000\\c\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			pFriend->NickName),
		false);
	form->SetFixed(true);
	form->Open((FRESH_PFN_RESULT)&FrMessengerDlg::OnBlockCancelFriendDlgResult,
		pos, 0);
	WRect rect = form->GetRect();
	form->Adjust(rect);
	form->MoveWindow(WPoint(rect.x, rect.y));
}

void FrMessengerDlg::OnAliasToFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (pFriend && !m_pMemoDlg)
	{
		WPoint pos(m_rect.x + 60.0f, m_rect.y + 60.0f);
		m_pMemoDlg =
			CreateForm<FrMemoDlg>(g_pFresh->GetManager(), this, "memo", this);
		m_pMemoDlg->SetFixed(true);
		m_pMemoDlg->Open((FRESH_PFN_RESULT)&FrMessengerDlg::OnMemoDlgResult,
			pos, 1);
		WRect rect = m_pMemoDlg->GetRect();
		m_pMemoDlg->Adjust(rect);
		m_pMemoDlg->MoveWindow(WPoint(rect.x, rect.y));
	}
}

void FrMessengerDlg::DrawFriendGroup(int param)
{
	FrListItem* item = (FrListItem*)param;
	sFriend* pFriend = (sFriend*)item->pData;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	gdi->SetAlpha(m_wndAlpha * m_wndAlpha2);
	if (pFriend->State == 5)
	{
		gdi->SetTextColor(0xff4c4c4c, 0xffffffff);
		gdi->SetTextStyle(0);
		char message[128] = "\0";
		switch (MESSENGER()->GetComplete())
		{
		case 0:
			wsprintf(message, "%s",
				"\xc1\xa2\xbc\xd3\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xbd");
			break;
		case 1:
			wsprintf(message, "%s",
				"\xb7\xce\xb1\xd7\xc0\xce\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 4:
			wsprintf(message, "%s", "\xc4\xa3\xb1\xb8 \xbe\xf8\xc0\xbd");
			break;
		case 3:
			wsprintf(message, "%s",
				"\xc4\xa3\xb1\xb8\xb8\xae\xbd\xba\xc6\xae \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 \xc1\xdf");
			break;
		case 2:
			wsprintf(message, "%s",
				"\xb7\xce\xb1\xd7\xc0\xce\xbf\xa1 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		}
		g_pFresh->GetManager()->PrintText(
			WPoint(item->pos.x + 40, item->pos.y + 7), 0, message, -1.0f,
			0xffffffff);
		gdi->SetAlpha(1.0f);
	}
	else
	{
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(1);
		unsigned char state;
		if (pFriend->IsLogOn)
		{
			state = m_groupRow[0].State;
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 25, item->pos.y + 7), 0,
				MakeStr("\xbf\xc2\xb6\xf3\xc0\xce(%d)", m_friendOnCount), -1.0f,
				0xffffffff);
		}
		else
		{
			state = m_groupRow[1].State;
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 25, item->pos.y + 7), 0,
				MakeStr("\xbf\xc0\xc7\xc1\xb6\xf3\xc0\xce(%d)",
					m_friendCount - m_friendOnCount),
				-1.0f, 0xffffffff);
		}
		gdi->DrawTexture(m_pGroupIcon[state],
			WRect(item->pos.x + 5, item->pos.y + 5,
				m_pGroupIcon[state]->Width(), m_pGroupIcon[state]->Height()),
			FrALPHA(0xffffffff, m_wndAlpha * m_wndAlpha2), 0);
		gdi->SetAlpha(1.0f);
	}
}

void FrMessengerDlg::DrawGuildGroup(int param)
{
	FrListItem* item = (FrListItem*)param;
	sFriend* pFriend = (sFriend*)item->pData;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	gdi->SetAlpha(m_wndAlpha * m_wndAlpha2);
	if (pFriend->State == 5)
	{
		gdi->SetTextColor(0xff4c4c4c, 0xffffffff);
		gdi->SetTextStyle(0);
		char message[128] = "\0";
		switch (MESSENGER()->GetComplete())
		{
		case 0:
			wsprintf(message, "%s",
				"\xc1\xa2\xbc\xd3\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xbd");
			break;
		case 1:
			wsprintf(message, "%s",
				"\xb7\xce\xb1\xd7\xc0\xce\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 4:
			wsprintf(message, "%s",
				"\xb1\xe6\xb5\xe5\xbf\xf8 \xbe\xf8\xc0\xbd");
			break;
		case 3:
			wsprintf(message, "%s",
				"\xb1\xe6\xb5\xe5\xb8\xae\xbd\xba\xc6\xae \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 \xc1\xdf");
			break;
		case 2:
			wsprintf(message, "%s",
				"\xb7\xce\xb1\xd7\xc0\xce\xbf\xa1 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		}
		g_pFresh->GetManager()->PrintText(
			WPoint(item->pos.x + 40, item->pos.y + 7), 0, message, -1.0f,
			0xffffffff);
		gdi->SetAlpha(1.0f);
	}
	else
	{
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(1);
		unsigned char state;
		if (pFriend->IsLogOn)
		{
			state = m_groupRow[0].State;
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 25, item->pos.y + 7), 0,
				MakeStr("\xbf\xc2\xb6\xf3\xc0\xce(%d)", m_guildOnCount), -1.0f,
				0xffffffff);
		}
		else
		{
			state = m_groupRow[1].State;
			g_pFresh->GetManager()->PrintText(
				WPoint(item->pos.x + 25, item->pos.y + 7), 0,
				MakeStr("\xbf\xc0\xc7\xc1\xb6\xf3\xc0\xce(%d)",
					m_guildCount - m_guildOnCount),
				-1.0f, 0xffffffff);
		}
		gdi->DrawTexture(m_pGroupIcon[state],
			WRect(item->pos.x + 5, item->pos.y + 5,
				m_pGroupIcon[state]->Width(), m_pGroupIcon[state]->Height()),
			FrALPHA(0xffffffff, m_wndAlpha * m_wndAlpha2), 0);
		gdi->SetAlpha(1.0f);
	}
}

void FrMessengerDlg::UpdateFriendList()
{
	if (!m_pFriendList || !m_pGuildList)
		return;
	std::map<unsigned long, sFriend>* friends = Buddy()->GetBuddyList();
	if (!friends)
		return;
	m_friendOnList.clear();
	m_friendOffList.clear();
	m_guildOnList.clear();
	m_guildOffList.clear();
	m_guildList.clear();
	m_pFriendList->ClearItem();
	m_pGuildList->ClearItem();
	std::map<unsigned long, sFriend>::iterator it;
	for (it = friends->begin(); it != friends->end(); ++it)
	{
		sFriend& pFriend = it->second;
		if (pFriend.IsBlocked)
			pFriend.IsLogOn = false;
		if (pFriend.PangyaFriend && pFriend.IsAccept && pFriend.IsLogOn)
			m_friendOnList.push_back(&pFriend);
		else if (pFriend.PangyaFriend)
		{
			if (!pFriend.IsAccept && !pFriend.IsAgree)
			{
				const char* waiting = "(\xb4\xeb\xb1\xe2\xc1\xdf)";
				strcpy(pFriend.szAlias, waiting);
			}
			if (pFriend.IsAgree && !pFriend.IsAccept)
			{
				const char* requesting = "(\xbf\xe4\xc3\xbb\xc1\xdf)";
				strcpy(pFriend.szAlias, requesting);
			}
			m_friendOffList.push_back(&pFriend);
		}
		if (pFriend.GuildFriend && pFriend.IsLogOn)
		{
			pFriend.dwGuildId = Doc()->m_myInfo.info.dwGuildId;
			strcpy(pFriend.szEmblemName, Doc()->m_myInfo.info.szEmblemName);
			m_guildList.push_back(&pFriend);
			m_guildOnList.push_back(&pFriend);
		}
		else if (pFriend.GuildFriend && !pFriend.IsLogOn)
		{
			m_guildOffList.push_back(&pFriend);
		}
	}
	std::list<sFriend*>::iterator friendIt;
	m_pFriendList->AddItem(&m_groupRow[0]);
	if (m_groupRow[0].State)
	{
		for (friendIt = m_friendOnList.begin();
			friendIt != m_friendOnList.end(); ++friendIt)
			m_pFriendList->AddItem(*friendIt);
		if (m_friendOnList.size() == 0)
			m_pFriendList->AddItem(&m_groupRow[2]);
	}
	m_pFriendList->AddItem(&m_groupRow[1]);
	if (m_groupRow[1].State)
	{
		for (friendIt = m_friendOffList.begin();
			friendIt != m_friendOffList.end(); ++friendIt)
			m_pFriendList->AddItem(*friendIt);
		if (m_friendOffList.size() == 0)
			m_pFriendList->AddItem(&m_groupRow[2]);
	}
	if (IsLocalContent(S4NT_VIEW_OFFLINE_GUILD))
	{
		m_pGuildList->AddItem(&m_groupRow[0]);
		if (m_groupRow[0].State)
		{
			for (friendIt = m_guildOnList.begin();
				friendIt != m_guildOnList.end(); ++friendIt)
				m_pGuildList->AddItem(*friendIt);
			if (m_guildOnList.size() == 0)
				m_pGuildList->AddItem(&m_groupRow[2]);
		}
		m_pGuildList->AddItem(&m_groupRow[1]);
		if (m_groupRow[1].State)
		{
			for (friendIt = m_guildOffList.begin();
				friendIt != m_guildOffList.end(); ++friendIt)
				m_pGuildList->AddItem(*friendIt);
			if (m_guildOffList.size() == 0)
				m_pGuildList->AddItem(&m_groupRow[2]);
		}
	}
	else
	{
		for (friendIt = m_guildList.begin(); friendIt != m_guildList.end();
			++friendIt)
			m_pGuildList->AddItem(*friendIt);
		if (m_guildList.size() == 0)
			m_pGuildList->AddItem(&m_groupRow[2]);
	}
	m_pFriendList->SortItem(FriendCompare);
	SetCount();
}

void FrMessengerDlg::OnMessenger_CloseLBtnDown()
{
	GetClientRect(MESSENGER()->m_dlgRect);
	Close(true);
	MESSENGER()->SetGameFocus(true);
}

void FrMessengerDlg::OnMessenger_FriendBtnInit(int param)
{
	m_pTabBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTabBtn[0])
		m_pTabBtn[0]->SetStatus(FrButton::PRESSED);
}

void FrMessengerDlg::OnMessenger_FriendLBtnDown()
{
	FindNextTopFocus(false);
	RestoreOther(0);
	m_pFriendList->SetVisible(true);
	m_pStateBtn->SetVisible(true);
	m_pGuildList->SetVisible(false);
	m_pNoticeBtn->SetVisible(false);
	m_pRightMenu->SetVisible(false);
	m_pStateMenu->SetVisible(false);
}

void FrMessengerDlg::OnMessenger_GuildBtnInit(int param)
{
	m_pTabBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!Doc()->m_myInfo.info.dwGuildId)
		m_pTabBtn[1]->Enable(false);
}

void FrMessengerDlg::OnMessenger_GuildLBtnDown()
{
	FindNextTopFocus(false);
	if (Doc()->m_myInfo.info.dwGuildId)
	{
		RestoreOther(1);
		m_pFriendList->SetVisible(false);
		m_pStateBtn->SetVisible(false);
		m_pGuildList->SetVisible(true);
		m_pRightMenu->SetVisible(false);
		m_pStateMenu->SetVisible(false);
	}
	else
	{
		RestoreOther(0);
	}
	m_pRightMenu->SetVisible(false);
	m_pStateMenu->SetVisible(false);
}

void FrMessengerDlg::OnMessenger_StateBtnInit(int param)
{
	m_pStateBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMessengerDlg::OnMessenger_StateLBtnDown()
{
	m_pStateMenu->SetVisible(true);
}

void FrMessengerDlg::OnMessenger_GuildNoticeBtnInit(int param)
{
	m_pNoticeBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNoticeBtn)
	{
		m_pNoticeBtn->Enable(false);
		m_pNoticeBtn->SetVisible(false);
	}
}

void FrMessengerDlg::OnMessenger_GuildNoticeLBtnDown()
{
	if (!MESSENGER()->m_pGuildNoticeDlg)
	{
		const WRect& ownRect = GetRect();
		WPoint pos;
		pos.x = ownRect.x + 60;
		pos.y = ownRect.y + 60;
		MESSENGER()->m_pNewsReader = new NewsReader;
		if (NetResourceManager::Instance()->IsRSSDownLoadComplete())
			MESSENGER()->m_pNewsReader->Load("guildnews.xml", 5);
		MESSENGER()->m_pGuildNoticeDlg = CreateForm<FrGuildNoticeDlg>(
			g_pFresh->GetManager(), this, "guild_notice");
		MESSENGER()->m_pGuildNoticeDlg->DataBind(MESSENGER()->m_pNewsReader, 0);
		MESSENGER()->m_pGuildNoticeDlg->SetFixed(true);
		MESSENGER()->m_pGuildNoticeDlg->Open(
			(FRESH_PFN_RESULT)&FrMessengerDlg::OnGuildNoticeDlgResult, pos, 1);
		MESSENGER()->m_pGuildNoticeDlg->SetTopmost(true);
		MESSENGER()->m_pGuildNoticeDlg->FindNextTopFocus(false);
		WRect rect = MESSENGER()->m_pGuildNoticeDlg->GetRect();
		MESSENGER()->m_pGuildNoticeDlg->Adjust(rect);
		MESSENGER()->m_pGuildNoticeDlg->MoveWindow(WPoint(rect.x, rect.y));
	}
}

void FrMessengerDlg::RestoreOther(unsigned long uid)
{
	for (unsigned long i = 0; i < 2; ++i)
	{
		if (i == uid)
			m_pTabBtn[i]->SetStatus(FrButton::PRESSED);
		else
			m_pTabBtn[i]->SetStatus(FrButton::NORMAL);
	}
}

void FrMessengerDlg::OnMessenger_FindLBtnDown()
{
	if (!m_pAddFriendDlg)
	{
		WPoint pos(m_rect.x + 60.0f, m_rect.y + 60.0f);
		m_pAddFriendDlg = CreateForm<FrAddFriendDlg>(g_pFresh->GetManager(),
			this, "addfriend", this);
		m_pAddFriendDlg->SetFixed(true);
		m_pAddFriendDlg->Open(
			(FRESH_PFN_RESULT)&FrMessengerDlg::OnAddFriendDlgResult, pos, 1);
		WRect rect = m_pAddFriendDlg->GetRect();
		m_pAddFriendDlg->Adjust(rect);
		m_pAddFriendDlg->MoveWindow(WPoint(rect.x, rect.y));
	}
}

bool FrMessengerDlg::OnNoteDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
		if (pFriend)
		{
			WSendPacket packet((enumClientPacket)60);
			packet.Encode2(0x111);
			packet.Encode4(pFriend->Uid);
			packet.EncodeStr(std::string(m_pNoteDlg->GetNote()));
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}
	}
	m_pNoteDlg = NULL;
	return true;
}

bool FrMessengerDlg::OnAddFriendDlgResult(int result, FrForm* pForm)
{
	m_pAddFriendDlg = NULL;
	return true;
}

bool FrMessengerDlg::OnOptionDlgResult(int result, FrForm* pForm)
{
	m_pOptionDlg = NULL;
	return true;
}

bool FrMessengerDlg::OnMemoDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
		if (pFriend)
		{
			WSendPacket packet(31);
			packet.Encode4(pFriend->Uid);
			packet.EncodeStr(std::string(m_pMemoDlg->GetMemo()));
			packet.Send(TO_MSN);
		}
	}
	m_pMemoDlg = NULL;
	return true;
}

bool FrMessengerDlg::OnRemoveFriendDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
		if (pFriend)
		{
			WSendPacket packet(28);
			packet.Encode4(pFriend->Uid);
			packet.EncodeStr(std::string(pFriend->NickName));
			packet.Send(TO_MSN);
		}
		m_selectUID = -1;
	}
	return true;
}

bool FrMessengerDlg::OnBlockFriendDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
		if (pFriend)
		{
			WSendPacket packet(26);
			packet.Encode4(pFriend->Uid);
			packet.Send(TO_MSN);
		}
		m_selectUID = -1;
	}
	return true;
}

bool FrMessengerDlg::OnBlockCancelFriendDlgResult(int result, FrForm* pForm)
{
	if (result == 1)
	{
		sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
		if (pFriend)
		{
			WSendPacket packet(27);
			packet.Encode4(pFriend->Uid);
			packet.Send(TO_MSN);
		}
		m_selectUID = -1;
	}
	return true;
}

bool FrMessengerDlg::OnGuildNoticeDlgResult(int result, FrForm* pForm)
{
	MESSENGER()->m_pGuildNoticeDlg = NULL;
	if (MESSENGER()->m_pNewsReader)
	{
		delete MESSENGER()->m_pNewsReader;
		MESSENGER()->m_pNewsReader = NULL;
	}
	return true;
}

MSN::eStatusType FrMessengerDlg::GetBuddyStatus(sFriend* pFriend)
{
	if (pFriend->IsBlock)
		return (MSN::eStatusType)2;
	if (pFriend->IsBusy)
		return (MSN::eStatusType)3;
	if (pFriend->IsPlay && pFriend->IsLogOn)
		return (MSN::eStatusType)0;
	if (pFriend->IsDive && pFriend->IsLogOn)
		return (MSN::eStatusType)1;
	return (MSN::eStatusType)5;
}

void FrMessengerDlg::SetCount()
{
	std::map<unsigned long, sFriend>* friends = Buddy()->GetBuddyList();
	if (!friends)
		return;
	m_friendCount = 0;
	m_friendOnCount = 0;
	m_guildCount = 0;
	m_guildOnCount = 0;
	for (std::map<unsigned long, sFriend>::iterator it = friends->begin();
		it != friends->end(); ++it)
	{
		if (it->second.PangyaFriend)
			++m_friendCount;
		if (it->second.GuildFriend)
			++m_guildCount;
		if (it->second.IsLogOn)
		{
			if (it->second.PangyaFriend && it->second.IsAccept)
				++m_friendOnCount;
			if (it->second.GuildFriend)
				++m_guildOnCount;
		}
	}
}

void FrMessengerDlg::OnGoWithFriend()
{
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (!pFriend)
		return;
	if (pFriend->userPosition.iRoomIdx == 0xffff)
	{
		char message[256] = "\0";
		wsprintf(message,
			"%s\xb4\xd4\xc0\xcc \xb0\xd4\xc0\xd3\xb9\xe6\xbf\xa1 \xb5\xe9\xbe\xee\xb0\xa1 \xc0\xd6\xc1\xf6 \xbe\xca\xbe\xc6 \xb5\xfb\xb6\xf3\xb0\xa5 \xbc\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			pFriend->NickName);
		MESSENGER()->Notify1(message);
	}
	if (pFriend->userPosition.iServerGUID == Doc()->m_gameServerUID)
	{
		WSendPacket packet((enumClientPacket)0xac);
		packet.Encode1(pFriend->userPosition.iChannelUid);
		packet.Encode2(pFriend->userPosition.iRoomIdx);
		packet.Send(TO_GAME);
	}
	else
	{
		if (g_pFresh->IsCurrentLayout("REALMYROOM"))
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 0xdf, 0, 0, 0, 0, 0);
		Doc()->m_gameServerList.clear();
		Doc()->m_channelList.clear();
		WSendPacket packet((enumClientPacket)0x43);
		packet.Send(TO_GAME);
		Doc()->SetGoWithModeStart(m_selectUID);
	}
}

void FrMessengerDlg::OnInviteFriend()
{
	if (!IsLocalContent(S4_INVITE_FRIEND))
		return;
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	if (pFriend && pFriend->IsLogOn && pFriend->IsAccept)
	{
		if (!Doc()->IsInviteMode())
		{
			Doc()->SetFriendGameSvrUID(pFriend->userPosition.iServerGUID);
			Doc()->SetInviteMode(1);
			WSendPacket packet((enumClientPacket)0xb2);
			packet.EncodeStr(pFriend->NickName);
			packet.Encode4(pFriend->Uid);
			packet.Send(TO_GAME);
		}
		else
		{
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 0x23,
				(int)"\xc3\xca\xb4\xeb \xbf\xe4\xc3\xbb\xc1\xdf\xbf\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xc0\xcc\xbf\xeb\xc7\xcf\xbc\xbc\xbf\xe4.",
				0xff000000, 0, 0, 0);
		}
	}
}

void FrMessengerDlg::OnInviteFriendRealMyRoom(bool bRealMyRoom)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return;
	sFriend* pFriend = Buddy()->GetBuddy(m_selectUID);
	bool accepted = bRealMyRoom ? true : pFriend->IsAccept;
	if (!pFriend || !accepted)
		return;
	if (g_pFresh->IsCurrentLayout("REALMYROOM") ||
		g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23,
			(int)"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb4\xc2 \xb4\xd9\xb8\xa5 \xc0\xaf\xc0\xfa\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc5\xc2 \xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
	}
	else
	{
		Doc()->m_rmrMode = 5;
		Doc()->m_replayTargetUID = pFriend->Uid;
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x222, 0, 0, 0, 0, 0));
	}
}

CMessengerInfo::CMessengerInfo()
	: m_pMessengerDlg(NULL), m_notifyTime(0), m_bNotify(false)
{
	m_dlgRect.x = 200.0f;
	m_dlgRect.y = 100.0f;
	m_dlgRect.w = 220.0f;
	m_dlgRect.h = 300.0f;
	m_alarmRect.x = 663.0f;
	m_alarmRect.y = 0.0f;
	m_alarmRect.w = 38.0f;
	m_alarmRect.h = 32.0f;
	m_myStatus = (MSN::eStatusType)4;
	m_pGuildNoticeDlg = NULL;
	m_pNewsReader = NULL;
	m_checkTime = 0;
	memset(&m_groupExpansion, true, sizeof(m_groupExpansion));
	m_popupIndex = 0;
	m_complete = (eConnectStatus)0;
	m_bAutoAway = false;
	m_chatSlotList.clear();
	m_focusList.clear();
	m_serverList.clear();
	m_bWaitServerList = false;
	m_curServerIndex = 0;
	if (IsLocalContent(S4_GOWITH_FRIEND))
	{
		Doc()->SetGoWithModeEnd();
	}
	if (IsLocalContent(S4_INVITE_FRIEND))
	{
		m_bEnableInvite = false;
	}
}

void CMessengerInfo::Open(bool bForce)
{
	if (NET()->GetSocket(WNetworkSystem::NET_MSN)->m_bConnected &&
		NET()->IsConnectionComplete(WNetworkSystem::NET_MSN))
	{
		Toggle();
		return;
	}
	Notify2(
		"\xb8\xde\xbd\xc5\xc0\xfa\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc1\xdf....");
	static DWORD connectTimes[5] = { 0 };
	static int connectCount = 0;
	DWORD now = GetTickCount();
	if (connectCount >= 5)
	{
		if (now - connectTimes[4] > 90000)
		{
			connectCount = 0;
			connectTimes[0] = 0;
		}
		else
		{
			Notify2(
				"\xc0\xe1\xbd\xc3 \xc8\xc4\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.");
			return;
		}
	}
	if ((!NET()->IsConnected(WNetworkSystem::NET_MSN) &&
			!NET()->IsConnectionComplete(WNetworkSystem::NET_MSN)) ||
		NET()->IsConnectionComplete(WNetworkSystem::NET_MSN) == 1)
	{
		NET()->Init(WNetworkSystem::NET_MSN);
		NET()->SendMessage(WNetworkSystem::NET_MSN, NetworkUnit::eEVENT_NEXT);
		if (now - connectTimes[0] < 60000)
		{
			connectTimes[connectCount++] = now;
		}
		else
		{
			int i;
			for (i = 0; i < connectCount; ++i)
			{
				if (now - connectTimes[i] < 60000)
					break;
			}
			if (i < connectCount)
			{
				int count = connectCount - i;
				for (int j = 0; j < count; ++j)
					connectTimes[j] = connectTimes[j + i];
				connectTimes[count] = now;
				connectCount = count + 1;
			}
			else
			{
				connectTimes[0] = now;
				connectCount = 1;
			}
		}
	}
}

bool CMessengerInfo::Close(bool bForce)
{
	if (m_pGuildNoticeDlg)
		m_pGuildNoticeDlg->Close(true);
	CMessengerInfo* messenger = MESSENGER();
	if (!bForce && !IsVisible())
		return true;
	std::list<sChatSlot>& slots = messenger->m_chatSlotList;
	std::list<sChatSlot>::iterator it = slots.begin();
	while (it != slots.end())
	{
		if ((*it).pDlg)
		{
			if (!(*it).pDlg->IsTopFocus())
			{
				++it;
				continue;
			}
			if (!(*it).pDlg->Close(bForce))
				return false;
		}
		if (!(*it).bAlarm || bForce)
		{
			it = slots.erase(it);
			if (!bForce)
				return false;
		}
		else
		{
			++it;
		}
	}
	if (m_pMessengerDlg)
	{
		m_pMessengerDlg->GetClientRect(m_dlgRect);
		m_pMessengerDlg->Adjust(m_dlgRect);
		m_pMessengerDlg->SetFadeout(true);
		return false;
	}
	if (bForce)
	{
		Doc()->m_bMessengerAlarm = false;
		Doc()->m_bRefreshCamera = false;
	}
	return true;
}

void CMessengerInfo::CloseWindow()
{
	SaveCurrentFocus();
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	for (std::list<sChatSlot>::iterator it = slots.begin(); it != slots.end();
		++it)
	{
		if ((*it).pDlg)
		{
			(*it).pDlg->GetClientRect((*it).rect);
			(*it).pDlg->Adjust((*it).rect);
			(*it).pDlg->Close(true);
			(*it).pDlg = NULL;
		}
	}
	if (m_pMessengerDlg)
	{
		m_pMessengerDlg->GetClientRect(m_dlgRect);
		m_pMessengerDlg->Adjust(m_dlgRect);
		m_pMessengerDlg->Close(true);
		m_pMessengerDlg = NULL;
	}
}

void CMessengerInfo::Toggle()
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	bool visible = IsVisible() ? true : false;
	bool chatOpened = false;
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		SetGameFocus(true);
	if (visible == true)
	{
		SaveCurrentFocus();
		for (std::list<sChatSlot>::iterator it = slots.begin();
			it != slots.end(); ++it)
		{
			sChatSlot& chatSlot = *it;
			if (chatSlot.pDlg)
			{
				chatSlot.pDlg->GetClientRect(chatSlot.rect);
				chatSlot.pDlg->SaveTempChat(chatSlot);
				chatSlot.pDlg->SetFadeout(true);
			}
		}
		if (m_pMessengerDlg)
		{
			m_pMessengerDlg->GetClientRect(m_dlgRect);
			m_pMessengerDlg->SetFadeout(true);
		}
	}
	else
	{
		Notify4(false);
		FrMessengerChatDlg* focus = NULL;
		if (m_focusList.size() > 0)
		{
			for (std::list<unsigned long>::iterator it = m_focusList.begin();
				it != m_focusList.end(); ++it)
			{
				unsigned long uid = *it;
				if (uid == -1)
				{
					if (!m_pMessengerDlg)
					{
						m_pMessengerDlg = CreateForm<FrMessengerDlg>(
							g_pFresh->GetManager(), this, "messenger");
						if (m_pMessengerDlg)
						{
							if (!IS_KINDOF(CGolfTask, AfxGetTask()))
								m_pMessengerDlg->Adjust(m_dlgRect);
							m_pMessengerDlg->SetFixed(true);
							m_pMessengerDlg->Open(
								(FRESH_PFN_RESULT)&CMessengerInfo::
									OnMessengerDlgResult,
								WPoint(m_dlgRect.x, m_dlgRect.y), 12);
							m_pMessengerDlg->SetTopmost(true);
							m_pMessengerDlg->SetFadeout(visible);
							if (IsLocalContent(S4_INVITE_FRIEND))
								m_pMessengerDlg->EnableInvite(m_bEnableInvite);
						}
					}
				}
				else
				{
					for (std::list<sChatSlot>::iterator slot = slots.begin();
						slot != slots.end(); ++slot)
					{
						sChatSlot& chatSlot = *slot;
						if (!chatSlot.bAlarm && !chatSlot.pDlg &&
							chatSlot.uid == uid)
						{
							chatSlot.pDlg = CreateForm<FrMessengerChatDlg>(
								g_pFresh->GetManager(), this, "messenger_chat");
							if (!IS_KINDOF(CGolfTask, AfxGetTask()))
								chatSlot.pDlg->Adjust(chatSlot.rect);
							chatSlot.pDlg->SetFixed(true);
							chatSlot.pDlg->Open(NULL,
								WPoint(chatSlot.rect.x, chatSlot.rect.y), 12);
							chatSlot.pDlg->SetUID(chatSlot.uid);
							chatSlot.pDlg->SetTopmost(true);
							chatSlot.pDlg->SetFadeout(visible);
							chatOpened = true;
							chatSlot.pDlg->RestoreChat(chatSlot);
							focus = chatSlot.pDlg;
						}
					}
				}
			}
			if (!m_pMessengerDlg)
			{
				m_pMessengerDlg = CreateForm<FrMessengerDlg>(
					g_pFresh->GetManager(), this, "messenger");
				if (m_pMessengerDlg)
				{
					if (!IS_KINDOF(CGolfTask, AfxGetTask()))
						m_pMessengerDlg->Adjust(m_dlgRect);
					m_pMessengerDlg->SetFixed(true);
					m_pMessengerDlg->Open(
						(FRESH_PFN_RESULT)&CMessengerInfo::OnMessengerDlgResult,
						WPoint(m_dlgRect.x, m_dlgRect.y), 12);
					m_pMessengerDlg->SetTopmost(true);
					m_pMessengerDlg->SetFadeout(visible);
					m_pMessengerDlg->GetClientRect(m_dlgRect);
					if (IsLocalContent(S4_INVITE_FRIEND))
						m_pMessengerDlg->EnableInvite(m_bEnableInvite);
				}
			}
			if (focus)
				focus->FindNextTopFocus(true);
		}
		else
		{
			if (!m_pMessengerDlg)
			{
				m_pMessengerDlg = CreateForm<FrMessengerDlg>(
					g_pFresh->GetManager(), this, "messenger");
				if (m_pMessengerDlg)
				{
					if (!IS_KINDOF(CGolfTask, AfxGetTask()))
						m_pMessengerDlg->Adjust(m_dlgRect);
					m_pMessengerDlg->SetFixed(true);
					m_pMessengerDlg->Open(
						(FRESH_PFN_RESULT)&CMessengerInfo::OnMessengerDlgResult,
						WPoint(m_dlgRect.x, m_dlgRect.y), 12);
					m_pMessengerDlg->SetTopmost(true);
					m_pMessengerDlg->SetFadeout(visible);
					m_pMessengerDlg->GetClientRect(m_dlgRect);
					Refresh();
					if (IsLocalContent(S4_INVITE_FRIEND))
						m_pMessengerDlg->EnableInvite(m_bEnableInvite);
				}
			}
		}
		m_focusList.clear();
	}
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (!CChatMsg::Instance()->IsOpened())
			g_pFresh->GetManager()->SetExclusiveKey(!visible && chatOpened);
	}
	g_audio->PlaySfx("ui_icon_boing2");
}

void CMessengerInfo::SaveCurrentFocus()
{
	std::string name;
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	std::list<FrWnd*>& windows = g_pFresh->GetManager()->GetTopWndList();
	for (std::list<FrWnd*>::iterator it = windows.begin(); it != windows.end();
		++it)
	{
		std::list<sChatSlot>::iterator slot;
		FrWnd* form = *it;
		if (IS_KINDOF(FrForm, form))
		{
			form->GetWindowName(name);
			if (!strcmp(name.c_str(), "messenger"))
				m_focusList.push_back(-1);
			else if (!strcmp(name.c_str(), "messenger_chat"))
			{
				for (slot = slots.begin(); slot != slots.end(); ++slot)
					if ((*slot).pDlg == form)
						m_focusList.push_back((*slot).uid);
			}
		}
	}
}

void CMessengerInfo::ApplySavedFocus()
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	for (std::list<unsigned long>::iterator it = m_focusList.begin();
		it != m_focusList.end(); ++it)
	{
		unsigned long uid = *it;
		if (uid == -1)
		{
			if (m_pMessengerDlg)
				m_pMessengerDlg->FindNextTopFocus(false);
		}
		else
		{
			for (std::list<sChatSlot>::iterator slot = slots.begin();
				slot != slots.end(); ++slot)
				if ((*slot).uid == uid && (*slot).pDlg)
					(*slot).pDlg->FindNextTopFocus(false);
		}
	}
}

void CMessengerInfo::Process(float delta)
{
	if (MESSENGER()->GetComplete() != 4)
	{
		if (NET()->IsConnected(WNetworkSystem::NET_MSN) &&
			NET()->IsConnectionComplete(WNetworkSystem::NET_MSN) &&
			NET()->IsConnected(WNetworkSystem::NET_GAME))
		{
			m_checkTime += delta;
			if (m_checkTime > 60)
			{
				m_checkTime = 0;
				NET()->ForceShutDown(WNetworkSystem::NET_MSN);
				Close(true);
				NET()->Init(WNetworkSystem::NET_MSN);
				NET()->SendMessage(WNetworkSystem::NET_MSN,
					NetworkUnit::eEVENT_NEXT);
			}
		}
		else if (!NET()->IsConnected(WNetworkSystem::NET_MSN))
			m_checkTime = 0;
	}
	if (NET()->IsConnected(WNetworkSystem::NET_MSN) &&
		NET()->IsConnected(WNetworkSystem::NET_GAME) &&
		MESSENGER()->GetMyStatus() != 3)
	{
		bool changed = false;
		if (m_bAutoAway)
		{
			if (g_mouse->IsUpdated() || g_ime->IsUpdated())
			{
				m_bAutoAway = false;
				changed = true;
			}
		}
		else if (timeGetTime() - g_mouse->GetLastInputTime() > 120000 &&
			timeGetTime() - g_ime->GetLastInputTime() > 120000)
		{
			m_bAutoAway = true;
			changed = true;
		}
		if (changed)
		{
			if (m_bAutoAway)
				MESSENGER()->SetMyStatus((MSN::eStatusType)1);
			else
				MESSENGER()->SetMyStatus((MSN::eStatusType)4);
		}
	}
	if (IS_KINDOF(CGolfTask, AfxGetTask()) &&
		NET()->IsConnected(WNetworkSystem::NET_MSN))
	{
		if (g_input->GetButton(LEFT_BUTTON) == 1 &&
			!CChatMsg::Instance()->IsOpened())
		{
			g_pFresh->GetManager()->SetExclusiveKey(false);
			SetGameFocus(true);
		}
		if (g_input->GetButton(RIGHT_BUTTON) == 1 ||
			g_input->GetButton(LEFT_BUTTON) == 1)
		{
			const WPoint& pos = g_pFresh->GetManager()->GetMousePos();
			bool outside = true;
			WRect rect;
			std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
			for (std::list<sChatSlot>::iterator it = slots.begin();
				it != slots.end(); ++it)
			{
				if ((*it).pDlg)
				{
					(*it).pDlg->GetClientRect(rect);
					if (rect.IsInRect(pos))
						outside = false;
					if ((*it).pDlg->GetEmoticonClientRect(rect) &&
						rect.IsInRect(pos))
						outside = false;
				}
			}
			if (m_pMessengerDlg)
			{
				m_pMessengerDlg->GetClientRect(rect);
				if (rect.IsInRect(pos))
					outside = false;
			}
			if (outside)
				SetGameFocus(true);
			else
				SetGameFocus(false);
		}
	}
	if (m_bNotify)
	{
		m_notifyTime += delta;
		if (m_notifyTime > 4)
		{
			if (IS_KINDOF(CLobbyTask, AfxGetTask()))
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(CShopTask, AfxGetTask()))
				AfxGetTask()->GetActor("Shop")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(CFamilyTask, AfxGetTask()))
				AfxGetTask()->GetActor("Family")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(CGolfTask, AfxGetTask()))
				AfxGetTask()->GetActor("GolfRule")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
				AfxGetTask()->GetActor("AvatarChat")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 0xa8, 0, 0, 0, 0, 0);
			m_bNotify = false;
			m_notifyTime = 0;
		}
	}
}

bool CMessengerInfo::OnMessengerDlgResult(int result, FrForm* pForm)
{
	m_pMessengerDlg = NULL;
	return true;
}

bool CMessengerInfo::OnMessengerChatDlgResult(int result, FrForm* pForm)
{
	std::list<sChatSlot>::iterator it = m_chatSlotList.begin();
	while (it != m_chatSlotList.end())
	{
		if ((*it).pDlg == pForm)
			(*it).pDlg = NULL;
	}
	return true;
}

void CMessengerInfo::Notify1(const char* msg)
{
	FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
	if (form)
	{
		form->SetMessage(msg, false);
		form->Open(NULL, 3);
		form->SetTopmost(true);
		form->FindNextTopFocus(false);
	}
}

void CMessengerInfo::Notify2(const char* msg)
{
	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(CShopTask, AfxGetTask()))
		AfxGetTask()->GetActor("Shop")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(CFamilyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Family")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 168, (int)msg, 1, 0, 0, 0);
	m_notifyTime = 0;
	m_bNotify = true;
}

void CMessengerInfo::Notify3()
{
	Doc()->m_bMessengerAlarm = true;
	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	else if (IS_KINDOF(CShopTask, AfxGetTask()))
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	else if (IS_KINDOF(CFamilyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Family") << MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	else if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 169, 1, 0, 0, 0, 0);
	g_audio->PlaySfx("notice_dingdong", NULL, NULL, NULL, 0, 0.5f, 200.0f);
}

void CMessengerInfo::Notify4(bool bAlarm)
{
	Doc()->m_bMessengerInit = bAlarm;
	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
	else if (IS_KINDOF(CShopTask, AfxGetTask()))
		AfxGetTask()->GetActor("Shop")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
	else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
	else if (IS_KINDOF(CFamilyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Family")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
	else if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 172, bAlarm, 0, 0, 0, 0);
}

void CMessengerInfo::AlarmBtnUp(unsigned long uid)
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	int count = slots.size();
	for (std::list<sChatSlot>::iterator it = slots.begin(); it != slots.end();
		++it)
	{
		sChatSlot& slot = *it;
		if (!slot.pDlg && slot.bAlarm && (uid == -1 || uid == slot.uid))
		{
			slot.bAlarm = false;
			slot.pDlg = CreateForm<FrMessengerChatDlg>(g_pFresh->GetManager(),
				this, "messenger_chat");
			slot.pDlg->SetFixed(true);
			slot.pDlg->Open(NULL, 12);
			slot.pDlg->SetUID(slot.uid);
			slot.pDlg->SetTopmost(true);
			slot.pDlg->FindNextTopFocus(false);
			slot.pDlg->RestoreChat(slot);
			RepositionPopupDlg(slot.pDlg, count);
		}
	}
	Doc()->m_bMessengerAlarm = false;
	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
	else if (IS_KINDOF(CShopTask, AfxGetTask()))
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
	else if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
	else if (IS_KINDOF(CFamilyTask, AfxGetTask()))
		AfxGetTask()->GetActor("Family")
			<< MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
	else if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 0xa9, 0, 0, 0, 0, 0);
}

void CMessengerInfo::RecvChat(sFriend* pFriend, std::string msg)
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	int count = slots.size();
	if (count < 10)
	{
		bool found = false;
		sMsnChat chat;
		std::list<sChatSlot>::iterator it;
		for (it = slots.begin(); it != slots.end(); ++it)
		{
			if ((*it).uid == pFriend->Uid)
			{
				chat.nick = pFriend->NickName;
				chat.msg = msg;
				(*it).chatList.push_back(chat);
				if ((*it).pDlg)
				{
					(*it).pDlg->AddChatLine(chat.nick, chat.msg);
					if (!(*it).pDlg->IsTopFocus())
						(*it).pDlg->Vibration(1.0f);
				}
				if (!(*it).bAlarm && !(*it).pDlg)
					Notify4(true);
				found = true;
				break;
			}
		}
		if (!found)
		{
			sChatSlot slot;
			sMsnChat newChat;
			slot.bAlarm = true;
			slot.uid = pFriend->Uid;
			newChat.nick = pFriend->NickName;
			newChat.msg = msg;
			slot.chatList.push_back(newChat);
			slots.push_back(slot);
			Notify3();
		}
	}
	else
	{
		WSendPacket packet(0x1e);
		packet.Encode4(pFriend->Uid);
		packet.EncodeStr(std::string(
			"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xc0\xb4\xe4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
		packet.Send(TO_MSN);
	}
}

void CMessengerInfo::SetOfflineUserNick(const char* nick, unsigned long uid)
{
	if (m_pMessengerDlg)
		m_pMessengerDlg->SetAddFriendNick(nick, uid);
}

void CMessengerInfo::SetTargetNick(const char* nick)
{
	if (m_pMessengerDlg)
		m_pMessengerDlg->SetTargetNick(nick);
}

void CMessengerInfo::Refresh()
{
	if (m_pMessengerDlg)
		m_pMessengerDlg->UpdateFriendList();
}

void CMessengerInfo::SetMyStatus(MSN::eStatusType status)
{
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN) &&
		MESSENGER()->GetComplete() == 4)
	{
		m_myStatus = status;
		WSendPacket packet(0x1d);
		packet.Encode1(m_myStatus);
		packet.Send(TO_MSN);
	}
}

bool CMessengerInfo::IsVisible()
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	for (std::list<sChatSlot>::iterator it = slots.begin(); it != slots.end();
		++it)
		if ((*it).pDlg)
			return true;
	if (m_pMessengerDlg)
		return true;
	return false;
}

void CMessengerInfo::RepositionPopupDlg(FrMessengerChatDlg* pDlg, int index)
{
	if (!pDlg)
		return;
	if (index == 0)
	{
		m_popupIndex = 1;
		return;
	}
	if (index > 0)
	{
		WRect rect = pDlg->GetRect();
		if (g_view->GetWidth() - rect.w <= 0)
			return;
		if (g_view->GetHeight() - rect.h <= 0)
			return;
		WPoint pos(rect.x +
				(g_view->GetWidth() - rect.w) * (m_popupIndex * 20) /
					(g_view->GetHeight() - rect.h),
			rect.y + m_popupIndex * 20);
		if ((int)(g_view->GetWidth() - rect.w) > 0)
			pos.x = (int)pos.x % (int)(g_view->GetWidth() - rect.w);
		if ((int)(g_view->GetHeight() - rect.h) > 0)
			pos.y = (int)pos.y % (int)(g_view->GetHeight() - rect.h);
		pDlg->MoveWindow(pos);
		++m_popupIndex;
	}
}

void CMessengerInfo::SetGameFocus(bool bFocus)
{
	m_bGameFocus = bFocus;
	if (bFocus && IS_KINDOF(CGolfTask, AfxGetTask()))
		g_pFresh->GetManager()->ResetTopFocus();
}

void CMessengerInfo::InvalidateCurrentServer()
{
	int index = 0;
	std::list<MSNServerInfo>::iterator it;
	for (it = m_serverList.begin(); it != m_serverList.end(); it++, ++index)
	{
		MSNServerInfo& server = *it;
		if (index == m_curServerIndex)
		{
			server.bInvalid = true;
			break;
		}
	}
}
