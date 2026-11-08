#include "frienddlg.h"
#include "giftdlg.h"
#include "post_senddlg.h"
#include "buddymanager.h"
#include "user_info.h"
#include "frlistbox.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "binstr.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrFriendDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFriendDlg, FrForm)

ON_FRESH_VI("friend_listbox", FRCMD_INIT, FrFriendDlg::OnFriendListInit)
ON_FRESH_VI("friend_listbox", FRCMD_OWNERDRAW,
	FrFriendDlg::OnFriendListOwnerDraw)
ON_FRESH_VV("friend_listbox", FRCMD_LBUTTONUP, FrFriendDlg::OnFriendListLBtnUp)
ON_FRESH_VV("friend_listbox", FRCMD_RBUTTONUP, FrFriendDlg::OnFriendListRBtnUp)
ON_FRESH_VV("friend_listbox", FRCMD_DBLCLICK, FrFriendDlg::OnFriendListDblClick)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrFriendDlg::OnCancelBtnUp)

END_FRESH_MSGMAP()

FrFriendDlg::FrFriendDlg()
{
	m_pFriendList = NULL;
	m_pGiftDlg = NULL;
	m_uid = (unsigned long)-1;
}

void FrFriendDlg::SetParent(FrGiftDlg* pParent)
{
	m_pGiftDlg = pParent;
}

void FrFriendDlg::SetParent2(CPost_SendDlg* pParent)
{
	m_pPostSendDlg = pParent;
}

bool FrFriendDlg::IsSelectPerson()
{
	if (m_uid == (unsigned long)-1)
		return false;
	return true;
}

bool FrFriendDlg::HaveFriends()
{
	if (m_pFriendList->GetCurrentItemSize(false) > 0)
		return true;
	return false;
}

void FrFriendDlg::UpdateFriendList()
{
	if (m_pFriendList == NULL)
		return;

	std::map<unsigned long, sFriend>* pList = Buddy()->GetBuddyList();

	m_pFriendList->ClearItem();

	for (std::map<unsigned long, sFriend>::iterator it = pList->begin();
		it != pList->end(); ++it)
	{
		sFriend* pFriend = &(*it).second;

		if (pFriend->IsAccept || pFriend->IsAgree)
			m_pFriendList->AddItem(pFriend);
	}
}

void FrFriendDlg::OnFriendListInit(int param)
{
	m_pFriendList = DYNAMIC_CAST(FrListBox, param);

	if (m_pFriendList == NULL)
		return;

	m_pSexIcon[0] = m_pFriendList->GetBitmap("i_male");
	m_pSexIcon[1] = m_pFriendList->GetBitmap("i_female");

	m_pFriendList->UseRightButton(true);

	UpdateFriendList();
}

