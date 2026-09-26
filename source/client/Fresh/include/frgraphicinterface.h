#pragma once
#include "../../Wangreal/include/wtypes.h"

class Bitmap;

class FrGraphicInterface
{
public:
	void DrawTexture(const Bitmap* bitmap, const WRect& src, const WRect& dst,
		unsigned long color, int flags) const;
};

inline unsigned long FrALPHA(unsigned long color, float alpha)
{
	return ((int)((color >> 24) * alpha) << 24) | (color & 0x00ffffff);
}
