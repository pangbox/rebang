#include "stdafx.h"
#include "camera.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "polysoup.h"
#include "quadtree.h"
#include "clientsetting.h"
#include "mousecursor.h"
#include "inputmanager.h"
#include "helpmsg.h"
#include "itemwindow.h"
#include "quitwindow.h"
#include "fresh.h"
#include "wmathutil.inl"
#include "soundmanager.h"
#include "capturedbg.h"
#include "user_info.h"
#include "messengerdlg.h"

extern "C" void _WriteBarrier();
#pragma intrinsic(_WriteBarrier)

extern Fresh* g_pFresh;
IMPLEMENT_ACTOR(CCamera, IActor)

CCamera::CCamera()
{
	(void)&g_HUGE;
	AddSkipList(0x84c82);
}

void CCamera::OnInit()
{
	m_rotateAcc = 0;
	m_moveAcc = 0;
	m_horizonAcc = 0;
	m_heightRate = 0;
	m_change = 0;
	m_translated = false;
	m_inputDelay = 0;
	m_camMode = false;
	m_sendTime = 0;
	m_angleChanged = false;
	m_active = false;
	m_remainRotation = 0;
	m_totalRotation = 0;
	m_spotCam.time = 0;
	m_spotCam.moving = false;
	m_currentAngle = 0;
	m_targetAngle = 0;
}

void CCamera::OnProcess(float dt)
{
	if (!m_active || CCapturedBg::Instance()->IsCaptured() ||
		(Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].info.dwIdentity & 1))
		return;
	if (g_input->GetButton(MIDDLE_BUTTON) == 1 && CItemWindow::IsClosed())
	{
		m_camMode = !m_camMode;
		CMouseCursor::Instance()->SetCamMode(m_camMode);
		if (m_camMode)
			SendMsg(this, "Screen", 433, 0, 0, 0, 0);
	}
	if (CMouseCursor::Instance()->GetArea() == 22 &&
		CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) &&
		g_input->GetButton(LEFT_BUTTON) == 2)
		SendMsg(this, "GolfRule", 120, 8, 0, 0, 0);
	bool gallery = false;
	if (OnlinePlay() && Doc()->GetReplayState() != 2)
	{
		if (IsMyTurn(false))
		{
			m_sendTime += dt;
			if (m_sendTime > 2)
			{
				m_sendTime -= 2;
				if (m_angleChanged)
				{
					m_angleChanged = false;
					WSendPacket packet((enumClientPacket)19);
					packet.EncodeBuffer(&GOLFDOC()->m_cameraData.angle, 4);
					packet.Send((eSendTo)0);
				}
			}
		}
		else
		{
			gallery = true;
		}
	}
	else if (Doc()->m_gameMode == 5 && !IsMyTurn(false))
		gallery = true;
	if (gallery)
	{
		float angle = Between(dt * -0.52359879f,
			GetBaseAngle(m_targetAngle - m_currentAngle), dt * 0.52359879f);
		if (!WisZero(angle))
		{
			SendMsg(this, "Player", 417, (int)&angle, 0, 0, 0);
			SendMsg(this, "Spot", 417, (int)&angle, 0, 0, 0);
			SendMsg(this, "Screen", 417, (int)&angle, 0, 0, 0);
			SendMsg(this, "Grid", 417, (int)&angle, 0, 0, 0);
			SendMsg(this, "Caddie", 417, (int)&angle, 0, 0, 0);
			SendMsg(this, "GuideLine", 417, 0, 0, 0, 0);
			m_currentAngle = GetBaseAngle(m_currentAngle + angle);
			RotateForGallery(angle);
		}
	}

	ProcessToSpot(dt);
	FixedCamera(dt);
	if (!(GOLFDOC()->m_gameMode & 0x8000))
	{
		if (m_inputDelay > 0)
		{
			m_inputDelay -= dt;
		}
		else
		{
			if (WisZero(m_remainRotation))
				ProcessInput(dt);
			else
				ProcessForcedMove(dt);
			GetRotateAcc(dt);
			GetMoveAcc(dt);
			GetHorizonAcc(dt);
		}
	}
}

