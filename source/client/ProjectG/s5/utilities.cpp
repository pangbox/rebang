#include "stdafx.h"
#include "utilities.h"
#include "equipskin.h"
#include "parttidlist.h"
#include "contentsdoc.h"
#include "gatewayactor.h"
#include "wlocalize.h"

namespace
{
	unsigned long l_dwSRVPROP;
}

namespace S5
{
	namespace UTIL
	{
		void SetServerProperty(unsigned long property)
		{
			l_dwSRVPROP = property;
		}

		unsigned long GetServerProperty()
		{
			return l_dwSRVPROP;
		}

		int EncodingUrl(const char* source, char* dest)
		{
			int length = lstrlen(source);
			if (!length)
				return 0;
			const char* p = source;
			char plain[] = "/?:=,<>\"-_.~'(){}\\[]";
			int escaped = 0;
			for (; p < source + length; ++p)
				if (*p != ' ' && !(*p >= '0' && *p <= '9') &&
					!(*p >= 'A' && *p <= 'Z') && !(*p >= 'a' && *p <= 'z') &&
					!strchr(plain, *p))
					++escaped;
			int size = length + 2 * escaped + 1;
			if (dest)
			{
				char* out = dest;
				const char* cursor = source;
				for (; cursor < source + length; ++cursor, ++out)
				{
					if (*cursor == ' ')
						*out = '+';
					else if ((*cursor >= '0' && *cursor <= '9') ||
						(*cursor >= 'A' && *cursor <= 'Z') ||
						(*cursor >= 'a' && *cursor <= 'z') ||
						strchr(plain, *cursor))
						*out = *cursor;
					else
					{
						*out++ = '%';
						unsigned char high = (unsigned char)*cursor >> 4;
						if (high <= 9)
							*out++ = high + '0';
						else
							*out++ = high + 'A' - 10;
						unsigned char low = *cursor & 15;
						if (low <= 9)
							*out = low + '0';
						else
							*out = low + 'A' - 10;
					}
				}
				*out = 0;
			}
			return size;
		}

		int DecodingUrl(const char* source, char* dest)
		{
			int length = lstrlen(source);
			if (!length)
				return 0;
			const char* p = source;
			const char* end = source + length;
			int escaped = 0;
			for (; p < end - 2;)
			{
				if (*p == '%' &&
					((p[1] >= '0' && p[1] <= '9') ||
						(p[1] >= 'A' && p[1] <= 'F') ||
						(p[1] >= 'a' && p[1] <= 'f')) &&
					((p[2] >= '0' && p[2] <= '9') ||
						(p[2] >= 'A' && p[2] <= 'F') ||
						(p[2] >= 'a' && p[2] <= 'f')))
				{
					++escaped;
					p += 3;
				}
				else
					++p;
			}
			int size = length - 2 * escaped + 1;
			if (dest)
			{
				while (source < end)
				{
					if (*source == '%' &&
						((source[1] >= '0' && source[1] <= '9') ||
							(source[1] >= 'A' && source[1] <= 'F') ||
							(source[1] >= 'a' && source[1] <= 'f')) &&
						((source[2] >= '0' && source[2] <= '9') ||
							(source[2] >= 'A' && source[2] <= 'F') ||
							(source[2] >= 'a' && source[2] <= 'f')))
					{
						if (source[1] >= '0' && source[1] <= '9')
							*dest = (source[1] - '0') * 16;
						else if (source[1] >= 'A' && source[1] <= 'F')
							*dest = (source[1] - 'A' + 10) * 16;
						else
							*dest = (source[1] - 'a' + 10) * 16;
						char low = source[2];
						if (low >= '0' && low <= '9')
							*dest += low - '0';
						else if (low >= 'A' && low <= 'F')
							*dest += low - 'A' + 10;
						else
							*dest += low - 'a' + 10;
						++dest;
						source += 3;
					}
					else
					{
						if (*source == '+')
							*dest = ' ';
						else
							*dest = *source;
						++dest;
						++source;
					}
				}
				*dest = 0;
			}
			return size;
		}

		int GetEquipSkinIndex(eEquipSkin& index, unsigned long typeId)
		{
			if ((typeId & 0xfc000000) != 0x38000000)
				return 0;
			switch ((typeId >> 22) & 15)
			{
			case 0:
				index = EQUIP_SKIN_WINDOW_BASE;
				return 1;
			case 1:
				index = EQUIP_SKIN_WINDOW_FRAME;
				return 1;
			case 2:
				index = EQUIP_SKIN_WINDOW_STICKER;
				return 1;
			case 3:
				index = EQUIP_SKIN_SLOT_BASE;
				return 1;
			case 4:
				index = EQUIP_SKIN_SLOT_FACE;
				return 1;
			case 6:
				index = EQUIP_SKIN_TITLE;
				return 1;
			}
			return 0;
		}

