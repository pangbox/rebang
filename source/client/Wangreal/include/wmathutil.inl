#pragma once

inline float __fastcall RayIntersectAABB(const Waabb& box, const WVector& start,
	const WVector& ray, WPlane* result, float radius)
{
	for (int i = 0; i < 3; i = (i == 0) ? 2 : ((i == 1) ? 3 : 1))
	{
		if (Wabs(ray.p[i]) < g_EPSILON)
			continue;

		float edge;
		if (ray.p[i] > 0.0f)
		{
			if (box.max.p[i] + radius < start.p[i])
				return 1.0f;
			edge = box.min.p[i] - radius;
			if (start.p[i] + ray.p[i] < edge)
				return 1.0f;
			if (start.p[i] > edge)
				continue;
		}
		else
		{
			if (box.min.p[i] - radius > start.p[i])
				return 1.0f;
			edge = box.max.p[i] + radius;
			if (start.p[i] + ray.p[i] > edge)
				return 1.0f;
			if (start.p[i] < edge)
				continue;
		}

		float t = (edge - start.p[i]) / ray.p[i];
		int j;
		for (j = 0; j < 3; ++j)
		{
			if (j == i)
				continue;
			float hit = start.p[j] + t * ray.p[j];
			if (hit < box.min.p[j] - radius || hit > box.max.p[j] + radius)
				break;
		}

		if (j == 3)
		{
			if (result)
			{
				for (int k = 0; k < 3; ++k)
				{
					if (k != i)
					{
						result->normal.p[k] = 0.0f;
					}
					else
					{
						result->normal.p[k] = ray.p[k] < 0.0f ? 1.0f : -1.0f;
						result->dis = -(
							(start.p[k] + t * ray.p[k]) * result->normal.p[k]);
					}
				}
			}

			return t;
		}
	}

	return 1.0f;
}

inline float RayIntersectAABBSolid(const Waabb& box, const WVector& start,
	const WVector& ray, WPlane* result, float radius)
{
	for (int i = 0; i < 3; i = !i ? 2 : (i == 1 ? 3 : 1))
	{
		if (Wabs(ray.p[i]) < 0.00001f)
			continue;

		float plane;
		if (ray.p[i] > 0.0f)
		{
			if (radius + box.max.p[i] < start.p[i] ||
				ray.p[i] + start.p[i] < (plane = box.min.p[i] - radius))
				break;
		}
		else
		{
			if (box.min.p[i] - radius > start.p[i] ||
				ray.p[i] + start.p[i] > (plane = radius + box.max.p[i]))
				break;
		}

		float t = (plane - start.p[i]) / ray.p[i];

		int j;
		for (j = 0; j < 3; j++)
		{
			if (j == i)
				continue;

			float p = t * ray.p[j] + start.p[j];
			if (p < box.min.p[j] - radius || p > radius + box.max.p[j])
				break;
		}

		if (j == 3)
		{
			if (result)
			{
				for (int k = 0; k < 3; k++)
				{
					if (k != i)
						result->normal.p[k] = 0.0f;
					else
					{
						result->normal.p[k] = (ray.p[k] < 0.0f) ? 1.0f : -1.0f;
						result->dis = -(
							(t * ray.p[k] + start.p[k]) * result->normal.p[k]);
					}
				}
			}
			return t;
		}
	}

	return 1.0f;
}

inline float GetBaseAngle(float angle)
{
	if (Abs(angle) > g_2_PI)
		angle -= floorf(angle / g_2_PI) * g_2_PI;
	if (angle > g_PI)
		return angle - g_2_PI;
	if (angle < -g_PI)
		return angle + g_2_PI;
	return angle;
}
