#pragma once
#include <math.h>
#include <string.h>

struct gaV2;
struct gaV3;
struct gaM3;
struct gaM4;
struct gaQ;

struct gaMath
{
	static int Sign(int iValue);

	static float Sign(float fValue);
	static float Sin(float fValue);
	static float Cos(float fValue);
	static float Tan(float fValue);
	static float ASin(float fValue);
	static float ACos(float fValue);
	static float ATan(float fValue);
	static float ATan2(float fY, float fX);
	static float Sqr(float fValue);
	static float Sqrt(float fValue);
	static float InvSqrt(float fValue);
	static float Pow(float fBase, float fExponent);
	static float Exp(float fValue);
	static float Log(float fValue);
	static float FAbs(float fValue);
	static float Abs(float fValue);
	static int Abs(int iValue);
	static float Floor(float fValue);
	static float Ceil(float fValue);
	static float FMod(float fX, float fY);

	static float UnitRandom(unsigned int seed = 0);
	static float SymmetricRandom(unsigned int seed = 0);
	static float SymmetricIntervalRandom(float halfExtent,
		unsigned int seed = 0);
	static float SymmetricHalfRandom(unsigned int seed = 0);
	static float IntervalRandom(float min, float max, unsigned int seed = 0);
	static void UnitRandomArray(float* dest, int len, unsigned int seed = 0);
	static void SymmetricRandomArray(float* dest, int len,
		unsigned int seed = 0);

	static bool NearlyZero(float fValue, float fTolerance);
	static bool NearlyEqual(float fA, float fB, float fTolerance);

	static const float ZERO;
	static const float HALF;
	static const float ID;
	static const float TWO;
	static const float MAX_REAL;
	static const float EPSILON;
	static const float TOLERANCE;
	static const float INF;
	static const float PI;
	static const float TWO_PI;
	static const float HALF_PI;
	static const float INV_PI;
	static const float INV_HALF_PI;
	static const float INV_TWO_PI;
	static const float TO_RADIAN;
	static const float TO_DEGREE;
	static const float SQRT_HALF;
};

struct gaV2
{
	float x, y;
	gaV2() { }
	gaV2(const float* af);
	gaV2(const gaV2& v);
	gaV2(float fx, float fy);

	gaV2(float* af);

	gaV2& SetZero();
	gaV2& SetOne();
	gaV2& SetHalf();
	gaV2& SetUnitX();
	gaV2& SetUnitY();
	gaV2& SetUnitZ();
	gaV2& Set(float f);
	gaV2& Set(float fx, float fy);
	gaV2& Set(const gaV2& v);

	operator float*();
	operator const float*() const;
	float& operator[](int i);
	float operator[](int i) const;

	gaV2& operator+=(const gaV2& v);
	gaV2& operator-=(const gaV2& v);
	gaV2& operator*=(float f);
	gaV2& operator/=(float f);
	gaV2 operator+() const;

	gaV2 operator+(const gaV2& v) const;
	gaV2 operator-() const;
	gaV2 operator-(const gaV2& v) const;
	gaV2 operator*(float f) const;
	gaV2 operator/(float f) const;

	bool operator==(const gaV2& v) const;
	bool operator!=(const gaV2& v) const;

	float Length() const;
	float SquaredLength() const;
	float Dot(const gaV2& v) const;
	float Normalize();
	float Normalize(gaV2& v) const;
	gaV2 Perp() const;
	gaV2 UnitPerp() const;
	bool IsFinite() const;
	bool IsZero() const;

	static gaV2 UnitRandom(unsigned int seed = 0);
	static void Orthonormalize(gaV2* const av);
	static void GenerateOrthonormalBasis(gaV2& U, gaV2& V, bool bUnitLengthV);

	static const gaV2 ZERO;
	static const gaV2 ONE;
	static const gaV2 UNIT_X;
	static const gaV2 UNIT_Y;
	static const gaV2 MAX;
	static const gaV2 INF;
};

struct gaV3
{
	float x, y, z;
	gaV3() { }
	gaV3(const float* af);
	gaV3(const gaV3& v);
	gaV3(float fx, float fy, float fz);

	gaV3(float* af);

	gaV3& SetZero();
	gaV3& SetOne();
	gaV3& SetHalf();
	gaV3& SetUnitX();
	gaV3& SetUnitY();
	gaV3& SetUnitZ();
	gaV3& Set(float f);
	gaV3& Set(float fx, float fy, float fz);
	gaV3& Set(const gaV3& v);

	operator float*();
	operator const float*() const;
	float& operator[](int i);
	float operator[](int i) const;

	gaV3& operator+=(const gaV3& v);
	gaV3& operator-=(const gaV3& v);
	gaV3& operator*=(float f);
	gaV3& operator/=(float f);
	gaV3 operator+() const;

	gaV3 operator+(const gaV3& v) const;
	gaV3 operator-() const;
	gaV3 operator-(const gaV3& v) const;
	gaV3 operator*(float f) const;
	gaV3 operator/(float f) const;
	friend gaV3 operator*(float f, const gaV3& v);

	bool operator==(const gaV3& v) const;
	bool operator!=(const gaV3& v) const;

	float Length() const;
	float SquaredLength() const;
	float Dot(const gaV3& v) const;
	float Normalize();
	float Normalize(gaV3& v) const;
	gaV3 Cross(const gaV3& v) const;
	gaV3 UnitCross(const gaV3& v) const;
	bool IsFinite() const;
	bool IsZero() const;

	static gaV3 UnitRandom(unsigned int seed = 0);
	static void Orthonormalize(gaV3* const av);
	static void GenerateOrthonormalBasis(gaV3& U, gaV3& V, gaV3& W,
		bool bUnitLengthW);
	static void GenerateOrthonormalBasis2(gaV3& N, gaV3& T, gaV3& K,
		bool bUnitLengthN);

	static const gaV3 ZERO;
	static const gaV3 ONE;
	static const gaV3 UNIT_X;
	static const gaV3 UNIT_Y;
	static const gaV3 UNIT_Z;
	static const gaV3 MAX;
	static const gaV3 INF;
};

struct gaM3
{
	union
	{
		float m[3][3];
		struct
		{
			float m11, m12, m13;
			float m21, m22, m23;
			float m31, m32, m33;
		};
	};
	gaM3() { }
	gaM3(const float* af);
	gaM3(const float af[3][3]);
	gaM3(const gaM3& mat);

	gaM3(float _11, float _12, float _13, float _21, float _22, float _23,
		float _31, float _32, float _33);

