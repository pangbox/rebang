#include "winput.h"
#include "dikeyboard.h"

int DirectInputKeyboard::ascii2scan[256];
int DirectInputKeyboard::scan2ascii_with_shift[256];
int DirectInputKeyboard::scan2ascii_without_shift[256];

DirectInputKeyboard::w_conv_scan_code DirectInputKeyboard::convList[99] = {
	{ 0x01, 0x1B, 0x1B },
	{ 0x02, 0x31, 0x21 },
	{ 0x03, 0x32, 0x40 },
	{ 0x04, 0x33, 0x23 },
	{ 0x05, 0x34, 0x24 },
	{ 0x06, 0x35, 0x25 },
	{ 0x07, 0x36, 0x5E },
	{ 0x08, 0x37, 0x26 },
	{ 0x09, 0x38, 0x2A },
	{ 0x0A, 0x39, 0x28 },
	{ 0x0B, 0x30, 0x29 },
	{ 0x0C, 0x2D, 0x5F },
	{ 0x0D, 0x3D, 0x2B },
	{ 0x0E, 0x08, 0x08 },
	{ 0x0F, 0x09, 0x09 },
	{ 0x10, 0x51, 0x51 },
	{ 0x11, 0x57, 0x57 },
	{ 0x12, 0x45, 0x45 },
	{ 0x13, 0x52, 0x52 },
	{ 0x14, 0x54, 0x54 },
	{ 0x15, 0x59, 0x59 },
	{ 0x16, 0x55, 0x55 },
	{ 0x17, 0x49, 0x49 },
	{ 0x18, 0x4F, 0x4F },
	{ 0x19, 0x50, 0x50 },
	{ 0x1A, 0x5B, 0x7B },
	{ 0x1B, 0x5D, 0x7D },
	{ 0x1C, 0x0D, 0x0D },
	{ 0x1D, 0x1E, 0x1E },
	{ 0x1E, 0x41, 0x41 },
	{ 0x1F, 0x53, 0x53 },
	{ 0x20, 0x44, 0x44 },
	{ 0x21, 0x46, 0x46 },
	{ 0x22, 0x47, 0x47 },
	{ 0x23, 0x48, 0x48 },
	{ 0x24, 0x4A, 0x4A },
	{ 0x25, 0x4B, 0x4B },
	{ 0x26, 0x4C, 0x4C },
	{ 0x27, 0x3B, 0x3A },
	{ 0x28, 0x27, 0x22 },
	{ 0x29, 0x60, 0x7E },
	{ 0x2A, 0x81, 0x81 },
	{ 0x2B, 0x5C, 0x7C },
	{ 0x2C, 0x5A, 0x5A },
	{ 0x2D, 0x58, 0x58 },
	{ 0x2E, 0x43, 0x43 },
	{ 0x2F, 0x56, 0x56 },
	{ 0x30, 0x42, 0x42 },
	{ 0x31, 0x4E, 0x4E },
	{ 0x32, 0x4D, 0x4D },
	{ 0x33, 0x2C, 0x3C },
	{ 0x34, 0x2E, 0x3E },
	{ 0x35, 0x2F, 0x3F },
	{ 0x36, 0x82, 0x82 },
	{ 0x38, 0x1C, 0x1C },
	{ 0x39, 0x20, 0x20 },
	{ 0x3A, 0x0B, 0x0B },
	{ 0x3B, 0x0E, 0x0E },
	{ 0x3C, 0x0F, 0x0F },
	{ 0x3D, 0x10, 0x10 },
	{ 0x3E, 0x11, 0x11 },
	{ 0x3F, 0x12, 0x12 },
	{ 0x40, 0x13, 0x13 },
	{ 0x41, 0x14, 0x14 },
	{ 0x42, 0x15, 0x15 },
	{ 0x43, 0x16, 0x16 },
	{ 0x44, 0x17, 0x17 },
	{ 0x45, 0x83, 0x83 },
	{ 0x46, 0x0A, 0x0A },
	{ 0x47, 0x85, 0x85 },
	{ 0x48, 0x86, 0x86 },
	{ 0x49, 0x87, 0x87 },
	{ 0x4A, 0x88, 0x88 },
	{ 0x4B, 0x89, 0x89 },
	{ 0x4C, 0x8A, 0x8A },
	{ 0x4D, 0x8B, 0x8B },
	{ 0x4E, 0x8C, 0x8C },
	{ 0x4F, 0x8D, 0x8D },
	{ 0x50, 0x8E, 0x8E },
	{ 0x51, 0x8F, 0x8F },
	{ 0x52, 0x90, 0x90 },
	{ 0x57, 0x18, 0x18 },
	{ 0x58, 0x19, 0x19 },
	{ 0x66, 0x66, 0x66 },
	{ 0x9C, 0x91, 0x91 },
	{ 0x9D, 0x1F, 0x1F },
	{ 0xB7, 0x84, 0x84 },
	{ 0xB8, 0x1D, 0x1D },
	{ 0xC7, 0x0C, 0x0C },
	{ 0xC8, 0x06, 0x06 },
	{ 0xC9, 0x02, 0x02 },
	{ 0xCB, 0x04, 0x04 },
	{ 0xCD, 0x05, 0x05 },
	{ 0xCF, 0x1A, 0x1A },
	{ 0xD0, 0x07, 0x07 },
	{ 0xD1, 0x03, 0x03 },
	{ 0xD2, 0x7F, 0x7F },
	{ 0xD3, 0x01, 0x01 },
	{ 0x37, 0x92, 0x92 },
};

