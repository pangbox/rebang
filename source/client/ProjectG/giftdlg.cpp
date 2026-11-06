#include "minatl.h"
#include "giftdlg.h"
#include "checkoutdlg.h"
#include "frienddlg.h"
#include "emoticondlg.h"
#include "frcombobox.h"
#include "browser.h"
#include "logininfo.h"
#include "buddymanager.h"
#include "binstr.h"
#include "mathconsts.h"
#include "../../shared/s5/sharedutilities.h"
#include <wininet.h>

extern Fresh* g_pFresh;
extern WView* g_view;

extern days_t days[];

IMPLEMENT_OBJECT(FrGiftDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrGiftDlg, FrForm)
ON_FRESH_VI("checkout", FRCMD_INIT, FrGiftDlg::OnCheckoutInit)
ON_FRESH_VV("checkout", FRCMD_LBUTTONUP, FrGiftDlg::OnCheckoutBtnUp)
ON_FRESH_VI("friend", FRCMD_INIT, FrGiftDlg::OnFriendBtnInit)
ON_FRESH_VV("friend", FRCMD_LBUTTONUP, FrGiftDlg::OnFriendBtnUp)
ON_FRESH_VI("cart", FRCMD_INIT, FrGiftDlg::OnCartInit)
ON_FRESH_VI("cart", FRCMD_OWNERDRAW, FrGiftDlg::OnCartOwnerDraw)
ON_FRESH_VI("sxid", FRCMD_INIT, FrGiftDlg::OnSxIdInit)
ON_FRESH_VI("edid", FRCMD_INIT, FrGiftDlg::OnEdIdInit)
ON_FRESH_BI("edid", FRCMD_ENTERKEY, FrGiftDlg::OnEdIdEnterKey)
ON_FRESH_VV("edid", FRCMD_LBUTTONDOWN, FrGiftDlg::OnEdIdDown)
ON_FRESH_VI("id_check", FRCMD_INIT, FrGiftDlg::OnBtIdCheckInit)
ON_FRESH_VV("id_check", FRCMD_LBUTTONUP, FrGiftDlg::OnBtIdCheckUp)
ON_FRESH_VI("sxnick", FRCMD_INIT, FrGiftDlg::OnSxNickInit)
ON_FRESH_VI("ednick", FRCMD_INIT, FrGiftDlg::OnEdNickInit)
ON_FRESH_BI("ednick", FRCMD_ENTERKEY, FrGiftDlg::OnEdNickEnterKey)
ON_FRESH_VV("ednick", FRCMD_LBUTTONDOWN, FrGiftDlg::OnEdNickDown)
ON_FRESH_VI("emoticon", FRCMD_INIT, FrGiftDlg::OnEmoticonBtnInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONUP, FrGiftDlg::OnEmoticonBtnUp)
ON_FRESH_VI("nick_check", FRCMD_INIT, FrGiftDlg::OnBtNickCheckInit)
ON_FRESH_VV("nick_check", FRCMD_LBUTTONUP, FrGiftDlg::OnBtNickCheckUp)
ON_FRESH_VI("message", FRCMD_INIT, FrGiftDlg::OnMsgInit)
ON_FRESH_BI("message", FRCMD_ENTERKEY, FrGiftDlg::OnMsgEnterKey)
ON_FRESH_VI("confirm", FRCMD_INIT, FrGiftDlg::OnEdConfirmInit)
ON_FRESH_VI("price", FRCMD_INIT, FrGiftDlg::OnPriceInit)
ON_FRESH_VI("my_money", FRCMD_INIT, FrGiftDlg::OnMyMoneyInit)
ON_FRESH_VI("money_left", FRCMD_INIT, FrGiftDlg::OnMoneyLeftInit)
ON_FRESH_VI("price_cookie", FRCMD_INIT, FrGiftDlg::OnPrice_CookieInit)
ON_FRESH_VI("my_money_cookie", FRCMD_INIT, FrGiftDlg::OnMyMoney_CookieInit)
ON_FRESH_VI("money_left_cookie", FRCMD_INIT, FrGiftDlg::OnMoneyLeft_CookieInit)
ON_FRESH_VI("num_static", FRCMD_INIT, FrGiftDlg::OnNumStaticInit)
ON_FRESH_VI("num", FRCMD_INIT, FrGiftDlg::OnNumInit)
ON_FRESH_VI("num_prev", FRCMD_INIT, FrGiftDlg::OnNumPrevInit)
ON_FRESH_VV("num_prev", FRCMD_LBUTTONUP, FrGiftDlg::OnNumPrevBtnUp)
ON_FRESH_VI("num_next", FRCMD_INIT, FrGiftDlg::OnNumNextInit)
ON_FRESH_VV("num_next", FRCMD_LBUTTONUP, FrGiftDlg::OnNumNextBtnUp)
ON_FRESH_VI("days", FRCMD_INIT, FrGiftDlg::OnDaysInit)
ON_FRESH_VV("days", FRCMD_LBUTTONDOWN, FrGiftDlg::OnDaysBtnDown)
ON_FRESH_VI("warn", FRCMD_INIT, FrGiftDlg::OnWarnInit)
ON_FRESH_VI("explane", FRCMD_INIT, FrGiftDlg::OnExplaneStaticInit)
END_FRESH_MSGMAP()

FrGiftDlg::FrGiftDlg()
{
	memset(&m_item, 0, sizeof(m_item));
	m_receiverUid = -1;
	m_receiver = "";
	m_enoughPang = false;
	m_enoughCookie = false;
	m_pFriendBtn = NULL;
	m_pCheckout = NULL;
	m_pCart = NULL;
	m_pPrice = NULL;
	m_pMyMoney = NULL;
	m_pMoneyLeft = NULL;
	m_pPriceCookie = NULL;
	m_pMyMoneyCookie = NULL;
	m_pMoneyLeftCookie = NULL;
	m_pNumStatic = NULL;
	m_pNum = NULL;
	m_pNumPrev = NULL;
	m_pNumNext = NULL;
	m_pFriendForm = NULL;
	m_pBtNickCheck = NULL;
	m_pWarn = NULL;
	m_pExplane = NULL;
	m_pSxNick = NULL;
	m_pEmoticonBtn = NULL;
	m_pEmoticonForm = NULL;
	m_initialPang = 0;
	m_initialCookie = 0;
	m_remainingCash = 0;
	m_showCashInfo = false;
	m_childCount = 0;
	memset(m_childOptions, 0, sizeof(m_childOptions));
	m_receiverMode = -1;
}

