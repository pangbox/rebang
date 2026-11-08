#include "binstr.h"
#include "../../shared/localize.h"

cToken::cToken()
{
	m_Len = m_Pos = m_ResLen = 0;
	m_pcResBuf = NULL;
}

cToken::~cToken()
{
	if (m_pcResBuf)
		delete[] m_pcResBuf;
}

bool cToken::GetResultBuf()
{
	if (m_Len + 1 > m_ResLen)
	{
		if (m_pcResBuf)
			delete[] m_pcResBuf;
		m_pcResBuf = new char[m_Len + 1];
		if (m_pcResBuf == NULL)
		{
			m_ResLen = 0;
			return false;
		}
		m_ResLen = m_Len + 1;
	}
	return true;
}

bool cToken::GetToken(char* const pOut, const char* pcSep, int nSep)
{
	*pOut = 0;
	if (!GetBuf())
		return false;

	int nPos = m_Pos;
	bool bFind = false;
	while (nPos < m_Len)
	{
		int j = 0;
		for (int i = 0; i < nSep; ++i, ++j)
		{
			char c = pcSep[j];
			if (GetBuf()[nPos] == c)
			{
				if (!Custom_IsLeadByte(c))
					goto FOUND;
				++j;
				bool bMatch;
				if (IsLocalCountry(30))
					bMatch = nPos + 1 < m_Len && GetBuf()[nPos + 1] == pcSep[j];
				else
					bMatch = GetBuf()[nPos + 1] == pcSep[j];
				if (bMatch)
					goto FOUND2;
			}
			else if (Custom_IsLeadByte(c))
				++j;
		}
		pOut[nPos - m_Pos] = GetBuf()[nPos];
		if (Custom_IsLeadByte(GetBuf()[nPos]))
		{
			if (IsLocalCountry(30))
			{
				if (nPos + 1 < m_Len)
				{
					++nPos;
					pOut[nPos - m_Pos] = GetBuf()[nPos];
				}
			}
			else if (++nPos < m_Len)
				pOut[nPos - m_Pos] = GetBuf()[nPos];
		}
		++nPos;
	}
	goto END;
FOUND:
	bFind = true;
END:
	pOut[nPos - m_Pos] = 0;
	goto RET;
FOUND2:
	++nPos;
	bFind = true;
	pOut[nPos - m_Pos - 1] = 0;
RET:
	m_Pos = nPos < m_Len ? nPos + 1 : m_Len;
	return bFind;
}

bool cToken::GetTokenFullSep(char* const pOut, const char* pcSep, int nSep)
{
	*pOut = 0;
	if (!GetBuf())
		return false;

	int nPos = m_Pos;
	int nFind = PosB(GetBuf() + nPos, m_Len - nPos, pcSep, nSep);
	if (nFind < 0)
	{
		nFind = m_Len - nPos;
		memcpy(pOut, GetBuf() + m_Pos, nFind);
		pOut[nFind] = 0;
		m_Pos = m_Len;
		return false;
	}
	memcpy(pOut, GetBuf() + m_Pos, nFind);
	pOut[nFind] = 0;
	m_Pos += nFind + nSep;
	return true;
}

int cToken::GetTokenNum(int nPos, char cSep) const
{
	return GetTokenNum(nPos, &cSep, 1);
}

int cToken::GetTokenNum(int nPos, const char* pcSep, int nSep) const
{
	if (!GetBuf())
		return 0;

	int nCount = nPos < m_Len;
	for (; nPos < m_Len; ++nPos)
	{
		for (int i = 0; i < nSep; ++i)
		{
			if (GetBuf()[nPos] == pcSep[i])
			{
				++nCount;
				break;
			}
		}
		if (Custom_IsLeadByte(GetBuf()[nPos]))
			++nPos;
	}
	return nCount;
}

char cToken::GetChar()
{
	return (GetBuf() && m_Pos < m_Len) ? GetBuf()[m_Pos++] : 0;
}

int cToken::GetChange(char* const pOut)
{
	if (GetBuf() && m_Pos < m_Len)
	{
		memcpy(pOut, GetBuf() + m_Pos, m_Len - m_Pos);
		return m_Len - m_Pos;
	}
	pOut[0] = 0;
	return 0;
}

cTokenV::cTokenV()
{
	m_pcBuf = NULL;
}

cTokenV::~cTokenV()
{
}

bool cTokenV::Init(const char* pcBuf, int nLen)
{
	if (pcBuf == NULL || nLen < 1)
		return false;
	m_pcBuf = pcBuf;
	m_Len = nLen;
	m_Pos = 0;
	return true;
}

cTokenS::cTokenS()
{
	m_pcBuf = NULL;
}

cTokenS::~cTokenS()
{
	if (m_pcBuf)
		delete[] m_pcBuf;
}

bool cTokenS::Init(const char* pcBuf, int nLen)
{
	if (pcBuf == NULL || nLen < 1)
		return false;

	if (nLen > m_Len)
	{
		if (m_pcBuf)
			delete[] m_pcBuf;
		m_pcBuf = new char[nLen];
		if (m_pcBuf == NULL)
		{
			m_Len = 0;
			return false;
		}
	}
	memcpy(m_pcBuf, pcBuf, nLen);
	m_Len = nLen;
	m_Pos = 0;
	return true;
}

