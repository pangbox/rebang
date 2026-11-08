#include "s5/demonlanddlg.h"
#include "s5/utilities.h"
#include "netresourcemanager.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

bool FrDemonNextHole::OnInit()
{
	m_pPatternBitmap = g_pFresh->GetBitmap("demon_nexthole_pattern");
	if (!m_pPatternBitmap)
		return false;
	unsigned long course = GetCurMap() > 127 && GetCurMap() != 253
		? GetCurMap() - 128
		: GetCurMap();
	IFF_STRUCT::sCourse* info = ItemManager()->FindCourse(course | 0x28000000);
	if (!info)
		return false;
	m_pCoursePage[0] =
		g_resrcmng->GetOverlay(MakeStr("%s_1.png", info->c.Name), 0);
	m_pCoursePage[1] =
		g_resrcmng->GetOverlay(MakeStr("%s_2.png", info->c.Name), 0);
	if (m_pCoursePage[0] || m_pCoursePage[1])
	{
		m_imageWidth = m_pCoursePage[0]->GetWidth();
		m_imageHeight = m_pCoursePage[0]->GetHeight();
	}
	return FrForm::OnInit();
}

void FrDemonNextHole::SetNextHole(int hole)
{
	if (m_pCoursePage[0] && m_pCoursePage[1])
	{
		if (hole <= 10 && hole != 19 &&
			Doc()->m_holeOrder[GetHoles() - 1] != GOLFDOC()->m_currentHole)
		{
			m_page = 0;
		}
		else
		{
			m_page = 1;
			hole -= 10;
		}
		if (hole == 19 ||
			Doc()->m_holeOrder[GetHoles() - 1] == GOLFDOC()->m_currentHole)
			S5::UTIL::CalculateUVPos(m_sourceRect, m_imageWidth, m_imageHeight,
				140.0f, 93.0f, 8);
		else
			S5::UTIL::CalculateUVPos(m_sourceRect, m_imageWidth, m_imageHeight,
				140.0f, 93.0f, hole - 1);
	}
}

const Bitmap* GetTitleBitmap(const sRivalData* rival)
{
	if (rival->titleTypeId)
	{
		IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(rival->titleTypeId);
		if (skin)
			return g_pFresh->GetManager()->GetBitmap("TITLES", skin->c.Icon);
		else
			return g_pFresh->GetManager()->GetBitmap("LEVELS",
				MakeStr("level_%03d", rival->level + 1));
	}
	else
	{
		return g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("level_%03d", rival->level + 1));
	}
}

void FrDemonProcessRank::OnListBoxUserInfoList_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi || !param)
		return;
	FrListItem* item = (FrListItem*)param;
	sRivalData* rival = (sRivalData*)item->pData;
	WRect titleRect;
	const Bitmap* titleBitmap = GetTitleBitmap(rival);
	gi->DrawTexture(titleBitmap,
		WRect(titleRect.x, titleRect.y + 8.0f - (int)(titleRect.h * 0.5f),
			titleRect.w, titleRect.h),
		0xffffffff, 0);
	if (rival->state == 3)
		gi->Print(WPoint(item->pos.x + 21.0f, item->pos.y + 3.0f), 1, "-");
	else
		gi->Print(WPoint(item->pos.x + 21.0f, item->pos.y + 3.0f), 1, "%d",
			rival->rank);
	if (rival->guildUID)
	{
		const Bitmap* emblem =
			NetResourceManager::Instance()->GetEmblemByName(rival->guildMark);
		if (emblem)
			gi->DrawTexture(emblem,
				WRect(item->pos.x + 33.0f, item->pos.y + 3.0f - 6.0f,
					(float)emblem->Width(), (float)emblem->Height()),
				0xffffffff, 0);
	}
	g_pFresh->GetManager()->PrintText(
		WPoint(item->pos.x + 60.0f, item->pos.y + 3.0f), 0, rival->nickname,
		90.0f, 0xffffffff);
	gi->Print(WPoint(item->pos.x + 179.0f, item->pos.y + 3.0f), 2, "%d",
		rival->totalScore);
	gi->Print(WPoint(item->pos.x + 222.0f, item->pos.y + 3.0f), 2, "%I64d",
		rival->totalPang);
	if (rival->state == 2 || GetHoleIndex(rival->hole) == 19)
		gi->Print(WPoint(item->pos.x + 255.0f, item->pos.y + 3.0f), 2, "End");
	else if (rival->state == 3)
		gi->Print(WPoint(item->pos.x + 255.0f, item->pos.y + 3.0f), 2, "Out");
	else
		gi->Print(WPoint(item->pos.x + 255.0f, item->pos.y + 3.0f), 2,
			"%d\xc8\xa6", GetHoleIndex(rival->hole));
}

