#include "questinfodlg.h"

#include "wfont.h"
#include "actor.h"
#include "../../shared/sharedtables.h"
#include "wresrcmng.h"
#include <string.h>

extern Fresh* g_pFresh;

extern WResourceManager* g_resrcmng;
extern WView* g_view;

struct QuestReward
{
	unsigned long flag;
	unsigned long typeId;
	unsigned long quantity;
};
static const QuestReward questRewards[20] = {
	{ 0x1,     0x1a00000f, 0x3   },
	{ 0x2,     0x18000007, 0x3   },
	{ 0x4,     0x18000005, 0x3   },
	{ 0x8,     0x18000008, 0x3   },
	{ 0x10,    0x1a000010, 0x1f4 },
	{ 0x20,    0x18000004, 0x3   },
	{ 0x40,    0x1a000010, 0x1f4 },
	{ 0x80,    0x0,        0x1   },
	{ 0x100,   0x1a00000f, 0x3   },
	{ 0x200,   0x18000028, 0x1   },
	{ 0x400,   0x18000006, 0x1   },
	{ 0x800,   0x18000007, 0x5   },
	{ 0x1000,  0x18000000, 0x4   },
	{ 0x2000,  0x18000001, 0x4   },
	{ 0x8000,  0x18000005, 0x3   },
	{ 0x10000, 0x18000005, 0x3   },
	{ 0x20000, 0x18000005, 0x3   },
	{ 0x40000, 0x18000005, 0x3   },
	{ 0x80000, 0x18000005, 0x3   },
	{ 0x80000, 0x18000005, 0x3   },
};

