#include "stdafx.h"

#include "camerawork.h"
#include "commonutil.h"
#include "soundmanager.h"
#include "shareddoc.h"
#include "golfdoc.h"
#include "wview.h"
#include "inputmanager.h"
#include "taskmanager.h"
#include "actor.h"
#include "golfball.h"
#include "golfcadie.h"
#include "quadtree.h"
#include "scenemanager.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wutil.h"
#include "club.h"
#include "wresrcmng.h"
#include "parttidlist.h"
#include "../../shared/classdefine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char s_scriptCmd[1024];

extern WMatrix g_camera;

float Random(float min, float max, int seed);

sCameraSelectionData puttTable[] = {
	{ 1,  false, false },
	{ 1,  false, false },
	{ 1,  false, false },
	{ 1,  false, false },
	{ 1,  false, false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 1,  false, false },
	{ 1,  false, false },
	{ -1, false, false },
	{ -1, false, false },
	{ -1, false, false },
	{ -1, false, false },
};

sCameraSelectionData driveTable[] = {
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ -1, true,  false },
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ 0,  true,  true  },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ 1,  true,  false },
	{ -1, true,  false },
	{ -1, true,  false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 1,  true,  false },
	{ -1, true,  false },
	{ 0,  false, false },
	{ -1, true,  false },
	{ -1, true,  false },
	{ 1,  true,  false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 0,  false, false },
	{ 0,  false, true  },
};

#define CAMERA_CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

enum
{
	DRIVE_CAMERA_BASE = 0,
	PUTT_CAMERA_BASE = 100
};

CCameraWork::CCameraWork()
	: m_pfnCamera(0), m_driveRateSum(0), m_puttRateSum(0), m_bScript(false)
{
	m_driveNum = 29;
	m_puttNum = 13;

	for (int i = 0; i < m_driveNum; i++)
	{
		m_driveRateSum += driveTable[i].rate > 0 ? driveTable[i].rate : 0;

		if (driveTable[i].bEnable)
		{
			m_enableList.push_back(i + DRIVE_CAMERA_BASE);
		}
	}

	for (int i = 0; i < m_puttNum; i++)
	{
		m_puttRateSum += Max(0, puttTable[i].rate);

		if (puttTable[i].bEnable)
		{
			m_enableList.push_back(i + PUTT_CAMERA_BASE);
		}
	}

	memset(m_charCam, 0, sizeof(m_charCam));
	m_pRankPuppet = 0;
	m_pPuppet = 0;
	m_pMotion = 0;
	m_roll = 0.0f;
	m_pPivot = &GolfBall().m_pos;
	m_bPitchLimit = false;
	m_bLock = false;
	m_bCheckCamera = true;
	m_manualType = 1;
	m_bInstalled = false;
}

CCameraWork::~CCameraWork()
{
	m_unusedList.clear();

	for (int i = 0; i < 4; i++)
	{
		if (g_resrcmng && m_charCam[i].pPuppet)
		{
			g_resrcmng->Release(m_charCam[i].pPuppet, false);
			m_charCam[i].pPuppet = 0;
		}
	}

	if (g_resrcmng && m_pRankPuppet)
	{
		g_resrcmng->Release(m_pRankPuppet, false);
		m_pRankPuppet = 0;
	}
}

void CCameraWork::Load(void)
{
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (GOLFDOC()->GetPlayer(i)->state == 3)
		{
			continue;
		}

		unsigned long tidChar = GOLFDOC()->GetPartTidList(i).m_charTid;

		if (FindHoleInCam(tidChar))
		{
			continue;
		}

		for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++)
		{
			if (GOLFDOC()->GetPlayer(j)->state == 3)
			{
				continue;
			}

			if (m_charCam[j].charTypeId == 0)
			{
				IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tidChar);

				if (pChar)
				{
					char name[64];
					sprintf(name, "%s_cam.apet", pChar->Data);
					m_charCam[j].pPuppet =
						g_resrcmng->GetPuppet(name, false, true, false);
				}
				else
				{
					m_charCam[j].pPuppet = 0;
				}

				m_charCam[j].charTypeId = tidChar;

				if (m_charCam[j].pPuppet == 0)
				{
					if ((tidChar & 0x3FFFFFF) == 4)
					{
						m_charCam[j].pPuppet = g_resrcmng->GetPuppet(
							"m_def_cam.apet", false, true, false);
					}
					else
					{
						m_charCam[j].pPuppet = g_resrcmng->GetPuppet(
							"f_def_cam.apet", false, true, false);
					}
				}
				break;
			}
		}
	}

	m_bAbsAnim = false;
}

WPuppet* CCameraWork::FindHoleInCam(unsigned long id)
{
	for (int i = 0; i < GetPlayerNum(); i++)
	{
		if (m_charCam[i].charTypeId == id)
		{
			return m_charCam[i].pPuppet;
		}
	}

	return 0;
}

void CCameraWork::ReplayCamera(bool keep)
{
	if (!keep)
	{
		m_replayList.clear();
	}

	int index;

	if (GolfClub().GetType() == 3)
	{
		index = m_selectType - 100;

		do
		{
			index = (index + 1) % m_puttNum;
		} while (puttTable[index].rate <= 0);

		index += 100;
	}
	else
	{
		index = m_selectType;

		do
		{
			index = (index + 1) % m_driveNum;
		} while (driveTable[index].rate <= 0);
	}

	m_input.Reset();
	m_selectType = index;
	m_nextType = -1;
	m_bReserved1a0 = false;
	m_bLock = false;
	m_fov = 55.0f * g_DEGTORAD;
	m_replayIndex = 0;
	SetCamera(index, true);
	CheckCameraType(true);
	InitCtrlPoints(m_type);
}

void CCameraWork::SelectCamera(int type)
{
	m_input.Reset();
	m_nextType = -1;
	m_bScript = false;
	m_pMotion = 0;

	m_inputMatrix = g_camera;
	m_matrix = g_camera;

	m_fov = 55.0f * g_DEGTORAD;
	m_bLock = type == -1;
	m_rand = rand();
	m_replayIndex = -1;
	m_replayList.clear();
	m_pPivot = &GolfBall().m_pos;

	if (type == -1)
	{
		if (GolfClub().GetType() == 3)
		{
			int pick = m_rand % m_puttRateSum;
			type = 100;

			for (int i = 0; i < m_puttNum; i++)
			{
				pick -= puttTable[i].rate > 0 ? puttTable[i].rate : 0;

				if (pick < 0)
				{
					type = i + 100;
					break;
				}
			}
		}
		else
		{
			int pick = m_rand % m_driveRateSum;
			type = 0;

			for (int i = 0; i < m_driveNum; i++)
			{
				pick -= driveTable[i].rate > 0 ? driveTable[i].rate : 0;

				if (pick < 0)
				{
					type = i;
					break;
				}
			}
		}
	}

	m_selectType = type;
	m_pPuppet = 0;
	SetCamera(type, true);

	if (type >= 200)
	{
		return;
	}

	CheckCameraType(false);
	InitCtrlPoints(m_type);
}

void CCameraWork::SetCaddieCamera(const WMatrix& matrix, const WVector& target)
{
	m_bLock = false;
	m_bScript = false;
	m_fov = 55.0f * g_DEGTORAD;
	m_inputMatrix = matrix;
	SetCamera(0xCA, true);

	m_inputMatrix.GetRotMatrix(&m_caddieRot);
	WVector dir;
	dir = WVector::UNIT_POS_Z * m_caddieRot;
	WVector toTarget;
	toTarget = target - GolfBall().m_pos;
	m_angle = CalcDeltaAngle(toTarget, dir);
	m_src[0] = toTarget * 0.5f + GolfBall().m_pos + WVector(0.0f, 10.0f, 0.0f);
	m_srcTrans = m_src[0];
	m_destTrans = matrix.pivot;
	m_matrix = m_inputMatrix;
}

float CCameraWork::LoadCamera(eCamMode mode, char* name, const WVector& target)
{
	m_bScript = true;
	m_bLock = false;
	m_scriptPos = target;

	m_pPivot = &m_scriptPos;
	m_roll = 0.0f;

	if (mode == 0 || mode == 2)
	{
		CPartTidList& tidList =
			GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);

		m_pPuppet = FindHoleInCam(tidList.m_charTid);
	}
	else if (mode == 1)
	{
		if (m_pRankPuppet == 0)
		{
			switch (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType)
			{
			case 0:
			case 3:
			case 7:
				m_pRankPuppet = g_resrcmng->GetPuppet("rank_stroke.apet", false,
					true, false);
				break;
			case 1:
				m_pRankPuppet = g_resrcmng->GetPuppet("rank_match.apet", false,
					true, false);
				break;
			}
		}

		m_pPuppet = m_pRankPuppet;
	}
	else if (mode == 3)
	{
		m_pPuppet = FindHoleInCam(
			GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer).m_charTid);

		char* motion = 0;

		rand();

		AfxGetTask()->GetActor("Player")
			<< MsgObject(0, 0x2715, (intptr_t)&motion, 0, 0, 0, 0);
		name = MakeStr("%s_%02d", motion, 1);
	}
	else
	{
		m_pPuppet = 0;
	}

	if (m_pPuppet)
	{
		m_pMotion = m_pPuppet->FindMotionData(name);
	}
	else
	{
		m_pMotion = 0;
	}

	InitVars();

	if (m_pMotion)
	{
		m_time = m_pMotion->s;
		float length = m_pMotion->e - m_pMotion->s;

		for (w_motion_data* next =
				 m_pPuppet->FindMotionData(m_pMotion->nextmotion);
			next; next = m_pPuppet->FindMotionData(next->nextmotion))
		{
			if (Wabs(next->e - next->s) < g_EPSILON)
			{
				break;
			}

			length += next->e - next->s;

			if ((mode == 0 || mode == 203) && length > 10.0f)
			{
				length = 10.0f;
				break;
			}
		}

		m_finishTime = length + m_pMotion->s;
		return length;
	}

	m_finishTime = -1.0f;
	return -1.0f;
}

WVector CCameraWork::GetSourceCamera(void)
{
	return m_src[0];
}

WVector CCameraWork::GetDestCamera(void)
{
	return m_dest;
}

void CCameraWork::SetDestCamera(const WVector& dest)
{
	m_dest = dest;
}

void CCameraWork::SetInstalledCamera(const WVector& source)
{
	SetCamera(0x15, false);
	m_src[0] = source;

	m_bInstalled = true;
}

void CCameraWork::UninstallCamera(void)
{
	m_bInstalled = false;
}

