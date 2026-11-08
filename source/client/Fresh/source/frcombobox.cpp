#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "frcombobox.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "chatmsg.h"
#include "commonutil.h"

IMPLEMENT_OBJECT(FrComboBox, FrEdit)

BEGIN_FRESH_MSGMAP(FrComboBox, FrEdit)
ON_FRESH_VV("unfold", FRCMD_LBUTTONUP, FrComboBox::OnUnfold)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrComboBox::OnListOwnerDraw)
ON_FRESH_VV("list", FRCMD_LBUTTONDOWN, FrComboBox::OnListBtnDown)
ON_FRESH_VI("list", FRCMD_LOSTKEYFOCUS, FrComboBox::OnListLostKeyFocus)
END_FRESH_MSGMAP()

FrComboBox::FrComboBox()
{
	m_maxListNum = 10;
	m_pButton = NULL;
	m_pListBox = NULL;
	m_selected = m_comboList.end();
	m_selectColor = 0;
}

FrComboBox::~FrComboBox()
{
	ClearList();
}

void FrComboBox::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrEdit::Init(item, pManager, pParent);

	FrElementDoc* pDoc = pManager->GetDocument();

	m_multiLine = false;

	m_pButton = new FrButton;
	if (m_pButton)
	{
		FrGuiItem& info = m_buttonInfo;
		info = item;
		info.m_caption = "unfold";
		info.m_name = "unfold";
		info.m_rect.Reset();

		m_pButton->SetOwner(this);
		m_pButton->Init(info, pManager, this, true);
		m_pButton->UseDblClick(false);
		m_pButton->SetPushDelay(0.0f);

		WRect rect = m_pButton->GetRect();
		rect.x += m_rect.w - rect.w;
		rect.y += (int)(float(m_rect.h - rect.h + 1.0f) * 0.5f);
		m_pButton->SetRect(rect);
	}

	if (item.m_param.find("selectcolor") != item.m_param.end())
		sscanf(item.m_param["selectcolor"].c_str(), "%x", &m_selectColor);

	m_pListBox = new FrListBox;
	if (m_pListBox)
	{
		FrGuiItem& info = m_listInfo;
		info = item;
		info.m_caption = "list";
		info.m_name = "list";
		info.m_rect.right -= info.m_rect.left;
		info.m_rect.bottom -= info.m_rect.top;
		info.m_rect.left = 0;
		float height = pManager->GetDesktop()->GetRect().h;
		info.m_rect.top = (m_rect.y > height * 0.5f) ? -5 : 5;

		m_pListBox->SetItemSize(info.m_rect.right, m_lineHeight);
		m_pListBox->SetOwner(this);
		m_pListBox->Init(info, pManager, this);
		m_pListBox->SetTopmost(true);

		if (item.m_param.find("maxlistnum") != item.m_param.end())
			sscanf(item.m_param["maxlistnum"].c_str(), "%d", &m_maxListNum);

		ResizeListRect();

		m_pListBox->SetVisible(false);
		m_pListBox->SetKeyEvent(true);
		m_pListBox->EnableHover(true, 0.0f);
	}
}

void FrComboBox::IncreaseMaxListNum(int maxListNum)
{
	if (maxListNum < m_maxListNum)
		return;

	m_maxListNum = maxListNum;
	ResizeListRect();
}

void FrComboBox::ResizeListRect()
{
	if (!m_pListBox)
		return;

	int listNum = m_comboList.size();
	if (listNum > m_maxListNum)
	{
		listNum = m_maxListNum;
	}
	else if (listNum < 1)
	{
		listNum = 1;
	}

	WRect rect = m_pListBox->GetRect();
	rect.h = (float)(m_lineHeight * listNum + m_topMargin * 2);
	rect.y = (rect.y > m_rect.y) ? m_rect.y + m_rect.h + 2.0f
								 : m_rect.y - rect.h - 2.0f;

	m_pListBox->SetRect(rect);
}

void FrComboBox::AddString(const char* string)
{
	FrLine* pItem = new FrLine;
	pItem->text = string;
	LimitText(pItem->text);
	m_comboList.push_back(pItem);

	if (m_pListBox)
		m_pListBox->AddItem(pItem);

	ResizeListRect();
}

