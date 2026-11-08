#include "mapselectdlg.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "actor.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

static const unsigned char s_mapOrder[18] = { 0x13, 0x10, 0x0f, 0x0e, 0x0d,
	0x0b, 0x08, 0x0a, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09,
	0x7f };

IMPLEMENT_OBJECT(FrMapSelectDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMapSelectDlg, FrForm)

ON_FRESH_VI("select", FRCMD_INIT, FrMapSelectDlg::OnSelectInit)
ON_FRESH_VV("select", FRCMD_LBUTTONUP, FrMapSelectDlg::OnSelectBtnUp)
ON_FRESH_VI("select", FRCMD_OWNERDRAW, FrMapSelectDlg::OnSelectOwnerDraw)

END_FRESH_MSGMAP()

FrMapSelectDlg::FrMapSelectDlg()
	: m_pMapList(NULL), m_selectMap(0), m_bNoRandomMap(false)
{
	m_bSendChange = true;
}

FrMapSelectDlg::~FrMapSelectDlg()
{
}

void FrMapSelectDlg::OnSelectInit(int param)
{
	m_pMapList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pMapList)
	{
		std::map<unsigned int, IFF_STRUCT::sCourse>& courseMap =
			ItemManager()->m_CourseMap;
		int count = 0;
		FrListItem* pSelected = NULL;

		if (IsLocalContent(S3_ROOKIE_CHANNEL) &&
			(Doc()->m_curChannel.Type & 0x800))
		{
			for (int i = 0; i < 18; i++)
			{
				unsigned int tid = s_mapOrder[i] | 0x28000000;
				IFF_STRUCT::sCourse* pCourse = &courseMap[tid];

				if ((Doc()->m_curChannel.Type & 0x800) &&
					pCourse->Difficulty == 3)
					continue;

				if (pCourse && (Doc()->m_curChannel.Type & 0x800) &&
					!Doc()->CanUseRookieChannelMap(
						pCourse->c.TypeId & 0x3ffffff))
					continue;

				FrListItem* pItem = m_pMapList->AddItem(pCourse);
				count++;

				if (pSelected == NULL &&
					m_selectMap == (pCourse->c.TypeId & 0x3ffffff))
					pSelected = pItem;
			}

			for (int i = 0; i < 18; i++)
			{
				unsigned int tid = s_mapOrder[i] | 0x28000000;
				IFF_STRUCT::sCourse* pCourse = &courseMap[tid];

				if (pCourse && (Doc()->m_curChannel.Type & 0x800) &&
					Doc()->CanUseRookieChannelMap(
						pCourse->c.TypeId & 0x3ffffff) &&
					pCourse->Difficulty != 3)
					continue;

				FrListItem* pItem = m_pMapList->AddItem(pCourse);
				count++;

				if (pSelected == NULL &&
					m_selectMap == (pCourse->c.TypeId & 0x3ffffff))
					pSelected = pItem;
			}
		}
		else
		{
			for (int i = 0; i < 18; i++)
			{
				unsigned int tid = s_mapOrder[i] | 0x28000000;
				IFF_STRUCT::sCourse* pCourse = &courseMap[tid];

				if (!pCourse->c.Final)
					continue;

				if ((Doc()->m_curChannel.Type & 0x80) &&
					pCourse->Difficulty == 1)
					continue;

				FrListItem* pItem = m_pMapList->AddItem(pCourse);
				count++;

				if (pSelected == NULL &&
					m_selectMap == (pCourse->c.TypeId & 0x3ffffff))
					pSelected = pItem;
			}
		}

		int empty;
		if (count % 3 == 0)
			empty = 0;
		else
			empty = 3 - count % 3;
		for (int i = 0; i < empty; i++)
			m_pMapList->AddItem(NULL);

		m_pMapList->SelectItem(pSelected, true);
	}
}

void FrMapSelectDlg::SetSelectMap(int map)
{
	if (m_pMapList == NULL)
		return;

	for (FrListBox::ITEM_LIST::iterator it = m_pMapList->m_itemList.begin();
		it != m_pMapList->m_itemList.end(); ++it)
	{
		IFF_STRUCT::sCourse* pCourse = (IFF_STRUCT::sCourse*)(*it)->pData;

		if (pCourse && map == (pCourse->c.TypeId & 0x3ffffff))
		{
			m_selectMap = map;
			m_pMapList->SelectItem(*it, true);
			break;
		}
	}
}

