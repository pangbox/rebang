#include "mathconsts.h"
#include "projectg.h"
#include "wresrcmng.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "fx.h"
#include "capturedbg.h"
#include "guideline.h"

IMPLEMENT_ACTOR(CGuideLine, IActor)

CGuideLine::CGuideLine()
{
	m_texHandle = 0;
	AddSkipList(0x85806);
}

void CGuideLine::OnPreLoadInit()
{
	m_vl[0] = &m_v[0];
	m_vl[1] = &m_v[1];
	m_vl[2] = &m_v[3];
	m_vl[3] = &m_v[2];
	m_vl[4] = NULL;

	for (int i = 0; i < 80; i++)
		m_road[i].seq = NULL;

	CSceneManager::Instance()->RegisterElement(OBJ_FUNC, NULL, this,
		"\xb0\xa1\xc0\xcc\xb5\xe5\xb6\xf3\xc0\xce", 0, NULL);

	m_texHandle = g_resrcmng->LoadTexture("~Track.jpg", 0, 0, 0);
}

void CGuideLine::OnLoad()
{
}

void CGuideLine::OnInit()
{
	m_bChanged = false;
	m_bRemake = false;
	m_bActive = false;
	m_lifeTime = 0.0f;
	m_roadNum = 0;
	m_bDelayFx = false;
}

void CGuideLine::OnDestroy()
{
	if (g_resrcmng && m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
}

void CGuideLine::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode < 15 || GolfBall().m_groundType != 2 ||
		!m_bActive || CCapturedBg::Instance()->IsCaptured())
		return;

	if (m_bRemake && !m_bChanged)
		CalcSegment();

	if (m_lifeTime > 0.0f)
	{
		m_lifeTime -= delta;
		if (m_lifeTime < 0.0f)
			SetActive(false);
	}

	m_bRemake = m_bChanged;
	m_bChanged = false;

	if (m_alpha > 0.0f && m_alpha < 255.0f)
	{
		m_alpha = Min(m_alpha + delta * 255.0f, 255.0f);

		for (int i = 0; i < 4; i++)
			m_v[i].diffuse = ((int)m_alpha << 24) | 0xffffff;
	}
}

void CGuideLine::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x1a0:
		SetActive(false);
		break;
	case 0x1a1:
		if (m_bActive)
		{
			if (!m_bRemake)
			{
				for (int i = 0; i < 80; i++)
				{
					if (!m_road[i].seq)
						break;
					CFx::Instance()->CloseSequence(m_road[i].seq, false);
					m_road[i].seq = NULL;
				}
			}
			m_bChanged = true;
		}
		break;
	case 0x1a2:
		SetActive(msg.param1 != 0);
		break;
	}
}

void CGuideLine::SetActive(bool bActive)
{
	if (m_bActive)
	{
		for (int i = 0; i < 80; i++)
		{
			if (!m_road[i].seq)
				break;
			CFx::Instance()->CloseSequence(m_road[i].seq, false);
			m_road[i].seq = NULL;
		}
	}

	m_bActive = bActive;
	if (bActive)
	{
		m_bRemake = false;
		m_bChanged = false;
		m_bDelayFx = true;
		CalcSegment();
		m_lifeTime = m_roadNum * 0.05f + 5.0f;
	}
}

void CGuideLine::Display()
{
	if (Skipped())
		return;

	if (GOLFDOC()->m_tutorialMode < 15)
		return;

	if (GolfBall().m_groundType == 2 && !m_bRemake && m_bActive &&
		!CCapturedBg::Instance()->IsCaptured())
	{
		WTVertex* vertices = m_v;
		for (int i = 0; i < m_roadNum - 1; i++)
		{
			vertices[0].x = m_road[i].left.x;
			vertices[0].y = m_road[i].left.y;
			vertices[0].z = m_road[i].left.z;
			vertices[0].tu = 0.0f;
			vertices[0].tv = ((float)((double)(float)i)) * .1f;
			vertices[1].x = m_road[i].right.x;
			vertices[1].y = m_road[i].right.y;
			float& vertexZ = vertices[1].z;
			vertexZ = m_road[i].right.z;
			vertices[1].tv = ((float)((double)(float)i)) * .1f;
			vertices[1].tu = 1.0f;
			vertices[2].SetPosition(m_road[i + 1].left);
			vertices[2].tu = 0.0f;
			vertices[2].tv = (i + 1.f) * 0.1f;
			vertices[3].SetPosition(m_road[i + 1].right);
			vertices[3].tv = (i + 1.f) * 0.1f;
			vertices[3].tu = 1.0f;
			g_view->DrawPolygonFan(m_vl, m_texHandle & 0x7ff | 0x20b00000,
				0x400, 0);
		}
	}
}

