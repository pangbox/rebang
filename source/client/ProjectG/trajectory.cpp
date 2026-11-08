#include "stdafx.h"
#include "trajectory.h"
#include "golfball.h"
#include "golfdoc.h"
#include "quadtree.h"
#include "wpuppet.h"
#include "wmathutil.inl"

IMPLEMENT_ACTOR(CTrajectory, IActor)

CTrajectory::CTrajectory()
{
	m_pComet = NULL;
	m_pDoughnut = NULL;
	m_pPuttLine = NULL;
	m_pBallTrack = NULL;
	m_pSkidMark = NULL;
	m_pBallWindTrack = NULL;
	for (int i = 0; i < 2; ++i)
		m_pTrace[i] = NULL;
	m_pLastPuttLine = NULL;
	for (int i = 0; i < 32; ++i)
		m_pPuttLines[i] = NULL;
	m_pSkidMarkCobra = NULL;
	m_pExplodeMark = NULL;
	AddSkipList(0x8c342);
	m_dispPriority = DISP_PRIORITY_1;
}

void CTrajectory::OnDestroy()
{
	if (m_pComet)
	{
		delete m_pComet;
		m_pComet = NULL;
	}
	if (m_pDoughnut)
	{
		delete m_pDoughnut;
		m_pDoughnut = NULL;
	}
	if (m_pBallTrack)
	{
		delete m_pBallTrack;
		m_pBallTrack = NULL;
	}
	if (m_pSkidMark)
	{
		delete m_pSkidMark;
		m_pSkidMark = NULL;
	}
	if (m_pBallWindTrack)
	{
		delete m_pBallWindTrack;
		m_pBallWindTrack = NULL;
	}
	if (m_pPuttLine)
	{
		delete m_pPuttLine;
		m_pPuttLine = NULL;
	}
	for (int i = 0; i < 32; ++i)
		if (m_pPuttLines[i])
		{
			delete m_pPuttLines[i];
			m_pPuttLines[i] = NULL;
		}
	if (m_pSkidMarkCobra)
	{
		delete m_pSkidMarkCobra;
		m_pSkidMarkCobra = NULL;
	}
	if (m_pExplodeMark)
	{
		delete m_pExplodeMark;
		m_pExplodeMark = NULL;
	}
}

void CTrajectory::OnPreLoadInit()
{
	CSceneManager::Instance()->RegisterElement(OBJ_EFFECT, NULL, this,
		"\xb1\xcb\xc0\xfb", 0, NULL);
}

void CTrajectory::OnLoad()
{
	m_pComet = new CComet;
	m_pDoughnut = new CDoughnut;
	m_pBallTrack = new CBallTrack;
	m_pSkidMark = new CSkidMark;
	m_pBallWindTrack = new CBallWindTrack;
	m_pPuttLine = new CPuttLine;
	m_pSkidMarkCobra = new ntSkidMark_Cobra;
	m_pExplodeMark = new ntExplodeMark;
}

void CTrajectory::OnProcess(float delta)
{
	if (GolfBall().m_playFrame >= 0)
		for (int i = 0; i < 2; ++i)
			if (m_pTrace[i])
				m_pTrace[i]->Process(delta);
}

void CTrajectory::Display()
{
	for (int i = 0; i < 32; ++i)
		if (m_pPuttLines[i])
			m_pPuttLines[i]->Display();
	if (!Skipped() && GolfBall().m_playFrame >= 0)
	{
		for (int i = 0; i < 2; ++i)
			if (m_pTrace[i])
				m_pTrace[i]->Display();
		m_pExplodeMark->Display();
	}
}

