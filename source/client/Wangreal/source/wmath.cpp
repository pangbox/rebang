#include "wmath.h"

const WVector WVector::ONE(1.0f, 1.0f, 1.0f);
const WVector WVector::UNIT_POS_X(1.0f, 0.0f, 0.0f);
const WVector WVector::UNIT_POS_Y(0.0f, 1.0f, 0.0f);
const WVector WVector::UNIT_POS_Z(0.0f, 0.0f, 1.0f);
const WVector WVector::UNIT_NEG_X(-1.0f, 0.0f, 0.0f);
const WVector WVector::UNIT_NEG_Y(0.0f, -1.0f, 0.0f);
const WVector WVector::UNIT_NEG_Z(0.0f, 0.0f, -1.0f);
const WMatrix WMatrix::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	1.0f);
__declspec(align(8)) const WMatrix4 WMatrix4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
__declspec(align(8)) const WVector4 WVector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);

float* costable = 0;
float* sqrttable = 0;
float* acostable = 0;
const WVector WVector::ZERO(0.0f, 0.0f, 0.0f);
const WMatrix WMatrix::ZERO(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f);
__declspec(align(8)) const WMatrix4 WMatrix4::ZERO(0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
__declspec(align(8)) const WVector4 WVector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

WVector4::WVector4(float x_, float y_, float z_, float w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

Waabb& Waabb::operator+=(const WVector& vector)
{
	if (vector.x > 0.0f)
		max.x += vector.x;
	else
		min.x += vector.x;
	if (vector.y > 0.0f)
		max.y += vector.y;
	else
		min.y += vector.y;
	if (vector.z > 0.0f)
		max.z += vector.z;
	else
		min.z += vector.z;
	return *this;
}

void WMatrix::Normalize()
{
	xa.Normalize();
	ya.Normalize();
	za.Normalize();
}

void WMatrix::GetRotMatrix(WMatrix* result) const
{
	WVector v(1.0f, 0.0f, 0.0f);
	*result = *this;
	result->xm = result->ym = result->zm = 0.0f;
	v = v * *result;
	float size = 1.0f / (float)sqrt(v.y * v.y + v.x * v.x + v.z * v.z);
	result->xa *= size;
	result->ya *= size;
	result->za *= size;
}

void WMatrix::AxisScale(WVector& scale)
{
	Normalize();
	xx = scale.x * xx;
	yx = yx * scale.x;
	zx = zx * scale.x;
	xy = scale.y * xy;
	yy = scale.y * yy;
	zy = scale.y * zy;
	xz = xz * scale.z;
	yz = yz * scale.z;
	zz = zz * scale.z;
}

void WQuat::Reset()
{
	x = y = z = 0.0f;
	w = 1.0f;
}

void WQuat::Normalize()
{
	float size = 1.0f / (float)sqrt(x * x + y * y + z * z + w * w);
	x *= size;
	y *= size;
	z *= size;
	w *= size;
}

void WQuat::SetFromAxisAngle(const WVector& vector, float theta)
{
	float halfTheta = theta * 0.5f;
	float sinValue = (float)sin(halfTheta);
	x = sinValue * vector.x;
	y = sinValue * vector.y;
	z = sinValue * vector.z;
	w = (float)cos(halfTheta);
}

void WQuat::ConvertToAxisAngle(WVector* vector, float* theta)
{
	float cosine;
	*(unsigned long*)&cosine = *(unsigned long*)&w;
	float angle = (float)acos((double)cosine);
	*theta = 2.0f * angle;
	float absoluteTheta;
	if (*(unsigned long*)theta & 0x80000000)
		absoluteTheta = -*theta;
	else
		absoluteTheta = *theta;
	if (absoluteTheta < g_EPSILON)
	{
		vector->x = 1.0f;
		vector->y = 0.0f;
		vector->z = 0.0f;
		return;
	}
	float sinValue = 1.0f / (float)sin(*theta * 0.5f);
	vector->x = sinValue * x;
	vector->y = sinValue * y;
	vector->z = sinValue * z;
}

void WQuat::ConvertToRotationMatrix(WMatrix& rotation) const
{
	float fTx = 2.0f * x;
	float fTy = 2.0f * y;
	float fTz = 2.0f * z;
	float fTwx = fTx * w;
	float fTwy = fTy * w;
	float fTwz = fTz * w;
	float fTxx = fTx * x;
	float fTxy = fTy * x;
	float fTxz = fTz * x;
	float fTyy = fTy * y;
	float fTyz = fTz * y;
	float fTzz = fTz * z;

	rotation.xm = rotation.ym = rotation.zm = 0.0f;
	rotation.xx = 1.0f - (fTyy + fTzz);
	rotation.yx = fTxy - fTwz;
	rotation.zx = fTxz + fTwy;
	rotation.xy = fTxy + fTwz;
	rotation.yy = 1.0f - (fTxx + fTzz);
	rotation.zy = fTyz - fTwx;
	rotation.xz = fTxz - fTwy;
	rotation.yz = fTyz + fTwx;
	rotation.zz = 1.0f - (fTxx + fTyy);
}

void WQuat::SetFromAngles(float yaw, float pitch, float roll)
{
	float fSinYaw = sinf(yaw * 0.5f);
	float fSinPitch = sinf(pitch * 0.5f);
	float fSinRoll = sinf(roll * 0.5f);
	float fCosYaw = cosf(yaw * 0.5f);
	float fCosPitch = cosf(pitch * 0.5f);
	float fCosRoll = cosf(roll * 0.5f);
	x = fCosPitch * fCosYaw * fSinRoll - fCosRoll * fSinPitch * fSinYaw;
	y = fCosPitch * fSinRoll * fSinYaw + fCosRoll * fCosYaw * fSinPitch;
	z = fCosRoll * fCosPitch * fSinYaw - fCosYaw * fSinRoll * fSinPitch;
	w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	Normalize();
}

void WQuat::operator=(const WMatrix& matrix)
{
	float t = matrix.xx + matrix.yy + matrix.zz;
	float len;
	float s;
	if (t > 0.0f)
	{
		len = (float)sqrt((float)(t + 1.0f));
		s = 0.5f / len;
		w = len * 0.5f;
		x = (matrix.yz - matrix.zy) * s;
		y = (matrix.zx - matrix.xz) * s;
		z = (matrix.xy - matrix.yx) * s;
		return;
	}

	switch (matrix.xx > matrix.yy ? (matrix.xx > matrix.zz ? 0 : 2)
								  : (matrix.yy > matrix.zz ? 1 : 2))
	{
	case 0:
		len = (float)sqrt(1.0f + matrix.xx - matrix.yy - matrix.zz);
		s = 0.5f / len;
		x = len * 0.5f;
		y = (matrix.xy + matrix.yx) * s;
		z = (matrix.xz + matrix.zx) * s;
		w = (matrix.yz - matrix.zy) * s;
		break;
	case 1:
		len = (float)sqrt(1.0f - matrix.xx + matrix.yy - matrix.zz);
		s = 0.5f / len;
		x = (matrix.xy + matrix.yx) * s;
		y = len * 0.5f;
		z = (matrix.yz + matrix.zy) * s;
		w = (matrix.zx - matrix.xz) * s;
		break;
	case 2:
		len = (float)sqrt(1.0f - matrix.xx - matrix.yy + matrix.zz);
		s = 0.5f / len;
		x = (matrix.xz + matrix.zx) * s;
		y = (matrix.yz + matrix.zy) * s;
		z = len * 0.5f;
		w = (matrix.xy - matrix.yx) * s;
		break;
	}
}

void WMatrix::operator=(const WQuat& quat)
{
	float _xx = 2.0f * quat.x * quat.x;
	float _yy = 2.0f * quat.y * quat.y;
	float _zz = 2.0f * quat.z * quat.z;
	float _xy = 2.0f * quat.x * quat.y;
	float _xz = 2.0f * quat.x * quat.z;
	float _yz = 2.0f * quat.y * quat.z;
	float _wx = 2.0f * quat.w * quat.x;
	float _wy = 2.0f * quat.w * quat.y;
	float _wz = 2.0f * quat.w * quat.z;

	xx = 1.0f - (_yy + _zz);
	yx = _xy - _wz;
	zx = _xz + _wy;
	xy = _xy + _wz;
	yy = 1.0f - (_xx + _zz);
	zy = _yz - _wx;
	xz = _xz - _wy;
	yz = _yz + _wx;
	zz = 1.0f - (_xx + _yy);
}

WVector __fastcall MinPointLineSegment(WVector& vp, WVector& va, WVector& vb,
	WVector* vt)
{
	WVector vm = vb - va;
	WVector vdiff = vp - va;
	float t = vdiff * vm;
	float mSqr;
	if (t <= 0.0f)
	{
		*vt = va;
	}
	else if (t >= (mSqr = vm.SquareMagnitude()))
	{
		vdiff = vdiff - vm;
		*vt = vb;
	}
	else
	{
		t /= mSqr;
		*vt = va + vm * t;
		vdiff = vp - *vt;
	}
	return vdiff;
}

void __cdecl UninitMath()
{
}

void WMatrix::Rotate(float angle, char direct)
{
	if (direct >= 3)
		*this = RotMat(angle, direct - 3) * *this;
	else
		*this = *this * RotMat(angle, direct);
}

void WMatrix::Rotate(const WVector& rotation)
{
	*this = *this *
		(RotMat(rotation.x, 0) * RotMat(rotation.y, 1) * RotMat(rotation.z, 2));
}

void __cdecl InitMath()
{
}

float __cdecl CalcDeltaAngle(const WVector& v1, const WVector& v2)
{
	union FloatBits
	{
		float value;
		unsigned long bits;
	};
	FloatBits xBits, yBits, zBits;
	zBits.value = v1.z;
	xBits.value = v1.x;
	yBits.value = 0.0f;
	WVector va;
	*(unsigned long*)&va.x = xBits.bits;
	*(unsigned long*)&va.y = yBits.bits;
	*(unsigned long*)&va.z = zBits.bits;
	zBits.value = v2.z;
	xBits.value = v2.x;
	yBits.value = 0.0f;
	WVector vb;
	*(unsigned long*)&vb.x = xBits.bits;
	*(unsigned long*)&vb.y = yBits.bits;
	*(unsigned long*)&vb.z = zBits.bits;
	float la = (float)sqrt(va.x * va.x + va.y * va.y + va.z * va.z);
	float lb = (float)sqrt(vb.x * vb.x + vb.y * vb.y + vb.z * vb.z);
	if ((*(unsigned long*)&la & 0x80000000 ? -la : la) < g_EPSILON ||
		(*(unsigned long*)&lb & 0x80000000 ? -lb : lb) < g_EPSILON)
		return 0.0f;
	// Preserve the float rounding at both boundaries of the original inline call.
	volatile float cosine = Between<float>(-1.0f, va * vb / (la * lb), 1.0f);
	volatile float angle = (float)acos((double)cosine);
	float ang = angle;
	return v2.x * v1.z - v2.z * v1.x > 0.0f ? ang : -ang;
}

float __fastcall WCollisionTest(const Waabb& origin, const WVector& vec,
	const Waabb& target, WPlane* plane)
{
	Waabb area(origin.min, origin.max);
	area += vec;
	if (!(area & target) || WisEqual(vec, WVector::ZERO, g_EPSILON))
		return 1.0f;

	int direct = 3;
	float t = 1.0f;
	float t2;
	for (int i = 0; i < 3; i++)
	{
		float absoluteComponent;
		if (*(unsigned long*)&vec.p[i] & 0x80000000)
			absoluteComponent = -vec.p[i];
		else
			absoluteComponent = vec.p[i];
		if (absoluteComponent < g_EPSILON)
			continue;
		if (vec.p[i] > 0.0f)
			t2 = 1.0f - (area.max.p[i] - target.min.p[i]) / vec.p[i];
		else
			t2 = 1.0f - (area.min.p[i] - target.max.p[i]) / vec.p[i];
		if (direct == 3 || t < t2)
		{
			t = t2;
			direct = i;
		}
	}

	if (direct >= 3 || t >= 1.0f)
		return 1.0f;
	float absoluteT;
	if (*(unsigned long*)&t & 0x80000000)
		absoluteT = -t;
	else
		absoluteT = t;
	if ((Abs(t), absoluteT) < g_EPSILON)
		return 0.0f;

	WVector gap;
	gap = vec * t;
	if (WisEqual(WCollisionTest(origin + gap, vec * (1.0f - t), target, 0),
			1.0f, g_EPSILON))
	{
		return 1.0f;
	}
	if (plane)
	{
		plane->normal = WVector::ZERO;
		plane->normal.p[direct] = vec.p[direct] > 0.0f ? -1.0f : 1.0f;
	}
	return t;
}
