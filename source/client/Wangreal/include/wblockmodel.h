#pragma once
#include "wlist.h"
#include "wmath.h"

class WView;

class WBlockModel
{
public:
	struct WConvexArea
	{
		Waabb aabb;
		int count;
		int check;
		WPlane plane[1];
	};

	struct WOctree
	{
		struct FruitList
		{
			unsigned char flag;
			WConvexArea* area;
		};

		WVector pivot;
		unsigned char mask;
		WOctree* leaf[8];
		int check;
		int listnum;
		FruitList list[1];
	};

	WBlockModel();
	~WBlockModel();
	void Reset();
	void AddTriangleList(const WVector*, int);
	void Build();
	float CollTest(const WVector&, const WVector&, float, WPlane*);
	void DebugRender(WView*);
	void DebugRender(WView*, const Waabb&, WOctree*, int);

private:
	static int CalcDepth(WOctree*);
	int PositionDecision(int);
	static bool ColTestRough(const WVector&, const WVector&, float,
		const Waabb&);
	static float CollTest(const WVector&, const WVector&, float, WPlane*,
		WConvexArea*, float);
	bool IsConvex(const WVector*, int, WPlane*);
	static unsigned char CalcFlag(const Waabb&, const WVector&);
	static float Explore(WOctree*, const Waabb&, const WVector&, const WVector&,
		float, WPlane*, float);
	Waabb ExtendAABB(const Waabb&, const Waabb&);
	Waabb ExtendAABB(const WVector&, const WVector&, float);
	static WVector FixedPivot(WList<WConvexArea*>*, WVector);
	WOctree* ExtendTree(WList<WConvexArea*>*);
	void Add(WConvexArea*);

	static unsigned long g_treemask[8];
	WList<WConvexArea*> m_colarea;
	Waabb m_colbound;
	WOctree* m_root;
};