void CTrajectory::HandleMsg(const MsgObject& msg)
{
	CBallTrace** slot = NULL;
	if (msg.message != 396)
	{
		for (int i = 0; i < 2; ++i)
			if (!m_pTrace[i])
			{
				slot = &m_pTrace[i];
				break;
			}
		if (!slot)
			return;
	}
	switch (msg.message)
	{
	case 344:
		if (m_pComet)
		{
			*slot = m_pComet;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_EFFECT)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_EFFECT);
		}
		break;
	case 348:
		if (m_pDoughnut)
		{
			*slot = m_pDoughnut;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_EFFECT)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_EFFECT);
		}
		break;
	case 345:
		if (m_pBallTrack)
		{
			*slot = m_pBallTrack;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_EFFECT)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_EFFECT);
		}
		break;
	case 346:
		if (m_pSkidMark)
		{
			m_pSkidMark->SetFrame(GolfBall().m_afterBurnerFrame,
				GolfBall().m_afterBurnerFrame + 75);
			*slot = m_pSkidMark;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_FUNC)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_FUNC);
		}
		break;
	case 347:
		if (m_pBallWindTrack)
		{
			*slot = m_pBallWindTrack;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_EFFECT)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_EFFECT);
		}
		break;
	case 349:
		if (m_pPuttLine)
		{
			m_pLastPuttLine = new CPuttLine;
			for (int i = 0; i < 32; ++i)
				if (!m_pPuttLines[i])
				{
					m_pPuttLines[i] = m_pLastPuttLine;
					break;
				}
			*slot = m_pLastPuttLine;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_FUNC)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_FUNC);
		}
		break;
	case 350:
		if (m_pSkidMarkCobra)
		{
			m_pSkidMarkCobra->SetFrame(0, GolfBall().m_cobraFrame);
			*slot = m_pSkidMarkCobra;
			(*slot)->Init();
			if (CSceneManager::Instance()->GetType("\xb1\xcb\xc0\xfb") !=
				OBJ_FUNC)
				CSceneManager::Instance()->SetType("\xb1\xcb\xc0\xfb",
					OBJ_FUNC);
		}
		break;
	case 351:
		m_pExplodeMark->Init(GolfBall().m_pos);
		break;
	case 396:
	{
		WPuppet* ball = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (ball && ball->GetRootBone())
			ball->SetAlpha(255);
		for (int j = 0; j < 2; ++j)
			m_pTrace[j] = NULL;
		if (m_pExplodeMark)
			m_pExplodeMark->Uninit();
		if (m_pLastPuttLine)
		{
			if (!m_pLastPuttLine->IsKeep())
			{
				for (int j = 0; j < 32; ++j)
					if (m_pPuttLines[j] == m_pLastPuttLine && m_pPuttLines[j])
					{
						delete m_pPuttLines[j];
						m_pPuttLines[j] = NULL;
					}
			}
			else
				m_pLastPuttLine->RemoveUnkeptAlpha();
		}

		int i;
		for (i = 0; i < 32; ++i)
			if (!m_pPuttLines[i])
				break;
		if (i == 32)
		{
			int oldest = 0;
			int index;
			for (int j = 0; j < 32; ++j)
				if (m_pPuttLines[j] && m_pPuttLines[j]->HowOldAreYou() > oldest)
				{
					oldest = m_pPuttLines[j]->HowOldAreYou();
					index = j;
				}
			if (m_pPuttLines[index])
			{
				delete m_pPuttLines[index];
				m_pPuttLines[index] = NULL;
			}
		}
		break;
	}
	case 352:
	{
		WPuppet* ball = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (ball && ball->GetRootBone())
			ball->SetAlpha(255);
		m_pLastPuttLine = NULL;
		for (int i = 0; i < 32; ++i)
			if (m_pPuttLines[i])
			{
				delete m_pPuttLines[i];
				m_pPuttLines[i] = NULL;
			}
		break;
	}
	}
}

CBallTrace::CBallTrace()
{
	m_reserved = 0;
	m_texHandle = 0;
	m_bActive = false;
	m_vl[0] = &m_vtx[0];
	m_vl[1] = &m_vtx[1];
	m_vl[2] = &m_vtx[3];
	m_vl[3] = &m_vtx[2];
	m_vl[4] = NULL;
}

CBallTrack::CBallTrack()
{
	m_pPoint = NULL;
	m_bRainbow = false;
	m_pPointEx = NULL;
	m_rainbowTexHandle = 0;
	m_texHandle = g_resrcmng->LoadTexture("[swingtale01.jpg", 0, 0, NULL);
}

CBallTrack::~CBallTrack()
{
	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
	if (g_resrcmng && m_rainbowTexHandle)
	{
		g_resrcmng->Release(m_rainbowTexHandle);
		m_rainbowTexHandle = 0;
	}
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
	if (m_pPointEx)
	{
		delete[] m_pPointEx;
		m_pPointEx = NULL;
	}
}

