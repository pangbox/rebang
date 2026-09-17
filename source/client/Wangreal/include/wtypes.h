#pragma once

class WPoint
{
public:
	float x;
	float y;
};

class WRect
{
public:
	template <class Width, class Height>
	WRect(float x, float y, Width width, Height height)
		: x(x), y(y), width((float)width), height((float)height)
	{
	}

	float x;
	float y;
	float width;
	float height;
};
