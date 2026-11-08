#include "stdafx.h"
#include "ccardbook.h"
#include "carddlg.h"
#include "taskmanager.h"
#include "frarea.h"
#include "frlistbox.h"
#include "wresrcmng.h"
extern Fresh* g_pFresh;

namespace ILFILL_CARDBOOK
{
	struct Pad
	{
		int v;
		void g() { ILFILL64 ILFILL64 ILFILL64 ILFILL16 ILFILL16 }
	};
}

CCardSlot::CCardSlot()
{
	m_pushed = 0;
	m_rect = WRect(0.f, 0.f, 0.f, 0.f);
	m_pButton = NULL;
	m_pCard = NULL;
	m_pItem = NULL;
	m_pCards = NULL;
	m_pOverlay = NULL;
	m_pCardPackDlg = NULL;
	m_pItemAttachCardDlg = NULL;
	m_pSpCardDlg = NULL;
}

CCardSlot::~CCardSlot()
{
	if (m_pOverlay && g_resrcmng)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}
}

void CCardSlot::SetCard(unsigned long typeId)
{
	if (typeId)
	{
		m_pCards = NULL;
		for (CARD_LIST_IT it = Doc()->m_cardList.begin();
			it != Doc()->m_cardList.end(); ++it)
		{
			m_pCard = ItemManager()->FindCard(typeId);
			m_pItem = ItemManager()->FindCommonItem(typeId);
			m_pCards = &*it;
			if (m_pCards)
			{
				if (m_pCards->tid == typeId)
					break;
				m_pCard = NULL;
				m_pItem = NULL;
				m_pCards = NULL;
			}
		}
	}
	else
	{
		m_pCard = NULL;
		m_pItem = NULL;
		m_pCards = NULL;
	}
	if (m_pButton && typeId)
	{
		if (m_pCard && m_pItem)
		{
			m_pButton->SetVisible(true);
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::NORMAL);
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::OVER);
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::PRESSED);
		}
	}
	else
	{
		if (m_pButton && !typeId)
			m_pButton->SetVisible(false);
	}
	m_pushed = 0;
	if (m_pOverlay && g_resrcmng)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}
}

void CCardSlot::SetPosition(WRect& rect)
{
	m_rect = rect;
}

void CCardSlot::Select(unsigned char state)
{
	m_pushed = state;
}

void CCardSlot::UseCard()
{
	if (!m_pCard || !m_pItem || !m_pCards)
		return;
	switch ((m_pCards->tid >> 22) & 15)
	{
	case 3:
		m_pCardPackDlg = CreateForm<FrOpenCardPackDlg>(g_pFresh->GetManager(),
			this, "cardpack_open");
		m_pCardPackDlg->m_cardId = m_pCards->uid;
		m_pCardPackDlg->m_cardTypeId = m_pCards->tid;
		m_pCardPackDlg->Open((FRESH_PFN_RESULT)&CCardSlot::OnCardPackDlgResult,
			1);
		break;
	case 2:
		m_pSpCardDlg =
			CreateForm<FrSpCardUseDlg>(g_pFresh->GetManager(), this, "usecard");
		m_pSpCardDlg->SetSpCardInfo(m_pCards->tid);
		m_pSpCardDlg->Open((FRESH_PFN_RESULT)&CCardSlot::OnSpCardDlgResult, 1);
		break;
	default:
		m_pCardDlg =
			CreateForm<FrCardUseDlg>(g_pFresh->GetManager(), this, "usecard");
		m_pCardDlg->m_cardId = m_pCards->uid;
		m_pCardDlg->m_cardTypeId = m_pCards->tid;
		m_pCardDlg->Open((FRESH_PFN_RESULT)&CCardSlot::OnCardDlgResult, 1);
		break;
	}
}

bool CCardSlot::OnCardPackDlgResult(int ret, FrForm* pForm)
{
	if (ret == 1)
	{
		FrOpenCardPackDlg* dlg = (FrOpenCardPackDlg*)pForm;
		if (dlg && ((dlg->m_cardTypeId >> 22) & 15) == 3)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			WSendPacket p((enumClientPacket)194);
			p.Encode4(dlg->m_cardTypeId);
			p.Encode4(dlg->m_cardId);
			p.Send(TO_GAME);
		}
	}
	m_pCardPackDlg = NULL;
	return true;
}

