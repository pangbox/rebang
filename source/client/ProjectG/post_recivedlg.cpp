#include "post_recivedlg.h"
#include "transferableitem.h"
#include "returngiftdlg.h"
#include "ucclibrary.h"
#include "frscrollbar.h"
#include "frarea.h"
#include <algorithm>

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(CPost_ReciveDlg, FrForm)
BEGIN_FRESH_MSGMAP(CPost_ReciveDlg, FrForm)
ON_FRESH_VI("recive_post", FRCMD_INIT, CPost_ReciveDlg::OnRecivePostInit)
ON_FRESH_VV("recive_post", FRCMD_LBUTTONUP, CPost_ReciveDlg::OnRecivePostBtnUp)
ON_FRESH_VI("send_post", FRCMD_INIT, CPost_ReciveDlg::OnSendPostInit)
ON_FRESH_VV("send_post", FRCMD_LBUTTONUP, CPost_ReciveDlg::OnSendPostBtnUp)
ON_FRESH_VI("delete_letter", FRCMD_INIT, CPost_ReciveDlg::OnDeleteAllLetterInit)
ON_FRESH_VV("delete_letter", FRCMD_LBUTTONUP,
	CPost_ReciveDlg::OnDeleteAllLetterBtnUp)
ON_FRESH_VI("write_letter", FRCMD_INIT, CPost_ReciveDlg::OnWriteLetterInit)
ON_FRESH_VV("write_letter", FRCMD_LBUTTONUP,
	CPost_ReciveDlg::OnWriteLetterBtnUp)
ON_FRESH_VI("post_slot", FRCMD_INIT, CPost_ReciveDlg::OnPostListInit)
ON_FRESH_VI("post_slot", FRCMD_OWNERDRAW, CPost_ReciveDlg::OnPostListOwnerDraw)
ON_FRESH_VV("post_slot", FRCMD_LBUTTONDOWN, CPost_ReciveDlg::OnPostListBtnDown)
ON_FRESH_VV("post_slot", FRCMD_RBUTTONDOWN, CPost_ReciveDlg::OnPostListRBtnDown)
ON_FRESH_VV("post_slot", FRCMD_MOUSEMOVE, CPost_ReciveDlg::OnPostListMouseMove)
ON_FRESH_VV("post_slot", FRCMD_DBLCLICK, CPost_ReciveDlg::OnPostListDClick)
ON_FRESH_VI("page_list", FRCMD_INIT, CPost_ReciveDlg::OnPageListInit)
ON_FRESH_VI("page_list", FRCMD_OWNERDRAW, CPost_ReciveDlg::OnPageListOwnerDraw)
ON_FRESH_VV("page_list", FRCMD_LBUTTONDOWN, CPost_ReciveDlg::OnPageListBtnDown)
ON_FRESH_VI("btn_minus", FRCMD_INIT, CPost_ReciveDlg::OnPageDownInit)
ON_FRESH_VV("btn_minus", FRCMD_LBUTTONUP, CPost_ReciveDlg::OnPageDownBtnUp)
ON_FRESH_VI("btn_pluse", FRCMD_INIT, CPost_ReciveDlg::OnPageUpInit)
ON_FRESH_VV("btn_pluse", FRCMD_LBUTTONUP, CPost_ReciveDlg::OnPageUpBtnUp)
ON_FRESH_BI("Edit_Page", FRCMD_ENTERKEY, CPost_ReciveDlg::OnEditPageEndterKey)
ON_FRESH_VI("tooltip", FRCMD_INIT, CPost_ReciveDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, CPost_ReciveDlg::OnToolTipOnwerDraw)
ON_FRESH_VI("index_area", FRCMD_INIT, CPost_ReciveDlg::OnIndexAreaInit)
ON_FRESH_VV("index_area", FRCMD_OWNERDRAW,
	CPost_ReciveDlg::OnIndexAreaOwnerDraw)
ON_FRESH_VI("index_list", FRCMD_INIT, CPost_ReciveDlg::OnIndexListInit)
ON_FRESH_VV("index_list", FRCMD_LBUTTONDOWN, CPost_ReciveDlg::OnIndexListLBDown)
ON_FRESH_VV("index_list", FRCMD_OWNERDRAW,
	CPost_ReciveDlg::OnIndexListMouseMove)
END_FRESH_MSGMAP()

CPost_ReciveDlg::CPost_ReciveDlg()
{
	m_pContents = NULL;
	m_requestPage = 1;
	m_curPage = 1;
	m_pSelectItem = NULL;
	m_questionType = -1;
	m_pageStep = 1;
	static char* windFiles[] = { "[font_wind.jpg" };
	WTITLEFONT wind;
	memset(&wind, 0, sizeof(wind));
	wind.filename = windFiles;
	wind.fontw = 16;
	wind.fonth = 16;
	wind.numPages = 1;
	wind.texw = 128;
	wind.texh = 64;
	wind.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&wind);
	m_pIndexArea = NULL;
	m_pIndexList = NULL;
	m_indexPageNum = 0;
	m_curIndexPage = 0;
	m_selIndex = 0;
	m_overIndex = 0;
	m_indexNum = 0;
	m_pIndexRect = NULL;
	m_pDeleteAllLetter = NULL;
	m_pWriteLetter = NULL;
	m_lastSelRow = -1;
	OnLoadImage();
	OnLoadPostList();
}

CPost_ReciveDlg::~CPost_ReciveDlg()
{
	if (m_pTitleFont)
		m_pTitleFont->Erase();
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
	if (m_pIndexRect)
	{
		delete[] m_pIndexRect;
		m_pIndexRect = NULL;
	}
}

void CPost_ReciveDlg::OnRecivePostInit(int param)
{
	m_pRecivePost = DYNAMIC_CAST(FrButton, param);
}

void CPost_ReciveDlg::OnRecivePostBtnUp()
{
}

void CPost_ReciveDlg::OnSendPostInit(int param)
{
	m_pSendPost = DYNAMIC_CAST(FrButton, param);
}

void CPost_ReciveDlg::OnSendPostBtnUp()
{
	FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
	form->SetMessage(
		"\xb0\xf8\xbb\xe7\xc1\xdf!!! \xb3\xaa\xc1\xdf\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4~",
		false);
	form->Open(NULL, FrESCAPE | FrENTER);
}

