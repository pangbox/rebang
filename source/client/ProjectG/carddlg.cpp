#include "carddlg.h"
#include "shareddoc.h"
#include "frwnd.h"
#include "frlistbox.h"
#include "frbutton.h"
#include "fredit.h"
#include "uccmanager.h"
#include "ucclibrary.h"
#include "netresourcemanager.h"
#include "../../shared/localize.h"

sItemInfo* GetMyItemInfo(unsigned long id);

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrSpCardUseDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrSpCardUseDlg, FrForm)
ON_FRESH_VI("item", FRCMD_INIT, FrSpCardUseDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrSpCardUseDlg::OnItemOwnerDraw)
ON_FRESH_VI("text1", FRCMD_INIT, FrSpCardUseDlg::OnText1Init)
ON_FRESH_VI("text2", FRCMD_INIT, FrSpCardUseDlg::OnText2Init)
END_FRESH_MSGMAP()

FrSpCardUseDlg::FrSpCardUseDlg()
{
	m_spCardTid = 0;
	m_overlappedCardTid = 0;
}

FrSpCardUseDlg::~FrSpCardUseDlg()
{
}

void FrSpCardUseDlg::SetSpCardInfo(unsigned long& tid)
{
	m_spCardTid = tid;
	m_overlappedCardTid = GetOverlappedCard();
}

unsigned long FrSpCardUseDlg::GetOverlappedCard()
{
	for (std::list<sSCardAvilityPeriodInfo>::iterator it =
			 Doc()->m_cardAbilityList[0].begin();
		it != Doc()->m_cardAbilityList[0].end(); ++it)
	{
		if ((*it).tid != m_spCardTid && (*it).cardType == 2 &&
			ItemManager()->CompareSystemTime(Doc()->GetServerTime(),
				(*it).useEndTime) != 1 &&
			(*it).Avility == ItemManager()->FindCard(m_spCardTid)->Avility)
			return (*it).tid;
	}
	return 0;
}

void FrSpCardUseDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

void FrSpCardUseDlg::OnText1Init(int param)
{
	m_pText1 = DYNAMIC_CAST(FrStatic, param);
	if (m_pText1)
	{
		m_pText1->Enable(false);
		m_pText1->SetVisible(false);
	}
}

void FrSpCardUseDlg::OnText2Init(int param)
{
	m_pText2 = DYNAMIC_CAST(FrStatic, param);
	if (m_pText2)
	{
		m_pText2->Enable(false);
		m_pText2->SetVisible(false);
	}
}

void FrSpCardUseDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	char buffer[512] = "";
	if (m_overlappedCardTid)
	{
		device->SetTextColor(0xff000000, 0xffffffff);
		device->Print(
			WPoint(m_pItem->GetRect().x + 20.f, m_pItem->GetRect().y + 11.f), 0,
			"\xc7\xf6\xc0\xe7 \xc0\xfb\xbf\xeb\xc1\xdf\xc0\xce \xc4\xab\xb5\xe5 \xc8\xbf\xb0\xfa\xb0\xa1 \xbb\xe8\xc1\xa6\xb5\xc7\xb0\xed");
		device->Print(
			WPoint(m_pItem->GetRect().x + 20.f, m_pItem->GetRect().y + 28.f), 0,
			"\xbb\xf5\xb7\xce\xbf\xee \xc4\xab\xb5\xe5 \xc8\xbf\xb0\xfa\xb0\xa1 \xc0\xfb\xbf\xeb\xb5\xcb\xb4\xcf\xb4\xd9.");
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_overlappedCardTid);
		if (!card)
			return;
		device->SetTextColor(0xff000000, 0xffffffff);
		device->SetTextStyle(1);
		device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
				WPoint(20.f, 50.f),
			0, "%s", card->c.Name);
		device->SetTextStyle(0);
		{
			IFF_STRUCT::sDesc* desc =
				ItemManager()->FindDesc(m_overlappedCardTid);
			if (desc)
			{
				strcpy(buffer, desc->Desc);
				for (int i = 0; i < 512; ++i)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
				device->Print(
					WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
						WPoint(20.f, 70.f),
					0, "%s", buffer);
			}
		}
		card = ItemManager()->FindCard(m_spCardTid);
		if (!card)
			return;
		device->SetTextColor(0xff000000, 0xffffffff);
		device->SetTextStyle(1);
		device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
				WPoint(20.f, 110.f),
			0, "%s", card->c.Name);
		device->SetTextStyle(0);
		{
			IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_spCardTid);
			if (desc)
			{
				strcpy(buffer, desc->Desc);
				for (int i = 0; i < 512; ++i)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
				device->Print(
					WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
						WPoint(20.f, 130.f),
					0, "%s", buffer);
			}
		}
		const Bitmap* bitmap = g_pFresh->GetBitmap("Overlapping_Arrow");
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 90.f, m_pItem->GetRect().y + 90.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
	{
		device->SetTextColor(0xff000000, 0xffffffff);
		device->Print(
			WPoint(m_pItem->GetRect().x + 20.f, m_pItem->GetRect().y + 11.f), 0,
			"\xc0\xcc \xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd8\xbc\xad \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1");
		device->Print(
			WPoint(m_pItem->GetRect().x + 20.f, m_pItem->GetRect().y + 28.f), 0,
			"\xb4\xd9\xc0\xbd \xc6\xaf\xbc\xf6\xc8\xbf\xb0\xfa\xb8\xa6 \xbe\xf2\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_spCardTid);
		if (!card)
			return;
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", card->SubIcon);
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 55.f, m_pItem->GetRect().y + 100.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		device->SetTextColor(0xff000000, 0xffffffff);
		device->SetTextStyle(1);
		device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
				WPoint(20.f, 50.f),
			0, "%s", card->c.Name);
		device->SetTextStyle(0);
		device->SetTextColor(0xff000000, 0xffffffff);
		device->SetTextStyle(0);
		{
			IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_spCardTid);
			if (desc)
			{
				strcpy(buffer, desc->Desc);
				for (int i = 0; i < 512; ++i)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
				device->Print(
					WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
						WPoint(20.f, 70.f),
					0, "%s", buffer);
			}
		}
	}
	const Bitmap* bitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "card_tiki1");
	if (bitmap)
		device->DrawTexture(bitmap,
			WRect(m_pItem->GetRect().x + 200.f, m_pItem->GetRect().y + 45.f,
				(float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
}

IMPLEMENT_OBJECT(FrCardUseDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrCardUseDlg, FrForm)
ON_FRESH_VI("item", FRCMD_INIT, FrCardUseDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrCardUseDlg::OnItemOwnerDraw)
END_FRESH_MSGMAP()

FrCardUseDlg::FrCardUseDlg()
{
	m_cardTypeId = 0;
	m_cardId = 0;
	m_reserved1 = 0;
	m_reserved2 = 0;
}

FrCardUseDlg::~FrCardUseDlg()
{
}

void FrCardUseDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

void FrCardUseDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	char buffer[512] = "";
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId);
	if (!card)
		return;
	unsigned long type = (m_cardTypeId >> 22) & 15;
	const Bitmap* bitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", card->SubIcon);
	if (bitmap)
		device->DrawTexture(bitmap,
			WRect(m_pItem->GetRect().x + 55.f, m_pItem->GetRect().y + 100.f,
				(float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
	bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", "card_tiki1");
	if (bitmap)
		device->DrawTexture(bitmap,
			WRect(m_pItem->GetRect().x + 200.f, m_pItem->GetRect().y + 45.f,
				(float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(1);
	device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(20.f, 50.f),
		0, "%s", card->c.Name);
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(0);
	if (type == 2 || type == 1)
	{
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_cardTypeId);
		if (desc)
		{
			strcpy(buffer, desc->Desc);
			for (int i = 0; i < 512; ++i)
				if (buffer[i] == '\n')
				{
					buffer[i] = 0;
					break;
				}
			device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
					WPoint(20.f, 70.f),
				0, "%s", buffer);
		}
	}
	else if (type == 0)
	{
		float y = 0.f;
		for (int i = 0; i < 5; ++i)
			if (card->COM[i] > 0)
			{
				if (i == 0)
					sprintf(buffer, "%s + %d",
						"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4", card->COM[0]);
				else if (i == 1)
					sprintf(buffer, "%s + %d",
						"\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4",
						card->COM[1]);
				else if (i == 2)
					sprintf(buffer, "%s + %d",
						"\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4",
						card->COM[2]);
				else if (i == 3)
					sprintf(buffer, "%s + %d",
						"\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4", card->COM[3]);
				else if (i == 4)
					sprintf(buffer, "%s + %d",
						"\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4", card->COM[4]);
				device->Print(
					WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
						WPoint(20.f, y + 70.f),
					0, "%s", buffer);
				y += 15.f;
			}
	}
}

IMPLEMENT_OBJECT(FrOpenCardPackDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrOpenCardPackDlg, FrForm)
ON_FRESH_VI("item", FRCMD_INIT, FrOpenCardPackDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrOpenCardPackDlg::OnItemOwnerDraw)
ON_FRESH_VI("levelImg", FRCMD_INIT, FrOpenCardPackDlg::OnLevelImgInit)
END_FRESH_MSGMAP()

FrOpenCardPackDlg::FrOpenCardPackDlg()
{
	m_cardTypeId = 0;
	m_cardId = 0;
}

FrOpenCardPackDlg::~FrOpenCardPackDlg()
{
}

void FrOpenCardPackDlg::OnLevelImgInit(int param)
{
	m_pLevelImg = DYNAMIC_CAST(FrArea, param);
}

void FrOpenCardPackDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

void FrOpenCardPackDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	char buffer[512] = "";
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId);
	if (!card)
		return;
	unsigned long type = (m_cardTypeId >> 22) & 15;
	const Bitmap* bitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", card->c.Icon);
	if (bitmap)
		device->DrawTexture(bitmap,
			WRect(m_pItem->GetRect().x + 20.f, m_pItem->GetRect().y + 50.f,
				(float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(1);
	device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(210.f, 55.f),
		0, "%s", card->c.Name);
	device->SetTextStyle(0);
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(1);
	device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(160.f, 55.f),
		0, "%s", "\xc0\xcc\xb8\xa7: ");
	device->SetTextStyle(0);
	char levelText[100] = "";
	if (card->c.IsUnderLvl)
	{
		const char* text = "\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf";
		strcpy(levelText, text);
	}
	else
	{
		const char* text = "\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3";
		strcpy(levelText, text);
	}
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(1);
	device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
			WPoint(160.f, 75.f),
		0, "%s", levelText);
	device->SetTextStyle(0);
	if (m_pLevelImg)
		m_pLevelImg->SetBgImg(MakeStr("level_%03d", card->c.Level + 1));
	device->SetTextColor(0xff000000, 0xffffffff);
	device->SetTextStyle(0);
	if (type == 3)
	{
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_cardTypeId);
		if (desc)
		{
			strcpy(buffer, desc->Desc);
			for (int i = 0; i < 512; ++i)
				if (buffer[i] == '\n')
				{
					buffer[i] = 0;
					break;
				}
			device->Print(WPoint(m_pItem->GetRect().x, m_pItem->GetRect().y) +
					WPoint(25.f, 155.f),
				0, "%s", buffer);
		}
	}
}