void CGuideLine::CalcSegment()
{
	if (GolfBall().m_groundType != 2)
		return;

	GolfBall().Reset(GolfBall().m_pos);

	unsigned char cur = GOLFDOC()->m_currentPlayer;
	sPlayerData* pPlayer = GOLFDOC()->GetPlayer(cur);
	WVector pin = GOLFDOC()->GetHoleData().pin;
	float dx = pin.x - pPlayer->pos.x;
	float dz = pin.z - pPlayer->pos.z;
	float power = sqrtf(dz * dz + dx * dx);
	power /= GolfClub().GetRange() * 3.2f;
	power = GolfClub().GetPower() * sqrtf(power);

	float angle;
	SendMsg(this, "Player", 0x110, (int)&angle, 0, 0, 0);

	GolfBall().m_vel = power * RotMat(angle, 1).za;
	GolfBall().m_state = CGolfBall::BALL_ROLL;
	GolfBall().m_rollFrame = 0;

	m_alpha = 1.0f;
	m_roadNum = 0;

	SetPoint(GOLFDOC()->m_cameraData.mat.za * 1.6f);

	while (m_roadNum < 80 && GolfBall().m_state == CGolfBall::BALL_ROLL)
	{
		CGolfBall::sExpected* pExpected = GolfBall().NewPreData();
		GetPVS().BallProcess(0.02f);
		pExpected->pos = GolfBall().m_pos;

		WVector step;
		step = GolfBall().m_pos - m_road[m_roadNum - 1].pos;
		step.y = 0.0f;
		step.Normalize();
		step *= 1.6f;

		WVector dir[2];
		dir[0] = (m_road[m_roadNum - 1].pos + step) - GolfBall().m_pos;
		dir[1] = m_road[m_roadNum - 1].pos - GolfBall().m_pos;

		while (
			m_roadNum < 80 && dir[0].x * dir[1].x + dir[0].z * dir[1].z > 0.0f)
		{
			SetPoint(step);
			dir[0] = (m_road[m_roadNum - 1].pos + step) - GolfBall().m_pos;
			dir[1] = m_road[m_roadNum - 1].pos - GolfBall().m_pos;
		}
	}

	GolfBall().Reset(m_road[0].pos);
	GolfBall().m_groundType = 2;
	m_bDelayFx = false;
}

void CGuideLine::SetPoint(const WVector& step)
{
	if (m_roadNum == 0)
	{
		m_road[m_roadNum].pos = GolfBall().m_pos;
	}
	else
	{
		float radius = GolfBall().GetRadius();
		m_road[m_roadNum].pos =
			GetPVS().GetGroundPoint(m_road[m_roadNum - 1].pos + step, false,
				NULL, false) +
			WVector(0.0f, radius, 0.0f);
	}

	WVector side = WCrossProduct(step, WVector::UNIT_POS_Y).Normalize();
	side *= GolfBall().GetRadius();

	m_road[m_roadNum].left = side + m_road[m_roadNum].pos;
	m_road[m_roadNum].right = m_road[m_roadNum].pos - side;
	m_road[m_roadNum].left.y -= GolfBall().GetRadius();
	m_road[m_roadNum].right.y -= GolfBall().GetRadius();

	m_road[m_roadNum].seq =
		CFx::Instance()->OpenSequence("TrackItem.seq", false);
	m_road[m_roadNum].seq->Pos() = m_road[m_roadNum].pos;

	if (m_bDelayFx)
		m_road[m_roadNum].seq->SetDelay(m_roadNum * 50.0f);

	m_roadNum++;
}