void CPost_ReciveDlg::OnDeleteAllLetterInit(int param)
{
	m_pDeleteAllLetter = DYNAMIC_CAST(FrButton, param);
}

struct delete_letter_check
{
	delete_letter_check()
		: count(0)
	{
	}
	void operator()(const sMailInfoBrief& info)
	{
		if (info.itemCount == 0 && count < 100)
		{
			++count;
			ids.push_back(info.id);
		}
	}
	unsigned int count;
	std::list<unsigned long> ids;
};

void CPost_ReciveDlg::OnDeleteAllLetterBtnUp()
{
	sMailBoxListForPage* page =
		CTransferableItem::Instance()->GetMailBoxPageList();
	delete_letter_check check = std::for_each(page->mailList.begin(),
		page->mailList.end(), delete_letter_check());
	if (check.count)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		form->SetMessage(
			MakeStr(
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc3\xb7\xba\xce\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xc7\xf6\xc0\xe7 \xc6\xe4\xc0\xcc\xc1\xf6\xc0\xc7 \xc6\xed\xc1\xf6(%d\xb0\xb3)\xb8\xa6 \xb8\xf0\xb5\xce \xbb\xe8\xc1\xa6 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?\n\n\\c0xffff0000\\c(\xc1\xd6\xc0\xc7 : \xb3\xbb\xbf\xeb\xc0\xbb \xc0\xd0\xc1\xf6 \xbe\xca\xc0\xba \xc6\xed\xc1\xf6\xb5\xb5 \xb8\xf0\xb5\xce \xbb\xe8\xc1\xa6 \xb5\xcb\xb4\xcf\xb4\xd9.)\\c0xff000000\\c",
				check.count),
			false);
		form->Open((FRESH_PFN_RESULT)&CPost_ReciveDlg::OnDeleteAllLetterResult,
			FrESCAPE | FrENTER);
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		form->SetMessage(
			"\xbb\xe8\xc1\xa6\xc7\xd2 \xc6\xed\xc1\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
	m_lastSelRow = -1;
}

bool CPost_ReciveDlg::OnDeleteAllLetterResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		sMailBoxListForPage* page =
			CTransferableItem::Instance()->GetMailBoxPageList();
		delete_letter_check check = std::for_each(page->mailList.begin(),
			page->mailList.end(), delete_letter_check());
		WSendPacket packet((enumClientPacket)0xbe);
		packet.Encode4(check.count);
		std::list<unsigned long>::iterator it = check.ids.begin();
		while (it != check.ids.end())
		{
			packet.Encode4(*it);
			++it;
		}
		packet.Encode4(m_curPage);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
		m_pSelectItem = NULL;
	}
	return true;
}

void CPost_ReciveDlg::OnWriteLetterInit(int param)
{
	m_pWriteLetter = DYNAMIC_CAST(FrButton, param);
}

void CPost_ReciveDlg::OnWriteLetterBtnUp()
{
	if (IsLocalContent((localContentType_t)8) &&
		(Doc()->m_myInfo.info.dwFlagBlock & 0x80))
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\\c0xffff0000\\c\xc6\xed\xc1\xf6\xbe\xb2\xb1\xe2 \xba\xed\xb7\xb0\\c0xff000000\\c \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.\n\n\xb0\xed\xb0\xb4\xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		return;
	}
	AfxGetTask()->GetActor("RealMyRoom")
		<< MsgObject(NULL, 0x221, 0, 0, 0, 0, 0);
	SetFadeout(true);
	Close(true);
}

void CPost_ReciveDlg::OnPageListInit(int param)
{
	m_pPageList = DYNAMIC_CAST(FrListBox, param);
	if (m_pPageList)
	{
		m_pPageList->ClearItem();
		for (int i = 1; i <= 11; ++i)
		{
			FrListItem* item = m_pPageList->AddItem((void*)(m_pageStep * i));
			if (i == 1)
				m_pPageList->SelectItem(item, true);
		}
		m_pPageList->SetMultiSelect(false);
		m_pPageList->SetVisible(false);
	}
}

void CPost_ReciveDlg::OnPageListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	int page = (int)item->pData;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	float dx = 7.5f, dy = 0.0f;
	if (!page)
		return;
	if (page > m_totalPage)
		item->selected = false;
	graphic->SetTextColor(0xff959494, 0xffffffff);
	if (item->selected)
	{
		graphic->SetTextStyle(1);
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	else if (item->underCursor)
	{
		dy = -0.5f;
		dx = 7.0f;
	}
	else
		graphic->SetTextStyle(0);
	if (page != m_totalPage)
	{
		graphic->Print(WPoint(item->pos.x + dx, item->pos.y + dy), 1, "%d ",
			page);
		graphic->SetTextStyle(0);
		graphic->SetTextColor(0xff959494, 0xffffffff);
		graphic->Print(WPoint(item->pos.x + 12.0f, item->pos.y), 1, "|");
	}
	else
		graphic->Print(WPoint(item->pos.x + dx, item->pos.y + dy), 1, "%d",
			page);
}

void CPost_ReciveDlg::OnPageListBtnDown()
{
	FrListItem* item = m_pPageList->GetItemUnderCursor();
	if (!item)
		return;
	int page = (int)item->pData;
	int maximum = m_totalPage;
	if (page < 1)
		page = 1;
	else if (page > maximum)
		page = maximum;
	m_requestPage = page;
	OnLoadPostList();
}

void CPost_ReciveDlg::OnPostListInit(int param)
{
	m_pPostList = DYNAMIC_CAST(FrListBox, param);
	if (m_pPostList)
	{
		m_pPostList->SetMultiSelect(false);
		m_pPostList->UseDummy(true);
		m_pPostList->SetPushSound(NULL);
		m_pPostList->AddNoRButtonRect(m_miniBtn[1].rect, 1);
		m_pPostList->AddNoRButtonRect(m_miniBtn[0].rect, 0);
	}
}

