#include "noticedlg.h"
#include "frbutton.h"
#include "frviewer.h"
#include "frlistbox.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "fresh.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrNoticeDlg, FrForm)

char* num1[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };

BEGIN_FRESH_MSGMAP(FrNoticeDlg, FrForm)
ON_FRESH_VI("cancel", FRCMD_INIT, FrNoticeDlg::OnCancelInit)
ON_FRESH_VI("view", FRCMD_INIT, FrNoticeDlg::OnViewInit)
ON_FRESH_VV("view", FRCMD_LBUTTONDOWN, FrNoticeDlg::OnViewBtnDown)
ON_FRESH_VI("num", FRCMD_INIT, FrNoticeDlg::OnNumInit)
ON_FRESH_VV("num", FRCMD_LBUTTONUP, FrNoticeDlg::OnNumLBtnUp)
ON_FRESH_VI("num", FRCMD_OWNERDRAW, FrNoticeDlg::OnNumOwnerDraw)
ON_FRESH_VI("prev", FRCMD_INIT, FrNoticeDlg::OnPrevInit)
ON_FRESH_VV("prev", FRCMD_LBUTTONUP, FrNoticeDlg::OnPrevBtnUp)
ON_FRESH_VI("next", FRCMD_INIT, FrNoticeDlg::OnNextInit)
ON_FRESH_VV("next", FRCMD_LBUTTONUP, FrNoticeDlg::OnNextBtnUp)
END_FRESH_MSGMAP()

FrNoticeDlg::FrNoticeDlg()
{
	m_pCancel = NULL;
	m_pViewer = NULL;
	m_curIndex = 0;
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pNumList = NULL;
	m_typeId = 0;
	m_openedIndex = 0;
	m_bScrollBar = false;
	m_startIndex = -1;
}

FrNoticeDlg::~FrNoticeDlg()
{
	std::vector<sNoticeImage>::iterator it;
	for (it = m_images.begin(); it != m_images.end(); ++it)
		(*it).bnList.clear();
	m_images.clear();
}

void FrNoticeDlg::LoadOnly(const char* filename)
{
	if (filename)
	{
		m_images.clear();
		sNoticeImage image;
		strcpy(image.name, filename);
		m_images.push_back(image);
	}
}

void FrNoticeDlg::LoadInitFile(const char* filename)
{
	if (filename)
		LoadNoticeImageFile(filename, m_images, 10, "hg");

	if (m_startIndex >= 0)
	{
		m_curIndex = m_startIndex;
		if (m_pPrev)
			m_pPrev->SetVisible(false);
		if (m_pNext)
			m_pNext->SetVisible(false);
	}
	else if (m_images.size())
		m_curIndex = 0;
	else
	{
		if (m_pPrev)
			m_pPrev->SetVisible(false);
		if (m_pNext)
			m_pNext->SetVisible(false);
	}

	if (m_pPrev)
	{
		WRect rect = m_pPrev->GetRect();
		m_pPrev->MoveWindow(
			WPoint(rect.x - (m_images.size() * 15.0f + 20.0f), rect.y));
		m_pPrev->Enable(m_curIndex > 0);
	}
	if (m_pNext && m_images.size())
		m_pNext->Enable(m_curIndex < m_images.size() - 1);

	if (m_pNumList && m_startIndex == -1)
	{
		for (unsigned char i = 0; i < 10; ++i)
		{
			if (i >= 10 - m_images.size())
				m_pNumList->AddItem(num1[i - (10 - m_images.size())]);
			else
				m_pNumList->AddItem(NULL);
		}
	}
}

void FrNoticeDlg::Prev()
{
	OnPrevBtnUp();
}

void FrNoticeDlg::Next()
{
	OnNextBtnUp();
}

void FrNoticeDlg::OnPrevInit(int param)
{
	m_pPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pPrev)
		m_pPrev->SetPushDelay(0.0f);
}

void FrNoticeDlg::OnNextInit(int param)
{
	m_pNext = DYNAMIC_CAST(FrButton, param);
	if (m_pNext)
		m_pNext->SetPushDelay(0.0f);
}

void FrNoticeDlg::OnPrevBtnUp()
{
	if (m_curIndex != 0)
	{
		--m_curIndex;
		m_openedIndex = m_curIndex;
		OpenImage();
		SetScrollBar();
	}
}

void FrNoticeDlg::OnNextBtnUp()
{
	if (m_curIndex != m_images.size() - 1)
	{
		++m_curIndex;
		m_openedIndex = m_curIndex;
		OpenImage();
		SetScrollBar();
	}
}

