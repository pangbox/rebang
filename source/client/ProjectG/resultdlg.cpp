#include "resultdlg.h"
#include "exhibition.h"
#include "scoredlg.h"
#include "treasuredlg.h"
#include "treasure.h"
#include "intrusion.h"
#include "gmtoolkit.h"
#include "levelupitemdlg.h"
#include "../../shared/sharedtables.h"
#include "frwnd.h"
#include "user_info.h"
#include "userinfoform.h"
#include "netresourcemanager.h"
#include "projectg.h"
#include "inputmanager.h"
#include "mousecursor.h"
#include "../../shared/localize.h"
#include "mathconsts.h"

extern "C" __declspec(dllimport) int __cdecl mkdir(const char* path);

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrResultDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrResultDlg, FrForm)
ON_FRESH_VI("stage", FRCMD_INIT, FrResultDlg::OnStageInit)
ON_FRESH_VV("stage", FRCMD_OWNERDRAW, FrResultDlg::OnStageOwnerDraw)
ON_FRESH_VI("trophy", FRCMD_INIT, FrResultDlg::OnTrophyInit)
ON_FRESH_VI("id", FRCMD_INIT, FrResultDlg::OnIdInit)
ON_FRESH_VI("id", FRCMD_OWNERDRAW, FrResultDlg::OnIdOwnerDraw)
ON_FRESH_VI("match", FRCMD_INIT, FrResultDlg::OnMatchInit)
ON_FRESH_VI("smatch", FRCMD_INIT, FrResultDlg::OnMatchStaticInit)
ON_FRESH_VI("score", FRCMD_INIT, FrResultDlg::OnScoreInit)
ON_FRESH_VI("sscore", FRCMD_INIT, FrResultDlg::OnScoreStaticInit)
ON_FRESH_VI("pang", FRCMD_INIT, FrResultDlg::OnPangInit)
ON_FRESH_VI("bonus", FRCMD_INIT, FrResultDlg::OnBonusInit)
ON_FRESH_VI("sbonus", FRCMD_INIT, FrResultDlg::OnBonusStaticInit)
ON_FRESH_VI("getitem", FRCMD_INIT, FrResultDlg::OnGetItemInit)
ON_FRESH_VV("getitem", FRCMD_OWNERDRAW, FrResultDlg::OnGetItemOwnerDraw)
ON_FRESH_VI("level", FRCMD_INIT, FrResultDlg::OnLevelInit)
ON_FRESH_VI("exp", FRCMD_INIT, FrResultDlg::OnExpInit)
ON_FRESH_VI("exp_gb", FRCMD_INIT, FrResultDlg::OnExpGaugeBarInit)
ON_FRESH_VI("levelup", FRCMD_INIT, FrResultDlg::OnLevelUpInit)
END_FRESH_MSGMAP()

FrResultDlg::FrResultDlg()
{
	m_expTime = 0.0f;
	ResetVars();
	FindWinner();
	static char* files[] = { "[bigfont.jpg" };
	tagWTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 32;
	font.fonth = 32;
	font.numPages = 1;
	font.texw = 256;
	font.texh = 128;
	font.pCharSet = "1234567890ym%-./x ";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&font);
	if (IsLocalContent((localContentType_t)0x17))
		m_pHalloweenOverlay =
			g_resrcmng->GetOverlay(MakeStr("%s.tga", "z_Halloween_Male"), 0);
}

FrResultDlg::~FrResultDlg()
{
	if (IsLocalContent((localContentType_t)0x17) && g_resrcmng &&
		m_pHalloweenOverlay)
	{
		g_resrcmng->Release(m_pHalloweenOverlay);
		m_pHalloweenOverlay = NULL;
	}
	if (m_pPartTidList)
	{
		delete m_pPartTidList;
		m_pPartTidList = NULL;
	}
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void FrResultDlg::ResetVars()
{
	m_pStage = NULL;
	m_pLevel = NULL;
	m_pExp = NULL;
	m_pExpGaugeBar = NULL;
	m_pExhibition = NULL;
	m_pLevelUp = NULL;
	m_pPartTidList = NULL;
	memset(&m_winner, 0, sizeof(m_winner));
}

void FrResultDlg::FindWinner()
{
	memset(Doc()->m_teamResult, 0, sizeof(Doc()->m_teamResult));
	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if (it->rank == 1)
		{
			m_winner = *it;
			if (Doc()->m_roomInfo.gameType == 4 ||
				Doc()->m_roomInfo.gameType == 10)
				return;
		}
		Doc()->m_teamResult[it->team].score += it->rank;
		Doc()->m_teamResult[it->team].pang += it->totalPang;
	}
}

bool FrResultDlg::OnInit()
{
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		m_startLevel = m_level = Doc()->m_userInfo[0].stat.Level;
		m_exp = Doc()->m_userInfo[0].stat.dwExp;
	}
	else
	{
		m_startLevel = m_level = Doc()->m_myInfo.stat.Level;
		m_exp = Doc()->m_myInfo.stat.dwExp;
	}
	m_addExp = Doc()->m_holeStatistics[0].dwExp;
	int maxExp = Doc()->m_levelTable[m_level].exp;
	m_bExpDone = m_addExp == 0;
	m_exp *= 10;
	m_addExp *= 10;
	maxExp *= 10;
	if (m_pExp)
	{
		char buffer[32];
		sprintf(buffer, "%d / %d", m_exp / 10, maxExp / 10);
		m_pExp->SetCaption(buffer);
	}
	if (m_pLevel)
	{
		m_pLevel->SetBgImg(MakeStr("level_%03d", m_level + 1));
	}
	if (m_pExpGaugeBar)
	{
		m_pExpGaugeBar->SetBorder(1);
		m_pExpGaugeBar->SetRange(0, maxExp, 0);
		m_pExpGaugeBar->SetPos(m_exp);
	}
	SetDesc(MakeStr(
		"%49s1\xb5\xee\xc0\xba %s\xb4\xd4\xc0\xcc \xc2\xf7\xc1\xf6\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
		"", m_winner.nickname));
	if (!Doc()->m_bSentHoleStat)
	{
		Doc()->m_bSentHoleStat = true;
		if (!(bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
		{
			WSendPacket packet((enumClientPacket)6);
			packet.EncodeBuffer(&Doc()->m_holeStatistics[0],
				sizeof(sPangYaUserStatistics));
			packet.Send(TO_GAME);
			Doc()->Log30sScore();
		}
	}
	return true;
}

void FrResultDlg::OnProc(const float delta)
{
	m_expTime += delta;
	if (m_pExhibition)
	{
		const WRect& rect = m_pStage->GetRect();
		if (rect.x != m_stageRect.x || rect.y != m_stageRect.y)
		{
			m_pExhibition->SetCenter(rect.x + rect.w * .5f,
				rect.y + rect.h * .5f);
			m_stageRect = rect;
		}
		m_pExhibition->Process(delta, true);
	}
	if (m_pExpGaugeBar && !m_bExpDone)
	{
		if (m_expTime < 1.f)
			return;
		if (m_expTime - 1.f < .01f)
			return;
		int amount = float2int((m_expTime - 1.f) * 100.f);
		m_expTime -= amount * .01f;
		int maxExp = Doc()->m_levelTable[m_level].exp * 10;
		if (amount > (m_addExp > 0 ? m_addExp : -m_addExp))
		{
			m_bExpDone = true;
			m_exp +=
				(m_addExp > 0 ? 1 : -1) * (m_addExp > 0 ? m_addExp : -m_addExp);
			m_addExp = 0;
		}
		else
		{
			m_exp += (m_addExp > 0 ? 1 : -1) * amount;
			m_addExp -= (m_addExp > 0 ? 1 : -1) * amount;
		}
		if (m_exp < 0 && !m_bExpDone)
		{
			m_bExpDone = true;
			m_exp = 0;
		}
		char buffer[32];
		if (m_exp >= maxExp)
		{
			m_exp -= maxExp;
			++m_level;
			maxExp = Doc()->m_levelTable[m_level].exp * 10;
			m_pExpGaugeBar->SetRange(0, maxExp, 0);
			m_pExpGaugeBar->SetPos(m_exp);
			if (m_pLevel)
			{
				sprintf(buffer, "level_%03d", m_level + 1);
				m_pLevel->SetBgImg(buffer);
			}
		}
		else
			m_pExpGaugeBar->SetPos(m_exp);
		if (m_pExp)
		{
			sprintf(buffer, "%d / %d", m_exp / 10, maxExp / 10);
			m_pExp->SetCaption(buffer);
		}
	}
	else if (m_bExpDone && m_pLevelUp && m_startLevel < m_level)
		m_pLevelUp->SetVisible(true);
}

void FrResultDlg::OnStageInit(int param)
{
	m_pStage = DYNAMIC_CAST(FrArea, param);
	if (m_pStage)
	{
		char buffer[64] = "";
		sprintf(buffer, "30in_back_%s", GetCurMapName());
		m_pStage->SetBgImg(buffer);
	}
	ShowPet();
}

void FrResultDlg::OnStageOwnerDraw()
{
	char buffer[64] = "";
	sprintf(buffer, "30in_back_%s", GetCurMapName());
	m_pStage->SetBgImg(buffer);
	if (m_pExhibition)
		m_pExhibition->Display(0, 0);
	const WRect& rect = m_pStage->GetRect();
	WPoint pos(rect.x + 333.f, rect.y + 34.f);
	const char* text;
	if (Doc()->m_roomInfo.gameType == 10 &&
		(Doc()->m_rivalList[MyIndex()].approachTime == -1 ||
			Doc()->m_rivalList[MyIndex()].approachResultDistance == -1))
		text = MakeStr("-");
	else
		text = MakeStr("%d", Doc()->m_rivalList[MyIndex()].rank);
	if (m_pTitleFont)
		m_pTitleFont->Print(g_view, pos.x, pos.y, text, 0, -1, NULL);
}

void FrResultDlg::OnTrophyInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (!area)
		return;
	if (Doc()->m_eventType)
	{
		IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(Doc()->m_roomType);
		if (!match || Doc()->m_eventType < 1 || Doc()->m_eventType > 3)
			area->SetVisible(false);
		else
			area->SetBgImg(match->Icon[Doc()->m_eventType - 1]);
		const char* text = MakeStr("%d", Doc()->m_rivalList[MyIndex()].rank);
		float width = m_pTitleFont->GetTextWidth(g_view, text);
		WPoint pos;
		pos.x = width + GetRect().x + 340.f;
		if (Doc()->m_golfGame.gameType == 5)
			pos.y = GetRect().y + 100.f;
		else
			pos.y = GetRect().y + 40.f;
		area->MoveWindow(pos);
	}
	else
	{
		area->SetVisible(false);
	}
}

void FrResultDlg::OnMatchStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text && Doc()->m_roomInfo.gameType == 10)
		text->SetCaption("\xb3\xb2\xc0\xba \xb0\xc5\xb8\xae :");
}

void FrResultDlg::OnMatchInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_roomInfo.gameType == 10)
	{
		if (Doc()->m_rivalList[MyIndex()].approachTime == -1)
			text->SetCaption("Out");
		else
		{
			unsigned int distance =
				Doc()->m_rivalList[MyIndex()].approachResultDistance;
			if (distance == -1)
				text->SetCaption("Out");
			else
				text->SetCaption(
					MakeStr("%d.%d y", distance / 10, distance % 10));
		}
	}
	else
	{
		IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(Doc()->m_roomType);
		if (!match)
			return;
		char buffer[80] = "";
		if (Doc()->m_roomInfo.realGameType == 14)
		{
			const char* name = "\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba";
			strcpy(buffer, name);
		}
		else
			strcpy(buffer, match->Name);
		text->SetCaption(buffer);
	}
}

void FrResultDlg::OnIdInit(int param)
{
	m_pId = DYNAMIC_CAST(FrArea, param);
}

