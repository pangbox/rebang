#include "stdafx.h"
#include "s5/utilities.h"
typedef std::list<struct sClubSet*>::iterator ClubModelIterator; // HACK!
#include "club.h"
#include "golfdoc.h"
#include "golfball.h"
#include "quadtree.h"
#include "autotarget.h"
#include "polysoup.h"
#include "inputmanager.h"
#include "chatmsg.h"
#include "quitwindow.h"
#include "itemwindow.h"
#include "cardsystem.h"
#include "golftask.h"
#include "helpmsg.h"
#include "hackingmanager.h"
#include "soundmanager.h"
#include "actor.h"
#include "taskmanager.h"
#include "slicestr.h"
#include "wresrcmng.h"
#include "wpuppet.h"
#include "packet.h"
#include "../../shared/localize.h"
#include "mathconsts.h"

const localContentType_t S4_RANGE_BUFF = (localContentType_t)0x70;
extern WMatrix g_camera;
extern Fresh* g_pFresh;

namespace S5
{
	bool IsUsableCalipers();
}

void PerformPowerBarSetting();
bool IsMyTurn(bool check);
sPlayerData* PLAYER(unsigned char index);

char* clubName[] = { "1W", "2W", "3W", "4W", "5W", "2I", "3I", "4I", "5I", "6I",
	"7I", "8I", "9I", "SW", "PW", "1PT", "2PT", NULL };

float bar_speed[17] = { 460.0f, 460.0f, 460.0f, 460.0f, 460.0f, 440.0f, 440.0f,
	410.0f, 410.0f, 390.0f, 390.0f, 390.0f, 390.0f, 360.0f, 360.0f, 200.0f,
	200.0f };

float ground_accuracy[15] = { 8.0f, 8.0f, 0.0f, 1.0f, 5.0f, 5.0f, 5.0f, 5.0f,
	5.0f, 0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f };

inline bool IsUsedItembyMe(unsigned long item)
{
	if (IsMyTurn(false))
	{
		unsigned char cur = GOLFDOC()->m_currentPlayer;
		return GOLFDOC()->GetPlayer(cur)->item == item ? true : false;
	}
	return false;
}

CClub::CClub()
	: m_pClub(NULL), m_clubList(16, 8)
{
	Init();
}

CClub& GolfClub()
{
	static CClub s_club;
	return s_club;
}

CClub::~CClub()
{
	for (sClub* p = m_clubList.Start(); p; p = m_clubList.Next())
	{
		if (p)
			delete p;
	}
	m_clubList.Reset();
}

void CClub::ClearClubSet()
{
	for (ClubModelIterator it = m_clubSetList.begin();
		it != m_clubSetList.end(); it++)
	{
		sClubSet* pSet = *it;
		for (int i = 0; i < 4; i++)
		{
			if (g_resrcmng)
			{
				if (pSet->puppet[i])
				{
					g_resrcmng->Release(pSet->puppet[i], false);
					pSet->puppet[i] = NULL;
				}
				if (g_resrcmng && pSet->uiPuppet[i])
				{
					g_resrcmng->Release(pSet->uiPuppet[i], false);
					pSet->uiPuppet[i] = NULL;
				}
			}
		}
		delete pSet;
	}
	m_clubSetList.clear();
	m_bCalcRandomPower = false;
}

void CClub::Init()
{
	AddClub("1W", 230.0f, 10.0f, 236.0f, 1.61f, 0.55f, 0);
	AddClub("2W", 210.0f, 13.0f, 204.0f, 1.41f, 0.5f, 0);
	AddClub("3W", 190.0f, 16.0f, 176.0f, 1.26f, 0.45f, 0);
	AddClub("2I", 180.0f, 20.0f, 161.0f, 1.07f, 0.45f, 1);
	AddClub("3I", 170.0f, 24.0f, 149.0f, 0.95f, 0.45f, 1);
	AddClub("4I", 160.0f, 28.0f, 139.0f, 0.83f, 0.45f, 1);
	AddClub("5I", 150.0f, 32.0f, 131.0f, 0.73f, 0.45f, 1);
	AddClub("6I", 140.0f, 36.0f, 124.0f, 0.67f, 0.41f, 1);
	AddClub("7I", 130.0f, 40.0f, 118.0f, 0.61f, 0.36f, 1);
	AddClub("8I", 120.0f, 44.0f, 114.0f, 0.57f, 0.3f, 1);
	AddClub("9I", 110.0f, 48.0f, 110.0f, 0.53f, 0.25f, 1);
	AddClub("PW", 100.0f, 52.0f, 107.0f, 0.49f, 0.18f, 2);
	AddClub("SW", 80.0f, 56.0f, 93.0f, 0.42f, 0.17f, 2);
	AddClub("1PT", 20.0f, 0.0f, 30.0f, 0.0f, 0.0f, 3);
	AddClub("2PT", 10.0f, 0.0f, 21.0f, 0.0f, 0.0f, 3);
	m_reserved = 0;
	m_powerShot = 0;
	m_approachMode = 4;
	m_barAlpha = 0;
	m_barWidth = 0;
	m_bar = 2;
	m_bItemWindow = false;
	m_prevIndex = 0;
	m_randomPower = 0;
	m_bCalcRandomPower = false;
	m_pClub = m_clubList.Find("1W");
	m_curIndex = m_pClub->index;
}

