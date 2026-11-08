#include <numeric>
#include "tradedlg.h"
#include "iteminfodlg.h"
#include "itemstorage.h"
#include "chatmsg.h"
#include "frdesktop.h"
#include "gatewayactor.h"
#include "binstr.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;
void DrawAbility(const sTradeItem& item, FrListItem* row);
void DrawPartsSlot(WPoint pos, sTradeItem* item)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
	if (!part)
		return;
	int slot;
	float characterX = 5.f;
	float caddieX = 5.f;
	for (slot = 1; slot < part->CharacterSlot + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTid != (*it).partsTid || item->dwGuid != (*it).partsUid)
				continue;
			IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
			if (!card || slot != (*it).slotNum || (*it).cardType != 0)
				continue;
			found = true;
			const Bitmap* bitmap = g_pFresh->GetBitmap(card->SlotImg);
			if (!bitmap)
				continue;
			graphic->DrawTexture(bitmap,
				WRect(pos.x + characterX, pos.y + 54.0f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
			characterX += 16.0f;
		}
		if (found)
			continue;
		const Bitmap* bitmap = g_pFresh->GetBitmap("card_character_empty");
		if (!bitmap)
			continue;
		graphic->DrawTexture(bitmap,
			WRect(pos.x + characterX, pos.y + 54.0f, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
		characterX += 16.0f;
	}
	for (slot = 1; slot < part->CaddieSlot + 1; ++slot)
	{
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it =
				 Doc()->m_cardAbilityList[0].begin();
			it != Doc()->m_cardAbilityList[0].end(); ++it)
		{
			if (item->dwTid != (*it).partsTid || item->dwGuid != (*it).partsUid)
				continue;
			IFF_STRUCT::sCard* card = ItemManager()->FindCard((*it).tid);
			if (!card || slot != (*it).slotNum || (*it).cardType != 1)
				continue;
			found = true;
			const Bitmap* bitmap = g_pFresh->GetBitmap(MakeStr(card->SlotImg));
			if (!bitmap)
				continue;
			graphic->DrawTexture(bitmap,
				WRect(pos.x + caddieX, pos.y + 70.0f, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
			caddieX += 16.0f;
		}
		if (found)
			continue;
		const Bitmap* bitmap = g_pFresh->GetBitmap("card_caddie_empty");
		if (!bitmap)
			continue;
		graphic->DrawTexture(bitmap,
			WRect(pos.x + caddieX, pos.y + 70.0f, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
		caddieX += 16.0f;
	}
}

void SetTextColorByPrice(__int64 price)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (price > 9999999)
		gi->SetTextColor(0xfff50018, 0xffffffff);
	else if (price > 999999)
		gi->SetTextColor(0xffd5009b, 0xffffffff);
	else if (price > 9999)
		gi->SetTextColor(0xffc79500, 0xffffffff);
	else if (price > 999)
		gi->SetTextColor(0xff0e0e0e, 0xffffffff);
	else
		gi->SetTextColor(0xff626262, 0xffffffff);
}
__int64 SumForTradeItemList(__int64 sum, const sTradeItem& item)
{
	return sum + item.i64Price;
}

bool CheckPackageSaleItem(const sTradeItem& item)
{
	switch (item.dwTid >> 26)
	{
	case 2:
		return IsUccClothes(item.dwTid) ? true : false;
	case 4:
		return false;
	default:
		return true;
	}
}

void DrawAbility(const sTradeItem& item, FrListItem* row)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	short ability[5];
	short slots[5] = { 0 };
	unsigned long type = item.dwTid >> 26;
	bool draw = true;
	switch (type)
	{
	case 2:
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item.dwTid);
		if (!part)
			return;
		memcpy(ability, part->Attr, sizeof(ability));
		memcpy(slots, part->Slot, sizeof(slots));
		if (IsLocalContent(S4_CARD_SYSTEM) && row)
			DrawPartsSlot(row->pos, (sTradeItem*)&item);
	}
	break;
	case 7:
	{
		IFF_STRUCT::sCaddie* caddie = ItemManager()->FindCaddie(item.dwTid);
		if (!caddie)
			return;
		memcpy(ability, caddie->Attr, sizeof(ability));
	}
	break;
	case 8:
	{
		IFF_STRUCT::sCadItem* caddie = ItemManager()->FindCadItem(item.dwTid);
		if (!caddie)
			return;
		memcpy(ability, caddie->COM, sizeof(ability));
	}
	break;
	default:
		draw = false;
		break;
	}
	if (draw)
	{
		float dy = 0.f;
		const char* name = "";
		int i = 0;
		for (int remaining = 5; remaining; --remaining, ++i)
		{
			if (ability[i] || slots[i])
			{
				switch (i)
				{
				case 0:
					name = "\xc6\xc4\xbf\xf6";
					break;
				case 1:
					name = "\xc4\xc1\xc6\xae\xb7\xd1";
					break;
				case 2:
					name = "\xc1\xa4\xc8\xae\xb5\xb5";
					break;
				case 3:
					name = "\xbd\xba\xc7\xc9";
					break;
				case 4:
					name = "\xc4\xbf\xba\xea";
					break;
				}
				if (ability[i] > 0 || slots[i] > 0)
				{
					gi->SetTextColor(0xff0000ff, 0xffffffff);
					const char* text;
					if (ability[i] == 0)
						text = MakeStr("%s\xbd\xbd\xb7\xd4%+d", name, slots[i]);
					else if (slots[i] == 0)
						text = MakeStr("%s%+d", name, ability[i]);
					else
						text = MakeStr("%s%+d/\xbd\xbd\xb7\xd4%+d", name,
							ability[i], slots[i]);
					gi->Print(WPoint(182.f, dy + 36.f) + row->pos, 2, text);
					dy += 14.f;
				}
			}
		}
	}
	else if (type == 4)
	{
		gi->SetTextColor(0xff009000, 0xffffffff);
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 36.f), 2,
			"\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xc7\xd1 \xba\xf1\xbf\xeb:");
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 50.f), 2,
			MakeStr("%d\xc6\xce", item.dwUpgradeCost));
	}
}

bool CannotPurchaseItem(const FrListItem* row)
{
	sTradeItem* item = (sTradeItem*)row->pData;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	return (item->wFlagTradeLimit & 1) ? true : false;
}

const char* __fastcall MakeStatusString(int index, int ability, int slots)
{
	const char* names[] = { K2L_Compatibility("\xc6\xc4\xbf\xf6"),
		K2L_Compatibility("\xc4\xc1\xc6\xae\xb7\xd1"),
		K2L_Compatibility("\xc1\xa4\xc8\xae\xb5\xb5"),
		K2L_Compatibility("\xbd\xba\xc7\xc9"),
		K2L_Compatibility("\xc4\xbf\xba\xea") };
	if (ability != 0)
	{
		if (slots != 0)
			return MakeStr("%s+%d/%s+%d", names[index], ability,
				K2L_Compatibility("\xbd\xbd\xb7\xd4"), slots);
		return MakeStr("%s+%d", names[index], ability);
	}
	if (slots != 0)
	{
		return MakeStr("%s%s+%d", names[index],
			K2L_Compatibility("\xbd\xbd\xb7\xd4"), slots);
	}
	return "";
}

const char* __fastcall MakeCardStatusString(int index, int ability)
{
	const char* names[] = { K2L_Compatibility(
								"\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4") };
	if (ability != 0)
		return MakeStr("%s+%d", names[index], ability);
	return "";
}

IMPLEMENT_OBJECT(FrTradeDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeDlg, FrForm)
ON_FRESH_VI("tradedlg", FRCMD_INIT, FrTradeDlg::OnTradeDlgInit)
ON_FRESH_VI("visitor_back", FRCMD_INIT, FrTradeDlg::OnVisitorBackInit)
ON_FRESH_VV("visitor_back", FRCMD_OWNERDRAW, FrTradeDlg::OnVisitorBackOwnerDraw)
ON_FRESH_VI("mypang", FRCMD_INIT, FrTradeDlg::OnMyPangInit)
ON_FRESH_VV("mypang", FRCMD_OWNERDRAW, FrTradeDlg::OnMyPangOwnerDraw)
ON_FRESH_VI("trade_item_list", FRCMD_INIT, FrTradeDlg::OnTradeItemListInit)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONDOWN,
	FrTradeDlg::OnTradeItemListLClick)
ON_FRESH_VV("trade_item_list", FRCMD_RBUTTONDOWN,
	FrTradeDlg::OnTradeItemListRClick)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	FrTradeDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("noitem01", FRCMD_INIT, FrTradeDlg::OnNoItem01Init)
ON_FRESH_VI("noitem02", FRCMD_INIT, FrTradeDlg::OnNoItem02Init)
ON_FRESH_VI("visitor", FRCMD_INIT, FrTradeDlg::OnVisitorInit)
ON_FRESH_VI("userid", FRCMD_INIT, FrTradeDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, FrTradeDlg::OnTitleInit)
ON_FRESH_VI("edit", FRCMD_INIT, FrTradeDlg::OnEditBnInit)
ON_FRESH_VV("edit", FRCMD_LBUTTONUP, FrTradeDlg::OnEditBnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrTradeDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrTradeDlg::OnCloseBnUp)
ON_FRESH_VI("hide", FRCMD_INIT, FrTradeDlg::OnHideBnInit)
ON_FRESH_VV("hide", FRCMD_LBUTTONUP, FrTradeDlg::OnHideBnUp)
ON_FRESH_VI("exit", FRCMD_INIT, FrTradeDlg::OnExitBnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, FrTradeDlg::OnExitBnUp)
ON_FRESH_VI("package", FRCMD_INIT, FrTradeDlg::OnPackagePurchaseButtonInit)
ON_FRESH_VV("package", FRCMD_LBUTTONUP, FrTradeDlg::OnPackagePurchaseLButtonUp)
END_FRESH_MSGMAP()

FrTradeDlg::FrTradeDlg()
{
	m_pVisitorBack = NULL;
	m_pTradeItemList = NULL;
	m_pNoItem01 = NULL;
	m_pNoItem02 = NULL;
	m_pVisitor = NULL;
	m_pUserId = NULL;
	m_pTitle = NULL;
	m_pEditBn = NULL;
	m_pCloseBn = NULL;
	m_pHideBn = NULL;
	m_pExitBn = NULL;
	m_pItemBaseN = g_pFresh->GetBitmap("item_base_n");
	m_pItemBaseO = g_pFresh->GetBitmap("item_base_o");
	m_pItemBaseD = g_pFresh->GetBitmap("item_base_d");
	m_pItemBaseEmpty = g_pFresh->GetBitmap("item_base_empty");
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pNumFont = g_resrcmng->GetTitleFont();
	m_pNumFont->Create(&font);
	static char* pangFiles[] = { "[font_bong.jpg" };
	WTITLEFONT pangFont;
	memset(&pangFont, 0, sizeof(pangFont));
	pangFont.filename = pangFiles;
	pangFont.fontw = 9;
	pangFont.fonth = 16;
	pangFont.numPages = 1;
	pangFont.texw = 128;
	pangFont.texh = 32;
	pangFont.pCharSet = "1234567890,-";
	m_pPangFont = g_resrcmng->GetTitleFont();
	m_pPangFont->Create(&pangFont);
	m_bMyShop = false;
	m_ownerUID = 0xffffffff;
	memset(m_reserved168, 0, sizeof(m_reserved168));
	m_bSendingServer = false;
	m_reserved18c = 0;
	m_pPackagePurchaseBn = NULL;
	m_totalPrice = 0;
	m_packageSale = eTypeSalesTrade_None;
	m_buyBtn[0].pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtn[0].rect = WRect(144.f, 66.f, m_buyBtn[0].pBitmap->Width(),
		m_buyBtn[0].pBitmap->Height());
	m_buyBtn[0].index = -1;
	m_buyBtn[1].pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtn[1].rect = WRect(144.f, 66.f, m_buyBtn[1].pBitmap->Width(),
		m_buyBtn[1].pBitmap->Height());
	m_buyBtn[1].index = -1;
	m_clock.pBitmap = g_pFresh->GetBitmap("clock");
	m_clock.rect =
		WRect(50.f, 35.f, m_clock.pBitmap->Width(), m_clock.pBitmap->Height());
	m_deleteMark.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteMark.rect = WRect(50.f, 35.f, m_deleteMark.pBitmap->Width(),
		m_deleteMark.pBitmap->Height());
}

FrTradeDlg::~FrTradeDlg()
{
	if (m_pPangFont)
		m_pPangFont->Erase();
	if (m_pNumFont)
		m_pNumFont->Erase();
	if (g_resrcmng && m_pPangFont)
	{
		g_resrcmng->Release(m_pPangFont);
		m_pPangFont = NULL;
	}
	if (g_resrcmng && m_pNumFont)
	{
		g_resrcmng->Release(m_pNumFont);
		m_pNumFont = NULL;
	}
	if (!m_bMyShop && AfxGetTask() && CSharedDoc::IsInstantiated())
		Doc()->SendTradeExitShop(m_ownerUID);
}

void FrTradeDlg::OnTradeDlgInit(int param)
{
}

void FrTradeDlg::OnVisitorBackInit(int param)
{
	m_pVisitorBack = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeDlg::OnVisitorBackOwnerDraw()
{
}

void FrTradeDlg::OnMyPangInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeDlg::OnMyPangOwnerDraw()
{
	DrawPang();
}

void FrTradeDlg::OnTradeItemListInit(int param)
{
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pTradeItemList)
		m_pTradeItemList->UseRightButton(true);
}

void FrTradeDlg::OnTradeItemListLClick()
{
	FrListItem* row = m_pTradeItemList->GetItemUnderCursor();
	if (!row)
		return;
	if (!row->selected)
		m_pTradeItemList->ToggleItem(row);
	if (m_bMyShop || m_bSendingServer || IsPackageSale())
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	if ((item->wFlagTradeLimit & 1))
		return;
	const WPoint* mouse = &g_pFresh->GetManager()->GetMousePos();
	if (!(m_buyBtn[0].rect + row->pos).IsInRect(*mouse))
		return;
	m_buyBtn[0].index = row->idx;
	sTradeItem* selected = (sTradeItem*)row->pData;
	if (selected->i64Price > Doc()->m_myInfo.stat.i64Pang)
	{
		SetTradeResultCode(21, 0xe1, NULL);
		return;
	}
	if (Doc()->m_myInfo.stat.Level == 0)
	{
		SetTradeResultCode(23, 0xe1, NULL);
		return;
	}
	FrTradeBuyDlg* dialog =
		CreateForm<FrTradeBuyDlg>(g_pFresh->GetManager(), this, "tradebuydlg");
	if (!dialog)
		return;
	dialog->Open((FRESH_PFN_RESULT)&FrTradeDlg::OnTradeBuyDlgResult, FrRET_OK);
	dialog->SetFadeout(false);
	dialog->SetBuyItem(selected);
	dialog->SetControlsBuyItem(selected);
}

void FrTradeDlg::OnTradeItemListRClick()
{
	FrListItem* row = m_pTradeItemList->GetItemUnderCursor();
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	if (item)
	{
		FrItemInfoDlg* dialog =
			CreateForm<FrItemInfoDlg>(g_pFresh->GetManager(), this, "iteminfo");
		if (dialog)
		{
			dialog->Open(NULL, 3);
			dialog->SetItemInfo(item);
			if (IsLocalContent(S4_CARD_SYSTEM) && (item->dwTid >> 26) == 2)
			{
				FrEdit* edit =
					DYNAMIC_CAST(FrEdit, dialog->FindChildByName("avility"));
				if (edit)
				{
					IFF_STRUCT::sPart* part =
						ItemManager()->FindPart(item->dwTid);
					if (part)
						for (int i = 0; i < 5; ++i)
						{
							if (part->Attr[i] != 0 || part->Slot[i] != 0)
								edit->AddText(
									MakeStr(
										"\\c0xff0000ff\\c%s\\c0xff000000\\c",
										MakeStatusString(i, part->Attr[i],
											part->Slot[i])),
									false, true);
						}
					unsigned long* cardTid = &item->attachCardTid[0][0];
					for (int remaining = 8; remaining; --remaining, ++cardTid)
					{
						unsigned long tid = *cardTid;
						if (tid)
						{
							unsigned long type = (tid >> 22) & 15;
							if (type == 0)
							{
								IFF_STRUCT::sCard* card =
									ItemManager()->FindCard(tid);
								if (card)
									for (int i = 0; i < 5; ++i)
									{
										if (card->COM[i] > 0)
											edit->AddText(
												MakeStr(
													"\\c0xff60ee0e\\c%s\\c0xff000000\\c",
													MakeCardStatusString(i,
														card->COM[i])),
												false, true);
									}
							}
							else if (type == 1)
							{
								IFF_STRUCT::sDesc* desc =
									ItemManager()->FindDesc(tid);
								if (desc)
								{
									char text[512];
									strcpy(text, desc->Desc);
									for (int i = 0; i < 512; ++i)
									{
										if (text[i] == '\n')
										{
											text[i] = 0;
											break;
										}
									}
									edit->AddText(
										MakeStr(
											"\\c0xffff0000\\c%s\\c0xff000000\\c",
											text),
										false, true);
								}
							}
						}
					}
				}
			}
		}
	}
	if (!row->selected)
		m_pTradeItemList->ToggleItem(row);
}

