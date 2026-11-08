inline int WisEqual(const WVector& left, const WVector& right, float epsilon)
{
	if (WisEqual(left.x, right.x, epsilon) &&
		WisEqual(left.y, right.y, epsilon) &&
		WisEqual(left.z, right.z, epsilon))
		return 1;
	else
		return 0;
}

inline WVector WVector::operator-() const
{
	return WVector(-x, -y, -z);
}

inline WVector operator+(const WVector& left, const WVector& right)
{
	return WVector(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline WVector operator-(const WVector& left, const WVector& right)
{
	return WVector(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline WVector operator*(const WVector& vector, float scalar)
{
	return WVector(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

inline WVector operator/(const WVector& vector, float scalar)
{
	return vector * (1.0f / scalar);
}

inline WVector operator*(float scalar, const WVector& vector)
{
	return vector * scalar;
}

inline int operator==(const WVector& a, const WVector& b)
{
	return WisEqual(a, b, g_EPSILON);
}

inline int operator!=(const WVector& a, const WVector& b)
{
	return !(a == b);
}

inline float operator*(const WVector& left, const WVector& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline float WDotProduct(const WVector& left, const WVector& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline WVector WCrossProduct(const WVector& left, const WVector& right)
{
	return WVector(left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x);
}

inline float WVectorLen(const WVector& vector)
{
	return sqrtf(
		vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

#if defined(REBANG_LEGACY_CPP) && !defined(WANGREAL_DEVICE)
inline WVector& WVector::operator=(const WVector& other)
{
	memcpy(this, &other, sizeof(WVector));
	return *this;
}
#endif

inline void WVector::operator+=(const WVector& right)
{
	x = right.x + x;
	y = right.y + y;
	z = right.z + z;
}

inline void WVector::operator-=(const WVector& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

inline void WVector::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

inline void WVector::operator/=(float scalar)
{
	*this *= 1.0f / scalar;
}

inline void WVector::Reset()
{
	x = y = z = 0.0f;
}

inline float WVector::SquareMagnitude() const
{
	return x * x + y * y + z * z;
}

inline float WVector::Magnitude() const
{
	return WVectorLen(*this);
}

inline WVector& WVector::Normalize()
{
	if (WisEqual(*this, ZERO, g_EPSILON))
		*this *= 0.0f;
	else
		*this *= (float)((1.0f / (Magnitude())));
	return *this;
}

inline void WVector::Negate()
{
	x = -x;
	y = -y;
	z = -z;
}

inline WVector operator*(const WVector& vector, const WMatrix& matrix)
{
	return WVector(vector.x * matrix.xx + vector.y * matrix.xy +
			vector.z * matrix.xz + matrix.xm,
		vector.x * matrix.yx + vector.y * matrix.yy + vector.z * matrix.yz +
			matrix.ym,
		vector.x * matrix.zx + vector.y * matrix.zy + vector.z * matrix.zz +
			matrix.zm);
}

inline WMatrix operator*(const WMatrix& left, const WMatrix& right)
{
	WMatrix m;
	m.xx = left.xx * right.xx + left.yx * right.xy + left.zx * right.xz;
	m.yx = left.xx * right.yx + left.yx * right.yy + left.zx * right.yz;
	m.zx = left.xx * right.zx + left.yx * right.zy + left.zx * right.zz;
	m.xy = left.xy * right.xx + left.yy * right.xy + left.zy * right.xz;
	m.yy = left.xy * right.yx + left.yy * right.yy + left.zy * right.yz;
	m.zy = left.xy * right.zx + left.yy * right.zy + left.zy * right.zz;
	m.xz = left.xz * right.xx + left.yz * right.xy + left.zz * right.xz;
	m.yz = left.xz * right.yx + left.yz * right.yy + left.zz * right.yz;
	m.zz = left.xz * right.zx + left.yz * right.zy + left.zz * right.zz;
	m.xm =
		left.xm * right.xx + left.ym * right.xy + left.zm * right.xz + right.xm;
	m.ym =
		left.xm * right.yx + left.ym * right.yy + left.zm * right.yz + right.ym;
	m.zm =
		left.xm * right.zx + left.ym * right.zy + left.zm * right.zz + right.zm;
	return m;
}

inline WMatrix operator*(const WMatrix& matrix, float scalar)
{
	WMatrix result;
	result.xa = matrix.xa * scalar;
	result.ya = matrix.ya * scalar;
	result.za = matrix.za * scalar;
	result.pivot = matrix.pivot;
	return result;
}

#ifndef WANGREAL_DEVICE
inline WMatrix RotMat(WVector rotation)
{
	return WMatrix(1.0f, 0.0f, 0.0f, 0.0f, cosf(rotation.x), -sinf(rotation.x),
			   0.0f, sinf(rotation.x), cosf(rotation.x)) *
		WMatrix(cosf(rotation.y), 0.0f, sinf(rotation.y), 0.0f, 1.0f, 0.0f,
			-sinf(rotation.y), 0.0f, cosf(rotation.y)) *
		WMatrix(cosf(rotation.z), sinf(rotation.z), 0.0f, -sinf(rotation.z),
			cosf(rotation.z), 0.0f, 0.0f, 0.0f, 1.0f);
}

inline WMatrix RotMat(float angle, char direct)
{
	float c = cosf(angle);
	float s = sinf(angle);

	switch (direct)
	{
	case 0:
		return WMatrix(1.0f, 0.0f, 0.0f, 0.0f, c, -s, 0.0f, s, c);
	case 2:
		return WMatrix(c, s, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 1.0f);
	default:
		return WMatrix(c, 0.0f, s, 0.0f, 1.0f, 0.0f, -s, 0.0f, c);
	}
}

inline WMatrix RotMat(const WVector& axis, float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	WVector v = axis;
	v.Normalize();

	return WMatrix((v.x * v.x) * (1.0f - c) + c,
		(v.x * v.y) * (1.0f - c) - v.z * s, (v.x * v.z) * (1.0f - c) + v.y * s,
		(v.x * v.y) * (1.0f - c) + v.z * s, (v.y * v.y) * (1.0f - c) + c,
		(v.y * v.z) * (1.0f - c) - v.x * s, (v.x * v.z) * (1.0f - c) - v.y * s,
		(v.y * v.z) * (1.0f - c) + v.x * s, (v.z * v.z) * (1.0f - c) + c);
}
#endif

inline WVector RotVec(const WVector& vector, const WMatrix& matrix)
{
	return WVector(vector.x * matrix.xx + vector.y * matrix.xy +
			vector.z * matrix.xz,
		vector.x * matrix.yx + vector.y * matrix.yy + vector.z * matrix.yz,
		vector.x * matrix.zx + vector.y * matrix.zy + vector.z * matrix.zz);
}

inline float __fastcall TransformZ(float x, float y, float z, const WMatrix& m)
{
	return x * m.zx + y * m.zy + z * m.zz + m.zm;
}

inline float __fastcall TransformZ(const WVector& v, const WMatrix& m)
{
	return (v.z * m.zz) + (v.x * m.zx) + (v.y * m.zy) + m.zm;
}

inline WMatrix& WMatrix::operator=(const WMatrix& other)
{
	memcpy(&xa, &other.xa, sizeof(WVector));
	memcpy(&ya, &other.ya, sizeof(WVector));
	memcpy(&za, &other.za, sizeof(WVector));
	memcpy(&pivot, &other.pivot, sizeof(WVector));
	return *this;
}

inline void WMatrix::operator*=(float scalar)
{
	xa *= scalar;
	ya *= scalar;
	za *= scalar;
}

inline void WMatrix::operator/=(float scalar)
{
	float inv = 1.0f / scalar;
	xa *= inv;
	ya *= inv;
	za *= inv;
}

inline WMatrix WMatrix::operator~() const
{
	WMatrix r;
	WVector c;
	c.x = (float)(double)(yy * zz - yz * zy);
	c.y = (float)(double)(xz * zy - zz * xy);
	c.z = (float)(double)(yz * xy - yy * xz);
	float inv = 1.0f / (xx * c.x + yx * c.y + zx * c.z);
	r.xx = c.x * inv;
	r.yx = (yz * zx - yx * zz) * inv;
	r.zx = (yx * zy - zx * yy) * inv;
	r.xy = c.y * inv;
	r.yy = (xx * zz - zx * xz) * inv;
	r.zy = (zx * xy - xx * zy) * inv;
	r.xz = c.z * inv;
	r.yz = (yx * xz - yz * xx) * inv;
	r.zz = (yy * xx - yx * xy) * inv;
	r.xm = -(r.xx * xm + r.xy * ym + r.xz * zm);
	r.ym = -(r.yx * xm + r.yy * ym + r.yz * zm);
	r.zm = -(r.zx * xm + r.zy * ym + r.zz * zm);
	return r;
}

#ifndef WANGREAL_DEVICE
inline void WMatrix::operator*=(const WMatrix& right)
{
	*this = *this * right;
}
#endif

inline WMatrix::WMatrix(float xx_, float yx_, float zx_, float xy_, float yy_,
	float zy_, float xz_, float yz_, float zz_)
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
	xm = ym = zm = 0.0f;
}

inline WMatrix::WMatrix(const WPlane& source)
{
	WPlane plane = source;
	plane.Normalize();
	xx = 1 - 2 * plane.x * plane.x;
	yx = -2 * plane.x * plane.y;
	zx = -2 * plane.x * plane.z;
	xy = -2 * plane.x * plane.y;
	yy = 1 - 2 * plane.y * plane.y;
	zy = -2 * plane.y * plane.z;
	xz = -2 * plane.x * plane.z;
	yz = -2 * plane.y * plane.z;
	zz = 1 - 2 * plane.z * plane.z;
	xm = -2 * plane.x * plane.dis;
	ym = -2 * plane.y * plane.dis;
	zm = -2 * plane.z * plane.dis;
}

inline void WMatrix::Reset()
{
	xx = yy = zz = 1.0f;
	yx = zx = xy = zy = xz = yz = xm = ym = zm = 0.0f;
}

inline void WMatrix::Transpose()
{
	float xy_ = xy;
	float xz_ = xz;
	float yz_ = yz;
	xy = yx;
	yx = xy_;
	xz = zx;
	zx = xz_;
	yz = zy;
	zy = yz_;
}

inline bool operator==(const WMatrix& left, const WMatrix& right)
{
	return WisEqual(left.xa, right.xa, 1.0e-5f) &&
		WisEqual(left.ya, right.ya, 1.0e-5f) &&
		WisEqual(left.za, right.za, 1.0e-5f) &&
		WisEqual(left.pivot, right.pivot, 1.0e-5f);
}

inline bool operator!=(const WMatrix& left, const WMatrix& right)
{
	return !(left == right);
}

inline WMatrix4 operator*(const WMatrix4& a, const WMatrix4& b)
{
	WMatrix4 m;
	m.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] +
		a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
	m.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] +
		a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
	m.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] +
		a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
	m.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] +
		a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];

	m.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] +
		a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
	m.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] +
		a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
	m.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] +
		a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
	m.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] +
		a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];

	m.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] +
		a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
	m.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] +
		a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
	m.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] +
		a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
	m.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] +
		a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];

	m.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] +
		a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];
	m.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] +
		a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];
	m.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] +
		a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];
	m.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] +
		a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];
	return m;
}

