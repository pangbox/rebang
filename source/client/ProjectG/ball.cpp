#include "stdafx.h"
#include "ball.h"
#include "golfdoc.h"
#include "golfball.h"
#include "fx.h"
#include "soundmanager.h"
#include "quadtree.h"
#include "wbone.h"
#include "minatl.h"
#include "mathconsts.h"

extern WMatrix g_camera;
extern unsigned long g_CurrentTime;
float BALL_AMBIENT_RATIO = 0.8f;

IMPLEMENT_ACTOR(CBall, IActor)

CBall::CBall()
	: m_pPuppet(NULL), m_shadow(0), m_bVisible(true), m_lastFrame(-1)
{
	m_dispPriority = DISP_PRIORITY_1;
}

void CBall::OnPreLoadInit()
{
	m_axis = WVector::UNIT_POS_X;
	for (int i = 0; i < 4; ++i)
	{
		m_vertices[i].diffuse = 0x80ffffff;
		m_vertices[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_vertices[i].tv = (i & 1) ? 0.0f : 1.0f;
	}
	m_vtx[0] = &m_vertices[0];
	m_vtx[1] = &m_vertices[1];
	m_vtx[2] = &m_vertices[3];
	m_vtx[3] = &m_vertices[2];
	m_vtx[4] = NULL;
	m_light.type = 2;
	m_shadow = g_resrcmng->LoadTexture("[ball_shadow.jpg", 0, 0, 0);
	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, NULL, NULL,
		"\xb0\xf8", 8, NULL);
	CSceneManager::Instance()->RegisterElement(OBJ_FUNC, NULL, this,
		"\xb0\xf8\xb1\xd7\xb8\xb2\xc0\xda", 0, NULL);
}

void CBall::OnDestroy()
{
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end();
		++it)
	{
		if (*it)
		{
			if (g_resrcmng && (*it)->puppet)
			{
				g_resrcmng->Release((*it)->puppet, false);
				(*it)->puppet = NULL;
			}
			if (*it)
			{
				delete *it;
				*it = NULL;
			}
		}
	}
	m_balls.clear();
	if (g_resrcmng && m_shadow)
	{
		g_resrcmng->Release(m_shadow);
		m_shadow = 0;
	}
}

void CBall::OnLoad()
{
	if (IsLocalCountry(30))
	{
		sFxKeeper keeper = { this, &CFxKeeper::EmptyFx };
		CFx::Instance()->AddFxKeeper(keeper);
	}
	if (IsLocalContent((localContentType_t)63))
		AddBall(0x14000000);
	for (int i = 0; i < GOLFDOC()->m_playerNum; ++i)
		if (PLAYER(i)->state != 3)
			AddBall(Doc()->m_userInfo[i].userEquip.tidBall);
	if (m_balls.begin() != m_balls.end())
		m_pPuppet = (*m_balls.begin())->puppet;
}

void CBall::OnReload()
{
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end();
		it++)
	{
		if (*it)
		{
			if (g_resrcmng && (*it)->puppet)
			{
				g_resrcmng->Release((*it)->puppet, false);
				(*it)->puppet = NULL;
			}
			if (*it)
			{
				delete *it;
				*it = NULL;
			}
		}
	}
	m_balls.clear();
	CSceneManager::Instance()->SetPuppet("\xb0\xf8", NULL);
	OnLoad();
}

void CBall::OnInit()
{
	m_matrix.Reset();
	m_down = 0;
	m_downStart = -1;
	m_light.nearone = g_lightset.nearone;
}

CBall::sBall* CBall::AddBall(unsigned long typeId)
{
	sBall* ball = FindBall(typeId);
	if (ball)
		return ball;
	IFF_STRUCT::sBall* info = ItemManager()->FindBall(typeId);
	if (info)
	{
		ball = new sBall;
		ball->info = info;
		char name[40];
		sprintf(name, "%s.pet", info->Data);
		ball->puppet = g_resrcmng->GetPuppet(name, 0, true, false);
		m_balls.push_back(ball);
	}
	return ball;
}

CBall::sBall* CBall::FindBall(unsigned long typeId)
{
	std::list<sBall*>::iterator it;
	it = std::find_if(m_balls.begin(), m_balls.end(), sFindBall(typeId));
	if (it != m_balls.end())
		return *it;
	return NULL;
}

