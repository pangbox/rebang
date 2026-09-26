// This source file is based mostly on Dave Eberly's Wild Magic.
// https://www.geometrictools.com/

#include <stdlib.h>
#include "gamath.h"

static union
{
	unsigned char c[4];
	float s;
} _inf = {
	{ 0x00, 0x00, 0x80, 0x7f }
};

const float gaMath::ZERO = 0.0f;
const float gaMath::HALF = 0.5f;
const float gaMath::ID = 1.0f;
const float gaMath::TWO = 2.0f;
const float gaMath::MAX_REAL = 3.402823466e+38f;
const float gaMath::EPSILON = 1.192092896e-07f;
const float gaMath::TOLERANCE = 1e-06f;
const float gaMath::INF = _inf.s;
const float gaMath::PI = 3.14159265358979323846f;
const float gaMath::TWO_PI = 2.0f * gaMath::PI;
const float gaMath::HALF_PI = 0.5f * gaMath::PI;
const float gaMath::INV_PI = 1.0f / gaMath::PI;
const float gaMath::INV_HALF_PI = 1.0f / gaMath::HALF_PI;
const float gaMath::INV_TWO_PI = 1.0f / gaMath::TWO_PI;
const float gaMath::TO_RADIAN = gaMath::PI / 180.0f;
const float gaMath::TO_DEGREE = 180.0f / gaMath::PI;
const float gaMath::SQRT_HALF = 0.7071067811865475244f;

float gaMath::UnitRandom(unsigned int seed)
{
	if (seed > 0)
		srand(seed);

	return (float)rand() / (float)RAND_MAX;
}

float gaMath::SymmetricRandom(unsigned int seed)
{
	if (seed > 0)
		srand(seed);

	return 2.0f * (float)rand() / (float)RAND_MAX - ID;
}

float gaMath::SymmetricIntervalRandom(float halfExtent, unsigned int seed)
{
	if (seed > 0)
		srand(seed);

	return 2.0f * halfExtent * (float)rand() / (float)RAND_MAX - halfExtent;
}

float gaMath::SymmetricHalfRandom(unsigned int seed)
{
	if (seed > 0)
		srand(seed);

	return (float)rand() / (float)RAND_MAX - HALF;
}

float gaMath::IntervalRandom(float min, float max, unsigned int seed)
{
	if (seed > 0)
		srand(seed);

	return min + (max - min) * (float)rand() / (float)RAND_MAX;
}

void gaMath::UnitRandomArray(float* dest, int len, unsigned int seed)
{
	int i;
	if (seed > 0)
		srand(seed);

	for (i = 0; i < len; i++)
		dest[i] = (float)rand() / (float)RAND_MAX;
}

void gaMath::SymmetricRandomArray(float* dest, int len, unsigned int seed)
{
	int i;
	if (seed > 0)
		srand(seed);

	for (i = 0; i < len; i++)
		dest[i] = 2.0f * (float)rand() / (float)RAND_MAX - ID;
}

const gaV2 gaV2::ZERO(0.0f, 0.0f);
const gaV2 gaV2::ONE(1.0f, 1.0f);
const gaV2 gaV2::UNIT_X(1.0f, 0.0f);
const gaV2 gaV2::UNIT_Y(0.0f, 1.0f);
const gaV2 gaV2::MAX(3.402823466e+38f, 3.402823466e+38f);
const gaV2 gaV2::INF(gaMath::INF, gaMath::INF);

gaV2 gaV2::UnitRandom(unsigned int seed)
{
	float x = gaMath::UnitRandom(seed);
	float y = gaMath::UnitRandom();
	return gaV2(x, y);
}

void gaV2::Orthonormalize(gaV2* const av)
{
	// compute u0
	av[0].Normalize();

	// compute u1
	float fDot0 = av[0].Dot(av[1]);
	av[1] -= av[0] * fDot0;
	av[1].Normalize();
}