void CCameraWork::SetCamera(int type, bool immediately)
{
	if (m_bInstalled)
	{
		return;
	}

	WVector dir;
	CGolfBall& ball = GolfBall();

	int lastIndex = ball.m_renderNum;
	int midIndex = ball.m_topFrame > 0 ? ball.m_topFrame : 0;
	int landIndex = ball.m_bounceFrame > 0 ? ball.m_bounceFrame : 0;

	m_type = type;
	m_hideFrame = -1;

	int replayCount = m_replayList.size();

	if (m_replayIndex >= 0 && m_replayIndex < replayCount)
	{
		int& replay = m_replayList[m_replayIndex++];
		m_type = replay;
	}
	else
	{
		m_replayIndex = -1;
		m_replayList.push_back(m_type);
	}

	if (!m_bLock)
	{
		m_input.Reset();
	}

	if (immediately)
	{
		m_src[0] = m_inputMatrix.pivot;
		m_dest = m_inputMatrix.pivot + m_inputMatrix.za;
		m_matrix = GetCameraMatrix(m_src[0], m_dest, m_roll);

		InitVars();
	}

	if (m_type < 100 && immediately)
	{
		m_startFrame = 75.0f;
	}
	else
	{
		m_startFrame = -1.0f;
	}

	m_roll = 0.0f;
	m_latchedFrame = -2;
	if (m_type < 100 && immediately && GolfBall().m_renderNum < 50)
	{
		m_pfnCamera = NULL;
		return;
	}

	switch (m_type)
	{
	case 1:
		m_startFrame = -1.0f;
		m_pfnCamera = &CCameraWork::LookBackCamera;
		break;
	case 2:
		m_pfnCamera = &CCameraWork::Rail_or_SideCamera;
		break;
	case 3:
	{
		dir = WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f),
			ball.PreData(midIndex)->vel);
		dir = dir.Normalize() * 10.0f;
		m_srcPos = WVector(0.0f, (float)(m_rand % 11) - 5.0f, 0.0f) +
			ball.PreData(midIndex)->pos + dir;
		m_state = m_rand % 2 + 1;
		m_pfnCamera = &CCameraWork::PanCamera;
		break;
	}
	case 4:
	case 11:
	{
		dir = ball.PreData(lastIndex)->pos - ball.PreData(landIndex)->pos;

		if (WisZero(dir.x, 1e-5f) && WisZero(dir.z, 1e-5f))
		{
			dir = ball.PreData(lastIndex)->pos - ball.PreData(0)->pos;

			dir.y = 0.0f;
			dir.Normalize();
		}
		else
		{
			dir.y = 0.0f;
			dir = RotVec(dir.Normalize(),
				RotMat(
					Random(-120.0f * g_DEGTORAD, 120.0f * g_DEGTORAD, m_rand),
					1));
		}

		if (ball.m_bOB)
		{
			m_srcPos = dir * 50.0f + ball.PreData(lastIndex)->pos +
				WVector(0.0f, 25.0f, 0.0f);
		}
		else
		{
			m_srcPos = dir * 20.0f + ball.PreData(lastIndex)->pos +
				WVector(0.0f, 10.0f, 0.0f);
		}

		if (m_type == 4)
		{
			m_pfnCamera = &CCameraWork::DropCamera;
		}
		else
		{
			m_pfnCamera = &CCameraWork::Drop_and_ZoomCamera;
		}

		break;
	}
	case 5:
		if (GolfClub().GetType() == 3)
		{
			m_startFrame = 25.0f;
		}

		if (m_frame > 10)
		{
			WVector dir;

			m_dest = ball.PreData(m_frame - 10)->pos;
			dir = ball.PreData(m_frame - 10)->pos -
				ball.PreData(m_frame - 9)->pos;
			dir.y = 0.0f;
			dir.Normalize();
			dir.y = 0.5f;
			dir.Normalize();

			m_src[0] = dir * 40.0f + ball.PreData(m_frame)->pos;
		}

		m_fov = 55.0f * g_DEGTORAD;
		m_pfnCamera = &CCameraWork::Chase_and_FollowCamera;
		break;
	case 7:
		m_startFrame = -1.0f;
		m_pfnCamera = &CCameraWork::CircleCamera;
		break;
	case 8:
		m_pfnCamera = &CCameraWork::Chase_and_LookBackCamera;
		break;
	case 9:
	{
		m_startFrame = -1.0f;
		dir = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->vel);
		dir = dir.Normalize() * 40.0f;

		dir.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->vel;
		m_src[0] = dir + ball.PreData(0)->pos +
			vel.Normalize() * ((float)(m_rand % 11) - 5.0f);
		m_dest = ball.PreData(0)->pos;
		m_dest.y += 2.0f;
		m_pfnCamera = &CCameraWork::ZoomInCamera;
		break;
	}
	case 10:
		m_pfnCamera = &CCameraWork::CaddyViewCamera;
		break;
	case 18:
	{
		m_startFrame = -1.0f;
		dir = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->vel);
		dir = dir.Normalize() * 40.0f;

		dir.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->vel;
		m_src[0] = dir + ball.PreData(0)->pos +
			vel.Normalize() * ((float)(m_rand % 11) - 5.0f);
		m_dest = ball.PreData(0)->pos;
		m_dest.y += 2.0f;
		m_angle = 15.0f * g_DEGTORAD;
		m_pfnCamera = &CCameraWork::FollowWithSwingCamera;
		break;
	}
	case 19:
		m_startFrame = -1.0f;
		m_time = 0.0f;
		m_angle = 0.0f;
		m_pfnCamera = &CCameraWork::SidePanWithZoomInCamera;
		break;
	case 12:
	{
		m_startFrame = -1.0f;
		dir = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->vel);
		dir = dir.Normalize() * 40.0f;

		dir.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->vel;
		m_src[0] = dir + ball.PreData(0)->pos +
			vel.Normalize() * ((float)(m_rand % 11) - 5.0f);
		m_dest = ball.PreData(0)->pos;
		m_dest.y += 2.0f;
		m_pfnCamera = &CCameraWork::BezierCamera;
		break;
	}
	case 13:
	{
		m_startFrame = -1.0f;
		dir = WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f),
			ball.PreData(0)->vel);

		dir.y = 0.0f;
		dir = dir.Normalize() * 30.0f;

		m_src[0] = ball.PreData(0)->pos +
			(ball.PreData(lastIndex)->pos - ball.PreData(0)->pos) * 0.5f + dir;
		m_src[0].y = ball.PreData(midIndex)->pos.y + 2.0f;

		m_pfnCamera = &CCameraWork::FromMidPointCamera;
		break;
	}
	case 14:
	{
		m_startFrame = -1.0f;
		dir =
			(ball.PreData(m_colFrame - 1)->pos - ball.PreData(m_colFrame)->pos)
				.Normalize() +
			(ball.PreData(m_colFrame + 1)->pos - ball.PreData(m_colFrame)->pos)
				.Normalize();

		dir.y = 0.0f;
		dir = dir.Normalize() * 160.0f;
		m_src[0] = dir + ball.PreData(m_colFrame)->pos;
		m_src[0].y += 96.0f;
		m_pfnCamera = &CCameraWork::FromColPointCamera;
		break;
	}
	case 20:
	{
		WPuppet* pPuppet = CSceneManager::Instance()->GetPuppet(
			MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer));

		float ratio = 0.3f;
		m_dest = (1.0f - ratio) *
				pPuppet->FindBone("Bip01 Head")->GetMatrix().pivot +
			ratio * pPuppet->FindBone("Bip01 HeadNub")->GetMatrix().pivot;
		m_src[0] = m_dest -
			RotMat(GOLFDOC()->GetCameraData().angle + 20.0f * g_DEGTORAD, 1)
					.za *
				3.0f;
		m_src[0].y += 0.5f;
		m_startFrame = -1.0f;
		m_pfnCamera = &CCameraWork::PangyaACamera;
		break;
	}
	case 21:
		m_angle = 90.0f * g_DEGTORAD;
		m_dest = ball.PreData(0)->pos + WVector(0.0f, 3.0f, 0.0f);
		m_src[0] = ball.PreData(0)->pos -
			RotMat(GOLFDOC()->GetCameraData().angle + m_angle, 1).za * 15.0f +
			WVector(0.0f, 3.0f, 0.0f);
		m_pfnCamera = &CCameraWork::FixedAtSideCamera;
		break;
	case 22:
		m_dest = GolfBall().PreData(0)->pos + WVector(0.0f, 3.0f, 0.0f);
		m_src[0] = ball.PreData(0)->pos +
			RotMat(GOLFDOC()->GetCameraData().angle +
					Random(-90.0f * g_DEGTORAD, 90.0f * g_DEGTORAD, m_rand),
				1)
					.za *
				30.0f +
			WVector(0.0f, 10.0f, 0.0f);
		m_pfnCamera = &CCameraWork::FromRandomPointCamera;

		break;
	case 23:
		m_startFrame = 0.0f;
		m_pfnCamera = &CCameraWork::BankFromHeadCamera;
		break;
	case 24:
		if (m_replayCount != 0 || m_pPuppet != NULL)
		{
			m_startFrame = -1.0f;
		}
		else
		{
			m_startFrame = 25.0f;

			WMatrix rot = RotMat(GOLFDOC()->GetCameraData().angle, 1);

			m_src[0] = ball.PreData(0)->pos +
				RotVec(WVector(10.0f, 0.0f, -20.0f), rot);
			m_dest =
				ball.PreData(0)->pos + RotVec(WVector(0.0f, 2.0f, -2.0f), rot);
		}

		m_pfnCamera = &CCameraWork::CobraACamera;
		break;
	case 25:
		if (m_replayCount != 0 || m_pPuppet != NULL)
		{
			m_startFrame = -1.0f;
		}
		else
		{
			m_startFrame = 25.0f;

			m_frame = GolfBall().m_playFrame;

			if (m_frame < GolfBall().m_cobraFrame)
			{
				m_src[0] = GOLFDOC()->GetCameraData().mat.pivot;
				m_dest = m_src[0] + GOLFDOC()->GetCameraData().mat.za;
			}
			else
			{
				m_roll = 15.0f * g_DEGTORAD -
					(float)(m_frame - GolfBall().m_cobraFrame) *
						(0.1f * g_DEGTORAD);

				m_src[0] =
					GolfBall()
						.PreData((GolfBall().m_cobraFrame + m_frame * 4) / 5)
						->pos;
				m_src[0].y += 1.0f;

				m_dest = GolfBall().m_pos;
			}
		}

		m_pfnCamera = &CCameraWork::CobraBCamera;
		break;
	case 26:
		if (m_replayCount != 0 || m_pPuppet != NULL)
		{
			m_startFrame = -1.0f;
		}
		else
		{
			m_startFrame = 25.0f;
		}

		m_pfnCamera = &CCameraWork::TomahawkACamera;
		break;
	case 27:
	{
		m_startFrame = 25.0f;
		dir = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->vel);
		dir = dir.Normalize() * 40.0f;

		dir.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->vel;
		m_src[0] = dir + ball.PreData(0)->pos +
			vel.Normalize() * ((float)(m_rand % 11) - 5.0f);
		m_dest = ball.PreData(0)->pos;
		m_dest.y += 2.0f;

		m_pfnCamera = &CCameraWork::SpikeACamera;
		break;
	}
	case 28:
	{
		float dist = Between(100.0f,
			ball.PreData(m_frame)->vel.Magnitude() * 2.0f, 150.0f);

		if (m_frame > 10)
		{
			WVector dir;

			m_dest = ball.PreData(m_frame - 10)->pos;
			dir = ball.PreData(m_frame - 10)->pos -
				ball.PreData(m_frame - 9)->pos;
			dir.y = 0.0f;
			dir.Normalize();
			dir.y = 2.0f;
			dir.Normalize();

			m_src[0] = dist * dir + ball.PreData(m_frame)->pos;
		}

		m_fov = 55.0f * g_DEGTORAD;
		m_pfnCamera = &CCameraWork::TopViewCamera;
		break;
	}
	case 101:
	{
		dir = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(0)->vel);
		dir.Normalize();
		m_src[0] =
			ball.PreData(0)->pos + dir * 16.0f + WVector(0.0f, 32.0f, 0.0f);
		m_dest = ball.PreData(0)->pos;
		m_dest.y += 2.5f;
		m_pfnCamera = &CCameraWork::SideCamera;
		break;
	}
	case 102:
	{
		dir = ball.PreData(0)->vel;
		m_src[0] = ball.PreData(0)->pos + WVector(0.0f, 25.0f, 0.0f) -
			dir.Normalize() * 5.0f;
		m_dest = ball.PreData(0)->pos;
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	}
	case 103:
	{
		dir = (ball.PreData(0)->pos - GetHoleCupPos()).Normalize();

		m_src[0] = GetHoleCupPos() + WVector(0.0f, 25.0f, 0.0f) -
			(dir +
				WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f),
					dir)
					.Normalize()) *
				3.0f;
		m_dest = ball.PreData(0)->pos;
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	}
	case 104:
	{
		dir = (ball.PreData(0)->pos - ball.PreData(ball.m_renderNum)->pos)
				  .Normalize();
		m_src[0] =
			WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f), dir)
					.Normalize() *
				3.0f +
			ball.PreData(ball.m_renderNum / 2)->pos;
		m_dest = ball.PreData(0)->pos;
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	}
	case 105:
		m_pfnCamera = &CCameraWork::BallSideCamera;
		break;
	case 106:
		m_dest = ball.PreData(0)->pos + WVector(0.0f, 3.0f, 0.0f);
		m_src[0] = m_dest -
			RotMat(GOLFDOC()->GetCameraData().angle + 100.0f * g_DEGTORAD, 1)
					.za *
				16.0f;
		m_pfnCamera = &CCameraWork::FollowBallCamera;
		m_bCheckGround = false;
		break;
	case 107:
	{
		dir = GetHoleCupPos() - ball.PreData(0)->pos;

		dir.y = 0.0f;
		m_src[0] = ball.PreData(0)->pos +
			dir.Normalize() * 20.0f * RotMat(-150.0f * g_DEGTORAD, 1);
		m_src[0].y += 20.0f;
		m_dest = (ball.PreData(0)->pos + GetHoleCupPos()) * 0.5f;
		m_pfnCamera = &CCameraWork::FixedCamera;
		break;
	}
	case 108:
	{
		dir = GetHoleCupPos() - ball.PreData(0)->pos;

		dir.y = 0.0f;
		m_src[0] = GetHoleCupPos() +
			dir.Normalize() * 20.0f * RotMat(-60.0f * g_DEGTORAD, 1);
		m_src[0].y += 20.0f;
		m_dest = (ball.PreData(0)->pos + GetHoleCupPos()) * 0.5f;
		m_pfnCamera = &CCameraWork::FixedCamera;
		break;
	}
	case 109:
	{
		dir = GetHoleCupPos() - ball.PreData(0)->pos;

		dir.y = 0.0f;
		m_src[0] = GetHoleCupPos() +
			dir.Normalize() * 10.0f * RotMat(-90.0f * g_DEGTORAD, 1);
		m_src[0].y += 20.0f;
		m_dest = ball.m_pos;
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	}
	case 110:
	{
		dir = ball.m_vel;
		m_src[0] = GetHoleCupPos() - dir.Normalize() * 10.0f;
		m_src[0].y += 10.0f;
		m_dest = ball.m_pos;
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	}
	case 100:
		m_pfnCamera = &CCameraWork::JustLookingCamera;
		break;
	case 111:
	{
		dir = GetHoleCupPos() - ball.m_pos;

		dir.y = 0.0f;
		m_src[0] = GetHoleCupPos() +
			dir.Normalize() * 15.0f * RotMat(-20.0f * g_DEGTORAD, 1);
		m_src[0].y += 20.0f;
		m_dest = ball.m_pos;
		m_pfnCamera = &CCameraWork::PuttSecondACamera;
		break;
	}
	case 112:
	{
		dir = GetHoleCupPos() - ball.PreData(0)->pos;

		dir.y = 0.0f;
		dir.Normalize();
		m_src[0] =
			GetHoleCupPos() + dir * 60.0f * RotMat(-3.0f * g_DEGTORAD, 1);
		m_dest = ball.m_pos;
		m_fov = 10.0f * g_DEGTORAD;
		m_pfnCamera = &CCameraWork::PuttSecondBCamera;
		break;
	}
	case 150:
		m_src[0] = GetHoleCupPos() - WVector(0.0f, 2.0f, 0.0f);
		m_dest = m_src[0] + WVector(0.0f, 1.0f, 0.2f);
		m_pfnCamera = &CCameraWork::FromHoleCupCamera;
		break;
	case 205:
		m_matrix = GOLFDOC()->GetDefaultCamera();
		m_src[0] = m_matrix.pivot;
		m_dest = m_src[0] + m_matrix.za;
		m_fov = 1.0f * g_DEGTORAD;
		m_finishTime = 1.0f;
		m_time = 0.0f;
		m_startFrame = -1.0f;
		m_pfnCamera = &CCameraWork::ZoomOutCamera;
		break;
	case 203:
		if ((IsMassGame() &&
				GOLFDOC()->m_currentHole !=
					WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) ||
			WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 10)
		{
			m_src[0] = GolfBall().m_pos -
				RotMat(GOLFDOC()->GetCameraData().angle, 1).za * 25.0f;
			m_src[0].y += 5.0f;
			m_dest = m_src[0] + RotMat(GOLFDOC()->GetCameraData().angle, 1).za;
			m_matrix = GetCameraMatrix(m_src[0], m_dest, 0.0f);

			SetCamera(204, false);

			break;
		}
		else
		{
			float time = LoadCamera((eCamMode)3, NULL, GetTeeShotPos());

			m_bCheckGround = false;
			m_startFrame = -1.0f;
			if (time > 0.0f)
			{
				m_pfnCamera = NULL;
			}
			else
			{
				m_bScript = false;

				m_pfnCamera = &CCameraWork::IntroACamera;

				m_angle = 150.0f * g_DEGTORAD;
				m_distance = 80.0f;
				m_height = 15.0f;
			}
		}
		break;
	case 204:
		m_srcRot = m_matrix;
		m_destRot = GOLFDOC()->GetDefaultCamera();
		m_srcTrans = m_matrix.pivot;
		m_destTrans = GOLFDOC()->GetCameraData().mat.pivot;
		m_time = 0.0f;
		m_finishTime = 1.0f;
		m_startFrame = -1.0f;

		m_pfnCamera = &CCameraWork::ToDefaultCamera;

		break;
	case 200:
		m_pfnCamera = &CCameraWork::FirstPersonCamera;
		break;
	case 201:
		m_pfnCamera = &CCameraWork::StillFrameCamera;
		break;
	case 202:
		m_startFrame = -1.0f;
		m_pfnCamera = &CCameraWork::CaddieChanceCamera;
		break;
	case 206:
		m_startFrame = -1.0f;
		switch (m_replayCount)
		{
		case 1:
			m_src[0] = GolfBall().m_pos +
				(GOLFDOC()->GetCameraData().mat.ya +
					GOLFDOC()->GetCameraData().mat.za) *
					32.0f * RotMat(120.0f * g_DEGTORAD, 1);
			m_dest = GolfBall().m_pos;
			break;

		case 2:
			m_src[0] = GolfBall().m_pos +
				(GOLFDOC()->GetCameraData().mat.ya +
					GOLFDOC()->GetCameraData().mat.za) *
					32.0f * RotMat(-110.0f * g_DEGTORAD, 1);
			m_dest = GolfBall().m_pos + WVector(0.0f, 2.0f, 0.0f);
			break;
		}

		m_pfnCamera = NULL;

		break;
	case 207:
		m_startFrame = -1.0f;
		switch (m_replayCount)
		{
		case 1:
			m_srcTrans =
				WVector(-5.92f, -4.61f, 37.5f) * GOLFDOC()->GetCameraData().mat;
			m_srcRot = MakeCameraMatrix(m_srcTrans,
				m_srcTrans + WVector(0.21f, 0.3f, -0.93f));

			m_destTrans =
				WVector(0.99f, -5.4f, 37.78f) * GOLFDOC()->GetCameraData().mat;
			m_destRot = MakeCameraMatrix(m_destTrans,
				m_destTrans + WVector(-0.3f, 0.41f, -0.86f));
			break;

		case 2:
			m_srcTrans = WVector(-10.28f, -0.51f, 17.76f) *
				GOLFDOC()->GetCameraData().mat;
			m_srcRot = MakeCameraMatrix(m_srcTrans,
				m_srcTrans + WVector(0.93f, 0.09f, 0.35f));

			m_destTrans =
				WVector(-6.45f, 0.62f, 13.05f) * GOLFDOC()->GetCameraData().mat;
			m_destRot = MakeCameraMatrix(m_destTrans,
				m_destTrans + WVector(0.1f, -0.07f, 0.99f));
			break;
		}

		m_finishTime = 0.5f;

		m_pfnCamera = &CCameraWork::SwingReplayBCamera;

		break;
	case 208:
		m_startFrame = -1.0f;
		m_dest = GetHoleCupPos();
		m_dest.y += 3.0f;
		m_angle = 0.0f;
		m_distance = 64.0f;
		m_pfnCamera = &CCameraWork::ExtraTimeCamera;
		m_manualType = 0;
		break;
	case 0:
		m_pfnCamera = &CCameraWork::FollowCamera;
		break;
	default:
		m_pfnCamera = &CCameraWork::FollowCamera;
		break;
	}

	for (int i = 1; i < 3; i++)
	{
		m_src[i] = m_src[0];
	}
}