IMPLEMENT_OBJECT(FrOpenCardPackResultDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrOpenCardPackResultDlg, FrForm)
ON_FRESH_VI("item", FRCMD_INIT, FrOpenCardPackResultDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrOpenCardPackResultDlg::OnItemOwnerDraw)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrOpenCardPackResultDlg::OnItemOwnerDraw)
ON_FRESH_VI("cardpack_frm_num", FRCMD_INIT,
	FrOpenCardPackResultDlg::OnCardFrmNumInit)
END_FRESH_MSGMAP()

FrOpenCardPackResultDlg::FrOpenCardPackResultDlg()
{
	for (int i = 0; i < 3; ++i)
		m_cardTypeId[i] = 0;
}

FrOpenCardPackResultDlg::~FrOpenCardPackResultDlg()
{
}

void FrOpenCardPackResultDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

void FrOpenCardPackResultDlg::OnCardFrmNumInit(int param)
{
	m_pCardFrmNum = DYNAMIC_CAST(FrArea, param);
	if (m_cardTypeId[1] && m_cardTypeId[2] && m_cardTypeId[3])
		m_pCardFrmNum->SetBgImg("cardpack_frm_num_3");
	else
		m_pCardFrmNum->SetBgImg("cardpack_frm_num_1");
}

void FrOpenCardPackResultDlg::OnItemOwnerDraw(int param)
{
	char buffer[512];
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	if (m_cardTypeId[0])
	{
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId[0]);
		if (!card)
			return;
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", card->c.Icon);
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 30.f, m_pItem->GetRect().y + 85.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		device->SetTextColor(0xff000000, 0xffffffff);
		device->PrintText(
			WPoint(m_pItem->GetRect().x + 16.f, m_pItem->GetRect().y + 160.f),
			0, MakeStr("%s", card->c.Name), NULL);
	}
	if (m_cardTypeId[1])
	{
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId[1]);
		if (!card)
			return;
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", card->c.Icon);
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 126.f, m_pItem->GetRect().y + 94.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
	}
	if (m_cardTypeId[2] && m_cardTypeId[3])
	{
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId[2]);
		if (!card)
			return;
		const Bitmap* bitmap =
			g_pFresh->GetManager()->GetBitmap("ITEMS", card->c.Icon);
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 178.f, m_pItem->GetRect().y + 94.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		card = ItemManager()->FindCard(m_cardTypeId[3]);
		if (!card)
			return;
		bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", card->c.Icon);
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(m_pItem->GetRect().x + 230.f, m_pItem->GetRect().y + 94.f,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
	}
}

IMPLEMENT_OBJECT(FrCardAttachDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrCardAttachDlg, FrForm)
ON_FRESH_VI("item", FRCMD_INIT, FrCardAttachDlg::OnItemInit)
ON_FRESH_VI("item", FRCMD_OWNERDRAW, FrCardAttachDlg::OnItemOwnerDraw)
ON_FRESH_VI("cardavility", FRCMD_INIT, FrCardAttachDlg::OnCardAvilityInit)
ON_FRESH_VI("cardavilityold", FRCMD_INIT, FrCardAttachDlg::OnCardAvilityOldInit)
END_FRESH_MSGMAP()

FrCardAttachDlg::FrCardAttachDlg()
{
	m_cardTypeId = 0;
	m_cardId = 0;
	m_reserved1 = 0;
	m_reserved2 = 0;
	m_charSlot = 0;
	m_caddieSlot = 0;
	m_pCardAvility = NULL;
	m_pCardAvilityOld = NULL;
}

FrCardAttachDlg::~FrCardAttachDlg()
{
}

bool FrCardAttachDlg::OnSetDlgTxt()
{
	if (!m_cardTypeId)
		return false;
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_cardTypeId);
	if (!card)
		return false;
	char buffer[512] = "";
	unsigned long type = (m_cardTypeId >> 22) & 15;
	if (type == 0)
	{
		for (int i = 0; i < 5; ++i)
			if (card->COM[i] > 0)
			{
				if (i == 0)
					sprintf(buffer, "%s + %d",
						"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4", card->COM[0]);
				else if (i == 1)
					sprintf(buffer, "%s + %d",
						"\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4",
						card->COM[1]);
				else if (i == 2)
					sprintf(buffer, "%s + %d",
						"\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4",
						card->COM[2]);
				else if (i == 3)
					sprintf(buffer, "%s + %d",
						"\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4", card->COM[3]);
				else if (i == 4)
					sprintf(buffer, "%s + %d",
						"\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4", card->COM[4]);
				m_pCardAvility->AddText(MakeStr("\\cT\\c%s\\cN\\c", buffer),
					false, true);
			}
	}
	else if (type == 1)
	{
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_cardTypeId);
		if (desc)
		{
			strcpy(buffer, desc->Desc);
			for (int i = 0; i < 512; ++i)
				if (buffer[i] == '\n')
				{
					buffer[i] = 0;
					break;
				}
			m_pCardAvility->AddText(MakeStr("\\cT\\c%s\\cN\\c", buffer), false,
				true);
		}
	}
	if (m_charSlot > 0 || m_caddieSlot > 0)
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			unsigned long type = ((*it).tid >> 22) & 15;
			if ((*it).partsTid == m_itemTypeId && (*it).partsUid == m_itemId)
			{
				if (type == 0)
				{
					if (m_charSlot == (*it).slotNum)
					{
						IFF_STRUCT::sCard* card =
							ItemManager()->FindCard((*it).tid);
						if (card)
							for (int i = 0; i < 5; ++i)
								if (card->COM[i] > 0)
								{
									memset(buffer, 0, sizeof(buffer));
									if (i == 0)
										sprintf(buffer, "%s + %d",
											"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4",
											card->COM[0]);
									else if (i == 1)
										sprintf(buffer, "%s + %d",
											"\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4",
											card->COM[1]);
									else if (i == 2)
										sprintf(buffer, "%s + %d",
											"\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4",
											card->COM[2]);
									else if (i == 3)
										sprintf(buffer, "%s + %d",
											"\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4",
											card->COM[3]);
									else if (i == 4)
										sprintf(buffer, "%s + %d",
											"\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4",
											card->COM[4]);
									m_pCardAvilityOld->AddText(
										MakeStr("\\cT\\c%s\\cN\\c", buffer),
										false, true);
								}
					}
				}
				else if (type == 1 && m_caddieSlot == (*it).slotNum)
				{
					IFF_STRUCT::sDesc* desc =
						ItemManager()->FindDesc((*it).tid);
					if (desc)
					{
						strcpy(buffer, desc->Desc);
						for (int i = 0; i < 512; ++i)
							if (buffer[i] == '\n')
							{
								buffer[i] = 0;
								break;
							}
						m_pCardAvilityOld->AddText(
							MakeStr("\\cT\\c%s\\cN\\c", buffer), false, true);
					}
				}
			}
		}
	return true;
}