void FrResultDlg::OnIdOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float offset = 0.f;
	sRivalData& rival = Doc()->m_rivalList[MyIndex()];
	if (rival.guildUID)
	{
		const Bitmap* emblem =
			NetResourceManager::Instance()->GetEmblemByName(rival.guildMark);
		if (emblem)
		{
			graphic->DrawTexture(emblem,
				WRect(m_pId->GetRect().x, m_pId->GetRect().y - 6.f,
					emblem->Width(), emblem->Height()),
				0xffffffff, 0);
			offset = 27.f;
		}
	}
	if (!CProjectG::Instance()->HidePrivacy())
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
		g_pFresh->GetManager()->PrintText(
			WPoint(offset + m_pId->GetRect().x, m_pId->GetRect().y), 0,
			rival.nickname, -1.f, 0xffffffff);
	}
}

void FrResultDlg::OnScoreStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text && Doc()->m_roomInfo.gameType == 10)
		text->SetCaption("\xb3\xb2\xc0\xba \xbd\xc3\xb0\xa3 :");
}

void FrResultDlg::OnScoreInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_roomInfo.gameType == 10)
	{
		int time = Doc()->m_rivalList[MyIndex()].approachTime;
		if (time == -1)
			text->SetCaption("-");
		else
			text->SetCaption(
				MakeStr("%d.%03d \xc3\xca", time / 1000, time % 1000));
	}
	else
	{
		const char* format =
			Doc()->m_rivalList[MyIndex()].totalScore > 0 ? "+%d" : "%d";
		text->SetCaption(
			MakeStr(format, Doc()->m_rivalList[MyIndex()].totalScore));
	}
}

void FrResultDlg::OnPangInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_roomInfo.gameType == 10)
	{
		text->SetCaption("0");
		for (int i = 6; i < 12; ++i)
			if (Doc()->m_awardItem[i].uid == MyGuid(true))
			{
				text->SetCaption(MakeStr("%d", Doc()->m_awardItem[i].pang));
				break;
			}
	}
	else
	{
		text->SetCaption(
			MakeStr("%I64d", Doc()->m_rivalList[MyIndex()].totalPang));
	}
}

void FrResultDlg::OnBonusStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text && Doc()->m_roomInfo.gameType == 10)
		text->SetVisible(false);
}

void FrResultDlg::OnBonusInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_roomInfo.gameType == 10)
		text->SetVisible(false);
	__int64 bonus = (int)((float)(unsigned int)Doc()->m_expRate *
			(float)(Doc()->m_rivalList[MyIndex()].totalPang +
				Doc()->m_rivalList[MyIndex()].totalBonusPang) *
			Doc()->m_pangRate * .01f -
		(float)Doc()->m_rivalList[MyIndex()].totalPang);
	text->SetCaption(MakeStr("%I64d", (__int64)bonus));
}

void FrResultDlg::OnGetItemInit(int param)
{
	m_pGetItem = DYNAMIC_CAST(FrArea, param);
}

void FrResultDlg::OnGetItemOwnerDraw()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float offset = 0.f;
	for (int i = 0; i < 12; ++i)
	{
		if (Doc()->m_awardItem[i].uid == MyGuid(true) &&
			Doc()->m_awardItem[i].pang)
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(Doc()->m_awardItem[i].pang);
			const Bitmap* bitmap = NULL;
			switch ((unsigned long)Doc()->m_awardItem[i].pang >> 26)
			{
			case 2:
				bitmap =
					g_pFresh->GetManager()->GetBitmap("FASHION", item->Icon);
				break;
			case 5:
				bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
				break;
			case 6:
				bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
				break;
			}
			if (bitmap)
			{
				graphic->DrawTexture(bitmap,
					WRect(offset + m_pGetItem->GetRect().x,
						m_pGetItem->GetRect().y, bitmap->Width(),
						bitmap->Height()),
					0xffffffff, 0);
				offset += 50.f;
			}
		}
	}
	if (IsLocalContent((localContentType_t)0x17) &&
		Doc()->IsWearHalloweenItem())
	{
		if (m_pHalloweenOverlay)
		{
			m_pHalloweenOverlay->Render(g_view, WRect(0, 0, 1, 1),
				WRect(offset + m_pId->GetRect().x + 150.f,
					m_pId->GetRect().y + 5.f, m_pHalloweenOverlay->GetWidth(),
					m_pHalloweenOverlay->GetHeight()),
				0, 0xffffffff, 0, 0);
		}
		graphic->SetTextStyle(1);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetScale(2.f);
		g_pFresh->GetManager()->PrintText(
			WPoint(m_pId->GetRect().x + 141.f, m_pId->GetRect().y + 41.f), 0,
			"+100", -1.f, 0xffffffff);
		graphic->SetTextColor(0xffffffff, 0xffffffff);
		g_pFresh->GetManager()->PrintText(
			WPoint(m_pId->GetRect().x + 140.f, m_pId->GetRect().y + 40.f), 0,
			"+100", -1.f, 0xffffffff);
		graphic->SetScale(1.f);
	}
	for (std::vector<unsigned long>::iterator it =
			 Doc()->m_acquireItemList[0].begin();
		it != Doc()->m_acquireItemList[0].end(); ++it)
	{
		const Bitmap* bitmap = NULL;
		if (*it >> 26 == 6)
		{
			IFF_STRUCT::sItem* item = ItemManager()->FindItem(*it);
			if (item)
				bitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", item->c.Icon);
		}
		else if (*it >> 26 == 29)
		{
			IFF_STRUCT::sQuestDrop* item = ItemManager()->FindQuestDrop(*it);
			if (item)
				bitmap = g_pFresh->GetManager()->GetBitmap("QUESTDROP",
					item->c.Icon);
		}
		if (bitmap)
		{
			graphic->DrawTexture(bitmap,
				WRect(offset + m_pGetItem->GetRect().x - 11.f,
					m_pGetItem->GetRect().y - 5.f, bitmap->Width(),
					bitmap->Height()),
				0xffffffff, 0);
			offset += 50.f;
		}
	}
}

void FrResultDlg::OnLevelInit(int param)
{
	m_pLevel = DYNAMIC_CAST(FrArea, param);
}

void FrResultDlg::OnExpInit(int param)
{
	m_pExp = DYNAMIC_CAST(FrStatic, param);
}

void FrResultDlg::OnExpGaugeBarInit(int param)
{
	m_pExpGaugeBar = DYNAMIC_CAST(FrGaugeBar, param);
}

void FrResultDlg::OnLevelUpInit(int param)
{
	m_pLevelUp = DYNAMIC_CAST(FrArea, param);
	if (m_pLevelUp)
		m_pLevelUp->SetVisible(false);
}

void FrResultDlg::ShowPet()
{
	if (m_pExhibition)
	{
		delete m_pExhibition;
		m_pExhibition = NULL;
	}
	if (m_pPartTidList)
	{
		delete m_pPartTidList;
		m_pPartTidList = NULL;
	}
	if (!m_pStage)
		return;
	WRect rect = m_pStage->GetRect();
	m_pExhibition = new CExhibition(rect.x + rect.w * .5f,
		rect.y + rect.h * .5f, .33f, false);
	m_pExhibition->SetDistance(20.f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);
	unsigned char index = MyIndex();
	unsigned char motion;
	if (index < Doc()->m_rivalList.size())
	{
		if (Doc()->m_golfGame.gameType == 5)
		{
			if ((unsigned)Doc()->m_teamResult[0].score <
					(unsigned)Doc()->m_teamResult[1].score ||
				(Doc()->m_teamResult[0].score == Doc()->m_teamResult[1].score &&
					Doc()->m_teamResult[0].pang > Doc()->m_teamResult[1].pang))
				motion = Doc()->m_rivalList[MyIndex()].team == 0 ? 1 : 3;
			else if ((unsigned)Doc()->m_teamResult[0].score >
					(unsigned)Doc()->m_teamResult[1].score ||
				(Doc()->m_teamResult[0].score == Doc()->m_teamResult[1].score &&
					Doc()->m_teamResult[0].pang < Doc()->m_teamResult[1].pang))
				motion = Doc()->m_rivalList[MyIndex()].team == 1 ? 1 : 3;
			else
				motion = 2;
		}
		else if (Doc()->m_golfGame.gameType == 6)
		{
			if (Doc()->m_eventFlag == 0)
				motion = Doc()->m_rivalList[MyIndex()].team == 0 ? 1 : 3;
			else if (Doc()->m_eventFlag == 1)
				motion = Doc()->m_rivalList[MyIndex()].team == 1 ? 1 : 3;
			else
				motion = 3;
		}
		else
		{
			motion = Doc()->m_rivalList[index].rank;
			if (motion == 1)
				motion = 1;
			else if (motion > Doc()->m_rivalList.size() * 6 / 10)
				motion = 3;
			else
				motion = 2;
		}
	}
	else
		motion = 3;
	if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
	{
		m_pPartTidList = new CPartTidList;
		if (!m_pPartTidList->SetTids(&Doc()->m_userInfo[0].charInfo, 255) ||
			!m_pPartTidList->IsComboValid())
			m_pPartTidList->SetDefaultTids();
		m_pExhibition->SetModel(*m_pPartTidList,
			Doc()->m_userInfo[0].charInfo.tidAuxParts,
			MakeStr("%d\xb5\xee\xb8\xf0\xbc\xc7", motion), 3.f, 0);
	}
	else
	{
		m_pExhibition->SetModel(Doc()->GetMyPartTidList(),
			Doc()->GetMyAuxPartTidList(),
			MakeStr("%d\xb5\xee\xb8\xf0\xbc\xc7", motion), 3.f, 0);
	}
}

IMPLEMENT_OBJECT(FrResultTeamDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrResultTeamDlg, FrForm)
ON_FRESH_VI("stage", FRCMD_INIT, FrResultTeamDlg::OnStageInit)
ON_FRESH_VV("stage", FRCMD_OWNERDRAW, FrResultTeamDlg::OnStageOwnerDraw)
ON_FRESH_VI("trophy", FRCMD_INIT, FrResultTeamDlg::OnTrophyInit)
ON_FRESH_VI("match", FRCMD_INIT, FrResultTeamDlg::OnMatchInit)
ON_FRESH_VI("id", FRCMD_INIT, FrResultTeamDlg::OnIdInit)
ON_FRESH_VI("id", FRCMD_OWNERDRAW, FrResultTeamDlg::OnIdOwnerDraw)
ON_FRESH_VI("score", FRCMD_INIT, FrResultTeamDlg::OnScoreInit)
ON_FRESH_VI("pang", FRCMD_INIT, FrResultTeamDlg::OnPangInit)
ON_FRESH_VI("bonus", FRCMD_INIT, FrResultTeamDlg::OnBonusInit)
ON_FRESH_VI("getitem", FRCMD_INIT, FrResultTeamDlg::OnGetItemInit)
ON_FRESH_VV("getitem", FRCMD_OWNERDRAW, FrResultTeamDlg::OnGetItemOwnerDraw)
ON_FRESH_VI("level", FRCMD_INIT, FrResultTeamDlg::OnLevelInit)
ON_FRESH_VI("exp", FRCMD_INIT, FrResultTeamDlg::OnExpInit)
ON_FRESH_VI("exp_gb", FRCMD_INIT, FrResultTeamDlg::OnExpGaugeBarInit)
ON_FRESH_VI("levelup", FRCMD_INIT, FrResultTeamDlg::OnLevelUpInit)
ON_FRESH_VI("red", FRCMD_INIT, FrResultTeamDlg::OnRedInit)
ON_FRESH_VV("red", FRCMD_OWNERDRAW, FrResultTeamDlg::OnRedOwnerDraw)
ON_FRESH_VI("blue", FRCMD_INIT, FrResultTeamDlg::OnBlueInit)
ON_FRESH_VV("blue", FRCMD_OWNERDRAW, FrResultTeamDlg::OnBlueOwnerDraw)
ON_FRESH_VI("red_score", FRCMD_INIT, FrResultTeamDlg::OnRedScoreInit)
ON_FRESH_VI("blue_score", FRCMD_INIT, FrResultTeamDlg::OnBlueScoreInit)
ON_FRESH_VI("red_pang", FRCMD_INIT, FrResultTeamDlg::OnRedPangInit)
ON_FRESH_VI("blue_pang", FRCMD_INIT, FrResultTeamDlg::OnBluePangInit)
ON_FRESH_VI("result", FRCMD_INIT, FrResultTeamDlg::OnResultInit)
ON_FRESH_VI("rank_static", FRCMD_INIT, FrResultTeamDlg::OnRankStaticInit)
ON_FRESH_VI("result_static", FRCMD_INIT, FrResultTeamDlg::OnResultStaticInit)
ON_FRESH_VI("win_lose", FRCMD_INIT, FrResultTeamDlg::OnWinLoseInit)
ON_FRESH_VI("matchtype", FRCMD_INIT, FrResultTeamDlg::OnMatchTypeInit)
ON_FRESH_VI("bonus_static", FRCMD_INIT, FrResultTeamDlg::OnBonusStaticInit)
END_FRESH_MSGMAP()

