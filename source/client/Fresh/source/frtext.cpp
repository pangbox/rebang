#include "frtext.h"
#include "frgraphicinterface.h"
#include <stdlib.h>

enum
{
	MAX_FRTAG_CHAR_SIZE = 1
};

static const char g_FrTagChar[MAX_FRTAG_CHAR_SIZE] = { 'c' };

unsigned int StringExtractTagTextFindFirst(const std::string& src,
	std::string& output)
{
	std::string szToken;
	unsigned int first = src.length();
	for (int i = 0; i < MAX_FRTAG_CHAR_SIZE; i++)
	{
		std::string szTagch = "\\";
		szTagch += g_FrTagChar[i];
		unsigned int pos = StringExtractFindFirst(src, szTagch.c_str(),
			szTagch.c_str(), szToken);
		if (pos != std::string::npos && pos < first)
		{
			first = pos;
			output = szTagch + szToken + szTagch;
		}
	}
	if (first == src.length())
		return std::string::npos;
	return first;
}

void FrTextToken::RunTag(FrGraphicInterface* pGDI)
{
	switch (m_type)
	{
	case FrTEXT_TAG_STYLE_THICK:
		pGDI->SetTextStyle(FrGDI::THICK);
		break;
	case FrTEXT_TAG_STYLE_NONE:
		pGDI->SetTextStyle(FrGDI::NONE);
		break;
	case FrTEXT_TAG_STYLE_OUTLINE:
		pGDI->SetTextStyle(FrGDI::OUTLINE);
		break;
	case FrTEXT_TAG_STYLE_SHADOW:
		pGDI->SetTextStyle(FrGDI::SHADOW);
		break;
	case FrTEXT_TAG_COLOR1:
	{
		unsigned long color;
		if (GetColorValue(&color))
			pGDI->SetTextColor(color, 0xffffffff);
	}
	break;
	case FrTEXT_TAG_COLOR2:
	{
		unsigned long clr1, clr2;
		if (GetColorValue(&clr1, &clr2))
			pGDI->SetTextColor(clr1, clr2);
	}
	break;
	}
}

bool FrTextToken::IsTag() const
{
	return m_type != FrTEXT_TAG_STRING;
}

bool FrTextToken::GetColorValue(unsigned long* textcolor,
	unsigned long* outcolor) const
{
	if (m_type != FrTEXT_TAG_COLOR2)
		return false;
	std::string trimSet = " \\";
	trimSet += g_FrTagChar[0];
	std::string copy = m_text;
	StringTrim(copy, trimSet.c_str());
	unsigned int pos = copy.find(",");
	if (copy.length() == 0 || pos == std::string::npos)
		return false;
	std::string szColor1 = copy.substr(0, pos);
	std::string szColor2 = copy.substr(pos + 1, copy.length() - pos);
	if (szColor1.length() == 0 || szColor2.length() == 0)
		return false;
	*textcolor = strtoul(szColor1.c_str(), 0, 16);
	*outcolor = strtoul(szColor2.c_str(), 0, 16);
	return true;
}

bool FrTextToken::GetColorValue(unsigned long* color) const
{
	if (m_type != FrTEXT_TAG_COLOR1)
		return false;
	std::string trimSet = " \\";
	trimSet += g_FrTagChar[0];
	std::string copy = m_text;
	StringTrim(copy, trimSet.c_str());
	if (copy.length() == 0)
		return false;
	*color = strtoul(copy.c_str(), 0, 16);
	return true;
}

void FrTextToken::ComposeData(const std::string& str)
{
	m_text = str;
	m_type = FrTEXT_TAG_STRING;
	if (m_text.length() < 2)
		return;
	std::string trimSet = " \\";
	trimSet += g_FrTagChar[0];
	std::string copy = m_text;
	StringTrim(copy, trimSet.c_str());
	if (IsStyleTag(copy))
	{
		SetStyleTagType(copy[0]);
	}
	else if (IsColorTag(copy))
		SetColorTagType(copy);
}

bool FrTextToken::IsStyleTag(const std::string& szTrimedText)
{
	if (szTrimedText.length() == 1)
		if (strchr("nNtToOsS", szTrimedText[0]))
			return true;
	return false;
}

__forceinline bool FrTextToken::IsColorTag(const std::string& szTrimedText)
{
	const char* format = "0xAARRGGBB";
	if (szTrimedText.length() >= strlen(format) && szTrimedText[0] == '0' &&
		szTrimedText[1] == 'x')
		return true;
	return false;
}

bool FrTextToken::SetStyleTagType(const char ch)
{
	if (ch == 't' || ch == 'T')
		m_type = FrTEXT_TAG_STYLE_THICK;
	else if (ch == 'n' || ch == 'N')
		m_type = FrTEXT_TAG_STYLE_NONE;
	else if (ch == 'o' || ch == 'O')
		m_type = FrTEXT_TAG_STYLE_OUTLINE;
	else if (ch == 's' || ch == 'S')
		m_type = FrTEXT_TAG_STYLE_SHADOW;
	else
		return false;
	return true;
}

bool FrTextToken::SetColorTagType(const std::string& str)
{
	unsigned int len = str.length();
	const char* format = "0xAARRGGBB";
	if (len >= strlen(format) && str[0] == '0' && str[1] == 'x')
	{
		const char* format2 = "0xAARRGGBB,0xAARRGGBB";
		unsigned int pos = str.find(',');
		if (pos != std::string::npos && len == strlen(format2))
		{
			if (str[pos + 1] == '0' && str[pos + 2] == 'x')
			{
				m_type = FrTEXT_TAG_COLOR2;
				return true;
			}
		}
		else
		{
			m_type = FrTEXT_TAG_COLOR1;
			return true;
		}
	}
	return false;
}

