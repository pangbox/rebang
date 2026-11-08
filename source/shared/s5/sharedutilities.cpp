#include <ctype.h>
#include <tchar.h>

#include "sharedutilities.h"

static const unsigned long l_CLASSIC_CLEARBONUS[20][3] = {
	{ 20, 40,  60  },
	{ 50, 100, 140 },
	{ 55, 105, 145 },
	{ 80, 150, 190 },
	{ 65, 120, 170 },
	{ 24, 48,  72  },
	{ 50, 100, 140 },
	{ 70, 125, 175 },
	{ 40, 80,  120 },
	{ 55, 105, 145 },
	{ 40, 80,  120 },
	{ 20, 40,  60  },
	{ 0,  0,   0   },
	{ 80, 150, 190 },
	{ 20, 40,  60  },
	{ 20, 40,  60  },
	{ 40, 80,  120 },
	{ 40, 80,  120 },
	{ 0,  0,   0   },
	{ 40, 80,  120 },
};

static const sHoleInBonus l_CLASSIC_HOLEINBONUS[20] = {
	{ 158, 158, 93,  55,  37, 0 },
	{ 243, 243, 143, 84,  56, 0 },
	{ 256, 256, 150, 89,  59, 0 },
	{ 351, 351, 207, 122, 81, 0 },
	{ 256, 256, 150, 89,  59, 0 },
	{ 145, 145, 85,  50,  34, 0 },
	{ 243, 243, 143, 84,  56, 0 },
	{ 280, 280, 164, 97,  65, 0 },
	{ 156, 156, 92,  54,  36, 0 },
	{ 256, 256, 150, 89,  59, 0 },
	{ 176, 176, 103, 61,  41, 0 },
	{ 182, 182, 107, 63,  42, 0 },
	{ 0,   0,   0,   0,   0,  0 },
	{ 340, 340, 200, 118, 79, 0 },
	{ 104, 104, 61,  36,  24, 0 },
	{ 100, 100, 59,  35,  23, 0 },
	{ 173, 173, 102, 60,  40, 0 },
	{ 800, 800, 520, 260, 90, 0 },
	{ 0,   0,   0,   0,   0,  0 },
	{ 173, 173, 102, 60,  40, 0 },
};

static const sHoleInBonus l_NULL_HOLEINBONUS = { 0, 0, 0, 0, 0, 0 };

static sHoleInBonus l_HOLEINBONUS[20] = {
	{ 50,  50,  30,  12,  3,  0 },
	{ 100, 100, 60,  30,  11, 0 },
	{ 110, 110, 65,  32,  12, 0 },
	{ 180, 180, 98,  59,  20, 0 },
	{ 150, 150, 85,  43,  16, 0 },
	{ 55,  55,  36,  17,  5,  0 },
	{ 100, 100, 60,  30,  11, 0 },
	{ 150, 150, 85,  43,  16, 0 },
	{ 80,  80,  52,  26,  9,  0 },
	{ 110, 110, 65,  32,  12, 0 },
	{ 80,  80,  52,  26,  9,  0 },
	{ 50,  50,  30,  12,  3,  0 },
	{ 0,   0,   0,   0,   0,  0 },
	{ 180, 180, 98,  59,  20, 0 },
	{ 50,  50,  30,  12,  3,  0 },
	{ 50,  50,  30,  12,  3,  0 },
	{ 80,  80,  52,  26,  9,  0 },
	{ 800, 800, 520, 260, 90, 0 },
	{ 0,   0,   0,   0,   0,  0 },
	{ 80,  80,  52,  26,  9,  0 },
};

namespace S5
{
	namespace CLASSICSRV
	{
		unsigned long GetClearBonus(unsigned char numPlayer,
			unsigned char mapType)
		{
			int players = numPlayer - 2 < 0 ? 0 : numPlayer - 2;
			int map =
				(mapType <= 0x7f || mapType == 0xfd) ? mapType : mapType - 0x80;

			if (players < 0 || players > 2 || map < 0 || map >= 20)
			{
				return 0;
			}

			return l_CLASSIC_CLEARBONUS[map][players];
		}

		const sHoleInBonus* GetCourseHoleInBonus(unsigned char mapType)
		{
			int map =
				(mapType <= 0x7f || mapType == 0xfd) ? mapType : mapType - 0x80;
			if (map < 0 || map >= 20)
				return &l_NULL_HOLEINBONUS;

			return &l_CLASSIC_HOLEINBONUS[map];
		}
	}

	namespace RENTALPARTS
	{
		int IsRentableParts(unsigned long typeId)
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
			if (part == NULL)
				return FALSE;
			return part->RentalPrice > 0;
		}