void FrMapSelectDlg::OnSelectBtnUp()
{
	FrListItem* pItem = m_pMapList->GetSelected();
	if (pItem == NULL)
		return;

	IFF_STRUCT::sCourse* pCourse = (IFF_STRUCT::sCourse*)pItem->pData;
	if (pCourse == NULL)
		return;

	if (((Doc()->m_curChannel.Type & 0x800) && pCourse->Difficulty >= 3) ||
		(m_bNoRandomMap && (pCourse->c.TypeId & 0x3ffffff) == 0x7f))
	{
		m_pMapList->ToggleItem(pItem);
		return;
	}

	if (IsLocalContent(S3_ROOKIE_CHANNEL) &&
		(Doc()->m_curChannel.Type & 0x800) &&
		!Doc()->CanUseRookieChannelMap(pCourse->c.TypeId & 0x3ffffff))
	{
		m_pMapList->ToggleItem(pItem);
		return;
	}

	if (m_bSendChange &&
		(stricmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0 ||
			stricmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") ==
				0 ||
			stricmp(g_pFresh->GetManager()->GetLayoutID(), "AVATARCHAT_MAIN") ==
				0))
	{
		WSendPacket packet((enumClientPacket)10);
		packet.Encode2(0xffff);
		packet.Encode1(1);
		packet.Encode1(3);
		packet.Encode1((unsigned char)pCourse->c.TypeId);
		packet.Send(TO_GAME);

		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
	else
	{
		m_selectMap = pCourse->c.TypeId & 0x3ffffff;
	}

	Close(true);
}

void FrMapSelectDlg::OnSelectOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	IFF_STRUCT::sCourse* pCourse = (IFF_STRUCT::sCourse*)pItem->pData;

	const Bitmap* pBitmap;
	if (pCourse)
		pBitmap =
			pManager->GetBitmap("ITEMS", MakeStr("mid_%s", pCourse->c.Icon));
	else
		pBitmap = pManager->GetBitmap("ITEMS", "mid_map_hide");

	if (pBitmap)
	{
		pGDI->DrawTexture(pBitmap,
			WRect(0, 0, (float)pBitmap->Width(), (float)pBitmap->Height()),
			WRect(pItem->pos.x, pItem->pos.y, (float)pBitmap->Width(),
				(float)pBitmap->Height()),
			0xffffffff, 0);

		if (IsLocalContent(S3_MAP_EVENT) && pCourse)
		{
			unsigned long rate =
				Doc()->GetMapEventPangRate(pCourse->c.TypeId & 0x3ffffff);
			char name[128] = { 0 };
			sprintf(name, "mid_pangrate_%d", rate);

			const Bitmap* pRate = g_pFresh->GetBitmap(name);
			if (pRate)
			{
				float x = pItem->pos.x + 5.0f;
				float y = (float)pBitmap->Height() - (float)pRate->Height() -
					2.0f + pItem->pos.y;
				pGDI->DrawTexture(pRate,
					WRect(x, y, (float)pRate->Width(), (float)pRate->Height()),
					0xffffffff, 0);
			}
		}
	}

	if (pCourse &&
		(((Doc()->m_curChannel.Type & 0x800) && pCourse->Difficulty >= 3) ||
			(m_bNoRandomMap && (pCourse->c.TypeId & 0x3ffffff) == 0x7f)))
	{
		const Bitmap* pLock =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "map_lock");
		if (pLock)
		{
			pGDI->DrawTexture(pLock,
				WRect(0, 0, (float)pLock->Width(), (float)pLock->Height()),
				WRect(pItem->pos.x, pItem->pos.y, (float)pLock->Width(),
					(float)pLock->Height()),
				0xffffffff, 0);
		}
	}

	if (IsLocalContent(S3_ROOKIE_CHANNEL) && pCourse &&
		(Doc()->m_curChannel.Type & 0x800) &&
		!Doc()->CanUseRookieChannelMap(pCourse->c.TypeId & 0x3ffffff))
	{
		const Bitmap* pLock =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "map_lock");
		if (pLock)
		{
			pGDI->DrawTexture(pLock,
				WRect(0, 0, (float)pLock->Width(), (float)pLock->Height()),
				WRect(pItem->pos.x, pItem->pos.y, (float)pLock->Width(),
					(float)pLock->Height()),
				0xffffffff, 0);
		}
	}

	if (pItem->selected && pItem->pData)
	{
		const Bitmap* pSelect =
			g_pFresh->GetManager()->GetBitmap("RANKING", "map_select");
		if (pSelect)
		{
			pGDI->DrawTexture(pSelect,
				WRect(pItem->pos.x - 6.0f, pItem->pos.y - 6.0f,
					(float)pSelect->Width(), (float)pSelect->Height()),
				0xffffffff, 0);
		}
	}
}
