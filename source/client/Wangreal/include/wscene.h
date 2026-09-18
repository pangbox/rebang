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
	unsigned int xiFlag0;
	unsigned int xiFlag1;
	int xiBaseVtxIdx;
	int xNumVertices;
	int xiBaseIdxIdx;
	int xNumIndices;
	unsigned int xdwDiffuse;
	WMatrix xmW;
	float xfDepth;
	int xnmTransfs;
	const WMatrix* const* xpapmW;
	const WMatrix* const* xpapmO;
};

struct WxViewState
{
	LightSet xLight;
	WMatrix4 xmView;
	WMatrix4 xmProj;
};
