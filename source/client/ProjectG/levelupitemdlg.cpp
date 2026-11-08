#include "levelupitemdlg.h"
#include "projectg.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "../../shared/sharedtables.h"

extern Fresh* g_pFresh;

struct LevelUpGift
{
	int level;
	unsigned long tid;
	int count;
};

static const LevelUpGift s_levelUpGift[] = {
	{ 0,  0x0,        0     },
	{ 1,  0x0,        0     },
	{ 2,  0x18000005, 10    },
	{ 3,  0x1a000011, 20    },
	{ 4,  0x18000004, 10    },
	{ 5,  0x1a00000f, 5     },
	{ 6,  0x1a000010, 3000  },
	{ 7,  0x18000010, 5     },
	{ 8,  0x70000002, 18    },
	{ 9,  0x1a000028, 5     },
	{ 10, 0x1a000002, 5     },
	{ 11, 0x1a000010, 5000  },
	{ 12, 0x18000011, 10    },
	{ 13, 0x70000003, 18    },
	{ 14, 0x18000025, 10    },
	{ 15, 0x1a000002, 5     },
	{ 16, 0x1a000010, 10000 },
	{ 17, 0x7cc00003, 1     },
	{ 18, 0x1a00003d, 3     },
	{ 19, 0x18000025, 20    },
	{ 20, 0x1a000002, 5     },
	{ 21, 0x1a000033, 1     },
};

IMPLEMENT_OBJECT(FrLevelupItemForm, FrForm)

BEGIN_FRESH_MSGMAP(FrLevelupItemForm, FrForm)

ON_FRESH_VI("levelupitem", FRCMD_INIT, FrLevelupItemForm::OnLevelUpItemInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrLevelupItemForm::OnCloseUp)
ON_FRESH_VI("level", FRCMD_INIT, FrLevelupItemForm::OnLevelIcon)
ON_FRESH_VI("frame2", FRCMD_INIT, FrLevelupItemForm::OnFrame2Init)
ON_FRESH_VI("frame3", FRCMD_INIT, FrLevelupItemForm::OnFrame3Init)
ON_FRESH_VI("itemsend", FRCMD_INIT, FrLevelupItemForm::OnItemSendInit)
ON_FRESH_VI("leveluplist", FRCMD_INIT, FrLevelupItemForm::OnItemListInit)
ON_FRESH_VI("leveluplist", FRCMD_OWNERDRAW,
	FrLevelupItemForm::OnItemListOwnerDraw)
ON_FRESH_VI("itemexplane", FRCMD_INIT, FrLevelupItemForm::OnItemExplaneInit)

END_FRESH_MSGMAP()