int StrToIntDefA(const char* str, int def)
{
	if (str == NULL || *str == 0)
		return def;

	int i = strlen(str) - 1;
	int result = 0;
	int mul = 1;
	for (; i > 0; --i)
	{
		char c = str[i];
		if (c < '0' || c > '9')
			return def;
		result += (c - '0') * mul;
		mul *= 10;
	}
	if (str[0] == '-')
		result = -result;
	else
	{
		if (str[0] < '0' || str[0] > '9')
			return def;
		result += (str[0] - '0') * mul;
	}
	return result;
}

int StrToIntDefFilterA(const char* str, int def)
{
	if (str == NULL)
		return def;

	int last = strlen(str) - 1;
	int start;
	for (start = 0; start <= last; ++start)
	{
		if (str[start] >= '0' && str[start] <= '9')
			break;
	}
	if (start > last)
		return def;

	int i;
	for (i = start + 1; i <= last; ++i)
	{
		if (str[i] < '0' || str[i] > '9')
			break;
	}
	--i;

	int result = 0;
	int mul = 1;
	for (; i > start; --i)
	{
		char c = str[i];
		if (c < '0' || c > '9')
			return def;
		result += (c - '0') * mul;
		mul *= 10;
	}
	if (str[start] == '-')
		result = -result;
	else
	{
		if (str[start] < '0' || str[start] > '9')
			return def;
		result += (str[start] - '0') * mul;
	}
	return result;
}

char* IntToStrA(int value, char* str)
{
	int i;
	int len;
	int div;
	if (value < 0)
	{
		str[0] = '-';
		i = 1;
		len = 2;
		div = -1;
	}
	else
	{
		i = 0;
		len = 1;
		div = 1;
	}
	for (; value / div > 9; ++len)
		div *= 10;
	str[len] = 0;
	for (; i < len; ++i)
	{
		str[i] = value / div + '0';
		value %= div;
		div /= 10;
	}
	return str;
}

char* TrimA(char* str, const char* trim)
{
	int start = 0;
	int end = strlen(str) - 1;
	while (start <= end && strrchr(trim, str[start]))
	{
		if (Custom_IsLeadByte(str[start]))
			++start;
		++start;
	}
	if (start <= end)
	{
		while (strrchr(trim, str[end]))
			--end;
		int len = end - start;
		memmove(str, str + start, len + 1);
		str[len + 1] = 0;
	}
	else
		str[0] = 0;
	return str;
}

char* ExchangeA(char* str, const char* from, const char* to)
{
	cTokenS token;
	char* buf = new char[strlen(str) + 1];
	buf[0] = 0;
	token.Init(str, strlen(str));
	str[0] = 0;
	bool bRet = token.GetTokenFullSep(buf, from, strlen(from));
	lstrcat(str, buf);
	while (bRet)
	{
		lstrcat(str, to);
		bRet = token.GetTokenFullSep(buf, from, strlen(from));
		lstrcat(str, buf);
	}
	delete[] buf;
	return str;
}

char* RemoveA(char* str, const char* remove)
{
	cTokenS token;
	char* buf = new char[strlen(str) + 1];
	buf[0] = 0;
	token.Init(str, strlen(str));
	str[0] = 0;
	bool bRet;
	do
	{
		bRet = token.GetTokenFullSep(buf, remove, strlen(remove));
		lstrcat(str, buf);
	} while (bRet);
	delete[] buf;
	return str;
}

char* MakePriceStrA(char* str, __int64 price)
{
	sprintf(str, "%I64d", price);
	int len = strlen(str);
	int commas = len - 1 > 0 ? (len - 1) / 3 : 0;
	int newLen = len + commas;
	str[newLen] = 0;
	str[newLen - 1] = str[len - 1];
	for (int i = len - 2, j = newLen - 2; i >= 0 && j >= 0; --i, --j)
	{
		if ((len - i - 1) % 3 == 0)
			str[j--] = ',';
		str[j] = str[i];
	}
	return str;
}

char* UpperCase(const char* src, char* dest)
{
	int len = strlen(src);
	for (int i = 0; i < len; ++i)
	{
		char c = src[i];
		if (Custom_IsLeadByte(c))
			++i;
		else
			dest[i] = (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
	}
	dest[len] = 0;
	return dest;
}

char* LowerCase(const char* src, char* dest)
{
	int len = strlen(src);
	for (int i = 0; i < len; ++i)
	{
		char c = src[i];
		if (Custom_IsLeadByte(c))
			++i;
		else
			dest[i] = (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	}
	dest[len] = 0;
	return dest;
}

void MakeShortFileName(const char* path, char* name)
{
	cTokenV token;
	token.Init(path, strlen(path));
	int num = token.GetTokenNum(0, "/\\", 2);
	for (int i = 0; i < num; ++i)
		token.GetToken(name, "/\\", 2);
}

void ExtractPath(const char* path, char* dir)
{
	cTokenV token;
	token.Init(path, strlen(path));
	dir[0] = 0;
	int num = token.GetTokenNum(0, "/\\", 2) - 1;
	for (int i = 0; i < num; ++i)
	{
		char* p;
		token.GetToken(&p, "/\\", 2);
		lstrcat(dir, p);
		lstrcat(dir, "\\");
	}
}

int __cdecl mbscut(char* dest, const char* src, unsigned int len)
{
	if (len == 0)
	{
		dest[0] = 0;
		return 0;
	}
	if (strlen(src) > len)
	{
		unsigned int last = len - 1;
		if (Custom_IsLeadByte(src[last]))
		{
			unsigned int i = 0;
			for (; i < last; ++i)
			{
				if (Custom_IsLeadByte(src[i]))
					++i;
			}
			len = i;
		}
	}
	strncpy(dest, src, len);
	return strlen(dest);
}