void CCamera::ProcessToSpot(float dt)
{
	if (GOLFDOC()->m_tutorialMode < 1 || CQuitWindow::Instance()->IsActive() ||
		!CItemWindow::IsClosed() || !IsMyTurn(false) ||
		!WisZero(m_remainRotation))
		return;
	if (g_input->GetDown("NUM_1", true) || g_input->GetDown("PAD_1", true))
	{
		if (GOLFDOC()->m_gameMode != 0x40000 &&
			GetPVS().GetMeshIndex(GolfBall().m_pos +
					(GolfClub().GetRange() * 3.2f) *
						GOLFDOC()->m_cameraData.mat.za,
				false) == -1)
			return;
		switch (GOLFDOC()->m_gameMode)
		{
		case 8:
		case 256:
			SendMsg(this, "GolfRule", 120, 0x40000, 0, 0, 0);
			SetSpotCamera(false);
			g_audio->PlaySfx("camera_move_to_spot");
			break;
		case 512:
			if (GOLFDOC()->m_bDropRequest)
				break;
		case 0x8000:
			SendMsg(this, "GolfRule", 120, 0x40000, 0, 0, 0);
			SetSpotCamera(true);
			g_audio->PlaySfx("camera_move_to_spot");
			break;
		case 0x40000:
			SendMsg(this, "GolfRule", 120, 8, 0, 0, 0);
			break;
		}
	}
	if (GOLFDOC()->m_gameMode == 0x40000)
	{
		if (GolfClub().Updated())
			SetSpotCamera(true);
		if (m_spotCam.moving)
		{
			m_spotCam.time += dt;
			if (m_spotCam.time > 1.2f)
			{
				m_spotCam.time = 1.2f;
				m_spotCam.moving = false;
			}
			m_spotCam.pitch = m_spotCam.startPitch +
				(m_spotCam.endPitch - m_spotCam.startPitch) * m_spotCam.time;
			m_pos = m_spotCam.start + m_spotCam.time * m_spotCam.velocity;
			m_pos.y = Max(GetPVS().GetGroundPoint(m_pos, false, NULL, true).y +
					GOLFDOC()->m_cameraData.clearance,
				m_pos.y);
			m_target = m_pos + GOLFDOC()->m_cameraData.mat.za;
			m_target.y += tan((double)m_spotCam.pitch);
			g_camera = MakeCameraMatrix(m_pos, m_target);
		}
	}
}

void CCamera::SetSpotCamera(bool apply)
{
	m_spotCam.moving = !apply;
	int index;
	m_spotCam.end = GetPVS().GetGroundPoint(GolfBall().m_pos +
			(GolfClub().GetRange() * 3.2f) * GOLFDOC()->m_cameraData.mat.za,
		true, &index, true);
	if (index == -1)
	{
		if (apply)
			SendMsg(this, "GolfRule", 120, 8, 0, 0, 0);
		return;
	}
	m_spotCam.end += RotVec(WVector(0, 30, -60), GOLFDOC()->m_cameraData.mat);
	m_spotCam.endPitch = -0.2617994f;
	if (apply)
	{
		m_pos = m_spotCam.end;
		m_target = m_spotCam.end + GOLFDOC()->m_cameraData.mat.za;
		m_target.y += tan((double)m_spotCam.endPitch);
		g_camera = MakeCameraMatrix(m_pos, m_target);
	}
	else
	{
		m_spotCam.start = g_camera.pivot;
		m_spotCam.startPitch =
			atanf(g_camera.za.y / sqrtf(1 - g_camera.za.y * g_camera.za.y));
		m_spotCam.velocity = (m_spotCam.end - m_spotCam.start) * (1.f / 1.2f);
		m_spotCam.time = 0;
	}
}

