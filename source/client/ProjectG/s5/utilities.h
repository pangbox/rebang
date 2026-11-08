#pragma once

class WRect;
enum eEquipSkin;
enum eGameServerProperty;
namespace IFF_STRUCT
{
	struct sPart;
}

namespace S5
{
	namespace UTIL
	{
		static char s_base64Table[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
			'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
			'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '+', '/' };

		static int s_base64Index[256] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
			-1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
			-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
			42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1 };

		void SetServerProperty(unsigned long property);
		unsigned long GetServerProperty();
		int EncodingUrl(const char* source, char* dest);
		int DecodingUrl(const char* source, char* dest);
		int GetEquipSkinIndex(eEquipSkin& index, unsigned long typeId);
		int Base64Encode(char* pSrc, int nLength, char* pDst);
		int Base64Decode(char* pSrc, unsigned char* pDst, int nLength);
		int IsLunarYear(unsigned short year);
		unsigned short GetMonthlyDayCount(unsigned short year,
			unsigned short month);
		int GetHoleinCoinCount(int count, int type);
		int GetWindowBorderSize(HWND window);
		int GetWindowTitleBarSize(HWND window);
		bool IsInRect(const RECT& rect, int x, int y);
		bool IsOverlappedRect(const RECT& a, const RECT& b);
		bool CalculateUVPos(WRect& rect, int imageWidth, int imageHeight,
			float cellWidth, float cellHeight, int index);
		void SendCutInPacket(unsigned long uid, short type,
			unsigned long gameMode, unsigned long charTid);
		unsigned long GetNickColor(unsigned long identity, int premium,
			unsigned short flag);
		bool IsEventServerControl(eGameServerProperty property);
		int ErasePartsCardAll(unsigned long partGuid,
			std::list<unsigned long>& cards);
		bool FindVisibleWindowFromExStyle(unsigned long style,
			std::vector<HWND>& windows);
	}

	bool UnderCatogory(unsigned long charTid, unsigned long category,
		IFF_STRUCT::sPart* part);
	void GetLine(char* source, char* dest);
	bool UnderCategory(unsigned long charTid, unsigned long partTid,
		unsigned long category);
	bool IsUsableCalipers();
	bool ISCutinItem(unsigned long typeId);

	inline void EnableFreshWindow(FrWnd* wnd, bool enable)
	{
		wnd->SetVisible(enable);
		wnd->Enable(enable);
	}

	inline void EnableFreshWindow(FrWnd* wnd)
	{
		wnd->SetVisible(true);
		wnd->Enable(true);
	}

	inline void DisableFreshWindow(FrWnd* wnd)
	{
		wnd->SetVisible(false);
		wnd->Enable(false);
	}
}

namespace _util
{
	const char* GetAvatarServerServiceMessage(int code);
	const char* GetNormalServerServiceMessage(int code);
	bool IsAvatarChatServer(int property);
	void GenerateRandomTidParts(unsigned long charTid,
		unsigned long* const parts, unsigned long* const auxiliary);
}
