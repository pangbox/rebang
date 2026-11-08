#include "frquickmovedlg.h"
#include "frviewer.h"
#include "frbutton.h"
#include "mathconsts.h"

static char s_descImage[5][64] = { "DUMMY", "user_notice_1.jpg",
	"user_notice_2.jpg", "user_notice_3.jpg", "user_notice_4.jpg" };

static char s_btnImage[5][64] = { "DUMMY", "btn_notice_go_", "DUMMY", "DUMMY",
	"btn_notice_go_selfdesign_" };

IMPLEMENT_OBJECT(FrQuickMoveDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrQuickMoveDlg, FrForm)

ON_FRESH_VI("desc_view", FRCMD_INIT, FrQuickMoveDlg::OnInitDescViewer)
ON_FRESH_VV("desc_view", FRCMD_LBUTTONDOWN, FrQuickMoveDlg::OnLBDownViewer)

ON_FRESH_VI("button_prev", FRCMD_INIT, FrQuickMoveDlg::OnInitBtnPrev)
ON_FRESH_VI("button_next", FRCMD_INIT, FrQuickMoveDlg::OnInitBtnNext)
ON_FRESH_VI("button_move", FRCMD_INIT, FrQuickMoveDlg::OnInitBtnMove)

ON_FRESH_VV("button_prev", FRCMD_LBUTTONDOWN, FrQuickMoveDlg::OnLBDownBtnPrev)
ON_FRESH_VV("button_next", FRCMD_LBUTTONDOWN, FrQuickMoveDlg::OnLBDownBtnNext)
ON_FRESH_VV("button_move", FRCMD_LBUTTONDOWN, FrQuickMoveDlg::OnLBDownBtnMove)

END_FRESH_MSGMAP()

FrQuickMoveDlg::FrQuickMoveDlg()
{
	m_pDescViewer = NULL;
	memset(m_descImage, 0, sizeof(m_descImage));

	m_page = 1;
}

FrQuickMoveDlg::~FrQuickMoveDlg()
{
	m_page = 1;
}

void FrQuickMoveDlg::SetDescImage(const char* image)
{
	if (m_pDescViewer)
	{
		memset(m_descImage, 0, sizeof(m_descImage));
		strcpy(m_descImage, image);
		m_pDescViewer->ShowScrollBar(true);
		m_pDescViewer->Open(m_descImage);
	}
}

void FrQuickMoveDlg::OnInitDescViewer(int param)
{
	m_pDescViewer = DYNAMIC_CAST(FrViewer, param);

	if (m_pDescViewer)
	{
		SetDescImage(s_descImage[m_page]);
	}
}

void FrQuickMoveDlg::OnLBDownViewer()
{
	Close(true);
}

void FrQuickMoveDlg::OnInitBtnPrev(int param)
{
	m_pBtnPrev = DYNAMIC_CAST(FrButton, param);

	if (m_pBtnPrev)
	{
		m_pBtnPrev->SetPushDelay(0.1f);
	}
}

void FrQuickMoveDlg::OnInitBtnNext(int param)
{
	m_pBtnNext = DYNAMIC_CAST(FrButton, param);

	if (m_pBtnNext)
	{
		m_pBtnNext->SetPushDelay(0.1f);
	}
}

void FrQuickMoveDlg::OnInitBtnMove(int param)
{
	m_pBtnMove = DYNAMIC_CAST(FrButton, param);

	if (m_pBtnMove)
	{
		m_pBtnMove->SetPushDelay(0.1f);
	}
}

void FrQuickMoveDlg::OnLBDownBtnPrev()
{
	if (m_page > 1)
	{
		m_page--;
	}

	SetDescImage(s_descImage[m_page]);

	if (m_page == 3 || m_page == 2)
	{
		m_pBtnMove->SetVisible(false);
	}
	else
	{
		m_pBtnMove->SetVisible(true);
		m_pBtnMove->SetButtonImg(MakeStr("%sn", s_btnImage[m_page]),
			FrButton::NORMAL);
		m_pBtnMove->SetButtonImg(MakeStr("%so", s_btnImage[m_page]),
			FrButton::OVER);
		m_pBtnMove->SetButtonImg(MakeStr("%sd", s_btnImage[m_page]),
			FrButton::PRESSED);
	}
}

void FrQuickMoveDlg::OnLBDownBtnNext()
{
	if (m_page < 4)
	{
		m_page++;
	}

	SetDescImage(s_descImage[m_page]);

	if (m_page == 3 || m_page == 2)
	{
		m_pBtnMove->SetVisible(false);
	}
	else
	{
		m_pBtnMove->SetVisible(true);
		m_pBtnMove->SetButtonImg(MakeStr("%sn", s_btnImage[m_page]),
			FrButton::NORMAL);
		m_pBtnMove->SetButtonImg(MakeStr("%so", s_btnImage[m_page]),
			FrButton::OVER);
		m_pBtnMove->SetButtonImg(MakeStr("%sd", s_btnImage[m_page]),
			FrButton::PRESSED);
	}
}

void FrQuickMoveDlg::OnLBDownBtnMove()
{
	switch (m_page)
	{
	case 1:

		CTaskManager::Instance()->ChangeTask("CShopTask", "", 0);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 198, 4, 0, 0, 0);
		break;

	case 4:

		CTaskManager::Instance()->ChangeTask("CShopTask", "", 0);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 198, 3, 0, 0, 0);

		break;
	}

	Close(true);
}