void CCameraWork::CheckCameraType(bool replay)
{
	if (!m_bCheckCamera)
	{
		return;
	}

	if (GolfClub().GetType() == 3)
	{
		if (GolfBall().m_groundType == 2)
		{
			float angle = CalcDeltaAngle(GetHoleCupPos() - GolfBall().m_pos,
				GolfBall().m_vel);
			if ((angle > 0.0f ? angle : -angle) > 45.0f * g_DEGTORAD)
			{
				SetCamera(100, true);
			}
		}
		else
		{
			int index = rand() % m_enableList.size();

			SetCamera(m_enableList[index], true);
		}

		return;
	}

	if (GOLFDOC()->GetBar().grade == 1)
	{
		if (GolfBall().m_renderNum == 0)
		{
			SetCamera(0x16, true);
		}
		else
		{
			switch (m_rand % 3)
			{
			case 0:
				SetCamera(100, true);
				break;
			case 1:
				SetCamera(0x15, true);
				break;
			case 2:
				SetCamera(0x16, true);
				break;
			}
		}

		return;
	}

	int powerFlags = GolfBall().m_initSpecial & 0x70;

	if (powerFlags)
	{
		const CGolfBall::sExpected* pLast =
			GolfBall().PreData(GolfBall().m_renderNum);
		WVector toCup = GetHoleCupPos() - pLast->pos;

		int chance = 1;
		if (replay)
		{
			chance = 5;
		}

		if (sqrtf(toCup.x * toCup.x + toCup.z * toCup.z) < 16.0f)
		{
			int r = rand();

			switch (powerFlags)
			{
			case 0x10:
				if (r % 100 < chance * 5 &&
					GolfBall().m_tomahawkFrame == GolfBall().m_bounceFrame)
				{
					SetCamera(0x1a, true);
				}
				break;
			case 0x20:
				if (r % 100 < chance * 10)
				{
					if (r % 10 < 5)
					{
						SetCamera(0x18, true);
					}
					else
					{
						SetCamera(0x19, true);
					}
				}
				break;
			case 0x40:
				if (r % 100 < chance * 5 &&
					GolfBall().m_spikeEndFrame == GolfBall().m_bounceFrame)
				{
					SetCamera(0x1b, true);
				}
				break;
			}
		}
	}

	if (m_contactFrame < 0.0f)
	{
		return;
	}

	if (m_type == 10)
	{
		if (!m_bCaddieChase)
		{
			SetCamera(0, true);
			return;
		}

		IActor* pCaddie = AfxGetTask()->GetActor("Caddie");
		if (pCaddie)
		{
			CGolfCaddie::sCaddie* pState;
			pCaddie << MsgObject(0, 0x1e0, (intptr_t)&pState, 0, 0, 0, 0);

			if (pState == 0 || !pState->m_caddieView)
			{
				SetCamera(0, true);
			}
		}
	}

	for (m_colFrame = 0; m_colFrame < float2int(m_lastFrame); m_colFrame++)
	{
		const CGolfBall::sExpected* pData = GolfBall().PreData(m_colFrame);

		if (pData && pData->event == 2)
		{
			break;
		}
	}

	if (m_colFrame < float2int(m_contactFrame))
	{
		m_state |= 0x20000000;

		if (m_colFrame - m_startFrame < 100.0f)
		{
			SetCamera(0xe, true);
		}
		else if (m_type == 4 || m_type == 0xb)
		{
			SetCamera(0xd, true);
		}
	}
	else if (m_contactFrame - m_startFrame < 100.0f ||
		m_topFrame - m_startFrame < 50.0f)
	{
		SetCamera(0xd, true);
	}
	else if (m_contactFrame - m_startFrame < 150.0f)
	{
		switch (m_rand % 2)
		{
		case 0:
			SetCamera(4, true);
			break;
		case 1:
			SetCamera(0xd, true);
			break;
		}
	}
}

