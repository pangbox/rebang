#include "wmemblock.inl"
#include "wdevmng.h"
#include "wvideo.h"
#include <stdlib.h>
#include <string.h>

typedef void*(__cdecl* WEnumDeviceProc)(int);

bool WDeviceManager::ResetVideoDevice(bool windowed, int width, int height,
	int bpp, long refresh, int flags)
{
	for (WVideoDev* video = vidList.Start(); video; video = vidList.Next())
	{
		for (WDevice* device = devList.Start(); device; device = devList.Next())
		{
			if (strstr(device->GetDeviceName(), video->GetDeviceName()) ||
				strstr(video->GetDeviceName(), device->GetDeviceName()))
				return ((WVideoDev*)device)
					->Reset(windowed, width, height, bpp, refresh, flags);
		}
	}
	return false;
}

char* WDeviceManager::EnumVideoDevice(void)
{
	static char text[512];

	int offset;
	WVideoDev* video;
	for (video = vidList.Start(), offset = 0; video; video = vidList.Next())
	{
		strcpy(text + offset, video->GetDeviceName());
		offset += (int)strlen(video->GetDeviceName()) + 1;
	}
	text[offset] = '\0';
	return text;
}

char* WDeviceManager::EnumVideoMode(char* name)
{
	for (WVideoDev* video = vidList.Start(); video; video = vidList.Next())
	{
		if (!strcmpi(video->GetDeviceName(), name))
			return video->EnumModeName();
	}
	return 0;
}

void WDeviceManager::LoadModule(const char* name, int flags)
{
	char drive[4];
	WIN32_FIND_DATAA find;
	char dir[260];
	char path[260];
	char fname[128];
	char module[128];
	char temp[64];
	char search[128];
	char ext[8];

	GetModuleFileNameA(0, path, 260);
	_splitpath(path, drive, dir, fname, ext);
	strcpy(search, "wangreal");
	char* mark = strrchr(fname, '_');
	if (mark)
	{
		strcpy(temp, mark);
		strcat(search, temp);
	}
	strcat(search, ".dll");

	HANDLE handle = FindFirstFileA(search, &find);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strstr(find.cFileName, "wang"))
			{
				module[0] = '\0';
				strcat(module, find.cFileName);
				HINSTANCE library = LoadLibraryA(module);
				if (library)
				{
					WEnumDeviceProc enum_video = (flags & 1)
						? (WEnumDeviceProc)GetProcAddress(library,
							  "WEnumVideoDevices")
						: 0;
					WEnumDeviceProc enum_input = (flags & 8)
						? (WEnumDeviceProc)GetProcAddress(library,
							  "WEnumInputDevices")
						: 0;
					GetProcAddress(library, "WVersion");
					if (!enum_video && !enum_input)
						FreeLibrary(library);
					else
					{
						int index = 0;
						if (enum_video)
						{
							WVideoDev* video;
							do
							{
								video = (WVideoDev*)enum_video(index);
								if (video)
								{
									vidList += video;
									++index;
								}
							} while (video);
						}
						index = 0;
						if (enum_input)
						{
							WInputDev* input;
							do
							{
								input = (WInputDev*)enum_input(index);
								if (input)
								{
									inpList += input;
									++index;
								}
							} while (input);
						}
						dllList += library;
					}
				}
			}
		} while (FindNextFileA(handle, &find));
	}
	FindClose(handle);
}

WVideoDev* WDeviceManager::CreateVideoDevice(char* name, char* mode, int flags)
{
	WVideoDev* created = 0;
	for (WVideoDev* video = vidList.Start(); video; video = vidList.Next())
	{
		if (created)
			break;
		if (name && strcmpi(video->GetDeviceName(), name))
			continue;
		created = video->MakeClone(mode, hwnd, flags);
	}
	if (created)
		devList += (WDevice*)created;
	return created;
}

WInputDev* WDeviceManager::CreateInputDevice(char* name, char* mode)
{
	WInputDev* created = 0;
	for (WInputDev* input = inpList.Start(); input; input = inpList.Next())
	{
		if (created)
			break;
		if (name && strcmpi(input->GetDeviceName(), name))
			continue;
		created = input->MakeClone(mode, hwnd);
	}
	if (created)
		devList += (WDevice*)created;
	return created;
}

void WDeviceManager::Release(WDevice* device)
{
	devList -= device;
	delete device;
}

WDeviceManager::WDeviceManager(const char* name, int flags)
{
	LoadModule(name, flags);
}

WDeviceManager::~WDeviceManager()
{
	for (;;)
	{
		WDevice* device = devList.Start();
		if (!device)
			break;
		Release(device);
	}

	for (HINSTANCE library = dllList.Start(); library; library = dllList.Next())
		FreeLibrary(library);
}