	void SetZero();
	void SetIdentity();
	void SetColumn(int iCol, const gaV3& v);
	void SetColumns(const gaV3& v0, const gaV3& v1, const gaV3& v2);
	void SetDiagonal(float f0, float f1, float f2);
	void MakeDiagonal(float f0, float f1, float f2);
	float& operator()(int iRow, int iCol);

	float operator()(int iRow, int iCol) const;
	float* operator[](int iRow);
	float* operator[](int iRow) const;
	gaV3 GetColumn(int iCol) const;
	void GetColumns(gaV3& v0, gaV3& v1, gaV3& v2) const;
	gaV3 GetDiagonal() const;
	operator float*();
	operator const float*() const;
	gaM3& operator*=(const gaM3& mat);

	gaM3& operator*=(float f);
	gaM3& operator+=(const gaM3& mat);
	gaM3& operator-=(const gaM3& mat);
	gaM3& operator/=(float f);
	gaM3 operator+() const;

	gaM3 operator+(const gaM3& mat) const;
	gaM3 operator-() const;
	gaM3 operator-(const gaM3& mat) const;
	gaM3 operator*(const gaM3& mat) const;
	gaM3 operator*(float f) const;
	gaV3 operator*(const gaV3& v) const;
	gaM3 operator/(float f) const;
	gaV3 TransposeTimes(const gaV3& v) const;
	gaM3 TransposeTimes(const gaM3& mat) const;
	gaM3 TimesTranspose(const gaM3& mat) const;
	gaM3 operator^(const gaV3& v) const;

	bool operator==(const gaM3& mat) const;
	bool operator!=(const gaM3& mat) const;

	gaM3 Transpose() const;
	gaM3 Inverse() const;
	float Determinant() const;
	gaM3& SkewSymmetric(const gaV3& v);

	void Orthonormalize();
	bool CholeskyDecomposition();

	void ToAxisAngle(gaV3& axis, float& radian) const;
	void FromAxisAngle(const gaV3& axis, float radian);

	bool ToEulerAnglesXYZ(float& angleX, float& angleY, float& angleZ) const;
	bool ToEulerAnglesXZY(float& angleX, float& angleZ, float& angleY) const;
	bool ToEulerAnglesYXZ(float& angleY, float& angleX, float& angleZ) const;
	bool ToEulerAnglesYZX(float& angleY, float& angleZ, float& angleX) const;
	bool ToEulerAnglesZXY(float& angleZ, float& angleX, float& angleY) const;
	bool ToEulerAnglesZYX(float& angleZ, float& angleY, float& angleX) const;
	void FromEulerAnglesXYZ(float yaw, float pitch, float roll);
	void FromEulerAnglesXZY(float yaw, float pitch, float roll);
	void FromEulerAnglesYXZ(float yaw, float pitch, float roll);
	void FromEulerAnglesYZX(float yaw, float pitch, float roll);
	void FromEulerAnglesZXY(float yaw, float pitch, float roll);
	void FromEulerAnglesZYX(float yaw, float pitch, float roll);

	static const gaM3 ZERO;
	static const gaM3 ID;
};

struct gaV8
{
	float lx, ly, lz, lw;
	float ax, ay, az, aw;
	gaV8() { }
	gaV8(float LX, float LY, float LZ, float AX, float AY, float AZ);
	gaV8(float* af);
	gaV8(const gaV8& v);

	gaV8(const gaV3& vL, const gaV3& vA);

	float& operator[](int i) const;
	operator float*();
	operator const float*() const;

	bool operator==(const gaV8& v) const;
	bool operator!=(const gaV8& v) const;

	gaV8 operator+(const gaV8& v) const;
	gaV8 operator-(const gaV8& v) const;
	gaV8 operator-() const;
	gaV8 operator*(float f) const;
	gaV8 operator/(float f) const;

	gaV8& operator+=(const gaV8& v);
	gaV8& operator-=(const gaV8& v);
	gaV8& operator*=(float f);
	gaV8& operator/=(float f);

	static const gaV8 ZERO;
	static const gaV8 UNIT_LX;
	static const gaV8 UNIT_LY;
	static const gaV8 UNIT_LZ;
	static const gaV8 UNIT_AX;
	static const gaV8 UNIT_AY;
	static const gaV8 UNIT_AZ;

	void SetLinear(const gaV3& v);
	void SetAngular(const gaV3& v);
	gaV3* GetLinear();
	gaV3* GetAngular();
	void Rotate(const gaM3& mat);
	gaV8 RotatedNewVector(const gaM3& mat);
	void AequalVecTimesMat(const float* af, const gaM3& mat);
};

struct gaT3
{
	gaM3 R;
	gaV3 T;
	gaV3 S;
	gaT3() { }
	gaT3(const gaM3& r, const gaV3& t, const gaV3& s);

	gaT3(const gaT3& t);

	gaT3& SetIdentity();
	gaV3 Transform(gaV3& v) const;
	gaM4 ToM4LH();
	static gaM4 ToM4LH(const gaM3& r, const gaV3& t, const gaV3& s);
	gaM4 ToM4RH();
	static gaM4 ToM4RH(const gaM3& r, const gaV3& t, const gaV3& s);

	static const gaT3 ID;
};

struct gaQ
{
	float x, y, z, w;
	gaQ() { }
	gaQ(const float* af);
	gaQ(float fx, float fy, float fz, float fw);
	gaQ(const gaQ& q);
	gaQ(const gaM3& mat);
	gaQ(const gaV3& axis, float radian);

	gaQ(const gaV3* av);

	gaQ& SetZero();
	gaQ& Set(float fx, float fy, float fz, float fw);
	gaQ& Set(const gaV3& axis, float radian);
	gaQ& Set(const gaQ& q);

	operator float*();
	operator const float*() const;
	float& operator[](int i);
	float operator[](int i) const;

	gaQ& operator+=(const gaQ& q);
	gaQ& operator-=(const gaQ& q);
	gaQ& operator*=(const gaQ& q);
	gaQ& operator*=(float f);
	gaQ& operator/=(float f);
	gaQ operator+() const;

	gaQ operator+(const gaQ& q) const;
	gaQ operator-() const;
	gaQ operator-(const gaQ& q) const;
	gaQ operator*(const gaQ& q) const;
	gaQ operator*(float f) const;
	gaV3 operator*(const gaV3& v) const;
	gaQ operator/(float f) const;