void FrFriendDlg::OnFriendListOwnerDraw(int param)
{
	if (param == 0)
		return;
	FrListItem* pItem = (FrListItem*)param;
	sFriend* pFriend = (sFriend*)pItem->pData;
	if (pFriend == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pIcon = m_pSexIcon[pFriend->Gender];
	WRect rect(pItem->pos.x - 8.0f, pItem->pos.y, (float)m_pSexIcon[0]->Width(),
		(float)pIcon->Height());

	if (pFriend->Uid == m_uid)
		pGDI->Box(WRect(rect.x, rect.y, m_pFriendList->GetItemWidth() + 8.0f,
					  m_pFriendList->GetItemHeight()),
			0x999b9dff);

	pGDI->DrawTexture(pIcon, rect,
		((pFriend->IsAccept && pFriend->IsLogOn) ? 255 : 80) << 24 | 0xffffff,
		0);

	if (pItem->underCursor)
	{
		pGDI->SetTextColor(0xffffffff, 0xff808080);
		pGDI->SetTextStyle(2);
	}
	else
	{
		pGDI->SetTextColor(0xff4c4c4c, 0xffffffff);
		pGDI->SetTextStyle(0);
	}

	char szNick[22] = {
		0,
	};
	BOOL bPrinted = FALSE;
	if (strcmp(pFriend->szAlias, "Friend") == 0)
	{
		if (!pItem->underCursor)
		{
			pGDI->SetTextColor(0xff000000, 0xffffffff);
			pGDI->SetTextStyle(0);
		}

		if (strlen(pFriend->NickName) > 18)
		{
			mbscut(szNick, pFriend->NickName, 18);
			strcat(szNick, "..");
			g_pFresh->GetManager()->PrintText(
				WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 0, szNick,
				-1.0f, 0xffffffff);
		}
		else
		{
			g_pFresh->GetManager()->PrintText(
				WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 0,
				pFriend->NickName, -1.0f, 0xffffffff);
		}

		bPrinted = TRUE;
	}
	else
	{
		g_pFresh->GetManager()->PrintText(
			WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 0,
			pFriend->szAlias, -1.0f, 0xffffffff);
	}

	if (!pItem->underCursor)
	{
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		pGDI->SetTextStyle(0);
	}

	if (!bPrinted)
	{
		if (strlen(pFriend->NickName) > 9)
		{
			mbscut(szNick, pFriend->NickName, 9);
			strcat(szNick, "..");
			g_pFresh->GetManager()->PrintText(
				WPoint(pItem->pos.x + 82.0f, pItem->pos.y + 7.0f), 0, szNick,
				-1.0f, 0xffffffff);
		}
		else
		{
			g_pFresh->GetManager()->PrintText(
				WPoint(pItem->pos.x + 82.0f, pItem->pos.y + 7.0f), 0,
				pFriend->NickName, -1.0f, 0xffffffff);
		}
	}
}

void FrFriendDlg::OnFriendListLBtnUp()
{
	FrListItem* pItem = m_pFriendList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	sFriend* pFriend = (sFriend*)pItem->pData;
	if (pFriend == NULL)
		return;

	if (pFriend->Uid == (unsigned long)-1)
		return;

	if (m_uid == (unsigned long)-1 && m_pGiftDlg)
	{
		if (m_pGiftDlg->m_pEdNick->GetLine(1, false))
			m_prevNick = m_pGiftDlg->m_pEdNick->GetLine(1, false);

		if (m_pGiftDlg->m_pEdConfirm->GetLine(1, false))
			m_prevConfirmNick = m_pGiftDlg->m_pEdConfirm->GetLine(1, false);
	}
	else if (m_uid == (unsigned long)-1 && m_pPostSendDlg)
	{
		if (m_pPostSendDlg->m_pReciveUser->GetLine(1, false))
			m_prevConfirmNick =
				m_pPostSendDlg->m_pReciveUser->GetLine(1, false);
	}

	m_uid = pFriend->Uid;
	if (m_pGiftDlg)
	{
		m_pGiftDlg->m_pEdNick->SetLine(1, pFriend->NickName, 0, false, 0);
		m_pGiftDlg->m_pEdConfirm->SetLine(1, pFriend->NickName, 0, false, 0);
	}
	else if (m_pPostSendDlg)
	{
		m_pPostSendDlg->m_pReciveUser->SetLine(1, pFriend->NickName, 0, false,
			0);
		m_pPostSendDlg->m_pUserNick->SetLine(1, pFriend->NickName, 0, false, 0);
	}
}

void FrFriendDlg::OnFriendListRBtnUp()
{
	if (CUserInfo::Instance()->IsVisible())
		return;
	if (!CUserInfo::IsInstantiated())
		return;
	OnFriendListLBtnUp();

	if (m_uid == (unsigned long)-1)
		return;
	sFriend* pFriend = Buddy()->GetBuddy(m_uid);
	if (pFriend == NULL)
		return;

	CUserInfo::Instance()->SetInfo(m_uid, 0, true, true, true, true,
		std::string(pFriend->NickName));
}

void FrFriendDlg::OnFriendListDblClick()
{
	if (m_uid == (unsigned long)-1)
		return;

	OnOK();
}

void FrFriendDlg::OnCancelBtnUp()
{
	if (m_uid != (unsigned long)-1 && m_pGiftDlg)
	{
		m_pGiftDlg->m_pEdNick->SetLine(1, m_prevNick.c_str(), 0, false, 0);
		m_pGiftDlg->m_pEdConfirm->SetLine(1, m_prevConfirmNick.c_str(), 0,
			false, 0);
	}

	OnCancel();
}
