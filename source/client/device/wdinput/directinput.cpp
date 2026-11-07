#include "winput.h"
#include "directinput.h"
#include "dimouse.h"
#include "dikeyboard.h"
#include <string.h>

WDirectInput::WDirectInput()
{
	this->di = NULL;
}

WDirectInput::~WDirectInput()
{
	if (this->di)
	{
		this->di->Release();
		this->di = NULL;
	}
}

char* WDirectInput::GetDeviceName()
{
	return "DirectInput";
}

char* WDirectInput::EnumModeName()
{
	return "Keyboard\0Mouse\0\0";
}

WInputDev* WDirectInput::MakeClone(char* modeName, HWND hWnd)
{
	if (!this->di)
	{
		DirectInput8Create(
			reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)),
			DIRECTINPUT_VERSION, IID_IDirectInput8A,
			reinterpret_cast<void**>(&this->di), NULL);
	}

	if (!strcmpi(modeName, "keyboard"))
	{
		return new DirectInputKeyboard(this->di, hWnd);
	}
	if (!strcmpi(modeName, "mouse"))
	{
		return new DirectInputMouse(this->di, hWnd);
	}
	return NULL;
}

#include "wproc.inl"