void CCameraWork::InitCtrlPoints(int type)
{
	CGolfBall& ball = GolfBall();

	int midIndex = ball.m_topFrame > 0 ? ball.m_topFrame : 0;
	int landIndex = ball.m_bounceFrame > 0 ? ball.m_bounceFrame : 0;
	int spinIndex = ball.m_rollFrame > 0 ? ball.m_rollFrame : 0;
	int startIndex = Between(0, float2int(m_startFrame), ball.m_renderNum);

	if (midIndex == 0)
	{
		midIndex = ball.m_renderNum / 2;
	}

	if (spinIndex < midIndex)
	{
		midIndex = ball.m_renderNum / 3;
		spinIndex = ball.m_renderNum * 2 / 3;
	}

	m_lift = 5.0f;

	WVector dir;
	if (GolfBall().IsHoleIn() == 1)
	{
		dir = ball.PreData(ball.m_renderNum)->pos - ball.PreData(0)->pos;
	}
	else
	{
		dir = GetHoleCupPos() - ball.PreData(ball.m_renderNum)->pos;
	}

	dir.y = 0.0f;
	dir.Normalize();

	WVector vel;
	WVector side;
	switch (type)
	{
	case 0:
	{
		m_ctrlPoint[0] = m_inputMatrix.pivot;

		vel = ball.PreData(0)->vel * 0.5f;
		const CGolfBall::sExpected* pMid = ball.PreData(midIndex);
		if (pMid)
		{
			m_ctrlPoint[1] = vel + pMid->pos;
		}
		else
		{
			m_ctrlPoint[1] = WVector(vel.x, vel.y + 1.0f, vel.z);
		}

		side =
			WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f), vel)
				.Normalize() *
			30.0f;
		const CGolfBall::sExpected* pSpin = ball.PreData(spinIndex);
		if (pSpin)
		{
			m_ctrlPoint[2] = pSpin->pos + WVector(0.0f, 25.0f, 0.0f) + side;
		}
		else
		{
			m_ctrlPoint[2] = m_ctrlPoint[1] + WVector(0.0f, 25.0f, 0.0f) + side;
		}

		WVector back;
		back = ball.PreData(0)->pos - ball.PreData(ball.m_renderNum)->pos;
		back.y = 0.0f;

		if (WDotProduct(back, dir) > 0.0f)
		{
			dir = dir.Normalize() * 15.0f;
		}
		else
		{
			dir = dir.Normalize() * -15.0f;
		}
		dir.y += 5.0f;

		const CGolfBall::sExpected* pLast = ball.PreData(ball.m_renderNum);
		if (pLast)
		{
			m_ctrlPoint[3] = pLast->pos + dir * 5.0f;
		}
		else
		{
			m_ctrlPoint[3] = m_ctrlPoint[2] + dir * 5.0f;
		}
		break;
	}
	case 1:
	{
		vel = ball.PreData(startIndex)->vel;
		m_ctrlPoint[0] = vel * 0.8f + ball.PreData(startIndex)->pos;
		vel = ball.PreData(midIndex)->vel;
		m_ctrlPoint[1] = ball.PreData(midIndex)->pos +
			WVector(0.0f, 50.0f, 0.0f) + vel * 2.0f;
		vel = ball.PreData(spinIndex)->vel;
		m_ctrlPoint[2] =
			ball.PreData(spinIndex)->pos + WVector(0.0f, 10.0f, 0.0f) + vel;
		m_ctrlPoint[3] =
			ball.PreData(ball.m_renderNum)->pos + WVector(0.0f, 10.0f, 0.0f);
		break;
	}
	case 2:
	{
		vel = ball.PreData(0)->vel;
		side =
			WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f), vel)
				.Normalize() *
			25.0f;
		m_ctrlPoint[0] = side + ball.PreData(startIndex)->pos;
		vel = ball.PreData(midIndex)->vel;
		m_ctrlPoint[1] = ball.PreData(midIndex)->pos +
			WVector(0.0f, 50.0f, 0.0f) + vel * 1.2f + side;
		vel = ball.PreData(spinIndex)->vel;
		m_ctrlPoint[2] = vel * 1.5f + ball.PreData(spinIndex)->pos + side;
		if (ball.m_bOB)
		{
			m_ctrlPoint[3] = ball.PreData(ball.m_renderNum)->pos +
				WVector(0.0f, 30.0f, 0.0f) + side * 5.0f;
		}
		else
		{
			m_ctrlPoint[3] = ball.PreData(ball.m_renderNum)->pos +
				WVector(0.0f, 10.0f, 0.0f) + side;
		}
		break;
	}
	case 4:
	case 11:
		m_ctrlPoint[0] = ball.PreData(landIndex)->pos;
		m_ctrlPoint[1] =
			ball.PreData(landIndex -
					float2int((ball.m_renderNum - landIndex) * -0.5f))
				->pos;
		m_ctrlPoint[2] =
			ball.PreData(landIndex -
					float2int((ball.m_renderNum - landIndex) * -0.75f))
				->pos;
		m_ctrlPoint[3] = ball.PreData(float2int(m_lastFrame))->pos +
			WVector(0.0f, 3.0f, 0.0f);
		break;
	case 8:
	{
		m_ctrlPoint[0] = m_inputMatrix.pivot;
		vel = ball.PreData(0)->vel;
		side =
			WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f), vel)
				.Normalize() *
			30.0f;
		m_ctrlPoint[1] = ball.PreData(midIndex)->pos +
			WVector(0.0f, 25.0f, 0.0f) + vel * 2.5f + side;
		vel = ball.PreData(midIndex)->vel;
		m_ctrlPoint[2] = vel * 1.5f + ball.PreData(spinIndex)->pos + side;
		if (ball.m_bOB)
		{
			m_ctrlPoint[3] = ball.PreData(ball.m_renderNum)->pos +
				WVector(0.0f, 10.0f, 0.0f) + dir * 40.0f;
		}
		else
		{
			WVector back;
			back = ball.PreData(0)->pos - ball.PreData(ball.m_renderNum)->pos;
			back.y = 0.0f;
			if (WDotProduct(back, dir) > 0.0f)
			{
				m_ctrlPoint[3] = ball.PreData(ball.m_renderNum)->pos +
					WVector(0.0f, 10.0f, 0.0f) - dir * 40.0f;
			}
			else
			{
				m_ctrlPoint[3] = ball.PreData(ball.m_renderNum)->pos +
					WVector(0.0f, 10.0f, 0.0f) + dir * 40.0f;
			}
		}
		break;
	}
	case 12:
	{
		m_ctrlPoint[0] = m_src[0];
		m_ctrlPoint[1] = WVector((m_rand % 2 ? 1.0f : -1.0f) * 10.0f, 100.0f,
							 (m_rand % 2 ? 1.0f : -1.0f) * 10.0f) +
			ball.PreData(midIndex)->pos;
		side =
			WCrossProduct(WVector(0.0f, m_rand % 2 ? 1.0f : -1.0f, 0.0f), dir)
				.Normalize() *
			15.0f;
		m_ctrlPoint[2] = WVector((m_rand % 2 ? 1.0f : -1.0f) * 10.0f, 50.0f,
							 (m_rand % 2 ? 1.0f : -1.0f) * 10.0f) +
			ball.PreData(spinIndex)->pos + side;
		dir *= -15.0f;
		dir += side * 0.3f;
		dir.y += 3.0f;
		if (ball.m_bOB)
		{
			m_ctrlPoint[3] = dir * 5.0f + ball.PreData(ball.m_renderNum)->pos;
		}
		else
		{
			m_ctrlPoint[3] = dir * 3.0f + ball.PreData(ball.m_renderNum)->pos;
		}
		break;
	}
	case 18:
	{
		m_ctrlPoint[0] = m_src[0];
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(0)->vel)
				   .Normalize() *
			100.0f;
		m_ctrlPoint[1] =
			ball.PreData(midIndex)->pos + WVector(0.0f, 100.0f, 0.0f) - side;
		m_ctrlPoint[2] =
			ball.PreData(spinIndex)->pos + WVector(0.0f, 50.0f, 0.0f) + side;
		dir = side;
		dir = dir.Normalize() * 30.0f;
		dir.y += 15.0f;
		m_ctrlPoint[3] = dir + ball.PreData(ball.m_renderNum)->pos;
		break;
	}
	}

	WVector rest = ball.PreData(ball.m_renderNum)->pos - m_ctrlPoint[3];
	m_lift = rest.Magnitude() * 0.1f;
	m_lift = Between(1.0f, m_lift, 5.0f);
}

void CCameraWork::InitVars(void)
{
	m_topFrame = (float)GolfBall().m_topFrame;
	m_contactFrame = (float)GolfBall().m_bounceFrame;
	m_lastFrame = (float)GolfBall().m_renderNum;
	m_latchedFrame = -2;
	m_frame = 0;
	m_time = 0.0f;
	m_bPanSound = false;
	m_ballSoundState = 0;
	m_quakeStepTime = 0.0f;
	m_soundTime = 0.0f;
	m_finishTime = -1.0f;
	m_bCheckGround = true;
	m_lift = 5.0f;
	m_manualType = 2;

	if (m_rand % 6 == 0 && GetPVS().GetObjColNum() == 0 &&
		GolfClub().GetType() != 3 &&
		GetPVS().GetGroundType(GolfBall().PreData(GolfBall().m_renderNum)->pos,
			true, 0) == 2)
	{
		m_bGreenFov = true;
	}
	else
	{
		m_bGreenFov = false;
	}

	m_bReserved1a0 = false;

	m_bPassHole = false;
	m_state = 1;
	m_oldBallPos = GolfBall().m_pos;

	g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
}