void CBallTrack::Init()
{
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}

	if (m_pPointEx)
	{
		delete[] m_pPointEx;
		m_pPointEx = NULL;
	}

	if ((Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall &
			0x3ffffff) == 32)
	{
		m_bRainbow = true;
		m_pPointEx = new sPointEx[GolfBall().m_renderNum + 1];
		if (!m_rainbowTexHandle)
			m_rainbowTexHandle =
				g_resrcmng->LoadTexture("[rainbow_001.jpg", 0, 0, NULL);
	}
	else
	{
		m_bRainbow = false;
	}

	m_pPoint = new sPoint[GolfBall().m_renderNum + 1];
	m_bActive = true;
	m_fadeDuration = Between(.5f,
		float(GolfBall().m_renderNum - GolfBall().m_bounceFrame) * .02f, 2.f);
	m_maxAlpha = 255;
	m_alpha = 255;
	m_fadeTime = GolfBall().m_bounceFrame * .02f + m_fadeDuration - 1;
	m_lastFrame = 0;
	m_pointNum = 1;
	WVector side =
		WCrossProduct(WVector::UNIT_POS_Y, GolfBall().PreData(0)->vel);
	side.Normalize();
	side *= GolfBall().GetRadius() * 1.2f;
	m_pPoint[0].pos1 = GolfBall().PreData(0)->pos + side;
	m_pPoint[0].pos2 = GolfBall().PreData(0)->pos - side;

	if (m_bRainbow)
	{
		m_pPointEx[0].center = GolfBall().PreData(0)->pos;
		m_pPointEx[0].side = side;
		m_pPointEx[0].scale = 0;
		m_pPointEx[0].bActive = true;
		for (int i = 1; i < GolfBall().m_renderNum + 1; ++i)
		{
			m_pPointEx[i].bActive = false;
			m_pPointEx[i].scale = 0;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		m_vtx[i].tu = i & 2 ? 1.f : 0.f;
		m_vtx[i].tv = i & 1 ? 0.f : 1.f;
	}
}

void CBallTrack::Process(float delta)
{
	if (!m_bActive)
		return;

	int frame = GolfBall().m_playFrame;
	if (m_lastFrame != frame)
	{
		for (int i = m_lastFrame + 1; i <= frame; ++i)
		{
			if (i - m_lastFrame >= 5 || GolfBall().PreData(i)->event ||
				!((GolfBall().PreData(i)->pos -
					  GolfBall().PreData(m_lastFrame)->pos)
						.Magnitude() < 3.2f))
			{
				if (GolfBall().m_state)
				{
					WVector side;
					side = WCrossProduct(WVector::UNIT_POS_Y,
						GolfBall().PreData(i)->vel);
					side.Normalize();
					side *= GolfBall().GetRadius() * 1.2f;
					m_pPoint[m_pointNum].pos1 =
						GolfBall().PreData(i)->pos + side;
					m_pPoint[m_pointNum].pos2 =
						GolfBall().PreData(i)->pos - side;
					++m_pointNum;
					m_lastFrame = i;
					if (m_bRainbow)
					{
						m_pPointEx[m_pointNum].center =
							GolfBall().PreData(i)->pos;
						m_pPointEx[m_pointNum].side = side;
						m_pPointEx[m_pointNum].bActive = true;
					}
				}
			}
		}
	}

	if (m_bRainbow)
	{
		UpdateRainbowEffect(delta);
	}

	if (m_lastFrame < GolfBall().m_renderNum && GolfBall().m_state)
	{
		WVector side;
		side =
			WCrossProduct(WVector::UNIT_POS_Y, GolfBall().PreData(frame)->vel);
		side.Normalize();
		side *= GolfBall().GetRadius() * 1.2f;
		m_pPoint[m_pointNum].pos1 = GolfBall().m_pos + side;
		m_pPoint[m_pointNum].pos2 = GolfBall().m_pos - side;
		if (m_bRainbow)
		{
			m_pPointEx[m_pointNum].center = GolfBall().m_pos;
			m_pPointEx[m_pointNum].side = side;
			m_pPointEx[m_pointNum].bActive = true;
		}
	}

	float fade = .6f;
	if (m_lastFrame > GolfBall().m_renderNum - 30)
		fade = 1.2f;
	for (int i = 0; i <= m_pointNum; ++i)
	{
		m_pPoint[i].alpha -= fade * delta;
		if (m_pPoint[i].alpha < 0)
			m_pPoint[i].alpha = 0;
	}

	m_fadeTime -= delta;
	if (m_fadeTime < m_fadeDuration)
		m_alpha -= (1.f / m_fadeDuration) * m_maxAlpha * delta;
	else
		m_alpha = m_maxAlpha;
}

void CBallTrack::Display()
{
	if (!m_bActive)
		return;

	int texture = m_texHandle;
	int extra = m_lastFrame < GolfBall().m_renderNum;
	if (m_bRainbow)
		texture = m_rainbowTexHandle;
	for (int i = 1; i < m_pointNum + extra; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int point = j & 2 ? i : i - 1;
			m_vtx[j].diffuse =
				((int)(m_pPoint[point].alpha * 255) << 24) | 0xffffff;
			m_vtx[j].SetPosition(
				(j & 1 ? m_pPoint[point].pos2 : m_pPoint[point].pos1));
		}
		g_view->DrawPolygonFan(m_vl, (texture & 0x7ff) | 0xc00000, 0x400,
			false);
	}
}