void FrResultTeamDlg::OnStageInit(int param)
{
	FrResultDlg::OnStageInit(param);
	m_pGuildTrophy = NULL;
	if (Doc()->m_golfGame.gameType == 6)
	{
		if (Doc()->m_eventFlag == GUILDMATCHUP(0)->team &&
			Doc()->m_eventFlag != 2)
			m_pGuildTrophy =
				g_pFresh->GetManager()->GetBitmap("TROPHY", "guild");
	}
}

bool FrResultTeamDlg::OnInit()
{
	FrResultDlg::OnInit();
	if (Doc()->m_golfGame.gameType == 6)
	{
		memset(&m_guildInfo, 0, sizeof(m_guildInfo));
		if (Doc()->m_eventFlag == 2)
			SetDesc(MakeStr(
				"%49s \xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc0\xda\xbf\xf5\xc0\xbb \xb0\xa1\xb8\xae\xbc\xbc\xbf\xe4^^",
				""));
		else if (Doc()->m_eventFlag == GUILDMATCHUP(0)->team)
			SetDesc(MakeStr(
				"%49s %s\xb1\xe6\xb5\xe5\xb0\xa1 \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				"", GUILDMATCHUP(0)->guildName));
		else if (Doc()->m_eventFlag == GUILDMATCHUP(1)->team)
			SetDesc(MakeStr(
				"%49s %s\xb1\xe6\xb5\xe5\xb0\xa1 \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				"", GUILDMATCHUP(1)->guildName));
		m_guildInfo = Doc()->m_roomInfo.GuildInfo;
	}
	return true;
}

void FrResultTeamDlg::OnStageOwnerDraw()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	WRect form = GetRect();
	g_view->DrawLine2D(WPoint(form.x, form.y + 91.f),
		WPoint(form.x + form.w - 10.f, form.y + 91.f), 0xff000000, 0xff000000,
		0);
	if (Doc()->m_golfGame.gameType == 6)
	{
		if (m_pExhibition)
			m_pExhibition->Display(0, 0);
		const WRect& rect = m_pStage->GetRect();
		if (m_pGuildTrophy)
			graphic->DrawTexture(m_pGuildTrophy,
				WRect(rect.x + 343.f, rect.y + 10.f, m_pGuildTrophy->Width(),
					m_pGuildTrophy->Height()),
				0xffffffff, 0);
		const Bitmap* emblem = NULL;
		switch (Doc()->m_eventFlag)
		{
		case 0:
			if (GUILDMATCHUP(0)->team == 0)
				emblem = m_pRedEmblem;
			break;
		case 1:
			if (GUILDMATCHUP(0)->team == 1)
				emblem = m_pBlueEmblem;
			break;
		}
		if (emblem)
			graphic->DrawTexture(emblem,
				WRect(rect.x + 364.f, rect.y + 22.f, emblem->Width(),
					emblem->Height()),
				0xffffffff, 0);
	}
	else
		FrResultDlg::OnStageOwnerDraw();
}

void FrResultTeamDlg::OnTrophyInit(int param)
{
	FrResultDlg::OnTrophyInit(param);
}

void FrResultTeamDlg::OnMatchTypeInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (Doc()->m_golfGame.gameType == 6)
		text->SetCaption("\xb1\xe6\xb5\xe5 \xb1\xe2\xbf\xa9\xb5\xb5 :");
	else
		text->SetCaption("\xb4\xeb   \xc8\xb8 :");
}

void FrResultTeamDlg::OnMatchInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (Doc()->m_golfGame.gameType == 6)
	{
		if (text)
			text->SetCaption(MakeStr("%d \xc1\xa1", Doc()->m_guildPoint));
	}
	else
	{
		IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(Doc()->m_roomType);
		if (text && match)
			text->SetCaption(MakeStr("%s %s", match->Name, "\xc6\xc0\xc0\xfc"));
	}
}

void FrResultTeamDlg::OnIdInit(int param)
{
	m_pTeamId = DYNAMIC_CAST(FrArea, param);
}

void FrResultTeamDlg::OnIdOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	float offset = 0.f;
	sRivalData& rival = Doc()->m_rivalList[MyIndex()];
	if (rival.guildUID)
	{
		const Bitmap* emblem =
			NetResourceManager::Instance()->GetEmblemByName(rival.guildMark);
		if (emblem)
		{
			graphic->DrawTexture(emblem,
				WRect(m_pTeamId->GetRect().x, m_pTeamId->GetRect().y - 6.f,
					emblem->Width(), emblem->Height()),
				0xffffffff, 0);
			offset = 27.f;
		}
	}
	if (!CProjectG::Instance()->HidePrivacy())
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
		g_pFresh->GetManager()->PrintText(
			WPoint(offset + m_pTeamId->GetRect().x, m_pTeamId->GetRect().y), 0,
			rival.nickname, -1.f, 0xffffffff);
	}
}

void FrResultTeamDlg::OnScoreInit(int param)
{
	FrResultDlg::OnScoreInit(param);
}

void FrResultTeamDlg::OnPangInit(int param)
{
	FrResultDlg::OnPangInit(param);
}

void FrResultTeamDlg::OnBonusStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (Doc()->m_golfGame.gameType == 6)
		text->SetCaption("\xb1\xe6\xb5\xe5 \xc6\xce :");
	else
		text->SetCaption("\xba\xb8\xb3\xca\xbd\xba :");
}

void FrResultTeamDlg::OnBonusInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_golfGame.gameType == 6)
		text->SetCaption(MakeStr("%d \xc6\xce", Doc()->m_guildBonusPang));
	else
	{
		__int64 bonus = (int)((float)(unsigned int)Doc()->m_expRate *
				(float)(Doc()->m_rivalList[MyIndex()].totalPang +
					Doc()->m_rivalList[MyIndex()].totalBonusPang) *
				Doc()->m_pangRate * .01f -
			(float)Doc()->m_rivalList[MyIndex()].totalPang);
		if (Doc()->m_eventFlag == 2)
			text->SetCaption(MakeStr("%I64d", bonus));
		else if (Doc()->m_eventFlag == Doc()->m_rivalList[MyIndex()].team)
			text->SetCaption(MakeStr("%I64d+100", bonus));
		else
			text->SetCaption(MakeStr("%I64d-50", bonus));
	}
}

void FrResultTeamDlg::OnGetItemInit(int param)
{
	FrResultDlg::OnGetItemInit(param);
}

void FrResultTeamDlg::OnGetItemOwnerDraw()
{
	FrResultDlg::OnGetItemOwnerDraw();
}

void FrResultTeamDlg::OnLevelInit(int param)
{
	FrResultDlg::OnLevelInit(param);
}

void FrResultTeamDlg::OnExpInit(int param)
{
	FrResultDlg::OnExpInit(param);
}

void FrResultTeamDlg::OnExpGaugeBarInit(int param)
{
	FrResultDlg::OnExpGaugeBarInit(param);
}

void FrResultTeamDlg::OnLevelUpInit(int param)
{
	FrResultDlg::OnLevelUpInit(param);
}

void FrResultTeamDlg::OnRankStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text && Doc()->m_golfGame.gameType == 6)
		text->SetVisible(false);
}

void FrResultTeamDlg::OnResultStaticInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text && Doc()->m_golfGame.gameType != 6)
		text->SetVisible(false);
}

void FrResultTeamDlg::OnRedInit(int param)
{
	m_pRed = DYNAMIC_CAST(FrArea, param);
	if (!m_pRed)
		return;
	if (Doc()->m_golfGame.gameType == 6)
		m_pRedEmblem = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);
	else
		m_pRed->SetBgImg("30in_team_redteam_dark");
}

void FrResultTeamDlg::OnRedOwnerDraw()
{
	if (Doc()->m_golfGame.gameType == 6)
	{
		WRect rect = m_pRed->GetRect();
		if (m_pRedEmblem)
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pRedEmblem,
				WRect(rect.x + 10.f, rect.y + 1.f, m_pRedEmblem->Width(),
					m_pRedEmblem->Height()),
				0xffffffff, 0);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x, rect.y + 30.f), 0,
			m_guildInfo.szName[0], -1.f, 0xffffffff);
	}
}

void FrResultTeamDlg::OnBlueInit(int param)
{
	m_pBlue = DYNAMIC_CAST(FrArea, param);
	if (!m_pBlue)
		return;
	if (Doc()->m_golfGame.gameType == 6)
		m_pBlueEmblem = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);
	else
		m_pBlue->SetBgImg("30in_team_blueteam_dark");
}

void FrResultTeamDlg::OnBlueOwnerDraw()
{
	if (Doc()->m_golfGame.gameType != 6)
		return;
	WRect rect = m_pBlue->GetRect();
	if (m_pBlueEmblem)
		g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pBlueEmblem,
			WRect(rect.x + 10.f, rect.y + 1.f, m_pBlueEmblem->Width(),
				m_pBlueEmblem->Height()),
			0xffffffff, 0);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x, rect.y + 30.f), 0,
		m_guildInfo.szName[1], -1.f, 0xffffffff);
}

void FrResultTeamDlg::OnRedScoreInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_golfGame.gameType == 5)
		text->SetCaption(MakeStr("%d", Doc()->m_teamResult[0].score));
	else
		text->SetCaption(MakeStr("%d", Doc()->m_guildScore[0]));
}

void FrResultTeamDlg::OnBlueScoreInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (!text)
		return;
	if (Doc()->m_golfGame.gameType == 5)
		text->SetCaption(MakeStr("%d", Doc()->m_teamResult[1].score));
	else
		text->SetCaption(MakeStr("%d", Doc()->m_guildScore[1]));
}

void FrResultTeamDlg::OnRedPangInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text)
		text->SetCaption(MakeStr("%I64dp", Doc()->m_teamResult[0].pang));
}

void FrResultTeamDlg::OnBluePangInit(int param)
{
	FrStatic* text = DYNAMIC_CAST(FrStatic, param);
	if (text)
		text->SetCaption(MakeStr("%I64dp", Doc()->m_teamResult[1].pang));
}

void FrResultTeamDlg::OnResultInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (!area || Doc()->m_golfGame.gameType == 6)
		return;
	switch (Doc()->m_eventFlag)
	{
	case 0:
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			area->SetBgImg("30in_team_redteam_win");
		else
			area->SetBgImg("30in_team_blueteam_lose");
		break;
	case 1:
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			area->SetBgImg("30in_team_redteam_lose");
		else
			area->SetBgImg("30in_team_blueteam_win");
		break;
	case 2:
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			area->SetBgImg("30in_team_redteam_draw");
		else
			area->SetBgImg("30in_team_blueteam_draw");
		break;
	}
}

void FrResultTeamDlg::OnWinLoseInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	if (!area || Doc()->m_golfGame.gameType != 6)
		return;
	char image[32];
	switch (Doc()->m_eventFlag)
	{
	case 0:
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			strcpy(image, "win2");
		else
			strcpy(image, "lose2");
		break;
	case 1:
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			strcpy(image, "lose2");
		else
			strcpy(image, "win2");
		break;
	case 2:
		strcpy(image, "draw2");
		break;
	}
	area->SetBgImg(g_pFresh->GetManager()->GetBitmap("SCORECARD", image));
}

bool UniteRankCompare(const void* first, const void* second)
{
	const sRivalData* a = (const sRivalData*)first;
	const sRivalData* b = (const sRivalData*)second;
	if (a->rank < b->rank)
		return true;
	if (a->rank > b->rank)
		return false;
	if (a->quitOrder > b->quitOrder)
		return true;
	if (a->quitOrder < b->quitOrder)
		return false;
	if (a->totalScore < b->totalScore)
		return true;
	if (a->totalScore > b->totalScore)
		return false;
	if (a->totalPang > b->totalPang)
		return true;
	if (a->totalPang < b->totalPang)
		return false;
	return a->order < b->order;
}

