#include "minatl.h"
#include "s5/utilities.h"
#include "frdesktop.h"
#include "titles_client.h"
#include "checkoutdlg.h"
#include "frcombobox.h"
#include "caddieinfodlg.h"
#include "browser.h"
#include "logininfo.h"
#include "binstr.h"
#include "wlocalize.h"
#include "mathconsts.h"
#include "../../shared/s5/sharedutilities.h"
#include <wininet.h>

extern Fresh* g_pFresh;
extern WView* g_view;

IMPLEMENT_OBJECT(FrCheckoutDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrCheckoutDlg, FrForm)
ON_FRESH_VI("checkout", FRCMD_INIT, FrCheckoutDlg::OnCheckoutInit)
ON_FRESH_VV("checkout", FRCMD_LBUTTONUP, FrCheckoutDlg::OnCheckoutLBtnUp)
ON_FRESH_VI("cart", FRCMD_INIT, FrCheckoutDlg::OnCartInit)
ON_FRESH_VI("cart", FRCMD_OWNERDRAW, FrCheckoutDlg::OnCartOwnerDraw)
ON_FRESH_VI("price", FRCMD_INIT, FrCheckoutDlg::OnPriceInit)
ON_FRESH_VI("my_money", FRCMD_INIT, FrCheckoutDlg::OnMyMoneyInit)
ON_FRESH_VI("money_left", FRCMD_INIT, FrCheckoutDlg::OnMoneyLeftInit)
ON_FRESH_VI("price_cookie", FRCMD_INIT, FrCheckoutDlg::OnPrice_CookieInit)
ON_FRESH_VI("my_money_cookie", FRCMD_INIT, FrCheckoutDlg::OnMyMoney_CookieInit)
ON_FRESH_VI("money_left_cookie", FRCMD_INIT,
	FrCheckoutDlg::OnMoneyLeft_CookieInit)
ON_FRESH_VI("num_static", FRCMD_INIT, FrCheckoutDlg::OnNumStaticInit)
ON_FRESH_VI("explane", FRCMD_INIT, FrCheckoutDlg::OnExplaneStaticInit)
ON_FRESH_VI("num", FRCMD_INIT, FrCheckoutDlg::OnNumInit)
ON_FRESH_BI("num", FRCMD_ENTERKEY, FrCheckoutDlg::OnNumEnterKey)
ON_FRESH_VI("num_prev", FRCMD_INIT, FrCheckoutDlg::OnNumPrevInit)
ON_FRESH_VV("num_prev", FRCMD_LBUTTONUP, FrCheckoutDlg::OnNumPrevLBtnUp)
ON_FRESH_VI("num_next", FRCMD_INIT, FrCheckoutDlg::OnNumNextInit)
ON_FRESH_VV("num_next", FRCMD_LBUTTONUP, FrCheckoutDlg::OnNumNextLBtnUp)
ON_FRESH_VI("days", FRCMD_INIT, FrCheckoutDlg::OnDaysInit)
ON_FRESH_VI("days2", FRCMD_INIT, FrCheckoutDlg::OnDays2Init)
ON_FRESH_VV("days", FRCMD_LBUTTONDOWN, FrCheckoutDlg::OnDaysBtnDown)
ON_FRESH_VV("days2", FRCMD_LBUTTONDOWN, FrCheckoutDlg::OnDays2BtnDown)
END_FRESH_MSGMAP()

days_t days[] = {
	{ K2L_Compatibility("1\xc0\xcf"),                1   },
	{ K2L_Compatibility("7\xc0\xcf"),                7   },
	{ K2L_Compatibility("15\xc0\xcf"),               15  },
	{ K2L_Compatibility("30\xc0\xcf"),               30  },
	{ K2L_Compatibility("365\xc0\xcf"),              365 },
	{ K2L_Compatibility("\xb9\xab\xc1\xa6\xc7\xd1"), 0   }
};

FrCheckoutDlg::FrCheckoutDlg()
{
	m_pCheckout = NULL;
	m_pCart = NULL;
	m_pPrice = NULL;
	m_pMyMoney = NULL;
	m_pMoneyLeft = NULL;
	m_pPrice_Cookie = NULL;
	m_pMyMoney_Cookie = NULL;
	m_pMoneyLeft_Cookie = NULL;
	m_pNumStatic = NULL;
	m_pExplaneStatic = NULL;
	m_pNum = NULL;
	m_pNumPrev = NULL;
	m_pNumNext = NULL;
	m_bOwned = false;
	m_bUnderLevel = false;
	m_bOverLevel = false;
	m_bEnoughPang = false;
	m_bEnoughCookie = false;
	m_bRental = false;
	m_confirmType = 0;
	m_num = 1;
	m_myPang = 0;
	m_myCookie = 0;
	m_remainCash = 0;
	m_bShowCash = false;
	m_coupons.clear();
	m_bCouponEnable = true;
	m_selectedCoupon = -1;
	m_reserved1c0 = 0;
	m_discountPang = 0;
	m_discountCookie = 0;
	m_reserved1d8 = 0;
	m_childOptionNum = 0;
	memset(m_childOption, 0, sizeof(m_childOption));
	m_daysCombo[0].pItem = NULL;
	m_daysCombo[1].pItem = NULL;
	m_bSubmitted = false;
}

std::list<FrCheckoutDlg::CartItem>& FrCheckoutDlg::Cart()
{
	return m_cart;
}

void FrCheckoutDlg::Process()
{
	if (!m_pNum)
		return;
	int num = atoi(m_pNum->GetLine(1, false));
	if (num != m_num)
		SetPrice(num);
}

