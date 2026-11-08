#include "stdafx.h"
#include "fallingspot.h"
#include "fontinfo.h"
#include "club.h"
#include "3dfont.h"
#include "simpleoverlay.h"
#include "golfball.h"
#include "golfdoc.h"
#include "quadtree.h"
#include "clientsetting.h"
#include "inputmanager.h"
#include "chatmsg.h"
#include "wmathutil.inl"
#include "capturedbg.h"
#include "projectg.h"
#include "mousecursor.h"
#include "wpuppet.h"
#include "../../shared/sharedtables.h"

const unsigned long CSpot::m_adwColors[5] = { 0xffff0000, 0xffff8a00,
	0xffffde00, 0xff95fb21, 0xff02ff42 };

IMPLEMENT_ACTOR(CSpot, IActor)

CSpot::CSpot()
{
	m_pRangeFont = NULL;
	m_pPlayerFont = NULL;
	m_pClubRangeInfo = NULL;
	m_texIndicator = 0;
	m_texFalling = 0;
	m_texSlope = 0;
	m_texHoleTarget = 0;
	m_pSpotLine = NULL;
	m_pPlayerPos = NULL;
	m_pPlayerUI = NULL;
	m_texScoreTarget = 0;
	m_pRivalPos = NULL;
	m_pNameFont = NULL;
	AddSkipList(0x1400);
	m_dispPriority = DISP_PRIORITY_1;
	m_pFan[0] = &m_vtx[0];
	m_pFan[1] = &m_vtx[1];
	m_pFan[4] = NULL;
	m_pFallIndicator1 = NULL;
	m_pFallIndicator2 = NULL;
	m_pFan[2] = &m_vtx[3];
	m_pFan[3] = &m_vtx[2];
}

void CSpot::OnPreLoadInit()
{
	m_bShowPlayer = true;
	CSceneManager::Instance()->RegisterElement(OBJ_CUSTOM, NULL, this,
		"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1", 0, NULL);
}

void CSpot::OnDestroy()
{
	if (g_resrcmng && m_pSpotLine)
	{
		g_resrcmng->Release(m_pSpotLine);
		m_pSpotLine = NULL;
	}
	if (g_resrcmng && m_pFallIndicator1)
	{
		g_resrcmng->Release(m_pFallIndicator1);
		m_pFallIndicator1 = NULL;
	}
	if (g_resrcmng && m_pFallIndicator2)
	{
		g_resrcmng->Release(m_pFallIndicator2);
		m_pFallIndicator2 = NULL;
	}
	if (g_resrcmng && m_texIndicator)
	{
		g_resrcmng->Release(m_texIndicator);
		m_texIndicator = 0;
	}
	if (g_resrcmng && m_texFalling)
	{
		g_resrcmng->Release(m_texFalling);
		m_texFalling = 0;
	}
	if (g_resrcmng && m_texSlope)
	{
		g_resrcmng->Release(m_texSlope);
		m_texSlope = 0;
	}
	if (g_resrcmng && m_pPlayerPos)
	{
		g_resrcmng->Release(m_pPlayerPos);
		m_pPlayerPos = NULL;
	}
	if (g_resrcmng && m_texScoreTarget)
	{
		g_resrcmng->Release(m_texScoreTarget);
		m_texScoreTarget = 0;
	}
	if (g_resrcmng && m_texHoleTarget)
	{
		g_resrcmng->Release(m_texHoleTarget);
		m_texHoleTarget = 0;
	}
	if (m_pPlayerUI)
	{
		delete m_pPlayerUI;
		m_pPlayerUI = NULL;
	}
	if (m_pPlayerFont)
	{
		delete m_pPlayerFont;
		m_pPlayerFont = NULL;
	}
	if (m_pRangeFont)
	{
		delete m_pRangeFont;
		m_pRangeFont = NULL;
	}
	if (m_pClubRangeInfo)
	{
		delete m_pClubRangeInfo;
		m_pClubRangeInfo = NULL;
	}
	if (m_pRivalPos)
	{
		delete[] m_pRivalPos;
		m_pRivalPos = NULL;
	}
}