bool ReportRankCompare(const void* first, const void* second)
{
	const sRivalData* a = (const sRivalData*)first;
	const sRivalData* b = (const sRivalData*)second;
	if (a->rank < b->rank)
		return true;
	if (a->rank > b->rank)
		return false;
	if (a->quitOrder > b->quitOrder)
		return true;
	if (a->quitOrder < b->quitOrder)
		return false;
	if (a->totalScore < b->totalScore)
		return true;
	if (a->totalScore > b->totalScore)
		return false;
	if (a->totalPang > b->totalPang)
		return true;
	if (a->totalPang < b->totalPang)
		return false;
	return a->order < b->order;
}

bool UniteGuildIndexCompare(const void* first, const void* second)
{
	const sRivalData* a = (const sRivalData*)first;
	const sRivalData* b = (const sRivalData*)second;
	if (a->quitOrder > b->quitOrder)
		return true;
	if (a->quitOrder < b->quitOrder)
		return false;
	if (a->guildPoint > b->guildPoint)
		return true;
	if (a->guildPoint < b->guildPoint)
		return false;
	if (a->totalPang > b->totalPang)
		return true;
	if (a->totalPang < b->totalPang)
		return false;
	return a->order < b->order;
}

IMPLEMENT_OBJECT(FrUniteResultDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrUniteResultDlg, FrForm)
ON_FRESH_VI("cancel", FRCMD_INIT, FrUniteResultDlg::OnCancelButtonInit)
ON_FRESH_VI("awardinfo", FRCMD_INIT, FrUniteResultDlg::OnAwardInfoInit)
ON_FRESH_VV("awardinfo", FRCMD_LBUTTONUP, FrUniteResultDlg::OnAwardInfoClick)
ON_FRESH_VI("myinfoarea", FRCMD_INIT, FrUniteResultDlg::OnMyInfoAreaInit)
ON_FRESH_VI("myinfoarea", FRCMD_OWNERDRAW, FrUniteResultDlg::OnMyInfoAreaDraw)
ON_FRESH_VI("exp_gb", FRCMD_INIT, FrUniteResultDlg::OnExpGaugeBarInit)
ON_FRESH_VI("exp_area", FRCMD_INIT, FrUniteResultDlg::OnExpAreaInit)
ON_FRESH_VI("exp_area", FRCMD_OWNERDRAW, FrUniteResultDlg::OnExpAreaOwnerDraw)
ON_FRESH_VI("resultlist", FRCMD_INIT, FrUniteResultDlg::OnRankListBoxInit)
ON_FRESH_VI("resultlist", FRCMD_OWNERDRAW,
	FrUniteResultDlg::OnRankListOwnerDraw)
ON_FRESH_VV("resultlist", FRCMD_LBUTTONUP, FrUniteResultDlg::OnRankListLBtnUp)
ON_FRESH_VV("resultlist", FRCMD_RBUTTONUP, FrUniteResultDlg::OnRankListRBtnUp)
ON_FRESH_VI("resultwinner", FRCMD_INIT, FrUniteResultDlg::OnDescAreaInit)
ON_FRESH_VI("resultwinner", FRCMD_OWNERDRAW,
	FrUniteResultDlg::OnDescAreaOwnerDraw)
ON_FRESH_VI("awardarea", FRCMD_INIT, FrUniteResultDlg::OnAwardAreaInit)
ON_FRESH_VI("awardarea", FRCMD_OWNERDRAW,
	FrUniteResultDlg::OnAwardAreaOwnerDraw)
ON_FRESH_VI("itemarea", FRCMD_INIT, FrUniteResultDlg::OnItemAreaInit)
ON_FRESH_VI("itemarea", FRCMD_OWNERDRAW, FrUniteResultDlg::OnItemAreaOwnerDraw)
ON_FRESH_VI("teamarea", FRCMD_INIT, FrUniteResultDlg::OnTeamResultInit)
ON_FRESH_VI("teamarea", FRCMD_OWNERDRAW,
	FrUniteResultDlg::OnTeamResultOwnerDraw)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, FrUniteResultDlg::OnTooltipOwnerDraw)
ON_FRESH_VI("nextlevel", FRCMD_INIT, FrUniteResultDlg::OnNextLevelTextInit)
END_FRESH_MSGMAP()

void FrUniteResultDlg::OnCancelButtonInit(int param)
{
	if (!m_pCancelBtn)
		m_pCancelBtn = DYNAMIC_CAST(FrButton, param);
}

void FrUniteResultDlg::OnAwardInfoInit(int param)
{
	if (!m_pAwardInfoBtn)
		m_pAwardInfoBtn = DYNAMIC_CAST(FrButton, param);
}

void FrUniteResultDlg::OnAwardInfoClick()
{
	if (!m_pAwardInfoDlg)
	{
		m_pAwardInfoDlg = CreateForm<FrAwardInfoDlg>(g_pFresh->GetManager(),
			this, "awardinfomation");
		m_pAwardInfoDlg->Open(
			(FRESH_PFN_RESULT)&FrUniteResultDlg::OnAwardInfoDlgResult, 0);
	}
}

