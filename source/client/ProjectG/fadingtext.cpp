#include "fresh.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "frgraphicinterface.h"
#include "fadingtext.h"

extern Fresh* g_pFresh;
CFadingText::CFadingText()
{
	m_time = 0;
	m_pWnd = NULL;
	m_x = 0;
	m_y = 0;
	m_color = 0xff000000;
	m_type = FADE_OUT;

	m_bActive = false;
}

CFadingText::~CFadingText()
{
}

void CFadingText::Print(const char* text, float time, FrWnd* pWnd, float x,
	float y, eFadeType type, unsigned long color)
{
	strncpy(m_text, text, 512);
	m_time = time;
	m_pWnd = pWnd;
	m_x = x;
	m_y = y;
	m_color = color;
	m_type = type;
	m_elapsed = 0;

	if (type == FADE_OUT)
	{
		m_alpha = 255;
		m_dir = -1;
	}
	else if (type == FADE_IN)
	{
		m_alpha = 0;
		m_dir = 1;
	}
	else if (type == FADE_INOUT)
	{
		m_alpha = 0;
		m_dir = 2;
	}

	m_bActive = true;
}

void CFadingText::Process()
{
	float elapsed = 0;
	static DWORD s_prevTick = 0;
	DWORD tick = GetTickCount();
	if (!s_prevTick)
		s_prevTick = tick;
	DWORD diff = tick - s_prevTick;
	if (diff > 50)
	{
		elapsed = diff * 0.001f;
		s_prevTick = tick;
	}

	if (m_type == FADE_OUT || m_type == FADE_INOUT)
	{
		if (m_time < m_elapsed)
			m_bActive = false;
	}

	if (!m_bActive)
		return;

	float x = 0;
	float y = 0;
	if (m_pWnd)
	{
		x = m_pWnd->GetRect().x;
		y = m_pWnd->GetRect().y;
	}

	x += m_x;
	y += m_y;
	int alpha = m_dir > 0 ? 0 : 255;

	switch (m_type)
	{
	case FADE_OUT:
	case FADE_IN:

		alpha += (int)((m_elapsed / m_time * 255.0f) * m_dir);
		break;

	case FADE_INOUT:

		alpha += (int)((m_elapsed / m_time * 255.0f) * m_dir);

		break;
	}
	if (alpha < 0)
		alpha = 0;
	else if (alpha > 255)
		alpha = 255;
	if (alpha == 255 && m_type == FADE_INOUT)
	{
		m_dir = -2;
		m_elapsed = 0;
	}

	unsigned long color = (alpha << 24) + (m_color & 0x00ffffff);

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	gdi->SetTextColor(color, 0xffffffff);
	gdi->Print(WPoint(x, y), 0, m_text);

	m_elapsed += elapsed;
}