	bool operator==(const gaQ& q) const;
	bool operator!=(const gaQ& q) const;
	void FromRotationMatrix(const gaM3& mat);

	void FromRotationMatrix(const gaV3* av);
	void ToRotationMatrix(gaM3& mat) const;
	void ToRotationMatrix(gaV3* av) const;
	void FromAxisAngle(const gaV3& axis, float radian);
	void ToAxisAngle(gaV3& axis, float& radian) const;
	void FromTwoVectors(const gaV3& v0, const gaV3& v1);

	float Length() const;
	float SquaredLength() const;
	float Normalize();
	float Dot(const gaQ& q) const;
	gaQ Inverse() const;
	gaQ Conjugate() const;
	bool IsFinite() const;

	static gaQ Random(unsigned int seed = 0);
	static gaQ Slerp(float t, const gaQ& p, const gaQ& q);
	static gaQ SlerpExtraSpins(float t, const gaQ& p, const gaQ& q,
		int iExtraSpins);
	static gaQ Squad(float t, const gaQ& p, const gaQ& a, const gaQ& b,
		const gaQ& q);

	static const gaQ ZERO;
	static const gaQ ID;
};

struct gaP4
{
	float a, b, c, d;
	gaP4() { }

	gaP4(float fa, float fb, float fc, float fd);
};

struct gaM4
{
	union
	{
		float m[4][4];
		struct
		{
			float m11, m12, m13, m14;
			float m21, m22, m23, m24;
			float m31, m32, m33, m34;
			float m41, m42, m43, m44;
		};
	};
	gaM4() { }
	gaM4(const float* af);
	gaM4(const float af[4][4]);
	gaM4(const gaM4& mat);

	gaM4(float _11, float _12, float _13, float _14, float _21, float _22,
		float _23, float _24, float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44);

	void SetZero();
	void SetIdentity();
	operator float*();
	operator const float*() const;
};

inline float gaMath::Sign(float fValue)
{
	if (fValue > 0.0f)
		return 1.0f;

	if (fValue < 0.0f)
		return -1.0f;

	return 0.0f;
}

inline int gaMath::Sign(int iValue)
{
	if (iValue > 0)
		return 1;

	if (iValue < 0)
		return -1;

	return 0;
}

inline float gaMath::Sin(float fValue)
{
	return sinf(fValue);
}

inline float gaMath::Cos(float fValue)
{
	return cosf(fValue);
}

inline float gaMath::Tan(float fValue)
{
	return (float)tan(fValue);
}

inline float gaMath::ASin(float fValue)
{
	if (-1.0f < fValue)
	{
		if (fValue < ID)
			return (float)asin(fValue);
		else
			return HALF_PI;
	}
	else
	{
		return -HALF_PI;
	}
}

inline float gaMath::ACos(float value)
{
	if (-1.0f < value)
	{
		if (value < ID)
			return (float)acos(value);
		else
			return ZERO;
	}
	else
	{
		return PI;
	}
}

inline float gaMath::ATan(float fValue)
{
	return (float)atan(fValue);
}

inline float gaMath::ATan2(float y, float x)
{
	return (float)atan2(y, x);
}

inline float gaMath::Sqr(float fValue)
{
	return fValue * fValue;
}

inline float gaMath::Sqrt(float fValue)
{
	return (float)sqrt(fValue);
}

inline float gaMath::InvSqrt(float value)
{
	return (float)(ID / sqrt(value));
}

inline float gaMath::Pow(float fBase, float fExponent)
{
	return (float)pow(fBase, fExponent);
}

inline float gaMath::Exp(float fValue)
{
	return (float)exp(fValue);
}

inline float gaMath::Log(float fValue)
{
	return (float)log(fValue);
}

inline float gaMath::FAbs(float fValue)
{
	return (float)fabs(fValue);
}

inline int gaMath::Abs(int iValue)
{
	return iValue >= 0 ? iValue : -iValue;
}

inline float gaMath::Abs(float fValue)
{
	return (float)fabs(fValue);
}

inline float gaMath::Floor(float fValue)
{
	return (float)floor(fValue);
}

inline float gaMath::Ceil(float fValue)
{
	return (float)ceil(fValue);
}

inline float gaMath::FMod(float fX, float fY)
{
	return (float)fmod(fX, fY);
}

inline bool gaMath::NearlyZero(float fValue, float fTolerance)
{
	return FAbs(fValue) < fTolerance;
}

inline bool gaMath::NearlyEqual(float fA, float fB, float fTolerance)
{
	return FAbs(fA - fB) < fTolerance;
}

inline gaV2::gaV2(float* af)
{
	x = af[0];
	y = af[1];
}

inline gaV2::gaV2(float fx, float fy)
{
	x = fx;
	y = fy;
}

inline gaV2::gaV2(const gaV2& v)
{
	x = v.x;
	y = v.y;
}

inline gaV2::gaV2(const float* af)
{
	x = af[0];
	y = af[1];
}

inline gaV2& gaV2::SetZero()
{
	x = y = 0.0f;
	return *this;
}

inline gaV2& gaV2::SetOne()
{
	x = y = 1.0f;
	return *this;
}

inline gaV2& gaV2::SetHalf()
{
	x = y = 0.5f;
	return *this;
}

inline gaV2& gaV2::SetUnitX()
{
	x = 1.0f;
	y = 0.0f;
	return *this;
}

inline gaV2& gaV2::SetUnitY()
{
	x = 0.0f;
	y = 1.0f;
	return *this;
}

inline gaV2& gaV2::SetUnitZ()
{
	x = 0.0f;
	y = 0.0f;
	return *this;
}

