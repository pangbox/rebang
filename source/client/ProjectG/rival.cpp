#include "minatl.h"
#include "projectg.h"
#include "rival.h"
#include "golfdoc.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "autotarget.h"
#include "wind.h"
#include "commonutil.h"

CRival::CRival()
{
	int& state = m_unknown40;
	state = 0;
	m_curve = 0.f;
	m_spin = 0.f;
	m_result = 0;
}

CRival::~CRival()
{
}

void CRival::Shot()
{
	if (WisEqual(m_power, 1.f, g_EPSILON))
		GOLFDOC()->m_bar.bFullPower = true;
	GOLFDOC()->m_bar.power = s_powerBarWidth * m_power + s_barStart;
	GOLFDOC()->m_bar.impact = Random(-10.f, 10.f) + GOLFDOC()->m_bar.center;
	float offset = Abs(GOLFDOC()->m_bar.impact - s_barStart);
	if (offset < 2.f)
		GOLFDOC()->m_bar.grade = 4;
	else if (offset < GolfClub().GetNarrowAccuracy())
		GOLFDOC()->m_bar.grade = 3;
	else if (offset < GolfClub().GetWideAccuracy())
		GOLFDOC()->m_bar.grade = 2;
	else
		GOLFDOC()->m_bar.grade = 1;
}

void CRival::SetResult()
{
	m_resultPos = GolfBall().PreData(GolfBall().m_renderNum)->pos;
	if (!GolfBall().m_bOB && GolfBall().m_standGround > 4)
	{
		m_result = 0;
		m_curve = 0.f;
		m_spin = 0.f;
	}
	else
	{
		if (GetPVS().GetObjColNum() > 0)
			m_result = 1;
		else if (GolfBall().m_water == 2 || GolfBall().m_water == 3)
			m_result = 2;
		else if (GolfBall().m_water == 1)
			m_result = 3;
		else
			m_result = 4;
		m_curve = GolfBall().m_initCurve;
		m_spin = GolfBall().m_initSpin;
	}
}

WVector CRival::GetGreenSlope()
{
	WVector dir;
	dir = GetHoleCupPos() - GolfBall().m_pos;
	dir.y = 0.f;
	int count = (int)(dir.Magnitude() / 3.2f);
	dir.Normalize();
	WVector slope;
	slope.Reset();
	WVector pos = GolfBall().m_pos;
	for (int i = 0; i < count; i++)
	{
		pos += dir * 3.2f;
		WVector normal;
		normal = GetPVS().GetMeshNormal(pos);
		normal.y = 0.f;
		slope += normal;
	}
	return slope;
}

void CRival::SetVariable(CGolfDoc* pDoc)
{
	int mesh;
	WVector offset;
	float maxPower = GetPVS().GetMaxPowerOnGround(GolfBall().m_pos);
	if (m_result)
	{
		GolfClub().SetAccuracy();

		offset = m_resultPos - m_pos;
		GolfClub().SetClub((unsigned char)m_club);
		if (PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)3)
		{
			if (m_result == 1)
			{
				m_club = Min(m_club + 2, 12);
				GolfClub().SetClub((unsigned char)m_club);
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos =
					GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
				return;
			}
		}
		else
		{
			switch (m_result)
			{
			case 1:
				if (PLAYER(GOLFDOC()->m_currentPlayer)->flag > 1)
					m_club = Min(m_club + 3, 12);
				else
					m_club = Min(m_club + 2, 12);
				GolfClub().SetClub((unsigned char)m_club);
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos =
					GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
				return;
			case 2:
				if (PLAYER(GOLFDOC()->m_currentPlayer)->flag == 1)
					goto selectClub;
				break;
			case 3:
				if (PLAYER(GOLFDOC()->m_currentPlayer)->flag == 1)
					goto aim;
				break;
			case 4:
				m_angle = CalcDeltaAngle(m_pos - GolfBall().m_pos - offset,
					WVector::UNIT_POS_Z);
				{
					const WVector& axis =
						WVector::UNIT_POS_Z * ((GolfClub().GetRange()) * 3.2f);
					m_pos = GolfBall().m_pos + axis * RotMat(m_angle, 1);
				}
				break;
			}
			if (PLAYER(GOLFDOC()->m_currentPlayer)->flag > 1)
			{
				m_club = Min(m_club + 3, 12);
				GolfClub().SetClub((unsigned char)m_club);
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos =
					GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
			}
			return;
		}
	}
	else
	{
selectClub:
		GolfClub().ClearPointFactor();
		GolfClub().SelectBestClub();
		m_club = GolfClub().GetIndex();
		while (true)
		{
			m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
			m_pos = GolfBall().m_pos +
				(m_pos - GolfBall().m_pos) *
					(GolfClub().GetType() == 3 ? 1.f : maxPower);

			GetPVS().GetGroundPoint(m_pos, true, &mesh, true);

			if (mesh < 0)
				GolfClub().SetPointFactor(0.f);
			else if (GetPVS().GetGroundType(m_pos, true, NULL) == 4)
				GolfClub().SetPointFactor(.8f);
			else if (GetPVS().GetGroundType(m_pos, true, NULL) == 3)
				GolfClub().SetPointFactor(.6f);
			else
				break;

			GolfClub().SelectBestClub();
			if (GolfClub().GetIndex() == m_club)
				break;
			m_club = GolfClub().GetIndex();
			if (GolfClub().GetIndex() == 12)
			{
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos = GolfBall().m_pos +
					(m_pos - GolfBall().m_pos) *
						(GolfClub().GetType() == 3 ? 1.f : maxPower);
				GetPVS().GetGroundPoint(m_pos, true, &mesh, true);
				if (mesh < 0)
				{
					GolfClub().SetPointFactor(0.f);
					GolfClub().SelectBestClub();
					m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
					m_pos = GolfBall().m_pos +
						(m_pos - GolfBall().m_pos) *
							(GolfClub().GetType() == 3 ? 1.f : maxPower);
					m_club = GolfClub().GetIndex();
				}
				break;
			}
		}
		GolfClub().ClearPointFactor();
	}
