#pragma once
#include "wmath.h"

struct LightSet
{
	enum
	{
		W_LIGHT_NOT_INITIALIZED = 0,
		W_LIGHT_POINT = 1,
		W_LIGHT_DIRECTIONAL = 2
	};
	unsigned long type;
	WVector nearone;
	unsigned long diffuse;
	unsigned long ambient;
	unsigned long ambient2;

	LightSet()
	{
		type = W_LIGHT_NOT_INITIALIZED;
		diffuse = 0;
		ambient = 0;
		ambient2 = 0xffffff;
		nearone.x = nearone.y = nearone.z = 0.0f;
	}
	void operator=(const LightSet& other)
	{
		memcpy(this, &other, sizeof(LightSet));
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