bool CCardSlot::OnSpCardDlgResult(int ret, FrForm* pForm)
{
	if (ret == 1)
	{
		FrSpCardUseDlg* dlg = (FrSpCardUseDlg*)pForm;
		if (dlg)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			WSendPacket p((enumClientPacket)181);
			p.Encode4(dlg->GetSpCardTid());
			p.Send(TO_GAME);
		}
	}
	m_pSpCardDlg = NULL;
	return true;
}

bool CCardSlot::OnCardDlgResult(int ret, FrForm* pForm)
{
	if (ret == 1 && pForm)
	{
		FrCardUseDlg* dlg = (FrCardUseDlg*)pForm;
		unsigned long type = (dlg->m_cardTypeId >> 22) & 15;
		if (type == 2)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			WSendPacket p((enumClientPacket)181);
			p.Encode4(dlg->m_cardTypeId);
			p.Send(TO_GAME);
		}
		else if (type == 0 || type == 1)
		{
			m_pItemAttachCardDlg = CreateForm<FrItemAttachCardDlg>(
				g_pFresh->GetManager(), this, "itemattachcarddlg");
			m_pItemAttachCardDlg->m_cardId = dlg->m_cardId;
			m_pItemAttachCardDlg->m_cardTypeId = dlg->m_cardTypeId;
			m_pItemAttachCardDlg->m_cardType = (dlg->m_cardTypeId >> 22) & 15;
			m_pItemAttachCardDlg->Open(
				(FRESH_PFN_RESULT)&CCardSlot::OnItemAttachCardDlgResult, 1);
		}
	}
	m_pCardDlg = NULL;
	return true;
}

bool CCardSlot::OnItemAttachCardDlgResult(int ret, FrForm* pForm)
{
	if (pForm && ret == 1)
	{
		FrItemAttachCardDlg* dlg = (FrItemAttachCardDlg*)pForm;
		if (dlg->m_selectSlot == 255)
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
				(int)"\306\304\303\367\300\307 \275\275\267\324\300\314 \274\261\305\303\265\307\301\366 \276\312\276\322\275\300\264\317\264\331.",
				0, 0, 0, 0));
			return true;
		}
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(dlg->m_itemTypeId);
		if (part != NULL)
		{
			if (ItemManager()->IsPartAttachCard(dlg->m_cardTypeId,
					dlg->m_itemTypeId, dlg->m_selectSlot))
			{
				sCards card;
				card.tid = dlg->m_cardTypeId;
				card.uid = dlg->m_cardId;
				card.partTid = dlg->m_itemTypeId;
				card.partUid = dlg->m_itemId;
				card.selectedCardSlotNum = dlg->m_selectSlot;
				WSendPacket p((enumClientPacket)192);
				p.EncodeBuffer(&card, sizeof(card));
				p.Send(TO_GAME);
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			}
			else
			{
				AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
					(int)"\275\275\267\324\260\372 \304\253\265\345\300\307 \305\270\300\324\300\314 \264\336\266\363\274\255 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.",
					0, 0, 0, 0));
			}
		}
	}
	return true;
}

void CCardSlot::Draw(FrGraphicInterface* pGI)
{
	if (m_pushed)
	{
		const Bitmap* bmp = NULL;
		if (m_pushed == 1)
			bmp = g_pFresh->GetBitmap("select_card_d");
		else if (m_pushed == 2)
			bmp = g_pFresh->GetBitmap("select_card_0");
		if (bmp)
			pGI->DrawTexture(bmp,
				WRect(m_rect.x, m_rect.y, bmp->Width(), bmp->Height()),
				0xffffffff, 0);
	}
	if (m_pushed == 1)
	{
		if (m_pItem)
		{
			unsigned long tid = m_pItem->TypeId;
			if (tid)
			{
				if (ItemManager()->FindCard(tid))
				{
					if (!m_pOverlay)
					{
						IFF_STRUCT::sCard* card = ItemManager()->FindCard(tid);
						if (card)
						{
							char filename[64];
							sprintf(filename, "%s.png", card->Image);
							m_pOverlay = g_resrcmng->GetOverlay(filename, 0);
						}
					}
					if (m_pOverlay)
						m_pOverlay->Render(g_view, WRect(0, 0, 1, 1),
							WRect(92, 117, 248, 499), 0, 0xffffffff, 0, 0);
				}
			}
		}
		const Bitmap* help[2] = { g_pFresh->GetBitmap("help_1"),
			g_pFresh->GetBitmap("help_2") };
		if (help[0])
		{
			WRect helpRect(12, 439, help[0]->Width(), help[0]->Height());
			pGI->DrawTexture(help[0], helpRect, 0xffffffff, 0);
		}
		if (help[1])
		{
			WRect helpRect(268, 439, help[1]->Width(), help[1]->Height());
			pGI->DrawTexture(help[1], helpRect, 0xffffffff, 0);
		}
		if (m_pCards)
		{
			IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_pCards->tid);
			pGI->SetTextColor(0xff887966, 0xffffffff);
			if (m_pItem)
				pGI->Print(WPoint(120, 458), 0, m_pItem->Name);
			if (desc)
			{
				char lines[3][128] = { "\0" };
				unsigned long len = strlen(desc->Desc);
				unsigned long line = 0, column = 0;
				char* text = new char[len + 1];
				memset(text, 0, len + 1);
				if (text)
				{
					strcpy(text, desc->Desc);
					for (unsigned long i = 0; i < len; ++i)
					{
						lines[line][column] = desc->Desc[i];
						column++;
						if (text[i] == '\n' || column >= 41 && text[i] == ' ')
						{
							lines[line][column - 1] = 0;
							line++;
							column = 0;
						}
						if (line >= 3)
							break;
					}
					pGI->Print(WPoint(120, 474), 0, lines[0]);
					pGI->Print(WPoint(72, 486), 0, lines[1]);
					pGI->Print(WPoint(72, 498), 0, lines[2]);
					delete[] text;
				}
			}
			pGI->SetTextColor(0xff000000, 0xffffffff);
		}
	}
}

