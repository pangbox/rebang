#include "winput.h"
#include "wime.h"

#include <string.h>

#include <mmsystem.h>

char* WIme::GetTextComp()
{
	return this->m_TextComp;
}

int WIme::IME_Enter()
{
	if (this->m_hWnd)
	{
		this->m_hIMC = ImmGetContext(this->m_hWnd);
		return this->m_hIMC != NULL;
	}
	return 0;
}

void WIme::Leave()
{
	ImmReleaseContext(this->m_hWnd, this->m_hIMC);
}

int WIme::Check()
{
	return !(this->m_property & IME_PROP_SPECIAL_UI) &&
		(this->m_property & IME_PROP_AT_CARET);
}

WIme::WIme()
{
}

WIme::WIme(HWND hwnd_)
{
	this->m_hWnd = NULL;
	this->m_bActive = false;
	this->m_hKeyLayout = NULL;
	this->m_nState = 0;
	this->m_nCompLen = 0;
	this->m_property = 0;
	this->m_charWidth = 0;
	this->m_charHeight = 0;
	this->m_hIMC = NULL;
	this->m_dwConvMode = 0;
	memset(this->m_TextComp, 0, sizeof(this->m_TextComp));
	this->old_current = 0;
	this->current = 0;
	this->pos = 0;
	this->bUpdated = false;
	this->dwLastInputTime = timeGetTime();
	InitIme(hwnd_);
}

WIme::~WIme()
{
}

void WIme::InitIme(HWND hwnd_)
{
	if (hwnd_)
	{
		this->m_hWnd = hwnd_;
	}

	this->m_hKeyLayout = GetKeyboardLayout(0);
	this->m_property = ImmGetProperty(this->m_hKeyLayout, 4u);
	this->ClearData();
}

void WIme::ClearData()
{
	for (int i = 0; i < 32; i++)
	{
		this->m_hwndCand[i] = NULL;
		this->m_candList[i] = NULL;
	}

	this->m_nState = 0;
	this->m_nCompLen = 0;
	this->old_current = 0;
	this->current = 0;
	this->pos = 0;
}

int WIme::WinProc(UINT msg, unsigned long wparam, unsigned long lparam)
{
	if (!m_bActive)
		return 0;
	switch (msg)
	{
	case WM_IME_STARTCOMPOSITION:
		OnStartComposition(wparam, lparam);
		return 1;
	case WM_IME_ENDCOMPOSITION:
		OnEndComposition(wparam, lparam);
		return 1;
	case WM_IME_COMPOSITION:
		OnComposition(wparam, lparam);
		return 1;
	case WM_IME_SETCONTEXT:
		OnSetContext(wparam, lparam);
		return 1;
	case WM_IME_NOTIFY:
		return OnNotify(wparam, lparam);
	case WM_INPUTLANGCHANGE:
		OnInputLangChange(wparam, lparam);
		break;
	case WM_IME_CONTROL:
		OnControl(wparam, lparam);
		return 1;
	case WM_IME_COMPOSITIONFULL:
		OnCompositionFull(wparam, lparam);
		return 1;
	case WM_KEYDOWN:
		if (LOWORD(wparam) >= 0x21 && LOWORD(wparam) <= 0x2E)
		{
			OnChar(wparam | 0x80);
		}
		return 1;
	case WM_CHAR:
		OnChar(wparam);
		break;
	}
	return 0;
}

void WIme::OnStartComposition(unsigned int dwCommand, long dwData)
{
	if (Check())
	{
		m_nCompLen = 0;
		m_nState |= 1;
		current = 0;
		pos = 0;
	}
}

void WIme::OnInputLangChange(unsigned int dwCommand, long dwData)
{
	int i;
	CANDIDATEFORM Candidate;

	if (ImmIsIME(this->m_hKeyLayout) && this->m_property & IME_PROP_AT_CARET)
	{
		this->ClearData();
	}

	InitIme(NULL);
	if (IME_Enter())
	{
		{
			i = 0;
			do
			{
				if (!(this->m_property & IME_PROP_AT_CARET) &&
					ImmGetCandidateWindow(this->m_hIMC, i, &Candidate))
				{
					if (Candidate.dwStyle)
					{
						Candidate.dwStyle = 0;
						ImmSetCandidateWindow(this->m_hIMC, &Candidate);
					}
				}
				++i;
			} while (i < 32);
			ImmReleaseContext(this->m_hWnd, this->m_hIMC);
		}
	}
}