void CClub::AddCrypticValue()
{
	if (HackingManager::Instance())
	{
		HackingManager::Instance()->AddCrypticIntValue(&m_powerShot);
		for (sClub* p = m_clubList.Start(); p; p = m_clubList.Next())
		{
			HackingManager::Instance()->AddCrypticFloatValue(&p->range);
			HackingManager::Instance()->AddCrypticFloatValue(&p->lie);
			HackingManager::Instance()->AddCrypticFloatValue(&p->power);
			HackingManager::Instance()->AddCrypticFloatValue(&p->curve);
			HackingManager::Instance()->AddCrypticFloatValue(&p->spin);
			HackingManager::Instance()->AddCrypticFloatValue(&p->barSpeed);
		}
	}
}

void CClub::Process(float deltaTime)
{
	if (GOLFDOC()->m_tutorialMode >= 1 &&
		(!IsLocalContent(S4_REPLAY_SYSTEM) || Doc()->m_playingMode != 2))
	{
		if (IsMyTurn(false))
		{
			ClubProcess(deltaTime);
			BarProcess(deltaTime);
		}
		if (m_prevIndex != m_curIndex)
		{
			m_flag.Enable(1);
			if (GetType() != GetType(m_prevIndex))
				m_flag.Enable(2);
			else
				m_flag.Disable(2);
			GOLFDOC()->m_bar.gauge =
				m_pClub->type == 3 ? s_barStart : s_barGaugeStart;
			GOLFDOC()->m_bar.gaugeStep = GOLFDOC()->m_bar.gauge;
			if (IsMyTurn(false) && (GOLFDOC()->m_gameMode & 0x200) &&
				m_curIndex < m_prevIndex && m_approachMode == 4)
			{
				WMatrix mat;
				g_camera = GOLFDOC()->MakeTopViewCameraMatrix(true);
			}
			m_prevIndex = m_curIndex;
		}
		else
		{
			m_flag.Disable(3);
		}
	}
}