inline void __fastcall SetWMatrix4FromWMatrix(WMatrix4& d, const WMatrix& s)
{
	for (int i = 0; i < 3; ++i)
	{
		*(WVector*)d.m[i] = (&s.xa)[i];
		d.m[i][3] = 0.0f;
	}
	*(WVector*)d.m[3] = s.pivot;
	d.m[3][3] = 1.0f;
}

inline WMatrix4::WMatrix4(float p0, float p1, float p2, float p3, float p4,
	float p5, float p6, float p7, float p8, float p9, float p10, float p11,
	float p12, float p13, float p14, float p15)
{
	p[0] = p0;
	p[1] = p1;
	p[2] = p2;
	p[3] = p3;
	p[4] = p4;
	p[5] = p5;
	p[6] = p6;
	p[7] = p7;
	p[8] = p8;
	p[9] = p9;
	p[10] = p10;
	p[11] = p11;
	p[12] = p12;
	p[13] = p13;
	p[14] = p14;
	p[15] = p15;
}

inline WMatrix4& WMatrix4::operator=(const WMatrix4& other)
{
	memcpy(this, &other, sizeof(WMatrix4));
	return *this;
}

inline WVector2D::WVector2D(const float* v)
{
	x = v[0];
	y = v[1];
}

inline WVector2D operator*(float scale, const WVector2D& v)
{
	return WVector2D(scale * v.x, scale * v.y);
}