void CBallTrack::UpdateRainbowEffect(float delta)
{
	for (int i = 0; i < m_pointNum; ++i)
	{
		if (m_pPointEx[i].bActive == true)
			m_pPointEx[i].scale += delta * 5;
		if (m_pPointEx[i].scale > 1)
		{
			m_pPoint[i].pos1 =
				m_pPointEx[i].center + m_pPointEx[i].scale * m_pPointEx[i].side;
			m_pPoint[i].pos2 =
				m_pPointEx[i].center - m_pPointEx[i].scale * m_pPointEx[i].side;
		}
	}
}

CSkidMark::CSkidMark()
{
	m_pPoint = NULL;
	m_startFrame = -1;
	m_endFrame = -1;
	m_pointNum = 0;
	m_curPoint = 0;
	m_texHandle = g_resrcmng->LoadTexture("~skidmark01.jpg", 0, 0, NULL);
}

CSkidMark::~CSkidMark()
{
	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}

	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}

	m_pointNum = 0;
	m_curPoint = 0;
}

void CSkidMark::SetFrame(int start, int end)
{
	m_startFrame = Max(start, GolfBall().m_playFrame);
	m_endFrame = Min(end, GolfBall().m_renderNum);
}

unsigned char CSkidMark::GetPointAlpha(int index, int frame)
{
	if (index > 0 && frame > 0)
	{
		WVector pos = GolfBall().PreData(frame)->pos;
		WVector prev = GolfBall().PreData(frame - 1)->pos;
		if ((pos - prev).Magnitude() < .01f)
		{
			int alpha = (int)(m_pPoint[index - 1].alpha * 1.1);
			if (alpha >= 255)
				alpha = 255;
			return (unsigned char)alpha;
		}
	}

	return (int)(float(m_pointNum - index) / m_pointNum * 192);
}

void CSkidMark::Init()
{
	if (m_startFrame < GolfBall().m_playFrame)
		m_startFrame = GolfBall().m_playFrame;
	if (m_endFrame < 0 || m_endFrame > GolfBall().m_renderNum)
		m_endFrame = GolfBall().m_renderNum;

	m_pointNum = m_endFrame - m_startFrame + 1;
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}

	if (m_pointNum > 0)
		m_pPoint = new sPoint[m_pointNum];

	m_curPoint = 0;
	m_lastFrame = m_startFrame - 1;
	for (int i = 0; i < 4; ++i)
	{
		m_vtx[i].tu = i & 2 ? 1.f : 0.f;
		m_vtx[i].tv = i & 1 ? 0.f : 1.f;
	}

	m_bActive = true;
}

void CSkidMark::Process(float delta)
{
	if (!m_bActive)
		return;
	int frame = GolfBall().m_playFrame;
	if (frame >= m_startFrame)
	{
		WVector side =
			WCrossProduct(WVector::UNIT_POS_Y, GolfBall().PreData(frame)->vel);
		side.Normalize();
		side *= GolfBall().GetRadius() * 1.2f;
		for (int i = m_lastFrame + 1; i <= frame; ++i)
			if (m_curPoint < m_pointNum)
			{
				WVector pos = GolfBall().PreData(i)->pos;
				pos.y = GetPVS().GetGroundPoint(pos, true, NULL, true).y;
				m_pPoint[m_curPoint].pos[0] = pos + side;
				m_pPoint[m_curPoint].pos[1] = pos - side;
				m_pPoint[m_curPoint].alpha = GetPointAlpha(m_curPoint, i);
				++m_curPoint;
			}
		m_lastFrame = frame;
		if (frame < m_endFrame && m_curPoint < m_pointNum)
		{
			WVector pos = GolfBall().m_pos;
			pos.y = GetPVS().GetGroundPoint(pos, true, NULL, true).y;
			m_pPoint[m_curPoint].pos[0] = pos + side;
			m_pPoint[m_curPoint].pos[1] = pos - side;
			m_pPoint[m_curPoint].alpha = GetPointAlpha(m_curPoint, m_lastFrame);
		}
	}
}

void CSkidMark::Display()
{
	if (!m_bActive)
		return;
	int count;
	if (m_lastFrame < m_endFrame)
		count = m_curPoint + 1;
	else
		count = m_curPoint;
	int pointNum = Min(m_pointNum, count);
	for (int i = 1; i < pointNum; ++i)
	{
		int a = m_pPoint[i].alpha;
		unsigned long color = 0xff000000 | (a << 16) | (a << 8) | a;
		for (int j = 0; j < 4; ++j)
		{
			m_vtx[j].diffuse = color;
			int point = j & 2 ? i : i - 1;
			m_vtx[j].SetPosition(
				(j & 1 ? m_pPoint[point].pos[1] : m_pPoint[point].pos[0]));
		}
		g_view->DrawPolygonFan(m_vl, (m_texHandle & 0x7ff) | 0x21b00000, 0x400,
			false);
	}
}

