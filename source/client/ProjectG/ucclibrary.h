#pragma once

#include <string.h>
#include <string>

class WReceivedPacket;
class FrGraphicInterface;
struct sItemInfo;
struct sUserInfoTime;
struct sUserInfo;
struct sRoomSlot;
struct sCharacterInfo;
struct sMailIncludeItem;
namespace IFF_STRUCT
{
	struct sPart;
}

namespace ranktype
{
	struct sRankUserInfo;
}

struct sUccHeader
{
	unsigned long flags;
	unsigned long unknown4;
	unsigned long unknown8;
	unsigned char reserved[256];

	void Initialize()
	{
		flags = 0;
		unknown4 = 0;
		unknown8 = 0;
		memset(reserved, 0, sizeof(reserved));
	}
};

template <class T>
inline T Max(T a, T b, T c)
{
	return Max(a, Max(b, c));
}

template <class T>
inline T Min(T a, T b, T c)
{
	return Min(a, Min(b, c));
}

struct sHSV;

struct sRGB
{
	sRGB() { r = g = b = 0; }

	sHSV toHSV();

	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct sHSV
{
	sHSV() { h = s = v = 0; }

	sRGB toRGB();

	unsigned char h;
	unsigned char s;
	unsigned char v;
};

struct sRGBA : public sRGB
{
	sRGBA() { a = 0; }

	sRGBA(unsigned char _r, unsigned char _g, unsigned char _b)
	{
		r = _r;
		g = _g;
		b = _b;
		a = 0xff;
	}

	sRGBA(unsigned char _r, unsigned char _g, unsigned char _b,
		unsigned char _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	sRGBA& operator=(const sRGBA& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
		return *this;
	}

	bool operator==(const sRGBA& c)
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	void RemoveAlpha();

	unsigned char a;
};

struct IPoint
{
	IPoint()
	{
		x = 0;
		y = 0;
	}

	IPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	IPoint(float _x, float _y)
	{
		x = (int)_x;
		y = (int)_y;
	}

	bool operator==(const IPoint& p) const { return x == p.x && y == p.y; }

	int x;
	int y;
};

void RGBToHLS(unsigned char r, unsigned char g, unsigned char b,
	unsigned char& h, unsigned char& l, unsigned char& s);
void HLSToRGB(unsigned char h, unsigned char l, unsigned char s,
	unsigned char& r, unsigned char& g, unsigned char& b);
void ClearTexCache(Bitmap* bitmap);
void Convert24to32(Bitmap* src, Bitmap* dst, const Bitmap* mask);
void CreateItemThumbnail(const Bitmap* texture, const Bitmap* mask,
	Bitmap* thumbnail);
FrGraphicInterface* GetGDI();
void RefreshTexCache(Bitmap* bitmap);
void RefreshTexCache(Bitmap& bitmap);

bool IsUccClothes(unsigned long typeId);
const Bitmap* GetUccItemIcon(unsigned long guid);
const char* GetUccItemName(unsigned long guid);

const Bitmap* GetUccItemIcon(sItemInfo* item);
const char* GetUccItemName(sItemInfo* item);
sItemInfo* GetMyItemInfo(unsigned long id);
sItemInfo* GetMyItemInfo(unsigned long typeId, const char* uccIndex,
	unsigned short seq);
bool IsSameClothes(IFF_STRUCT::sPart* first, IFF_STRUCT::sPart* second);

void GetResizedImage(const Bitmap* source, const Bitmap* mask, Bitmap* dest,
	int width, int height);

const char* GetUccTempFoler();
std::string GenerateTextureFullPath(int side, unsigned long typeId,
	const char* uccIndex);
std::string GenerateIconFilename(unsigned long typeId, const char* uccIndex);
std::string GetMaskFilename(const char* filename);
bool ZipUccClothes(Bitmap* front, Bitmap* back, Bitmap* icon,
	const char* filename);
void Convert32to24(Bitmap* source, Bitmap* dest, const Bitmap* mask);

void RequestUccInfo(sUserInfoTime& user);
void RequestUccInfo(sUserInfo& user);
void RequestUccInfo(sRoomSlot& slot);
void RequestUccInfo(ranktype::sRankUserInfo& user);
void RequestUccInfo(sCharacterInfo& character);
void RequestUccInfo(sMailIncludeItem& item);

std::string GenerateTextureName_F(unsigned long typeId, const char* index);
std::string GenerateTextureName_B(unsigned long typeId, const char* index);

const Bitmap* GetUccItemIcon(unsigned long typeId, const char* index);
const char* GetUccItemName(unsigned long typeId, const char* index);
bool IsSameClothes(unsigned long first, unsigned long second);

bool UnzipUccClothes(unsigned char* data, unsigned long size, Bitmap* front,
	Bitmap* back, Bitmap* icon, sUccHeader* header);
bool LoadWarningClothes(unsigned int type, Bitmap* front, Bitmap* back,
	Bitmap* icon);

void MixColor(sRGBA& first, sRGBA& second, sRGBA& result);
void SetBrightness(sRGBA& color, sRGBA& brightness, sRGBA& result);