void FrGiftDlg::SendGift(const IFF_ITEM_COMMON& item, __int64 pang,
	__int64 cash)
{
	m_item = item;
	m_totalPang = 0;
	m_totalCookie = 0;
	m_enoughPang = false;
	m_enoughCookie = false;
	if (m_item.IsCash)
	{
		if (m_pWarn)
			m_pWarn->SetVisible(false);
	}
	else
	{
		if (m_pSxId)
			m_pSxId->SetVisible(false);
		if (m_pEdId)
			m_pEdId->SetVisible(false);
		if (m_pBtIdCheck)
			m_pBtIdCheck->SetVisible(false);
		if (m_pSxNick)
			m_pSxNick->SetVisible(false);
		if (m_pEdNick)
			m_pEdNick->SetVisible(false);
		if (m_pBtNickCheck)
			m_pBtNickCheck->SetVisible(false);
		if (m_pMessage)
			m_pMessage->SetKeyFocus(true);
		if (m_pWarn)
			m_pWarn->SetVisible(true);
		if (m_pEmoticonBtn)
			m_pEmoticonBtn->SetVisible(false);
	}
	m_periodIndex = 1;
	unsigned long group = m_item.TypeId >> 26;
	unsigned long type;
	if (group == 14)
	{
		IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(item.TypeId);
		if (skin && !S5::ISCutinItem(item.TypeId))
		{
			for (int i = 0; i < 5; ++i)
				if (skin->COM[i])
				{
					m_periodIndex = i;
					break;
				}
		}
	}
	else if (group == 8)
	{
		IFF_STRUCT::sCadItem* cad = ItemManager()->FindCadItem(item.TypeId);
		for (int i = 0; i < 4; ++i)
			if (cad->COM[i])
			{
				m_periodIndex = i;
				break;
			}
	}
	else if (group == 16)
	{
		IFF_STRUCT::sMascot* mascot = ItemManager()->FindMascot(item.TypeId);
		for (int i = 0; i < 4; ++i)
			if (mascot->COM[i])
			{
				m_periodIndex = i;
				break;
			}
	}
	else
	{
		m_childCount = ItemManager()->FindChildItemNumber(item.TypeId);
		if (m_childCount > 0)
		{
			memset(m_childOptions, 0, sizeof(m_childOptions));
			if (m_childCount > 5)
				m_childCount = 5;
			for (int i = 0; i < m_childCount; ++i)
			{
				IFF_STRUCT::sChildItem* child =
					ItemManager()->FindChildItem(item.TypeId, i);
				if (!child || !(child->IsDisplay))
					continue;
				if (child->NumberFlag)
				{
					sprintf(m_childOptions[i].label, "%d\xb0\xb3",
						(unsigned int)child->Number);
					m_childOptions[i].currency = child->IsCash;
					m_childOptions[i].quantityPrice = child->NumberPrice;
					m_childOptions[i].quantityMode = child->NumberFlag;
					m_childOptions[i].quantity = child->Number;
				}
				else if (child->TimeFlag)
				{
					switch (child->TimeFlag)
					{
					case 0:
						break;
					case 3:
						sprintf(m_childOptions[i].label, "%d\xbd\xc3\xb0\xa3",
							(unsigned int)child->Time);
						break;
					case 4:
						sprintf(m_childOptions[i].label, "%d\xc0\xcf",
							(unsigned int)child->Time);
						break;
					case 5:
						sprintf(m_childOptions[i].label, "%d\xb0\xb3\xbf\xf9",
							(unsigned int)child->Time);
						break;
					default:
						sprintf(m_childOptions[i].label, "%d\xc0\xcf",
							(unsigned int)child->Time);
						break;
					}
					m_childOptions[i].currency = child->IsCash;
					m_childOptions[i].durationPrice = child->TimePrice;
					m_childOptions[i].durationMode = child->TimeFlag;
					m_childOptions[i].duration = child->Time;
				}
			}
		}
	}
	m_initialPang = pang;
	m_initialCookie = cash;
	RecalculateTotalCookie(pang, cash, true);
	type = m_item.TypeId;
	bool fixed_count = false;
	if ((type & 0xfc000000) == 0x18000000 && !(type & 0x2000000))
	{
		IFF_STRUCT::sItem* item_info = ItemManager()->FindItem(type);
		if (item_info->COM[0] > 0)
		{
			fixed_count = true;
			if (m_pNum)
				m_pNum->SetLine(1, MakeStr("%d", (int)item_info->COM[0]), 0,
					false, 0);
		}
	}
	type = m_item.TypeId;
	if ((type & 0x2000000) || (type & 0xfc000000) != 0x18000000 || fixed_count)
	{
		if (m_pNumStatic)
			m_pNumStatic->SetVisible(false);
		if (m_pNum)
			m_pNum->SetVisible(false);
		if (m_pNumPrev)
			m_pNumPrev->SetVisible(false);
		if (m_pNumNext)
			m_pNumNext->SetVisible(false);
	}
	else
	{
		if (m_pNum)
		{
			WPoint origin = m_pNum->GetRect().TopLeft();
			WPoint point(origin.x + (m_item.IsCash ? 50 : -50), origin.y);
			m_pNum->MoveWindow(point);
		}
		if (m_pNumPrev)
		{
			WPoint origin = m_pNumPrev->GetRect().TopLeft();
			WPoint point(origin.x + (m_item.IsCash ? 50 : -50), origin.y);
			m_pNumPrev->MoveWindow(point);
		}
		if (m_pNumNext)
		{
			WPoint origin = m_pNumNext->GetRect().TopLeft();
			WPoint point(origin.x + (m_item.IsCash ? 50 : -50), origin.y);
			m_pNumNext->MoveWindow(point);
		}
	}
	m_rebuildDays = true;
	m_pDays->SetVisible(false);
}