void FrDemonNextHole::SetState(unsigned char state)
{
	if (m_state == state)
	{
		if (IsEnabled() && m_state == 1)
		{
			Enable(false);
			SetVisible(false);
		}
		return;
	}
	m_state = state;
	if (m_state == 1)
	{
		Enable(false);
		SetVisible(false);
		return;
	}
	Enable(true);
	SetVisible(true);
	SetNextHole(GetNextHole());
}

IMPLEMENT_OBJECT(FrDemonNextHole, FrForm)

BEGIN_FRESH_MSGMAP(FrDemonNextHole, FrForm)
ON_FRESH_VI("nextholepattern", FRCMD_INIT, FrDemonNextHole::OnAreaPattern_Init)
ON_FRESH_VI("nextholepattern", FRCMD_OWNERDRAW,
	FrDemonNextHole::OnAreaPattern_OwnerDraw)
ON_FRESH_VI("nexthole", FRCMD_INIT, FrDemonNextHole::OnAreaNextHole_Init)
ON_FRESH_VI("nexthole", FRCMD_OWNERDRAW,
	FrDemonNextHole::OnAreaNextHole_OwnerDraw)
END_FRESH_MSGMAP()

FrDemonNextHole::FrDemonNextHole()
{
	m_pPattern = NULL;
	m_pNextHole = NULL;
	m_pPatternBitmap = NULL;
	m_state = 1;
}

FrDemonNextHole::~FrDemonNextHole()
{
}

void FrDemonNextHole::OnAreaPattern_Init(int param)
{
	m_pPattern = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDemonNextHole::OnAreaPattern_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi && m_pPatternBitmap)
	{
		WRect rect(GetRect().x + 120.0f, GetRect().y + 110.0f, 63.0f, 35.0f);
		gi->DrawTexture(m_pPatternBitmap, rect, 0xffffffff, 0);
	}
}

void FrDemonNextHole::OnAreaNextHole_Init(int param)
{
	m_pNextHole = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDemonNextHole::OnAreaNextHole_OwnerDraw(int param)
{
	if (m_state != 1)
	{
		WRect rect(m_pNextHole->GetRect().TopLeft(), WSize(140.0f, 93.0f));
		m_pCoursePage[m_page]->Render(g_view, m_sourceRect, rect, 0, 0xffffffff,
			0.0f, 0);
	}
}

IMPLEMENT_OBJECT(FrDemonAttackGauge, FrForm)

BEGIN_FRESH_MSGMAP(FrDemonAttackGauge, FrForm)
ON_FRESH_VI("attackgauge", FRCMD_INIT,
	FrDemonAttackGauge::OnImageBarAttackPoint_Init)
ON_FRESH_VI("gaugemarker", FRCMD_INIT,
	FrDemonAttackGauge::OnAreaAttackMarker_Init)
ON_FRESH_VI("gaugemarker", FRCMD_OWNERDRAW,
	FrDemonAttackGauge::OnAreaAttackMarker_OwnerDraw)
ON_FRESH_VI("blingbar", FRCMD_INIT, FrDemonAttackGauge::OnAreaBlingBar_Init)
ON_FRESH_VI("blingbar", FRCMD_OWNERDRAW,
	FrDemonAttackGauge::OnAreaBlingBar_OwnerDraw)
END_FRESH_MSGMAP()

FrDemonAttackGauge::FrDemonAttackGauge()
{
	m_pAttackGauge = NULL;
	m_pAttackMarker = NULL;
	m_pMarkerBitmap = NULL;
}

FrDemonAttackGauge::~FrDemonAttackGauge()
{
}

bool FrDemonAttackGauge::OnInit()
{
	m_pMarkerBitmap = g_pFresh->GetBitmap("demon_attack_point");
	m_pBlingBitmap = g_pFresh->GetBitmap("demon_gauge_light");
	return FrForm::OnInit();
}

void FrDemonAttackGauge::SetState(unsigned char state)
{
	if (m_state != state)
	{
		m_state = state;
		if (m_state == 1)
		{
			Enable(false);
			SetVisible(false);
		}
		else
		{
			Enable(true);
			SetVisible(true);
		}
	}
}

void FrDemonAttackGauge::OnImageBarAttackPoint_Init(int param)
{
	m_pAttackGauge = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pAttackGauge)
	{
		m_pAttackGauge->SetRange(0, 1000, 0);
		m_pAttackGauge->SetBarSpeed(250.0f);
		m_pAttackGauge->SetDestPos(1000, false);
	}
}

