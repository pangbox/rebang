#pragma once

class cToken
{
public:
	cToken();
	virtual ~cToken();
	virtual const char* GetBuf() const = 0;
	int GetPos() const;
	int GetLen() const;
	void MovePos(int pos);
	bool IsOutOfToken() const;
	bool GetToken(char* const pOut, const char* pcSep, int nSep);
	bool GetToken(char** ppOut, const char* pcSep, int nSep);
	bool GetTokenWithoutFail(char* const pOut, const char* pcSep, int nSep);
	bool GetTokenWithoutFail(char** ppOut, const char* pcSep, int nSep);
	bool GetTokenFullSep(char* const pOut, const char* pcSep, int nSep);
	bool GetTokenFullSep(char** ppOut, const char* pcSep, int nSep);
	int GetTokenNum(int nPos, char cSep) const;
	int GetTokenNum(int nPos, const char* pcSep, int nSep) const;
	char GetChar();
	int GetChange(char* const pOut);

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
	bool Init(const char* pcBuf, int nLen);

protected:
	const char* m_pcBuf;
};

class cTokenS : public cToken
{
public:
	cTokenS();
	virtual ~cTokenS();
	virtual const char* GetBuf() const;
	bool Init(const char* pcBuf, int nLen);

protected:
	char* m_pcBuf;
};

int StrToIntDefA(const char* str, int def);
int StrToIntDefFilterA(const char* str, int def);
char* IntToStrA(int value, char* str);
char* MakePriceStrA(char* str, __int64 price);
char* TrimA(char* str, const char* trim);
char* ExchangeA(char* str, const char* from, const char* to);
char* RemoveA(char* str, const char* remove);
char* UpperCase(const char* src, char* dest);
char* LowerCase(const char* src, char* dest);
void MakeShortFileName(const char* path, char* name);
void ExtractPath(const char* path, char* dir);
int __cdecl mbscut(char* dest, const char* src, unsigned int len);

#include "binstr.inl"