		int Base64Encode(char* input, int length, char* dest)
		{
			unsigned char encoded[4];
			unsigned char block[3] = { 0, 0, 0 };
			int output = 0;
			char* last = input + length - 1;
			int size = (length % 3 ? 4 : 0) + 4 * (length / 3) + 1;
			int i = 0;
			char* source = input;
			for (; source <= last; ++i, ++source)
			{
				int count = i % 3;
				block[count] = *source;
				if (count == 2 || source == last)
				{
					encoded[0] = block[0] >> 2;
					encoded[1] = (block[1] >> 4) | ((block[0] & 3) << 4);
					encoded[2] = (block[2] >> 6) | ((block[1] & 15) << 2);
					encoded[3] = block[2] & 63;
					dest[output++] = s_base64Table[encoded[0]];
					dest[output++] = s_base64Table[encoded[1]];
					dest[output++] =
						count == 0 ? '=' : s_base64Table[encoded[2]];
					dest[output++] =
						count < 2 ? '=' : s_base64Table[encoded[3]];
					block[0] = block[1] = block[2] = 0;
				}
			}
			dest[output] = 0;
			return size;
		}

		int Base64Decode(char* input, unsigned char* dest, int length)
		{
			int previous = 0;
			int output = 0;
			int state = 0;
			char* source = input;
			for (; *source; ++source)
			{
				int value = s_base64Index[*source];
				if (value == -1)
					continue;
				switch (state)
				{
				case 0:
					state = 1;
					break;
				case 1:
					if (output < length)
						dest[output++] = (previous << 2) | ((value >> 4) & 3);
					state = 2;
					break;
				case 2:
					if (output < length)
						dest[output++] = (previous << 4) | ((value >> 2) & 15);
					state = 3;
					break;
				case 3:
					if (output < length)
						dest[output++] = value | (previous << 6);
					state = 0;
					break;
				}
				previous = value;
			}
			if (output > 0)
				dest[output] = 0;
			return output;
		}

		int IsLunarYear(unsigned short year)
		{
			if (year % 4 == 0)
			{
				if (year % 100 == 0)
				{
					if (year % 400 == 0)
						return 1;
					else
						return 0;
				}
				else
					return 1;
			}
			else
				return 0;
		}

		unsigned short GetMonthlyDayCount(unsigned short year,
			unsigned short month)
		{
			if (month == 2)
			{
				if (IsLunarYear(year))
					return 28;
				return 29;
			}
			else if (month == 4 || month == 6 || month == 9 || month == 11)
				return 30;
			return 31;
		}

		int GetHoleinCoinCount(int count, int type)
		{
			if (!count)
				return count;
			if (type >= 2)
				return count * type;
			return count;
		}

		int GetWindowBorderSize(HWND window)
		{
			RECT outer, inner;
			GetWindowRect(window, &outer);
			GetClientRect(window, &inner);
			return ((outer.right - outer.left) - (inner.right - inner.left)) /
				2;
		}

		int GetWindowTitleBarSize(HWND window)
		{
			RECT outer, inner;
			GetWindowRect(window, &outer);
			GetClientRect(window, &inner);
			return (outer.bottom - outer.top) - (inner.bottom - inner.top) -
				GetWindowBorderSize(window);
		}

		bool IsInRect(const RECT& rect, int x, int y)
		{
			return x >= rect.left && x <= rect.right && y >= rect.top &&
				y <= rect.bottom;
		}

		bool IsOverlappedRect(const RECT& a, const RECT& b)
		{
			if (IsInRect(b, a.left, a.top) || IsInRect(b, a.left, a.bottom) ||
				IsInRect(b, a.right, a.top) || IsInRect(b, a.right, a.bottom) ||
				IsInRect(a, b.left, b.top) || IsInRect(a, b.left, b.bottom) ||
				IsInRect(a, b.right, b.top) || IsInRect(a, b.right, b.bottom))
				return true;
			return false;
		}