inline WVector2D operator*(const WVector2D& v, float scale)
{
	return WVector2D(v.x * scale, v.y * scale);
}

inline WVector2D operator+(const WVector2D& a, const WVector2D& b)
{
	return WVector2D(a.x + b.x, a.y + b.y);
}

inline WVector2D operator-(const WVector2D& a, const WVector2D& b)
{
	return WVector2D(a.x - b.x, a.y - b.y);
}

inline void WMatrix4::operator*=(const WMatrix4& other)
{
	*this = *this * other;
}

inline void WVector2D::Reset()
{
	x = y = 0.0f;
}

inline float WVector2D::Magnitude() const
{
	return sqrtf(SquareMagnitude());
}

inline float WVector2D::SquareMagnitude() const
{
	return x * x + y * y;
}

inline void WVector2D::Negate()
{
	x = -x;
	y = -y;
}

#ifdef REBANG_LEGACY_CPP
inline WVector2D& WVector2D::operator=(const WVector2D& other)
{
	x = other.x;
	y = other.y;
	return *this;
}
#endif

inline void WVector2D::operator+=(const WVector2D& v)
{
	x += v.x;
	y += v.y;
}

inline void WVector2D::operator-=(const WVector2D& v)
{
	x -= v.x;
	y -= v.y;
}

