#pragma once
#include "wlist.h"
#include "wmath.h"

class WView;

class WBlockModel
{
public:
	WBlockModel();
	~WBlockModel();
	void Reset();
	void Build();
	float CollTest(const WVector&, const WVector&, float, WPlane*);
	void AddTriangleList(const WVector*, int);
	struct WConvexArea
	{
		Waabb aabb;
		int count;
		int check;
		WPlane plane[1];
	};

private:
	WList<WConvexArea*> m_colarea;
	Waabb m_colbound;
	void Add(WConvexArea*);
	bool IsConvex(const WVector*, int, WPlane*);
	Waabb ExtendAABB(const Waabb&, const Waabb&);
	Waabb ExtendAABB(const WVector&, const WVector&, float);
	static bool ColTestRough(const WVector&, const WVector&, float,
		const Waabb&);
	static bool ColTestRough(const Waabb&, const Waabb&);
	static float CollTest(const WVector&, const WVector&, float, WPlane*,
		WConvexArea*, float);
	struct WOctree
	{
		WVector pivot;
		unsigned char mask;
		WOctree* leaf[8];
		int check;
		int listnum;
		struct FruitList
		{
			unsigned char flag;
			WConvexArea* area;
		};

		FruitList list[1];
	};
	WOctree* m_root;
	static unsigned long g_treemask[8];
	int PositionDecision(int);
	WOctree* ExtendTree(WList<WConvexArea*>*);
	static int CalcDepth(WOctree*);
	static unsigned char CalcFlag(const Waabb&, const WVector&);
	static WVector FixedPivot(WList<WConvexArea*>*, WVector);
	static float Explore(WOctree*, const Waabb&, const WVector&, const WVector&,
		float, WPlane*, float);

public:
	void DebugRender(WView*);
	void DebugRender(WView*, const Waabb&, WOctree*, int);
};
