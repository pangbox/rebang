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

inline WVector operator*(float scalar, const WVector& vector)
{
	return vector * scalar;
}

inline float operator*(const WVector& left, const WVector& right)
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

#ifdef REBANG_LEGACY_CPP
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

inline float operator*(const WPlane& plane, const WVector& vector)
{
	return plane.normal * vector + plane.dis;
}

inline float operator*(const WVector& vector, const WPlane& plane)
{
	const WVector& value = vector;
	return plane.normal * value + plane.dis;
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
	WQuat from1 = from;
	float omega, cosom, sinom, scale0, scale1;

	cosom = WDotProduct(from, to);

	if (cosom < 0.0f)
	{
		from1.x = -from.x;
		from1.y = -from.y;
		from1.z = -from.z;
		from1.w = -from.w;
		cosom = -cosom;
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

inline bool Wobb::IsInclude(const WVector& point) const
{
	WVector direction = point - center;
	return Abs(direction * extend[0]) <= (extend[0] * extend[0]) &&
		Abs(direction * extend[1]) <= (extend[1] * extend[1]) &&
		Abs(direction * extend[2]) <= (extend[2] * extend[2]);
}

inline WVector4::WVector4(float x_, float y_, float z_, float w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}