void FrUniteResultDlg::OnMyInfoAreaInit(int param)
{
	if (!m_pMyInfoArea)
		m_pMyInfoArea = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnMyInfoAreaDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	sRivalData rival;
	if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport)
	{
		rival = Doc()->m_rivalList[m_tikiReportIndex];
	}
	else
	{
		if (Doc()->m_rivalList.size() <= MyIndex())
			return;
		rival = Doc()->m_rivalList[MyIndex()];
	}
	WPoint origin(m_pMyInfoArea->GetRect().x, m_pMyInfoArea->GetRect().y);
	float textY = 0;
	int space = graphic->SetSpace(0);
	unsigned char back = m_resultDlgType == 0 ? 2 : rival.team;
	if (m_pRankBack[back])
	{
		WPoint point;
		point.x = origin.x + 13.f;
		point.y =
			origin.y + (11.f - ((float)(m_pRankBack[back]->Height() >> 1)));
		WRect rankRect(point.x, point.y, m_pRankBack[back]->Width(),
			m_pRankBack[back]->Height());
		graphic->DrawTexture(m_pRankBack[back], rankRect, 0xffffffff, 0);
		graphic->SetTextColor(0xffffffff, 0xffffffff);
		graphic->SetTextStyle(1);
		point.x += m_pRankBack[back]->Width() >> 1;
		point.y += 2.f;
		unsigned char rank =
			m_resultDlgType == 2 ? (unsigned char)m_myRank : rival.rank;
		WPoint rankPos(point.x, point.y);
		if (rank < 255)
			graphic->Print(rankPos, 1, "%d", rank);
		else
			graphic->Print(rankPos, 1, "-");
		graphic->SetTextStyle(0);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		textY = point.y;
	}
	m_pLevelIcon = g_pFresh->GetBitmap(MakeStr("level_%03d", rival.level + 1));
	if (m_pLevelIcon)
		graphic->DrawTexture(m_pLevelIcon,
			WRect(origin.x + 47.f,
				origin.y + (11.f - (m_pLevelIcon->Height() >> 1)),
				(float)m_pLevelIcon->Width(), (float)m_pLevelIcon->Height()),
			0xffffffff, 0);
	if (rival.guildUID)
	{
		m_pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(rival.guildMark);
		if (m_pEmblem)
			graphic->DrawTexture(m_pEmblem,
				WRect(origin.x + 110.f,
					origin.y + (11.f - (m_pEmblem->Height() >> 1)),
					(float)m_pEmblem->Width(), (float)m_pEmblem->Height()),
				0xffffffff, 0);
	}
	if (m_bTikiReport)
	{
		if (rival.uid == MyUID())
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->SetTextStyle(1);
		}
		else
		{
			if (rival.state == 3)
				graphic->SetTextColor(0xff808080, 0xffffffff);
			else
				graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->SetTextStyle(0);
		}
	}
	else if (rival.oid == MyGuid(false))
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(1);
	}
	else
	{
		if (rival.state == 3)
			graphic->SetTextColor(0xff808080, 0xffffffff);
		else
			graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
	}
	WPoint nicknamePos(origin.x + 141.f, textY);
	g_pFresh->GetManager()->PrintText(nicknamePos, 0, rival.nickname, -1.f,
		0xffffffff);
	WPoint scorePos(origin.x + 270.f, textY);
	if (m_resultDlgType == 2)
		graphic->Print(scorePos, 2, "%d", rival.guildPoint);
	else
		graphic->Print(scorePos, 2, "%d", rival.totalScore);
	WPoint pangPos(origin.x + 340.f, textY);
	graphic->Print(pangPos, 2, "%I64d", rival.totalPang);
	WPoint bonusPos(origin.x + 347.f, textY);
	graphic->SetTextColor(0xff006493, 0xffffffff);
	if (rival.totalBonusPang > 0)
	{
		graphic->SetTextStyle(1);
		graphic->Print(bonusPos, 0, "(+%I64d)", rival.totalBonusPang);
	}
	else
		graphic->Print(bonusPos, 0, "(0)");
	graphic->SetTextStyle(0);
	graphic->SetTextColor(0xff000000, 0xffffffff);
	bool bonus = false;
	if ((rival.capability & 0x80) && m_pPcBonus)
	{
		graphic->DrawTexture(m_pPcBonus,
			WRect(origin.x + 408.f,
				origin.y + (11.f - (m_pPcBonus->Height() >> 1)),
				(float)m_pPcBonus->Width(), (float)m_pPcBonus->Height()),
			0xffffffff, 0);
		bonus = true;
	}
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(rival.mascotTypeId);
	if (item)
	{
		WRect rect(origin.x + 453.f,
			origin.y + (11.f - ((float)(m_pMascot01->Height() >> 1))),
			(float)m_pMascot01->Width(), (float)m_pMascot01->Height());
		if (item->TypeId == 0x40000001)
		{
			graphic->DrawTexture(m_pMascot01, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x40000002)
		{
			graphic->DrawTexture(m_pMascot02, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x40000009)
		{
			graphic->DrawTexture(m_pMascot09, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000a)
		{
			graphic->DrawTexture(m_pMascot10, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000b)
		{
			graphic->DrawTexture(m_pMascot11, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000c)
		{
			graphic->DrawTexture(m_pMascot12, rect, 0xffffffff, 0);
			bonus = true;
		}
	}
	if (rival.pangMastery == 1)
	{
		graphic->DrawTexture(m_pBonusMa,
			WRect(origin.x + 470.f,
				origin.y + (11.f - (m_pBonusMa->Height() >> 1)),
				(float)m_pBonusMa->Width(), (float)m_pBonusMa->Height()),
			0xffffffff, 0);
		bonus = true;
	}
	if (rival.pangNitro == 1)
	{
		WRect rect(0, 0, (float)m_pBonusMa2->Width(),
			(float)m_pBonusMa2->Height());
		if (rival.pangMastery == 1)
		{
			rect.x = origin.x + 470.f + (m_pBonusMa2->Width() >> 1);
			rect.y = origin.y + (11.f - ((float)(m_pBonusMa2->Height() >> 1)));
		}
		else
		{
			rect.x = origin.x + 470.f;
			rect.y = origin.y + (11.f - ((float)(m_pBonusMa2->Height() >> 1)));
		}
		graphic->DrawTexture(m_pBonusMa2, rect, 0xffffffff, 0);
	}
	else if (bonus != true && m_pIconZero)
	{
		graphic->DrawTexture(m_pIconZero,
			WRect(origin.x + 440.f,
				origin.y + (11.f - (m_pIconZero->Height() >> 1)),
				(float)m_pIconZero->Width(), (float)m_pIconZero->Height()),
			0xffffffff, 0);
	}
	graphic->SetSpace(space);
}

void FrUniteResultDlg::OnExpGaugeBarInit(int param)
{
	if (!m_pExpGaugeBar)
		m_pExpGaugeBar = DYNAMIC_CAST(FrGaugeBar, param);
}

void FrUniteResultDlg::OnExpAreaInit(int param)
{
	if (!m_pExpArea)
		m_pExpArea = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnExpAreaOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	int space = graphic->SetSpace(0);
	WRect rect = m_pExpGaugeBar->GetRect();
	WPoint point(rect.x + rect.w * .5f, rect.y + rect.h * .5f - 5.f);
	if (!m_bLevelUp)
	{
		graphic->SetTextColor(0xffffffff, 0xffffffff);
		graphic->SetTextStyle(1);
		graphic->Print(point, 1, "%d / %d", m_exp / 10, m_maxExp / 10);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(0);
	}
	else
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("font_levelup");
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(point.x - (bitmap->Width() >> 1),
					point.y - (rect.h * .5f - 5.f), bitmap->Width(),
					bitmap->Height()),
				0xffffffff, 0);
	}
	graphic->SetSpace(space);
}

void FrUniteResultDlg::OnRankListBoxInit(int param)
{
	if (!m_pRankList)
		m_pRankList = DYNAMIC_CAST(FrListBox, param);
	m_pRankList->UseRightButton(true);
}

void FrUniteResultDlg::OnRankListOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	FrListItem* listItem = (FrListItem*)param;
	if (!listItem)
		return;
	sRivalData* rival = (sRivalData*)listItem->pData;
	if (!rival)
		return;
	float textY = 0;
	int space = graphic->SetSpace(0);
	if (listItem->selected == true &&
		(!m_bTikiReport || !IsLocalContent((localContentType_t)0x14)))
		graphic->Box(WRect(listItem->pos.x + 4.f, listItem->pos.y,
						 m_pRankList->GetItemWidth(),
						 m_pRankList->GetItemHeight()),
			0x999b9dff, 0, 0.f);
	int type = m_resultDlgType;
	unsigned char back = 2;
	if (type)
		back = rival->team;
	const Bitmap* bitmap;
	if (back < 3 && (bitmap = m_pRankBack[back]))
	{
		WPoint point = listItem->pos;
		point.x += 13.f;
		point.y += 11.f - (bitmap->Height() * .5f);
		{
			WRect rankRect(point.x, point.y, (float)m_pRankBack[back]->Width(),
				(float)m_pRankBack[back]->Height());
			graphic->DrawTexture(m_pRankBack[back], rankRect, 0xffffffff, 0);
		}
		graphic->SetTextColor(0xffffffff, 0xffffffff);
		graphic->SetTextStyle(1);
		point.x += m_pRankBack[back]->Width() * .5f;
		point.y += 2.f;
		int rank;
		if (m_resultDlgType == 2)
		{
			if (rival->oid == MyGuid(false))
				m_myRank = listItem->no + 1;
			rank = listItem->no + 1;
		}
		else
			rank = rival->rank;
		{
			WPoint rankPos(point.x, point.y);
			if (rank < 255)
			{
				graphic->Print(rankPos, 1, "%d", rank);
			}
			else
			{
				graphic->Print(WPoint(point.x, point.y), 1, "-");
			}
		}
		graphic->SetTextStyle(0);
		graphic->SetTextColor(0xff000000, 0xffffffff);
		textY = point.y;
	}
	m_pLevelIcon = g_pFresh->GetBitmap(MakeStr("level_%03d", rival->level + 1));
	if (m_pLevelIcon)
		graphic->DrawTexture(m_pLevelIcon,
			WRect(listItem->pos.x + 47.f,
				listItem->pos.y +
					(11.f - ((float)(m_pLevelIcon->Height() >> 1))),
				(float)m_pLevelIcon->Width(), (float)m_pLevelIcon->Height()),
			0xffffffff, 0);
	if (rival->guildUID)
	{
		m_pEmblem =
			NetResourceManager::Instance()->GetEmblemByName(rival->guildMark);
		if (m_pEmblem)
			graphic->DrawTexture(m_pEmblem,
				WRect(listItem->pos.x + 110.f,
					listItem->pos.y +
						(11.f - ((float)(m_pEmblem->Height() >> 1))),
					(float)m_pEmblem->Width(), (float)m_pEmblem->Height()),
				0xffffffff, 0);
	}
	if (m_bTikiReport)
	{
		if (rival->uid == MyUID())
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->SetTextStyle(1);
		}
		else
		{
			if (rival->state == 3)
			{
				graphic->SetTextColor(0xff808080, 0xffffffff);
				graphic->SetTextStyle(0);
			}
			else
			{
				graphic->SetTextColor(0xff000000, 0xffffffff);
				graphic->SetTextStyle(0);
			}
		}
	}
	else if (rival->oid == MyGuid(false))
	{
		graphic->SetTextColor(0xff000000, 0xffffffff);
		graphic->SetTextStyle(1);
	}
	else
	{
		if (rival->state == 3)
		{
			graphic->SetTextColor(0xff808080, 0xffffffff);
			graphic->SetTextStyle(0);
		}
		else
		{
			graphic->SetTextColor(0xff000000, 0xffffffff);
			graphic->SetTextStyle(0);
		}
	}
	WPoint nicknamePos(listItem->pos.x + 141.f, textY);
	g_pFresh->GetManager()->PrintText(nicknamePos, 0, rival->nickname, -1.f,
		0xffffffff);
	WPoint scorePos(listItem->pos.x + 270.f, textY);
	if (m_resultDlgType == 2)
		graphic->Print(scorePos, 2, "%d", rival->guildPoint);
	else
		graphic->Print(scorePos, 2, "%d", rival->totalScore);
	WPoint pangPos(listItem->pos.x + 340.f, textY);
	graphic->Print(pangPos, 2, "%I64d", rival->totalPang);
	WPoint bonusPos(listItem->pos.x + 347.f, textY);
	graphic->SetTextColor(0xff006493, 0xffffffff);
	if (rival->totalBonusPang > 0)
	{
		graphic->SetTextStyle(1);
		graphic->Print(bonusPos, 0, "(+%I64d)", rival->totalBonusPang);
	}
	else
		graphic->Print(bonusPos, 0, "(0)");
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->SetTextStyle(0);
	bool bonus = false;
	if ((rival->capability & 0x80) && m_pPcBonus)
	{
		graphic->DrawTexture(m_pPcBonus,
			WRect(listItem->pos.x + 408.f,
				listItem->pos.y + (11.f - ((float)(m_pPcBonus->Height() >> 1))),
				(float)m_pPcBonus->Width(), (float)m_pPcBonus->Height()),
			0xffffffff, 0);
		bonus = true;
	}
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(rival->mascotTypeId);
	if (item)
	{
		WRect rect(listItem->pos.x + 453.f,
			listItem->pos.y + (11.f - ((float)(m_pMascot01->Height() >> 1))),
			(float)m_pMascot01->Width(), (float)m_pMascot01->Height());
		if (item->TypeId == 0x40000001)
		{
			graphic->DrawTexture(m_pMascot01, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x40000002)
		{
			graphic->DrawTexture(m_pMascot02, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x40000009)
		{
			graphic->DrawTexture(m_pMascot09, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000a)
		{
			graphic->DrawTexture(m_pMascot10, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000b)
		{
			graphic->DrawTexture(m_pMascot11, rect, 0xffffffff, 0);
			bonus = true;
		}
		else if (item->TypeId == 0x4000000c)
		{
			graphic->DrawTexture(m_pMascot12, rect, 0xffffffff, 0);
			bonus = true;
		}
	}
	if (rival->pangMastery == 1)
	{
		graphic->DrawTexture(m_pBonusMa,
			WRect(listItem->pos.x + 470.f,
				listItem->pos.y + (11.f - ((float)(m_pBonusMa->Height() >> 1))),
				(float)m_pBonusMa->Width(), (float)m_pBonusMa->Height()),
			0xffffffff, 0);
		bonus = true;
	}
	if (rival->pangNitro == 1)
	{
		WRect rect(0, 0, (float)m_pBonusMa2->Width(),
			(float)m_pBonusMa2->Height());
		if (rival->pangMastery == 1)
		{
			rect.x = listItem->pos.x + (470.f + (m_pBonusMa2->Width() >> 1));
			rect.y = listItem->pos.y +
				(11.f - ((float)(m_pBonusMa2->Height() >> 1)));
		}
		else
		{
			rect.x = listItem->pos.x + 470.f;
			rect.y = listItem->pos.y +
				(11.f - ((float)(m_pBonusMa2->Height() >> 1)));
		}
		graphic->DrawTexture(m_pBonusMa2, rect, 0xffffffff, 0);
	}
	else if (bonus != true && m_pIconZero)
		graphic->DrawTexture(m_pIconZero,
			WRect(listItem->pos.x + 440.f,
				listItem->pos.y +
					(11.f - ((float)(m_pIconZero->Height() >> 1))),
				(float)m_pIconZero->Width(), (float)m_pIconZero->Height()),
			0xffffffff, 0);
	int medal = -1;
	if (m_resultDlgType != 3 &&
		(!IsLocalContent((localContentType_t)0x2b) ||
			Doc()->m_roomInfo.nUserLimit < 100))
	{
		unsigned char firstHole = Doc()->m_holeOrder[0];
		if (rival->hole == firstHole)
			medal = -1;
		else
		{
			medal = GetMedalIndex(rival->oid);
			if (medal >= 0 && medal <= 2 && rival->flag > 3 && !m_bTikiReport)
				medal = 3;
		}
	}
	float offset = 0;
	if (medal >= 0 && medal <= 2)
	{
		WRect medalRect = WRect(listItem->pos.x + 515.f,
			listItem->pos.y +
				(11.f - ((float)(m_pMedal[medal]->Height() >> 1))),
			(float)m_pMedal[medal]->Width(), (float)m_pMedal[medal]->Height());
		graphic->DrawTexture(m_pMedal[medal], medalRect, 0xffffffff, 0);
		offset = m_pMedal[medal]->Width();
	}
	for (int i = 0; i <= 5; ++i)
	{
		if (Doc()->m_awardItem[i].uid == rival->oid)
		{
			WRect rect((listItem->pos.x + 515.f) + offset,
				listItem->pos.y +
					(11.f - ((float)(m_pPrize[i]->Height() >> 1))),
				(float)m_pPrize[i]->Width(), (float)m_pPrize[i]->Height());
			graphic->DrawTexture(m_pPrize[i], rect, 0xffffffff, 0);
			if (!m_bRankTooltip)
				m_bRankTooltip = SetTooltip(rect, i);
			offset += m_pPrize[i]->Width();
		}
	}
	if ((int)offset == 0 && m_pIconZero)
		graphic->DrawTexture(m_pIconZero,
			WRect(listItem->pos.x + 515.f,
				listItem->pos.y +
					(11.f - ((float)(m_pIconZero->Height() >> 1))),
				(float)m_pIconZero->Width(), (float)m_pIconZero->Height()),
			0xffffffff, 0);
	graphic->SetSpace(space);
}

void FrUniteResultDlg::OnRankListLBtnUp()
{
	if (m_bTikiReport)
		return;
	FrListItem* item = m_pRankList->GetItemUnderCursor();
	if (item && item->pData)
	{
		m_selectedUID = ((sRivalData*)item->pData)->oid;
		m_pRankList->SelectItem(item, true);
		if (!m_pScoreDlg)
		{
			if (Doc()->m_golfGame.gameType == 6)
			{
				m_pScoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(),
					this, "score_guild");
				m_pScoreDlg->Open(
					(FRESH_PFN_RESULT)&FrUniteResultDlg::OnScoreDlgResult, 0);
				m_pScoreDlg->EnableDrag(false);
				m_pScoreDlg->SetGuildPlayer(m_selectedUID);
				m_pScoreDlg->SetVisibleCloseBtn(false);
			}
			else
			{
				m_pScoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(),
					this, "score");
				m_pScoreDlg->Open(
					(FRESH_PFN_RESULT)&FrUniteResultDlg::OnScoreDlgResult, 0);
				m_pScoreDlg->EnableDrag(false);
				if (m_selectedUID == -1)
					m_selectedUID = MyGuid(false);
				m_pScoreDlg->SetPlayer(m_selectedUID);
				m_pScoreDlg->SetVisibleCloseBtn(true);
			}
		}
		if (m_pScoreDlg)
			m_pScoreDlg->SetPlayer(m_selectedUID);
	}
}

void FrUniteResultDlg::OnRankListRBtnUp()
{
	if (m_bTikiReport)
		return;
	FrListItem* item = m_pRankList->GetItemUnderCursor();
	if (item && item->pData)
	{
		sRivalData* rival = (sRivalData*)item->pData;
		m_selectedUID = rival->oid;
		if (m_pScoreDlg)
			m_pScoreDlg->SetPlayer(m_selectedUID);
		if (CUserInfo::Instance() && rival->state != 3)
		{
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				Doc()->m_briefUserInfoMap.find(rival->oid);
			if (it != Doc()->m_briefUserInfoMap.end())
				it->second.roomIndex = Doc()->m_roomInfo.roomGuid;
			CUserInfo::Instance()->SetInfo(rival->uid, rival->oid, true, true,
				false, false, rival->nickname);
			if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())
				CUserInfo::Instance()->GetDlg()->DisableFlag(FrESCAPE);
		}
	}
}

void FrUniteResultDlg::OnDescAreaInit(int param)
{
	if (!m_pDescArea)
		m_pDescArea = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnDescAreaOwnerDraw(int param)
{
	WRect rect = m_pDescArea->GetRect();
	rect.x += 15.f;
	rect.y -= 1.f;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->SetTextStyle(0);
	const char* text;
	if (m_resultDlgType == 0)
		text = MakeStr(
			"1\xb5\xee\xc0\xba %s\xb4\xd4\xc0\xcc \xc2\xf7\xc1\xf6\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
			m_winner.nickname);
	else if (Doc()->m_eventFlag == 0)
		text = MakeStr(
			"RED \xc6\xc0\xc0\xcc \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			m_winner.nickname);
	else
		text = MakeStr(
			"BLUE \xc6\xc0\xc0\xcc \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			m_winner.nickname);
	WPoint point(rect.x, rect.y);
	if (text)
		g_pFresh->GetManager()->PrintText(point, 0, text, -1.f, 0xffffffff);
}

void FrUniteResultDlg::OnAwardAreaInit(int param)
{
	if (!m_pAwardArea)
		m_pAwardArea = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnAwardAreaOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	sRivalData rival;
	if (m_bTikiReport && IsLocalContent((localContentType_t)0x14))
		rival = Doc()->m_rivalList[m_tikiReportIndex];
	else
		rival = Doc()->m_rivalList[MyIndex()];
	IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(Doc()->m_roomType);
	WRect origin = GetRect();
	float pointX = origin.x + 505.f;
	const float& pointY = origin.y;
	bool award = false;
	if (match && Doc()->m_eventType >= 1 && Doc()->m_eventType <= 3)
	{
		const Bitmap* bitmap =
			g_pFresh->GetBitmap(match->Icon[Doc()->m_eventType - 1]);
		if (bitmap)
		{
			float x = GetRect().x;
			float baseY = GetRect().y;
			const float& y = (float)baseY;
			graphic->DrawTexture(bitmap,
				WRect(x + (505.f - (bitmap->Width() >> 1)),
					y + (381.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
			pointX += bitmap->Width() >> 1;
			award = true;
		}
	}
	for (int i = 0; i <= 5; ++i)
	{
		if (m_bTikiReport)
		{
			if (Doc()->m_awardItem[i].uid == rival.uid)
			{
				WRect rect(pointX,
					((float)pointY) +
						(381.f - ((float)(m_pPrize[i]->Height() >> 1))),
					(float)m_pPrize[i]->Width(), (float)m_pPrize[i]->Height());
				graphic->DrawTexture(m_pPrize[i], rect, 0xffffffff, 0);
				pointX += m_pPrize[i]->Width();
				if (!m_bAwardTooltip)
					m_bAwardTooltip = SetTooltip(rect, i);
				award = true;
			}
		}
		else if (Doc()->m_awardItem[i].uid == MyGuid(false))
		{
			WRect rect(pointX,
				((float)pointY) +
					(381.f - ((float)(m_pPrize[i]->Height() >> 1))),
				(float)m_pPrize[i]->Width(), (float)m_pPrize[i]->Height());
			graphic->DrawTexture(m_pPrize[i], rect, 0xffffffff, 0);
			pointX += m_pPrize[i]->Width();
			if (!m_bAwardTooltip)
				m_bAwardTooltip = SetTooltip(rect, i);
			award = true;
		}
	}
	if (!award)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("icon_zero_vs");
		if (bitmap)
		{
			WPoint pos(GetRect().x, GetRect().y);
			graphic->DrawTexture(bitmap,
				WRect(pos.x + 522.f, pos.y + (381.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		}
	}
}

void FrUniteResultDlg::OnItemAreaInit(int param)
{
	if (!m_pItemArea)
		m_pItemArea = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnItemAreaOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	WRect origin = GetRect();
	float offset = 0.f;
	float couponOffset = 0.f;
	const Bitmap* bitmap = NULL;
	for (int i = 0; i < 12; ++i)
	{
		if (Doc()->m_awardItem[i].uid == MyGuid(true) &&
			Doc()->m_awardItem[i].pang)
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(Doc()->m_awardItem[i].pang);
			if (item)
			{
				switch ((unsigned long)Doc()->m_awardItem[i].pang >> 26)
				{
				case 2:
					bitmap = g_pFresh->GetManager()->GetBitmap("FASHION",
						item->Icon);
					break;
				case 5:
					bitmap =
						g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
					break;
				case 6:
					bitmap =
						g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
					break;
				default:
					bitmap = NULL;
					break;
				}
			}
			if (bitmap)
			{
				unsigned id = Doc()->m_awardItem[i].pang;
				if (ItemManager()->GetCouponKind(id) == 1)
					couponOffset = -10.f;
				graphic->DrawTexture(bitmap,
					WRect(origin.x + (372.f - (bitmap->Width() + offset)),
						origin.y + (382.f - ((bitmap->Height() - 23) >> 1)) +
							couponOffset,
						bitmap->Width(), bitmap->Height()),
					0xffffffff, 0);
				offset += bitmap->Width();
			}
		}
	}
	for (std::map<unsigned long, int>::iterator it = m_prizeMap.begin();
		it != m_prizeMap.end(); ++it)
	{
		int count = (*it).second;
		unsigned id = (*it).first;
		IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(id);
		if (item)
		{
			const Bitmap* bitmap = g_pFresh->GetBitmap(item->Icon);
			if (bitmap)
			{
				WRect rect(origin.x + (372.f - (bitmap->Width() + offset)),
					origin.y + (382.f - ((bitmap->Height() - 23) >> 1)),
					bitmap->Width(), bitmap->Height());
				graphic->DrawTexture(bitmap, rect, 0xffffffff, 0);
				if (count > 1)
				{
					char text[8] = { 0 };
					itoa(count, text, 10);
					m_pWindFont->Print(g_view, rect.x + 30.f, rect.y + 35.f,
						text, 0, 0xffffffff, NULL);
				}
				offset += bitmap->Width() * .75f;
			}
		}
	}
	if (offset < .01f)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("icon_zero_vs");
		graphic->DrawTexture(bitmap,
			WRect(origin.x + 260.f,
				origin.y + (382.f - (bitmap->Height() >> 1)), bitmap->Width(),
				bitmap->Height()),
			0xffffffff, 0);
	}
}

void FrUniteResultDlg::OnTeamResultInit(int param)
{
	if (!m_pTeamResult)
		m_pTeamResult = DYNAMIC_CAST(FrArea, param);
}

void FrUniteResultDlg::OnTeamResultOwnerDraw(int param)
{
	if (m_resultDlgType == 0)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	WRect origin = GetRect();
	WRect back0 = WRect(origin.x + 13.f,
		origin.y + (468.f - (m_pRankBack[0]->Height() >> 1)),
		(float)m_pRankBack[0]->Width(), (float)m_pRankBack[0]->Height());
	graphic->DrawTexture(m_pRankBack[0], back0, 0xffffffff, 0);
	WRect back1 = WRect(origin.x + 13.f,
		origin.y + (492.f - (m_pRankBack[1]->Height() >> 1)),
		(float)m_pRankBack[1]->Width(), (float)m_pRankBack[1]->Height());
	graphic->DrawTexture(m_pRankBack[1], back1, 0xffffffff, 0);
	if (m_resultDlgType == 1)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("icon_redteam");
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(origin.x + (117.f - (bitmap->Width() >> 1)),
					origin.y + (468.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		bitmap = g_pFresh->GetBitmap("icon_blueteam");
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(origin.x + (117.f - (bitmap->Width() >> 1)),
					origin.y + (492.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
	}
	else
	{
		const Bitmap* bitmap = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[0]);
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(origin.x + (117.f - (bitmap->Width() >> 1)),
					origin.y + (471.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		bitmap = NetResourceManager::Instance()->GetEmblemByName(
			Doc()->m_roomInfo.GuildInfo.szEmblemName[1]);
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(origin.x + (117.f - (bitmap->Width() >> 1)),
					origin.y + (492.f - (bitmap->Height() >> 1)),
					(float)bitmap->Width(), (float)bitmap->Height()),
				0xffffffff, 0);
		const WRect& rect = m_pTeamResult->GetRect();
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 120.f, rect.y + 17.f),
			0, Doc()->m_roomInfo.GuildInfo.szName[0], -1.f, 0xffffffff);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 120.f, rect.y + 39.f),
			0, Doc()->m_roomInfo.GuildInfo.szName[1], -1.f, 0xffffffff);
	}
	origin = GetRect();
	graphic->Print(WPoint(origin.x + 277.f, origin.y + 463.f), 1, "%d",
		Doc()->m_teamResult[0].score);
	graphic->Print(WPoint(origin.x + 277.f, origin.y + 487.f), 1, "%d",
		Doc()->m_teamResult[1].score);
	graphic->Print(WPoint(origin.x + 357.f, origin.y + 463.f), 1, "%I64d",
		Doc()->m_teamResult[0].pang);
	graphic->Print(WPoint(origin.x + 357.f, origin.y + 487.f), 1, "%I64d",
		Doc()->m_teamResult[1].pang);
	const Bitmap* bitmap;
	if (Doc()->m_eventFlag == 0)
		bitmap = g_pFresh->GetBitmap("30in_team_redteam_win");
	else if (Doc()->m_eventFlag == 1)
		bitmap = g_pFresh->GetBitmap("30in_team_blueteam_win");
	else if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport)
	{
		if (Doc()->m_rivalList[m_tikiReportIndex].team == 0)
			bitmap = g_pFresh->GetBitmap("30in_team_redteam_draw");
		else
			bitmap = g_pFresh->GetBitmap("30in_team_blueteam_draw");
	}
	else
	{
		if (Doc()->m_rivalList[MyIndex()].team == 0)
			bitmap = g_pFresh->GetBitmap("30in_team_redteam_draw");
		else
			bitmap = g_pFresh->GetBitmap("30in_team_blueteam_draw");
	}
	if (bitmap)
	{
		m_teamAniTime = min(m_teamAniTime, 3.14f);
		float scale = 1.f + sinf(m_teamAniTime) * .2f;
		WRect imageRect =
			WRect(origin.x + (522.f - bitmap->Width() * scale * .5f),
				origin.y + (482.f - bitmap->Height() * scale * .5f),
				bitmap->Width() * scale, (bitmap->Height() * scale));
		graphic->DrawTexture(bitmap, imageRect, 0xffffffff, 0);
	}
}

void FrUniteResultDlg::OnTooltipOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic || !m_pCurTooltip)
		return;
	float mouseX = g_input->GetMousePoint().x;
	float mouseY = g_input->GetMousePoint().y;
	float width = m_pCurTooltip->Width();
	float height = m_pCurTooltip->Height();
	WRect rect(mouseX + 12.f, mouseY + 23.f, width, height);
	if (rect.x >= g_view->GetWidth() - width)
		rect.x = g_view->GetWidth() - width;
	if (rect.y >= g_view->GetHeight() - height)
		rect.y = g_view->GetHeight() - height;
	graphic->DrawTexture(m_pCurTooltip, rect, 0xffffffff, 0);
}

void FrUniteResultDlg::OnNextLevelTextInit(int param)
{
	if (!m_pNextLevelText)
		m_pNextLevelText = DYNAMIC_CAST(FrEdit, param);
}

FrUniteResultDlg::FrUniteResultDlg()
{
	m_pCancelBtn = NULL;
	m_pAwardInfoBtn = NULL;
	m_pRankList = NULL;
	m_pMyInfoArea = NULL;
	m_pAwardInfoDlg = NULL;
	m_pScoreDlg = NULL;
	m_pTreasureScore = NULL;
	m_pTreasureGift = NULL;
	m_pExpGaugeBar = NULL;
	m_pExpArea = NULL;
	m_pDescArea = NULL;
	m_pAwardArea = NULL;
	m_pItemArea = NULL;
	m_pTeamResult = NULL;
	m_pNextLevelText = NULL;
	for (int i = 0; i < 3; ++i)
		m_pRankBack[i] = NULL;
	m_pLevelIcon = NULL;
	m_pEmblem = NULL;
	m_pPcBonus = NULL;
	m_pMascot02 = NULL;
	m_pMascot01 = NULL;
	m_pMascot09 = NULL;
	m_pMascot10 = NULL;
	m_pMascot11 = NULL;
	m_pMascot12 = NULL;
	m_pBonusMa = NULL;
	m_pBonusMa2 = NULL;
	for (int i = 0; i < 4; ++i)
		m_pMedal[i] = NULL;
	for (int i = 0; i < 6; ++i)
		m_pPrize[i] = NULL;
	m_pIconZero = NULL;
	m_pCurTooltip = NULL;
	for (int i = 0; i < 6; ++i)
		m_pTooltip[i] = NULL;
	m_expTime = 0.f;
	m_resultDlgType = 0;
	m_bLevelUp = false;
	m_level = 0;
	m_exp = 0;
	m_maxExp = 0;
	m_addExp = 0;
	m_selectedUID = -1;
	m_teamAniTime = 0.f;
	m_elapsedTime = 0.f;
	m_myRank = 0;
	m_bRankTooltip = false;
	m_bAwardTooltip = false;
	m_pAngelFont = g_resrcmng->GetTitleFont();
	char* angel = "[font_angel.jpg";
	m_angelFontInfo.filename = &angel;
	m_angelFontInfo.flag = 0;
	m_angelFontInfo.fonth = 11;
	m_angelFontInfo.fontw = 6;
	m_angelFontInfo.numPages = 1;
	m_angelFontInfo.texw = 128;
	m_angelFontInfo.texh = 32;
	m_angelFontInfo.pCharSet = "1234567890";
	m_bTikiReport = false;
	GetLocalTime(&m_tikiReportTime);
	m_tikiReportIndex = 0;
	m_pAngelFont->Create(&m_angelFontInfo);
	static char* wind[] = { "[font_wind.jpg" };
	memset(&m_windFontInfo, 0, sizeof(m_windFontInfo));
	m_windFontInfo.filename = wind;
	m_windFontInfo.fontw = 16;
	m_windFontInfo.fonth = 16;
	m_windFontInfo.numPages = 1;
	m_windFontInfo.texw = 128;
	m_windFontInfo.texh = 64;
	m_windFontInfo.pCharSet = "1234567890ym%-.?/:,";
	m_pWindFont = g_resrcmng->GetTitleFont();
	m_pWindFont->Create(&m_windFontInfo);
}

FrUniteResultDlg::~FrUniteResultDlg()
{
	if (g_resrcmng && m_pWindFont)
	{
		g_resrcmng->Release(m_pWindFont);
		m_pWindFont = NULL;
	}
	if (g_resrcmng && m_pAngelFont)
	{
		g_resrcmng->Release(m_pAngelFont);
		m_pAngelFont = NULL;
	}
	m_bTikiReport = false;
	m_tikiReportIndex = 0;
	if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())
		CUserInfo::Instance()->Close();
	m_pCancelBtn = NULL;
}

bool FrUniteResultDlg::Close(bool bOK)
{
	if (m_pTreasureScore)
		m_pTreasureScore->Close(bOK);
	if (m_pTreasureGift)
		m_pTreasureGift->Close(bOK);
	return FrForm::Close(bOK);
}

bool FrUniteResultDlg::CheckChildDlgVisible()
{
	if (m_pAwardInfoDlg && m_pAwardInfoDlg->IsVisible())
	{
		m_pAwardInfoDlg->Close(true);
		return true;
	}
	if (m_pScoreDlg && m_pScoreDlg->IsVisible())
	{
		m_pScoreDlg->Close(true);
		return true;
	}
	if (m_pTreasureScore || m_pTreasureGift)
		return true;
	if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())
	{
		CUserInfo::Instance()->Close();
		return true;
	}
	return false;
}

bool FrUniteResultDlg::OnInit()
{
	LoadBitmapA();
	SetResultDlgType();
	SetDataToRankListBox();
	FindWinner();
	SetPrizeResult();
	int developer;
	if (IsLocalContent((localContentType_t)10) &&
		(developer = (Doc()->m_myInfo.info.dwIdentity & 4) == 4) &&
		CGMToolkit::Instance() && CGMToolkit::Instance()->GetPrizeSwitch())
		CGMToolkit::Instance()->SendPrize(m_pRankList);
	m_pCancelBtn->Enable(false);
	DisableFlag(FrESCAPE);
	m_level = Doc()->m_myInfo.stat.Level;
	m_exp = Doc()->m_myInfo.stat.dwExp * 10;
	m_maxExp = Doc()->m_levelTable[m_level].exp * 10;
	m_addExp = Doc()->m_holeStatistics[0].dwExp * 10;
	m_bDone = m_addExp == 0;
	if (m_pExpGaugeBar)
	{
		m_pExpGaugeBar->SetBorder(1);
		m_pExpGaugeBar->SetRange(0, m_maxExp, 0);
		m_pExpGaugeBar->SetPos(m_exp);
	}
	WRect rect = m_pRankList->GetRect();
	WPoint pos = m_pNextLevelText->GetRect().TopLeft();
	pos.y = rect.y + rect.h + 100.f;
	m_pNextLevelText->MoveWindow(pos);
	m_pNextLevelText->SetVisible(IsLocalContent((localContentType_t)0x29));
	unsigned long exp =
		Doc()->m_holeStatistics[0].dwExp + Doc()->m_myInfo.stat.dwExp;
	if (exp > Doc()->m_levelTable[m_level].exp)
	{
		m_pNextLevelText->SetVisible(false);
	}
	else
	{
		std::string desc;
		if (m_level < 21)
			desc.assign(s_levelUpText[m_level][2]);
		else if (m_level <= 65)
		{
			desc.assign(g_LevelTable[m_level + 1].name);
			desc += s_levelUpText[21][2];
		}
		m_pNextLevelText->AddLine(desc.c_str(), 0, false);
	}
	if (m_bTikiReport && IsLocalContent((localContentType_t)0x14))
	{
		Doc()->m_bSentHoleStat = true;
		if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			return true;
		Doc()->m_myInfo.stat.dwExp += Doc()->m_holeStatistics[0].dwExp;
		Doc()->Log30sScore();
	}
	else if (!Doc()->m_bSentHoleStat)
	{
		Doc()->m_bSentHoleStat = true;
		if ((bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
			return true;
		WSendPacket packet((enumClientPacket)6);
		packet.EncodeBuffer(&Doc()->m_holeStatistics[0],
			sizeof(sPangYaUserStatistics));
		packet.Send(TO_GAME);
		Doc()->Log30sScore();
	}
	{
		const char* title;
		char reportTitle[256];
		if (m_bTikiReport && IsLocalContent((localContentType_t)0x14))
		{
			sprintf(reportTitle, "30\xc0\xce \xb4\xeb\xc8\xb8 %d-%d-%d %d:%d",
				m_tikiReportTime.wYear, m_tikiReportTime.wMonth,
				m_tikiReportTime.wDay, m_tikiReportTime.wHour,
				m_tikiReportTime.wMinute);
			title = reportTitle;
		}
		else
		{
			if (IsMassGame(Doc()->m_roomInfo.gameType) &&
				Doc()->m_roomInfo.gameType != 10)
			{
				IFF_STRUCT::sMatch* match =
					ItemManager()->FindMatch(Doc()->m_roomInfo.tidMatch);
				if (match)
				{
					const char* matchName = Doc()->m_roomInfo.realGameType == 14
						? "\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba"
						: match->Name;
					title = matchName;
				}
				else
					title = GetGameTypeName(Doc()->m_roomInfo.gameType);
			}
			else
				title = GetGameTypeName(Doc()->m_roomInfo.gameType);
		}
		char type[32];
		memset(type, 0, sizeof(type));
		switch (m_resultDlgType)
		{
		case 0:
		{
			const char* name = "\xb0\xb3\xc0\xce\xc0\xfc";
			strcpy(type, name);
			break;
		}
		case 1:
		{
			const char* name = "\xc6\xc0\xc0\xfc";
			strcpy(type, name);
			break;
		}
		case 2:
		{
			const char* name = "\xb1\xe6\xb5\xe5\xc0\xfc";
			strcpy(type, name);
			break;
		}
		}
		SetCaption(MakeStr("            (%s %s)", title, type));
	}
	return true;
}

void FrUniteResultDlg::OnProc(const float delta)
{
	m_bRankTooltip = false;
	m_bAwardTooltip = false;
	m_expTime += delta;
	m_elapsedTime += delta;
	if (m_expTime < 1.f)
		return;
	if (m_expTime - 1.f < .01f)
		return;
	int amount = (int)(100.f * (m_expTime - 1.f));
	m_expTime -= amount * .01f;
	int maxExp = Doc()->m_levelTable[m_level].exp * 10;
	if (amount > (m_addExp > 0 ? m_addExp : -m_addExp))
	{
		m_bDone = true;
		m_exp +=
			(m_addExp > 0 ? 1 : -1) * (m_addExp > 0 ? m_addExp : -m_addExp);
		m_addExp = 0;
	}
	else
	{
		m_exp += (m_addExp > 0 ? 1 : -1) * amount;
		m_addExp -= (m_addExp > 0 ? 1 : -1) * amount;
	}
	if (m_exp < 0 && !m_bDone)
	{
		m_bDone = true;
		m_exp = 0;
	}
	if (m_exp >= maxExp)
	{
		m_exp -= maxExp;
		++m_level;
		m_bLevelUp = true;
		if (m_pExpGaugeBar)
		{
			m_pExpGaugeBar->SetRange(0, Doc()->m_levelTable[m_level].exp * 10,
				0);
			m_pExpGaugeBar->SetPos(m_exp);
		}
	}
	else if (m_pExpGaugeBar)
		m_pExpGaugeBar->SetPos(m_exp);
	m_teamAniTime += delta * 2.f;
	if (m_elapsedTime >= 17.f)
		m_pRankList->UseRightButton(false);
	if (m_bDone == true)
	{
		if (CTHunter::Instance()->IsReceiveGiftPacket() == false ||
			m_bTikiReport == true)
		{
			m_pCancelBtn->Enable(true);
			EnableFlag(FrESCAPE);
		}
		else if (CTHunter::Instance()->IsReceiveGiftPacket() == true &&
			!Doc()->m_bTreasureResult)
		{
			if (!m_pTreasureScore)
			{
				m_pTreasureScore = CreateForm<FrTreasureScore>(
					g_pFresh->GetManager(), this, "trscore");
				m_pTreasureScore->Open((FRESH_PFN_RESULT)&FrUniteResultDlg::
										   OnTreasureScoreDlgResult,
					0);
				m_pTreasureScore->EnableDrag(false);
				m_pTreasureScore->SetFixed(true);
			}
			if (!m_pTreasureGift)
			{
				m_pTreasureGift = CreateForm<FrTreasureGift>(
					g_pFresh->GetManager(), this, "trgift");
				m_pTreasureGift->MoveWindow(m_pTreasureGift->GetCalcPoint());
				m_pTreasureGift->Open((FRESH_PFN_RESULT)&FrUniteResultDlg::
										  OnTreasureGiftDlgResult,
					FrESCAPE);
				m_pTreasureGift->EnableDrag(false);
				m_pTreasureGift->SetFixed(true);
			}
			if (m_pTreasureScore && m_pTreasureGift)
				Doc()->m_bTreasureResult = true;
		}
	}
}

void FrUniteResultDlg::LoadBitmapA()
{
	m_pRankBack[0] = g_pFresh->GetBitmap("30in_team_redteam_rank");
	m_pRankBack[1] = g_pFresh->GetBitmap("30in_team_blueteam_rank");
	m_pRankBack[2] = g_pFresh->GetBitmap("30in_rank");
	m_pLevelIcon = NULL;
	m_pEmblem = NULL;
	m_pPcBonus = g_pFresh->GetBitmap("pc_icon_bonus_20");
	m_pMascot02 = g_pFresh->GetBitmap("bonus_mascot_02");
	m_pMascot01 = g_pFresh->GetBitmap("bonus_mascot_01");
	m_pMascot09 = g_pFresh->GetBitmap("bonus_mascot_09");
	m_pMascot10 = g_pFresh->GetBitmap("bonus_mascot_10");
	m_pMascot11 = g_pFresh->GetBitmap("bonus_mascot_11");
	m_pMascot12 = g_pFresh->GetBitmap("bonus_mascot_12");
	m_pBonusMa = g_pFresh->GetBitmap("bonus_ma");
	m_pBonusMa2 = g_pFresh->GetBitmap("bonus_ma2");
	m_pMedal[0] = g_pFresh->GetBitmap("rank_gold");
	m_pMedal[1] = g_pFresh->GetBitmap("rank_silver");
	m_pMedal[2] = g_pFresh->GetBitmap("rank_bronze");
	m_pMedal[3] = g_pFresh->GetBitmap("rank_noaward");
	m_pPrize[0] = g_pFresh->GetBitmap("prize_goodluck");
	m_pPrize[1] = g_pFresh->GetBitmap("prize_speeder");
	m_pPrize[2] = g_pFresh->GetBitmap("prize_runner");
	m_pPrize[3] = g_pFresh->GetBitmap("prize_chipin");
	m_pPrize[4] = g_pFresh->GetBitmap("prize_long_putting");
	m_pPrize[5] = g_pFresh->GetBitmap("prize_recovery");
	m_pIconZero = g_pFresh->GetBitmap("icon_zero");
	m_pTooltip[0] = g_pFresh->GetBitmap("tooltip_goodluck");
	m_pTooltip[1] = g_pFresh->GetBitmap("tooltip_speeder");
	m_pTooltip[2] = g_pFresh->GetBitmap("tooltip_runner");
	m_pTooltip[3] = g_pFresh->GetBitmap("tooltip_chipin");
	m_pTooltip[4] = g_pFresh->GetBitmap("tooltip_longputter");
	m_pTooltip[5] = g_pFresh->GetBitmap("tooltip_recovery");
	m_pTooltip[6] = g_pFresh->GetBitmap("tooltip_winning_contents");
}

int FrUniteResultDlg::GetMedalIndex(unsigned long uid)
{
	unsigned char intrusionCount = 0;
	if (IsLocalContent((localContentType_t)0x2e))
		intrusionCount = CIntrusion::Instance()->GetCalcExpMemberSize();
	if (intrusionCount == 0)
	{
		if (Doc()->m_rivalList.size() < 10)
			return 3;
	}
	else if (intrusionCount < 10)
	{
		return 3;
	}
	unsigned char index = 255;
	int counter = 0;
	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it, ++counter)
	{
		if (it->oid == uid)
		{
			index = (unsigned char)counter;
			break;
		}
	}
	if ((index == 255 || Doc()->m_rivalList[index].flag > 3) && !m_bTikiReport)
		return 3;
	unsigned char rank;
	if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport)
	{
		int counter = 0;
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it, ++counter)
			if (it->oid == uid)
				return Doc()->m_rivalList[counter].holeStroke[0] - 1;
	}
	else
		rank = Doc()->m_rivalList[index].rank;
	unsigned char count = 0;
	unsigned char excluded = 0;
	if (intrusionCount == 0)
	{
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
		{
			if ((it->state == 3 && GetHoleIndex(it->hole) >= 4) ||
				it->state != 3)
				++count;
			if (it->state != 3 && it->rank < rank && it->flag > 3)
				++excluded;
		}
	}
	else
	{
		count = intrusionCount;
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it)
			if (it->state != 3 && it->rank < rank && it->flag > 3)
				++excluded;
	}
	rank -= excluded;
	if (Doc()->m_golfGame.holes == 9)
	{
		if (count <= 14)
		{
			return 3;
		}
		else if (count <= 18)
		{
			if (rank == 1)
				return 2;
		}
		else if (count <= 22)
		{
			if (rank == 1)
				return 1;
		}
		else if (count <= 26)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	}
	else if (Doc()->m_golfGame.holes == 18)
	{
		if (count <= 14)
		{
			if (rank == 1)
				return 2;
		}
		else if (count <= 18)
		{
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		}
		else if (count <= 22)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		}
		else if (count <= 26)
		{
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		}
		else
		{
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}
	return 3;
}