const gaV3 gaV3::ZERO(0.0f, 0.0f, 0.0f);
const gaV3 gaV3::ONE(1.0f, 1.0f, 1.0f);
const gaV3 gaV3::UNIT_X(1.0f, 0.0f, 0.0f);
const gaV3 gaV3::UNIT_Y(0.0f, 1.0f, 0.0f);
const gaV3 gaV3::UNIT_Z(0.0f, 0.0f, 1.0f);
const gaV3 gaV3::MAX(3.402823466e+38f, 3.402823466e+38f, 3.402823466e+38f);
const gaV3 gaV3::INF(gaMath::INF, gaMath::INF, gaMath::INF);

gaV3 gaV3::UnitRandom(unsigned int seed)
{
	float z = gaMath::SymmetricRandom(seed);
	float r = gaMath::Sqrt(gaMath::ID - z * z);
	float t = gaMath::UnitRandom() * gaMath::TWO_PI;
	return gaV3(r * gaMath::Cos(t), r * gaMath::Sin(t), z);
}

void gaV3::Orthonormalize(gaV3* const av)
{
	// If the input vectors are v0, v1, and v2, then the Gram-Schmidt
	// orthonormalization produces vectors u0, u1, and u2 as follows,
	//
	//   u0 = v0/|v0|
	//   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
	//   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
	//
	// where |A| indicates length of vector A and A*B indicates dot
	// product of vectors A and B.

	// compute u0
	av[0].Normalize();

	// compute u1
	float fDot0 = av[0].Dot(av[1]);
	av[1] -= fDot0 * av[0];
	av[1].Normalize();

	// compute u2
	float fDot1 = av[1].Dot(av[2]);
	fDot0 = av[0].Dot(av[2]);
	av[2] -= fDot0 * av[0] + fDot1 * av[1];
	av[2].Normalize();
}

void gaV3::GenerateOrthonormalBasis(gaV3& U, gaV3& V, gaV3& W,
	bool bUnitLengthW)
{
	if (!bUnitLengthW)
		W.Normalize();

	float fInvLength;

	if (gaMath::FAbs(W.x) >= gaMath::FAbs(W.y))
	{
		// W.x or W.z is the largest magnitude component, swap them
		fInvLength = gaMath::InvSqrt(W.x * W.x + W.z * W.z);
		U.x = -W.z * fInvLength;
		U.y = 0.0f;
		U.z = +W.x * fInvLength;
	}
	else
	{
		// W.y or W.z is the largest magnitude component, swap them
		fInvLength = gaMath::InvSqrt(W.y * W.y + W.z * W.z);
		U.x = 0.0f;
		U.y = +W.z * fInvLength;
		U.z = -W.y * fInvLength;
	}

	V = W.Cross(U);
}

void gaV3::GenerateOrthonormalBasis2(gaV3& N, gaV3& T, gaV3& K,
	bool bUnitLengthN)
{
	if (!bUnitLengthN)
		N.Normalize();

	if (gaMath::FAbs(N.z) > gaMath::SQRT_HALF)
	{
		// choose p in y-z plane
		float a = N.y * N.y + N.z * N.z;
		float k = gaMath::InvSqrt(a);
		T.x = 0;
		T.y = -N.z * k;
		T.z = N.y * k;
		// set q = n x p
		K.x = a * k;
		K.y = -N.x * T.z;
		K.z = N.x * T.y;
	}
	else
	{
		// choose p in x-y plane
		float a = N.x * N.x + N.y * N.y;
		float k = gaMath::InvSqrt(a);
		T.x = -N.y * k;
		T.y = N.x * k;
		T.z = 0;
		// set q = n x p
		K.x = -N.z * T.y;
		K.y = N.z * T.x;
		K.z = a * k;
	}
}