void FrCheckoutDlg::RecalculateTotalCookie(__int64 pang, __int64 cookie,
	bool update)
{
	m_totalPang = 0;
	m_totalCookie = 0;
	for (std::list<CartItem>::iterator it = m_cart.begin(); it != m_cart.end();
		++it)
	{
		__int64 price = 0xffffffffui64;
		if (m_bRental && ((*it).pItem->TypeId & 0xfc000000) == 0x08000000)
		{
			IFF_STRUCT::sPart* part =
				ItemManager()->FindPart((*it).pItem->TypeId);
			if (part && part->RentalPrice > 0)
				m_totalPang += part->RentalPrice;
		}
		else
		{
			unsigned long typeId = (*it).pItem->TypeId;
			unsigned long group = typeId >> 26;
			if (group == 14)
			{
				IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(typeId);
				unsigned long sub = ((*it).pItem->TypeId >> 22) & 15;
				if (sub == 5)
				{
					unsigned long value = (*it).pItem->SalePrice;
					if (!(value > 0 && value < (*it).pItem->Price))
						value = (*it).pItem->Price;
					price = value;
					if ((*it).pItem->IsCash)
						m_totalCookie += price;
					else
						m_totalPang += price;
				}
				else if (sub == 6)
				{
					unsigned long value = (*it).pItem->SalePrice;
					if (!(value > 0 && value < (*it).pItem->Price))
						value = (*it).pItem->Price;
					price = value;
					if ((*it).pItem->IsCash)
						m_totalCookie += price;
					else
						m_totalPang += price;
				}
				else if (skin->c.IsCash)
					m_totalCookie += skin->COM[(*it).period];
				else
					m_totalPang += skin->COM[(*it).period];
			}
			else if (group == 8)
			{
				IFF_STRUCT::sCadItem* item = ItemManager()->FindCadItem(typeId);
				if (item->c.IsCash)
					m_totalCookie += item->COM[(*it).period];
				else
					m_totalPang += item->COM[(*it).period];
			}
			else if (group == 16)
			{
				IFF_STRUCT::sMascot* item = ItemManager()->FindMascot(typeId);
				if (item->c.IsCash)
					m_totalCookie += item->COM[(*it).period];
				else
					m_totalPang += item->COM[(*it).period];
			}
			else if (m_childOptionNum > 0)
			{
				if (m_childOption[(*it).period].periodType)
					price = m_childOption[(*it).period].periodPrice;
				else if (m_childOption[(*it).period].numType)
					price = m_childOption[(*it).period].numPrice;
				if (m_childOption[(*it).period].bCookie)
					m_totalCookie += price;
				else
					m_totalPang += price;
			}
			else
			{
				unsigned long value = (*it).pItem->SalePrice;
				if (!(value > 0 && value < (*it).pItem->Price))
					value = (*it).pItem->Price;
				price = value;
				if ((*it).pItem->IsCash)
					m_totalCookie += price;
				else
					m_totalPang += price;
			}
		}
	}
	if (update)
	{
		char buffer[32];
		bool hasPang = false, hasCookie = false;
		if (m_pPrice)
		{
			MakePriceStrA(buffer, max(0i64, m_totalPang - m_discountPang));
			strcat(buffer, "\xc6\xce");
			m_pPrice->SetLine(1, buffer, 0, false, 0);
			if (m_totalPang)
				hasPang = true;
		}
		if (m_pPrice_Cookie)
		{
			__int64 price = m_totalCookie - m_discountCookie;
			sprintf(buffer, "%I64d\xc4\xed\xc5\xb0", max(0i64, price));
			m_pPrice_Cookie->SetLine(1, buffer, 0, false, 0);
			if (price)
			{
				hasCookie = true;
				m_remainCash = Doc()->RemainOwnCash(Doc()->m_provType, price);
			}
		}
		if (hasPang == true && hasCookie == false)
		{
			m_bShowCash = false;
			m_remainCash = 0;
		}
		else
			m_bShowCash = true;
		if (m_pMyMoney)
		{
			MakePriceStrA(buffer, pang);
			strcat(buffer, "\xc6\xce");
			m_pMyMoney->SetLine(1, buffer, 0, false, 0);
		}
		if (m_pMyMoney_Cookie)
		{
			sprintf(buffer, "%I64d\xc4\xed\xc5\xb0", cookie);
			m_pMyMoney_Cookie->SetLine(1, buffer, 0, false, 0);
		}
	}
	SetPrice(1);
}

void FrCheckoutDlg::Checkout(__int64 pang, __int64 cookie, bool rental)
{
	m_bRental = rental;
	m_bOwned = false;
	m_bUnderLevel = false;
	m_bOverLevel = false;
	m_bEnoughPang = false;
	m_bEnoughCookie = false;
	m_childOptionNum = 0;
	memset(m_childOption, 0, sizeof(m_childOption));
	if (m_pCart)
	{
		m_pCart->ClearItem();
		for (std::list<CartItem>::iterator it = m_cart.begin();
			it != m_cart.end(); ++it)
		{
			CartItem& row = *it;
			FixCartItem(row);
			m_pCart->AddItem(&row);
			if (row.pItem->InStock)
			{
				if (row.pItem->IsUnderLvl &&
					row.pItem->Level < Doc()->m_myInfo.stat.Level)
					m_bOverLevel = true;
				else if (!row.pItem->IsUnderLvl &&
					row.pItem->Level > Doc()->m_myInfo.stat.Level)
					m_bUnderLevel = true;
			}
			if (row.ownedNum)
				m_bOwned = true;
		}
	}
	m_daysCombo[0].bRebuild = true;
	m_daysCombo[0].pCombo->SetVisible(false);
	m_daysCombo[1].bRebuild = true;
	m_daysCombo[1].pCombo->SetVisible(false);
	m_myPang = pang;
	m_myCookie = cookie;
	RecalculateTotalCookie(pang, cookie, true);
	std::list<CartItem>::iterator item = m_cart.begin();
	bool fixedNum = false;
	if (((*item).pItem->TypeId & 0xfc000000) == 0x18000000 &&
		!((*item).pItem->TypeId & 0x2000000))
	{
		IFF_STRUCT::sItem* data =
			ItemManager()->FindItem((*item).pItem->TypeId);
		if (data->COM[0] > 0)
		{
			fixedNum = true;
			if (m_pNum)
				m_pNum->SetLine(1, MakeStr("%d", data->COM[0]), 0, false, 0);
		}
	}
	if ((!((*item).pItem->TypeId & 0x2000000) &&
			((*item).pItem->TypeId & 0xfc000000) == 0x18000000 && !fixedNum &&
			m_cart.size() <= 1) ||
		(*item).pItem->TypeId == 0x1a000041 ||
		(*item).pItem->TypeId == 0x1a0000cc)
	{
		if (m_pNum)
		{
			WPoint pos = m_pNum->GetRect().TopLeft();
			m_pNum->MoveWindow(
				WPoint(pos.x + ((*item).pItem->IsCash ? 50 : -50), pos.y));
		}
		if (m_pNumPrev)
		{
			WPoint pos = m_pNumPrev->GetRect().TopLeft();
			m_pNumPrev->MoveWindow(
				WPoint(pos.x + ((*item).pItem->IsCash ? 50 : -50), pos.y));
		}
		if (m_pNumNext)
		{
			WPoint pos = m_pNumNext->GetRect().TopLeft();
			m_pNumNext->MoveWindow(
				WPoint(pos.x + ((*item).pItem->IsCash ? 50 : -50), pos.y));
		}
	}
	else
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
}

