#include "stdafx.h"
#include "guidebook.h"
#include "mousecursor.h"

extern Fresh* g_pFresh;
extern WResourceManager* g_resrcmng;
extern WView* g_view;

IMPLEMENT_OBJECT(FrGuideBookDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrGuideBookDlg, FrForm)
ON_FRESH_VI("content", FRCMD_INIT, FrGuideBookDlg::OnContentInit)
ON_FRESH_VI("content", FRCMD_OWNERDRAW, FrGuideBookDlg::OnContentOwnerDraw)
ON_FRESH_VI("prev", FRCMD_INIT, FrGuideBookDlg::OnPrevInit)
ON_FRESH_VV("prev", FRCMD_LBUTTONUP, FrGuideBookDlg::OnPrevBtnUp)
ON_FRESH_VI("next", FRCMD_INIT, FrGuideBookDlg::OnNextInit)
ON_FRESH_VV("next", FRCMD_LBUTTONUP, FrGuideBookDlg::OnNextBtnUp)
ON_FRESH_VI("tip1", FRCMD_INIT, FrGuideBookDlg::OnTip1Init)
ON_FRESH_VI("tip2", FRCMD_INIT, FrGuideBookDlg::OnTip2Init)
ON_FRESH_VI("tip3", FRCMD_INIT, FrGuideBookDlg::OnTip3Init)
ON_FRESH_VI("tip4", FRCMD_INIT, FrGuideBookDlg::OnTip4Init)
ON_FRESH_VI("tip5", FRCMD_INIT, FrGuideBookDlg::OnTip5Init)
ON_FRESH_VI("tip6", FRCMD_INIT, FrGuideBookDlg::OnTip6Init)
ON_FRESH_VI("tip7", FRCMD_INIT, FrGuideBookDlg::OnTip7Init)
ON_FRESH_VI("tip8", FRCMD_INIT, FrGuideBookDlg::OnTip8Init)
ON_FRESH_VI("tip9", FRCMD_INIT, FrGuideBookDlg::OnTip9Init)
ON_FRESH_VV("tip1", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip1BtnUp)
ON_FRESH_VV("tip2", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip2BtnUp)
ON_FRESH_VV("tip3", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip3BtnUp)
ON_FRESH_VV("tip4", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip4BtnUp)
ON_FRESH_VV("tip5", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip5BtnUp)
ON_FRESH_VV("tip6", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip6BtnUp)
ON_FRESH_VV("tip7", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip7BtnUp)
ON_FRESH_VV("tip8", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip8BtnUp)
ON_FRESH_VV("tip9", FRCMD_LBUTTONUP, FrGuideBookDlg::OnTip9BtnUp)
END_FRESH_MSGMAP()

FrGuideBookDlg::FrGuideBookDlg()
	: m_pPrev(NULL), m_pNext(NULL)
{
	for (int i = 0; i < 9; ++i)
		m_pTip[i] = NULL;
	m_pPage = NULL;
}

FrGuideBookDlg::~FrGuideBookDlg()
{
	if (g_resrcmng && m_pPage)
	{
		g_resrcmng->Release(m_pPage);
		m_pPage = NULL;
	}
}

void FrGuideBookDlg::OpenChapter(int chapter)
{
	if (m_pPrev)
		m_pPrev->Enable(chapter > 1);
	for (unsigned int i = 0; i < 9; ++i)
		if (i + 1 != chapter)
			m_pTip[i]->Unselect(false);
	if (g_resrcmng && m_pPage)
	{
		g_resrcmng->Release(m_pPage);
		m_pPage = NULL;
	}
	m_pPage =
		g_resrcmng->GetOverlay(MakeStr("%d_%d.tga", chapter, 1), 0x80000000);
	if (m_pPage)
	{
		m_chapter = chapter;
		m_page = 1;
	}
}

void FrGuideBookDlg::OnContentInit(int param)
{
	m_pContent = DYNAMIC_CAST(FrArea, (IObject*)param);
}

void FrGuideBookDlg::OnContentOwnerDraw(int param)
{
	if (m_pPage)
	{
		WRect src(0, 0, 1, 1);
		WRect rect = m_pContent->GetRect();
		rect.x += (int)(((float)(rect.w - m_pPage->GetWidth()) + 1) * .5f);
		rect.y += (int)((rect.h - m_pPage->GetHeight() + 1) * .5f);
		rect.w = m_pPage->GetWidth();
		rect.h = m_pPage->GetHeight();
		m_pPage->Render(g_view, src, rect, 0, 0xffffffff, 0, 0);
	}
}

void FrGuideBookDlg::OnPrevInit(int param)
{
	m_pPrev = DYNAMIC_CAST(FrButton, (IObject*)param);
	m_pPrev->Enable(false);
	m_chapter = 1;
	m_page = 1;
	if (g_resrcmng && m_pPage)
	{
		g_resrcmng->Release(m_pPage);
		m_pPage = NULL;
	}
	m_pPage = g_resrcmng->GetOverlay(MakeStr("%d_%d.tga", m_chapter, m_page),
		0x80000000);
}