void FrGiftDlg::GiftProgress(void)
{
	FrListBox::ITEM_LIST& list = m_pCart->m_itemList;
	const char* text = m_pMessage->GetLine(1, false);
	Doc()->m_chatManager.FilteringHack(text, true);
	unsigned long count = atoi(m_pNum->GetLine(1, false));
	sBuyItem item;
	if (m_childCount > 0)
	{
		if ((m_item.TypeId & 0xfc000000) == 0x18000000)
		{
			WSendPacket packet((enumClientPacket)0x1f);
			packet.EncodeStr(m_packetText);
			packet.Encode4(m_receiverUid);
			packet.EncodeStr(std::string(text));
			packet.Encode1(1);
			if (m_item.TypeId & 0x2000000)
			{
				count = m_childOptions[m_periodIndex].quantityMode
					? m_childOptions[m_periodIndex].quantity
					: 1;
				item.ItemCount = count;
			}
			else
				item.ItemCount = count;
			packet.Encode2(1);
			item.TypeCode = m_item.TypeId;
			item.DayCount = m_childOptions[m_periodIndex].duration;
			item.sChildItemIndex = (unsigned short)m_periodIndex;
			packet.EncodeBuffer(&item, 16);
			packet.Send(TO_GAME);
		}
		else
		{
			unsigned long size = list.size();
			WSendPacket packet((enumClientPacket)0x1f);
			packet.EncodeStr(m_packetText);
			packet.Encode4(m_receiverUid);
			packet.EncodeStr(std::string(text));
			packet.Encode1(0);
			packet.Encode2((unsigned short)size);
			for (FrListBox::ITEM_LIST::iterator it = list.begin();
				it != list.end(); ++it)
			{
				IFF_ITEM_COMMON* info = (IFF_ITEM_COMMON*)(*it)->pData;
				item.TypeCode = info->TypeId;
				item.DayCount = m_childOptions[m_periodIndex].duration;
				item.sChildItemIndex = (unsigned short)m_periodIndex;
				packet.EncodeBuffer(&item, 16);
			}
			packet.Send(TO_GAME);
		}
	}
	else
	{
		count = 1;
		unsigned long size = list.size();
		WSendPacket packet((enumClientPacket)0x1f);
		packet.EncodeStr(m_packetText);
		packet.Encode4(m_receiverUid);
		packet.EncodeStr(std::string(text));
		packet.Encode1(0);
		packet.Encode2((unsigned short)size);
		for (FrListBox::ITEM_LIST::iterator it = list.begin(); it != list.end();
			++it)
		{
			IFF_ITEM_COMMON* info = (IFF_ITEM_COMMON*)(*it)->pData;
			item.TypeCode = info->TypeId;
			item.sChildItemIndex = -1;
			unsigned long type = m_item.TypeId;
			if (type >> 26 == 5)
			{
				IFF_STRUCT::sBall* ball = ItemManager()->FindBall(type);
				if (ball)
					count = ball->COM[0];
			}
			else if (type >> 26 == 6 && (type & 0x2000000))
			{
				IFF_STRUCT::sItem* info = ItemManager()->FindItem(type);
				if (info)
					count = info->COM[0];
			}
			else if (type >> 26 == 6 && !(type & 0x2000000) && m_item.IsCash &&
				(unsigned short)size == 1)
			{
				IFF_STRUCT::sItem* info = ItemManager()->FindItem(type);
				if (info)
					count = info->COM[0];
			}
			type = m_item.TypeId;
			if (type >> 26 == 14 || type >> 26 == 8 || type >> 26 == 16)
			{
				if (!S5::ISCutinItem(type))
					item.DayCount = days[m_periodIndex].days;
			}
			else
				item.DayCount = 0;
			item.ItemCount = count;
			packet.EncodeBuffer(&item, 16);
		}
		packet.Send(TO_GAME);
	}
	Doc()->m_giftName = std::string(m_pEdNick->GetLine(1, false));
	Doc()->m_giftIndex = m_receiverUid;
	AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0xc0, 0, 0, 0, 0, 0);
	AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	Close(FrOK, true);
}

void FrGiftDlg::SetReceiverInfo(unsigned long uid, const sPangYaUserInfo& info)
{
	m_receiverUid = uid;
	if (m_pEdNick)
	{
		m_pEdNick->SetLine(1, info.sNick, 0, false, 0);
	}
	if (m_pEdConfirm)
	{
		switch (m_receiverMode)
		{
		case 0:
		{
			if (m_pEdId)
				m_pEdId->ClearLine();
			std::string masked(info.sID);
			unsigned int length = masked.length();
			masked.erase(masked.begin() + (length >> 1), masked.end());
			unsigned int stars = length - masked.length();
			masked.insert(masked.end(), stars, '*');
			if (m_pEdConfirm)
				m_pEdConfirm->SetLine(1, masked.c_str(), 0, false, 0);
			m_receiver = info.sID;
			break;
		}
		case 1:
			m_pEdConfirm->SetLine(1, info.sNick, 0, false, 0);
			m_receiver = info.sNick;
			break;
		}
	}
}

void FrGiftDlg::SetPrice(int count)
{
	m_enoughPang = true;
	m_enoughCookie = true;
	if (m_pPrice && m_pMyMoney && m_totalPang)
	{
		char buffer[32] = "";
		MakePriceStrA(buffer, m_totalPang * count);
		strcat(buffer, "\xc6\xce");
		m_pPrice->SetLine(1, buffer, 0, false, 0);
		__int64 money = Doc()->m_myInfo.stat.i64Pang;
		__int64 price = m_totalPang * count;
		__int64 left = money - price;
		if (left < 0)
		{
			if (m_pMoneyLeft)
			{
				MakePriceStrA(buffer, price - money);
				strcat(buffer, "\xc6\xce \xba\xce\xc1\xb7");
				m_pMoneyLeft->SetLine(1, buffer, 0xffff0000, false, 0);
			}
			m_enoughPang = false;
		}
		else if (m_pMoneyLeft)
		{
			MakePriceStrA(buffer, left);
			strcat(buffer, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, buffer, 0, false, 0);
		}
	}
	else if (m_pMoneyLeft)
		m_pMoneyLeft->SetLine(1, m_pMyMoney->GetLine(1, false), 0, false, 0);
	if (m_pPriceCookie && m_pMyMoneyCookie && m_totalCookie)
	{
		char buffer[32] = "";
		sprintf(buffer, "%d\xc4\xed\xc5\xb0", m_totalCookie);
		m_pPriceCookie->SetLine(1, buffer, 0, false, 0);
		__int64 money = Doc()->m_cookie;
		__int64 price = m_totalCookie;
		__int64 left = money - price;
		if (left < 0)
		{
			if (m_pMoneyLeftCookie)
			{
				MakePriceStrA(buffer, price - money);
				strcat(buffer, "\xc4\xed\xc5\xb0 \xba\xce\xc1\xb7");
				m_pMoneyLeftCookie->SetLine(1, buffer, 0xffff0000, false, 0);
			}
			m_enoughCookie = false;
		}
		else if (m_pMoneyLeftCookie)
		{
			sprintf(buffer, "%d\xc4\xed\xc5\xb0", left);
			m_pMoneyLeftCookie->SetLine(1, buffer, 0, false, 0);
		}
	}
	else if (m_pMoneyLeftCookie)
		m_pMoneyLeftCookie->SetLine(1, m_pMyMoneyCookie->GetLine(1, false), 0,
			false, 0);
	if (m_pNum)
		m_pNum->SetLine(1, MakeStr("%d", count), 0, false, 0);
}