void CSpot::OnLoad()
{
	m_pClubRangeInfo = new CClubRangeInfo;
	m_pRangeFont = new C3dFont;
	m_pRangeFont->LoadTexture("[font_wind.jpg", 16, 16, "1234567890ym%-.?/:", 3,
		-30);
	m_pPlayerFont = new C3dFont;
	m_pPlayerFont->LoadTexture("[nplayer.jpg", 32, 16, "0123rb", 0, -30);
	m_pFallIndicator1 = g_resrcmng->GetOverlay("[fall_indicator1.png", 0);
	m_pFallIndicator2 = g_resrcmng->GetOverlay("[fall_indicator2.png", 0);
	m_pSpotLine = g_resrcmng->GetOverlay("spot_line.bmp", 0);
	m_pPlayerPos = g_resrcmng->GetOverlay("{player_pos.bmp", 0);
	switch (Doc()->m_golfGame.gameType)
	{
	case 0:
	case 3:
	case 7:
		m_pPlayerUI = new CSimpleUI("[nplayer.jpg");
		m_pPlayerUI->Add("1", WRect(0, 0, 32.f, 16.f), false);
		m_pPlayerUI->Add("2", WRect(32, 0, 32.f, 16.f), false);
		m_pPlayerUI->Add("3", WRect(0, 16, 32.f, 16.f), false);
		m_pPlayerUI->Add("4", WRect(32, 16, 32.f, 16.f), false);
		break;
	case 1:
		m_pPlayerUI = new CSimpleUI("[nplayer.jpg");
		m_pPlayerUI->Add("r", WRect(0, 32, 32.f, 16.f), false);
		m_pPlayerUI->Add("b", WRect(32, 32, 32.f, 16.f), false);
		break;
	case 6:
		m_pRivalPos = new sRivalPos[Doc()->m_rivalList.size()];
		for (unsigned char i = 0; i < Doc()->m_rivalList.size(); ++i)
		{
			if (Doc()->m_myInfo.info.dwGuildId ==
				Doc()->m_rivalList[i].guildUID)
			{
				m_pRivalPos[i].rate = 0;
				m_pRivalPos[i].progress = -1;
				m_pRivalPos[i].hole = Doc()->m_holeOrder[0];
				m_pRivalPos[i].srcPos =
					GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).tee;
			}
		}
		break;
	case 4:
	case 5:
	case 9:
	case 10:
		m_pRivalPos = new sRivalPos[Doc()->m_rivalList.size()];
		for (unsigned char j = 0; j < Doc()->m_rivalList.size(); ++j)
		{
			m_pRivalPos[j].rate = 0;
			if (Doc()->m_golfGame.gameType == 10)
				m_pRivalPos[j].progress = 0;
			m_pRivalPos[j].hole = Doc()->m_holeOrder[0];
			m_pRivalPos[j].srcPos = Doc()->m_rivalList[j].ballPos;
		}
		break;
	}

	m_texIndicator = g_resrcmng->LoadTexture("{indicator.bmp", 0, 0, NULL);
	m_texFalling = g_resrcmng->LoadTexture("[falling.jpg", 0, 0, NULL);
	m_texSlope = g_resrcmng->LoadTexture("[slope.jpg", 0, 0, NULL);
	m_texHoleTarget =
		g_resrcmng->LoadTexture("[target_hole_posi.jpg", 0, 0, NULL);
	if (GOLFDOC()->m_tutorialMode < 15)
		m_texScoreTarget =
			g_resrcmng->LoadTexture("[score_target.jpg", 0, 0, NULL);
	m_pNameFont = CChatMsg::Instance()->GetMaskedFont();
}

inline CSpot::sRivalPos::sRivalPos()
{
}

void CSpot::OnInit()
{
	m_groundType = 0;
	m_angle = 0;
	m_rangeAngle = 0;
	m_pos.Reset();
	m_targetPos.Reset();
	m_srcPos.Reset();
	m_moveRate = 1;
	m_bEnable = true;
	m_lineWidth = 2;
	m_markerSize = 12;
	m_width = 20;
	m_height = 20;
	m_offsetRate = 0;
	m_pulse = 0;
	m_playerAngle = 0;
	m_powerPhase = 0;
	m_bRotate = true;
	m_powerDir = 1;
	m_prevOffset.Reset();
	float angle = Random(-g_PI, g_PI);
	WVector direction;
	direction.x = cosf(angle);
	direction.y = 0;
	direction.z = sinf(angle);
	float range = GolfClub().GetRange() * 3.2f;
	m_nextOffset = direction * Random(0.f, range * .02f);
}

