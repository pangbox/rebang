#pragma once

#include "shareddoc.h"

inline unsigned char GetAccumulateLimit(unsigned char grade)
{
	unsigned char limit = 0;
	switch (grade)
	{
	case 0:
		limit = 8;
		break;
	case 1:
		limit = 14;
		break;
	case 2:
		limit = 0;
		break;
	}
	return limit;
}

inline unsigned char GetGradeBegin_Grade(unsigned char grade)
{
	unsigned char begin = 0;
	switch (grade)
	{
	case 0:
		begin = 0;
		break;
	case 1:
		begin = 8;
		break;
	case 2:
		begin = 0;
		break;
	}
	return begin;
}

bool IsCompleteBeforeGrade(unsigned char grade)
{
	switch (grade)
	{
	case 0:
		return true;
	case 1:
		return Doc()->m_tutorialComplete[0] == 0xFF ? true : false;
	case 2:
		if (Doc()->m_tutorialComplete[0] == 0xFF &&
			Doc()->m_tutorialComplete[1] == 0x3F00)
			return true;
		return false;
	}
	return false;
}
