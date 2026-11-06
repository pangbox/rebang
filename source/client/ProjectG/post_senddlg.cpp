#include "post_senddlg.h"
#include "transferableitem.h"
#include "emoticondlg.h"
#include "buddymanager.h"
#include "frarea.h"
#include "ucclibrary.h"
#include "gatewayactor.h"
#include "binstr.h"
#include <algorithm>

extern Fresh* g_pFresh;

void __fastcall CallErrorMsg(const char* message)
{
	FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify");
	form->SetMessage(message, false);
	form->Open(NULL, FrESCAPE | FrENTER);
}

IMPLEMENT_OBJECT(CPost_SendDlg, FrForm)
BEGIN_FRESH_MSGMAP(CPost_SendDlg, FrForm)
ON_FRESH_VI("recive_user", FRCMD_INIT, CPost_SendDlg::OnPostReciveUserInit)
ON_FRESH_BI("recive_user", FRCMD_ENTERKEY,
	CPost_SendDlg::OnPostReceiveUserEnterKey)
ON_FRESH_VI("user_Nick", FRCMD_INIT, CPost_SendDlg::OnPostUserNickInit)
ON_FRESH_VI("recive_msg", FRCMD_INIT, CPost_SendDlg::OnPostReciveMsgInit)
ON_FRESH_BI("recive_msg", FRCMD_ENTERKEY,
	CPost_SendDlg::OnPostReciveMsgEnterKey)
ON_FRESH_VI("search_user", FRCMD_INIT, CPost_SendDlg::OnPostSearchUserInit)
ON_FRESH_VV("search_user", FRCMD_LBUTTONUP,
	CPost_SendDlg::OnPostSearchUserBtnUp)
ON_FRESH_VI("friend_list", FRCMD_INIT, CPost_SendDlg::OnPostFriendListInit)
ON_FRESH_VV("friend_list", FRCMD_LBUTTONUP,
	CPost_SendDlg::OnPostFriendListBtnUp)
ON_FRESH_VI("item_appending", FRCMD_INIT, CPost_SendDlg::OnPostItemAddInit)
ON_FRESH_VV("item_appending", FRCMD_LBUTTONUP,
	CPost_SendDlg::OnPostItemAddBtnUp)
ON_FRESH_VI("post_slot", FRCMD_INIT, CPost_SendDlg::OnPostListInit)
ON_FRESH_VI("post_slot", FRCMD_OWNERDRAW, CPost_SendDlg::OnPostListOwnerDraw)
ON_FRESH_VI("Btnemoticon", FRCMD_INIT, CPost_SendDlg::OnEmoticonBtnInit)
ON_FRESH_VV("Btnemoticon", FRCMD_LBUTTONUP, CPost_SendDlg::OnEmoticonBtnUp)
ON_FRESH_VI("edit_servicecharge", FRCMD_INIT,
	CPost_SendDlg::OnPostServiceChargeInit)
END_FRESH_MSGMAP()

CPost_SendDlg::CPost_SendDlg()
{
	m_pFriendDlg = NULL;
	m_pReciveUser = NULL;
	m_pReciveMsg = NULL;
	m_pSearchUser = NULL;
	m_pFriendList = NULL;
	m_pEmoticonBtn = NULL;
	m_pEmoticonDlg = NULL;
	m_pItemAdd = NULL;
	m_pMyItemList = NULL;
	m_receiverUid = -1;
	m_receiverNick = "";
	m_receiverId = "";
	m_bReceiverChecked = false;
	CTransferableItem::Instance()->Reset();
	CTransferableItem::Instance()->SetTransferableItemList();
	CTransferableItem::Instance()->Set_RegistrationListSize(4);
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
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

CPost_SendDlg::~CPost_SendDlg()
{
	Reset();
	if (m_pTitleFont)
		m_pTitleFont->Erase();
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void CPost_SendDlg::Reset()
{
	m_pFriendDlg = NULL;
	m_pReciveUser = NULL;
	m_pReciveMsg = NULL;
	m_pSearchUser = NULL;
	m_pFriendList = NULL;
	m_pEmoticonBtn = NULL;
	m_pEmoticonDlg = NULL;
	m_pItemAdd = NULL;
	m_pMyItemList = NULL;
	m_receiverUid = -1;
	m_receiverNick = "";
	m_receiverId = "";
	m_bReceiverChecked = false;
	CTransferableItem::Instance()->Reset();
	CTransferableItem::Instance()->SetTransferableItemList();
	CTransferableItem::Instance()->Set_RegistrationListSize(4);
}

void CPost_SendDlg::OnPostReciveUserInit(int param)
{
	m_pReciveUser = DYNAMIC_CAST(FrEdit, param);
}

void CPost_SendDlg::OnPostReciveMsgInit(int param)
{
	m_pReciveMsg = DYNAMIC_CAST(FrEdit, param);
	if (m_pReciveMsg)
	{
		m_pReciveMsg->SetCharLimit(800, true);
	}
}

bool CPost_SendDlg::OnPostReciveMsgEnterKey(int param)
{
	return true;
}

void CPost_SendDlg::OnPostUserNickInit(int param)
{
	m_pUserNick = DYNAMIC_CAST(FrEdit, param);
}

void CPost_SendDlg::OnPostSearchUserInit(int param)
{
	m_pSearchUser = DYNAMIC_CAST(FrButton, param);
}

bool CPost_SendDlg::OnPostReceiveUserEnterKey(int param)
{
	OnPostSearchUserBtnUp();
	return false;
}

void CPost_SendDlg::OnPostSearchUserBtnUp()
{
	if (m_pReciveUser)
	{
		const char* name = m_pReciveUser->GetLine(1, false);
		if (!stricmp(name, MyNick()))
		{
			FrForm* form =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			form->SetMessage(
				"\xba\xbb\xc0\xce\xbf\xa1\xb0\xd4\xb4\xc2 \xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ",
				false);
			form->Open(NULL, FrESCAPE | FrENTER);
			return;
		}
		int length = strlen(name);
		if (name && length > 0 && length < 22)
		{
			WSendPacket packet((enumClientPacket)7);
			packet.Encode1(0);
			packet.EncodeStr(std::string(name));
			packet.Send(TO_GAME);
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		}
		else
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
				(int)"\xb9\xde\xb4\xc2 \xc0\xcc\xc0\xc7 \xb4\xd0\xb3\xd7\xc0\xd3\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
		}
	}
}

void CPost_SendDlg::OnPostFriendListInit(int param)
{
	m_pFriendList = DYNAMIC_CAST(FrButton, param);
}

void CPost_SendDlg::OnPostFriendListBtnUp()
{
	if (BuddyMGR->GetBuddySize() == 0)
	{
		if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_MSN))
		{
			WSendPacket packet((enumClientPacket)0x3c);
			packet.Encode2(0x11f);
			packet.Send(TO_GAME);
		}
		else
		{
			WSendPacket packet(0x13);
			packet.Encode2(0x11f);
			packet.Send(TO_MSN);
		}
	}
	if (!m_pFriendDlg)
	{
		m_pFriendDlg = CreateForm<FrFriendDlg>(g_pFresh->GetManager(), this,
			"post_friend_list_form");
		if (m_pFriendDlg)
		{
			m_pFriendDlg->SetParent2(this);
			m_pFriendDlg->EnableDrag(false);
			m_pFriendDlg->Open(
				(FRESH_PFN_RESULT)&CPost_SendDlg::OnFriendListResult,
				WPoint(602.0f, m_rect.y), FrESCAPE);
		}
	}
}