void CPost_ReciveDlg::OnPostListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap;
	if (!row->pData)
		bitmap = m_pItemBaseEmpty;
	else
	{
		if (row->selected)
		{
			bitmap = m_pItemBaseD;
			graphic->SetTextColor(0xffff0000, 0xffffffff);
		}
		else if (row->underCursor)
		{
			bitmap = m_pItemBaseO;
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
		else
		{
			bitmap = m_pItemBaseN;
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
	}
	graphic->SetTextStyle(0);
	if (bitmap)
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	if (!row->pData)
		return;
	sMailInfoBrief& mail = *(sMailInfoBrief*)row->pData;
	const char* name = "\xc6\xed\xc1\xf6";
	unsigned long typeId = mail.item.dwTID;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	int count;
	if (common && mail.itemCount != 0)
	{
		name = common->Name;
		count = mail.item.iCount;
		bitmap = g_pFresh->GetBitmap(common->Icon);
	}
	else
	{
		if (!mail.bRead)
			bitmap = m_pArrivalMail;
		else
			bitmap = m_pOpenMail;
		count = 0;
	}
	float dx, dy;
	if (bitmap)
	{
		switch (typeId >> 26)
		{
		case 1:
		case 2:
		case 8:
		case 15:
		case 16:
		case 28:
			dx = 7.0f;
			dy = 2.0f;
			break;
		case 11:
			dx = 9.0f;
			dy = 12.0f;
			break;
		case 14:
			if ((typeId & 0x03c00000) == 0x00800000)
			{
				dx = 7.0f;
				dy = 0.0f;
			}
			else
			{
				dx = 16.0f;
				dy = 27.0f;
			}
			break;
		case 31:
			dx = 16.0f;
			dy = 12.0f;
			break;
		default:
			dx = 16.0f;
			dy = 12.0f;
			break;
		}
	}
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId) &&
		mail.itemCount > 0)
		bitmap = GetUccItemIcon(mail.item.dwTID, mail.item.csUCCIndex);
	if (bitmap && graphic)
	{
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + dx, row->pos.y + dy, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	}
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	miniBtn_t btn;
	if (mail.itemCount > 0)
		btn = m_miniBtn[0];
	else
		btn = m_miniBtn[1];
	WRect rect = m_miniBtn[1].rect + row->pos;
	if (!btn.bEnable)
	{
		graphic->DrawTexture(btn.pNormal, WRect(rect), 0x88ffffff, 0);
	}
	else if (row->underCursor && rect.IsInRect(mouse))
	{
		graphic->DrawTexture(btn.pOver, rect + WPoint(0.0f, 0.0f), 0xffffffff,
			0);
		TOOLTIP()->SetToolTip((eBarButton)64);
	}
	else
	{
		graphic->DrawTexture(btn.pNormal, WRect(rect), 0xffffffff, 0);
	}
	if (mail.item.btItemType && mail.item.iTimeCount > 0)
	{
		graphic->SetTextStyle(2);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		const char* format = NULL;
		if ((typeId & 0xfc000000) != 0x40000000)
		{
			format = "%d\xc0\xcf";
		}
		else
		{
			IFF_ITEM_COMMON* timeItem = ItemManager()->FindCommonItem(typeId);
			if (timeItem)
				format = timeItem->TimeFlag == 1 ? "%d\xc0\xcf"
												 : "\xb9\xab\xc1\xa6\xc7\xd1";
		}
		if (format)
		{
			WPoint labelPos(row->pos.x + 72.0f, row->pos.y + 72.0f);
			graphic->Print(WPoint(labelPos), 2, format, mail.item.iTimeCount);
		}
	}
	else if (count > 1)
	{
		unsigned int i = 0;
		const char* text = MakeStr("%d", count);
		float x = row->pos.x + 72.0f;
		for (i = 0; i < strlen(text); ++i)
			x -= m_pTitleFont->GetCharWidth(g_view, text[i] - '0');
		m_pTitleFont->Print(g_view, x, row->pos.y + 42.0f, text, 0, 0xffffffff,
			NULL);
	}
	if (mail.itemCount == -1)
	{
		WPoint labelPos(row->pos.x + 182.0f, row->pos.y + 23.0f);
		graphic->Print(WPoint(labelPos), 2, "%d", typeId);
	}
	else if (mail.itemCount > 1)
	{
		WPoint labelPos(row->pos.x + 182.0f, row->pos.y + 23.0f);
		graphic->Print(WPoint(labelPos), 2, " + %d\xb0\xb3",
			mail.itemCount - 1);
	}
	graphic->SetTextStyle(0);
	{
		WPoint labelPos(row->pos.x + 182.0f, row->pos.y + 7.0f);
		graphic->Print(WPoint(labelPos), 2, "%s", name);
	}
	if (IsLocalContent((localContentType_t)0x51) &&
		(mail.item.dwTID & 0xfc000000) == 0x08000000 && mail.itemCount > 0)
		DrawPartsSlot(graphic, row);
}

void CPost_ReciveDlg::DrawPartsSlot(FrGraphicInterface* graphic,
	FrListItem* row)
{
	int slot;
	sMailInfoBrief* item = (sMailInfoBrief*)row->pData;
	if (!item)
		return;
	if (!graphic)
		graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float characterX = 5.0f;
	float caddieX = 5.0f;
	if ((int)item->item.CharacterSlotNum <= -1 ||
		item->item.CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->item.dwTID);
		if (part)
		{
			item->item.CharacterSlotNum = part->CharacterSlot;
			if ((int)item->item.CharacterSlotNum <= -1 ||
				item->item.CharacterSlotNum >= 5)
				item->item.CharacterSlotNum = 0;
		}
		else
			item->item.CharacterSlotNum = 0;
	}
	for (slot = 1; slot < item->item.CharacterSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->item.dwTID == (*it).partsTid &&
				item->item.dwIDX == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 0)
				{
					found = true;
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + characterX, row->pos.y + 54.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						characterX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + characterX, row->pos.y + 54.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				characterX += 16.0f;
			}
		}
	}
	if ((int)item->item.CaddieSlotNum <= -1 || item->item.CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->item.dwTID);
		if (part)
		{
			item->item.CaddieSlotNum = part->CaddieSlot;
			if ((int)item->item.CaddieSlotNum <= -1 ||
				item->item.CaddieSlotNum >= 5)
				item->item.CaddieSlotNum = 0;
		}
		else
			item->item.CaddieSlotNum = 0;
	}
	for (slot = 1; slot < item->item.CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->item.dwTID == (*it).partsTid &&
				item->item.dwIDX == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 1)
				{
					found = true;
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr(card->SlotImg));
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						caddieX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.0f;
			}
		}
	}
}

void CPost_ReciveDlg::OnReset()
{
	m_requestPage = 1;
	OnLoadPostList();
}