float CBall::GetDownDelta()
{
	if (m_downStart == GolfBall().m_renderNum)
		return 1.0f;
	float delta = Max(0.0f, float(GolfBall().m_playFrame - m_downStart));
	return delta / float(GolfBall().m_renderNum - m_downStart);
}

void CBall::OnProcess(float delta)
{
	if (IsLocalContent((localContentType_t)79) && Doc()->GetPlayingMode() == 2)
	{
		if (Doc()->m_replayState == 11 || Doc()->m_replayState == 12)
		{
			if (Doc()->m_replayPlayType == 2)
			{
				if (Doc()->m_replayState == 11)
				{
					if (Doc()->m_replayShotIndex > -1)
					{
						Doc()->m_replayShotIndex -= 2;
						if (Doc()->m_replayShotIndex == 0)
							Doc()->m_replayState = 1;
						else if (Doc()->m_replayShotIndex < 0)
							Doc()->m_replayState = 14;
					}
				}
				else if (Doc()->m_replayState == 12)
					Doc()->m_replayState = 3;
				AfxGetTask()->GetActor("GolfRule")
					<< MsgObject(NULL, 164, 0, 0, 0, 0, 0);
			}
			else if (Doc()->m_replayPlayType == 1)
				AfxGetTask()->GetActor("GolfRule")
					<< MsgObject(NULL, 164, 0, 0, 0, 0, 0);
		}
	}
	if (!m_bVisible)
		return;
	CGolfBall& ball = GolfBall();
	if (ball.m_state == CGolfBall::BALL_STOP)
	{
		m_matrix.pivot = ball.m_pos;
		if (m_down > 0)
			m_matrix.pivot.y -= ball.GetRadius() * m_down * 2;
		if (GOLFDOC()->m_gameMode & 0x80)
		{
			DetachFx(3);
			DetachFx(5);
		}
	}
	else
	{
		m_light.diffuse = g_lightset.diffuse;
		if (GolfBall().m_vel.Magnitude() < 10)
		{
			DetachFx(3);
			DetachFx(5);
		}
		m_axis = WCrossProduct(WVector::UNIT_POS_Y, ball.m_vel);
		if (WisEqual(m_axis, WVector::ZERO, 1e-5f))
			m_axis = WVector::UNIT_POS_Y;
		m_matrix = RotMat(m_axis, ball.m_rot.y);
		m_matrix.pivot = ball.m_pos;
		if (ball.m_playFrame > 0 && ball.m_afterBurnerFrame > 0 &&
			ball.m_playFrame >= ball.m_afterBurnerFrame &&
			ball.m_playFrame < ball.m_afterBurnerFrame + 50)
			m_matrix.pivot += WVector((rand() % 11 - 5) * 0.005f, 0,
				(rand() % 11 - 5) * 0.005f);
		if (m_down > 0)
			m_matrix.pivot.y = m_matrix.pivot.y -
				GetDownDelta() * ball.GetRadius() * m_down * 2;
		if (m_lastFrame < GolfBall().m_bounceFrame)
		{
			if (GolfBall().m_bounceFrame <= GolfBall().m_playFrame)
				AttachFx(4);
		}
		m_lastFrame = GolfBall().m_playFrame;
		sBall* info = FindBall(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
		if (info)
		{
			for (unsigned int i = 0; i < 4; ++i)
				if (info->spray[i])
				{
					TransformBall();
					return;
				}
			for (unsigned int i = 0; i < 4; ++i)
				if (info->sequence[i])
				{
					TransformBall();
					return;
				}
		}
	}
}

void CBall::TransformBall()
{
	float scale;
	if (GolfBall().m_playFrame < 0)
	{
		scale = 1.0f;
	}
	else
	{
		WVector projected[2];
		projected[0] = g_view->Projection(
			GolfBall().m_pos - g_camera.ya * GolfBall().GetRadius());
		projected[1] = g_view->Projection(
			g_camera.ya * GolfBall().GetRadius() + GolfBall().m_pos);
		scale = Between(0.1f, Abs(projected[0].y - projected[1].y), 1.5f);
		scale = 1.5f / scale;
	}
	if (m_pPuppet)
	{
		m_pPuppet->ApplyBones(NULL, m_matrix * scale);
		m_pPuppet->Transform(NULL, 1.0f, false);
		m_pPuppet->UpdateBSphere();
	}
}

void CBall::OnDisplay()
{
	if (!m_bVisible)
		return;
	TransformBall();
	if ((!m_pPuppet || m_pPuppet->GetAlpha(NULL)) &&
		(GolfBall().m_state || m_bLightChanged))
	{
		m_bLightChanged = false;
		if (m_pPuppet)
			m_pPuppet->UpdateLightSource(&m_light, false, false);
	}
}

void CBall::Display()
{
	if (Skipped())
		return;
	if (!m_bVisible)
		return;
	WVector pin = GetHoleCupPos();
	WVector delta = pin - GolfBall().m_pos;
	if (sqrtf(delta.x * delta.x + delta.z * delta.z) <
		GolfBall().GetRadius() * 4)
		return;
	float radius = GolfBall().GetRadius() * 1.2f;
	int index;
	WVector pos =
		GetPVS().GetGroundPoint(GolfBall().m_pos, true, &index, false);
	if (index != -1)
	{
		WVector normal = GetPVS().GetMeshNormal(index);
		WVector x = WCrossProduct(normal, WVector::UNIT_POS_Z);
		WVector y = WCrossProduct(normal, x);
		x.Normalize();
		y.Normalize();
		WVector base = pos + WVector(0, 0.001f, 0);
		WVector vertex;
		for (int i = 0; i < 4; ++i)
		{
			vertex = base;
			vertex += x * (((i & 2) ? 1.0f : -1.0f) * radius);
			vertex += y * (((i & 1) ? 1.0f : -1.0f) * radius);
			m_vertices[i].SetPosition(vertex);
		}
		g_view->DrawPolygonFan(m_vtx, (m_shadow & 0x7ff) | 0x22300000, 0x400,
			0);
	}
}

void CBall::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}
	switch (msg.message)
	{
	case 77:
	{
		unsigned long typeId = msg.param1;
		int player = msg.param2;
		DetachFx(0);
		DetachFx(1);
		sBall* ball = AddBall(typeId);
		if (ball)
		{
			Doc()->m_userInfo[player].userEquip.tidBall = typeId;
			m_pPuppet = ball->puppet;
			if (m_pPuppet)
				m_pPuppet->UpdateLightSource(&m_light, false, false);
			CSceneManager::Instance()->SetPuppet("\xb0\xf8", m_pPuppet);
			AttachFx(0);
			AttachFx(1);
		}
	}
	break;
	case 76:
	{
		unsigned long typeId = msg.param1;
		DetachFx(0);
		DetachFx(1);
		sBall* ball = AddBall(typeId);
		if (ball)
		{
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall =
				typeId;
			m_pPuppet = ball->puppet;
			if (m_pPuppet)
				m_pPuppet->UpdateLightSource(&m_light, false, false);
			CSceneManager::Instance()->SetPuppet("\xb0\xf8", m_pPuppet);
			AttachFx(0);
			AttachFx(1);
		}
	}
	break;
	case 355:
		m_bVisible = true;
		CSceneManager::Instance()->SetVisible("\xb0\xf8", true);
		break;
	case 356:
		m_bVisible = false;
		CSceneManager::Instance()->SetVisible("\xb0\xf8", false);
		break;
	case 416:
	{
		DetachFx(0);
		DetachFx(1);
		m_lastFrame = -1;
		sBall* ball = FindBall(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
		if (ball)
		{
			m_pPuppet = ball->puppet;
			if (m_pPuppet)
				m_pPuppet->UpdateLightSource(&m_light, false, false);
			CSceneManager::Instance()->SetPuppet("\xb0\xf8", m_pPuppet);
			AttachFx(0);
			AttachFx(1);
		}
	}
	break;
	case 491:
		m_bLightChanged = true;
		m_light.diffuse = ModifyColor(g_lightset.diffuse,
			GetPVS().GetShadowIntensity(GolfBall().m_pos));
		m_light.ambient = ModifyColor(g_lightset.ambient, BALL_AMBIENT_RATIO);
		break;
	case 493:
		switch (GetPVS().GetGroundType(GolfBall().m_pos, true, false))
		{
		case 0:
		case 1:
		case 2:
		case 6:
		case 9:
			m_down = 0;
			break;
		case 3:
		case 4:
		case 5:
			m_down = 1.0f - GetPVS().GetMinPowerOnGround(GolfBall().m_pos);
			break;
		}
		break;
	case 492:
		switch (GetPVS().GetGroundType(
			GolfBall().PreData(GolfBall().m_renderNum)->pos, true, false))
		{
		case 0:
		case 1:
		case 2:
		case 6:
		case 9:
			m_down = 0;
			m_downStart = -1;
			break;
		case 3:
		case 4:
		case 5:
			m_down = 1.0f -
				GetPVS().GetMinPowerOnGround(
					GolfBall().PreData(GolfBall().m_renderNum)->pos);
			m_downStart =
				Max(GolfBall().m_renderNum - 100, GolfBall().m_rollFrame);
			break;
		}
		break;
	case 494:
	{
		sBall* ball = FindBall(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
		if (ball)
		{
			AttachFx(0);
			AttachFx(1);
			ball->spray[2] = ball->spray[3] = NULL;
			ball->sequence[2] = ball->sequence[3] = NULL;
		}
	}
	break;
	case 495:
		AttachFx(2);
		AttachFx(3);
		break;
	}
}

void CBall::AttachFx(int type)
{
	if (type < 0 || type >= 7)
		return;
	sBall* ball = FindBall(
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
	if (!ball || !ball->info || !ball->info->Fx[type][0])
		return;
	int length = strlen(ball->info->Fx[type]);
	if (length < 5)
		return;
	Attacher attacher;
	WBone* bone;
	if (ball->info->FxBone[type][0] && m_pPuppet &&
		(bone = m_pPuppet->FindBone(ball->info->FxBone[type])))
	{
		attacher.mat[0] = &bone->GetMatrix();
	}
	else if (m_pPuppet)
	{
		w_bound_box* box;
		if (ball->info->FxBone[type][0])
			box = m_pPuppet->FindBoundBox(ball->info->FxBone[type]);
		else
			box = m_pPuppet->FindBoundBox("center");
		if (box)
		{
			if (box->bone)
				attacher.mat[0] = &box->bone->GetMatrix();
			attacher.pos[0] = &box->info->spherePivot;
		}
	}
	int index;
	switch (type)
	{
	case 0:
		index = 0;
		break;
	case 1:
		index = 1;
		break;
	case 3:
		index = 2;
		break;
	case 5:
		index = 3;
		break;
	default:
		index = -1;
		break;
	}
	void* fx =
		CFx::Instance()->Attach(attacher, ball->info->Fx[type], true, NULL);
	if (index == -1)
		return;
	if (!strcmpi(ball->info->Fx[type], "BFX_B3.seq"))
		g_audio->PlaySfx("Ball_B", NULL, 0, NULL, 0, 0.5f, 200.0f);
	if (!strcmpi(ball->info->Fx[type] + length - 3, "spr"))
		ball->spray[index] = (CFxSpray*)fx;
	else
		ball->sequence[index] = (CFxSequence*)fx;
}

void CBall::DetachFx(int type)
{
	if (type != 0 && type != 1 && type != 3 && type != 5)
		return;
	sBall* ball = FindBall(
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
	if (!ball)
		return;
	int index;
	switch (type)
	{
	case 0:
		index = 0;
		break;
	case 1:
		index = 1;
		break;
	case 3:
		index = 2;
		break;
	case 5:
		index = 3;
		break;
	default:
		return;
	}
	if (ball->spray[index])
	{
		CFx::Instance()->CloseSpray(ball->spray[index]);
		ball->spray[index] = NULL;
	}
	if (ball->sequence[index])
	{
		if (index == 0)
			CFx::Instance()->CloseSequence(ball->sequence[index], true);
		else
			CFx::Instance()->CloseSequence(ball->sequence[index], false);
		ball->sequence[index] = NULL;
	}
}

void CBall::EmptyFx(void* fx)
{
	if (!fx)
		return;
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end();
		it++)
	{
		sBall* ball = *it;
		if (ball)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (ball->spray[i] == fx)
					ball->spray[i] = NULL;
				if (ball->sequence[i] == fx)
					ball->sequence[i] = NULL;
			}
		}
	}
}