bool FrGiftDlg::OnFriendListResult(int result, FrForm* form)
{
	if (result == 1)
	{
		m_receiverMode = 0;
		m_receiverUid = m_pFriendForm->GetUid();
		m_receiver = m_pEdConfirm->GetLine(1, false);
	}
	m_pFriendForm = NULL;
	return true;
}

bool FrGiftDlg::OnChargeResult(int result, FrForm*)
{
	if (result == 1)
	{
		CBrowser::Instance()->InitializePostArgument();
		switch (Doc()->m_provType)
		{
		case 2:
		{
			CBrowser::Instance()->AddPostArgument("sn", "9");
			CLoginInfo* login = CLoginInfo::Instance();
			CBrowser* browser = CBrowser::Instance();
			browser->AddPostArgument("mn", MakeStr("%d", login->AuthUid()));
			CBrowser::Instance()->OpenPost(
				"https://qa.www.gametree.co.kr/InGame/Gateway.aspx", false,
				true);
			break;
		}
		case 4:
		{
			const char* cookie =
				((NhnLogin*)CLoginInfo::Instance()->QueryInterface())
					->GetSiteCookie();
			if (!InternetSetCookieA("https://alpha-bill.hangame.com/", 0,
					cookie))
			{
				GetLastError();
				MessageBoxA(0, "Cookie Setting Fail!", "Error", 0);
				return false;
			}
			CBrowser::Instance()->AddPostArgument("CHNL", "HG");
			CBrowser::Instance()->OpenPost(
				"https://alpha-bill.hangame.com/pay/charge.nhn", false, true);
			break;
		}
		default:
			return false;
		}
		MsgObject event(NULL, 0xc2, 0, 0, 0, 0, 0);
		CTask* task = AfxGetTask();
		IActor* actor = task->GetActor("Shop");
		actor << event;
		g_input->Reset();
		Close(FrCANCEL, true);
	}
	return true;
}

bool FrGiftDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == 1)
		GiftProgress();
	return true;
}

void FrGiftDlg::OnCheckoutInit(int param)
{
	m_pCheckout = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrGiftDlg::OnCheckoutBtnUp()
{
	if (m_receiverMode == -1 || m_receiverUid == -1)
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xba\xb8\xb3\xbb\xb1\xe2 \xc0\xfc\xbf\xa1 \xb9\xde\xb4\xc2 \xc0\xcc\xb0\xa1 \xc8\xae\xbd\xc7\xc8\xf7 \xb8\xc2\xb4\xc2\xc1\xf6\n\xb2\xc0 \\c0xffff0000\\c'\xc8\xae\xc0\xce'\\c0xff000000\\c \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4",
			0, 0, 0, 0);
	}
	else if (Doc()->m_myInfo.info.IsIdentity(0x100))
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb8\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe8\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else if ((m_receiverMode == 0 && m_receiver == MyNick()) ||
		(m_receiverMode == 1 && m_receiver == MyNick()))
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4\xb4\xc2 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else
	{
		if (!m_enoughPang)
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xc6\xce\xc0\xcc \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\n\xb0\xd4\xc0\xd3\xbf\xa1 \xc2\xfc\xbf\xa9\xc7\xd8\xbc\xad \xc6\xce\xc0\xbb \xb8\xf0\xc0\xb8\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			return;
		}
		if (!m_enoughCookie)
		{
			FrForm* dialog = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_okcancel", NULL);
			dialog->SetMessage(
				"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\n\xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
				false);
			dialog->Open((FRESH_PFN_RESULT)&FrGiftDlg::OnChargeResult, 3);
			return;
		}
		if (!ItemManager()->IsInSalePeriod(m_item.TypeId,
				Doc()->GetServerTime()))
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			return;
		}
		FrForm* dialog = CreateForm<FrForm>(g_pFresh->GetManager(), this,
			"notify_okcancel_big", NULL);
		std::string text(MakeStr(
			"'\\c0xffff0000\\c%s\\c0xff000000\\c'\xb4\xd4\xbf\xa1\xb0\xd4 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbb\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			m_pEdNick->GetLine(1, false)));
		text += " ";
		text +=
			"\n\\c0xffff0000\\c\xbc\xb1\xb9\xb0\xc7\xcf\xbd\xc3\xb1\xe2 \xc0\xfc\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb1\xe2\xc7\xd1\xb0\xfa \xbc\xf6\xb7\xae\xc0\xbb \xb2\xc0 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.\\c0xff000000\\c";
		text +=
			"\n\\c0xffff0000\\c\xc0\xcc \xbb\xf3\xc7\xb0\xc0\xba \xc3\xbb\xbe\xe0\xc3\xb6\xc8\xb8(\xb1\xb8\xb8\xc5\xc3\xeb\xbc\xd2)\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9.\\c0xff000000\\c";
		dialog->SetMessage(text.c_str(), false);
		dialog->Open((FRESH_PFN_RESULT)&FrGiftDlg::OnConfirmResult, 1);
	}
}