void CPost_ReciveDlg::DrawStatus2(FrGraphicInterface* graphic, FrListItem* row,
	short* const x, short* const y, float alpha)
{
	float dy = 0.0f;
	for (int i = 0; i < 5; ++i)
		if (x[i] || y[i])
		{
			graphic->SetTextColor(0xff0000ff, 0xffffffff);
			graphic->Print(row->pos +
					WPoint(182.0f + alpha, 36.0f + alpha + dy),
				2, MakeStatusString(i, x[i], y[i]));
			dy += 14.0f;
		}
}

const char* CPost_ReciveDlg::MakeStatusString(int a, int b, int c)
{
	WRect rect = GetRect();
	const char* names[] = { "\xc6\xc4\xbf\xf6", "\xc4\xc1\xc6\xae\xb7\xd1",
		"\xc1\xa4\xc8\xae\xb5\xb5", "\xbd\xba\xc7\xc9", "\xc4\xbf\xba\xea" };
	if (b)
	{
		if (c)
			return MakeStr("%s+%d/%s+%d", names[a], b, "\xbd\xbd\xb7\xd4", c);
		return MakeStr("%s+%d", names[a], b);
	}
	if (c)
		return MakeStr("%s%s+%d", names[a], "\xbd\xbd\xb7\xd4", c);
	return "";
}

void CPost_ReciveDlg::OnPostListBtnDown()
{
	FrListItem* row = m_pPostList->GetItemUnderCursor();
	if (!row || !row->pData)
		return;
	m_pSelectItem = row;
	sMailInfoBrief* mail = (sMailInfoBrief*)row->pData;
	if (!mail)
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if ((m_miniBtn[1].rect + row->pos).IsInRect(mouse))
	{
		if (mail->itemCount > 0)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(mail->item.dwTID);
			if (common)
			{
				if (m_miniBtn[0].bEnable)
					Question(0);
			}
			else
			{
				FrForm* form =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				form->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xbf\xa1 \xc3\xb7\xba\xce\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc7\xf6\xc0\xe7 \xbf\xc5\xb1\xe6\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				form->Open(NULL, FrESCAPE | FrENTER);
			}
		}
		else if (m_miniBtn[1].bEnable)
			Question(1);
		m_lastSelRow = row->idx;
	}
	else
	{
		m_questionType = -1;
		ToServerSendData(row, 0xbd);
	}
}

void CPost_ReciveDlg::OnPostListRBtnDown()
{
}

void CPost_ReciveDlg::Question(int type)
{
	if (type == -1)
		return;
	m_questionType = type;
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
	if (type != 0)
		form->SetMessage(
			"\xc6\xed\xc1\xf6\xb8\xa6 \xbb\xe8\xc1\xa6\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
	else
		form->SetMessage(
			"\xbc\xb1\xb9\xb0\xc0\xbb \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xb8\xb7\xce \xbf\xc5\xb1\xe2\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
	form->Open((FRESH_PFN_RESULT)&CPost_ReciveDlg::OnPostQuestionResult,
		FrENTER);
}

bool CPost_ReciveDlg::OnPostQuestionResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		if (m_questionType != 0)
			ToServerSendData(m_pSelectItem, 0xbe);
		else
			ToServerSendData(m_pSelectItem, 0xbf);
	}
	else
	{
		m_pSelectItem = NULL;
		m_lastSelRow = -1;
	}
	return true;
}