inline gaV2& gaV2::Set(const gaV2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

inline gaV2& gaV2::Set(float fx, float fy)
{
	x = fx;
	y = fy;
	return *this;
}

inline gaV2& gaV2::Set(float f)
{
	x = y = f;
	return *this;
}

inline gaV2::operator float*()
{
	return &x;
}

inline gaV2::operator const float*() const
{
	return &x;
}

inline float gaV2::operator[](int i) const
{
	return (&x)[i];
}

inline float& gaV2::operator[](int i)
{
	return (&x)[i];
}

inline gaV2& gaV2::operator+=(const gaV2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline gaV2& gaV2::operator-=(const gaV2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline gaV2& gaV2::operator*=(float f)
{
	x *= f;
	y *= f;
	return *this;
}

inline gaV2& gaV2::operator/=(float f)
{
	float fInv = gaMath::ID / f;
	x *= fInv;
	y *= fInv;
	return *this;
}

inline gaV2 gaV2::operator+(const gaV2& v) const
{
	return gaV2(x + v.x, y + v.y);
}

inline gaV2 gaV2::operator+() const
{
	return *this;
}

inline gaV2 gaV2::operator-(const gaV2& v) const
{
	return gaV2(x - v.x, y - v.y);
}

inline gaV2 gaV2::operator-() const
{
	return gaV2(-x, -y);
}

inline gaV2 gaV2::operator*(float f) const
{
	return gaV2(x * f, y * f);
}

inline gaV2 gaV2::operator/(float f) const
{
	float fInv = gaMath::ID / f;
	return gaV2(x * fInv, y * fInv);
}

inline bool gaV2::operator==(const gaV2& v) const
{
	return x == v.x && y == v.y;
}

inline bool gaV2::operator!=(const gaV2& v) const
{
	return x != v.x || y != v.y;
}

inline float gaV2::Length() const
{
	return gaMath::Sqrt(x * x + y * y);
}

inline float gaV2::SquaredLength() const
{
	return x * x + y * y;
}

inline float gaV2::Dot(const gaV2& v) const
{
	return x * v.x + y * v.y;
}

inline float gaV2::Normalize(gaV2& v) const
{
	float fLength = Length();

	if (fLength < gaMath::TOLERANCE)
	{
		v.SetZero();
		return gaMath::ZERO;
	}

	v = *this * (gaMath::ID / fLength);
	return fLength;
}

inline float gaV2::Normalize()
{
	float fLength = Length();

	if (fLength < gaMath::TOLERANCE)
	{
		SetZero();
		return gaMath::ZERO;
	}

	*this *= gaMath::ID / fLength;
	return fLength;
}

inline gaV2 gaV2::Perp() const
{
	return gaV2(y, -x);
}

inline gaV2 gaV2::UnitPerp() const
{
	gaV2 kPerp(y, -x);
	kPerp.Normalize();
	return kPerp;
}

inline void gaV2::GenerateOrthonormalBasis(gaV2& U, gaV2& V, bool bUnitLengthV)
{
	if (!bUnitLengthV)
		V.Normalize();
	U = V.Perp();
}

inline bool gaV2::IsFinite() const
{
	return gaMath::FAbs(x) <= gaMath::MAX_REAL &&
		gaMath::FAbs(y) <= gaMath::MAX_REAL;
}

inline bool gaV2::IsZero() const
{
	return x == gaMath::ZERO && y == gaMath::ZERO;
}

inline gaV3::gaV3(float* af)
{
	x = af[0];
	y = af[1];
	z = af[2];
}

inline gaV3::gaV3(const gaV3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline gaV3::gaV3(float fx, float fy, float fz)
{
	x = fx;
	y = fy;
	z = fz;
}

inline gaV3::gaV3(const float* af)
{
	x = af[0];
	y = af[1];
	z = af[2];
}

inline gaV3& gaV3::SetZero()
{
	x = y = z = gaMath::ZERO;
	return *this;
}

inline gaV3& gaV3::SetOne()
{
	x = y = z = gaMath::ID;
	return *this;
}

inline gaV3& gaV3::SetHalf()
{
	x = y = z = gaMath::HALF;
	return *this;
}

inline gaV3& gaV3::SetUnitX()
{
	x = gaMath::ID;
	y = z = gaMath::ZERO;
	return *this;
}

inline gaV3& gaV3::SetUnitY()
{
	y = gaMath::ID;
	x = z = gaMath::ZERO;
	return *this;
}

inline gaV3& gaV3::SetUnitZ()
{
	z = gaMath::ID;
	x = y = gaMath::ZERO;
	return *this;
}

inline gaV3& gaV3::Set(const gaV3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

inline gaV3& gaV3::Set(float fx, float fy, float fz)
{
	x = fx;
	y = fy;
	z = fz;
	return *this;
}

inline gaV3& gaV3::Set(float f)
{
	x = y = z = f;
	return *this;
}

inline gaV3::operator float*()
{
	return &x;
}

inline gaV3::operator const float*() const
{
	return &x;
}

inline float gaV3::operator[](int i) const
{
	return (&x)[i];
}

inline float& gaV3::operator[](int i)
{
	return (&x)[i];
}

inline gaV3& gaV3::operator+=(const gaV3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline gaV3& gaV3::operator-=(const gaV3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline gaV3& gaV3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

inline gaV3& gaV3::operator/=(float f)
{
	float fInv = gaMath::ID / f;
	x *= fInv;
	y *= fInv;
	z *= fInv;
	return *this;
}

inline gaV3 gaV3::operator+(const gaV3& v) const
{
	return gaV3(x + v.x, y + v.y, z + v.z);
}

inline gaV3 gaV3::operator-(const gaV3& v) const
{
	return gaV3(x - v.x, y - v.y, z - v.z);
}

inline gaV3 gaV3::operator*(float f) const
{
	return gaV3(x * f, y * f, z * f);
}

inline gaV3 gaV3::operator/(float f) const
{
	float fInv = gaMath::ID / f;
	return gaV3(x * fInv, y * fInv, z * fInv);
}

inline gaV3 operator*(float f, const gaV3& v)
{
	return gaV3(f * v.x, f * v.y, f * v.z);
}

inline bool gaV3::operator==(const gaV3& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

inline bool gaV3::operator!=(const gaV3& v) const
{
	return x != v.x || y != v.y || z != v.z;
}

inline float gaV3::Length() const
{
	return gaMath::Sqrt(x * x + y * y + z * z);
}

inline float gaV3::SquaredLength() const
{
	return x * x + y * y + z * z;
}

inline float gaV3::Dot(const gaV3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

inline float gaV3::Normalize()
{
	float fLength = Length();

	if (fLength < gaMath::TOLERANCE)
	{
		SetZero();
		return gaMath::ZERO;
	}

	*this *= gaMath::ID / fLength;
	return fLength;
}

inline gaV3 gaV3::operator+() const
{
	return *this;
}

inline gaV3 gaV3::operator-() const
{
	return gaV3(-x, -y, -z);
}

inline float gaV3::Normalize(gaV3& v) const
{
	float fLength = Length();

	if (fLength < gaMath::TOLERANCE)
	{
		v.SetZero();
		return gaMath::ZERO;
	}

	v = *this * (gaMath::ID / fLength);
	return fLength;
}

inline gaV3 gaV3::Cross(const gaV3& v) const
{
	return gaV3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

inline gaV3 gaV3::UnitCross(const gaV3& v) const
{
	gaV3 kCross(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	kCross.Normalize();
	return kCross;
}

inline bool gaV3::IsFinite() const
{
	return gaMath::FAbs(x) <= gaMath::MAX_REAL &&
		gaMath::FAbs(y) <= gaMath::MAX_REAL &&
		gaMath::FAbs(z) <= gaMath::MAX_REAL;
}

inline bool gaV3::IsZero() const
{
	return x == gaMath::ZERO && y == gaMath::ZERO && z == gaMath::ZERO;
}

inline gaM3::gaM3(const gaM3& mat)
{
	memcpy(m, mat, sizeof(m));
}

inline gaM3::gaM3(float _11, float _12, float _13, float _21, float _22,
	float _23, float _31, float _32, float _33)
{
	m11 = _11;
	m12 = _12;
	m13 = _13;
	m21 = _21;
	m22 = _22;
	m23 = _23;
	m31 = _31;
	m32 = _32;
	m33 = _33;
}

inline gaM3::gaM3(const float af[3][3])
{
	memcpy(m, af, sizeof(m));
}

inline gaM3::gaM3(const float* af)
{
	memcpy(m, af, sizeof(m));
}

inline void gaM3::SetZero()
{
	memset(m, 0, sizeof(m));
}

inline void gaM3::SetIdentity()
{
	*this = ID;
}

inline void gaM3::SetColumn(int iCol, const gaV3& v)
{
	m[0][iCol] = v.x;
	m[1][iCol] = v.y;
	m[2][iCol] = v.z;
}

inline void gaM3::SetColumns(const gaV3& v0, const gaV3& v1, const gaV3& v2)
{
	SetColumn(0, v0);
	SetColumn(1, v1);
	SetColumn(2, v2);
}

inline void gaM3::SetDiagonal(float f0, float f1, float f2)
{
	m11 = f0;
	m22 = f1;
	m33 = f2;
}

inline void gaM3::MakeDiagonal(float f0, float f1, float f2)
{
	SetZero();
	SetDiagonal(f0, f1, f2);
}

inline float gaM3::operator()(int iRow, int iCol) const
{
	return m[iRow][iCol];
}

inline float& gaM3::operator()(int iRow, int iCol)
{
	return m[iRow][iCol];
}

inline float* gaM3::operator[](int iRow) const
{
	return (float*)m[iRow];
}

inline float* gaM3::operator[](int iRow)
{
	return m[iRow];
}

inline gaV3 gaM3::GetColumn(int iCol) const
{
	return gaV3(m[0][iCol], m[1][iCol], m[2][iCol]);
}

inline void gaM3::GetColumns(gaV3& v0, gaV3& v1, gaV3& v2) const
{
	v0 = GetColumn(0);
	v1 = GetColumn(1);
	v2 = GetColumn(2);
}

inline gaV3 gaM3::GetDiagonal() const
{
	return gaV3(m11, m22, m33);
}

inline gaM3::operator float*()
{
	return &m11;
}

inline gaM3::operator const float*() const
{
	return &m11;
}

inline gaM3& gaM3::operator*=(const gaM3& mat)
{
	return *this = *this * mat;
}

inline gaM3& gaM3::operator*=(float f)
{
	return *this = *this * f;
}

inline gaM3& gaM3::operator+=(const gaM3& mat)
{
	return *this = *this + mat;
}

inline gaM3& gaM3::operator-=(const gaM3& mat)
{
	return *this = *this - mat;
}

inline gaM3& gaM3::operator/=(float f)
{
	return *this = *this / f;
}

inline gaM3 gaM3::operator+() const
{
	return *this;
}

inline gaM3 gaM3::operator+(const gaM3& mat) const
{
	return gaM3(m11 + mat.m11, m12 + mat.m12, m13 + mat.m13, m21 + mat.m21,
		m22 + mat.m22, m23 + mat.m23, m31 + mat.m31, m32 + mat.m32,
		m33 + mat.m33);
}

inline gaM3 gaM3::operator-() const
{
	return gaM3(-m11, -m12, -m13, -m21, -m22, -m23, -m31, -m32, -m33);
}

inline gaM3 gaM3::operator-(const gaM3& mat) const
{
	return gaM3(m11 - mat.m11, m12 - mat.m12, m13 - mat.m13, m21 - mat.m21,
		m22 - mat.m22, m23 - mat.m23, m31 - mat.m31, m32 - mat.m32,
		m33 - mat.m33);
}

inline gaM3 gaM3::operator*(const gaM3& mat) const
{
	gaM3 ret;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ret.m[i][j] = m[i][0] * mat.m[0][j] + m[i][1] * mat.m[1][j] +
				m[i][2] * mat.m[2][j];
		}
	}
	return ret;
}

inline gaM3 gaM3::operator*(float f) const
{
	return gaM3(m11 * f, m12 * f, m13 * f, m21 * f, m22 * f, m23 * f, m31 * f,
		m32 * f, m33 * f);
}

inline gaV3 gaM3::operator*(const gaV3& v) const
{
	return gaV3(m11 * v.x + m12 * v.y + m13 * v.z,
		m21 * v.x + m22 * v.y + m23 * v.z, m31 * v.x + m32 * v.y + m33 * v.z);
}

inline gaM3 gaM3::operator/(float f) const
{
	float fInv = gaMath::ID / f;
	return gaM3(m11 * fInv, m12 * fInv, m13 * fInv, m21 * fInv, m22 * fInv,
		m23 * fInv, m31 * fInv, m32 * fInv, m33 * fInv);
}

inline gaV3 gaM3::TransposeTimes(const gaV3& v) const
{
	return gaV3(m11 * v.x + m21 * v.y + m31 * v.z,
		m12 * v.x + m22 * v.y + m32 * v.z, m13 * v.x + m23 * v.y + m33 * v.z);
}

inline gaM3 gaM3::TransposeTimes(const gaM3& mat) const
{
	gaM3 ret;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ret.m[i][j] = m[0][i] * mat.m[0][j] + m[1][i] * mat.m[1][j] +
				m[2][i] * mat.m[2][j];
		}
	}
	return ret;
}

inline gaM3 gaM3::TimesTranspose(const gaM3& mat) const
{
	gaM3 ret;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ret.m[i][j] = m[i][0] * mat.m[j][0] + m[i][1] * mat.m[j][1] +
				m[i][2] * mat.m[j][2];
		}
	}
	return ret;
}

inline bool gaM3::operator==(const gaM3& mat) const
{
	return m11 == mat.m11 && m12 == mat.m12 && m13 == mat.m13 &&
		m21 == mat.m21 && m22 == mat.m22 && m23 == mat.m23 && m31 == mat.m31 &&
		m32 == mat.m32 && m33 == mat.m33;
}

inline bool gaM3::operator!=(const gaM3& mat) const
{
	return m11 != mat.m11 || m12 != mat.m12 || m13 != mat.m13 ||
		m21 != mat.m21 || m22 != mat.m22 || m23 != mat.m23 || m31 != mat.m31 ||
		m32 != mat.m32 || m33 != mat.m33;
}

inline gaM3 gaM3::Transpose() const
{
	return gaM3(m11, m21, m31, m12, m22, m32, m13, m23, m33);
}

inline gaM3 gaM3::Inverse() const
{
	gaM3 inverse;
	inverse.m11 = m22 * m33 - m23 * m32;
	inverse.m12 = m13 * m32 - m12 * m33;
	inverse.m13 = m12 * m23 - m13 * m22;
	inverse.m21 = m23 * m31 - m21 * m33;
	inverse.m22 = m11 * m33 - m13 * m31;
	inverse.m23 = m13 * m21 - m11 * m23;
	inverse.m31 = m21 * m32 - m22 * m31;
	inverse.m32 = m12 * m31 - m11 * m32;
	inverse.m33 = m11 * m22 - m12 * m21;
	float fDet = m11 * inverse.m11 + m12 * inverse.m21 + m13 * inverse.m31;
	if (gaMath::FAbs(fDet) <= gaMath::TOLERANCE)
		return ZERO;
	return inverse / fDet;
}

inline float gaM3::Determinant() const
{
	return m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) +
		m13 * (m21 * m32 - m22 * m31);
}

inline gaM3& gaM3::SkewSymmetric(const gaV3& v)
{
	m11 = m22 = m33 = gaMath::ZERO;
	m12 = -v.z;
	m13 = v.y;
	m21 = v.z;
	m23 = -v.x;
	m31 = -v.y;
	m32 = v.x;
	return *this;
}

inline gaV8::gaV8(float LX, float LY, float LZ, float AX, float AY, float AZ)
{
	lx = LX;
	ly = LY;
	lz = LZ;
	lw = 0.0f;
	ax = AX;
	ay = AY;
	az = AZ;
	aw = 0.0f;
}

inline gaV8::gaV8(float* af)
{
	memcpy(this, af, sizeof(gaV8));
}

inline gaV8::gaV8(const gaV8& v)
{
	memcpy(this, &v, sizeof(gaV8));
}

inline gaV8::gaV8(const gaV3& vL, const gaV3& vA)
{
	lx = vL.x;
	ly = vL.y;
	lz = vL.z;
	lw = 0.0f;
	ax = vA.x;
	ay = vA.y;
	az = vA.z;
	aw = 0.0f;
}

inline float& gaV8::operator[](int i) const
{
	return ((float*)&lx)[i];
}

inline gaV8::operator float*()
{
	return &lx;
}

inline gaV8::operator const float*() const
{
	return &lx;
}

inline bool gaV8::operator==(const gaV8& v) const
{
	return lx == v.lx && ly == v.ly && lz == v.lz && ax == v.ax && ay == v.ay &&
		az == v.az;
}

inline bool gaV8::operator!=(const gaV8& v) const
{
	return lx != v.lx || ly != v.ly || lz != v.lz || ax != v.ax || ay != v.ay ||
		az != v.az;
}

inline gaV8 gaV8::operator+(const gaV8& v) const
{
	return gaV8(lx + v.lx, ly + v.ly, lz + v.lz, ax + v.ax, ay + v.ay,
		az + v.az);
}

inline gaV8 gaV8::operator-(const gaV8& v) const
{
	return gaV8(lx - v.lx, ly - v.ly, lz - v.lz, ax - v.ax, ay - v.ay,
		az - v.az);
}

inline gaV8 gaV8::operator-() const
{
	return gaV8(-lx, -ly, -lz, -ax, -ay, -az);
}

inline gaV8 gaV8::operator*(float f) const
{
	return gaV8(lx * f, ly * f, lz * f, ax * f, ay * f, az * f);
}

inline gaV8 gaV8::operator/(float f) const
{
	float fInv = gaMath::ID / f;
	return gaV8(lx * fInv, ly * fInv, lz * fInv, ax * fInv, ay * fInv,
		az * fInv);
}

inline gaV8& gaV8::operator+=(const gaV8& v)
{
	lx += v.lx;
	ly += v.ly;
	lz += v.lz;
	ax += v.ax;
	ay += v.ay;
	az += v.az;
	return *this;
}

inline gaV8& gaV8::operator-=(const gaV8& v)
{
	lx -= v.lx;
	ly -= v.ly;
	lz -= v.lz;
	ax -= v.ax;
	ay -= v.ay;
	az -= v.az;
	return *this;
}

inline gaV8& gaV8::operator*=(float f)
{
	lx *= f;
	ly *= f;
	lz *= f;
	ax *= f;
	ay *= f;
	az *= f;
	return *this;
}

inline gaV8& gaV8::operator/=(float f)
{
	float fInv = gaMath::ID / f;
	lx *= fInv;
	ly *= fInv;
	lz *= fInv;
	ax *= fInv;
	ay *= fInv;
	az *= fInv;
	return *this;
}

inline void gaV8::SetLinear(const gaV3& v)
{
	lx = v.x;
	ly = v.y;
	lz = v.z;
}

inline void gaV8::SetAngular(const gaV3& v)
{
	ax = v.x;
	ay = v.y;
	az = v.z;
}

inline gaV3* gaV8::GetLinear()
{
	return (gaV3*)&lx;
}

inline gaV3* gaV8::GetAngular()
{
	return (gaV3*)&ax;
}

inline void gaV8::Rotate(const gaM3& mat)
{
	SetLinear(mat * *GetLinear());
	SetAngular(mat * *GetAngular());
}

inline gaV8 gaV8::RotatedNewVector(const gaM3& mat)
{
	return gaV8(mat * *GetLinear(), mat * *GetAngular());
}

inline gaT3::gaT3(const gaM3& r, const gaV3& t, const gaV3& s)
{
	R = r;
	T = t;
	S = s;
}

inline gaT3::gaT3(const gaT3& t)
{
	R = t.R;
	T = t.T;
	S = t.S;
}

inline gaT3& gaT3::SetIdentity()
{
	*this = ID;
	return *this;
}

inline gaV3 gaT3::Transform(gaV3& v) const
{
	return R * gaV3(S.x * v.x, S.y * v.y, S.z * v.z) + T;
}

inline gaM4 gaT3::ToM4LH()
{
	return ToM4LH(R, T, S);
}

inline gaM4 gaT3::ToM4RH()
{
	return ToM4RH(R, T, S);
}

inline gaQ::gaQ(const gaM3& mat)
{
	FromRotationMatrix(mat);
}

inline gaQ::gaQ(const gaV3* av)
{
	FromRotationMatrix(av);
}

inline gaQ::gaQ(const float* af)
{
	x = af[0];
	y = af[1];
	z = af[2];
	w = af[3];
}

inline gaQ::gaQ(float fx, float fy, float fz, float fw)
{
	x = fx;
	y = fy;
	z = fz;
	w = fw;
}

inline gaQ::gaQ(const gaQ& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

inline gaQ::gaQ(const gaV3& axis, float radian)
{
	FromAxisAngle(axis, radian);
}

inline gaQ& gaQ::SetZero()
{
	x = y = z = w = gaMath::ZERO;
	return *this;
}

inline gaQ& gaQ::Set(float fx, float fy, float fz, float fw)
{
	x = fx;
	y = fy;
	z = fz;
	w = fw;
	return *this;
}

inline gaQ& gaQ::Set(const gaV3& axis, float radian)
{
	FromAxisAngle(axis, radian);
	return *this;
}

inline gaQ& gaQ::Set(const gaQ& q)
{
	*this = q;
	return *this;
}

inline gaQ::operator float*()
{
	return &x;
}

inline gaQ::operator const float*() const
{
	return &x;
}

inline float& gaQ::operator[](int i)
{
	return (&x)[i];
}

inline float gaQ::operator[](int i) const
{
	return (&x)[i];
}

inline gaQ& gaQ::operator+=(const gaQ& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

inline gaQ& gaQ::operator-=(const gaQ& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

inline gaQ& gaQ::operator*=(const gaQ& q)
{
	return *this = *this * q;
}

inline gaQ& gaQ::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

inline gaQ& gaQ::operator/=(float f)
{
	float fInv = gaMath::ID / f;
	x *= fInv;
	y *= fInv;
	z *= fInv;
	w *= fInv;
	return *this;
}

inline gaQ gaQ::operator+() const
{
	return *this;
}

inline gaQ gaQ::operator+(const gaQ& q) const
{
	return gaQ(x + q.x, y + q.y, z + q.z, w + q.w);
}

inline gaQ gaQ::operator-() const
{
	return gaQ(-x, -y, -z, -w);
}

inline gaQ gaQ::operator-(const gaQ& q) const
{
	return gaQ(x - q.x, y - q.y, z - q.z, w - q.w);
}

inline gaQ gaQ::operator*(const gaQ& q) const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	return gaQ(w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w + z * q.x - x * q.z,
		w * q.z + z * q.w + x * q.y - y * q.x,
		w * q.w - x * q.x - y * q.y - z * q.z);
}

inline gaQ gaQ::operator*(float f) const
{
	return gaQ(x * f, y * f, z * f, w * f);
}

inline gaQ gaQ::operator/(float f) const
{
	float fInv = gaMath::ID / f;
	return gaQ(x * fInv, y * fInv, z * fInv, w * fInv);
}

inline bool gaQ::operator==(const gaQ& q) const
{
	return x == q.x && y == q.y && z == q.z && w == q.w;
}

inline bool gaQ::operator!=(const gaQ& q) const
{
	return x != q.x || y != q.y || z != q.z || w != q.w;
}

inline gaV3 gaQ::operator*(const gaV3& v) const
{
	gaM3 mat;
	ToRotationMatrix(mat);
	return mat * v;
}

inline void gaQ::FromRotationMatrix(const gaM3& mat)
{
	float fTrace = mat.m11 + mat.m22 + mat.m33;
	float fRoot;
	if (fTrace > gaMath::ZERO)
	{
		fRoot = gaMath::Sqrt(fTrace + gaMath::ID);
		w = gaMath::HALF * fRoot;
		fRoot = gaMath::HALF / fRoot;
		x = (mat.m32 - mat.m23) * fRoot;
		y = (mat.m13 - mat.m31) * fRoot;
		z = (mat.m21 - mat.m12) * fRoot;
	}
	else
	{
		static const int next[3] = { 1, 2, 0 };
		int i = 0;
		if (mat.m22 > mat.m11)
			i = 1;
		if (mat.m33 > mat.m[i][i])
			i = 2;
		int j = next[i];
		int k = next[j];
		fRoot =
			gaMath::Sqrt(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + gaMath::ID);
		(*this)[i] = gaMath::HALF * fRoot;
		fRoot = gaMath::HALF / fRoot;
		w = (mat.m[k][j] - mat.m[j][k]) * fRoot;
		(*this)[j] = (mat.m[j][i] + mat.m[i][j]) * fRoot;
		(*this)[k] = (mat.m[k][i] + mat.m[i][k]) * fRoot;
	}
}

inline void gaQ::FromRotationMatrix(const gaV3* av)
{
	gaM3 mat;
	mat.SetColumns(av[0], av[1], av[2]);
	FromRotationMatrix(mat);
}

inline void gaQ::ToRotationMatrix(gaM3& mat) const
{
	float fTx = gaMath::TWO * x;
	float fTy = gaMath::TWO * y;
	float fTz = gaMath::TWO * z;
	float fTwx = fTx * w;
	float fTwy = fTy * w;
	float fTwz = fTz * w;
	float fTxx = fTx * x;
	float fTxy = fTy * x;
	float fTxz = fTz * x;
	float fTyy = fTy * y;
	float fTyz = fTz * y;
	float fTzz = fTz * z;
	mat.m11 = gaMath::ID - (fTyy + fTzz);
	mat.m12 = fTxy - fTwz;
	mat.m13 = fTxz + fTwy;
	mat.m21 = fTxy + fTwz;
	mat.m22 = gaMath::ID - (fTxx + fTzz);
	mat.m23 = fTyz - fTwx;
	mat.m31 = fTxz - fTwy;
	mat.m32 = fTyz + fTwx;
	mat.m33 = gaMath::ID - (fTxx + fTyy);
}

inline void gaQ::ToRotationMatrix(gaV3* av) const
{
	gaM3 mat;
	ToRotationMatrix(mat);
	mat.GetColumns(av[0], av[1], av[2]);
}

inline void gaQ::FromAxisAngle(const gaV3& axis, float radian)
{
	radian *= gaMath::HALF;
	float fSin = gaMath::Sin(radian);
	x = fSin * axis.x;
	y = fSin * axis.y;
	z = fSin * axis.z;
	w = gaMath::Cos(radian);
}

inline void gaQ::ToAxisAngle(gaV3& axis, float& radian) const
{
	float fSqrLength = x * x + y * y + z * z;
	if (fSqrLength > gaMath::ZERO)
	{
		radian = gaMath::TWO * gaMath::ACos(w);
		float fInvLength = gaMath::InvSqrt(fSqrLength);
		axis.x = x * fInvLength;
		axis.y = y * fInvLength;
		axis.z = z * fInvLength;
	}
	else
	{
		radian = gaMath::ZERO;
		axis.x = gaMath::ID;
		axis.y = gaMath::ZERO;
		axis.z = gaMath::ZERO;
	}
}

inline void gaQ::FromTwoVectors(const gaV3& v0, const gaV3& v1)
{
	gaV3 bisector = v0 + v1;
	bisector.Normalize();
	w = v0.Dot(bisector);
	if (w != gaMath::ZERO)
	{
		gaV3 axis = v0.Cross(bisector);
		x = axis.x;
		y = axis.y;
		z = axis.z;
	}
	else if (gaMath::FAbs(v0.x) >= gaMath::FAbs(v0.y))
	{
		float inverseLength = gaMath::InvSqrt(v0.x * v0.x + v0.z * v0.z);
		x = -v0.z * inverseLength;
		y = gaMath::ZERO;
		z = v0.x * inverseLength;
	}
	else
	{
		float inverseLength = gaMath::InvSqrt(v0.y * v0.y + v0.z * v0.z);
		x = gaMath::ZERO;
		y = v0.z * inverseLength;
		z = -v0.y * inverseLength;
	}
}

inline float gaQ::Length() const
{
	return gaMath::Sqrt(x * x + y * y + z * z + w * w);
}

inline float gaQ::SquaredLength() const
{
	return x * x + y * y + z * z + w * w;
}

inline float gaQ::Normalize()
{
	float fLength = Length();

	if (fLength > gaMath::TOLERANCE)
	{
		float fInvLength = gaMath::ID / fLength;
		x *= fInvLength;
		y *= fInvLength;
		z *= fInvLength;
		w *= fInvLength;
	}
	else
	{
		fLength = gaMath::ZERO;
		x = gaMath::ZERO;
		y = gaMath::ZERO;
		z = gaMath::ZERO;
		w = gaMath::ZERO;
	}

	return fLength;
}

inline float gaQ::Dot(const gaQ& q) const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

inline gaQ gaQ::Inverse() const
{
	float fNorm = SquaredLength();
	if (fNorm > gaMath::ZERO)
		return Conjugate() / fNorm;
	return ZERO;
}

inline gaQ gaQ::Conjugate() const
{
	return gaQ(-x, -y, -z, w);
}

inline bool gaQ::IsFinite() const
{
	return gaMath::FAbs(x) <= gaMath::MAX_REAL &&
		gaMath::FAbs(y) <= gaMath::MAX_REAL &&
		gaMath::FAbs(z) <= gaMath::MAX_REAL &&
		gaMath::FAbs(w) <= gaMath::MAX_REAL;
}

inline gaQ gaQ::Slerp(float t, const gaQ& p, const gaQ& q)
{
	float angle = gaMath::ACos(p.Dot(q));
	if (gaMath::FAbs(angle) <= gaMath::TOLERANCE)
		return p;
	float inverseSin = gaMath::ID / gaMath::Sin(angle);
	float fromWeight = gaMath::Sin((gaMath::ID - t) * angle) * inverseSin;
	float toWeight = gaMath::Sin(t * angle) * inverseSin;
	return p * fromWeight + q * toWeight;
}

inline gaQ gaQ::SlerpExtraSpins(float t, const gaQ& p, const gaQ& q,
	int iExtraSpins)
{
	float angle = gaMath::ACos(p.Dot(q));
	if (gaMath::FAbs(angle) <= gaMath::TOLERANCE)
		return p;
	float phase = gaMath::PI * iExtraSpins * t;
	float inverseSin = gaMath::ID / gaMath::Sin(angle);
	float fromWeight =
		gaMath::Sin((gaMath::ID - t) * angle - phase) * inverseSin;
	float toWeight = gaMath::Sin(t * angle + phase) * inverseSin;
	return p * fromWeight + q * toWeight;
}

inline gaQ gaQ::Squad(float t, const gaQ& p, const gaQ& a, const gaQ& b,
	const gaQ& q)
{
	gaQ endpoints = Slerp(t, p, q);
	gaQ controls = Slerp(t, a, b);
	float blend = gaMath::TWO * t * (gaMath::ID - t);
	return Slerp(blend, endpoints, controls);
}

inline gaP4::gaP4(float fa, float fb, float fc, float fd)
{
	a = fa;
	b = fb;
	c = fc;
	d = fd;
}

inline gaM4::gaM4(const float* af)
{
	memcpy(m, af, sizeof(m));
}

inline gaM4::gaM4(const float af[4][4])
{
	memcpy(m, af, sizeof(m));
}

inline gaM4::gaM4(const gaM4& mat)
{
	memcpy(m, mat, sizeof(m));
}

inline gaM4::gaM4(float _11, float _12, float _13, float _14, float _21,
	float _22, float _23, float _24, float _31, float _32, float _33, float _34,
	float _41, float _42, float _43, float _44)
{
	m11 = _11;
	m12 = _12;
	m13 = _13;
	m14 = _14;
	m21 = _21;
	m22 = _22;
	m23 = _23;
	m24 = _24;
	m31 = _31;
	m32 = _32;
	m33 = _33;
	m34 = _34;
	m41 = _41;
	m42 = _42;
	m43 = _43;
	m44 = _44;
}

inline void gaM4::SetZero()
{
	memset(m, 0, sizeof(m));
}

inline void gaM4::SetIdentity()
{
	SetZero();
	m11 = m22 = m33 = m44 = gaMath::ID;
}

inline gaM4::operator float*()
{
	return &m11;
}

inline gaM4::operator const float*() const
{
	return &m11;
}
