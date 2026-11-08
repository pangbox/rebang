#include "stdafx.h"
#include "fx.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "clientsetting.h"
#include "wbone.h"
#include "mathconsts.h"

extern WMatrix g_camera;

#include "ground_info.h"

CBase::CBase()
	: m_ball(NULL), m_ringTexture(0), m_numberTexture(0)
{
	m_y = COption::Instance()->gGetBar_Y() + 4;
	m_x = s_barStart - 82;
	Load();
	m_ringTexture = g_resrcmng->LoadTexture("{condition_line.png", 0, 0, 0);
	m_numberTexture = g_resrcmng->LoadTexture("[font_conditoin.jpg", 0, 0, 0);
	m_spin.Reset();
	m_ringAngle = 0;
	m_angle = 0;
	m_camera.Reset();
	float scale = g_view->GetWidth() / 640.f;
	m_camera.pivot = WVector(0, 0, -1.8f) * scale;
	m_light.type = 2;
	m_light.ambient = 0x808080;
	m_light.diffuse = 0xffffff;
	m_light.nearone = WVector(-1.f, -1.f, 1.f).Normalize();
	m_normal.Reset();
	m_matrix.Reset();
	m_vtx[0] = &m_vertices[0];
	m_vtx[1] = &m_vertices[1];
	m_vtx[2] = &m_vertices[3];
	m_vtx[3] = &m_vertices[2];
	m_vtx[4] = NULL;
	m_penalty = 0;
	m_ringCount = 12;
	m_fx.Init(g_resrcmng);
	m_fx.SetCamera(&g_camera);
	m_fx.m_frameTime = 1000.f / 30.f;
}

CBase::~CBase()
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

			delete *it;
		}
	}

	m_balls.clear();
	if (m_ringTexture)
	{
		g_resrcmng->Release(m_ringTexture);
		m_ringTexture = 0;
	}

	if (m_numberTexture)
	{
		g_resrcmng->Release(m_numberTexture);
		m_numberTexture = 0;
	}

	m_fx.ClearAll(false);
}

void CBase::Load()
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

			delete *it;
		}
	}

	m_balls.clear();
	m_fx.ClearAll(false);
	for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; i++)
		if (PLAYER(i)->state != 3)
			AddBall(Doc()->m_userInfo[i].userEquip.tidBall);
}

void CBase::ResetUiPosition()
{
	float y = COption::Instance()->gGetBar_Y();
	m_y = y + 4;
	m_fx.SetScreenCenter(m_x, m_y);
	m_camera.pivot = WVector(0, 0, -1.8f) * (g_view->GetWidth() / 640.f);
}

void CBase::Process(float delta)
{
	if (GOLFDOC()->m_tutorialMode >= 12)
	{
		if (GolfBall().m_playFrame < 0)
		{
			AfxGetTask()->GetActor("Spin") << MsgObject(NULL, 353,
				(int)&m_spin.x, (int)&m_spin.y, 0, 0, 0);
			m_spin *= delta * (g_PI * 4);
			if (GolfClub().GetType() != 3)
				m_matrix *= RotMat(m_spin.x, 1) * RotMat(m_spin.y, 0);
		}
		else
		{
			m_matrix =
				RotMat(GolfBall().m_rot.x, 1) * RotMat(GolfBall().m_rot.y, 0);
		}

		m_ringAngle += delta * (g_PI / 10);
		delta *= 1000;
		m_fx.ProcessAttacher(delta);
		m_fx.Process(delta);
	}
}

void CBase::Display()
{
	float nearClip = g_view->GetClipNearValue();
	nearClip = Max(0.f, nearClip);
	float farClip = g_view->GetClipFarValue();
	float fov = g_view->GetFOV();
	g_view->SetFOV(g_view->GetRatio() * (120 * g_DEGTORAD));
	g_view->SetClip(.1f, 10, false);
	g_view->SetCamera(m_camera);
	g_view->SetScreenCenter(m_x, m_y);
	g_view->UpdateCamera();
	if (m_ball && m_ball->puppet)
	{
		m_ball->puppet->ApplyBones(NULL, m_matrix);
		m_ball->puppet->Transform(NULL, 1.f, false);
		m_ball->puppet->UpdateLightSource(&m_light, false, false);
		m_ball->puppet->Render(g_view, true, 1.f, true, true, false);
	}

	if (GolfBall().m_playFrame < 0)
		DrawRing();
	m_fx.Display(g_view);
	g_view->Flush(0);
	g_view->SetCamera(g_camera);
	g_view->SetClip(nearClip, farClip, false);
	g_view->SetFOV(fov);
	g_view->SetScreenCenter(g_view->GetWidth() * .5f,
		g_view->GetHeight() * .5f);
	g_view->UpdateCamera();
}