void CCameraWork::InputProcess(float elapsed)
{
	if (m_manualType == 0)
	{
		g_camera = GetCameraMatrix(m_src[0], m_dest, m_roll);
		return;
	}

	if (g_input->GetDown("\xbb\xf3", true) ||
		g_input->GetDown("\xc7\xcf", true) ||
		g_input->GetDown("\xc1\xc2", true) ||
		g_input->GetDown("\xbf\xec", true))
	{
		m_input.bActive = true;
	}

	if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))
	{
		if (g_input->Get("\xbb\xf3", true))
		{
			m_input.fov -= elapsed * (30.0f * g_DEGTORAD);
		}

		if (g_input->Get("\xc7\xcf", true))
		{
			m_input.fov += elapsed * (30.0f * g_DEGTORAD);
		}
	}

	if (!g_input->Get("\xbb\xf3", true) && !g_input->Get("\xc7\xcf", true) &&
		!(Wabs(m_input.fov) < g_EPSILON) && m_manualType == 2)
	{
		if (m_input.fov > 0.0f)
		{
			m_input.fov -= elapsed * (60.0f * g_DEGTORAD);
			if (m_input.fov < 0.0f)
			{
				m_input.fov = 0.0f;
			}
		}
		else
		{
			m_input.fov += elapsed * (60.0f * g_DEGTORAD);
			if (m_input.fov > 0.0f)
			{
				m_input.fov = 0.0f;
			}
		}
	}

	float speed;

	if (m_manualType == 2)
	{
		speed = 30.0f * g_DEGTORAD;
	}
	else
	{
		speed = 15.0f * g_DEGTORAD;
	}

	if (!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))
	{
		if (g_input->Get("\xbb\xf3", true))
		{
			if (m_manualType == 1 || !m_bPitchLimit)
			{
				m_input.rot.y += speed * elapsed;
			}
		}

		if (g_input->Get("\xc7\xcf", true))
		{
			m_input.rot.y -= speed * elapsed;
		}
	}

	if (!g_input->Get("\xbb\xf3", true) && !g_input->Get("\xc7\xcf", true) &&
		!(Wabs(m_input.rot.y) < g_EPSILON) && m_manualType == 2)
	{
		if (m_input.rot.y > 0.0f)
		{
			m_input.rot.y -= speed * elapsed * 2.0f;
			if (m_input.rot.y < 0.0f)
			{
				m_input.rot.y = 0.0f;
			}
		}
		else
		{
			m_input.rot.y += speed * elapsed * 2.0f;
			if (m_input.rot.y > 0.0f)
			{
				m_input.rot.y = 0.0f;
			}
		}
	}

	if (m_manualType == 2)
	{
		speed = 90.0f * g_DEGTORAD;
	}
	else
	{
		speed = 30.0f * g_DEGTORAD;
	}

	if (g_input->Get("\xc1\xc2", true))
	{
		m_input.rot.x -= speed * elapsed;
	}

	if (g_input->Get("\xbf\xec", true))
	{
		m_input.rot.x += speed * elapsed;
	}

	if (!g_input->Get("\xc1\xc2", true) && !g_input->Get("\xbf\xec", true) &&
		!(Wabs(m_input.rot.x) < g_EPSILON) && m_manualType == 2)
	{
		if (m_input.rot.x > 0.0f)
		{
			m_input.rot.x -= speed * elapsed * 2.0f;
			if (m_input.rot.x < 0.0f)
			{
				m_input.rot.x = 0.0f;
			}
		}
		else
		{
			m_input.rot.x += speed * elapsed * 2.0f;
			if (m_input.rot.x > 0.0f)
			{
				m_input.rot.x = 0.0f;
			}
		}
	}

	if (m_manualType != 2)
	{
		m_input.rot.x =
			Between(-60.0f * g_DEGTORAD, m_input.rot.x, 60.0f * g_DEGTORAD);
	}
	m_input.rot.y =
		Between(-30.0f * g_DEGTORAD, m_input.rot.y, 30.0f * g_DEGTORAD);

	if (!m_input.bActive)
	{
		m_input.fov = 0.0f;
		m_input.rot.y = 0.0f;
		m_input.rot.x = 0.0f;
	}

	if (m_bLock)
	{
		float fov = m_input.fov + 55.0f * g_DEGTORAD;
		g_view->SetFOV(fov < 10.0f / g_RADTODEG
				? 10.0f / g_RADTODEG
				: (fov > g_PI_DIV_2 ? 90.0f * g_DEGTORAD : fov));

		if (Wabs(m_input.rot.x) < g_EPSILON && Wabs(m_input.rot.y) < g_EPSILON)
		{
			g_camera = m_matrix;
			return;
		}

		g_camera = GetCameraMatrix(m_matrix.pivot,
			WVector(sinf(m_input.rot.x) * cosf(m_input.rot.y),
				sinf(m_input.rot.y),
				cosf(m_input.rot.x) * cosf(m_input.rot.y)) *
				m_inputMatrix,
			m_roll);
		return;
	}

	float fov = m_fov + m_input.fov;
	g_view->SetFOV(fov < 10.0f / g_RADTODEG
			? 10.0f / g_RADTODEG
			: (fov > g_PI_DIV_2 ? 90.0f * g_DEGTORAD : fov));

	m_matrix = GetCameraMatrix(m_src[0], m_dest, m_roll);

	if (Wabs(m_input.rot.x) < g_EPSILON && Wabs(m_input.rot.y) < g_EPSILON)
	{
		g_camera = m_matrix;
	}
	else if (m_manualType == 2)
	{
		WVector dir = m_matrix.pivot - *m_pPivot;
		float distance = Max(10.0f,
			(1.0f - Abs(m_input.rot.x) * 0.47746483f) * dir.Magnitude());

		dir.Normalize();
		dir *= distance;

		const WVector& pivot = *m_pPivot;
		WVector source = dir * RotMat(m_matrix.xa, m_input.rot.y) *
				RotMat(m_input.rot.x, 1) +
			pivot;
		WVector dest = m_matrix.za * RotMat(m_matrix.xa, m_input.rot.y) *
				RotMat(m_input.rot.x, 1) +
			source;

		g_camera = GetCameraMatrix(source, dest, m_roll);
	}
	else
	{
		g_camera = GetCameraMatrix(m_matrix.pivot,
			WVector(sinf(m_input.rot.x) * cosf(m_input.rot.y),
				sinf(m_input.rot.y),
				cosf(m_input.rot.x) * cosf(m_input.rot.y)) *
				m_matrix,
			m_roll);
	}

	if (GolfBall().m_playFrame > 0)
	{
		ProcessBallSound();
	}
}

void CCameraWork::Process(float elapsed, float time)
{
	for (int i = 2; i > 0; i--)
	{
		m_src[i] = m_src[i - 1];
	}

	m_frame = GolfBall().m_playFrame;
	m_curFrame = (float)m_frame;
	const float prevTime = m_time;
	bool scripted = false;
	WVector up(0.0f, 1.0f, 0.0f);
	m_time += elapsed;

	if (m_bScript)
	{
		static WMatrix s_toCamera(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 0.0f);
		static bool s_targetSent = false;

		if (m_pMotion == 0)
		{
			return;
		}

		if (m_time > m_pMotion->e)
		{
			ScriptProcess(m_pMotion, prevTime, m_pMotion->e);

			float over = m_time - m_pMotion->e;
			m_pMotion = m_pPuppet->FindMotionData(m_pMotion->nextmotion);

			if (m_pMotion)
			{
				if (Wabs(m_pMotion->e - m_pMotion->s) < g_EPSILON)
				{
					m_time = m_pMotion->s;
					m_finishTime = m_time - 1.0f;
				}
				else
				{
					m_time = over + m_pMotion->s;
				}
				ScriptProcess(m_pMotion, m_pMotion->s, m_time);
			}
		}
		else
		{
			ScriptProcess(m_pMotion, prevTime, m_time);
		}

		if (m_pMotion)
		{
			m_pPuppet->ApplyBones(m_pPuppet->GetBones(0, m_time, 0, 0, false));
			m_pPuppet->Transform(0, 1.0f, 0);

			WBone* pBone = m_pPuppet->FindBone(m_pMotion->applybone);
			float yaw;
			WVector offset;
			WVector dir(0.0f, 0.0f, 1.0f);
			if (pBone == 0)
			{
				pBone = m_pPuppet->GetRootBone();
			}

			if (pBone)
			{
				AfxGetTask()->GetActor("Player")
					<< MsgObject(0, 0x110, (intptr_t)&yaw, 0, 0, 0, 0);
				AfxGetTask()->GetActor("Player")
					<< MsgObject(0, 0x128, (intptr_t)&offset, 0, 0, 0, 0);

				WMatrix mat;
				mat = pBone->GetMatrix();

				if (!(Wabs(offset.x) < g_EPSILON) ||
					!(Wabs(offset.y) < g_EPSILON) ||
					!(Wabs(offset.z) < g_EPSILON))
				{
					if (!m_bAbsAnim)
					{
						mat = mat * RotMat(yaw, 1);
					}
				}

				WMatrix rot;
				mat.GetRotMatrix(&rot);
				rot = s_toCamera * rot;

				WVector pos = mat.pivot;
				if (m_type == 0xcb)
				{
					WMatrix turn(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
						1.0f);
					turn.Rotate(g_PI + yaw, 1);

					rot = rot * turn;
					pos = pos * turn;

					if (s_targetSent)
					{
						AfxGetTask()->GetActor("Player") << MsgObject(0, 0x127,
							(intptr_t)&m_scriptPos, 1, 0, 0, 0);
					}

					if (!s_targetSent)
					{
						s_targetSent = true;
					}
				}
				else if (strcmpi(m_pPuppet->GetPuppetName(),
							 "i_def_cam.apet") == 0)
				{
					if (strstr(m_pMotion->name,
							"\xbf\xec\xb5\xe5\xc6\xc4\xbf\xf6\xbc\xa6"
							"2"))
					{
						static WMatrix s_turnAround(-1.0f, 0.0f, 0.0f, 0.0f,
							1.0f, 0.0f, 0.0f, 0.0f, -1.0f);
						rot = rot * s_turnAround;
						pos = pos * s_turnAround;
					}
				}

				dir = dir * rot;

				scripted = true;
				m_src[0] = offset = pos + m_scriptPos - offset;
				m_dest = dir = dir + m_src[0];
				up = rot.ya;
			}
		}

		if (m_type == 0xcb && m_finishTime > 0.0f && m_time > m_finishTime)
		{
			s_targetSent = false;
			m_bScript = false;
			SetCamera(0xcc, false);
		}
	}
	else
	{
		m_pMotion = 0;
		if (m_type < 0xca && CheckState())
		{
			m_oldBallPos = GolfBall().m_pos;
			InputProcess(elapsed);
			return;
		}

		if (m_nextType >= 0)
		{
			SetCamera(m_nextType, false);
			m_nextType = -1;
			InitCtrlPoints(m_type);
		}

		if (m_curFrame < m_startFrame)
		{
			m_latchedFrame = m_frame;
		}
		else
		{
			if (m_type == 3 || m_type == 4 || m_type == 11)
			{
				if (m_latchedFrame < (int)m_startFrame)
				{
					m_src[0] = m_srcPos;
				}
			}

			if (m_pfnCamera)
			{
				(this->*m_pfnCamera)(elapsed, time);
				CheckDriveCameraBallVisibilityAndSetBallVisibilitySafeCamera();
			}
		}

		if (m_state & 0x80000000)
		{
			if (m_time - m_quakeTime < 0.8f)
			{
				EarthQuake();
			}
			else
			{
				m_state &= 0x7fffffff;
			}
		}
	}

	if (m_bCheckGround)
	{
		CheckGround(m_src[0], m_dest);
	}

	if ((m_bGreenFov || WVectorLen(m_src[0] - GolfBall().m_pos) > 192.0f) &&
		m_frame > 0)
	{
		if (GolfBall().PreData(m_frame)->state == 3 && m_type != 0x19)
		{
			m_fov =
				Max(20.0f * g_DEGTORAD, m_fov - elapsed * (10.0f * g_DEGTORAD));
			m_dest.y -= (1.0f - m_fov * (1.0f / (55.0f * g_DEGTORAD))) * 5.0f;
		}
	}

	m_oldBallPos = GolfBall().m_pos;

	InputProcess(elapsed);

	if (!scripted)
	{
		return;
	}

	g_camera.ya = up;
	g_camera.xa = WCrossProduct(g_camera.ya, g_camera.za);
}

bool CCameraWork::CheckGround(WVector& pos, WVector& dest)
{
	float y = Max(pos.y,
		GetPVS().GetBasePosY(pos, GOLFDOC()->GetCameraData().clearance));

	if (m_bScript)
	{
		dest.y += y - pos.y;
	}

	if (y > pos.y)
	{
		pos.y = y;
		return true;
	}

	return false;
}

void CCameraWork::ScriptProcess(w_motion_data* motion, float from, float to)
{
	if (m_pPuppet == 0)
	{
		return;
	}

	if (!(from <= motion->e) || from == to)
	{
		return;
	}

	from *= 30.0f;
	to = Min(motion->e, to) * 30.0f;
	int first = Ceil(from, 0.03f);
	int last = to == (float)floor((double)to)
		? float2int(to) - 1
		: float2int((float)floor((double)to));

	if (first <= last)
	{
		int start, end;

		if (m_pPuppet->CheckFrameData(first, last, start, end))
		{
			for (int i = start; i <= end; i++)
			{
				ParsePuppetScript(m_pPuppet->GetFrameData(i));
			}
		}
	}
}

