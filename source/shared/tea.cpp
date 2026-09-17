typedef unsigned int U32;

static const U32 DELTA = 0x9e3779b9;

void Decipher(const U32* const v, U32* const w, const U32* const k)
{
	w[0] = v[0];
	w[1] = v[1];
	U32 sum = DELTA * 16;
	for (int round = 0; round < 16; ++round)
	{
		w[1] -=
			(((w[0] << 4) ^ (w[0] >> 5)) + w[0]) ^ (sum + k[(sum >> 11) & 3]);
		sum -= DELTA;
		w[0] -= (((w[1] << 4) ^ (w[1] >> 5)) + w[1]) ^ (sum + k[sum & 3]);
	}
}

int DecodeTEA_NBYTE(char* szDest, int nSize, const char* szSrc,
	const U32* paKey)
{
	U32 v[2] = { 0, 0 };
	U32 k[4];
	k[0] = paKey[0];
	k[1] = paKey[1];
	k[2] = paKey[2];
	k[3] = paKey[3];

	if (nSize > 0)
	{
		for (int offset = 0; nSize > offset; offset += 8)
		{
			v[0] = *(const U32*)(szSrc + offset);
			v[1] = *(const U32*)(szSrc + offset + 4);
			Decipher(v, (U32*)(szDest + offset), k);
		}
	}
	return 8;
}
