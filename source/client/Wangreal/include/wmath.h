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

inline float Wabs(const float& value)
{
	return (*(const unsigned long*)&value & 0x80000000) ? -value : value;
}

inline int WisEqual(const float& left, const float& right, float epsilon)
{
	return Wabs(left - right) < epsilon;
}

class WVector2D
{
public:
	WVector2D() { }
	WVector2D(float x_, float y_)
		: x(x_), y(y_)
	{
	}
	float x;
	float y;
};

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

	WVector() DEFAULT_IMPL;

	WVector(const float x_, const float y_, const float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}

#ifdef REBANG_LEGACY_CPP
	WVector& operator=(const WVector& other);
#else
	WVector& operator=(const WVector& other) = default;
#endif

	void Reset();
	WVector operator-() const;

	WVector& Normalize();
	void operator+=(const WVector& right);

	float Magnitude() const;

	float SquareMagnitude() const;

	void operator*=(float scalar);
	void operator/=(float scalar);
	void operator-=(const WVector& other);

	static const WVector ONE;
	static const WVector ZERO;
	static const WVector UNIT_POS_X;
	static const WVector UNIT_POS_Y;
	static const WVector UNIT_POS_Z;
	static const WVector UNIT_NEG_X;
	static const WVector UNIT_NEG_Y;
	static const WVector UNIT_NEG_Z;
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

	WVector4() { }

	WVector4(float x_, float y_, float z_, float w_);

	static const WVector4 ONE;
	static const WVector4 ZERO;
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

	Waabb& operator+=(const Waabb& box);
	Waabb& operator+=(const WVector& vector);
	void Reset()
	{
		min.Reset();
		max.Reset();
	}
	void Default();
	bool IsInclude(const Waabb& box) const;
	bool IsInclude(const WVector& vector) const;
	bool IsInclude(WVector& vector) const;
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
	WVector GetCenter() const;
};

class Wobb
{
public:
	Wobb() { }

	WVector center;
	WVector extend[3];

	bool IsInclude(const WVector& point) const;
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

	WMatrix() { }

	WMatrix(float xx_, float yx_, float zx_, float xy_, float yy_, float zy_,
		float xz_, float yz_, float zz_)
		: xx(xx_),
		  yx(yx_),
		  zx(zx_),
		  xy(xy_),
		  yy(yy_),
		  zy(zy_),
		  xz(xz_),
		  yz(yz_),
		  zz(zz_)
	{
		pivot.Reset();
	}

	WMatrix& operator=(const WMatrix& other);
	void operator=(const WQuat& quat);
	void Normalize();
	void Reset()
	{
		xx = yy = zz = 1.0f;
		yx = zx = xy = zy = xz = yz = xm = ym = zm = 0.0f;
	}
	void GetRotMatrix(WMatrix* result) const;
	WMatrix operator~() const;
	void operator*=(float right);
	void AxisScale(WVector& scale);
	void Rotate(float angle, char direct);
	void Rotate(const WVector& rotation);

	static const WMatrix IDENTITY;
	static const WMatrix ZERO;
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

	WMatrix4() { }

	WMatrix4(float p0, float p1, float p2, float p3, float p4, float p5,
		float p6, float p7, float p8, float p9, float p10, float p11, float p12,
		float p13, float p14, float p15);

	WMatrix4& operator=(const WMatrix4& other);

	static const WMatrix4 IDENTITY;
	static const WMatrix4 ZERO;
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

	WPlane(const WVector& normal_, const WVector& point);

	WPlane(const WVector& normal_, float dis_)
	{
		x = normal_.x;
		y = normal_.y;
		z = normal_.z;
		dis = dis_;
	}

	WPlane(float x_, float y_, float z_, float dis_)
	{
		x = x_;
		y = y_;
		z = z_;
		dis = dis_;
	}

	WPlane& operator=(const WPlane& other);
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

int WisEqual(const WVector& left, const WVector& right, float epsilon);
Waabb operator+(const Waabb& box, const WVector& vector);
WMatrix operator*(const WMatrix& left, const WMatrix& right);
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
