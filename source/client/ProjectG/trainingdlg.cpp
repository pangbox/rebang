#include "trainingdlg.h"
#include "guidebook.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "wlocalize.h"
#include "wlocalize.h"
#include <io.h>

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrTrainingDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrTrainingDlg, FrForm)

ON_FRESH_VI("training", FRCMD_INIT, FrTrainingDlg::OnTrainingInit)
ON_FRESH_VI("training", FRCMD_OWNERDRAW, FrTrainingDlg::OnTrainingOwnerDraw)
ON_FRESH_VV("training", FRCMD_LBUTTONUP, FrTrainingDlg::OnTrainingBtnUp)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrTrainingDlg::OnTrainingCloseBtnUp)

END_FRESH_MSGMAP()

sButtonInfo buttonList[4] = {
	{ 0, "i61",
     K2L_Compatibility(
			"\xc4\xb3\xb5\xf0\xb6\xfb \xb9\xe8\xbf\xec\xb1\xe2")         },
	{ 1, "i62",
     K2L_Compatibility(
			"\xc8\xa5\xc0\xda \xbf\xac\xbd\xc0\xc7\xcf\xb1\xe2")         },
	{ 2, "i65", K2L_Compatibility("\xc4\xb3\xb5\xf0\xba\xcf")            },
	{ 3, "i64",
     K2L_Compatibility(
			"\xc6\xaf\xbc\xf6\xbc\xa6 \xbf\xac\xbd\xc0\xc7\xcf\xb1\xe2") }
};

void FrTrainingDlg::OnTrainingInit(int param)
{
	m_pTraining = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pTraining->ClearItem();

	m_pTraining->AddItem(&buttonList[0]);
	m_pTraining->AddItem(&buttonList[1]);
	m_pTraining->AddItem(&buttonList[2]);

	m_pOptionDlg = NULL;
}

void FrTrainingDlg::OnTrainingBtnUp()
{
	if (m_pTraining == NULL)
		return;

	FrListItem* pItem = m_pTraining->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	sButtonInfo* pInfo = (sButtonInfo*)pItem->pData;
	if (pInfo == NULL)
		return;

	switch (pInfo->type)
	{
	case 0:

		Doc()->m_gameMode = 0;

		SetCurMap(0xfd);
		Doc()->m_golfGame.gameType = 12;
		strcpy(Doc()->m_golfGame.gameTypeName, Doc()->m_gameTypeInfo[12].name);
		Doc()->m_golfGame.holes = 1;

		for (unsigned char i = 0; i < 18; i++)
			Doc()->m_holeOrder[i] = 0;

		Doc()->m_holeOrder[0] = 1;

		break;

	case 1:
	{
		Doc()->m_gameMode = 2;

		SetCurMap(0);
		Doc()->m_golfGame.gameType = 0;
		strcpy(Doc()->m_golfGame.gameTypeName, Doc()->m_gameTypeInfo[0].name);
		Doc()->m_golfGame.holes = 18;
		Doc()->m_holeType = 0;

		for (unsigned char i = 0; i < 18; i++)
		{
			Doc()->m_holeOrder[i] = i + 1;
			Doc()->m_courseMap[i] = rand() % 3;
		}
		WSendPacket send((enumClientPacket)0x5a);
		send.Encode1(0);
		send.Send(TO_GAME);
	}
	break;

	case 2:
	{
		FrGuideBookDlg* pDlg = CreateForm<FrGuideBookDlg>(
			g_pFresh->GetManager(), this, "guide_book", NULL);

		if (pDlg)
		{
			pDlg->Open(NULL, 1);
		}

		Close(FrCANCEL, true);
	}
		return;

	case 3:
		Doc()->m_gameMode = 3;

		SetCurMap(0xfd);
		Doc()->m_golfGame.gameType = 0;
		strcpy(Doc()->m_golfGame.gameTypeName, Doc()->m_gameTypeInfo[0].name);
		Doc()->m_golfGame.holes = 1;

		for (unsigned char i = 0; i < 18; i++)
			Doc()->m_holeOrder[i] = 0;

		Doc()->m_holeOrder[0] = 2;
		break;
	}

	Close(FrOK, true);
}

void FrTrainingDlg::OnTrainingOwnerDraw(int param)
{
	if (m_pTraining == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (param == NULL)
		return;

	FrListItem* pItem = (FrListItem*)param;
	sButtonInfo* pInfo = (sButtonInfo*)pItem->pData;
	if (pInfo == NULL)
		return;

	const Bitmap* pBase = g_pFresh->GetManager()->GetBitmap("ITEMS", "ch_base");
	if (pBase)
		pGDI->DrawTexture(pBase,
			WRect(pItem->pos.x, pItem->pos.y, (float)pBase->Width(),
				(float)pBase->Height()),
			0xFFFFFFFF, 0);

	const Bitmap* pIcon =
		g_pFresh->GetManager()->GetBitmap("ICONS", pInfo->icon);
	if (pIcon)
		pGDI->DrawTexture(pIcon,
			WRect(pItem->pos.x + 17.0f, pItem->pos.y + 5.0f,
				(float)pIcon->Width(), (float)pIcon->Height()),
			0xFFFFFFFF, 0);

	if (pItem->underCursor)
	{
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
		pGDI->SetTextStyle(2);
	}
	else
	{
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}

	g_pFresh->GetManager()->PrintText(
		WPoint(pItem->pos.x + 80.0f, pItem->pos.y + 22.0f), 0, pInfo->text,
		-1.0f, 0xFFFFFFFF);
}

void FrTrainingDlg::OnTrainingCloseBtnUp()
{
	Close(FrCANCEL, true);
}

bool FrTrainingDlg::OnNewTrainingOptionDlgResult(int result, FrForm* form)
{
	m_pOptionDlg = NULL;

	if (result == FrOK)
	{
		WSendPacket send((enumClientPacket)0x5a);
		send.Encode1(0);
		send.Send(TO_GAME);

		close(FrOK);
	}
	else
	{
		close(FrCANCEL);
	}
	return true;
}
