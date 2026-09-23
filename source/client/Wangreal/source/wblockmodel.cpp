#include "wmemblock.inl"
#include "wblockmodel.h"
#include "wview.h"

unsigned long WBlockModel::g_treemask[8] = { 0x15, 0x16, 0x19, 0x1a, 0x25, 0x26,
	0x29, 0x2a };

WBlockModel::WBlockModel()
{
	m_root = 0;
}

WBlockModel::~WBlockModel()
{
	Reset();
}

void WBlockModel::Reset()
{
	WConvexArea* area = m_colarea.Start();
	while (area)
	{
		delete[] area;
		area = m_colarea.Next();
	}
	m_colarea.Reset();
}

void WBlockModel::Build()
{
	m_root = ExtendTree(&m_colarea);
}

int __fastcall WBlockModel::CalcDepth(WOctree* node)
{
	int depth = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (node->mask & (1 << i))
		{
			int child = CalcDepth(node->leaf[i]);
			if (child > depth)
				depth = child;
		}
	}
	return depth + 1;
}

int WBlockModel::PositionDecision(int flag)
{
	unsigned int position = 0;
	for (int i = 0; i < 6; i += 2)
	{
		unsigned int both = 3 << i;
		if ((both & flag) == both)
			return 8;
		if ((flag & (1 << i)) == 0)
			position |= 1 << (i / 2);
	}
	return position;
}

WVector WBlockModel::FixedPivot(WList<WConvexArea*>* list, WVector pivot)
{
	for (int i = 0; i < 3; ++i)
	{
		WVector fixed_pivot = pivot;
		WConvexArea* area = list->Start();
		int cnt = 0;
		for (; area; area = list->Next())
		{
			if (((CalcFlag(area->aabb, pivot) >> (i * 2)) & 3) == 3)
			{
				float fixed = Abs(area->aabb.min.p[i] - pivot.p[i]) <
						Abs(area->aabb.max.p[i] - pivot.p[i])
					? area->aabb.min.p[i] - 0.0001f
					: area->aabb.max.p[i] + 0.0001f;
				if (cnt == 0 ||
					Abs(fixed - pivot.p[i]) <
						Abs(fixed_pivot.p[i] - pivot.p[i]))
					fixed_pivot.p[i] = fixed;
				++cnt;
			}
		}
		area = list->Start();
		int fixed_cnt = 0;
		for (; area; area = list->Next())
		{
			if (((CalcFlag(area->aabb, fixed_pivot) >> (i * 2)) & 3) == 3)
				++fixed_cnt;
		}
		if (fixed_cnt < cnt)
			pivot = fixed_pivot;
	}
	return pivot;
}

WBlockModel::WOctree* WBlockModel::ExtendTree(WList<WConvexArea*>* list)
{
	WList<WConvexArea*> sublist[9];
	WVector pivot = WVector::ZERO;
	WConvexArea* area = list->Start();
	int cnt = 0;
	for (; area; area = list->Next(), ++cnt)
	{
		pivot += (area->aabb.min + area->aabb.max) * 0.5f;
	}
	if (!cnt)
		return 0;
	pivot *= 1.0f / cnt;
	for (area = list->Start(); area; area = list->Next())
	{
		if (CalcFlag(area->aabb, pivot) != 0x3f)
		{
			pivot = FixedPivot(list, pivot);
			break;
		}
	}
	area = list->Start();
	int childcnt = 0;
	cnt = 0;
	int mask = 0;
	for (; area; area = list->Next(), ++cnt)
	{
		int slot = PositionDecision(CalcFlag(area->aabb, pivot));
		sublist[slot] += area;
		if (slot < 8)
		{
			++childcnt;
			mask |= 1 << slot;
		}
	}
	int i = 0;
	WOctree* tree = (WOctree*)new char[sizeof(WOctree) +
		(cnt - childcnt - 1) * sizeof(WOctree::FruitList)];
	tree->mask = (unsigned char)mask;
	tree->listnum = cnt - childcnt;
	tree->pivot = pivot;
	tree->check = 0;
	for (area = sublist[8].Start(); area; area = sublist[8].Next(), ++i)
	{
		tree->list[i].area = area;
		tree->list[i].flag = CalcFlag(area->aabb, pivot);
	}
	for (i = 0; i < 8; ++i)
		tree->leaf[i] = ExtendTree(&sublist[i]);
	return tree;
}

void WBlockModel::Add(WConvexArea* area)
{
	if (!m_colarea.Start())
	{
		m_colbound = area->aabb;
	}
	else
	{
		m_colbound = ExtendAABB(m_colbound, area->aabb);
	}
	m_colarea += area;
}

