#include "giftalarmdlg.h"
#include "shareddoc.h"
#include "treasure.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrGiftAlarmDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrGiftAlarmDlg, FrForm)

ON_FRESH_VI("giftlist", FRCMD_INIT, FrGiftAlarmDlg::OnGiftListInit)
ON_FRESH_VI("giftlist", FRCMD_OWNERDRAW, FrGiftAlarmDlg::OnGiftListOwnerDraw)
ON_FRESH_VI("infomsg", FRCMD_INIT, FrGiftAlarmDlg::OnInfoMsgInit)

END_FRESH_MSGMAP()

struct GiftMessage
{
	void* gift;
	char* lines[2];
};
struct TreasureMessage
{
	unsigned long typeId;
	unsigned short count;
	char sender[64];
	char message[128];
};

FrGiftAlarmDlg::FrGiftAlarmDlg()
{
	m_pGiftList = NULL;
	m_drawIndex = 0;
	m_giftNum = 0;
	m_blinkTime = 0;
	CSharedDoc::Instance()->m_bRefreshCamera = false;
	m_pArrivalMail = g_pFresh->RegisterBitmap("rmr_Arrival_mail");
	m_pOpenMail = g_pFresh->RegisterBitmap("rmr_Open_mail");
}

FrGiftAlarmDlg::~FrGiftAlarmDlg()
{
	for (FrListBox::ITEM_LIST::iterator it = m_pGiftList->m_itemList.begin();
		it != m_pGiftList->m_itemList.end(); ++it)
	{
		FrListItem* item = *it;
		GiftMessage* gift = (GiftMessage*)item->pData;
		if (gift)
		{
			delete[] gift->lines[0];
			delete[] gift->lines[1];
		}
		delete gift;
	}
}

void FrGiftAlarmDlg::OnInfoMsgInit(int param)
{
	m_pInfoMsg = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrGiftAlarmDlg::MakeSenderMsg(const char* msg, char** const lines)
{
	int length = strlen(msg);
	if (length > 20)
	{
		int lead = 0, other = 0;
		for (int i = length - 1; i >= 20; --i)
		{
			if (IsDBCSLeadByte(msg[i]))
				++lead;
			else
				++other;
		}
		if (lead % 2)
			++lead;
		int split = length - other - lead;
		memcpy(lines[0], msg, split);
		lines[0][split] = 0;
		for (int i = 0; i < 20; ++i)
		{
			if (msg[split] != ' ')
				break;
			++split;
		}
		strcpy(lines[1], msg + split);
	}
	else
	{
		strcpy(lines[0], msg);
		lines[1][0] = 0;
	}
}

void FrGiftAlarmDlg::OnGiftListInit(int param)
{
	char message[128] = "\0";
	m_pGiftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pGiftList)
	{
		if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
		{
			for (std::list<sMailInfoBrief>::iterator it =
					 CSharedDoc::Instance()->m_mailList.begin();
				it != CSharedDoc::Instance()->m_mailList.end(); ++it)
			{
				GiftMessage* gift = new GiftMessage;
				gift->gift = &*it;
				gift->lines[0] = new char[80];
				gift->lines[1] = new char[80];
				MakeSenderMsg(message, gift->lines);
				m_pGiftList->AddItem(gift);
			}
			m_giftNum = CSharedDoc::Instance()->m_mailList.size();
			return;
		}
		{
			for (std::list<sGiftInfo>::iterator it =
					 CSharedDoc::Instance()->m_sendGiftList.begin();
				it != CSharedDoc::Instance()->m_sendGiftList.end(); ++it)
			{
				GiftMessage* gift = new GiftMessage;
				sGiftInfo* info = &*it;
				gift->gift = info;
				gift->lines[0] = new char[80];
				gift->lines[1] = new char[80];
				MakeSenderMsg(info->sMsg, gift->lines);
				m_pGiftList->AddItem(gift);
			}
			m_giftNum = CSharedDoc::Instance()->m_sendGiftList.size();
		}
	}
}