void CClub::ClubProcess(float deltaTime)
{
	if (g_pFresh->GetManager()->GetWheelFocus() != NULL)
		return;
	if (CQuitWindow::Instance()->IsActive())
		return;
	if (!CItemWindow::IsClosed())
		return;
	if (CChatMsg::Instance()->IsOpened())
		return;
	if (g_input->Get("LALT", true))
		return;
	if (g_input->Get("RALT", true))
		return;
	int state;
	MsgObject msg(NULL, 0x199, (int)&state, 0, 0, 0, 0);
	AfxGetTask()->GetActor("Player") << msg;
	if (state == 3)
		return;
	if (GOLFDOC()->m_tutorialMode == 10)
		return;
	WVector delta = g_input->GetMouseDelta();
	if (m_curIndex < m_clubList.GetCount() - 1 &&
		(delta.z < 0.0f ||
			((g_input->GetDown("PAGE_DOWN", true) ||
				 g_input->GetDown("PAD_PLUS", true)) &&
				!g_input->Get("LSHIFT", true) &&
				!g_input->Get("RSHIFT", true)) ||
			(g_input->GetDown("\xc1\xc2", true) &&
				(g_input->Get("LSHIFT", true) ||
					g_input->Get("RSHIFT", true)) &&
				!g_input->Get("LCONTROL", true) &&
				!g_input->Get("RCONTROL", true))))
	{
		if (m_powerShot != 0)
		{
			m_pClub = SelectClub(m_curIndex + 1);
			if (m_pClub == NULL || m_pClub->type == 3)
			{
				m_pClub = SelectClub(m_curIndex);
				g_audio->PlaySfx("play_unplayable");
				CHelpMsg::Instance()->SetHelpMsg(0x12, NULL);
			}
			else
			{
				m_curIndex++;
			}
		}
		else
		{
			m_curIndex++;
			m_pClub = SelectClub(m_curIndex);
		}
		if (S5::IsUsableCalipers() == true ||
			IsUsedItembyMe(0x18000009) == true)
			PerformPowerBarSetting();
		g_audio->PlaySfx("\xc5\xac\xb7\xb4\xb1\xb3\xc3\xbc");
	}
	if (!(delta.z > 0.0f) &&
		(((!g_input->GetDown("PAGE_UP", true) &&
			  !g_input->GetDown("PAD_MINUS", true)) ||
			 g_input->Get("LSHIFT", true)) ||
			g_input->Get("RSHIFT", true)))
	{
		if (!g_input->GetDown("\xbf\xec", true))
			return;
		if (!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))
			return;
		if (g_input->Get("LCONTROL", true))
			return;
		if (g_input->Get("RCONTROL", true))
			return;
	}
	if (IsLocalContent(S3_SAFETEE_ITEM) &&
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
		m_limitIndex = 0;
	if (m_curIndex > m_limitIndex)
	{
		m_curIndex--;
		m_pClub = SelectClub(m_curIndex);
		g_audio->PlaySfx("\xc5\xac\xb7\xb4\xb1\xb3\xc3\xbc");
		if (S5::IsUsableCalipers() == true ||
			IsUsedItembyMe(0x18000009) == true)
			PerformPowerBarSetting();
		return;
	}
	if (CHelpMsg::Instance() && m_limitIndex > 0)
	{
		g_audio->PlaySfx("play_unplayable");
		CHelpMsg::Instance()->SetHelpMsg(0x10, NULL);
	}
}

void CClub::BarProcess(float deltaTime)
{
	switch (m_bar)
	{
	case 0:
		if (m_flag.GetFlag(1))
			m_barDelay = 0;
		m_barDelay += deltaTime;
		if (m_barDelay > .7f || CChatMsg::Instance()->IsActive())
			m_bar = 1;
		break;
	case 1:
		if (m_flag.GetFlag(1) &&
			(!CChatMsg::Instance()->IsActive() || !m_bItemWindow))
		{
			m_bar = 3;
			m_barAlpha = 0;
			m_barWidth = 456.f;
		}
		else
		{
			m_barWidth -= deltaTime * 1000.f;
			if (m_barWidth < 0.f)
			{
				m_barWidth = 0;
				m_bar = 2;
				if (OnlinePlay())
				{
					WSendPacket packet((enumClientPacket)0x16);
					packet.Encode1(m_curIndex);
					packet.Send(TO_GAME);
				}
			}
		}
		break;
	case 3:
		m_barAlpha += deltaTime * 765.f;
		if (m_barAlpha > 255.f)
		{
			m_barDelay = 0;
			m_bar = 0;
			m_barAlpha = 255.f;
		}
		break;
	case 2:
		if (m_flag.GetFlag(1) && !CChatMsg::Instance()->IsActive() &&
			!m_bItemWindow)
		{
			if (GolfBall().m_groundType == 2)
			{
				if (OnlinePlay())
				{
					WSendPacket packet((enumClientPacket)0x16);
					packet.Encode1(m_curIndex);
					packet.Send(TO_GAME);
				}
			}
			else
			{
				m_bar = 3;
				m_barAlpha = 0;
				m_barWidth = 456.f;
			}
		}
		break;
	}
}

void CClub::AddClub(char* name, float range, float lie, float power,
	float curve, float spin, unsigned char type)
{
	sClub* pClub = m_clubList.Find(name);
	if (pClub)
	{
		pClub->range = range;
		pClub->lie = (lie * g_DEGTORAD);
		pClub->type = type;
		pClub->power = power;
		pClub->curve = curve;
		pClub->spin = spin;
		return;
	}
	pClub = new sClub;
	pClub->pointFactor = 1.0f;
	pClub->range = range;
	pClub->lie = (lie * g_DEGTORAD);
	pClub->type = type;
	pClub->power = power;
	pClub->curve = curve;
	pClub->spin = spin;
	pClub->index = (unsigned char)m_clubList.GetCount();
	strcpy(pClub->name, name);
	m_clubList.AddItem(pClub, pClub->name, false);
	for (unsigned char i = 0; i < 17; i++)
	{
		if (strcmp(pClub->name, clubName[i]) == 0)
		{
			pClub->id = i;
			break;
		}
	}
	pClub->barSpeed = bar_speed[pClub->id];
	if (HackingManager::Instance())
	{
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->range);
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->lie);
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->power);
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->curve);
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->spin);
		HackingManager::Instance()->AddCrypticFloatValue(&pClub->barSpeed);
	}
}