void FrDemonAttackGauge::OnAreaAttackMarker_Init(int param)
{
	m_pAttackMarker = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDemonAttackGauge::OnAreaAttackMarker_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi && m_pMarkerBitmap)
	{
		WRect rect(GetRect().x + 50.0f, GetRect().y + 50.0f, 24.0f, 15.0f);
		gi->DrawTexture(m_pMarkerBitmap, rect, 0xffffffff, 0);
	}
}

void FrDemonAttackGauge::OnAreaBlingBar_Init(int param)
{
	m_pBlingBar = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDemonAttackGauge::OnAreaBlingBar_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi && m_pBlingBitmap)
	{
		FrGaugeBarImage* gauge = m_pAttackGauge;

		const float x = gauge->GetRect().x - (m_pBlingBitmap->Width() >> 1) +
			gauge->GetPixelPerHeight();
		const unsigned height = m_pBlingBitmap->Height();
		{
			WRect rect;
			rect.x = x;
			rect.y = gauge->GetRect().y - (height >> 1) + 15.0f;
			rect.w = 33.0f;
			rect.h = 68.0f;
			gi->DrawTexture(m_pBlingBitmap, rect, 0xffffffff, 0);
		}
	}
}

IMPLEMENT_OBJECT(FrDemonProcessRank, FrForm)

BEGIN_FRESH_MSGMAP(FrDemonProcessRank, FrForm)
ON_FRESH_VI("leaderpet", FRCMD_INIT, FrDemonProcessRank::OnAreaLeaderPet_Init)
ON_FRESH_VI("leaderpet", FRCMD_OWNERDRAW,
	FrDemonProcessRank::OnAreaLeaderPet_OwnerDraw)
ON_FRESH_VI("attackerlist", FRCMD_INIT,
	FrDemonProcessRank::OnListBoxUserInfoList_Init)
ON_FRESH_VI("attackerlist", FRCMD_OWNERDRAW,
	FrDemonProcessRank::OnListBoxUserInfoList_OwnerDraw)
END_FRESH_MSGMAP()

FrDemonProcessRank::FrDemonProcessRank()
{
	m_pUserInfoList = NULL;
}

FrDemonProcessRank::~FrDemonProcessRank()
{
}

bool FrDemonProcessRank::OnInit()
{
	return FrForm::OnInit();
}

void FrDemonProcessRank::OnAreaLeaderPet_OwnerDraw(int param)
{
}

