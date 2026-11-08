#include "jrencrypt.h"

void SimpleStreamEncrypt_Alpha(const char* src, char* tar, unsigned int len,
	unsigned int key)
{
	bool same_pointer = false;
	if (src == tar)
	{
		tar = new char[len];
		same_pointer = true;
	}

	unsigned int fxor;
	unsigned int upos;

	unsigned int udword = len >> 2;
	unsigned int uextra = len & 3;

	if (udword > 0)
	{
		((unsigned int*)tar)[0] = ((const unsigned int*)src)[0] ^ key;
		for (unsigned int i = 1; i < udword; i++)
			((unsigned int*)tar)[i] = ((const unsigned int*)src)[i - 1] ^
				((const unsigned int*)src)[i];
		fxor = ((const unsigned int*)src)[udword - 1];
		upos = udword * 4;
	}
	else
	{
		fxor = key;
		upos = 0;
	}

	for (unsigned int j = 0; j < uextra; j++)
		tar[upos + j] = src[upos + j] ^ (char)(fxor >> (j * 8));

	memcpy((char*)src, tar, len);

	if (same_pointer)
	{
		delete[] tar;
	}
}

void SimpleStreamDecrypt_Alpha(const char* src, char* tar, unsigned int len,
	unsigned int key)
{
	bool same_pointer = false;
	if (src == tar)
	{
		tar = new char[len];
		same_pointer = true;
	}

	unsigned int fxor;
	unsigned int upos;

	unsigned int udword = len >> 2;
	unsigned int uextra = len & 3;

	if (udword > 0)
	{
		((unsigned int*)tar)[0] = ((const unsigned int*)src)[0] ^ key;
		for (unsigned int i = 1; i < udword; i++)
			((unsigned int*)tar)[i] =
				((const unsigned int*)src)[i] ^ ((unsigned int*)tar)[i - 1];
		fxor = ((unsigned int*)tar)[udword - 1];
		upos = udword * 4;
	}
	else
	{
		fxor = key;
		upos = 0;
	}

	for (unsigned int j = 0; j < uextra; j++)
		tar[upos + j] = src[upos + j] ^ (char)(fxor >> (j * 8));

	memcpy((char*)src, tar, len);

	if (same_pointer)
	{
		delete[] tar;
	}
}

void SimpleStreamEncrypt_Delta(const char* src, char* tar, unsigned int len,
	unsigned int key)
{
	unsigned int fxor;
	unsigned int upos;

	unsigned int udword = len >> 2;
	unsigned int uextra = len & 3;

	if (udword > 0)
	{
		((unsigned int*)tar)[0] =
			((const unsigned int*)src)[0] ^ key ^ 0x9C6C95CE;
		for (unsigned int i = 1; i < udword; i++)
			((unsigned int*)tar)[i] = ((const unsigned int*)src)[i - 1] ^
				(i * i * (i + 966) * (i + 2020)) ^
				((const unsigned int*)src)[i] ^ 0x9C6C95CE;
		fxor = ((const unsigned int*)src)[udword - 1];
		upos = udword * 4;
	}
	else
	{
		fxor = key;
		upos = 0;
	}

	for (unsigned int j = 0; j < uextra; j++)
		tar[upos + j] = src[upos + j] ^ (char)(fxor >> (j * 8));
}

void SimpleStreamDecrypt_Delta(const char* src, char* tar, unsigned int len,
	unsigned int key)
{
	unsigned int fxor;
	unsigned int upos;

	unsigned int udword = len >> 2;
	unsigned int uextra = len & 3;

	if (udword > 0)
	{
		((unsigned int*)tar)[0] =
			((const unsigned int*)src)[0] ^ key ^ 0x9C6C95CE;
		for (unsigned int i = 1; i < udword; i++)
			((unsigned int*)tar)[i] = ((const unsigned int*)src)[i] ^
				(i * i * (i + 966) * (i + 2020)) ^ ((unsigned int*)tar)[i - 1] ^
				0x9C6C95CE;
		fxor = ((unsigned int*)tar)[udword - 1];
		upos = udword * 4;
	}
	else
	{
		fxor = key;
		upos = 0;
	}

	for (unsigned int j = 0; j < uextra; j++)
		tar[upos + j] = src[upos + j] ^ (char)(fxor >> (j * 8));
}