CClub::sClub* CClub::SelectClub(unsigned char index)
{
	sClub* pClub = m_clubList.Start();
	for (unsigned char i = 0; i < index; i++)
		pClub = m_clubList.Next();
	return pClub;
}

void CClub::SelectClubByDist(float dist)
{
	m_curIndex = m_clubList.GetCount() - 1;
	m_pClub = m_clubList.End();
	while (m_pClub && m_curIndex != m_limitIndex &&
		((m_powerShot != 0 && m_pClub->type == 3) || !(GetRange() > dist)))
	{
		m_pClub = m_clubList.Prev();
		m_curIndex--;
	}
	if (m_pClub == NULL)
	{
		m_curIndex = 0;
		m_pClub = m_clubList.Find("1W");
	}
}

void CClub::SelectBestClub()
{
	WVector pin = GOLFDOC()->GetHoleData().pin;
	SelectBestClub(pin);
	if (S5::IsUsableCalipers())
		PerformPowerBarSetting();
}

void CClub::SelectBestClub(const WVector& pin)
{
	const WVector& pos = GolfBall().m_pos;
	float dx = pos.x - pin.x;
	float dz = pos.z - pin.z;
	m_limitIndex = 0;
	dx *= dx;
	float dist = sqrtf(dz * dz + dx) * 0.3125f;
	const volatile float& storedDistance = dist;
	if (storedDistance < 10.0f)
		m_approachMode = 0;
	else if (dist < 15.0f)
		m_approachMode = 1;
	else if (dist < 28.0f)
		m_approachMode = 2;
	else if (dist < 58.0f)
		m_approachMode = 3;
	else
		m_approachMode = 4;
	SetAccuracy();
	if (GolfBall().m_groundType == 2)
	{
		switch (m_approachMode)
		{
		case 0:
			m_bPowerPutter = 0;
			m_pClub = m_clubList.Find("2PT");
			break;
		case 1:
			m_bPowerPutter = 0;
			m_pClub = m_clubList.Find("1PT");
			break;
		case 2:
			m_bPowerPutter = 1;
			m_pClub = m_clubList.Find("2PT");
			break;
		case 3:
		case 4:
			m_bPowerPutter = 1;
			m_pClub = m_clubList.Find("1PT");
			break;
		}
		m_curIndex = m_pClub->index;
		m_limitIndex = 13;
		GOLFDOC()->m_bar.gauge = 140.0f;
		GOLFDOC()->m_bar.gaugeStep = GOLFDOC()->m_bar.gauge;
	}
	else
	{
		m_bPowerPutter = 1;
		float best = -10000.0f;
		int meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
		WVector dir =
			GOLFDOC()->m_pAutoTarget->GetPoint("1W") - GolfBall().m_pos;
		dir.y = 0.f;
		dir.Normalize();
		bool bHazard = false;
		float r, f = 0.f;
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].stat.Level < 6 &&
			GetDifficulty() < 3)
		{
			r = GetRange("1W") * 3.2f;
			for (f = 0.8f; f < 1.01f; f += 0.1f)
			{
				if (GetPVS().GetGroundType(GolfBall().m_pos + dir * (f * r),
						true, NULL) == 10)
				{
					bHazard = true;
					break;
				}
				if (GOLFDOC()->m_pPolySoup->IsOutOfBound(
						GolfBall().m_pos + dir * (f * r)))
				{
					bHazard = true;
					break;
				}
			}
		}
		for (sClub* pClub = m_clubList.Start(); pClub;
			pClub = m_clubList.Next())
		{
			if (pClub->index < 13)
			{
				if (pClub->lie <
					GetPVS().GetGroundSlope(meshIndex) + 0.06981317f)
				{
					m_limitIndex++;
				}
				else
				{
					float range = GetRange(pClub->name);
					float score = range - dist;
					score = score > 0 ? score : -score;
					score = pClub->barSpeed * 0.5f - score;
					if (range > dist)
						score += 10.0f;
					if (bHazard)
					{
						r = range * 3.2f;
						for (f = 0.8f; f < 1.01f; f += 0.1f)
						{
							if (GOLFDOC()->m_pPolySoup->IsOutOfBound(
									GolfBall().m_pos + dir * (f * r)))
							{
								score -= 1000.0f;
							}
							else
							{
								switch (GetPVS().GetGroundType(
									GolfBall().m_pos + dir * (f * r), true,
									NULL))
								{
								case 3:
									score -= 200.0f;
									break;
								case 4:
								case 6:
								case 7:
								case 11:
									score -= 100.0f;
									break;
								case 10:
									score -= 1000.0f;
									break;
								}
							}
						}
					}
					score *= pClub->pointFactor;
					if (score > best)
					{
						m_pClub = pClub;
						m_curIndex = pClub->index;
						best = score;
					}
				}
			}
		}
		if (m_approachMode < 4)
		{
			m_limitIndex = Max<unsigned char>(7, m_limitIndex);
			m_clubList.Start();
			sClub* p = NULL;
			for (int i = 0; i < 11; i++)
				p = m_clubList.Next();
			m_pClub = p;
			m_curIndex = p->index;
		}
		if (GolfBall().m_groundType == 3)
		{
			m_clubList.Start();
			sClub* p = NULL;
			for (unsigned char i = 0; i < 12; i++)
				p = m_clubList.Next();
			m_pClub = p;
			m_curIndex = p->index;
		}
	}
	if (GolfBall().m_groundType == 0)
	{
		if (Doc()->m_golfGame.gameType == 1)
		{
			unsigned char cur = GOLFDOC()->m_currentPlayer;
			sPlayerData* pPlayer = GOLFDOC()->GetPlayer(cur);
			unsigned char team = pPlayer->team;
			sTeamData* pTeam = GOLFDOC()->GetTeam(team);
			if (pTeam->stroke[GOLFDOC()->m_currentHole - 1] <=
					(unsigned char)1 &&
				m_curIndex > 2)
			{
				m_pClub = m_clubList.Find("3W");
				m_curIndex = m_pClub->index;
			}
		}
		else
		{
			unsigned char cur = GOLFDOC()->m_currentPlayer;
			sPlayerData* pPlayer = GOLFDOC()->GetPlayer(cur);
			if (pPlayer->stroke[GOLFDOC()->m_currentHole - 1] <=
					(unsigned char)1 &&
				m_curIndex > 2)
			{
				m_pClub = m_clubList.Find("3W");
				m_curIndex = m_pClub->index;
			}
		}
	}
	if (GOLFDOC()->m_tutorialMode == 10)
	{
		m_pClub = m_clubList.Find("1W");
		m_curIndex = m_pClub->index;
	}
	m_prevIndex = m_curIndex;
}