bool CCardSlot::BoundControl(FrButton* pButton)
{
	m_pButton = pButton;
	if (m_pButton)
	{
		if (m_pItem)
		{
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::NORMAL);
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::OVER);
			m_pButton->SetButtonImg(m_pItem->Icon, FrButton::PRESSED);
		}
		m_pButton->SetVisible(false);
		m_pButton->SetPushDelay(.1f);
		m_pButton->UseDblClick(true);
	}
	return m_pButton ? true : false;
}

bool CCardPackPage::Init()
{
	m_pSlot = new CCardSlot[8];
	struct SlotInfo
	{
		unsigned long tid;
		float x, y, w, h;
	};
	SlotInfo slots[8] = {
		{ 2092957696, 439, 247 },
        { 2092957700, 521, 247 },
        { 0,          603, 247 },
		{ 0,          685, 247 },
        { 2092957697, 439, 377 },
        { 2092957698, 521, 377 },
		{ 2092957699, 603, 377 },
        { 0,          685, 377 }
	};
	if (!m_pCardPackList)
	{
		m_pCardPackList = new std::vector<unsigned long>;
		if (m_pCardPackList)
			m_pCardPackList->clear();
	}
	for (int i = 0; i < 8; i++)
	{
		if (m_pCardPackList)
			m_pCardPackList->push_back(slots[i].tid);
		m_pSlot[i].SetCard(0);
		WRect rect(slots[i].x, slots[i].y, 0, 0);
		m_pSlot[i].SetPosition(rect);
		m_pSlot[i].Select(0);
	}
	return false;
}

void CCardPackPage::Draw(FrGraphicInterface* pGI)
{
	for (int i = 0; i < 8; i++)
	{
		if (m_pSlot)
		{
			m_pSlot[i].Draw(pGI);
			sCards* card = m_pSlot[i].GetCards();
			if (card && m_pFont)
			{
				WRect rect = m_pSlot[i].GetPosition();
				m_pFont->Print(g_view, rect.x + 42, rect.y + 50,
					MakeStr("%d", card->cardCount), 0, 0xffffffff, 0);
			}
		}
	}
}

void CCardPackPage::SetCard(unsigned char slot, unsigned long typeId)
{
	if (m_pSlot)
		m_pSlot[slot].SetCard(typeId);
}

void CCardPackPage::ShowSlotControl(bool bShow)
{
	for (int i = 0; i < 8; i++)
		if (m_pSlot && m_pSlot[i].GetCardInfo())
			m_pSlot[i].Show(bShow);
}

void CCardPackPage::SelectSlot(unsigned char action, unsigned char slot)
{
	if (!m_pSlot)
		return;
	unsigned char use = 0;
	if (action == 2)
	{
		action = 1;
		use = 1;
	}
	for (int i = 0; i < 8; i++)
	{
		if (i == slot)
		{
			if (action != 1)
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(action);
			}
			else
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(1);
			}
		}
		else
		{
			if (action != 1)
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(action);
			}
			else
				m_pSlot[i].Select(0);
		}
	}
	if (use == true)
	{
		if (m_pSlot)
			m_pSlot[slot].UseCard();
	}
}