CBallWindTrack::CBallWindTrack()
	: m_pLineEffect(NULL)
{
	m_prevCamPos.Reset();
	m_camMove.Reset();
	m_pLineEffect = new sLineEffect[256];
}

CBallWindTrack::~CBallWindTrack()
{
	if (m_pLineEffect)
	{
		delete[] m_pLineEffect;
		m_pLineEffect = NULL;
	}
}

void CBallWindTrack::Init()
{
	m_ballTrack.Init();
	m_bActive = true;
	for (int i = 0; i < 256; ++i)
		m_pLineEffect[i].bActive = false;
}

void CBallWindTrack::Process(float delta)
{
	m_ballTrack.Process(delta);
	if (!m_bActive)
		return;

	if (GolfBall().m_bounceFrame <= 0 ||
		GolfBall().m_playFrame <=
			(GolfBall().m_bounceFrame + GolfBall().m_topFrame) / 2)
	{
		m_camMove = g_camera.pivot - m_prevCamPos;
		float length = m_camMove.Magnitude();
		if (length < 1)
		{
			m_prevCamPos = g_camera.pivot;
			return;
		}
		{
			m_camMove.Normalize();
			for (int i = 0; i < 256; ++i)
			{
				if (m_pLineEffect[i].bActive)
				{
					if ((m_pLineEffect[i].start - g_camera.pivot) *
								g_camera.za <
							0 ||
						(m_pLineEffect[i].start - GolfBall().m_pos)
								.Magnitude() > 160)
						m_pLineEffect[i].bActive = false;
					else
					{
						m_pLineEffect[i].alpha = (int)(128 *
							Min((m_pLineEffect[i].start - g_camera.pivot)
										.Magnitude() *
									.005f,
								1.f));
						m_pLineEffect[i].start -= (m_camMove * length) * .5f;
						m_pLineEffect[i].end = m_pLineEffect[i].start -
							m_camMove * m_pLineEffect[i].length;
					}
				}
				else
				{
					m_pLineEffect[i].start = g_camera.pivot + m_camMove * 100 +
						WVector(Random(-1.f, 1.f), Random(-1.f, 1.f),
							Random(-1.f, 1.f)) *
							75;
					m_pLineEffect[i].length = Random(-1.f, 1.f) * 30 + 100;
					m_pLineEffect[i].bActive = true;
					m_pLineEffect[i].alpha = 0;
				}
			}
			WVector direction = GolfBall().m_vel;
			direction.Normalize();
			m_lineFactor = direction.y > 0 ? 1.f : direction.y + .5f;
			m_prevCamPos = g_camera.pivot;
		}
	}
	else
	{
		m_bActive = false;
	}
}

void CBallWindTrack::Display()
{
	m_ballTrack.Display();
	if (m_bActive && m_lineFactor > 0)
	{
		bool moving = m_camMove.Magnitude() != 0.f;
		if (moving)
		{
			for (int i = 0; i < 256; ++i)
			{
				if (m_pLineEffect[i].bActive)
				{
					g_view->DrawLine(m_pLineEffect[i].start,
						((int)(m_pLineEffect[i].alpha * m_lineFactor) << 24) |
							0xffffff,
						m_pLineEffect[i].end, 0, 0x20c00000);
				}
			}
		}
	}
}

CComet::CComet()
{
	m_cometTexHandle =
		g_resrcmng->LoadTexture("~ball_straight.jpg", 0, 0, NULL);
	m_genInterval = .1f;
	m_genNum = 20;

	for (int i = 0; i < 4; ++i)
		m_vtx[i].diffuse = 0xffffffff;
}

CComet::~CComet()
{
	if (m_cometTexHandle)
	{
		g_resrcmng->Release(m_cometTexHandle);
		m_cometTexHandle = 0;
	}
}

void CComet::Init()
{
	m_length = 1;
	m_tailNum = 0;
	m_genIndex = 0;
	m_alpha = 255;
	m_genTime = 0;
	m_bActive = true;
	WVector side = WCrossProduct(GolfBall().m_vel, g_camera.za);
	side = side.Normalize() * 2 * GolfBall().GetRadius();
	m_point[0][0] = GolfBall().m_pos + side;
	m_point[0][1] = GolfBall().m_pos - side;
}