struct QuestText
{
	unsigned char index;
	bool complete;
	const char* name;
	const char* npc;
	const char* description;
	const char* message;
};
static QuestText questTexts[14] = {
	{ 0,  false, K2L_Compatibility("\xc5\xac\xb7\xb4\xbc\xb1\xc5\xc3"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb8\xf0\xb8\xa7\xc1\xf6\xb1\xe2 \xb8\xc5\xbb\xe7\xbf\xa3 \xb1\xe2\xc3\xca\xb0\xa1 \xc6\xb0\xc6\xb0\xc7\xd8\xbe\xdf \xc7\xcf\xb4\xc2 \xb9\xfd! \xc6\xce\xbe\xdf\xc0\xc7 \xb1\xe2\xba\xbb\xb0\xfa \xc5\xac\xb7\xb4 \xbc\xb1\xc5\xc3 \xb9\xe6\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xc7\xa5\xbd\xc3\xb5\xc8 \xb0\xf7\xbf\xa1 \xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1\012\xbf\xc5\xb1\xe2\xb1\xe2")                                         },
	{ 1,  false, K2L_Compatibility("\xc5\xb8\xb1\xb8\xb9\xe6\xb9\xfd"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xc6\xce\xbe\xdf! \xb8\xda\xc1\xf8 \xbc\xa6\xc0\xbb \xc7\xcf\xb4\xc2 \xb9\xe6\xb9\xfd\xbf\xa1 \xb4\xeb\xc7\xd8 \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xc7\xa5\xbd\xc3\xb5\xc8 \xb0\xf7\xc0\xb8\xb7\xce\012\xbc\xa6 \xc3\xc4\xba\xb8\xb1\xe2")                                                         },
	{ 2,  false, K2L_Compatibility("\xc6\xce\xbe\xdf\xb0\xd4\xc0\xcc\xc1\xf6"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb4\xf5 \xb8\xd6\xb8\xae! \xb4\xf5 \xb0\xad\xc7\xcf\xb0\xd4! \xc6\xce\xbe\xdf\xc4\xde\xba\xb8\xb0\xd4\xc0\xcc\xc1\xf6\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb \xc3\xc4\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xc6\xce\xbe\xdf\xc4\xde\xba\xb8\xb0\xd4\xc0\xcc\xc1\xf6\xb8\xa6\012\xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc6\xc4\xbf\xf6\xbc\xa6 \xc4\xa1\xb1\xe2") },
	{ 3,  false, K2L_Compatibility("\xc6\xdb\xc6\xc3\xc0\xc7 \xb1\xe2\xba\xbb"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xc8\xa6\xc0\xc7 \xb8\xb6\xb9\xab\xb8\xae! \xc6\xdb\xc6\xc3\xbf\xa1 \xb4\xeb\xc7\xd8 \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda"),
     K2L_Compatibility("\xc6\xdb\xc6\xc3 \xb9\xe8\xbf\xec\xb1\xe2")																						   },
	{ 4,  false, K2L_Compatibility("\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xbe\xd1, \xc0\xcc\xb7\xb1 \xbd\xc5\xb1\xe2\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc? \xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe7\xbf\xeb\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe7\xbf\xeb\xc0\xb8\xb7\xce\012\xc6\xc4\xbf\xf6\xbc\xa6 \xc4\xa1\xb1\xe2")                                         },
	{ 5,  false, K2L_Compatibility("\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xc7\xae\xb9\xe7, \xb8\xf0\xb7\xa1\xb9\xe7, \xb9\xd9\xb4\xd9, \xbf\xac\xb8\xf8... \xb4\xd9\xbe\xe7\xc7\xd1 \xc1\xf6\xb8\xe9\xbf\xa1 \xb4\xeb\xc7\xd8 \xb0\xf8\xba\xce\xc7\xcf\xc0\xda!"),
     K2L_Compatibility(
			"\xc4\xfb\xc1\xee\xb7\xce \xbe\xcb\xbe\xc6\xba\xb8\xb4\xc2\012\xc1\xf6\xb8\xe9\xc0\xc7 \xbc\xd3\xbc\xba")                                         },
	{ 6,  false, K2L_Compatibility("\xbd\xba\xc4\xda\xbe\xee\xb0\xe8\xbb\xea"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xbe\xcb\xbd\xf6\xb4\xde\xbd\xf6? \xc6\xce\xbe\xdf\xc0\xc7 \xc1\xa1\xbc\xf6 \xb0\xe8\xbb\xea\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xc4\xfb\xc1\xee\xb7\xce \xbe\xcb\xbe\xc6\xba\xb8\xb4\xc2\012\xc6\xce\xbe\xdf\xc0\xc7 \xc1\xa1\xbc\xf6")                                         },
	{ 7,  false,
     K2L_Compatibility(
			"4\xc8\xa6\xbf\xa1\xbc\xad \xc8\xa6\xc0\xce\xc7\xcf\xb1\xe2"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xc3\xca\xba\xb8 \xb4\xeb\xc5\xbb\xc3\xe2! \xc1\xf6\xb1\xdd\xb1\xee\xc1\xf6 \xb9\xe8\xbf\xee \xb8\xf0\xb5\xe7 \xb0\xcd\xc0\xbb \xc0\xc0\xbf\xeb\xc7\xcf\xbf\xa9 \xbd\xc7\xbd\xc0 \xb5\xb5\xc0\xfc!"),
     K2L_Compatibility(
			"\xc6\xc4 4\xc8\xa6\xbf\xa1\xbc\xad\012Par\xc0\xcc\xbb\xf3 \xb1\xe2\xb7\xcf\xc7\xcf\xb1\xe2")                                                     },
	{ 8,  false,
     K2L_Compatibility("\xb0\xf8 \xbf\xc5\xb1\xe2\xb1\xe2"),                     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"), K2L_Compatibility("\xb5\xb5\xc0\xfa\xc8\xf7 \xbc\xa6\xc0\xbb \xc4\xa5 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc8\xb2\xc0\xcc\xb6\xf3\xb8\xe9? \xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2 \xb1\xe2\xb4\xc9\xc0\xbb \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2 \xb1\xe2\xb4\xc9 \xbe\xcb\xbe\xc6\xba\xb8\xb1\xe2")                                                             },
	{ 9,  false, K2L_Compatibility("\xb0\xf8 \xb1\xe2\xbf\xef\xb1\xe2"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xc1\xf6\xb8\xe9\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2\xb8\xa6 \xc0\xd0\xb0\xed \xb4\xf5\xbf\xed \xc1\xa4\xb1\xb3\xc7\xd1 \xbc\xa6\xbf\xa1 \xb5\xb5\xc0\xfc\xc7\xcf\xc0\xda!"),
     K2L_Compatibility(
			"\xc1\xf6\xb8\xe9\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2\xb8\xa6 \xc0\xd0\xb4\xc2 \xb9\xfd")                                                            },
	{ 10, false, K2L_Compatibility("\xb9\xd9\xb6\xf7 \xc0\xd0\xb1\xe2"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb9\xd9\xb6\xf7\xc0\xbb \xc0\xd0\xb0\xed \xb0\xf8\xc0\xc7 \xb1\xcb\xc0\xfb\xc0\xbb \xbf\xb9\xc3\xf8\xc7\xcf\xb6\xf3!"),
     K2L_Compatibility(
			"\xb9\xd9\xb6\xf7\xc0\xc7 \xbc\xbc\xb1\xe2\xbf\xcd \xb9\xe6\xc7\xe2 \xbe\xcb\xbe\xc6\xba\xb8\xb1\xe2")                                            },
	{ 11, false, K2L_Compatibility("\xc5\xb8\xc1\xa1\xc0\xcc\xb5\xbf"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb0\xf8\xc0\xc7 \xc5\xb8\xc1\xa1\xc0\xbb \xc1\xb6\xc0\xfd\xc7\xcf\xbf\xa9 \xb1\xcb\xc0\xfb\xc0\xc7 \xb0\xa2\xb5\xb5\xb8\xa6 \xb9\xd9\xb2\xe3\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xb0\xf8\xc0\xc7 \xc5\xb8\xc1\xa1 \xc1\xb6\xc0\xfd\xc7\xcf\xb1\xe2")                                                                             },
	{ 12, false, K2L_Compatibility("\xc6\xc4\xbf\xf6\xbd\xba\xc7\xc9"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb0\xad\xb7\xc2\xc7\xd1 \xbd\xba\xc7\xc9! \xc6\xc4\xbf\xf6\xbd\xba\xc7\xc9\xbf\xa1 \xb4\xeb\xc7\xd8 \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda."),
     K2L_Compatibility(
			"\xc6\xc4\xbf\xf6 \xbd\xba\xc7\xc9 \xbc\xa6 \xc0\xcd\xc8\xf7\xb1\xe2")                                                                            },
	{ 13, false, K2L_Compatibility("\xc6\xc4\xbf\xf6\xc4\xbf\xba\xea"),
     K2L_Compatibility("\xba\xc0\xb4\xd9\xb8\xae"),
     K2L_Compatibility(
			"\xb0\xc5\xb4\xeb\xc7\xd1 \xc0\xe5\xbe\xd6\xb9\xb0\xb5\xb5 \xbf\xe4\xb8\xae\xc1\xb6\xb8\xae \xba\xf1\xc4\xd1\xb0\xa1\xb4\xc2 \xc6\xc4\xbf\xf6 \xc4\xbf\xba\xea\xbc\xa6\xc0\xbb \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda!"),
     K2L_Compatibility(
			"\xc0\xe5\xbe\xd6\xb9\xb0\xc0\xbb \xc7\xc7\xc7\xd8 \xb8\xf1\xc7\xa5\xc1\xf6\xc1\xa1\xc0\xb8\xb7\xce \xb0\xf8 \xba\xb8\xb3\xbb\xb1\xe2")           },
};

IMPLEMENT_ACTOR(FrQuestInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrQuestInfoDlg, FrForm)

ON_FRESH_VI("quest_info", FRCMD_INIT, FrQuestInfoDlg::OnQuestInfoInit)
ON_FRESH_VI("reward_item", FRCMD_INIT, FrQuestInfoDlg::OnRewardItemListInit)
ON_FRESH_VI("reward_item", FRCMD_OWNERDRAW,
	FrQuestInfoDlg::OnRewardItemListOwnerDraw)
ON_FRESH_VI("name", FRCMD_INIT, FrQuestInfoDlg::OnQuestNameEditInit)
ON_FRESH_VI("message", FRCMD_INIT, FrQuestInfoDlg::OnQuestMessageEditInit)
ON_FRESH_VI("explane", FRCMD_INIT, FrQuestInfoDlg::OnQuestExplaneEditInit)

END_FRESH_MSGMAP()

FrQuestInfoDlg::FrQuestInfoDlg()
{
	m_pRewardItemList = NULL;
	m_pItemFrame = NULL;
	m_bReward = false;
	m_questIndex = 0;
	m_pQuestName = NULL;
	m_pQuestMessage = NULL;
	m_pQuestExplane = NULL;
	static char* filenames[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = filenames;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

FrQuestInfoDlg::~FrQuestInfoDlg()
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void FrQuestInfoDlg::OnQuestInfoInit(int)
{
}

void FrQuestInfoDlg::OnRewardItemListInit(int param)
{
	m_pRewardItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	if (m_pRewardItemList)
	{
		m_pRewardItemList->SetPushSound(NULL);
		m_pRewardItemList->ClearItem();
		m_pItemFrame = g_pFresh->RegisterBitmap("re_itemframe_n");
	}
}

void FrQuestInfoDlg::OnRewardItemListOwnerDraw(int param)
{
	if (!param)
		return;
	FrListItem* item = (FrListItem*)param;
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	float offsetX = 0.0f, offsetY = 0.0f;
	if (m_pRewardItemList->m_itemList.size() > 1)
	{
		switch (item->idx)
		{
		case 0:
			offsetY = 7.0f;
			offsetX = 20.0f;
			break;
		case 1:
			offsetY = 7.0f;
			offsetX = 20.0f;
			break;
		}
	}
	else
	{
		offsetY = 7.0f;
		offsetX = 100.0f;
	}
	device->SetTextStyle(0);
	if (m_pItemFrame)
		device->DrawTexture(m_pItemFrame,
			WRect(item->pos.x + offsetX, item->pos.y + offsetY,
				(float)m_pItemFrame->Width(), (float)m_pItemFrame->Height()),
			0xffffffff, 0);
	unsigned long typeId = (unsigned long)item->pData;
	if (typeId)
	{
		const char* icon = "caddie_no";
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
		if (common)
		{
			device->SetTextStyle(1);
			WPoint pos(item->pos.x + 160.0f, item->pos.y + 7.0f);
			pos.x += offsetX;
			pos.y += offsetY;
			device->Print(pos, 2, "%s", common->Name);
			device->SetTextStyle(0);
			icon = common->Icon;
		}
		const Bitmap* bitmap = g_pFresh->GetBitmap(icon);
		if (bitmap)
		{
			float x, y;
			switch (typeId >> 26)
			{
			case 1:
			case 2:
			case 8:
			case 9:
			case 15:
			case 16:
			case 28:
				x = 7.0f;
				y = 2.0f;
				break;

			case 11:
				x = 9.0f;
				y = 12.0f;
				break;
			case 14:
				if ((typeId & 0x3c00000) == 0x800000)
				{
					x = 7.0f;
					y = 12.0f;
				}
				else
				{
					x = 16.0f;
					y = 27.0f;
				}
				break;
			default:
				x = 10.0f;
				y = 20.0f;
				break;
			}
			device->DrawTexture(bitmap,
				WRect((item->pos.x + x) + offsetX, (item->pos.y + y) + offsetY,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
			{
				int count = questRewards[m_questIndex].quantity;
				const char* quantity = MakeStr("%d", count);
				float posX = item->pos.x + 70.0f;
				for (count = 0; count < strlen(quantity); ++count)
					posX -= m_pTitleFont->GetCharWidth(g_view,
						quantity[count] - '0');
				m_pTitleFont->Print(g_view, posX + offsetX - 10.0f,
					item->pos.y + offsetY + 50.0f, quantity, 0, 0xffffffff,
					NULL);
			}
		}
	}
}

void FrQuestInfoDlg::OnQuestNameEditInit(int param)
{
	m_pQuestName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrQuestInfoDlg::OnQuestMessageEditInit(int param)
{
	m_pQuestMessage = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrQuestInfoDlg::OnQuestExplaneEditInit(int param)
{
	m_pQuestExplane = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrQuestInfoDlg::SetQuestExplane(int index)
{
	m_pQuestName->AddText(MakeStr("%d : %s", index + 1, questTexts[index].name),
		false, true);
	m_pQuestMessage->AddText(questTexts[index].message, false, true);
	m_pQuestExplane->AddText(questTexts[index].description, false, true);
	if (questRewards[index].typeId)
	{
		m_pRewardItemList->AddItem((void*)questRewards[index].typeId);
		m_bReward = true;
	}
	else if (index == 7)
	{
		if (Doc()->m_myInfo.info.gender % 2 == 0)
			m_pRewardItemList->AddItem((void*)0x800a010);
		else
			m_pRewardItemList->AddItem((void*)0x8048009);
		m_bReward = true;
	}
	m_questIndex = index;
}

IMPLEMENT_ACTOR(FrQuestGift, FrForm)

BEGIN_FRESH_MSGMAP(FrQuestGift, FrForm)

ON_FRESH_VI("reward_item", FRCMD_INIT, FrQuestGift::OnGiftListInit)
ON_FRESH_VI("reward_item", FRCMD_OWNERDRAW, FrQuestGift::OnGiftListOwnerDraw)
ON_FRESH_VV("reward_item", FRCMD_LBUTTONDOWN, FrQuestGift::OnGiftListLBtnDown)
ON_FRESH_VI("select", FRCMD_INIT, FrQuestGift::OnSelectInit)
ON_FRESH_VI("itemback4", FRCMD_INIT, FrQuestGift::OnItemBack4)
ON_FRESH_VI("itemback5", FRCMD_INIT, FrQuestGift::OnItemBack5)
ON_FRESH_VI("itemback6", FRCMD_INIT, FrQuestGift::OnItemBack6)
ON_FRESH_VI("itemback7", FRCMD_INIT, FrQuestGift::OnItemBack7)
ON_FRESH_VI("itemback8", FRCMD_INIT, FrQuestGift::OnItemBack8)
ON_FRESH_VI("itemback9", FRCMD_INIT, FrQuestGift::OnItemBack9)
ON_FRESH_VI("ok", FRCMD_INIT, FrQuestGift::OnOkInit)

END_FRESH_MSGMAP()

FrQuestGift::FrQuestGift()
	: m_pGiftList(NULL), m_pSelect(NULL), m_pOk(NULL)
{
	memset(m_pItemBack, 0, sizeof(m_pItemBack));
	m_questIndex = 0;
	m_pItemFrame = NULL;
	m_pNotify = NULL;
	m_bDataSet = false;
	static char* filenames[] = { "[font_wind.jpg" };
	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = filenames;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

FrQuestGift::~FrQuestGift()
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void FrQuestGift::OnGiftListInit(int param)
{
	m_pGiftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pItemFrame = g_pFresh->RegisterBitmap("re_itemframe_n");
}

void FrQuestGift::OnGiftListOwnerDraw(int param)
{
	if (!param)
		return;
	FrListItem* item = (FrListItem*)param;
	FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
	float offsetX = 0.0f, offsetY = 0.0f;
	if (m_pGiftList->m_itemList.size() == 2)
	{
		switch (item->idx)
		{
		case 0:
			offsetY = 7.0f;
			offsetX = 5.0f;
			break;
		case 1:
			offsetY = 7.0f;
			offsetX = 70.0f;
			break;
		}
	}
	else if (m_pGiftList->m_itemList.size() == 3)
	{
		switch (item->idx)
		{
		case 0:
			offsetY = 7.0f;
			offsetX = 5.0f;
			break;
		case 1:
			offsetY = 7.0f;
			offsetX = 70.0f;
			break;
		case 2:
			offsetX = 5.0f;
			offsetY = 10.0f;
			break;
		}
	}
	else
	{
		offsetY = 7.0f;
		offsetX = 100.0f;
	}
	device->SetTextStyle(0);
	if (m_pItemFrame)
		device->DrawTexture(m_pItemFrame,
			WRect(item->pos.x + offsetX, item->pos.y + offsetY,
				(float)m_pItemFrame->Width(), (float)m_pItemFrame->Height()),
			0xffffffff, 0);
	unsigned long typeId = (unsigned long)item->pData;
	if (typeId)
	{
		const char* icon = "caddie_no";
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
		if (common)
		{
			device->SetTextStyle(1);
			WPoint pos(item->pos.x + 160.0f, item->pos.y + 7.0f);
			pos.x += offsetX;
			pos.y += offsetY;
			device->Print(pos, 2, "%s", common->Name);
			device->SetTextStyle(0);
			icon = common->Icon;
		}
		const Bitmap* bitmap = g_pFresh->GetBitmap(icon);
		if (bitmap)
		{
			float x, y;
			switch (typeId >> 26)
			{
			case 2:
				x = 7.0f;
				y = -5.0f;
				break;
			case 1:
			case 8:
			case 9:
			case 15:
			case 16:
			case 28:
				x = 7.0f;
				y = 2.0f;
				break;
			case 11:
				x = 9.0f;
				y = 12.0f;
				break;
			case 14:
				if ((typeId & 0x3c00000) == 0x800000)
				{
					x = 7.0f;
					y = 12.0f;
				}
				else
				{
					x = 16.0f;
					y = 27.0f;
				}
				break;
			default:
				x = 10.0f;
				y = 20.0f;
				break;
			}
			device->DrawTexture(bitmap,
				WRect((item->pos.x + x) + offsetX, (item->pos.y + y) + offsetY,
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
			if (m_bDataSet != true && questRewards[m_questIndex].quantity != 1)
			{
				int count = questRewards[m_questIndex].quantity;
				const char* quantity = MakeStr("%d", count);
				float posX = item->pos.x + 70.0f;
				for (count = 0; count < strlen(quantity); ++count)
					posX -= m_pTitleFont->GetCharWidth(g_view,
						quantity[count] - '0');
				m_pTitleFont->Print(g_view, posX + offsetX - 10.0f,
					item->pos.y + offsetY + 50.0f, quantity, 0, 0xffffffff,
					NULL);
			}
		}
	}
}

void FrQuestGift::OnGiftListLBtnDown()
{
	if (m_pGiftList && !m_bDataSet && m_pGiftList->m_itemList.size() != 1)
	{
		FrListItem* item = m_pGiftList->GetItemUnderCursor();
		if (item)
		{
			unsigned long typeId = (unsigned long)item->pData;
			if (typeId)
			{
				WSendPacket packet((enumClientPacket)168);
				packet.Encode4(typeId);
				packet.Send(TO_GAME);
				m_pNotify =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				m_pNotify->SetMessage(
					"\xbc\xb1\xc5\xc3\xc7\xd1 \xbc\xb1\xb9\xb0\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);

				m_pNotify->Open((FRESH_PFN_RESULT)&FrQuestGift::OnNotifyResult,
					WPoint((g_view->GetWidth() - m_pNotify->GetRect().w) * 0.5f,
						80.0f),
					FrENTER);
				m_pNotify->EnableDrag(false);
				m_pNotify->MoveCursor("ok");
			}
		}
	}
}

void FrQuestGift::OnSelectInit(int param)
{
	m_pSelect = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pSelect)
		m_pSelect->SetVisible(false);
}

void FrQuestGift::OnItemBack4(int param)
{
	m_pItemBack[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnItemBack5(int param)
{
	m_pItemBack[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnItemBack6(int param)
{
	m_pItemBack[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnItemBack7(int param)
{
	m_pItemBack[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnItemBack8(int param)
{
	m_pItemBack[4] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnItemBack9(int param)
{
	m_pItemBack[5] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrQuestGift::OnOkInit(int param)
{
	m_pOk = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

bool FrQuestGift::OnNotifyResult(int, FrForm*)
{
	Close(FrOK, true);
	return true;
}

void FrQuestGift::SetQuestExplane(int index)
{
	if (questRewards[index].typeId)
		m_pGiftList->AddItem((void*)questRewards[index].typeId);
	else if (index == 7)
	{
		if (m_pSelect)
			m_pSelect->SetVisible(true);
		if (Doc()->m_myInfo.info.gender % 2 == 0)
			m_pGiftList->AddItem((void*)0x800a010);
		else
			m_pGiftList->AddItem((void*)0x8048009);
	}
	m_questIndex = index;
}

void FrQuestGift::SetData(int type)
{
	switch (type)
	{
	case 0:
		m_pGiftList->AddItem((void*)0x10000012);
		m_pGiftList->AddItem((void*)0x1c000000);
		break;
	case 1:
		if (Doc()->m_myInfo.info.gender % 2 == 0)
		{
			m_pGiftList->AddItem((void*)0x8006012);
			m_pGiftList->AddItem((void*)0x8010001);
		}
		else
		{
			m_pGiftList->AddItem((void*)0x8044013);
			m_pGiftList->AddItem((void*)0x804e002);
		}
		m_pGiftList->AddItem((void*)0x18000027);
		SetSize();
		break;
	}
	m_bDataSet = true;
}

void FrQuestGift::SetSize()
{
	int height = m_pGiftList->GetItemHeight();
	WRect rect = GetRect();
	rect.h += height;
	SetRect(rect);
	rect = m_pGiftList->GetRect();
	rect.h += height;
	m_pGiftList->SetRect(rect);
	WPoint point;
	rect = m_pOk->GetRect();
	point.x = rect.x;
	point.y = rect.y + height;
	m_pOk->MoveWindow(point);
	for (int i = 0; i < 3; ++i)
	{
		rect = m_pItemBack[i]->GetRect();
		rect.h += height + 10;
		m_pItemBack[i]->SetRect(rect);
	}
	for (int i = 3; i < 6; ++i)
	{
		rect = m_pItemBack[i]->GetRect();
		point.x = rect.x;
		point.y = rect.y + (height + 10.0f);
		m_pItemBack[i]->MoveWindow(point);
	}
}
