#include "autotarget.h"
#include "club.h"
#include "quadtree.h"

CAutoTarget::~CAutoTarget()
{
	Reset();
}

void CAutoTarget::Reset()
{
	m_pointList.clear();
}

float CAutoTarget::GetAngle()
{
	return CalcDeltaAngle(GetPoint(NULL) - GolfBall().m_pos,
		WVector::UNIT_POS_Z);
}

float CAutoTarget::InArea(const WVector& a, const WVector& b,
	float radius) const
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z)) -
		radius;
}

int CAutoTarget::CalcDropPos(const WVector& pos)
{
	if (GetPVS().GetGroundType(pos, true, NULL) < 3)
	{
		unsigned char player = GOLFDOC()->m_currentPlayer;
		m_dropPos = GOLFDOC()->GetPlayer(player)->startPos;
		return 3;
	}
	if (GOLFDOC()->m_pPolySoup->IsOutOfBound(pos))
	{
		unsigned char player = GOLFDOC()->m_currentPlayer;
		m_dropPos = GOLFDOC()->GetPlayer(player)->startPos;
		return 4;
	}
	float distance = 3200.0f;
	WVector direction;
	for (unsigned char i = 0; i < m_pointList.size() - 1; ++i)
	{
		WVector nearest;
		nearest = GetNearest(i);
		float length = (nearest - pos).Magnitude();
		if (length < distance)
		{
			distance = length;
			direction = nearest - pos;
			direction.y = 0.0f;
		}
	}
	distance = 32.0f;
	direction.Normalize();
	for (; distance < 192.0f; distance += 32.0f)
	{
		unsigned char ground =
			GetPVS().GetGroundType(pos + direction * distance, true, NULL);
		if (ground == 10)
		{
			unsigned char player = GOLFDOC()->m_currentPlayer;
			m_dropPos = GOLFDOC()->GetPlayer(player)->startPos;
			return 2;
		}
		if (ground == 1)
		{
			m_dropPos = GetPVS().GetGroundPoint(pos + direction * distance,
				false, NULL, false);
			m_dropPos.y += GolfBall().GetRadius();
			return 1;
		}
	}
	unsigned char player = GOLFDOC()->m_currentPlayer;
	m_dropPos = GOLFDOC()->GetPlayer(player)->startPos;
	return 0;
}

WVector CAutoTarget::GetNearest(unsigned char index)
{
	float t = 0.0f;
	float nearest = 1.0f;
	float distance = 3200.0f;
	float step = 1.0f /
		(float)ceil(
			(double)((m_pointList[index + 1] - m_pointList[index]).Magnitude() /
				16.0f));
	do
	{
		WVector pos;
		pos = (1.0f - t) * m_pointList[index] + t * m_pointList[index + 1];
		if (GetPVS().GetGroundType(pos, true, NULL) == 1)
		{
			float length = (GolfBall().m_pos - pos).Magnitude();
			if (length < distance)
			{
				nearest = t;
				distance = length;
			}
		}
		t += step;
	} while (t < 1.0f);
	return (1.0f - nearest) * m_pointList[index] +
		nearest * m_pointList[index + 1];
}

float CAutoTarget::GetDistance(const WVector& a, const WVector& b)
{
	float x = a.x - b.x;
	float z = a.z - b.z;
	return sqrtf((float)(x * x) + z * z);
}

WVector CAutoTarget::GetPoint(const char* club)
{
	float range = GolfClub().GetRange(club);
	range *= 3.2f;
	if (club)
	{
		if (GolfClub().GetApproachMode() < 4 ||
			InArea(GolfBall().m_pos, GetHoleCupPos(), range) < 96.0f)
			return GetHoleCupPos();
	}
	else
	{
		if ((GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 0 ||
				(GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) == 0 &&
					GOLFDOC()->GetHoleData().par < 5)) &&
			(GolfClub().GetApproachMode() < 4 ||
				InArea(GolfBall().m_pos, GetHoleCupPos(), range) < 32.0f))
			return GetHoleCupPos();
	}

	for (int i = m_pointList.size() - 2; i >= 0; --i)
	{
		if (InArea(GolfBall().m_pos, m_pointList[i], range) *
				InArea(GolfBall().m_pos, m_pointList[i + 1], range) <
			0.0f)
		{
			WVector point =
				SolveEquation((unsigned char)i, range) + GolfBall().m_pos;
			WVector directions[2];
			directions[0] = point - GolfBall().m_pos;
			const WVector& target = GetHoleCupPos();
			directions[1] = target - GolfBall().m_pos;
			if (directions[0].x * directions[1].x +
					directions[0].z * directions[1].z >
				0.0f)
			{
				WVector delta = directions[1] - directions[0];
				delta.y = 0.0f;
				WVector2D targetXZ(directions[1].x, directions[1].z);
				if (delta.Magnitude() <
					sqrtf(targetXZ.x * targetXZ.x + targetXZ.y * targetXZ.y))
				{
					point.y = 0.0f;
					return point;
				}
			}
		}
	}
	return GetHoleCupPos();
}

WVector CAutoTarget::SolveEquation(unsigned char index, float dist)
{
	WVector points[2];
	points[0] = m_pointList[index] - GolfBall().m_pos;
	points[1] = m_pointList[index + 1] - GolfBall().m_pos;
	float x[2], z[2];
	if (Wabs(points[0].x - points[1].x) < g_EPSILON)
	{
		x[0] = x[1] = points[0].x;
		z[0] = sqrtf(dist * dist - points[0].x * points[0].x);
		z[1] = z[0] * -1.0f;
	}
	else
	{
		float slope = (points[1].z - points[0].z) / (points[1].x - points[0].x);
		float intercept = points[0].z - slope * points[0].x;

		float root =
			sqrtf(dist * (slope * slope + 1.0f) * dist - intercept * intercept);
		x[0] = (root - intercept * slope) / (slope * slope + 1.0f);
		x[1] = (intercept * slope * -1.0f - root) / (slope * slope + 1.0f);
		z[0] = slope * x[0] + intercept;
		z[1] = slope * x[1] + intercept;
	}
	WVector result;
	if (x[0] >= Min(points[0].x, points[1].x) &&
		x[0] <= Max(points[0].x, points[1].x))
	{
		result.x = x[0];
		result.z = z[0];
	}
	else
	{
		result.x = x[1];
		result.z = z[1];
	}
	return result;
}