void FrGiftDlg::OnFriendBtnInit(int param)
{
	m_pFriendBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrGiftDlg::OnFriendBtnUp(void)
{
	if (!BuddyMGR->GetBuddySize())
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
	if (!m_pFriendForm)
	{
		FrFriendDlg* dialog = CreateForm<FrFriendDlg>(g_pFresh->GetManager(),
			this, "friend_list_form", 0);
		m_pFriendForm = dialog;
		if (dialog)
		{
			m_pEdId->ClearLine();
			m_pFriendForm->SetParent(this);
			m_pFriendForm->EnableDrag(false);
			WPoint point;
			point.x = 582.0f;
			point.y = 35.0f;
			m_pFriendForm->Open(
				static_cast<FRESH_PFN_RESULT>(&FrGiftDlg::OnFriendListResult),
				point, 1);
		}
	}
}

void FrGiftDlg::OnCartInit(int param)
{
	m_pCart = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_miniBtn[6].bitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_miniBtn[6].rect = WRect(40.f, 35.f, (float)m_miniBtn[6].bitmap->Width(),
		(float)m_miniBtn[6].bitmap->Height());
}

void FrGiftDlg::OnCartOwnerDraw(int argument)
{
	if (!argument)
		return;
	FrListItem* draw = (FrListItem*)argument;
	FrWndManager* manager = m_pCart->WndManager();
	FrGraphicInterface* graphic = manager->GetGDI();
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)draw->pData;
	manager->GetBitmap("ITEMS", item->Icon);
	const Bitmap* bitmap;
	unsigned long type = item->TypeId;
	if (type >> 26 == 14)
		bitmap = m_pCart->WndManager()->GetBitmap("SKINS", item->Icon);
	else if (type >> 26 == 15)
		bitmap = m_pCart->WndManager()->GetBitmap("HAIRS", item->Icon);
	else
	{
		bitmap = m_pCart->WndManager()->GetBitmap("ITEMS", item->Icon);
		if (!bitmap)
			bitmap =
				m_pCart->WndManager()->GetBitmap("ITEMS_FASHION", item->Icon);
	}
	if (bitmap)
	{
		WRect rect(draw->pos.x, draw->pos.y, bitmap->Width(), bitmap->Height());
		type = item->TypeId;
		if (type >> 26 != 2 && type >> 26 != 15)
		{
			rect.x += 10.0f;
			rect.y += 20.0f;
		}
		graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
		if (m_childCount > 0 && m_childOptions[m_periodIndex].durationMode)
		{
			WPoint point(draw->pos.x, draw->pos.y + 10.0f);
			WRect rect(point.x + m_miniBtn[6].rect.x,
				point.y + m_miniBtn[6].rect.y, m_miniBtn[6].rect.w,
				m_miniBtn[6].rect.h);
			graphic->DrawTexture(m_miniBtn[6].bitmap, rect, 0xffffffff, 0);
		}
	}
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->SetTextStyle(1);
	float title =
		graphic->Print(WPoint(90.0f, 14.0f) + draw->pos, 0, "%s%s", item->Name,
			item->InStock == 2 ? "(\xbc\xb1\xb9\xb0\xc0\xfc\xbf\xeb)" : "");
	CItemManager* items = ItemManager();
	if (!items->IsInSalePeriod(item->TypeId, Doc()->GetServerTime()))
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->Print(WPoint((float)(int)title + 90.0f, 14.0f) + draw->pos, 0,
			"(\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3 \xc1\xbe\xb7\xe1)");
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	graphic->SetTextStyle(0);
	int pricewidth;
	int width;
	if (m_childCount > 0)
	{
		__int64 price = 0;
		if (m_childOptions[m_periodIndex].quantityMode)
			price = m_childOptions[m_periodIndex].quantityPrice;
		else if (m_childOptions[m_periodIndex].durationMode)
			price = m_childOptions[m_periodIndex].durationPrice;
		if (m_childOptions[m_periodIndex].currency)
		{
			width = (int)graphic->Print(WPoint(90.0f, 34.0f) + draw->pos, 0,
				"\xb0\xa1    \xb0\xdd:   ");
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%I64d\xc4\xed\xc5\xb0", price);
		}
		else
		{
			width = (int)graphic->Print(WPoint(90.0f, 34.0f) + draw->pos, 0,
				"\xb0\xa1    \xb0\xdd:   ");
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%I64d\xc6\xce", price);
		}
	}
	else if (item->IsCash)
	{
		width = (int)graphic->Print(WPoint(90.0f, 34.0f) + draw->pos, 0,
			"\xb0\xa1    \xb0\xdd:   ");
		type = item->TypeId;
		if (type >> 26 == 14)
		{
			IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(type);
			if (!S5::ISCutinItem(item->TypeId))
			{
				if (skin)
				{
					graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
							draw->pos,
						0, "%d\xc4\xed\xc5\xb0", (int)skin->COM[m_periodIndex]);
				}
			}
			else
			{
				if (skin)
				{
					if (item->SalePrice > 0 && item->SalePrice < item->Price)
					{
						graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
								draw->pos,
							0, "%d\xc4\xed\xc5\xb0", item->SalePrice);
					}
					else
					{
						graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
								draw->pos,
							0, "%d\xc4\xed\xc5\xb0", item->Price);
					}
				}
			}
		}
		else if (type >> 26 == 8)
		{
			IFF_STRUCT::sCadItem* cad = ItemManager()->FindCadItem(type);
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d\xc4\xed\xc5\xb0", (int)cad->COM[m_periodIndex]);
		}
		else
		{
			pricewidth = float2int(
				graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos,
					0, "%d\xc4\xed\xc5\xb0", item->Price));
			if (item->SalePrice > 0 && item->SalePrice < item->Price)
			{
				float sum = (float)pricewidth + (float)width;
				g_view->DrawLine2D(
					WPoint(((float)width + 90.0f) - 2.0f,
						(float)float2int(
							(float)graphic->GetFontHeight() * 0.5f) +
							34.0f) +
						draw->pos,
					WPoint(sum + 92.0f,
						(float)float2int(
							(float)graphic->GetFontHeight() * 0.5f) +
							34.0f) +
						draw->pos,
					0xff000000, 0xff000000, 0);
				graphic->SetTextColor(0xffff0000, 0xffffffff);
				graphic->Print(WPoint(sum + 90.0f, 34.0f) + draw->pos, 0,
					" %d\xc4\xed\xc5\xb0", item->SalePrice);
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}
		}
	}
	else
	{
		width = (int)graphic->Print(WPoint(90.0f, 34.0f) + draw->pos, 0,
			"\xb0\xa1    \xb0\xdd:   ");
		type = item->TypeId;
		if (type >> 26 == 14)
		{
			if (!S5::ISCutinItem(type))
			{
				IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(item->TypeId);
				if (skin)
				{
					graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
							draw->pos,
						0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
						skin->COM[m_periodIndex],
						skin->COM[m_periodIndex] * 5 / 100);
				}
			}
			else
			{
				if (item->SalePrice > 0 && item->SalePrice < item->Price)
				{
					graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
							draw->pos,
						0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
						item->SalePrice, item->SalePrice * 5 / 100);
				}
				else
				{
					graphic->Print(WPoint((float)width + 90.0f, 34.0f) +
							draw->pos,
						0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
						item->Price, item->Price * 5 / 100);
				}
			}
		}
		else if (type >> 26 == 8)
		{
			IFF_STRUCT::sCadItem* cad = ItemManager()->FindCadItem(type);

			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
				cad->COM[m_periodIndex], cad->COM[m_periodIndex] * 5 / 100);
		}
		else if (type >> 26 == 16)
		{
			IFF_STRUCT::sMascot* mascot = ItemManager()->FindMascot(type);

			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
				mascot->COM[m_periodIndex],
				mascot->COM[m_periodIndex] * 5 / 100);
		}
		else
		{
			pricewidth = float2int(
				graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos,
					0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
					item->Price, item->SalePrice * 5 / 100));
			if (item->SalePrice > 0 && item->SalePrice < item->Price)
			{
				float sum = (float)pricewidth + (float)width;
				g_view->DrawLine2D(
					WPoint(((float)width + 90.0f) - 2.0f,
						(float)float2int(
							(float)graphic->GetFontHeight() * 0.5f) +
							34.0f) +
						draw->pos,
					WPoint(sum + 92.0f,
						(float)float2int(
							(float)graphic->GetFontHeight() * 0.5f) +
							34.0f) +
						draw->pos,
					0xff000000, 0xff000000, 0);
				graphic->SetTextColor(0xffff0000, 0xffffffff);
				graphic->Print(WPoint(sum + 90.0f, 34.0f) + draw->pos, 0,
					" %d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)",
					item->SalePrice, item->SalePrice * 5 / 100);
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}
		}
	}
	_SYSTEMTIME* periodTimes[2] = { 0, 0 };
	float row = 54.0f;
	if (ItemManager()->HasSalePeriod(item->TypeId, &periodTimes[0],
			&periodTimes[1]) &&
		periodTimes[0]->wYear && periodTimes[1]->wYear)
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->Print(WPoint(90.0f, 54.0f) + draw->pos, 0,
			MakeStr(
				"\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf",
				(int)periodTimes[0]->wMonth, (int)periodTimes[0]->wDay,
				(int)periodTimes[1]->wMonth, (int)periodTimes[1]->wDay));
		graphic->SetTextColor(0xff000000, 0xffffffff);
		row = 74.0f;
		static WRect originalRect = m_pDays->GetRect();
		WRect rect = originalRect;
		rect.y += 18.0f;
		m_pDays->FrWnd::SetRect(rect);
	}
	manager = g_pFresh->GetManager();
	bitmap =
		manager->GetBitmap("LEVELS", MakeStr("level_%03d", item->Level + 1));
	if (bitmap)
	{
		WRect rect;
		rect.x = graphic->GetTextExtend("\xb7\xb9    \xba\xa7:") + draw->pos.x +
			102.0f;
		rect.y = row + draw->pos.y - 3.0f;
		rect.w = (float)bitmap->Width();
		rect.h = (float)bitmap->Height();
		graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xb7\xb9    \xba\xa7:");
		graphic->Print(WPoint(rect.w + rect.x + 5.0f, row + draw->pos.y), 0,
			"\xc0\xcc\xbb\xf3");
	}
	row += 20.0f;
	if (item->TypeId >> 26 == 7)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		const char* period = (item->IsCash) ? "\xb9\xab\xc1\xa6\xc7\xd1"
											: "\xbf\xf9\xb1\xde\xc1\xa6";
		graphic->Print(
			WPoint(graphic->GetTextExtend("\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") +
					95.0f,
				row) +
				draw->pos,
			0, period);
	}
	else if (item->TypeId >> 26 == 8)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (m_rebuildDays)
		{
			m_pDays->SetVisible(true);
			IFF_STRUCT::sCadItem* info =
				ItemManager()->FindCadItem(item->TypeId);
			for (int i = 0; i < 4; ++i)
				if (info->COM[i])
					m_pDays->AddString(days[i].text);
			m_pDays->SetLine(1, days[m_periodIndex].text, 0, false, 0);
			m_rebuildDays = false;
		}
	}
	else if (item->TypeId >> 26 == 14 && !S5::ISCutinItem(item->TypeId))
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (m_rebuildDays)
		{
			m_pDays->SetVisible(true);
			IFF_STRUCT::sSkin* info = ItemManager()->FindSkin(item->TypeId);
			for (int i = 0; i < 5; ++i)
				if (info->COM[i])
					m_pDays->AddString(days[i].text);
			m_pDays->SetLine(1, days[m_periodIndex].text, 0, false, 0);
			m_rebuildDays = false;
		}
	}
	else if (item->TypeId >> 26 == 16)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (m_rebuildDays)
		{
			m_pDays->SetVisible(true);
			IFF_STRUCT::sMascot* info = ItemManager()->FindMascot(item->TypeId);
			for (int i = 0; i < 5; ++i)
				if (info->COM[i])
					m_pDays->AddString(days[i].text);
			m_pDays->SetLine(1, days[m_periodIndex].text, 0, false, 0);
			m_rebuildDays = false;
		}
	}
	else if (m_childCount > 0)
	{
		IFF_STRUCT::sChildItem* child =
			ItemManager()->FindChildItem(item->TypeId, m_periodIndex);
		if (child && child->NumberFlag)
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
				"\xb0\xb3    \xbc\xf6:");
			graphic->SetTextColor(0xffff0000, 0xffffffff);
		}
		else
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
				"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			graphic->SetTextColor(0xffff0000, 0xffffffff);
		}
		if (m_rebuildDays)
		{
			m_pDays->SetVisible(true);
			for (int i = 0; i < m_childCount; ++i)
			{
				child = ItemManager()->FindChildItem(item->TypeId, i);
				if (child && (child->IsDisplay))
					m_pDays->AddString(m_childOptions[i].label);
			}
			m_pDays->SetLine(1, m_childOptions[m_periodIndex].label, 0, false,
				0);
			m_rebuildDays = false;
		}
	}
	if (m_pExplane && m_showCashInfo)
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->SetTextStyle(1);
		CSharedDoc* doc = Doc();
		const char* currency = 0;
		switch (doc->m_provType)
		{
		case 2:
			currency = "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac";
			break;
		case 4:
			currency = "\xc7\xd1\xc4\xda\xc0\xce";
			break;
		}
		if (currency)
		{
			const WRect* rect = &m_pExplane->GetRect();
			if (m_remainingCash < 0)
			{
				graphic->Print(WPoint(rect->x - 70.0f, rect->y - 20.0f), 0,
					MakeStr(
						"%s \xc0\xdc\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
						currency));
			}
			else
			{
				__int64 cash = doc->m_cash;
				__int64 exchanged = doc->m_bonusCash;
				exchanged =
					S5::ExchangeOwnCookieToCash(exchanged - m_totalCookie, 100);
				if (exchanged < 0)
					cash = Doc()->m_cash + exchanged;
				graphic->Print(WPoint(rect->x - 140.0f, rect->y - 20.0f), 0,
					MakeStr(
						"\xb1\xb8\xb8\xc5 \xc8\xc4 \xc3\xd6\xc1\xbe %s \xc0\xdc\xbe\xd7\xc0\xba %I64d\xbf\xf8 \xc0\xd4\xb4\xcf\xb4\xd9.",
						currency, cash > 0 ? cash : 0));
			}
		}
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
	}
}

