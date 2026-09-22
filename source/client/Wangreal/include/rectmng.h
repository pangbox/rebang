#pragma once
#include "wmempak.h"

struct EmptyRect
{
	int x1;
	int y1;
	int x2;
	int y2;
	int extent;
	EmptyRect* next;
};

class WRectManager
{
public:
	WRectManager(int width, int height) { Clear(width, height); }
	WRectManager() { }
	void Clear(int width, int height)
	{
		mem.Clear();
		list = mem.Alloc();
		list->x1 = list->y1 = 0;
		list->x2 = width;
		list->y2 = height;
		list->next = 0;
	}
	int Alloc(int width, int height, int& x, int& y)
	{
		EmptyRect* rect = Find(width, height);
		if (!rect)
			return 1;
		Divide(rect, width, height, x, y);
		return 0;
	}

private:
	void Divide(EmptyRect* rect, int width, int height, int& x, int& y)
	{
		EmptyRect* extra;
		x = rect->x1;
		y = rect->y1;
		if (rect->x2 - rect->x1 >= rect->y2 - rect->y1)
		{
			if (rect->y2 - rect->y1 > height)
			{
				extra = mem.Alloc();
				extra->x1 = rect->x1;
				extra->y1 = rect->y1 + height;
				extra->x2 = rect->x1 + width;
				extra->y2 = rect->y2;
				extra->extent =
					(extra->y2 - extra->y1) * (extra->x2 - extra->x1);
				extra->next = rect->next;
				rect->next = extra;
			}
			rect->x1 += width;
			rect->extent = (rect->y2 - rect->y1) * (rect->x2 - rect->x1);
		}
		else
		{
			if (rect->x2 - rect->x1 > width)
			{
				extra = mem.Alloc();
				extra->x1 = rect->x1 + width;
				extra->y1 = rect->y1;
				extra->x2 = rect->x2;
				extra->y2 = rect->y1 + height;
				extra->extent =
					(extra->y2 - extra->y1) * (extra->x2 - extra->x1);
				extra->next = rect->next;
				rect->next = extra;
			}
			rect->y1 += height;
			rect->extent = (rect->x2 - rect->x1) * (rect->y2 - rect->y1);
		}
	}
	EmptyRect* Find(int width, int height)
	{
		EmptyRect* found = 0;
		int extent;
		for (EmptyRect* rect = list; rect; rect = rect->next)
		{
			if (rect->x2 - rect->x1 >= width && rect->y2 - rect->y1 >= height &&
				(!found || extent > rect->extent))
			{
				found = rect;
				extent = (rect->x2 - rect->x1) * (rect->y2 - rect->y1);
			}
		}
		return found;
	}
	WMemPak<EmptyRect> mem;
	EmptyRect* list;
};
