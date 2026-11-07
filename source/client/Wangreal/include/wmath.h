#pragma once
#include "rebang.h"
#include "wtypes.h"
#include "wutil.h"
#include <math.h>
#include <string.h>

const float g_PI = 3.14159265358979323846f;
const float g_2_PI = 6.28318530717958647692f;
const float g_PI_DIV_2 = 1.57079632679489661923f;
const float g_DEGTORAD = 0.01745329251994329577f;
const float g_RADTODEG = 57.2957795130823208768f;
const float g_HUGE = 3.402823466e+38f;
const float g_EPSILON = 0.00001f;
const float g_CM_TO_WU = 0.32f;

// This is a placeholder; there were probably inlines that used these
// constants, but we have not recovered them yet.
inline void InstantiateWangrealMathInlines()
{
	(void)(g_PI, g_2_PI, g_PI_DIV_2);
	(void)(g_HUGE, g_EPSILON, g_CM_TO_WU);
}

inline float Wabs(const float& value)
{
	return (*(const unsigned long*)&value & 0x80000000) ? -value : value;
}

inline int WisZero(const float& value, float epsilon = g_EPSILON)
{
	return Wabs(value) < epsilon;
}

inline int WisEqual(const float& left, const float& right, float epsilon)
{
	return Wabs(left - right) < epsilon;
}

inline float RadianNormalize2PI(float angle)
{
	if (angle >= g_2_PI || angle < 0.0f)
	{
		angle -= (float)floor(angle * 0.15915494f) * g_2_PI;
	}

	return angle;
}

inline float RadianNormalizePI(float angle)
{
	angle = RadianNormalize2PI(angle);
	if (angle > g_PI)
	{
		angle -= g_2_PI;
	}

	return angle;
}

inline float RadianDelta(float a, float b)
{
	return RadianNormalizePI(a - b);
}

class WMatrix;
class WQuat;

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

	static const WVector ONE;
	static const WVector ZERO;
	static const WVector UNIT_POS_X;
	static const WVector UNIT_POS_Y;
	static const WVector UNIT_POS_Z;
	static const WVector UNIT_NEG_X;
	static const WVector UNIT_NEG_Y;
	static const WVector UNIT_NEG_Z;

	WVector() DEFAULT_IMPL;

	WVector(const float x_, const float y_, const float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}

	WVector(const float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

#ifdef REBANG_LEGACY_CPP
#ifndef WANGREAL_DEVICE
	WVector& operator=(const WVector& other);
#endif
#else
	WVector& operator=(const WVector& other) = default;
#endif

	void Reset();
	void Negate();
	WVector operator-() const;

	WVector& Normalize();
	void operator+=(const WVector& right);

	float Magnitude() const;

	float SquareMagnitude() const;

	void operator*=(float scalar);
	void operator/=(float scalar);
	void operator-=(const WVector& other);
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

	static const WVector4 ONE;
	static const WVector4 ZERO;

	WVector4() { }

	WVector4(float x_, float y_, float z_, float w_);
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
	static const WMatrix ZERO;

	WMatrix() { }

	WMatrix(float xx_, float yx_, float zx_, float xy_, float yy_, float zy_,
		float xz_, float yz_, float zz_);
	WMatrix(const class WPlane& plane);

	WMatrix& operator=(const WMatrix& other);
	void operator=(const WQuat& quat);
	void Normalize();
	void Debug();
	void Reset();
	void GetRotMatrix(WMatrix* result) const;
	WMatrix operator~() const;
	void operator*=(float right);
	void operator*=(const WMatrix& right);
	void operator/=(float right);
	void AxisScale(WVector& scale);
	void Transpose();
	void Rotate(float angle, char direct);
	void Rotate(const WVector& rotation);
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
			unsigned long : 32;
			float xy, yy, zy;
			unsigned long : 32;
			float xz, yz, zz;
			unsigned long : 32;
			float xm, ym, zm;
		};

		float m[4][4];
		float p[16];
	};

	static const WMatrix4 IDENTITY;
	static const WMatrix4 ZERO;

	WMatrix4() { }

	WMatrix4(float p0, float p1, float p2, float p3, float p4, float p5,
		float p6, float p7, float p8, float p9, float p10, float p11, float p12,
		float p13, float p14, float p15);

	WMatrix4& operator=(const WMatrix4& other);
	void operator*=(const WMatrix4& other);
};

class WVector2D
{
public:
	WVector2D() { }
	WVector2D(float x_, float y_)
		: x(x_), y(y_)
	{
	}
	WVector2D(const float* v);
	void Reset();
	float Magnitude() const;
	float SquareMagnitude() const;
	WVector2D& Normalize();
	void Negate();
	void operator+=(const WVector2D& v);
	void operator-=(const WVector2D& v);
	void operator*=(float f);
	void operator/=(float f);
#ifdef REBANG_LEGACY_CPP
	WVector2D& operator=(const WVector2D& other);
#endif
	float x;
	float y;
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
			float dis;
		};
		struct
		{
			float a;
			float b;
			float c;
			float d;
		};
		struct
		{
			WVector normal;
		};
	};

	WPlane() { }

	WPlane(float x_, float y_, float z_, float dis_)
	{
		x = x_;
		y = y_;
		z = z_;
		dis = dis_;
	}

	WPlane(const WVector& normal_, float dis_)
	{
		x = normal_.x;
		y = normal_.y;
		z = normal_.z;
		dis = dis_;
	}

	WPlane(const WVector& normal_, const WVector& point);
	void Normalize();

	WPlane& operator=(const WPlane& other);
};

class WQuat
{
public:
	float x;
	float y;
	float z;
	float w;