void FrUniteResultDlg::SetResultDlgType()
{
	switch (Doc()->m_golfGame.gameType)
	{
	case 4:
		m_resultDlgType = 0;
		break;
	case 5:
		m_resultDlgType = 1;
		break;
	case 6:
		m_resultDlgType = 2;
		break;
	}
}

void FrUniteResultDlg::SetTikiReportState(_SYSTEMTIME& time)
{
	if (IsLocalContent((localContentType_t)0x14))
	{
		m_bTikiReport = true;
		m_tikiReportTime = time;
		m_tikiReportIndex = 0;
		int index = 0;
		for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
			it != Doc()->m_rivalList.end(); ++it, ++index)
			if (it->uid == MyUID())
			{
				m_tikiReportIndex = index;
				break;
			}
	}
}

void FrUniteResultDlg::SetDataToRankListBox()
{
	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if (it->state == 3)
			it->rank = 255;
		else
		{
			it->rank = 1;
			for (std::vector<sRivalData>::iterator other =
					 Doc()->m_rivalList.begin();
				other != Doc()->m_rivalList.end(); ++other)
			{
				if (it != other && other->state != 3)
				{
					if (it->totalScore > other->totalScore ||
						(it->totalScore == other->totalScore &&
							it->totalPang < other->totalPang))
						++it->rank;
				}
			}
		}
	}
	for (std::vector<sRivalData>::iterator item = Doc()->m_rivalList.begin();
		item != Doc()->m_rivalList.end(); ++item)
		m_pRankList->AddItem(&*item);
	if (m_resultDlgType == 2)
		m_pRankList->SortItem(UniteGuildIndexCompare);
	else if (g_pFresh->IsCurrentLayout("REALMYROOM"))
		m_pRankList->SortItem(ReportRankCompare);
	else
		m_pRankList->SortItem(UniteRankCompare);
	if ((Doc()->m_myInfo.info.dwIdentity & 4) &&
		IsLocalContent((localContentType_t)0x2b) &&
		Doc()->m_roomInfo.nUserLimit >= 100)
		SaveGMEvent();
}