void FrComboBox::DelString(int index)
{
	for (std::list<FrLine*>::iterator it = m_comboList.begin();
		it != m_comboList.end(); ++it, --index)
	{
		if (index < 1)
		{
			if (m_pListBox)
				m_pListBox->DelItem(*it);

			if (m_selected == it)
				m_selected = m_comboList.end();

			m_comboList.erase(it);
			ResizeListRect();
			return;
		}
	}
}

int FrComboBox::FindString(const char* string)
{
	int index = 1;
	for (std::list<FrLine*>::iterator it = m_comboList.begin();
		it != m_comboList.end(); ++it, ++index)
	{
		if ((*it)->text == string)
			return index;
	}

	return 0;
}

void FrComboBox::ClearList()
{
	m_selected = m_comboList.end();
	sequence_delete(m_comboList.begin(), m_comboList.end());
	m_comboList.clear();

	if (m_pListBox)
		m_pListBox->ClearItem();

	ResizeListRect();
}

bool FrComboBox::IsSelectString(const std::string& compStr)
{
	if (m_selected == m_comboList.end())
	{
		return false;
	}

	return ((*m_selected)->text == compStr) ? true : false;
}

void FrComboBox::OnKeyFocus(CChatMsg* im)
{
	FrEdit::OnKeyFocus(im);

	if (im->GetConsolKeyCode() == 6)
	{
		if (--m_selected == m_comboList.end())
			--m_selected;

		if (m_selected != m_comboList.end())
		{
			FrLine* pLine = *m_selected;
			SetLine(1, pLine->text.c_str(), 0, false, 0);
			im->SetChatText(pLine->text.c_str(), false);
		}
	}

	if (im->GetConsolKeyCode() == 7)
	{
		if (++m_selected == m_comboList.end())
			m_selected = m_comboList.begin();

		if (m_selected != m_comboList.end())
		{
			FrLine* pLine = *m_selected;
			SetLine(1, pLine->text.c_str(), 0, false, 0);
			im->SetChatText(pLine->text.c_str(), false);
		}
	}
}

void FrComboBox::OnUnfold()
{
	if (m_pListBox)
	{
		if (m_pListBox->IsVisible())
			m_pListBox->SetVisible(false);
		else
		{
			m_pListBox->SetVisible(true);
			m_pListBox->SetKeyFocus(false);
		}
	}
}

void FrComboBox::OnListOwnerDraw(int var1)
{
	if (!var1)
	{
		const WRect& rc = m_pListBox->GetRect();
		GDI()->Box(rc, m_bgColor);
		GDI()->LineBox(rc, m_borderColor);
		return;
	}

	FrListItem* pItem = (FrListItem*)var1;
	FrLine* pLine = (FrLine*)pItem->pData;

	if (pItem->underCursor)
	{
		WRect rect = m_pListBox->GetRect();
		rect.x = pItem->pos.x;
		rect.y = pItem->pos.y + m_topMargin / 2;
		rect.w -= m_pScrBar->GetRect().w;
		rect.h = (float)m_lineHeight;

		unsigned long color = ~m_bgColor | 0xff000000;
		if (m_selectColor)
			color = m_selectColor;

		GDI()->Box(rect, color);
		GDI()->SetTextColor(m_bgColor, m_fontColor2);
		GDI()->SetTextStyle(2);
		GDI()->Print(WPoint(m_leftMargin - 2.0f, m_topMargin - 2.0f) +
				pItem->pos,
			0, pLine->text.c_str());
	}
	else
	{
		GDI()->SetTextColor(m_fontColor, m_fontColor2);
		GDI()->SetTextStyle(m_font);
		GDI()->Print(WPoint((float)m_leftMargin, (float)m_topMargin) +
				pItem->pos,
			0, pLine->text.c_str());
	}
}

