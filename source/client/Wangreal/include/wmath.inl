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