bool FrUniteResultDlg::SetTooltip(WRect rect, int index)
{
	if (index >= 0 && index <= 6)
	{
		WPoint point(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
		if (rect.IsInRect(point))
		{
			m_pCurTooltip = m_pTooltip[index];
			CMouseCursor::Instance()->SetCursor(1);
			return true;
		}
		if (!m_bRankTooltip && !m_bAwardTooltip)
			m_pCurTooltip = NULL;
	}
	return false;
}

void FrUniteResultDlg::FindWinner()
{
	memset(Doc()->m_teamResult, 0, sizeof(Doc()->m_teamResult));
	for (std::vector<sRivalData>::iterator it = Doc()->m_rivalList.begin();
		it != Doc()->m_rivalList.end(); ++it)
	{
		if (it->rank == 1)
		{
			m_winner = *it;
			if (Doc()->m_roomInfo.gameType == 4 ||
				Doc()->m_roomInfo.gameType == 10)
				return;
		}
		switch (m_resultDlgType)
		{
		case 1:
			Doc()->m_teamResult[it->team].score += it->rank;
			Doc()->m_teamResult[it->team].pang += it->totalPang;
			break;
		case 2:
			Doc()->m_teamResult[it->team].score += it->guildPoint;
			Doc()->m_teamResult[it->team].pang += it->totalPang;
			break;
		}
	}
}

bool FrUniteResultDlg::OnAwardInfoDlgResult(int result, FrForm* form)
{
	m_pAwardInfoDlg = NULL;
	return true;
}

bool FrUniteResultDlg::OnScoreDlgResult(int result, FrForm* form)
{
	m_pScoreDlg = NULL;
	return true;
}

bool FrUniteResultDlg::OnTreasureScoreDlgResult(int result, FrForm* form)
{
	m_pTreasureScore = NULL;
	return true;
}

bool FrUniteResultDlg::OnTreasureGiftDlgResult(int result, FrForm* form)
{
	g_input->ExclusiveGetDownUseDone("ESCAPE");
	if (m_pTreasureScore)
		m_pTreasureScore->Close(true);
	if (m_pCancelBtn)
		m_pCancelBtn->Enable(true);
	EnableFlag(FrESCAPE);
	m_pTreasureGift = NULL;
	return true;
}

void FrUniteResultDlg::SetPrizeResult()
{
	m_prizeMap.clear();
	std::vector<unsigned long>& items = Doc()->m_acquireItemList[0];
	for (std::vector<unsigned long>::iterator it = items.begin();
		it != items.end(); ++it)
	{
		unsigned long type = *it;
		std::map<unsigned long, int>::iterator found = m_prizeMap.find(type);
		if (found == m_prizeMap.end())
			m_prizeMap.insert(std::make_pair(type, 1));
		else
			++(*found).second;
	}
}

bool FrUniteResultDlg::SaveGMEvent()
{
	mkdir("log");
	mkdir("log/event");
	SYSTEMTIME time;
	GetLocalTime(&time);
	FILE* file =
		fopen(MakeStr("log/event/Result_%04d_%02d_%02d_%02d_%02d.txt",
				  time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute),
			"wt");
	if (!file)
		return false;
	char buffer[512] = "";
	sprintf(buffer, "Room Info : %s > %s > %s(%d)\n",
		Doc()->m_curGameServer.name, Doc()->m_curChannel.Name,
		Doc()->m_roomInfo.title, Doc()->m_roomInfo.roomGuid);
	fwrite(buffer, strlen(buffer), 1, file);
	sprintf(buffer, "Rank : uid\tnick\t\thole\tscore\tgamepang\tbonuspang\n");
	fwrite(buffer, strlen(buffer), 1, file);
	sprintf(buffer,
		"------------------------------------------------------------\n");
	fwrite(buffer, strlen(buffer), 1, file);
	char hole[32] = "";
	int rank = 1;
	for (FrListBox::ITEM_LIST::iterator it = m_pRankList->m_itemList.begin();
		it != m_pRankList->m_itemList.end(); ++it)
	{
		sRivalData* rival = (sRivalData*)(*it)->pData;
		if (rival->state == 2 || GetHoleIndex(rival->hole) == 19)
			sprintf(hole, "End");
		else if (rival->state == 3)
			sprintf(hole, "Out");
		else
			sprintf(hole, "%d", GetHoleIndex(rival->hole));
		sprintf(buffer, "%03d : %d\t%s\t%s\t%d\t%I64d\t%I64d\n", rank++,
			rival->uid, rival->nickname, hole, rival->totalScore,
			rival->totalPang, rival->totalBonusPang);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer,
		"------------------------------------------------------------\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	return true;
}

IMPLEMENT_OBJECT(FrAwardInfoDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrAwardInfoDlg, FrForm)
END_FRESH_MSGMAP()

FrAwardInfoDlg::FrAwardInfoDlg()
{
	m_elapsedTime = 0.0f;
}

FrAwardInfoDlg::~FrAwardInfoDlg()
{
}

bool FrAwardInfoDlg::OnInit()
{
	(void)GetRect();
	SetCaption("\xbc\xf6\xbb\xf3\xb3\xbb\xbf\xeb");
	return true;
}

void FrAwardInfoDlg::OnProc(const float delta)
{
	m_elapsedTime += delta;
}