void FrComboBox::OnListBtnDown()
{
	FrListItem* pItem = m_pListBox->GetItemUnderCursor();
	if (pItem)
	{
		FrLine* pLine = (FrLine*)pItem->pData;

		for (std::list<FrLine*>::iterator it = m_comboList.begin();
			it != m_comboList.end(); ++it)
		{
			if (pLine == *it)
				m_selected = it;
		}

		SetLine(1, pLine->text.c_str(), 0, false, 0);
		SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, (int)pItem, NULL);
	}

	m_pListBox->SetVisible(false);
}

void FrComboBox::OnListLostKeyFocus(int var1)
{
	WRect rcList = m_pListBox->GetRect();
	WRect rcBtn = m_pButton->GetRect();

	if (!rcList.IsInRect(((FrInputState*)var1)->mousePos) &&
		!rcBtn.IsInRect(((FrInputState*)var1)->mousePos))
		m_pListBox->SetVisible(false);
}

void FrComboBox::Enable(bool enable)
{
	FrEdit::Enable(enable);
	if (m_pButton)
		m_pButton->Enable(enable);
}

IMPLEMENT_OBJECT(FrComboCtlEx, FrWnd)

BEGIN_FRESH_MSGMAP(FrComboCtlEx, FrEdit)
ON_FRESH_VV("unfold", FRCMD_LBUTTONUP, FrComboCtlEx::OnUnfold)
ON_FRESH_VI("unfold", FRCMD_OWNERDRAW, FrComboCtlEx::OnUnfoldOwnerDraw)
ON_FRESH_VI("list", FRCMD_OWNERDRAW, FrComboCtlEx::OnListOwnerDraw)
ON_FRESH_VV("list", FRCMD_LBUTTONDOWN, FrComboCtlEx::OnListBtnDown)
ON_FRESH_VI("list", FRCMD_LOSTKEYFOCUS, FrComboCtlEx::OnListLostKeyFocus)
END_FRESH_MSGMAP()

FrComboCtlEx::FrComboCtlEx()
{
	memset(m_BitmapSkin, 0, sizeof(m_BitmapSkin));
	m_pSelectItem = NULL;
	m_eDrawStyle = IDS_TEXT;
	m_ItemOffset.x = 0;
	m_ItemOffset.y = 0;
	m_byCurrentIdx = 0;
	m_bFold = true;
}

FrComboCtlEx::~FrComboCtlEx()
{
}

void FrComboCtlEx::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrEdit::Init(item, pManager, pParent);

	FrElementDoc* pDoc = pManager->GetDocument();

	m_multiLine = false;

	if (item.m_param.find("liststyle") != item.m_param.end())
		sscanf(item.m_param["liststyle"].c_str(), "%d", &m_eDrawStyle);

	if (item.m_param.find("align") != item.m_param.end())
		sscanf(item.m_param["align"].c_str(), "%d", &m_align);

	m_BitmapSkin[NORMAL] = pDoc->GetBitmap(item.m_param["normal"]);
	m_BitmapSkin[OVER] = pDoc->GetBitmap(item.m_param["over"]);

	m_pButton = new FrButton;
	if (m_pButton)
	{
		FrGuiItem& info = m_buttonInfo;
		info = item;
		info.m_caption = "unfold";
		info.m_name = "unfold";
		info.m_rect.Reset();

		m_pButton->SetOwner(this);
		m_pButton->Init(info, pManager, this, true);
		m_pButton->UseDblClick(false);
		m_pButton->SetPushDelay(0.0f);
	}

	m_pListBox = new FrListBox;
	if (m_pListBox)
	{
		FrGuiItem& info = m_listInfo;
		info = item;
		info.m_caption = "list";
		info.m_name = "list";
		info.m_rect.right -= info.m_rect.left;
		info.m_rect.bottom -= info.m_rect.top;
		info.m_rect.left = 0;
		float height = pManager->GetDesktop()->GetRect().h;
		info.m_rect.top = (m_rect.y > height * 0.5f) ? -5 : 5;

		if (m_BitmapSkin[NORMAL])
			m_pListBox->SetItemSize(m_BitmapSkin[NORMAL]->Width(),
				m_topMargin + m_BitmapSkin[NORMAL]->Height());
		else
			m_pListBox->SetItemSize(info.m_rect.right, m_lineHeight);

		AlignListItem();

		m_pListBox->SetOwner(this);
		m_pListBox->Init(info, pManager, this);
		m_pListBox->SetTopmost(true);

		if (item.m_param.find("maxlistnum") != item.m_param.end())
			sscanf(item.m_param["maxlistnum"].c_str(), "%d", &m_maxListNum);

		if (m_eDrawStyle == IDS_IMG)
			m_TexItemList.reserve(m_maxListNum);

		m_pListBox->SetVisible(false);
		m_pListBox->SetKeyEvent(true);
		m_pListBox->EnableHover(true, 0.0f);

		WRect rt = m_pButton->GetRect();
		rt.x = m_BitmapSkin[NORMAL]->Width() + m_rect.x - rt.w;
		rt.y += (int)(float(m_rect.h - rt.h + 1.0f) * 0.5f);
		m_pButton->SetRect(rt);

		rt = m_pListBox->GetRect();
		rt.x = m_rect.x;
		rt.y = m_rect.y + m_topMargin + m_BitmapSkin[NORMAL]->Height();
		rt.w = (float)m_BitmapSkin[NORMAL]->Width();
		rt.h = (float)m_BitmapSkin[NORMAL]->Height() * m_maxListNum +
			m_maxListNum * m_topMargin;
		m_pListBox->SetRect(rt);

		m_rect.w = rt.w;
		m_rect.h = (float)m_BitmapSkin[NORMAL]->Height();
		SetRect(m_rect);
	}
}