void FrCardAttachDlg::OnCardAvilityInit(int param)
{
	m_pCardAvility = DYNAMIC_CAST(FrEdit, param);
	m_pCardAvility->SetVisible(true);
}

void FrCardAttachDlg::OnCardAvilityOldInit(int param)
{
	m_pCardAvilityOld = DYNAMIC_CAST(FrEdit, param);
	m_pCardAvilityOld->SetVisible(true);
}

void FrCardAttachDlg::OnItemInit(int param)
{
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

void FrCardAttachDlg::OnItemOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
}

CTFrameMove::CTFrameMove()
	: m_frameCount(3)
{
	m_frames.reserve(m_frameCount);
}

CTFrameMove::CTFrameMove(unsigned char frameCount)
	: m_frameCount(frameCount)
{
	m_frames.reserve(frameCount);
}

CTFrameMove::~CTFrameMove()
{
	Reset();
}

void CTFrameMove::AddFrame(FrArea* frame)
{
	if (frame)
		m_frames.push_back(frame);
}

bool CTFrameMove::SetFrameWidth(float width)
{
	if (0.0f == width)
		return false;
	unsigned char count = (unsigned char)m_frames.size();
	unsigned char first, last;
	if (count == 3)
	{
		first = 3;
		last = 6;
	}
	else if (count == 9)
	{
		first = 0;
		last = 3;
	}
	for (unsigned char i = first; i < last; ++i)
	{
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.w += width;
		m_frames[i]->SetRect(rect);
	}
	return true;
}

bool CTFrameMove::SetFrameHeight(float height)
{
	if (0.0f == height)
		return false;
	unsigned char count = (unsigned char)m_frames.size();
	unsigned char first, middle, last;
	if (count == 6)
	{
		first = 0;
		middle = 3;
		last = 6;
	}
	for (unsigned char i = first; i < middle; ++i)
	{
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.h += height;
		m_frames[i]->SetRect(rect);
	}
	for (unsigned char j = middle; j < last; ++j)
	{
		WRect rect;
		rect = m_frames[j]->GetRect();
		rect.y += height;
		m_frames[j]->SetRect(rect);
	}
	return true;
}

IMPLEMENT_OBJECT(FrItemAttachCardDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrItemAttachCardDlg, FrForm)
ON_FRESH_VI("attach_item_list", FRCMD_INIT,
	FrItemAttachCardDlg::OnMyItemListInit)
ON_FRESH_VV("attach_item_list", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnMyItemListLClick)
ON_FRESH_VV("attach_item_list", FRCMD_RBUTTONDOWN,
	FrItemAttachCardDlg::OnMyItemListRClick)
ON_FRESH_VI("attach_item_list", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnMyItemListOwnerDraw)
ON_FRESH_VI("attach_card_icon", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachCardIconInit)
ON_FRESH_VV("attach_card_icon", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnAttachCardIconOwnerDraw)
ON_FRESH_VI("attach_card_name", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachCardNameInit)
ON_FRESH_VI("attach_card_ability", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachCardAbilityInit)
ON_FRESH_VI("attach_item_icon", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachItemIconInit)
ON_FRESH_VV("attach_item_icon", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnAttachItemIconOwnerDraw)
ON_FRESH_VI("attach_item_name", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachItemNameInit)
ON_FRESH_VI("attach_item_ability", FRCMD_INIT,
	FrItemAttachCardDlg::OnAttachItemAbilityInit)
ON_FRESH_VI("attach_char_img", FRCMD_INIT,
	FrItemAttachCardDlg::OnAreaCharImgInit)
ON_FRESH_VI("foldbtn", FRCMD_INIT, FrItemAttachCardDlg::OnItemListFoldInit)
ON_FRESH_VV("foldbtn", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnItemListFoldLClick)
ON_FRESH_VI("unfoldbtn", FRCMD_INIT, FrItemAttachCardDlg::OnItemListUnfoldInit)
ON_FRESH_VV("unfoldbtn", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnItemListUnfoldLClick)
ON_FRESH_VI("card_frm_04", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_04Init)
ON_FRESH_VI("card_frm_05", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_05Init)
ON_FRESH_VI("card_frm_06", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_06Init)
ON_FRESH_VI("card_frm_07", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_07Init)
ON_FRESH_VI("card_frm_08", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_08Init)
ON_FRESH_VI("card_frm_09", FRCMD_INIT, FrItemAttachCardDlg::OnDlgFrm_09Init)
ON_FRESH_VI("charbtn_1", FRCMD_INIT, FrItemAttachCardDlg::OnCharSlotBtnInit_1)
ON_FRESH_VV("charbtn_1", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCharSlotBtnDown_1)
ON_FRESH_VV("charbtn_1", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_1)
ON_FRESH_VI("charbtn_2", FRCMD_INIT, FrItemAttachCardDlg::OnCharSlotBtnInit_2)
ON_FRESH_VV("charbtn_2", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCharSlotBtnDown_2)
ON_FRESH_VV("charbtn_2", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_2)
ON_FRESH_VI("charbtn_3", FRCMD_INIT, FrItemAttachCardDlg::OnCharSlotBtnInit_3)
ON_FRESH_VV("charbtn_3", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCharSlotBtnDown_3)
ON_FRESH_VV("charbtn_3", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_3)
ON_FRESH_VI("charbtn_4", FRCMD_INIT, FrItemAttachCardDlg::OnCharSlotBtnInit_4)
ON_FRESH_VV("charbtn_4", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCharSlotBtnDown_4)
ON_FRESH_VV("charbtn_4", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_4)
ON_FRESH_VI("caddiebtn_1", FRCMD_INIT,
	FrItemAttachCardDlg::OnCaddieSlotBtnInit_1)
ON_FRESH_VV("caddiebtn_1", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCaddieSlotBtnDown_1)
ON_FRESH_VV("caddiebtn_1", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_1)
ON_FRESH_VI("caddiebtn_2", FRCMD_INIT,
	FrItemAttachCardDlg::OnCaddieSlotBtnInit_2)
ON_FRESH_VV("caddiebtn_2", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCaddieSlotBtnDown_2)
ON_FRESH_VV("caddiebtn_2", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_2)
ON_FRESH_VI("caddiebtn_3", FRCMD_INIT,
	FrItemAttachCardDlg::OnCaddieSlotBtnInit_3)
ON_FRESH_VV("caddiebtn_3", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCaddieSlotBtnDown_3)
ON_FRESH_VV("caddiebtn_3", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_3)
ON_FRESH_VI("caddiebtn_4", FRCMD_INIT,
	FrItemAttachCardDlg::OnCaddieSlotBtnInit_4)
ON_FRESH_VV("caddiebtn_4", FRCMD_LBUTTONDOWN,
	FrItemAttachCardDlg::OnCaddieSlotBtnDown_4)
ON_FRESH_VV("caddiebtn_4", FRCMD_OWNERDRAW,
	FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_4)
END_FRESH_MSGMAP()