inline void WVector2D::operator*=(float f)
{
	x *= f;
	y *= f;
}

inline void WVector2D::operator/=(float f)
{
	float inv = 1.0f / f;
	x *= inv;
	y *= inv;
}

inline WVector2D& WVector2D::Normalize()
{
	if (WisZero(x) && WisZero(y))
		*this *= 0.0f;
	else
		*this *= 1.0f / Magnitude();
	return *this;
}

inline WVector4::WVector4(float x_, float y_, float z_, float w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

inline float operator*(const WPlane& plane, const WVector& vector)
{
	return plane.normal * vector + plane.dis;
}

inline float operator*(const WVector& vector, const WPlane& plane)
{
	const WVector& value = vector;
	return plane.normal * value + plane.dis;
}

inline void WPlane::Normalize()
{
	float scale = 1.0f / sqrtf(x * x + y * y + z * z);
	x *= scale;
	y *= scale;
	z *= scale;
	dis *= scale;
}

inline WPlane::WPlane(const WVector& normal_, const WVector& point)
	: normal(normal_)
{
	dis = -(normal * point);
}

inline WPlane& WPlane::operator=(const WPlane& other)
{
	normal = other.normal;
	dis = other.dis;
	return *this;
}

inline float WDotProduct(const WQuat& left, const WQuat& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z +
		left.w * right.w;
}

inline WQuat WQuaternionSlerp(const WQuat& from, const WQuat& to, float t)
{
	WQuat from1;
	float omega, cosom, sinom, scale0, scale1;

	cosom = WDotProduct(from, to);

	if (cosom < 0.0f)
	{
		from1 = WQuat(-from.x, -from.y, -from.z, -from.w);
		cosom = -cosom;
	}
	else
	{
		from1 = from;
	}

	if ((1.0f + cosom) > 0.05f)
	{
		if ((1.0f - cosom) < 0.05f)
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}
		else
		{
			omega = acosf(cosom);
			sinom = sinf(omega);

			scale0 = sinf((1.0f - t) * omega) / sinom;
			scale1 = sinf(t * omega) / sinom;
		}

		return WQuat(scale0 * from1.x + scale1 * to.x,
			scale0 * from1.y + scale1 * to.y, scale0 * from1.z + scale1 * to.z,
			scale0 * from1.w + scale1 * to.w);
	}
	else
	{
		scale0 = sinf((1.0f - t) * g_PI_DIV_2);
		scale1 = sinf(t * g_PI_DIV_2);

		return WQuat(scale0 * from1.x - scale1 * from1.y,
			scale0 * from1.y + scale1 * from1.x,
			scale0 * from1.z - scale1 * from1.w,
			scale0 * from1.w + scale1 * from1.z);
	}
}

