#pragma once

class cToken
{
public:
	cToken();
	virtual ~cToken();
	virtual const char* GetBuf() const = 0;
	int GetPos() const { return m_Pos; }
	int GetLen() const { return m_Len; }
	void MovePos(int pos);
	bool IsOutOfToken() const;
	bool GetToken(char* const out, const char* separators, int length);
	bool GetToken(char** out, const char* separators, int length)
	{
		if (!GetResultBuf())
		{
			if (out)
				*out = 0;
			return false;
		}
		if (out)
			*out = m_pcResBuf;
		return GetToken(m_pcResBuf, separators, length);
	}
	bool GetTokenWithoutFail(char* const out, const char* separators,
		int length);
	bool GetTokenWithoutFail(char** out, const char* separators, int length);
	bool GetTokenFullSep(char* const out, const char* separators, int length);
	bool GetTokenFullSep(char** out, const char* separators, int length)
	{
		if (!GetResultBuf())
		{
			if (out)
				*out = 0;
			return false;
		}
		if (out)
			*out = m_pcResBuf;
		return GetTokenFullSep(m_pcResBuf, separators, length);
	}
	int GetTokenNum(int pos, char separator) const;
	int GetTokenNum(int pos, const char* separators, int length) const;
	char GetChar();
	int GetChange(char* const out);

protected:
	bool GetResultBuf();
	int m_Pos;
	int m_Len;
	int m_ResLen;
	char* m_pcResBuf;
};

class cTokenV : public cToken
{
public:
	cTokenV();
	virtual ~cTokenV();
	virtual const char* GetBuf() const;
	bool Init(const char* buffer, int length);

protected:
	const char* m_pcBuf;
};
