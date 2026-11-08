#include "avataruserlistdlg.h"
#include "frlistbox.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "mousecursor.h"
#include "netresourcemanager.h"
#include "projectg.h"
#include "../../shared/localize.h"
extern Fresh* g_pFresh;
IMPLEMENT_OBJECT(FrAvatarUserListDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrAvatarUserListDlg, FrForm)

ON_FRESH_VI("userlist", FRCMD_INIT, FrAvatarUserListDlg::OnUserListInit)
ON_FRESH_VI("userlist", FRCMD_OWNERDRAW,
	FrAvatarUserListDlg::OnUserListOwnerDraw)
ON_FRESH_VV("userlist", FRCMD_LBUTTONDOWN,
	FrAvatarUserListDlg::OnUserListLBtnDown)
ON_FRESH_VV("userlist", FRCMD_RBUTTONUP, FrAvatarUserListDlg::OnUserListRBtnUp)

END_FRESH_MSGMAP()

FrAvatarUserListDlg::FrAvatarUserListDlg()
{
	m_pUserList = NULL;
	m_alpha = 0.0f;
	m_selectedUid = 0xffffffff;
}

FrAvatarUserListDlg::~FrAvatarUserListDlg()
{
}

void FrAvatarUserListDlg::OnUserListInit(int param)
{
	m_pUserList = (FrListBox*)param;
	m_pUserList->UseRightButton(true);

	m_pIcon[0] = m_pUserList->GetBitmap("i_male");
	m_pIcon[1] = m_pUserList->GetBitmap("i_female");
	m_pIcon[2] = m_pUserList->GetBitmap("i_male_02");
	m_pIcon[3] = m_pUserList->GetBitmap("i_female_02");
	m_pIcon[4] = m_pUserList->GetBitmap("i_male_03");
	m_pIcon[5] = m_pUserList->GetBitmap("i_female_03");

	m_pMannerIcon[0] = m_pUserList->GetBitmap("i_male_manner");
	m_pMannerIcon[1] = m_pUserList->GetBitmap("i_female_manner");

	m_pAngelIcon[0] = m_pUserList->GetBitmap("i_male_angel");
	m_pAngelIcon[1] = m_pUserList->GetBitmap("i_female_angel");

	m_pInGameIcon = m_pUserList->GetBitmap("ingame");
}

void FrAvatarUserListDlg::OnUserListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (!pSlot)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	const Bitmap* pIcon;

	if (pSlot->angelicWings)
		pIcon = m_pAngelIcon[pSlot->gender % 2];
	else if (pSlot->manner)
		pIcon = m_pMannerIcon[pSlot->gender % 2];
	else
		pIcon = m_pIcon[pSlot->gender];

	WRect dest(pItem->pos.x, pItem->pos.y, (float)pIcon->Width(),
		(float)pIcon->Height());

	if (pSlot->dwGuid == m_selectedUid)
		pDevice->Box(WRect(dest.x, dest.y, (float)m_pUserList->GetItemWidth(),
						 (float)m_pUserList->GetItemHeight()),
			((unsigned char)(m_alpha * 255.0f) << 24) | 0x9b9bff, 0, 0.0f);

	pDevice->DrawTexture(pIcon, dest, 0xffffffff, 0);

	if (CProjectG::Instance()->HidePrivacy())
		return;

	if (pSlot->dwTitle)
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->dwTitle);
		if (pSkin)
			pIcon = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->c.Icon);
		else
			pIcon = g_pFresh->GetManager()->GetBitmap("LEVELS",
				MakeStr("level_%03d", pSlot->level + 1));
	}
	else
	{
		pIcon = g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("level_%03d", pSlot->level + 1));
	}

	if (pIcon)
	{
		pDevice->DrawTexture(pIcon,
			WRect(pItem->pos.x + 50.0f - float2int(pIcon->Width() * 0.5f),
				pItem->pos.y + 13.0f - float2int(pIcon->Height() * 0.5f),
				(float)pIcon->Width(), (float)pIcon->Height()),
			0xffffffff, 0);
	}

	if (pSlot->GuildId && !pSlot->IsIdentity(0x14))
	{
		const Bitmap* pEmblem = NetResourceManager::Instance()->GetEmblemByName(
			pSlot->szEmblemName);

		if (pEmblem)
		{
			pDevice->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 85.0f, pItem->pos.y + 7.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xffffffff, 0);
		}
	}

	unsigned long color =
		(unsigned char)(pSlot->dwIdentity & 0x14) ? 0x80ffffff : 0xffffffff;

	if (pItem->underCursor)
	{
		pDevice->SetTextColor(0xffffffff, 0xff808080);
		pDevice->SetTextStyle(2);
	}
	else
	{
		pDevice->SetTextColor(0xff000000, 0xffffffff);
		pDevice->SetTextStyle(0);
	}

	g_pFresh->GetManager()->PrintText(
		WPoint(pItem->pos.x + 112.0f, pItem->pos.y + 7.0f), 0, pSlot->sNick,
		-1.0f, color);
}

void FrAvatarUserListDlg::OnUserListLBtnDown()
{
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
}

void FrAvatarUserListDlg::OnUserListRBtnUp()
{
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	CSharedDoc* pDoc = Doc();
}

void FrAvatarUserListDlg::UpdateUserList()
{
	m_pUserList->ClearItem();

	for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
		it != Doc()->m_slotList.end(); ++it)
	{
		if (IsLocalContent(S3_GM_TOOLKIT))
		{
			std::map<unsigned long, sBriefUserInfo>::iterator itBrief =
				Doc()->m_briefUserInfoMap.find(sSlotInfo(*it).dwGuid);

			if (!(bool)(Doc()->m_myInfo.info.dwIdentity & 4))
			{
				if ((*it).dwIdentity & 4)
				{
					if (!((*itBrief).second.state & 1))
						continue;
				}
			}
		}
		else if (!(bool)(Doc()->m_myInfo.info.dwIdentity & 4) &&
			((*it).dwIdentity & 0x14))
		{
			continue;
		}

		m_pUserList->AddItem(&(*it));
	}
}

void FrAvatarUserListDlg::OnProc(const float delta)
{
	if (CMouseCursor::Instance()->InArea(m_rect))
	{
		m_alpha += delta * 2.0f;
		if (m_alpha > 1.0f)
			m_alpha = 1.0f;
	}
	else
	{
		m_alpha -= delta * 2.0f;
		if (m_alpha < 0.6f)
			m_alpha = 0.6f;
	}

	SetAlpha2ToChild(m_alpha);
}