void CCamera::ProcessInput(float dt)
{
	static bool first = true;
	static bool down = false;
	static bool up = false;
	static unsigned int speed = 0;
	static bool repeat = false;
	static unsigned long tick = 0;
	static unsigned long last = 0;
	m_rotateDirection = 0;
	m_moveDirection = 0;
	m_horizonDirection = 0;
	if (((GOLFDOC()->m_gameMode & 0x40000) && m_spotCam.moving) ||
		CQuitWindow::Instance()->IsActive() || !CItemWindow::IsClosed())
		return;
	if (CMouseCursor::Instance()->GetArea() == 23 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xc1\xc2", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
	{
		m_rotateDirection = 1;
		Rotate(dt * 1.0471976f);
	}
	if (CMouseCursor::Instance()->GetArea() == 24 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xbf\xec", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
	{
		m_rotateDirection = -1;
		Rotate(dt * -1.0471976f);
	}
	if (GOLFDOC()->m_gameMode & 0x40000)
		return;
	if (CMouseCursor::Instance()->GetArea() == 8 &&
			g_pFresh->IsDesktopFocused() ||
		CMouseCursor::Instance()->GetArea() == 25 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xbb\xf3", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
	{
		if (GOLFDOC()->m_gameMode == 512)
		{
			m_horizonDirection = 1;
			Translate(dt);
		}
		else if (m_heightRate < 1)
		{
			m_moveDirection = 1;
			UpDown(dt);
		}
	}
	if (CMouseCursor::Instance()->GetArea() == 9 &&
			g_pFresh->IsDesktopFocused() ||
		CMouseCursor::Instance()->GetArea() == 26 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xc7\xcf", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
	{
		if (GOLFDOC()->m_gameMode == 512)
		{
			m_horizonDirection = -1;
			Translate(dt * -1.f);
		}
		else if (m_heightRate > 0)
		{
			m_moveDirection = -1;
			UpDown(dt * -1.f);
		}
	}
	float accelerated = 0;
	if (CMouseCursor::Instance()->GetArea() == 27 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xbb\xf3", true) &&
			(g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) ||
		g_input->Get("HOME", true))
	{
		if (GOLFDOC()->m_gameMode == 512)
		{
			tick = timeGetTime();
			if (tick - last > 800 && !repeat)
			{
				last = tick;
				if (!first)
					repeat = true;
				first = false;
			}
			if (tick - last > 50 && repeat == true)
			{
				last = tick;
				if (speed < 25)
					++speed;
				accelerated = (speed >> 1) * dt;
			}
			if (m_heightRate > 0)
			{
				m_moveDirection = -1;
				UpDown((accelerated + dt) * -1.f);
			}
			down = true;
			up = false;
		}
		else if (IsMyTurn(false))
		{
			m_horizonDirection = 1;
			Translate(dt);
		}
	}
	else if (!up)
	{
		tick = 0;
		last = 0;
		repeat = false;
		accelerated = 0;
		speed = 0;
		down = false;
		first = true;
	}
	if (CMouseCursor::Instance()->GetArea() == 28 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0 ||
		g_input->Get("\xc7\xcf", true) &&
			(g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) ||
		g_input->Get("END", true))
	{
		if (GOLFDOC()->m_gameMode == 512)
		{
			tick = timeGetTime();
			if (tick - last > 800 && !repeat)
			{
				last = tick;
				if (!first)
					repeat = true;
				first = false;
			}
			if (tick - last > 50 && repeat == true)
			{
				last = tick;
				if (speed < 25)
					++speed;
				accelerated = (speed >> 1) * dt;
			}
			if (m_heightRate < 1)
			{
				m_moveDirection = 1;
				UpDown(accelerated + dt);
			}
			up = true;
			down = false;
		}
		else if (IsMyTurn(false))
		{
			m_horizonDirection = -1;
			Translate(dt * -1.f);
		}
	}
	else if (!down)
	{
		tick = 0;
		last = 0;
		repeat = false;
		speed = 0;
		up = false;
		first = true;
	}
	bool guide, score;
	SendMsg(this, "GuideBook", 300, (int)&guide, 0, 0, 0);
	SendMsg(this, "Score", 313, (int)&score, 0, 0, 0);
	if ((GOLFDOC()->m_gameMode & 512) && IsMyTurn(false) && !m_moveDirection &&
		!m_horizonDirection && !CMouseCursor::Instance()->GetArea() &&
		g_input->GetButton(LEFT_BUTTON) == 1 &&
		GOLFDOC()->m_tutorialMode >= 1 && !CUserInfo::Instance()->IsVisible() &&
		!guide && !score && !g_pFresh->GetManager()->HasValidWindow() &&
		CMessengerInfo::Instance()->IsGameFocus() == true)
		ProcessMousePick(dt);
}

void CCamera::ProcessMousePick(float dt)
{
	WVector mouse = g_input->GetMousePoint();
	WVector start, ray;
	g_view->GetScreen2World(mouse.x, mouse.y, start, ray);
	WVector hit;
	hit = GetPVS().GetRayIntersection(start, ray);
	if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true))
	{
		hit.y += GolfBall().GetRadius();
		if (GOLFDOC()->m_bDropRequest)
		{
			WVector positions[2];
			positions[0] = GolfBall().m_pos - GetHoleCupPos();
			positions[0].y = 0;
			positions[1] = hit - GetHoleCupPos();
			positions[1].y = 0;
			unsigned char ground = GetPVS().GetGroundType(hit, true, NULL);
			if (!GetPVS().OverlappedWithObject(hit, 0) &&
				(!ground ||
					ground == 1 && !GOLFDOC()->m_pPolySoup->IsOutOfBound(hit) &&
						positions[1].Magnitude() >
							positions[0].Magnitude() - 25.6f))
			{
				GOLFDOC()->m_bDropRequest = false;
				if (OnlinePlay() && GOLFDOC()->m_tutorialMode == 15)
				{
					WSendPacket packet((enumClientPacket)25);
					packet.EncodeBuffer(&hit, sizeof(hit));
					packet.Send((eSendTo)0);
				}
				else
					SendMsg(this, "GolfRule", 141, (int)&hit, 0, 0, 0);
			}
			else
			{
				g_audio->PlaySfx("play_unplayable");
				if (CHelpMsg::Instance())
					CHelpMsg::Instance()->SetHelpMsg(20, NULL);
			}
		}
		else
		{
			if (!WNetworkSystem::Instance()->IsConnected(
					(WNetworkSystem::eNetUnit)0))
				SendMsg(this, "GolfRule", 142, (int)&hit, 0, 0, 0);
		}
	}
	else
	{
		WVector screen = g_view->Projection(GolfBall().m_pos);
		if (sqrtf((mouse.y - screen.y) * (mouse.y - screen.y) +
				(mouse.x - screen.x) * (mouse.x - screen.x)) > 10)
		{
			ray = hit - GolfBall().m_pos;
			ray.y = 0;
			float distance = ray * ray;
			ray.Normalize();
			// HACK!
			_WriteBarrier();
			GolfClub().SelectClubByDist(sqrtf(distance) * 0.3125f);
			WVector cross;
			cross = WCrossProduct(g_camera.ya, ray);
			float angle;
			if (Wabs(cross.y) < g_EPSILON ||
				WisZero(angle = acosf(Between(-1.f, ray * g_camera.ya, 1.f))))
			{
				SendMsg(this, "Spot", 245, 0, 0, 0, 0);
			}
			else
			{
				if (cross.y > 0)
				{
					m_rotateDirection = -1;
					angle *= -1;
				}
				else
				{
					m_rotateDirection = 1;
				}
				angle = Between(-g_PI, angle, g_PI);
				m_remainRotation = angle;
				m_totalRotation = angle;
				angle += GOLFDOC()->m_cameraData.angle;
				SendMsg(this, "Spot", 246, (int)&angle, (int)&hit, 0, 0);
			}
		}
	}
}

void CCamera::ProcessForcedMove(float dt)
{
	Rotate((m_rotateDirection == 1 ? 1.f : -1.f) * dt * 4.1887903f);
	if (WisZero(m_totalRotation))
		dt = 1;
	else
		dt = 1 - m_remainRotation / m_totalRotation;
	SendMsg(this, "Spot", 247, (int)&dt, 0, 0, 0);
}

void CCamera::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}
	switch (msg.message)
	{
	case 241:
		m_change = 0;
		m_heightRate = 0.5f;
		break;
	case 238:
		m_pos = GOLFDOC()->m_cameraData.mat.pivot;
		m_target =
			GOLFDOC()->m_cameraData.mat.pivot + GOLFDOC()->m_cameraData.mat.za;
		m_target.y += tan((double)GOLFDOC()->m_cameraData.pitch);
		m_change = 0;
		m_heightRate = m_baseHeightRate =
			GetHeightRate(GOLFDOC()->m_cameraData.mat.pivot.y);
		m_initialHeightRate =
			GetHeightRate(GOLFDOC()->m_cameraData.mat.pivot.y);
		break;
	case 239:
		m_lowest = GetPVS().GetLowest();
		m_ceiling = m_lowest + 192;
		break;
	case 240:
		m_translated = false;
		break;
	case 242:
		m_targetAngle = *(float*)msg.param1;
		break;
	case 410:
		m_active = true;
		break;
	case 411:
		m_active = false;
		break;
	case 412:
		*(int*)msg.param1 = m_active;
		break;
	case 10008:
		m_sendTime = 0;
		GOLFDOC()->m_cameraData.angle = *(float*)msg.param1;
		if (!IsMyTurn(false))
		{
			m_targetAngle = GOLFDOC()->m_cameraData.angle;
			m_currentAngle = GOLFDOC()->m_cameraData.angle;
		}
		break;
	}
}