void CBase::DrawRing()
{
	g_view->SetScreenCenter(m_x, m_y);
	g_view->UpdateCamera();
	int num, penalty;
	if (GolfClub().GetType() == 3)
	{
		num = 12;
		penalty = 0;
	}
	else
	{
		num = m_ringCount;
		penalty = m_penalty;
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			WVector vertex;
			vertex = RotMat(float(i + (j & 1)) * (g_PI * 2) / num, 1).za * .17f;
			vertex.y = ((j & 2) ? -1.f : 1.f) * .05f;
			m_vertices[j].SetPosition(vertex * GetBaseMatrix());
			m_vertices[j].diffuse = 0xff00ff96;
		}

		if (i % (num / 2) < 2)
		{
			for (int j = 0; j < 4; j++)
			{
				m_vertices[j].tu = (!penalty ? 1.f : .85f) *
						(((i % 2) ? .125f : 0.f) +
							((j & 1) ? 1.f : 0.f) * .125f) +
					(penalty / 16) * .25f;
				m_vertices[j].tv =
					(penalty + int(((unsigned int)j >> 1) & 1)) * .0625f;
			}

			g_view->DrawPolygonFan(m_vtx, m_numberTexture & 0x7ff, 0, 0);
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				m_vertices[j].tu = (j & 1) ? 1.f : 0.f;
				m_vertices[j].tv = (j & 2) ? 0.f : 1.f;
			}

			g_view->DrawPolygonFan(m_vtx, m_ringTexture & 0x7ff, 0, 0);
		}
	}

	g_view->SetScreenCenter(g_view->GetWidth() * .5f,
		g_view->GetHeight() * .5f);
	g_view->UpdateCamera();
}

void CBase::SetNormal(const WVector& normal)
{
	m_normal = normal;
	GolfBall().m_groundType =
		GetPVS().GetGroundType(GolfBall().m_pos, true, NULL);
	GetPVS().SetCondition();
	if (GolfBall().m_groundType == 2)
	{
		GOLFDOC()->GetCameraData().pitch = -13 * g_DEGTORAD;
	}
	else
	{
		GOLFDOC()->GetCameraData().pitch = 0;
		PLAYER(GOLFDOC()->m_currentPlayer)->putt[GOLFDOC()->m_currentHole - 1] =
			0;
	}

	if (GolfBall().m_groundType == 2)
		m_penalty = 0;
	else
		m_penalty = 100 -
			int(GetPVS().GetMaxPowerOnGround(GolfBall().m_pos) * 100 + .1f);
	m_ringCount = m_penalty ? 16 : 12;
}

WMatrix CBase::GetBaseMatrix()
{
	WMatrix matrix;
	matrix.ya = m_normal;
	matrix.xa = WCrossProduct(matrix.ya, WVector::UNIT_POS_Z);
	matrix.za = WCrossProduct(matrix.xa, matrix.ya);
	matrix.xa.Normalize();
	matrix.za.Normalize();
	matrix.pivot.Reset();
	matrix = RotMat(m_ringAngle * -2.f, 1) * matrix * RotMat(m_angle * -1.f, 1);
	return matrix;
}

CBase::sBall* CBase::ChangeBall(unsigned long typeId)
{
	sBall* ball = AddBall(typeId);
	m_fx.ClearAll(false);
	if (ball)
	{
		m_ball = ball;
		AttachFx();
	}

	return ball;
}

CBase::sBall* CBase::AddBall(unsigned long typeId)
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

CBase::sBall* CBase::FindBall(unsigned long typeId)
{
	std::list<sBall*>::iterator it;
	it = std::find_if(m_balls.begin(), m_balls.end(), sFindBall(typeId));
	if (it != m_balls.end())
		return *it;
	return NULL;
}

void CBase::ChangeTurn()
{
	ChangeBall(Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall);
}

void CBase::AttachFx()
{
	sBall* ball = m_ball;
	if (m_ball && m_ball->info && m_ball->puppet)
	{
		switch (m_ball->info->c.TypeId)
		{
		case 0x14000001:
		case 0x14000008:
		case 0x14000009:
		case 0x1400000e:
		case 0x1400003c:
		case 0x1400003d:
			return;
		default:
			if (m_ball->info->Fx[0][0])
			{
				int length = strlen(ball->info->Fx[0]);
				if (length >= 5)
				{
					Attacher attacher;
					attacher.x = m_x;
					attacher.y = m_y;
					WBone* bone =
						m_ball->puppet->FindBone(m_ball->info->FxBone[0]);
					if (bone)
						attacher.mat[0] = &bone->GetMatrix();
					else
					{
						w_bound_box* box;
						if (m_ball->info->FxBone[0][0])
							box = m_ball->puppet->FindBoundBox(
								m_ball->info->FxBone[0]);
						else
							box = m_ball->puppet->FindBoundBox("center");
						if (box)
						{
							if (box->bone)
								attacher.mat[0] = &box->bone->GetMatrix();
							attacher.pos[0] = &box->info->spherePivot;
						}
					}

					if (!strcmpi(m_ball->info->Fx[0] + length - 3, "spr"))
					{
						CFxSpray* spray =
							m_fx.LoadSpray(m_ball->info->Fx[0], NULL);
						if (spray)
						{
							spray->m_attacher = &attacher;
							spray->m_centerX = attacher.x;
							spray->m_centerY = attacher.y;
							memcpy(spray->m_attachMat, attacher.mat,
								sizeof(attacher.mat));
							memcpy(spray->m_attachPos, attacher.pos,
								sizeof(attacher.pos));
							m_fx.AddSpray(spray);
						}
					}
					else
					{
						CFxSequence* sequence =
							m_fx.LoadSequence(m_ball->info->Fx[0], true, NULL);
						if (sequence)
						{
							sequence->m_attacher = &attacher;
							sequence->m_centerX = attacher.x;
							sequence->m_centerY = attacher.y;
							m_fx.AddSequence(sequence);
						}
					}
				}
			}
		}
	}
}