void CSpot::OnProcess(float dt)
{
	if (CCapturedBg::Instance()->IsCaptured())
		return;

	if (!GolfClub().IsCalcRandomPower())
	{
		if (Doc()->m_golfGame.gameType != 3 || !IsMyTurn(false) ||
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x1800000c ||
			GolfClub().PowerShotType() == 0 ||
			(GolfClub().GetType() != 0 && GolfClub().GetType() != 1))
		{
			m_offsetRate = 0;
			m_pos = m_targetPos;
			GolfClub().SetRandomPower(0);
		}
		else
		{
			if (!GOLFDOC()->m_shotPhase)
				m_offsetRate += dt * 8;
			if (m_offsetRate > 1)
			{
				m_offsetRate = 0;
				m_pos = m_targetPos + m_nextOffset;
				m_prevOffset = m_nextOffset;
				WVector direction, side;
				direction = m_targetPos - GolfBall().m_pos;
				direction.y = 0;
				direction.Normalize();
				side = WCrossProduct(direction, WVector::UNIT_POS_Y);
				side.y = 0;
				side.Normalize();
				if (rand() % 10 || GolfClub().GetType() != 0)
					m_nextOffset = direction * Random(-1.f, 1.f) +
						side * Random(-1.f, 1.f);
				else
					m_nextOffset = direction * Random(1.f, 1.5f) +
						side * Random(-1.f, 1.f);
				float range = GolfClub().GetRange() * 3.2f;
				m_nextOffset *= range * .02f;
			}
			else
			{
				m_pos = GetPVS().GetGroundPoint(m_targetPos +
						(1 - m_offsetRate) * m_prevOffset +
						m_offsetRate * m_nextOffset,
					true, &m_groundType, true);
			}
			WVector distance = m_pos - GolfBall().m_pos;
			distance.y = 0;
			float power = distance.Magnitude() * .3125f;
			power -= GolfClub().GetRange();
			GolfClub().SetRandomPower(power);
		}
	}
	else
	{
		m_offsetRate = 0;
	}

	if (GOLFDOC()->m_target.bShow)
	{
		m_playerAngle += dt * g_PI;
		if (m_playerAngle > g_PI * 2)
			m_playerAngle -= g_PI * 2;
	}

	if (GolfClub().Updated())
		m_bRotate = true;
	if (!g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) &&
		g_input->GetDown("HIDE", true))
		m_bShowPlayer = !m_bShowPlayer;
	switch (Doc()->m_golfGame.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		for (unsigned char i = 0; i < Doc()->m_rivalList.size(); ++i)
		{
			const unsigned char rivalIndex = i;
			WVector distance;
			if (Doc()->m_rivalList[rivalIndex].state)
				continue;
			if (m_pRivalPos[i].hole != GOLFDOC()->m_currentHole)
				continue;
			if (m_pRivalPos[i].progress < 1)
				m_pRivalPos[i].progress += dt * m_pRivalPos[i].rate;
			if (Doc()->m_golfGame.gameType == 10)
			{
				if (i == MyIndex())
				{
					if (GolfBall().m_playFrame >= 0 &&
						GolfBall().m_playFrame < GolfBall().m_renderNum)
					{
						distance = GetHoleCupPos() - GolfBall().m_pos;
						distance.y = 0;
						float length10 = WVectorLen(distance) * 10;
						Doc()->m_rivalList[rivalIndex].approachDistance =
							(int)floorf(length10 * .3125f);
					}
				}
				else if (m_pRivalPos[i].progress > 1)
				{
					m_pRivalPos[i].progress = 1;
					Doc()->m_rivalList[rivalIndex].approachDistance =
						Doc()->m_rivalList[rivalIndex].approachResultDistance;
					SendMsg(this, "Score", 42, 0, 0, 0, 0);
				}
				else if (m_pRivalPos[i].progress < 1)
				{
					float rate = GetMovingRate(m_pRivalPos[i].progress);

					distance = GetHoleCupPos() -
						((1 - rate) * m_pRivalPos[i].srcPos +
							rate * Doc()->m_rivalList[rivalIndex].ballPos);
					distance.y = 0;
					float length10 = WVectorLen(distance) * 10;
					Doc()->m_rivalList[rivalIndex].approachDistance =
						(int)floorf(length10 * .3125f);
				}
			}
			else if (m_pRivalPos[i].progress > 1)
			{
				m_pRivalPos[i].progress = 1;

				distance = Doc()->m_rivalList[rivalIndex].ballPos -
					GOLFDOC()->GetHoleData(m_pRivalPos[i].hole).pin;
				distance.y = 0;
				if (distance.Magnitude() < GolfBall().GetRadius() * 2)
				{
					m_pRivalPos[i].progress = -1;
					m_pRivalPos[i].hole =
						Doc()->m_holeOrder[GetHoleIndex(m_pRivalPos[i].hole)];
				}
			}
		}
		break;
	}

	if (!m_bEnable)
		return;

	m_rangeAngle += dt * g_PI;
	if (m_rangeAngle > g_PI * 2)
		m_rangeAngle -= g_PI * 2;

	m_pulse += dt * .5f;
	if (m_pulse > 1)
		m_pulse = 0;

	if (m_bRotate && WisEqual(m_moveRate, 1.f, g_EPSILON))
	{
		if (GolfClub().Updated())
			m_pulse = 0;
		m_bRotate = false;
		Rotate();
	}

	if (GolfClub().GetType() == 3 || !WisEqual(m_moveRate, 1.f, g_EPSILON))
	{
		m_powerBlend = 1;
	}
	else
	{
		m_powerPhase += dt * m_powerDir * 2;
		if (m_powerPhase > 1)
		{
			m_powerPhase = 1;
			m_powerDir = -1;
		}
		else if (m_powerPhase < 0)
		{
			m_powerPhase = 0;
			m_powerDir = 1;
		}

		m_powerBlend =
			GetPVS().GetMaxPowerOnGround() * (m_powerPhase * m_powerPhase) +
			GetPVS().GetMinPowerOnGround() * (1 - m_powerPhase * m_powerPhase);
	}
}