		bool CalculateUVPos(WRect& rect, int imageWidth, int imageHeight,
			float cellWidth, float cellHeight, int index)
		{
			if (imageWidth < cellWidth || imageHeight < cellHeight)
				return false;
			int rows = imageHeight / cellHeight;
			int cols = imageWidth / cellWidth;
			if (rows * cols < index)
				return false;
			float u = (index / rows) * cellWidth,
				  v = (index % rows) * cellHeight;
			rect.x = u * (1.0f / imageWidth);
			rect.y = v * (1.0f / imageHeight);
			rect.w = cellWidth / imageWidth;
			rect.h = cellHeight / imageHeight;
			return true;
		}

		void SendCutInPacket(unsigned long uid, short type,
			unsigned long gameMode, unsigned long charTid)
		{
			WSendPacket packet((enumClientPacket)231);
			packet.Encode4(uid);
			packet.Encode2(type);
			packet.Encode4(gameMode);
			packet.Encode4(charTid);
			packet.Send(TO_GAME);
		}

		unsigned long GetNickColor(unsigned long identity, int premium,
			unsigned short flag)
		{
			if (identity & 0x14)
			{
				if (Doc()->m_myInfo.info.dwIdentity & 0x14)
					return 0x50ffffff;
				if (!(flag & 1))
					return 0;
			}
			else if (IsLocalContent((localContentType_t)16) && premium)
				return 0xff33dbff;
			return 0xffffffff;
		}

		bool IsEventServerControl(eGameServerProperty property)
		{
			if (property & Doc()->m_serverProperty)
				return true;
			return false;
		}

		int ErasePartsCardAll(unsigned long partGuid,
			std::list<unsigned long>& cards)
		{
			std::list<sSCardAvilityPeriodInfo>::iterator
				it = Doc()->m_cardAbilityList[0].begin(),
				end = Doc()->m_cardAbilityList[0].end();
			while (it != end)
			{
				if ((*it).partsUid == partGuid)
				{
					cards.push_back((*it).tid);
					it = Doc()->m_cardAbilityList[0].erase(it);
				}
				else
					++it;
			}
			std::list<sItemInfo>& parts = Doc()->m_partsList;
			std::list<sItemInfo>::iterator part = parts.begin();
			for (; part != parts.end(); ++part)
				if (partGuid == (*part).guid)
					break;
			if (part != Doc()->m_partsList.end())
			{
				for (int i = 0; i < 12; ++i)
				{
					if ((*part).attachCard[i])
						for (std::list<unsigned long>::iterator card =
								 cards.begin();
							card != cards.end(); ++card)
							if ((*part).attachCard[i] == *card)
								(*part).attachCard[i] = 0;
				}
			}
			return cards.size() != 0;
		}

		bool FindVisibleWindowFromExStyle(unsigned long style,
			std::vector<HWND>& windows)
		{
			for (HWND window = FindWindow(NULL, NULL); window;
				window = GetWindow(window, GW_HWNDNEXT))
				if (IsWindowVisible(window) &&
					(style & GetWindowLong(window, GWL_EXSTYLE)) == style)
					windows.push_back(window);
			return !windows.empty();
		}
	}

	bool UnderCatogory(unsigned long charTid, unsigned long category,
		IFF_STRUCT::sPart* part)
	{
		if (!(part->c.TypeId & 0x600) &&
			((part->c.TypeId >> 18) & 0xff) == (charTid & 0x03ffffff))
		{
			if ((1 << part->Category) & category)
				return true;
			else
				return false;
		}
		return false;
	}

	void GetLine(char* source, char* dest)
	{
		while (*source)
		{
			if (*source == '\\' && source[1] == 'n')
			{
				*dest++ = '\n';
				source += 2;
			}
			else
				*dest++ = *source++;
		}
	}

	bool UnderCategory(unsigned long charTid, unsigned long partTid,
		unsigned long category)
	{
		if (!(partTid & 0x600) &&
			((partTid >> 18) & 0xff) == (charTid & 0x03ffffff))
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(partTid);
			if (part && ((1 << part->Category) & category))
				return true;
		}
		return false;
	}

	bool IsUsableCalipers()
	{
		IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
		if (!actor)
			return false;
		int usable = 0;
		actor << MsgObject(NULL, 623, 1, (int)&usable, 0, 0, 0);
		if (usable == 1)
			return true;
		return false;
	}
}
namespace _util
{
	const char* GetAvatarServerServiceMessage(int code)
	{
		const char* result = NULL;
		switch (code)
		{
		case 2:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
		case 2048:
		case 4096:
			result = K2L_Compatibility(
				"\xb4\xeb\xc8\xad\xb9\xe6\x20\xc0\xfc\xbf\xeb\x20\xbc\xad\xb9\xf6\xbf\xa1\xbc\xad\xb4\xc2\x20\xb0\xd4\xc0\xd3\xb9\xe6\xc0\xbb\x20\xb8\xb8\xb5\xe9\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e");
			break;
		}
		return result;
	}