void CCameraWork::ParsePuppetScript(char* script)
{
	static const char* s_scriptKey[] = { "*" };

	if (!script)
	{
		return;
	}

	char* p = strchr(script, '*');

	while (p)
	{
		int len = 0;

		while (p[len])
		{
			if (p[++len] == ' ')
			{
				break;
			}

			if (p[len] == '(')
			{
				len++;
				break;
			}
		}

		if (len <= 1)
		{
			break;
		}

		strncpy(s_scriptCmd, p, len);
		s_scriptCmd[len] = 0;
		p += len;
		for (int i = 0; i < sizeof(s_scriptKey) / sizeof(s_scriptKey[0]); i++)
		{
			strcmpi(s_scriptCmd, s_scriptKey[i]);
		}
		p = strchr(p, '*');
	}
}

bool CCameraWork::CheckState(void)
{
	if (m_curFrame >= m_lastFrame)
	{
		return true;
	}

	if (GolfBall().m_holeIn == 1)
	{
		WVector dist = GOLFDOC()->GetHoleData().pin;
		dist = GolfBall().m_pos - dist;

		if (!m_bPassHole)
		{
			if (sqrtf(dist.x * dist.x + dist.z * dist.z) <
					GolfBall().GetRadius() * 3.0f &&
				dist.y < 0.0f)
			{
				m_bPassHole = true;
			}
		}

		if (m_bPassHole)
		{
			return true;
		}
	}

	return false;
}

void CCameraWork::ProcessBallSound(void)
{
	if (m_time - m_soundTime < 0.1f && m_ballSoundState != 1)
	{
		return;
	}

	m_soundTime = m_time;

	WVector offset;
	offset = GolfBall().m_pos - m_src[0];
	float distance = offset.Magnitude();

	if (m_ballSoundState == 0)
	{
		float speed = GolfBall().m_vel.Magnitude();
		if (speed > 80.0f && distance < 100.0f)
		{
			m_ballSoundState = 1;
			g_audio->PlaySfx("\xb0\xf8_\xb9\xd9\xb6\xf7", &offset, 5,
				"\xb0\xf8_\xb9\xd9\xb6\xf7", 0, 0, 100.0f);
		}
	}
	else if (m_ballSoundState & 1)
	{
		if (GolfBall().m_vel.Magnitude() > 80.0f)
		{
			if (distance < 100.0f)
			{
				if (!g_audio->IsPlaying("\xb0\xf8_\xb9\xd9\xb6\xf7"))
				{
					g_audio->PlaySfx("\xb0\xf8_\xb9\xd9\xb6\xf7", &offset, 5,
						"\xb0\xf8_\xb9\xd9\xb6\xf7", 0, 0, 100.0f);
				}

				g_audio->SetSFXVolumeWhilePlaying("\xb0\xf8_\xb9\xd9\xb6\xf7",
					(100.0f - distance) * 0.01f);
			}
			else if (m_ballSoundState == 1)
			{
				g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
				m_ballSoundState = 3;
			}
		}
		else
		{
			g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
			m_ballSoundState = 2;
		}
	}
}

void CCameraWork::StopBallSound(void)
{
	m_ballSoundState = 2;
	g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
}

void CCameraWork::InterpolateBezier(WVector* out, float t, WVector* points,
	int count)
{
	float t2 = t * t;
	float s = 1.0f - t;
	float s2 = s * s;

	if (count == 3)
	{
		*out = s2 * points[0] + s * t * 2.0f * points[1] + t2 * points[2];
	}
	else if (count == 4)
	{
		float t3 = t2 * t;
		float s3 = s2 * s;
		*out = s3 * points[0] + s2 * t * 3.0f * points[1] +
			s * t2 * 3.0f * points[2] + t3 * points[3];
	}
}

void CCameraWork::CheckDriveCameraBallVisibilityAndSetBallVisibilitySafeCamera(
	void)
{
	if (m_type >= 100)
	{
		return;
	}

	if (!driveTable[m_type].bCheckVisible)
	{
		return;
	}

	Waabb& aabb = GOLFDOC()->m_pPolySoup->FindModel(0)->aabb;
	WVector size = aabb.max - aabb.min;

	WVector toBall = GolfBall().m_pos - m_src[0];

	float intersection = 0.0f;
	if (!(toBall.Magnitude() > size.Magnitude() * 2.0f))
	{
		intersection = GetPVS().RayIntersectAll(m_src[0], toBall, true);
	}

	if (intersection == 1.0f)
	{
		m_hideFrame = -1;
	}
	else if (m_hideFrame < 0)
	{
		m_hideFrame = m_frame;
	}
	else if (m_frame - m_hideFrame > 50)
	{
		m_bInstalled = false;
		unsigned int i = rand() % m_enableList.size();

		if (m_enableList[i] == m_type)
		{
			i = (i + 1) % m_enableList.size();

			if (m_enableList[i] == m_type)
			{
				return;
			}
		}

		SetCamera(m_enableList[i], false);
	}
}

void CCameraWork::EarthQuake(void)
{
	if (m_time - m_quakeStepTime < 0.1f)
	{
		return;
	}

	m_quakeStepTime = m_time;

	int strength = 6 - (int)((m_time - m_quakeTime) * 6.0f);
	if (strength == 0)
	{
		strength = 1;
	}
	float half = strength * 0.05f;

	WVector shake;
	shake.x = (rand() % strength) * 0.1f - half;
	shake.y = (rand() % strength) * 0.1f - half;
	shake.z = (rand() % strength) * 0.1f - half;
	m_src[0] += shake * 0.1f;
}

void CCameraWork::FollowCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);

	if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
	}
	else if (m_curFrame < m_lastFrame)
	{
		float s = (time + m_curFrame - m_contactFrame) /
			(m_lastFrame - m_contactFrame);
		m_dest = GolfBall().m_pos + WVector(0.0f, s * m_lift, 0.0f);
	}
}

void CCameraWork::LookBackCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);

	if (m_state & 0x20000000)
	{
		if (m_curFrame < (float)m_colFrame - 40.0f)
		{
			m_dest = GolfBall().m_pos;
		}
		else
		{
			m_nextType = 14;
		}
	}
	else
	{
		if (m_curFrame < m_contactFrame * 0.65f &&
			(m_src[0] - m_dest).Magnitude() > 10.0f)
		{
			m_dest = GolfBall().m_pos;
		}
		else
		{
			m_nextType = 2;
		}
	}
}

void CCameraWork::Rail_or_SideCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);
	m_fov = 55.0f * g_DEGTORAD - g_DEGTORAD * (sinf(t * g_PI) * 25.0f);

	if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
	}
	else if (m_curFrame < m_lastFrame)
	{
		float s = (time + m_curFrame - m_contactFrame) /
			(m_lastFrame - m_contactFrame);
		m_dest = GolfBall().m_pos + WVector(0.0f, s * m_lift, 0.0f);
	}
}

void CCameraWork::PanCamera(float elapsed, float time)
{
	bool panning;

	if (m_state & 0x20000000)
	{
		panning = !(m_curFrame > (float)m_colFrame - 40.0f);
	}
	else
	{
		panning = !(m_curFrame > m_contactFrame * 0.7f);
	}

	if (panning)
	{
		int offset;

		if (m_state & 1)
		{
			m_dest = GolfBall().m_pos;
			offset = 0;
		}
		else
		{
			offset = -10;
			m_dest = GolfBall().PreData(m_frame + offset)->pos * (1.0f - time) +
				GolfBall().PreData(m_frame + offset + 1)->pos * time;
		}

		float t = (time + m_curFrame - m_startFrame) /
			(m_contactFrame * 0.7f - m_startFrame);
		m_fov =
			55.0f * g_DEGTORAD - g_DEGTORAD * (sinf(t * g_PI_DIV_2) * 25.0f);

		if (GolfClub().GetType() != 0)
		{
			return;
		}

		if (!m_bPanSound && m_topFrame - m_curFrame < 15.0f - offset)
		{
			WVector pos, dir;
			pos = GolfBall().PreData(float2int(m_curFrame))->pos;
			dir = GolfBall().PreData(float2int(m_curFrame) + 1)->pos - pos;
			dir.y = 0.0f;
			pos = m_src[0] - pos;
			pos.y = 0.0f;

			if (WCrossProduct(dir, pos).y > 0.0f)
			{
				g_audio->PlaySfx("pass_LR", 0, 0, 0, 0, 0.5f, 200.0f);
			}
			else
			{
				g_audio->PlaySfx("pass_RL", 0, 0, 0, 0, 0.5f, 200.0f);
			}

			m_bPanSound = true;
		}
	}
	else
	{
		m_fov = 55.0f * g_DEGTORAD;
		m_nextType = (m_state & 0x20000000) ? 14 : 0;
	}
}

void CCameraWork::DropCamera(float elapsed, float time)
{
	if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
	}
	else if (m_curFrame < m_lastFrame)
	{
		float t = time + m_curFrame - m_contactFrame;
		InterpolateBezier(&m_dest, t / (m_lastFrame - m_contactFrame),
			m_ctrlPoint, 4);
	}
}

void CCameraWork::Chase_and_FollowCamera(float elapsed, float time)
{
	m_dest = m_dest * 0.9f + GolfBall().m_pos * 0.1f;

	float speed = GolfBall().m_vel.Magnitude();
	WVector pos, dir;
	bool found = false;
	for (int i = m_frame; i >= Max(2, m_frame - 37); i--)
	{
		WVector prevDir =
			GolfBall().PreData(i - 1)->pos - GolfBall().PreData(i - 2)->pos;
		WVector curDir =
			GolfBall().PreData(i)->pos - GolfBall().PreData(i - 1)->pos;
		prevDir.y = 0.0f;
		curDir.y = 0.0f;
		prevDir.Normalize();
		curDir.Normalize();

		float cos45 = (float)cos((double)(45.0f * g_DEGTORAD));
		float dot = curDir * prevDir;

		if (dot < cos45)
		{
			float cos80 = (float)cos((double)(80.0f * g_DEGTORAD));

			pos = GolfBall().PreData(i - 1)->pos;
			pos.y = GolfBall().m_pos.y;
			dir = curDir - prevDir;

			if (dot < cos80)
			{
				WMatrix right = RotMat(-5.0f * g_DEGTORAD, 1);
				WMatrix left = RotMat(5.0f * g_DEGTORAD, 1);
				float side = WCrossProduct(prevDir, curDir).y;
				dir = RotVec(dir, side > 0.0f ? right : left);
			}
			found = true;
			break;
		}
	}

	if (!found)
	{
		pos = GolfBall().m_pos;
		dir = GolfBall().PreData(m_frame - 1)->pos -
			GolfBall().PreData(m_frame)->pos;
	}

	dir.y = 0.0f;
	dir.Normalize();
	dir.y = 0.5f;
	dir.Normalize();

	WVector target;
	target = pos + dir * Between(40.0f, speed * 0.4f, 80.0f);

	WVector toSource = m_src[0] - m_dest;
	WVector toTarget = target - m_dest;
	if (toTarget.Magnitude() > toSource.Magnitude())
	{
		toTarget = toSource;
		toTarget.y = 0.0f;
		toTarget.Normalize();
		toTarget.y = 0.5f;
		toTarget.Normalize();
		target = toTarget * toSource.Magnitude() + m_dest;
	}

	float t = Between(g_EPSILON, speed * 0.001f, 0.1f);

	m_src[0] = m_src[0] * (1.0f - t) + target * t;
	InterpolateBezier(&m_src[0], 0.5f, &m_src[0], 3);
}

