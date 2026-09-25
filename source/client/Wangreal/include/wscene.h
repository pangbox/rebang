#pragma once
#include "wmath.h"

struct LightSet
{
	unsigned long type;
	WVector nearOne;
	unsigned long diffuse;
	unsigned long ambient;
	unsigned long ambient2;

	LightSet()
	{
		type = 0;
		diffuse = 0;
		ambient = 0;
		ambient2 = 0xffffff;
		nearOne.x = nearOne.y = nearOne.z = 0.0f;
	}
};

struct WxBatchState
{
	int xiFlag0;
	int xiFlag1;
	int xiBaseVtxIdx;
	int xnVtxs;
	int xiBaseIdxIdx;
	int xnIdxs;
	ulong xdwDiffuse;
	WMatrix xmW;
	float xfDepth;
	int xnmTransfs;
	const WMatrix* const* xpapmW;
	const WMatrix* const* xpapmO;

	WxBatchState()
	{
		xiFlag0 = 0;
		xiFlag1 = 0;
		xiBaseVtxIdx = 0;
		xnVtxs = 0;
		xiBaseIdxIdx = 0;
		xnIdxs = 0;
		xdwDiffuse = 0xffffffff;
		xfDepth = g_HUGE;
		xnmTransfs = 0;
		xpapmW = 0;
		xpapmO = 0;
		xmW.Reset();
	}
};

struct WxViewState
{
	LightSet xLight;
	WMatrix4 xmView;
	WMatrix4 xmProj;
};

class WScene
{
public:
	virtual ~WScene();
	virtual LightSet* GetLightSet(const WVector& pos);
	virtual float CollTest(const WVector&, const WVector&, WPlane*, float);
};