void CComet::Generate()
{
	WVector side = WCrossProduct(GolfBall().m_vel, g_camera.za);
	side = side.Normalize() * 2 * GolfBall().GetRadius();
	m_genIndex = (m_genIndex + 1) % m_genNum;
	m_point[m_genIndex][0] = GolfBall().m_pos + side;
	m_point[m_genIndex][1] = GolfBall().m_pos - side;
	m_tailNum = 0;
	if ((int)m_length < m_genNum)
		m_length += 1;
}

void CComet::Process(float delta)
{
	if (!m_bActive)
		return;

	if (GolfBall().m_vel.y < 0)
	{
		m_length = (m_length - .5f) > 0 ? m_length - .5f : 0.f;
		m_genTime += Min(delta, delta * m_alpha * .005f);
		m_alpha = 0.f < (m_alpha - delta * 150) ? m_alpha - delta * 150 : 0.f;
		if (Wabs(m_alpha) < g_EPSILON)
		{
			m_bActive = false;
			m_alpha = 0;
		}
	}
	else
	{
		m_genTime += delta;
	}

	if (m_genTime > m_genInterval)
	{
		m_genTime -= m_genInterval;
		Generate();
	}
	else
	{
		WVector side = WCrossProduct(GolfBall().m_vel, g_camera.za);
		side.Normalize();
		side *= GolfBall().GetRadius() * 2;
		m_tail[0] = GolfBall().m_pos + side;
		m_tail[1] = GolfBall().m_pos - side;
		m_tailNum = 1;
	}
}

void CComet::Display()
{
	if (!m_bActive)
		return;

	unsigned long color = ((int)m_alpha << 24) | 0xffffff;
	for (int i = 0; i < (int)m_length - 1; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int point;
			if (j & 2)
				point = (m_genIndex - i + m_genNum - 1) % m_genNum;
			else
				point = (m_genIndex - i + m_genNum) % m_genNum;
			m_vtx[j].SetPosition(m_point[point][j & 1]);
			m_vtx[j].tu = j & 2
				? float(m_tailNum + i + 1) / (m_tailNum + (int)m_length)
				: float(m_tailNum + i) / (m_tailNum + (int)m_length);
			m_vtx[j].tv = j & 1 ? 0.f : 1.f;
			m_vtx[j].diffuse = color;
		}
		g_view->DrawPolygonFan(m_vl, (m_cometTexHandle & 0x7ff) | 0x20c00000,
			0x400, false);
	}
	if (m_tailNum && m_length >= 1)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_vtx[j].SetPosition(
				j & 2 ? m_point[m_genIndex][j & 1] : m_tail[j & 1]);
			m_vtx[j].tu = j & 2 ? 1.f / (m_length + 1) : 0.f;
			m_vtx[j].tv = j & 1 ? 0.f : 1.f;
			m_vtx[j].diffuse = color;
		}
		g_view->DrawPolygonFan(m_vl, (m_cometTexHandle & 0x7ff) | 0x20c00000,
			0x400, false);
	}
}

CDoughnut::CDoughnut()
{
	m_genNum = 10;
	m_pParticle = new sParticle[m_genNum];
	m_texHandle = g_resrcmng->LoadTexture("~ball_circle01.jpg", 0, 0, NULL);
	m_genInterval = .1f;
}

CDoughnut::~CDoughnut()
{
	if (m_pParticle)
	{
		delete[] m_pParticle;
		m_pParticle = NULL;
	}

	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
}

void CDoughnut::Init()
{
	for (int i = 0; i < m_genNum; ++i)
		m_pParticle[i].bActive = false;

	m_genTime = 0;
	m_genIndex = 0;
	m_bActive = true;
}

void CDoughnut::Process(float delta)
{
	if (!m_bActive)
		return;

	if (GolfBall().m_bounceFrame > 0)
	{
		m_bActive = false;
		return;
	}

	for (int i = 0; i < m_genNum; ++i)
	{
		if (m_pParticle[i].bActive)
		{
			m_pParticle[i].alpha -= delta * 255;
			m_pParticle[i].size -= delta * .8f;
			if (m_pParticle[i].alpha < 0)
				m_pParticle[i].bActive = false;
		}
	}

	m_genTime += delta;
	if (m_genTime > m_genInterval)
	{
		m_genTime -= m_genInterval;
		m_pParticle[m_genIndex].bActive = true;
		m_pParticle[m_genIndex].alpha = 255;
		m_pParticle[m_genIndex].size = 1;
		m_pParticle[m_genIndex].mat = MakeCameraMatrix(GolfBall().m_pos,
			GolfBall().m_pos + GolfBall().m_vel);
		m_genIndex = (m_genIndex + 1) % m_genNum;
	}
}