void CCameraWork::CircleCamera(float elapsed, float time)
{
	if (m_frame < GolfBall().m_spikeDiveFrame)
	{
		if (m_roll > 0.0f)
		{
			m_roll -= elapsed * (60.0f * g_DEGTORAD);
			if (m_roll < 0.0f)
			{
				m_roll = 0.0f;
			}
		}

		WVector dir = GolfBall().m_pos - m_src[0];

		dir.y = 0.0f;
		m_distance = dir.Magnitude();
		dir.Normalize();
		dir = WCrossProduct(dir, WVector::UNIT_POS_Y);
		m_src[0] += dir * (m_distance * g_PI) * elapsed;
	}
	else
	{
		m_src[0] += GolfBall().m_vel * 0.75f * elapsed;
		m_dest += GolfBall().m_vel * 0.75f * elapsed;
		m_dest.y -= elapsed;
	}
}

void CCameraWork::Chase_and_LookBackCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);
	m_fov = 55.0f * g_DEGTORAD - g_DEGTORAD * (sinf(t * g_PI) * 25.0f);

	if (m_state & 0x20000000)
	{
		if (m_curFrame < (float)m_colFrame - 50.0f)
		{
			m_dest = GolfBall().m_pos;
		}
		else
		{
			m_nextType = 14;
		}
	}
	else
	{
		if (m_curFrame < m_contactFrame)
		{
			m_dest = GolfBall().m_pos;
		}
		else if (m_curFrame < m_lastFrame)
		{
			float s = (time + m_curFrame - m_contactFrame) /
				(m_lastFrame - m_contactFrame);
			m_dest = GolfBall().m_pos + WVector(0.0f, s * m_lift, 0.0f);
		}
	}
}

void CCameraWork::ZoomInCamera(float elapsed, float time)
{
	if (m_curFrame < m_topFrame)
	{
		float t = (time + m_curFrame) / m_topFrame;
		m_fov = 55.0f * g_DEGTORAD - g_DEGTORAD * (t * 35.0f);

		m_dest = GolfBall().m_pos;
		m_dest.y += (1.0f - t) * 2.0f;
	}
	else
	{
		m_fov = 55.0f * g_DEGTORAD;
		m_nextType = (m_state & 0x20000000) ? 14 : 11;
	}
}

void CCameraWork::CaddyViewCamera(float elapsed, float time)
{
	WPuppet* pCaddie = CSceneManager::Instance()->GetPuppet("\xc4\xb3\xb5\xf0");

	if (pCaddie == 0)
	{
		return;
	}

	WVector dir = pCaddie->GetMatrix().pivot - GolfBall().m_pos;
	dir.y = Min(0.0f, dir.y + 10.0f);
	dir.Normalize();

	WBone* pHead = pCaddie->FindBoneByTail("Head");
	if (pHead)
	{
		m_src[0] = WVector(pHead->GetMatrix().pivot.x,
			pHead->GetMatrix().pivot.y + 5.0f, pHead->GetMatrix().pivot.z);
	}
	else
	{
		m_src[0] = WVector(pCaddie->GetMatrix().pivot.x,
			pCaddie->GetMatrix().pivot.y + 10.0f, pCaddie->GetMatrix().pivot.z);
	}

	m_src[0] += WCrossProduct(WVector::UNIT_POS_Y, dir) * 10.0f + dir * 35.0f;
	m_dest = GolfBall().m_pos;
}

void CCameraWork::Drop_and_ZoomCamera(float elapsed, float time)
{
	if (m_curFrame < m_contactFrame)
	{
		float t = (time + m_curFrame - m_startFrame) /
			(m_contactFrame - m_startFrame);
		m_fov = 15.0f * g_DEGTORAD - t * (-40.0f * g_DEGTORAD);

		m_dest = GolfBall().m_pos;
	}
	else if (m_curFrame < m_lastFrame)
	{
		float t = time + m_curFrame - m_contactFrame;
		InterpolateBezier(&m_dest, t / (m_lastFrame - m_contactFrame),
			m_ctrlPoint, 4);
	}
}

void CCameraWork::BezierCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);

	if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
		m_dest.y += (1.0f - t) * 2.0f;
	}
	else if (m_curFrame < m_lastFrame)
	{
		float s = (time + m_curFrame - m_contactFrame) /
			(m_lastFrame - m_contactFrame);
		m_dest = GolfBall().m_pos + WVector(0.0f, s * m_lift, 0.0f);
	}
}

void CCameraWork::FollowWithSwingCamera(float elapsed, float time)
{
	float t = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);
	InterpolateBezier(&m_src[0], t, m_ctrlPoint, 4);

	if (m_curFrame < m_contactFrame)
	{
		m_angle -= elapsed * (2.0f * g_DEGTORAD);
	}

	WVector offset = GolfBall().m_pos - m_src[0];
	m_dest = offset * RotMat(m_angle, 0) + m_src[0];
}

void CCameraWork::SidePanWithZoomInCamera(float elapsed, float time)
{
	WVector dir = GolfBall().m_vel;

	dir.y = 0.0f;
	dir.Normalize();

	WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir).Normalize();

	if (m_time < 2.0f)
	{
		m_src[0] = GolfBall().m_pos + dir * ((1.0f - m_time * 3.0f) * 2.0f) +
			side * 3.0f;

		if (m_time < 0.5f)
		{
			m_dest = m_src[0] - side;
		}
		else
		{
			if (m_angle > -75.0f * g_DEGTORAD)
			{
				m_angle -= elapsed * (60.0f * g_DEGTORAD);
			}

			m_dest = m_src[0] - side * RotMat(m_angle, 1);
		}
	}
	else
	{
		m_nextType = 4;
	}
}

void CCameraWork::FromMidPointCamera(float elapsed, float time)
{
	if (m_state & 0x20000000)
	{
		if (m_curFrame < (float)m_colFrame - 40.0f)
		{
			m_dest = GolfBall().m_pos;
		}
		else
		{
			m_nextType = 14;
		}
	}
	else if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
	}
	else if (m_contactFrame > 250.0f)
	{
		m_nextType = 4;
	}
	else
	{
		float t = (time + m_curFrame - m_contactFrame) /
			(m_lastFrame - m_contactFrame + 1.0f);
		m_dest = GolfBall().m_pos + WVector(0.0f, t * 5.0f, 0.0f);
	}
}

void CCameraWork::FromColPointCamera(float elapsed, float time)
{
	if (m_curFrame < m_contactFrame)
	{
		m_dest = GolfBall().m_pos;
	}
	else
	{
		float t = (time + m_curFrame - m_contactFrame) /
			(m_lastFrame - m_contactFrame);
		m_dest = GolfBall().m_pos + WVector(0.0f, t * 5.0f, 0.0f);
	}
}

void CCameraWork::PangyaACamera(float elapsed, float time)
{
	if (m_time > 2.0f)
	{
		SetCamera(0x15, true);
	}
}

void CCameraWork::FixedAtSideCamera(float elapsed, float time)
{
	m_dest = GolfBall().m_pos;
}

void CCameraWork::FromRandomPointCamera(float elapsed, float time)
{
	time = (time + m_curFrame - m_startFrame) / (m_lastFrame - m_startFrame);

	m_dest = (1.0f - time) * GolfBall().PreData(0)->pos +
		time * GolfBall().m_pos + WVector(0.0f, 3.0f, 0.0f);
}

void CCameraWork::BankFromHeadCamera(float elapsed, float time)
{
	time = (time + m_curFrame - m_startFrame) / (m_topFrame - m_startFrame);

	m_roll = ((time * 60.0f) - 30.0f) * g_DEGTORAD;

	WVector dir = GolfBall().m_vel;
	dir.Normalize();

	WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir);
	side.Normalize();

	m_dest = side * (time * 0.5f - 1.0f) + GolfBall().m_pos;
	m_src[0] = dir * ((1.0f - time) * 50.0f) + m_dest;

	if (m_curFrame > m_topFrame)
	{
		m_nextType = 0;
	}
}

void CCameraWork::CobraACamera(float elapsed, float time)
{
	if (m_frame < GolfBall().m_cobraFrame)
	{
		float t = (time + m_curFrame) / GolfBall().m_cobraFrame;

		WVector dir = GolfBall().PreData(GolfBall().m_cobraFrame)->pos -
			GolfBall().PreData(0)->pos;
		WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir);
		side.Normalize();
		side *= 5.0f;

		m_src[0] = GolfBall().PreData(0)->pos +
			(GolfBall().PreData(GolfBall().m_cobraFrame)->pos -
				GolfBall().PreData(0)->pos) *
				(t * 0.3f + 0.4f) +
			side;
		m_dest = GolfBall().m_pos + WVector(0.0f, 1.0f - t, 0.0f);
	}
	else
	{
		m_nextType = 4;
	}
}

void CCameraWork::CobraBCamera(float elapsed, float time)
{
	if (m_frame < GolfBall().m_cobraFrame)
	{
		float t = (time + m_curFrame - m_startFrame) /
			(GolfBall().m_cobraFrame - m_startFrame);

		m_roll = 15.0f * g_DEGTORAD;

		m_src[0] = (1.0f - time) *
				GolfBall()
					.PreData((GolfBall().m_cobraFrame + m_frame * 4) / 5)
					->pos +
			time *
				GolfBall()
					.PreData((GolfBall().m_cobraFrame + (m_frame + 1) * 4) / 5)
					->pos;
		m_src[0].y += t * 2.0f - 1.0f;

		m_dest = GolfBall().m_pos;
	}
	else if (m_frame < GolfBall().m_bounceFrame)
	{
		m_dest.x = GolfBall().m_pos.x;
		m_dest.z = GolfBall().m_pos.z;

		if (m_frame < GolfBall().m_topFrame)
		{
			m_roll -= elapsed * (5.0f * g_DEGTORAD);

			if (m_frame < GolfBall().m_cobraFrame + 50)
			{
				m_src[0].y += elapsed * 6.0f;
			}
			else
			{
				m_dest.y += elapsed * 10.0f;
			}
		}
		else
		{
			if (m_dest.y > GolfBall().m_pos.y)
			{
				m_dest.y -= elapsed * 20.0f;
			}
			else
			{
				m_dest.y -= elapsed * 5.0f;
			}
		}
	}
}

void CCameraWork::TomahawkACamera(float elapsed, float time)
{
	if (m_time < 2.0f)
	{
		CGolfBall& ball = GolfBall();

		WMatrix view = MakeCameraMatrix(GolfBall().m_pos,
			GolfBall().m_pos - GolfBall().m_vel);

		m_src[0] = WVector(-0.7f, -0.3f, m_time - 3.0f) * view;
		m_dest = GolfBall().PreData(Max(0, m_frame - 25))->pos;
	}
	else
	{
		if (m_curFrame > m_topFrame)
		{
			m_nextType = 4;
		}
		else
		{
			m_nextType = 0x13;
		}
	}
}

