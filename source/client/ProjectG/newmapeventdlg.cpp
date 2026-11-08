#include "newmapeventdlg.h"
#include "frarea.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "../../shared/sharedtables.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrNewMapEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrNewMapEventDlg, FrForm)

ON_FRESH_VI("caption", FRCMD_INIT, FrNewMapEventDlg::OnCaptionInit)
ON_FRESH_VI("info1", FRCMD_INIT, FrNewMapEventDlg::OnInfo1Init)
ON_FRESH_VI("score0", FRCMD_INIT, FrNewMapEventDlg::OnScore0Init)
ON_FRESH_VI("score1", FRCMD_INIT, FrNewMapEventDlg::OnScore1Init)
ON_FRESH_VI("score2", FRCMD_INIT, FrNewMapEventDlg::OnScore2Init)
ON_FRESH_VI("score3", FRCMD_INIT, FrNewMapEventDlg::OnScore3Init)
ON_FRESH_VI("score4", FRCMD_INIT, FrNewMapEventDlg::OnScore4Init)
ON_FRESH_VI("score5", FRCMD_INIT, FrNewMapEventDlg::OnScore5Init)
ON_FRESH_VV("score0", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore0OwnerDraw)
ON_FRESH_VV("score1", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore1OwnerDraw)
ON_FRESH_VV("score2", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore2OwnerDraw)
ON_FRESH_VV("score3", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore3OwnerDraw)
ON_FRESH_VV("score4", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore4OwnerDraw)
ON_FRESH_VV("score5", FRCMD_OWNERDRAW, FrNewMapEventDlg::OnScore5OwnerDraw)

END_FRESH_MSGMAP()

FrNewMapEventDlg::FrNewMapEventDlg()
{
}

FrNewMapEventDlg::~FrNewMapEventDlg()
{
}

void FrNewMapEventDlg::OnCaptionInit(int param)
{
	FrArea* pCaption = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnInfo1Init(int param)
{
	FrArea* pInfo = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore0Init(int param)
{
	m_pScore[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore1Init(int param)
{
	m_pScore[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore2Init(int param)
{
	m_pScore[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore3Init(int param)
{
	m_pScore[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore4Init(int param)
{
	m_pScore[4] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore5Init(int param)
{
	m_pScore[5] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrNewMapEventDlg::OnScore0OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 1)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "5under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "5under_00");

	const WRect& rect = m_pScore[0]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}

void FrNewMapEventDlg::OnScore1OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 2)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "10under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "10under_00");

	const WRect& rect = m_pScore[1]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}

void FrNewMapEventDlg::OnScore2OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 4)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "15under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "15under_00");

	const WRect& rect = m_pScore[2]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}

void FrNewMapEventDlg::OnScore3OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 8)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "20under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "20under_00");

	const WRect& rect = m_pScore[3]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}

void FrNewMapEventDlg::OnScore4OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 16)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "25under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "25under_00");

	const WRect& rect = m_pScore[4]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}

void FrNewMapEventDlg::OnScore5OwnerDraw()
{
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pBitmap;
	if (Doc()->m_newMapEventMask & 32)
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "30under_01");
	else
		pBitmap = pManager->GetBitmap("NEWMAP_EVENT", "30under_00");

	const WRect& rect = m_pScore[5]->GetRect();
	WRect src(0, 0, pBitmap->Width(), pBitmap->Height());
	WRect dst(rect.x, rect.y, src.w, src.h);
	pGDI->DrawTexture(pBitmap, src, dst, 0xffffffff, 0);
}