void FrComboCtlEx::AddString(const char* string)
{
	if (m_eDrawStyle == IDS_IMG)
		return;

	FrLine* pItem = new FrLine;
	pItem->text = string;
	LimitText(pItem->text);
	m_comboList.push_back(pItem);

	if (m_pListBox)
		m_pListBox->AddItem(pItem);
}

void FrComboCtlEx::AddTexItem(const char* fileName)
{
	if (m_eDrawStyle == IDS_TEXT)
		return;

	FrElementDoc* pDoc = WndManager()->GetDocument();
	const Bitmap* pTex = pDoc->GetBitmap(fileName);
	if (pTex)
		m_TexItemList.push_back(pTex);

	FrLine* pItem = new FrLine;
	char buf[8];
	sprintf(buf, "%d", m_comboList.size() + 1);
	pItem->text = buf;
	m_comboList.push_back(pItem);

	if (m_pListBox)
		m_pListBox->AddItem(pItem);
}

void FrComboCtlEx::SelectItem(int idx)
{
	int i = 0;
	std::vector<const Bitmap*>::iterator it = m_TexItemList.begin();
	for (; it != m_TexItemList.end(); ++it)
	{
		if (i++ == idx)
		{
			m_pSelectItem = *it;
			m_byCurrentIdx = (unsigned char)idx;
			break;
		}
	}

	if (it == m_TexItemList.end())
	{
		m_pSelectItem = NULL;
		m_byCurrentIdx = 0;
	}
}

void FrComboCtlEx::ClearAllListItem()
{
	m_pListBox->ClearItem();
	m_TexItemList.clear();
}

void FrComboCtlEx::AlignListItem()
{
	switch (m_align)
	{
	case leftAlign:
		m_ItemOffset.x = (float)m_leftMargin;
		break;
	case centerAlign:
		m_ItemOffset.x = m_pListBox->GetItemWidth() * 0.5f + m_leftMargin;
		break;
	case rightAlign:
		m_ItemOffset.x = (float)(m_pListBox->GetItemWidth() - m_leftMargin);
		break;
	default:
		return;
	}

	m_ItemOffset.y = (float)m_topMargin;
}