CCardPage::CCardPage()
{
	m_type = 0;
	m_pCardList1 = NULL;
	m_pCardList2 = NULL;
	m_pCardList3 = NULL;
	m_pCardList4 = NULL;
	m_pSlotBg = NULL;
}

CCardPage::~CCardPage()
{
	if (m_pCardList1)
	{
		delete[] m_pCardList1;
		m_pCardList1 = NULL;
	}
	if (m_pCardList2)
	{
		delete[] m_pCardList2;
		m_pCardList2 = NULL;
	}
	if (m_pCardList3)
	{
		delete[] m_pCardList3;
		m_pCardList3 = NULL;
	}
	if (m_pCardList4)
	{
		delete[] m_pCardList4;
		m_pCardList4 = NULL;
	}
}

bool CCardPage::Init()
{
	BuildCardList();
	m_pSlot = new CCardSlot[12];
	WRect slots[12];
	slots[0] = WRect(426, 216, 0, 0);
	slots[1] = WRect(517, 216, 0, 0);
	slots[2] = WRect(607, 216, 0, 0);
	slots[3] = WRect(698, 216, 0, 0);
	slots[4] = WRect(426, 298, 0, 0);
	slots[5] = WRect(517, 298, 0, 0);
	slots[6] = WRect(607, 298, 0, 0);
	slots[7] = WRect(698, 298, 0, 0);
	slots[8] = WRect(426, 380, 0, 0);
	slots[9] = WRect(517, 380, 0, 0);
	slots[10] = WRect(607, 380, 0, 0);
	slots[11] = WRect(698, 380, 0, 0);
	for (int i = 0; i < 12; i++)
	{
		m_pSlot[i].SetCard(0);
		m_pSlot[i].SetPosition(slots[i]);
		m_pSlot[i].Select(0);
	}
	m_pSlotBg = g_pFresh->GetBitmap("collection_bg");
	return false;
}

void CCardPage::Draw(FrGraphicInterface* pGI)
{
	unsigned long total = 0;
	unsigned long pages = GetPageCount(m_type);
	for (unsigned long i = 0; i < pages; i++)
	{
		std::vector<unsigned long>* list = GetCardList(m_type);
		if (!list)
			break;
		total += list[i].size();
	}
	for (int i = 0; i < 12; i++)
	{
		if (m_pSlot && total > i + 12 * (unsigned long)m_page)
		{
			if (m_pSlotBg)
			{
				WRect rect = m_pSlot[i].GetPosition();
				pGI->DrawTexture(m_pSlotBg,
					WRect(rect.x, rect.y, m_pSlotBg->Width(),
						m_pSlotBg->Height()),
					0xffffffff, 0);
			}
			m_pSlot[i].Draw(pGI);
			sCards* card = m_pSlot[i].GetCards();
			if (card && m_pFont)
			{
				WRect rect = m_pSlot[i].GetPosition();
				m_pFont->Print(g_view, rect.x + 42, rect.y + 50,
					MakeStr("%d", card->cardCount), 0, 0xffffffff, 0);
			}
		}
	}
}

void CCardPage::PageSeparate(std::vector<unsigned long>*& pList,
	unsigned char type)
{
	if (type < 1 || type > 4)
		return;
	unsigned long count = 0, page = 0, index = 0;
	for (CardIterator it = ItemManager()->m_CardMap.begin();
		it != ItemManager()->m_CardMap.end(); ++it)
	{
		IFF_STRUCT::sCard card = (*it).second;
		IFF_STRUCT::sCard* found = ItemManager()->FindCard((*it).first);
		if (found && card.Volume == type && ((card.c.TypeId >> 22) & 15) < 3)
			++count;
	}
	if (count > 0)
	{
		unsigned long pages = count / 12;
		if (count % 12)
			++pages;
		pList = new std::vector<unsigned long>[pages];
		for (CardIterator it = ItemManager()->m_CardMap.begin();
			it != ItemManager()->m_CardMap.end(); ++it)
		{
			IFF_STRUCT::sCard card = (*it).second;
			IFF_STRUCT::sCard* found = ItemManager()->FindCard((*it).first);
			unsigned long tid = (*it).first;
			if (card.Volume == type && ((card.c.TypeId >> 22) & 15) < 3)
			{
				pList[page].push_back(tid);
				++index;
			}
			if (index == 12)
			{
				++page;
				index = 0;
				if (pages < page)
					break;
			}
		}
	}
}