void CDoughnut::Display()
{
	if (!m_bActive)
		return;

	for (int i = 0; i < m_genNum; ++i)
	{
		if (!m_pParticle[i].bActive)
			continue;

		for (int j = 0; j < 4; ++j)
		{
			WVector pos;
			pos = m_pParticle[i].mat.pivot;
			pos += ((j & 2 ? 1.f : -1.f) * m_pParticle[i].mat.xa +
					   (j & 1 ? 1.f : -1.f) * m_pParticle[i].mat.ya) *
				(m_pParticle[i].size * .5f);
			m_vtx[j].SetPosition(pos);
			m_vtx[j].diffuse = ((int)m_pParticle[i].alpha << 24) + 0xffffff;
			m_vtx[j].tu = j & 2 ? 1.f : 0.f;
			m_vtx[j].tv = j & 1 ? 0.f : 1.f;
		}

		g_view->DrawPolygonFan(m_vl, (m_texHandle & 0x7ff) | 0xc00000, 0x400,
			false);
	}
}

CPuttLine::CPuttLine()
{
	m_pPoint = NULL;
}

CPuttLine::~CPuttLine()
{
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
}

void CPuttLine::Init()
{
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}

	m_pPoint = new sPoint[GolfBall().m_renderNum + 1];
	m_lastPoint = GolfBall().m_renderNum;
	m_createTime = timeGetTime();
	m_bActive = true;

	for (int i = 0; i <= GolfBall().m_renderNum; ++i)
	{
		WVector side =
			WCrossProduct(WVector::UNIT_POS_Y, GolfBall().PreData(i)->vel);
		side.Normalize();
		side *= GolfBall().GetRadius() * .45f;
		m_pPoint[i].pos[0] = GolfBall().PreData(i)->pos + side;
		m_pPoint[i].pos[1] = GolfBall().PreData(i)->pos - side;
		m_pPoint[i].pos[0].y -= GolfBall().GetRadius();
		m_pPoint[i].pos[1].y -= GolfBall().GetRadius();
		m_pPoint[i].alpha = 0;
	}
}

void CPuttLine::Process(float delta)
{
	if (GOLFDOC()->m_gameMode == 8)
		m_bActive = false;

	if (!m_bActive || !m_pPoint || !GolfBall().m_state)
		return;

	int fadeStart = Max(0, GolfBall().m_playFrame - 150);
	int fadeEnd = Max(0, GolfBall().m_playFrame - 25);
	for (int i = 0; i <= GolfBall().m_playFrame; ++i)
	{
		if (GolfBall().PreData(i)->state == CGolfBall::BALL_HOLE)
		{
			m_pPoint[i].alpha = 0;
			continue;
		}

		if (GetCurMap() == 10)
		{
			if (GolfBall().PreData(i)->state != CGolfBall::BALL_ROLL)
			{
				m_pPoint[i].alpha = 0;
				continue;
			}
			unsigned char ground =
				GetPVS().GetGroundType(GolfBall().PreData(i)->pos, true, NULL);
			if (ground == 3 || ground == 4 || ground == 11)
			{
				m_pPoint[i].alpha = 256;
				continue;
			}
		}

		if (i <= fadeStart)
		{
			m_pPoint[i].alpha = 0;
		}
		else if (i >= fadeEnd)
		{
			m_pPoint[i].alpha = 64;
		}
		else
		{
			if (fadeEnd == fadeStart)
				m_pPoint[i].alpha = 0;
			else
				m_pPoint[i].alpha =
					(i - fadeStart) * 64 / (fadeEnd - fadeStart);
		}
	}
}

void CPuttLine::Display()
{
	if (!m_bActive || !m_pPoint)
		return;

	for (int i = 1; i <= m_lastPoint; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int point = j & 2 ? i : i - 1;
			m_vtx[j].SetPosition(
				(j & 1 ? m_pPoint[point].pos[1] : m_pPoint[point].pos[0]));
			if (m_pPoint[point].alpha == 256)
				m_vtx[j].diffuse = 0x80e6c882;
			else
				m_vtx[j].diffuse = (m_pPoint[point].alpha << 24) | 0xffffff;
		}
		g_view->DrawPolygonFan(m_vl, 0x20300000, 0x400, false);
	}
}

bool CPuttLine::IsKeep()
{
	int i;
	for (i = 0; i <= m_lastPoint; ++i)
		if (m_pPoint[i].alpha == 256)
			break;
	return i != m_lastPoint ? true : false;
}

int CPuttLine::HowOldAreYou()
{
	return timeGetTime() - m_createTime;
}