void FrTradeDlg::OnTradeItemListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (!row->pData)
		return;
	unsigned long color = 0xffffffff;
	const Bitmap* bitmap = m_pItemBaseN;
	gi->SetTextColor(0xff000000, 0xffffffff);
	sTradeItem item = *(sTradeItem*)row->pData;
	unsigned long tid = item.dwTid;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(tid);
	if (!common)
		return;
	if (!m_bMyShop && (item.wFlagTradeLimit & 1))
		color = 0xff909090;
	unsigned long baseColor = color;
	if (IsPackageSale())
		baseColor = 0x20ffffff;
	gi->SetTextStyle(0);
	if (bitmap)
		gi->DrawTexture(bitmap,
			WRect(row->pos.x, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			baseColor, 0);
	unsigned long type = tid >> 26;
	if (type == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((tid >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* characterBitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (characterBitmap)
				gi->DrawTexture(characterBitmap,
					WRect(row->pos.x + 3.f, row->pos.y + 3.f,
						(float)characterBitmap->Width(),
						(float)characterBitmap->Height()),
					0xffffffff, 0);
		}
	}
	bitmap = g_pFresh->GetBitmap(common->Icon);
	if (bitmap)
	{
		float x, y;
		switch (type)
		{
		case 2:
			x = 7.f;
			y = 2.f;
			break;
		default:
			x = 16.f;
			y = 27.f;
			break;
		}
		if (IsLocalContent(S4_UCC) && (item.status & 1) &&
			IsUccClothes(item.dwTid))
			bitmap = GetUccItemIcon(item.dwGuid);
		if (bitmap)
			gi->DrawTexture(bitmap,
				WRect(row->pos.x + x, row->pos.y + y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				color, 0);
	}
	if (IsLocalContent(S4_UCC) && IsUccClothes(item.dwTid))
	{
		if ((item.status & 1) && item.Seq == 1 && m_pUccOriginal)
			gi->DrawTexture(m_pUccOriginal,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccOriginal->Width(),
					(float)m_pUccOriginal->Height()),
				0xffffffff, 0);
		else if ((item.status & 1) && item.Seq > 1 && m_pUccCopied)
			gi->DrawTexture(m_pUccCopied,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccCopied->Width(),
					(float)m_pUccCopied->Height()),
				0xffffffff, 0);
		else if ((item.status & 2) && m_pUccTempSaved)
			gi->DrawTexture(m_pUccTempSaved,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccTempSaved->Width(),
					(float)m_pUccTempSaved->Height()),
				0xffffffff, 0);
	}

	const char* name;
	if (IsLocalContent(S4_UCC) && IsUccClothes(item.dwTid))
		name = GetUccItemName(item.dwGuid);
	else
		name = common->Name;
	gi->SetTextStyle(1);
	gi->Print(WPoint(182.f, 7.f) + row->pos, 2, name);
	gi->SetTextStyle(0);
	char price[32] = { 0 };
	MakePriceStrA(price, item.i64Price);
	SetTextColorByPrice(item.i64Price);
	if (item.iNum > 1)
	{
		unsigned int i = 0;
		char* count = MakeStr("%d", item.iNum);
		float x = row->pos.x + 54.f;
		for (i = 0; i < strlen(count); ++i)
			x -= m_pNumFont->GetCharWidthA(g_view, count[i] - '0');
		m_pNumFont->Print(g_view, x, row->pos.y + 55.f, count, 0, 0xffffffff,
			NULL);
		gi->Print(WPoint(182.f, 23.f) + row->pos, 2,
			MakeStr("\xb0\xb3\xb4\xe7 %s\xc6\xce", price));
	}
	else
		gi->Print(WPoint(182.f, 23.f) + row->pos, 2,
			MakeStr("%s\xc6\xce", price));
	gi->SetTextColor(0xff000000, 0xffffffff);
	DrawAbility(item, row);
	bool canBuy = !(item.wFlagTradeLimit & 1);
	if (!m_bMyShop && !IsPackageSale() && canBuy)
	{
		const WPoint* mouse = &g_pFresh->GetManager()->GetMousePos();
		if (row->underCursor && (m_buyBtn[0].rect + row->pos).IsInRect(*mouse))
			gi->DrawTexture(m_buyBtn[1].pBitmap, m_buyBtn[1].rect + row->pos,
				0xffffffff, 0);
		else
			gi->DrawTexture(m_buyBtn[0].pBitmap, m_buyBtn[0].rect + row->pos,
				0xffffffff, 0);
	}
	switch (item.dwTid >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(row->pos.x + 40.f -
						float2int((float)level->Width() * 0.5f),
					row->pos.y + 51.f -
						float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				color, 0);
	}
	break;
	}
}

void FrTradeDlg::OnNoItem01Init(int param)
{
	m_pNoItem01 = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pNoItem01)
		m_pNoItem01->SetVisible(false);
}

void FrTradeDlg::OnNoItem02Init(int param)
{
	m_pNoItem02 = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pNoItem02)
		m_pNoItem02->SetVisible(false);
}

void FrTradeDlg::OnVisitorInit(int param)
{
	m_pVisitor = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	UpdateCountVisitor();
}

void FrTradeDlg::OnUserIdInit(int param)
{
	m_pUserId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	UpdateTradeTitle();
}

