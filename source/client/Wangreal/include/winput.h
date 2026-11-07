#pragma once
#include <windows.h>
#include "wdevice.h"
#include <mmsystem.h>
class WInputDev : public WDevice
{
public:
	enum Sort
	{
		Axis = 0,
		Button = 1,
		KeyState = 2,
		BufState = 3,
		ImeCompState = 4,
		ImeCandWinState = 5,
		ImeCompDoneState = 6,
		ImeOpenState = 7,
		ImeAlphaNumericState = 8,
		ImeCandListState = 9,
		ImeConvState = 10,
		ImePropAtCaret = 11,
		ImeInitAfterWndShown = 12,
		ImeTermKeyMapSharing = 13,
	};
	enum Axis_
	{
		X = 1,
		Y = 2,
		Z = 3
	};

	WInputDev()
	{
		active = true;
		bUpdated = false;
	}
	virtual ~WInputDev() { }
	virtual bool InitDevice(HWND, bool) { return false; }
	virtual void SetActive(bool stat) { active = stat; }
	virtual void Reset() { }
	virtual void Update(unsigned long) { }
	virtual int GetState(int, int) { return 0; }
	virtual unsigned long GetEventTime(int) { return 0; }
	virtual bool IsAlphaNumericMode() { return true; }
	virtual void SetAlphaNumericMode(bool) { }
	virtual bool IsUpdated() { return bUpdated; }
	virtual unsigned long GetLastInputTime() { return dwLastInputTime; }
	virtual void ResetInputTime() { dwLastInputTime = timeGetTime(); }
	virtual void SetState(int, int) { }
	virtual void SetAlphaNumericMode() { }
	virtual void SetOpenStatus(bool) { }
	virtual WInputDev* MakeClone(char*, HWND) { return 0; }

protected:
	bool active;
	bool bUpdated;
	unsigned long dwLastInputTime;
};