void WIme::OnSetContext(unsigned int dwCommand, long dwData)
{
	if (this->m_property & IME_PROP_AT_CARET)
	{
		dwData &= ~0x80000001;
	}

	DefWindowProcA(this->m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}

void WIme::OnComposition(unsigned int dwCommand, long dwData)
{
	if (Check())
	{
		if (dwData & GCS_RESULTSTR)
		{
			this->GetResultString();
		}
		else if (dwData & GCS_COMPSTR)
		{
			this->GetCompString(dwData);
		}
	}
}

void WIme::OnEndComposition(unsigned int dwCommand, long dwData)
{
	if (Check())
	{
		this->m_nState &= ~1;
		this->m_nCompLen = 0;
		memset(this->m_TextComp, 0, sizeof(this->m_TextComp));
	}
}

void WIme::OnCompositionFull(unsigned int dwCommand, long dwData)
{
}

int WIme::OnNotify(unsigned int dwCommand, long dwData)
{
	switch (dwCommand)
	{
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 8:
		DWORD dwSenMode;
		IME_Enter();
		ImmGetConversionStatus(m_hIMC, &m_dwConvMode, &dwSenMode);
		Leave();
		break;
	default:
		return 0;
	}
	return 1;
}

void WIme::OnControl(unsigned int dwCommand, long dwData)
{
}

void WIme::OnChar(unsigned int nChar)
{
	switch (nChar)
	{
	case '\b':
		this->ringBuf[this->current++ & 0x3FF] = '\b';
		break;
	case '\t':
		this->ringBuf[this->current++ & 0x3FF] = '\t';
		break;
	case '\r':
		this->ringBuf[this->current++ & 0x3FF] = '\r';
		break;
	case 0x1B:
		this->ringBuf[this->current++ & 0x3FF] = 0x1B;
		break;
	case 0xA1:
		this->ringBuf[this->current++ & 0x3FF] = 2;
		break;
	case 0xA2:
		this->ringBuf[this->current++ & 0x3FF] = 3;
		break;
	case 0xA3:
		this->ringBuf[this->current++ & 0x3FF] = 0x1A;
		break;
	case 0xA4:
		this->ringBuf[this->current++ & 0x3FF] = 0x0C;
		break;
	case 0xA5:
		this->ringBuf[this->current++ & 0x3FF] = 0x04;
		break;
	case 0xA7:
		this->ringBuf[this->current++ & 0x3FF] = 0x05;
		break;
	case 0xA6:
		this->ringBuf[this->current++ & 0x3FF] = 0x06;
		return;
	case 0xA8u:
		this->ringBuf[this->current++ & 0x3FF] = 0x07;
		return;
	case 0xAEu:
		this->ringBuf[this->current++ & 0x3FF] = 0x01;
		return;
	default:
		break;
	}

	if (nChar >= 0x20 && nChar <= 0x7E)
	{
		char str[2];
		str[0] = nChar;
		str[1] = 0;
		this->PutString(str);
	}
}

void WIme::PutString(char* str)
{
	int len = strlen(str);
	for (int i = 0; i < len; ++i)
	{
		this->ringBuf[this->current++ & 0x3FF] = str[i];
	}
}

int WIme::GetResultString()
{
	static char str[128];
	DWORD dwBuf;
	if (IME_Enter())
	{
		dwBuf = ImmGetCompositionStringA(m_hIMC, GCS_RESULTSTR, NULL, 0);
		if (dwBuf > 0)
		{
			if (dwBuf > 127)
				dwBuf = 127;
			ImmGetCompositionStringA(m_hIMC, GCS_RESULTSTR, str, dwBuf);
			str[dwBuf] = 0;
			ProcessResultString(str);
		}
		Leave();
	}
	return 1;
}

int WIme::GetCompString(long flag)
{
	static char str[256];
	static char strAttr[256];

	DWORD dwBuf;
	DWORD dwAttr;

	if (!IME_Enter())
	{
		return 0;
	}

	dwBuf = ImmGetCompositionStringA(this->m_hIMC, GCS_COMPSTR, NULL, 0);
	if (dwBuf > 0)
	{
		ImmGetCompositionStringA(this->m_hIMC, GCS_COMPSTR, str, dwBuf);
		str[dwBuf] = 0;
		if (flag & GCS_COMPATTR)
		{
			dwAttr =
				ImmGetCompositionStringA(this->m_hIMC, GCS_COMPATTR, NULL, 0);
			if (dwAttr > 0)
			{
				ImmGetCompositionStringA(this->m_hIMC, GCS_COMPATTR, strAttr,
					dwAttr);
				strAttr[dwAttr] = 0;
			}
		}
		this->ProcessCompString(str, strAttr);
		this->m_nCompLen = dwBuf;
	}
	ImmReleaseContext(this->m_hWnd, this->m_hIMC);

	return 1;
}

void WIme::ProcessCompString(char* str, char* strattr)
{
	if (lstrlenA(str) <= 2)
	{
		strcpy(this->m_TextComp, str);
	}
}

void WIme::ProcessResultString(char* str)
{
	this->PutString(str);
}

WInputDev* WIme::MakeClone(char* modeName, HWND hwnd)
{
	return new WIme(hwnd);
}

int WIme::GetState(int sort, int n)
{
	if (sort == 3)
	{
		if (n == -1)
			return 1;
		if (n == -2)
		{
			int code = 0;
			if (Check())
				strcpy((char*)&code, m_TextComp);
			return code;
		}
		if (pos < current)
		{
			if (ringBuf[pos & 1023] >= 128)
			{
				int code = ringBuf[pos++ & 1023];
				code |= ringBuf[pos++ & 1023] << 8;
				return code;
			}
			return ringBuf[pos++ & 1023];
		}
	}
	return 0;
}

void WIme::SetAlphaNumericMode(bool alnum)
{
	DWORD dwSenMode;

	if (!this->m_hIMC)
	{
		IME_Enter();

		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(this->m_hIMC, alnum == 0, dwSenMode);
		ImmReleaseContext(this->m_hWnd, this->m_hIMC);
	}
	else
	{
		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(this->m_hIMC, alnum == 0, dwSenMode);
	}
}

bool WIme::IsAlphaNumericMode()
{
	DWORD dwSenMode;

	if (!this->m_hIMC)
	{
		IME_Enter();

		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmReleaseContext(this->m_hWnd, this->m_hIMC);
	}

	return this->m_dwConvMode == 0;
}

void WIme::Reset()
{
	this->SetActive(true);
	ImmNotifyIME(this->m_hIMC, NI_COMPOSITIONSTR, 4, 0);
	this->ClearData();
	memset(this->m_TextComp, 0, sizeof(this->m_TextComp));
}

void WIme::Update(unsigned long timeStamp)
{
	if (current != old_current)
	{
		old_current = current;
		bUpdated = true;
		dwLastInputTime = timeGetTime();
	}
	else
	{
		bUpdated = false;
	}
}

#include "wproc.inl"
