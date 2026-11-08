#include "standinginline.h"
#include "frviewer.h"
#include "frlistbox.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(CStandingInLine, FrForm)
BEGIN_FRESH_MSGMAP(CStandingInLine, FrForm)

ON_FRESH_VI("Viewer_Total", FRCMD_INIT, CStandingInLine::OnView_TotalInit)
ON_FRESH_VI("Viewer_Total", FRCMD_OWNERDRAW,
	CStandingInLine::OnView_TotalOwnerDraw)
ON_FRESH_VV("Viewer_Total", FRCMD_LBUTTONUP,
	CStandingInLine::OnView_TotalButtonUp)

ON_FRESH_VI("List_BtnArray", FRCMD_INIT, CStandingInLine::OnArrayListInit)
ON_FRESH_VI("List_BtnArray", FRCMD_OWNERDRAW,
	CStandingInLine::OnArrayListOwnerDraw)
ON_FRESH_VV("List_BtnArray", FRCMD_LBUTTONDOWN,
	CStandingInLine::OnArrayListBtnDown)
ON_FRESH_VV("List_BtnArray", FRCMD_RBUTTONDOWN,
	CStandingInLine::OnArrayListRBtnDown)

END_FRESH_MSGMAP()

CStandingInLine::CStandingInLine()
{
	m_pViewTotal = NULL;
	m_pArrayList = NULL;
	m_selectNum = 0;
}

CStandingInLine::~CStandingInLine()
{
}

void CStandingInLine::OnView_TotalInit(int param)
{
	m_pViewTotal = DYNAMIC_CAST(FrViewer, param);

	if (m_pViewTotal)
	{
		WRect rect = m_pViewTotal->GetRect();

		m_pViewTotal->SetRect(WRect(rect.x, rect.y, rect.w + 91.0f, rect.h));
	}
}

void CStandingInLine::OnView_TotalOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	gdi->SetTextColor(0xff1f1f1f, 0xffffffff);

	WRect rect = m_pViewTotal->GetRect();

	gdi->Print(WPoint(rect.x + 30.0f, rect.y), 0, "\xc0\xfc \xc3\xbc");
}

void CStandingInLine::OnView_TotalButtonUp()
{
	m_selectNum = 0;
	FrForm::OnOK();
}

void CStandingInLine::OnArrayListInit(int param)
{
	m_pArrayList = DYNAMIC_CAST(FrListBox, param);

	if (m_pArrayList)
	{
		m_pArrayList->UseRightButton(true);

		for (int i = 1; i < 14; ++i)
		{
			m_pArrayList->AddItem((void*)i);
		}
	}
}

void CStandingInLine::OnArrayListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL)
		return;
	int index = (int)pItem->pData;
	if (index == 0)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	gdi->SetTextColor(0xff1f1f1f, 0xffffffff);
	gdi->Print(pItem->pos, 0, "%s", g_szArrayName[index - 1]);
}

void CStandingInLine::OnArrayListBtnDown()
{
	FrListItem* pItem = m_pArrayList->GetItemUnderCursor();
	if (pItem == NULL)
		return;

	int index = (int)pItem->pData;
	if (index == 0)
		return;

	if (index > 13)
		return;

	m_selectNum = index;

	FrForm::OnOK();
}

void CStandingInLine::OnArrayListRBtnDown()
{
	FrForm::OnCancel();
}