inline WQuat::WQuat(const WVector& axis, float angle)
{
	SetFromAxisAngle(axis, angle);
}

inline WQuat::WQuat(const float* v)
{
	x = v[0];
	y = v[1];
	z = v[2];
	w = v[3];
}

inline WQuat& WQuat::operator=(const WQuat& other)
{
	memcpy(this, &other, sizeof(WQuat));
	return *this;
}

inline int operator&(const Waabb& left, const Waabb& right)
{
	for (int i = 0; i < 3; i++)
	{
		if (left.min.p[i] > right.max.p[i] || left.max.p[i] < right.min.p[i])
			return 0;
	}
	return 1;
}

inline Waabb operator+(const Waabb& box, const WVector& vector)
{
	Waabb result = box;
	for (int i = 0; i < 3; i++)
	{
		result.max.p[i] += vector.p[i];
		result.min.p[i] += vector.p[i];
	}
	return result;
}

#ifndef WANGREAL_DEVICE
inline Waabb ExpandAABB(const Waabb& a, const Waabb& b)
{
	return Waabb(WVector(Min(a.min.x, b.min.x), Min(a.min.y, b.min.y),
					 Min(a.min.z, b.min.z)),
		WVector(Max(a.max.x, b.max.x), Max(a.max.y, b.max.y),
			Max(a.max.z, b.max.z)));
}
#endif

inline Waabb& Waabb::operator=(const Waabb& box)
{
	min = box.min;
	max = box.max;
	return *this;
}

inline Waabb& Waabb::operator+=(const WVector& vec)
{
	if (vec.x > 0.0f)
		max.x += vec.x;
	else
		min.x += vec.x;
	if (vec.y > 0.0f)
		max.y += vec.y;
	else
		min.y += vec.y;
	if (vec.z > 0.0f)
		max.z += vec.z;
	else
		min.z += vec.z;
	return *this;
}

inline Waabb& Waabb::operator+=(const Waabb& box)
{
	AddPoint(box.min);
	AddPoint(box.max);
	return *this;
}

inline void Wobb::operator+=(const WVector& vector)
{
	center.x += vector.x;
	center.y += vector.y;
	center.z += vector.z;
}

inline void Wobb::operator*=(float scale)
{
	extend[0] *= scale;
	extend[1] *= scale;
	extend[2] *= scale;
}

inline void Wobb::operator*=(const WVector& scale)
{
	extend[0] *= scale.x;
	extend[1] *= scale.y;
	extend[2] *= scale.z;
}

inline Wobb& Wobb::operator=(const Wobb& obb)
{
	center = obb.center;
	extend[0] = obb.extend[0];
	extend[1] = obb.extend[1];
	extend[2] = obb.extend[2];
	return *this;
}

inline Wobb& Wobb::operator=(const Waabb& box)
{
	center = (box.min + box.max) * 0.5f;
	extend[0] = WVector(box.max.x - box.min.x, 0.0f, 0.0f) * 0.5f;
	extend[1] = WVector(0.0f, box.max.y - box.min.y, 0.0f) * 0.5f;
	extend[2] = WVector(0.0f, 0.0f, box.max.z - box.min.z) * 0.5f;
	return *this;
}

inline Wobb& Wobb::operator=(const WMatrix& mat)
{
	center = mat.pivot;
	extend[0] = mat.xa;
	extend[1] = mat.ya;
	extend[2] = mat.za;
	return *this;
}

inline bool Wobb::IsInclude(const WVector& point) const
{
	WVector direction = point - center;
	if (Abs(direction * extend[0]) <= (extend[0] * extend[0]) &&
		Abs(direction * extend[1]) <= (extend[1] * extend[1]) &&
		Abs(direction * extend[2]) <= (extend[2] * extend[2]))
		return true;
	return false;
}

inline void Wobb::Normalize()
{
	extend[0].Normalize();
	extend[1].Normalize();
	extend[2].Normalize();
}