void CSpot::Display()
{
	unsigned char j;
	unsigned char i;
	WVector screen;
	if (Skipped() || CCapturedBg::Instance()->IsCaptured())
		return;
	if (GOLFDOC()->m_target.bShow)
	{
		bool plane = GOLFDOC()->m_tutorialMode == 2 ||
			GOLFDOC()->m_tutorialMode == 6 || GOLFDOC()->m_tutorialMode == 11 ||
			GOLFDOC()->m_tutorialMode == 10 ||
			GOLFDOC()->m_tutorialMode == 13 || GOLFDOC()->m_tutorialMode == 14;
		if (plane)
		{
			float scale = GOLFDOC()->m_tutorialMode == 2 ? 2.f : 1.f;
			for (int i = 0; i < 4; ++i)
			{
				WVector pos = GOLFDOC()->m_target.pos;
				pos.y += 1;
				pos.x += (i & 1 ? 1.f : -1.f) * scale * 44.8f;
				pos.z += (i & 2 ? 1.f : -1.f) * scale * 44.8f;
				m_vtx[i].SetPosition(pos);
				m_vtx[i].tu = i & 1 ? 1.f : 0.f;
				m_vtx[i].tv = i & 2 ? 0.f : 1.f;
				m_vtx[i].diffuse = 0xffffffff;
			}
			g_view->DrawPolygonFan(m_pFan, m_texScoreTarget & 0x7ff, 0x400,
				false);
		}
		else
		{
			WMatrix matrix;
			matrix = RotMat(m_playerAngle, 1);
			matrix *= .5f;
			matrix.pivot = GOLFDOC()->m_target.pos;
			GOLFDOC()->m_target.pet->ApplyBones(NULL, matrix);
			GOLFDOC()->m_target.pet->Transform(NULL, 1.f, 0);
			GOLFDOC()->m_target.pet->UpdateLightSource(&g_lightset, false,
				NULL);
			GOLFDOC()->m_target.pet->Render(g_view, true, 1.f, false, true,
				false);
		}
	}

	if (m_bShowPlayer)
	{
		if (GOLFDOC()->m_gameMode & 512)
		{
			switch (Doc()->m_golfGame.gameType)
			{
			case 0:
			case 3:
			case 7:
				for (i = 0; i < GOLFDOC()->m_playerNum; ++i)
				{
					if (PLAYER(i)->state || i == GOLFDOC()->m_currentPlayer)
						continue;
					if (!PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1])
						continue;

					screen = g_view->Projection(PLAYER(i)->pos);
					if (0.f < screen.z && screen.z < 1.f)
					{
						m_pPlayerPos->Render(g_view, WRect(0, 0, 1.f, 1.f),
							WRect(screen.x - 4, screen.y - 4, 8.f, 8.f), 0,
							PLAYER(i)->color | 0x80000000, 0, 0);
						m_pPlayerUI->SetColor(PLAYER(i)->color | 0x80000000);
						m_pPlayerUI->Render(MakeStr("%d", i + 1), screen.x,
							screen.y - 12, 1.f, 34);
					}
				}
				break;
			case 1:
				for (i = 0; i < 2; ++i)
				{
					if (TEAM(i)->state ||
						i == PLAYER(GOLFDOC()->m_currentPlayer)->team)
						continue;
					if (!TEAM(i)->stroke[GOLFDOC()->m_currentHole - 1])
						continue;

					screen = g_view->Projection(TEAM(i)->pos);
					if (0.f < screen.z && screen.z < 1.f)
					{
						m_pPlayerPos->Render(g_view, WRect(0, 0, 1.f, 1.f),
							WRect(screen.x - 4, screen.y - 4, 8.f, 8.f), 0,
							TEAM(i)->color | 0x80000000, 0, 0);
						m_pPlayerUI->SetColor(TEAM(i)->color | 0x80000000);
						m_pPlayerUI->Render(i == 0 ? "r" : "b", screen.x,
							screen.y - 12, 1.f, 34);
					}
				}
				break;
			case 6:
				if (!CProjectG::Instance()->HidePI())
				{
					for (i = 0; i < Doc()->m_rivalList.size(); ++i)
					{
						if (Doc()->m_rivalList[i].state)
							continue;
						if (Doc()->m_rivalList[i].oid == MyGuid(true))
							continue;
						if (m_pRivalPos[i].hole != GOLFDOC()->m_currentHole ||
							m_pRivalPos[i].progress < 0)
							continue;
						if (Doc()->m_myInfo.info.dwGuildId !=
							Doc()->m_rivalList[i].guildUID)
							continue;
						float rate = GetMovingRate(m_pRivalPos[i].progress);
						screen = g_view->Projection(GetPVS().GetGroundPoint(
							(1 - rate) * m_pRivalPos[i].srcPos +
								rate * Doc()->m_rivalList[i].ballPos,
							false, NULL, true));
						if (0.f < screen.z && screen.z < 1.f)
						{
							unsigned long color = Doc()->m_rivalList[i].team
								? 0x800060ff
								: 0x80ff0000;

							m_pPlayerPos->Render(g_view, WRect(0, 0, 1.f, 1.f),
								WRect(screen.x - 4, screen.y - 4, 8.f, 8.f), 0,
								color, 0, 0);
							m_pNameFont->Print(g_view,
								screen.x -
									m_pNameFont->GetTextWidth(g_view,
										Doc()->m_rivalList[i].nickname) *
										.5f,
								screen.y - 15, Doc()->m_rivalList[i].nickname,
								0, 0xffffffff, NULL);
							m_pNameFont->Flush(g_view);
						}
					}
				}
				break;
			case 4:
			case 5:
			case 9:
			case 10:
				if (!CProjectG::Instance()->HidePI())
				{
					for (i = 0; i < Doc()->m_rivalList.size(); ++i)
					{
						if (Doc()->m_rivalList[i].state)
							continue;
						if (Doc()->m_rivalList[i].oid == MyGuid(true))
							continue;
						if (m_pRivalPos[i].hole != GOLFDOC()->m_currentHole ||
							m_pRivalPos[i].progress < 0)
							continue;

						float rate = GetMovingRate(m_pRivalPos[i].progress);
						screen = g_view->Projection(GetPVS().GetGroundPoint(
							(1 - rate) * m_pRivalPos[i].srcPos +
								rate * Doc()->m_rivalList[i].ballPos,
							false, NULL, true));
						if (0.f < screen.z && screen.z < 1.f)
						{
							unsigned long color;
							if (Doc()->m_golfGame.gameType == 5)
								color = Doc()->m_rivalList[i].team ? 0x800060ff
																   : 0x80ff0000;
							else if (Doc()->m_golfGame.gameType == 10)
							{
								color =
									Doc()->m_rivalList[i].approachDistance == -1
									? 0x80505050
									: 0x80ffffff;
							}
							else
								color = 0x80ffffff;

							m_pPlayerPos->Render(g_view, WRect(0, 0, 1.f, 1.f),
								WRect(screen.x - 4, screen.y - 4, 8.f, 8.f), 0,
								color, 0, 0);
							if (Doc()->m_golfGame.gameType == 10)
							{
								if (!Doc()->IsCurrentMissionBlind() ||
									(Doc()->IsCurrentMissionBlind() &&
										Doc()->m_rivalList[i].oid ==
											MyGuid(false)))
								{
									m_pNameFont->Print(g_view,
										screen.x -
											m_pNameFont->GetTextWidth(g_view,
												Doc()
													->m_rivalList[i]
													.nickname) *
												.5f,
										screen.y - 15,
										Doc()->m_rivalList[i].nickname, 0,
										0xffffffff, NULL);
									m_pNameFont->Flush(g_view);
								}
							}
							else
							{
								m_pNameFont->Print(g_view,
									screen.x -
										m_pNameFont->GetTextWidth(g_view,
											Doc()->m_rivalList[i].nickname) *
											.5f,
									screen.y - 15,
									Doc()->m_rivalList[i].nickname, 0,
									0xffffffff, NULL);
								m_pNameFont->Flush(g_view);
							}
						}
					}
				}
				break;
			}
		}
		else if (GOLFDOC()->m_gameMode & 0x8dca)
		{
			WVector pos;
			switch (Doc()->m_golfGame.gameType)
			{
			case 0:
			case 3:
			case 7:
				for (i = 0; i < GOLFDOC()->m_playerNum; ++i)
				{
					if (PLAYER(i)->state || i == GOLFDOC()->m_currentPlayer)
						continue;
					if (!PLAYER(i)->stroke[GOLFDOC()->m_currentHole - 1])
						continue;
					for (j = 0; j < 4; ++j)
					{
						pos = PLAYER(i)->pos;
						pos.x += (j & 1 ? 1.f : -1.f) * .5f;
						pos.z += (j & 2 ? 1.f : -1.f) * .5f;
						m_vtx[j].SetPosition(pos);
						m_vtx[j].diffuse = PLAYER(i)->color | 0x80000000;
						m_vtx[j].tu = j & 1 ? 1.f : 0.f;
						m_vtx[j].tv = j & 2 ? 0.f : 1.f;
					}
					g_view->DrawPolygonFan(m_pFan, m_texSlope & 0x7ff, 0x400,
						false);
					DrawIndicator(PLAYER(i)->pos,
						PLAYER(i)->color | 0x80000000);
					if (!CProjectG::Instance()->HidePI())
					{
						m_pPlayerFont->SetPos(PLAYER(i)->pos);
						m_pPlayerFont->SetColor(PLAYER(i)->color | 0x80000000);
						m_pPlayerFont->Printf(g_view, 0, "%d", i);
					}
				}
				break;
			case 1:
				for (i = 0; i < 2; ++i)
				{
					if (TEAM(i)->state ||
						i == PLAYER(GOLFDOC()->m_currentPlayer)->team)
						continue;
					if (!TEAM(i)->stroke[GOLFDOC()->m_currentHole - 1])
						continue;
					for (j = 0; j < 4; ++j)
					{
						pos = TEAM(i)->pos;
						pos.x += (j & 1 ? 1.f : -1.f) * .5f;
						pos.z += (j & 2 ? 1.f : -1.f) * .5f;
						m_vtx[j].SetPosition(pos);
						m_vtx[j].diffuse = TEAM(i)->color | 0x80000000;
						m_vtx[j].tu = j & 1 ? 1.f : 0.f;
						m_vtx[j].tv = j & 2 ? 0.f : 1.f;
					}
					g_view->DrawPolygonFan(m_pFan, m_texSlope & 0x7ff, 0x400,
						false);
					DrawIndicator(TEAM(i)->pos, TEAM(i)->color | 0x80000000);
					if (!CProjectG::Instance()->HidePI())
					{
						m_pPlayerFont->SetPos(TEAM(i)->pos);
						m_pPlayerFont->SetColor(TEAM(i)->color | 0x80000000);
						m_pPlayerFont->Printf(g_view, 0, i == 0 ? "r" : "b");
					}
				}
				break;
			case 6:
				if (!CProjectG::Instance()->HidePI())
				{
					for (i = 0; i < Doc()->m_rivalList.size(); ++i)
					{
						if (Doc()->m_rivalList[i].state)
							continue;
						if (Doc()->m_rivalList[i].oid == MyGuid(true))
							continue;
						if (m_pRivalPos[i].hole != GOLFDOC()->m_currentHole ||
							m_pRivalPos[i].progress < 0)
							continue;
						if (Doc()->m_myInfo.info.dwGuildId !=
							Doc()->m_rivalList[i].guildUID)
							continue;
						float rate = GetMovingRate(m_pRivalPos[i].progress);
						WVector ground = GetPVS().GetGroundPoint((1 - rate) *
									m_pRivalPos[i].srcPos +
								rate * Doc()->m_rivalList[i].ballPos,
							false, NULL, true);
						if (g_view->InFrustum(ground))
						{
							unsigned long color = Doc()->m_rivalList[i].team
								? 0x800060ff
								: 0x80ff0000;

							for (j = 0; j < 4; ++j)
							{
								pos = ground;
								pos.x += (j & 1 ? 1.f : -1.f) * .5f;
								pos.y += GolfBall().GetRadius();
								pos.z += (j & 2 ? 1.f : -1.f) * .5f;
								m_vtx[j].SetPosition(pos);
								m_vtx[j].diffuse = color;
								m_vtx[j].tu = j & 1 ? 1.f : 0.f;
								m_vtx[j].tv = j & 2 ? 0.f : 1.f;
							}
							g_view->DrawPolygonFan(m_pFan, m_texSlope & 0x7ff,
								0x400, false);
							pos = ground;
							pos.y += GolfBall().GetRadius();
							DrawIndicator(pos, color);

							screen = g_view->Projection(pos);
							m_pNameFont->Print(g_view,
								screen.x -
									m_pNameFont->GetTextWidth(g_view,
										Doc()->m_rivalList[i].nickname) *
										.5f,
								screen.y - 32, Doc()->m_rivalList[i].nickname,
								0, 0xffffffff, NULL);
							m_pNameFont->Flush(g_view);
						}
					}
				}
				break;
			case 4:
			case 5:
			case 9:
			case 10:
				if (!CProjectG::Instance()->HidePI())
				{
					for (i = 0; i < Doc()->m_rivalList.size(); ++i)
					{
						if (Doc()->m_rivalList[i].state)
							continue;
						if (m_pRivalPos[i].hole != GOLFDOC()->m_currentHole ||
							m_pRivalPos[i].progress < 0)
							continue;
						if (Doc()->m_rivalList[i].oid == MyGuid(true) &&
							(Doc()->m_golfGame.gameType != 10 ||
								!(GOLFDOC()->m_gameMode & 0x800)))
							continue;

						float rate = GetMovingRate(m_pRivalPos[i].progress);
						WVector ground = GetPVS().GetGroundPoint((1 - rate) *
									m_pRivalPos[i].srcPos +
								rate * Doc()->m_rivalList[i].ballPos,
							false, NULL, true);
						if (g_view->InFrustum(ground))
						{
							unsigned long color;
							if (Doc()->m_golfGame.gameType == 5)
								color = Doc()->m_rivalList[i].team ? 0x800060ff
																   : 0x80ff0000;
							else if (Doc()->m_golfGame.gameType == 10)
							{
								if (Doc()->m_rivalList[i].oid == MyGuid(true))
									color = 0x80ff0000;
								else
									color =
										Doc()->m_rivalList[i]
												.approachDistance == -1
										? 0x80505050
										: 0x80ffffff;
							}
							else
								color = 0x80ffffff;

							for (j = 0; j < 4; ++j)
							{
								pos = ground;
								pos.x += (j & 1 ? 1.f : -1.f) * .5f;
								pos.y += GolfBall().GetRadius();
								pos.z += (j & 2 ? 1.f : -1.f) * .5f;
								m_vtx[j].SetPosition(pos);
								m_vtx[j].diffuse = color;
								m_vtx[j].tu = j & 1 ? 1.f : 0.f;
								m_vtx[j].tv = j & 2 ? 0.f : 1.f;
							}
							g_view->DrawPolygonFan(m_pFan, m_texSlope & 0x7ff,
								0x400, false);
							pos = ground;
							pos.y += GolfBall().GetRadius();
							DrawIndicator(pos, color);

							screen = g_view->Projection(pos);
							if (Doc()->m_golfGame.gameType == 10)
							{
								if (!Doc()->IsCurrentMissionBlind() ||
									(Doc()->IsCurrentMissionBlind() &&
										Doc()->m_rivalList[i].oid ==
											MyGuid(false)))
								{
									m_pNameFont->Print(g_view,
										screen.x -
											m_pNameFont->GetTextWidth(g_view,
												Doc()
													->m_rivalList[i]
													.nickname) *
												.5f,
										screen.y - 32,
										Doc()->m_rivalList[i].nickname, 0,
										0xffffffff, NULL);
									m_pNameFont->Flush(g_view);
								}
							}
							else
							{
								m_pNameFont->Print(g_view,
									screen.x -
										m_pNameFont->GetTextWidth(g_view,
											Doc()->m_rivalList[i].nickname) *
											.5f,
									screen.y - 32,
									Doc()->m_rivalList[i].nickname, 0,
									0xffffffff, NULL);
								m_pNameFont->Flush(g_view);
							}
						}
					}
				}
				break;
			}
		}
	}

	if (m_bEnable && GOLFDOC()->m_tutorialMode >= 1)
	{
		WVector diff = GolfBall().m_pos - m_pos;
		if (m_groundType == -1)
			m_color = 0xffff0000;
		else
		{
			float height = diff.y * .3125f;
			height *= .9144f;
			height = Between(-4.f, height, 4.f);
			float blend = (height + 4) * .5f;

			float fraction = blend - (int)blend;

			if (Wabs(fraction) < g_EPSILON)
				m_color = m_adwColors[(int)blend];
			else
				m_color = ULblend(m_adwColors[(int)blend],
					m_adwColors[(int)blend + 1],
					(unsigned char)(fraction * 255));
		}
		if (GOLFDOC()->m_gameMode & 512)
		{
			if (!IsMyTurn(false) || WisEqual(m_moveRate, 1.f, g_EPSILON))
				g_view->Projection2(&m_vtx[0], m_pos);
			else
			{
				WVector pos;
				pos = m_srcPos * (1 - m_moveRate) + m_pos * m_moveRate;
				g_view->Projection2(&m_vtx[0], pos);
				m_vtx[0].sx = g_view->GetWidth() * .5f;
				WVector screen = g_view->Projection(m_msgPos);
				CMouseCursor::Instance()->ForceMove(screen.x, screen.y, -1.f);
			}
			g_view->Projection2(&m_vtx[1], GolfBall().m_pos);
			if (IsMyTurn(false) && m_vtx[0].sy > m_vtx[1].sy)
				m_vtx[0].sy *= -1.f;
			if (_ValidTopViewLineDraw() == true)
			{
				WVector2D line =
					m_powerBlend * WVector2D(m_vtx[0].sx, m_vtx[0].sy) +
					(1 - m_powerBlend) * WVector2D(m_vtx[1].sx, m_vtx[1].sy);
				float angle = atan2f(m_vtx[0].sy - m_vtx[1].sy,
								  m_vtx[0].sx - m_vtx[1].sx) +
					g_PI * .5f;
				WVector delta = WVector(m_vtx[0].sx, m_vtx[0].sy, 0.f) -
					WVector(m_vtx[1].sx, m_vtx[1].sy, 0.f);
				float length = (0,
					sqrtf(delta.x * delta.x + delta.y * delta.y +
						delta.z * delta.z));
				m_pSpotLine->RenderWithAxis(g_view, WRect(0, 0, 1.f, 1.f),
					WRect(line.x - m_lineWidth * .5f, line.y, m_lineWidth,
						length * m_powerBlend),
					line.x, line.y, angle, 0, m_color, 0);
				m_pSpotLine->RenderWithAxis(g_view, WRect(0, 0, 1.f, 1.f),
					WRect(m_vtx[0].sx - m_lineWidth * .5f, m_vtx[0].sy,
						m_lineWidth, (1 - m_powerBlend) * length),
					m_vtx[0].sx, m_vtx[0].sy, angle, 0, 0xa0000000, 0);
			}
			if (m_groundType == -1)
				m_pFallIndicator2->Render(g_view, WRect(0, 0, 1.f, 1.f),
					WRect(m_vtx[0].sx - m_markerSize * .5f,
						m_vtx[0].sy - m_markerSize * .5f, m_markerSize,
						m_markerSize),
					0, m_color, 0, 0);
			else
			{
				m_pFallIndicator1->Render(g_view, WRect(0, 0, 1.f, 1.f),
					WRect(m_vtx[0].sx - m_markerSize * .5f,
						m_vtx[0].sy - m_markerSize * .5f, m_markerSize,
						m_markerSize),
					0, m_color, 0, 0);
				DrawHoleIndicator();
				DrawRange();
			}
			if (WisEqual(m_moveRate, 1.f, g_EPSILON))
			{
				m_pClubRangeInfo->SetColor(m_color);
				m_pClubRangeInfo->SetPos(m_pos);
				m_pClubRangeInfo->Display();
			}
		}
		else if (m_groundType != -1)
		{
			DrawIndicator(m_pos, m_color);
			m_pRangeFont->SetColor(m_color);
			m_pRangeFont->SetPos(m_pos);
			m_pRangeFont->Printf(g_view, -5, "%.0fy",
				sqrtf(diff.x * diff.x + diff.z * diff.z) * .3125f);
			DrawHoleIndicator();
			DrawRange();
		}
	}
}