void CPuttLine::RemoveUnkeptAlpha()
{
	for (int i = 0; i <= m_lastPoint; ++i)
		if (m_pPoint[i].alpha != 256)
			m_pPoint[i].alpha = 0;
}

ntSkidMark_Cobra::ntSkidMark_Cobra()
{
	m_pPoint = NULL;
	m_startFrame = -1;
	m_endFrame = -1;
	m_pointNum = 0;
	m_curPoint = 0;
	m_texHandle = g_resrcmng->LoadTexture("[_cobra_ef.jpg", 0, 0, NULL);
}

ntSkidMark_Cobra::~ntSkidMark_Cobra()
{
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}

	m_pointNum = 0;
	m_curPoint = 0;
}

void ntSkidMark_Cobra::Init()
{
	if (m_startFrame < GolfBall().m_playFrame)
		m_startFrame = GolfBall().m_playFrame;
	if (m_endFrame < 0 || m_endFrame > GolfBall().m_cobraFrame)
		m_endFrame = GolfBall().m_renderNum;
	m_pointNum = m_endFrame - m_startFrame + 1;
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
	if (m_pointNum > 0)
	{
		m_pPoint = new sPoint[m_pointNum];
	}
	m_curPoint = 0;
	m_lastFrame = m_startFrame - 1;
	for (int i = 0; i < 4; ++i)
	{
		m_vtx[i].tu = i & 2 ? 1.f : 0.f;
		m_vtx[i].tv = i & 1 ? 0.f : 1.f;
	}
	m_bActive = true;
}

void ntSkidMark_Cobra::Process(float delta)
{
	if (!m_bActive)
		return;
	int frame = GolfBall().m_playFrame < GolfBall().m_cobraFrame
		? GolfBall().m_playFrame
		: GolfBall().m_cobraFrame;
	if (frame >= m_startFrame)
	{
		WVector side =
			WCrossProduct(WVector(0, 1, 0), GolfBall().PreData(frame)->vel);
		side.Normalize();
		side *= GolfBall().GetRadius() * 10;
		for (int i = m_lastFrame + 1; i <= frame; ++i)
			if (m_curPoint < m_pointNum)
			{
				WVector pos = GolfBall().PreData(i)->pos;
				pos.y = GetPVS().GetGroundPoint(pos, true, NULL, true).y;
				m_pPoint[m_curPoint].pos[0] = pos + side;
				m_pPoint[m_curPoint].pos[1] = pos - side;
				++m_curPoint;
			}
		m_lastFrame = frame;
		if (frame < m_endFrame && m_curPoint < m_pointNum)
		{
			WVector pos = GolfBall().m_pos;
			pos.y = GetPVS().GetGroundPoint(pos, true, NULL, true).y;
			m_pPoint[m_curPoint].pos[0] = pos + side;
			m_pPoint[m_curPoint].pos[1] = pos - side;
		}
	}
}

void ntSkidMark_Cobra::Display()
{
	if (!m_bActive)
		return;

	int count;
	if (m_lastFrame < m_endFrame)
		count = m_curPoint + 1;
	else
		count = m_curPoint;

	int pointNum = Min(m_pointNum, count);
	for (int i = 1; i < pointNum; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_vtx[j].diffuse = 0xffffffff;
			int point = j & 2 ? i : i - 1;
			m_vtx[j].SetPosition(
				(j & 1 ? m_pPoint[point].pos[1] : m_pPoint[point].pos[0]));
		}

		g_view->DrawPolygonFan(m_vl, (m_texHandle & 0x7ff) | 0x20200000, 0,
			false);
	}
}

void ntSkidMark_Cobra::SetFrame(int start, int end)
{
	m_startFrame = Max(start, GolfBall().m_playFrame);
	m_endFrame = Min(end, GolfBall().m_renderNum);
}

ntExplodeMark::ntExplodeMark()
{
	m_bActive = false;
	m_texHandle = g_resrcmng->LoadTexture("[_boom.jpg", 0, 0, NULL);
}

ntExplodeMark::~ntExplodeMark()
{
	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
}

void ntExplodeMark::Init(const WVector& pos)
{
	m_bActive = true;
	m_pos = pos;
}

void ntExplodeMark::Uninit()
{
	m_bActive = false;
}

void ntExplodeMark::Process()
{
}

void ntExplodeMark::Display()
{
	if (m_bActive)
		GetPVS().DrawDecal(m_texHandle,
			GetPVS().GetGroundPoint(m_pos, false, NULL, false),
			WVector(15, 0, 0), WVector(0, 0, 15), 1.f, 1.f, W_POLY_BLEND_NORMAL,
			false);
}
