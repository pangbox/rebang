#pragma once
#include <list>
#include <string>
#include <string.h>

class FrGraphicInterface;

enum eFrPRINTOPT
{
	FrTEXT_PRINT_EXCLUDE_TAG,
	FrTEXT_PRINT_ALL
};

enum eFrTOKENTYPE
{
	FrTEXT_TAG_STRING,
	FrTEXT_TAG_STYLE_THICK,
	FrTEXT_TAG_STYLE_NONE,
	FrTEXT_TAG_STYLE_OUTLINE,
	FrTEXT_TAG_STYLE_SHADOW,
	FrTEXT_TAG_COLOR1,
	FrTEXT_TAG_COLOR2
};

class FrTextLine;

class FrTextToken
{
public:
	FrTextToken();
	FrTextToken(const char* text) { ComposeData(std::string(text)); }
	FrTextToken(const std::string& text) { ComposeData(text); }
	~FrTextToken() { }

	void RunTag(FrGraphicInterface* gdi);

	void Text(std::string text);
	std::string Text() const { return m_text; }
	bool IsTag() const;
	eFrTOKENTYPE Type() const { return m_type; }
	bool GetColorValue(unsigned long* color, unsigned long* outline) const;
	bool GetColorValue(unsigned long* color) const;

private:
	void ComposeData(const std::string& text);
	bool IsStyleTag(const std::string& text);
	bool SetStyleTagType(const char type);
	bool IsColorTag(const std::string& text);
	bool SetColorTagType(const std::string& text);

	std::string m_text;
	eFrTOKENTYPE m_type;
};

class FrTEXT
{
public:
	FrTEXT()
		: m_option(FrTEXT_PRINT_ALL)
	{
	}
	FrTEXT(const std::string& text);
	FrTEXT(const char* text);
	~FrTEXT();

	void operator=(const std::string& text) { ComposeData(text); }
	void operator=(const char* text);
	void operator<<(std::string& text) const;

	void SetPrintOption(eFrPRINTOPT option);
	eFrPRINTOPT GetPrintOption() const;
	unsigned int GetLineSize() const { return m_ltLine.size(); }
	void GetTokenList(std::list<FrTextToken>& list) const;
	float GetWidthLong(FrGraphicInterface* gdi) const;
	void Clear();

	typedef std::list<FrTextLine*> LTFRTXLINE;
	typedef LTFRTXLINE::iterator LTFRTXLINE_I;
	typedef LTFRTXLINE::const_iterator LTFRTXLINE_CI;

private:
	eFrPRINTOPT m_option;
	LTFRTXLINE m_ltLine;
	std::string m_srcText;

	void ComposeData(const std::string& text);
	std::string GetPrintData() const;
};

inline unsigned int StringExtractFindFirst(const std::string& src,
	const char* szBegin, const char* szEnd, std::string& out)
{
	if (szBegin == NULL || szEnd == NULL)
		return std::string::npos;

	unsigned int begin = src.find(szBegin, 0);
	if (begin == std::string::npos)
		return std::string::npos;

	unsigned int end = src.find(szEnd, begin + 1);
	if (end == std::string::npos)
		return std::string::npos;

	unsigned int len = strlen(szBegin);
	out = std::string(src.begin() + begin + len, src.begin() + end);
	return begin;
}

inline void StringTrim(std::string& str, const char* chars)
{
	if (chars == NULL)
		return;

	std::string result;
	unsigned int size = str.length();
	unsigned int count = strlen(chars);
	for (unsigned int i = 0; i < size; i++)
	{
		bool found = false;
		unsigned int j;
		for (j = 0; j < count; j++)
		{
			if (chars[j] == str[i])
			{
				found = true;
				break;
			}
		}
		if (!found)
			result += str[i];
	}

	str = result;
}
