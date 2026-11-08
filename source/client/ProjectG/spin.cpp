#include "projectg.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "clientsetting.h"
#include "mousecursor.h"
#include "quitwindow.h"
#include "itemwindow.h"
#include "spin.h"
#include "../../shared/localize.h"

IMPLEMENT_ACTOR(CSpin, IActor)

CSpin::CSpin()
{
	m_dispPriority = DISP_PRIORITY_4;
	AddSkipList(0x400);
}

void CSpin::OnPreLoadInit()
{
	m_centerY = COption::Instance()->gGetBar_Y() + 4.0f;
	m_centerX = s_barStart - 82.0f;
}

void CSpin::Reset()
{
	m_worldX = m_centerX;
	m_worldY = m_centerY;
	m_hitX = 0.0f;
	m_hitY = 0.0f;
	m_autoOffsetX = 0.0f;
	m_autoOffsetY = 0.0f;
	m_autoX = m_autoOffsetX + m_centerX;
	m_autoY = m_autoOffsetY + m_centerY;
	m_bAutoPoint = false;
	m_bClicked = false;
	m_bLoop = false;
	m_direction = 2;
	m_nextDirection = 2;
	m_loopType = 0;
	m_bUnused = false;
	m_color = 0xffffffff;
	m_bActive = false;
	m_bLock = false;
}

void CSpin::OnInit()
{
	Reset();
}

void CSpin::ProcessAutoPoint(float delta)
{
	if (!m_bAutoPoint)
		return;

	float x = m_autoOffsetX;
	float y = m_autoOffsetY;
	ProcessLoopHitPoint(delta, x, y, delta);

	m_autoOffsetX = x;
	m_autoX = x + m_centerX;
	m_autoOffsetY = y;
	m_autoY = y + m_centerY;
}