bool CSpot::_ValidTopViewLineDraw()
{
	if (!g_view->InFrustum(GolfBall().m_pos) && !g_view->InFrustum(m_pos))
	{
		WVector camera(g_camera.pivot.x, 0, g_camera.pivot.z);
		WVector ball(GolfBall().m_pos.x, 0, GolfBall().m_pos.z);
		WVector spot(m_pos.x, 0, m_pos.z);
		float cameraDistance = (camera - ball).Magnitude();
		float spotDistance = (spot - ball).Magnitude();
		if (cameraDistance > spotDistance)
			return false;
	}

	return true;
}

void CSpot::DrawRange()
{
	if (GolfClub().GetType() == 3 || Doc()->m_golfGame.gameType == 3)
		return;
	float range = GolfClub().GetRange() * 3.2f;
	range *= .05f;

	for (int i = 0; i < 4; ++i)
	{
		WVector pos;
		pos.x = (i & 2 ? 1.f : -1.f) * m_pulse * range;
		pos.y = range * .1f;
		pos.z = (i & 1 ? 1.f : -1.f) * m_pulse * range;
		pos = pos * RotMat(m_rangeAngle * .7f, 1);
		pos += m_pos;
		m_vtx[i].tu = i & 2 ? 1.f : 0.f;
		m_vtx[i].tv = i & 1 ? 0.f : 1.f;
		m_vtx[i].diffuse = ((int)((1 - m_pulse) * 255) << 24) | 0xffffff;
		m_vtx[i].SetPosition(pos);
	}

	g_view->DrawPolygonFan(m_pFan, (m_texFalling & 0x7ff) | 0xc00000, 0x400, 0);
}