void FrCheckoutDlg::FixCartItem(CartItem& item)
{
	unsigned long typeId = item.pItem->TypeId;
	switch (typeId >> 26)
	{
	case 2:
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
		if (part && m_bRental && part->RentalPrice > 0)
			item.period = 7;
		break;
	}
	case 14:
	{
		IFF_STRUCT::sSkin* part = ItemManager()->FindSkin(typeId);
		if (part && !S5::ISCutinItem(item.pItem->TypeId))
		{
			for (int i = 0; i < 5; ++i)
				if (part->COM[i])
				{
					item.period = i;
					break;
				}
		}
		break;
	}
	case 8:
	{
		IFF_STRUCT::sCadItem* part = ItemManager()->FindCadItem(typeId);
		if (part)
		{
			for (int i = 0; i < 5; ++i)
				if (part->COM[i])
				{
					item.period = i;
					break;
				}
		}
		break;
	}
	case 16:
	{
		IFF_STRUCT::sMascot* part = ItemManager()->FindMascot(typeId);
		if (part)
		{
			for (int i = 0; i < 5; ++i)
				if (part->COM[i])
				{
					item.period = i;
					break;
				}
		}
		break;
	}
	default:
		m_childOptionNum = ItemManager()->FindChildItemNumber(typeId);
		if (m_childOptionNum > 0)
		{
			memset(m_childOption, 0, sizeof(m_childOption));
			if (m_childOptionNum > 5)
				m_childOptionNum = 5;
			for (int i = 0; i < m_childOptionNum; ++i)
			{
				IFF_STRUCT::sChildItem* child =
					ItemManager()->FindChildItem(item.pItem->TypeId, i);
				if (!child || !child->IsDisplay)
					continue;
				if (child->NumberFlag)
				{
					sprintf(m_childOption[i].text, "%d\xb0\xb3", child->Number);
					m_childOption[i].bCookie = child->IsCash;
					m_childOption[i].numPrice = child->NumberPrice;
					m_childOption[i].numType = child->NumberFlag;
					m_childOption[i].num = child->Number;
				}
				else if (child->TimeFlag)
				{
					switch (child->TimeFlag)
					{
					case 0:
						break;
					case 3:
						sprintf(m_childOption[i].text, "%d\xbd\xc3\xb0\xa3",
							child->Time);
						break;
					case 4:
						sprintf(m_childOption[i].text, "%d\xc0\xcf",
							child->Time);
						break;
					case 5:
						sprintf(m_childOption[i].text, "%d\xb0\xb3\xbf\xf9",
							child->Time);
						break;
					default:
						sprintf(m_childOption[i].text, "%d\xc0\xcf",
							child->Time);
						break;
					}
					m_childOption[i].bCookie = child->IsCash;
					m_childOption[i].periodPrice = child->TimePrice;
					m_childOption[i].periodType = child->TimeFlag;
					m_childOption[i].period = child->Time;
				}
			}
		}
		break;
	}
}

void FrCheckoutDlg::SetPrice(int num)
{
	m_bEnoughPang = true;
	m_bEnoughCookie = true;
	if (num < 1)
		num = 1;
	else if (num > 99)
		num = 99;
	m_num = num;
	if (m_pPrice && m_pMyMoney && m_totalPang)
	{
		char buffer[32];
		MakePriceStrA(buffer, max(0i64, m_totalPang * m_num - m_discountPang));
		strcat(buffer, "\xc6\xce");
		m_pPrice->SetLine(1, buffer, 0, false, 0);
		__int64 money = Doc()->m_myInfo.stat.i64Pang;
		__int64 price = m_totalPang * m_num;
		__int64 reduction = m_discountPang;
		__int64 left = money - price + reduction;
		if (left < 0)
		{
			if (m_pMoneyLeft)
			{
				MakePriceStrA(buffer, price - reduction - money);
				strcat(buffer, "\xc6\xce \xba\xce\xc1\xb7");
				m_pMoneyLeft->SetLine(1, buffer, 0xffff0000, false, 0);
			}
			m_bEnoughPang = false;
		}
		else if (m_pMoneyLeft)
		{
			__int64 discount = reduction;
			if (discount > price)
				discount = price;
			MakePriceStrA(buffer, money - price + discount);
			strcat(buffer, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, buffer, 0, false, 0);
		}
	}
	else if (m_pMoneyLeft)
		m_pMoneyLeft->SetLine(1, m_pMyMoney->GetLine(1, false), 0, false, 0);
	if (m_pPrice_Cookie && m_pMyMoney_Cookie)
	{
		char buffer[32];
		sprintf(buffer, "%I64d\xc4\xed\xc5\xb0",
			max(0i64, m_totalCookie - m_discountCookie));
		m_pPrice_Cookie->SetLine(1, buffer, 0, false, 0);
		__int64 money = Doc()->m_cookie;
		__int64 price = m_totalCookie;
		__int64 reduction = m_discountCookie;
		__int64 left = money - price + reduction;
		if (left < 0)
		{
			if (m_pMoneyLeft_Cookie)
			{
				MakePriceStrA(buffer, price - reduction - money);
				strcat(buffer, "\xc4\xed\xc5\xb0 \xba\xce\xc1\xb7");
				m_pMoneyLeft_Cookie->SetLine(1, buffer, 0xffff0000, false, 0);
			}
			m_bEnoughCookie = false;
		}
		else if (m_pMoneyLeft_Cookie)
		{
			__int64 discount = reduction;
			if (discount > price)
				discount = price;
			sprintf(buffer, "%d\xc4\xed\xc5\xb0", money - price + discount);
			m_pMoneyLeft_Cookie->SetLine(1, buffer, 0, false, 0);
		}
	}
	else if (m_pMoneyLeft_Cookie)
		m_pMoneyLeft_Cookie->SetLine(1, m_pMyMoney_Cookie->GetLine(1, false), 0,
			false, 0);
	if (m_pNum)
		m_pNum->SetLine(1, MakeStr("%d", m_num), 0, false, 0);
}

bool FrCheckoutDlg::OnChargeResult(int result, FrForm*)
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
		Close(FrOK, true);
	}
	return true;
}