void CPost_ReciveDlg::ToServerSendData(FrListItem* row, unsigned long packetId)
{
	if (row && row->pData)
	{
		sMailInfoBrief* mail = (sMailInfoBrief*)row->pData;
		if (packetId == 0xbe)
		{
			WSendPacket packet((enumClientPacket)0xbe);
			packet.Encode4(1);
			packet.Encode4(mail->id);
			packet.Encode4(m_curPage);
			packet.Send(TO_GAME);
		}
		else
		{
			WSendPacket packet(packetId);
			packet.Encode4(mail->id);
			packet.Send(TO_GAME);
		}
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
		if (packetId != 0xbd)
			m_pSelectItem = NULL;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		form->SetMessage(
			"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void CPost_ReciveDlg::OnPostListMouseMove()
{
}

void CPost_ReciveDlg::OnPostDescDlgCreate()
{
	sMailInfo* info = CTransferableItem::Instance()->GetMailInfo();
	if (info)
	{
		m_pContents = CreateForm<CPost_Contents>(g_pFresh->GetManager(), this,
			"post_contents");
		if (m_pContents)
		{
			FrForm::EnableTail(false);
			m_pContents->SetData(info);
			m_pContents->Open(
				(FRESH_PFN_RESULT)&CPost_ReciveDlg::OnPostMessageResult,
				FrESCAPE);
			m_pContents->SetFadeout(true);
		}
	}
}

bool CPost_ReciveDlg::OnPostMessageResult(int result, FrForm* form)
{
	if (m_pSelectItem)
	{
		sMailInfoBrief* mail = (sMailInfoBrief*)m_pSelectItem->pData;
		if (mail)
		{
			m_pSelectItem->selected = false;
			mail->bRead = true;
			m_pSelectItem = NULL;
		}
	}
	return true;
}

void CPost_ReciveDlg::OnPageDownInit(int param)
{
	m_pPageDown = DYNAMIC_CAST(FrButton, param);
}

void CPost_ReciveDlg::OnPageDownBtnUp()
{
	if ((unsigned int)m_curIndexPage > 0)
	{
		--m_curIndexPage;
		BuildIndexPage(m_curIndexPage);
		int page = m_curIndexPage;
		m_selIndex = 0;
		WSendPacket packet((enumClientPacket)0xbc);
		packet.Encode4(page * 5 + 1);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

void CPost_ReciveDlg::OnPageUpInit(int param)
{
	m_pPageUp = DYNAMIC_CAST(FrButton, param);
}

void CPost_ReciveDlg::OnPageUpBtnUp()
{
	if ((unsigned int)(m_curIndexPage + 1) < (unsigned int)m_indexPageNum)
	{
		++m_curIndexPage;
		BuildIndexPage(m_curIndexPage);
		int page = m_curIndexPage;
		m_selIndex = 0;
		WSendPacket packet((enumClientPacket)0xbc);
		packet.Encode4(page * 5 + 1);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

void CPost_ReciveDlg::OnPostListDClick()
{
}

bool CPost_ReciveDlg::OnEditPageEndterKey(int param)
{
	return true;
}

int CPost_ReciveDlg::GetItemNumInMyPocket(unsigned long typeId)
{
	int count = 0;
	if (typeId == 0 || (typeId & 0xfc000000) == 0x04000000)
	{
		for (std::map<unsigned int, sCharacterInfo>::iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(it->second.tid);
			if (common && (typeId == 0 || it->second.tid == typeId))
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x08000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_partsList.begin();
			it != Doc()->m_partsList.end(); ++it)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(it->tid);
			if (common && (typeId == 0 || it->tid == typeId))
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x18000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); ++it)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(it->tid);
			if (common && (typeId == 0 || it->tid == typeId))
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x1c000000)
	{
		for (std::map<unsigned int, sCaddieInfo>::iterator it =
				 Doc()->m_caddieMap.begin();
			it != Doc()->m_caddieMap.end(); ++it)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(it->second.tid);
			if (common && (typeId == 0 || it->second.tid == typeId))
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x20000000)
		count = 0;
	if (typeId == 0 || (typeId & 0xfc000000) == 0x3c000000)
	{
		for (std::map<unsigned int, sCharacterInfo>::iterator it =
				 Doc()->m_charMap.begin();
			it != Doc()->m_charMap.end(); ++it)
		{
			IFF_STRUCT::sHairStyle* hair = ItemManager()->FindHairStyle(typeId);
			if (hair && hair->cHairID == it->second.hairClr &&
				(hair->cCharID | 0x04000000) == it->second.tid)
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x10000000)
	{
		for (std::map<unsigned int, sItemInfo>::iterator it =
				 Doc()->m_clubSetMap.begin();
			it != Doc()->m_clubSetMap.end(); ++it)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (common && (typeId == 0 || (*it).second.tid == typeId))
				++count;
		}
	}
	if (typeId == 0 || (typeId & 0xfc000000) == 0x14000000)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_ballList.begin();
			it != Doc()->m_ballList.end(); ++it)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(it->tid);
			if (common && (typeId == 0 || it->tid == typeId))
				++count;
		}
	}
	if ((typeId >> 26) == 9 && (typeId & 0x03e00000) == 0x00200000)
	{
		IFF_STRUCT::sSetItem* set = ItemManager()->FindSetItem(typeId);
		for (unsigned char i = 0; i < set->nElems; ++i)
		{
			if (typeId == 0 || (set->ElemIds[i] & 0xfc000000) != 0x04000000)
			{
				for (std::list<sItemInfo>::iterator it =
						 Doc()->m_partsList.begin();
					it != Doc()->m_partsList.end(); ++it)
				{
					IFF_ITEM_COMMON* common =
						ItemManager()->FindCommonItem(it->tid);
					if (common && (typeId == 0 || it->tid == set->ElemIds[i]))
						++count;
				}
			}
		}
	}
	if (typeId == 0 || (typeId >> 26) == 14)
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_cutinList.begin();
			it != Doc()->m_cutinList.end(); ++it)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(it->tid);
			if (common && (typeId == 0 || it->tid == typeId))
				++count;
		}
	}
	return count;
}

bool CPost_ReciveDlg::OnReturnGiftDlgResult(int result, FrForm* form)
{
	if (result != FrOK)
		return true;
	{
		FrReturnGiftDlg* dialog = DYNAMIC_CAST(FrReturnGiftDlg, form);
		if (!dialog)
			return true;
		{
			WSendPacket packet((enumClientPacket)0x4d);
			packet.Encode4(dialog->m_giftIdx);
			packet.Encode4(dialog->m_typeId);
			packet.Send(TO_GAME);
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			EnableMiniBtns(false);
		}
	}
	return true;
}

bool CPost_ReciveDlg::OnMoveGiftDlgResult(int result, FrForm* form)
{
	if (result == FrOK && m_pMoveGift)
	{
		FrForm* notify =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		notify->SetMessage(
			"\xbc\xb1\xb9\xb0 \xbf\xc5\xb1\xe2\xb1\xe2\xb8\xa6 \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		notify->Open(NULL, FrESCAPE | FrENTER);
		WSendPacket packet((enumClientPacket)0x24);
		packet.Encode4(m_pMoveGift->guid);
		packet.Encode4(m_pMoveGift->tid);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
	return true;
}

void CPost_ReciveDlg::OnLoadImage()
{
	m_miniBtn[0].pNormal = g_pFresh->RegisterBitmap("rmr_giftmove_btn_n");
	m_miniBtn[0].pOver = g_pFresh->RegisterBitmap("rmr_giftmove_btn_o");
	m_miniBtn[0].rect =
		WRect(103.0f, 66.0f, (float)m_miniBtn[0].pNormal->Width(),
			(float)m_miniBtn[0].pNormal->Height());
	m_miniBtn[0].bEnable = true;
	m_miniBtn[1].pNormal = g_pFresh->RegisterBitmap("rmr_delete_btn_n");
	m_miniBtn[1].pOver = g_pFresh->RegisterBitmap("rmr_delete_btn_o");
	m_miniBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_miniBtn[1].pNormal->Width(),
			(float)m_miniBtn[1].pNormal->Height());
	m_miniBtn[1].bEnable = true;
	m_pItemBaseN = g_pFresh->RegisterBitmap("item_base_n");
	m_pItemBaseO = g_pFresh->RegisterBitmap("item_base_o");
	m_pItemBaseD = g_pFresh->RegisterBitmap("item_base_d");
	m_pItemBaseEmpty = g_pFresh->RegisterBitmap("item_base_empty");
	m_pArrivalMail = g_pFresh->RegisterBitmap("rmr_Arrival_mail");
	m_pOpenMail = g_pFresh->RegisterBitmap("rmr_Open_mail");
}

void CPost_ReciveDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void CPost_ReciveDlg::OnToolTipOnwerDraw(int param)
{
	m_toolTipMng.Process();
	int tip = TOOLTIP()->GetCurTipIndex();
	if (tip >= 64 && tip <= 65)
		TOOLTIP()->ClearToolTip();
}

void CPost_ReciveDlg::BuildIndexPage(int page)
{
	m_indexNum = 0;
	if (m_pIndexRect)
	{
		delete[] m_pIndexRect;
		m_pIndexRect = NULL;
	}
	if (m_pageStep < 1)
	{
		m_indexPageNum = 0;
		m_indexNum = 0;
		return;
	}
	int remainder = m_totalPage % 5;
	m_indexPageNum = m_totalPage / 5;
	if (remainder)
		m_indexPageNum++;
	if (m_totalPage - page * 5 >= 5)
		m_indexNum = 5;
	else
	{
		if (remainder)
			m_indexNum = (unsigned char)remainder;
		if (m_indexNum < 1)
			return;
	}
	m_pIndexRect = new WRect[m_indexNum];
	float left = 243.0f - (float)(m_indexNum * 30.0f) * 0.5f;
	if (m_pIndexRect)
		for (unsigned int i = 0; i < m_indexNum; ++i)
		{
			m_pIndexRect[i] = WRect(left + i * 30.0f, 300.0f,
				left + (float)(i * 30.0f) + 30.0f, 310.0f);
		}
}

void CPost_ReciveDlg::EnableMiniBtns(bool enable)
{
	for (int i = 0; i < 2; ++i)
		m_miniBtn[i].bEnable = enable;
	if (m_pDeleteAllLetter)
		m_pDeleteAllLetter->Enable(enable);
	if (m_pWriteLetter)
		m_pWriteLetter->Enable(enable);
}

void CPost_ReciveDlg::OnIndexAreaInit(int param)
{
	m_pIndexArea = DYNAMIC_CAST(FrArea, param);
}

void CPost_ReciveDlg::OnIndexAreaOwnerDraw()
{
	if (!m_pIndexRect)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	for (int i = 0; i < m_indexNum; ++i)
	{
		WRect rect = GetRect();
		if (i == m_selIndex)
		{
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->SetTextStyle(1);
		}
		else if (i == m_overIndex)
		{
			graphic->SetTextColor(0xffaa1111, 0xffffffff);
			graphic->SetTextStyle(1);
		}
		else
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->SetTextStyle(0);
		}
		graphic->Print(WPoint((float)(rect.x + m_pIndexRect[i].x) + 15.0f,
						   rect.y + m_pIndexRect[i].y),
			1, "%d", m_curIndexPage * 5 + i + 1);
		if (i != 0)
		{
			graphic->SetTextColor(0xffaaaaaa, 0xffffffff);
			graphic->SetTextStyle(0);
			graphic->Print(
				WPoint(rect.x + m_pIndexRect[i].x, rect.y + m_pIndexRect[i].y),
				1, "|");
		}
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
	}
}