void FrComboCtlEx::OnListOwnerDraw(int var1)
{
	if (!var1)
		return;

	FrListItem* pItem = (FrListItem*)var1;
	FrLine* pLine = (FrLine*)pItem->pData;

	GDI()->DrawTexture(m_BitmapSkin[NORMAL],
		WRect(pItem->pos.x, m_ItemOffset.y + pItem->pos.y,
			m_BitmapSkin[NORMAL]->Width(), m_BitmapSkin[NORMAL]->Height()),
		0xffffffff, 0);

	if (pItem->underCursor)
	{
		GDI()->DrawTexture(m_BitmapSkin[OVER],
			WRect(pItem->pos.x, m_ItemOffset.y + pItem->pos.y,
				m_BitmapSkin[OVER]->Width(), m_BitmapSkin[OVER]->Height()),
			0xffffffff, 0);
	}

	GDI()->SetTextColor(m_fontColor, m_fontColor2);
	GDI()->SetTextStyle(m_font);

	if (m_eDrawStyle == IDS_TEXT)
	{
		pItem->pos.x += 0.5f;
		pItem->pos.y += 0.5f;
		GDI()->Print(WPoint(pItem->pos.x, pItem->pos.y), 0,
			pLine->text.c_str());
		return;
	}

	const Bitmap* pTex = NULL;
	if (m_TexItemList.size())
		pTex = m_TexItemList[pItem->idx];

	if (pTex)
	{
		pItem->pos.x -= pTex->Width() / 2;
		GDI()->DrawTexture(pTex,
			WRect(pItem->pos.x + m_ItemOffset.x,
				m_ItemOffset.y + pItem->pos.y + 2.0f, pTex->Width(),
				pTex->Height()),
			0xffffffff, 0);
	}
}

void FrComboCtlEx::OnListBtnDown()
{
	FrListItem* pItem = m_pListBox->GetItemUnderCursor();
	if (pItem)
	{
		if (m_eDrawStyle == IDS_IMG)
			SelectItem(pItem->idx);

		FrLine* pLine = (FrLine*)pItem->pData;

		for (std::list<FrLine*>::iterator it = m_comboList.begin();
			it != m_comboList.end(); ++it)
		{
			if (pLine == *it)
				m_selected = it;
		}

		SetLine(1, pLine->text.c_str(), 0, false, 0);
		SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, (int)pItem, NULL);
		m_bFold = true;
	}

	m_pListBox->SetVisible(false);
}

void FrComboCtlEx::OnKeyFocus(CChatMsg* im)
{
	FrEdit::OnKeyFocus(im);

	if (im->GetConsolKeyCode() == 6)
	{
		if (--m_selected == m_comboList.end())
			--m_selected;

		if (m_selected != m_comboList.end())
		{
			FrLine* pLine = *m_selected;
			SetLine(1, pLine->text.c_str(), 0, false, 0);
			im->SetChatText(pLine->text.c_str(), false);
		}
	}

	if (im->GetConsolKeyCode() == 7)
	{
		if (++m_selected == m_comboList.end())
			m_selected = m_comboList.begin();

		if (m_selected != m_comboList.end())
		{
			FrLine* pLine = *m_selected;
			SetLine(1, pLine->text.c_str(), 0, false, 0);
			im->SetChatText(pLine->text.c_str(), false);
		}
	}
}

void FrComboCtlEx::OnListLostKeyFocus(int var1)
{
	WRect rcList = m_pListBox->GetRect();
	WRect rcBtn = m_pButton->GetRect();

	if (!rcList.IsInRect(((FrInputState*)var1)->mousePos) &&
		!rcBtn.IsInRect(((FrInputState*)var1)->mousePos))
	{
		m_pListBox->SetVisible(false);
		m_bFold = true;
	}
}

void FrComboCtlEx::OnUnfold()
{
	if (m_pListBox)
	{
		if (m_pListBox->IsVisible())
		{
			m_pListBox->SetVisible(false);
			m_bFold = true;
		}
		else
		{
			m_pListBox->SetVisible(true);
			m_pListBox->SetKeyFocus(false);
			m_bFold = false;
		}
	}
}

void FrComboCtlEx::OnUnfoldOwnerDraw(int var1)
{
	if (m_eDrawStyle == IDS_IMG)
	{
		if (m_pSelectItem)
		{
			WRect rc = m_pButton->GetRect();
			rc.x -= m_pSelectItem->Width() / 2;
			GDI()->DrawTexture(m_pSelectItem,
				WRect(rc.x + m_ItemOffset.x, rc.y + 2.0f,
					m_pSelectItem->Width(), m_pSelectItem->Height()),
				0xffffffff, 0);
		}
	}
}