void CCameraWork::SpikeACamera(float elapsed, float time)
{
	if (m_topFrame - 75.0f > m_curFrame)
	{
		float t = (time + m_curFrame) / m_topFrame;
		m_fov = 55.0f * g_DEGTORAD - t * (35.0f * g_DEGTORAD);

		m_dest = GolfBall().m_pos;
		m_dest.y += (1.0f - t) * 2.0f;
	}
	else if (m_curFrame < m_topFrame)
	{
		m_fov = 55.0f * g_DEGTORAD;
		float t = (time + m_curFrame) / m_topFrame;
		m_roll = (t * 60.0f - 30.0f) * g_DEGTORAD;

		WVector dir = GolfBall().m_vel;
		dir.Normalize();

		WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir);
		side.Normalize();

		m_dest = side * (t * 0.5f - 1.0f) + GolfBall().m_pos;
		m_src[0] = dir * ((1.0f - t * 0.9f) * 50.0f) + m_dest;

		m_srcTrans = GolfBall().m_pos - m_dest;
	}
	else if (m_frame < GolfBall().m_spikeDiveFrame)
	{
		if (m_roll > 0.0f)
		{
			m_roll -= elapsed * (60.0f * g_DEGTORAD);
			if (m_roll < 0.0f)
			{
				m_roll = 0.0f;
			}
		}

		WVector dir = GolfBall().m_pos - m_src[0];

		dir.y = 0.0f;
		m_distance = dir.Magnitude();
		dir.Normalize();
		dir = WCrossProduct(dir, WVector::UNIT_POS_Y);
		float s = (m_curFrame - m_topFrame) * 0.02f;
		if (s < 1.0f)
		{
			m_src[0] += dir *
				((s * 1.2f + 1.0f) * m_distance * (120.0f * g_DEGTORAD)) *
				elapsed;
			m_src[0].y += elapsed;
			m_dest.y += elapsed * 0.75f;
			m_dest += m_srcTrans * elapsed;
		}
	}
	else
	{
		m_src[0] += GolfBall().m_vel * 0.93f * elapsed;
		m_dest += GolfBall().m_vel * 0.93f * elapsed;

		if (GolfBall().m_afterBurnerFrame == -1 ||
			m_frame < GolfBall().m_afterBurnerFrame)
		{
			m_dest.y -= elapsed * 0.5f;
		}
	}
}

void CCameraWork::TopViewCamera(float elapsed, float time)
{
	m_dest = m_dest * 0.9f + GolfBall().m_pos * 0.1f;

	WVector dir =
		m_inputMatrix.pivot - GolfBall().PreData(GolfBall().m_renderNum)->pos;
	float distance =
		Between(100.0f, GolfBall().m_vel.Magnitude() * 2.0f, 150.0f);

	dir.y = 0.0f;
	dir.Normalize();

	dir.y = 2.0f;
	dir.Normalize();

	WVector target = GolfBall().m_pos + dir * distance;

	m_src[0] = m_src[0] * 0.9f + target * 0.1f;
	InterpolateBezier(&m_src[0], 0.5f, &m_src[0], 3);
}

void CCameraWork::JustLookingCamera(float elapsed, float time)
{
	if (m_type == 100)
	{
		WVector axis = m_matrix.za;
		WVector toBall = (GolfBall().m_pos - m_src[0]).Normalize();
		WVector toStart = (m_oldBallPos - m_matrix.pivot).Normalize();

		if (toBall.y > axis.y || toBall.y < toStart.y)
		{
			toBall.y = (toBall.y - axis.y) * elapsed * 0.1f + axis.y;
			m_dest = toBall + m_src[0];
		}
		else
		{
			float scale =
				sqrtf((1.0f - axis.y * axis.y) / (1.0f - toBall.y * toBall.y));

			toBall.x *= scale;
			toBall.y = axis.y;
			toBall.z *= scale;
			m_dest = toBall + m_src[0];
		}
	}
	else
	{
		m_dest = GolfBall().m_pos;
	}

	if (m_lastFrame > 180.0f && m_curFrame > m_lastFrame * 0.5f &&
		(GolfBall().PreData(GolfBall().m_renderNum)->pos - GetHoleCupPos()) *
				(GolfBall().PreData(0)->pos - GetHoleCupPos()) >
			0.0f &&
		(GolfBall().PreData(GolfBall().m_renderNum)->pos - GetHoleCupPos())
				.Magnitude() < 16.0f)
	{
		SetCamera(m_rand % 2 + 0x6f, true);
	}
}

void CCameraWork::SideCamera(float elapsed, float time)
{
	WVector velocity = GolfBall().m_vel;

	velocity.y = 0.0f;
	m_src[0] += velocity * elapsed * 0.8f;
	m_dest = GolfBall().m_pos;
	m_dest.y += 2.5f;
}

void CCameraWork::BallSideCamera(float elapsed, float time)
{
	WVector back;
	back = GolfBall().m_vel;
	WVector side;
	side = WCrossProduct(WVector::UNIT_POS_Y, back);
	back = back.Normalize() * GolfBall().GetRadius() * 2.0f;
	side = side.Normalize() * GolfBall().GetRadius() * 1.2f;

	float t = (time + m_curFrame) / m_lastFrame < 1.0f
		? (time + m_curFrame) / m_lastFrame
		: 1.0f;

	m_dest = GolfBall().m_pos + side - back;
	m_src[0] = (m_dest - m_matrix.pivot) * t + m_matrix.pivot - back;
}

void CCameraWork::FollowBallCamera(float elapsed, float time)
{
	WVector cup = GOLFDOC()->GetHoleData().pin;
	WVector v = GolfBall().m_pos - cup;
	v.y = 0.0f;
	float cupDistance2 = v.x * v.x;
	cupDistance2 += v.z * v.z;

	v = GolfBall().m_vel;

	v.y = 0.0f;

	if (!(sqrtf(v.z * v.z + v.x * v.x) > 1.0f) ||
		!(sqrtf(cupDistance2) > GolfBall().GetRadius() * 3.0f))
	{
		return;
	}

	m_dest = GetPVS().GetGroundPoint(GolfBall().m_pos, false, 0, false);
	m_dest.y += GolfBall().GetRadius();
	m_src[0] = m_dest - v.Normalize() * 3.2f;
	m_src[0].y += 1.0f;
}

void CCameraWork::FixedCamera(float elapsed, float time)
{
	if (!g_view->InFrustumSafe(
			WSphere(GolfBall().m_pos, GolfBall().GetRadius())))
	{
		SetCamera(m_rand % 2 + 0x6d, true);
	}
}

void CCameraWork::PuttSecondACamera(float elapsed, float time)
{
	m_src[0].y -= elapsed * 9.0f;
	m_dest = GolfBall().m_pos;
}

void CCameraWork::PuttSecondBCamera(float elapsed, float time)
{
	if ((g_camera.pivot - WVector(GOLFDOC()->GetHoleData().pin)).Magnitude() <
		16.0f)
	{
		m_pfnCamera = 0;
		return;
	}

	WVector dir =
		GolfBall().PreData(0)->pos - WVector(GOLFDOC()->GetHoleData().pin);
	dir.y = 0.0f;
	dir.Normalize();
	dir *= elapsed * 20.0f;

	m_src[0] += dir;
	m_dest = GolfBall().m_pos;

	m_fov += elapsed * (10.0f * g_DEGTORAD);
}

void CCameraWork::FromHoleCupCamera(float elapsed, float time)
{
	WVector cup = GOLFDOC()->GetHoleData().pin;
	WVector& source = m_src[0] = WVector(cup.x, cup.y - 2.0f, cup.z);
	m_dest = WVector(source.x, source.y + 1.0f, source.z + 0.2f);
}

void CCameraWork::IntroACamera(float elapsed, float time)
{
	m_angle -= elapsed * (30.0f * g_DEGTORAD);
	m_distance -= elapsed * 11.0f;
	m_height -= elapsed * 2.0f;

	WMatrix rot = RotMat(m_angle + GOLFDOC()->GetCameraData().angle, 1);
	m_src[0] = GolfBall().m_pos - rot.za * m_distance;
	m_src[0].y += m_height;

	m_dest = rot.za + m_src[0];

	if (m_angle < 0.0f)
	{
		m_nextType = 0xcc;
	}
}

void CCameraWork::IntroBCamera(float elapsed, float time)
{
	float t = m_time * 0.2f;
	m_src[0] = (m_destTrans - m_srcTrans) * t + m_srcTrans;
	m_src[0].y += m_height;

	m_dest = GolfBall().m_pos;
	float x = t * 2.0f - 1.0f;
	m_dest.y += m_height - (m_height - 3.0f) * (x > 0.0f ? x : -x);

	if (m_time > 5.0f)
	{
		m_nextType = 0xcd;
	}
}

void CCameraWork::ZoomOutCamera(float elapsed, float time)
{
	m_fov += elapsed * (55.0f * g_DEGTORAD);

	if (m_fov > 55.0f * g_DEGTORAD)
	{
		m_fov = 55.0f * g_DEGTORAD;
	}
}

void CCameraWork::IntroCCamera(float elapsed, float time)
{
	m_angle -= elapsed * (30.0f * g_DEGTORAD);
	m_distance += elapsed * 2.0f;

	WPuppet* pPuppet = CSceneManager::Instance()->GetPuppet(
		MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer));
	WBone* pHead = pPuppet->FindBone("Bip01 Head");
	float t = m_time * 0.2f;

	m_dest = (1.0f - t) * pHead->GetMatrix().pivot +
		(GolfBall().m_pos + WVector(0.0f, 5.0f, 0.0f)) * t;

	WMatrix rot = RotMat(GOLFDOC()->GetCameraData().angle + m_angle, 1);
	m_src[0] = m_dest - rot.za * m_distance;

	if (t > 1.0f)
	{
		m_nextType = 0xcc;
	}
}

void CCameraWork::IntroDCamera(float elapsed, float time)
{
	if (m_time < 3.0f)
	{
		WVector move = elapsed * GOLFDOC()->GetCameraData().mat.xa;
		m_src[0] += move;
		m_dest += move;
	}
	else if (m_time > 4.0f)
	{
		m_src[0] = GolfBall().m_pos -
			25.0f * RotMat(GOLFDOC()->GetCameraData().angle, 1).za;
		m_src[0].y += 5.0f;
		m_dest = m_src[0] + RotMat(GOLFDOC()->GetCameraData().angle, 1).za;
		m_matrix = GetCameraMatrix(m_src[0], m_dest, 0.0f);

		m_nextType = 0xCC;
	}
}

void CCameraWork::ToDefaultCamera(float elapsed, float time)
{
	if (m_time > m_finishTime)
	{
		return;
	}

	m_src[0] = (m_destTrans - m_srcTrans) * m_time + m_srcTrans;

	WMatrix rot;
	rot = WQuaternionSlerp(m_srcRot, m_destRot, m_time);

	m_dest = rot.za + m_src[0];
}

void CCameraWork::FirstPersonCamera(float elapsed, float time)
{
}

void CCameraWork::StillFrameCamera(float elapsed, float time)
{
}

void CCameraWork::CaddieChanceCamera(float elapsed, float time)
{
	float t = Between(0.0f, 1.0f - m_time * 0.33333334f, 1.0f);
	WVector offset = WVector::UNIT_POS_Z * m_caddieRot;
	offset = offset * RotMat(t * m_angle, 1);

	float clamped = CAMERA_CLAMP(t, 0.0f, 1.0f);
	m_src[0] = (m_srcTrans - m_destTrans) * clamped + m_destTrans;
	m_dest = offset + m_src[0];
}

void CCameraWork::SwingReplayACamera(float elapsed, float time)
{
}

void CCameraWork::SwingReplayBCamera(float elapsed, float time)
{
	if (m_time > m_finishTime)
	{
		return;
	}

	float t = m_time / m_finishTime;

	m_src[0] = (m_destTrans - m_srcTrans) * t + m_srcTrans;

	WMatrix rot;
	rot = WQuaternionSlerp(m_srcRot, m_destRot, t);

	m_dest = rot.za + m_src[0];
}

void CCameraWork::ExtraTimeCamera(float elapsed, float time)
{
	m_angle += elapsed * (5.0f * g_DEGTORAD);

	WMatrix rot;
	rot.Reset();
	rot.Rotate(m_angle, 1);

	m_src[0] = rot.za * m_distance + m_dest;
	m_src[0].y += m_distance * 0.2f;
}

void CCameraWork::Quake(void)
{
	m_state |= 0x80000000;
	m_quakeTime = m_time;
	m_quakeStepTime = -1.0f;
}

WMatrix CCameraWork::GetCameraMatrix(const WVector& eye, const WVector& at,
	float roll)
{
	if (Wabs(roll) < g_EPSILON)
	{
		return MakeCameraMatrix(eye, at);
	}
	else
	{
		return RotMat(roll, 2) * MakeCameraMatrix(eye, at);
	}
}