void CSpin::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode < 12)
		return;
	if (CQuitWindow::Instance()->IsActive())
		return;
	if (!CItemWindow::IsClosed())
		return;
	if (GolfClub().GetType() == 3)
		return;
	if (!m_bActive)
		return;

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000000 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000011 ||
		GOLFDOC()->m_tutorialMode == 12)
		m_rangeY = 30.0f;
	else
		m_rangeY = (float)(int)PLAYER(GOLFDOC()->m_currentPlayer)->spin;

	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000001 ||
		GOLFDOC()->m_tutorialMode == 12)
		m_rangeX = 30.0f;
	else
		m_rangeX = (float)(int)PLAYER(GOLFDOC()->m_currentPlayer)->curve;

	if (m_bLoop == true)
		ProcessAutoPoint(delta);

	if (m_bLock)
		return;

	float x = g_input->GetMousePoint().x - m_centerX;
	float y = g_input->GetMousePoint().y - m_centerY;

	if (CMouseCursor::Instance()->GetArea() == 3 &&
		CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) &&
		g_input->GetButton(LEFT_BUTTON))
	{
		if (IsInclude(x, y))
		{
			m_hitX = x;
			m_hitY = y;
			m_color = 0xffffffff;
		}
		else if (WisZero(x))
		{
			m_hitX = 0.0f;
			m_hitY = (y > 0.0f ? 1.0f : -1.0f) * m_rangeY;
			m_color = 0xa0ffffff;
		}
		else
		{
			float slope = y / x;
			float hx = m_rangeY /
				sqrtf(
					m_rangeX * m_rangeX * slope * slope + m_rangeY * m_rangeY) *
				m_rangeX;

			if (slope > (float)tan(-20.0 * g_DEGTORAD) &&
				slope < (float)tan(20.0 * g_DEGTORAD))
			{
				m_hitX = (x > 0.0f ? 1.0f : -1.0f) * m_rangeX;
				m_hitY = 0.0f;
				m_color = 0xa0ffffff;
			}
			else if (slope > (float)tan(g_PI / 2.0f - 20.0 * g_DEGTORAD) ||
				slope < (float)tan(g_PI / 2.0f + 20.0 * g_DEGTORAD))
			{
				m_hitX = 0.0f;
				m_hitY = y > 0.0f ? m_rangeY : m_rangeY * -1.0f;
				m_color = 0xa0ffffff;
			}
			else
			{
				m_hitX = x > 0.0f ? hx : hx * -1.0f;
				m_color = 0xa0ffffff;
				m_hitY = m_hitX * slope;
			}
		}
	}

	if (GOLFDOC()->m_tutorialMode == 12 && m_bClicked && IsPointInAutoPoint())
	{
		m_bClicked = false;
		IActor* pActor = GetActor("Tutorial");
		if (pActor)
			pActor << MsgObject(NULL, 329, 0, 0, 0, 0, 0);
	}

	if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) &&
		!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))
	{
		float step = delta * g_PI;
		x = m_hitX;
		y = m_hitY;

		if (g_input->Get("\xbb\xf3", true))
		{
			float limit =
				sqrtf(1.0f - x * x / (m_rangeX * m_rangeX)) * m_rangeY * -1.0f;
			if (y == limit)
			{
				float angle = acosf(Between(-1.0f, x / m_rangeX, 1.0f));
				if (x > 0.0f)
					m_angle = Max(angle * -1.0f - step, -g_PI / 2.0f);
				else
					m_angle = Min(step - angle, -g_PI / 2.0f);
				x = cosf(m_angle) * m_rangeX;
				y = sinf(m_angle) * m_rangeY;
			}
			else
			{
				y -= delta * 50.0f;
				if (y < limit)
				{
					y = limit;
					m_color = 0xa0ffffff;
				}
				else
					m_color = 0xffffffff;
			}
		}

		if (g_input->Get("\xc7\xcf", true))
		{
			float limit =
				sqrtf(1.0f - x * x / (m_rangeX * m_rangeX)) * m_rangeY;
			if (y == limit)
			{
				float angle = acosf(Between(-1.0f, x / m_rangeX, 1.0f));
				if (x < 0.0f)
					m_angle = Max(angle - step, g_PI / 2.0f);
				else
					m_angle = Min(angle + step, g_PI / 2.0f);
				x = cosf(m_angle) * m_rangeX;
				y = sinf(m_angle) * m_rangeY;
			}
			else
			{
				y += delta * 50.0f;
				if (y > limit)
				{
					y = limit;
					m_color = 0xa0ffffff;
				}
				else
					m_color = 0xffffffff;
			}
		}

		if (g_input->Get("\xc1\xc2", true))
		{
			float limit =
				sqrtf(1.0f - y * y / (m_rangeY * m_rangeY)) * m_rangeX * -1.0f;
			if (x == limit)
			{
				float angle = acosf(Between(-1.0f, x / m_rangeX, 1.0f));
				if (y > 0.0f)
					m_angle = Min(angle + step, g_PI);
				else
					m_angle = Max(angle * -1.0f - step, -g_PI);
				x = cosf(m_angle) * m_rangeX;
				y = sinf(m_angle) * m_rangeY;
			}
			else
			{
				x -= delta * 50.0f;
				if (x < limit)
				{
					x = limit;
					m_color = 0xa0ffffff;
				}
				else
					m_color = 0xffffffff;
			}
		}

		if (g_input->Get("\xbf\xec", true))
		{
			float limit =
				sqrtf(1.0f - y * y / (m_rangeY * m_rangeY)) * m_rangeX;
			if (x == limit)
			{
				float angle = acosf(Between(-1.0f, x / m_rangeX, 1.0f));
				if (y < 0.0f)
					m_angle = Min(step - angle, 0.0f);
				else
					m_angle = Max(angle - step, 0.0f);
				x = cosf(m_angle) * m_rangeX;
				y = sinf(m_angle) * m_rangeY;
			}
			else
			{
				x += delta * 50.0f;
				if (x > limit)
				{
					x = limit;
					m_color = 0xa0ffffff;
				}
				else
					m_color = 0xffffffff;
			}
		}

		m_hitX = x;
		m_hitY = y;
		ConvertToWorld(x, y);
	}
	else
		ConvertToWorld(m_hitX, m_hitY);
}

void CSpin::GetNextDirection()
{
	switch (m_loopType)
	{
	case 0:
		m_direction = 0;
		break;

	case 1:
		if (m_nextDirection == 5)
			m_nextDirection = 2;
		else
			m_nextDirection++;
		break;

	case 2:
		if (m_direction == 3)
			m_nextDirection = 5;
		else if (m_direction == 5)
			m_nextDirection = 3;
		break;
	}
}

