#pragma once
#include "rebang.h"
#include <math.h>

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

const float g_PI = 3.14159265358979323846f;
const float g_2_PI = 6.28318530717958647692f;
const float g_PI_DIV_2 = 1.57079632679489661923f;
const float g_DEGTORAD = 0.01745329251994329577f;
const float g_HUGE = 3.402823466e+38f;
const float g_EPSILON = 0.00001f;
const float g_CM_TO_WU = 0.32f;

namespace
{
	// This is a placeholder; there were probably inlines that used these
	// constants, but we have not recovered them yet.
	inline void InstantiateWangrealMathInlines()
	{
		(void)g_PI;
		(void)g_2_PI;
		(void)g_PI_DIV_2;
		(void)g_HUGE;
		(void)g_EPSILON;
		(void)g_CM_TO_WU;
	}
}

template <class T>
__forceinline T Abs(T value)
{
	T result;
	if (value > 0)
		result = value;
	else
		result = -value;
	return result;
}

class WVector2D
{
public:
	float x;
	float y;
};

class WVector
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float p[3];
	};

	static const WVector ZERO;
	static const WVector UNIT_POS_Y;

	friend float operator*(const WVector& left, const WVector& right);

	__forceinline WVector() DEFAULT_IMPL;

	__forceinline WVector(float x_, float y_, float z_)
		: x(x_), y(y_), z(z_)
	{
	}

	WVector& Normalize();
	float Magnitude() const;
	float SquareMagnitude() const;
	void operator+=(const WVector& right);
};

class WVector4
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		float p[4];
	};
};

class WMatrix
{
public:
	union
	{
		struct
		{
			WVector xa;
			WVector ya;
			WVector za;
			WVector pivot;
		};
		struct
		{
			float xx, yx, zx;
			float xy, yy, zy;
			float xz, yz, zz;
			float xm, ym, zm;
		};
		float p[12];
	};

	static const WMatrix IDENTITY;
};

class WMatrix4
{
public:
	union
	{
		struct
		{
			WVector xa;
			float wx;
			WVector ya;
			float wy;
			WVector za;
			float wz;
			WVector pivot;
			float wm;
		};

		struct
		{
			float xx, yx, zx;
			unsigned char gapC[4];
			float xy, yy, zy;
			unsigned char gap1C[4];
			float xz, yz, zz;
			unsigned char gap2C[4];
			float xm, ym, zm;
		};

		float m[4][4];
		float p[16];
	};
};

class WPlane
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		struct
		{
			float a;
			float b;
			float c;
		};

		struct
		{
			WVector normal;
		};
	};

	float dis;
};

class WSphere
{
public:
	WVector pos;
	float radius;
};

#include "wmath.inl"