void CCardPage::BuildCardList()
{
	PageSeparate(m_pCardList1, 1);
	PageSeparate(m_pCardList2, 2);
	PageSeparate(m_pCardList3, 3);
	PageSeparate(m_pCardList4, 4);
}

std::vector<unsigned long>* CCardPage::GetCardList(unsigned char type)
{
	std::vector<unsigned long>* result = NULL;
	switch (type)
	{
	case 1:
		result = m_pCardList1;
		break;
	case 2:
		result = m_pCardList2;
		break;
	case 3:
		result = m_pCardList3;
		break;
	case 4:
		result = m_pCardList4;
		break;
	}
	return result;
}

unsigned long CCardPage::GetPageCount(unsigned char type)
{
	unsigned long count = 0;
	for (CardIterator it = ItemManager()->m_CardMap.begin();
		it != ItemManager()->m_CardMap.end(); ++it)
	{
		IFF_STRUCT::sCard card = (*it).second;
		IFF_STRUCT::sCard* found = ItemManager()->FindCard((*it).first);
		if (found && card.Volume == type && ((card.c.TypeId >> 22) & 15) < 3)
			++count;
	}
	unsigned long pages = count / 12;
	if (count % 12)
		++pages;
	return pages;
}

void CCardPage::SetCard(unsigned char slot, unsigned long typeId)
{
	if (m_pSlot)
		m_pSlot[slot].SetCard(typeId);
}

void CCardPage::ShowSlotControl(bool bShow)
{
	for (int i = 0; i < 12; i++)
		if (m_pSlot)
			m_pSlot[i].Show(bShow);
}

void CCardPage::SelectSlot(unsigned char action, unsigned char slot)
{
	if (!m_pSlot)
		return;
	unsigned char use = 0;
	if (action == 2)
	{
		action = 1;
		use = 1;
	}
	for (int i = 0; i < 12; i++)
	{
		if (i == slot)
		{
			if (action != 1)
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(action);
			}
			else
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(1);
			}
		}
		else
		{
			if (action != 1)
			{
				if (m_pSlot[i].IsPushed() != 1)
					m_pSlot[i].Select(action);
			}
			else
				m_pSlot[i].Select(0);
		}
	}
	if (use == true)
	{
		if (m_pSlot && m_pSlot[slot].GetCards()->tid != 2088763430)
			m_pSlot[slot].UseCard();
	}
}

CCardBook::CCardBook()
{
	ClearVariables();
	LoadImages();
	m_pPage[0] = new CCardPackPage;
	m_pPage[1] = new CCardPage;
	m_pPage[0]->Init();
	m_pPage[1]->Init();
}

CCardBook::~CCardBook()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_pPage[i])
		{
			delete m_pPage[i];
			m_pPage[i] = NULL;
		}
	}
}

void CCardBook::ClearVariables()
{
	m_bShow = false;
	m_bShowProperty = false;
	m_tab = 0;
	m_pageNum = 0;
	m_curPage = 0;
	m_overPage = 0;
	m_selectedSlot = 0;
	m_cardPackList.clear();
	m_pCurCardList = NULL;
	m_pageIndexRect.clear();
	m_pCardBookArea = NULL;
	m_pPropertyArea = NULL;
	m_pIndexList = NULL;
	for (int i = 0; i < 5; i++)
		m_pTab[i] = NULL;
	m_pUseBtn = NULL;
	for (int i = 0; i < 4; i++)
	{
		m_pCardBookBg[i] = NULL;
		m_pCardPackBg[i] = NULL;
		m_pCollectionBg[i] = NULL;
	}
	for (int i = 0; i < 2; i++)
		m_pPage[i] = NULL;
}

void CCardBook::LoadImages()
{
	m_pCardBookBg[0] = g_pFresh->GetBitmap("cardbook_bg1");
	m_pCardBookBg[1] = g_pFresh->GetBitmap("cardbook_bg2");
	m_pCardBookBg[2] = g_pFresh->GetBitmap("cardbook_bg3");
	m_pCardBookBg[3] = g_pFresh->GetBitmap("cardbook_bg4");
	m_pCardPackBg[0] = g_pFresh->GetBitmap("cardpack1");
	m_pCardPackBg[1] = g_pFresh->GetBitmap("cardpack2");
	m_pCardPackBg[2] = g_pFresh->GetBitmap("cardpack3");
	m_pCardPackBg[3] = g_pFresh->GetBitmap("cardpack4");
	m_pCollectionBg[0] = g_pFresh->GetBitmap("collection1");
	m_pCollectionBg[1] = g_pFresh->GetBitmap("collection2");
	m_pCollectionBg[2] = g_pFresh->GetBitmap("collection3");
	m_pCollectionBg[3] = g_pFresh->GetBitmap("collection4");
}

