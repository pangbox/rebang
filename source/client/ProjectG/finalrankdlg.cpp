#include "finalrankdlg.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "projectg.h"
#include "netresourcemanager.h"
extern Fresh* g_pFresh;
bool FinalRankCompare(const void* a, const void* b);

IMPLEMENT_OBJECT(FrFinalRankDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFinalRankDlg, FrForm)

ON_FRESH_VI("list_left", FRCMD_INIT, FrFinalRankDlg::OnRankLeftInit)
ON_FRESH_VI("list_left", FRCMD_OWNERDRAW, FrFinalRankDlg::OnRankLeftOwnerDraw)
ON_FRESH_VI("list_right", FRCMD_INIT, FrFinalRankDlg::OnRankRightInit)
ON_FRESH_VI("list_right", FRCMD_OWNERDRAW, FrFinalRankDlg::OnRankRightOwnerDraw)

END_FRESH_MSGMAP()
FrFinalRankDlg::FrFinalRankDlg()
	: m_pRankLeft(NULL), m_pRankRight(NULL)

{
	std::vector<sRivalData>::iterator it;

	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		if ((*it).state == 3)
		{
			(*it).rank = 0xFF;
			continue;
		}

		(*it).rank = 1;
		std::vector<sRivalData>::iterator it2;
		for (it2 = Doc()->m_rivalList.begin(); it2 != Doc()->m_rivalList.end();
			++it2)
		{
			if (it == it2 || (*it2).state == 3)
				continue;

			if ((*it).totalScore > (*it2).totalScore ||
				((*it).totalScore == (*it2).totalScore &&
					(*it).totalPang < (*it2).totalPang))
				(*it).rank++;
		}
	}
}

void FrFinalRankDlg::OnRankLeftInit(int param)

{
	m_pRankLeft = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	std::vector<sRivalData>::iterator it;

	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		if ((*it).rank < 16)
			m_pRankLeft->AddItem(&(*it));
	}

	m_pRankLeft->SortItem(FinalRankCompare);
}

void FrFinalRankDlg::OnRankLeftOwnerDraw(int param)

{
	if (param == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	int space = pGDI->SetSpace(0);
	FrListItem* pItem = (FrListItem*)param;
	sRivalData* pRival = (sRivalData*)pItem->pData;
	Doc()->m_rivalList.end();

	if (pRival->oid == MyGuid(false))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->SetTextStyle(0);
	if (pRival->state == 3)

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
	else
		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d",
			pRival->rank);
	float nickX = 95.0f;

	if (pRival->guildUID)
	{
		const Bitmap* pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(pRival->guildMark);

		if (pEmblem)
		{
			pGDI->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 95.0f, pItem->pos.y + 3.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xFFFFFFFF, 0);
			nickX = 122.0f;
		}
	}

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(nickX + pItem->pos.x, pItem->pos.y + 3.0f), 0,
			pRival->nickname, 120.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 186.0f, pItem->pos.y + 3.0f), 2, "%d",
		pRival->totalScore);

	pGDI->Print(WPoint(pItem->pos.x + 230.0f, pItem->pos.y + 3.0f), 2, "%I64d",
		pRival->totalPang);

	if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 268.0f, pItem->pos.y + 3.0f), 2,
			"End");
	else
		pGDI->Print(WPoint(pItem->pos.x + 268.0f, pItem->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival->hole));

	pGDI->SetSpace(space);
}

void FrFinalRankDlg::OnRankRightInit(int param)

{
	m_pRankRight = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	std::vector<sRivalData>::iterator it;

	for (it = Doc()->m_rivalList.begin(); it != Doc()->m_rivalList.end(); ++it)
	{
		if ((*it).rank > 15)
			m_pRankRight->AddItem(&(*it));
	}

	m_pRankRight->SortItem(FinalRankCompare);
}

void FrFinalRankDlg::OnRankRightOwnerDraw(int param)

{
	if (param == NULL)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	int space = pGDI->SetSpace(0);
	FrListItem* pItem = (FrListItem*)param;
	sRivalData* pRival = (sRivalData*)pItem->pData;
	Doc()->m_rivalList.end();

	if (pRival->oid == MyGuid(false))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pRival->state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->SetTextStyle(0);
	if (pRival->state == 3)

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
	else
		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d",
			pRival->rank);
	float nickX = 95.0f;

	if (pRival->guildUID)
	{
		const Bitmap* pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(pRival->guildMark);

		if (pEmblem)
		{
			pGDI->DrawTexture(pEmblem,
				WRect(pItem->pos.x + 95.0f, pItem->pos.y + 3.0f - 6.0f,
					(float)pEmblem->Width(), (float)pEmblem->Height()),
				0xFFFFFFFF, 0);
			nickX = 122.0f;
		}
	}

	if (CProjectG::Instance()->HidePrivacy() == false)
		g_pFresh->GetManager()->PrintText(
			WPoint(nickX + pItem->pos.x, pItem->pos.y + 3.0f), 0,
			pRival->nickname, 120.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 186.0f, pItem->pos.y + 3.0f), 2, "%d",
		pRival->totalScore);

	pGDI->Print(WPoint(pItem->pos.x + 230.0f, pItem->pos.y + 3.0f), 2, "%I64d",
		pRival->totalPang);

	if (pRival->state == 2 || GetHoleIndex(pRival->hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 268.0f, pItem->pos.y + 3.0f), 2,
			"End");
	else
		pGDI->Print(WPoint(pItem->pos.x + 268.0f, pItem->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(pRival->hole));

	pGDI->SetSpace(space);
}

bool FinalRankCompare(const void* a, const void* b)

{
	const sRivalData* p1 = (const sRivalData*)a;
	const sRivalData* p2 = (const sRivalData*)b;

	if (p1->quitOrder > p2->quitOrder)
		return true;
	if (p1->quitOrder < p2->quitOrder)

		return false;

	if (p1->totalScore < p2->totalScore)
		return true;

	if (p1->totalScore > p2->totalScore)
		return false;

	if (p1->totalPang > p2->totalPang)
		return true;

	if (p1->totalPang < p2->totalPang)
		return false;

	return p1->order < p2->order;
}
