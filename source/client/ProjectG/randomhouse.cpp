// Based on MT19937 integer version.
// Original disclaimer included below.
// --
// A C-program for MT19937: Integer version (1998/4/6)
//  genrand() generates one pseudorandom unsigned integer (32bit)
// which is uniformly distributed among 0 to 2^32-1  for each
// call. sgenrand(seed) set initial values to the working area
// of 624 words. Before genrand(), sgenrand(seed) must be
// called once. (seed is any 32-bit integer except for 0).
//   Coded by Takuji Nishimura, considering the suggestions by
// Topher Cooper and Marc Rieffel in July-Aug. 1997.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// You should have received a copy of the GNU Library General
// Public License along with this library; if not, write to the
// Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
// 02111-1307  USA
//
// Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.
// When you use this, send an email to: matumoto@math.keio.ac.jp
// with an appropriate reference to your work.
//
// REFERENCE
// M. Matsumoto and T. Nishimura,
// "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform
// Pseudo-Random Number Generator",
// ACM Transactions on Modeling and Computer Simulation,
// Vol. 8, No. 1, January 1998, pp 3--30.
//
// See
//     http://www.math.keio.ac.jp/~matumoto/emt.html
// and
//     http://www.acm.org/pubs/citations/journals/tomacs/1998-8-1/p3-matsumoto/

#include "randomhouse.h"

#define N 624
#define M 397
#define MATRIX_A 0x9908b0df
#define UPPER_MASK 0x80000000
#define LOWER_MASK 0x7fffffff

#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y) (y >> 11)
#define TEMPERING_SHIFT_S(y) (y << 7)
#define TEMPERING_SHIFT_T(y) (y << 15)
#define TEMPERING_SHIFT_L(y) (y >> 18)

CRandomHouse::CRandomHouse()
	: m_mt(NULL), m_mti(N + 1), m_seed(4357)
{
	m_mt = new unsigned long[N];
}

CRandomHouse::~CRandomHouse()
{
	if (m_mt)
	{
		delete[] (m_mt);
		(m_mt) = NULL;
	}
}

void CRandomHouse::SetRandomSeed(unsigned long seed)
{
	if (seed == 0)
		seed = m_seed;
	else
		m_seed = seed;

	int i;

	for (i = 0; i < N; i++)
	{
		m_mt[i] = seed & 0xffff0000;
		seed = 69069 * seed + 1;
		m_mt[i] |= (seed & 0xffff0000) >> 16;
		seed = 69069 * seed + 1;
	}
	m_mti = N;
}

unsigned long CRandomHouse::GetRandom()
{
	unsigned long y;
	static unsigned long mag01[2] = { 0x0, MATRIX_A };

	if (m_mti >= N)
	{
		int kk;

		if (m_mti == N + 1)
			SetRandomSeed(0);

		for (kk = 0; kk < N - M; kk++)
		{
			y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
			m_mt[kk] = m_mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (; kk < N - 1; kk++)
		{
			y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
			m_mt[kk] = m_mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (m_mt[N - 1] & UPPER_MASK) | (m_mt[0] & LOWER_MASK);
		m_mt[N - 1] = m_mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1];

		m_mti = 0;
	}

	y = m_mt[m_mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);

	return y;
}