void CSpin::ProcessLoopHitPoint(float delta, float& x, float& y, float& z)
{
	float limit;

	switch (m_direction)
	{
	case 1:
		if (y < 0.0f)
		{
			y += delta * 50.0f;
			if (y >= 0.0f)
			{
				GetNextDirection();
				m_direction = m_nextDirection;
				y = 0.0f;
			}
		}
		else if (x < 0.0f)
		{
			x += delta * 50.0f;
			if (x >= 0.0f)
			{
				GetNextDirection();
				m_direction = m_nextDirection;
				x = 0.0f;
			}
		}
		else if (y > 0.0f)
		{
			y -= delta * 50.0f;
			if (y <= 0.0f)
			{
				GetNextDirection();
				m_direction = m_nextDirection;
				y = 0.0f;
			}
		}
		else
		{
			x -= delta * 50.0f;
			if (x <= 0.0f)
			{
				GetNextDirection();
				m_direction = m_nextDirection;
				x = 0.0f;
			}
		}
		break;

	case 2:
		limit = sqrtf(1.0f - x * x * 0.0025f) * -20.0f;
		if (y == limit)
			m_direction = m_nextDirection != 0;
		else
		{
			y -= delta * 50.0f;
			if (y < limit)
				y = limit;
		}
		break;

	case 3:
		limit = sqrtf(1.0f - y * y * 0.0025f) * -20.0f;
		if (x == limit)
			m_direction = m_nextDirection != 0;
		else
		{
			x -= delta * 50.0f;
			if (x < limit)
				x = limit;
		}
		break;

	case 4:
		limit = sqrtf(1.0f - x * x * 0.0025f) * 20.0f;
		if (y == limit)
			m_direction = m_nextDirection != 0;
		else
		{
			y += delta * 50.0f;
			if (y > limit)
				y = limit;
		}
		break;

	case 5:
		limit = sqrtf(1.0f - y * y * 0.0025f) * 20.0f;
		if (x == limit)
			m_direction = m_nextDirection != 0;
		else
		{
			x += delta * 50.0f;
			if (x > limit)
				x = limit;
		}
		break;
	}
}

void CSpin::ConvertToWorld(float x, float y)
{
	m_worldX = x + m_centerX;
	m_worldY = y + m_centerY;
}

bool CSpin::IsPointInAutoPoint()
{
	if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
		return false;

	if (fabs(m_autoX - m_centerX) < 0.6 && fabs(m_autoY - m_centerY) < 0.6)
		return false;

	if (fabs(m_autoX - m_worldX) < 2.0f && fabs(m_autoY - m_worldY) < 2.0f)
		return true;

	return false;
}

void CSpin::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 353:
		if (GolfClub().GetType() == 3)
		{
			*(float*)msg.param1 = 0.0f;
			*(float*)msg.param2 = 0.0f;
		}
		else
		{
			*(float*)msg.param1 = m_hitX * 0.033333335f;
			*(float*)msg.param2 = m_hitY * 0.033333335f;
		}
		break;

	case 355:
		m_bActive = true;
		break;

	case 356:
		m_bActive = false;
		break;

	case 357:
		switch (msg.param1)
		{
		case 0:
			m_hitX = Between(-1.0f, GolfBall().m_initCurve, 1.0f) * 30.0f;
			m_hitY = Between(-1.0f, GolfBall().m_initSpin, 1.0f) * 30.0f;
			break;

		case 1:
			m_hitX = (float)msg.param2;
			m_hitY = (float)msg.param3;
			break;

		case 2:
			m_autoOffsetX = (float)msg.param2;
			m_autoOffsetY = (float)msg.param3;
			m_autoX = m_centerX + m_autoOffsetX;
			m_autoY = m_centerY + m_autoOffsetY;
			return;
		}

		{
			bool bInclude = IsInclude(m_hitX, m_hitY);
			m_bActive = true;
			ConvertToWorld(m_hitX, m_hitY);
			m_color = bInclude ? 0xffffffff : 0xa0ffffff;
		}
		break;

	case 359:
		*(unsigned long*)msg.param3 = m_bActive ? m_color : 0;
	case 358:
		*(float*)msg.param1 = m_worldX;
		*(float*)msg.param2 = m_worldY;
		break;

	case 360:
		*(float*)msg.param1 = m_autoX;
		*(float*)msg.param2 = m_autoY;
		*(unsigned long*)msg.param3 = m_color;
		break;

	case 354:
		*(float*)msg.param1 = m_hitX / m_rangeX;
		*(float*)msg.param2 = m_hitY / m_rangeY;
		break;

	case 361:
		if (msg.param1 < 2)
			m_bAutoPoint = msg.param1 == 1;
		else if (msg.param1 == 3)
			m_bLock = true;
		else if (msg.param1 == 4)
			m_bLock = false;
		else
			m_bClicked = true;
		break;

	case 362:
		m_bLoop = !m_bLoop;
		break;

	case 363:
		m_direction = msg.param1;
		m_nextDirection = msg.param2;
		m_loopType = msg.param3;
		break;

	case 10010:
		Reset();
		break;

	case 10011:
		m_centerY = COption::Instance()->gGetBar_Y() + 4.0f;
		ConvertToWorld(m_hitX, m_hitY);
		break;
	}
}

bool CSpin::IsInclude(float x, float y)
{
	return x * x / (m_rangeX * m_rangeX) + y * y / (m_rangeY * m_rangeY) < 1.0f
		? true
		: false;
}