void CSpot::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 250:
		*(float*)msg.param1 = CalcDeltaAngle(m_pos - GolfBall().m_pos,
			m_targetPos - GolfBall().m_pos);
		break;
	case 244:
		*(WVector*)msg.param1 = m_pos;
		break;
	case 245:
		m_bRotate = true;
		break;
	case 246:
	{
		float angle = *(float*)msg.param1;
		m_msgPos = *(WVector*)msg.param2;
		m_srcPos = m_pos;
		m_targetPos = GolfBall().m_pos +
			(GolfClub().GetRange() * 3.2f) * RotMat(angle, 1).za;
		m_moveRate = 0;
		m_pulse = .4f;
		break;
	}
	case 247:
		m_moveRate = *(float*)msg.param1;
		break;
	case 248:
		if (Doc()->m_golfGame.gameType == 10)
			m_pRivalPos[msg.param1].hole = (unsigned char)msg.param2;
		if (m_pRivalPos[msg.param1].hole == msg.param2)
		{
			m_pRivalPos[msg.param1].rate = 50.f / msg.param3;
			m_pRivalPos[msg.param1].progress = 0;
			m_pRivalPos[msg.param1].srcPos =
				Doc()->m_rivalList[msg.param1].ballPos;
			if (Doc()->m_golfGame.gameType == 10 &&
				Doc()->m_rivalList[msg.param1].approachTime == -1)
				m_pRivalPos[msg.param1].progress = 1;
		}
		break;
	case 249:
	{
		unsigned char hole = (unsigned char)msg.param2;
		if (IsValidHole(hole))
		{
			m_pRivalPos[msg.param1].hole = hole;
			m_pRivalPos[msg.param1].progress = -1;
			m_pRivalPos[msg.param1].srcPos =
				GOLFDOC()->GetHoleData(m_pRivalPos[msg.param1].hole).tee;
			Doc()->m_rivalList[msg.param1].ballPos =
				m_pRivalPos[msg.param1].srcPos;
		}
		break;
	}
	case 251:
		UpdateRivals();
		break;
	case 355:
		m_bEnable = true;
		break;
	case 356:
		m_bEnable = false;
		break;
	case 400:
		m_bEnable = msg.param1 == 1;
		break;
	case 417:
		m_angle += *(float*)msg.param1;
		m_bRotate = true;
		break;
	case 10008:
		m_angle = *(float*)msg.param1;
		m_bRotate = true;
		break;
	}
}