inline Waabb WBlockModel::ExtendAABB(const Waabb& a, const Waabb& b)
{
	return Waabb(WVector(Min(a.min.x, b.min.x), Min(a.min.y, b.min.y),
					 Min(a.min.z, b.min.z)),
		WVector(Max(a.max.x, b.max.x), Max(a.max.y, b.max.y),
			Max(a.max.z, b.max.z)));
}

inline Waabb WBlockModel::ExtendAABB(const WVector& a, const WVector& b,
	float rlen)
{
	return Waabb(WVector(Min(a.x, b.x) - rlen, Min(a.y, b.y) - rlen,
					 Min(a.z, b.z) - rlen),
		WVector(Max(a.x, b.x) + rlen, Max(a.y, b.y) + rlen,
			Max(a.z, b.z) + rlen));
}

inline bool WBlockModel::ColTestRough(const WVector& pos, const WVector& vec,
	float rlen, const Waabb& aabb)
{
	for (int i = 0; i < 3; ++i)
	{
		if (vec.p[i] > 0.0f)
		{
			if (pos.p[i] + vec.p[i] + rlen < aabb.min.p[i] ||
				pos.p[i] - rlen > aabb.max.p[i])
				return false;
		}
		else
		{
			if (pos.p[i] + vec.p[i] - rlen > aabb.max.p[i] ||
				pos.p[i] + rlen < aabb.min.p[i])
				return false;
		}
	}
	return true;
}

float WBlockModel::CollTest(const WVector& pos, const WVector& vec, float rlen,
	WPlane* out, WConvexArea* area, float min)
{
	float t[3];
	WVector contact;
	for (int i = 0; i < area->count; ++i)
	{
		t[0] = area->plane[i] * pos - rlen;
		t[1] = area->plane[i] * (pos + vec) - rlen;
		if (t[0] > 0.0f && t[1] > 0.0f)
			return min;
		if (t[0] >= 0.0f && t[1] <= 0.0f && t[0] > t[1])
		{
			t[2] = t[0] / (t[0] - t[1]);
			if (min > t[2])
			{
				contact = pos + vec * t[2];
				int j;
				for (j = 0; j < area->count; ++j)
				{
					if (i != j)
					{
						const WPlane& plane = area->plane[j];
						if (plane.normal * contact + plane.dis - rlen > 0.0f)
							break;
					}
				}
				if (j == area->count)
				{
					if (out)
						*out = area->plane[i];
					return t[2];
				}
			}
		}
	}
	return min;
}

float WBlockModel::CollTest(const WVector& pos, const WVector& move,
	float radius, WPlane* hit)
{
	float best = 1.0f;
	if (ColTestRough(pos, move, radius, m_colbound))
	{
		if (m_root)
		{
			return Explore(m_root, ExtendAABB(pos, pos + move, radius), pos,
				move, radius, hit, 1.0f);
		}
		for (WConvexArea* area = m_colarea.Start(); area;
			area = m_colarea.Next())
		{
			if (ColTestRough(pos, move, radius, area->aabb))
				best = CollTest(pos, move, radius, hit, area, best);
		}
	}
	return best;
}

bool WBlockModel::IsConvex(const WVector* veclist, int vtxnum, WPlane* out)
{
	for (int i = 0; i < vtxnum; i += 3)
	{
		WPlane plane;
		plane = WPlane(WCrossProduct(veclist[i + 1] - veclist[i],
						   veclist[i + 2] - veclist[i])
						   .Normalize(),
			veclist[i]);
		for (int j = 0; j < vtxnum; ++j)
		{
			float t = plane * veclist[j];
			if (t > 0.001f)
			{
				*out = plane;
				return false;
			}
		}
	}
	return true;
}

void WBlockModel::AddTriangleList(const WVector* veclist, int vtxnum)
{
	WPlane plist[32];
	WPlane plane;
	Waabb aabb;
	int n = 0;
	IsConvex(veclist, vtxnum, &plane);
	for (int i = 0; i < vtxnum;)
	{
		plane = WPlane(WCrossProduct(veclist[i + 1] - veclist[i],
						   veclist[i + 2] - veclist[i])
						   .Normalize(),
			veclist[i]);
		int j;
		for (j = 0; j < n; ++j)
		{
			if (Abs(plane.x - plist[j].x) < 0.001f &&
				Abs(plane.y - plist[j].y) < 0.001f &&
				Abs(plane.z - plist[j].z) < 0.001f &&
				Abs(plane.dis - plist[j].dis) < 0.001f)
				break;
		}
		if (j == n)
			plist[n++] = plane;
		for (int k = 0; k < 3; ++k, ++i)
		{
			if (i == 0 || aabb.min.x > veclist[i].x)
				aabb.min.x = veclist[i].x;
			if (i == 0 || aabb.min.y > veclist[i].y)
				aabb.min.y = veclist[i].y;
			if (i == 0 || aabb.min.z > veclist[i].z)
				aabb.min.z = veclist[i].z;
			if (i == 0 || aabb.max.x < veclist[i].x)
				aabb.max.x = veclist[i].x;
			if (i == 0 || aabb.max.y < veclist[i].y)
				aabb.max.y = veclist[i].y;
			if (i == 0 || aabb.max.z < veclist[i].z)
				aabb.max.z = veclist[i].z;
		}
	}
	WConvexArea* area =
		(WConvexArea*)new char[sizeof(WConvexArea) + (n - 1) * sizeof(WPlane)];
	area->aabb = aabb;
	area->count = n;
	memcpy(area->plane, plist, n * sizeof(WPlane));
	Add(area);
}