void FrGiftDlg::OnSxIdInit(int param)
{
	m_pSxId = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pSxId)
		m_pSxId->SetVisible(false);
}

void FrGiftDlg::OnEdIdInit(int param)
{
	m_pEdId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pEdId)
		m_pEdId->SetKeyFocus(true);
	if (m_pEdId)
		m_pEdId->SetVisible(false);
}

bool FrGiftDlg::OnEdIdEnterKey(int param)
{
	return false;
}

void FrGiftDlg::OnEdIdDown()
{
}

void FrGiftDlg::OnEdNickDown()
{
	m_receiverMode = -1;
}

void FrGiftDlg::OnBtIdCheckInit(int param)
{
	m_pBtIdCheck = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pBtIdCheck)
		m_pBtIdCheck->SetVisible(false);
}

void FrGiftDlg::OnBtIdCheckUp()
{
}

void FrGiftDlg::OnSxNickInit(int param)
{
	m_pSxNick = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrGiftDlg::OnEdNickInit(int param)
{
	m_pEdNick = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrGiftDlg::OnEdNickEnterKey(int param)
{
	OnBtNickCheckUp();
	return false;
}

void FrGiftDlg::OnMsgInit(int param)
{
	m_pMessage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrGiftDlg::OnMsgEnterKey(int param)
{
	OnCheckoutBtnUp();
	return false;
}

void FrGiftDlg::OnEdConfirmInit(int param)
{
	m_pEdConfirm = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pEdConfirm)
		m_pEdConfirm->SetLine(1, "<\xc8\xae\xc0\xce \xbe\xc8\xb5\xca>", 0,
			false, 0);
}

void FrGiftDlg::OnBtNickCheckInit(int param)
{
	m_pBtNickCheck = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrGiftDlg::OnBtNickCheckUp()
{
	if (m_pEdConfirm)
		m_pEdConfirm->ClearLine();
	if (m_pEdNick)
	{
		const char* nick = m_pEdNick->GetLine(1, false);
		int length = strlen(nick);
		if (length <= 0)
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xb9\xde\xb4\xc2\xc0\xcc\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
				0, 0, 0, 0);
		}
		else if (length >= 22)
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)MakeStr(
					"\xb4\xeb\xc8\xad\xb8\xed\xc0\xba %d\xc0\xda \xb1\xee\xc1\xf6 \xc0\xd4\xb7\xc2\xc7\xd2\xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					22),
				0, 0, 0, 0);
		}
		else if (strcmp(MyNick(), nick))
		{
			WSendPacket packet((enumClientPacket)7);
			packet.Encode1(0);
			packet.EncodeStr(std::string(nick));
			packet.Send(TO_GAME);
			m_receiverMode = 0;
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		}
		else
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4\xb4\xc2 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
	}
	if (m_pMessage)
		m_pMessage->SetKeyFocus(true);
}