void FrTradeDlg::OnEditBnInit(int param)
{
	m_pEditBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeDlg::OnEditBnUp()
{
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	if (actor)
		actor << MsgObject(NULL, 0x273, IsPackageSale() ? 4 : 5, 0, 0, 0, 0);
	FrTradeEditDlg* dialog = CreateForm<FrTradeEditDlg>(g_pFresh->GetManager(),
		this, "tradeeditdlg");
	if (dialog)
	{
		dialog->SetPackageSale(GetPackageSale());
		dialog->Open(NULL, FrRET_OK | FrRET_CANCEL);
		dialog->SetFadeout(false);
		Doc()->SendTradeEditShop();
		OnFreshOkay();
	}
}

void FrTradeDlg::OnCloseBnInit(int param)
{
	m_pCloseBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeDlg::OnCloseBnUp()
{
	if (Doc()->GetStateTrade() == STATE_TRADE_OUTOFSTOCK)
		SetTradeResultCode(19, 0xe1, NULL);
	else
		SetTradeResultCode(18, 0xe1, NULL);
}

void FrTradeDlg::OnHideBnInit(int param)
{
	m_pHideBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeDlg::OnHideBnUp()
{
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	if (actor)
		actor << MsgObject(NULL, 0x273, IsPackageSale() ? 4 : 5, 0, 0, 0, 0);
	OnFreshOkay();
}

void FrTradeDlg::OnExitBnInit(int param)
{
	m_pExitBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeDlg::OnExitBnUp()
{
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	if (actor)
		actor << MsgObject(NULL, 0x273, 5, 0, 0, 0, 0);
	OnFreshOkay();
}

void FrTradeDlg::OnPackagePurchaseButtonInit(int param)
{
	m_pPackagePurchaseBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeDlg::OnPackagePurchaseLButtonUp()
{
	if (!IsPackageSale() || m_bMyShop || m_bSendingServer)
		return;
	if (FindIf(m_pTradeItemList->m_itemList, CannotPurchaseItem))
		return;
	if (m_totalPrice > Doc()->m_myInfo.stat.i64Pang)
	{
		SetTradeResultCode(21, 0xe1, NULL);
	}
	else
	{
		if (Doc()->m_myInfo.stat.Level == 0)
		{
			SetTradeResultCode(23, 0xe1, NULL);
			return;
		}
		FrTradeBuyDlg* dialog = CreateForm<FrTradeBuyDlg>(
			g_pFresh->GetManager(), this, "tradebuydlg");
		if (dialog)
		{
			dialog->Open((FRESH_PFN_RESULT)&FrTradeDlg::OnTradeBuyDlgResult,
				FrRET_OK);
			dialog->SetFadeout(false);
			dialog->SetPackageSale();
			dialog->SetBuyItemList();
			dialog->SetControlsItemList();
		}
	}
}

bool FrTradeDlg::OnInit()
{
	EnableDrag(false);
	switch (Doc()->GetStateTrade())
	{
	case STATE_TRADE_OPEN:
	case STATE_TRADE_OUTOFSTOCK:
	case STATE_TRADE_ENTER:
		if (Doc()->GetListTradeItem()->size() <= 0)
		{
			ShowStaticOutOfStock(true);
			EnableHideBn(false);
		}
		else
		{
			ShowStaticOutOfStock(false);
			EnableHideBn(true);
		}
		break;
	}
	return true;
}

void FrTradeDlg::OpenTradeShop(unsigned long uid)
{
	if (uid == 0xffffffff)
	{
		OnFreshOkay();
		return;
	}
	m_ownerUID = uid;
	if (uid == Doc()->m_myInfo.info.dwUID)
	{
		m_bMyShop = true;
		SetListTradeItem(false);
		if (m_pVisitorBack)
			m_pVisitorBack->SetVisible(true);
		if (m_pVisitor)
			m_pVisitor->SetVisible(true);
		if (m_pEditBn)
			m_pEditBn->SetVisible(true);
		if (m_pCloseBn)
			m_pCloseBn->SetVisible(true);
		if (m_pHideBn)
			m_pHideBn->SetVisible(true);
		if (m_pExitBn)
			m_pExitBn->SetVisible(false);
		if (m_pPackagePurchaseBn)
			m_pPackagePurchaseBn->SetVisible(false);
	}
	else
	{
		m_bMyShop = false;
		if (m_pVisitorBack)
			m_pVisitorBack->SetVisible(false);
		if (m_pVisitor)
			m_pVisitor->SetVisible(false);
		if (m_pEditBn)
			m_pEditBn->SetVisible(false);
		if (m_pCloseBn)
			m_pCloseBn->SetVisible(false);
		if (m_pHideBn)
			m_pHideBn->SetVisible(false);
		if (m_pExitBn)
			m_pExitBn->SetVisible(true);
	}
	if (m_pUserId)
	{
		char text[64] = { 0 };
		sprintf(text, "\xb3\xaa\xc0\xc7 \xbb\xf3\xc1\xa1");
		if (!m_bMyShop)
		{
			for (std::list<sSlotInfo>::iterator it = Doc()->m_slotList.begin();
				it != Doc()->m_slotList.end(); ++it)
			{
				if ((*it).dwUserUID == Doc()->GetTradeUID())
				{
					sprintf(text,
						MakeStr("%s \xb4\xd4\xc0\xc7 \xbb\xf3\xc1\xa1",
							(*it).sNick));
					break;
				}
			}
		}
		m_pUserId->SetLine(1, text, 0, false, 0);
	}
	if (!m_bMyShop)
	{
		m_pNoItem01->SetCaption(
			"\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc7\xb0\xc0\xfd\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pNoItem02->SetCaption(
			"\xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xc5\xbc\xad \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9.");
	}
}

void FrTradeDlg::SetListTradeItem(bool clear)
{
	if (m_pTradeItemList)
	{
		m_totalPrice = 0;
		m_pTradeItemList->ClearItem();
		if (!clear)
		{
			std::list<sTradeItem>* list = Doc()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator it = list->begin();
				it != list->end(); ++it)
			{
				IFF_ITEM_COMMON* common =
					ItemManager()->FindCommonItem((*it).dwTid);
				if (common)
				{
					(*it).wFlagTradeLimit = 0;
					if ((common->Level & 0x7f) > Doc()->m_myInfo.stat.Level)
						(*it).wFlagTradeLimit |= 1;
					m_pTradeItemList->AddItem(&*it);
					m_totalPrice += (*it).i64Price;
				}
			}
		}
	}
}

void FrTradeDlg::DrawPang()
{
	if (g_pFresh->GetManager()->GetGDI() && m_pPangFont)
	{
		char text[32] = { 0 };
		WRect pos = GetRect();
		static float width;
		static int fontWidth = m_pPangFont->GetFontInfo()->fontw;
		if (m_bMyShop)
			MakePriceStrA(text, Doc()->GetTradeIncome());
		else if (m_packageSale == eTypeSalesTrade_Each)
			MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		else
			MakePriceStrA(text, m_totalPrice);
		width = (float)((strlen(text) - 1) * (fontWidth - 1.f));
		m_pPangFont->Print(g_view, pos.x + 130.f - width, pos.y + 380.f, text,
			0, 0xffffffff, NULL);
	}
}

void FrTradeDlg::UpdateTradeTitle()
{
	if (m_pTitle)
	{
		m_pTitle->ClearLine();
		const char* title = Doc()->GetTradeTitle();
		m_pTitle->SetLine(1, title, 0, false, 0);
	}
}

void FrTradeDlg::UpdateCountVisitor()
{
	if (m_pVisitor)
	{
		char text[32] = { 0 };
		MakePriceStrA(text, Doc()->GetCountVisitor());
		m_pVisitor->SetLine(1, text, 0, false, 0);
	}
}

bool FrTradeDlg::OnTradeBuyDlgResult(int result, FrForm* form)
{
	if (result == FrOK)
		EnableSendingServer(true);
	return true;
}

void FrTradeDlg::EnableSendingServer(bool enable)
{
	m_bSendingServer = enable;
	if (m_pExitBn)
		m_pExitBn->Enable(!enable);
}

void FrTradeDlg::ShowStaticOutOfStock(bool show)
{
	if (m_pNoItem01)
		m_pNoItem01->SetVisible(show);
	if (m_pNoItem02)
		m_pNoItem02->SetVisible(show);
}

void FrTradeDlg::EnableHideBn(bool enable)
{
	if (m_pHideBn)
		m_pHideBn->Enable(enable);
}

void FrTradeDlg::SetPackageSale(eTypeSalesTrade type)
{
	m_packageSale = type;
	if (type != eTypeSalesTrade_Package && m_pPackagePurchaseBn)
	{
		m_pPackagePurchaseBn->SetVisible(false);
		m_pPackagePurchaseBn->Enable(false);
	}
}

IMPLEMENT_OBJECT(FrTradeBuyDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeBuyDlg, FrForm)
ON_FRESH_VI("tradebuydlg", FRCMD_INIT, FrTradeBuyDlg::OnTradeBuyDlgInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrTradeBuyDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTradeBuyDlg::OnOkBnUp)
ON_FRESH_VI("buyitem_icon", FRCMD_INIT, FrTradeBuyDlg::OnBuyItemIconInit)
ON_FRESH_VV("buyitem_icon", FRCMD_OWNERDRAW,
	FrTradeBuyDlg::OnBuyItemIconOwnerDraw)
ON_FRESH_VI("buyitem_name", FRCMD_INIT, FrTradeBuyDlg::OnBuyItemNameInit)
ON_FRESH_VI("buyitem_ability", FRCMD_INIT, FrTradeBuyDlg::OnBuyItemAbilityInit)
ON_FRESH_VI("my_pang", FRCMD_INIT, FrTradeBuyDlg::OnMyPangInit)
ON_FRESH_VI("price", FRCMD_INIT, FrTradeBuyDlg::OnPriceInit)
ON_FRESH_VI("pang_left", FRCMD_INIT, FrTradeBuyDlg::OnPangLeftInit)
ON_FRESH_VI("num", FRCMD_INIT, FrTradeBuyDlg::OnNumberInit)
ON_FRESH_VI("num_next", FRCMD_INIT, FrTradeBuyDlg::OnNumberNextBnInit)
ON_FRESH_VV("num_next", FRCMD_LBUTTONUP, FrTradeBuyDlg::OnNumberNextBnUp)
ON_FRESH_VI("num_prev", FRCMD_INIT, FrTradeBuyDlg::OnNumberPrevBnInit)
ON_FRESH_VV("num_prev", FRCMD_LBUTTONUP, FrTradeBuyDlg::OnNumberPrevBnUp)
ON_FRESH_VI("purchase_list", FRCMD_INIT, FrTradeBuyDlg::OnPurchaseListInit)
ON_FRESH_VI("purchase_list", FRCMD_OWNERDRAW,
	FrTradeBuyDlg::OnPurchaseListOwnerdraw)
END_FRESH_MSGMAP()

FrTradeBuyDlg::FrTradeBuyDlg()
{
	m_pBuyItemIcon = NULL;
	m_pBuyItemName = NULL;
	m_pBuyItemAbility = NULL;
	m_pMyPang = NULL;
	m_pPrice = NULL;
	m_pPangLeft = NULL;
	m_pNumberNextBn = NULL;
	m_pNumberPrevBn = NULL;
	m_pNumber = NULL;
	memset(&m_buyItem, 0, sizeof(m_buyItem));
	m_pBuyItem = NULL;
	m_bClosing = false;
	m_pPurchaseList = NULL;
	m_packageSale = eTypeSalesTrade_None;
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pNumFont = g_resrcmng->GetTitleFont();
	m_pNumFont->Create(&font);
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	m_prevNumber = 0;
}

FrTradeBuyDlg::~FrTradeBuyDlg()
{
	if (m_pNumFont)
		m_pNumFont->Erase();
	if (g_resrcmng && m_pNumFont)
	{
		g_resrcmng->Release(m_pNumFont);
		m_pNumFont = NULL;
	}
}

void FrTradeBuyDlg::OnTradeBuyDlgInit(int param)
{
}

void FrTradeBuyDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeBuyDlg::OnOkBnUp()
{
	bool canBuy = true;
	if (m_packageSale == eTypeSalesTrade_Package)
	{
		if (Doc()->GetListTradeItem()->size() == 0)
		{
			SetTradeResultCode(13, 0xe1, NULL);
			canBuy = false;
		}
		else
		{
			std::list<sTradeItem>* list = Doc()->GetListTradeItem();
			__int64 sum = std::accumulate(list->begin(), list->end(),
				(__int64)0, SumForTradeItemList);
			if (sum > Doc()->m_myInfo.stat.i64Pang)
			{
				SetTradeResultCode(21, 0xe1, NULL);
				canBuy = false;
			}
			for (std::list<sTradeItem>::iterator it = list->begin();
				it != list->end(); ++it)
			{
				if (!Doc()->ConfirmInsertItem((*it).dwTid, (*it).iNum))
				{
					FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify");
					WRect rect = form->GetRect();
					form->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
					form->SetTimeLimit(12.f);
					form->SetMessage(
						"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						false);
					form->Open(NULL, 0);
					OnFreshCancel();
					return;
				}
			}
		}
	}
	else
	{
		if (!m_pBuyItem || !m_pNumber)
		{
			SetTradeResultCode(0, 0x7d, NULL);
			canBuy = false;
		}
		else
		{
			memcpy(&m_buyItem, m_pBuyItem, sizeof(m_buyItem));
			std::list<sTradeItem>* list = Doc()->GetListTradeItem();
			std::list<sTradeItem>::iterator it;
			for (it = list->begin(); it != list->end(); ++it)
				if ((*it).iIndex == m_buyItem.iIndex)
					break;
			if (it == list->end())
			{
				SetTradeResultCode(13, 0xe1, NULL);
				canBuy = false;
			}
			else
			{
				m_buyItem.iNum = atoi(m_pNumber->GetLine(1, false));
				if (m_buyItem.iNum >= m_pBuyItem->iNum)
					m_buyItem.iNum = m_pBuyItem->iNum;
				if (m_buyItem.iNum <= 1)
					m_buyItem.iNum = 1;
				if (m_buyItem.i64Price * m_buyItem.iNum >
					Doc()->m_myInfo.stat.i64Pang)
				{
					SetTradeResultCode(21, 0xe1, NULL);
					canBuy = false;
				}
				if (!Doc()->ConfirmInsertItem(m_buyItem.dwTid, m_buyItem.iNum))
				{
					FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify");
					WRect rect = form->GetRect();
					form->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
					form->SetTimeLimit(12.f);
					form->SetMessage(
						"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						false);
					form->Open(NULL, 0);
					OnFreshCancel();
					return;
				}
			}
		}
	}
	if (canBuy)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel");
		form->SetMessage(
			"\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		form->Open((FRESH_PFN_RESULT)&FrTradeBuyDlg::OnConfirmResult,
			FrESCAPE | FrENTER);
	}
	else
	{
		OnFreshCancel();
	}
}

bool CheckTradeItem(unsigned long uid, const sTradeItem& item)
{
	if (Doc()->GetStateTrade() == STATE_TRADE_ENTER && uid &&
		uid != 0xffffffff && item.dwGuid && item.dwTid && item.i64Price)
	{
		if (item.iNum <= 0)
			return false;
		return true;
	}
	return false;
}

bool FrTradeBuyDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == FrOK && form)
	{
		if (GetPackageSale() == eTypeSalesTrade_Package)
		{
			WSendPacket packet((enumClientPacket)0x11a);
			packet.Encode4(Doc()->GetTradeUID());
			unsigned char count = 0;
			int mark = packet.GetMark();
			packet.Encode1(count);
			std::list<sTradeItem>* list = Doc()->GetListTradeItem();
			if (list)
			{
				std::list<sTradeItem>::iterator it = list->begin();
				std::list<sTradeItem>& items = *Doc()->GetListTradeItem();
				std::list<sTradeItem>::iterator end = items.end();
				for (; it != end; ++it)
				{
					if (!CheckTradeItem(Doc()->GetTradeUID(), *it))
					{
						// HACK!
						do
						{
							SetTradeResultCode(0, 0x7d, NULL);
						} while (false);
						SetClosing(true);
						OnFreshCancel();
						break;
					}
					packet.EncodeBuffer(&*it, sizeof(sTradeItem));
					++count;
				}
			}
			packet.EncodeData(mark, (char*)&count, 1);
			packet.Send(TO_GAME);
		}
		else
		{
			if (!Doc()->SendTradeBuyItem(Doc()->GetTradeUID(), m_buyItem))
			{
				SetTradeResultCode(0, 0x7d, NULL);
				SetClosing(true);
				OnFreshCancel();
			}
			else
			{
				SetClosing(true);
				OnFreshOkay();
			}
		}
	}
	return true;
}

void FrTradeBuyDlg::OnBuyItemIconInit(int param)
{
	m_pBuyItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeBuyDlg::OnBuyItemIconOwnerDraw()
{
	if (!m_pBuyItem)
		return;
	WPoint pos(m_pBuyItemIcon->GetRect().x, m_pBuyItemIcon->GetRect().y);
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (!m_pBuyItem->dwTid)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(m_pBuyItem->dwTid);
	if (!common)
		return;
	float dx = 0.f, dy = 0.f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if ((m_pBuyItem->dwTid >> 26) == 2)
	{
		dx = -12.f;
		dy = -20.f;
	}
	if (IsLocalContent(S4_UCC) && (m_pBuyItem->status & 1) &&
		IsUccClothes(m_pBuyItem->dwTid))
		bitmap = GetUccItemIcon(m_pBuyItem->dwGuid);
	if (bitmap)
	{
		WRect itemRect(pos.x + dx + 14.f, pos.y + dy + 11.f,
			(float)bitmap->Width(), (float)bitmap->Height());
		gi->DrawTexture(bitmap, itemRect, 0xffffffff, 0);
		gi->SetTextColor(0xff000000, 0xffffffff);
		gi->SetTextStyle(4);
		if (IsLocalContent(S4_UCC) && IsUccClothes(m_pBuyItem->dwTid))
		{
			if ((m_pBuyItem->status & 1) && m_pBuyItem->Seq == 1 &&
				m_pUccOriginal)
			{
				gi->DrawTexture(m_pUccOriginal,
					WRect(pos.x + 51.f, pos.y + 43.f,
						(float)m_pUccOriginal->Width(),
						(float)m_pUccOriginal->Height()),
					0xffffffff, 0);
			}
			else if ((m_pBuyItem->status & 1) && m_pBuyItem->Seq > 1 &&
				m_pUccCopied)
			{
				gi->DrawTexture(m_pUccCopied,
					WRect(pos.x + 51.f, pos.y + 43.f,
						(float)m_pUccCopied->Width(),
						(float)m_pUccCopied->Height()),
					0xffffffff, 0);
			}
			else if ((m_pBuyItem->status & 2) && m_pUccTempSaved)
			{
				gi->DrawTexture(m_pUccTempSaved,
					WRect(pos.x + 51.f, pos.y + 43.f,
						(float)m_pUccTempSaved->Width(),
						(float)m_pUccTempSaved->Height()),
					0xffffffff, 0);
			}
		}
		if (m_pBuyItem->iNum > 1)
		{
			unsigned int i = 0;
			char* count = MakeStr("%d", m_pBuyItem->iNum);
			float x = pos.x + 54.f;
			for (i = 0; i < strlen(count); ++i)
				x -= m_pNumFont->GetCharWidthA(g_view, count[i] - '0');
			m_pNumFont->Print(g_view, x, pos.y + 42.f, count, 0, 0xffffffff,
				NULL);
		}
	}
	switch (m_pBuyItem->dwTid >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(pos.x + 38.f - float2int((float)level->Width() * 0.5f),
					pos.y + 35.f - float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

void FrTradeBuyDlg::OnBuyItemNameInit(int param)
{
	m_pBuyItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeBuyDlg::OnBuyItemAbilityInit(int param)
{
	m_pBuyItemAbility = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeBuyDlg::OnMyPangInit(int param)
{
	m_pMyPang = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pMyPang)
	{
		char price[32] = { 0 };
		MakePriceStrA(price, Doc()->m_myInfo.stat.i64Pang);
		m_pMyPang->SetLine(1, price, 0, false, 0);
	}
}

void FrTradeBuyDlg::OnPriceInit(int param)
{
	m_pPrice = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeBuyDlg::OnPangLeftInit(int param)
{
	m_pPangLeft = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeBuyDlg::OnNumberInit(int param)
{
	m_pNumber = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeBuyDlg::OnNumberNextBnInit(int param)
{
	m_pNumberNextBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumberNextBn)
		m_pNumberNextBn->SetPushDelay(0.f);
}

void FrTradeBuyDlg::OnNumberNextBnUp()
{
	if (!m_pNumberNextBn || !m_pNumber || !m_pBuyItem)
		return;
	int count = atoi(m_pNumber->GetLine(1, false));
	++count;
	if (count >= m_pBuyItem->iNum)
	{
		count = m_pBuyItem->iNum;
		m_pNumberNextBn->Enable(false);
	}
	m_pNumberPrevBn->Enable(true);
	m_pNumber->ClearLine();
	m_pNumber->SetLine(1, MakeStr("%d", count), 0, false, 0);
	UpdatePrice();
}

void FrTradeBuyDlg::OnNumberPrevBnInit(int param)
{
	m_pNumberPrevBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNumberPrevBn)
		m_pNumberPrevBn->SetPushDelay(0.f);
}

void FrTradeBuyDlg::OnNumberPrevBnUp()
{
	if (!m_pNumberPrevBn || !m_pNumber || !m_pBuyItem)
		return;
	int count = atoi(m_pNumber->GetLine(1, false));
	--count;
	if (count <= 1)
	{
		count = 1;
		m_pNumberPrevBn->Enable(false);
	}
	m_pNumberNextBn->Enable(true);
	m_pNumber->ClearLine();
	m_pNumber->SetLine(1, MakeStr("%d", count), 0, false, 0);
	UpdatePrice();
}

void FrTradeBuyDlg::OnPurchaseListInit(int param)
{
	m_pPurchaseList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrTradeBuyDlg::OnPurchaseListOwnerdraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	if (!item)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	if (!common)
		return;
	gi->SetTextColor(0xff000000, 0xffffffff);
	if ((item->dwTid >> 26) == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* characterBitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (characterBitmap)
				gi->DrawTexture(characterBitmap,
					WRect(row->pos.x + 3.f, row->pos.y + 3.f,
						(float)characterBitmap->Width(),
						(float)characterBitmap->Height()),
					0xffffffff, 0);
		}
	}
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (bitmap)
	{
		float x, y;
		switch (item->dwTid >> 26)
		{
		case 2:
			x = 7.f;
			y = 2.f;
			break;
		default:
			x = 16.f;
			y = 27.f;
			break;
		}
		if (IsLocalContent(S4_UCC) && (item->status & 1) &&
			IsUccClothes(item->dwTid))
			bitmap = GetUccItemIcon(item->dwGuid);
		if (bitmap)
			gi->DrawTexture(bitmap,
				WRect(row->pos.x + x, row->pos.y + y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
	{
		if ((item->status & 1) && item->Seq == 1 && m_pUccOriginal)
		{
			gi->DrawTexture(m_pUccOriginal,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccOriginal->Width(),
					(float)m_pUccOriginal->Height()),
				0xffffffff, 0);
		}
		else if ((item->status & 1) && item->Seq > 1 && m_pUccCopied)
		{
			gi->DrawTexture(m_pUccCopied,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccCopied->Width(),
					(float)m_pUccCopied->Height()),
				0xffffffff, 0);
		}
		else if ((item->status & 2) && m_pUccTempSaved)
		{
			gi->DrawTexture(m_pUccTempSaved,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccTempSaved->Width(),
					(float)m_pUccTempSaved->Height()),
				0xffffffff, 0);
		}
	}
	const char* name;
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
		name = GetUccItemName(item->dwGuid);
	else
		name = common->Name;
	gi->SetTextStyle(1);
	gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 7.f), 2, name);
	gi->SetTextStyle(0);
	char price[32] = { 0 };
	MakePriceStrA(price, item->i64Price);
	SetTextColorByPrice(item->i64Price);
	gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 23.f), 2,
		MakeStr("%s\xc6\xce", price));
	gi->SetTextColor(0xff000000, 0xffffffff);
	DrawAbility(*item, row);
	switch (item->dwTid >> 26)
	{
	case 2:
	case 4:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(row->pos.x + 40.f -
						float2int((float)level->Width() * 0.5f),
					row->pos.y + 51.f -
						float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

bool FrTradeBuyDlg::OnInit()
{
	EnableDrag(false);
	return true;
}

void FrTradeBuyDlg::OnProc(const float delta)
{
	if (!m_pBuyItem || !m_pNumberNextBn || !m_pNumberPrevBn)
		return;
	int count = atoi(m_pNumber->GetLine(1, false));
	if (count <= 1)
	{
		m_pNumberNextBn->Enable(true);
		m_pNumberPrevBn->Enable(false);
		m_pNumber->ClearLine();
		m_pNumber->SetLine(1, MakeStr("%d", 1), 0, false, 0);
	}
	else if (count >= m_pBuyItem->iNum)
	{
		count = m_pBuyItem->iNum;
		m_pNumberNextBn->Enable(false);
		m_pNumberPrevBn->Enable(true);
		m_pNumber->ClearLine();
		m_pNumber->SetLine(1, MakeStr("%d", count), 0, false, 0);
	}
	else
	{
		m_pNumberNextBn->Enable(true);
		m_pNumberPrevBn->Enable(true);
	}
	UpdatePrice();
}

void FrTradeBuyDlg::SetBuyItem(sTradeItem* item)
{
	m_pBuyItemName->ClearLine();
	m_pBuyItemAbility->ClearLine();
	m_pBuyItem = item;
	if (!item)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	if (!common || !m_pBuyItemName)
		return;
	if (IsLocalContent(S4_UCC) && (item->status & 1) &&
		IsUccClothes(item->dwTid))
	{
		const char* name = GetUccItemName(item->dwGuid);
		m_pBuyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", name), false, true);
	}
	else
		m_pBuyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", common->Name),
			false, true);
	short ability[5];
	short slots[5] = { 0 };
	unsigned long type = item->dwTid >> 26;
	switch (type)
	{
	case 2:
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
		if (part == NULL)
			return;
		memcpy(ability, part->Attr, sizeof(ability));
		memcpy(slots, part->Slot, sizeof(slots));
	}
	break;
	case 7:
	{
		IFF_STRUCT::sCaddie* caddie = ItemManager()->FindCaddie(item->dwTid);
		if (caddie == NULL)
			return;
		memcpy(ability, caddie->Attr, sizeof(ability));
	}
	break;
	case 8:
	{
		IFF_STRUCT::sCadItem* caddie = ItemManager()->FindCadItem(item->dwTid);
		if (caddie == NULL)
			return;
		memcpy(ability, caddie->COM, sizeof(ability));
	}
	break;
	default:
		return;
	}
	const char* statName = "";
	int i = 0;
	for (int remaining = 5; remaining; --remaining, ++i)
	{
		if (!ability[i] && !slots[i])
			continue;
		switch (i)
		{
		case 0:
			statName = "\xc6\xc4\xbf\xf6";
			break;
		case 1:
			statName = "\xc4\xc1\xc6\xae\xb7\xd1";
			break;
		case 2:
			statName = "\xc1\xa4\xc8\xae\xb5\xb5";
			break;
		case 3:
			statName = "\xbd\xba\xc7\xc9";
			break;
		case 4:
			statName = "\xc4\xbf\xba\xea";
			break;
		}
		if (ability[i] > 0 || slots[i] > 0)
		{
			if (ability[i] == 0)
				m_pBuyItemAbility->AddText(
					MakeStr(
						"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
						statName, slots[i]),
					false, true);
			else if (slots[i] == 0)
				m_pBuyItemAbility->AddText(
					MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName,
						ability[i]),
					false, true);
			else
				m_pBuyItemAbility->AddText(
					MakeStr(
						"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
						statName, ability[i], slots[i]),
					false, true);
		}
	}
}

void FrTradeBuyDlg::SetBuyItemList()
{
	const std::list<sTradeItem>* list = Doc()->GetListTradeItem();
	for (std::list<sTradeItem>::const_iterator it = list->begin();
		it != list->end(); ++it)
		m_pPurchaseList->AddItem((void*)&*it);
}

void FrTradeBuyDlg::SetControlsItemList()
{
	m_pNumber->SetLine(1, "1", 0, false, 0);
	UpdatePriceList();
	m_pNumberNextBn->Enable(false);
	m_pNumberPrevBn->Enable(false);
	m_pNumber->Enable(false);
}

void FrTradeBuyDlg::UpdatePriceList()
{
	char price[32] = { 0 };
	char left[32] = { 0 };
	std::list<sTradeItem>* list = Doc()->GetListTradeItem();
	__int64 sum = std::accumulate(list->begin(), list->end(), (__int64)0,
		SumForTradeItemList);
	MakePriceStrA(price, sum);
	m_pPrice->SetLine(1, price, 0, false, 0);
	MakePriceStrA(left, Doc()->m_myInfo.stat.i64Pang - sum);
	m_pPangLeft->SetLine(1, left, 0, false, 0);
}

void FrTradeBuyDlg::SetControlsBuyItem(sTradeItem* item)
{
	if (!m_pPrice || !m_pPangLeft || !m_pNumberNextBn || !m_pNumberPrevBn ||
		!m_pNumber)
		return;
	m_pPrice->ClearLine();
	m_pPangLeft->ClearLine();
	m_pNumber->ClearLine();
	if (!item)
	{
		m_pNumberNextBn->Enable(false);
		m_pNumberPrevBn->Enable(false);
		m_pNumber->Enable(false);
		return;
	}
	m_pNumber->SetLine(1, "1", 0, false, 0);
	UpdatePrice();
	if (item->iNum == 1)
	{
		m_pNumberNextBn->Enable(false);
		m_pNumberPrevBn->Enable(false);
		m_pNumber->Enable(false);
	}
	else
	{
		m_pNumberNextBn->Enable(true);
		m_pNumberPrevBn->Enable(true);
		m_pNumber->Enable(true);
	}
}

void FrTradeBuyDlg::UpdatePrice()
{
	if (!m_pPrice || !m_pNumber || !m_pBuyItem)
		return;
	char price[32] = { 0 };
	char left[32] = { 0 };
	int count = atoi(m_pNumber->GetLine(1, false));
	if (m_prevNumber != count)
	{
		MakePriceStrA(price, m_pBuyItem->i64Price * count);
		m_pPrice->SetLine(1, price, 0, false, 0);
		MakePriceStrA(left,
			Doc()->m_myInfo.stat.i64Pang - m_pBuyItem->i64Price * count);
		m_pPangLeft->SetLine(1, left, 0, false, 0);
		m_prevNumber = count;
	}
}

IMPLEMENT_OBJECT(FrTradeResultDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeResultDlg, FrForm)
ON_FRESH_VI("traderesultdlg", FRCMD_INIT,
	FrTradeResultDlg::OnTradeResultDlgInit)
ON_FRESH_VI("resultitem_icon", FRCMD_INIT,
	FrTradeResultDlg::OnResultItemIconInit)
ON_FRESH_VV("resultitem_icon", FRCMD_OWNERDRAW,
	FrTradeResultDlg::OnResultItemIconOwnerDraw)
ON_FRESH_VI("resultitem_name", FRCMD_INIT,
	FrTradeResultDlg::OnResultItemNameInit)
ON_FRESH_VI("resultitem_ability", FRCMD_INIT,
	FrTradeResultDlg::OnResultItemAbilityInit)
ON_FRESH_VI("purchase_list", FRCMD_INIT, FrTradeResultDlg::OnPurchaseListInit)
ON_FRESH_VI("purchase_list", FRCMD_OWNERDRAW,
	FrTradeResultDlg::OnPurchaseListOwnerdraw)
END_FRESH_MSGMAP()

FrTradeResultDlg::FrTradeResultDlg()
{
	m_pResultItemIcon = NULL;
	m_pResultItemName = NULL;
	m_pResultItemAbility = NULL;
	memset(&m_resultItem, 0, sizeof(m_resultItem));
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pNumFont = g_resrcmng->GetTitleFont();
	m_pNumFont->Create(&font);
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	m_pPurchaseList = NULL;
}

FrTradeResultDlg::~FrTradeResultDlg()
{
	if (m_pNumFont)
		m_pNumFont->Erase();
	if (g_resrcmng && m_pNumFont)
	{
		g_resrcmng->Release(m_pNumFont);
		m_pNumFont = NULL;
	}
}

void FrTradeResultDlg::OnTradeResultDlgInit(int param)
{
}

void FrTradeResultDlg::OnResultItemIconInit(int param)
{
	m_pResultItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeResultDlg::OnResultItemIconOwnerDraw()
{
	if (!m_resultItem.dwTid)
		return;
	if (!m_pResultItemIcon)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(m_resultItem.dwTid);
	if (!common)
		return;
	float dx = 0.f, dy = 0.f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if ((m_resultItem.dwTid >> 26) == 2)
	{
		dx = -12.f;
		dy = -20.f;
	}
	if (IsLocalContent(S4_UCC) && (m_resultItem.status & 1) &&
		IsUccClothes(m_resultItem.dwTid))
		bitmap = GetUccItemIcon(m_resultItem.dwGuid);
	if (!bitmap)
		return;
	WPoint pos(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y);
	WRect itemRect(pos.x + dx + 14.f, pos.y + dy + 11.f, (float)bitmap->Width(),
		(float)bitmap->Height());
	gi->DrawTexture(bitmap, itemRect, 0xffffffff, 0);
	gi->SetTextColor(0xff000000, 0xffffffff);
	gi->SetTextStyle(4);
	if (IsLocalContent(S4_UCC) && IsUccClothes(m_resultItem.dwTid))
	{
		if ((m_resultItem.status & 1) && m_resultItem.Seq == 1 &&
			m_pUccOriginal)
		{
			WPoint markerPos(m_pResultItemIcon->GetRect().x,
				m_pResultItemIcon->GetRect().y);
			gi->DrawTexture(m_pUccOriginal,
				WRect(markerPos.x + 51.f, markerPos.y + 43.f,
					(float)m_pUccOriginal->Width(),
					(float)m_pUccOriginal->Height()),
				0xffffffff, 0);
		}
		else if ((m_resultItem.status & 1) && m_resultItem.Seq > 1 &&
			m_pUccCopied)
		{
			WPoint markerPos(m_pResultItemIcon->GetRect().x,
				m_pResultItemIcon->GetRect().y);
			gi->DrawTexture(m_pUccCopied,
				WRect(markerPos.x + 51.f, markerPos.y + 43.f,
					(float)m_pUccCopied->Width(),
					(float)m_pUccCopied->Height()),
				0xffffffff, 0);
		}
		else if ((m_resultItem.status & 2) && m_pUccTempSaved)
		{
			WPoint markerPos(m_pResultItemIcon->GetRect().x,
				m_pResultItemIcon->GetRect().y);
			gi->DrawTexture(m_pUccTempSaved,
				WRect(markerPos.x + 51.f, markerPos.y + 43.f,
					(float)m_pUccTempSaved->Width(),
					(float)m_pUccTempSaved->Height()),
				0xffffffff, 0);
		}
	}
	if (m_resultItem.iNum > 1)
	{
		unsigned int i = 0;
		char* count = MakeStr("%d", m_resultItem.iNum);
		float x = pos.x + 54.f;
		for (i = 0; i < strlen(count); ++i)
			x -= m_pNumFont->GetCharWidthA(g_view, count[i] - '0');
		m_pNumFont->Print(g_view, x, pos.y + 42.f, count, 0, 0xffffffff, NULL);
	}
	switch (m_resultItem.dwTid >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(pos.x + 38.f - float2int((float)level->Width() * 0.5f),
					pos.y + 35.f - float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

void FrTradeResultDlg::OnResultItemNameInit(int param)
{
	m_pResultItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeResultDlg::OnResultItemAbilityInit(int param)
{
	m_pResultItemAbility = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeResultDlg::OnPurchaseListInit(int param)
{
	m_pPurchaseList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrTradeResultDlg::OnPurchaseListOwnerdraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (!row->pData)
		return;
	gi->SetTextColor(0xff000000, 0xffffffff);
	sTradeItem* item = (sTradeItem*)row->pData;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	if ((item->dwTid >> 26) == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* characterBitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (characterBitmap)
				gi->DrawTexture(characterBitmap,
					WRect(row->pos.x + 3.f, row->pos.y + 3.f,
						(float)characterBitmap->Width(),
						(float)characterBitmap->Height()),
					0xffffffff, 0);
		}
	}
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (bitmap)
	{
		float x, y;
		switch (item->dwTid >> 26)
		{
		case 2:
			x = 7.f;
			y = 2.f;
			break;
		default:
			x = 16.f;
			y = 27.f;
			break;
		}
		if (IsLocalContent(S4_UCC) && (item->status & 1) &&
			IsUccClothes(item->dwTid))
			bitmap = GetUccItemIcon(item->dwGuid);
		if (bitmap)
			gi->DrawTexture(bitmap,
				WRect(row->pos.x + x, row->pos.y + y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
	}
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
	{
		if ((item->status & 1) && item->Seq == 1 && m_pUccOriginal)
		{
			gi->DrawTexture(m_pUccOriginal,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccOriginal->Width(),
					(float)m_pUccOriginal->Height()),
				0xffffffff, 0);
		}
		else if ((item->status & 1) && item->Seq > 1 && m_pUccCopied)
		{
			gi->DrawTexture(m_pUccCopied,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccCopied->Width(),
					(float)m_pUccCopied->Height()),
				0xffffffff, 0);
		}
		else if ((item->status & 2) && m_pUccTempSaved)
		{
			gi->DrawTexture(m_pUccTempSaved,
				WRect(row->pos.x + 50.f, row->pos.y + 52.f,
					(float)m_pUccTempSaved->Width(),
					(float)m_pUccTempSaved->Height()),
				0xffffffff, 0);
		}
	}
	const char* name;
	if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
		name = GetUccItemName(item->dwGuid);
	else
		name = common->Name;
	gi->SetTextStyle(1);
	gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 7.f), 2, name);
	gi->SetTextStyle(0);
	char price[32] = { 0 };
	MakePriceStrA(price, item->i64Price);
	SetTextColorByPrice(item->i64Price);
	gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 23.f), 2,
		MakeStr("%s\xc6\xce", price));
	gi->SetTextColor(0xff000000, 0xffffffff);
	DrawAbility(*item, row);
	switch (item->dwTid >> 26)
	{
	case 2:
	case 4:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(row->pos.x + 40.f -
						float2int((float)level->Width() * 0.5f),
					row->pos.y + 51.f -
						float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

bool FrTradeResultDlg::OnInit()
{
	EnableDrag(false);
	return true;
}

class CListAssignHelper
{
	FrListBox* m_pList;

public:
	CListAssignHelper(FrListBox* list)
		: m_pList(list)
	{
	}
	template <class T>
	void operator()(T& item)
	{
		m_pList->AddItem(&item);
	}
};

void FrTradeResultDlg::SetResultItemList(std::vector<sTradeItem>* list)
{
	m_resultItemList.swap(*list);
	std::for_each(m_resultItemList.begin(), m_resultItemList.end(),
		CListAssignHelper(m_pPurchaseList));
}

void FrTradeResultDlg::SetResultItem(sTradeItem* item)
{
	m_pResultItemName->ClearLine();
	m_pResultItemAbility->ClearLine();
	if (item)
	{
		memcpy(&m_resultItem, item, sizeof(m_resultItem));
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
		if (common && m_pResultItemName)
		{
			if (IsLocalContent(S4_UCC) && (item->status & 1) &&
				IsUccClothes(item->dwTid))
			{
				const char* name = GetUccItemName(item->dwGuid);
				m_pResultItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", name),
					false, true);
			}
			else
				m_pResultItemName->AddText(
					MakeStr("\\cT\\c%s\\cN\\c", common->Name), false, true);
			short ability[5];
			short slots[5] = { 0 };
			unsigned long type = item->dwTid >> 26;
			switch (type)
			{
			case 2:
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
				if (!part)
					return;
				memcpy(ability, part->Attr, sizeof(ability));
				memcpy(slots, part->Slot, sizeof(slots));
			}
			break;
			case 7:
			{
				IFF_STRUCT::sCaddie* caddie =
					ItemManager()->FindCaddie(item->dwTid);
				if (!caddie)
					return;
				memcpy(ability, caddie->Attr, sizeof(ability));
			}
			break;
			case 8:
			{
				IFF_STRUCT::sCadItem* caddie =
					ItemManager()->FindCadItem(item->dwTid);
				if (!caddie)
					return;
				memcpy(ability, caddie->COM, sizeof(ability));
			}
			break;
			default:
				return;
			}
			const char* statName = "";
			int i = 0;
			for (int remaining = 5; remaining; --remaining, ++i)
			{
				if (ability[i] || slots[i])
				{
					switch (i)
					{
					case 0:
						statName = "\xc6\xc4\xbf\xf6";
						break;
					case 1:
						statName = "\xc4\xc1\xc6\xae\xb7\xd1";
						break;
					case 2:
						statName = "\xc1\xa4\xc8\xae\xb5\xb5";
						break;
					case 3:
						statName = "\xbd\xba\xc7\xc9";
						break;
					case 4:
						statName = "\xc4\xbf\xba\xea";
						break;
					}
					if (ability[i] > 0 || slots[i] > 0)
					{
						if (ability[i] == 0)
							m_pResultItemAbility->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									statName, slots[i]),
								false, true);
						else if (slots[i] == 0)
							m_pResultItemAbility->AddText(
								MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
									statName, ability[i]),
								false, true);
						else
							m_pResultItemAbility->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									statName, ability[i], slots[i]),
								false, true);
					}
				}
			}
		}
	}
	else
		memset(&m_resultItem, 0, sizeof(m_resultItem));
}

template <class T>
struct _TradeItemSort
{
	bool operator()(const T& a, const T& b) const
	{
		if ((a.dwTid >> 26) == 2 && (b.dwTid >> 26) != 2)
			return false;
		if ((a.dwTid >> 26) == 2 && (b.dwTid >> 26) == 2)
		{
			int aPart = a.dwTid & 0x03ffffff;
			int bPart = b.dwTid & 0x03ffffff;
			if (aPart < bPart)
				return true;
			if (aPart > bPart)
				return false;
		}
		if (a.dwTid < b.dwTid)
			return true;
		if (a.dwTid > b.dwTid)
			return false;
		return a.dwGuid < b.dwGuid;
	}
};

IMPLEMENT_OBJECT(FrTradeEditDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeEditDlg, FrForm)
ON_FRESH_VI("tradeeditdlg", FRCMD_INIT, FrTradeEditDlg::OnTradeEditDlgInit)
ON_FRESH_VI("my_item_list", FRCMD_INIT, FrTradeEditDlg::OnMyItemListInit)
ON_FRESH_VV("my_item_list", FRCMD_LBUTTONDOWN,
	FrTradeEditDlg::OnMyItemListLClick)
ON_FRESH_VV("my_item_list", FRCMD_RBUTTONDOWN,
	FrTradeEditDlg::OnMyItemListRClick)
ON_FRESH_VI("my_item_list", FRCMD_OWNERDRAW,
	FrTradeEditDlg::OnMyItemListOwnerDraw)
ON_FRESH_VI("trade_item_list", FRCMD_INIT, FrTradeEditDlg::OnTradeItemListInit)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONDOWN,
	FrTradeEditDlg::OnTradeItemListLClick)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	FrTradeEditDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("visitor", FRCMD_INIT, FrTradeEditDlg::OnVisitorInit)