DirectInputKeyboard::DirectInputKeyboard(IDirectInput8A* di, HWND hWnd)
{
	memset(this->keyState, 0, sizeof this->keyState);
	this->diKey = NULL;
	this->diBackup = di;
	this->DirectInputKeyboard::InitDevice(hWnd, false);
	memset(scan2ascii_with_shift, 0, sizeof scan2ascii_with_shift);
	memset(scan2ascii_without_shift, 0, sizeof scan2ascii_without_shift);
	memset(ascii2scan, 0, sizeof ascii2scan);
	memset(this->timebuf, 0, sizeof this->timebuf);
	this->bufCur = 0;
	this->passCur = 0;
	this->asciiCur = 0;
	this->asciiPos = 0;
	memset(this->asciiBuf, 0, sizeof this->asciiBuf);
	memset(this->keyState, 0, sizeof this->keyState);
	for (unsigned int n = 0; n < sizeof(convList) / sizeof(convList[0]); ++n)
	{
		scan2ascii_with_shift[convList[n].di_scan] = convList[n].asciiwithshift;
		scan2ascii_without_shift[convList[n].di_scan] = convList[n].ascii;
		ascii2scan[convList[n].ascii] = convList[n].di_scan;
	}
}

DirectInputKeyboard::~DirectInputKeyboard()
{
	if (this->diKey)
	{
		this->diKey->Unacquire();
		this->diKey->Release();
		this->diKey = NULL;
	}
}

bool DirectInputKeyboard::InitDevice(HWND hWnd, bool exclusive)
{
	DIPROPDWORD prop;

	if (!this->diBackup)
	{
		return false;
	}

	if (this->diKey)
	{
		this->diKey->Unacquire();
		this->diKey->Release();
		this->diKey = NULL;
	}

	if (this->diBackup->CreateDevice(GUID_SysKeyboard, &this->diKey, NULL) !=
		S_OK)
	{
		return false;
	}

	if (this->diKey->SetDataFormat(&c_dfDIKeyboard) != S_OK)
	{
		return false;
	}

	DWORD flags =
		DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE);
	if (this->diKey->SetCooperativeLevel(hWnd, flags) != S_OK)
	{
		return false;
	}

	prop.diph.dwHow = 0;
	prop.diph.dwObj = 0;
	prop.diph.dwSize = sizeof(DIPROPDWORD);
	prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	prop.dwData = 64;

	if (this->diKey->SetProperty(DIPROP_BUFFERSIZE,
			reinterpret_cast<LPCDIPROPHEADER>(&prop)) != S_OK)
	{
		return false;
	}

	if (this->Acquire() != S_OK)
	{
		return false;
	}

	return true;
}