void CPost_SendDlg::OnPostItemAddInit(int param)
{
	m_pItemAdd = DYNAMIC_CAST(FrButton, param);
}

void CPost_SendDlg::OnPostItemAddBtnUp()
{
	char allowed = 0;
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);
	if (allowed == 1)
	{
		if (!m_pMyItemList)
		{
			m_pMyItemList = CreateForm<CPost_MyItemList>(g_pFresh->GetManager(),
				this, "myitem_choice");
			FrForm::EnableTail(false);
			m_pMyItemList->Open(
				(FRESH_PFN_RESULT)&CPost_SendDlg::OnPostItemListResutl,
				FrESCAPE | FrRET_CANCEL);
			m_pMyItemList->SetFadeout(false);
		}
	}
	else
	{
		actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
	}
}

void CPost_SendDlg::OnPostListInit(int param)
{
	m_pPostList = DYNAMIC_CAST(FrListBox, param);
}

void CPost_SendDlg::OnPostListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	sTradeItem* item = (sTradeItem*)row->pData;
	if (!item)
		return;
	unsigned long typeId = item->dwTid;
	int count = item->iNum;
	if (!graphic || !typeId)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	if (!common)
		return;
	float offsetX = 0.0f, offsetY = 0.0f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent((localContentType_t)0x57) && (item->status & 1) &&
		IsUccClothes(item->dwTid))
	{
		const Bitmap* ucc = GetUccItemIcon(item->dwGuid);
		if (ucc)
			bitmap = ucc;
	}
	if ((item->dwTid & 0xfc000000) == 0x08000000)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 255) | 0x04000000);
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

	if ((typeId & 0xfc000000) == 0x08000000)
	{
		offsetX = -12.0f;
		offsetY = -20.0f;
	}
	if (bitmap)
	{
		graphic->DrawTexture(bitmap,
			WRect(row->pos.x + offsetX + 14.0f, row->pos.y + offsetY + 11.0f,
				(float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(4);
	}
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item->dwTid))
	{
		if ((item->status & 1) && item->Seq == 1 && m_pUccOriginal)
			graphic->DrawTexture(m_pUccOriginal,
				WRect(row->pos.x + 3.0f, row->pos.y + 15.0f,
					(float)m_pUccOriginal->Width(),
					(float)m_pUccOriginal->Height()),
				0xffffffff, 0);
		else if ((item->status & 1) && item->Seq > 1 && m_pUccCopied)
			graphic->DrawTexture(m_pUccCopied,
				WRect(row->pos.x + 3.0f, row->pos.y + 15.0f,
					(float)m_pUccCopied->Width(),
					(float)m_pUccCopied->Height()),
				0xffffffff, 0);
		else if ((item->status & 2) && m_pUccTempSaved)
			graphic->DrawTexture(m_pUccTempSaved,
				WRect(row->pos.x + 3.0f, row->pos.y + 15.0f,
					(float)m_pUccTempSaved->Width(),
					(float)m_pUccTempSaved->Height()),
				0xffffffff, 0);
	}

	if (IsLocalContent((localContentType_t)0x51) &&
		(item->dwTid & 0xfc000000) == 0x08000000)
		DrawPartsSlot(graphic, row);
	if (count > 1)
	{
		unsigned int i = 0;
		const char* text = MakeStr("%d", count);
		float x = row->pos.x + 54.0f;
		for (i = 0; i < strlen(text); ++i)
			x -= m_pTitleFont->GetCharWidth(g_view, text[i] - '0');
		m_pTitleFont->Print(g_view, x, row->pos.y + 42.0f, text, 0, 0xffffffff,
			NULL);
	}
}

void CPost_SendDlg::DrawPartsSlot(FrGraphicInterface* graphic, FrListItem* row)
{
	int slot;
	sTradeItem* item = (sTradeItem*)row->pData;
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
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
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
			if (item->dwTid == (*it).partsTid && item->dwGuid == (*it).partsUid)
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
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
			item->CaddieSlotNum = 0;
	}
	for (slot = 1; slot < item->CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTid == (*it).partsTid && item->dwGuid == (*it).partsUid)
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

void CPost_SendDlg::OnPostServiceChargeInit(int param)
{
	m_pServiceCharge = DYNAMIC_CAST(FrEdit, param);
	if (m_pServiceCharge)
	{
		m_pServiceCharge->AddText(MakeStr("%6d", 100), false, true);
	}
}

void CPost_SendDlg::OnEmoticonBtnInit(int param)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, param);
	if (m_pEmoticonBtn)
	{
		m_pEmoticonBtn->SetVisible(true);
	}
}