void CCamera::LookAt(WVector& pos, WVector& target)
{
	g_camera = MakeCameraMatrix(pos, target);
}

void CCamera::Rotate(float dt)
{
	dt *= m_rotateAcc;
	if (m_remainRotation < 0 || 0 < m_remainRotation)
	{
		if (Wabs(m_remainRotation) < Wabs(dt) || WisZero(m_remainRotation - dt))
		{
			dt = m_remainRotation;
			m_remainRotation = 0;
			m_totalRotation = 0;
		}
		else
		{
			m_remainRotation -= dt;
		}
	}
	WMatrix matrix = g_camera * RotMat(dt, 1);
	matrix = g_camera;
	WVector& pos = matrix.pivot;
	pos.x = pos.y = pos.z = 0;
	matrix = matrix * RotMat(dt, 1);
	RotateEnvironment(dt);
	if (GOLFDOC()->m_gameMode & 8)
	{
		pos = GOLFDOC()->m_cameraData.mat.pivot;
	}
	else
	{
		pos = GOLFDOC()->m_cameraData.target +
			(g_camera.pivot - GOLFDOC()->m_cameraData.target) * RotMat(dt, 1);
		pos = GetPVS().TestCameraCollision(pos);
		if (GOLFDOC()->m_gameMode & 512)
			pos.y = g_camera.pivot.y;
	}
	g_camera = matrix;
	m_pos = g_camera.pivot;
	m_target = g_camera.pivot + g_camera.za;
	if (GOLFDOC()->m_gameMode & 0x40000 && !m_spotCam.moving)
		SetSpotCamera(true);
}