void CClub::SetAccuracy()
{
	unsigned char cur = GOLFDOC()->m_currentPlayer;
	sPlayerData* pPlayer = PLAYER(cur);
	m_narrowAccuracy = (float)(int)pPlayer->accuracy +
		ground_accuracy[GolfBall().m_groundType];
	float wide = m_narrowAccuracy + 10.0f;
	if (wide > 35.0f)
		wide = 35.0f;
	m_wideAccuracy = wide;
	if (IsLocalContent(S3_SAFETEE_ITEM))
	{
		unsigned char cur2 = GOLFDOC()->m_currentPlayer;
		pPlayer = PLAYER(cur2);
		if (pPlayer->item == 0x18000028)
		{
			unsigned char cur3 = GOLFDOC()->m_currentPlayer;
			pPlayer = PLAYER(cur3);
			m_narrowAccuracy =
				(float)(int)pPlayer->accuracy + ground_accuracy[1];
		}
	}
}

void CClub::SetApproachMode()
{
	WVector pin = GOLFDOC()->GetHoleData().pin;
	CGolfBall& ball = GolfBall();

	float dx = ball.m_pos.x - pin.x;
	float dz = ball.m_pos.z - pin.z;
	m_limitIndex = 7;
	float dist = sqrtf(dx * dx + dz * dz) * .3125f;
	const volatile float& storedDistance = dist;
	if (storedDistance < 10.0f)
	{
		m_bPowerPutter = 0;
		m_approachMode = 0;
	}
	else if (dist < 15.0f)
	{
		m_bPowerPutter = 0;
		m_approachMode = 1;
	}
	else if (dist < 28.f)
	{
		m_approachMode = 2;
		m_bPowerPutter = true;
	}
	else if (dist < 58.f)
	{
		m_approachMode = 3;
		m_bPowerPutter = true;
	}
	else
	{
		m_approachMode = 4;
		m_limitIndex = 0;
		m_bPowerPutter = true;
	}

	if (GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2)
		m_bPowerPutter = true;
}