void FrGuideBookDlg::OnPrevBtnUp()
{
	int chapter = m_chapter;
	int page = m_page - 1;
	for (; chapter > 0; --chapter, page = 1)
	{
		for (; page > 0; --page)
		{
			if (g_resrcmng && m_pPage)
			{
				g_resrcmng->Release(m_pPage);
				m_pPage = NULL;
			}
			m_pPage = g_resrcmng->GetOverlay(
				MakeStr("%d_%d.tga", chapter, page), 0x80000000);
			if (m_pPage)
			{
				m_pNext->Enable(true);
				m_chapter = chapter;
				m_page = page;
				for (unsigned int i = 0; i < 9; ++i)
					if (i + 1 == chapter)
						m_pTip[i]->Select(false);
					else
						m_pTip[i]->Unselect(false);
				if (chapter == 1 && page == 1 && m_pPrev)
					m_pPrev->Enable(false);
				return;
			}
		}
	}
}

void FrGuideBookDlg::OnNextInit(int param)
{
	m_pNext = DYNAMIC_CAST(FrButton, (IObject*)param);
}

void FrGuideBookDlg::OnNextBtnUp()
{
	WOverlay* overlay = g_resrcmng->GetOverlay(
		MakeStr("%d_%d.tga", m_chapter, m_page + 1), 0x80000000);
	if (overlay)
	{
		if (g_resrcmng && m_pPage)
		{
			g_resrcmng->Release(m_pPage);
			m_pPage = NULL;
		}
		m_pPage = overlay;
		m_pPrev->Enable(true);
		++m_page;
	}
	else
	{
		overlay = g_resrcmng->GetOverlay(MakeStr("%d_%d.tga", m_chapter + 1, 1),
			0x80000000);
		if (overlay)
		{
			if (g_resrcmng && m_pPage)
			{
				g_resrcmng->Release(m_pPage);
				m_pPage = NULL;
			}
			m_pPage = overlay;
			m_pPrev->Enable(true);
			++m_chapter;
			m_page = 1;
			for (unsigned int i = 0; i < 9; ++i)
				if (i + 1 == m_chapter)
					m_pTip[i]->Select(false);
				else
					m_pTip[i]->Unselect(false);
		}
		else
			m_pNext->Enable(false);
	}
}

void FrGuideBookDlg::OnTip1Init(int param)
{
	m_pTip[0] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
	m_pTip[0]->Select(false);
}

void FrGuideBookDlg::OnTip2Init(int param)
{
	m_pTip[1] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip3Init(int param)
{
	m_pTip[2] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip4Init(int param)
{
	m_pTip[3] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip5Init(int param)
{
	m_pTip[4] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip6Init(int param)
{
	m_pTip[5] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip7Init(int param)
{
	m_pTip[6] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip8Init(int param)
{
	m_pTip[7] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip9Init(int param)
{
	m_pTip[8] = DYNAMIC_CAST(FrTextButton, (IObject*)param);
}

void FrGuideBookDlg::OnTip1BtnUp()
{
	OpenChapter(1);
}

void FrGuideBookDlg::OnTip2BtnUp()
{
	OpenChapter(2);
}

void FrGuideBookDlg::OnTip3BtnUp()
{
	OpenChapter(3);
}

void FrGuideBookDlg::OnTip4BtnUp()
{
	OpenChapter(4);
}

void FrGuideBookDlg::OnTip5BtnUp()
{
	OpenChapter(5);
}

void FrGuideBookDlg::OnTip6BtnUp()
{
	OpenChapter(6);
}

void FrGuideBookDlg::OnTip7BtnUp()
{
	OpenChapter(7);
}

void FrGuideBookDlg::OnTip8BtnUp()
{
	OpenChapter(8);
}

void FrGuideBookDlg::OnTip9BtnUp()
{
	OpenChapter(9);
}

IMPLEMENT_ACTOR(CGuideBook, IActor)

CGuideBook::CGuideBook()
	: m_pDlg(NULL)
{
	m_dispPriority = DISP_PRIORITY_4;
}

void CGuideBook::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 298:
		if (!m_pDlg)
		{
			m_pDlg = CreateForm<FrGuideBookDlg>(g_pFresh->GetManager(), this,
				"guide_book", NULL);
			if (m_pDlg)
			{
				m_pDlg->Open((FRESH_PFN_RESULT)&CGuideBook::OnDlgResult, 1);
				m_pDlg->EnableDrag(false);
			}
			CMouseCursor::Instance()->SetMode(CMouseCursor::SCREEN_GAME);
			CMouseCursor::Instance()->Enable(CMouseCursor::AREA_AUXMENU_TIP,
				false);
		}
		break;
	case 299:
		if (m_pDlg)
			m_pDlg->Close(FrOK, true);
		break;
	case 300:
		*(bool*)msg.param1 = m_pDlg != NULL;
		break;
	}
}

bool CGuideBook::OnDlgResult(int result, FrForm* form)
{
	m_pDlg = NULL;
	CMouseCursor::Instance()->Enable(CMouseCursor::AREA_AUXMENU_TIP, true);
	return true;
}
