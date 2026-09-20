#pragma once
#include <math.h>

class gaMath
{
public:
	static __forceinline float Sin(float value) { return sinf(value); }
	static __forceinline float Cos(float value) { return cosf(value); }

	static __forceinline float ASin(float value)
	{
		return (float)::asin((double)value);
	}
};