bool FrCheckoutDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == FrOK && !m_bSubmitted)
	{
		FrListBox::ITEM_LIST& list = m_pCart->m_itemList;
		int num = atoi(m_pNum->GetLine(1, false));
		sBuyItem buy;
		CartItem& item = *(CartItem*)(*list.begin())->pData;
		if (IsLocalContent((localContentType_t)0x17) &&
			Doc()->IsHaveHalloweenItem() &&
			(item.pItem->TypeId & 0xfc000000) == 0x08000000 &&
			(item.pItem->TypeId & 0x1800) == 0x1800)
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xc0\xcc\xb9\xcc \xb1\xb8\xb8\xc5\xc7\xd1 \xb9\xb0\xb0\xc7\xc0\xd4\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
		}
		else
		{
			if (m_childOptionNum > 0)
			{
				if ((item.pItem->TypeId & 0xfc000000) == 0x18000000)
				{
					WSendPacket packet((enumClientPacket)0x1d);
					IFF_STRUCT::sItem* data =
						ItemManager()->FindItem(item.pItem->TypeId);
					if (m_childOption[item.period].numType)
						num = m_childOption[item.period].num;
					buy.TypeCode = item.pItem->TypeId;
					buy.DayCount = m_childOption[item.period].period;
					buy.sChildItemIndex = item.period;
					buy.ItemCount = num;
					packet.Encode1(0);
					packet.Encode2(1);
					packet.EncodeBuffer(&buy, sizeof(buy));
					packet.Send(TO_GAME);
				}
				else
				{
					unsigned int count = list.size();
					WSendPacket packet((enumClientPacket)0x1d);
					packet.Encode1(0);
					packet.Encode2(count);
					for (FrListBox::ITEM_LIST::iterator it = list.begin();
						it != list.end(); ++it)
					{
						item = *(CartItem*)(*it)->pData;
						buy.TypeCode = item.pItem->TypeId;
						buy.DayCount = m_childOption[item.period].period;
						buy.sChildItemIndex = item.period;
						buy.ItemCount = num;
						packet.EncodeBuffer(&buy, sizeof(buy));
					}
					packet.Send(TO_GAME);
				}
			}
			else
			{
				unsigned long typeId = item.pItem->TypeId;
				if ((typeId >> 26) == 5)
				{
					IFF_STRUCT::sBall* ball = ItemManager()->FindBall(typeId);
					if (ball)
						num = ball->COM[0];
				}
				else if ((typeId >> 26) == 6 && (typeId & 0x2000000) &&
					typeId != 0x1a000041 && typeId != 0x1a0000cc)
				{
					IFF_STRUCT::sItem* data = ItemManager()->FindItem(typeId);
					if (data)
						num = data->COM[0];
				}
				if (!Doc()->ConfirmInsertItem(item.pItem->TypeId, num))
				{
					FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify");
					WRect rect = dlg->GetRect();
					dlg->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
					dlg->SetTimeLimit(12.0f);
					dlg->SetMessage(
						"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						false);
					dlg->Open(NULL, 0);
					return true;
				}
				if (m_bRental)
				{
					for (FrListBox::ITEM_LIST::iterator it = list.begin();
						it != list.end(); ++it)
					{
						CartItem& row = *(CartItem*)(*it)->pData;
						if (!S5::RENTALPARTS::IsRentableParts(
								row.pItem->TypeId))
						{
							AfxGetTask()->GetActor("Shop") << MsgObject(NULL,
								0x23,
								(int)"\xb1\xb8\xb8\xc5 \xc3\xb3\xb8\xae \xbf\xc0\xb7\xf9. \xc3\xb3\xc0\xbd\xba\xce\xc5\xcd \xb4\xd9\xbd\xc3 \xbd\xc3\xc0\xdb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
								0, 0, 0, 0);
							return true;
						}
					}
				}
				if (list.size() > 1)
				{
					std::list<unsigned long> ids;
					for (FrListBox::ITEM_LIST::iterator it = list.begin();
						it != list.end(); ++it)
					{
						CartItem& row = *(CartItem*)(*it)->pData;
						ids.push_back(row.pItem->TypeId);
					}
					ids.sort();
					ids.unique();
					if (ids.size() != list.size())
					{
						FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(),
							this, "notify");
						WRect rect = dlg->GetRect();
						dlg->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
						dlg->SetTimeLimit(12.0f);
						dlg->SetMessage(
							"\xc4\xab\xc6\xae \xb1\xb8\xb8\xc5 \xb0\xfa\xc1\xa4\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xec \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xdc\xc7\xb0\xc0\xb8\xb7\xce \xb1\xb8\xb8\xc5\xb8\xa6 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
							false);
						dlg->Open(NULL, 0);
						return true;
					}
				}
				unsigned int count = list.size();
				WSendPacket packet((enumClientPacket)0x1d);
				packet.Encode1(m_bRental);
				packet.Encode2(count);
				for (FrListBox::ITEM_LIST::iterator it = list.begin();
					it != list.end(); ++it)
				{
					CartItem& item = *(CartItem*)(*it)->pData;
					buy.TypeCode = item.pItem->TypeId;
					buy.DayCount = days[item.period].days;
					buy.sChildItemIndex = -1;
					buy.ItemCount = num;
					packet.EncodeBuffer(&buy, sizeof(buy));
				}
				packet.Send(TO_GAME);
			}
			m_bSubmitted = true;
			AfxGetTask()->GetActor("Shop")
				<< MsgObject(NULL, 0xc0, 0, 0, 0, 0, 0);
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			Close(FrOK, true);
		}
	}
	return true;
}

bool __fastcall CompareUsableCoupon(const FrCheckoutDlg::sCoupon& a,
	const FrCheckoutDlg::sCoupon& b)
{
	return a.bUsable > b.bUsable;
}

bool __fastcall ComparePeriodCoupon(const FrCheckoutDlg::sCoupon& a,
	const FrCheckoutDlg::sCoupon& b)
{
	return a.period < b.period;
}

unsigned long FrCheckoutDlg::GetGuidSelectedCoupon()
{
	m_bCouponEnable = false;
	return 0;

	// This code is dead in this version.
	std::map<unsigned int, sCoupon>::iterator it;
	it = m_coupons.find(m_selectedCoupon);
	if (it != m_coupons.end())
		return (*it).first;
	return 0;
}

void FrCheckoutDlg::OnCheckoutInit(int param)
{
	m_pCheckout = DYNAMIC_CAST(FrButton, param);
	m_miniBtn[6].pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_miniBtn[6].rect =
		WRect(40.0f, 35.0f, (float)m_miniBtn[6].pBitmap->Width(),
			(float)m_miniBtn[6].pBitmap->Height());
}