ON_FRESH_VI("userid", FRCMD_INIT, FrTradeEditDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, FrTradeEditDlg::OnTitleInit)
ON_FRESH_VI("trade_title_edit", FRCMD_INIT, FrTradeEditDlg::OnTitleEditInit)
ON_FRESH_BI("trade_title_edit", FRCMD_ENTERKEY,
	FrTradeEditDlg::OnTitleEditEnterKey)
ON_FRESH_VI("trade_title_edit_bn", FRCMD_INIT,
	FrTradeEditDlg::OnTitleEditBnInit)
ON_FRESH_VV("trade_title_edit_bn", FRCMD_LBUTTONUP,
	FrTradeEditDlg::OnTitleEditBnUp)
ON_FRESH_VI("trade_myitem_icon", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemIconInit)
ON_FRESH_VV("trade_myitem_icon", FRCMD_OWNERDRAW,
	FrTradeEditDlg::OnTradeMyItemIconOwnerDraw)
ON_FRESH_VI("trade_myitem_name", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemNameInit)
ON_FRESH_VI("trade_myitem_ability", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemAbilityInit)
ON_FRESH_VI("trade_myitem_number", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemNumberInit)
ON_FRESH_VI("trade_myitem_price", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemPriceInit)
ON_FRESH_VV("trade_myitem_price", FRCMD_LBUTTONUP,
	FrTradeEditDlg::OnTradeMyItemPriceClick)