void FrDemonProcessRank::OnAreaLeaderPet_Init(int param)
{
	m_pLeaderPet = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDemonProcessRank::OnListBoxUserInfoList_Init(int param)
{
	m_pUserInfoList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

IMPLEMENT_OBJECT(FrDemonFinalScore, FrForm)

BEGIN_FRESH_MSGMAP(FrDemonFinalScore, FrForm)
ON_FRESH_VI("attackgauge", FRCMD_INIT,
	FrDemonFinalScore::OnImageBarAttackPoint_Init)
ON_FRESH_VI("giftlist", FRCMD_INIT, FrDemonFinalScore::OnInit_Gift_List)
ON_FRESH_VI("giftlist", FRCMD_OWNERDRAW,
	FrDemonFinalScore::OnOwnerDraw_Gift_List)
END_FRESH_MSGMAP()

FrDemonFinalScore::FrDemonFinalScore()
	: m_pAttackGauge(NULL)
{
}

FrDemonFinalScore::~FrDemonFinalScore()
{
}

bool FrDemonFinalScore::OnInit()
{
	if (!m_boxOpener.OnInit())
		return false;
	return FrForm::OnInit();
}

void FrDemonFinalScore::OnProc(const float dt)
{
	m_boxOpener.OnProc(dt);
}

void FrDemonFinalScore::OnOwnerDraw_Gift_List(int param)
{
	if (param)
		m_boxOpener.DrawGiftList((FrListItem*)param);
}

void FrDemonFinalScore::OnImageBarAttackPoint_Init(int param)
{
	m_pAttackGauge = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pAttackGauge)
	{
		m_pAttackGauge->SetRange(0, 1000, 0);
		m_pAttackGauge->SetBarSpeed(250.0f);
		m_pAttackGauge->SetDestPos(1000, false);
	}
}

void FrDemonFinalScore::OnInit_Gift_List(int param)
{
	m_boxOpener.m_pGiftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

IMPLEMENT_OBJECT(FrDemonLandTicket, FrForm)

BEGIN_FRESH_MSGMAP(FrDemonLandTicket, FrForm)
ON_FRESH_VI("ticketmsg", FRCMD_INIT, FrDemonLandTicket::OnInit_TicketMsg_Static)
END_FRESH_MSGMAP()

FrDemonLandTicket::FrDemonLandTicket()
{
	m_pTicketMessage = NULL;
	m_ticketNumber = 0;
}

FrDemonLandTicket::~FrDemonLandTicket()
{
}

void FrDemonLandTicket::SetTicketNumber(int number)
{
	m_ticketNumber = number;
	if (m_pTicketMessage)
		m_pTicketMessage->SetCaption(MakeStr(
			"\xb4\xeb\xb1\xe2\xb9\xf8\xc8\xa3 : %d \xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xb8\xb8 \xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbc\xbc\xbf\xe4.",
			number));
}

void FrDemonLandTicket::OnInit_TicketMsg_Static(int param)
{
	m_pTicketMessage = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pTicketMessage)
		m_pTicketMessage->SetCaption(
			"\xb8\xb6\xbf\xd5\xc0\xc7 \xb6\xa5 \xc0\xd4\xc0\xe5 \xbf\xe4\xc3\xbb \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.");
}

CDemonLandForm::CDemonLandForm()
{
	InitializeForm();
}

CDemonLandForm::~CDemonLandForm()
{
}

void CDemonLandForm::InitializeForm()
{
	m_pNextHole = NULL;
	m_pAttackGauge = NULL;
	m_pProcessRank = NULL;
	m_pFinalScore = NULL;
}

FrDemonNextHole* CDemonLandForm::GetDemonNextHole()
{
	return m_pNextHole;
}

FrDemonAttackGauge* CDemonLandForm::GetDemonAttackGauge()
{
	return m_pAttackGauge;
}

FrDemonProcessRank* CDemonLandForm::GetDemonProcessRank()
{
	return m_pProcessRank;
}

FrDemonFinalScore* CDemonLandForm::GetDemonFinalScore()
{
	return m_pFinalScore;
}

void CDemonLandForm::ReleaseForm()
{
}

void CDemonLandForm::AdjustWindowPos()
{
	if (m_pNextHole && m_pAttackGauge)
	{
		WRect rect;
		AdjustNextHolePos(rect);
		AdjustAttackGaugePos(rect);
	}
}

void CDemonLandForm::AdjustNextHolePos(WRect& rect)
{
	const WRect& current = m_pNextHole->GetRect();
	rect.x = 0.0f;
	rect.y = current.h;
	rect.w = current.w;
	rect.h = current.h;
	rect.x = g_view->GetWidth() * 0.5f - rect.w;
	m_pNextHole->SetRect(rect);
}

void CDemonLandForm::AdjustAttackGaugePos(WRect& rect)
{
	const WRect& current = m_pAttackGauge->GetRect();
	rect.x = 0.0f;
	rect.y = current.y;
	rect.w = current.w;
	rect.h = current.h;
	rect.x = g_view->GetWidth() * 0.5f + current.w;
	m_pAttackGauge->SetRect(rect);
}

bool CDemonLandForm::CreateDemonLandForm(eDemonDlg dlg)
{
	switch (dlg)
	{
	case DEMON_NEXT_HOLE:
		m_pNextHole = CreateForm<FrDemonNextHole>(g_pFresh->GetManager(), NULL,
			"demon_nexthole", NULL);
		m_pNextHole->EnableDrag(false);
		m_pNextHole->Open(NULL, 12);
		m_pNextHole->SetState(1);
		break;
	case DEMON_ATTACK_GAUGE:
		m_pAttackGauge = CreateForm<FrDemonAttackGauge>(g_pFresh->GetManager(),
			NULL, "demon_attackgauge", NULL);
		m_pAttackGauge->EnableDrag(false);
		m_pAttackGauge->Open(NULL, 12);
		break;
	case DEMON_PROCESS_RANK:
		m_pProcessRank = CreateForm<FrDemonProcessRank>(g_pFresh->GetManager(),
			NULL, "demon_processrank", NULL);
		m_pProcessRank->EnableDrag(false);
		m_pProcessRank->SetFixed(true);
		m_pProcessRank->Open(NULL, 12);
		break;
	case DEMON_FINAL_SCORE:
		break;
	default:
		return false;
	}
	return true;
}