void DirectInputKeyboard::ClearBuffer()
{
	this->bufCur = 0;
	this->passCur = 0;
	this->asciiCur = 0;
	this->asciiPos = 0;
	memset(this->asciiBuf, 0, sizeof this->asciiBuf);
	memset(this->keyState, 0, sizeof this->keyState);
}

int DirectInputKeyboard::WinProc(UINT message, unsigned long wParam,
	unsigned long lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	if (message != WM_ACTIVATE)
	{
		return 0;
	}
	if (wParam)
	{
		this->Acquire();
		this->GetDeviceData();
	}
	else
	{
		this->diKey->Unacquire();
	}
	this->bufCur = 0;
	this->passCur = 0;
	this->asciiCur = 0;
	this->asciiPos = 0;
	memset(this->asciiBuf, 0, sizeof this->asciiBuf);
	memset(this->keyState, 0, sizeof this->keyState);
	return 1;
}

void DirectInputKeyboard::FlushBuffer(DWORD timeStamp)
{
	this->Reset();
	for (; this->passCur < this->bufCur; ++this->passCur)
	{
		if (timeStamp < this->keybuf[this->passCur & 0xFF].dwTimeStamp)
		{
			break;
		}
		DIDEVICEOBJECTDATA& buf = this->keybuf[this->passCur & 0xFF];
		int shiftedScanCode =
			this->keyState[DIK_LSHIFT] || this->keyState[DIK_RSHIFT]
			? scan2ascii_with_shift[buf.dwOfs]
			: scan2ascii_without_shift[buf.dwOfs];
		if (shiftedScanCode && this->keyState[ascii2scan[shiftedScanCode]])
		{
			this->asciiBuf[this->asciiPos++ & 0xFF] = shiftedScanCode;
			this->timebuf[shiftedScanCode] = buf.dwTimeStamp;
		}
	}
	if (this->bufCur == this->passCur)
	{
		this->passCur = 0;
		this->bufCur = 0;
	}
}

void DirectInputKeyboard::GetDeviceData()
{
	HRESULT result;
	DIDEVICEOBJECTDATA dims[64];
	DWORD dwItems;
	Acquire();
	do
	{
		dwItems = 64;
		result =
			diKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dims, &dwItems, 0);
		if (result != DI_OK && result != DI_BUFFEROVERFLOW)
			break;
		for (DWORD i = 0; i < dwItems;)
		{
			DWORD n = dwItems;
			if (n >= 256 - (bufCur & 255))
				n = 256 - (bufCur & 255);
			memcpy(&keybuf[bufCur & 255], &dims[i],
				sizeof(DIDEVICEOBJECTDATA) * n);
			bufCur += n;
			i += n;
		}
	} while (result == DI_BUFFEROVERFLOW);
}

void DirectInputKeyboard::Update(unsigned long timeStamp)
{
	if (timeStamp && this->active)
	{
		if (this->diKey)
		{
			for (int i = 0; i < 5; i++)
			{
				if (this->diKey->GetDeviceState(256, this->keyState) !=
					DIERR_NOTACQUIRED)
				{
					break;
				}
				memset(this->keyState, 0, 0x100);
				this->diKey->Acquire();
			}
			this->GetDeviceData();
			if (this->bufCur - this->passCur > 0)
			{
				this->FlushBuffer(timeStamp);
			}
		}
	}
	else
	{
		memset(this->keyState, 0, sizeof this->keyState);
	}
}

unsigned long DirectInputKeyboard::GetEventTime(int n)
{
	return this->timebuf[n];
}

int DirectInputKeyboard::GetState(int sort, int n)
{
	switch (sort)
	{
	case 2:
		if (n == -1)
			return 1;
		if (keyState)
		{
			if (keyState[ascii2scan[n]])
				return 1;
		}
		return 0;
	case 3:
		if (n == -1)
			return 1;
		for (int i = asciiCur; i < asciiPos; ++i)
		{
			if (asciiBuf[i & 255] == n)
				return 1;
		}
		return 0;
	}
	return 0;
}

void DirectInputKeyboard::Reset()
{
	this->asciiCur = this->asciiPos;
}

HRESULT DirectInputKeyboard::Acquire()
{
	return this->diKey->Acquire();
}

#include "wproc.inl"
