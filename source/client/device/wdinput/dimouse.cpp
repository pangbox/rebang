#include "winput.h"
#include "dimouse.h"
#include <windows.h>
#include <stddef.h>

DirectInputMouse::DirectInputMouse(IDirectInput8A* di, HWND hWnd)
{
	this->diBackup = di;
	this->hEvent = NULL;
	this->active = true;
	this->diMouse = NULL;
	this->m_moveMode = false;
	this->m_myHwnd = hWnd;
	this->bufCur = 0;
	this->passCur = 0;
	this->z = 0;
	this->y = 0;
	this->x = 0;
	this->b[0] = 0;
	this->b[1] = 0;
	this->b[2] = 0;
	this->b[3] = 0;
	ZeroMemory(this->timebuf, sizeof(this->timebuf));
	this->bUpdated = false;
	this->dwLastInputTime = timeGetTime();
	this->DirectInputMouse::InitDevice(hWnd, true);
}

DirectInputMouse::~DirectInputMouse()
{
	if (this->hEvent)
	{
		CloseHandle(this->hEvent);
		this->hEvent = NULL;
	}
	if (this->diMouse)
	{
		this->diMouse->Unacquire();
		this->diMouse->Release();
		this->diMouse = NULL;
	}
}

bool DirectInputMouse::InitDevice(HWND hWnd, bool exclusive)
{
	DIPROPDWORD diProp;

	if (this->diMouse)
	{
		this->diMouse->Unacquire();
		this->diMouse->Release();
		this->diMouse = NULL;
	}

	if ((this->diBackup->CreateDevice(GUID_SysMouse, &this->diMouse, NULL)) !=
		S_OK)
	{
		return false;
	}

	if ((this->diMouse->SetDataFormat(&c_dfDIMouse)) != S_OK)
	{
		return false;
	}

	DWORD flags =
		DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE);
	if ((this->diMouse->SetCooperativeLevel(hWnd, flags)) != S_OK)
	{
		return false;
	}

	if (this->hEvent)
	{
		CloseHandle(this->hEvent);
		this->hEvent = NULL;
	}

	this->hEvent = CreateEventA(NULL, 0, 0, NULL);
	if (!this->hEvent)
	{
		return false;
	}

	if ((this->diMouse->SetEventNotification(this->hEvent)) != S_OK)
	{
		return false;
	}

	diProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diProp.dwData = 16;
	diProp.diph.dwSize = sizeof(DIPROPDWORD);
	diProp.diph.dwHow = 0;
	diProp.diph.dwObj = 0;

	if ((this->diMouse->SetProperty(DIPROP_BUFFERSIZE,
			reinterpret_cast<LPCDIPROPHEADER>(&diProp))) != S_OK)
	{
		if (this->diMouse)
		{
			this->diMouse->Release();
			this->diMouse = NULL;
		}
		return false;
	}

	this->passCur = 0;
	this->bufCur = 0;
	return true;
}

void DirectInputMouse::GetDeviceData()
{
	HRESULT result;
	DIDEVICEOBJECTDATA dims[16];
	DWORD dwItems;
	Acquire();
	do
	{
		dwItems = 16;
		result = diMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dims,
			&dwItems, 0);
		if (result != DI_OK && result != DI_BUFFEROVERFLOW)
			break;
		for (DWORD i = 0; i < dwItems;)
		{
			DWORD n = dwItems;
			if (n >= 256 - (bufCur & 255))
				n = 256 - (bufCur & 255);
			memcpy(&mousebuf[bufCur & 255], &dims[i],
				sizeof(DIDEVICEOBJECTDATA) * n);
			bufCur += n;
			i += n;
		}
	} while (result == DI_BUFFEROVERFLOW);
}

unsigned long DirectInputMouse::GetEventTime(int n)
{
	return this->timebuf[n];
}