void CSpot::Rotate()
{
	m_groundType = 0;
	WVector pos;
	pos = GolfBall().m_pos +
		(GolfClub().GetRange() * 3.2f) * RotMat(m_angle, 1).za;
	m_targetPos = GetPVS().GetGroundPoint(pos, true, &m_groundType, true);
	float offset = m_offsetRate;
	if (Wabs(offset) < g_EPSILON)
		m_pos = m_targetPos;
}

void CSpot::DrawHoleIndicator()
{
	WVector dir = GetHoleCupPos() - m_pos;
	dir.y = 0;
	WVector side(dir.z, 0, dir.x * -1.f);
	float alpha = Between(0.f, (dir.Magnitude() - 32) / 64, 1.f) * 255 * .9f;
	float fade =
		Between(0.f, 1 - (g_camera.pivot - m_pos).Magnitude() / 640, 1.f) *
		255 * .9f;
	dir.Normalize();
	side.Normalize();

	for (int i = 0; i < 4; ++i)
	{
		WVector pos = m_pos;
		pos.y += 2;
		pos += (dir * (i & 1 ? 1.f : -1.f) + side * (i & 2 ? 1.f : -1.f)) * 10;
		m_vtx[i].SetPosition(pos);
		m_vtx[i].tu = i & 2 ? 1.f : 0.f;
		m_vtx[i].tv = i & 1 ? 0.f : 1.f;
		m_vtx[i].diffuse =
			(m_color & 0xffffff) | ((unsigned char)Min(alpha, fade) << 24);
	}

	g_view->DrawPolygonFan(m_pFan, m_texHoleTarget & 0x7ff, 0x400, 0);
}