FrItemAttachCardDlg::FrItemAttachCardDlg()
{
	m_pMyItemList = NULL;
	m_pCardIcon = NULL;
	m_pCardName = NULL;
	m_pCardAbility = NULL;
	m_pItemIcon = NULL;
	m_pItemName = NULL;
	m_pItemAbility = NULL;
	for (int i = 0; i < 4; ++i)
	{
		m_pCharSlotBtn[i] = NULL;
		m_pCaddieSlotBtn[i] = NULL;
	}
	m_itemTypeId = 0;
	m_itemId = 0;
	m_pLine = g_resrcmng->GetOverlay("line.tga", 0);
	m_pCardAttachDlg = NULL;
	m_pItemClickBmp = g_pFresh->GetBitmap("trade_myitem_base_click");
	m_pItemOverBmp = g_pFresh->GetBitmap("trade_myitem_base_over");
	m_myItemList.clear();
	m_pSelectItem = NULL;
	initCardSlotBtnTxt();
	m_selectSlot = 0xff;
	m_bFold = false;
	m_pCharEmptyBmp = g_pFresh->GetBitmap("card_character_empty");
	m_pCaddieEmptyBmp = g_pFresh->GetBitmap("card_caddie_empty");
	m_pUccOriginalBmp = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopiedBmp = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSavedBmp = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
}

FrItemAttachCardDlg::~FrItemAttachCardDlg()
{
	if (g_resrcmng && m_pLine)
	{
		g_resrcmng->Release(m_pLine);
		m_pLine = NULL;
	}
}

bool FrItemAttachCardDlg::OnInit()
{
	EnableDrag(false);
	SetListCardUsableMyItem();
	SetListMyItem();
	SetSelectMyItem(NULL);
	SetCardInfo(m_cardTypeId);
	return true;
}

void FrItemAttachCardDlg::initCardSlotBtnTxt()
{
	memset(m_charSlotTxt[0], 0, sizeof(m_charSlotTxt[0]));
	memset(m_charSlotTxt[1], 0, sizeof(m_charSlotTxt[1]));
	memset(m_charSlotTxt[2], 0, sizeof(m_charSlotTxt[2]));
	memset(m_charSlotTxt[3], 0, sizeof(m_charSlotTxt[3]));
	memset(m_caddieSlotTxt[0], 0, sizeof(m_caddieSlotTxt[0]));
	memset(m_caddieSlotTxt[1], 0, sizeof(m_caddieSlotTxt[1]));
	memset(m_caddieSlotTxt[2], 0, sizeof(m_caddieSlotTxt[2]));
	memset(m_caddieSlotTxt[3], 0, sizeof(m_caddieSlotTxt[3]));

	for (int i = 0; i < 4; ++i)
	{
		m_bCharSlotAttached[i] = false;
		m_bCaddieSlotAttached[i] = false;
		m_charSlotCardTypeId[i] = 0;
		m_caddieSlotCardTypeId[i] = 0;
	}
}

void FrItemAttachCardDlg::OnAttachCardIconInit(int param)
{
	m_pCardIcon = DYNAMIC_CAST(FrArea, param);
}

void FrItemAttachCardDlg::OnAttachCardIconOwnerDraw()
{
	if (m_cardTypeId)
	{
		WPoint pos(m_pCardIcon->GetRect().x, m_pCardIcon->GetRect().y);
		DrawItem(pos, m_cardTypeId, m_cardId);
	}
}

void FrItemAttachCardDlg::OnAttachCardNameInit(int param)
{
	m_pCardName = DYNAMIC_CAST(FrEdit, param);
}

void FrItemAttachCardDlg::OnAttachCardAbilityInit(int param)
{
	m_pCardAbility = DYNAMIC_CAST(FrEdit, param);
}

void FrItemAttachCardDlg::OnAttachItemIconInit(int param)
{
	m_pItemIcon = DYNAMIC_CAST(FrArea, param);
}

void FrItemAttachCardDlg::OnAttachItemIconOwnerDraw()
{
	if (m_pSelectItem)
	{
		WPoint pos = WPoint(m_pItemIcon->GetRect().x, m_pItemIcon->GetRect().y);
		DrawItem(pos, m_pSelectItem->typeId, m_pSelectItem->id);
		DrawPartsSlot(pos, m_pSelectItem->typeId, m_pSelectItem->id, NULL);
	}
}

void FrItemAttachCardDlg::OnAttachItemNameInit(int param)
{
	m_pItemName = DYNAMIC_CAST(FrEdit, param);
}

void FrItemAttachCardDlg::OnAttachItemAbilityInit(int param)
{
	m_pItemAbility = DYNAMIC_CAST(FrEdit, param);
}

void FrItemAttachCardDlg::OnAreaCharImgInit(int param)
{
	m_pCharImg = DYNAMIC_CAST(FrArea, param);
}

void FrItemAttachCardDlg::OnItemListFoldInit(int param)
{
	m_pFoldBtn = DYNAMIC_CAST(FrButton, param);
	m_pFoldBtn->SetVisible(true);
}

void FrItemAttachCardDlg::OnItemListUnfoldInit(int param)
{
	m_pUnfoldBtn = DYNAMIC_CAST(FrButton, param);
	m_pUnfoldBtn->SetVisible(false);
}

void FrItemAttachCardDlg::OnItemListFoldLClick()
{
	WRect rect;
	m_pMyItemList->GetClientRect(rect);
	rect.h += 100.f;
	m_pMyItemList->SetClientRect(rect);
	m_frameMove.SetFrameHeight(+100.f);
	m_pFoldBtn->SetVisible(false);
	m_pUnfoldBtn->SetVisible(true);
	m_bFold = true;
}

void FrItemAttachCardDlg::OnItemListUnfoldLClick()
{
	WRect rect;
	m_pMyItemList->GetClientRect(rect);
	rect.h -= 100.f;
	m_pMyItemList->SetClientRect(rect);
	m_frameMove.SetFrameHeight(-100.f);
	m_pFoldBtn->SetVisible(true);
	m_pUnfoldBtn->SetVisible(false);
	m_bFold = false;
}

void FrItemAttachCardDlg::OnDlgFrm_04Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

void FrItemAttachCardDlg::OnDlgFrm_05Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

void FrItemAttachCardDlg::OnDlgFrm_06Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

void FrItemAttachCardDlg::OnDlgFrm_07Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

void FrItemAttachCardDlg::OnDlgFrm_08Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

