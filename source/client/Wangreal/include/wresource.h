#pragma once
#include <windows.h>
#include "baseobject.h"

class WVideoDev;

class WResourceManager
{
public:
	WVideoDev* video;
	void* audio;
	// missing WList type et al.
	char unknown_008[0x1cc];

	void Release(int handle);
	int UploadTexture(const char* name, class Bitmap* bitmap,
		unsigned long style, RECT* rect);
};

class WResource : public BaseObject
{
public:
	WResource()
		: m_resrcMng(0)
	{
	}

	__forceinline virtual ~WResource() { }

protected:
	__forceinline WResourceManager* GetResrcManager() const
	{
		return m_resrcMng;
	}

private:
	WResourceManager* m_resrcMng;
};