void CPost_ReciveDlg::OnIndexListInit(int param)
{
	m_pIndexList = DYNAMIC_CAST(FrListBox, param);
}

void CPost_ReciveDlg::OnIndexListLBDown()
{
	if (m_indexNum && m_pIndexRect)
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		for (unsigned char i = 0; i < m_indexNum; ++i)
		{
			WRect rect = GetRect();
			WRect indexRect = m_pIndexRect[i];
			indexRect.x += rect.x;
			indexRect.y += rect.y;
			indexRect.w += rect.x;
			indexRect.h += rect.y;
			if (mouse.x >= indexRect.x && indexRect.w >= mouse.x &&
				mouse.y >= indexRect.y && mouse.y <= indexRect.h)
			{
				int page = m_curIndexPage * 5 + i + 1;
				m_selIndex = i;
				WSendPacket packet((enumClientPacket)0xbc);
				packet.Encode4(page);
				packet.Send(TO_GAME);
				AfxGetTask()->GetActor("RealMyRoom")
					<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
				EnableMiniBtns(false);
				break;
			}
		}
	}
}

void CPost_ReciveDlg::OnIndexListMouseMove()
{
	if (m_indexNum && m_pIndexRect)
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		for (unsigned char i = 0; i < m_indexNum; ++i)
		{
			WRect rect = GetRect();
			WRect indexRect = m_pIndexRect[i];
			indexRect.x += rect.x;
			indexRect.y += rect.y;
			indexRect.w += rect.x;
			indexRect.h += rect.y;
			if (mouse.x >= indexRect.x && indexRect.w >= mouse.x &&
				mouse.y >= indexRect.y && mouse.y <= indexRect.h)
			{
				m_overIndex = i;
				return;
			}
		}
		m_overIndex = 0xff;
	}
}

void CPost_ReciveDlg::OnLoadPostList()
{
	WSendPacket packet(0xbc);
	packet.Encode4(m_curPage);
	packet.Send(TO_GAME);
	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	EnableMiniBtns(false);
}

