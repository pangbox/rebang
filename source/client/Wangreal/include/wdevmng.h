#pragma once

#include <windows.h>
#include "wlist.h"

class WDevice;
class WVideoDev;
class WAudioDev;
class WInputDev;

class WDeviceManager
{
public:
	enum
	{
		VIDEO = 1,
		AUDIO = 2,
		NETWORK = 4,
		INPUT = 8
	};

	WDeviceManager(const char* name, int flags);
	~WDeviceManager();
	void SetHWND(HWND window) { hwnd = window; }
	WVideoDev* CreateVideoDevice(char* name, char* mode, int flags);
	WInputDev* CreateInputDevice(char* name, char* mode);
	bool ResetVideoDevice(bool windowed, int width, int height, int bpp,
		long style, int fillMode);
	void Release(WDevice* device);

private:
	void LoadModule(const char* name, int flags);
	WList<WVideoDev*> vidList;
	WList<WAudioDev*> audList;
	WList<WInputDev*> inpList;
	WList<HINSTANCE> dllList;
	WList<WDevice*> devList;
	char* EnumVideoDevice();
	char* EnumVideoMode(char* name);
	HWND hwnd;
};