void CCamera::Translate(float dt)
{
	if (GOLFDOC()->m_gameMode == 512)
	{
		float rate = Between(.1f,
			(g_camera.pivot.y -
				GetPVS().GetGroundPoint(g_camera.pivot, true, NULL, true).y) *
				.01f,
			1.f);
		g_camera.pivot +=
			GOLFDOC()->m_cameraData.mat.za * (rate * m_horizonAcc * dt * 400);
		g_camera.pivot = GetPVS().TestCameraCollision(g_camera.pivot);
	}
	else
	{
		WVector diff =
			GOLFDOC()->m_cameraData.mat.za * (dt * m_horizonAcc * 400);
		WVector pos = GetPVS().TestCameraCollision(g_camera.pivot + diff);
		float baseGround = m_lowest;
		float ground =
			Max(GetPVS().GetGroundPoint(pos, true, NULL, true).y, baseGround);
		pos.y = Between(ground + GOLFDOC()->m_cameraData.clearance, pos.y,
			m_ceiling);
		diff = pos - g_camera.pivot;
		m_pos += diff;
		m_target = m_pos + GOLFDOC()->m_cameraData.mat.za;
		m_heightRate = (m_pos.y - m_lowest) / 192;
		m_change = m_heightRate - m_baseHeightRate;
		m_target.y += tan((double)GetCameraAngle());
		g_camera = MakeCameraMatrix(m_pos, m_target);
		m_translated = true;
		m_baseHeightRate =
			GetHeightRate(ground + GOLFDOC()->m_cameraData.clearance);

		float before =
			(((g_camera.pivot - GOLFDOC()->m_cameraData.mat.pivot) - diff) *
				GOLFDOC()->m_cameraData.mat.za);
		float after = ((g_camera.pivot - GOLFDOC()->m_cameraData.mat.pivot) *
			GOLFDOC()->m_cameraData.mat.za);
		float crossing = before * after;
		if (GOLFDOC()->m_gameMode == 256 && crossing < 0 &&
			Abs(g_camera.pivot.y - GOLFDOC()->m_cameraData.mat.pivot.y) < 3.2f)
		{
			m_pos = GOLFDOC()->m_cameraData.mat.pivot;
			m_target = GOLFDOC()->m_cameraData.mat.pivot +
				GOLFDOC()->m_cameraData.mat.za;
			m_target.y += tan((double)GOLFDOC()->m_cameraData.pitch);
			m_change = 0;
			m_heightRate = m_baseHeightRate;
			g_camera.pivot = GOLFDOC()->m_cameraData.mat.pivot;
			if (IsMyTurn(true))
			{
				GOLFDOC()->m_uiAlpha = 255;
				CChatMsg::Instance()->ReplaceBackupUiAlpha();
			}
			GOLFDOC()->m_gameMode = 8;
			m_inputDelay = .25f;
		}
		else if (GOLFDOC()->m_gameMode == 8)
		{
			GOLFDOC()->m_uiAlpha = 128;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
			GOLFDOC()->m_gameMode = 256;
		}
	}
}