		int GetRentalPrice(unsigned long typeId)
		{
			if (IsRentableParts(typeId))
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
				return part->RentalPrice;
			}

			return 0x7fffffff;
		}

		void CollectRentalItem(std::list<sItemInfo>& rentalItems,
			const std::list<sItemInfo>& items)
		{
			for (std::list<sItemInfo>::const_iterator it = items.begin();
				it != items.end(); ++it)
			{
				if ((*it).ItemType == 6)
					rentalItems.push_back(*it);
			}
		}

		int IsExistExpiredRentalItem(const std::list<sItemInfo>& items)
		{
			for (std::list<sItemInfo>::const_iterator it = items.begin();
				it != items.end(); ++it)
			{
				if ((*it).ItemType == 6 && (*it).Expired)
					return TRUE;
			}

			return FALSE;
		}
	}

	__int64 ExchangeOwnCashToCookie(__int64 cash, __int64 cookie, int rate)
	{
		return cash / rate + cookie;
	}

	__int64 ExchangeOwnCookieToCash(__int64 cookie, int rate)
	{
		return rate * cookie;
	}
}

namespace _util
{
	ePwdErrorCode VerifyInputPassword(const std::string& password)
	{
		ePwdErrorCode ret = PWD_OK;

		ret = VerifyLength(password, 5, 8);
		if (ret != PWD_OK)
		{
			return ret;
		}

		ret = VerifyStringContents_IsNumber(password);
		if (ret != PWD_OK)
		{
			return ret;
		}

		return PWD_OK;
	}

	ePwdErrorCode VerifyLength(const std::string& str, int minLength,
		int maxLength)
	{
		int len = str.length();
		if (len == 0)
		{
			return PWD_EMPTY;
		}

		if (minLength > len)
		{
			return PWD_TOO_SHORT;
		}

		if (maxLength < len)
		{
			return PWD_TOO_LONG;
		}

		return PWD_OK;
	}

	ePwdErrorCode VerifyStringContents_IsNumber(const std::string& str)
	{
		int len = str.length();
		for (int i = 0; i < len; i++)
		{
			if (!isdigit(str.at(i)))
			{
				return PWD_NOT_NUMBER;
			}
		}

		return PWD_OK;
	}

	ePwdErrorCode VerifyStringContents_Limit(const std::string& str)
	{
		int len = str.length();

		char password[9] = {
			0,
		};
		_tcsncpy(password, str.c_str(), 8);

		char numbers[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		int start;
		for (start = 0; start < 10; start++)
		{
			if (password[0] == numbers[start])
				break;
		}

		char check[9];
		memset(check, 0, sizeof(check));
		for (int i = 0; i < 8; i++)
			check[i] = numbers[start];

		bool same = true;
		for (int i = 0; i < len; i++)
		{
			if (password[i] != check[i])
			{
				same = false;
				break;
			}
		}

		if (same)
			return PWD_SAME_CHARS;

		if (start < 6)
		{
			memset(check, 0, sizeof(check));
			for (int n = start, i = 0; i < 8 && n < 10; i++, n++)
			{
				check[i] = numbers[n];
			}

			same = true;
			for (int i = 0; i < len; i++)
			{
				if (password[i] != check[i])
				{
					same = false;
					break;
				}
			}

			if (same)
				return PWD_ASCENDING;
		}

		if (start > 3)
		{
			memset(check, 0, sizeof(check));
			for (int n = start, i = 0; i < 8 && n >= 0; i++, n--)
			{
				check[i] = numbers[n];
			}

			same = true;
			for (int i = 0; i < len; i++)
			{
				if (password[i] != check[i])
				{
					same = false;
					break;
				}
			}

			if (same)
				return PWD_DESCENDING;
		}

		return PWD_OK;
	}
}

namespace S5
{
	bool IsSpecialGameStyle(eGameStyle style)
	{
		switch (style)
		{
		case GAME_STYLE_SPECIAL1:
		case GAME_STYLE_SPECIAL2:
			return true;
		}
		return false;
	}

	bool IsSpecialGame(unsigned char gameType, eGameStyle style)
	{
		if (gameType == 14)
		{
			return true;
		}

		switch (style)
		{
		case GAME_STYLE_SPECIAL1:
		case GAME_STYLE_SPECIAL2:
			return true;
		}

		return false;
	}

	SYSTEMTIME MakeSysTime(unsigned short year, unsigned short month,
		unsigned short day, unsigned short hour)
	{
		SYSTEMTIME st;
		memset(&st, 0, sizeof(st));
		st.wYear = year;
		st.wMonth = month;
		st.wDay = day;
		st.wHour = hour;

		return st;
	}
}