void CCardBook::ShowCardProperty(bool bShow)
{
	m_bShowProperty = bShow;
}

void CCardBook::DrawBackGround(FrGraphicInterface* pGI)
{
	if (m_pCardBookArea && m_pCardBookArea->IsVisible())
	{
		WPoint pos[4];
		pos[0] = WPoint(379, 147);
		pos[1] = WPoint(635, 147);
		pos[2] = WPoint(379, 403);
		pos[3] = WPoint(635, 403);
		for (int i = 0; i < 4; i++)
			pGI->DrawTexture(m_pCardBookBg[i],
				WRect(pos[i].x, pos[i].y, m_pCardBookBg[i]->Width(),
					m_pCardBookBg[i]->Height()),
				0xffffffff, 0);
	}
}

void CCardBook::DrawCard(FrGraphicInterface* pGI)
{
	if (m_pPage[1])
	{
		WPoint pos[4];
		pos[0] = WPoint(412, 197);
		pos[1] = WPoint(668, 197);
		pos[2] = WPoint(412, 453);
		pos[3] = WPoint(668, 453);
		for (int i = 0; i < 4; i++)
			pGI->DrawTexture(m_pCollectionBg[i],
				WRect(pos[i].x, pos[i].y, m_pCollectionBg[i]->Width(),
					m_pCollectionBg[i]->Height()),
				0xffffffff, 0);
		CCardPage* page = GetCardPage();
		CCardSlot* slots = page->GetCardSlot();
		if (slots)
		{
			std::vector<unsigned long>* list = page->GetCardList(m_tab);
			unsigned long total = 0;
			if (list)
			{
				unsigned long pages = page->GetPageCount(m_tab);
				for (unsigned long i = 0; i < pages; i++)
					total += list[i].size();
			}
			for (int i = 0; i < 12; i++)
			{
				unsigned long index = i + 12 * m_curPage + 1;
				if (index > total)
					break;
				WRect rect = slots[i].GetPosition();
				pGI->SetTextColor(0xff887966, 0xffffffff);
				pGI->SetTextStyle(1);
				pGI->Print(WPoint(rect.x - 2, rect.y + 1), 2, "%d", index);
				pGI->SetTextColor(0xff000000, 0xffffffff);
				pGI->SetTextStyle(0);
			}
		}
		m_pPage[1]->Draw(pGI);
		DrawPageIndex(pGI);
	}
}

void CCardBook::DrawCardPack(FrGraphicInterface* pGI)
{
	if (m_pPage[0])
	{
		WPoint pos[4];
		pos[0] = WPoint(412, 197);
		pos[1] = WPoint(668, 197);
		pos[2] = WPoint(412, 453);
		pos[3] = WPoint(668, 453);
		for (int i = 0; i < 4; i++)
			pGI->DrawTexture(m_pCardPackBg[i],
				WRect(pos[i].x, pos[i].y, m_pCardPackBg[i]->Width(),
					m_pCardPackBg[i]->Height()),
				0xffffffff, 0);
		m_pPage[0]->Draw(pGI);
	}
}

void CCardBook::DrawProperty(FrGraphicInterface* pGI)
{
	WRect rect(0, 0, 0, 0);
}

void CCardBook::DrawPageIndex(FrGraphicInterface* pGI)
{
	if (m_pageIndexRect.size())
	{
		for (unsigned long i = 0; i < m_pageIndexRect.size(); i++)
		{
			WRect rect = m_pageIndexRect[i];
			if (i == m_curPage)
			{
				pGI->SetTextColor(0xffff0000, 0xffffffff);
				pGI->SetTextStyle(1);
			}
			else if (i == m_overPage)
			{
				pGI->SetTextColor(0xffaa1111, 0xffffffff);
				pGI->SetTextStyle(1);
			}
			else
			{
				pGI->SetTextColor(0xff000000, 0xffffffff);
				pGI->SetTextStyle(0);
			}
			pGI->Print(WPoint((rect.w - rect.x) * .5f + rect.x, rect.y), 1,
				"%d", i + 1);
			pGI->SetTextColor(0xff000000, 0xffffffff);
			pGI->SetTextStyle(0);
			if (i + 1 < m_pageIndexRect.size())
				pGI->Print(WPoint(rect.w, rect.y), 1, "|");
		}
	}
}

