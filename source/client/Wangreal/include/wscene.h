#pragma once
#include "wmath.h"

struct LightSet
{
	unsigned int type;
	WVector nearOne;
	unsigned int diffuse;
	unsigned int ambient;
	unsigned int ambient2;
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