void DirectInputMouse::FlushBuffer(DWORD timeStamp)
{
	this->bUpdated = true;
	this->dwLastInputTime = timeGetTime();
	for (; this->passCur < this->bufCur; this->passCur += 1)
	{
		if (timeStamp < this->mousebuf[this->passCur & 0xFF].dwTimeStamp)
		{
			break;
		}

		switch (this->mousebuf[this->passCur & 0xFF].dwOfs)
		{
		case DIMOFS_X:
			this->x += this->mousebuf[this->passCur & 0xFF].dwData;
			break;
		case DIMOFS_Y:
			this->y += this->mousebuf[this->passCur & 0xFF].dwData;
			break;
		case DIMOFS_Z:
			this->z += this->mousebuf[this->passCur & 0xFF].dwData;
			break;
		case DIMOFS_BUTTON0:
			this->b[0] =
				((this->mousebuf[this->passCur & 0xFF].dwData & 0x80) != 0);
			if (this->b[0])
			{
				this->timebuf[0] =
					this->mousebuf[this->passCur & 0xFF].dwTimeStamp;
			}
			break;
		case DIMOFS_BUTTON1:
			this->b[1] =
				((this->mousebuf[this->passCur & 0xFF].dwData & 0x80) != 0);
			if (this->b[1])
			{
				this->timebuf[1] =
					this->mousebuf[this->passCur & 0xFF].dwTimeStamp;
			}
			break;
		case DIMOFS_BUTTON2:
			this->b[2] =
				((this->mousebuf[this->passCur & 0xFF].dwData & 0x80) != 0);
			if (this->b[2])
			{
				this->timebuf[2] =
					this->mousebuf[this->passCur & 0xFF].dwTimeStamp;
			}
			break;
		case DIMOFS_BUTTON3:
			this->b[3] =
				((this->mousebuf[this->passCur & 0xFF].dwData & 0x80) != 0);
			if (this->b[3])
			{
				this->timebuf[3] =
					this->mousebuf[this->passCur & 0xFF].dwTimeStamp;
			}
			break;
		default:
			break;
		}
	}
	if (this->bufCur == this->passCur)
	{
		this->passCur = 0;
		this->bufCur = 0;
	}
}

void DirectInputMouse::Update(unsigned long timeStamp)
{
	POINT curPt;
	DIMOUSESTATE dims;

	this->bUpdated = false;
	this->z = 0;
	this->y = 0;
	this->x = 0;

	if (timeStamp && this->active)
	{
		if (this->diMouse)
		{
			int i = 0;
			do
			{
				++i;
				if (this->diMouse->GetDeviceState(sizeof(dims), &dims) == S_OK)
				{
					break;
				}
				ZeroMemory(&dims, sizeof(dims));
				if (this->m_moveMode == 0)
				{
					this->diMouse->Acquire();
				}
				++i;
			} while (i < 5);
			this->GetDeviceData();
			if (this->bufCur - this->passCur > 0)
			{
				this->FlushBuffer(timeStamp);
			}
		}
		if (this->m_moveMode)
		{
			GetCursorPos(&curPt);
			POINT pos = { curPt.x - m_clientX, curPt.y - m_clientY };
			MoveWindow(m_myHwnd, pos.x, pos.y, m_clientWidth, m_clientHeight,
				TRUE);
		}
	}
	else
	{
		this->b[0] = 0;
		this->b[1] = 0;
		this->b[2] = 0;
		this->b[3] = 0;
	}
}

void DirectInputMouse::ClearBuffer()
{
	this->bufCur = 0;
	this->passCur = 0;
	this->z = 0;
	this->y = 0;
	this->x = 0;
	this->b[0] = 0;
	this->b[1] = 0;
	this->b[2] = 0;
	this->b[3] = 0;
}

int DirectInputMouse::WinProc(UINT msg, unsigned long wParam,
	unsigned long lParam)
{
	RECT clientRect;
	POINT pt;

	switch (msg)
	{
	case WM_LBUTTONUP:
		if (!this->m_moveMode)
		{
			break;
		}
	case WM_NCLBUTTONUP:
		this->m_moveMode = false;
		break;

	case WM_NCLBUTTONDOWN:
		if (wParam == 2)
		{
			this->m_moveMode = true;
			GetCursorPos(&pt);
			GetWindowRect(this->m_myHwnd, &clientRect);
			this->m_clientX = pt.x - clientRect.left;
			this->m_clientY = pt.y - clientRect.top;
			this->m_clientWidth = clientRect.right - clientRect.left;
			this->m_clientHeight = clientRect.bottom - clientRect.top;
		}
		break;

	case WM_ACTIVATE:
		if (wParam)
		{
			this->Acquire();
			this->GetDeviceData();
			this->ClearBuffer();
		}
		else
		{
			this->diMouse->Unacquire();
			this->m_moveMode = false;
			this->ClearBuffer();
		}
		return 1;

	default:
		break;
	}

	return 0;
}

int DirectInputMouse::GetState(int sort, int n)
{
	switch (sort)
	{
	case 1:
		return this->b[n];
	case 0:
		if (n == 1)
			return x;
		if (n == 2)
			return y;
		if (n == 3)
			return z;
		return 0;
	default:
		return 0;
	}
}

HRESULT DirectInputMouse::Acquire()
{
	if (!this->m_moveMode)
	{
		return this->diMouse->Acquire();
	}
	return 1;
}

#include "wproc.inl"
