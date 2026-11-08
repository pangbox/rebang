#include "util.h"

static bool IsZero(float f)
{
	return f < g_EPSILON;
}

void Ut::MakeObjectName(std::string& name, const char* prefix,
	const IObject& obj, int index)
{
	name.clear();

	if (prefix)
	{
		name = prefix;
		name += "_";
	}
	name += obj.GetRTTI()->GetName();
	name += MakeStr("_%06d", index);
}

void Ut::BuildAppropriatePosList(
	const std::vector<WPolySoup::sTriangle*>* triangles, float interval,
	std::vector<WVector>& posList)
{
	std::vector<WPolySoup::sTriangle*>::const_iterator it = triangles->begin();
	while (it != triangles->end())
	{
		WVector v0 = (*it)->v[0];
		WVector v1 = (*it)->v[1];
		WVector v2 = (*it)->v[2];
		WVector center = (v0 + v1 + v2) * (1.0f / 3.0f);

		bool bAppropriate = true;

		std::vector<WVector>::iterator pos = posList.begin(),
									   end = posList.end();
		while (pos != end)
		{
			if ((center - *pos).SquareMagnitude() < interval * interval)
			{
				bAppropriate = false;
				break;
			}

			++pos;
		}

		if (bAppropriate)
			posList.push_back(center);

		++it;
	}
}

char* Ut::GenDateTimeStrA(char* buf, const char* format)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(buf, format, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		st.wSecond, st.wMilliseconds);

	return buf;
}

gaQ Ut::GetRotationQuaternion(const gaV3& a, const gaV3& b,
	const gaV3& fallbackAxis)
{
	gaQ q;

	gaV3 v0 = a;
	gaV3 v1 = b;
	v0.Normalize();
	v1.Normalize();

	float d = v0.Dot(v1);

	if (d >= gaMath::ID)
	{
		return gaQ::ID;
	}

	if (d < (gaMath::TOLERANCE - gaMath::ID))
	{
		if (fallbackAxis != gaV3::ZERO)
		{
			q.FromAxisAngle(fallbackAxis, gaMath::PI);
		}
		else
		{
			gaV3 axis = gaV3::UNIT_X.Cross(a);
			if (gaMath::NearlyZero(axis.Length(),
					gaMath::TOLERANCE * gaMath::TOLERANCE))
				axis = gaV3::UNIT_Y.Cross(a);
			q.FromAxisAngle(axis, gaMath::PI);
		}
	}
	else
	{
		float s = gaMath::Sqrt((gaMath::ID + d) * gaMath::TWO);
		float invs = gaMath::ID / s;

		gaV3 c = v0.Cross(v1);

		q.x = c.x * invs;
		q.y = c.y * invs;
		q.z = c.z * invs;
		q.w = gaMath::HALF * s;
		q.Normalize();
	}
	return q;
}