bool FrNoticeDlg::OnInit()
{
	if (m_pViewer)
	{
		bool bScrollBar = false;
		float offset = floor(m_pViewer->GetScrollBarOffset() * 0.5f);
		OpenImage();
		int width = m_pViewer->GetSrcWidth();
		if (width > 550)
			width = 550;
		width -= (int)m_pViewer->GetViewWidth();
		int height = m_pViewer->GetSrcHeight();
		if (height > 400)
		{
			height = 400;
			bScrollBar = true;
		}
		if (bScrollBar)
			m_bScrollBar = true;
		else
			m_bScrollBar = false;
		if (bScrollBar)
			m_pViewer->ShowScrollBar(true);
		else
			m_pViewer->ShowScrollBar(false);
		height -= (int)m_pViewer->GetViewHeight();
		WRect rect = m_pViewer->GetRect();
		rect.w += width;
		rect.h += height;
		if (bScrollBar)
			rect.w += offset * 2.0f;
		m_pViewer->SetRect(rect);

		if (m_pCancel)
		{
			rect = m_pCancel->GetRect();
			rect.x += width;
			if (bScrollBar)
				rect.x += offset * 2.0f;
			else
				rect.x -= offset;
			rect.x -= 1.0f;
			m_pCancel->MoveWindow(WPoint(rect.x, rect.y));
		}

		rect = GetRect();
		rect.x -= width >> 1;
		rect.y -= height >> 1;
		rect.w += width;
		rect.h += height;
		if (bScrollBar)
			rect.w += offset * 2.0f;
		else
			rect.w -= offset;
		SetRect(rect);
		m_openedIndex = m_curIndex;
	}
	return FrForm::OnInit();
}

void FrNoticeDlg::SetScrollBar()
{
	bool bScrollBar = false;
	float offset = floor(m_pViewer->GetScrollBarOffset() * 0.5f);
	m_pViewer->GetSrcWidth();
	if (m_pViewer->GetSrcHeight() > 400)
		bScrollBar = true;

	WRect rect;
	if (m_pCancel)
	{
		rect = m_pCancel->GetRect();
		if (bScrollBar)
			rect.x += offset * 3.0f;
		else if (m_bScrollBar)
			rect.x -= offset * 3.0f;
		m_pCancel->MoveWindow(WPoint(rect.x, rect.y));
	}

	rect = GetRect();
	if (bScrollBar)
		rect.w += offset * 3.0f;
	else if (m_bScrollBar)
		rect.w -= offset * 3.0f;
	SetRect(rect);

	rect = m_pViewer->GetRect();
	if (bScrollBar)
		rect.w += offset * 2.0f;
	else if (m_bScrollBar)
		rect.w -= offset * 2.0f;
	m_pViewer->SetRect(rect);

	if (bScrollBar)
	{
		m_pViewer->ShowScrollBar(true);
		m_bScrollBar = true;
	}
	else
	{
		m_pViewer->ShowScrollBar(false);
		m_bScrollBar = false;
	}
}

void FrNoticeDlg::OnCancelInit(int param)
{
	m_pCancel = DYNAMIC_CAST(FrButton, param);
}

void FrNoticeDlg::OnViewInit(int param)
{
	m_pViewer = DYNAMIC_CAST(FrViewer, param);
}

void FrNoticeDlg::OnViewBtnDown()
{
	WPoint point;
	point = g_pFresh->GetManager()->GetMousePos();
	std::list<sNoticeImage::sBn>::iterator it;
	for (it = m_images[m_curIndex].bnList.begin();
		it != m_images[m_curIndex].bnList.end(); ++it)
	{
		WPoint pos(m_pViewer->GetRect().x, m_pViewer->GetRect().y);
		WRect rect(pos.x + (*it).rect.x, pos.y + (*it).rect.y, (*it).rect.w,
			(*it).rect.h);
		if (rect.IsInRect(point))
		{
			m_typeId = (*it).typeId;
			Close(FrOK, true);
			return;
		}
	}
	Close(FrNONE, true);
}

void FrNoticeDlg::OnNumInit(int param)
{
	m_pNumList = DYNAMIC_CAST(FrListBox, param);
}

void FrNoticeDlg::OpenImage()
{
	if (m_images.size() && m_pViewer->Open(m_images[m_curIndex].name))
	{
		if (m_pPrev)
			m_pPrev->Enable(m_curIndex > 0);
		if (m_pNext)
			m_pNext->Enable(m_curIndex < m_images.size() - 1);
		m_pViewer->SetKeyFocus(true);
	}
	else
	{
		Close(true);
	}
}

void FrNoticeDlg::OnNumLBtnUp()
{
	FrListItem* item = m_pNumList->GetItemUnderCursor();
	if (!item || !item->pData)
		return;
	m_curIndex = atoi((const char*)item->pData) - 1;
	if (m_openedIndex != m_curIndex)
	{
		m_openedIndex = m_curIndex;
		OpenImage();
		SetScrollBar();
	}
}

void FrNoticeDlg::OnNumOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item && item->pData)
	{
		const char* text = (const char*)item->pData;
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		if (gdi)
		{
			unsigned int page = atoi(text);
			if (m_curIndex == page - 1)
			{
				gdi->SetTextColor(0xffff0000, 0xffffffff);
				gdi->SetTextStyle(1);
			}
			else
			{
				gdi->SetTextColor(0xff000000, 0xffffffff);
				gdi->SetTextStyle(0);
			}
			gdi->Print(item->pos, 0, text);
		}
	}
}
