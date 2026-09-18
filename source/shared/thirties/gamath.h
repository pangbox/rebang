#pragma once
#include <math.h>

class gaMath
{
public:
	static __forceinline float ASin(float value)
	{
		return (float)::asin((double)value);
	}
};