const gaV8 gaV8::ZERO(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const gaV8 gaV8::UNIT_LX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const gaV8 gaV8::UNIT_LY(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const gaV8 gaV8::UNIT_LZ(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
const gaV8 gaV8::UNIT_AX(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
const gaV8 gaV8::UNIT_AY(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
const gaV8 gaV8::UNIT_AZ(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

const gaM3 gaM3::ZERO(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const gaM3 gaM3::ID(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

void gaM3::Orthonormalize()
{
	// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
	// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	// compute q0
	float fInvLength = gaMath::InvSqrt(
		m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);

	m[0][0] *= fInvLength;
	m[1][0] *= fInvLength;
	m[2][0] *= fInvLength;

	// compute q1
	float fDot0 = m[0][0] * m[0][1] + m[1][0] * m[1][1] + m[2][0] * m[2][1];

	m[0][1] -= fDot0 * m[0][0];
	m[1][1] -= fDot0 * m[1][0];
	m[2][1] -= fDot0 * m[2][0];

	fInvLength = gaMath::InvSqrt(
		m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);

	m[0][1] *= fInvLength;
	m[1][1] *= fInvLength;
	m[2][1] *= fInvLength;

	// compute q2
	float fDot1 = m[0][1] * m[0][2] + m[1][1] * m[1][2] + m[2][1] * m[2][2];

	fDot0 = m[0][0] * m[0][2] + m[1][0] * m[1][2] + m[2][0] * m[2][2];

	m[0][2] -= fDot0 * m[0][0] + fDot1 * m[0][1];
	m[1][2] -= fDot0 * m[1][0] + fDot1 * m[1][1];
	m[2][2] -= fDot0 * m[2][0] + fDot1 * m[2][1];

	fInvLength = gaMath::InvSqrt(
		m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);

	m[0][2] *= fInvLength;
	m[1][2] *= fInvLength;
	m[2][2] *= fInvLength;
}

bool gaM3::CholeskyDecomposition()
{
	int i, j, k;
	float sum, *a, *b, *aa, *bb, *cc;
	float recip[3];

	aa = &m11;
	for (i = 0; i < 3; i++)
	{
		bb = &m11;
		cc = &m11 + i * 3;
		for (j = 0; j < i; j++)
		{
			sum = *cc;
			a = aa;
			b = bb;
			for (k = j; k; k--)
				sum -= (*(a++)) * (*(b++));
			*cc = sum * recip[j];
			bb += 3;
			cc++;
		}
		sum = *cc;
		a = aa;
		for (k = i; k; k--, a++)
			sum -= (*a) * (*a);
		if (sum <= gaMath::ZERO)
			return false;
		*cc = gaMath::Sqrt(sum);
		recip[i] = gaMath::ID / *cc;
		aa += 3;
	}
	return true;
}

void gaM3::ToAxisAngle(gaV3& axis, float& radian) const
{
	float fCos = (m[0][0] + m[1][1] + m[2][2] - gaMath::ID) * gaMath::HALF;
	radian = gaMath::ACos(fCos);

	if (radian > gaMath::ZERO)
	{
		if (radian < gaMath::PI)
		{
			axis.x = m[2][1] - m[1][2];
			axis.y = m[0][2] - m[2][0];
			axis.z = m[1][0] - m[0][1];
			axis.Normalize();
		}
		else
		{
			// angle is PI
			float fHalfInverse;
			if (m[0][0] >= m[1][1])
			{
				// r00 >= r11
				if (m[0][0] >= m[2][2])
				{
					// r00 is maximum diagonal term
					axis.x = gaMath::HALF *
						gaMath::Sqrt(m[0][0] - m[1][1] - m[2][2] + gaMath::ID);
					fHalfInverse = gaMath::HALF / axis.x;
					axis.y = fHalfInverse * m[0][1];
					axis.z = fHalfInverse * m[0][2];
				}
				else
				{
					// r22 is maximum diagonal term
					axis.z = gaMath::HALF *
						gaMath::Sqrt(m[2][2] - m[0][0] - m[1][1] + gaMath::ID);
					fHalfInverse = gaMath::HALF / axis.z;
					axis.x = fHalfInverse * m[0][2];
					axis.y = fHalfInverse * m[1][2];
				}
			}
			else
			{
				// r11 > r00
				if (m[1][1] >= m[2][2])
				{
					// r11 is maximum diagonal term
					axis.y = gaMath::HALF *
						gaMath::Sqrt(m[1][1] - m[0][0] - m[2][2] + gaMath::ID);
					fHalfInverse = gaMath::HALF / axis.y;
					axis.x = fHalfInverse * m[0][1];
					axis.z = fHalfInverse * m[1][2];
				}
				else
				{
					// r22 is maximum diagonal term
					axis.z = gaMath::HALF *
						gaMath::Sqrt(m[2][2] - m[0][0] - m[1][1] + gaMath::ID);
					fHalfInverse = gaMath::HALF / axis.z;
					axis.x = fHalfInverse * m[0][2];
					axis.y = fHalfInverse * m[1][2];
				}
			}
		}
	}
	else
	{
		// The angle is 0 and the matrix is the identity.  Any axis will
		// work, so just use the x-axis.
		axis.x = 1.0f;
		axis.y = 0.0f;
		axis.z = 0.0f;
	}
}

void gaM3::FromAxisAngle(const gaV3& axis, float radian)
{
	float fCos = (float)cos(radian);
	float fSin = (float)sin(radian);
	float fOneMinusCos = gaMath::ID - fCos;
	float fX2 = axis.x * axis.x;
	float fY2 = axis.y * axis.y;
	float fZ2 = axis.z * axis.z;
	float fXYM = axis.x * axis.y * fOneMinusCos;
	float fXZM = axis.x * axis.z * fOneMinusCos;
	float fYZM = axis.y * axis.z * fOneMinusCos;
	float fXSin = axis.x * fSin;
	float fYSin = axis.y * fSin;
	float fZSin = axis.z * fSin;

	m[0][0] = fX2 * fOneMinusCos + fCos;
	m[0][1] = fXYM - fZSin;
	m[0][2] = fXZM + fYSin;
	m[1][0] = fXYM + fZSin;
	m[1][1] = fY2 * fOneMinusCos + fCos;
	m[1][2] = fYZM - fXSin;
	m[2][0] = fXZM - fYSin;
	m[2][1] = fYZM + fXSin;
	m[2][2] = fZ2 * fOneMinusCos + fCos;
}

bool gaM3::ToEulerAnglesXYZ(float& angleX, float& angleY, float& angleZ) const
{
	// rot =  cy*cz          -cy*sz           sy
	//        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
	//       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

	if (m[0][2] < gaMath::ID)
	{
		if (m[0][2] > -1.0f)
		{
			angleX = gaMath::ATan2(-m[1][2], m[2][2]);
			angleY = (float)asin(m[0][2]);
			angleZ = gaMath::ATan2(-m[0][1], m[0][0]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
			angleX = -gaMath::ATan2(m[1][0], m[1][1]);
			angleY = -gaMath::HALF_PI;
			angleZ = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
		angleX = gaMath::ATan2(m[1][0], m[1][1]);
		angleY = gaMath::HALF_PI;
		angleZ = 0.0f;
		return false;
	}
}

bool gaM3::ToEulerAnglesXZY(float& angleX, float& angleZ, float& angleY) const
{
	// rot =  cy*cz          -sz              cz*sy
	//        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
	//       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

	if (m[0][1] < gaMath::ID)
	{
		if (m[0][1] > -1.0f)
		{
			angleX = gaMath::ATan2(m[2][1], m[1][1]);
			angleZ = (float)asin(-m[0][1]);
			angleY = gaMath::ATan2(m[0][2], m[0][0]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  XA - YA = atan2(r20,r22)
			angleX = gaMath::ATan2(m[2][0], m[2][2]);
			angleZ = gaMath::HALF_PI;
			angleY = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
		angleX = gaMath::ATan2(-m[2][0], m[2][2]);
		angleZ = -gaMath::HALF_PI;
		angleY = 0.0f;
		return false;
	}
}

bool gaM3::ToEulerAnglesYXZ(float& angleY, float& angleX, float& angleZ) const
{
	// rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
	//        cx*sz           cx*cz          -sx
	//       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

	if (m[1][2] < gaMath::ID)
	{
		if (m[1][2] > -1.0f)
		{
			angleY = gaMath::ATan2(m[0][2], m[2][2]);
			angleX = (float)asin(-m[1][2]);
			angleZ = gaMath::ATan2(m[1][0], m[1][1]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			angleY = gaMath::ATan2(m[0][1], m[0][0]);
			angleX = gaMath::HALF_PI;
			angleZ = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		angleY = gaMath::ATan2(-m[0][1], m[0][0]);
		angleX = -gaMath::HALF_PI;
		angleZ = 0.0f;
		return false;
	}
}

bool gaM3::ToEulerAnglesYZX(float& angleY, float& angleZ, float& angleX) const
{
	// rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
	//        sz              cx*cz          -cz*sx
	//       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

	if (m[1][0] < gaMath::ID)
	{
		if (m[1][0] > -1.0f)
		{
			angleY = gaMath::ATan2(-m[2][0], m[0][0]);
			angleZ = (float)asin(m[1][0]);
			angleX = gaMath::ATan2(-m[1][2], m[1][1]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
			angleY = -gaMath::ATan2(m[2][1], m[2][2]);
			angleZ = -gaMath::HALF_PI;
			angleX = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + XA = atan2(r21,r22)
		angleY = gaMath::ATan2(m[2][1], m[2][2]);
		angleZ = gaMath::HALF_PI;
		angleX = 0.0f;
		return false;
	}
}

bool gaM3::ToEulerAnglesZXY(float& angleZ, float& angleX, float& angleY) const
{
	// rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
	//        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
	//       -cx*sy           sx              cx*cy

	if (m[2][1] < gaMath::ID)
	{
		if (m[2][1] > -1.0f)
		{
			angleZ = gaMath::ATan2(-m[0][1], m[1][1]);
			angleX = (float)asin(m[2][1]);
			angleY = gaMath::ATan2(-m[2][0], m[2][2]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
			angleZ = -gaMath::ATan2(m[0][2], m[0][0]);
			angleX = -gaMath::HALF_PI;
			angleY = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
		angleZ = gaMath::ATan2(m[0][2], m[0][0]);
		angleX = gaMath::HALF_PI;
		angleY = 0.0f;
		return false;
	}
}

bool gaM3::ToEulerAnglesZYX(float& angleZ, float& angleY, float& angleX) const
{
	// rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
	//        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
	//       -sy              cy*sx           cx*cy

	if (m[2][0] < gaMath::ID)
	{
		if (m[2][0] > -1.0f)
		{
			angleZ = gaMath::ATan2(m[1][0], m[0][0]);
			angleY = (float)asin(-m[2][0]);
			angleX = gaMath::ATan2(m[2][1], m[2][2]);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
			angleZ = -gaMath::ATan2(m[0][1], m[0][2]);
			angleY = gaMath::HALF_PI;
			angleX = 0.0f;
			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
		angleZ = gaMath::ATan2(-m[0][1], -m[0][2]);
		angleY = -gaMath::HALF_PI;
		angleX = 0.0f;
		return false;
	}
}

void gaM3::FromEulerAnglesXYZ(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	*this = matX * (matY * matZ);
}

void gaM3::FromEulerAnglesXZY(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	*this = matX * (matZ * matY);
}

void gaM3::FromEulerAnglesYXZ(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	*this = matY * (matX * matZ);
}

void gaM3::FromEulerAnglesYZX(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	*this = matY * (matZ * matX);
}

void gaM3::FromEulerAnglesZXY(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	*this = matZ * (matX * matY);
}

void gaM3::FromEulerAnglesZYX(float yaw, float pitch, float roll)
{
	float fCos, fSin;

	fCos = gaMath::Cos(yaw);
	fSin = gaMath::Sin(yaw);
	gaM3 matZ(fCos, -fSin, 0.0f, fSin, fCos, 0.0f, 0.0f, 0.0f, 1.0f);

	fCos = gaMath::Cos(pitch);
	fSin = gaMath::Sin(pitch);
	gaM3 matY(fCos, 0.0f, fSin, 0.0f, 1.0f, 0.0f, -fSin, 0.0f, fCos);

	fCos = gaMath::Cos(roll);
	fSin = gaMath::Sin(roll);
	gaM3 matX(1.0f, 0.0f, 0.0f, 0.0f, fCos, -fSin, 0.0f, fSin, fCos);

	*this = matZ * (matY * matX);
}

const gaT3 gaT3::ID(gaM3::ID, gaV3::ZERO, gaV3::ONE);

const gaQ gaQ::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const gaQ gaQ::ID(0.0f, 0.0f, 0.0f, 1.0f);