void CCardBook::Show(bool bShow)
{
	m_bShow = bShow;
	if (m_pCardBookArea)
	{
		m_pCardBookArea->SetVisible(bShow);
		for (int i = 0; i < 3; i++)
			if (m_pTab[i])
				m_pTab[i]->SetVisible(m_bShow);
		if (m_bShow == true)
			ButtonTabControl(0);
	}
	ShowCardProperty(bShow);
	if (m_pPropertyArea)
		m_pPropertyArea->SetVisible(bShow);
	ShowSlotControl(m_bShow);
	if (!bShow)
		m_pUseBtn->SetVisible(false);
	if (m_pIndexList)
		m_pIndexList->SetVisible(bShow);
}

void CCardBook::Draw()
{
	if (m_bShow)
	{
		FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
		if (pGI)
		{
			DrawBackGround(pGI);
			if (m_tab)
				DrawCard(pGI);
		}
	}
}

void CCardBook::ReBuildCurrentPage()
{
	ICardBookPage* page = NULL;
	if (m_tab == 0)
	{
		BuildCardPackTab();
		page = m_pPage[0];
	}
	else
	{
		BuildCardPage(m_curPage);
		page = m_pPage[1];
	}
	if (page && m_pUseBtn)
		m_pUseBtn->SetVisible(false);
}

void CCardBook::BuildCardPackTab()
{
	CCardPackPage* page = GetCardPackPage();
	if (page)
	{
		std::vector<unsigned long>* list = page->GetCardPackList();
		if (list)
		{
			int slot = 0;
			for (std::vector<unsigned long>::iterator it = list->begin();
				it != list->end(); ++it)
			{
				unsigned long tid = *it;
				for (CARD_LIST_IT card = Doc()->m_cardList.begin();
					card != Doc()->m_cardList.end(); ++card)
				{
					if ((*card).tid == tid)
					{
						page->SetCard(slot, (*card).tid);
						break;
					}
					else
						page->SetCard(slot, 0);
				}
				if (++slot == 8)
					break;
			}
			if (m_pUseBtn)
				m_pUseBtn->SetVisible(false);
		}
	}
}

void CCardBook::BuildCardTab(unsigned long type)
{
	if (!type)
		return;
	CCardPage* page = GetCardPage();
	m_pageIndexRect.clear();
	if (page)
	{
		page->m_type = type;
		m_pCurCardList = page->GetCardList(type);
		if (m_pCurCardList)
		{
			unsigned long pages = page->GetPageCount(type);
			m_pageNum = pages;
			m_curPage = 0;
			BuildCardPage(0);
			float left = 586.f - ((pages * 20.f) * .5f);
			for (unsigned long i = 0; i < pages; i++)
			{
				WRect rect;
				float width = i * 20.f;
				width += left;
				width += 20.f;
				rect = WRect(i * 20.f + left, 492.f, width, 502.f);
				m_pageIndexRect.push_back(rect);
			}
		}
		else
			BuildCardPage(255);
		if (m_pUseBtn)
			m_pUseBtn->SetVisible(false);
	}
}

void CCardBook::BuildCardPage(unsigned long index)
{
	if (index != 255)
	{
		if (!m_pCurCardList)
			return;
		CCardPage* page = GetCardPage();
		if (page->GetPageCount(m_tab) - 1 < index)
			return;
		m_curPage = index;
		if (page)
		{
			page->m_page = index;
			int slot = 0;
			for (int i = 0; i < 12; i++)
				page->SetCard(i, 0);
			for (std::vector<unsigned long>::iterator it =
					 m_pCurCardList[index].begin();
				it != m_pCurCardList[index].end(); ++it)
			{
				unsigned long tid = *it;
				for (CARD_LIST_IT card = Doc()->m_cardList.begin();
					card != Doc()->m_cardList.end(); ++card)
				{
					if ((*card).tid == tid)
					{
						page->SetCard(slot, (*card).tid);
						break;
					}
					else
						page->SetCard(slot, 0);
				}
				++slot;
			}
		}
	}
	else
	{
		CCardPage* page = GetCardPage();
		if (page)
			for (int i = 0; i < 12; i++)
				page->SetCard(i, 0);
	}
	if (m_pUseBtn)
		m_pUseBtn->SetVisible(false);
}