float CClub::GetNarrowAccuracy()
{
	if (m_pClub->type == 3)
		return 0.0f;
	float f = (float)ceil((double)((GolfClub().m_powerShot != 0 ? 0.6f : 1.0f) *
		m_narrowAccuracy));
	if (f < 2.0f)
		f = 2.0f;
	return f;
}

float CClub::GetWideAccuracy()
{
	if (m_pClub->type == 3)
		return 0.0f;
	float f = (float)ceil((double)((GolfClub().m_powerShot != 0 ? 0.6f : 1.0f) *
		m_narrowAccuracy));
	if (f < 2.0f)
		f = 2.0f;
	f = f + 10.0f;
	if (f > 35.0f)
		f = 35.0f;
	return f;
}

float CClub::GetRange(const char* name)
{
	sClub* club = name ? m_clubList.Find(name) : m_pClub;
	float range = club->range;
	switch (club->type)
	{
	case 0:
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid ==
			0x10000012)
		{
			float lucky = GetAirNightLuckyRange(club->name);
			if (lucky > 0.f)
				range = GetPowerShotFactor() +
					float(PLAYER(GOLFDOC()->m_currentPlayer)->power) + lucky;
			else
			{
				float sum = GetPowerShotFactor() + club->range;
				range = float(PLAYER(GOLFDOC()->m_currentPlayer)->power) + sum;
			}
		}
		else
		{
			float sum = GetPowerShotFactor() + club->range;
			unsigned char cur = GOLFDOC()->m_currentPlayer;
			float power = GOLFDOC()->GetPlayer(cur)->power;
			range = power + power + sum;
		}
		break;
	case 1:
		range = GetPowerShotFactor() + club->range;
		break;
	case 2:
		switch (m_approachMode)
		{
		case 0:
		case 1:
		case 2:
			range = (m_powerShot != 0 ? 30.f : 0.f) + 30.f;
			break;
		case 3:
			range = (m_powerShot != 0 ? 20.f : 0.f) + 60.f;
			break;
		case 4:
			range = GetPowerShotFactor() + club->range;
			break;
		}
		break;
	case 3:
		range = m_bPowerPutter ? (Is("1PT", club) ? 40.f : 30.f) : club->range;
		break;
	}
	if (m_pClub->type != 3)
	{
		range =
			Doc()->GetAuxPartProperty(GOLFDOC()->m_currentPlayer, 0) + range;
		if (IsLocalContent(S4_RANGE_BUFF))
		{
			float buff =
				(float)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer, 16);
			if (GolfClub().m_powerShot != 0)
				range =
					(float)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer, 17) +
					buff + range;
			else
				range = buff + range;
		}
		else
		{
			bool power = false;
			CCardManager::Instance()->SetPlayerIndex(
				GOLFDOC()->m_currentPlayer);
			if (GolfClub().m_powerShot != 0)
				power = true;
			range =
				(float)CCardManager::Instance()->GetTotalRange(power) + range;
		}
	}
	return range;
}

float CClub::GetLie(const char* name)
{
	sClub* pClub = name ? m_clubList.Find(name) : m_pClub;
	if (m_approachMode == 4)
		return pClub->lie;
	float initSpin = GolfBall().m_initSpin;
	return initSpin * 0.06981316953897476f + pClub->lie;
}

