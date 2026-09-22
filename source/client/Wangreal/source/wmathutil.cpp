#include "wmemblock.inl"
#include "wmath.h"
#include "wview.h"

static int box_v[8][3] = {
	{ 0, 0, 0 },
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 1, 1, 0 },
	{ 0, 0, 1 },
	{ 1, 0, 1 },
	{ 0, 1, 1 },
	{ 1, 1, 1 },
};

static int box_p[6][4] = {
	{ 0, 2, 3, 1 },
	{ 4, 5, 7, 6 },
	{ 0, 1, 5, 4 },
	{ 3, 2, 6, 7 },
	{ 1, 3, 7, 5 },
	{ 2, 0, 4, 6 },
};

void __cdecl MakePlaneEq(WPlane* out_plane, const Waabb& aabb,
	const WMatrix* matrix)
{
	WVector vt[8];
	if (matrix)
	{
		for (int i = 0; i < 8; ++i)
		{
			vt[i] = WVector(box_v[i][0] ? aabb.max.x : aabb.min.x,
						box_v[i][1] ? aabb.max.y : aabb.min.y,
						box_v[i][2] ? aabb.max.z : aabb.min.z) *
				*matrix;
		}
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			vt[i] = WVector(box_v[i][0] ? aabb.max.x : aabb.min.x,
				box_v[i][1] ? aabb.max.y : aabb.min.y,
				box_v[i][2] ? aabb.max.z : aabb.min.z);
		}
	}

	for (int i = 0; i < 6; i += 2)
	{
		out_plane[i].normal = WCrossProduct(vt[box_p[i][0]] - vt[box_p[i][1]],
			vt[box_p[i][1]] - vt[box_p[i][2]]);
		out_plane[i].dis = -(out_plane[i].normal * vt[box_p[i][3]]);
		out_plane[i + 1].normal = -out_plane[i].normal;
		out_plane[i + 1].dis = -(out_plane[i + 1].normal * vt[box_p[i + 1][3]]);
	}
}

bool __fastcall PlaneFromVecs(WPlane* result, const WVector& first,
	const WVector& second, const WVector& point)
{
	result->normal = WCrossProduct(first, second);
	result->normal.Normalize();
	if (result->normal.SquareMagnitude() == 0.0f)
		return false;
	result->dis = -(result->normal * point);
	return true;
}

float __cdecl RayIntersect(const WVector& pivot, const WVector& vec,
	WPlane* plane, WPlane* out, float rlen)
{
	WVector cross;
	float dotproduct;
	float len;
	float dis;
	int i;
	for (i = 0; i < 6; i += 2)
	{
		WPlane* selected = &plane[i];
		dotproduct =
			selected->y * vec.y + (selected->x * vec.x + selected->z * vec.z);
		float absoluteDot;
		if (*(unsigned long*)&dotproduct & 0x80000000)
			absoluteDot = -dotproduct;
		else
			absoluteDot = dotproduct;
		if (absoluteDot < g_EPSILON)
			continue;
		if (dotproduct > 0.0f)
		{
			++selected;
			dotproduct = -dotproduct;
		}

		dis = *selected * pivot - rlen;
		dotproduct = -dotproduct;
		if (dis > dotproduct)
			return 1.0f;
		if (!(dis >= 0.0f))
			continue;

		len = dis / dotproduct;
		cross = pivot + vec * len;
		int j;
		for (j = 0; j < 6; j += 2)
		{
			if (j == i)
				continue;
			if (plane[j] * cross - rlen > 0.0f ||
				plane[j + 1] * cross - rlen > 0.0f)
				break;
		}

		if (j == 6)
		{
			if (out)
			{
				*out = WPlane(selected->x, selected->y, selected->z,
					selected->dis - rlen);
			}
			return len;
		}
	}
	return 1.0f;
}

float __cdecl ObbIntersect(const Waabb& src_aabb, const WVector& direction,
	const Wobb& dest_obb, WPlane* out_plane)
{
	float t1;
	WVector normal;
	int i;
	WVector vec;
	for (i = 0; i < 3; ++i)
	{
		{
			float absoluteDirection;
			if (*(const unsigned long*)&direction.p[i] & 0x80000000)
				absoluteDirection = -direction.p[i];
			else
				absoluteDirection = direction.p[i];
			if (absoluteDirection < g_EPSILON)
				continue;
		}
		if (direction.p[i] > 0.0f)
		{
			if (dest_obb.IsInclude(src_aabb.min) &&
				dest_obb.IsInclude(src_aabb.min + direction))
				t1 =
					(dest_obb.center.p[i] - src_aabb.max.p[i]) / direction.p[i];
			else
				continue;
		}
		else
		{
			if (dest_obb.IsInclude(src_aabb.max) &&
				dest_obb.IsInclude(WVector(
					*(volatile const float*)&src_aabb.max.x + direction.x,
					src_aabb.max.y + direction.y,
					src_aabb.max.z + direction.z)))
				t1 =
					(dest_obb.center.p[i] - src_aabb.min.p[i]) / direction.p[i];
			else
				continue;
		}
		if (t1 < 0.0f)
		{
			if (t1 > -0.001f)
				t1 = 0.0f;
		}
		if (t1 >= 0.0f)
		{
			vec = direction * t1;
			int j;
			for (j = 0; j < 3; ++j)
			{
				if (j == i)
					continue;
				if (src_aabb.max.p[j] + vec.p[j] < dest_obb.center.p[j] ||
					*(volatile float*)&vec.p[j] + src_aabb.min.p[j] >
						dest_obb.center.p[j])
					break;
			}
			if (j == 3)
			{
				if (out_plane)
				{
					normal.Reset();
					normal.p[i] = direction.p[i] < 0.0f ? 1.0f : -1.0f;
					*out_plane = WPlane(normal,
						vec.p[i] < 0.0f ? WVector(vec.x + src_aabb.min.x,
											  vec.y + src_aabb.min.y,
											  vec.z + src_aabb.min.z)
										: WVector(vec.x + src_aabb.max.x,
											  vec.y + src_aabb.max.y,
											  vec.z + src_aabb.max.z));
				}
				return t1;
			}
		}
	}
	return 1.0f;
}