ON_FRESH_VI("trade_myitem_ok_bn", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemOkBnInit)
ON_FRESH_VV("trade_myitem_ok_bn", FRCMD_LBUTTONUP,
	FrTradeEditDlg::OnTradeMyItemOkBnUp)
ON_FRESH_VI("trade_myitem_cancel_bn", FRCMD_INIT,
	FrTradeEditDlg::OnTradeMyItemCancelBnInit)
ON_FRESH_VV("trade_myitem_cancel_bn", FRCMD_LBUTTONUP,
	FrTradeEditDlg::OnTradeMyItemCancelBnUp)
ON_FRESH_VI("open", FRCMD_INIT, FrTradeEditDlg::OnOpenBnInit)
ON_FRESH_VV("open", FRCMD_LBUTTONUP, FrTradeEditDlg::OnOpenBnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrTradeEditDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrTradeEditDlg::OnCloseBnUp)
ON_FRESH_VI("exit", FRCMD_INIT, FrTradeEditDlg::OnExitBnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, FrTradeEditDlg::OnExitBnUp)
ON_FRESH_VI("package_check", FRCMD_INIT, FrTradeEditDlg::OnPackageSaleInit)
ON_FRESH_VV("package_check", FRCMD_LBUTTONUP,
	FrTradeEditDlg::OnPackageSaleButtonUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, FrTradeEditDlg::OnTooltipAreaInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, FrTradeEditDlg::OnTooltipAreaOwnerDraw)
END_FRESH_MSGMAP()

FrTradeEditDlg::FrTradeEditDlg()
{
	m_pTradeItemList = NULL;
	m_pMyItemList = NULL;
	m_pVisitor = NULL;
	m_pUserId = NULL;
	m_pTitle = NULL;
	m_pTitleEdit = NULL;
	m_pTradeMyItemIcon = NULL;
	m_pTradeMyItemName = NULL;
	m_pTradeMyItemAbility = NULL;
	m_pTradeMyItemNumber = NULL;
	m_pTradeMyItemPrice = NULL;
	m_pTradeMyItemOkBn = NULL;
	m_pTradeMyItemCancelBn = NULL;
	m_pOpenBn = NULL;
	m_pCloseBn = NULL;
	m_pExitBn = NULL;
	m_pPackageSale = NULL;
	m_pTooltipArea = NULL;
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pNumFont = g_resrcmng->GetTitleFont();
	m_pNumFont->Create(&font);
	static char* pangFiles[] = { "[font_bong.jpg" };
	WTITLEFONT pangFont;
	memset(&pangFont, 0, sizeof(pangFont));
	pangFont.filename = pangFiles;
	pangFont.fontw = 9;
	pangFont.fonth = 16;
	pangFont.numPages = 1;
	pangFont.texw = 128;
	pangFont.texh = 32;
	pangFont.pCharSet = "1234567890,-";
	m_pPangFont = g_resrcmng->GetTitleFont();
	m_pPangFont->Create(&pangFont);
	m_pMyItemBaseClick = g_pFresh->GetBitmap("trade_myitem_base_click");
	m_pMyItemBaseOver = g_pFresh->GetBitmap("trade_myitem_base_over");
	m_pTradeItemBaseNormal = g_pFresh->GetBitmap("trade_item_base_normal");
	m_pTradeItemBaseOver = g_pFresh->GetBitmap("trade_item_base_over");
	m_pTradeItemBaseClick = g_pFresh->GetBitmap("trade_item_base_click");
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	memset(m_reserved190, 0, sizeof(m_reserved190));
	memset(m_title, 0, sizeof(m_title));
	m_myItemList.clear();
	m_tradeItemList.clear();
	m_pSelectItem = NULL;
	m_deleteBtn[0].pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_normal");
	m_deleteBtn[0].rect = WRect(120.f, 30.f, m_deleteBtn[0].pBitmap->Width(),
		m_deleteBtn[0].pBitmap->Height());
	m_deleteBtn[1].pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_over");
	m_deleteBtn[1].rect = WRect(120.f, 30.f, m_deleteBtn[1].pBitmap->Width(),
		m_deleteBtn[1].pBitmap->Height());
	m_editBtn[0].pBitmap = g_pFresh->GetBitmap("trade_item_edit_bn_normal");
	m_editBtn[0].rect = WRect(80.f, 30.f, m_editBtn[0].pBitmap->Width(),
		m_editBtn[0].pBitmap->Height());
	m_editBtn[1].pBitmap = g_pFresh->GetBitmap("trade_item_edit_bn_over");
	m_editBtn[1].rect = WRect(80.f, 30.f, m_editBtn[1].pBitmap->Width(),
		m_editBtn[1].pBitmap->Height());
	m_clock.pBitmap = g_pFresh->GetBitmap("clock");
	m_clock.rect =
		WRect(50.f, 35.f, m_clock.pBitmap->Width(), m_clock.pBitmap->Height());
	m_deleteMark.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteMark.rect = WRect(50.f, 35.f, m_deleteMark.pBitmap->Width(),
		m_deleteMark.pBitmap->Height());
	m_packageSale = eTypeSalesTrade_None;
}

FrTradeEditDlg::~FrTradeEditDlg()
{
	if (m_pPangFont)
		m_pPangFont->Erase();
	if (m_pNumFont)
		m_pNumFont->Erase();
	if (g_resrcmng && m_pPangFont)
	{
		g_resrcmng->Release(m_pPangFont);
		m_pPangFont = NULL;
	}
	if (g_resrcmng && m_pNumFont)
	{
		g_resrcmng->Release(m_pNumFont);
		m_pNumFont = NULL;
	}
}

void FrTradeEditDlg::OnTradeEditDlgInit(int param)
{
}

void FrTradeEditDlg::OnMyItemListInit(int param)
{
	m_pMyItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pMyItemList)
		m_pMyItemList->UseRightButton(true);
}

void FrTradeEditDlg::OnMyItemListLClick()
{
	if (m_tradeItemList.size() < 6)
	{
		FrListItem* row = m_pMyItemList->GetItemUnderCursor();
		if (row)
		{
			if (!row->selected)
			{
				m_pMyItemList->ToggleItem(row);
				return;
			}
			sTradeItem* item = (sTradeItem*)row->pData;
			SetTradeMyItem(item);
			SetControlsTradeMyItem(item, false);
		}
	}
}

void FrTradeEditDlg::OnMyItemListRClick()
{
	if (m_tradeItemList.size() >= 6)
		return;
	FrListItem* row = m_pMyItemList->GetItemUnderCursor();
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	if (item)
	{
		FrItemInfoDlg* dialog =
			CreateForm<FrItemInfoDlg>(g_pFresh->GetManager(), this, "iteminfo");
		if (dialog)
		{
			dialog->Open(NULL, 3);
			dialog->SetItemInfo(item);
			if (IsLocalContent(S4_CARD_SYSTEM))
			{
				FrEdit* edit =
					DYNAMIC_CAST(FrEdit, dialog->FindChildByName("avility"));
				if (edit)
				{
					IFF_STRUCT::sPart* part =
						ItemManager()->FindPart(item->dwTid);
					if (part)
						for (int i = 0; i < 5; ++i)
						{
							if (part->Attr[i] != 0 || part->Slot[i] != 0)
								edit->AddText(
									MakeStr(
										"\\c0xff0000ff\\c%s\\c0xff000000\\c",
										MakeStatusString(i, part->Attr[i],
											part->Slot[i])),
									false, true);
						}
					unsigned long* cardTid = &item->attachCardTid[0][0];
					for (int remaining = 8; remaining; --remaining, ++cardTid)
					{
						unsigned long tid = *cardTid;
						if (tid)
						{
							unsigned long type = (tid >> 22) & 15;
							if (type == 0)
							{
								IFF_STRUCT::sCard* card =
									ItemManager()->FindCard(tid);
								if (card)
									for (int i = 0; i < 5; ++i)
									{
										if (card->COM[i] > 0)
											edit->AddText(
												MakeStr(
													"\\c0xff60ee0e\\c%s\\c0xff000000\\c",
													MakeCardStatusString(i,
														card->COM[i])),
												false, true);
									}
							}
							else if (type == 1)
							{
								IFF_STRUCT::sDesc* desc =
									ItemManager()->FindDesc(tid);
								if (desc)
								{
									char text[512];
									strcpy(text, desc->Desc);
									for (int i = 0; i < 512; ++i)
									{
										if (text[i] == '\n')
										{
											text[i] = 0;
											break;
										}
									}
									edit->AddText(
										MakeStr(
											"\\c0xffff0000\\c%s\\c0xff000000\\c",
											text),
										false, true);
								}
							}
						}
					}
				}
			}
		}
	}
	if (!row->selected)
	{
		m_pMyItemList->ToggleItem(row);
		return;
	}
	SetTradeMyItem(item);
	SetControlsTradeMyItem(item, false);
}

void FrTradeEditDlg::OnMyItemListOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	const Bitmap* bitmap = NULL;
	if (row->selected)
	{
		bitmap = m_pMyItemBaseClick;
		gi->SetTextColor(0xffff0000, 0xffffffff);
	}
	else if (row->underCursor)
	{
		bitmap = m_pMyItemBaseOver;
		gi->SetTextColor(0xff000000, 0xffffffff);
	}
	else
		gi->SetTextColor(0xff000000, 0xffffffff);
	if (bitmap)
		gi->DrawTexture(bitmap,
			WRect(row->pos.x + 2.f, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			0xffffffff, 0);
	if ((item->dwTid >> 26) == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((item->dwTid >> 18) & 255) | 0x04000000);
		if (character)
		{
			const Bitmap* icon =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (icon)
				gi->DrawTexture(icon,
					WRect(row->pos.x + 7.f, row->pos.y + 5.f,
						(float)icon->Width(), (float)icon->Height()),
					0xffffffff, 0);
		}
	}
	WPoint pos = WPoint(13.f, 20.f) + row->pos;
	DrawItem(pos, item, 0);
	if (IsLocalContent(S4_CARD_SYSTEM) && (item->dwTid >> 26) == 2)
		DrawPartsSlot(pos, (sTradeItem*)row->pData);
}

void FrTradeEditDlg::OnTradeItemListInit(int param)
{
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeItemListLClick()
{
	FrListItem* row = m_pTradeItemList->GetItemUnderCursor();
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if ((m_deleteBtn[0].rect + row->pos).IsInRect(mouse))
	{
		m_pTradeItemList->SelectItem(m_pTradeItemList->GetItemUnderCursor(),
			true);
		SetTradeResultCode(20, 0xe1, NULL);
		return;
	}
	bool enable = false;
	for (std::list<sTradeItem>::iterator it = m_myItemList.begin();
		it != m_myItemList.end(); ++it)
	{
		if ((*it).dwGuid == item->dwGuid)
		{
			if ((*it).iNum + item->iNum > 1)
				enable = true;
			break;
		}
	}
	SetTradeMyItem(item);
	SetControlsTradeMyItem(item, enable);
}

void FrTradeEditDlg::OnTradeItemListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	const Bitmap* bitmap = m_pTradeItemBaseNormal;
	if (bitmap)
		gi->DrawTexture(bitmap,
			WRect(row->pos.x + 1.f, row->pos.y + 1.f,
				(float)m_pTradeItemBaseNormal->Width(),
				(float)m_pTradeItemBaseNormal->Height()),
			0xffffffff, 0);
	char price[32] = { 0 };
	MakePriceStrA(price, item->i64Price);
	const char* text;
	if (item->iNum == 1)
		text = MakeStr("%s\xc6\xce", price);
	else
		text = MakeStr("\xb0\xb3\xb4\xe7%s\xc6\xce", price);
	if (IsLocalContent(S4_UCC) && (item->status & 1) &&
		IsUccClothes(item->dwTid))
	{
		const char* name = GetUccItemName(item->dwGuid);
		gi->Print(WPoint(10.f, 13.f) + row->pos, 0, "%s", name);
	}
	else
		gi->Print(WPoint(10.f, 13.f) + row->pos, 0, "%s", common->Name);
	SetTextColorByPrice(item->i64Price);
	gi->Print(WPoint(12.f, 32.f) + row->pos, 0, text);
	gi->SetTextColor(0xff000000, 0xffffffff);
	gi->DrawTexture(m_deleteBtn[0].pBitmap, m_deleteBtn[0].rect + row->pos,
		0xffffffff, 0);
}

void FrTradeEditDlg::OnVisitorInit(int param)
{
	m_pVisitor = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	if (!m_pVisitor)
		return;
	char text[32] = { 0 };
	MakePriceStrA(text, Doc()->GetCountVisitor());
	m_pVisitor->SetLine(1, text, 0, false, 0);
}

void FrTradeEditDlg::OnUserIdInit(int param)
{
	m_pUserId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);

	if (m_pUserId)
		m_pUserId->SetLine(1, "\xb3\xaa\xc0\xc7 \xbb\xf3\xc1\xa1", 0, false, 0);
}

void FrTradeEditDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	UpdateTradeTitle();
}