float CClub::GetPower()
{
	float result = m_pClub->power;
	float aux = Doc()->GetAuxPartProperty(GOLFDOC()->m_currentPlayer, 0);
	float add = 0.f;
	if (GolfClub().m_powerShot != 0)
		add = GetPowerShotFactor() + m_randomPower;
	if (IsLocalContent(S4_RANGE_BUFF))
	{
		float buff =
			(float)(int)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer, 16);
		if (GolfClub().m_powerShot != 0)
			buff = (float)(int)Doc()->GetBuffValue(GOLFDOC()->m_currentPlayer,
					   17) +
				buff;
		aux = buff + aux;
	}
	else
	{
		bool power = false;
		CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_currentPlayer);
		if (GolfClub().m_powerShot != 0)
			power = true;
		aux = (float)(int)CCardManager::Instance()->GetTotalRange(power) + aux;
	}
	switch (m_pClub->type)
	{
	case 0:
	{
		float range, power;
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid ==
			0x10000012)
		{
			range = GetAirNightLuckyRange(m_pClub->name);
			power = GetAirNightLuckyPower(m_pClub->name);
			if (range > 0.f && power > 0.f)
			{
				int pwr = PLAYER(GOLFDOC()->m_currentPlayer)->power;
				result = ((aux * .5f + pwr + add) / range * 1.5f + 1.f) * power;
			}
			else
			{
				int pwr = PLAYER(GOLFDOC()->m_currentPlayer)->power;
				range = m_pClub->range;
				power = m_pClub->power;
				result = ((aux * .5f + pwr + add) / range * 1.5f + 1.f) * power;
			}
		}
		else
		{
			unsigned char index = GOLFDOC()->m_currentPlayer;
			int pwr = GOLFDOC()->GetPlayer(index)->power;
			range = m_pClub->range;
			power = m_pClub->power;
			result =
				(((aux * .5f + pwr) + (aux * .5f + pwr) + add) / range * 1.5f +
					1.f) *
				power;
		}
		break;
	}
	case 1:
		result = (add / (float)m_pClub->range + 1.0f) * (float)m_pClub->power;
		result += (float)m_pClub->power / (float)m_pClub->range * aux * 1.3f;
		break;
	case 2:
		switch (m_approachMode)
		{
		case 0:
		case 1:
		case 2:
		{
			float f;
			if (m_powerShot != 0)
				f = 28.f;
			else
				f = 0.f;
			result = (GetLie() * .5f / (56.f * g_DEGTORAD) + .5f) * (f + 52.0f);
			break;
		}
		case 3:
		{
			float f;
			if (m_powerShot != 0)
				f = 18.f;
			else
				f = 0.f;
			result = (GetLie() * .5f / (56.f * g_DEGTORAD) + .5f) * (f + 80.0f);
			break;
		}
		case 4:
			result =
				(add / (float)m_pClub->range + 1.0f) * (float)m_pClub->power;
			break;
		}
		result = result + (float)m_pClub->power / (float)m_pClub->range * aux;
		break;
	case 3:
		if (m_bPowerPutter)
			result = strstr(m_pClub->name, "1PT") ? 42.f : 36.f;
		else
			result = m_pClub->power;
		break;
	}
	return result;
}

void CClub::ClearPointFactor()
{
	for (sClub* p = m_clubList.Start(); p; p = m_clubList.Next())
		p->pointFactor = 1.0f;
}

void CClub::SetPointFactor(float factor)
{
	m_pClub->pointFactor = factor;
}

unsigned char CClub::GetType(unsigned char index)
{
	for (sClub* p = m_clubList.Start(); p; p = m_clubList.Next())
	{
		if (p->index == index)
			return p->type;
	}
	return 0;
}

void CClub::SetClub(const char* name)
{
	m_pClub = m_clubList.Find(name);
	m_curIndex = m_pClub->index;
}

void CClub::SetClub(unsigned char index)
{
	m_pClub = SelectClub(index);
	m_curIndex = m_pClub->index;
}

