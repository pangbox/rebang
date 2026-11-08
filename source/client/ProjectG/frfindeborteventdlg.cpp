#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "frfindeborteventdlg.h"
#include "shareddoc.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrFindEbortEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrFindEbortEventDlg, FrForm)

ON_FRESH_VI("current_ebort", FRCMD_OWNERDRAW,
	FrFindEbortEventDlg::OnCurrentItemOwnerDraw)

END_FRESH_MSGMAP()

FrFindEbortEventDlg::FrFindEbortEventDlg()
	: m_reserved(0)
{
	for (int i = 0; i < 4; i++)
	{
		m_ebortCount[i] = 0;
		m_countPos[i] = WPoint(0, 0);
	}
}

FrFindEbortEventDlg::~FrFindEbortEventDlg()
{
}

void FrFindEbortEventDlg::OnCurrentItemOwnerDraw(int param)
{
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;
	int space = pDevice->SetSpace(0);

	WRect rect = GetRect();
	for (int i = 0; i < 4; i++)
	{
		WPoint pos = m_countPos[i];
		pos.x += rect.x;
		pos.y += rect.y;

		pDevice->PrintText(pos, 2, MakeStr("%d", m_ebortCount[i]), 0);
	}

	pDevice->SetSpace(space);
}

bool FrFindEbortEventDlg::OnInit()
{
	bool bFind[4] = {
		false,
	};

	for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); ++it)
	{
		if ((*it).tid == 0x1a000045)
		{
			m_ebortCount[0] = (*it).Common[0];
			bFind[0] = true;
		}
		else if ((*it).tid == 0x1a000044)
		{
			m_ebortCount[1] = (*it).Common[0];
			bFind[1] = true;
		}
		else if ((*it).tid == 0x1a000047)
		{
			m_ebortCount[2] = (*it).Common[0];
			bFind[2] = true;
		}
		else if ((*it).tid == 0x1a000046)
		{
			m_ebortCount[3] = (*it).Common[0];
			bFind[3] = true;
		}

		if (bFind[0] && bFind[1] && bFind[2] && bFind[3])
			break;
	}

	m_countPos[0] = WPoint(551, 59);
	m_countPos[1] = WPoint(551, 74);
	m_countPos[2] = WPoint(551, 89);
	m_countPos[3] = WPoint(551, 104);

	return true;
}