inline unsigned char WBlockModel::CalcFlag(const Waabb& box,
	const WVector& point)
{
	int flag = 0;
	if (box.min.x < point.x)
		flag = 1;
	if (box.max.x > point.x)
		flag |= 2;
	if (box.min.y < point.y)
		flag |= 4;
	if (box.max.y > point.y)
		flag |= 8;
	if (box.min.z < point.z)
		flag |= 0x10;
	if (box.max.z > point.z)
		flag |= 0x20;
	return (unsigned char)flag;
}

float __fastcall WBlockModel::Explore(WOctree* node, const Waabb& box,
	const WVector& pos, const WVector& move, float radius, WPlane* hit,
	float best)
{
	node->check = 1;
	unsigned int flag = CalcFlag(box, node->pivot);
	WOctree::FruitList* entry = node->list;
	for (int k = node->listnum - 1; k >= 0; --k, ++entry)
	{
		unsigned int cover = entry->flag & flag;
		if ((cover & 3) && (cover & 0xc) && (cover & 0x30))
		{
			WConvexArea* area = entry->area;
			if (ColTestRough(pos, move, radius, area->aabb))
			{
				best = CollTest(pos, move, radius, hit, area, best);
				entry->area->check = 1;
			}
		}
	}
	for (int i = 0; i < 8; ++i)
	{
		if ((node->mask & (1 << i)) && (g_treemask[i] & flag) == g_treemask[i])
			best = Explore(node->leaf[i], box, pos, move, radius, hit, best);
	}
	return best;
}

static void RenderAABB(const Waabb& box, WView* view, unsigned long color,
	float expand)
{
	static int box_p[6][4] = {
		{ 0, 2, 3, 1 },
        { 4, 5, 7, 6 },
        { 0, 1, 5, 4 },
        { 3, 2, 6, 7 },
		{ 1, 3, 7, 5 },
        { 2, 0, 4, 6 }
	};
	static int box_v[8][3] = {
		{ 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 0, 0, 1 },
		{ 1, 0, 1 },
        { 0, 1, 1 },
        { 1, 1, 1 }
	};
	WVector corner[8];
	for (int i = 0; i < 8; ++i)
	{
		corner[i] = WVector(box_v[i][0] ? (float)(box.max.x + expand)
										: (float)(box.min.x - expand),
			box_v[i][1] ? (float)(box.max.y + expand)
						: (float)(box.min.y - expand),
			box_v[i][2] ? (float)(box.max.z + expand)
						: (float)(box.min.z - expand));
	}
	for (int face = 0; face < 6; ++face)
	{
		for (int edge = 0; edge < 4; ++edge)
			view->DrawLine(corner[box_p[face][(edge - 1) & 3]], color,
				corner[box_p[face][edge]], color, 0);
	}
}

void WBlockModel::DebugRender(WView* view)
{
	DebugRender(view, m_colbound, m_root, 0);
}

void WBlockModel::DebugRender(WView* view, const Waabb& box, WOctree* node,
	int depth)
{
	Waabb child;
	if (node->check)
	{
		RenderAABB(box, view, 0x4000ff00, 0.0f);
		node->check = 0;
	}
	for (int k = node->listnum - 1; k >= 0; --k)
	{
		if (node->list[k].area->check)
		{
			node->list[k].area->check = 0;
			RenderAABB(node->list[k].area->aabb, view, 0xffff0000, 0.0f);
		}
	}
	for (int i = 0; i < 8; ++i)
	{
		if (node->mask & (1 << i))
		{
			child = ExtendAABB(node->pivot,
				WVector(!(i & 1) ? box.min.x : box.max.x,
					!(i & 2) ? box.min.y : box.max.y,
					!(i & 4) ? box.min.z : box.max.z),
				0.0f);
			DebugRender(view, child, node->leaf[i], depth + 1);
		}
	}
}