void FrItemAttachCardDlg::OnDlgFrm_09Init(int param)
{
	m_frameMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

bool FrItemAttachCardDlg::OnCardAttachResult(int result, FrForm* form)
{
	if (result == FrOK)
		ResetCardSlotInfo();
	else if (result == FrCANCEL)
	{
		SetPartsSlotBtnTxt();
		m_selectSlot = 0xff;
	}
	m_pCardAttachDlg = NULL;
	return true;
}

void FrItemAttachCardDlg::OnOpenAttachDlg(int charSlot, int caddieSlot)
{
	m_pCardAttachDlg = CreateForm<FrCardAttachDlg>(g_pFresh->GetManager(), this,
		"attach_card", NULL);
	m_pCardAttachDlg->m_cardTypeId = m_cardTypeId;
	m_pCardAttachDlg->m_cardId = m_cardId;
	m_pCardAttachDlg->m_itemTypeId = m_itemTypeId;
	m_pCardAttachDlg->m_itemId = m_itemId;
	m_pCardAttachDlg->m_charSlot = charSlot;
	m_pCardAttachDlg->m_caddieSlot = caddieSlot;
	m_pCardAttachDlg->OnSetDlgTxt();
	m_pCardAttachDlg->Open(
		(FRESH_PFN_RESULT)&FrItemAttachCardDlg::OnAgreeAttachingResult,
		FrESCAPE);
}

void FrItemAttachCardDlg::SetCharacterAttatchBtnTxt(unsigned long tid,
	char* text)
{
	if (!m_cardTypeId)
		strcpy(text, "Card Info error...");
	char result[50] = "";
	char buffer[255] = "";
	int count = 0;
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(tid);
	if (card)
		for (int i = 0; i < 5; ++i)
			if (card->COM[i] > 0)
			{
				if (count == 1)
					strcat(result, ",");
				memset(buffer, 0, sizeof(buffer));
				if (i == 0)
				{
					sprintf(buffer, "%s + %d",
						"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4", card->COM[0]);
					strcat(result, buffer);
					++count;
				}
				else if (i == 1)
				{
					sprintf(buffer, "%s + %d",
						"\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4",
						card->COM[1]);
					strcat(result, buffer);
					++count;
				}
				else if (i == 2)
				{
					sprintf(buffer, "%s + %d",
						"\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4",
						card->COM[2]);
					strcat(result, buffer);
					++count;
				}
				else if (i == 3)
				{
					sprintf(buffer, "%s + %d",
						"\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4", card->COM[3]);
					strcat(result, buffer);
					++count;
				}
				else if (i == 4)
				{
					sprintf(buffer, "%s + %d",
						"\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4", card->COM[4]);
					strcat(result, buffer);
					++count;
				}
			}
	strcpy(text, result);
}

void FrItemAttachCardDlg::SetCaddieAttatchBtnTxt(unsigned long tid, char* text)
{
	if (!m_cardTypeId)
		strcpy(text, "Card Info error...");
	char buffer[512] = "";
	IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(tid);
	if (desc)
	{
		strcpy(buffer, desc->Desc);
		for (int i = 0; i < 512; ++i)
			if (buffer[i] == '\n')
			{
				buffer[i] = 0;
				break;
			}
	}
	strncpy(text, buffer, 49);
}

void FrItemAttachCardDlg::DrawBtnCharSlotimg(float x, float y,
	unsigned long tid)
{
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(tid);
	if (card)
	{
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		if (device)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
			if (bitmap)
				device->DrawTexture(bitmap,
					WRect(x + 5.f, y + 4.f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
}

void FrItemAttachCardDlg::DrawBtnCaddieSlotimg(float x, float y,
	unsigned long tid)
{
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(tid);
	if (card)
	{
		FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
		if (device)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap(MakeStr(card->SlotImg));
			if (bitmap)
				device->DrawTexture(bitmap,
					WRect(x + 5.f, y + 4.f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
}

void FrItemAttachCardDlg::OnCharSlotBtnInit_1(int param)
{
	m_pCharSlotBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[0]->Enable(false);
}

void FrItemAttachCardDlg::OnCharSlotBtnDown_1()
{
	if (m_cardType == 0)
	{
		OnOpenAttachDlg(1, 0);
		m_selectSlot = 1;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb8\xaf\xc5\xcd \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_1()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[0]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_charSlotTxt[0]);
	if (!m_bCharSlotAttached[0])
	{
		const Bitmap* bitmap = m_pCharEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCardTypeId[0]);
}

void FrItemAttachCardDlg::OnCharSlotBtnInit_2(int param)
{
	m_pCharSlotBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[1]->Enable(false);
}

void FrItemAttachCardDlg::OnCharSlotBtnDown_2()
{
	if (m_cardType == 0)
	{
		OnOpenAttachDlg(2, 0);
		m_selectSlot = 2;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb8\xaf\xc5\xcd \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_2()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[1]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_charSlotTxt[1]);
	if (!m_bCharSlotAttached[1])
	{
		const Bitmap* bitmap = m_pCharEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCardTypeId[1]);
}

void FrItemAttachCardDlg::OnCharSlotBtnInit_3(int param)
{
	m_pCharSlotBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[2]->Enable(false);
}

void FrItemAttachCardDlg::OnCharSlotBtnDown_3()
{
	if (m_cardType == 0)
	{
		OnOpenAttachDlg(3, 0);
		m_selectSlot = 3;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb8\xaf\xc5\xcd \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_3()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[2]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_charSlotTxt[2]);
	if (!m_bCharSlotAttached[2])
	{
		const Bitmap* bitmap = m_pCharEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCardTypeId[2]);
}

void FrItemAttachCardDlg::OnCharSlotBtnInit_4(int param)
{
	m_pCharSlotBtn[3] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[3]->Enable(false);
}

void FrItemAttachCardDlg::OnCharSlotBtnDown_4()
{
	if (m_cardType == 0)
	{
		OnOpenAttachDlg(4, 0);
		m_selectSlot = 4;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb8\xaf\xc5\xcd \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_4()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[3]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_charSlotTxt[3]);
	if (!m_bCharSlotAttached[3])
	{
		const Bitmap* bitmap = m_pCharEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCardTypeId[3]);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnInit_1(int param)
{
	m_pCaddieSlotBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[0]->Enable(false);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnDown_1()
{
	if (m_cardType == 1)
	{
		OnOpenAttachDlg(0, 1);
		m_selectSlot = 5;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb5\xf0 \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_1()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[0]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_caddieSlotTxt[0]);
	if (!m_bCaddieSlotAttached[0])
	{
		const Bitmap* bitmap = m_pCaddieEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCardTypeId[0]);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnInit_2(int param)
{
	m_pCaddieSlotBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[1]->Enable(false);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnDown_2()
{
	if (m_cardType == 1)
	{
		OnOpenAttachDlg(0, 2);
		m_selectSlot = 6;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb5\xf0 \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_2()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[1]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_caddieSlotTxt[1]);
	if (!m_bCaddieSlotAttached[1])
	{
		const Bitmap* bitmap = m_pCaddieEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCardTypeId[1]);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnInit_3(int param)
{
	m_pCaddieSlotBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[2]->Enable(false);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnDown_3()
{
	if (m_cardType == 1)
	{
		OnOpenAttachDlg(0, 3);
		m_selectSlot = 7;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb5\xf0 \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_3()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[2]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_caddieSlotTxt[2]);
	if (!m_bCaddieSlotAttached[2])
	{
		const Bitmap* bitmap = m_pCaddieEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCardTypeId[2]);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnInit_4(int param)
{
	m_pCaddieSlotBtn[3] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[3]->Enable(false);
}

void FrItemAttachCardDlg::OnCaddieSlotBtnDown_4()
{
	if (m_cardType == 1)
	{
		OnOpenAttachDlg(0, 4);
		m_selectSlot = 8;
	}
	else
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xc4\xb3\xb5\xf0 \xc4\xab\xb5\xe5\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, FrESCAPE | FrENTER);
	}
}

void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_4()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[3]->GetRect();
	device->SetTextColor(0xff000000, 0xffffffff);
	device->Print11(WPoint((rect.x + 20.f), rect.y + 6.f), 0, "%s",
		m_caddieSlotTxt[3]);
	if (!m_bCaddieSlotAttached[3])
	{
		const Bitmap* bitmap = m_pCaddieEmptyBmp;
		if (bitmap)
			device->DrawTexture(bitmap,
				WRect(rect.x + 5.f, rect.y + 4.f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCardTypeId[3]);
}

void FrItemAttachCardDlg::OnProc(const float delta)
{
}

void FrItemAttachCardDlg::SetListMyItem()
{
	if (m_pMyItemList)
	{
		m_pMyItemList->ClearItem();
		for (std::list<sItemAttachCard>::iterator it = m_myItemList.begin();
			it != m_myItemList.end(); it++)
			m_pMyItemList->AddItem(&*it);
	}
}

void FrItemAttachCardDlg::OnMyItemListInit(int param)
{
	m_pMyItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pMyItemList)
		m_pMyItemList->UseRightButton(true);
	SetDesc(
		"\xc0\xc7\xbb\xf3\xbf\xa1 \xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
}

void FrItemAttachCardDlg::OnMyItemListLClick()
{
	FrListItem* item = m_pMyItemList->GetItemUnderCursor();
	if (item)
	{
		m_selectSlot = 0xff;
		if (!item->selected)
			m_pMyItemList->ToggleItem(item);
		else
			SetSelectMyItem((sItemAttachCard*)item->pData);
	}
}

void FrItemAttachCardDlg::OnMyItemListRClick()
{
	WPoint pos = GetRect().TopLeft();
}

void FrItemAttachCardDlg::OnMyItemListOwnerDraw(int param)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	sItemAttachCard* item = (sItemAttachCard*)row->pData;
	const Bitmap* bitmap = NULL;
	if (row->selected)
	{
		bitmap = m_pItemClickBmp;
		device->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (row->underCursor)
	{
		bitmap = m_pItemOverBmp;
		device->SetTextColor(0xff000000, 0xffffffff);
	}
	else
		device->SetTextColor(0xff000000, 0xffffffff);
	if (bitmap)
		device->DrawTexture(bitmap,
			WRect(row->pos.x + 2.f, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	if ((item->typeId >> 26) == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->typeId >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* bitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (bitmap)
				device->DrawTexture(bitmap,
					WRect(row->pos.x + 7.f, row->pos.y + 5.f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
		}
	}
	WPoint pos(row->pos.x + 13.f, row->pos.y + 20.f);
	DrawItem(pos, item->typeId, item->id);
	DrawPartsSlot(pos, item->typeId, item->id, device);
	DrawPartsLine();
}

void FrItemAttachCardDlg::SetListCardUsableMyItem()
{
	m_myItemList.clear();
	sItemAttachCard item = { 0, 0 };
	for (int character = 0; character < 10; ++character)
	{
		std::list<sItemInfo>& parts = Doc()->m_partsList;
		for (std::list<sItemInfo>::iterator it = parts.begin();
			it != parts.end(); it++)
		{
			if ((*it).ItemType == 6 && (*it).Expired)
				continue;
			if (character != (((*it).tid >> 18) & 0xff))
				continue;
			IFF_STRUCT::sPart* part = ItemManager()->FindPart((*it).tid);
			if (part && (part->CaddieSlot > 0 || part->CharacterSlot > 0))
			{
				item.id = (*it).guid;
				item.typeId = (*it).tid;
				m_myItemList.push_back(item);
			}
		}
		std::list<sItemInfo>& ucc = Doc()->m_uccItemList;
		for (std::list<sItemInfo>::iterator it = ucc.begin(); it != ucc.end();
			it++)
		{
			if (character != (((*it).tid >> 18) & 0xff))
				continue;
			IFF_STRUCT::sPart* part = ItemManager()->FindPart((*it).tid);
			if (part && ((*it).status & 1))
			{
				if (UccManager()->FindClothes((*it).tid, (*it).UccIndex) &&
					!UccManager()->IsDownloadComplete((*it).tid,
						(*it).UccIndex) &&
					!NetResourceManager::Instance()->IsDownloadRequested(
						RESOURCE_UCC_CLOTHES, (*it).tid, (*it).UccIndex))
					continue;
				if ((*it).status & 1)
				{
					item.id = (*it).guid;
					item.typeId = (*it).tid;
					m_myItemList.push_back(item);
				}
			}
		}
	}
	SetListEquipParts();
	std::list<sItemAttachCard>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end();)
		if (IsEquipParts((*it).typeId, (*it).id))
			it = m_myItemList.erase(it);
		else
			it++;
}

void FrItemAttachCardDlg::DrawItem(WPoint pos, unsigned long tid,
	unsigned long id)
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device || !tid)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(tid);
	if (!common)
		return;
	float x = 0.f, y = 0.f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent(S4_UCC) && IsUccClothes(tid))
		bitmap = GetUccItemIcon(id);
	unsigned long type = tid >> 26;
	if (type == 2)
	{
		x = -12.f;
		y = -20.f;
	}
	if (bitmap)
	{
		device->DrawTexture(bitmap,
			WRect(pos.x + x + 14.f, pos.y + y + 11.f, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
		device->SetTextColor(0xff000000, 0xffffffff);
		device->SetTextStyle(4);
	}
	if (IsLocalContent(S4_UCC) && IsUccClothes(tid))
	{
		sItemInfo* item = GetMyItemInfo(id);
		if (item)
		{
			WRect rect(pos.x + 47.f, pos.y + 43.f,
				(float)m_pUccOriginalBmp->Width(),
				(float)m_pUccOriginalBmp->Height());
			if ((item->status & 1) && item->Seq == 1 && m_pUccOriginalBmp)
				device->DrawTexture(m_pUccOriginalBmp, rect, 0xffffffff, 0);
			else if ((item->status & 1) && item->Seq > 1 && m_pUccCopiedBmp)
				device->DrawTexture(m_pUccCopiedBmp, rect, 0xffffffff, 0);
			else if ((item->status & 2) && m_pUccTempSavedBmp)
				device->DrawTexture(m_pUccTempSavedBmp, rect, 0xffffffff, 0);
		}
	}
	switch (type)
	{
	case 2:
	{
		const Bitmap* level =
			g_pFresh->GetBitmap(MakeStr("level_%03d", common->Level + 1));
		if (level)
			device->DrawTexture(level,
				WRect(pos.x + 38.f - float2int((float)level->Width() * 0.5f),
					pos.y + 35.f - float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
		std::list<sItemInfo>& parts = Doc()->m_partsList;
		std::list<sItemInfo>::iterator it =
			std::find_if(parts.begin(), parts.end(), _FindByGuid(id));
		if (it != parts.end() && (*it).ItemType == 6)
		{
			const Bitmap* clock = g_pFresh->GetBitmap("clock");
			device->DrawTexture(clock,
				WRect(pos.x + 60.f, pos.y - 15.f, clock->Width(),
					clock->Height()),
				0xffffffff, 0);
		}
	}
	break;
	}
}

void FrItemAttachCardDlg::SetCardInfo(unsigned long tid)
{
	if (!tid)
		return;
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(tid);
	if (card && (tid >> 26) == 31)
	{
		if (m_pCardName)
			m_pCardName->AddText(MakeStr("\\c0xff000000\\c%s", card->c.Name),
				false, true);
		if (m_pCardAbility)
			for (int i = 0; i < 5; ++i)
				if (card->COM[i] > 0)
				{
					char buffer[512];
					memset(buffer, 0, sizeof(buffer));
					if (i == 0)
						sprintf(buffer, "%s + %d",
							"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4", card->COM[0]);
					else if (i == 1)
						sprintf(buffer, "%s + %d",
							"\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4",
							card->COM[1]);
					else if (i == 2)
						sprintf(buffer, "%s + %d",
							"\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4",
							card->COM[2]);
					else if (i == 3)
						sprintf(buffer, "%s + %d",
							"\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4", card->COM[3]);
					else if (i == 4)
						sprintf(buffer, "%s + %d",
							"\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4", card->COM[4]);
					m_pCardAbility->AddText(
						MakeStr("\\c0xff0000ff\\c%s", buffer), false, true);
				}
	}
}

void FrItemAttachCardDlg::SetListEquipParts()
{
	m_equipPartsList.clear();
	for (CHAR_MAP_IT it = Doc()->m_charMap.begin();
		it != Doc()->m_charMap.end(); ++it)
		for (int i = 0; i < 24; ++i)
			if ((*it).second.tidParts[i])
			{
				sItemAttachCard item;
				item.typeId = (*it).second.tidParts[i];
				item.id = (*it).second.ItemIdList[i];
				m_equipPartsList.push_back(item);
			}
}

bool FrItemAttachCardDlg::IsEquipParts(unsigned long tid, unsigned long id)
{
	bool found = false;
	for (std::list<sItemAttachCard>::iterator it = m_equipPartsList.begin();
		it != m_equipPartsList.end(); it++)
		if (tid == (*it).typeId && id == (*it).id)
		{
			m_equipPartsList.erase(it);
			found = true;
			break;
		}
	return found;
}

void FrItemAttachCardDlg::SetSelectMyItem(sItemAttachCard* item)
{
	m_pItemName->ClearLine();
	m_pItemAbility->ClearLine();
	m_pSelectItem = item;
	if (item)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->typeId);
		if (common && m_pItemName)
		{
			m_itemTypeId = item->typeId;
			m_itemId = item->id;
			if (IsLocalContent(S4_UCC) && IsUccClothes(common->TypeId))
				m_pItemName->AddText(
					MakeStr("\\c0xff000000\\c%s", GetUccItemName(item->id)),
					false, true);
			else
				m_pItemName->AddText(
					MakeStr("\\c0xff000000\\c%s", common->Name), false, true);
			switch (item->typeId >> 26)
			{
			case 2:
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->typeId);
				if (part)
				{
					short attr[5];
					memcpy(attr, part->Attr, sizeof(attr));
					short slot[5];
					memcpy(slot, part->Slot, sizeof(slot));
					const char* label = "";
					for (int i = 0, count = 5; count > 0; ++i, --count)
						if (attr[i] != 0 || slot[i] != 0)
						{
							switch (i)
							{
							case 0:
								label = "\xc6\xc4\xbf\xf6";
								break;
							case 1:
								label = "\xc4\xc1\xc6\xae\xb7\xd1";
								break;
							case 2:
								label = "\xc1\xa4\xc8\xae\xb5\xb5";
								break;
							case 3:
								label = "\xbd\xba\xc7\xc9";
								break;
							case 4:
								label = "\xc4\xbf\xba\xea";
								break;
							}
							if (attr[i] > 0 || slot[i] > 0)
							{
								if (attr[i] == 0)
									m_pItemAbility->AddText(
										MakeStr(
											"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
											label, slot[i]),
										false, true);
								else if (slot[i] == 0)
									m_pItemAbility->AddText(
										MakeStr(
											"\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
											label, attr[i]),
										false, true);
								else
									m_pItemAbility->AddText(
										MakeStr(
											"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
											label, attr[i], slot[i]),
										false, true);
							}
						}
				}
			}
			break;
			}
			if ((item->typeId >> 26) == 2)
			{
				IFF_STRUCT::sChar* character = ItemManager()->FindChar(
					((item->typeId >> 18) & 0xff) | 0x04000000);
				if (character)
					m_pCharImg->SetBgImg(MakeStr("s_%s", character->c.Icon));
			}
			SetPartsSlot(item->typeId);
			SetPartsSlotBtnTxt();
		}
	}
}

void FrItemAttachCardDlg::SetPartsSlot(unsigned long tid)
{
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(tid);
	for (int i = 0; i < 4; ++i)
	{
		m_pCharSlotBtn[i]->SetButtonImg("cd_slot_d", FrButton::NORMAL);
		m_pCharSlotBtn[i]->Enable(false);
		m_pCaddieSlotBtn[i]->SetButtonImg("cd_slot_d", FrButton::NORMAL);
		m_pCaddieSlotBtn[i]->Enable(false);
	}
	for (int i = 0; i < part->CharacterSlot; ++i)
	{
		m_pCharSlotBtn[i]->SetButtonImg("cd_slot_n", FrButton::NORMAL);
		m_pCharSlotBtn[i]->Enable(true);
	}
	for (int i = 0; i < part->CaddieSlot; ++i)
	{
		m_pCaddieSlotBtn[i]->SetButtonImg("cd_slot_n", FrButton::NORMAL);
		m_pCaddieSlotBtn[i]->Enable(true);
	}
}

void FrItemAttachCardDlg::SetPartsSlotBtnTxt()
{
	char buffer[512];
	initCardSlotBtnTxt();
	for (std::list<sSCardAvilityPeriodInfo>::iterator it =
			 Doc()->m_cardAbilityList[0].begin();
		it != Doc()->m_cardAbilityList[0].end(); ++it)
	{
		if ((*it).partsUid == m_itemId && (*it).partsTid == m_itemTypeId)
		{
			int slot = (*it).slotNum;
			IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
			if (card)
			{
				if ((*it).cardType == 0)
				{
					for (int i = 0; i < 5; ++i)
						if (card->COM[i] > 0)
							switch (slot)
							{
							case 1:
								m_bCharSlotAttached[0] = true;
								m_charSlotCardTypeId[0] = (*it).tid;
								SetCharacterAttatchBtnTxt((*it).tid,
									m_charSlotTxt[0]);
								break;
							case 2:
								m_bCharSlotAttached[1] = true;
								m_charSlotCardTypeId[1] = (*it).tid;
								SetCharacterAttatchBtnTxt((*it).tid,
									m_charSlotTxt[1]);
								break;
							case 3:
								m_bCharSlotAttached[2] = true;
								m_charSlotCardTypeId[2] = (*it).tid;
								SetCharacterAttatchBtnTxt((*it).tid,
									m_charSlotTxt[2]);
								break;
							case 4:
								m_bCharSlotAttached[3] = true;
								m_charSlotCardTypeId[3] = (*it).tid;
								SetCharacterAttatchBtnTxt((*it).tid,
									m_charSlotTxt[3]);
								break;
							}
				}
				else if ((*it).cardType == 1)
				{
					switch (slot)
					{
					case 1:
						m_bCaddieSlotAttached[0] = true;
						m_caddieSlotCardTypeId[0] = (*it).tid;
						SetCaddieAttatchBtnTxt((*it).tid, m_caddieSlotTxt[0]);
						break;
					case 2:
						m_bCaddieSlotAttached[1] = true;
						m_caddieSlotCardTypeId[1] = (*it).tid;
						SetCaddieAttatchBtnTxt((*it).tid, m_caddieSlotTxt[1]);
						break;
					case 3:
						m_bCaddieSlotAttached[2] = true;
						m_caddieSlotCardTypeId[2] = (*it).tid;
						SetCaddieAttatchBtnTxt((*it).tid, m_caddieSlotTxt[2]);
						break;
					case 4:
						m_bCaddieSlotAttached[3] = true;
						m_caddieSlotCardTypeId[3] = (*it).tid;
						SetCaddieAttatchBtnTxt((*it).tid, m_caddieSlotTxt[3]);
						break;
					}
				}
			}
		}
	}
}

void FrItemAttachCardDlg::DrawPartsSlot(WPoint pos, unsigned long tid,
	unsigned long id, FrGraphicInterface* device)
{
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(tid);
	if (!part)
		return;
	if (!device)
		device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	int slot;
	float charX = 5.f, caddieX = 5.f;
	for (slot = 1; slot < part->CharacterSlot + 1; ++slot)
	{
		bool attached = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (tid == (*it).partsTid && id == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 0)
				{
					attached = true;
					const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
					if (bitmap)
					{
						device->DrawTexture(bitmap,
							WRect(pos.x + charX, pos.y + 54.f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						charX += 16.f;
					}
				}
			}
		}
		if (!attached)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
			if (bitmap)
			{
				device->DrawTexture(bitmap,
					WRect(pos.x + charX, pos.y + 54.f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
				charX += 16.f;
			}
		}
	}
	for (slot = 1; slot < part->CaddieSlot + 1; ++slot)
	{
		bool attached = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (tid == (*it).partsTid && id == (*it).partsUid)
			{
				IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
				if (card && slot == (*it).slotNum && (*it).cardType == 1)
				{
					attached = true;
					const Bitmap* bitmap =
						g_pFresh->GetBitmap(MakeStr(card->SlotImg));
					if (bitmap)
					{
						device->DrawTexture(bitmap,
							WRect(pos.x + caddieX, pos.y + 70.f,
								(float)bitmap->Width(),
								(float)bitmap->Height()),
							0xffffffff, 0);
						caddieX += 16.f;
					}
				}
			}
		}
		if (!attached)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (bitmap)
			{
				device->DrawTexture(bitmap,
					WRect(pos.x + caddieX, pos.y + 70.f, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);
				caddieX += 16.f;
			}
		}
	}
}

void FrItemAttachCardDlg::DrawPartsLine()
{
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	float offset = 0.f;
	for (int i = 0; i < 3; ++i)
	{
		if (m_pLine)
		{
			m_pLine->Render(g_view, WRect(0.f, 0.f, 1.f, 1.f),
				WRect(offset + 307.f, 155.f, 1.f, 75.f), 0, 0xffffffff, 0.f, 0);
			if (m_bFold == true)
				m_pLine->Render(g_view, WRect(0.f, 0.f, 1.f, 1.f),
					WRect(offset + 307.f, 255.f, 1.f, 75.f), 0, 0xffffffff, 0.f,
					0);
		}
		offset += 91.f;
	}
}

void FrItemAttachCardDlg::AgreeAttaching()
{
	unsigned long tid;
	int slot = m_selectSlot > 4 ? m_selectSlot - 1 : m_selectSlot - 5;
	if (m_selectSlot > 4)
		tid = m_charSlotCardTypeId[slot];
	else
		tid = m_caddieSlotCardTypeId[slot];
	FrForm* form =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
	form->SetMessage(
		MakeStr(
			"\xb1\xe2\xc1\xb8\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc3\xb4\xf8 \xc4\xab\xb5\xe5(%s)\xc0\xc7 \xc8\xbf\xb0\xfa\xb0\xa1 \xbb\xe8\xc1\xa6\xb5\xcb\xb4\xcf\xb4\xd9. \xc1\xa4\xb8\xbb\xb7\xce \xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			ItemManager()->GetItemName(tid)),
		false);
	form->Open((FRESH_PFN_RESULT)&FrItemAttachCardDlg::OnCardAttachResult,
		FrESCAPE);
}

bool FrItemAttachCardDlg::OnAgreeAttachingResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		bool attached;
		int slot = m_selectSlot > 4 ? m_selectSlot - 1 : m_selectSlot - 5;
		if (m_selectSlot > 4)
			attached = m_bCharSlotAttached[slot];
		else
			attached = m_bCaddieSlotAttached[slot];
		if (attached)
			AgreeAttaching();
		else
			ResetCardSlotInfo();
	}
	else if (result == FrCANCEL)
	{
		SetPartsSlotBtnTxt();
		m_selectSlot = 0xff;
	}
	m_pCardAttachDlg = NULL;
	return true;
}

void FrItemAttachCardDlg::ResetCardSlotInfo()
{
	SetPartsSlotBtnTxt();
	switch (m_selectSlot)
	{
	case 1:
		m_bCharSlotAttached[0] = true;
		m_charSlotCardTypeId[0] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotTxt[0]);
		break;
	case 2:
		m_bCharSlotAttached[1] = true;
		m_charSlotCardTypeId[1] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotTxt[1]);
		break;
	case 3:
		m_bCharSlotAttached[2] = true;
		m_charSlotCardTypeId[2] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotTxt[2]);
		break;
	case 4:
		m_bCharSlotAttached[3] = true;
		m_charSlotCardTypeId[3] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotTxt[3]);
		break;
	case 5:
		m_bCaddieSlotAttached[0] = true;
		m_caddieSlotCardTypeId[0] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotTxt[0]);
		break;
	case 6:
		m_bCaddieSlotAttached[1] = true;
		m_caddieSlotCardTypeId[1] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotTxt[1]);
		break;
	case 7:
		m_bCaddieSlotAttached[2] = true;
		m_caddieSlotCardTypeId[2] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotTxt[2]);
		break;
	case 8:
		m_bCaddieSlotAttached[3] = true;
		m_caddieSlotCardTypeId[3] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotTxt[3]);
		break;
	}
}

WPoint FrSPCardBuffDlg::m_Pos(-1.f, 0.f);
unsigned long FrSPCardBuffDlg::m_SpecialBuffStartTick = 0;

IMPLEMENT_OBJECT(FrSPCardBuffDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrSPCardBuffDlg, FrForm)
ON_FRESH_VI("spcardlist", FRCMD_INIT, FrSPCardBuffDlg::OnSPCardListInit)
ON_FRESH_VI("spcardlist", FRCMD_OWNERDRAW,
	FrSPCardBuffDlg::OnSPCardListOwnerDraw)
END_FRESH_MSGMAP()

FrSPCardBuffDlg::FrSPCardBuffDlg()
{
	m_pSPCardList = NULL;
	if (m_Pos.x < 0.0f)
	{
		SetPosition(WPoint(g_view->GetWidth() - 155.0f, 48.0f));
	}
}

FrSPCardBuffDlg::~FrSPCardBuffDlg()
{
}

bool FrSPCardBuffDlg::OnInit()
{
	FrForm::OnInit();
	CardIterator it;
	for (it = ItemManager()->m_CardMap.begin();
		it != ItemManager()->m_CardMap.end(); ++it)
		g_pFresh->RegisterBitmap((*it).second.BuffImg);
	return true;
}

bool FrSPCardBuffDlg::Close(eFormRet ret, bool bSound)
{
	if (g_view)
	{
		WPoint pos;
		pos = GetRect().TopLeft();
		pos.x = pos.x / g_view->GetWidth() * 800.0f;
		pos.y = pos.y / g_view->GetHeight() * 600.0f;
		SetPosition(pos);
	}
	return FrForm::Close(ret, bSound);
}

void FrSPCardBuffDlg::OnProc(const float delta)
{
}

void FrSPCardBuffDlg::SetBuffItem(
	const std::list<sSCardAvilityPeriodInfo>& cards, bool bClear)
{
	if (m_pSPCardList)
	{
		if (bClear)
			m_pSPCardList->ClearItem();
		for (std::list<sSCardAvilityPeriodInfo>::const_iterator it =
				 cards.begin();
			it != cards.end(); it++)
		{
			const sSCardAvilityPeriodInfo* info = &*it;
			if (((info->tid & 0xfc000000) == 0x7c000000 &&
					info->cardType == 2) ||
				ItemManager()->IsItemBuff(info->tid))
				m_pSPCardList->AddItem((void*)info);
		}
		int rows = (GetSpecialCardNum() - 1) / 4;
		if (rows > 0)
		{
			WRect rect;
			m_pSPCardList->GetClientRect(rect);
			rect.h += m_pSPCardList->GetItemHeight() * rows;
			m_pSPCardList->SetClientRect(rect);
			rect = GetRect();
			rect.h += m_pSPCardList->GetItemHeight() * rows;
			SetRect(rect);
		}
	}
}

int FrSPCardBuffDlg::GetSpecialCardNum()
{
	if (!m_pSPCardList)
		return 0;
	return m_pSPCardList->GetCurrentItemSize(false);
}

void FrSPCardBuffDlg::OnSPCardListInit(int param)
{
	m_pSPCardList = DYNAMIC_CAST(FrListBox, param);
}

void FrSPCardBuffDlg::OnSPCardListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	sSCardAvilityPeriodInfo* info = (sSCardAvilityPeriodInfo*)item->pData;
	if (!info)
		return;
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	if (!device)
		return;
	const Bitmap* bitmap;
	if ((info->tid & 0xfc000000) == 0x7c000000)
	{
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(info->tid);
		if (!card)
			return;
		bitmap = g_pFresh->GetBitmap(card->BuffImg);
	}
	else
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(info->tid);
		if (!common)
			return;
		bitmap = g_pFresh->GetBitmap(MakeStr("%s_buff", common->Icon));
	}
	if (bitmap)
	{
		unsigned long textColor = 0xffff4949;
		unsigned long imageColor = 0xff494949;
		char text[32] = "-:-";
		int remaining = GetRemainedSec(info) / 60;
		int hour = remaining / 60;
		int minute = remaining % 60;
		if (hour >= 1 || minute >= 1)
		{
			if (hour < 0)
				hour = 0;
			if (minute < 0)
				minute = 0;
			sprintf(text, "%.2d:%.2d", hour, minute);
			textColor = 0xff494949;
			imageColor = 0xffffffff;
		}
		device->DrawTexture(bitmap,
			WRect(item->pos.x, item->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			imageColor, 0);
		unsigned long oldColor = device->GetTextColor();
		device->SetTextColor(textColor, 0xffffffff);
		device->SetTextStyle(2);
		device->Print11(item->pos +
				WPoint((float)bitmap->Width(), (float)bitmap->Height()),
			2, text);
		device->SetTextColor(oldColor, 0xffffffff);
		device->SetTextStyle(0);
	}
}

int GetTotalDay_S(SYSTEMTIME date)
{
	int total = 0;
	int days[12] = { 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (date.wYear < 2 || date.wMonth < 1 || date.wMonth > 12 || date.wDay > 31)
		return 0;
	for (int year = 1; year < date.wYear; ++year)
	{
		if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
			total += 366;
		else
			total += 365;
	}
	if ((date.wYear % 4 == 0 && date.wYear % 100 != 0) || date.wYear % 400 == 0)
		days[1] = 29;
	else
		days[1] = 28;
	for (int month = 0; month < date.wMonth - 1; ++month)
		total += days[month];
	if (days[date.wMonth - 1] < date.wDay)
		date.wDay = days[date.wMonth - 1];
	return total + date.wDay;
}

int FrSPCardBuffDlg::GetRemainedSec(const sSCardAvilityPeriodInfo* info)
{
	int result;
	unsigned long elapsedSeconds;
	if ((info->tid & 0xfc000000) != 0x7c000000)
	{
		result =
			(info->useEndTime.wSecond << 16) | info->useEndTime.wMilliseconds;
		unsigned long start = (info->useStartTime.wSecond << 16) |
			info->useStartTime.wMilliseconds;
		elapsedSeconds = (g_CurrentTime - start) / 1000;
	}
	else
	{
		SYSTEMTIME& server = Doc()->GetServerTime();
		SYSTEMTIME now = server;
		int hour = info->useEndTime.wHour;
		int minute = info->useEndTime.wMinute;
		int second = info->useEndTime.wSecond;
		int day = info->useEndTime.wDay;
		if (second < now.wSecond)
		{
			--minute;
			second += 60;
		}
		second -= now.wSecond;
		if (minute < now.wMinute)
		{
			--hour;
			minute += 60;
		}
		minute -= now.wMinute;
		if (hour < now.wHour)
		{
			--day;
			hour += 24;
		}
		hour -= now.wHour;
		if (day != now.wDay)
		{
			SYSTEMTIME end = info->useEndTime;
			if (day < 1)
			{
				--end.wMonth;
				if (end.wMonth < 1)
					end.wMonth = 12;
			}
			int days;
			if (end.wMonth != now.wMonth)
				days = GetTotalDay_S(end) - now.wDay + GetTotalDay_S(server);
			else
				days = day - now.wDay;
			hour += days * 24;
		}
		result = second + (minute + hour * 60) * 60;
		elapsedSeconds = (g_CurrentTime - m_SpecialBuffStartTick) / 1000;
	}
	return result - elapsedSeconds;
}