void CCamera::UpDown(float dt)
{
	if (GOLFDOC()->m_gameMode == 512)
	{
		float ground =
			GetPVS().GetGroundPoint(g_camera.pivot, true, NULL, true).y + 16;
		float height = g_camera.pivot.y - ground;
		if (height < 300)
			dt *= Max(0.2f, height / 300);
		g_camera.pivot.y += dt * 400;
		g_camera.pivot.y = Between(ground, g_camera.pivot.y, m_lowest + 2560);
	}
	else
	{
		if (GOLFDOC()->m_gameMode == 8 && dt > 0)
		{
			GOLFDOC()->m_gameMode = 256;
			GOLFDOC()->m_uiAlpha = 128;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}
		m_change =
			Between(0.f, m_change + dt * m_moveAcc, 1 - m_baseHeightRate);
		if (m_translated)
		{
			m_heightRate = m_change + m_baseHeightRate;
		}
		else
		{
			if (GolfBall().m_groundType == 2)
				m_heightRate = m_baseHeightRate + m_change;
			else
				m_heightRate = m_change * m_change + m_baseHeightRate;
			g_camera.pivot = GOLFDOC()->m_cameraData.mat.pivot -
				GOLFDOC()->m_cameraData.mat.za *
					((GOLFDOC()->m_cameraData.mat.pivot.y - m_lowest + 192) *
						(1 - m_baseHeightRate) * m_change);
		}
		g_camera.pivot.y =
			(GOLFDOC()->m_cameraData.mat.pivot.y - m_lowest + 192) *
				m_heightRate +
			m_lowest;
		g_camera.pivot = GetPVS().TestCameraCollision(g_camera.pivot);
		m_pos = g_camera.pivot;
		m_target = g_camera.pivot + GOLFDOC()->m_cameraData.mat.za;
		m_target.y += tan((double)GetCameraAngle());
		g_camera = MakeCameraMatrix(m_pos, m_target);
		if (!m_translated && m_heightRate == m_initialHeightRate)
		{
			GOLFDOC()->m_gameMode = 8;
			if (IsMyTurn(true))
			{
				GOLFDOC()->m_uiAlpha = 255;
				CChatMsg::Instance()->ReplaceBackupUiAlpha();
			}
		}
	}
}

