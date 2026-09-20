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

inline float operator*(const WVector& left, const WVector& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

__forceinline WVector WCrossProduct(const WVector& left, const WVector& right)
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

inline float WVector::SquareMagnitude() const
{
	return *this * *this;
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

inline Waabb::Waabb()
{
}

inline Waabb::Waabb(const WVector& minimum, const WVector& maximum)
{
	min = minimum;
	max = maximum;
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

inline WPlane::WPlane(const WVector& normal_, const WVector& point)
{
	normal = normal_;
	dis = -(normal * point);
}
