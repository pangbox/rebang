#include "stdafx.h"

#include "approachdesc.h"

class Mission_1 : public DescBlock
{
public:
	Mission_1(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"1\xc0\xa7 \xc0\xaf\xc0\xfa\xc0\xc7 \xba\xf1\xb0\xc5\xb8\xae\xb0\xa1 %s\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_2 : public DescBlock
{
public:
	Mission_2(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param2)
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"1~3\xc0\xa7\xc0\xc7 \xba\xf1\xb0\xc5\xb8\xae\xc0\xc7 \xc7\xd5\xb0\xe8\xb0\xa1 %s\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_3 : public DescBlock
{
public:
	Mission_3(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"OUT(\xc4\xa8\xc0\xce)\xc0\xbb \xc7\xdf\xc0\xbb \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_4 : public DescBlock
{
public:
	Mission_4(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xc0\xfc\xbf\xf8\xc0\xcc 20\xc3\xca\xb0\xa1 \xc1\xf6\xb3\xaa\xb1\xe2 \xc0\xfc\xbf\xa1 \xc5\xb8\xb1\xb8\xb8\xa6 \xbf\xcf\xb7\xe1\xc7\xdf\xc0\xbb \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_5 : public DescBlock
{
public:
	Mission_5(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xbf\xa9\xc0\xda";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xb3\xb2\xc0\xda";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"1~3\xc0\xa7\xb0\xa1 \xc0\xfc\xba\xce %s(\xc4\xb3\xb8\xaf\xc5\xcd)\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_6 : public DescBlock
{
public:
	Mission_6(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[16] = { 0 };
		switch (param1)
		{
		case 1:
		{
			const char* text = "\xb1\xee\xb8\xc1\xc1\xa6\xb8\xae";
			strcpy(word, text);
		}
		break;
		case 2:
		{
			const char* text = "\xc7\xcf\xbe\xe7\xc1\xa6\xb8\xae";
			strcpy(word, text);
		}
		break;
		case 3:
		{
			const char* text = "\xba\xf4\xb8\xae";
			strcpy(word, text);
		}
		break;
		case 4:
		{
			const char* text = "\xb6\xf3\xb9\xcc";
			strcpy(word, text);
		}
		break;
		}
		strcpy(m_desc,
			MakeStr(
				"%s\xbf\xcd \xc7\xd4\xb2\xb2 \xc7\xc3\xb7\xb9\xc0\xcc\xc7\xd1 \xc0\xaf\xc0\xfa\xb0\xa1 1\xb5\xee\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_7 : public DescBlock
{
public:
	Mission_7(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		sprintf(m_desc,
			"%d\xc0\xa7\xb8\xa6 \xc7\xd1 \xc0\xaf\xc0\xfa\xb4\xc2 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
			param1);
	}
};

class Mission_8 : public DescBlock
{
public:
	Mission_8(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"\xb8\xf0\xb5\xe7 \xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba \xb0\xc5\xb8\xae\xb0\xa1 %s\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_9 : public DescBlock
{
public:
	Mission_9(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xbf\xa9\xc0\xda";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xb3\xb2\xc0\xda";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"1\xc0\xa7\xb0\xa1 %s(\xc4\xb3\xb8\xaf\xc5\xcd)\xc0\xcf \xb0\xe6\xbf\xec \xb8\xf0\xb5\xe7 %s(\xc4\xb3\xb8\xaf\xc5\xcd)\xb4\xc2 \xb9\xcc\xbc\xc7 \xbc\xba\xb0\xf8.",
				word, word));
	}
};

class Mission_10 : public DescBlock
{
public:
	Mission_10(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[16] = { 0 };
		switch (param2)
		{
		case 0:
		{
			const char* text = "\xb4\xa9\xb8\xae";
			strcpy(word, text);
		}
		break;
		case 1:
		{
			const char* text = "\xc7\xcf\xb3\xaa";
			strcpy(word, text);
		}
		break;
		case 2:
		{
			const char* text = "\xbe\xc6\xc0\xfa";
			strcpy(word, text);
		}
		break;
		case 3:
		{
			const char* text = "\xbc\xbc\xbd\xc7";
			strcpy(word, text);
		}
		break;
		case 4:
		{
			const char* text = "\xb8\xc6\xbd\xba";
			strcpy(word, text);
		}
		break;
		case 5:
		{
			const char* text = "\xc4\xed";
			strcpy(word, text);
		}
		break;
		case 6:
		{
			const char* text = "\xbe\xc6\xb8\xb0";
			strcpy(word, text);
		}
		break;
		case 7:
		{
			const char* text = "\xc4\xab\xc1\xee";
			strcpy(word, text);
		}
		break;
		case 8:
		{
			const char* text = "\xb7\xe7\xbd\xc3\xbe\xc6";
			strcpy(word, text);
		}
		break;
		case 9:
		{
			const char* text = "\xb3\xda";
			strcpy(word, text);
		}
		break;
		}
		strcpy(m_desc,
			MakeStr(
				"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 10y \xb9\xcc\xb8\xb8\xc0\xc7 \xb8\xf0\xb5\xe7 %s \xc0\xaf\xc0\xfa\xb4\xc2 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_11 : public DescBlock
{
public:
	Mission_11(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[2][8] = { 0 };
		if (!param1)
		{
			const char* first = "\xc2\xa6\xbc\xf6";
			strcpy(word[0], first);
			const char* second = "\xc8\xa6\xbc\xf6";
			strcpy(word[1], second);
		}
		else
		{
			const char* first = "\xc8\xa6\xbc\xf6";
			strcpy(word[0], first);
			const char* second = "\xc2\xa6\xbc\xf6";
			strcpy(word[1], second);
		}
		strcpy(m_desc,
			MakeStr(
				"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 %s\xc0\xce \xc0\xaf\xc0\xfa\xb0\xa1 %s\xc0\xce \xc0\xaf\xc0\xfa\xba\xb8\xb4\xd9 \xb8\xb9\xc0\xb8\xb8\xe9 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word[0], word[1]));
	}
};

class Mission_12 : public DescBlock
{
public:
	Mission_12(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xb0\xfa\xb9\xdd\xbc\xf6 \xc0\xcc\xbb\xf3\xc0\xcc OUT(\xc4\xa8\xc0\xce)\xc0\xbb \xc7\xdf\xc0\xbb \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_13 : public DescBlock
{
public:
	Mission_13(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xbf\xa9\xc0\xda";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xb3\xb2\xc0\xda";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae 1y\xc0\xcc\xb3\xbb\xc0\xc7 \xc0\xaf\xc0\xfa\xb0\xa1 %s(\xc4\xb3\xb8\xaf\xc5\xcd)\xc0\xce \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8",
				word));
	}
};

class Mission_14 : public DescBlock
{
public:
	Mission_14(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		switch (param1)
		{
		case 1:
		{
			const char* text = "1y";
			strcpy(word, text);
		}
		break;
		case 2:
		{
			const char* text = "2y";
			strcpy(word, text);
		}
		break;
		case 3:
		{
			const char* text = "3y";
			strcpy(word, text);
		}
		break;
		case 4:
		{
			const char* text = "4y";
			strcpy(word, text);
		}
		break;
		case 5:
		{
			const char* text = "5y";
			strcpy(word, text);
		}
		break;
		}
		strcpy(m_desc,
			MakeStr(
				"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 %s\xb9\xcc\xb8\xb8\xc0\xce \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8",
				word));
	}
};

class Mission_15 : public DescBlock
{
public:
	Mission_15(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param2)
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"%d\xc0\xa7 \xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 %s\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				param1, word));
	}
};

class Mission_16 : public DescBlock
{
public:
	Mission_16(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"4\xb8\xed \xc0\xcc\xbb\xf3\xc0\xc7 \xc0\xaf\xc0\xfa\xb0\xa1 \xc4\xa8\xc0\xce(OUT)\xc0\xbb \xc7\xd1 \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8";
		strcpy(m_desc, text);
	}
};

class Mission_17 : public DescBlock
{
public:
	Mission_17(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		strcpy(m_desc,
			MakeStr(
				"\xb8\xf0\xb5\xe7\xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae \xc7\xd5\xb0\xe8\xb0\xa1 %dy \xb9\xcc\xb8\xb8\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				param2));
	}
};

class Mission_18 : public DescBlock
{
public:
	Mission_18(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		sprintf(m_desc,
			"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 %d.%d \xc0\xce \xc0\xaf\xc0\xfa\xb0\xa1 \xc0\xd6\xc0\xbb \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
			param1, param2);
	}
};

class Mission_19 : public DescBlock
{
public:
	Mission_19(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[2][8] = { 0 };
		if (!param1)
		{
			const char* text = "\xb3\xb2\xc0\xda";
			strcpy(word[0], text);
		}
		else
		{
			const char* text = "\xbf\xa9\xc0\xda";
			strcpy(word[0], text);
		}
		if (!param2)
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word[1], text);
		}
		else
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word[1], text);
		}
		strcpy(m_desc,
			MakeStr(
				"%s(\xc4\xb3\xb8\xaf\xc5\xcd)\xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae \xc7\xd5\xb0\xe8\xb0\xa1 %s\xc0\xce \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word[0], word[1]));
	}
};

class Mission_20 : public DescBlock
{
public:
	Mission_20(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 \xb0\xa1\xc0\xe5 \xb8\xd5 \xc0\xaf\xc0\xfa\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_21 : public DescBlock
{
public:
	Mission_21(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		sprintf(m_desc,
			MakeStr(
				"\xb8\xf0\xb5\xe7\xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3\xc0\xcc %d\xc3\xca \xb9\xcc\xb8\xb8\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				param1));
	}
};

class Mission_22 : public DescBlock
{
public:
	Mission_22(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"1\xc0\xa7\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xc7 \xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3\xc0\xcc 10\xc3\xca \xb9\xcc\xb8\xb8\xc0\xcf\xb6\xa7 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_23 : public DescBlock
{
public:
	Mission_23(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[16] = { 0 };
		switch (param1)
		{
		case 0:
		{
			const char* text = "\xba\xc0\xb4\xd9\xb8\xae";
			strcpy(word, text);
		}
		break;
		case 1:
		{
			const char* text = "\xc7\xc7\xc7\xc9";
			strcpy(word, text);
		}
		break;
		case 2:
		{
			const char* text = "\xb6\xec\xb6\xa5\xbb\xd1";
			strcpy(word, text);
		}
		break;
		case 3:
		{
			const char* text = "\xb5\xb9\xc7\xc7\xb4\xcf";
			strcpy(word, text);
		}
		break;
		case 4:
		{
			const char* text = "\xb7\xce\xb7\xce";
			strcpy(word, text);
		}
		break;
		case 5:
		{
			const char* text = "\xc5\xa5\xb8\xb6";
			strcpy(word, text);
		}
		break;
		case 6:
		{
			const char* text = "\xc6\xbc\xc5\xb0";
			strcpy(word, text);
		}
		break;
		case 7:
		{
			const char* text = "\xc4\xab\xb5\xf0\xbf\xa1";
			strcpy(word, text);
		}
		break;
		}
		sprintf(m_desc,
			"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae 20y\xb9\xcc\xb8\xb8, %s(\xc4\xb3\xb5\xf0)\xbb\xe7\xbf\xeb \xc0\xaf\xc0\xfa\xb4\xc2 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
			word);
	}
};

class Mission_24 : public DescBlock
{
public:
	Mission_24(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xb0\xa1\xc0\xe5 \xba\xfc\xb8\xa5\xbd\xc3\xb0\xa3\xbe\xc8\xbf\xa1 \xc5\xb8\xb1\xb8\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_25 : public DescBlock
{
public:
	Mission_25(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[8] = { 0 };
		if (!param1)
		{
			const char* text = "\xc2\xa6\xbc\xf6";
			strcpy(word, text);
		}
		else
		{
			const char* text = "\xc8\xa6\xbc\xf6";
			strcpy(word, text);
		}
		strcpy(m_desc,
			MakeStr(
				"\xb3\xb2\xc0\xba\xb0\xc5\xb8\xae\xb0\xa1 %s\xc0\xce \xc0\xaf\xc0\xfa\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
				word));
	}
};

class Mission_26 : public DescBlock
{
public:
	Mission_26(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xb0\xa1\xc0\xe5 \xb4\xca\xb0\xd4 \xc5\xb8\xb1\xb8\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xcf \xb0\xe6\xbf\xec \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_27 : public DescBlock
{
public:
	Mission_27(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xc4\xb3\xb8\xaf\xc5\xcd \xbc\xba\xba\xb0\xb7\xce \xb3\xb2\xc0\xba\xb0\xc5\xb8\xae \xc6\xf2\xb1\xd5\xc0\xbb \xb3\xbb\xbe\xee \xc0\xfb\xc0\xba\xc2\xca\xc0\xcc \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_28 : public DescBlock
{
public:
	Mission_28(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xc4\xb3\xb8\xaf\xc5\xcd \xbc\xba\xba\xb0\xb7\xce \xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3 \xc6\xf2\xb1\xd5\xc0\xbb \xb3\xbb\xbe\xee \xb8\xb9\xc0\xba\xc2\xca\xc0\xcc \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

class Mission_29 : public DescBlock
{
public:
	Mission_29(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		if (name)
			strcpy(m_desc,
				MakeStr(
					"[%s]\xb4\xd4\xc0\xcc \xc4\xa8\xc0\xce\xc0\xbb \xc7\xcf\xb8\xe9 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.",
					name));
	}
};

class Mission_30 : public DescBlock
{
public:
	Mission_30(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		char word[32] = { 0 };
		switch (param1)
		{
		case 2:
		{
			const char* text = "\xc5\xe4\xb8\xb6\xc8\xa3\xc5\xa9";
			strcpy(word, text);
		}
		break;
		case 4:
		{
			const char* text = "\xbd\xba\xc6\xc4\xc0\xcc\xc5\xa9";
			strcpy(word, text);
		}
		break;
		case 8:
		{
			const char* text = "\xc4\xda\xba\xea\xb6\xf3";
			strcpy(word, text);
		}
		break;
		}
		sprintf(m_desc,
			"%s\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc4\xa8\xc0\xce\xc0\xbb \xc7\xd1 \xc0\xaf\xc0\xfa\xb4\xc2 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8",
			word);
	}
};

class Mission_31 : public DescBlock
{
public:
	Mission_31(unsigned long param1, unsigned long param2, const char* name)
	{
		BuildDesc(param1, param2, name);
	}
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
		const char* text =
			"\xb0\xa1\xc0\xe5 \xbb\xa1\xb8\xae \xc4\xa8\xc0\xce\xc0\xbb \xc7\xd1 \xc0\xaf\xc0\xfa\xb4\xc2 \xb9\xcc\xbc\xc7\xbc\xba\xb0\xf8.";
		strcpy(m_desc, text);
	}
};

CApproachDesc::CApproachDesc(int mission, unsigned long param1,
	unsigned long param2, const char* name)
	: m_desc(NULL)
{
	switch (mission)
	{
	case 1:
		m_desc = new Mission_1(param1, param2, name);
		break;
	case 2:
		m_desc = new Mission_2(param1, param2, name);
		break;
	case 3:
		m_desc = new Mission_3(param1, param2, name);
		break;
	case 4:
		m_desc = new Mission_4(param1, param2, name);
		break;
	case 5:
		m_desc = new Mission_5(param1, param2, name);
		break;
	case 6:
		m_desc = new Mission_6(param1, param2, name);
		break;
	case 7:
		m_desc = new Mission_7(param1, param2, name);
		break;
	case 8:
		m_desc = new Mission_8(param1, param2, name);
		break;
	case 9:
		m_desc = new Mission_9(param1, param2, name);
		break;
	case 10:
		m_desc = new Mission_10(param1, param2, name);
		break;
	case 11:
		m_desc = new Mission_11(param1, param2, name);
		break;
	case 12:
		m_desc = new Mission_12(param1, param2, name);
		break;
	case 13:
		m_desc = new Mission_13(param1, param2, name);
		break;
	case 14:
		m_desc = new Mission_14(param1, param2, name);
		break;
	case 15:
		m_desc = new Mission_15(param1, param2, name);
		break;
	case 16:
		m_desc = new Mission_16(param1, param2, name);
		break;
	case 17:
		m_desc = new Mission_17(param1, param2, name);
		break;
	case 18:
		m_desc = new Mission_18(param1, param2, name);
		break;
	case 19:
		m_desc = new Mission_19(param1, param2, name);
		break;
	case 20:
		m_desc = new Mission_20(param1, param2, name);
		break;
	case 21:
		m_desc = new Mission_21(param1, param2, name);
		break;
	case 22:
		m_desc = new Mission_22(param1, param2, name);
		break;
	case 23:
		m_desc = new Mission_23(param1, param2, name);
		break;
	case 24:
		m_desc = new Mission_24(param1, param2, name);
		break;
	case 25:
		m_desc = new Mission_25(param1, param2, name);
		break;
	case 26:
		m_desc = new Mission_26(param1, param2, name);
		break;
	case 27:
		m_desc = new Mission_27(param1, param2, name);
		break;
	case 28:
		m_desc = new Mission_28(param1, param2, name);
		break;
	case 29:
		m_desc = new Mission_29(param1, param2, name);
		break;
	case 30:
		m_desc = new Mission_30(param1, param2, name);
		break;
	case 31:
		m_desc = new Mission_31(param1, param2, name);
		break;
	}
}