void FrGiftAlarmDlg::OnGiftListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	char sender[128] = "\0";
	char line1[80] = "\0";
	char line2[80] = "\0";
	unsigned long typeId;
	int type, count;
	if (IsLocalContent(S4_MAILBOX_FOR_REAL_MYROOM))
	{
		GiftMessage* gift = (GiftMessage*)item->pData;
		if (!gift)
			return;
		sMailInfoBrief* mail = (sMailInfoBrief*)gift->gift;
		wsprintf(sender, "%s", mail->sender);
		wsprintf(line1, "%s", gift->lines[0]);
		wsprintf(line2, "%s", gift->lines[1]);
		typeId = mail->item.dwTID;
		if (mail->item.iTimeCount > 0)
		{
			type = 2;
			count = mail->item.iTimeCount;
		}
		else
		{
			type = 0;
			count = mail->item.iCount;
		}
	}
	else
	{
		GiftMessage* gift = (GiftMessage*)item->pData;
		if (!gift)
			return;
		sGiftInfo* info = (sGiftInfo*)gift->gift;
		wsprintf(sender, "%s", info->sFromID);
		wsprintf(line1, "%s", gift->lines[0]);
		wsprintf(line2, "%s", gift->lines[1]);
		typeId = info->tid;
		type = info->ItemType;
		count = info->Arg0;
	}
	m_drawIndex = item->idx;
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(0);
	g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 10, item->pos.y + 5),
		0, sender, -1, 0xffffffff);
	const Bitmap* message =
		g_pFresh->GetManager()->GetBitmap("FRAMES8", "text_message");
	if (message)
	{
		gdi->DrawTexture(message,
			WRect(item->pos.x + 24, item->pos.y + 27, (float)message->Width(),
				(float)message->Height()),
			0xffffffff, 0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	g_pFresh->GetManager()->PrintText(
		WPoint(item->pos.x + 28, item->pos.y + 56), 0, line1, -1, 0xffffffff);
	g_pFresh->GetManager()->PrintText(
		WPoint(item->pos.x + 28, item->pos.y + 74), 0, line2, -1, 0xffffffff);
	char name[80] = "\0";
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	const Bitmap* icon;
	if (!common)
	{
		icon = m_pArrivalMail;
		wsprintf(name, "\xc6\xed\xc1\xf6");
	}
	else
	{
		icon = g_pFresh->GetBitmap(common->Icon);
		wsprintf(name, "%s", common->Name);
	}
	if (icon)
	{
		float x, y;
		switch (typeId >> 26)
		{
		case 2:
		case 28:
			x = -10;
			y = -27;
			break;
		case 6:
			x = 0;
			y = 0;
			break;
		case 5:
			x = -10;
			y = -8;
			break;
		default:
			x = -10;
			y = -27;
			break;
		}
		{
			gdi->DrawTexture(icon,
				WRect(x + item->pos.x + 36, y + item->pos.y + 121,
					(float)icon->Width(), (float)icon->Height()),
				0xffffffff, 0);
		}
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(1);
	gdi->Print(WPoint(item->pos.x + 181, item->pos.y + 96), 2, "%s", name);
	gdi->SetTextStyle(0);
	const char* text = NULL;
	if (type >= 3 && type <= 5)
	{
		if (type == 3)
			text = MakeStr("%d\xbd\xc3\xb0\xa3", count);
		else if (type == 4)
			text = MakeStr("%d\xc0\xcf", count);
		else
			text = MakeStr("%d\xb0\xb3\xbf\xf9", count);
	}
	else
	{
		if ((typeId >> 26) == 28)
		{
			if (count > 0)
				text = MakeStr("%d\xb0\xb3", count);
			else
			{
				IFF_STRUCT::sAuxPart* part = ItemManager()->FindAuxPart(typeId);
				if (part && part->COM[0] > 0)
					text = MakeStr("%d\xb0\xb3", part->COM[0]);
			}
		}
		else if ((typeId >> 26) == 5)
		{
			if (count > 0)
				text = MakeStr("%d\xb0\xb3", count);
			else
			{
				IFF_STRUCT::sBall* ball = ItemManager()->FindBall(typeId);
				if (ball && ball->COM[0] > 0)
					text = MakeStr("%d\xb0\xb3", ball->COM[0]);
			}
		}
		else if ((typeId >> 26) == 6)
		{
			if (typeId == 436207632)
				text = MakeStr("%d\xc6\xce", count);
			else
				text = MakeStr("%d\xb0\xb3", count);
		}
		else if ((typeId >> 26) == 14)
		{
			text = MakeStr("%d\xc0\xcf", count);
		}
		else if ((typeId >> 26) == 2)
		{
			text = MakeStr("%d\xb0\xb3", 1);
		}
	}
	if (text)
		gdi->Print(WPoint(item->pos.x + 181, item->pos.y + 116), 2, text);
}

void FrGiftAlarmDlg::OnProc(const float dt)
{
	static bool changed = false;
	static bool visible = false;
	m_blinkTime += dt;
	if (m_blinkTime > 1.0f)
	{
		visible ^= true;
		m_blinkTime = 0;
		changed = true;
	}
	if (visible)
	{
		if (!changed)
			return;
		m_pInfoMsg->SetCaption(MakeStr(
			"\xbf\xec\xc6\xed\xc7\xd4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4   (%d/%d)",
			m_drawIndex + 1, m_giftNum));
	}
	else
	{
		if (!changed)
			return;
		m_pInfoMsg->SetCaption("");
	}
	changed = false;
}

IMPLEMENT_OBJECT(CTreasureAlarmDlg, FrForm)

BEGIN_FRESH_MSGMAP(CTreasureAlarmDlg, FrForm)

ON_FRESH_VI("giftlist", FRCMD_INIT, CTreasureAlarmDlg::OnListBox_Gift_Init)
ON_FRESH_VI("giftlist", FRCMD_OWNERDRAW,
	CTreasureAlarmDlg::OnListBox_Gift_OwnerDraw)
ON_FRESH_VI("infomsg", FRCMD_INIT, CTreasureAlarmDlg::OnStatic_Msg_Init)

END_FRESH_MSGMAP()

CTreasureAlarmDlg::CTreasureAlarmDlg()
{
	m_pGiftList = NULL;
	m_pMsg = NULL;
}

CTreasureAlarmDlg::~CTreasureAlarmDlg()
{
	for (FrListBox::ITEM_LIST::iterator it = m_pGiftList->m_itemList.begin();
		it != m_pGiftList->m_itemList.end(); ++it)
		delete (TreasureMessage*)(*it)->pData;
}

void CTreasureAlarmDlg::OnListBox_Gift_Init(int param)
{
	m_pGiftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pGiftList)
	{
		CTHunter::Instance()->ReAdjustRepository();
		unsigned char count = CTHunter::Instance()->GetRepositorySize();
		for (unsigned char i = 0; i < count; ++i)
		{
			sTreasureHunt info;
			info = CTHunter::Instance()->GetGiftInfo(i);
			unsigned long typeId = info.dwTid;
			unsigned short quantity = info.wCount;
			TreasureMessage* gift = new TreasureMessage;
			gift->typeId = typeId;
			gift->count = quantity;
			strncpy(gift->sender, "\xc6\xae\xb7\xb9\xc1\xae \xbc\xb1\xb9\xb0",
				64);
			strncpy(gift->message, "", 128);
			m_pGiftList->AddItem(gift);
		}
		m_giftNum = count;
	}
}

void CTreasureAlarmDlg::OnListBox_Gift_OwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	TreasureMessage* gift = (TreasureMessage*)item->pData;
	if (!gift)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(gift->typeId);
	if (!common)
		return;
	m_drawIndex = item->idx;
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(0);
	g_pFresh->GetManager()->PrintText(WPoint(item->pos.x + 10, item->pos.y + 5),
		0, gift->sender, -1, 0xffffffff);
	const Bitmap* message = g_pFresh->RegisterBitmap("text_message_treasure");
	if (message)
	{
		gdi->DrawTexture(message,
			WRect(item->pos.x + 24, item->pos.y + 27, (float)message->Width(),
				(float)message->Height()),
			0xffffffff, 0);
	}
	gdi->SetTextColor(0xffffffff, 0xffffffff);
	g_pFresh->GetManager()->PrintText(
		WPoint(item->pos.x + 28, item->pos.y + 56), 0, gift->message, -1,
		0xffffffff);
	const Bitmap* icon = g_pFresh->GetBitmap(common->Icon);
	if (icon)
	{
		float x, y;
		switch (common->TypeId >> 26)
		{
		case 6:
			x = 0;
			y = 0;
			break;
		case 5:
			x = -10;
			y = -8;
			break;
		default:
			x = -10;
			y = -27;
			break;
		}
		{
			gdi->DrawTexture(icon,
				WRect(x + item->pos.x + 36, y + item->pos.y + 121,
					(float)icon->Width(), (float)icon->Height()),
				0xffffffff, 0);
		}
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(1);
	gdi->Print(WPoint(item->pos.x + 181, item->pos.y + 96), 2, "%s",
		common->Name);
	gdi->SetTextStyle(0);
	const char* text = NULL;
	unsigned long typeId = common->TypeId;
	if ((typeId >> 26) == 28)
	{
		if (gift->count > 0)
			text = MakeStr("%d\xb0\xb3", gift->count);
		else
		{
			IFF_STRUCT::sAuxPart* part = ItemManager()->FindAuxPart(typeId);
			if (part && part->COM[0] > 0)
				text = MakeStr("%d\xb0\xb3", part->COM[0]);
		}
	}
	else if ((typeId >> 26) == 5)
	{
		if (gift->count > 0)
			text = MakeStr("%d\xb0\xb3", gift->count);
		else
		{
			IFF_STRUCT::sBall* ball = ItemManager()->FindBall(typeId);
			if (ball && ball->COM[0] > 0)
				text = MakeStr("%d\xb0\xb3", ball->COM[0]);
		}
	}
	else if ((typeId >> 26) == 6)
	{
		if (typeId == 436207632)
			text = MakeStr("%d\xc6\xce", gift->count);
		else
			text = MakeStr("%d\xb0\xb3", gift->count);
	}
	else if ((typeId >> 26) == 14)
	{
		text = MakeStr("%d\xc0\xcf", gift->count);
	}
	else
	{
		text = MakeStr("%d\xb0\xb3", 1);
	}

	if (text)
		gdi->Print(WPoint(item->pos.x + 181, item->pos.y + 116), 2, text);
}

void CTreasureAlarmDlg::OnStatic_Msg_Init(int param)
{
	m_pMsg = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void CTreasureAlarmDlg::OnProc(const float dt)
{
	static bool changed = false;
	static bool visible = false;
	m_blinkTime += dt;
	if (m_blinkTime > 1.0f)
	{
		visible ^= true;
		m_blinkTime = 0;
		changed = true;
	}
	if (visible)
	{
		if (!changed)
			return;
		m_pMsg->SetCaption(
			"\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4");
	}
	else
	{
		if (!changed)
			return;
		m_pMsg->SetCaption("");
	}
	changed = false;
}