void CCardBook::ShowSlotControl(bool bShow)
{
	ICardBookPage* pack = m_pPage[0];
	ICardBookPage* page = m_pPage[1];
	if (bShow == true)
	{
		if (m_tab == 0)
		{
			if (pack)
				pack->ShowSlotControl(true);
			if (page)
				page->ShowSlotControl(false);
		}
		else
		{
			if (pack)
				pack->ShowSlotControl(false);
			if (page)
				page->ShowSlotControl(true);
		}
	}
	else
	{
		if (pack)
			pack->ShowSlotControl(false);
		if (page)
			page->ShowSlotControl(false);
	}
}

void CCardBook::UseCard()
{
	ICardBookPage* page = NULL;
	if (m_tab == 0)
		page = m_pPage[0];
	else
		page = m_pPage[1];
	if (page)
		page->SelectSlot(2, m_selectedSlot);
}

void CCardBook::ButtonTabControl(unsigned char tab)
{
	m_tab = tab;
	if (tab)
		BuildCardTab(tab);
	else
		BuildCardPackTab();
	for (int i = 0; i < 3; i++)
	{
		if (!m_pTab[i])
			break;
		if (i == 0)
		{
			if (tab == 0)
			{
				m_pTab[i]->SetButtonImg("tab_pack_d", FrButton::NORMAL);
				m_pTab[i]->SetButtonImg("tab_pack_d", FrButton::OVER);
			}
			else
			{
				m_pTab[i]->SetButtonImg("tab_pack_n", FrButton::NORMAL);
				m_pTab[i]->SetButtonImg("tab_pack_o", FrButton::OVER);
			}
		}
		else if (i == tab)
		{
			m_pTab[i]->SetButtonImg(MakeStr("tab_vol%d_d", i),
				FrButton::NORMAL);
			m_pTab[i]->SetButtonImg(MakeStr("tab_vol%d_d", i), FrButton::OVER);
		}
		else
		{
			m_pTab[i]->SetButtonImg(MakeStr("tab_vol%d_n", i),
				FrButton::NORMAL);
			m_pTab[i]->SetButtonImg(MakeStr("tab_vol%d_o", i), FrButton::OVER);
		}
	}
}

void CCardBook::CardSlotControl(unsigned char page, unsigned char action,
	unsigned char slot)
{
	if (m_pPage[page])
		m_pPage[page]->SelectSlot(slot, action);
}

void CCardBook::PageIndexBehavior(const WPoint& pt, unsigned char action)
{
	unsigned char index = 0;
	if (m_pageIndexRect.size())
	{
		std::vector<WRect>::iterator it;
		for (it = m_pageIndexRect.begin(); it != m_pageIndexRect.end();
			++it, ++index)
		{
			if ((*it).x <= pt.x && (*it).y <= pt.y && (*it).w >= pt.x &&
				(*it).h >= pt.y)
			{
				switch (action)
				{
				case 1:
					m_curPage = index;
					BuildCardPage(index);
					break;
				case 2:
					m_overPage = index;
					break;
				}
				return;
			}
		}
		m_overPage = index;
	}
}

void CCardBook::DrawCardPackHelp(unsigned long typeId)
{
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bmp = g_pFresh->GetBitmap("help_1");
	if (bmp)
		pGI->DrawTexture(bmp, WRect(12, 439, bmp->Width(), bmp->Height()),
			0xffffffff, 0);
	bmp = g_pFresh->GetBitmap("help_2");
	if (bmp)
		pGI->DrawTexture(bmp, WRect(268, 439, bmp->Width(), bmp->Height()),
			0xffffffff, 0);
	pGI->SetTextColor(0xff887966, 0xffffffff);
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(typeId);
	IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(typeId);
	if (card)
		pGI->Print(WPoint(120, 458), 0, card->c.Name);
	if (desc)
	{
		unsigned long len = strlen(desc->Desc);
		unsigned long line = 0, column = 0;
		char text[512] = { 0 };
		strcpy(text, desc->Desc);
		char lines[3][128] = { 0 };
		for (unsigned long i = 0; i < len; i++)
		{
			lines[line][column++] = text[i];
			if (column > 38 && len > i + 1)
			{
				if ((unsigned char)text[i] >= 0x81 &&
					(unsigned char)text[i] <= 0xfe)
					lines[line][column++] = text[++i];
				lines[line][column] = 0;
				line++;
				column = 0;
			}
			if (line >= 3)
				break;
		}
		pGI->Print(WPoint(120, 474), 0, lines[0]);
		pGI->Print(WPoint(72, 486), 0, lines[1]);
		pGI->Print(WPoint(72, 498), 0, lines[2]);
	}
	pGI->SetTextColor(0xff000000, 0xffffffff);
}