class FrTextLine
{
	typedef std::list<FrTextToken> LTFRTOKEN;
	typedef LTFRTOKEN::iterator LTFRTOKEN_I;
	typedef LTFRTOKEN::const_iterator LTFRTOKEN_CI;

public:
	FrTextLine(const char* str) { ComposeData(std::string(str)); }
	FrTextLine(const std::string& str) { ComposeData(str); }
	~FrTextLine() { }

	std::string GetLineText(eFrPRINTOPT option) const
	{
		std::string output;

		if (option == FrTEXT_PRINT_EXCLUDE_TAG)
		{
			for (LTFRTOKEN_CI itr = m_ltFrToken.begin();
				itr != m_ltFrToken.end(); ++itr)
				if (!(*itr).IsTag())
					output += (*itr).Text();
		}
		else
			for (LTFRTOKEN_CI itr = m_ltFrToken.begin();
				itr != m_ltFrToken.end(); ++itr)
				output += (*itr).Text();
		return output;
	}

	void GetTokenList(std::list<FrTextToken>& output) const
	{
		for (LTFRTOKEN_CI itr = m_ltFrToken.begin(); itr != m_ltFrToken.end();
			++itr)
			output.push_back(*itr);
	}

	float GetTextWidth(FrGraphicInterface* pGDI) const
	{
		std::string szLine = GetLineText(FrTEXT_PRINT_EXCLUDE_TAG);
		return pGDI->GetTextExtend11(szLine.c_str());
	}

private:
	void ComposeData(const std::string& src)
	{
		std::string szToken, szText;
		std::string copy = src;
		unsigned int pos;
		while ((pos = StringExtractTagTextFindFirst(copy, szToken)) !=
			std::string::npos)
		{
			if (pos != 0)
			{
				szText._Assign(copy.begin(), copy.begin() + pos,
					std::input_iterator_tag());
				m_ltFrToken.push_back(FrTextToken(szText));
			}
			m_ltFrToken.push_back(FrTextToken(szToken));
			if (szText.length() + szToken.length() >= copy.length())
				return;
			copy = std::string(
				copy.begin() + szText.length() + szToken.length(), copy.end());
		}
		szText._Assign(copy.begin(), copy.end(), std::input_iterator_tag());
		m_ltFrToken.push_back(FrTextToken(szText));
	}

	LTFRTOKEN m_ltFrToken;
};

FrTEXT::~FrTEXT()
{
	Clear();
}

void FrTEXT::Clear()
{
	m_option = FrTEXT_PRINT_EXCLUDE_TAG;
	m_srcText = "";
	for (LTFRTXLINE_I itr = m_ltLine.begin(); itr != m_ltLine.end(); ++itr)
		delete *itr;
	m_ltLine.clear();
}

void FrTEXT::GetTokenList(std::list<FrTextToken>& output) const
{
	LTFRTXLINE_CI itr = m_ltLine.begin();
	while (itr != m_ltLine.end())
	{
		FrTextLine* pLine = *itr;
		std::list<FrTextToken> tokenList;
		pLine->GetTokenList(tokenList);
		output.insert(output.end(), tokenList.begin(), tokenList.end());
		if (++itr != m_ltLine.end())
			output.push_back(FrTextToken("\n"));
	}
}

std::string FrTEXT::GetPrintData() const
{
	if (m_option == FrTEXT_PRINT_ALL)
		return m_srcText;
	std::string output = "";
	LTFRTXLINE_CI itr = m_ltLine.begin();
	while (itr != m_ltLine.end())
	{
		FrTextLine* pLine = *itr;
		output += pLine->GetLineText(m_option);
		if (++itr != m_ltLine.end())
			output += "\n";
	}
	return output;
}

void FrTEXT::ComposeData(const std::string& src)
{
	bool same = m_srcText == src;
	if (same)
		return;
	Clear();
	m_srcText = src;
	std::string buffer = "";
	for (std::string::const_iterator srcci = src.begin(); srcci != src.end();
		++srcci)
	{
		if (*srcci == '\n' || *srcci == '\r')
		{
			if (buffer.length())
			{
				FrTextLine* pTxLine = new FrTextLine(buffer);
				m_ltLine.push_back(pTxLine);
				buffer.clear();
			}
			else
			{
				FrTextLine* pTxLine = new FrTextLine(" ");
				m_ltLine.push_back(pTxLine);
				buffer.clear();
			}
		}
		else if (srcci + 1 != src.end() && *srcci == '\\' &&
			*(srcci + 1) == 'n')
		{
			if (buffer.length())
			{
				FrTextLine* pTxLine = new FrTextLine(buffer);
				m_ltLine.push_back(pTxLine);
			}
			else
			{
				FrTextLine* pTxLine = new FrTextLine(" ");
				m_ltLine.push_back(pTxLine);
			}
			buffer.clear();
			++srcci;
		}
		else
			buffer += *srcci;
	}
	if (buffer.length())
	{
		FrTextLine* pLast = new FrTextLine(buffer);
		m_ltLine.push_back(pLast);
	}
}

float FrTEXT::GetWidthLong(FrGraphicInterface* pGDI) const
{
	float fLong = 0;
	for (LTFRTXLINE_CI itr = m_ltLine.begin(); itr != m_ltLine.end(); ++itr)
	{
		float width = (*itr)->GetTextWidth(pGDI);
		if (width > fLong)
			fLong = width;
	}
	return fLong;
}