float CCamera::GetCameraAngle()
{
	return Min(0.f, GOLFDOC()->m_cameraData.pitch - atanf(m_change * .65f));
}

float CCamera::GetChangeRate()
{
	return m_change * m_change;
}

void CCamera::GetRotateAcc(float dt)
{
	static int previous = 0;
	if (previous == m_rotateDirection && m_rotateDirection)
		m_rotateAcc = Between(0.f, m_rotateAcc + dt, 1.f);
	else
		m_rotateAcc = 0;
	previous = m_rotateDirection;
}

void CCamera::GetMoveAcc(float dt)
{
	static int previous = 0;
	if (previous == m_moveDirection && m_moveDirection)
		m_moveAcc = Between(0.f, m_moveAcc + dt, 1.f);
	else
		m_moveAcc = 0;
	previous = m_moveDirection;
}

void CCamera::GetHorizonAcc(float dt)
{
	static int previous = 0;
	if (previous == m_horizonDirection && m_horizonDirection)
		m_horizonAcc = Between(0.f, m_horizonAcc + dt, 1.f);
	else
		m_horizonAcc = 0;
	previous = m_horizonDirection;
}

float CCamera::GetHeightRate(float height)
{
	float delta = height - m_lowest;
	return delta / (delta + 192);
}

void CCamera::FixedCamera(float dt)
{
	if (CQuitWindow::Instance()->IsActive() || !CItemWindow::IsClosed() ||
		((unsigned char)(Doc()->m_myInfo.info.dwIdentity >> 1) & 1) ||
		GOLFDOC()->m_bDropRequest)
		return;
	int index;
	for (index = 3; index < 10; ++index)
	{
		if (g_input->GetDown(MakeStr("PAD_%d", index), true) ||
			g_input->GetDown(MakeStr("NUM_%d", index), true))
		{
			WPolySoup::sCamera* camera = GOLFDOC()->m_pPolySoup->FindCamera(
				MakeStr("\xc4\xab\xb8\xde\xb6\xf3%d", index));
			if (camera)
			{
				m_yaw = 0;
				m_pitch = 0;
				m_pos = camera->mat.pivot;
				m_target = m_pos + camera->mat.za * 10;
				g_camera = camera->mat;
				g_view->SetFOV(camera->fov * g_DEGTORAD);
				GOLFDOC()->m_gameMode = 0x8000;
			}
			return;
		}
	}
	if (GOLFDOC()->m_gameMode == 0x8000)
	{
		m_rotateDirection = 0;
		if (CMouseCursor::Instance()->GetArea() == 23 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0)
		{
			m_rotateDirection = 1;
			YawFixedCamera(dt);
		}
		if (CMouseCursor::Instance()->GetArea() == 24 &&
			(int)g_input->GetButton(LEFT_BUTTON) > 0)
		{
			m_rotateDirection = -1;
			YawFixedCamera(dt * -1.f);
		}
		if (g_input->Get("\xc1\xc2", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
			RotateEnvironment(dt * 1.0471976f);
		if (g_input->Get("\xbf\xec", true) && !g_input->Get("LCONTROL", true) &&
			!g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) &&
			!g_input->Get("RSHIFT", true))
			RotateEnvironment(dt * -1.0471976f);
		GetRotateAcc(dt);
		m_horizonDirection = 0;
		int area = CMouseCursor::Instance()->GetArea();
		if (area == 8 || area == 25 && (int)g_input->GetButton(LEFT_BUTTON) > 0)
		{
			m_horizonDirection = 1;
			PitchFixedCamera(dt);
		}
		area = CMouseCursor::Instance()->GetArea();
		if (area == 9 || area == 26 && (int)g_input->GetButton(LEFT_BUTTON) > 0)
		{
			m_horizonDirection = -1;
			PitchFixedCamera(dt * -1.f);
		}
		GetHorizonAcc(dt);
	}
}

void CCamera::YawFixedCamera(float dt)
{
	double angle = dt * m_rotateAcc * 5;
	m_yaw -= angle;
	if (m_yaw > 10)
	{
		m_yaw = 10;
	}
	else if (m_yaw < -10)
	{
		m_yaw = -10;
	}
	else
	{
		m_target -= g_camera.xa * angle;
		g_camera = MakeCameraMatrix(m_pos, m_target);
	}
}

void CCamera::PitchFixedCamera(float dt)
{
	double angle = dt * m_horizonAcc * 5;
	m_pitch += (float)angle;
	if (m_pitch > 5)
	{
		m_pitch = 5;
	}
	else if (m_pitch < -5)
	{
		m_pitch = -5;
	}
	else
	{
		m_target += g_camera.ya * angle;
		g_camera = MakeCameraMatrix(m_pos, m_target);
	}
}

void CCamera::RotateEnvironment(float angle)
{
	GOLFDOC()->m_cameraData.angle =
		GetBaseAngle(GOLFDOC()->m_cameraData.angle + angle);
	GOLFDOC()->m_cameraData.mat = RotMat(GOLFDOC()->m_cameraData.angle +
			(IsMyTurn(true) ? 0 : GOLFDOC()->m_cameraData.angleOffset),
		1);
	WVector direction = GOLFDOC()->m_cameraData.mat.za *
		(GOLFDOC()->m_cameraData.distance * -1.f);
	direction.y = GOLFDOC()->m_cameraData.height;
	GOLFDOC()->m_cameraData.mat.pivot = GetPVS().TestCameraCollision(
		GOLFDOC()->m_cameraData.target + direction);
	m_initialHeightRate = GetHeightRate(GOLFDOC()->m_cameraData.mat.pivot.y);
	if (IsMyTurn(false))
	{
		m_angleChanged = true;
		SendMsg(this, "Player", 417, (int)&angle, 0, 0, 0);
		SendMsg(this, "Spot", 417, (int)&angle, 0, 0, 0);
		SendMsg(this, "Screen", 417, (int)&angle, 0, 0, 0);
		SendMsg(this, "Grid", 417, (int)&angle, 0, 0, 0);
		SendMsg(this, "Caddie", 417, (int)&angle, 0, 0, 0);
		SendMsg(this, "GuideLine", 417, 0, 0, 0, 0);
	}
}

void CCamera::RotateForGallery(float angle)
{
	if (!((unsigned char)(Doc()->m_myInfo.info.dwIdentity >> 1) & 1))
		return;
	WMatrix matrix = g_camera * RotMat(angle, 1);
	matrix = g_camera;
	WVector& pos = matrix.pivot;
	pos.x = pos.y = pos.z = 0;
	matrix = matrix * RotMat(angle, 1);
	RotateEnvironment(angle);
	if (GOLFDOC()->m_gameMode & 8)
	{
		pos = GOLFDOC()->m_cameraData.mat.pivot;
	}
	else
	{
		pos = GOLFDOC()->m_cameraData.target +
			(g_camera.pivot - GOLFDOC()->m_cameraData.target) *
				RotMat(angle, 1);
		pos = GetPVS().TestCameraCollision(pos);
		if (GOLFDOC()->m_gameMode & 512)
			pos.y = g_camera.pivot.y;
	}
	g_camera = matrix;
	m_pos = g_camera.pivot;
	m_target = g_camera.pivot + g_camera.za;
}