void CPost_ReciveDlg::SetLoadPostList()
{
	sMailBoxListForPage* page =
		CTransferableItem::Instance()->GetMailBoxPageList();
	m_totalPage = page->totalPage;
	m_curPage = page->curPage;
	m_pPageList->ClearItem();
	for (int i = 1; i <= m_totalPage; ++i)
	{
		FrListItem* item = m_pPageList->AddItem((void*)(m_pageStep * i));
		if (i == m_curPage)
			m_pPageList->SelectItem(item, true);
	}
	SetLoadPostList(&page->mailList);
	BuildIndexPage(m_curIndexPage);
	if ((unsigned int)(m_curIndexPage * 5 + 1 + m_selIndex) >
			(unsigned int)m_totalPage &&
		m_totalPage != 0)
	{
		m_selIndex = m_indexNum - 1;
		int page = m_selIndex + 1;
		WSendPacket packet(0xbc);
		packet.Encode4(page);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

void CPost_ReciveDlg::SetLoadPostList(std::list<sMailInfoBrief>* list)
{
	m_pPostList->ClearItem();
	sMailInfoBrief mail;
	if (list->size())
	{
		for (std::list<sMailInfoBrief>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if (IsLocalContent((localContentType_t)0x51))
			{
				IFF_STRUCT::sPart* part;
				if ((it->item.dwTID & 0xfc000000) == 0x08000000 &&
					it->itemCount > 0)
				{
					part = ItemManager()->FindPart(it->item.dwTID);
					if (part)
					{
						it->item.CharacterSlotNum = part->CharacterSlot;
						it->item.CaddieSlotNum = part->CaddieSlot;
					}
					else
					{
						it->item.CharacterSlotNum = 0;
						it->item.CaddieSlotNum = 0;
					}
				}
				else
				{
					it->item.CharacterSlotNum = 0;
					it->item.CaddieSlotNum = 0;
				}
			}
			FrListItem* row = m_pPostList->AddItem(&*it);
			row->AddNoRBtnIdx(0);
			row->AddNoRBtnIdx(1);
		}
		if (list->size() < 20)
		{
			for (int i = list->size(); i < 20; ++i)
				m_pPostList->AddItem(NULL);
		}
		m_pPostList->ResetTopRow();
		int row = (m_lastSelRow - 1) / 2;
		if (m_lastSelRow != -1 && row >= 0)
			m_pPostList->GetScrollBar()->SetCurTopRow(row);
		else
			m_pPostList->GetScrollBar()->ScrollToFirst();
		m_lastSelRow = -1;
	}
}

void CPost_ReciveDlg::OnDeleteItem(FrListItem* row)
{
	ToServerSendData(row, 0xbe);
}

IMPLEMENT_OBJECT(CPost_MessgeDlg, FrForm)
BEGIN_FRESH_MSGMAP(CPost_MessgeDlg, FrForm)
ON_FRESH_VI("recive_user", FRCMD_INIT,
	CPost_MessgeDlg::OnPostMessage_ReciveUserInit)
ON_FRESH_VI("letter_contents", FRCMD_INIT,
	CPost_MessgeDlg::OnPostMessage_LetterContentsInit)
ON_FRESH_VI("letter_date", FRCMD_INIT,
	CPost_MessgeDlg::OnPostMessage_LetterDateInit)
END_FRESH_MSGMAP()

CPost_MessgeDlg::CPost_MessgeDlg()
{
	m_pReciveUser = NULL;
	m_pLetterContents = NULL;
	m_pLetterDate = NULL;
}

CPost_MessgeDlg::~CPost_MessgeDlg()
{
}

void CPost_MessgeDlg::OnPostMessage_ReciveUserInit(int param)
{
	m_pReciveUser = DYNAMIC_CAST(FrEdit, param);
}

void CPost_MessgeDlg::OnPostMessage_LetterContentsInit(int param)
{
	m_pLetterContents = DYNAMIC_CAST(FrEdit, param);
}

void CPost_MessgeDlg::OnPostMessage_LetterDateInit(int param)
{
	m_pLetterDate = DYNAMIC_CAST(FrEdit, param);
}

void CPost_MessgeDlg::OnPostDescFrame(bool show)
{
	m_pReciveUser->SetElementFadeOut(!show);
	m_pLetterContents->SetElementFadeOut(!show);
	m_pLetterDate->SetElementFadeOut(!show);
}

void CPost_MessgeDlg::SetGiftDescString(const char* item, const char* contents,
	const char* sender, const char* date)
{
	WRect rect = GetRect();
	if (m_pReciveUser->GetLine(1, false) > 0)
		m_pReciveUser->ClearLine();
	if (m_pLetterContents->GetLine(1, false) > 0)
		m_pLetterContents->ClearLine();
	if (m_pLetterDate->GetLine(1, false) > 0)
		m_pLetterDate->ClearLine();
	if (!item)
	{
		m_pLetterContents->AddText(
			MakeStr(
				"\xb5\xb5\xc2\xf8\xc7\xd1 \xbf\xec\xc6\xed\xb9\xb0\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
			false, true);
		return;
	}
	m_pReciveUser->AddText(MakeStr("%s", item), false, true);
	if (strlen(contents) > 2)
		m_pLetterContents->AddText(MakeStr("%s", contents), false, true);
	else
		m_pLetterContents->AddText(
			MakeStr(
				"\xb5\xb5\xc2\xf8\xc7\xd1 \xbf\xec\xc6\xed\xb9\xb0\xbf\xa1 \xb3\xbb\xbf\xeb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
			false, true);

	m_pLetterDate->AddText(
		MakeStr("%s \\c0xff7a6625\\c(%s)\\c0xff000000\\c", sender, date), false,
		true);
}

void CPost_MessgeDlg::OnOK()
{
	SetFadeout(true);
	FrForm::OnOK();
}

IMPLEMENT_OBJECT(CPost_Contents, FrForm)
BEGIN_FRESH_MSGMAP(CPost_Contents, FrForm)
ON_FRESH_VI("recive_user", FRCMD_INIT,
	CPost_Contents::OnPostMessage_ReciveUserInit)
ON_FRESH_VI("send_user", FRCMD_INIT, CPost_Contents::OnPostMessage_SendUserInit)
ON_FRESH_VI("letter_contents", FRCMD_INIT,
	CPost_Contents::OnPostMessage_LetterContentsInit)
ON_FRESH_VI("letter_date", FRCMD_INIT,
	CPost_Contents::OnPostMessage_LetterDateInit)
ON_FRESH_VI("post_slot", FRCMD_INIT, CPost_Contents::OnPostListInit)
ON_FRESH_VI("post_slot", FRCMD_OWNERDRAW, CPost_Contents::OnPostListOwnerDraw)
ON_FRESH_VV("post_slot", FRCMD_LBUTTONDOWN, CPost_Contents::OnPostListBtnDown)
ON_FRESH_VV("post_slot", FRCMD_RBUTTONDOWN, CPost_Contents::OnPostListRBtnDown)
ON_FRESH_VV("post_slot", FRCMD_MOUSEMOVE, CPost_Contents::OnPostListMouseMove)
ON_FRESH_VV("post_slot", FRCMD_DBLCLICK, CPost_Contents::OnPostListDClick)
END_FRESH_MSGMAP()

CPost_Contents::CPost_Contents()
{
	static char* windFiles[] = { "[font_wind.jpg" };
	WTITLEFONT wind;
	memset(&wind, 0, sizeof(wind));
	wind.filename = windFiles;
	wind.fontw = 16;
	wind.fonth = 16;
	wind.numPages = 1;
	wind.texw = 128;
	wind.texh = 64;
	wind.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&wind);
}

CPost_Contents::~CPost_Contents()
{
	if (m_pTitleFont)
		m_pTitleFont->Erase();
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void CPost_Contents::OnPostMessage_ReciveUserInit(int param)
{
	m_pReciveUser = DYNAMIC_CAST(FrEdit, param);
}

void CPost_Contents::OnPostMessage_SendUserInit(int param)
{
	m_pSendUser = DYNAMIC_CAST(FrEdit, param);
}

void CPost_Contents::OnPostMessage_LetterContentsInit(int param)
{
	m_pLetterContents = DYNAMIC_CAST(FrEdit, param);
}

void CPost_Contents::OnPostMessage_LetterDateInit(int param)
{
	m_pLetterDate = DYNAMIC_CAST(FrEdit, param);
	if (m_pLetterDate)
		m_pLetterDate->AddText("2008-02-01 16:10", false, true);
}

void CPost_Contents::OnPostListInit(int param)
{
	m_pPostList = DYNAMIC_CAST(FrListBox, param);
}

void CPost_Contents::OnPostListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->SetTextStyle(0);
	sMailIncludeItem* item = (sMailIncludeItem*)row->pData;
	if (!item)
		return;
	unsigned long typeId = item->dwTID;
	const char* icon = "item0_13";
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	int count;
	if (common)
	{
		icon = common->Icon;
		count = item->iCount;
	}
	else
		count = 0;
	const Bitmap* bitmap = g_pFresh->GetBitmap(icon);
	float dx, dy;
	if (bitmap)
		switch (typeId >> 26)
		{
		case 1:
		case 2:
		case 8:
		case 9:
		case 15:
		case 16:
		case 28:
			dx = 7.0f;
			dy = 2.0f;
			break;
		case 11:
			dx = 9.0f;
			dy = 12.0f;
			break;
		case 14:
			if ((typeId & 0x03c00000) == 0x00800000)
			{
				dx = 7.0f;
				dy = 0.0f;
			}
			else
			{
				dx = 16.0f;
				dy = 27.0f;
			}
			break;
		default:
			dy = 27.0f;
			dx = 16.0f;
			break;
		case 31:
			dy = 12.0f;
			dx = 16.0f;
			break;
		}
	unsigned int group = typeId >> 26;
	if (group == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((typeId >> 18) & 255) | 0x04000000);
		if (character)
		{
			const Bitmap* bitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + 3.0f, row->pos.y + 3.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
	if (IsLocalContent((localContentType_t)0x57) && typeId &&
		IsUccClothes(typeId))
		bitmap = GetUccItemIcon(item->dwTID, item->csUCCIndex);
	if (bitmap && graphic)
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + dx, row->pos.y + dy, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	if (item->btItemType && item->iTimeCount > 0)
	{
		graphic->SetTextStyle(2);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		WPoint labelPos(row->pos.x + 72.0f, row->pos.y + 72.0f);
		graphic->Print(WPoint(labelPos), 2, "%d\xc0\xcf", item->iTimeCount);
	}
	else if (count > 1)
	{
		unsigned int i = 0;
		const char* text = MakeStr("%d", count);
		float x = row->pos.x + 72.0f;
		for (i = 0; i < strlen(text); ++i)
			x -= m_pTitleFont->GetCharWidth(g_view, text[i] - '0');
		m_pTitleFont->Print(g_view, x, row->pos.y + 60.0f, text, 0, 0xffffffff,
			NULL);
	}
	if (IsLocalContent((localContentType_t)0x51) && group == 2)
		DrawPartsSlot(graphic, row);
}

void CPost_Contents::DrawPartsSlot(FrGraphicInterface* graphic, FrListItem* row)
{
	int slot;
	sMailIncludeItem* item = (sMailIncludeItem*)row->pData;
	if (!item)
		return;
	if (!graphic)
		graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float characterX = 5.0f;
	float caddieX = 5.0f;
	if ((int)item->CharacterSlotNum <= -1 || item->CharacterSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTID);
		if (part)
		{
			item->CharacterSlotNum = part->CharacterSlot;
			if ((int)item->CharacterSlotNum <= -1 ||
				item->CharacterSlotNum >= 5)
				item->CharacterSlotNum = 0;
		}
		else
			item->CharacterSlotNum = 0;
	}
	for (slot = 1; slot < item->CharacterSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTID == (*it).partsTid && item->dwIDX == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 0)
				{
					found = true;
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + characterX, row->pos.y + 54.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						characterX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + characterX, row->pos.y + 54.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				characterX += 16.0f;
			}
		}
	}
	if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTID);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
		{
			item->CaddieSlotNum = 0;
		}
	}
	for (slot = 1; slot < item->CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTID == (*it).partsTid && item->dwIDX == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 1)
				{
					found = true;
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr(card->SlotImg));
					if (bitmap)
					{
						graphic->DrawTexture(bitmap,
							WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						caddieX += 16.0f;
					}
				}
			}
		}
		if (!found)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + caddieX, row->pos.y + 70.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.0f;
			}
		}
	}
}