void CSpot::DrawIndicator(const WVector& pos, int color)
{
	WTVertex p;
	g_view->Projection2(&p, pos);
	if (!(p.sz <= 0 || p.sz >= 1))
	{
		for (int i = 0; i < 4; ++i)
		{
			m_vtx[i].sz = p.sz;
			m_vtx[i].rhw = p.rhw;
			m_vtx[i].diffuse = color;
		}
		m_vtx[0].sx = p.sx - m_width * .5f;

		m_vtx[0].tu = 0.f;
		m_vtx[0].tv = 1.f;
		m_vtx[0].sy = p.sy;
		m_vtx[1].sx = p.sx - m_width * .5f;
		m_vtx[1].sy = p.sy - m_height;
		m_vtx[1].tu = 0.f;
		m_vtx[1].tv = 0.f;
		m_vtx[2].sx = p.sx + m_width * .5f;
		m_vtx[2].sy = p.sy;
		m_vtx[2].tu = 1.f;
		m_vtx[2].tv = 1.f;
		m_vtx[3].sx = p.sx + m_width * .5f;
		m_vtx[3].sy = p.sy - m_height;
		m_vtx[3].tu = 1.f;
		m_vtx[3].tv = 0.f;
		if (!(m_vtx[0].sx > g_view->GetWidth() - .5f || m_vtx[2].sx < .5f ||
				m_vtx[0].sy < .5f || m_vtx[1].sy > g_view->GetHeight() - .5f))
		{
			for (int i = 0; i < 4; ++i)
				ClipIndicator(i);
			g_view->DrawProjPolygonFan(m_pFan,
				(m_texIndicator & 0x7ff) | 0x2400000, 0);
		}
	}
}

void CSpot::ClipIndicator(int index)
{
	if (m_vtx[index].sx < .5f)
	{
		m_vtx[index].tu += (.5f - m_vtx[index].sx) / m_width;
		m_vtx[index].sx = .5f;
	}
	else if (m_vtx[index].sx > g_view->GetWidth() - .5f)
	{
		m_vtx[index].tu -=
			(m_vtx[index].sx - g_view->GetWidth() + .5f) / m_width;
		m_vtx[index].sx = g_view->GetWidth() - .5f;
	}
	if (m_vtx[index].sy < .5f)
	{
		m_vtx[index].tv += (.5f - m_vtx[index].sy) / m_height;
		m_vtx[index].sy = .5f;
	}
	else if (m_vtx[index].sy > g_view->GetHeight() - .5f)
	{
		m_vtx[index].tv -=
			(m_vtx[index].sy - g_view->GetHeight() + .5f) / m_height;
		m_vtx[index].sy = g_view->GetHeight() - .5f;
	}
}

float CSpot::GetMovingRate(float t)
{
	return (3 - t * 2) * t * t;
}

bool CSpot::UpdateRivals()
{
	if (m_pRivalPos)
	{
		delete[] m_pRivalPos;
		m_pRivalPos = NULL;
	}

	int count = Doc()->m_rivalList.size();
	if (count > 0)
	{
		m_pRivalPos = new sRivalPos[count];
		for (unsigned char i = 0; i < count; ++i)
		{
			m_pRivalPos[i].rate = 0;
			if (Doc()->m_golfGame.gameType == 10)
				m_pRivalPos[i].progress = 0;
			m_pRivalPos[i].hole = Doc()->m_rivalList[i].hole;
			m_pRivalPos[i].srcPos = Doc()->m_rivalList[i].ballPos;
		}
		return true;
	}

	return false;
}