sClubSet* CClub::AddClubSet(unsigned long tid)
{
	sClubSet* pSet = FindClubSet(tid);
	if (pSet != NULL)
		return pSet;

	IFF_STRUCT::sClubSet* pInfo = ItemManager()->FindClubSet(tid);
	if (pInfo)
	{
		pSet = new sClubSet;
		pSet->pClubSet = pInfo;
		for (int i = 0; i < 4; i++)
		{
			IFF_STRUCT::sClub* pIffClub =
				ItemManager()->FindClub(pInfo->ClubIds[i]);
			pSet->puppet[i] = g_resrcmng->GetPuppet(
				MakeStr("%s.pet", pIffClub->Data), false, true, false);
			pSet->puppet[i]->CalcBound(NULL, true);
			pSet->uiPuppet[i] = g_resrcmng->GetPuppet(
				MakeStr("%s.pet", pIffClub->Data), false, true, false);
			pSet->uiPuppet[i]->CalcBound(NULL, true);
			w_bound_box* pCamera = pSet->puppet[i]->FindBoundBox("camera");
			w_bound_box* pTarget = pSet->puppet[i]->FindBoundBox("target");
			WMatrix& f = pSet->uiCamera[i];
			LightSet& g = pSet->uiLight[i];
			g.diffuse = 0xffffff;
			pSet->uiLight[i].ambient = 0x808080;
			g.ambient2 = 0xffffff;
			g.type = 2;
			float angle;
			if (pCamera != NULL && pTarget != NULL)
			{
				angle = 0.f;
				WSliceStr option(pCamera->info->option);
				option.Get("*angle", "%f", &angle);
				f.pivot = pCamera->info->spherePivot;
				f.za = (pTarget->info->spherePivot - pCamera->info->spherePivot)
						   .Normalize();

				WVector up(0.f, cosf(angle * g_DEGTORAD),
					sinf(angle * g_DEGTORAD));
				const volatile WVector& storedUp = up;
				f.xa = WVector(up.y * f.za.z - up.z * f.za.y,
					storedUp.z * f.za.x - up.x * f.za.z,
					up.x * f.za.y - storedUp.y * f.za.x)
						   .Normalize();
				const WVector& right = f.xa;
				f.ya = WCrossProduct(f.za, right);
				g.nearone = f.ya * 10.f + f.pivot - pTarget->info->spherePivot;
			}
			else
			{
				f.pivot.z = 0.0f;
				f.pivot.y = 0.0f;
				f.pivot.x = 0.0f;
				f.za.y = 0.0f;
				f.za.x = 0.0f;
				f.ya.z = 0.0f;
				f.ya.x = 0.0f;
				f.xa.z = 0.0f;
				f.xa.y = 0.0f;
				f.za.z = 1.0f;
				f.ya.y = 1.0f;
				f.xa.x = 1.0f;
				g.nearone = f.ya * 10.f + f.pivot;
			}
			g.nearone.Negate();
			g.nearone.Normalize();
		}
		m_clubSetList.push_back(pSet);
	}

	return pSet;
}

sClubSet* CClub::FindClubSet(unsigned long tid)
{
	ClubModelIterator it;
	it = std::find_if(m_clubSetList.begin(), m_clubSetList.end(),
		sFindClubSet(tid));
	if (it != m_clubSetList.end())
		return *it;
	return NULL;
}

float CClub::GetPowerShotFactor()
{
	float table[4] = { 0.0f, 10.0f, 20.0f, 15.0f };
	return table[m_powerShot];
}

float CClub::GetAirNightLuckyRange(char* name)
{
	float result = 0.0f;
	if (stricmp(name, "1W") == 0)
		result = 216.0f;
	else if (stricmp(name, "2W") == 0)
		result = 206.0f;
	else if (stricmp(name, "3W") == 0)
		result = 196.0f;
	return result;
}

float CClub::GetAirNightLuckyPower(char* name)
{
	float result = 0.0f;
	if (stricmp(name, "1W") == 0)
		result = 216.0f;
	else if (stricmp(name, "2W") == 0)
		result = 194.0f;
	else if (stricmp(name, "3W") == 0)
		result = 176.0f;
	return result;
}

const char* CClub::GetClubString(eClub club)
{
	switch (club)
	{
	case CLUB_WOOD_1:
		return "WOOD_1";
	case CLUB_WOOD_2:
		return "WOOD_2";
	case CLUB_WOOD_3:
		return "WOOD_3";
	case CLUB_IRON_2:
		return "IRON_2";
	case CLUB_IRON_3:
		return "IRON_3";
	case CLUB_IRON_4:
		return "IRON_4";
	case CLUB_IRON_5:
		return "IRON_5";
	case CLUB_IRON_6:
		return "IRON_6";
	case CLUB_IRON_7:
		return "IRON_7";
	case CLUB_IRON_8:
		return "IRON_8";
	case CLUB_IRON_9:
		return "IRON_9";
	case CLUB_WEDGE_P:
		return "WEDGE_P";
	case CLUB_WEDGE_S:
		return "WEDGE_S";
	case CLUB_PUTTER_1:
		return "PUTTER_1";
	case CLUB_PUTTER_2:
		return "PUTTER_2";
	default:
		return "NOT DEFINED";
	}
}