float __cdecl RayIntersectAABB(const Waabb& box, const WVector& start,
	const WVector& ray, WMatrix* matrix, WPlane* result, float radius)
{
	if (!matrix)
		return RayIntersectAABB(box, start, ray, result, radius);

	WPlane planes[6];
	MakePlaneEq(planes, box, matrix);
	return RayIntersect(start, ray, planes, result, radius);
}

float __cdecl AabbIntersect(const Waabb& src_aabb, const WVector& direction,
	const Waabb& dest_aabb, WPlane* out_plane)
{
	float t1;
	WVector normal;
	int i;
	WVector vec;
	for (i = 0; i < 3; ++i)
	{
		float absoluteDirection;
		if (*(const unsigned long*)&direction.p[i] & 0x80000000)
			absoluteDirection = -direction.p[i];
		else
			absoluteDirection = direction.p[i];
		if (absoluteDirection < g_EPSILON)
			continue;
		if (direction.p[i] > 0.0f)
		{
			if (src_aabb.min.p[i] > dest_aabb.max.p[i] ||
				src_aabb.max.p[i] + direction.p[i] < dest_aabb.min.p[i])
				continue;
			t1 = (dest_aabb.min.p[i] - src_aabb.max.p[i]) / direction.p[i];
		}
		else
		{
			if (src_aabb.min.p[i] + direction.p[i] > dest_aabb.max.p[i] ||
				dest_aabb.min.p[i] > src_aabb.max.p[i])
				continue;
			t1 = (dest_aabb.max.p[i] - src_aabb.min.p[i]) / direction.p[i];
		}
		if (t1 < 0.0f)
		{
			if (t1 > -0.001f)
				t1 = 0.0f;
		}
		if (!(t1 >= 0.0f))
			continue;
		vec = direction * t1;
		int j;
		for (j = 0; j < 3; ++j)
		{
			if (j == i)
				continue;
			if (src_aabb.max.p[j] + vec.p[j] < dest_aabb.min.p[j] ||
				src_aabb.min.p[j] + vec.p[j] > dest_aabb.max.p[j])
				break;
		}
		if (j == 3)
		{
			if (out_plane)
			{
				normal.Reset();
				normal.p[i] = direction.p[i] < 0.0f ? 1.0f : -1.0f;
				*out_plane = WPlane(normal,
					vec.p[i] < 0.0f
						? WVector(vec.x + src_aabb.min.x,
							  vec.y + src_aabb.min.y, vec.z + src_aabb.min.z)
						: WVector(vec.x + src_aabb.max.x,
							  vec.y + src_aabb.max.y, vec.z + src_aabb.max.z));
			}
			return t1;
		}
	}
	return 1.0f;
}

float __cdecl DotContact(const WVector& vec, const WPlane* plane)
{
	float max, d;
	for (int i = 0; i < 3; ++i)
	{
		d = vec * plane[i * 2];
		if (i == 0)
			max = d;
		else if (d > max)
			max = d;
		if (!(d > 0.0f))
		{
			d = vec * plane[i * 2 + 1];
			if (d > max)
				max = d;
		}
	}
	return max;
}

WMatrix __cdecl Make3rdCamMatrix(const WVector& position, const WVector& target)
{
	WMatrix result;
	result.za = target - position;
	result.za.Normalize();
	result.ya = WVector::UNIT_POS_Y;
	result.xa = WCrossProduct(result.ya, result.za);
	result.xa.Normalize();
	result.ya = WCrossProduct(result.za, result.xa);
	result.ya.Normalize();
	result.pivot = position;
	return result;
}

WRect __cdecl ScaleRect(WView* view, WRect* rectangle)
{
	WRect result;
	result.x = view->GetWidth() * rectangle->x / 640.0f;
	result.y = view->GetHeight() * rectangle->y / 480.0f;
	result.w = view->GetWidth() * rectangle->w / 640.0f;
	result.h = view->GetHeight() * rectangle->h / 480.0f;
	return result;
}

void __cdecl ScalePoint(WView* view, WPoint* point)
{
	point->x = view->GetHeight() * point->x / 640.0f;
	point->y = view->GetHeight() * point->y / 480.0f;
}