void FrGiftDlg::OnPriceInit(int param)
{
	m_pPrice = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnMyMoneyInit(int param)
{
	m_pMyMoney = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnMoneyLeftInit(int param)
{
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnPrice_CookieInit(int param)
{
	m_pPriceCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnMyMoney_CookieInit(int param)
{
	m_pMyMoneyCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnMoneyLeft_CookieInit(int param)
{
	m_pMoneyLeftCookie = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnNumStaticInit(int param)
{
	m_pNumStatic = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrGiftDlg::OnNumInit(int param)
{
	m_pNum = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrGiftDlg::OnNumPrevInit(int param)
{
	m_pNumPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumPrev)
		m_pNumPrev->SetPushDelay(0.f);
}

void FrGiftDlg::OnNumPrevBtnUp()
{
	if (!m_pNum)
		return;
	int count = atoi(m_pNum->GetLine(1, false));
	if (count > 1)
		SetPrice(count - 1);
}

void FrGiftDlg::OnNumNextInit(int param)
{
	m_pNumNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumNext)
		m_pNumNext->SetPushDelay(0.f);
}

void FrGiftDlg::OnNumNextBtnUp()
{
	if (!m_pNum)
		return;
	int count = atoi(m_pNum->GetLine(1, false));
	if (count < 10)
		SetPrice(count + 1);
}

void FrGiftDlg::RecalculateTotalCookie(__int64 pang, __int64 cash, bool refresh)
{
	m_totalPang = 0;
	m_totalCookie = 0;
	if (m_pCart)
	{
		m_pCart->ClearItem();
		unsigned long type = m_item.TypeId;
		unsigned long group = type >> 26;
		if (group == 14)
		{
			if ((type & 0x3c00000) == 0x1400000)
			{
				if (m_item.IsCash)
				{
					if (m_item.SalePrice > 0 && m_item.SalePrice < m_item.Price)
						m_totalCookie += m_item.SalePrice;
					else
						m_totalCookie += m_item.Price;
				}
				else
				{
					unsigned long price = m_item.SalePrice;
					if (!(price > 0 && price < m_item.Price))
						price = m_item.Price;
					m_totalPang += price * 5 / 100 + price;
				}
			}
			else
			{
				IFF_STRUCT::sSkin* item = ItemManager()->FindSkin(type);
				if (item->c.IsCash)
					m_totalCookie += item->COM[m_periodIndex];
				else
					m_totalPang += item->COM[m_periodIndex] * 5 / 100 +
						item->COM[m_periodIndex];
			}
		}
		else if (group == 8)
		{
			IFF_STRUCT::sCadItem* item = ItemManager()->FindCadItem(type);
			if (item->c.IsCash)
				m_totalCookie += item->COM[m_periodIndex];
			else
				m_totalPang += item->COM[m_periodIndex] * 5 / 100 +
					item->COM[m_periodIndex];
		}
		else if (group == 16)
		{
			IFF_STRUCT::sMascot* item = ItemManager()->FindMascot(type);
			if (item->c.IsCash)
				m_totalCookie += item->COM[m_periodIndex];
			else
				m_totalPang += item->COM[m_periodIndex] * 5 / 100 +
					item->COM[m_periodIndex];
		}
		else if (m_childCount > 0)
		{
			__int64 price = 0xffffffffui64;
			if (m_childOptions[m_periodIndex].durationMode)
				price = m_childOptions[m_periodIndex].durationPrice;
			else if (m_childOptions[m_periodIndex].quantityMode)
				price = m_childOptions[m_periodIndex].quantityPrice;
			if (m_childOptions[m_periodIndex].currency)
				m_totalCookie += price;
			else
				m_totalPang += price * 5 / 100 + price;
		}
		else
		{
			if (m_item.IsCash)
			{
				if (m_item.SalePrice > 0 && m_item.SalePrice < m_item.Price)
					m_totalCookie += m_item.SalePrice;
				else
					m_totalCookie += m_item.Price;
			}
			else
			{
				unsigned long price = m_item.SalePrice;
				if (!(price > 0 && price < m_item.Price))
					price = m_item.Price;
				m_totalPang += price * 5 / 100 + price;
			}
		}
		m_pCart->AddItem(&m_item);
	}
	if (refresh)
	{
		char buffer[32];
		bool has_pang = false, has_cash = false;
		if (m_pPrice)
		{
			MakePriceStrA(buffer, m_totalPang);
			strcat(buffer, "\xc6\xce");
			m_pPrice->SetLine(1, buffer, 0, false, 0);
			if (m_totalPang > 0)
				has_pang = true;
		}
		if (m_pPriceCookie)
		{
			sprintf(buffer, "%I64d\xc4\xed\xc5\xb0", m_totalCookie);
			m_pPriceCookie->SetLine(1, buffer, 0, false, 0);
			if (m_totalCookie > 0)
			{
				has_cash = true;
				m_remainingCash =
					Doc()->RemainOwnCash(Doc()->m_provType, m_totalCookie);
			}
		}
		if (has_pang == true && has_cash == false)
		{
			m_showCashInfo = false;
			m_remainingCash = 0;
		}
		else
			m_showCashInfo = true;
		if (m_pMyMoney)
		{
			MakePriceStrA(buffer, pang);
			strcat(buffer, "\xc6\xce");
			m_pMyMoney->SetLine(1, buffer, 0, false, 0);
		}
		if (m_pMyMoneyCookie)
		{
			sprintf(buffer, "%I64d\xc4\xed\xc5\xb0", cash);
			m_pMyMoneyCookie->SetLine(1, buffer, 0, false, 0);
		}
	}
	SetPrice(1);
}

void FrGiftDlg::OnDaysInit(int param)
{
	m_pDays = DYNAMIC_CAST(FrComboBox, (FrWnd*)param);
}

void FrGiftDlg::OnDaysBtnDown()
{
	if (m_childCount > 0)
	{
		const char* line = m_pDays->GetLine(1, false);
		for (int i = 0; i < m_childCount; ++i)
			if (!stricmp(m_childOptions[i].label, line))
				m_periodIndex = i;
	}
	else
	{
		const char* line = m_pDays->GetLine(1, false);
		for (int i = 0; i < 5; ++i)
			if (!stricmp(days[i].text, line))
				m_periodIndex = i;
	}
	RecalculateTotalCookie(m_initialPang, m_initialCookie, true);
}

void FrGiftDlg::OnWarnInit(int param)
{
	m_pWarn = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pWarn)
		m_pWarn->SetCaption(
			"\xc6\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc4\xa3\xb1\xb8\xbf\xa1\xb0\xd4\xb8\xb8 \xbc\xb1\xb9\xb0\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
}

void FrGiftDlg::OnEmoticonBtnInit(int param)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrGiftDlg::OnEmoticonBtnUp()
{
	if (m_pEmoticonBtn && !m_pEmoticonForm)
	{
		m_pEmoticonForm = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(),
			this, "emoticon", NULL);
		if (m_pEmoticonForm)
		{
			WPoint pos(0, 0);
			const WRect& rect = m_pEmoticonBtn->GetRect();
			pos.x = rect.x - 200.f;
			pos.y = rect.y + 24.f;
			m_pEmoticonForm->Open(
				(FRESH_PFN_RESULT)&FrGiftDlg::OnEmoticonResult, pos, 3);
		}
	}
}

void FrGiftDlg::OnExplaneStaticInit(int param)
{
	m_pExplane = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

bool FrGiftDlg::OnEmoticonResult(int result, FrForm* form)
{
	if (m_pEmoticonForm && result == 1)
	{
		FrEmoticonDlg* dlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (dlg)
		{
			const char* emoticon = dlg->GetSelectedIcon();
			if (emoticon)
			{
				FrEdit* edit = DYNAMIC_CAST(FrEdit, FindChildByName("id"));
				if (m_pEdNick)
				{
					const char* front = m_pEdNick->GetEditText_Front();
					const char* comp = m_pEdNick->GetEditText_Comp();
					const char* end = m_pEdNick->GetEditText_End();
					m_pEdNick->SetLine(1,
						MakeStr("%s%s%s%s", front, comp, emoticon, end), 0,
						false, 0);
					m_pEdNick->SetKeyFocus(true);
				}
			}
		}
	}
	m_pEmoticonForm = NULL;
	return true;
}
