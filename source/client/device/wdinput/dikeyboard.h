#pragma once
#include <winput.h>
#include <wproc.h>
#include <dinput.h>

class DirectInputKeyboard : public WInputDev, public WProc
{
public:
	DirectInputKeyboard(IDirectInput8A* di, HWND hWnd);
	virtual ~DirectInputKeyboard();
	virtual bool InitDevice(HWND hWnd, bool exclusive);
	virtual WProc* ExternProc() { return this; }
	virtual int WinProc(unsigned int message, unsigned long wParam,
		unsigned long lParam);
	virtual unsigned long GetEventTime(int n);
	virtual int GetState(int sort, int n);
	virtual void Reset();
	virtual void Update(unsigned long timeStamp);

private:
	HRESULT Acquire();
	void ClearBuffer();
	void FlushBuffer(DWORD timestamp);
	void GetDeviceData();

	IDirectInput8A* diBackup;
	IDirectInputDevice8A* diKey;
	DIDEVICEOBJECTDATA keybuf[256];
	unsigned int timebuf[256];
	int asciiBuf[256];
	int asciiPos;
	int asciiCur;
	int bufCur;
	int passCur;
	char keyState[256];

	struct w_conv_scan_code
	{
		int di_scan;
		int ascii;
		int asciiwithshift;
	};

	static int ascii2scan[256];
	static int scan2ascii_with_shift[256];
	static int scan2ascii_without_shift[256];
	static w_conv_scan_code convList[99];
};
