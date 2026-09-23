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

inline float WVectorLen(const WVector& vector)
{
	return (float)sqrt(vector.SquareMagnitude());
}

inline WVector WCrossProduct(const WVector& left, const WVector& right)
{
	return WVector(left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x);
}

inline void WVector::operator+=(const WVector& right)
{
	x += right.x;
	y += right.y;
	z += right.z;
}

#ifdef REBANG_LEGACY_CPP
inline WVector& WVector::operator=(const WVector& other)
{
	memcpy(this, &other, sizeof(WVector));
	return *this;
}
#endif

inline float WVector::SquareMagnitude() const
{
	return *this * *this;
}

inline void WVector::Reset()
{
	x = y = z = 0.0f;
}

inline float WVector::Magnitude() const
{
	return (float)sqrt(SquareMagnitude());
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

inline int operator&(const Waabb& left, const Waabb& right)
{
	for (int i = 0; i < 3; i++)
	{
		if (left.min.p[i] > right.max.p[i] || left.max.p[i] < right.min.p[i])
			return 0;
	}
	return 1;
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

inline bool Wobb::IsInclude(const WVector& point) const
{
	WVector direction = point - center;
	return Abs(direction * extend[0]) <= (extend[0] * extend[0]) &&
		Abs(direction * extend[1]) <= (extend[1] * extend[1]) &&
		Abs(direction * extend[2]) <= (extend[2] * extend[2]);
}

inline float __fastcall TransformZ(float x, float y, float z, const WMatrix& m)
{
	return x * m.zx + y * m.zy + z * m.zz + m.zm;
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

inline WPlane::WPlane(const WVector& normal_, const WVector& point)
	: normal(normal_)
{
	dis = -(normal * point);
}

inline int WisEqual(const WVector& left, const WVector& right, float epsilon)
{
	return WisEqual(left.x, right.x, epsilon) &&
		WisEqual(left.y, right.y, epsilon) &&
		WisEqual(left.z, right.z, epsilon);
}

inline WVector& WVector::Normalize()
{
	if (WisEqual(*this, ZERO, g_EPSILON))
		*this *= 0.0f;
	else
		*this *= 1.0f / Magnitude();
	return *this;
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

inline float __fastcall TransformZ(const WVector& v, const WMatrix& m)
{
	return (v.z * m.zz) + (v.x * m.zx) + (v.y * m.zy) + m.zm;
}

inline WVector RotVec(const WVector& vector, const WMatrix& matrix)
{
	return WVector(vector.x * matrix.xx + vector.y * matrix.xy +
			vector.z * matrix.xz,
		vector.x * matrix.yx + vector.y * matrix.yy + vector.z * matrix.yz,
		vector.x * matrix.zx + vector.y * matrix.zy + vector.z * matrix.zz);
}

inline WMatrix& WMatrix::operator=(const WMatrix& other)
{
	xa = other.xa;
	ya = other.ya;
	za = other.za;
	pivot = other.pivot;
	return *this;
}

inline WPlane& WPlane::operator=(const WPlane& other)
{
	normal = other.normal;
	dis = other.dis;
	return *this;
}