	const char* GetNormalServerServiceMessage(int code)
	{
		const char* result = NULL;
		if (code == 8192)
			result = K2L_Compatibility(
				"\xc7\xf6\xc0\xe7\x20\xbc\xad\xb9\xf6\xbf\xa1\xbc\xad\xb4\xc2\x20\xb4\xeb\xc8\xad\xb9\xe6\xc0\xbb\x20\xb8\xb8\xb5\xe9\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e");
		return result;
	}

	bool IsAvatarChatServer(int property)
	{
		if (property & Doc()->m_curGameServer.eventFlags)
			return true;
		return false;
	}

	void GenerateRandomTidParts(unsigned long charTid,
		unsigned long* const parts, unsigned long* const auxiliary)
	{
		CPartTidList combo;
		*auxiliary = 0;
		*parts = 0;
		Doc()->m_itemManager.GetDefCombo(charTid, parts);
		do
		{
			switch (charTid & 0x03ffffff)
			{
			case 0:
				parts[0] = (rand() % 11) | 0x8000800;
				parts[3] = (rand() % 20) | 0x8006000;
				parts[4] = (rand() % 3) | 0x8008800;
				parts[5] = (rand() % 17) | 0x800A000;
				parts[8] = (rand() % 11) | 0x8010000;
				break;
			case 1:
				parts[0] = (rand() % 20) | 0x8040800;
				parts[2] = (rand() % 20) | 0x8044000;
				parts[4] = (rand() % 11) | 0x8048000;
				parts[7] = (rand() % 10) | 0x804E000;
				break;
			case 2:
				parts[0] = (rand() % 9) | 0x8080800;
				parts[2] = (rand() % 18 + 1) | 0x8084000;
				parts[4] = (rand() % 4) | 0x8088800;
				parts[5] = (rand() % 14 + 1) | 0x808A000;
				parts[7] = (rand() % 11) | 0x808E000;
				parts[8] = (rand() % 3) | 0x8090800;
				break;
			case 3:
				parts[0] = (rand() % 7) | 0x80C0000;
				parts[2] = (rand() % 22) | 0x80C4000;
				parts[4] = (rand() % 5) | 0x80C8000;
				parts[5] = (rand() % 15) | 0x80CA000;
				parts[7] = (rand() % 12) | 0x80CE000;
				parts[10] = (rand() % 4) | 0x80D4800;
				break;
			case 4:
				parts[0] = (rand() % 6) | 0x8100000;
				parts[2] = (rand() % 17) | 0x8104000;
				parts[3] = (rand() % 8) | 0x8106000;
				parts[4] = (rand() % 12) | 0x8108000;
				parts[5] = (rand() % 10) | 0x810A000;
				parts[9] = (rand() % 4) | 0x8112000;
				parts[10] = (rand() % 2) | 0x8114000;
				parts[11] = (rand() % 2) | 0x8116000;
				break;
			case 5:
				parts[0] = (rand() % 6) | 0x8140000;
				parts[2] = (rand() % 12) | 0x8144000;
				parts[5] = (rand() % 13) | 0x814A000;
				parts[7] = (rand() % 15) | 0x814E000;
				break;
			case 6:
				parts[0] = (rand() % 10) | 0x8180000;
				parts[2] = (rand() % 15) | 0x8184000;
				parts[4] = (rand() % 8) | 0x8188000;
				parts[5] = (rand() % 9) | 0x818A000;
				break;
			case 8:
				parts[0] = 136314880;
				parts[2] = 136331288;
				parts[4] = 136347648;
				parts[5] = 136355840;
				parts[7] = 136372224;
				break;
			case 9:
				parts[0] = 136577054;
				parts[2] = 136593408;
				parts[4] = 136609795;
				parts[5] = 136617984;
				parts[7] = 136634375;
				break;
			default:
				break;
			}

			sCharacterInfo info;
			info.tid = charTid;
			memcpy(info.tidParts, parts, 0x60);
			combo.SetTids(&info, 0xff);
		} while (!combo.IsComboValid());
	}
}