FrLevelupItemForm::FrLevelupItemForm()
{
	m_pLevelIcon = NULL;
	m_pFrame2 = NULL;
	m_pFrame3 = NULL;
	m_pItemSend = NULL;
	m_pItemList = NULL;
	m_pItemExplane = NULL;
	m_level = 0;
	m_state = 0;
	m_itemTypeId = 0;
	m_itemTypeId2 = 1;
	m_bResult = false;
	static char* s_fontFile[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_fontFile;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

FrLevelupItemForm::~FrLevelupItemForm()
{
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrLevelupItemForm::OnLevelUpItemInit(int param)
{
}

void FrLevelupItemForm::OnCloseUp()
{
	Close(FrNONE, true);
}

bool FrLevelupItemForm::Close(eFormRet ret, bool bSound)
{
	return FrForm::Close(ret, bSound);
}

void FrLevelupItemForm::OnLevelIcon(int param)
{
	m_pLevelIcon = DYNAMIC_CAST(FrArea, param);
}

void FrLevelupItemForm::OnFrame2Init(int param)
{
	m_pFrame2 = DYNAMIC_CAST(FrArea, param);
}

void FrLevelupItemForm::OnFrame3Init(int param)
{
	m_pFrame3 = DYNAMIC_CAST(FrArea, param);
}

void FrLevelupItemForm::OnItemSendInit(int param)
{
	m_pItemSend = DYNAMIC_CAST(FrEdit, param);
}

void FrLevelupItemForm::OnItemListInit(int param)
{
	m_pItemList = DYNAMIC_CAST(FrListBox, param);
	m_clockIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_clockIcon.rect = WRect(50.0f, 35.0f, (float)m_clockIcon.pBitmap->Width(),
		(float)m_clockIcon.pBitmap->Height());
	m_deleteIcon.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_deleteIcon.rect =
		WRect(50.0f, 35.0f, (float)m_deleteIcon.pBitmap->Width(),
			(float)m_deleteIcon.pBitmap->Height());
}

void FrLevelupItemForm::OnItemListOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;
	unsigned long* pTid = (unsigned long*)pItem->pData;
	if (!*pTid)
		return;
	IFF_ITEM_COMMON* pInfo = ItemManager()->FindCommonItem(*pTid);
	if (!pInfo)
		return;
	unsigned char timeFlag = pInfo->TimeFlag;
	const Bitmap* pBitmap =
		g_pFresh->GetManager()->GetBitmap("ITEMS", pInfo->Icon);
	if (!pBitmap)
		return;
	float x = 25.0f, y = 30.0f, countOffset = 0.0f;
	ItemManager()->GetCouponKind(*pTid);
	if (m_level > 1)
	{
		if ((*pTid & 0xfc000000) != 0x70000000)
		{
			x = 75.0f;
			y = 30.0f;
		}
		else
		{
			x = 55.0f;
			y = 0.0f;
		}
		countOffset += 55.0f;
	}
	WRect rect(x + pItem->pos.x + 18.0f, y + pItem->pos.y + 18.0f,
		(float)pBitmap->Width(), (float)pBitmap->Height());
	pGDI->DrawTexture(pBitmap, rect, 0xffffffff, 0);
	pGDI->SetTextColor(0xff000000, 0xffffffff);
	pGDI->SetTextStyle(4);
	if (timeFlag >= 2)
	{
		pGDI->DrawTexture(m_clockIcon.pBitmap,
			m_clockIcon.rect +
				WPoint(pItem->pos.x + 15.0f, pItem->pos.y + 15.0f),
			0xffffffff, 0);
		return;
	}
	int count;
	if (m_level == 1)
		count = 10;
	else
	{
		if (m_level >= 21)
			return;
		count = s_levelUpGift[m_level].count;
		if (count <= 1)
			return;
	}
	const char* text = MakeStr("%d", count);
	float textX = pItem->pos.x + 90.0f;
	for (count = 0; count < strlen(text); ++count)
		textX -= m_pFont->GetCharWidth(g_view, text[count] - '0');
	m_pFont->Print(g_view, textX + countOffset, pItem->pos.y + 75.0f, text, 0,
		0xffffffff, NULL);
}

void FrLevelupItemForm::OnItemExplaneInit(int param)
{
	m_pItemExplane = DYNAMIC_CAST(FrEdit, param);
}

bool FrLevelupItemForm::OnInit()
{
	if (m_pLevelIcon)
	{
		char name[32];
		sprintf(name, "level_%03d", m_level + 1);
		m_pLevelIcon->SetBgImg(name);
	}
	SetItemInfo();
	if (!m_state && m_bResult)
	{
		WSendPacket send((enumClientPacket)147);
		send.Send(TO_GAME);
	}
	return true;
}

void FrLevelupItemForm::SetItemInfo()
{
	if (m_level > 21)
	{
		m_pItemSend->AddText(s_levelUpText[21][0], false, true);
		m_pItemExplane->AddText(s_levelUpText[21][1], false, true);
	}
	else
	{
		m_pItemSend->AddText(s_levelUpText[m_level][0], false, true);
		m_pItemExplane->AddText(s_levelUpText[m_level][1], false, true);
	}
	if (m_level == 1)
	{
		m_itemTypeId = 0x18000008;
		m_itemTypeId2 = 0x18000007;
		m_pItemList->AddItem(&m_itemTypeId);
		m_pItemList->AddItem(&m_itemTypeId2);
	}
	else
	{
		if (m_level > 21)
			m_itemTypeId = 0x1a000033;
		else
			m_itemTypeId = s_levelUpGift[m_level].tid;
		m_pItemList->AddItem(&m_itemTypeId);
	}
	WRect rect = m_pFrame3->GetRect();
	m_pItemExplane->MoveWindow(WPoint(rect.x + 50.0f, rect.y + 15.0f));
	if (ItemManager()->GetCouponKind(s_levelUpGift[m_level].tid) == 1)
		SetDesc(
			"     \xc8\xb9\xb5\xe6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc4\xed\xc6\xf9\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");
	else
		SetDesc(
			"     \xc8\xb9\xb5\xe6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xbc\xb1\xb9\xb0\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");
}

void FrLevelupItemForm::OnProc(const float delta)
{
}

bool FrLevelupItemForm::SetData(sLevelUpDone data)
{
	m_level = data.bLevel;
	m_state = data.bItemType;
	m_bResult = data.bDone != 0;
	return true;
}