void FrCheckoutDlg::OnCheckoutLBtnUp()
{
	if (IsUnderLevel() || IsOverLevel())
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xc7\xf6\xc0\xe7 \xb7\xb9\xba\xa7\xb7\xce\xb4\xc2 \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
			0, 0, 0, 0);
		return;
	}
	int ownedParts = Doc()->m_partsList.size() + Doc()->m_clubSetMap.size();
	FrListBox::ITEM_LIST& list = m_pCart->m_itemList;
	unsigned long charType = 0, hairType = 0;
	for (FrListBox::ITEM_LIST::iterator it = list.begin(); it != list.end();
		++it)
	{
		CartItem& item = *(CartItem*)(*it)->pData;
		if (ownedParts > 1000)
		{
			switch (item.pItem->TypeId >> 26)
			{
			case 2:
			case 4:
				AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
					(int)"\xb4\xf5 \xc0\xcc\xbb\xf3 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return;
			}
		}
		if ((item.pItem->TypeId & 0xfc000000) == 0x20000000)
		{
			IFF_STRUCT::sCadItem* data =
				ItemManager()->FindCadItem(item.pItem->TypeId);
			std::map<unsigned int, sCaddieInfo>& caddies = Doc()->m_caddieMap;
			std::map<unsigned int, sCaddieInfo>::const_iterator cad =
				caddies.begin();
			const sCaddieInfo* selected;
			bool found = false;
			for (; cad != caddies.end(); ++cad)
			{
				selected = &(*cad).second;
				if (selected->tid ==
					(((data->c.TypeId >> 21) & 31) | 0x1c000000))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
					(int)"\xb2\xd9\xb9\xcc\xb1\xe2\xc7\xd2 \xc4\xb3\xb5\xf0\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return;
			}

			IFF_STRUCT::sCaddie* caddie =
				ItemManager()->FindCaddie(selected->tid);
			if (selected->Rent_flag && selected->Remain_Date == 0 && caddie &&
				caddie->MonthlyFee > 0)
			{
				OpenCaddieInfo(*selected);
				return;
			}
		}
		if ((item.pItem->TypeId & 0xfc000000) == 0x38000000 &&
			(item.pItem->TypeId & 0x3c00000) == 0x1800000)
		{
			Title* title = g_TitleManager->GetTitle(
				(eTitle)(item.pItem->TypeId & 0x3fffff));
			if (title && !title->CanEquip())
			{
				AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
					(int)"\xb1\xb8\xc0\xd4\xb0\xa1\xb4\xc9\xc7\xd1 \xc1\xb6\xb0\xc7\xc0\xbb \xb0\xae\xc3\xdf\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
				return;
			}
		}
		if (item.ownedNum)
		{
			unsigned long type = item.pItem->TypeId;
			unsigned long group = type >> 26;
			if (group == 6 || group == 5 ||
				(group == 9 &&
					(((type >> 21) & 31) == 6 || ((type >> 21) & 31) == 5)))
			{
				if (item.ownedNum < 100000)
				{
					if (m_childOptionNum > 0)
					{
						if (m_childOption[item.period].periodType)
						{
							AfxGetTask()->GetActor("Shop") << MsgObject(NULL,
								0x23,
								(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb9\xb0\xb0\xc7\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
								0, 0, 0, 0);
							return;
						}
					}
					else if (ItemManager()->IsDisableDuplicateItem(type))
					{
						AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
							(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb9\xb0\xb0\xc7\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
							0, 0, 0, 0);
						return;
					}
				}
				else
				{
					AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
						(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb9\xb0\xb0\xc7\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
					return;
				}
			}
			else
			{
				switch (item.pItem->TypeId >> 26)
				{
				case 2:
				{
					if (item.pItem->IsCash)
					{
						if (m_bRental)
						{
							AfxGetTask()->GetActor("Shop") << MsgObject(NULL,
								0x23,
								(int)"\xc0\xcc\xb9\xcc \xb4\xeb\xbf\xa9\xc1\xdf\xc0\xce \xc0\xc7\xbb\xf3\xc0\xba \xc3\xdf\xb0\xa1\xb7\xce \xb4\xeb\xbf\xa9\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
								0, 0, 0, 0);
							return;
						}
						AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
							(int)"\xc4\xed\xc5\xb0 \xc0\xc7\xbb\xf3\xc0\xba \xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb0\xc5\xb3\xaa \xb4\xeb\xbf\xa9\xc1\xdf\xbf\xa1\xb4\xc2 \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
							0, 0, 0, 0);
						return;
					}
					m_confirmType = 4;
					break;
				}
				case 15:
					break;
				default:
				{
					AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
						(int)"\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xb9\xb0\xb0\xc7\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
					return;
				}
				}
			}
		}
		unsigned long type = item.pItem->TypeId;
		unsigned long group = type >> 26;
		if (group == 15)
		{
			IFF_STRUCT::sHairStyle* hair = ItemManager()->FindHairStyle(type);
			m_confirmType = 1;
			std::map<unsigned int, sCharacterInfo>& chars = Doc()->m_charMap;
			for (std::map<unsigned int, sCharacterInfo>::const_iterator c =
					 chars.begin();
				c != chars.end(); ++c)
				if (((*c).second.tid & 0x3ffffff) == hair->cCharID &&
					(*c).second.hairClr == hair->cHairID)
				{
					AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
						(int)"\xc7\xd8\xb4\xe7\xc4\xb3\xb8\xaf\xc5\xcd\xb4\xc2 \xbc\xb1\xc5\xc3\xc7\xd1 \xbf\xb0\xbb\xf6\xbe\xe0\xb0\xfa \xb0\xb0\xc0\xba \xbb\xf6\xc0\xc7 \xbf\xb0\xbb\xf6\xc0\xbb \xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9",
						0, 0, 0, 0);
					return;
				}
		}
		else if (group == 8)
		{
			IFF_STRUCT::sCadItem* data = ItemManager()->FindCadItem(type);
			m_confirmType = 2;

			for (std::map<unsigned int, sCaddieInfo>::iterator c =
					 Doc()->m_caddieMap.begin();
				c != Doc()->m_caddieMap.end(); ++c)
			{
				if ((*c).second.tid ==
					(((item.pItem->TypeId >> 21) & 31) | 0x1c000000))
					m_confirmType = 3;
				if ((*c).second.Remain_Partdate >= 17520)
				{
					AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
						(int)"\xb4\xf5\xc0\xcc\xbb\xf3 \xb1\xb8\xc0\xd4\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
					return;
				}
			}
		}
		else if (group == 16)
		{
			std::map<unsigned int, sMascotInfo>& mascots = Doc()->m_mascotMap;
			for (std::map<unsigned int, sMascotInfo>::const_iterator c =
					 mascots.begin();
				c != mascots.end(); ++c)
				if ((*c).second.tid == type)
				{
					m_confirmType = 3;
					if ((*c).second.Remain_Date >= 17520)
					{
						AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
							(int)"\xb4\xf5\xc0\xcc\xbb\xf3 \xb1\xb8\xc0\xd4\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
							0, 0, 0, 0);
						return;
					}
					break;
				}
		}
		if (!ItemManager()->IsInSalePeriod(item.pItem->TypeId,
				Doc()->GetServerTime()))
		{
			AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
				(int)"\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				0, 0, 0, 0);
			return;
		}
		if ((item.pItem->TypeId >> 26) == 1)
			charType = item.pItem->TypeId;
		else if ((item.pItem->TypeId >> 26) == 15)
			hairType = item.pItem->TypeId;
	}
	if (charType && hairType)
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xc4\xb3\xb8\xaf\xc5\xcd\xbf\xcd \xbf\xb0\xbb\xf6\xbe\xe0\xc0\xba \xb5\xbf\xbd\xc3\xbf\xa1 \xb1\xb8\xb8\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}
	if (!m_bEnoughPang)
	{
		AfxGetTask()->GetActor("Shop") << MsgObject(NULL, 0x23,
			(int)"\xc6\xce\xc0\xcc \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\x0a\xb0\xd4\xc0\xd3\xbf\xa1 \xc2\xfc\xbf\xa9\xc7\xd8\xbc\xad \xc6\xce\xc0\xbb \xb8\xf0\xc0\xb8\xbc\xbc\xbf\xe4.",
			0, 0, 0, 0);
		return;
	}
	if (!m_bEnoughCookie)
	{
		FrForm* dlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel");
		dlg->SetMessage(
			"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\x0a\xc1\xf6\xb1\xdd \xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		dlg->Open((FRESH_PFN_RESULT)&FrCheckoutDlg::OnChargeResult,
			FrESCAPE | FrENTER);
		return;
	}
	std::string text;
	if (m_bCouponEnable && m_selectedCoupon >= 0)
	{
		char buffer[128];
		CartItem& item = *(CartItem*)(*m_pCart->m_itemList.begin())->pData;
		char price[16] = "";
		if (m_totalPang > 0)
		{
			__int64 value = 0;
			__int64 total = m_totalPang * m_num;
			if (m_discountPang < total)
				value = total - m_discountPang;
			sprintf(price, "%I64d\xc6\xce", value);
		}
		else if (m_totalCookie > 0)
		{
			__int64 value = 0;
			if (m_discountCookie < m_totalCookie)
				value = m_totalCookie - m_discountCookie;
			sprintf(price, "%I64d\xc4\xed\xc5\xb0", value);
		}
		else
			sprintf(price, "0\xc4\xed\xc5\xb0");
		sprintf(buffer,
			"\\c0xffff0000\\c%s\xc0\xbb(\xb8\xa6) \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb4\xc2\xb5\xa5 %s\xc0\xcc(\xb0\xa1) \xbc\xd2\xbf\xe4\xb5\xcb\xb4\xcf\xb4\xd9.\x0a\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			item.pItem->Name, price);
		text = buffer;
	}
	else
	{
		switch (m_confirmType)
		{
		case 1:
			text =
				"\\c0xffff0000\\c\xbf\xb0\xbb\xf6\xbe\xe0\xc0\xba \xb1\xb8\xb8\xc5\xbf\xcd \xb5\xbf\xbd\xc3\xbf\xa1 \xbb\xe7\xbf\xeb\xc0\xcc \xb5\xcb\xb4\xcf\xb4\xd9.\x0a\xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \xb8\xd3\xb8\xae\xbb\xf6\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xd1 \xbb\xf6\xc0\xb8\xb7\xce \xbf\xb0\xbb\xf6\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
			break;
		case 2:
			text =
				"\\c0xffff0000\\c\xb1\xe2\xc1\xb8\xc0\xc7 \xc4\xb3\xb5\xf0\xb2\xd9\xb9\xcc\xb1\xe2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc0\xd6\xb4\xd9\xb8\xe9 \xb5\xa4\xbe\xee\xbe\xb2\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9.\x0a \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
			break;
		case 3:
			text =
				"\\c0xffff0000\\c\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xc0\xcf\xbc\xf6\xb8\xb8\xc5\xad \xb1\xe2\xb0\xa3\xc0\xcc \xbf\xac\xc0\xe5\xb5\xcb\xb4\xcf\xb4\xd9.\x0a \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
			break;
		case 4:
			text =
				"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xc2 \xc0\xc7\xbb\xf3\xc0\xd4\xb4\xcf\xb4\xd9.\x0a \xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
			break;
		case 0:
			// HACK: Dead code.
			max(0i64, m_totalPang * m_num - m_discountPang);
		default:
			text =
				"\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
			break;
		}
	}
	text +=
		"\x0a\x0a\xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb1\xe2 \xc0\xfc\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb1\xe2\xc7\xd1\xb0\xfa \xbc\xf6\xb7\xae\xc0\xbb \xb2\xc0 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.\x0a\xc0\xcc \xbb\xf3\xc7\xb0\xc0\xba \xc3\xbb\xbe\xe0\xc3\xb6\xc8\xb8(\xb1\xb8\xb8\xc5\xc3\xeb\xbc\xd2)\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9.";
	FrForm* dlg =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel_big");
	dlg->SetMessage(text.c_str(), false);
	dlg->Open((FRESH_PFN_RESULT)&FrCheckoutDlg::OnConfirmResult,
		FrESCAPE | FrENTER);
}