void FrTradeEditDlg::OnTitleEditInit(int param)
{
	m_pTitleEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrTradeEditDlg::OnTitleEditEnterKey(int param)
{
	eResultTrade result = SendTradeTitle();
	if (result != eResultTrade_Success)
		SetTradeResultCode(result, 0xe1, NULL);
	return true;
}

void FrTradeEditDlg::OnTitleEditBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnTitleEditBnUp()
{
	eResultTrade result = SendTradeTitle();
	if (result != eResultTrade_Success)
		SetTradeResultCode(result, 0xe1, NULL);
}

void FrTradeEditDlg::OnTradeMyItemIconInit(int param)
{
	m_pTradeMyItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemIconOwnerDraw()
{
	if (m_pSelectItem)
	{
		WPoint pos(m_pTradeMyItemIcon->GetRect().x,
			m_pTradeMyItemIcon->GetRect().y);
		DrawItem(pos, m_pSelectItem, 0);
		if (IsLocalContent(S4_CARD_SYSTEM) && (m_pSelectItem->dwTid >> 26) == 2)
			DrawPartsSlot(pos, m_pSelectItem);
	}
	DrawPang();
}

void FrTradeEditDlg::OnTradeMyItemNameInit(int param)
{
	m_pTradeMyItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemAbilityInit(int param)
{
	m_pTradeMyItemAbility = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemNumberInit(int param)
{
	m_pTradeMyItemNumber = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemPriceInit(int param)
{
	m_pTradeMyItemPrice = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pTradeMyItemPrice)
		m_pTradeMyItemPrice->SetCharLimit(8, false);
}

void FrTradeEditDlg::OnTradeMyItemPriceClick()
{
	if (m_pTradeMyItemPrice)
		m_pTradeMyItemPrice->SetKeyFocus(true);
}

void FrTradeEditDlg::OnTradeMyItemOkBnInit(int param)
{
	m_pTradeMyItemOkBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemOkBnUp()
{
	if (!m_pSelectItem)
		return;
	sTradeItem item = *m_pSelectItem;
	int number = atoi(m_pTradeMyItemNumber->GetLine(1, false));
	char price[10] = { 0 };
	const char* text = m_pTradeMyItemPrice->GetLine(1, false);
	int i = 0;
	char* out = price;
	for (; i < 9; ++i)
	{
		char ch = text[i];
		if (ch == 0)
			break;
		if (ch >= '0' && ch <= '9')
			*out++ = ch;
		else if (ch != ',')
		{
			price[0] = 0;
			break;
		}
	}
	__int64 value = atoi(price);
	int available = item.iNum;
	for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin();
		it != m_tradeItemList.end(); ++it)
	{
		if (&*it == m_pSelectItem)
		{
			available = (*it).iNum;
			for (std::list<sTradeItem>::iterator mine = m_myItemList.begin();
				mine != m_myItemList.end(); ++mine)
			{
				if ((*mine).dwGuid == m_pSelectItem->dwGuid)
				{
					available += (*mine).iNum;
					break;
				}
			}
			break;
		}
	}
	if (IsPushedPackageSaleButton() && CheckPackageSaleItem(item))
	{
		SetTradeResultCode(29, 0xe1, NULL);
		return;
	}
	if (number < 1 || number > available)
	{
		SetTradeResultCode(10, 0xe1, NULL);
		return;
	}
	if (value <= 0)
	{
		SetTradeResultCode(11, 0xe1, NULL);
		return;
	}
	if (value > 30000000)
	{
		SetTradeResultCode(12, 0xe1, NULL);
		return;
	}
	item.iNum = number;
	item.i64Price = value;
	DelListMyItem(&item);
	AddListTradeItem(&item);
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
	if (Doc()->GetListTradeItem()->size() > 0)
		SetControlsButton(true, true);
	else
		SetControlsButton(true, false);
	if (m_pMyItemList)
		m_pMyItemList->UnselectAllItem();
}

void FrTradeEditDlg::OnTradeMyItemCancelBnInit(int param)
{
	m_pTradeMyItemCancelBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnTradeMyItemCancelBnUp()
{
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
}

void FrTradeEditDlg::OnOpenBnInit(int param)
{
	m_pOpenBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnOpenBnUp()
{
	int result = CheckTradeEdit();
	if (result == 1)
	{
		FrTradeFinishEditDlg* dialog = CreateForm<FrTradeFinishEditDlg>(
			g_pFresh->GetManager(), this, "tradefinisheditdlg");
		if (dialog)
		{
			dialog->Open(NULL, FrRET_OK | FrRET_CANCEL);
			dialog->SetFadeout(false);
			dialog->SetListTradeItem(&m_tradeItemList);
			dialog->SetTradeUserId();
			dialog->SetTradeTitle(m_title);
			dialog->SetPackageSale(IsPushedPackageSaleButton()
					? eTypeSalesTrade_Package
					: eTypeSalesTrade_Each);
		}
	}
	else
	{
		SetTradeResultCode(result, 0xe1, NULL);
	}
}

void FrTradeEditDlg::OnCloseBnInit(int param)
{
	m_pCloseBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnCloseBnUp()
{
	SetTradeResultCode(18, 0xe1, NULL);
}

void FrTradeEditDlg::OnExitBnInit(int param)
{
	m_pExitBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeEditDlg::OnExitBnUp()
{
	if (Doc()->GetListTradeItem()->size() > 0)
	{
		Doc()->SendTradeOpenShop();
		OnFreshCancel();
	}
	else
	{
		Doc()->SendTradeCloseShop();
		OnFreshCancel();
	}
}

void FrTradeEditDlg::OnPackageSaleInit(int param)
{
	m_pPackageSale = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pPackageSale)
		m_toolTipMng.Add((FrWnd**)&m_pPackageSale, (eBarButton)0x47);
}

void FrTradeEditDlg::OnPackageSaleButtonUp()
{
	m_pPackageSale->GetStatus();
}

void FrTradeEditDlg::OnTooltipAreaInit(int param)
{
	m_pTooltipArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTradeEditDlg::OnTooltipAreaOwnerDraw(int param)
{
	m_toolTipMng.Process();
}

bool FrTradeEditDlg::OnInit()
{
	EnableDrag(false);
	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
	if (Doc()->GetListTradeItem()->size() > 0)
		SetControlsButton(true, true);
	else
		SetControlsButton(false, false);
	if (IsPackageSale())
		PushPackageSaleButton();
	return true;
}

bool FrTradeEditDlg::PushPackageSaleButton()
{
	if (!m_pPackageSale)
		return false;
	m_pPackageSale->SetStatus(FrButton::PRESSED);
	return true;
}

bool FrTradeEditDlg::IsPushedPackageSaleButton()
{
	return m_pPackageSale->GetStatus() == FrButton::PRESSED ? true : false;
}

void FrTradeEditDlg::SetListEnableTradingMyItem()
{
	m_myItemList.clear();
	sTradeItem item;
	std::list<sItemInfo>* list;
	{
		list = &Doc()->m_partsList;
		for (std::list<sItemInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			IFF_ITEM_COMMON* info = ItemManager()->FindCommonItem((*it).tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				item.dwGuid = (*it).guid;
				item.dwTid = (*it).tid;
				item.iNum = 1 - GetNumTradingItem((*it).guid);
				item.i64Price = 0;
				if (IsLocalContent(S4_CARD_SYSTEM))
				{
					IFF_STRUCT::sPart* part =
						ItemManager()->FindPart((*it).tid);
					if (part)
					{
						item.CharacterSlotNum = part->CharacterSlot;
						item.CaddieSlotNum = part->CaddieSlot;
						memset(item.attachCardTid, 0,
							sizeof(item.attachCardTid));
						for (std::list<sSCardAvilityPeriodInfo>::iterator card =
								 Doc()->m_cardAbilityList[0].begin();
							card != Doc()->m_cardAbilityList[0].end(); ++card)
						{
							if ((*card).partsTid == (*it).tid &&
								(*card).partsUid == (*it).guid)
							{
								item.iNum = 0;
								break;
							}
						}
					}
				}
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	if (IsLocalContent(S4_UCC))
	{
		{
			list = &Doc()->m_uccItemList;
			for (std::list<sItemInfo>::iterator it = list->begin();
				it != list->end(); ++it)
			{
				if (GetNumTradingItem((*it).guid) > 0)
					continue;
				IFF_ITEM_COMMON* info =
					ItemManager()->FindCommonItem((*it).tid);
				if (info && (info->IsSalable == 1 || info->IsSalable == 3))
				{
					bool equipped = false;
					std::map<unsigned int, sCharacterInfo>::iterator
						character = Doc()->m_charMap.begin(),
						end = Doc()->m_charMap.end();
					if (character != end)
					{
						for (; character != end; ++character)
						{
							if ((*character).second.tid ==
									(((*it).tid >> 18 & 255) | 0x04000000) &&
								(*character)
										.second
										.ItemIdList[((*it).tid >> 13) & 31] ==
									(*it).guid)
							{
								equipped = true;
								break;
							}
						}
					}
					if (equipped)
						continue;
					item.dwGuid = (*it).guid;
					item.dwTid = (*it).tid;
					strncpy(item.UccIndex, (*it).UccIndex, 9);
					item.Seq = (*it).Seq;
					item.iNum = 1;
					item.i64Price = 0;
					item.status = (*it).status;
					if (IsLocalContent(S4_CARD_SYSTEM))
					{
						IFF_STRUCT::sPart* part =
							ItemManager()->FindPart((*it).tid);
						if (part)
						{
							item.CharacterSlotNum = part->CharacterSlot;
							item.CaddieSlotNum = part->CaddieSlot;
							memset(item.attachCardTid, 0,
								sizeof(item.attachCardTid));
							for (std::list<sSCardAvilityPeriodInfo>::iterator
									 card = Doc()->m_cardAbilityList[0].begin();
								card != Doc()->m_cardAbilityList[0].end();
								++card)
							{
								if ((*card).partsTid == (*it).tid &&
									(*card).partsUid == (*it).guid)
								{
									item.iNum = 0;
									break;
								}
							}
						}
					}
					if (item.iNum > 0)
						m_myItemList.push_back(item);
				}
			}
		}
	}
	{
		list = &Doc()->m_auxPartList;
		for (std::list<sItemInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			IFF_ITEM_COMMON* info = ItemManager()->FindCommonItem((*it).tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				item.dwGuid = (*it).guid;
				item.dwTid = (*it).tid;
				item.iNum = (*it).Common[0] - GetNumTradingItem((*it).guid);
				item.i64Price = 0;
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	SetListEquipParts();
	for (std::list<sTradeItem>::iterator it = m_myItemList.begin();
		it != m_myItemList.end();)
	{
		if (IsEquipParts((*it).dwTid, (*it).dwGuid))
			it = m_myItemList.erase(it);
		else
			++it;
	}
	{
		list = &Doc()->m_myItemList;
		for (std::list<sItemInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			IFF_ITEM_COMMON* info = ItemManager()->FindCommonItem((*it).tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				item.dwGuid = (*it).guid;
				item.dwTid = (*it).tid;
				item.iNum = (*it).Common[0] - GetNumTradingItem((*it).guid) -
					GetNumEquipItem((*it).tid);
				item.i64Price = 0;
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	{
		list = &Doc()->m_ballList;
		for (std::list<sItemInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if ((*it).tid == Doc()->m_myInfo.userEquip.tidBall)
				continue;
			IFF_ITEM_COMMON* info = ItemManager()->FindCommonItem((*it).tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				item.dwGuid = (*it).guid;
				item.dwTid = (*it).tid;
				item.iNum = (*it).Common[0] - GetNumTradingItem((*it).guid);
				item.i64Price = 0;
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	{
		std::map<unsigned int, sCaddieInfo>* list = &Doc()->m_caddieMap;
		for (std::map<unsigned int, sCaddieInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if ((*it).second.guid == Doc()->m_myInfo.userEquip.guidCaddie)
				continue;
			IFF_ITEM_COMMON* info =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				item.dwGuid = (*it).second.guid;
				item.dwTid = (*it).second.tid;
				item.iNum = 1 - GetNumTradingItem((*it).second.guid);
				item.i64Price = 0;
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	{
		std::map<unsigned int, sItemInfo>* list = &Doc()->m_clubSetMap;
		for (std::map<unsigned int, sItemInfo>::iterator it = list->begin();
			it != list->end(); ++it)
		{
			if ((*it).second.guid == Doc()->m_myInfo.userEquip.guidClubSet)
				continue;
			IFF_ITEM_COMMON* info =
				ItemManager()->FindCommonItem((*it).second.tid);
			if (info && (info->IsSalable == 1 || info->IsSalable == 3))
			{
				memset(&item, 0, sizeof(item));
				item.dwGuid = (*it).second.guid;
				item.dwTid = (*it).second.tid;
				item.iNum = 1 - GetNumTradingItem((*it).second.guid);
				item.i64Price = 0;
				item.dwUpgradeCost = 0;
				memcpy(item.arrEnchant, (*it).second.Common,
					sizeof(item.arrEnchant));
				for (int remaining = 5, i = 0; remaining; --remaining, ++i)
				{
					for (unsigned int level = 0;
						(int)level < item.arrEnchant[i]; ++level)
					{
						IFF_STRUCT::sEnchant* enchant =
							ItemManager()->FindEnchant(
								((0x340 | i) << 20) | level);
						if (enchant)
							item.dwUpgradeCost += enchant->ReqdPang;
					}
				}
				if (item.iNum > 0)
					m_myItemList.push_back(item);
			}
		}
	}
	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		{
			std::list<sCards>* list = &Doc()->m_cardList;
			for (std::list<sCards>::iterator it = list->begin();
				it != list->end(); ++it)
			{
				IFF_ITEM_COMMON* info =
					ItemManager()->FindCommonItem((*it).tid);
				if (info && (info->IsSalable == 1 || info->IsSalable == 3))
				{
					item.dwGuid = (*it).uid;
					item.dwTid = (*it).tid;
					item.iNum = (*it).cardCount - GetNumTradingItem((*it).uid);
					item.i64Price = 0;
					if (item.iNum > 0)
						m_myItemList.push_back(item);
				}
			}
		}
	}
}

void FrTradeEditDlg::SetListEquipParts()
{
	m_equipPartsList.clear();
	for (std::map<unsigned int, sCharacterInfo>::iterator it =
			 Doc()->m_charMap.begin();
		it != Doc()->m_charMap.end(); ++it)
	{
		for (int i = 0; i < 24; ++i)
		{
			if ((*it).second.tidParts[i] != 0)
			{
				sPartEquipInfo part;
				part.typeId = (*it).second.tidParts[i];
				part.id = (*it).second.ItemIdList[i];
				m_equipPartsList.push_back(part);
			}
		}
	}
}

bool FrTradeEditDlg::IsEquipParts(unsigned long typeId, unsigned long id)
{
	bool found = false;
	std::list<sPartEquipInfo>::iterator it;
	for (it = m_equipPartsList.begin(); it != m_equipPartsList.end(); it++)
	{
		if ((*it).id == 0 && (*it).typeId == typeId)
		{
			found = true;
			break;
		}
		if ((*it).id != 0 && (*it).typeId == typeId && (*it).id == id)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		m_equipPartsList.erase(it);
		found = true;
	}
	return found;
}

int FrTradeEditDlg::GetNumEquipItem(unsigned long typeId)
{
	int count = 0;
	for (int i = 0; i < 10; ++i)
		if (typeId == Doc()->m_myInfo.userEquip.tidItemSlot[i])
			++count;
	return count;
}

int FrTradeEditDlg::GetNumTradingItem(unsigned long id)
{
	int count = 0;
	for (std::list<sTradeItem>::iterator it =
			 Doc()->GetListTradeItem()->begin();
		it != Doc()->GetListTradeItem()->end(); ++it)
	{
		if ((*it).dwGuid == id)
			count += (*it).iNum;
	}
	return count;
}

void FrTradeEditDlg::SetListTradingItem()
{
	std::list<sTradeItem>* list = Doc()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = list->begin(); it != list->end();
		++it)
		m_tradeItemList.push_back(*it);
}

void FrTradeEditDlg::SetListMyItem()
{
	if (m_pMyItemList)
	{
		m_pMyItemList->ClearItem();
		_TradeItemSort<sTradeItem> compare;
		m_myItemList.sort(compare);
		for (std::list<sTradeItem>::iterator it = m_myItemList.begin();
			it != m_myItemList.end(); ++it)
			m_pMyItemList->AddItem(&*it);
	}
}

void FrTradeEditDlg::SetListTradeItem()
{
	if (m_pTradeItemList)
	{
		m_pTradeItemList->ClearItem();
		for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin();
			it != m_tradeItemList.end(); ++it)
			m_pTradeItemList->AddItem(&*it);
	}
}

void FrTradeEditDlg::SetTradeMyItem(sTradeItem* item)
{
	if (m_pTradeMyItemName)
		m_pTradeMyItemName->ClearLine();
	if (m_pTradeMyItemAbility)
		m_pTradeMyItemAbility->ClearLine();
	m_pSelectItem = item;
	if (item)
	{
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
		if (common && m_pTradeMyItemName)
		{
			if (IsLocalContent(S4_UCC) && (item->status & 1) &&
				IsUccClothes(item->dwTid))
			{
				const char* name = GetUccItemName(item->dwGuid);
				m_pTradeMyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", name),
					false, true);
			}
			else
				m_pTradeMyItemName->AddText(
					MakeStr("\\cT\\c%s\\cN\\c", common->Name), false, true);
			short ability[5];
			short slots[5] = { 0 };
			unsigned long type = item->dwTid >> 26;
			bool draw = true;
			switch (type)
			{
			case 2:
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->dwTid);
				if (part == NULL)
					return;
				memcpy(ability, part->Attr, sizeof(ability));
				memcpy(slots, part->Slot, sizeof(slots));
			}
			break;
			case 7:
			{
				IFF_STRUCT::sCaddie* caddie =
					ItemManager()->FindCaddie(item->dwTid);
				if (caddie == NULL)
					return;
				memcpy(ability, caddie->Attr, sizeof(ability));
			}
			break;
			case 8:
			{
				IFF_STRUCT::sCadItem* caddie =
					ItemManager()->FindCadItem(item->dwTid);
				if (caddie == NULL)
					return;
				memcpy(ability, caddie->COM, sizeof(ability));
			}
			break;
			default:
				draw = false;
				break;
			}
			if (draw)
			{
				const char* statName = "";
				for (int remaining = 5, i = 0; remaining; --remaining, ++i)
				{
					if (ability[i] || slots[i])
					{
						switch (i)
						{
						case 0:
							statName = "\xc6\xc4\xbf\xf6";
							break;
						case 1:
							statName = "\xc4\xc1\xc6\xae\xb7\xd1";
							break;
						case 2:
							statName = "\xc1\xa4\xc8\xae\xb5\xb5";
							break;
						case 3:
							statName = "\xbd\xba\xc7\xc9";
							break;
						case 4:
							statName = "\xc4\xbf\xba\xea";
							break;
						}
						if (ability[i] > 0 || slots[i] > 0)
						{
							if (ability[i] == 0)
								m_pTradeMyItemAbility->AddText(
									MakeStr(
										"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
										statName, slots[i]),
									false, true);
							else if (slots[i] == 0)
								m_pTradeMyItemAbility->AddText(
									MakeStr(
										"\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
										statName, ability[i]),
									false, true);
							else
								m_pTradeMyItemAbility->AddText(
									MakeStr(
										"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
										statName, ability[i], slots[i]),
									false, true);
						}
					}
				}
			}
			else if (type == 4)
			{
				m_pTradeMyItemAbility->AddText(
					MakeStr(
						"\\c0xff009000\\c\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xc7\xd1 \xba\xf1\xbf\xeb:\n%d\xc6\xce\\c0xff000000\\c",
						item->dwUpgradeCost),
					false, true);
			}
		}
	}
}

void FrTradeEditDlg::AddListTradeItem(sTradeItem* item)
{
	if (!item || !m_pSelectItem)
		return;
	std::list<sTradeItem>::iterator it;
	for (it = m_tradeItemList.begin(); it != m_tradeItemList.end(); ++it)
		if (&*it == m_pSelectItem)
			break;
	bool add = true;
	if (it != m_tradeItemList.end())
	{
		if (&*it == m_pSelectItem)
			add = false;
	}
	if (add)
	{
		it = m_tradeItemList.insert(m_tradeItemList.end(), *item);
		m_pTradeItemList->AddItem(&*it);
	}
	else
	{
		*it = *item;
	}
}

void FrTradeEditDlg::DelListTradeItem(sTradeItem* item)
{
	if (!item)
		return;
	for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin();
		it != m_tradeItemList.end(); ++it)
	{
		if (&*it != item)
			continue;
		m_pTradeItemList->DelItem(&*it);
		m_tradeItemList.erase(it);
		return;
	}
}

void FrTradeEditDlg::AddListMyItem(sTradeItem* item)
{
	if (!item)
		return;
	std::list<sTradeItem>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end(); ++it)
		if ((*it).dwGuid == item->dwGuid)
			break;
	if (it == m_myItemList.end())
	{
		it = m_myItemList.insert(m_myItemList.end(), *item);
		m_pMyItemList->AddItem(&*it);
	}
	else if (item->iNum > 0)
	{
		(*it).iNum += item->iNum;
	}
}

void FrTradeEditDlg::DelListMyItem(sTradeItem* item)
{
	if (!item || !m_pSelectItem)
		return;
	std::list<sTradeItem>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end(); ++it)
	{
		if ((*it).dwGuid == item->dwGuid)
			break;
	}
	if (it != m_myItemList.end())
	{
		int count = 0;
		for (std::list<sTradeItem>::iterator trade = m_tradeItemList.begin();
			trade != m_tradeItemList.end(); ++trade)
		{
			if (&*trade == m_pSelectItem)
			{
				count = (*trade).iNum;
				break;
			}
		}
		if (&*it == m_pSelectItem)
		{
			if ((*it).iNum == item->iNum)
			{
				m_pMyItemList->DelItem(&*it);
				m_myItemList.erase(it);
			}
			else if ((*it).iNum > item->iNum)
				(*it).iNum -= item->iNum;
		}
		else
		{
			(*it).iNum += count - item->iNum;
		}
	}
}

void FrTradeEditDlg::CancelSelectedTradeItem()
{
	if (!m_pTradeItemList)
		return;
	FrListItem* row = m_pTradeItemList->GetSelected();
	if (!row)
		return;
	sTradeItem* item = (sTradeItem*)row->pData;
	if (!item)
		return;
	if (item == m_pSelectItem)
	{
		SetTradeMyItem(NULL);
		SetControlsTradeMyItem(NULL, false);
	}
	AddListMyItem(item);
	DelListTradeItem(item);
	if (m_tradeItemList.size() <= 0)
	{
		if (Doc()->GetListTradeItem()->size() > 0)
			SetControlsButton(false, true);
		else
			SetControlsButton(false, false);
	}
}

void FrTradeEditDlg::DrawPang()
{
	if (!g_pFresh->GetManager()->GetGDI() || !m_pPangFont)
		return;
	char text[32] = { 0 };
	WRect pos = GetRect();
	static float width;
	static int fontWidth = m_pPangFont->GetFontInfo()->fontw;
	MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
	width = (float)((strlen(text) - 1) * (fontWidth - 1.f));
	m_pPangFont->Print(g_view, pos.x + 130.f - width, pos.y + 483.f, text, 0,
		0xffffffff, NULL);
}

void FrTradeEditDlg::DrawItem(WPoint pos, sTradeItem* item, unsigned char state)
{
	if (!item)
		return;
	unsigned long tid = item->dwTid;
	int number = item->iNum;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	if (!tid)
		return;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(tid);
	if (!common)
		return;
	float dx = 0.f, dy = 0.f;
	const Bitmap* bitmap = g_pFresh->GetBitmap(common->Icon);
	if (IsLocalContent(S4_UCC) && (item->status & 1) &&
		IsUccClothes(item->dwTid))
		bitmap = GetUccItemIcon(item->dwGuid);
	if ((tid >> 26) == 2)
	{
		dx = -12.f;
		dy = -20.f;
	}
	if (bitmap)
	{
		WRect itemRect(pos.x + dx + 14.f, pos.y + dy + 11.f,
			(float)bitmap->Width(), (float)bitmap->Height());
		gi->DrawTexture(bitmap, itemRect, 0xffffffff, 0);
		gi->SetTextColor(0xff000000, 0xffffffff);
		gi->SetTextStyle(4);
		if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
		{
			if ((item->status & 1) && item->Seq == 1 && m_pUccOriginal)
			{
				gi->DrawTexture(m_pUccOriginal,
					WRect(pos.x + 46.f, pos.y + 48.f,
						(float)m_pUccOriginal->Width(),
						(float)m_pUccOriginal->Height()),
					0xffffffff, 0);
			}
			else if ((item->status & 1) && item->Seq > 1 && m_pUccCopied)
			{
				gi->DrawTexture(m_pUccCopied,
					WRect(pos.x + 46.f, pos.y + 48.f,
						(float)m_pUccCopied->Width(),
						(float)m_pUccCopied->Height()),
					0xffffffff, 0);
			}
			else if ((item->status & 2) && m_pUccTempSaved)
			{
				gi->DrawTexture(m_pUccTempSaved,
					WRect(pos.x + 46.f, pos.y + 48.f,
						(float)m_pUccTempSaved->Width(),
						(float)m_pUccTempSaved->Height()),
					0xffffffff, 0);
			}
		}
		if (number > 1)
		{
			unsigned int i = 0;
			char* count = MakeStr("%d", number);
			float x = pos.x + 54.f;
			for (i = 0; i < strlen(count); ++i)
				x -= m_pNumFont->GetCharWidthA(g_view, count[i] - '0');
			m_pNumFont->Print(g_view, x, pos.y + 42.f, count, 0, 0xffffffff,
				NULL);
		}
	}
	switch (tid >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(pos.x + 38.f - float2int((float)level->Width() * 0.5f),
					pos.y + 35.f - float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

void FrTradeEditDlg::SetControlsButton(bool open, bool close)
{
	m_pOpenBn->Enable(open);
	m_pCloseBn->Enable(close);
	m_pExitBn->Enable(true);
}

void FrTradeEditDlg::SetControlsTradeMyItem(sTradeItem* item, bool enable)
{
	if (m_pTradeMyItemNumber)
		m_pTradeMyItemNumber->ClearLine();
	if (m_pTradeMyItemPrice)
		m_pTradeMyItemPrice->ClearLine();
	if (!item)
	{
		m_pTradeMyItemNumber->Enable(false);
		m_pTradeMyItemPrice->Enable(false);
		m_pTradeMyItemOkBn->Enable(false);
		m_pTradeMyItemCancelBn->Enable(false);
		return;
	}
	m_pTradeMyItemNumber->Enable(true);
	m_pTradeMyItemPrice->Enable(true);
	m_pTradeMyItemOkBn->Enable(true);
	m_pTradeMyItemCancelBn->Enable(true);
	if (item->iNum == 1)
	{
		m_pTradeMyItemNumber->SetLine(1, "1", 0, false, 0);
		m_pTradeMyItemNumber->Enable(enable);
	}
	if (item->i64Price > 0)
	{
		m_pTradeMyItemNumber->SetLine(1, MakeStr("%d", item->iNum), 0, false,
			0);
		m_pTradeMyItemPrice->SetLine(1, MakeStr("%I64d", item->i64Price), 0,
			false, 0);
	}
}

int FrTradeEditDlg::CheckTradeEdit()
{
	memset(m_title, 0, sizeof(m_title));
	if (m_tradeItemList.size() <= 0)
		return 8;
	const char* title = Doc()->GetTradeTitle();
	if (strlen(title) >= 32)
		return 6;
	char filtered[256] = { 0 };
	strcpy(filtered, title);
	if (Doc()->m_chatManager.Filtering(filtered))
		return 7;
	if (strlen(title) == 0 && strlen(Doc()->GetTradeTitle()) == 0)
		return 5;
	WTL::CString trimmed(title);
	trimmed.TrimRight();
	if (trimmed.GetLength() == 0)
		return 17;
	if (IsPushedPackageSaleButton() &&
		FindIf(m_tradeItemList, CheckPackageSaleItem))
		return 30;
	strcpy(m_title, title);
	return 1;
}

void FrTradeEditDlg::UpdateTradeTitle()
{
	if (!m_pTitle)
		return;
	m_pTitle->ClearLine();
	const char* title = Doc()->GetTradeTitle();
	m_pTitle->SetLine(1, title, 0, false, 0);
}

void FrTradeEditDlg::UpdateTradeTitleEdit()
{
	if (!m_pTitleEdit)
		return;
	m_pTitleEdit->ClearLine();
	const char* title = Doc()->GetTradeTitle();
	m_pTitleEdit->SetLine(1, title, 0, false, 0);
}

eResultTrade FrTradeEditDlg::SendTradeTitle()
{
	eResultTrade result = eResultTrade_Success;
	if (m_pTitleEdit)
	{
		WTL::CString trimmed(m_pTitleEdit->GetLine(1, false));
		trimmed.TrimRight();
		if (trimmed.GetLength() == 0)
			return eResultTrade_EmptyTitle;
		const char* title = m_pTitleEdit->GetLine(1, false);
		int length = strlen(title);
		char filtered[256] = { 0 };
		strcpy(filtered, title);
		if (Doc()->m_chatManager.Filtering(filtered))
			result = eResultTrade_FilteredTitle;
		else if (length > 0 && length < 32)
		{
			if (strcmp(title, Doc()->GetTradeTitle()) != 0)
				Doc()->SendTradeTitle(title);
		}
		m_pTitleEdit->ClearLine();
	}
	return result;
}

IMPLEMENT_OBJECT(FrTradeFinishEditDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeFinishEditDlg, FrForm)
ON_FRESH_VI("tradefinisheditdlg", FRCMD_INIT,
	FrTradeFinishEditDlg::OnTradeFinishEditDlgInit)
ON_FRESH_VI("trade_item_list", FRCMD_INIT,
	FrTradeFinishEditDlg::OnTradeItemListInit)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	FrTradeFinishEditDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("userid", FRCMD_INIT, FrTradeFinishEditDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, FrTradeFinishEditDlg::OnTitleInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrTradeFinishEditDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTradeFinishEditDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrTradeFinishEditDlg::FrTradeFinishEditDlg()
{
	m_pTradeItemList = NULL;
	m_pUserId = NULL;
	m_pTitle = NULL;
	m_pItemBaseN = g_pFresh->GetBitmap("item_base_n");
	m_pItemBaseO = g_pFresh->GetBitmap("item_base_o");
	m_pItemBaseD = g_pFresh->GetBitmap("item_base_d");
	m_pItemBaseEmpty = g_pFresh->GetBitmap("item_base_empty");
	m_pUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:,";
	m_pNumFont = g_resrcmng->GetTitleFont();
	m_pNumFont->Create(&font);
	memset(m_title, 0, sizeof(m_title));
	m_pTradeList = NULL;
	m_titleState = 0;
	m_packageSale = eTypeSalesTrade_None;
	m_clock.pBitmap = g_pFresh->GetBitmap("clock");
	m_clock.rect =
		WRect(50.f, 35.f, m_clock.pBitmap->Width(), m_clock.pBitmap->Height());
	m_deleteMark.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteMark.rect = WRect(50.f, 35.f, m_deleteMark.pBitmap->Width(),
		m_deleteMark.pBitmap->Height());
}

FrTradeFinishEditDlg::~FrTradeFinishEditDlg()
{
	if (m_pNumFont)
		m_pNumFont->Erase();
	if (g_resrcmng && m_pNumFont)
	{
		g_resrcmng->Release(m_pNumFont);
		m_pNumFont = NULL;
	}
}

void FrTradeFinishEditDlg::OnTradeFinishEditDlgInit(int param)
{
}

void FrTradeFinishEditDlg::OnUserIdInit(int param)
{
	m_pUserId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeFinishEditDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrTradeFinishEditDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeFinishEditDlg::OnOkBnUp()
{
	int index = 0;
	int result = 0;
	if (m_pTradeList->size() <= 0 || m_pTradeList->size() > 6 ||
		m_titleState <= 0)
	{
		SetTradeResultCode(result, 0xe1, NULL);
		Doc()->SendTradeCloseShop();
	}
	else
	{
		if (m_titleState == 2)
			Doc()->SendTradeTitle(std::string(m_title));
		WSendPacket packet((enumClientPacket)0x7c);
		packet.Encode4(m_packageSale);
		packet.Encode4(m_pTradeList->size());
		for (std::list<sTradeItem>::iterator it = m_pTradeList->begin();
			it != m_pTradeList->end(); ++it)
		{
			(*it).iIndex = index;
			packet.EncodeBuffer(&*it, sizeof(sTradeItem));
			++index;
		}
		packet.Send(TO_GAME);
	}
	OnFreshOkay();
}

bool FrTradeFinishEditDlg::OnInit()
{
	EnableDrag(false);
	return true;
}

void FrTradeFinishEditDlg::OnTradeItemListInit(int param)
{
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrTradeFinishEditDlg::OnTradeItemListOwnerDraw(int param)
{
	FrListItem* row = (FrListItem*)param;
	if (!row)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	const Bitmap* bitmap;
	if (!row->pData)
		bitmap = m_pItemBaseEmpty;
	else
	{
		if (row->selected)
		{
			bitmap = m_pItemBaseD;
			gi->SetTextColor(0xffff0000, 0xffffffff);
		}
		else if (row->underCursor)
		{
			bitmap = m_pItemBaseO;
			gi->SetTextColor(0xff000000, 0xffffffff);
		}
		else
		{
			bitmap = m_pItemBaseN;
			gi->SetTextColor(0xff000000, 0xffffffff);
		}
	}
	gi->SetTextStyle(0);
	if (bitmap)
	{
		unsigned long color = 0xffffffff;
		if (IsPackageSale())
			color = 0x20ffffff;
		gi->DrawTexture(bitmap,
			WRect(row->pos.x, row->pos.y, (float)bitmap->Width(),
				(float)bitmap->Height()),
			color, 0);
	}
	if (!row->pData)
		return;
	sTradeItem item = *(sTradeItem*)row->pData;
	unsigned long tid = item.dwTid;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(tid);
	if (!common)
		return;
	unsigned long type = tid >> 26;
	if (type == 18)
	{
		unsigned long subtype = (item.dwTid >> 11) & 0x7ff;
		if (subtype == 12 || subtype == 11)
			return;
	}
	if (type == 2)
	{
		IFF_STRUCT::sChar* character =
			ItemManager()->FindChar(((tid >> 18) & 0xff) | 0x04000000);
		if (character)
		{
			const Bitmap* characterBitmap =
				g_pFresh->GetBitmap(MakeStr("s_%s", character->c.Icon));
			if (characterBitmap)
				gi->DrawTexture(characterBitmap,
					WRect(row->pos.x + 3.f, row->pos.y + 3.f,
						(float)characterBitmap->Width(),
						(float)characterBitmap->Height()),
					0xffffffff, 0);
		}
	}
	bitmap = g_pFresh->GetBitmap(common->Icon);
	if (bitmap)
	{
		float x, y;
		switch (type)
		{
		case 2:
			x = 7.f;
			y = 2.f;
			break;
		default:
			x = 16.f;
			y = 27.f;
			break;
		}
		if (IsLocalContent(S4_UCC) && (item.status & 1) &&
			IsUccClothes(item.dwTid))
			bitmap = GetUccItemIcon(item.dwGuid);
		if (bitmap)
			gi->DrawTexture(bitmap,
				WRect(row->pos.x + x, row->pos.y + y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);
		if (IsLocalContent(S4_UCC) && IsUccClothes(item.dwTid))
		{
			if ((item.status & 1) && item.Seq == 1 && m_pUccOriginal)
				gi->DrawTexture(m_pUccOriginal,
					WRect(row->pos.x + 49.f, row->pos.y + 54.f,
						(float)m_pUccOriginal->Width(),
						(float)m_pUccOriginal->Height()),
					0xffffffff, 0);
			else if ((item.status & 1) && item.Seq > 1 && m_pUccCopied)
				gi->DrawTexture(m_pUccCopied,
					WRect(row->pos.x + 49.f, row->pos.y + 54.f,
						(float)m_pUccCopied->Width(),
						(float)m_pUccCopied->Height()),
					0xffffffff, 0);
			else if ((item.status & 2) && m_pUccTempSaved)
				gi->DrawTexture(m_pUccTempSaved,
					WRect(row->pos.x + 49.f, row->pos.y + 54.f,
						(float)m_pUccTempSaved->Width(),
						(float)m_pUccTempSaved->Height()),
					0xffffffff, 0);
		}
	}
	if (IsLocalContent(S4_UCC) && (item.status & 1) && IsUccClothes(item.dwTid))
	{
		const char* name = GetUccItemName(item.dwGuid);
		gi->SetTextStyle(1);
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 7.f), 2, name);
		gi->SetTextStyle(0);
	}
	else
	{
		gi->SetTextStyle(1);
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 7.f), 2,
			common->Name);
		gi->SetTextStyle(0);
	}
	char price[32] = { 0 };
	MakePriceStrA(price, item.i64Price);
	SetTextColorByPrice(item.i64Price);
	if (item.iNum > 1)
	{
		unsigned int i = 0;
		char* count = MakeStr("%d", item.iNum);
		float x = row->pos.x + 54.f;
		for (i = 0; i < strlen(count); ++i)
			x -= m_pNumFont->GetCharWidthA(g_view, count[i] - '0');
		m_pNumFont->Print(g_view, x, row->pos.y + 55.f, count, 0, 0xffffffff,
			NULL);
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 23.f), 2,
			MakeStr("\xb0\xb3\xb4\xe7 %s\xc6\xce", price));
	}
	else
		gi->Print(WPoint(row->pos.x + 182.f, row->pos.y + 23.f), 2,
			MakeStr("%s\xc6\xce", price));
	gi->SetTextColor(0xff000000, 0xffffffff);
	DrawAbility(item, row);
	switch (item.dwTid >> 26)
	{
	case 2:
	case 4:
	case 7:
	case 16:
	case 28:
	{
		const Bitmap* level = g_pFresh->GetBitmap(
			MakeStr("level_%03d", (common->Level & 0x7f) + 1));
		if (level)
			gi->DrawTexture(level,
				WRect(row->pos.x + 40.f -
						float2int((float)level->Width() * 0.5f),
					row->pos.y + 51.f -
						float2int((float)level->Height() * 0.5f),
					(float)level->Width(), (float)level->Height()),
				0xffffffff, 0);
	}
	break;
	}
}

void FrTradeFinishEditDlg::SetTradeUserId()
{
	if (m_pUserId)
		m_pUserId->SetLine(1,
			K2L_Compatibility("\xb3\xaa\xc0\xc7 \xbb\xf3\xc1\xa1"), 0, false,
			0);
}

void FrTradeFinishEditDlg::SetTradeTitle(char* title)
{
	if (!title || strlen(title) >= 32)
		return;
	m_titleState = 0;
	int length = strlen(title);
	int oldLength = strlen(Doc()->GetTradeTitle());
	if (length <= 0 && oldLength <= 0)
	{
		m_titleState = 0;
		return;
	}
	if (length > 0 && length < 32)
	{
		strcpy(m_title, title);
		m_titleState = 2;
	}
	else if (oldLength > 0 && oldLength < 32)
	{
		strcpy(m_title, Doc()->GetTradeTitle());
		m_titleState = 1;
	}
	if (m_pTitle)
		m_pTitle->SetLine(1, m_title, 0, false, 0);
}

void FrTradeFinishEditDlg::SetListTradeItem(std::list<sTradeItem>* list)
{
	if (m_pTradeItemList)
	{
		m_pTradeList = list;
		m_pTradeItemList->ClearItem();
		for (std::list<sTradeItem>::iterator it = m_pTradeList->begin();
			it != m_pTradeList->end(); ++it)
			m_pTradeItemList->AddItem(&*it);
	}
}

IMPLEMENT_OBJECT(FrTradeWarningDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeWarningDlg, FrForm)
ON_FRESH_VI("TradeWarningDlg", FRCMD_INIT,
	FrTradeWarningDlg::OnTradeWarningDlgInit)
ON_FRESH_VI("msg01", FRCMD_INIT, FrTradeWarningDlg::OnMsg01Init)
ON_FRESH_VI("msg02", FRCMD_INIT, FrTradeWarningDlg::OnMsg02Init)
ON_FRESH_VI("ok", FRCMD_INIT, FrTradeWarningDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTradeWarningDlg::OnOkBnUp)
ON_FRESH_VI("cancel", FRCMD_INIT, FrTradeWarningDlg::OnCancelBnInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrTradeWarningDlg::OnCancelBnUp)
END_FRESH_MSGMAP()

FrTradeWarningDlg::FrTradeWarningDlg()
{
	m_pMsg01 = NULL;
	m_pMsg02 = NULL;
	m_type = 1;
}

FrTradeWarningDlg::~FrTradeWarningDlg()
{
}

void FrTradeWarningDlg::OnTradeWarningDlgInit(int param)
{
}

void FrTradeWarningDlg::OnMsg01Init(int param)
{
	m_pMsg01 = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrTradeWarningDlg::OnMsg02Init(int param)
{
	m_pMsg02 = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrTradeWarningDlg::OnOkBnInit(int param)
{
	m_pOkBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeWarningDlg::OnOkBnUp()
{
	switch (m_type)
	{
	case 18:
	case 19:
		Doc()->SendTradeCloseShop();
		{
			IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
			if (actor)
				actor << MsgObject(NULL, 0x273, 5, 0, 0, 0, 0);
		}
		break;
	case 20:
	{
		FrWnd* wnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			"tradeeditdlg");
		if (!wnd)
		{
			FrTradeEditDlg* form = DYNAMIC_CAST(FrTradeEditDlg, wnd);
		}
		else
			((FrTradeEditDlg*)wnd)->CancelSelectedTradeItem();
	}
	break;
	}
	OnFreshOkay();
}

void FrTradeWarningDlg::OnCancelBnInit(int param)
{
	m_pCancelBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeWarningDlg::OnCancelBnUp()
{
	OnFreshCancel();
}

void FrTradeWarningDlg::SetControls(int type)
{
	if (type == 0)
	{
		m_pCancelBn->SetVisible(false);
		m_pOkBn->MoveWindow(WPoint(GetRect().x + 92.f, GetRect().y + 137.f));
	}
	else
	{
		m_pOkBn->SetVisible(true);
		m_pCancelBn->SetVisible(true);
		m_pOkBn->MoveWindow(WPoint(GetRect().x + 50.f, GetRect().y + 137.f));
		m_pCancelBn->MoveWindow(
			WPoint(GetRect().x + 135.f, GetRect().y + 137.f));
	}
}

void FrTradeWarningDlg::SetWarningMessageType(int type, unsigned char param)
{
	m_type = type;
	m_pMsg01->SetTextColor(0xff000000, 0xffffffff);
	switch (m_type)
	{
	case 0:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg01->SetCaption("\xbf\xc0\xb7\xf9");
		m_pMsg02->SetCaption(
			"\xb0\xfc\xb7\xc3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc3\xca\xb1\xe2\xc8\xad\xb5\xcb\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 2:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg01->SetCaption("\xbf\xc0\xb7\xf9");
		m_pMsg02->SetCaption(
			"\xc0\xcc\xb9\xcc \xbb\xf3\xc1\xa1\xc0\xcc \xb0\xb3\xbc\xb3\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 3:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg01->SetCaption(
			"\xbb\xf3\xc1\xa1\xc0\xcc \xb4\xdd\xc7\xfb\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.");
		SetControls(0);
		break;
	case 4:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg01->SetCaption(
			"\xbb\xf3\xc1\xa1\xc0\xcc \xc6\xed\xc1\xfd\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb0\xb3\xbc\xb3 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.");
		SetControls(0);
		break;
	case 5:
		m_pMsg01->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xc0\xcc \xc0\xd4\xb7\xc2\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xc0\xd4\xb7\xc2 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 6:
		m_pMsg01->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xc0\xcc \xc0\xdf\xb8\xf8\xc0\xd4\xb7\xc2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xc1\xa4\xbb\xf3 \xc0\xd4\xb7\xc2 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 17:
		m_pMsg01->SetCaption(
			"\xbb\xf3\xc1\xa1\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xc8\xc4 \xc8\xae\xc0\xce \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf\xbc\xad");
		m_pMsg02->SetCaption(
			"\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc1\xa1\xb8\xed\xc0\xce\xc1\xf6 \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		SetControls(0);
		break;
	case 7:
		m_pMsg01->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xbf\xa1 \xc0\xfb\xb4\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba");
		m_pMsg02->SetCaption(
			"\xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 8:
		m_pMsg01->SetCaption(
			"\xb5\xee\xb7\xcf\xc7\xcf\xbd\xc5 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb0\xb3\xbc\xb3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 9:
		m_pMsg01->SetCaption(
			"\xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb0\xb3\xbc\xb3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 10:
		m_pMsg01->SetCaption(
			"\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6\xb7\xae\xc0\xcc \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 11:
		m_pMsg01->SetCaption(
			"\xbe\xc6\xc0\xcc\xc5\xdb \xb0\xa1\xb0\xdd\xc0\xcc \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 12:
		m_pMsg01->SetCaption(
			"\xbe\xc6\xc0\xcc\xc5\xdb \xb0\xa1\xb0\xdd\xc0\xcc 3\xc3\xb5\xb8\xb8\xc6\xce \xc1\xa6\xc7\xd1\xc0\xbb \xb3\xd1\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		break;
	case 13:
		m_pMsg01->SetCaption(
			"\xba\xbb \xbb\xf3\xc7\xb0\xc0\xba \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xb8\xa5 \xbb\xf3\xc7\xb0\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		SetControls(0);
		break;
	case 14:
		m_pMsg01->SetCaption(
			"\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb\xc0\xda\xb0\xa1 \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 18:
		m_pMsg01->SetCaption(
			"\xb5\xee\xb7\xcf\xb5\xc8 \xb8\xf0\xb5\xe7 \xb9\xb0\xc7\xb0\xc0\xcc \xc3\xeb\xbc\xd2\xb5\xcb\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb4\xdd\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		break;
	case 19:
		m_pMsg01->SetCaption(
			"\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb4\xdd\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		break;
	case 20:
		m_pMsg01->SetCaption(
			"\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xb9\xb0\xc7\xb0\xc0\xbb \xb5\xee\xb7\xcf");
		m_pMsg02->SetCaption(
			"\xc3\xeb\xbc\xd2\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		break;
	case 21:
		m_pMsg01->SetCaption(
			"\xbc\xd2\xc1\xf6\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb9\xb0\xc7\xb0\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 22:
		m_pMsg01->SetCaption(
			"\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xc5\xbc\xad \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 15:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		if (param != 0)
			m_pMsg01->SetCaption(
				"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		else
			m_pMsg01->SetCaption(
				"\xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xce \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg02->SetCaption(
			"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 24:
		m_pMsg01->SetCaption(
			"\xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb3\xca\xb9\xab \xb8\xb9\xc0\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 16:
		m_pMsg01->SetTextColor(0xffff0000, 0xffffffff);
		m_pMsg01->SetCaption(
			"\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 25:
		m_pMsg01->SetCaption(
			"\xc7\xf6\xc0\xe7 \xc4\xab\xb5\xe5\xb0\xa1 \xc0\xce\xc3\xa6\xc6\xae\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba");
		m_pMsg02->SetCaption(
			"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 26:
		m_pMsg01->SetCaption(
			"\xc3\xa4\xb3\xce\xbf\xa1 \xc0\xa7\xc4\xa1\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 27:
		m_pMsg01->SetCaption(
			"\xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1 \xc0\xa7\xc4\xa1\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 28:
		m_pMsg01->SetCaption(
			"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xb8\xb8\xb5\xe5\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption(
			"\xb4\xd9\xb8\xa5 \xb4\xeb\xc8\xad\xb9\xe6\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 29:
		m_pMsg01->SetCaption(
			"\xb9\xad\xc0\xbd\xc6\xc7\xb8\xc5\xb7\xce \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	case 30:
		m_pMsg01->SetCaption(
			"\xb9\xad\xc0\xbd\xc6\xc7\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1");
		m_pMsg02->SetCaption(
			"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		SetControls(0);
		break;
	default:
		OnFreshCancel();
		break;
	}
}

void FrTradeWarningDlg::SetWarningMessageForPackage(int type)
{
	if (type >= 1900001 && type <= 1900039)
	{
		m_pMsg01->SetCaption(MakeStr(
			"\xb9\xad\xc0\xbd\xc6\xc7\xb8\xc5 \xbf\xc0\xb7\xf9 [%d]", type));
		SetControls(0);
	}
	else
		SetWarningMessageType(type, 0);
}

IMPLEMENT_OBJECT(FrTradeLevelLimitDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrTradeLevelLimitDlg, FrForm)
ON_FRESH_VI("ok", FRCMD_INIT, FrTradeLevelLimitDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTradeLevelLimitDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrTradeLevelLimitDlg::FrTradeLevelLimitDlg()
{
}

FrTradeLevelLimitDlg::~FrTradeLevelLimitDlg()
{
}

void FrTradeLevelLimitDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTradeLevelLimitDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrTradeLevelLimitDlg::OnInit()
{
	return true;
}

void SetTradeResultCode(int code, int packetType, sTradeItem* item)
{
	if (code == 15)
	{
		FrWnd* wnd = g_pFresh->GetDesktop()->FindChildByName("tradewarningdlg");
		if (!wnd)
		{
			FrTradeWarningDlg* dialog = DYNAMIC_CAST(FrTradeWarningDlg, wnd);
			dialog = CreateForm<FrTradeWarningDlg>(g_pFresh->GetManager(), NULL,
				"tradewarningdlg");
			if (dialog)
			{
				dialog->Open(NULL, FrESCAPE);
				dialog->SetWarningMessageType(15, (unsigned char)packetType);
				dialog->SetFadeout(false);
			}
		}
	}
	else if (code == 1)
	{
		if (packetType == 0xea)
		{
			if (IsLocalContent(S4_ITEM_STORAGE) == true)
			{
				if (CItemStorage::Instance())
				{
					if (CItemStorage::Instance()->IsVisible())
						return;
				}
			}
			FrTradeResultDlg* dialog = CreateForm<FrTradeResultDlg>(
				g_pFresh->GetManager(), NULL, "traderesultdlg");
			if (dialog)
			{
				dialog->Open(NULL, FrESCAPE);
				dialog->SetFadeout(false);
				dialog->SetResultItem(item);
			}
		}
	}
	else if (code == 23)
	{
		FrTradeLevelLimitDlg* dialog = CreateForm<FrTradeLevelLimitDlg>(
			g_pFresh->GetManager(), NULL, "tradelevellimitdlg");
		if (dialog)
			dialog->Open(NULL, FrRET_OK);
	}
	else
	{
		FrWnd* result =
			g_pFresh->GetDesktop()->FindChildByName("traderesultdlg");
		if (!result)
		{
			FrTradeResultDlg* resultDialog =
				DYNAMIC_CAST(FrTradeResultDlg, result);
			FrWnd* wnd =
				g_pFresh->GetDesktop()->FindChildByName("tradewarningdlg");
			if (!wnd)
			{
				FrTradeWarningDlg* dialog =
					DYNAMIC_CAST(FrTradeWarningDlg, wnd);
				dialog = CreateForm<FrTradeWarningDlg>(g_pFresh->GetManager(),
					NULL, "tradewarningdlg");
				if (dialog)
				{
					dialog->Open(NULL, FrESCAPE);
					dialog->SetWarningMessageType(code, 0);
					dialog->SetFadeout(false);
				}
			}
		}
		else
		{
			switch (code)
			{
			case 3:
				CChatMsg::Instance()->AddChatMsg(
					"\xbb\xf3\xc1\xa1\xc0\xcc \xb4\xdd\xc7\xfb\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
					0xffff, false, false);
				break;
			case 4:
				CChatMsg::Instance()->AddChatMsg(
					"\xbb\xf3\xc1\xa1\xc0\xcc \xc6\xed\xc1\xfd\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xb3\xbc\xb3 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
					0xffff, false, false);
				break;
			}
		}
	}
}