void CPost_SendDlg::OnEmoticonBtnUp()
{
	if (m_pEmoticonBtn && !m_pEmoticonDlg)
	{
		m_pEmoticonDlg =
			CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon");
		if (m_pEmoticonDlg)
		{
			WPoint pos(0, 0);
			const WRect& rect = m_pEmoticonBtn->GetRect();
			pos.x = rect.x - 200.0f;
			pos.y = rect.y + 24.0f;
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&CPost_SendDlg::OnEmoticonResult, pos,
				FrESCAPE | FrENTER);
		}
	}
}

void CPost_SendDlg::OnOK()
{
	int count = CTransferableItem::Instance()->Get_RegistrationListSize();
	std::string message = GetLetterMsg();
	int length = message.size();
	if (!m_bReceiverChecked)
	{
		CallErrorMsg(
			"\xbc\xb1\xb9\xb0\xc7\xd2 \xb4\xeb\xbb\xf3\xc0\xc7 \xc0\xcc\xb8\xa7\xc0\xcc \xc8\xae\xc0\xce\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xcb\xbb\xf6\xc0\xb8\xb7\xce \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb0\xc5\xb3\xaa \xc4\xa3\xb1\xb8\xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xc3\xdf\xb0\xa1\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4. ");
	}
	else
	{
		if (strlen(m_pReciveUser->GetLine(1, false)) == 0 &&
			strlen(m_pUserNick->GetLine(1, false)) == 0)
		{
			CallErrorMsg(
				"\xbc\xb1\xb9\xb0\xc7\xd2 \xb4\xeb\xbb\xf3\xc0\xc7 \xc0\xcc\xb8\xa7\xc0\xcc \xc8\xae\xc0\xce\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xcb\xbb\xf6\xc0\xb8\xb7\xce \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb0\xc5\xb3\xaa \xc4\xa3\xb1\xb8\xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xc3\xdf\xb0\xa1\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4. ");
			return;
		}
		if (strlen(m_pReciveMsg->GetLine(1, false)) == 0 &&
			CTransferableItem::Instance()->Get_RegistrationListSize() == 0)
		{
			CallErrorMsg(
				"\xb8\xde\xbd\xc3\xc1\xf6\xb3\xaa \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xb7\xba\xce\xc7\xcf\xbc\xc5\xbe\xdf \xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if (count == 0 && Doc()->m_myInfo.stat.i64Pang < 100)
		{
			CallErrorMsg(
				"\xbc\xf6\xbc\xf6\xb7\xe1\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
			return;
		}
		if (count * 500 > Doc()->m_myInfo.stat.i64Pang)
			CallErrorMsg(
				"\xbc\xf6\xbc\xf6\xb7\xe1\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
		else
		{
			if (length > 800)
			{
				CallErrorMsg(
					"\xc6\xed\xc1\xf6 \xb3\xbb\xbf\xeb\xc0\xba 800\xc0\xda\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if (!VerifySendItemList())
				CallErrorMsg(
					"\xc3\xb7\xba\xce\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe9 \xc1\xdf\xbf\xa1 \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			else if (!VerifyIncludeSameItem())
				CallErrorMsg(
					"\xb0\xb0\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb5\xce \xb0\xa1\xc1\xf6 \xc0\xcc\xbb\xf3 \xc3\xb7\xba\xce\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			else
			{
				FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"rmr_notify");
				form->SetMessage(
					MakeStr(
						"\xbc\xf6\xbc\xf6\xb7\xe1\xb0\xa1 %d \xc0\xd4\xb4\xcf\xb4\xd9. \n\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbb\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
						GetSendServiceCharge()),
					false);
				form->Open(
					(FRESH_PFN_RESULT)&CPost_SendDlg::OnSendQuestionResult,
					FrENTER);
				return;
			}
		}
		CTransferableItem::Instance()->ResetRegistrationItemList();
		m_pPostList->ClearItem();
	}
}

bool CPost_SendDlg::OnSendQuestionResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		SetFadeout(true);
		FrForm::OnOK();
	}
	return true;
}

void CPost_SendDlg::OnCancel()
{
	CTransferableItem::Instance()->ResetRegistrationItemList();
	FrForm::OnCancel();
}

bool CPost_SendDlg::OnFriendListResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		m_bReceiverChecked = true;
		if (!m_pFriendDlg->IsSelectPerson() && m_pFriendDlg->HaveFriends())
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 35,
				(int)"\xc4\xa3\xb1\xb8\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			m_pFriendDlg = NULL;
			return true;
		}
		m_receiverUid = m_pFriendDlg->GetUid();
		m_receiverId = m_pReciveUser->GetLine(1, false);
		m_receiverNick = m_pReciveUser->GetLine(1, false);
		m_pFriendDlg = NULL;
		return true;
	}
	else
	{
		m_pReciveUser->ClearLine();
		m_pUserNick->ClearLine();
		m_receiverUid = -1;
		m_receiverId.clear();
		m_receiverNick.clear();
	}
	m_pFriendDlg = NULL;
	return true;
}

bool CPost_SendDlg::OnPostItemListResutl(int result, FrForm* form)
{
	if (form)
		FrForm::EnableTail(true);
	if (result == FrOK)
	{
		m_pPostList->ClearItem();
		std::list<sTradeItem>* list =
			CTransferableItem::Instance()->GetList_RegistrationItem();
		for (std::list<sTradeItem>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if (IsLocalContent((localContentType_t)0x51))
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(it->dwTid);
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
	else
	{
		std::list<sTradeItem>* list =
			CTransferableItem::Instance()->GetList_RegistrationItem();
		if (m_pPostList->GetCurrentItemSize(true) != 0)
		{
			for (std::list<sTradeItem>::iterator it = list->begin();
				it != list->end();)
			{
				if (!m_pPostList->FindItem(&*it))
					it = CTransferableItem::Instance()
							 ->DelRegistrationItem_itor_return(it->dwGuid);
				else
					++it;
			}
		}
		else
		{
			CTransferableItem::Instance()->ResetRegistrationItemList();
		}
	}
	int count = CTransferableItem::Instance()->Get_RegistrationListSize();
	int charge = count * 500;
	if (charge > Doc()->m_myInfo.stat.i64Pang)
	{
		CallErrorMsg(
			"\xbc\xf6\xbc\xf6\xb7\xe1\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
		CTransferableItem::Instance()->ResetRegistrationItemList();
		m_pPostList->ClearItem();
	}
	m_pServiceCharge->ClearLine();
	if (count == 0)
		m_pServiceCharge->AddText(MakeStr("%6d", 100), false, true);
	else
		m_pServiceCharge->AddText(MakeStr("%6d", charge), false, true);
	m_pMyItemList = NULL;
	return true;
}

void CPost_SendDlg::SetReceiverInfo(unsigned long uid,
	const sPangYaUserInfo& info)
{
	const char* nick = info.sNick;
	m_receiverUid = uid;
	m_receiverNick = nick;
	m_receiverId = info.sID;
	if (IsLocalContent((localContentType_t)0x10))
	{
		if (info.nChannelingFlag == 1)
			m_receiverId = m_pReciveUser->GetLine(1, false);
		else
			m_receiverId = nick;
	}
	else
	{
		m_receiverId = nick;
	}
	if (m_pReciveUser)
	{
		m_bReceiverChecked = true;
		m_pReciveUser->SetLine(1, m_receiverId.c_str(), 0, false, 0);
		m_pUserNick->SetVisible(true);
	}
	if (m_pUserNick)
	{
		m_pUserNick->SetLine(1, m_receiverNick.c_str(), 0, false, 0);
	}
}

bool CPost_SendDlg::OnEmoticonResult(int result, FrForm* form)
{
	if (m_pEmoticonDlg && result == FrOK)
	{
		FrEmoticonDlg* dialog = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (dialog && dialog->GetSelectedIcon())
		{
			FrEdit* edit = DYNAMIC_CAST(FrEdit, FindChildByName("id"));
			if (m_pReciveUser)
			{
				m_pReciveUser->GetSelectLine();
				const char* front = m_pReciveUser->GetEditText_Front();
				const char* comp = m_pReciveUser->GetEditText_Comp();
				const char* end = m_pReciveUser->GetEditText_End();
				m_pReciveUser->SetLine(1,
					MakeStr("%s%s%s%s", front, comp, dialog->GetSelectedIcon(),
						end),
					0, false, 0);
				m_pReciveUser->SetKeyFocus(true);
			}
		}
	}
	m_pEmoticonDlg = NULL;
	return true;
}

std::string CPost_SendDlg::GetLetterMsg()
{
	static std::string message;
	message.clear();
	int count = m_pReciveMsg->GetLineNum();
	for (int i = 1; i <= count; ++i)
	{
		message += m_pReciveMsg->GetLine(i, false);
		if (i != count)
			message += "\n";
	}
	return message;
}

int CPost_SendDlg::GetSendListSize()
{
	return CTransferableItem::Instance()->Get_RegistrationListSize();
}

std::list<sMailIncludeItem>* CPost_SendDlg::GetSendList()
{
	m_sendList.clear();
	sMailIncludeItem item;
	memset(&item, 0, sizeof(item));
	std::list<sTradeItem>* list =
		CTransferableItem::Instance()->GetList_RegistrationItem();
	for (std::list<sTradeItem>::iterator it = list->begin(); it != list->end();
		++it)
	{
		item.dwIDX = it->dwGuid;
		item.dwTID = it->dwTid;
		item.iCount = it->iNum;
		item.btItemType = it->byItemType;
		item.iTimeCount = it->wTime;
		m_sendList.push_back(item);
	}
	return &m_sendList;
}

bool CPost_SendDlg::VerifySendItemList()
{
	std::list<sTradeItem>* list =
		CTransferableItem::Instance()->GetList_RegistrationItem();
	for (std::list<sTradeItem>::iterator it = list->begin(); it != list->end();
		++it)
	{
		unsigned long typeId = it->dwTid;
		sItemInfo* item = Doc()->FindMyItemByGuid(it->dwGuid);
		if ((item && item->tid != typeId) || it->iNum > 99)
			return false;
	}
	return true;
}

bool CPost_SendDlg::VerifyIncludeSameItem()
{
	std::vector<unsigned long> types;
	std::list<sTradeItem>* list =
		CTransferableItem::Instance()->GetList_RegistrationItem();
	for (std::list<sTradeItem>::iterator it = list->begin(); it != list->end();
		++it)
		types.push_back(it->dwTid);
	unsigned int limit = types.size() - 1;
	for (unsigned int i = 0; i < limit; ++i)
		if (std::count(types.begin(), types.end(), types[i]) > 1)
			return false;
	return true;
}

int CPost_SendDlg::GetSendServiceCharge()
{
	if (CTransferableItem::Instance()->Get_RegistrationListSize() == 0)
		return 100;
	return CTransferableItem::Instance()->Get_RegistrationListSize() * 500;
}

IMPLEMENT_OBJECT(CPost_MyItemList, FrForm)
BEGIN_FRESH_MSGMAP(CPost_MyItemList, FrForm)
ON_FRESH_VI("myitem_list", FRCMD_INIT, CPost_MyItemList::OnPost_ItemListInit)
ON_FRESH_VI("myitem_list", FRCMD_OWNERDRAW,
	CPost_MyItemList::OnPost_ItemListOwnerDraw)
ON_FRESH_VV("myitem_list", FRCMD_LBUTTONDOWN,
	CPost_MyItemList::OnPost_ItemListBtnDown)
ON_FRESH_VV("myitem_list", FRCMD_RBUTTONDOWN,
	CPost_MyItemList::OnPost_ItemListRBtnDown)
ON_FRESH_VV("myitem_list", FRCMD_MOUSEMOVE,
	CPost_MyItemList::OnPost_ItemListMouseMove)
ON_FRESH_VV("myitem_list", FRCMD_DBLCLICK,
	CPost_MyItemList::OnPost_ItemListDClick)
ON_FRESH_VI("select_itemlist", FRCMD_INIT,
	CPost_MyItemList::OnPost_SelectListInit)
ON_FRESH_VI("select_itemlist", FRCMD_OWNERDRAW,
	CPost_MyItemList::OnPost_SelectListOwnerDraw)
ON_FRESH_VV("select_itemlist", FRCMD_LBUTTONDOWN,
	CPost_MyItemList::OnPost_SelectListBtnDown)
ON_FRESH_VV("select_itemlist", FRCMD_MOUSEMOVE,
	CPost_MyItemList::OnPost_SelectListMouseMove)
ON_FRESH_VV("select_itemlist", FRCMD_DBLCLICK,
	CPost_MyItemList::OnPost_SelectListDClick)
ON_FRESH_VI("select_item_img", FRCMD_INIT, CPost_MyItemList::OnPost_ImgSelect)
ON_FRESH_VI("Edit_array", FRCMD_INIT, CPost_MyItemList::OnPost_EditArrayInit)
ON_FRESH_VI("Btn_Array", FRCMD_INIT, CPost_MyItemList::OnPostBtnArrayInit)
ON_FRESH_VV("Btn_Array", FRCMD_LBUTTONUP,
	CPost_MyItemList::OnPostBtnArrayButtonUp)
END_FRESH_MSGMAP()

CPost_MyItemList::CPost_MyItemList()
{
	m_pParent = NULL;
	m_pItemCountDlg = NULL;
	m_pCountItem = NULL;
	m_pBtnArray = NULL;
	m_pEditArray = NULL;
	m_pArrayDlg = NULL;
	m_arrayType = -1;
	m_pSelectN = g_pFresh->GetBitmap("rmr_select_n");
	m_pSelectD = g_pFresh->GetBitmap("rmr_select_d");
	m_pLine = g_pFresh->GetBitmap("rmr_line");
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
	static char* bongFiles[] = { "[font_bong.jpg" };
	WTITLEFONT bong;
	memset(&bong, 0, sizeof(bong));
	bong.filename = bongFiles;
	bong.fontw = 9;
	bong.fonth = 16;
	bong.numPages = 1;
	bong.texw = 128;
	bong.texh = 32;
	bong.pCharSet = "1234567890,-";
	m_pPangFont = g_resrcmng->GetTitleFont();
	m_pPangFont->Create(&bong);
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	m_cancelBtn.pNormal = g_pFresh->RegisterBitmap("rmr_visitor_book_cancel_n");
	m_cancelBtn.pOver = g_pFresh->RegisterBitmap("rmr_visitor_book_cancel_o");
	m_cancelBtn.rect = WRect(52.0f, 60.0f, (float)m_cancelBtn.pNormal->Width(),
		(float)m_cancelBtn.pNormal->Height());
}

CPost_MyItemList::~CPost_MyItemList()
{
	if (m_pTitleFont)
		m_pTitleFont->Erase();
	if (m_pPangFont)
		m_pPangFont->Erase();
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
	if (g_resrcmng && m_pPangFont)
	{
		g_resrcmng->Release(m_pPangFont);
		m_pPangFont = NULL;
	}
}

void CPost_MyItemList::OnPost_ItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pItemList)
	{
		m_pItemList->SetMultiSelect(true);
	}
}

void CPost_MyItemList::OnPost_ItemListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!row->pData)
		return;
	const Bitmap* selection;
	if (row->selected)
	{
		selection = m_pSelectD;
		graphic->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (row->underCursor)
	{
		selection = m_pSelectN;
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	else
	{
		selection = NULL;
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	sTradeItem* item = (sTradeItem*)row->pData;
	WRect client;
	GetClientRect(client);
	WRect line;
	for (int y = 0; y < 2; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			line = WRect(x * 92.0f + client.x + 126.0f,
				y * 110.0f + 87.0f + client.y, (float)m_pLine->Width(),
				(float)m_pLine->Height());
			graphic->DrawTexture(m_pLine, line, 0xffffffff, 0);
		}
	}
	if (selection)
		graphic->DrawTexture(selection,
			WRect(row->pos.x - 2.0f, row->pos.y, (float)selection->Width(),
				(float)selection->Height()),
			0xffffffff, 0);
	if ((item->dwTid & 0xfc000000) == 0x08000000)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 255) | 0x04000000);
		if (character)
		{
			const Bitmap* bitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(row->pos.x + 7.0f, row->pos.y + 5.0f,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
		}
	}

	DrawItem(WPoint(row->pos.x + 13.0f, row->pos.y + 20.0f), item, 0);
	if (IsLocalContent((localContentType_t)0x51) &&
		(item->dwTid & 0xfc000000) == 0x08000000)
		DrawPartsSlot(graphic, row);
}

void CPost_MyItemList::DrawPartsSlot(FrGraphicInterface* graphic,
	FrListItem* row)
{
	int slot;
	sTradeItem* item = (sTradeItem*)row->pData;
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
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
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
			if (item->dwTid == (*it).partsTid && item->dwGuid == (*it).partsUid)
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
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
		if (part)
		{
			item->CaddieSlotNum = part->CaddieSlot;
			if ((int)item->CaddieSlotNum <= -1 || item->CaddieSlotNum >= 5)
				item->CaddieSlotNum = 0;
		}
		else
			item->CaddieSlotNum = 0;
	}
	for (slot = 1; slot < item->CaddieSlotNum + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTid == (*it).partsTid && item->dwGuid == (*it).partsUid)
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

void CPost_MyItemList::OnPost_ItemListBtnDown()
{
	FrListItem* row = m_pItemList->GetItemUnderCursor();
	if (row)
	{
		sTradeItem* item = (sTradeItem*)row->pData;
		if (!item)
		{
			CallErrorMsg(
				"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.");
			return;
		}
		if (!row->selected)
		{
			row->selected = true;
			CallErrorMsg(
				"\xc0\xcc\xb9\xcc \xc3\xdf\xb0\xa1\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.");
			return;
		}
		if (m_pSelectList->FindItem(row))
		{
			row->selected = false;
			CallErrorMsg(
				"\xb0\xb0\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xdf\xb0\xa1\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if (CTransferableItem::Instance()->Get_RegistrationListSize() >= 4)
		{
			row->selected = false;
			CallErrorMsg(
				"\xc0\xcc\xb9\xcc 4\xb0\xb3\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb8\xf0\xb5\xce \xbc\xb1\xc5\xc3\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if (!CTransferableItem::Instance()->IsCheckCardParts(item->dwGuid,
				item->dwTid))
		{
			row->selected = false;
			CallErrorMsg(
				"\xc4\xab\xb5\xe5\xb0\xa1 \xc0\xce\xc3\xa6\xc6\xae \xb5\xc8 \xc0\xc7\xbb\xf3\xc0\xba \xc7\xf6\xc0\xe7 \xbc\xb1\xb9\xb0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if ((item->dwTid & 0xfc000000) == 0x7c000000)
		{
			row->selected = false;
			CallErrorMsg(
				"\xc4\xab\xb5\xe5\xb4\xc2 \xc7\xf6\xc0\xe7 \xbc\xb1\xb9\xb0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if (item->iNum > 1)
		{
			if (!m_pItemCountDlg)
			{
				m_pCountItem = row;
				m_pItemCountDlg = CreateForm<CPost_ItemCountDlg>(
					g_pFresh->GetManager(), this, "post_itemcount");
				m_pItemCountDlg->SetRect(WRect(row->pos.x - 20.0f,
					row->pos.y + 80.0f, m_pItemCountDlg->GetRect().w,
					m_pItemCountDlg->GetRect().h));
				m_pItemCountDlg->SetSelectItem(item->dwGuid, item->iNum);
				m_pItemCountDlg->Open(
					(FRESH_PFN_RESULT)&CPost_MyItemList::OnPostItemCountResult,
					FrESCAPE | FrRET_CANCEL);
			}
		}
		else if (CTransferableItem::Instance()->SetRegistrationItem(
					 item->dwGuid, 1))
		{
			if (!m_pImgSelect->IsVisible())
				m_pImgSelect->SetVisible(true);
			if (!m_pSelectList->IsVisible())
				m_pSelectList->SetVisible(true);
			SetList();
		}
		else
		{
			row->selected = false;
			CallErrorMsg(
				"\xbf\xec\xc6\xed\xbf\xa1 \xc6\xf7\xc7\xd4\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbf\xc5\xb1\xe2\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
		}
	}
}

void CPost_MyItemList::OnPost_ItemListMouseMove()
{
}

void CPost_MyItemList::OnPost_ItemListRBtnDown()
{
	if (false)
		SetList();
}

void CPost_MyItemList::OnPost_ItemListDClick()
{
}

void CPost_MyItemList::OnPost_ImgSelect(int param)
{
	m_pImgSelect = DYNAMIC_CAST(FrArea, param);
	if (m_pImgSelect)
	{
		m_pImgSelect->SetVisible(false);
	}
}

void CPost_MyItemList::OnPost_SelectListInit(int param)
{
	m_pSelectList = DYNAMIC_CAST(FrListBox, param);
	if (m_pSelectList)
	{
		m_pSelectList->SetVisible(false);
	}
}

void CPost_MyItemList::OnPost_EditArrayInit(int param)
{
	m_pEditArray = DYNAMIC_CAST(FrEdit, param);
	if (m_pEditArray)
	{
		m_pEditArray->ClearLine();
		m_pEditArray->AddText("\xc0\xfc \xc3\xbc", false, true);
	}
}

void CPost_MyItemList::OnPost_SelectListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	sTradeItem* item = (sTradeItem*)row->pData;
	if (!item)
		return;
	if ((item->dwTid & 0xfc000000) == 0x08000000)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 255) | 0x04000000);
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

	WPoint pos;
	if (row->underCursor)
		pos = WPoint(row->pos.x + 3.0f, row->pos.y + 5.0f);
	else
		pos = WPoint(row->pos.x + 5.0f, row->pos.y + 7.0f);
	DrawItem(pos, item, 0);
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if ((row->underCursor || row->selected) &&
		(m_cancelBtn.rect + row->pos).IsInRect(mouse))
	{
		graphic->DrawTexture(m_cancelBtn.pOver, (m_cancelBtn.rect + row->pos),
			0xffffffff, 0);
	}
	else
	{
		graphic->DrawTexture(m_cancelBtn.pNormal, (m_cancelBtn.rect + row->pos),
			0xffffffff, 0);
	}
	if (IsLocalContent((localContentType_t)0x51) &&
		(item->dwTid & 0xfc000000) == 0x08000000)
		DrawPartsSlot(graphic, row);
}

void CPost_MyItemList::OnPost_SelectListBtnDown()
{
	FrListItem* row = m_pSelectList->GetItemUnderCursor();
	if (row)
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

		if ((m_cancelBtn.rect + row->pos).IsInRect(mouse))
		{
			sTradeItem* item = (sTradeItem*)row->pData;
			if (!item)
			{
				CallErrorMsg(
					"\xbc\xb1\xc5\xc3\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.");
				return;
			}
			if (CTransferableItem::Instance()->DelRegistrationItem(
					item->dwGuid))
			{
				if (!CTransferableItem::Instance()->Get_RegistrationListSize())
				{
					m_pSelectList->ClearItem();
					m_pSelectList->SetVisible(false);
					m_pImgSelect->SetVisible(false);
				}
				SetList();
			}
			else
			{
				row->selected = true;
				CallErrorMsg(
					"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xc5\xc3 \xc3\xeb\xbc\xd2 \xbd\xc7\xc6\xd0");
			}
		}
	}
}

void CPost_MyItemList::OnPost_SelectListMouseMove()
{
}

void CPost_MyItemList::OnPost_SelectListDClick()
{
}

void CPost_MyItemList::OnPostBtnArrayInit(int param)
{
	m_pBtnArray = DYNAMIC_CAST(FrButton, param);
}

void CPost_MyItemList::OnPostBtnArrayButtonUp()
{
	WRect rect = m_pBtnArray->GetRect();
	if (!m_pArrayDlg)
	{
		m_pArrayDlg = CreateForm<CStandingInLine>(g_pFresh->GetManager(), this,
			"arraydlg");
		if (m_pArrayDlg)
			m_pArrayDlg->Open(
				(FRESH_PFN_RESULT)&CPost_MyItemList::OnPostItemArrayResult,
				WPoint(rect.x, rect.y + rect.h), FrESCAPE);
	}
}

void CPost_MyItemList::DrawItem(WPoint pos, sTradeItem* item,
	unsigned char state)
{
	if (!item)
		return;
	unsigned long typeId = item->dwTid;
	int count = item->iNum;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic || !typeId)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
	if (!common)
		return;
	float offsetX = 0.0f, offsetY = 0.0f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent((localContentType_t)0x57) && (item->status & 1) &&
		IsUccClothes(item->dwTid))
	{
		const Bitmap* ucc = GetUccItemIcon(item->dwGuid);
		if (ucc)
			bitmap = ucc;
	}
	if ((typeId >> 26) == 2)
	{
		offsetX = -12.0f;
		offsetY = -20.0f;
	}
	if (bitmap)
	{
		WRect rect(pos.x + offsetX + 14.0f, pos.y + offsetY + 11.0f,
			(float)bitmap->Width(), (float)bitmap->Height());
		graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(4);
		if (IsLocalContent((localContentType_t)0x57) &&
			IsUccClothes(item->dwTid))
		{
			if ((item->status & 1) && item->Seq == 1 && m_pUccOriginal)
				graphic->DrawTexture(m_pUccOriginal,
					WRect(pos.x + 3.0f, pos.y + 15.0f,
						(float)m_pUccOriginal->Width(),
						(float)m_pUccOriginal->Height()),
					0xffffffff, 0);
			else if ((item->status & 1) && item->Seq > 1 && m_pUccCopied)
				graphic->DrawTexture(m_pUccCopied,
					WRect(pos.x + 3.0f, pos.y + 15.0f,
						(float)m_pUccCopied->Width(),
						(float)m_pUccCopied->Height()),
					0xffffffff, 0);
			else if ((item->status & 2) && m_pUccTempSaved)
				graphic->DrawTexture(m_pUccTempSaved,
					WRect(pos.x + 3.0f, pos.y + 15.0f,
						(float)m_pUccTempSaved->Width(),
						(float)m_pUccTempSaved->Height()),
					0xffffffff, 0);
		}
		if (count > 1)
		{
			unsigned int i = 0;
			const char* text = MakeStr("%d", count);
			float x = pos.x + 54.0f;
			for (i = 0; i < strlen(text); ++i)
				x -= m_pTitleFont->GetCharWidth(g_view, text[i] - '0');
			m_pTitleFont->Print(g_view, x, pos.y + 42.0f, text, 0, 0xffffffff,
				NULL);
		}
	}
	switch (typeId >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
		bitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", common->Level + 1));
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(pos.x + 38.0f - float2int((float)bitmap->Width() * 0.5f),
					pos.y + 35.0f - float2int((float)bitmap->Height() * 0.5f),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		break;
	}
}

void CPost_MyItemList::DrawPang()
{
	if (g_pFresh->GetManager()->GetGDI() && m_pPangFont)
	{
		char text[32] = { 0 };
		WRect rect = GetRect();
		static int fontWidth = m_pPangFont->GetFontInfo()->fontw;
		static float width;
		MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		width = (float)((strlen(text) - 1) * (fontWidth - 1.0f));
		m_pPangFont->Print(g_view, rect.x + 130.0f - width, rect.y + 483.0f,
			text, 0, 0xffffffff, NULL);
	}
}

void CPost_MyItemList::OnOK()
{
	FrForm::OnOK();
}

bool CPost_MyItemList::OnInit()
{
	SetList();
	return true;
}

void CPost_MyItemList::SetList()
{
	m_pItemList->ClearItem();
	m_pSelectList->ClearItem();
	std::list<sTradeItem>* available =
		CTransferableItem::Instance()->GetList_TransferableItem();
	std::list<sTradeItem>* selected =
		CTransferableItem::Instance()->GetList_RegistrationItem();
	for (std::list<sTradeItem>::iterator it = available->begin();
		it != available->end(); ++it)
	{
		sTradeItem* item = &*it;
		if ((item->dwTid & 0xfc000000) == 0x7c000000)
			continue;
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
		if (common && !common->IsSalable)
			continue;
		if (Doc()->IsEquipParts(item->dwGuid, item->dwTid))
			continue;
		FrListItem* row = NULL;
		if (IsLocalContent((localContentType_t)0x51))
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(it->dwTid);
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

		if (m_arrayType == -1)
		{
			row = m_pItemList->AddItem(item);
		}
		else if (m_arrayType >= 0 && m_arrayType < 10)
		{
			if (((it->dwTid >> 26) == 2 || (it->dwTid >> 26) == 1 ||
					(it->dwTid >> 26) == 9) &&
				((it->dwTid >> 18) & 255) == m_arrayType)
				row = m_pItemList->AddItem(item);
		}
		else if (m_arrayType == 10)
		{
			if ((it->dwTid & 0xfc000000) == 0x10000000)
				row = m_pItemList->AddItem(item);
		}
		else if (m_arrayType == 11)
		{
			if ((it->dwTid & 0xfc000000) == 0x18000000)
				row = m_pItemList->AddItem(item);
		}
		else if (m_arrayType == 12)
		{
			if ((it->dwTid & 0xfc000000) == 0x7c000000)
				row = m_pItemList->AddItem(item);
		}
		else if (m_arrayType == 13)
		{
			unsigned long group = it->dwTid >> 26;
			if (group == 7 || group == 8 || group == 28 || group == 16 ||
				group == 14)
				row = m_pItemList->AddItem(item);
		}
		if (row && m_arrayType == -1)
		{
			sTradeItem* data = (sTradeItem*)row->pData;
			for (std::list<sTradeItem>::iterator reg = selected->begin();
				reg != selected->end(); ++reg)
			{
				if (data->dwGuid == reg->dwGuid)
				{
					if (!m_pSelectList->IsVisible())
					{
						m_pSelectList->SetVisible(true);
						m_pImgSelect->SetVisible(true);
					}
					row->selected = true;
					break;
				}
			}
			continue;
		}
		for (std::list<sTradeItem>::iterator reg = selected->begin();
			reg != selected->end(); ++reg)
		{
			if (item->dwGuid == reg->dwGuid)
			{
				if (!m_pSelectList->IsVisible())
				{
					m_pSelectList->SetVisible(true);
					m_pImgSelect->SetVisible(true);
				}
				if (row)
					row->selected = true;
				break;
			}
		}
	}
	for (std::list<sTradeItem>::iterator reg = selected->begin();
		reg != selected->end(); ++reg)
		m_pSelectList->AddItem(&*reg);
}

void CPost_MyItemList::SetParent(CPost_SendDlg* parent)
{
	m_pParent = parent;
}

bool CPost_MyItemList::OnPostItemCountResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		m_count = 0;
		m_count = m_pItemCountDlg->GetCount();
		if (CTransferableItem::Instance()->SetRegistrationItem(
				m_pItemCountDlg->GetSelectItem(), m_count))
		{
			if (!m_pImgSelect->IsVisible())
				m_pImgSelect->SetVisible(true);
			if (!m_pSelectList->IsVisible())
				m_pSelectList->SetVisible(true);
			SetList();
		}
		else
		{
			m_pCountItem->selected = false;
			CallErrorMsg(
				"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xc5\xc3 \xc3\xeb\xbc\xd2 \xbd\xc7\xc6\xd0");
		}
	}
	else
	{
		m_pCountItem->selected = false;
		CallErrorMsg(
			"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xc5\xc3 \xc3\xeb\xbc\xd2");
	}
	m_pItemCountDlg = NULL;
	return true;
}

bool CPost_MyItemList::OnPostItemArrayResult(int result, FrForm* form)
{
	if (result == FrOK)
	{
		m_arrayType = m_pArrayDlg->GetSelectNum() - 1;
		m_pEditArray->ClearLine();
		if (m_arrayType == -1)
			m_pEditArray->AddText("\xc0\xfc \xc3\xbc", false, true);
		else
			m_pEditArray->AddText(MakeStr("%s", g_szArrayName[m_arrayType]),
				false, true);
		SetList();
	}
	m_pArrayDlg = NULL;
	return true;
}

CPost_ItemCountDlg::CPost_ItemCountDlg()
{
	m_pMinus = NULL;
	m_pPluse = NULL;
	m_pEditItemCount = NULL;
	m_count = 1;
}

CPost_ItemCountDlg::~CPost_ItemCountDlg()
{
}

IMPLEMENT_OBJECT(CPost_ItemCountDlg, FrForm)
BEGIN_FRESH_MSGMAP(CPost_ItemCountDlg, FrForm)
ON_FRESH_VI("btn_minus", FRCMD_INIT, CPost_ItemCountDlg::OnPostMinusInit)
ON_FRESH_VV("btn_minus", FRCMD_LBUTTONUP, CPost_ItemCountDlg::OnPostMinusBtnUp)
ON_FRESH_VI("btn_pluse", FRCMD_INIT, CPost_ItemCountDlg::OnPostPluseInit)
ON_FRESH_VV("btn_pluse", FRCMD_LBUTTONUP, CPost_ItemCountDlg::OnPostPluseBtnUp)
ON_FRESH_VI("input_count", FRCMD_INIT,
	CPost_ItemCountDlg::OnPostEditItemCountInit)
ON_FRESH_BI("input_count", FRCMD_ENTERKEY,
	CPost_ItemCountDlg::OnPostEditItemCountEnterKey)
END_FRESH_MSGMAP()

void CPost_ItemCountDlg::OnPostMinusInit(int param)
{
	m_pMinus = DYNAMIC_CAST(FrButton, param);
	if (m_pMinus)
	{
		m_pMinus->SetPushDelay(0.0f);
	}
}

void CPost_ItemCountDlg::OnPostMinusBtnUp()
{
	m_count = atoi(m_pEditItemCount->GetLine(1, false)) - 1;
	if (m_count < 1)
		m_count = 1;
	SetText();
}

void CPost_ItemCountDlg::OnPostPluseInit(int param)
{
	m_pPluse = DYNAMIC_CAST(FrButton, param);
	if (m_pPluse)
	{
		m_pPluse->SetPushDelay(0.0f);
	}
}

void CPost_ItemCountDlg::OnPostPluseBtnUp()
{
	m_count = atoi(m_pEditItemCount->GetLine(1, false)) + 1;
	if (m_count > m_maxCount)
		m_count = m_maxCount;
	SetText();
}

void CPost_ItemCountDlg::OnPostEditItemCountInit(int param)
{
	m_pEditItemCount = DYNAMIC_CAST(FrEdit, param);
	if (m_pEditItemCount)
		m_pEditItemCount->SetCharLimit(2, true);
	SetText();
}

int CPost_ItemCountDlg::GetCount()
{
	WRect rect = GetRect();
	m_count = atoi(m_pEditItemCount->GetLine(1, false));
	int maximum = m_maxCount;
	m_count = m_count < 1 ? 1 : (m_count > maximum ? maximum : m_count);
	return m_count;
}

void CPost_ItemCountDlg::SetText()
{
	WRect rect = GetRect();
	if (m_pEditItemCount)
	{
		m_pEditItemCount->ClearLine();
		m_pEditItemCount->AddText(MakeStr("%d", m_count), false, true);
	}
}

void CPost_ItemCountDlg::SetSelectItem(unsigned long item, int maxCount)
{
	m_selectItem = item;
	m_maxCount = maxCount;
}

unsigned long CPost_ItemCountDlg::GetSelectItem()
{
	return m_selectItem;
}

bool CPost_ItemCountDlg::OnPostEditItemCountEnterKey(int param)
{
	OnOK();
	return true;
}