void FrCheckoutDlg::OpenCaddieInfo(const sCaddieInfo& info)
{
	FrWnd* wnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (!wnd)
	{
		FrCaddieInfoDlg* dlg = DYNAMIC_CAST(FrCaddieInfoDlg, wnd);
		dlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this,
			"caddie_info");
		if (dlg)
		{
			dlg->SetCaddieInfo(info);
			dlg->Open(NULL, FrESCAPE | FrENTER);
		}
	}
}

void FrCheckoutDlg::OnDaysInit(int param)
{
	m_daysCombo[0].pCombo = DYNAMIC_CAST(FrComboBox, param);
}

void FrCheckoutDlg::OnDays2Init(int param)
{
	m_daysCombo[1].pCombo = DYNAMIC_CAST(FrComboBox, param);
}

void FrCheckoutDlg::OnDaysBtnDown()
{
	if (m_childOptionNum > 0)
	{
		const char* text = m_daysCombo[0].pCombo->GetLine(1, false);
		for (int j = 0; j < m_childOptionNum; ++j)
			if (!stricmp(m_childOption[j].text, text))
				m_daysCombo[0].pItem->period = j;
	}
	else
	{
		const char* text = m_daysCombo[0].pCombo->GetLine(1, false);
		for (int j = 0; j < 5; ++j)
			if (!stricmp(days[j].text, text))
				m_daysCombo[0].pItem->period = j;
	}
	RecalculateTotalCookie(m_myPang, m_myCookie, true);
}

void FrCheckoutDlg::OnDays2BtnDown()
{
	if (m_childOptionNum > 0)
	{
		const char* text = m_daysCombo[1].pCombo->GetLine(1, false);
		for (int j = 0; j < m_childOptionNum; ++j)
			if (!stricmp(m_childOption[j].text, text))
				m_daysCombo[1].pItem->period = j;
	}
	else
	{
		const char* text = m_daysCombo[1].pCombo->GetLine(1, false);
		for (int j = 0; j < 5; ++j)
			if (!stricmp(days[j].text, text))
				m_daysCombo[1].pItem->period = j;
	}
	RecalculateTotalCookie(m_myPang, m_myCookie, true);
}

void FrCheckoutDlg::OnCartInit(int param)
{
	m_pCart = DYNAMIC_CAST(FrListBox, param);
}

