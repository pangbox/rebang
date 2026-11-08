#include "newrecorddlg.h"
#include "courseorder.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fresh.h"
#include "commonutil.h"
#include "s5/utilities.h"
#include "s5/classicserver/classicserver.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrNewRecordDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrNewRecordDlg, FrForm)

ON_FRESH_VI("courselist", FRCMD_INIT, FrNewRecordDlg::OnCourseListInit)
ON_FRESH_VI("courselist", FRCMD_OWNERDRAW,
	FrNewRecordDlg::OnCourseListOwnerDraw)

END_FRESH_MSGMAP()

void FrNewRecordDlg::OnCourseListInit(int param)
{
	m_pCourseList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);

	if (m_pCourseList == NULL)
		return;

	std::map<unsigned int, IFF_STRUCT::sCourse>& courseMap =
		ItemManager()->m_CourseMap;

	for (int i = 0; i < 18; ++i)
	{
		std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it =
			courseMap.find(s_courseOrder[i] | 0x28000000);

		if (it == courseMap.end())
			continue;

		IFF_STRUCT::sCourse* pCourse = &it->second;

		if (pCourse->c.Final == false)
			continue;

		m_pCourseList->AddItem(pCourse);
	}
}

void FrNewRecordDlg::OnCourseListOwnerDraw(int param)
{
	if (param == NULL)
		return;

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI == NULL)
		return;

	FrListItem* pItem = (FrListItem*)param;
	IFF_STRUCT::sCourse* pCourse = (IFF_STRUCT::sCourse*)pItem->pData;

	if (pCourse == NULL)
		return;

	const Bitmap* pBitmap =
		pManager->GetBitmap("COURSE", MakeStr("small_%s", pCourse->c.Icon));

	if (pBitmap == NULL)
		return;

	int index = pCourse->c.TypeId & 0x3FFFFFF;

	sMapStatistics* pStat =
		S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())
		? &Doc()->m_myInfo.classicMapStat[index]
		: &Doc()->m_myInfo.mapStat[index];

	int bestScore = pStat->cBestScore;
	WRect rect(pItem->pos.x, pItem->pos.y, (float)pBitmap->Width(),
		(float)pBitmap->Height());

	pGDI->DrawTexture(pBitmap, rect,
		(bestScore >= 0 || bestScore == 127) ? 0x60000000 : 0xFFFFFFFF, 0);
}