	WQuat() DEFAULT_IMPL;

	WQuat(float x_, float y_, float z_, float w_)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}

	WQuat(const WVector& axis, float angle);
	WQuat(const float* v);

	WQuat& operator=(const WQuat& other);
	void Reset();
	void Normalize();
	void ConvertToAxisAngle(WVector* vector, float* theta);
	void ConvertToRotationMatrix(WMatrix& rotation) const;
	void SetFromAngles(float yaw, float pitch, float roll);
	void operator=(const WMatrix& matrix);

private:
	void SetFromAxisAngle(const WVector& vector, float theta);
};

class WSphere
{
public:
	WSphere() { }
	WSphere(const WVector& position, float length)
	{
		pos = position;
		radius = length;
	}

	WVector pos;
	float radius;
};

class Waabb
{
public:
	Waabb() { Reset(); }
	Waabb(const WVector& minimum, const WVector& maximum)
	{
		min = minimum;
		max = maximum;
	}
	Waabb(short* minimum, short* maximum);

	WVector min;
	WVector max;

	Waabb& operator=(const Waabb& box);
	Waabb& operator+=(const Waabb& box);
	Waabb& operator+=(const WVector& vector);
	void Reset()
	{
		min.Reset();
		max.Reset();
	}
	void Default();
	bool IsInclude(const WVector& vector) const
	{
		if (min.x <= vector.x && vector.x <= max.x && min.y <= vector.y &&
			vector.y <= max.y && min.z <= vector.z && vector.z <= max.z)
			return true;
		return false;
	}
	bool IsInclude(const WSphere& sphere) const;
	bool IsInclude(const Waabb& box) const
	{
		if (box.min.x >= min.x && box.max.x <= max.x && box.min.y >= min.y &&
			box.max.y <= max.y && box.min.z >= min.z && box.max.z <= max.z)
			return true;
		return false;
	}
	void Clear()
	{
		min = WVector(3.402823466e+38f, 3.402823466e+38f, 3.402823466e+38f);
		max = WVector(-3.402823466e+38f, -3.402823466e+38f, -3.402823466e+38f);
	}
	void AddPoint(const WVector& vector)
	{
		if (vector.x > max.x)
			max.x = vector.x;
		if (vector.x < min.x)
			min.x = vector.x;
		if (vector.y > max.y)
			max.y = vector.y;
		if (vector.y < min.y)
			min.y = vector.y;
		if (vector.z > max.z)
			max.z = vector.z;
		if (vector.z < min.z)
			min.z = vector.z;
	}
	WVector GetCenter() const
	{
		WVector center;
		center.x = (min.x + max.x) * .5f;
		center.y = (min.y + max.y) * .5f;
		center.z = (min.z + max.z) * .5f;
		return center;
	}
};

class Wobb
{
public:
	Wobb() { }
	Wobb(const Wobb& obb) { *this = obb; }
	Wobb(const WMatrix& mat) { *this = mat; }

	WVector center;
	WVector extend[3];

	void operator*=(float scale);
	void operator*=(const WVector& scale);
	void operator+=(const WVector& vector);
	Wobb& operator=(const WMatrix& mat);
	Wobb& operator=(const Wobb& obb);
	Wobb& operator=(const Waabb& box);
	bool IsInclude(const WVector& point) const;
	bool IsIncludeObb(const Wobb& obb, const WVector& point) const;
	void Normalize();
};

int WisEqual(const WVector& left, const WVector& right, float epsilon);
Waabb operator+(const Waabb& box, const WVector& vector);
WMatrix operator*(const WMatrix& left, const WMatrix& right);
WMatrix operator*(const WMatrix& matrix, float scalar);
WMatrix RotMat(const WVector& axis, float angle);
WMatrix RotMat(float angle, char direct);
WMatrix RotMat(WVector rotation);
WVector MinPointLineSegment(WVector& vp, WVector& va, WVector& vb, WVector* vt);
float WCollisionTest(const Waabb& origin, const WVector& vec,
	const Waabb& target, WPlane* plane);
float __cdecl CalcDeltaAngle(const WVector& v1, const WVector& v2);
void __cdecl InitMath();
void __cdecl UninitMath();

class WView;

void __cdecl MakePlaneEq(WPlane* out_plane, const Waabb& aabb,
	const WMatrix* matrix);
bool __fastcall PlaneFromVecs(WPlane* result, const WVector& first,
	const WVector& second, const WVector& point);
float __cdecl RayIntersect(const WVector& pivot, const WVector& vec,
	WPlane* plane, WPlane* out, float rlen);
float __cdecl ObbIntersect(const Waabb& src_aabb, const WVector& direction,
	const Wobb& dest_obb, WPlane* out_plane);
float __fastcall RayIntersectAABB(const Waabb& box, const WVector& start,
	const WVector& ray, WPlane* result, float radius);

float __cdecl RayIntersectAABB(const Waabb& box, const WVector& start,
	const WVector& ray, WMatrix* matrix, WPlane* result, float radius);
float __cdecl AabbIntersect(const Waabb& src_aabb, const WVector& direction,
	const Waabb& dest_aabb, WPlane* out_plane);
float __cdecl DotContact(const WVector& vec, const WPlane* plane);
WMatrix __cdecl Make3rdCamMatrix(const WVector& position,
	const WVector& target);
WRect __cdecl ScaleRect(WView* view, WRect* rectangle);
void __cdecl ScalePoint(WView* view, WPoint* point);

float WVectorLen(const WVector& vector);
WVector RotVec(const WVector& vector, const WMatrix& matrix);

#include "wmath.inl"