void FrCheckoutDlg::OnCartOwnerDraw(int param)
{
	FrListItem* draw = (FrListItem*)param;
	if (!draw)
		return;
	FrGraphicInterface* graphic = m_pCart->WndManager()->GetGDI();
	if (!graphic)
		return;
	CartItem& item = *(CartItem*)draw->pData;

	const Bitmap* bitmap;
	if ((item.pItem->TypeId >> 26) == 14)
		bitmap = m_pCart->WndManager()->GetBitmap("SKINS", item.pItem->Icon);
	else if ((item.pItem->TypeId >> 26) == 15)
		bitmap = m_pCart->WndManager()->GetBitmap("HAIRS", item.pItem->Icon);
	else
	{
		bitmap = m_pCart->WndManager()->GetBitmap("ITEMS", item.pItem->Icon);
		if (!bitmap)
			bitmap = m_pCart->WndManager()->GetBitmap("ITEMS_FASHION",
				item.pItem->Icon);
	}
	if (bitmap)
	{
		WRect rect(draw->pos.x, draw->pos.y, (float)bitmap->Width(),
			(float)bitmap->Height());
		if ((item.pItem->TypeId & 0xfc000000) != 0x08000000)
		{
			rect.x += 10.0f;
			rect.y += 20.0f;
		}
		graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
		if (m_childOptionNum > 0 && m_childOption[item.period].periodType)
			graphic->DrawTexture(m_miniBtn[6].pBitmap,
				m_miniBtn[6].rect + WPoint(draw->pos.x, draw->pos.y + 10.0f),
				0xffffffff, 0);
	}
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->SetTextStyle(1);
	float title = graphic->Print(WPoint(90.0f, 14.0f) + draw->pos, 0, "%s%s",
		item.pItem->Name,
		item.pItem->InStock == 3 ? "(\xb1\xb8\xc0\xd4\xc0\xfc\xbf\xeb)" : "");
	CItemManager* items = ItemManager();
	if (!items->IsInSalePeriod(item.pItem->TypeId, Doc()->GetServerTime()))
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->Print(WPoint((float)(int)title + 90.0f, 14.0f) + draw->pos, 0,
			"(\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3 \xc1\xbe\xb7\xe1)");
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	graphic->SetTextStyle(0);
	int width;
	char unit[8];
	if (m_childOptionNum > 0)
	{
		if (m_bRental ? 0 : (m_childOption[item.period].bCookie ? 1 : 0))
		{
			width = float2int(graphic->Print(WPoint(90.0f, 34.0f) + draw->pos,
				0, "\xb0\xa1    \xb0\xdd:   "));
			const char* currency = "\xc4\xed\xc5\xb0";
			strcpy(unit, currency);
		}
		else
		{
			width = float2int(graphic->Print(WPoint(90.0f, 34.0f) + draw->pos,
				0, "\xb0\xa1    \xb0\xdd:   "));
			const char* currency = "\xc6\xce";
			strcpy(unit, currency);
		}
	}
	else
	{
		if (m_bRental ? 0 : (item.pItem->IsCash ? 1 : 0))
		{
			width = float2int(graphic->Print(WPoint(90.0f, 34.0f) + draw->pos,
				0, "\xb0\xa1    \xb0\xdd:   "));
			const char* currency = "\xc4\xed\xc5\xb0";
			strcpy(unit, currency);
		}
		else
		{
			width = float2int(graphic->Print(WPoint(90.0f, 34.0f) + draw->pos,
				0, "\xb0\xa1    \xb0\xdd:   "));
			const char* currency = "\xc6\xce";
			strcpy(unit, currency);
		}
	}

	if ((item.pItem->TypeId >> 26) == 14)
	{
		if (S5::ISCutinItem(item.pItem->TypeId))
		{
			IFF_STRUCT::sSkin* skin =
				ItemManager()->FindSkin(item.pItem->TypeId);
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d%s", skin->c.Price, unit);
		}
		else if ((item.pItem->TypeId & 0x3c00000) == 0x1800000)
		{
			IFF_STRUCT::sSkin* skin =
				ItemManager()->FindSkin(item.pItem->TypeId);
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d%s", skin->c.Price, unit);
		}
		else
		{
			IFF_STRUCT::sSkin* skin =
				ItemManager()->FindSkin(item.pItem->TypeId);
			graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
				"%d%s", skin->COM[item.period], unit);
		}
	}
	else if ((item.pItem->TypeId >> 26) == 8)
	{
		IFF_STRUCT::sCadItem* part =
			ItemManager()->FindCadItem(item.pItem->TypeId);
		graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
			"%d%s", part->COM[item.period], unit);
	}
	else if ((item.pItem->TypeId >> 26) == 16)
	{
		IFF_STRUCT::sMascot* part =
			ItemManager()->FindMascot(item.pItem->TypeId);
		graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
			"%d%s", part->COM[item.period], unit);
	}
	else if (m_childOptionNum > 0)
	{
		__int64 price = 0;
		if (m_childOption[item.period].numType)
			price = m_childOption[item.period].numPrice;
		else if (m_childOption[item.period].periodType)
			price = m_childOption[item.period].periodPrice;
		graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
			"%I64d%s", price, unit);
	}
	else if (m_bRental)
	{
		unsigned long rentalPrice =
			S5::RENTALPARTS::GetRentalPrice(item.pItem->TypeId);
		graphic->Print(WPoint((float)width + 90.0f, 34.0f) + draw->pos, 0,
			"%d%s", rentalPrice, unit);
	}
	else
	{
		float first = (float)width;
		float priceX = first + 90.0f;
		width = float2int(graphic->Print(WPoint(priceX, 34.0f) + draw->pos, 0,
			"%d%s", item.pItem->Price, unit));
		if (item.pItem->SalePrice > 0 &&
			item.pItem->SalePrice < item.pItem->Price)
		{
			float end = first + width;
			g_view->DrawLine2D(
				WPoint(priceX - 2.0f,
					34.0f + float2int(graphic->GetFontHeight() * 0.5f)) +
					draw->pos,
				WPoint(end + 92.0f,
					34.0f + float2int(graphic->GetFontHeight() * 0.5f)) +
					draw->pos,
				0xff000000, 0xff000000, 0);
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->Print(WPoint(end + 90.0f, 34.0f) + draw->pos, 0, " %d%s",
				item.pItem->SalePrice, unit);
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
	}
	SYSTEMTIME* start = NULL;
	SYSTEMTIME* end = NULL;
	float row = 54.0f;
	if (ItemManager()->HasSalePeriod(item.pItem->TypeId, &start, &end) &&
		start->wYear && end->wYear)
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->Print(WPoint(90.0f, 54.0f) + draw->pos, 0,
			MakeStr(
				"\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf",
				start->wMonth, start->wDay, end->wMonth, end->wDay));
		graphic->SetTextColor(0xff000000, 0xffffffff);
		row = 74.0f;
		for (int i = 0; i < 2; ++i)
		{
			static WRect original = m_daysCombo[i].pCombo->GetRect();
			WRect rect = original;
			rect.y += 18.0f;
			m_daysCombo[i].pCombo->FrWnd::SetRect(rect);
		}
	}
	bitmap = g_pFresh->GetManager()->GetBitmap("LEVELS",
		MakeStr("level_%03d", item.pItem->Level + 1));
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
		const char* relation =
			!item.pItem->IsUnderLvl ? "\xc0\xcc\xbb\xf3" : "\xc0\xcc\xc7\xcf";
		if ((!item.pItem->IsUnderLvl &&
				item.pItem->Level > Doc()->m_myInfo.stat.Level) ||
			(item.pItem->IsUnderLvl &&
				item.pItem->Level < Doc()->m_myInfo.stat.Level))
		{
			graphic->Print(WPoint(rect.x + rect.w + 5.0f, row + draw->pos.y), 0,
				relation);
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->Print(WPoint(graphic->GetTextExtend(relation) + rect.w +
								   rect.x + 10.0f,
							   row + draw->pos.y),
				0, "(\xb1\xb8\xc0\xd4\xba\xd2\xb0\xa1)");
		}
		else
			graphic->Print(WPoint(rect.x + rect.w + 5.0f, row + draw->pos.y), 0,
				relation);
	}
	row += 20.0f;

	if ((item.pItem->TypeId >> 26) == 7)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		IFF_STRUCT::sCaddie* caddie =
			ItemManager()->FindCaddie(item.pItem->TypeId);
		if (caddie && caddie->MonthlyFee <= 0)
		{
			// bad edge case for clang-format :(
			graphic->Print(WPoint(graphic->GetTextExtend(
									  "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") +
								   95.0f,
							   row) +
					draw->pos,
				0, "\xb9\xab\xc1\xa6\xc7\xd1");
		}
		else
		{
			graphic->Print(WPoint(graphic->GetTextExtend(
									  "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") +
								   95.0f,
							   row) +
					draw->pos,
				0,
				"\xb1\xb8\xc0\xd4\xc0\xcf\xb7\xce\xba\xce\xc5\xcd %d\xc0\xcf \xc8\xc4 \xc0\xe7\xb0\xed\xbf\xeb",
				30);
		}
		if (item.ownedNum)
		{
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			if ((item.pItem->TypeId & 0xfc000000) == 0x1c000000)
				row += 20.0f;
			graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
				" (\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf8 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6 %d)",
				item.ownedNum);
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
	}
	else if ((item.pItem->TypeId >> 26) == 8)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (item.ownedNum)
		{
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->Print(WPoint(230.0f, row) + draw->pos, 0,
				"(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
		IFF_STRUCT::sCadItem* part =
			ItemManager()->FindCadItem(item.pItem->TypeId);
		item.pDaysCombo = &m_daysCombo[draw->no % 2 ? 1 : 0];
		if (item.pDaysCombo->bRebuild)
		{
			item.pDaysCombo->pCombo->ClearList();
			for (int i = 0; i < 4; ++i)
				if (part->COM[i] && i != 1)
					item.pDaysCombo->pCombo->AddString(days[i].text);
			item.pDaysCombo->bRebuild = false;
			item.period = 0;
			item.pDaysCombo->pCombo->SetLine(1, days[item.period].text, 0,
				false, 0);
			item.pDaysCombo->pCombo->SetVisible(true);
			RecalculateTotalCookie(0, 0, false);
		}
		item.pDaysCombo->pItem = &item;
	}
	else if ((item.pItem->TypeId >> 26) == 14)
	{
		if (!S5::ISCutinItem(item.pItem->TypeId) &&
			(item.pItem->TypeId & 0x3c00000) != 0x1800000)
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
				"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			if (item.ownedNum)
			{
				graphic->SetTextColor(0xffff0000, 0xffffffff);
				graphic->Print(WPoint(230.0f, row) + draw->pos, 0,
					"(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}
		}
		IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(item.pItem->TypeId);
		int index = draw->no % 2 ? 1 : 0;
		if (m_daysCombo[index].pItem)
			m_daysCombo[index].bRebuild =
				item.pItem->TypeId != m_daysCombo[index].pItem->pItem->TypeId;
		else
			m_daysCombo[index].bRebuild = true;
		item.pDaysCombo = &m_daysCombo[index];
		if (item.pDaysCombo->bRebuild)
		{
			item.pDaysCombo->pCombo->ClearList();
			for (int i = 0; i < 5; ++i)
				if (skin->COM[i])
					item.pDaysCombo->pCombo->AddString(days[i].text);
			item.pDaysCombo->bRebuild = false;
		}
		item.pDaysCombo->pCombo->SetLine(1, days[item.period].text, 0, false,
			0);
		if (S5::ISCutinItem(item.pItem->TypeId))
			item.pDaysCombo->pCombo->SetVisible(false);
		else if ((item.pItem->TypeId & 0x3c00000) == 0x1800000)
			item.pDaysCombo->pCombo->SetVisible(false);
		else
			item.pDaysCombo->pCombo->SetVisible(true);
		item.pDaysCombo->pItem = &item;
	}
	else if ((item.pItem->TypeId >> 26) == 16)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (item.ownedNum)
		{
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->Print(WPoint(230.0f, row) + draw->pos, 0,
				"(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
		IFF_STRUCT::sMascot* mascot =
			ItemManager()->FindMascot(item.pItem->TypeId);
		item.pDaysCombo = &m_daysCombo[draw->no % 2 ? 1 : 0];
		if (item.pDaysCombo->bRebuild)
		{
			item.pDaysCombo->pCombo->ClearList();
			for (int i = 0; i < 5; ++i)
				if (mascot->COM[i])
					item.pDaysCombo->pCombo->AddString(days[i].text);
			item.pDaysCombo->bRebuild = false;
		}
		item.pDaysCombo->pCombo->SetLine(1, days[item.period].text, 0, false,
			0);
		item.pDaysCombo->pCombo->SetVisible(true);
		item.pDaysCombo->pItem = &item;
	}
	else if (m_childOptionNum > 0)
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->Print(WPoint(90.0f, row) + draw->pos, 0,
			"\xb1\xb8\xc0\xd4\xb9\xe6\xb9\xfd:");
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		if (item.ownedNum)
		{
			graphic->SetTextColor(0xffff0000, 0xffffffff);
			graphic->Print(WPoint(230.0f, row) + draw->pos, 0,
				"(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
			graphic->SetTextColor(0xff000000, 0xffffffff);
		}
		item.pDaysCombo = &m_daysCombo[draw->no % 2 ? 1 : 0];
		if (item.pDaysCombo->bRebuild)
		{
			item.pDaysCombo->pCombo->ClearLine();
			for (int i = 0; i < m_childOptionNum; ++i)
				item.pDaysCombo->pCombo->AddString(m_childOption[i].text);
			item.pDaysCombo->bRebuild = false;
		}
		item.pDaysCombo->pCombo->SetLine(1, m_childOption[item.period].text, 0,
			false, 0);
		item.pDaysCombo->pCombo->SetVisible(true);
		item.pDaysCombo->pItem = &item;
	}
	if (m_pExplaneStatic && m_bShowCash)
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
			const WRect* rect = &m_pExplaneStatic->GetRect();
			if (m_remainCash < 0)
			{
				graphic->Print(WPoint(rect->x - 70.0f, rect->y - 20.0f), 0,
					MakeStr(
						"%s \xc0\xdc\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
						currency));
			}
			else
			{
				__int64 cash = doc->m_cash;
				__int64 exchanged =
					doc->m_bonusCash - m_totalCookie + m_discountCookie;
				exchanged = S5::ExchangeOwnCookieToCash(exchanged, 100);
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
	if (IsLocalContent((localContentType_t)0x17) &&
		!Doc()->IsHaveHalloweenItem() &&
		(item.pItem->TypeId & 0xfc000000) == 0x08000000 &&
		(item.pItem->TypeId & 0x1800) == 0x1800)
	{
		graphic->SetTextColor(0xffff0000, 0xffffffff);
		graphic->Print(WPoint(20.0f, 330.0f) + draw->pos, 0,
			"*\xc8\xa3\xb9\xda\xb8\xf0\xc0\xda\xb4\xc2 \xb0\xe8\xc1\xa4\xb4\xe7 \xc7\xd1\xb9\xf8 \xb9\xdb\xbf\xa1 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		graphic->SetTextColor(0xff000000, 0xffffffff);
	}
	{
		WRect cartRect = m_pCart->GetRect() + draw->pos;
		WRect buttonRect =
			m_miniBtn[6].rect + WPoint(draw->pos.x, draw->pos.y + 10.0f);
	}
}

void FrCheckoutDlg::OnPriceInit(int param)
{
	m_pPrice = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnMyMoneyInit(int param)
{
	m_pMyMoney = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnMoneyLeftInit(int param)
{
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnPrice_CookieInit(int param)
{
	m_pPrice_Cookie = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnMyMoney_CookieInit(int param)
{
	m_pMyMoney_Cookie = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnMoneyLeft_CookieInit(int param)
{
	m_pMoneyLeft_Cookie = DYNAMIC_CAST(FrEdit, param);
}

void FrCheckoutDlg::OnNumStaticInit(int param)
{
	m_pNumStatic = DYNAMIC_CAST(FrStatic, param);
}

void FrCheckoutDlg::OnExplaneStaticInit(int param)
{
	m_pExplaneStatic = DYNAMIC_CAST(FrStatic, param);
}

void FrCheckoutDlg::OnNumInit(int param)
{
	m_pNum = DYNAMIC_CAST(FrEdit, param);
}

bool FrCheckoutDlg::OnNumEnterKey(int param)
{
	return false;
}

void FrCheckoutDlg::OnNumPrevInit(int param)
{
	m_pNumPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pNumPrev)
		m_pNumPrev->SetPushDelay(0.0f);
}

void FrCheckoutDlg::OnNumPrevLBtnUp()
{
	if (m_pNum)
	{
		int num = atoi(m_pNum->GetLine(1, false));
		if (num > 1)
			SetPrice(num - 1);
	}
}

void FrCheckoutDlg::OnNumNextInit(int param)
{
	m_pNumNext = DYNAMIC_CAST(FrButton, param);
	if (m_pNumNext)
		m_pNumNext->SetPushDelay(0.0f);
}

void FrCheckoutDlg::OnNumNextLBtnUp()
{
	if (m_pNum)
	{
		int num = atoi(m_pNum->GetLine(1, false));
		if (num < 99)
			SetPrice(num + 1);
	}
}