void CPost_Contents::OnPostListBtnDown()
{
}

void CPost_Contents::OnPostListRBtnDown()
{
}

void CPost_Contents::OnPostListMouseMove()
{
}

void CPost_Contents::OnPostListDClick()
{
}

void CPost_Contents::SetData(sMailInfo* info)
{
	if (m_pSendUser)
	{
		m_pSendUser->ClearLine();
		if (strlen(info->szFromNick))
			m_pSendUser->AddText(MakeStr("%s", info->szFromNick), false, true);
	}
	if (m_pReciveUser)
	{
		m_pReciveUser->ClearLine();
		if (strlen(Doc()->m_myInfo.info.sNick))
			m_pReciveUser->AddText(MakeStr("%s", Doc()->m_myInfo.info.sNick),
				false, true);
	}
	if (m_pLetterContents)
	{
		m_pLetterContents->ClearLine();
		if (strlen(info->szContent))
			m_pLetterContents->AddText(MakeStr("%s", info->szContent), false,
				true);
	}
	if (m_pLetterDate)
	{
		m_pLetterDate->ClearLine();
		if (strlen(info->szDate))
			m_pLetterDate->AddText(MakeStr("%s", info->szDate), false, true);
	}
	SetData(&info->m_lMailItemList);
}

void CPost_Contents::SetData(std::list<sMailIncludeItem>* list)
{
	m_pPostList->ClearItem();
	sMailIncludeItem item;
	if (list->size())
		for (std::list<sMailIncludeItem>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if (IsLocalContent((localContentType_t)0x51) &&
				(it->dwTID & 0xfc000000) == 0x08000000)
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(it->dwTID);
				if (part)
				{
					it->CharacterSlotNum = part->CharacterSlot;
					it->CaddieSlotNum = part->CaddieSlot;
				}
				else
				{
					it->CharacterSlotNum = 0;
					it->CaddieSlotNum = 0;
				}
			}
			m_pPostList->AddItem(&*it);
		}
}