aim:
	WVector dir = m_pos - GolfBall().m_pos;
	dir.y = 0.f;
	dir.Normalize();
	if (GolfClub().GetType() == 3)
	{
		WVector slope = GetGreenSlope();
		m_angle = CalcDeltaAngle(m_pos - GolfBall().m_pos - slope * 5.f,
			WVector::UNIT_POS_Z);
		m_pos = GolfBall().m_pos + WVector::UNIT_POS_Z * RotMat(m_angle, 1);
		offset = GetHoleCupPos() - GolfBall().m_pos - slope * 5.f;
		offset.y = 0.f;
		m_power = Min(offset.Magnitude() / (GolfClub().GetRange() * 3.2f), 1.f);
	}
	else
	{
		float windFactor;
		if (GolfClub().GetType() == 2)
		{
			if (GolfClub().GetApproachMode() <= 2)
				windFactor = 1.f;
			else
				windFactor = m_power * 3.f;
		}
		else
			windFactor = 3.f;
		m_angle = CalcDeltaAngle(m_pos - GolfBall().m_pos -
				Wind().GetGlobalWind() * windFactor,
			WVector::UNIT_POS_Z);
		{
			const WVector& axis =
				WVector::UNIT_POS_Z * ((GolfClub().GetRange()) * 3.2f);
			m_pos = GolfBall().m_pos + axis * RotMat(m_angle, 1);
		}
		if (PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
			m_power = 1.f;
		else if (m_result == 3)
		{
			if ((m_resultPos - GolfBall().m_pos).Magnitude() <
				(m_pos - GolfBall().m_pos).Magnitude())
			{
				m_power = .9f;
				GolfBall().m_initSpin = 0.f;
			}
			else
			{
				m_power = 1.f;
				GolfBall().m_initSpin = 1.f;
			}
		}
		else
		{
			offset = GetHoleCupPos() - GolfBall().m_pos -
				Wind().GetGlobalWind() * windFactor;
			offset.y = 0.f;
			m_power =
				Min(offset.Magnitude() / (GolfClub().GetRange() * 3.2f), 1.f);
		}
		WVector rayDir(dir.x, tanf(GolfClub().GetLie()), dir.z);
		offset = rayDir * ((float)(GolfClub().GetRange() * 3.2f) * .5f);
		offset.y -= 32.f;
		float fraction =
			GetPVS().RayIntersectAll(GolfBall().m_pos, offset, true);
		if (fraction < 1.f)
		{
			if (fraction > .5f && WisZero(GolfBall().m_initSpin, g_EPSILON) &&
				WisZero(m_curve, g_EPSILON) &&
				!PLAYER(GOLFDOC()->m_currentPlayer)->flag)
			{
				if (GetPVS().RayIntersectAll(GolfBall().m_pos,
						offset * RotMat(10.f * g_DEGTORAD, 1), true) < 1.f)
					GolfBall().m_initCurve = 1.f;
				else if (GetPVS().RayIntersectAll(GolfBall().m_pos,
							 offset * RotMat(-10.f * g_DEGTORAD, 1),
							 true) < 1.f)
					GolfBall().m_initCurve = -1.f;
				else
					GolfBall().m_initSpin = 1.f;
			}
			else if (fraction < .3f)
			{
				m_club = Min(m_club + 3, 13);
				GolfClub().SetClub((unsigned char)m_club);
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos =
					GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
				dir = m_pos - GolfBall().m_pos;
				dir.y = 0.f;
				dir.Normalize();
				WVector rayDir(dir.x, tanf(GolfClub().GetLie()), dir.z);
				offset = rayDir * ((float)(GolfClub().GetRange() * 3.2f) * .5f);
				offset.y -= 32.f;
				float angle = (PLAYER(GOLFDOC()->m_currentPlayer)->flag + 1) *
					(5.f * g_DEGTORAD);
				if (!(GetPVS().RayIntersectAll(GolfBall().m_pos, offset, true) <
						1.f))
				{
					if (GetPVS().RayIntersectAll(GolfBall().m_pos,
							offset * RotMat(angle, 1), true) < 1.f)
					{
						angle *= -1.f;
						m_pos = GolfBall().m_pos +
							((maxPower * .8f) * (m_pos - GolfBall().m_pos)) *
								RotMat(angle, 1);
					}
					else if (GetPVS().RayIntersectAll(GolfBall().m_pos,
								 offset * RotMat(angle * -1.f, 1), true) < 1.f)
					{
						m_pos = GolfBall().m_pos +
							((maxPower * .8f) * (m_pos - GolfBall().m_pos)) *
								RotMat(angle, 1);
					}
				}
			}
		}
		else
		{
			if (GetPVS().RayIntersectAll(GolfBall().m_pos + offset, offset,
					true) < 1.f)
			{
				m_club = Min(m_club + 1, 12);
				GolfClub().SetClub((unsigned char)m_club);
				m_pos = pDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos =
					GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
			}
		}
	}
}
